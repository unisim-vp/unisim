/*
 * SocketThread.cpp
 *
 *  Created on: 23 avr. 2010
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

#include "SocketServerThread.hpp"

namespace unisim {
namespace service {
namespace pim {
namespace network {

SocketServerThread::SocketServerThread(char* host, uint16_t port, uint8_t connection_req_nb) :
	SocketThread(host, port)
{
	request_nbre = connection_req_nb;

	struct sockaddr_in serv_addr;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		error("ERROR opening socket");
	}

	memset(&serv_addr, 0, sizeof(struct sockaddr_in));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(hostport);
	serv_addr.sin_addr.s_addr = hostname;
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		int array[] = {sockfd};
        error(array, "ERROR on binding");
	}

	/* listen()
	 * Prepare to accept connections on socket FD.
	 * N connection requests will be queued before further requests are refused.
	 * Returns 0 on success, -1 for errors.  */

	if (listen(sockfd,request_nbre) < 0) {
		int array[] = {sockfd};
		error(array, "listen failed");
	}

#ifdef WIN32

	u_long NonBlock = 1;
	if(ioctlsocket(sockfd, FIONBIO, &NonBlock) != 0) {
		int array[] = {sockfd};
		error(array, "ioctlsocket failed");
	}

#else

	int flags = fcntl(sockfd, F_GETFL, 0);
	if (flags < 0)	{
		int array[] = {sockfd};
		error(array, "fcntl failed");
	}

	if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) < 0) {
		int array[] = {sockfd};
		error(array, "fcntl failed");
	}

#endif

}

void SocketServerThread::Run() {

#ifdef WIN32
		int cli_addr_len;
#else
		socklen_t cli_addr_len;
#endif

	int nbHandlers = protocolHandlers->size();
	int connected = 0;
	struct sockaddr_in cli_addr;

    cli_addr_len = sizeof(cli_addr);

	int newsockfd;
    do {
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &cli_addr_len);
        if (newsockfd >= 0) {

        	SocketThread *target = protocolHandlers->at(connected++);
        	target->Start(newsockfd);
        }
    } while (connected < nbHandlers);

}

} // network 
} // end pim 
} // end service 
} // end unisim 


