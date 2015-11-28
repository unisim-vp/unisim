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

#include <ieee1666/kernel/sensitive.h>
#include <ieee1666/kernel/event.h>
#include <ieee1666/kernel/interface.h>
#include <ieee1666/kernel/port.h>
#include <ieee1666/kernel/event_finder.h>
#include <ieee1666/kernel/process.h>

namespace sc_core {

////////////////////////////////// sc_sensitive ///////////////////////////////////////////

sc_sensitive& sc_sensitive::operator << (const sc_event& event)
{
	sc_object *process_object = process_handle.get_process_object();
	
	if(process_object) ((sc_process *) process_object)->add_static_sensitivity(event);
// 	{
// 		sc_process *process = (sc_process *) process_object;
// 		process->add_static_sensitivity(event);
// 	}
	return *this;
}

sc_sensitive& sc_sensitive::operator << (const sc_interface& itf)
{
	sc_object *process_object = process_handle.get_process_object();
	
	if(process_object) ((sc_process *) process_object)->add_static_sensitivity(itf);
// 	{
// 		sc_process *process = (sc_process *) process_object;
// 		const sc_event& event = itf.default_event();
// 		process->add_static_sensitivity(event);
// 	}
	return *this;
}

sc_sensitive& sc_sensitive::operator << (const sc_port_base& port)
{
	sc_object *process_object = process_handle.get_process_object();

	if(process_object) ((sc_process *) process_object)->add_static_sensitivity(port); // port.add_process_statically_sensitive_to_port((sc_process *) process_object);

	return *this;
}

sc_sensitive& sc_sensitive::operator << (sc_event_finder& event_finder)
{
	sc_object *process_object = process_handle.get_process_object();
	
	if(process_object) ((sc_process *) process_object)->add_static_sensitivity(event_finder); //event_finder.get_port().add_process_statically_sensitive_to_event_finder((sc_process *) process_object, event_finder);

	return *this;
}

///////////////////////////////////////////////////////////////////////////////////////////

sc_sensitive::sc_sensitive()
	: process_handle()
{
}

sc_sensitive::~sc_sensitive()
{
}

void sc_sensitive::bind(sc_process_handle _process_handle)
{
	process_handle = _process_handle;
}

} // end of namespace sc_core
