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

//template void ECT::IOC_Channel_t::checkChangeStateAndWait<uint8_t>(const sc_time clk);
//template void ECT::IOC_Channel_t::checkChangeStateAndWait<uint16_t>(const sc_time clk);

ECT::ECT(const sc_module_name& name, Object *parent) :
	Object(name, parent)
	, sc_module(name)

	, Client<TrapReporting>(name, parent)
	, Service<Memory<service_address_t> >(name, parent)
	, Service<Registers>(name, parent)
	, Client<Memory<service_address_t> >(name, parent)

	, trap_reporting_import("trap_reporting_import", this)

	, slave_socket("slave_socket")

	, memory_export("memory_export", this)
	, memory_import("memory_import", this)
	, registers_export("registers_export", this)
	, logger(0)

	, bus_cycle_time_int(250000)
	, param_bus_cycle_time_int("bus-cycle-time", this, bus_cycle_time_int)

	 // MC9S12XDP512V2 - ECT baseAddress
	, baseAddress(0x0040)
	, param_baseAddress("base-address", this, baseAddress)

	, interrupt_offset_channel0(0xEE)
	, param_interrupt_offset_channel0("interrupt-offset-channel0", this, interrupt_offset_channel0)
	, interrupt_offset_timer_overflow(0xDE)
	, param_interrupt_offset_timer_overflow("interrupt-offset-timer-overflow", this, interrupt_offset_timer_overflow)
	, interrupt_pulse_accumulatorA_overflow(0xDC)
	, param_interrupt_pulse_accumulatorA_overflow("interrupt-pulse-accumulatorA-overflow", this, interrupt_pulse_accumulatorA_overflow)
	, interrupt_pulse_accumulatorB_overflow(0xC8)
	, param_interrupt_pulse_accumulatorB_overflow("interrupt-pulse-accumulatorB-overflow", this, interrupt_pulse_accumulatorB_overflow)

	, interrupt_pulse_accumulatorA_input_edge(0xDA)
	, param_interrupt_pulse_accumulatorA_input_edge("interrupt-pulse-accumulator-input-edge", this, interrupt_pulse_accumulatorA_input_edge)

	, debug_enabled(false)
	, param_debug_enabled("debug-enabled", this, debug_enabled)

	, buildin_signal_generator(true)
	, param_buildin_signal_generator("buildin-signal-generator", this, buildin_signal_generator, "Use buildin signal generator or external instrument")
	, edge_detector_period_int(25000)
	, param_edge_detector_period("edge-detector-period", this, edge_detector_period_int, "Edge detector period in pico-seconds. Default 25000ps.")

	, pinLogic_reg("pin", this, pinLogic, 8, "ECT input/output pin")
	, signalLengthArray_reg("signal-length-array", this, signalLengthArray, 8, "Signal length for Buffered IC channels. Associated to pins P0-P3.")

	, prnt_write(false)
	, icsys_write(false)
	, main_timer_enabled(false)
	, down_counter_enabled(false)
	, delay_counter_enabled(false)
{

	char channelName[20];

	for (uint8_t i=0; i<4; i++) {
		sprintf (channelName, "pa%d", i);
		pc8bit[i] = new PulseAccumulator8Bit(this, i, &pacn_register[i], &paxh_registers[i]);
	}

	for (uint8_t i=0; i<8; i++) {

		pinLogic[i] = false;
		pinLogic_reg[i].SetMutable(true);
		signalLengthArray[i] = 0;
		signalLengthArray_reg[i].SetMutable(true);

		if (i < 4) {
			ioc_channel[i] = new IOC_Channel_t(this, i, &pinLogic[i], &tc_registers[i], &tcxh_registers[i], pc8bit[i]);
		} else {
			ioc_channel[i] = new IOC_Channel_t(this, i, &pinLogic[i], &tc_registers[i], NULL, NULL);
		}

	}


	logger = new unisim::kernel::logger::Logger(*this);

	interrupt_request(*this);
	slave_socket.register_b_transport(this, &ECT::read_write);
	bus_clock_socket.register_b_transport(this, &ECT::updateCRGClock);

	SC_HAS_PROCESS(ECT);

	SC_THREAD(RunInputCapture);

	SC_THREAD(RunMainTimerCounter);

	SC_THREAD(RunDownCounter);

	pacA = new PulseAccumulatorA("PACA", this, &pinLogic[7], &signalLengthArray[7], pc8bit[3], pc8bit[2]);
	pacB = new PulseAccumulatorB("PACB", this, &pinLogic[0], &signalLengthArray[0], pc8bit[1], pc8bit[0]);

	Reset();

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

	if (logger) { delete logger; logger = NULL;}

}

inline void ECT::main_timer_enable() {
	main_timer_enabled = true;
	if ((tscr1_register & 0x80) != 0) {
		main_timer_enable_event.notify();
	}
}

inline void ECT::main_timer_disable() {main_timer_enabled = false; }

/**
 * TODO:
 * Timer Module Stops While in Wait
 *  - 0 Allows the timer module to continue running during wait.
 *  - 1 Disables the timer counter, pulse accumulators and modulus down counter when the MCU is in wait mode.
 *  Timer interrupts cannot be used to get the MCU out of wait.
 */
void ECT::enterWaitMode() {
	if ((tscr1_register & 0x40) != 0) {
		main_timer_disable();
	}
}

void ECT::exitWaitMode() {
	main_timer_enable();
}

/**
 * TODO:
 * Timer and Modulus Counter Stop While in Freeze Mode
 *  - 0 Allows the timer and modulus counter to continue running while in freeze mode.
 *  - 1 Disables the timer and modulus counter whenever the MCU is in freeze mode. This is useful for emulation.
 *  The pulse accumulators do not stop in freeze mode.
 */
void ECT::enterFreezeMode() {
	if ((tscr1_register & 0x20) != 0) {
		main_timer_disable();
	}
}

void ECT::exitFreezeMode() {
	main_timer_enable();
}

/**
 * This thread emulate the edge detector using :
 * - Edge configuration TCL3/TCL4,
 * - Edge sharing ICSYS
 * - Delay DLYCT
 *
 * 0: Capture disabled
 * 1: Capture on rising edges only
 * 2: Capture on falling edges only
 * 3: Capture on any edge (rising or falling)
 */
