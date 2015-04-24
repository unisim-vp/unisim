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

#include <ieee1666/kernel/process.h>
#include <ieee1666/kernel/kernel.h>

namespace sc_core {

sc_process_owner::sc_process_owner(bool _automatic)
	: automatic(_automatic)
{
}

sc_process_owner::~sc_process_owner()
{
}

bool sc_process_owner::is_automatic() const
{
	return automatic;
}

sc_process::sc_process(const char *_name, sc_process_owner *_process_owner, sc_process_owner_method_ptr _process_owner_method_ptr, sc_curr_proc_kind _process_kind)
	: sc_object(_name)
	, process_owner(_process_owner)
	, process_owner_method_ptr(_process_owner_method_ptr)
	, process_kind(_process_kind)
	, flag_dynamic(sc_kernel::get_kernel()->get_status() > SC_END_OF_ELABORATION)
	, automatic_process_owner(process_owner->is_automatic())
	, ref_count(1)
	, enabled(true)
	, suspended(false)
	, runnable_on_resuming(false)
{
}

sc_process::~sc_process()
{
	clear_static_sensitivity();
	if(process_owner)
	{
		if(automatic_process_owner) delete process_owner;
	}
}

sc_curr_proc_kind sc_process::proc_kind() const
{
	return process_kind;
}

bool sc_process::dynamic() const
{
	return flag_dynamic;
}

void sc_process::call_process_owner_method()
{
	(process_owner->*process_owner_method_ptr)();
}

void sc_process::acquire()
{
	ref_count++;
}

void sc_process::release()
{
	if(--ref_count == 0)
	{
		delete this;
	}
}

void sc_process::add_static_sensitivity(const sc_event& event)
{
	static_sensitivity.push_back(&event);
	
	switch(process_kind)
	{
		case SC_NO_PROC_:
			break;
		case SC_METHOD_PROC_:
			event.add_statically_sensitive_method_process((sc_method_process *) this);
			break;
		case SC_THREAD_PROC_:
		case SC_CTHREAD_PROC_:
			event.add_statically_sensitive_thread_process((sc_thread_process *) this);
			break;
	}
}

void sc_process::clear_static_sensitivity()
{
	unsigned int i;
	unsigned int num_events = static_sensitivity.size();
	
	for(i = 0; i < num_events; i++)
	{
		const sc_event *event = static_sensitivity[i];
		
		switch(process_kind)
		{
			case SC_NO_PROC_:
				break;
			case SC_METHOD_PROC_:
				event->remove_statically_sensitive_method_process((sc_method_process *) this);
				break;
			case SC_THREAD_PROC_:
			case SC_CTHREAD_PROC_:
				event->remove_statically_sensitive_thread_process((sc_thread_process *) this);
				break;
		}
	}
	
	static_sensitivity.clear();
}

} // end of namespace sc_core
