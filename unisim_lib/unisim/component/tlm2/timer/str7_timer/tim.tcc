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
namespace str7_timer {

using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::EndDebug;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;
using unisim::util::endian::Host2LittleEndian;
using unisim::util::endian::LittleEndian2Host;

template<unsigned int BUSWIDTH>
TIM<BUSWIDTH> ::
TIM(const sc_module_name &name, Object *parent) :
	unisim::kernel::service::Object(name, parent),
	Client<TrapReporting>(name, parent),
	sc_module(name),
	timeri_irq("timeri_irq"),
	toi_irq("toi_irq"),
	icia_irq("icia_irq"),
	icib_irq("icib_irq"),
	ocia_irq("ocia_irq"),
	ocib_irq("ocib_irq"),
	ocmpa_edge("ocmpa_edge"),
	ocmpb_edge("ocmpb_edge"),
	icapa_edge("icapa_edge"),
	icapb_edge("icapb_edge"),
	in_mem("in_mem"),
	trap_reporting_import("trap_reporting_import", this),
	icar(0),
	icbr(0),
	ocar((uint16_t)0x8000),
	ocbr((uint16_t)0x8000),
	cntr((uint16_t)0xfffc),
	cr1(0),
	cr2(0),
	sr(0),
	icapa_edge_prev(false),
	icapb_edge_prev(false),
	mem_fifo("mem_fifo"),
	icapa_fifo("icapa_fifo"),
	icapb_fifo("icapb_fifo"),
	ocmpa_prev(false),
	ocmpb_prev(false),
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
	param_verbose_all("verbose-all", this, verbose_all, "Display all the verbose information"),
	verbose_setup(false),
	param_verbose_setup("verbose-setup", this, verbose_setup, "Display module setup verbose information"),
	verbose_run(false),
	param_verbose_run("verbose-run", this, verbose_run, "Display module run verbose information"),
	trap_on_verbose_run(false),
	param_verbose_tlm("verbose-tlm", this, verbose_tlm, "Display module tlm verbose information"),
	verbose_tlm(false),
	param_trap_on_verbose_run("trap-on-verbose-run", this, trap_on_verbose_run, "Send a trap when displaying run verbose information (requires verbose-run to be also activated)"),
	trap_on_verbose_tlm(false),
	param_trap_on_verbose_tlm("trap-on-verbose-tlm", this, trap_on_verbose_tlm, "Send a trap when displaying tlm verbose information (requires verbose-tlm to be also activated)"),
	trap_on_warning(false),
	param_trap_on_warning("trap-on-warning", this, trap_on_warning, "Send a trap when displaying a warning message"),
	reg_icar("ICAR", this, icar, "ICAR register"),
	reg_icbr("ICBR", this, icbr, "ICBR register"),
	reg_ocar("OCAR", this, ocar, "OCAR register"),
	reg_ocbr("OCBR", this, ocbr, "OCBR register"),
	reg_cntr("CNTR", this, cntr, "CNTR register"),
	reg_cr1("CR1", this, cr1, "CR1 register"),
	reg_cr2("CR2", this, cr2, "CR2 register"),
	reg_sr("SR", this, sr, "SR register")
{
	SC_HAS_PROCESS(TIM);
	SC_THREAD(MemFifoHandler);
	SC_THREAD(IcapaFifoHandler);
	SC_THREAD(IcapbFifoHandler);
	SC_THREAD(TimerOverflowHandler);
	SC_THREAD(OutputCompareAHandler);
	SC_THREAD(OutputCompareBHandler);

	/* register the callbacks */
	timeri_irq.register_nb_transport_bw(this, &THIS_MODULE::TimeriNbTransportBw);
	timeri_irq.register_invalidate_direct_mem_ptr(this, &THIS_MODULE::TimeriInvalidateDirectMemPtr);
	toi_irq.register_nb_transport_bw(this, &THIS_MODULE::TOINbTransportBw);
	toi_irq.register_invalidate_direct_mem_ptr(this, &THIS_MODULE::TOIInvalidateDirectMemPtr);
	icia_irq.register_nb_transport_bw(this, &THIS_MODULE::ICIANbTransportBw);
	icia_irq.register_invalidate_direct_mem_ptr(this, &THIS_MODULE::ICIAInvalidateDirectMemPtr);
	icib_irq.register_nb_transport_bw(this, &THIS_MODULE::ICIBNbTransportBw);
	icib_irq.register_invalidate_direct_mem_ptr(this, &THIS_MODULE::ICIBInvalidateDirectMemPtr);
	ocia_irq.register_nb_transport_bw(this, &THIS_MODULE::OCIANbTransportBw);
	ocia_irq.register_invalidate_direct_mem_ptr(this, &THIS_MODULE::OCIAInvalidateDirectMemPtr);
	ocib_irq.register_nb_transport_bw(this, &THIS_MODULE::OCIBNbTransportBw);
	ocib_irq.register_invalidate_direct_mem_ptr(this, &THIS_MODULE::OCIBInvalidateDirectMemPtr);
	ocmpa_edge.register_nb_transport_bw(this, &THIS_MODULE::OCMPANbTransportBw);
	ocmpa_edge.register_invalidate_direct_mem_ptr(this, &THIS_MODULE::OCMPAInvalidateDirectMemPtr);
	ocmpb_edge.register_nb_transport_bw(this, &THIS_MODULE::OCMPBNbTransportBw);
	ocmpb_edge.register_invalidate_direct_mem_ptr(this, &THIS_MODULE::OCMPBInvalidateDirectMemPtr);
	icapa_edge.register_nb_transport_fw(this, &THIS_MODULE::ICAPANbTransportFw);
	icapa_edge.register_b_transport(this, &THIS_MODULE::ICAPABTransport);
	icapa_edge.register_get_direct_mem_ptr(this, &THIS_MODULE::ICAPAGetDirectMemPtr);
	icapa_edge.register_transport_dbg(this, &THIS_MODULE::ICAPATransportDbg);
	icapb_edge.register_nb_transport_fw(this, &THIS_MODULE::ICAPBNbTransportFw);
	icapb_edge.register_b_transport(this, &THIS_MODULE::ICAPBBTransport);
	icapb_edge.register_get_direct_mem_ptr(this, &THIS_MODULE::ICAPBGetDirectMemPtr);
	icapb_edge.register_transport_dbg(this, &THIS_MODULE::ICAPBTransportDbg);
	in_mem.bind(*this);
}

template<unsigned int BUSWIDTH>
TIM<BUSWIDTH> ::
~TIM()
{
}

template<unsigned int BUSWIDTH>
bool
TIM<BUSWIDTH> ::
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

