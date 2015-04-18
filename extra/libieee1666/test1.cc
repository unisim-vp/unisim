#include <systemc.h>
#include <iostream>
#include <ieee1666/util/backtrace.h>

class Mod : public sc_module
{
public:
	Mod(const sc_module_name& name)
		: t(10.0, SC_MS)
	{
		SC_HAS_PROCESS(Mod);
		
		SC_THREAD(Tic);
		SC_THREAD(Tac);
		
		SC_METHOD(Toc);
		sensitive << e2;
	}
private:
	sc_event e1;
	sc_event e2;
	sc_time t;
	
	void Tic()
	{
		std::cout << "Tic: Hello world !" << std::endl;
		
		bool sp = false;
		while(1)
		{
			wait(e1);
			std::cout << sc_time_stamp() << ": Tic" << std::endl;
			e2.notify(t);
			if(!sp)
			{
				sc_spawn(sc_bind(&Mod::Hop, this));
				sp=true;
			}
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
	
	void Toc()
	{
		std::cout << "Toc: Hello world !" << std::endl;
		
		std::cout << sc_time_stamp() << ": Toc" << std::endl;
		//next_trigger(e2);
	}
	
	void Hop()
	{
		std::cout << "Hop: Hello world !" << std::endl;
		
		std::cout << sc_time_stamp() << ": Hop" << std::endl;
		next_trigger(e2);
	}
};

int sc_main(int argc, char *argv[])
{
	sc_set_time_resolution(10.0, SC_MS);

	Mod *mod = new Mod("toto");

	sc_start(100.0, SC_MS);
	
 	delete mod;
	return 0;
}
