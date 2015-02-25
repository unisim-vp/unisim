#ifndef __UNISIM_SERVICE_DEFAULT_MONITOR_HH__
#define __UNISIM_SERVICE_DEFAULT_MONITOR_HH__

#include <unisim/kernel/service/service.hh>

#include <unisim/service/interfaces/monitor_if.hh>

using unisim::service::interfaces::Monitor_if;

namespace unisim {
namespace service {
namespace monitor {

class DefaultMonitor : public unisim::service::interfaces::Monitor_if
{
public:
	DefaultMonitor();
	virtual ~DefaultMonitor();

	virtual void refresh();

protected:

private:


};

} // end of namespace monitor
} // end of namespace service
} // end of namespace unisim

#endif

