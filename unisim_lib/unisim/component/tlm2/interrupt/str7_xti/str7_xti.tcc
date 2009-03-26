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

#ifndef __UNISIM_COMPONENT_TLM2_INTERRUPT_STR7_XTI_XTI_TCC__
#define __UNISIM_COMPONENT_TLM2_INTERRUPT_STR7_XTI_XTI_TCC__

#define LOCATION 	"On location " << __FUNCTION__ << ":unisim/component/tlm2/interrupt/str7_xti/str7_xti.tcc:" << __LINE__ << endl
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

#include <string>

namespace unisim {
namespace component {
namespace tlm2 {
namespace interrupt {
namespace str7_xti {

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebug;
using std::endl;

template <unsigned int BUS_WIDTH, bool DEBUG>
STR7_XTI<BUS_WIDTH, DEBUG> ::
STR7_XTI(const sc_module_name& name, Object* parent) :
		Object(name, parent),
		sc_module(name),
		in_irq("in_irq"),
		out_irq("out_irq"),
		slave_socket("slave_socket"),
		sr(0),
		ctrl(0),
		mr(0),
		tr(0),
		pr(0),
		base_address(0),
		param_base_address("base-address", this, base_address),
		logger(*this),
		verbose_all(false),
		param_verbose_all("verbose-all", this, verbose_all),
		verbose_setup(false),
		param_verbose_setup("verbose-setup", this, verbose_setup),
		verbose_run(false),
		param_verbose_run("verbose-run", this, verbose_run)
{
	slave_socket.bind(*this);
	out_irq.bind(*this);
	in_irq.register_nb_transport_fw(this, &STR7_XTI<BUS_WIDTH, DEBUG>::in_irq_nb);
	in_irq.register_b_transport(this, &STR7_XTI<BUS_WIDTH, DEBUG>::in_irq_b); 
    in_irq.register_transport_dbg(this, &STR7_XTI<BUS_WIDTH, DEBUG>::in_irq_dbg);
	in_irq.register_get_direct_mem_ptr(this, &STR7_XTI<BUS_WIDTH, DEBUG>::in_irq_dmi);
}

template <unsigned int BUS_WIDTH, bool DEBUG>
STR7_XTI<BUS_WIDTH, DEBUG> ::
~STR7_XTI()
{
}

template <unsigned int BUS_WIDTH, bool DEBUG>
bool
STR7_XTI<BUS_WIDTH, DEBUG> ::
Setup()
{
	if (VerboseAll())
	{
		verbose_setup = true;
		verbose_run = true;
	}

	if (base_address & 0x00FF)
	{
		logger << DebugError << LOCATION
			<< "\"base_address\" must be aligned to 0xFF"
			<< EndDebug;
		return false;
	}

	if (VerboseSetup())
		logger << DebugInfo << Object::GetName() << " setup:" << endl
			<< "- setting base address to 0x" << hex << base_address << dec
			<< EndDebug;

	return true;
}

/* START: methods implementing the "target" socket */

template <unsigned int BUS_WIDTH, bool DEBUG>
void 
STR7_XTI<BUS_WIDTH, DEBUG> ::
b_transport(tlm::tlm_generic_payload& trans, sc_core::sc_time& t)
{
	if (trans.get_command() == tlm::TLM_IGNORE_COMMAND)
	{
		logger << DebugWarning << LOCATION << endl
			<< "Received b_transport on slave_socket with an ignore command, which the XTI doesn't know how to handle" << endl
			<< TIME(t) << endl;
		TRANS(logger, trans);
		logger << EndDebug;
		trans.set_response_status(tlm::TLM_COMMAND_ERROR_RESPONSE);
		return;
	}

	unsigned int size = trans.get_data_length();
	if (size != 1)
	{
		logger << DebugError << LOCATION << endl
			<< "Received b_transport on slave_socket with a size of " << size << " (size of 1 expected)" << endl
			<< TIME(t) << endl;
		TRANS(logger, trans);
		logger << EndDebug;
		return;
	}

	uint64_t addr;
	addr = trans.get_address();
	addr = addr & ~base_address;

	unsigned char *data_ptr = trans.get_data_ptr();
	tlm::tlm_response_status status;
	if (trans.is_write())
		status = WriteRegister(addr, *data_ptr, t);
	else
		status = ReadRegister(addr, *data_ptr, t);
	
	trans.set_response_status(status);
}

template <unsigned int BUS_WIDTH, bool DEBUG>
tlm::tlm_sync_enum 
STR7_XTI<BUS_WIDTH, DEBUG> ::
nb_transport_fw(tlm::tlm_generic_payload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t)
{
	if (trans.get_command() == tlm::TLM_IGNORE_COMMAND)
	{
		logger << DebugWarning << LOCATION << endl
			<< "Received nb_transport_fw on slave_socket with an ignore command, which the XTI doesn't know how to handle" << endl
			<< TIME(t) << endl
			<< PHASE(phase) << endl;
		TRANS(logger, trans);
		logger << EndDebug;
		trans.set_response_status(tlm::TLM_COMMAND_ERROR_RESPONSE);
		phase = tlm::BEGIN_RESP;
		return tlm::TLM_COMPLETED;
	}

	switch (phase)
	{
		case tlm::END_REQ:
		case tlm::BEGIN_RESP:
		case tlm::END_RESP:
			logger << DebugError << LOCATION << endl
				<< "Received nb_transport_fw on slave socket with an unhandled phase" << endl
				<< TIME(t) << endl
				<< PHASE(phase) << endl;
			TRANS(logger, trans);
			logger << EndDebug;
			trans.set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
			phase = tlm::BEGIN_RESP;
			return tlm::TLM_COMPLETED;
			break;
	}

	if (trans.get_command() == tlm::TLM_IGNORE_COMMAND)
	{
		logger << DebugWarning << LOCATION << endl
			<< "Received nb_transport_fw on slave_socket with an ignore command, which the XTI doesn't know how to handle" << endl
			<< TIME(t) << endl
			<< PHASE(phase) << endl;
		TRANS(logger, trans);
		logger << EndDebug;
		trans.set_response_status(tlm::TLM_COMMAND_ERROR_RESPONSE);
		phase = tlm::BEGIN_RESP;
		return tlm::TLM_COMPLETED;
	}

	unsigned int size = trans.get_data_length();
	if (size != 1)
	{
		logger << DebugError << LOCATION << endl
			<< "Received nb_transport_fw on slave_socket with a size of " << size << " (size of 1 expected)" << endl
			<< TIME(t) << endl
			<< PHASE(phase) << endl;
		TRANS(logger, trans);
		logger << EndDebug;
		trans.set_response_status(tlm::TLM_ADDRESS_ERROR_RESPONSE);
		phase = tlm::BEGIN_RESP;
		return tlm::TLM_COMPLETED;
	}

	uint64_t addr;
	addr = trans.get_address();
	addr = addr & ~base_address;

	unsigned char *data_ptr = trans.get_data_ptr();
	tlm::tlm_response_status status;
	if (trans.is_write())
	{
		phase = tlm::END_REQ;
		status = WriteRegister(addr, *data_ptr, t);
	}
	else
	{
		phase = tlm::BEGIN_RESP;
		status = ReadRegister(addr, *data_ptr, t);
	}

	trans.set_response_status(status);
	return tlm::TLM_COMPLETED;
}

template <unsigned int BUS_WIDTH, bool DEBUG>
bool 
STR7_XTI<BUS_WIDTH, DEBUG> ::
get_direct_mem_ptr(tlm::tlm_generic_payload& trans, tlm::tlm_dmi& dmi_data)
{
	/* not supported */
	return false;
}

template <unsigned int BUS_WIDTH, bool DEBUG>
unsigned int 
STR7_XTI<BUS_WIDTH, DEBUG> ::
transport_dbg(tlm::tlm_generic_payload& trans)
{
	/* TODO */
}

/* END: methods implementing the "target" socket */

/* START: methods implementing the "out_irq" socket */

template <unsigned int BUS_WIDTH, bool DEBUG>
tlm::tlm_sync_enum 
STR7_XTI<BUS_WIDTH, DEBUG> ::
tlm_bw_nonblocking_transport_bw_if(TLMInterruptPayload &trans, tlm::tlm_phase& phase, sc_core::sc_time& t)
{
	/* we should never be called through this interface */
	return tlm::TLM_COMPLETED;
}

template <unsigned int BUS_WIDTH, bool DEBUG>
void 
STR7_XTI<BUS_WIDTH, DEBUG> ::
invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range)
{
	/* ignored,
	 * direct memory interface not supported */
}

/* END: methods implementing the "out_irq" socket */

/* START: callbacks for the multi_passthrough_target_socket in_irq */

template <unsigned int BUS_WIDTH, bool DEBUG>
tlm::tlm_sync_enum
STR7_XTI<BUS_WIDTH, DEBUG> ::
in_irq_nb(int index, TLMInterruptPayload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t)
{
	/* TODO */
	switch (phase)
	{
		case tlm::BEGIN_REQ:
		case tlm::END_REQ:
		case tlm::BEGIN_RESP:
		case tlm::END_RESP:
			break;
	}
	phase = tlm::END_REQ;
	return tlm::TLM_COMPLETED;
}

template <unsigned int BUS_WIDTH, bool DEBUG>
void 
STR7_XTI<BUS_WIDTH, DEBUG> ::
in_irq_b(int index, TLMInterruptPayload& trans, sc_core::sc_time& t)
{
	/* TODO */
}

template <unsigned int BUS_WIDTH, bool DEBUG>
unsigned int 
STR7_XTI<BUS_WIDTH, DEBUG> ::
in_irq_dbg(int index, TLMInterruptPayload &trans)
{
	/* TODO */
}

template <unsigned int BUS_WIDTH, bool DEBUG>
bool 
STR7_XTI<BUS_WIDTH, DEBUG> ::
in_irq_dmi(int index, TLMInterruptPayload& trans, tlm::tlm_dmi& dmi)
{
	/* TODO */
}

/* END: callbacks for the multi_passthrough_target_socket in_irq */

template <unsigned int BUS_WIDTH, bool DEBUG>
tlm::tlm_response_status
STR7_XTI<BUS_WIDTH, DEBUG> ::
ReadRegister(uint64_t addr, uint8_t& data, sc_core::sc_time& t)
{
	const char *name = 0;

	switch (addr)
	{
		case SR_ADDR:
			data = SR();
			name = SR_NAME;
			break;
		case CTRL_ADDR:
			data = CTRL();
			name = CTRL_NAME;
			break;
		case MRH_ADDR:
			data = MRH();
			name = MRH_NAME;
			break;
		case MRL_ADDR:
			data = MRL();
			name = MRL_NAME;
			break;
		case TRH_ADDR:
			data = TRH();
			name = TRH_NAME;
			break;
		case TRL_ADDR:
			data = TRL();
			name = TRL_NAME;
			break;
		case PRH_ADDR:
			data = PRH();
			name = PRH_NAME;
			break;
		case PRL_ADDR:
			data = PRL();
			name = PRL_NAME;
			break;
		default:
			logger << DebugWarning
				<< "Trying to read from unknown register" << endl
				<< " - address = 0x" << hex << addr << dec
				<< EndDebug;
			return tlm::TLM_ADDRESS_ERROR_RESPONSE;
	}

	if (VerboseRun())
		logger << DebugInfo
			<< "Reading from register " << name << " with value = 0x" << hex << data << dec
			<< EndDebug;
	return tlm::TLM_OK_RESPONSE;
}

template <unsigned int BUS_WIDTH, bool DEBUG>
tlm::tlm_response_status
STR7_XTI<BUS_WIDTH, DEBUG> ::
WriteRegister(uint64_t addr, uint8_t& data, sc_core::sc_time& t)
{
	const char *name = 0;
	uint8_t written_data = data;

	switch (addr)
	{
		case SR_ADDR:
			written_data = SetSR(data);
			name = SR_NAME;
			break;
		case CTRL_ADDR:
			written_data = SetCTRL(data);
			name = CTRL_NAME;
			break;
		case MRH_ADDR:
			written_data = SetMRH(data);
			name = MRH_NAME;
			break;
		case MRL_ADDR:
			written_data = SetMRL(data);
			name = MRL_NAME;
			break;
		case TRH_ADDR:
			written_data = SetTRH(data);
			name = TRH_NAME;
			break;
		case TRL_ADDR:
			written_data = SetTRL(data);
			name = TRL_NAME;
			break;
		case PRH_ADDR:
			written_data = SetPRH(data);
			name = PRH_NAME;
			break;
		case PRL_ADDR:
			written_data = SetPRL(data);
			name = PRL_NAME;
			break;
		default:
			logger << DebugWarning
				<< "Trying to write from unknown register" << endl
				<< " - address = 0x" << hex << addr << dec
				<< " - data = 0x" << hex << data << dec
				<< EndDebug;
			return tlm::TLM_ADDRESS_ERROR_RESPONSE;
	}

	if (VerboseRun())
		logger << DebugInfo
			<< "Writing to register " << name << " with value = 0x" << hex << data << dec
			<< " (final value = 0x" << written_data << ")"
			<< EndDebug;
	return tlm::TLM_OK_RESPONSE;
}

template <unsigned int BUS_WIDTH, bool DEBUG>
bool
STR7_XTI<BUS_WIDTH, DEBUG> ::
VerboseAll()
{
	return DEBUG && verbose_all;
}

template <unsigned int BUS_WIDTH, bool DEBUG>
bool
STR7_XTI<BUS_WIDTH, DEBUG> ::
VerboseSetup()
{
	return DEBUG && verbose_setup;
}

template <unsigned int BUS_WIDTH, bool DEBUG>
bool
STR7_XTI<BUS_WIDTH, DEBUG> ::
VerboseRun()
{
	return DEBUG && verbose_run;
}

} // end of namespace str7_xti
} // end of namespace interrupt
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_INTERRUPT_STR7_XTI_XTI_TCC__
