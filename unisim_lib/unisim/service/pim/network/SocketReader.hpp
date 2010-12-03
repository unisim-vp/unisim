/*
 * SocketReader.hpp
 *
 *  Created on: 23 avr. 2010
 *      Author: rnouacer
 */

#ifndef SOCKETREADER_HPP_
#define SOCKETREADER_HPP_

#include "BlockingQueue.hpp"
#include "BlockingQueue.tcc"
#include "GenericThread.hpp"

#define MAXDATASIZE		255

namespace unisim {
namespace service {
namespace pim {
namespace network {

class SocketReader: public GenericThread {
public:

	SocketReader(const int sock, bool _blocking);
	~SocketReader();

	virtual void Run();
	virtual char* receive();
	virtual void stop();

protected:

private:
	int sockfd;
	BlockingQueue<char *> *buffer_queue;
	bool blocking;
	int input_buffer_size;
	int input_buffer_index;
	char input_buffer[MAXDATASIZE+1];

	char getChar();

};

} // network 
} // end pim 
} // end service 
} // end unisim 

#endif /* SOCKETREADER_HPP_ */

