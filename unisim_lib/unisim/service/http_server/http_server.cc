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
#include <unisim/util/endian/endian.hh>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>

namespace unisim {
namespace service {
namespace http_server {

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;

namespace {

bool ParseHex(void *buffer, unsigned int buf_size, const std::string& str)
{
	std::size_t l_pos = 0;
	std::size_t len = str.length();
	if(l_pos >= len) return false;
	
	char c = 0;
	do
	{
		c = str[l_pos++];
		if((c != ' ') && (c != '\t') && (c != '0'))
		{
			return false;
		}
	}
	while((l_pos < len) && (c != '0'));
	
	if(l_pos >= len) return false;
	
	c = str[l_pos++];
	
	if((c != 'x') && (c != 'X')) return false;
	
	if(l_pos >= len) return false;
	
	std::size_t r_pos = (len - 1);
	
	if(r_pos >= l_pos)
	{
		do
		{
			c = str[r_pos];
			if((c != ' ') && (c != '\t')) break; 
		}
		while(--r_pos >= l_pos);
	}
	
	if(l_pos > r_pos) return false;

#if BYTE_ORDER == BIG_ENDIAN
	uint8_t *buf = (uint8_t *) buffer + buf_size - 1;
	uint8_t *buf_end = (uint8_t *) buffer - 1;
#else
	uint8_t *buf = (uint8_t *) buffer;
	uint8_t *buf_end = (uint8_t *) buffer + buf_size;
#endif
	
	bool h, o;
	for(h = false, o = false; (r_pos >= l_pos); r_pos--, h = !h)
	{
		c = str[r_pos];
		
		char d;
		if((c >= '0') && (c <= '9'))
		{
			d = (c - '0');
		}
		else if((c >= 'a') && (c <= 'f'))
		{
			d = 10 + (c - 'a');
		}
		else if((c >= 'A') && (c <= 'F'))
		{
			d = 10 + (c - 'A');
		}
		else
		{
			return false;
		}
		
		if(!o)
		{
			if(h)
			{
				*buf |= d << 4;
#if BYTE_ORDER == BIG_ENDIAN
				buf--;
#else
				buf++;
#endif
				o = (r_pos > l_pos) && (buf == buf_end);
			}
			else
			{
				*buf = d;
			}
		}
	}
	
	while(buf != buf_end)
	{
		if(h)
		{
			h = false;
		}
		else
		{
			*buf = 0;
		}
#if BYTE_ORDER == BIG_ENDIAN
		buf--;
#else
		buf++;
#endif
	}
	
	return true;
}

template <typename T>
bool ParseHex(T& value, const std::string& str)
{
	return ParseHex(&value, sizeof(value), str);
}

}

HttpServer::HttpServer(const char *name, unisim::kernel::service::Object *parent)
	: unisim::kernel::service::Object(name, parent, "HTTP server")
	, unisim::kernel::service::Client<unisim::service::interfaces::HttpServer>(name, parent)
	, unisim::kernel::service::Client<unisim::service::interfaces::Registers>(name, parent)
	, unisim::util::hypapp::HttpServer()
	, http_server_import()
	, registers_import()
	, logger(*this)
	, sim_program_name(GetSimulator()->FindVariable("program-name")->operator std::string())
	, sim_authors(GetSimulator()->FindVariable("authors")->operator std::string())
	, sim_copyright(GetSimulator()->FindVariable("copyright")->operator std::string())
	, sim_version(GetSimulator()->FindVariable("version")->operator std::string())
	, sim_description(GetSimulator()->FindVariable("version")->operator std::string())
	, sim_license(GetSimulator()->FindVariable("license")->operator std::string())
	, param_verbose("verbose", this, verbose, "enable/disable verbosity")
	, http_port(0)
	, param_http_port("http-port", this, http_port, "HTTP port")
	, http_max_clients(10)
	, param_http_max_clients("http-max-clients", this, http_max_clients, "HTTP max clients")
	, http_server_import_map()
	, registers_import_map()
	, kernel_has_parameters(false)
	, kernel_has_statistics(false)
	, css_files()
	, js_files()
	, browser_actions()
	, toolbar_actions()
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
	for(i = 0; i < MAX_IMPORTS; i++)
	{
		std::stringstream http_server_import_name_sstr;
		http_server_import_name_sstr << "http-server-import[" << i << "]";
		http_server_import[i] = new unisim::kernel::service::ServiceImport<unisim::service::interfaces::HttpServer>(http_server_import_name_sstr.str().c_str(), this);
		
		std::stringstream registers_import_name_sstr;
		registers_import_name_sstr << "registers-import[" << i << "]";
		registers_import[i] = new unisim::kernel::service::ServiceImport<unisim::service::interfaces::Registers>(registers_import_name_sstr.str().c_str(), this);
	}
}

HttpServer::~HttpServer()
{
	Kill();
	JoinLoopThread();
	
	unsigned int i;
	for(i = 0; i < MAX_IMPORTS; i++)
	{
		delete http_server_import[i];
		delete registers_import[i];
	}
	
	for(ToolbarActions::const_iterator it = toolbar_actions.begin(); it != toolbar_actions.end(); it++)
	{
		const unisim::service::interfaces::ToolbarAction *a = *it;
		delete a;
	}

	
	for(BrowserActions::const_iterator it = browser_actions.begin(); it != browser_actions.end(); it++)
	{
		const unisim::service::interfaces::BrowserAction *a = (*it).second;
		delete a;
	}
}

bool HttpServer::EndSetup()
{
	std::list<unisim::kernel::service::VariableBase *> kernel_param_lst;
	std::list<unisim::kernel::service::VariableBase *> kernel_stat_lst;
	
	GetSimulator()->GetRootVariables(kernel_param_lst, unisim::kernel::service::VariableBase::VAR_PARAMETER);
	kernel_has_parameters = !kernel_param_lst.empty();
	
	GetSimulator()->GetRootVariables(kernel_stat_lst, unisim::kernel::service::VariableBase::VAR_STATISTIC);
	kernel_has_statistics = !kernel_stat_lst.empty();
	
	unsigned int i;

	for(i = 0; i < MAX_IMPORTS; i++)
	{
		if(*http_server_import[i])
		{
			unisim::kernel::service::Object *http_server_object = http_server_import[i]->GetService();
		
			if(http_server_object)
			{
				http_server_import_map[http_server_object] = http_server_import[i];
			
				AddJSAction(
					unisim::service::interfaces::BrowserOpenTabAction(
					/* name        */ http_server_object->GetName(),
					/* object name */ http_server_object->GetName(),
					/* label       */ "Open",
					/* tile        */ unisim::service::interfaces::OpenTabAction::TOP_MIDDLE_TILE,
					/* uri         */ http_server_object->URI()
				));
			}
		}
	}
	
	for(i = 0; i < MAX_IMPORTS; i++)
	{
		if(*registers_import[i])
		{
			unisim::kernel::service::Object *object = registers_import[i]->GetService();
			
			if(object)
			{
				registers_import_map[object] = registers_import[i];
			}
		}
	}

	std::list<unisim::kernel::service::Object *> objects;
	GetSimulator()->GetObjects(objects);
	
	std::list<unisim::kernel::service::Object *>::const_iterator object_it;
	for(object_it = objects.begin(); object_it != objects.end(); object_it++)
	{
		unisim::kernel::service::Object *object = *object_it;
		
		std::list<unisim::kernel::service::VariableBase *> param_lst;
		std::list<unisim::kernel::service::VariableBase *> stat_lst;
		object->GetVariables(param_lst, unisim::kernel::service::VariableBase::VAR_PARAMETER);
		object->GetVariables(stat_lst, unisim::kernel::service::VariableBase::VAR_STATISTIC);

		if(!param_lst.empty())
		{
			AddJSAction(
				unisim::service::interfaces::BrowserOpenTabAction(
				/* name        */ std::string("config-") + object->GetName(),
				/* object name */ object->GetName(),
				/* label       */ "Configure",
				/* tile        */ unisim::service::interfaces::OpenTabAction::BOTTOM_TILE,
				/* uri         */ std::string("/config?object=") + unisim::util::hypapp::URI_Encoder::EncodeComponent(object->GetName())
			));
		}
		
		if(!stat_lst.empty())
		{
			AddJSAction(
				unisim::service::interfaces::BrowserOpenTabAction(
				/* name        */ std::string("stats-") + object->GetName(),
				/* object name */ object->GetName(),
				/* label       */ "Show statistics",
				/* tile        */ unisim::service::interfaces::OpenTabAction::BOTTOM_TILE,
				/* uri         */ std::string("/stats?object=") + unisim::util::hypapp::URI_Encoder::EncodeComponent(object->GetName())
			));
		}

		std::map<unisim::kernel::service::Object *, unisim::kernel::service::ServiceImport<unisim::service::interfaces::Registers> *>::iterator it = registers_import_map.find(object);
				
		if(it != registers_import_map.end())
		{
			unisim::kernel::service::ServiceImport<unisim::service::interfaces::Registers> *import = (*it).second;
			
			if(import)
			{
				AddJSAction(
					unisim::service::interfaces::BrowserOpenTabAction(
					/* name        */ std::string("registers-") + object->GetName(),
					/* object name */ object->GetName(),
					/* label       */ "Show registers",
					/* tile        */ unisim::service::interfaces::OpenTabAction::TOP_RIGHT_TILE,
					/* uri         */ std::string("/registers?object=") + unisim::util::hypapp::URI_Encoder::EncodeComponent(object->GetName())
				));
			}
		}
	}
	
	for(i = 0; i < MAX_IMPORTS; i++)
	{
		if(*http_server_import[i])
		{
			struct WebInterfaceModdingScanner : unisim::service::interfaces::WebInterfaceModdingScanner
			{
				WebInterfaceModdingScanner(HttpServer& _http_server)
					: http_server(_http_server)
				{
				}
				
				virtual void Append(const unisim::service::interfaces::CSSFile& f)
				{
					http_server.AddCSSFile(f);
				}
				
				virtual void Append(const unisim::service::interfaces::JSFile& f)
				{
					http_server.AddJSFile(f);
				}
				
				virtual void Append(const unisim::service::interfaces::BrowserDoAction& a)
				{
					http_server.AddJSAction(a);
				}
				
				virtual void Append(const unisim::service::interfaces::BrowserOpenTabAction& a)
				{
					http_server.AddJSAction(a);
				}
				
				virtual void Append(const unisim::service::interfaces::ToolbarDoAction& a)
				{
					http_server.AddJSAction(a);
				}

				virtual void Append(const unisim::service::interfaces::ToolbarOpenTabAction& a)
				{
					http_server.AddJSAction(a);
				}
			private:
				HttpServer& http_server;
			};
			
			WebInterfaceModdingScanner scanner(*this);
			(*http_server_import[i])->ScanWebInterfaceModdings(scanner);
		}
	}
	
	if(verbose)
	{
		logger << DebugInfo << browser_actions.size() << " browser action(s) defined" << EndDebugInfo;
		BrowserActions::const_iterator it;
		for(it = browser_actions.begin(); it != browser_actions.end(); it++)
		{
			const unisim::service::interfaces::BrowserAction *a = (*it).second;
			logger << DebugInfo << (*a) << EndDebugInfo;
		}
	}
	
	StartLoopThread();
	logger << DebugInfo << "HTTP server started on TCP/IP port #" << http_port << EndDebugInfo;
	return true;
}

void HttpServer::AddCSSFile(const unisim::service::interfaces::CSSFile& f)
{
	css_files.push_back(f);
}

void HttpServer::AddJSFile(const unisim::service::interfaces::JSFile& f)
{
	js_files.push_back(f);
}

void HttpServer::AddJSAction(const unisim::service::interfaces::BrowserDoAction& a)
{
	browser_actions.insert(std::pair<std::string, unisim::service::interfaces::BrowserAction *>(a.GetObjectName(), new unisim::service::interfaces::BrowserDoAction(a)));
}

void HttpServer::AddJSAction(const unisim::service::interfaces::BrowserOpenTabAction& a)
{
	browser_actions.insert(std::pair<std::string, unisim::service::interfaces::BrowserAction *>(
		a.GetObjectName(),
		new unisim::service::interfaces::BrowserOpenTabAction(a, std::string("return gui && gui.open_tab('") + unisim::service::interfaces::to_string(a.GetTile()) + "','" + a.GetName() + "','" + a.GetURI() + "')")
	));
}

void HttpServer::AddJSAction(const unisim::service::interfaces::ToolbarDoAction& a)
{
	toolbar_actions.push_back(new unisim::service::interfaces::ToolbarDoAction(a));
}

void HttpServer::AddJSAction(const unisim::service::interfaces::ToolbarOpenTabAction& a)
{
	toolbar_actions.push_back(new unisim::service::interfaces::ToolbarOpenTabAction(a, std::string("return gui && gui.open_tab('") + unisim::service::interfaces::to_string(a.GetTile()) + "','" + a.GetName() + "','" + a.GetURI() + "')"));
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
		pos = hierarchical_name.length();
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
		pos = hierarchical_name.length();
		return found_root_object;
	}
	
