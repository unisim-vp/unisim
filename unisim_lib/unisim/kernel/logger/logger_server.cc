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
#include <unisim/kernel/service/service.hh>

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

LoggerServer::LoggerServer(const char *name, unisim::kernel::service::Object *parent)
  : unisim::kernel::service::Object(name, parent)
  , xml_writer_(0)
  , opt_std_err_(true)
  , opt_std_out_(false)
  , opt_std_err_color_(false)
  , opt_std_out_color_(false)
  , opt_file_(false)
  , opt_filename_("logger_output.txt")
  , opt_xml_file_(false)
  , opt_xml_filename_("logger_output.xml")
  , opt_xml_file_gzipped_(false)
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
{
	pthread_mutex_init(&mutex, NULL);
}

LoggerServer::~LoggerServer()
{
  Close();
  pthread_mutex_destroy(&mutex);
}

void
LoggerServer::Close()
{
  if (clients.size()) {
    std::cerr << "Error(LoggerServer::close): "
              << "client loggers still connected" << std::endl;
  }
  
  if (xml_writer_ != NULL) {
    int rc = xmlTextWriterEndElement(xml_writer_);
    if (rc < 0) {
      std::cerr << "Error(LoggerServer): "
                << "could not close the root element of xml output" << std::endl;
    }
    rc = xmlTextWriterEndDocument(xml_writer_);
    if (rc < 0) {
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

bool
LoggerServer::Setup() 
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

void
LoggerServer::AddClient( Logger const* client )
{
  if (not clients.insert( client ).second) {
    std::cerr << "Error(LoggerServer::AddClient): "
              << "internal issue, client already connected" << std::endl;
  }
}

void
LoggerServer::RemoveClient( Logger const* client )
{
  if (not clients.erase( client )) {
    std::cerr << "Error(LoggerServer::AddClient): "
              << "internal issue, attempting to disconnect a unknown client" << std::endl;
  }
}

void
LoggerServer::XmlDebug(const char *type, std::string name, const char *buffer)
{
  int rc;
  rc = xmlTextWriterStartElement(xml_writer_, BAD_CAST "DEBUG");
  if (rc < 0)
    std::cerr << "Error(LoggerServer): could not add a debug message of type \"" << type << "\"" << std::endl;
  xmlChar *xml_type = xmlCharStrdup(type);
  rc = xmlTextWriterWriteAttribute(xml_writer_, BAD_CAST "type", xml_type);
  free(xml_type);
  if(rc < 0)
    std::cerr << "Error(LoggerServer): could not add \"type\" attribute to debug message of type \"" << type << "\"" << std::endl;
  xmlChar *xml_obj_name = xmlCharStrdup(name.c_str());
  rc = xmlTextWriterWriteAttribute(xml_writer_, BAD_CAST "source", xml_obj_name);
  free(xml_obj_name);
  if (rc < 0)
    std::cerr << "Error(LoggerServer): could not add \"source\" attribute to debug message of type \"" << type << "\"" << std::endl;
  rc = xmlTextWriterWriteFormatString(xml_writer_, "%s", buffer);
  if (rc < 0) 
    std::cerr << "Error(LoggerServer): could not attach message buffer to debug message of type \"" << type << "\" buffer = \"" << buffer << "\"" << std::endl;
  rc = xmlTextWriterEndElement(xml_writer_);
  if (rc < 0)
    std::cerr << "Error(LoggerServer): could not close debug message of type \"" << type << "\"" << std::endl;
}

void LoggerServer::Print(std::ostream& os, bool opt_color, mode_t mode, std::string name, const char *buffer)
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
	int prefix_length = name.size() + 2;
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

void LoggerServer::Print(mode_t mode, std::string name, const char *buffer)
{
	pthread_mutex_lock(&mutex);
	
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

void LoggerServer::DebugInfo(std::string name, const char *buffer)
{
	Print(INFO_MODE, name, buffer);
}

void LoggerServer::DebugWarning(std::string name, const char *buffer)
{
	Print(WARNING_MODE, name, buffer);
}

void LoggerServer::DebugError(std::string name, const char *buffer)
{
	Print(ERROR_MODE, name, buffer);
}

} // end of namespace logger
} // end of namespace kernel
} // end of namespace unisim

