#include "unisim/component/tlm2/bus/generic_bus/bus.hh"
#include "unisim/kernel/service/service.hh"
#include "unisim/kernel/tlm2/tlm.hh"

class Initiator : 
public sc_module,
public unisim::kernel::service::Object,
public tlm::tlm_bw_transport_if<> {
public:
	tlm::tlm_initiator_socket<> init_socket;
	sc_event end_req_event;
	sc_event end_resp_event;
	unisim::kernel::tlm2::PayloadFabric<tlm::tlm_generic_payload> payload_fabric;

	SC_HAS_PROCESS(Initiator);

	Initiator(const sc_module_name &name, unisim::kernel::service::Object *parent = 0) : 
	sc_module(name),
	unisim::kernel::service::Object(name, parent),
	init_socket("init_socket") {
		SC_THREAD(thread);
//		dont_initialize();
		init_socket.bind(*this);
	}

	void thread() {
		tlm::tlm_generic_payload *trans;
		unsigned int i = 0;

		while(i < 10) {
			trans = payload_fabric.allocate();
			sc_time time(1.0, SC_NS);
			cerr << GetName() << "(" << sc_time_stamp() + time << "): Sending transaction " << trans << endl;
//			init_socket->b_transport(trans, delay);
			tlm::tlm_phase phase = tlm::BEGIN_REQ;

			switch(init_socket->nb_transport_fw(*trans, phase, time)) {
			case tlm::TLM_ACCEPTED:
				cerr << GetName() << "(" << sc_time_stamp() + time << "): transaction accepted, waiting for END_REQ or BEGIN_RESP (for transaction " << trans << ")" << endl;
				wait(end_req_event);
				cerr << GetName() << "(" << sc_time_stamp() << "): END_REQ received (for transaction " << trans << ")"  << endl;
				wait(end_resp_event);
				break;
			case tlm::TLM_UPDATED:
				break;
			case tlm::TLM_COMPLETED:
				break;
			}
			cerr << GetName() << "(" << sc_time_stamp() << "): Response received (for transaction " << trans << ")" << endl;
			trans->release();
			i++;
		}
	}

	virtual tlm::tlm_sync_enum nb_transport_bw(tlm::tlm_generic_payload &trans, tlm::tlm_phase &phase, sc_core::sc_time &t) {
		switch(phase) {
		case tlm::BEGIN_REQ:
			cerr << GetName() << "(" << sc_time_stamp() + t << "): received unexpected BEGIN_REQ" << endl;
			sc_stop();
			wait();
			break;
		case tlm::END_REQ:
			cerr << GetName() << "(" << sc_time_stamp() + t << "): received END_REQ (for transaction " << &trans << ")" << endl;
			end_req_event.notify(t);
			return tlm::TLM_ACCEPTED;
			break;
		case tlm::BEGIN_RESP:
			cerr << GetName() << "(" << sc_time_stamp() + t << "): received BEGIN_RESP (for transaction " << &trans << ")" << endl;
			end_resp_event.notify(t);
			return tlm::TLM_COMPLETED;
			break;
		case tlm::END_RESP:
			cerr << GetName() << "(" << sc_time_stamp() + t << "): received unexpected END_RESP" << endl;
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

class Target :
public sc_module,
public unisim::kernel::service::Object,
public tlm::tlm_fw_transport_if<> {
public:
	tlm::tlm_target_socket<> targ_socket;

	SC_HAS_PROCESS(Target);

	Target(const sc_module_name &name, unisim::kernel::service::Object *parent = 0) :
	sc_module(name),
	unisim::kernel::service::Object(name, parent),
	targ_socket("targ_socket") {
		targ_socket.bind(*this);
	}

	virtual tlm::tlm_sync_enum nb_transport_fw(tlm::tlm_generic_payload &trans, tlm::tlm_phase &phase, sc_core::sc_time &t) {
		switch(phase) {
		case tlm::BEGIN_REQ:
			cerr << GetName() << "(" << sc_time_stamp() + t << "): received BEGIN_REQ" << endl;
			t = t + sc_time(10,SC_NS);
			return tlm::TLM_COMPLETED;
			break;
		case tlm::END_REQ:
			cerr << GetName() << "(" << sc_time_stamp() + t << "): received unexpected END_REQ" << endl;
			sc_stop();
			wait();
			break;
		case tlm::BEGIN_RESP:
			cerr << GetName() << "(" << sc_time_stamp() + t << "): received unexpected BEGIN_RESP" << endl;
			sc_stop();
			wait();
			break;
		case tlm::END_RESP:
			cerr << GetName() << "(" << sc_time_stamp() + t << "): received END_RESP" << endl;
			return tlm::TLM_COMPLETED;
			break;
		}
		return tlm::TLM_COMPLETED;
	}

	virtual void b_transport(tlm::tlm_generic_payload &trans, sc_time &delay) {
		cerr << GetName() << "(" << sc_time_stamp() + delay << "): Request received" << endl;
		delay += sc_time(1.0, SC_NS);
	}

	virtual bool get_direct_mem_ptr(tlm::tlm_generic_payload &trans, tlm::tlm_dmi &dmi_data) {
		return false; // Dummy implementation
	}

	virtual unsigned int transport_dbg(tlm::tlm_generic_payload &trans) {
		return 0; // Dummy implementation
	}
};


class Top1 :
public sc_module,
public unisim::kernel::service::Object {
public:
	Initiator *init;
	Initiator *init2;
	Target *targ;
	unisim::component::tlm2::bus::generic_bus::Bus<> *bus;

	SC_HAS_PROCESS(Top1);

	Top1(const sc_module_name &name, unisim::kernel::service::Object *parent = 0) :
	sc_module(name),
	unisim::kernel::service::Object(name, parent) {
		init = new Initiator("init", this);
		init2 = new Initiator("init2", this);
		targ = new Target("targ", this);
		bus = new unisim::component::tlm2::bus::generic_bus::Bus<>("bus", this);

		init->init_socket(bus->targ_socket);
		init2->init_socket(bus->targ_socket);
		bus->init_socket(targ->targ_socket);
	}

	bool Setup() {
		return true;
	}
};

