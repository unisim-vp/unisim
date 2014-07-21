#ifndef __UTMEMORY_HH__
#define __UTMEMORY_HH__

#include <systemc.h>
#include <tlm.h>
#include <map>

class UTMemory :
	public sc_core::sc_module,
	public virtual tlm::tlm_fw_b_transport_if<> {

public:
	tlm::tlm_b_target_socket<> socket;

	SC_HAS_PROCESS(UTMemory);
	UTMemory(sc_core::sc_module_name name,
			uint64_t val) :
		sc_core::sc_module(name),
		socket("socket"),
		mem(){
			// bind the interfaceto the target socket
			socket(*this);
			mem[0] = val;
			cerr << name << " :: mem[0] = " << mem[0] << endl;
		}
	
	void b_transport(tlm::tlm_generic_payload &payload) {
		sc_dt::uint64 addr = payload.get_address();

		uint64_t &data = *reinterpret_cast<uint64_t *>(payload.get_data_ptr());
		if(payload.is_read()) {
			data = mem[addr];
			cerr << name() << " :: return " << data << " (mem[" << addr << "])" << endl;
		} else {
			// we suppose it is a write operation
			mem[addr] = data;
			cerr << name() << " :: mem[" << addr << "] = " << data << endl;
		}
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

#endif // __UTMEMORY_HH__
