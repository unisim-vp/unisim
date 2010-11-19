/*
 * SocketThread.hpp
 *
 *  Created on: 27 avr. 2010
 *      Author: rnouacer
 */

#ifndef SOCKETTHREAD_HPP_
#define SOCKETTHREAD_HPP_

#include <stdint.h>

#include "GenericThread.hpp"
#include "SocketReader.hpp"
#include "SocketWriter.hpp"

namespace unisim {
namespace service {
namespace pim {
namespace network {

class SocketThread: public GenericThread {
public:

	// Protocol list "NONE", "GDB", "PIM"

	SocketThread(char* host, uint16_t port);
	SocketThread();

	~SocketThread();

	void Start(int sockfd);

	virtual void Run() { };
	virtual void send(const char* data);
	virtual char* receive();
	virtual string getProtocol() { return "NONE"; }

protected:

	uint32_t hostname;
	uint16_t hostport;
	int sockfd;

	SocketReader *reader;
	SocketWriter *writer;

	/*
	 *  this routine converts the address into an internet ip
	 *
	 *  e.g. in_addr_t serv_addr = name_resolve("127.0.0.1");
	 */
	uint32_t name_resolve(char *host_name);

};

} // network 
} // end pim 
} // end service 
} // end unisim 

#endif /* SOCKETTHREAD_HPP_ */
