
#ifndef __UNISIM_SERVICE_MONITOR_HH__
#define __UNISIM_SERVICE_MONITOR_HH__

#include <unisim/kernel/service/service.hh>

#include <unisim/service/interfaces/monitor_if.hh>
#include <unisim/service/monitor/monitor_server.hh>

using unisim::kernel::service::Object;
using unisim::kernel::service::Service;
using unisim::kernel::service::ServiceExportBase;
using unisim::kernel::service::ServiceExport;

using unisim::service::interfaces::Monitor_if;
using unisim::service::monitor::MonitorServer;

namespace unisim {
namespace service {
namespace monitor {

class Monitor
	 : public Service<Monitor_if>

{
public:
	ServiceExport<Monitor_if> monitor_export;

	Monitor(const char *name, Object *parent = 0, const char *description = 0);
	virtual ~Monitor();

	virtual void OnDisconnect();
	virtual bool BeginSetup();
	virtual bool Setup(ServiceExportBase *service_export);
	virtual bool EndSetup();

	virtual void refresh_value(const char* name, bool value);
	virtual void refresh_value(const char* name, double value);

	virtual void refresh_value(const char* name, bool value, double time);
	virtual void refresh_value(const char* name, double value, double time);

	virtual void refresh_time(double time);

protected:

private:
	MonitorServer* server;
};

} // end of namespace monitor
} // end of namespace service
} // end of namespace unisim

#endif
