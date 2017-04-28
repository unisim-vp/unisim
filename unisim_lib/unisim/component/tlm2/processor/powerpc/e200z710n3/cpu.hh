/*
 *  Copyright (c) 2007-2010,
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

#ifndef __UNISIM_COMPONENT_TLM2_PROCESSOR_POWERPC_E200Z710N3_CPU_HH__
#define __UNISIM_COMPONENT_TLM2_PROCESSOR_POWERPC_E200Z710N3_CPU_HH__

#include <unisim/component/cxx/processor/powerpc/e200z710n3/cpu.hh>
#include <systemc.h>
#include <unisim/kernel/service/service.hh>
#include <unisim/kernel/logger/logger.hh>
#include <unisim/kernel/tlm2/tlm.hh>
#include <inttypes.h>
#include <stack>
#include <vector>

namespace unisim {
namespace component {
namespace tlm2 {
namespace processor {
namespace powerpc {
namespace e200z710n3 {

typedef unisim::component::cxx::processor::powerpc::e200z710n3::PHYSICAL_ADDRESS PHYSICAL_ADDRESS;
typedef unisim::component::cxx::processor::powerpc::e200z710n3::STORAGE_ATTR STORAGE_ATTR;

using unisim::kernel::tlm2::PayloadFabric;
using unisim::kernel::service::Object;
using unisim::kernel::service::Client;
using unisim::kernel::service::Parameter;
using unisim::kernel::service::Statistic;
using unisim::kernel::service::Formula;
using unisim::kernel::logger::Logger;

class CPU
	: public sc_module
	, public unisim::component::cxx::processor::powerpc::e200z710n3::CPU
	, public tlm::tlm_bw_transport_if<>
{
public:
	
	typedef unisim::component::cxx::processor::powerpc::e200z710n3::CPU Super;
	typedef tlm::tlm_initiator_socket<unisim::component::cxx::processor::powerpc::e200z710n3::CONFIG::FSB_WIDTH * 8> ahb_if_type;

	ahb_if_type ahb_if; // master interface
	sc_core::sc_in<bool>                m_por;            // power-on reset
	sc_core::sc_in<bool>                p_reset_b;        // reset
	sc_core::sc_in<bool>                p_nmi_b;          // NMI
	sc_core::sc_in<bool>                p_mcp_b;          // MCP
	sc_core::sc_in<sc_dt::sc_uint<30> > p_rstbase;        // reset base
	sc_core::sc_in<sc_dt::sc_uint<8> >  p_cpuid;          // CPU ID
	sc_core::sc_in<bool>                p_extint_b;       // external interrupt
	sc_core::sc_in<bool>                p_crint_b;        // critical interrupt
	sc_core::sc_in<bool>                p_avec_b;         // autovector
	sc_core::sc_in<sc_dt::sc_uint<14> > p_voffset;        // Interrupt vector offset for vectored interrupts
	sc_core::sc_out<bool>               p_iack;           // interrupt acknowledge
	
	CPU(const sc_module_name& name, Object *parent = 0);
	virtual ~CPU();
	
	virtual bool EndSetup();

	// Back path
	virtual tlm::tlm_sync_enum nb_transport_bw(tlm::tlm_generic_payload& payload, tlm::tlm_phase& phase, sc_core::sc_time& t);
	virtual void invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range);
	
	virtual void Synchronize();
	virtual void Idle();
	virtual void InterruptAcknowledge();
	
	void P_RESET_B_Process();
	void P_NMI_B_Process();
	void P_MCP_B_Process();
	void P_EXTIN_B_Process();
	void P_CRINT_B_Process();
	void P_AVEC_B_Process();
	void P_IACK_Process();
	void Run();
	
protected:
	sc_time GetBurstLatency(uint32_t size, const sc_time& latency) const;
	virtual bool AHBInsnRead(PHYSICAL_ADDRESS physical_addr, void *buffer, uint32_t size, STORAGE_ATTR storage_attr);
	virtual bool AHBDataRead(PHYSICAL_ADDRESS physical_addr, void *buffer, uint32_t size, STORAGE_ATTR storage_attr, bool rwitm);
	virtual bool AHBDataWrite(PHYSICAL_ADDRESS physical_addr, const void *buffer, uint32_t size, STORAGE_ATTR storage_attr);

	virtual bool AHBDebugInsnRead(PHYSICAL_ADDRESS physical_addr, void *buffer, uint32_t size, STORAGE_ATTR storage_attr);
	virtual bool AHBDebugDataRead(PHYSICAL_ADDRESS physical_addr, void *buffer, uint32_t size, STORAGE_ATTR storage_attr);
	virtual bool AHBDebugDataWrite(PHYSICAL_ADDRESS physical_addr, const void *buffer, uint32_t size, STORAGE_ATTR storage_attr);
private:
	PayloadFabric<tlm::tlm_generic_payload> payload_fabric;
	sc_time cpu_cycle_time;         //<! CPU core cycle time
	sc_time bus_cycle_time;         //<! Bus (AHB) cycle time
	sc_time cpu_time;               //<! local time (relative to sc_time_stamp)
	sc_time timer_time;             //<! absolute time from the internal timers point of view
	sc_time nice_time;              //<! period of synchronization with other threads
	sc_time run_time;               //<! absolute timer (local time + sc_time_stamp)
	sc_time idle_time;              //<! total idle time
	bool enable_host_idle;
	sc_event start_event;
	sc_event external_event;
	sc_event int_ack_event;
	double ipc;
	double one;
	bool enable_dmi;
	bool debug_dmi;

	Parameter<sc_time> param_cpu_cycle_time;
	Parameter<sc_time> param_bus_cycle_time;
	Parameter<sc_time> param_nice_time;
	Parameter<double> param_ipc;
	Parameter<bool> param_enable_host_idle;
	Parameter<bool> param_enable_dmi;
	Parameter<bool> param_debug_dmi;
	Statistic<double> stat_one;
	Statistic<sc_time> stat_run_time;
	Statistic<sc_time> stat_idle_time;
	Formula<double> formula_idle_rate;
	Formula<double> formula_load_rate;
	
	unisim::kernel::tlm2::DMIRegionCache dmi_region_cache;

	inline void AlignToBusClock() ALWAYS_INLINE;
	void AlignToBusClock(sc_time& t);
};

} // end of namespace e200z710n3
} // end of namespace powerpc
} // end of namespace processor
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif
