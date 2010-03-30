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
 * Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 */
 
#ifndef __UNISIM_COMPONENT_TLM2_PROCESSOR_ARM_HH__
#define __UNISIM_COMPONENT_TLM2_PROCESSOR_ARM_HH__

#include <systemc.h>
#include <tlm.h>
#include <tlm_utils/passthrough_target_socket.h>
#include <tlm_utils/peq_with_get.h>
#include "unisim/component/cxx/processor/arm/cpu.hh"
#include "unisim/component/cxx/processor/arm/cache_interface.hh"
#include "unisim/kernel/tlm2/tlm.hh"
#include "unisim/component/tlm2/interrupt/types.hh"
#include <inttypes.h>
#include <string>

namespace unisim {
namespace component {
namespace tlm2 {
namespace processor {
namespace arm {

using unisim::component::cxx::processor::arm::CPU;
using unisim::component::tlm2::interrupt::InterruptProtocolTypes;
using unisim::component::tlm2::interrupt::TLMInterruptPayload;
using unisim::kernel::service::Parameter;
using unisim::kernel::service::Object;
using unisim::kernel::service::Client;
using unisim::kernel::service::Service;
using unisim::kernel::service::ServiceExport;
using unisim::kernel::service::ServiceImport;
using unisim::component::cxx::processor::arm::CacheInterface;

using std::string;

template <class CONFIG, bool BLOCKING = false>
class ARM :
	public sc_module,
	public tlm::tlm_bw_transport_if<>,
	public CPU<CONFIG>,
	public CacheInterface<typename CONFIG::address_t> {
public:
	typedef tlm::tlm_base_protocol_types::tlm_payload_type  transaction_type;
	typedef tlm::tlm_base_protocol_types::tlm_phase_type    phase_type;
	typedef tlm::tlm_sync_enum     sync_enum_type;
	
	typedef typename CONFIG::address_t address_t;
	typedef CPU<CONFIG> inherited;
	typedef ARM<CONFIG, BLOCKING> THIS_MODULE;

	/*****************************************************************************************************
	 * Port to the bus and its virtual methods to handle incomming calls.                          START *
	 *****************************************************************************************************/

	// Master port to the bus port
	tlm::tlm_initiator_socket<CONFIG::FSB_BURST_SIZE> master_socket;
	
private:
	// virtual method implementation to handle backward path of transactions sent through the master_port
	virtual sync_enum_type nb_transport_bw(transaction_type &trans, phase_type &phase, sc_core::sc_time &time);
	// virtual method implementation to handle backward path of the dmi mechanism
	virtual void invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range);

	/*****************************************************************************************************
	 * Port to the bus and its virtual methods to handle incomming calls.                            END *
	 *****************************************************************************************************/

	/*****************************************************************************************************
	 * Incomming ports for the incomming interrupts                                                START *
	 *****************************************************************************************************/

public:
	// incomming port for the irq interrupts
	tlm_utils::passthrough_target_socket<THIS_MODULE, 1, InterruptProtocolTypes> in_irq;

private:
	sync_enum_type IrqNbTransportFw(TLMInterruptPayload& trans, phase_type &phase, sc_core::sc_time &time);
	void IrqBTransport(TLMInterruptPayload& trans, sc_core::sc_time &time);
	unsigned int IrqTransportDbg(TLMInterruptPayload& trans);
	bool IrqGetDirectMemPtr(TLMInterruptPayload& trans, tlm::tlm_dmi& dmi);

public:
	// incomming port for the fiq interrupts
	tlm_utils::passthrough_target_socket<THIS_MODULE, 1, InterruptProtocolTypes> in_fiq;

private:
	sync_enum_type FiqNbTransportFw(TLMInterruptPayload& trans, phase_type &phase, sc_core::sc_time &time);
	void FiqBTransport(TLMInterruptPayload& trans, sc_core::sc_time &time);
	unsigned int FiqTransportDbg(TLMInterruptPayload& trans);
	bool FiqGetDirectMemPtr(TLMInterruptPayload& trans, tlm::tlm_dmi& dmi);

	/*****************************************************************************************************
	 * Incomming ports for the incomming interrupts                                                  END *
	 *****************************************************************************************************/

	/*****************************************************************************************************
	 * Interrupt queue and handler                                                                 START *
	 *****************************************************************************************************/
	
