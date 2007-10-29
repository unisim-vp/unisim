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

#include "unisim/service/interfaces/logger.hh"
#include "unisim/service/interfaces/time.hh"
#include "unisim/service/logger/logger_server.hh"
#include "unisim/service/logger/logger_server_interface.hh"
#include "unisim/service/logger/logger.hh"
#include "unisim/util/xml/xml.hh"
#include "unisim/kernel/service/service.hh"
#include <sstream>
#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>

namespace unisim {
namespace service {
namespace logger {

using unisim::service::interfaces::Time;
using unisim::util::xml::Node;
using unisim::util::xml::PtrList;
using unisim::util::xml::Property;
using unisim::kernel::service::Object;
using unisim::kernel::service::Client;
using unisim::kernel::service::ServiceExport;
using unisim::kernel::service::ServiceImport;
using unisim::kernel::service::Parameter;

using namespace std;

static const char *XML_ENCODING = "UTF-8"; 

typedef unisim::service::interfaces::Logger LoggerIF;

/* Constructor and destructor start */

LoggerServer::LoggerServer(const char *name, Object *parent) :
	Object(name, parent),
	Client<Time>(name, parent),
	time_import("time_import", this),
	filename(""),
	param_filename("filename", this, filename),
	zip(false),
	param_zip("zip", this, zip),
	std_out(false),
	param_std_out("std_out", this, std_out),
	std_err(false),
	param_std_err("std_err", this, std_err),
	show_time(false),
	param_show_time("show-time", this, show_time),
	show_function(false),
	param_show_function("show-function", this, show_function),
	show_file(false),
	param_show_file("show-file", this, show_file),
	show_line(false),
	param_show_line("show-line", this, show_line),
	node_list(),
	writer(NULL) {
	/* create the individual loggers */
	for(unsigned int i = 0; i < MAX_LOGGERS; i++) {
		stringstream s;
		s << "logger[" << i << "]";
		logger[i] = new Logger(s.str().c_str(), this);
	}
	/* create the exports */
	for(unsigned int i = 0; i < MAX_LOGGERS; i++) {
		stringstream s;
		s << "logger_export[" << i << "]";
		logger_export[i] = new ServiceExport<LoggerIF>(s.str().c_str(), this);
	}
	/* connect the server ports to the services providing each port */
	for(unsigned int i = 0; i < MAX_LOGGERS; i++) 
		(*logger_export[i]) >> logger[i]->logger_export;
	/* set the loggers callback to the logger server */
	for(unsigned int i = 0; i < MAX_LOGGERS; i++)
		logger[i]->SetServer(this);
}

LoggerServer::
~LoggerServer() {
	if(writer != NULL) {
		int rc = xmlTextWriterEndElement(writer);
		if(rc < 0) {
			cerr << "Error(LoggerServer::OnDisconnect): "
				<< "could not close the root element" << endl;
		}
		rc = xmlTextWriterEndDocument(writer);
		if(rc < 0) {
			cerr << "Warning(LoggerServer::OnDisconnect): "
				<< "could not correctly close the XMLWriter" << endl;
		}
		xmlFreeTextWriter(writer);
	}

	if(node_list != 0) delete node_list;
	for(unsigned int i = 0; i < MAX_LOGGERS; i++) {
		if(logger[i]) delete logger[i];
	}
	for(unsigned int i = 0; i < MAX_LOGGERS; i++) {
		if(logger_export[i]) delete logger_export[i];
	}
}

/* Constructor and destructor stop */

/* Service methods end */
bool
LoggerServer::
Setup() {
	/* Open the file if available, otherwise
	 *   no file will be used.
	 * If the file opening fails, return false,
	 *   otherwise return true. */
	cerr << "INFO(LoggerServer::ClientIndependentSetup): "
		<< " filename name = " << filename << endl;
	if(filename != "") {
		stringstream filename_str;
		filename_str << filename;
		if(zip) filename_str << ".gz";
		writer = xmlNewTextWriterFilename(filename_str.str().c_str(), zip?1:0);
		if(writer == NULL) {
			cerr << "Error(LoggerServer::Setup): "
				<< "could not open output file for logging" << endl;
			return false;
		}
		int rc = xmlTextWriterSetIndent(writer, 0);
		if(rc < 0) {
			cerr << "Warning(LoggerServer::Setup): could not set indentation" << endl;
		}
		rc = xmlTextWriterStartDocument(writer, NULL, XML_ENCODING, NULL);
		if(rc < 0) {
			cerr << "Error(LoggerServer::Setup): "
				<< "error starting the xml document" << endl;
			return false;
		}
		rc = xmlTextWriterStartElement(writer, BAD_CAST "LOGGER");
		if(rc < 0) {
			cerr << "Error(LoggerServer::Setup): "
				<< "error starting the xml document" << endl;
			return false;
		}
	}
	
	return true;
}

void
LoggerServer::
OnDisconnect() {
	if(writer != NULL) { 
		int rc = xmlTextWriterEndElement(writer);
		if(rc < 0) {
			cerr << "Error(LoggerServer::OnDisconnect): "
				<< "could not close the root element" << endl;
		}
		rc = xmlTextWriterEndDocument(writer);
		if(rc < 0) {
			cerr << "Warning(LoggerServer::OnDisconnect): "
				<< "could not correctly close the XMLWriter" << endl;
		}
		xmlFreeTextWriter(writer);
	}
}

/* Service methods stop */

void
LoggerServer::
Message(Node *node) {
	Property *property;

	if(node_list == 0)
		node_list = new PtrList<Node>();
	if(time_import) {
		double time = time_import->GetTime();
		stringstream tmp_str;
		
		tmp_str << time;
		property = new Property(Logger::string_time, tmp_str.str());
		*node << *property;
	}
	node_list->push_back(node);
	Flush();
}

void
LoggerServer::
Flush() {
	/* if the node list is empty, then do nothing */
	if(node_list == 0)
		return;
	/* iterate through all the nodes in the list */
	PtrList<Node>::iterator it;
	for(it = node_list->begin(); it != node_list->end(); it++) {
		FlushMessage(**it);
	}
	delete node_list;
	node_list = new PtrList<Node>;
}

void
LoggerServer::
FlushMessage(Node &node) {
	if(writer)
		FlushMessage(node, writer);
	
	if(std_out) 
		FlushMessage(node, cout);

	if(std_err)
		FlushMessage(node, cerr);
}

void
LoggerServer::
FlushTime(Node &node, ostream &os) {
	const Property *time = NULL;

	if(show_time) {
		time = node.GetProperty(Logger::string_time);
		if(time != NULL) {
			os << time->Value() << "secs ";
		}
	}
}

void 
LoggerServer::
FlushMessage(Node &node, ostream &os) {
	list<Node *> *nodes;
	string object_name = node.GetProperty(Logger::string_source)->Value(); //= node.Name();
		
	nodes = node.Childs();
	list<Node *>::iterator it;
	for(it = nodes->begin();it != nodes->end(); it++) {
		if((**it).Name() == Logger::string_debug_info) {
			os << "\033[36;40m";
			FlushTime(node, os);			
			FlushDebugInfoNode(**it, os, object_name);
			os << "\033[37;40m";
			return;
		}
		if((**it).Name() == Logger::string_debug_warning) {
			os << "\033[33;40m";
			FlushTime(node, os);			
			FlushDebugWarningNode(**it, os, object_name);
			os << "\033[37;40m";
			return;
		}
		if((**it).Name() == Logger::string_debug_error) {
			os << "\033[31;40m";
			FlushTime(node, os);			
			FlushDebugErrorNode(**it, os, object_name);
			os << "\033[37;40m";
			return;
		}
	}
}

void 
LoggerServer::
FlushMessage(Node &node, xmlTextWriterPtr &out) {
	list<Node *> *nodes;
	int rc;
	const Property *object_property = node.GetProperty(Logger::string_source);
	const Property *time_property = node.GetProperty(Logger::string_time);
//	string object_name = node.GetProperty()->Value(); //= node.Name();
//	string time_str = node.GetProperty(Logger::string_time)->Value();
	
	rc = xmlTextWriterStartElement(out, BAD_CAST "MESSAGE");
	if(rc < 0) {
		cerr << "Error(LoggerServer::FlushMessage): couldn't not add an element" << endl;
	}
	if(object_property) { 
		rc = xmlTextWriterWriteAttribute(out, BAD_CAST "source", BAD_CAST object_property->Value().c_str());
		if(rc < 0) 
			cerr << "Error(LoggerServer::FlushMessage): couldn't not append source attribute" << endl;
	} else 
		cerr << "Error(LoggerServer::FlushMessage): couldn't not find source attribute" << endl;
		
	if(time_property) {
		rc = xmlTextWriterWriteAttribute(out, BAD_CAST "time", BAD_CAST time_property->Value().c_str());
		if(rc < 0) {
			cerr << "Error(LoggerServer::FlushMessage): couldn't not append time attribute" << endl;
		}
	}
		
	
	nodes = node.Childs();
	list<Node *>::iterator it;
	for(it = nodes->begin();it != nodes->end(); it++) {
		if((**it).Name() == Logger::string_debug_info) {
			FlushDebugInfoNode(**it, out);
		}
		if((**it).Name() == Logger::string_debug_warning) {
			FlushDebugWarningNode(**it, out);
		}
		if((**it).Name() == Logger::string_debug_error) {
			FlushDebugErrorNode(**it, out);
		}
	}
	xmlTextWriterEndElement(out);
	if(rc < 0) {
		cerr << "Error(LoggerServer::FlushMessage): couldn't not close element" << endl;
	}
}

void
LoggerServer::
FlushLocation(Node &node, ostream &os) {
	list<Node *> *nodes;
	const Property *file = NULL;
	const Property *function = NULL;
	const Property *line = NULL;
		
	nodes = node.Childs();
	list<Node *>::iterator it;
	for(it = nodes->begin();it != nodes->end(); it++) {
		if(show_file) {
			if((**it).Name() == Logger::string_file) {
				file = (**it).GetProperty(Logger::string_value);
			}
		}
		if(show_function) {
			if((**it).Name() == Logger::string_function) {
				function = (**it).GetProperty(Logger::string_value);
			}
		}
		if(show_line) {
			if((**it).Name() == Logger::string_line) {
				line = (**it).GetProperty(Logger::string_value);
			}
		}
	}
	if(file != NULL ||
		function != NULL ||
		line != NULL) {
		if(show_file || show_function || show_line) os << ",";
		if(show_file && file != NULL)
			os << file->Value();
		if(show_function && function != NULL) {
			if(file != NULL) os << ":";
			os << function->Value();
		}
		if(show_line && line != NULL) {
			if(file != NULL || function != NULL) os << ":";
			os << line->Value();
		}
	}
}

void
LoggerServer::
FlushLocation(Node &node, xmlTextWriterPtr &out) {
	list<Node *> *nodes;
	const Property *file = NULL;
	const Property *function = NULL;
	const Property *line = NULL;
	int rc;
	
	nodes = node.Childs();
	list<Node *>::iterator it;
	for(it = nodes->begin();it != nodes->end(); it++) {
		if((**it).Name() == Logger::string_file) {
			file = (**it).GetProperty(Logger::string_value);
		}
		if((**it).Name() == Logger::string_function) {
			function = (**it).GetProperty(Logger::string_value);
		}
		if((**it).Name() == Logger::string_line) {
			line = (**it).GetProperty(Logger::string_value);
		}
	}
	if(file != NULL ||
		function != NULL ||
		line != NULL) {
		if(file != NULL) {
			rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "file", BAD_CAST file->Value().c_str());
			if(rc < 0) 
				cerr << "Error(LoggerServer::FlushLocation): error while writing file location" << endl;
		}
		if(function != NULL) {
			rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "function", BAD_CAST function->Value().c_str());
			if(rc < 0) 
				cerr << "Error(LoggerServer::FlushLocation): error while writing function location" << endl;
		}
		if(line != NULL) {
			rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "line", BAD_CAST line->Value().c_str());
			if(rc < 0) 
				cerr << "Error(LoggerServer::FlushLocation): error while writing line location" << endl;
		}
	}
}

