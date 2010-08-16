/*
 * SyncExample.h
 *
 *  Created on: 10 févr. 2010
 *      Author: rnouacer
 */

#ifndef GENERIC_THREAD_H_
#define GENERIC_THREAD_H_

#include <stdio.h>
#include <stdlib.h>
#include<iostream>
#include<pthread.h>

namespace unisim {
namespace service {
namespace pim {
namespace network {


using namespace std;

void* executer(void* param);

class TObject
{
public:
	/**
	 *  This method has to be overloaded.
	 *  It implement the behavior of the thread
	 */
	virtual void Run(){}
	virtual void error(const char* msg);
	virtual void error(const int* fd, const char* msg);

};

class GenericThread : public TObject
{
public:

	~GenericThread() { if (!isTerminated()) stop(); }
	virtual void start() { ret=pthread_create(&thid,NULL,executer,(void*)this); terminated = false; }
	virtual void join() { pthread_join(thid,NULL); }
	virtual void stop() { terminated = true; }
	virtual bool isTerminated() { return terminated; }

protected:
	bool terminated;
	typedef GenericThread super;

private:
	pthread_t thid;
	int ret;
};

} // network 
} // end pim 
} // end service 
} // end unisim 

#endif /* GENERIC_THREAD_H_ */

