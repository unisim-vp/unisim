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

//#ifdef WIN32
//#define write(fd, buf, len)	send(fd, buf, len, 0)
//#define read(fd, buf, len)	recv(fd, buf, len, 0)
//#endif /* _WIN32 */

class SocketThread: public GenericThread {
public:

	// Protocol list "NONE", "GDB", "PIM"

	SocketThread(string host, uint16_t port, bool _blocking);
	SocketThread();

	~SocketThread();

	void Start(int sockfd, bool _blocking);

	virtual void Run() { };
	virtual bool send_packet(const char* data, bool blocking);
	virtual string getProtocol() { return "NONE"; }

	virtual bool receive_packet(string& s, bool blocking);
//	virtual bool GetPacket(string& s, bool blocking);
	bool GetChar(char& c, bool blocking);
//	bool GetChar(char& c, bool blocking);


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
