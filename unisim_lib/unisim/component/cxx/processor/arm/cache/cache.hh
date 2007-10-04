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
#include "unisim/kernel/service/service.hh"
#include "unisim/service/interfaces/logger.hh"
#include "unisim/service/interfaces/memory.hh"

#include "unisim/component/cxx/processor/arm/cache/types.hh"
#include "unisim/component/cxx/processor/arm/cache_interface.hh"
#include "unisim/component/cxx/processor/arm/cache/set.hh"

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace arm {
namespace cache {

using namespace std;
using unisim::kernel::service::Service;
using unisim::kernel::service::Client;
using unisim::kernel::service::Parameter;
using unisim::kernel::service::ServiceImport;
using unisim::kernel::service::ServiceExport;
using unisim::service::interfaces::Memory;
using unisim::service::interfaces::Logger;
    
template <class CONFIG>
class Cache : 
	public CacheInterface<typename CONFIG::address_t>,
	public Service<Memory<typename CONFIG::address_t> >,
	public Client<Memory<typename CONFIG::address_t> >,
	public Client<Logger> {
public:
	typedef typename CONFIG::address_t address_t;

	Cache(const char* name,
			CacheInterface<address_t> *_next_mem_level,
			Object* parent = 0);

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
	virtual void PrWrite(address_t addr, const void *buffer, uint32_t size);
	virtual void PrRead(address_t addr, void *buffer, uint32_t size);
	
	// Cache -> Memory Interface (debugg dervice)
	virtual bool ReadMemory(address_t addr, void *buffer, uint32_t size);
	virtual bool WriteMemory(address_t addr, const void *buffer, uint32_t size);
	
	ServiceExport<Memory<address_t> > memory_export;
	ServiceImport<Memory<address_t> > memory_import;
	ServiceImport<Logger> logger_import;
	//   ServiceImport<CachePowerEstimatorInterface> power_estimator_import;
	//   ServiceImport<PowerModeInterface> power_mode_import;
	//   ServiceExport<PowerModeInterface> power_mode_export;

private:
	bool        enabled;
	uint32_t       lock;
	uint32_t lock_index;
	
	CacheInterface<address_t> *next_mem_level;

	/* Cache blocks */
	Set<CONFIG> cache[CONFIG::NSETS];

	void DecodeAddress(address_t addr, address_t& tag, address_t& index, address_t& offset);
	
	uint32_t GetCacheSize();
	uint32_t GetCacheAssociativity();
	uint32_t GetCacheLineSize();
	void Enable();
	void Disable();
	bool IsEnabled();

	void LoadDataFromMem(address_t add, void* buffer, uint32_t size);
	void StoreDataInMem(address_t add, const void* buffer, uint32_t size);

protected:
	/* verbose options */
	bool verbose_all;
	Parameter<bool> param_verbose_all;
};

} // end of namespace cache
} // end of namespace arm
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim
    
#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_CACHE_CACHE_HH__
