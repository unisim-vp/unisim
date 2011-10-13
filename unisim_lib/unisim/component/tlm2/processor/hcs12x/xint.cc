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
uint8_t XINT::XINT_REGS_RESET_VALUES[XINT::XINT_MEMMAP_SIZE];

XINT::XINT(const sc_module_name& name, Object *parent) :
	Object(name, parent),
	sc_module(name),
	Service<Memory<service_address_t> >(name, parent),
	Service<Registers>(name, parent),
	Client<Memory<service_address_t> >(name, parent),

	interrupt_request("interrupt_request"),

	memory_export("memory_export", this),
	memory_import("memory_import", this),
	registers_export("registers_export", this),

	input_payload_queue("input_payload_queue"),

	isHardwareInterrupt(false),
	debug_enabled(false),
	param_debug_enabled("debug-enabled", this, debug_enabled)
{

	interrupt_request(*this);
	fromCPU_Target.register_b_transport(this, &XINT::getVectorAddress);
	slave_socket.register_b_transport(this, &XINT::read_write);

	SC_HAS_PROCESS(XINT);

	SC_THREAD(Run);

	zeroTime = sc_time((double)0, SC_PS);

	XINT_REGS_ADDRESSES[IVBR]		= 0x0121;	// S12XINT: Address of the Interrupt Vector Base Register
	XINT_REGS_RESET_VALUES[IVBR]	= 0xFF;		// IVBR is only one.

	XINT_REGS_ADDRESSES[INT_XGPRIO] = 0x0126;
	XINT_REGS_RESET_VALUES[INT_XGPRIO]	= 0x01;

	XINT_REGS_ADDRESSES[INT_CFADDR] = 0x0127;
	XINT_REGS_RESET_VALUES[INT_CFADDR] = 0x10;

	XINT_REGS_ADDRESSES[INT_CFDATA0] = 0x0128;
	XINT_REGS_RESET_VALUES[INT_CFDATA0]	= 0x01;

	XINT_REGS_ADDRESSES[INT_CFDATA1] = 0x0129;
	XINT_REGS_RESET_VALUES[INT_CFDATA1]	= 0x01;

	XINT_REGS_ADDRESSES[INT_CFDATA2] = 0x012A;
	XINT_REGS_RESET_VALUES[INT_CFDATA2]	= 0x01;

	XINT_REGS_ADDRESSES[INT_CFDATA3] = 0x012B;
	XINT_REGS_RESET_VALUES[INT_CFDATA3]	= 0x01;

	XINT_REGS_ADDRESSES[INT_CFDATA4] = 0x012C;
	XINT_REGS_RESET_VALUES[INT_CFDATA4]	= 0x01;

	XINT_REGS_ADDRESSES[INT_CFDATA5] = 0x012D;
	XINT_REGS_RESET_VALUES[INT_CFDATA5]	= 0x01;

	XINT_REGS_ADDRESSES[INT_CFDATA6] = 0x012E;
	XINT_REGS_RESET_VALUES[INT_CFDATA6]	= 0x01;

	XINT_REGS_ADDRESSES[INT_CFDATA7] = 0x012F;
	XINT_REGS_RESET_VALUES[INT_CFDATA7]	= 0x01;

	Reset();

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

}

// Master methods
void XINT::invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range)
{
	// Leave this empty as it is designed for memory mapped buses
}

unsigned int XINT::transport_dbg(XINT_Payload& payload)
{
	// Leave this empty as it is designed for memory mapped buses
	return 0;
}

void XINT::b_transport(XINT_Payload& payload, sc_core::sc_time& t) {
	payload.acquire();
	input_payload_queue.notify(payload, t);
}

bool XINT::get_direct_mem_ptr(XINT_Payload& payload, tlm_dmi&  dmi_data) {
	return false;
}

