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
 
#include <string>
#include <sstream>
#include "unisim/service/interfaces/logger.hh"
#include "unisim/util/xml/xml.hh"

using unisim::util::xml::Property;
using unisim::util::xml::Node;
using unisim::util::xml::PtrList;
using unisim::service::interfaces::Logger;
using namespace std;

const string unisim::service::interfaces::Logger::string_bool = "bool";
const string unisim::service::interfaces::Logger::string_value = "value";
const string unisim::service::interfaces::Logger::string_string = "string";
const string unisim::service::interfaces::Logger::string_char = "char";
const string unisim::service::interfaces::Logger::string_int = "int";
const string unisim::service::interfaces::Logger::string_short_int = "short_int";
const string unisim::service::interfaces::Logger::string_long_int = "long_int";
const string unisim::service::interfaces::Logger::string_long_long_int = "long_long_int";
const string unisim::service::interfaces::Logger::string_unsigned_char = "unsigned_char";
const string unisim::service::interfaces::Logger::string_unsigned_int = "unsigned_int";
const string unisim::service::interfaces::Logger::string_unsigned_short_int = "unsigned_short_int";
const string unisim::service::interfaces::Logger::string_unsigned_long_int = "unsigned_long_int";
const string unisim::service::interfaces::Logger::string_unsigned_long_long_int = "unsigned_long_long_int";
const string unisim::service::interfaces::Logger::string_float = "float";
const string unisim::service::interfaces::Logger::string_double = "double";
const string unisim::service::interfaces::Logger::string_mode = "mode";
const string unisim::service::interfaces::Logger::string_dec = "dec";
const string unisim::service::interfaces::Logger::string_hex = "hex";
const string unisim::service::interfaces::Logger::string_endl = "endl";
const string unisim::service::interfaces::Logger::string_debug_info = "debug_info";
const string unisim::service::interfaces::Logger::string_debug_warning = "debug_warning";
const string unisim::service::interfaces::Logger::string_debug_error = "debug_error";
const string unisim::service::interfaces::Logger::string_source = "source";
const string unisim::service::interfaces::Logger::string_file = "file";
const string unisim::service::interfaces::Logger::string_function = "function";
const string unisim::service::interfaces::Logger::string_line = "line";
const string unisim::service::interfaces::Logger::string_time = "time";
const string unisim::service::interfaces::Logger::string_message = "message";

unisim::service::interfaces::Logger::Logger() : ostream(0) {
	buffer = 0;
	current_node = 0;
	num_status = decimal;
	append_mode = normal;
}

unisim::service::interfaces::Logger::~Logger() {
	delete buffer;
	delete current_node;
}

Logger &
unisim::service::interfaces::
operator <<(Logger &os, const char *val) {
	string tmp_str(val);
	Property *property;
	Node *node;
	
	if(os.current_node != 0) {
		switch(os.append_mode) {
		case Logger::normal:
			node = new Node(os.string_string);
			break;
		case Logger::file:
			node = new Node(os.string_file);
			break;
		case Logger::function:
			node = new Node(os.string_function);
			break;
		case Logger::line:
			node = new Node(os.string_line);
			break;
		}
		property = new Property(os.string_value, tmp_str);
		*node << *property;
		*os.current_node << *node;
		os.append_mode = Logger::normal;
	} 
	return os;
}

Logger &
unisim::service::interfaces::
operator <<(Logger &os, string &val) {
	Property *property;
	Node *node;
	
	if(os.current_node != 0) {
		switch(os.append_mode) {
		case Logger::normal:
			node = new Node(os.string_string);
			break;
		case Logger::file:
			node = new Node(os.string_file);
			break;
		case Logger::function:
			node = new Node(os.string_function);
			break;
		case Logger::line:
			node = new Node(os.string_line);
			break;
		}
		property = new Property(os.string_value, val);
		*node << *property;
		*os.current_node << *node;
		os.append_mode = Logger::normal;
	}
	return os;
}

