/*
 *  Copyright (c) 2008,
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
 *          Gilles Mouchard (gilles.mouchard@cea.fr)
 */
 
#ifndef __UNISIM_COMPONENT_TLM2_INTERCONNECT_GENERIC_ROUTER_TCC__
#define __UNISIM_COMPONENT_TLM2_INTERCONNECT_GENERIC_ROUTER_TCC__

#include "unisim/component/tlm2/interconnect/generic_router/router_dispatcher.tcc"
#include <cmath>
#include <sstream>

#define LOCATION 	" - location = " << __FUNCTION__ << ":unisim_lib/unisim/component/tlm2/interconnect/generic_router/router.tcc:" << __LINE__
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
	RouterPayloadExtension *_extension = 0; \
	(X).get_extension(_extension); \
	if(_extension) { \
		(L) << endl; \
		(L) << "   - extension(" << _extension << "):" << endl; \
		if(_extension->Empty()) \
			(L) << "     - empty" << endl; \
		else \
			(L) << "     - input port = " << _extension->Front(); \
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
	RouterPayloadExtension *_extension = 0; \
	(X).get_extension(_extension); \
	if(_extension) { \
		(L) << endl; \
		(L) << "   - extension(" << _extension << "):" << endl; \
		if(_extension->Empty()) \
			(L) << "     - empty" << endl; \
		else \
			(L) << "     - input port = " << _extension->Front(); \
	} \
}

