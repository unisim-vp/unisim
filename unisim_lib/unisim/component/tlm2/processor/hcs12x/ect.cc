/*
 *  Copyright (c) 2008, 2011
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

#include <unisim/component/tlm2/processor/hcs12x/ect.hh>
#include "unisim/util/debug/simple_register.hh"

namespace unisim {
namespace component {
namespace tlm2 {
namespace processor {
namespace hcs12x {

using unisim::util::debug::SimpleRegister;

ECT::ECT(const sc_module_name& name, Object *parent) :
	Object(name, parent),
	sc_module(name),

	Client<TrapReporting>(name, parent),
	Service<Memory<service_address_t> >(name, parent),
	Service<Registers>(name, parent),
	Client<Memory<service_address_t> >(name, parent),

	trap_reporting_import("trap_reporting_import", this),

	slave_socket("slave_socket"),

	memory_export("memory_export", this),
	memory_import("memory_import", this),
	registers_export("registers_export", this),

	bus_cycle_time_int(0),
	param_bus_cycle_time_int("bus-cycle-time", this, bus_cycle_time_int),

	baseAddress(0x0040), // MC9S12XDP512V2 - ECT baseAddress
	param_baseAddress("base-address", this, baseAddress),

	interrupt_offset_channel0(0xEE),
	param_interrupt_offset_channel0("interrupt-offset-channel0", this, interrupt_offset_channel0),
	interrupt_offset_overflow(0xDE),
	param_interrupt_offset_overflow("interrupt-offset-overflow", this, interrupt_offset_overflow),

	debug_enabled(false),
	param_debug_enabled("debug-enabled", this, debug_enabled)
{

	interrupt_request(*this);
	slave_socket.register_b_transport(this, &ECT::read_write);
	bus_clock_socket.register_b_transport(this, &ECT::updateCRGClock);

	SC_HAS_PROCESS(ECT);

	SC_THREAD(run);

}

ECT::~ECT() {

	// Release registers_registry
	map<string, unisim::util::debug::Register *>::iterator reg_iter;

	for(reg_iter = registers_registry.begin(); reg_iter != registers_registry.end(); reg_iter++)
	{
		if(reg_iter->second)
			delete reg_iter->second;
	}

	registers_registry.clear();

}

void ECT::run() {

	sc_time delay = sc_time(1, SC_MS);

	while (true) {

		wait();
//		wait(delay);
//		for (int index=0; index < 8; index++) {
//			assertInterrupt(interrupt_offset_channel0 - index*2);
//		}

	}
}

void ECT::assertInterrupt(uint8_t interrupt_offset) {
	// assert ATD_SequenceComplete_Interrupt

	tlm_phase phase = BEGIN_REQ;
	XINT_Payload *payload = xint_payload_fabric.allocate();

	payload->interrupt_offset = interrupt_offset;

	sc_time local_time = quantumkeeper.get_local_time();

	tlm_sync_enum ret = interrupt_request->nb_transport_fw(*payload, phase, local_time);

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
void ECT::invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range)
{
	// Leave this empty as it is designed for memory mapped buses
}

// Master methods
tlm_sync_enum ECT::nb_transport_bw( XINT_Payload& payload, tlm_phase& phase, sc_core::sc_time& t)
{
	if(phase == BEGIN_RESP)
	{
		payload.release();
		return TLM_COMPLETED;
	}
	return TLM_ACCEPTED;
}


void ECT::read_write( tlm::tlm_generic_payload& trans, sc_time& delay )
{
	tlm::tlm_command cmd = trans.get_command();
	sc_dt::uint64 address = trans.get_address();
	unsigned char* data_ptr = trans.get_data_ptr();
	unsigned int data_length = trans.get_data_length();

	assert(address >= baseAddress);

	if (cmd == tlm::TLM_READ_COMMAND) {
		read(address - baseAddress, data_ptr, data_length);
	} else if (cmd == tlm::TLM_WRITE_COMMAND) {
		write(address - baseAddress, data_ptr, data_length);
	}

	trans.set_response_status( tlm::TLM_OK_RESPONSE );
}

bool ECT::read(uint8_t offset, unsigned char* value, uint8_t size) {

	memset(value, 0, size);

	switch (offset) {
		case TIOS: {
			*((uint8_t *)value) =  tios_register;
		} break;
		case CFORC: {
			*((uint8_t *)value) = 0x00;
		} break;
		case OC7M: {
			*((uint8_t *)value) = oc7m_register;
		} break;
		case OC7D: {
			*((uint8_t *)value) =  oc7d_register;
		} break;
		case TCNT_HIGH: {
			if (size == 2) {
				*((uint16_t *)value) = tcnt_register;
			} else {
				*((uint8_t *)value) = tcnt_register >> 8;
			}

		} break;
		case TCNT_LOW: {
			*((uint8_t *)value) = tcnt_register & 0x00FF;
		} break;
		case TSCR1: {
			*((uint8_t *)value) = tscr1_register & 0xF8;
		} break;
		case TTOF: {
			*((uint8_t *)value) = ttof_register;
		} break;
		case TCTL1: {
			if (size == 2) {
				*((uint16_t *)value) = tctl12_register;
			} else {
				*((uint8_t *)value) =  tctl12_register >> 8;
			}

		} break;
		case TCTL2: {
			*((uint8_t *)value) =  tctl12_register & 0x00FF;
		} break;
		case TCTL3: {
			if (size == 2) {
				*((uint16_t *)value) = tctl34_register;
			} else {
				*((uint8_t *)value) = tctl34_register >> 8;
			}

		} break;
		case TCTL4: {
			*((uint8_t *)value) = tctl34_register & 0x00FF;
		} break;
		case TIE: {
			*((uint8_t *)value) =  tie_register;
		} break;
		case TSCR2: {
			*((uint8_t *)value) =  tscr2_register & 0x8F;
		} break;
		case TFLG1: {
			*((uint8_t *)value) = tflg1_register;
		} break;
		case TFLG2: {
			*((uint8_t *)value) = tflg2_register;
		} break;
		case TC0_HIGH: {
			if (size == 2) {
				*((uint16_t *)value) = tc_registers[0];
			} else {
				*((uint8_t *)value) =  tc_registers[0] >> 8;
			}
		} break;
		case TC0_LOW: {
			*((uint8_t *)value) =  tc_registers[0] & 0x00FF;
		} break;
		case TC1_HIGH: {
			if (size == 2) {
				*((uint16_t *)value) = tc_registers[1];
			} else {
				*((uint8_t *)value) =  tc_registers[1] >> 8;
			}
		} break;
		case TC1_LOW: {
			*((uint8_t *)value) =  tc_registers[1] & 0x00FF;
		} break;
		case TC2_HIGH: {
			if (size == 2) {
				*((uint16_t *)value) = tc_registers[2];
			} else {
				*((uint8_t *)value) =  tc_registers[2] >> 8;
			}
		} break;
		case TC2_LOW: {
			*((uint8_t *)value) =  tc_registers[2] & 0x00FF;
		} break;
		case TC3_HIGH: {
			if (size == 2) {
				*((uint16_t *)value) = tc_registers[3];
			} else {
				*((uint8_t *)value) =  tc_registers[3] >> 8;
			}
		} break;
		case TC3_LOW: {
			*((uint8_t *)value) =  tc_registers[3] & 0x00FF;
		} break;
		case TC4_HIGH: {
			if (size == 2) {
				*((uint16_t *)value) = tc_registers[4];
			} else {
				*((uint8_t *)value) =  tc_registers[4] >> 8;
			}
		} break;
		case TC4_LOW: {
			*((uint8_t *)value) =  tc_registers[4] & 0x00FF;
		} break;
		case TC5_HIGH: {
			if (size == 2) {
				*((uint16_t *)value) = tc_registers[5];
			} else {
				*((uint8_t *)value) =  tc_registers[5] >> 8;
			}
		} break;
		case TC5_LOW: {
			*((uint8_t *)value) =  tc_registers[5] & 0x00FF;
		} break;
		case TC6_HIGH: {
			if (size == 2) {
				*((uint16_t *)value) = tc_registers[6];
			} else {
				*((uint8_t *)value) =  tc_registers[6] >> 8;
			}
		} break;
		case TC6_LOW: {
			*((uint8_t *)value) =  tc_registers[6] & 0x00FF;
		} break;
		case TC7_HIGH: {
			if (size == 2) {
				*((uint16_t *)value) = tc_registers[7];
			} else {
				*((uint8_t *)value) =  tc_registers[7] >> 8;
			}
		} break;
		case TC7_LOW: {
			*((uint8_t *)value) =  tc_registers[7] & 0x00FF;
		} break;
		case PACTL: {
			*((uint8_t *)value) =  pactl_register & 0x7F;
		} break;
		case PAFLG: {
			*((uint8_t *)value) = paflg_register & 0x03;
		} break;
		case PACN3: {
			if (size == 2) {
				*((uint16_t *)value) = pacn32_register;
			} else {
				*((uint8_t *)value) = pacn32_register >> 8;
			}
		} break;
		case PACN2: {
			*((uint8_t *)value) = pacn32_register & 0x00FF;
		} break;
		case PACN1: {
			if (size == 2) {
				*((uint16_t *)value) = pacn10_register;
			} else {
				*((uint8_t *)value) = pacn10_register >> 8;
			}
		} break;
		case PACN0: {
			*((uint8_t *)value) = pacn10_register & 0x00FF;
		} break;
		case MCCTL: {
			*((uint8_t *)value) = mcctl_register & 0xE7;
		} break;
		case MCFLG: {
			*((uint8_t *)value) = mcflg_register & 0x8F;
		} break;
		case ICPAR: {
			*((uint8_t *)value) = icpar_register & 0x0F;
		} break;
		case DLYCT: {
			*((uint8_t *)value) = dlyct_register;
		} break;
		case ICOVW: {
			*((uint8_t *)value) = icovw_register;
		} break;
		case ICSYS: {
			*((uint8_t *)value) = icsys_register;
		} break;
		case RESERVED: { /* Reserved Address */} break;
		case TIMTST: { /* Timer Test Register */} break;
		case PTPSR: {
			*((uint8_t *)value) = ptpsr_register;
		} break;
		case PTMCPSR: {
			*((uint8_t *)value) = ptmcpsr_register;
		} break;
		case PBCTL: {
			*((uint8_t *)value) = pbctl_register & 0x42;
		} break;
		case PBFLG: {
			*((uint8_t *)value) = pbflg_register & 0x02;
		} break;
		case PA3H: {
			*((uint8_t *)value) = paxh_registers[3];
		} break;
		case PA2H: {
			*((uint8_t *)value) = paxh_registers[2];
		} break;
		case PA1H: {
			*((uint8_t *)value) = paxh_registers[1];
		} break;
		case PA0H: {
			*((uint8_t *)value) = paxh_registers[0];
		} break;
		case MCCNT_HIGH: {
			if (size == 2) {
				*((uint16_t *)value) = mccnt_register;
			} else {
				*((uint8_t *)value) = mccnt_register >> 8;
			}

		} break;
		case MCCNT_LOW: {
			*((uint8_t *)value) = mccnt_register & 0x00FF;
		} break;
		case TC0H_HIGH: {
			if (size == 2) {
				*((uint16_t *)value) = tcxh_registers[0];
			} else {
				*((uint8_t *)value) = tcxh_registers[0] >> 8;
			}

		} break;
		case TC0H_LOW: {
			*((uint8_t *)value) = tcxh_registers[0] & 0x00FF;
		} break;
		case TC1H_HIGH: {
			if (size == 2) {
				*((uint16_t *)value) = tcxh_registers[1];
			} else {
				*((uint8_t *)value) = tcxh_registers[1] >> 8;
			}

		} break;
		case TC1H_LOW: {
			*((uint8_t *)value) = tcxh_registers[1] & 0x00FF;
		} break;
		case TC2H_HIGH: {
			if (size == 2) {
				*((uint16_t *)value) = tcxh_registers[2];
			} else {
				*((uint8_t *)value) = tcxh_registers[2] >> 8;
			}

		} break;
		case TC2H_LOW: {
			*((uint8_t *)value) = tcxh_registers[2] & 0x00FF;
		} break;
		case TC3H_HIGH: {
			if (size == 2) {
				*((uint16_t *)value) = tcxh_registers[3];
			} else {
				*((uint8_t *)value) = tcxh_registers[3] >> 8;
			}

		} break;
		case TC3H_LOW: {
			*((uint8_t *)value) = tcxh_registers[3] & 0x00FF;
		} break;

		default: {
			return false;
		}
	}

	return true;
}

