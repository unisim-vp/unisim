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
#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>
#include <sstream>

namespace unisim {
namespace kernel {
namespace logger {

static const char *XML_ENCODING = "UTF-8"; 

LoggerServer *LoggerServer::singleton = 0;
unsigned long long int LoggerServer::singleton_refs = 0;
const char *LoggerServer::name = "kernel_logger";
std::vector<const unisim::kernel::service::Object *> *LoggerServer::obj_refs = 0;

LoggerServer::
LoggerServer() :
	xml_writer(0),
	opt_std_err(true),
	param_std_err("kernel_logger.std_err", 0, opt_std_err, 
			"Show logger output through the standard error output"),
	opt_std_out(false),
	param_std_out("kernel_logger.std_out", 0, opt_std_out, 
			"Show logger output through the standard output"),
	opt_std_err_color(false),
	param_std_err_color("kernel_logger.std_err_color", 0, opt_std_err_color, 
			"Colorize logger output through the standard error output (only works if std_err is active)"),
	opt_std_out_color(false),
	param_std_out_color("kernel_logger.std_out_color", 0, opt_std_out_color, 
			"Colorize logger output through the standard output (only works if std_out is active)"),
	opt_file(false),
	param_file("kernel_logger.file", 0, opt_file, 
			"Keep logger output in a file"),
	opt_filename("logger_output.txt"),
	param_filename("kernel_logger.filename", 0, opt_filename, 
			"Filename to keep logger output (the option file must be activated)"),
	opt_xml_file(false),
	param_xml_file("kernel_logger.xml_file", 0, opt_xml_file, 
			"Keep logger output in a file xml formatted"),
	opt_xml_filename("logger_output.xml"),
	param_xml_filename("kernel_logger.xml_filename", 0, opt_xml_filename, 
			"Filename to keep logger xml output (the option xml_file must be activated)"),
	opt_xml_file_gzipped(false),
	param_xml_file_gzipped("kernel_logger.xml_file_gzipped", 0, opt_xml_file_gzipped, 
			"If the xml_file option is active, the output file will be compressed (a .gz extension will be automatically added to the xml_filename option") 
{
}

LoggerServer::
~LoggerServer() {
	if(xml_writer != NULL) {
		int rc = xmlTextWriterEndElement(xml_writer);
		if(rc < 0) {
			cerr << "Error(LoggerServer): "
				<< "could not close the root element of xml output" << endl;
		}
		rc = xmlTextWriterEndDocument(xml_writer);
		if(rc < 0) {
			cerr << "Warning(LoggerServer::OnDisconnect): "
				<< "could not correctly close the xml output file" << endl;
		}
		xmlFreeTextWriter(xml_writer);
		xml_writer = NULL;
	}
	if (text_file.is_open())
	{
		text_file.close();
		if (text_file.is_open())
		{
			cerr << "Warning(LoggerServer::OnDisconnect): "
				<< "could not correctly close the text output file" << endl;
		}
	}
}

bool
LoggerServer::
Setup() 
{
	/* check if a xml output needs to be created */
	if (opt_xml_file) 
	{
		/* create and initialize the xml output */
		std::stringstream xml_filename;
		xml_filename << opt_xml_filename;
		if (opt_xml_file_gzipped) xml_filename << ".gz";
		xml_writer = xmlNewTextWriterFilename(xml_filename.str().c_str(), opt_xml_file_gzipped ? 1 : 0);
		if (xml_writer == NULL) 
		{
			cerr << "Error(LoggerServer::Setup): "
				<< "could not open xml output file for logging" << endl;
			return false;
		}
		int rc = xmlTextWriterSetIndent(xml_writer, 1);
		if (rc < 0) 
		{
			cerr << "Warning(LoggerServer::Setup): "
				<< "could not set indentation" << endl;
		}
		rc = xmlTextWriterStartDocument(xml_writer, NULL, XML_ENCODING, NULL);
		if (rc < 0) 
		{
			cerr << "Error(LoggerServer::Setup): "
				<< "error starting the xml document" << endl;
			return false;
		}
		rc = xmlTextWriterStartElement(xml_writer, BAD_CAST "LOGGER");
		if (rc < 0) 
		{
			cerr << "Error(LoggerServer::Setup): "
				<< "error starting the xml document" << endl;
			return false;
		}
	}
	/* check if a file text output needs to be created */
	if (opt_file)
	{
		/* create and initialize the text output file */
		std::stringstream filename;
		filename << opt_filename;
		text_file.open(filename.str().c_str(), std::ios::trunc);
		if (!text_file.is_open())
		{
			cerr << "Error(LoggerServer::Setup): "
				<< "could not open text output file for logging" << endl;
			return false;
		}
	}
	return true;
}

void
LoggerServer::
OnDisconnect() {
	if (xml_writer != NULL) 
	{
		int rc = xmlTextWriterEndElement(xml_writer);
		if (rc < 0)
		{
			cerr << "Error(LoggerServer): "
				<< "could not close the root element of xml output" << endl;
		}
		rc = xmlTextWriterEndDocument(xml_writer);
		if (rc < 0)
		{
			cerr << "Warning(LoggerServer::OnDisconnect): "
				<< "could not correctly close the xml output file" << endl;
		}
		xmlFreeTextWriter(xml_writer);
		xml_writer = NULL;
	}
	if (text_file.is_open())
	{
		text_file.close();
		if (text_file.is_open())
		{
			cerr << "Warning(LoggerServer::OnDisconnect): "
				<< "could not correctly close the text output file" << endl;
		}
	}
}

LoggerServer *
LoggerServer::
GetInstanceWithoutCountingReference()
{
	if (singleton == 0)
	{
		singleton = new LoggerServer();
		obj_refs = new std::vector<const unisim::kernel::service::Object *>();
	}
	return singleton;
}

LoggerServer *
LoggerServer::
GetInstance(const unisim::kernel::service::Object &obj) {
	if (singleton == 0) 
	{
		singleton = new LoggerServer();
		obj_refs = new std::vector<const unisim::kernel::service::Object *>();
	}
	singleton_refs++;
	obj_refs->push_back(&obj);
	return singleton;
}

void
LoggerServer::
RemoveInstance(const unisim::kernel::service::Object &obj) {
	singleton_refs--;
	vector<const unisim::kernel::service::Object *>::iterator it;
	for (it = obj_refs->begin(); it != obj_refs->end(); it++)
	{
		if (*it == &obj)
		{
			obj_refs->erase(it);
			break;
		}
	}
//	cerr << "++++ Removing LoggerServer instance for " << obj.GetName() << ". " << singleton_refs << " refs remaining:" << endl;
//	for (it = obj_refs->begin(); it != obj_refs->end(); it++)
//		cerr << " - " << (*it)->GetName() << endl;
	if(singleton_refs == 0 && singleton != 0) {
		singleton->OnDisconnect();
		delete singleton;
		delete obj_refs;
		singleton = 0;
		obj_refs = 0;
	}
}

const char * 
LoggerServer::
GetObjectName() {
	return name;
}

void 
LoggerServer::
XmlDebug(const char *type, const unisim::kernel::service::Object &obj, const char *buffer) {
	int rc;
	rc = xmlTextWriterStartElement(xml_writer, BAD_CAST "DEBUG");
	if(rc < 0)
		cerr << "Error(LoggerServer): could not add a debug message of type \"" << type << "\"" << endl;
	xmlChar *xml_type = xmlCharStrdup(type);
	rc = xmlTextWriterWriteAttribute(xml_writer, BAD_CAST "type", xml_type);
	free(xml_type);
	if(rc < 0)
		cerr << "Error(LoggerServer): could not add \"type\" attribute to debug message of type \"" << type << "\"" << endl;
	xmlChar *xml_obj_name = xmlCharStrdup(obj.GetName());
	rc = xmlTextWriterWriteAttribute(xml_writer, BAD_CAST "source", xml_obj_name);
	free(xml_obj_name);
	if(rc < 0)
		cerr << "Error(LoggerServer): could not add \"source\" attribute to debug message of type \"" << type << "\"" << endl;
	rc = xmlTextWriterWriteFormatString(xml_writer, "%s", buffer);
	if(rc < 0) 
		cerr << "Error(LoggerServer): could not attach message buffer to debug message of type \"" << type << "\" buffer = \"" << buffer << "\"" << endl;
	rc = xmlTextWriterEndElement(xml_writer);
	if(rc < 0)
		cerr << "Error(LoggerServer): could not close debug message of type \"" << type << "\"" << endl;
}

void
LoggerServer::
DebugInfo(const unisim::kernel::service::Object &obj, const char *buffer) {
	if (opt_std_out) 
	{
		if ( opt_std_out_color ) std::cout << "\033[36m";

		std::cout << obj.GetName() << ": ";
		int prefix_length = strlen(obj.GetName()) + 2;
		std::string prefix(prefix_length, ' ');
		const char *b = buffer;
		for ( const char *p = strchr(b, '\n');
				p != NULL;
				p = strchr(b, '\n') )
		{
			std::cout.write(b, p - b);
			std::cout << std::endl << prefix;
			b = p + 1;
		}
		std::cout << b << std::endl;
		
		if ( opt_std_out_color ) std::cout << "\033[0m";
	}
	if ( opt_std_err )
	{
		if ( opt_std_err_color ) std::cerr << "\033[36m";

		std::cerr << obj.GetName() << ": ";
		int prefix_length = strlen(obj.GetName()) + 2;
		std::string prefix(prefix_length, ' ');
		const char *b = buffer;
		for ( const char *p = strchr(b, '\n');
				p != NULL;
				p = strchr(b, '\n') )
		{
			std::cerr.write(b, p - b);
			std::cerr << std::endl << prefix;
			b = p + 1;
		}
		std::cerr << b << std::endl;
		
		if ( opt_std_err_color ) std::cerr << "\033[0m";
	}
	if (opt_xml_file) 
	{
		XmlDebug("info", obj, buffer);
	}
	if (opt_file)
	{
		text_file << obj.GetName() << ": " << buffer << endl;
	}
}

void 
LoggerServer::
DebugWarning(const unisim::kernel::service::Object &obj, const char *buffer) {
	if (opt_std_out) 
	{
		if ( opt_std_out_color ) std::cout << "\033[33m";

		std::cout << obj.GetName() << ": WARNING! ";
		int prefix_length = strlen(obj.GetName()) + 2;
		std::string prefix(prefix_length, ' ');
		const char *b = buffer;
		for ( const char *p = strchr(b, '\n');
				p != NULL;
				p = strchr(b, '\n') )
		{
			std::cout.write(b, p - b);
			std::cout << std::endl << prefix;
			b = p + 1;
		}
		std::cout << b << std::endl;
		
		if ( opt_std_out_color ) std::cout << "\033[0m";
	}
	if ( opt_std_err )
	{
		if ( opt_std_err_color ) std::cerr << "\033[33m";

		std::cerr << obj.GetName() << ": WARNING! ";
		int prefix_length = strlen(obj.GetName()) + 2;
		std::string prefix(prefix_length, ' ');
		const char *b = buffer;
		for ( const char *p = strchr(b, '\n');
				p != NULL;
				p = strchr(b, '\n') )
		{
			std::cerr.write(b, p - b);
			std::cerr << std::endl << prefix;
			b = p + 1;
		}
		std::cerr << b << std::endl;
		
		if ( opt_std_err_color ) std::cerr << "\033[0m";
	}
	if (opt_xml_file)
	{
		XmlDebug("warning", obj, buffer);
	}
	if (opt_file)
	{
		text_file << obj.GetName() << ": WARNING! " << buffer << endl;
	}
}

void 
LoggerServer::
DebugError(const unisim::kernel::service::Object &obj, const char *buffer) {
	if (opt_std_out) 
	{
		if ( opt_std_out_color ) std::cout << "\033[31m";

		std::cout << obj.GetName() << ": ERROR! ";
		int prefix_length = strlen(obj.GetName()) + 2;
		std::string prefix(prefix_length, ' ');
		const char *b = buffer;
		for ( const char *p = strchr(b, '\n');
				p != NULL;
				p = strchr(b, '\n') )
		{
			std::cout.write(b, p - b);
			std::cout << std::endl << prefix;
			b = p + 1;
		}
		std::cout << b << std::endl;
		
		if ( opt_std_out_color ) std::cout << "\033[0m";
	}
	if ( opt_std_err )
	{
		if ( opt_std_err_color ) std::cerr << "\033[31m";

		std::cerr << obj.GetName() << ": ERROR! ";
		int prefix_length = strlen(obj.GetName()) + 2;
		std::string prefix(prefix_length, ' ');
		const char *b = buffer;
		for ( const char *p = strchr(b, '\n');
				p != NULL;
				p = strchr(b, '\n') )
		{
			std::cerr.write(b, p - b);
			std::cerr << std::endl << prefix;
			b = p + 1;
		}
		std::cerr << b << std::endl;
		
		if ( opt_std_err_color ) std::cerr << "\033[0m";
	}
	if (opt_xml_file) 
	{
		XmlDebug("error", obj, buffer);
	}
	if (opt_file)
	{
		text_file << obj.GetName() << ": ERROR! " << buffer << endl;
	}
}

} // end of namespace logger
} // end of namespace kernel
} // end of namespace unisim

