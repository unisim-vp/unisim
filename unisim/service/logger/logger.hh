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
 
#ifndef __UNISIM_SERVICE_LOGGER_HH__
#define __UNISIM_SERVICE_LOGGER_HH__

#include "unisim/service/interfaces/logger.hh"
#include "unisim/service/logger/logger_server_interface.hh"
#include "unisim/kernel/service/service.hh"
#include <string>

namespace unisim {
namespace service {
namespace logger {
	
using unisim::kernel::service::Object;
using unisim::kernel::service::Service;
using unisim::kernel::service::ServiceExport;
using unisim::kernel::service::Parameter;
using unisim::service::logger::LoggerServerInterface;
using std::string;

class Logger :
    public Service<unisim::service::interfaces::Logger> {
public:
    /* Exported services */
    ServiceExport<unisim::service::interfaces::Logger> logger_export;
	
    /* Constructor/Destructor */
    Logger(const char *name, Object *parent = 0);
    virtual ~Logger();

    /* Service methods */
    virtual void OnDisconnect();
    virtual bool Setup();
    
    /* Method to connect the logger to a the server */
    void SetServer(LoggerServerInterface *server);

    /* Methods provided by the ServiceExport<unisim::service::interface::Logger> */	
	virtual void Append(bool val);
	virtual void Append(const char *val);
	virtual void Append(std::string &val);
	virtual void Append(const std::string &val);
	virtual void Append(char val);
	virtual void Append(unsigned char val);
	virtual void Append(short int val);
	virtual void Append(unsigned short int val);
	virtual void Append(int val);
	virtual void Append(unsigned int val);
	virtual void Append(long int val);
	virtual void Append(unsigned long int val);
	virtual void Append(long long int val);
	virtual void Append(unsigned long long int val);
	virtual void Append(float val);
	virtual void Append(double val);
	virtual void AppendHex();
	virtual void AppendDec();
	virtual void AppendEndl();
	virtual void AppendFlush();
	virtual void AppendDebugInfo();		
	virtual void AppendEndDebugInfo();		
	virtual void AppendDebugWarning();
	virtual void AppendEndDebugWarning();
	virtual void AppendDebugError();
	virtual void AppendEndDebugError();
	virtual void AppendFile();
	virtual void AppendFunction();
	virtual void AppendLine();
	
private:
	void Message(Node *node);
	void Flush();
	
	LoggerServerInterface *server;
    string client_name;

	PtrList<unisim::util::xml::Node> *buffer;
	unisim::util::xml::Node *current_node;

	enum NumericalMode {decimal = 0,
				hexadecimal = 1};
	NumericalMode num_status;
	enum AppendMode {normal = 0,
		file = 1,
		function = 2,
		line = 3};
	AppendMode append_mode;

public:
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

} // end of namespace logger
} // end of namespace service
} // end of namespace unisim

#endif /* __UNISIM_SERVICE_LOGGER_HH__ */
