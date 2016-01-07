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

#ifndef __IEEE1666_KERNEL_KERNEL_H__
#define __IEEE1666_KERNEL_KERNEL_H__

#include <ieee1666/kernel/fwd.h>
#include <ieee1666/kernel/time.h>
#include <ieee1666/kernel/allocator.h>
#include <stack>
#include <vector>
#include <iosfwd>
#include <map>
#include <deque>
#include <unordered_set>
#include <set>
#include <ieee1666/kernel/kernel_event.h>
#include <ieee1666/kernel/thread_process.h>
#include <ieee1666/kernel/method_process.h>
#include <ieee1666/kernel/prim_channel.h>
#include <ieee1666/kernel/process_handle.h>

namespace sc_core {

class sc_kernel
{
public:
	sc_kernel();
	~sc_kernel();
	
	void push_module_name(sc_module_name *module_name);
	void pop_module_name();
	sc_module_name *get_top_of_module_name_stack() const;
	
	static sc_kernel *get_kernel();
	
	void begin_module(sc_object *object);
	void end_module();
	sc_object *get_current_object() const;
	sc_object *get_current_writer() const;
	sc_method_process *get_current_method_process() const;
	sc_thread_process *get_current_thread_process() const;

	sc_process_handle create_thread_process(const char *name, sc_process_owner *process_owner, sc_process_owner_method_ptr process_owner_method_ptr, const sc_spawn_options *);
	sc_process_handle create_cthread_process(const char *name, sc_process_owner *process_owner, sc_process_owner_method_ptr process_owner_method_ptr, const sc_spawn_options *, sc_event_finder& edge_event_finder);
	sc_process_handle create_method_process(const char *name, sc_process_owner *process_owner, sc_process_owner_method_ptr process_owner_method_ptr, const sc_spawn_options *);
	
	void initialize();
	void do_delta_steps(bool once);
	bool do_timed_step();
	void simulate(const sc_time& duration);
	void start(const sc_time& duration, sc_starvation_policy p = SC_RUN_TO_TIME);
	
	void register_object(sc_object *object);
	void register_event(sc_event *event);
	void register_module(sc_module *module);
	void register_port(sc_port_base *port);
	void register_export(sc_export_base *exp);
	void register_prim_channel(sc_prim_channel *prim_channel);
	void register_thread_process(sc_thread_process *thread_process);
	void register_method_process(sc_method_process *method_process);
	
	void unregister_object(sc_object *object);
	void unregister_event(sc_event *event);
	void unregister_module(sc_module *module);
	void unregister_port(sc_port_base *port);
	void unregister_export(sc_export_base *exp);
	void unregister_prim_channel(sc_prim_channel *prim_channel);
	void unregister_thread_process(sc_thread_process *thread_process);
	void unregister_method_process(sc_method_process *method_process);

	void terminate_thread_process(sc_thread_process *thread_process);
	void terminate_method_process(sc_method_process *method_process);
	void disconnect_thread_process(sc_thread_process *thread_process);
	void disconnect_method_process(sc_method_process *method_process);
	void release_terminated_thread_processes();
	void release_terminated_method_processes();
	
	// time resolution management
	void set_time_resolution(double v, sc_time_unit tu, bool user);
	sc_dt::uint64 get_time_discrete_value(double d, sc_time_unit tu) const;
	sc_time get_time_resolution() const;
	const sc_time& get_max_time() const;
	void print_time(std::ostream& os, const sc_time& t) const;
	double time_discrete_value_to_seconds(sc_dt::uint64 discrete_value) const;

	// events
	inline sc_kernel_event *notify(sc_event *e) ALWAYS_INLINE;
	inline sc_timed_kernel_event *notify(sc_event *e, const sc_time& t) ALWAYS_INLINE;
	
	// wait
	inline void wait() ALWAYS_INLINE;
	inline void wait(int n) ALWAYS_INLINE;
	inline void wait(const sc_event& e) ALWAYS_INLINE;
	inline void wait(const sc_event_and_list& el) ALWAYS_INLINE;
	inline void wait(const sc_event_or_list& el) ALWAYS_INLINE;
	inline void wait(const sc_time& t) ALWAYS_INLINE;
	inline void wait(const sc_time& t, const sc_event& e) ALWAYS_INLINE;
	inline void wait(const sc_time& t, const sc_event_and_list& el) ALWAYS_INLINE;
	inline void wait(const sc_time& t, const sc_event_or_list& el) ALWAYS_INLINE;

