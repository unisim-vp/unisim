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

using unisim::kernel::service::Object;
using unisim::kernel::service::Simulator;
using unisim::kernel::service::VariableBase;

PIMThread::PIMThread(string _name) :
		SocketThread(),
		name(_name)
{

}

void PIMThread::Run(){

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

		char *buffer = receive_packet(blocking);

		if (buffer == NULL) {
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

		if (!super::isTerminated()) {

			string buf_str(buffer);

// qRcmd,cmd:var_name:value
			int start_index = 0;
			int end_index = buf_str.find(',');
			string qRcmd = buf_str.substr(start_index, end_index-start_index);

			start_index = end_index+1;

			end_index = buf_str.find(':', start_index);
			string cmd = buf_str.substr(start_index, end_index-start_index);
			start_index = end_index+1;

			if (cmd.compare("read") == 0) {

				end_index = buf_str.find(':', start_index);
				string name = buf_str.substr(start_index, end_index-start_index);
				start_index = end_index+1;

				for (int i=0; i < simulator_variables.size(); i++) {
					if (name.compare(simulator_variables[i]->GetName()) == 0) {

						std::ostringstream os;
						os << simulator_variables[i]->GetName() << ":";

						double val = *(simulator_variables[i]);
						os << stringify(val);

						std::string str = os.str();

						const char *bstr = str.c_str();

//						cerr << name << " send: " << bstr << endl;

						while (true) {
							if (!send_packet(bstr, blocking)) {
								if (blocking) {
									cerr << "PIM-Target unable to send !" << endl;
								} else {
#ifdef WIN32
									Sleep(1);
#else
									usleep(1000);
#endif
									continue;
								}

							}
							break;
						}

						os.str(std::string());

						break;
					}
				}

			} else if (cmd.compare("write") == 0) {

				end_index = buf_str.find(':');
				string name = buf_str.substr(start_index, end_index-start_index);
				start_index = end_index+1;

				string value = buf_str.substr(start_index);

				for (int i=0; i < simulator_variables.size(); i++) {
					if (name.compare(simulator_variables[i]->GetName()) == 0) {

						*(simulator_variables[i]) = convertTo<double>(value);
						break;
					}
				}

			} else {
				cerr << "PIM-Target UNKNOWN command => " << buffer << std::endl;
			}

		}

		if (buffer != NULL) {
			free(buffer);
			buffer = NULL;
		}

	}

}


} // end pim
} // end service
} // end unisim
