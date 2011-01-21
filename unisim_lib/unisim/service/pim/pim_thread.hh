/*
 * pim_thread.hh
 *
 *  Created on: 20 janv. 2011
 *      Author: rnouacer
 */

#ifndef PIM_THREAD_HH_
#define PIM_THREAD_HH_

#include <unisim/kernel/service/service.hh>

#include <unisim/service/pim/convert.hh>

#include <unisim/service/pim/network/SocketThread.hpp>

namespace unisim {
namespace service {
namespace pim {

using namespace std;

using unisim::service::pim::network::SocketThread;

class PIMThread : public SocketThread {
public:
	PIMThread(string _name);

	virtual void Run();
	virtual string getProtocol() { return "PIM"; }

private:
	string name;

};


} // end pim
} // end service
} // end unisim


#endif /* PIM_THREAD_HH_ */
