/*
 * pim_thread.hh
 *
 *  Created on: 20 janv. 2011
 *      Author: rnouacer
 */

#ifndef PIM_THREAD_HH_
#define PIM_THREAD_HH_

#include <unisim/kernel/service/service.hh>

#include <unisim/service/interfaces/time.hh>

#include <unisim/service/pim/convert.hh>
#include <unisim/service/time/sc_time/time.hh>

#include <unisim/service/pim/network/SocketThread.hpp>

namespace unisim {
namespace service {
namespace pim {

using namespace std;

using unisim::kernel::service::Object;

using unisim::service::pim::network::SocketThread;

class PIMThread : public SocketThread, virtual public Object {
public:
	PIMThread(const char *_name, Object *_parent = 0);
	~PIMThread();

	virtual void Run();
	double GetSimTime();

private:
	string name;

};


} // end pim
} // end service
} // end unisim


#endif /* PIM_THREAD_HH_ */