	if (verbose_setup)
	{
		logger << DebugInfo << "Verbose options:" << endl
			<< " - verbose-setup activated" << endl
			<< " - verbose-run ";
		if(unlikely(verbose_run))
			logger << "activated";
		else
			logger << "deactivated";
		logger << endl
			<< " - verbose-tlm ";
		if (verbose_tlm)
			logger << "activated";
		else
			logger << "deactivated";
		logger << endl
			<< " - trap-on-verbose-run ";
		if (trap_on_verbose_run)
			logger << "activated";
		else
			logger << "deactivated";
		logger << endl
			<< " - trap-on-verbose-tlm ";
		if (trap_on_verbose_tlm)
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

/* START: output timer global interrupt socket methods */
template<unsigned int BUSWIDTH>
tlm::tlm_sync_enum
TIM<BUSWIDTH> ::
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
			TrapOnWarning();
			break;
	}
	return tlm::TLM_COMPLETED;
}

template<unsigned int BUSWIDTH>
void
TIM<BUSWIDTH> ::
TimeriInvalidateDirectMemPtr(sc_dt::uint64 start_range, sc_dt::uint64 end_range)
{
	/* not supported */
}

/* END: output timer global interrupt socket methods */

/* START: output timer overflow interrupt (TOI) socket methods */
template<unsigned int BUSWIDTH>
tlm::tlm_sync_enum
TIM<BUSWIDTH> ::
TOINbTransportBw(TLMInterruptPayload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t)
{
	switch (phase)
	{
		case tlm::END_REQ:
			// nothing to do just complete the transaction
			if (VerboseTLM())
			{
				logger << DebugInfo << "Received transaction on toi_irq in nb_transport_bw, "
					<< "completing the transaction" << endl
					<< ITRANS(trans) << endl
					<< PHASE(phase) << endl
					<< TIME(t) << EndDebug;
			}
			trans.release();
			break;
		case tlm::BEGIN_REQ:
		case tlm::BEGIN_RESP:
		case tlm::END_RESP:
			logger << DebugWarning << "Received unexpected transaction phase on toi_irq" << endl
				<< LOCATION << endl
				<< ITRANS(trans) << endl
				<< PHASE(phase) << endl
				<< TIME(t)
				<< EndDebugWarning;
			TrapOnWarning();
			break;
	}
	return tlm::TLM_COMPLETED;
}

template<unsigned int BUSWIDTH>
void
TIM<BUSWIDTH> ::
TOIInvalidateDirectMemPtr(sc_dt::uint64 start_range, sc_dt::uint64 end_range)
{
	/* not supported */
}

/* END: output timer overflow interrupt (TOI) socket methods */

/* START: output input capture A interrupt (ICIA) socket methods */
template<unsigned int BUSWIDTH>
tlm::tlm_sync_enum
TIM<BUSWIDTH> ::
ICIANbTransportBw(TLMInterruptPayload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t)
{
	switch (phase)
	{
		case tlm::END_REQ:
			// nothing to do just complete the transaction
			if (VerboseTLM())
			{
				logger << DebugInfo << "Received transaction on icia_irq in nb_transport_bw, "
					<< "completing the transaction" << endl
					<< ITRANS(trans) << endl
					<< PHASE(phase) << endl
					<< TIME(t) << EndDebug;
			}
			break;
		case tlm::BEGIN_REQ:
		case tlm::BEGIN_RESP:
		case tlm::END_RESP:
			logger << DebugWarning << "Received unexpected transaction phase on icia_irq" << endl
				<< LOCATION << endl
				<< ITRANS(trans) << endl
				<< PHASE(phase) << endl
				<< TIME(t)
				<< EndDebugWarning;
			TrapOnWarning();
			break;
	}
	return tlm::TLM_COMPLETED;
}

template<unsigned int BUSWIDTH>
void
TIM<BUSWIDTH> ::
ICIAInvalidateDirectMemPtr(sc_dt::uint64 start_range, sc_dt::uint64 end_range)
{
	/* not supported */
}

/* END: output input capture A interrupt (ICIA) socket methods */

/* START: output input capture B interrupt (ICIB) socket methods */
template<unsigned int BUSWIDTH>
tlm::tlm_sync_enum
TIM<BUSWIDTH> ::
ICIBNbTransportBw(TLMInterruptPayload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t)
{
	switch (phase)
	{
		case tlm::END_REQ:
			// nothing to do just complete the transaction
			if (VerboseTLM())
			{
				logger << DebugInfo << "Received transaction on icib_irq in nb_transport_bw, "
					<< "completing the transaction" << endl
					<< ITRANS(trans) << endl
					<< PHASE(phase) << endl
					<< TIME(t) << EndDebug;
			}
			break;
		case tlm::BEGIN_REQ:
		case tlm::BEGIN_RESP:
		case tlm::END_RESP:
			logger << DebugWarning << "Received unexpected transaction phase on icib_irq" << endl
				<< LOCATION << endl
				<< ITRANS(trans) << endl
				<< PHASE(phase) << endl
				<< TIME(t)
				<< EndDebugWarning;
			TrapOnWarning();
			break;
	}
	return tlm::TLM_COMPLETED;
}

template<unsigned int BUSWIDTH>
void
TIM<BUSWIDTH> ::
ICIBInvalidateDirectMemPtr(sc_dt::uint64 start_range, sc_dt::uint64 end_range)
{
	/* not supported */
}

/* END: output input capture B interrupt (ICIB) socket methods */

/* START: output output compare A interrupt (OCIA) socket methods */
template<unsigned int BUSWIDTH>
tlm::tlm_sync_enum
TIM<BUSWIDTH> ::
OCIANbTransportBw(TLMInterruptPayload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t)
{
	switch (phase)
	{
		case tlm::END_REQ:
			// nothing to do just complete the transaction
			if (VerboseTLM())
			{
				logger << DebugInfo << "Received transaction on ocia_irq in nb_transport_bw, "
					<< "completing the transaction" << endl
					<< ITRANS(trans) << endl
					<< PHASE(phase) << endl
					<< TIME(t) << EndDebug;
			}
			break;
		case tlm::BEGIN_REQ:
		case tlm::BEGIN_RESP:
		case tlm::END_RESP:
			logger << DebugWarning << "Received unexpected transaction phase on ocia_irq" << endl
				<< LOCATION << endl
				<< ITRANS(trans) << endl
				<< PHASE(phase) << endl
				<< TIME(t)
				<< EndDebugWarning;
			TrapOnWarning();
			break;
	}
	return tlm::TLM_COMPLETED;
}

template<unsigned int BUSWIDTH>
void
TIM<BUSWIDTH> ::
OCIAInvalidateDirectMemPtr(sc_dt::uint64 start_range, sc_dt::uint64 end_range)
{
	/* not supported */
}

/* END: output output compare A interrupt (OCIA) socket methods */