void
LoggerServer::
FlushDebugInfoNode(Node &node, ostream &os, string &object_name) {
	os << "INFO(" << object_name;
	FlushLocation(node, os);
	os << "): ";
	FlushDebugNode(node, os);
}

void
LoggerServer::
FlushDebugWarningNode(Node &node, ostream &os, string &object_name) {
	os << "WARNING(" << object_name;
	FlushLocation(node, os);
	os << "): ";
	FlushDebugNode(node, os);
}

void
LoggerServer::
FlushDebugErrorNode(Node &node, ostream &os, string &object_name) {
	os << "ERROR(" << object_name;
	FlushLocation(node, os);
	os << "): ";
	FlushDebugNode(node, os);
}

void 
LoggerServer::
FlushDebugInfoNode(Node &node, xmlTextWriterPtr &out) {
	int rc;
	
	rc = xmlTextWriterStartElement(writer, BAD_CAST Logger::string_debug_info.c_str());
	if(rc < 0) {
		cerr << "Error(LoggerServer::FlushMessage): couldn't write debug info element" << endl;
	}
	FlushLocation(node, out);
	FlushDebugNode(node, out);
	rc = xmlTextWriterEndElement(writer);
	if(rc < 0) {
		cerr << "Error(LoggerServer::FlushMessage): couldn't close debug info element" << endl;
	}
}

