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

#include "unisim/kernel/service/xml_helper.hh"
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
XMLHelper(Simulator *_simulator) :
	simulator(_simulator),
	cur_var(0),
	cur_status(NONE),
	name_token(0),
	variables_token(0),
	object_token(0),
	variable_token(0),
	type_token(0),
	value_token(0),
	default_value_token(0),
	data_type_token(0),
	description_token(0),
	_text_token(0)
{
	name_token = xmlCharStrdup("name");
	variables_token = xmlCharStrdup("variables");
	object_token = xmlCharStrdup("object");
	variable_token = xmlCharStrdup("variable");
	type_token = xmlCharStrdup("type");
	value_token = xmlCharStrdup("value");
	default_value_token = xmlCharStrdup("default_value");
	data_type_token = xmlCharStrdup("data_type");
	description_token = xmlCharStrdup("description");
	_text_token = xmlCharStrdup("#text");

}

XMLHelper::
~XMLHelper()
{
	free(name_token); name_token = 0;
	free(variables_token); variables_token = 0;
	free(object_token); object_token = 0;
	free(variable_token); variable_token = 0;
	free(type_token); type_token = 0;
	free(value_token); value_token = 0;
	free(default_value_token); default_value_token = 0;
	free(data_type_token); data_type_token = 0;
	free(description_token); description_token = 0;
	free(_text_token); _text_token = 0;
}

bool
XMLHelper::
XmlfyVariables(const char *filename, VariableBase::Type type) {
	xmlTextWriterPtr writer;

	writer = xmlNewTextWriterFilename(filename, 0);
	if(writer == NULL) {
		cerr << "Error(Simulator::XmlfyVariables): "
			<< "could not open output file (" 
			<< filename << ")" << endl;
		return false;
	}
	int rc = xmlTextWriterSetIndent(writer, 1);
	if(rc < 0) {
		cerr << "Warning(Simulator::XmlfyVariables): "
			<< "could not set indentation" << endl;
	} else {
		rc = xmlTextWriterSetIndentString(writer, (xmlChar*)"\t");
		if (rc < 0) {
			cerr << "Warning(Simulator::XmlfyVariables): "
				<< "could not set indentation string" << endl;
		}
	}
	rc = xmlTextWriterStartDocument(writer, NULL,
			XML_ENCODING, NULL);
	if(rc < 0) {
		cerr << "Error(Simulator::XmlfyVariables): "
			<< "error starting the xml document" << endl;
		xmlFreeTextWriter(writer);
		return false;
	}
	// rc = xmlTextWriterStartElement(writer, BAD_CAST "VARIABLES");
	rc = xmlTextWriterStartElement(writer, BAD_CAST "variables");
	if(rc < 0) {
		cerr << "Error(Simulator::XmlfyVariables): "
			<< "error starting the xml document" << endl;
		xmlFreeTextWriter(writer);
		return false;
	}

	/* First the variables of the different objects contained in the simulator
	 * are xmlfied, respecting the tree structure of the objects.
	 * Afterwards the simulator level variables (those that are directly 
	 * attached to the simulator).
	 */
	list<Object *> obj_list;
	list<Object *>::iterator obj_iter;
	Simulator::simulator->GetRootObjects(obj_list);
	for ( obj_iter = obj_list.begin();
			obj_iter != obj_list.end();
			obj_iter++ )
	{
		if ( HasVariable(*obj_iter, type) )
		{
			rc = XmlfyVariables(writer, *obj_iter, type);
			if ( rc < 0 )
			{
				cerr << "Error(ServiceManage::XmlfyVariables): "
					<< "error writing root object"
					<< endl;
				xmlFreeTextWriter(writer);
				return false;
			}
		}
	}

	map<const char *, VariableBase *, Simulator::ltstr>::iterator var_iter;
	for ( var_iter = Simulator::simulator->variables.begin();
			var_iter != Simulator::simulator->variables.end();
			var_iter++ )
	{
		if ( (*var_iter).second->IsVisible() && (type == VariableBase::VAR_VOID ||
				type == (*var_iter).second->GetType()))
		{
			// check that the variable is a root variable by checking that it
			//   has not object owner
			bool root_var = ((*var_iter).second->GetOwner() == 0);
			if ( root_var )
			{
				rc = XmlfyVariable(writer, (*var_iter).second);
				if ( rc < 0 )
				{
					cerr << "Error(Simulator::XmlfyVariables): "
							<< "error writing variable"
							<< endl;
					xmlFreeTextWriter(writer);
					return false;
				}
			}
		}
	}

	rc = xmlTextWriterEndElement(writer);
	if(rc < 0) {
		cerr << "Error(Simulator::XmlfyVariables): "
			<< "could not close the root element" << endl;
		xmlFreeTextWriter(writer);
		return false;
	}
	rc = xmlTextWriterEndDocument(writer);
	if(rc < 0) {
		cerr << "Warning(Simulator::XmlfyVariables): "
			<< "could not correctly close the XMLWriter"
			<< endl;
	}
	xmlFreeTextWriter(writer);

	return true;
}

bool
XMLHelper::
HasVariable(const Object *obj,
		VariableBase::Type type)
{
	list<VariableBase *> var_list;
	list<VariableBase *>::iterator var_iter;
	obj->GetVariables(var_list);
	for(var_iter = var_list.begin();
			var_iter != var_list.end();
			var_iter++) {
		if ((*var_iter)->IsSerializable() && (type == VariableBase::VAR_VOID ||
				type == (*var_iter)->GetType()))
			return true;
	}
	
	list<Object *> obj_list;
	list<Object *>::iterator obj_iter;
	obj_list = obj->GetLeafs();
	for (obj_iter = obj_list.begin();
			obj_iter != obj_list.end();
			obj_iter++)
	{
		if (HasVariable(*obj_iter, type))
			return true;
	}
	
	return false;
}

int
XMLHelper::
XmlfyVariables(xmlTextWriterPtr writer,
		const Object *obj,
		VariableBase::Type type)
{
	int rc;

	// initiate object
	rc = xmlTextWriterStartElement(writer, BAD_CAST "object");
	if (rc < 0) return rc;

	// write object name
	{
		xmlChar* obj_name = xmlCharStrdup(obj->GetName());

		rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "name", obj_name);
		free(obj_name); obj_name = 0;
	}
	if (rc < 0) return rc;

	// dump inner objects
	list<Object *> obj_list;
	list<Object *>::iterator obj_iter;
	obj_list = obj->GetLeafs();
	for (obj_iter = obj_list.begin();
			obj_iter != obj_list.end();
			obj_iter++)
	{
		if (HasVariable(*obj_iter, type))
		{
			rc = XmlfyVariables(writer, *obj_iter, type);
			if (rc < 0)
			{
				cerr << "Error(ServiceManage::XmlfyVariables): "
					<< "error writing object"
					<< endl;
				return rc;
			}
		}
	}

	// dump object variables
	list<VariableBase *> var_list;
	list<VariableBase *>::iterator var_iter;
	obj->GetVariables(var_list);
	for(var_iter = var_list.begin();
			var_iter != var_list.end();
			var_iter++) {
		if (type == VariableBase::VAR_VOID ||
				type == (*var_iter)->GetType())
		{
			rc = XmlfyVariable(writer, *var_iter);
			if(rc < 0) {
				cerr << "Error(Simulator::XmlfyVariables): "
					<< "error writing variable"
					<< endl;
				return rc;
			}
		}
	}
	
	rc = xmlTextWriterEndElement(writer);
	if (rc < 0) return rc;

	return 0;
}

int
XMLHelper::
XmlfyVariable(xmlTextWriterPtr writer, 
		const VariableBase *var) 
{
	int rc;

	rc = xmlTextWriterStartElement(writer, BAD_CAST "variable");
	if(rc < 0) return rc;

	// write variable name
	{
		xmlChar* var_name = xmlCharStrdup(var->GetVarName());
		rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "name", var_name);
		free(var_name); var_name = 0;
	}
	if (rc < 0) return rc;

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
		case VariableBase::VAR_SIGNAL:
			rc = xmlTextWriterWriteFormatString(writer, "signal");
			break;
		case VariableBase::VAR_VOID:
		case VariableBase::VAR_ARRAY:
		case VariableBase::VAR_FORMULA:
			cerr << "Unexpected variable type to Xmlfy. Variable name is '"
				<< var->GetVarName() << "'" << endl;
			break;
	}
	if(rc < 0) return rc;
	rc = xmlTextWriterEndElement(writer);
	if(rc < 0) return rc;

//	// writing the variable name
//	rc = xmlTextWriterStartElement(writer, BAD_CAST "name");
//	if(rc < 0) return rc;
//	rc = xmlTextWriterWriteFormatString(writer, "%s", var->GetName());
//	if(rc < 0) return rc;
//	rc = xmlTextWriterEndElement(writer);
//	if(rc < 0) return rc;

	// writing the variable data type
	rc = xmlTextWriterStartElement(writer, BAD_CAST "data_type");
	if(rc < 0) return rc;
	rc = xmlTextWriterWriteFormatString(writer, "%s", var->GetDataTypeName());
	if(rc < 0) return rc;
	rc = xmlTextWriterEndElement(writer);
	if(rc < 0) return rc;

//	// writing the variable default value
//	rc = xmlTextWriterStartElement(writer, BAD_CAST "default_value");
//	if(rc < 0) return rc;
//	if(string("double").compare(var->GetDataTypeName()) == 0 ||
//		string("float").compare(var->GetDataTypeName()) == 0) {
//		rc = xmlTextWriterWriteFormatString(writer, "%e", (double) *var);
//	} else {
//		rc = xmlTextWriterWriteFormatString(writer, "%s", ((string) *var).c_str());
//	}
//	if(rc < 0) return rc;
//	rc = xmlTextWriterEndElement(writer);
//	if(rc < 0) return rc;

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

	return 1;
}

bool
XMLHelper::
LoadXmlVariables(const char *_filename, VariableBase::Type type)
{
	string filename = simulator->SearchSharedDataFile(_filename);
	
	xmlTextReaderPtr reader;
	int ret;

	cerr << "Loading xml parameters from: " << filename << endl;
	
	/*
	 * Pass some special parsing options to activate DTD attribute defaulting,
	 * entities substitution and DTD validation
	 */
	reader = xmlReaderForFile(filename.c_str(), NULL, 0);
	if (reader != NULL) {
		cur_status = NONE;
		cur_var = NULL;
		ret = xmlTextReaderRead(reader);
		while (ret == 1) {
			if(ProcessXmlVariableNode(reader, type))
				ret = xmlTextReaderRead(reader);
			else
				return false;
		}
		xmlFreeTextReader(reader);
		if (ret != 0) {
			cerr << filename << ": failed to parse" << endl;
			return false;
		}
		return true;
	} else {
		cerr << "Unable to open " << filename << endl;
		return false;
	}
}

