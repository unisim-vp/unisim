/*
 * gdbthread.cc
 *
 *  Created on: 10 févr. 2014
 *      Author: rnouacer
 */

#include <unisim/service/pim/gdbthread.hh>

namespace unisim {
namespace service {
namespace pim {

using namespace std;

const char* DBGData::DEFAULT_MASTER = "DBG";
const char* DBGData::DEFAULT_MASTER_SITE = "workbench";
const char* DBGData::DEFAULT_SLAVE = "CPU";
const char* DBGData::DEFAULT_SLAVE_SITE = "simulator";
const char* DBGData::NAME_ATTR = "name";
const char* DBGData::VALUE_ATTR = "value";
const char* DBGData::REG_NUM_ATTR = "reg_num";
const char* DBGData::ADDRESS_ATTR = "address";
const char* DBGData::SIZE_ATTR = "size";
const char* DBGData::TYPE_ATTR = "type";
const char* DBGData::FILE_NAME_ATTR = "filename";
const char* DBGData::FILE_COLUMN_ATTR = "column";
const char* DBGData::FILE_LINE_ATTR = "line";


const string GDBThread::SEGMENT_SEPARATOR = ";";
const string GDBThread::FIELD_SEPARATOR = ":";

std::ostream& operator << (std::ostream& os, DBGData& data) {

	switch (data.getCommand()) {
		case DBGData::TERMINATE: {
			os << "Req./Res.=\"TERMINATE\"";
		} break;
		case DBGData::QUERY_VAR_READ: {
			os << "InitiatorSite=\"" << data.getMasterSite() << "\" : Initiator=\"" << data.getMaster() << "\" : : Target=\"" << data.getSlave()  << "\" : Req./Res.=\"READ\"";
		} break;
		case DBGData::QUERY_VAR_WRITE: {
			os << "InitiatorSite=\"" << data.getMasterSite() << "\" : Initiator=\"" << data.getMaster() << "\" : : Target=\"" << data.getSlave()  << "\" : Req./Res.=\"WRITE\"";
			os << " : Value=\"" << data.getAttribute("value") << "\"";
		} break;
		case DBGData::QUERY_VAR_LISTEN: {
			os << "InitiatorSite=\"" << data.getMasterSite() << "\" : Initiator=\"" << data.getMaster() << "\" : : Target=\"" << data.getSlave()  << "\" : Req./Res.=\"LISTEN\"";
		} break;
		case DBGData::QUERY_VAR_UNLISTEN: {
			os << "InitiatorSite=\"" << data.getMasterSite() << "\" : Initiator=\"" << data.getMaster() << "\" : : Target=\"" << data.getSlave()  << "\" : Req./Res.=\"UNLISTEN\"";
		} break;
		case DBGData::UNKNOWN: {
			os << "Req./Res.=\"" << data.getCommand() << "\"";
		} break;
	}

	os << endl;

	return os;
}

// ----- DBGData --------------

DBGData::DBGData(DBGData::DBGCOMMANDS _command) : command(_command), simTime(0), masterSite(std::string()), master(std::string()), slaveSite(std::string()), slave(std::string()) {

	setMasterSite(DBGData::DEFAULT_MASTER_SITE);
	setMaster(DBGData::DEFAULT_MASTER);
	setSlaveSite(DBGData::DEFAULT_SLAVE_SITE);
	setSlave(DBGData::DEFAULT_SLAVE);

}

DBGData::DBGData(DBGCOMMANDS _command, double _simTime) : command(_command), simTime(_simTime), masterSite(std::string()), master(std::string()), slaveSite(std::string()), slave(std::string()) {
	setMasterSite(DBGData::DEFAULT_MASTER_SITE);
	setMaster(DBGData::DEFAULT_MASTER);
	setSlaveSite(DBGData::DEFAULT_SLAVE_SITE);
	setSlave(DBGData::DEFAULT_SLAVE);
}

DBGData::DBGData(DBGCOMMANDS _name, double _simTime, string _masterSite, string _master, string _slaveSite, string _slave) :
	command(_name), simTime(_simTime), masterSite(_masterSite), master(_master), slaveSite(_slaveSite), slave(_slave) {

}

DBGData::DBGData(DBGCOMMANDS _command, double _simTime, DBGData* refData) : command(_command), simTime(_simTime) {

	setMasterSite(refData->getMasterSite());
	setMaster(refData->getMaster());
	setSlaveSite(refData->getSlaveSite());
	setSlave(refData->getSlave());

}

DBGData::~DBGData() { attributes.clear(); }

DBGData::DBGCOMMANDS DBGData::getCommand() const { return (command); }

bool DBGData::addAttribute(std::string name, std::string value) {

	if (attributes.find(name) == attributes.end()) {
		attributes.insert ( std::pair<std::string, std::string>(name, value) );
	} else {
		return (false);
	}

	return (true);
}

bool DBGData::removeAttribute(std::string name) {

	if (attributes.find(name) == attributes.end()) {
		return (false);
	} else {
		attributes.erase (name);
	}

	return (true);
}

std::string DBGData::getAttribute(std::string name) {

	std::map<std::string, std::string>::iterator it = attributes.find(name);
	if (it == attributes.end()) {
		return (std::string());
	}

	return (it->second);
}

bool DBGData::setAttribute(std::string name, std::string value) {

	std::map<std::string, std::string>::iterator it = attributes.find(name);
	if (it == attributes.end()) {
		return (false);
	} else {
		it->second = value;
	}

	return  (true);
}

std::map<std::string, std::string> DBGData::getAttributes() { return (attributes); }

double DBGData::getSimTime() const {
	return simTime;
}

void DBGData::setSimTime(double simTime) {
	this->simTime = simTime;
}

void DBGData::setMasterSite(string _initiatorSite) { masterSite = _initiatorSite; }
string DBGData::getMasterSite() const { return (masterSite); }

void DBGData::setMaster(string _initiator) { master = _initiator; }
string DBGData::getMaster() const { return (master); }

string DBGData::getSlaveSite() const {
	return slaveSite;
}

void DBGData::setSlaveSite(string _targetSite) {
	this->slaveSite = _targetSite;
}

void DBGData::setSlave(string _target) { slave = _target; }
string DBGData::getSlave() const { return (slave); }


// ---- GDBThread  ---------------

GDBThread::GDBThread(const char *_name, Object *_parent):
	SocketThread()
	, name(string(_name))
	, receiveDataQueue(NULL)
	, sendDataQueue(NULL)

{
	receiveDataQueue = new BlockingCircularQueue<DBGData*, QUEUE_SIZE>();
	sendDataQueue = new BlockingCircularQueue<DBGData*, QUEUE_SIZE>();

	receiver = new ReceiveThread(this, receiveDataQueue);
	sender = new SendThread(this, sendDataQueue);

}

GDBThread::~GDBThread() {

	delete receiveDataQueue; receiveDataQueue = NULL;
	delete sendDataQueue; sendDataQueue = NULL;
	delete receiver; receiver = NULL;
	delete sender; sender = NULL;
}

bool GDBThread::isData() {
	return (!receiveDataQueue->isEmpty());
}

DBGData* GDBThread::receiveData() {
	return (receiveDataQueue->next());
}

void GDBThread::sendData(DBGData* data) {
	sendDataQueue->add(data);
}

void GDBThread::run() {

	waitConnection();

	receiver->start();
	sender->start();

}

GDBThread::ReceiveThread::ReceiveThread(GDBThread *_parent, BlockingCircularQueue<DBGData*, QUEUE_SIZE> *_dataQueue) : GenericThread(), parent(_parent), dataQueue(_dataQueue) {}
GDBThread::ReceiveThread::~ReceiveThread() {}

void GDBThread::ReceiveThread::run(){

//	while (!parent->isTerminated()) {
	while (!parent->isTerminated() && !isTerminated()) {

		string buf_str;

//		parent->lockSocket();

//		while (parent->GetPacketWithAck(buf_str, false, false)) {
		while (parent->GetPacketWithAck(buf_str, true, false)) {

			unsigned int pos = 0;
			unsigned int len = buf_str.length();

			if ((buf_str.compare("EOS") == 0) || (super::isTerminated())) {
				cout << "gdbThread:: Receive TERMINATE" << endl;

				DBGData *request = new DBGData(DBGData::TERMINATE);
				dataQueue->add(request);

			} else {

				if (buf_str.compare("DBG_SUSPEND") == 0) {
					DBGData *request = new DBGData(DBGData::DBG_SUSPEND);
					dataQueue->add(request);
				}
				else {
					switch(buf_str[pos++])
					{
						case 'g': {
							DBGData *request = new DBGData(DBGData::DBG_READ_REGISTERS);
							dataQueue->add(request);

						}

							break;

						case 'p': {
							DBGData *request = new DBGData(DBGData::DBG_READ_SELECTED_REGISTER);

							unsigned int reg_num;
							if(!ParseHex(buf_str, pos, reg_num)) break;
							request->addAttribute(DBGData::REG_NUM_ATTR, stringify(reg_num));
							dataQueue->add(request);

						}

							break;

						case 'G': {
							DBGData *request = new DBGData(DBGData::DBG_WRITE_REGISTERS);
							request->addAttribute(DBGData::VALUE_ATTR, buf_str.substr(1));
							dataQueue->add(request);

						}
							break;

						case 'P': {
							DBGData *request = new DBGData(DBGData::DBG_WRITE_SELECTED_REGISTER);

							unsigned int reg_num;
							if(!ParseHex(buf_str, pos, reg_num)) break;
							request->addAttribute(DBGData::REG_NUM_ATTR, stringify(reg_num));

							if(buf_str[pos++] != '=') break;
							request->addAttribute(DBGData::VALUE_ATTR, buf_str.substr(pos));

							dataQueue->add(request);

						}
							break;

						case 'm': {
							DBGData *request = new DBGData(DBGData::DBG_READ_MEMORY);

							unsigned int addr;
							if(!ParseHex(buf_str, pos, addr)) break;
							request->addAttribute(DBGData::ADDRESS_ATTR, stringify(addr));

							if(buf_str[pos++] != ',') break;

							unsigned int size;
							if(!ParseHex(buf_str, pos, size)) break;
							request->addAttribute(DBGData::SIZE_ATTR, stringify(size));

							if(pos != len) break;

							dataQueue->add(request);

						}
							break;

						case 'M': {
							DBGData *request = new DBGData(DBGData::DBG_WRITE_MEMORY);

							unsigned int addr;
							if(!ParseHex(buf_str, pos, addr)) break;
							request->addAttribute(DBGData::ADDRESS_ATTR, stringify(addr));

							if(buf_str[pos++] != ',') break;

							unsigned int size;
							if(!ParseHex(buf_str, pos, size)) break;
							request->addAttribute(DBGData::SIZE_ATTR, stringify(size));

							if(buf_str[pos++] != ':') break;

							request->addAttribute(DBGData::VALUE_ATTR, buf_str.substr(pos));

							dataQueue->add(request);

						}

							break;

						case 's': {
							DBGData *request = new DBGData(DBGData::DBG_STEP_INSTRUCTION);

							unsigned int addr;
							if(ParseHex(buf_str, pos, addr)) {
								request->addAttribute(DBGData::ADDRESS_ATTR, stringify(addr));
							}
							dataQueue->add(request);

						}
							break;

						case 'c': {
							DBGData *request = new DBGData(DBGData::DBG_CONTINUE);

							unsigned int addr;
							if(ParseHex(buf_str, pos, addr)) {
								request->addAttribute(DBGData::ADDRESS_ATTR, stringify(addr));
							}
							dataQueue->add(request);

						}
							break;

						case 'H': {
							DBGData *request = new DBGData(DBGData::UNKNOWN);
							dataQueue->add(request);

						}
							break;

						case 'k': {
							DBGData *request = new DBGData(DBGData::DBG_KILL_COMMAND);
							dataQueue->add(request);
						}
							break;

						case 'R': {
							DBGData *request = new DBGData(DBGData::DBG_RESET_COMMAND);
							dataQueue->add(request);

						}
							break;

						case '?': {
							DBGData *request = new DBGData(DBGData::DBG_GET_LAST_SIGNAL);
							dataQueue->add(request);

						}
							break;

						case '!': {
							DBGData *request = new DBGData(DBGData::DBG_ENABLE_EXTENDED_MODE);
							dataQueue->add(request);

						}
							break;

						case 'Z': {
							DBGData *request = new DBGData(DBGData::DBG_SET_BREAKPOINT_WATCHPOINT);

							unsigned int type;
							unsigned int addr;
							unsigned int size;
							if(!ParseHex(buf_str, pos, type)) break;
							if(buf_str[pos++] != ',') break;
							if(!ParseHex(buf_str, pos, addr)) break;
							if(buf_str[pos++] != ',') break;
							if(!ParseHex(buf_str, pos, size)) break;

							request->addAttribute(DBGData::TYPE_ATTR, stringify(type));
							request->addAttribute(DBGData::ADDRESS_ATTR, stringify(addr));
							request->addAttribute(DBGData::SIZE_ATTR, stringify(size));

							dataQueue->add(request);

						}
							break;

						case 'z': {
							DBGData *request = new DBGData(DBGData::DBG_REMOVE_BREAKPOINT_WATCHPOINT);

							unsigned int type;
							unsigned int addr;
							unsigned int size;
							if(!ParseHex(buf_str, pos, type)) break;
							if(buf_str[pos++] != ',') break;
							if(!ParseHex(buf_str, pos, addr)) break;
							if(buf_str[pos++] != ',') break;
							if(!ParseHex(buf_str, pos, size)) break;

							request->addAttribute(DBGData::TYPE_ATTR, stringify(type));
							request->addAttribute(DBGData::ADDRESS_ATTR, stringify(addr));
							request->addAttribute(DBGData::SIZE_ATTR, stringify(size));

							dataQueue->add(request);
						}
							break;

						default:

							if(buf_str.substr(0, 9) == "qSymbol::")
							{
								DBGData *response =  new DBGData(DBGData::QUERY_SYMBOL_ACCEPT);
								parent->sendData(response);

								while (true) {

									string packet;
									bool	found_error = false;

//									if(parent->GetPacketWithAck(packet, false, false))
									if(parent->GetPacketWithAck(packet, true, false))
									{
										if (packet.compare("OK") == 0) {
											break;
										} else if (packet.substr(0, 8).compare("qSymbol:") == 0) {
											vector<string> segments;

											stringSplit(packet, ":", segments);

											string name;
											string value;

											if (segments.size() == 2) // read request
											{
												hexToText(segments[1].c_str(), segments[1].size(), name);

												DBGData *request;
												if (name.compare("*") == 0) {
													request = request = new DBGData(DBGData::QUERY_SYMBOL_READ_ALL);
												} else {
													request = new DBGData(DBGData::QUERY_SYMBOL_READ);
													request->addAttribute(DBGData::NAME_ATTR, name);
												}
												dataQueue->add(request);
											}
											else if (segments.size() == 3) // write_symbol request
											{
												hexToText(segments[2].c_str(), segments[2].size(), name);

												DBGData *request =  new DBGData(DBGData::QUERY_SYMBOL_WRITE);
												request->addAttribute(DBGData::NAME_ATTR, name);
												request->addAttribute(DBGData::VALUE_ATTR, segments[1]);
												dataQueue->add(request);
											}
											else {
												found_error = true;
											}

										} else {
											found_error = true;
										}

										if (found_error) {
//											if(verbose)
//											{
//												logger << DebugWarning << "HandleSymbolLookup:: unknown command (" << packet << ")" << EndDebugWarning;
//											}
										}
									}
								}

							}
							else if(buf_str == "vCont?")
							{
								DBGData *request = new DBGData(DBGData::DBG_VERBOSE_RESUME_ACTIONS);
								dataQueue->add(request);
							}
							else if((buf_str.substr(0, 7) == "vCont;c") || (buf_str.substr(0, 7) == "vCont;C"))
							{
								DBGData *request = new DBGData(DBGData::DBG_VERBOSE_RESUME_CONTINUE);
								dataQueue->add(request);

							}
							else if((buf_str.substr(0, 7) == "vCont;s") || (buf_str.substr(0, 7) == "vCont;s"))
							{

								DBGData *request = new DBGData(DBGData::DBG_VERBOSE_RESUME_STEP);
								dataQueue->add(request);
							}
							else if(buf_str.substr(0, 6) == "qRcmd,")
							{
								HandleQRcmd(buf_str.substr(6));
							}
							else
							{
								DBGData *request = new DBGData(DBGData::DBG_UNKNOWN);
								dataQueue->add(request);
							}

							break;

					} // end of switch
				}
			}
		}

//		parent->unlockSocket();
//
//#ifdef WIN32
//		Sleep(1);
//#else
//		usleep(1000);
//#endif

	}

}

void GDBThread::ReceiveThread::HandleQRcmd(string command) {

	// TODO: rewrite the string parsing to uniforms with the rest of the code

	// qRcmd,cmd;var_name[:value]{;var_name[:value]}

	int start_index = 0;
	int end_index = command.find(';', start_index);
	string cmdPrefix = command.substr(start_index, end_index-start_index);

	start_index = end_index+1;

	if (cmdPrefix.compare("var_listen") == 0) {

		do {

			DBGData *request = new DBGData(DBGData::QUERY_VAR_LISTEN);

			string name = command.substr(start_index, end_index-start_index);

			request->setMasterSite("workbench");
			request->setMaster(name);
			request->setSlaveSite("simulator");
			request->setSlave(name);

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
			DBGData *request = new DBGData(DBGData::QUERY_VAR_UNLISTEN);

			string name = command.substr(start_index, end_index-start_index);

			request->setMasterSite("workbench");
			request->setMaster(name);
			request->setSlaveSite("simulator");
			request->setSlave(name);

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

			DBGData *request = new DBGData(DBGData::QUERY_VAR_READ);

			string name = command.substr(start_index, end_index-start_index);

			request->setMasterSite("workbench");
			request->setMaster(name);
			request->setSlaveSite("simulator");
			request->setSlave(name);

			dataQueue->add(request);

			start_index = end_index+1;

			end_index = command.find(';', start_index);
			if (end_index != string::npos) {
				start_index = end_index+1;
			}
		} while (end_index != string::npos);

	}
	else if (cmdPrefix.compare("var_write") == 0) {

		DBGData *request = new DBGData(DBGData::QUERY_VAR_WRITE);

		end_index = command.find(':');

		string name = command.substr(start_index, end_index-start_index);
		start_index = end_index+1;

		request->setMasterSite("workbench");
		request->setMaster(name);
		request->setSlaveSite("simulator");
		request->setSlave(name);

		string value = command.substr(start_index);

		request->addAttribute("value", value);

		dataQueue->add(request);

	} else {

		// TODO: rewrite the string parsing to uniforms with the rest of the code

		unsigned int separator_index = command.find_first_of(':');
		string cmdPrefix;

		if (separator_index == static_cast<unsigned int>(string::npos)) {
			cmdPrefix = command;
		} else {
			cmdPrefix = command.substr(0, separator_index);
		}

		if (cmdPrefix.compare("parameters") == 0) {
			/*
			 * return parametersNumber
			 *        "Otype:name:dataType:value:format:description
			 */
			DBGData *request = new DBGData(DBGData::QUERY_PARAMETERS);
			dataQueue->add(request);

		}
		else if (cmdPrefix.compare("parameter") == 0) {

			DBGData *request = new DBGData(DBGData::QUERY_PARAMETER);

			std::string str = command.substr(separator_index+1);

			separator_index = str.find_first_of(':');
			string name;
			if (separator_index == static_cast<unsigned int>(string::npos)) {	  // parameter read request
				name = str;
			} else {		  // parameter write request
				name = str.substr(0, separator_index);

				std::string value = str.substr(separator_index+1);

				request->addAttribute(DBGData::VALUE_ATTR, value);
			}

			request->addAttribute(DBGData::NAME_ATTR, name);

			dataQueue->add(request);

		}
		else if (cmdPrefix.compare("symboles") == 0) {

			/*
			 * return symbNumber
			 *        {"OsymbName:symbAddress:symbSize:symbType"}
			 */
			DBGData *request = new DBGData(DBGData::QUERY_SYMBOLES);

			dataQueue->add(request);

		}
		else if (cmdPrefix.compare("disasm") == 0) {
			/**
			 * command: qRcmd, disasm:address:size
			 *
			 * return disasmSize
			 *        "O<next_address>
	         *        "O<disassembled code>"
			 *
			 */

			DBGData *request = new DBGData(DBGData::QUERY_DISASM);

			if (separator_index != string::npos) {
				separator_index++;
				unsigned int symbol_address;
				unsigned int symbol_size;

				if(ParseHex(command, separator_index, symbol_address)) {
					request->addAttribute(DBGData::ADDRESS_ATTR, stringify(symbol_address));
				}

				if(command[separator_index++] == ':') {
					if(ParseHex(command, separator_index, symbol_size)) {
						request->addAttribute(DBGData::SIZE_ATTR, stringify(symbol_size));
					}
				}
			}

			dataQueue->add(request);

		}
		else if (cmdPrefix.compare("start-pim") == 0) {

			DBGData *request = new DBGData(DBGData::QUERY_START_PIM);

			dataQueue->add(request);

		}
		else if (cmdPrefix.compare("time") == 0) {
			DBGData *request = new DBGData(DBGData::QUERY_TIME);

			dataQueue->add(request);

		}
		else if (cmdPrefix.compare("endian") == 0) {
			DBGData *request = new DBGData(DBGData::QUERY_ENDIAN);

			dataQueue->add(request);

		}
		else if (cmdPrefix.compare("registers") == 0) {
			DBGData *request = new DBGData(DBGData::QUERY_REGISTERS);

			dataQueue->add(request);

		}
		else if (cmdPrefix.compare("stack") == 0) {
			DBGData *request = new DBGData(DBGData::QUERY_STACK);

			dataQueue->add(request);

		}
		else if (cmdPrefix.compare("physicalAddress") == 0) {

			DBGData *request = new DBGData(DBGData::QUERY_PHYSICAL_ADDRESS);

			long logical_address;

			int start_index = 0;
			int end_index = command.find(':', start_index);
			if(end_index != string::npos) {

				string cmd = command.substr(start_index, end_index-start_index);
				start_index = end_index+1;

				string hex = command.substr(start_index);
				start_index = end_index+1;

				request->addAttribute(DBGData::ADDRESS_ATTR, hex);

			}

			dataQueue->add(request);

		}
		else if (cmdPrefix.compare("structuredAddress") == 0) {

			DBGData *request = new DBGData(DBGData::QUERY_STRUCTURED_ADDRESS);

			long logical_address;

			int start_index = 0;
			int end_index = command.find(':', start_index);
			if(end_index != string::npos) {

				string cmd = command.substr(start_index, end_index-start_index);
				start_index = end_index+1;

				string hex = command.substr(start_index);
				start_index = end_index+1;

				request->addAttribute(DBGData::ADDRESS_ATTR, hex);
			}

			dataQueue->add(request);

		}
		else if (cmdPrefix.compare("srcaddr") == 0) {

			// qRcmd,srcaddr:filename;lineno;colno
			DBGData *request = new DBGData(DBGData::QUERY_SRCADDR);

			int start_index = separator_index + 1;
			int end_index = command.find(';');
			if(end_index != string::npos) {
				string source_filename = command.substr(start_index, end_index-start_index);
				request->addAttribute(DBGData::FILE_NAME_ATTR, source_filename);

				start_index = end_index+1;

				end_index = command.find(';', start_index);
				if(end_index != string::npos) {
					string lineno = command.substr(start_index, end_index-start_index);
					request->addAttribute(DBGData::FILE_LINE_ATTR, lineno);

					start_index = end_index+1;

					end_index = command.find(';', start_index);
					if(end_index != string::npos) {
						string colno = command.substr(start_index, end_index-start_index);
						request->addAttribute(DBGData::FILE_COLUMN_ATTR, colno);

						start_index = end_index+1;

					}
				}

			}

			dataQueue->add(request);

		}
		else if (cmdPrefix.compare("statistics") == 0) {
			DBGData *request = new DBGData(DBGData::QUERY_STATISTICS);

			dataQueue->add(request);

		} else {
			DBGData *response = new DBGData(DBGData::DBG_ERROR_MALFORMED_REQUEST);
			parent->sendData(response);
		}

	}

}


GDBThread::SendThread::SendThread(GDBThread *_parent, BlockingCircularQueue<DBGData*, QUEUE_SIZE> *_dataQueue) : GenericThread(), parent(_parent), dataQueue(_dataQueue) {}
GDBThread::SendThread::~SendThread() {}

void GDBThread::SendThread::run(){

//	while (!parent->isTerminated()) {
	while (!parent->isTerminated() && !isTerminated()) {

		DBGData* response = dataQueue->next();

		switch (response->getCommand()) {
			case DBGData::DBG_OK_RESPONSE: {
				bool result = parent->OutputTextWithAck("OK", 2, false);

				if (response) { delete response; response = NULL; }

				continue;
			} break;
			case DBGData::DBG_NOK_RESPONSE: {
				bool result = parent->OutputTextWithAck("NOK", 2, false);

				if (response) { delete response; response = NULL; }

				continue;
			} break;
			case DBGData::DBG_ERROR_MALFORMED_REQUEST: {
				bool result = parent->OutputTextWithAck("E00", 3, false);

				if (response) { delete response; response = NULL; }

				continue;
			} break;
			case DBGData::DBG_ERROR_READING_DATA_EPERM: {
				bool result = parent->OutputTextWithAck("E01", 3, false);

				if (response) { delete response; response = NULL; }

				continue;
			} break;
			case DBGData::DBG_KILL_COMMAND: {

				parent->stop();

				if (response) { delete response; response = NULL; }

				continue;
			} break;
			case DBGData::DBG_PROCESS_EXIT: {
				parent->PutPacketWithAck("W00", false);

				parent->stop();

				if (response) { delete response; response = NULL; }

				continue;

			} break;
			case DBGData::DBG_REPORT_STOP: {
				string packet = "S" + response->getAttribute(DBGData::VALUE_ATTR);

				parent->PutPacketWithAck(packet, false);

				if (response) { delete response; response = NULL; }

				continue;

			} break;
			case DBGData::DBG_REPORT_EXTENDED_STOP: {
				string packet("T05");

				parent->PutPacketWithAck(packet, false);

				if (response) { delete response; response = NULL; }

				continue;

			} break;
			case DBGData::DBG_READ_WATCHPOINT: {
				string packet("T05");

				std::stringstream sstr;
				sstr << "rwatch";

				sstr << ":" << response->getAttribute(DBGData::ADDRESS_ATTR);

				packet += sstr.str();
				packet += ";";

				sstr.str(std::string());

				parent->PutPacketWithAck(packet, false);

				if (response) { delete response; response = NULL; }

				continue;

			} break;
			case DBGData::DBG_WRITE_WATCHPOINT: {
				string packet("T05");

				std::stringstream sstr;
				sstr << "watch";

				sstr << ":" << response->getAttribute(DBGData::ADDRESS_ATTR);

				packet += sstr.str();
				packet += ";";

				sstr.str(std::string());

				parent->PutPacketWithAck(packet, false);

				if (response) { delete response; response = NULL; }

				continue;

			} break;
			case DBGData::DBG_READ_REGISTERS: {
				string packet = response->getAttribute(DBGData::VALUE_ATTR);
				bool result = parent->PutPacketWithAck(packet, false);

				if (response) { delete response; response = NULL; }

//				if(!result)
//				{
//					Object::Stop(0);
//					return (DebugControl<ADDRESS>::DBG_KILL);
//				}

				continue;

			} break;
			case DBGData::DBG_READ_SELECTED_REGISTER: {

				string packet = response->getAttribute(DBGData::VALUE_ATTR);

				parent->PutPacketWithAck(packet, false);

				if (response) { delete response; response = NULL; }

				continue;
			} break;
			case DBGData::QUERY_SYMBOL_ACCEPT: {
				stringstream os;
				os << "qSymbol::";

				std::string str = os.str();

				bool result = parent->PutPacketWithAck(str, false);

				os.str(std::string());

				if (response) { delete response; response = NULL; }

				if (!result) {
					exit(-1);
					break;
				}

			} break;
			case DBGData::QUERY_SYMBOL_READ: {

				string packet = "";
				packet.append("qSymbol:");
				packet.append(response->getAttribute(DBGData::VALUE_ATTR));
				packet.append(":");
				packet.append(response->getAttribute(DBGData::NAME_ATTR));

				parent->PutPacketWithAck(packet, false);

				if (response) { delete response; response = NULL; }

				continue;

			} break;
			case DBGData::QUERY_SYMBOL_READ_ALL: {

				if (response) { delete response; response = NULL; }

				continue;

			} break;
			case DBGData::QUERY_SYMBOL_WRITE: {

			} break;
			case DBGData::QUERY_SYMBOL: {
				string packet = "qSymbol:";
				parent->PutPacketWithAck(packet, false);

				if (response) { delete response; response = NULL; }

				continue;

			} break;
			case DBGData::DBG_READ_MEMORY: {
				string packet = response->getAttribute(DBGData::VALUE_ATTR);
				parent->PutPacketWithAck(packet, false);

				if (response) { delete response; response = NULL; }

				continue;

			} break;
			case DBGData::DBG_VERBOSE_RESUME_ACTIONS: {
				stringstream os;

				os << "vCont;" << response->getAttribute(DBGData::VALUE_ATTR);

				std::string str = os.str();

				bool result = parent->PutPacketWithAck(str, false);

				os.str(std::string());

				if (response) { delete response; response = NULL; }

				if (!result) {
					exit(-1);
					break;
				}

			} break;
			case DBGData::QUERY_PARAMETERS: {

				std::map<std::string, std::string> attributes = response->getAttributes();

				parent->PutPacketWithAck(stringify(attributes.size()), false);

				for (std::map<std::string, std::string>::iterator it=attributes.begin(); it!=attributes.end(); ++it) {

					string str = it->second;

					parent->OutputTextWithAck(str.c_str(), str.size(), false);
				}

				if (response) { delete response; response = NULL; }

				continue;

			} break;
			case DBGData::QUERY_PARAMETER: {

				std::string value = response->getAttribute(DBGData::VALUE_ATTR);
				parent->OutputTextWithAck(value.c_str(), value.size(), false);

				if (response) { delete response; response = NULL; }

				continue;

			} break;
			case DBGData::QUERY_SYMBOLES: {
				/**
				 * command: qRcmd, symboles
				 * return "symbName:symbAddress:symbSize:symbType"
				 *
				 * with: symbType in {"FUNCTION", "VARIABLE"}
				 */

				std::map<std::string, std::string> attributes = response->getAttributes();

				parent->PutPacketWithAck(stringify(attributes.size()), false);

				for (std::map<std::string, std::string>::iterator it=attributes.begin(); it!=attributes.end(); ++it) {

					string str = it->second;

					parent->OutputTextWithAck(str.c_str(), str.size(), false);
				}

				if (response) { delete response; response = NULL; }

				continue;

			} break;
			case DBGData::QUERY_DISASM: {
				std::map<std::string, std::string> attributes = response->getAttributes();

				parent->PutPacketWithAck(stringify(attributes.size()), false);

				for (std::map<std::string, std::string>::iterator it=attributes.begin(); it!=attributes.end(); ++it) {

					string str = it->second;

					parent->OutputTextWithAck(str.c_str(), str.size(), false);
				}

				if (response) { delete response; response = NULL; }

				continue;

			} break;
			case DBGData::QUERY_TIME: {

				parent->PutPacketWithAck(stringify(response->getSimTime()), false);

				if (response) { delete response; response = NULL; }

				continue;

			} break;
			case DBGData::QUERY_ENDIAN: {
				string endian = response->getAttribute(DBGData::VALUE_ATTR);

				parent->PutPacketWithAck(endian, false);

				if (response) { delete response; response = NULL; }

				continue;

			} break;
			case DBGData::QUERY_REGISTERS: {
				string registers = response->getAttribute(DBGData::VALUE_ATTR);

				parent->PutPacketWithAck(registers, false);

				if (response) { delete response; response = NULL; }

				continue;

			} break;
			case DBGData::QUERY_STACK: {
				string stack = response->getAttribute(DBGData::VALUE_ATTR);

				parent->OutputTextWithAck(stack.c_str(), stack.length(), false);

				if (response) { delete response; response = NULL; }

				continue;

			} break;
			case DBGData::QUERY_PHYSICAL_ADDRESS: {
				string address = response->getAttribute(DBGData::ADDRESS_ATTR);

				parent->PutPacketWithAck(address, false);

				if (response) { delete response; response = NULL; }

				continue;

			} break;
			case DBGData::QUERY_STRUCTURED_ADDRESS: {
				string address = response->getAttribute(DBGData::ADDRESS_ATTR);

				parent->PutPacketWithAck(address, false);

				if (response) { delete response; response = NULL; }

				continue;

			} break;
			case DBGData::QUERY_SRCADDR: {
				string address = response->getAttribute(DBGData::ADDRESS_ATTR);

				parent->PutPacketWithAck(address, false);

				if (response) { delete response; response = NULL; }

				continue;

			} break;
			case DBGData::QUERY_STATISTICS: {

				std::map<std::string, std::string> attributes = response->getAttributes();

				std::stringstream sstr;
				if (!attributes.empty()) {
					std::map<std::string, std::string>::iterator it=attributes.begin();
					sstr << it->first << ":" << it->second;
					it++;
					for (; it!=attributes.end(); ++it) {

						sstr << ";" << it->first << ":" << it->second;
					}
				}

				string str = sstr.str();

				parent->OutputTextWithAck(str.c_str(), str.size(), false);

				sstr.str(std::string());

				if (response) { delete response; response = NULL; }

				continue;

			} break;
			case DBGData::UNKNOWN: {
				stringstream os;

				os << "";

				std::string str = os.str();

				bool result = parent->PutPacketWithAck(str, false);

				os.str(std::string());

				if (response) { delete response; response = NULL; }

				if (!result) {
					exit(-1);
					break;
				}

			} break;

			case DBGData::QUERY_VAR_READ: {
				stringstream os;

				os << response->getSimTime() << SEGMENT_SEPARATOR;

				string targetVar = response->getMaster();

				os << targetVar << FIELD_SEPARATOR;

				os << response->getAttribute(DBGData::VALUE_ATTR) << SEGMENT_SEPARATOR;

				std::string str = os.str();

				bool result = parent->PutPacketWithAck(str, false);

				os.str(std::string());

				if (response) { delete response; response = NULL; }

				if (!result) {
					exit(-1);
					break;
				}


			} break;
			case DBGData::QUERY_VAR_LISTEN: {
				stringstream os;

				os << response->getSimTime() << SEGMENT_SEPARATOR;

				string targetVar = response->getMaster();

				os << targetVar << FIELD_SEPARATOR;

				os << response->getAttribute("value") << SEGMENT_SEPARATOR;

				std::string str = os.str();

				bool result = parent->PutPacketWithAck(str, false);

				os.str(std::string());

				if (response) { delete response; response = NULL; }

				if (!result) {
					exit(-1);
					break;
				}

			} break;

			default: {
				cerr << "GDBThread UNKNOWN response => " << response << std::endl;

				if (response) { delete response; response = NULL; }

				continue;
			} break;
		}

	}
}

// -------------------

} // pim namespace
} //service namespace
} // unisim namespace




