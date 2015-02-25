
#ifndef __UNISIM_SERVICE_INTERFACES_MONITOR_IF_HH__
#define __UNISIM_SERVICE_INTERFACES_MONITOR_IF_HH__

#include <unisim/kernel/service/service.hh>

namespace unisim {
namespace service {
namespace interfaces {

class Monitor_if : public unisim::kernel::service::ServiceInterface
{
public:
	virtual void refresh() = 0;
};

} // end of namespace interfaces
} // end of namespace service
} // end of namespace unisim

#endif
