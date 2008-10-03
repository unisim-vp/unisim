/*
 *  Copyright (c) 2008,
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
 * Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 */
 
#ifndef __UNISIM_COMPONENT_TLM2_INTERCONNECT_GENERIC_ROUTER_ROUTER_DISPATCHER_TCC__
#define __UNISIM_COMPONENT_TLM2_INTERCONNECT_GENERIC_ROUTER_ROUTER_DISPATCHER_TCC__

namespace unisim {
namespace component {
namespace tlm2 {
namespace interconnect {
namespace generic_router {

template<typename OWNER, class CONFIG>
RouterDispatcher<OWNER, CONFIG>::
RouterDispatcher(const sc_module_name &name, unsigned int id, const sc_core::sc_time &cycle_time, OWNER *owner, cb_t cb) :
	sc_module(name),
	m_id(id),
	m_cycle_time(cycle_time),
	m_queue(this, &RouterDispatcher<OWNER, CONFIG>::QueueCB),
	m_owner(owner),
	m_cb(cb),
	ready_queue() {
}

template<typename OWNER, class CONFIG>
RouterDispatcher<OWNER, CONFIG>::
~RouterDispatcher() {
}

template<typename OWNER, class CONFIG>
void
RouterDispatcher<OWNER, CONFIG>::
Push(transaction_type &trans, const sc_core::sc_time &time) {
	phase_type phase = tlm::BEGIN_REQ;
	m_queue.notify(trans, phase, time);
}

template<typename OWNER, class CONFIG>
void
RouterDispatcher<OWNER, CONFIG>::
Completed(const sc_core::sc_time &time) {
}

template<typename OWNER, class CONFIG>
void
RouterDispatcher<OWNER, CONFIG>::
QueueCB(transaction_type &trans, const phase_type &phase) {
	(m_owner->*m_cb)(m_id, trans);
}

} // end of namespace generic_router
} // end of namespace interconnect
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_INTERCONNECT_GENERIC_ROUTER_ROUTER_DISPATCHER_TCC__