namespace unisim {
namespace component {
namespace tlm2 {
namespace interconnect {
namespace generic_router {

using namespace unisim::kernel::logger;

template <class CONFIG> const unsigned int unisim::component::tlm2::interconnect::generic_router::Router<CONFIG>::INPUT_SOCKETS;
template <class CONFIG> const unsigned int unisim::component::tlm2::interconnect::generic_router::Router<CONFIG>::OUTPUT_SOCKETS;

template<class CONFIG>
Router<CONFIG>::
Router(const sc_module_name &name, Object *parent) :
unisim::kernel::service::Object(name, parent, "A memory-mapped router"),
unisim::kernel::service::Service<unisim::service::interfaces::Memory<uint64_t> >(name, parent),
unisim::kernel::service::Client<unisim::service::interfaces::Memory<uint64_t> >(name, parent),
sc_module(name),
memory_export("memory-export", this),
m_req_dispatcher(),
m_rsp_dispatcher(),
cycle_time(SC_ZERO_TIME),
param_cycle_time("cycle_time", this, cycle_time, "Time to process a request/response by the router"),
port_buffer_size(0),
param_port_buffer_size("port_buffer_size", this, port_buffer_size, "Defines the size of the buffer for incomming requests in each of the input ports (0 = infinite)"),
logger(*this),
verbose_all(false),
param_verbose_all(0),
verbose_setup(false),
param_verbose_setup(0),
verbose_tlm(false),
param_verbose_tlm(0),
verbose_tlm_debug(false),
param_verbose_tlm_debug(0),
verbose_memory_interface(false),
param_verbose_memory_interface(0)
{
	param_port_buffer_size.SetFormat(unisim::kernel::service::VariableBase::FMT_DEC);
	
	if (VERBOSE)
	{
		param_verbose_all = new unisim::kernel::service::Parameter<bool>("verbose_all", this, verbose_all, "Activate all the verbose options");
		param_verbose_setup = new unisim::kernel::service::Parameter<bool>("verbose_setup", this, verbose_setup, "Display Object setup information");
		param_verbose_tlm = new unisim::kernel::service::Parameter<bool>("verbose_tlm", this, verbose_tlm, "Display TLM blocking/non-blocking transactions information");
		param_verbose_tlm_debug = new unisim::kernel::service::Parameter<bool>("verbose_tlm_debug", this, verbose_tlm_debug, "Display TLM debug transactions information");
		param_verbose_memory_interface = new unisim::kernel::service::Parameter<bool>("verbose_memory_interface", this, verbose_memory_interface, "Display memory interface operations information");
	}

	char const * const mapping_desc =
		"Defined a mapping of the router with format \"[range_start]\",\"[range_end]\",\"[outport_index]\" where [range_start], [range_end] and [outport_index] are to be replaced with the initial address, end address (= range_start + range_size - 1) and the output port index respectively";
	/* instantiate the mapping parameters */
	for(unsigned int i = 0; i < MAX_NUM_MAPPINGS; i++) 
	{
		std::stringstream buf;
		buf << "mapping_" << i;
		param_mapping[i] = new unisim::kernel::service::Parameter<Mapping>(buf.str().c_str(), this, mapping[i], mapping_desc);
	}
	
	/* create target sockets */
	for (unsigned int i = 0; i < INPUT_SOCKETS; i++)
	{
		std::stringstream targ_socket_name_sstr;
		targ_socket_name_sstr << "targ_socket_" << i;
		targ_socket[i] = new TargSocket(targ_socket_name_sstr.str().c_str());
	}

	/* register target sockets callbacks */
	for (unsigned int i = 0; i < INPUT_SOCKETS; i++)
	{
		(*targ_socket[i]).register_nb_transport_fw(	this, &Router<CONFIG>::T_nb_transport_fw_cb, i);
		(*targ_socket[i]).register_b_transport(		this, &Router<CONFIG>::T_b_transport_cb, i);
		(*targ_socket[i]).register_transport_dbg(		this, &Router<CONFIG>::T_transport_dbg_cb, i);
		(*targ_socket[i]).register_get_direct_mem_ptr(this, &Router<CONFIG>::T_get_direct_mem_ptr_cb, i);
	}
//	/* create target sockets and register socket callbacks */
//	for (unsigned int i = 0; i < MAX_INPUT_SOCKETS; i++)
//	{
//		std::stringstream buf;
//		buf << "targ_socket[" << i << "]";
//		targ_socket[i] = new tlm_utils::passthrough_target_socket_tagged<Router, BUSWIDTH, TYPES>(buf.str().c_str());
//		targ_socket[i]->register_nb_transport_fw(	this, &Router<CONFIG>::T_nb_transport_fw_cb, i);
//		targ_socket[i]->register_b_transport(		this, &Router<CONFIG>::T_b_transport_cb, i);
//		targ_socket[i]->register_transport_dbg(		this, &Router<CONFIG>::T_transport_dbg_cb, i);
//		targ_socket[i]->register_get_direct_mem_ptr(this, &Router<CONFIG>::T_get_direct_mem_ptr_cb, i);
//	}

	// TODO: remove ==>	/* register target multi socket callbacks */
 	// TODO: remove ==>	targ_socket.register_nb_transport_fw(    this, &Router<CONFIG>::T_nb_transport_fw_cb);
 	// TODO: remove ==>	targ_socket.register_b_transport(        this, &Router<CONFIG>::T_b_transport_cb);
 	// TODO: remove ==>	targ_socket.register_transport_dbg(      this, &Router<CONFIG>::T_transport_dbg_cb);
 	// TODO: remove ==>	targ_socket.register_get_direct_mem_ptr( this, &Router<CONFIG>::T_get_direct_mem_ptr_cb);

	/* create initiator sockets */
	for (unsigned int i = 0; i < OUTPUT_SOCKETS; i++)
	{
		std::stringstream init_socket_name_sstr;
		init_socket_name_sstr << "init_socket_" << i;
		init_socket[i] = new InitSocket(init_socket_name_sstr.str().c_str());
	}
	
	/* create initiator sockets and register initiator sockets callbacks */
	for (unsigned int i = 0; i < OUTPUT_SOCKETS; i++)
	{
		(*init_socket[i]).register_nb_transport_bw(			this, &Router<CONFIG>::I_nb_transport_bw_cb, i);
		(*init_socket[i]).register_invalidate_direct_mem_ptr(	this, &Router<CONFIG>::I_invalidate_direct_mem_ptr_cb, i);
	}

	/* create memory_imports */
	for (unsigned int i = 0; i < OUTPUT_SOCKETS; i++)
	{
		std::stringstream str;
		str << "memory-import[" << i << "]";
		memory_import[i] = new unisim::kernel::service::ServiceImport<unisim::service::interfaces::Memory<uint64_t> >(str.str().c_str(), this);
		memory_export.SetupDependsOn(*memory_import[i]);
	}
//	/* create initiator sockets and register socket callbacks */
//	for (unsigned int i = 0; i < MAX_OUTPUT_SOCKETS; i++)
//	{
//		std::stringstream buf;
//		buf << "init_socket[" << i << "]";
//		init_socket[i] = new tlm_utils::simple_initiator_socket_tagged<Router, BUSWIDTH, TYPES>(buf.str().c_str());
//		init_socket[i]->register_nb_transport_bw(			this, &Router<CONFIG>::I_nb_transport_bw_cb, i);
//		init_socket[i]->register_invalidate_direct_mem_ptr(	this, &Router<CONFIG>::I_invalidate_direct_mem_ptr_cb, i);
//	}

	// TODO: remove ==>	/* register initiator socket callbacks */
	// TODO: remove ==>	init_socket.register_nb_transport_bw(           this, &Router<CONFIG>::I_nb_transport_bw_cb);
	// TODO: remove ==>	init_socket.register_invalidate_direct_mem_ptr( this, &Router<CONFIG>::I_invalidate_direct_mem_ptr_cb);

//	SC_THREAD(Dispatch);
	
	for (unsigned int i = 0; i < OUTPUT_SOCKETS; i++) 
	{
		RouterDispatcher<Router<CONFIG>, CONFIG> *dispatcher;
		std::stringstream sstr;
		sstr << "router_request_dispatcher[" << i << "]";
		dispatcher = new RouterDispatcher<Router<CONFIG>, CONFIG>(sstr.str().c_str(), i, this, &Router<CONFIG>::SendReq);
		m_req_dispatcher.push_back(dispatcher);
	}

	for (unsigned int i = 0; i < INPUT_SOCKETS; i++) 
	{
		RouterDispatcher<Router<CONFIG>, CONFIG> *dispatcher;
		std::stringstream sstr;
		sstr << "router_response_dispatcher[" << i << "]";
		dispatcher = new RouterDispatcher<Router<CONFIG>, CONFIG>(sstr.str().c_str(), i, this, &Router<CONFIG>::SendRsp);
		m_rsp_dispatcher.push_back(dispatcher);
	}
}

template<class CONFIG>
Router<CONFIG>::
~Router() {
	typename std::vector<RouterDispatcher<Router<CONFIG>, CONFIG> *>::iterator m_req_dispatcher_iter;

	for (m_req_dispatcher_iter = m_req_dispatcher.begin(); m_req_dispatcher_iter != m_req_dispatcher.end(); m_req_dispatcher_iter++)
	{
		delete *m_req_dispatcher_iter;
	}

	typename std::vector<RouterDispatcher<Router<CONFIG>, CONFIG> *>::iterator m_rsp_dispatcher_iter;

	for (m_rsp_dispatcher_iter = m_rsp_dispatcher.begin(); m_rsp_dispatcher_iter != m_rsp_dispatcher.end(); m_rsp_dispatcher_iter++)
	{
		delete *m_rsp_dispatcher_iter;
	}

	for (unsigned int i = 0; i < OUTPUT_SOCKETS; i++)
	{
		delete init_socket[i];
	}

	for (unsigned int i = 0; i < INPUT_SOCKETS; i++)
	{
		delete targ_socket[i];
	}

	for (unsigned int i = 0; i < MAX_NUM_MAPPINGS; i++) 
	{
		delete param_mapping[i];
	}

	for (unsigned int i = 0; i < OUTPUT_SOCKETS; i++)
	{
		delete memory_import[i];
	}

	if (param_verbose_all) delete param_verbose_all;
	if (param_verbose_setup) delete param_verbose_setup;
	if (param_verbose_tlm) delete param_verbose_tlm;
	if (param_verbose_tlm_debug) delete param_verbose_tlm_debug;
	if (param_verbose_memory_interface) delete param_verbose_memory_interface;
}

template<class CONFIG>
bool
Router<CONFIG>::
BeginSetup()
{
	const unsigned int num_mappings = CONFIG::MAX_NUM_MAPPINGS;

	SetVerboseAll();

	if (cycle_time == SC_ZERO_TIME) 
	{
		logger << DebugError << "PARAMETER ERROR: the " << param_cycle_time.GetName() << " parameter must be bigger than 0" << endl
			<< LOCATION << EndDebug;
		return false;
	}

	bool has_mapping = false;
	for (unsigned int i = 0; i < num_mappings; i++) 
	{
		if(mapping[i].used) has_mapping = true;	
	}
	if (!has_mapping) 
	{
		logger << DebugError << "PARAMETER ERROR: no mapping was defined" << endl
			<< LOCATION << EndDebug;
		return false;
	}
	for (unsigned int i = 0; i < num_mappings; i++) 
	{
		if (mapping[i].used && mapping[i].output_port >= OUTPUT_SOCKETS) 
		{
			logger << DebugWarning << "PARAMETER ERROR: mapping has a bigger output_port than available ports" << endl
				<< LOCATION << endl;
			logger << "  mapping_" << i << ": " << endl;
			logger << "    - start_range = 0x" << hex << mapping[i].range_start << dec << endl;
			logger << "    - end_range   = 0x" << hex << mapping[i].range_end << dec << endl;
			logger << "    - output_port = " << mapping[i].output_port;
			logger << EndDebug;
		}
	}

	/* initialize ready queues */
	for (unsigned int i = 0; i < INPUT_SOCKETS; i++)
		m_targ_req_ready.push_back(SC_ZERO_TIME);
	for (unsigned int i = 0; i < OUTPUT_SOCKETS; i++)
		m_init_rsp_ready.push_back(SC_ZERO_TIME);
	
	/* display the configuration of the router */
	if (VerboseSetup()) 
	{
		logger << DebugInfo << "Setting cycle_time to " << cycle_time << endl;
		logger << "Mappings (MAX_NUM_MAPPINGS = " << num_mappings << "): ";
		for (unsigned int i = 0; i < num_mappings; i++) 
		{
			if (mapping[i].used) 
			{
				logger << endl << " - " << i << ": start_range = 0x" << hex << mapping[i].range_start << dec << endl;
				logger << "      end_range   = 0x" << hex << mapping[i].range_end << dec << endl;
				logger << "      output_port = " << mapping[i].output_port;
			}
		}
		logger << endl;
		logger << "Number of incomming ports = " << INPUT_SOCKETS << endl;
		logger << "Number of outgoing ports = " << OUTPUT_SOCKETS << endl;
		logger << EndDebug;
	}

	return true;
}

/*************************************************************************
 * Multi passtrough initiator socket callbacks                     START *
 *************************************************************************/

template<class CONFIG>
tlm::tlm_sync_enum 
Router<CONFIG>::
I_nb_transport_bw_cb(int id, transaction_type &trans, phase_type &phase, sc_core::sc_time &time) {
	switch (phase) {
		case tlm::BEGIN_REQ:
		case tlm::END_RESP:
			logger << DebugError << "Received nb_transport_bw on init_socket[" << id << "], with unexpected phase" << endl
				<< TIME(time) << endl
				<< PHASE(phase) << endl;
			TRANS(logger, trans);
			logger << EndDebug;
			Object::Stop(-1);
			break;
		case tlm::BEGIN_RESP:
			/* a response has been received through the init_socket */
			{	
				m_req_dispatcher[id]->Completed(&trans, time);
				trans.acquire();
				/* check when the response can be queued into the response queue through the handled port (recovering the router extension)
				 * we must send an end response message */
				unsigned int targ_id;
				if(!GetRouterExtension(trans, targ_id)) {
					logger << DebugError << "Could not find the router extension from transaction response" << endl
						<< LOCATION << endl
						<< TIME(time) << endl;
					ETRANS(logger, trans);
					logger << EndDebug;
					Object::Stop(-1);
					return tlm::TLM_COMPLETED; // should never occur
				}
				if (VerboseTLM()) {
					logger << DebugInfo << "Received response through init_socket[" << id << "], queueing it to be sent through targ_socket[" << targ_id << "]"  << endl
						<< TIME(time) << endl;
					ETRANS(logger, trans);
					logger << EndDebug;
				}
				/* check when the request can be accepted by the router */
				sc_core::sc_time cur_time = sc_time_stamp() + time;
				if (cur_time <= m_init_rsp_ready[id]) {
					/* the init port is not ready to receive the response, get the time when it will be ready */
					time = m_init_rsp_ready[id] - sc_time_stamp();
					m_init_rsp_ready[id] = m_init_rsp_ready[id] + cycle_time;
				} else {
					/* the inti port is ready for the time the response is received, however we have to make sure
					 *   that the incomming transactions is synchronized with the router cycle_time */
					sc_core::sc_time t_time = ((cycle_time * floor(cur_time / cycle_time)) + cycle_time);
					time = t_time - sc_time_stamp();
					m_init_rsp_ready[id] = t_time + cycle_time;
				}
				/* push the response into the response dispatcher */
				m_rsp_dispatcher[targ_id]->Push(trans, time);
				/* change the phase and signal that the response is completed */
				phase = tlm::END_RESP;
				trans.release();
				return tlm::TLM_COMPLETED;
			}
			break;
		case tlm::END_REQ:
			/* just signal that the socket can be used again */
			m_req_dispatcher[id]->Completed(&trans, time);
			return tlm::TLM_COMPLETED;
			break;
	}
	return tlm::TLM_ACCEPTED;
}

template<class CONFIG>
void
Router<CONFIG>::
I_invalidate_direct_mem_ptr_cb(int id, sc_dt::uint64 start_range, sc_dt::uint64 end_range)
{
	unsigned int mapping_id;
	
	for(mapping_id = 0; mapping_id < MAX_NUM_MAPPINGS; mapping_id++)
	{
		if(mapping[mapping_id].used)
		{
			if(mapping[mapping_id].output_port == (unsigned int) id)
			{
				// do reverse translation on DMI invalidation range
				start_range -= mapping[mapping_id].translation;
				start_range += mapping[mapping_id].range_start;
				end_range -= mapping[mapping_id].translation;
				end_range += mapping[mapping_id].range_start;

				sc_dt::uint64 mapping_start_addr = mapping[mapping_id].range_start;
				sc_dt::uint64 mapping_end_addr = mapping[mapping_id].range_end;
				
				if((mapping_end_addr >= start_range) && (mapping_start_addr <= end_range))
				{
					// collision
					if(mapping_start_addr < start_range) mapping_start_addr = start_range; // cut lower region
					if(mapping_end_addr > end_range) mapping_end_addr = end_range; // cut upper region
				
					// invalidate direct memory pointers on all input sockets
					unsigned int i;
					for(i = 0; i < INPUT_SOCKETS; i++)
					{
						(*targ_socket[i])->invalidate_direct_mem_ptr(mapping_start_addr, mapping_end_addr);
					}
				}
			}
		}
	}
}

/*************************************************************************
 * Multi passtrough initiator socket callbacks                       END *
 *************************************************************************/

/*************************************************************************
 * Multi passtrough target socket callbacks                        START *
 *************************************************************************/

template<class CONFIG>
tlm::tlm_sync_enum
Router<CONFIG>::
T_nb_transport_fw_cb(int id, transaction_type &trans, phase_type &phase, sc_core::sc_time &time) {
	if (trans.get_command() == tlm::TLM_IGNORE_COMMAND) {
		logger << DebugWarning << "Received nb_transport_fw on port " << id << ", with ignore command, which the router doesn't know how to handle" << endl
			<< TIME(time) << endl
			<< PHASE(phase) << endl;
		TRANS(logger, trans);
		logger << EndDebug;
		return tlm::TLM_ACCEPTED;
	}
	switch (phase) {
	case tlm::END_REQ:
	case tlm::BEGIN_RESP:
		logger << DebugError << "Received nb_transport_fw on port " << id << ", with unexpected phase" << endl
			<< TIME(time) << endl
			<< PHASE(phase) << endl;
		TRANS(logger, trans);
		logger << EndDebug;
		Object::Stop(-1);
		break;
	case tlm::BEGIN_REQ:
		{
			trans.acquire();
			if (VerboseTLM()) {
				logger << DebugInfo << "Received nb_transport_fw on port " << id << ", queueing it for future dispatching" << endl
					<< TIME(time) << endl;
				TRANS(logger, trans);
				logger << EndDebug;
			}
			/* check when the request can be accepted by the router */
			sc_core::sc_time cur_time = sc_time_stamp() + time;
			if (cur_time <= m_targ_req_ready[id]) {
				/* the target port is not ready, get the time when it will be ready */
				time = m_targ_req_ready[id] - sc_time_stamp();
				m_targ_req_ready[id] = m_targ_req_ready[id] + cycle_time;
			} else {
				/* the target port is ready for the time the request is received, however we have to make sure
				 *   that the incomming transactions is synchronized with the router cycle_time */
				sc_core::sc_time t_time = ((cycle_time * floor(cur_time / cycle_time)) + cycle_time);
				time = t_time - sc_time_stamp();
				m_targ_req_ready[id] = t_time + cycle_time;
			}

			/* get the init port through which the transaction should be forwarded */
			unsigned int mapping_id;
			if (!ApplyMap(trans, mapping_id)) {
				trans.set_response_status(tlm::TLM_ADDRESS_ERROR_RESPONSE);
				logger << DebugWarning << "When handling received transaction could not apply any mapping" << endl
					<< TIME(time) << endl
					<< PHASE(phase) << endl
					<< LOCATION << endl;
				TRANS(logger, trans);
				logger << EndDebug;
				return tlm::TLM_COMPLETED;
			}

			/* perform the address translation */
			uint64_t translated_addr = trans.get_address();
			translated_addr -= mapping[mapping_id].range_start;
			translated_addr += mapping[mapping_id].translation;
			trans.set_address(translated_addr);

			/* checking command */
			/* insert the input port id into the transaction */
			SetRouterExtension(trans, id);
			/* push the transaction to the corresponding init port queue */
			m_req_dispatcher[mapping[mapping_id].output_port]->Push(trans, time);
			trans.release();
			/* change the phase and return */
			phase = tlm::END_REQ;
			return tlm::TLM_UPDATED;
		}
		break;
	case tlm::END_RESP:
		/* just signal that the socket can be used again */
		m_rsp_dispatcher[id]->Completed(&trans, time);
		/* the transaction is now finished for us, release it */
		trans.release();
		return tlm::TLM_COMPLETED;
		break;
	}
	return tlm::TLM_ACCEPTED;
}

template<class CONFIG>
void
Router<CONFIG>::
T_b_transport_cb(int id, transaction_type &trans, sc_core::sc_time &time)
{
	/* the first thing that must be done is the translation from the mapping table */
	if (VerboseTLM()) 
	{
		logger << DebugInfo << "Received b_transport on port " << id << ", forwarding it" << endl
			<< TIME(time) << endl;
		TRANS(logger, trans);
		logger << EndDebug;
	}
	/* check the address of the transaction to perform the port routing */
	unsigned int mapping_id;
	bool found = ApplyMap(trans, mapping_id);
	if (!found) 
	{
		trans.set_response_status(tlm::TLM_ADDRESS_ERROR_RESPONSE);
		logger << DebugWarning << "Received transaction on port " << id << " has an unmapped address"  << endl
			<< LOCATION << endl
			<< TIME(time) << endl;
		TRANS(logger, trans);
		logger << EndDebug;
		return;
	}
	/* perform the address translation */
	uint64_t translated_addr = trans.get_address();
	translated_addr -= mapping[mapping_id].range_start;
	translated_addr += mapping[mapping_id].translation;
	if (VerboseTLM() && translated_addr != trans.get_address())
	{
		logger << DebugInfo << "Performing address translation on transaction:" << endl;
		TRANS(logger, trans);
		logger << endl
			<< "--> address translated to 0x" << hex << translated_addr << dec
			<< EndDebug;
	}
	trans.set_address(translated_addr);
	/* forward the transaction to the selected output port */
	if(VerboseTLM()) {
		logger << DebugInfo << "Forwarding transaction received on port " << id << " to port " << mapping[mapping_id].output_port << endl
			<< TIME(time) << endl;
		TRANS(logger, trans);
		logger << EndDebug;
	}
	(*init_socket[mapping[mapping_id].output_port])->b_transport(trans, time);
	if (VerboseTLM()) 
	{
		logger << DebugInfo << "Forwarding transaction reply to port " << id << endl
			<< TIME(time) << endl;
		TRANS(logger, trans);
		logger << EndDebug;
	}
}

template<class CONFIG>
unsigned int
Router<CONFIG>::
T_transport_dbg_cb(int id, transaction_type &trans) 
{
	if (VerboseTLMDebug())
	{
		logger << DebugInfo << "Received transport_dbg on port " << id << " forwarding it" << endl;
		TRANS(logger, trans);
		logger << EndDebug;
	}
	// only handle reads and writes
	if (!trans.is_read() && !trans.is_write()) return 0;

	// modify request queues and send the transaction trhough the init_sockets
	unsigned int counter = 0;
	unsigned char *trans_initial_buffer = trans.get_data_ptr();
	unsigned char *trans_buffer = trans.get_data_ptr();
	unsigned int trans_size = trans.get_data_length();
	sc_dt::uint64 trans_addr = trans.get_address();
	std::vector<unsigned int> mappings;
	std::vector<unsigned int>::iterator it;
	ApplyMap(trans, mappings);
	for(it = mappings.begin(); it != mappings.end(); it++)  {
		sc_dt::uint64 buffer_index;
		sc_dt::uint64 buffer_addr;
		unsigned int buffer_size;
		if (mapping[*it].range_start > trans_addr) {
			buffer_addr = mapping[*it].range_start;
			buffer_index = mapping[*it].range_start - trans_addr;
			if (mapping[*it].range_end >= trans_addr + trans_size - 1)
				buffer_size = 1 + (trans_addr + trans_size - 1) - mapping[*it].range_start;
			else
				buffer_size = 1 + mapping[*it].range_end - mapping[*it].range_start;
		} else {
			buffer_addr = trans_addr;
			buffer_index = 0;
			if (mapping[*it].range_end >= trans_addr + trans_size - 1)
				buffer_size = trans_size;
			else
				buffer_size = 1 + mapping[*it].range_end - trans_addr;
		}
		trans.set_data_ptr(trans_buffer + buffer_index);
		trans.set_data_length(buffer_size);
		uint64_t translated_addr = buffer_addr - mapping[*it].range_start + mapping[*it].translation;
		trans.set_address(translated_addr);
		if (VerboseTLMDebug())
		{
			logger << DebugInfo << "Sending transport_dbg on port " << mapping[*it].output_port << endl;
			TRANS(logger, trans);
			logger << EndDebug;
		}
		counter += (*init_socket[mapping[*it].output_port])->transport_dbg(trans);
		if (trans.is_read())
			m_req_dispatcher[mapping[*it].output_port]->ReadTransportDbg(id, trans);
		else
			m_req_dispatcher[mapping[*it].output_port]->WriteTransportDbg(id, trans);
		// if the request is a write then be sure to modify the response queue
		if (trans.is_write())
			m_rsp_dispatcher[id]->WriteTransportDbg(id, trans);
	}
	if (VerboseTLMDebug())
	{
		if (trans.is_read())
		{
			logger << DebugInfo << "Finished transport_dbg reading " << counter << " bytes" << endl
				<< "  with read data = " << hex;
			for (unsigned int i = 0; i < counter; i++)
			{
				logger << (unsigned int)trans_initial_buffer[i] << " ";
			}
			logger << EndDebug;
		}
		else
		{
			logger << DebugInfo << "Finished transport_dbg writing " << counter << "bytes" << EndDebug;
		}
	}
	return counter;
}

template<class CONFIG>
unsigned int
Router<CONFIG>::
ReadTransportDbg(unsigned int id, transaction_type &trans) {
	unsigned int counter = 0;
	unsigned char *trans_buffer = trans.get_data_ptr();
	unsigned int trans_size = trans.get_data_length();
	sc_dt::uint64 trans_addr = trans.get_address();
	std::vector<unsigned int> mappings;
	std::vector<unsigned int>::iterator it;
	ApplyMap(trans, mappings);
	for(it = mappings.begin(); it != mappings.end(); it++) {
		sc_dt::uint64 buffer_index;
		sc_dt::uint64 buffer_addr;
		unsigned int buffer_size;
		if (mapping[*it].range_start > trans_addr) {
			buffer_addr = mapping[*it].range_start;
			buffer_index = mapping[*it].range_start - trans_addr;
			if (mapping[*it].range_end >= trans_addr + trans_size - 1)
				buffer_size = 1 + (trans_addr + trans_size - 1) - mapping[*it].range_start;
			else
				buffer_size = 1 + mapping[*it].range_end - mapping[*it].range_start;
		} else {
			buffer_addr = trans_addr;
			buffer_index = 0;
			if (mapping[*it].range_end >= trans_addr + trans_size - 1)
				buffer_size = trans_size;
			else
				buffer_size = 1 + mapping[*it].range_end - trans_addr;
		}
		trans.set_data_ptr(trans_buffer + buffer_index);
		trans.set_data_length(buffer_size);
		uint64_t translated_addr = buffer_addr - mapping[*it].range_start + mapping[*it].translation;
		trans.set_address(translated_addr);
		counter += (*init_socket[mapping[*it].output_port])->transport_dbg(trans);
		m_req_dispatcher[mapping[*it].output_port]->ReadTransportDbg(id, trans);	
	}
	logger << DebugInfo << "ReadTransportDbg of address 0x" << hex << trans_addr << dec
			<< " with size " << trans_size << ", " << counter << " bytes successfully read" << endl;
	return counter;
}

template<class CONFIG>
unsigned int
Router<CONFIG>::
WriteTransportDbg(unsigned int id, transaction_type &trans) {
	unsigned int counter = 0;
	unsigned char *trans_buffer = trans.get_data_ptr();
	unsigned int trans_size = trans.get_data_length();
	sc_dt::uint64 trans_addr = trans.get_address();
	std::vector<unsigned int> mappings;
	std::vector<unsigned int>::iterator it;
	ApplyMap(trans, mappings);
	for(it = mappings.begin(); it != mappings.end(); it++) {
		sc_dt::uint64 buffer_index;
		sc_dt::uint64 buffer_addr;
		unsigned int buffer_size;
		if (mapping[*it].range_start > trans_addr) {
			buffer_addr = mapping[*it].range_start;
			buffer_index = mapping[*it].range_start - trans_addr;
			if (mapping[*it].range_end >= trans_addr + trans_size - 1)
				buffer_size = 1 + (trans_addr + trans_size - 1) - mapping[*it].range_start;
			else
				buffer_size = 1 + mapping[*it].range_end - mapping[*it].range_start;
		} else {
			buffer_addr = trans_addr;
			buffer_index = 0;
			if (mapping[*it].range_end >= trans_addr + trans_size - 1)
				buffer_size = trans_size; 
			else
				buffer_size = 1 + mapping[*it].range_end - trans_addr;
		}
		trans.set_data_ptr(trans_buffer + buffer_index);
		trans.set_data_length(buffer_size);
		uint64_t translated_addr = buffer_addr - mapping[*it].range_start + mapping[*it].translation;
		trans.set_address(translated_addr);
		counter += (*init_socket[mapping[*it].output_port])->transport_dbg(trans);
		m_req_dispatcher[mapping[*it].output_port]->WriteTransportDbg(id, trans);	
	}
	return counter;
}

template <class CONFIG>
bool
Router<CONFIG> ::
ReadMemory(uint64_t addr, void *buffer, uint32_t size)
{
	if (VerboseMemoryInterface())
	{
		logger << DebugInfo << "Received memory interface ReadMemory with address=0x" << hex << addr << dec
			<< " and size=" << size
			<< ", forwarding it" << EndDebug;
	}
	std::vector<unsigned int> mappings;
	std::vector<unsigned int>::iterator it;
	ApplyMap(addr, size, mappings);
	for(it = mappings.begin(); it != mappings.end(); it++) {
		sc_dt::uint64 buffer_index;
		sc_dt::uint64 buffer_addr;
		unsigned int buffer_size;
		if (mapping[*it].range_start > addr) {
			buffer_addr = mapping[*it].range_start;
			buffer_index = mapping[*it].range_start - addr;
			if (mapping[*it].range_end >= addr + size - 1)
				buffer_size = 1 + (addr + size - 1) - mapping[*it].range_start;
			else
				buffer_size = 1 + mapping[*it].range_end - mapping[*it].range_start;
		} else {
			buffer_addr = addr;
			buffer_index = 0;
			if (mapping[*it].range_end >= addr + size - 1)
				buffer_size = size;
			else
				buffer_size = 1 + mapping[*it].range_end - addr;
		}
		uint64_t translated_addr = buffer_addr - mapping[*it].range_start + mapping[*it].translation;
		if (VerboseMemoryInterface())
		{
			logger << DebugInfo << "Sending ReadMemory on port " << mapping[*it].output_port << " with addr 0x" << hex << buffer_addr << dec
				<< "(translated to 0x" << hex << translated_addr << dec << ")"
				<< " and size " << buffer_size << "(" << size << ")" << EndDebugInfo;
		}
		if (!(*memory_import[mapping[*it].output_port])) return false;
		if (!(*memory_import[mapping[*it].output_port])->ReadMemory(translated_addr, ((uint8_t *)buffer) + buffer_index, buffer_size))
			return false;
	}
	return true;
}

template <class CONFIG>
bool
Router<CONFIG> ::
WriteMemory(uint64_t addr, const void *buffer, uint32_t size)
{
	if (VerboseMemoryInterface())
	{
		logger << DebugInfo << "Received memory interface WriteMemory with address=0x" << hex << addr << dec
			<< " and size=" << size
			<< ", forwarding it" << EndDebug;
	}
	std::vector<unsigned int> mappings;
	std::vector<unsigned int>::iterator it;
	ApplyMap(addr, size, mappings);
	for(it = mappings.begin(); it != mappings.end(); it++) {
		sc_dt::uint64 buffer_index;
		sc_dt::uint64 buffer_addr;
		unsigned int buffer_size;
		if (mapping[*it].range_start > addr) {
			buffer_addr = mapping[*it].range_start;
			buffer_index = mapping[*it].range_start - addr;
			if (mapping[*it].range_end >= addr + size - 1)
				buffer_size = 1 + (addr + size - 1) - mapping[*it].range_start;
			else
				buffer_size = 1 + mapping[*it].range_end - mapping[*it].range_start;
		} else {
			buffer_addr = addr;
			buffer_index = 0;
			if (mapping[*it].range_end >= addr + size - 1)
				buffer_size = size;
			else
				buffer_size = 1 + mapping[*it].range_end - addr;
		}
		uint64_t translated_addr = buffer_addr - mapping[*it].range_start + mapping[*it].translation;
		if (VerboseMemoryInterface())
		{
			logger << DebugInfo << "Sending WriteMemory on port " << mapping[*it].output_port << " with addr 0x" << hex << addr << dec
				<< " (translated to 0x" << hex << translated_addr << dec << ")"
				<< " and size " << buffer_size << "(" << size << ")" << endl
				<< "  with data = " << hex;
			for (unsigned int i = 0; i < buffer_size; i++)
				logger << (unsigned int)(((uint8_t *)buffer)[buffer_index + i]) << " ";
			logger << dec << EndDebugInfo;
		}
		if (!(*memory_import[mapping[*it].output_port])) return false;
		if (!(*memory_import[mapping[*it].output_port])->WriteMemory(translated_addr, ((uint8_t *)buffer) + buffer_index, buffer_size))
			return false;
	}
	return true;
}

template<class CONFIG>
bool
Router<CONFIG>::
T_get_direct_mem_ptr_cb(int id, transaction_type &trans, tlm::tlm_dmi &dmi) {
	/* the first thing that must be done is the translation from the mapping table */
	if (VerboseTLM()) 
	{
		logger << DebugInfo << "Received get_direct_mem_ptr on port " << id << ", forwarding it" << endl;
		TRANS(logger, trans);
		logger << EndDebug;
	}
	/* check the address of the transaction to perform the port routing */
	unsigned int mapping_id;
	bool found = ApplyMap(trans, mapping_id);
	if (!found) 
	{
		logger << DebugError << "Received get_direct_mem_ptr transaction on port " << id << " has an unmapped address"  << endl
			<< LOCATION << endl;
		TRANS(logger, trans);
		logger << EndDebug;
		dmi.set_granted_access(tlm::tlm_dmi::DMI_ACCESS_READ_WRITE);
		dmi.set_start_address(0);
		dmi.set_end_address((sc_dt::uint64) -1);
		return false;
	}
	/* perform the address translation */
	uint64_t translated_addr = trans.get_address();
	translated_addr -= mapping[mapping_id].range_start;
	translated_addr += mapping[mapping_id].translation;
	if (VerboseTLM() && translated_addr != trans.get_address())
	{
		logger << DebugInfo << "Performing address translation on transaction:" << endl;
		TRANS(logger, trans);
		logger << endl
			<< "--> address translated to 0x" << hex << translated_addr << dec
			<< EndDebug;
	}
	trans.set_address(translated_addr);
	/* forward the transaction to the selected output port */
	if(VerboseTLM()) {
		logger << DebugInfo << "Forwarding get_direct_mem_ptr received on port " << id << " to port " << mapping[mapping_id].output_port << endl;
		TRANS(logger, trans);
		logger << EndDebug;
	}
	bool dmi_status = (*init_socket[mapping[mapping_id].output_port])->get_direct_mem_ptr(trans, dmi);
	
	sc_dt::uint64 dmi_start_address = dmi.get_start_address();
	sc_dt::uint64 dmi_end_address = dmi.get_end_address();
	
	// do reverse translation on DMI
	dmi_start_address -= mapping[mapping_id].translation;
	dmi_start_address += mapping[mapping_id].range_start;
	dmi_end_address -= mapping[mapping_id].translation;
	dmi_end_address += mapping[mapping_id].range_start;

	// restrict address range of DMI
	sc_dt::uint64 start_range = mapping[mapping_id].range_start;
	sc_dt::uint64 end_range = mapping[mapping_id].range_end;
	
	if(dmi_start_address < start_range)
	{
		// cut lower region
		dmi.set_dmi_ptr(dmi.get_dmi_ptr() + (start_range - dmi_start_address));
		dmi.set_start_address(start_range);
	}
	else
	{
		dmi.set_start_address(dmi_start_address);
	}
		
	if(dmi_end_address > end_range)
	{
		// cut upper region
		dmi.set_end_address(end_range);
	}
	else
	{
		dmi.set_end_address(dmi_end_address);
	}

	// add router latency per byte
	if(dmi_status)
	{
		dmi.set_read_latency(dmi.get_read_latency() + (cycle_time / (CONFIG::BUSWIDTH / 8)));
		dmi.set_write_latency(dmi.get_write_latency() + (cycle_time / (CONFIG::BUSWIDTH / 8)));
	}
	return dmi_status;
}

/*************************************************************************
 * Multi passthrough target socket callbacks                         END *
 *************************************************************************/

/*************************************************************************
 * Dispatch handlers                                               START *
 *************************************************************************/

template<class CONFIG>
void 
Router<CONFIG>::
SendReq(unsigned int id, transaction_type &trans) {
	/* a request is ready to be sent through the init_port
	 *   prepare phase and time */
	phase_type phase = tlm::BEGIN_REQ;
	sc_core::sc_time time(SC_ZERO_TIME);
	
	if (VerboseTLM()) {
		logger << DebugInfo << "Sending transaction request through init_socket[" << id << "]" << endl
			<< TIME(time) << endl;
		TRANS(logger, trans);
		logger << EndDebug;
	}
	
	switch((*init_socket[id])->nb_transport_fw(trans, phase, time)) {
		case tlm::TLM_ACCEPTED:
			/* the request has been accepted */
			if (VerboseTLM()) {
				logger << DebugInfo << "Transaction request sent from init_req_fifo[" << id << "] accepted (TLM_ACCEPTED), waiting for the corresponding END_REQ" << endl
					<< TIME(time) << endl;
				TRANS(logger, trans);
				logger << EndDebug;
			}
			/* nothing else to do, just wait for the corresponding END_REQ */
			break;
		case tlm::TLM_UPDATED:
			/* check if the phase has changed */
			switch(phase) {
				case tlm::BEGIN_REQ:
					/* phase has not changed, consider it as a TLM_ACCEPTED */
					if (VerboseTLM()) {
						logger << DebugInfo << "Transaction request sent from init_req_fifo[" << id << "] accepted (TLM_UPDATED, BEGIN_REQ), waiting for the corresponding END_REQ" << endl
							<< TIME(time) << endl;
						TRANS(logger, trans);
						logger << EndDebug;
					}
					/* nothing else to do, just wait for the corresponding END_REQ */
					break;
				case tlm::END_REQ:
					if (VerboseTLM()) {
						logger << DebugInfo << "Transaction sent from init_req_fifo[" << id << "] accepted (TLM_UPDATED with phase END_REQ), removing the transaction from the request queue" << endl
							<< TIME(time) << endl;
						TRANS(logger, trans);
						logger << EndDebug;
					}
					m_req_dispatcher[id]->Completed(&trans, time);
					break;	
				case tlm::BEGIN_RESP:
					/* the request has been accepted, and the response has been produced
					 * check when the response can be queued into the response queue through the handled port (recovering the router extension)
					 * we must send an end response message */
					{
						unsigned int targ_id;
						/* notify to the request dispatcher that the transaction request has been completed */
						m_req_dispatcher[id]->Completed(&trans, time);
						if(!GetRouterExtension(trans, targ_id)) {
							logger << DebugError << "Could not find the router extension from transaction response" << endl
								<< LOCATION << endl
								<< TIME(time) << endl;
							ETRANS(logger, trans);
							logger << EndDebug;
							Object::Stop(-1);
							return; // should never occur
						}
						if (VerboseTLM()) {
							logger << DebugInfo << "Transaction request sent through the init_port[" << id << "] accepted and response received (TLM_UPDATED, BEGIN_REQ), queueing the response to be sent through the targ_socket[" << targ_id << "]" << endl
								<< TIME(time) << endl;
							ETRANS(logger, trans);
							logger << EndDebug;
						}
						/* check when the request can be accepted by the router */
						sc_core::sc_time cur_time = sc_time_stamp() + time;
						if (cur_time <= m_init_rsp_ready[id]) {
							/* the init port is not ready to receive the response, get the time when it will be ready */
							time = m_init_rsp_ready[id] - sc_time_stamp();
							m_init_rsp_ready[id] = m_init_rsp_ready[id] + cycle_time;
						} else {
							/* the inti port is ready for the time the response is received, however we have to make sure
							 *   that the incomming transactions is synchronized with the router cycle_time */
							sc_core::sc_time t_time = ((cycle_time * floor(cur_time / cycle_time)) + cycle_time);
							time = t_time - sc_time_stamp();
							m_init_rsp_ready[id] = t_time + cycle_time;
						}
						/* push the response into the response dispatcher */
						m_rsp_dispatcher[targ_id]->Push(trans, time);

						/* immediately send an END_REQ through the init_port */
						phase = tlm::END_REQ;
						if((*init_socket[id])->nb_transport_fw(trans, phase, time) != tlm::TLM_COMPLETED) {
							logger << DebugError << "When sending END_REQ did not receive TLM_COMPLETED from init_socket[" << id << "]" << endl
								<< LOCATION << endl
								<< TIME(time) << endl;
							ETRANS(logger, trans);
							logger << EndDebug;
							Object::Stop(-1);
						}
					}
					break;
				case tlm::END_RESP:
					/* unexpected phase changes */
					logger << DebugError << "Unexpected phase change when sending request transaction through init_socket[" << id << "]" << endl
						<< LOCATION << endl
						<< TIME(time) << endl
						<< PHASE(phase) << endl;
					TRANS(logger, trans);
					logger << EndDebug;
					Object::Stop(-1);
					break;
			}
			break;
		case tlm::TLM_COMPLETED:
			/* the init_port has completely handled the transaction request
			 * if the request was a write we can remove the transaction
			 * if the request was a read we have to forward the response to the corresponding targ_port */
			{
				/* notify to the request dispatcher that the transaction request has been completed */
				m_req_dispatcher[id]->Completed(&trans, time);
				unsigned int targ_id;
				if(!GetRouterExtension(trans, targ_id)) {
					logger << DebugError << "Could not find the router extension from transaction response" << endl
						<< LOCATION << endl
						<< TIME(time) << endl;
					ETRANS(logger, trans);
					logger << EndDebug;
					Object::Stop(-1);
					return; // should never occur
				}
				if (VerboseTLM()) {
					logger << DebugInfo << "Transaction request sent through the init_port[" << id << "] accepted and response received (TLM_UPDATED, BEGIN_REQ), queueing the response to be sent through the targ_socket[" << targ_id << "]" << endl
						<< TIME(time) << endl;
					ETRANS(logger, trans);
					logger << EndDebug;
				}
				/* check when the request can be accepted by the router */
				sc_core::sc_time cur_time = sc_time_stamp() + time;
				if (cur_time <= m_init_rsp_ready[id]) {
					/* the init port is not ready to receive the response, get the time when it will be ready */
					time = m_init_rsp_ready[id] - sc_time_stamp();
					m_init_rsp_ready[id] = m_init_rsp_ready[id] + cycle_time;
				} else {
					/* the init port is ready for the time the response is received, however we have to make sure
						*   that the incomming transactions is synchronized with the router cycle_time */
					sc_core::sc_time t_time = ((cycle_time * floor(cur_time / cycle_time)) + cycle_time);
					time = t_time - sc_time_stamp();
					m_init_rsp_ready[id] = t_time + cycle_time;
				}
				/* push the response into the response dispatcher */
				m_rsp_dispatcher[targ_id]->Push(trans, time);
			}
			break;
	}
}

template<class CONFIG>
void 
Router<CONFIG>::
SendRsp(unsigned int id, transaction_type &trans) {
	/* a response is ready to be sent through the targ_port[id]
	 *   prepare phase and time */
	phase_type phase = tlm::BEGIN_RESP;
	sc_core::sc_time time(SC_ZERO_TIME);
	
	if (VerboseTLM()) {
		logger << DebugInfo << "Sending transaction response through targ_socket[" << id << "]" << endl
			<< TIME(time) << endl;
		TRANS(logger, trans);
		logger << EndDebug;
	}
	
	switch((*targ_socket[id])->nb_transport_bw(trans, phase, time)) {
		case tlm::TLM_ACCEPTED:
			/* the response has been accepted */
			if (VerboseTLM()) {
				logger << DebugInfo << "Transaction response sent through targ_socket[" << id << "] accepted (TLM_ACCEPTED), waiting for the corresponding END_RESP" << endl
					<< TIME(time) << endl;
				ETRANS(logger, trans);
				logger << EndDebug;
			}
			/* nothing else to do, just wait for the corresponding END_REQ */
			break;
		case tlm::TLM_UPDATED:
			logger << DebugError << "Unexpected TLM_UPDATED received when sending transaction response through targ_socket[" << id << "]" << endl
				<< LOCATION << endl
				<< TIME(time) << endl
				<< PHASE(phase) << endl;
			ETRANS(logger, trans);
			logger << EndDebug;
			break;
		case tlm::TLM_COMPLETED:
			if (VerboseTLM()) {
				logger << DebugInfo << "Transaction response sent through targ_socket[" << id << "] completed (TLM_COMPLETED), removing it from the router" << endl
					<< TIME(time) << endl;
				ETRANS(logger, trans);
				logger << EndDebug;
			}
			/* the response has been completed, we can remove it from the response queue */
			m_rsp_dispatcher[id]->Completed(&trans, time);
			/* release the transaction */
			break;
	}
}

/*************************************************************************
 * Dispatch handlers                                                 END *
 *************************************************************************/

/*************************************************************************
 * Helper methods                                                  START *
 *************************************************************************/

template<class CONFIG>
inline sc_time
Router<CONFIG>::
Sync(const sc_time &time) {
	sc_core::sc_time wait_time;
	wait_time = time + sc_time_stamp();
	double div = wait_time / cycle_time;
	wait_time = cycle_time * (std::floor(div) + 1);
	wait_time = wait_time - sc_time_stamp();
	//dispatch_event.notify(wait_time);
	return wait_time;
/*		sc_core::sc_time wait_time;
		if (cycle_time > time) {
			wait_time = cycle_time;
		} else {
			double div = time / cycle_time;
			wait_time = cycle_time * (std::floor(div) + 1);
		}
		dispatch_event.notify(wait_time);
*/
}
	
template <class CONFIG>
inline bool
Router<CONFIG> ::
ApplyMap(uint64_t addr, uint32_t size, unsigned int &applied_mapping) const
{
	bool found = false;
	for(unsigned int i = 0; !found && i < MAX_NUM_MAPPINGS; i++) {
		if(mapping[i].used) {
			if((addr >= mapping[i].range_start) && ((addr + size - 1) <= mapping[i].range_end)) {
				found = true;
				applied_mapping = i;
			}
		}
	}

	return found;
}

template <class CONFIG>
inline bool
Router<CONFIG> ::
ApplyMap(const transaction_type &trans, unsigned int &applied_mapping) const
{
	uint64_t address = trans.get_address();
	unsigned int size = trans.get_data_length();
	return ApplyMap(address, size, applied_mapping);
}

template <class CONFIG>
inline void 
Router<CONFIG> ::
ApplyMap(uint64_t addr, uint32_t size, std::vector<unsigned int> &port_mappings) const
{
	sc_dt::uint64 cur_addr = addr;
	unsigned int cur_size = size;
	while (cur_size) {
		unsigned int index = 0;
		bool found = false;
		for (; !found && index < MAX_NUM_MAPPINGS; index++) {
			if (mapping[index].used) {
				if (cur_addr >= mapping[index].range_start && cur_addr <= mapping[index].range_end) {
					found = true;
					port_mappings.push_back(index);
					if(cur_size > (mapping[index].range_end - cur_addr + 1)) // partially covered
					{
						sc_dt::uint64 next_addr = mapping[index].range_end + 1;
						if(cur_addr > next_addr) return; // detect address overflow
						cur_addr = next_addr;
						cur_size = cur_size - (mapping[index].range_end - cur_addr + 1);
					}
					else // totally covered
					{
						sc_dt::uint64 next_addr = cur_addr + cur_size;
						if(cur_addr > next_addr) return; // detect address overflow
						cur_addr = next_addr;
						cur_size = 0;
					}
				}
			}
		}
		if (!found) {
			sc_dt::uint64 closest_range_start = 0;
			for (index = 0; index < MAX_NUM_MAPPINGS; index++) {
				if (mapping[index].used) {
					if ((cur_addr < mapping[index].range_start) && ((cur_addr + cur_size) > mapping[index].range_start)) {
						if (!found)
							closest_range_start = mapping[index].range_start;
						else
							if (closest_range_start > mapping[index].range_start)
								closest_range_start = mapping[index].range_start;
						found = true;
					}
				}
			}
			if (!found) return;
			cur_size = cur_size - (closest_range_start - cur_addr);
			cur_addr = closest_range_start;
		}
	}
}

template<class CONFIG>
inline void
Router<CONFIG>::
ApplyMap(const transaction_type &trans, std::vector<unsigned int> &port_mappings) const
{
	sc_dt::uint64 addr = trans.get_address();
	unsigned int size = trans.get_data_length();
	ApplyMap(addr, size, port_mappings);
}

template<class CONFIG>
inline void
Router<CONFIG>::
SetRouterExtension(transaction_type &trans, unsigned int port) 
{
	RouterPayloadExtension *router_extension;
	trans.get_extension(router_extension);
	if(!router_extension) {
		router_extension = new RouterPayloadExtension();
		router_extension->Push(port);
		trans.set_extension(router_extension);
	} else {
		router_extension->Push(port);
	}
}

template<class CONFIG>
inline bool
Router<CONFIG>::
GetRouterExtension(transaction_type &trans, unsigned int &port) {
	RouterPayloadExtension *router_extension;
	trans.get_extension(router_extension);
	if(!router_extension) return false;
	if(router_extension->Empty()) return false;
	port = router_extension->Front();
	router_extension->Pop();
	return true;
}

/*************************************************************************
 * Helper methods                                                    END *
 *************************************************************************/

/*************************************************************************
 * Verbose methods                                                 START *
 *************************************************************************/

template <class CONFIG>
inline void
Router<CONFIG> ::
SetVerboseAll()
{
	if (VERBOSE && verbose_all)
	{
		verbose_setup = true;
		verbose_tlm = true;
		verbose_tlm_debug = true;
		verbose_memory_interface = true;
	}
}

template<class CONFIG>
inline bool 
Router<CONFIG> ::
VerboseSetup() 
{
	if (VERBOSE && verbose_setup) 
		return true;
	return false;
}

template<class CONFIG>
inline bool
Router<CONFIG> ::
VerboseTLM() 
{
	if (VERBOSE && verbose_tlm)
		return true;
	return false;
}

template <class CONFIG>
inline bool
Router<CONFIG> ::
VerboseTLMDebug() 
{
	if (VERBOSE && verbose_tlm_debug)
		return true;
	return false;
}

template <class CONFIG>
inline bool
Router<CONFIG> ::
VerboseMemoryInterface()
{
	if (VERBOSE && verbose_memory_interface)
		return true;
	return false;
}

/*************************************************************************
 * Verbose methods                                                   END *
 *************************************************************************/

} // end of namespace generic_router
} // end of namespace interconnect
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_INTERCONNECT_GENERIC_ROUTER_TCC__
