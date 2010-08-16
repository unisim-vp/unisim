/*
 * SocketReader.cpp
 *
 *  Created on: 23 avr. 2010
 *      Author: rnouacer
 */

#include "assert.h"
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

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

#include "SocketReader.hpp"

namespace unisim {
namespace service {
namespace pim {
namespace network {

SocketReader::SocketReader(const int sock) {
	assert(sock >= 0);
	sockfd = sock;
	buffer_queue = new BlockingQueue<char *>();
};

SocketReader::~SocketReader() { };

/*
 * FD_ZERO(sockfd, &write_flags)	sets all associated flags in the socket to 0
 * FD_SET (sockfd, &write_flags)	used to set a socket for checking
 * FD_CLR (sockfd, &write_flags)	used to clear a socket from being checked
 * FD_ISSET(sockfd, &write_flags)	used to query as to if the socket is ready for reading or writing.
 */

void SocketReader::Run() {

	fd_set read_flags, write_flags;
	struct timeval waitd;

	int err;
	int n;

	terminated = false;

	while (!terminated) {
		waitd.tv_sec = 0;
		waitd.tv_usec = 1000;

		FD_ZERO(&read_flags); // Zero the flags ready for using
		FD_ZERO(&write_flags);

		// Set the sockets read flag, so when select is called it examines
		// the read status of available data.
		FD_SET(sockfd, &read_flags);

		// Now call select
		err = select(sockfd+1, &read_flags, &write_flags, (fd_set*)0,&waitd);
		if (err < 0) { // If select breaks then pause for 1 seconds
			sleep(1); // then continue
			continue;
		}

		// Now select will have modified the flag sets to tell us
		// what actions can be performed

		// Check if data is available to read
		if (FD_ISSET(sockfd, &read_flags)) {
			FD_CLR(sockfd, &read_flags);

			char *input_buffer = new char[MAXDATASIZE+1];
			memset(input_buffer, 0, MAXDATASIZE+1);

		    n = read(sockfd, input_buffer, MAXDATASIZE);

		    if (n < 0) {
		    	int array[] = {sockfd};
		    	error(array, "ERROR reading from socket");
		    } else if (n > 0) {
		    	buffer_queue->add(input_buffer);
		    }

		}

	}

}

char* SocketReader::receive() {

	char* str = NULL;
	char* buffer = NULL;

	buffer_queue->next(str);

	if (str != NULL) {

		int str_size = strlen(str);

		buffer = (char *) malloc(str_size+1);
		memset(buffer, 0, str_size+1);
		memcpy(buffer, str, str_size);

		free(str);
		str = NULL;
	}

	return buffer;
}


void SocketReader::stop() {
	super::stop();

	if (buffer_queue) {
		delete buffer_queue;
		buffer_queue = NULL;
	}

}

} // network 
} // end pim 
} // end service 
} // end unisim 


