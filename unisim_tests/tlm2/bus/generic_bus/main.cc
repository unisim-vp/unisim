#include <systemc.h>
#include "bus_test.hh"
#include "unisim/kernel/service/service.hh"

using unisim::kernel::service::ServiceManager;
using unisim::kernel::service::VariableBase;

int sc_main(int argv, char **argc) {
	Top1 top("top");


	VariableBase *var = ServiceManager::GetParameter("top.bus.bus_cycle_time");
    *var = 0.0;
	if(!ServiceManager::Setup()) {
		return 0;
	}

	sc_start();
}
