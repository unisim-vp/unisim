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

using unisim::kernel::service::Simulator;
using unisim::kernel::service::VariableBase;

PIMThread::PIMThread(const char *_name, Object *_parent) :
	SocketThread()
	, Object(_name, _parent)
	, name(string(_name))
{

}

PIMThread::~PIMThread() {

	pim_trace_file.close();
}

double PIMThread::GetSimTime() {
	return Object::GetSimulator()->GetSimTime();
}

void PIMThread::run(){

	pim_trace_file.open ("pim_trace.xls");


	cerr << "PIM::TargetThread start RUN " << std::endl;

	// Explore Simulator Variables to Generate PIM XML File
	vector<VariableBase*> simulator_variables;

	std::list<VariableBase *> lst;

	Simulator::simulator->GetRegisters(lst);

	for (std::list<VariableBase *>::iterator it = lst.begin(); it != lst.end(); it++) {

		if (!((VariableBase *) *it)->IsVisible()) continue;

		simulator_variables.push_back((VariableBase *) *it);

	}

	lst.clear();

	while (!super::isTerminated()) {

		string buf_str;

		if (!GetPacket(buf_str, blocking)) {
			if (blocking) {
				cerr << "PIM-Target receive **NULL**" << endl;
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

//		cerr << "PIM-Target receive " << buffer << std::endl;

		if ((buf_str.compare("EOS") == 0) || (super::isTerminated())) {
			super::stop();
		} else {

// qRcmd,cmd;var_name[:value]{;var_name[:value]}
			int start_index = 0;
			int end_index = buf_str.find(',');
			string qRcmd = buf_str.substr(start_index, end_index-start_index);

			start_index = end_index+1;

			end_index = buf_str.find(';', start_index);
			string cmd = buf_str.substr(start_index, end_index-start_index);

			start_index = end_index+1;

			if (cmd.compare("read") == 0) {

				std::ostringstream os;

				os << GetSimTime() << ";";

				do {

					string name = buf_str.substr(start_index, end_index-start_index);
					start_index = end_index+1;

					for (int i=0; i < simulator_variables.size(); i++) {
						if (name.compare(simulator_variables[i]->GetName()) == 0) {

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

					end_index = buf_str.find(';', start_index);
					if (end_index != string::npos) {
						start_index = end_index+1;
					}
				} while (end_index != string::npos);

				std::string str = os.str();

				PutPacket(str);

				os.str(std::string());


			} else if (cmd.compare("write") == 0) {

				end_index = buf_str.find(':');
				string name = buf_str.substr(start_index, end_index-start_index);
				start_index = end_index+1;

				string value = buf_str.substr(start_index);

				for (int i=0; i < simulator_variables.size(); i++) {
					if (name.compare(simulator_variables[i]->GetName()) == 0) {

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
				cerr << "PIM-Target UNKNOWN command => " << buf_str << std::endl;
			}

		}

	}

	cerr << "********** PIM-Target EXIT THREAD ********* " << std::endl;

}


} // end pim
} // end service
} // end unisim

