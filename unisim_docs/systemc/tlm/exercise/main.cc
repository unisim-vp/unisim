#include <iostream>
#include <systemc.h>
#include <tlm.h>
#include "utfibonacci.hh"
#include "utmemory.hh"

using namespace std;

int sc_main(int argc, char *argv[]) {
	uint64_t val = 10;

	UTMemory mem("Memory", val);
	UTFibonacci fib("Fibonacci");

	fib.socket(mem.socket);
	cerr << "time = " << sc_time_stamp() << endl;
	sc_start();
	cerr << "fib(" << mem.read(0) << ") = " << mem.read(5) << endl;
	cerr << "time = " << sc_time_stamp() << endl;
	cerr << "bye" << endl;
}
