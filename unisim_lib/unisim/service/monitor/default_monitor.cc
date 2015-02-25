
#include <unisim/service/monitor/default_monitor.hh>

#include <iostream>

namespace unisim {
namespace service {
namespace monitor {

DefaultMonitor::DefaultMonitor()
{

}

DefaultMonitor::~DefaultMonitor()
{

}

void DefaultMonitor::refresh()
{
	std::cout << "I AM DEFAUT MONITOR" << std::endl;
}

} // end of namespace monitor
} // end of namespace service
} // end of namespace unisim

