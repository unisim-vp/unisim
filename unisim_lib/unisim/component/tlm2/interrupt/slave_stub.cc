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

#include "unisim/component/tlm2/interrupt/slave_stub.hh"

using unisim::component::tlm2::interrupt::InterruptSlaveStub;

InterruptSlaveStub ::
InterruptSlaveStub(const sc_module_name& name, Object *parent) :
	Object(name, parent),
	sc_module(name),
	in_interrupt("in_interrupt")
{
	SC_HAS_PROCESS(InterruptSlaveStub);

	in_interrupt.register_nb_transport_fw(this, &InterruptSlaveStub::InInterruptNb);
	in_interrupt.register_b_transport(this, &InterruptSlaveStub::InInterruptB);
	in_interrupt.register_transport_dbg(this, &InterruptSlaveStub::InInterruptDbg);
	in_interrupt.register_get_direct_mem_ptr(this, &InterruptSlaveStub::InInterruptDmi);
}

InterruptSlaveStub ::
~InterruptSlaveStub()
{
}

bool
InterruptSlaveStub ::
Setup()
{
	return true;
}

tlm::tlm_sync_enum
InterruptSlaveStub ::
InInterruptNb(TLMInterruptPayload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t)
{
	// nothing to do, we are a stub
	return tlm::TLM_COMPLETED;
}

void 
InterruptSlaveStub ::
InInterruptB(TLMInterruptPayload& trans, sc_core::sc_time& t)
{
	// nothing to do, we are a stub
}

unsigned int
InterruptSlaveStub ::
InInterruptDbg(TLMInterruptPayload& trans)
{
	// nothing to do, we are a stub
	return 0;
}

bool
InterruptSlaveStub ::
InInterruptDmi(TLMInterruptPayload& trans, tlm::tlm_dmi& dmi)
{
	// nothing to do, we are a stub
	return false;
}

