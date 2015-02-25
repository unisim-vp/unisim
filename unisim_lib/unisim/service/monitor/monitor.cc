
#include <unisim/service/monitor/monitor.hh>
#include <unisim/service/monitor/monitor_server.hh>

using unisim::service::monitor::MonitorServer;

namespace unisim {
namespace service {
namespace monitor {

Monitor::Monitor()
{
	server = MonitorServer::getInstance();
}

Monitor::~Monitor()
{
	MonitorServer::releaseInstance();
}

void Monitor::refresh()
{
	server->refresh();
}

} // end of namespace monitor
} // end of namespace service
} // end of namespace unisim