bool ECT::write(uint8_t offset, unsigned char* value, uint8_t size) {

	switch (offset) {
		case TIOS: {
			 tios_register = *((uint8_t *)value);
		} break;
		case CFORC: {
			cforc_register = *((uint8_t *)value);
		} break;
		case OC7M: {
			oc7m_register = *((uint8_t *)value);
		} break;
		case OC7D: {
			oc7d_register = *((uint8_t *)value);
		} break;
		case TCNT_HIGH: {
			if (size == 2) {
				tcnt_register = *((uint16_t *)value);
			} else {
				tcnt_register = (tcnt_register & 0x00FF) | ((uint16_t) *((uint8_t *)value) << 8);
			}

		} break;
		case TCNT_LOW: {
			tcnt_register = (tcnt_register & 0xFF00) | *((uint8_t *)value);
		} break;
		case TSCR1: {
			tscr1_register = (tscr1_register & 0x07) | (*((uint8_t *)value) & 0xF8);
		} break;
		case TTOF: {
			ttof_register = *((uint8_t *)value);
		} break;
		case TCTL1: {
			if (size == 2) {
				tctl12_register = *((uint16_t *)value);
			} else {
				tctl12_register = (tctl12_register & 0x00FF) | ((uint16_t) *((uint8_t *)value) << 8);
			}

		} break;
		case TCTL2: {
			tctl12_register = (tctl12_register & 0xFF00) | *((uint8_t *)value);
		} break;
		case TCTL3: {
			if (size == 2) {
				tctl34_register = *((uint16_t *)value);
			} else {
				tctl34_register = (tctl34_register & 0x00FF) | ((uint16_t) *((uint8_t *)value) << 8);
			}
		} break;
		case TCTL4: {
			tctl34_register = (tctl34_register & 0xFF00) | *((uint8_t *)value);
		} break;
		case TIE: {
			tie_register = *((uint8_t *)value);
		} break;
		case TSCR2: {
			tscr2_register = (tscr2_register & 0x70) | (*((uint8_t *)value) & 0x8F);
		} break;
		case TFLG1: {
			tflg1_register = *((uint8_t *)value);
		} break;
		case TFLG2: {
			tflg2_register = (tflg2_register & 0x7F) | (*((uint8_t *)value) & 0x80);
		} break;
		case TC0_HIGH: {
			if (size == 2) {
				tc_registers[0] = *((uint16_t *)value);
			} else {
				tc_registers[0] = (tc_registers[0] & 0x00FF) | ((uint16_t) *((uint8_t *)value) << 8);
			}
		} break;
		case TC0_LOW: {
			tc_registers[0] = (tc_registers[0] & 0xFF00) | *((uint8_t *)value);
		} break;
		case TC1_HIGH: {
			if (size == 2) {
				tc_registers[1] = *((uint16_t *)value);
			} else {
				tc_registers[1] = (tc_registers[1] & 0x00FF) | ((uint16_t) *((uint8_t *)value) << 8);
			}
		} break;
		case TC1_LOW: {
			tc_registers[1] = (tc_registers[1] & 0xFF00) | *((uint8_t *)value);
		} break;
		case TC2_HIGH: {
			if (size == 2) {
				tc_registers[2] = *((uint16_t *)value);
			} else {
				tc_registers[2] = (tc_registers[2] & 0x00FF) | ((uint16_t) *((uint8_t *)value) << 8);
			}
		} break;
		case TC2_LOW: {
			tc_registers[2] = (tc_registers[2] & 0xFF00) | *((uint8_t *)value);
		} break;
		case TC3_HIGH: {
			if (size == 2) {
				tc_registers[3] = *((uint16_t *)value);
			} else {
				tc_registers[3] = (tc_registers[3] & 0x00FF) | ((uint16_t) *((uint8_t *)value) << 8);
			}
		} break;
		case TC3_LOW: {
			tc_registers[3] = (tc_registers[3] & 0xFF00) | *((uint8_t *)value);
		} break;
		case TC4_HIGH: {
			if (size == 2) {
				tc_registers[4] = *((uint16_t *)value);
			} else {
				tc_registers[4] = (tc_registers[4] & 0x00FF) | ((uint16_t) *((uint8_t *)value) << 8);
			}
		} break;
		case TC4_LOW: {
			tc_registers[4] = (tc_registers[4] & 0xFF00) | *((uint8_t *)value);
		} break;
		case TC5_HIGH: {
			if (size == 2) {
				tc_registers[5] = *((uint16_t *)value);
			} else {
				tc_registers[5] = (tc_registers[5] & 0x00FF) | ((uint16_t) *((uint8_t *)value) << 8);
			}
		} break;
		case TC5_LOW: {
			tc_registers[5] = (tc_registers[5] & 0xFF00) | *((uint8_t *)value);
		} break;
		case TC6_HIGH: {
			if (size == 2) {
				tc_registers[6] = *((uint16_t *)value);
			} else {
				tc_registers[6] = (tc_registers[6] & 0x00FF) | ((uint16_t) *((uint8_t *)value) << 8);
			}
		} break;
		case TC6_LOW: {
			tc_registers[6] = (tc_registers[6] & 0xFF00) | *((uint8_t *)value);
		} break;
		case TC7_HIGH: {
			if (size == 2) {
				tc_registers[7] = *((uint16_t *)value);
			} else {
				tc_registers[7] = (tc_registers[7] & 0x00FF) | ((uint16_t) *((uint8_t *)value) << 8);
			}
		} break;
		case TC7_LOW: {
			tc_registers[7] = (tc_registers[7] & 0xFF00) | *((uint8_t *)value);
		} break;
		case PACTL: {
			pactl_register = (pactl_register & 0x80) | (*((uint8_t *)value) & 0x7F);
		} break;
		case PAFLG: {
			paflg_register = (paflg_register & 0xFC) | (*((uint8_t *)value) & 0x03);
		} break;
		case PACN3: {
			if (size == 2) {
				pacn32_register = *((uint16_t *)value);
			} else {
				pacn32_register = (pacn32_register & 0x00FF) | ((uint16_t) *((uint8_t *)value) << 8);
			}
		} break;
		case PACN2: {
			pacn32_register = (pacn32_register & 0xFF00) | *((uint8_t *)value);
		} break;
		case PACN1: {
			if (size == 2) {
				pacn10_register = *((uint16_t *)value);
			} else {
				pacn10_register = (pacn10_register & 0x00FF) | ((uint16_t) *((uint8_t *)value) << 8);
			}
		} break;
		case PACN0: {
			pacn10_register = (pacn10_register & 0xFF00) | *((uint8_t *)value);
		} break;
		case MCCTL: {
			mcctl_register = *((uint8_t *)value);
		} break;
		case MCFLG: {++
			mcflg_register = (mcflg_register & 0x7F) | (*((uint8_t *)value) & 0x80);
		} break;
		case ICPAR: {
			icpar_register = (icpar_register & 0xF0) | (*((uint8_t *)value) & 0x0F);
		} break;
		case DLYCT: {
			dlyct_register = *((uint8_t *)value);
		} break;
		case ICOVW: {
			icovw_register = *((uint8_t *)value);
		} break;
		case ICSYS: {
			icsys_register = *((uint8_t *)value);
		} break;
		case RESERVED: { /* Reserved Address */} break;
		case TIMTST: { /* Timer Test Register */} break;
		case PTPSR: {
			ptpsr_register = *((uint8_t *)value);
		} break;
		case PTMCPSR: {
			ptmcpsr_register = *((uint8_t *)value);
		} break;
		case PBCTL: {
			pbctl_register = (pbctl_register & 0xBD) | (*((uint8_t *)value) & 0x42);
		} break;
		case PBFLG: {
			pbflg_register = (pbflg_register & 0xFD) | (*((uint8_t *)value) & 0x02);
		} break;
		case PA3H: {
			/* don't accept write */;
		} break;
		case PA2H: {
			/* don't accept write */;
		} break;
		case PA1H: {
			/* don't accept write */;
		} break;
		case PA0H: {
			/* don't accept write */;
		} break;
		case MCCNT_HIGH: {
			if (size == 2) {
				mccnt_register = *((uint16_t *)value);
			} else {
				mccnt_register = (mccnt_register & 0x00FF) | ((uint16_t) (*((uint8_t *)value) << 8));
			}

		} break;
		case MCCNT_LOW: {
			mccnt_register = (mccnt_register & 0xFF00) | *((uint8_t *)value);
		} break;
		case TC0H_HIGH: {
			/* don't accept write */;
		} break;
		case TC0H_LOW: {
			/* don't accept write */;
		} break;
		case TC1H_HIGH: {
			/* don't accept write */;
		} break;
		case TC1H_LOW: {
			/* don't accept write */;
		} break;
		case TC2H_HIGH: {
			/* don't accept write */;
		} break;
		case TC2H_LOW: {
			/* don't accept write */;
		} break;
		case TC3H_HIGH: {
			/* don't accept write */;
		} break;
		case TC3H_LOW: {
			/* don't accept write */;
		} break;

		default: {
			return false;
		}
	}

	return true;
}