void ECT::RunInputCapture() {

	/* initialize random seed: */
	srand (12345);

	uint8_t edge = 0;
	uint16_t edgeDelay = 0;

	while (true) {
		while (!main_timer_enabled || ((tscr1_register & 0x80) == 0)) {
			wait(main_timer_enable_event);
		}

		for (uint8_t i=0; i<8; i++) {

			// is capture activated (and hence EdgeDetector Activated) ?
			// is the channel selected as input capture ?
			if ((ioc_channel[i]->getValideEdge() != 0) && isInputCapture(i)) {

				// if edge sharing is enabled then channels [4,7] have been respectively stimulated by channels [0,3].
				if ((i>4) && ((icsys_register & (1 << i)) != 0)) {
					if (isInputCapture(i-4)) {
						continue;
					} else {
						*logger << DebugWarning << "Share Input action of Input Capture Channels " << std::dec << i << " and " << std::dec << (i+4) << " is enabled while channel " << std::dec << i << " isn't selected as Input Capture channel" << std::endl << EndDebugWarning;
					}
				}

				if (isValidSignal(i, edge, edgeDelay)) {

					ioc_channel[i]->RunInputCapture(edgeDelay);
					// is sharing edge enabled ?
					if ((i<4) && (((icsys_register & (1 << (i+4))) != 0))) {
						if (isInputCapture(i+4)) {
							ioc_channel[i+4]->RunInputCapture(edgeDelay);
						} else {
							*logger << DebugWarning << "Share Input action of Input Capture Channels " << std::dec << i << " and " << std::dec << (i+4) << " is enabled while channel " << std::dec << (i+4) << " isn't selected as Input Capture channel" << std::endl << EndDebugWarning;
						}

					}

				}

			}
			else	// Capture is disabled or The channel is selected to Output capture
			{

			}
		}

		// pause one period to generate new edge
		wait(edge_detector_period);
	}

}

void ECT::RunMainTimerCounter() {

	while (true) {
		while (!main_timer_enabled || ((tscr1_register & 0x80) == 0)) {
			wait(main_timer_enable_event);
		}

		while (main_timer_enabled && ((tscr1_register & 0x80) != 0)) {
			/**
			 * If the pulse accumulator is disabled (PACTL::PAEN=0) then
			 *     the prescaler clock from the timer is always used as an input clock to the timer counter.
			 * else
			 *     the pulse accumulator clock (PACLK) is used depending on PACTL::CLK[1:] bits configuration.
			 *
			 * The change from one selected clock to the other  happens immediately after these bits are written.
			 */
			if ((pactl_register & 0x40) == 0) {
				sc_time prescaled_time;
				// is TSCR1::PRNT=0 ?
				if ((tscr1_register & 0x08) == 0) {
					prescaled_time = pow(2, (tscr2_register & 0x07)) * bus_cycle_time;
				} else {
					prescaled_time = (ptpsr_register + 1) * bus_cycle_time;
				}

				wait(prescaled_time);

			} else {
				wait(paclk_event);
			}

			if (tcnt_register == 0xFFFF) {
				// Timer overflow
				tcnt_register = 0x0000;
				if ((tc_registers[7] != 0xFFFF) || !isTimerCounterResetEnabled()) {
					// set TFLG2::TOF timer overflow flag
					tflg2_register = tflg2_register | 0x80;
				}
				assertInterrupt(interrupt_offset_timer_overflow);
			} else {
				if ((tc_registers[7] != 0x0000) || !isTimerCounterResetEnabled()) {
					tcnt_register = tcnt_register + 1;
				}
			}
		}
	}
}

void ECT::RunDownCounter() {

	/**
	 * The 16-bit modulus down-counter can control the transfer of the IC registers and the pulse accumulators
	 * contents to the respective holding registers for a given period, every time the count reaches zero.
	 *
	 * The modulus down-counter can also be used as a stand-alone time base with periodic interrupt capability.
	 */

	while (true) {
		while ((!down_counter_enabled) || ((mcctl_register & 0x04) == 0)) {
			wait(down_counter_enable_event);
		}

		uint8_t mccnt_prescaler = mcctl_register & 0x03;
		sc_time mccnt_period = bus_cycle_time;
		if (mccnt_prescaler > 0) {
			mccnt_period = bus_cycle_time * pow(2, mccnt_prescaler+1);
		}

		while ((down_counter_enabled) && ((mcctl_register & 0x04) != 0)) {

			wait(mccnt_period);
		}

	}
}

void ECT::RunCaptureCompare() {
	for (uint8_t i=0; i<8; i++) {
		if (!isInputCapture(i)) {
			ioc_channel[i]->RunCaptureCompare(false);
		}
	}
}