tlm_sync_enum XINT::nb_transport_fw(XINT_Payload& payload, tlm_phase& phase, sc_core::sc_time& t)
{
	switch(phase)
	{
		case BEGIN_REQ:
			// accepts an interrupt request modeled by payload
			phase = END_REQ; // update the phase

			payload.acquire();
			input_payload_queue.notify(payload, t); // queue the payload and the associative time

			return TLM_UPDATED;
		case END_REQ:
			cout << sc_time_stamp() << ":" << name() << ": received an unexpected phase END_REQ" << endl;

			Object::Stop(-1);
			break;
		case BEGIN_RESP:
			cout << sc_time_stamp() << ":" << name() << ": received an unexpected phase BEGIN_RESP" << endl;

			Object::Stop(-1);
			break;
		case END_RESP:
			cout << sc_time_stamp() << ":" << name() << ": received an unexpected phase END_RESP" << endl;

			Object::Stop(-1);
			break;
		default:
			cout << sc_time_stamp() << ":" << name() << ": received an unexpected phase" << endl;

			Object::Stop(-1);
			break;
	}

	return TLM_ACCEPTED;
}

/*
 *    This method is called to compute the interrupt vector based on currentIPL
 */
void XINT::getVectorAddress( tlm::tlm_generic_payload& trans, sc_time& delay )
{

	INT_TRANS_T *buffer = (INT_TRANS_T *) trans.get_data_ptr();

	uint8_t newIPL = 0;
	address_t vectorAddress = 0;

	/*
	 * Check if it is reset
	 */
	if (interrupt_flags[XINT::INT_CLK_MONITOR_RESET_OFFSET/2]) {
		vectorAddress = get_ClockMonitorReset_Vector();
		interrupt_flags[XINT::INT_CLK_MONITOR_RESET_OFFSET/2] = false;
	}
	else if (interrupt_flags[XINT::INT_COP_WATCHDOG_RESET_OFFSET/2]) {
		vectorAddress = get_COPWatchdogReset_Vector();
		interrupt_flags[XINT::INT_COP_WATCHDOG_RESET_OFFSET/2] = false;
	}
	else if (interrupt_flags[XINT::INT_ILLEGAL_ACCESS_RESET_OFFSET/2]) {
		vectorAddress = get_IllegalAccessReset_Vector();
		interrupt_flags[XINT::INT_ILLEGAL_ACCESS_RESET_OFFSET/2] = false;
	}
	else if (interrupt_flags[XINT::INT_SYS_RESET_OFFSET/2]) {
		vectorAddress = get_SysReset_Vector();
		interrupt_flags[XINT::INT_SYS_RESET_OFFSET/2] = false;
	}
	else if (interrupt_flags[XINT::INT_XIRQ_OFFSET/2]) {
		vectorAddress = get_XIRQ_Vector();
		interrupt_flags[XINT::INT_XIRQ_OFFSET/2] = false;
	}
	else {
		uint8_t selectedInterrupt = 0;
		uint8_t visibleFlags = getINT_CFADDR()/2;

		for (int index=0; index < CFDATA_SIZE; index++) { // Check only the selected interrupts
			if (interrupt_flags[visibleFlags+index]) {
				uint8_t dataPriority = int_cfdata[index] && 0x07;

				// if 7-bit=0 then cpu else xgate
				if ((int_cfdata[index] & 0x80) == 0)
				{
					if (dataPriority >= newIPL) {  // priority of maskable interrupts is from high offset to low
						newIPL = dataPriority;
						selectedInterrupt = visibleFlags+index;
					}
				}
				else ;

			}
		}


//		vectorAddress = (getIVBR() << 8) + selectedInterrupt * 2;
		vectorAddress = getIVBR();
		vectorAddress = (vectorAddress << 8) + selectedInterrupt * 2;
		interrupt_flags[selectedInterrupt] = false;
	}

	if (vectorAddress == 0) {
		if (isHardwareInterrupt) {
			vectorAddress = get_Spurious_Vector();
			newIPL = 0x7;
		} else {
			vectorAddress = getIVBR() << 8 ;
		}
	}

	// The comparaison of the newIPL to the currentIPL is done by the CPU during I-bit-interrupt handling
	buffer->ipl = newIPL;
	buffer->vectorAddress = vectorAddress;

//	if (debug_enabled) {
//		cout << "XINT::  Vector Address 0x" << std::hex << vectorAddress << std::dec << endl;
//	}

	isHardwareInterrupt = false;
	trans.set_response_status( tlm::TLM_OK_RESPONSE );

}

