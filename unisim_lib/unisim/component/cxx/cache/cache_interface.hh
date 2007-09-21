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
 */
 
#ifndef __UNISIM_COMPONENT_CXX_CACHE_CACHE_INTERFACE_HH__
#define __UNISIM_COMPONENT_CXX_CACHE_CACHE_INTERFACE_HH__

#include <inttypes.h>

namespace unisim {
namespace component {
namespace cxx {
namespace cache {

// +-------------+                             +----------+                             +---------+                       +---------+
// |             |                             |          |  ProcessorToCacheInterface  |         |  BusInterface         |         |
// |             |                             |          |---------------------------->|         |---------------------->|         |
// |  Processor  |  ProcessorToCacheInterface  |   L1     |                             |   L2    |                       |         |
// |    Core     |---------------------------->|  Cache   |    BusToCacheInterface      |  Cache  |  BusToCacheInterface  |   Bus   |
// |             |                             |          |<----------------------------|         |<----------------------|         |
// |             |                             |          |                             |         |                       |         |
// +-------------+                             +----------+                             +---------+                       +---------+

// Cache control
typedef enum {
	CC_NONE = 0,                       // Default behavior (i.e. Write back, snooping disabled in other processors)
	CC_WRITE_THROUGH = 8,              // Write through access (i.e. writes go to both cache and memory)
	CC_INHIBITED = 4,                  // Cache is inhibited
	CC_MEMORY_COHERENCY_ENFORCED = 2,  // Memory coherency enforced (i.e. snooping enabled in other processors)
} CacheControl;

template <class PHYSICAL_ADDR>
class CacheInterface
{
public:
	virtual void PrInvalidateBlock(PHYSICAL_ADDR physical_addr, CacheControl cc) = 0;
	virtual void PrFlushBlock(PHYSICAL_ADDR physical_addr, CacheControl cc) = 0;
	virtual void PrZeroBlock(PHYSICAL_ADDR physical_addr, CacheControl cc) = 0;
	virtual void PrWrite(PHYSICAL_ADDR physical_addr, const void *buffer, uint32_t size, CacheControl cc) = 0;
	virtual void PrRead(PHYSICAL_ADDR physical_addr, void *buffer, uint32_t size, CacheControl cc) = 0;
	virtual void PrReadX(PHYSICAL_ADDR physical_addr, void *buffer, uint32_t size, CacheControl cc) = 0;
	virtual void BusInvalidateBlock(PHYSICAL_ADDR physical_addr) = 0;
	virtual void BusFlushBlock(PHYSICAL_ADDR physical_addr) = 0;
	virtual void BusRead(PHYSICAL_ADDR physical_addr, void *buffer, uint32_t size) = 0;
	virtual void BusReadX(PHYSICAL_ADDR physical_addr, void *buffer, uint32_t size) = 0;
};

typedef enum { BC_NONE = 0, BC_GLOBAL = 1 } BusControl;
typedef enum { BS_OK = 0, BS_ERROR = 1 } BusStatus;
typedef enum { CS_MISS = 0, CS_HIT = 1, CS_SHARED = 2, CS_MODIFIED = 4, CS_BUSY = 8 } CacheStatus;


template <class PHYSICAL_ADDR>
class BusInterface
{
public:
	BusInterface(){}
	virtual ~BusInterface(){}
	virtual void Reset() = 0;
	virtual void BusRead(PHYSICAL_ADDR physical_addr, void *buffer, uint32_t size, BusControl bc, CacheStatus& cs) = 0;
	virtual void BusReadX(PHYSICAL_ADDR physical_addr, void *buffer, uint32_t size, BusControl bc, CacheStatus& cs) = 0;
	virtual void BusWrite(PHYSICAL_ADDR physical_addr, const void *buffer, uint32_t size, BusControl bc) = 0;
	virtual void BusInvalidateBlock(PHYSICAL_ADDR physical_addr, BusControl bc) = 0;
	virtual void BusFlushBlock(PHYSICAL_ADDR physical_addr, BusControl bc) = 0;
	virtual void BusZeroBlock(PHYSICAL_ADDR physical_addr, BusControl bc) = 0;
	virtual void BusInvalidateTLBEntry(PHYSICAL_ADDR addr, BusControl bc) = 0;
	virtual void BusSetCacheStatus(CacheStatus cs) = 0;
};

template <class PHYSICAL_ADDR>
class DeviceInterface
{
public:
	DeviceInterface(){}
	virtual ~DeviceInterface(){}
	virtual void Reset() = 0;
	virtual BusStatus DevRead(PHYSICAL_ADDR physical_addr, void *buffer, uint32_t size, BusControl bc) = 0;
	virtual BusStatus DevReadX(PHYSICAL_ADDR physical_addr, void *buffer, uint32_t size, BusControl bc) = 0;
	virtual BusStatus DevWrite(PHYSICAL_ADDR physical_addr, const void *buffer, uint32_t size, BusControl bc) = 0;
	virtual BusStatus DevInvalidateBlock(PHYSICAL_ADDR physical_addr, BusControl bc) = 0;
	virtual BusStatus DevFlushBlock(PHYSICAL_ADDR physical_addr, BusControl bc) = 0;
	virtual BusStatus DevZeroBlock(PHYSICAL_ADDR physical_addr, BusControl bc) = 0;
	virtual BusStatus DevInvalidateTLBEntry(PHYSICAL_ADDR addr, BusControl bc) = 0;
	virtual void DevOnBusError(BusStatus bs) = 0;
};


} // end of namespace cache
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
