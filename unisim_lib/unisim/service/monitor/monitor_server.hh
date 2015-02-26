
#ifndef __UNISIM_SERVICE_MONITOR_SERVER_HH__
#define __UNISIM_SERVICE_MONITOR_SERVER_HH__

#include <unisim/kernel/service/service.hh>

#include <unisim/service/interfaces/monitor_if.hh>

using unisim::kernel::service::Object;
using unisim::kernel::service::ServiceExportBase;
using unisim::service::interfaces::Monitor_if;

namespace unisim {
namespace service {
namespace monitor {


class MonitorServer
	: public Object
	, public Monitor_if
{
public:
	MonitorServer(const char *name, Object *parent = 0, const char *description = 0);
	virtual ~MonitorServer();

	virtual void OnDisconnect();
	virtual bool BeginSetup();
	virtual bool Setup(ServiceExportBase *service_export);
	virtual bool EndSetup();

	static MonitorServer* getInstance(Object *parent);
	static void releaseInstance();

	virtual void refresh_value(const char* name, bool value);
	virtual void refresh_value(const char* name, double value);

	virtual void refresh_value(const char* name, bool value, double time);
	virtual void refresh_value(const char* name, double value, double time);

	virtual void refresh_time(double time);

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
