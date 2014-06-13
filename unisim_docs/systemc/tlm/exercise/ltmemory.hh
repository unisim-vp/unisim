#ifndef __LTMEMORY_HH__
#define __LTMEMORY_HH__

#include <systemc.h>
#include <tlm.h>
#include <map>

class LTMemory :
	public sc_core::sc_module,
	public virtual tlm::tlm_fw_nb_transport_if<> {

public:
	tlm::tlm_nb_target_socket<> socket;
	sc_core::sc_event event;
	tlm::tlm_generic_payload *req;
	bool sync;

	SC_HAS_PROCESS(LTMemory);
	LTMemory(sc_core::sc_module_name name,
			uint64_t val) :
		sc_core::sc_module(name),
		socket("socket"),
		event(),
		mem(),
		sync(false),
		req(0){
			// bind the interfaceto the target socket
			socket(*this);
			mem[0] = val;
			cerr << name << " :: mem[0] = " << mem[0] << endl;
			SC_THREAD(resp_thread);
		}
	
	void resp_thread() {
		while(1) {
			wait(event);
			cerr << name() << " resp_thread" << endl;

			if(req == 0) {
				cerr << name() << "No request found in resp_thread" << endl;
				sc_stop();
				wait();
			}

			if(!req->is_read()) {
				cerr << name() << "resp_thread: request is not a read" << endl;
				sc_stop();
				wait();
			}

			tlm::tlm_phase phase = tlm::BEGIN_RESP;
			sc_core::sc_time delay = SC_ZERO_TIME;
			sc_dt::uint64 addr = req->get_address();
			uint64_t &data = *reinterpret_cast<uint64_t *>(req->get_data_ptr());
			data = mem[addr];
			cerr << name() << " :: return " << data << " (mem[" << addr << "])" << endl;
			switch(socket->nb_transport(*req, phase, delay)) {
				case tlm::TLM_REJECTED:
					cerr << name() << "Received a reject status response" << endl;
					sc_stop();
					wait();
					break;
				case tlm::TLM_UPDATED:
					cerr << name() << "Received a updated status response" << endl;
					sc_stop();
					wait();
					break;
				case tlm::TLM_ACCEPTED:
					cerr << name() << "Received a accepted status response" << endl;
					sc_stop();
					wait();
					break;
				case tlm::TLM_COMPLETED:
					req = 0;
					break;
			}
		}
	}

	virtual tlm::tlm_sync_enum nb_transport(tlm::tlm_generic_payload &payload,
			tlm::tlm_phase &phase,
			sc_core::sc_time &t) {
		cerr << name() << " :: nb_transport" << endl;

		if(payload.is_read()) {
			sync = !sync;
			if(sync) {
				req = &payload;
				cerr << name() << " :: received a read" << endl;
				event.notify(t + sc_time(20, SC_NS));
				return tlm::TLM_ACCEPTED;
			} else {
				sc_dt::uint64 addr = payload.get_address();
				uint64_t &data = *reinterpret_cast<uint64_t *>(payload.get_data_ptr());

				data = mem[addr];
				cerr << name() << " :: return " << data << "(mem[" << addr << "])" << endl;
				t += sc_time(20, SC_NS);
				phase = tlm::BEGIN_RESP;
				return tlm::TLM_COMPLETED;
			}
		} else {
			if(payload.is_write()) {
				sc_dt::uint64 addr = payload.get_address();
				uint64_t &data = *reinterpret_cast<uint64_t *>(payload.get_data_ptr());
				mem[addr] = data;
				cerr << name() << " :: mem[" << addr << "] = " << data << endl;
				phase = tlm::BEGIN_RESP;
				return tlm::TLM_COMPLETED;
			}
		}
		// it is a ignore, do nothing
		return tlm::TLM_COMPLETED;
	}

	unsigned int transport_dbg(tlm::tlm_debug_payload &payload) {
		// not supported
		return 0;
	}

	bool get_direct_mem_ptr(const sc_dt::uint64 &address,
			tlm::tlm_dmi_mode &dmi_mode,
			tlm::tlm_dmi &dmi_data) {
		// not supported
		return false;
	}

	uint64_t read(uint64_t addr) {
		return mem[addr];
	}

private:
	std::map<sc_dt::uint64, uint64_t> mem;
};

#endif // __LTMEMORY_HH__
