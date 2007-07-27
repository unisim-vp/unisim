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

namespace unisim {
namespace service {
namespace logger {

Logger(const char *name, Object *parent) :
    Object(name, parent),
    Service<unisim::service::interface::Logger>(name, parent),
    logger_export("logger_export", this) {
}

~Logger() {
}

/* Service methods */
void OnDisconnect() {
    if(server)
	server->Flush();
}

bool Setup() {
    if(logger_export.GetClient() != 0) {
    	client_name = logger_export.GetClient()->GetName();
    } else {
    	client_name = "UnknownObject";
    }

    return true;
}

/* Method to connect the logger to a the server */
void 
Logger::
SetServer(LoggerServerInterface *server) {
    this->server = server;
}

/* Methods provided by the ServiceExport<LoggerInterface> */	
void Message(Node *node) {
    Node *name_node;
    Property *source_property;

    if(server) {
	source_property = new Property(LoggerInterface::string_source, client_name);
	name_node = new Node(LoggerInterface::string_message);
	*name_node << *source_property;
	*name_node << *node;
	server->Message(name_node);
    }
}

void Flush() {
    if(server)
	server->Flush();
}

} // end of namespace logger
} // end of namespace service
} // end of namespace unisim