void 
LoggerServer::
FlushDebugWarningNode(Node &node, xmlTextWriterPtr &out) {
	int rc;
	
	rc = xmlTextWriterStartElement(writer, BAD_CAST Logger::string_debug_warning.c_str());
	if(rc < 0) {
		cerr << "Error(LoggerServer::FlushMessage): couldn't write debug warning element" << endl;
	}
	FlushLocation(node, out);
	FlushDebugNode(node, out);
	rc = xmlTextWriterEndElement(writer);
	if(rc < 0) {
		cerr << "Error(LoggerServer::FlushMessage): couldn't close debug warning element" << endl;
	}
}

void 
LoggerServer::
FlushDebugErrorNode(Node &node, xmlTextWriterPtr &out) {
	int rc;
	
	rc = xmlTextWriterStartElement(writer, BAD_CAST Logger::string_debug_error.c_str());
	if(rc < 0) {
		cerr << "Error(LoggerServer::FlushMessage): couldn't write debug error element" << endl;
	}
	FlushLocation(node, out);
	FlushDebugNode(node, out);
	rc = xmlTextWriterEndElement(writer);
	if(rc < 0) {
		cerr << "Error(LoggerServer::FlushMessage): couldn't close debug error element" << endl;
	}
}

void
LoggerServer::
FlushDebugNode(Node &node, ostream &os) {
	list<Node *> *nodes;
	
	nodes = node.Childs();
	list<Node *>::iterator it;
	for(it = nodes->begin();it != nodes->end(); it++) {
		if((**it).Name() == Logger::string_bool) {
			FlushBoolNode(**it, os);
			continue;
		}
		if((**it).Name() == Logger::string_string) {
			FlushStringNode(**it, os);
			continue;
		}
		if((**it).Name() == Logger::string_char) {
			FlushCharNode(**it, os);
			continue;
		}
		if((**it).Name() == Logger::string_int) {
			FlushIntNode(**it, os);
			continue;
		}
		if((**it).Name() == Logger::string_short_int) {
			FlushShortIntNode(**it, os);
			continue;
		}
		if((**it).Name() == Logger::string_long_int) {
			FlushLongIntNode(**it, os);
			continue;
		}
		if((**it).Name() == Logger::string_long_long_int) {
			FlushLongLongIntNode(**it, os);
			continue;
		}
		if((**it).Name() == Logger::string_unsigned_char) {
			FlushUnsignedCharNode(**it, os);
			continue;
		}
		if((**it).Name() == Logger::string_unsigned_int) {
			FlushUnsignedIntNode(**it, os);
			continue;
		}
		if((**it).Name() == Logger::string_unsigned_short_int) {
			FlushUnsignedShortIntNode(**it, os);
			continue;
		}
		if((**it).Name() == Logger::string_unsigned_long_int) {
			FlushUnsignedLongIntNode(**it, os);
			continue;
		}
		if((**it).Name() == Logger::string_unsigned_long_long_int) {
			FlushUnsignedLongLongIntNode(**it, os);
			continue;
		}
		if((**it).Name() == Logger::string_float) {
			FlushFloatNode(**it, os);
			continue;
		}
		if((**it).Name() == Logger::string_double) {
			FlushDoubleNode(**it, os);
			continue;
		}
		if((**it).Name() == Logger::string_endl) {
			FlushEndlNode(**it, os);
			continue;
		}
	}
}

