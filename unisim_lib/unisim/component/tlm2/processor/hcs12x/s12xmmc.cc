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
	MMC(name, parent),
	Client<TrapReporting>(name, parent),
	trap_reporting_import("trap_reproting_import", this)


{

	reset();

	cpu_socket.register_b_transport(this, &S12XMMC::b_transport);

	tlm2_btrans_time = sc_time((double)0, SC_PS);

	MMC_REGS_ADDRESSES[0] = 0x000A;
	MMC_REGS_ADDRESSES[1] = 0x000B;
	MMC_REGS_ADDRESSES[2] = 0x0010;
	MMC_REGS_ADDRESSES[3] = 0x0011;
	MMC_REGS_ADDRESSES[4] = 0x0013;
	MMC_REGS_ADDRESSES[5] = 0x0016;
	MMC_REGS_ADDRESSES[6] = 0x0017;
	MMC_REGS_ADDRESSES[7] = 0x0030;
	MMC_REGS_ADDRESSES[8] = 0x011C;
	MMC_REGS_ADDRESSES[9] = 0x011D;
	MMC_REGS_ADDRESSES[10] = 0x011E;
	MMC_REGS_ADDRESSES[11] = 0x011F;


	deviceMap[0].start_address = 0x0080;
	deviceMap[0].end_address = 0x00AF;

	deviceMap[1].start_address = 0x0120;
	deviceMap[1].end_address = 0x012F;
/*
	deviceMap[2].start_address = 0x02C0;
	deviceMap[2].end_address = 0x02DF;
*/
	deviceMap[2].start_address = 0x0300;
	deviceMap[2].end_address = 0x0327;

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


	/**
	 *  TODO:
	 *   - remove internal router
	 *   - use a Device Register Memory Map to identify the read/write target
	 */


	bool find = false;
	for (int i=0; (i<MMC_SIZE) && !find; i++) {
		find = (MMC_REGS_ADDRESSES[i] == logicalAddress);
	}

	if (find) {
		if (cmd == tlm::TLM_READ_COMMAND) {
			*((uint8_t *) buffer->buffer) = inherited::read(logicalAddress);
		} else {
			inherited::write(logicalAddress, *((uint8_t *) buffer->buffer));
		}

	} else {

// Start => Workaround code for unimplemented controller
		find = true;
		if (logicalAddress <= REG_HIGH_OFFSET) {

			find = false;
			for (int i=0; (i<DEVICE_MAP_SIZE) && !find; i++) {
				find = ((deviceMap[i].start_address <= logicalAddress) && (logicalAddress <= deviceMap[i].end_address));
			}

			if (!find) {

				if (CONFIG::DEBUG_ENABLE && trap_reporting_import) {
					trap_reporting_import->ReportTrap();
				}

				cerr << "WARNING: S12XMMC => Device at 0x" << std::hex << logicalAddress << " Not present in the emulated platform." << std::dec << std::endl;
			}

		}

		if (find) {
// End => Workaround code for unimplemented controller

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

	}

	trans.set_response_status( tlm::TLM_OK_RESPONSE );
}


} // end of namespace hcs12x
} // end of namespace processor
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

