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

XINT::XINT(const sc_module_name& name, Object *parent) {

	fromCPU_Target.register_b_transport(this, &XINT::b_transport);
	slave_socket.register_b_transport(this, &XINT::read_write);

	SC_HAS_PROCESS(XINT);

	SC_THREAD(Run);
}

XINT::~XINT() {

}

void XINT::b_transport( tlm::tlm_generic_payload& trans, sc_time& delay )
{
	// This method is called to compute the interrupt vector based on currentIPL and issued interrupt (Hw/Sw)

	INT_TRANS_T *buffer = (INT_TRANS_T *) trans.get_data_ptr();

	uint8_t currentIPL = buffer->ipl;

	trans.set_response_status( tlm::TLM_OK_RESPONSE );

/*
	uint8_t int_cfdata[8];
	uint8_t cpuPriority = 0, xgatePriority = 0;
	uint8_t cpuIndex = 0, xgateIndex = 0;

	getCFDATA(int_cfdata);

	for (uint8_t i=0; i<8; i++) {

		uint8_t dataPriority = int_cfdata[i] && 0x07;

		// if 7-bit=0 then cpu else xgate
		if ((int_cfdata[i] & 0x80) == 0)
		{
			if (dataPriority > cpuPriority) {
				cpuPriority = dataPriority;
				cpuIndex = i;
			}
		}
		else
		{
			if (dataPriority > xgatePriority) {
				xgatePriority = dataPriority;
				xgateIndex = i;
			}
		}
	}
*/

}

void XINT::Run()
{
	// This thread is waked-up by any hardware interrupt

	/* TODO:
	 *  Check which interrupt if (reset) setIVBR(0xFF)
	 */
	tlm::tlm_generic_payload* trans = new tlm::tlm_generic_payload;
	sc_time delay = sc_time(1, SC_NS);

	trans->set_response_status( tlm::TLM_INCOMPLETE_RESPONSE );

	toCPU_Initiator->b_transport( *trans, delay );

	if (trans->is_response_error() )
		SC_REPORT_ERROR("TLM-2", "Response error from b_transport");

	wait();
}


address_t XINT::getIntVector(uint8_t index) {

	// TODO:

	return 0;
}

void XINT::getCFDATA(uint8_t data[])
{

	if (int_cfaddr == 0) {
		for (uint8_t i=0; i<8; i++) {
			data[i] = 0;
		}
	} else {
		for (uint8_t i=0; i<8; i++) {
			data[i] = int_cfdata[i];
		}
	}
}

void XINT::reset() {

	write(CONFIG::IVBR_ADDRESS, CONFIG::IVBR_RESET_VALUE);
	write(CONFIG::INT_XGPRIO, CONFIG::INT_XGPRIO_RESET_VALUE);
	write(CONFIG::INT_CFADDR, CONFIG::INT_CFADDR_RESET_VALUE);
	write(CONFIG::INT_CFDATA0, CONFIG::INT_CFDATA0_RESET_VALUE);
	write(CONFIG::INT_CFDATA1, CONFIG::INT_CFDATA1_RESET_VALUE);
	write(CONFIG::INT_CFDATA2, CONFIG::INT_CFDATA2_RESET_VALUE);
	write(CONFIG::INT_CFDATA3, CONFIG::INT_CFDATA3_RESET_VALUE);
	write(CONFIG::INT_CFDATA4, CONFIG::INT_CFDATA4_RESET_VALUE);
	write(CONFIG::INT_CFDATA5, CONFIG::INT_CFDATA5_RESET_VALUE);
	write(CONFIG::INT_CFDATA6, CONFIG::INT_CFDATA6_RESET_VALUE);
	write(CONFIG::INT_CFDATA7, CONFIG::INT_CFDATA7_RESET_VALUE);
}

void XINT::read_write( tlm::tlm_generic_payload& trans, sc_time& delay )
{
	tlm::tlm_command cmd = trans.get_command();
	sc_dt::uint64 address = trans.get_address();
	uint8_t* data_ptr = (uint8_t *)trans.get_data_ptr();
	unsigned int len = trans.get_data_length();
	unsigned char* byt = trans.get_byte_enable_ptr();
	unsigned int wid = trans.get_streaming_width();

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
		case CONFIG::IVBR_ADDRESS: setIVBR(value); break;
		case CONFIG::INT_XGPRIO: setINT_XGPRIO(value); break;
		case CONFIG::INT_CFADDR: setINT_CFADDR(value); break;
		case CONFIG::INT_CFDATA0: setINT_CFDATA(0, value); break;
		case CONFIG::INT_CFDATA1: setINT_CFDATA(1, value); break;
		case CONFIG::INT_CFDATA2: setINT_CFDATA(2, value); break;
		case CONFIG::INT_CFDATA3: setINT_CFDATA(3, value); break;
		case CONFIG::INT_CFDATA4: setINT_CFDATA(4, value); break;
		case CONFIG::INT_CFDATA5: setINT_CFDATA(5, value); break;
		case CONFIG::INT_CFDATA6: setINT_CFDATA(6, value); break;
		case CONFIG::INT_CFDATA7: setINT_CFDATA(7, value); break;
		default: ;
	}
}

void XINT::read(address_t address, uint8_t &value)
{
	switch (address)
	{
		case CONFIG::IVBR_ADDRESS: value = getIVBR();
		case CONFIG::INT_XGPRIO: value = getINT_XGPRIO();;
		case CONFIG::INT_CFADDR: value = getINT_CFADDR();
		case CONFIG::INT_CFDATA0: value = getINT_CFDATA(0);
		case CONFIG::INT_CFDATA1: value = getINT_CFDATA(1);
		case CONFIG::INT_CFDATA2: value = getINT_CFDATA(2);
		case CONFIG::INT_CFDATA3: value = getINT_CFDATA(3);
		case CONFIG::INT_CFDATA4: value = getINT_CFDATA(4);
		case CONFIG::INT_CFDATA5: value = getINT_CFDATA(5);
		case CONFIG::INT_CFDATA6: value = getINT_CFDATA(6);
		case CONFIG::INT_CFDATA7: value = getINT_CFDATA(7);
		default: value = 0;
	}
}

uint8_t XINT::getIVBR() { return ivbr; }
void XINT::setIVBR(uint8_t value) { ivbr = value; }

uint8_t	XINT::getINT_XGPRIO() { return int_xgprio; }
void XINT::setINT_XGPRIO(uint8_t value) { int_xgprio = value; }

uint8_t	XINT::getINT_CFADDR() { return int_cfaddr; }
void XINT::setINT_CFADDR(uint8_t value) { int_cfaddr = value; }

uint8_t	XINT::getINT_CFDATA(uint8_t index) { assert(index < 8); return int_cfdata[index]; }
void XINT::setINT_CFDATA(uint8_t index, uint8_t value) { assert(index < 8); int_cfdata[index] = value; }

} // end of namespace hcs12x
} // end of namespace processor
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

