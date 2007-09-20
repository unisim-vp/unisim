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
    Parameter<bool> param_show_function;
    bool show_function;
    Parameter<bool> param_show_line;
    bool show_line;
    Parameter<bool> param_show_file;
    bool show_file;
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
};
} // end of logger namespace
} // end of service namespace
} // end of unisim namespace

#endif /* __UNISIM_SERVICE_LOGGER_LOGGERSERVER_HH__ */
