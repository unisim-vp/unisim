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

#include <unisim/component/tlm2/processor/hcs12x/s12pit24b4cv1.hh>
#include <unisim/component/tlm2/processor/hcs12x/xint.hh>

namespace unisim {
namespace component {
namespace tlm2 {
namespace processor {
namespace hcs12x {

S12PIT24B4C::S12PIT24B4C(const sc_module_name& name, Object *parent) :
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

	, pitcflmt_register(0x00)
	, pitflt_register(0x00)
	, pitce_register(0x00)
	, pitmux_register(0x00)
	, pitinte_register(0x00)
	, pittf_register(0x00)
	, pitmtld0_register(0x00)
	, pitmtld1_register(0x0000)

	, bus_cycle_time_int(250000)
	, param_bus_cycle_time_int("bus-cycle-time", this, bus_cycle_time_int)

	, baseAddress(0x0340)
	, param_baseAddress("base-address", this, baseAddress)

	, interrupt_offset_channel0(0x7A)
	, param_interrupt_offset_channel0("interrupt-offset-channel0", this, interrupt_offset_channel0)

	, debug_enabled(false)
	, param_debug_enabled("debug-enabled", this, debug_enabled)

{


	interrupt_request(*this);

	slave_socket.register_b_transport(this, &S12PIT24B4C::read_write);
	bus_clock_socket.register_b_transport(this, &S12PIT24B4C::updateBusClock);

	SC_HAS_PROCESS(S12PIT24B4C);

	char counterName[20];

	for (uint8_t i=0; i<4; i++) {

		sprintf (counterName, "CNT%d", i);

		counter[i] = new CNT16(counterName, this, i, &pitcnt_register[i], &pitld_register[i]);
	}

	Reset();
}

S12PIT24B4C::~S12PIT24B4C() {

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

S12PIT24B4C::CNT16::CNT16(const sc_module_name& name, S12PIT24B4C *_parent, uint8_t _index, uint16_t* _counter_register, uint16_t* _load_register) :
		sc_module(name), parent(_parent), index(_index), counter_register(_counter_register), load_register(_load_register), isEnabled(false)
{
	SC_HAS_PROCESS(CNT16);
	SC_THREAD(process);
}

void S12PIT24B4C::CNT16::process() {

	while (true) {

		while ((!isEnabled) || !parent->isPITEnabled()) {
			wait(start_event);
		}

		*counter_register = *load_register;
		while ((isEnabled) && parent->isPITEnabled()) {

			wait(period);
			if (*counter_register == 0) {
				parent->setTimeoutFlag(index);
				break;
			} else {
				*counter_register = *counter_register - 1;
			}
		}

	}
}

void S12PIT24B4C::read_write( tlm::tlm_generic_payload& trans, sc_time& delay )
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

bool S12PIT24B4C::read(unsigned int offset, const void *buffer, unsigned int data_length) {

	switch (offset) {
	case PITCFLMT:	*((uint8_t *) buffer) = pitcflmt_register & 0xE0; break;	// 1 byte
	case PITFLT: *((uint8_t *) buffer) = 0x00; break;	// PITFLT read 0x00
	case PITCE: *((uint8_t *) buffer) = pitce_register & 0x0F; break;	// 1 byte
	case PITMUX: *((uint8_t *) buffer) = pitmux_register; break;	// 1 byte
	case PITINTE: *((uint8_t *) buffer) = pitinte_register; break;	// 1 byte
	case PITTF: *((uint8_t *) buffer) = pittf_register; break; // 1 byte
	case PITMTLD0: *((uint8_t *) buffer) = pitmtld0_register; break; // 1 byte
	case PITMTLD1: *((uint8_t *) buffer) = pitmtld1_register; break; // 1 byte
	default: {
		if ((offset >= PITLD0) && (offset <= PITCNT3)) {
			uint8_t group_index = (offset - PITLD0) / 2;
			uint8_t group_register = (offset - PITLD0) % 2;
			if (group_register == 0) {
				*((uint16_t *) buffer) = Host2BigEndian(pitld_register[group_index]);
			} else {
				*((uint16_t *) buffer) = Host2BigEndian(pitcnt_register[group_index]);
			}
		}
		else if ((offset >= RESERVED) && (offset < (RESERVED + 16))) {
			memset((void*) buffer, 0, data_length);
		}
		else {
			return (false);
		}
	} break;

	}

	return (true);
}

//=====================================================================
//=             registers setters and getters                         =
//=====================================================================

bool S12PIT24B4C::write(unsigned int offset, const void *buffer, unsigned int data_length) {

	switch (offset) {
	case PITCFLMT:	{
		if (data_length == 1) {
			uint8_t val = *((uint8_t *) buffer) & 0xE3;
			pitcflmt_register = val;	// 1 byte
		}
		else if (data_length == 2) 	{
			/**
			 * Any group of timers and micro timers can be restarted at the same time by using one16-bit write
			 * to the adjacent PITCFLMT and PITFLT registers with the relevant bits set.
			 */
			uint16_t val = BigEndian2Host(*((uint16_t *) buffer)) & 0xE30F;
			pitcflmt_register = (val >> 8);	// high byte to PITCFLMT
			pitflt_register = (val & 0x00FF); // low byte to PITFLT

			uint8_t mask = 0x01;
			for (uint8_t i=0; (i < 4) && isPITEnabled(); i++) {
				if ((pitflt_register & mask) != 0) {
					counter[i]->load();
				}

				mask = mask << 1;
			}
		}

		uint8_t mask = 0x01;
		for (uint8_t i=0; (i < 2) && isPITEnabled(); i++) {
			if ((pitcflmt_register & mask) != 0) {
				updateMicroCouter(i);
			}

			mask = mask << 1;
		}

		if (!isPITEnabled()) {
			for (uint8_t i=0; i < 4; i++) {
				counter[i]->disable();
			}
			pittf_register = 0;
		}
	} break;
	case PITFLT: {
		uint8_t val = *((uint8_t *) buffer) & 0x0F;
		pitflt_register = val; // 1 byte
		uint8_t mask = 0x01;
		for (uint8_t i=0; (i < 4) && isPITEnabled(); i++) {
			if ((pitflt_register & mask) != 0) {
				counter[i]->load();
			}

			mask = mask << 1;
		}
	} break;
	case PITCE: {
		uint8_t val = *((uint8_t *) buffer) & 0x0F;
		pitce_register = val; // 1 byte
		uint8_t mask = 0x01;
		for (uint8_t i=0; i < 4; i++) {
			if ((pitce_register & mask) == 0) {
				pittf_register = pittf_register & ~mask;
				counter[i]->disable();
			} else {
				counter[i]->enable();
			}

			mask = mask << 1;
		}
	} break;
	case PITMUX: {
		uint8_t val = *((uint8_t *) buffer) & 0x0F;
		pitmux_register = val; // 1 byte
		uint8_t mask = 0x01;
		for (uint8_t i=0; i < 4; i++) {
			if ((pitmux_register & mask) != 0) {
				counter[i]->setPeriod(periods[1]);
			} else {
				counter[i]->setPeriod(periods[0]);
			}

			mask = mask << 1;
		}

	} break;
	case PITINTE: {
		uint8_t val = *((uint8_t *) buffer) & 0x0F;
		pitinte_register = val; // 1 byte
	} break;
	case PITTF: {
		uint8_t val = *((uint8_t *) buffer) & 0x0F;
		pittf_register = pittf_register & ~val; // 1 byte
	} break;
	case PITMTLD0: {
		uint8_t val = *((uint8_t *) buffer);
		pitmtld0_register = val; // 1 byte
		updateMicroCouter(0);
	} break;
	case PITMTLD1: {
		uint8_t val = *((uint8_t *) buffer);
		pitmtld1_register = val; // 1 byte
		updateMicroCouter(1);
	} break;
	default: {
		if ((offset >= PITLD0) && (offset <= PITCNT3)) {
			uint8_t group_index = (offset - PITLD0) / 2;
			uint8_t group_register = (offset - PITLD0) % 2;
			uint16_t val = BigEndian2Host(*((uint16_t *) buffer));
			if (group_register == 0) {
				pitld_register[group_index] = val;
			} else {
				// write to PITCNTx registers has no meaning or effect
//				pitcnt_register[group_index] = val;
			}
		}
		else if ((offset >= RESERVED) && (offset < (RESERVED + 16))) {
			// write to reserved area has no effect
		}
		else {
			return (false);
		}
	} break;

	}

	return (true);
}

void S12PIT24B4C::updateMicroCouter(uint8_t index) {

	if (index == 0) {
		periods[index] = bus_cycle_time * (pitmtld0_register + 1);
	} else {
		periods[index] = bus_cycle_time * (pitmtld1_register + 1);
	}

	uint8_t mask = 0x01;
	for (uint8_t i=0; i < 4; i++) {
		if (((pitmux_register & mask) != 0) && (index == 1)) {
			counter[i]->setPeriod(periods[1]);
		} else {
			counter[i]->setPeriod(periods[0]);
		}

		mask = mask << 1;
	}

}

void S12PIT24B4C::setTimeoutFlag(uint8_t index) {
	pittf_register = pittf_register | (1 << index);

	if ((pitinte_register & (1 << index)) != 0) {
		assertInterrupt(interrupt_offset_channel0 + index);
	}
}


void S12PIT24B4C::assertInterrupt(uint8_t interrupt_offset) {

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
void S12PIT24B4C::invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range)
{
	// Leave this empty as it is designed for memory mapped buses
}

// Master methods
tlm_sync_enum S12PIT24B4C::nb_transport_bw( XINT_Payload& payload, tlm_phase& phase, sc_core::sc_time& t)
{
	if(phase == BEGIN_RESP)
	{
		payload.release();
		return (TLM_COMPLETED);
	}
	return (TLM_ACCEPTED);
}

void S12PIT24B4C::ComputeInternalTime() {

	bus_cycle_time = sc_time((double)bus_cycle_time_int, SC_PS);

	updateMicroCouter(0);
	updateMicroCouter(1);
}

void S12PIT24B4C::updateBusClock(tlm::tlm_generic_payload& trans, sc_time& delay) {

	sc_dt::uint64*   external_bus_clock = (sc_dt::uint64*) trans.get_data_ptr();
    trans.set_response_status( tlm::TLM_OK_RESPONSE );

	bus_cycle_time_int = *external_bus_clock;

	ComputeInternalTime();
}


//=====================================================================
//=                  Client/Service setup methods                     =
//=====================================================================

bool S12PIT24B4C::BeginSetup() {


	char buf[80];

	sprintf(buf, "%s.PITCFLMT",sc_object::name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &pitcflmt_register);

	unisim::kernel::service::Register<uint8_t> *pitcflmt_var = new unisim::kernel::service::Register<uint8_t>("PITCFLMT", this, pitcflmt_register, "PIT Control and Force Load Micro Timer Register (PITCFLMT)");
	extended_registers_registry.push_back(pitcflmt_var);
	pitcflmt_var->setCallBack(this, PITCFLMT, &CallBackObject::write, NULL);

	sprintf(buf, "%s.PITFLT",sc_object::name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &pitflt_register);

	unisim::kernel::service::Register<uint8_t> *pitflt_var = new unisim::kernel::service::Register<uint8_t>("PITFLT", this, pitflt_register, "PIT Force Load Timer (PITFLT)");
	extended_registers_registry.push_back(pitflt_var);
	pitflt_var->setCallBack(this, PITFLT, &CallBackObject::write, NULL);

	sprintf(buf, "%s.PITCE",sc_object::name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &pitce_register);

	unisim::kernel::service::Register<uint8_t> *pitce_var = new unisim::kernel::service::Register<uint8_t>("PITCE", this, pitce_register, "PIT Channel Enable register (PITCE)");
	extended_registers_registry.push_back(pitce_var);
	pitce_var->setCallBack(this, PITCE, &CallBackObject::write, NULL);

	sprintf(buf, "%s.PITMUX",sc_object::name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &pitmux_register);

	unisim::kernel::service::Register<uint8_t> *pitmux_var = new unisim::kernel::service::Register<uint8_t>("PITMUX", this, pitmux_register, "PIT Multiplex register (PITMUX)");
	extended_registers_registry.push_back(pitmux_var);
	pitmux_var->setCallBack(this, PITMUX, &CallBackObject::write, NULL);

	sprintf(buf, "%s.PITINTE",sc_object::name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &pitinte_register);

	unisim::kernel::service::Register<uint8_t> *pitinte_var = new unisim::kernel::service::Register<uint8_t>("PITINTE", this, pitinte_register, "PIT Interrupt Enable register (PITINTE)");
	extended_registers_registry.push_back(pitinte_var);
	pitinte_var->setCallBack(this, PITINTE, &CallBackObject::write, NULL);

	sprintf(buf, "%s.PITTF",sc_object::name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &pittf_register);

	unisim::kernel::service::Register<uint8_t> *pittf_var = new unisim::kernel::service::Register<uint8_t>("PITTF", this, pittf_register, "PIT Time-out Flag Register (PITTF)");
	extended_registers_registry.push_back(pittf_var);
	pittf_var->setCallBack(this, PITTF, &CallBackObject::write, NULL);

	sprintf(buf, "%s.PITMTLD0",sc_object::name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &pitmtld0_register);

	unisim::kernel::service::Register<uint8_t> *pitmtld0_var = new unisim::kernel::service::Register<uint8_t>("PITMTLD0", this, pitmtld0_register, "PIT Micro Timer Load Register 0 (PITMTLD0)");
	extended_registers_registry.push_back(pitmtld0_var);
	pitmtld0_var->setCallBack(this, PITMTLD0, &CallBackObject::write, NULL);

	sprintf(buf, "%s.PITMTLD1",sc_object::name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &pitmtld1_register);

	unisim::kernel::service::Register<uint8_t> *pitmtld1_var = new unisim::kernel::service::Register<uint8_t>("PITMTLD1", this, pitmtld1_register, "PIT Micro Timer Load register 1 (PITMTLD1)");
	extended_registers_registry.push_back(pitmtld1_var);
	pitmtld1_var->setCallBack(this, PITMTLD1, &CallBackObject::write, NULL);

	sprintf(buf, "%s.PITLD0",sc_object::name());
	registers_registry[buf] = new SimpleRegister<uint16_t>(buf, &pitld_register[0]);

	unisim::kernel::service::Register<uint16_t> *pitld0_var = new unisim::kernel::service::Register<uint16_t>("PITLD0", this, pitld_register[0], "PIT Load Register 0 (PITLD0)");
	extended_registers_registry.push_back(pitld0_var);
	pitld0_var->setCallBack(this, PITLD0, &CallBackObject::write, NULL);

	sprintf(buf, "%s.PITCNT0",sc_object::name());
	registers_registry[buf] = new SimpleRegister<uint16_t>(buf, &pitcnt_register[0]);

	unisim::kernel::service::Register<uint16_t> *pitcnt0_var = new unisim::kernel::service::Register<uint16_t>("PITCNT0", this, pitcnt_register[0], "PIT Count Register 0 (PITCNT0)");
	extended_registers_registry.push_back(pitcnt0_var);
	pitcnt0_var->setCallBack(this, PITCNT0, &CallBackObject::write, NULL);

	sprintf(buf, "%s.PITLD1",sc_object::name());
	registers_registry[buf] = new SimpleRegister<uint16_t>(buf, &pitld_register[1]);

	unisim::kernel::service::Register<uint16_t> *pitld1_var = new unisim::kernel::service::Register<uint16_t>("PITLD1", this, pitld_register[1], "PIT Load Register 1 (PITLD1)");
	extended_registers_registry.push_back(pitld1_var);
	pitld1_var->setCallBack(this, PITLD1, &CallBackObject::write, NULL);

	sprintf(buf, "%s.PITCNT1",sc_object::name());
	registers_registry[buf] = new SimpleRegister<uint16_t>(buf, &pitcnt_register[1]);

	unisim::kernel::service::Register<uint16_t> *pitcnt1_var = new unisim::kernel::service::Register<uint16_t>("PITCNT1", this, pitcnt_register[1], "PIT count Register 1 (PITCNT1)");
	extended_registers_registry.push_back(pitcnt1_var);
	pitcnt1_var->setCallBack(this, PITCNT1, &CallBackObject::write, NULL);

	sprintf(buf, "%s.PITLD2",sc_object::name());
	registers_registry[buf] = new SimpleRegister<uint16_t>(buf, &pitld_register[2]);

	unisim::kernel::service::Register<uint16_t> *pitld2_var = new unisim::kernel::service::Register<uint16_t>("PITLD2", this, pitld_register[2], "PIT Load register 2 (PITLD2)");
	extended_registers_registry.push_back(pitld2_var);
	pitld2_var->setCallBack(this, PITLD2, &CallBackObject::write, NULL);

	sprintf(buf, "%s.PITCNT2",sc_object::name());
	registers_registry[buf] = new SimpleRegister<uint16_t>(buf, &pitcnt_register[2]);

	unisim::kernel::service::Register<uint16_t> *pitcnt2_var = new unisim::kernel::service::Register<uint16_t>("PITCNT2", this, pitcnt_register[2], "PIT Count register 2 (PITCNT2)");
	extended_registers_registry.push_back(pitcnt2_var);
	pitcnt2_var->setCallBack(this, PITCNT2, &CallBackObject::write, NULL);

	sprintf(buf, "%s.PITLD3",sc_object::name());
	registers_registry[buf] = new SimpleRegister<uint16_t>(buf, &pitld_register[3]);

	unisim::kernel::service::Register<uint16_t> *pitld3_var = new unisim::kernel::service::Register<uint16_t>("PITLD3", this, pitld_register[3], "PIT Load register 3 (PITLD3)");
	extended_registers_registry.push_back(pitld3_var);
	pitld3_var->setCallBack(this, PITLD3, &CallBackObject::write, NULL);

	sprintf(buf, "%s.PITCNT3",sc_object::name());
	registers_registry[buf] = new SimpleRegister<uint16_t>(buf, &pitcnt_register[3]);

	unisim::kernel::service::Register<uint16_t> *pitcnt3_var = new unisim::kernel::service::Register<uint16_t>("PITCNT3", this, pitcnt_register[3], "PIT Count register 3 (PITCNT3)");
	extended_registers_registry.push_back(pitcnt3_var);
	pitcnt3_var->setCallBack(this, PITCNT3, &CallBackObject::write, NULL);

	ComputeInternalTime();

	return (true);
}

bool S12PIT24B4C::Setup(ServiceExportBase *srv_export) {

	return (true);
}

bool S12PIT24B4C::EndSetup() {
	return (true);
}

Register* S12PIT24B4C::GetRegister(const char *name)
{
	if(registers_registry.find(string(name)) != registers_registry.end())
		return (registers_registry[string(name)]);
	else
		return (NULL);

}

void S12PIT24B4C::OnDisconnect() {
}

void S12PIT24B4C::Reset() {

	pitcflmt_register = 0x00;
	pitflt_register = 0x00;
	pitce_register = 0x00;
	pitmux_register = 0x00;
	pitinte_register = 0x00;
	pittf_register = 0x00;
	pitmtld0_register = 0x00;
	pitmtld1_register = 0x0000;
	for (uint8_t i=0; i < 4; i++) {
		pitld_register[i] = 0x0000;
		pitcnt_register[i] = 0x0000;
	}

}


//=====================================================================
//=             memory interface methods                              =
//=====================================================================

bool S12PIT24B4C::ReadMemory(physical_address_t addr, void *buffer, uint32_t size) {

	if ((addr >= baseAddress) && (addr < (baseAddress+40))) {

		physical_address_t offset = addr-baseAddress;

		switch (offset) {
		case PITCFLMT:	*((uint8_t *) buffer) = pitcflmt_register; break;	// 1 byte
		case PITFLT: *((uint8_t *) buffer) = pitflt_register; break;	// 1 byte
		case PITCE: *((uint8_t *) buffer) = pitce_register; break;	// 1 byte
		case PITMUX: *((uint8_t *) buffer) = pitmux_register; break;	// 1 byte
		case PITINTE: *((uint8_t *) buffer) = pitinte_register; break;	// 1 byte
		case PITTF: *((uint8_t *) buffer) = pittf_register; break; // 1 byte
		case PITMTLD0: *((uint8_t *) buffer) = pitmtld0_register; break; // 1 byte
		case PITMTLD1: *((uint8_t *) buffer) = pitmtld1_register; break; // 1 byte
		default: {
			if ((offset >= PITLD0) && (offset <= PITCNT3)) {
				uint8_t group_index = (offset - PITLD0) / 2;
				uint8_t group_register = (offset - PITLD0) % 2;
				if (group_register == 0) {
					*((uint16_t *) buffer) = Host2BigEndian(pitld_register[group_index]);
				} else {
					*((uint16_t *) buffer) = Host2BigEndian(pitcnt_register[group_index]);
				}
			}
			else if ((offset >= RESERVED) && (offset < (RESERVED + 16))) {
				memcpy((void*) buffer, (reserved_register + offset), size);
			}
			else {
				return (false);
			}
		} break;
		}

		return (true);

	}

	return (false);
}

bool S12PIT24B4C::WriteMemory(physical_address_t addr, const void *buffer, uint32_t size) {

	if ((addr >= baseAddress) && (addr < (baseAddress+40))) {

		if (size == 0) {
			return (true);
		}

		physical_address_t offset = addr-baseAddress;

		switch (offset) {
		case PITCFLMT:	{
			uint8_t val = *((uint8_t *) buffer);
			pitcflmt_register = val;	// 1 byte
		} break;
		case PITFLT: {
			uint8_t val = *((uint8_t *) buffer);
			*((uint8_t *) buffer) = pitflt_register; // 1 byte
		} break;
		case PITCE: {
			uint8_t val = *((uint8_t *) buffer);
			*((uint8_t *) buffer) = pitce_register; // 1 byte
		} break;
		case PITMUX: {
			uint8_t val = *((uint8_t *) buffer);
			*((uint8_t *) buffer) = pitmux_register; // 1 byte
		} break;
		case PITINTE: {
			uint8_t val = *((uint8_t *) buffer);
			*((uint8_t *) buffer) = pitinte_register; // 1 byte
		} break;
		case PITTF: {
			uint8_t val = *((uint8_t *) buffer);
			*((uint8_t *) buffer) = pittf_register; // 1 byte
		} break;
		case PITMTLD0: {
			uint8_t val = *((uint8_t *) buffer);
			*((uint8_t *) buffer) = pitmtld0_register; // 1 byte
		} break;
		case PITMTLD1: {
			uint8_t val = *((uint8_t *) buffer);
			*((uint8_t *) buffer) = pitmtld1_register; // 1 byte
		} break;
		default: {
			if ((offset >= PITLD0) && (offset <= PITCNT3)) {
				uint8_t group_index = (offset - PITLD0) / 2;
				uint8_t group_register = (offset - PITLD0) % 2;
				uint16_t val = BigEndian2Host(*((uint16_t *) buffer));
				if (group_register == 0) {
					pitld_register[group_index] = val;
				} else {
					// write to PITCNTx registers has no meaning or effect
					pitcnt_register[group_index] = val;
				}
			}
			else if ((offset >= RESERVED) && (offset < (RESERVED + 16))) {
				// write to reserved area has no effect
				memcpy((reserved_register + offset), buffer, size);
			}
			else {
				return (false);
			}
		} break;
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