Logger &
unisim::service::interfaces::
operator <<(Logger &os, const string &val) {
	Property *property;
	Node *node;
	
	if(os.current_node != 0) {
		switch(os.append_mode) {
		case Logger::normal:
			node = new Node(os.string_string);
			break;
		case Logger::file:
			node = new Node(os.string_file);
			break;
		case Logger::function:
			node = new Node(os.string_function);
			break;
		case Logger::line:
			node = new Node(os.string_line);
			break;
		}
		property = new Property(os.string_value, val);
		*node << *property;
		*os.current_node << *node;
		os.append_mode = Logger::normal;
	}
	return os;
}

Logger &
unisim::service::interfaces::
operator <<(Logger &os, bool val) {
	Node *node;
	Property *property;
	stringstream tmp_str;
	
	if(os.current_node != 0 && os.append_mode == Logger::normal) {
		tmp_str << (val?"true":"false");
		node = new Node(os.string_bool);
		property = new Property(os.string_value, tmp_str.str());
		*node << *property;
		*os.current_node << *node; 
	}
	if(os.current_node != 0)
		os.append_mode = Logger::normal;
		
	return os;
}

Logger &
unisim::service::interfaces::
operator <<(Logger &os, char val) {
	Node *node;
	Property *property;
	stringstream tmp_str;
	
	if(os.current_node != 0) {
		tmp_str << val;
		switch(os.append_mode) {
		case Logger::normal:
			switch(os.num_status) {
			case Logger::decimal:
				property = new Property(os.string_mode, os.string_dec);
				break;
			case Logger::hexadecimal:
				property = new Property(os.string_mode, os.string_hex);
				break;
			}
			node = new Node(os.string_char);
			*node << *property;
			break;
		case Logger::file:
			node = new Node(os.string_file);
			break;
		case Logger::function:
			node = new Node(os.string_function);
			break;
		case Logger::line:
			node = new Node(os.string_line);
			break;
		}
		property = new Property(os.string_value, tmp_str.str());
		*node << *property;
		*os.current_node << *node;
		os.append_mode = Logger::normal;
	}
	return os;	
}

Logger &
unisim::service::interfaces::
operator <<(Logger &os, unsigned char val) {
	Node *node;
	Property *property;
	stringstream tmp_str;
	
	if(os.current_node != 0) {
		tmp_str << val;
		switch(os.append_mode) {
		case Logger::normal:
			switch(os.num_status) {
			case Logger::decimal:
				property = new Property(os.string_mode, os.string_dec);
				break;
			case Logger::hexadecimal:
				property = new Property(os.string_mode, os.string_hex);
				break;
			}
			node = new Node(os.string_unsigned_char);
			*node << *property;
			break;
		case Logger::file:
			node = new Node(os.string_file);
			break;
		case Logger::function:
			node = new Node(os.string_function);
			break;
		case Logger::line:
			node = new Node(os.string_line);
			break;
		}
		property = new Property(os.string_value, tmp_str.str());
		*node << *property;
		*os.current_node << *node;
		os.append_mode = Logger::normal;
	}
	return os;	
}

Logger &
unisim::service::interfaces::
operator <<(Logger &os, short int val) {
	Node *node;
	Property *property;
	stringstream tmp_str;
	
	if(os.current_node != 0) {
		tmp_str << val;
		switch(os.append_mode) {
		case Logger::normal:
			switch(os.num_status) {
			case Logger::decimal:
				property = new Property(os.string_mode, os.string_dec);
				break;
			case Logger::hexadecimal:
				property = new Property(os.string_mode, os.string_hex);
				break;
			}
			node = new Node(os.string_short_int);
			*node << *property;
			break;
		case Logger::file:
			node = new Node(os.string_file);
			break;
		case Logger::function:
			node = new Node(os.string_function);
			break;
		case Logger::line:
			node = new Node(os.string_line);
			break;
		}
		property = new Property(os.string_value, tmp_str.str());
		*node << *property;
		*os.current_node << *node;
		os.append_mode = Logger::normal;
	}
	return os;	
}