/* START: output output compare B interrupt (OCIB) socket methods */
template<unsigned int BUSWIDTH>
tlm::tlm_sync_enum
TIM<BUSWIDTH> ::
OCIBNbTransportBw(TLMInterruptPayload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t)
{
	switch (phase)
	{
		case tlm::END_REQ:
			// nothing to do just complete the transaction
			if (VerboseTLM())
			{
				logger << DebugInfo << "Received transaction on ocib_irq in nb_transport_bw, "
					<< "completing the transaction" << endl
					<< ITRANS(trans) << endl
					<< PHASE(phase) << endl
					<< TIME(t) << EndDebug;
			}
			break;
		case tlm::BEGIN_REQ:
		case tlm::BEGIN_RESP:
		case tlm::END_RESP:
			logger << DebugWarning << "Received unexpected transaction phase on ocib_irq" << endl
				<< LOCATION << endl
				<< ITRANS(trans) << endl
				<< PHASE(phase) << endl
				<< TIME(t)
				<< EndDebugWarning;
			TrapOnWarning();
			break;
	}
	return tlm::TLM_COMPLETED;
}

template<unsigned int BUSWIDTH>
void
TIM<BUSWIDTH> ::
OCIBInvalidateDirectMemPtr(sc_dt::uint64 start_range, sc_dt::uint64 end_range)
{
	/* not supported */
}

/* END: output output compare B interrupt (OCIB) socket methods */

/* START: output compare A (OCMPA) edge socket methods */

template<unsigned int BUSWIDTH>
tlm::tlm_sync_enum
TIM<BUSWIDTH> ::
OCMPANbTransportBw(TLMInterruptPayload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t)
{
	switch (phase)
	{
		case tlm::END_REQ:
			// nothing to do just complete the transaction
			if (VerboseTLM())
			{
				logger << DebugInfo << "Received transaction on ocmpa_edge in nb_transport_bw, "
					<< "completing the transaction" << endl
					<< ITRANS(trans) << endl
					<< PHASE(phase) << endl
					<< TIME(t) << EndDebug;
			}
			break;
		case tlm::BEGIN_REQ:
		case tlm::BEGIN_RESP:
		case tlm::END_RESP:
			logger << DebugWarning << "Received unexpected transaction phase on ocmpa_edge" << endl
				<< LOCATION << endl
				<< ITRANS(trans) << endl
				<< PHASE(phase) << endl
				<< TIME(t)
				<< EndDebugWarning;
			TrapOnWarning();
			break;
	}
	return tlm::TLM_COMPLETED;
}

template<unsigned int BUSWIDTH>
void
TIM<BUSWIDTH> ::
OCMPAInvalidateDirectMemPtr(sc_dt::uint64 start_range, sc_dt::uint64 end_range)
{
	/* not supported */
}

/* END: output compare A (OCMPA) edge socket methods */

/* START: output compare A (OCMPB) edge socket methods */

template<unsigned int BUSWIDTH>
tlm::tlm_sync_enum
TIM<BUSWIDTH> ::
OCMPBNbTransportBw(TLMInterruptPayload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t)
{
	switch (phase)
	{
		case tlm::END_REQ:
			// nothing to do just complete the transaction
			if (VerboseTLM())
			{
				logger << DebugInfo << "Received transaction on ocmpb_edge in nb_transport_bw, "
					<< "completing the transaction" << endl
					<< ITRANS(trans) << endl
					<< PHASE(phase) << endl
					<< TIME(t) << EndDebug;
			}
			break;
		case tlm::BEGIN_REQ:
		case tlm::BEGIN_RESP:
		case tlm::END_RESP:
			logger << DebugWarning << "Received unexpected transaction phase on ocmpb_edge" << endl
				<< LOCATION << endl
				<< ITRANS(trans) << endl
				<< PHASE(phase) << endl
				<< TIME(t)
				<< EndDebugWarning;
			TrapOnWarning();
			break;
	}
	return tlm::TLM_COMPLETED;
}

template<unsigned int BUSWIDTH>
void
TIM<BUSWIDTH> ::
OCMPBInvalidateDirectMemPtr(sc_dt::uint64 start_range, sc_dt::uint64 end_range)
{
	/* not supported */
}

/* END: output compare A (OCMPB) edge socket methods */

/* START: input capture A (ICAPA) edge socket methods */

template<unsigned int BUSWIDTH>
tlm::tlm_sync_enum
TIM<BUSWIDTH> ::
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
			logger << DebugError << "Received unexpected phase on icapa_edge nb_transport_fw handler" << endl
				<< LOCATION << endl
				<< ITRANS(trans) << endl
				<< PHASE(phase) << endl
				<< TIME(t) << EndDebugError;
			Object::Stop(-1);
			break;
	}
}

template<unsigned int BUSWIDTH>
void
TIM<BUSWIDTH> ::
ICAPABTransport(TLMInterruptPayload& trans, sc_core::sc_time& t)
{
	wait(t);
	ProcessICAPA(trans.level);
	return;
}

template<unsigned int BUSWIDTH>
bool
TIM<BUSWIDTH> ::
ICAPAGetDirectMemPtr(TLMInterruptPayload& trans, tlm::tlm_dmi& dmi_data)
{
	/* not supported */
	return false;
}

template<unsigned int BUSWIDTH>
unsigned int
TIM<BUSWIDTH> ::
ICAPATransportDbg(TLMInterruptPayload& trans)
{
	/* no sense for interrupt */
	return 0;
}

/* END: input capture A (ICAPA) edge socket methods */

/* START: input capture B (ICAPB) edge socket methods */

template<unsigned int BUSWIDTH>
tlm::tlm_sync_enum
TIM<BUSWIDTH> ::
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
			logger << DebugError << "Received unexpected phase on icapb_edge nb_transport_fw handler" << endl
				<< LOCATION << endl
				<< ITRANS(trans) << endl
				<< PHASE(phase) << endl
				<< TIME(t) << EndDebugError;
			Object::Stop(-1);
			break;
	}
}

template<unsigned int BUSWIDTH>
void
TIM<BUSWIDTH> ::
ICAPBBTransport(TLMInterruptPayload& trans, sc_core::sc_time& t)
{
	wait(t);
	ProcessICAPB(trans.level);
	return;
}

template<unsigned int BUSWIDTH>
bool
TIM<BUSWIDTH> ::
ICAPBGetDirectMemPtr(TLMInterruptPayload& trans, tlm::tlm_dmi& dmi_data)
{
	/* not supported */
	return false;
}

template<unsigned int BUSWIDTH>
unsigned int
TIM<BUSWIDTH> ::
ICAPBTransportDbg(TLMInterruptPayload& trans)
{
	/* not supported */
	return 0;
}

/* END: input capture B (ICAPB) edge socket methods */
	
/* START: input memory socket methods */

