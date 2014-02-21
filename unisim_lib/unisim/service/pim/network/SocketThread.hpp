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

	SocketThread(string host, uint16_t port);
	SocketThread();

	virtual ~SocketThread();

	void startSocketThread(int sockfd);

	virtual void run() { };
	void closeSockfd();

	bool GetChar(char& c, bool blocking);
	bool GetPacket(string& s, bool blocking);
	bool PutChar(char c);
	bool PutPacket(const string& data);
	bool OutputText(const char *s, int count);
	bool FlushOutput();

//	bool canWrite() {
//		fd_set read_flags;
//
//	    /* Initialize and set `readset` and `maxfd` */
//
//	    struct timeval waitd;
//	    waitd.tv_sec  = 0;
//	    waitd.tv_usec = 0;
//	    FD_ZERO(&read_flags);
//
//		// Set the sockets read flag, so when select is called it examines
//		// the read status of available data.
//		FD_SET(sockfd, &read_flags);
//
//		// Now call select
//		if (select(sockfd+1, &read_flags, NULL, NULL, &waitd) == 0)
//		{
//		    return true;
//		}
//		else
//		{
//		    return false;
//		}
//	}

	bool GetPacketWithAck(string& s, bool blocking, bool Acknowledgment);
	bool PutPacketWithAck(const string& data, bool Acknowledgment);
	bool OutputTextWithAck(const char *s, int count, bool Acknowledgment);

	void setSockfd(int sockfd);
	int getSockfd() { return (sockfd); }
	void waitConnection();
	void lockSocket() { pthread_mutex_lock( &sockfd_mutex ); }
	void unlockSocket() { pthread_mutex_unlock( &sockfd_mutex ); }

protected:

	uint32_t hostname;
	uint16_t hostport;
	int sockfd;

	/*
	 *  this routine converts the address into an internet ip
	 *
	 *  e.g. in_addr_t serv_addr = name_resolve("127.0.0.1");
	 */
	uint32_t name_resolve(const char *host_name);

	pthread_mutex_t sockfd_mutex;
	pthread_mutex_t sockfd_condition_mutex;
	pthread_cond_t  sockfd_condition_cond;

	pthread_mutexattr_t Attr;

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
