/*
 * SocketClientThread.cpp
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

#include "SocketClientThread.hpp"

namespace unisim {
namespace service {
namespace pim {
namespace network {

void SocketClientThread::Run() {

	struct sockaddr_in serv_addr;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		error("ERROR opening socket");
	}

	memset(&serv_addr, 0, sizeof(struct sockaddr_in));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(hostport);
	serv_addr.sin_addr.s_addr = hostname;
	int err;
	do {
		err = connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
		if (err < 0) {
			sleep(1);
		}
	} while (err < 0);

//	if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
//		int array[] = {sockfd};
//        error(array, "ERROR on Connecting");
//    }

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

	writer = new SocketWriter(sockfd);
	writer->start();

	reader = new SocketReader(sockfd);
	reader->start();

}

} // network 
} // end pim 
} // end service 
} // end unisim 



