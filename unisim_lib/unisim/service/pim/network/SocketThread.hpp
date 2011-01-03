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

namespace unisim {
namespace service {
namespace pim {
namespace network {

#ifdef _WIN32
#	define write(fd, buf, len)	send(fd, buf, len, 0)
#	define read(fd, buf, len)	recv(fd, buf, len, 0)
#endif /* _WIN32 */

class SocketThread: public GenericThread {
public:

	// Protocol list "NONE", "GDB", "PIM"

	SocketThread(char* host, uint16_t port, bool _blocking);
	SocketThread();

	~SocketThread();

	void Start(int sockfd, bool _blocking);

	virtual void Run() { };
	virtual bool send_packet(const char* data, bool blocking);
	virtual char* receive_packet(bool blocking);
	virtual string getProtocol() { return "NONE"; }

protected:

	uint32_t hostname;
	uint16_t hostport;
	int sockfd;
	bool blocking;

	/*
	 *  this routine converts the address into an internet ip
	 *
	 *  e.g. in_addr_t serv_addr = name_resolve("127.0.0.1");
	 */
	uint32_t name_resolve(char *host_name);

	void getChar(char& c, bool blocking);

private:
	int input_buffer_size;
	int input_buffer_index;

	char *input_buffer;

};

} // network 
} // end pim 
} // end service 
} // end unisim 

#endif /* SOCKETTHREAD_HPP_ */
