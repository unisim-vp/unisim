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

/*
 * This module implement the S12ECT16B8C_V2 controler
 */

#ifndef __UNISIM_COMPONENT_TLM2_PROCESSOR_ECT_HH__
#define __UNISIM_COMPONENT_TLM2_PROCESSOR_ECT_HH__

#include <inttypes.h>
#include <iostream>
#include <cmath>
#include <map>

#include <systemc.h>

#include <tlm.h>
#include <tlm_utils/tlm_quantumkeeper.h>
#include <tlm_utils/peq_with_get.h>
#include "tlm_utils/simple_target_socket.h"

#include "unisim/kernel/logger/logger.hh"
#include <unisim/kernel/service/service.hh>
#include "unisim/kernel/tlm2/tlm.hh"

#include "unisim/service/interfaces/memory.hh"
#include "unisim/service/interfaces/registers.hh"
#include "unisim/service/interfaces/trap_reporting.hh"

#include "unisim/util/debug/register.hh"

#include <unisim/component/cxx/processor/hcs12x/config.hh>
#include <unisim/component/cxx/processor/hcs12x/types.hh>

#include <unisim/component/tlm2/processor/hcs12x/tlm_types.hh>

namespace unisim {
namespace component {
namespace tlm2 {
namespace processor {
namespace hcs12x {

using namespace std;
using namespace tlm;
using namespace tlm_utils;

using unisim::kernel::logger::Logger;
using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugError;
using unisim::kernel::logger::EndDebug;

using unisim::kernel::service::Object;
using unisim::kernel::service::Client;
using unisim::kernel::service::Service;
using unisim::kernel::service::ServiceExport;
using unisim::kernel::service::ServiceImport;
using unisim::kernel::service::ServiceExportBase;
using unisim::kernel::service::Parameter;
using unisim::kernel::service::RegisterArray;
using unisim::service::interfaces::TrapReporting;

using unisim::service::interfaces::Memory;
using unisim::service::interfaces::Registers;

using unisim::util::debug::Register;

using unisim::component::cxx::processor::hcs12x::ADDRESS;
using unisim::component::cxx::processor::hcs12x::address_t;
using unisim::component::cxx::processor::hcs12x::physical_address_t;
using unisim::component::cxx::processor::hcs12x::service_address_t;
using unisim::component::cxx::processor::hcs12x::CONFIG;

using unisim::kernel::service::Object;
using unisim::kernel::tlm2::PayloadFabric;

class ECT :
	public sc_module,
	virtual public tlm_bw_transport_if<XINT_REQ_ProtocolTypes>,
	public Client<TrapReporting >,
	public Service<Memory<service_address_t> >,
	public Service<Registers>,
	public Client<Memory<service_address_t> >

{
public:

	//=========================================================
	//=                REGISTERS OFFSETS                      =
	//=========================================================

	enum REGS_OFFSETS {TIOS, CFORC, OC7M, OC7D, TCNT_HIGH, TCNT_LOW, TSCR1,
						TTOF, TCTL1, TCTL2, TCTL3, TCTL4, TIE, TSCR2, TFLG1, TFLG2,
						TC0_HIGH, TC0_LOW, TC1_HIGH, TC1_LOW, TC2_HIGH, TC2_LOW,
						TC3_HIGH, TC3_LOW, TC4_HIGH, TC4_LOW, TC5_HIGH, TC5_LOW,
						TC6_HIGH, TC6_LOW, TC7_HIGH, TC7_LOW, PACTL, PAFLG,
						PACN3, PACN2, PACN1, PACN0, MCCTL, MCFLG, ICPAR, DLYCT,
						ICOVW, ICSYS, RESERVED, TIMTST, PTPSR, PTMCPSR, PBCTL,
						PBFLG, PA3H, PA2H, PA1H, PA0H, MCCNT_HIGH, MCCNT_LOW,
						TC0H_HIGH, TC0H_LOW, TC1H_HIGH, TC1H_LOW,
						TC2H_HIGH, TC2H_LOW, TC3H_HIGH, TC3H_LOW};

	//=========================================================
	//=                MODULE INTERFACE                       =
	//=========================================================

	ServiceImport<TrapReporting > trap_reporting_import;

	tlm_initiator_socket<CONFIG::EXTERNAL2UNISIM_BUS_WIDTH, XINT_REQ_ProtocolTypes> interrupt_request;

	tlm_utils::simple_target_socket<ECT> slave_socket;

	tlm_utils::simple_target_socket<ECT> bus_clock_socket;

	ServiceExport<Memory<service_address_t> > memory_export;
	ServiceImport<Memory<service_address_t> > memory_import;
	ServiceExport<Registers> registers_export;

	// the kernel logger
	unisim::kernel::logger::Logger *logger;

	ECT(const sc_module_name& name, Object *parent = 0);
	virtual ~ECT();

	void RunInputCapture();
	void RunMainTimerCounter();
	inline void main_timer_enable();
	inline void main_timer_disable();

	void RunCaptureCompare();

	void RunDownCounter();
	inline void down_counter_enable() { down_counter_enabled = true; down_counter_enable_event.notify(); }
	inline void down_counter_disable() {down_counter_enabled = false; }

	inline void delay_counter_enable() { delay_counter_enabled = true; delay_counter_enable_event.notify(); }
	inline void delay_counter_disable() { delay_counter_enabled = false; }
	inline bool isDelayCounterEnabled() { return delay_counter_enabled; }

	inline void enterWaitMode();
	inline void exitWaitMode();
	inline void enterFreezeMode();
	inline void exitFreezeMode();

	void assertInterrupt(uint8_t interrupt_offset);
	void updateCRGClock(tlm::tlm_generic_payload& trans, sc_time& delay);

	inline void latchToHoldingRegisters();
	inline void loadRegisterToModulusCounterCountregister();
	inline void computeDelayCounter();

    //================================================================
    //=                    tlm2 Interface                            =
    //================================================================
	virtual void invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range);
	virtual tlm_sync_enum nb_transport_bw( XINT_Payload& payload, tlm_phase& phase, sc_core::sc_time& t);

