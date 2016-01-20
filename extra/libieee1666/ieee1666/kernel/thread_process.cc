/*
 *  Copyright (c) 2014,
 *  Commissariat a l'Energie Atomique (CEA)
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without modification,
 *  are permitted provided that the following conditions are met:
 *
 *   - Redistributions of source code must retain the above copyright notice, this
 *     list of conditions and the following disclaimer.
 *
 *   - Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 *   - Neither the name of CEA nor the names of its contributors may be used to
 *     endorse or promote products derived from this software without specific prior
 *     written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 *  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *  DISCLAIMED.
 *  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 *  OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 *  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: Gilles Mouchard (gilles.mouchard@cea.fr)
 */

#include <ieee1666/kernel/thread_process.h>
#include <ieee1666/kernel/kernel.h>
#include <ieee1666/kernel/spawn.h>
#include <ieee1666/kernel/event.h>
#include <ieee1666/util/backtrace.h>
#include <stdexcept>

namespace sc_core {

#if !SC_THREAD_PROCESSES_USE_PTHREADS
sc_thread_process_helper::sc_thread_process_helper(sc_thread_process *_thread_process, sc_coroutine::caller_type& _yield)
	: thread_process(_thread_process)
	, yield(_yield)
{
	thread_process->thread_process_helper = this;
	yield();
	thread_process->call_process_owner_method();
}

sc_thread_process_helper::~sc_thread_process_helper()
{
	thread_process->thread_process_helper = 0;
}
#endif

sc_thread_process::sc_thread_process(const char *_name, sc_process_owner *_process_owner, sc_process_owner_method_ptr _process_owner_method_ptr, bool clocked, const sc_spawn_options *spawn_options)
	: sc_process(_name, _process_owner, _process_owner_method_ptr, clocked ? SC_CTHREAD_PROC_ : SC_THREAD_PROC_, spawn_options)
#if SC_THREAD_PROCESSES_USE_PTHREADS
	, thrd()
	, callee_mutex()
	, caller_mutex()
	, cond_callee()
	, cond_caller()
#else
	, coro(0)
	, thread_process_helper(0)
#endif
	, stack_size(spawn_options ? spawn_options->get_stack_size() : 0)
	, started(false)
	, flag_killed(false)
	, flag_reset(false)
	, flag_is_unwinding(false)
	, flag_throw_it(false)
	, thread_process_terminated(false)
	, thread_process_terminated_event(IEEE1666_KERNEL_PREFIX "_terminated_event")
	, thread_process_reset_event(IEEE1666_KERNEL_PREFIX "_reset_event")
	, wait_type(WAIT_DEFAULT)
	, wait_count(0)
	, wait_event(0)
	, wait_event_list(0)
	, wait_and_event_list_remaining_count(0)
	, wait_time_out_event(IEEE1666_KERNEL_PREFIX "_wait_time_out_event")
{
#if SC_THREAD_PROCESSES_USE_PTHREADS
	pthread_mutex_init(&callee_mutex, NULL);
	pthread_mutex_init(&caller_mutex, NULL);
	pthread_cond_init(&cond_callee, NULL);
	pthread_cond_init(&cond_caller, NULL);
#endif
	kernel->register_thread_process(this);
}

sc_thread_process::~sc_thread_process()
{
	if(started)
	{
		kill();
	}

	kernel->unregister_thread_process(this);

#if SC_THREAD_PROCESSES_USE_PTHREADS
	pthread_mutex_destroy(&callee_mutex);
	pthread_mutex_destroy(&caller_mutex);
	pthread_cond_destroy(&cond_callee);
	pthread_cond_destroy(&cond_caller);
#endif
	
}

void sc_thread_process::set_stack_size(int _stack_size)
{
	stack_size = _stack_size;
}

void sc_thread_process::start()
{
#if SC_THREAD_PROCESSES_USE_PTHREADS
	pthread_mutex_lock(&caller_mutex);

	pthread_attr_t attr;
	pthread_attr_init(&attr);
	if(stack_size)
	{
		pthread_attr_setstacksize(&attr, stack_size);
	}
	pthread_create(&thrd, &attr, &sc_thread_process::thread_work, this);

	started = true;
// 	std::cerr << "kernel waits " << name() << std::endl;
	pthread_cond_wait(&cond_caller, &caller_mutex);
// 	std::cerr << "kernel wakes up" << std::endl;
	pthread_mutex_unlock(&caller_mutex);

#else
	if(stack_size)
	{
		sc_coroutine_attributes coro_attributes = sc_coroutine_attributes(stack_size);
		coro = new sc_coroutine(boost::bind( &sc_thread_process::coroutine_work, this, _1), coro_attributes);
	}
	else
	{
		coro = new sc_coroutine(boost::bind( &sc_thread_process::coroutine_work, this, _1));
	}
	started = true;
#endif
}

void sc_thread_process::yield()
{
#if SC_THREAD_PROCESSES_USE_PTHREADS
	pthread_mutex_lock(&caller_mutex);
// 	std::cerr << name() << " signals kernel" << std::endl;
	pthread_cond_signal(&cond_caller);
	pthread_mutex_lock(&callee_mutex);
	pthread_mutex_unlock(&caller_mutex);
// 	std::cerr << name() << " waits kernel" << std::endl;
	pthread_cond_wait(&cond_callee, &callee_mutex);
// 	std::cerr << name() << " wakes up" << std::endl;
	pthread_mutex_unlock(&callee_mutex);
#else
	assert(thread_process_helper != 0);
	thread_process_helper->yield(); // yield to kernel
#endif
	if(flag_killed)
	{
		flag_killed = false;
		flag_is_unwinding = true;
		throw sc_unwind_exception(false);
	}
	else if(flag_reset)
	{
		flag_reset = true;
		flag_is_unwinding = true;
		throw sc_unwind_exception(true);
	}
	else if(flag_throw_it)
	{
		flag_throw_it = false;
		user_exception->throw_it();
	}
}

void sc_thread_process::trigger_statically()
{
	if(wait_type != WAIT_DEFAULT)
	{
		// call to wait(...) with one or more non-integer arguments overrides static sensitivity
		return;
	}
	
	if(wait_count > 0)
	{
		wait_count--;
		return;
	}
	
	if(enabled)
	{
		if(suspended)
		{
			runnable_on_resuming = true;
		}
		else
		{
			kernel->trigger(this);
		}
	}
}

void sc_thread_process::trigger_dynamically(const sc_event *triggered_event)
{
	bool trigger_cond = false;
	
	if(enabled)
	{
		switch(wait_type)
		{
			case WAIT_DEFAULT: // wait() => static sensitivity
				// should not reach that point
				break;
			case WAIT_EVENT: // wait(e)
				wait_type = WAIT_DEFAULT;
				trigger_cond = true;
				break;
			case WAIT_EVENT_AND_LIST: // wait(e1 & ... & en)
				if(--wait_and_event_list_remaining_count == 0)
				{
					// got all events in the event AND list
					wait_event_list->release();
					wait_type = WAIT_DEFAULT;
					trigger_cond = true;
				}
				break;
			case WAIT_EVENT_OR_LIST: // wait(e1 | ... | en)
				// got one event from the event OR list
				// avoid all events in the event OR list to trigger again the thread process
				wait_event_list->remove_dynamically_sensitive_thread_process(this);
				wait_event_list->release();
				wait_type = WAIT_DEFAULT;
				trigger_cond = true;
				break;
			case WAIT_TIME_OUT: // wait(t)
				wait_type = WAIT_DEFAULT;
				trigger_cond = true;
				break;
			case WAIT_TIME_OUT_OR_EVENT: // wait(t, e)
				if(triggered_event == &wait_time_out_event)
				{
					// got time out event first
					wait_event->remove_dynamically_sensitive_thread_process(this);
				}
				else // triggered_event == wait_event
				{
					// got event first: canceling time out event
					wait_time_out_event.cancel();
					wait_time_out_event.clear_dynamically_sensitive_processes();
				}
				wait_type = WAIT_DEFAULT;
				trigger_cond = true;
				break;

			case WAIT_TIME_OUT_OR_EVENT_AND_LIST: // wait(t, e1 & ... & en)
				if(triggered_event == &wait_time_out_event)
				{
					// got time out event first
					wait_event_list->remove_dynamically_sensitive_thread_process(this);
					wait_type = WAIT_DEFAULT;
					trigger_cond = true;
				}
				else if(--wait_and_event_list_remaining_count == 0)
				{
					// got all events in the event AND list first: canceling time out event
					wait_time_out_event.cancel();
					wait_time_out_event.clear_dynamically_sensitive_processes();
					
					wait_event_list->release();
					wait_type = WAIT_DEFAULT;
					trigger_cond = true;
				}
				break;

			case WAIT_TIME_OUT_OR_EVENT_OR_LIST: // wait(t, e1 | ... \ en)
				if(triggered_event == &wait_time_out_event)
				{
					// got time out event first
					wait_event_list->remove_dynamically_sensitive_thread_process(this);
				}
				else
				{
					// got an event from event OR list first: canceling time out event
					wait_time_out_event.cancel();
					wait_time_out_event.clear_dynamically_sensitive_processes();

					wait_event_list->remove_dynamically_sensitive_thread_process(this);
					wait_event_list->release();
				}
				wait_type = WAIT_DEFAULT;
				trigger_cond = true;
				break;
		}
	}
	
	if(trigger_cond)
	{
		if(suspended)
		{
			runnable_on_resuming = true;
		}
		else
		{
			kernel->trigger(this);
		}
	}
}

void sc_thread_process::wait()
{
	wait_type = WAIT_DEFAULT;
	yield();
}

void sc_thread_process::wait(int n)
{
	if(n <= 0) throw std::runtime_error("wait argument shall be greater than zero");
	
	wait_type = WAIT_DEFAULT;
	wait_count = n - 1;
	yield();
}

void sc_thread_process::wait(const sc_event& e)
{
	wait_type = WAIT_EVENT;
	wait_event = &e;
	wait_event->add_dynamically_sensitive_thread_process(this);
	yield();
}

void sc_thread_process::wait(const sc_event_and_list& el)
{
	wait_type = WAIT_EVENT_AND_LIST;
	wait_event_list = &el;
	wait_event_list->acquire();
	wait_event_list->add_dynamically_sensitive_thread_process(this);
	wait_and_event_list_remaining_count = wait_event_list->size();
	yield();
}

void sc_thread_process::wait(const sc_event_or_list& el)
{
	wait_type = WAIT_EVENT_OR_LIST;
	wait_event_list = &el;
	wait_event_list->acquire();
	wait_event_list->add_dynamically_sensitive_thread_process(this);
	yield();
}

void sc_thread_process::wait(const sc_time& t)
{
	wait_type = WAIT_TIME_OUT;
	wait_time_out = t;
	wait_time_out_event.add_dynamically_sensitive_thread_process(this);
	wait_time_out_event.notify(wait_time_out);
	yield();
}

void sc_thread_process::wait(const sc_time& t, const sc_event& e)
{
	wait_type = WAIT_TIME_OUT_OR_EVENT;
	wait_time_out = t;
	wait_time_out_event.add_dynamically_sensitive_thread_process(this);
	wait_time_out_event.notify(wait_time_out);
	wait_event = &e;
	wait_event->add_dynamically_sensitive_thread_process(this);
	yield();
}

void sc_thread_process::wait(const sc_time& t, const sc_event_and_list& el)
{
	wait_type = WAIT_TIME_OUT_OR_EVENT_AND_LIST;
	wait_time_out = t;
	wait_time_out_event.add_dynamically_sensitive_thread_process(this);
	wait_time_out_event.notify(wait_time_out);
	wait_event_list = &el;
	wait_event_list->acquire();
	wait_event_list->add_dynamically_sensitive_thread_process(this);
	yield();
}

void sc_thread_process::wait(const sc_time& t, const sc_event_or_list& el)
{
	wait_type = WAIT_TIME_OUT_OR_EVENT_OR_LIST;
	wait_time_out = t;
	wait_time_out_event.add_dynamically_sensitive_thread_process(this);
	wait_time_out_event.notify(wait_time_out);
	wait_event_list = &el;
	wait_event_list->acquire();
	wait_event_list->add_dynamically_sensitive_thread_process(this);
	yield();
}

void sc_thread_process::switch_to()
{
#if SC_THREAD_PROCESSES_USE_PTHREADS
	pthread_mutex_lock(&callee_mutex);
// 	std::cerr << "kernel signals " << name() << std::endl;
	pthread_cond_signal(&cond_callee);
	pthread_mutex_lock(&caller_mutex);
	pthread_mutex_unlock(&callee_mutex);
// 	std::cerr << "kernel waits " << name() << std::endl;
	pthread_cond_wait(&cond_caller, &caller_mutex);
// 	std::cerr << "kernel wakes up" << std::endl;
	pthread_mutex_unlock(&caller_mutex);
#else
	if(*coro)
	{
		(*coro)(); // yield to thread
	}
#endif
}

void sc_thread_process::terminate()
{
//	std::cerr << sc_time_stamp() << ":" << name() << ".terminate()" << std::endl;
	thread_process_terminated = true;
	thread_process_terminated_event.notify();
#if SC_THREAD_PROCESSES_USE_PTHREADS
	pthread_mutex_lock(&caller_mutex);
// 	std::cerr << name() << "signals kernel" << std::endl;
	pthread_cond_signal(&cond_caller);
	pthread_mutex_unlock(&caller_mutex);
	
	pthread_exit(NULL);
#endif
	started = false;
	
	kernel->terminate_thread_process(this);
}

#if SC_THREAD_PROCESSES_USE_PTHREADS
void *sc_thread_process::thread_work(void *arg)
{
	sc_thread_process *thread_process = static_cast<sc_thread_process *>(arg);
	
	// SC_THREAD/SC_CTHREAD process method is run once unless process is reset
	while(true)
	{
		try
		{
			thread_process->yield();
			thread_process->call_process_owner_method();
		}
		catch(sc_unwind_exception& exc)
		{
			thread_process->flag_is_unwinding = false;
			if(exc.is_reset()) continue;
		}
		
		break;
	}
	
	thread_process->terminate();
}
#else
void sc_thread_process::coroutine_work(sc_coroutine::caller_type& yield)
{
	// SC_THREAD/SC_CTHREAD process method is run once unless process is reset
	while(true)
	{
		try
		{
			sc_thread_process_helper(this, yield);
		}
		catch(sc_unwind_exception& exc)
		{
			flag_is_unwinding = false;
			if(exc.is_reset()) continue;
		}
		
		break;
	}

	terminate();
}
#endif

bool sc_thread_process::terminated() const
{
	return thread_process_terminated;
}

const sc_event& sc_thread_process::terminated_event() const
{
	return thread_process_terminated_event;
}

bool sc_thread_process::is_unwinding() const
{
	return flag_is_unwinding;
}

const sc_event& sc_thread_process::reset_event() const
{
	return thread_process_reset_event;
}

void sc_thread_process::suspend()
{
	suspended = true;
}

void sc_thread_process::resume()
{
    suspended = false;

	if(runnable_on_resuming)
	{
		runnable_on_resuming = false;
		
		kernel->trigger(this);
	}
}

void sc_thread_process::disable()
{
	enabled = false;
}

void sc_thread_process::enable()
{
	enabled = true;
}

void sc_thread_process::kill()
{
	enabled = false;
	flag_killed = true;
	
	if(started)
	{
		if(!thread_process_terminated)
		{
			if(kernel->get_current_thread_process() == this)
			{
				// suicide
				flag_is_unwinding = true;
				throw sc_unwind_exception(false);
			}
			else
			{
				// kill requested by another process
				
				switch_to(); // switch to thread being killed and let thread die (throw)
			}
		}
#if SC_THREAD_PROCESSES_USE_PTHREADS
		pthread_join(thrd, NULL);		
#else
		if(coro)
		{
			delete coro;
			coro = 0;
		}
#endif

		started = false;
	}
}

void sc_thread_process::reset(bool async)
{
	wait_type = WAIT_DEFAULT; // restore static sensitivity
	
	flag_reset = true;
	
	if(started)
	{
		if(!thread_process_terminated)
		{
			if(kernel->get_current_thread_process() == this)
			{
				// self reset
				flag_is_unwinding = true;
				throw sc_unwind_exception(true);
			}
			else
			{
				// reset requested by another process
				
				switch_to(); // switch to thread being reset and let thread reset (throw) itself
			}
		}
	}
}

void sc_thread_process::throw_it(const sc_user_exception& user_exception)
{
	wait_type = WAIT_DEFAULT; // restore static sensitivity
	flag_throw_it = true;
	
	if(started)
	{
		if(!thread_process_terminated)
		{
			if(kernel->get_current_thread_process() == this)
			{
				// self throw
				user_exception.throw_it(); 
			}
			else
			{
				this->user_exception = &user_exception;
				switch_to(); // switch to thread being reset and let thread throw itself
			}
		}
	}
	
}

const char *sc_thread_process::kind() const
{
	return "sc_thread_process";
}

} // end of namespace sc_core
