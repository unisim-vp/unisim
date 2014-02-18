/*
 * gdbthread.hh
 *
 *  Created on: 10 févr. 2014
 *      Author: rnouacer
 */

#ifndef GDBTHREAD_HH_
#define GDBTHREAD_HH_

#include <iostream>
#include <string>

#include <unisim/kernel/service/service.hh>

#include <unisim/service/pim/network/SocketThread.hpp>
#include <unisim/service/pim/network/BlockingCircularQueue.hpp>

using namespace std;

namespace unisim {
namespace service {
namespace pim {

using unisim::kernel::service::Object;

using unisim::service::pim::network::SocketThread;
using unisim::service::pim::network::BlockingCircularQueue;

class DBGData {
public:

	enum DBGCOMMANDS {TERMINATE, VAR_READ, VAR_WRITE, VAR_LISTEN, VAR_UNLISTEN, VAR_READ_RESPONSE, VAR_LISTEN_RESPONSE, UNKNOWN};

//	enum TargetCOMMANDS {CONTINUE, SUSPEND, DISCONNECT, READ_REGISTERS, WRITE_REGISTERS, SET_THREAD_CONTEXT, STEP_CYCLE, KILL_COMMAND, READ_MEMORY, WRITE_MEMORY, READ_SELECTED_REGISTER, WRITE_SELECTED_REGISTER, QUERY_VARIABLE, STEP_INSTRUCTION, REMOVE_BREAKPOINT, SET_BREAKPOINT, GET_LAST_SIGNAL, UNKNOWN};

	DBGData(DBGCOMMANDS _name) : name(_name), simTime(0), initiatorSite(std::string()), initiator(std::string()),	target(std::string()) { }

	DBGData(DBGCOMMANDS _name, string _initiatorSite, string _initiator,	string _target) :
		name(_name), simTime(0), initiatorSite(_initiatorSite), initiator(_initiator),	target(_target) {}


	~DBGData() { attributes.clear(); }

	DBGCOMMANDS getName() const { return (name); }

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

	double getSimTime() const {
		return simTime;
	}

	void setSimTime(double simTime) {
		this->simTime = simTime;
	}

	void setInitiatorSite(string _initiatorSite) { initiatorSite = _initiatorSite; }
	string getInitiatorSite() const { return (initiatorSite); }

	void setInitiator(string _initiator) { initiator = _initiator; }
	string getInitiator() const { return (initiator); }

	string getTargetSite() const {
		return targetSite;
	}

	void setTargetSite(string _targetSite) {
		this->targetSite = _targetSite;
	}

	void setTarget(string _target) { target = _target; }
	string getTarget() const { return (target); }

	friend std::ostream& operator << (std::ostream& os, DBGData& data);

protected:

private:
	double		simTime;
	DBGCOMMANDS name;
	string initiatorSite;
	string initiator;
	string targetSite;
	string target;

	std::map<std::string, std::string> attributes;

};

class GDBThread : public SocketThread {
public:

	enum MODES {RECEIVE, SEND};

	GDBThread(const char *_name, MODES _mode, Object *_parent = 0):
		SocketThread()
		, name(string(_name))
		, mode(_mode)
		, dataQueue(NULL)

	{
		dataQueue = new BlockingCircularQueue<DBGData*>();
	}

	~GDBThread() {
		delete dataQueue; dataQueue = NULL;
	}

	DBGData* getData() {
		return (dataQueue->next());
	}

	void addData(DBGData* data) {
		dataQueue->add(data);
	}

	virtual void run() {

		if (mode == RECEIVE) {
			receiveLoop();
		} else {
			sendLoop();
		}

	}

private:
	static const string SEGMENT_SEPARATOR;
	static const string FIELD_SEPARATOR;

	string name;
	MODES mode;
	BlockingCircularQueue<DBGData*> *dataQueue;

	void receiveLoop() {

		while (!super::isTerminated()) {

			string buf_str;

			pthread_mutex_lock( &sockfd_mutex );

			if (!GetDatagramPacket(buf_str, false)) {

			    pthread_mutex_unlock( &sockfd_mutex );

#ifdef WIN32
				Sleep(1);
#else
				usleep(1000);
#endif
				continue;

			}

		    pthread_mutex_unlock( &sockfd_mutex );

			if ((buf_str.compare("EOS") == 0) || (super::isTerminated())) {
				DBGData *request = new DBGData(DBGData::TERMINATE);
				dataQueue->add(request);

			} else {

				if (1 == 0) {

				}
				else if(buf_str.substr(0, 6) == "qRcmd,")
				{
					HandleQRcmd(buf_str.substr(6));
				}

			}

		}
	}

