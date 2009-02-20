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

#include <unisim/component/tlm2/processor/hcs12x/s12xmmc.hh>
#include <unisim/component/tlm2/processor/hcs12x/tlm_types.hh>

namespace unisim {
namespace component {
namespace tlm2 {
namespace processor {
namespace hcs12x {

S12XMMC::S12XMMC(const sc_module_name& name, Object *parent) :
	Object(name, parent),
	sc_module(name),
	MMC(name, parent)

{

	reset();

	cpu_socket.register_b_transport(this, &S12XMMC::b_transport);

	tlm2_btrans_time = sc_time((double)0, SC_PS);

/*
	SC_HAS_PROCESS(S12XMMC);
	sensitive << cpu_socket;

	SC_THREAD(Run);
*/
}

S12XMMC::~S12XMMC() {

}

void S12XMMC::reset() {
	inherited::Reset();
}

void S12XMMC::Run() {
	while (true) {
		wait();
	}
}

void S12XMMC::b_transport( tlm::tlm_generic_payload& trans, sc_time& delay ) {

	sc_dt::uint64 logicalAddress = trans.get_address();
	MMC_DATA *buffer = (MMC_DATA *) trans.get_data_ptr();
	tlm::tlm_command cmd = trans.get_command();

	// MMC Registers. To speed-up simulation.
	if ((logicalAddress >= MMC_LOW_ADDRESS) && (logicalAddress <= MMC_HIGH_ADDRESS)) {
		if (cmd == tlm::TLM_READ_COMMAND) {
			*((uint8_t *) buffer->buffer) = inherited::read(logicalAddress);
		} else {
			inherited::write(logicalAddress, *((uint8_t *) buffer->buffer));
		}

	} else {

		tlm::tlm_generic_payload* mmc_trans = payloadFabric.allocate();

		mmc_trans->set_data_ptr( (unsigned char *)buffer->buffer );

		mmc_trans->set_data_length( buffer->data_size );
		mmc_trans->set_streaming_width( buffer->data_size );

		mmc_trans->set_byte_enable_ptr( 0 );
		mmc_trans->set_dmi_allowed( false );
		mmc_trans->set_response_status( tlm::TLM_INCOMPLETE_RESPONSE );

		if (cmd == tlm::TLM_READ_COMMAND) {
			mmc_trans->set_command( tlm::TLM_READ_COMMAND );
		} else {
			mmc_trans->set_command( tlm::TLM_WRITE_COMMAND );
		}

		if (isPaged(logicalAddress, 0x00, false)) {
			physical_address_t addr = inherited::getPhysicalAddress((address_t) logicalAddress, buffer->type, buffer->isGlobal);
			mmc_trans->set_address( addr );
			external_socket->b_transport( *mmc_trans, tlm2_btrans_time );
		} else {
			mmc_trans->set_address( logicalAddress );
			local_socket->b_transport( *mmc_trans, tlm2_btrans_time );
		}

		if (mmc_trans->is_response_error() ) {
			cerr << "Access error to 0x" << std::hex << mmc_trans->get_address() << std::dec << endl;
			SC_REPORT_ERROR("S12XMMC : ", "Response error from b_transport.");
		}


		mmc_trans->release();

	}

	trans.set_response_status( tlm::TLM_OK_RESPONSE );
}


} // end of namespace hcs12x
} // end of namespace processor
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