Logger &
unisim::service::interfaces::
operator <<(Logger &os, unsigned short int val) {
	Node *node;
	Property *property;
	stringstream tmp_str;
	
	if(os.current_node != 0) {
		tmp_str << val;
		switch(os.append_mode) {
		case Logger::normal:
			switch(os.num_status) {
			case Logger::decimal:
				property = new Property(os.string_mode, os.string_dec);
				break;
			case Logger::hexadecimal:
				property = new Property(os.string_mode, os.string_hex);
				break;
			}
			node = new Node(os.string_unsigned_short_int);
			*node << *property;
			break;
		case Logger::file:
			node = new Node(os.string_file);
			break;
		case Logger::function:
			node = new Node(os.string_function);
			break;
		case Logger::line:
			node = new Node(os.string_line);
			break;
		}
		property = new Property(os.string_value, tmp_str.str());
		*node << *property;
		*os.current_node << *node;
		os.append_mode = Logger::normal;
	}
	return os;	
}

Logger &
unisim::service::interfaces::
operator <<(Logger &os, int val) {
	Node *node;
	Property *property;
	stringstream tmp_str;
	
	if(os.current_node != 0) {
		tmp_str << val;
		switch(os.append_mode) {
		case Logger::normal:
			switch(os.num_status) {
			case Logger::decimal:
				property = new Property(os.string_mode, os.string_dec);
				break;
			case Logger::hexadecimal:
				property = new Property(os.string_mode, os.string_hex);
				break;
			}
			node = new Node(os.string_int);
			*node << *property;
			break;
		case Logger::file:
			node = new Node(os.string_file);
			break;
		case Logger::function:
			node = new Node(os.string_function);
			break;
		case Logger::line:
			node = new Node(os.string_line);
			break;
		}
		property = new Property(os.string_value, tmp_str.str());
		*node << *property;
		*os.current_node << *node;
		os.append_mode = Logger::normal;
	}
	return os;	
}

Logger &
unisim::service::interfaces::
operator <<(Logger &os, unsigned int val) {
	Node *node;
	Property *property;
	stringstream tmp_str;
	
	if(os.current_node != 0) {
		tmp_str << val;
		switch(os.append_mode) {
		case Logger::normal:
			switch(os.num_status) {
			case Logger::decimal:
				property = new Property(os.string_mode, os.string_dec);
				break;
			case Logger::hexadecimal:
				property = new Property(os.string_mode, os.string_hex);
				break;
			}
			node = new Node(os.string_unsigned_int);
			*node << *property;
			break;
		case Logger::file:
			node = new Node(os.string_file);
			break;
		case Logger::function:
			node = new Node(os.string_function);
			break;
		case Logger::line:
			node = new Node(os.string_line);
			break;
		}
		property = new Property(os.string_value, tmp_str.str());
		*node << *property;
		*os.current_node << *node;
		os.append_mode = Logger::normal;
	}
	return os;	
}

Logger &
unisim::service::interfaces::
operator <<(Logger &os, long int val) {
	Node *node;
	Property *property;
	stringstream tmp_str;
	
	if(os.current_node != 0) {
		tmp_str << val;
		switch(os.append_mode) {
		case Logger::normal:
			switch(os.num_status) {
			case Logger::decimal:
				property = new Property(os.string_mode, os.string_dec);
				break;
			case Logger::hexadecimal:
				property = new Property(os.string_mode, os.string_hex);
				break;
			}
			node = new Node(os.string_long_int);
			*node << *property;
			break;
		case Logger::file:
			node = new Node(os.string_file);
			break;
		case Logger::function:
			node = new Node(os.string_function);
			break;
		case Logger::line:
			node = new Node(os.string_line);
			break;
		}
		property = new Property(os.string_value, tmp_str.str());
		*node << *property;
		*os.current_node << *node;
		os.append_mode = Logger::normal;
	}
	return os;	
}

