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
 * Author: Daniel Gracia Perez (daniel.gracia-perez@cea.fr) 
 */

#include "unisim/component/tlm2/interconnect/generic_router/router.hh"
#include "unisim/kernel/service/service.hh"
#include "unisim/kernel/logger/logger.hh"
#include "unisim/kernel/tlm2/tlm.hh"
#include <vector>
#include <map>

#define TRANS(L,X) \
{ \
	(L) << " - trans = " << &(X) << endl \
		<< "   - " << ((X).is_read()?"read":"write") << endl \
		<< "   - address = 0x" << hex << (X).get_address() << dec << endl \
		<< "   - data_length = " << (X).get_data_length(); \
	if((X).is_write()) { \
		(L) << endl << "   - data ="; \
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
	(L) << endl; \
	if((X).get_response_status() == tlm::TLM_OK_RESPONSE) { \
		(L) << "   - data ="; \
		for(unsigned int _trans_i = 0; _trans_i < (X).get_data_length(); _trans_i++) { \
			(L) << " " << hex << (unsigned int)((X).get_data_ptr()[_trans_i]) << dec; \
		} \
	} \
}

using namespace unisim::kernel::logger;

template<bool BLOCKING = false, bool DEBUG = false>
class Initiator : 
public sc_module,
public unisim::kernel::service::Object,
public tlm::tlm_bw_transport_if<> {
private:
	struct InitiatorAccess {
		bool read;
		uint64_t addr;
		unsigned char data[4];
		unsigned int size;
	};

public:
	tlm::tlm_initiator_socket<> init_socket;
	sc_event end_req_event;
	sc_event end_resp_event;
	unisim::kernel::tlm2::PayloadFabric<tlm::tlm_generic_payload> payload_fabric;
	Logger logger;	
	uint64_t base_address;
	unisim::kernel::service::Parameter<uint64_t> param_base_address;
	uint64_t num_accesses;
	unisim::kernel::service::Parameter<uint64_t> param_num_accesses;
	vector<InitiatorAccess> access_vector;

	SC_HAS_PROCESS(Initiator);

	Initiator(const sc_module_name &name, unisim::kernel::service::Object *parent = 0) : 
	sc_module(name),
	unisim::kernel::service::Object(name, parent),
	init_socket("init_socket"),
	base_address(0),
	param_base_address("base_address", this, base_address, "Memory accesses base address"),
	num_accesses(10),
	param_num_accesses("num_accesses", this, num_accesses, "Number of accesses to do"),
	access_vector(),
	logger(*this) {
		SC_THREAD(thread);
		init_socket.bind(*this);
	}

	~Initiator() {
	}

	virtual bool Setup() {
		if(num_accesses == 0) {
			logger << DebugError << "num_accesses should be bigger than 0" << EndDebug;
			return false;
		}
		access_vector.resize(num_accesses * 2);
		for(unsigned int i = 0; i < num_accesses * 2; i++) {
			access_vector[i].size = 4;
			if(i % 2 == 0) {
				access_vector[i].read = false;
				access_vector[i].addr = base_address + (i * 4);
				for(unsigned int j = 0; j < 4; j++) {
					unsigned char data = i % 0xff;
					access_vector[i].data[j] = data;
				}
			} else {
				access_vector[i].read = true;
				access_vector[i].addr = base_address + ((i - 1) * 4);
				for(unsigned int j = 0; j < 4; j++) {
					access_vector[i].data[j] = 0;
				}
			}
		}
		return true;
	}

	virtual void OnDisconnect() {
		cerr << this->GetName() << ": OnDisconnect" << endl;
	}

	void thread() {
		tlm::tlm_generic_payload *trans;

		for(unsigned int i = 0; i < access_vector.size(); i++) {
			trans = payload_fabric.allocate();
			trans->set_address(access_vector[i].addr);
			if(access_vector[i].read) {
				trans->set_read();
			} else {
				trans->set_write();
			}
			trans->set_data_ptr(access_vector[i].data);
			trans->set_data_length(access_vector[i].size);
			sc_time time(1.0, SC_NS);
			if(DEBUG) {
				logger << DebugInfo << "Sending transaction" << endl
					<< " - time = " << sc_time_stamp() + time << endl;
				TRANS(logger, *trans);
//					<< " - trans = " << trans << endl
//					<< "   - address = 0x" << hex << trans->get_address() << dec << endl;
//				logger << "   - data_ptr = " << (void *)trans->get_data_ptr() << endl;
//				logger << "   - size = " << trans->get_data_length() << endl;
//				if(trans->is_read()) {
//					logger << "   - command = read" << endl;
//				} else {
//					logger << "   - command = write" << endl;
//					logger << "   - data =";
//					unsigned char *tdata = trans->get_data_ptr();
//					for(unsigned int i = 0; i < trans->get_data_length(); i++) {
//						logger << " " << hex << (unsigned int)(tdata[i]) << dec;
//					}
//				}
				logger << EndDebug;
			}
			tlm::tlm_phase phase = tlm::BEGIN_REQ;

			if(BLOCKING) {
				init_socket->b_transport(*trans, time);
			} else {
				switch(init_socket->nb_transport_fw(*trans, phase, time)) {
				case tlm::TLM_ACCEPTED:
					if(DEBUG)
						logger << DebugInfo << "Transaction accepted, waiting for the end request event" << endl
							<< " - time = " << sc_time_stamp() + time << endl
							<< " - trans = " << trans << EndDebug;
					wait(end_req_event);
					if(DEBUG)
						logger << DebugInfo << "END_REQ received, waiting for the end response event" << endl
							<< " - time = " << sc_time_stamp() << endl
							<< " - trans = " << trans << EndDebug;
					wait(end_resp_event);
					if(DEBUG)
						logger << DebugInfo << "END_RESP received" << endl
							<< " - time = " << sc_time_stamp() << endl
							<< " - trans = " << trans << EndDebug;
					break;
				case tlm::TLM_UPDATED:
					break;
				case tlm::TLM_COMPLETED:
					break;
				}
				if(DEBUG)
					logger << DebugInfo << "Transaction finished, releasing transaction" << endl
						<< " - time = " << sc_time_stamp() << endl
						<< " - trans = " << trans << EndDebug;
				trans->release();
			}
			if(trans->is_read()) {
				if(DEBUG) {
					logger << DebugInfo << "Transaction answer received" << endl
						<< " - time = " << sc_time_stamp() + time << endl;
					ETRANS(logger, *trans);
					logger << EndDebug;
				}
			}
			wait(time);
		}
	}

	virtual tlm::tlm_sync_enum nb_transport_bw(tlm::tlm_generic_payload &trans, tlm::tlm_phase &phase, sc_core::sc_time &t) {
		switch(phase) {
		case tlm::BEGIN_REQ:
			logger << DebugError << "Received unexpected BEGIN_REQ phase, returning TLM_ACCEPTED" << endl
				<< " - time = " << sc_time_stamp() + t << endl
				<< " - trans = " << &trans << EndDebug;
			sc_stop();
			wait();
			break;
		case tlm::END_REQ:
			if(DEBUG)
				logger << DebugInfo << "Received END_REQ, notifying end request event" << endl
					<< " - time = " << sc_time_stamp() + t << endl
					<< " - trans = " << &trans << EndDebug;
			end_req_event.notify(t);
			return tlm::TLM_ACCEPTED;
			break;
		case tlm::BEGIN_RESP:
			if(DEBUG)
				logger << DebugInfo << "Received END_RESP, notifying end response event" << endl
					<< " - time = " << sc_time_stamp() + t << endl
					<< " - trans = " << &trans << EndDebug;
			end_resp_event.notify(t);
			return tlm::TLM_COMPLETED;
			break;
		case tlm::END_RESP:
			logger << DebugError << "Received unexpected END_RESP phase, returning TLM_ACCEPTED" << endl
				<< " - time = " << sc_time_stamp() + t << endl
				<< " - trans = " << &trans << EndDebug;
			sc_stop();
			wait();
			break;
		}
		return tlm::TLM_ACCEPTED; // Dummy implementation
	}

	virtual void invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range) {
		// Dummy implementation
	}

};

