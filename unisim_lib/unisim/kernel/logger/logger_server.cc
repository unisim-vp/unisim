/*
 *  Copyright (c) 2008,
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
 * Author: Daniel Gracia Perez (daniel.gracia-perez@cea.fr) 
 */

#include <unisim/kernel/logger/logger_server.hh>
#include <unisim/kernel/kernel.hh>
#include <unisim/util/hypapp/hypapp.hh>

#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>

#include <sstream>
#include <string>
#include <iostream>
#include <stdexcept>

namespace unisim {
namespace kernel {
namespace logger {

static const char *XML_ENCODING = "UTF-8"; 

LoggerServer::LoggerServer(const char *name, unisim::kernel::Object *parent)
	: unisim::kernel::Object(name, parent, "Logger")
	, unisim::kernel::Service<unisim::service::interfaces::HttpServer>(name, parent)
	, http_server_export("http-server-export", this)
	, clients()
	, xml_writer_(0)
	, text_file_()
	, global_http_log()
	, http_logs_per_client()
	, opt_std_err_(true)
	, opt_std_out_(false)
	, opt_std_err_color_(false)
	, opt_std_out_color_(false)
	, opt_file_(false)
	, opt_filename_("logger_output.txt")
	, opt_xml_file_(false)
	, opt_xml_filename_("logger_output.xml")
	, opt_xml_file_gzipped_(false)
	, opt_http_(false)
	, opt_http_max_log_size_(256)
	, mutex()
	, param_std_err("std_err", this, opt_std_err_, "Show logger output through the standard error output")
	, param_std_out("std_out", this, opt_std_out_, "Show logger output through the standard output")
	, param_std_err_color("std_err_color", this, opt_std_err_color_, "Colorize logger output through the standard error output (only works if std_err is active)")
	, param_std_out_color("std_out_color", this, opt_std_out_color_, "Colorize logger output through the standard output (only works if std_out is active)")
	, param_file("file", this, opt_file_, "Keep logger output in a file")
	, param_filename("filename", this, opt_filename_, "Filename to keep logger output (the option file must be activated)")
	, param_xml_file("xml_file", this, opt_xml_file_, "Keep logger output in a file xml formatted")
	, param_xml_filename("xml_filename", this, opt_xml_filename_, "Filename to keep logger xml output (the option xml_file must be activated)")
	, param_xml_file_gzipped("xml_file_gzipped", this, opt_xml_file_gzipped_, "Compress the xml output (a .gz extension is automatically appended to the xml_filename option)")
	, param_http("http", this, opt_http_, "Show logger output through HTTP")
	, param_http_max_log_size("http_max_log_size", this, opt_http_max_log_size_, "Maximum log size for HTTP output")
{
	param_http_max_log_size.SetFormat(unisim::kernel::VariableBase::FMT_DEC);
	
	pthread_mutex_init(&mutex, NULL);
}

LoggerServer::~LoggerServer()
{
	Close();
	for(HTTP_LOGS_PER_CLIENT::iterator it = http_logs_per_client.begin(); it != http_logs_per_client.end(); it++)
	{
		HTTP_LOG *http_log = (*it).second;
		delete http_log;
	}
	pthread_mutex_destroy(&mutex);
}

void LoggerServer::Close()
{
	if (clients.size())
	{
		std::cerr << "Error(LoggerServer::close): "
		          << "client loggers still connected" << std::endl;
		std::set<Logger const*>::iterator client_iter;
		for(client_iter = clients.begin(); client_iter != clients.end(); client_iter++)
		{
			Logger const* client = *client_iter;
			std::cerr << "Error(LoggerServer::close): client \"" << client->GetName() << "\" still connected" << std::endl;
		}
	}

	if(xml_writer_ != NULL)
	{
		int rc = xmlTextWriterEndElement(xml_writer_);
		if(rc < 0)
		{
			std::cerr << "Error(LoggerServer): "
			          << "could not close the root element of xml output" << std::endl;
		}
		rc = xmlTextWriterEndDocument(xml_writer_);
		if(rc < 0)
		{
			std::cerr << "Warning(LoggerServer::Close): "
			          << "could not correctly close the xml output file" << std::endl;
		}
		xmlFreeTextWriter(xml_writer_);
		xml_writer_ = NULL;
	}

	if (text_file_.is_open())
	{
		text_file_.close();
		if (text_file_.is_open())
		{
			std::cerr << "Error(LoggerServer::Close): "
			          << "could not correctly close the text output file" << std::endl;
			throw std::logic_error("internal error");
		}
	}
}

bool LoggerServer::Setup() 
{
	/* check if a xml output needs to be created */
	if (opt_xml_file_) 
	{
		/* create and initialize the xml output */
		std::stringstream xml_filename;
		xml_filename << opt_xml_filename_;
		if (opt_xml_file_gzipped_) xml_filename << ".gz";
		xml_writer_ = xmlNewTextWriterFilename(xml_filename.str().c_str(), opt_xml_file_gzipped_ ? 1 : 0);
		if (xml_writer_ == NULL) 
		{
			std::cerr << "Error(LoggerServer::Setup): "
			          << "could not open xml output file for logging" << std::endl;
			return false;
		}
		int rc = xmlTextWriterSetIndent(xml_writer_, 1);
		if (rc < 0) 
		{
			std::cerr << "Warning(LoggerServer::Setup): "
			          << "could not set indentation" << std::endl;
		}
		rc = xmlTextWriterStartDocument(xml_writer_, NULL, XML_ENCODING, NULL);
		if (rc < 0) 
		{
			std::cerr << "Error(LoggerServer::Setup): "
			          << "error starting the xml document" << std::endl;
			return false;
		}
		rc = xmlTextWriterStartElement(xml_writer_, BAD_CAST "LOGGER");
		if (rc < 0) 
		{
			std::cerr << "Error(LoggerServer::Setup): "
			          << "error starting the xml document" << std::endl;
			return false;
		}
	}
	/* check if a file text output needs to be created */
	if (opt_file_)
	{
		/* create and initialize the text output file */
		std::stringstream filename;
		filename << opt_filename_;
		text_file_.open(filename.str().c_str(), std::ios::trunc);
		if (!text_file_.is_open())
		{
			std::cerr << "Error(LoggerServer::Setup): "
			          << "could not open text output file for logging" << std::endl;
			return false;
		}
	}
	return true;
}

void LoggerServer::AddClient( Logger const* client )
{
	if (not clients.insert( client ).second)
	{
		std::cerr << "Error(LoggerServer::AddClient): "
		          << "internal issue, client already connected" << std::endl;
	}
}

void LoggerServer::RemoveClient( Logger const* client )
{
	if (not clients.erase( client ))
	{
		std::cerr << "Error(LoggerServer::AddClient): "
		          << "internal issue, attempting to disconnect a unknown client" << std::endl;
	}
}

void LoggerServer::XmlDebug(const char *type, const char *name, const char *buffer)
{
	int rc;
	rc = xmlTextWriterStartElement(xml_writer_, BAD_CAST "DEBUG");
	if (rc < 0)
	{
		std::cerr << "Error(LoggerServer): could not add a debug message of type \"" << type << "\"" << std::endl;
	}
	xmlChar *xml_type = xmlCharStrdup(type);
	rc = xmlTextWriterWriteAttribute(xml_writer_, BAD_CAST "type", xml_type);
	free(xml_type);
	if(rc < 0)
	{
		std::cerr << "Error(LoggerServer): could not add \"type\" attribute to debug message of type \"" << type << "\"" << std::endl;
	}
	xmlChar *xml_obj_name = xmlCharStrdup(name);
	rc = xmlTextWriterWriteAttribute(xml_writer_, BAD_CAST "source", xml_obj_name);
	free(xml_obj_name);
	if (rc < 0)
	{
		std::cerr << "Error(LoggerServer): could not add \"source\" attribute to debug message of type \"" << type << "\"" << std::endl;
	}
	rc = xmlTextWriterWriteFormatString(xml_writer_, "%s", buffer);
	if (rc < 0) 
	{
		std::cerr << "Error(LoggerServer): could not attach message buffer to debug message of type \"" << type << "\" buffer = \"" << buffer << "\"" << std::endl;
	}
	rc = xmlTextWriterEndElement(xml_writer_);
	if (rc < 0)
	{
		std::cerr << "Error(LoggerServer): could not close debug message of type \"" << type << "\"" << std::endl;
	}
}

void LoggerServer::Print(std::ostream& os, bool opt_color, mode_t mode, const char *name, const char *buffer)
{
	if(opt_color)
	{
		switch(mode)
		{
			case INFO_MODE: os << "\033[36m"; break;
			case WARNING_MODE: os << "\033[33m"; break;
			case ERROR_MODE: os << "\033[31m"; break;
			default: break;
		}
	}

	os << name << ": ";
	switch(mode)
	{
		case WARNING_MODE: os << "WARNING! "; break;
		case ERROR_MODE: os << "ERROR! "; break;
		default: break;
	}
	int prefix_length = strlen(name) + 2;
	std::string prefix(prefix_length, ' ');
	const char *b = buffer;
	for (const char *p = strchr(b, '\n'); p != NULL; p = strchr(b, '\n'))
	{
		os.write(b, p - b);
		os << std::endl << prefix;
		b = p + 1;
	}
	os << b << std::endl;
		
	if (opt_color) os << "\033[0m";
}

void LoggerServer::Print(mode_t mode, const char *name, const char *buffer)
{
	pthread_mutex_lock(&mutex);
	
	if(opt_http_)
	{
		
		HTTP_LOGS_PER_CLIENT::iterator it = http_logs_per_client.find(name);
		HTTP_LOG *http_log = 0;
		if(it != http_logs_per_client.end())
		{
			http_log = (*it).second;
		}
		else
		{
			http_logs_per_client[name] = http_log = new HTTP_LOG();
		}
		
		while(global_http_log.size() > opt_http_max_log_size_)
		{
			global_http_log.pop_front();
		}
		while(http_log->size() > opt_http_max_log_size_)
		{
			http_log->pop_front();
		}
		
		HTTP_LOG_ENTRY http_log_entry(mode, name, buffer);
		
		global_http_log.push_back(http_log_entry);
		http_log->push_back(http_log_entry);
	}
	
	if(opt_std_out_)
	{
		Print(std::cout, opt_std_out_color_, mode, name, buffer);
	}
	if (opt_std_err_)
	{
		Print(std::cerr, opt_std_err_color_, mode, name, buffer);
	}
	if(opt_xml_file_)
	{
		switch(mode)
		{
			case INFO_MODE: XmlDebug("info", name, buffer); break;
			case WARNING_MODE: XmlDebug("warning", name, buffer); break;
			case ERROR_MODE: XmlDebug("error", name, buffer); break;
			default: break;
		}
		
	}
	if(opt_file_)
	{
		text_file_ << name << ": ";
		switch(mode)
		{
			case WARNING_MODE: text_file_ << "WARNING! "; break;
			case ERROR_MODE: text_file_ << "ERROR! "; break;
			default: break;
		}
		text_file_ << buffer << std::endl;
	}
	
	pthread_mutex_unlock(&mutex);
}

void LoggerServer::DebugNull( const char *name, const char *buffer )
{
}

void LoggerServer::DebugInfo(const char *name, const char *buffer)
{
	Print(INFO_MODE, name, buffer);
}

void LoggerServer::DebugWarning(const char *name, const char *buffer)
{
	Print(WARNING_MODE, name, buffer);
}

void LoggerServer::DebugError(const char *name, const char *buffer)
{
	Print(ERROR_MODE, name, buffer);
}

void LoggerServer::PrintHttpLog(std::ostream& os, const HTTP_LOG& http_log, bool global)
{
	for(HTTP_LOG::const_iterator it = http_log.begin(); it != http_log.end(); it++)
	{
		std::string s;
		
		const HTTP_LOG_ENTRY& http_log_entry = *it;
		mode_t mode = http_log_entry.GetMode();
		const std::string& msg = http_log_entry.GetMessage();
		os << "\t\t\t<span";
		switch(mode)
		{
			case INFO_MODE   : os << " class=\"info\""; break;
			case WARNING_MODE: os << " class=\"warning\""; break;
			case ERROR_MODE  : os << " class=\"error\""; break;
			default          : break;
		}
		
		os << ">";
		if(global)
		{
			std::string s;
			
			const std::string& name = http_log_entry.GetName();
			s.append(name);
			s.append(1, ':');
			s.append(1, ' ');
			switch(mode)
			{
				case WARNING_MODE: s.append("WARNING! "); break;
				case ERROR_MODE  : s.append("ERROR! "); break;
				default          : break;
			}
			std::size_t prefix_length = name.length();
			std::string prefix(prefix_length, ' ');
			std::size_t base_pos = 0;
			for(std::size_t pos = msg.find_first_of('\n'); pos != std::string::npos; pos = msg.find_first_of('\n', base_pos))
			{
				s.append(msg, base_pos, pos - base_pos);
				s.append(1, '\n');
				s.append(prefix);
				base_pos = pos + 1;
			}
			
			s.append(msg, base_pos, std::string::npos);
			os << unisim::util::hypapp::HTML_Encoder::Encode(s);
		}
		else
		{
			switch(mode)
			{
				case WARNING_MODE: os << "WARNING!&nbsp;"; break;
				case ERROR_MODE  : os << "ERROR!&nbsp;"; break;
				default          : break;
			}
			os << unisim::util::hypapp::HTML_Encoder::Encode(msg) << "<br>";
		}
		
		os << "</span>" << std::endl;
	}
}

bool LoggerServer::ServeHttpRequest(unisim::util::hypapp::HttpRequest const& req, unisim::util::hypapp::ClientConnection const& conn)
{
	unisim::util::hypapp::HttpResponse response;
	
	if(req.GetPath() == "")
	{
		switch(req.GetRequestType())
		{
			case unisim::util::hypapp::Request::OPTIONS:
				response.Allow("OPTIONS, GET, HEAD");
				break;
				
			case unisim::util::hypapp::Request::GET:
			case unisim::util::hypapp::Request::HEAD:
			{
				struct QueryDecoder : public unisim::util::hypapp::Form_URL_Encoded_Decoder
				{
					QueryDecoder()
						: object_name()
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
					
					std::string object_name;
				};

				unisim::kernel::Object *object = 0;
				
				if(req.HasQuery())
				{
					QueryDecoder query_decoder;
				
					if(query_decoder.Decode(req.GetQuery(), std::cerr))
					{
						object = GetSimulator()->FindObject(query_decoder.object_name.c_str());
					}
				}
				
				response << "<!DOCTYPE html>" << std::endl;
				response << "<html>" << std::endl;
				response << "\t<head>" << std::endl;
				if(object)
				{
					response << "\t\t<title>Log of " << unisim::util::hypapp::HTML_Encoder::Encode(object->GetName()) << "</title>" << std::endl;
				}
				else
				{
					response << "\t\t<title>Log</title>" << std::endl;
				}
				response << "\t\t<meta name=\"description\" content=\"user interface for logs over HTTP\">" << std::endl;
				response << "\t\t<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">" << std::endl;
				response << "\t\t<link rel=\"stylesheet\" href=\"/unisim/kernel/logger/style.css\" type=\"text/css\" />" << std::endl;
				response << "\t\t<script type=\"application/javascript\">document.domain='" << req.GetDomain() << "';</script>" << std::endl;
				response << "\t\t<script type=\"application/javascript\" src=\"/unisim/service/http_server/uri.js\"></script>" << std::endl;
				response << "\t\t<script type=\"application/javascript\" src=\"/unisim/service/http_server/embedded_script.js\"></script>" << std::endl;
				//response << "\t\t<script type=\"application/javascript\" src=\"/unisim/kernel/logger/script.js\"></script>" << std::endl;
				response << "\t</head>" << std::endl;
				response << "\t<body>" << std::endl;
				response << "\t\t<div class=\"log\">" << std::endl;
				
				if(req.HasQuery())
				{
					if(object)
					{
						pthread_mutex_lock(&mutex);
						HTTP_LOGS_PER_CLIENT::const_iterator http_log_it = http_logs_per_client.find(object->GetName());
						if(http_log_it != http_logs_per_client.end())
						{
							HTTP_LOG *http_log = (*http_log_it).second;
							PrintHttpLog(response, *http_log, false);
						}
						pthread_mutex_unlock(&mutex);
					}
				}
				else
				{
					pthread_mutex_lock(&mutex);
					PrintHttpLog(response, global_http_log, true);
					pthread_mutex_unlock(&mutex);
				}

				response << "\t\t</div>" << std::endl;
				response << "\t</body>" << std::endl;
				response << "</html>" << std::endl;
				
				break;
			}
			
			default:
				response.SetStatus(unisim::util::hypapp::HttpResponse::METHOD_NOT_ALLOWED);
				response.Allow("OPTIONS, GET, HEAD");
				
				response << "<!DOCTYPE html>" << std::endl;
				response << "<html>" << std::endl;
				response << "\t<head>" << std::endl;
				response << "\t\t<title>Error 405 (Method Not Allowed)</title>" << std::endl;
				response << "\t\t<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">" << std::endl;
				response << "\t\t<meta name=\"description\" content=\"Error 405 (Method Not Allowed)\">" << std::endl;
				response << "\t\t<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">" << std::endl;
				response << "\t\t<script type=\"application/javascript\">document.domain='" << req.GetDomain() << "';</script>" << std::endl;
				response << "\t\t<style>" << std::endl;
				response << "\t\t\tbody { font-family:Arial,Helvetica,sans-serif; font-style:normal; font-size:14px; text-align:left; font-weight:400; color:black; background-color:white; }" << std::endl;
				response << "\t\t</style>" << std::endl;
				response << "\t</head>" << std::endl;
				response << "\t<body>" << std::endl;
				response << "\t\t<p>HTTP Method not allowed</p>" << std::endl;
				response << "\t</body>" << std::endl;
				response << "</html>" << std::endl;
				break;
		}
	}
	else
	{
		response.SetStatus(unisim::util::hypapp::HttpResponse::NOT_FOUND);
		
		response << "<!DOCTYPE html>" << std::endl;
		response << "<html>" << std::endl;
		response << "\t<head>" << std::endl;
		response << "\t\t<title>Error 404 (Not Found)</title>" << std::endl;
		response << "\t\t<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">" << std::endl;
		response << "\t\t<meta name=\"description\" content=\"Error 404 (Not Found)\">" << std::endl;
		response << "\t\t<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">" << std::endl;
		response << "\t\t<script type=\"application/javascript\">document.domain='" << req.GetDomain() << "';</script>" << std::endl;
		response << "\t\t<style>" << std::endl;
		response << "\t\t\tbody { font-family:Arial,Helvetica,sans-serif; font-style:normal; font-size:14px; text-align:left; font-weight:400; color:black; background-color:white; }" << std::endl;
		response << "\t\t</style>" << std::endl;
		response << "\t</head>" << std::endl;
		response << "\t<body>" << std::endl;
		response << "\t\t<p>Unavailable</p>" << std::endl;
		response << "\t</body>" << std::endl;
		response << "</html>" << std::endl;
	}
	
	return conn.Send(response.ToString((req.GetRequestType() == unisim::util::hypapp::Request::HEAD) || (req.GetRequestType() == unisim::util::hypapp::Request::OPTIONS)));
}

void LoggerServer::ScanWebInterfaceModdings(unisim::service::interfaces::WebInterfaceModdingScanner& scanner)
{
	scanner.Append(unisim::service::interfaces::ToolbarOpenTabAction(
		/* name */      GetName(), 
		/* label */     "<img src=\"/unisim/kernel/logger/icon.svg\">",
		/* tips */      "Log",
		/* tile */      unisim::service::interfaces::OpenTabAction::BOTTOM_TILE,
		/* uri */       URI()
	));

	std::set<Logger const*>::const_iterator it;
	for(it = clients.begin(); it != clients.end(); it++)
	{
		Logger const *client = *it;
		
		scanner.Append(unisim::service::interfaces::BrowserOpenTabAction(
			/* name        */ std::string(this->GetName()) + "-" + client->GetName(),
			/* object name */ client->GetName(),
			/* label       */ "Show log",
			/* tile        */ unisim::service::interfaces::OpenTabAction::BOTTOM_TILE,
			/* uri         */ URI() + std::string("?object=") + unisim::util::hypapp::URI_Encoder::EncodeComponent(client->GetName())
		));
	}
}

} // end of namespace logger
} // end of namespace kernel
} // end of namespace unisim

