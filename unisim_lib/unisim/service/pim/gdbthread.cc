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

	switch (data.getName()) {
		case DBGData::TERMINATE: {
			os << "Req./Res.=\"" << data.getName() << "\"";
		} break;
		case DBGData::VAR_READ: {
			os << "InitiatorSite=\"" << data.getInitiatorSite() << "\" : Initiator=\"" << data.getInitiator() << "\" : : Target=\"" << data.getTarget()  << "\" : Req./Res.=\"" << data.getName() << "\"";
		} break;
		case DBGData::VAR_WRITE: {
			os << "InitiatorSite=\"" << data.getInitiatorSite() << "\" : Initiator=\"" << data.getInitiator() << "\" : : Target=\"" << data.getTarget()  << "\" : Req./Res.=\"" << data.getName() << "\"";
			os << " : Value=\"" << data.getAttribute("value") << "\"";
		} break;
		case DBGData::VAR_LISTEN: {
			os << "InitiatorSite=\"" << data.getInitiatorSite() << "\" : Initiator=\"" << data.getInitiator() << "\" : : Target=\"" << data.getTarget()  << "\" : Req./Res.=\"" << data.getName() << "\"";
		} break;
		case DBGData::VAR_UNLISTEN: {
			os << "InitiatorSite=\"" << data.getInitiatorSite() << "\" : Initiator=\"" << data.getInitiator() << "\" : : Target=\"" << data.getTarget()  << "\" : Req./Res.=\"" << data.getName() << "\"";
		} break;
		case DBGData::UNKNOWN: {
			os << "Req./Res.=\"" << data.getName() << "\"";
		} break;
	}

	os << endl;

	return os;
}

} // pim namespace
} //service namespace
} // unisim namespace