template<unsigned int BUSWIDTH>
tlm::tlm_sync_enum
TIM<BUSWIDTH> ::
nb_transport_fw(tlm::tlm_generic_payload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t)
{
	if (VerboseTLM())
	{
		logger << DebugInfo
			<< "Received memory transaction:" << endl;
		TRANS(logger, trans);
		logger << endl
			<< PHASE(phase) << endl
			<< TIME(t)
			<< EndDebugInfo;
	}
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
			Object::Stop(-1);
			break;
	}
}

template<unsigned int BUSWIDTH>
void
TIM<BUSWIDTH> ::
b_transport(tlm::tlm_generic_payload& trans, sc_core::sc_time& t)
{
	if (VerboseTLM())
	{
		logger << DebugInfo
			<< "Received blocking memory transaction:" << endl;
		TRANS(logger, trans);
		logger << endl
			<< TIME(t)
			<< EndDebugInfo;
	}
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

template<unsigned int BUSWIDTH>
bool
TIM<BUSWIDTH> ::
get_direct_mem_ptr(tlm::tlm_generic_payload& trans, tlm::tlm_dmi& dmi_data)
{
	/* not supported */
	return false;
}

template<unsigned int BUSWIDTH>
unsigned int
TIM<BUSWIDTH> ::
transport_dbg(tlm::tlm_generic_payload& trans)
{
	return 0;
}

/* END: input memory socket methods */

/* Memory requests fifo handler */
template<unsigned int BUSWIDTH>
void
TIM<BUSWIDTH> ::
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
							Object::Stop(-1);
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
template<unsigned int BUSWIDTH>
void
TIM<BUSWIDTH> ::
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
template<unsigned int BUSWIDTH>
void
TIM<BUSWIDTH> ::
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

template<unsigned int BUSWIDTH>
tlm::tlm_response_status
TIM<BUSWIDTH> ::
ReadRegister(uint64_t addr, uint16_t &value)
{
	uint64_t reg_index = addr - base_address;
	bool done = false;
	char const * name;
	switch (reg_index)
	{
		case 0x0: // TIMn_ICAR
			done = true;
			value = ICAR();
			name = "ICAR";
			break;
		case 0x4: // TIMn_ICBR
			done = true;
			value = ICBR();
			name = "ICBR";
			break;
		case 0x8: // TIMn_OCAR
			done = true;
			value = OCAR();
			name = "OCAR";
			break;
		case 0xc: // TIMn_OCBR
			done = true;
			value = OCBR();
			name = "OCBR";
			break;
		case 0x10: // TIMn_CNTR
			{
				/* update the value of CNTR before returning it */
				sc_core::sc_time delta_time = sc_time_stamp() - cntr_update_time;
				uint64_t cntr_update;
				if (ECKEN())
					/* the external clock is used */
					cntr_update = floor(delta_time / extclk_cycle_time);
				else
					/* the internal clock is used */
					cntr_update = floor(delta_time / (pclk2_cycle_time * (CC() + 1)));
				cntr_update += CNTR();
				SetCNTR((uint16_t)(cntr_update % (uint64_t)0x10000));
				cntr_update_time = sc_time_stamp();
				/* get the CNTR value */
				done = true;
				value = CNTR();
				name = "CNTR";
			}
			break;
		case 0x14: // TIMn_CR1
			done = true;
			value = CR1();
			name = "CR1";
			break;
		case 0x18: // TIMn_CR2
			done = true;
			value = CR2();
			name = "CR2";
			break;
		case 0x1c: // TIMn_SR
			done = true;
			value = SR();
			name = "SR";
			break;
		default:
			logger << DebugWarning 
				<< "Trying to read from an unknown address (0x" << hex << addr << dec << ")"
				<< EndDebugWarning;
			TrapOnWarning();
			break;
	}
	if (!done)
	{
		logger << DebugWarning
			<< "TODO: read not handled for address 0x" << hex << addr << dec
			<< " (register index = 0x" << hex << reg_index << dec << ")" << endl
			<< LOCATION
			<< EndDebugWarning;
		TrapOnWarning();
		return tlm::TLM_ADDRESS_ERROR_RESPONSE;
	}
	if (VerboseRun())
		logger << DebugInfo
			<< "Reading register " << name << " with value 0x" << hex << value << dec
			<< EndDebugInfo;
	return tlm::TLM_OK_RESPONSE;
}

template<unsigned int BUSWIDTH>
tlm::tlm_response_status 
TIM<BUSWIDTH> ::
WriteRegister(uint64_t addr, uint16_t value)
{
	uint64_t reg_index = addr - base_address;
	bool done = false;
	switch (reg_index)
	{
		case 0x0: // TIMn_ICAR
			done = true;
			logger << DebugWarning
				<< "Trying to write ICAR (read only register) with 0x" << hex << value << dec
				<< EndDebugWarning;
			break;
		case 0x4: // TIMn_ICBR
			done = true;
			logger << DebugWarning
				<< "Trying to write ICBR (read only register) with 0x" << hex << value << dec
				<< EndDebugWarning;
			break;
		case 0x8: // TIMn_OCAR
			done = true;
			ocar = value;
			if (VerboseRun())
			{
				logger << DebugInfo
					<< "Setting OCAR to 0x" << hex << OCAR() << dec
					<< EndDebugInfo;
			}
			if (EN())
			{
				/* update CNTR */
				sc_core::sc_time delta_time = sc_time_stamp() - cntr_update_time;
				uint64_t cntr_update;
				if (ECKEN())
					// the external clock is used
					cntr_update = floor(delta_time / extclk_cycle_time);
				else
					// the internal clock is used
					cntr_update = floor(delta_time / (pclk2_cycle_time * (CC() + 1)));
				cntr_update += CNTR();
				SetCNTR((uint16_t)(cntr_update % (uint64_t)0x10000));
				cntr_update_time = sc_time_stamp();
				// notify when the counter will reach the OCAR value
				uint32_t rem;
				if (OCAR() < CNTR()) rem = ((uint32_t)0x10000 + (uint32_t)OCAR()) - CNTR();
				else rem = OCAR() - CNTR();
				sc_core::sc_time t;
				if (ECKEN())
					t = extclk_cycle_time * rem;
				else
					t = pclk2_cycle_time * (CC() + 1) * rem;
				if (VerboseRun())
					logger << DebugInfo
						<< "Notifying output compare A in " << t
						<< EndDebugInfo;
				oca_event.notify(t);
			}
			break;
		case 0xc: // TIMn_OCBR
			done = true;
			ocbr = value;
			if (VerboseRun())
			{
				logger << DebugInfo
					<< "Setting OCBR to 0x" << hex << OCBR() << dec
					<< EndDebugInfo;
			}
			if (EN())
			{
				/* update CNTR */
				sc_core::sc_time delta_time = sc_time_stamp() - cntr_update_time;
				uint64_t cntr_update;
				if (ECKEN())
					// the external clock is used
					cntr_update = floor(delta_time / extclk_cycle_time);
				else
					// the internal clock is used
					cntr_update = floor(delta_time / (pclk2_cycle_time * (CC() + 1)));
				cntr_update += CNTR();
				SetCNTR((uint16_t)(cntr_update % (uint64_t)0x10000));
				cntr_update_time = sc_time_stamp();
				// notify when the counter will reach the OCBR value
				uint32_t rem;
				if (OCBR() < CNTR()) rem = ((uint32_t)0x10000 + (uint32_t)OCBR()) - CNTR();
				else rem = OCBR() - CNTR();
				sc_core::sc_time t;
				if (ECKEN())
					t = extclk_cycle_time * rem;
				else
					t = pclk2_cycle_time * (CC() + 1) * rem;
				if (VerboseRun())
					logger << DebugInfo
						<< "Notifying output compare B in " << t
						<< EndDebugInfo;
				ocb_event.notify(t);
			}
			break;
		case 0x10: // TIMn_CNTR
			done = true;
			SetCNTR(value);
			cntr_update_time = sc_time_stamp();
			// check if the timer has been activated or desactivated
			if (EN())
			{
				// compute when the next signal should be generated
				sc_core::sc_time tmp_cycle_time;
				if (ECKEN())
					// external clock
					tmp_cycle_time = extclk_cycle_time;
				else
					// internal clock
					tmp_cycle_time = pclk2_cycle_time * (CC() + 1);
				uint32_t rem = (uint32_t)0x10000 - CNTR();
				cntr_update_time = sc_time_stamp();
				// set a notification for when the timer gets to 0
				sc_core::sc_time t = tmp_cycle_time * rem;
				if (VerboseRun())
					logger << DebugInfo
						<< "Notifying timer count in " << t 
						<< EndDebugInfo;
				to_event.notify(t);
				// set a notification for when the timer gets to OCAR
				if (CNTR() > OCAR())
					rem = (uint32_t)0x10000 + OCAR() - CNTR();
				else
					rem = CNTR() - OCAR();
				t = tmp_cycle_time * rem;
				if (VerboseRun())
					logger << DebugInfo
						<< "Notifying output compare A in " << t
						<< EndDebugInfo;
				oca_event.notify(t);
				// set a notification for when the timer gets to OCAB
				if (CNTR() > OCBR())
					rem = (uint32_t)0x10000 + OCBR() - CNTR();
				else
					rem = CNTR() - OCBR();
				t = tmp_cycle_time * rem;
				if (VerboseRun())
					logger << DebugInfo
						<< "Notifying output compare B in " << t
						<< EndDebugInfo;
				ocb_event.notify(t);
			}
			break;
		case 0x14: // TIMn_CR1
			{
				done = true;
				bool old_folva = FOLVA();
				bool old_folvb = FOLVB();
				SetCR1(value);
				if (VerboseRun())
				{
					logger << DebugInfo
						<< "Setting CR1 to 0x" << hex << CR1() << dec << endl
						<< " - EN    " << EN() << endl
						<< " - PWMI  " << PWMI() << endl
						<< " - FOLVB " << FOLVB() << endl
						<< " - FOLVA " << FOLVA() << endl
						<< " - OLVLB " << OLVLB() << endl
						<< " - OLVLA " << OLVLA() << endl
						<< " - OCBE  " << OCBE() << endl
						<< " - OCAE  " << OCAE() << endl
						<< " - OPM   " << OPM() << endl
						<< " - PWM   " << PWM() << endl
						<< " - IEDGB " << IEDGB() << endl
						<< " - IEDGA " << IEDGA() << endl
						<< " - EXEDG " << EXEDG() << endl
						<< " - ECKEN " << ECKEN();
					logger << EndDebugInfo;
				}
				// check if the timer has been activated or desactivated
				if (EN())
				{
					// compute when the next signal should be generated
					sc_core::sc_time tmp_cycle_time;
					if (ECKEN())
						// external clock
						tmp_cycle_time = extclk_cycle_time;
					else
						// internal clock
						tmp_cycle_time = pclk2_cycle_time * (CC() + 1);
					uint32_t rem = (uint32_t)0x10000 - CNTR();
					cntr_update_time = sc_time_stamp();
					// set a notification for when the timer gets to 0
					sc_core::sc_time t = tmp_cycle_time * rem;
					if (VerboseRun())
						logger << DebugInfo
							<< "Notifying timer count in " << t 
							<< EndDebugInfo;
					to_event.notify(t);
					// set a notification for when the timer gets to OCAR
					if (CNTR() > OCAR())
						rem = (uint32_t)0x10000 + OCAR() - CNTR();
					else
						rem = CNTR() - OCAR();
					t = tmp_cycle_time * rem;
					if (VerboseRun())
						logger << DebugInfo
							<< "Notifying output compare A in " << t
							<< EndDebugInfo;
					oca_event.notify(t);
					// set a notification for when the timer gets to OCAB
					if (CNTR() > OCBR())
						rem = (uint32_t)0x10000 + OCBR() - CNTR();
					else
						rem = CNTR() - OCBR();
					t = tmp_cycle_time * rem;
					if (VerboseRun())
						logger << DebugInfo
							<< "Notifying output compare B in " << t
							<< EndDebugInfo;
					ocb_event.notify(t);
				}

				/* Force compare mode */
				if (FOLVA() && !old_folva)
					if (!PWM() && !OPM()) SendOCMPA(OLVLA());
				if (FOLVB() && !old_folvb)
					SendOCMPB(OLVLB());
			}
			break;
		case 0x18: // TIMn_CR2
			done = true;
			cr2 = value & ~(uint16_t)0x0700;
			if (VerboseRun())
			{
				logger << DebugInfo
					<< "Setting CR2 to 0x" << hex << CR2() << dec << endl
					<< " - ICAIE " << ICAIE() << endl
					<< " - OCAIE " << OCAIE() << endl
					<< " - TOIE  " << TOIE() << endl
					<< " - ICBIE " << ICBIE() << endl
					<< " - OCBIE " << OCBIE() << endl
					<< " - CC    " << CC() << " (0x" << hex << CC() << dec << ")";
				logger << EndDebugInfo;
			}
			SendInputCaptureAInterrupt();
			SendOutputCompareAInterrupt();
			SendTimerOverflowInterrupt();
			SendInputCaptureBInterrupt();
			SendOutputCompareBInterrupt();
			SendGlobalTimerInterrupt();
			break;
		case 0x1c: // TIMn_SR
			{
				uint32_t old_sr = SR();
				bool old_icfa = ICFA();
				bool old_ocfa = OCFA();
				bool old_tof = TOF();
				bool old_icfb = ICFB();
				bool old_ocfb = OCFB();
				sr = SR() & (value & ~(uint16_t)0x07ff);
				done = true;
				if (VerboseRun())
				{
					logger << DebugInfo
						<< "Resetting SR to 0x" << hex << SR() << " (from 0x" << old_sr << " with write value = 0x" << value << ")" << dec << endl
						<< " - ICFA " << ICFA() << endl
						<< " - OCFA " << OCFA() << endl
						<< " - TOF  " << TOF() << endl
						<< " - ICFB " << ICFB() << endl
						<< " - OCFB " << OCFB()
						<< EndDebugInfo;
				}
				// for the modified bits send the new interrupt signal
				bool global = false;
				if (old_icfa != ICFA()) { SendInputCaptureAInterrupt(); global = true; }
				if (old_ocfa != OCFA()) { SendOutputCompareAInterrupt(); global = true; }
				if (old_tof != TOF()) { SendTimerOverflowInterrupt(); global = true; }
				if (old_icfb != ICFB()) { SendInputCaptureBInterrupt(); global = true; }
				if (old_ocfb != OCFB()) { SendOutputCompareBInterrupt(); global = true; }
				if (global) SendGlobalTimerInterrupt();
			}
			break;
		default:
			logger << DebugWarning 
				<< "Trying to write into an unknown address (0x" << hex << addr << dec << ") with value"
				<< " 0x" << hex << value << dec
				<< EndDebugWarning;
			TrapOnWarning();
			break;
	}
	if (!done)
	{
		logger << DebugWarning
			<< "TODO: write not handled for address 0x" << hex << addr << dec
			<< " (register index = 0x" << hex << reg_index << dec << ")" << endl
			<< LOCATION
			<< EndDebugWarning;
		TrapOnWarning();
		return tlm::TLM_ADDRESS_ERROR_RESPONSE;
	}
	return tlm::TLM_OK_RESPONSE;
}

/* START: interrupt transaction generation methods */

template<unsigned int BUSWIDTH>
void 
TIM<BUSWIDTH> ::
SendInputCaptureAInterrupt()
{
	bool level = ICFA() && ICAIE();
	TLMInterruptPayload *trans = irqPayloadFabric.allocate();
	trans->level = level;
	tlm::tlm_phase phase = tlm::BEGIN_REQ;
	sc_core::sc_time t = SC_ZERO_TIME;
	switch (icia_irq->nb_transport_fw(*trans, phase, t))
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
}

template<unsigned int BUSWIDTH>
void 
TIM<BUSWIDTH> ::
SendOutputCompareAInterrupt()
{
	bool level = OCFA() && OCAIE();
	TLMInterruptPayload *trans = irqPayloadFabric.allocate();
	trans->level = level;
	tlm::tlm_phase phase = tlm::BEGIN_REQ;
	sc_core::sc_time t = SC_ZERO_TIME;
	switch (ocia_irq->nb_transport_fw(*trans, phase, t))
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
}

template<unsigned int BUSWIDTH>
void 
TIM<BUSWIDTH> ::
SendTimerOverflowInterrupt()
{
	bool level = TOF() && TOIE();
	TLMInterruptPayload *trans = irqPayloadFabric.allocate();
	trans->level = level;
	tlm::tlm_phase phase = tlm::BEGIN_REQ;
	sc_core::sc_time t = SC_ZERO_TIME;
	switch (toi_irq->nb_transport_fw(*trans, phase, t))
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
}

template<unsigned int BUSWIDTH>
void 
TIM<BUSWIDTH> ::
SendInputCaptureBInterrupt()
{
	bool level = ICFB() && ICBIE();
	TLMInterruptPayload *trans = irqPayloadFabric.allocate();
	trans->level = level;
	tlm::tlm_phase phase = tlm::BEGIN_REQ;
	sc_core::sc_time t = SC_ZERO_TIME;
	switch (icib_irq->nb_transport_fw(*trans, phase, t))
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
}

template<unsigned int BUSWIDTH>
void 
TIM<BUSWIDTH> ::
SendOutputCompareBInterrupt()
{
	bool level = OCFB() && OCBIE();
	TLMInterruptPayload *trans = irqPayloadFabric.allocate();
	trans->level = level;
	tlm::tlm_phase phase = tlm::BEGIN_REQ;
	sc_core::sc_time t = SC_ZERO_TIME;
	switch (ocib_irq->nb_transport_fw(*trans, phase, t))
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
}

template<unsigned int BUSWIDTH>
void 
TIM<BUSWIDTH> ::
SendGlobalTimerInterrupt()
{
	bool level = (TOF() && TOIE()) ||
		(ICFA() && ICAIE()) ||
		(ICFB() && ICBIE()) ||
		(OCFA() && OCAIE()) ||
		(OCFB() && OCBIE());
	TLMInterruptPayload *trans = irqPayloadFabric.allocate();
	trans->level = level;
	tlm::tlm_phase phase = tlm::BEGIN_REQ;
	sc_core::sc_time t = SC_ZERO_TIME;
	switch (timeri_irq->nb_transport_fw(*trans, phase, t))
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

}

/* END: interrupt transaction generation methods */

/* START: OCMP<A/B> transaction generation methods */

template<unsigned int BUSWIDTH>
void 
TIM<BUSWIDTH> ::
SendOCMPA(bool level)
{
	/* only change the signal if different than the previous output */
	if (level != ocmpa_prev) return;
	ocmpa_prev = level;

	TLMInterruptPayload *trans = irqPayloadFabric.allocate();
	trans->level = level;
	tlm::tlm_phase phase = tlm::BEGIN_REQ;
	sc_core::sc_time t = SC_ZERO_TIME;
	switch (ocmpa_edge->nb_transport_fw(*trans, phase, t))
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
}

template<unsigned int BUSWIDTH>
void 
TIM<BUSWIDTH> ::
SendOCMPB(bool level)
{
	/* only change the signal if different than the previous output */
	if (level != ocmpb_prev) return;
	ocmpb_prev = level;

	TLMInterruptPayload *trans = irqPayloadFabric.allocate();
	trans->level = level;
	tlm::tlm_phase phase = tlm::BEGIN_REQ;
	sc_core::sc_time t = SC_ZERO_TIME;
	switch (ocmpb_edge->nb_transport_fw(*trans, phase, t))
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
}

/* END: OCMP<A/B> transaction generation methods */

/* START: event handlers */

template<unsigned int BUSWIDTH>
void 
TIM<BUSWIDTH> ::
TimerOverflowHandler()
{
	while(1)
	{
		wait(to_event);
		// check if timer count is enabled, otherwise it is just an spurious event that can be ignored
		if (!EN()) continue;
		// set the counter to 0
		SetCNTR(0);
		cntr_update_time = sc_time_stamp();
		// set the TOF bit if not already set, if it was not already send an interrupt may need to be generated
		if (!TOF())
		{
			sr = sr | (uint16_t)0x2000;
			if (VerboseRun()) 
			{
				logger << DebugInfo << "Detected timer overflow at " << sc_time_stamp() << ", setting TOF";
				if (TOIE())
					logger << " and sending interrupt";
				logger << EndDebugInfo;
			}
			// check if an interrupt should be generated
			if (TOIE())
			{
				SendTimerOverflowInterrupt();
				SendGlobalTimerInterrupt();
			}
		}
		// notify the timer overflow event for the next overflow
		uint32_t rem = (uint32_t)0x10000;
		// check which clock (internal or external) is being used
		sc_core::sc_time t;
		if (ECKEN())
			// external clock
			t = extclk_cycle_time * rem;
		else
			// internal clock
			t = pclk2_cycle_time * (uint32_t)(CC() + 1) * rem;
		if (VerboseRun())
			logger << DebugInfo
				<< "Notifying timer overflow in " << t
				<< EndDebugInfo;
		to_event.notify(t);
	}
}

template<unsigned int BUSWIDTH>
void
TIM<BUSWIDTH> ::
OutputCompareAHandler()
{
	while(1)
	{
		wait(oca_event);
		// check if timer count is enabled, otherwise it is just a spurious event that can be ignored
		if (!EN()) continue;
		// check that we are using the timer clock and not the external, otherwise the event is spurious and can be ignored
		if (ECKEN()) continue;
		// update the counter to OCAR
		SetCNTR(OCAR());
		cntr_update_time = sc_time_stamp();
		// set the OCFA bit if not already set, if it was not already send an interrupt may need to be generated
		// note that OCFA can not be set if PWM is active
		if (!OCFA() && !OPM() && !PWM())
		{
			sr = sr | (uint16_t)0x4000;
			if (VerboseRun())
			{
				logger << DebugInfo << "Detected output compare A at " << sc_time_stamp() << ", setting OCFA";
				if (OCAIE())
					logger << " and sending interrupt";
				logger << EndDebugInfo;
			}
			// check if an interrupt should be generated
			if (OCAIE())
			{
				SendOutputCompareAInterrupt();
				SendGlobalTimerInterrupt();
			}
		}
		// check if the OCMP<A/B> pin needs to be set
		if (OCAE())
			SendOCMPA(OLVLA());
		// notify the output compare for the next count
		uint32_t rem = (uint32_t)0x10000;
		sc_core::sc_time t;
		if (ECKEN())
			// external clock
			t = extclk_cycle_time * rem;
		else
			// internal clock
			t = pclk2_cycle_time * (uint32_t)(CC() + 1) * rem;
		oca_event.notify(t);
	}
}

template<unsigned int BUSWIDTH>
void
TIM<BUSWIDTH> ::
OutputCompareBHandler()
{
	while(1)
	{
		wait(ocb_event);
		// check if timer count is enabled, otherwise it is just a spurious event that can be ignored
		if (!EN()) continue;
		// update the counter to OCBR
		SetCNTR(OCBR());
		cntr_update_time = sc_time_stamp();
		// set the OCFB bit if not already set, if it was not already send an interrupt may need to be generated
		if (!OCFB())
		{
			sr = sr | (uint16_t)0x0800;
			if (VerboseRun())
			{
				logger << DebugInfo << "Detected output compare B at " << sc_time_stamp() << ", setting OCFB";
				if (OCBIE())
					logger << " and sending interrupt";
				logger << EndDebugInfo;
			}
			// check if an interrupt should be generated
			if (OCBIE())
			{
				SendOutputCompareBInterrupt();
				SendGlobalTimerInterrupt();
			}
		}
		// check if the OCMP<A/B> pin needs to be set
		if (OCBE())
		{
			if (VerboseRun())
				logger << DebugInfo << "Setting OCMPB to OLVLB (" << OLVLB() << ") at " << sc_time_stamp()
					<< EndDebugInfo;
			SendOCMPB(OLVLB());
		}
		// check if we are in PWM mode
		if ((PWM() && !OPM()) || (PWM() && OPM() && !FOLVA()))
		{
			SendOCMPA(OLVLB());
			// Reset counter to 0xfffc
			SetCNTR((uint16_t)0xfffc);
			cntr_update_time = sc_time_stamp();
			// set a notification for when the timer gets to 0
			sc_core::sc_time tmp_cycle_time;
			sc_core::sc_time t;
			if (ECKEN())
				// external clock
				tmp_cycle_time = extclk_cycle_time;
			else
				// internal clock
				tmp_cycle_time = pclk2_cycle_time * (CC() + 1);
			uint32_t rem = (uint32_t)0x10000 - CNTR();
			t = tmp_cycle_time * rem;
			if (VerboseRun())
				logger << DebugInfo
					<< "Notifying timer count in " << t
					<< EndDebugInfo;
			to_event.notify(t);
			// set a notification for when the timer gets to OCAR
			if (CNTR() > OCAR())
				rem = (uint32_t)0x10000 + OCAR() - CNTR();
			else
				rem = CNTR() - OCAR();
			t = tmp_cycle_time * rem;
			if (VerboseRun())
				logger << DebugInfo
					<< "Notifying output compare A in " << t
					<< EndDebugInfo;
			oca_event.notify(t);
			// set a notification for when the timer gets to OCAB
			if (CNTR() > OCBR())
				rem = (uint32_t)0x10000 + OCBR() - CNTR();
			else
				rem = CNTR() - OCBR();
			t = tmp_cycle_time * rem;
			if (VerboseRun())
				logger << DebugInfo
					<< "Notifying output compare B in " << t
					<< EndDebugInfo;
			ocb_event.notify(t);
		}
		else
		{
			// notify the output compare for the next count
			uint32_t rem = (uint32_t)0x10000;
			sc_core::sc_time t;
			if (ECKEN())
				// external clock
				t = extclk_cycle_time * rem;
			else
				// internal clock
				t = pclk2_cycle_time * (uint32_t)(CC() + 1) * rem;
			if (VerboseRun())
				logger << DebugInfo
					<< "Notifying output compare B in " << t
					<< EndDebugInfo;
			ocb_event.notify(t);
		}
	}
}


/* END: event handler */

template<unsigned int BUSWIDTH>
void
TIM<BUSWIDTH> ::
ProcessICAPA(bool level)
{
	/* if the current value of the icapa signal and the new is the same, nothing is needed */
	if (level == icapa_edge_prev) return;

	/* the level has changed, set the new current icapa signal value */
	icapa_edge_prev = level;

	/* copy the current counter value into the IC<A/B>R register, CNTR must be updated before */
	if (EN())
	{
		/* handle the one pulse mode independently of the level */
		if ((OPM() && !PWM() && !PWMI()) ||
				(OPM() && FOLVA()))
		{
			// reset the counter to the 0xfffc value
			cntr_update_time = sc_time_stamp();
			SetCNTR((uint16_t)0xfffc);
			// set a notification for when the timer gets to 0
			sc_core::sc_time tmp_cycle_time;
			sc_core::sc_time t;
			if (ECKEN())
				// external clock
				tmp_cycle_time = extclk_cycle_time;
			else
				// internal clock
				tmp_cycle_time = pclk2_cycle_time * (CC() + 1);
			uint32_t rem = (uint32_t)0x10000 - CNTR();
			t = tmp_cycle_time * rem;
			if (VerboseRun())
				logger << DebugInfo
					<< "Notifying timer count in " << t
					<< EndDebugInfo;
			to_event.notify(t);
			// set a notification for when the timer gets to OCAR
			if (CNTR() > OCAR())
				rem = (uint32_t)0x10000 + OCAR() - CNTR();
			else
				rem = CNTR() - OCAR();
			t = tmp_cycle_time * rem;
			if (VerboseRun())
				logger << DebugInfo
					<< "Notifying output compare A in " << t
					<< EndDebugInfo;
			oca_event.notify(t);
			// set a notification for when the timer gets to OCBR
			if (CNTR() > OCBR())
				rem = (uint32_t)0x10000 + OCBR() - CNTR();
			else
				rem = CNTR() - OCBR();
			t = tmp_cycle_time * rem;
			if (VerboseRun())
				logger << DebugInfo
					<< "Notifying output compare B in " << t
					<< EndDebugInfo;
			ocb_event.notify(t);
			// copy OLVLB to OCMPA
			SendOCMPA(OLVLB());
		}

		/* only perform a capture if the active edge is detected */
		if (level != IEDGA()) return;

		sc_core::sc_time delta_time = sc_time_stamp() - cntr_update_time;
		uint64_t cntr_update;
		if (ECKEN())
			/* the external clock is used */
			cntr_update = floor(delta_time / extclk_cycle_time);
		else
			/* the internal clock is used */
			cntr_update = floor(delta_time / (pclk2_cycle_time * (CC() + 1)));
		cntr_update += CNTR();
		SetCNTR((uint16_t)(cntr_update % (uint64_t)0x10000));
		cntr_update_time = sc_time_stamp();
		SetICAR(CNTR());
	
		/* handle pulse width modulation input */
		if ((PWMI() && !OPM()) || (PWMI() && !FOLVA()))
		{
			// set CNTR to 0
			cntr_update_time = sc_time_stamp();
			SetCNTR(0);
			sc_core::sc_time tmp_cycle_time;
			sc_core::sc_time t;
			if (ECKEN())
				// external clock
				tmp_cycle_time = extclk_cycle_time;
			else
				// internal clock
				tmp_cycle_time = pclk2_cycle_time * (CC() + 1);
			// set a notification for when the timer gets to OCAR
			uint32_t rem;
			if (CNTR() > OCAR())
				rem = (uint32_t)0x10000 + OCAR() - CNTR();
			else
				rem = CNTR() - OCAR();
			t = tmp_cycle_time * rem;
			if (VerboseRun())
				logger << DebugInfo
					<< "Notifying output compare A in " << t
					<< EndDebugInfo;
			oca_event.notify(t);
			// set a notification for when the timer gets to OCBR
			if (CNTR() > OCBR())
				rem = (uint32_t)0x10000 + OCBR() - CNTR();
			else
				rem = CNTR() - OCBR();
			t = tmp_cycle_time * rem;
			if (VerboseRun())
				logger << DebugInfo
					<< "Notifying output compare B in " << t
					<< EndDebugInfo;
			ocb_event.notify(t);
		}
	}

	/* generate an interrupt (set the ICF<A/B> bit of the SR register */
	if (ICFA()) return;
	SetICFA(true);
	/* Call the send interrupt methods (that will only send an interrupt if the interrupts are enabled */
	SendInputCaptureAInterrupt();
	SendGlobalTimerInterrupt();

}

template<unsigned int BUSWIDTH>
void
TIM<BUSWIDTH> ::
ProcessICAPB(bool level)
{
	/* if the current value of the icapb signal and the new is the same, nothing is needed */
	if (level == icapb_edge_prev) return;

	/* the level has changed, set the new current icapb signal value */
	icapb_edge_prev = level;

	if (EN())
	{
		/* only perform a capture if the active edge is detected */
		if (level != IEDGB()) return;
		/* copy the current counter value into the IC<A/B>R register, CNTR must be updated before */
		sc_core::sc_time delta_time = sc_time_stamp() - cntr_update_time;
		uint64_t cntr_update;
		if (ECKEN())
			/* the external clock is used */
			cntr_update = floor(delta_time / extclk_cycle_time);
		else
			/* the internal clock is used */
			cntr_update = floor(delta_time / (pclk2_cycle_time * (CC() + 1)));
		cntr_update += CNTR();
		SetCNTR((uint16_t)(cntr_update % (uint64_t)0x10000));
		cntr_update_time = sc_time_stamp();
		SetICBR(CNTR());
	}

	/* generate an interrupt (set the ICF<A/B> bit of the SR register, if not already send */
	if (ICFB()) return;
	SetICFB(true);
	/* Call the send interrupt methods (that will only send an interrupt if the interrupts are enabled */
	SendInputCaptureBInterrupt();
	SendGlobalTimerInterrupt();
}

/* START: verbose methods */

template<unsigned int BUSWIDTH>
bool 
TIM<BUSWIDTH> ::
VerboseAll()
{
	return verbose_all;
}

template<unsigned int BUSWIDTH>
bool 
TIM<BUSWIDTH> ::
VerboseSetup()
{
	return verbose_setup;
}

template<unsigned int BUSWIDTH>
bool 
TIM<BUSWIDTH> ::
VerboseRun()
{
	if(unlikely(verbose_run))
	{
		if (trap_on_verbose_run && trap_reporting_import)
			trap_reporting_import->ReportTrap();
		return true;
	}
	return false;
}

template<unsigned int BUSWIDTH>
bool 
TIM<BUSWIDTH> ::
VerboseTLM()
{
	if (verbose_tlm)
	{
		if (trap_on_verbose_tlm && trap_reporting_import)
			trap_reporting_import->ReportTrap();
		return true;
	}
	return false;
}

template<unsigned int BUSWIDTH>
void
TIM<BUSWIDTH> ::
TrapOnWarning()
{
	if (trap_on_warning && trap_reporting_import)
		trap_reporting_import->ReportTrap();
}

/* END: verbose methods */

} // end of namespace str7_timer
} // end of namespace timer
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_TIMER_STR7_TIMER_HH__

