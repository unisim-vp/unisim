#include "unisim/kernel/logger/logger.hh"
#include <iostream>

namespace unisim {
namespace kernel {
namespace logger {

Logger::Logger(const unisim::kernel::service::Object &_obj) : obj(_obj), buffer(), mode(NO_MODE) {
	server = LoggerServer::GetInstance();
}

Logger&
Logger::operator <<(ostream& (*f)(ostream &)) {
	if(mode == NO_MODE) return *this;
	buffer << f;
	return *this;
}

Logger&
Logger::operator <<(Logger& (*f)(Logger &)) {
	return f(*this);
}

void
Logger::PrintMode() {
	cerr << "Current mode (" << obj.GetName() << "): ";
	switch(mode) {
	case NO_MODE:
		cerr << "NO_MODE";
		break;
	case INFO_MODE:
		cerr << "INFO_MODE";
		break;
	case WARNING_MODE:
		cerr << "WARNING_MODE";
		break;
	case ERROR_MODE:
		cerr << "ERROR_MODE";
		break;
	}
	cerr << endl;
}

void
Logger::DebugInfo() {
	if(mode != NO_MODE) {
		return;
	}
	mode = INFO_MODE;
	buffer.clear();
	buffer.str("");
}

void
Logger::EndDebugInfo() {
	if(mode != INFO_MODE) {
		return;
	}
	mode = NO_MODE;
	server->DebugInfo(obj, buffer.str().c_str());
}

void
Logger::DebugWarning() {
	if(mode != NO_MODE) return;
	mode = WARNING_MODE;
	buffer.clear();
	buffer.str("");
}

void
Logger::EndDebugWarning() {
	if(mode != WARNING_MODE) return;
	mode = NO_MODE;
	server->DebugWarning(obj, buffer.str().c_str());
}

void
Logger::DebugError() {
	if(mode != NO_MODE) return;
	mode = ERROR_MODE;
	buffer.clear();
	buffer.str("");
}

void
Logger::EndDebugError() {
	if(mode != ERROR_MODE) return;
	mode = NO_MODE;
	server->DebugError(obj, buffer.str().c_str());
}

void
Logger::EndDebug() {
	switch(mode) {
	case INFO_MODE:
		EndDebugInfo();
		break;
	case WARNING_MODE:
		EndDebugWarning();
		break;
	case ERROR_MODE:
		EndDebugError();
		break;
	}
}

Logger &
DebugInfo(Logger &l) {
	l.DebugInfo();
	return l;
}

Logger &
EndDebugInfo(Logger &l) {
	l.EndDebugInfo();
	return l;
}

Logger &
DebugWarning(Logger &l) {
	l.DebugWarning();
	return l;
}

Logger &
EndDebugWarning(Logger &l) {
	l.EndDebugWarning();
	return l;
}

Logger &
DebugError(Logger &l) {
	l.DebugError();
	return l;
}

Logger &
EndDebugError(Logger &l) {
	l.EndDebugError();
	return l;
}

Logger &
EndDebug(Logger &l) {
	l.EndDebug();
	return l;
}

} // end of namespace logger
} // end of namespace kernel
} // end of namespace unisim
