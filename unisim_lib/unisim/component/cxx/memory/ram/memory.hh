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
 *          Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 */
 
#ifndef __UNISIM_COMPONENT_CXX_MEMORY_RAM_MEMORY_HH__
#define __UNISIM_COMPONENT_CXX_MEMORY_RAM_MEMORY_HH__

#include <inttypes.h>
#include "unisim/service/interfaces/memory.hh"
#include "unisim/util/hash_table/hash_table.hh"
#include "unisim/kernel/service/service.hh"

namespace unisim {
namespace component {
namespace cxx {
namespace memory {
namespace ram {

using unisim::util::hash_table::HashTable;
using unisim::kernel::service::Object;
using unisim::kernel::service::Service;
using unisim::kernel::service::Client;
using unisim::kernel::service::ServiceExport;
using unisim::kernel::service::Parameter;
using unisim::kernel::service::Statistic;

/* forward declaration */
template <class PHYSICAL_ADDR, uint32_t PAGE_SIZE> class Memory;

template <class PHYSICAL_ADDR, uint32_t PAGE_SIZE = 1024 * 1024>
class MemoryPage
{
public:
	MemoryPage(PHYSICAL_ADDR _key);
	~MemoryPage();
private:
	friend class HashTable<PHYSICAL_ADDR, MemoryPage<PHYSICAL_ADDR, PAGE_SIZE> >;
	friend class Memory<PHYSICAL_ADDR, PAGE_SIZE>;
	PHYSICAL_ADDR key;
	MemoryPage<PHYSICAL_ADDR, PAGE_SIZE> *next;
	uint8_t *storage;
};

template <class PHYSICAL_ADDR, uint32_t PAGE_SIZE = 1024 * 1024>
class Memory : public Service<unisim::service::interfaces::Memory<PHYSICAL_ADDR> >
{
public:
	/* exported services */
	ServiceExport<unisim::service::interfaces::Memory<PHYSICAL_ADDR> > memory_export;
	
	Memory(const  char *name, Object *parent = 0); //, PHYSICAL_ADDR org, uint64_t bytesize);
	virtual ~Memory();
	
	/* service methods */
	virtual void OnDisconnect();
	virtual bool Setup();
	
	/* unisim::service::interfaces::Memory methods */
	virtual void Reset();
	virtual bool WriteMemory(PHYSICAL_ADDR physical_addr, const void *buffer, uint32_t size);
	virtual bool ReadMemory(PHYSICAL_ADDR physical_addr, void *buffer, uint32_t size);
	bool WriteMemory(PHYSICAL_ADDR physical_addr, const void *buffer, uint32_t size, const uint8_t *byte_enable, uint32_t byte_enable_length, uint32_t streaming_width);
	bool ReadMemory(PHYSICAL_ADDR physical_addr, void *buffer, uint32_t size, const uint8_t *byte_enable, uint32_t byte_enable_length, uint32_t streaming_width);
	void *GetDirectAccess(PHYSICAL_ADDR physical_addr, PHYSICAL_ADDR& physical_start_addr, PHYSICAL_ADDR& physical_end_addr);
protected:
	PHYSICAL_ADDR org;
	PHYSICAL_ADDR bytesize;
	PHYSICAL_ADDR lo_addr;
	PHYSICAL_ADDR hi_addr;
	PHYSICAL_ADDR memory_usage;
	
private:
	HashTable<PHYSICAL_ADDR, MemoryPage<PHYSICAL_ADDR, PAGE_SIZE> > hash_table;
	
	Parameter<PHYSICAL_ADDR> param_org;
	Parameter<PHYSICAL_ADDR> param_bytesize;
	Statistic<PHYSICAL_ADDR> stat_memory_usage;
};

} // end of namespace ram
} // end of namespace memory
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim 

#endif //__UNISIM_COMPONENT_CXX_MEMORY_RAM_MEMORY_HH__
