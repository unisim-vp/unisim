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
 
#ifndef __UNISIM_SERVICE_LOGGER_LOGGERSERVER_HH__
#define __UNISIM_SERVICE_LOGGER_LOGGERSERVER_HH__

#include "unisim/service/interfaces/logger.hh"
#include "unisim/service/logger/logger.hh"
#include "unisim/service/logger/logger_server_interface.hh"
#include "unisim/service/interfaces/time.hh"
#include "unisim/util/xml/xml.hh"
#include "unisim/kernel/service/service.hh"
#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>

namespace unisim {
namespace service {
namespace logger {
	
using unisim::service::logger::Logger;
using unisim::service::logger::LoggerServerInterface;
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

class LoggerServer : 
public LoggerServerInterface,
	public Client<Time> {
private:
	static const unsigned int MAX_LOGGERS = 1024;
	
public:
	/* Exported services */
	ServiceExport<unisim::service::interfaces::Logger> *logger_export[MAX_LOGGERS];
	/* Imported services */
	ServiceImport<Time> time_import;
		
	/* Constructor/Destructor */
	LoggerServer(const char *name, Object *parent = 0);
	virtual ~LoggerServer();
	
	/* Service methods */
	virtual void OnDisconnect();
	virtual bool Setup();

	/* LoggerServerInterface methods required by the individual loggers */
	virtual void Message(Node *node);
	virtual void Flush();
	
private:
	Logger *logger[MAX_LOGGERS];

	Parameter<string> param_filename;
	string filename;
	xmlTextWriterPtr writer;
	Parameter<bool> param_zip;
	bool zip;
	Parameter<bool> param_std_out;
	bool std_out;
	Parameter<bool> param_std_err;
	bool std_err;
	Parameter<bool> param_show_time;
	bool show_time;
	Parameter<bool> param_show_function;
	bool show_function;
	Parameter<bool> param_show_line;
	bool show_line;
	Parameter<bool> param_show_file;
	bool show_file;
	Parameter<bool> param_network_enable;
	bool network_enable;
	Parameter<string> param_network_hostname;
	string network_hostname;
	Parameter<uint32_t> param_network_port;
	uint32_t network_port;
	PtrList<Node> *node_list;

	void FlushMessage(Node &node);
	void FlushMessage(Node &node, ostream &os);
	void FlushDebugNode(Node &node, ostream &os);
	void FlushLocation(Node &node, ostream &os);
	void FlushTime(Node &node, ostream &os);
	void FlushDebugInfoNode(Node &node, ostream &os, string &object_name);
	void FlushDebugWarningNode(Node &node, ostream &os, string &object_name);
	void FlushDebugErrorNode(Node &node, ostream &os, string &object_name);
	void FlushBoolNode(Node &node, ostream &os);
	void FlushStringNode(Node &node, ostream &os);
	void FlushCharNode(Node &node, ostream &os);
	void FlushIntNode(Node &node, ostream &os);
	void FlushShortIntNode(Node &node, ostream &os);
	void FlushLongIntNode(Node &node, ostream &os);
	void FlushLongLongIntNode(Node &node, ostream &os);
	void FlushUnsignedCharNode(Node &node, ostream &os);
	void FlushUnsignedIntNode(Node &node, ostream &os);
	void FlushUnsignedShortIntNode(Node &node, ostream &os);
	void FlushUnsignedLongIntNode(Node &node, ostream &os);
	void FlushUnsignedLongLongIntNode(Node &node, ostream &os);
	void FlushFloatNode(Node &node, ostream &os);
	void FlushDoubleNode(Node &node, ostream &os);
	void FlushEndlNode(Node &node, ostream &os);
	void FlushMessage(Node &node, xmlTextWriterPtr &out);
	void FlushDebugNode(Node &node, xmlTextWriterPtr &out);
	void FlushLocation(Node &node, xmlTextWriterPtr &out);
	void FlushDebugInfoNode(Node &node, xmlTextWriterPtr &out);
	void FlushDebugWarningNode(Node &node, xmlTextWriterPtr &out);
	void FlushDebugErrorNode(Node &node, xmlTextWriterPtr &out);
	void FlushBoolNode(Node &node, xmlTextWriterPtr &out);
	void FlushStringNode(Node &node, xmlTextWriterPtr &out);
	void FlushCharNode(Node &node, xmlTextWriterPtr &out);
	void FlushIntNode(Node &node, xmlTextWriterPtr &out);
	void FlushShortIntNode(Node &node, xmlTextWriterPtr &out);
	void FlushLongIntNode(Node &node, xmlTextWriterPtr &out);
	void FlushLongLongIntNode(Node &node, xmlTextWriterPtr &out);
	void FlushUnsignedCharNode(Node &node, xmlTextWriterPtr &out);
	void FlushUnsignedIntNode(Node &node, xmlTextWriterPtr &out);
	void FlushUnsignedShortIntNode(Node &node, xmlTextWriterPtr &out);
	void FlushUnsignedLongIntNode(Node &node, xmlTextWriterPtr &out);
	void FlushUnsignedLongLongIntNode(Node &node, xmlTextWriterPtr &out);
	void FlushFloatNode(Node &node, xmlTextWriterPtr &out);
	void FlushDoubleNode(Node &node, xmlTextWriterPtr &out);
	void FlushEndlNode(Node &node, xmlTextWriterPtr &out);
	void FlushMessageOverNetwork(Node &node);
	void FlushDebugNodeOverNetwork(Node &node, stringstream &buffer);
	void FlushLocationOverNetwork(Node &node, stringstream &buffer);
//	void FlushTimeOverNetwork(Node &node, stringstream &buffer);
	void FlushDebugInfoNodeOverNetwork(Node &node, stringstream &buffer);
	void FlushDebugWarningNodeOverNetwork(Node &node, stringstream &buffer);
	void FlushDebugErrorNodeOverNetwork(Node &node, stringstream &buffer);
	void FlushBoolNodeOverNetwork(Node &node, stringstream &buffer);
	void FlushStringNodeOverNetwork(Node &node, stringstream &buffer);
	void FlushCharNodeOverNetwork(Node &node, stringstream &buffer);
	void FlushIntNodeOverNetwork(Node &node, stringstream &buffer);
	void FlushShortIntNodeOverNetwork(Node &node, stringstream &buffer);
	void FlushLongIntNodeOverNetwork(Node &node, stringstream &buffer);
	void FlushLongLongIntNodeOverNetwork(Node &node, stringstream &buffer);
	void FlushUnsignedCharNodeOverNetwork(Node &node, stringstream &buffer);
	void FlushUnsignedIntNodeOverNetwork(Node &node, stringstream &buffer);
	void FlushUnsignedShortIntNodeOverNetwork(Node &node, stringstream &buffer);
	void FlushUnsignedLongIntNodeOverNetwork(Node &node, stringstream &buffer);
	void FlushUnsignedLongLongIntNodeOverNetwork(Node &node, stringstream &buffer);
	void FlushFloatNodeOverNetwork(Node &node, stringstream &buffer);
	void FlushDoubleNodeOverNetwork(Node &node, stringstream &buffer);
	void FlushEndlNodeOverNetwork(Node &node, stringstream &buffer);
	
	/* network capabilities variables and methods */
	/**
	 * This method is responsible of setting the network connection.
	 *
	 * @return true on success
	 */
	bool SetupNetwork();
	/**
	 * Indicates if the network connection has been stablished or not.
	 */
	bool network_connected;
	/**
	 * The network socket handler.
	 */
	int network_socket;
	/**
	 * Network constants
	 */
	static const uint32_t NETWORK_CONTENT_LENGTH = 10000;
	static const uint32_t NETWORK_MESSAGE_START = 0;
	static const uint32_t NETWORK_MESSAGE_END = 1;
	static const uint32_t NETWORK_MESSAGE_SOURCE = 2;
	static const uint32_t NETWORK_MESSAGE_TIME = 3;
	static const uint32_t NETWORK_DEBUG_INFO_START = 4;
	static const uint32_t NETWORK_DEBUG_INFO_END = 5;
	static const uint32_t NETWORK_DEBUG_WARNING_START = 6;
	static const uint32_t NETWORK_DEBUG_WARNING_END = 7;
	static const uint32_t NETWORK_DEBUG_ERROR_START = 8;
	static const uint32_t NETWORK_DEBUG_ERROR_END = 9;
	static const uint32_t NETWORK_LOCATION_FILE = 10;
	static const uint32_t NETWORK_LOCATION_FUNCTION = 11;
	static const uint32_t NETWORK_LOCATION_LINE = 12;
	static const uint32_t NETWORK_NUMMODE_DEC = 13;
	static const uint32_t NETWORK_NUMMODE_HEX = 14;
	static const uint32_t NETWORK_BOOL = 15;
	static const uint32_t NETWORK_STRING = 16;
	static const uint32_t NETWORK_CHAR = 17;
	static const uint32_t NETWORK_INT = 18;
	static const uint32_t NETWORK_SHORTINT = 19;
	static const uint32_t NETWORK_LONGINT = 20;
	static const uint32_t NETWORK_LONGLONGINT = 21;
	static const uint32_t NETWORK_UNSIGNEDCHAR = 22;
	static const uint32_t NETWORK_UNSIGNEDINT = 23;
	static const uint32_t NETWORK_UNSIGNEDSHORTINT = 24;
	static const uint32_t NETWORK_UNSIGNEDLONGINT = 25;
	static const uint32_t NETWORK_UNSIGNEDLONGLONGINT = 26;
	static const uint32_t NETWORK_FLOAT = 27;
	static const uint32_t NETWORK_DOUBLE = 28;
	static const uint32_t NETWORK_ENDL = 29;
};
} // end of logger namespace
} // end of service namespace
} // end of unisim namespace

#endif /* __UNISIM_SERVICE_LOGGER_LOGGERSERVER_HH__ */
