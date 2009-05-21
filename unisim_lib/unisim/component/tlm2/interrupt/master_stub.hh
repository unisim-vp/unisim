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
 * Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 */

#ifndef __UNISIM_COMPONENT_TLM2_INTERRUPT_MASTER_STUB_HH__
#define __UNISIM_COMPONENT_TLM2_INTERRUPT_MASTER_STUB_HH__

#include <systemc.h>
#include <tlm.h>
#include <tlm_utils/simple_initiator_socket.h>

#include "unisim/kernel/service/service.hh"
#include "unisim/component/tlm2/interrupt/types.hh"

namespace unisim {
namespace component {
namespace tlm2 {
namespace interrupt {

using unisim::kernel::service::Object;

class InterruptMasterStub :
	public Object,
	public sc_module
{
public:
	tlm_utils::simple_initiator_socket<InterruptMasterStub, 1, InterruptProtocolTypes> out_interrupt;

	InterruptMasterStub(const sc_module_name& name, Object *parent = 0);
	virtual ~InterruptMasterStub();

	virtual bool Setup();

private:
	/* START: callback methods for the out_interrupt sockets */
	tlm::tlm_sync_enum OutInterruptNb(TLMInterruptPayload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t);
	void OutInterruptDMI(sc_dt::uint64 start_range, sc_dt::uint64 end_range);
	/* END: callback methods for the out_interrupt sockets */
};

} // end of namespace interrupt
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_INTERRUPT_MASTER_STUB_HH__
