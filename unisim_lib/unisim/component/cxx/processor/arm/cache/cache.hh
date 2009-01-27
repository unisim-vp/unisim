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
 *        : Adriana Carloganu (adriana.carloganu@cea.fr)
 *------------------------------------------------------------
 * Description: 
 *  Arm Cache: - do not support coherence so BusInterface should not be used (empty functions)
 *             - used two replacement policies that could be selected by software
 *             - could be enabled/disabled by software
 */
 //-----------------------------------------------------------

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_CACHE_CACHE_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_CACHE_CACHE_HH__

#include <inttypes.h>
#include "unisim/component/cxx/processor/arm/cache/types.hh"
#include "unisim/component/cxx/processor/arm/cache_interface.hh"
#include "unisim/component/cxx/processor/arm/cache/set.hh"

#ifndef SOCLIB

#include "unisim/kernel/service/service.hh"
#include "unisim/service/interfaces/memory.hh"

#endif // SOCLIB

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace arm {
namespace cache {

using namespace std;

#ifndef SOCLIB

using unisim::kernel::service::Object;
using unisim::kernel::service::Parameter;
using unisim::kernel::service::Service;
using unisim::kernel::service::Client;
using unisim::service::interfaces::Memory;

#endif // SOCLIB

template <class CONFIG>
class Cache : 
	public CacheInterfaceWithMemoryService<typename CONFIG::address_t> {
public:
	typedef typename CONFIG::address_t address_t;
	typedef CacheInterfaceWithMemoryService<address_t> inherited;

#ifdef SOCLIB
	Cache(CacheInterface<address_t> *_next_mem_level);
	
#else // SOCLIB
	
	Cache(const char* name,
			CacheInterface<address_t> *_next_mem_level,
			Object* parent = 0);
	
#endif // SOCLIB

	virtual ~Cache();

	virtual bool Setup();
	virtual void OnDisconnect();

	virtual void SetLock(uint32_t lock, uint32_t index);

	// Proccessor -> Cache Interface
	virtual void PrReset();
	virtual void PrInvalidate();
	virtual void PrInvalidateSet(uint32_t index);
	virtual void PrInvalidateBlock(address_t addr);
	virtual void PrInvalidateBlock(uint32_t index, uint32_t way);   //add for Arm Cache
	virtual void PrFlushBlock(address_t addr);
	virtual void PrFlushBlock(uint32_t index, uint32_t way);   //add for Arm Cache
	virtual void PrCleanBlock(address_t addr);
	virtual void PrCleanBlock(uint32_t index, uint32_t way);   //add for Arm Cache
	virtual void PrZeroBlock(address_t addr);
	virtual void PrWrite(address_t addr, const uint8_t *buffer, uint32_t size);
	virtual void PrRead(address_t addr, uint8_t *buffer, uint32_t size);
	
#ifndef SOCLIB
	
	// Cache -> Memory Interface (debug device)
	virtual void Reset();
	virtual bool ReadMemory(uint64_t addr, void *buffer, uint32_t size);
	virtual bool WriteMemory(uint64_t addr, const void *buffer, uint32_t size);
	
#endif // SOCLIB
	

private:
	bool        enabled;
	uint32_t       lock;
	uint32_t lock_index;
	
	CacheInterface<address_t> *next_mem_level;

	/* Cache blocks */
	Set<CONFIG> cache[CONFIG::NSETS];

	void DecodeAddress(address_t addr, address_t& tag, address_t& index, address_t& offset);
	address_t CodeAddress(address_t tag, address_t index);
	
	uint32_t GetCacheSize();
	uint32_t GetCacheAssociativity();
	uint32_t GetCacheLineSize();
	void Enable();
	void Disable();
	bool IsEnabled();

	void LoadDataFromMem(address_t add, uint8_t* buffer, uint32_t size);
	void StoreDataInMem(address_t add, const uint8_t* buffer, uint32_t size);

protected:
	/* verbose options */
	bool verbose_all;
	bool verbose_pr_read;
	bool verbose_pr_write;
	bool verbose_read_memory;
	bool verbose_write_memory;
	
#ifndef SOCLIB
	
	Parameter<bool> param_verbose_all;
	Parameter<bool> param_verbose_pr_read;
	Parameter<bool> param_verbose_pr_write;
	Parameter<bool> param_verbose_read_memory;
	Parameter<bool> param_verbose_write_memory;
	
#endif // SOCLIB
	
	bool VerbosePrRead();
	bool VerbosePrWrite();
	bool VerboseReadMemory();
	bool VerboseWriteMemory();
	bool HasVerbose();
};

} // end of namespace cache
} // end of namespace arm
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim
    
#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_CACHE_CACHE_HH__
