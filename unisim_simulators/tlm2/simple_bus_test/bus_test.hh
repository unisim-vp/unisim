#include "unisim/component/tlm2/simple_bus/bus.hh"
#include "unisim/kernel/service/service.hh"
//#include "unisim/component/tlm2/simple_bus/bus.tcc"

class Initiator : 
public sc_module,
public unisim::kernel::service::Object,
public tlm::tlm_bw_transport_if<> {
public:
	tlm::tlm_initiator_socket<> init_socket;

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
		tlm::tlm_generic_payload trans;
		unsigned int i = 0;

		while(i < 10) {
			sc_time delay(1.0, SC_NS);
			cerr << GetName() << "(" << sc_time_stamp() + delay << "): Sending data" << endl;
			init_socket->b_transport(trans, delay);
			wait(delay);
			cerr << GetName() << "(" << sc_time_stamp() << "): Response received" << endl;
			i++;
		}
	}

	virtual tlm::tlm_sync_enum nb_transport_bw(tlm::tlm_generic_payload &trans, tlm::tlm_phase &phase, sc_core::sc_time &t) {
		return tlm::TLM_COMPLETED; // Dummy implementation
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
	unisim::component::tlm2::bus::BusController<> *bus;

	SC_HAS_PROCESS(Top1);

	Top1(const sc_module_name &name, unisim::kernel::service::Object *parent = 0) :
	sc_module(name),
	unisim::kernel::service::Object(name, parent) {
		init = new Initiator("init", this);
		init2 = new Initiator("init2", this);
		targ = new Target("targ", this);
		bus = new unisim::component::tlm2::bus::BusController<>("bus", this);

		init->init_socket(bus->targ_socket);
		init2->init_socket(bus->targ_socket);
		bus->init_socket(targ->targ_socket);
	}

	bool Setup() {
		return true;
	}
};

