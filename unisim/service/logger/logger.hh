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

#include "unisim/service/interface/logger.hh"
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
using unisim::service::interface::Logger;
using unisim::service::logger::LoggerServerInterface;
using std::string;

class Logger :
    public Service<unisim::service::interface::Logger> {
public:
    /* Exported services */
    ServiceExport<unisim::service::interface::Logger> logger_export;
	
    /* Constructor/Destructor */
    Logger(const char *name, Object *parent = 0);
    virtual ~Logger();

    /* Service methods */
    virtual void OnDisconnect();
    virtual bool Setup();
    
    /* Method to connect the logger to a the server */
    void SetServer(LoggerServerInterface *server);

    /* Methods provided by the ServiceExport<unisim::service::interface::Logger> */	
    virtual void Message(Node *node);
    virtual void Flush();

private:
    LoggerServerInterface *server;
    string client_name;
};

} // end of namespace logger
} // end of namespace service
} // end of namespace unisim

#endif /* __UNISIM_SERVICE_LOGGER_HH__ */
