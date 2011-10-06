/*
 * SocketClientThread.hpp
 *
 *  Created on: 27 avr. 2010
 *      Author: rnouacer
 */

#ifndef SOCKETCLIENTTHREAD_HPP_
#define SOCKETCLIENTTHREAD_HPP_


#include "SocketThread.hpp"

namespace unisim {
namespace service {
namespace pim {
namespace network {

class SocketClientThread: public SocketThread {
public:

	SocketClientThread(string host, uint16_t port, bool _blocking) :
			SocketThread(host, port, _blocking) { };

	virtual void Run();

};

} // network 
} // end pim 
} // end service 
} // end unisim 

#endif /* SOCKETCLIENTTHREAD_HPP_ */
