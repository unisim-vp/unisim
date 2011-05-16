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
 
#include <systemc.h>
#include <tlm.h>
#include "unisim/kernel/tlm2/tlm.hh"
#include "unisim/component/tlm2/processor/arm/arm926ejs/arm926ejs.hh"
#include "unisim/kernel/logger/logger.hh"

#define LOCATION \
	" - location = " \
	<< __FUNCTION__ \
	<< ":unisim_lib/unisim/component/tlm2/processor/arm/arm926ejs/arm926ejs.cc:" \
	<< __LINE__
#define TIME(X) \
	" - time = " \
	<< sc_time_stamp() + (X) \
	<< " (current time = " << sc_time_stamp() << ")"
#define PHASE(X) 	" - phase = " \
	<< ( (X) == tlm::BEGIN_REQ  ? 	"BEGIN_REQ" : \
	   ( (X) == tlm::END_REQ    ? 	"END_REQ" : \
	   ( (X) == tlm::BEGIN_RESP ? 	"BEGIN_RESP" : \
	   ( (X) == tlm::END_RESP   ?   "END_RESP" : \
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
		for(unsigned int _trans_i = 0; \
				_trans_i < (X).get_data_length(); \
				_trans_i++) { \
			(L) << " " << hex << (unsigned int)((X).get_data_ptr()[_trans_i]) \
				<< dec; \
		} \
	} \
}

#define ITRANS(X)	" - trans.level = " << (X).level

namespace unisim {
namespace component {
namespace tlm2 {
namespace processor {
namespace arm {
namespace arm926ejs {

using namespace unisim::kernel::logger;

ARM926EJS :: 
ARM926EJS(const sc_module_name& name, Object *parent)
	: unisim::kernel::service::Object(name, parent)
	, sc_module(name)
	, unisim::component::cxx::processor::arm::arm926ejs::CPU(name, parent)
	, master_socket("master_socket")
	, nirq("nirq_port")
	, nfiq("nfiq_port")
	, end_read_rsp_event()
	, payload_fabric()
	, tmp_time()
	, cpu_cycle_time()
	, bus_cycle_time()
	, cpu_time()
	, bus_time()
	, quantum_time()
	, last_cpu_time()
	, nice_time()
	, next_nice_time()
	, nice_time_int(10)
	, ipc(1.0)
	, bus_cycle_time_int(0)
	, param_nice_time("nice-time", this, nice_time_int,
			"Maximum time between systemc waits in picoseconds.")
	, param_ipc("ipc", this, ipc,
			"Instructions per cycle performance.")
	, param_bus_cycle_time("bus-cycle-time", this, bus_cycle_time_int,
			"The processor bus cycle time in picoseconds.")
	, param_cpu_time("cpu-time", this, cpu_time,
			"The processor time")
	, verbose_tlm(false)
	, param_verbose_tlm("verbose_tlm", this, verbose_tlm, 
			"Display TLM information")
{
	param_nice_time.SetFormat(
			unisim::kernel::service::VariableBase::FMT_DEC);
	param_bus_cycle_time.SetFormat(
			unisim::kernel::service::VariableBase::FMT_DEC);
	master_socket.bind(*this);
	
	SC_THREAD(Run);
	SC_METHOD(IRQHandler);
	sensitive << nirq;
	SC_METHOD(FIQHandler);
	sensitive << nfiq;
}

ARM926EJS ::
~ARM926EJS() 
{
}

/** Initialization of the module
 * Initializes the module depending on the parameters values.
 *
 * @return  true if the initialization suceeds, false otherwise
 */
bool 
ARM926EJS:: 
BeginSetup() 
{
	if (!inherited::BeginSetup()) 
	{
		inherited::logger << DebugError
			<< "Error while trying to set up the ARM cpu" << endl
			<< LOCATION
			<< EndDebugError;
		return false;
	}
	
	cpu_cycle_time = sc_time((double)inherited::cpu_cycle_time, SC_PS);
	bus_cycle_time = sc_time((double)bus_cycle_time_int, SC_PS);
	nice_time = sc_time((double)nice_time_int, SC_PS);
	if (inherited::verbose) {
		inherited::logger << DebugInfo << LOCATION
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

void 
ARM926EJS ::
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
void
ARM926EJS ::
Sync() 
{
	wait(quantum_time);
	cpu_time = sc_time_stamp();
	quantum_time = SC_ZERO_TIME;
}

/** Updates the cpu time to the next bus cycle.
 * Updates the cpu time to the next bus cycle. Additionally it updates the
 * quantum time and if necessary synchronizes with the global SystemC clock.
 */
void 
ARM926EJS :: 
BusSynchronize() {
	// Note: this needs to be better tested, but in order to avoid 
	//   multiplications and divisions with sc_time we do a loop expecting
	//   it will not loop too much. An idea of the operation to perform to
	//   avoid the loop:
	// quantum_time += 
	//      ((((cpu_time + quantum_time) / bus_cycle_time) + 1) * bus_cycle_time) -
	//      (cpu_time + quantum_time);while ( bus_time < (cpu_time + quantum_time) )
	while ( bus_time < (cpu_time + quantum_time) )
		bus_time += bus_cycle_time;
	quantum_time = bus_time - cpu_time;
	if (quantum_time > nice_time)
		Sync();
	
	return;
}
	
/** Main thread of the CPU simulator.
 * It is a loop that simply executes one instruction and increases the cpu time.
 * Also the quantum time is updated, and if it is bigger than the nice time, the
 * global SystemC time is updated.
 */
void
ARM926EJS ::
Run()
{
	/* compute the average time of each instruction */
	sc_time time_per_instruction = cpu_cycle_time * ipc;
	while (1)
	{
		StepInstruction();
		quantum_time += time_per_instruction;
		if (quantum_time > nice_time)
			Sync();
	}
}

void 
ARM926EJS :: 
Reset() 
{
}
	
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
tlm::tlm_sync_enum 
ARM926EJS ::
nb_transport_bw (transaction_type &trans, 
		phase_type &phase, 
		sc_core::sc_time &time) 
{
	sync_enum_type ret = tlm::TLM_ACCEPTED;

	if (trans.get_command() == tlm::TLM_IGNORE_COMMAND) 
	{
		inherited::logger << DebugWarning << "Received nb_transport_bw on master socket" 
			<< ", with an ignore, which the cpu doesn't know how to handle" 
			<< endl
			<< TIME(time) << endl
			<< PHASE(phase) << endl;
		TRANS(inherited::logger, trans);
		inherited::logger << EndDebug;
		return ret;
	}

	switch(phase)
	{
		case tlm::BEGIN_REQ:
		case tlm::END_RESP:
			/* The cpu should not receive the BEGIN_REQ (as it is the cpu which 
			 * generates cpu requests), neither END_RESP (as it is the cpu which
			 * ends responses) */
			inherited::logger << DebugError << "Received nb_transport_bw on master_socket" 
				<< ", with unexpected phase" << endl
				<< LOCATION << endl
				<< TIME(time) << endl
				<< PHASE(phase) << endl;
			TRANS(inherited::logger, trans);
			inherited::logger << EndDebug;
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
				inherited::logger << DebugError << "Received nb_transport_bw on BEGIN_RESP phase, with unexpected write transaction" << endl
					<< LOCATION << endl
					<< TIME(time) << endl 
					<< PHASE(phase) << endl;
				TRANS(inherited::logger, trans);
				inherited::logger << EndDebug;
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

	/* this code should never be executed, if you are here something is wrong 
	 *   above :( */
	inherited::logger << DebugError 
		<< "Reached end of nb_transport_bw, THIS SHOULD NEVER HAPPEN" << endl
		<< LOCATION << endl
		<< TIME(time) << endl
		<< PHASE(phase) << endl;
	TRANS(inherited::logger, trans);
	inherited::logger << EndDebug;
	sc_stop();
	wait();
	// useless return to avoid compiler warnings/errors
	return ret;
}

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
void 
ARM926EJS ::
invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range) 
{
	// nothing to do, we are not using dmi
}
	
/** nIRQ port handler */
void 
ARM926EJS ::
IRQHandler()
{
	if ( nirq )
		exception &= ~unisim::component::cxx::processor::arm::exception::IRQ;
	else
		exception |= unisim::component::cxx::processor::arm::exception::IRQ;
}

/** nFIQ port handler */
void 
ARM926EJS ::
FIQHandler()
{
	if ( nfiq )
		exception &= ~unisim::component::cxx::processor::arm::exception::FIQ;
	else
		exception |= unisim::component::cxx::processor::arm::exception::FIQ;
}
	
/**
 * Virtual method implementation to handle non intrusive reads performed by the inherited
 * cpu to perform external memory accesses.
 * It uses the TLM2 debugging interface to request the data.
 *
 * @param addr		the read base address
 * @param buffer 	the buffer to copy the data to the read
 * @param size		the size of the read
 */

bool 
ARM926EJS ::
ExternalReadMemory(uint64_t addr, void *buffer, uint32_t size)
{
	transaction_type *trans;
	unsigned int read_size;

	trans = payload_fabric.allocate();
	trans->set_address(addr);
	trans->set_data_length(size);
	trans->set_data_ptr((uint8_t *)buffer);
	trans->set_read();

	read_size = master_socket->transport_dbg(*trans);

	if (trans->is_response_ok() && read_size == size)
	{
		trans->release();
		return true;
	}

	trans->release();
	return false;
}

/**
 * Virtual method implementation to handle non intrusive writes performed by the inherited
 * cpu to perform external memory accesses.
 * It uses the TLM2 debugging interface to request the data.
 *
 * @param addr		the write base address
 * @param buffer	the buffer to write into the external memory
 * @param size		the size of the write
 */
bool 
ARM926EJS ::
ExternalWriteMemory(uint64_t addr, const void *buffer, uint32_t size)
{
	transaction_type *trans;
	unsigned int write_size;

	trans = payload_fabric.allocate();
	trans->set_address(addr);
	trans->set_data_length(size);
	trans->set_data_ptr((uint8_t *)buffer);
	trans->set_write();

	write_size = master_socket->transport_dbg(*trans);

	if (trans->is_response_ok() && write_size == size)
	{
		trans->release();
		return true;
	}

	trans->release();
	return false;
}

/**
 * Virtual method implementation to handle memory read operations performed by 
 * the ARM processor implementation.
 * If working with a blocking (BLOCKING = TRUE) version of the ARM processor 
 * this method synchronizes the  processor with the bus (increase local time) 
 * and sends it. If a synchronization is necessary a SystemC synchronization is 
 * performed.
 * TODO: if working with a non-block
 * 
 * @param addr    the read base address
 * @param buffer  the buffer to copy the data to read
 * @param size    the size of the read
 */
void 
ARM926EJS :: 
PrRead(uint32_t addr, 
	uint8_t *buffer, 
	uint32_t size) {
		
	/* Use blocking transactions.
	 * Steps:
	 * 1 - check when the request can be send (synchronize with the bus)
	 * 2 - create the transaction
	 * 3 - send the transaction
	 * 4 - release the transaction
	 */
	// 1 - synchronize with the bus
	BusSynchronize();
	
	// 2 - create the transaction
	transaction_type *trans;
	trans = payload_fabric.allocate();
	trans->set_address(addr);
	trans->set_data_length(size);
	trans->set_data_ptr(buffer);
	trans->set_read();
		
	// 3 - send the transaction
	master_socket->b_transport(*trans, quantum_time);
	if ( quantum_time > nice_time )
		Sync();
		
	// 4 - release the transaction
	trans->release();
	return;
}

void 
ARM926EJS :: 
PrWrite(uint32_t addr, 
	const uint8_t *buffer, 
	uint32_t size) 
{
	/* Use blocking transactions.
	 * Steps:
	 * 1 - check when the request can be send (synchronize with the bus)
	 * 2 - create the transaction
	 * 3 - send the transaction
	 */
	// 1 - synchronize with the bus
	BusSynchronize();
		
	// 2 - create the transaction
	transaction_type *trans;
	trans = payload_fabric.allocate();
	trans->set_address(addr);
	trans->set_data_length(size);
	trans->set_data_ptr((unsigned char *)buffer);
	trans->set_write();

	// 3 - send the transaction
	master_socket->b_transport(*trans, quantum_time);
	if ( quantum_time > nice_time )
		Sync();

	// 4 - release the transaction
	trans->release();
	return;
}

} // end of namespace armemu
} // end of namespace arm
} // end of namespace processor
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#undef LOCATION
#undef TIME
#undef PHASE
#undef TRANS

