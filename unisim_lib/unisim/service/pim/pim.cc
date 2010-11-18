
/*
 *  Copyright (c) 2010,
 *  Commissariat a l'Energie Atomique (CEA)
 *  All rights reserved.
 *
 *
 * Authors: Reda   Nouacer  (reda.nouacer@cea.fr)
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>


#include <fstream>
#include <sstream>
#include <iostream>
#include <list>

#include <unisim/service/pim/pim.hh>

namespace unisim {
namespace service {
namespace pim {

using namespace std;
using unisim::kernel::service::VariableBase;

PIM::PIM(const char *name, Simulator *simulator, uint16_t port, char* host, Object *parent) : 
	Object(name,parent),
	filename("pim.xml"),
	param_filename("filename", this, filename),
	fSimulator(simulator),
	fPort(port),
	fHost(host)
{ 

}

PIM::~PIM() {

	if (target) { delete target; target = NULL; }

	if (socketfd) { delete socketfd; socketfd = NULL; }

	for (int i=0; i < pim_model.size(); i++) {
		if (pim_model[i]) { delete pim_model[i]; pim_model[i] = NULL;}
	}
	pim_model.clear();
}

void PIM::GetExportedVariables(vector<component_t*> &pim) {

	// Explore Simulator Variables to Generate PIM XML File
	std::list<VariableBase *> lst;

	fSimulator->GetRegisters(lst);

	for (std::list<VariableBase *>::iterator it = lst.begin(); it != lst.end(); it++) {

		if (!((VariableBase *) *it)->IsVisible()) continue;

		simulator_variables.push_back((VariableBase *) *it);
		string var_name(((VariableBase *) *it)->GetName());
		size_t pos = var_name.find_first_of('.');
		string component_name = var_name.substr(0, pos);
		string short_var_name = var_name.substr(pos+1);

		component_t* component = FindComponent(component_name);
		if (component == NULL) {
			component = new component_t();

			component->name = component_name;
			component->description = "bla bla";

			pim.push_back(component);
		}

		component->pins.push_back((VariableBase *) *it);

	}
}

component_t* PIM::FindComponent(const string name) {

	for (int i=0; i < pim_model.size(); i++) {
		if (pim_model[i]->name.compare(name) == 0) {
			return pim_model[i];
		}
	}

	return NULL;
}


bool PIM::Setup() {

	GetExportedVariables(pim_model);

}

void PIM::Run() {

	// Open Socket Stream
	socketfd = new SocketServerThread(fHost, fPort);
//	socketfd = new SocketClientThread(fHost, fPort);

	socketfd->start();

	socketfd->join();

	cerr << "PIM connection success " << std::endl;

	// Start Simulation <-> ToolBox communication
//	target = new TargetThread("Target", &simulator_variables, socketfd);
	target = new TargetThread("Target", &simulator_variables, socketfd);

	target->start();

	target->join();

}

TargetThread::TargetThread(char* _name, vector<VariableBase*> *variables, SocketThread *fd) :
		socketfd(fd),
		name(_name),
		fVariables(variables)
{

}

void TargetThread::Run(){

	cerr << "PIM::TargetThread start RUN " << std::endl;

	while (!isTerminated()) {

		char *buffer = socketfd->receive();

		cerr << "PIM-Target receive " << buffer << std::endl;

		if (!isTerminated()) {
			string buf_str(buffer);

// qRcmd,cmd:var_name:value
			int index = buf_str.find(',');
			string qRcmd = buf_str.substr(0, index);
			buf_str = buf_str.substr(index+1);

			index = buf_str.find(':');
			string cmd = buf_str.substr(0, index);
			buf_str = buf_str.substr(index+1);

			if (cmd.compare("read") == 0) {

				index = buf_str.find(':');
				string name = buf_str.substr(0, index);
				buf_str = buf_str.substr(index+1);

				for (int i=0; i < fVariables->size(); i++) {
					if (name.compare((*fVariables)[i]->GetName()) == 0) {

						std::ostringstream os;
						os << (*fVariables)[i]->GetName() << ":";

						double val = *((*fVariables)[i]);
						os << stringify(val);

						std::string str = os.str();

						const char *buffer = str.c_str();

						socketfd->send(buffer);

						cout << name << " send: " << buffer << endl;

						os.str(std::string());
						break;
					}
				}

			} else if (cmd.compare("write") == 0) {

				index = buf_str.find(':');
				string name = buf_str.substr(0, index);
				buf_str = buf_str.substr(index+1);

				string value = buf_str.substr(index+1);

				for (int i=0; i < fVariables->size(); i++) {
					if (name.compare((*fVariables)[i]->GetName()) == 0) {

						*((*fVariables)[i]) = convertTo<double>(value);
						break;
					}
				}

			} else {
				cerr << "PIM-Target UNKNOWN command => " << buffer << std::endl;
			}
		}


		if (buffer != NULL) {
			free(buffer);
			buffer = NULL;
		}

	}

}

// *************************************************************

void PIM::GeneratePimFile() {

	stringstream pimfile;

	pimfile << filename;

	GetExportedVariables(pim_model);

	SavePimToXml(pim_model, pimfile.str());

}

/**
 * ConvertInput:
 * @in: string in a given encoding
 * @encoding: the encoding used
 *
 * Converts @in into UTF-8 for processing with libxml2 APIs
 *
 * Returns the converted UTF-8 string, or NULL in case of error.
 */
xmlChar *
PIM::ConvertInput(const char *in, const char *encoding)
{
    xmlChar *out;
    int ret;
    int size;
    int out_size;
    int temp;
    xmlCharEncodingHandlerPtr handler;

    if (in == 0)
        return 0;

    handler = xmlFindCharEncodingHandler(encoding);

    if (!handler) {
        printf("ConvertInput: no encoding handler found for '%s'\n",
               encoding ? encoding : "");
        return 0;
    }

    size = (int) strlen(in) + 1;
    out_size = size * 2 - 1;
    out = (unsigned char *) xmlMalloc((size_t) out_size);

    if (out != 0) {
        temp = size - 1;
        ret = handler->input(out, &out_size, (const xmlChar *) in, &temp);
        if ((ret < 0) || (temp - size + 1)) {
            if (ret < 0) {
                printf("ConvertInput: conversion wasn't successful.\n");
            } else {
                printf
                    ("ConvertInput: conversion wasn't successful. converted: %i octets.\n",
                     temp);
            }

            xmlFree(out);
            out = 0;
        } else {
            out = (unsigned char *) xmlRealloc(out, out_size + 1);
            out[out_size] = 0;  /*null terminating out */
        }
    } else {
        printf("ConvertInput: no mem\n");
    }

    return out;
}

/**
 * testXmlwriterDoc:
 * @file: the output file
 *
 * test the xmlWriter interface when creating a new document
 */
void PIM::SavePimToXml(vector<component_t*> &pim, const string file)
{
    int rc;
    xmlTextWriterPtr writer;
    xmlChar *tmp;
    xmlDocPtr doc;


    /* Create a new XmlWriter for DOM, with no compression. */
    writer = xmlNewTextWriterDoc(&doc, 0);
    if (writer == NULL) {
        printf("SavePimToXml: Error creating the xml writer\n");
        return;
    }

    xmlTextWriterSetIndent(writer, true);

    tmp = ConvertInput("    ", DEFAULT_XML_ENCODING);
    xmlTextWriterSetIndentString(writer, tmp);

    /* Start the document with the xml default for the version,
     * encoding ISO 8859-1 and the default for the standalone
     * declaration. */
    rc = xmlTextWriterStartDocument(writer, NULL, DEFAULT_XML_ENCODING, NULL);
    if (rc < 0) {
        printf("SavePimToXml: Error at xmlTextWriterStartDocument\n");
        return;
    }

    /* Start an element named "pim". Since thist is the first
     * element, this will be the root element of the document. */
    rc = xmlTextWriterStartElement(writer, BAD_CAST "pim");
    if (rc < 0) {
        printf("SavePimToXml: Error at xmlTextWriterStartElement\n");
        return;
    }

    /* Write a comment as child of "object".
     * Please observe, that the input to the xmlTextWriter functions
     * HAS to be in UTF-8, even if the output XML is encoded
     * in iso-8859-1 */
    tmp = ConvertInput("This is the PIM of xyz architecture", DEFAULT_XML_ENCODING);
    rc = xmlTextWriterWriteComment(writer, tmp);
    if (rc < 0) {
        printf("SavePimToXml: Error at xmlTextWriterWriteComment\n");
        return;
    }
    if (tmp != NULL) xmlFree(tmp);

	for (int i=0; i < pim_model.size(); i++) {

	    /* Start an element named "component" as child of "pim". */
	    rc = xmlTextWriterStartElement(writer, BAD_CAST "component");
	    if (rc < 0) {
	        printf("SavePimToXml: Error at xmlTextWriterStartElement\n");
	        return;
	    }

	    /* Add an attribute with name "name" and value "component_t::name" to "component". */
	    rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "name", BAD_CAST pim_model[i]->name.c_str());
	    if (rc < 0) {
	        printf("SavePimToXml: Error at xmlTextWriterWriteAttribute\n");
	        return;
	    }

	    /* Write a comment as child of "component" */
	    tmp = ConvertInput(pim_model[i]->name.c_str(), DEFAULT_XML_ENCODING);
	    rc = xmlTextWriterWriteFormatComment(writer, "%s exported interface", tmp);
	    if (rc < 0) {
	        printf
	            ("SavePimToXml: Error at xmlTextWriterWriteFormatComment\n");
	        return;
	    }
	    if (tmp != NULL) xmlFree(tmp);

	    /* Write an element named "description" as child of "component". */
	    tmp = ConvertInput(pim_model[i]->description.c_str(), DEFAULT_XML_ENCODING);
	    rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "description", "%s", tmp);
	    if (rc < 0) {
	        printf
	            ("SavePimToXml: Error at xmlTextWriterWriteFormatElement\n");
	        return;
	    }
	    if (tmp != NULL) xmlFree(tmp);

		for (int j=0; j < pim_model[i]->pins.size(); j++) {

		    rc = xmlTextWriterStartElement(writer, BAD_CAST "pin");
		    if (rc < 0) {
		        printf
		            ("SavePimToXml: Error at xmlTextWriterWriteFormatElement\n");
		        return;
		    }

		    if (pim_model[i]->pins[j]->IsMutable()) {
		    	rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "isMutable", BAD_CAST "true");
		    } else {
		    	rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "isMutable", BAD_CAST "false");
		    }

		    if (rc < 0) {
		        printf("SavePimToXml: Error at xmlTextWriterWriteAttribute\n");
		        return;
		    }

		    rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "dataType", BAD_CAST pim_model[i]->pins[j]->GetDataTypeName());
		    if (rc < 0) {
		        printf("SavePimToXml: Error at xmlTextWriterWriteAttribute\n");
		        return;
		    }

			string var_name(pim_model[i]->pins[j]->GetName());
			size_t pos = var_name.find_first_of('.');
			string component_name = var_name.substr(0, pos);
			string short_var_name = var_name.substr(pos+1);

		    tmp = ConvertInput(short_var_name.c_str(), DEFAULT_XML_ENCODING);
		    rc = xmlTextWriterWriteString(writer, tmp);
		    if (rc < 0) {
		        printf
		            ("SavePimToXml: Error at xmlTextWriterWriteFormatElement\n");
		        return;
		    }
		    if (tmp != NULL) xmlFree(tmp);

		    /* Close the element named "pin". */
		    rc = xmlTextWriterEndElement(writer);
		    if (rc < 0) {
		        printf("SavePimToXml: Error at xmlTextWriterEndElement\n");
		        return;
		    }

		}

	    /* Close the element named "component". */
	    rc = xmlTextWriterEndElement(writer);
	    if (rc < 0) {
	        printf("SavePimToXml: Error at xmlTextWriterEndElement\n");
	        return;
	    }

	}


    /* Here we could close the elements "component" and "pim" using the
     * function xmlTextWriterEndElement, but since we do not want to
     * write any other elements, we simply call xmlTextWriterEndDocument,
     * which will do all the work. */

    /* Close the element named "pim".
     * we simply call xmlTextWriterEndDocument, which close all elements and the document
     */

    rc = xmlTextWriterEndDocument(writer);
    if (rc < 0) {
        printf("SavePimToXml: Error at xmlTextWriterEndDocument\n");
        return;
    }

    xmlFreeTextWriter(writer);

    xmlSaveFileEnc(file.c_str(), doc, DEFAULT_XML_ENCODING);

    xmlFreeDoc(doc);
}


// **************************************************************

void PIM::LoadPimFile() {

	stringstream pimfile;

	pimfile << filename;

	LoadPimFromXml(pim_model, filename);

}

void PIM::ParseComponent (xmlDocPtr doc, xmlNodePtr componentNode, component_t *component) {

	if ((xmlStrcmp(componentNode->name, (const xmlChar *)"component"))) {
		cerr << "Error: Can't parse " << componentNode->name << endl;
		return;
	}

	std::list<VariableBase *> lst;

	fSimulator->GetRegisters(lst);

	component->name = string((char*) xmlGetProp(componentNode, (const xmlChar *)"name"));

	xmlNodePtr 	componentChild = componentNode->xmlChildrenNode;
	while (componentChild != NULL) {
		if (!xmlStrcmp(componentChild->name, (const xmlChar *)"pin")) {

			string onePinName = string((char*) xmlNodeListGetString(doc, componentChild->xmlChildrenNode, 1));

			onePinName = component->name + "." + onePinName;

			for (std::list<VariableBase *>::iterator it = lst.begin(); it != lst.end(); it++) {
				if (strcmp(((VariableBase *) *it)->GetName(), onePinName.c_str()) == 0) {
					component->pins.push_back((VariableBase *) *it);

					break;
				}
			}


		} else if ((!xmlStrcmp(componentChild->name, (const xmlChar *)"description"))) {
			component->description = (char*) xmlNodeListGetString(doc, componentChild->xmlChildrenNode, 1);
		}

		componentChild = componentChild->next;
	}

	return;
}

int PIM::LoadPimFromXml(vector<component_t*> &pim, const string filename) {

	const char *path = "//component";

	xmlDocPtr doc = NULL;
	xmlXPathContextPtr context = NULL;
	xmlXPathObjectPtr xmlobject;
	int result = 0;

	doc = xmlParseFile (GetSimulator()->SearchSharedDataFile(filename.c_str()).c_str());
	if (!doc)
	{
		cerr << __FILE__ << ":" << __LINE__ << " Could not parse the document" << endl;
		return (0);
	}

	xmlXPathInit ();
	context = xmlXPathNewContext (doc);

	xmlobject = xmlXPathEval ((xmlChar *) path, context);
	xmlXPathFreeContext (context);
	if ((xmlobject->type == XPATH_NODESET) && (xmlobject->nodesetval))
	{
		if (xmlobject->nodesetval->nodeNr)
		{
			result = xmlobject->nodesetval->nodeNr;

			xmlNodePtr node;
			component_t *component;
			for (int i=0; i<xmlobject->nodesetval->nodeNr; i++) {
				component = new component_t();
				node = xmlobject->nodesetval->nodeTab[i];
				ParseComponent (doc, node, component);
				pim_model.push_back(component);
			}
		}
	}

	xmlXPathFreeObject (xmlobject);
	xmlFreeDoc(doc);

	return (result);
}


} // end pim
} // end service
} // end unisim 



