#ifndef __LTFIBONACCI_HH__
#define __LTFIBONACCI_HH__

#include <systemc.h>
#include <tlm.h>
#include <inttypes.h>

//#define __QUANTUM

class LTFibonacci :
	public sc_core::sc_module,
	public virtual tlm::tlm_bw_nb_transport_if<> {
private:
	sc_core::sc_event event;
#ifdef __QUANTUM
	tlm::tlm_quantumkeeper m_qk;
#endif

public:
	tlm::tlm_nb_initiator_socket<> socket;

public:
	SC_HAS_PROCESS(LTFibonacci);
	LTFibonacci(sc_core::sc_module_name name) :
		sc_core::sc_module(name),
		socket("socket"),
		event() {
			// bind the interfaceto the target socket
			socket(*this);

			// the initiator thread
			SC_THREAD(run);
		}

#ifdef __QUANTUM
	void inc_quantum(const sc_time &t) {
		cerr << name() << " :: incrementing quantum " << t << endl;
		m_qk.inc(t);
		if(m_qk.need_sync()) m_qk.sync();
	}
#endif

	void write(uint64_t address, uint64_t val) {
		/* SystemC code to send a write request to the memory */
		tlm::tlm_generic_payload t;
		tlm::tlm_phase phase = tlm::BEGIN_REQ;
		sc_time delay;

		cerr << name() << " :: write" << endl;
		t.set_address(address);
		t.set_data_ptr(reinterpret_cast<unsigned char *>(&val));
		t.set_data_length(8);
		t.set_write();

#ifdef __QUANTUM
		inc_quantum(sc_time(10, SC_NS));
#else
		wait(10, SC_NS);
#endif

#ifdef __QUANTUM
		switch(socket->nb_transport(t, phase, m_qk.get_local_time())) {
#else
		delay = SC_ZERO_TIME;
		switch(socket->nb_transport(t, phase, delay)) {
#endif
			case tlm::TLM_REJECTED:
			cerr << name() << " :: Received rejected response status" << endl;
			sc_stop();
			wait();
			break;
		case tlm::TLM_UPDATED:
			cerr << name() << " :: Received updated response status" << endl;
			sc_stop();
			wait();
			break;
		case tlm::TLM_COMPLETED:
			if(phase != tlm::BEGIN_RESP) {
				cerr << name() << " :: Received completed response status, but not BEGIN_RESP phase" << endl;
				sc_stop();
				wait();
			}
#ifdef __QUANTUM
			if(m_qk.need_sync()) m_qk.sync();
#else
			wait(delay);
#endif
			break;
		case tlm::TLM_ACCEPTED:
			cerr << name() << " :: Received accepted response status for a write request" << endl;
			sc_stop();
			wait();
			break;
		}
	}

	uint64_t read(uint64_t address) {
		/* SystemC code to send a read request to the memory */
		uint64_t read_value;
		tlm::tlm_generic_payload t;
		tlm::tlm_phase phase = tlm::BEGIN_REQ;
		sc_time delay;

		cerr << name() << " :: read" << endl;

		t.set_address(address);
		t.set_data_ptr(reinterpret_cast<unsigned char *>(&read_value));
		t.set_data_length(8);
		t.set_read();

#ifdef __QUANTUM
		inc_quantum(sc_time(10, SC_NS));
#else
		wait(10, SC_NS);
#endif

#ifdef __QUANTUM
		cerr << name() << " :: (before transport) current time = " << m_qk.get_current_time() << endl;
		switch(socket->nb_transport(t, phase, m_qk.get_local_time())) {
#else
		delay = SC_ZERO_TIME;
		switch(socket->nb_transport(t, phase, delay)) {
#endif
		case tlm::TLM_REJECTED:
			cerr << name() << " :: Received rejected response status" << endl;
			sc_stop();
			wait();
			break;
		case tlm::TLM_UPDATED:
			cerr << name() << " :: Received updated response status" << endl;
			sc_stop();
			wait();
			break;
		case tlm::TLM_COMPLETED:
			if(phase != tlm::BEGIN_RESP) {
				cerr << name() << " :: Received completed response status, but not BEGIN_RESP phase" << endl;
				sc_stop();
				wait();
			}
			cerr << name() << " :: Request completed" << endl;
			// we have received the response, take the time to resynchronize
			//   the system
			// if we are using the quantum keeper just increment it and 
			//   check if a sync is needed
#ifdef __QUANTUM
			if(m_qk.need_sync()) m_qk.sync();
#else
			wait(delay);
#endif
			break;
		case tlm::TLM_ACCEPTED:
			cerr << name() << " :: Request accepted (" << delay << ")" << endl;
			// we are forced to wait for the response
			//   but first we should sync our clock
			wait(event);
#ifdef __QUANTUM
			cerr << name() << " :: (after transport) current time = " << m_qk.get_current_time() << endl;
//			if(m_qk.need_sync()) m_qk.sync();
			m_qk.reset();
			cerr << name() << " :: (after reset) current time = " << m_qk.get_current_time() << endl;
#endif
			break;
		}

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
#ifdef __QUANTUM
				m_qk.sync();
#endif
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

	virtual tlm::tlm_sync_enum nb_transport(
			tlm::tlm_generic_payload& trans,
			tlm::tlm_phase& pahse,
			sc_core::sc_time& t) {
		event.notify(t);
		return tlm::TLM_COMPLETED;   // Dummy implementation
	}
	
	virtual void invalidate_direct_mem_ptr(sc_dt::uint64 start_range,
			sc_dt::uint64 end_range)
	{
		// No DMI support: ignore
	}

};

#endif // __LTFIBONACCI_HH__
