/*
 *  Copyright (c) 2010,
 *  Commissariat a l'Energie Atomique (CEA)
 *  All rights reserved.
 *
 * Authors: Reda   Nouacer  (reda.nouacer@cea.fr)
 */

#ifndef PIM_HH_
#define PIM_HH_

#include <stdint.h>
#include <string.h>
#include <iostream>
#include <sstream>
#include <string>
#include <stdexcept>

#include <libxml/xmlmemory.h>
#include <libxml/xpath.h>
#include <libxml/parser.h>
#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>

#include <unisim/kernel/service/service.hh>

#include <unisim/service/pim/convert.hh>

#include <unisim/service/pim/network/GenericThread.hpp>
#include <unisim/service/pim/network/SocketServerThread.hpp>
#include <unisim/service/pim/network/SocketClientThread.hpp>


// 127.0.0.1 is the default localhost-name
#define DEFAULT_HOST	"127.0.0.1"
#define DEFAULT_XML_ENCODING "ISO-8859-1"	// UTF-8 , ISO-8859-1

namespace unisim {
namespace service {
namespace pim {

using namespace std;

using unisim::kernel::service::Object;
using unisim::kernel::service::Parameter;
using unisim::kernel::service::Simulator;
using unisim::kernel::service::VariableBase;

using unisim::service::pim::network::SocketThread;
using unisim::service::pim::network::GenericThread;
using unisim::service::pim::network::SocketServerThread;
using unisim::service::pim::network::SocketClientThread;

class component_t { // VirtualComponent Hierarchical (getChldren() != null)
public:
	~component_t() {

		for (int i=0; i < pins.size(); i++) {
			if (pins[i]) { delete pins[i]; pins[i] = NULL; }
		}
		pins.clear();
	}

	string				name;
	string				description;
	vector<VariableBase*>		pins;
};

class PIM : public Object, public GenericThread
{
public:

	PIM(const char *name, Simulator *simulator, uint16_t port, char* host = DEFAULT_HOST, Object *parent = 0);

	~PIM();
	virtual bool Setup();
	virtual void Run();
	void GeneratePimFile();
	void LoadPimFile();
	void getAllVariables(vector<VariableBase*> *variables);

private:
	Simulator *fSimulator;
	uint16_t fPort;
	char* fHost;

	vector<component_t*> pim_model;
	vector<VariableBase*> simulator_variables;

	string				filename;
	Parameter<string>	param_filename;

	void ParseComponent (xmlDocPtr doc, xmlNodePtr cur, component_t *component);
	int LoadPimFromXml(vector<component_t*> &pim, const string filename);
	component_t* FindComponent(const string name);

	void GetExportedVariables(vector<component_t*> &pim);
	xmlChar *ConvertInput(const char *in, const char *encoding);
	void SavePimToXml(vector<component_t*> &pim, const string filename);

	SocketServerThread *socketfd;
//	SocketClientThread *socketfd;

	GenericThread *target;

};

class TargetThread : public GenericThread {
public:
	TargetThread(char* _name, vector<VariableBase*> *variables, SocketThread *fd);

	virtual void Run();

private:
	char* name;
	vector<VariableBase*> *fVariables;
	SocketThread *socketfd;
};


} // end pim
} // end service
} // end unisim 

#endif // PIM_HH_



