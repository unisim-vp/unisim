
#include <unisim/service/monitor/monitor_server.hh>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_ARTIMON
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

MonitorServer::MonitorServer(const char *name, Object *parent, const char *description)
	: Object(name, parent)
{

#ifdef HAVE_ARTIMON
		monitor = new ArtimonMonitor("ArtimonMonitor", this);
#else
		monitor = new DefaultMonitor("DefaultMonitor", this);
#endif //  HAVE_ARTIMON

}

MonitorServer::~MonitorServer()
{
	if (monitor) { delete monitor; monitor = NULL; }
}

void MonitorServer::OnDisconnect()
{

}

bool MonitorServer::BeginSetup()
{
	return true;
}

bool MonitorServer::Setup(ServiceExportBase *service_export)
{
	return true;
}

bool MonitorServer::EndSetup()
{
	return true;
}

MonitorServer* MonitorServer::getInstance(Object *parent)
{
	if (singleton == NULL) {

		singleton = new MonitorServer("MonitorServer", parent);
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

void MonitorServer::refresh_value(const char* name, bool value)
{
	monitor->refresh_value(name, value);
}

void MonitorServer::refresh_value(const char* name, double value)
{
	monitor->refresh_value(name, value);
}

void MonitorServer::refresh_value(const char* name, bool value, double time)
{
	monitor->refresh_value(name, value, time);
}

void MonitorServer::refresh_value(const char* name, double value, double time)
{
	monitor->refresh_value(name, value, time);
}

void MonitorServer::refresh_time(double time)
{
	monitor->refresh_time(time);
}


} // end of namespace monitor
} // end of namespace service
} // end of namespace unisim