void
LoggerServer::
FlushDebugNode(Node &node, xmlTextWriterPtr &out) {
	list<Node *> *nodes;
	
	nodes = node.Childs();
	list<Node *>::iterator it;
	for(it = nodes->begin();it != nodes->end(); it++) {
		if((**it).Name() == Logger::string_bool) {
			FlushBoolNode(**it, out);
			continue;
		}
		if((**it).Name() == Logger::string_string) {
			FlushStringNode(**it, out);
			continue;
		}
		if((**it).Name() == Logger::string_char) {
			FlushCharNode(**it, out);
			continue;
		}
		if((**it).Name() == Logger::string_int) {
			FlushIntNode(**it, out);
			continue;
		}
		if((**it).Name() == Logger::string_short_int) {
			FlushShortIntNode(**it, out);
			continue;
		}
		if((**it).Name() == Logger::string_long_int) {
			FlushLongIntNode(**it, out);
			continue;
		}
		if((**it).Name() == Logger::string_long_long_int) {
			FlushLongLongIntNode(**it, out);
			continue;
		}
		if((**it).Name() == Logger::string_unsigned_char) {
			FlushUnsignedCharNode(**it, out);
			continue;
		}
		if((**it).Name() == Logger::string_unsigned_int) {
			FlushUnsignedIntNode(**it, out);
			continue;
		}
		if((**it).Name() == Logger::string_unsigned_short_int) {
			FlushUnsignedShortIntNode(**it, out);
			continue;
		}
		if((**it).Name() == Logger::string_unsigned_long_int) {
			FlushUnsignedLongIntNode(**it, out);
			continue;
		}
		if((**it).Name() == Logger::string_unsigned_long_long_int) {
			FlushUnsignedLongLongIntNode(**it, out);
			continue;
		}
		if((**it).Name() == Logger::string_float) {
			FlushFloatNode(**it, out);
			continue;
		}
		if((**it).Name() == Logger::string_double) {
			FlushDoubleNode(**it, out);
			continue;
		}
		if((**it).Name() == Logger::string_endl) {
			FlushEndlNode(**it, out);
			continue;
		}
	}
}

