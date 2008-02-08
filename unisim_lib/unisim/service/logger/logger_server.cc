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
#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>

#include <iostream>
#include <sstream>
#include <sys/types.h>


#ifdef WIN32

#include <winsock2.h>

#else

#include <sys/socket.h>
#include <netinet/in.h>
#include <linux/un.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <sys/times.h>
#include <fcntl.h>

#endif

#include <unistd.h>
#include <errno.h>

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
	network_enable(false),
	param_network_enable("network-enable", this, network_enable),
	network_hostname("localhost"),
	param_network_hostname("network-hostname", this, network_hostname),
	network_port(0),
	param_network_port("network-port", this, network_port),
	node_list(),
	writer(NULL),
	network_connected(false) {
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
	cerr << "INFO(LoggerServer::Setup): "
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
	/* Setup the network if necessary */
	if(network_enable) {
		cerr << "INFO(LoggerServer::Setup): "
			<< "trying to set the network support" << endl;
		return SetupNetwork();
	}
	
	return true;
}

bool
LoggerServer::
SetupNetwork() {
	struct sockaddr_in sonadr;
	struct sockaddr_un sonadr_un;

	network_socket = socket(PF_INET, SOCK_STREAM, 0);
	if(network_socket < 0)
	{
		cerr << "ERROR(LoggerServer::SetupNetwork): Can't create socket for connection to " << network_hostname
			<< ":" << network_port << endl;
		network_connected = false;
		return network_connected;
	}
	memset((char *) &sonadr, 0, sizeof(sonadr));
	memset((char *) &sonadr_un, 0, sizeof(sonadr_un));
	sonadr.sin_family = AF_INET;
	sonadr.sin_port = htons(network_port);
	sonadr.sin_addr.s_addr = inet_addr(network_hostname.c_str());
	
	cerr << "INFO(LoggerServer::SetupNetwork): Trying to connect to " << network_hostname << endl;

	if(sonadr.sin_addr.s_addr != -1) {
		//host format is xxx.yyy.zzz.ttt
		network_connected = connect(network_socket, (struct sockaddr *) &sonadr, sizeof(sonadr)) != -1;
	} else {
		//host format is www.whereitis.gnu need to ask dns
		struct hostent *hp;
		int i = 0;
		hp = gethostbyname(network_hostname.c_str());
		if(!hp) {
			cerr << "INFO(LoggerServer::SetupNetwork): Was not able to determine IP address from host name for" << network_hostname << ":" << network_port << endl;
		} else {
			while(!network_connected && hp->h_addr_list[i] != NULL) {
				memcpy((char *) &sonadr.sin_addr,
				(char *) hp->h_addr_list[i],
				sizeof(sonadr.sin_addr));
				network_connected = connect(network_socket, (struct sockaddr *) &sonadr, sizeof(sonadr)) != -1;
				i++;
			}
		}
	}
	if(!network_connected) {
#ifdef WIN32
		closesocket(network_socket);
#else
		close(network_socket);
#endif
		network_socket = -1;
		cerr << "INFO(LoggerServer::SetupNetwork): Can't connect to " << network_hostname << ":" << network_port << endl;
		return false;
	}

	return network_connected;
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
	
	if(network_connected)
		FlushMessageOverNetwork(node);
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
	rc = xmlTextWriterEndElement(out);
	if(rc < 0) {
		cerr << "Error(LoggerServer::FlushMessage): couldn't not close element" << endl;
	}
}

