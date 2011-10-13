/*
 * s12xeetx.hh
 *
 *  Created on: 10 oct. 2011
 *      Author: rnouacer
 */

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_HCS12X_S12XEETX_HH_
#define __UNISIM_COMPONENT_CXX_PROCESSOR_HCS12X_S12XEETX_HH_

#include <systemc.h>

#include <tlm.h>
#include <tlm_utils/tlm_quantumkeeper.h>
#include <tlm_utils/peq_with_get.h>
#include "tlm_utils/simple_target_socket.h"

#include "unisim/kernel/service/service.hh"
#include "unisim/kernel/logger/logger.hh"
#include "unisim/kernel/tlm2/tlm.hh"

#include "unisim/service/interfaces/registers.hh"

#include "unisim/util/debug/register.hh"
#include "unisim/util/debug/simple_register.hh"

#include <unisim/component/cxx/processor/hcs12x/config.hh>
#include <unisim/component/cxx/processor/hcs12x/types.hh>

#include "unisim/component/tlm2/memory/ram/memory.hh"
#include <unisim/component/tlm2/processor/hcs12x/tlm_types.hh>

#include <inttypes.h>

namespace unisim {
namespace component {
namespace tlm2 {
namespace processor {
namespace hcs12x {

using namespace std;
using namespace tlm;
using namespace tlm_utils;

using unisim::kernel::tlm2::PayloadFabric;
using unisim::kernel::service::Object;
using unisim::kernel::service::Client;
using unisim::kernel::service::Parameter;
using unisim::kernel::logger::Logger;

using unisim::service::interfaces::Registers;

using unisim::util::debug::Register;
using unisim::util::debug::SimpleRegister;

using unisim::component::tlm2::memory::ram::DEFAULT_BURST_LENGTH;
using unisim::component::tlm2::memory::ram::DEFAULT_BUSWIDTH;
using unisim::component::tlm2::memory::ram::DEFAULT_DEBUG;
using unisim::component::tlm2::memory::ram::DEFAULT_PAGE_SIZE;
using unisim::component::tlm2::memory::ram::DEFAULT_ADDRESS;

using unisim::component::cxx::processor::hcs12x::service_address_t;
using unisim::component::cxx::processor::hcs12x::physical_address_t;
using unisim::component::cxx::processor::hcs12x::CONFIG;

template <unsigned int BUSWIDTH = DEFAULT_BUSWIDTH, class ADDRESS = DEFAULT_ADDRESS, unsigned int BURST_LENGTH = DEFAULT_BURST_LENGTH, uint32_t PAGE_SIZE = DEFAULT_PAGE_SIZE, bool DEBUG = DEFAULT_DEBUG>
class S12XEETX :
	public unisim::component::tlm2::memory::ram::Memory<BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>
	, virtual public tlm_bw_transport_if<XINT_REQ_ProtocolTypes>

{
public:
	typedef unisim::component::tlm2::memory::ram::Memory<BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG> inherited;


	//=========================================================
	//=                REGISTERS OFFSETS                      =
	//=========================================================

	enum REGS_OFFSETS {ECLKDIV, RESERVED1, RESERVED2, ECNFG, EPROT, ESTAT, ECMD, RESERVED3, EADDRHI, EADDRLO, EDATAHI, EDATALO};

	tlm_initiator_socket<CONFIG::EXTERNAL2UNISIM_BUS_WIDTH, XINT_REQ_ProtocolTypes> interrupt_request;

	// interface with bus
	tlm_utils::simple_target_socket<S12XEETX> slave_socket;
	tlm_utils::simple_target_socket<S12XEETX> bus_clock_socket;


	/**
	 * Constructor.
	 *
	 * @param name the name of the module
	 * @param parent the parent service
	 */
	S12XEETX(const sc_module_name& name, Object *parent = 0);
	/**
	 * Destructor
	 */
	virtual ~S12XEETX();

//	void Process();

	void updateBusClock(tlm::tlm_generic_payload& trans, sc_time& delay);
	void setEEPROMClock();
	void assertInterrupt();

	/* Service methods */
	/** BeginSetup
	 * Initializes the service interface. */
	virtual bool BeginSetup();

	virtual void Reset();

	/**
	 * TLM2 Slave methods
	 */
	virtual void invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range);
	virtual bool get_direct_mem_ptr(tlm::tlm_generic_payload& payload, tlm::tlm_dmi& dmi_data);
	virtual unsigned int transport_dbg(tlm::tlm_generic_payload& payload);
	virtual tlm::tlm_sync_enum nb_transport_fw(tlm::tlm_generic_payload& payload, tlm::tlm_phase& phase, sc_core::sc_time& t);
	virtual void b_transport(tlm::tlm_generic_payload& payload, sc_core::sc_time& t);
	virtual tlm_sync_enum nb_transport_bw( XINT_Payload& payload, tlm_phase& phase, sc_core::sc_time& t);

	void read_write( tlm::tlm_generic_payload& trans, sc_time& delay );

	//=====================================================================
	//=             memory interface methods                              =
	//=====================================================================

	virtual bool ReadMemory(service_address_t addr, void *buffer, uint32_t size);
	virtual bool WriteMemory(service_address_t addr, const void *buffer, uint32_t size);

	//=====================================================================
	//=             EEPROM Registers Interface interface methods               =
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
	bool read(uint8_t offset, void *buffer, unsigned int data_length);
	bool write(uint8_t offset, const void *buffer, unsigned int data_length);


protected:

private:

	void ComputeInternalTime();
	tlm_quantumkeeper quantumkeeper;

	PayloadFabric<XINT_Payload> xint_payload_fabric;

	double	bus_cycle_time_int;
	Parameter<double>	param_bus_cycle_time_int;
	sc_time		bus_cycle_time;

	address_t	baseAddress;
	Parameter<address_t>   param_baseAddress;

	physical_address_t	global_start_address;
	Parameter<physical_address_t>   param_global_start_address;
	physical_address_t	global_end_address;
	Parameter<physical_address_t>   param_global_end_address;

	uint64_t protected_area_start_address;
	Parameter<uint64_t> param_protected_area_start_address;

	bool protection_enabled;
	Parameter<bool> param_protection_enabled;

	uint8_t cmd_interruptOffset;
	Parameter<uint8_t> param_cmd_interruptOffset;

	// Registers map
	map<string, Register *> registers_registry;

	std::vector<unisim::kernel::service::VariableBase*> extended_registers_registry;

	uint8_t eclkdiv_reg, reserved1_reg, reserved2_reg, ecnfg_reg, eprot_reg, estat_reg, ecmd_reg, reserved3_reg;
	uint16_t eaddr_reg, edata_reg;

};

} // end of hcs12x
} // end of processor
} // end of tlm2
} // end of component
} // end of unisim

#endif /* __UNISIM_COMPONENT_CXX_PROCESSOR_HCS12X_S12XEETX_HH_ */
