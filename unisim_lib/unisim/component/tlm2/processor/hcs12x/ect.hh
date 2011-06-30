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

using unisim::kernel::service::Object;
using unisim::kernel::service::Client;
using unisim::kernel::service::Service;
using unisim::kernel::service::ServiceExport;
using unisim::kernel::service::ServiceImport;
using unisim::kernel::service::ServiceExportBase;
using unisim::kernel::service::Parameter;
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

	ECT(const sc_module_name& name, Object *parent = 0);
	virtual ~ECT();

	void start();
	void assertInterrupt(uint8_t interrupt_offset);
	void updateCRGClock(tlm::tlm_generic_payload& trans, sc_time& delay);
	void runCaptureCompareAction();

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

private:
	void ComputeInternalTime();

	tlm_quantumkeeper quantumkeeper;
	PayloadFabric<XINT_Payload> xint_payload_fabric;


	double	bus_cycle_time_int;	// The time unit is PS
	Parameter<double>	param_bus_cycle_time_int;
	sc_time		bus_cycle_time;

	address_t	baseAddress;
	Parameter<address_t>   param_baseAddress;

	uint8_t interrupt_offset_channel0;
	Parameter<uint8_t> param_interrupt_offset_channel0;
	uint8_t interrupt_offset_overflow;
	Parameter<uint8_t> param_interrupt_offset_overflow;

	bool	debug_enabled;
	Parameter<bool>	param_debug_enabled;

	// Registers map
	map<string, Register *> registers_registry;

	//==============================
	//=            REGISTER SET    =
	//==============================

	uint8_t	tios_register, cforc_register, oc7m_register, oc7d_register,
			tscr1_register, tie_register, tscr2_register, tflg1_register, tflg2_register,
			pactl_register, paflg_register, mcctl_register, mcflg_register, icpar_register,
			dlyct_register, icovw_register, icsys_register, reserved_address, timtst_register,
			ptpsr_register, ptmcpsr_register, pbctl_register, pbflg_register, paxh_registers[4];

	uint16_t tcnt_register, ttof_register, tctl12_register, tctl34_register, pacn32_register,
			pacn10_register, tc_registers[8], mccnt_register, tcxh_registers[4];

	class Channel_t : public sc_module {
	public:

		Channel_t(const sc_module_name& name, ECT *parent, const uint8_t channel_number, uint16_t *tc_ptr);

		void Run();
		void runCaptureCompareAction();
		void wakeup();
		void disable();

	private:

		uint16_t *tc_register_ptr;

		uint8_t channel_index;
		uint8_t channelMask;
		ECT	*ectParent;
		sc_event wakeup_event;

		template <class T> void checkChangeStateAndWait(const sc_time clk);

	} *channel[8];

}; /* end class ECT */

} // end of namespace hcs12x
} // end of namespace processor
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif /*__UNISIM_COMPONENT_TLM2_PROCESSOR_ECT_HH__*/
