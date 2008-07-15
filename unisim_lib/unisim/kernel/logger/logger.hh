#ifndef __UNISIM_KERNEL_LOGGER_HH__
#define __UNISIM_KERNEL_LOGGER_HH__

#include <sstream>
#include "unisim/kernel/logger/logger_server.hh"
#include "unisim/kernel/service/service.hh"

namespace unisim {
namespace kernel {
namespace logger {

using namespace std;

class Logger {
public:
	Logger(const unisim::kernel::service::Object &_obj);
	~Logger();

	Logger& operator << (std::ostream& (*f)(std::ostream &));
	Logger& operator << (Logger& (*f)(Logger &));

	template<typename T> Logger& operator << (const T& t) {
		buffer << t;
		return *this;
	}

	void DebugInfo();
	void EndDebugInfo();
	void DebugWarning();
	void EndDebugWarning();
	void DebugError();
	void EndDebugError();
	void EndDebug();

private:
	std::stringstream buffer;
	const unisim::kernel::service::Object &obj;
	enum mode_t {NO_MODE,
		INFO_MODE,
		WARNING_MODE,
		ERROR_MODE};
	mode_t mode;
	void PrintMode();

	LoggerServer *server;
};

Logger& DebugInfo(Logger&);
Logger& EndDebugInfo(Logger&);
Logger& DebugWarning(Logger&);
Logger& EndDebugWarning(Logger&);
Logger& DebugError(Logger&);
Logger& EndDebugError(Logger&);
Logger& EndDebug(Logger&);

} // end of namespace logger
} // end of namespace kernel
} // end of namespace unisim

#endif // __UNISIM_KERNEL_LOGGER_HH__