void XINT::Run()
{
	XINT_Payload *payload;

	bool hasVisibleInterrupt;

	// This thread is waked-up by any not CPU interrupt

	while (true) {

		wait(input_payload_queue.get_event());

		hasVisibleInterrupt = false;

		do
		{

			payload = input_payload_queue.get_next_transaction();
			if (payload) {

//				if (debug_enable) {
//					cout << "*************  XINT: Receive INT-ID (offset/2) " << std::hex << payload->interrupt_offset/2 << std::dec << std::endl;
//				}

				if ((payload->interrupt_offset == XINT::INT_CLK_MONITOR_RESET_OFFSET) ||
					(payload->interrupt_offset == XINT::INT_COP_WATCHDOG_RESET_OFFSET) ||
					(payload->interrupt_offset == XINT::INT_ILLEGAL_ACCESS_RESET_OFFSET) ||
					(payload->interrupt_offset == XINT::INT_SYS_RESET_OFFSET) ||
					(payload->interrupt_offset == XINT::INT_XIRQ_OFFSET) ||
					((payload->interrupt_offset >= getINT_CFADDR()) && (payload->interrupt_offset < (getINT_CFADDR() + 16)))) {

					if (debug_enabled) {
						cout << "*************  XINT: Receive Visible INT-ID (offset/2) " << std::hex << payload->interrupt_offset/2 << std::dec << std::endl;
					}

					interrupt_flags[payload->interrupt_offset/2] = true;
					hasVisibleInterrupt = true;
				}

				payload->release();
			}

		} while(payload);

		payload = NULL;

		if (hasVisibleInterrupt) {
			isHardwareInterrupt = true;

			// Rise interrupt

			tlm::tlm_generic_payload* trans = payloadFabric.allocate();

			trans->set_command( tlm::TLM_WRITE_COMMAND );
			trans->set_response_status( tlm::TLM_INCOMPLETE_RESPONSE );

			toCPU_Initiator->b_transport( *trans, zeroTime );

			if (trans->is_response_error() )
				SC_REPORT_ERROR("XINT : ", " interrupt notification failed.");


			trans->release();
		}
	}

}


void XINT::Reset() {

	ivbr = XINT_REGS_RESET_VALUES[IVBR];
	int_xgprio = XINT_REGS_RESET_VALUES[INT_XGPRIO];
	int_cfaddr = XINT_REGS_RESET_VALUES[INT_CFADDR];
	int_cfdata[0] = XINT_REGS_RESET_VALUES[INT_CFDATA0];
	int_cfdata[1] = XINT_REGS_RESET_VALUES[INT_CFDATA1];
	int_cfdata[2] = XINT_REGS_RESET_VALUES[INT_CFDATA2];
	int_cfdata[3] =  XINT_REGS_RESET_VALUES[INT_CFDATA3];
	int_cfdata[4] =  XINT_REGS_RESET_VALUES[INT_CFDATA4];
	int_cfdata[5] =  XINT_REGS_RESET_VALUES[INT_CFDATA5];
	int_cfdata[6] =  XINT_REGS_RESET_VALUES[INT_CFDATA6];
	int_cfdata[7] =  XINT_REGS_RESET_VALUES[INT_CFDATA7];

	for (int i=0; i<XINT_SIZE; i++) {
		interrupt_flags[i] = false;
	}

}

