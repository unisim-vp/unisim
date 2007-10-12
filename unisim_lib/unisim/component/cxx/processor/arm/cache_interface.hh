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
 * Authors: Gilles Mouchard (gilles.mouchard@cea.fr)
 *          Adriana Carloganu (adriana.carloganu@cea.fr)
 *          Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 */
 
#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_CACHE_INTERFACE_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_CACHE_INTERFACE_HH__

#include <inttypes.h>

#include "unisim/kernel/service/service.hh"
#include "unisim/service/interfaces/logger.hh"
#include "unisim/service/interfaces/memory.hh"


// Physical addressed cache connection
// +-------------+                             +----------+                             +---------+                       +---------+
// |             |                             |          |  ProcessorToCacheInterface  |         |  BusInterface         |         |
// |             |                             |          |---------------------------->|         |---------------------->|         |
// |  Processor  |  ProcessorToCacheInterface  |   L1     |                             |   L2    |                       |         |
// |    Core     |---------------------------->|  Cache   |    BusInterface             |  Cache  |  BusInterface         |   Bus   |
// |             |                             |          |<----------------------------|         |<----------------------|         |
// |             |                             |          |                             |         |                       |         |
// +-------------+                             +----------+                             +---------+                       +---------+

//-----------------------------------------------------------

// Virtual addressed cache connection
// +-------------+                             +----------+         +---------+         +---------+                       +---------+
// |             |                             |          |  PtoCI  |         |  PtoCI  |         |  BusInterface         |         |
// |             |                             |          |-------->|MMU VA-PA|-------->|         |---------------------->|         |
// |  Processor  |  ProcessorToCacheInterface  |   L1     |         +---------+         |   L2    |                       |         |
// |    Core     |---------------------------->|  Cache   |    BusToCacheInterface      |  Cache  |  BusToCacheInterface  |   Bus   |
// |             |    (PtoCI)                  |          |<----------------------------|         |<----------------------|         |
// |             |                             |          |                             |         |                       |         |
// +-------------+                             +----------+                             +---------+                       +---------+

//-----------------------------------------------------------


namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace arm {

using unisim::service::interfaces::Memory;
using unisim::service::interfaces::Logger;

using unisim::kernel::service::Object;
using unisim::kernel::service::Service;
using unisim::kernel::service::Client;
using unisim::kernel::service::ServiceImport;
using unisim::kernel::service::ServiceExport;
using unisim::service::interfaces::Memory;
using unisim::service::interfaces::Logger;

template <class address_t>
class CacheInterface {
public:
	virtual ~CacheInterface() {}

	// invalidate without checking if modified data
	virtual void PrInvalidateBlock(uint32_t set, uint32_t way) = 0;

	// write modified data into memory then invalidate all copies in caches
	virtual void PrFlushBlock(uint32_t set, uint32_t way) = 0;

	// write modified data into memory, do not invalidate block
	virtual void PrCleanBlock(uint32_t set, uint32_t way) = 0;

	virtual void PrReset()   = 0;
	// invalidate without checking if modified data
	virtual void PrInvalidate() = 0;
	virtual void PrInvalidateSet(uint32_t set) = 0;
	virtual void PrInvalidateBlock(address_t addr) = 0;

	// write modified data into memory then invalidate all copies in caches
	virtual void PrFlushBlock(address_t addr) = 0;

	// write modified data into memory, do not invalidate block
	virtual void PrCleanBlock(address_t addr) = 0;           //add for Arm Cache

	virtual void PrZeroBlock(address_t addr) = 0;

	virtual void PrWrite(address_t addr, const void *buffer, uint32_t size) = 0;
	virtual void PrRead(address_t addr, void *buffer, uint32_t size) = 0;
};

template <class address_t>
class CacheInterfaceWithMemoryService :
	public CacheInterface<address_t>,
	public Service<Memory<address_t> >,
	public Client<Memory<address_t> >,
	public Client<Logger> {
public:
	CacheInterfaceWithMemoryService(const char *name,
			Object *parent) :
		Object(name, parent),
		Service<Memory<address_t> >(name, parent),
		Client<Memory<address_t> >(name, parent),
		Client<Logger>(name, parent),
		memory_export("memory_export", this),
		memory_import("memory_import", this),
		logger_import("logger_import", this) {}

	virtual ~CacheInterfaceWithMemoryService() {}
	
	virtual void SetLock(uint32_t lock, uint32_t set) = 0;

	virtual uint32_t GetCacheSize()= 0;
	virtual uint32_t GetCacheAssociativity() = 0;
	virtual uint32_t GetCacheLineSize() = 0;

	virtual void Enable()    = 0;
	virtual void Disable()   = 0;
	virtual bool IsEnabled() = 0;
	
	ServiceExport<Memory<address_t> > memory_export;
	ServiceImport<Memory<address_t> > memory_import;
	ServiceImport<Logger> logger_import;

//	virtual void Reset() = 0;
//	virtual bool ReadMemory(address_t addr, void *buffer, uint32_t size) = 0;
//	virtual bool WriteMemory(address_t addr, const void *buffer, uint32_t size) = 0;
};

} // end of namespace arm
} // end of namespace processor
} // end of namespace cxx 
} // end of namespace component
} // end of namespace unisim 

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_CACHE_INTERFACE_HH__
