all: utfib router ltfib ltfib_quantum

utfib: main.cc utfibonacci.hh utmemory.hh
	g++ main.cc -o utfib -I/local/home/gracia/opt/systemc-2.2.0/include -I/local/home/gracia/svn/SystemC/TLM-2007-11-29/include -L/local/home/gracia/opt/systemc-2.2.0/lib-linux -lsystemc

utrouter: main_router.cc utfibonacci.hh utmemory.hh utrouter.hh
	g++ main_router.cc -o utrouter -I/local/home/gracia/opt/systemc-2.2.0/include -I/local/home/gracia/svn/SystemC/TLM-2007-11-29/include -L/local/home/gracia/opt/systemc-2.2.0/lib-linux -lsystemc

ltfib: ltmain.cc ltfibonacci.hh ltmemory.hh
	g++ ltmain.cc -o ltfib -I/local/home/gracia/opt/systemc-2.2.0/include -I/local/home/gracia/svn/SystemC/TLM-2007-11-29/include -L/local/home/gracia/opt/systemc-2.2.0/lib-linux -lsystemc

ltfib_quantum: ltmain.cc ltfibonacci.hh ltmemory.hh
	g++ ltmain.cc -o ltfib_quantum -D__QUANTUM -I/local/home/gracia/opt/systemc-2.2.0/include -I/local/home/gracia/svn/SystemC/TLM-2007-11-29/include -L/local/home/gracia/opt/systemc-2.2.0/lib-linux -lsystemc

clean:
	rm -rf utfib utrouter ltfib *.o

