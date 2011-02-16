/*
 *  Copyright (c) 2011,
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
 * Authors: Gilles Mouchard (gilles.mouchard@cea.fr)
 */

#ifndef __UNISIM_COMPONENT_CXX_TIMER_XILINX_XPS_INTC_XPS_TIMER_TCC__
#define __UNISIM_COMPONENT_CXX_TIMER_XILINX_XPS_INTC_XPS_TIMER_TCC__

#include <unisim/util/endian/endian.hh>
#include <unisim/util/arithmetic/arithmetic.hh>
#include <string.h>

namespace unisim {
namespace component {
namespace cxx {
namespace timer {
namespace xilinx {
namespace xps_timer {

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;

template <class CONFIG>
XPS_Timer<CONFIG>::XPS_Timer(const char *name, Object *parent)
	: Object(name, parent)
	, Service<Memory<typename CONFIG::MEMORY_ADDR> >(name, parent)
	, memory_export("memory-export", this)
	, logger(*this)
	, verbose(false)
	, tcsr0(0)
	, tlr0(0)
	, tcr0(0)
	, tcsr1(0)
	, tlr1(0)
	, tcr1(0)
	, param_verbose("verbose", this, verbose, "Enable/Disable verbosity")
{
	std::stringstream sstr_description;
	sstr_description << "This module implements a Xilinx XPS Timer/Counter (v1.02a). It has the following characteristics:" << std::endl;
	sstr_description << "PLB data width: " << CONFIG::C_SPLB_DWITH << " bits" << std::endl;
	sstr_description << "Width of the counters: " << CONFIG::C_COUNT_WIDTH << " bits" << std::endl;
	sstr_description << "Address range: 0x" << std::hex << CONFIG::C_BASEADDR << "-0x" << CONFIG::C_HIGHADDR << std::dec << std::endl;
	sstr_description << "One timer only: " << (CONFIG::C_ONE_TIMER_ONLY ? "yes" : "no") << std::endl;
	sstr_description << "CaptureTrig0 assertion level: " << (CONFIG::C_TRIG0_ASSERT ? "high" : "low") << std::endl;
	sstr_description << "CaptureTrig1 assertion level: " << (CONFIG::C_TRIG1_ASSERT ? "high" : "low") << std::endl;
	sstr_description << "GenerateOut0 assertion level: " << (CONFIG::C_GEN0_ASSERT ? "high" : "low") << std::endl;
	sstr_description << "GenerateOut0 assertion level: " << (CONFIG::C_GEN1_ASSERT ? "high" : "low") << std::endl;
	
	Object::SetDescription(sstr_description.str().c_str());
}

template <class CONFIG>
XPS_Timer<CONFIG>::~XPS_Timer()
{
}

template <class CONFIG>
bool XPS_Timer<CONFIG>::BeginSetup()
{
	Reset();
	return true;
}

template <class CONFIG>
void XPS_Timer<CONFIG>::Reset()
{
	tcsr0 = 0;
	tlr0 = 0;
	tcr0 = 0;
	tcsr1 = 0;
	tlr1 = 0;
	tcr1 = 0;
}

template <class CONFIG>
bool XPS_Timer<CONFIG>::ReadMemory(typename CONFIG::MEMORY_ADDR addr, void *buffer, uint32_t size)
{
	if(size > 0)
	{
		if(!IsMapped(addr, size)) return false;
		uint8_t *dst = (uint8_t *) buffer;
		
		do
		{
			uint32_t value;
			Read(addr, value);
			if(size >= 4)
			{
				memcpy(dst, &value, 4);
				size -= 4;
				dst += 4;
				addr += 4;
			}
			else
			{
				memcpy(dst, &value, size);
				size = 0;
			}
		}
		while(size);
	}
	return true;
}

template <class CONFIG>
bool XPS_Timer<CONFIG>::WriteMemory(typename CONFIG::MEMORY_ADDR addr, const void *buffer, uint32_t size)
{
	if(size > 0)
	{
		if(!IsMapped(addr, size)) return false;
		const uint8_t *src = (const uint8_t *) buffer;
		
		do
		{
			uint32_t value;
			if(size >= 4)
			{
				memcpy(&value, src, 4);
				size -= 4;
				src += 4;
				addr += 4;
			}
			else
			{
				Read(addr, value);
				memcpy(&value, src, size);
				size = 0;
			}
			Write(addr, value);
		}
		while(size);
	}
	return true;
}

template <class CONFIG>
bool XPS_Timer<CONFIG>::ReadMemory(typename CONFIG::MEMORY_ADDR addr, void *buffer, uint32_t size, const uint8_t *byte_enable, uint32_t byte_enable_length, uint32_t streaming_width)
{
	return false; // TODO
}

template <class CONFIG>
bool XPS_Timer<CONFIG>::WriteMemory(typename CONFIG::MEMORY_ADDR addr, const void *buffer, uint32_t size, const uint8_t *byte_enable, uint32_t byte_enable_length, uint32_t streaming_width)
{
	return false; // TODO
}

template <class CONFIG>
template <typename T>
void XPS_Timer<CONFIG>::Read(typename CONFIG::MEMORY_ADDR addr, T& value)
{
	typename CONFIG::MEMORY_ADDR offset = addr - CONFIG::C_BASEADDR;
	if(verbose)
	{
		logger << DebugInfo << "Reading register 0x" << std::hex << offset << std::dec << "(";
		switch(offset)
		{
			case CONFIG::TCSR0: logger << "tcsr0"; break;
			case CONFIG::TLR0: logger << "tlr0"; break;
			case CONFIG::TCR0: logger << "tcr0"; break;
			case CONFIG::TCSR1: logger << "tcsr1"; break;
			case CONFIG::TLR1: logger << "tlr1"; break;
			case CONFIG::TCR1: logger << "tcr1"; break;
			default: logger << "?"; break;
		}
		logger << ")" << EndDebugInfo;
	}
	uint32_t reg_value;
	switch(sizeof(T))
	{
		case 1:
			switch(offset)
			{
				case CONFIG::TCSR0: reg_value = GetTCSR0() >> 24; break;
				case CONFIG::TLR0: reg_value = GetTLR0() >> 24; break;
				case CONFIG::TCR0: reg_value = GetTCR0() >> 24; break;
				case CONFIG::TCSR1: reg_value = GetTCSR1() >> 24; break;
				case CONFIG::TLR1: reg_value = GetTLR1() >> 24; break;
				case CONFIG::TCR1: reg_value = GetTCR1() >> 24; break;
				default: reg_value = 0; break;
			}
			break;
		case 2:
			switch(offset)
			{
				case CONFIG::TCSR0: reg_value = GetTCSR0() >> 16; break;
				case CONFIG::TLR0: reg_value = GetTLR0() >> 16; break;
				case CONFIG::TCR0: reg_value = GetTCR0() >> 16; break;
				case CONFIG::TCSR1: reg_value = GetTCSR1() >> 16; break;
				case CONFIG::TLR1: reg_value = GetTLR1() >> 16; break;
				case CONFIG::TCR1: reg_value = GetTCR1() >> 16; break;
				default: reg_value = 0; break;
			}
			break;
		case 4:
			switch(offset)
			{
				case CONFIG::TCSR0: reg_value = GetTCSR0(); break;
				case CONFIG::TLR0: reg_value = GetTLR0(); break;
				case CONFIG::TCR0: reg_value = GetTCR0(); break;
				case CONFIG::TCSR1: reg_value = GetTCSR1(); break;
				case CONFIG::TLR1: reg_value = GetTLR1(); break;
				case CONFIG::TCR1: reg_value = GetTCR1(); break;
				default: reg_value = 0; break;
			}
			break;
		default: reg_value = 0; break;
	}
	
	value = unisim::util::endian::Host2BigEndian(reg_value);
}

template <class CONFIG>
template <typename T>
void XPS_Timer<CONFIG>::Write(typename CONFIG::MEMORY_ADDR addr, T value)
{
	typename CONFIG::MEMORY_ADDR offset = addr - CONFIG::C_BASEADDR;
	if(verbose)
	{
		logger << DebugInfo << "Writing register 0x" << std::hex << offset << std::dec << "(";
		switch(offset)
		{
			case CONFIG::TCSR0: logger << "tcsr0"; break;
			case CONFIG::TLR0: logger << "tlr0"; break;
			case CONFIG::TCSR1: logger << "tcsr1"; break;
			case CONFIG::TLR1: logger << "tlr1"; break;
			default: logger << "?"; break;
		}
		logger << ")" << EndDebugInfo;
	}

	uint32_t reg_value = unisim::util::endian::BigEndian2Host(value);

	switch(sizeof(T))
	{
		case 1:
			switch(offset)
			{
				case CONFIG::TCSR0: SetTCSR0((GetTCSR0() & 0x00ffffffUL) | ((uint32_t) reg_value << 24)); break;
				case CONFIG::TLR0: SetTLR0((GetTLR0() & 0x00ffffffUL) | ((uint32_t) reg_value << 24)); break;
				case CONFIG::TCSR1: SetTCSR1((GetTCSR1() & 0x00ffffffUL) | ((uint32_t) reg_value << 24)); break;
				case CONFIG::TLR1: SetTLR1((GetTLR1() & 0x00ffffffUL) | ((uint32_t) reg_value << 24)); break;
				default: break;
			}
			break;
		case 2:
			switch(offset)
			{
				case CONFIG::TCSR0: SetTCSR0((GetTCSR0() & 0x0000ffffUL) | ((uint32_t) reg_value << 16)); break;
				case CONFIG::TLR0: SetTLR0((GetTLR0() & 0x0000ffffUL) | ((uint32_t) reg_value << 16)); break;
				case CONFIG::TCSR1: SetTCSR1((GetTCSR1() & 0x0000ffffUL) | ((uint32_t) reg_value << 16)); break;
				case CONFIG::TLR1: SetTLR1((GetTLR1() & 0x0000ffffUL) | ((uint32_t) reg_value << 16)); break;
				default: break;
			}
			break;
		case 4:
			switch(offset)
			{
				case CONFIG::TCSR0: SetTCSR0(reg_value); break;
				case CONFIG::TLR0: SetTLR0(reg_value); break;
				case CONFIG::TCSR1: SetTCSR1(reg_value); break;
				case CONFIG::TLR1: SetTLR1(reg_value); break;
				default: break;
			}
			break;
	}
}

template <class CONFIG>
bool XPS_Timer<CONFIG>::IsMapped(typename CONFIG::MEMORY_ADDR addr, uint32_t size) const
{
	return (addr >= CONFIG::C_BASEADDR) && ((addr + size - 1) <= CONFIG::C_HIGHADDR);
}

template <class CONFIG>
uint32_t XPS_Timer<CONFIG>::GetTCSR0() const
{
	return tcsr0 & CONFIG::TCSR0_MASK;
}

template <class CONFIG>
uint32_t XPS_Timer<CONFIG>::GetTLR0() const
{
	return tlr0 & CONFIG::COUNTER_MASK;
}

template <class CONFIG>
uint32_t XPS_Timer<CONFIG>::GetTCR0() const
{
	return tcr0 & CONFIG::COUNTER_MASK;
}

template <class CONFIG>
uint32_t XPS_Timer<CONFIG>::GetTCSR1() const
{
	return CONFIG::C_ONE_TIMER_ONLY ? 0 : tcsr1 & CONFIG::TCSR1_MASK;
}

template <class CONFIG>
uint32_t XPS_Timer<CONFIG>::GetTLR1() const
{
	return tlr1 & CONFIG::COUNTER_MASK;
}

template <class CONFIG>
uint32_t XPS_Timer<CONFIG>::GetTCR1() const
{
	return CONFIG::C_ONE_TIMER_ONLY ? 0 : tcr1 & CONFIG::COUNTER_MASK;
}

template <class CONFIG>
void XPS_Timer<CONFIG>::SetTCSR0(uint32_t value)
{
	// write value in TCSR0, preserve T0INT
	tcsr0 = (tcsr0 & CONFIG::TCSR0_T0INT_MASK) | (value & ~CONFIG::TCSR0_T0INT_MASK & CONFIG::TCSR0_MASK);
	
	if(value & CONFIG::TCSR0_T0INT_MASK)
	{
		// clear T0INT
		tcsr0 = tcsr0 & ~CONFIG::TCSR0_T0INT_MASK;
	}
	
	if(tcsr0 & CONFIG::TCSR0_ENALL_MASK)
	{
		// set ENT0 and ENT1
		tcsr0 = tcsr0 | CONFIG::TCSR0_ENT0_MASK;
		if(!CONFIG::C_ONE_TIMER_ONLY)
		{
			tcsr1 = tcsr1 | CONFIG::TCSR1_ENT1_MASK;
		}
	}

	if(!CONFIG::C_ONE_TIMER_ONLY)
	{
		// mirror ENALL in TCSR1
		tcsr1 = (tcsr1 & ~CONFIG::TCSR1_ENALL_MASK) | (tcsr0 & CONFIG::TCSR0_ENALL_MASK);
	}
}

template <class CONFIG>
void XPS_Timer<CONFIG>::SetTCSR0_T0INT(uint32_t value)
{
	SetTCSR0((GetTCSR0() & ~CONFIG::TCSR0_T0INT_MASK) | ((value << CONFIG::TCSR0_T0INT_OFFSET) & CONFIG::TCSR0_T0INT_MASK));
}

template <class CONFIG>
void XPS_Timer<CONFIG>::SetTLR0(uint32_t value)
{
	tlr0 = value & CONFIG::COUNTER_MASK;
}

template <class CONFIG>
void XPS_Timer<CONFIG>::SetTCR0(uint32_t value)
{
	tcr0 = value & CONFIG::COUNTER_MASK;
}

template <class CONFIG>
void XPS_Timer<CONFIG>::SetTCSR1(uint32_t value)
{
	if(!CONFIG::C_ONE_TIMER_ONLY)
	{
		// write value in TCSR1, preserve T1INT
		tcsr1 = (tcsr1 & CONFIG::TCSR1_T1INT_MASK) | (value & ~CONFIG::TCSR1_T1INT_MASK & CONFIG::TCSR1_MASK);
		
		if(value & CONFIG::TCSR1_T1INT_MASK)
		{
			// clear T1INT
			tcsr1 = tcsr1 & ~CONFIG::TCSR1_T1INT_MASK;
		}
		
		if(tcsr1 & CONFIG::TCSR1_ENALL_MASK)
		{
			// set ENT0 and ENT1
			tcsr0 = tcsr0 | CONFIG::TCSR0_ENT0_MASK;
			tcsr1 = tcsr1 | CONFIG::TCSR1_ENT1_MASK;
		}

		// mirror ENALL in TCSR0
		tcsr0 = (tcsr0 & ~CONFIG::TCSR0_ENALL_MASK) | (tcsr1 & CONFIG::TCSR1_ENALL_MASK);
	}
}

template <class CONFIG>
void XPS_Timer<CONFIG>::SetTCSR1_T1INT(uint32_t value)
{
	SetTCSR1((GetTCSR1() & ~CONFIG::TCSR1_T1INT_MASK) | ((value << CONFIG::TCSR1_T1INT_OFFSET) & CONFIG::TCSR1_T1INT_MASK));
}

template <class CONFIG>
void XPS_Timer<CONFIG>::SetTLR1(uint32_t value)
{
	tlr1 = value & CONFIG::COUNTER_MASK;
}

template <class CONFIG>
void XPS_Timer<CONFIG>::SetTCR1(uint32_t value)
{
	if(!CONFIG::C_ONE_TIMER_ONLY)
	{
		tcr1 = value & CONFIG::COUNTER_MASK;
	}
}

template <class CONFIG>
uint32_t XPS_Timer<CONFIG>::GetTCSR0_ENALL() const
{
	return (GetTCSR0() & CONFIG::TCSR0_ENALL_MASK) >> CONFIG::TCSR0_ENALL_OFFSET;
}

template <class CONFIG>
uint32_t XPS_Timer<CONFIG>::GetTCSR0_PWMA0() const
{
	return (GetTCSR0() & CONFIG::TCSR0_PWMA0_MASK) >> CONFIG::TCSR0_PWMA0_OFFSET;
}

template <class CONFIG>
uint32_t XPS_Timer<CONFIG>::GetTCSR0_T0INT() const
{
	return (GetTCSR0() & CONFIG::TCSR0_T0INT_MASK) >> CONFIG::TCSR0_T0INT_OFFSET;
}

template <class CONFIG>
uint32_t XPS_Timer<CONFIG>::GetTCSR0_ENT0() const
{
	return (GetTCSR0() & CONFIG::TCSR0_ENT0_MASK) >> CONFIG::TCSR0_ENT0_OFFSET;
}

template <class CONFIG>
uint32_t XPS_Timer<CONFIG>::GetTCSR0_LOAD0() const
{
	return (GetTCSR0() & CONFIG::TCSR0_LOAD0_MASK) >> CONFIG::TCSR0_LOAD0_OFFSET;
}

template <class CONFIG>
uint32_t XPS_Timer<CONFIG>::GetTCSR0_ARHT0() const
{
	return (GetTCSR0() & CONFIG::TCSR0_ARHT0_MASK) >> CONFIG::TCSR0_ARHT0_OFFSET;
}

template <class CONFIG>
uint32_t XPS_Timer<CONFIG>::GetTCSR0_CAPT0() const
{
	return (GetTCSR0() & CONFIG::TCSR0_CAPT0_MASK) >> CONFIG::TCSR0_CAPT0_OFFSET;
}

template <class CONFIG>
uint32_t XPS_Timer<CONFIG>::GetTCSR0_GENT0() const
{
	return (GetTCSR0() & CONFIG::TCSR0_GENT0_MASK) >> CONFIG::TCSR0_GENT0_OFFSET;
}

template <class CONFIG>
uint32_t XPS_Timer<CONFIG>::GetTCSR0_UDT0() const
{
	return (GetTCSR0() & CONFIG::TCSR0_UDT0_MASK) >> CONFIG::TCSR0_UDT0_OFFSET;
}

template <class CONFIG>
uint32_t XPS_Timer<CONFIG>::GetTCSR0_MDT0() const
{
	return (GetTCSR0() & CONFIG::TCSR0_MDT0_MASK) >> CONFIG::TCSR0_MDT0_OFFSET;
}

template <class CONFIG>
uint32_t XPS_Timer<CONFIG>::GetTCSR1_ENALL() const
{
	return (GetTCSR1() & CONFIG::TCSR1_ENALL_MASK) >> CONFIG::TCSR1_ENALL_OFFSET;
}

template <class CONFIG>
uint32_t XPS_Timer<CONFIG>::GetTCSR1_PWMB0() const
{
	return (GetTCSR1() & CONFIG::TCSR1_PWMB0_MASK) >> CONFIG::TCSR1_PWMB0_OFFSET;
}

template <class CONFIG>
uint32_t XPS_Timer<CONFIG>::GetTCSR1_T1INT() const
{
	return (GetTCSR1() & CONFIG::TCSR1_T1INT_MASK) >> CONFIG::TCSR1_T1INT_OFFSET;
}

template <class CONFIG>
uint32_t XPS_Timer<CONFIG>::GetTCSR1_ENT1() const
{
	return (GetTCSR1() & CONFIG::TCSR1_ENT1_MASK) >> CONFIG::TCSR1_ENT1_OFFSET;
}

template <class CONFIG>
uint32_t XPS_Timer<CONFIG>::GetTCSR1_EINT1() const
{
	return (GetTCSR1() & CONFIG::TCSR1_EINT1_MASK) >> CONFIG::TCSR1_EINT1_OFFSET;
}

template <class CONFIG>
uint32_t XPS_Timer<CONFIG>::GetTCSR1_LOAD1() const
{
	return (GetTCSR1() & CONFIG::TCSR1_LOAD1_MASK) >> CONFIG::TCSR1_LOAD1_OFFSET;
}

template <class CONFIG>
uint32_t XPS_Timer<CONFIG>::GetTCSR1_ARHT1() const
{
	return (GetTCSR1() & CONFIG::TCSR1_ARHT1_MASK) >> CONFIG::TCSR1_ARHT1_OFFSET;
}

template <class CONFIG>
uint32_t XPS_Timer<CONFIG>::GetTCSR1_CAPT1() const
{
	return (GetTCSR1() & CONFIG::TCSR1_CAPT1_MASK) >> CONFIG::TCSR1_CAPT1_OFFSET;
}

template <class CONFIG>
uint32_t XPS_Timer<CONFIG>::GetTCSR1_UDT1() const
{
	return (GetTCSR1() & CONFIG::TCSR1_UDT1_MASK) >> CONFIG::TCSR1_UDT1_OFFSET;
}

template <class CONFIG>
uint32_t XPS_Timer<CONFIG>::GetTCSR1_MDT1() const
{
	return (GetTCSR1() & CONFIG::TCSR1_MDT1_MASK) >> CONFIG::TCSR1_MDT1_OFFSET;
}

template <class CONFIG>
bool XPS_Timer<CONFIG>::IsVerbose() const
{
	return verbose;
}

} // end of namespace xps_timer
} // end of namespace xilinx
} // end of namespace timer
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
