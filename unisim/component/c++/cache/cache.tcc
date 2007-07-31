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
 
#ifndef __UNISIM_COMPONENT_CXX_CACHE_CACHE_TCC__
#define __UNISIM_COMPONENT_CXX_CACHE_CACHE_TCC__

namespace unisim {
namespace component {
namespace cxx {
namespace cache {

template <class CACHE_BLOCK, class PHYSICAL_ADDRESS, uint32_t CACHE_SIZE, uint32_t CACHE_BLOCK_SIZE, uint32_t CACHE_ASSOCIATIVITY, ReplacementPolicy REPLACEMENT_POLICY>
Cache<CACHE_BLOCK, PHYSICAL_ADDRESS, CACHE_SIZE, CACHE_BLOCK_SIZE, CACHE_ASSOCIATIVITY, REPLACEMENT_POLICY>::Cache(const char *name, CacheInterface<PHYSICAL_ADDRESS> *_next_level_cache, BusInterface<PHYSICAL_ADDRESS> *_bus_interface, Object *parent) :
	Object(name, parent),
	Service<Memory<PHYSICAL_ADDRESS> >(name, parent),
	Client<Memory<PHYSICAL_ADDRESS> >(name, parent),
	Client<CachePowerEstimator>(name, parent),
	Client<PowerMode>(name, parent),
	Service<PowerMode>(name, parent),
	to_mem("to-mem", this),
	to_cpu("to-cpu", this),
	power_estimator_import("power-estimator-import", this),
	power_mode_import("power-mode-import", this),
	power_mode_export("power-mode-export", this),
	next_level_cache(_next_level_cache),
	bus_interface(_bus_interface),
	enabled(false),
	address_only_broadcast_enabled(false)
{
	Reset();
}

template <class CACHE_BLOCK, class PHYSICAL_ADDRESS, uint32_t CACHE_SIZE, uint32_t CACHE_BLOCK_SIZE, uint32_t CACHE_ASSOCIATIVITY, ReplacementPolicy REPLACEMENT_POLICY>
Cache<CACHE_BLOCK, PHYSICAL_ADDRESS, CACHE_SIZE, CACHE_BLOCK_SIZE, CACHE_ASSOCIATIVITY, REPLACEMENT_POLICY>::~Cache()
{
}

template <class CACHE_BLOCK, class PHYSICAL_ADDRESS, uint32_t CACHE_SIZE, uint32_t CACHE_BLOCK_SIZE, uint32_t CACHE_ASSOCIATIVITY, ReplacementPolicy REPLACEMENT_POLICY>
void Cache<CACHE_BLOCK, PHYSICAL_ADDRESS, CACHE_SIZE, CACHE_BLOCK_SIZE, CACHE_ASSOCIATIVITY, REPLACEMENT_POLICY>::SetPowerMode(unsigned int frequency, unsigned int voltage)
{
	if(power_mode_import) power_mode_import->SetPowerMode(frequency, voltage);
}

template <class CACHE_BLOCK, class PHYSICAL_ADDRESS, uint32_t CACHE_SIZE, uint32_t CACHE_BLOCK_SIZE, uint32_t CACHE_ASSOCIATIVITY, ReplacementPolicy REPLACEMENT_POLICY>
unsigned int Cache<CACHE_BLOCK, PHYSICAL_ADDRESS, CACHE_SIZE, CACHE_BLOCK_SIZE, CACHE_ASSOCIATIVITY, REPLACEMENT_POLICY>::GetMaxFrequency()
{
	return power_mode_import ? power_mode_import->GetMaxFrequency() : 0;
}
template <class CACHE_BLOCK, class PHYSICAL_ADDRESS, uint32_t CACHE_SIZE, uint32_t CACHE_BLOCK_SIZE, uint32_t CACHE_ASSOCIATIVITY, ReplacementPolicy REPLACEMENT_POLICY>
unsigned int Cache<CACHE_BLOCK, PHYSICAL_ADDRESS, CACHE_SIZE, CACHE_BLOCK_SIZE, CACHE_ASSOCIATIVITY, REPLACEMENT_POLICY>::GetDefaultVoltage()
{
	return power_mode_import ? power_mode_import->GetDefaultVoltage() : 0;
}

template <class CACHE_BLOCK, class PHYSICAL_ADDRESS, uint32_t CACHE_SIZE, uint32_t CACHE_BLOCK_SIZE, uint32_t CACHE_ASSOCIATIVITY, ReplacementPolicy REPLACEMENT_POLICY>
void Cache<CACHE_BLOCK, PHYSICAL_ADDRESS, CACHE_SIZE, CACHE_BLOCK_SIZE, CACHE_ASSOCIATIVITY, REPLACEMENT_POLICY>::Enable()
{
	enabled = true;
}

template <class CACHE_BLOCK, class PHYSICAL_ADDRESS, uint32_t CACHE_SIZE, uint32_t CACHE_BLOCK_SIZE, uint32_t CACHE_ASSOCIATIVITY, ReplacementPolicy REPLACEMENT_POLICY>
void Cache<CACHE_BLOCK, PHYSICAL_ADDRESS, CACHE_SIZE, CACHE_BLOCK_SIZE, CACHE_ASSOCIATIVITY, REPLACEMENT_POLICY>::Disable()
{
	enabled = false;
}

template <class CACHE_BLOCK, class PHYSICAL_ADDRESS, uint32_t CACHE_SIZE, uint32_t CACHE_BLOCK_SIZE, uint32_t CACHE_ASSOCIATIVITY, ReplacementPolicy REPLACEMENT_POLICY>
void Cache<CACHE_BLOCK, PHYSICAL_ADDRESS, CACHE_SIZE, CACHE_BLOCK_SIZE, CACHE_ASSOCIATIVITY, REPLACEMENT_POLICY>::EnableAddressOnlyBroadcast()
{
	address_only_broadcast_enabled = true;
}

template <class CACHE_BLOCK, class PHYSICAL_ADDRESS, uint32_t CACHE_SIZE, uint32_t CACHE_BLOCK_SIZE, uint32_t CACHE_ASSOCIATIVITY, ReplacementPolicy REPLACEMENT_POLICY>
void Cache<CACHE_BLOCK, PHYSICAL_ADDRESS, CACHE_SIZE, CACHE_BLOCK_SIZE, CACHE_ASSOCIATIVITY, REPLACEMENT_POLICY>::DisableAddressOnlyBroadcast()
{
	address_only_broadcast_enabled = false;
}

template <class CACHE_BLOCK, class PHYSICAL_ADDRESS, uint32_t CACHE_SIZE, uint32_t CACHE_BLOCK_SIZE, uint32_t CACHE_ASSOCIATIVITY, ReplacementPolicy REPLACEMENT_POLICY>
bool Cache<CACHE_BLOCK, PHYSICAL_ADDRESS, CACHE_SIZE, CACHE_BLOCK_SIZE, CACHE_ASSOCIATIVITY, REPLACEMENT_POLICY>::IsEnabled()
{
	return enabled;
}

template <class CACHE_BLOCK, class PHYSICAL_ADDRESS, uint32_t CACHE_SIZE, uint32_t CACHE_BLOCK_SIZE, uint32_t CACHE_ASSOCIATIVITY, ReplacementPolicy REPLACEMENT_POLICY>
void Cache<CACHE_BLOCK, PHYSICAL_ADDRESS, CACHE_SIZE, CACHE_BLOCK_SIZE, CACHE_ASSOCIATIVITY, REPLACEMENT_POLICY>::Reset()
{
	enabled = false;
	address_only_broadcast_enabled = false;
	Invalidate();
}

template <class CACHE_BLOCK, class PHYSICAL_ADDRESS, uint32_t CACHE_SIZE, uint32_t CACHE_BLOCK_SIZE, uint32_t CACHE_ASSOCIATIVITY, ReplacementPolicy REPLACEMENT_POLICY>
bool Cache<CACHE_BLOCK, PHYSICAL_ADDRESS, CACHE_SIZE, CACHE_BLOCK_SIZE, CACHE_ASSOCIATIVITY, REPLACEMENT_POLICY>::ReadMemory(PHYSICAL_ADDRESS physical_addr, void *buffer, uint32_t size)
{
	uint32_t read_offset = 0;

	do
	{
		uint32_t offset = physical_addr & (CACHE_BLOCK_SIZE - 1);
		uint32_t size_to_block_boundary = CACHE_BLOCK_SIZE - offset;
		if(size_to_block_boundary >= size)
		{
			// Memory access does not cross a cache block boundary
			ReadMemoryBlockAligned(physical_addr, (uint8_t *) buffer + read_offset, size);
			size = 0;
		}
		else
		{
			// Memory access crosses a cache block boundary
			ReadMemoryBlockAligned(physical_addr, (uint8_t *) buffer + read_offset, size_to_block_boundary);
			size -= size_to_block_boundary;
			read_offset += size_to_block_boundary;
			physical_addr += size_to_block_boundary;
		}
	} while(size > 0);
	return true;
}

template <class CACHE_BLOCK, class PHYSICAL_ADDRESS, uint32_t CACHE_SIZE, uint32_t CACHE_BLOCK_SIZE, uint32_t CACHE_ASSOCIATIVITY, ReplacementPolicy REPLACEMENT_POLICY>
bool Cache<CACHE_BLOCK, PHYSICAL_ADDRESS, CACHE_SIZE, CACHE_BLOCK_SIZE, CACHE_ASSOCIATIVITY, REPLACEMENT_POLICY>::WriteMemory(PHYSICAL_ADDRESS physical_addr, const void *buffer, uint32_t size)
{
	uint32_t write_offset = 0;

	do
	{
		uint32_t offset = physical_addr & (CACHE_BLOCK_SIZE - 1);
		uint32_t size_to_block_boundary = CACHE_BLOCK_SIZE - offset;
		if(size_to_block_boundary >= size)
		{
			// Memory access does not cross a cache block boundary
			WriteMemoryBlockAligned(physical_addr, (uint8_t *) buffer + write_offset, size);
			size = 0;
		}
		else
		{
			// Memory access crosses a cache block boundary
			WriteMemoryBlockAligned(physical_addr, (uint8_t *) buffer + write_offset, size_to_block_boundary);
			size -= size_to_block_boundary;
			write_offset += size_to_block_boundary;
			physical_addr += size_to_block_boundary;
		}
	} while(size > 0);
	return true;
}

template <class CACHE_BLOCK, class PHYSICAL_ADDRESS, uint32_t CACHE_SIZE, uint32_t CACHE_BLOCK_SIZE, uint32_t CACHE_ASSOCIATIVITY, ReplacementPolicy REPLACEMENT_POLICY>
void Cache<CACHE_BLOCK, PHYSICAL_ADDRESS, CACHE_SIZE, CACHE_BLOCK_SIZE, CACHE_ASSOCIATIVITY, REPLACEMENT_POLICY>::ReadMemoryBlockAligned(PHYSICAL_ADDRESS physical_addr, void *buffer, uint32_t size)
{
	PHYSICAL_ADDRESS base_physical_addr;
	uint32_t index, offset;
	CACHE_BLOCK *block;
	
	if(to_mem)
	{
		// Read the cache block from the next level cache
		to_mem->ReadMemory(physical_addr, buffer, size);
	}

	// Is the cache enabled ?
	if(enabled)
	{
		// Decode the physical address
		DecodeAddress(physical_addr, base_physical_addr, index, offset);
	
		// search for the block in the set
		block = SearchBlock(index, base_physical_addr);

		if(block)
		{
			// Hit
			if(block->IsModified())
			{
				// read data from the block
				block->Read(offset, buffer, size);
			}
		}
	}
}

template <class CACHE_BLOCK, class PHYSICAL_ADDRESS, uint32_t CACHE_SIZE, uint32_t CACHE_BLOCK_SIZE, uint32_t CACHE_ASSOCIATIVITY, ReplacementPolicy REPLACEMENT_POLICY>
void Cache<CACHE_BLOCK, PHYSICAL_ADDRESS, CACHE_SIZE, CACHE_BLOCK_SIZE, CACHE_ASSOCIATIVITY, REPLACEMENT_POLICY>::WriteMemoryBlockAligned(PHYSICAL_ADDRESS physical_addr, const void *buffer, uint32_t size)
{
	PHYSICAL_ADDRESS base_physical_addr;
	uint32_t index, offset;
	CACHE_BLOCK *block;
	
	if(to_mem)
	{
		// Write the cache block from the next level cache
		to_mem->WriteMemory(physical_addr, buffer, size);
	}

	// Is the cache enabled ?
	if(enabled)
	{
		// Decode the physical address
		DecodeAddress(physical_addr, base_physical_addr, index, offset);
	
		// search for the block in the set
		block = SearchBlock(index, base_physical_addr);

		if(block)
		{
			// Hit
			// write data into the block
			block->Write(offset, buffer, size);
		}
	}
}



template <class CACHE_BLOCK, class PHYSICAL_ADDRESS, uint32_t CACHE_SIZE, uint32_t CACHE_BLOCK_SIZE, uint32_t CACHE_ASSOCIATIVITY, ReplacementPolicy REPLACEMENT_POLICY>
void Cache<CACHE_BLOCK, PHYSICAL_ADDRESS, CACHE_SIZE, CACHE_BLOCK_SIZE, CACHE_ASSOCIATIVITY, REPLACEMENT_POLICY>::InvalidateSet(uint32_t index)
{
	uint32_t way;

	// scan all cache ways
	for(way = 0; way < (uint32_t) CACHE_ASSOCIATIVITY; way++)
	{
		CACHE_BLOCK *block = &cache[index][way];
		
		block->Invalidate();
	}

	InitializeReplacementPolicy(index);
}

template <class CACHE_BLOCK, class PHYSICAL_ADDRESS, uint32_t CACHE_SIZE, uint32_t CACHE_BLOCK_SIZE, uint32_t CACHE_ASSOCIATIVITY, ReplacementPolicy REPLACEMENT_POLICY>
void Cache<CACHE_BLOCK, PHYSICAL_ADDRESS, CACHE_SIZE, CACHE_BLOCK_SIZE, CACHE_ASSOCIATIVITY, REPLACEMENT_POLICY>::Invalidate()
{
	uint32_t index;	/* a cache index */
	
	/* scan all cache index */
	for(index = 0; index < CACHE_SIZE / CACHE_BLOCK_SIZE / CACHE_ASSOCIATIVITY; index++)
	{
		InvalidateSet(index);
	}
}

template <class CACHE_BLOCK, class PHYSICAL_ADDRESS, uint32_t CACHE_SIZE, uint32_t CACHE_BLOCK_SIZE, uint32_t CACHE_ASSOCIATIVITY, ReplacementPolicy REPLACEMENT_POLICY>
void Cache<CACHE_BLOCK, PHYSICAL_ADDRESS, CACHE_SIZE, CACHE_BLOCK_SIZE, CACHE_ASSOCIATIVITY, REPLACEMENT_POLICY>::PrInvalidateBlock(PHYSICAL_ADDRESS physical_addr, CacheControl cc)
{
	PHYSICAL_ADDRESS base_physical_addr;
	uint32_t index, offset;
	CACHE_BLOCK *block;
	
	// Is the cache enabled ?
	if(enabled)
	{
		// Decode the physical address
		DecodeAddress(physical_addr, base_physical_addr, index, offset);
		
		// Search the block in the set
		block = SearchBlock(index, base_physical_addr);
		
		// Hit or Miss ?
		if(block)
		{
			// Cache Hit

			// Invalidate the block
			block->Invalidate();
		}
	}
	
	if(next_level_cache)
	{
		// Invalidate the block in the next level cache
		next_level_cache->PrInvalidateBlock(physical_addr, cc);
		return;
	}

	if(address_only_broadcast_enabled && (cc & CC_MEMORY_COHERENCY_ENFORCED))
	{
		// Broadcast invalidate block on the bus
		bus_interface->BusInvalidateBlock(physical_addr, BC_GLOBAL);
	}
}

template <class CACHE_BLOCK, class PHYSICAL_ADDRESS, uint32_t CACHE_SIZE, uint32_t CACHE_BLOCK_SIZE, uint32_t CACHE_ASSOCIATIVITY, ReplacementPolicy REPLACEMENT_POLICY>
void Cache<CACHE_BLOCK, PHYSICAL_ADDRESS, CACHE_SIZE, CACHE_BLOCK_SIZE, CACHE_ASSOCIATIVITY, REPLACEMENT_POLICY>::PrFlushBlock(PHYSICAL_ADDRESS physical_addr, CacheControl cc)
{
	PHYSICAL_ADDRESS base_physical_addr;
	uint32_t index, offset;
	CACHE_BLOCK *block;
	
	// Is the cache enabled ?
	if(enabled)
	{
		// Decode the physical address
		DecodeAddress(physical_addr, base_physical_addr, index, offset);
	
		// Search the block in the set
		block = SearchBlock(index, base_physical_addr);
	
		// Hit or Miss ?
		if(block)
		{
			// Cache Hit
			
			// Is the block has been previously modified ?
			if(block->IsModified())
			{
				// yes: write back the block to memory, and turn on snooping in other processors if memory coherency is enforced
				bus_interface->BusWrite(block->base_physical_addr, block->data, CACHE_BLOCK_SIZE, (cc & CC_MEMORY_COHERENCY_ENFORCED) ? BC_GLOBAL : BC_NONE);
			}
			
			// Invalidate the block
			block->Invalidate();

			if(next_level_cache)
			{
				// Invalidate the block in the next level cache
				next_level_cache->PrInvalidateBlock(physical_addr, CC_NONE);
			}

			if(address_only_broadcast_enabled && (cc & CC_MEMORY_COHERENCY_ENFORCED))
			{
				// Broadcast the flush block on the bus
				bus_interface->BusFlushBlock(physical_addr, BC_GLOBAL);
			}
			return;
		}
	}

	// Cache miss or cache disabled

	if(next_level_cache)
	{
		// Flush the block in the next level cache
		next_level_cache->PrFlushBlock(physical_addr, cc);
		return;
	}

	if(address_only_broadcast_enabled && (cc & CC_MEMORY_COHERENCY_ENFORCED))
	{
		// Broadcast the flush block on the bus, and turn on snooping in other processors
		bus_interface->BusFlushBlock(physical_addr, BC_GLOBAL);
	}
}

template <class CACHE_BLOCK, class PHYSICAL_ADDRESS, uint32_t CACHE_SIZE, uint32_t CACHE_BLOCK_SIZE, uint32_t CACHE_ASSOCIATIVITY, ReplacementPolicy REPLACEMENT_POLICY>
void Cache<CACHE_BLOCK, PHYSICAL_ADDRESS, CACHE_SIZE, CACHE_BLOCK_SIZE, CACHE_ASSOCIATIVITY, REPLACEMENT_POLICY>::PrZeroBlock(PHYSICAL_ADDRESS physical_addr, CacheControl cc)
{
	PHYSICAL_ADDRESS base_physical_addr;
	uint32_t index, offset;
	CACHE_BLOCK *block;
	
	// Is the cache enabled ?
	if(enabled)
	{
		// Decode the physical address
		DecodeAddress(physical_addr, base_physical_addr, index, offset);
	
		// Search the block in the set
		block = SearchBlock(index, base_physical_addr);
	
		// Hit or Miss ?
		if(!block)
		{
			// Cache Miss

			// Choose a block to replace within the set
			block = ChooseBlockToReplace(index);
			
			// Is the block has been previously modified ?
			if(block->IsModified())
			{
				// yes:evict the block
				if(next_level_cache)
					next_level_cache->PrWrite(block->base_physical_addr, block->data, CACHE_BLOCK_SIZE, cc);
				else
					bus_interface->BusWrite(block->base_physical_addr, block->data, CACHE_BLOCK_SIZE, (cc & CC_MEMORY_COHERENCY_ENFORCED) ? BC_GLOBAL : BC_NONE);
			}

			// Invalidate the chosen block
			block->Invalidate();
			
			// Update the block physical address
			block->base_physical_addr = base_physical_addr;
		}
		
		// Fill the block with zero's
		block->Zero();
		block->SetModified();

		// Update the replacement policy
		UpdateReplacementPolicy(block, index);
	}
	
	if(next_level_cache)
	{
		// Zero block in the next level cache
		next_level_cache->PrZeroBlock(physical_addr, cc);
		return;
	}

	// Broadcast the zero block on the bus, and turn on snooping in other processors
	bus_interface->BusZeroBlock(physical_addr, (cc & CC_MEMORY_COHERENCY_ENFORCED) ? BC_GLOBAL : BC_NONE);
}

template <class CACHE_BLOCK, class PHYSICAL_ADDRESS, uint32_t CACHE_SIZE, uint32_t CACHE_BLOCK_SIZE, uint32_t CACHE_ASSOCIATIVITY, ReplacementPolicy REPLACEMENT_POLICY>
void Cache<CACHE_BLOCK, PHYSICAL_ADDRESS, CACHE_SIZE, CACHE_BLOCK_SIZE, CACHE_ASSOCIATIVITY, REPLACEMENT_POLICY>::DoAction(CACHE_BLOCK *block, Event ev, CacheControl cc, CacheStatus& cs)
{
	cs = CS_MISS;

	switch(block->GetAction(ev))
	{
		case AcNone:
			break;
		case AcBusRd:
			if(next_level_cache)
				next_level_cache->PrRead(block->base_physical_addr, block->data, CACHE_BLOCK_SIZE, cc);
			else
				bus_interface->BusRead(block->base_physical_addr, block->data, CACHE_BLOCK_SIZE, (cc & CC_MEMORY_COHERENCY_ENFORCED) ? BC_GLOBAL : BC_NONE, cs);
			// I should report a refill access to power estimator
			break;
		case AcBusRdX:
			if(next_level_cache)
				next_level_cache->PrReadX(block->base_physical_addr, block->data, CACHE_BLOCK_SIZE, cc);
			else
				bus_interface->BusReadX(block->base_physical_addr, block->data, CACHE_BLOCK_SIZE, (cc & CC_MEMORY_COHERENCY_ENFORCED) ? BC_GLOBAL : BC_NONE, cs);
			// I should report a refill access to power estimator
			break;
		case AcWB:
			if(next_level_cache)
				next_level_cache->PrWrite(block->base_physical_addr, block->data, CACHE_BLOCK_SIZE, CC_NONE);
			else
				bus_interface->BusWrite(block->base_physical_addr, block->data, CACHE_BLOCK_SIZE, (cc & CC_MEMORY_COHERENCY_ENFORCED) ? BC_GLOBAL : BC_NONE);
			// I should report an evict access to power estimator
			break;
	}
}

template <class CACHE_BLOCK, class PHYSICAL_ADDRESS, uint32_t CACHE_SIZE, uint32_t CACHE_BLOCK_SIZE, uint32_t CACHE_ASSOCIATIVITY, ReplacementPolicy REPLACEMENT_POLICY>
void Cache<CACHE_BLOCK, PHYSICAL_ADDRESS, CACHE_SIZE, CACHE_BLOCK_SIZE, CACHE_ASSOCIATIVITY, REPLACEMENT_POLICY>::PrRead(PHYSICAL_ADDRESS physical_addr, void *buffer, uint32_t size, CacheControl cc)
{
	uint32_t offset = physical_addr & (CACHE_BLOCK_SIZE - 1);
	uint32_t size_to_block_boundary = CACHE_BLOCK_SIZE - offset;
	if(size_to_block_boundary >= size)
	{
		// Memory access does not cross a cache block boundary
		PrReadBlockAligned(physical_addr, buffer, size, cc);
	}
	else
	{
		// Memory access crosses a cache block boundary
		PrReadBlockAligned(physical_addr, buffer, size_to_block_boundary, cc);
		PrReadBlockAligned((physical_addr + CACHE_BLOCK_SIZE) & ~(CACHE_BLOCK_SIZE - 1), (uint8_t *) buffer + size_to_block_boundary, size - size_to_block_boundary, cc);
	}
}

template <class CACHE_BLOCK, class PHYSICAL_ADDRESS, uint32_t CACHE_SIZE, uint32_t CACHE_BLOCK_SIZE, uint32_t CACHE_ASSOCIATIVITY, ReplacementPolicy REPLACEMENT_POLICY>
void Cache<CACHE_BLOCK, PHYSICAL_ADDRESS, CACHE_SIZE, CACHE_BLOCK_SIZE, CACHE_ASSOCIATIVITY, REPLACEMENT_POLICY>::PrReadX(PHYSICAL_ADDRESS physical_addr, void *buffer, uint32_t size, CacheControl cc)
{
	uint32_t offset = physical_addr & (CACHE_BLOCK_SIZE - 1);
	uint32_t size_to_block_boundary = CACHE_BLOCK_SIZE - offset;
	if(size_to_block_boundary >= size)
	{
		// Memory access does not cross a cache block boundary
		PrReadXBlockAligned(physical_addr, buffer, size, cc);
	}
	else
	{
		// Memory access crosses a cache block boundary
		PrReadXBlockAligned(physical_addr, buffer, size_to_block_boundary, cc);
		PrReadXBlockAligned((physical_addr + CACHE_BLOCK_SIZE) & ~(CACHE_BLOCK_SIZE - 1), (uint8_t *) buffer + size_to_block_boundary, size - size_to_block_boundary, cc);
	}
}

template <class CACHE_BLOCK, class PHYSICAL_ADDRESS, uint32_t CACHE_SIZE, uint32_t CACHE_BLOCK_SIZE, uint32_t CACHE_ASSOCIATIVITY, ReplacementPolicy REPLACEMENT_POLICY>
void Cache<CACHE_BLOCK, PHYSICAL_ADDRESS, CACHE_SIZE, CACHE_BLOCK_SIZE, CACHE_ASSOCIATIVITY, REPLACEMENT_POLICY>::PrWrite(PHYSICAL_ADDRESS physical_addr, const void *buffer, uint32_t size, CacheControl cc)
{
	uint32_t offset = physical_addr & (CACHE_BLOCK_SIZE - 1);
	uint32_t size_to_block_boundary = CACHE_BLOCK_SIZE - offset;
	if(size_to_block_boundary >= size)
	{
		// Memory access does not cross a cache block boundary
		PrWriteBlockAligned(physical_addr, buffer, size, cc);
	}
	else
	{
		// Memory access crosses a cache block boundary
		PrWriteBlockAligned(physical_addr, buffer, size_to_block_boundary, cc);
		PrWriteBlockAligned((physical_addr + CACHE_BLOCK_SIZE) & ~(CACHE_BLOCK_SIZE - 1), (uint8_t *) buffer + size_to_block_boundary, size - size_to_block_boundary, cc);
	}
}


template <class CACHE_BLOCK, class PHYSICAL_ADDRESS, uint32_t CACHE_SIZE, uint32_t CACHE_BLOCK_SIZE, uint32_t CACHE_ASSOCIATIVITY, ReplacementPolicy REPLACEMENT_POLICY>
inline void Cache<CACHE_BLOCK, PHYSICAL_ADDRESS, CACHE_SIZE, CACHE_BLOCK_SIZE, CACHE_ASSOCIATIVITY, REPLACEMENT_POLICY>::PrReadBlockAligned(PHYSICAL_ADDRESS physical_addr, void *buffer, uint32_t size, CacheControl cc)
{
	PHYSICAL_ADDRESS base_physical_addr;
	uint32_t index, offset;
	CACHE_BLOCK *block;
	
	// Is the cache enabled ?
	if(enabled)
	{
		// Decode the physical address
		DecodeAddress(physical_addr, base_physical_addr, index, offset);
	
		// search for the block in the set
		block = SearchBlock(index, base_physical_addr);

		if(block)
		{
			// Local cache Read Hit
			CacheStatus cs;
			DoAction(block, EvPrRd, cc, cs);
			block->UpdateState(EvPrRd, cs & CS_SHARED);
		}
		else
		{
			// Local cache Read Miss
			// Choose a block to replace in the set
			block = ChooseBlockToReplace(index);
		
			// Is the block has been previously modified ?
			if(block->IsModified())
			{
				// Evict the block
				if(next_level_cache)
					next_level_cache->PrWrite(block->base_physical_addr, block->data, CACHE_BLOCK_SIZE, cc);
				else
					bus_interface->BusWrite(block->base_physical_addr, block->data, CACHE_BLOCK_SIZE, (cc & CC_MEMORY_COHERENCY_ENFORCED) ? BC_GLOBAL : BC_NONE);
			}
			
			// Invalidate the chosen block
			block->Invalidate();

			// Update the block physical address
			block->base_physical_addr = base_physical_addr;
			
			// Do an action on the bus (according to the coherency protocol), and report an error to processor if the action has generated a bus error
			CacheStatus cs;
			DoAction(block, EvPrRd, cc, cs);

			block->UpdateState(EvPrRd, cs & CS_SHARED);
		}
	
		// read data from the block
		block->Read(offset, buffer, size);

		// Update the replacement policy
		UpdateReplacementPolicy(block, index);

		// report a read access to power estimator
		if(power_estimator_import) power_estimator_import->ReportReadAccess();
		
		return;
	}
	
	// Cache is disabled

	if(next_level_cache)
	{
		// Read the cache block from the next level cache
		next_level_cache->PrRead(physical_addr, buffer, size, cc);
		return;
	}

	// Read the cache block from memory
	CacheStatus cs;
	bus_interface->BusRead(physical_addr, buffer, size, (cc & CC_MEMORY_COHERENCY_ENFORCED) ? BC_GLOBAL : BC_NONE, cs);
}

template <class CACHE_BLOCK, class PHYSICAL_ADDRESS, uint32_t CACHE_SIZE, uint32_t CACHE_BLOCK_SIZE, uint32_t CACHE_ASSOCIATIVITY, ReplacementPolicy REPLACEMENT_POLICY>
inline void Cache<CACHE_BLOCK, PHYSICAL_ADDRESS, CACHE_SIZE, CACHE_BLOCK_SIZE, CACHE_ASSOCIATIVITY, REPLACEMENT_POLICY>::PrReadXBlockAligned(PHYSICAL_ADDRESS physical_addr, void *buffer, uint32_t size, CacheControl cc)
{
	PHYSICAL_ADDRESS base_physical_addr;
	uint32_t index, offset;
	CACHE_BLOCK *block;
	
	// Is the cache enabled ?
	if(enabled)
	{
		// Decode the physical address
		DecodeAddress(physical_addr, base_physical_addr, index, offset);
	
		// search for the block in the set
		block = SearchBlock(index, base_physical_addr);

		if(block)
		{
			// Local cache Read Hit
			CacheStatus cs;
			DoAction(block, EvPrRdX, cc, cs);
			block->UpdateState(EvPrRdX, cs & CS_SHARED);
		}
		else
		{
			// Local cache Read Miss
			// Choose a block to replace in the set
			block = ChooseBlockToReplace(index);
		
			// Is the block has been previously modified ?
			if(block->IsModified())
			{
				// Evict the block
				if(next_level_cache)
					next_level_cache->PrWrite(block->base_physical_addr, block->data, CACHE_BLOCK_SIZE, cc);
				else
					bus_interface->BusWrite(block->base_physical_addr, block->data, CACHE_BLOCK_SIZE, (cc & CC_MEMORY_COHERENCY_ENFORCED) ? BC_GLOBAL : BC_NONE);
			}

			// Invalidate the chosen block
			block->Invalidate();
			
			// Update the block physical address
			block->base_physical_addr = base_physical_addr;
			
			// Do an action on the bus (according to the coherency protocol), and report an error to processor if the action has generated a bus error
			CacheStatus cs;
			DoAction(block, EvPrRdX, cc, cs);

			block->UpdateState(EvPrRdX, cs & CS_SHARED);
		}
	
		// read data from the block
		block->Read(offset, buffer, size);
		
		// Update the replacement policy
		UpdateReplacementPolicy(block, index);

		// report a read access to power estimator
		if(power_estimator_import) power_estimator_import->ReportReadAccess();

		return;
	}
	
	// Cache is disabled

	if(next_level_cache)
	{
		// Read the cache block from the next level cache
		next_level_cache->PrReadX(physical_addr, buffer, size, cc);
		return;
	}

	// Read the cache block from memory
	CacheStatus cs;
	bus_interface->BusReadX(physical_addr, buffer, size, (cc & CC_MEMORY_COHERENCY_ENFORCED) ? BC_GLOBAL : BC_NONE, cs);
}

template <class CACHE_BLOCK, class PHYSICAL_ADDRESS, uint32_t CACHE_SIZE, uint32_t CACHE_BLOCK_SIZE, uint32_t CACHE_ASSOCIATIVITY, ReplacementPolicy REPLACEMENT_POLICY>
inline void Cache<CACHE_BLOCK, PHYSICAL_ADDRESS, CACHE_SIZE, CACHE_BLOCK_SIZE, CACHE_ASSOCIATIVITY, REPLACEMENT_POLICY>::PrWriteBlockAligned(PHYSICAL_ADDRESS physical_addr, const void *buffer, uint32_t size, CacheControl cc)
{
	PHYSICAL_ADDRESS base_physical_addr;
	uint32_t index, offset;
	CACHE_BLOCK *block;

	// Is the cache enabled ?
	if(enabled)
	{
		// Decode the physical address
		DecodeAddress(physical_addr, base_physical_addr, index, offset);

		// search for the block in the set
		block = SearchBlock(index, base_physical_addr);
		
		if(block)
		{
			// Local cache Write Hit
			CacheStatus cs;
			DoAction(block, EvPrWr, cc, cs);
			block->UpdateState(EvPrWr, cs & CS_SHARED);
		}
		else
		{
			// Local cache Write Miss
			// Choose a block to replace in the set
			block = ChooseBlockToReplace(index);
		
			// Is the block has been previously modified ?
			if(block->IsModified())
			{
				// Evict the block
				if(next_level_cache)
					next_level_cache->PrWrite(block->base_physical_addr, block->data, CACHE_BLOCK_SIZE, cc);
				else
					bus_interface->BusWrite(block->base_physical_addr, block->data, CACHE_BLOCK_SIZE, (cc & CC_MEMORY_COHERENCY_ENFORCED) ? BC_GLOBAL : BC_NONE);
			}

			// Invalidate the chosen block
			block->Invalidate();
			
			// Update the block physical address
			block->base_physical_addr = base_physical_addr;
			
			// Do an action on the bus (according to the coherency protocol), and report an error to processor if the action has generated a bus error
			CacheStatus cs;
			DoAction(block, EvPrWr, cc, cs);

			// Update the block state
			block->UpdateState(EvPrWr, cs & CS_SHARED);
		}
	
		// Write data into the block
		block->Write(offset, buffer, size);
		//block->SetModified();

		// Update the replacement policy
		UpdateReplacementPolicy(block, index);

		// report a write access to power estimator
		if(power_estimator_import) power_estimator_import->ReportWriteAccess();
		
		// If write through is activated, then also write into next level cache/bus
		if(cc & CC_WRITE_THROUGH)
		{
			if(next_level_cache)
			{
				// Write into the next level cache
				next_level_cache->PrWrite(physical_addr, buffer, size, cc);
			}
			else
			{
				// Write into memory
				bus_interface->BusWrite(physical_addr, buffer, size, (cc & CC_MEMORY_COHERENCY_ENFORCED) ? BC_GLOBAL : BC_NONE);
			}
		}

		return;
	}

	// Cache is disabled ?

	if(next_level_cache)
	{
		// Write into the next level cache
		next_level_cache->PrWrite(physical_addr, buffer, size, cc);
		return;
	}

	// Write into memory
	bus_interface->BusWrite(physical_addr, buffer, size, (cc & CC_MEMORY_COHERENCY_ENFORCED) ? BC_GLOBAL : BC_NONE);
}

template <class CACHE_BLOCK, class PHYSICAL_ADDRESS, uint32_t CACHE_SIZE, uint32_t CACHE_BLOCK_SIZE, uint32_t CACHE_ASSOCIATIVITY, ReplacementPolicy REPLACEMENT_POLICY>
void Cache<CACHE_BLOCK, PHYSICAL_ADDRESS, CACHE_SIZE, CACHE_BLOCK_SIZE, CACHE_ASSOCIATIVITY, REPLACEMENT_POLICY>::BusInvalidateBlock(PHYSICAL_ADDRESS physical_addr)
{
	PHYSICAL_ADDRESS base_physical_addr;
	uint32_t index, offset;
	CACHE_BLOCK *block;
	
	// Is cache enabled ?
	if(enabled)
	{
		// Decode the physical address
		DecodeAddress(physical_addr, base_physical_addr, index, offset);
		
		// Search for the block in the set
		block = SearchBlock(index, base_physical_addr);
		
		// Hit or Miss ?
		if(block)
		{
			// Cache Snoop Hit

			// Invalidate the block
			block->Invalidate();
		}
	}
	
	if(next_level_cache) next_level_cache->BusInvalidateBlock(physical_addr);
}

template <class CACHE_BLOCK, class PHYSICAL_ADDRESS, uint32_t CACHE_SIZE, uint32_t CACHE_BLOCK_SIZE, uint32_t CACHE_ASSOCIATIVITY, ReplacementPolicy REPLACEMENT_POLICY>
void Cache<CACHE_BLOCK, PHYSICAL_ADDRESS, CACHE_SIZE, CACHE_BLOCK_SIZE, CACHE_ASSOCIATIVITY, REPLACEMENT_POLICY>::BusFlushBlock(PHYSICAL_ADDRESS physical_addr)
{
	PHYSICAL_ADDRESS base_physical_addr;
	uint32_t index, offset;
	CACHE_BLOCK *block;
	CacheStatus cs = CS_MISS;

	// Is cache enabled ?
	if(enabled)
	{
		// Decode the physical address
		DecodeAddress(physical_addr, base_physical_addr, index, offset);
	
		// Search for the block in the set
		block = SearchBlock(index, base_physical_addr);
	
		// Hit or Miss ?
		if(block)
		{
			// Cache Snoop Hit

			// Is the block has been previously modified ?
			if(block->IsModified())
			{
				// yes:write back block to memory
				bus_interface->BusWrite(block->base_physical_addr, block->data, CACHE_BLOCK_SIZE, BC_NONE);
			}
			
			// Invalidate the block
			block->Invalidate();

			// report a cache miss to the bus
			bus_interface->BusSetCacheStatus(CS_HIT);
		}
	}

	// cache disabled or cache miss
	if(next_level_cache)
	{
		// the flush command just pass through the cache
		next_level_cache->BusFlushBlock(physical_addr);
	}
}
	
template <class CACHE_BLOCK, class PHYSICAL_ADDRESS, uint32_t CACHE_SIZE, uint32_t CACHE_BLOCK_SIZE, uint32_t CACHE_ASSOCIATIVITY, ReplacementPolicy REPLACEMENT_POLICY>
void Cache<CACHE_BLOCK, PHYSICAL_ADDRESS, CACHE_SIZE, CACHE_BLOCK_SIZE, CACHE_ASSOCIATIVITY, REPLACEMENT_POLICY>::BusRead(PHYSICAL_ADDRESS physical_addr, void *buffer, uint32_t size)
{
	PHYSICAL_ADDRESS base_physical_addr;
	uint32_t index, offset;
	CACHE_BLOCK *block;

	// Is the cache enabled ?
	if(enabled)
	{
		// Decode the physical address
		DecodeAddress(physical_addr, base_physical_addr, index, offset);
	
		// search for the block in the set
		block = SearchBlock(index, base_physical_addr);

		// report a read access to power estimator
		if(power_estimator_import) power_estimator_import->ReportReadAccess();

		// Hit or Miss ?
		if(block)
		{
			// Snoop Hit
			CacheStatus block_cs = (CacheStatus)(CS_HIT | (block->IsModified() ? CS_MODIFIED : CS_MISS));
			
			// Read the data from the block
			block->Read(offset, buffer, size);
			
			CacheStatus cs;
			// Do an action on the bus (according to the coherency protocol), and report an error to processor if the action has generated a bus error
			DoAction(block, EvBusRd, cc, cs);
			
			// Update the block state
			block->UpdateState(EvBusRd, cs & CS_SHARED);
			
			// report the block status to the bus before block state update
			bus_interface->BusSetCacheStatus(block_cs);
		}
	}
	
	// Cache disabled or cache miss
	if(next_level_cache)
	{
		// the Read command just pass through the cache
		next_level_cache->BusRead(physical_addr, buffer, size);
	}
}

template <class CACHE_BLOCK, class PHYSICAL_ADDRESS, uint32_t CACHE_SIZE, uint32_t CACHE_BLOCK_SIZE, uint32_t CACHE_ASSOCIATIVITY, ReplacementPolicy REPLACEMENT_POLICY>
void Cache<CACHE_BLOCK, PHYSICAL_ADDRESS, CACHE_SIZE, CACHE_BLOCK_SIZE, CACHE_ASSOCIATIVITY, REPLACEMENT_POLICY>::BusReadX(PHYSICAL_ADDRESS physical_addr, void *buffer, uint32_t size)
{
	PHYSICAL_ADDRESS base_physical_addr;
	uint32_t index, offset;
	CACHE_BLOCK *block;

	// Is the cache enabled ?
	if(enabled)
	{
		// Decode the physical address
		DecodeAddress(physical_addr, base_physical_addr, index, offset);
	
		// search for the block in the set
		block = SearchBlock(index, base_physical_addr);

		// report a read access to power estimator
		if(power_estimator_import) power_estimator_import->ReportReadAccess();

		// Hit or Miss ?
		if(block)
		{
			// Snoop Hit
			CacheStatus block_cs = (CacheStatus)(CS_HIT | (block->IsModified() ? CS_MODIFIED : CS_MISS));
			
			// Read the data from the block
			block->Read(offset, buffer, size);
			
			CacheStatus cs;
			// Do an action on the bus (according to the coherency protocol), and report an error to processor if the action has generated a bus error
			DoAction(block, EvBusRdX, cc, cs);
			
			// Update the block state
			block->UpdateState(EvBusRdX, cs & CS_SHARED);
			
			// report the block status to the bus before block state update
			bus_interface->BusSetCacheStatus(block_cs);
		}
	}
	
	// Cache disabled or cache miss
	if(next_level_cache)
	{
		// the Read command just pass through the cache
		next_level_cache->BusReadX(physical_addr, buffer, size);
	}
}

// ------------------------------------------------ PRIVATE Methods -----------------------------------------------------------------


const int B0 = 1 << 0;
const int B1 = 1 << 1;
const int B2 = 1 << 2;
const int B3 = 1 << 3;
const int B4 = 1 << 4;
const int B5 = 1 << 5;
const int B6 = 1 << 6;

template <class CACHE_BLOCK, class PHYSICAL_ADDRESS, uint32_t CACHE_SIZE, uint32_t CACHE_BLOCK_SIZE, uint32_t CACHE_ASSOCIATIVITY, ReplacementPolicy REPLACEMENT_POLICY>
void Cache<CACHE_BLOCK, PHYSICAL_ADDRESS, CACHE_SIZE, CACHE_BLOCK_SIZE, CACHE_ASSOCIATIVITY, REPLACEMENT_POLICY>::InitializeReplacementPolicy(uint32_t index)
{
	uint32_t way;
	
	if(CACHE_ASSOCIATIVITY > 1)
	{
		switch(REPLACEMENT_POLICY)
		{
			case RP_LRU:
				// range each block to make block on way #0 be the least recently used
				// so that at the first access to this cache index will allocate a block on way #0
				for(way = 0; way < (uint32_t) CACHE_ASSOCIATIVITY; way++)
				{
					CACHE_BLOCK *block = &cache[index][way];
						
					// block->way = way;
					// block->lru_bits = (CACHE_ASSOCIATIVITY - 1) - way;
						
					if(way > 0)
					{
						block->next = &cache[index][way - 1];
					}
					else
					{
						block->next = 0;
						lru_block[index] = block;
					}
						
					if(way < CACHE_ASSOCIATIVITY - 1)
					{
						block->prev = &cache[index][way + 1];
					}
					else
					{
						mru_block[index] = block;
						block->prev = 0;
					}
				}
				break;
			case RP_PLRU:
				plru_bits[index] = 0;
				break;
		}
	}
}

template <class CACHE_BLOCK, class PHYSICAL_ADDRESS, uint32_t CACHE_SIZE, uint32_t CACHE_BLOCK_SIZE, uint32_t CACHE_ASSOCIATIVITY, ReplacementPolicy REPLACEMENT_POLICY>
void Cache<CACHE_BLOCK, PHYSICAL_ADDRESS, CACHE_SIZE, CACHE_BLOCK_SIZE, CACHE_ASSOCIATIVITY, REPLACEMENT_POLICY>::InitializeReplacementPolicy()
{
	uint32_t index;	/* a cache index */
	uint32_t way;
	
	if(CACHE_ASSOCIATIVITY > 1)
	{
		/* scan all cache index */
		for(index = 0; index < CACHE_SIZE / CACHE_BLOCK_SIZE / CACHE_ASSOCIATIVITY; index++)
		{
			InitializeReplacementPolicy(index);
		}
	}
}

template <class CACHE_BLOCK, class PHYSICAL_ADDRESS, uint32_t CACHE_SIZE, uint32_t CACHE_BLOCK_SIZE, uint32_t CACHE_ASSOCIATIVITY, ReplacementPolicy REPLACEMENT_POLICY>
void Cache<CACHE_BLOCK, PHYSICAL_ADDRESS, CACHE_SIZE, CACHE_BLOCK_SIZE, CACHE_ASSOCIATIVITY, REPLACEMENT_POLICY>::UpdateReplacementPolicy(CACHE_BLOCK *block, uint32_t index)
{
	uint32_t way;
	
	if(CACHE_ASSOCIATIVITY > 1)
	{
		switch(REPLACEMENT_POLICY)
		{
			case RP_LRU:
				// update the replacement policy if the block is not the most recently used within the set
				if(block->prev)
				{
					// Update LRU replacement policy
					if(block->next)
					{
						block->next->prev = block->prev;
					}
					else
					{
						lru_block[index] = block->prev;
					}
						
					block->prev->next = block->next;
					block->next = mru_block[index];
					block->prev = 0;
					mru_block[index]->prev = block;
					mru_block[index] = block;
				}
				break;
			case RP_PLRU:
				way = block - cache[index];
				
				switch(CACHE_ASSOCIATIVITY)
				{
					case 2:
						switch(way)
						{
							case 1:
								plru_bits[index] |= B0;
								break;
							case 2:
								plru_bits[index] &= ~B0;
								break;
						}
						break;
						
					case 4:
						switch(way)
						{
							case 0:
								plru_bits[index] |= B0  + B1;
								break;

							case 1:
								plru_bits[index] |= B0;
								plru_bits[index] &= B1;
								break;

							case 2:
								plru_bits[index] |= B2;
								plru_bits[index] &= B0;
								break;

							case 3:
								plru_bits[index] &= B0 + B2;
								break;
						}
						break;

					case 8:
						switch(way)
						{
							case 0:
								plru_bits[index] |= B0 + B1 + B3;
								break;
							case 1:
								plru_bits[index] |= B0 + B1;
								plru_bits[index] &= ~B3;
								break;

							case 2:
								plru_bits[index] |= B0 + B4;
								plru_bits[index] &= ~B1;
								break;

							case 3:
								plru_bits[index] |= B0;
								plru_bits[index] &= ~(B1 + B4);
								break;

							case 4:
								plru_bits[index] |= B2 + B5;
								plru_bits[index] &= ~B0;
								break;

							case 5:
								plru_bits[index] |= B2;
								plru_bits[index] &= ~(B0 + B5);
								break;

							case 6:
								plru_bits[index] |=B6;
								plru_bits[index] &=~(B0 + B2);
								break;

							case 7:
								plru_bits[index] &= ~(B0 + B2 + B6);
								break;
						}
						break;
				}

				break;
		}
	}
}

template <class CACHE_BLOCK, class PHYSICAL_ADDRESS, uint32_t CACHE_SIZE, uint32_t CACHE_BLOCK_SIZE, uint32_t CACHE_ASSOCIATIVITY, ReplacementPolicy REPLACEMENT_POLICY>
CACHE_BLOCK *Cache<CACHE_BLOCK, PHYSICAL_ADDRESS, CACHE_SIZE, CACHE_BLOCK_SIZE, CACHE_ASSOCIATIVITY, REPLACEMENT_POLICY>::ChooseBlockToReplace(uint32_t index)
{
	uint32_t way;
	
	if(CACHE_ASSOCIATIVITY > 1)
	{
		switch(REPLACEMENT_POLICY)
		{
			case RP_LRU:
				return lru_block[index];
			case RP_PLRU:
				for(way = 0; way < CACHE_ASSOCIATIVITY; way++)
				{
					CACHE_BLOCK *block = &cache[index][way];
					if(!block->IsValid())
					{
						return block;
					}
				}
	
				switch(CACHE_ASSOCIATIVITY)
				{
					case 2:
						if(plru_bits[index] & B0)
							return &cache[index][1];
						else
							return &cache[index][0];
						break;
						
					case 4:
						if(plru_bits[index] & B0)
						{
							if(plru_bits[index] & B2) return &cache[index][3]; else return &cache[index][2];
						}
						else
						{
							if(plru_bits[index] & B1) return &cache[index][1]; else return &cache[index][0];
						}
						break;
	
					case 8:
						if(plru_bits[index] & B0)
						{
							if(plru_bits[index] & B2)
							{
								if(plru_bits[index] & B5) return &cache[index][5]; else return &cache[index][4];
							}
							else
							{
								if(plru_bits[index] & B6) return &cache[index][7]; else return &cache[index][6];
							}
						}
						else
						{
							if(plru_bits[index] & B1)
							{
								if(plru_bits[index] & B4) return &cache[index][3]; else return &cache[index][2];
							}
							else
							{
								if(plru_bits[index] & B3) return &cache[index][1]; else return &cache[index][0];
							}
						}
				}
				break;
		}
	}
	
	return &cache[index][0];
}

template <class CACHE_BLOCK, class PHYSICAL_ADDRESS, uint32_t CACHE_SIZE, uint32_t CACHE_BLOCK_SIZE, uint32_t CACHE_ASSOCIATIVITY, ReplacementPolicy REPLACEMENT_POLICY>
inline CACHE_BLOCK *Cache<CACHE_BLOCK, PHYSICAL_ADDRESS, CACHE_SIZE, CACHE_BLOCK_SIZE, CACHE_ASSOCIATIVITY, REPLACEMENT_POLICY>::SearchBlock(uint32_t index, PHYSICAL_ADDRESS base_physical_addr)
{
	CACHE_BLOCK *block;
	uint32_t way;

	if(CACHE_ASSOCIATIVITY > 1)
	{
		switch(REPLACEMENT_POLICY)
		{
			case RP_LRU:
				// search for the cache block in the set, starting with most recently used blocks
				for(block = mru_block[index]; block; block = block->next)
				{
					if(block->IsValid() && block->base_physical_addr == base_physical_addr)
					{
						// Cache Hit
						return block;
					}
				}
				break;
				
			case RP_PLRU:
				for(way = 0; way < CACHE_ASSOCIATIVITY; way++)
				{
					CACHE_BLOCK *block = &cache[index][way];
					if(block->IsValid() && block->base_physical_addr == base_physical_addr)
					{
						// Cache Hit
						return block;
					}
				}
				break;
		}

		return 0;
	}

	block = &cache[index][0];
	return (block->IsValid() && block->base_physical_addr == base_physical_addr) ? block : 0;
}

template <class CACHE_BLOCK, class PHYSICAL_ADDRESS, uint32_t CACHE_SIZE, uint32_t CACHE_BLOCK_SIZE, uint32_t CACHE_ASSOCIATIVITY, ReplacementPolicy REPLACEMENT_POLICY>
inline void Cache<CACHE_BLOCK, PHYSICAL_ADDRESS, CACHE_SIZE, CACHE_BLOCK_SIZE, CACHE_ASSOCIATIVITY, REPLACEMENT_POLICY>::DecodeAddress(PHYSICAL_ADDRESS physical_addr, PHYSICAL_ADDRESS& base_physical_addr, uint32_t& index, uint32_t& offset)
{
	// +-----------------+--------+--------+
	// |       tag       |  index | offset |
	// +-----------------+--------+--------+
	index = (physical_addr / CACHE_BLOCK_SIZE) % (CACHE_SIZE / CACHE_BLOCK_SIZE / CACHE_ASSOCIATIVITY);
	base_physical_addr = physical_addr & (~(CACHE_BLOCK_SIZE - 1));
	offset = physical_addr & (CACHE_BLOCK_SIZE - 1);
}

} // end of namespace cache
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
