/*
 *  Copyright (c) 2007,
 *  Commissariat a l'Energie Atomique (CEA)
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without modification,
 *  are permitted provided that the following conditions are met:
 *
 *   - Redistributions of source code must retain the above copyright notice, this
 *     list of conditions and the following disclaimer.
 *
 *   - Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 *   - Neither the name of CEA nor the names of its contributors may be used to
 *     endorse or promote products derived from this software without specific prior
 *     written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 *  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *  DISCLAIMED.
 *  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 *  OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 *  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 */
 
#ifndef __FULLSYSTEM_TLM_SHAREDMEMORY_SIMPLEBUS_MESSAGE_HH__
#define __FULLSYSTEM_TLM_SHAREDMEMORY_SIMPLEBUS_MESSAGE_HH__

#include <inttypes.h>
#include "utils/garbage_collector/garbage_collector.hh"

namespace full_system {
namespace tlm {
namespace shared_memory {
namespace simple_bus {

using full_system::utils::garbage_collector::Pointer;

class BusRequest
{
private:
	bool req;
};

class BusGrant
{
public:
	bool grant;
};

class Request<class ADDRESS_TYPE>
{
public:
	enum Type {
		READ,         // Read request
		WRITE        // Write request
	};
	
	Type type;       // Request type
	ADDRESS_TYPE addr;    // Address
	unsigned int size;              // Size of bus transfer (<= DATA_SIZE)
	Pointer<uint8_t> write_data;   // Data to write into memory
};

class Response
{
public:
	Pointer<uint8_t> read_data; // Data read from memory/target processor caches
};

} // end of namespace simple_bus
} // end of namespace shared_memory
} // end of namespace tlm
} // end of namespace full_system

#endif /* __FULLSYSTEM_TLM_SHAREDMEMORY_SIMPLEBUS_MESSAGE_HH__ */
