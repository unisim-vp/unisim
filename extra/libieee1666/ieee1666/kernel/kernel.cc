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
#include <ieee1666/kernel/module.h>
#include <ieee1666/kernel/port.h>
#include <ieee1666/kernel/export.h>
#include <ieee1666/kernel/prim_channel.h>
#include <ieee1666/kernel/module_name.h>
#include <ieee1666/kernel/thread_process.h>
#include <ieee1666/kernel/method_process.h>
#include <ieee1666/kernel/time.h>
#include <ieee1666/kernel/kernel_event.h>
#include <math.h>
#include <string.h>
#include <limits>

extern int sc_main(int argc, char *argv[]);

namespace sc_core {

sc_kernel *sc_kernel::kernel = 0;
static int argc;
static char **argv;

static const char *time_unit_strings[SC_SEC + 1] = { "fs", "ps", "ns", "us", "ms", "s" };

sc_kernel::sc_kernel()
	: module_name_stack()
	, object_stack()
	, unique_name_map()
	, top_level_objects()
	, event_registry()
	, module_table()
	, port_table()
	, export_table()
	, prim_channel_table()
	, thread_process_table()
	, method_process_table()
	, time_resolution_fixed_by_user(false)
	, time_resolution_fixed(false)
	, time_resolution_scale_factors_table_base_index(0)
	, time_resolution_scale_factors_table()
	, max_time()
	, current_thread_process(0)
	, current_method_process(0)
	, current_time_stamp()
	, kernel_events_allocator()
	, timed_kernel_events_allocator()	
	, runnable_thread_processes()
	, runnable_method_processes()
	, updatable_prim_channels()
	, delta_events() 
	, schedule()
	, user_requested_stop(false)
	, user_requested_pause(false)
	, stop_mode(SC_STOP_IMMEDIATE)
	, status(SC_ELABORATION)
	, initialized(false)
	, end_of_simulation_invoked(false)
	, start_of_simulation_invoked(false)
	, delta_count(0)
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
	sc_module_name *module_name = module_name_stack.empty() ? 0 : module_name_stack.top();
	
	if(!module_name) throw std::runtime_error("module name stack is empty");
	
	return module_name;
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

void sc_kernel::report_before_end_of_elaboration()
{
	unsigned int i;
	unsigned int num_modules = module_table.size();
	for(i = 0; i < num_modules; i++) module_table[i]->before_end_of_elaboration();
	unsigned int num_ports = port_table.size();
	for(i = 0; i < num_ports; i++) port_table[i]->before_end_of_elaboration();
	unsigned int num_exports = port_table.size();
	for(i = 0; i < num_exports; i++) port_table[i]->before_end_of_elaboration();
	unsigned int num_prim_channels = prim_channel_table.size();
	for(i = 0; i < num_prim_channels; i++) prim_channel_table[i]->before_end_of_elaboration();
}

void sc_kernel::report_end_of_elaboration()
{
	unsigned int i;
	unsigned int num_modules = module_table.size();
	for(i = 0; i < num_modules; i++) module_table[i]->end_of_elaboration();
	unsigned int num_ports = port_table.size();
	for(i = 0; i < num_ports; i++) port_table[i]->end_of_elaboration();
	unsigned int num_exports = port_table.size();
	for(i = 0; i < num_exports; i++) port_table[i]->end_of_elaboration();
	unsigned int num_prim_channels = prim_channel_table.size();
	for(i = 0; i < num_prim_channels; i++) prim_channel_table[i]->end_of_elaboration();
}

void sc_kernel::report_start_of_simulation()
{
	unsigned int i;
	unsigned int num_modules = module_table.size();
	for(i = 0; i < num_modules; i++) module_table[i]->start_of_simulation();
	unsigned int num_ports = port_table.size();
	for(i = 0; i < num_ports; i++) port_table[i]->start_of_simulation();
	unsigned int num_exports = port_table.size();
	for(i = 0; i < num_exports; i++) port_table[i]->start_of_simulation();
	unsigned int num_prim_channels = prim_channel_table.size();
	for(i = 0; i < num_prim_channels; i++) prim_channel_table[i]->start_of_simulation();
	start_of_simulation_invoked = true;
}

void sc_kernel::report_end_of_simulation()
{
	unsigned int i;
	unsigned int num_modules = module_table.size();
	for(i = 0; i < num_modules; i++) module_table[i]->end_of_simulation();
	unsigned int num_ports = port_table.size();
	for(i = 0; i < num_ports; i++) port_table[i]->end_of_simulation();
	unsigned int num_exports = port_table.size();
	for(i = 0; i < num_exports; i++) port_table[i]->end_of_simulation();
	unsigned int num_prim_channels = prim_channel_table.size();
	for(i = 0; i < num_prim_channels; i++) prim_channel_table[i]->end_of_simulation();
	end_of_simulation_invoked = true;
}

void sc_kernel::initialize()
{
	unsigned int i;
	
	status = SC_BEFORE_END_OF_ELABORATION;
	report_before_end_of_elaboration();
	
	unsigned int num_modules = module_table.size();
	for(i = 0; i < num_modules; i++)
	{
		sc_module *module =	module_table[i];
		
		module->before_end_of_elaboration();
	}
	
	status = SC_END_OF_ELABORATION;
	report_end_of_elaboration();
	
	for(i = 0; i < num_modules; i++)
	{
		sc_module *module =	module_table[i];
		
		module->end_of_elaboration();
	}

	// time resolution can no longer change
	time_resolution_fixed = true;

	// start thread processes in suspend state
	unsigned int num_thread_processes = thread_process_table.size();
	
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
		method_process->commit_next_trigger();
	}

