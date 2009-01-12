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
 
#ifndef __UNISIM_COMPONENT_TLM_PROCESSOR_ARM_TCC__
#define __UNISIM_COMPONENT_TLM_PROCESSOR_ARM_TCC__

#include <systemc.h>
#include "unisim/kernel/tlm/tlm.hh"
#include "unisim/component/tlm/message/simple_fsb.hh"
#include "unisim/service/interfaces/logger.hh"

#define LOCATION Function << __FUNCTION__ << File << __FILE__ << Line << __LINE__
#define LOCATION 	" - location = " << __FUNCTION__ << ":unisim_lib/unisim/component/tlm2/processor/arm/arm.tcc:" << __LINE__
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
namespace tlm {
namespace processor {
namespace arm {

using namespace unisim::kernel::logger;

template<class CONFIG, bool BLOCKING>
ARM<CONFIG, BLOCKING> :: 
ARM(const sc_module_name& name, Object *parent) :
	Object(name, parent),
	sc_module(name),
	CPU<CONFIG>(name, this, parent),
	master_socket("master_socket"),
	cpu_cycle_time(),
	bus_cycle_time(),
	cpu_time(),
	bus_time(),
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
	verbose_all(false),
	param_verbose_all("verbose_all", this, verbose_all, "Activate all the verbose options"),
	verbose_setup(false),
	param_verbose_setup("verbose_setup", this, verbose_setup, "Display object setup information"),
	verbose_non_blocking(false),
	param_verbose_non_blocking("verbose_non_blocking", this, verbose_non_blocking, "Display non_blocking transaction handling"),
	verbose_blocking(false),
	param_verbose_blocking("verbose_blocking", this, verbose_blocking, "Display blocking transaction handling")
{
	master_socket.bind(*this);
	
	SC_THREAD(Run);
}

template<class CONFIG, bool BLOCKING>
ARM<CONFIG, BLOCKING> ::
~ARM() {
}

template<class CONFIG, bool BLOCKING>
void 
ARM<CONFIG, BLOCKING> ::
Stop(int ret) {
	// Call BusSynchronize to account for the remaining time spent in the cpu core
	BusSynchronize();
	sc_stop();
	wait();
}

template<class CONFIG, bool BLOCKING>
void
ARM<CONFIG, BLOCKING> ::
Sync() {
	sc_time time_spent = cpu_time - last_cpu_time;
	last_cpu_time = cpu_time;
	wait(time_spent);
}

template<class CONFIG, bool BLOCKING>
bool 
ARM<CONFIG, BLOCKING> :: 
Setup() {
	if(!inherited::Setup()) {
		if(inherited::logger_import)
			(*inherited::logger_import) << DebugError << LOCATION
				<< "Error while trying to set up the ARM cpu"
				<< Endl << EndDebugError;
		return false;
	}
	
	/* check verbose settings */
	if(CONFIG::DEBUG_ENABLE && inherited::verbose_all) {
		verbose_tlm_bus_synchronize = true;
		verbose_tlm_run_thread = true;
	} else {
		if(CONFIG::DEBUG_ENABLE && verbose_tlm_bus_synchronize && inherited::logger_import)
			(*inherited::logger_import) << DebugInfo << LOCATION
				<< "verbose-tlm-bus-synchronize = true"
				<< Endl << EndDebugInfo;		
		if(CONFIG::DEBUG_ENABLE && verbose_tlm_run_thread && inherited::logger_import)
			(*inherited::logger_import) << DebugInfo << LOCATION
				<< "verbose-tlm-run-thread = true"
				<< Endl << EndDebugInfo;		
	}
	
	cpu_cycle_time = sc_time((double)cpu_cycle_time_int, SC_PS);
	bus_cycle_time = sc_time((double)bus_cycle_time_int, SC_PS);
	nice_time = sc_time((double)nice_time_int, SC_PS);
	if(CONFIG::DEBUG_ENABLE && inherited::verbose_setup && inherited::logger_import) {
		(*inherited::logger_import) << DebugInfo << LOCATION
			<< "Setting CPU cycle time to " << cpu_cycle_time.to_string() << Endl
			<< "Setting Bus cycle time to " << bus_cycle_time.to_string() << Endl
			<< "Setting nice time to " << nice_time.to_string() << Endl
			<< "Setting IPC to " << ipc << Endl
			<< EndDebugInfo;
	}
	
	return true;
}

template<class CONFIG, bool BLOCKING>
void 
ARM<CONFIG, BLOCKING> :: 
BusSynchronize() {
//	if(cpu_time < sc_time_stamp()) {
//		cerr << "sc_time_stamp bigger than cpu_time" << endl;
//		cerr << "sc_time_stamp = " << sc_time_stamp() << endl;
//		cerr << "cpu_time = " << cpu_time << endl;
//		sc_stop();
//		wait();
//	}
	
	sc_time time_spent = cpu_time - last_cpu_time;
	last_cpu_time = cpu_time;

	while(cpu_time >= bus_time) {
		bus_time += bus_cycle_time;
	}
	sc_time sleep_time = bus_time - sc_time_stamp();
	wait(sleep_time);
	cpu_time = sc_time_stamp();
	last_cpu_time = sc_time_stamp();
	bus_time = sc_time_stamp();
	return;

	if(CONFIG::DEBUG_ENABLE && verbose_tlm_bus_synchronize && inherited::logger_import)
		(*inherited::logger_import) << DebugInfo << LOCATION
			<< "Bus synchro START" 
			<< Endl << EndDebugInfo;
	sc_dt::uint64 current_time_tu = sc_time_stamp().value();
	sc_dt::uint64 time_spent_tu = time_spent.value();
	if(CONFIG::DEBUG_ENABLE && verbose_tlm_bus_synchronize && inherited::logger_import)
		(*inherited::logger_import) << DebugInfo << LOCATION
			<< "time_spent_tu = " << time_spent_tu 
			<< Endl << EndDebugInfo;
	sc_dt::uint64 next_time_tu = current_time_tu + time_spent_tu;
	if(CONFIG::DEBUG_ENABLE && verbose_tlm_bus_synchronize && inherited::logger_import)
		(*inherited::logger_import) << DebugInfo << LOCATION
			<< "next_time_tu = " << next_time_tu 
			<< Endl << EndDebugInfo;
	sc_dt::uint64 bus_cycle_time_tu = bus_cycle_time.value();
	if(CONFIG::DEBUG_ENABLE && verbose_tlm_bus_synchronize && inherited::logger_import)
		(*inherited::logger_import) << DebugInfo << LOCATION
			<< "bus_cycle_time_tu = " << bus_cycle_time_tu 
			<< Endl << EndDebugInfo;
	sc_dt::uint64 bus_time_phase_tu = next_time_tu % bus_cycle_time_tu;
	if(CONFIG::DEBUG_ENABLE && verbose_tlm_bus_synchronize && inherited::logger_import)
		(*inherited::logger_import) << DebugInfo << LOCATION
			<< "bus_time_phase_tu = " << bus_time_phase_tu
			<< Endl << EndDebugInfo;
	if(time_spent_tu || bus_time_phase_tu) {
		sc_dt::uint64 delay_tu = next_time_tu - current_time_tu + (bus_cycle_time_tu - bus_time_phase_tu);
		if(CONFIG::DEBUG_ENABLE && verbose_tlm_bus_synchronize && inherited::logger_import)
			(*inherited::logger_import) << DebugInfo << LOCATION
				<< "delay_tu = " << delay_tu
				<< Endl << EndDebugInfo;
		wait(sc_time(delay_tu, false));
		cpu_time = sc_time_stamp();
		bus_time = cpu_time + bus_cycle_time;
	}
	if(CONFIG::DEBUG_ENABLE && verbose_tlm_bus_synchronize && inherited::logger_import)
		(*inherited::logger_import) << DebugInfo << LOCATION
			<< "Bus synchro END" 
			<< Endl << EndDebugInfo;
}
	
template<class CONFIG, bool BLOCKING>
void 
ARM<CONFIG, BLOCKING>::Synchronize()
{
	sc_time time_spent = cpu_time - last_cpu_time;
	last_cpu_time = cpu_time;
	wait(time_spent);
	cpu_time = sc_time_stamp();
//	next_nice_sctime = sc_time_stamp() + nice_sctime;
}

template<class CONFIG, bool BLOCKING>
void 
ARM<CONFIG, BLOCKING> :: 
Run() {
	sc_time time_per_instruction = cpu_cycle_time * ipc;
//	sc_time stat_refresh_time = stat_reporting_period;
//	sc_time stats_refresh_time_step = stats_refresh_time;
	
	while(1) {
//		if(sc_time_stamp() > stat_refresh_time) {
//			stat_refresh_time = sc_time_stamp() + stat_reporting_period;
//			SendStats();
//		}
		if(cpu_time >= bus_time) {
			bus_time += bus_cycle_time;
			CPU<CONFIG>::OnBusCycle();
			if(cpu_time >= next_nice_time) {
				next_nice_time = cpu_time + nice_time;
				Synchronize();
			}
		}
		if(CONFIG::DEBUG_ENABLE && verbose_tlm_run_thread && inherited::logger_import)
			(*inherited::logger_import) << DebugInfo << LOCATION
				<< "Executing step"
				<< Endl << EndDebugInfo;
		CPU<CONFIG>::StepInstruction();
		if(CONFIG::DEBUG_ENABLE && verbose_tlm_run_thread && inherited::logger_import)
			(*inherited::logger_import) << DebugInfo << LOCATION
				<< "Finished executing step"
				<< Endl << EndDebugInfo;
		cpu_time += time_per_instruction;
	}
}

template<class CONFIG, bool BLOCKING>
void 
ARM<CONFIG, BLOCKING> :: 
Reset() {
}
	
/**
 * Virtual method implementation to handle backward path of transactions sent through the 
 * master_port
 *
 * @param trans the transcation to handle
 * @param phase the state of the transaction to handle (should only be END_REQ or BEGIN_RESP)
 * @param time  the relative time at which the call is being done
 *
 * @return      the synchronization status
 */
template<class CONFIG, bool BLOCKING>
sync_enum_type 
ARM<CONFIG, BLOCKING> ::
nb_transport_bw(transaction_type &trans, phase_type &phase, sc_core::sc_time &time) 
{
	sync_enum_type ret = tlm::TLM_ACCEPTED;

	if (trans.get_command() == tlm::TLM_IGNORE_COMMAND) 
	{
		logger << DebugWarning << "Received nb_transport_bw on port " << id << ", with an ignore, which the cpu doesn't know how to handle" << endl
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
			/* The cpu should not receive the BEGIN_REQ (as it is the cpu which generates cpu requests), 
			 *   neither END_RESP (as it is the cpu which ends responses) */
			logger << DebugError << "Received nb_transport_bw on init_socket[" << id << "], with unexpected phase" << endl
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
			end_read_event.notify(time);
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
template<class CONFIG, bool BLOCKING>
void 
ARM<CONFIG, BLOCKING> ::
invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range) {
	// nothing to do, we are not using dmi
}

template<class CONFIG, bool BLOCKING> 
void 
ARM<CONFIG, BLOCKING> :: 
PrRead(address_t addr, 
	uint8_t *buffer, 
	uint32_t size) {
	if(CONFIG::DEBUG_ENABLE && verbose_tlm_commands && inherited::logger_import)
		(*inherited::logger_import) << DebugInfo << LOCATION
			<< "Performing PrRead"
			<< Endl << EndDebugInfo;
	// synchonize with bus cycle time
	BusSynchronize();

	// create the transaction
	tlm::tlm_generic_payload *trans;
	trans = payload_fabric.allocate();
	trans->set_address(addr);
	trans->set_data_length(size);
	trans->set_data_ptr(buffer);
	trans->set_read();

	
	if (BLOCKING)
	{
		// send the transaction (blocking version)
		sc_time time(SC_ZERO_TIME);
		master_socket->b_transport(*trans, time);
	}
	else
	{
		// send the transaction (non-blocking version)
		tlm::tlm_phase phase = tlm::BEGIN_REQ;
		switch (master_socket->nb_transport_fw(*trans, phase, time)) 
		{
			case tlm::TLM_ACCEPTED:
				if (CONFIG::DEBUG_ENABLE)
				{
					logger << DebugInfo << "Read transaction accepted, waiting for the response" << endl
						<< TIME(time) << endl;
					TRANS(logger, trans);
					logger << EndDebugInfo;
				}
				wait(end_read_event);
				if (CONFIG::DEBUG_ENABLE)
				{			
					logger << DebugInfo << "Read transaction finished" << endl
						<< TIME(time) << endl;
					ETRANS(logger, trans);
					logger << EndDebugInfo;
				}
				break;
			case tlm::TLM_UPDATED:
				switch(phase) {
				case tlm::BEGIN_REQ:
				case tlm::END_RESP:
				case tlm::BEGIN_RESP:
					logger << DebugError << "Received TLM_UPDATED with unexpected phase" << endl
						<< LOCATION << endl
						<< TIME(time) << endl
						<< PHASE(phase) << endl;
						TRANS(logger, *trans);
						logger << EndDebug;
						sc_stop();
						wait();
						break;
				case tlm::END_REQ:
					if (CONFIG::DEBUG_ENABLE) {
						logger << DebugInfo << "Received TLM_UPDATED with END_REQ, waiting for the response event" << endl
							<< TIME(time) << endl;
						TRANS(looger, trans);
						logger << EndDebug;
					}
					wait(end_read_rsp_event);
					if (CONFIG::DEBUG_ENABLE) {
						logger << DebugInfo << "end response event received" << endl
							<< TIME(time) << EndDebug;
					}
					break;
				}
				break;
			case tlm::TLM_COMPLETED:
				break;
		}
	}

	if (CONFIG::DEBUG_ENABLE) {
		logger << DebugInfo << "Transaction answer received" << endl
			<< " - time = " << sc_time_stamp() + time << endl;
		ETRANS(logger, *trans);
		logger << EndDebug;
	}
	
	wait(time);

	if (CONFIG::DEBUG_ENABLE)
		logger << DebugInfo << "Transaction finished, releasing transaction" << endl
			<< " - time = " << sc_time_stamp() << endl
			<< " - trans = " << trans << EndDebug;

	trans->release();

	// bus transaction request
	Pointer<SimpleFSBRequest<address_t, CONFIG::FSB_BURST_SIZE> > req = 
		new(req) SimpleFSBRequest<address_t, CONFIG::FSB_BURST_SIZE>(); 
	// bus transaction response
	Pointer<SimpleFSBResponse<CONFIG::FSB_BURST_SIZE> > rsp; 
	// event notified when bus transaction response is ready
	sc_event rsp_ev; 
	// message for bus transaction
	Pointer<TlmMessage<SimpleFSBRequest<address_t, CONFIG::FSB_BURST_SIZE>, 
		SimpleFSBResponse<CONFIG::FSB_BURST_SIZE> > > msg =
		new(msg) TlmMessage<SimpleFSBRequest<address_t, CONFIG::FSB_BURST_SIZE>, 
			SimpleFSBResponse<CONFIG::FSB_BURST_SIZE> >(req, rsp_ev); 
			
	// transaction is a READ
	req->type = SimpleFSBRequest<address_t, CONFIG::FSB_BURST_SIZE>::READ; 
	// transaction address
	req->addr = addr; 
	 // actual transaction size
	req->size = size;
			
	// loop until the request succeeds
	if(CONFIG::DEBUG_ENABLE && verbose_tlm_commands && inherited::logger_import)
		(*inherited::logger_import) << DebugInfo << LOCATION
			<< "PrRead: loop until bus transaction request is accepted" 
			<< Endl << EndDebugInfo;
	while(!master_port->Send(msg)) {
		// if bus transaction request is not accepted then retry later
		if(CONFIG::DEBUG_ENABLE && verbose_tlm_commands && inherited::logger_import)
			(*inherited::logger_import) << DebugInfo << LOCATION
				<< "PrRead: retry transaction request"
				<< Endl << EndDebugInfo;
		wait(bus_cycle_time);
		cpu_time += bus_cycle_time;
		bus_time += bus_cycle_time;
		inherited::OnBusCycle();
		if(CONFIG::DEBUG_ENABLE && verbose_tlm_commands && inherited::logger_import)
			(*inherited::logger_import) << DebugInfo << LOCATION
				<< "PrRead: retrying transaction request"
				<< Endl << EndDebugInfo;
	}
			
	// bus transaction request has been accepted at this point
	if(CONFIG::DEBUG_ENABLE && verbose_tlm_commands && inherited::logger_import)
		(*inherited::logger_import) << DebugInfo << LOCATION
			<< "PrRead: transaction request accepted, waiting for response"
			<< Endl << EndDebugInfo;
	
	// wait for the bus transaction response
	sc_time prev_cpu_time = cpu_time;
	wait(rsp_ev);
	cpu_time = sc_time_stamp();
	if(cpu_time == prev_cpu_time) {
	  cerr << "Warning: cpu_time = " << cpu_time
	       << " prev_cpu_time = " << prev_cpu_time << endl;
	}
	while(bus_time < cpu_time) {
		inherited::OnBusCycle();
		bus_time += bus_cycle_time;
	}
	rsp = msg->GetResponse();
	if(CONFIG::DEBUG_ENABLE && verbose_tlm_commands && inherited::logger_import)
		(*inherited::logger_import) << DebugInfo << LOCATION
		 	<< "PrRead: response received"
			<< Endl << EndDebugInfo;
	
	// bus transaction response has been received at this point
	if(CONFIG::DEBUG_ENABLE && verbose_tlm_commands && inherited::logger_import)
		(*inherited::logger_import) << DebugInfo << LOCATION
		 	<< "PrRead: copying response data"
			<< Endl << EndDebugInfo;
	// copy the data from the response
	memcpy(buffer, rsp->read_data, size);
	
	if(CONFIG::DEBUG_ENABLE && verbose_tlm_commands && inherited::logger_import)
		(*inherited::logger_import) << DebugInfo << LOCATION
			<< "PrRead finished"
			<< Endl << EndDebugInfo;
}

template<class CONFIG, bool BLOCKING> 
void 
ARM<CONFIG, BLOCKING> :: 
PrWrite(address_t addr, 
	const uint8_t *buffer, 
	uint32_t size) {
	if(CONFIG::DEBUG_ENABLE && verbose_tlm_commands && inherited::logger_import)
		(*inherited::logger_import) << DebugInfo << LOCATION
			<< "Performing PrWrite"
			<< Endl << EndDebugInfo;
	// synchonize with bus cycle time
	BusSynchronize();

	// bus transaction request
	Pointer<SimpleFSBRequest<address_t, CONFIG::FSB_BURST_SIZE> > req = 
		new(req) SimpleFSBRequest<address_t, CONFIG::FSB_BURST_SIZE>(); 
	// message for bus transaction
	Pointer<TlmMessage<SimpleFSBRequest<address_t, CONFIG::FSB_BURST_SIZE>, 
		SimpleFSBResponse<CONFIG::FSB_BURST_SIZE> > > msg =
		new(msg) TlmMessage<SimpleFSBRequest<address_t, CONFIG::FSB_BURST_SIZE>, 
			SimpleFSBResponse<CONFIG::FSB_BURST_SIZE> >(req); 
			
	// transaction is a WRITE
	req->type = SimpleFSBRequest<address_t, CONFIG::FSB_BURST_SIZE>::WRITE; 
	// transaction address
	req->addr = addr; 
	// actual transaction size
	req->size = size;
	// copy the data to the message
	memcpy(req->write_data, buffer, size);

	// loop until the request succeeds
	if(CONFIG::DEBUG_ENABLE && verbose_tlm_commands && inherited::logger_import)
		(*inherited::logger_import) << DebugInfo << LOCATION
			<< "PrWrite: loop until bus transaction request is accepted" 
			<< Endl << EndDebugInfo;
	while(!master_port->Send(msg)) {
		// if bus transaction request is not accepted then retry later
		if(CONFIG::DEBUG_ENABLE && verbose_tlm_commands && inherited::logger_import)
			(*inherited::logger_import) << DebugInfo << LOCATION
				<< "PrWrite: retry transaction request"
				<< Endl << EndDebugInfo;
		wait(bus_cycle_time);
		cpu_time += bus_cycle_time;
		bus_time += bus_cycle_time;
		inherited::OnBusCycle();
		if(CONFIG::DEBUG_ENABLE && verbose_tlm_commands && inherited::logger_import)
			(*inherited::logger_import) << DebugInfo << LOCATION
				<< "PrWrite: retrying transaction request"
				<< Endl << EndDebugInfo;
	}
			
	if(CONFIG::DEBUG_ENABLE && verbose_tlm_commands && inherited::logger_import)
		(*inherited::logger_import) << DebugInfo << LOCATION
			<< "PrWrite finished"
			<< Endl << EndDebugInfo;
}

/*********************************************************************************
 * TODO: Methods to be removed                                             START *
 *********************************************************************************/

template<class CONFIG, bool BLOCKING>
void 
ARM<CONFIG, BLOCKING>::
SetLock(uint32_t lock, uint32_t set) {
	if(inherited::logger_import) 
		(*inherited::logger_import) << DebugError << LOCATION
			<< "Unsupported command SetLock"
			<< Endl << EndDebugError;
	sc_stop();
	wait();
}

template<class CONFIG, bool BLOCKING>
void 
ARM<CONFIG, BLOCKING>::
PrInvalidateBlock(uint32_t set, uint32_t way) {
	if(inherited::logger_import) 
		(*inherited::logger_import) << DebugError << LOCATION
			<< "Unsupported command PrInvalidateBlock"
			<< Endl << EndDebugError;
	sc_stop();
	wait();
}

template<class CONFIG, bool BLOCKING>
void 
ARM<CONFIG, BLOCKING>::
PrFlushBlock(uint32_t set, uint32_t way) {
	if(inherited::logger_import) 
		(*inherited::logger_import) << DebugError << LOCATION
			<< "Unsupported command PrFlushBlock"
			<< Endl << EndDebugError;
	sc_stop();
	wait();
}

template<class CONFIG, bool BLOCKING>
void ARM<CONFIG, BLOCKING>::
PrCleanBlock(uint32_t set, uint32_t way) {
	if(inherited::logger_import) 
		(*inherited::logger_import) << DebugError << LOCATION
			<< "Unsupported command PrCleanBlock"
			<< Endl << EndDebugError;
	sc_stop();
	wait();
}

template<class CONFIG, bool BLOCKING>
uint32_t 
ARM<CONFIG, BLOCKING>::
GetCacheSize() {
	if(inherited::logger_import) 
		(*inherited::logger_import) << DebugError << LOCATION
			<< "Unsupported command GetCacheSize"
			<< Endl << EndDebugError;
	sc_stop();
	wait();
	
	return 0;
}

template<class CONFIG, bool BLOCKING>
uint32_t 
ARM<CONFIG, BLOCKING>::
GetCacheAssociativity() {
	if(inherited::logger_import) 
		(*inherited::logger_import) << DebugError << LOCATION
			<< "Unsupported command GetCacheAssociativity"
			<< Endl << EndDebugError;
	sc_stop();
	wait();
	
	return 0;
}

template<class CONFIG, bool BLOCKING>
uint32_t 
ARM<CONFIG, BLOCKING>::
GetCacheBlockSize() {
	if(inherited::logger_import) 
		(*inherited::logger_import) << DebugError << LOCATION
			<< "Unsupported command GetCacheBlockSize"
			<< Endl << EndDebugError;
	sc_stop();
	wait();
	
	return 0;
}

template<class CONFIG, bool BLOCKING>
void 
ARM<CONFIG, BLOCKING>::
Enable() {
	if(inherited::logger_import) 
		(*inherited::logger_import) << DebugError << LOCATION
			<< "Unsupported command Enable"
			<< Endl << EndDebugError;
	sc_stop();
	wait();
}

template<class CONFIG, bool BLOCKING>
void 
ARM<CONFIG, BLOCKING>::
Disable() {
	if(inherited::logger_import) 
		(*inherited::logger_import) << DebugError << LOCATION
			<< "Unsupported command Disable"
			<< Endl << EndDebugError;
	sc_stop();
	wait();
}

template<class CONFIG, bool BLOCKING>
bool
ARM<CONFIG, BLOCKING>::
IsEnabled() {
	if(inherited::logger_import) 
		(*inherited::logger_import) << DebugError << LOCATION
			<< "Unsupported command IsEnabled"
			<< Endl << EndDebugError;
	sc_stop();
	wait();
	
	return false;
}

template<class CONFIG, bool BLOCKING>
void 
ARM<CONFIG, BLOCKING>::
PrReset() {
	if(inherited::logger_import) 
		(*inherited::logger_import) << DebugError << LOCATION
			<< "Unsupported command PrReset"
			<< Endl << EndDebugError;
	sc_stop();
	wait();
}

template<class CONFIG, bool BLOCKING>
void 
ARM<CONFIG, BLOCKING>::
PrInvalidate() {
	if(inherited::logger_import) 
		(*inherited::logger_import) << DebugError << LOCATION
			<< "Unsupported command PrInvalidate"
			<< Endl << EndDebugError;
	sc_stop();
	wait();
}

template<class CONFIG, bool BLOCKING>
void 
ARM<CONFIG, BLOCKING>::
PrInvalidateSet(uint32_t set) {
	if(inherited::logger_import) 
		(*inherited::logger_import) << DebugError << LOCATION
			<< "Unsupported command PrInvalideSet"
			<< Endl << EndDebugError;
	sc_stop();
	wait();
}

template<class CONFIG, bool BLOCKING>
void 
ARM<CONFIG, BLOCKING>::
PrInvalidateBlock(address_t addr) {
	if(inherited::logger_import) 
		(*inherited::logger_import) << DebugError << LOCATION
			<< "Unsupported command PrInvalidateBlock"
			<< Endl << EndDebugError;
	sc_stop();
	wait();
}

template<class CONFIG, bool BLOCKING>
void 
ARM<CONFIG, BLOCKING>::
PrFlushBlock(address_t addr) {
	if(inherited::logger_import) 
		(*inherited::logger_import) << DebugError << LOCATION
			<< "Unsupported command PrFlushBlock"
			<< Endl << EndDebugError;
	sc_stop();
	wait();
}

template<class CONFIG, bool BLOCKING>
void 
ARM<CONFIG, BLOCKING>::
PrCleanBlock(address_t addr) {
	if(inherited::logger_import) 
		(*inherited::logger_import) << DebugError << LOCATION
			<< "Unsupported command PrCleanBlock"
			<< Endl << EndDebugError;
	sc_stop();
	wait();
}

template<class CONFIG, bool BLOCKING>
void 
ARM<CONFIG, BLOCKING>::
PrZeroBlock(address_t addr) {
	if(inherited::logger_import) 
		(*inherited::logger_import) << DebugError << LOCATION
			<< "Unsupported command PrZeroBlock"
			<< Endl << EndDebugError;
	sc_stop();
	wait();
}

/*********************************************************************************
 * TODO: Methods to be removed                                              STOP *
 *********************************************************************************/

} // end of namespace arm
} // end of namespace processor
} // end of namespace tlm
} // end of namespace component
} // end of namespace unisim

#undef LOCATION

#endif // __UNISIM_COMPONENT_TLM_PROCESSOR_ARM_TCC__
