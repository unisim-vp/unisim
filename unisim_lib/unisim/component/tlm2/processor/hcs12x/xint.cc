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

namespace unisim {
namespace component {
namespace tlm2 {
namespace processor {
namespace hcs12x {

XINT::XINT(const sc_module_name& name, Object *parent) :
	sc_module(name),
	isHardwareInterrupt(false)
{

	reset();

	fromCPU_Target.register_b_transport(this, &XINT::b_transport);
	slave_socket.register_b_transport(this, &XINT::read_write);

	SC_HAS_PROCESS(XINT);

	SC_THREAD(Run);
	sensitive << interrupt_request;
}

XINT::~XINT() {

}

/*
 *    This method is called to compute the interrupt vector based on currentIPL
 */
void XINT::b_transport( tlm::tlm_generic_payload& trans, sc_time& delay )
{

	// The CPU has taken in account the external interrupt request => disactivate the interrupt request
	toCPU_Initiator = false;

	INT_TRANS_T *buffer = (INT_TRANS_T *) trans.get_data_ptr();

	uint8_t newIPL = 0;
	address_t vectorAddress = 0;
	uint8_t cfaddr = getINT_CFADDR();


	/*
	 * Check if it is reset
	 */
	if (interrupt_request[XINT::INT_CLK_MONITOR_RESET_OFFSET/2]->read())
		vectorAddress = get_ClockMonitorReset_Vector();
	else if (interrupt_request[XINT::INT_COP_WATCHDOG_RESET_OFFSET/2]->read())
		vectorAddress = get_COPWatchdogReset_Vector();
	else if (interrupt_request[XINT::INT_ILLEGAL_ACCESS_RESET_OFFSET/2]->read())
		vectorAddress = get_IllegalAccessReset_Vector();
	else if (interrupt_request[XINT::INT_SYS_RESET_OFFSET/2]->read())
		vectorAddress = get_SysReset_Vector();
	else if (interrupt_request[XINT::INT_XIRQ_OFFSET/2]->read())
		vectorAddress = get_XIRQ_Vector();
	else
		for (int index=cfaddr; index < cfaddr+8; index++) { // Check only the selected interrupts
			if (interrupt_request[index]->read()) {

				uint8_t dataPriority = int_cfdata[index] && 0x07;

				// if 7-bit=0 then cpu else xgate
				if ((int_cfdata[index] & 0x80) == 0)
				{
					if (dataPriority > newIPL) {
						newIPL = dataPriority;
						vectorAddress = ((address_t) getIVBR() << 8) + cfaddr + index;
					}
				}
				else ;

			}
		}

	if (vectorAddress == 0) {
		if (isHardwareInterrupt) {
			vectorAddress = get_Spurious_Vector();
			newIPL = 0x7;
		} else {
			vectorAddress = (address_t) getIVBR() << 8 ;
		}
	}

	// The comparaison of the newIPL to the currentIPL is done by the CPU during I-bit-interrupt handling
	buffer->ipl = newIPL;
	buffer->vectorAddress = vectorAddress;

	isHardwareInterrupt = false;
	trans.set_response_status( tlm::TLM_OK_RESPONSE );

}

void XINT::Run()
{
	// This thread is waked-up by any not CPU interrupt

	while (true) {
		wait();

		/*
		 *  Check which interrupt if (reset) setIVBR(0xFF)
		 */

		if (interrupt_request[XINT::INT_CLK_MONITOR_RESET_OFFSET/2]->read() ||
				interrupt_request[XINT::INT_COP_WATCHDOG_RESET_OFFSET/2]->read() ||
				interrupt_request[XINT::INT_ILLEGAL_ACCESS_RESET_OFFSET/2]->read() ||
				interrupt_request[XINT::INT_SYS_RESET_OFFSET/2]->read() )
		{
			setIVBR(0xFF);
		}

		isHardwareInterrupt = true;
		toCPU_Initiator = true;
	}

}


address_t XINT::getIntVector(uint8_t index) {

	// TODO:

	return 0;
}

void XINT::reset() {

	write(IVBR_ADDRESS, IVBR_RESET_VALUE);
	write(INT_XGPRIO, INT_XGPRIO_RESET_VALUE);
	write(INT_CFADDR, INT_CFADDR_RESET_VALUE);
	write(INT_CFDATA0, INT_CFDATA0_RESET_VALUE);
	write(INT_CFDATA1, INT_CFDATA1_RESET_VALUE);
	write(INT_CFDATA2, INT_CFDATA2_RESET_VALUE);
	write(INT_CFDATA3, INT_CFDATA3_RESET_VALUE);
	write(INT_CFDATA4, INT_CFDATA4_RESET_VALUE);
	write(INT_CFDATA5, INT_CFDATA5_RESET_VALUE);
	write(INT_CFDATA6, INT_CFDATA6_RESET_VALUE);
	write(INT_CFDATA7, INT_CFDATA7_RESET_VALUE);
}

void XINT::read_write( tlm::tlm_generic_payload& trans, sc_time& delay )
{
	tlm::tlm_command cmd = trans.get_command();
	sc_dt::uint64 address = trans.get_address();
	uint8_t* data_ptr = (uint8_t *)trans.get_data_ptr();

	if (cmd == tlm::TLM_READ_COMMAND) {
		read((address_t) address, *data_ptr);
	} else if (cmd == tlm::TLM_WRITE_COMMAND) {
		write((address_t) address, *data_ptr);
	}

	trans.set_response_status( tlm::TLM_OK_RESPONSE );
}

void XINT::write(address_t address, uint8_t value)
{
	switch (address)
	{
		case IVBR_ADDRESS: setIVBR(value); break;
		case INT_XGPRIO: setINT_XGPRIO(value); break;
		case INT_CFADDR: setINT_CFADDR(value); break;
		case INT_CFDATA0: setINT_CFDATA(0, value); break;
		case INT_CFDATA1: setINT_CFDATA(1, value); break;
		case INT_CFDATA2: setINT_CFDATA(2, value); break;
		case INT_CFDATA3: setINT_CFDATA(3, value); break;
		case INT_CFDATA4: setINT_CFDATA(4, value); break;
		case INT_CFDATA5: setINT_CFDATA(5, value); break;
		case INT_CFDATA6: setINT_CFDATA(6, value); break;
		case INT_CFDATA7: setINT_CFDATA(7, value); break;
		default: ;
	}
}

void XINT::read(address_t address, uint8_t &value)
{
	switch (address)
	{
		case IVBR_ADDRESS: value = getIVBR();
		case INT_XGPRIO: value = getINT_XGPRIO();;
		case INT_CFADDR: value = getINT_CFADDR();
		case INT_CFDATA0: value = getINT_CFDATA(0);
		case INT_CFDATA1: value = getINT_CFDATA(1);
		case INT_CFDATA2: value = getINT_CFDATA(2);
		case INT_CFDATA3: value = getINT_CFDATA(3);
		case INT_CFDATA4: value = getINT_CFDATA(4);
		case INT_CFDATA5: value = getINT_CFDATA(5);
		case INT_CFDATA6: value = getINT_CFDATA(6);
		case INT_CFDATA7: value = getINT_CFDATA(7);
		default: value = 0;
	}
}

uint8_t XINT::getIVBR() { return ivbr; }
void XINT::setIVBR(uint8_t value) { ivbr = value; }

uint8_t	XINT::getINT_XGPRIO() { return int_xgprio; }
void XINT::setINT_XGPRIO(uint8_t value) { int_xgprio = value; }

uint8_t	XINT::getINT_CFADDR() { return int_cfaddr; }
void XINT::setINT_CFADDR(uint8_t value) { int_cfaddr = value; }

uint8_t	XINT::getINT_CFDATA(uint8_t index)
{
	assert(index < 8);

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

void XINT::setINT_CFDATA(uint8_t index, uint8_t value)
{
	assert(index < 8);

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

} // end of namespace hcs12x
} // end of namespace processor
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

