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
#include <sstream>

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
	, VICIntSourceIdentifierInterface()
	, nvicfiqin()
	, nvicirqin()
	, vicvectaddrin() 
	, bus_target_socket("bus_target_socket")
	, int_source(0)
	, nvicfiq_value(false)
	, nvicirq_value(false)
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

	for ( int i = 0; i < NUM_SOURCE_INT; i++ )
	{
		std::stringstream id_name;

		id_name << "VICIntSourceIdentifier[" << i << "]";
		source_identifier_method[i] =
			new VICIntSourceIdentifier(id_name.str().c_str(),
					i, this);
	}
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

/** Update the status of the VIC depending on all the possible entries
 */
void
VIC ::
UpdateStatus()
{
	// apply software interrupt register and the values of 
	//   interrupt sources to the raw interrupt register
	uint32_t softint = GetVICSOFTINT();
	uint32_t rawintr = 0;
	for ( unsigned int i =0; i < NUM_SOURCE_INT; i++ )
	{
		rawintr |= (softint & int_source) & (0x01UL << i);
	}
	SetVICRAWINTR(rawintr);

	// compute the new fiq and irq status
	uint32_t intselect = GetVICINTSELECT();
	uint32_t intenable = GetVICINTENABLE();
	uint32_t fiqstatus = 0;
	uint32_t irqstatus = 0;
	for ( unsigned int i = 0; i < NUM_SOURCE_INT; i++ )
	{
		if ( intselect & (0x01UL << i) )
			fiqstatus |= (rawintr & (0x01UL << i));
		else
			irqstatus |= (rawintr & (0x01UL << i));
	}
	SetVICFIQSTATUS(fiqstatus);
	SetVICIRQSTATUS(irqstatus);

	// compute the new nVICFIQ and VICITOP1
	uint32_t itop1 = GetVICITOP1();
	itop1 &= ~(0x01UL << 6); // clean bit 6
	if ( !nvicfiqin || fiqstatus )
	{
		nvicfiq_value = false;
		itop1 |= (0x01UL << 6); // set bit 6
	}
	else
		nvicfiq_value = true;

	// compute the VECTIRQn signals
	//   and set the VICVECTADDR if required
	bool vectirq[15];
	bool priority = false;
	uint32_t nonvect_irqs = 0;
	for ( unsigned int i = 0; i < NUM_VECT_INT; i++ )
	{
		uint32_t vectcntl = GetVICVECTCNTL(i);
		uint32_t src = VECTCNTLSource(vectcntl);

		if ( VECTCNTLEnable(vectcntl) )
		{
			// prepare the nonvectored irqs
			nonvect_irqs |= (0x01UL << src);
			if ( priority )
				vectirq[i] = false;
			else
			{
				if ( irqstatus & (0x01UL << src) )
				{
					vectirq[i] = true;
					priority = true;
					SetVICVECTADDR(GetVICVECTADDR(i));
				}
			}
		}
	}

	// if there is no vectored irq generated then
	//   a nonvectored irq can be raised
	// also set the default vector addr if none was detected
	bool nonvectored_irq = false;
	if ( !priority )
	{
		SetVICVECTADDR(GetVICDEFVECTADDR());
		if ( irqstatus & ~nonvect_irqs )
		{
			nonvectored_irq = true;
			priority = true;
		}
	}

	// if a vectored or nonvectored irq was generated
	//   then set the nVICIRQ signal, otherwise set it 
	//   if an external IRQ was generated
	// NOTE: the nvicirq_value is inverted
	nvicirq_value = true;
	if ( priority )
		nvicirq_value = false;
	else
	{
		if ( nvicfiqin )
		{
			nvicirq_value = false;
			SetVICVECTADDR(vicvectaddrin);
		}
	}
	// update itop1
	itop1 &= ~(0x01UL << 7);
	if ( !nvicirq_value )
		itop1 |= (0x01UL << 7);
	SetVICITOP1(itop1);
}

