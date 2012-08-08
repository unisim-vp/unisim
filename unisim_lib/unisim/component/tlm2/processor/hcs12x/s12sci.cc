/*
 * s12sci.cc
 *
 *  Created on: 8 août 2012
 *      Author: rnouacer
 */

#include <unisim/component/tlm2/processor/hcs12x/s12sci.hh>
#include <unisim/component/tlm2/processor/hcs12x/xint.hh>

namespace unisim {
namespace component {
namespace tlm2 {
namespace processor {
namespace hcs12x {


S12SCI::S12SCI(const sc_module_name& name, Object *parent) :
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

	, scibyte00_register(&scibdh_register)
	, scibyte01_register(&scibdl_register)
	, scibyte02_register(&scicr1_register)

	, scibdh_register(0x00)
	, scibdl_register(0x00)
	, scicr1_register(0x00)
	, sciasr1_register(0x00)
	, sciacr1_register(0x00)
	, sciacr2_register(0x00)
	, scicr2_register(0x00)
	, scisr1_register(0x00)
	, scisr2_regsiter(0x00)
	, scidrh_register(0x00)
	, scidrl_register(0x00)

	, bus_cycle_time_int(250000)
	, param_bus_cycle_time_int("bus-cycle-time", this, bus_cycle_time_int)

	, baseAddress(0x00C8)
	, param_baseAddress("base-address", this, baseAddress)

	, interrupt_offset(0xD6)
	, param_interrupt_offset("interrupt-offset", this, interrupt_offset)

	, debug_enabled(false)
	, param_debug_enabled("debug-enabled", this, debug_enabled)

{


	interrupt_request(*this);

	slave_socket.register_b_transport(this, &S12SCI::read_write);
	bus_clock_socket.register_b_transport(this, &S12SCI::updateBusClock);

	SC_HAS_PROCESS(S12SCI);

}


S12SCI::~S12SCI() {

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


void S12SCI::read_write( tlm::tlm_generic_payload& trans, sc_time& delay )
{
	tlm::tlm_command cmd = trans.get_command();
	sc_dt::uint64 address = trans.get_address();
	uint8_t* data_ptr = (uint8_t *)trans.get_data_ptr();
	unsigned int data_length = trans.get_data_length();

	assert(address >= baseAddress);

	if (cmd == tlm::TLM_READ_COMMAND) {
		memset(data_ptr, 0, data_length);
		read(address - baseAddress, data_ptr, data_length);
	} else if (cmd == tlm::TLM_WRITE_COMMAND) {
		write(address - baseAddress, data_ptr, data_length);
	}

	trans.set_response_status( tlm::TLM_OK_RESPONSE );
}


bool S12SCI::read(unsigned int offset, const void *buffer, unsigned int data_length) {

	switch (offset) {
	case 0x00: *((uint8_t *) buffer) = *scibyte00_register; break;	// 1 byte
	case 0x01: *((uint8_t *) buffer) = *scibyte01_register; break;	// 1 byte
	case 0x02: *((uint8_t *) buffer) = *scibyte02_register; break; // 1 byte

	case SCICR2: *((uint8_t *) buffer) = scicr2_register; break; // 1 byte
	case SCISR1: *((uint8_t *) buffer) = scisr1_register; break; // 1 byte
	case SCISR2: *((uint8_t *) buffer) = scisr2_regsiter; break; // 1 bytes
	case SCIDRH: *((uint8_t *) buffer) = scidrh_register; break; // 1 bytes
	case SCIDRL: *((uint8_t *) buffer) = scidrl_register; break; // 1 bytes
	}

	return (true);
}

//=====================================================================
//=             registers setters and getters                         =
//=====================================================================


bool S12SCI::write(unsigned int offset, const void *buffer, unsigned int data_length) {

	switch (offset) {
	case 0x00: *scibyte00_register = *((uint8_t *) buffer); break;	// 1 byte
	case 0x01: *scibyte01_register = *((uint8_t *) buffer); break;	// 1 byte
	case 0x02: *scibyte02_register = *((uint8_t *) buffer); break; // 1 byte

	case SCICR2: scicr2_register = *((uint8_t *) buffer); break; // 1 byte
	case SCISR1: scisr1_register = *((uint8_t *) buffer); break; // 1 byte
	case SCISR2: scisr2_regsiter = *((uint8_t *) buffer); break; // 1 bytes
	case SCIDRH: scidrh_register = *((uint8_t *) buffer); break; // 1 bytes
	case SCIDRL: scidrl_register = *((uint8_t *) buffer); break; // 1 bytes
	}

	return (true);
}


void S12SCI::assertInterrupt(uint8_t interrupt_offset) {

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

void S12SCI::invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range)
{
	// Leave this empty as it is designed for memory mapped buses
}

// Master methods

tlm_sync_enum S12SCI::nb_transport_bw( XINT_Payload& payload, tlm_phase& phase, sc_core::sc_time& t)
{
	if(phase == BEGIN_RESP)
	{
		payload.release();
		return (TLM_COMPLETED);
	}
	return (TLM_ACCEPTED);
}


void S12SCI::ComputeInternalTime() {

	bus_cycle_time = sc_time((double)bus_cycle_time_int, SC_PS);

}


void S12SCI::updateBusClock(tlm::tlm_generic_payload& trans, sc_time& delay) {

	sc_dt::uint64*   external_bus_clock = (sc_dt::uint64*) trans.get_data_ptr();
    trans.set_response_status( tlm::TLM_OK_RESPONSE );

	bus_cycle_time_int = *external_bus_clock;

	ComputeInternalTime();
}


//=====================================================================
//=                  Client/Service setup methods                     =
//=====================================================================


bool S12SCI::BeginSetup() {

	char buf[80];

	sprintf(buf, "%s.SCIBDH",sc_object::name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &scibdh_register);

	unisim::kernel::service::Register<uint8_t> *scibdh_var = new unisim::kernel::service::Register<uint8_t>("SCIBDH", this, scibdh_register, "SCI Baud Rate Registers High byte (SCIBDH)");
	extended_registers_registry.push_back(scibdh_var);
	scibdh_var->setCallBack(this, SCIBDH, &CallBackObject::write, NULL);

	sprintf(buf, "%s.SCIBDL",sc_object::name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &scibdl_register);

	unisim::kernel::service::Register<uint8_t> *scibdl_var = new unisim::kernel::service::Register<uint8_t>("SCIBDL", this, scibdl_register, "SCI Baud Rate Registers Low byte (SCIBDL)");
	extended_registers_registry.push_back(scibdl_var);
	scibdl_var->setCallBack(this, SCIBDL, &CallBackObject::write, NULL);

	sprintf(buf, "%s.SCICR1",sc_object::name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &scicr1_register);

	unisim::kernel::service::Register<uint8_t> *scicr1_var = new unisim::kernel::service::Register<uint8_t>("SCICR1", this, scicr1_register, "SCI Control Register 1 (SCICR1)");
	extended_registers_registry.push_back(scicr1_var);
	scicr1_var->setCallBack(this, SCICR1, &CallBackObject::write, NULL);

	sprintf(buf, "%s.SCIASR1",sc_object::name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &sciasr1_register);

	unisim::kernel::service::Register<uint8_t> *sciasr1_var = new unisim::kernel::service::Register<uint8_t>("SCIASR1", this, sciasr1_register, "SCI Alternative Status Register 1 (SCIASR1)");
	extended_registers_registry.push_back(sciasr1_var);
	sciasr1_var->setCallBack(this, SCIASR1, &CallBackObject::write, NULL);

	sprintf(buf, "%s.SCIACR1",sc_object::name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &sciacr1_register);

	unisim::kernel::service::Register<uint8_t> *sciacr1_var = new unisim::kernel::service::Register<uint8_t>("SCIACR1", this, sciacr1_register, "SCI Alternative Control Register (SCIACR1)");
	extended_registers_registry.push_back(sciacr1_var);
	sciacr1_var->setCallBack(this, SCIACR1, &CallBackObject::write, NULL);

	sprintf(buf, "%s.SCIACR2",sc_object::name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &sciacr2_register);

	unisim::kernel::service::Register<uint8_t> *sciacr2_var = new unisim::kernel::service::Register<uint8_t>("SCIACR2", this, sciacr2_register, "SCI Alternative Control Register 2 (SCIACR2)");
	extended_registers_registry.push_back(sciacr2_var);
	sciacr2_var->setCallBack(this, SCIACR2, &CallBackObject::write, NULL);

	sprintf(buf, "%s.SCICR2",sc_object::name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &scicr2_register);

	unisim::kernel::service::Register<uint8_t> *scicr2_var = new unisim::kernel::service::Register<uint8_t>("SCICR2", this, scicr2_register, "SCI Control Register 2 (SCICR2)");
	extended_registers_registry.push_back(scicr2_var);
	scicr2_var->setCallBack(this, SCICR2, &CallBackObject::write, NULL);

	sprintf(buf, "%s.SCISR1",sc_object::name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &scisr1_register);

	unisim::kernel::service::Register<uint8_t> *scisr1_var = new unisim::kernel::service::Register<uint8_t>("SCISR1", this, scisr1_register, "SCI Status Register 1 (SCISR1)");
	extended_registers_registry.push_back(scisr1_var);
	scisr1_var->setCallBack(this, SCISR1, &CallBackObject::write, NULL);

	sprintf(buf, "%s.SCISR2",sc_object::name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &scisr2_regsiter);

	unisim::kernel::service::Register<uint8_t> *scisr2_var = new unisim::kernel::service::Register<uint8_t>("SCISR2", this, scisr2_regsiter, "SCI Status Register 2 (SCISR2)");
	extended_registers_registry.push_back(scisr2_var);
	scisr2_var->setCallBack(this, SCISR2, &CallBackObject::write, NULL);

	sprintf(buf, "%s.SCIDRH",sc_object::name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &scidrh_register);

	unisim::kernel::service::Register<uint8_t> *scidrh_var = new unisim::kernel::service::Register<uint8_t>("SCIDRH", this, scidrh_register, "SCI Data Registers High byte (SCIDRH)");
	extended_registers_registry.push_back(scidrh_var);
	scidrh_var->setCallBack(this, SCIDRH, &CallBackObject::write, NULL);

	sprintf(buf, "%s.SCIDRL",sc_object::name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &scidrl_register);

	unisim::kernel::service::Register<uint8_t> *scidrl_var = new unisim::kernel::service::Register<uint8_t>("SCIDRL", this, scidrl_register, "SCI Data Registers Low byte (SCIDRL)");
	extended_registers_registry.push_back(scidrl_var);
	scidrl_var->setCallBack(this, SCIDRL, &CallBackObject::write, NULL);

	Reset();

	ComputeInternalTime();

	return (true);
}


bool S12SCI::Setup(ServiceExportBase *srv_export) {

	return (true);
}


bool S12SCI::EndSetup() {
	return (true);
}


Register* S12SCI::GetRegister(const char *name)
{
	if(registers_registry.find(string(name)) != registers_registry.end())
		return (registers_registry[string(name)]);
	else
		return (NULL);

}


void S12SCI::OnDisconnect() {
}


void S12SCI::Reset() {

	scibdh_register = 0x00;
	scibdl_register = 0x00;
	scicr1_register = 0x00;
	sciasr1_register = 0x00;
	sciacr1_register = 0x00;
	sciacr2_register = 0x00;
	scicr2_register = 0x00;
	scisr1_register = 0x00;
	scisr2_regsiter = 0x00;
	scidrh_register = 0x00;
	scidrl_register = 0x00;

}


//=====================================================================
//=             memory interface methods                              =
//=====================================================================


bool S12SCI::ReadMemory(physical_address_t addr, void *buffer, uint32_t size) {

	if ((addr >= baseAddress) && (addr < (baseAddress+8))) {

		physical_address_t offset = addr-baseAddress;

		switch (offset) {
		case 0x00: *((uint8_t *) buffer) = *scibyte00_register; break;	// 1 byte
		case 0x01: *((uint8_t *) buffer) = *scibyte01_register; break;	// 1 byte
		case 0x02: *((uint8_t *) buffer) = *scibyte02_register; break; // 1 byte

		case SCICR2: *((uint8_t *) buffer) = scicr2_register; break; // 1 byte
		case SCISR1: *((uint8_t *) buffer) = scisr1_register; break; // 1 byte
		case SCISR2: *((uint8_t *) buffer) = scisr2_regsiter; break; // 1 bytes
		case SCIDRH: *((uint8_t *) buffer) = scidrh_register; break; // 1 bytes
		case SCIDRL: *((uint8_t *) buffer) = scidrl_register; break; // 1 bytes

		}

		return (true);

	}

	return (false);
}


bool S12SCI::WriteMemory(physical_address_t addr, const void *buffer, uint32_t size) {

	if ((addr >= baseAddress) && (addr < (baseAddress+8))) {

		if (size == 0) {
			return (true);
		}

		physical_address_t offset = addr-baseAddress;

		switch (offset) {
		case 0x00: *scibyte00_register = *((uint8_t *) buffer); break;	// 1 byte
		case 0x01: *scibyte01_register = *((uint8_t *) buffer); break;	// 1 byte
		case 0x02: *scibyte02_register = *((uint8_t *) buffer); break; // 1 byte

		case SCICR2: scicr2_register = *((uint8_t *) buffer); break; // 1 byte
		case SCISR1: scisr1_register = *((uint8_t *) buffer); break; // 1 byte
		case SCISR2: scisr2_regsiter = *((uint8_t *) buffer); break; // 1 bytes
		case SCIDRH: scidrh_register = *((uint8_t *) buffer); break; // 1 bytes
		case SCIDRL: scidrl_register = *((uint8_t *) buffer); break; // 1 bytes
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





