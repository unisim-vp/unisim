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
#include <unisim/component/tlm2/processor/hcs12x/xint.hh>

namespace unisim {
namespace component {
namespace tlm2 {
namespace processor {
namespace hcs12x {

CRG::CRG(const sc_module_name& name, Object *parent) :
	Object(name, parent)
	, sc_module(name)

	, unisim::kernel::Client<TrapReporting>(name, parent)
	, unisim::kernel::Service<Memory<physical_address_t> >(name, parent)
	, unisim::kernel::Service<Registers>(name, parent)
	, unisim::kernel::Client<Memory<physical_address_t> >(name, parent)

	, trap_reporting_import("trap_reporting_import", this)

	, bus_clock_socket("Bus-Clock")
	, slave_socket("slave_socket")

	, memory_export("memory_export", this)
	, memory_import("memory_import", this)
	, registers_export("registers_export", this)

	, oscillator_clock_value(250000)
	, param_oscillator_clock_int("oscillator-clock", this, oscillator_clock_value)

	, osc_ok_int(4096)
	, param_osc_ok("osc-ok", this, osc_ok_int, "A number greater equal than 4096 rising OSCCLK edges within a check window is called osc ok.")
	, osc_fail(false)
	, var_osc_fail("osc-fail", this, osc_fail, "Oscillator instrument. Used to emulate oscillator fail and trigger the clock monitor.")

	, check_window_int(50000)
	, param_check_window("check_window", this, check_window_int, "A time window of 50,000 VCO clock cycles is called check window.\n VCO clock cycles are generated by the PLL when running at minimum frequency Fscm.")

	, pll_stabilization_delay_value(0.24)
	, param_pll_stabilization_delay("pll-stabilization-delay", this, pll_stabilization_delay_value, "Total PLL stabilization delay (acquisition + tracking). e.g. for Fosc = 4MHz and REFDV = #$00, SYNR = #$09 the acquisition_delay = 0.09 ms and tracking_delay = 0.16 ms")

	, self_clock_mode_value(100000)
	, param_self_clock_mode_clock("param-self-clock-mode-clock", this, self_clock_mode_value, "Self Clock Mode frequency between 1MHz and 5.5 MHz (unit PS)")

	, armcop_write_enabled(false)
	, clock_monitor_enabled(true)

	, cop_timeout_reset(false)
	, cop_timeout_restart(false)
	, copwai_write(false)

	, rti_enabled(true)
	, cop_enabled(true)
	, scme_write(false)

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

	, rti_fdr(1)

	, synr_register(0x09)
	, refdv_register(0x00)
	, ctflg_register(0x00)
	, crgflg_register(0x00)
	, crgint_register(0x00)
	, clksel_register(0x00)
	, pllctl_register(0xF1)
	, rtictl_register(0x00)
	, copctl_register(0x00)
	, forbyp_register(0x00)
	, ctctl_register(0x00)
	, armcop_register(0x00)

{

	param_oscillator_clock_int.SetFormat(unisim::kernel::VariableBase::FMT_DEC);
	param_self_clock_mode_clock.SetFormat(unisim::kernel::VariableBase::FMT_DEC);
	param_pll_stabilization_delay.SetFormat(unisim::kernel::VariableBase::FMT_DEC);
	param_check_window.SetFormat(unisim::kernel::VariableBase::FMT_DEC);
	param_osc_ok.SetFormat(unisim::kernel::VariableBase::FMT_DEC);

	var_osc_fail.SetMutable(true);
	var_osc_fail.SetSerializable(false);
	var_osc_fail.SetVisible(true);

	interrupt_request(*this);

	slave_socket.register_b_transport(this, &CRG::read_write);

//	reset_socket.register_b_transport(this, &CRG::reset_handler);

	SC_HAS_PROCESS(CRG);

	SC_THREAD(runPLL_LockTrack_Detector);
	SC_THREAD(runRTI);
	SC_THREAD(runCOP);
	SC_THREAD(runClockMonitor);

	xint_payload = xint_payload_fabric.allocate();
	bus_clk_trans = payloadFabric.allocate();

}

CRG::~CRG() {

	xint_payload->release();
	bus_clk_trans->release();

	// Release registers_registry
	unsigned int i;
	unsigned int n = extended_registers_registry.size();
	for (i=0; i<n; i++) {
		delete extended_registers_registry[i];
	}

}


void CRG::read_write( tlm::tlm_generic_payload& trans, sc_time& delay )
{
	tlm::tlm_command cmd = trans.get_command();
	sc_dt::uint64 address = trans.get_address();
	uint8_t* data_ptr = (uint8_t *)trans.get_data_ptr();
	unsigned int data_length = trans.get_data_length();

	if ((address >= baseAddress) && (address < (baseAddress + MEMORY_MAP_SIZE))) {

		if (cmd == tlm::TLM_READ_COMMAND) {
			memset(data_ptr, 0, data_length);
			read(address - baseAddress, data_ptr, data_length);
		} else if (cmd == tlm::TLM_WRITE_COMMAND) {
			write(address - baseAddress, data_ptr, data_length);
		}

		trans.set_response_status( tlm::TLM_OK_RESPONSE );

	} else {
		trans.set_response_status( tlm::TLM_INCOMPLETE_RESPONSE );
	}

}

bool CRG::read(unsigned int offset, const void *buffer, unsigned int data_length) {

	switch (offset) {
		case SYNR: *((uint8_t *) buffer) = synr_register & 0x3F; break;
		case REFDV: *((uint8_t *) buffer) = refdv_register & 0x3F; break;
		case CTFLG: *((uint8_t *) buffer) = 0; break;
		case CRGFLG: *((uint8_t *) buffer) = crgflg_register; break;
		case CRGINT: *((uint8_t *) buffer) = crgint_register & 0xD2; break;
		case CLKSEL: *((uint8_t *) buffer) = clksel_register & 0xCB; break;
		case PLLCTL: *((uint8_t *) buffer) = pllctl_register; break;
		case RTICTL: *((uint8_t *) buffer) = rtictl_register; break;
		case COPCTL: *((uint8_t *) buffer) = copctl_register & 0xC7; break;
		case FORBYP: *((uint8_t *) buffer) = 0; break;
		case CTCTL: *((uint8_t *) buffer) = 0; break;
		case ARMCOP: *((uint8_t *) buffer) = 0; break;
		default: return (false);
	}

	return (true);
}

//=====================================================================
//=             registers setters and getters                         =
//=====================================================================

bool CRG::write(unsigned int offset, const void *buffer, unsigned int data_length) {

	uint8_t value = *((uint8_t *) buffer);
	switch (offset) {
		case SYNR: {
			 // if (PLLSEL == 1) then return;
			if ((clksel_register & 0x80) != 0) {
				return (true);
			}

			synr_register = value & 0x3F;

			/**
			 * Write to SYNR register initializes the lock detector bit and the track detector bit.
			 */
			lock_track_detector_event.notify();


		} break;
		case REFDV: {
			 // if (PLLSEL == 1) then return;
			if ((clksel_register & 0x80) != 0) {
				return (true);
			}

			refdv_register = value & 0x3F;

			// Write to SYNR register initializes the lock detector bit and the track detector bit.
			lock_track_detector_event.notify();

		} break;
		case CTFLG: {
			// This register is reserved for factory testing
			return (true);
		} break;
		case CRGFLG: {

			//  RTIF: set at the end of RTI period
			if ((value & 0x80) != 0) {
				crgflg_register = crgflg_register & 0x7F;
			}

			/** TODO:
			 *  PORF: set when a power on occurs
			 */
			if ((value & 0x40) != 0) {
				crgflg_register = crgflg_register & 0xBF;
			}

			/**
			 * TODO:
			 *  LVRF: set if low voltage reset
			 */
			if ((value & 0x20) != 0) {
				crgflg_register = crgflg_register & 0xDF;
			}

			/**
			 *  LOCKIF: is set when Lock status bit change
			 */
			if ((value & 0x10) != 0) {
				crgflg_register = crgflg_register & 0xEF;
			}

			/**
			 * TODO:
			 *  SCMIF: is set when SCM status bit change
			 */
			if ((value & 0x02) != 0) {
				crgflg_register = crgflg_register & 0xFD;
			}

		} break;
		case CRGINT: {
			/**
			 * TODO:
			 * ILAF bit is set to 1 when an illegal address reset occurs.
			 * ILAF bit is Unaffected by system reset.
			 * ILAF bit is Cleared by power on or low voltage reset.
			 */

			uint8_t val = value & 0xD2;

			uint8_t old_ilaf_bit = crgint_register & 0x40;
			crgint_register = val & 0x92;
			if ((val & 0x40) == 0) {
				crgint_register = crgint_register | old_ilaf_bit;
			}

			if ((val & 0x80) != 0) {
				rti_enabled = true;
				rti_enable_event.notify();
			}

		} break;
		case CLKSEL: {

			uint8_t old_clksel_register = clksel_register;

			value = value & 0xCB;

			// CLKSEL::COPWAI write once in Normal modes
			if (copwai_write) {
				uint8_t copwai_bit = old_clksel_register & 0x01;
				value = (value & 0xFE) | copwai_bit;
			} else {
				copwai_write = true;
			}

			clksel_register = value;

			if (((old_clksel_register & 0x04) == 0) && ((clksel_register & 0x04) != 0)) {
				// While the PLLWAI bit is set, the AUTO bit is set to 1.
				pllctl_register = (pllctl_register | 0x20);
			}

			updateBusClock();


		} break;
		case PLLCTL: {
			uint8_t cme_bit = pllctl_register & 0x80;
			uint8_t pllon_bit = pllctl_register & 0x40;
			uint8_t auto_bit = pllctl_register & 0x20;

			// PLLCTL::SCME write once in Normal modes
			// PLLCTL::SCME can not be cleared while operating in self clock mode (CRGFLG::SCM = 1).
			uint8_t scme_bit = pllctl_register & 0x01;
			if ((scme_write) || ((crgflg_register & 0x01) != 0)) {
				value = (value & 0xFE) | scme_bit;
			} else {
				scme_write = true;
			}

			/**
			 * PLLCTL::CME : write any time except when CRGFLG::SCM=1
			 */
			// check CRGFLG::SCM bit
			if ((crgflg_register & 0x01) != 0) {
				value = (value & 0x7F) | cme_bit;
			}

			// check CLKSEL::PLLSEL
			if ((clksel_register & 0x80) != 0) {
				value = (value & 0xBF) | pllon_bit;
			}

			// check CLKSEl::PLLWAI
			if ((clksel_register & 0x040) != 0) {
				value = (value & 0xDF) | auto_bit;
			}

			pllctl_register = value;

			if ((pllctl_register & 0x80) != 0) {
				// enables the clock monitor
				clockmonitor_enable_event.notify();
			}

			// TODO: implement acquisition and tracking
			if (((pllctl_register & 0x20) != 0) || ((pllctl_register & 0x10) != 0)) {
				// TODO: High bandwidth (acquisition) mode
			} else {
				// TODO: Low bandwidth (tracking) mode
			}

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
				cop_enabled = true;
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
		default: return (false);
	}

	return (true);
}

void CRG::systemReset() {
	assertInterrupt(XINT::INT_SYS_RESET_OFFSET);
}

void CRG::runClockMonitor() {

/**
 * If no OSCCLK edges are detected within a certain time, the clock monitor within the oscillator block
 * generates a clock monitor fail event. The CRG then asserts self clock mode or generates a system reset
 * depending on the state of SCME bit. If the clock monitor is disabled or the presence of clocks is detected
 * no failure is indicated by the oscillator block.The clock monitor function is enabled/disabled by the CME
 * control bit.
 */

	/* initialize random seed: */
	srand ( 12345 );

	/**
	 */

	while (true && osc_fail) {

		while (((pllctl_register & 0x80) == 0) || (!clock_monitor_enabled)) {
			wait(clockmonitor_enable_event);
		}

		while (((pllctl_register & 0x80) != 0) && (clock_monitor_enabled)) {

			wait(osc_ok);

// note: all the prime numbers under 100
//  2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97

// 481th prime number to 500th prime number
// 	3433, 3449, 3457, 3461, 3463, 3467, 3469, 3491, 3499, 3511, 3517, 3527, 3529, 3533, 3539, 3541, 3547, 3557, 3559, 3571

			if ((rand() % 3571) == 0) {
				// check SCME bit
				if ((pllctl_register & 0x01) != 0) {
					activateSelfClockMode();
				} else {
					systemReset();
				}

			} else {
				if ((crgflg_register & 0x01) != 0) {
					deactivateSelfClockMode();
				}
			}

		}

	}

}


void CRG::activateSelfClockMode() {
	// clear CLKSEL::PLLSEL bit
	clksel_register = clksel_register & 0x7F;

	// Set SCM bit to activate self clock mode and set SCMIF interrupt flag
	crgflg_register = crgflg_register | 0x03;

	assertInterrupt(interrupt_offset_self_clock_mode);

	lock_track_detector_event.notify();
}

void CRG::deactivateSelfClockMode() {

	// clear SCM bit to deactivate Self Clock Mode
	crgflg_register = crgflg_register & 0xFE;

	// set SCMIF interrupt flag
	crgflg_register = crgflg_register | 0x02;

	assertInterrupt(interrupt_offset_self_clock_mode);

	lock_track_detector_event.notify();
}

void CRG::activateStopMode() {
	// clear CLKSEL::PLLSEL bit
	clksel_register = clksel_register & 0x7F;

	// check CLKSEL::PSTP bit
	if ((clksel_register & 0x40) != 0) {
		// TODO: Oscillator continues to run in stop mode (pseudo stop)
		/**
		 * Pseudo stop mode allows for faster STOP recovery and reduces the mechanical stress and
		 * aging of the resonator in case of frequent STOP conditions at the expense of a slightly increased power consumption
		 */

		/**
		 *  If the PRE bit is cleared the RTI dividers will go static while pseudo stop mode is active.
		 *  The RTI dividers will not initialize like in wait mode with RTIWAI bit set.
		 */
		if ((pllctl_register & 0x04) == 0) {
			rti_enabled = false;
		}

		/**
		 * If the PCE bit is cleared, the COP dividers will go static while pseudo stop mode is active.
		 * The COP dividers will not initialize like in wait mode with COPWAI bit set
		 */
		if ((pllctl_register & 0x02) == 0) {
			cop_enabled = false;
		}

	} else {
		// TODO: Oscillator is disabled in stop mode.
	}

	// In stop mode the clock monitor is disabled independently of the PLLCTL::CME bit setting
	clock_monitor_enabled = false;
}

void CRG::activateWaitMode() {

	// check CLKSEL::PLLWAI bit
	if ((clksel_register & 0x08) != 0) {
		// clear CLKSEL::PLLSEL bit
		clksel_register = clksel_register & 0x7F;
	}

	// check CLKSEL::RTIWAI
	if ((clksel_register & 0x02) != 0) {
		/**
		 * RTIWAI=1 : RTI stops and initializes the RTI dividers whenever the part goes into wait mode
		 */
		rtictl_register = 0x00;
		crgint_register = crgint_register & 0x7F;

	}

	// check CLKSEL::COPWAI
	if ((clksel_register & 0x01) != 0) {
		/**
		 * COPWAI=1 : COP stops and initializes the COP counter whenever the part goes into wait mode
		 * COPWAI)0 : COP keeps running in wait mode
		 */

		// reset to 0 the COPCTL::WCOP, COPCTL::CRx bits
		copctl_register = copctl_register & 0x60;

	}
}

void CRG::wakeupFromStopMode() {

	// check PLLCTL::FSTWKP and if PLLCTL::SCME = 0 then PLLCTL::FSTWKP has no effect
	clock_monitor_enabled = true;
	if (((pllctl_register & 0x08) !=0) && ((pllctl_register & 0x01) != 0)) {
		clockmonitor_enable_event.notify();
	}

	rti_enabled = true;
	if ((crgint_register & 0x80) != 0) {
		rti_enable_event.notify();
	}

	cop_enabled = true;
	if (((copctl_register & 0x07) != 0) && ((copctl_register & 0x80) != 0))  {
		cop_enable_event.notify();
	}

}

void CRG::runCOP() {

	/**
	 * Windowed COP operation is enabled by setting WCOP bit in the COPCTL register.
	 * When WCOP is set, a write to the ARMCOP register must occur in the last 25% of the selected
	 * period. A write during the first 75% of the selected period will reset the part. As long as all writes occur during
	 * this window, 0x_55 can be written as often as desired. Once 0x_AA is written after the 0x_55, the time-out logic
	 * restarts and the user must wait until the next window before writing to ARMCOP.
	 */

	while (true) {
		while (((copctl_register & 0x07) == 0) || ((copctl_register & 0x80) == 0) || (!cop_enabled))  {
			wait(cop_enable_event);
		}

		// select_cop_timeout
		sc_time cop_timeout;

		// is SCM bit set
		if ((crgflg_register & 0x01) != 0) {
			// PLLCLK at minimum frequency Fscm;
			cop_timeout = pll_clock * (pow((double) 2,14) * (copctl_register & 0x07));
		} else {
			cop_timeout = oscillator_clock * (pow((double) 2,14) * (copctl_register & 0x07));
		}

		sc_time cop_timeout_75 = cop_timeout * 0.75;
		sc_time cop_timeout_25 = cop_timeout - cop_timeout_75;

		// "cop_timeout_reset" and "cop_timeout_restart" values change by writing in the ARMCOP register
		cop_timeout_reset = false;
		cop_timeout_restart = false;
		armcop_write_enabled = false;

		wait(cop_timeout_75);

		if (cop_timeout_reset) {
			// A write into ARMCOP register during the first 75% of the selected period will reset the part
			continue;
		}

		if (((copctl_register & 0x07) == 0) || ((copctl_register & 0x80) == 0)) {
			// If the CRG enter in wait mode and CLKSEL::COPWAI is set to 1
			// the COP stops and has to be restarted manually
			continue;
		}

		armcop_write_enabled = true;

		wait(cop_timeout_25);
		if (!cop_timeout_restart) {
			systemReset();
		}

		cop_timeout_restart = false;
	}
}

void CRG::runRTI() {
	/**
	 * - CRG generates a real-time interrupt when the selected interrupt time period elapses
	 * - RTI interrupts are locally disabled by setting the RTIE bit to 0
	 * - The RTIF is set to 1 when a timeout occurs, and is cleared to 0 by writing 1 to RTIF
	 */

	sc_time delay;

	while (true) {

		while (((crgint_register & 0x80) == 0) || ((rtictl_register & 0x70) == 0) || (!rti_enabled)) {
			wait(rti_enable_event);
		}

		// is SCM bit set
		if ((crgflg_register & 0x01) != 0) {
			// PLLCLK at minimum frequency Fscm;
			delay = pll_clock * rti_fdr;
		} else {
			delay = oscillator_clock * rti_fdr;
		}

		wait(delay);

		crgflg_register = crgflg_register | 0x80;

		assertInterrupt(interrupt_offset_rti);

	}
}

void CRG::assertInterrupt(unsigned int interrupt_offset) {

	if ((interrupt_offset == interrupt_offset_pll_lock) && ((crgint_register & 0x10) == 0)) return;
	if ((interrupt_offset == interrupt_offset_self_clock_mode) && ((crgint_register & 0x02) == 0)) return;

	tlm_phase phase = BEGIN_REQ;

	xint_payload->acquire();

	xint_payload->setInterruptOffset(interrupt_offset);

	sc_time local_time = quantumkeeper.get_local_time();

	tlm_sync_enum ret = interrupt_request->nb_transport_fw(*xint_payload, phase, local_time);

	xint_payload->release();

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
		return (TLM_COMPLETED);
	}
	return (TLM_ACCEPTED);
}

void CRG::runPLL_LockTrack_Detector() {

	while (true) {

		/**
		 * The lock detector compares the frequencies of the FEEDBACK clock, and the REFERENCE clock.
		 * Therefore, the speed of the lock detector is directly proportional to the final reference frequency.
		 * The circuit determines the mode of the PLL and the lock condition based on this comparison
		 */

		wait(lock_track_detector_event);

		int multiply_coef = 2 * (synr_register + 1) / (refdv_register + 1);
		// is SCM bit set
		if ((crgflg_register & 0x01) != 0) {
			// PLLCLK at minimum frequency Fscm;
			pll_clock = self_clock_mode_clock / multiply_coef;
		} else {
			pll_clock = oscillator_clock / multiply_coef;
		}

		/**
		 *  TODO:
		 *  - Implement a mechanism to emulate unstable PLL
		 *  - Implement Lock detector functionality
		 *  - Implement Track detector functionality
		 */
		crgflg_register = crgflg_register & 0xF7; // clear LOCK bit

		assertInterrupt(interrupt_offset_pll_lock);

		wait(pll_stabilization_delay);

		crgflg_register = crgflg_register | 0x08; // set LOCK bit

		crgflg_register = crgflg_register | 0x04; // set TRACK bit

		updateBusClock();

		assertInterrupt(interrupt_offset_pll_lock);

	}

}

void CRG::updateBusClock() {

	/**
	 *  PLL Select Bit:
	 *  Writing a 1 when LOCK = 0 and AUTO = 1, or TRACK = 0 and AUTO = 0 has no effect.
	 */
	if (((clksel_register & 0x80) == 0) ||
			(((crgflg_register & 0x08) == 0) && ((pllctl_register & 0x20) != 0)) ||
			((((crgflg_register & 0x04) == 0) && ((pllctl_register & 0x20) == 0))) ||
			((pllctl_register & 0x40) == 0)) {

		bus_clock = oscillator_clock * 2;

	} else {
		bus_clock = pll_clock * 2;
	}

	// notify bus_clock update
	sc_dt::uint64 bus_clock_value = bus_clock.value();

	bus_clk_trans->acquire();
	bus_clk_trans->set_command( tlm::TLM_WRITE_COMMAND );
	bus_clk_trans->set_data_ptr( (unsigned char *)&bus_clock_value);

	sc_time delay = SC_ZERO_TIME;

	for (unsigned int i = 0; i < bus_clock_socket.size(); i++) {
		bus_clk_trans->set_response_status( tlm::TLM_INCOMPLETE_RESPONSE );

		bus_clock_socket[i]->b_transport( *bus_clk_trans, delay);

		if (bus_clk_trans->is_response_error() )
			SC_REPORT_ERROR("CRG : ", "Response error from b_transport");

	}

	bus_clk_trans->release();
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
			rti_enabled = false;
		}
	}

}

//=====================================================================
//=                  Client/Service setup methods                     =
//=====================================================================

bool CRG::BeginSetup() {


	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".SYNR", &synr_register));

