#include <systemc.h>
#include <iostream>

class Mod : public sc_module
{
public:
	Mod(const sc_module_name& name)
	{
		SC_HAS_PROCESS(Mod);
		
		SC_THREAD(Process);
	}
private:
	void Process()
	{
		std::cout << "Hello world !" << std::endl;
		
		while(1)
		{
			wait();
			std::cout << "Awaken" << std::endl;
		}
	}
};

int sc_main(int argc, const char *argv[])
{
 	Mod *mod = new Mod("toto");
 	
	sc_set_time_resolution(1.0, SC_PS);
	sc_time t = sc_time(1.23456789, SC_SEC);
	
	std::cout << "t=" << t << std::endl;
	std::cout << "t=" << t.to_seconds() << " seconds" << std::endl;
	std::cout << "time_resolution=" << sc_get_time_resolution() << std::endl;
	std::cout << "max_time=" << sc_max_time() << std::endl;
	sc_start();
	
 	delete mod;
	return 0;
}
