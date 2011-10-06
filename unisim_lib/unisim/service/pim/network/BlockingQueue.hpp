/*
 * BlockingQueue.hpp
 *
 *  Created on: 26 avr. 2010
 *      Author: rnouacer
 */

#ifndef BLOCKINGQUEUE_HPP_
#define BLOCKINGQUEUE_HPP_

#include<pthread.h>
#include <queue>

namespace unisim {
namespace service {
namespace pim {
namespace network {

template <class T>
class BlockingQueue {
public:
	typedef T QUEUE_DATATYPE;

	BlockingQueue();
	~BlockingQueue();

	virtual void add(T);
	virtual void next(T&);
	virtual bool isEmpty();
	virtual size_t size();
	void lock() { pthread_mutex_lock( &queue_mutex ); };
	void unlock() { pthread_mutex_unlock( &queue_mutex ); };
	std::queue<T> getQueue() { return buffer_queue; }

protected:

	pthread_mutex_t queue_mutex;
	pthread_mutex_t condition_mutex;
	pthread_cond_t  condition_cond;

private:
	std::queue<T> buffer_queue;
	bool alive;
};


} // network 
} // end pim 
} // end service 
} // end unisim 

#endif /* BLOCKINGQUEUE_HPP_ */

