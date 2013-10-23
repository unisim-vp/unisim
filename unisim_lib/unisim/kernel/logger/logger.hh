/*
 *  Copyright (c) 2008,
 *  Commissariat a l'Energie Atomique (CEA)
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without modification,
 *  are permitted provided that the following conditions are met:
 *
 *   - Redistributions of source code must retain the above copyright notice, this
 *     list of conditions and the following disclaimer.
 *
 *   - Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 *   - Neither the name of CEA nor the names of its contributors may be used to
 *     endorse or promote products derived from this software without specific prior
 *     written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 *  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *  DISCLAIMED.
 *  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 *  OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 *  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Author: Daniel Gracia Perez (daniel.gracia-perez@cea.fr) 
 */

#ifndef __UNISIM_KERNEL_LOGGER_HH__
#define __UNISIM_KERNEL_LOGGER_HH__

#include <ios>
#include <ostream>
#include <sstream>

namespace unisim { 
namespace kernel {
namespace service {

class Object;

}
}
}

namespace unisim {
namespace kernel {
namespace logger {

class LoggerServer;

class Logger {
public:
	Logger(const unisim::kernel::service::Object &obj);
	~Logger();

	friend Logger& operator << (Logger& logger, std::ios_base& (*f)(std::ios_base &));
	friend Logger& operator << (Logger& logger, std::ostream& (*f)(std::ostream &));
	friend Logger& operator << (Logger& logger, Logger& (*f)(Logger &));

	template<typename T> Logger& operator << (const T& t) {
		buffer_ << t;
		return *this;
	}

  // Returns the raw STL output stream that is used to print out the log.
  // Use this method with caution. Before handling the stream one of the open
  // debug tags should have been used, and after the close of the debug tags the
  // stream should not be used any longer.
  // Example of usage:
  //   logger << DebugInfo; // start debug info message
  //   {
  //     std::ostream& log_stream = logger.GetStream(); // get the raw output
  //                                                    // stream
  //     // send whatever you want to the output stream
  //     log_stream << "Hello world";
  //     // ...
  //   } // we ensure that the log_stream liveness is in between DebugInfo and
  //     // EndDebugInfo tags, so it is not used outside that scope
  //   logger << EndDebugInfo;
  //
  // More tricky usages are possible, but not recommended.
  std::ostream& GetStream();

	void DebugInfo();
	void EndDebugInfo();
	void DebugWarning();
	void EndDebugWarning();
	void DebugError();
	void EndDebugError();
	void EndDebug();

private:
	std::stringstream buffer_;
	const unisim::kernel::service::Object &obj_;
	enum mode_t {NO_MODE,
		INFO_MODE,
		WARNING_MODE,
		ERROR_MODE};
	mode_t mode_;
	void PrintMode();

  unisim::kernel::logger::LoggerServer *server_;
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

