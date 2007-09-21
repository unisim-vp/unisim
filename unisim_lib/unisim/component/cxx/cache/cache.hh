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
 
#ifndef __UNISIM_COMPONENT_CXX_CACHE_CACHE_HH__
#define __UNISIM_COMPONENT_CXX_CACHE_CACHE_HH__

#include <unisim/component/cxx/cache/cache_interface.hh>
#include <unisim/kernel/service/service.hh>
#include <unisim/service/interfaces/memory.hh>
#include <unisim/service/interfaces/cache_power_estimator.hh>
#include <unisim/service/interfaces/power_mode.hh>
#include <iostream>

namespace unisim {
namespace component {
namespace cxx {
namespace cache {

using namespace std;

using unisim::kernel::service::Object;
using unisim::kernel::service::Service;
using unisim::kernel::service::Client;
using unisim::kernel::service::ServiceImport;
using unisim::kernel::service::ServiceExport;
using unisim::service::interfaces::Memory;
using unisim::service::interfaces::CachePowerEstimator;
using unisim::service::interfaces::PowerMode;

typedef enum { RP_LRU, RP_PLRU } ReplacementPolicy;

typedef enum { EvPrRd = 0, EvPrRdX = 1, EvPrWr = 2, EvBusRd = 3, EvBusRdX = 4 } Event;
typedef enum { AcNone = 0, AcBusRd = 1, AcBusRdX = 2, AcWB = 3 } Action;

template <class CACHE_BLOCK, class PHYSICAL_ADDRESS, uint32_t CACHE_SIZE, uint32_t CACHE_BLOCK_SIZE, uint32_t CACHE_ASSOCIATIVITY, ReplacementPolicy REPLACEMENT_POLICY>
class Cache :
	public CacheInterface<PHYSICAL_ADDRESS>,
	public Service<Memory<PHYSICAL_ADDRESS> >,
	public Client<Memory<PHYSICAL_ADDRESS> >,
	public Client<CachePowerEstimator>,
	public Client<PowerMode>,
	public Service<PowerMode>
{
public:
	ServiceImport<Memory<PHYSICAL_ADDRESS> > to_mem;
	ServiceExport<Memory<PHYSICAL_ADDRESS> > to_cpu;
	ServiceImport<CachePowerEstimator> power_estimator_import;
	ServiceImport<PowerMode> power_mode_import;
	ServiceExport<PowerMode> power_mode_export;

	Cache(const char *name,
	      CacheInterface<PHYSICAL_ADDRESS> *next_level_cache,
	      BusInterface<PHYSICAL_ADDRESS> *bus_interface,
	      Object *parent = 0);
	virtual ~Cache();

	void Enable();
	void Disable();
	bool IsEnabled();
	void EnableAddressOnlyBroadcast();
	void DisableAddressOnlyBroadcast();
	
	/* Invalidate all the cache */
	void InvalidateSet(uint32_t index);
	void Invalidate();

	// Cache Power mode interface
	virtual void SetPowerMode(unsigned int frequency, unsigned int voltage);
	virtual unsigned int GetMaxFrequency(); // in Mhz
	virtual unsigned int GetDefaultVoltage(); // in mV

	// Memory interface
	virtual void Reset();
	virtual bool ReadMemory(PHYSICAL_ADDRESS addr, void *buffer, uint32_t size);
	virtual bool WriteMemory(PHYSICAL_ADDRESS addr, const void *buffer, uint32_t size);

	// Processor to cache interface
	virtual void PrInvalidateBlock(PHYSICAL_ADDRESS physical_addr, CacheControl cc);
	virtual void PrFlushBlock(PHYSICAL_ADDRESS physical_addr, CacheControl cc);
	virtual void PrZeroBlock(PHYSICAL_ADDRESS physical_addr, CacheControl cc);
	virtual void PrWrite(PHYSICAL_ADDRESS addr, const void *buffer, uint32_t size, CacheControl cc);
	virtual void PrRead(PHYSICAL_ADDRESS addr, void *buffer, uint32_t size, CacheControl cc);
	virtual void PrReadX(PHYSICAL_ADDRESS addr, void *buffer, uint32_t size, CacheControl cc);
	
	// Bus to cache interface
	virtual void BusInvalidateBlock(PHYSICAL_ADDRESS physical_addr);
	virtual void BusFlushBlock(PHYSICAL_ADDRESS physical_addr);
	virtual void BusRead(PHYSICAL_ADDRESS physical_addr, void *buffer, uint32_t size);
	virtual void BusReadX(PHYSICAL_ADDRESS physical_addr, void *buffer, uint32_t size);

private:
	bool enabled;
	bool address_only_broadcast_enabled;
	CacheInterface<PHYSICAL_ADDRESS> *next_level_cache;
	BusInterface<PHYSICAL_ADDRESS> *bus_interface;
	CacheControl cc;

	/* Cache blocks */
	CACHE_BLOCK cache[CACHE_SIZE / CACHE_BLOCK_SIZE / CACHE_ASSOCIATIVITY][CACHE_ASSOCIATIVITY];

	/* Most recently used blocks (LRU policy) */
	CACHE_BLOCK *mru_block[CACHE_SIZE / CACHE_BLOCK_SIZE / CACHE_ASSOCIATIVITY];
	/* Least recently used blocks (LRU policy) */
	CACHE_BLOCK *lru_block[CACHE_SIZE / CACHE_BLOCK_SIZE / CACHE_ASSOCIATIVITY];

	/* Pseudo-LRU bits */
	uint32_t plru_bits[CACHE_SIZE / CACHE_BLOCK_SIZE / CACHE_ASSOCIATIVITY];

	/* Select a block according to the replacement policy */
	inline CACHE_BLOCK *ChooseBlockToReplace(uint32_t index)
#if defined(__GNUC__) && (__GNUC__ >= 3)
	__attribute__((always_inline))
#endif
	;

	/* Initialize the replacement policy */
	void InitializeReplacementPolicy(uint32_t index);
	void InitializeReplacementPolicy();

	/* Update the replacement policy */
	void UpdateReplacementPolicy(CACHE_BLOCK *block, uint32_t index);

	/* Associative search of a block */
	inline CACHE_BLOCK *SearchBlock(uint32_t index, PHYSICAL_ADDRESS physical_addr)
#if defined(__GNUC__) && (__GNUC__ >= 3)
	__attribute__((always_inline))
#endif
	;

	/* Decode physical address */
	inline void DecodeAddress(PHYSICAL_ADDRESS addr, PHYSICAL_ADDRESS& base_physical_addr, uint32_t& index, uint32_t& offset)
#if defined(__GNUC__) && (__GNUC__ >= 3)
	__attribute__((always_inline))
#endif
	;
	
	inline void DoAction(CACHE_BLOCK *block, Event ev, CacheControl cc, CacheStatus& cs)
#if defined(__GNUC__) && (__GNUC__ >= 3)
	__attribute__((always_inline))
#endif
	;

	void ReadMemoryBlockAligned(PHYSICAL_ADDRESS addr, void *buffer, uint32_t size);
	void WriteMemoryBlockAligned(PHYSICAL_ADDRESS addr, const void *buffer, uint32_t size);

	/* Write into cache */
	inline void PrWriteBlockAligned(PHYSICAL_ADDRESS addr, const void *buffer, uint32_t size, CacheControl cc)
#if defined(__GNUC__) && (__GNUC__ >= 3)
	__attribute__((always_inline))
#endif
	;

	/* Read from cache */
	inline void PrReadBlockAligned(PHYSICAL_ADDRESS addr, void *buffer, uint32_t size, CacheControl cc)
#if defined(__GNUC__) && (__GNUC__ >= 3)
	__attribute__((always_inline))
#endif
	;
	
	/* Read from cache */
	inline void PrReadXBlockAligned(PHYSICAL_ADDRESS addr, void *buffer, uint32_t size, CacheControl cc)
#if defined(__GNUC__) && (__GNUC__ >= 3)
	__attribute__((always_inline))
#endif
	;

};

} // end of namespace cache
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
