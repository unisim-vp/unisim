/*
 *  Copyright (c) 2007,
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
 * Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 */
 
#ifndef __UNISIM_COMPONENT_TLM_PROCESSOR_ARM_HH__
#define __UNISIM_COMPONENT_TLM_PROCESSOR_ARM_HH__

#include <systemc.h>
#include "unisim/component/cxx/processor/arm/armcpu.hh"
#include "unisim/component/cxx/cache/cache_interface.hh"
#include "unisim/component/tlm/message/simple_fsb.hh"
#include "unisim/kernel/tlm/tlm.hh"
#include "unisim/util/garbage_collector/garbage_collector.hh"
#include <inttypes.h>

namespace unisim {
namespace component {
namespace tlm {
namespace processor {
namespace arm {

using unisim::component::cxx::processor::arm::CPU;
using unisim::kernel::service::Parameter;
using unisim::kernel::tlm::TlmMessage;
using unisim::kernel::tlm::TlmSendIf;
using unisim::util::garbage_collector::Pointer;
using unisim::component::tlm::message::SimpleFSBRequest;
using unisim::component::tlm::message::SimpleFSBResponse;
using unisim::component::cxx::cache::CacheInterface;
using unisim::component::cxx::cache::CacheControl;

template <class CONFIG>
class ARM :
	public sc_module,
	public CPU<CONFIG>,
	public CacheInterface<typename CONFIG::address_t>
{
public:
	typedef typename CONFIG::address_t address_t;
	typedef CPU<CONFIG> inherited;
	
	// Bus port
	sc_port<TlmSendIf<SimpleFSBRequest<address_t, CONFIG::FSB_BURST_SIZE>, Response<CONFIG::FSB_BURST_SIZE> > > master_port;
	
	ARM(const sc_module_name& name, Object *parent = 0);
	virtual ~ARM();
	
	virtual void Stop(int ret);
	virtual void Sync();
	
	virtual bool Setup();

	void BusSynchronize();
	
	void Run();

	virtual void Reset();
	
	virtual void PrInvalidateBlock(address_t addr, 
			CacheControl cc);
	virtual void PrFlushBlock(address_t addr, 
			CacheControl cc);
	virtual void PrZeroBlock(address_t addr, 
			CacheControl cc);
	virtual void PrWrite(address_t addr, 
			const void *buffer, 
			uint32_t size, 
			CacheControl cc);
	virtual void PrRead(address_t addr, 
			void *buffer, 
			uint32_t size, 
			CacheControl cc);
	virtual void PrReadX(address_t addr, 
			void *buffer, 
			uint32_t size, 
			CacheControl cc);
	virtual void BusInvalidateBlock(address_t addr);
	virtual void BusFlushBlock(address_t addr);
	virtual void BusRead(address_t addr, 
			void *buffer, uint32_t size);
	virtual void BusReadX(address_t addr, 
			void *buffer, 
			uint32_t size);

private:
	bool DebugEnable();
	
	sc_time cpu_cycle_time;
	sc_time bus_cycle_time;
	sc_time cpu_time;
	sc_time bus_time;
	sc_time last_cpu_time;
	sc_time nice_time;
	sc_time next_nice_time;
	uint64_t nice_time_int;
	double ipc;
	uint64_t cpu_cycle_time_int;
	uint64_t bus_cycle_time_int;
	
	Parameter<uint64_t> param_nice_time;
	Parameter<double> param_ipc;
	Parameter<uint64_t> param_cpu_cycle_time;
	Parameter<uint64_t> param_bus_cycle_time;
	
	// verbose parameters
	bool verbose_tlm_bus_synchronize;
	Parameter<bool> param_verbose_tlm_bus_synchronize;
	bool verbose_tlm_run_thread;
	Parameter<bool> param_verbose_tlm_run_thread;
	bool verbose_tlm_commands;
	Parameter<bool> param_verbose_tlm_commands;
};

} // end of namespace arm
} // end of namespace processor
} // end of namespace tlm
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM_PROCESSOR_ARM_HH__
