
#include <unisim/service/monitor/monitor.hh>
#include <unisim/service/monitor/monitor_server.hh>

using unisim::service::monitor::MonitorServer;

namespace unisim {
namespace service {
namespace monitor {

Monitor::Monitor(const char *name, Object *parent, const char *description)
	: Object(name, parent)
	, Service<Monitor_if>(name, parent)

	, monitor_export("monitor-export", this)

{
	server = MonitorServer::getInstance(this);
}

Monitor::~Monitor()
{
	MonitorServer::releaseInstance();
}

void Monitor::OnDisconnect()
{

}

bool Monitor::BeginSetup()
{
	return true;
}

bool Monitor::Setup(ServiceExportBase *service_export)
{
	return true;
}

bool Monitor::EndSetup()
{
	return true;
}

void Monitor::refresh_value(const char* name, bool value)
{
	server->refresh_value(name, value);
}

void Monitor::refresh_value(const char* name, double value)
{
	server->refresh_value(name, value);
}

void Monitor::refresh_value(const char* name, bool value, double time)
{
	server->refresh_value(name, value, time);
}

void Monitor::refresh_value(const char* name, double value, double time)
{
	server->refresh_value(name, value, time);
}


void Monitor::refresh_time(double time)
{
	server->refresh_time(time);
}

} // end of namespace monitor
} // end of namespace service
} // end of namespace unisim

