
#include <unisim/service/monitor/default_monitor.hh>

#include <iostream>

namespace unisim {
namespace service {
namespace monitor {

DefaultMonitor::DefaultMonitor(const char *name, Object *parent, const char *description)
	: Object(name, parent)
{

}

DefaultMonitor::~DefaultMonitor()
{

}

void DefaultMonitor::OnDisconnect()
{

}

bool DefaultMonitor::BeginSetup()
{
	return true;
}

bool DefaultMonitor::Setup(ServiceExportBase *service_export)
{
	return true;
}

bool DefaultMonitor::EndSetup()
{
	return true;
}

void DefaultMonitor::refresh_value(const char* name, bool value)
{
	//	extern void (* artimon_refresh_atom)(int pos, int at);
	//	extern void (* artimon_refresh_signal)(int pos, unsigned * sig);
	std::cout << "refresh_value bool" << std::endl;
}

void DefaultMonitor::refresh_value(const char* name, double value)
{
	//	extern void (* artimon_refresh_atom)(int pos, int at);
	//	extern void (* artimon_refresh_signal)(int pos, unsigned * sig);
	std::cout << "refresh_value double" << std::endl;

}

void DefaultMonitor::refresh_value(const char* name, bool value, double time)
{
	//	extern void (* artimon_refresh_atom)(int pos, int at);
	//	extern void (* artimon_refresh_signal)(int pos, unsigned * sig);
	//	extern double (* artimon_refresh_time)(double t);
	std::cout << "refresh_value bool/time" << std::endl;

}

void DefaultMonitor::refresh_value(const char* name, double value, double time)
{
	//	extern void (* artimon_refresh_atom)(int pos, int at);
	//	extern void (* artimon_refresh_signal)(int pos, unsigned * sig);
	//	extern double (* artimon_refresh_time)(double t);
	std::cout << "refresh_value double/time" << std::endl;

}

void DefaultMonitor::refresh_time(double time)
{
	//	extern double (* artimon_refresh_time)(double t);
	std::cout << "refresh_time" << std::endl;

}

} // end of namespace monitor
} // end of namespace service
} // end of namespace unisim

