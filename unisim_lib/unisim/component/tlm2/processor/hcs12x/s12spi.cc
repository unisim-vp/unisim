/*
 * s12spi.cc
 *
 *  Created on: 8 août 2012
 *      Author: rnouacer
 */

#include <unisim/component/tlm2/processor/hcs12x/s12spi.hh>
#include <unisim/component/tlm2/processor/hcs12x/xint.hh>

namespace unisim {
namespace component {
namespace tlm2 {
namespace processor {
namespace hcs12x {


S12SPI::S12SPI(const sc_module_name& name, Object *parent) :
	Object(name, parent)
	, sc_module(name)

	, unisim::kernel::service::Client<TrapReporting>(name, parent)
	, unisim::kernel::service::Service<Memory<physical_address_t> >(name, parent)
	, unisim::kernel::service::Service<Registers>(name, parent)
	, unisim::kernel::service::Client<Memory<physical_address_t> >(name, parent)

	, trap_reporting_import("trap_reporting_import", this)

	, bus_clock_socket("bus_clock_socket")
	, slave_socket("slave_socket")

	, memory_export("memory_export", this)
	, memory_import("memory_import", this)
	, registers_export("registers_export", this)

	, spicr1_register(0x00)
	, spicr2_register(0x00)
	, spibr_register(0x00)
	, spisr_register(0x00)
	, spidr_register(0x00)

	, bus_cycle_time_int(250000)
	, param_bus_cycle_time_int("bus-cycle-time", this, bus_cycle_time_int)

	, baseAddress(0x00D8)
	, param_baseAddress("base-address", this, baseAddress)

	, interrupt_offset(0xD8)
	, param_interrupt_offset("interrupt-offset", this, interrupt_offset)