    virtual void read_write( tlm::tlm_generic_payload& trans, sc_time& delay );

	//=====================================================================
	//=                  Client/Service setup methods                     =
	//=====================================================================

	virtual bool BeginSetup();
	virtual bool Setup(ServiceExportBase *srv_export);
	virtual bool EndSetup();

	virtual void OnDisconnect();
	virtual void Reset();


	//=====================================================================
	//=             memory interface methods                              =
	//=====================================================================

	virtual bool ReadMemory(service_address_t addr, void *buffer, uint32_t size);
	virtual bool WriteMemory(service_address_t addr, const void *buffer, uint32_t size);

	//=====================================================================
	//=             Registers Interface interface methods               =
	//=====================================================================

	/**
	 * Gets a register interface to the register specified by name.
	 *
	 * @param name The name of the requested register.
	 * @return A pointer to the RegisterInterface corresponding to name.
	 */
    virtual Register *GetRegister(const char *name);

	//=====================================================================
	//=             registers setters and getters                         =
	//=====================================================================
    bool read(uint8_t offset, unsigned char* value, uint8_t size);
    bool write(uint8_t offset, unsigned char* value, uint8_t size);


protected:
    inline bool isInputCapture(uint8_t channel_index);
    inline bool transferOutputCompareToTimerPort(uint8_t channel_index);
    inline void setTimerinterruptFlag(uint8_t ioc_index) { tflg1_register = tflg1_register | (1 << ioc_index); }
    bool isNoInputCaptureOverWrite(uint8_t ioc_index) { return ((icovw_register & (1 << ioc_index)) != 0);  }
    uint16_t getMainTimerValue() { return tcnt_register; }
    bool isLatchMode() { return ((icsys_register & 0x01) != 0); }
    bool isBufferEnabled() { return ((icsys_register & 0x02) != 0); }
    bool isPulseAccumulatorsMaximumCount() { return ((icsys_register & 0x04) != 0); }

	/**
	 * To operate the 16-bit pulse accumulators A and B (PACA and PACB) independently of input capture or
	 * output compare 7 and 0 respectively, the user must set the corresponding bits: IOSx = 1, OMx = 0, and
	 * OLx = 0. OC7M7 or OC7M0 in the OC7M register must also be cleared
	 */

    bool isPulseAccumulatorAEnabled() {
    	return ((pactl_register & 0x40) != 0) &&
    			((tios_register & 0x0C) != 0) &&
    			((tctl12_register & 0x00F0) == 0) &&
    			((oc7m_register & 0x80) == 0);
    }

    bool isPulseAccumulatorBEnabled() {
    	return ((pbctl_register & 0x40) != 0) &&
    			((tios_register & 0x03) != 0) &&
    			((tctl12_register & 0x000F) == 0) &&
    			((oc7m_register & 0x01) == 0);
    }

    bool isPulseAccumulators8BitEnabled(uint8_t pac_index) {
		if ((icpar_register & (1 << pac_index)) != 0) {
	    	if (pac_index < 2) {
	    		if (!isPulseAccumulatorAEnabled()) {
	    			return true;
	    		}
	    	} else {
	    		if (!isPulseAccumulatorBEnabled()) {
	    			return true;
	    		}
	    	}
		}

    	return false;
    }

    bool isOutputCompareMaskEnabled(uint8_t ioc_index) { return ((oc7m_register & (1 << ioc_index)) != 0); }
    uint8_t getInterruptOffsetChannel0() { return interrupt_offset_channel0; }
    uint8_t getInterruptPulseAccumulatorAOverflow() { return interrupt_pulse_accumulatorA_overflow; }
    uint8_t getInterruptPulseAccumulatorBOverflow() { return interrupt_pulse_accumulatorB_overflow; }
    bool isInputOutputInterruptEnabled(uint8_t ioc_index) { return ((tie_register & (1 << ioc_index)) != 0); }
    bool isTimerOverflowinterruptEnabled() { return ((tscr2_register & 0x80) != 0); }
    bool isTimerCounterResetEnabled() { return ((tscr2_register & 0x08) != 0); }
    uint8_t getTimerFlagSettingMode() { return (icsys_register & 0x08) >> 3; }
    void setPulseAccumulatorAOverflowFlag() { paflg_register = paflg_register | 0x02; }
    void setPulseAccumulatorBOverflowFlag() { pbflg_register = pbflg_register | 0x02; }

    /**
     * isValidEdge() method model the "Edge Detector" circuit
     */
    bool isValidEdge(uint8_t channel_index, uint8_t& edgeValue, uint16_t& edgeDelay) {
		bool isValid = false;
		if (buildin_edge_generator) {
			/* generate edge code */
			edgeValue = (uint8_t) rand() % 3;
			if (edge_delay_counter > 0) {
				edgeDelay = (uint16_t) rand() % (edge_delay_counter * 2);
			}

			/**
			 * TODO:
			 * - I have to choose edge frequency to emulate physical environment without causing system troubleshooting.
			 * - I have to take into account the edge delay to generate next edge.
			 */

		} else {
			if (pinLogic[channel_index]) {
				edgeValue = 1; // rising edge
			} else {
				edgeValue = 2; // falling edge
			}
			if (channel_index < 4) {
				edgeDelay = edgeDelayArray[channel_index];
			}
		}


		if ((edgeValue == ioc_channel[channel_index]->getValideEdge()) ||
				((ioc_channel[channel_index]->getValideEdge() == 3) && ((edgeValue == 1) || (edgeValue == 2))))
		{
			// channels [0,3] are associated with a delay counter
			isValid = true;
			if (channel_index < 4) {
				if (isDelayCounterEnabled()) {
					if (edgeDelay < edge_delay_counter) {
						isValid = false;
					}
				}
			}

		}

		return isValid;
    }

    uint8_t getClockSelection() { return (pactl_register & 0x0C) >> 2; }

	sc_time edge_detector_period;

private:
	inline void ComputeInternalTime();
	inline void configureEdgeDetector();
    inline void configureOutputAction();

	tlm_quantumkeeper quantumkeeper;
	PayloadFabric<XINT_Payload> xint_payload_fabric;


	double	bus_cycle_time_int;	// The time unit is PS
	Parameter<double>	param_bus_cycle_time_int;
	sc_time		bus_cycle_time;

	uint16_t	edge_delay_counter;

	address_t	baseAddress;
	Parameter<address_t>   param_baseAddress;

	// Enhanced Capture Timer Channel 0 : IVBR + 0xEE
	// Enhanced Capture Timer Channel 1 : IVBR + 0xEC
	// Enhanced Capture Timer Channel 2 : IVBR + 0xEA
	// Enhanced Capture Timer Channel 3 : IVBR + 0xE8
	// Enhanced Capture Timer Channel 4 : IVBR + 0xE6
	// Enhanced Capture Timer Channel 5 : IVBR + 0xE4
	// Enhanced Capture Timer Channel 6 : IVBR + 0xE2
	// Enhanced Capture Timer Channel 7 : IVBR + 0xE0

	uint8_t interrupt_offset_channel0;
	Parameter<uint8_t> param_interrupt_offset_channel0;

	uint8_t interrupt_offset_timer_overflow;
	Parameter<uint8_t> param_interrupt_offset_timer_overflow;

	uint8_t interrupt_pulse_accumulatorA_overflow;
	Parameter<uint8_t> param_interrupt_pulse_accumulatorA_overflow;

	uint8_t interrupt_pulse_accumulatorB_overflow;
	Parameter<uint8_t> param_interrupt_pulse_accumulatorB_overflow;

	uint8_t interrupt_pulse_accumulatorA_input_edge;
	Parameter<uint8_t> param_interrupt_pulse_accumulatorA_input_edge;

	bool	debug_enabled;
	Parameter<bool>	param_debug_enabled;

	bool	buildin_edge_generator;
	Parameter<bool>	param_buildin_edge_generator;

	uint64_t	edge_detector_period_int;
	Parameter<uint64_t>	param_edge_detector_period;

	// Registers map
	map<string, Register *> registers_registry;

	bool prnt_write; // TSCR1::PRNT is write once bit
	bool icsys_write; // ICSYS register is write once in normal mode

	sc_event pulse_accumulatorA_enable_event;
	sc_event pulse_accumulatorB_enable_event;

