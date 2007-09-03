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
 
#ifndef __FULLSYSTEM_SHARED_MEMORY_SIMPLE_BUS_ARM_HH__
#define __FULLSYSTEM_SHARED_MEMORY_SIMPLE_BUS_ARM_HH__

#include <systemc.h>
#include "processors/arm/armcpu.hh"
#include "generic/cache/cache_interface.hh"
#include "tlm/shared_memory/snooping_bus/message.hh"
#include "tlm/tlm.hh"

namespace full_system {
namespace tlm {
namespace shared_memory {
namespace simple_bus {

using namespace full_system::processors::arm;
using full_system::generic::bus::BS_OK;
using full_system::generic::bus::CS_SHARED;
using full_system::generic::bus::CS_MISS;
using full_system::tlm::TlmMessage;
using full_system::tlm::TlmSendIf;
using full_system::utils::garbage_collector::Pointer;
using full_system::utils::services::Parameter;
using full_system::tlm::shared_memory::snooping_bus::Request;
using full_system::tlm::shared_memory::snooping_bus::Response;
using full_system::generic::cache::CacheInterface;
using full_system::generic::cache::CacheControl;

template <class CONFIG>
class ARM :
	public sc_module,
	public full_system::processors::arm::CPU<CONFIG>,
	public CacheInterface<typename CONFIG::address_t>
{
public:
	typedef typename CONFIG::address_t address_t;
	typedef full_system::processors::arm::CPU<CONFIG> inherited;
	
	// Bus port
	sc_port<TlmSendIf<Request<physical_address_t, CONFIG::FSB_BURST_SIZE>, Response<CONFIG::FSB_BURST_SIZE> > > bus_port;
	
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

} // end of namespace simple_bus
} // end of namespace shared_memory
} // end of namespace tlm
} // end of namespace full_system

#endif
