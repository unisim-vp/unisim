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
 
#ifndef __UNISIM_COMPONENT_TLM2_PROCESSOR_ARM_ARM926EJS_ARM926EJS_HH__
#define __UNISIM_COMPONENT_TLM2_PROCESSOR_ARM_ARM926EJS_ARM926EJS_HH__

#include <systemc.h>
#include <tlm.h>
#include "unisim/component/cxx/processor/arm/arm926ejs/cpu.hh"
#include "unisim/kernel/tlm2/tlm.hh"
#include <inttypes.h>

namespace unisim {
namespace component {
namespace tlm2 {
namespace processor {
namespace arm {
namespace arm926ejs {

class ARM926EJS
	: public sc_module
	, public tlm::tlm_bw_transport_if<>
	, public unisim::component::cxx::processor::arm::arm926ejs::CPU
{
public:
	typedef tlm::tlm_base_protocol_types::tlm_payload_type  transaction_type;
	typedef tlm::tlm_base_protocol_types::tlm_phase_type    phase_type;
	typedef tlm::tlm_sync_enum     sync_enum_type;
	
	typedef unisim::component::cxx::processor::arm::arm926ejs::CPU inherited;

	/**************************************************************************
	 * Port to the bus and its virtual methods to handle                START *
	 *   incomming calls.                                                     *
	 **************************************************************************/

	// Master port to the bus port
	tlm::tlm_initiator_socket<32> master_socket;

private:
	/** Non blocking backward method
	 *
	 * Virtual method implementation to handle backward path of transactions 
	 * sent through the master_port
	 *
	 * @param trans the transcation to handle
	 * @param phase the state of the transaction to handle (should only be 
	 *        END_REQ or BEGIN_RESP)
	 * @param time  the relative time at which the call is being done
	 *
	 * @return      the synchronization status
	 */
	virtual sync_enum_type nb_transport_bw(transaction_type &trans, 
			phase_type &phase, 
			sc_core::sc_time &time);
	/** Invalidate direct memory pointer
	 *
 	 * Virtual method implementation to handle backward path of dmi requests 
	 * sent through the master port.
	 * We do not use the dmi option in our simulator, so this method is 
	 * unnecessary. However, we have to declare it in order to be able to 
	 * compile the simulator.
	 *
	 * @param start_range the start address of the memory range to remove
	 * @param end_range   the end address of the memory range to remove
	 */
	virtual void invalidate_direct_mem_ptr(sc_dt::uint64 start_range, 
			sc_dt::uint64 end_range);

	/**************************************************************************
	 * Port to the bus and its virtual methods to handle                  END *
	 *   incomming calls.                                                     *
	 **************************************************************************/

	/**************************************************************************
	 * Interrupt ports and their handles                                START *
	 **************************************************************************/

public:
	// Slave port for the IRQ signal
	sc_core::sc_in<bool> nirq;
	// Slave port for the FIQ signal
	sc_core::sc_in<bool> nfiq;

private:
	/** nIRQ port handler */
	void IRQHandler();
	/** nFIQ port handler */
	void FIQHandler();
	
	/**************************************************************************
	 * Interrupt ports and their handles                                  END *
	 **************************************************************************/

public:
	SC_HAS_PROCESS(ARM926EJS);
	ARM926EJS(const sc_module_name& name, Object *parent = 0);
	virtual ~ARM926EJS();

	virtual void Stop(int ret);
	virtual void Sync();
	
	virtual bool Setup();

	void BusSynchronize();
	
	void Run();

	virtual void Reset();
	
	// cache interface implemented by the arm processor to get the request from 
	//   the caches
	virtual void PrWrite(uint32_t addr, const uint8_t *buffer, uint32_t size);
	virtual void PrRead(uint32_t addr, uint8_t *buffer, uint32_t size);

private:
	/** Non intrusive memory read method.
	 * Non intrusive memory read method that the inherited cpu uses and should 
	 *   be redefined to access the tlm2 debug interface.
	 *
	 * @param addr the address to read
	 * @param buffer a buffer to put the read data
	 * @param size the amount of data to read
	 *
	 * @return true on success, false otherwise
	 */
	virtual bool ExternalReadMemory(uint64_t addr, 
			void *buffer, 
			uint32_t size);
	/** Non intrusive memory write method.
	 * Non intrusive memory write method that the inherited cpu uses and should 
	 *   be redefined to access the tlm2 debug interface.
	 *
	 * @param addr the address to write
	 * @param buffer a buffer with the write data
	 * @param size the amount of data to read
	 *
	 * @return true on success, false otherwise
	 */
	virtual bool ExternalWriteMemory(uint64_t addr, 
			const void *buffer, 
			uint32_t size);
	
	/** Event used to signalize the end of a read transaction.
	 * Method PrRead waits for this event once the read transaction has been 
	 *   sent, and the nb_transport_bw notifies on it when the read transaction 
	 *   is finished. 
	 */
	sc_event end_read_rsp_event;
	
	unisim::kernel::tlm2::PayloadFabric<tlm::tlm_generic_payload>
		payload_fabric;

	/** A temporary variable used anywhere in the code to compute a time.
	 * A temporary variable that can be used anywhere in the code to compute a 
	 *   time.
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
	uint64_t bus_cycle_time_int;
	
	unisim::kernel::service::Parameter<uint64_t> param_nice_time;
	unisim::kernel::service::Parameter<double> param_ipc;
	unisim::kernel::service::Parameter<uint64_t> param_bus_cycle_time;
	
	/*************************************************************************
	 * Logger, verbose and trap parameters/methods/ports               START *
	 *************************************************************************/

	bool verbose_tlm;
	unisim::kernel::service::Parameter<bool> param_verbose_tlm;
	inline bool VerboseTLM();

	/*************************************************************************
	 * Logger, verbose and trap parameters/methods/ports                 END *
	 *************************************************************************/
};

} // end of namespace arm926ejs
} // end of namespace arm
} // end of namespace processor
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_PROCESSOR_ARM_ARM926EJS_ARM926EJS_HH__
