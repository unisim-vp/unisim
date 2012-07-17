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

#include <assert.h>

#include <unisim/component/tlm2/processor/hcs12x/xint.hh>
#include <unisim/component/tlm2/processor/hcs12x/tlm_types.hh>
#include "unisim/util/debug/simple_register.hh"

namespace unisim {
namespace component {
namespace tlm2 {
namespace processor {
namespace hcs12x {

using unisim::util::debug::SimpleRegister;

address_t XINT::XINT_REGS_ADDRESSES[XINT::XINT_MEMMAP_SIZE];

XINT::XINT(const sc_module_name& name, Object *parent) :
	Object(name, parent),
	sc_module(name),
	unisim::kernel::service::Service<Memory<physical_address_t> >(name, parent),
	unisim::kernel::service::Service<Registers>(name, parent),
	Client<Memory<physical_address_t> >(name, parent),

	interrupt_request("interrupt_request"),

	memory_export("memory_export", this),
	memory_import("memory_import", this),
	registers_export("registers_export", this),

	input_payload_queue("input_payload_queue"),

	debug_enabled(false),
	param_debug_enabled("debug-enabled", this, debug_enabled)
{

	interrupt_request(*this);

	toCPU12X_request(*this);
	toXGATE_request(*this);

	slave_socket.register_b_transport(this, &XINT::read_write);

	SC_HAS_PROCESS(XINT);

	SC_THREAD(run);

	int_cfwdata = (uint8_t *) malloc (XINT_SIZE * sizeof(uint8_t));

	zeroTime = sc_time((double)0, SC_PS);

	XINT_REGS_ADDRESSES[IVBR]		= 0x0121;	// S12XINT: Address of the Interrupt Vector Base Register

	XINT_REGS_ADDRESSES[INT_XGPRIO] = 0x0126;

	XINT_REGS_ADDRESSES[INT_CFADDR] = 0x0127;

	XINT_REGS_ADDRESSES[INT_CFDATA0] = 0x0128;

	XINT_REGS_ADDRESSES[INT_CFDATA1] = 0x0129;

	XINT_REGS_ADDRESSES[INT_CFDATA2] = 0x012A;

	XINT_REGS_ADDRESSES[INT_CFDATA3] = 0x012B;

	XINT_REGS_ADDRESSES[INT_CFDATA4] = 0x012C;

	XINT_REGS_ADDRESSES[INT_CFDATA5] = 0x012D;

	XINT_REGS_ADDRESSES[INT_CFDATA6] = 0x012E;

	XINT_REGS_ADDRESSES[INT_CFDATA7] = 0x012F;

}

XINT::~XINT() {

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

	free(int_cfwdata);
}

// Master methods
void XINT::invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range)
{
	// Leave this empty as it is designed for memory mapped buses
}

unsigned int XINT::transport_dbg(XINT_Payload& payload)
{
	// Leave this empty as it is designed for memory mapped buses
	return (0);
}

void XINT::b_transport(XINT_Payload& payload, sc_core::sc_time& t) {
	payload.acquire();
	input_payload_queue.notify(payload);
}

bool XINT::get_direct_mem_ptr(XINT_Payload& payload, tlm_dmi&  dmi_data) {
	return (false);
}

tlm_sync_enum XINT::nb_transport_fw(XINT_Payload& payload, tlm_phase& phase, sc_core::sc_time& t)
{
	switch(phase)
	{
		case BEGIN_REQ:
			// accepts an interrupt request modeled by payload
			phase = END_REQ; // update the phase

			payload.acquire();
			input_payload_queue.notify(payload); // queue the payload and the associative time

			return (TLM_UPDATED);
		case END_REQ:
			cout << sc_time_stamp() << ":" << sc_object::name() << ": received an unexpected phase END_REQ" << endl;

			Object::Stop(-1);
			break;
		case BEGIN_RESP:
			cout << sc_time_stamp() << ":" << sc_object::name() << ": received an unexpected phase BEGIN_RESP" << endl;

			Object::Stop(-1);
			break;
		case END_RESP:
			cout << sc_time_stamp() << ":" << sc_object::name() << ": received an unexpected phase END_RESP" << endl;

			Object::Stop(-1);
			break;
		default:
			cout << sc_time_stamp() << ":" << sc_object::name() << ": received an unexpected phase" << endl;

			Object::Stop(-1);
			break;
	}

	return (TLM_ACCEPTED);
}

bool XINT::selectInterrupt(INT_TRANS_T &buffer) {

	if (interrupt_flags[XINT::INT_CLK_MONITOR_RESET_OFFSET/2].getState()) {

		buffer.setVectorAddress(get_ClockMonitorReset_Vector());
		buffer.setID(XINT::INT_CLK_MONITOR_RESET_OFFSET/2);
		buffer.setPriority(0x7);

	}
	else if (interrupt_flags[XINT::INT_COP_WATCHDOG_RESET_OFFSET/2].getState()) {

		buffer.setVectorAddress(get_COPWatchdogReset_Vector());
		buffer.setID(XINT::INT_COP_WATCHDOG_RESET_OFFSET/2);
		buffer.setPriority(0x7);

	}
	else if (interrupt_flags[XINT::INT_ILLEGAL_ACCESS_RESET_OFFSET/2].getState()) {

		buffer.setVectorAddress(get_IllegalAccessReset_Vector());
		buffer.setID(XINT::INT_ILLEGAL_ACCESS_RESET_OFFSET/2);
		buffer.setPriority(0x7);

	}
	else if (interrupt_flags[XINT::INT_SYS_RESET_OFFSET/2].getState()) {

		buffer.setVectorAddress(get_SysReset_Vector());
		buffer.setID(XINT::INT_SYS_RESET_OFFSET/2);
		buffer.setPriority(0x7);

	}
	else if (interrupt_flags[XINT::INT_XIRQ_OFFSET/2].getState()) {

		buffer.setVectorAddress(get_XIRQ_Vector());
		buffer.setID(XINT::INT_XIRQ_OFFSET/2);
		buffer.setPriority(0x7);
	}
	else {
//		for (int index=0; index < XINT_SIZE; index++) {
		for (int index=0x30; index < 0x79; index++) {
			if (interrupt_flags[index].getState()) {
				uint8_t dataPriority = int_cfwdata[index] & 0x07;

				if (dataPriority >= buffer.getPriority()) {  // priority of maskable interrupts is from high offset to low
					buffer.setPriority(dataPriority);
					buffer.setID(index);
					buffer.setVectorAddress(((address_t) getIVBR() << 8) + index * 2);
				}

			}
		}

	}

//	if (buffer.getVectorAddress() == 0) {
//		buffer.setVectorAddress(((address_t) getIVBR() << 8));
//		return (false);
//	}

	if (buffer.getVectorAddress() == 0) {
		buffer.setVectorAddress(get_Spurious_Vector());
		buffer.setPriority(0x7);
		buffer.setID(get_Spurious_Vector()/2);
		return (false);
	}

	return (true);

}

void XINT::run()
{
	XINT_Payload *payload;

	// This thread is waked-up by any not CPU interrupt

	tlm::tlm_generic_payload* trans = payloadFabric.allocate();

	tlm_phase *phase = new tlm_phase(BEGIN_REQ);

	while (true) {

		wait(input_payload_queue.get_event() | retry_event);

		do
		{

			payload = input_payload_queue.get_next_transaction();
			if (payload) {

				uint8_t id = payload->interrupt_offset/2;

				interrupt_flags[id].setState(true);
				interrupt_flags[id].setPayload(payload);

				trans->set_command( tlm::TLM_WRITE_COMMAND );
				trans->set_response_status( tlm::TLM_INCOMPLETE_RESPONSE );

//				cout << "XINT rise " << std::hex << (unsigned int) payload->interrupt_offset << "  @ " << sc_time_stamp().to_seconds() << endl;

				// if 7-bit=0 then cpu else xgate
				if ((int_cfwdata[id] & 0x80) == 0)
				{
					tlm_sync_enum ret = toCPU12X_request->nb_transport_fw( *trans, *phase, zeroTime );
				}
				else {
					tlm_sync_enum ret = toXGATE_request->nb_transport_fw( *trans, *phase, zeroTime );
				}

			}

		} while(payload);

		payload = NULL;

	}

	delete phase;
	trans->release();

}

// Master methods
tlm_sync_enum XINT::nb_transport_bw(tlm::tlm_generic_payload& trans, tlm_phase& phase, sc_core::sc_time& t)
{

	// The comparaison of the newIPL to the currentIPL is done by the CPU during I-bit-interrupt handling

	INT_TRANS_T *buffer = (INT_TRANS_T *) trans.get_data_ptr();
	uint8_t cpuIPL = buffer->getPriority();
	if (selectInterrupt(*buffer)) {
		if (buffer->getPriority() > cpuIPL) {
			interrupt_flags[buffer->getID()].setState(false);
			interrupt_flags[buffer->getID()].releasePayload();

		}

		retry_event.notify();
	}

	trans.set_response_status(tlm::TLM_OK_RESPONSE);
	return (TLM_ACCEPTED);
}



void XINT::Reset() {

	XINT_REGS_ADDRESSES[IVBR]		= 0x0121;	// S12XINT: Address of the Interrupt Vector Base Register

	XINT_REGS_ADDRESSES[INT_XGPRIO] = 0x0126;

	XINT_REGS_ADDRESSES[INT_CFADDR] = 0x0127;

	ivbr = 0xFF;
	int_xgprio = 0x01;
	int_cfaddr = 0x10;

	for (uint8_t i=0; i< XINT::INT_RAM_ACCESS_VIOLATION_OFFSET/2; i++) {
		int_cfwdata[i] = 0x00;
	}
	for (uint8_t i=XINT::INT_RAM_ACCESS_VIOLATION_OFFSET/2; i<XINT::INT_XIRQ_OFFSET/2; i++) {
		int_cfwdata[i] = 0x01;
	}
	for (uint8_t i=XINT::INT_XIRQ_OFFSET/2; i<XINT_SIZE; i++) {
		int_cfwdata[i] = 0x00;
	}

	for (int i=0; i<XINT_SIZE; i++) {
		interrupt_flags[i].setState(false);
		interrupt_flags[i].releasePayload();
	}

}

bool XINT::BeginSetup() {

	Reset();

	char buf[80];

	sprintf(buf, "%s.IVBR",sc_object::name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &ivbr);

	unisim::kernel::service::Register<uint8_t> *ivbr_var = new unisim::kernel::service::Register<uint8_t>("IVBR", this, ivbr, "Interrupt Vector base register (IVBR)");
	extended_registers_registry.push_back(ivbr_var);
	ivbr_var->setCallBack(this, IVBR, &CallBackObject::write, NULL);

	sprintf(buf, "%s.INT_XGPRIO",sc_object::name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &int_xgprio);

	unisim::kernel::service::Register<uint8_t> *int_xgprio_var = new unisim::kernel::service::Register<uint8_t>("INT_XGPRIO", this, int_xgprio, "XGate Interrupt Priority Configuration Register (INT_XGPRIO)");
	extended_registers_registry.push_back(int_xgprio_var);
	int_xgprio_var->setCallBack(this, INT_XGPRIO, &CallBackObject::write, NULL);

	sprintf(buf, "%s.INT_CFADDR",sc_object::name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &int_cfaddr);

	unisim::kernel::service::Register<uint8_t> *int_cfaddr_var = new unisim::kernel::service::Register<uint8_t>("INT_CFADDR", this, int_cfaddr, "Interrupt Request Configuration Address Register (INT_CFADDR)");
	extended_registers_registry.push_back(int_cfaddr_var);
	int_cfaddr_var->setCallBack(this, INT_CFADDR, &CallBackObject::write, NULL);

	for (uint8_t i=0; i<XINT_SIZE; i++) {
		sprintf(buf, "%s.INT_CFDATA%d", sc_object::name(), i);
		registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &int_cfwdata[i]);

		sprintf(buf, "INT_CFDATA%d", i);

		unisim::kernel::service::Register<uint8_t> *int_cfwdata_var = new unisim::kernel::service::Register<uint8_t>(buf, this, int_cfwdata[i], "Interrupt Request Configuration Data Registers (INT_CFDATA)");
		extended_registers_registry.push_back(int_cfwdata_var);
		int_cfwdata_var->setCallBack(this, INT_CFDATA0+i, &CallBackObject::write, NULL);
	}

