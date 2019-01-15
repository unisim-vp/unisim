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

#ifndef __UNISIM_KERNEL_SERVICE_HTTP_SERVER_HTTP_SERVER_HH__
#define __UNISIM_KERNEL_SERVICE_HTTP_SERVER_HTTP_SERVER_HH__

#include <unisim/kernel/service/service.hh>
#include <unisim/kernel/logger/logger.hh>
#include <unisim/util/hypapp/hypapp.hh>

namespace unisim {
namespace kernel {
namespace http_server {

class HttpServer;

class HttpRequest
{
public:
	bool IsValid() const { return valid; }
	bool HasQuery() const { return has_query; }
	bool HasFragment() const { return has_fragment; }
	unisim::util::hypapp::Request::request_type_t GetRequestType() const { return req.GetRequestType(); }
	char const* GetRequestURI() const { return req.GetRequestURI(); }
	char const* GetHost() const { return req.GetHost(); }
	char const* GetContentType() const { return req.GetContentType(); }
	unsigned int GetContentLength() const { return req.GetContentLength(); }
	char const* GetContent() const { return req.GetContent(); }
	const std::string& GetAbsolutePath() const { return abs_path; }
	const std::string& GetServerRoot() const { return server_root; }
	const std::string& GetPath() const { return path; }
	const std::string& GetQuery() const { return query; }
	const std::string& GetFragment() const { return fragment; }
	const std::string& GetDomain() const { return domain; }
	bool HasPort() const { return has_port; }
	unsigned int GetPort() const { return port; }
private:
	friend class HttpServer;
	friend std::ostream& operator << (std::ostream& os, const HttpRequest& http_request);
	
	HttpRequest(const unisim::util::hypapp::Request& req, std::ostream& err_log);
	HttpRequest(const std::string& server_root, const std::string& path, const HttpRequest& http_request);
	void ShiftAbsolutePath(std::size_t abs_path_pos);
	
	bool valid;
	bool has_query;
	bool has_fragment;
	const unisim::util::hypapp::Request& req;
	std::string abs_path;
	std::string server_root;
	std::string path;
	std::string query;
	std::string fragment;
	std::string domain;
	bool has_port;
	unsigned int port;
};

std::ostream& operator << (std::ostream& os, const HttpRequest& http_request);

class HttpServer
	: public unisim::kernel::service::Object
	, public unisim::util::hypapp::HttpServer
{
public:
	HttpServer(const char *name, Object *parent = 0);
	virtual ~HttpServer();
	
	void Setup();
	
	virtual void Serve(unisim::util::hypapp::ClientConnection const& conn);
	
private:
	unisim::kernel::logger::Logger logger;
	
	std::string program_name;
	
	unisim::kernel::service::Parameter<bool> param_verbose;
	int http_port;
	unisim::kernel::service::Parameter<int> param_http_port;
	int http_max_clients;
	unisim::kernel::service::Parameter<int> param_http_max_clients;
	
	std::string Href(unisim::kernel::service::Object *object) const;
	unisim::kernel::service::Object *FindChildObject(unisim::kernel::service::Object *object, const std::string& child_hierarchical_name, std::size_t& pos);
	unisim::kernel::service::Object *FindObject(const std::string& hierarchical_name, std::size_t& pos);
	bool ServeFile(const std::string& path, unisim::util::hypapp::ClientConnection const& conn);
	bool ServeRootDocument(HttpRequest const& req, unisim::util::hypapp::ClientConnection const& conn);
	bool ServeHeader(HttpRequest const& req, unisim::util::hypapp::ClientConnection const& conn);
	void Crawl(std::ostream& os, const std::string& object_url, unisim::kernel::service::Object *object, unsigned int indent_level);
	void Crawl(std::ostream& os, unsigned int indent_level);
	//bool ServeBrowser(HttpRequest const& req, unisim::util::hypapp::ClientConnection const& conn);
	bool ServeVariables(HttpRequest const& req, unisim::util::hypapp::ClientConnection const& conn, unisim::kernel::service::VariableBase::Type var_type);
	bool ServeFooter(HttpRequest const& req, unisim::util::hypapp::ClientConnection const& conn);
	bool Serve404(HttpRequest const& req, unisim::util::hypapp::ClientConnection const& conn);
};

} // end of namespace http_server
} // end of namespace kernel
} // end of namespace unisim

#endif // __UNISIM_KERNEL_SERVICE_HTTP_SERVER_HTTP_SERVER_HH__
