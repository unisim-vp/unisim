#include <iostream>
#include <systemc.h>
#include <tlm.h>
#include "utfibonacci.hh"
#include "utmemory.hh"
#include "utrouter.hh"

using namespace std;

int sc_main(int argc, char *argv[]) {
	uint64_t val = 10;

	UTMemory mem1("Memory1", val);
	UTMemory mem2("Memory2", val);
	UTFibonacci fib("Fibonacci");
	UTRouter router("Router");

	fib.socket(router.in_socket);
	router.out_socket1(mem1.socket);
	router.out_socket2(mem2.socket);
	cerr << "time = " << sc_time_stamp() << endl;
	sc_start(0);
	cerr << "fib(" << mem1.read(0) << ") = " << mem1.read(5) << endl;
	cerr << "fib(" << mem2.read(0) << ") = " << mem2.read(5) << endl;
	cerr << "time = " << sc_time_stamp() << endl;
	cerr << "bye" << endl;
}
