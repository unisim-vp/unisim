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
	, verbose(0)
	, param_verbose("verbose", this, verbose,
			"Verbose level (0 = no verbose).")
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

	// check the address range
	if ( (addr >= 0x01000UL) ||
			((0x01000L - size) < addr) )
	{
		logger << DebugWarning
			<< "Incorrect access address range:" << std::endl
			<< " - read = " << is_read << std::endl
			<< " - addr = 0x" << std::hex << addr << std::dec << std::endl
			<< " - size = " << size;
		if ( !is_read )
		{
			logger << std::endl
				<< " - data =" << std::hex;
			for ( unsigned int i = 0; i < size; i++ )
				logger << " " << (unsigned int)data[i];
			logger << std::dec;
		}
		logger << std::endl
			<< "Ending transaction with address error."
			<< EndDebugWarning;
		// set the tlm error status
		trans.set_response_status(tlm::TLM_ADDRESS_ERROR_RESPONSE);
	}

	// do not accept request that access more than one register, neither accept
	//   streaming requests
	// NOTE: this maybe something that might be supported in future versions
	//       of the module implementation
	if ( (((addr & 0x03) + size) > 4) || trans.get_streaming_width() ) 
	{
		logger << DebugWarning
			<< "Multiple register access and/or streamed accesses"
			<< " are not supported:" << std::endl
			<< " - read = " << is_read << std::endl
			<< " - addr = 0x" << std::hex << addr << std::dec << std::endl
			<< " - size = " << size;
		if ( !is_read )
		{
			logger << std::endl
				<< " - data =" << std::hex;
			for ( unsigned int i = 0; i < size; i++ )
				logger << " " << (unsigned int)data[i];
			logger << std::dec;
		}
		logger << std::endl
			<< "Ending transaction with burst error."
			<< EndDebugWarning;
		// set the tlm error status
		trans.set_response_status(tlm::TLM_BURST_ERROR_RESPONSE);
	}

	if ( is_read )
	{
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
		handled = true;
	}

	else // it is write
	{
		/* Steps performed when writing:
		 * 1 - make a copy of destination register before modifying it
		 * 2 - update register with new data
		 * 3 - get the value of the new register
		 * 4 - verify new value of the register and perform required
		 *     actions
		 */
		uint32_t cur_addr = addr & ~(uint32_t)0x03UL;
		uint32_t prev_value, new_value;

		/* 1 - make a copy of destination register before modifying it
		 */
		prev_value = GetRegister(cur_addr);

		/* 2 - update register with new data */
		memcpy(&regs[addr], data, size);

		/* 3 - get the value of the new register */
		new_value = GetRegister(cur_addr);

		/* 4 - verify new value of the register and perform required 
		 *     actions */
		if ( ( cur_addr == TIMER1CONTROL ) ||
				( cur_addr == TIMER2CONTROL ) )
		{
			uint32_t unmod_value = new_value;
			new_value = (new_value & ~0xffffff00UL) | 
				(prev_value & 0xffffff00UL);
			/* if the new value needed to be fixed rewrite it to the 
			 *   register */
			if ( unmod_value != new_value )
			{
				SetRegister(cur_addr, new_value);
			}
			/* if the value did not change no further action is 
			 *   required */
			if ( prev_value == new_value )
			{
				handled = true;
			}
			else 
			{
				handled = true;
				/* if the new value is deactivating the timer nothing
				 *   needs to be done */
				if ( (new_value & (uint32_t)0x080UL) == 0 )
				{
					if ( (prev_value & (uint32_t)0x080UL) != 0 )
					{
						if ( VERBOSE(V0, V_STATUS) )
							logger << DebugInfo
								<< "Timer "
								<< ((cur_addr == TIMER1CONTROL) ?
										"1" : "2")
								<< " disabled at "
								<< (sc_time_stamp() + delay)
								<< EndDebugInfo;
					}
				}

				else
				{
					/* if the TimerEn did change from 0 to 1 then set 
					 *   the update time for t1/t2 */
					if ( ((prev_value & (uint32_t)0x080) == 0) &&
							((new_value & (uint32_t)0x080) != 0) )
					{
						handled &= true;
						if ( cur_addr == TIMER1CONTROL )
							t1_update_time = sc_time_stamp() + delay;
						else
							t2_update_time = sc_time_stamp() + delay;

						if ( VERBOSE(V0, V_STATUS) )
							logger << DebugInfo
								<< "Timer "
								<< ((cur_addr == TIMER1CONTROL) ?
										"1" : "2")
								<< " enabled at "
								<< ((cur_addr == TIMER1CONTROL) ?
										t1_update_time :
										t2_update_time)
								<< EndDebugInfo;
					}
					else
					{
						/* update the value of the TimerXValue if the
						 *   timer was activated before the access */
						if ( (prev_value & (uint32_t)0x080) != 0 )
						{
							logger << DebugError
								<< "Update timer " 
								<< ((cur_addr == TIMER1CONTROL) ?
										"1" : "2") 
								<< std::endl
								<< " - prev value = 0x" << std::hex
								<< prev_value << std::endl
								<< " - new value = 0x" 
								<< new_value << std::dec
								<< EndDebugError;
							handled &= false;
						}
					}
					/* if the IntEnable entry is not active no further 
					 *   action is needed */
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
			}
		}

		else if ( cur_addr == TIMER1LOAD ||
				cur_addr == TIMER2LOAD )
		{
			handled = true;
			if ( new_value == 0 )
				logger << DebugWarning
					<< "Setting "
					<< ((cur_addr == TIMER1LOAD) ?
							"TIMER1LOAD" :
							"TIMER2LOAD")
					<< " to 0, this might cause continuous interrupts"
					<< " under certain circumstances"
					<< EndDebugWarning;

			// update the timer 1/2 value to the new value
			wait(delay);
			delay = SC_ZERO_TIME;
			if ( cur_addr == TIMER1LOAD )
			{
				t1_update_time = sc_time_stamp();
				SetRegister(TIMER1VALUE, new_value);
			}
			else
			{
				t2_update_time = sc_time_stamp();
				SetRegister(TIMER2VALUE, new_value);
			}
		}

		else if ( cur_addr == TIMER1VALUE ||
				cur_addr == TIMER2VALUE )
		{
			handled = true;
			logger << DebugWarning
				<< "Trying to write into "
				<< ((cur_addr == TIMER1LOAD) ?
							"TIMER1LOAD" :
							"TIMER2LOAD")
				<< " which is read-only, ignoring new value (0x"
				<< std::hex << new_value << std::dec << ") and keeping"
				<< " the old value (0x"
				<< std::hex << prev_value << std::dec << ")"
				<< EndDebugWarning;
			if ( cur_addr == TIMER1VALUE )
				SetRegister(TIMER1VALUE, prev_value);
			else
				SetRegister(TIMER2VALUE, prev_value);
		}
	}

	if ( !handled )
	{
		// display error
		logger << DebugError
			<< "Access to dual timer:" << std::endl
			<< " - read = " << is_read << std::endl
			<< " - addr = 0x" << std::hex << addr << std::dec << std::endl
			<< " - size = " << size;
		if ( !is_read )
		{
			logger << std::endl
				<< " - data =" << std::hex;
			for ( unsigned int i = 0; i < size; i++ )
				logger << " " << (unsigned int)data[i];
			logger << std::dec;
		}
		logger << std::endl
			<< "Stopping simulation because of unhandled behavior"
			<< EndDebugError;
		// stop simulation
		unisim::kernel::service::Simulator::simulator->Stop(this, __LINE__);
	}

	// everything went fine, update the status of the tlm response
	trans.set_response_status(tlm::TLM_OK_RESPONSE);

	if ( VERBOSE(V0, V_READ | V_WRITE) )
	{
		logger << DebugInfo
			<< "Access to dual timer:" << std::endl
			<< " - read = " << is_read << std::endl
			<< " - addr = 0x" << std::hex << addr << std::dec << std::endl
			<< " - size = " << size << std::endl
			<< " - data =" << std::hex;
		for ( unsigned int i = 0; i < size; i++ )
			logger << " " << (unsigned int)data[i];
		logger << std::dec
			<< EndDebugInfo;
	}
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

/** Returns the register pointed by the given address
 *
 * @param addr the address to consider
 * @return the value of the register pointed by the address
 */
uint32_t 
DualTimer ::
GetRegister(uint32_t addr) const
{
	const uint8_t *data = &(regs[addr]);
	uint32_t value = 0;

	memcpy(&value, data, 4);
	value = LittleEndian2Host(value);
	return value;
}

/** Sets the register pointed by the given address
 *
 * @param addr the address to consider
 * @param value the value to set the register
 */
void 
DualTimer ::
SetRegister(uint32_t addr, uint32_t value)
{
	uint8_t *data = &(regs[addr]);

	value = Host2LittleEndian(value);
	memcpy(data, &value, 4);
}

/** Update the status of the timer at the given time
 *
 * @param delay the delta time 
 */
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
				t1_update_time = cur_time;
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