	unisim::kernel::variable::Register<uint8_t> *synr_var = new unisim::kernel::variable::Register<uint8_t>("SYNR", this, synr_register, "CRG Synthesizer Register (SYNR)");
	extended_registers_registry.push_back(synr_var);
	synr_var->setCallBack(this, SYNR, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".REFDV", &refdv_register));

	unisim::kernel::variable::Register<uint8_t> *refdv_var = new unisim::kernel::variable::Register<uint8_t>("REFDV", this, refdv_register, "CRG Reference Divider Register (REFDV)");
	extended_registers_registry.push_back(refdv_var);
	refdv_var->setCallBack(this, REFDV, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".CTFLG", &ctflg_register));

	unisim::kernel::variable::Register<uint8_t> *ctflg_var = new unisim::kernel::variable::Register<uint8_t>("CTFLG", this, ctflg_register, "CRG Test Flags Register (CTFLG)");
	extended_registers_registry.push_back(ctflg_var);
	ctflg_var->setCallBack(this, CTFLG, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".CRGFLG", &crgflg_register));

	unisim::kernel::variable::Register<uint8_t> *crgflg_var = new unisim::kernel::variable::Register<uint8_t>("CRGFLG", this, crgflg_register, "CRG Flags Register (CRGFLG)");
	extended_registers_registry.push_back(crgflg_var);
	crgflg_var->setCallBack(this, CRGFLG, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".CRGINT", &crgint_register));

	unisim::kernel::variable::Register<uint8_t> *crgint_var = new unisim::kernel::variable::Register<uint8_t>("CRGINT", this, crgint_register, "CRG Interrupt Enable Register (CRGINT)");
	extended_registers_registry.push_back(crgint_var);
	crgint_var->setCallBack(this, CRGINT, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".CLKSEL", &clksel_register));

	unisim::kernel::variable::Register<uint8_t> *clksel_var = new unisim::kernel::variable::Register<uint8_t>("CLKSEL", this, clksel_register, "CRG Clock Select Register (CLKSEL)");
	extended_registers_registry.push_back(clksel_var);
	clksel_var->setCallBack(this, CLKSEL, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".PLLCTL", &pllctl_register));

	unisim::kernel::variable::Register<uint8_t> *pllctl_var = new unisim::kernel::variable::Register<uint8_t>("PLLCTL", this, pllctl_register, "CRG PLL Control Register (PLLCTL)");
	extended_registers_registry.push_back(pllctl_var);
	pllctl_var->setCallBack(this, PLLCTL, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".RTICTL", &rtictl_register));

	unisim::kernel::variable::Register<uint8_t> *rtictl_var = new unisim::kernel::variable::Register<uint8_t>("RTICTL", this, rtictl_register, "CRG RTI Control Register (RTICTL)");
	extended_registers_registry.push_back(rtictl_var);
	rtictl_var->setCallBack(this, RTICTL, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".COPCTL", &copctl_register));

	unisim::kernel::variable::Register<uint8_t> *copctl_var = new unisim::kernel::variable::Register<uint8_t>("COPCTL", this, copctl_register, "CRG COP Control Register (COPCTL)");
	extended_registers_registry.push_back(copctl_var);
	copctl_var->setCallBack(this, COPCTL, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".FORBYP", &forbyp_register));

	unisim::kernel::variable::Register<uint8_t> *forbyp_var = new unisim::kernel::variable::Register<uint8_t>("FORBYP", this, forbyp_register, "CRG Force and Bypass Test Register (FORBYP)");
	extended_registers_registry.push_back(forbyp_var);
	forbyp_var->setCallBack(this, FORBYP, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".CTCTL", &ctctl_register));

	unisim::kernel::variable::Register<uint8_t> *ctctl_var = new unisim::kernel::variable::Register<uint8_t>("CTCTL", this, ctctl_register, "CRG Test Control Register (CTCTL)");
	extended_registers_registry.push_back(ctctl_var);
	ctctl_var->setCallBack(this, CTCTL, &CallBackObject::write, NULL);

	registers_registry.AddRegisterInterface(new SimpleRegister<uint8_t>(std::string(sc_object::name()) + ".ARMCOP", &armcop_register));

	unisim::kernel::variable::Register<uint8_t> *armcop_var = new unisim::kernel::variable::Register<uint8_t>("ARMCOP", this, armcop_register, "CRG COP Arm/Timer Reset (ARMCOP)");
	extended_registers_registry.push_back(armcop_var);
	armcop_var->setCallBack(this, ARMCOP, &CallBackObject::write, NULL);

	oscillator_clock = sc_time((double) oscillator_clock_value, SC_PS);

	bus_clock = oscillator_clock * 2;

	// TODO: parameterize this variable or use another mechanism (asynchronous thread or potentiometer)

	/**
	 * The following pll_delta_time value is measured for the following characteristics
	 * fosc = 4 MHz, fBUS = 40 MHz equivalent fVCO = 80 MHz: REFDV = #$00, SYNR = #$09, CS = 4.7 nF, CP = 470 pF, RS = 4.7 kΩ
	 */
	pll_stabilization_delay =  sc_time((double) pll_stabilization_delay_value, SC_MS); // acquisition_delay = 0.09 ms and tracking_delay = 0.16 ms

	self_clock_mode_clock =   sc_time((double) self_clock_mode_value, SC_PS);

	osc_ok = osc_ok_int * oscillator_clock;

