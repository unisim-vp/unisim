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

	SocketReader(const int sock);
	~SocketReader();

	virtual void Run();
	virtual char* receive();
	virtual void stop();

protected:

private:
	int sockfd;
	BlockingQueue<char *> *buffer_queue;
};

} // network 
} // end pim 
} // end service 
} // end unisim 

#endif /* SOCKETREADER_HPP_ */