//=====================================================================
//=                  Client/Service setup methods                     =
//=====================================================================

bool ECT::BeginSetup() {

	//	char buf[80];
	//
	//	sprintf(buf, "%s.ATDCTL0",name());
	//	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &atdctl0_register);


		Reset();

		ComputeInternalTime();

		return true;
}

bool ECT::Setup(ServiceExportBase *srv_export) {
	return true;
}

bool ECT::EndSetup() {
	return true;
}

/**
 * Gets a register interface to the register specified by name.
 *
 * @param name The name of the requested register.
 * @return A pointer to the RegisterInterface corresponding to name.
 */
Register* ECT::GetRegister(const char *name)
{
	if(registers_registry.find(string(name)) != registers_registry.end())
		return registers_registry[string(name)];
	else
		return NULL;

}

void ECT::OnDisconnect() {
}

void ECT::Reset() {

		tios_register = 0x00;
		cforc_register = 0x00;
		oc7m_register = 0x00;
		oc7d_register = 0x00;
		tcnt_register = 0x0000;
		tscr1_register = 0x0000;
		ttof_register = 0x00;
		tctl12_register = 0x0000;
		tctl34_register = 0x0000;
		tie_register = 0x0000;
		tscr2_register = 0x00;
		tflg1_register = 0x00;
		tflg2_register = 0x00;
		for (uint8_t i=0; i<8; i++) { tc_registers[i] = 0x0000; }
		pactl_register = 0x00;
		paflg_register = 0x00;
		pacn32_register = 0x0000;
		pacn10_register = 0x0000;
		mcctl_register = 0x00;
		mcflg_register = 0x00;
		icpar_register = 0x00;
		dlyct_register = 0x00;
		icovw_register = 0x00;
		icsys_register = 0x00;
		timtst_register = 0x00;
		ptpsr_register = 0x00;
		ptmcpsr_register = 0x00;
		pbctl_register = 0x00;
		pbflg_register = 0x00;
		for (uint8_t i=0; i<4; i++) { paxh_registers[i] = 0x00; }
		mccnt_register = 0xFFFF;
		for (uint8_t i=0; i<4; i++) { tcxh_registers[i] = 0x0000; }

}

