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

#include <unisim/service/http_server/http_server.hh>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

namespace unisim {
namespace service {
namespace http_server {

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;

HttpServer::HttpServer(const char *name, unisim::kernel::service::Object *parent)
	: unisim::kernel::service::Object(name, parent)
	, unisim::kernel::service::Client<unisim::service::interfaces::HttpServer>(name, parent)
	, unisim::util::hypapp::HttpServer()
	, http_server_import()
	, logger(*this)
	, program_name(GetSimulator()->FindVariable("program-name")->operator std::string())
	, param_verbose("verbose", this, verbose, "enable/disable verbosity")
	, http_port(0)
	, param_http_port("http-port", this, http_port, "HTTP port")
	, http_max_clients(10)
	, param_http_max_clients("http-max-clients", this, http_max_clients, "HTTP max clients")
	, http_server_map()
{
	param_http_port.SetFormat(unisim::kernel::service::VariableBase::FMT_DEC);
	param_http_port.SetMutable(false);
	param_http_max_clients.SetFormat(unisim::kernel::service::VariableBase::FMT_DEC);
	param_http_max_clients.SetMutable(false);
	
	logger << DebugInfo << "HTTP server on port #" << http_port << EndDebugInfo;
	this->SetTCPPort(http_port);
	this->SetMaxClients(http_max_clients);
	if(verbose)
	{
		this->SetLog(logger.DebugInfoStream());
	}
	else
	{
		this->SetLog(logger.DebugNullStream());
	}
	this->SetWarnLog(logger.DebugWarningStream());
	this->SetErrLog(logger.DebugErrorStream());
	
	unsigned int i;
	for(i = 0; i < MAX_HTTP_SERVERS; i++)
	{
		std::stringstream http_server_import_name_sstr;
		http_server_import_name_sstr << "http-server-import[" << i << "]";
		http_server_import[i] = new unisim::kernel::service::ServiceImport<unisim::service::interfaces::HttpServer>(http_server_import_name_sstr.str().c_str(), this);
	}
}

HttpServer::~HttpServer()
{
	Kill();
	JoinLoopThread();
	
	unsigned int i;
	for(i = 0; i < MAX_HTTP_SERVERS; i++)
	{
		delete http_server_import[i];
	}
}

bool HttpServer::EndSetup()
{
	unsigned int i;
	for(i = 0; i < MAX_HTTP_SERVERS; i++)
	{
		if(*http_server_import[i])
		{
			unisim::kernel::service::Object *object = http_server_import[i]->GetService();
		
			if(object)
			{
				http_server_map[object] = http_server_import[i];
			}
		}
	}
	
	StartLoopThread();
	logger << DebugInfo << "HTTP server started on TCP/IP port #" << http_port << EndDebugInfo;
	return true;
}

std::string HttpServer::Href(unisim::kernel::service::Object *object) const
{
	std::string href;
	
	if(object)
	{
		unisim::kernel::service::Object *parent = object->GetParent();
		
		if(parent)
		{
			href = Href(parent);
		}
		
		href += '/';
		href += object->GetObjectName();
	}
	else
	{
		href = "/kernel";
	}
	
	return href;
}

unisim::kernel::service::Object *HttpServer::FindChildObject(unisim::kernel::service::Object *object, const std::string& hierarchical_name, std::size_t& pos)
{
	std::size_t curr_pos = pos;
	if(curr_pos >= hierarchical_name.length()) return 0;

	if(hierarchical_name[curr_pos] == '/')
	{
		pos = curr_pos + 1;
		return (pos < hierarchical_name.length()) ? FindChildObject(object, hierarchical_name, pos) : 0; // eat "/"
	}
	
	std::size_t hierarchical_delimiter_pos = hierarchical_name.find_first_of('/', curr_pos);
	
	std::size_t len = ((hierarchical_delimiter_pos == std::string::npos) ? hierarchical_name.length() : hierarchical_delimiter_pos) - curr_pos;
	
	if((len == 1) && (hierarchical_name[curr_pos] == '.'))
	{
		pos = hierarchical_delimiter_pos;
		return (pos < hierarchical_name.length()) ? FindChildObject(object, hierarchical_name, pos) : 0; // eat "/."
	}
	
	if((len == 2) && (hierarchical_name[pos] == '.') && (hierarchical_name[pos + 1] == '.'))
	{
		pos = hierarchical_delimiter_pos;
		unisim::kernel::service::Object *parent = object->GetParent();
		return (pos < hierarchical_name.length()) ? (parent ? FindChildObject(parent, hierarchical_name, pos) : 0) : parent; // eat "../"
	}

	const std::list<unisim::kernel::service::Object *>& leaf_objects = object->GetLeafs();
	
	std::list<unisim::kernel::service::Object *>::const_iterator it;
	
	unisim::kernel::service::Object *found_child = 0;
	
	for(it = leaf_objects.begin(); it != leaf_objects.end(); it++)
	{
		unisim::kernel::service::Object *child = *it;
		
		if(hierarchical_name.compare(curr_pos, len, child->GetObjectName()) == 0)
		{
			// found child
			found_child = child;
			break;
		}
	}
	
	if(!found_child)
	{
		// not found
		return 0;
	}
	
	if(hierarchical_delimiter_pos == std::string::npos)
	{
		return found_child;
	}
	
	pos = hierarchical_delimiter_pos/* + 1*/;
	
	unisim::kernel::service::Object *found_object = (pos < hierarchical_name.length()) ? FindChildObject(found_child, hierarchical_name, pos) : 0;
	
	return found_object ? found_object : found_child;
}

unisim::kernel::service::Object *HttpServer::FindObject(const std::string& hierarchical_name, std::size_t& pos)
{
	std::size_t curr_pos = pos;
	if(curr_pos >= hierarchical_name.length()) return 0;

	if(hierarchical_name[curr_pos] != '/') return 0;
	
	curr_pos++;
	
	std::size_t hierarchical_delimiter_pos = hierarchical_name.find_first_of('/', curr_pos);
	
	std::size_t len = ((hierarchical_delimiter_pos == std::string::npos) ? hierarchical_name.length() : hierarchical_delimiter_pos) - curr_pos;
	
	if((len == 1) && (hierarchical_name[curr_pos] == '.'))
	{
		pos = hierarchical_delimiter_pos;
		return (pos < hierarchical_name.length()) ? FindObject(hierarchical_name, pos) : 0; // eat "/."
	}
	
	if((len == 2) && (hierarchical_name[curr_pos] == '.') && (hierarchical_name[curr_pos + 1] == '.'))
	{
		return 0; // "../" is forbidden for root
	}

	std::list<unisim::kernel::service::Object *> root_objects;
	
	GetSimulator()->GetRootObjects(root_objects);
	
	std::list<unisim::kernel::service::Object *>::const_iterator it;
	
	unisim::kernel::service::Object *found_root_object = 0;
	
	for(it = root_objects.begin(); it != root_objects.end(); it++)
	{
		unisim::kernel::service::Object *root_object = *it;
		
		if(hierarchical_name.compare(curr_pos, len, root_object->GetObjectName()) == 0)
		{
			// found root object
			found_root_object = root_object;
			break;
		}
	}

	if(!found_root_object)
	{
		// not found
		return 0;
	}
	
	if(hierarchical_delimiter_pos == std::string::npos)
	{
		return found_root_object;
	}
	
	pos = hierarchical_delimiter_pos;
	
	unisim::kernel::service::Object *found_object = (pos < hierarchical_name.length()) ? FindChildObject(found_root_object, hierarchical_name, pos) : 0;
	
	return found_object ? found_object : found_root_object;
}

static std::string String_to_HTML(const std::string& s)
{
	std::stringstream sstr;
	std::size_t pos = 0;
	std::size_t len = s.length();
	
	for(pos = 0; pos < len; pos++)
	{
		char c = s[pos];

		switch(c)
		{
			case '\n':
				sstr << "<br>";
				break;
			case '<':
				sstr << "&lt;";
				break;
			case '>':
				sstr << "&gt;";
				break;
			case '&':
				sstr << "&amp;";
				break;
			case '"':
				sstr << "&quot;";
				break;
			case '\'':
				sstr << "&apos;";
				break;
			case ' ':
				sstr << "&nbsp;";
				break;
			case '\t':
				sstr << "&nbsp;&nbsp;&nbsp;&nbsp;";
			default:
				sstr << c;
		}
	}
	
	return sstr.str();
}

bool HttpServer::ServeFile(const std::string& path, unisim::util::hypapp::ClientConnection const& conn)
{
	if(Verbose())
	{
		logger << DebugInfo << "opening File \"" << path << "\"" << EndDebugInfo;
	}
	
	std::ifstream file(path.c_str(), std::fstream::binary);

	if(file.is_open())
	{
		if(file.seekg(0, file.end))
		{
			std::streampos length = file.tellg();
			
			if(file.seekg(0, file.beg))
			{
				std::ostringstream http_header_sstr;
				std::string ext = path.substr(path.find_last_of("/."));
				
				http_header_sstr << "HTTP/1.1 200 OK\r\n";
				http_header_sstr << "Connection: keep-alive\r\n";
				http_header_sstr << "Content-Type: ";
				if((ext == ".htm") || (ext == ".html"))
					http_header_sstr << "text/html; charset=utf-8";
				else if(ext == ".css")
					http_header_sstr << "text/css";
				else if(ext == ".js")
					http_header_sstr << "application/javascript";
				else if(ext == ".png")
					http_header_sstr << "image/png";
				else if((ext == ".jpg") || (ext == ".jpeg"))
					http_header_sstr << "image/jpeg";
				else if(ext == ".svg")
					http_header_sstr << "image/svg+xml";
				else if(ext == ".ico")
					http_header_sstr << "image/x-icon";
				else
					http_header_sstr << "application/octet-stream";
				
				http_header_sstr << "\r\nContent-Length: " << length << "\r\n\r\n";
				
				std::string http_header(http_header_sstr.str());

				if(Verbose())
				{
					logger << DebugInfo << "sending HTTP response header: " << std::endl << http_header << EndDebugInfo;
				}
				
				if(!conn.Send(http_header.c_str(), http_header.length()))
				{
					logger << DebugWarning << "I/O error or connection closed by peer while sending HTTP response header" << EndDebugWarning;
					return false;
				}
				
				if(Verbose())
				{
					logger << DebugInfo << "sending HTTP response header: done" << EndDebugInfo;
				}

				if(Verbose())
				{
					logger << DebugInfo << "sending HTTP response body" << EndDebugInfo;
				}

				if(length > 0)
				{
					char buffer[4096];
					std::streampos count = length;
					do
					{
						std::streamsize n = file.readsome(buffer, sizeof(buffer));
						
						if(file.fail()) return false;
						
						if(n > 0)
						{
							if(!conn.Send(buffer, n))
							{
								logger << DebugWarning << "I/O error or connection closed by peer while sending HTTP response body" << EndDebugWarning;
								return false;
							}
							count -= n;
						}
					}
					while(count > 0);
				}
				
				if(Verbose())
				{
					logger << DebugInfo << "sending HTTP response body: done" << EndDebugInfo;
				}

				return true;
			}
			else
			{
				logger << DebugWarning << "Can't seek at beginning of File \"" << path << "\"" << EndDebugWarning;
			}
		}
		else
		{
			logger << DebugWarning << "Can't seek at end of File \"" << path << "\"" << EndDebugWarning;
		}
	}
	else
	{
		logger << DebugWarning << "Can' open File \"" << path << "\" for reading" << EndDebugWarning;
	}
	
	return false;
}

bool HttpServer::ServeHeader(unisim::util::hypapp::HttpRequest const& req, unisim::util::hypapp::ClientConnection const& conn)
{
	return conn.Send("HTTP/1.1 200 OK\r\nConnection: keep-alive\r\nContent-Type: text/html; charset=utf-8\r\nContent-Length: 6\r\n\r\nHead\r\n");
}

void HttpServer::Crawl(std::ostream& os, const std::string& object_url, unisim::kernel::service::Object *object, unsigned int indent_level)
{
	for(unsigned int i = 0; i < indent_level; i++) os << '\t';
	os << "<span class=\"browser-item noselect\" ondblclick=\"gui.open_object('" << object_url << "','" <<  String_to_HTML(object->GetName()) << "')\"" << ">" << String_to_HTML(object->GetObjectName()) << "</span>" << std::endl;;
	
	const std::list<unisim::kernel::service::Object *>& leaf_objects = object->GetLeafs();
	
	if(!leaf_objects.empty())
	{
		for(unsigned int i = 0; i < indent_level; i++) os << '\t';
		os << "<ul class=\"tree\">" << std::endl;
		indent_level++;
		
		std::list<unisim::kernel::service::Object *>::const_iterator it;
		std::list<unisim::kernel::service::Object *>::const_iterator next_it;
		
		for(it = leaf_objects.begin(); it != leaf_objects.end(); it = next_it)
		{
			unisim::kernel::service::Object *child = *it;
			next_it = it;
			next_it++;
			
			for(unsigned int i = 0; i < indent_level; i++) os << '\t';
			os << "<li";
			if(next_it == leaf_objects.end())
			{
				os << " class=\"last\"";
			}
			os << ">" << std::endl;
			std::string child_url(object_url);
			child_url += '/';
			child_url += child->GetObjectName();
			Crawl(os, child_url, child, indent_level + 1);
			for(unsigned int i = 0; i < indent_level; i++) os << '\t';
			os << "</li>" << std::endl;
		}
		
		indent_level--;
		for(unsigned int i = 0; i < indent_level; i++) os << '\t';
		os << "</ul>" << std::endl;
	}
}

void HttpServer::Crawl(std::ostream& os, unsigned int indent_level)
{
	for(unsigned int i = 0; i < indent_level; i++) os << '\t';
	os << "<div class=\"browser-item noselect\" ondblclick=\"gui.open_object('/kernel','kernel','" << program_name << "')\"" << ">" << String_to_HTML(program_name) << "</div>" << std::endl;;

	std::list<unisim::kernel::service::Object *> root_objects;
	
	GetSimulator()->GetRootObjects(root_objects);
	
	if(!root_objects.empty())
	{
		for(unsigned int i = 0; i < indent_level; i++) os << '\t';
		os << "<ul class=\"tree\">" << std::endl;
		indent_level++;
		
		std::list<unisim::kernel::service::Object *>::const_iterator it;
		
		for(it = root_objects.begin(); it != root_objects.end(); it++)
		{
			unisim::kernel::service::Object *root_object = *it;
			
			for(unsigned int i = 0; i < indent_level; i++) os << '\t';
			os << "<li>" << std::endl;
			std::string root_object_url("/");
			root_object_url += root_object->GetObjectName();
			Crawl(os, root_object_url, root_object, indent_level + 1);
			for(unsigned int i = 0; i < indent_level; i++) os << '\t';
			os << "</li>" << std::endl;
		}
		indent_level--;
		for(unsigned int i = 0; i < indent_level; i++) os << '\t';
		os << "</ul>" << std::endl;
	}
}

#if 0
bool HttpServer::ServeBrowser(HttpRequest const& req, unisim::util::hypapp::ClientConnection const& conn)
{
	struct QueryDecoder : public unisim::util::hypapp::Form_URL_Encoded_Decoder
	{
		QueryDecoder(HttpServer& _http_server)
			: http_server(_http_server)
			, object_name()
		{
		}
		
		virtual bool FormAssign(const std::string& name, const std::string& value)
		{
			if(name == "object")
			{
				object_name = value;
				return true;
			}
			
			return false;
		}
		
		HttpServer& http_server;
		std::string object_name;
	};

	std::ostringstream doc_sstr;
	doc_sstr << "<!DOCTYPE html>" << std::endl;
	doc_sstr << "<html>" << std::endl;
	doc_sstr << "\t<head>" << std::endl;
	doc_sstr << "\t\t<title>Object browser</title>" << std::endl;
	doc_sstr << "\t\t<meta name=\"description\" content=\"object browser over HTTP\">" << std::endl;
	doc_sstr << "\t\t<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">" << std::endl;
	doc_sstr << "\t\t<link rel=\"shortcut icon\" type=\"image/x-icon\" href=\"/favicon.ico\" />" << std::endl;
	doc_sstr << "\t\t<link rel=\"stylesheet\" href=\"/unisim/kernel/http_server/browser_style.css\" type=\"text/css\" />" << std::endl;
	//doc_sstr << "\t\t<script type=\"application/javascript\" src=\"/unisim/kernel/http_server/load_object.js\"></script>" << std::endl;
	doc_sstr << "\t\t<script type=\"application/javascript\" src=\"/unisim/kernel/http_server/browser_script.js\"></script>" << std::endl;
	doc_sstr << "\t</head>" << std::endl;
	doc_sstr << "\t<body>" << std::endl;

	if(req.HasQuery())
	{
		QueryDecoder query_decoder(*this);
	
		if(query_decoder.Decode(req.GetQuery(), logger.DebugWarningStream()))
		{
			Crawl(doc_sstr, query_decoder.object_name, 2);
		}
	}
	else
	{
		if(verbose)
		{
			logger << DebugInfo << "missing query" << EndDebugInfo;
		}
	}
	
	doc_sstr << "\t</body>" << std::endl;
	doc_sstr << "</html>" << std::endl;
	
	std::string doc(doc_sstr.str());

	std::ostringstream http_header_sstr;
	http_header_sstr << "HTTP/1.1 200 OK\r\n";
	http_header_sstr << "Server: UNISIM-VP\r\n";
	http_header_sstr << "Cache-control: no-cache\r\n";
	http_header_sstr << "Connection: keep-alive\r\n";
	http_header_sstr << "Content-length: " << doc.length() << "\r\n";
	http_header_sstr << "Content-Type: text/html\r\n";
	http_header_sstr << "\r\n";
	
	std::string http_header(http_header_sstr.str());

	if(verbose)
	{
		logger << DebugInfo << "sending HTTP response header: " << std::endl << http_header << EndDebugInfo;
	}
	if(!conn.Send(http_header.c_str(), http_header.length()))
	{
		logger << DebugWarning << "I/O error or connection closed by peer while sending HTTP header" << EndDebugWarning;
		return false;
	}
	
	if(verbose)
	{
		logger << DebugInfo << "sending HTTP response header: done" << EndDebugInfo;
	}
	
	if(req.GetRequestType() == unisim::util::hypapp::Request::HEAD) return true;
	
	if(verbose)
	{
		logger << DebugInfo << "sending HTTP response body: " << std::endl << doc << EndDebugInfo;
	}
	
	if(!conn.Send(doc.c_str(), doc.length()))
	{
		logger << DebugWarning << "I/O error or connection closed by peer while sending HTTP response body" << EndDebugWarning;
		return false;
	}
	
	if(verbose)
	{
		logger << DebugInfo << "sending HTTP response body: done" << EndDebugInfo;
	}
	
	return true;
}
#endif

bool HttpServer::ServeVariables(unisim::util::hypapp::HttpRequest const& req, unisim::util::hypapp::ClientConnection const& conn, unisim::kernel::service::VariableBase::Type var_type)
{
	struct QueryDecoder : public unisim::util::hypapp::Form_URL_Encoded_Decoder
	{
		QueryDecoder(HttpServer& _http_server)
			: http_server(_http_server)
			, object_name()
		{
		}
		
		virtual bool FormAssign(const std::string& name, const std::string& value)
		{
			if(name == "object")
			{
				object_name = value;
				return true;
			}
			
			return false;
		}
		
		HttpServer& http_server;
		std::string object_name;
	};
	
	std::ostringstream doc_sstr;
	
	doc_sstr << "<!DOCTYPE html>" << std::endl;
	doc_sstr << "<html>" << std::endl;
	doc_sstr << "\t<head>" << std::endl;
	doc_sstr << "\t\t<title>Object configuration</title>" << std::endl;
	doc_sstr << "\t\t<meta name=\"description\" content=\"object configurator over HTTP\">" << std::endl;
	doc_sstr << "\t\t<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">" << std::endl;
	doc_sstr << "\t\t<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">" << std::endl;
	doc_sstr << "\t\t<link rel=\"shortcut icon\" type=\"image/x-icon\" href=\"/favicon.ico\" />" << std::endl;
	doc_sstr << "\t\t<link rel=\"stylesheet\" href=\"/unisim/kernel/http_server/var_style.css\" type=\"text/css\" />" << std::endl;
	doc_sstr << "\t\t<script type=\"application/javascript\">document.domain='" << req.GetDomain() << "';</script>" << std::endl;
	doc_sstr << "\t\t<script type=\"application/javascript\" src=\"/unisim/kernel/http_server/var_script.js\"></script>" << std::endl;
	doc_sstr << "\t</head>" << std::endl;
	doc_sstr << "\t<body>" << std::endl;
	
	if(req.HasQuery())
	{
		QueryDecoder query_decoder(*this);
	
		if(query_decoder.Decode(req.GetQuery(), logger.DebugWarningStream()))
		{
			bool is_kernel = (query_decoder.object_name == "kernel");
			unisim::kernel::service::Object *object = is_kernel ? 0 : GetSimulator()->FindObject(query_decoder.object_name.c_str());
			if(object || is_kernel)
			{
				std::list<unisim::kernel::service::VariableBase *> var_lst;
				if(object)
				{
					object->GetVariables(var_lst, var_type);
				}
				else
				{
					GetSimulator()->GetRootVariables(var_lst, var_type);
				}
				
				if(req.GetRequestType() == unisim::util::hypapp::Request::POST)
				{
					struct Form_URL_Encoded_Decoder : public unisim::util::hypapp::Form_URL_Encoded_Decoder
					{
						virtual bool FormAssign(const std::string& _name, const std::string& _value)
						{
							name = _name;
							value = _value;
							return true;
						}
						
						std::string name;
						std::string value;
					};

					Form_URL_Encoded_Decoder decoder;
					if(decoder.Decode(std::string(req.GetContent(), req.GetContentLength()), logger.DebugWarningStream()))
					{
						if(object)
						{
							(*object)[decoder.name] = decoder.value.c_str();
						}
						else
						{
							*GetSimulator()->FindVariable(decoder.name.c_str()) = decoder.value.c_str();
						}
					}
				}
				
				if(!var_lst.empty())
				{
					doc_sstr << "\t\t\t<table class=\"var-table\">" << std::endl;
					doc_sstr << "\t\t\t\t<thead>" << std::endl;
					doc_sstr << "\t\t\t\t\t<tr>" << std::endl;
					doc_sstr << "\t\t\t\t\t\t<th class=\"var-name\">Name</th>" << std::endl;
					doc_sstr << "\t\t\t\t\t\t<th class=\"var-value\">Value</th>" << std::endl;
					doc_sstr << "\t\t\t\t\t\t<th class=\"var-data-type\">Data Type</th>" << std::endl;
					doc_sstr << "\t\t\t\t\t\t<th class=\"var-description\">Description</th>" << std::endl;
					doc_sstr << "\t\t\t\t\t</tr>" << std::endl;
					doc_sstr << "\t\t\t\t</thead>" << std::endl;
					doc_sstr << "\t\t\t\t<tbody>" << std::endl;
					std::list<unisim::kernel::service::VariableBase *>::const_iterator var_iter;
					unsigned int var_id;
					for(var_id = 0, var_iter = var_lst.begin(); var_iter != var_lst.end(); var_id++, var_iter++)
					{
						unisim::kernel::service::VariableBase *var = *var_iter;
						
						doc_sstr << "\t\t\t\t\t<tr>" << std::endl;
						doc_sstr << "\t\t\t\t\t\t<td class=\"var-name\">" << String_to_HTML(var->GetVarName()) << "</td>" << std::endl;
						doc_sstr << "\t\t\t\t\t\t<td class=\"var-value\">" << std::endl;
						doc_sstr << "\t\t\t\t\t\t\t<form action=\"/config?object=";
						if(object) doc_sstr << String_to_HTML(object->GetName());
						doc_sstr << "\" method=\"post\" enctype=\"application/x-www-form-urlencoded\">" << std::endl;
						doc_sstr << "\t\t\t\t\t\t\t\t<input";
						if(var->HasEnumeratedValues())
						{
							doc_sstr << " list=\"input-list-" << var_id << "\"";
						}
						doc_sstr << " class=\"var-value-text" << (var->IsMutable() ? "" : " disabled") << "\" type=\"text\" name=\"" << String_to_HTML(var->GetVarName()) << "\" value=\"" << String_to_HTML(std::string(*var)) << "\"" /*<< (var->IsMutable() ? "" : " disabled")*/ << (var->IsMutable() ? "" : " readonly") << ">" << std::endl;
						if(var->HasEnumeratedValues())
						{
							doc_sstr << "\t\t\t\t\t\t\t\t<datalist id=\"input-list-" << var_id << "\">" << std::endl;
							std::vector<std::string> values;
							var->GetEnumeratedValues(values);
							
							std::vector<std::string>::const_iterator it;
							for(it = values.begin(); it != values.end(); it++)
							{
								const std::string& value = (*it);
								doc_sstr << "\t\t\t\t\t\t\t\t\t\t<option value=\"" << String_to_HTML(value) << "\">" << String_to_HTML(value) << "</option>" << std::endl;
							}
							doc_sstr << "\t\t\t\t\t\t\t\t</datalist>" << std::endl;
						}
						
						doc_sstr << "\t\t\t\t\t\t\t</form>" << std::endl;
						doc_sstr << "\t\t\t\t\t\t</td>" << std::endl;
//						doc_sstr << "\t\t\t\t\t\t<td>" << String_to_HTML(std::string(*var)) << "</td>" << std::endl;
						doc_sstr << "\t\t\t\t\t\t<td class=\"var-data-type\">" << String_to_HTML(var->GetDataTypeName()) << "</td>" << std::endl;
						doc_sstr << "\t\t\t\t\t\t<td class=\"var-description\">" << String_to_HTML(var->GetDescription()) << "</td>" << std::endl;
						doc_sstr << "\t\t\t\t\t</tr>" << std::endl;
					}
					
					doc_sstr << "\t\t\t\t</tbody>" << std::endl;
					doc_sstr << "\t\t\t</table>" << std::endl;
				}
			}
		}
	}
	else
	{
		if(verbose)
		{
			logger << DebugInfo << "missing query" << EndDebugInfo;
		}
	}

	doc_sstr << "\t</body>" << std::endl;
	doc_sstr << "</html>" << std::endl;
	
	
	std::string doc(doc_sstr.str());

	std::ostringstream http_header_sstr;
	http_header_sstr << "HTTP/1.1 200 OK\r\n";
	http_header_sstr << "Server: UNISIM-VP\r\n";
	http_header_sstr << "Cache-control: no-cache\r\n";
	http_header_sstr << "Connection: keep-alive\r\n";
	http_header_sstr << "Content-length: " << doc.length() << "\r\n";
	http_header_sstr << "Content-Type: text/html; charset=utf-8\r\n";
	http_header_sstr << "\r\n";
	
	std::string http_header(http_header_sstr.str());

	if(verbose)
	{
		logger << DebugInfo << "sending HTTP response header: " << std::endl << http_header << EndDebugInfo;
	}
	if(!conn.Send(http_header.c_str(), http_header.length()))
	{
		logger << DebugWarning << "I/O error or connection closed by peer while sending HTTP header" << EndDebugWarning;
		return false;
	}
	
	if(verbose)
	{
		logger << DebugInfo << "sending HTTP response header: done" << EndDebugInfo;
	}
	
	if(req.GetRequestType() == unisim::util::hypapp::Request::HEAD) return true;
	
	if(verbose)
	{
		logger << DebugInfo << "sending HTTP response body: " << std::endl << doc << EndDebugInfo;
	}
	
	if(!conn.Send(doc.c_str(), doc.length()))
	{
		logger << DebugWarning << "I/O error or connection closed by peer while sending HTTP response body" << EndDebugWarning;
		return false;
	}
	
	if(verbose)
	{
		logger << DebugInfo << "sending HTTP response body: done" << EndDebugInfo;
	}
	
	return true;
}

bool HttpServer::ServeFooter(unisim::util::hypapp::HttpRequest const& req, unisim::util::hypapp::ClientConnection const& conn)
{
	return conn.Send("HTTP/1.1 200 OK\r\nConnection: keep-alive\r\nContent-Type: text/html; charset=utf-8\r\nContent-Length: 8\r\n\r\nFooter\r\n");
}

bool HttpServer::ServeRootDocument(unisim::util::hypapp::HttpRequest const& req, unisim::util::hypapp::ClientConnection const& conn)
{
	std::ostringstream doc_sstr;
	
	doc_sstr << "<!DOCTYPE html>" << std::endl;
	doc_sstr << "<html>" << std::endl;
	doc_sstr << "\t<head>" << std::endl;
	doc_sstr << "\t\t<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">" << std::endl;
	doc_sstr << "\t\t<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">" << std::endl;
	doc_sstr << "\t\t<meta name=\"description\" content=\"remote control interface over HTTP\">" << std::endl;
	doc_sstr << "\t\t<title>" << String_to_HTML(program_name) << "</title>" << std::endl;
	doc_sstr << "\t\t<link rel=\"shortcut icon\" type=\"image/x-icon\" href=\"/favicon.ico\" />" << std::endl;
	doc_sstr << "\t\t<link rel=\"stylesheet\" href=\"/unisim/kernel/http_server/style.css\" type=\"text/css\" />" << std::endl;
	doc_sstr << "\t\t<script type=\"application/javascript\">document.domain='" << req.GetDomain() << "';</script>" << std::endl;
	doc_sstr << "\t\t<script type=\"application/javascript\" src=\"/unisim/kernel/http_server/script.js\"></script>" << std::endl;
	doc_sstr << "\t</head>" << std::endl;
	doc_sstr << "\t<body>" << std::endl;
	
	doc_sstr << "\t<div id=\"content-div\">" << std::endl;
	doc_sstr << "\t\t<div class=\"tile\" id=\"left-tile-div\">" << std::endl;
	doc_sstr << "\t\t\t<div class=\"tab-headers-history-shortcut\" id=\"left-tab-headers-history-shortcut\"></div>" << std::endl;
	doc_sstr << "\t\t\t<div class=\"tab-headers-scroller tab-headers-left-scroller\" id=\"left-tab-headers-left-scroller\"></div>" << std::endl;
	doc_sstr << "\t\t\t<div class=\"tab-headers-scroller tab-headers-right-scroller\" id=\"left-tab-headers-right-scroller\"></div>" << std::endl;
	doc_sstr << "\t\t\t<div class=\"tab-headers\" id=\"left-tab-headers-div\">" << std::endl;
	doc_sstr << "\t\t\t\t<div class=\"tab-header noselect left-tab-header-div\" id=\"browser-tab-header\">" << std::endl;
	doc_sstr << "\t\t\t\t\tBrowser" << std::endl;
	doc_sstr << "\t\t\t\t</div>" << std::endl;
	doc_sstr << "\t\t\t</div>" << std::endl;
	doc_sstr << "\t\t\t<div class=\"tab-contents\" id=\"left-tab-contents-div\">" << std::endl;
	doc_sstr << "\t\t\t\t<div name=\"browser\" class=\"tab-content left-tab-content-div\" id=\"browser\">" << std::endl;
	Crawl(doc_sstr, 5);
	doc_sstr << "\t\t\t\t</div>" << std::endl;
	doc_sstr << "\t\t\t</div>" << std::endl;
	doc_sstr << "\t\t</div>" << std::endl;
	doc_sstr << "\t\t<div class=\"resizer\" id=\"left-horiz-resizer-div\"></div>" << std::endl;
	doc_sstr << "\t\t<div id=\"right-div\">" << std::endl;
	doc_sstr << "\t\t\t<div id=\"top-div\">" << std::endl;
	doc_sstr << "\t\t\t\t<div class=\"tile\" id=\"top-middle-tile-div\">" << std::endl;
	doc_sstr << "\t\t\t\t\t<div class=\"tab-headers-history-shortcut\" id=\"top-middle-tab-headers-history-shortcut\"></div>" << std::endl;
	doc_sstr << "\t\t\t\t\t<div class=\"tab-headers-scroller tab-headers-left-scroller\" id=\"top-middle-tab-headers-left-scroller\"></div>" << std::endl;
	doc_sstr << "\t\t\t\t\t<div class=\"tab-headers-scroller tab-headers-right-scroller\" id=\"top-middle-tab-headers-right-scroller\"></div>" << std::endl;
	doc_sstr << "\t\t\t\t\t<div class=\"tab-headers\" id=\"top-middle-tab-headers-div\"></div>" << std::endl;
	doc_sstr << "\t\t\t\t\t<div class=\"tab-contents\" id=\"top-middle-tab-contents-div\"></div>" << std::endl;
	doc_sstr << "\t\t\t\t</div>" << std::endl;
	doc_sstr << "\t\t\t\t<div class=\"resizer\" id=\"right-horiz-resizer-div\"></div>" << std::endl;
	doc_sstr << "\t\t\t\t<div class=\"tile\" id=\"top-right-tile-div\">" << std::endl;
	doc_sstr << "\t\t\t\t\t<div class=\"tab-headers-history-shortcut\" id=\"top-right-tab-headers-history-shortcut\"></div>" << std::endl;
	doc_sstr << "\t\t\t\t\t<div class=\"tab-headers-scroller tab-headers-left-scroller\" id=\"top-right-tab-headers-left-scroller\"></div>" << std::endl;
	doc_sstr << "\t\t\t\t\t<div class=\"tab-headers-scroller tab-headers-right-scroller\" id=\"top-right-tab-headers-right-scroller\"></div>" << std::endl;
	doc_sstr << "\t\t\t\t\t<div class=\"tab-headers\" id=\"top-right-tab-headers-div\"></div>" << std::endl;
	doc_sstr << "\t\t\t\t\t<div class=\"tab-contents\" id=\"top-right-tab-contents-div\"></div>" << std::endl;
	doc_sstr << "\t\t\t\t</div>" << std::endl;
	doc_sstr << "\t\t\t</div>" << std::endl;
	doc_sstr << "\t\t\t<div class=\"resizer\" id=\"vert-resizer-div\"></div>" << std::endl;
	doc_sstr << "\t\t\t<div class=\"tile\" id=\"bottom-tile-div\">" << std::endl;
	doc_sstr << "\t\t\t\t<div class=\"tab-headers-history-shortcut\" id=\"bottom-tab-headers-history-shortcut\"></div>" << std::endl;
	doc_sstr << "\t\t\t\t<div class=\"tab-headers-scroller tab-headers-left-scroller\" id=\"bottom-tab-headers-left-scroller\"></div>" << std::endl;
	doc_sstr << "\t\t\t\t<div class=\"tab-headers-scroller tab-headers-right-scroller\" id=\"bottom-tab-headers-right-scroller\"></div>" << std::endl;
	doc_sstr << "\t\t\t\t<div class=\"tab-headers\" id=\"bottom-tab-headers-div\"></div>" << std::endl;
	doc_sstr << "\t\t\t\t<div class=\"tab-contents\" id=\"bottom-tab-contents-div\"></div>" << std::endl;
	doc_sstr << "\t\t\t</div>" << std::endl;
	doc_sstr << "\t\t</div>" << std::endl;
	doc_sstr << "\t</div>" << std::endl;
	doc_sstr << "\t</body>" << std::endl;
	doc_sstr << "</html>" << std::endl;
	
	std::string doc(doc_sstr.str());

	std::ostringstream http_header_sstr;
	http_header_sstr << "HTTP/1.1 200 OK\r\n";
	http_header_sstr << "Server: UNISIM-VP\r\n";
	http_header_sstr << "Cache-control: no-cache\r\n";
	http_header_sstr << "Connection: keep-alive\r\n";
	http_header_sstr << "Content-length: " << doc.length() << "\r\n";
	http_header_sstr << "Content-Type: text/html; charset=utf-8\r\n";
	http_header_sstr << "\r\n";
	
	std::string http_header(http_header_sstr.str());

	if(verbose)
	{
		logger << DebugInfo << "sending HTTP response header: " << std::endl << http_header << EndDebugInfo;
	}
	if(!conn.Send(http_header.c_str(), http_header.length()))
	{
		logger << DebugWarning << "I/O error or connection closed by peer while sending HTTP header" << EndDebugWarning;
		return false;
	}
	
	if(verbose)
	{
		logger << DebugInfo << "sending HTTP response header: done" << EndDebugInfo;
	}
	
	if(req.GetRequestType() == unisim::util::hypapp::Request::HEAD) return true;
			
	if(verbose)
	{
		logger << DebugInfo << "sending HTTP response body: " << std::endl << doc << EndDebugInfo;
	}
	
	if(!conn.Send(doc.c_str(), doc.length()))
	{
		logger << DebugWarning << "I/O error or connection closed by peer while sending HTTP response body" << EndDebugWarning;
		return false;
	}
	
	if(verbose)
	{
		logger << DebugInfo << "sending HTTP response body: done" << EndDebugInfo;
	}
	
	return true;
}

bool HttpServer::Serve404(unisim::util::hypapp::HttpRequest const& req, unisim::util::hypapp::ClientConnection const& conn)
{
	// Note: When browsing option "Show friendly HTTP error messages" is enabled, IE5+ does not show 404 error document if it is less than 512 bytes long 
	std::ostringstream doc_sstr;
	doc_sstr << "<!DOCTYPE html>" << std::endl;
	doc_sstr << "<html>" << std::endl;
	doc_sstr << "\t<head>" << std::endl;
	doc_sstr << "\t\t<title>Error 404 (Not Found)</title>" << std::endl;
	doc_sstr << "\t\t<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">" << std::endl;
	doc_sstr << "\t\t<meta name=\"description\" content=\"Error 404 (Not Found)\">" << std::endl;
	doc_sstr << "\t\t<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">" << std::endl;
	doc_sstr << "\t\t<script type=\"application/javascript\">document.domain='" << req.GetDomain() << "';</script>" << std::endl;
	doc_sstr << "\t\t<style>" << std::endl;
	doc_sstr << "\t\t\tbody { font-family:Arial,Helvetica,sans-serif; font-style:normal; font-size:14px; text-align:left; font-weight:400; color:black; background-color:white; }" << std::endl;
	doc_sstr << "\t\t</style>" << std::endl;
	doc_sstr << "\t</head>" << std::endl;
	doc_sstr << "\t<body>" << std::endl;
	doc_sstr << "\t\t<p>Unavailable</p>" << std::endl;
	doc_sstr << "\t</body>" << std::endl;
	doc_sstr << "</html>" << std::endl;

	std::string doc(doc_sstr.str());
	
	std::ostringstream http_header_sstr;
	http_header_sstr << "HTTP/1.1 404 Not Found\r\n";
	http_header_sstr << "Server: UNISIM-VP\r\n";
	http_header_sstr << "Cache-control: no-cache\r\n";
	http_header_sstr << "Connection: keep-alive\r\n";
	http_header_sstr << "Content-length: " << doc.length() << "\r\n";
	http_header_sstr << "Content-Type: text/html; charset=utf-8\r\n";
	http_header_sstr << "\r\n";
	
	std::string http_header(http_header_sstr.str());

	if(!conn.Send(http_header.c_str(), http_header.length())) return false;
	
	if(req.GetRequestType() == unisim::util::hypapp::Request::HEAD) return true;
			
	return conn.Send(doc.c_str(), doc.length());
}

void HttpServer::Serve(unisim::util::hypapp::ClientConnection const& conn)
{
	struct MessageLoop : public unisim::util::hypapp::MessageLoop
	{
		MessageLoop(HttpServer& _http_server)
			: unisim::util::hypapp::MessageLoop(
				_http_server,
				_http_server.verbose ? _http_server.logger.DebugInfoStream()    : _http_server.logger.DebugNullStream(),
				_http_server.logger.DebugWarningStream(),
				_http_server.logger.DebugErrorStream())
			, http_server(_http_server)
		{
		}
		
		virtual bool SendResponse(unisim::util::hypapp::Request const& req, unisim::util::hypapp::ClientConnection const& conn)
		{
			unisim::util::hypapp::HttpRequest http_request(req, http_server.logger.DebugWarningStream());
			
			if(http_request.IsValid()) // http request is valid?
			{
				if(http_server.Verbose())
				{
					http_server.logger << DebugInfo << "URI \"" << req.GetRequestURI() << "\" is valid" << EndDebugInfo;
				}
				
				if(http_request.GetAbsolutePath() == "/") // URI refers to root document (directly or indirectly)
				{
					if(http_server.Verbose())
					{
						http_server.logger << DebugInfo << "URI \"" << req.GetRequestURI() << "\" refers to root" << EndDebugInfo;
					}
					
					return http_server.ServeRootDocument(http_request, conn);
				}
// 				else if((http_request.GetAbsolutePath() == "/header") || (http_request.GetAbsolutePath() == "/header/"))
// 				{
// 					return http_server.ServeHeader(http_request, conn);
// 				}
// 				else if((http_request.GetAbsolutePath() == "/browser") || (http_request.GetAbsolutePath() == "/browser/"))
// 				{
// 					return http_server.ServeBrowser(http_request, conn);
// 				}
				else if((http_request.GetAbsolutePath() == "/config") || (http_request.GetAbsolutePath() == "/config/"))
				{
					return http_server.ServeVariables(http_request, conn, unisim::kernel::service::VariableBase::VAR_PARAMETER);
				}
				else if((http_request.GetAbsolutePath() == "/stats") || (http_request.GetAbsolutePath() == "/stats/"))
				{
					return http_server.ServeVariables(http_request, conn, unisim::kernel::service::VariableBase::VAR_STATISTIC);
				}
				else if((http_request.GetAbsolutePath() == "/kernel") || (http_request.GetAbsolutePath() == "/kernel/"))
				{
					return http_server.Serve404(http_request, conn);
				}
// 				else if((http_request.GetAbsolutePath() == "/footer") || (http_request.GetAbsolutePath() == "/footer/"))
// 				{
// 					return http_server.ServeFooter(http_request, conn);
// 				}
				else
				{
					std::string path = http_server.GetSimulator()->GetSharedDataDirectory() + http_request.GetAbsolutePath();
					
					if(http_server.Verbose())
					{
						http_server.logger << DebugInfo << "looking if \"" << path << "\" exists and is a regular file" << EndDebugInfo;
					}
					
					struct stat st;
					if((stat(path.c_str(), &st) == 0) && S_ISREG(st.st_mode))
					{
						if(http_server.ServeFile(path, conn)) return true;
					}
					else
					{
						if(http_server.Verbose())
						{
							http_server.logger << DebugInfo << "\"" << path << "\" does not exist or is not a regular file" << EndDebugInfo;
						}

						std::size_t pos = 0;
						unisim::kernel::service::Object *object = http_server.FindObject(http_request.GetAbsolutePath(), pos);
						
						if(object)
						{
							if(http_server.Verbose())
							{
								http_server.logger << DebugInfo << "delegating Http request to Object \"" << object->GetName() << "\"" << EndDebugInfo;
							}
							std::string object_http_request_server_root(http_request.GetAbsolutePath().substr(0, pos));
							std::string object_http_request_path(http_request.GetAbsolutePath().substr(pos));
							unisim::util::hypapp::HttpRequest object_http_request(object_http_request_server_root, object_http_request_path, http_request);
							return !http_server.RouteHttpRequest(object, object_http_request, conn) || !http_server.killed;
						}
						else
						{
							if(http_server.Verbose())
							{
								http_server.logger << DebugInfo << "URI \"" << req.GetRequestURI() << "\" refers neither a regular file nor an object" << EndDebugInfo;
							}
						}
					}
				}
			}
			else
			{
				if(http_server.Verbose())
				{
					http_server.logger << DebugInfo << "URI \"" << req.GetRequestURI() << "\" is invalid" << EndDebugInfo;
				}
			}
			
			if(http_server.Verbose())
			{
				http_server.logger << DebugInfo << "sending HTTP response 404 Not Found" << EndDebugInfo;
			}
			
			if(!http_server.Serve404(http_request, conn))
			{
				http_server.logger << DebugWarning << "I/O error or connection closed by peer while sending HTTP 404 Not Found" << EndDebugWarning;
			}
			return false;
		}
		
	private:
		HttpServer& http_server;
	};
	
	MessageLoop msg_loop = MessageLoop(*this);
	msg_loop.Run(conn);
}

bool HttpServer::RouteHttpRequest(unisim::kernel::service::Object *object, unisim::util::hypapp::HttpRequest const& req, unisim::util::hypapp::ClientConnection const& conn)
{
	std::map<unisim::kernel::service::Object *, unisim::kernel::service::ServiceImport<unisim::service::interfaces::HttpServer> *>::iterator it = http_server_map.find(object);
	if(it != http_server_map.end())
	{
		unisim::kernel::service::ServiceImport<unisim::service::interfaces::HttpServer> *import = (*it).second;
		if(import)
		{
			return (*import)->ServeHttpRequest(req, conn);
		}
	}
	
	return Serve404(req, conn);
}

bool HttpServer::ServeDefault(unisim::util::hypapp::HttpRequest const& req, unisim::util::hypapp::ClientConnection const& conn)
{
	std::ostringstream doc_sstr;
	
	doc_sstr << "<!DOCTYPE html>" << std::endl;
	doc_sstr << "<html>" << std::endl;
	doc_sstr << "\t<head>" << std::endl;
	doc_sstr << "\t\t<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">" << std::endl;
	doc_sstr << "\t\t<script type=\"application/javascript\">document.domain='" << req.GetDomain() << "';</script>" << std::endl;
	doc_sstr << "\t</head>" << std::endl;
	doc_sstr << "\t<body>" << std::endl;
	doc_sstr << "\t</body>" << std::endl;
	doc_sstr << "</html>" << std::endl;

	std::string doc(doc_sstr.str());
		
	std::ostringstream http_header_sstr;
	http_header_sstr << "HTTP/1.1 200 OK\r\n";
	http_header_sstr << "Server: UNISIM-VP\r\n";
	http_header_sstr << "Cache-control: no-cache\r\n";
	http_header_sstr << "Connection: keep-alive\r\n";
	http_header_sstr << "Content-length: " << doc.length() << "\r\n";
	http_header_sstr << "Content-Type: text/html; charset=utf-8\r\n";
	http_header_sstr << "\r\n";
	
	std::string http_header(http_header_sstr.str());

	if(!conn.Send(http_header.c_str(), http_header.length())) return false;
	
	if(req.GetRequestType() == unisim::util::hypapp::Request::HEAD) return true;
			
	return conn.Send(doc.c_str(), doc.length());
}

} // end of namespace http_server
} // end of namespace service
} // end of namespace unisim