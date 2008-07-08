#include "unisim/kernel/service/service.hh"
#include "unisim/kernel/logger/logger.hh"
#include <string>

#ifndef __UNISIM_KERNEL_LOGGER_LOGGER_SERVER_HH__
#define __UNISIM_KERNEL_LOGGER_LOGGER_SERVER_HH__

namespace unisim {
namespace kernel {
namespace logger {

class LoggerServer : 
	unisim::kernel::service::Object {
private:
	typedef unisim::kernel::service::Parameter<bool> bool_parameter_t;
	typedef unisim::kernel::service::Parameter<std::string> string_parameter_t;

public:
	LoggerServer();
	~LoggerServer();

	static LoggerServer *GetInstance();
	
	void DebugInfo(const unisim::kernel::service::Object &obj, const char *buffer);
	void DebugWarning(const unisim::kernel::service::Object &obj, const char *buffer);
	void DebugError(const unisim::kernel::service::Object &obj, const char *buffer);

	bool_parameter_t param_std_err;
	bool_parameter_t param_std_out;
	bool_parameter_t param_std_color;
	bool_parameter_t param_file;
	string_parameter_t param_filename;
	bool_parameter_t param_xml_file;
	string_parameter_t param_xml_filename;

private:
	static LoggerServer *singleton;

	bool opt_std_err;
	bool opt_std_out;
	bool opt_std_color;
	bool opt_file;
	std::string opt_filename;
	bool opt_xml_file;
	std::string opt_xml_filename;

};

} // end of namespace logger
} // end of namespace kernel
} // end of namespace unisim

#endif // __UNISIM_KERNEL_LOGGER_LOGGER_SERVER_HH__
