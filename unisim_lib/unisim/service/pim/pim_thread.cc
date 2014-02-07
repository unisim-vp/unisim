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

std::ostream& operator << (std::ostream& os, PIMData& data) {

	switch (data.getName()) {
		case PIMData::TERMINATE: {
			os << "Req./Res.=\"" << data.getName() << "\"";
		} break;
		case PIMData::VAR_READ: {
			os << "InitiatorSite=\"" << data.getInitiatorSite() << "\" : Initiator=\"" << data.getInitiator() << "\" : : Target=\"" << data.getTarget()  << "\" : Req./Res.=\"" << data.getName() << "\"";
		} break;
		case PIMData::VAR_WRITE: {
			os << "InitiatorSite=\"" << data.getInitiatorSite() << "\" : Initiator=\"" << data.getInitiator() << "\" : : Target=\"" << data.getTarget()  << "\" : Req./Res.=\"" << data.getName() << "\"";
			os << " : Value=\"" << data.getAttribute("value") << "\"";
		} break;
		case PIMData::VAR_LISTEN: {
			os << "InitiatorSite=\"" << data.getInitiatorSite() << "\" : Initiator=\"" << data.getInitiator() << "\" : : Target=\"" << data.getTarget()  << "\" : Req./Res.=\"" << data.getName() << "\"";
		} break;
		case PIMData::VAR_UNLISTEN: {
			os << "InitiatorSite=\"" << data.getInitiatorSite() << "\" : Initiator=\"" << data.getInitiator() << "\" : : Target=\"" << data.getTarget()  << "\" : Req./Res.=\"" << data.getName() << "\"";
		} break;
		case PIMData::UNKNOWN: {
			os << "Req./Res.=\"" << data.getName() << "\"";
		} break;
	}

	os << endl;

	return os;
}

PIMThread::PIMThread(const char *_name, Object *_parent) :
	SocketThread()
	, VariableBaseListener()
	, Object(_name, _parent)

	, requestThread(NULL)
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

	pim_trace_file.close();

}

void PIMThread::run(){

	requestThread = new RequestThread("request-Thread");
	requestThread->startSocketThread(getSockfd(), blocking);

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

		PIMData* request = requestThread->getRequest();

		if (request->getName() == PIMData::TERMINATE) {
			if (requestThread) {
				requestThread->stop();
			}

			super::stop();
		} else {

			// qRcmd,cmd;var_name[:value]{;var_name[:value]}

			if (request->getName() == PIMData::VAR_LISTEN) {

				string targetVar = request->getTarget();
				for (int i=0; i < simulator_variables.size(); i++) {
					if (targetVar.compare(simulator_variables[i]->GetName()) == 0) {
						simulator_variables[i]->AddListener(this);
						break;
					}
				}

			} else	if (request->getName() == PIMData::VAR_UNLISTEN) {

				string targetVar = request->getTarget();

				for (int i=0; i < simulator_variables.size(); i++) {

					if (targetVar.compare(simulator_variables[i]->GetName()) == 0) {

						simulator_variables[i]->RemoveListener(this);

						break;
					}

				}

			} else	if (request->getName() == PIMData::VAR_READ) {

				std::ostringstream os;

				os << GetSimTime() << ";";

				string targetVar = request->getTarget();

				for (int i=0; i < simulator_variables.size(); i++) {

					if (targetVar.compare(simulator_variables[i]->GetName()) == 0) {

						os << simulator_variables[i]->GetName() << ":";

						if (strcmp(simulator_variables[i]->GetDataTypeName(), "double precision floating-point") == 0) {
							double val = *(simulator_variables[i]);
							os << stringify(val);
						}
						else if (strcmp(simulator_variables[i]->GetDataTypeName(), "single precision floating-point") == 0) {
							float val = *(simulator_variables[i]);
							os << stringify(val);
						}
						else if (strcmp(simulator_variables[i]->GetDataTypeName(), "boolean") == 0) {
							bool val = *(simulator_variables[i]);
							os << stringify(val);
						}
						else {
							uint64_t val = *(simulator_variables[i]);
							os << stringify(val);
						}

						os << ";";

						break;
					}
				}

				std::string str = os.str();

				PutDatagramPacket(str);

				os.str(std::string());

			} else if (request->getName() == PIMData::VAR_WRITE) {

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

		}

	}

	cerr << "********** PIM-Target EXIT THREAD ********* " << std::endl;

}

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
void PIMThread::VariableBaseNotify(const VariableBase *var) {
	std::ostringstream os;

	os << GetSimTime() << ";";

	os << var->GetName() << ":";

	if (strcmp(var->GetDataTypeName(), "double precision floating-point") == 0) {
		double val = *(var);
		os << stringify(val);
	}
	else if (strcmp(var->GetDataTypeName(), "single precision floating-point") == 0) {
		float val = *(var);
		os << stringify(val);
	}
	else if (strcmp(var->GetDataTypeName(), "boolean") == 0) {
		bool val = *(var);
		os << stringify(val);
	}
	else {
		uint64_t val = *(var);
		os << stringify(val);
	}

	os << ";";

	std::string str = os.str();

	PutDatagramPacket(str);

	os.str(std::string());

}

} // end pim
} // end service
} // end unisim

