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
 */
 
#ifndef __UNISIM_COMPONENT_TLM2_INTERCONNECT_GENERIC_ROUTER_TCC__
#define __UNISIM_COMPONENT_TLM2_INTERCONNECT_GENERIC_ROUTER_TCC__

#include "unisim/component/tlm2/interconnect/generic_router/router_dispatcher.tcc"
#include <cmath>

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

template<class CONFIG>
Router<CONFIG>::
Router(const sc_module_name &name, Object *parent) :
sc_module(name),
Object(name, parent),
// TODO: remove ==> targ_socket("targ_socket"),
// TODO: remove ==> init_socket("init_socket"), 
m_req_dispatcher(),
m_rsp_dispatcher(),
cycle_time(SC_ZERO_TIME),
cycle_time_double(0.0),
param_cycle_time_double("cycle_time", this, cycle_time_double, "Time to process a request/response by the router in SC_PS)"),
num_input_sockets(0),
param_num_input_sockets("num_input_sockets", this, num_input_sockets, "Defines the number of input sockets used (must be bigger than 0)"),
num_output_sockets(0),
param_num_output_sockets("num_output_sockets", this, num_output_sockets, "Defines the number of output sockets used (must be bigger than 0)"),
port_buffer_size(0),
param_port_buffer_size("port_buffer_size", this, port_buffer_size, "Defines the size of the buffer for incomming requests in each of the input ports (0 = infinite)"),
logger(*this),
verbose_all(false),
verbose_setup(false),
verbose_non_blocking(false),
verbose_blocking(false)
{
	if (DEBUG)
	{
		param_verbose_all = new unisim::kernel::service::Parameter<bool>("verbose_all", this, verbose_all, "Activate all the verbose options");
		param_verbose_setup = new unisim::kernel::service::Parameter<bool>("verbose_setup", this, verbose_setup, "Display Object setup information");
		param_verbose_non_blocking = new unisim::kernel::service::Parameter<bool>("verbose_non_blocking", this, verbose_non_blocking, "Display non_blocking transactions handling");
		param_verbose_blocking = new unisim::kernel::service::Parameter<bool>("verbose_blocking", this, verbose_blocking, "Display blocking transactions handling");
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

	/* create target sockets and register socket callbacks */
	for (unsigned int i = 0; i < MAX_INPUT_SOCKETS; i++)
	{
		std::stringstream buf;
		buf << "targ_socket[" << i << "]";
		targ_socket[i] = new tlm_utils::passthrough_target_socket_tagged<Router, BUSWIDTH, TYPES>(buf.str().c_str());
		targ_socket[i]->register_nb_transport_fw(	this, &Router<CONFIG>::T_nb_transport_fw_cb, i);
		targ_socket[i]->register_b_transport(		this, &Router<CONFIG>::T_b_transport_cb, i);
		targ_socket[i]->register_transport_dbg(		this, &Router<CONFIG>::T_transport_dbg_cb, i);
		targ_socket[i]->register_get_direct_mem_ptr(this, &Router<CONFIG>::T_get_direct_mem_ptr_cb, i);
	}

	// TODO: remove ==>	/* register target multi socket callbacks */
 	// TODO: remove ==>	targ_socket.register_nb_transport_fw(    this, &Router<CONFIG>::T_nb_transport_fw_cb);
 	// TODO: remove ==>	targ_socket.register_b_transport(        this, &Router<CONFIG>::T_b_transport_cb);
 	// TODO: remove ==>	targ_socket.register_transport_dbg(      this, &Router<CONFIG>::T_transport_dbg_cb);
 	// TODO: remove ==>	targ_socket.register_get_direct_mem_ptr( this, &Router<CONFIG>::T_get_direct_mem_ptr_cb);

	/* create initiator sockets and register socket callbacks */
	for (unsigned int i = 0; i < MAX_OUTPUT_SOCKETS; i++)
	{
		std::stringstream buf;
		buf << "init_socket[" << i << "]";
		init_socket[i] = new tlm_utils::simple_initiator_socket_tagged<Router, BUSWIDTH, TYPES>(buf.str().c_str());
		init_socket[i]->register_nb_transport_bw(			this, &Router<CONFIG>::I_nb_transport_bw_cb, i);
		init_socket[i]->register_invalidate_direct_mem_ptr(	this, &Router<CONFIG>::I_invalidate_direct_mem_ptr_cb, i);
	}

	// TODO: remove ==>	/* register initiator socket callbacks */
	// TODO: remove ==>	init_socket.register_nb_transport_bw(           this, &Router<CONFIG>::I_nb_transport_bw_cb);
	// TODO: remove ==>	init_socket.register_invalidate_direct_mem_ptr( this, &Router<CONFIG>::I_invalidate_direct_mem_ptr_cb);

//	SC_THREAD(Dispatch);
}

template<class CONFIG>
Router<CONFIG>::
~Router() {
	typename std::vector<RouterDispatcher<Router<CONFIG>, CONFIG> *>::iterator m_req_dispatcher_iter;

	for(m_req_dispatcher_iter = m_req_dispatcher.begin(); m_req_dispatcher_iter != m_req_dispatcher.end(); m_req_dispatcher_iter++)
	{
		delete *m_req_dispatcher_iter;
	}

	typename std::vector<RouterDispatcher<Router<CONFIG>, CONFIG> *>::iterator m_rsp_dispatcher_iter;

	for(m_rsp_dispatcher_iter = m_rsp_dispatcher.begin(); m_rsp_dispatcher_iter != m_rsp_dispatcher.end(); m_rsp_dispatcher_iter++)
	{
		delete *m_rsp_dispatcher_iter;
	}

	for(unsigned int i = 0; i < MAX_NUM_MAPPINGS; i++) {
		delete param_mapping[i];
	}
}

template<class CONFIG>
bool
Router<CONFIG>::
Setup()
{
	const unsigned int num_mappings = CONFIG::MAX_NUM_MAPPINGS;
	const unsigned int max_input_sockets = CONFIG::MAX_INPUT_SOCKETS;
	const unsigned int max_output_sockets = CONFIG::MAX_OUTPUT_SOCKETS;

	if (num_input_sockets == 0 || num_input_sockets > max_input_sockets)
	{
		logger << DebugError << "PARAMETER ERROR: the num_input_sockets parameter must be bigger than 0 and smaller than " << (max_input_sockets + 1) << endl
			<< LOCATION << EndDebug;
		return false;
	}

	for (unsigned int i = num_input_sockets; i < max_input_sockets; i++)
		delete targ_socket[i];

	if (num_output_sockets == 0 || num_output_sockets > max_output_sockets)
	{
		logger << DebugError << "PARAMETER ERROR: the num_output_sockets parameter must be bigger than 0 and smaller than" << (max_output_sockets + 1) << endl
			<< LOCATION << EndDebug;
		return false;
	}

	for (unsigned int i = num_output_sockets; i < max_output_sockets; i++)
		delete init_socket[i];

	if (cycle_time_double == 0.0) 
	{
		logger << DebugError << "PARAMETER ERROR: the cycle_time parameter must be bigger than 0" << endl
			<< LOCATION << EndDebug;
		return false;
	}

	cycle_time = sc_time(cycle_time_double, SC_PS);

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
		if (mapping[i].used && mapping[i].output_port >= num_output_sockets) 
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

	for (unsigned int i = 0; i < num_output_sockets; i++) 
	{
		RouterDispatcher<Router<CONFIG>, CONFIG> *dispatcher;
		std::stringstream sstr;
		sstr << "router_request_dispatcher[" << i << "]";
		dispatcher = new RouterDispatcher<Router<CONFIG>, CONFIG>(sstr.str().c_str(), i, cycle_time, this, &Router<CONFIG>::SendReq);
		m_req_dispatcher.push_back(dispatcher);
	}

	for (unsigned int i = 0; i < num_input_sockets; i++) 
	{
		RouterDispatcher<Router<CONFIG>, CONFIG> *dispatcher;
		std::stringstream sstr;
		sstr << "router_response_dispatcher[" << i << "]";
		dispatcher = new RouterDispatcher<Router<CONFIG>, CONFIG>(sstr.str().c_str(), i, cycle_time, this, &Router<CONFIG>::SendRsp);
		m_rsp_dispatcher.push_back(dispatcher);
	}

	/* initialize ready queues */
	for (unsigned int i = 0; i < num_input_sockets; i++)
		m_targ_req_ready.push_back(SC_ZERO_TIME);
	for (unsigned int i = 0; i < num_output_sockets; i++)
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
		logger << "Number of incomming ports = " << num_input_sockets << endl;
		logger << "Number of outgoing ports = " << num_output_sockets << endl;
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
	if (trans.get_command() == tlm::TLM_IGNORE_COMMAND || trans.get_command() == tlm::TLM_WRITE_COMMAND) {
		logger << DebugWarning << "Received nb_transport_bw on port " << id << ", with an ignore or a write command, which the router doesn't know how to handle" << endl
			<< TIME(time) << endl
			<< PHASE(phase) << endl;
		TRANS(logger, trans);
		logger << EndDebug;
		return tlm::TLM_ACCEPTED;
	}
	switch (phase) {
		case tlm::BEGIN_REQ:
		case tlm::END_RESP:
			logger << DebugError << "Received nb_transport_bw on init_socket[" << id << "], with unexpected phase" << endl
				<< TIME(time) << endl
				<< PHASE(phase) << endl;
			TRANS(logger, trans);
			logger << EndDebug;
			sc_stop();
			wait();
			break;
		case tlm::BEGIN_RESP:
			/* a response has been received through the init_socket */
			{
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
					sc_stop();
					wait();
				}
				if (VerboseNonBlocking()) {
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
			m_req_dispatcher[id]->Completed(time);
			/* if the transaction is a write, we do not expect a response and the request is finished for us.
			 *   we can release it */
			if (trans.is_write()) {
				trans.release();
			}
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
	/* nothing to do */
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
		sc_stop();
		wait();
		break;
	case tlm::BEGIN_REQ:
		{
			trans.acquire();
			if (VerboseNonBlocking()) {
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
			unsigned int init_id;
			if (!ApplyMap(trans, init_id)) {
				logger << DebugError << "When handling received transaction mapping output port is bigger or equal than number of output ports ("
					<< init_id << ">=" << num_output_sockets << ")" << endl
					<< TIME(time) << endl
					<< PHASE(phase) << endl
					<< LOCATION << endl;
				TRANS(logger, trans);
				logger << EndDebug;
				sc_stop();
				wait();
			}
			/* checking command */
			if (trans.is_read()) {
				/* insert the input port id into the transaction */
				SetRouterExtension(trans, id);
				/* push the transaction to the corresponding init port queue */
				m_req_dispatcher[init_id]->Push(trans, time);
				trans.release();
				/* change the phase and return */
				phase = tlm::END_REQ;
				return tlm::TLM_UPDATED;
			} else {
				/* trans is a write, the request is complete at this point so we need to copy it to forward it */
				transaction_type *clone_trans = payload_fabric.allocate();
				// clone_trans->acquire(); // not necessary because it is being done in the payload_fabric
				unsigned char *data = (unsigned char *)malloc(sizeof(unsigned char) * trans.get_data_length());
				clone_trans->set_data_ptr(data);
				if (trans.get_byte_enable_length() != 0) {
					unsigned char *byte_enable = (unsigned char *)malloc(sizeof(unsigned char) * trans.get_byte_enable_length());
					clone_trans->set_byte_enable_ptr(byte_enable);
				}
				clone_trans->deep_copy_from(trans);
				/* push the transaction to the corresponding init port queue */
				m_req_dispatcher[init_id]->Push(*clone_trans, time);
				/* finish the incomming request */
				phase = tlm::END_REQ;
				trans.set_response_status(tlm::TLM_OK_RESPONSE);
				trans.release();
				clone_trans->release();
				return tlm::TLM_COMPLETED;
			}
		}
		break;
	case tlm::END_RESP:
		/* just signal that the socket can be used again */
		m_rsp_dispatcher[id]->Completed(time);
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
	if(VerboseBlocking()) {
		logger << DebugInfo << "Received b_transport on port " << id << ", forwarding it" << endl
			<< TIME(time) << endl;
		TRANS(logger, trans);
		logger << EndDebug;
	}
	/* check the address of the transaction to perform the port routing */
	unsigned int port;
	bool found = ApplyMap(trans, port);
	if(!found) {
		logger << DebugError << "Received transaction on port " << id << " has an unmapped address"  << endl
			<< LOCATION << endl
			<< TIME(time) << endl;
		TRANS(logger, trans);
		logger << EndDebug;
		return;
	}
	/* check that output port is within bounds??? */
	if(mapping[port].output_port >= num_output_sockets) {
		logger << DebugError << "When handling received transaction" << endl;
		TRANS(logger, trans);
		logger << endl << "mapping output port is bigger or equal than number of output ports ("
			<< mapping[port].output_port << ">=" << num_output_sockets << ")"
			<< EndDebug;
		return;
	}
	// SetRouterExtension(trans, id); // extensions are not needed for the blocking version of the transport method
	//                                //   because the context is keeped
	/* forward the transaction to the selected output port */
	if(VerboseBlocking()) {
		logger << DebugInfo << "Forwarding transaction received on port " << id << " to port " << port << endl
			<< TIME(time) << endl;
		TRANS(logger, trans);
		logger << EndDebug;
	}
	(*init_socket[port])->b_transport(trans, time);
	if (VerboseBlocking()) 
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
	// only handle reads and writes
	if (!trans.is_read() && !trans.is_write()) return 0;

	// modify request queues and send the transaction trhough the init_sockets
	unsigned int counter = 0;
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
		trans.set_address(buffer_addr);
		counter += (*init_socket[mapping[*it].output_port])->transport_dbg(trans);
		if (trans.is_read())
			m_req_dispatcher[mapping[*it].output_port]->ReadTransportDbg(id, trans);
		else
			m_req_dispatcher[mapping[*it].output_port]->WriteTransportDbg(id, trans);
	}
	// if the request is a write then be sure to modify the response queue
	if (trans.is_write())
		m_rsp_dispatcher[id]->WriteTransportDbg(id, trans);
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
		trans.set_address(buffer_addr);
		counter += (*init_socket[mapping[*it].output_port])->transport_dbg(trans);
		m_req_dispatcher[mapping[*it].output_port]->ReadTransportDbg(id, trans);	
	}
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
		trans.set_address(buffer_addr);
		counter += (*init_socket[mapping[*it].output_port])->transport_dbg(trans);
		m_req_dispatcher[mapping[*it].output_port]->WriteTransportDbg(id, trans);	
	}
	return counter;
}

template<class CONFIG>
bool
Router<CONFIG>::
T_get_direct_mem_ptr_cb(int id, transaction_type &trans, tlm::tlm_dmi &dmi) {
	/* nothing to do */
	return false;
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
	
	if (VerboseNonBlocking()) {
		logger << DebugInfo << "Sending transaction request through init_socket[" << id << "]" << endl
			<< TIME(time) << endl;
		TRANS(logger, trans);
		logger << EndDebug;
	}
	
	switch((*init_socket[id])->nb_transport_fw(trans, phase, time)) {
		case tlm::TLM_ACCEPTED:
			/* the request has been accepted */
			if (VerboseNonBlocking()) {
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
					if (VerboseNonBlocking()) {
						logger << DebugInfo << "Transaction request sent from init_req_fifo[" << id << "] accepted (TLM_UPDATED, BEGIN_REQ), waiting for the corresponding END_REQ" << endl
							<< TIME(time) << endl;
						TRANS(logger, trans);
						logger << EndDebug;
					}
					/* nothing else to do, just wait for the corresponding END_REQ */
					break;
				case tlm::END_REQ:
					/* if the transaction is a write we should not have received a TLM_UPDATED with END_REQ, but TLM_COMPLETED instead */
					if(trans.is_write()) {
						logger << DebugError << "Received a TLM_UPDATED with phase to END_REQ when sending a write request transaction through init_socket[" << id << "], a TLM_COMPLETED should have been received" << endl
							<< LOCATION << endl
							<< TIME(time) << endl;
						TRANS(logger, trans);
						logger << EndDebug;
						sc_stop();
						wait();
					}
					/* we can remove the request from the queue, so new ones can be sent */
					if (VerboseNonBlocking()) {
						logger << DebugInfo << "Transaction sent from init_req_fifo[" << id << "] accepted (TLM_UPDATED with phase END_REQ), removing the transaction from the request queue" << endl
							<< TIME(time) << endl;
						TRANS(logger, trans);
						logger << EndDebug;
					}
					m_req_dispatcher[id]->Completed(time);
					break;	
				case tlm::BEGIN_RESP:
					/* if the transaction is a write we should not have received a TLM_UPDATED with BEGIN_RESP, but TLM_COMPLETED instead */
					if(trans.is_write()) {
						logger << DebugError << "Received a TLM_UPDATED with phase to BEGIN_RESP when sending a write request transaction through init_socket[" << id << "], a TLM_COMPLETED should have been received" << endl
							<< LOCATION << endl
							<< TIME(time) << endl;
						TRANS(logger, trans);
						logger << EndDebug;
						sc_stop();
						wait();
					}
					/* the request has been accepted, and the response has been produced
					 * check when the response can be queued into the response queue through the handled port (recovering the router extension)
					 * we must send an end response message */
					{
						unsigned int targ_id;
						/* notify to the request dispatcher that the transaction request has been completed */
						m_req_dispatcher[id]->Completed(time);
						if(!GetRouterExtension(trans, targ_id)) {
							logger << DebugError << "Could not find the router extension from transaction response" << endl
								<< LOCATION << endl
								<< TIME(time) << endl;
							ETRANS(logger, trans);
							logger << EndDebug;
							sc_stop();
							wait();
						}
						if (VerboseNonBlocking()) {
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
							sc_stop();
							wait();
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
					sc_stop();
					wait();
					break;
			}
			break;
		case tlm::TLM_COMPLETED:
			/* the init_port has completely handled the transaction request
			 * if the request was a write we can remove the transaction
			 * if the request was a read we have to forward the response to the corresponding targ_port */
			{
				/* notify to the request dispatcher that the transaction request has been completed */
				m_req_dispatcher[id]->Completed(time);
				if(trans.is_write()) {
					if (VerboseNonBlocking()) {
						logger << DebugInfo << "Transaction request send through the init_port[" << id << "] completed (TLM_COMPLETED)" << endl
							<< TIME(time) << endl;
						ETRANS(logger, trans);
						logger << EndDebug;
					}
					/* we can release the transaction */
					trans.release();
				} else {
					/* the transaction is a read */
					unsigned int targ_id;
					if(!GetRouterExtension(trans, targ_id)) {
						logger << DebugError << "Could not find the router extension from transaction response" << endl
							<< LOCATION << endl
							<< TIME(time) << endl;
						ETRANS(logger, trans);
						logger << EndDebug;
						sc_stop();
						wait();
					}
					if (VerboseNonBlocking()) {
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
	
	if (VerboseNonBlocking()) {
		logger << DebugInfo << "Sending transaction response through targ_socket[" << id << "]" << endl
			<< TIME(time) << endl;
		TRANS(logger, trans);
		logger << EndDebug;
	}
	
	switch((*targ_socket[id])->nb_transport_bw(trans, phase, time)) {
		case tlm::TLM_ACCEPTED:
			/* the response has been accepted */
			if (VerboseNonBlocking()) {
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
			if (VerboseNonBlocking()) {
				logger << DebugInfo << "Transaction response sent through targ_socket[" << id << "] completed (TLM_COMPLETED), removing it from the router" << endl
					<< TIME(time) << endl;
				ETRANS(logger, trans);
				logger << EndDebug;
			}
			/* the response has been completed, we can remove it from the response queue */
			m_rsp_dispatcher[id]->Completed(time);
			/* release the transaction */
			trans.release();
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
	
template<class CONFIG>
inline bool
Router<CONFIG>::
ApplyMap(const transaction_type &trans, unsigned int &port) const
{
	bool found = false;
	uint64_t address = trans.get_address();
	unsigned int size = trans.get_data_length();
	for(unsigned int i = 0; !found && i < MAX_NUM_MAPPINGS; i++) {
		if(mapping[i].used) {
			if(address >= mapping[i].range_start && (address + size - 1) <= mapping[i].range_end) {
				found = true;
				port = mapping[i].output_port;
			}
		}
	}

	return found;
}

template<class CONFIG>
inline void
Router<CONFIG>::
ApplyMap(const transaction_type &trans, std::vector<unsigned int> &port_mappings) const
{
	sc_dt::uint64 addr = trans.get_address();
	unsigned int size = trans.get_data_length();
	sc_dt::uint64 cur_addr = addr;
	unsigned int cur_size = size;
	while (cur_addr < addr + size) {
		unsigned int index = 0;
		bool found = false;
		for (; !found && index < MAX_NUM_MAPPINGS; index++) {
			if (mapping[index].used) {
				if (cur_addr >= mapping[index].range_start && cur_addr <= mapping[index].range_end) {
					found = true;
					port_mappings.push_back(index);
					if (cur_size - 1 <= mapping[index].range_end - cur_addr) {
						cur_addr = mapping[index].range_end + 1;
						cur_size = 1 + size - (mapping[index].range_end - addr);
					} else 
						cur_addr = addr + size;
				}
			}
		}
		if (!found) {
			sc_dt::uint64 closest_range_start = 0;
			for (index = 0; index < MAX_NUM_MAPPINGS; index++) {
				if (mapping[index].used) {
					if (cur_addr < mapping[index].range_start && cur_addr + cur_size > mapping[index].range_start) {
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
			cur_size = size - (closest_range_start - cur_addr);
			cur_addr = closest_range_start;
		}
	}
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

template<class CONFIG>
inline bool 
Router<CONFIG>::
VerboseSetup() {
	if(DEBUG && (verbose_all || verbose_setup)) 
		return true;
	return false;
}

template<class CONFIG>
inline bool
Router<CONFIG>::
VerboseNonBlocking() {
	if(DEBUG && (verbose_all || verbose_non_blocking))
		return true;
	return false;
}

template<class CONFIG>
inline bool
Router<CONFIG>::
VerboseBlocking() {
	if(DEBUG && (verbose_all || verbose_blocking))
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