	return (true);
}

bool XINT::Setup(ServiceExportBase *srv_export) {
	return (true);
}

bool XINT::EndSetup() {
	return (true);
}

/**
 * Gets a register interface to the register specified by name.
 *
 * @param name The name of the requested register.
 * @return A pointer to the RegisterInterface corresponding to name.
 */
Register* XINT::GetRegister(const char *name)
{
	if(registers_registry.find(string(name)) != registers_registry.end())
		return (registers_registry[string(name)]);
	else
		return (NULL);

}

void XINT::read_write( tlm::tlm_generic_payload& trans, sc_time& delay )
{
	tlm::tlm_command cmd = trans.get_command();
	sc_dt::uint64 address = trans.get_address();
	uint8_t* data_ptr = (uint8_t *)trans.get_data_ptr();
	unsigned int data_length = trans.get_data_length();

	if (cmd == tlm::TLM_READ_COMMAND) {
		memset(data_ptr, 0, data_length);
		read((address_t) address, data_ptr, data_length);
	} else if (cmd == tlm::TLM_WRITE_COMMAND) {
		write((address_t) address, data_ptr, data_length);
	}

	trans.set_response_status( tlm::TLM_OK_RESPONSE );
}

bool XINT::write(unsigned int address, const void *buffer, unsigned int data_length)
{
	uint8_t value = *((uint8_t *) buffer);
	if (address == XINT_REGS_ADDRESSES[IVBR]) setIVBR(value);
	else if (address == XINT_REGS_ADDRESSES[INT_XGPRIO]) setINT_XGPRIO(value);
	else if (address == XINT_REGS_ADDRESSES[INT_CFADDR]) setINT_CFADDR(value);
	else if (address == XINT_REGS_ADDRESSES[INT_CFDATA0]) write_INT_CFDATA(0, value);
	else if (address == XINT_REGS_ADDRESSES[INT_CFDATA1]) write_INT_CFDATA(1, value);
	else if (address == XINT_REGS_ADDRESSES[INT_CFDATA2]) write_INT_CFDATA(2, value);
	else if (address == XINT_REGS_ADDRESSES[INT_CFDATA3]) write_INT_CFDATA(3, value);
	else if (address == XINT_REGS_ADDRESSES[INT_CFDATA4]) write_INT_CFDATA(4, value);
	else if (address == XINT_REGS_ADDRESSES[INT_CFDATA5]) write_INT_CFDATA(5, value);
	else if (address == XINT_REGS_ADDRESSES[INT_CFDATA6]) write_INT_CFDATA(6, value);
	else if (address == XINT_REGS_ADDRESSES[INT_CFDATA7]) write_INT_CFDATA(7, value);
	else return (false);

	return (true);
}

bool XINT::read(unsigned int address, const void *buffer, unsigned int data_length)
{
	if (address == XINT_REGS_ADDRESSES[IVBR]) *((uint8_t *) buffer) = getIVBR();
	else if (address == XINT_REGS_ADDRESSES[INT_XGPRIO]) *((uint8_t *) buffer) = getINT_XGPRIO();
	else if (address == XINT_REGS_ADDRESSES[INT_CFADDR]) *((uint8_t *) buffer) = getINT_CFADDR();
	else if (address == XINT_REGS_ADDRESSES[INT_CFDATA0]) *((uint8_t *) buffer) = read_INT_CFDATA(0);
	else if (address == XINT_REGS_ADDRESSES[INT_CFDATA1]) *((uint8_t *) buffer) = read_INT_CFDATA(1);
	else if (address == XINT_REGS_ADDRESSES[INT_CFDATA2]) *((uint8_t *) buffer) = read_INT_CFDATA(2);
	else if (address == XINT_REGS_ADDRESSES[INT_CFDATA3]) *((uint8_t *) buffer) = read_INT_CFDATA(3);
	else if (address == XINT_REGS_ADDRESSES[INT_CFDATA4]) *((uint8_t *) buffer) = read_INT_CFDATA(4);
	else if (address == XINT_REGS_ADDRESSES[INT_CFDATA5]) *((uint8_t *) buffer) = read_INT_CFDATA(5);
	else if (address == XINT_REGS_ADDRESSES[INT_CFDATA6]) *((uint8_t *) buffer) = read_INT_CFDATA(6);
	else if (address == XINT_REGS_ADDRESSES[INT_CFDATA7]) *((uint8_t *) buffer) = read_INT_CFDATA(7);
	else return (false);

	return (true);
}

uint8_t XINT::getIVBR() { return (ivbr); }
void XINT::setIVBR(uint8_t value) { ivbr = value; }

uint8_t	XINT::getINT_XGPRIO() { return (int_xgprio); }
void XINT::setINT_XGPRIO(uint8_t value) { int_xgprio = value; }

uint8_t	XINT::getINT_CFADDR() { return (int_cfaddr); }
void XINT::setINT_CFADDR(uint8_t value) { int_cfaddr = value; }

uint8_t	XINT::read_INT_CFDATA(uint8_t index)
{

	uint8_t cfaddr = getINT_CFADDR();

	if (cfaddr == 0xF0) {
		/*
		 *  if 0x00F4-0x00FE are selected
		 *  then
		 *   - write access to PRIOLVL[2:0] are ignored
		 *   - read access of PRIOLVL[2:0] return all 0s
		 */
		if (index > 1) return (int_cfwdata[getINT_CFADDR()/2 + index] & 0x80);

		/*
		 * CFDATA1 register is used for IRQ interrupt when writing 0xF0 to CFADDR
		 * The !IRQ interrupt cannot be handled by XGATE module. For this reason RQST is always set to 0.
		 */
		if (index == 1) return (int_cfwdata[getINT_CFADDR()/2 + index] & 0x7F);
	}
	else
		/*
		 * - Writing all 0s selects non-existing configuration registers.
		 */
		if (cfaddr == 0x0) return (0);

	/*
	 * - Write access to CFDATA of the spurious interrupt will be ignored
	 * - Read access to CFDATA of the spurious interrupt will always return 0x7
	 */
	if ((getINT_CFADDR() + index*2) == XINT::INT_SPURIOUS_OFFSET) return (0x7);


	return (int_cfwdata[getINT_CFADDR()/2 + index]);
}

void XINT::write_INT_CFDATA(uint8_t index, uint8_t value)
{
	uint8_t cfaddr = getINT_CFADDR();

	if (cfaddr == 0xF0) {
		/*
		 *  if 0x00F4-0x00FE are selected
		 *  then
		 *   - write access to PRIOLVL[2:0] are ignored
		 *   - read access of PRIOLVL[2:0] return all 0s
		 */
		if (index > 1) return;

		/*
		 * CFDATA1 register is used for IRQ interrupt when writing 0xF0 to CFADDR
		 * The !IRQ interrupt cannot be handled by XGATE module. For this reason RQST is always set to 0.
		 */
		if (index == 1)
		{
			int_cfwdata[getINT_CFADDR()/2 + index] = value & 0x7F;
			return;

		}
	}
	else
		/*
		 * - Writing all 0s selects non-existing configuration registers.
		 */
		if (cfaddr == 0x0) return;

	/*
	 * - Write access to CFDATA of the spurious interrupt will be ignored
	 * - Read access to CFDATA of the spurious interrupt will always return 0x7
	 */
	if ((getINT_CFADDR() + index*2) == XINT::INT_SPURIOUS_OFFSET) return;

	cout << "XINT write to => " << (unsigned int) (getINT_CFADDR()/2 + index) << "  = 0x" << std::hex << (unsigned int) value << endl;

	int_cfwdata[getINT_CFADDR()/2 + index] = value;
}


//=====================================================================
//=             memory interface methods                              =
//=====================================================================

bool XINT::ReadMemory(physical_address_t addr, void *buffer, uint32_t size) {

	if ((address_t) addr == XINT_REGS_ADDRESSES[IVBR]) { *(uint8_t *) buffer = getIVBR(); return (true); }
	else if ((address_t) addr == XINT_REGS_ADDRESSES[INT_XGPRIO]) { *(uint8_t *) buffer = getINT_XGPRIO(); return (true); }
	else if ((address_t) addr == XINT_REGS_ADDRESSES[INT_CFADDR]) { *(uint8_t *) buffer = getINT_CFADDR(); return (true); }
	else if ((address_t) addr == XINT_REGS_ADDRESSES[INT_CFDATA0]) { *(uint8_t *) buffer = int_cfwdata[getINT_CFADDR()/2 + 0]; return (true); }
	else if ((address_t) addr == XINT_REGS_ADDRESSES[INT_CFDATA1]) { *(uint8_t *) buffer = int_cfwdata[getINT_CFADDR()/2 + 1]; return (true); }
	else if ((address_t) addr == XINT_REGS_ADDRESSES[INT_CFDATA2]) { *(uint8_t *) buffer = int_cfwdata[getINT_CFADDR()/2 + 2]; return (true); }
	else if ((address_t) addr == XINT_REGS_ADDRESSES[INT_CFDATA3]) { *(uint8_t *) buffer = int_cfwdata[getINT_CFADDR()/2 + 3]; return (true); }
	else if ((address_t) addr == XINT_REGS_ADDRESSES[INT_CFDATA4]) { *(uint8_t *) buffer = int_cfwdata[getINT_CFADDR()/2 + 4]; return (true); }
	else if ((address_t) addr == XINT_REGS_ADDRESSES[INT_CFDATA5]) { *(uint8_t *) buffer = int_cfwdata[getINT_CFADDR()/2 + 5]; return (true); }
	else if ((address_t) addr == XINT_REGS_ADDRESSES[INT_CFDATA6]) { *(uint8_t *) buffer = int_cfwdata[getINT_CFADDR()/2 + 6]; return (true); }
	else if ((address_t) addr == XINT_REGS_ADDRESSES[INT_CFDATA7]) { *(uint8_t *) buffer = int_cfwdata[getINT_CFADDR()/2 + 7]; return (true); }

	return (false);
}

bool XINT::WriteMemory(physical_address_t addr, const void *buffer, uint32_t size) {

	for (uint8_t i=0; i<XINT_MEMMAP_SIZE; i++) {
		if (XINT_REGS_ADDRESSES[i] == addr) {
			write(addr, buffer, size);
			return (true);
		}
	}

	return (false);
}


} // end of namespace hcs12x
} // end of namespace processor
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

