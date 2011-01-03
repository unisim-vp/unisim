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

SocketServerThread::SocketServerThread(char* host, uint16_t port, bool _blocking, uint8_t connection_req_nb) :
	SocketThread(host, port, _blocking)
{
	request_nbre = connection_req_nb;

	struct sockaddr_in serv_addr;

	primary_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (primary_sockfd < 0) {
		error("ERROR opening socket");
	}

	memset(&serv_addr, 0, sizeof(struct sockaddr_in));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(hostport);
	serv_addr.sin_addr.s_addr = hostname;
	if (bind(primary_sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		int array[] = {primary_sockfd};
        error(array, "ERROR on binding");
	}

	/* listen()
	 * Prepare to accept connections on socket FD.
	 * N connection requests will be queued before further requests are refused.
	 * Returns 0 on success, -1 for errors.  */

	if (listen(primary_sockfd,request_nbre) < 0) {
		int array[] = {primary_sockfd};
		error(array, "listen failed");
	}

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

    do {
        sockfd = accept(primary_sockfd, (struct sockaddr *) &cli_addr, &cli_addr_len);
        if (sockfd >= 0) {

// *** This option is used to disable the Nagle TCP algorithm (disable buffering) ***
			int opt = 1;
			if (setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, (char*)&opt, sizeof(opt)) < 0) {
				int array[] = {sockfd};
				error(array, "setsockopt <IPPROTO_TCP, TCP_NODELAY> failed");
			}

#ifdef WIN32

			u_long NonBlock = 1;
			if(ioctlsocket(sockfd, FIONBIO, &NonBlock) != 0) {
				int array[] = {sockfd};
				error(array, "ioctlsocket <FIONBIO, NonBlock> failed");
			}

#else

			int flags = fcntl(sockfd, F_GETFL, 0);
			if (flags < 0)	{
				int array[] = {sockfd};
				error(array, "fcntl <F_GETFL> failed");
			}

			if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) < 0) {
				int array[] = {sockfd};
				error(array, "fcntl <F_SETFL, flags | O_NONBLOCK> failed");
			}

#endif

			if (bindHandler(sockfd)) {
        		connected++;
        	}
        }
    } while (connected < nbHandlers);

}

bool SocketServerThread::bindHandler(int sockfd) {

	if (!send_packet("WHO", true)) {
		cerr << "SocketServerThread:: unable to send <WHO>" << endl;
		return false;
	}

	char* protocol = receive_packet(true);

	SocketThread *target = protocolHandlers->at(0);
	bool found = false;
	if (target->getProtocol().compare(protocol) == 0) {
		if (!send_packet("ACK", true)) {
			cerr << "SocketServerThread:: unable to send <ACK for protocol>" << endl;
			return false;
		}

		found = true;
	} else {
		if (!send_packet("NACK", true)) {
			cerr << "SocketServerThread:: unable to send <NACK for protocol>" << endl;
			return false;
		}
	}

	char* ack = receive_packet(true);

	if (found) {
		target->Start(sockfd, blocking);
	}

}


} // network 
} // end pim 
} // end service 
} // end unisim 


