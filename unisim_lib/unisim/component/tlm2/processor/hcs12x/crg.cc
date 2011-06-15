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
#include "unisim/util/debug/simple_register.hh"

namespace unisim {
namespace component {
namespace tlm2 {
namespace processor {
namespace hcs12x {

using unisim::util::debug::SimpleRegister;

CRG::CRG(const sc_module_name& name, Object *parent) :
	Object(name, parent)
	, sc_module(name)

	, Client<TrapReporting>(name, parent)
	, Service<Memory<service_address_t> >(name, parent)
	, Service<Registers>(name, parent)
	, Client<Memory<service_address_t> >(name, parent)

	, trap_reporting_import("trap_reporting_import", this)

	, bus_clock_socket("Bus-Clock")
	, slave_socket("slave_socket")

	, memory_export("memory_export", this)
	, memory_import("memory_import", this)
	, registers_export("registers_export", this)

	, armcop_write_enabled(false)
	, cop_timeout_reset(false)
	, cop_timeout_restart(false)

	, oscillator_clock_int(0)
	, param_oscillator_clock_int("oscillator-clock", this, oscillator_clock_int)

	, baseAddress(0x0034)
	, param_baseAddress("base-address", this, baseAddress)

	, interrupt_offset_rti(0xF0)
	, param_interrupt_offset_rti("interrupt-offset-rti", this, interrupt_offset_rti)
	, interrupt_offset_pll_lock(0xC6)
	, param_interrupt_offset_pll_lock("interrupt-offset-pll-lock", this, interrupt_offset_pll_lock)
	, interrupt_offset_self_clock_mode(0xC4)
	, param_interrupt_offset_self_clock_mode("interrupt-offset-self-clock-mode", this, interrupt_offset_self_clock_mode)
	
