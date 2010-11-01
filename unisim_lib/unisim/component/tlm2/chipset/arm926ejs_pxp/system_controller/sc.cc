/*
 *  Copyright (c) 2010,
 *  Commissariat a l'Energie Atomique (CEA)
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without 
 *  modification, are permitted provided that the following conditions are met:
 *
 *   - Redistributions of source code must retain the above copyright notice, 
 *     this list of conditions and the following disclaimer.
 *
 *   - Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 *   - Neither the name of CEA nor the names of its contributors may be used to
 *     endorse or promote products derived from this software without specific 
 *     prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED.
 *  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY
 *  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF 
 *  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 */
 
#define SC_INCLUDE_DYNAMIC_PROCESSES
#include "unisim/component/tlm2/chipset/arm926ejs_pxp/system_controller/sc.hh"
#include "unisim/kernel/tlm2/tlm.hh"
#include "unisim/util/endian/endian.hh"
#include <inttypes.h>
#include <assert.h>

namespace unisim {
namespace component {
namespace tlm2 {
namespace chipset {
namespace arm926ejs_pxp {
namespace system_controller {

using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugError;
using unisim::util::endian::Host2LittleEndian;
using unisim::util::endian::LittleEndian2Host;

SystemController ::
SystemController(const sc_module_name &name, Object *parent)
	: unisim::kernel::service::Object(name, parent)
	, sc_module(name)
	, bus_target_socket("bus_target_socket")
	, base_addr(0)
	, param_base_addr("base-addr", this, base_addr,
			"Base address of the system controller.")
	, refclk(31250000)
	, param_refclk("refclk", this, refclk,
			"Reference clock (REFCLK) period in picoseconds.")
	, timclk(1000000)
	, param_timclk("timclk", this, timclk,
			"Timer module clock (TIMCLK) period in picoseconds.")
	, logger(*this)
{
	bus_target_socket.register_nb_transport_fw(this,
			&SystemController::bus_target_nb_transport_fw);
	bus_target_socket.register_b_transport(this,
			&SystemController::bus_target_b_transport);
	bus_target_socket.register_get_direct_mem_ptr(this,
			&SystemController::bus_target_get_direct_mem_ptr);
	bus_target_socket.register_transport_dbg(this,
			&SystemController::bus_target_transport_dbg);

	// init the registers values
	memset(regs, 0, 256);
	uint32_t val = Host2LittleEndian((uint32_t)0x09);
	memcpy(&regs[0], &val, sizeof(val));
}

SystemController ::
~SystemController()
{
}

bool 
SystemController::
Setup()
{
	refclk_out_port = refclk;
	timclken0_out_port = refclk;
	return true;
}

/**************************************************************************/
/* Virtual methods for the target socket for the bus connection     START */
/**************************************************************************/

tlm::tlm_sync_enum
SystemController ::
bus_target_nb_transport_fw(transaction_type &trans,
		phase_type &phase,
		sc_core::sc_time &time)
{
	assert("TODO" == 0);
	return tlm::TLM_COMPLETED;
}

void 
SystemController ::
bus_target_b_transport(transaction_type &trans, 
		sc_core::sc_time &delay)
{
	uint32_t addr = trans.get_address() - base_addr;
	uint8_t *data = trans.get_data_ptr();
	uint32_t size = trans.get_data_length();
	bool is_read = trans.is_read();
	bool handled = false;

	logger << DebugError
		<< (is_read ? "Reading" : "Writing") << " 0x" 
		<< std::hex << addr
		<< " (0x"
		<< trans.get_address() << std::dec
		<< ") of " << size << " bits"
		<< EndDebugError;

	if ( is_read )
	{
		handled = true;
		memcpy(data, &regs[addr], size);
		trans.set_response_status(tlm::TLM_OK_RESPONSE);
	}
	else // writing
	{
		uint32_t prev_value, new_value;
		memcpy(&prev_value, &regs[addr & ~(uint32_t)0x03], 4);
		memcpy(&regs[addr], data, size);
		memcpy(&new_value, &regs[addr & ~(uint32_t)0x03], 4);
		prev_value = LittleEndian2Host(prev_value);
		new_value = LittleEndian2Host(new_value);
		if ( (addr & ~(uint32_t)0x03) == 0x0 )
		{
			handled = true;
			// writing into SCCTRL
			uint32_t unmod_value = new_value;
			new_value = (new_value & ~0xff000ef8UL) | 
				(prev_value & 0xff000ef8UL);
			logger << DebugError
				<< "Writing 0x"
				<< std::hex << new_value
				<< " (previous value 0x"
				<< prev_value
				<< ", received 0x"
				<< unmod_value << std::dec
				<< ")"
				<< EndDebugError;
			// update timers if necessary
			bool timeren3sel = false;
			bool timeren2sel = false;
			bool timeren1sel = false;
			bool timeren0sel = false;
			if ( new_value & (((uint32_t)1) << 21) ) timeren3sel = true;
			if ( new_value & (((uint32_t)1) << 19) ) timeren2sel = true;
			if ( new_value & (((uint32_t)1) << 17) ) timeren1sel = true;
			if ( new_value & (((uint32_t)1) << 15) ) timeren0sel = true;
			// TODO:
			// if ( timeren3sel ) timclken3_out_port = timclk;
			// else timclken3_out_port = refclk;
			// if ( timeren2sel ) timclken2_out_port = timclk;
			// else timclken2_out_port = refclk;
			// END TODO
			if ( timeren1sel ) timclken1_out_port = timclk;
			else timclken1_out_port = refclk;
			if ( timeren0sel ) timclken0_out_port = timclk;
			else timclken0_out_port = refclk;
			logger << DebugError
				<< "timeren0sel = " << timeren0sel << std::endl
				<< "timeren1sel = " << timeren1sel << std::endl
				<< "timeren2sel = " << timeren2sel << std::endl
				<< "timeren3sel = " << timeren3sel << std::endl
				<< EndDebugError;
			// store the new value in the register file
			new_value = Host2LittleEndian(new_value);
			memcpy(&regs[addr & ~(uint32_t)0x03], &new_value, 4);
			trans.set_response_status(tlm::TLM_OK_RESPONSE);
		}
		else
		{
			logger << DebugError
				<< "Writing 0x"
				<< std::hex << new_value
				<< " (previous value 0x"
				<< prev_value << std::dec
				<< ")"
				<< EndDebugError;
		}
	}
	if ( !handled )
		assert("TODO" == 0);
}

bool 
SystemController ::
bus_target_get_direct_mem_ptr(transaction_type &trans, 
		tlm::tlm_dmi &dmi_data)
{
	assert("TODO" == 0);
	return false;
}

unsigned int 
SystemController ::
bus_target_transport_dbg(transaction_type &trans)
{
	assert("TODO" == 0);
	return 0;
}

/**************************************************************************/
/* Virtual methods for the target socket for the bus connection       END */
/**************************************************************************/

} // end of namespace system_controller 
} // end of namespace arm926ejs_pxp
} // end of namespace chipset
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim


