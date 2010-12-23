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
#include <signal.h>
#include <errno.h>

#define MAXDATASIZE		255

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

	GenericThread() :
		thid(0), ret(0), terminated(false), started(false) {}
	~GenericThread() {
		if (!isTerminated()) stop();
		if (thid) {	pthread_detach(thid);}
	}

	virtual void start() { ret=pthread_create(&thid,NULL,executer,(void*)this); terminated = false; started = true; }
	virtual void join() { pthread_join(thid,NULL); }
	virtual void stop() { terminated = true; started = false; }
	virtual bool isTerminated() { return terminated; }
	virtual bool setTerminated(bool b) { terminated = b; }
	virtual bool isStarted() { return started; }
	virtual bool setStarted(bool b) { started = b; }
	virtual int kill() {
		if (thid) {
			int err = pthread_kill(thid, SIGKILL);
			thid = NULL;
			return err;
		}
	}

protected:
	typedef GenericThread super;

private:
	pthread_t thid;
	int ret;
	bool terminated;
	bool started;
};

} // network 
} // end pim 
} // end service 
} // end unisim 

#endif /* GENERIC_THREAD_H_ */

