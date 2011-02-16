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

#ifndef __UNISIM_COMPONENT_CXX_TIMER_XILINX_XPS_TIMER_XPS_TIMER_HH__
#define __UNISIM_COMPONENT_CXX_TIMER_XILINX_XPS_TIMER_XPS_TIMER_HH__

#include <unisim/kernel/service/service.hh>
#include <unisim/kernel/logger/logger.hh>

#include <unisim/service/interfaces/memory.hh>

namespace unisim {
namespace component {
namespace cxx {
namespace timer {
namespace xilinx {
namespace xps_timer {

using unisim::kernel::service::Object;
using unisim::kernel::service::Parameter;
using unisim::kernel::service::Service;
using unisim::kernel::service::ServiceExport;
using unisim::kernel::service::ServiceExportBase;
using unisim::service::interfaces::Memory;

template <class CONFIG>
class XPS_Timer
	: public Service<Memory<typename CONFIG::MEMORY_ADDR> >
{
public:
	ServiceExport<Memory<typename CONFIG::MEMORY_ADDR> > memory_export;
	
	XPS_Timer(const char *name, Object *parent = 0);
	virtual ~XPS_Timer();
	
	virtual bool BeginSetup();

	virtual void Reset();
	virtual bool ReadMemory(typename CONFIG::MEMORY_ADDR addr, void *buffer, uint32_t size);
	virtual bool WriteMemory(typename CONFIG::MEMORY_ADDR addr, const void *buffer, uint32_t size);
	virtual bool ReadMemory(typename CONFIG::MEMORY_ADDR addr, void *buffer, uint32_t size, const uint8_t *byte_enable, uint32_t byte_enable_length, uint32_t streaming_width);
	virtual bool WriteMemory(typename CONFIG::MEMORY_ADDR addr, const void *buffer, uint32_t size, const uint8_t *byte_enable, uint32_t byte_enable_length, uint32_t streaming_width);
	
	template <typename T> void Read(typename CONFIG::MEMORY_ADDR addr, T& value);
	template <typename T> void Write(typename CONFIG::MEMORY_ADDR addr, T value);
	bool IsMapped(typename CONFIG::MEMORY_ADDR addr, uint32_t size) const;
protected:
	unisim::kernel::logger::Logger logger;
	bool verbose;
	bool IsVerbose() const;
private:
	uint32_t tcsr0;   // Control/Status Register 0
	uint32_t tlr0;    // Load register 0
	uint32_t tcr0;    // Timer/Counter Register 0
	uint32_t tcsr1;   // Control/Status Register 1
	uint32_t tlr1;    // Load register 1
	uint32_t tcr1;    // Timer/Counter Register 1
	
	uint32_t tcsr0_toggle;
	uint32_t tcsr1_toggle;
	bool tcr0_toggle;
	bool tcr1_toggle;
protected:
	uint32_t GetTCSR0() const;
	uint32_t GetTLR0() const;
	uint32_t GetTCR0() const;
	uint32_t GetTCSR1() const;
	uint32_t GetTLR1() const;
	uint32_t GetTCR1() const;
	
	void SetTCSR0(uint32_t value);
	void SetTLR0(uint32_t value);
	void SetTCR0(uint32_t value);
	void SetTCSR1(uint32_t value);
	void SetTLR1(uint32_t value);
	void SetTCR1(uint32_t value);
	void SetTCSR0_T0INT(uint32_t value);
	void SetTCSR1_T1INT(uint32_t value);
	
	uint32_t GetTCSR0_ENALL() const;
	uint32_t GetTCSR0_PWMA0() const;
	uint32_t GetTCSR0_T0INT() const;
	uint32_t GetTCSR0_ENT0() const;
	uint32_t GetTCSR0_LOAD0() const;
	uint32_t GetTCSR0_ARHT0() const;
	uint32_t GetTCSR0_CAPT0() const;
	uint32_t GetTCSR0_GENT0() const;
	uint32_t GetTCSR0_UDT0() const;
	uint32_t GetTCSR0_MDT0() const;
	
	uint32_t GetTCSR1_ENALL() const;
	uint32_t GetTCSR1_PWMB0() const;
	uint32_t GetTCSR1_T1INT() const;
	uint32_t GetTCSR1_ENT1() const;
	uint32_t GetTCSR1_EINT1() const;
	uint32_t GetTCSR1_LOAD1() const;
	uint32_t GetTCSR1_ARHT1() const;
	uint32_t GetTCSR1_CAPT1() const;
	uint32_t GetTCSR1_UDT1() const;
	uint32_t GetTCSR1_MDT1() const;
	
	Parameter<bool> param_verbose;
};

} // end of namespace xps_timer
} // end of namespace xilinx
} // end of namespace timer
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