	typedef struct
	{
		bool fiq;
		bool level;
	} interrupt_t;
	std::queue<interrupt_t *> free_interrupt_queue;
	tlm_utils::peq_with_get<interrupt_t> interrupt_queue;
	/** Thread too handle incomming interrupts */
	void InterruptHandler();

	/*****************************************************************************************************
	 * Interrupt queue and handler                                                                   END *
	 *****************************************************************************************************/
public:
	SC_HAS_PROCESS(ARM);
	ARM(const sc_module_name& name, Object *parent = 0);
	virtual ~ARM();

	virtual void Stop(int ret);
	virtual void Sync();
	
	virtual bool Setup();

	void BusSynchronize();
	
	void Run();

	virtual void Reset();
	
	// cache interface implemented by the arm processor to get the request from the caches
	virtual void PrWrite(address_t addr, const uint8_t *buffer, uint32_t size);
	virtual void PrRead(address_t addr, uint8_t *buffer, uint32_t size);
	// TODO: Check if the following methods can be removed somehow
	// the following instruction exist in the arm cache interface but they can not be used 
	virtual void SetLock(uint32_t lock, uint32_t set);
	virtual void PrInvalidateBlock(uint32_t set, uint32_t way);
	virtual void PrFlushBlock(uint32_t set, uint32_t way);
	virtual void PrCleanBlock(uint32_t set, uint32_t way);
	virtual uint32_t GetCacheSize();
	virtual uint32_t GetCacheAssociativity();
	virtual uint32_t GetCacheBlockSize();
	virtual void Enable();
	virtual void Disable();
	virtual bool IsEnabled();
	virtual void PrReset();
	virtual void PrInvalidate();
	virtual void PrInvalidateSet(uint32_t set);
	virtual void PrInvalidateBlock(address_t addr);
	virtual void PrFlushBlock(address_t addr);
	virtual void PrCleanBlock(address_t addr);
	virtual void PrZeroBlock(address_t addr);
	// end of TODO

private:
	/* non intrusive memory methods that the inherited cpu uses and that should
	 * be redefined to access the tlm2 debug interface */
	virtual bool ExternalReadMemory(uint64_t addr, void *buffer, uint32_t size);
	virtual bool ExternalWriteMemory(uint64_t addr, const void *buffer, uint32_t size);
	
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
//	uint64_t cpu_cycle_time_int;
	uint64_t bus_cycle_time_int;
	
	Parameter<uint64_t> param_nice_time;
	Parameter<double> param_ipc;
//	Parameter<uint64_t> param_cpu_cycle_time;
	Parameter<uint64_t> param_bus_cycle_time;
	
	/*************************************************************************
	 * Logger, verbose and trap parameters/methods/ports               START *
	 *************************************************************************/

	//unisim::kernel::logger::Logger logger;
	bool verbose_all;
	unisim::kernel::service::Parameter<bool> param_verbose_all;
	bool verbose_setup;
	unisim::kernel::service::Parameter<bool> param_verbose_setup;
	inline bool VerboseSetup();
	/* TODO: remove this verbose commands */
	bool verbose_tlm;
	unisim::kernel::service::Parameter<bool> param_verbose_tlm;
	inline bool VerboseTLM();

	bool verbose_tlm_bus_synchronize;
	unisim::kernel::service::Parameter<bool> param_verbose_tlm_bus_synchronize;
	bool verbose_tlm_run_thread;
	unisim::kernel::service::Parameter<bool> param_verbose_tlm_run_thread;
	bool verbose_tlm_commands;
	unisim::kernel::service::Parameter<bool> param_verbose_tlm_commands;
	/* End of TODO */
	bool verbose_tlm_irq;
	unisim::kernel::service::Parameter<bool> param_verbose_tlm_irq;
	inline bool VerboseTLMIrq();
	bool trap_on_verbose_tlm_irq;
	unisim::kernel::service::Parameter<bool> param_trap_on_verbose_tlm_irq;
	inline void TrapOnVerboseTLMIrq();

	/*************************************************************************
	 * Logger, verbose and trap parameters/methods/ports                 END *
	 *************************************************************************/
};

} // end of namespace arm
} // end of namespace processor
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_PROCESSOR_ARM_HH__
