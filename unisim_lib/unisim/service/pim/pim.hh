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

#include <unisim/kernel/service/service.hh>

#include <unisim/service/pim/convert.hh>

#include <unisim/service/pim/network/GenericThread.hpp>
#include <unisim/service/pim/network/SocketThread.hpp>
#include <unisim/service/pim/network/SocketServerThread.hpp>
#include <unisim/service/pim/network/SocketClientThread.hpp>
#include <unisim/service/pim/pim_thread.hh>

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

		pins.clear();
	}

	string				name;
	string				description;
	vector<VariableBase*>		pins;
};

class PIM : public Object, public GenericThread
{
public:

	PIM(const char *name, Object *parent = 0);

	~PIM();
	virtual bool Setup();
	virtual void run();
	void generatePimFile();
	int loadPimFile();
	void getAllVariables(vector<VariableBase*> *variables);

private:

	uint16_t fPort;
	string fHost;

	vector<component_t*> pim_model;

	Parameter<uint16_t> param_tcp_port;
	Parameter<string> param_host;

	string				filename;
	Parameter<string>	param_filename;

	component_t* findComponent(const string name);

	SocketServerThread *socketfd;
//	SocketClientThread *socketfd;

	SocketThread *target;

};

} // end pim
} // end service
} // end unisim 

#endif // PIM_HH_



