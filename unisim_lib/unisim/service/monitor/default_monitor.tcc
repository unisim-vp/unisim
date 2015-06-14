

#include <unisim/service/monitor/default_monitor.hh>

#include <iostream>

namespace unisim {
namespace service {
namespace monitor {

template <class ADDRESS>
DefaultMonitor<ADDRESS>::DefaultMonitor(const char *name, Object *parent, const char *description)
	: Object(name, parent)
{

}

template <class ADDRESS>
DefaultMonitor<ADDRESS>::~DefaultMonitor()
{

}

template <class ADDRESS>
void DefaultMonitor<ADDRESS>::OnDisconnect()
{

}

template <class ADDRESS>
bool DefaultMonitor<ADDRESS>::BeginSetup()
{
	return true;
}

template <class ADDRESS>
bool DefaultMonitor<ADDRESS>::Setup(ServiceExportBase *service_export)
{
	return true;
}

template <class ADDRESS>
bool DefaultMonitor<ADDRESS>::EndSetup()
{
	return true;
}

template <class ADDRESS>
int DefaultMonitor<ADDRESS>::generate_monitor_spec(const char* file_path) {
	std::cout << "DefaultMonitor:: generate_monitor_spec() " << file_path << std::endl;
	return (0);
}

template <class ADDRESS>
void DefaultMonitor<ADDRESS>::refresh_value(const char* name, bool value)
{
	//	extern void (* artimon_refresh_atom)(int pos, int at);
	//	extern void (* artimon_refresh_signal)(int pos, unsigned * sig);
	std::cout << "refresh_value bool" << std::endl;
}

template <class ADDRESS>
void DefaultMonitor<ADDRESS>::refresh_value(const char* name, double value)
{
	//	extern void (* artimon_refresh_atom)(int pos, int at);
	//	extern void (* artimon_refresh_signal)(int pos, unsigned * sig);
	std::cout << "refresh_value double" << std::endl;

}

template <class ADDRESS>
void DefaultMonitor<ADDRESS>::refresh_value(const char* name, bool value, double time)
{
	//	extern void (* artimon_refresh_atom)(int pos, int at);
	//	extern void (* artimon_refresh_signal)(int pos, unsigned * sig);
	//	extern double (* artimon_refresh_time)(double t);
	std::cout << "refresh_value bool/time" << std::endl;

}

template <class ADDRESS>
void DefaultMonitor<ADDRESS>::refresh_value(const char* name, double value, double time)
{
	//	extern void (* artimon_refresh_atom)(int pos, int at);
	//	extern void (* artimon_refresh_signal)(int pos, unsigned * sig);
	//	extern double (* artimon_refresh_time)(double t);
	std::cout << "refresh_value double/time" << std::endl;

}

template <class ADDRESS>
void DefaultMonitor<ADDRESS>::refresh_time(double time)
{
	//	extern double (* artimon_refresh_time)(double t);
	std::cout << "refresh_time" << std::endl;

}

} // end of namespace monitor
} // end of namespace service
} // end of namespace unisim

