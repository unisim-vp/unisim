/*
 * SocketThread.hpp
 *
 *  Created on: 23 avr. 2010
 *      Author: rnouacer
 */

#ifndef SOCKETSERVERTHREAD_HPP_
#define SOCKETSERVERTHREAD_HPP_


#include "SocketThread.hpp"

namespace unisim {
namespace service {
namespace pim {
namespace network {

class SocketServerThread: public SocketThread {
public:

	SocketServerThread(char* host, uint16_t port, bool _blocking, uint8_t connection_req_nb);

	virtual void Run();

	void setProtocolHandlers(vector<SocketThread*> *protocolHandlers) { this->protocolHandlers = protocolHandlers; }

private:
	uint8_t request_nbre;
	vector<SocketThread*> *protocolHandlers;

	bool bindHandler(int newsockfd);
};

} // network 
} // end pim 
} // end service 
} // end unisim 

#endif /* SOCKETSERVERTHREAD_HPP_ */
