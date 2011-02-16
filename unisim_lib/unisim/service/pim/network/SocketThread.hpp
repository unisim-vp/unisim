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

class SocketThread: public GenericThread {
public:

	// Protocol list "NONE", "GDB", "PIM"

	SocketThread(string host, uint16_t port, bool _blocking);
	SocketThread();

	~SocketThread();

	void Start(int sockfd, bool _blocking);

	virtual void Run() { };
	virtual string getProtocol() { return "NONE"; }

	virtual bool send_packet(string& data, bool blocking);

	virtual bool receive_packet(string& s, bool blocking);

	bool GetChar(char& c, bool blocking);


	void SetSockfd(int sockfd);
	void waitConnection();

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
	uint32_t name_resolve(const char *host_name);

	pthread_mutex_t sockfd_mutex;
	pthread_mutex_t sockfd_condition_mutex;
	pthread_cond_t  sockfd_condition_cond;

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
