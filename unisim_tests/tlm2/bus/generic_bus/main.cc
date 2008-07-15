#include <systemc.h>
#include "bus_test.hh"
#include "unisim/kernel/service/service.hh"

using unisim::kernel::service::ServiceManager;
using unisim::kernel::service::VariableBase;
using unisim::kernel::logger::LoggerServer;

int sc_main(int argv, char **argc) {
	Top<true> top("top");


	VariableBase *var = ServiceManager::GetParameter("top.bus.bus_cycle_time");
    *var = 10.0;
	var = ServiceManager::GetParameter("top.bus.verbose_all");
	*var = true;
	var = ServiceManager::GetParameter("kernel_logger.std_out");
	*var = true;
	var = ServiceManager::GetParameter("kernel_logger.std_out_color");
	*var = true;
	var = ServiceManager::GetParameter("kernel_logger.std_err");
	*var = false;
	var = ServiceManager::GetParameter("kernel_logger.std_err_color");
	*var = false;
	var = ServiceManager::GetParameter("kernel_logger.xml_file");
	*var = true;
	if(!ServiceManager::Setup()) {
		cerr << "Setup error" << endl;
		return 0;
	}

	sc_start();

	cerr << "Bye, bye" << endl;

}