	, debug_enabled(false)
	, param_debug_enabled("debug-enabled", this, debug_enabled)

{


	interrupt_request(*this);

	slave_socket.register_b_transport(this, &S12SPI::read_write);
	bus_clock_socket.register_b_transport(this, &S12SPI::updateBusClock);

	SC_HAS_PROCESS(S12SPI);

}


S12SPI::~S12SPI() {

	// Release registers_registry
	map<string, unisim::util::debug::Register *>::iterator reg_iter;

	for(reg_iter = registers_registry.begin(); reg_iter != registers_registry.end(); reg_iter++)
	{
		if(reg_iter->second)
			delete reg_iter->second;
	}

	registers_registry.clear();

	unsigned int i;
	unsigned int n = extended_registers_registry.size();
	for (i=0; i<n; i++) {
		delete extended_registers_registry[i];
	}

}


void S12SPI::read_write( tlm::tlm_generic_payload& trans, sc_time& delay )
{

	tlm::tlm_command cmd = trans.get_command();
	sc_dt::uint64 address = trans.get_address();
	uint8_t* data_ptr = (uint8_t *)trans.get_data_ptr();
	unsigned int data_length = trans.get_data_length();

	if ((address >= baseAddress) && (address < (baseAddress + 8))) {

		if (cmd == tlm::TLM_READ_COMMAND) {
			memset(data_ptr, 0, data_length);

			std::cerr << "S12SPI::Warning: READ access to 0x" << std::hex << (address - baseAddress) << std::endl;

			read(address - baseAddress, data_ptr, data_length);
		} else if (cmd == tlm::TLM_WRITE_COMMAND) {

			std::cerr << "S12SPI::Warning: WRITE access to 0x" << std::hex << (address - baseAddress) << std::endl;

			write(address - baseAddress, data_ptr, data_length);
		}

		trans.set_response_status( tlm::TLM_OK_RESPONSE );

	} else {
		trans.set_response_status( tlm::TLM_INCOMPLETE_RESPONSE );
	}

}


bool S12SPI::read(unsigned int offset, const void *buffer, unsigned int data_length) {

	switch (offset) {
	case SPICR1: *((uint8_t *) buffer) = spicr1_register; break;	// 1 byte
	case SPICR2: *((uint8_t *) buffer) = spicr2_register; break;	// 1 byte
	case SPIBR: *((uint8_t *) buffer) = spibr_register; break;	// 1 byte
	case SPISR: *((uint8_t *) buffer) = spisr_register; break; // 1 byte
	case RESERVED1: *((uint8_t *) buffer) = 0x00; break; // 1 byte
	case SPIDR: *((uint8_t *) buffer) = spidr_register; break; // 1 bytes
	case RESERVED2: *((uint8_t *) buffer) = 0x00; break; // 1 bytes
	case RESERVED3: *((uint8_t *) buffer) = 0x00; break; // 1 bytes
	}

	return (true);
}

//=====================================================================
//=             registers setters and getters                         =
//=====================================================================


bool S12SPI::write(unsigned int offset, const void *buffer, unsigned int data_length) {

	switch (offset) {
	case SPICR1: spicr1_register = *((uint8_t *) buffer); break;	// 1 byte
	case SPICR2: spicr2_register = *((uint8_t *) buffer); break;	// 1 byte
	case SPIBR:  spibr_register = *((uint8_t *) buffer); break;	// 1 byte
	case SPISR:  spisr_register = *((uint8_t *) buffer); break; // 1 byte
	case RESERVED1:  break; // 1 byte
	case SPIDR:  spidr_register = *((uint8_t *) buffer); break; // 1 bytes
	case RESERVED2:  break; // 1 bytes
	case RESERVED3:  break; // 1 bytes

	}

	return (true);
}


void S12SPI::assertInterrupt(uint8_t interrupt_offset) {

	tlm_phase phase = BEGIN_REQ;
	XINT_Payload *payload = xint_payload_fabric.allocate();

	payload->setInterruptOffset(interrupt_offset);

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

void S12SPI::invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range)
{
	// Leave this empty as it is designed for memory mapped buses
}

// Master methods

tlm_sync_enum S12SPI::nb_transport_bw( XINT_Payload& payload, tlm_phase& phase, sc_core::sc_time& t)
{
	if(phase == BEGIN_RESP)
	{
		payload.release();
		return (TLM_COMPLETED);
	}
	return (TLM_ACCEPTED);
}


void S12SPI::ComputeInternalTime() {

	bus_cycle_time = sc_time((double)bus_cycle_time_int, SC_PS);

}


void S12SPI::updateBusClock(tlm::tlm_generic_payload& trans, sc_time& delay) {

	sc_dt::uint64*   external_bus_clock = (sc_dt::uint64*) trans.get_data_ptr();
    trans.set_response_status( tlm::TLM_OK_RESPONSE );

	bus_cycle_time_int = *external_bus_clock;

	ComputeInternalTime();
}


//=====================================================================
//=                  Client/Service setup methods                     =
//=====================================================================


bool S12SPI::BeginSetup() {

	char buf[80];

	sprintf(buf, "%s.SPICR1",sc_object::name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &spicr1_register);

	unisim::kernel::service::Register<uint8_t> *spicr1_var = new unisim::kernel::service::Register<uint8_t>("SPICR1", this, spicr1_register, "SPI Control Register 1 (SPICR1)");
	extended_registers_registry.push_back(spicr1_var);
	spicr1_var->setCallBack(this, SPICR1, &CallBackObject::write, NULL);

	sprintf(buf, "%s.SPICR2",sc_object::name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &spicr2_register);

	unisim::kernel::service::Register<uint8_t> *spicr2_var = new unisim::kernel::service::Register<uint8_t>("SPICR2", this, spicr2_register, "SPI Control Register 2 (SPICR2)");
	extended_registers_registry.push_back(spicr2_var);
	spicr2_var->setCallBack(this, SPICR2, &CallBackObject::write, NULL);

	sprintf(buf, "%s.SPIBR",sc_object::name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &spibr_register);

	unisim::kernel::service::Register<uint8_t> *spibr_var = new unisim::kernel::service::Register<uint8_t>("SPIBR", this, spibr_register, "SPI Baud Rate Register (SPIBR)");
	extended_registers_registry.push_back(spibr_var);
	spibr_var->setCallBack(this, SPIBR, &CallBackObject::write, NULL);

	sprintf(buf, "%s.SPISR",sc_object::name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &spisr_register);

	unisim::kernel::service::Register<uint8_t> *spisr_var = new unisim::kernel::service::Register<uint8_t>("SPISR", this, spisr_register, "SPI Status register (SPISR)");
	extended_registers_registry.push_back(spisr_var);
	spisr_var->setCallBack(this, SPISR, &CallBackObject::write, NULL);

	sprintf(buf, "%s.SPIDR",sc_object::name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &spidr_register);

	unisim::kernel::service::Register<uint8_t> *spidr_var = new unisim::kernel::service::Register<uint8_t>("SPIDR", this, spidr_register, "SPI Data register (SPIDR)");
	extended_registers_registry.push_back(spidr_var);
	spidr_var->setCallBack(this, SPIDR, &CallBackObject::write, NULL);

	Reset();

	ComputeInternalTime();

	return (true);
}


bool S12SPI::Setup(ServiceExportBase *srv_export) {

	return (true);
}


bool S12SPI::EndSetup() {
	return (true);
}


Register* S12SPI::GetRegister(const char *name)
{
	if(registers_registry.find(string(name)) != registers_registry.end())
		return (registers_registry[string(name)]);
	else
		return (NULL);

}


void S12SPI::OnDisconnect() {
}


void S12SPI::Reset() {

	spicr1_register = 0x00;
	spicr2_register = 0x00;
	spibr_register = 0x00;
	spisr_register = 0x00;
	spidr_register = 0x00;
}


//=====================================================================
//=             memory interface methods                              =
//=====================================================================


bool S12SPI::ReadMemory(physical_address_t addr, void *buffer, uint32_t size) {

	if ((addr >= baseAddress) && (addr < (baseAddress+8))) {

		physical_address_t offset = addr-baseAddress;

		switch (offset) {
		case SPICR1: *((uint8_t *) buffer) = spicr1_register; break;	// 1 byte
		case SPICR2: *((uint8_t *) buffer) = spicr2_register; break;	// 1 byte
		case SPIBR: *((uint8_t *) buffer) = spibr_register; break;	// 1 byte
		case SPISR: *((uint8_t *) buffer) = spisr_register; break; // 1 byte
		case RESERVED1: *((uint8_t *) buffer) = 0x00; break; // 1 byte
		case SPIDR: *((uint8_t *) buffer) = spidr_register; break; // 1 bytes
		case RESERVED2: *((uint8_t *) buffer) = 0x00; break; // 1 bytes
		case RESERVED3: *((uint8_t *) buffer) = 0x00; break; // 1 bytes

		}

		return (true);

	}

	return (false);
}


bool S12SPI::WriteMemory(physical_address_t addr, const void *buffer, uint32_t size) {

	if ((addr >= baseAddress) && (addr < (baseAddress+8))) {

		if (size == 0) {
			return (true);
		}

		physical_address_t offset = addr-baseAddress;

		switch (offset) {
		case SPICR1: spicr1_register = *((uint8_t *) buffer); break;	// 1 byte
		case SPICR2: spicr2_register = *((uint8_t *) buffer); break;	// 1 byte
		case SPIBR:  spibr_register = *((uint8_t *) buffer); break;	// 1 byte
		case SPISR:  spisr_register = *((uint8_t *) buffer); break; // 1 byte
		case RESERVED1:  break; // 1 byte
		case SPIDR:  spidr_register = *((uint8_t *) buffer); break; // 1 bytes
		case RESERVED2:  break; // 1 bytes
		case RESERVED3:  break; // 1 bytes
		}

		return (true);
	}

	return (false);

}


} // end of namespace hcs12x
} // end of namespace processor
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim






