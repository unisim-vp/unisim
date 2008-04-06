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
XmlfyVariables(const char *filename) {
	xmlTextWriterPtr writer;

	writer = xmlNewTextWriterFilename(filename, 0);
	if(writer == NULL) {
		cerr << "Error(ServiceManager::XmlfyVariables): "
			<< "could not open output file (" 
			<< filename << ")" << endl;
		return false;
	}
	int rc = xmlTextWriterSetIndent(writer, 2);
	if(rc < 0) {
		cerr << "Warning(ServiceManager::XmlfyVariables): "
			<< "could not set indentation" << endl;
	}
	rc = xmlTextWriterStartDocument(writer, NULL,
			XML_ENCODING, NULL);
	if(rc < 0) {
		cerr << "Error(ServiceManager::XmlfyVariables): "
			<< "error starting the xml document" << endl;
		return false;
	}
	rc = xmlTextWriterStartElement(writer, BAD_CAST "VARIABLES");
	if(rc < 0) {
		cerr << "Error(ServiceManager::XmlfyVariables): "
			<< "error starting the xml document" << endl;
		return false;
	}

	
	list<VariableBase *> param_list;
	list<VariableBase *>::iterator param_iter;
	ServiceManager::GetParameters(param_list);
	for(param_iter = param_list.begin();
			param_iter != param_list.end();
			param_iter++) {
		rc = XmlfyVariable(writer, *param_iter);
		if(rc < 0) {
			cerr << "Error(ServiceManager::XmlfyVariables): "
				<< "error writing parameter variable"
				<< endl;
			return false;
		}
	}

	list<VariableBase *> reg_list;
	list<VariableBase *>::iterator reg_iter;
	ServiceManager::GetRegisters(reg_list);
	for(reg_iter = reg_list.begin();
			reg_iter != reg_list.end();
			reg_iter) {
		rc = XmlfyVariable(writer, *reg_iter);
		if(rc < 0) {
			cerr << "Error(ServiceManager::XmlfyVariables): "
				<< "error writing register variable"
				<< endl;
			return false;
		}
	}

	list<VariableBase *> stat_list;
	list<VariableBase *>::iterator stat_iter;
	ServiceManager::GetStatistics(stat_list);
	for(stat_iter = stat_list.begin();
			stat_iter != stat_list.end();
			stat_iter) {
		rc = XmlfyVariable(writer, *stat_iter);
		if(rc < 0) {
			cerr << "Error(ServiceManager::XmlfyVariables): "
				<< "error writing statistic variable"
				<< endl;
			return false;
		}
	}

	rc = xmlTextWriterEndElement(writer);
	if(rc < 0) {
		cerr << "Error(ServiceManager::XmlfyVariables): "
			<< "could not close the root element" << endl;
		return false;
	}
	rc = xmlTextWriterEndDocument(writer);
	if(rc < 0) {
		cerr << "Warning(ServiceManager::XmlfyVariables): "
			<< "could not correctly close the XMLWriter"
			<< endl;
	}
	xmlFreeTextWriter(writer);

	return true;
}

int
XMLHelper::
XmlfyVariable(xmlTextWriterPtr writer, 
		VariableBase *var) {
	int rc;

	rc = xmlTextWriterStartElement(writer, BAD_CAST "variable");
	if(rc < 0) return rc;

	// writing the variable type
	rc = xmlTextWriterStartElement(writer, BAD_CAST "type");
	if(rc < 0) return rc;
	switch(var->GetType()) {
		case VariableBase::VAR_PARAMETER:
			rc = xmlTextWriterWriteFormatString(writer, "parameter");
			break;
		case VariableBase::VAR_STATISTIC:
			rc = xmlTextWriterWriteFormatString(writer, "statistic");
			break;
		case VariableBase::VAR_REGISTER:
			rc = xmlTextWriterWriteFormatString(writer, "register");
			break;
	}
	if(rc < 0) return rc;
	rc = xmlTextWriterEndElement(writer);
	if(rc < 0) return rc;

	// writing the variable name
	rc = xmlTextWriterStartElement(writer, BAD_CAST "name");
	if(rc < 0) return rc;
	rc = xmlTextWriterWriteFormatString(writer, "%s", var->GetName());
	if(rc < 0) return rc;
	rc = xmlTextWriterEndElement(writer);
	if(rc < 0) return rc;

	// writing the variable data type
	rc = xmlTextWriterStartElement(writer, BAD_CAST "data_type");
	if(rc < 0) return rc;
	rc = xmlTextWriterWriteFormatString(writer, "%s", var->GetDataTypeName());
	if(rc < 0) return rc;
	rc = xmlTextWriterEndElement(writer);
	if(rc < 0) return rc;

	// writing the variable default value
	rc = xmlTextWriterStartElement(writer, BAD_CAST "default_value");
	if(rc < 0) return rc;
	if(string("double").compare(var->GetDataTypeName()) == 0 ||
		string("float").compare(var->GetDataTypeName()) == 0) {
		rc = xmlTextWriterWriteFormatString(writer, "%e", (double) *var);
	} else {
		rc = xmlTextWriterWriteFormatString(writer, "%s", ((string) *var).c_str());
	}
	if(rc < 0) return rc;
	rc = xmlTextWriterEndElement(writer);
	if(rc < 0) return rc;

	// writing the variable value
	rc = xmlTextWriterStartElement(writer, BAD_CAST "value");
	if(rc < 0) return rc;
	if(string("double").compare(var->GetDataTypeName()) == 0 ||
		string("float").compare(var->GetDataTypeName()) == 0) {
		rc = xmlTextWriterWriteFormatString(writer, "%e", (double) *var);
	} else {
		rc = xmlTextWriterWriteFormatString(writer, "%s", ((string) *var).c_str());
	}
	if(rc < 0) return rc;
	rc = xmlTextWriterEndElement(writer);
	if(rc < 0) return rc;

	// writing variable description
	rc = xmlTextWriterStartElement(writer, BAD_CAST "description");
	if(rc < 0) return rc;
	rc = xmlTextWriterWriteFormatString(writer, "%s", var->GetDescription());
	if(rc < 0) return rc;
	rc = xmlTextWriterEndElement(writer);
	if(rc < 0) return rc;

	// if the variable has enumerated values then create
	// an entry with the possible values
	if(var->HasEnumeratedValues()) {
		rc = xmlTextWriterStartElement(writer, BAD_CAST "enumeration");
		if(rc < 0) return rc;
		vector<string> values;
		var->GetEnumeratedValues(values);
		if(values.empty()) return -1;
		vector<string>::iterator it;
		for(it = values.begin(); it != values.end(); it++) {
			rc = xmlTextWriterStartElement(writer, BAD_CAST "item");
			if(rc < 0) return rc;
			rc = xmlTextWriterWriteFormatString(writer, "%s", (*it).c_str());
			if(rc < 0) return rc;
			rc = xmlTextWriterEndElement(writer);
			if(rc < 0) return rc;
		}
		rc = xmlTextWriterEndElement(writer);
		if(rc < 0) return rc;
	}

	// closing variable element
	rc = xmlTextWriterEndElement(writer);
	if(rc < 0) return rc;

	return 0;
}

bool 
XMLHelper::
XmlfyParameters(const char *filename) {
	xmlTextWriterPtr writer;

	writer = xmlNewTextWriterFilename(filename, 0);
	if(writer == NULL) {
		cerr << "Error(ServiceManager::XmlfyParameters): "
			<< "could not open output file (" 
			<< filename << ")" << endl;
		return false;
	}
	int rc = xmlTextWriterSetIndent(writer, 2);
	if(rc < 0) {
		cerr << "Warning(ServiceManager::XmlfyParameters): "
			<< "could not set indentation" << endl;
	}
	rc = xmlTextWriterStartDocument(writer, NULL,
			XML_ENCODING, NULL);
	if(rc < 0) {
		cerr << "Error(ServiceManager::XmlfyParameters): "
			<< "error starting the xml document" << endl;
		return false;
	}
	rc = xmlTextWriterStartElement(writer, BAD_CAST "VARIABLES");
	if(rc < 0) {
		cerr << "Error(ServiceManager::XmlfyParameters): "
			<< "error starting the xml document" << endl;
		return false;
	}

	
	list<VariableBase *> param_list;
	list<VariableBase *>::iterator param_iter;
	ServiceManager::GetParameters(param_list);
	for(param_iter = param_list.begin();
			param_iter != param_list.end();
			param_iter++) {
		rc = XmlfyVariable(writer, *param_iter);
		if(rc < 0) {
			cerr << "Error(ServiceManager::XmlfyParameters): "
				<< "error writing parameter variable"
				<< endl;
			return false;
		}
	}
	
	rc = xmlTextWriterEndElement(writer);
	if(rc < 0) {
		cerr << "Error(ServiceManager::XmlfyParameters): "
			<< "could not close the root element" << endl;
		return false;
	}
	rc = xmlTextWriterEndDocument(writer);
	if(rc < 0) {
		cerr << "Warning(ServiceManager::XmlfyParameters): "
			<< "could not correctly close the XMLWriter"
			<< endl;
	}
	xmlFreeTextWriter(writer);

	return true;
}

bool
XMLHelper::
LoadXmlVariables(const char *filename, bool params,
		bool regs, bool stats) {
	xmlTextReaderPtr reader;
	int ret;

	cerr << "Loading xml parameters from: " << filename << endl;
	
	/*
	 * Pass some special parsing options to activate DTD attribute defaulting,
	 * entities substitution and DTD validation
	 */
	reader = xmlReaderForFile(filename, NULL, 0);
	if (reader != NULL) {
		cur_status = NONE;
		cur_var = NULL;
		ret = xmlTextReaderRead(reader);
		while (ret == 1) {
			if(ProcessXmlVariableNode(reader, params, regs, stats))
				ret = xmlTextReaderRead(reader);
			else
				return false;
		}
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
ProcessXmlVariableNode(xmlTextReaderPtr reader, bool params, bool regs, bool stats) { 
	const xmlChar *name, *value;

	name = xmlTextReaderConstName(reader);
	if (name == NULL) {
		cerr << "Could not read Xml variable node" << endl;
		return false;
	}

	if(xmlStrEqual(name, xmlCharStrdup("VARIABLES"))) {
		if(xmlTextReaderNodeType(reader) == 1)
			cerr << "VARIABLES" << endl;
		return true;
	}
	
	if(xmlStrEqual(name, xmlCharStrdup("variable"))) {
		if(xmlTextReaderNodeType(reader) == 1) {
			cur_var = new CurVariable();
		}
		if(xmlTextReaderNodeType(reader) == 15) {
			cerr << "  variable" << endl;
			cerr << "    type = " << cur_var->type.str() << endl;
			cerr << "    name = " << cur_var->name.str() << endl;
			cerr << "    value = " << cur_var->value.str() << endl;
			cerr << "    description = " << cur_var->description.str() << endl;
			bool modify = 
				(params && cur_var->type.str().compare("parameter") == 0) ||
				(regs && cur_var->type.str().compare("register") == 0) ||
				(stats && cur_var->type.str().compare("statistic") == 0);
			if(modify) {
				VariableBase *variable = ServiceManager::GetParameter(cur_var->name.str().c_str());
				if(variable == NULL) {
					cerr << "  !! could not get variable '" << cur_var->name.str() << "'" << endl;
					return false;
				}
				*variable = cur_var->value.str().c_str();
			}
			delete cur_var;
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
		// nothing to do
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
	
	if(xmlStrEqual(name, xmlCharStrdup("data_type"))) {
		if(xmlTextReaderNodeType(reader) == 1) {
			cur_status = DATA_TYPE;
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
		case TYPE:
			cur_var->type << value;
			break;
		case NAME:
			cur_var->name << value;
			break;
		case VALUE:
			cur_var->value << value;
			break;
		case DESCRIPTION:
			cur_var->description << value;
			break;
		case DATA_TYPE:
			cur_var->data_type << value;
			break;
		}
		return true;
	}
	
	return true;
}

bool 
XMLHelper::
LoadXmlParameters(const char *filename) {
	LoadXmlVariables(filename, true, false, false);
}

} // end of namespace service
} // end of namespace kernel
} // end of namespace unisim
