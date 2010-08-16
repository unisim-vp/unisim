/*
 * BlockingQueue.cpp
 *
 *  Created on: 26 avr. 2010
 *      Author: rnouacer
 */

#ifndef BLOCKINGQUEUE_TCC_
#define BLOCKINGQUEUE_TCC_

#include "BlockingQueue.hpp"

namespace unisim {
namespace service {
namespace pim {
namespace network {


template <class T>
BlockingQueue<T>::BlockingQueue() {
	pthread_mutex_init (&queue_mutex, NULL);
	pthread_mutex_init (&condition_mutex, NULL);
	pthread_cond_init (&condition_cond, NULL);
	alive = true;
}

template <class T>
BlockingQueue<T>::~BlockingQueue() {

	alive = false;
	// wake-up waiting thread
	pthread_cond_signal( &condition_cond );
};

template <class T>
void BlockingQueue<T>::add(T data) {

	if (!alive) return;

    pthread_mutex_lock( &condition_mutex );

    pthread_mutex_lock( &queue_mutex );
    buffer_queue.push(data);
    pthread_mutex_unlock( &queue_mutex );

    pthread_cond_signal( &condition_cond );

    pthread_mutex_unlock( &condition_mutex );

}

template <class T>
void BlockingQueue<T>::next(T& data) {

	pthread_mutex_lock( &condition_mutex );
	while( isEmpty() && alive)
	{
		pthread_cond_wait( &condition_cond, &condition_mutex );
	}

	if (alive) {
	    pthread_mutex_lock( &queue_mutex );
		data = buffer_queue.front();
		buffer_queue.pop();
	    pthread_mutex_unlock( &queue_mutex );
	}

	pthread_mutex_unlock( &condition_mutex );

}

template <class T>
bool BlockingQueue<T>::isEmpty() {

	bool result = false;

    pthread_mutex_lock( &queue_mutex );
    result = buffer_queue.empty();
    pthread_mutex_unlock( &queue_mutex );

    return result;
}

template <class T>
size_t BlockingQueue<T>::size() {

	size_t size = 0;

    pthread_mutex_lock( &queue_mutex );
    size = buffer_queue.size();
    pthread_mutex_unlock( &queue_mutex );

    return size;
}

} // network 
} // end pim 
} // end service 
} // end unisim 

#endif // BLOCKINGQUEUE_TCC_