bool XINT::BeginSetup() {

	char buf[80];

	sprintf(buf, "%s.IVBR",name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &ivbr);
	extended_registers_registry.push_back(new unisim::kernel::service::Register<uint8_t>("IVBR", this, ivbr, "Interrupt Vector Base Register (IVBR)"));

	sprintf(buf, "%s.INT_XGPRIO",name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &int_xgprio);
	extended_registers_registry.push_back(new unisim::kernel::service::Register<uint8_t>("INT_XGPRIO", this, int_xgprio, "XGATE Interrupt Priority Configuration Register (INT_XGPRIO)"));

	sprintf(buf, "%s.INT_CFADDR",name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &int_cfaddr);
	extended_registers_registry.push_back(new unisim::kernel::service::Register<uint8_t>("INT_CFADDR", this, int_cfaddr, "Interrupt Request Configuration Address Register (INT_CFADDR)"));

	char shortName[20];
	for (uint8_t i=0; i<8; i++) {
		sprintf(shortName, "INT_CFDATA%d", i);
		sprintf(buf, "%s.%s", name(), shortName);
		registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &int_cfdata[i]);
		extended_registers_registry.push_back(new unisim::kernel::service::Register<uint8_t>(shortName, this, int_cfdata[i], "Interrupt Request Configuration Data Register"));
	}

	return true;
}

bool XINT::Setup(ServiceExportBase *srv_export) {
	return true;
}

bool XINT::EndSetup() {
	return true;
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
		return registers_registry[string(name)];
	else
		return NULL;

}

void XINT::read_write( tlm::tlm_generic_payload& trans, sc_time& delay )
{
	tlm::tlm_command cmd = trans.get_command();
	sc_dt::uint64 address = trans.get_address();
	uint8_t* data_ptr = (uint8_t *)trans.get_data_ptr();

	if (cmd == tlm::TLM_READ_COMMAND) {
		unsigned int data_length = trans.get_data_length();
		memset(data_ptr, 0, data_length);
		read((address_t) address, *data_ptr);
	} else if (cmd == tlm::TLM_WRITE_COMMAND) {
		write((address_t) address, *data_ptr);
	}

	trans.set_response_status( tlm::TLM_OK_RESPONSE );
}

bool XINT::write(address_t address, uint8_t value)
{
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
	else return false;

	return true;
}

bool XINT::read(address_t address, uint8_t &value)
{
	if (address == XINT_REGS_ADDRESSES[IVBR]) value = getIVBR();
	else if (address == XINT_REGS_ADDRESSES[INT_XGPRIO]) value = getINT_XGPRIO();
	else if (address == XINT_REGS_ADDRESSES[INT_CFADDR]) value = getINT_CFADDR();
	else if (address == XINT_REGS_ADDRESSES[INT_CFDATA0]) value = read_INT_CFDATA(0);
	else if (address == XINT_REGS_ADDRESSES[INT_CFDATA1]) value = read_INT_CFDATA(1);
	else if (address == XINT_REGS_ADDRESSES[INT_CFDATA2]) value = read_INT_CFDATA(2);
	else if (address == XINT_REGS_ADDRESSES[INT_CFDATA3]) value = read_INT_CFDATA(3);
	else if (address == XINT_REGS_ADDRESSES[INT_CFDATA4]) value = read_INT_CFDATA(4);
	else if (address == XINT_REGS_ADDRESSES[INT_CFDATA5]) value = read_INT_CFDATA(5);
	else if (address == XINT_REGS_ADDRESSES[INT_CFDATA6]) value = read_INT_CFDATA(6);
	else if (address == XINT_REGS_ADDRESSES[INT_CFDATA7]) value = read_INT_CFDATA(7);
	else return false;

	return true;
}

uint8_t XINT::getIVBR() { return ivbr; }
void XINT::setIVBR(uint8_t value) { ivbr = value; }

uint8_t	XINT::getINT_XGPRIO() { return int_xgprio; }
void XINT::setINT_XGPRIO(uint8_t value) { int_xgprio = value; }

uint8_t	XINT::getINT_CFADDR() { return int_cfaddr; }
void XINT::setINT_CFADDR(uint8_t value) { int_cfaddr = value; }

