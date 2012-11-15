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

#include "unisim/kernel/logger/logger_server.hh"

#include <sstream>
#include <string>
#include <iostream>
#include <ios>

#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>

#include "unisim/kernel/service/service.hh"

namespace unisim {
namespace kernel {
namespace logger {

static const char *XML_ENCODING = "UTF-8"; 

LoggerServer *LoggerServer::singleton_ = 0;
unsigned long long int LoggerServer::singleton_refs_ = 0;
const char *LoggerServer::name_ = "kernel_logger";
std::vector<const unisim::kernel::service::Object *> *LoggerServer::obj_refs_ = 0;

LoggerServer::
LoggerServer() :
	xml_writer_(0),
	opt_std_err_(true),
	param_std_err_("kernel_logger.std_err", 0, opt_std_err_,
			"Show logger output through the standard error output"),
	opt_std_out_(false),
	param_std_out_("kernel_logger.std_out", 0, opt_std_out_,
			"Show logger output through the standard output"),
	opt_std_err_color_(false),
	param_std_err_color_("kernel_logger.std_err_color", 0, opt_std_err_color_,
			"Colorize logger output through the standard error output _(only works if std_err is active)"),
	opt_std_out_color_(false),
	param_std_out_color_("kernel_logger.std_out_color", 0, opt_std_out_color_,
			"Colorize logger output through the standard output _(only works if std_out is active)"),
	opt_file_(false),
	param_file_("kernel_logger.file", 0, opt_file_,
			"Keep logger output in a file"),
	opt_filename_("logger_output.txt"),
	param_filename_("kernel_logger.filename", 0, opt_filename_,
			"Filename to keep logger output _(the option file must be activated)"),
	opt_xml_file_(false),
	param_xml_file_("kernel_logger.xml_file", 0, opt_xml_file_,
			"Keep logger output in a file xml formatted"),
	opt_xml_filename_("logger_output.xml"),
	param_xml_filename_("kernel_logger.xml_filename", 0, opt_xml_filename_,
			"Filename to keep logger xml output _(the option xml_file must be activated)"),
	opt_xml_file_gzipped_(false),
	param_xml_file_gzipped_("kernel_logger.xml_file_gzipped", 0, opt_xml_file_gzipped_,
			"If the xml_file option is active, the output file will be compressed (a .gz extension will be automatically added to the xml_filename option") 
{
}

LoggerServer::~LoggerServer() {
	if (xml_writer_ != NULL) {
		int rc = xmlTextWriterEndElement(xml_writer_);
		if (rc < 0) {
      std::cerr << "Error(LoggerServer): "
				<< "could not close the root element of xml output" << std::endl;
		}
		rc = xmlTextWriterEndDocument(xml_writer_);
		if (rc < 0) {
      std::cerr << "Warning(LoggerServer::OnDisconnect): "
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
      std::cerr << "Warning(LoggerServer::OnDisconnect): "
				<< "could not correctly close the text output file" << std::endl;
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

void LoggerServer::OnDisconnect() {
}

LoggerServer* LoggerServer::GetInstanceWithoutCountingReference()
{
	if (singleton_ == 0)
	{
		singleton_ = new LoggerServer();
		obj_refs_ = new std::vector<const unisim::kernel::service::Object *>();
	}
	return singleton_;
}

LoggerServer* LoggerServer::GetInstance(const unisim::kernel::service::Object &obj) {
	if (singleton_ == 0) 
	{
		singleton_ = new LoggerServer();
		obj_refs_ = new std::vector<const unisim::kernel::service::Object *>();
	}
	singleton_refs_++;
	obj_refs_->push_back(&obj);
	return singleton_;
}

void LoggerServer::RemoveInstance(const unisim::kernel::service::Object &obj) {
	singleton_refs_--;
  std::vector<const unisim::kernel::service::Object *>::iterator it;
	for (it = obj_refs_->begin(); it != obj_refs_->end(); it++)
	{
		if (*it == &obj)
		{
			obj_refs_->erase(it);
			break;
		}
	}
	if ((singleton_refs_ == 0) && (singleton_ != 0)) {
		singleton_->OnDisconnect();
		delete singleton_;
		delete obj_refs_;
		singleton_ = 0;
		obj_refs_ = 0;
	}
}

const char* LoggerServer::GetObjectName() {
	return name_;
}

void LoggerServer::XmlDebug(const char *type, const unisim::kernel::service::Object &obj, const char *buffer) {
	int rc;
	rc = xmlTextWriterStartElement(xml_writer_, BAD_CAST "DEBUG");
	if (rc < 0)
		std::cerr << "Error(LoggerServer): could not add a debug message of type \"" << type << "\"" << std::endl;
	xmlChar *xml_type = xmlCharStrdup(type);
	rc = xmlTextWriterWriteAttribute(xml_writer_, BAD_CAST "type", xml_type);
	free(xml_type);
	if(rc < 0)
		std::cerr << "Error(LoggerServer): could not add \"type\" attribute to debug message of type \"" << type << "\"" << std::endl;
	xmlChar *xml_obj_name = xmlCharStrdup(obj.GetName());
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

void LoggerServer::DebugInfo(const unisim::kernel::service::Object &obj, const char *buffer) {
	if (opt_std_out_) {
		if (opt_std_out_color_) std::cout << "\033[36m";

		std::cout << obj.GetName() << ": ";
		int prefix_length = strlen(obj.GetName()) + 2;
		std::string prefix(prefix_length, ' ');
		const char *b = buffer;
		for (const char *p = strchr(b, '\n');
				p != NULL;
				p = strchr(b, '\n')) {
			std::cout.write(b, p - b);
			std::cout << std::endl << prefix;
			b = p + 1;
		}
		std::cout << b << std::endl;
		
		if (opt_std_out_color_) std::cout << "\033[0m";
	}
	if (opt_std_err_) {
		if (opt_std_err_color_) std::cerr << "\033[36m";

		std::cerr << obj.GetName() << ": ";
		int prefix_length = strlen(obj.GetName()) + 2;
		std::string prefix(prefix_length, ' ');
		const char *b = buffer;
		for ( const char *p = strchr(b, '\n');
				p != NULL;
				p = strchr(b, '\n') ) {
			std::cerr.write(b, p - b);
			std::cerr << std::endl << prefix;
			b = p + 1;
		}
		std::cerr << b << std::endl;
		
		if (opt_std_err_color_) std::cerr << "\033[0m";
	}
	if (opt_xml_file_) {
		XmlDebug("info", obj, buffer);
	}
	if (opt_file_) {
		text_file_ << obj.GetName() << ": " << buffer << std::endl;
	}
}

void LoggerServer::DebugWarning(const unisim::kernel::service::Object &obj, const char *buffer) {
	if (opt_std_out_) {
		if (opt_std_out_color_) std::cout << "\033[33m";

		std::cout << obj.GetName() << ": WARNING! ";
		int prefix_length = strlen(obj.GetName()) + 2;
		std::string prefix(prefix_length, ' ');
		const char *b = buffer;
		for (const char *p = strchr(b, '\n');
				p != NULL;
				p = strchr(b, '\n')) {
			std::cout.write(b, p - b);
			std::cout << std::endl << prefix;
			b = p + 1;
		}
		std::cout << b << std::endl;
		
		if (opt_std_out_color_) std::cout << "\033[0m";
	}
	if (opt_std_err_) {
		if (opt_std_err_color_) std::cerr << "\033[33m";

		std::cerr << obj.GetName() << ": WARNING! ";
		int prefix_length = strlen(obj.GetName()) + 2;
		std::string prefix(prefix_length, ' ');
		const char *b = buffer;
		for (const char *p = strchr(b, '\n');
				p != NULL;
				p = strchr(b, '\n')) {
			std::cerr.write(b, p - b);
			std::cerr << std::endl << prefix;
			b = p + 1;
		}
		std::cerr << b << std::endl;
		
		if (opt_std_err_color_) std::cerr << "\033[0m";
	}
	if (opt_xml_file_) {
		XmlDebug("warning", obj, buffer);
	}
	if (opt_file_) {
		text_file_ << obj.GetName() << ": WARNING! " << buffer << std::endl;
	}
}

void LoggerServer::DebugError(const unisim::kernel::service::Object &obj, const char *buffer) {
	if (opt_std_out_) {
		if (opt_std_out_color_) std::cout << "\033[31m";

		std::cout << obj.GetName() << ": ERROR! ";
		int prefix_length = strlen(obj.GetName()) + 2;
		std::string prefix(prefix_length, ' ');
		const char *b = buffer;
		for (const char *p = strchr(b, '\n');
				p != NULL;
				p = strchr(b, '\n')) {
			std::cout.write(b, p - b);
			std::cout << std::endl << prefix;
			b = p + 1;
		}
		std::cout << b << std::endl;
		
		if (opt_std_out_color_) std::cout << "\033[0m";
	}
	if (opt_std_err_) {
		if (opt_std_err_color_) std::cerr << "\033[31m";

		std::cerr << obj.GetName() << ": ERROR! ";
		int prefix_length = strlen(obj.GetName()) + 2;
		std::string prefix(prefix_length, ' ');
		const char *b = buffer;
		for (const char *p = strchr(b, '\n');
				p != NULL;
				p = strchr(b, '\n')) {
			std::cerr.write(b, p - b);
			std::cerr << std::endl << prefix;
			b = p + 1;
		}
		std::cerr << b << std::endl;
		
		if (opt_std_err_color_) std::cerr << "\033[0m";
	}
	if (opt_xml_file_) {
		XmlDebug("error", obj, buffer);
	}
	if (opt_file_) {
		text_file_ << obj.GetName() << ": ERROR! " << buffer << std::endl;
	}
}

} // end of namespace logger
} // end of namespace kernel
} // end of namespace unisim

