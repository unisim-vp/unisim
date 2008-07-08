#include <systemc.h>
#include "bus_test.hh"
#include "unisim/kernel/service/service.hh"

using unisim::kernel::service::ServiceManager;
using unisim::kernel::service::VariableBase;

int sc_main(int argv, char **argc) {
	Top1 top("top");


	VariableBase *var = ServiceManager::GetParameter("top.bus.bus_cycle_time");
    *var = 0.0;
	var = ServiceManager::GetParameter("kernel_logger.std_out");
	*var = true;
	var = ServiceManager::GetParameter("kernel_logger.std_color");
	*var = true;
	if(!ServiceManager::Setup()) {
		return 0;
	}

	sc_start();
}