uint8_t	XINT::read_INT_CFDATA(uint8_t index)
{
	assert(index < CFDATA_SIZE);

	uint8_t cfaddr = getINT_CFADDR();

	if (cfaddr == 0xF0) {
		/*
		 *  if 0x00F4-0x00FE are selected
		 *  then
		 *   - write access to PRIOLVL[2:0] are ignored
		 *   - read access of PRIOLVL[2:0] return all 0s
		 */
		if (index > 1) return int_cfdata[index] & 0x80;

		/*
		 * CFDATA1 register is used for IRQ interrupt when writing 0xF0 to CFADDR
		 * The !IRQ interrupt cannot be handled by XGATE module. For this reason RQST is always set to 0.
		 */
		if (index == 1) return int_cfdata[index] & 0x7F;
	}
	else
		/*
		 * - Writing all 0s selects non-existing configuration registers.
		 */
		if (cfaddr == 0x0) return 0;

	/*
	 * - Write access to CFDATA of the spurious interrupt will be ignored
	 * - Read access to CFDATA of the spurious interrupt will always return 0x7
	 */
	if (cfaddr+index == XINT::INT_SPURIOUS_OFFSET) return 0x7;


	return int_cfdata[index];
}

void XINT::write_INT_CFDATA(uint8_t index, uint8_t value)
{
	assert(index < CFDATA_SIZE);

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
			int_cfdata[index] = value & 0x7F;
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
	if (cfaddr+index == XINT::INT_SPURIOUS_OFFSET) return;

	int_cfdata[index] = value;
}


//=====================================================================
//=             memory interface methods                              =
//=====================================================================

bool XINT::ReadMemory(service_address_t addr, void *buffer, uint32_t size) {

	if ((address_t) addr == XINT_REGS_ADDRESSES[IVBR]) { *(uint8_t *) buffer = getIVBR(); return true; }
	else if ((address_t) addr == XINT_REGS_ADDRESSES[INT_XGPRIO]) { *(uint8_t *) buffer = getINT_XGPRIO(); return true; }
	else if ((address_t) addr == XINT_REGS_ADDRESSES[INT_CFADDR]) { *(uint8_t *) buffer = getINT_CFADDR(); return true; }
	else if ((address_t) addr == XINT_REGS_ADDRESSES[INT_CFDATA0]) { *(uint8_t *) buffer = int_cfdata[0]; return true; }
	else if ((address_t) addr == XINT_REGS_ADDRESSES[INT_CFDATA1]) { *(uint8_t *) buffer = int_cfdata[1]; return true; }
	else if ((address_t) addr == XINT_REGS_ADDRESSES[INT_CFDATA2]) { *(uint8_t *) buffer = int_cfdata[2]; return true; }
	else if ((address_t) addr == XINT_REGS_ADDRESSES[INT_CFDATA3]) { *(uint8_t *) buffer = int_cfdata[3]; return true; }
	else if ((address_t) addr == XINT_REGS_ADDRESSES[INT_CFDATA4]) { *(uint8_t *) buffer = int_cfdata[4]; return true; }
	else if ((address_t) addr == XINT_REGS_ADDRESSES[INT_CFDATA5]) { *(uint8_t *) buffer = int_cfdata[5]; return true; }
	else if ((address_t) addr == XINT_REGS_ADDRESSES[INT_CFDATA6]) { *(uint8_t *) buffer = int_cfdata[6]; return true; }
	else if ((address_t) addr == XINT_REGS_ADDRESSES[INT_CFDATA7]) { *(uint8_t *) buffer = int_cfdata[7]; return true; }

	return false;
}

bool XINT::WriteMemory(service_address_t addr, const void *buffer, uint32_t size) {

	if (size == 0) {
		return true;
	}

	for (uint8_t i=0; i<XINT_MEMMAP_SIZE; i++) {
		if (XINT_REGS_ADDRESSES[i] == addr) {
			write(addr, *(uint8_t *) buffer);
			return true;
		}
	}

	return false;
}


} // end of namespace hcs12x
} // end of namespace processor
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

