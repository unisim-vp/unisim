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
 
#ifndef __UNISIM_COMPONENT_TLM_PROCESSOR_TMS320_TCC__
#define __UNISIM_COMPONENT_TLM_PROCESSOR_TMS320_TCC__

#include <systemc.h>
#include <tlm.h>
#include "unisim/kernel/tlm2/tlm.hh"
#include "unisim/service/interfaces/logger.hh"

#define LOCATION 	" - location = " << __FUNCTION__ << ":unisim_lib/unisim/component/tlm2/processor/tms320/tms320.tcc:" << __LINE__
#define TIME(X) 	" - time = " << sc_time_stamp() + (X)
#define PHASE(X) 	" - phase = " << 	( (X) == tlm::BEGIN_REQ  ? 	"BEGIN_REQ" : \
										( (X) == tlm::END_REQ    ? 	"END_REQ" : \
										( (X) == tlm::BEGIN_RESP ? 	"BEGIN_RESP" : \
										( (X) == tlm::END_RESP   ?  "END_RESP" : \
										  							"UNINITIALIZED_PHASE"))))
#define TRANS(L,X) \
{ \
	(L) << " - trans = " << &(X) << endl \
		<< "   - " << ((X).is_read()?"read":"write") << endl \
		<< "   - address = 0x" << hex << (X).get_address() << dec << endl \
		<< "   - data_length = " << (X).get_data_length(); \
	if((X).is_write()) { \
		(L) << endl; \
		(L) << "   - data ="; \
		for(unsigned int _trans_i = 0; _trans_i < (X).get_data_length(); _trans_i++) { \
			(L) << " " << hex << (unsigned int)((X).get_data_ptr()[_trans_i]) << dec; \
		} \
	} \
}

#define ETRANS(L,X) \
{ \
	(L) << " - trans = " << &(X) << endl \
		<< "   - " << ((X).is_read()?"read":"write") << endl \
		<< "   - address = 0x" << hex << (X).get_address() << dec << endl \
		<< "   - data_length = " << (X).get_data_length() << endl \
	    << "   - response_status = "; \
	switch((X).get_response_status()) { \
	case tlm::TLM_OK_RESPONSE: \
		(L) << "TLM_OK_RESPONSE"; \
		break; \
	case tlm::TLM_INCOMPLETE_RESPONSE: \
		(L) << "TLM_INCOMPLETE_RESPONSE"; \
		break; \
	case tlm::TLM_GENERIC_ERROR_RESPONSE: \
		(L) << "TLM_GENERIC_ERROR_RESPONSE"; \
		break; \
	case tlm::TLM_ADDRESS_ERROR_RESPONSE: \
		(L) << "TLM_ADDRESS_ERROR_RESPONSE"; \
		break; \
	case tlm::TLM_COMMAND_ERROR_RESPONSE: \
		(L) << "TLM_COMMAND_ERROR_RESPONSE"; \
		break; \
	case tlm::TLM_BURST_ERROR_RESPONSE: \
		(L) << "TLM_BURST_ERROR_RESPONSE"; \
		break; \
	case tlm::TLM_BYTE_ENABLE_ERROR_RESPONSE: \
		(L) << "TLM_BYTE_ENABLE_ERROR_RESPONSE"; \
		break; \
	} \
	if((X).get_response_status() == tlm::TLM_OK_RESPONSE) { \
		(L) << endl; \
		(L) << "   - data ="; \
		for(unsigned int _trans_i = 0; _trans_i < (X).get_data_length(); _trans_i++) { \
			(L) << " " << hex << (unsigned int)((X).get_data_ptr()[_trans_i]) << dec; \
		} \
	} \
}

