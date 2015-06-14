
#ifndef __UNISIM_SERVICE_MONITOR_HH__
#define __UNISIM_SERVICE_MONITOR_HH__

#include <unisim/kernel/service/service.hh>

#include <unisim/service/interfaces/debug_event.hh>

#include <unisim/service/interfaces/monitor_if.hh>
#include <unisim/service/monitor/monitor_server.hh>

using unisim::kernel::service::Object;
using unisim::kernel::service::Service;
using unisim::kernel::service::Parameter;
using unisim::kernel::service::ServiceExportBase;
using unisim::kernel::service::ServiceExport;
using unisim::kernel::service::ServiceImport;

using unisim::service::interfaces::DebugEventListener;
using unisim::service::interfaces::DebugEventTrigger;

using unisim::service::interfaces::Monitor_if;
using unisim::service::monitor::MonitorServer;

namespace unisim {
namespace service {
namespace monitor {

template <class ADDRESS>
class Monitor
	 : public Service<Monitor_if<ADDRESS> >

	, public Service<DebugEventListener<ADDRESS> >
	, public unisim::kernel::service::Client<DebugEventTrigger<ADDRESS> >

{
public:
	ServiceExport<Monitor_if<ADDRESS> > monitor_export;

	ServiceExport<DebugEventListener<ADDRESS> > debug_event_listener_export;
	ServiceImport<DebugEventTrigger<ADDRESS> > debug_event_trigger_import;

	Monitor(const char *name, Object *parent = 0, const char *description = 0);
	virtual ~Monitor();

	// DebugEventListener
	virtual void OnDebugEvent(const unisim::util::debug::Event<ADDRESS>& event);

	virtual void OnDisconnect();
	virtual bool BeginSetup();
	virtual bool Setup(ServiceExportBase *service_export);
	virtual bool EndSetup();

	virtual int generate_monitor_spec(const char* file_path);

	virtual void refresh_value(const char* name, bool value);
	virtual void refresh_value(const char* name, double value);

	virtual void refresh_value(const char* name, bool value, double time);
	virtual void refresh_value(const char* name, double value, double time);

	virtual void refresh_time(double time);

protected:

private:
	MonitorServer<ADDRESS>* server;

	std::string xml_spec_file_path;
	Parameter<std::string> param_spec_xml_file_path;

};

} // end of namespace monitor
} // end of namespace service
} // end of namespace unisim

#endif