void
LoggerServer::
FlushBoolNode(Node &node, xmlTextWriterPtr &out) {
	const Property *value = node.GetProperty(Logger::string_value);
	int rc = xmlTextWriterStartElement(out, BAD_CAST Logger::string_bool.c_str());
	if(rc < 0)
		cerr << "Error(LoggerServer::FlushBoolNode): error" << endl;
	rc = xmlTextWriterWriteFormatString(out, "%s", value->Value().c_str());
	if(rc < 0)
		cerr << "Error(LoggerServer::FlushBoolNode): error" << endl;
	xmlTextWriterEndElement(out);
}

void
LoggerServer::
FlushBoolNode(Node &node, ostream &os) {
	const Property *value = node.GetProperty(Logger::string_value);
	os << value->Value();
}

void
LoggerServer::
FlushStringNode(Node &node, xmlTextWriterPtr &out) {
	const Property *value = node.GetProperty(Logger::string_value);
	int rc = xmlTextWriterWriteFormatString(out, "%s", value->Value().c_str());
	if(rc < 0)
		cerr << "Error(LoggerServer::FlushStringNode): error" << endl;
}

void
LoggerServer::
FlushStringNode(Node &node, ostream &os) {
	const Property *value = node.GetProperty(Logger::string_value);
	os << value->Value();
}

void
LoggerServer::
FlushCharNode(Node &node, xmlTextWriterPtr &out) {
	const Property *value = node.GetProperty(Logger::string_value);
	int rc = xmlTextWriterWriteFormatString(out, "%s", value->Value().c_str());
	if(rc < 0)
		cerr << "Error(LoggerServer::FlushCharNode): error" << endl;
}

void
LoggerServer::
FlushCharNode(Node &node, ostream &os) {
	const Property *value = node.GetProperty(Logger::string_value);
	const Property *mode = node.GetProperty(Logger::string_mode);
	stringstream str;
	char ch;
	
	str << value->Value();
	str >> ch;
	os << (char)ch;
}

void
LoggerServer::
FlushIntNode(Node &node, xmlTextWriterPtr &out) {
	const Property *value = node.GetProperty(Logger::string_value);
	const Property *mode = node.GetProperty(Logger::string_mode);
	int rc = xmlTextWriterStartElement(out, BAD_CAST Logger::string_int.c_str());
	if(rc < 0)
		cerr << "Error(LoggerServer::FlushIntNode): error" << endl;
	if(mode->Value() == Logger::string_hex) {
		rc = xmlTextWriterWriteAttribute(out, BAD_CAST "mode", BAD_CAST Logger::string_hex.c_str());
		if(rc < 0)
			cerr << "Error(LoggerServer::FlushIntNode): error" << endl;
	}
	rc = xmlTextWriterWriteFormatString(out, "%s", value->Value().c_str());
	if(rc < 0)
		cerr << "Error(LoggerServer::FlushIntNode): error" << endl;
	xmlTextWriterEndElement(out);
}

void
LoggerServer::
FlushIntNode(Node &node, ostream &os) {
	const Property *value = node.GetProperty(Logger::string_value);
	const Property *mode = node.GetProperty(Logger::string_mode);
	stringstream str;
	int i;
	
	str << value->Value();
	str >> i;
	if(mode->Value() == Logger::string_hex)
		os << hex;
	os << i;
	os << dec;
}

void
LoggerServer::
FlushShortIntNode(Node &node, xmlTextWriterPtr &out) {
	const Property *value = node.GetProperty(Logger::string_value);
	const Property *mode = node.GetProperty(Logger::string_mode);
	int rc = xmlTextWriterStartElement(out, BAD_CAST Logger::string_short_int.c_str());
	if(rc < 0)
		cerr << "Error(LoggerServer::FlushShortIntNode): error" << endl;
	if(mode->Value() == Logger::string_hex) {
		rc = xmlTextWriterWriteAttribute(out, BAD_CAST "mode", BAD_CAST Logger::string_hex.c_str());
		if(rc < 0)
			cerr << "Error(LoggerServer::FlushShortIntNode): error" << endl;
	}
	rc = xmlTextWriterWriteFormatString(out, "%s", value->Value().c_str());
	if(rc < 0)
		cerr << "Error(LoggerServer::FlushShortIntNode): error" << endl;
	xmlTextWriterEndElement(out);
}

void
LoggerServer::
FlushShortIntNode(Node &node, ostream &os) {
	const Property *value = node.GetProperty(Logger::string_value);
	const Property *mode = node.GetProperty(Logger::string_mode);
	stringstream str;
	short int si;
	
	str << value->Value();
	str >> si;
	if(mode->Value() == Logger::string_hex)
		os << hex;
	os << si;
	os << dec;
}

void
LoggerServer::
FlushLongIntNode(Node &node, xmlTextWriterPtr &out) {
	const Property *value = node.GetProperty(Logger::string_value);
	const Property *mode = node.GetProperty(Logger::string_mode);
	int rc = xmlTextWriterStartElement(out, BAD_CAST Logger::string_long_int.c_str());
	if(rc < 0)
		cerr << "Error(LoggerServer::FlushLongIntNode): error" << endl;
	if(mode->Value() == Logger::string_hex) {
		rc = xmlTextWriterWriteAttribute(out, BAD_CAST "mode", BAD_CAST Logger::string_hex.c_str());
		if(rc < 0)
			cerr << "Error(LoggerServer::FlushLongIntNode): error" << endl;
	}
	rc = xmlTextWriterWriteFormatString(out, "%s", value->Value().c_str());
	if(rc < 0)
		cerr << "Error(LoggerServer::FlushLongIntNode): error" << endl;
	xmlTextWriterEndElement(out);
}

void
LoggerServer::
FlushLongIntNode(Node &node, ostream &os) {
	const Property *value = node.GetProperty(Logger::string_value);
	const Property *mode = node.GetProperty(Logger::string_mode);
	stringstream str;
	long int li;
	
	str << value->Value();
	str >> li;
	if(mode->Value() == Logger::string_hex)
		os << hex;
	os << li;
	os << dec;
}

void
LoggerServer::
FlushLongLongIntNode(Node &node, xmlTextWriterPtr &out) {
	const Property *value = node.GetProperty(Logger::string_value);
	const Property *mode = node.GetProperty(Logger::string_mode);
	int rc = xmlTextWriterStartElement(out, BAD_CAST Logger::string_long_long_int.c_str());
	if(rc < 0)
		cerr << "Error(LoggerServer::FlushLongLongIntNode): error" << endl;
	if(mode->Value() == Logger::string_hex) {
		rc = xmlTextWriterWriteAttribute(out, BAD_CAST "mode", BAD_CAST Logger::string_hex.c_str());
		if(rc < 0)
			cerr << "Error(LoggerServer::FlushLongLongIntNode): error" << endl;
	}
	rc = xmlTextWriterWriteFormatString(out, "%s", value->Value().c_str());
	if(rc < 0)
		cerr << "Error(LoggerServer::FlushLongLongIntNode): error" << endl;
	xmlTextWriterEndElement(out);
}

void
LoggerServer::
FlushLongLongIntNode(Node &node, ostream &os) {
	const Property *value = node.GetProperty(Logger::string_value);
	const Property *mode = node.GetProperty(Logger::string_mode);
	stringstream str;
	long long int lli;
	
	str << value->Value();
	str >> lli;
	if(mode->Value() == Logger::string_hex)
		os << hex;
	os << lli;
	os << dec;
}

