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

#ifndef __UNISIM_COMPONENT_TLM2_INTERRUPT_STR7_EIC_STR7_EIC_TCC__
#define __UNISIM_COMPONENT_TLM2_INTERRUPT_STR7_EIC_STR7_EIC_TCC__

#define LOCATION 	" - location = " << __FUNCTION__ << ":unisim_lib/unisim/component/tlm2/interrupt/str7_eic/str7_eic.tcc:" << __LINE__
#define TIME(X) 	" - time = " << sc_time_stamp() + (X)
#define PHASE(X) 	" - phase = " << 	( (X) == tlm::BEGIN_REQ  ? 	"BEGIN_REQ" : \
										( (X) == tlm::END_REQ    ? 	"END_REQ" : \
										( (X) == tlm::BEGIN_RESP ? 	"BEGIN_RESP" : \
										( (X) == tlm::END_RESP   ?  "END_RESP" : \
										  							"UNINITIALIZED_PHASE"))))
#define ITRANS(X)	" - trans.level = " << (X).level
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

#if defined(__GNUC__) && ((__GNUC__ >= 2 && __GNUC_MINOR__ >= 96) || __GNUC__ >= 3)
#define likely(x)       __builtin_expect((x),1)
#define unlikely(x)     __builtin_expect((x),0)
#else
#define likely(x) (x)
#define unlikely(x) (x)
#endif

#include <sstream>