bool 
XMLHelper::
ProcessXmlVariableNode(xmlTextReaderPtr reader, VariableBase::Type type) 
{ 
	const xmlChar* name = 0;
	const xmlChar* value = 0;
	xmlChar* name_attr = 0;

	name = xmlTextReaderConstName(reader);
	if (name == NULL) 
	{
		cerr << "Could not read Xml variable node" << endl;
		return false;
	}
	
	if (xmlStrEqual(name, variables_token))
	{
//		if (xmlTextReaderNodeType(reader) == 1)
//			cerr << "variables" << endl;
		return true;
	}

	if (xmlStrEqual(name, object_token))
	{
		if (xmlTextReaderNodeType(reader) == 1)
		{
			name_attr = xmlTextReaderGetAttribute(reader, name_token);
			if (name_attr == 0)
			{
				cerr << "Error: could not get object name" << endl;
				return false;
			}
			cerr << " + object " << name_attr << endl;
			cur_object.push_back(string((char *)name_attr));
			free(name_attr);
		}
		if (xmlTextReaderNodeType(reader) == 15)
		{
			cerr << " - object " << endl;
			cur_object.pop_back();
		}
		return true;
	}
	
	if (xmlStrEqual(name, variable_token))
	{
		if (xmlTextReaderNodeType(reader) == 1)
		{
			name_attr = xmlTextReaderGetAttribute(reader, name_token);
			if (name_attr == 0)
			{
				cerr << "Error: could not get variable name" << endl;
				return false;
			}
			cur_var = new CurVariable();
			if ( cur_object.size() != 0 )
				cur_var->name << cur_object.back() << "." << name_attr;
			else 
				cur_var->name << name_attr;
			free(name_attr);
		}
		if (xmlTextReaderNodeType(reader) == 15)
		{
//			cerr << "    variable" << endl;
//			cerr << "      type = " << cur_var->type.str() << endl;
//			cerr << "      name = " << cur_var->name.str() << endl;
//			cerr << "      value = " << cur_var->value.str() << endl;
			// cerr << "    description = " << cur_var->description.str() << endl;
			
			bool modify = 
				(type == VariableBase::VAR_VOID) ||
				(cur_var->type.str().empty()) ||
				(type == VariableBase::VAR_PARAMETER && cur_var->type.str().compare("parameter") == 0) ||
				(type == VariableBase::VAR_REGISTER && cur_var->type.str().compare("register") == 0) ||
				(type == VariableBase::VAR_STATISTIC && cur_var->type.str().compare("statistic") == 0);
			if (modify)
			{
				simulator->SetVariable(cur_var->name.str().c_str(), cur_var->value.str().c_str());
			}
			delete cur_var;
		}
		return true;
	}

	if(xmlStrEqual(name, type_token)) {
		if(xmlTextReaderNodeType(reader) == 1) {
			cur_status = TYPE;
		}
		if(xmlTextReaderNodeType(reader) == 15) {
			cur_status = NONE;
		}
		return true;
	}

	// TODO: this code should be removed because name is now an attribute
	if(xmlStrEqual(name, name_token)) {
		if(xmlTextReaderNodeType(reader) == 1) {
			cur_status = NAME;
		}
		if(xmlTextReaderNodeType(reader) == 15) {
			cur_status = NONE;
		}
		return true;
	}
	// end of TODO
	
	// TODO: this code should be removed because default_value has dissappeared
	if(xmlStrEqual(name, default_value_token)) {
		// nothing to do
		return true;
	}
	// end of TODO

	if(xmlStrEqual(name, value_token)) {
		if(xmlTextReaderNodeType(reader) == 1) {
			cur_status = VALUE;
		}
		if(xmlTextReaderNodeType(reader) == 15) {
			cur_status = NONE;
		}
		return true;
	}
	
	if(xmlStrEqual(name, description_token)) {
		if(xmlTextReaderNodeType(reader) == 1) {
			cur_status = DESCRIPTION;
		}
		if(xmlTextReaderNodeType(reader) == 15) {
			cur_status = NONE;
		}
		return true;
	}
	
	if(xmlStrEqual(name, data_type_token)) {
		if(xmlTextReaderNodeType(reader) == 1) {
			cur_status = DATA_TYPE;
		}
		if(xmlTextReaderNodeType(reader) == 15) {
			cur_status = NONE;
		}
	}
	
	if(xmlStrEqual(name, _text_token)) {
		value = xmlTextReaderConstValue(reader);
		switch(cur_status) {
		case NONE:
			break;
		case TYPE:
			cur_var->type << value;
			break;
		// TODO: this code should be removed because name now is an attribute
		case NAME:
			cur_var->name << value;
			break;
		// end of TODO
		case VALUE:
			cur_var->value << value;
			break;
		case DESCRIPTION:
			// cur_var->description << value;
			break;
		case DATA_TYPE:
			cur_var->data_type << value;
			break;
		}
		return true;
	}
	
	return true;
}

} // end of namespace service
} // end of namespace kernel
} // end of namespace unisim
