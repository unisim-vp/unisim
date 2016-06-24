/*
 * SyncExample.cpp
 *
 *  Created on: 10 févr. 2010
 *      Author: rnouacer
 */

#include "GenericThread.hpp"

#include <unistd.h>

namespace unisim {
namespace service {
namespace pim {
namespace network {

void TObject::error(const char* msg) {
    perror(msg);
    exit(1);
}

void TObject::error(const int* fd, const char* msg) {

	int size = sizeof(fd) / sizeof(*fd);
	for (int i=0; i<size; i++) {
#ifdef WIN32
		closesocket(fd[i]);
#else
		close(fd[i]);
#endif
	}

    perror(msg);
    exit(1);
}

void* executer(void* param) {
   TObject *obj=(TObject*)param;
   obj->run();

   return NULL;
}

} // network 
} // end pim 
} // end service 
} // end unisim 


