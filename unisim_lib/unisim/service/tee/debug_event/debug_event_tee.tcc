
/*
 *  Copyright (c) 2009,
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
 * Authors: Reda   Nouacer  (reda.nouacer@cea.fr)
 */

#include "unisim/service/tee/debug_event/debug_event_tee.hh"

namespace unisim {
namespace service {
namespace tee {
namespace debug_event {

template <class ADDRESS, uint8_t size>
DebugEventTee<ADDRESS, size>::DebugEventTee(const char* name, Object *parent) :
	Object(name, parent)
	, Service<DebugEventListener<ADDRESS> >(name, parent)
	, Client<DebugEventListener<ADDRESS> >(name, parent)
	, Service<DebugEventTrigger<ADDRESS> >(name, parent)
	, Client<DebugEventTrigger<ADDRESS> >(name, parent)

	, debug_event_listener_export("debug-event-listener-export", this)
	, debug_event_trigger_export("debug-event-trigger-export", this)
	, debug_event_trigger_import("debug-event-trigger-import", this)

{

	for (uint8_t i=0; i<size; i++) {
		std::ostringstream out;
		out << "debug-event-listener-import-" << i;
		debug_event_listener_import[i] = new ServiceImport<DebugEventListener<ADDRESS> >(out.str().c_str(), this);
	}

}

template <class ADDRESS, uint8_t size>
DebugEventTee<ADDRESS, size>::~DebugEventTee() {
	for (uint8_t i=0; i<size; i++) {
		if (debug_event_listener_import[i]) {
			delete debug_event_listener_import[i];
			debug_event_listener_import[i] = NULL;
		}
	}

}

// interface of Event_Listener

template <class ADDRESS, uint8_t size>
void DebugEventTee<ADDRESS, size>::OnDebugEvent(const unisim::util::debug::Event<ADDRESS>& event)
{
	for (uint8_t i=0; (i < size); i++) {
		if (*debug_event_listener_import[i]) {
			(*debug_event_listener_import[i])->OnDebugEvent(event);
		}
	}
}

// interface of Event_Trigger

template <class ADDRESS, uint8_t size>
bool DebugEventTee<ADDRESS, size>::Listen(const unisim::util::debug::Event<ADDRESS>& event)
{
	return debug_event_trigger_import->Listen(event);
}

template <class ADDRESS, uint8_t size>
bool DebugEventTee<ADDRESS, size>::Unlisten(const unisim::util::debug::Event<ADDRESS>& event)
{
	return debug_event_trigger_import->Unlisten(event);
}

template <class ADDRESS, uint8_t size>
bool DebugEventTee<ADDRESS, size>::IsEventListened(const unisim::util::debug::Event<ADDRESS>& event) const
{
	return debug_event_trigger_import->IsEventListened(event);
}

template <class ADDRESS, uint8_t size>
void DebugEventTee<ADDRESS, size>::EnumerateListenedEvents(std::list<const unisim::util::debug::Event<ADDRESS> *>& lst, typename unisim::util::debug::Event<ADDRESS>::Type ev_type) const
{
	debug_event_trigger_import->EnumerateListenedEvents(lst, ev_type);
}

} // end debug_event
} // end tee 
} // end service
} // end unisim 