void
LoggerServer::
FlushUnsignedCharNode(Node &node, xmlTextWriterPtr &out) {
	const Property *value = node.GetProperty(Logger::string_value);
	int rc = xmlTextWriterWriteFormatString(out, "%s", value->Value().c_str());
	if(rc < 0)
		cerr << "Error(LoggerServer::FlushUnsignedCharNode): error" << endl;
}

void
LoggerServer::
FlushUnsignedCharNode(Node &node, ostream &os) {
	const Property *value = node.GetProperty(Logger::string_value);
	stringstream str;
	unsigned char uc;
	
	str << value->Value();
	str >> uc;
	os << (unsigned char)uc;
}

void
LoggerServer::
FlushUnsignedIntNode(Node &node, xmlTextWriterPtr &out) {
	const Property *value = node.GetProperty(Logger::string_value);
	const Property *mode = node.GetProperty(Logger::string_mode);
	int rc = xmlTextWriterStartElement(out, BAD_CAST Logger::string_unsigned_int.c_str());
	if(rc < 0)
		cerr << "Error(LoggerServer::FlushUnsignedIntNode): error" << endl;
	if(mode->Value() == Logger::string_hex) {
		rc = xmlTextWriterWriteAttribute(out, BAD_CAST "mode", BAD_CAST Logger::string_hex.c_str());
		if(rc < 0)
			cerr << "Error(LoggerServer::FlushUnsignedIntNode): error" << endl;
	}
	rc = xmlTextWriterWriteFormatString(out, "%s", value->Value().c_str());
	if(rc < 0)
		cerr << "Error(LoggerServer::FlushUnsignedIntNode): error" << endl;
	xmlTextWriterEndElement(out);
}

void
LoggerServer::
FlushUnsignedIntNode(Node &node, ostream &os) {
	const Property *value = node.GetProperty(Logger::string_value);
	const Property *mode = node.GetProperty(Logger::string_mode);
	stringstream str;
	unsigned int ui;
	
	str << value->Value();
	str >> ui;
	if(mode->Value() == Logger::string_hex)
		os << hex;
	os << ui;
	os << dec;
}

void
LoggerServer::
FlushUnsignedShortIntNode(Node &node, xmlTextWriterPtr &out) {
	const Property *value = node.GetProperty(Logger::string_value);
	const Property *mode = node.GetProperty(Logger::string_mode);
	int rc = xmlTextWriterStartElement(out, BAD_CAST Logger::string_unsigned_short_int.c_str());
	if(rc < 0)
		cerr << "Error(LoggerServer::FlushUnsignedShortIntNode): error" << endl;
	if(mode->Value() == Logger::string_hex) {
		rc = xmlTextWriterWriteAttribute(out, BAD_CAST "mode", BAD_CAST Logger::string_hex.c_str());
		if(rc < 0)
			cerr << "Error(LoggerServer::FlushUnsignedShortIntNode): error" << endl;
	}
	rc = xmlTextWriterWriteFormatString(out, "%s", value->Value().c_str());
	if(rc < 0)
		cerr << "Error(LoggerServer::FlushUnsignedShortIntNode): error" << endl;
	xmlTextWriterEndElement(out);
}

void
LoggerServer::
FlushUnsignedShortIntNode(Node &node, ostream &os) {
	const Property *value = node.GetProperty(Logger::string_value);
	const Property *mode = node.GetProperty(Logger::string_mode);
	stringstream str;
	unsigned short int usi;
	
	str << value->Value();
	str >> usi;
	if(mode->Value() == Logger::string_hex)
		os << hex;
	os << usi;
	os << dec;
}

void
LoggerServer::
FlushUnsignedLongIntNode(Node &node, xmlTextWriterPtr &out) {
	const Property *value = node.GetProperty(Logger::string_value);
	const Property *mode = node.GetProperty(Logger::string_mode);
	int rc = xmlTextWriterStartElement(out, BAD_CAST Logger::string_unsigned_long_int.c_str());
	if(rc < 0)
		cerr << "Error(LoggerServer::FlushUnsignedLongIntNode): error" << endl;
	if(mode->Value() == Logger::string_hex) {
		rc = xmlTextWriterWriteAttribute(out, BAD_CAST "mode", BAD_CAST Logger::string_hex.c_str());
		if(rc < 0)
			cerr << "Error(LoggerServer::FlushUnsignedLongIntNode): error" << endl;
	}
	rc = xmlTextWriterWriteFormatString(out, "%s", value->Value().c_str());
	if(rc < 0)
		cerr << "Error(LoggerServer::FlushUnsignedLongIntNode): error" << endl;
	xmlTextWriterEndElement(out);
}