Logger &
unisim::service::interfaces::
operator <<(Logger &os, unsigned long int val) {
	Node *node;
	Property *property;
	stringstream tmp_str;
	
	if(os.current_node != 0) {
		tmp_str << val;
		switch(os.append_mode) {
		case Logger::normal:
			switch(os.num_status) {
			case Logger::decimal:
				property = new Property(os.string_mode, os.string_dec);
				break;
			case Logger::hexadecimal:
				property = new Property(os.string_mode, os.string_hex);
				break;
			}
			node = new Node(os.string_unsigned_long_int);
			*node << *property;
			break;
		case Logger::file:
			node = new Node(os.string_file);
			break;
		case Logger::function:
			node = new Node(os.string_function);
			break;
		case Logger::line:
			node = new Node(os.string_line);
			break;
		}
		property = new Property(os.string_value, tmp_str.str());
		*node << *property;
		*os.current_node << *node;
		os.append_mode = Logger::normal;
	}
	return os;	
}

Logger &
unisim::service::interfaces::
operator <<(Logger &os, long long int val) {
	Node *node;
	Property *property;
	stringstream tmp_str;
	
	if(os.current_node != 0) {
		tmp_str << val;
		switch(os.append_mode) {
		case Logger::normal:
			switch(os.num_status) {
			case Logger::decimal:
				property = new Property(os.string_mode, os.string_dec);
				break;
			case Logger::hexadecimal:
				property = new Property(os.string_mode, os.string_hex);
				break;
			}
			node = new Node(os.string_long_long_int);
			*node << *property;
			break;
		case Logger::file:
			node = new Node(os.string_file);
			break;
		case Logger::function:
			node = new Node(os.string_function);
			break;
		case Logger::line:
			node = new Node(os.string_line);
			break;
		}
		property = new Property(os.string_value, tmp_str.str());
		*node << *property;
		*os.current_node << *node;
		os.append_mode = Logger::normal;
	}
	return os;	
}

Logger &
unisim::service::interfaces::
operator <<(Logger &os, unsigned long long int val) {
	Node *node;
	Property *property;
	stringstream tmp_str;
	
	if(os.current_node != 0) {
		tmp_str << val;
		switch(os.append_mode) {
		case Logger::normal:
			switch(os.num_status) {
			case Logger::decimal:
				property = new Property(os.string_mode, os.string_dec);
				break;
			case Logger::hexadecimal:
				property = new Property(os.string_mode, os.string_hex);
				break;
			}
			node = new Node(os.string_unsigned_long_long_int);
			*node << *property;
			break;
		case Logger::file:
			node = new Node(os.string_file);
			break;
		case Logger::function:
			node = new Node(os.string_function);
			break;
		case Logger::line:
			node = new Node(os.string_line);
			break;
		}
		property = new Property(os.string_value, tmp_str.str());
		*node << *property;
		*os.current_node << *node;
		os.append_mode = Logger::normal;
	}
	return os;	
}

Logger &
unisim::service::interfaces::
operator <<(Logger &os, float val) {
	Node *node;
	Property *property;
	stringstream tmp_str;
	
	if(os.current_node != 0) {
		tmp_str << val;
		switch(os.append_mode) {
		case Logger::normal:
			node = new Node(os.string_float);
			break;
		case Logger::file:
			node = new Node(os.string_file);
			break;
		case Logger::function:
			node = new Node(os.string_function);
			break;
		case Logger::line:
			node = new Node(os.string_line);
			break;
		}
		property = new Property(os.string_value, tmp_str.str());
		*node << *property;
		*os.current_node << *node;
		os.append_mode = Logger::normal;
	}
	return os;
}

