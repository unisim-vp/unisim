#include <iostream>
#include <systemc.h>
#include <tlm.h>
#include "ltfibonacci.hh"
#include "ltmemory.hh"

using namespace std;

int sc_main(int argc, char *argv[]) {
	uint64_t val = 10;

	tlm::tlm_quantumkeeper().set_global_quantum(sc_time(40, SC_NS));

	LTMemory mem("Memory", val);
	LTFibonacci fib("Fibonacci");

	fib.socket(mem.socket);
	cerr << "time = " << sc_time_stamp() << endl;
	sc_start();
	cerr << "fib(" << mem.read(0) << ") = " << mem.read(5) << endl;
	cerr << "time = " << sc_time_stamp() << endl;
	cerr << "bye" << endl;
}
