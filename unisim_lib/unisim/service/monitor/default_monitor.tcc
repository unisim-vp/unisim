

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
	std::cout << "Default_monitor::refresh_value bool is running" << std::endl;
}

template <class ADDRESS>
void DefaultMonitor<ADDRESS>::refresh_value(const char* name, double value)
{
	std::cout << "Default_monitor::refresh_value double is running" << std::endl;
}

template <class ADDRESS>
void DefaultMonitor<ADDRESS>::refresh_value(const char* name, bool value, double time)
{
	std::cout << "Default_monitor::refresh_value bool/time is running" << std::endl;
}

template <class ADDRESS>
void DefaultMonitor<ADDRESS>::refresh_value(const char* name, double value, double time)
{
	std::cout << "Default_monitor::refresh_value double/time is running" << std::endl;
}

template <class ADDRESS>
void DefaultMonitor<ADDRESS>::refresh_time(double time)
{
	std::cout << "Default_monitor::refresh_time is running" << std::endl;
}

} // end of namespace monitor
} // end of namespace service
} // end of namespace unisim