void ECT::ComputeInternalTime() {

	bus_cycle_time = sc_time((double)bus_cycle_time_int, SC_PS);
}

void ECT::updateCRGClock(tlm::tlm_generic_payload& trans, sc_time& delay) {

	sc_dt::uint64*   external_bus_clock = (sc_dt::uint64*) trans.get_data_ptr();
    trans.set_response_status( tlm::TLM_OK_RESPONSE );

	bus_cycle_time_int = *external_bus_clock;

	ComputeInternalTime();
}

//=====================================================================
//=             memory interface methods                              =
//=====================================================================

bool ECT::ReadMemory(service_address_t addr, void *buffer, uint32_t size) {

	// TODO: This is a temporary code. The ECT is not implemented yet
	if (addr-baseAddress < 0x40) {
		*((uint8_t *) buffer) = 0x0;
		return true;
	} else {
		return false;
	}

	// TODO: the code below has to be used once the ECT is implemented
//	return read(addr-baseAddress, *(uint8_t *) buffer);
}

bool ECT::WriteMemory(service_address_t addr, const void *buffer, uint32_t size) {

	// TODO: This is a temporary code. The ECT is not implemented yet
	if (addr-baseAddress < 0x40) {
		return true;
	} else {
		return false;
	}

	// TODO: the code below has to be used once the ECT is implemented
//	return write(addr-baseAddress, *(uint8_t *) buffer);
}


} // end of namespace hcs12x
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim


