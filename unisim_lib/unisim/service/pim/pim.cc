
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
	if (initiator) { delete initiator; initiator = NULL; }

	if (serverSockfd) { delete serverSockfd; serverSockfd = NULL; }
	if (clientSockfd) { delete clientSockfd; clientSockfd = NULL; }

	for (int i=0; i < pim_model.size(); i++) {
		if (pim_model[i]) { delete pim_model[i]; pim_model[i] = NULL;}
	}
	pim_model.clear();
	input_variables.clear();
	output_variables.clear();
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
			pin_t *onePin = new pin_t();
			char* onePinName = (char*) xmlNodeListGetString(doc, componentChild->xmlChildrenNode, 1);

			onePin->name = component->name + "." + onePinName;

			if (strcmp((char*) xmlGetProp(componentChild, (const xmlChar *)"isMutable"), "false") == 0) {
				onePin->isMutable = false;
			} else {
				onePin->isMutable = true;
			}

			onePin->type = (char*) xmlGetProp(componentChild, (const xmlChar *)"type");

			for (std::list<VariableBase *>::iterator it = lst.begin(); it != lst.end(); it++) {
				if (strcmp(((VariableBase *) *it)->GetName(), onePin->name.c_str()) == 0) {
					onePin->var = (VariableBase *) *it;
					break;
				}
			}

			component->pins.push_back(onePin);

		} else if ((!xmlStrcmp(componentChild->name, (const xmlChar *)"description"))) {
			component->description = (char*) xmlNodeListGetString(doc, componentChild->xmlChildrenNode, 1);
		} else if ((!xmlStrcmp(componentChild->name, (const xmlChar *)"network"))) {
			xmlNodePtr networkChild = componentChild->xmlChildrenNode;
			if ((!xmlStrcmp(networkChild->name, (const xmlChar *)"socket"))) {
				component->host = (char*) xmlGetProp(networkChild, (const xmlChar *)"host");
				component->port = atoi((char*) xmlGetProp(networkChild, (const xmlChar *)"port"));
			}

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

void PIM::GetExportedVariables(vector<component_t*> &pim) {

	// Explore Simulator Variables to Generate PIM XML File
	std::list<VariableBase *> lst;

	fSimulator->GetRegisters(lst);

	for (std::list<VariableBase *>::iterator it = lst.begin(); it != lst.end(); it++) {

		if (!((VariableBase *) *it)->IsVisible()) continue;

		string var_name(((VariableBase *) *it)->GetName());
		size_t pos = var_name.find_first_of('.');
		string component_name = var_name.substr(0, pos);
		string short_var_name = var_name.substr(pos+1);

		component_t* component = FindComponent(component_name);
		if (component == NULL) {
			component = new component_t();

			component->name = component_name;
			component->description = "bla bla";
			component->host = "127.0.0.1";
			component->port = 1234;
			component->network = "socket";

			pim.push_back(component);
		}

		pin_t *onePin = new pin_t();
		onePin->name = short_var_name;
		onePin->type = ((VariableBase *) *it)->GetDataTypeName();
		onePin->isMutable = ((VariableBase *) *it)->IsMutable();
		onePin->var = (VariableBase *) *it;

		component->pins.push_back(onePin);

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

// *************************************************************

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

    /* Start an element named "object". Since thist is the first
     * element, this will be the root element of the document. */
    rc = xmlTextWriterStartElement(writer, BAD_CAST "object");
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

	    /* Start an element named "component" as child of "object". */
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

	    /* Start an element named "network" as child of "component". */
	    rc = xmlTextWriterStartElement(writer, BAD_CAST "network");
	    if (rc < 0) {
	        printf("SavePimToXml: Error at xmlTextWriterStartElement\n");
	        return;
	    }

	    if (pim_model[i]->network.compare("socket") == 0) {
		    rc = xmlTextWriterStartElement(writer, BAD_CAST pim_model[i]->network.c_str());
		    if (rc < 0) {
		        printf
		            ("SavePimToXml: Error at xmlTextWriterWriteFormatElement\n");
		        return;
		    }

		    rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "host", BAD_CAST pim_model[i]->host.c_str());
		    if (rc < 0) {
		        printf("SavePimToXml: Error at xmlTextWriterWriteAttribute\n");
		        return;
		    }

		    stringstream buffer;
		    buffer << pim_model[i]->port;
		    rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "port", BAD_CAST buffer.str().c_str());
		    if (rc < 0) {
		        printf("SavePimToXml: Error at xmlTextWriterWriteAttribute\n");
		        return;
		    }
		    buffer.str(std::string());

		    /* Close the element named "socket". */
		    rc = xmlTextWriterEndElement(writer);
		    if (rc < 0) {
		        printf("SavePimToXml: Error at xmlTextWriterEndElement\n");
		        return;
		    }

	    }

	    /* Close the element named "network". */
	    rc = xmlTextWriterEndElement(writer);
	    if (rc < 0) {
	        printf("SavePimToXml: Error at xmlTextWriterEndElement\n");
	        return;
	    }

		for (int j=0; j < pim_model[i]->pins.size(); j++) {

		    rc = xmlTextWriterStartElement(writer, BAD_CAST "pin");
		    if (rc < 0) {
		        printf
		            ("SavePimToXml: Error at xmlTextWriterWriteFormatElement\n");
		        return;
		    }

		    if (pim_model[i]->pins[j]->isMutable) {
		    	rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "isMutable", BAD_CAST "true");
		    } else {
		    	rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "isMutable", BAD_CAST "false");
		    }

		    if (rc < 0) {
		        printf("SavePimToXml: Error at xmlTextWriterWriteAttribute\n");
		        return;
		    }

		    rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "type", BAD_CAST pim_model[i]->pins[j]->type.c_str());
		    if (rc < 0) {
		        printf("SavePimToXml: Error at xmlTextWriterWriteAttribute\n");
		        return;
		    }

		    tmp = ConvertInput(pim_model[i]->pins[j]->name.c_str(), DEFAULT_XML_ENCODING);
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


    /* Here we could close the elements "component" and "object" using the
     * function xmlTextWriterEndElement, but since we do not want to
     * write any other elements, we simply call xmlTextWriterEndDocument,
     * which will do all the work. */

    /* Close the element named "object".
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


// *************************************************************

bool PIM::Setup() {

/*
	stringstream pimfile;

	pimfile << fSimulator->GetSharedDirectory() << "/" << filename;
*/

	GetExportedVariables(pim_model);
/*
	SavePimToXml(pim_model, pimfile.str());


	exit(0);


	LoadPimFromXml(pim_model, filename);
*/
	for (int i=0; i < pim_model.size(); i++) {
		for (int j=0; j < pim_model[i]->pins.size(); j++) {
			cerr << "VAR : " << pim_model[i]->pins[j]->name << endl;
			if (pim_model[i]->pins[j]->isMutable) {
				input_variables.push_back(pim_model[i]->pins[j]);
			} else {
				output_variables.push_back(pim_model[i]->pins[j]);
			}
		}
	}

}

void PIM::Run() {

	// Open Socket Stream
	serverSockfd = new SocketServerThread(fHost, fPort);
//	clientSockfd = new SocketClientThread(fHost, fPort);

	serverSockfd->start();
//	clientSockfd->start();

	serverSockfd->join();
//	clientSockfd->join();

	cerr << "PIM connection success " << std::endl;

	// Start Simulation <-> ToolBox communication
//	target = new TargetThread("Target", &input_variables, clientSockfd);
	target = new TargetThread("Target", &input_variables, serverSockfd);
//	initiator = new InitiatorThread("Initiator", &output_variables, clientSockfd);
	initiator = new InitiatorThread("Initiator", &output_variables, serverSockfd);

	target->start();
	initiator->start();

	target->join();
	initiator->join();

}

TargetThread::TargetThread(char* _name, vector<pin_t*> *variables, SocketThread *fd) :
		sockfd(fd),
		name(_name),
		fVariables(variables)
{

}

void TargetThread::Run(){

	cerr << "PIM::TargetThread start RUN " << std::endl;

	while (!isTerminated()) {

		char *buffer = sockfd->receive();

		string buf_str(buffer);

		int index = buf_str.find(';');
		buf_str = buf_str.substr(0, index);

		index = buf_str.find(':');
		string name = buf_str.substr(0, index);
		buf_str = buf_str.substr(index+1);

		index = buf_str.find(':');
		string time = buf_str.substr(0, index);

		string value = buf_str.substr(index+1);

		cerr << "PIM-Target receive " << buffer << std::endl;

		for (int i=0; i < fVariables->size(); i++) {
			if (name.compare((*fVariables)[i]->var->GetName()) == 0) {

				*((*fVariables)[i]->var) = convertTo<double>(value);
				break;
			}
		}

		free(buffer);
		buffer = NULL;

	}

}

InitiatorThread::InitiatorThread(char* _name, vector<pin_t*> *variables, SocketThread *fd) :
		sockfd(fd),
		name(_name),
		fVariables(variables)
{

}

void InitiatorThread::Run(){

	double time = 0;
	uint32_t period = 1e6; // 1000 millisecond

	cerr << "PIM::InitiatorThread start RUN " << std::endl;

	while (!isTerminated()) {

//	var_name:time:value;


		for (int i=0; i < fVariables->size(); i++) {

			std::ostringstream os;
			os << (*fVariables)[i]->var->GetName() << ":";
			os << time << ":";
			double val = *((*fVariables)[i]->var);
			os << stringify(val);

//			char value[10];
//			sprintf(value,"%d", rand() % 10);
//			os << value;

			os << ";";

			std::string str = os.str();

			const char *buffer = str.c_str();

			sockfd->send(buffer);

			printf("%s send: %s\n", name, buffer);

			os.str(std::string());

		}

		usleep(period);
		time = time + period;
	}

}

} // end pim
} // end service
} // end unisim 



