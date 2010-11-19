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

#include <sstream>
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

SocketReader::~SocketReader() {

	if (buffer_queue) { delete buffer_queue; buffer_queue = NULL; }
};

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
	stringstream receive_buffer;

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
		    	receive_buffer << input_buffer;
		    }

		} else {
			string bstr = receive_buffer.str();
			receive_buffer.str("");
			while (bstr.size() > 0) {
				int pos = 0;
				switch (bstr[pos++]) {
					case '+': break;
					case '-': break;
					case '$': {
						int diese_index = bstr.find_first_of('#');
						char* buffer = (char *) malloc(diese_index);
						memset(buffer, 0, diese_index);
						memcpy(buffer, bstr.c_str()+1, diese_index-1);
						buffer_queue->add(buffer);

						pos = diese_index+3;

					} break;
					default : ;
				}

				if (pos < bstr.size()) {
					bstr = bstr.substr(pos);
				} else {
					bstr = "";
				}

			}
		}

	}

}

char* SocketReader::receive() {

	char* str = NULL;

	buffer_queue->next(str);

	return str;
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


