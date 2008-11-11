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

#include <unisim/component/tlm2/processor/hcs12x/xint.hh>
#include <unisim/component/tlm2/processor/hcs12x/tlm_types.hh>

namespace unisim {
namespace component {
namespace tlm2 {
namespace processor {
namespace hcs12x {

XINT::XINT(const sc_module_name& name, Object *parent) {

	SC_HAS_PROCESS(XINT);

	SC_THREAD(Run);
}

XINT::~XINT() {

}

void XINT::Run() {

	tlm::tlm_generic_payload* trans = new tlm::tlm_generic_payload;
	sc_time delay = sc_time(SC_ZERO, SC_NS);

	XINT_READ_CMD_T buffer;

	while (1) {
		// Get from CPU the current IPL
		trans->set_command( tlm::TLM_READ_COMMAND );
		trans->set_address( 0 );
		trans->set_data_ptr( (unsigned char *) &buffer );

		trans->set_data_length(sizeof(XINT_READ_CMD_T));
		trans->set_streaming_width(sizeof(XINT_READ_CMD_T));

		trans->set_byte_enable_ptr( 0 );
		trans->set_dmi_allowed( false );
		trans->set_response_status( tlm::TLM_INCOMPLETE_RESPONSE );

		toCPU_Initiator->b_transport( *trans, delay );

		wait(sc_time(rand()%100,SC_NS));
	}
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


address_t XINT::getIntVector(uint8_t index) {

	// TODO:

	return 0;
}

void XINT::getCFDATA(uint8_t data[]) {

	uint8_t int_cfaddr = registers->read(CONFIG::INT_CFADDR);

	if (int_cfaddr == 0) {
		for (uint8_t i=0; i<8; i++) {
			data[i] = 0;
		}
	} else {
		for (uint8_t i=0; i<8; i++) {
			data[i] = registers->read(CONFIG::INT_CFDATA0 + i);
		}
	}
}

void XINT::reset() {

	registers->write(CONFIG::IVBR_ADDRESS, CONFIG::IVBR_RESET_VALUE);
	registers->write(CONFIG::INT_XGPRIO, CONFIG::INT_XGPRIO_RESET_VALUE);
	registers->write(CONFIG::INT_CFADDR, CONFIG::INT_CFADDR_RESET_VALUE);
	registers->write(CONFIG::INT_CFDATA0, CONFIG::INT_CFDATA0_RESET_VALUE);
	registers->write(CONFIG::INT_CFDATA1, CONFIG::INT_CFDATA1_RESET_VALUE);
	registers->write(CONFIG::INT_CFDATA2, CONFIG::INT_CFDATA2_RESET_VALUE);
	registers->write(CONFIG::INT_CFDATA3, CONFIG::INT_CFDATA3_RESET_VALUE);
	registers->write(CONFIG::INT_CFDATA4, CONFIG::INT_CFDATA4_RESET_VALUE);
	registers->write(CONFIG::INT_CFDATA5, CONFIG::INT_CFDATA5_RESET_VALUE);
	registers->write(CONFIG::INT_CFDATA6, CONFIG::INT_CFDATA6_RESET_VALUE);
	registers->write(CONFIG::INT_CFDATA7, CONFIG::INT_CFDATA7_RESET_VALUE);

}

} // end of namespace hcs12x
} // end of namespace processor
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

