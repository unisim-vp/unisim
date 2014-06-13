#ifndef __UTFIBONACCI_HH__
#define __UTFIBONACCI_HH__

#include <systemc.h>
#include <tlm.h>
#include <inttypes.h>

class UTFibonacci :
	public sc_core::sc_module,
	public virtual tlm::tlm_bw_b_transport_if {

public:
	tlm::tlm_b_initiator_socket<> socket;

public:
	SC_HAS_PROCESS(UTFibonacci);
	UTFibonacci(sc_core::sc_module_name name) :
		sc_core::sc_module(name),
		socket("socket") {
			// bind the interfaceto the target socket
			socket(*this);

			// the initiator thread
			SC_THREAD(run);
		}
	
	void write(uint64_t address, uint64_t val) {
		/* SystemC code to send a write request to the memory */
		tlm::tlm_generic_payload t;

		t.set_address(address);
		t.set_data_ptr(reinterpret_cast<unsigned char *>(&val));
		t.set_data_length(8);
		t.set_write();

		socket->b_transport(t);
	}

	uint64_t read(uint64_t address) {
		/* SystemC code to send a read request to the memory */
		uint64_t read_value;
		tlm::tlm_generic_payload t;

		t.set_address(address);
		t.set_data_ptr(reinterpret_cast<unsigned char *>(&read_value));
		t.set_data_length(8);
		t.set_read();

		socket->b_transport(t);

		return read_value;
	}

	void run() {
		uint64_t init_addr = 0;
		uint64_t i_addr = 1;
		uint64_t a_addr = 2;
		uint64_t b_addr = 3;
		uint64_t a_prev_addr = 4;
		uint64_t res_addr = 5;

		if(read(init_addr) == 0) {
			cerr << "fib :: mem[init_addr] = 0" << endl;
			write(res_addr, 0);
			sc_core::sc_stop();
			wait();
		}
		if(read(init_addr) <= 2) {
			cerr << "fib :: mem[init_addr] == " << read(init_addr) << endl;
			write(res_addr, 1);
			sc_core::sc_stop();
			wait();
		}
		write(a_addr, 1);
		write(b_addr, 1);
		write(i_addr, 3);
		while(1) {
			cerr << "fib :: loop (i == " << read(i_addr) << ", init == " << read(init_addr) << ")" << endl;
			if(read(i_addr) > read(init_addr)) {
				cerr << "fib :: finishing" << endl;
				write(res_addr, read(b_addr));
				sc_core::sc_stop();
				wait();
			}
			cerr << "fib :: inc (a == " << read(a_addr) << ", b == " << read(b_addr) << ")" << endl;
			uint64_t a_prev = read(a_addr);
			uint64_t b = read(b_addr);
			write(a_addr, b);
			write(b_addr, a_prev + b);
			write(i_addr, read(i_addr) + 1);
		}
	}

	void invalidate_direct_mem_ptr(sc_dt::uint64 start_range,
			sc_dt::uint64 end_range)
	{
		// No DMI support: ignore
	}

};

#endif // __UTFIBONACCI_HH__
