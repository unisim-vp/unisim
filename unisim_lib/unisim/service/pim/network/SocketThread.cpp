/*
 * SocketThread.cpp
 *
 *  Created on: 27 avr. 2010
 *      Author: rnouacer
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <fstream>

#ifdef WIN32

#include <winsock2.h>

#else

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <sys/times.h>
#include <fcntl.h>

#endif

#include "SocketThread.hpp"

namespace unisim {
namespace service {
namespace pim {
namespace network {

uint32_t SocketThread::name_resolve(char *host_name)
{
	struct in_addr addr;
	struct hostent *host_ent;

	if ((addr.s_addr=inet_addr(host_name))==(unsigned)-1) {
		host_ent=gethostbyname(host_name);

		if(host_ent==NULL) return(-1);

		memcpy(host_ent->h_addr, (char *)&addr.s_addr, host_ent->h_length);
	}

	return (addr.s_addr);
}

SocketThread::SocketThread(char* host, uint16_t port) {
	hostname = name_resolve(host);
	hostport = port;
}

SocketThread::SocketThread() {

}

SocketThread::~SocketThread() {

	if (reader) { reader->stop(); reader->join(); delete reader; }
	if (writer) { writer->stop(); writer->join(); delete writer; }

	if (sockfd) {
#ifdef WIN32
		closesocket(sockfd);
#else
		close(sockfd);
#endif
	}

}

void SocketThread::Start(int sockfd) {

	this->sockfd = sockfd;

	writer = new SocketWriter(sockfd);
	writer->start();

	reader = new SocketReader(sockfd);
	reader->start();

	this->start();
}

void SocketThread::send(const char* data) {

	writer->send(data);
}

char* SocketThread::receive() {

	return reader->receive();
}

} // network 
} // end pim 
} // end service 
} // end unisim 



