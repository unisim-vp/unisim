/*
 *  Copyright (c) 2007,
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
 * Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 */
 
#ifndef __UNISIM_SERVICE_INTERFACES_LOGGER_HH__
#define __UNISIM_SERVICE_INTERFACES_LOGGER_HH__

#include <string>
#include <iostream>
#include "unisim/util/xml/xml.hh"

namespace unisim {
namespace service {
namespace interfaces {
	
using unisim::util::xml::Node;
using unisim::util::xml::PtrList;

/* Forward declaration of the interface */
class Logger;

/* Operators that can be applied to the Logger to build messages */
Logger &operator <<(Logger &os, bool val);
Logger &operator <<(Logger &os, const char *val);
Logger &operator <<(Logger &os, std::string &val);
Logger &operator <<(Logger &os, const std::string &val);
Logger &operator <<(Logger &os, char val);
Logger &operator <<(Logger &os, unsigned char val);
Logger &operator <<(Logger &os, short int val);
Logger &operator <<(Logger &os, unsigned short int val);
Logger &operator <<(Logger &os, int val);
Logger &operator <<(Logger &os, unsigned int val);
Logger &operator <<(Logger &os, long int val);
Logger &operator <<(Logger &os, unsigned long int val);
Logger &operator <<(Logger &os, long long int val);
Logger &operator <<(Logger &os, unsigned long long int val);
Logger &operator <<(Logger &os, float val);
Logger &operator <<(Logger &os, double val);
Logger &operator <<(Logger &os, Logger& (*fn)(Logger&));
Logger &Hex(Logger &os);
Logger &Dec(Logger &os);
Logger &Endl(Logger &os);
Logger &Flush(Logger &os);
Logger &DebugInfo(Logger &os);
Logger &EndDebugInfo(Logger &os);
Logger &DebugWarning(Logger &os);
Logger &EndDebugWarning(Logger &os);
Logger &DebugError(Logger &os);
Logger &EndDebugError(Logger &os);
Logger &File(Logger &os);
Logger &Function(Logger &os);
Logger &Line(Logger &os);

class Logger : public std::ostream {
public:
	friend Logger &unisim::service::interfaces::operator <<(Logger &os, bool val);
	friend Logger &unisim::service::interfaces::operator <<(Logger &os, const char *val);
	friend Logger &unisim::service::interfaces::operator <<(Logger &os, std::string &val);
	friend Logger &unisim::service::interfaces::operator <<(Logger &os, const std::string &val);
	friend Logger &unisim::service::interfaces::operator <<(Logger &os, char val);
	friend Logger &unisim::service::interfaces::operator <<(Logger &os, unsigned char val);
	friend Logger &unisim::service::interfaces::operator <<(Logger &os, short int val);
	friend Logger &unisim::service::interfaces::operator <<(Logger &os, unsigned short int val);
	friend Logger &unisim::service::interfaces::operator <<(Logger &os, int val);
	friend Logger &unisim::service::interfaces::operator <<(Logger &os, unsigned int val);
	friend Logger &unisim::service::interfaces::operator <<(Logger &os, long int val);
	friend Logger &unisim::service::interfaces::operator <<(Logger &os, unsigned long int val);
	friend Logger &unisim::service::interfaces::operator <<(Logger &os, long long int val);
	friend Logger &unisim::service::interfaces::operator <<(Logger &os, unsigned long long int val);
	friend Logger &unisim::service::interfaces::operator <<(Logger &os, float val);
	friend Logger &unisim::service::interfaces::operator <<(Logger &os, double val);
	friend Logger &unisim::service::interfaces::operator <<(Logger &os, Logger& (*fn)(Logger&));
	friend Logger &unisim::service::interfaces::Hex(Logger &os);
	friend Logger &unisim::service::interfaces::Dec(Logger &os);
	friend Logger &unisim::service::interfaces::Endl(Logger &os);
	friend Logger &unisim::service::interfaces::Flush(Logger &os);
	friend Logger &unisim::service::interfaces::DebugInfo(Logger &os);		
	friend Logger &unisim::service::interfaces::EndDebugInfo(Logger &os);		
	friend Logger &unisim::service::interfaces::DebugWarning(Logger &os);
	friend Logger &unisim::service::interfaces::EndDebugWarning(Logger &os);
	friend Logger &unisim::service::interfaces::DebugError(Logger &os);
	friend Logger &unisim::service::interfaces::EndDebugError(Logger &os);
	friend Logger &unisim::service::interfaces::File(Logger &os);
	friend Logger &unisim::service::interfaces::Function(Logger &os);
	friend Logger &unisim::service::interfaces::Line(Logger &os);
	
//protected:

	/* constructor and destructor */
	Logger();
	~Logger();
	virtual void Message(unisim::util::xml::Node *node) = 0;
	virtual void Flush() = 0;

//	string name;
	PtrList<unisim::util::xml::Node> *buffer;
	unisim::util::xml::Node *current_node;
//	stringstream buffer;
//	enum StatusType {undefined = 0,
//		debug_info = 1,
//		debug_warning = 2,
//		debug_error = 3};
	enum NumericalMode {decimal = 0,
				hexadecimal = 1};
	NumericalMode num_status;
	enum AppendMode {normal = 0,
		file = 1,
		function = 2,
		line = 3};
	AppendMode append_mode;

	static const std::string string_bool;
	static const std::string string_value;
	static const std::string string_string;
	static const std::string string_char;
	static const std::string string_int;
	static const std::string string_short_int;
	static const std::string string_long_int;
	static const std::string string_long_long_int;
	static const std::string string_unsigned_char;
	static const std::string string_unsigned_int;
	static const std::string string_unsigned_short_int;
	static const std::string string_unsigned_long_int;
	static const std::string string_unsigned_long_long_int;
	static const std::string string_float;
	static const std::string string_double;
	static const std::string string_mode;
	static const std::string string_dec;
	static const std::string string_hex;
	static const std::string string_endl;
	static const std::string string_debug_info;
	static const std::string string_debug_warning;
	static const std::string string_debug_error;
	static const std::string string_source;
	static const std::string string_file;
	static const std::string string_function;
	static const std::string string_line;
	static const std::string string_time;
	static const std::string string_message;

};

}
}
}

#endif /* __UNISIM_SERVICE_INTERFACES_LOGGER_HH__ */


