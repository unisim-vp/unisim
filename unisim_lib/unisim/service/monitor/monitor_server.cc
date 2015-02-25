
#include <unisim/service/monitor/monitor_server.hh>

#if HAVE_ARTIMON
	#include <unisim/service/monitor/artimon_monitor.hh>
	using unisim::service::monitor::ArtimonMonitor;
#else
	#include <unisim/service/monitor/default_monitor.hh>
	using unisim::service::monitor::DefaultMonitor;
#endif //  HAVE_ARTIMON

namespace unisim {
namespace service {
namespace monitor {

MonitorServer* MonitorServer::singleton = 0;
int MonitorServer::singletonRef = 0;

MonitorServer::MonitorServer()
{
#if HAVE_ARTIMON
		monitor = new ArtimonMonitor();
#else
		monitor = new DefaultMonitor();
#endif //  HAVE_ARTIMON

}

MonitorServer::~MonitorServer()
{
	if (monitor) { delete monitor; monitor = NULL; }
}

MonitorServer* MonitorServer::getInstance()
{
	if (singleton == NULL) {

		singleton = new MonitorServer();
		singletonRef = 0;
	}

	singletonRef++;

	return singleton;
}

void MonitorServer::releaseInstance()
{
	assert((singletonRef > 0) && (singleton != NULL));

	singletonRef--;

	if (singletonRef == 0) {
		delete singleton; singleton = NULL; singletonRef = 0;
	}
}

void MonitorServer::refresh()
{
	monitor->refresh();
}

} // end of namespace monitor
} // end of namespace service
} // end of namespace unisim

