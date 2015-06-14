
#include <unisim/service/monitor/monitor.hh>

namespace unisim {
namespace service {
namespace monitor {

template <class ADDRESS>
Monitor<ADDRESS>::Monitor(const char *name, Object *parent, const char *description)
	: Object(name, parent)
	, Service<Monitor_if<ADDRESS> >(name, parent)

	, Service<DebugEventListener<ADDRESS> >(name, parent)
	, unisim::kernel::service::Client<DebugEventTrigger<ADDRESS> >(name, parent)

	, monitor_export("monitor-export", this)
	, debug_event_listener_export("debug-event-listener-export", this)
	, debug_event_trigger_import("debug-event-trigger-import", this)

	, xml_spec_file_path("xml_spec_file_path.xml")
	, param_spec_xml_file_path("xml-spec-file-path", this, xml_spec_file_path)

{
	server = MonitorServer<ADDRESS>::getInstance(this);
}

template <class ADDRESS>
Monitor<ADDRESS>::~Monitor()
{
	MonitorServer<ADDRESS>::releaseInstance();
}

template <class ADDRESS>
void Monitor<ADDRESS>::OnDisconnect()
{

}

template <class ADDRESS>
bool Monitor<ADDRESS>::BeginSetup()
{
	int result = generate_monitor_spec(xml_spec_file_path.c_str());

	return true;
}

template <class ADDRESS>
bool Monitor<ADDRESS>::Setup(ServiceExportBase *service_export)
{
	return true;
}

template <class ADDRESS>
bool Monitor<ADDRESS>::EndSetup()
{
	return true;
}

template <class ADDRESS>
int Monitor<ADDRESS>::generate_monitor_spec(const char* file_path) {
	return (server->generate_monitor_spec(file_path));
}

template <class ADDRESS>
void Monitor<ADDRESS>::OnDebugEvent(const unisim::util::debug::Event<ADDRESS>& event)
{
//	switch(event.GetType())
//	{
//		case unisim::util::debug::Event<ADDRESS>::EV_BREAKPOINT:
//			break;
//		case unisim::util::debug::Event<ADDRESS>::EV_WATCHPOINT:
//			watchpoint_hit = dynamic_cast<const Watchpoint<ADDRESS> *> (&event);
//			break;
//		default:
//			// ignore event
//			return;
//	}
//
//	trap = true;
//	synched = true;
}

template <class ADDRESS>
void Monitor<ADDRESS>::refresh_value(const char* name, bool value)
{
	server->refresh_value(name, value);
}

template <class ADDRESS>
void Monitor<ADDRESS>::refresh_value(const char* name, double value)
{
	server->refresh_value(name, value);
}

template <class ADDRESS>
void Monitor<ADDRESS>::refresh_value(const char* name, bool value, double time)
{
	server->refresh_value(name, value, time);
}

template <class ADDRESS>
void Monitor<ADDRESS>::refresh_value(const char* name, double value, double time)
{
	server->refresh_value(name, value, time);
}


template <class ADDRESS>
void Monitor<ADDRESS>::refresh_time(double time)
{
	server->refresh_time(time);
}

} // end of namespace monitor
} // end of namespace service
} // end of namespace unisim