	// initial wake-up of all SC_THREAD/SC_CTHREAD processes
	for(i = 0; i < num_thread_processes; i++)
	{
		sc_thread_process *thread_process = thread_process_table[i];
		
		current_thread_process = thread_process;
		thread_process->switch_to();
	}
	
	initialized = true;
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
				if(user_requested_stop && (stop_mode == SC_STOP_IMMEDIATE)) return;
				
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
				if(user_requested_stop && (stop_mode == SC_STOP_IMMEDIATE)) return;
			}
			while(runnable_thread_processes.size());
		}
		
		delta_count++;

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
		
		if(user_requested_stop) return;
		
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
		
		if(user_requested_pause) break;
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
	status = SC_START_OF_SIMULATION;
	report_start_of_simulation();
	
	if(duration == SC_ZERO_TIME)
	{
		do_delta_steps(true);
	}
	else
	{
		sc_time until_time(current_time_stamp);
		until_time += duration;

		do
		{
			do_delta_steps(false);
			if(user_requested_stop || user_requested_pause) break;
			do_timed_step();
			if(user_requested_stop) break;
		}
		while((current_time_stamp <= until_time) && (runnable_thread_processes.size() || runnable_method_processes.size()));
	}
	
	if(user_requested_stop)
	{
		status = SC_STOPPED;
	}
	else
	{
		status = SC_PAUSED;
	}
}

void sc_kernel::start(const sc_time& duration, sc_starvation_policy p)
{
	if(status == SC_END_OF_SIMULATION) return;
	
	sc_time end_time;
	
	if(p == SC_RUN_TO_TIME)
	{
		end_time = current_time_stamp;
		end_time += duration;
	}

	if(!initialized) initialize();
	simulate(duration);

	if(p == SC_RUN_TO_TIME)
	{
		current_time_stamp = end_time;
	}
}

void sc_kernel::add_top_level_object(sc_object *object)
{
	top_level_objects.push_back(object);
}

void sc_kernel::register_event(sc_event *event)
{
	event_registry.insert(std::pair<std::string, sc_event *>(event->name(), event));
}

void sc_kernel::unregister_event(sc_event *event)
{
	std::map<std::string, sc_event *>::iterator it = event_registry.find(event->name());
	
	if(it != event_registry.end())
	{
		event_registry.erase(it);
	}
	else
	{
		throw std::runtime_error("Internal error! can't unregister event because it is not registered");
	}
}