	pos = hierarchical_delimiter_pos;
	
	unisim::kernel::service::Object *found_object = (pos < hierarchical_name.length()) ? FindChildObject(found_root_object, hierarchical_name, pos) : 0;
	
	return found_object ? found_object : found_root_object;
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

void HttpServer::Crawl(std::ostream& os, unisim::kernel::service::Object *object, unsigned int indent_level, bool last)
{
	const std::list<unisim::kernel::service::Object *>& leaf_objects = object->GetLeafs();
	
	unisim::kernel::service::ServiceImport<unisim::service::interfaces::HttpServer> *import = 0;
	std::map<unisim::kernel::service::Object *, unisim::kernel::service::ServiceImport<unisim::service::interfaces::HttpServer> *>::iterator it = http_server_import_map.find(object);
	if(it != http_server_import_map.end())
	{
		import = (*it).second;
	}
	
	std::pair<BrowserActions::const_iterator, BrowserActions::const_iterator> browser_actions_range = browser_actions.equal_range(object->GetName());
	
	if(import || (browser_actions_range.first != browser_actions_range.second) || !leaf_objects.empty())
	{
		for(unsigned int i = 0; i < indent_level; i++) os << '\t';
		os << "<li";
		if(last)
		{
			os << " class=\"last\"";
		}
		os << ">" << std::endl;
		
		indent_level++;
		
		for(unsigned int i = 0; i < indent_level; i++) os << '\t';
		os << "<span title=\"" << object->GetDescription() << "\" class=\"browser-item noselect\"";
		
		if(import)
		{
			os << " ondblclick=\"gui.open_tab('top-middle-tile','" <<  unisim::util::hypapp::HTML_Encoder::Encode(object->GetName()) << "','" << object->GetName() << "','" << object->URI() << "')\"";
		}

		if(browser_actions_range.first != browser_actions_range.second)
		{
			os << " oncontextmenu=\"gui.create_context_menu(event, [";
			for(BrowserActions::const_iterator it = browser_actions_range.first; it != browser_actions_range.second; it++)
			{
				if(it != browser_actions_range.first) os << ",";
				const unisim::service::interfaces::BrowserAction *a = (*it).second;
				os << "{label:'" << a->GetLabel() << "', action:function() { " << a->GetJSCodeSnippet() << "; } }";
			}
			os << "])\"";
		}
		else
		{
			os << " oncontextmenu=\"return false\"";
		}
		os << ">" << unisim::util::hypapp::HTML_Encoder::Encode(object->GetObjectName());
		os << "</span>" << std::endl;;
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
				
				Crawl(os, child, indent_level + 1, next_it == leaf_objects.end());
			}
			
			indent_level--;
			for(unsigned int i = 0; i < indent_level; i++) os << '\t';
			os << "</ul>" << std::endl;
			
			indent_level--;
			for(unsigned int i = 0; i < indent_level; i++) os << '\t';
			os << "</li>" << std::endl;
		}
	}
}