template<bool DEBUG = false>
class Target :
public sc_module,
public unisim::kernel::service::Object,
public tlm::tlm_fw_transport_if<> {
private:
	map<uint64_t, unsigned char> mem;

public:
	tlm::tlm_target_socket<> targ_socket;
	Logger logger;

	SC_HAS_PROCESS(Target);

	Target(const sc_module_name &name, unisim::kernel::service::Object *parent = 0) :
	sc_module(name),
	unisim::kernel::service::Object(name, parent),
	targ_socket("targ_socket"),
	mem(),
	logger(*this) {
		targ_socket.bind(*this);
	}

	~Target() {
	}

	virtual bool Setup() {
		return true;
	}

	virtual void OnDisconnect() {
		cerr << this->GetName() << ": OnDisconnect" << endl;
	}

	virtual tlm::tlm_sync_enum nb_transport_fw(tlm::tlm_generic_payload &trans, tlm::tlm_phase &phase, sc_core::sc_time &t) {
		sc_time delay(10, SC_NS);
		switch(phase) {
		case tlm::BEGIN_REQ:
			if(DEBUG) {
				logger << DebugInfo << "Received BEGIN_REQ phase, returning TLM_COMPLETED with delay of " << delay << endl
					<< " - time = " << sc_time_stamp() + t << endl
					<< " - trans = " << &trans << EndDebug;
			}
			t = t + delay;
			return tlm::TLM_COMPLETED;
			break;
		case tlm::END_REQ:
			logger << DebugError << "Received unexpected END_REQ phase" << endl
				<< " - time = " << sc_time_stamp() + t << endl
				<< " - trans = " << &trans << EndDebug;
			sc_stop();
			wait();
			break;
		case tlm::BEGIN_RESP:
			logger << DebugError << "Received unexpected BEGIN_RESP phase" << endl
				<< " - time = " << sc_time_stamp() + t << endl
				<< " - trans = " << &trans << EndDebug;
			sc_stop();
			wait();
			break;
		case tlm::END_RESP:
			if(DEBUG)
				logger << DebugInfo << "Received END_RESP phase, returning TLM_COMPLETED" << endl
					<< " - time = " << sc_time_stamp() + t << endl
					<< " - trans = " << &trans << EndDebug;
			return tlm::TLM_COMPLETED;
			break;
		}
		return tlm::TLM_COMPLETED;
	}

	virtual void b_transport(tlm::tlm_generic_payload &trans, sc_time &delay) {
		sc_time my_delay(1.0, SC_NS);
		if(DEBUG) {
			logger << DebugInfo << "Received blocking transaction, returning with a delay of " << my_delay << endl
				<< " - time = " << sc_time_stamp() + delay << endl;
			TRANS(logger, trans);
			logger << EndDebug;
		}
		unsigned char *data = trans.get_data_ptr();
		unsigned int size = trans.get_data_length();
		uint64_t addr = trans.get_address();
		map<uint64_t, unsigned char>::iterator it;
		if(trans.is_read()) {
			for(unsigned int i = 0; i < size; i++) {
				it = mem.find(addr + i);
				if(it != mem.end())
					data[i] = it->second;
				else
					data[i] = 0;
			}
		} else {
			for(unsigned int i = 0; i < size; i++) {
				mem[addr + i] = data[i];
			}
		}
		trans.set_response_status(tlm::TLM_OK_RESPONSE);
		delay += my_delay;
		logger << DebugInfo << "Responding to blocking transaction:" << endl
			<< " - time = " << sc_time_stamp() + delay << endl;
		ETRANS(logger, trans);
		logger << EndDebug;
	}

	virtual bool get_direct_mem_ptr(tlm::tlm_generic_payload &trans, tlm::tlm_dmi &dmi_data) {
		return false; // Dummy implementation
	}

	virtual unsigned int transport_dbg(tlm::tlm_generic_payload &trans) {
		return 0; // Dummy implementation
	}
};