void sc_kernel::add_module(sc_module *module)
{
	module_table.push_back(module);
}

void sc_kernel::add_port(sc_port_base *port)
{
	port_table.push_back(port);
}

void sc_kernel::add_export(sc_export_base *exp)
{
	export_table.push_back(exp);
}

void sc_kernel::add_prim_channel(sc_prim_channel *prim_channel)
{
	prim_channel_table.push_back(prim_channel);
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

void sc_kernel::wait(const sc_event_and_list& el)
{
	current_thread_process->wait(el);
}

void sc_kernel::wait(const sc_event_or_list& el)
{
	current_thread_process->wait(el);
}

void sc_kernel::wait(const sc_time& t)
{
	current_thread_process->wait(t);
}

void sc_kernel::wait(const sc_time& t, const sc_event& e)
{
	current_thread_process->wait(t, e);
}

void sc_kernel::wait(const sc_time& t, const sc_event_and_list& el)
{
	current_thread_process->wait(t, el);
}

void sc_kernel::wait(const sc_time& t, const sc_event_or_list& el)
{
	current_thread_process->wait(t, el);
}

void sc_kernel::next_trigger()
{
	current_method_process->next_trigger();
}

void sc_kernel::next_trigger(const sc_event& e)
{
	current_method_process->next_trigger(e);
}

void sc_kernel::next_trigger(const sc_event_and_list& el)
{
	current_method_process->next_trigger(el);
}

void sc_kernel::next_trigger(const sc_event_or_list& el)
{
	current_method_process->next_trigger(el);
}

void sc_kernel::next_trigger(const sc_time& t)
{
	current_method_process->next_trigger(t);
}

void sc_kernel::next_trigger(const sc_time& t, const sc_event& e)
{
	current_method_process->next_trigger(t, e);
}

void sc_kernel::next_trigger(const sc_time& t, const sc_event_and_list& el)
{
	current_method_process->next_trigger(t, el);
}

void sc_kernel::next_trigger(const sc_time& t, const sc_event_or_list& el)
{
	current_method_process->next_trigger(t, el);
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

void sc_kernel::set_stop_mode(sc_stop_mode mode)
{
	stop_mode = mode;
}

sc_stop_mode sc_kernel::get_stop_mode() const
{
	return stop_mode;
}

void sc_kernel::stop()
{
	user_requested_stop = true;
	
	if(status == SC_PAUSED)
	{
		status = SC_END_OF_SIMULATION;
		report_end_of_simulation();
	}
}

void sc_kernel::pause()
{
	if(user_requested_stop) return;
	
	user_requested_pause = true;
}

sc_status sc_kernel::get_status() const
{
	return status;
}

bool sc_kernel::is_end_of_simulation_invoked() const
{
	return end_of_simulation_invoked;
}

bool sc_kernel::is_start_of_simulation_invoked() const
{
	return start_of_simulation_invoked;
}

bool sc_kernel::is_running() const
{
	return status == SC_RUNNING;
}

sc_dt::uint64 sc_kernel::get_delta_count() const
{
	return delta_count;
}

bool sc_kernel::pending_activity_at_current_time() const
{
	std::multimap<sc_time, sc_timed_kernel_event *>::const_iterator it = schedule.begin();

	if(it != schedule.end())
	{
		const sc_time& time_of_pending_activity = (*it).first;
		return time_of_pending_activity == current_time_stamp;
	}
	
	return false;
}

bool sc_kernel::pending_activity_at_future_time() const
{
	std::multimap<sc_time, sc_timed_kernel_event *>::const_iterator it = schedule.begin();

	if(it != schedule.end())
	{
		const sc_time& time_of_pending_activity = (*it).first;
		return time_of_pending_activity > current_time_stamp;
	}
	
	return false;
}

bool sc_kernel::pending_activity() const
{
	return schedule.size() != 0;
}

sc_time sc_kernel::time_to_pending_activity() const
{
	std::multimap<sc_time, sc_timed_kernel_event *>::const_iterator it = schedule.begin();

	if(it != schedule.end())
	{
		const sc_time& time_of_pending_activity = (*it).first;
		return time_of_pending_activity - current_time_stamp;
	}
	
	return max_time - current_time_stamp;
}

const std::vector<sc_object*>& sc_kernel::get_top_level_objects() const
{
	return top_level_objects;
}

sc_object *sc_kernel::find_object(const char* name) const
{
	unsigned int n_top_level_objects = top_level_objects.size();
	unsigned int i;
	for(i = 0; i < n_top_level_objects; i++)
	{
		sc_object *object = top_level_objects[i];
		if(strcmp(object->name(), name) == 0) return object;
		
		sc_object *found_child_object = object->find_child_object(name);
		if(found_child_object) return found_child_object;
	}
	return 0;
}

sc_event *sc_kernel::find_event(const char *name) const
{
	std::map<std::string, sc_event *>::const_iterator it = event_registry.find(name);
	
	return (it != event_registry.end()) ? (*it).second : 0;
}

bool sc_kernel::hierarchical_name_exists(const char *name) const
{
	sc_object *found_object = find_object(name);
	
	if(found_object) return true;
	
	sc_event *found_event = find_event(name);
	
	if(found_event) return true;
	
	return false;
}

const char* sc_kernel::gen_unique_name( const char* seed ) const
{
	static std::string unique_name;
	
	std::string s;
	
	std::map<std::string, unsigned int>::iterator it = unique_name_map.find(seed);
	
	unsigned int id;
	
	if(it != unique_name_map.end())
	{
		id = ++(*it).second;
	}
	else
	{
		id = 0;
		unique_name_map.insert(std::pair<std::string, unsigned int>(seed, id));
	}
	
	std::stringstream sstr;
	sstr << seed << '_' << id;
	unique_name = sstr.str();
	return unique_name.c_str();
}

int sc_elab_and_sim(int _argc, char* _argv[])
{
	argc = _argc;
	argv = _argv;
	
	int status = sc_main(argc, argv);
	
	if(sc_kernel::kernel) delete sc_kernel::kernel;
	return status;
}

int sc_argc()
{
	return argc;
}

const char* const* sc_argv()
{
	return argv;
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
	sc_kernel::get_kernel()->pause();
}

void sc_set_stop_mode( sc_stop_mode mode )
{
	sc_kernel::get_kernel()->set_stop_mode(mode);
}

sc_stop_mode sc_get_stop_mode()
{
	return sc_kernel::get_kernel()->get_stop_mode();
}

void sc_stop()
{
	sc_kernel::get_kernel()->stop();
}

const sc_time& sc_time_stamp()
{
	return sc_kernel::get_kernel()->get_current_time_stamp();
}

const sc_dt::uint64 sc_delta_count()
{
	return sc_kernel::get_kernel()->get_delta_count();
}

bool sc_is_running()
{
	return sc_kernel::get_kernel()->is_running();
}

bool sc_pending_activity_at_current_time()
{
	return sc_kernel::get_kernel()->pending_activity_at_current_time();
}

bool sc_pending_activity_at_future_time()
{
	return sc_kernel::get_kernel()->pending_activity_at_future_time();
}

bool sc_pending_activity()
{
	return sc_kernel::get_kernel()->pending_activity();
}

sc_time sc_time_to_pending_activity()
{
	return sc_kernel::get_kernel()->time_to_pending_activity();
}

sc_status sc_get_status()
{
	return sc_kernel::get_kernel()->get_status();
}

bool sc_start_of_simulation_invoked()
{
	return sc_kernel::get_kernel()->is_start_of_simulation_invoked();
}

bool sc_end_of_simulation_invoked()
{
	return sc_kernel::get_kernel()->is_end_of_simulation_invoked();
}

const std::vector<sc_object*>& sc_get_top_level_objects()
{
	return sc_kernel::get_kernel()->get_top_level_objects();
}

sc_object* sc_find_object( const char* name)
{
	return sc_kernel::get_kernel()->find_object(name);
}

bool sc_hierarchical_name_exists(const char *name)
{
	return sc_kernel::get_kernel()->hierarchical_name_exists(name);
}

const char* sc_gen_unique_name( const char* seed )
{
	return sc_kernel::get_kernel()->gen_unique_name(seed);
}

void next_trigger()
{
	sc_kernel::get_kernel()->next_trigger();
}

void next_trigger(const sc_event& e)
{
	sc_kernel::get_kernel()->next_trigger(e);
}

void next_trigger(const sc_event_or_list& el)
{
	sc_kernel::get_kernel()->next_trigger(el);
}

void next_trigger(const sc_event_and_list& el)
{
	sc_kernel::get_kernel()->next_trigger(el);
}

void next_trigger(const sc_time& t)
{
	sc_kernel::get_kernel()->next_trigger(t);
}

void next_trigger(double d, sc_time_unit tu)
{
	sc_time t = sc_time(d, tu);
	sc_kernel::get_kernel()->next_trigger(t);
}

void next_trigger(const sc_time& t, const sc_event& e)
{
	sc_kernel::get_kernel()->next_trigger(t, e);
}

void next_trigger(double d, sc_time_unit tu, const sc_event& e)
{
	sc_time t = sc_time(d, tu);
	sc_kernel::get_kernel()->next_trigger(t, e);
}

void next_trigger(const sc_time& t, const sc_event_or_list& el)
{
	sc_kernel::get_kernel()->next_trigger(t, el);
}

void next_trigger(double d, sc_time_unit tu, const sc_event_or_list& el)
{
	sc_time t = sc_time(d, tu);
	sc_kernel::get_kernel()->next_trigger(t, el);
}

void next_trigger(const sc_time& t, const sc_event_and_list& el)
{
	sc_kernel::get_kernel()->next_trigger(t, el);
}

void next_trigger(double d, sc_time_unit tu, const sc_event_and_list& el)
{
	sc_time t = sc_time(d, tu);
	sc_kernel::get_kernel()->next_trigger(t, el);
}

void wait()
{
	sc_kernel::get_kernel()->wait();
}

void wait( int )
{
}

void wait(const sc_event& e)
{
	sc_kernel::get_kernel()->wait(e);
}

void wait(const sc_event_or_list & el)
{
	sc_kernel::get_kernel()->wait(el);
}

void wait(const sc_event_and_list& el)
{
	sc_kernel::get_kernel()->wait(el);
}

void wait(const sc_time& t)
{
	sc_kernel::get_kernel()->wait(t);
}

void wait(double d, sc_time_unit tu)
{
	sc_time t = sc_time(d, tu);
	sc_kernel::get_kernel()->wait(t);
}

void wait(const sc_time& t, const sc_event& e)
{
	sc_kernel::get_kernel()->wait(t, e);
}

void wait(double d, sc_time_unit tu, const sc_event& e)
{
	sc_time t = sc_time(d, tu);
	sc_kernel::get_kernel()->wait(t, e);
}

void wait(const sc_time& t, const sc_event_or_list& el)
{
	sc_kernel::get_kernel()->wait(t, el);
}

void wait(double d, sc_time_unit tu, const sc_event_or_list& el)
{
	sc_time t = sc_time(d, tu);
	sc_kernel::get_kernel()->wait(t, el);
}

void wait(const sc_time& t, const sc_event_and_list& el)
{
	sc_kernel::get_kernel()->wait(t, el);
}

void wait(double d, sc_time_unit tu, const sc_event_and_list& el)
{
	sc_time t = sc_time(d, tu);
	sc_kernel::get_kernel()->wait(t, el);
}

} // end of namespace sc_core

int main(int argc, char *argv[], char *envp[])
{
	return sc_core::sc_elab_and_sim(argc, argv);
}
