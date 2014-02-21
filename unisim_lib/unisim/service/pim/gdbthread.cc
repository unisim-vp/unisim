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

const string GDBThread::SEGMENT_SEPARATOR = ";";
const string GDBThread::FIELD_SEPARATOR = ":";

std::ostream& operator << (std::ostream& os, DBGData& data) {

	switch (data.getCommand()) {
		case DBGData::TERMINATE: {
			os << "Req./Res.=\"TERMINATE\"";
		} break;
		case DBGData::VAR_READ: {
			os << "InitiatorSite=\"" << data.getInitiatorSite() << "\" : Initiator=\"" << data.getInitiator() << "\" : : Target=\"" << data.getTarget()  << "\" : Req./Res.=\"READ\"";
		} break;
		case DBGData::VAR_WRITE: {
			os << "InitiatorSite=\"" << data.getInitiatorSite() << "\" : Initiator=\"" << data.getInitiator() << "\" : : Target=\"" << data.getTarget()  << "\" : Req./Res.=\"WRITE\"";
			os << " : Value=\"" << data.getAttribute("value") << "\"";
		} break;
		case DBGData::VAR_LISTEN: {
			os << "InitiatorSite=\"" << data.getInitiatorSite() << "\" : Initiator=\"" << data.getInitiator() << "\" : : Target=\"" << data.getTarget()  << "\" : Req./Res.=\"LISTEN\"";
		} break;
		case DBGData::VAR_UNLISTEN: {
			os << "InitiatorSite=\"" << data.getInitiatorSite() << "\" : Initiator=\"" << data.getInitiator() << "\" : : Target=\"" << data.getTarget()  << "\" : Req./Res.=\"UNLISTEN\"";
		} break;
		case DBGData::UNKNOWN: {
			os << "Req./Res.=\"" << data.getCommand() << "\"";
		} break;
	}

	os << endl;

	return os;
}

} // pim namespace
} //service namespace
} // unisim namespace




