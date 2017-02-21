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

#ifndef __UNISIM_SERVICE_TEE_DEBUG_EVENT_HH__
#define __UNISIM_SERVICE_TEE_DEBUG_EVENT_HH__

#include <stdint.h>

#include <unisim/kernel/service/service.hh>

#include <unisim/service/interfaces/debug_event.hh>

namespace unisim {
namespace service {
namespace tee {
namespace debug_event {


using unisim::service::interfaces::DebugEventListener;
using unisim::service::interfaces::DebugEventTrigger;

using unisim::kernel::service::Object;
using unisim::kernel::service::Client;
using unisim::kernel::service::Service;
using unisim::kernel::service::ServiceExport;
using unisim::kernel::service::ServiceImport;

template <class ADDRESS, uint8_t size = 16 >
class DebugEventTee :
	  public Service<DebugEventListener<ADDRESS> >
	, public Client<DebugEventListener<ADDRESS> >
	, public Service<DebugEventTrigger<ADDRESS> >
	, public Client<DebugEventTrigger<ADDRESS> >

{
public:

	ServiceExport<DebugEventListener<ADDRESS> > debug_event_listener_export;        // Debuger->debug_event_listener_import >> Tee->debug_event_listener_export
	ServiceImport<DebugEventListener<ADDRESS> > *debug_event_listener_import[size]; // Tee->debug_event_listener_import >> Client->debug_event_listener_export

	ServiceExport<DebugEventTrigger<ADDRESS> > debug_event_trigger_export; // Client->debug_event_trigger_import >> Tee->debug_event_trigger_export
	ServiceImport<DebugEventTrigger<ADDRESS> > debug_event_trigger_import; // Tee->debug_event_trigger_import >> Debuger->debug_event_trigger_export

	DebugEventTee(const char* name, Object *parent = 0);
	~DebugEventTee();

	// interface of Event_Listener
	void OnDebugEvent(const unisim::util::debug::Event<ADDRESS>& event);

	// interface of Event_Trigger
	bool Listen(const unisim::util::debug::Event<ADDRESS>& event);

	bool Unlisten(const unisim::util::debug::Event<ADDRESS>& event);

	bool IsEventListened(const unisim::util::debug::Event<ADDRESS>& event) const;

	void EnumerateListenedEvents(std::list<const unisim::util::debug::Event<ADDRESS> *>& lst, typename unisim::util::debug::Event<ADDRESS>::Type ev_type = unisim::util::debug::Event<ADDRESS>::EV_UNKNOWN) const;

};

} // end debug_event
} // end tee 
} // end service
} // end unisim 


#endif