/** Source interrupt handling */
void 
VIC ::
VICIntSourceReceived(int id, bool value)
{
	uint32_t orig = int_source;
	if ( value )
		int_source |= (0x01UL << id);
	else
		int_source &= ~(0x01UL << id);
	UpdateStatus();

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
		if ( (0x01000UL - size) >= addr )
		{
			memcpy(data, &(regs[addr]), size);
			if ( addr >= 0x0fe0UL )
			{
				handled = true;
			}
		}
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
		if ( !is_read )
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

/**************************************************************************/
/* Methods to get and set the registers                             START */
/**************************************************************************/

/** Returns the enable field of the VICVECTCNTL register.
 *
 * @param value the register value
 * @return true if enable is set, false otherwise
 */
bool 
VIC ::
VECTCNTLEnable(uint32_t value)
{
	if ( value & (0x01UL << 5) )
		return true;
	return false;
}

/** Returns the source field of the VICVECTCNTL register
 *
 * @param value the register value
 * @return the value of the source field
 */
uint32_t 
VIC ::
VECTCNTLSource(uint32_t value)
{
	return value & 0x01fUL;
}

/** Returns the register pointed by the given address
 *
 * @param addr the address to consider
 * @return the value of the register pointed by the address
 */
uint32_t
VIC ::
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
VIC ::
SetRegister(uint32_t addr, uint32_t value)
{
	uint8_t *data = &(regs[addr]);

	value = Host2LittleEndian(value);
	memcpy(data, &value, 4);
}

/** Returns the IRQ status register
 *
 * @return the value of the IRQ status register
 */
uint32_t 
VIC ::
GetVICIRQSTATUS() const
{
	return GetRegister(VICIRQSTATUSAddr);
}

/** Sets the IRQ status register
 *
 * @param value the value to set
 */
void 
VIC ::
SetVICIRQSTATUS(uint32_t value)
{
	SetRegister(VICIRQSTATUSAddr, value);
}

/** Returns the FIQ status register
 *
 * @return the value of the FIQ status register
 */
uint32_t 
VIC :: 
GetVICFIQSTATUS() const
{
	return GetRegister(VICFIQSTATUSAddr);
}

/** Sets the FIQ status register
 *
 * @param value the value to set
 */
void 
VIC ::
SetVICFIQSTATUS(uint32_t value)
{
	SetRegister(VICFIQSTATUSAddr, value);
}

/** Returns the raw interrupt register
 *
 * @return the value of the raw interrupt register
 */
uint32_t 
VIC ::
GetVICRAWINTR() const
{
	return GetRegister(VICRAWINTRAddr);
}

/** Sets the raw interrupt register
 *
 * @param value the value to set
 */
void 
VIC ::
SetVICRAWINTR(uint32_t value)
{
	SetRegister(VICRAWINTRAddr, value);
}

/** Returns the interrupt select register
 *
 * @return the value of the interrupt select register
 */
uint32_t 
VIC ::
GetVICINTSELECT() const
{
	return GetRegister(VICINTSELECTAddr);
}

/** Sets the interrupt select register
 *
 * @param value the value to set
 */
void 
VIC ::
SetVICINTSELECT(uint32_t value)
{
	SetRegister(VICINTSELECTAddr, value);
}

/** Returns the interrupt enable register
 *
 * @return the value of the interrupt enable register
 */
uint32_t 
VIC ::
GetVICINTENABLE() const
{
	return GetRegister(VICINTENABLEAddr);
}

/** Sets the interrupt enable register
 *
 * @param value the value to set
 */
void 
VIC ::
SetVICINTENABLE(uint32_t value)
{
	SetRegister(VICINTENABLEAddr, value);
}

/** Returns the software interrupt register
 *
 * @return the value of the software interrupt register
 */
uint32_t 
VIC ::
GetVICSOFTINT() const
{
	return GetRegister(VICSOFTINTAddr);
}

/** Sets the software interrupt register
 *
 * @param value the value to set
 */
void 
VIC ::
SetVICSOFTINT(uint32_t value)
{
	SetRegister(VICSOFTINTAddr, value);
}

/** Returns the vector address register
 *
 * @return the value of the vector address register
 */
uint32_t 
VIC ::
GetVICVECTADDR() const
{
	return GetRegister(VICVECTADDRAddr);
}

/** Set the vector address register
 *
 * @param value the value to set
 */
void 
VIC ::
SetVICVECTADDR(uint32_t value)
{
	SetRegister(VICVECTADDRAddr, value);
}

/** Returns the default vector address register
 *
 * @return the value of the default vector address register
 */
uint32_t 
VIC ::
GetVICDEFVECTADDR() const
{
	return GetRegister(VICDEFVECTADDRAddr);
}

/** Sets the default vector address register
 *
 * @param value the value to set
 */
void 
VIC ::
SetVICDEFVECTADDR(uint32_t value)
{
	SetRegister(VICDEFVECTADDRAddr, value);
}

/** Returns one of the vector address registers
 *
 * @param index the index to look for
 * @return the value of the indexed vector address register
 */
uint32_t 
VIC ::
GetVICVECTADDR(uint32_t index) const
{
	return GetRegister(VICVECTADDRBaseAddr + (4 * index));
}

/** Sets the value of one of the vector address registers
 *
 * @param index the index to look for
 * @param value the value to set
 */
void 
VIC ::
SetVICVECTADDR(uint32_t index, uint32_t value)
{
	SetRegister(VICVECTADDRBaseAddr + (4 * index),
			value);
}

/** Returns one of the vector control registers
 *
 * @param index the index to look for
 * @return the value of the indexed vector control register
 */
uint32_t 
VIC ::
GetVICVECTCNTL(uint32_t index) const
{
	return GetRegister(VICVECTCNTLBaseAddr + (4 * index));
}

/** Sets the value of one of the vector controll registers
 *
 * @param index the index to look for
 * @param value the value to set
 */
void 
VIC ::
SetVICVECTCNTL(uint32_t index, uint32_t value)
{
	SetRegister(VICVECTCNTLBaseAddr + (4 * index), value);
}

/** Returns the test output register (nVICIRQ/nVICFIQ)
 *
 * @return the value of the test output register (nVICIRQ/nVICFIQ)
 */
uint32_t 
VIC ::
GetVICITOP1() const
{
	return GetRegister(VICITOP1Addr);
}

/** Set the test output register (nVICIRQ/nVICFIQ)
 *
 * @param value the value to set
 */
void 
VIC ::
SetVICITOP1(uint32_t value)
{
	SetRegister(VICITOP1Addr, value);
}

/**************************************************************************/
/* Methods to get and set the registers                               END */
/**************************************************************************/

} // end of namespace vic
} // end of namespace arm926ejs_pxp
} // end of namespace chipset
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim


