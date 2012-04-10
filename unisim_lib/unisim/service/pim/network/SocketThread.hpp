/*
 * SocketThread.hpp
 *
 *  Created on: 27 avr. 2010
 *      Author: rnouacer
 */

#ifndef SOCKETTHREAD_HPP_
#define SOCKETTHREAD_HPP_

#include <stdint.h>
#include <sstream>

#include "GenericThread.hpp"

namespace unisim {
namespace service {
namespace pim {
namespace network {

class SocketThread: public GenericThread {
public:

	SocketThread(string host, uint16_t port, bool _blocking);
	SocketThread();

	virtual ~SocketThread();

	void startSocketThread(int sockfd, bool _blocking);

	virtual void run() { };

	bool GetChar(char& c, bool blocking);
	virtual bool GetPacket(string& s, bool blocking);

	bool PutChar(char c);
	virtual bool PutPacket(const string& data, bool blocking);
	bool OutputText(const char *s, int count);
	bool FlushOutput();

	void setSockfd(int sockfd);
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

	std::stringstream output_buffer_strm;

	void init();
};

} // network 
} // end pim 
} // end service 
} // end unisim 

#endif /* SOCKETTHREAD_HPP_ */
