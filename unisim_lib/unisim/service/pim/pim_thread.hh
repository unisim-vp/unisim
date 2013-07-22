/*
 * pim_thread.hh
 *
 *  Created on: 20 janv. 2011
 *      Author: rnouacer
 */

#ifndef PIM_THREAD_HH_
#define PIM_THREAD_HH_

#include <fstream>

#include <math.h>

#include <unisim/kernel/service/service.hh>

#include <unisim/service/interfaces/time.hh>

#include <unisim/util/converter/convert.hh>
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

	virtual void run();
	double GetSimTime();

	bool UpdateTimeRatio() {
/*
	- add a time_ratio = HotsTime/SimulatedTime response
	- the time_ratio is used by timed/periodic operations
*/


		double new_time_ratio = last_time_ratio;
		double sim_time = Object::GetSimulator()->GetSimTime();
		double host_time = Object::GetSimulator()->GetHostTime();

		bool has_changed = false;

		if (sim_time > 0) {
			new_time_ratio = host_time / sim_time;
		}
		if ((sim_time == 0) || (fabs(last_time_ratio - new_time_ratio) > 0.1)) {
			pim_trace_file << (sim_time * 1000) << " \t" << (new_time_ratio) << endl;
			last_time_ratio = new_time_ratio;
			has_changed = true;
		}

		return has_changed;
	}

	double GetLastTimeRatio() { return last_time_ratio; }

private:
	string name;
	double last_time_ratio;
	ofstream pim_trace_file;

};


} // end pim
} // end service
} // end unisim


#endif /* PIM_THREAD_HH_ */
