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
#include <ieee1666/kernel/spawn.h>
#include <ieee1666/kernel/kernel.h>
#include <ieee1666/kernel/event.h>
#include <ieee1666/kernel/interface.h>
#include <ieee1666/kernel/port.h>
#include <ieee1666/kernel/event_finder.h>

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

sc_process::sc_process(const char *_name, sc_process_owner *_process_owner, sc_process_owner_method_ptr _process_owner_method_ptr, sc_curr_proc_kind _process_kind, const sc_spawn_options *spawn_options)
	: sc_object(_name)
	, process_owner(_process_owner)
	, process_owner_method_ptr(_process_owner_method_ptr)
	, process_kind(_process_kind)
	, flag_dynamic(sc_kernel::get_kernel()->get_status() > SC_END_OF_ELABORATION)
	, flag_dont_initialize((process_kind == SC_CTHREAD_PROC_) ? true : (spawn_options ? spawn_options->get_flag_dont_initialize() : false))
	, automatic_process_owner(process_owner->is_automatic())
	, trigger_requested(false)
	, ref_count(1)
	, enabled(true)
	, suspended(false)
	, runnable_on_resuming(false)
{
	add_static_sensitivity(spawn_options);
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

bool sc_process::dont_initialize() const
{
	return flag_dont_initialize;
}

const char *sc_process::kind() const
{
	return "sc_process";
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

void sc_process::add_static_sensitivity(const sc_interface& itf)
{
	const sc_event& event = itf.default_event();
	add_static_sensitivity(event);
}

void sc_process::add_static_sensitivity(const sc_port_base& port)
{
	port.add_process_statically_sensitive_to_port(this);
}

void sc_process::add_static_sensitivity(const sc_export_base& exp)
{
	throw std::runtime_error("unimplemented function: sc_process sensitive to sc_export");
}

void sc_process::add_static_sensitivity(const sc_event_finder& event_finder)
{
	event_finder.get_port().add_process_statically_sensitive_to_event_finder(this, event_finder);
}

void sc_process::remove_static_sensitivity(const sc_event& e)
{
	std::list<const sc_event *>::iterator it;
	
	for(it = static_sensitivity.begin(); it != static_sensitivity.end();)
	{
		const sc_event *event = *it;
		if(event == &e)
		{
			it = static_sensitivity.erase(it); 
		}
		else
		{
			it++;
		}
	}
}

void sc_process::add_static_sensitivity(const sc_spawn_options *spawn_options)
{
	unsigned int i;
	
	const std::vector<const sc_event *>& sensitive_events = spawn_options->get_sensitive_events();
	unsigned int num_sensitive_events = sensitive_events.size();
	for(i = 0; i <num_sensitive_events; i++)
	{
		add_static_sensitivity(*sensitive_events[i]);
	}

	const std::vector<const sc_port_base *>& sensitive_ports = spawn_options->get_sensitive_ports();
	unsigned int num_sensitive_ports = sensitive_ports.size();
	for(i = 0; i <num_sensitive_ports; i++)
	{
		add_static_sensitivity(*sensitive_ports[i]);
	}

	const std::vector<const sc_export_base *>& sensitive_exports = spawn_options->get_sensitive_exports();
	unsigned int num_sensitive_exports = sensitive_exports.size();
	for(i = 0; i <num_sensitive_exports; i++)
	{
		add_static_sensitivity(*sensitive_exports[i]);
	}

	const std::vector<const sc_interface *>& sensitive_interfaces = spawn_options->get_sensitive_interfaces();
	unsigned int num_sensitive_interfaces = sensitive_interfaces.size();
	for(i = 0; i <num_sensitive_interfaces; i++)
	{
		add_static_sensitivity(*sensitive_interfaces[i]);
	}

	const std::vector<const sc_event_finder *>& sensitive_event_finders = spawn_options->get_sensitive_event_finders();
	unsigned int num_sensitive_event_finders = sensitive_event_finders.size();
	for(i = 0; i <num_sensitive_event_finders; i++)
	{
		add_static_sensitivity(*sensitive_event_finders[i]);
	}
}

void sc_process::clear_static_sensitivity()
{
	std::list<const sc_event *>::iterator it;
	
	for(it = static_sensitivity.begin(); it != static_sensitivity.end(); it++)
	{
		const sc_event *event = *it;
		
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
/*
void sc_process::process_spawn_options(const spawn_options *spawn_options)
{
	if(spawn_options)
	{
		const std::vector<const sc_event *>& sensitive_events = get_sensitive_events();
		
		
		
		const std::vector<const sc_port_base *>& sensitive_ports = get_sensitive_ports();
		const std::vector<const sc_export_base *>& sensitive_exports = get_sensitive_exports();
		const std::vector<const sc_interface *>& sensitive_interfaces = get_sensitive_interfaces();
		const std::vector<const sc_event_finder *>& sensitive_event_finders = get_sensitive_event_finders();
	}
}*/

} // end of namespace sc_core
