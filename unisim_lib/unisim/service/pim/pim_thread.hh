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
#include <unistd.h>

#include <unisim/kernel/service/service.hh>

#include <unisim/service/interfaces/time.hh>

#include <unisim/util/converter/convert.hh>
#include <unisim/service/time/sc_time/time.hh>

#include <unisim/service/pim/network/SocketThread.hpp>
#include <unisim/service/pim/network/BlockingCircularQueue.hpp>

using namespace std;

namespace unisim {
namespace service {
namespace pim {

using unisim::kernel::service::Object;
using unisim::kernel::service::VariableBaseListener;
using unisim::kernel::service::Simulator;
using unisim::kernel::service::VariableBase;

using unisim::service::pim::network::SocketThread;
using unisim::service::pim::network::BlockingCircularQueue;

class PIMData {
public:

	enum PCOMMANDS {TERMINATE, VAR_READ, VAR_WRITE, VAR_LISTEN, VAR_UNLISTEN, UNKNOWN};

	PIMData(PCOMMANDS _name) : name(_name), initiatorSite(std::string()), initiator(std::string()),	target(std::string()) {}

	PIMData(PCOMMANDS _name, string _initiatorSite, string _initiator,	string _target) :
		name(_name), initiatorSite(_initiatorSite), initiator(_initiator),	target(_target) {}


	~PIMData() { attributes.clear(); }

	PCOMMANDS getName() { return (name); }

	bool addAttribute(std::string name, std::string value) {

		if (attributes.find(name) == attributes.end()) {
			attributes.insert ( std::pair<std::string, std::string>(name, value) );
		} else {
			return (false);
		}

		return (true);
	}

	bool removeAttribute(std::string name) {

		if (attributes.find(name) == attributes.end()) {
			return (false);
		} else {
			attributes.erase (name);
		}

		return (true);
	}

	std::string getAttribute(std::string name) {

		std::map<std::string, std::string>::iterator it = attributes.find(name);
		if (it == attributes.end()) {
			return (std::string());
		}

		return (it->second);
	}

	bool setAttribute(std::string name, std::string value) {

		std::map<std::string, std::string>::iterator it = attributes.find(name);
		if (it == attributes.end()) {
			return (false);
		} else {
			it->second = value;
		}

		return  (true);
	}

	std::map<std::string, std::string> getAttributes() { return (attributes); }

	void setInitiatorSite(string _initiatorSite) { initiatorSite = _initiatorSite; }
	string getInitiatorSite() { return (initiatorSite); }

	void setInitiator(string _initiator) { initiator = _initiator; }
	string getInitiator() { return (initiator); }

	void setTarget(string _target) { target = _target; }
	string getTarget() { return (target); }

	friend std::ostream& operator << (std::ostream& os, PIMData& data);

protected:

private:
	PCOMMANDS name;
	string initiatorSite;
	string initiator;
	string target;

	std::map<std::string, std::string> attributes;

};

class RequestThread : public SocketThread {
public:
	RequestThread(const char *_name, Object *_parent = 0):
		SocketThread()
		, name(string(_name))
		, requestsQueue(NULL)

	{
		requestsQueue = new BlockingCircularQueue<PIMData*>();
	}

	~RequestThread() {
		delete requestsQueue; requestsQueue = NULL;
	}

	PIMData* getRequest() {
		PIMData *request = requestsQueue->next();

		return request;
	}

	virtual void run() {

		while (!super::isTerminated()) {

			string buf_str;

			if (!GetDatagramPacket(buf_str, blocking)) {
				if (blocking) {
					cerr << "PIM-RequestThread receive **NULL**" << endl;
					break;
				} else {
	#ifdef WIN32
					Sleep(1);
	#else
					usleep(1000);
	#endif
					continue;
				}
			}

			if ((buf_str.compare("EOS") == 0) || (super::isTerminated())) {
				PIMData *request = new PIMData(PIMData::TERMINATE);
				requestsQueue->add(request);

			} else {

				// qRcmd,cmd;var_name[:value]{;var_name[:value]}
				int start_index = 0;
				int end_index = buf_str.find(',');
				string qRcmd = buf_str.substr(start_index, end_index-start_index);

				start_index = end_index+1;

				end_index = buf_str.find(';', start_index);
				string cmd = buf_str.substr(start_index, end_index-start_index);

				start_index = end_index+1;

				if (cmd.compare("listen") == 0) {

					do {

						PIMData *request = new PIMData(PIMData::VAR_LISTEN);

						string name = buf_str.substr(start_index, end_index-start_index);
						request->setInitiator(name);
						request->setTarget(name);
						request->setInitiatorSite("workbench");

						requestsQueue->add(request);

						start_index = end_index+1;

						end_index = buf_str.find(';', start_index);
						if (end_index != string::npos) {
							start_index = end_index+1;
						}
					} while (end_index != string::npos);

				}
				else	if (cmd.compare("unlisten") == 0) {

					do {
						PIMData *request = new PIMData(PIMData::VAR_UNLISTEN);

						string name = buf_str.substr(start_index, end_index-start_index);
						request->setInitiator(name);
						request->setTarget(name);
						request->setInitiatorSite("workbench");

						requestsQueue->add(request);

						start_index = end_index+1;

						end_index = buf_str.find(';', start_index);
						if (end_index != string::npos) {
							start_index = end_index+1;
						}
					} while (end_index != string::npos);


				}
				else	if (cmd.compare("read") == 0) {

					do {

						PIMData *request = new PIMData(PIMData::VAR_READ);

						string name = buf_str.substr(start_index, end_index-start_index);
						request->setInitiator(name);
						request->setTarget(name);
						request->setInitiatorSite("workbench");

						requestsQueue->add(request);

						start_index = end_index+1;

						end_index = buf_str.find(';', start_index);
						if (end_index != string::npos) {
							start_index = end_index+1;
						}
					} while (end_index != string::npos);

				}
				else if (cmd.compare("write") == 0) {

					PIMData *request = new PIMData(PIMData::VAR_WRITE);

					end_index = buf_str.find(':');

					string name = buf_str.substr(start_index, end_index-start_index);
					start_index = end_index+1;
					request->setInitiator(name);
					request->setTarget(name);
					request->setInitiatorSite("workbench");

					string value = buf_str.substr(start_index);

					request->addAttribute("value", value);

					requestsQueue->add(request);

				} else {
					PIMData *request = new PIMData(PIMData::UNKNOWN);
					requestsQueue->add(request);
				}

			}

		}

	}

private:
	string name;
	BlockingCircularQueue<PIMData*> *requestsQueue;
};

class PIMThread : public SocketThread, VariableBaseListener, virtual public Object {
public:
	PIMThread(const char *_name, Object *_parent = 0);
	~PIMThread();

	virtual void run();

	double GetSimTime();
	bool UpdateTimeRatio();
	double GetLastTimeRatio();

	virtual void VariableBaseNotify(const VariableBase *var);

private:
	string name;
	double last_time_ratio;
	ofstream pim_trace_file;

	RequestThread *requestThread;
};

} // end pim
} // end service
} // end unisim


#endif /* PIM_THREAD_HH_ */
