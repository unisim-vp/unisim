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
#include "unisim/component/tlm2/chipset/arm926ejs_pxp/vic/vic.hh"
#include "unisim/kernel/tlm2/tlm.hh"
#include "unisim/util/endian/endian.hh"
#include <inttypes.h>
#include <assert.h>

namespace unisim {
namespace component {
namespace tlm2 {
namespace chipset {
namespace arm926ejs_pxp {
namespace vic {

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugError;
using unisim::util::endian::Host2LittleEndian;
using unisim::util::endian::LittleEndian2Host;

VIC ::
VIC(const sc_module_name &name, Object *parent)
	: unisim::kernel::service::Object(name, parent)
	, sc_module(name)
	, bus_target_socket("bus_target_socket")
	, base_addr(0)
	, param_base_addr("base-addr", this, base_addr,
			"Base address of the system controller.")
	, logger(*this)
{
	bus_target_socket.register_nb_transport_fw(this,
			&VIC::bus_target_nb_transport_fw);
	bus_target_socket.register_b_transport(this,
			&VIC::bus_target_b_transport);
	bus_target_socket.register_get_direct_mem_ptr(this,
			&VIC::bus_target_get_direct_mem_ptr);
	bus_target_socket.register_transport_dbg(this,
			&VIC::bus_target_transport_dbg);

	// init the registers values
	memset(regs, 0, 0x01000UL);
	regs[0x0fe0UL] = 0x090;
	regs[0x0fe4UL] = 0x011;
	regs[0x0fe8UL] = 0x04;
	regs[0x0fecUL] = 0;
	regs[0x0ff0UL] = 0x0d;
	regs[0x0ff4UL] = 0x0f0;
	regs[0x0ff8UL] = 0x05;
	regs[0x0ffcUL] = 0x0b1;
}

VIC ::
~VIC()
{
}

bool 
VIC ::
Setup()
{
	return true;
}

/**************************************************************************/
/* Virtual methods for the target socket for the bus connection     START */
/**************************************************************************/

tlm::tlm_sync_enum
VIC ::
bus_target_nb_transport_fw(transaction_type &trans,
		phase_type &phase,
		sc_core::sc_time &time)
{
	unisim::kernel::service::Simulator::simulator->Stop(this, __LINE__);
	return tlm::TLM_COMPLETED;
}

void 
VIC ::
bus_target_b_transport(transaction_type &trans, 
		sc_core::sc_time &delay)
{
	uint32_t addr = trans.get_address() - base_addr;
	uint8_t *data = trans.get_data_ptr();
	uint32_t size = trans.get_data_length();
	bool is_read = trans.is_read();
	bool handled = false;

	if ( is_read )
	{
	}
	else // writing
	{
	}
	if ( !handled )
	{
		logger << DebugError
			<< "Access to VIC:" << std::endl
			<< " - read = " << is_read << std::endl
			<< " - addr = 0x" << std::hex << addr << std::dec << std::endl
			<< " - size = " << size;
		if ( is_read )
		{
			logger << std::endl
				<< " - data =" << std::hex;
			for ( int i = 0; i < size; i++ )
				logger << " " << (unsigned int)data[i];
			logger << std::dec;
		}
		logger << EndDebugError;
		unisim::kernel::service::Simulator::simulator->Stop(this, __LINE__);
	}
}

bool 
VIC ::
bus_target_get_direct_mem_ptr(transaction_type &trans, 
		tlm::tlm_dmi &dmi_data)
{
	unisim::kernel::service::Simulator::simulator->Stop(this, __LINE__);
	return false;
}

unsigned int 
VIC ::
bus_target_transport_dbg(transaction_type &trans)
{
	unisim::kernel::service::Simulator::simulator->Stop(this, __LINE__);
	return 0;
}

/**************************************************************************/
/* Virtual methods for the target socket for the bus connection       END */
/**************************************************************************/

} // end of namespace vic
} // end of namespace arm926ejs_pxp
} // end of namespace chipset
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim


