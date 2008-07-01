#include <systemc.h>
#include "bus_test.hh"
#include "unisim/kernel/service/service.hh"

using unisim::kernel::service::ServiceManager;
using unisim::kernel::service::VariableBase;

int sc_main(int argv, char **argc) {
	Top1 top("top");


	VariableBase *var = ServiceManager::GetParameter("top.bus.bus_cycle_time");
    *var = 1000.0;
	if(!ServiceManager::Setup()) {
		cerr << "Error" << endl;
	}

	sc_start();
}
