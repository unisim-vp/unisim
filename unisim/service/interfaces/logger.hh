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
 
/*******************************************************************
 *                     IMPORTANT
 * This class is a pure virtual class, so an interface, even if it
 * doesn't look like one. The friend operators that you see below
 * just call pure virtual methods.
 * The class uses the so called 'friend virtual' method.
 *******************************************************************/

#ifndef __UNISIM_SERVICE_INTERFACES_LOGGER_HH__
#define __UNISIM_SERVICE_INTERFACES_LOGGER_HH__

#include <string>

namespace unisim {
namespace service {
namespace interfaces {
	
/* Forward declaration of the interface */
class Logger;

/* Operators that can be applied to the Logger to build messages */
inline Logger &operator <<(Logger &os, bool val);
inline Logger &operator <<(Logger &os, const char *val);
inline Logger &operator <<(Logger &os, std::string &val);
inline Logger &operator <<(Logger &os, const std::string &val);
inline Logger &operator <<(Logger &os, char val);
inline Logger &operator <<(Logger &os, unsigned char val);
inline Logger &operator <<(Logger &os, short int val);
inline Logger &operator <<(Logger &os, unsigned short int val);
inline Logger &operator <<(Logger &os, int val);
inline Logger &operator <<(Logger &os, unsigned int val);
inline Logger &operator <<(Logger &os, long int val);
inline Logger &operator <<(Logger &os, unsigned long int val);
inline Logger &operator <<(Logger &os, long long int val);
inline Logger &operator <<(Logger &os, unsigned long long int val);
inline Logger &operator <<(Logger &os, float val);
inline Logger &operator <<(Logger &os, double val);
inline Logger &operator <<(Logger &os, Logger& (*fn)(Logger&));
inline Logger &Hex(Logger &os);
inline Logger &Dec(Logger &os);
inline Logger &Endl(Logger &os);
inline Logger &Flush(Logger &os);
inline Logger &DebugInfo(Logger &os);
inline Logger &EndDebugInfo(Logger &os);
inline Logger &DebugWarning(Logger &os);
inline Logger &EndDebugWarning(Logger &os);
inline Logger &DebugError(Logger &os);
inline Logger &EndDebugError(Logger &os);
inline Logger &File(Logger &os);
inline Logger &Function(Logger &os);
inline Logger &Line(Logger &os);

class Logger {
public:
	Logger();
	virtual ~Logger();
	
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

protected:
	virtual void Append(bool val) = 0;
	virtual void Append(const char *val) = 0;
	virtual void Append(std::string &val) = 0;
	virtual void Append(const std::string &val) = 0;
	virtual void Append(char val) = 0;
	virtual void Append(unsigned char val) = 0;
	virtual void Append(short int val) = 0;
	virtual void Append(unsigned short int val) = 0;
	virtual void Append(int val) = 0;
	virtual void Append(unsigned int val) = 0;
	virtual void Append(long int val) = 0;
	virtual void Append(unsigned long int val) = 0;
	virtual void Append(long long int val) = 0;
	virtual void Append(unsigned long long int val) = 0;
	virtual void Append(float val) = 0;
	virtual void Append(double val) = 0;
	virtual void AppendHex() = 0;
	virtual void AppendDec() = 0;
	virtual void AppendEndl() = 0;
	virtual void AppendFlush() = 0;
	virtual void AppendDebugInfo() = 0;		
	virtual void AppendEndDebugInfo() = 0;		
	virtual void AppendDebugWarning() = 0;
	virtual void AppendEndDebugWarning() = 0;
	virtual void AppendDebugError() = 0;
	virtual void AppendEndDebugError() = 0;
	virtual void AppendFile() = 0;
	virtual void AppendFunction() = 0;
	virtual void AppendLine() = 0;
};

inline Logger::Logger()
{
}

inline Logger::~Logger()
{
}

inline Logger &operator <<(Logger &os, const char *val) {
	os.Append(val);
	return os;
}

inline Logger &operator <<(Logger &os, std::string &val) {
	os.Append(val);
	return os;
}

inline Logger &operator <<(Logger &os, const std::string &val) {
	os.Append(val);
	return os;
}

inline Logger &operator <<(Logger &os, bool val) {
	os.Append(val);
	return os;
}

inline Logger &operator <<(Logger &os, char val) {
	os.Append(val);
	return os;
}

inline Logger &operator <<(Logger &os, unsigned char val) {
	os.Append(val);
	return os;	
}

inline Logger &operator <<(Logger &os, short int val) {
	os.Append(val);
	return os;	
}

inline Logger &operator <<(Logger &os, unsigned short int val) {
	os.Append(val);
	return os;	
}

inline Logger &operator <<(Logger &os, int val) {
	os.Append(val);
	return os;	
}

inline Logger &operator <<(Logger &os, unsigned int val) {
	os.Append(val);
	return os;	
}

inline Logger &operator <<(Logger &os, long int val) {
	os.Append(val);
	return os;	
}

inline Logger &operator <<(Logger &os, unsigned long int val) {
	os.Append(val);
	return os;	
}

inline Logger &operator <<(Logger &os, long long int val) {
	os.Append(val);
	return os;	
}

inline Logger &operator <<(Logger &os, unsigned long long int val) {
	os.Append(val);
	return os;	
}

inline Logger &operator <<(Logger &os, float val) {
	os.Append(val);
	return os;
}

inline Logger &operator <<(Logger &os, double val) {
	os.Append(val);
	return os;
}

inline Logger &operator <<(Logger &os,	Logger& (*fn)(Logger&))	{ 
	return (*fn)(os); 
}
		
inline Logger &Hex(Logger &os) {
	os.AppendHex();
	return os;
}

inline Logger &Dec(Logger &os) {
	os.AppendDec();
	return os;
}

inline Logger &Endl(Logger &os) {
	os.AppendEndl();
	return os;	
}

inline Logger &Flush(Logger &os) {
	os.AppendFlush();
	return os;
}

inline Logger &DebugInfo(Logger &os) {
	os.AppendDebugInfo();
	return os;
}

inline Logger &EndDebugInfo(Logger &os) {
	os.AppendEndDebugInfo();
	return os;
}

inline Logger &DebugWarning(Logger &os) {
	os.AppendDebugWarning();
	return os;
}

inline Logger &EndDebugWarning(Logger &os) {
	os.AppendEndDebugWarning();
	return os;
}

inline Logger &DebugError(Logger &os) {
	os.AppendDebugError();
	return os;
}

inline Logger &EndDebugError(Logger &os) {
	os.AppendEndDebugError();
	return os;
}

inline Logger &File(Logger &os) {
	os.AppendFile();
	return os;
}

inline Logger &Function(Logger &os) {
	os.AppendFunction();
	return os;
}

inline Logger &Line(Logger &os) {
	os.AppendLine();
	return os;
}

} // end of namespace interface
} // end of namespace service
} // end of namespace unisim

#endif /* __UNISIM_SERVICE_INTERFACES_LOGGER_HH__ */