void
LoggerServer::
FlushUnsignedLongIntNode(Node &node, ostream &os) {
	const Property *value = node.GetProperty(Logger::string_value);
	const Property *mode = node.GetProperty(Logger::string_mode);
	stringstream str;
	unsigned long int uli;
	
	str << value->Value();
	str >> uli;
	if(mode->Value() == Logger::string_hex)
		os << hex;
	os << uli;
	os << dec;
}

void
LoggerServer::
FlushUnsignedLongLongIntNode(Node &node, xmlTextWriterPtr &out) {
	const Property *value = node.GetProperty(Logger::string_value);
	const Property *mode = node.GetProperty(Logger::string_mode);
	int rc = xmlTextWriterStartElement(out, BAD_CAST Logger::string_unsigned_long_long_int.c_str());
	if(rc < 0)
		cerr << "Error(LoggerServer::FlushUnsignedLongLongIntNode): error" << endl;
	if(mode->Value() == Logger::string_hex) {
		rc = xmlTextWriterWriteAttribute(out, BAD_CAST "mode", BAD_CAST Logger::string_hex.c_str());
		if(rc < 0)
			cerr << "Error(LoggerServer::FlushUnsignedLongLongIntNode): error" << endl;
	}
	rc = xmlTextWriterWriteFormatString(out, "%s", value->Value().c_str());
	if(rc < 0)
		cerr << "Error(LoggerServer::FlushUnsignedLongLongIntNode): error" << endl;
	xmlTextWriterEndElement(out);
}

void
LoggerServer::
FlushUnsignedLongLongIntNode(Node &node, ostream &os) {
	const Property *value = node.GetProperty(Logger::string_value);
	const Property *mode = node.GetProperty(Logger::string_mode);
	stringstream str;
	unsigned long long int ulli;
	
	str << value->Value();
	str >> ulli;
	if(mode->Value() == Logger::string_hex)
		os << hex;
	os << ulli;
	os << dec;
}

void
LoggerServer::
FlushFloatNode(Node &node, xmlTextWriterPtr &out) {
	const Property *value = node.GetProperty(Logger::string_value);
	const Property *mode = node.GetProperty(Logger::string_mode);
	int rc = xmlTextWriterStartElement(out, BAD_CAST Logger::string_float.c_str());
	if(rc < 0)
		cerr << "Error(LoggerServer::FlushFloatNode): error" << endl;
	rc = xmlTextWriterWriteFormatString(out, "%s", value->Value().c_str());
	if(rc < 0)
		cerr << "Error(LoggerServer::FlushFloatNode): error" << endl;
	xmlTextWriterEndElement(out);
}

void
LoggerServer::
FlushFloatNode(Node &node, ostream &os) {
	const Property *value = node.GetProperty(Logger::string_value);
	stringstream str;
	float f;
	
	str << value->Value();
	str >> f;
	os << f;
}

void
LoggerServer::
FlushDoubleNode(Node &node, xmlTextWriterPtr &out) {
	const Property *value = node.GetProperty(Logger::string_value);
	const Property *mode = node.GetProperty(Logger::string_mode);
	int rc = xmlTextWriterStartElement(out, BAD_CAST Logger::string_double.c_str());
	if(rc < 0)
		cerr << "Error(LoggerServer::FlushDoubleNode): error" << endl;
	rc = xmlTextWriterWriteFormatString(out, "%s", value->Value().c_str());
	if(rc < 0)
		cerr << "Error(LoggerServer::FlushDoubleNode): error" << endl;
	xmlTextWriterEndElement(out);
}

void
LoggerServer::
FlushDoubleNode(Node &node, ostream &os) {
	const Property *value = node.GetProperty(Logger::string_value);
	stringstream str;
	double d;
	
	str << value->Value();
	str >> d;
	os << d;
}

void
LoggerServer::
FlushEndlNode(Node &node, xmlTextWriterPtr &out) {
	int rc = xmlTextWriterStartElement(out, BAD_CAST Logger::string_endl.c_str());
	if(rc < 0)
		cerr << "Error(LoggerServer::FlushEndlNode): error" << endl;
	xmlTextWriterEndElement(out);
}

void
LoggerServer::
FlushEndlNode(Node &node, ostream &os) {
	os << endl;	
}

} // end of logger namespace
} // end of plugins namespace
} // end of full_sytem namespace