//	sc_time		check_window;
//	uint32_t	check_window_int;

	return (true);
}

Register* CRG::GetRegister(const char *name)
{
	return registers_registry.GetRegister(name);
}

void CRG::ScanRegisters(unisim::service::interfaces::RegisterScanner& scanner)
{
	registers_registry.ScanRegisters(scanner);
}

void CRG::OnDisconnect() {
}

void CRG::Reset() {

	synr_register =  0x09;
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

	updateBusClock();

}

void CRG::ResetMemory() {

	Reset();

}

//=====================================================================
//=             memory interface methods                              =
//=====================================================================

bool CRG::ReadMemory(physical_address_t addr, void *buffer, uint32_t size) {

	if ((addr >= baseAddress) && (addr <= (baseAddress + (unsigned int) ARMCOP))) {

		physical_address_t offset = addr-baseAddress;

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
			default: return (false);
		}

		return (true);

	}

	return (false);
}

//bool CRG::WriteMemory(physical_address_t addr, const void *buffer, uint32_t size) {
//
//	if ((addr >= baseAddress) && (addr <= (baseAddress+ARMCOP))) {
//
//		if (size == 0) {
//			return true;
//		}
//
//		physical_address_t offset = addr-baseAddress;
//
//		return write(offset, *(uint8_t *) buffer);
//	}
//
//	return false;
//
//}

