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
 * Authors: Reda Nouacer (reda.nouacer@cea.fr)
 */
 
#ifndef __UNISIM_COMPONENT_TLM_PROCESSOR_HCS12X_HH__
#define __UNISIM_COMPONENT_TLM_PROCESSOR_HCS12X_HH__

#include <systemc.h>
#include "unisim/component/cxx/processor/hcs12x/hcs12x.hh"
#include "unisim/component/tlm/message/simple_fsb.hh"
#include "unisim/kernel/tlm/tlm.hh"
#include "unisim/util/garbage_collector/garbage_collector.hh"
#include <inttypes.h>
#include <string>

namespace unisim {
namespace component {
namespace tlm {
namespace processor {
namespace hcs12x {

using unisim::component::cxx::processor::hcs12x::physical_address_t;
using unisim::component::cxx::processor::hcs12x::CPU;
using unisim::kernel::service::Parameter;
using unisim::kernel::service::Object;
using unisim::kernel::service::Client;
using unisim::kernel::service::Service;
using unisim::kernel::service::ServiceExport;
using unisim::kernel::service::ServiceImport;
using unisim::kernel::tlm::TlmMessage;
using unisim::kernel::tlm::TlmSendIf;
using unisim::util::garbage_collector::Pointer;
using unisim::component::tlm::message::SimpleFSBRequest;
using unisim::component::tlm::message::SimpleFSBResponse;

using std::string;

class HCS12X :
	public sc_module,
	public CPU {
public:
	typedef CPU inherited;
    static const uint32_t FSB_WIDTH = 32; // (should 2 bytes!)

	// Bus port
	sc_port<TlmSendIf<SimpleFSBRequest<physical_address_t, FSB_WIDTH>, SimpleFSBResponse<FSB_WIDTH> > > master_port;
	
	HCS12X(const sc_module_name& name, Object *parent = 0);
	virtual ~HCS12X();
	
	virtual void Stop(int ret);
	virtual void Sync();
	
	virtual bool Setup();

	void BusSynchronize();
	
	void Run();

	virtual void Reset();
	
	// cache interface implemented by the arm processor to get the request from the caches
	virtual void BusWrite(physical_address_t addr, const uint8_t *buffer, uint32_t size);
	virtual void BusRead(physical_address_t addr, uint8_t *buffer, uint32_t size);


private:
	void Synchronize();
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
	
	uint64_t last_instruction_counter;
	
};

} // end of namespace hcs12x
} // end of namespace processor
} // end of namespace tlm
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM_PROCESSOR_HCS12X_HH__