void HttpServer::Crawl(std::ostream& os, unsigned int indent_level)
{
	for(unsigned int i = 0; i < indent_level; i++) os << '\t';
	
	os << "<span";
	if(!sim_program_name.empty() || !sim_authors.empty() || !sim_copyright.empty() || !sim_version.empty() || !sim_description.empty() || !sim_license.empty())
	{
		os << " title=\"";
		if(!sim_program_name.empty())
		{
			os << sim_program_name;
			if(!sim_version.empty())
			{
				os << " " << sim_version;
			}
			os << std::endl;
		}
		if(!sim_copyright.empty())
		{
			os << sim_copyright << std::endl;
		}
		if(!sim_license.empty())
		{
			os << "License: " << sim_license << std::endl;
		}
		if(!sim_authors.empty())
		{
			os << "Authors: " << sim_authors << std::endl;
		}
		os << "\"";
	}
	os << " class=\"browser-item noselect\"";
	if(kernel_has_parameters || kernel_has_statistics)
	{
		os << " oncontextmenu=\"gui.create_context_menu(event, [";
		if(kernel_has_parameters)
		{
			os << "{label:'Configure', action:function() { gui.open_tab('bottom-tile','Configuration of " <<  unisim::util::hypapp::HTML_Encoder::Encode(sim_program_name) << "','config-kernel','/config?object=kernel'); } }";
		}
		if(kernel_has_parameters && kernel_has_statistics)
		{
			os << ",";
		}
		if(kernel_has_statistics)
		{
			os << "{label:'Show statistics', action:function() { gui.open_tab('bottom-tile','Statistics of " <<  unisim::util::hypapp::HTML_Encoder::Encode(sim_program_name) << "','stats-kernel','/stats?object=kernel'); } }";
		}
		os << "])\"";
	}
	os << ">" << unisim::util::hypapp::HTML_Encoder::Encode(sim_program_name);
	os << "</span>" << std::endl;

	std::list<unisim::kernel::service::Object *> root_objects;
	
	GetSimulator()->GetRootObjects(root_objects);
	
	if(!root_objects.empty())
	{
		for(unsigned int i = 0; i < indent_level; i++) os << '\t';
		os << "<ul class=\"tree\">" << std::endl;
		indent_level++;
		
		std::list<unisim::kernel::service::Object *>::const_iterator it;
		std::list<unisim::kernel::service::Object *>::const_iterator next_it;
		
		for(it = root_objects.begin(); it != root_objects.end(); it = next_it)
		{
			unisim::kernel::service::Object *root_object = *it;
			next_it = it;
			next_it++;
			
			Crawl(os, root_object, indent_level + 1, next_it == root_objects.end());
		}
		indent_level--;
		for(unsigned int i = 0; i < indent_level; i++) os << '\t';
		os << "</ul>" << std::endl;
	}
}

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
	
	bool is_kernel = false;
	unisim::kernel::service::Object *object = 0;
	if(req.HasQuery())
	{
		QueryDecoder query_decoder(*this);
	
		if(query_decoder.Decode(req.GetQuery(), logger.DebugWarningStream()))
		{
			is_kernel = (query_decoder.object_name == "kernel");
			object = is_kernel ? 0 : GetSimulator()->FindObject(query_decoder.object_name.c_str());
		}
	}
	else
	{
		if(verbose)
		{
			logger << DebugInfo << "missing query" << EndDebugInfo;
		}
	}

	std::ostringstream doc_sstr;
	
	doc_sstr << "<!DOCTYPE html>" << std::endl;
	doc_sstr << "<html>" << std::endl;
	doc_sstr << "\t<head>" << std::endl;
	doc_sstr << "\t\t<title>";
	switch(var_type)
	{
		case unisim::kernel::service::VariableBase::VAR_PARAMETER:
			doc_sstr << "Configuration of ";
			break;
			
		case unisim::kernel::service::VariableBase::VAR_STATISTIC:
			doc_sstr << "Statistics of ";
			break;
			
		default:
			doc_sstr << "Variables of unknown type of";
			break;
	}
	doc_sstr << (object ? unisim::util::hypapp::HTML_Encoder::Encode(object->GetName()) : "an unknown object");
	doc_sstr << "</title>" << std::endl;
	doc_sstr << "\t\t<meta name=\"description\" content=\"user interface for object variables over HTTP\">" << std::endl;
	doc_sstr << "\t\t<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">" << std::endl;
	doc_sstr << "\t\t<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">" << std::endl;
	doc_sstr << "\t\t<link rel=\"shortcut icon\" type=\"image/x-icon\" href=\"/favicon.ico\" />" << std::endl;
	doc_sstr << "\t\t<link rel=\"stylesheet\" href=\"/unisim/service/http_server/var_style.css\" type=\"text/css\" />" << std::endl;
	doc_sstr << "\t\t<script type=\"application/javascript\">document.domain='" << req.GetDomain() << "';</script>" << std::endl;
	doc_sstr << "\t\t<script type=\"application/javascript\" src=\"/unisim/service/http_server/uri.js\"></script>" << std::endl;
	doc_sstr << "\t\t<script type=\"application/javascript\" src=\"/unisim/service/http_server/embedded_script.js\"></script>" << std::endl;
	doc_sstr << "\t\t<script type=\"application/javascript\" src=\"/unisim/service/http_server/var_script.js\"></script>" << std::endl;
	doc_sstr << "\t</head>" << std::endl;
	doc_sstr << "\t<body>" << std::endl;
	
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
				doc_sstr << "\t\t\t\t\t\t<td class=\"var-name\">" << unisim::util::hypapp::HTML_Encoder::Encode(var->GetVarName()) << "</td>" << std::endl;
				doc_sstr << "\t\t\t\t\t\t<td class=\"var-value\">" << std::endl;
				doc_sstr << "\t\t\t\t\t\t\t<form action=\"/config?object=";
				if(object) doc_sstr << unisim::util::hypapp::HTML_Encoder::Encode(object->GetName());
				doc_sstr << "\" method=\"post\" enctype=\"application/x-www-form-urlencoded\" onsubmit=\"save_window_scroll_top()\">" << std::endl;
				doc_sstr << "\t\t\t\t\t\t\t\t<input";
				if(var->HasEnumeratedValues())
				{
					doc_sstr << " list=\"input-list-" << var_id << "\"";
				}
				doc_sstr << " class=\"var-value-text" << (var->IsMutable() ? "" : " disabled") << "\" type=\"text\" name=\"" << unisim::util::hypapp::HTML_Encoder::Encode(var->GetVarName()) << "\" value=\"" << unisim::util::hypapp::HTML_Encoder::Encode(std::string(*var)) << "\"" /*<< (var->IsMutable() ? "" : " disabled")*/ << (var->IsMutable() ? "" : " readonly") << ">" << std::endl;
				if(var->HasEnumeratedValues())
				{
					doc_sstr << "\t\t\t\t\t\t\t\t<datalist id=\"input-list-" << var_id << "\">" << std::endl;
					std::vector<std::string> values;
					var->GetEnumeratedValues(values);
					
					std::vector<std::string>::const_iterator it;
					for(it = values.begin(); it != values.end(); it++)
					{
						const std::string& value = (*it);
						doc_sstr << "\t\t\t\t\t\t\t\t\t\t<option value=\"" << unisim::util::hypapp::HTML_Encoder::Encode(value) << "\">" << unisim::util::hypapp::HTML_Encoder::Encode(value) << "</option>" << std::endl;
					}
					doc_sstr << "\t\t\t\t\t\t\t\t</datalist>" << std::endl;
				}
				
				doc_sstr << "\t\t\t\t\t\t\t</form>" << std::endl;
				doc_sstr << "\t\t\t\t\t\t</td>" << std::endl;
				doc_sstr << "\t\t\t\t\t\t<td class=\"var-data-type\">" << unisim::util::hypapp::HTML_Encoder::Encode(var->GetDataTypeName()) << "</td>" << std::endl;
				doc_sstr << "\t\t\t\t\t\t<td class=\"var-description\">" << unisim::util::hypapp::HTML_Encoder::Encode(var->GetDescription()) << "</td>" << std::endl;
				doc_sstr << "\t\t\t\t\t</tr>" << std::endl;
			}
			
			doc_sstr << "\t\t\t\t</tbody>" << std::endl;
			doc_sstr << "\t\t\t</table>" << std::endl;
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

