
/*
 *  Copyright (c) 2008, 2012
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
 * s12spi.hh
 *
 *  Created on: 8 août 2012
 *      Author: rnouacer
 *
 *  Description: This module implement the S12SPI V4
 *
 */


#ifndef __UNISIM_COMPONENT_TLM2_PROCESSOR_S12SPI_HH_
#define __UNISIM_COMPONENT_TLM2_PROCESSOR_S12SPI_HH_

#include <inttypes.h>
#include <iostream>
#include <cmath>
#include <map>

#include <systemc>

#include <tlm.h>
#include <tlm_utils/tlm_quantumkeeper.h>
#include <tlm_utils/peq_with_get.h>
#include "tlm_utils/simple_target_socket.h"
#include "tlm_utils/multi_passthrough_initiator_socket.h"
//#include "tlm_utils/multi_passthrough_target_socket.h"

#include <unisim/kernel/service/service.hh>
#include "unisim/kernel/tlm2/tlm.hh"

#include "unisim/service/interfaces/memory.hh"
#include "unisim/service/interfaces/registers.hh"
#include "unisim/service/interfaces/trap_reporting.hh"

#include "unisim/util/debug/register.hh"
#include "unisim/util/debug/simple_register.hh"
#include "unisim/util/endian/endian.hh"

#include <unisim/component/cxx/processor/hcs12x/config.hh>
#include <unisim/component/cxx/processor/hcs12x/types.hh>

#include <unisim/component/tlm2/processor/hcs12x/tlm_types.hh>

namespace unisim {
namespace component {
namespace tlm2 {
namespace processor {
namespace hcs12x {

using namespace std;
using namespace sc_core;
using namespace sc_dt;
using namespace tlm;
using namespace tlm_utils;

using unisim::kernel::service::Object;
using unisim::kernel::service::Client;
using unisim::kernel::service::Service;
using unisim::kernel::service::ServiceExport;
using unisim::kernel::service::ServiceImport;
using unisim::kernel::service::ServiceExportBase;
using unisim::kernel::service::Parameter;
using unisim::kernel::service::CallBackObject;
using unisim::kernel::service::VariableBase;

using unisim::service::interfaces::Memory;
using unisim::service::interfaces::Registers;
using unisim::service::interfaces::TrapReporting;

using unisim::util::debug::Register;
using unisim::util::debug::SimpleRegister;
using unisim::util::endian::BigEndian2Host;
using unisim::util::endian::Host2BigEndian;


using unisim::component::cxx::processor::hcs12x::ADDRESS;
using unisim::component::cxx::processor::hcs12x::address_t;
using unisim::component::cxx::processor::hcs12x::physical_address_t;
using unisim::component::cxx::processor::hcs12x::physical_address_t;
using unisim::component::cxx::processor::hcs12x::CONFIG;

using unisim::kernel::service::Object;
using unisim::kernel::tlm2::ManagedPayload;
using unisim::kernel::tlm2::PayloadFabric;

class S12SPI :
	public sc_module
	, public CallBackObject
	, virtual public tlm_bw_transport_if<XINT_REQ_ProtocolTypes>
	, public Client<TrapReporting >
	, public Service<Memory<physical_address_t> >
	, public Service<Registers>
	, public Client<Memory<physical_address_t> >

{
public:
	//=========================================================
	//=                REGISTERS OFFSETS                      =
	//=========================================================

	static const uint8_t SPICR1	= 0x00;	// 1 byte
	static const uint8_t SPICR2	= 0x01;	// 1 byte
	static const uint8_t SPIBR		= 0x02;	// 1 byte
	static const uint8_t SPISR		= 0x03; // 1 byte
	static const uint8_t RESERVED1	= 0x04; // 1 byte
	static const uint8_t SPIDR		= 0x05; // 1 bytes
	static const uint8_t RESERVED2	= 0x06; // 1 bytes
	static const uint8_t RESERVED3	= 0x07; // 1 bytes

	ServiceImport<TrapReporting > trap_reporting_import;

	tlm_initiator_socket<CONFIG::EXTERNAL2UNISIM_BUS_WIDTH, XINT_REQ_ProtocolTypes> interrupt_request;

	tlm_utils::simple_target_socket<S12SPI> slave_socket;
	tlm_utils::simple_target_socket<S12SPI> bus_clock_socket;

	ServiceExport<Memory<physical_address_t> > memory_export;
	ServiceImport<Memory<physical_address_t> > memory_import;
	ServiceExport<Registers> registers_export;

	S12SPI(const sc_module_name& name, Object *parent = 0);
	virtual ~S12SPI();

	void assertInterrupt(uint8_t interrupt_offset);
	void ComputeInternalTime();

    //================================================================
    //=                    tlm2 Interface                            =
    //================================================================
	virtual void invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range);
	virtual tlm_sync_enum nb_transport_bw( XINT_Payload& payload, tlm_phase& phase, sc_core::sc_time& t);

    virtual void read_write( tlm::tlm_generic_payload& trans, sc_time& delay );

    void updateBusClock(tlm::tlm_generic_payload& trans, sc_time& delay);

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

	virtual bool ReadMemory(physical_address_t addr, void *buffer, uint32_t size);
	virtual bool WriteMemory(physical_address_t addr, const void *buffer, uint32_t size);

	//=====================================================================
	//=             S12SPI Registers Interface interface methods               =
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
	virtual bool read(unsigned int offset, const void *buffer, unsigned int data_length);
	virtual bool write(unsigned int offset, const void *buffer, unsigned int data_length);


protected:

private:
	tlm_quantumkeeper quantumkeeper;
	PayloadFabric<XINT_Payload> xint_payload_fabric;

	PayloadFabric<tlm::tlm_generic_payload> payloadFabric;

	double	bus_cycle_time_int;
	Parameter<double>	param_bus_cycle_time_int;
	sc_time		bus_cycle_time;

	// S12SPI baseAddress  SPI0=0x00D8:0x00DF  SPI1=0x00F0:0x00F7 SPI2=0x00F8:0x00FF
	address_t	baseAddress;
	Parameter<address_t>   param_baseAddress;

	uint8_t interrupt_offset;  // vector offset SPI0=0xD8  SPI1=0xBE SPI2=0xBC
	Parameter<uint8_t> param_interrupt_offset;

	bool	debug_enabled;
	Parameter<bool>	param_debug_enabled;

	// Registers map
	map<string, Register *> registers_registry;

	std::vector<unisim::kernel::service::VariableBase*> extended_registers_registry;

	// =============================================
	// =            Registers                      =
	// =============================================

	uint8_t spicr1_register;	// 1 byte
	uint8_t spicr2_register;	// 1 byte
	uint8_t spibr_register; // 1 byte
	uint8_t spisr_register; // 1 byte
	uint8_t spidr_register; // 1 bytes


}; /* end class S12SPI */

} // end of namespace hcs12x
} // end of namespace processor
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim



#endif /* __UNISIM_COMPONENT_TLM2_PROCESSOR_S12SPI_HH_ */
