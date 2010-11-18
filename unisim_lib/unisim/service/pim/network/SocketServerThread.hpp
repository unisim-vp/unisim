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

	SocketServerThread(char* host, uint16_t port, uint8_t connection_req_nb = 1);

	virtual void Run();
	int getLastSockfd() { return newsockfd; }

private:
	uint8_t request_nbre;
	int newsockfd;
};

} // network 
} // end pim 
} // end service 
} // end unisim 

#endif /* SOCKETSERVERTHREAD_HPP_ */