bool CRG::WriteMemory(physical_address_t addr, const void *buffer, uint32_t size) {

	if ((addr >= baseAddress) && (addr <= (baseAddress + (unsigned int) ARMCOP))) {

		if (size == 0) {
			return (true);
		}

		physical_address_t offset = addr-baseAddress;

		switch (offset) {
			case SYNR: synr_register = *((uint8_t *) buffer); break;
			case REFDV: refdv_register = *((uint8_t *) buffer); break;
			case CTFLG: ctflg_register = *((uint8_t *) buffer); break;
			case CRGFLG: crgflg_register = *((uint8_t *) buffer); break;
			case CRGINT: crgint_register = *((uint8_t *) buffer); break;
			case CLKSEL: clksel_register = *((uint8_t *) buffer); break;
			case PLLCTL: pllctl_register = *((uint8_t *) buffer); break;
			case RTICTL: rtictl_register = *((uint8_t *) buffer); break;
			case COPCTL: copctl_register = *((uint8_t *) buffer); break;
			case FORBYP: forbyp_register = *((uint8_t *) buffer); break;
			case CTCTL: ctctl_register = *((uint8_t *) buffer); break;
			case ARMCOP: armcop_register = *((uint8_t *) buffer); break;
		}

		return (true);
	}

	return (false);

}


} // end of namespace hcs12x
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

