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
 
#ifndef __UNISIM_COMPONENT_TLM2_TIMER_STR7_TIM_TCC__
#define __UNISIM_COMPONENT_TLM2_TIMER_STR7_TIM_TCC__

#include "unisim/util/endian/endian.hh"

#define LOCATION 	" - location = " << __FUNCTION__ << ":unisim_lib/unisim/component/tlm2/timer/str7_timer/tim.tcc:" << __LINE__
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

namespace unisim {
namespace component {
namespace tlm2 {
namespace timer {
namespace str7_tim {

using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::EndDebug;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;
using unisim::util::endian::Host2LittleEndian;
using unisim::util::endian::LittleEndian2Host;

template<unsigned int BUSWIDTH, bool VERBOSE>
TIM<BUSWIDTH, VERBOSE> ::
TIM(const sc_module_name &name, Object *parent) :
	Object(name, parent),
	sc_module(name),
	timeri_irq("timeri_irq"),
	ocmpa_irq("ocmpa_irq"),
	ocmpb_irq("ocmpb_irq"),
	icapa_irq("icapa_irq"),
	icapb_irq("icapb_irq"),
	in_mem("in_mem"),
	icar(0),
	icbr(0),
	ocar((uint16_t)0x8000),
	ocbr((uint16_t)0x8000),
	cntr((uint16_t)0xfffc),
	cr1(0),
	cr2(0),
	sr(0),
	mem_fifo("mem_fifo"),
	icapa_fifo("icapa_fifo"),
	icapb_fifo("icapb_fifo"),
	base_address(0),
	param_base_address("base-address", this, base_address, "Timer base address"),
	pclk2_cycle_time_double(0.0),
	pclk2_cycle_time(SC_ZERO_TIME),
	param_pclk2_cycle_time_double("pclk2-cycle-time", this, pclk2_cycle_time_double, "PCLK2 cycle time in PS"),
	extclk_cycle_time_double(0.0),
	extclk_cycle_time(SC_ZERO_TIME),
	param_extclk_cycle_time_double("extclk-cycle-time", this, extclk_cycle_time_double, "EXTCLK cycle time in PS"),
	inverse_icapa(false),
	param_inverse_icapa("inverse-icapa", this, inverse_icapa, "consider no interruption like interruption, and interruption like no interruption for irq capture A"),
	inverse_icapb(false),
	param_inverse_icapb("inverse-icapb", this, inverse_icapb, "consider no interruption like interruption, and interruption like no interruption for irq capture B"),
	logger(*this),
	verbose_all(false),
	verbose_setup(false),
	verbose_run(false),
	verbose_tlm(false)
{
	SC_HAS_PROCESS(TIM);
	SC_THREAD(MemFifoHandler);
	SC_THREAD(IcapaFifoHandler);
	SC_THREAD(IcapbFifoHandler);

	if (VERBOSE)
	{
		param_verbose_all = new unisim::kernel::service::Parameter<bool>("verbose-all", this, verbose_all, "Display all the verbose information");
		param_verbose_setup = new unisim::kernel::service::Parameter<bool>("verbose-setup", this, verbose_setup, "Display module setup verbose information");
		param_verbose_run = new unisim::kernel::service::Parameter<bool>("verbose-run", this, verbose_run, "Display module run verbose information");
		param_verbose_tlm = new unisim::kernel::service::Parameter<bool>("verbose-tlm", this, verbose_tlm, "Display module tlm verbose information");
	}

	/* register the callbacks */
	timeri_irq.register_nb_transport_bw(this, &THIS_MODULE::TimeriNbTransportBw);
	timeri_irq.register_invalidate_direct_mem_ptr(this, &THIS_MODULE::TimeriInvalidateDirectMemPtr);
	ocmpa_irq.register_nb_transport_bw(this, &THIS_MODULE::OCMPANbTransportBw);
	ocmpa_irq.register_invalidate_direct_mem_ptr(this, &THIS_MODULE::OCMPAInvalidateDirectMemPtr);
	ocmpb_irq.register_nb_transport_bw(this, &THIS_MODULE::OCMPBNbTransportBw);
	ocmpb_irq.register_invalidate_direct_mem_ptr(this, &THIS_MODULE::OCMPBInvalidateDirectMemPtr);
	icapa_irq.register_nb_transport_fw(this, &THIS_MODULE::ICAPANbTransportFw);
	icapa_irq.register_b_transport(this, &THIS_MODULE::ICAPABTransport);
	icapa_irq.register_get_direct_mem_ptr(this, &THIS_MODULE::ICAPAGetDirectMemPtr);
	icapa_irq.register_transport_dbg(this, &THIS_MODULE::ICAPATransportDbg);
	icapb_irq.register_nb_transport_fw(this, &THIS_MODULE::ICAPBNbTransportFw);
	icapb_irq.register_b_transport(this, &THIS_MODULE::ICAPBBTransport);
	icapb_irq.register_get_direct_mem_ptr(this, &THIS_MODULE::ICAPBGetDirectMemPtr);
	icapb_irq.register_transport_dbg(this, &THIS_MODULE::ICAPBTransportDbg);
	in_mem.bind(*this);
}

template<unsigned int BUSWIDTH, bool VERBOSE>
TIM<BUSWIDTH, VERBOSE> ::
~TIM()
{
	if (param_verbose_all) delete param_verbose_all;
	if (param_verbose_run) delete param_verbose_run;
	if (param_verbose_setup) delete param_verbose_setup;
	if (param_verbose_tlm) delete param_verbose_tlm;
}

template<unsigned int BUSWIDTH, bool VERBOSE>
bool
TIM<BUSWIDTH, VERBOSE> ::
Setup()
{
	if (VerboseAll())
		verbose_setup = verbose_run = verbose_tlm = true;

	if (pclk2_cycle_time_double <= 0.0)
	{
		logger << DebugError << "pclk2-cycle-time must be bigger than 0" << EndDebug;
		return false;
	}
	
	pclk2_cycle_time = sc_time(pclk2_cycle_time_double, SC_PS);

	if (extclk_cycle_time_double < 0.0) extclk_cycle_time_double = 0.0;
	if (extclk_cycle_time_double == 0.0)
	{
		logger << DebugWarning << "extclk-cycle-time should be bigger than 0 (otherwise it should not be used)" << EndDebug;
	}
	else
	{
		if ((extclk_cycle_time_double * 4) < pclk2_cycle_time_double)
		{
			logger << DebugError << "extclk-cycle-time should be at least four times bigger than pclk2-cycle-time" << EndDebug;
			return false;
		}
	}

	extclk_cycle_time = sc_time(extclk_cycle_time_double, SC_PS);

	if (VerboseSetup())
	{
		logger << DebugInfo << "Verbose options:" << endl
			<< " - verbose-setup activated" << endl
			<< " - verbose-run ";
		if (VerboseRun())
			logger << "activated";
		else
			logger << "deactivated";
		logger << endl
			<< " - verbose-tlm ";
		if (VerboseTLM())
			logger << "activated";
		else
			logger << "deactivated";
		logger << endl
			<< "Base address = 0x" << hex << base_address << dec << endl;
		logger << "PCLK2 cycle time = " << pclk2_cycle_time << endl;
		logger << "EXTCLk cycle time = " << extclk_cycle_time;
		if (extclk_cycle_time_double == 0.0)
			logger << " (should not be used during the simulation)";
		if (inverse_icapa)
			logger << endl << "The inversed value of icapa will be used";
		if (inverse_icapb)
			logger << endl << "The inversed value of icapb will be used";
		logger << EndDebugInfo;
	}

	return true;
}

/* START: output timer interrupt socket methods */
template<unsigned int BUSWIDTH, bool VERBOSE>
tlm::tlm_sync_enum
TIM<BUSWIDTH, VERBOSE> ::
TimeriNbTransportBw(TLMInterruptPayload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t)
{
	switch (phase)
	{
		case tlm::END_REQ:
			// nothing to do just complete the transaction
			if (VerboseTLM())
			{
				logger << DebugInfo << "Received transaction on timeri_irq in nb_transport_bw, "
					<< "completing the transaction" << endl
					<< ITRANS(trans) << endl
					<< PHASE(phase) << endl
					<< TIME(t) << EndDebug;
			}
			break;
		case tlm::BEGIN_REQ:
		case tlm::BEGIN_RESP:
		case tlm::END_RESP:
			logger << DebugWarning << "Received unexpected transaction phase on timeri_irq" << endl
				<< LOCATION << endl
				<< ITRANS(trans) << endl
				<< PHASE(phase) << endl
				<< TIME(t)
				<< EndDebugWarning;
			break;
	}
	return tlm::TLM_COMPLETED;
}

template<unsigned int BUSWIDTH, bool VERBOSE>
void
TIM<BUSWIDTH, VERBOSE> ::
TimeriInvalidateDirectMemPtr(sc_dt::uint64 start_range, sc_dt::uint64 end_range)
{
	/* not supported */
}

/* END: output timer interrupt socket methods */

/* START: output compare A (OCMPA) interrupt socket methods */

template<unsigned int BUSWIDTH, bool VERBOSE>
tlm::tlm_sync_enum
TIM<BUSWIDTH, VERBOSE> ::
OCMPANbTransportBw(TLMInterruptPayload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t)
{
	switch (phase)
	{
		case tlm::END_REQ:
			// nothing to do just complete the transaction
			if (VerboseTLM())
			{
				logger << DebugInfo << "Received transaction on ocmpa_irq in nb_transport_bw, "
					<< "completing the transaction" << endl
					<< ITRANS(trans) << endl
					<< PHASE(phase) << endl
					<< TIME(t) << EndDebug;
			}
			break;
		case tlm::BEGIN_REQ:
		case tlm::BEGIN_RESP:
		case tlm::END_RESP:
			logger << DebugWarning << "Received unexpected transaction phase on ocmpa_irq" << endl
				<< LOCATION << endl
				<< ITRANS(trans) << endl
				<< PHASE(phase) << endl
				<< TIME(t)
				<< EndDebugWarning;
			break;
	}
	return tlm::TLM_COMPLETED;
}

template<unsigned int BUSWIDTH, bool VERBOSE>
void
TIM<BUSWIDTH, VERBOSE> ::
OCMPAInvalidateDirectMemPtr(sc_dt::uint64 start_range, sc_dt::uint64 end_range)
{
	/* not supported */
}

/* END: output compare A (OCMPA) interrupt socket methods */

/* START: output compare A (OCMPB) interrupt socket methods */

template<unsigned int BUSWIDTH, bool VERBOSE>
tlm::tlm_sync_enum
TIM<BUSWIDTH, VERBOSE> ::
OCMPBNbTransportBw(TLMInterruptPayload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t)
{
	switch (phase)
	{
		case tlm::END_REQ:
			// nothing to do just complete the transaction
			if (VerboseTLM())
			{
				logger << DebugInfo << "Received transaction on ocmpb_irq in nb_transport_bw, "
					<< "completing the transaction" << endl
					<< ITRANS(trans) << endl
					<< PHASE(phase) << endl
					<< TIME(t) << EndDebug;
			}
			break;
		case tlm::BEGIN_REQ:
		case tlm::BEGIN_RESP:
		case tlm::END_RESP:
			logger << DebugWarning << "Received unexpected transaction phase on ocmpb_irq" << endl
				<< LOCATION << endl
				<< ITRANS(trans) << endl
				<< PHASE(phase) << endl
				<< TIME(t)
				<< EndDebugWarning;
			break;
	}
	return tlm::TLM_COMPLETED;
}

template<unsigned int BUSWIDTH, bool VERBOSE>
void
TIM<BUSWIDTH, VERBOSE> ::
OCMPBInvalidateDirectMemPtr(sc_dt::uint64 start_range, sc_dt::uint64 end_range)
{
	/* not supported */
}

/* END: output compare A (OCMPB) interrupt socket methods */

/* START: input capture A (ICAPA) interrupt socket methods */

template<unsigned int BUSWIDTH, bool VERBOSE>
tlm::tlm_sync_enum
TIM<BUSWIDTH, VERBOSE> ::
ICAPANbTransportFw(TLMInterruptPayload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t)
{
	irq_fifo_t *entry = 0;

	switch(phase)
	{
		case tlm::BEGIN_REQ:
			/* queue the transaction for future handling */
			if (free_irq_fifo.empty())
			{
				entry = new irq_fifo_t();
				free_irq_fifo.push(entry);
			}
			entry = free_irq_fifo.front();
			free_irq_fifo.pop();
			entry->level = trans.level;
			icapa_fifo.notify(*entry, t);
			/* increment the phase */
			phase = tlm::END_REQ;
			return tlm::TLM_COMPLETED;
			break;
		case tlm::END_RESP:
			/* nothing needs to be done just complete the transaction */
			return tlm::TLM_COMPLETED;
			break;
		case tlm::END_REQ:
		case tlm::BEGIN_RESP:
			logger << DebugError << "Received unexpected phase on icapa_irq nb_transport_fw handler" << endl
				<< LOCATION << endl
				<< ITRANS(trans) << endl
				<< PHASE(phase) << endl
				<< TIME(t) << EndDebugError;
			sc_stop();
			wait();
			break;
	}
}

template<unsigned int BUSWIDTH, bool VERBOSE>
void
TIM<BUSWIDTH, VERBOSE> ::
ICAPABTransport(TLMInterruptPayload& trans, sc_core::sc_time& t)
{
	wait(t);
	ProcessICAPA(trans.level);
	return;
}

template<unsigned int BUSWIDTH, bool VERBOSE>
bool
TIM<BUSWIDTH, VERBOSE> ::
ICAPAGetDirectMemPtr(TLMInterruptPayload& trans, tlm::tlm_dmi& dmi_data)
{
	/* not supported */
	return false;
}

template<unsigned int BUSWIDTH, bool VERBOSE>
unsigned int
TIM<BUSWIDTH, VERBOSE> ::
ICAPATransportDbg(TLMInterruptPayload& trans)
{
	/* no sense for interrupt */
	return 0;
}

/* END: input capture A (ICAPA) interrupt socket methods */

/* START: input capture B (ICAPB) interrupt socket methods */

template<unsigned int BUSWIDTH, bool VERBOSE>
tlm::tlm_sync_enum
TIM<BUSWIDTH, VERBOSE> ::
ICAPBNbTransportFw(TLMInterruptPayload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t)
{
	irq_fifo_t *entry = 0;

	switch(phase)
	{
		case tlm::BEGIN_REQ:
			/* queue the transaction for future handling */
			if (free_irq_fifo.empty())
			{
				entry = new irq_fifo_t();
				free_irq_fifo.push(entry);
			}
			entry = free_irq_fifo.front();
			free_irq_fifo.pop();
			entry->level = trans.level;
			icapb_fifo.notify(*entry, t);
			/* increment the phase */
			phase = tlm::END_REQ;
			return tlm::TLM_COMPLETED;
			break;
		case tlm::END_RESP:
			/* nothing needs to be done just complete the transaction */
			return tlm::TLM_COMPLETED;
			break;
		case tlm::END_REQ:
		case tlm::BEGIN_RESP:
			logger << DebugError << "Received unexpected phase on icapb_irq nb_transport_fw handler" << endl
				<< LOCATION << endl
				<< ITRANS(trans) << endl
				<< PHASE(phase) << endl
				<< TIME(t) << EndDebugError;
			sc_stop();
			wait();
			break;
	}
}

template<unsigned int BUSWIDTH, bool VERBOSE>
void
TIM<BUSWIDTH, VERBOSE> ::
ICAPBBTransport(TLMInterruptPayload& trans, sc_core::sc_time& t)
{
	wait(t);
	ProcessICAPB(trans.level);
	return;
}

template<unsigned int BUSWIDTH, bool VERBOSE>
bool
TIM<BUSWIDTH, VERBOSE> ::
ICAPBGetDirectMemPtr(TLMInterruptPayload& trans, tlm::tlm_dmi& dmi_data)
{
	/* not supported */
	return false;
}

template<unsigned int BUSWIDTH, bool VERBOSE>
unsigned int
TIM<BUSWIDTH, VERBOSE> ::
ICAPBTransportDbg(TLMInterruptPayload& trans)
{
	/* not supported */
	return 0;
}

/* END: input capture B (ICAPB) interrupt socket methods */
	
/* START: input memory socket methods */

template<unsigned int BUSWIDTH, bool VERBOSE>
tlm::tlm_sync_enum
TIM<BUSWIDTH, VERBOSE> ::
nb_transport_fw(tlm::tlm_generic_payload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t)
{
	switch(phase)
	{
		case tlm::BEGIN_REQ:
			/* acquire the transaction */
			trans.acquire();
			/* queue the transaction for future handling */
			mem_fifo.notify(trans, t);
			/* increment the phase */
			phase = tlm::END_REQ;
			/* signalize that the transaction has been accepted or completed if it is a write */
			if (trans.is_read()) return tlm::TLM_ACCEPTED;
			return tlm::TLM_COMPLETED;
			break;
		case tlm::END_RESP:
			/* nothing needs to be done just complete the transaction */
			trans.set_response_status(tlm::TLM_OK_RESPONSE);
			return tlm::TLM_COMPLETED;
			break;
		case tlm::END_REQ:
		case tlm::BEGIN_RESP:
			logger << DebugError << "Received unexpected phase on in_mem nb_transport_fw handler" << endl
				<< LOCATION << endl;
			TRANS(logger, trans);
			logger << endl
				<< PHASE(phase) << endl
				<< TIME(t) << EndDebugError;
			sc_stop();
			wait();
			break;
	}
}

template<unsigned int BUSWIDTH, bool VERBOSE>
void
TIM<BUSWIDTH, VERBOSE> ::
b_transport(tlm::tlm_generic_payload& trans, sc_core::sc_time& t)
{
	/* wait for the given time before performing the operation */
	wait(t);
	/* set t to the time it takes to perform the operation */
	t = pclk2_cycle_time;
	/* check if the transaction is read, write or ignore operation, and perform it */
	uint16_t val;
	switch(trans.get_command())
	{
		case tlm::TLM_READ_COMMAND:
			trans.set_response_status(ReadRegister(trans.get_address(), val));
			if (trans.is_response_ok())
			{
				(*(uint16_t *)trans.get_data_ptr()) = Host2LittleEndian(val);
			}
			break;
		case tlm::TLM_WRITE_COMMAND:
			val = LittleEndian2Host(*(uint16_t *)trans.get_data_ptr());
			trans.set_response_status(WriteRegister(trans.get_address(), val));
			break;
		case tlm::TLM_IGNORE_COMMAND:
			trans.set_response_status(tlm::TLM_OK_RESPONSE);
			break;
	}
}

template<unsigned int BUSWIDTH, bool VERBOSE>
bool
TIM<BUSWIDTH, VERBOSE> ::
get_direct_mem_ptr(tlm::tlm_generic_payload& trans, tlm::tlm_dmi& dmi_data)
{
	/* not supported */
	return false;
}

template<unsigned int BUSWIDTH, bool VERBOSE>
unsigned int
TIM<BUSWIDTH, VERBOSE> ::
transport_dbg(tlm::tlm_generic_payload& trans)
{
	return 0;
}

/* END: input memory socket methods */

/* Memory requests fifo handler */
template<unsigned int BUSWIDTH, bool VERBOSE>
void
TIM<BUSWIDTH, VERBOSE> ::
MemFifoHandler()
{
	sc_time t = SC_ZERO_TIME;

	while (1)
	{
		wait(mem_fifo.get_event());
		tlm::tlm_generic_payload *trans = mem_fifo.get_next_transaction();
		if (!trans) continue;

		uint16_t val;
		tlm::tlm_phase phase;
		switch (trans->get_command())
		{
			case tlm::TLM_READ_COMMAND:
				trans->set_response_status(ReadRegister(trans->get_address(), val));
				if (trans->is_response_ok())
					(*(uint16_t *)trans->get_data_ptr()) = Host2LittleEndian(val);
				/* send the transaction */
				phase = tlm::BEGIN_RESP;
				t = SC_ZERO_TIME;
				switch (in_mem->nb_transport_bw(*trans, phase, t))
				{
					case tlm::TLM_ACCEPTED:
					case tlm::TLM_UPDATED:
						/* the transaction has been taken by the receiver, we are done.
						 * The nb_transport_fw will wait for the end of the response transaction */
						break;
					case tlm::TLM_COMPLETED:
						if (trans->is_response_error())
						{
							logger << DebugError << "Memory response failed for transaction, stopping simulation:"
								<< LOCATION << endl;
							TRANS(logger, *trans);
							logger << endl
								<< PHASE(phase) << endl
								<< TIME(t) << EndDebug;
							sc_stop();
							wait();
						}
						break;
				}
				/* release the transaction */
				trans->release();
				trans = 0;
				break;
			case tlm::TLM_WRITE_COMMAND:
				val = LittleEndian2Host(*trans->get_data_ptr());
				trans->set_response_status(WriteRegister(trans->get_address(), val));
				trans->release();
				break;
		}
	}
}

/* ICAPA requests fifo handler */
template<unsigned int BUSWIDTH, bool VERBOSE>
void
TIM<BUSWIDTH, VERBOSE> ::
IcapaFifoHandler()
{
	while (1)
	{
		wait(icapa_fifo.get_event());
		irq_fifo_t *irq = icapa_fifo.get_next_transaction();
		ProcessICAPA(irq->level);
		free_irq_fifo.push(irq);
	}
}

/* ICAPB requests fifo handler */
template<unsigned int BUSWIDTH, bool VERBOSE>
void
TIM<BUSWIDTH, VERBOSE> ::
IcapbFifoHandler()
{
	while (1)
	{
		wait(icapb_fifo.get_event());
		irq_fifo_t *irq = icapa_fifo.get_next_transaction();
		ProcessICAPB(irq->level);
		free_irq_fifo.push(irq);
	}
}

template<unsigned int BUSWIDTH, bool VERBOSE>
tlm::tlm_response_status
TIM<BUSWIDTH, VERBOSE> ::
ReadRegister(uint64_t addr, uint16_t &value)
{
}

template<unsigned int BUSWIDTH, bool VERBOSE>
tlm::tlm_response_status 
TIM<BUSWIDTH, VERBOSE> ::
WriteRegister(uint64_t addr, uint16_t value)
{
}

template<unsigned int BUSWIDTH, bool VERBOSE>
void
TIM<BUSWIDTH, VERBOSE> ::
ProcessICAPA(bool level)
{
}

template<unsigned int BUSWIDTH, bool VERBOSE>
void
TIM<BUSWIDTH, VERBOSE> ::
ProcessICAPB(bool level)
{
}

} // end of namespace str7_tim
} // end of namespace timer
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_TIMER_STR7_TIMER_HH__