	bool main_timer_enabled;
	sc_event main_timer_enable_event;
	bool down_counter_enabled;
	sc_event down_counter_enable_event;
	bool delay_counter_enabled;
	sc_event delay_counter_enable_event;

	//==============================
	//=            REGISTER SET    =
	//==============================

	uint8_t	tios_register, cforc_register, oc7m_register, oc7d_register, pacn_register[4], paxh_registers[4],
			tscr1_register, tie_register, tscr2_register, tflg1_register, tflg2_register,
			pactl_register, paflg_register, mcctl_register, mcflg_register, icpar_register,
			dlyct_register, icovw_register, icsys_register, reserved_address, timtst_register,
			ptpsr_register, ptmcpsr_register, pbctl_register, pbflg_register;

	uint16_t tcnt_register, ttof_register, tctl12_register, tctl34_register,
			tc_registers[8], mccnt_load_register, mccnt_register, tcxh_registers[4];

	bool pinLogic[8];
	RegisterArray<bool> pinLogic_reg;
	uint16_t edgeDelayArray[4];
	RegisterArray<uint16_t> edgeDelayArray_reg;

	class PulseAccumulator8Bit {
	public:
		PulseAccumulator8Bit(ECT *parent, const uint8_t pacn_number, uint8_t *pacn_ptr, uint8_t* pah_ptr);

		void latchToHoldingRegisters();
		bool countEdge8Bit();
		void clearPACN() {	*pacn_register_ptr = 0x00;}
		uint8_t getIndex() { return pacn_index; }

	protected:

	private:
		uint8_t pacn_index;

		ECT	*ectParent;

		uint8_t *pacn_register_ptr;
		uint8_t* pah_register_ptr;

	};

	PulseAccumulator8Bit* pc8bit[4];

	class PulseAccumulator16Bit : public sc_module {
	public:
		PulseAccumulator16Bit(const sc_module_name& name, ECT *parent, PulseAccumulator8Bit *pacn_high, PulseAccumulator8Bit *pacn_low);

		void process();
		void latchToHoldingRegisters();
		bool countEdge16Bit();

		virtual void RunPulseAccumulator() = 0;
		virtual void wakeup() = 0;

	protected:
		ECT	*ectParent;
		sc_event pulse_accumulator_enable_event;

	private:
		PulseAccumulator8Bit *pacn_high_ptr;
		PulseAccumulator8Bit *pacn_low_ptr;

	};

	class PulseAccumulatorA : public PulseAccumulator16Bit {
	public:
		PulseAccumulatorA(const sc_module_name& name, ECT *parent, PulseAccumulator8Bit *pacn_high, PulseAccumulator8Bit *pacn_low);

		virtual void RunPulseAccumulator();
		virtual void wakeup() {
			 if (ectParent->isPulseAccumulatorAEnabled()) {
				 pulse_accumulator_enable_event.notify();
			 }
		}

	protected:

	private:

	} *pacA;

	class PulseAccumulatorB : public PulseAccumulator16Bit {
	public:
		PulseAccumulatorB(const sc_module_name& name, ECT *parent, PulseAccumulator8Bit *pacn_high, PulseAccumulator8Bit *pacn_low);

		virtual void RunPulseAccumulator();
		virtual void wakeup() {
			 if (ectParent->isPulseAccumulatorBEnabled()) {
				 pulse_accumulator_enable_event.notify();
			 }
		}

	protected:

	private:

	} *pacB;

	/**
	 * TODO: Each pair of pulse accumulator can be used as a 16-bit pulse accumulator.
	 */

	class IOC_Channel_t {
	public:

		IOC_Channel_t(ECT *parent, const uint8_t index, bool* pinLogic, uint16_t *tc_ptr, uint16_t* tch_ptr, PulseAccumulator8Bit* pc8bit);

		void RunCaptureCompare(bool forced);
		void latchToHoldingRegisters();
		void RunInputCapture(uint16_t edgeDelay);
		void setValideEdge(uint8_t edgeConfig) { valideEdge = edgeConfig; }
		uint8_t getValideEdge() { return valideEdge; }
		void setOutputAction(uint8_t outputAction) { this->outputAction = outputAction; };
		uint8_t getOutputAction() { return outputAction; };

	private:
		uint8_t ioc_index;
		uint8_t iocMask;
		uint8_t valideEdge;
		uint8_t outputAction;

		ECT	*ectParent;

		uint16_t* tc_register_ptr;
		uint16_t* tch_register_ptr;
		PulseAccumulator8Bit* pulseAccumulator;
		bool* channelPinLogic;

	} *ioc_channel[8];


}; /* end class ECT */

} // end of namespace hcs12x
} // end of namespace processor
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif /*__UNISIM_COMPONENT_TLM2_PROCESSOR_ECT_HH__*/
