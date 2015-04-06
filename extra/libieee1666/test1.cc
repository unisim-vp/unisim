#include <systemc.h>
#include <iostream>

class Mod : public sc_module
{
public:
	Mod(const sc_module_name& name)
		: t(10.0, SC_MS)
	{
		SC_HAS_PROCESS(Mod);
		
		SC_THREAD(Tic);
		SC_THREAD(Tac);
	}
private:
	sc_event e1;
	sc_event e2;
	sc_time t;
	
	void Tic()
	{
		std::cout << "Tic: Hello world !" << std::endl;
		
		while(1)
		{
			wait(e1);
			std::cout << sc_time_stamp() << ": Tic" << std::endl;
			e2.notify(t);
		}
	}

	void Tac()
	{
		std::cout << "Tac: Hello world !" << std::endl;
		
		while(1)
		{
			e1.notify(t);
			wait(e2);
			std::cout << sc_time_stamp() << ": Tac" << std::endl;
		}
	}
};

int sc_main(int argc, const char *argv[])
{
	sc_set_time_resolution(10.0, SC_MS);

	Mod *mod = new Mod("toto");

	sc_start();
	
 	delete mod;
	return 0;
}
