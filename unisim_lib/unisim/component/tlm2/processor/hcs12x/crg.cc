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
 * Authors: Reda   Nouacer  (reda.nouacer@cea.fr)
 */

#include <unisim/component/tlm2/processor/hcs12x/crg.hh>

namespace unisim {
namespace component {
namespace tlm2 {
namespace processor {
namespace hcs12x {

CRG::CRG(const sc_module_name& name, Object *parent) :
	Object(name, parent),
	sc_module(name),
	slave_socket("slave_socket"),
	Service<Memory<service_address_t> >(name, parent),
	Client<Memory<service_address_t> >(name, parent),
	Client<TrapReporting>(name, parent),
	trap_reporting_import("trap_reproting_import", this),
	memory_export("memory_export", this),
	memory_import("memory_import", this),
	baseAddress(0x0034), // MC9S12XDP512V2 - CRG baseAddress
	param_baseAddress("base-address", this, baseAddress),
	oscillator_clock_int(0),
	param_oscillator_clock_int("oscillator-clock", this, oscillator_clock_int),
	pll_clock_int(0),
	param_pll_clock_int("pll-clock", this, pll_clock_int)

{

	slave_socket.register_b_transport(this, &CRG::read_write);

	SC_HAS_PROCESS(CRG);

	SC_THREAD(Run);

}

CRG::~CRG() {

}

void CRG::Run() {

	while (true) {

		wait();
	}
}

void CRG::read_write( tlm::tlm_generic_payload& trans, sc_time& delay )
{
	tlm::tlm_command cmd = trans.get_command();
	sc_dt::uint64 address = trans.get_address();
	uint8_t* data_ptr = (uint8_t *)trans.get_data_ptr();

	assert(address >= baseAddress);

	if (cmd == tlm::TLM_READ_COMMAND) {
		read(address - baseAddress, *data_ptr);
	} else if (cmd == tlm::TLM_WRITE_COMMAND) {
		write(address - baseAddress, *data_ptr);
	}

	trans.set_response_status( tlm::TLM_OK_RESPONSE );
}

void CRG::read(uint8_t offset, uint8_t &value) {

	switch (offset) {
	default: ;
	}
}

void CRG::write(uint8_t offset, uint8_t val) {

	switch (offset) {
	default: ;
	}
}

//=====================================================================
//=                  Client/Service setup methods                     =
//=====================================================================

bool CRG::Setup() {

	/**
	 * TODO:
	 *  - One of the output of the CRG is the BusClock (hereafter fsb_cycle_time).
	 *  - In fact I have to connect all the others component to the CRG::BusClock output.
	 *  - If (PLLSEL == 1) the BusClock = PLLCLK / 2;
	 *  - PLLCLK = 2 * OSCCLK * (SYNR + 1) / (REFDV + 1)
	 */

	pll_clock = sc_time((double) pll_clock_int, SC_PS);

	oscillator_clock = sc_time((double) oscillator_clock_int, SC_PS);

	Reset();

	return true;
}

void CRG::OnDisconnect() {
}

void CRG::Reset() {
}


//=====================================================================
//=             memory interface methods                              =
//=====================================================================

bool CRG::ReadMemory(service_address_t addr, void *buffer, uint32_t size) {

	return false;
}

bool CRG::WriteMemory(service_address_t addr, const void *buffer, uint32_t size) {

	return false;
}


} // end of namespace hcs12x
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