namespace unisim {
namespace component {
namespace tlm2 {
namespace interrupt {
namespace str7_eic {

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebug;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;

template <unsigned int BUS_WIDTH>
STR7_EIC<BUS_WIDTH> ::
STR7_EIC(const sc_module_name& name, Object* parent) :
		Object(name, parent),
		Client<TrapReporting>(name, parent),
		sc_module(name),
		out_irq("out_irq"),
		out_fiq("out_fiq"),
		in_mem("in_mem"),
		trap_reporting_import("trap_reporting_import", this),
		icr(0),
		cicr(0),
		cipr(0),
		ivr(0),
		fir(0),
		ier0(0),
		ipr0(0),
		irq_status(0),
		fiq_status(0),
		fsm_state(READY),
		irq_fifo("irq_fifo"),
		fiq_fifo("fiq_fifo"),
		new_irq(0),
		stack_depth(0),
		irqPayloadFabric(),
		base_address(0),
		param_base_address("base-address", this, base_address),
		logger(*this),
		verbose_all(false),
		param_verbose_all("verbose-all", this, verbose_all, "Enable all the verbose options"),
		verbose_setup(false),
		param_verbose_setup("verbose-setup", this, verbose_setup, "Display module setup"),
		verbose_run(false),
		param_verbose_run("verbose-run", this, verbose_run, "Display runtime information"),
		verbose_tlm(false),
		param_verbose_tlm("verbose-tlm", this, verbose_tlm, "Display TLM2.0 transaction management"),
		trap_on_irq(false),
		param_trap_on_irq("trap-on-irq", this, trap_on_irq, "Send a trap when an IRQ arrives"),
		trap_on_fiq(false),
		param_trap_on_fiq("trap-on-fiq", this, trap_on_fiq, "Send a trap when a FIQ arrives"),
		trap_on_irq_handling(false),
		param_trap_on_irq_handling("trap-on-irq-handling", this, trap_on_irq_handling, "Send a trap when an IRQ is handled"),
		trap_on_fiq_handling(false),
		param_trap_on_fiq_handling("trap-on-fiq-handling", this, trap_on_fiq_handling, "Send a trap when a FIQ is handled"),
		trap_on_warning(false),
		param_trap_on_warning("trap-on-warning", this, trap_on_warning, "Send a trap when an unexpected behavior is found"),
		reg_icr("ICR", this, icr),
		reg_cicr("CICR", this, cicr),
		reg_cipr("CIPR", this, cipr),
		reg_ivr("IVR", this, ivr),
		reg_fir("FIR", this, fir),
		reg_ier0("IER0", this, ier0),
		reg_ipr0("IPR0", this, ipr0)
{
	SC_HAS_PROCESS(STR7_EIC);

	SC_THREAD(IRQFifoHandler);
	SC_THREAD(FIQFifoHandler);

	/* bind the in_mem socket to the methods implementations provided by the module */
	in_mem.bind(*this);
	/* register the in_irq sockets to the methods implementations provided by the module */
	for (unsigned int i = 0; i < NUM_IRQ; i++)
	{
		in_irq[i].register_nb_transport_fw(this, &THIS_MODULE::InIRQNb, i);
		in_irq[i].register_b_transport(this, &THIS_MODULE::InIRQB, i);
		in_irq[i].register_transport_dbg(this, &THIS_MODULE::InIRQDbg, i);
		in_irq[i].register_get_direct_mem_ptr(this, &THIS_MODULE::InIRQDmi, i);
	}
	/* register the in_fiq sockets to the methods implementations provided by the module */
	for (unsigned int i = 0; i < NUM_FIQ; i++)
	{
		in_fiq[i].register_nb_transport_fw(this, &THIS_MODULE::InFIQNb, i);
		in_fiq[i].register_b_transport(this, &THIS_MODULE::InFIQB, i);
		in_fiq[i].register_transport_dbg(this, &THIS_MODULE::InFIQDbg, i);
		in_fiq[i].register_get_direct_mem_ptr(this, &THIS_MODULE::InFIQDmi, i);
	}
	/* register the out_irq socket to the methods implementation provided by the module */
	out_irq.register_nb_transport_bw(this, &THIS_MODULE::OutIRQNb);
	out_irq.register_invalidate_direct_mem_ptr(this, &THIS_MODULE::OutIRQDMI);
	/* register the out_fiq socket to the methods implementation provided by the module */
	out_fiq.register_nb_transport_bw(this, &THIS_MODULE::OutFIQNb);
	out_fiq.register_invalidate_direct_mem_ptr(this, &THIS_MODULE::OutFIQDMI);

	unisim::kernel::service::Register<uint32_t> *reg_sir[32];
	for (unsigned int i = 0; i < 32; i++)
	{
		std::stringstream ss;
		ss << "SIR[" << i << "]";
		reg_sir[i] = new unisim::kernel::service::Register<uint32_t>(ss.str().c_str(), this, sir[i]);
	}
}

template <unsigned int BUS_WIDTH>
STR7_EIC<BUS_WIDTH> ::
~STR7_EIC()
{
}
	
template <unsigned int BUS_WIDTH>
bool
STR7_EIC<BUS_WIDTH> ::
Setup()
{
	if (verbose_all) verbose_setup = verbose_run = true;

	if (VerboseSetup())
	{
		logger << DebugInfo << "Setup:" << endl;
		if (VerboseAll()) logger << " - verbose-all activated" << endl;
		if (VerboseSetup()) logger << " - verbose-setup activated" << endl;
		if (VerboseRun()) logger << " - verbose-run activated" << endl;
		if (VerboseTLM()) logger << " - verbose-tlm activated" << endl;

		logger << " - base address = 0x" << hex << base_address << dec;
		logger << EndDebugInfo;
	}

	return true;
}

/* START: callback methods for the in_irq sockets */

template <unsigned int BUS_WIDTH>
tlm::tlm_sync_enum
STR7_EIC<BUS_WIDTH> ::
InIRQNb(int index, TLMInterruptPayload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t)
{
	switch (phase)
	{
		case tlm::BEGIN_REQ:
			IRQ(index, trans.level, t);
			break;
		default:
			logger << DebugWarning << "Received unexpected transaction phase on in_irq[" << index << "]:" << endl
				<< LOCATION << endl
				<< ITRANS(trans) << endl
				<< PHASE(phase) << endl
				<< TIME(t)
				<< EndDebugWarning;
			break;
	}

	return tlm::TLM_COMPLETED;
}

template <unsigned int BUS_WIDTH>
void
STR7_EIC<BUS_WIDTH> ::
InIRQB(int index, TLMInterruptPayload& trans, sc_core::sc_time& t)
{
	IRQ(index, trans.level, t);
}

template <unsigned int BUS_WIDTH>
unsigned int
STR7_EIC<BUS_WIDTH> ::
InIRQDbg(int index, TLMInterruptPayload& trans)
{
	/* This should never be called */
	return 0;
}

template <unsigned int BUS_WIDTH>
bool
STR7_EIC<BUS_WIDTH> ::
InIRQDmi(int index, TLMInterruptPayload& trans, tlm::tlm_dmi& dmi)
{
	/* This should never be called */
	return false;
}

/* END: callback methods for the in_irq sockets */

/* START: callback methods for the in_fiq sockets */

template <unsigned int BUS_WIDTH>
tlm::tlm_sync_enum
STR7_EIC<BUS_WIDTH> ::
InFIQNb(int index, TLMInterruptPayload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t)
{
	switch (phase)
	{
		case tlm::BEGIN_REQ:
			FIQ(index, trans.level, t);
			break;
		default:
			logger << DebugWarning << "Received unexpected transaction phase on in_fiq[" << index << "]:" << endl
				<< LOCATION << endl
				<< ITRANS(trans) << endl
				<< PHASE(phase) << endl
				<< TIME(t)
				<< EndDebugWarning;
			break;
	}

	return tlm::TLM_COMPLETED;
}

template <unsigned int BUS_WIDTH>
void
STR7_EIC<BUS_WIDTH> ::
InFIQB(int index, TLMInterruptPayload& trans, sc_core::sc_time& t)
{
	FIQ(index, trans.level, t);
}

template <unsigned int BUS_WIDTH>
unsigned int
STR7_EIC<BUS_WIDTH> ::
InFIQDbg(int index, TLMInterruptPayload& trans)
{
	/* This should never be called */
	return 0;
}

template <unsigned int BUS_WIDTH>
bool
STR7_EIC<BUS_WIDTH> ::
InFIQDmi(int index, TLMInterruptPayload& trans, tlm::tlm_dmi& dmi)
{
	/* This should never be called */
	return 0;
}

/* END: callback methods for the in_fiq sockets */

/* START: callback methods for the out_irq sockets */

template <unsigned int BUS_WIDTH>
tlm::tlm_sync_enum
STR7_EIC<BUS_WIDTH> ::
OutIRQNb(TLMInterruptPayload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t)
{
	switch (phase)
	{
		case tlm::END_REQ:
			/* nothing to do, just indicate that the transaction has been completed */
			trans.release();
			return tlm::TLM_COMPLETED;
			break;
		case tlm::BEGIN_REQ:
		case tlm::BEGIN_RESP:
		case tlm::END_RESP:
			/* unexpected phase received */
			logger << DebugError << "Unexpected phase received on out_irq:" << endl
				<< LOCATION << endl
				<< ITRANS(trans) << endl
				<< PHASE(phase) << endl
				<< TIME(t) << EndDebugError;
			sc_stop();
			wait();
			break;
	}
	// this code should never be executed
	logger << DebugError << "This code should never be executed" << endl
		<< LOCATION << endl
		<< ITRANS(trans) << endl
		<< PHASE(phase) << endl
		<< TIME(t) << EndDebugError;
	sc_stop();
	wait();
	return tlm::TLM_COMPLETED;
}

template <unsigned int BUS_WIDTH>
void
STR7_EIC<BUS_WIDTH> ::
OutIRQDMI(sc_dt::uint64 start_range, sc_dt::uint64 end_range)
{
	/* This method shoud never be used */
}

/* END: callback methods for the out_irq sockets */
	
/* START: callback methods for the out_fiq sockets */

template <unsigned int BUS_WIDTH>
tlm::tlm_sync_enum
STR7_EIC<BUS_WIDTH> ::
OutFIQNb(TLMInterruptPayload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t)
{
	switch (phase)
	{
		case tlm::END_REQ:
			/* nothing to do, just indicate that the transaction has been completed */
			trans.release();
			return tlm::TLM_COMPLETED;
			break;
		case tlm::BEGIN_REQ:
		case tlm::BEGIN_RESP:
		case tlm::END_RESP:
			/* unexpected phase received */
			logger << DebugError << "Unexpected phase received on out_fiq:" << endl
				<< LOCATION << endl
				<< ITRANS(trans) << endl
				<< PHASE(phase) << endl
				<< TIME(t) << EndDebugError;
			sc_stop();
			wait();
			break;
	}
	// this code should never be executed
	logger << DebugError << "This code should never be executed" << endl
		<< LOCATION << endl
		<< ITRANS(trans) << endl
		<< PHASE(phase) << endl
		<< TIME(t) << EndDebugError;
	sc_stop();
	wait();
	return tlm::TLM_COMPLETED;
}

template <unsigned int BUS_WIDTH>
void
STR7_EIC<BUS_WIDTH> ::
OutFIQDMI(sc_dt::uint64 start_range, sc_dt::uint64 end_range)
{
	/* This method should never be used */
}

/* END: callback methods for the out_fiq sockets */

/* START: methods implementing the "in_mem" socket */

template <unsigned int BUS_WIDTH>
void
STR7_EIC<BUS_WIDTH> ::
b_transport(tlm::tlm_generic_payload& trans, sc_core::sc_time& t)
{
	if (VerboseTLM())
	{
		logger << DebugInfo << "Received memory b_transport transaction:" << endl;
		TRANS(logger, trans);
		logger << endl << TIME(t) << EndDebug;
	}
	unsigned char *data = trans.get_data_ptr();
	unsigned int size = trans.get_data_length();
	uint64_t addr = trans.get_address();

	if (trans.is_read())
	{
		trans.set_response_status(tlm::TLM_OK_RESPONSE);
		if (size != 4)
		{
			memset(data, 0, size);
		}
		else
		{
			addr = addr - base_address;
			*(uint32_t *)data = ReadRegister(addr);
		}
		
		if (VerboseTLM()) 
		{
			logger << DebugInfo << "Replying received memory b_transport transaction:" << endl;
			ETRANS(logger, trans);
			logger << endl << TIME(t) << EndDebug;
		}
		return;
	}
	else 
	{
		if (trans.is_write())
		{
			trans.set_response_status(tlm::TLM_OK_RESPONSE);
			if (size != 4)
				return;
			addr = addr - base_address;
			WriteRegister(addr, *(uint32_t *)data);
			return;
		}
	}
}

template <unsigned int BUS_WIDTH>
tlm::tlm_sync_enum
STR7_EIC<BUS_WIDTH> ::
nb_transport_fw(tlm::tlm_generic_payload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t)
{
	if (VerboseTLM())
	{
		logger << DebugInfo << "Received memory nb_transport_fw transaction:" << endl;
		TRANS(logger, trans);
		logger << endl
			<< PHASE(phase) << endl
			<< TIME(t) << EndDebug;
	}
	/* TODO */
	return tlm::TLM_COMPLETED;
}

template <unsigned int BUS_WIDTH>
bool
STR7_EIC<BUS_WIDTH> ::
get_direct_mem_ptr(tlm::tlm_generic_payload& trans, tlm::tlm_dmi& dmi_data)
{
	/* This method should never be used */
	return false;
}

template <unsigned int BUS_WIDTH>
unsigned int
STR7_EIC<BUS_WIDTH> ::
transport_dbg(tlm::tlm_generic_payload& trans)
{
	unsigned char *data = trans.get_data_ptr();
	unsigned int size = trans.get_data_length();
	uint64_t addr = trans.get_address();

	if (trans.is_read())
	{
		trans.set_response_status(tlm::TLM_OK_RESPONSE);
		if (size == 0) return size;
		unsigned int read_size = 0;
		do
		{
			uint64_t offset = ((addr + read_size) % 4);
			uint64_t base = (addr + read_size) - offset;
			uint32_t reg = 0;
			reg = ReadRegister(base - base_address, false);
			cerr << hex << reg << dec;
			reg = reg >> ((offset) * 8);
			reg = reg & (uint64_t)0x00ff;
			unsigned char t = reg;
			cerr << " " << hex << (unsigned int)t << " (0x" << base << ")" << dec << endl;
			data[read_size] = t;
			read_size++;
		} while (read_size != size);
	
		for (unsigned int i = 0; i < size; i++)
			cerr << hex << (unsigned int)(data[i]) << dec << " ";
		cerr << endl;
		return size;
	}
	else if (trans.is_write())
	{
		trans.set_response_status(tlm::TLM_OK_RESPONSE);
		if (size == 0) return size;
		unsigned int write_size = 0;
		do
		{
			uint64_t offset = (addr + write_size) % 4;
			uint64_t base = addr - offset;
			uint32_t reg = 0;
			reg = ReadRegister(base - base_address, false);
			reg = reg & ~((uint32_t)0xff << (offset * 8));
			reg = reg + ((uint32_t)(data[write_size]) << (offset * 8));
			WriteRegister(base - base_address, reg, false);
			write_size++;
		} while (write_size != size);

		return size;
	}
	
	return 0;
}

/* END: methods implementing the "in_mem" socket */

/** Puts an incomming IRQ into the fifo queue to be handled when time has come to do it.
 * @param index		the port that received the changed IRQ
 * @param level		interrupt level
 * @param t			the time that the interrupt is received
 */
template <unsigned int BUS_WIDTH>
void
STR7_EIC<BUS_WIDTH> ::
IRQ(unsigned int index, bool level, sc_core::sc_time& t)
{
	struct irq_fifo_t *entry = new irq_fifo_t();
	entry->index = index;
	entry->level = level;
	irq_fifo.notify(*entry, t);

	if (VerboseRun())
	{
		logger << DebugInfo << "Received interrupt (level = " << level << ") on IRQ port " << index 
			<< " at " << (sc_time_stamp() + t) << " (current time = " << sc_time_stamp() << ")"
			<< EndDebugInfo;
	}
	if (TrapOnIRQ())
		trap_reporting_import->ReportTrap();
}

/** Handle the incomming irqs at the right time */
template <unsigned int BUS_WIDTH>
void
STR7_EIC<BUS_WIDTH> ::
IRQFifoHandler()
{
	sc_core::sc_event &ev = irq_fifo.get_event();
	struct irq_fifo_t *entry = 0;

	while (1)
	{
		wait(ev);
		entry = irq_fifo.get_next_transaction();
		if (entry != 0)
		{
			uint32_t old_irq_status = irq_status;
			while (entry != 0)
			{
				unsigned int index = entry->index;
				bool level = entry->level;
				delete entry;
				if (VerboseRun())
					logger << DebugInfo
						<< "Processing IRQ interrupt received on port " << index << " with level = " << level << " at " << sc_time_stamp()
						<< endl
						<< " - prev irq_status = 0x" << hex << irq_status << dec << endl;
				if (TrapOnIRQHandling())
					trap_reporting_import->ReportTrap();
				if (level)
					irq_status |= ((uint32_t)1 << index);
				else
					irq_status &= ~((uint32_t)1 << index);
				if (VerboseRun())
					logger << " - new irq_status = 0x" << hex << irq_status << dec
						<< EndDebugInfo;
				entry = irq_fifo.get_next_transaction();
			}
			// we need to update the FSM only if the status has really changed, 
			//   and even then only if the changes were to set one of the interrupts
			if (old_irq_status != irq_status)
			{
				uint32_t modified_irq_status = old_irq_status ^ irq_status;
				if (modified_irq_status & irq_status)
					FSMUpdate();
			}
		}
		else
		{
			logger << DebugWarning << "IRQ fifo handler woken up with any available transaction" << endl
				<< TIME(SC_ZERO_TIME) << endl
				<< LOCATION
				<< EndDebugWarning;
			if (TrapOnWarning())
				trap_reporting_import->ReportTrap();
		}
	}
}

/** Puts an incomming FIQ into the fifo queue to be handled when time has come to do it.
 * @param index		the port that received the changed FIQ
 * @param level		interrupt level
 * @param t			the time that the interrupt is received
 */
template <unsigned int BUS_WIDTH>
void
STR7_EIC<BUS_WIDTH> ::
FIQ(unsigned int index, bool level, sc_core::sc_time& t)
{
	struct irq_fifo_t *entry = new irq_fifo_t();
	entry->index = index;
	entry->level = level;
	fiq_fifo.notify(*entry, t);
	
	if (VerboseRun())
	{
		logger << DebugInfo << "Received interrupt (level = " << level << ") on IRQ port " << index 
			<< " at " << (sc_time_stamp() + t) << " (current time = " << sc_time_stamp() << ")"
			<< EndDebugInfo;
	}
	if (TrapOnFIQ())
		trap_reporting_import->ReportTrap();
}

/** Handle the incomming fiqs at the right time */
template <unsigned int BUS_WIDTH>
void
STR7_EIC<BUS_WIDTH> ::
FIQFifoHandler()
{
	sc_core::sc_event &ev = fiq_fifo.get_event();
	struct irq_fifo_t *entry = 0;

	while (1)
	{
		wait(ev);
		entry = fiq_fifo.get_next_transaction();
		if (entry != 0)
		{
			while (entry != 0)
			{
				unsigned int index = entry->index;
				bool level = entry->level;
				delete entry;
				if (VerboseRun())
					logger << DebugInfo
						<< "Processing FIQ interrupt received on port " << index << " with level = " << level << " at " << sc_time_stamp()
						<< EndDebugInfo;
				if (TrapOnFIQHandling())
					trap_reporting_import->ReportTrap();
				if (level)
					fiq_status |= ((uint32_t)1 << index);
				else
					fiq_status &= ~((uint32_t)1 << index);
				entry = fiq_fifo.get_next_transaction();
			}
			FSMUpdate();
		}
		else
		{
			logger << DebugWarning << "FIQ fifo handler woken up with an available transaction" << endl
				<< TIME(SC_ZERO_TIME) << endl
				<< LOCATION
				<< EndDebugWarning;
			if (TrapOnWarning())
				trap_reporting_import->ReportTrap();
		}
	}
}

/** Set FSM to READY state */
template <unsigned int BUS_WIDTH>
void 
STR7_EIC<BUS_WIDTH> ::
FSMReady()
{
	fsm_state = READY;
}

/** Set FSM to WAIT state */
template <unsigned int BUS_WIDTH>
void 
STR7_EIC<BUS_WIDTH> ::
FSMWait()
{
	fsm_state = WAIT;
}

/** Is the FSM at the READY state? */
template <unsigned int BUS_WIDTH>
bool 
STR7_EIC<BUS_WIDTH> ::
IsFSMReady()
{
	return fsm_state == READY;
}

/** Is the FSM at the WAIT state? */
template <unsigned int BUS_WIDTH>
bool 
STR7_EIC<BUS_WIDTH> ::
IsFSMWait()
{
	return fsm_state == WAIT;
}

/** Update the FSM. */
template <unsigned int BUS_WIDTH>
void
STR7_EIC<BUS_WIDTH> ::
FSMUpdate()
{
	/* update the pending interrupt register 0 (IPR0) */
	ipr0 |= IER0() & irq_status;

	/* get the highest priority interrupt if any */
	bool has_irq = false;
	unsigned int index;
	if (ipr0)
	{
		for (int i = 0; i < NUM_IRQ; i++)
		{
			if (ipr0 & ((uint32_t)1 << i))
			{
				if (SIPL(i) > CIPR() &&
					has_irq ? SIPL(i) >= SIPL(index) : true)
				{
					has_irq = true;
					index = i;
				}
			}
		}
		if (VerboseRun())
		{
			logger << DebugInfo << "Found pending interrupts (IPR0 = 0x" << hex << ipr0 << dec
				<< ")";
			if (has_irq)
			{
				logger << ", and determined that irq at index " << index 
					<< " has the highest priority (SIPL[" << index << "] = " << SIPL(index) << ", SIV[" << index << "] = 0x" << hex << SIV(index) << dec << ", CIPR = " << CIPR() << ")";
			}
			else
			{
				logger << ", but determined that none should be served because all has a priority lower that the current one (CIPR = "
					<< CIPR() << ")";
			}
		}
	
		if (has_irq) 
		{
			/* update the IVR registers */
			ivr &= (uint32_t)0xFFFF0000;
			ivr |= SIV(index);
			if (VerboseRun())
				logger << ", setting IVR to 0x" << hex << ivr << dec << EndDebugInfo;

			/* change the FSM state when an irq is detected */
			FSMWait();

			/* the interrupt is signaled only if global irqs (IRQ_EN) are activated */
			has_irq = IRQ_EN();
			new_irq = index;
		}
		else
			if (VerboseRun())
				logger << EndDebugInfo;
	}

	/* send the interrupt level through the out_irq port */
	TLMInterruptPayload *trans = irqPayloadFabric.allocate();
	trans->level = has_irq;
	tlm::tlm_phase phase = tlm::BEGIN_REQ;
	sc_core::sc_time t = SC_ZERO_TIME;
	if (VerboseTLM())
	{
		logger << DebugInfo << "Sending transaction through port out_irq:" << endl
			<< ITRANS(*trans) << endl
			<< PHASE(phase) << endl
			<< TIME(t) << EndDebugInfo;
	}
	switch (out_irq->nb_transport_fw(*trans, phase, t))
	{
		case tlm::TLM_ACCEPTED:
		case tlm::TLM_UPDATED:
			/* nothing else to do wait for the response (which does nothing) */
			break;
		case tlm::TLM_COMPLETED:
			/* release the transaction */
			trans->release();
			break;
	}

	/* the same process is done for the FIQ */
	/* update the pending interrupt register 0 (IPR0) */
	fir = fir | ((FIE() & fiq_status) << 2);

	/* send the interrupt level through the out_fiq port */
	TLMInterruptPayload *trans_fiq = irqPayloadFabric.allocate();
	trans_fiq->level = FIQ_EN() && FIP();
	phase = tlm::BEGIN_REQ;
	t = SC_ZERO_TIME;
	switch (out_fiq->nb_transport_fw(*trans_fiq, phase, t))
	{
		case tlm::TLM_ACCEPTED:
		case tlm::TLM_UPDATED:
			/* nothing else to do, wait for the response (which does nothing) */
			break;
		case tlm::TLM_COMPLETED:
			/* release the transaction */
			trans->release();
			break;
	}
}

/** Pushes the CICR and the CIPR into the stack */
template <unsigned int BUS_WIDTH>
void 
STR7_EIC<BUS_WIDTH> ::
Push()
{
	if (likely(stack_depth < STACK_DEPTH))
	{
		if (VerboseRun())
			logger << DebugInfo << "Pushing into the stack current CICR (0x" << hex << CICR() << dec << ")"
				<< " and CIPR (0x" << hex << CIPR() << dec << ")"
				<< ", stack depth before pushing = " << stack_depth
				<< EndDebugInfo;
		stack[stack_depth].cicr = CICR();
		stack[stack_depth].cipr = CIPR();
		stack_depth++;
	}
	else
	{
		logger << DebugError << "Stack overflow" << endl
			<< LOCATION << EndDebugError;
		sc_stop();
		wait();
	}
}

/** Pops the top of the stack from the stack */
template <unsigned int BUS_WIDTH>
void 
STR7_EIC<BUS_WIDTH> ::
Pop()
{
	uint32_t old_cicr = CICR();
	uint32_t old_cipr = CIPR();

	if (likely(stack_depth != 0))
	{
		stack_depth--;
		cicr = stack[stack_depth].cicr;
		cipr = stack[stack_depth].cipr;
		if (VerboseRun())
			logger << DebugInfo << "Popping from the stack"
				<< " CICR (0x" << hex << CICR() << ", previous = 0x" << old_cicr << ")"
				<< " and CIPR (0x" << CIPR() << ", previous = 0x" << old_cipr << dec << ")"
				<< ", stack depth after pushing = " << stack_depth
				<< EndDebugInfo;
	}
	else
	{
		logger << DebugError << "Stack underflow" << endl
			<< LOCATION << EndDebugError;
		sc_stop();
		wait();
	}
}

/** Read register method and update interrupt controller state as necessary.
 * @param addr		the register address
 * @return 			the contents of the register
 */
template <unsigned int BUS_WIDTH>
uint32_t 
STR7_EIC<BUS_WIDTH> ::
ReadRegister(uint32_t const addr, bool update)
{
	uint32_t index = addr;
	unsigned int sir_index = 0;

	if (addr >= 0x60 && addr <= 0xdc) index = 0x60;
	switch (index)
	{
		case 0x0: // ICR 
			return ICR();
			break;
		case 0x04: // CICR
			return CICR();
			break;
		case 0x08: // CIPR
			return CIPR();
			break;
		case 0x18: // IVR
			if (update)
			{
				if (IsFSMWait())
				{
					Push();
					cicr = new_irq;
					cipr = SIPL(new_irq);
					new_irq = 0;
				}	
				FSMReady();
			}
			return IVR();
			break;
		case 0x1c: // FIR
			return FIR();
			break;
		case 0x20: // IER0
			return IER0();
			break;
		case 0x40: // IPR0
			return IPR0();
			break;
		case 0x60: // SIR
			sir_index = (addr - 0x60) / 4;
			return SIR(sir_index);
			break;
		default:
			return 0;
			break;
	}
	return 0;
}

/** Write register method and update interrupt controller state as necessary.
 * @param addr		the register address
 * @param value		the data to write into the register
 */
template <unsigned int BUS_WIDTH>
void 
STR7_EIC<BUS_WIDTH> ::
WriteRegister(uint32_t addr, uint32_t value, bool update)
{
	unsigned int irq;
	unsigned int has_irq = false;

	uint32_t index = addr;
	uint32_t sir_index = 0;
	if (addr >= 0x60 && addr <= 0xdc) index = 0x60;
	switch (index)
	{
		case 0x0: // ICR
			icr = value & 0x3;
			if (update)
			{
				if (VerboseRun())
				{
					logger << DebugInfo
						<< "Writing ICR with value 0x" << hex << value << dec
						<< ", ICR new value is 0x" << hex << ICR() << dec
						<< EndDebugInfo;
				}
				FSMUpdate();
			}
			break;
		case 0x04: // CICR
			/* can not be modified, ignore write */
			break;
		case 0x08: // CIPR
			if (update)
			{
				if ((value & 0x0f) > CIPR())
					cipr = (value & 0x0f);
			}
			else
				cipr = (value & 0x0f);
			break;
		case 0x18: // IVR
			ivr = value & (uint32_t)0xffff0000ULL;
			break;
		case 0x1c: // FIR
			if (value & 0x3) // modifies FIE
			{
				fir &= ~(uint32_t)0x3;
				fir |= (value & (uint32_t)0x3);
			}
			if ((value >> 2) & 0x3) // modifies FIP
			{
				for (unsigned int i = 0; i < NUM_FIQ; i++)
				{
					if (value & (0x01 << (i + 2)) && FIP() & (0x01 << i))
						fir = fir & ~((uint32_t)0x01 << (i + 2));
				}
				if (update)
					FSMUpdate();
			}
			break;
		case 0x20: // IER0
			ier0 = value;
			break;
		case 0x40: // IPR0
			if (VerboseRun())
				logger << DebugInfo << "Writing IPR0 with value 0x" << hex << value << dec
					<< " (old IPR0 = 0x" << hex << IPR0() << dec << ", ";
			if (value)
			{
				for (unsigned int i = 0; i < NUM_IRQ; i++)
				{
					if ((value & ((uint32_t)0x01 << i)) && (IPR0() & ((uint32_t)0x01 << i)))
					{
						if (!has_irq)
						{
							has_irq = true;
							irq = i;
						}
						ipr0 = ipr0 & ~((uint32_t)0x01 << i);
					}
				}
			}
			if (VerboseRun())
				logger << "new IPR0 = 0X" << hex << IPR0() << dec << ")" << EndDebugInfo;
			if (update)
			{
				if (has_irq)
				{
					if (irq == CICR())
					{
						Pop();
					}
					else
					{
						if (VerboseRun())
							logger << DebugInfo << "Could not pop the stack because the removed irq (" << irq << ") is different than the CICR (" << CICR() << ")"
								<< EndDebugInfo;
					}
				}
				FSMUpdate();
			}
			break;
		case 0x60: // SIR
			sir_index = (addr - 0x60) / 4;
			sir[sir_index] = value & (uint32_t)0xffff000f;
			break;
		default:
			break;
	}
}

/* START: interrupt controller accessors */

template <unsigned int BUS_WIDTH>
uint32_t
STR7_EIC<BUS_WIDTH> ::
ICR()
{
	return icr;
}

template <unsigned int BUS_WIDTH>
uint32_t
STR7_EIC<BUS_WIDTH> ::
CICR()
{
	return cicr;
}

template <unsigned int BUS_WIDTH>
uint32_t
STR7_EIC<BUS_WIDTH> ::
CIPR()
{
	return cipr;
}

template <unsigned int BUS_WIDTH>
uint32_t
STR7_EIC<BUS_WIDTH> ::
IVR()
{
	return ivr;
}

template <unsigned int BUS_WIDTH>
uint32_t
STR7_EIC<BUS_WIDTH> ::
FIR()
{
	return fir;
}

template <unsigned int BUS_WIDTH>
uint32_t
STR7_EIC<BUS_WIDTH> ::
IER0()
{
	return ier0;
}

template <unsigned int BUS_WIDTH>
uint32_t
STR7_EIC<BUS_WIDTH> ::
IPR0()
{
	return ipr0;
}

template <unsigned int BUS_WIDTH>
uint32_t
STR7_EIC<BUS_WIDTH> ::
SIR(unsigned int index)
{
	return sir[index];
}

template <unsigned int BUS_WIDTH>
bool
STR7_EIC<BUS_WIDTH> ::
FIQ_EN()
{
	return ICR() & (0x01 << 1);
}

template <unsigned int BUS_WIDTH>
bool
STR7_EIC<BUS_WIDTH> ::
IRQ_EN()
{
	return ICR() & 0x01;
}

template <unsigned int BUS_WIDTH>
uint32_t
STR7_EIC<BUS_WIDTH> ::
CIC()
{
	return CICR() & (uint32_t)0x01f;
}

template <unsigned int BUS_WIDTH>
uint32_t
STR7_EIC<BUS_WIDTH> ::
CIP()
{
	return CIPR() & (uint32_t)0x0f;
}

template <unsigned int BUS_WIDTH>
uint32_t
STR7_EIC<BUS_WIDTH> ::
IVRhigh()
{
	return IVR() >> 16;
}

template <unsigned int BUS_WIDTH>
uint32_t
STR7_EIC<BUS_WIDTH> ::
IVRlow()
{
	return IVR() & (uint32_t)0x0ffff;
}

template <unsigned int BUS_WIDTH>
uint32_t
STR7_EIC<BUS_WIDTH> ::
FIP()
{
	return (FIR() >> 2) & (uint32_t)0x03;
}

template <unsigned int BUS_WIDTH>
uint32_t
STR7_EIC<BUS_WIDTH> ::
FIE()
{
	return FIR() & (uint32_t)0x03;
}

template <unsigned int BUS_WIDTH>
uint32_t
STR7_EIC<BUS_WIDTH> ::
SIV(unsigned int index)
{
	return SIR(index) >> 16;
}

template <unsigned int BUS_WIDTH>
uint32_t
STR7_EIC<BUS_WIDTH> ::
SIPL(unsigned int index)
{
	return SIR(index) & (uint32_t)0x0f;
}

/* END: interrupt controller accessors */

/* START: verbose methods */

template <unsigned int BUS_WIDTH>
bool
STR7_EIC<BUS_WIDTH> ::
VerboseAll()
{
	return verbose_all;
}

template <unsigned int BUS_WIDTH>
bool
STR7_EIC<BUS_WIDTH> ::
VerboseSetup()
{
	return verbose_setup;
}

template <unsigned int BUS_WIDTH>
bool
STR7_EIC<BUS_WIDTH> ::
VerboseRun()
{
	return verbose_run;
}

template <unsigned int BUS_WIDTH>
bool
STR7_EIC<BUS_WIDTH> ::
VerboseTLM()
{
	return verbose_tlm;
}

/* END: verbose methods */

/* START: trap methods */
template <unsigned int BUS_WIDTH>
bool
STR7_EIC<BUS_WIDTH> ::
TrapOnIRQ()
{
	return trap_on_irq && trap_reporting_import;
}

template <unsigned int BUS_WIDTH>
bool
STR7_EIC<BUS_WIDTH> ::
TrapOnFIQ()
{
	return trap_on_fiq && trap_reporting_import;
}

template <unsigned int BUS_WIDTH>
bool
STR7_EIC<BUS_WIDTH> ::
TrapOnIRQHandling()
{
	return trap_on_irq_handling && trap_reporting_import;
}
	
template <unsigned int BUS_WIDTH>
bool
STR7_EIC<BUS_WIDTH> ::
TrapOnFIQHandling()
{
	return trap_on_fiq_handling && trap_reporting_import;
}

template <unsigned int BUS_WIDTH>
bool
STR7_EIC<BUS_WIDTH>::
TrapOnWarning()
{
	return trap_on_warning && trap_reporting_import;
}

/* END: trap methods */
} // end of namespace str7_eic
} // end of namespace interrupt
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_INTERRUPT_STR7_EIC_STR7_EIC_TCC__