template<bool DEBUG = false>
class Top :
public sc_module,
public unisim::kernel::service::Object {
public:
	Initiator<true, DEBUG> *init;
	Initiator<true, DEBUG> *init2;
	Target<DEBUG> *targ;
	Target<DEBUG> *targ2;
	unisim::component::tlm2::interconnect::generic_router::Router<unisim::component::tlm2::interconnect::generic_router::DebugConfig> *drouter;
	unisim::component::tlm2::interconnect::generic_router::Router<> *router;

	SC_HAS_PROCESS(Top);

	Top(const sc_module_name &name, unisim::kernel::service::Object *parent = 0) :
	sc_module(name),
	unisim::kernel::service::Object(name, parent) {
		init = new Initiator<true, DEBUG>("init", this);
		init2 = new Initiator<true, DEBUG>("init2", this);
		targ = new Target<DEBUG>("targ", this);
		targ2 = new Target<DEBUG>("targ2", this);
		drouter = 0;
		router = 0;
		if(DEBUG)
			drouter = new unisim::component::tlm2::interconnect::generic_router::Router<unisim::component::tlm2::interconnect::generic_router::DebugConfig>("router", this);
		else
			router = new unisim::component::tlm2::interconnect::generic_router::Router<>("router", this);

		if(DEBUG) {
			init->init_socket(drouter->targ_socket);
			init2->init_socket(drouter->targ_socket);
			drouter->init_socket(targ->targ_socket);
			drouter->init_socket(targ2->targ_socket);
		} else {
			init->init_socket(router->targ_socket);
			init2->init_socket(router->targ_socket);
			router->init_socket(targ->targ_socket);
			router->init_socket(targ2->targ_socket);
		}
	}

	~Top() {
		delete init;
		delete init2;
		delete targ;
		if(router) delete router;
		if(drouter) delete drouter;
	}
	
	bool Setup() {
		return true;
	}
};


