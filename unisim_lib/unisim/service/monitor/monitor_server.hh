
#ifndef __UNISIM_SERVICE_MONITOR_SERVER_HH__
#define __UNISIM_SERVICE_MONITOR_SERVER_HH__

#include <unisim/kernel/service/service.hh>

#include <unisim/service/interfaces/monitor_if.hh>

using unisim::service::interfaces::Monitor_if;

namespace unisim {
namespace service {
namespace monitor {


class MonitorServer : public unisim::service::interfaces::Monitor_if
{
public:
	MonitorServer();
	virtual ~MonitorServer();

	static MonitorServer* getInstance();
	static void releaseInstance();

	virtual void refresh();
protected:

private:
	static MonitorServer* singleton;
	static int singletonRef;
	Monitor_if *monitor;

};

} // end of namespace monitor
} // end of namespace service
} // end of namespace unisim

#endif