bool HttpServer::ServeRegisters(unisim::util::hypapp::HttpRequest const& req, unisim::util::hypapp::ClientConnection const& conn)
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

	unisim::kernel::service::Object *object = 0;
	if(req.HasQuery())
	{
		QueryDecoder query_decoder(*this);
	
		if(query_decoder.Decode(req.GetQuery(), logger.DebugWarningStream()))
		{
			object = GetSimulator()->FindObject(query_decoder.object_name.c_str());
		}
	}
			
	std::ostringstream doc_sstr;
	
	doc_sstr << "<!DOCTYPE html>" << std::endl;
	doc_sstr << "<html>" << std::endl;
	doc_sstr << "\t<head>" << std::endl;
	doc_sstr << "\t\t<title>Registers of " << (object ? unisim::util::hypapp::HTML_Encoder::Encode(object->GetName()) : "an unknown object") << "</title>" << std::endl;
	doc_sstr << "\t\t<meta name=\"description\" content=\"user interface for " << (object ? unisim::util::hypapp::HTML_Encoder::Encode(object->GetName()) : "object") << " registers over HTTP\">" << std::endl;
	doc_sstr << "\t\t<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">" << std::endl;
	doc_sstr << "\t\t<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">" << std::endl;
	doc_sstr << "\t\t<link rel=\"shortcut icon\" type=\"image/x-icon\" href=\"/favicon.ico\" />" << std::endl;
	doc_sstr << "\t\t<link rel=\"stylesheet\" href=\"/unisim/service/http_server/reg_style.css\" type=\"text/css\" />" << std::endl;
	doc_sstr << "\t\t<script type=\"application/javascript\">document.domain='" << req.GetDomain() << "';</script>" << std::endl;
	doc_sstr << "\t\t<script type=\"application/javascript\" src=\"/unisim/service/http_server/uri.js\"></script>" << std::endl;
	doc_sstr << "\t\t<script type=\"application/javascript\" src=\"/unisim/service/http_server/embedded_script.js\"></script>" << std::endl;
	doc_sstr << "\t\t<script type=\"application/javascript\" src=\"/unisim/service/http_server/reg_script.js\"></script>" << std::endl;
	doc_sstr << "\t</head>" << std::endl;
	doc_sstr << "\t<body>" << std::endl;
	
	if(object)
	{
		std::map<unisim::kernel::service::Object *, unisim::kernel::service::ServiceImport<unisim::service::interfaces::Registers> *>::iterator it = registers_import_map.find(object);
		
		if(it != registers_import_map.end())
		{
			unisim::kernel::service::ServiceImport<unisim::service::interfaces::Registers> *import = (*it).second;
			
			if(import)
			{
				if(req.GetRequestType() == unisim::util::hypapp::Request::POST)
				{
					struct Form_URL_Encoded_Decoder : public unisim::util::hypapp::Form_URL_Encoded_Decoder
					{
						virtual bool FormAssign(const std::string& _name, const std::string& _value)
						{
							reg_name = _name;
							reg_value_str = _value;
							return true;
						}
						
						std::string reg_name;
						std::string reg_value_str;
					};

					Form_URL_Encoded_Decoder decoder;
					if(decoder.Decode(std::string(req.GetContent(), req.GetContentLength()), logger.DebugWarningStream()))
					{
						struct Setter : unisim::service::interfaces::RegisterScanner
						{
							Setter(const std::string _reg_name, const std::string& _reg_value_str, std::ostream& _warn_log) : reg_name(_reg_name), reg_value_str(_reg_value_str), warn_log(_warn_log) {}
							
							virtual void Append(unisim::service::interfaces::Register * reg)
							{
								if(reg_name == reg->GetName())
								{
									unsigned int reg_size = reg->GetSize();
									uint8_t reg_value[reg_size];
									if(ParseHex(reg_value, reg_size, reg_value_str))
									{
										reg->SetValue(reg_value);
									}
									else
									{
										warn_log << "parse error in \"" << reg_value_str << "\"" << std::endl;
									}
								}
							}
						private:
							const std::string& reg_name;
							const std::string& reg_value_str;
							std::ostream& warn_log;
						};
						
						Setter setter(decoder.reg_name, decoder.reg_value_str, logger.DebugWarningStream());
						(*import)->ScanRegisters(setter);
					}
				}
				
				struct Printer : unisim::service::interfaces::RegisterScanner
				{
					Printer(unisim::kernel::service::Object *_object, std::ostream& _doc_sstr) : object(_object), doc_sstr(_doc_sstr) {}
					
					virtual void Append(unisim::service::interfaces::Register * reg)
					{
						unsigned int reg_size = reg->GetSize();
						uint8_t reg_value[reg_size];
						reg->GetValue(&reg_value);
						
						doc_sstr << "\t\t\t\t<tr>" << std::endl;
						doc_sstr << "\t\t\t\t\t<td class=\"reg-name\">" << unisim::util::hypapp::HTML_Encoder::Encode(reg->GetName()) << "</td>" << std::endl;
						doc_sstr << "\t\t\t\t\t<td class=\"reg-size\">" << (reg->GetSize() * 8) << "</td>" << std::endl;
						doc_sstr << "\t\t\t\t\t<td class=\"reg-value\">" << std::endl;
						doc_sstr << "\t\t\t\t\t\t<form action=\"/registers?object=" << unisim::util::hypapp::HTML_Encoder::Encode(object->GetName()) << "\" method=\"post\" enctype=\"application/x-www-form-urlencoded\" onsubmit=\"save_window_scroll_top()\">" << std::endl;
						doc_sstr << "\t\t\t\t\t\t\t<input class=\"reg-value-text\" type=\"text\" name=\"" << unisim::util::hypapp::HTML_Encoder::Encode(reg->GetName()) << "\" value=\"0x" << std::hex;
#if BYTE_ORDER == BIG_ENDIAN
						for(int i = 0; i < (int) reg_size; i++)
#else
						for(int i = (reg_size - 1); i >= 0; i--)
#endif
						{
							doc_sstr << (reg_value[i] >> 4);
							doc_sstr << (reg_value[i] & 15);
						}
						
						doc_sstr << std::dec << "\">" << std::endl;
						
						doc_sstr << "\t\t\t\t\t\t</form>" << std::endl;
						doc_sstr << "\t\t\t\t\t</td>" << std::endl;
						doc_sstr << "\t\t\t\t</tr>" << std::endl;
					}
				private:
					unisim::kernel::service::Object *object;
					std::ostream& doc_sstr;
				};
				
				doc_sstr << "\t\t<table class=\"reg-table\">" << std::endl;
				doc_sstr << "\t\t\t<thead>" << std::endl;
				doc_sstr << "\t\t\t\t<tr>" << std::endl;
				doc_sstr << "\t\t\t\t\t<th class=\"reg-name\">Name</th>" << std::endl;
				doc_sstr << "\t\t\t\t\t<th class=\"reg-size\">Size</th>" << std::endl;
				doc_sstr << "\t\t\t\t\t<th class=\"reg-value\">Value</th>" << std::endl;
				doc_sstr << "\t\t\t\t</tr>" << std::endl;
				doc_sstr << "\t\t\t</thead>" << std::endl;
				doc_sstr << "\t\t\t<tbody>" << std::endl;
				Printer printer(object, doc_sstr);
				(*import)->ScanRegisters(printer);
				doc_sstr << "\t\t\t</tbody>" << std::endl;
				doc_sstr << "\t\t</table>" << std::endl;
			}
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

bool HttpServer::ServeRootDocument(unisim::util::hypapp::HttpRequest const& req, unisim::util::hypapp::ClientConnection const& conn)
{
	std::ostringstream doc_sstr;
	
	doc_sstr << "<!DOCTYPE html>" << std::endl;
	doc_sstr << "<html>" << std::endl;
	doc_sstr << "\t<head>" << std::endl;
	doc_sstr << "\t\t<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">" << std::endl;
	doc_sstr << "\t\t<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">" << std::endl;
	doc_sstr << "\t\t<meta name=\"description\" content=\"remote control interface over HTTP\">" << std::endl;
	doc_sstr << "\t\t<title>" << unisim::util::hypapp::HTML_Encoder::Encode(sim_program_name) << "</title>" << std::endl;
	doc_sstr << "\t\t<link rel=\"shortcut icon\" type=\"image/x-icon\" href=\"/favicon.ico\" />" << std::endl;
	doc_sstr << "\t\t<link rel=\"stylesheet\" href=\"/unisim/service/http_server/style.css\" type=\"text/css\" />" << std::endl;
	
	for(CSSFiles::const_iterator it = css_files.begin(); it != css_files.end(); it++)
	{
		const unisim::service::interfaces::CSSFile& css_file = *it;
		doc_sstr << "<link rel=\"stylesheet\" type=\"text/css\" href=\"" << css_file.GetFilename() << "\" />" << std::endl;
	}
	
	doc_sstr << "\t\t<script type=\"application/javascript\">document.domain='" << req.GetDomain() << "';</script>" << std::endl;
	doc_sstr << "\t\t<script type=\"application/javascript\" src=\"/unisim/service/http_server/uri.js\"></script>" << std::endl;
	doc_sstr << "\t\t<script type=\"application/javascript\" src=\"/unisim/service/http_server/script.js\"></script>" << std::endl;
	
	for(JSFiles::const_iterator it = js_files.begin(); it != js_files.end(); it++)
	{
		const unisim::service::interfaces::JSFile& js_file = *it;
		doc_sstr << "\t\t<script type=\"application/javascript\" src=\"" << js_file.GetFilename() << "\"></script>" << std::endl;
	}
	
	doc_sstr << "\t</head>" << std::endl;
	doc_sstr << "\t<body";
#if 0
	doc_sstr << " oncontextmenu=\"return false;\"" << std::endl; // this disables right-clicks
#endif
	doc_sstr << ">" << std::endl;
	
	doc_sstr << "\t<div id=\"toolbar-div\">" << std::endl;
	doc_sstr << "\t\t<div class=\"toolbar\">" << std::endl;
	for(ToolbarActions::const_iterator it = toolbar_actions.begin(); it != toolbar_actions.end(); it++)
	{
		const unisim::service::interfaces::ToolbarAction *a = *it;
		doc_sstr << "\t\t\t<div class=\"toolbar-item\">" << std::endl;
		doc_sstr << "\t\t\t\t<div id=\"" << a->GetName() << "\" title=\"" << a->GetTips() << "\" class=\"toolbar-button\" onclick=\"" << a->GetJSCodeSnippet() << "\">" << a->GetLabel() << "</div>" << std::endl;
		doc_sstr << "\t\t\t</div>" << std::endl;
	}
	doc_sstr << "\t\t</div>" << std::endl;
	doc_sstr << "\t</div>" << std::endl;
	doc_sstr << "\t<div id=\"content-div\">" << std::endl;
	doc_sstr << "\t\t<div class=\"tile\" id=\"left-tile-div\">" << std::endl;
	doc_sstr << "\t\t\t<div class=\"tab-headers-history-shortcut\" id=\"left-tab-headers-history-shortcut\"></div>" << std::endl;
	doc_sstr << "\t\t\t<div class=\"tab-headers-scroller tab-headers-left-scroller\" id=\"left-tab-headers-left-scroller\"></div>" << std::endl;
	doc_sstr << "\t\t\t<div class=\"tab-headers-scroller tab-headers-right-scroller\" id=\"left-tab-headers-right-scroller\"></div>" << std::endl;
	doc_sstr << "\t\t\t<div class=\"tab-headers\" id=\"left-tab-headers-div\">" << std::endl;
	doc_sstr << "\t\t\t\t<div class=\"tab-header noselect left-tab-header-div\" id=\"browser-tab-header\">" << std::endl;
	doc_sstr << "\t\t\t\t\t<span title=\"Browser of the hierarchy of simulation objects" << std::endl << std::endl << "Tips:" << std::endl << "- Hover over the object to display its description" << std::endl << "- Double click on the object to open it" << std::endl << "- Right click on the object to show a context menu\">Browser</span>" << std::endl;
	doc_sstr << "\t\t\t\t</div>" << std::endl;
	doc_sstr << "\t\t\t</div>" << std::endl;
	doc_sstr << "\t\t\t<div class=\"tab-contents\" id=\"left-tab-contents-div\">" << std::endl;
	doc_sstr << "\t\t\t\t<div name=\"browser-tab-content\" class=\"tab-content left-tab-content-div\" id=\"browser-tab-content\">" << std::endl;
	doc_sstr << "\t\t\t\t\t<div class=\"noselect\" id=\"browser\">" << std::endl;
	Crawl(doc_sstr, 6);
	doc_sstr << "\t\t\t\t\t</div>" << std::endl;
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
				else if((http_request.GetAbsolutePath() == "/config") || (http_request.GetAbsolutePath() == "/config/"))
				{
					return http_server.ServeVariables(http_request, conn, unisim::kernel::service::VariableBase::VAR_PARAMETER);
				}
				else if((http_request.GetAbsolutePath() == "/stats") || (http_request.GetAbsolutePath() == "/stats/"))
				{
					return http_server.ServeVariables(http_request, conn, unisim::kernel::service::VariableBase::VAR_STATISTIC);
				}
				else if((http_request.GetAbsolutePath() == "/registers") || (http_request.GetAbsolutePath() == "/registers/"))
				{
					return http_server.ServeRegisters(http_request, conn);
				}
				else if((http_request.GetAbsolutePath() == "/kernel") || (http_request.GetAbsolutePath() == "/kernel/"))
				{
					return http_server.Serve404(http_request, conn);
				}
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
	std::map<unisim::kernel::service::Object *, unisim::kernel::service::ServiceImport<unisim::service::interfaces::HttpServer> *>::iterator it = http_server_import_map.find(object);
	if(it != http_server_import_map.end())
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
