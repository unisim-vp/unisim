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
 
#include "unisim/service/logger/logger.hh"
#include "unisim/service/interfaces/logger.hh"
#include "unisim/service/logger/logger_server_interface.hh"
#include "unisim/kernel/service/service.hh"
#include "unisim/util/xml/xml.hh"
#include <string>
#include <sstream>

namespace unisim {
namespace service {
namespace logger {

typedef unisim::service::interfaces::Logger LoggerIF;

using unisim::kernel::service::Object;
using unisim::kernel::service::Service;
using unisim::util::xml::Property;
using unisim::util::xml::Node;

using namespace std;

const string Logger::string_bool = "bool";
const string Logger::string_value = "value";
const string Logger::string_string = "string";
const string Logger::string_char = "char";
const string Logger::string_int = "int";
const string Logger::string_short_int = "short_int";
const string Logger::string_long_int = "long_int";
const string Logger::string_long_long_int = "long_long_int";
const string Logger::string_unsigned_char = "unsigned_char";
const string Logger::string_unsigned_int = "unsigned_int";
const string Logger::string_unsigned_short_int = "unsigned_short_int";
const string Logger::string_unsigned_long_int = "unsigned_long_int";
const string Logger::string_unsigned_long_long_int = "unsigned_long_long_int";
const string Logger::string_float = "float";
const string Logger::string_double = "double";
const string Logger::string_mode = "mode";
const string Logger::string_dec = "dec";
const string Logger::string_hex = "hex";
const string Logger::string_endl = "endl";
const string Logger::string_debug_info = "debug_info";
const string Logger::string_debug_warning = "debug_warning";
const string Logger::string_debug_error = "debug_error";
const string Logger::string_source = "source";
const string Logger::string_file = "file";
const string Logger::string_function = "function";
const string Logger::string_line = "line";
const string Logger::string_time = "time";
const string Logger::string_message = "message";

Logger::Logger(const char *name, Object *parent) :
    Object(name, parent),
    Service<LoggerIF>(name, parent),
    logger_export("logger_export", this),
    buffer(0),
    current_node(0),
    num_status(decimal),
    append_mode(normal) {
}

Logger::~Logger() {
}

/* Service methods */
void Logger::OnDisconnect() {
    if(server)
    	server->Flush();
}

bool Logger::Setup() {
    if(logger_export.GetClient() != 0) {
    	client_name = logger_export.GetClient()->GetName();
    } else {
    	client_name = "UnknownObject";
    }

    return true;
}

/* Method to connect the logger to a the server */
void Logger::SetServer(LoggerServerInterface *server) {
    this->server = server;
}

/* Methods provided by the ServiceExport<LoggerIF> */	
void Logger::Append(const char *val) {
	string tmp_str(val);
	Property *property;
	Node *node;
	
	if(current_node != 0) {
		switch(append_mode) {
		case Logger::normal:
			node = new Node(string_string);
			break;
		case Logger::file:
			node = new Node(string_file);
			break;
		case Logger::function:
			node = new Node(string_function);
			break;
		case Logger::line:
			node = new Node(string_line);
			break;
		}
		property = new Property(string_value, tmp_str);
		*node << *property;
		*current_node << *node;
		append_mode = Logger::normal;
	} 
}

void Logger::Append(string &val) {
	Property *property;
	Node *node;
	
	if(current_node != 0) {
		switch(append_mode) {
		case Logger::normal:
			node = new Node(string_string);
			break;
		case Logger::file:
			node = new Node(string_file);
			break;
		case Logger::function:
			node = new Node(string_function);
			break;
		case Logger::line:
			node = new Node(string_line);
			break;
		}
		property = new Property(string_value, val);
		*node << *property;
		*current_node << *node;
		append_mode = Logger::normal;
	}
}

void Logger::Append(const string &val) {
	Property *property;
	Node *node;
	
	if(current_node != 0) {
		switch(append_mode) {
		case Logger::normal:
			node = new Node(string_string);
			break;
		case Logger::file:
			node = new Node(string_file);
			break;
		case Logger::function:
			node = new Node(string_function);
			break;
		case Logger::line:
			node = new Node(string_line);
			break;
		}
		property = new Property(string_value, val);
		*node << *property;
		*current_node << *node;
		append_mode = Logger::normal;
	}
}

void Logger::Append(bool val) {
	Node *node;
	Property *property;
	stringstream tmp_str;
	
	if(current_node != 0 && append_mode == Logger::normal) {
		tmp_str << (val?"true":"false");
		node = new Node(string_bool);
		property = new Property(string_value, tmp_str.str());
		*node << *property;
		*current_node << *node; 
	}
	if(current_node != 0)
		append_mode = Logger::normal;
}

void Logger::Append(char val) {
	Node *node;
	Property *property;
	stringstream tmp_str;
	
	if(current_node != 0) {
		tmp_str << val;
		switch(append_mode) {
		case Logger::normal:
			switch(num_status) {
			case Logger::decimal:
				property = new Property(string_mode, string_dec);
				break;
			case Logger::hexadecimal:
				property = new Property(string_mode, string_hex);
				break;
			}
			node = new Node(string_char);
			*node << *property;
			break;
		case Logger::file:
			node = new Node(string_file);
			break;
		case Logger::function:
			node = new Node(string_function);
			break;
		case Logger::line:
			node = new Node(string_line);
			break;
		}
		property = new Property(string_value, tmp_str.str());
		*node << *property;
		*current_node << *node;
		append_mode = Logger::normal;
	}
}

void Logger::Append(unsigned char val) {
	Node *node;
	Property *property;
	stringstream tmp_str;
	
	if(current_node != 0) {
		tmp_str << val;
		switch(append_mode) {
		case Logger::normal:
			switch(num_status) {
			case Logger::decimal:
				property = new Property(string_mode, string_dec);
				break;
			case Logger::hexadecimal:
				property = new Property(string_mode, string_hex);
				break;
			}
			node = new Node(string_unsigned_char);
			*node << *property;
			break;
		case Logger::file:
			node = new Node(string_file);
			break;
		case Logger::function:
			node = new Node(string_function);
			break;
		case Logger::line:
			node = new Node(string_line);
			break;
		}
		property = new Property(string_value, tmp_str.str());
		*node << *property;
		*current_node << *node;
		append_mode = Logger::normal;
	}
}

void Logger::Append(short int val) {
	Node *node;
	Property *property;
	stringstream tmp_str;
	
	if(current_node != 0) {
		tmp_str << val;
		switch(append_mode) {
		case Logger::normal:
			switch(num_status) {
			case Logger::decimal:
				property = new Property(string_mode, string_dec);
				break;
			case Logger::hexadecimal:
				property = new Property(string_mode, string_hex);
				break;
			}
			node = new Node(string_short_int);
			*node << *property;
			break;
		case Logger::file:
			node = new Node(string_file);
			break;
		case Logger::function:
			node = new Node(string_function);
			break;
		case Logger::line:
			node = new Node(string_line);
			break;
		}
		property = new Property(string_value, tmp_str.str());
		*node << *property;
		*current_node << *node;
		append_mode = Logger::normal;
	}
}

void
Logger::Append(unsigned short int val) {
	Node *node;
	Property *property;
	stringstream tmp_str;
	
	if(current_node != 0) {
		tmp_str << val;
		switch(append_mode) {
		case Logger::normal:
			switch(num_status) {
			case Logger::decimal:
				property = new Property(string_mode, string_dec);
				break;
			case Logger::hexadecimal:
				property = new Property(string_mode, string_hex);
				break;
			}
			node = new Node(string_unsigned_short_int);
			*node << *property;
			break;
		case Logger::file:
			node = new Node(string_file);
			break;
		case Logger::function:
			node = new Node(string_function);
			break;
		case Logger::line:
			node = new Node(string_line);
			break;
		}
		property = new Property(string_value, tmp_str.str());
		*node << *property;
		*current_node << *node;
		append_mode = Logger::normal;
	}
}

void Logger::Append(int val) {
	Node *node;
	Property *property;
	stringstream tmp_str;
	
	if(current_node != 0) {
		tmp_str << val;
		switch(append_mode) {
		case Logger::normal:
			switch(num_status) {
			case Logger::decimal:
				property = new Property(string_mode, string_dec);
				break;
			case Logger::hexadecimal:
				property = new Property(string_mode, string_hex);
				break;
			}
			node = new Node(string_int);
			*node << *property;
			break;
		case Logger::file:
			node = new Node(string_file);
			break;
		case Logger::function:
			node = new Node(string_function);
			break;
		case Logger::line:
			node = new Node(string_line);
			break;
		}
		property = new Property(string_value, tmp_str.str());
		*node << *property;
		*current_node << *node;
		append_mode = Logger::normal;
	}
}

void Logger::Append(unsigned int val) {
	Node *node;
	Property *property;
	stringstream tmp_str;
	
	if(current_node != 0) {
		tmp_str << val;
		switch(append_mode) {
		case Logger::normal:
			switch(num_status) {
			case Logger::decimal:
				property = new Property(string_mode, string_dec);
				break;
			case Logger::hexadecimal:
				property = new Property(string_mode, string_hex);
				break;
			}
			node = new Node(string_unsigned_int);
			*node << *property;
			break;
		case Logger::file:
			node = new Node(string_file);
			break;
		case Logger::function:
			node = new Node(string_function);
			break;
		case Logger::line:
			node = new Node(string_line);
			break;
		}
		property = new Property(string_value, tmp_str.str());
		*node << *property;
		*current_node << *node;
		append_mode = Logger::normal;
	}
}

void Logger::Append(long int val) {
	Node *node;
	Property *property;
	stringstream tmp_str;
	
	if(current_node != 0) {
		tmp_str << val;
		switch(append_mode) {
		case Logger::normal:
			switch(num_status) {
			case Logger::decimal:
				property = new Property(string_mode, string_dec);
				break;
			case Logger::hexadecimal:
				property = new Property(string_mode, string_hex);
				break;
			}
			node = new Node(string_long_int);
			*node << *property;
			break;
		case Logger::file:
			node = new Node(string_file);
			break;
		case Logger::function:
			node = new Node(string_function);
			break;
		case Logger::line:
			node = new Node(string_line);
			break;
		}
		property = new Property(string_value, tmp_str.str());
		*node << *property;
		*current_node << *node;
		append_mode = Logger::normal;
	}
}

void Logger::Append(unsigned long int val) {
	Node *node;
	Property *property;
	stringstream tmp_str;
	
	if(current_node != 0) {
		tmp_str << val;
		switch(append_mode) {
		case Logger::normal:
			switch(num_status) {
			case Logger::decimal:
				property = new Property(string_mode, string_dec);
				break;
			case Logger::hexadecimal:
				property = new Property(string_mode, string_hex);
				break;
			}
			node = new Node(string_unsigned_long_int);
			*node << *property;
			break;
		case Logger::file:
			node = new Node(string_file);
			break;
		case Logger::function:
			node = new Node(string_function);
			break;
		case Logger::line:
			node = new Node(string_line);
			break;
		}
		property = new Property(string_value, tmp_str.str());
		*node << *property;
		*current_node << *node;
		append_mode = Logger::normal;
	}
}

void Logger::Append(long long int val) {
	Node *node;
	Property *property;
	stringstream tmp_str;
	
	if(current_node != 0) {
		tmp_str << val;
		switch(append_mode) {
		case Logger::normal:
			switch(num_status) {
			case Logger::decimal:
				property = new Property(string_mode, string_dec);
				break;
			case Logger::hexadecimal:
				property = new Property(string_mode, string_hex);
				break;
			}
			node = new Node(string_long_long_int);
			*node << *property;
			break;
		case Logger::file:
			node = new Node(string_file);
			break;
		case Logger::function:
			node = new Node(string_function);
			break;
		case Logger::line:
			node = new Node(string_line);
			break;
		}
		property = new Property(string_value, tmp_str.str());
		*node << *property;
		*current_node << *node;
		append_mode = Logger::normal;
	}
}

void Logger::Append(unsigned long long int val) {
	Node *node;
	Property *property;
	stringstream tmp_str;
	
	if(current_node != 0) {
		tmp_str << val;
		switch(append_mode) {
		case Logger::normal:
			switch(num_status) {
			case Logger::decimal:
				property = new Property(string_mode, string_dec);
				break;
			case Logger::hexadecimal:
				property = new Property(string_mode, string_hex);
				break;
			}
			node = new Node(string_unsigned_long_long_int);
			*node << *property;
			break;
		case Logger::file:
			node = new Node(string_file);
			break;
		case Logger::function:
			node = new Node(string_function);
			break;
		case Logger::line:
			node = new Node(string_line);
			break;
		}
		property = new Property(string_value, tmp_str.str());
		*node << *property;
		*current_node << *node;
		append_mode = Logger::normal;
	}
}


void Logger::Append(float val) {
	Node *node;
	Property *property;
	stringstream tmp_str;
	
	if(current_node != 0) {
		tmp_str << val;
		switch(append_mode) {
		case Logger::normal:
			node = new Node(string_float);
			break;
		case Logger::file:
			node = new Node(string_file);
			break;
		case Logger::function:
			node = new Node(string_function);
			break;
		case Logger::line:
			node = new Node(string_line);
			break;
		}
		property = new Property(string_value, tmp_str.str());
		*node << *property;
		*current_node << *node;
		append_mode = Logger::normal;
	}
}


void Logger::Append(double val) {
	Node *node;
	Property *property;
	stringstream tmp_str;
	
	if(current_node != 0) {
		tmp_str << val;
		switch(append_mode) {
		case Logger::normal:
			node = new Node(string_double);
			break;
		case Logger::file:
			node = new Node(string_file);
			break;
		case Logger::function:
			node = new Node(string_function);
			break;
		case Logger::line:
			node = new Node(string_line);
			break;
		}
		property = new Property(string_value, tmp_str.str());
		*node << *property;
		*current_node << *node;
		append_mode = Logger::normal;
	}
}

void Logger::AppendHex() {
	num_status = Logger::hexadecimal;
	append_mode = Logger::normal;
}

void Logger::AppendDec() {
	num_status = Logger::decimal;
	append_mode = Logger::normal;
}

void Logger::AppendEndl() {
	Node *node = new Node(string_endl);

	if(current_node != 0) {
		*current_node << *node;
		append_mode = Logger::normal;
	}
}

void Logger::AppendFlush() {
	Flush();
	append_mode = Logger::normal;
}

void Logger::AppendDebugInfo() {
	Property *property;
	
	/* if there is a current node, then we can do nothing, first current
	 *   node should be close
	 *  NOTE: this is an error */
	if(current_node != 0)
		return;
	current_node = new Node(string_debug_info);
	append_mode = Logger::normal;
}

void Logger::AppendEndDebugInfo() {
	Message(current_node);
	current_node = 0;
	append_mode = Logger::normal;
}

void Logger::AppendDebugWarning() {
	Property *property;
	
	/* if there is a current node, then we can do nothing, first current
	 *   node should be close
	 *  NOTE: this is an error */
	if(current_node != 0)
		return;
	current_node = new Node(string_debug_warning);
	append_mode = Logger::normal;
}

void Logger::AppendEndDebugWarning() {
	Message(current_node);
	current_node = 0;
	append_mode = Logger::normal;
}

void Logger::AppendDebugError() {
	Property *property;
	
	/* if there is a current node, then we can do nothing, first current
	 *   node should be close
	 *  NOTE: this is an error */
	if(current_node != 0)
		return;
	current_node = new Node(string_debug_error);
	append_mode = Logger::normal;
}

void Logger::AppendEndDebugError() {
	Message(current_node);
	current_node = 0;
	append_mode = Logger::normal;
}

void Logger::AppendFile() {
	if(current_node != 0)
		 append_mode = Logger::file;
}

void Logger::AppendFunction() {
	if(current_node != 0)
		append_mode = Logger::function;
}

void Logger::AppendLine() {
	if(current_node != 0)
		append_mode = Logger::line;
}

// private methods
void Logger::Message(Node *node) {
	Node *name_node;
	Property *source_property;

	if(server) {
		source_property = new Property(string_source, client_name);
		name_node = new Node(string_message);
		*name_node << *source_property;
		*name_node << *node;
		server->Message(name_node);
	}
}

void Logger::Flush() {
	if(server)
		server->Flush();
}

} // end of namespace logger
} // end of namespace service
} // end of namespace unisim
