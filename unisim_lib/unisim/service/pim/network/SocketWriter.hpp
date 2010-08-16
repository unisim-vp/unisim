/*
 * SocketWriter.hpp
 *
 *  Created on: 23 avr. 2010
 *      Author: rnouacer
 */

#ifndef SOCKETWRITER_HPP_
#define SOCKETWRITER_HPP_


#include "BlockingQueue.hpp"
#include "BlockingQueue.tcc"
#include "GenericThread.hpp"

#define MAXDATASIZE		255

namespace unisim {
namespace service {
namespace pim {
namespace network {

class SocketWriter: public GenericThread {
public:

	SocketWriter(const int sock);
	~SocketWriter();

	virtual void Run();
	virtual void send(const char* data);
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

#endif /* SOCKETWRITER_HPP_ */

