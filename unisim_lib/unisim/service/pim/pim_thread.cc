/*
 * pim_thread.cc
 *
 *  Created on: 20 janv. 2011
 *      Author: rnouacer
 */

#include <unisim/service/pim/pim_thread.hh>

namespace unisim {
namespace service {
namespace pim {

using namespace std;

double PIMThread::GetSimTime() {
	return Object::GetSimulator()->GetSimTime();
}

double PIMThread::GetLastTimeRatio() { return (last_time_ratio); }

bool PIMThread::UpdateTimeRatio() {
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

PIMThread::PIMThread(const char *_name, Object *_parent) :
	SocketThread()
	, VariableBaseListener()
	, Object(_name, _parent)

	, requestThread(NULL)
	, responseThread(NULL)
	, name(string(_name))
	, last_time_ratio(-1)

{

}

PIMThread::~PIMThread() {

	if (requestThread) {
		if (!requestThread->isTerminated()) {
			requestThread->stop();
		}
		delete requestThread; requestThread = NULL;
	}

	if (responseThread) {
		if (!responseThread->isTerminated()) {
			responseThread->stop();
		}
		delete responseThread; responseThread = NULL;
	}

	pim_trace_file.close();

}

void PIMThread::run(){

	requestThread = new GDBThread("request-Thread", GDBThread::RECEIVE);
	requestThread->startSocketThread(getSockfd());

	responseThread = new GDBThread("response-Thread", GDBThread::SEND);
	responseThread->startSocketThread(getSockfd());

	pim_trace_file.open ("pim_trace.xls");


	cerr << "PIM::TargetThread start RUN " << std::endl;

	// Explore Simulator Variables to Generate PIM XML File
	vector<VariableBase*> simulator_variables;

	std::list<VariableBase *> lst;

	Simulator::simulator->GetSignals(lst);

	for (std::list<VariableBase *>::iterator it = lst.begin(); it != lst.end(); it++) {

		if (!((VariableBase *) *it)->IsVisible()) continue;

		simulator_variables.push_back((VariableBase *) *it);

	}

	lst.clear();

	while (!super::isTerminated()) {

		DBGData* request = requestThread->getData();

		if (request->getName() == DBGData::TERMINATE) {
			if (requestThread) {
				requestThread->stop();
			}

			if (responseThread) {
				responseThread->stop();
			}

			super::stop();
		} else {

			// qRcmd,cmd;var_name[:value]{;var_name[:value]}

			if (request->getName() == DBGData::VAR_LISTEN) {

				string targetVar = request->getTarget();
				for (int i=0; i < simulator_variables.size(); i++) {
					if (targetVar.compare(simulator_variables[i]->GetName()) == 0) {
						simulator_variables[i]->AddListener(this);
						break;
					}
				}

			} else	if (request->getName() == DBGData::VAR_UNLISTEN) {

				string targetVar = request->getTarget();

				for (int i=0; i < simulator_variables.size(); i++) {

					if (targetVar.compare(simulator_variables[i]->GetName()) == 0) {

						simulator_variables[i]->RemoveListener(this);

						break;
					}

				}

			} else	if (request->getName() == DBGData::VAR_READ) {

				string targetVar = request->getTarget();

				for (int i=0; i < simulator_variables.size(); i++) {

					if (targetVar.compare(simulator_variables[i]->GetName()) == 0) {

						DBGData *response = new DBGData(DBGData::VAR_READ_RESPONSE);

						response->setSimTime(GetSimTime());

						response->setInitiator(simulator_variables[i]->GetName());
						response->setInitiatorSite("simulator");
						response->setTarget(simulator_variables[i]->GetName());
						response->setTargetSite(request->getInitiatorSite());

						if (strcmp(simulator_variables[i]->GetDataTypeName(), "double precision floating-point") == 0) {
							double val = *(simulator_variables[i]);

							response->addAttribute("value", stringify(val));

						}
						else if (strcmp(simulator_variables[i]->GetDataTypeName(), "single precision floating-point") == 0) {
							float val = *(simulator_variables[i]);
							response->addAttribute("value", stringify(val));
						}
						else if (strcmp(simulator_variables[i]->GetDataTypeName(), "boolean") == 0) {
							bool val = *(simulator_variables[i]);
							response->addAttribute("value", stringify(val));
						}
						else {
							uint64_t val = *(simulator_variables[i]);
							response->addAttribute("value", stringify(val));
						}

						responseThread->addData(response);

						break;
					}
				}

			} else if (request->getName() == DBGData::VAR_WRITE) {

				string targetVar = request->getTarget();

				string value = request->getAttribute("value");

				for (int i=0; i < simulator_variables.size(); i++) {

					if (targetVar.compare(simulator_variables[i]->GetName()) == 0) {

						if (strcmp(simulator_variables[i]->GetDataTypeName(), "double precision floating-point") == 0) {

							*(simulator_variables[i]) = convertTo<double>(value);

						}
						else if (strcmp(simulator_variables[i]->GetDataTypeName(), "single precision floating-point") == 0) {

							*(simulator_variables[i]) = convertTo<float>(value);

						}
						else if (strcmp(simulator_variables[i]->GetDataTypeName(), "boolean") == 0) {

							*(simulator_variables[i]) = value.compare("false");
						}
						else {

							*(simulator_variables[i]) = convertTo<uint64_t>(value);

						}

						break;
					}
				}


			} else {
				cerr << "PIM-Target UNKNOWN command => " << std::endl;
			}

			if (request) { delete request; request = NULL; }

		}

	}

	cerr << "********** PIM-Target EXIT THREAD ********* " << std::endl;

}

void PIMThread::VariableBaseNotify(const VariableBase *var) {

	DBGData *response = new DBGData(DBGData::VAR_LISTEN_RESPONSE);

	response->setSimTime(GetSimTime());

	response->setInitiator(var->GetName());
	response->setInitiatorSite("simulator");
	response->setTarget(var->GetName());
	response->setTargetSite("_who_initiate_the_listener_");

	if (strcmp(var->GetDataTypeName(), "double precision floating-point") == 0) {
		double val = *(var);
		response->addAttribute("value", stringify(val));
	}
	else if (strcmp(var->GetDataTypeName(), "single precision floating-point") == 0) {
		float val = *(var);
		response->addAttribute("value", stringify(val));
	}
	else if (strcmp(var->GetDataTypeName(), "boolean") == 0) {
		bool val = *(var);
		response->addAttribute("value", stringify(val));
	}
	else {
		uint64_t val = *(var);
		response->addAttribute("value", stringify(val));
	}

	responseThread->addData(response);

}

} // end pim
} // end service
} // end unisim