	// next_trigger
	inline void next_trigger() ALWAYS_INLINE;
	inline void next_trigger(const sc_event& e) ALWAYS_INLINE;
	inline void next_trigger(const sc_event_and_list& el) ALWAYS_INLINE;
	inline void next_trigger(const sc_event_or_list& el) ALWAYS_INLINE;
	inline void next_trigger(const sc_time& t) ALWAYS_INLINE;
	inline void next_trigger(const sc_time& t, const sc_event& e) ALWAYS_INLINE;
	inline void next_trigger(const sc_time& t, const sc_event_and_list& el) ALWAYS_INLINE;
	inline void next_trigger(const sc_time& t, const sc_event_or_list& el) ALWAYS_INLINE;
	
	// processes
	inline void trigger(sc_thread_process *thread_process) ALWAYS_INLINE;
	inline void trigger(sc_method_process *method_process) ALWAYS_INLINE;
	
	// primitive channels
	inline void request_update(sc_prim_channel *prim_channel) ALWAYS_INLINE;
	
	inline const sc_time& get_current_time_stamp() const ALWAYS_INLINE;
	sc_process_handle get_current_process_handle() const;
	bool is_unwinding() const;
	
	void set_stop_mode(sc_stop_mode mode);
	sc_stop_mode get_stop_mode() const;
	void stop();
	void pause();
	
	sc_status get_status() const;
	bool is_start_of_simulation_invoked() const;
	bool is_end_of_simulation_invoked() const;
	bool is_running() const;
	sc_dt::uint64 get_delta_count() const;
	bool pending_activity_at_current_time() const;
	bool pending_activity_at_future_time() const;
	bool pending_activity() const;
	sc_time time_to_pending_activity() const;
	
	const std::vector<sc_object*>& get_top_level_objects() const;
	const std::vector<sc_event*>& get_top_level_events() const;
	sc_object *find_object(const char* name) const;
	sc_event *find_event(const char *name) const;
	bool hierarchical_name_exists(const char *name) const;
	const char* gen_unique_name( const char* seed ) const;
	
	// debug stuff
	void dump_hierarchy(std::ostream& os) const;
protected:
private:
	friend int sc_elab_and_sim(int _argc, char* _argv[]);
	
	// elaboration
	std::stack<sc_module_name *> module_name_stack;
	std::stack<sc_object *> object_stack;

	// unique name map: name_#
	mutable std::map<std::string, unsigned int> unique_name_map;

	// all simulation objects
	std::vector<sc_object *> top_level_objects;
	std::vector<sc_event *> top_level_events;
	std::map<std::string, sc_object *> object_registry;
	std::map<std::string, sc_event *> event_registry;
	std::vector<sc_module *> module_table;
	std::vector<sc_port_base *> port_table;
	std::vector<sc_export_base *> export_table;
	std::vector<sc_prim_channel *> prim_channel_table;
	std::vector<sc_thread_process *> thread_process_table;
	std::vector<sc_method_process *> method_process_table;
	std::set<sc_process_handle> process_handle_table;

	// time resolution management
	bool time_resolution_fixed_by_user;
	mutable bool time_resolution_fixed;
	int time_resolution_scale_factors_table_base_index;
	static const int TIME_RESOLUTION_SCALE_FACTORS_TABLE_SIZE = (3 * SC_SEC) + 1;
	double time_resolution_scale_factors_table[TIME_RESOLUTION_SCALE_FACTORS_TABLE_SIZE];
	sc_time max_time;
	
	// discrete event simulation kernel
	sc_object *current_object;
	sc_object *current_writer;
	sc_thread_process *current_thread_process;
	sc_method_process *current_method_process;
	sc_time current_time_stamp;                                        // current time stamp
	sc_allocator<sc_kernel_event> kernel_events_allocator;             // kernel events (delta event) allocator
	sc_allocator<sc_timed_kernel_event> timed_kernel_events_allocator; // timed kernel events (timed event) allocator	
	std::unordered_set<sc_thread_process *> runnable_thread_processes;         // SC_THREAD/SC_CTHREAD processes to wake-up
	std::unordered_set<sc_method_process *> runnable_method_processes;         // SC_METHOD processes to wake-up
	std::unordered_set<sc_prim_channel *> updatable_prim_channels;             // primitive channels to update
	std::unordered_set<sc_kernel_event *> delta_events;                        // notified delta events set 
	std::multimap<sc_time, sc_timed_kernel_event *> schedule;          // notified timed events set
	std::unordered_set<sc_thread_process *> terminated_thread_processes;
	std::unordered_set<sc_method_process *> terminated_method_processes;
	bool user_requested_stop;
	bool user_requested_pause;
	sc_stop_mode stop_mode;
	sc_status status;
	bool initialized;
	bool end_of_simulation_invoked;
	bool start_of_simulation_invoked;
	sc_dt::uint64 delta_count;
	
