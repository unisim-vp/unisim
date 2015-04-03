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
#include <stack>
#include <vector>
#include <iosfwd>

namespace sc_core {

class sc_kernel
{
public:
	sc_kernel();
	
	void push_module_name(sc_module_name *module_name);
	void pop_module_name();
	sc_module_name *get_top_of_module_name_stack() const;
	
	static sc_kernel *get_kernel();
	
	void begin_object(sc_object *object);
	void end_object();
	sc_object *get_current_object() const;
	sc_thread_process *get_current_thread_process() const;

	sc_thread_process *create_thread_process(const char *name, sc_process_owner *process_owner, sc_process_owner_method_ptr process_owner_method_ptr);

	void initialize();
	void start();
	
	void add_module(sc_module *module);
	void add_thread_process(sc_thread_process *thread_process);
	
	// time resolution management
	void set_time_resolution(double v, sc_time_unit tu, bool user);
	sc_dt::uint64 get_time_discrete_value(double d, sc_time_unit tu) const;
	sc_time get_time_resolution() const;
	const sc_time& get_max_time() const;
	void print_time(std::ostream& os, const sc_time& t) const;
	double time_discrete_value_to_seconds(sc_dt::uint64 discrete_value) const;

protected:
private:
	std::stack<sc_module_name *> module_name_stack;
	std::stack<sc_object *> object_stack;
	static sc_kernel *kernel;
	sc_thread_process *current_thread_process;
	
	std::vector<sc_module *> module_table;
	std::vector<sc_thread_process *> thread_process_table;

	// time resolution management
	bool time_resolution_fixed_by_user;
	bool time_resolution_fixed;
	int time_resolution_scale_factors_table_base_index;
	static const int TIME_RESOLUTION_SCALE_FACTORS_TABLE_SIZE = (3 * SC_SEC) + 1;
	double time_resolution_scale_factors_table[TIME_RESOLUTION_SCALE_FACTORS_TABLE_SIZE];
	
	sc_time max_time;
};

int sc_elab_and_sim(int argc, char* argv[]);
int sc_argc();
const char* const* sc_argv();

enum sc_starvation_policy
{
	SC_RUN_TO_TIME,
	SC_EXIT_ON_STARVATION
};

void sc_start();
void sc_start(const sc_time& duration, sc_starvation_policy p = SC_RUN_TO_TIME);
void sc_start(double duration, sc_time_unit tu, sc_starvation_policy p = SC_RUN_TO_TIME);

} // end of namespace sc_core

#endif
