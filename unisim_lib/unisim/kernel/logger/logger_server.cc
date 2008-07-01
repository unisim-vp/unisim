#include "unisim/kernel/logger/logger_server.hh"

namespace unisim {
namespace kernel {
namespace logger {

LoggerServer::
LoggerServer() :
	Object("kernel_logger", 0),
	opt_std_err(false),
	opt_std_out(false),
	opt_file(false),
	opt_filename("logger_output.txt"),
	opt_xml_file(false),
	opt_xml_filename("logger_output.xml"),
	param_std_err("std_err", this, opt_std_err, "Show logger output through the standard error output"),
	param_std_out("std_out", this, opt_std_out, "Show logger output through the standard output"),
	param_file("file", this, opt_file, "Keep logger output in a file"),
	param_filename("filename", this, opt_filename, "Filename to keep logger output (the option file must be activated)"),
	param_xml_file("xml_file", this, opt_xml_file, "Keep logger output in a file xml formatted"),
	param_xml_filename("xml_filename", this, opt_xml_filename, "Filename to keep logger xml output (the option xml_file must be activated)") {

}

LoggerServer::
~LoggerServer() {
}

} // end of namespace logger
} // end of namespace kernel
} // end of namespace unisim