void ECT::assertInterrupt(uint8_t interrupt_offset) {

	if ((interrupt_offset >= (interrupt_offset_channel0 - 0xE)) && (interrupt_offset <= interrupt_offset_channel0 ) && !isInputOutputInterruptEnabled((interrupt_offset_channel0 - interrupt_offset)/2)) return;
	if ((interrupt_offset == interrupt_offset_timer_overflow) && !isTimerOverflowinterruptEnabled()) return;
	if ((interrupt_offset == interrupt_pulse_accumulatorA_overflow) && ((pactl_register & 0x02) == 0)) return;
	if ((interrupt_offset == interrupt_pulse_accumulatorB_overflow) && ((pbctl_register & 0x02) == 0)) return;
	if ((interrupt_offset == interrupt_pulse_accumulatorA_input_edge) && ((pactl_register & 0x01) == 0)) return;


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
			if ((tscr1_register & 0x10) != 0) {
				tflg2_register = tflg2_register & 0x7F;
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
		case PACTL: {
			*((uint8_t *)value) =  pactl_register & 0x7F;
		} break;
		case PAFLG: {
			*((uint8_t *)value) = paflg_register & 0x03;
		} break;
		case PACN3: {
			if (size == 2) {
				*((uint16_t *)value) = (((uint16_t) pacn_register[3]) << 8) | pacn_register[2];
			} else {
				*((uint8_t *)value) = pacn_register[3];
			}
			if ((tscr1_register & 0x10) != 0) {
				paflg_register = paflg_register & 0xFC;
			}

		} break;
		case PACN2: {
			*((uint8_t *)value) = pacn_register[2];
			if ((tscr1_register & 0x10) != 0) {
				paflg_register = paflg_register & 0xFC;
			}

		} break;
		case PACN1: {
			if (size == 2) {
				*((uint16_t *)value) = (((uint16_t) pacn_register[1]) << 8) | pacn_register[0];
			} else {
				*((uint8_t *)value) = pacn_register[1];
			}
			if ((tscr1_register & 0x10) != 0) {
				pbflg_register = pbflg_register & 0xFD;
			}

		} break;
		case PACN0: {
			*((uint8_t *)value) = pacn_register[0];
			if ((tscr1_register & 0x10) != 0) {
				pbflg_register = pbflg_register & 0xFD;
			}
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

		default: {

			if ((offset == MCCNT_HIGH) || (offset == MCCNT_LOW)) {
				uint16_t readed_value = mccnt_register;
				if ((mcctl_register & 0x20) != 0) {
					readed_value = mccnt_load_register;
				} else {
					if ((tscr1_register & 0x10) != 0) {
						mcflg_register = mcflg_register & 0x7F;
					}
				}

				if (offset == MCCNT_HIGH) {
					if (size == 2) {
						*((uint16_t *)value) = readed_value;
					} else {
						*((uint8_t *)value) = readed_value >> 8;
					}
				} else {
					*((uint8_t *)value) = readed_value & 0x00FF;
				}
			} else if ((offset >= TC0_HIGH) && (offset <= TC7_LOW)) {
				/**
				 * The IC channels are composed of four standard IC registers and four buffered IC channels.
				 *  - An IC register is empty when it has been read or latched into the holding register.
				 *  - A holding register is empty when it has been read
				 */
				uint8_t tc_offset = offset - TC0_HIGH;
				if ((tc_offset % 2) == 0) // TCx_High ?
				{
					if (size == 2) {
						*((uint16_t *)value) = tc_registers[tc_offset/2];
						tc_registers[tc_offset/2] = 0x0000;
					} else {
						*((uint8_t *)value) =  tc_registers[tc_offset/2] >> 8;
						tc_registers[tc_offset] = tc_registers[tc_offset] & 0x00FF;
					}
				} else {
					*((uint8_t *)value) =  tc_registers[tc_offset/2] & 0x00FF;
					tc_registers[tc_offset] = tc_registers[tc_offset/2] & 0xFF00;
				}

				if ((tscr1_register & 0x10) != 0) {
					tflg1_register = tflg1_register & ~(1 << (tc_offset/2));
				}
			}
			else if ((offset >= PA3H) && (offset <= PA0H)) {
				uint8_t paxh_offset = offset - PA3H;
				*((uint8_t *)value) = paxh_registers[paxh_offset];
			}
			else if ((offset >= TC0H_HIGH) && (offset <= TC3H_LOW)) {
				uint8_t tcxh_offset = offset - TC0H_HIGH;

				if ((tcxh_offset % 2) == 0) // TCxH_High ?
				{
					if (size == 2) {
						*((uint16_t *)value) = tcxh_registers[tcxh_offset/2];
						tcxh_registers[tcxh_offset/2] = 0x0000;
					} else {
						*((uint8_t *)value) = tcxh_registers[tcxh_offset/2] >> 8;
						tcxh_registers[tcxh_offset/2] = tcxh_registers[tcxh_offset/2] & 0x00FF;
					}

				} else {
					*((uint8_t *)value) = tcxh_registers[tcxh_offset/2] & 0x00FF;
					tcxh_registers[tcxh_offset/2] = tcxh_registers[tcxh_offset/2] & 0xFF00;
				}
				/**
				 * In queue mode, reads of the holding register will latch the corresponding pulse accumulator value to its holding register
				 */
				if (((tcxh_offset/2) < 4) && (!isLatchMode())) {
					paxh_registers[tcxh_offset/2] = pacn_register[tcxh_offset/2];
				}
			}

			return false;
		}
	}

	return true;
}

bool ECT::write(uint8_t offset, unsigned char* value, uint8_t size) {

	switch (offset) {
		case TIOS: {
			 tios_register = *((uint8_t *)value);
			 pacA->wakeup();
			 pacB->wakeup();
		} break;
		case CFORC: {
			cforc_register = *((uint8_t *)value);

			for (uint8_t i=0; i<8; i++) {
				if (!isInputCapture(i) && ((cforc_register & (1 << i)) != 0)) {
					ioc_channel[i]->RunCaptureCompare(true);
				}
			}

		} break;
		case OC7M: {
			oc7m_register = *((uint8_t *)value);
			 pacA->wakeup();
			 pacB->wakeup();
		} break;
		case OC7D: {
			oc7d_register = *((uint8_t *)value);
		} break;
		case TCNT_HIGH: {

			*logger << DebugWarning << "Try writing to TCNT High register! Has no meaning or effect." << std::endl << EndDebugWarning;
			if ((tscr1_register & 0x10) != 0) {
				tflg2_register = tflg2_register & 0x7F;
			}

/*
			if (size == 2) {
				tcnt_register = *((uint16_t *)value);
			} else {
				tcnt_register = (tcnt_register & 0x00FF) | ((uint16_t) *((uint8_t *)value) << 8);
			}
*/
		} break;
		case TCNT_LOW: {
			*logger << DebugWarning << "Try writing to TCNT High register! Has no meaning or effect." << std::endl << EndDebugWarning;
			if ((tscr1_register & 0x10) != 0) {
				tflg2_register = tflg2_register & 0x7F;
			}

/*
			tcnt_register = (tcnt_register & 0xFF00) | *((uint8_t *)value);
*/
		} break;
		case TSCR1: {
			uint8_t old_prnt_bit = tscr1_register & 0x04;
			*((uint8_t *)value) = (tscr1_register & 0x07) | (*((uint8_t *)value) & 0xF8);

			if (prnt_write) {
				*((uint8_t *)value) = (*((uint8_t *)value) & 0xF7) | old_prnt_bit;
			} else {
				prnt_write = true;
			}
			tscr1_register = *((uint8_t *)value);

			if ((tscr1_register & 0x80) != 0) {
				main_timer_enable();
			} else {
				main_timer_disable();
			}
			/**
			 * TODO:
			 *  - use TSCR1::TFFCA Timer Fast Flag Clear All
			 *  - use TSCR1::PRNT Precision Timer (Figures 7-65 , 7-66 , 7-67 , 7-68
			 */
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
			configureOutputAction();
			 pacA->wakeup();
			 pacB->wakeup();

		} break;
		case TCTL2: {
			tctl12_register = (tctl12_register & 0xFF00) | *((uint8_t *)value);
			configureOutputAction();
			 pacA->wakeup();
			 pacB->wakeup();

		} break;
		case TCTL3: {
			if (size == 2) {
				tctl34_register = *((uint16_t *)value);
			} else {
				tctl34_register = (tctl34_register & 0x00FF) | ((uint16_t) *((uint8_t *)value) << 8);
			}
			configureEdgeDetector();
		} break;
		case TCTL4: {
			tctl34_register = (tctl34_register & 0xFF00) | *((uint8_t *)value);
			configureEdgeDetector();
		} break;
		case TIE: {
			tie_register = *((uint8_t *)value);
		} break;
		case TSCR2: {
			tscr2_register = (tscr2_register & 0x70) | (*((uint8_t *)value) & 0x8F);
		} break;
		case TFLG1: {
			// writing a one to the flag clears the flag.
			// writing a zero will not affect the current status of the bit
			// when TSCR1::TFFCA=1, the flag cannot be cleared via the normal flag clearing mechanism

			// chech TSCR1::TFFCA bit is cleared
			if ((tscr1_register & 0x10) == 0) {
				uint8_t val = *((uint8_t *)value);
				tflg1_register = tflg1_register & ~val;
			}
		} break;
		case TFLG2: {
			// writing a one to the flag clears the flag.
			// writing a zero will not affect the current status of the bit
			// when TSCR1::TFFCA=1, the flag cannot be cleared via the normal flag clearing mechanism

			if ((tscr1_register & 0x10) == 0) {
				uint8_t val = (*((uint8_t *)value) & 0x80);

				tflg2_register = tflg2_register & ~val;
			}

		} break;
		case PACTL: {
			uint8_t masked_value = (pactl_register & 0x80) | (*((uint8_t *)value) & 0x7F);
			pactl_register = masked_value;

			switch ((pactl_register & 0x30) >> 4) {
				case 0:  // Falling edge
				{
					// For edge detector configuration, I use the same codification as for TCTL3/TCTL4 register
					pacA->setValideEdge(2);
					pacA->setMode(false);
				} break;
				case 1: // Rising edge
				{
					// For edge detector configuration, I use the same codification as for TCTL3/TCTL4 register
					pacA->setValideEdge(1);
					pacA->setMode(false);
				} break;
				case 2: // Divide by 64 clock enabled with pin high level
				{
					pacA->setValideEdge(1); // I use rising edge code for High level
					pacA->setMode(true);
				} break;
				case 3: // Divide by 64 clock enabled with pin low level
				{
					pacA->setValideEdge(2); // I use falling edge code for Low level
					pacA->setMode(true);
				} break;


			}

			if ((pactl_register & 0x40) != 0) {
				pacA->wakeup();
			}


		} break;
		case PAFLG: {
			// writing a one to the flag clears the flag.
			// writing a zero will not affect the current status of the bit
			// when TSCR1::TFFCA=1, the flag cannot be cleared via the normal flag clearing mechanism

			if ((tscr1_register & 0x10) == 0) {
				uint8_t val = (*((uint8_t *)value) & 0x03);
				paflg_register = paflg_register & ~val;
			}
		} break;
		case PACN3: {
			if (size == 2) {
				pacn_register[3] = *((uint16_t *)value) >> 8;
				pacn_register[2] = *((uint16_t *)value) & 0x00FF;
			} else {
				pacn_register[3] = *((uint8_t *)value);
			}
			if ((tscr1_register & 0x10) != 0) {
				paflg_register = paflg_register & 0xFC;
			}

		} break;
		case PACN2: {
			pacn_register[2] = *((uint8_t *)value);
			if ((tscr1_register & 0x10) != 0) {
				paflg_register = paflg_register & 0xFC;
			}
		} break;
		case PACN1: {
			if (size == 2) {
				pacn_register[1] = *((uint16_t *)value) >> 8;
				pacn_register[0] = *((uint16_t *)value) & 0x00FF;
			} else {
				pacn_register[1] = *((uint8_t *)value);
			}
			if ((tscr1_register & 0x10) != 0) {
				pbflg_register = pbflg_register & 0xFD;
			}

		} break;
		case PACN0: {
			pacn_register[0] = *((uint8_t *)value);
			if ((tscr1_register & 0x10) != 0) {
				pbflg_register = pbflg_register & 0xFD;
			}
		} break;
		case MCCTL: {
			mcctl_register = *((uint8_t *)value);

			// input Capture Force latch Action ?
			if (((mcctl_register & 0x10) !=0) && ((icsys_register & 0x03) != 0)) {
				latchToHoldingRegisters();
			}

			// Force Load Register into the Modulus Counter count register ? (MCCTL::FLMC=1 and MCCTL::MCEN=1)
			if ((mcctl_register && 0x06) != 0) {
				loadRegisterToModulusCounterCountregister();
			}

			if ((mcctl_register & 0x04) == 0) {
				down_counter_disable();
				mccnt_register = 0xFFFF;
			} else {
				down_counter_enable();
			}
		} break;
		case MCFLG: {
			if ((tscr1_register & 0x10) == 0) {
				uint8_t val = *((uint8_t *)value) & 0x80;
				mcflg_register = mcflg_register & ~val;
			}

		} break;
		case ICPAR: {
			icpar_register = (icpar_register & 0xF0) | (*((uint8_t *)value) & 0x0F);
		} break;
		case DLYCT: {
			dlyct_register = *((uint8_t *)value);
			computeDelayCounter();
		} break;
		case ICOVW: {
			icovw_register = *((uint8_t *)value);
		} break;
		case ICSYS: {
			if (icsys_write) {
				*logger << DebugWarning << "ICSYS register has already been written! This register is write once in normal modes." << std::endl << EndDebugWarning;
			} else {
				icsys_write = true;
				icsys_register = *((uint8_t *)value);
			}

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
			 pacB->wakeup();

		} break;
		case PBFLG: {
			// when TSCR1::TFFCA=1, the flag cannot be cleared via the normal flag clearing mechanism
			if ((tscr1_register & 0x10) == 0) {
				uint8_t val = *((uint8_t *)value) & 0x02;
				pbflg_register = pbflg_register & ~val;
			}

		} break;

		default: {
			if ((offset == MCCNT_HIGH) || (offset == MCCNT_LOW)) {
				uint16_t old_mccnt_register = mccnt_register;

				if ((tscr1_register & 0x10) != 0) {
					mcflg_register = mcflg_register & 0x7F;
				}

				if (offset == MCCNT_HIGH) {
					if (size == 2) {
						mccnt_load_register = *((uint16_t *)value);
					} else {
						mccnt_load_register = (mccnt_load_register & 0x00FF) | ((uint16_t) (*((uint8_t *)value) << 8));
					}
				} else {
					mccnt_load_register = (mccnt_load_register & 0xFF00) | *((uint8_t *)value);
				}

				/**
				 * If the modulus down counter is enabled (MCCTL::MCEN=1) and modulus mode is enabled (MCCTL::MODMC=1),
				 * a write to MCCNT will update the load register with the value written to it. The count register
				 * will not be updated with the new value until the next counter underflow.
				 *
				 * The FLMC bit in MCCTL can be used to immediately update the count register with the new value
				 * if an immediate load is desired
				 *
				 *  If the modulus mode is not enabled (MCCTL::MODMC=0), a write to MCCNT will clear the modulus prescaler and
				 *  will immediately update the counter register with the value written to it
				 *  and down-counts to 0x0000 and stops.
				 *
				 */

				if (((mcctl_register & 0x44) != 0x44) || ((mcctl_register & 0x08) != 0)) {
					mccnt_register = mccnt_load_register;

					if ((mcctl_register & 0x40) == 0) {
						mcctl_register = mcctl_register & 0xFC;
					}
				}

				/**
				 * If a 0x0000 is written into MCCNT when LATQ and BUFFEN in ICSYS register are set,
				 * the input capture and pulse accumulator registers will be latched.
				 */
				if ((mccnt_register == 0x0000) && (old_mccnt_register != 0)) {
					if ((icsys_register & 0x03) == 0x03) {
						for (uint8_t i=0; i < 3; i++) {
							ioc_channel[i]->latchToHoldingRegisters();
						}

					}
				}

			} else if ((offset >= TC0_HIGH) && (offset <= TC7_LOW)) {
				uint8_t tc_offset = offset - TC0_HIGH;
				uint8_t channel_mask = (1 << tc_offset);

				// is the channel configured for "input capture" ?
				if ((tios_register & channel_mask) == 0) {
					*logger << DebugWarning << "Channel " << std::dec << (tc_offset%2) << " is configured for <<input capture>>. Writing Has no meaning or effect." << std::endl << EndDebugWarning;
				}
				// The channel is configured for "output compare"
				else {
					if ((tc_offset % 2) == 0) // TCx_High ?
					{
						if (size == 2) {
							tc_registers[tc_offset] = *((uint16_t *)value);
						} else {
							tc_registers[tc_offset] = (tc_registers[tc_offset] & 0x00FF) | ((uint16_t) *((uint8_t *)value) << 8);
						}

					} else {
						tc_registers[tc_offset] = (tc_registers[tc_offset] & 0xFF00) | *((uint8_t *)value);
					}
				}
			}
			else if ((offset >= PA3H) && (offset <= PA0H)) {
				/* don't accept write */;
			}
			else if ((offset >= TC0H_HIGH) && (offset <= TC3H_LOW)) {
				/* don't accept write */;
			}

			return false;
		}
	}

	return true;
}

inline void ECT::configureOutputAction() {

	for (uint8_t i=0; i<8; i++) {
		uint8_t outputAction = ((uint16_t) (tctl12_register & (3 << (i*2))) >> (i*2)) & 0x03;

		ioc_channel[i]->setOutputAction(outputAction);
	}
}

inline void ECT::configureEdgeDetector() {

	for (uint8_t i=0; i<8; i++) {
		uint8_t edgeConfig = ((uint16_t) (tctl34_register & (3 << (i*2))) >> (i*2)) & 0x03;

		ioc_channel[i]->setValideEdge(edgeConfig);
	}
}

inline void ECT::computeDelayCounter() {
	// TSCR1::PRNT = 0 ?
	if ((tscr1_register & 0x08) == 0) {
		// 32 * 2^dly10 * 4 = {disable,  256, 512, 1024} bus cycles
		uint8_t dly10 = dlyct_register & 0x03;
		if (dly10 == 0) {
			delay_counter_disable();

		} else {
			edge_delay_counter = 32 * pow(2, dly10) * 4;
			delay_counter_enable();
		}
	} else {
		// (dly70 + 1) * 4 = {disable, 8, ..., 1024} bus cycles
		if (dlyct_register == 0) {
			delay_counter_disable();
		} else {
			edge_delay_counter = (dlyct_register + 1) * 4;
			delay_counter_enable();
		}
	}
}

ECT::PulseAccumulator8Bit::PulseAccumulator8Bit(ECT *parent, const uint8_t pacn_number, uint8_t *pacn_ptr, uint8_t* pah_ptr) :
	pacn_register_ptr(pacn_ptr)
	, pah_register_ptr(pah_ptr)
	, pacn_index(pacn_number)

{

}

bool ECT::PulseAccumulator8Bit::countEdge8Bit() {

	bool overflow = false;

	if (*pacn_register_ptr == 0xFF) {
		overflow = true;
		if (!ectParent->isPulseAccumulatorsMaximumCount()) {
			*pacn_register_ptr = 0x00;
		}
		if (pacn_index == 3) {
			ectParent->setPulseAccumulatorAOverflowFlag();
			ectParent->assertInterrupt(ectParent->getInterruptPulseAccumulatorAOverflow());
		} else if (pacn_index == 1) {
			ectParent->setPulseAccumulatorBOverflowFlag();
			ectParent->assertInterrupt(ectParent->getInterruptPulseAccumulatorBOverflow());
		}
	} else {
		*pacn_register_ptr = *pacn_register_ptr + 1;
	}

	return overflow;
}

void ECT::PulseAccumulator8Bit::latchToHoldingRegisters() {

	*pah_register_ptr = *pacn_register_ptr;
	clearPACN();
}

ECT::PulseAccumulator16Bit::PulseAccumulator16Bit(const sc_module_name& name, ECT *parent, bool* pinLogic, uint16_t* signalLength, PulseAccumulator8Bit *pacn_high, PulseAccumulator8Bit *pacn_low) :
	sc_module(name)
	, ectParent(parent)
	, pacn_high_ptr(pacn_high)
	, pacn_low_ptr(pacn_low)
	, channelPinLogic(pinLogic)
	, channelSignalLength(signalLength)
{

	SC_HAS_PROCESS(ECT::PulseAccumulator16Bit);

	SC_THREAD(process);
	sensitive << pulse_accumulator_enable_event;

}

void ECT::PulseAccumulator16Bit::process() {
	RunPulseAccumulator();
}

void ECT::PulseAccumulator16Bit::latchToHoldingRegisters() {
	pacn_high_ptr->latchToHoldingRegisters();
	pacn_low_ptr->latchToHoldingRegisters();

}

ECT::PulseAccumulatorA::PulseAccumulatorA(const sc_module_name& name, ECT *parent, bool* pinLogic, uint16_t* signalLength, PulseAccumulator8Bit *pacn_high, PulseAccumulator8Bit *pacn_low) :
	ECT::PulseAccumulator16Bit::PulseAccumulator16Bit(name, parent, pinLogic, signalLength, pacn_high, pacn_low)
	, isGatedTimeMode(false)
{

}

void ECT::PulseAccumulatorA::RunPulseAccumulator() {

	while (true) {

		while (!ectParent->isPulseAccumulatorAEnabled()) {
			wait(pulse_accumulator_enable_event);
		}

		/**
		 *  check PACTL::PAMOD
		 *  0-> Event counter mode
		 *  1-> Gated time accumulator mode
		 */

		/**
		 * For PAMOD bit = 0 (event counter mode).
		 *   PACTL::PEDGE = 0 Falling edges on PT7 pin cause the count to be incremented
		 *   PACTL::PEDGE = 1 Rising edges on PT7 pin cause the count to be incremented
		 *
		 * For PAMOD bit = 1 (gated time accumulation mode).
		 *   PACTL::PEDGE = 0 PT7 input pin high enables bus clock divided by 64 to Pulse Accumulator and
		 *     the trailing falling edge on PT7 sets the PAIF flag.
		 *   PACTL::PEDGE = 1 PT7 input pin low enables bus clock divided by 64 to Pulse Accumulator and
		 *     the trailing rising edge on PT7 sets the PAIF flag.
		 *
		 * If the timer is not active (TEN = 0 in TSCR1), there is no divide-by-64 since the ÷64 clock is generated by the timer prescaler
		 */

		bool canCount = false;

		if (!isGatedTimeModeEnabled()) // if (PACTL::PAMOD = 0) then Event counter
		{
			uint8_t edgeValue = 0;
			uint16_t edgeDelay = 0;
			canCount = (ectParent->isValidSignal(7, edgeValue, edgeDelay));

		}
		else  // if (PACTL::PAMOD != 0) then gated time accumulation mode
		{
			bool isValid = false;
			uint8_t edgeValue;
			uint16_t edgeDelay;
			if (ectParent->isBuildin_edge_generator()) {
				/* generate edge code */
				do {
					edgeValue = (uint8_t) rand() % 3;
				} while (edgeValue == 0);

				// Divide bus clock by 64
				edgeDelay = (uint16_t) rand() % (64 * 2);

			} else {
				if (*channelPinLogic) {
					edgeValue = 1; // rising edge => High level
				} else {
					edgeValue = 2; // falling edge => Low level
				}
				edgeDelay = *channelSignalLength;
			}

			canCount = ((edgeValue == getValideEdge()) && (edgeDelay >= 64));
		}

		uint8_t clksel = ectParent->getClockSelection();

		if (canCount) {
			if (clksel == 1) {
				// Main timer counter use PACLK as input to timer counter clock
				ectParent->notify_paclk_event();
			}

			bool pacn_low_overflow = pacn_low_ptr->countEdge8Bit();
	    	if (pacn_low_overflow) {
				if (clksel == 2) {
					// Main timer counter use PACLK/256 as timer counter clock frequency
					ectParent->notify_paclk_event();
				}

	    		pacn_low_ptr->clearPACN();
	    		bool pacn_high_overflow = pacn_high_ptr->countEdge8Bit();

				if (pacn_high_overflow && (clksel == 3)) {
					// Main Timer counter use PACLK/65536 as timer counter clock frequency
					ectParent->notify_paclk_event();
				}

	    	}
	    }

	    wait(ectParent->edge_detector_period);

	}

}


ECT::PulseAccumulatorB::PulseAccumulatorB(const sc_module_name& name, ECT *parent, bool* pinLogic, uint16_t* signalLength, PulseAccumulator8Bit *pacn_high, PulseAccumulator8Bit *pacn_low) :
	ECT::PulseAccumulator16Bit::PulseAccumulator16Bit(name, parent, pinLogic, signalLength, pacn_high, pacn_low)
{

}

void ECT::PulseAccumulatorB::RunPulseAccumulator() {
	while (true) {

		while (!ectParent->isPulseAccumulatorBEnabled()) {
			wait(pulse_accumulator_enable_event);
		}

		uint8_t edgeValue = 0;
		uint16_t edgeDelay = 0;
	    if (ectParent->isValidSignal(0, edgeValue, edgeDelay)) {
	    	if (edgeDelay > 2) {
	    		if (pacn_low_ptr->countEdge8Bit()) {
	    			pacn_low_ptr->clearPACN();
	    			pacn_high_ptr->countEdge8Bit();
	    		}

	    	}
	    }

	    wait(ectParent->edge_detector_period);
	}
}

inline void ECT::latchToHoldingRegisters() {
	/**
	 * The contents of the input capture registers TC0 to TC3
	 * and their corresponding 8-bit pulse accumulators to be latched into the associated holding registers.
	 * The pulse accumulators will be automatically cleared when the latch action occurs;
	 */
	for (uint8_t i=0; i<4; i++) {
		pc8bit[i]->latchToHoldingRegisters();
	}
}

inline void ECT::loadRegisterToModulusCounterCountregister() {
	/**
	 * TODO:
	 * Loads the load register into the modulus counter count register (MCCNT).
	 * And resets the modulus counter prescaler. This means, the selected prescaler division rate become effective.
	 */

	uint8_t mcpr = mcctl_register & 0x03; // the prescaler division values are {1, 4, 8, 16}
}

inline bool ECT::isInputCapture(uint8_t channel_index) {
	if ((tios_register & (1 << channel_index)) == 0) {
		return true;
	} else {
		return false;
	}
}

inline bool ECT::transferOutputCompareToTimerPort(uint8_t channel_index) {
	if ((oc7m_register & (1 << channel_index)) != 0) {
		return true;
	} else {
		return false;
	}
}

//=====================================================
//=             ECT Channel methods                   =
//=====================================================

ECT::IOC_Channel_t::IOC_Channel_t(ECT *parent, const uint8_t index, bool* pinLogic, uint16_t *tc_ptr, uint16_t* tch_ptr, PulseAccumulator8Bit* pc8bit) :
	  ectParent(parent)
	, ioc_index(index)
	, tc_register_ptr(tc_ptr)
	, tch_register_ptr(tch_ptr)
	, pulseAccumulator(pc8bit)
	, channelPinLogic(pinLogic)

{

	iocMask = (0x01 << ioc_index);

}

void ECT::IOC_Channel_t::RunInputCapture(uint16_t edgeDelay) {

	// Non-Buffered IC channels
	if (tch_register_ptr == NULL) {
		if (!ectParent->isNoInputCaptureOverWrite(ioc_index) || (*tc_register_ptr == 0x0000)) {
			*tc_register_ptr = ectParent->getMainTimerValue();
			// set the TFLG1::CxF to show that input capture occurs
			ectParent->setTimerInterruptFlag(ioc_index);
			ectParent->assertInterrupt(ectParent->getInterruptOffsetChannel0() - (ioc_index * 2));
		}
	}
	else //  buffered IC Channels
	{
		if (ectParent->isPulseAccumulators8BitEnabled(pulseAccumulator->getIndex())) {
			if (edgeDelay > 2) {
				pulseAccumulator->countEdge8Bit();
			}
		}

		// IC latch mode (ICSYS::LATQ=1)
		if (ectParent->isLatchMode()) {
			if (!ectParent->isNoInputCaptureOverWrite(ioc_index) || ((*tc_register_ptr == 0x0000) && (*tch_register_ptr == 0x0000))) {
				// In case of latching, the contents of the holding register are overwritten
				*tc_register_ptr = ectParent->getMainTimerValue();
				if (ectParent->isBufferEnabled()) {
					*tch_register_ptr = *tc_register_ptr;
				}
			}

			/**
			 *  ICSYS::TFMOD=0 : The timer flags C3F-C0F in the TFLG1 are set
			 *                 when a valid input capture transition on the corresponding port pin occurs.
			 *  ICSYS::LATQ=1  : If the queue mode is not engaged, the timer flags C3F–C0F are set the same way as for TFMOD = 0.
			 */
			ectParent->setTimerInterruptFlag(ioc_index);
			ectParent->assertInterrupt(ectParent->getInterruptOffsetChannel0() - (ioc_index * 2));

		}
		else // IC Queue Mode (ICSYS::LATQ=0)
		{
			if (!ectParent->isNoInputCaptureOverWrite(ioc_index) || ((*tc_register_ptr == 0x0000) && (*tch_register_ptr == 0x0000))) {
				if (ectParent->isBufferEnabled()) {
					*tch_register_ptr = *tc_register_ptr;
					/**
					 *  ICSYS::TFMOD=1 : If in queue mode (BUFEN = 1 and LATQ = 0), the timer flags C3F–C0F in TFLG1 are set only when a latch
					 *                      on the corresponding holding register occurs.
					 *                   If the queue mode is not engaged, the timer flags C3F–C0F are set the same way as for TFMOD = 0.
					 */

					if ((ectParent->getTimerFlagSettingMode() != 0) && (*tch_register_ptr != 0x0000)) {
						ectParent->setTimerInterruptFlag(ioc_index);
						ectParent->assertInterrupt(ectParent->getInterruptOffsetChannel0() - (ioc_index * 2));
					}
				}
				*tc_register_ptr = ectParent->getMainTimerValue();
			}
		}

	}
}

void ECT::IOC_Channel_t::latchToHoldingRegisters() {

	/**
	 * The contents of the input capture registers TC0 to TC3
	 * and their corresponding 8-bit pulse accumulators to be latched into the associated holding registers.
	 */

	*tch_register_ptr = *tc_register_ptr;
	*tc_register_ptr = 0x0000;

	pulseAccumulator->latchToHoldingRegisters();
}

void ECT::IOC_Channel_t::RunCaptureCompare(bool forced) {
	/**
	 * This method is called by writing to "CFORC" register or by starting output compare thread.
	 * 1. Run the action which is programmed for output compare "x" to occur immediately.
	 * 2. if (forced=true) then
	 * 2.1.  The interrupt flag does not get set.
	 * 2.2   The forced output compare take precedence over a successful channel 7 output compare
	 */

	if (ectParent->isOutputCompareMaskEnabled(ioc_index)) {
		/**
		 * TODO:
		 * The corresponding OC7Dx bit in the OC7D register (output compare 7 data) will be transfered to the timer port
		 * on a successful channel 7 output compare.
		 */
	} else {

		switch (getOutputAction()) {
		case 0: /* Timer disconnected from output pin logic */ break;
		case 1: {
			// Toggle OCx output line
			// TODO:
		} break;
		case 2: {
			// Clear OCx output line to zero
			*channelPinLogic = true;
		} break;
		case 3: {
			// Set OCx output line to one

		} break;
		default: /* output action is encoded in TCTL1/TCTL2 registers on 2-bits */;
		}
	}

	/**
	 * check if channel corresponding OC7Dx bit in the output compare 7 data register (OC7D)
	 * will be transfered to the timer port on a successful channel 7 output compare ?
	 */
	if (ectParent->transferOutputCompareToTimerPort(ioc_index)) {

	}
}

//=====================================================================
//=                  Client/Service setup methods                     =
//=====================================================================

bool ECT::BeginSetup() {

	char buf[80];

	sprintf(buf, "%s.TIOS",name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &tios_register);

	sprintf(buf, "%s.CFORC",name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &cforc_register);

	sprintf(buf, "%s.OC7M",name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &oc7m_register);

	sprintf(buf, "%s.OC7D",name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &oc7d_register);

	sprintf(buf, "%s.TCNT",name());
	registers_registry[buf] = new SimpleRegister<uint16_t>(buf, &tcnt_register);

	sprintf(buf, "%s.TSCR1",name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &tscr1_register);

	sprintf(buf, "%s.TTOF",name());
	registers_registry[buf] = new SimpleRegister<uint16_t>(buf, &ttof_register);

	sprintf(buf, "%s.TCTL12",name());
	registers_registry[buf] = new SimpleRegister<uint16_t>(buf, &tctl12_register);

	sprintf(buf, "%s.TCTL34",name());
	registers_registry[buf] = new SimpleRegister<uint16_t>(buf, &tctl34_register);

	sprintf(buf, "%s.TIE",name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &tie_register);

	sprintf(buf, "%s.TSCR2",name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &tscr2_register);

	sprintf(buf, "%s.TFLG1",name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &tflg1_register);

	sprintf(buf, "%s.TFLG2",name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &tflg2_register);

	sprintf(buf, "%s.TC0",name());
	registers_registry[buf] = new SimpleRegister<uint16_t>(buf, &tc_registers[0]);

	sprintf(buf, "%s.TC1",name());
	registers_registry[buf] = new SimpleRegister<uint16_t>(buf, &tc_registers[1]);

	sprintf(buf, "%s.TC2",name());
	registers_registry[buf] = new SimpleRegister<uint16_t>(buf, &tc_registers[2]);

	sprintf(buf, "%s.TC3",name());
	registers_registry[buf] = new SimpleRegister<uint16_t>(buf, &tc_registers[3]);

	sprintf(buf, "%s.TC4",name());
	registers_registry[buf] = new SimpleRegister<uint16_t>(buf, &tc_registers[4]);

	sprintf(buf, "%s.TC5",name());
	registers_registry[buf] = new SimpleRegister<uint16_t>(buf, &tc_registers[5]);

	sprintf(buf, "%s.TC6",name());
	registers_registry[buf] = new SimpleRegister<uint16_t>(buf, &tc_registers[6]);

	sprintf(buf, "%s.TC7",name());
	registers_registry[buf] = new SimpleRegister<uint16_t>(buf, &tc_registers[7]);

	sprintf(buf, "%s.PACTL",name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &pactl_register);

	sprintf(buf, "%s.PAFLG",name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &paflg_register);

	uint8_t pacn_number;

	for (uint8_t i=0; i<4; i++) {
		sprintf(buf, "%s.PACN%d",name(), i);
#if BYTE_ORDER == BIG_ENDIAN
		registers_registry[buf] = new SimpleRegister<uint8_t>(buf, ((uint8_t*) (&pacn_register[i]+1)));
#else
		registers_registry[buf] = new SimpleRegister<uint8_t>(buf, ((uint8_t*) &pacn_register[i]));
#endif

		sprintf(buf, "%s.PA%dH",name(), i);

#if BYTE_ORDER == BIG_ENDIAN
		registers_registry[buf] = new SimpleRegister<uint8_t>(buf, ((uint8_t*) (&paxh_registers[i+1])));
#else
		registers_registry[buf] = new SimpleRegister<uint8_t>(buf, ((uint8_t*) &paxh_registers[i]));
#endif

	}

	sprintf(buf, "%s.MCCTL",name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &mcctl_register);

	sprintf(buf, "%s.MCFLG",name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &mcflg_register);

	sprintf(buf, "%s.ICPAR",name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &icpar_register);

	sprintf(buf, "%s.DLYCT",name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &dlyct_register);

	sprintf(buf, "%s.ICOVW",name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &icovw_register);

	sprintf(buf, "%s.ICSYS",name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &icsys_register);

	sprintf(buf, "%s.TIMTST",name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &timtst_register);

	sprintf(buf, "%s.PTPSR",name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &ptpsr_register);

	sprintf(buf, "%s.PTMCPSR",name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &ptmcpsr_register);

	sprintf(buf, "%s.PBCTL",name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &pbctl_register);

	sprintf(buf, "%s.PBFLG",name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &pbflg_register);

	sprintf(buf, "%s.MCCNT",name());
	registers_registry[buf] = new SimpleRegister<uint16_t>(buf, &mccnt_register);

	sprintf(buf, "%s.TC0H",name());
	registers_registry[buf] = new SimpleRegister<uint16_t>(buf, &tcxh_registers[0]);

	sprintf(buf, "%s.TC1H",name());
	registers_registry[buf] = new SimpleRegister<uint16_t>(buf, &tcxh_registers[1]);

	sprintf(buf, "%s.TC2H",name());
	registers_registry[buf] = new SimpleRegister<uint16_t>(buf, &tcxh_registers[2]);

	sprintf(buf, "%s.TC3H",name());
	registers_registry[buf] = new SimpleRegister<uint16_t>(buf, &tcxh_registers[3]);


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
		for (uint8_t i=0; i<4; i++) { pacn_register[i] = 0x00; }
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

		prnt_write = false;
		icsys_write = false;
		main_timer_enabled = false;
		down_counter_enabled = false;
		delay_counter_enabled = false;

		for (uint8_t i=0; i<8; i++) { pinLogic[i] = false; }

}

void ECT::ComputeInternalTime() {

	bus_cycle_time = sc_time((double)bus_cycle_time_int, SC_PS);

	edge_detector_period = sc_time((double) edge_detector_period_int, SC_PS);

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


