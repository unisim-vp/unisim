/*
 *  Copyright (c) 2018,
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
 * Authors: Gilles Mouchard (gilles.mouchard@cea.fr)
 */

#ifndef __UNISIM_SERVICE_HTTP_SERVER_HTTP_SERVER_HH__
#define __UNISIM_SERVICE_HTTP_SERVER_HTTP_SERVER_HH__

#include <unisim/kernel/service/service.hh>
#include <unisim/kernel/logger/logger.hh>
#include <unisim/util/hypapp/hypapp.hh>
#include <unisim/service/interfaces/http_server.hh>

namespace unisim {
namespace service {
namespace http_server {

class HttpServer;

class HttpServer
	: public unisim::kernel::service::Client<unisim::service::interfaces::HttpServer>
	, public unisim::util::hypapp::HttpServer
{
public:
	static const unsigned int MAX_HTTP_SERVERS = 256;
	
	unisim::kernel::service::ServiceImport<unisim::service::interfaces::HttpServer> *http_server_import[MAX_HTTP_SERVERS];
	
	HttpServer(const char *name, unisim::kernel::service::Object *parent = 0);
	virtual ~HttpServer();
	
	virtual bool EndSetup();
	
	virtual void Serve(unisim::util::hypapp::ClientConnection const& conn);
	
private:
	unisim::kernel::logger::Logger logger;
	
	std::string program_name;
	
	unisim::kernel::service::Parameter<bool> param_verbose;
	int http_port;
	unisim::kernel::service::Parameter<int> param_http_port;
	int http_max_clients;
	unisim::kernel::service::Parameter<int> param_http_max_clients;
	
	std::map<unisim::kernel::service::Object *, unisim::kernel::service::ServiceImport<unisim::service::interfaces::HttpServer> *> http_server_map;
	
	std::string Href(unisim::kernel::service::Object *object) const;
	unisim::kernel::service::Object *FindChildObject(unisim::kernel::service::Object *object, const std::string& child_hierarchical_name, std::size_t& pos);
	unisim::kernel::service::Object *FindObject(const std::string& hierarchical_name, std::size_t& pos);
	bool ServeFile(const std::string& path, unisim::util::hypapp::ClientConnection const& conn);
	bool ServeRootDocument(unisim::util::hypapp::HttpRequest const& req, unisim::util::hypapp::ClientConnection const& conn);
	bool ServeHeader(unisim::util::hypapp::HttpRequest const& req, unisim::util::hypapp::ClientConnection const& conn);
	void Crawl(std::ostream& os, const std::string& object_url, unisim::kernel::service::Object *object, unsigned int indent_level);
	void Crawl(std::ostream& os, unsigned int indent_level);
	//bool ServeBrowser(unisim::util::hypapp::HttpRequest const& req, unisim::util::hypapp::ClientConnection const& conn);
	bool ServeVariables(unisim::util::hypapp::HttpRequest const& req, unisim::util::hypapp::ClientConnection const& conn, unisim::kernel::service::VariableBase::Type var_type);
	bool ServeFooter(unisim::util::hypapp::HttpRequest const& req, unisim::util::hypapp::ClientConnection const& conn);
	bool Serve404(unisim::util::hypapp::HttpRequest const& req, unisim::util::hypapp::ClientConnection const& conn);
	bool RouteHttpRequest(unisim::kernel::service::Object *object, unisim::util::hypapp::HttpRequest const& req, unisim::util::hypapp::ClientConnection const& conn);
	bool ServeDefault(unisim::util::hypapp::HttpRequest const& req, unisim::util::hypapp::ClientConnection const& conn);
};

} // end of namespace http_server
} // end of namespace service
} // end of namespace unisim

#endif // __UNISIM_SERVICE_HTTP_SERVER_HTTP_SERVER_HH__