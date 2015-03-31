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

namespace sc_core {

sc_kernel *sc_kernel::kernel = 0;

sc_kernel::sc_kernel()
{
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

sc_thread_process *sc_kernel::create_thread_process(const char *name, sc_process_owner *process_owner, sc_process_owner_method_ptr process_owner_method_ptr)
{
	sc_thread_process *thread_process = new sc_thread_process(name, process_owner, process_owner_method_ptr);
	
	return thread_process;
}

void sc_kernel::initialize()
{
	unsigned int n = thread_process_table.size();
	
	unsigned int i;
	for(i = 0; i < n; i++)
	{
		sc_thread_process *thread_process = thread_process_table[i];
		
		thread_process->start();
		current_thread_process = thread_process;
		thread_process->resume();
	}

	unsigned int j;
	
	for(j = 0; j < 10; j++)
	{
		for(i = 0; i < n; i++)
		{
			sc_thread_process *thread_process = thread_process_table[i];
			
			current_thread_process = thread_process;
			thread_process->resume();
		}
	}
}

void sc_kernel::start()
{
	initialize();
}

void sc_kernel::add_module(sc_module *module)
{
	module_table.push_back(module);
}

void sc_kernel::add_thread_process(sc_thread_process *thread_process)
{
	thread_process_table.push_back(thread_process);
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
	sc_kernel::get_kernel()->start();
}

void sc_start(const sc_time& duration, sc_starvation_policy p)
{
}

void sc_start(double duration,sc_time_unit tu, sc_starvation_policy p)
{
}

} // end of namespace sc_core

extern int sc_main(int argc, const char *argv[]);

int main(int argc, const char *argv[], const char *envp[])
{
	return sc_main(argc, argv);
}
