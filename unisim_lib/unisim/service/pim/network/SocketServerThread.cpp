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

SocketServerThread::SocketServerThread(string host, uint16_t port, bool _blocking, uint8_t connection_req_nb) :
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

SocketServerThread::~SocketServerThread() {

#ifdef WIN32
		closesocket(primary_sockfd);
#else
		close(primary_sockfd);
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

	fd_set savefds;
	FD_ZERO(&savefds);
	FD_SET(primary_sockfd, &savefds);

	do {

		fd_set readfds = savefds;  // this must be set before each call to select()
		timeval timeout = {1, 0};  // this must be set before each call to select()

		int sel = select(primary_sockfd + 1, &readfds, 0, 0, &timeout);

		if (sel > 0 && FD_ISSET(primary_sockfd, &readfds)) {

			// client connected
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

		}
		else if (sel == 0) {
		// timeout occurred
		}

	} while ((connected < nbHandlers) && !isTerminated());

}

bool SocketServerThread::bindHandler(int sockfd) {

	string who("WHO");
	string ack("ACK");
	string nack("NACK");

	if (!send_packet(who, true)) {
		cerr << "SocketServerThread:: unable to send <WHO>" << endl;
		return false;
	}

	string protocol;
	receive_packet(protocol, true);

	bool found = false;
	for (int i=0; i < protocolHandlers->size(); i++) {

		if ((*protocolHandlers)[i]->getProtocol().compare(protocol) == 0) {
			if (!send_packet(ack, true)) {
				cerr << "SocketServerThread:: unable to send <ACK for protocol>" << endl;
				return false;
			}

			string ack;
			receive_packet(ack, true);

			(*protocolHandlers)[i]->Start(sockfd, blocking);

			found = true;
			break;
		}

	}

	if (!found) {
		if (!send_packet(nack, true)) {
			cerr << "SocketServerThread:: unable to send <NACK for protocol>" << endl;
			return false;
		}
	}

	return true;
}


} // network 
} // end pim 
} // end service 
} // end unisim 