Logger &
unisim::service::interfaces::
operator <<(Logger &os, double val) {
	Node *node;
	Property *property;
	stringstream tmp_str;
	
	if(os.current_node != 0) {
		tmp_str << val;
		switch(os.append_mode) {
		case Logger::normal:
			node = new Node(os.string_double);
			break;
		case Logger::file:
			node = new Node(os.string_file);
			break;
		case Logger::function:
			node = new Node(os.string_function);
			break;
		case Logger::line:
			node = new Node(os.string_line);
			break;
		}
		property = new Property(os.string_value, tmp_str.str());
		*node << *property;
		*os.current_node << *node;
		os.append_mode = Logger::normal;
	}
	return os;
}

Logger &
unisim::service::interfaces::
operator <<(Logger &os,
	Logger& (*fn)(Logger&))	{ 
	return (*fn)(os); 
}
		
Logger &
unisim::service::interfaces::
Hex(Logger &os) {
	os.num_status = Logger::hexadecimal;
	os.append_mode = Logger::normal;
	return os;
}

Logger &
unisim::service::interfaces::
Dec(Logger &os) {
	os.num_status = Logger::decimal;
	os.append_mode = Logger::normal;
	return os;
}

Logger &
unisim::service::interfaces::
Endl(Logger &os) {
	Node *node = new Node(os.string_endl);

	if(os.current_node != 0) {
		*os.current_node << *node;
		os.append_mode = Logger::normal;
	}
	return os;	
}

Logger &
unisim::service::interfaces::
Flush(Logger &os) {
	os.Flush();
	os.append_mode = Logger::normal;
	return os;
}

Logger &
unisim::service::interfaces::
DebugInfo(Logger &os) {
	Property *property;
	
	/* if there is a current node, then we can do nothing, first current
	 *   node should be close
	 *  NOTE: this is an error */
	if(os.current_node != 0)
		return os;
	os.current_node = new Node(os.string_debug_info);
	os.append_mode = Logger::normal;
//	property = new Property(os.string_source, os.name);
//	*os.current_node << *property;
	return os;
}

Logger &
unisim::service::interfaces::
EndDebugInfo(Logger &os) {
	os.Message(os.current_node);
	os.current_node = 0;
	os.append_mode = Logger::normal;
	return os;
}

Logger &
unisim::service::interfaces::
DebugWarning(Logger &os) {
	Property *property;
	
	/* if there is a current node, then we can do nothing, first current
	 *   node should be close
	 *  NOTE: this is an error */
	if(os.current_node != 0)
		return os;
	os.current_node = new Node(os.string_debug_warning);
	os.append_mode = Logger::normal;
//	property = new Property(os.string_source, os.name);
//	*os.current_node << *property;
	return os;
}

Logger &
unisim::service::interfaces::
EndDebugWarning(Logger &os) {
	os.Message(os.current_node);
	os.current_node = 0;
	os.append_mode = Logger::normal;
	return os;
}

Logger &
unisim::service::interfaces::
DebugError(Logger &os) {
	Property *property;
	
	/* if there is a current node, then we can do nothing, first current
	 *   node should be close
	 *  NOTE: this is an error */
	if(os.current_node != 0)
		return os;
	os.current_node = new Node(os.string_debug_error);
	os.append_mode = Logger::normal;
//	property = new Property(os.string_source, os.name);
//	*os.current_node << *property;
	return os;
}

Logger &
unisim::service::interfaces::
EndDebugError(Logger &os) {
	os.Message(os.current_node);
	os.current_node = 0;
	os.append_mode = Logger::normal;
	return os;
}

Logger &
unisim::service::interfaces::
File(Logger &os) {
	if(os.current_node != 0)
		 os.append_mode = Logger::file;
	return os;
}

Logger &
unisim::service::interfaces::
Function(Logger &os) {
	if(os.current_node != 0)
		os.append_mode = Logger::function;
	return os;
}

Logger &
unisim::service::interfaces::
Line(Logger &os) {
	if(os.current_node != 0)
		os.append_mode = Logger::line;
	return os;
}
