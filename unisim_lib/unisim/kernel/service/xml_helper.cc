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

#include <unisim/kernel/service/xml_helper.hh>
#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>
#include <map>
#include <iostream>

namespace unisim {
namespace kernel {
namespace service {

using std::map;

const char *XMLHelper::XML_ENCODING = "UTF-8"; 

XMLHelper::
XMLHelper() {}

XMLHelper::
~XMLHelper() {}
	
bool 
XMLHelper::
XmlfyParameters(const char *filename) {
    xmlTextWriterPtr writer;
    
	writer = xmlNewTextWriterFilename(filename, 0);
	if(writer == NULL) {
		cerr << "Error(ServiceManager::XmlfyParameters): "
			<< "could not open output file for logging" << endl;
		return false;
	}
	int rc = xmlTextWriterSetIndent(writer, 2);
	if(rc < 0) {
		cerr << "Warning(ServiceManager::XmlfyParameters): could not set indentation" << endl;
	}
	rc = xmlTextWriterStartDocument(writer, NULL, XML_ENCODING, NULL);
	if(rc < 0) {
		cerr << "Error(ServiceManager::XmlfyParameters): "
			<< "error starting the xml document" << endl;
		return false;
	}
	rc = xmlTextWriterStartElement(writer, BAD_CAST "PARAMETERS");
	if(rc < 0) {
		cerr << "Error(ServiceManager::XmlfyParameters): "
			<< "error starting the xml document" << endl;
		return false;
	}

	map<const char *, VariableBase *, ServiceManager::ltstr>::iterator variable_iter;

	for(variable_iter = ServiceManager::variables.begin(); variable_iter != ServiceManager::variables.end(); variable_iter++) {
		//.parameter arrays containers have a special type (an empty string "") and they should not appear
		//   in the parameter xml configuration file, so just ignore them
		if((*variable_iter).second->GetType() != VariableBase::VAR_PARAMETER) continue;
		if(strcmp("", (*variable_iter).second->GetDataTypeName()) == 0) continue;
		// opening parameter element
		rc = xmlTextWriterStartElement(writer, BAD_CAST "parameter");
		if(rc < 0) {
			cerr << "Error(ServiceManager::XmlfyParameters): "
				<< "error writing parameter element" << endl;
			return false;
		}
		
		// writing parameter name 
		rc = xmlTextWriterStartElement(writer, BAD_CAST "name");
		if(rc < 0) {
			cerr << "Error(ServiceManager::XmlfyParameters): "
				<< "error writing parameter name element" << endl;
			return false;
		}
		rc = xmlTextWriterWriteFormatString(writer, "%s", (*variable_iter).second->GetName());
		if(rc < 0) {
			cerr << "Error(ServiceManager::XmlfyParameters): error writing parameter name value" << endl;
			return false;
		}
		rc = xmlTextWriterEndElement(writer);
		if(rc < 0) {
			cerr << "Error(ServiceManager::XmlfyParameters): "
				<< "could not close the parameter name element" << endl;
			return false;
		}
		
		// writing parameter type 
		rc = xmlTextWriterStartElement(writer, BAD_CAST "type");
		if(rc < 0) {
			cerr << "Error(ServiceManager::XmlfyParameters): "
				<< "error writing parameter type element" << endl;
			return false;
		}
		rc = xmlTextWriterWriteFormatString(writer, "%s", (*variable_iter).second->GetDataTypeName());
		if(rc < 0) {
			cerr << "Error(ServiceManager::XmlfyParameters): error writing parameter type value" << endl;
			return false;
		}
		rc = xmlTextWriterEndElement(writer);
		if(rc < 0) {
			cerr << "Error(ServiceManager::XmlfyParameters): "
				<< "could not close the parameter name element" << endl;
			return false;
		}
		
		// writing parameter default value
		rc = xmlTextWriterStartElement(writer, BAD_CAST "default_value");
		if(rc < 0) {
			cerr << "Error(ServiceManager::XmlfyParameters): "
				<< "error writing parameter default value element" << endl;
			return false;
		}
		if(string("double").compare((*variable_iter).second->GetDataTypeName()) == 0 ||
				string("float").compare((*variable_iter).second->GetDataTypeName()) == 0) {
			rc = xmlTextWriterWriteFormatString(writer, "%e", (double) *(*variable_iter).second);
		} else {
			rc = xmlTextWriterWriteFormatString(writer, "%s", ((string) *(*variable_iter).second).c_str());
		}
		if(rc < 0) {
			cerr << "Error(ServiceManager::XmlfyParameters): error writing parameter default value" << endl;
			return false;
		}
		rc = xmlTextWriterEndElement(writer);
		if(rc < 0) {
			cerr << "Error(ServiceManager::XmlfyParameters): "
				<< "could not close the parameter default value element" << endl;
			return false;
		}
		
		// writing parameter value
		rc = xmlTextWriterStartElement(writer, BAD_CAST "value");
		if(rc < 0) {
			cerr << "Error(ServiceManager::XmlfyParameters): "
				<< "error writing parameter value element" << endl;
			return false;
		}
		if(string("double").compare((*variable_iter).second->GetDataTypeName()) == 0 ||
				string("float").compare((*variable_iter).second->GetDataTypeName()) == 0) {
			rc = xmlTextWriterWriteFormatString(writer, "%e", (double) *(*variable_iter).second);
		} else {
			rc = xmlTextWriterWriteFormatString(writer, "%s", ((string) *(*variable_iter).second).c_str());
		}
		if(rc < 0) {
			cerr << "Error(ServiceManager::XmlfyParameters): error writing parameter value" << endl;
			return false;
		}
		rc = xmlTextWriterEndElement(writer);
		if(rc < 0) {
			cerr << "Error(ServiceManager::XmlfyParameters): "
				<< "could not close the parameter value element" << endl;
			return false;
		}
		
		// writing parameter description
		rc = xmlTextWriterStartElement(writer, BAD_CAST "description");
		if(rc < 0) {
			cerr << "Error(ServiceManager::XmlfyParameters): "
				<< "error writing parameter description element" << endl;
			return false;
		}
		rc = xmlTextWriterWriteFormatString(writer, "%s", (*variable_iter).second->GetDescription());
		if(rc < 0) {
			cerr << "Error(ServiceManager::XmlfyParameters): error writing parameter description" << endl;
			return false;
		}
		rc = xmlTextWriterEndElement(writer);
		if(rc < 0) {
			cerr << "Error(ServiceManager::XmlfyParameters): "
				<< "could not close the parameter description element" << endl;
			return false;
		}

		// if the parameter has enumerated values then create an entry with the 
		//   possible values
		cerr << "checking for enumerated values " << (*variable_iter).second->GetName() 
			<< " (" << (*variable_iter).second->HasEnumeratedValues() << ")" << endl;
		if((*variable_iter).second->HasEnumeratedValues()) {
			rc = xmlTextWriterStartElement(writer, BAD_CAST "enumeration");
			if(rc < 0) {
				cerr << "Error(ServiceManager::XmlfyParameters): "
					<< "error writing parameter enumeration values" << endl;
				return false;
			}
			vector<string> values;
			(*variable_iter).second->GetEnumeratedValues(values);
			if(values.empty()) {
				cerr << "Error(ServiceManager::XmlfyParameters): "
					<< "could not get the parameter enumeration values" << endl;
				return false;
			}
			vector<string>::iterator it;
			for(it = values.begin(); it != values.end(); it++) {
				rc = xmlTextWriterStartElement(writer, BAD_CAST "item");
				if(rc < 0) {
					cerr << "Error(ServiceManager::XmlfyParameters): "
						<< "could not write parameter enumeration value entry" << endl;
					return false;
				}
				rc = xmlTextWriterWriteFormatString(writer, "%s", 
						(*it).c_str());
				if(rc < 0) {
					cerr << "Error(ServiceManager::XmlfyParameters): error writing parameter description" << endl;
					return false;
				}
				rc = xmlTextWriterEndElement(writer);
				if(rc < 0) {
					cerr << "Error(ServiceManager::XmlfyParameters): "
						<< "could not close parameter enumeration value entry" << endl;
					return false;
				}
			}
			rc = xmlTextWriterEndElement(writer);
			if(rc < 0) {
				cerr << "Error(ServiceManager::XmlfyParameters): "
					<< " could not close the parameter enumeration values" << endl;
				return false;
			}
		}
		
		// closing parameter element
		rc = xmlTextWriterEndElement(writer);
		if(rc < 0) {
			cerr << "Error(ServiceManager::XmlfyParameters): "
				<< "could not close the parameter element" << endl;
			return false;
		}
	}

	rc = xmlTextWriterEndElement(writer);
	if(rc < 0) {
		cerr << "Error(ServiceManager::XmlfyParameters): "
			<< "could not close the root element" << endl;
	}
	rc = xmlTextWriterEndDocument(writer);
	if(rc < 0) {
		cerr << "Warning(ServiceManager::XmlfyParameters): "
			<< "could not correctly close the XMLWriter" << endl;
	}
	xmlFreeTextWriter(writer);

	return true;
}

bool 
XMLHelper::
LoadXmlParameters(const char *filename) {
	xmlTextReaderPtr reader;
	int ret;

	cerr << "Loading xml parameters from: " << filename << endl;
	
	/*
	 * Pass some special parsing options to activate DTD attribute defaulting,
	 * entities substitution and DTD validation
	 */
	reader = xmlReaderForFile(filename, NULL, 0);
//			XML_PARSE_DTDATTR |  /* default DTD attributes */
//			XML_PARSE_NOENT |    /* substitute entities */
//			XML_PARSE_DTDVALID); /* validate with the DTD */
	if (reader != NULL) {
		cur_status = NONE;
		cur_param = NULL;
		ret = xmlTextReaderRead(reader);
		while (ret == 1) {
			if(ProcessXmlParamNode(reader))
				ret = xmlTextReaderRead(reader);
			else
				return false;
		}
//		/*
//		 * Once the document has been fully parsed check the validation results
//		 */
//		if (xmlTextReaderIsValid(reader) != 1) {
//			cerr << "Document " << filename << " does not validate" << endl;
//			return false;
//		}
		xmlFreeTextReader(reader);
		if (ret != 0) {
			cerr << filename << ": failed to parse" << endl;
			return false;
		}
	} else {
		cerr << "Unable to open " << filename << endl;
		return false;
	}
}

bool 
XMLHelper::
ProcessXmlParamNode(xmlTextReaderPtr reader) { 
	const xmlChar *name, *value;

	name = xmlTextReaderConstName(reader);
	if (name == NULL) {
		cerr << "Could not read Xml parameter node" << endl;
		return false;
	}

	if(xmlStrEqual(name, xmlCharStrdup("PARAMETERS"))) {
		if(xmlTextReaderNodeType(reader) == 1)
			cerr << "PARAMETERS" << endl;
		return true;
	}
	
	if(xmlStrEqual(name, xmlCharStrdup("parameter"))) {
		if(xmlTextReaderNodeType(reader) == 1) {
			cur_param = new CurParameter();
		}
		if(xmlTextReaderNodeType(reader) == 15) {
			cerr << "  parameter" << endl;
			cerr << "    name = " << cur_param->name.str() << endl;
			cerr << "    value = " << cur_param->value.str() << endl;
			cerr << "    description = " << cur_param->description.str() << endl;
			VariableBase *variable = ServiceManager::GetVariable(cur_param->name.str().c_str());
			if(variable == NULL) {
				cerr << "  !! could not get variable '" << cur_param->name.str() << "'" << endl;
				return false;
			}
			*variable = cur_param->value.str().c_str();
			delete cur_param;
		}
		return true;
	}

	if(xmlStrEqual(name, xmlCharStrdup("name"))) {
		if(xmlTextReaderNodeType(reader) == 1) {
			cur_status = NAME;
		}
		if(xmlTextReaderNodeType(reader) == 15) {
			cur_status = NONE;
		}
		return true;
	}
	
	if(xmlStrEqual(name, xmlCharStrdup("default_value"))) {
//		if(xmlTextReaderNodeType(reader) == 1) {
//			cur_status = VALUE;
//		}
//		if(xmlTextReaderNodeType(reader) == 15) {
//			cur_status = NONE;
//		}
		return true;
	}

	if(xmlStrEqual(name, xmlCharStrdup("value"))) {
		if(xmlTextReaderNodeType(reader) == 1) {
			cur_status = VALUE;
		}
		if(xmlTextReaderNodeType(reader) == 15) {
			cur_status = NONE;
		}
		return true;
	}
	
	if(xmlStrEqual(name, xmlCharStrdup("description"))) {
		if(xmlTextReaderNodeType(reader) == 1) {
			cur_status = DESCRIPTION;
		}
		if(xmlTextReaderNodeType(reader) == 15) {
			cur_status = NONE;
		}
		return true;
	}
	
	if(xmlStrEqual(name, xmlCharStrdup("type"))) {
		if(xmlTextReaderNodeType(reader) == 1) {
			cur_status = TYPE;
		}
		if(xmlTextReaderNodeType(reader) == 15) {
			cur_status = NONE;
		}
	}
	
	if(xmlStrEqual(name, xmlCharStrdup("#text"))) {
		value = xmlTextReaderConstValue(reader);
		switch(cur_status) {
		case NONE:
			break;
		case NAME:
			cur_param->name << value;
			break;
		case VALUE:
			cur_param->value << value;
			break;
		case DESCRIPTION:
			cur_param->description << value;
			break;
		case TYPE:
			cur_param->type << value;
			break;
		}
		return true;
	}
	
	return true;
}

} // end of namespace service
} // end of namespace kernel
} // end of namespace unisim
