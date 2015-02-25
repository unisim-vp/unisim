
#ifndef __UNISIM_SERVICE_MONITOR_HH__
#define __UNISIM_SERVICE_MONITOR_HH__

#include <unisim/kernel/service/service.hh>

#include <unisim/service/interfaces/monitor_if.hh>
#include <unisim/service/monitor/monitor_server.hh>

using unisim::service::interfaces::Monitor_if;
using unisim::service::monitor::MonitorServer;

namespace unisim {
namespace service {
namespace monitor {

class Monitor : public unisim::service::interfaces::Monitor_if
{
public:
	Monitor();
	virtual ~Monitor();
	virtual void refresh();

protected:

private:
	MonitorServer* server;
};

} // end of namespace monitor
} // end of namespace service
} // end of namespace unisim

#endif
