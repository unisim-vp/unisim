#include "unisim/kernel/logger/logger_server.hh"

namespace unisim {
namespace kernel {
namespace logger {

LoggerServer *LoggerServer::singleton = 0;

LoggerServer::
LoggerServer() :
	Object("kernel_logger", 0),
	opt_std_err(false),
	opt_std_out(false),
	opt_std_color(false),
	opt_file(false),
	opt_filename("logger_output.txt"),
	opt_xml_file(false),
	opt_xml_filename("logger_output.xml"),
	param_std_err("std_err", this, opt_std_err, "Show logger output through the standard error output"),
	param_std_out("std_out", this, opt_std_out, "Show logger output through the standard output"),
	param_std_color("std_color", this, opt_std_color, "Colorize logger output through the standard output and standard error output"),
	param_file("file", this, opt_file, "Keep logger output in a file"),
	param_filename("filename", this, opt_filename, "Filename to keep logger output (the option file must be activated)"),
	param_xml_file("xml_file", this, opt_xml_file, "Keep logger output in a file xml formatted"),
	param_xml_filename("xml_filename", this, opt_xml_filename, "Filename to keep logger xml output (the option xml_file must be activated)") {

}

LoggerServer::
~LoggerServer() {
}

LoggerServer *
LoggerServer::
GetInstance() {
	if(singleton == 0) {
		singleton = new LoggerServer();
	}
	return singleton;
}

void
LoggerServer::
DebugInfo(const unisim::kernel::service::Object &obj, const char *buffer) {
	if(opt_std_out) {
		if(opt_std_color) {
			std::cout << "\033[36;40m";
 			std::cout << obj.GetName() << ":" << endl << buffer << endl;
			std::cout << "\033[37;40m";
		} else {
			std::cout << "DebugInfo(" << obj.GetName() << "):" << endl << buffer << endl;
		}
	}
	if(opt_std_err) {
		if(opt_std_color) {
			std::cerr << "\033[36;40m";
 			std::cerr << obj.GetName() << ":" << endl << buffer << endl;
			std::cerr << "\033[37;40m";
		} else {
			std::cerr << "DebugInfo(" << obj.GetName() << "):" << endl << buffer << endl;
		}
	}
}

void 
LoggerServer::
DebugWarning(const unisim::kernel::service::Object &obj, const char *buffer) {
	if(opt_std_out) {
		if(opt_std_color) {
			std::cout << "\033[33;40m";
			std::cout << obj.GetName() << ":" << endl << buffer << endl;
			std::cout << "\033[37;40m";
		} else {
			std::cout << "DebugWarning(" << obj.GetName() << "): " << endl << buffer << endl;
		}
	}
	if(opt_std_err) {
		if(opt_std_out) {
			std::cerr << "\033[33;40m";
			std::cerr << obj.GetName() << ":" << endl << buffer << endl;
			std::cerr << "\033[37;40m";
		} else {
			std::cerr << "DebugWarning(" << obj.GetName() << "):" << endl << buffer << endl;
		}
	}
}

void 
LoggerServer::
DebugError(const unisim::kernel::service::Object &obj, const char *buffer) {
	if(opt_std_out) {
		if(opt_std_color) {
			std::cout << "\033[31;40m";
			std::cout << obj.GetName() << ":" << endl << buffer << endl;
			std::cout << "\033[37;40m";
		} else {
			std::cout << "DebugError(" << obj.GetName() << "): " << endl << buffer << endl;
		}
	}
	if(opt_std_err) {
		if(opt_std_color) {
			std::cerr << "\033[31;40m";
			std::cerr << obj.GetName() << ":" << endl << buffer << endl;
			std::cerr << "\033[37;40m";
		} else {
			std::cerr << "DebugError(" << obj.GetName() << "): " << endl << buffer << endl;
		}
	}
}

} // end of namespace logger
} // end of namespace kernel
} // end of namespace unisim

