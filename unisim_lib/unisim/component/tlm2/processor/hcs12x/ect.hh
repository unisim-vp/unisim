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
 * This module implement the S12ECT16B8C_V2 controler
 */

#ifndef __UNISIM_COMPONENT_TLM2_PROCESSOR_ECT_HH__
#define __UNISIM_COMPONENT_TLM2_PROCESSOR_ECT_HH__

#include <inttypes.h>
#include <iostream>
#include <cmath>

#include <systemc.h>

#include <tlm.h>
#include <tlm_utils/tlm_quantumkeeper.h>
#include <tlm_utils/peq_with_get.h>
#include "tlm_utils/simple_target_socket.h"

#include "unisim/service/interfaces/trap_reporting.hh"
#include <unisim/kernel/service/service.hh>
#include "unisim/service/interfaces/memory.hh"
#include "unisim/kernel/tlm2/tlm.hh"

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
	public Client<Memory<service_address_t> >

{
public:

	//=========================================================
	//=                REGISTERS OFFSETS                      =
	//=========================================================

	static const address_t	TIOS	= 0x0000;
	static const address_t	TSCR1	= 0x0006;
	static const address_t	TIE		= 0x000C;
	static const address_t	TSCR2	= 0x000D;
	static const address_t	TFLG1	= 0x000E;

	//=========================================================
	//=                MODULE INTERFACE                       =
	//=========================================================

	ServiceImport<TrapReporting > trap_reporting_import;

	tlm_initiator_socket<CONFIG::EXTERNAL2UNISIM_BUS_WIDTH, XINT_REQ_ProtocolTypes> interrupt_request;

	tlm_utils::simple_target_socket<ECT> slave_socket;

	ServiceExport<Memory<service_address_t> > memory_export;
	ServiceImport<Memory<service_address_t> > memory_import;

	ECT(const sc_module_name& name, Object *parent = 0);
	virtual ~ECT();

	void Run();
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
	//=             registers setters and getters                         =
	//=====================================================================
    void read(uint8_t offset, uint8_t &value);
    void write(uint8_t offset, uint8_t val);


protected:

private:
	tlm_quantumkeeper quantumkeeper;
	PayloadFabric<XINT_Payload> xint_payload_fabric;


	clock_t	bus_cycle_time_int;	// The time unit is PS
	Parameter<clock_t>	param_bus_cycle_time_int;
	sc_time		bus_cycle_time;

	address_t	baseAddress;
	Parameter<address_t>   param_baseAddress;

	uint8_t interrupt_offset_channel0;
	Parameter<uint8_t> param_interrupt_offset_channel0;
	uint8_t interrupt_offset_overflow;
	Parameter<uint8_t> param_interrupt_offset_overflow;

	//==============================
	//=            REGISTER SET    =
	//==============================

	uint8_t tios_register;
	uint8_t tscr1_register;
	uint8_t tie_register;
	uint8_t tscr2_register;
	uint8_t tflg1_register;

}; /* end class ECT */

} // end of namespace hcs12x
} // end of namespace processor
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif /*__UNISIM_COMPONENT_TLM2_PROCESSOR_ECT_HH__*/
