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

/*
 * This module implement the S12CRG_V6 controler
 */

#ifndef __UNISIM_COMPONENT_TLM2_PROCESSOR_CRG_HH__
#define __UNISIM_COMPONENT_TLM2_PROCESSOR_CRG_HH__

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

class CRG :
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

	enum REGS_OFFSETS {SYNR, REFDV, CTFLG, CRGFLG, CRGINT, CLKSEL, PLLCTL, RTICTL,
					COPCTL, FORBYP, CTCTL, ARMCOP};

	ServiceImport<TrapReporting > trap_reporting_import;

	tlm_initiator_socket<CONFIG::EXTERNAL2UNISIM_BUS_WIDTH, XINT_REQ_ProtocolTypes> interrupt_request;

	tlm_utils::simple_target_socket<CRG> slave_socket;

	ServiceExport<Memory<service_address_t> > memory_export;
	ServiceImport<Memory<service_address_t> > memory_import;
	ServiceExport<Registers> registers_export;

	CRG(const sc_module_name& name, Object *parent = 0);
	virtual ~CRG();

	void RunRTI();

	void assertInterrupt(uint8_t interrupt_offset);

    //================================================================
    //=                    tlm2 Interface                            =
    //================================================================
	virtual void invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range);
	virtual tlm_sync_enum nb_transport_bw( XINT_Payload& payload, tlm_phase& phase, sc_core::sc_time& t);

    virtual void read_write( tlm::tlm_generic_payload& trans, sc_time& delay );

	//=====================================================================
	//=                  Client/Service setup methods                     =
	//=====================================================================

	virtual bool Setup();
	virtual void OnDisconnect();
	virtual void Reset();


	//=====================================================================
	//=             memory interface methods                              =
	//=====================================================================

	virtual bool ReadMemory(service_address_t addr, void *buffer, uint32_t size);
	virtual bool WriteMemory(service_address_t addr, const void *buffer, uint32_t size);

	//=====================================================================
	//=             XINT Registers Interface interface methods               =
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
    void read(uint8_t offset, uint8_t &value);
    void write(uint8_t offset, uint8_t val);


protected:

private:
	tlm_quantumkeeper quantumkeeper;
	PayloadFabric<XINT_Payload> xint_payload_fabric;

	clock_t	oscillator_clock_int;	// The time unit is PS
	Parameter<clock_t>	param_oscillator_clock_int;
	sc_time		oscillator_clock;

	sc_time		pll_clock;
	sc_time		bus_clock;

	sc_event rti_enable_event;

	address_t	baseAddress;
	Parameter<address_t>   param_baseAddress;

	uint8_t interrupt_offset_rti;
	Parameter<uint8_t> param_interrupt_offset_rti;

	uint8_t interrupt_offset_pll_lock;
	Parameter<uint8_t> param_interrupt_offset_pll_lock;

	uint8_t interrupt_offset_self_clock_mode;
	Parameter<uint8_t> param_interrupt_offset_self_clock_mode;

	bool	debug_enabled;
	Parameter<bool>	param_debug_enabled;

	// Registers map
	map<string, Register *> registers_registry;

	// RTI Frequency Divide Rate
	double rti_fdr;

	void compute_clock();
	void initialize_rti_counter();

	void select_cop_timeout();
	void cop_reset();
	void restart_cop_timeout();

	// =============================================
	// =            Registers                      =
	// =============================================

	uint8_t synr_register, refdv_register, ctflg_register, crgflg_register, crgint_register,
			clksel_register, pllctl_register, rtictl_register, copctl_register, forbyp_register,
			ctctl_register, armcop_register;

}; /* end class CRG */

} // end of namespace hcs12x
} // end of namespace processor
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif /*__UNISIM_COMPONENT_TLM2_PROCESSOR_CRG_HH__*/

