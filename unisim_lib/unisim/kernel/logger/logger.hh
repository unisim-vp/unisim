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

