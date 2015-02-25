
#if HAVE_ARTIMON

#ifndef __UNISIM_SERVICE_ARTIMON_MONITOR_HH__
#define __UNISIM_SERVICE_ARTIMON_MONITOR_HH__


#include <unisim/kernel/service/service.hh>

#include <unisim/service/interfaces/monitor_if.hh>

using unisim::service::interfaces::Monitor_if;

namespace unisim {
namespace service {
namespace monitor {

class ArtimonMonitor : public unisim::service::interfaces::Monitor_if
{
public:
	ArtimonMonitor();
	virtual ~ArtimonMonitor();

	virtual void refresh();

protected:

private:


};

} // end of namespace monitor
} // end of namespace service
} // end of namespace unisim

#endif

#endif  // HAVE_ARTIMON