	, debug_enabled(false)
	, param_debug_enabled("debug-enabled", this, debug_enabled)

{

	interrupt_request(*this);

	slave_socket.register_b_transport(this, &CRG::read_write);

	SC_HAS_PROCESS(CRG);

	SC_THREAD(RunRTI);
	SC_THREAD(RunCOP);

}

CRG::~CRG() {

	// Release registers_registry
	map<string, unisim::util::debug::Register *>::iterator reg_iter;

	for(reg_iter = registers_registry.begin(); reg_iter != registers_registry.end(); reg_iter++)
	{
		if(reg_iter->second)
			delete reg_iter->second;
	}

	registers_registry.clear();

}


void CRG::read_write( tlm::tlm_generic_payload& trans, sc_time& delay )
{
	tlm::tlm_command cmd = trans.get_command();
	sc_dt::uint64 address = trans.get_address();
	uint8_t* data_ptr = (uint8_t *)trans.get_data_ptr();

	assert(address >= baseAddress);

	if (cmd == tlm::TLM_READ_COMMAND) {
		unsigned int data_length = trans.get_data_length();
		memset(data_ptr, 0, data_length);
		read(address - baseAddress, *data_ptr);
	} else if (cmd == tlm::TLM_WRITE_COMMAND) {
		write(address - baseAddress, *data_ptr);
	}

	trans.set_response_status( tlm::TLM_OK_RESPONSE );
}

bool CRG::read(uint8_t offset, uint8_t &value) {

	switch (offset) {
		case SYNR: value = synr_register & 0x3F; break;
		case REFDV: value = refdv_register & 0x3F; break;
		case CTFLG: value = 0; break;
		case CRGFLG: value = crgflg_register; break;
		case CRGINT: value = crgint_register & 0xD2; break;
		case CLKSEL: value = clksel_register & 0xCB; break;
		case PLLCTL: value = pllctl_register; break;
		case RTICTL: value = rtictl_register; break;
		case COPCTL: value = copctl_register & 0xC7; break;
		case FORBYP: value = 0; break;
		case CTCTL: value = 0; break;
		case ARMCOP: value = 0; break;
		default: return false;
	}

	return true;
}

bool CRG::write(uint8_t offset, uint8_t value) {

	switch (offset) {
		case SYNR: {
			 // if (PLLSEL == 1) then return;
			if ((clksel_register & 0x80) != 0) {
				return true;
			}

			synr_register = value & 0x3F;
			crgflg_register = crgflg_register | 0x0C; // set Lock and track bits

			compute_clock();
			updateBusClock();

		} break;
		case REFDV: {
			 // if (PLLSEL == 1) then return;
			if ((clksel_register & 0x80) != 0) {
				return true;
			}

			refdv_register = value & 0x3F;
			crgflg_register = crgflg_register | 0x1C;

			compute_clock();
			updateBusClock();
			assertInterrupt(interrupt_offset_pll_lock);
		} break;
		case CTFLG: {
			// This register is reserved for factory testing
			return true;
		} break;
		case CRGFLG: {
			/**
			 * TODO:
			 *  RTIF: set at the end of RTI period
			 *  PORF: set when a power on occurs
			 *  LVRF: set if low voltage reset
			 *  LOCKIF: is set when Lock status bit change
			 *  SCMIF: is set when SCM status bit change
			 */

			if ((value & 0x80) != 0) {
				crgflg_register = crgflg_register & 0x7F;
			}

			if ((value & 0x40) != 0) {
				crgflg_register = crgflg_register & 0xBF;
			}

			if ((value & 0x20) != 0) {
				crgflg_register = crgflg_register & 0xDF;
			}

			if ((value & 0x10) != 0) {
				crgflg_register = crgflg_register & 0xEF;
			}

			if ((value & 0x02) != 0) {
				crgflg_register = crgflg_register & 0xFD;
			}

		} break;
		case CRGINT: {
			uint8_t val = value & 0xD2;

			uint8_t old_ilaf_bit = crgint_register & 0x40;
			crgint_register = val & 0x92;
			if ((val & 0x40) == 0) {
				crgint_register = crgint_register | old_ilaf_bit;
			}

			if ((val & 0x80) != 0) {
				rti_enable_event.notify();
			}

		} break;
		case CLKSEL: {
			clksel_register = value;
			compute_clock();
			updateBusClock();

		} break;
		case PLLCTL: {
			uint8_t cme_bit = pllctl_register & 0x80;
			uint8_t pllon_bit = pllctl_register & 0x40;

			// check CRGFLG::SCM bit
			if ((crgflg_register & 0x01) != 0) {
				value = (value & 0x7F) | cme_bit;
			}

			// check CLKSEL::PLLSEL
			if ((clksel_register & 0x80) != 0) {
				value = (value & 0xBF) | pllon_bit;
			}

			pllctl_register = value;

		} break;
		case RTICTL: {
			rtictl_register = value;
			initialize_rti_counter();

		} break;
		case COPCTL: {

			// RSBCK: Anytime in special modes; write to "1" but not to "0" in all other modes
			if ((value & 0x40) == 0) {
				value = (value | (copctl_register & 0x40));
			}

			copctl_register = value & 0xE7;
			if (((copctl_register & 0x07) != 0) && ((copctl_register & 0x80) != 0)) {
				cop_enable_event.notify();
			}

		} break;
		case FORBYP: /* reserved */ break;
		case CTCTL: /* reserved */ break;
		case ARMCOP: {

			if ((copctl_register & 0x07) != 0) {
				if (((value != 0x55) && (value != 0xAA)) ||
						(((copctl_register & 0x80) != 0) && (!armcop_write_enabled)))
				{
					cop_timeout_reset = true;
				}

				uint8_t old_value = armcop_register;
				armcop_register = value;

				if ((old_value == 0x55) && (value == 0xAA)) {
					cop_timeout_restart = true;
				}
			}

		} break;
		default: return false;
	}

	return true;
}

void CRG::systemReset() {

}

void CRG::RunCOP() {

	/**
	 * Windowed COP operation is enabled by setting WCOP bit in the COPCTL register.
	 * When WCOP is set, a write to the ARMCOP register must occur in the last 25% of the selected
	 * period. A write during the first 75% of the selected period will reset the part. As long as all writes occur during
	 * this window, 0x_55 can be written as often as desired. Once 0x_AA is written after the 0x_55, the time-out logic
	 * restarts and the user must wait until the next window before writing to ARMCOP.
	 */

	while (true) {
		while (((copctl_register & 0x07) == 0) || ((copctl_register & 0x80) == 0))  {
			wait(cop_enable_event);
		}

		// select_cop_timeout
		sc_time cop_timeout;

		// is SCM bit set
		if ((crgflg_register & 0x01) != 0) {
			sc_time min_pll_clock = (2 * oscillator_clock) / 64 ;
			cop_timeout = min_pll_clock / ((2^14) * (copctl_register & 0x07));
		} else {
			cop_timeout = oscillator_clock / ((2^14) * (copctl_register & 0x07));
		}

		sc_time cop_timeout_75 = cop_timeout * 0.75;
		sc_time cop_timeout_25 = cop_timeout - cop_timeout_75;

		// "cop_timeout_reset" and "cop_timeout_restart" values change by writing in the ARMCOP register
		cop_timeout_reset = false;
		cop_timeout_restart = false;
		armcop_write_enabled = false;

		wait(cop_timeout_75);
		if (cop_timeout_reset) {
			systemReset();
		}

		armcop_write_enabled = true;

		wait(cop_timeout_25);
		if (!cop_timeout_restart) {
			systemReset();
		}

		cop_timeout_restart = false;
	}
}

void CRG::RunRTI() {
	/**
	 * - CRG generates a real-time interrupt when the selected interrupt time period elapses
	 * - RTI interrupts are locally disabled by setting the RTIE bit to 0
	 * - The RTIF is set to 1 when a timeout occurs, and is cleared to 0 by writing 1 to RTIF
	 */

	sc_time delay;

	while (true) {
		while ((crgint_register & 0x80) == 0) {
			wait(rti_enable_event);
		}

		// is SCM bit set
		if ((crgflg_register & 0x01) != 0) {
			//delay = rti_fdr * pll_clock;
			sc_time min_pll_clock = (2 * oscillator_clock) / 64 ;
			delay = rti_fdr * min_pll_clock;
		} else {
			delay = rti_fdr * oscillator_clock;
		}

		wait(delay);

		crgflg_register = crgflg_register | 0x80;
		assertInterrupt(interrupt_offset_rti);
	}
}

void CRG::assertInterrupt(uint8_t interrupt_offset) {

	if ((interrupt_offset == interrupt_offset_pll_lock) && ((crgint_register & 0x10) == 0)) return;
	if ((interrupt_offset == interrupt_offset_self_clock_mode) && ((crgint_register & 0x02) == 0)) return;

	tlm_phase phase = BEGIN_REQ;
	XINT_Payload *payload = xint_payload_fabric.allocate();

	payload->interrupt_offset = interrupt_offset;

	sc_time local_time = quantumkeeper.get_local_time();

	tlm_sync_enum ret = interrupt_request->nb_transport_fw(*payload, phase, local_time);

	payload->release();

	switch(ret)
	{
		case TLM_ACCEPTED:
			// neither payload, nor phase and local_time have been modified by the callee
			quantumkeeper.sync(); // synchronize to leave control to the callee
			break;
		case TLM_UPDATED:
			// the callee may have modified 'payload', 'phase' and 'local_time'
			quantumkeeper.set(local_time); // increase the time
			if(quantumkeeper.need_sync()) quantumkeeper.sync(); // synchronize if needed

			break;
		case TLM_COMPLETED:
			// the callee may have modified 'payload', and 'local_time' ('phase' can be ignored)
			quantumkeeper.set(local_time); // increase the time
			if(quantumkeeper.need_sync()) quantumkeeper.sync(); // synchronize if needed
			break;
	}

}

// Master methods
void CRG::invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range)
{
	// Leave this empty as it is designed for memory mapped buses
}

// Master methods
tlm_sync_enum CRG::nb_transport_bw( XINT_Payload& payload, tlm_phase& phase, sc_core::sc_time& t)
{
	if(phase == BEGIN_RESP)
	{
		payload.release();
		return TLM_COMPLETED;
	}
	return TLM_ACCEPTED;
}


void CRG::compute_clock() {

	pll_clock = 2 * oscillator_clock * (synr_register + 1) / (refdv_register +1) ;

	if ((clksel_register & 0x80) != 0) {
		bus_clock = pll_clock * 2;
	} else {
		bus_clock = oscillator_clock * 2;
	}

}

void CRG::updateBusClock() {
	// notify bus_clock update
	sc_dt::uint64 bus_clock_value = bus_clock.value();

	tlm::tlm_generic_payload* trans = payloadFabric.allocate();

	trans->set_command( tlm::TLM_WRITE_COMMAND );
	trans->set_data_ptr( (unsigned char *)&bus_clock_value);

	sc_time delay = SC_ZERO_TIME;

	for (uint8_t i = 0; i < bus_clock_socket.size(); i++) {
		trans->set_response_status( tlm::TLM_INCOMPLETE_RESPONSE );

		bus_clock_socket[i]->b_transport( *trans, delay);

		if (trans->is_response_error() )
			SC_REPORT_ERROR("CRG : ", "Response error from b_transport");

	}

	trans->release();
}

void CRG::initialize_rti_counter() {
	bool rtdec = ((rtictl_register & 0x80) != 0);
	uint8_t rtr30 = rtictl_register & 0x0F;
	uint8_t rtr64 = (rtictl_register & 0x70) >> 4;

	// RTI Frequency Divide Rate
	rti_fdr = 0;
	if (rtdec) {
		rti_fdr = (rtr30 + 1) * std::pow((double)10, 3);
		switch (rtr64) {
			case 1: rti_fdr = rti_fdr * 2; break;
			case 2: rti_fdr = rti_fdr * 5; break;
			case 3: rti_fdr = rti_fdr * 10; break;
			case 4: rti_fdr = rti_fdr * 20; break;
			case 5: rti_fdr = rti_fdr * 50; break;
			case 6: rti_fdr = rti_fdr * 100; break;
			case 7: rti_fdr = rti_fdr * 200; break;
		}
	} else {
		if (rtr64 != 0) {
			rti_fdr = (rtr30+1) * std::pow((double)2, 10+rtr64-1);
		} else {
			// disable RTI
		}
	}

}

//=====================================================================
//=                  Client/Service setup methods                     =
//=====================================================================

bool CRG::BeginSetup() {


	char buf[80];

	sprintf(buf, "%s.SYNR",name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &synr_register);

	sprintf(buf, "%s.REFDV",name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &refdv_register);

	sprintf(buf, "%s.CTFLG",name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &ctflg_register);

	sprintf(buf, "%s.CRGFLG",name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &crgflg_register);

	sprintf(buf, "%s.CRGINT",name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &crgint_register);

	sprintf(buf, "%s.CLKSEL",name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &clksel_register);

	sprintf(buf, "%s.PLLCTL",name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &pllctl_register);

	sprintf(buf, "%s.RTICTL",name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &rtictl_register);

	sprintf(buf, "%s.COPCTL",name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &copctl_register);

	sprintf(buf, "%s.FORBYP",name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &forbyp_register);

	sprintf(buf, "%s.CTCTL",name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &ctctl_register);

	sprintf(buf, "%s.ARMCOP",name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &armcop_register);

	oscillator_clock = sc_time((double) oscillator_clock_int, SC_PS);

	Reset();

	compute_clock();

	return true;
}

bool CRG::Setup(ServiceExportBase *srv_export) {

	return true;
}

bool CRG::EndSetup() {
	return true;
}

Register* CRG::GetRegister(const char *name)
{
	if(registers_registry.find(string(name)) != registers_registry.end())
		return registers_registry[string(name)];
	else
		return NULL;

}

void CRG::OnDisconnect() {
}

void CRG::Reset() {

	synr_register =  0x00;
	refdv_register = 0x00;
	ctflg_register = 0x00;
	crgflg_register = 0x00;
	crgint_register = 0x00;
	clksel_register = 0x00;
	pllctl_register = 0xF1;
	rtictl_register = 0x00;
	copctl_register = 0x00;
	forbyp_register =  0x00;
	ctctl_register = 0x00;
	armcop_register = 0x00;

}


//=====================================================================
//=             memory interface methods                              =
//=====================================================================

bool CRG::ReadMemory(service_address_t addr, void *buffer, uint32_t size) {

	service_address_t offset = addr-baseAddress;

	if (offset <= ARMCOP) {
		switch (offset) {
			case SYNR: *(uint8_t *) buffer = synr_register; break;
			case REFDV: *(uint8_t *) buffer = refdv_register; break;
			case CTFLG: *(uint8_t *) buffer = ctflg_register; break;
			case CRGFLG: *(uint8_t *) buffer = crgflg_register; break;
			case CRGINT: *(uint8_t *) buffer = crgint_register; break;
			case CLKSEL: *(uint8_t *) buffer = clksel_register; break;
			case PLLCTL: *(uint8_t *) buffer = pllctl_register; break;
			case RTICTL: *(uint8_t *) buffer = rtictl_register; break;
			case COPCTL: *(uint8_t *) buffer = copctl_register; break;
			case FORBYP: *(uint8_t *) buffer = forbyp_register; break;
			case CTCTL: *(uint8_t *) buffer = ctctl_register; break;
			case ARMCOP: *(uint8_t *) buffer = armcop_register; break;
			default: return false;
		}

		return true;
	}

	return false;
}

bool CRG::WriteMemory(service_address_t addr, const void *buffer, uint32_t size) {

	service_address_t offset = addr-baseAddress;

	if (offset <= ARMCOP) {
		return write(offset, *(uint8_t *) buffer);
	}

	return false;

}


} // end of namespace hcs12x
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

