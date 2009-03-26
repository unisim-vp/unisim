/*
 *  Copyright (c) 2009,
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
 *          Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 */
 
#ifndef __UNISIM_COMPONENT_TLM2_PROCESSOR_TMS320_HH__
#define __UNISIM_COMPONENT_TLM2_PROCESSOR_TMS320_HH__

#include <systemc.h>
#include <tlm.h>
#include "unisim/component/cxx/processor/tms320/cpu.hh"
#include "unisim/kernel/tlm2/tlm.hh"
#include "unisim/kernel/logger/logger.hh"
#include <inttypes.h>
#include <string>

namespace unisim {
namespace component {
namespace tlm2 {
namespace processor {
namespace tms320 {

using unisim::component::cxx::processor::tms320::CPU;
using unisim::kernel::service::Parameter;
using unisim::kernel::service::Object;
using unisim::kernel::service::Client;
using unisim::kernel::service::Service;
using unisim::kernel::service::ServiceExport;
using unisim::kernel::service::ServiceImport;

using std::string;

template <class CONFIG, bool DEBUG = false>
class TMS320 :
	public sc_module,
	public tlm::tlm_bw_transport_if<>,
	public CPU<CONFIG, DEBUG> {
public:
	typedef tlm::tlm_base_protocol_types::tlm_payload_type  transaction_type;
	typedef tlm::tlm_base_protocol_types::tlm_phase_type    phase_type;
	typedef tlm::tlm_sync_enum     sync_enum_type;
	
	typedef typename CONFIG::address_t address_t;
	typedef CPU<CONFIG, DEBUG> inherited;

	/*****************************************************************************************************
	 * Port to the bus and its virtual methods to handle incomming calls.                          START *
	 *****************************************************************************************************/
	// Master port to the bus port
	tlm::tlm_initiator_socket<CONFIG::FSB_BURST_SIZE> master_socket;
	
	// virtual method implementation to handle backward path of transactions sent through the master_port
	virtual sync_enum_type nb_transport_bw(transaction_type &trans, phase_type &phase, sc_core::sc_time &time);
	// virtual method implementation to handle backward path of the dmi mechanism
	virtual void invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range);

	/*****************************************************************************************************
	 * Port to the bus and its virtual methods to handle incomming calls.                            END *
	 *****************************************************************************************************/
	
	SC_HAS_PROCESS(TMS320);
	TMS320(const sc_module_name& name, Object *parent = 0);
	virtual ~TMS320();

	virtual void Stop(int ret);
	virtual void Sync();
	
	virtual bool Setup();

	void BusSynchronize();
	
	void Run();

	virtual void Reset();
	
	// cache interface implemented by the tms320 processor to get the request from the caches
	virtual bool PrWrite(address_t addr, const void *buffer, uint32_t size);
	virtual bool PrRead(address_t addr, void *buffer, uint32_t size);

private:
	/** Event used to signalize the end of a read transaction.
	 * Method PrRead waits for this event once the read transaction has been sent, and the 
	 *   nb_transport_bw notifies on it when the read transaction is finished. */
	sc_event end_read_rsp_event;
	
	unisim::kernel::tlm2::PayloadFabric<tlm::tlm_generic_payload> payload_fabric;

	/** A temporary variable that can be used anywhere in the code to compute a time.
	 * A temporary variable that can be used anywhere in the code to compute a time.
	 *   Should be initialized everytime it is used.
	 */
	sc_time tmp_time;
	
	sc_time cpu_cycle_time;
	sc_time bus_cycle_time;
	sc_time cpu_time;
	sc_time bus_time;
	sc_time quantum_time;
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
	
	/*************************************************************************
	 * Logger and verbose parameters/methods                           START *
	 *************************************************************************/

	unisim::kernel::logger::Logger logger;
	bool verbose_all;
	unisim::kernel::service::Parameter<bool> param_verbose_all;
	bool verbose_setup;
	unisim::kernel::service::Parameter<bool> param_verbose_setup;
	bool verbose_non_blocking;
	unisim::kernel::service::Parameter<bool> param_verbose_non_blocking;
	bool verbose_blocking;
	unisim::kernel::service::Parameter<bool> param_verbose_blocking;
	inline bool VerboseSetup();
	inline bool VerboseNonBlocking();
	inline bool VerboseBlocking();


	bool verbose_tlm_bus_synchronize;
	Parameter<bool> param_verbose_tlm_bus_synchronize;
	bool verbose_tlm_run_thread;
	Parameter<bool> param_verbose_tlm_run_thread;
	bool verbose_tlm_commands;
	Parameter<bool> param_verbose_tlm_commands;
	
	/*************************************************************************
	 * Logger and verbose parameters/methods                             END *
	 *************************************************************************/
};

} // end of namespace tms320
} // end of namespace processor
} // end of namespace tlm
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM_PROCESSOR_TMS320_HH__
