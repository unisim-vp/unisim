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
 	
	sc_start();
	
 	delete mod;
	return 0;
}
