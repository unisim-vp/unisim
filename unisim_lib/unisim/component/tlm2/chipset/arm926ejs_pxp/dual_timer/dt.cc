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
#include "unisim/component/tlm2/chipset/arm926ejs_pxp/dual_timer/dt.hh"
#include "unisim/kernel/tlm2/tlm.hh"
#include "unisim/util/endian/endian.hh"
#include <inttypes.h>
#include <assert.h>

namespace unisim {
namespace component {
namespace tlm2 {
namespace chipset {
namespace arm926ejs_pxp {
namespace dual_timer {

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugError;
using unisim::util::endian::Host2LittleEndian;
using unisim::util::endian::LittleEndian2Host;

DualTimer ::
DualTimer(const sc_module_name &name, Object *parent)
	: unisim::kernel::service::Object(name, parent)
	, sc_module(name)
	, bus_target_socket("bus_target_socket")
	, t1_update_time()
	, t2_update_time()
	, base_addr(0)
	, param_base_addr("base-addr", this, base_addr,
			"Base address of the system controller.")
	, logger(*this)
{
	bus_target_socket.register_nb_transport_fw(this,
			&DualTimer::bus_target_nb_transport_fw);
	bus_target_socket.register_b_transport(this,
			&DualTimer::bus_target_b_transport);
	bus_target_socket.register_get_direct_mem_ptr(this,
			&DualTimer::bus_target_get_direct_mem_ptr);
	bus_target_socket.register_transport_dbg(this,
			&DualTimer::bus_target_transport_dbg);

	// init the registers values
	memset(regs, 0, 256);
	uint32_t val = Host2LittleEndian((uint32_t)0xffffffffUL);
	memcpy(&regs[0x04], &val, 4);
	regs[0x08] = (uint8_t)0x20;
	memcpy(&regs[0x24], &val, 4);
	regs[0x28] = (uint8_t)0x20;
	regs[0xfe0] = (uint8_t)0x04;
	regs[0xfe4] = (uint8_t)0x18;
	regs[0xfe8] = (uint8_t)0x14;
	regs[0xff0] = (uint8_t)0x0d;
	regs[0xff4] = (uint8_t)0xf0;
	regs[0xff8] = (uint8_t)0x05;
	regs[0xffc] = (uint8_t)0xb1;
}

DualTimer ::
~DualTimer()
{
}

bool 
DualTimer::
Setup()
{
	return true;
}

/**************************************************************************/
/* Virtual methods for the target socket for the bus connection     START */
/**************************************************************************/

tlm::tlm_sync_enum
DualTimer ::
bus_target_nb_transport_fw(transaction_type &trans,
		phase_type &phase,
		sc_core::sc_time &time)
{
	assert("TODO" == 0);
	return tlm::TLM_COMPLETED;
}

void 
DualTimer ::
bus_target_b_transport(transaction_type &trans, 
		sc_core::sc_time &delay)
{
	uint32_t addr = trans.get_address() - base_addr;
	uint8_t *data = trans.get_data_ptr();
	uint32_t size = trans.get_data_length();
	bool is_read = trans.is_read();
	bool handled = false;

	// logger << DebugInfo
	//	<< (is_read ? "Reading" : "Writing") << " 0x" 
	//	<< std::hex << addr
	//	<< " (0x"
	//	<< trans.get_address() << std::dec
	//	<< ") of " << size << " bytes"
	//	<< EndDebugInfo;

	if ( is_read )
	{
		handled = true;
		/* in some conditions the registers might need to be updated
		 * for example:
		 * - when reading Current Value Register, TimerXValue of t1 or t2 and
		 *     the timer is enabled
		 *
		 * To make it simple we update the status of the timers each time
		 *   a read operation is performed.
		 */
		UpdateStatus(delay);
		memcpy(data, &regs[addr], size);
		//logger << DebugInfo
		//	<< "Read 0x"
		//	<< std::hex;
		//for ( int i = 0; i < size; i++ )
		//{
		//	logger << (unsigned int)regs[addr + 3 - i];
		//}
		//logger << std::dec
		//	<< EndDebugInfo;
		trans.set_response_status(tlm::TLM_OK_RESPONSE);
	}
	else // writing
	{
		/* Steps performed when writing:
		 * 1 - make a copy of destination register before modifying it
		 * 2 - update register with new data
		 * 3 - get the value of the new register
		 * 4 - verify new value of the register and perform required actions
		 */
		uint32_t prev_value, new_value;
		/* 1 - make a copy of destination register before modifying it */
		memcpy(&prev_value, &regs[addr & ~(uint32_t)0x03], 4);
		/* 2 - update register with new data */
		memcpy(&regs[addr], data, size);
		/* 3 - get the value of the new register */
		memcpy(&new_value, &regs[addr & ~(uint32_t)0x03], 4);
		/* 4 - verify new value of the register and perform required actions */
		prev_value = LittleEndian2Host(prev_value);
		new_value = LittleEndian2Host(new_value);
		if ( (addr & ~(uint32_t)0x03) == 0x08 )
		{
			// writing into Timer1Control
			uint32_t unmod_value = new_value;
			new_value = (new_value & ~0xffffff00UL) | 
				(prev_value & 0xffffff00UL);
			logger << DebugInfo
				<< "Writing 0x"
				<< std::hex << new_value
				<< " (previous value 0x"
				<< prev_value
				<< ", received 0x"
				<< unmod_value << std::dec
				<< ")"
				<< EndDebugInfo;
			/* if the new value needed to be fixed rewrite it to the register */
			if ( unmod_value != new_value )
			{
				uint32_t final_value = Host2LittleEndian(new_value);
				memcpy(&regs[addr & ~(uint32_t)0x03], &final_value, 4);
			}
			/* if the value did not change no further action is required */
			if ( prev_value == new_value )
			{
				handled = true;
			}
			else 
			{
				handled = true;
				/* if the TimerEn did change from 0 to 1 then set the
				 *   update time for t1 */
				if ( ((prev_value & (uint32_t)0x080) == 0) &&
						((new_value & (uint32_t)0x080) != 0) )
				{
					logger << DebugInfo
						<< "Timer enabled"
						<< EndDebugInfo;
					handled &= true;
					t1_update_time = sc_time_stamp();
				}
				else
				{
					/* update the value of the Timer1Value if the timer was 
					 * activated before the access */
					if ( (prev_value & (uint32_t)0x080) != 0 )
					{
						logger << DebugError
							<< "Update timer 1"
							<< EndDebugError;
						handled &= false;
					}
				}
				/* if the IntEnable entry is not active no further action is
				 * needed */
				if ( (new_value & (uint32_t)0x020) == 0 )
				{
					handled &= true;
				}
				else
				{
					logger << DebugError
						<< "IntEnable activated"
						<< EndDebugError;
					handled &= false;
				}
			}
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
DualTimer ::
bus_target_get_direct_mem_ptr(transaction_type &trans, 
		tlm::tlm_dmi &dmi_data)
{
	assert("TODO" == 0);
	return false;
}

unsigned int 
DualTimer ::
bus_target_transport_dbg(transaction_type &trans)
{
	assert("TODO" == 0);
	return 0;
}

/**************************************************************************/
/* Virtual methods for the target socket for the bus connection       END */
/**************************************************************************/

void 
DualTimer ::
UpdateStatus(sc_core::sc_time &delay)
{
	wait(delay);
	delay = SC_ZERO_TIME;
	
	uint32_t t1_control = 0;
	memcpy(&t1_control, &regs[0x8], sizeof(t1_control));
	t1_control = LittleEndian2Host(t1_control);
	if ( t1_control & (uint32_t)0x080 )
	{
		uint32_t prescale = (t1_control & (uint32_t)0x0c) >> 2;
		switch ( prescale )
		{
			case 0:
				prescale = 1; break;
			case 1: 
				prescale = 16; break;
			case 2:
				prescale = 256; break;
			default: 
				logger << DebugWarning
					<< "Undefined prescale, using 1"
					<< EndDebugWarning;
				prescale = 1;
				break;
		}
		const sc_core::sc_time &cur_time = sc_time_stamp();
		if ( t1_update_time < cur_time )
		{
			sc_core::sc_time diff_time = cur_time - t1_update_time;
			sc_core::sc_time timclk = 
				sc_core::sc_time((double)timclk_in_port, SC_PS);
			sc_core::sc_time timclken1 = 
				sc_core::sc_time((double)timclken1_in_port, SC_PS);
			uint64_t tick_time =
				((double)timclken1_in_port / (double)timclk_in_port) 
				* prescale * timclk_in_port;
			sc_core::sc_time tick = 
				sc_core::sc_time((double)tick_time, SC_PS);
			uint64_t diff = diff_time / tick;
			if ( diff != 0 )
			{
				uint32_t t1_current_val = 0;
				uint32_t t1_new_val = 0;
				memcpy(&t1_current_val, &regs[0x4], sizeof(t1_current_val));
				t1_current_val = LittleEndian2Host(t1_current_val);
				t1_new_val = t1_current_val - diff;
				t1_new_val = Host2LittleEndian(t1_new_val);
				memcpy(&regs[0x4], &t1_new_val, sizeof(t1_new_val));
	//			logger << DebugInfo
	//				<< "cur_time = " << sc_time_stamp() << std::endl
	//				<< "t1_update_time = " << t1_update_time << std::endl
	//				<< "diff_time = " << diff_time << std::endl
	//				<< "timclk = " << timclk << std::endl
	//				<< "timclken1 = " << timclken1 << std::endl
	//				<< "prescale = " << prescale << std::endl
	//				<< "tick = " << tick << std::endl
	//				<< "diff = " << diff
	//				<< "t1_current_val = 0x" << std::hex
	//				<< t1_current_val << std::endl
	//				<< "t1_new_val = 0x" << t1_new_val
	//				<< std::dec
	//				<< EndDebugInfo;
				t1_update_time = cur_time;
			} 
			else
			{
				// logger << DebugInfo
				//	<< "cur_time = " << sc_time_stamp() << std::endl
				//	<< "t1_update_time = " << t1_update_time << std::endl
				//	<< "diff_time = " << diff_time << std::endl
				//	<< "timclk = " << timclk << std::endl
				//	<< "timclken1 = " << timclken1 << std::endl
				//	<< "prescale = " << prescale << std::endl
				//	<< "tick = " << tick << std::endl
				//	<< "diff = " << diff
				//	<< EndDebugInfo;
			}
		}
	}
}

} // end of namespace dual_timer 
} // end of namespace arm926ejs_pxp
} // end of namespace chipset
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim


