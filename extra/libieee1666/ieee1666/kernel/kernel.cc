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

#include <ieee1666/kernel/kernel.h>
#include <ieee1666/kernel/module_name.h>
#include <ieee1666/kernel/thread_process.h>
#include <ieee1666/kernel/method_process.h>
#include <ieee1666/kernel/time.h>
#include <ieee1666/kernel/kernel_event.h>
#include <ieee1666/kernel/prim_channel.h>
#include <math.h>
#include <limits>

namespace sc_core {

sc_kernel *sc_kernel::kernel = 0;

static const char *time_unit_strings[SC_SEC + 1] = { "fs", "ps", "ns", "us", "ms", "s" };

sc_kernel::sc_kernel()
{
	set_time_resolution(1.0, SC_PS, false);
}

sc_kernel *sc_kernel::get_kernel()
{
	if(!kernel)
	{
		kernel = new sc_kernel();
	}
	
	return kernel;
}

void sc_kernel::push_module_name(sc_module_name *module_name)
{
	module_name_stack.push(module_name);
}

void sc_kernel::pop_module_name()
{
	module_name_stack.pop();
}

sc_module_name *sc_kernel::get_top_of_module_name_stack() const
{
	return module_name_stack.empty() ? 0 : module_name_stack.top();
}

void sc_kernel::begin_object(sc_object *object)
{
	object_stack.push(object);
}

void sc_kernel::end_object()
{
	object_stack.pop();
}

sc_object *sc_kernel::get_current_object() const
{
	return object_stack.empty() ? 0 : object_stack.top();
}

sc_thread_process *sc_kernel::get_current_thread_process() const
{
	return current_thread_process;
}

sc_thread_process *sc_kernel::create_thread_process(const char *name, sc_process_owner *process_owner, sc_process_owner_method_ptr process_owner_method_ptr, const sc_spawn_options *spawn_options)
{
	sc_thread_process *thread_process = new sc_thread_process(name, process_owner, process_owner_method_ptr, spawn_options);
	
	return thread_process;
}

sc_method_process *sc_kernel::create_method_process(const char *name, sc_process_owner *process_owner, sc_process_owner_method_ptr process_owner_method_ptr, const sc_spawn_options *spawn_options)
{
	sc_method_process *method_process = new sc_method_process(name, process_owner, process_owner_method_ptr, spawn_options);
	
	return method_process;
}

void sc_kernel::initialize()
{
	// time resolution can no longer change
	time_resolution_fixed = true;

	// start thread processes in suspend state
	unsigned int num_thread_processes = thread_process_table.size();
	
	unsigned int i;
	for(i = 0; i < num_thread_processes; i++)
	{
		sc_thread_process *thread_process = thread_process_table[i];
		
		thread_process->start(); // at this point, process is suspended before call to sc_process_owner method
	}
	
	// initial wake-up of all SC_METHOD processes
	unsigned int num_method_processes = method_process_table.size();
	for(i = 0; i < num_method_processes; i++)
	{
		sc_method_process *method_process = method_process_table[i];
		
		current_method_process = method_process;
		method_process->call_process_owner_method();
	}

	// initial wake-up of all SC_THREAD/SC_CTHREAD processes
	for(i = 0; i < num_thread_processes; i++)
	{
		sc_thread_process *thread_process = thread_process_table[i];
		
		current_thread_process = thread_process;
		thread_process->switch_to();
	}
}

void sc_kernel::do_delta_steps(bool once)
{
	do
	{
		// evaluation phase
		
		// wake up SC_METHOD processes
		if(runnable_method_processes.size())
		{
			do
			{
				sc_method_process *method_process = runnable_method_processes.front();
				runnable_method_processes.pop_front();
				
				current_method_process = method_process;
				method_process->call_process_owner_method();
			}
			while(runnable_method_processes.size());
		}

		// wake up SC_THREAD/SC_CTHREAD processes
		if(runnable_thread_processes.size())
		{
			do
			{
				sc_thread_process *thread_process = runnable_thread_processes.front();
				runnable_thread_processes.pop_front();
				
				current_thread_process = thread_process;
				thread_process->switch_to();
			}
			while(runnable_thread_processes.size());
		}
		
		// update phase
		
		if(updatable_prim_channels.size())
		{
			do
			{
				sc_prim_channel *prim_channel = updatable_prim_channels.front();
				updatable_prim_channels.pop_front();
				
				prim_channel->update();
			}
			while(updatable_prim_channels.size());
		}
		
		// delta notification phase
		
		if(delta_events.size())
		{
			do
			{
				sc_kernel_event *kernel_event = delta_events.front();
				delta_events.pop_front();
				
				sc_event *e = kernel_event->get_event();
				
				if(e)
				{
					e->trigger();
				}
				
				kernel_events_allocator.free(kernel_event);
			}
			while(delta_events.size());
		}
	}
	while(!once && (runnable_thread_processes.size() || runnable_method_processes.size()));
}

void sc_kernel::do_timed_step()
{
	std::multimap<sc_time, sc_timed_kernel_event *>::iterator it = schedule.begin();

	if(it != schedule.end())
	{
		current_time_stamp = (*it).first;
		
		do
		{
			sc_timed_kernel_event *kernel_event = (*it).second;
			
			sc_event *e = kernel_event->get_event();
			
			if(e)
			{
				e->trigger();
			}
			
			kernel_events_allocator.free(kernel_event);
			
			schedule.erase(it);
			it = schedule.begin();
		}
		while((*it).first == current_time_stamp);
	}
}

void sc_kernel::simulate(const sc_time& duration)
{
	if(duration == SC_ZERO_TIME)
	{
		do_delta_steps(true);
	}
	else
	{
		sc_time until_time = current_time_stamp + duration;

		do
		{
			do_delta_steps(false);
			do_timed_step();
		}
		while((current_time_stamp <= until_time) && (runnable_thread_processes.size() || runnable_method_processes.size()));
	}
}

void sc_kernel::start(const sc_time& duration, sc_starvation_policy p)
{
	sc_time end_time;
	
	if(p == SC_RUN_TO_TIME)
	{
		end_time = current_time_stamp;
		end_time += duration;
	}
	
	initialize();
	simulate(duration);

	if(p == SC_RUN_TO_TIME)
	{
		current_time_stamp = end_time;
	}
}

void sc_kernel::add_module(sc_module *module)
{
	module_table.push_back(module);
}

void sc_kernel::add_thread_process(sc_thread_process *thread_process)
{
	thread_process_table.push_back(thread_process);
}

void sc_kernel::add_method_process(sc_method_process *method_process)
{
	method_process_table.push_back(method_process);
}

void sc_kernel::set_time_resolution(double v, sc_time_unit tu, bool user)
{
	if(time_resolution_fixed)
	{
		throw std::runtime_error("time resolution already fixed");
	}
	
	if(user)
	{
		if(time_resolution_fixed_by_user)
		{
			std::runtime_error("time resolution already set");
		}
		time_resolution_fixed_by_user = true;
	}
	
	double log10_v = log10(v);
	double log10_v_int_part;
    if(modf(log10_v, &log10_v_int_part) != 0.0 )
	{
		throw std::runtime_error("time resolution shall be a power of ten");
	}

	time_resolution_scale_factors_table_base_index = static_cast<int>(log10_v) + (3 * tu);

	double time_resolution_sec = (v * pow10(-3 * (SC_SEC - tu)));
	if(time_resolution_sec < 1e-15)
	{
		throw std::runtime_error("time resolution is less than 1 fs");
	}

	int i;
	double scale_factor;
	for(i = time_resolution_scale_factors_table_base_index - 1, scale_factor = 0.1; i >= 0; i--, scale_factor /= 10)
	{
		time_resolution_scale_factors_table[i] = scale_factor;
	}
	for(i = time_resolution_scale_factors_table_base_index, scale_factor = 1; i < TIME_RESOLUTION_SCALE_FACTORS_TABLE_SIZE; i++, scale_factor *= 10)
	{
		time_resolution_scale_factors_table[i] = scale_factor;
	}
	
	if(user) time_resolution_fixed = true;
	
	max_time = sc_time(std::numeric_limits<sc_dt::uint64>::max());
}

sc_dt::uint64 sc_kernel::get_time_discrete_value(double d, sc_time_unit tu) const
{
	time_resolution_fixed = true;
	int time_resolution_scale_factors_table_index = 3 * tu;
	return static_cast<sc_dt::uint64>((d * time_resolution_scale_factors_table[time_resolution_scale_factors_table_index]) + 0.5);
}

void sc_kernel::print_time(std::ostream& os, const sc_time& t) const
{
	sc_dt::uint64 discrete_value = t.value();

	if(!discrete_value)
	{
		os << "0 s";
		return;
	}
	
	sc_time_unit tu = (sc_time_unit)(time_resolution_scale_factors_table_base_index / 3);
	while(((discrete_value % 1000) == 0) && (tu != SC_SEC))
	{
		discrete_value /= 1000;
		tu = (sc_time_unit)(tu + 1);
	}

	int num_zeros = time_resolution_scale_factors_table_base_index % 3;
	
	if(tu != SC_SEC)
	{
		while((discrete_value % 10) == 0)
		{
			discrete_value /= 10;
			num_zeros++;
		}
		
		if(num_zeros >= 3)
		{
			tu = (sc_time_unit)(tu + 1);
			num_zeros -= 3;
		}

		while(num_zeros)
		{
			discrete_value *= 10;
			num_zeros--;
		}
	}
	
	os << discrete_value;
	while(num_zeros)
	{
		os << '0';
		num_zeros--;
	}
	os << " " << time_unit_strings[tu];
}

double sc_kernel::time_discrete_value_to_seconds(sc_dt::uint64 discrete_value) const
{
	if(discrete_value == 0) return 0.0;
	time_resolution_fixed = true;
	
	double scale_factor = time_resolution_scale_factors_table[3 * SC_SEC];
	return (double) discrete_value / scale_factor;
}

sc_time sc_kernel::get_time_resolution() const
{
	return sc_time(1);
}

const sc_time& sc_kernel::get_max_time() const
{
	return max_time;
}

sc_kernel_event *sc_kernel::notify(sc_event *e)
{
	sc_kernel_event *kernel_event = kernel_events_allocator.allocate();
	kernel_event->initialize(e);
	delta_events.push_back(kernel_event);
	return kernel_event;
}

sc_timed_kernel_event *sc_kernel::notify(sc_event *e, const sc_time& t)
{
	sc_time kernel_event_time = current_time_stamp;
	kernel_event_time += t;
	sc_timed_kernel_event *timed_kernel_event = timed_kernel_events_allocator.allocate();
	timed_kernel_event->initialize(e, kernel_event_time);
	schedule.insert(std::pair<sc_time, sc_timed_kernel_event *>(kernel_event_time, timed_kernel_event));
	return timed_kernel_event;
}

void sc_kernel::wait()
{
	current_thread_process->wait();
}

void sc_kernel::wait(const sc_event& e)
{
	current_thread_process->wait(e);
}

void sc_kernel::next_trigger()
{
	current_method_process->next_trigger();
}

void sc_kernel::next_trigger(const sc_event& e)
{
	current_method_process->next_trigger(e);
}

void sc_kernel::trigger(sc_thread_process *thread_process)
{
	runnable_thread_processes.push_back(thread_process);
}

void sc_kernel::trigger(sc_method_process *method_process)
{
	runnable_method_processes.push_back(method_process);
}

const sc_time& sc_kernel::get_current_time_stamp() const
{
	return current_time_stamp;
}

int sc_elab_and_sim(int argc, char* argv[])
{
	return 0;
}

int sc_argc()
{
	return 0;
}

const char* const* sc_argv()
{
	return 0;
}

void sc_start()
{
    sc_start(sc_max_time() - sc_time_stamp(), SC_EXIT_ON_STARVATION);
}

void sc_start(const sc_time& duration, sc_starvation_policy p)
{
	sc_kernel::get_kernel()->start(duration, p);
}

void sc_start(double _duration,sc_time_unit _tu, sc_starvation_policy _p)
{
	sc_time duration(_duration, _tu);
	
	sc_start(duration, _p);
}

void sc_pause()
{
}

void sc_set_stop_mode( sc_stop_mode mode )
{
}

sc_stop_mode sc_get_stop_mode()
{
}

void sc_stop()
{
}

const sc_time& sc_time_stamp()
{
	return sc_kernel::get_kernel()->get_current_time_stamp();
}

const sc_dt::uint64 sc_delta_count()
{
}

bool sc_is_running()
{
}

bool sc_pending_activity_at_current_time()
{
}

bool sc_pending_activity_at_future_time()
{
}

bool sc_pending_activity()
{
}

sc_time sc_time_to_pending_activity()
{
}

sc_status sc_get_status()
{
}

} // end of namespace sc_core

extern int sc_main(int argc, const char *argv[]);

int main(int argc, const char *argv[], const char *envp[])
{
	return sc_main(argc, argv);
}
