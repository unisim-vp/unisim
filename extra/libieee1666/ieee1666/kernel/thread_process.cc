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

namespace sc_core {

sc_thread_process_helper::sc_thread_process_helper(sc_thread_process *_thread_process, sc_coroutine::caller_type& _yield)
	: thread_process(_thread_process)
	, yield(_yield)
{
	thread_process->thread_process_helper = this;
	std::cerr << "initial yield" << std::endl;
	yield();
	std::cerr << "initial wake-up" << std::endl;
	thread_process->call_process_owner_method();
}

sc_thread_process_helper::~sc_thread_process_helper()
{
	thread_process->thread_process_helper = 0;
}

sc_thread_process::sc_thread_process(const char *_name, sc_process_owner *_process_owner, sc_process_owner_method_ptr _process_owner_method_ptr, const sc_spawn_options *spawn_options)
	: sc_process(_name, _process_owner, _process_owner_method_ptr, SC_THREAD_PROC_)
	, coro(0)
	, thread_process_helper(0)
	, stack_size(spawn_options ? spawn_options->get_stack_size() : 0)
{
	sc_kernel::get_kernel()->add_thread_process(this);
}

sc_thread_process::~sc_thread_process()
{
	if(coro)
	{
		delete coro;
	}
}

void sc_thread_process::set_stack_size(int _stack_size)
{
	stack_size = _stack_size;
}

void sc_thread_process::start()
{
	if(stack_size)
	{
		sc_coroutine_attributes coro_attributes = sc_coroutine_attributes(stack_size);
		coro = new sc_coroutine(boost::bind( &sc_thread_process::coroutine_work, this, _1), coro_attributes);
	}
	else
	{
			coro = new sc_coroutine(boost::bind( &sc_thread_process::coroutine_work, this, _1));
	}
}

void sc_thread_process::yield()
{
	if(thread_process_helper)
	{
		thread_process_helper->yield(); // yield to kernel
	}
	else
	{
		std::cerr << "no thread process helper" << std::endl;
	}
}

void sc_thread_process::wait()
{
	yield();
}

void sc_thread_process::wait(const sc_event& e)
{
	e.add_dynamically_sensitive_thread_process(this);
	yield();
}

void sc_thread_process::switch_to()
{
	if(*coro)
	{
		(*coro)(); // yield to thread
	}
}

void sc_thread_process::coroutine_work(sc_coroutine::caller_type& yield)
{
	sc_thread_process_helper(this, yield);
}

void sc_thread_process::suspend(sc_descendant_inclusion_info include_descendants)
{
}

void sc_thread_process::resume(sc_descendant_inclusion_info include_descendants)
{
}

void sc_thread_process::disable(sc_descendant_inclusion_info include_descendants)
{
}

void sc_thread_process::enable(sc_descendant_inclusion_info include_descendants)
{
}

void sc_thread_process::kill(sc_descendant_inclusion_info include_descendants)
{
}

void sc_thread_process::reset(sc_descendant_inclusion_info include_descendants)
{
}

} // end of namespace sc_core
