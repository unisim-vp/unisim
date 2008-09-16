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

#include "unisim/component/tlm2/interconnect/generic_bus/bus.hh"
#include "unisim/kernel/service/service.hh"
#include "unisim/kernel/logger/logger.hh"
#include "unisim/kernel/tlm2/tlm.hh"


using namespace unisim::kernel::logger;

template<bool DEBUG = false>
class Initiator : 
public sc_module,
public unisim::kernel::service::Object,
public tlm::tlm_bw_transport_if<> {
public:
	tlm::tlm_initiator_socket<> init_socket;
	sc_event end_req_event;
	sc_event end_resp_event;
	unisim::kernel::tlm2::PayloadFabric<tlm::tlm_generic_payload> payload_fabric;
	Logger logger;	

	SC_HAS_PROCESS(Initiator);

	Initiator(const sc_module_name &name, unisim::kernel::service::Object *parent = 0) : 
	sc_module(name),
	unisim::kernel::service::Object(name, parent),
	init_socket("init_socket"),
	logger(*this) {
		SC_THREAD(thread);
		init_socket.bind(*this);
	}

	~Initiator() {
	}

	virtual bool Setup() {
		return true;
	}

	virtual void OnDisconnect() {
		cerr << this->GetName() << ": OnDisconnect" << endl;
	}

	void thread() {
		tlm::tlm_generic_payload *trans;
		unsigned int i = 0;

		while(i < 10) {
			trans = payload_fabric.allocate();
			sc_time time(1.0, SC_NS);
			if(DEBUG)
				logger << DebugInfo << "Sending transaction" << endl
					<< " - time = " << sc_time_stamp() + time << endl
					<< " - trans = " << trans << EndDebug;
			tlm::tlm_phase phase = tlm::BEGIN_REQ;

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
			i++;
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
public:
	tlm::tlm_target_socket<> targ_socket;
	Logger logger;

	SC_HAS_PROCESS(Target);

	Target(const sc_module_name &name, unisim::kernel::service::Object *parent = 0) :
	sc_module(name),
	unisim::kernel::service::Object(name, parent),
	targ_socket("targ_socket"),
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
		if(DEBUG)
			logger << DebugInfo << "Received blocking transaction, returning with a delay of " << my_delay << endl
				<< " - time = " << sc_time_stamp() + delay << endl
				<< " - trans = " << &trans << EndDebug;
		delay += my_delay;
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
	Initiator<DEBUG> *init;
	Initiator<DEBUG> *init2;
	Target<DEBUG> *targ;
	unisim::component::tlm2::interconnect::generic_bus::Bus<32, tlm::tlm_base_protocol_types, DEBUG> *bus;

	SC_HAS_PROCESS(Top);

	Top(const sc_module_name &name, unisim::kernel::service::Object *parent = 0) :
	sc_module(name),
	unisim::kernel::service::Object(name, parent) {
		init = new Initiator<DEBUG>("init", this);
		init2 = new Initiator<DEBUG>("init2", this);
		targ = new Target<DEBUG>("targ", this);
		bus = new unisim::component::tlm2::interconnect::generic_bus::Bus<32, tlm::tlm_base_protocol_types, DEBUG>("bus", this);

		init->init_socket(bus->targ_socket);
		init2->init_socket(bus->targ_socket);
		bus->init_socket(targ->targ_socket);
	}

	~Top() {
		delete init;
		delete init2;
		delete targ;
		delete bus;
	}
	
	bool Setup() {
		return true;
	}
};

