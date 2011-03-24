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
#include "../convert.hh"
#include <unisim/kernel/debug/debug.hh>

namespace unisim {
namespace service {
namespace pim {
namespace network {

uint32_t SocketThread::name_resolve(const char *host_name)
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

SocketThread::SocketThread(string host, uint16_t port, bool _blocking) :
		GenericThread(),
		hostname(0),
		hostport(0),
		sockfd(-1),
		blocking(_blocking),
		input_buffer_size(0),
		input_buffer_index(0),
		input_buffer(NULL)


{
	hostname = name_resolve(host.c_str());
	hostport = port;

	pthread_mutex_init (&sockfd_mutex, NULL);
	pthread_mutex_init (&sockfd_condition_mutex, NULL);
	pthread_cond_init (&sockfd_condition_cond, NULL);

	input_buffer = (char*) malloc(MAXDATASIZE+1);

}

SocketThread::SocketThread() :
				GenericThread(),
				hostname(0),
				hostport(0),
				sockfd(-1),
				blocking(true),
				input_buffer_size(0),
				input_buffer_index(0),
				input_buffer(NULL)

{
	pthread_mutex_init (&sockfd_mutex, NULL);
	pthread_mutex_init (&sockfd_condition_mutex, NULL);
	pthread_cond_init (&sockfd_condition_cond, NULL);

	input_buffer = (char*) malloc(MAXDATASIZE+1);

}

SocketThread::~SocketThread() {

	if (sockfd) {
#ifdef WIN32
		closesocket(sockfd);
#else
		close(sockfd);
#endif
	}

}

void SocketThread::Start(int sockfd, bool _blocking) {

	this->blocking = _blocking;
	SetSockfd(sockfd);

	this->start();
}

void SocketThread::SetSockfd(int sockfd) {

    pthread_mutex_lock( &sockfd_mutex );

	this->sockfd = sockfd;

//    pthread_cond_signal( &sockfd_condition_cond );
    pthread_cond_broadcast( &sockfd_condition_cond );

    pthread_mutex_unlock( &sockfd_mutex );

}

void SocketThread::waitConnection() {

    pthread_mutex_lock( &sockfd_mutex );

	if (sockfd == -1) {

		pthread_cond_wait( &sockfd_condition_cond, &sockfd_mutex );

	}

	pthread_mutex_unlock( &sockfd_mutex );

}


bool SocketThread::PutChar(char c) {

	output_buffer_strm << c;

	return true;
}

bool SocketThread::PutPacket(const string& data, bool blocking) {

	int data_size = data.size();

	output_buffer_strm << '$' << data << '#';

	uint8_t checksum = 0;

	for (int pos=0; pos < data_size; pos++)
	{
		checksum += (uint8_t) data[pos];
	}

	output_buffer_strm << Nibble2HexChar(checksum >> 4) << Nibble2HexChar(checksum & 0xf);

	return true;

}

bool SocketThread::OutputText(const char *s, int count)
{
	int i;
	uint8_t packet[1 + 2 * count + 1];
	uint8_t *p = packet;

	*p = 'O';
	p++;
	for(i = 0; i < count; i++, p += 2)
	{
		p[0] = Nibble2HexChar((uint8_t) s[i] >> 4);
		p[1] = Nibble2HexChar((uint8_t) s[i] & 0xf);
	}
	*p = 0;
	return PutPacket((const char *) packet, blocking);
}

bool SocketThread::FlushOutput() {

	fd_set read_flags, write_flags;
	struct timeval waitd;

	int err;

	waitConnection();

	string output_buffer = output_buffer_strm.str();
	output_buffer_strm.str(std::string());

	int output_buffer_size = output_buffer.size();
	int index = 0;
	while (output_buffer_size > 0) {

		waitd.tv_sec = 0;
		waitd.tv_usec = 1000;

		FD_ZERO(&read_flags); // Zero the flags ready for using
		FD_ZERO(&write_flags);

		// Set the write flag to check the write status of the socket
		FD_SET(sockfd, &write_flags);

		// Now call select
		err = select(sockfd+1, &read_flags,&write_flags, (fd_set*)0,&waitd);
		if (err < 0) {
			// If select breaks then pause for 1 milliseconds and then continue
#ifdef WIN32
			Sleep(1);
#else
			usleep(1000);
#endif

			continue;
		}

		// Now select will have modified the flag sets to tell us
		// what actions can be performed

		//Check if the socket is prepared to accept data
		if (FD_ISSET(sockfd, &write_flags)) {
			FD_CLR(sockfd, &write_flags);

			int n;

#ifdef WIN32
			n = send(sockfd, &(output_buffer.at(index)), output_buffer_size, 0);
#else
			n = write(sockfd, &(output_buffer.at(index)), output_buffer_size);
#endif
			if (n <= 0) {
				int array[] = {sockfd};
				error(array, "ERROR writing to socket");
			} else {
				index += n;
				output_buffer_size -= n;
			}

		} else {

			if (blocking) {
#ifdef WIN32
				Sleep(1);
#else
				usleep(1000);
#endif
				continue;
			} else {
				break;
			}

		}

	}

	if (output_buffer_size > 0) {
		return false;
	}

	return true;
}

bool SocketThread::GetPacket(string& str, bool blocking) {

	str.clear();

	uint8_t checkSum = 0;
	int packet_size = 0;
	uint8_t pchk;
	char c;

	while (true) {
		GetChar(c, blocking);
		if (c == 0) {
			if (blocking) {
				cerr << "receive EOF " << endl;
			}
			break;
		}
    	switch(c)
    	{
    		case '+':
    			break;
    		case '-':
   				// error response => e.g. retransmission of the last packet
    			break;
    		case 3: // '\003'
    			break;
    		case '$':

    			GetChar(c, blocking);
    			while (true) {
    				str = str + c;
        			packet_size++;
    				checkSum = checkSum + c;
    				GetChar(c, blocking);

    				if (c == '#') break;
    			}

    			GetChar(c, blocking);
    			pchk = HexChar2Nibble(c) << 4;
    			GetChar(c, blocking);
    			pchk = pchk + HexChar2Nibble(c);

    			if (checkSum != pchk) {
    				cerr << "receive_packet: wrong checksum checkSum= " << checkSum << " pchk= " << pchk << endl;
    				return false;
    			} else {

    				return true;
    			}

    			break;
    		default:
    			cerr << "receive_packet: protocol error (0x" << Nibble2HexChar(c) << ":" << c << ")";
    	}

	}

	return false;

}

bool SocketThread::GetChar(char& c, bool blocking) {

	fd_set read_flags, write_flags;
	struct timeval waitd;

	int err;

	int n;

	waitConnection();

	while (input_buffer_size == 0) {
		waitd.tv_sec = 0;
		waitd.tv_usec = 1000;

		FD_ZERO(&read_flags); // Zero the flags ready for using
		FD_ZERO(&write_flags);

		// Set the sockets read flag, so when select is called it examines
		// the read status of available data.
		FD_SET(sockfd, &read_flags);

		// Now call select
		err = select(sockfd+1, &read_flags, &write_flags, (fd_set*)0,&waitd);
		if (err < 0) {
			// If select breaks then pause for 1 milliseconds and then continue
#ifdef WIN32
			Sleep(1);
#else
			usleep(1000);
#endif

			continue;
		}

		// Now select will have modified the flag sets to tell us
		// what actions can be performed

		// Check if data is available to read
		if (FD_ISSET(sockfd, &read_flags)) {
			FD_CLR(sockfd, &read_flags);

			memset(input_buffer, 0, sizeof(input_buffer));

#ifdef WIN32
			n = recv(sockfd, input_buffer, MAXDATASIZE, 0);
#else
			n = read(sockfd, input_buffer, MAXDATASIZE);
#endif
			if (n <= 0)	{
		    	int array[] = {sockfd};
		    	error(array, "ERROR reading from socket");
		    } else {
		    	input_buffer_size = n;
		    	input_buffer_index = 0;
		    }

		} else {
			if (blocking) {
#ifdef WIN32
				Sleep(1);
#else
				usleep(1000);
#endif
				continue;
			} else {
				break;
			}
		}

	}


	if (input_buffer_size > 0) {
		c = input_buffer[input_buffer_index];
		input_buffer_size--;
		input_buffer_index++;
		return true;
	} else {
		c = 0;
		return false;
	}

}

} // network 
} // end pim 
} // end service 
} // end unisim 



