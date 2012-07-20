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
	Object(name, parent)
	, sc_module(name)
	, MMC(name, parent)
	, unisim::kernel::service::Client<TrapReporting>(name, parent)
	, cpu_socket("cpu_to_mmc")
	, trap_reporting_import("trap_reporting_import", this)
{

	cpu_socket.register_b_transport(this, &S12XMMC::cpu_b_transport);
	xgate_socket.register_b_transport(this, &S12XMMC::xgate_b_transport);

	/* create initiator sockets */
	for (unsigned int i = 0; i < MEMORY_MAP_SIZE; i++)
	{
		std::stringstream init_socket_name_sstr;
		init_socket_name_sstr << "init_socket_" << i;
		init_socket[i] = new InitSocket(init_socket_name_sstr.str().c_str());
	}

	tlm2_btrans_time = sc_time((double)0, SC_PS);


	deviceMap[0].start_address = 0x0034; // CRG
	deviceMap[0].end_address = 0x003F;

	deviceMap[1].start_address = 0x0040;  // ECT
	deviceMap[1].end_address = 0x007F;

	deviceMap[2].start_address = 0x0080;  // ATD1
	deviceMap[2].end_address = 0x00AF;

	deviceMap[3].start_address = 0x0110;  // EEPROM
	deviceMap[3].end_address = 0x011B;

	deviceMap[4].start_address = 0x0120;  // XINT
	deviceMap[4].end_address = 0x012F;

	deviceMap[5].start_address = 0x02C0;  // ATD0
	deviceMap[5].end_address = 0x02DF;

	deviceMap[6].start_address = 0x0300;  // PWM
	deviceMap[6].end_address = 0x0327;

	deviceMap[7].start_address = 0x0380;  // XGATE
	deviceMap[7].end_address = 0x03BF;

	// ********** Physical memory Map ***

	memory_map[0].start_addr = 0x34;
	memory_map[0].end_addr = 0x3f;

	memory_map[1].start_addr = 0x40;
	memory_map[1].end_addr = 0x7f;

	memory_map[2].start_addr = 0x80;
	memory_map[2].end_addr = 0xAF;

	memory_map[3].start_addr = 0x110;
	memory_map[3].end_addr = 0x11B;

	memory_map[4].start_addr = 0x120;
	memory_map[4].end_addr = 0x12F;

	memory_map[5].start_addr = 0x2c0;
	memory_map[5].end_addr = 0x2df;

	memory_map[6].start_addr = 0x300;
	memory_map[6].end_addr = 0x327;

	memory_map[7].start_addr = 0x380;
	memory_map[7].end_addr = 0x3BF;

	memory_map[8].start_addr = 0x0f8000;
	memory_map[8].end_addr = 0x0fffff;

	memory_map[9].start_addr = 0x13f000;
	memory_map[9].end_addr = 0x13ffff;

	memory_map[10].start_addr = 0x780000;
	memory_map[10].end_addr = 0x7fffff;

}

S12XMMC::~S12XMMC() {

}

void S12XMMC::xgate_b_transport( tlm::tlm_generic_payload& trans, sc_time& delay ) {

	sc_dt::uint64 logicalAddress = trans.get_address();
	MMC_DATA *buffer = (MMC_DATA *) trans.get_data_ptr();
	tlm::tlm_command cmd = trans.get_command();


	bool find = false;
	for (int i=0; (i<MMC_MEMMAP_SIZE) && !find; i++) {
		find = (MMC_REGS_ADDRESSES[i] == logicalAddress);
	}

	if (find) {
		if (cmd == tlm::TLM_READ_COMMAND) {
			memset(buffer->buffer, 0, buffer->data_size);
			inherited::read(logicalAddress, buffer->buffer, buffer->data_size);
		} else {
			inherited::write(logicalAddress, buffer->buffer, buffer->data_size);
		}

	} else {

		find = true;
		if (logicalAddress <= REG_HIGH_OFFSET) {

			find = false;
			for (int i=0; (i<DEVICE_MAP_SIZE) && !find; i++) {
				find = ((deviceMap[i].start_address <= logicalAddress) && (logicalAddress <= deviceMap[i].end_address));
			}

			if (!find) {

				if (debug_enabled) {
					cerr << "WARNING: S12XMMC => Device at 0x" << std::hex << logicalAddress << " Not present in the emulated platform." << std::dec << std::endl;
				}
				memset(buffer->buffer, 0, buffer->data_size);
			}

		}

		if (find) {

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

			physical_address_t addr = inherited::getXGATEPhysicalAddress((address_t) logicalAddress);

//			cout << "XGATE::MMC cpu_addr=0x" << std::hex << (unsigned int) logicalAddress << "  phy_addr=0x" << std::hex << addr << endl;

			mmc_trans->set_address( addr & 0x7FFFFF);

			for (int i=0; i <MEMORY_MAP_SIZE; i++) {
				if ((memory_map[i].start_addr <= addr) && (memory_map[i].end_addr >= addr)) {
					(*init_socket[i])->b_transport( *mmc_trans, tlm2_btrans_time );
					break;
				}
			}

			if (mmc_trans->is_response_error() ) {
				cerr << "Access error to 0x" << std::hex << mmc_trans->get_address() << " size = " << mmc_trans->get_data_length() << std::dec << endl;
				SC_REPORT_ERROR("S12XMMC : ", "Response error from b_transport.");
			}


			mmc_trans->release();

		}

	}

	trans.set_response_status( tlm::TLM_OK_RESPONSE );

}

void S12XMMC::cpu_b_transport( tlm::tlm_generic_payload& trans, sc_time& delay ) {

	sc_dt::uint64 logicalAddress = trans.get_address();
	MMC_DATA *buffer = (MMC_DATA *) trans.get_data_ptr();
	tlm::tlm_command cmd = trans.get_command();


	bool find = false;
	for (int i=0; (i<MMC_MEMMAP_SIZE) && !find; i++) {
		find = (MMC_REGS_ADDRESSES[i] == logicalAddress);
	}

	if (find) {
		if (cmd == tlm::TLM_READ_COMMAND) {
			memset(buffer->buffer, 0, buffer->data_size);
			inherited::read(logicalAddress, buffer->buffer, buffer->data_size);
		} else {
			inherited::write(logicalAddress, buffer->buffer, buffer->data_size);
		}

	} else {

		find = true;
		if (logicalAddress <= REG_HIGH_OFFSET) {

			find = false;
			for (int i=0; (i<DEVICE_MAP_SIZE) && !find; i++) {
				find = ((deviceMap[i].start_address <= logicalAddress) && (logicalAddress <= deviceMap[i].end_address));
			}

			if (!find) {

				if (debug_enabled) {
					cerr << "WARNING: S12XMMC => Device at 0x" << std::hex << logicalAddress << " Not present in the emulated platform." << std::dec << std::endl;
				}
				memset(buffer->buffer, 0, buffer->data_size);
			}

		}

		if (find) {

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

			physical_address_t addr = inherited::getCPU12XPhysicalAddress((address_t) logicalAddress, buffer->type, buffer->isGlobal);

			mmc_trans->set_address( addr & 0x7FFFFF);

			for (int i=0; i <MEMORY_MAP_SIZE; i++) {
				if ((memory_map[i].start_addr <= addr) && (memory_map[i].end_addr >= addr)) {
					(*init_socket[i])->b_transport( *mmc_trans, tlm2_btrans_time );
					break;
				}
			}

			if (mmc_trans->is_response_error() ) {
				cerr << "Access error to 0x" << std::hex << mmc_trans->get_address() << " size = " << mmc_trans->get_data_length() << std::dec << endl;
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