	void report_before_end_of_elaboration();
	void report_end_of_elaboration();
	void report_start_of_simulation();
	void report_end_of_simulation();
	
	static sc_kernel *kernel;

	sc_object *find_object(sc_object *parent_object, const char* name);	
};

int sc_elab_and_sim(int argc, char* argv[]);
int sc_argc();
const char* const* sc_argv();

void sc_start();
void sc_start(const sc_time& duration, sc_starvation_policy p = SC_RUN_TO_TIME);
void sc_start(double duration, sc_time_unit tu, sc_starvation_policy p = SC_RUN_TO_TIME);


void sc_pause();

extern void sc_set_stop_mode( sc_stop_mode mode );
extern sc_stop_mode sc_get_stop_mode();

void sc_stop();
inline const sc_time& sc_time_stamp() ALWAYS_INLINE;
sc_process_handle sc_get_current_process_handle();
bool sc_is_unwinding();
const sc_dt::uint64 sc_delta_count();
bool sc_is_running();
bool sc_pending_activity_at_current_time();
bool sc_pending_activity_at_future_time();
bool sc_pending_activity();
sc_time sc_time_to_pending_activity();
sc_status sc_get_status();
bool sc_start_of_simulation_invoked();
bool sc_end_of_simulation_invoked();
const std::vector<sc_object*>& sc_get_top_level_objects();
sc_object* sc_find_object( const char* );
bool sc_hierarchical_name_exists(const char *name);
const char* sc_gen_unique_name( const char* seed );
const std::vector<sc_event*>& sc_get_top_level_events();
sc_event* sc_find_event( const char* );

void next_trigger();
void next_trigger( const sc_event& );
void next_trigger( const sc_event_or_list & );
void next_trigger( const sc_event_and_list & );
void next_trigger( const sc_time& );
void next_trigger( double , sc_time_unit );
void next_trigger( const sc_time& , const sc_event& );
void next_trigger( double , sc_time_unit , const sc_event& );
void next_trigger( const sc_time& , const sc_event_or_list & );
void next_trigger( double , sc_time_unit , const sc_event_or_list & );
void next_trigger( const sc_time& , const sc_event_and_list & );
void next_trigger( double , sc_time_unit , const sc_event_and_list & );

void wait();
void wait( int );
void wait( const sc_event& );
void wait( const sc_event_or_list & );
void wait( const sc_event_and_list & );
void wait( const sc_time& );
void wait( double , sc_time_unit );
void wait( const sc_time& , const sc_event& );
void wait( double , sc_time_unit , const sc_event& );
void wait( const sc_time& , const sc_event_or_list & );
void wait( double , sc_time_unit , const sc_event_or_list & );
void wait( const sc_time& , const sc_event_and_list & );
void wait( double , sc_time_unit , const sc_event_and_list & );

/////////////////////////////// sc_kernel ////////////////////////////////

inline sc_kernel_event *sc_kernel::notify(sc_event *e)
{
// 	std::cerr << "sc_kernel::notify(" << e->name() << ")" << std::endl;
	sc_kernel_event *kernel_event = kernel_events_allocator.allocate();
	kernel_event->initialize(e);
	delta_events.insert(kernel_event);
	return kernel_event;
}

inline sc_timed_kernel_event *sc_kernel::notify(sc_event *e, const sc_time& t)
{
// 	std::cerr << "sc_kernel::notify(" << e->name() << ", " << t << ")" << std::endl;
	sc_time kernel_event_time = current_time_stamp;
	kernel_event_time += t;
	sc_timed_kernel_event *timed_kernel_event = timed_kernel_events_allocator.allocate();
	timed_kernel_event->initialize(e, kernel_event_time);
	schedule.insert(std::pair<sc_time, sc_timed_kernel_event *>(kernel_event_time, timed_kernel_event));
	return timed_kernel_event;
}

inline void sc_kernel::wait()
{
	if(!current_thread_process) throw std::runtime_error("calling wait from something not an SC_THREAD/SC_CTHREAD process");
	current_thread_process->wait();
}

inline void sc_kernel::wait(int n)
{
	if(!current_thread_process) throw std::runtime_error("calling wait from something not an SC_THREAD/SC_CTHREAD process");
	current_thread_process->wait(n);
}

inline void sc_kernel::wait(const sc_event& e)
{
	if(!current_thread_process) throw std::runtime_error("calling wait from something not an SC_THREAD/SC_CTHREAD process");
	current_thread_process->wait(e);
}

inline void sc_kernel::wait(const sc_event_and_list& el)
{
	if(!current_thread_process) throw std::runtime_error("calling wait from something not an SC_THREAD/SC_CTHREAD process");
	current_thread_process->wait(el);
}

inline void sc_kernel::wait(const sc_event_or_list& el)
{
	if(!current_thread_process) throw std::runtime_error("calling wait from something not an SC_THREAD/SC_CTHREAD process");
	current_thread_process->wait(el);
}

inline void sc_kernel::wait(const sc_time& t)
{
	if(!current_thread_process) throw std::runtime_error("calling wait from something not an SC_THREAD/SC_CTHREAD process");
	current_thread_process->wait(t);
}

inline void sc_kernel::wait(const sc_time& t, const sc_event& e)
{
	if(!current_thread_process) throw std::runtime_error("calling wait from something not an SC_THREAD/SC_CTHREAD process");
	current_thread_process->wait(t, e);
}

inline void sc_kernel::wait(const sc_time& t, const sc_event_and_list& el)
{
	if(!current_thread_process) throw std::runtime_error("calling wait from something not an SC_THREAD/SC_CTHREAD process");
	current_thread_process->wait(t, el);
}

inline void sc_kernel::wait(const sc_time& t, const sc_event_or_list& el)
{
	if(!current_thread_process) throw std::runtime_error("calling wait from something not an SC_THREAD/SC_CTHREAD process");
	current_thread_process->wait(t, el);
}

inline void sc_kernel::next_trigger()
{
	if(!current_method_process) throw std::runtime_error("calling next_trigger from something not an SC_METHOD process");
	current_method_process->next_trigger();
}

inline void sc_kernel::next_trigger(const sc_event& e)
{
	if(!current_method_process) throw std::runtime_error("calling next_trigger from something not an SC_METHOD process");
	current_method_process->next_trigger(e);
}

inline void sc_kernel::next_trigger(const sc_event_and_list& el)
{
	if(!current_method_process) throw std::runtime_error("calling next_trigger from something not an SC_METHOD process");
	current_method_process->next_trigger(el);
}

inline void sc_kernel::next_trigger(const sc_event_or_list& el)
{
	if(!current_method_process) throw std::runtime_error("calling next_trigger from something not an SC_METHOD process");
	current_method_process->next_trigger(el);
}

inline void sc_kernel::next_trigger(const sc_time& t)
{
	if(!current_method_process) throw std::runtime_error("calling next_trigger from something not an SC_METHOD process");
	current_method_process->next_trigger(t);
}

inline void sc_kernel::next_trigger(const sc_time& t, const sc_event& e)
{
	if(!current_method_process) throw std::runtime_error("calling next_trigger from something not an SC_METHOD process");
	current_method_process->next_trigger(t, e);
}

inline void sc_kernel::next_trigger(const sc_time& t, const sc_event_and_list& el)
{
	if(!current_method_process) throw std::runtime_error("calling next_trigger from something not an SC_METHOD process");
	current_method_process->next_trigger(t, el);
}

inline void sc_kernel::next_trigger(const sc_time& t, const sc_event_or_list& el)
{
	if(!current_method_process) throw std::runtime_error("calling next_trigger from something not an SC_METHOD process");
	current_method_process->next_trigger(t, el);
}

inline void sc_kernel::trigger(sc_thread_process *thread_process)
{
	if(!thread_process->trigger_requested)
	{
		runnable_thread_processes.insert(thread_process);
		thread_process->trigger_requested = true;
	}
}

inline void sc_kernel::trigger(sc_method_process *method_process)
{
// 	std::cerr << method_process->name() << " is runnable" << std::endl;
	if(!method_process->trigger_requested)
	{
		runnable_method_processes.insert(method_process);
		method_process->trigger_requested = true;
	}
}

inline void sc_kernel::request_update(sc_prim_channel *prim_channel)
{
	if(!prim_channel->update_requested)
	{
		updatable_prim_channels.insert(prim_channel);
		prim_channel->update_requested = true;
	}
}

inline const sc_time& sc_kernel::get_current_time_stamp() const
{
	return current_time_stamp;
}

const sc_time& sc_time_stamp()
{
	return sc_kernel::get_kernel()->get_current_time_stamp();
}

} // end of namespace sc_core

#endif