void 
LoggerServer::
FlushMessageOverNetwork(Node &node) {
	list<Node *> *nodes;
	int rc;
	const Property *object_property = node.GetProperty(Logger::string_source);
	const Property *time_property = node.GetProperty(Logger::string_time);
	stringstream buffer;
	buffer << NETWORK_MESSAGE_START << ";";
	if(object_property) { 
		buffer << NETWORK_MESSAGE_SOURCE << ":" 
			<< object_property->Value().length() << ":"
			<< object_property->Value() << ";";
	} else 
		cerr << "Error(LoggerServer::FlushMessage): couldn't not find source attribute" << endl;
		
	if(time_property) {
		buffer << NETWORK_MESSAGE_TIME << ":"
			<< time_property->Value().length() << ":"
			<< time_property->Value() << ";";
	}
		
	
	nodes = node.Childs();
	list<Node *>::iterator it;
	for(it = nodes->begin();it != nodes->end(); it++) {
		FlushLocationOverNetwork(**it, buffer);
		if((**it).Name() == Logger::string_debug_info) {
			FlushDebugInfoNodeOverNetwork(**it, buffer);
		}
		if((**it).Name() == Logger::string_debug_warning) {
			FlushDebugWarningNodeOverNetwork(**it, buffer);
		}
		if((**it).Name() == Logger::string_debug_error) {
			FlushDebugErrorNodeOverNetwork(**it, buffer);
		}
	}
	buffer << NETWORK_MESSAGE_END << ";" << endl;
	const char *msg = buffer.str().c_str();
	unsigned int msg_size = buffer.str().length();
	if(msg_size > 0) {
		unsigned int index = 0;
		do
		{
#ifdef WIN32
			int r = send(network_socket, msg + index, msg_size, 0);
			if(r == 0 || r == SOCKET_ERROR)
#else
			ssize_t r = write(network_socket, msg + index, msg_size);
			if(r <= 0)
#endif
			{
				cerr << "NETSTUB: can't write into socket" << endl;
			}

			index += r;
			msg_size -= r;
		}
		while(msg_size > 0);
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
FlushLocationOverNetwork(Node &node, stringstream &buffer) {
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
			buffer << NETWORK_LOCATION_FILE << ":" << file->Value().length() << ":" << file->Value() << ";";
		}
		if(function != NULL) {
			buffer << NETWORK_LOCATION_FUNCTION << ":" << function->Value().length() << ":" << function->Value() << ";";
		}
		if(line != NULL) {
			buffer << NETWORK_LOCATION_LINE << ":" << line->Value() << ";";
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
FlushDebugInfoNodeOverNetwork(Node &node, stringstream &buffer) {
	// steps:
	// - set the start debug info tag for debug node into the buffer
	// - fill the buffer with the node information
	// - set the end debug info tag for debug node into the buffer
	buffer << NETWORK_DEBUG_INFO_START << ";";
	FlushDebugNodeOverNetwork(node, buffer);
	buffer << NETWORK_DEBUG_INFO_END << ";";
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
FlushDebugWarningNodeOverNetwork(Node &node, stringstream &buffer) {
	// steps:
	// - set the start debug info tag for debug node into the buffer
	// - fill the buffer with the node information
	// - set the end debug info tag for debug node into the buffer
	buffer << NETWORK_DEBUG_WARNING_START << ";";
	FlushDebugNodeOverNetwork(node, buffer);
	buffer << NETWORK_DEBUG_WARNING_END << ";";
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
FlushDebugErrorNodeOverNetwork(Node &node, stringstream &buffer) {
	// steps:
	// - set the start debug info tag for debug node into the buffer
	// - fill the buffer with the node information
	// - set the end debug info tag for debug node into the buffer
	buffer << NETWORK_DEBUG_ERROR_START << ";";
	FlushDebugNodeOverNetwork(node, buffer);
	buffer << NETWORK_DEBUG_ERROR_END << ";";
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
FlushDebugNodeOverNetwork(Node &node, stringstream &buffer) {
	list<Node *> *nodes;
	
	nodes = node.Childs();
	list<Node *>::iterator it;
	for(it = nodes->begin();it != nodes->end(); it++) {
		if((**it).Name() == Logger::string_bool) {
			FlushBoolNodeOverNetwork(**it, buffer);
			continue;
		}
		if((**it).Name() == Logger::string_string) {
			FlushStringNodeOverNetwork(**it, buffer);
			continue;
		}
		if((**it).Name() == Logger::string_char) {
			FlushCharNodeOverNetwork(**it, buffer);
			continue;
		}
		if((**it).Name() == Logger::string_int) {
			FlushIntNodeOverNetwork(**it, buffer);
			continue;
		}
		if((**it).Name() == Logger::string_short_int) {
			FlushShortIntNodeOverNetwork(**it, buffer);
			continue;
		}
		if((**it).Name() == Logger::string_long_int) {
			FlushLongIntNodeOverNetwork(**it, buffer);
			continue;
		}
		if((**it).Name() == Logger::string_long_long_int) {
			FlushLongLongIntNodeOverNetwork(**it, buffer);
			continue;
		}
		if((**it).Name() == Logger::string_unsigned_char) {
			FlushUnsignedCharNodeOverNetwork(**it, buffer);
			continue;
		}
		if((**it).Name() == Logger::string_unsigned_int) {
			FlushUnsignedIntNodeOverNetwork(**it, buffer);
			continue;
		}
		if((**it).Name() == Logger::string_unsigned_short_int) {
			FlushUnsignedShortIntNodeOverNetwork(**it, buffer);
			continue;
		}
		if((**it).Name() == Logger::string_unsigned_long_int) {
			FlushUnsignedLongIntNodeOverNetwork(**it, buffer);
			continue;
		}
		if((**it).Name() == Logger::string_unsigned_long_long_int) {
			FlushUnsignedLongLongIntNodeOverNetwork(**it, buffer);
			continue;
		}
		if((**it).Name() == Logger::string_float) {
			FlushFloatNodeOverNetwork(**it, buffer);
			continue;
		}
		if((**it).Name() == Logger::string_double) {
			FlushDoubleNodeOverNetwork(**it, buffer);
			continue;
		}
		if((**it).Name() == Logger::string_endl) {
			FlushEndlNodeOverNetwork(**it, buffer);
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
FlushBoolNodeOverNetwork(Node &node, stringstream &buffer) {
	const Property *value = node.GetProperty(Logger::string_value);
	buffer << NETWORK_BOOL << ":" << value->Value() << ";";
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
FlushStringNodeOverNetwork(Node &node, stringstream &buffer) {
	const Property *value = node.GetProperty(Logger::string_value);
	buffer << NETWORK_STRING << ":" << value->Value().length() << ":" << value->Value() << ";";
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
//	const Property *mode = node.GetProperty(Logger::string_mode);
	stringstream str;
	char ch;
	
	str << value->Value();
	str >> ch;
	os << (char)ch;
}

void
LoggerServer::
FlushCharNodeOverNetwork(Node &node, stringstream &buffer) {
	const Property *value = node.GetProperty(Logger::string_value);
	stringstream str;
	char ch;
	
	str << value->Value();
	str >> ch;
	buffer << NETWORK_CHAR << ":" << (char)ch << ";";
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
FlushIntNodeOverNetwork(Node &node, stringstream &buffer) {
	const Property *value = node.GetProperty(Logger::string_value);
	const Property *mode = node.GetProperty(Logger::string_mode);
	
	buffer << NETWORK_INT << ":"; 
	if(mode->Value() == Logger::string_hex)
		buffer << NETWORK_NUMMODE_HEX;
	else
		buffer << NETWORK_NUMMODE_DEC;
	buffer << ":" << value->Value() << ";";
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
FlushShortIntNodeOverNetwork(Node &node, stringstream &buffer) {
	const Property *value = node.GetProperty(Logger::string_value);
	const Property *mode = node.GetProperty(Logger::string_mode);
	
	buffer << NETWORK_SHORTINT << ":"; 
	if(mode->Value() == Logger::string_hex)
		buffer << NETWORK_NUMMODE_HEX;
	else
		buffer << NETWORK_NUMMODE_DEC;
	buffer << ":" << value->Value() << ";";
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
FlushLongIntNodeOverNetwork(Node &node, stringstream &buffer) {
	const Property *value = node.GetProperty(Logger::string_value);
	const Property *mode = node.GetProperty(Logger::string_mode);
	
	buffer << NETWORK_LONGINT << ":"; 
	if(mode->Value() == Logger::string_hex)
		buffer << NETWORK_NUMMODE_HEX;
	else
		buffer << NETWORK_NUMMODE_DEC;
	buffer << ":" << value->Value() << ";";
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
FlushLongLongIntNodeOverNetwork(Node &node, stringstream &buffer) {
	const Property *value = node.GetProperty(Logger::string_value);
	const Property *mode = node.GetProperty(Logger::string_mode);
	
	buffer << NETWORK_LONGLONGINT << ":"; 
	if(mode->Value() == Logger::string_hex)
		buffer << NETWORK_NUMMODE_HEX;
	else
		buffer << NETWORK_NUMMODE_DEC;
	buffer << ":" << value->Value() << ";";
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
FlushUnsignedCharNodeOverNetwork(Node &node, stringstream &buffer) {
	const Property *value = node.GetProperty(Logger::string_value);
	stringstream str;
	char ch;
	
	str << value->Value();
	str >> ch;
	buffer << NETWORK_UNSIGNEDCHAR << ":" << (unsigned char)ch << ";";
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
FlushUnsignedIntNodeOverNetwork(Node &node, stringstream &buffer) {
	const Property *value = node.GetProperty(Logger::string_value);
	const Property *mode = node.GetProperty(Logger::string_mode);
	
	buffer << NETWORK_UNSIGNEDINT << ":"; 
	if(mode->Value() == Logger::string_hex)
		buffer << NETWORK_NUMMODE_HEX;
	else
		buffer << NETWORK_NUMMODE_DEC;
	buffer << ":" << value->Value() << ";";
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
FlushUnsignedShortIntNodeOverNetwork(Node &node, stringstream &buffer) {
	const Property *value = node.GetProperty(Logger::string_value);
	const Property *mode = node.GetProperty(Logger::string_mode);
	
	buffer << NETWORK_UNSIGNEDSHORTINT << ":"; 
	if(mode->Value() == Logger::string_hex)
		buffer << NETWORK_NUMMODE_HEX;
	else
		buffer << NETWORK_NUMMODE_DEC;
	buffer << ":" << value->Value() << ";";
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
FlushUnsignedLongIntNodeOverNetwork(Node &node, stringstream &buffer) {
	const Property *value = node.GetProperty(Logger::string_value);
	const Property *mode = node.GetProperty(Logger::string_mode);
	
	buffer << NETWORK_UNSIGNEDLONGINT << ":"; 
	if(mode->Value() == Logger::string_hex)
		buffer << NETWORK_NUMMODE_HEX;
	else
		buffer << NETWORK_NUMMODE_DEC;
	buffer << ":" << value->Value() << ";";
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
FlushUnsignedLongLongIntNodeOverNetwork(Node &node, stringstream &buffer) {
	const Property *value = node.GetProperty(Logger::string_value);
	const Property *mode = node.GetProperty(Logger::string_mode);
	
	buffer << NETWORK_UNSIGNEDLONGLONGINT << ":"; 
	if(mode->Value() == Logger::string_hex)
		buffer << NETWORK_NUMMODE_HEX;
	else
		buffer << NETWORK_NUMMODE_DEC;
	buffer << ":" << value->Value() << ";";
}

void
LoggerServer::
FlushFloatNode(Node &node, xmlTextWriterPtr &out) {
	const Property *value = node.GetProperty(Logger::string_value);
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
FlushFloatNodeOverNetwork(Node &node, stringstream &buffer) {
	const Property *value = node.GetProperty(Logger::string_value);
	
	buffer << NETWORK_FLOAT << ":" << value->Value() << ";"; 
}

void
LoggerServer::
FlushDoubleNode(Node &node, xmlTextWriterPtr &out) {
	const Property *value = node.GetProperty(Logger::string_value);
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
FlushDoubleNodeOverNetwork(Node &node, stringstream &buffer) {
	const Property *value = node.GetProperty(Logger::string_value);
	
	buffer << NETWORK_DOUBLE << ":" << value->Value() << ";"; 
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

void
LoggerServer::
FlushEndlNodeOverNetwork(Node &node, stringstream &buffer) {
	buffer << NETWORK_ENDL << ";";
}

} // end of logger namespace
} // end of plugins namespace
} // end of full_sytem namespace