	void sendLoop() {

		while (!super::isTerminated()) {

			DBGData* response = getData();

			std::ostringstream os;

			switch (response->getName()) {
				case DBGData::VAR_READ_RESPONSE: {
					os << response->getSimTime() << SEGMENT_SEPARATOR;

					string targetVar = response->getTarget();

					os << response->getTarget() << FIELD_SEPARATOR;

					os << response->getAttribute("value") << SEGMENT_SEPARATOR;

				} break;
				case DBGData::VAR_LISTEN_RESPONSE: {
					os << response->getSimTime() << SEGMENT_SEPARATOR;

					string targetVar = response->getTarget();

					os << response->getTarget() << FIELD_SEPARATOR;

					os << response->getAttribute("value") << SEGMENT_SEPARATOR;

				} break;

				default: cerr << "GDBThread UNKNOWN response => " << std::endl; break;
			}

			std::string str = os.str();

			pthread_mutex_lock( &sockfd_mutex );

			PutDatagramPacket(str);

		    pthread_mutex_unlock( &sockfd_mutex );

			os.str(std::string());

			if (response) { delete response; response = NULL; }
		}

	}

	void HandleQRcmd(string command) {

		// qRcmd,cmd;var_name[:value]{;var_name[:value]}
		int start_index = 0;
		int end_index = command.find(';', start_index);
		string cmdPrefix = command.substr(start_index, end_index-start_index);

		start_index = end_index+1;

		if (cmdPrefix.compare("var_listen") == 0) {

			do {

				DBGData *request = new DBGData(DBGData::VAR_LISTEN);

				string name = command.substr(start_index, end_index-start_index);

				request->setInitiatorSite("workbench");
				request->setInitiator(name);
				request->setTargetSite("simulator");
				request->setTarget(name);

				dataQueue->add(request);

				start_index = end_index+1;

				end_index = command.find(';', start_index);
				if (end_index != string::npos) {
					start_index = end_index+1;
				}
			} while (end_index != string::npos);

		}
		else	if (cmdPrefix.compare("var_unlisten") == 0) {

			do {
				DBGData *request = new DBGData(DBGData::VAR_UNLISTEN);

				string name = command.substr(start_index, end_index-start_index);

				request->setInitiatorSite("workbench");
				request->setInitiator(name);
				request->setTargetSite("simulator");
				request->setTarget(name);

				dataQueue->add(request);

				start_index = end_index+1;

				end_index = command.find(';', start_index);
				if (end_index != string::npos) {
					start_index = end_index+1;
				}
			} while (end_index != string::npos);


		}
		else	if (cmdPrefix.compare("var_read") == 0) {

			do {

				DBGData *request = new DBGData(DBGData::VAR_READ);

				string name = command.substr(start_index, end_index-start_index);

				request->setInitiatorSite("workbench");
				request->setInitiator(name);
				request->setTargetSite("simulator");
				request->setTarget(name);

				dataQueue->add(request);

				start_index = end_index+1;

				end_index = command.find(';', start_index);
				if (end_index != string::npos) {
					start_index = end_index+1;
				}
			} while (end_index != string::npos);

		}
		else if (cmdPrefix.compare("var_write") == 0) {

			DBGData *request = new DBGData(DBGData::VAR_WRITE);

			end_index = command.find(':');

			string name = command.substr(start_index, end_index-start_index);
			start_index = end_index+1;

			request->setInitiatorSite("workbench");
			request->setInitiator(name);
			request->setTargetSite("simulator");
			request->setTarget(name);

			string value = command.substr(start_index);

			request->addAttribute("value", value);

			dataQueue->add(request);

		} else {
			DBGData *request = new DBGData(DBGData::UNKNOWN);
			dataQueue->add(request);
		}

	}

};


} // pim namespace
} //service namespace
} // unisim namespace

#endif /* GDBTHREAD_HH_ */
