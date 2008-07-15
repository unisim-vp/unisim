#include "unisim/kernel/logger/logger_server.hh"
#include <sstream>

namespace unisim {
namespace kernel {
namespace logger {

static const char *XML_ENCODING = "UTF-8"; 

LoggerServer *LoggerServer::singleton = 0;
unsigned long long int LoggerServer::singleton_refs = 0;
const char *LoggerServer::name = "kernel_logger";

LoggerServer::
LoggerServer() :
	Object(LoggerServer::name, 0),
	xml_writer(0),
	opt_std_err(false),
	opt_std_out(false),
	opt_std_err_color(false),
	opt_std_out_color(false),
	opt_file(false),
	opt_filename("logger_output.txt"),
	opt_xml_file(false),
	opt_xml_filename("logger_output.xml"),
	opt_xml_file_gzipped(false),
	param_std_err("std_err", this, opt_std_err, "Show logger output through the standard error output"),
	param_std_out("std_out", this, opt_std_out, "Show logger output through the standard output"),
	param_std_err_color("std_err_color", this, opt_std_err_color, "Colorize logger output through the standard error output (only works if std_err is active)"),
	param_std_out_color("std_out_color", this, opt_std_out_color, "Colorize logger output through the standard output (only works if std_out is active)"),
	param_file("file", this, opt_file, "Keep logger output in a file"),
	param_filename("filename", this, opt_filename, "Filename to keep logger output (the option file must be activated)"),
	param_xml_file("xml_file", this, opt_xml_file, "Keep logger output in a file xml formatted"),
	param_xml_filename("xml_filename", this, opt_xml_filename, "Filename to keep logger xml output (the option xml_file must be activated)"),
	param_xml_file_gzipped("xml_file_gzipped", this, opt_xml_file_gzipped, "If the xml_file option is active, the output file will be compressed (a .gz extension will be automatically added to the xml_filename option") {
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
	}
}

bool
LoggerServer::
Setup() {
	/* check if a xml output needs to be created */
	if(opt_xml_file) {
		/* create and initialize the xml output */
		std::stringstream filename;
		filename << opt_xml_filename;
		if(opt_xml_file_gzipped) filename << ".gz";
		xml_writer = xmlNewTextWriterFilename(filename.str().c_str(), opt_xml_file_gzipped ? 1 : 0);
		if(xml_writer == NULL) {
			cerr << "Error(LoggerServer::Setup): "
				<< "could not open output file for logging" << endl;
			return false;
		}
		int rc = xmlTextWriterSetIndent(xml_writer, 1);
		if(rc < 0) {
			cerr << "Warning(LoggerServer::Setup): "
				<< "could not set indentation" << endl;
		}
		rc = xmlTextWriterStartDocument(xml_writer, NULL, XML_ENCODING, NULL);
		if(rc < 0) {
			cerr << "Error(LoggerServer::Setup): "
				<< "error starting the xml document" << endl;
			return false;
		}
		rc = xmlTextWriterStartElement(xml_writer, BAD_CAST "LOGGER");
		if(rc < 0) {
			cerr << "Error(LoggerServer::Setup): "
				<< "error starting the xml document" << endl;
			return false;
		}
	}
	return true;
}

void
LoggerServer::
OnDisconnect() {
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
	}
}

LoggerServer *
LoggerServer::
GetInstance() {
	if(singleton == 0) {
		singleton = new LoggerServer();
	}
	singleton_refs++;
	return singleton;
}

void
LoggerServer::
RemoveInstance() {
	singleton_refs--;
	if(singleton_refs == 0 && singleton != 0) {
		delete singleton;
		singleton = 0;
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
	rc = xmlTextWriterWriteAttribute(xml_writer, BAD_CAST "type", xmlCharStrdup(type));
	if(rc < 0)
		cerr << "Error(LoggerServer): could not add \"type\" attribute to debug message of type \"" << type << "\"" << endl;
	rc = xmlTextWriterWriteAttribute(xml_writer, BAD_CAST "source", xmlCharStrdup(obj.GetName()));
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
	if(opt_std_out) {
		if(opt_std_out_color) {
			std::cout << "\033[36m";
 			std::cout << obj.GetName() << ":" << endl << buffer << endl;
			std::cout << "\033[0m";
		} else {
			std::cout << "DebugInfo(" << obj.GetName() << "):" << endl << buffer << endl;
		}
	}
	if(opt_std_err) {
		if(opt_std_err_color) {
			std::cerr << "\033[36m";
 			std::cerr << obj.GetName() << ":" << endl << buffer << endl;
			std::cerr << "\033[0m";
		} else {
			std::cerr << "DebugInfo(" << obj.GetName() << "):" << endl << buffer << endl;
		}
	}
	if(opt_xml_file) {
		XmlDebug("info", obj, buffer);
	}
}

void 
LoggerServer::
DebugWarning(const unisim::kernel::service::Object &obj, const char *buffer) {
	if(opt_std_out) {
		if(opt_std_out_color) {
			std::cout << "\033[33m";
			std::cout << obj.GetName() << ":" << endl << buffer << endl;
			std::cout << "\033[0m";
		} else {
			std::cout << "DebugWarning(" << obj.GetName() << "): " << endl << buffer << endl;
		}
	}
	if(opt_std_err) {
		if(opt_std_err_color) {
			std::cerr << "\033[33";
			std::cerr << obj.GetName() << ":" << endl << buffer << endl;
			std::cerr << "\033[0m";
		} else {
			std::cerr << "DebugWarning(" << obj.GetName() << "):" << endl << buffer << endl;
		}
	}
	if(opt_xml_file) {
		XmlDebug("warning", obj, buffer);
	}
}

void 
LoggerServer::
DebugError(const unisim::kernel::service::Object &obj, const char *buffer) {
	if(opt_std_out) {
		if(opt_std_out_color) {
			std::cout << "\033[31m";
			std::cout << obj.GetName() << ":" << endl << buffer << endl;
			std::cout << "\033[0m";
		} else {
			std::cout << "DebugError(" << obj.GetName() << "): " << endl << buffer << endl;
		}
	}
	if(opt_std_err) {
		if(opt_std_err_color) {
			std::cerr << "\033[31m";
			std::cerr << obj.GetName() << ":" << endl << buffer << endl;
			std::cerr << "\033[0m";
		} else {
			std::cerr << "DebugError(" << obj.GetName() << "): " << endl << buffer << endl;
		}
	}
	if(opt_xml_file) {
		XmlDebug("error", obj, buffer);
	}
}

} // end of namespace logger
} // end of namespace kernel
} // end of namespace unisim

