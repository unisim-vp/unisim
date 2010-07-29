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
 
#ifndef __UNISIM_COMPONENT_TLM2_PROCESSOR_ARM_TCC__
#define __UNISIM_COMPONENT_TLM2_PROCESSOR_ARM_TCC__

#include <systemc.h>
#include <tlm.h>
#include "unisim/kernel/tlm2/tlm.hh"
#include "unisim/kernel/logger/logger.hh"
// #include "unisim/component/cxx/processor/arm/cpu.tcc"

#define LOCATION 	" - location = " << __FUNCTION__ << ":unisim_lib/unisim/component/tlm2/processor/arm/arm.tcc:" << __LINE__
#define TIME(X) 	" - time = " << sc_time_stamp() + (X) << " (current time = " << sc_time_stamp() << ")"
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

#define ITRANS(X)	" - trans.level = " << (X).level

namespace unisim {
namespace component {
namespace tlm2 {
namespace processor {
namespace arm {

using namespace unisim::kernel::logger;

template<class CONFIG, bool BLOCKING>
ARM<CONFIG, BLOCKING> :: 
ARM(const sc_module_name& name, Object *parent)
	: Object(name, parent)
	, sc_module(name)
	, CPU<CONFIG>(name, this, parent)
	, master_socket("master_socket")
	, in_irq("in_irq")
	, in_fiq("in_fiq")
	, free_interrupt_queue()
	, interrupt_queue("interrupt_queue")
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
//	, cpu_cycle_time_int(0)
	, bus_cycle_time_int(0)
	, param_nice_time("nice-time", this, nice_time_int)
	, param_ipc("ipc", this, ipc)
//	, param_cpu_cycle_time("cpu-cycle-time", this, cpu_cycle_time_int)
	, param_bus_cycle_time("bus-cycle-time", this, bus_cycle_time_int)
//	, inherited::logger(*this)
	, verbose_all(false)
	, param_verbose_all("verbose_all", this, verbose_all, "Activate all the verbose options")
	, verbose_setup(false)
	, param_verbose_setup("verbose_setup", this, verbose_setup, "Display object setup information")
	, verbose_tlm(false)
	, param_verbose_tlm("verbose_tlm", this, verbose_tlm, "Display TLM information")
	, verbose_tlm_bus_synchronize(false)
	, param_verbose_tlm_bus_synchronize("verbose_tlm_bus_synchronize", this, verbose_tlm_bus_synchronize, "Display bus synchronization handling")
	, verbose_tlm_run_thread(false)
	, param_verbose_tlm_run_thread("verbose_tlm_run_thread", this, verbose_tlm_run_thread, "Display main thread execution information")
	, verbose_tlm_commands(false)
	, param_verbose_tlm_commands("verbose_tlm_commands", this, verbose_tlm_commands, "Display commands handling")
	, verbose_tlm_irq(false)
	, param_verbose_tlm_irq("verbose_tlm_irq", this, verbose_tlm_irq, "Display TLM irq information")
	, trap_on_verbose_tlm_irq(false)
	, param_trap_on_verbose_tlm_irq("trap_on_verbose_tlm_irq", this, trap_on_verbose_tlm_irq, "Send a trap when displaying TLM irq information")
	, irq_trap_reporting_import("irq-trap-reporting-import", this)
{
	master_socket.bind(*this);
	
	in_irq.register_nb_transport_fw(this, &THIS_MODULE::IrqNbTransportFw);
	in_irq.register_b_transport(this, &THIS_MODULE::IrqBTransport);
	in_irq.register_transport_dbg(this, &THIS_MODULE::IrqTransportDbg);
	in_irq.register_get_direct_mem_ptr(this, &THIS_MODULE::IrqGetDirectMemPtr);

	in_fiq.register_nb_transport_fw(this, &THIS_MODULE::FiqNbTransportFw);
	in_fiq.register_b_transport(this, &THIS_MODULE::FiqBTransport);
	in_fiq.register_transport_dbg(this, &THIS_MODULE::FiqTransportDbg);
	in_fiq.register_get_direct_mem_ptr(this, &THIS_MODULE::FiqGetDirectMemPtr);

	SC_THREAD(Run);
	SC_THREAD(InterruptHandler);
}

template<class CONFIG, bool BLOCKING>
ARM<CONFIG, BLOCKING> ::
~ARM() 
{
}

/** Initialization of the module
 * Initializes the module depending on the parameters values.
 *
 * @return  true if the initialization suceeds, false otherwise
 */
template<class CONFIG, bool BLOCKING>
bool 
ARM<CONFIG, BLOCKING> :: 
Setup() 
{

	cerr << "debug_enable = " << CONFIG::DEBUG_ENABLE << endl;
	cerr << "verbose_all = " << inherited::verbose_all << endl;
	cerr << "verbose_setup = " << inherited::verbose_setup << endl;
	cerr << "verbose_tlm_irq = " << verbose_tlm_irq << endl;
	if (!inherited::Setup()) 
	{
		inherited::logger << DebugError
			<< "Error while trying to set up the ARM cpu" << endl
			<< LOCATION
			<< EndDebugError;
		return false;
	}
	
	/* TODO: remove once the non-blocking version has been implemented */
	if (!BLOCKING)
	{
		inherited::logger << DebugError << LOCATION 
		    << "Non-blocking mode not still implemented"
			<< EndDebugError;
		return false;
	}
	
	/* check verbose settings */
	if(CONFIG::DEBUG_ENABLE && inherited::verbose_all) {
		verbose_tlm_bus_synchronize = true;
		verbose_tlm_run_thread = true;
		verbose_tlm_commands = true;
		verbose_setup = true;
		verbose_tlm = true;
	} else {
		if(CONFIG::DEBUG_ENABLE && verbose_tlm_bus_synchronize)
			inherited::logger << DebugInfo << LOCATION
				<< "verbose-tlm-bus-synchronize = true"
				<< EndDebugInfo;		
		if(CONFIG::DEBUG_ENABLE && verbose_tlm_run_thread)
			inherited::logger << DebugInfo << LOCATION
				<< "verbose-tlm-run-thread = true"
				<< EndDebugInfo;		
	}
	
//	cpu_cycle_time = sc_time((double)cpu_cycle_time_int, SC_PS);
	cpu_cycle_time = sc_time((double)inherited::cpu_cycle_time, SC_PS);
	bus_cycle_time = sc_time((double)bus_cycle_time_int, SC_PS);
	nice_time = sc_time((double)nice_time_int, SC_PS);
	if(CONFIG::DEBUG_ENABLE && inherited::verbose_setup) {
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

template<class CONFIG, bool BLOCKING>
void 
ARM<CONFIG, BLOCKING> ::
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
template<class CONFIG, bool BLOCKING>
void
ARM<CONFIG, BLOCKING> ::
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
template<class CONFIG, bool BLOCKING>
void 
ARM<CONFIG, BLOCKING> :: 
BusSynchronize() {
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
template<class CONFIG, bool BLOCKING>
void
ARM<CONFIG, BLOCKING> ::
Run()
{
	/* compute the average time of each instruction */
	sc_time time_per_instruction = cpu_cycle_time * ipc;
	while (1)
	{
		if (CONFIG::DEBUG_ENABLE && verbose_tlm_run_thread)
			inherited::logger << DebugInfo << LOCATION
				<< "Executing step"
				<< EndDebugInfo;
		CPU<CONFIG>::StepInstruction();
		cpu_time += time_per_instruction;
		quantum_time += time_per_instruction;
		if (CONFIG::DEBUG_ENABLE && verbose_tlm_run_thread)
			inherited::logger << DebugInfo << LOCATION
				<< "Finished executing step"
				<< EndDebugInfo;
		if (quantum_time > nice_time)
		{
			wait(quantum_time);
			quantum_time = SC_ZERO_TIME;
			cpu_time = sc_time_stamp();
		}
	}
}

template<class CONFIG, bool BLOCKING>
void 
ARM<CONFIG, BLOCKING> :: 
Reset() {
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
template<class CONFIG, bool BLOCKING>
tlm::tlm_sync_enum 
ARM<CONFIG, BLOCKING> ::
nb_transport_bw (transaction_type &trans, phase_type &phase, sc_core::sc_time &time) 
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

	/* this code should never be executed, if you are here something is wrong above :( */
	inherited::logger << DebugError << "Reached end of nb_transport_bw, THIS SHOULD NEVER HAPPEN" << endl
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
	
/**
 * Implementation to handle incomming non blocking irq requests (received through the in_irq socket).
 *
 * @param trans      the incomming irq transaction
 * @param phase      the tlm_phase_type of the transaction, only BEGIN_REQ is accepted for interruptions
 * @param time       the time shift from the current time, at which the transaction is being send
 * @return           the return status of the incomming transaction
 */
template <class CONFIG, bool BLOCKING>
tlm::tlm_sync_enum
ARM<CONFIG, BLOCKING> ::
IrqNbTransportFw(TLMInterruptPayload& trans, phase_type &phase, sc_core::sc_time &time)
{
	switch(phase)
	{
		case tlm::BEGIN_REQ:
			if (VerboseTLMIrq())
			{
				inherited::logger << DebugInfo << "Received transaction on in_irq non-blocking interface:" << endl
				<< ITRANS(trans) << endl
				<< PHASE(phase) << endl
				<< TIME(time)
				<< EndDebugInfo;
			}
			if (trans.level)
			{
				// a irq interrupt has to be handled
				interrupt_t *interrupt;
				if (free_interrupt_queue.empty())
				{
					interrupt = new interrupt_t();
				}
				else
				{
					interrupt = free_interrupt_queue.front();
					free_interrupt_queue.pop();
				}
				interrupt->level = true;
				interrupt->fiq = false;
				interrupt_queue.notify(*interrupt, time);
			}
			break;
		case tlm::END_REQ:
		case tlm::BEGIN_RESP:
		case tlm::END_RESP:
			inherited::logger << DebugWarning << "Received unexpected phase on in_irq non-blocking interface:" << endl
				<< ITRANS(trans) << endl
				<< PHASE(phase) << endl
				<< TIME(time)
				<< EndDebugWarning;
			break;
	}
	TrapOnVerboseTLMIrq();
	return tlm::TLM_COMPLETED;
}

/**
 * Implementation to handle incomming blocking irq requests (received through the in_irq socket).
 *
 * @param trans      the incomming irq transaction
 * @param time       the time shift from the current time, at which the transaction is being send
 */
template <class CONFIG, bool BLOCKING>
void 
ARM<CONFIG, BLOCKING> ::
IrqBTransport(TLMInterruptPayload& trans, sc_core::sc_time &time)
{
	if (VerboseTLMIrq())
	{
		inherited::logger << DebugInfo << "Received transaction on in_irq blocking interface:" << endl
			<< ITRANS(trans) << endl
			<< TIME(time)
			<< EndDebugWarning;
	}
	if (trans.level)
	{
		// a irq interrupt has to be handled
		interrupt_t *interrupt;
		if (free_interrupt_queue.empty())
		{
			interrupt = new interrupt_t();
		}
		else
		{
			interrupt = free_interrupt_queue.front();
			free_interrupt_queue.pop();
		}
		interrupt->level = true;
		interrupt->fiq = false;
		interrupt_queue.notify(*interrupt, time);
	}
	TrapOnVerboseTLMIrq();
}

/**
 * Implementation to handle incomming debugging requests through the in_irq socket.
 * Note that this interface should never be called, because the interrupts are not addressable.
 *
 * @param trans      the incomming irq debugging transaction
 * @return           (not applicable) the number of bytes read/written
 */
template <class CONFIG, bool BLOCKING>
unsigned int 
ARM<CONFIG, BLOCKING> ::
IrqTransportDbg(TLMInterruptPayload& trans)
{
	return 0;
}

/**
 * Implementation of the handle of tlm_dmi requests received through the in_irq socket.
 * Note that this interface should never be called, because the interrupts are not addressable.
 * @param trans       the incomming irq tlm_dmi transaction
 * @return            true on success, false otherwise (always false in the interrupts case)
 */
template <class CONFIG, bool BLOCKING>
bool 
ARM<CONFIG, BLOCKING> ::
IrqGetDirectMemPtr(TLMInterruptPayload& trans, tlm::tlm_dmi& dmi)
{
	inherited::logger << DebugWarning << "Received an FIQ"
		<< EndDebugWarning;
	TrapOnVerboseTLMIrq();
	return false;
}

/**
 * Implementation to handle incomming non blocking fiq requests (received through the in_fiq socket).
 *
 * @param trans      the incomming fiq transaction
 * @param phase      the tlm_phase_type of the transaction, only BEGIN_REQ is accepted for interruptions
 * @param time       the time shift from the current time, at which the transaction is being send
 * @return           the return status of the incomming transaction
 */
template <class CONFIG, bool BLOCKING>
tlm::tlm_sync_enum
ARM<CONFIG, BLOCKING> ::
FiqNbTransportFw(TLMInterruptPayload& trans, phase_type &phase, sc_core::sc_time &time)
{
	switch(phase)
	{
		case tlm::BEGIN_REQ:
			if (VerboseTLMIrq())
			{
				inherited::logger << DebugInfo << "Received transaction on in_fiq non-blocking interface:" << endl
				<< ITRANS(trans) << endl
				<< PHASE(phase) << endl
				<< TIME(time)
				<< EndDebugInfo;
			}
			if (trans.level)
			{
				// a fiq interrupt has to be handled
				interrupt_t *interrupt;
				if (free_interrupt_queue.empty())
				{
					interrupt = new interrupt_t();
				}
				else
				{
					interrupt = free_interrupt_queue.front();
					free_interrupt_queue.pop();
				}
				interrupt->level = true;
				interrupt->fiq = true;
				interrupt_queue.notify(*interrupt, time);
			}
			break;
		case tlm::END_REQ:
		case tlm::BEGIN_RESP:
		case tlm::END_RESP:
			inherited::logger << DebugWarning << "Received unexpected phase on in_fiq non-blocking interface:" << endl
				<< ITRANS(trans) << endl
				<< PHASE(phase) << endl
				<< TIME(time)
				<< EndDebugWarning;
			break;
	}
	return tlm::TLM_COMPLETED;
}

/**
 * Implementation to handle incomming blocking fiq requests (received through the in_fiq socket).
 *
 * @param trans      the incomming fiq transaction
 * @param time       the time shift from the current time, at which the transaction is being send
 */
template <class CONFIG, bool BLOCKING>
void 
ARM<CONFIG, BLOCKING> ::
FiqBTransport(TLMInterruptPayload& trans, sc_core::sc_time &time)
{
	if (VerboseTLMIrq())
	{
		inherited::logger << DebugInfo << "Received transaction on in_fiq blocking interface:" << endl
			<< ITRANS(trans) << endl
			<< TIME(time)
			<< EndDebugWarning;
	}
	if (trans.level)
	{
		// a fiq interrupt has to be handled
		interrupt_t *interrupt;
		if (free_interrupt_queue.empty())
		{
			interrupt = new interrupt_t();
		}
		else
		{
			interrupt = free_interrupt_queue.front();
			free_interrupt_queue.pop();
		}
		interrupt->level = true;
		interrupt->fiq = true;
		interrupt_queue.notify(*interrupt, time);
	}
	TrapOnVerboseTLMIrq();
}

/**
 * Implementation to handle incomming debugging requests through the in_fiq socket.
 * Note that this interface should never be called, because the interrupts are not addressable.
 *
 * @param trans      the incomming fiq debugging transaction
 * @return           (not applicable) the number of bytes read/written
 */
template <class CONFIG, bool BLOCKING>
unsigned int 
ARM<CONFIG, BLOCKING> ::
FiqTransportDbg(TLMInterruptPayload& trans)
{
	return 0;
}

/**
 * Implementation of the handle of tlm_dmi requests received through the in_fiq socket.
 * Note that this interface should never be called, because the interrupts are not addressable.
 * @param trans       the incomming fiq tlm_dmi transaction
 * @return            true on success, false otherwise (always false in the interrupts case)
 */
template <class CONFIG, bool BLOCKING>
bool 
ARM<CONFIG, BLOCKING> ::
FiqGetDirectMemPtr(TLMInterruptPayload& trans, tlm::tlm_dmi& dmi)
{
	return false;
}

/**
 * Thread to handle incomming interruptions (irq and fiq) at the right time.
 */
template <class CONFIG, bool BLOCKING>
void
ARM<CONFIG, BLOCKING> ::
InterruptHandler()
{
	sc_event &ev = interrupt_queue.get_event();
	while(1)
	{
		wait(ev);
		bool irq = false;
		bool fiq = false;
		for (interrupt_t *interrupt = interrupt_queue.get_next_transaction();
				interrupt != 0;
				interrupt = interrupt_queue.get_next_transaction())
		{
			if (interrupt->fiq)
				fiq = true;
			else
				irq = true;
			delete interrupt;
		}
		if (VerboseTLMIrq())
			inherited::logger << DebugInfo << "Handling interrupt:" << endl
				<< " - type = " << (irq?"irq":"") << ((irq && fiq)?" & ":"") << (fiq?"fiq":"") << endl
				<< TIME(SC_ZERO_TIME)
				<< EndDebugInfo;
		TrapOnVerboseTLMIrq();
		if (irq) inherited::SetPendingException(inherited::IRQ_EXCEPTION);
		if (fiq) inherited::SetPendingException(inherited::FIQ_EXCEPTION);
	}
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

template<class CONFIG, bool BLOCKING>
bool 
ARM<CONFIG, BLOCKING> ::
ExternalReadMemory(uint64_t addr, void *buffer, uint32_t size)
{
	transaction_type *trans;
	unsigned int read_size;

	if(CONFIG::DEBUG_ENABLE && verbose_tlm_commands) inherited::logger << DebugInfo << LOCATION
			<< "Performing ExternalReadMemory"
			<< EndDebugInfo;
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
template<class CONFIG, bool BLOCKING>
bool 
ARM<CONFIG, BLOCKING> ::
ExternalWriteMemory(uint64_t addr, const void *buffer, uint32_t size)
{
	transaction_type *trans;
	unsigned int write_size;

	if(CONFIG::DEBUG_ENABLE && verbose_tlm_commands) inherited::logger << DebugInfo << LOCATION
			<< "Performing ExternalReadMemory"
			<< EndDebugInfo;
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
template<class CONFIG, bool BLOCKING> 
void 
ARM<CONFIG, BLOCKING> :: 
PrRead(address_t addr, 
	uint8_t *buffer, 
	uint32_t size) {
	if(CONFIG::DEBUG_ENABLE && verbose_tlm_commands)
		inherited::logger << DebugInfo << LOCATION
			<< "Performing PrRead"
			<< EndDebugInfo;
		
	if (BLOCKING)
	{
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
		cpu_time = sc_time_stamp() + quantum_time;
		if (quantum_time > nice_time)
			wait(quantum_time);
			
		// 4 - release the transaction
		trans->release();
		return;
	}	

	// synchonize with bus cycle time
	BusSynchronize();

	// create the transaction
	tlm::tlm_generic_payload *trans;
	trans = payload_fabric.allocate();
	trans->set_address(addr);
	trans->set_data_length(size);
	trans->set_data_ptr(buffer);
	trans->set_read();

	
	// send the transaction (non-blocking version)
	sc_time time(SC_ZERO_TIME);
	tlm::tlm_phase phase = tlm::BEGIN_REQ;
	switch (master_socket->nb_transport_fw(*trans, phase, time)) 
	{
		case tlm::TLM_ACCEPTED:
			if (CONFIG::DEBUG_ENABLE)
			{
				inherited::logger << DebugInfo << "Read transaction accepted, waiting for the response" << endl
					<< TIME(time) << endl;
				TRANS(inherited::logger, *trans);
				inherited::logger << EndDebugInfo;
			}
			wait(end_read_rsp_event);
			if (CONFIG::DEBUG_ENABLE)
			{			
				inherited::logger << DebugInfo << "Read transaction finished" << endl
					<< TIME(time) << endl;
				ETRANS(inherited::logger, *trans);
				inherited::logger << EndDebugInfo;
			}
			break;
		case tlm::TLM_UPDATED:
			switch(phase) {
			case tlm::BEGIN_REQ:
			case tlm::END_RESP:
			case tlm::BEGIN_RESP:
				inherited::logger << DebugError << "Received TLM_UPDATED with unexpected phase" << endl
					<< LOCATION << endl
					<< TIME(time) << endl
					<< PHASE(phase) << endl;
				TRANS(inherited::logger, *trans);
				inherited::logger << EndDebug;
				sc_stop();
				wait();
				break;
			case tlm::END_REQ:
				if (CONFIG::DEBUG_ENABLE) {
					inherited::logger << DebugInfo << "Received TLM_UPDATED with END_REQ, waiting for the response event" << endl
						<< TIME(time) << endl;
					TRANS(inherited::logger, *trans);
					inherited::logger << EndDebug;
				}
				wait(end_read_rsp_event);
				if (CONFIG::DEBUG_ENABLE) {
					inherited::logger << DebugInfo << "end response event received" << endl
						<< TIME(time) << EndDebug;
				}
				break;
			}
			break;
		case tlm::TLM_COMPLETED:
			break;
	}
	if (CONFIG::DEBUG_ENABLE) {
		inherited::logger << DebugInfo << "Transaction answer received" << endl
			<< " - time = " << sc_time_stamp() + time << endl;
		ETRANS(inherited::logger, *trans);
		inherited::logger << EndDebug;
	}

	wait(time);

	trans->release();

}

template<class CONFIG, bool BLOCKING> 
void 
ARM<CONFIG, BLOCKING> :: 
PrWrite(address_t addr, 
	const uint8_t *buffer, 
	uint32_t size) {
		if(CONFIG::DEBUG_ENABLE && verbose_tlm_commands)
			inherited::logger << DebugInfo << LOCATION
				<< "Performing PrWrite"
				<< EndDebugInfo;

		if (BLOCKING)
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
			cpu_time = sc_time_stamp() + quantum_time;
			if (quantum_time > nice_time)
				wait(quantum_time);
		
			// 4 - release the transaction
			trans->release();
			return;
		}	
}

/*********************************************************************************
 * TODO: Methods to be removed                                             START *
 *********************************************************************************/

template<class CONFIG, bool BLOCKING>
void 
ARM<CONFIG, BLOCKING>::
SetLock(uint32_t lock, uint32_t set) {
//	if(inherited::inherited::logger_import) 
//		inherited::logger << DebugError << LOCATION
//			<< "Unsupported command SetLock"
//			<< EndDebugError;
//	sc_stop();
//	wait();
}

template<class CONFIG, bool BLOCKING>
void 
ARM<CONFIG, BLOCKING>::
PrInvalidateBlock(uint32_t set, uint32_t way) {
//	if(inherited::inherited::logger_import) 
//		inherited::logger << DebugError << LOCATION
//			<< "Unsupported command PrInvalidateBlock"
//			<< EndDebugError;
//	sc_stop();
//	wait();
}

template<class CONFIG, bool BLOCKING>
void 
ARM<CONFIG, BLOCKING>::
PrFlushBlock(uint32_t set, uint32_t way) {
//	if(inherited::inherited::logger_import) 
//		inherited::logger << DebugError << LOCATION
//			<< "Unsupported command PrFlushBlock"
//			<< EndDebugError;
//	sc_stop();
//	wait();
}

template<class CONFIG, bool BLOCKING>
void ARM<CONFIG, BLOCKING>::
PrCleanBlock(uint32_t set, uint32_t way) {
//	if(inherited::inherited::logger_import) 
//		inherited::logger << DebugError << LOCATION
//			<< "Unsupported command PrCleanBlock"
//			<< EndDebugError;
//	sc_stop();
//	wait();
}

template<class CONFIG, bool BLOCKING>
uint32_t 
ARM<CONFIG, BLOCKING>::
GetCacheSize() {
//	if(inherited::inherited::logger_import) 
//		inherited::logger << DebugError << LOCATION
//			<< "Unsupported command GetCacheSize"
//			<< EndDebugError;
//	sc_stop();
//	wait();
	
	return 0;
}

template<class CONFIG, bool BLOCKING>
uint32_t 
ARM<CONFIG, BLOCKING>::
GetCacheAssociativity() {
//	if(inherited::inherited::logger_import) 
//		inherited::logger << DebugError << LOCATION
//			<< "Unsupported command GetCacheAssociativity"
//			<< EndDebugError;
//	sc_stop();
//	wait();
	
	return 0;
}

template<class CONFIG, bool BLOCKING>
uint32_t 
ARM<CONFIG, BLOCKING>::
GetCacheBlockSize() {
//	if(inherited::inherited::logger_import) 
//		inherited::logger << DebugError << LOCATION
//			<< "Unsupported command GetCacheBlockSize"
//			<< EndDebugError;
//	sc_stop();
//	wait();
	
	return 0;
}

template<class CONFIG, bool BLOCKING>
void 
ARM<CONFIG, BLOCKING>::
Enable() {
//	if(inherited::inherited::logger_import) 
//		inherited::logger << DebugError << LOCATION
//			<< "Unsupported command Enable"
//			<< EndDebugError;
//	sc_stop();
//	wait();
}

template<class CONFIG, bool BLOCKING>
void 
ARM<CONFIG, BLOCKING>::
Disable() {
//	if(inherited::inherited::logger_import) 
//		inherited::logger << DebugError << LOCATION
//			<< "Unsupported command Disable"
//			<< EndDebugError;
//	sc_stop();
//	wait();
}

template<class CONFIG, bool BLOCKING>
bool
ARM<CONFIG, BLOCKING>::
IsEnabled() {
//	if(inherited::inherited::logger_import) 
//		inherited::logger << DebugError << LOCATION
//			<< "Unsupported command IsEnabled"
//			<< EndDebugError;
//	sc_stop();
//	wait();
	
	return false;
}

template<class CONFIG, bool BLOCKING>
void 
ARM<CONFIG, BLOCKING>::
PrReset() {
//	if(inherited::inherited::logger_import) 
//		inherited::logger << DebugError << LOCATION
//			<< "Unsupported command PrReset"
//			<< EndDebugError;
//	sc_stop();
//	wait();
}

template<class CONFIG, bool BLOCKING>
void 
ARM<CONFIG, BLOCKING>::
PrInvalidate() {
//	if(inherited::inherited::logger_import) 
//		inherited::logger << DebugError << LOCATION
//			<< "Unsupported command PrInvalidate"
//			<< EndDebugError;
//	sc_stop();
//	wait();
}

template<class CONFIG, bool BLOCKING>
void 
ARM<CONFIG, BLOCKING>::
PrInvalidateSet(uint32_t set) {
//	if(inherited::inherited::logger_import) 
//		inherited::logger << DebugError << LOCATION
//			<< "Unsupported command PrInvalideSet"
//			<< EndDebugError;
//	sc_stop();
//	wait();
}

template<class CONFIG, bool BLOCKING>
void 
ARM<CONFIG, BLOCKING>::
PrInvalidateBlock(address_t addr) {
//	if(inherited::inherited::logger_import) 
//		inherited::logger << DebugError << LOCATION
//			<< "Unsupported command PrInvalidateBlock"
//			<< EndDebugError;
//	sc_stop();
//	wait();
}

template<class CONFIG, bool BLOCKING>
void 
ARM<CONFIG, BLOCKING>::
PrFlushBlock(address_t addr) {
//	if(inherited::inherited::logger_import) 
//		inherited::logger << DebugError << LOCATION
//			<< "Unsupported command PrFlushBlock"
//			<< EndDebugError;
//	sc_stop();
//	wait();
}

template<class CONFIG, bool BLOCKING>
void 
ARM<CONFIG, BLOCKING>::
PrCleanBlock(address_t addr) {
//	if(inherited::inherited::logger_import) 
//		inherited::logger << DebugError << LOCATION
//			<< "Unsupported command PrCleanBlock"
//			<< EndDebugError;
//	sc_stop();
//	wait();
}

template<class CONFIG, bool BLOCKING>
void 
ARM<CONFIG, BLOCKING>::
PrZeroBlock(address_t addr) {
//	if(inherited::inherited::logger_import) 
//		inherited::logger << DebugError << LOCATION
//			<< "Unsupported command PrZeroBlock"
//			<< EndDebugError;
//	sc_stop();
//	wait();
}

/*********************************************************************************
 * TODO: Methods to be removed                                              STOP *
 *********************************************************************************/

/*************************************************************************
 * Logger, verbose and trap parameters/methods/ports               START *
 *************************************************************************/

template<class CONFIG, bool BLOCKING>
bool 
ARM<CONFIG, BLOCKING>::
VerboseSetup()
{
	return CONFIG::DEBUG_ENABLE && verbose_setup;
}

template<class CONFIG, bool BLOCKING>
bool 
ARM<CONFIG, BLOCKING>::
VerboseTLM()
{
	return CONFIG::DEBUG_ENABLE && verbose_tlm;
}

template<class CONFIG, bool BLOCKING>
bool 
ARM<CONFIG, BLOCKING>::
VerboseTLMIrq()
{
	return CONFIG::DEBUG_ENABLE && verbose_tlm_irq;
}

template<class CONFIG, bool BLOCKING>
void
ARM<CONFIG, BLOCKING>::
TrapOnVerboseTLMIrq()
{
	if (CONFIG::DEBUG_ENABLE && trap_on_verbose_tlm_irq && irq_trap_reporting_import)
		irq_trap_reporting_import->ReportTrap();
}

/*************************************************************************
 * Logger, verbose and trap parameters/methods/ports                 END *
 *************************************************************************/
} // end of namespace arm
} // end of namespace processor
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#undef LOCATION

#endif // __UNISIM_COMPONENT_TLM2_PROCESSOR_ARM_TCC__