namespace unisim {
namespace component {
namespace tlm2 {
namespace processor {
namespace tms320 {

using namespace unisim::kernel::logger;

template<class CONFIG, bool DEBUG>
TMS320<CONFIG, DEBUG> :: 
TMS320(const sc_module_name& name, Object *parent) :
	Object(name, parent),
	sc_module(name),
	CPU<CONFIG, DEBUG>(name, parent),
	master_socket("master_socket"),
	cpu_cycle_time(),
	bus_cycle_time(),
	cpu_time(),
	bus_time(),
	quantum_time(),
	last_cpu_time(),
	nice_time(),
	next_nice_time(),
	nice_time_int(10),
	ipc(1.0),
	cpu_cycle_time_int(0),
	bus_cycle_time_int(0),
	param_nice_time("nice-time", this, nice_time_int),
	param_ipc("ipc", this, ipc),
	param_cpu_cycle_time("cpu-cycle-time", this, cpu_cycle_time_int),
	param_bus_cycle_time("bus-cycle-time", this, bus_cycle_time_int),
	logger(*this),
	verbose_all(false),
	param_verbose_all("verbose_all", this, verbose_all, "Activate all the verbose options"),
	verbose_setup(false),
	param_verbose_setup("verbose_setup", this, verbose_setup, "Display object setup information"),
	verbose_non_blocking(false),
	param_verbose_non_blocking("verbose_non_blocking", this, verbose_non_blocking, "Display non_blocking transaction handling"),
	verbose_blocking(false),
	param_verbose_blocking("verbose_blocking", this, verbose_blocking, "Display blocking transaction handling"),
	verbose_tlm_bus_synchronize(false),
	param_verbose_tlm_bus_synchronize("verbose_tlm_bus_synchronize", this, verbose_tlm_bus_synchronize, "Display bus synchronization handling"),
	verbose_tlm_run_thread(false),
	param_verbose_tlm_run_thread("verbose_tlm_run_thread", this, verbose_tlm_run_thread, "Display main thread execution information"),
	verbose_tlm_commands(false),
	param_verbose_tlm_commands("verbose_tlm_commands", this, verbose_tlm_commands, "Display commands handling")
{
	master_socket.bind(*this);
	
	SC_THREAD(Run);
}

template<class CONFIG, bool DEBUG>
TMS320<CONFIG, DEBUG> ::
~TMS320() 
{
}

/** Initialization of the module
 * Initializes the module depending on the parameters values.
 *
 * @return  true if the initialization suceeds, false otherwise
 */
template<class CONFIG, bool DEBUG>
bool 
TMS320<CONFIG, DEBUG> :: 
Setup() 
{
	if (!inherited::Setup()) 
	{
		logger << DebugError << LOCATION
			<< "Error while trying to set up the TMS320 cpu"
			<< EndDebugError;
		return false;
	}
	
	/* check verbose settings */
	if(DEBUG && inherited::verbose_all) {
		verbose_tlm_bus_synchronize = true;
		verbose_tlm_run_thread = true;
		verbose_tlm_commands = true;
		verbose_setup = true;
		verbose_blocking = true;
		verbose_non_blocking = true;
	} else {
		if(DEBUG && verbose_tlm_bus_synchronize)
			logger << DebugInfo << LOCATION
				<< "verbose-tlm-bus-synchronize = true"
				<< EndDebugInfo;		
		if(DEBUG && verbose_tlm_run_thread)
			logger << DebugInfo << LOCATION
				<< "verbose-tlm-run-thread = true"
				<< EndDebugInfo;		
	}
	
	cpu_cycle_time = sc_time((double)cpu_cycle_time_int, SC_PS);
	bus_cycle_time = sc_time((double)bus_cycle_time_int, SC_PS);
	nice_time = sc_time((double)nice_time_int, SC_PS);
	if(DEBUG && inherited::verbose_setup) {
		logger << DebugInfo << LOCATION
			<< "Setting CPU cycle time to " 
			<< cpu_cycle_time.to_string() << endl
			<< "Setting Bus cycle time to " 
			<< bus_cycle_time.to_string() << endl
			<< "Setting nice time to " << nice_time.to_string() << endl
			<< "Setting IPC to " << ipc << endl
			<< EndDebugInfo;
	}
	
	return true;
}

template<class CONFIG, bool DEBUG>
void 
TMS320<CONFIG, DEBUG> ::
Stop(int ret) 
{
	// Call BusSynchronize to account for the remaining time spent in the cpu 
	// core
	BusSynchronize();
	sc_stop();
	wait();
}

/** Synchronization demanded from the CPU implementation.
 * An example (an for the moment the only synchronization demanded by the CPU
 * implmentation) is the a synchronization demanded by the debugger.
 */
template<class CONFIG, bool DEBUG>
void
TMS320<CONFIG, DEBUG> ::
Sync() 
{
	wait(cpu_time - sc_time_stamp());
	cpu_time = sc_time_stamp();
	quantum_time = SC_ZERO_TIME;
}

/** Updates the cpu time to the next bus cycle.
 * Updates the cpu time to the next bus cycle. Additionally it updates the
 * quantum time and if necessary synchronizes with the global SystemC clock.
 */
template<class CONFIG, bool DEBUG>
void 
TMS320<CONFIG, DEBUG> :: 
BusSynchronize() {
	// FIXME: Synchronization with the front side bus is disabled as the current
	//        implementation below consumes at least 70% of the host CPU time
	return;
	quantum_time += 
		(((cpu_time / bus_cycle_time) + 1) * bus_cycle_time) -
		cpu_time;
	if (quantum_time > nice_time)
	{
		wait(quantum_time);
		cpu_time = sc_time_stamp();
	}
	else
	{
		cpu_time = sc_time_stamp() + quantum_time;
	}
	
	return;
}
	
/** Main thread of the CPU simulator.
 * It is a loop that simply executes one instruction and increases the cpu time.
 * Also the quantum time is updated, and if it is bigger than the nice time, the
 * global SystemC time is updated.
 */
template<class CONFIG, bool DEBUG>
void
TMS320<CONFIG, DEBUG> ::
Run()
{
	/* compute the average time of each instruction */
	sc_time time_per_instruction = cpu_cycle_time * ipc;
	while (1)
	{
		if (DEBUG && verbose_tlm_run_thread)
			logger << DebugInfo << LOCATION
				<< "Executing step"
				<< EndDebugInfo;
		inherited::StepInstruction();
		cpu_time += time_per_instruction;
		quantum_time += time_per_instruction;
		if (DEBUG && verbose_tlm_run_thread)
			logger << DebugInfo << LOCATION
				<< "Finished executing step"
				<< EndDebugInfo;
		if (unlikely(quantum_time > nice_time))
		{
			wait(quantum_time);
			quantum_time = SC_ZERO_TIME;
			cpu_time = sc_time_stamp();
		}
	}
}

template<class CONFIG, bool DEBUG>
void 
TMS320<CONFIG, DEBUG> :: 
Reset() {
	inherited::Reset();
}
	
/**
 * Virtual method implementation to handle backward path of transactions sent 
 * through the master_port
 *
 * @param trans the transcation to handle
 * @param phase the state of the transaction to handle (should only be END_REQ
 *        or BEGIN_RESP)
 * @param time  the relative time at which the call is being done
 *
 * @return      the synchronization status
 */
template<class CONFIG, bool DEBUG>
tlm::tlm_sync_enum 
TMS320<CONFIG, DEBUG> ::
nb_transport_bw (transaction_type &trans, phase_type &phase, sc_core::sc_time &time) 
{
	sync_enum_type ret = tlm::TLM_ACCEPTED;

	if (trans.get_command() == tlm::TLM_IGNORE_COMMAND) 
	{
		logger << DebugWarning << "Received nb_transport_bw on master socket" 
			<< ", with an ignore, which the cpu doesn't know how to handle" 
			<< endl
			<< TIME(time) << endl
			<< PHASE(phase) << endl;
		TRANS(logger, trans);
		logger << EndDebug;
		return ret;
	}

	switch(phase)
	{
		case tlm::BEGIN_REQ:
		case tlm::END_RESP:
			/* The cpu should not receive the BEGIN_REQ (as it is the cpu which 
			 * generates cpu requests), neither END_RESP (as it is the cpu which
			 * ends responses) */
			logger << DebugError << "Received nb_transport_bw on master_socket" 
				<< ", with unexpected phase" << endl
				<< LOCATION << endl
				<< TIME(time) << endl
				<< PHASE(phase) << endl;
			TRANS(logger, trans);
			logger << EndDebug;
			sc_stop();
			wait();
			break;
		case tlm::END_REQ:
			/* The request phase is finished.
			 * If the request was a write, then the request can be released, because we do not 
			 *   expect any answer. And we can send a TLM_COMPLETED for the same reason.
			 * If the request was a read, the request can not be released and TLM_ACCEPTED has to
			 *   be sent, and we should wait for the BEGIN_RESP phase. */
			if (trans.is_write()) 
			{
				trans.release();
				ret = tlm::TLM_COMPLETED;
			}
			else
				ret = tlm::TLM_ACCEPTED;
			return ret;
			break;
		case tlm::BEGIN_RESP:
			/* Starting the response phase.
			 * If the request is a write report an error and stop, we should not have received it.
			 * The target has initiated the response to a read request, compute when the request can
			 *   be completely accepted and send a TLM_COMPLETED back. Send an event to the PrRead
			 *   method to unlock the thread that originated the read transaction (using the end_read_event).
			 */
			trans.acquire();
			if (trans.is_write())
			{
				logger << DebugError << "Received nb_transport_bw on BEGIN_RESP phase, with unexpected write transaction" << endl
					<< LOCATION << endl
					<< TIME(time) << endl 
					<< PHASE(phase) << endl;
				TRANS(logger, trans);
				logger << EndDebug;
				sc_stop();
				wait();
				break;
			}
			tmp_time = sc_time_stamp() + time;
			/* TODO: increase tmp_time depending on the size of the transaction. */
			end_read_rsp_event.notify(time);
			ret = tlm::TLM_COMPLETED;
			trans.release();
			return ret;
			break;
	}

	/* this code should never be executed, if you are here something is wrong above :( */
	logger << DebugError << "Reached end of nb_transport_bw, THIS SHOULD NEVER HAPPEN" << endl
		<< LOCATION << endl
		<< TIME(time) << endl
		<< PHASE(phase) << endl;
	TRANS(logger, trans);
	logger << EndDebug;
	sc_stop();
	wait();
	// useless return to avoid compiler warnings/errors
	return ret;
}

/**
 * Virtual method implementation to handle backward path of dmi requests sent through the
 * master port.
 * We do not use the dmi option in our simulator, so this method is unnecessary. However,
 * we have to declare it in order to be able to compile the simulator.
 *
 * @param start_range the start address of the memory range to remove
 * @param end_range   the end address of the memory range to remove
 */
template<class CONFIG, bool DEBUG>
void 
TMS320<CONFIG, DEBUG> ::
invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range) {
	// nothing to do, we are not using dmi
}

/**
 * Virtual method implementation to handle memory read operations performed by 
 * the TMS320 processor implementation.
 * This method synchronizes the  processor with the bus (increase local time) 
 * and sends it. If a synchronization is necessary a SystemC synchronization is 
 * performed.
 * 
 * @param addr    the read base address
 * @param buffer  the buffer to copy the data to read
 * @param size    the size of the read
 */
template<class CONFIG, bool DEBUG> 
bool 
TMS320<CONFIG, DEBUG> :: 
PrRead(address_t addr, 
	void *buffer, 
	uint32_t size,
	bool interlocked) {
	if(DEBUG && verbose_tlm_commands)
		logger << DebugInfo << LOCATION
			<< "Performing PrRead"
			<< EndDebugInfo;
		
	/* Use blocking transactions.
		* Steps:
		* 1 - check when the request can be send (synchronize with the bus)
		* 2 - create the transaction
		* 3 - send the transaction
		* 4 - check the transaction response status
		* 5 - release the transaction
		*/
	// 1 - synchronize with the bus
	BusSynchronize();
	
	// 2 - create the transaction
	transaction_type *trans;
	trans = payload_fabric.allocate();
	trans->set_address(4 * addr); // convert the word address into a byte address
	trans->set_data_length(size);
	trans->set_data_ptr((unsigned char *) buffer);
	trans->set_read();
	
	// 3 - send the transaction
	master_socket->b_transport(*trans, quantum_time);
	cpu_time = sc_time_stamp() + quantum_time;
	if (quantum_time > nice_time)
		wait(quantum_time);
	
	// 4 - check the transaction response status
	bool status = trans->is_response_ok();
	
	// 5 - release the transaction
	trans->release();

	return status;
}

template<class CONFIG, bool DEBUG> 
bool 
TMS320<CONFIG, DEBUG> :: 
PrWrite(address_t addr, 
	const void *buffer, 
	uint32_t size,
	bool interlocked) {
		if(DEBUG && verbose_tlm_commands)
			logger << DebugInfo << LOCATION
				<< "Performing PrWrite"
				<< EndDebugInfo;

	/* Use blocking transactions.
		* Steps:
		* 1 - check when the request can be send (synchronize with the bus)
		* 2 - create the transaction
		* 3 - send the transaction
		* 4 - check the transaction response status
		* 5 - release the transaction
		*/
	// 1 - synchronize with the bus
	BusSynchronize();

	// 2 - create the transaction
	transaction_type *trans;
	trans = payload_fabric.allocate();
	trans->set_address(4 * addr); // convert the word address into a byte address
	trans->set_data_length(size);
	trans->set_data_ptr((unsigned char *)buffer);
	trans->set_write();

	// 3 - send the transaction
	master_socket->b_transport(*trans, quantum_time);
	cpu_time = sc_time_stamp() + quantum_time;
	if (quantum_time > nice_time)
		wait(quantum_time);

	// 4 - check the transaction response status
	bool status = trans->is_response_ok();

	// 5 - release the transaction
	trans->release();

	return status;
}

} // end of namespace tms320
} // end of namespace processor
} // end of namespace tlm
} // end of namespace component
} // end of namespace unisim

#undef LOCATION

#endif // __UNISIM_COMPONENT_TLM_PROCESSOR_TMS320_TCC__
