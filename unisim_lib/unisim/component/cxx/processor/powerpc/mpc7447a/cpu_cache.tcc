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

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_MPC7447A_CPU_CACHE_TCC__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_MPC7447A_CPU_CACHE_TCC__

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace powerpc {
namespace mpc7447a {

template <class CONFIG>
void CPU<CONFIG>::InvalidateDL1Set(uint32_t index)
{
	uint32_t way;

	CacheSet<typename CONFIG::DL1_CONFIG>& l1_set = dl1[index];

	l1_set.status.plru_bits = 0;
	
	for(way = 0; way < CacheSet<typename CONFIG::DL1_CONFIG>::ASSOCIATIVITY; way++)
	{
		CacheLine<typename CONFIG::DL1_CONFIG>& l1_line = l1_set[way];
		uint32_t sector;

		for(sector = 0; sector < CacheLine<typename CONFIG::DL1_CONFIG>::BLOCKS_PER_LINE; sector++)
		{
			CacheBlock<typename CONFIG::DL1_CONFIG>& l1_block = l1_line[sector];

			l1_block.status.valid = false;
			l1_block.status.dirty = false;
		}
		l1_line.status.valid = false;
		l1_line.SetBaseAddr(0);

		if(CONFIG::FAST_DL1_LOOKUP_ENABLE)
		{
			l1_line.status.prev = (way > 0) ? &l1_set[way - 1] : 0;
			l1_line.status.next = (way < (CacheSet<typename CONFIG::DL1_CONFIG>::ASSOCIATIVITY - 1)) ? &l1_set[way + 1] : 0;
		}
	}
	if(CONFIG::FAST_DL1_LOOKUP_ENABLE)
	{
		l1_set.status.mru_line = &l1_set[0];
		l1_set.status.lru_line = &l1_set[CacheSet<typename CONFIG::DL1_CONFIG>::ASSOCIATIVITY - 1];
	}
}

template <class CONFIG>
void CPU<CONFIG>::InvalidateDL1()
{
	if(unlikely(IsVerboseDL1()))
	{
		logger << DebugInfo << "Invalidating DL1" << endl << EndDebugInfo;
	}
	uint32_t index;
	
	for(index = 0; index < Cache<typename CONFIG::DL1_CONFIG>::NUM_SETS; index++)
	{
		InvalidateDL1Set(index);
	}
}

template <class CONFIG>
void CPU<CONFIG>::InvalidateIL1Set(uint32_t index)
{
	uint32_t way;
	
	CacheSet<typename CONFIG::IL1_CONFIG>& l1_set = il1[index];

	l1_set.status.plru_bits = 0;
	
	for(way = 0; way < CacheSet<typename CONFIG::IL1_CONFIG>::ASSOCIATIVITY; way++)
	{
		CacheLine<typename CONFIG::IL1_CONFIG>& l1_line = l1_set[way];
		uint32_t sector;

		for(sector = 0; sector < CacheLine<typename CONFIG::IL1_CONFIG>::BLOCKS_PER_LINE; sector++)
		{
			CacheBlock<typename CONFIG::IL1_CONFIG>& l1_block = l1_line[sector];

			l1_block.status.valid = false;
		}
		l1_line.status.valid = false;
		l1_line.SetBaseAddr(0);

		if(CONFIG::FAST_IL1_LOOKUP_ENABLE)
		{
			l1_line.status.prev = (way > 0) ? &l1_set[way - 1] : 0;
			l1_line.status.next = (way < (CacheSet<typename CONFIG::IL1_CONFIG>::ASSOCIATIVITY - 1)) ? &l1_set[way + 1] : 0;
		}
	}
	if(CONFIG::FAST_IL1_LOOKUP_ENABLE)
	{
		l1_set.status.mru_line = &l1_set[0];
		l1_set.status.lru_line = &l1_set[CacheSet<typename CONFIG::IL1_CONFIG>::ASSOCIATIVITY - 1];
	}
}

template <class CONFIG>
void CPU<CONFIG>::InvalidateIL1()
{
	if(unlikely(IsVerboseIL1()))
	{
		logger << DebugInfo << "Invalidating IL1" << endl << EndDebugInfo;
	}
	uint32_t index;
	
	for(index = 0; index < Cache<typename CONFIG::IL1_CONFIG>::NUM_SETS; index++)
	{
		InvalidateIL1Set(index);
	}
}

template <class CONFIG>
void CPU<CONFIG>::InvalidateL2Set(uint32_t index)
{
	uint32_t way;
	
	CacheSet<typename CONFIG::L2_CONFIG>& l2_set = l2[index];

	l2_set.status.plru_bits = 0;
	
	for(way = 0; way < CacheSet<typename CONFIG::L2_CONFIG>::ASSOCIATIVITY; way++)
	{
		CacheLine<typename CONFIG::L2_CONFIG>& l2_line = l2_set[way];
		uint32_t sector;

		for(sector = 0; sector < CacheLine<typename CONFIG::L2_CONFIG>::BLOCKS_PER_LINE; sector++)
		{
			CacheBlock<typename CONFIG::L2_CONFIG>& l2_block = l2_line[sector];

			l2_block.status.valid = false;
			l2_block.status.dirty = false;
		}
		l2_line.status.valid = false;
		l2_line.SetBaseAddr(0);

		if(CONFIG::FAST_L2_LOOKUP_ENABLE)
		{
			l2_line.status.prev = (way > 0) ? &l2_set[way - 1] : 0;
			l2_line.status.next = (way < (CacheSet<typename CONFIG::L2_CONFIG>::ASSOCIATIVITY - 1)) ? &l2_set[way + 1] : 0;
		}
	}
	if(CONFIG::FAST_L2_LOOKUP_ENABLE)
	{
		l2_set.status.mru_line = &l2_set[0];
		l2_set.status.lru_line = &l2_set[CacheSet<typename CONFIG::L2_CONFIG>::ASSOCIATIVITY - 1];
	}
}

template <class CONFIG>
void CPU<CONFIG>::InvalidateL2()
{
	if(unlikely(IsVerboseL2()))
	{
		logger << DebugInfo << "Invalidating L2" << endl << EndDebugInfo;
	}
	uint32_t index;
	
	for(index = 0; index < Cache<typename CONFIG::L2_CONFIG>::NUM_SETS; index++)
	{
		InvalidateL2Set(index);
	}
}

template <class CONFIG>
void CPU<CONFIG>::ClearAccessDL1(CacheAccess<typename CONFIG::DL1_CONFIG>& l1_access)
{
	l1_access.addr = 0;
	l1_access.line_base_addr = 0;
	l1_access.block_base_addr = 0;
	//uint32_t index;
	l1_access.way = 0;
	l1_access.sector = 0;
	l1_access.offset = 0;
	l1_access.size_to_block_boundary = 0;
	l1_access.set = 0;
	l1_access.line = 0;
	l1_access.line_to_evict = 0;
	l1_access.block = 0;
}

template <class CONFIG>
void CPU<CONFIG>::ClearAccessIL1(CacheAccess<typename CONFIG::IL1_CONFIG>& l1_access)
{
	l1_access.addr = 0;
	l1_access.line_base_addr = 0;
	l1_access.block_base_addr = 0;
	//uint32_t index;
	l1_access.way = 0;
	l1_access.sector = 0;
	l1_access.offset = 0;
	l1_access.size_to_block_boundary = 0;
	l1_access.set = 0;
	l1_access.line = 0;
	l1_access.line_to_evict = 0;
	l1_access.block = 0;
}

template <class CONFIG>
void CPU<CONFIG>::ClearAccessL2(CacheAccess<typename CONFIG::L2_CONFIG>& l2_access)
{
	l2_access.addr = 0;
	l2_access.line_base_addr = 0;
	l2_access.block_base_addr = 0;
	//uint32_t index;
	l2_access.way = 0;
	l2_access.sector = 0;
	l2_access.offset = 0;
	l2_access.size_to_block_boundary = 0;
	l2_access.set = 0;
	l2_access.line = 0;
	l2_access.line_to_evict = 0;
	l2_access.block = 0;
}

template <class CONFIG>
inline void CPU<CONFIG>::LookupDL1(CacheAccess<typename CONFIG::DL1_CONFIG>& l1_access)
{
	typename CONFIG::DL1_CONFIG::ADDRESS addr = l1_access.addr;
	typename CONFIG::DL1_CONFIG::ADDRESS line_base_addr;
	typename CONFIG::DL1_CONFIG::ADDRESS block_base_addr;
	uint32_t index;
	uint32_t sector;
	uint32_t offset;
	uint32_t size_to_block_boundary;
	uint32_t way;
	CacheSet<typename CONFIG::DL1_CONFIG> *set;
	CacheLine<typename CONFIG::DL1_CONFIG> *line;
	CacheBlock<typename CONFIG::DL1_CONFIG> *block;
	
	num_dl1_accesses++;
	// Decode the address
	dl1.DecodeAddress(addr, line_base_addr, block_base_addr, index, sector, offset, size_to_block_boundary);
	if(unlikely(IsVerboseDL1()))
	{
		logger << DebugInfo << "DL1 Lookup at 0x";
		logger << std::hex << addr << std::dec << " : ";
		logger << "line_base_addr=0x" << std::hex << line_base_addr << std::dec << ",";
		logger << "index=0x" << std::hex << index << std::dec << ",";
		logger << "sector=0x" << std::hex << sector << std::dec << ",";
		logger << "offset=0x" << std::hex << offset << std::dec << ",";
		logger << "size_to_block_boundary=0x" << std::hex << size_to_block_boundary << std::dec;
		logger << endl << EndDebugInfo;
	}
	l1_access.line_base_addr = line_base_addr;
	l1_access.block_base_addr = block_base_addr;
	l1_access.index = index;
	l1_access.sector = sector;
	l1_access.offset = offset;
	l1_access.size_to_block_boundary = size_to_block_boundary;
	l1_access.set = set = &dl1[index];

	// Associative search
	if(CONFIG::FAST_DL1_LOOKUP_ENABLE)
	{
		// Fast lookup algorithm
		for(line = set->status.mru_line; line != 0; line = line->status.next)
		{
			if(likely(line->status.valid && line->GetBaseAddr() == line_base_addr))
			{
				if(unlikely(IsVerboseDL1()))
				{
					logger << DebugInfo << "DL1 Line hit: index=" << l1_access.index << ", way=" << line->status.way << endl << EndDebugInfo;
				}
				// line hit: block may need a fill if not yet present in the line
				l1_access.line = line;
				l1_access.line_to_evict = 0;
				l1_access.way = line->status.way;
				block = &(*line)[sector];
				l1_access.block = block->status.valid ? block : 0;
				if(unlikely(IsVerboseDL1()))
				{
					logger << DebugInfo << "DL1 block " << (block->status.valid ? "hit" : "miss") << endl << EndDebugInfo;
				}

				if(unlikely(l1_access.line->status.prev != 0))
				{
					if(l1_access.line->status.next)
					{
						l1_access.line->status.next->status.prev = l1_access.line->status.prev;
					}
					else
					{
						l1_access.set->status.lru_line = l1_access.line->status.prev;
					}
					l1_access.line->status.prev->status.next = l1_access.line->status.next;
					l1_access.line->status.prev = 0;
					l1_access.line->status.next = l1_access.set->status.mru_line;
					l1_access.set->status.mru_line->status.prev = l1_access.line;
					l1_access.set->status.mru_line = l1_access.line;
				}
				return;
			}
		}
	}
	else
	{
		// Slow lookup algorithm
		for(way = 0; way < CONFIG::DL1_CONFIG::CACHE_ASSOCIATIVITY; way++)
		{
			line = &(*set)[way];
			if(line->status.valid && line->GetBaseAddr() == line_base_addr)
			{
				if(unlikely(IsVerboseDL1()))
				{
					logger << DebugInfo << "DL1 Line hit: way=" << way << endl << EndDebugInfo;
				}
				// line hit: block may need a fill if not yet present in the line
				l1_access.line = line;
				l1_access.line_to_evict = 0;
				l1_access.way = way;
				block = &(*line)[sector];
				l1_access.block = block->status.valid ? block : 0;
				if(unlikely(IsVerboseDL1()))
				{
					logger << DebugInfo << "DL1 block " << (block->status.valid ? "hit" : "miss") << endl << EndDebugInfo;
				}
				return;
			}
		}
	}

	// line miss
	l1_access.line = 0;
	l1_access.block = 0;
	num_dl1_misses++;
}

template <class CONFIG>
inline void CPU<CONFIG>::LookupIL1(CacheAccess<typename CONFIG::IL1_CONFIG>& l1_access)
{
	typename CONFIG::IL1_CONFIG::ADDRESS addr = l1_access.addr;
	typename CONFIG::IL1_CONFIG::ADDRESS line_base_addr;
	typename CONFIG::IL1_CONFIG::ADDRESS block_base_addr;
	uint32_t index;
	uint32_t sector;
	uint32_t offset;
	uint32_t size_to_block_boundary;
	uint32_t way;
	CacheSet<typename CONFIG::IL1_CONFIG> *set;
	CacheLine<typename CONFIG::IL1_CONFIG> *line;
	CacheBlock<typename CONFIG::IL1_CONFIG> *block;
	
	num_il1_accesses++;
	// Decode the address
	il1.DecodeAddress(addr, line_base_addr, block_base_addr, index, sector, offset, size_to_block_boundary);
	if(unlikely(IsVerboseIL1()))
	{
		logger << DebugInfo << "IL1 Lookup at 0x";
		logger << std::hex << addr << std::dec << " : ";
		logger << "line_base_addr=0x" << std::hex << line_base_addr << std::dec << ",";
		logger << "index=0x" << std::hex << index << std::dec << ",";
		logger << "sector=0x" << std::hex << sector << std::dec << ",";
		logger << "offset=0x" << std::hex << offset << std::dec << ",";
		logger << "size_to_block_boundary=0x" << std::hex << size_to_block_boundary << std::dec;
		logger << endl << EndDebugInfo;
	}
	l1_access.line_base_addr = line_base_addr;
	l1_access.block_base_addr = block_base_addr;
	l1_access.index = index;
	l1_access.sector = sector;
	l1_access.offset = offset;
	l1_access.size_to_block_boundary = size_to_block_boundary;
	l1_access.set = set = &il1[index];

	// Associative search
	if(CONFIG::FAST_IL1_LOOKUP_ENABLE)
	{
		// Fast lookup algorithm
		for(line = set->status.mru_line; line != 0; line = line->status.next)
		{
			if(likely(line->status.valid && line->GetBaseAddr() == line_base_addr))
			{
				if(unlikely(IsVerboseIL1()))
				{
					logger << DebugInfo << "IL1 Line hit: " << l1_access.index << ", way=" << line->status.way << endl << EndDebugInfo;
				}
				// line hit: block may need a fill if not yet present in the line
				l1_access.line = line;
				l1_access.line_to_evict = 0;
				l1_access.way = line->status.way;
				block = &(*line)[sector];
				l1_access.block = block->status.valid ? block : 0;
				if(unlikely(IsVerboseIL1()))
				{
					logger << DebugInfo << "IL1 block " << (block->status.valid ? "hit" : "miss") << endl << EndDebugInfo;
				}

				if(unlikely(l1_access.line->status.prev != 0))
				{
					if(l1_access.line->status.next)
					{
						l1_access.line->status.next->status.prev = l1_access.line->status.prev;
					}
					else
					{
						l1_access.set->status.lru_line = l1_access.line->status.prev;
					}
					l1_access.line->status.prev->status.next = l1_access.line->status.next;
					l1_access.line->status.prev = 0;
					l1_access.line->status.next = l1_access.set->status.mru_line;
					l1_access.set->status.mru_line->status.prev = l1_access.line;
					l1_access.set->status.mru_line = l1_access.line;
				}
				return;
			}
		}
	}
	else
	{
		// Slow lookup algorithm
		for(way = 0; way < CONFIG::IL1_CONFIG::CACHE_ASSOCIATIVITY; way++)
		{
			line = &(*set)[way];
			if(line->status.valid && line->GetBaseAddr() == line_base_addr)
			{
				if(unlikely(IsVerboseIL1()))
				{
					logger << DebugInfo << "IL1 Line hit: way=" << way << endl << EndDebugInfo;
				}
				// line hit: block may need a fill if not yet present in the line
				l1_access.line = line;
				l1_access.line_to_evict = 0;
				l1_access.way = way;
				block = &(*line)[sector];
				l1_access.block = block->status.valid ? block : 0;
				if(unlikely(IsVerboseIL1()))
				{
					logger << DebugInfo << "IL1 block " << (block->status.valid ? "hit" : "miss") << endl << EndDebugInfo;
				}
				return;
			}
		}
	}

	// line miss
	l1_access.line = 0;
	l1_access.block = 0;
	num_il1_misses++;
}

template <class CONFIG>
void CPU<CONFIG>::LookupL2(CacheAccess<typename CONFIG::L2_CONFIG>& l2_access)
{
	typename CONFIG::L2_CONFIG::ADDRESS addr = l2_access.addr;
	typename CONFIG::L2_CONFIG::ADDRESS line_base_addr;
	typename CONFIG::L2_CONFIG::ADDRESS block_base_addr;
	uint32_t index;
	uint32_t sector;
	uint32_t offset;
	uint32_t size_to_block_boundary;
	uint32_t way;
	CacheSet<typename CONFIG::L2_CONFIG> *set;
	CacheLine<typename CONFIG::L2_CONFIG> *line;
	CacheBlock<typename CONFIG::L2_CONFIG> *block;
	
	num_l2_accesses++;
	// Decode the address
	l2.DecodeAddress(addr, line_base_addr, block_base_addr, index, sector, offset, size_to_block_boundary);
	if(unlikely(IsVerboseL2()))
	{
		logger << DebugInfo << "L2 Lookup at 0x";
		logger << std::hex << addr << std::dec << " : ";
		logger << "line_base_addr=0x" << std::hex << line_base_addr << std::dec << ",";
		logger << "index=0x" << std::hex << index << std::dec << ",";
		logger << "sector=0x" << std::hex << sector << std::dec << ",";
		logger << "offset=0x" << std::hex << offset << std::dec << ",";
		logger << "size_to_block_boundary=0x" << std::hex << size_to_block_boundary << std::dec;
		logger << endl << EndDebugInfo;
	}
	l2_access.line_base_addr = line_base_addr;
	l2_access.block_base_addr = block_base_addr;
	l2_access.index = index;
	l2_access.sector = sector;
	l2_access.offset = offset;
	l2_access.size_to_block_boundary = size_to_block_boundary;
	l2_access.set = set = &l2[index];

	if(CONFIG::FAST_L2_LOOKUP_ENABLE)
	{
		// Fast lookup algorithm
		for(line = set->status.mru_line; line != 0; line = line->status.next)
		{
			if(likely(line->status.valid && line->GetBaseAddr() == line_base_addr))
			{
				if(unlikely(IsVerboseL2()))
				{
					logger << DebugInfo << "L2 Line hit: " << l2_access.index << ", way=" << line->status.way << endl << EndDebugInfo;
				}
				// line hit: block may need a fill if not yet present in the line
				l2_access.line = line;
				l2_access.line_to_evict = 0;
				l2_access.way = line->status.way;
				block = &(*line)[sector];
				l2_access.block = block->status.valid ? block : 0;
				if(unlikely(IsVerboseL2()))
				{
					logger << DebugInfo << "L2 block " << (block->status.valid ? "hit" : "miss") << endl << EndDebugInfo;
				}

				if(unlikely(l2_access.line->status.prev != 0))
				{
					if(l2_access.line->status.next)
					{
						l2_access.line->status.next->status.prev = l2_access.line->status.prev;
					}
					else
					{
						l2_access.set->status.lru_line = l2_access.line->status.prev;
					}
					l2_access.line->status.prev->status.next = l2_access.line->status.next;
					l2_access.line->status.prev = 0;
					l2_access.line->status.next = l2_access.set->status.mru_line;
					l2_access.set->status.mru_line->status.prev = l2_access.line;
					l2_access.set->status.mru_line = l2_access.line;
				}
				return;
			}
		}
	}
	else
	{
		// Slow lookup algorithm
		for(way = 0; way < CONFIG::L2_CONFIG::CACHE_ASSOCIATIVITY; way++)
		{
			line = &(*set)[way];
			if(line->status.valid && line->GetBaseAddr() == line_base_addr)
			{
				if(unlikely(IsVerboseL2()))
				{
					logger << DebugInfo << "L2 Line hit: way=" << way << endl << EndDebugInfo;
				}
				// line hit: block may need a fill if not yet present in the line
				l2_access.line = line;
				l2_access.line_to_evict = 0;
				l2_access.way = way;
				block = &(*line)[sector];
				l2_access.block = block->status.valid ? block : 0;
				if(unlikely(IsVerboseL2()))
				{
					logger << DebugInfo << "L2 block " << (block->status.valid ? "hit" : "miss") << endl << EndDebugInfo;
				}
				return;
			}
		}
	}

	// line miss
	l2_access.line = 0;
	l2_access.block = 0;
	num_l2_misses++;
}

template <class CONFIG>
inline void CPU<CONFIG>::EmuEvictDL1(CacheAccess<typename CONFIG::DL1_CONFIG>& l1_access)
{
	if(unlikely(IsVerboseDL1()))
	{
		logger << DebugInfo << "DL1: evicting line at 0x" << std::hex << l1_access.addr << std::dec << endl << EndDebugInfo;
	}
	if(l1_access.line_to_evict->status.valid)
	{
		uint32_t sector;
	
		for(sector = 0; sector < CacheLine<class CONFIG::DL1_CONFIG>::BLOCKS_PER_LINE; sector++)
		{
			CacheBlock<class CONFIG::DL1_CONFIG>& l1_block_to_evict = (*l1_access.line_to_evict)[sector];
	
			if(l1_block_to_evict.status.valid && l1_block_to_evict.status.dirty)
			{
				// dirty DL1 block eviction
				if(IsL2CacheEnabled())
				{
					// DL1 block eviction into L2
					CacheAccess<class CONFIG::L2_CONFIG> l2_access;

					l2_access.addr = l1_block_to_evict.GetBaseAddr();
					LookupL2(l2_access);

					if(l2_access.block)
					{
						if(unlikely(IsVerboseDL1()))
						{
							logger << DebugInfo << "DL1: evicting dirty block at 0x" << std::hex << l1_block_to_evict.GetBaseAddr() << std::dec << " into L2" << endl << EndDebugInfo;
						}
						memcpy(&(*l2_access.block)[l2_access.offset], &l1_block_to_evict[0], CacheBlock<typename CONFIG::DL1_CONFIG>::SIZE);
						l2_access.block->status.dirty = true;
	
						//UpdateReplacementPolicyL2(l2_access);
						
						if(unlikely(l2_power_estimator_import != 0)) l2_power_estimator_import->ReportWriteAccess();
					}
					else
					{
						if(unlikely(IsVerboseDL1()))
						{
							logger << DebugInfo << "DL1: evicting dirty block at 0x" << std::hex << l1_block_to_evict.GetBaseAddr() << std::dec << " into memory" << endl << EndDebugInfo;
						}
						// dirty DL1 block eviction into memory
						// MPC7450UM, Rev. 5, paragraphe 3.8.3, p3-91: Because cache block castouts and snoop pushes do not require snooping, the GBL signal is not asserted for these operations.
						BusWrite(l1_block_to_evict.GetBaseAddr(), &l1_block_to_evict[0], CacheBlock<class CONFIG::DL1_CONFIG>::SIZE);
					}
				}
				else
				{
					if(unlikely(IsVerboseDL1()))
					{
						logger << DebugInfo << "DL1: evicting dirty block at 0x" << std::hex << l1_block_to_evict.GetBaseAddr() << std::dec << " into memory" << endl << EndDebugInfo;
					}
					// dirty DL1 block eviction into memory
					// MPC7450UM, Rev. 5, paragraphe 3.8.3, p3-91: Because cache block castouts and snoop pushes do not require snooping, the GBL signal is not asserted for these operations.
					BusWrite(l1_block_to_evict.GetBaseAddr(), &l1_block_to_evict[0], CacheBlock<class CONFIG::DL1_CONFIG>::SIZE);
				}
			}
			l1_block_to_evict.status.valid = false;
			l1_block_to_evict.status.dirty = false;
		}
	
		l1_access.line_to_evict->status.valid = false;
	}
	l1_access.line = l1_access.line_to_evict;
	l1_access.line_to_evict = 0;
}

template <class CONFIG>
inline void CPU<CONFIG>::EmuEvictIL1(CacheAccess<typename CONFIG::IL1_CONFIG>& l1_access)
{
	if(unlikely(IsVerboseIL1()))
	{
		logger << DebugInfo << "IL1: evicting line at 0x" << std::hex << l1_access.addr << std::dec << endl << EndDebugInfo;
	}
	if(l1_access.line_to_evict->status.valid)
	{
		uint32_t sector;
	
		for(sector = 0; sector < CacheLine<class CONFIG::DL1_CONFIG>::BLOCKS_PER_LINE; sector++)
		{
			CacheBlock<class CONFIG::IL1_CONFIG>& l1_block_to_evict = (*l1_access.line_to_evict)[sector];
	
			l1_block_to_evict.status.valid = false;
		}
	
		l1_access.line_to_evict->status.valid = false;
	}
	l1_access.line = l1_access.line_to_evict;
	l1_access.line_to_evict = 0;
}

template <class CONFIG>
void CPU<CONFIG>::EmuEvictL2(CacheAccess<typename CONFIG::L2_CONFIG>& l2_access)
{
	if(unlikely(IsVerboseL2()))
	{
		logger << DebugInfo << "L2: evicting line at 0x" << std::hex << l2_access.addr << std::dec << endl << EndDebugInfo;
	}
	if(l2_access.line_to_evict->status.valid)
	{
		uint32_t l2_sector;
	
		for(l2_sector = 0; l2_sector < CacheLine<class CONFIG::L2_CONFIG>::BLOCKS_PER_LINE; l2_sector++)
		{
			CacheBlock<class CONFIG::L2_CONFIG>& l2_block_to_evict = (*l2_access.line_to_evict)[l2_sector];
	
			if(l2_block_to_evict.status.valid && l2_block_to_evict.status.dirty)
			{
				// dirty L2 block eviction into memory
				// MPC7450UM, Rev. 5, paragraphe 3.8.3, p3-91: Because cache block castouts and snoop pushes do not require snooping, the GBL signal is not asserted for these operations.
				if(unlikely(IsVerboseL2()))
				{
					logger << DebugInfo << "L2: evicting dirty block at 0x" << std::hex << l2_block_to_evict.GetBaseAddr() << std::dec << " into memory" << endl << EndDebugInfo;
				}
				BusWrite(l2_block_to_evict.GetBaseAddr(), &l2_block_to_evict[0], CacheBlock<class CONFIG::L2_CONFIG>::SIZE);
			}
			l2_block_to_evict.status.valid = false;
			l2_block_to_evict.status.dirty = false;
		}
	
		l2_access.line_to_evict->status.valid = false;
	}
	l2_access.line = l2_access.line_to_evict;
	l2_access.line_to_evict = 0;
}

template <class CONFIG>
inline void CPU<CONFIG>::ChooseLineToEvictDL1(CacheAccess<typename CONFIG::DL1_CONFIG>& l1_access)
{
	uint32_t way;
	uint32_t i;
	uint32_t n;
	uint32_t plru_bits = l1_access.set->status.plru_bits;

	if(unlikely(IsVerboseDL1()))
	{
		logger << DebugInfo << "DL1 PLRU bits:";
		for(i = 0; i < CONFIG::DL1_CONFIG::CACHE_ASSOCIATIVITY - 1; i++)
		{
			logger << "B" << i << "=" << ((plru_bits >> i) & 1) << " ";
		}
		logger << endl << EndDebugInfo;
	}
	for(i = 0, way = 0, n = 0; n < CONFIG::DL1_CONFIG::CACHE_LOG_ASSOCIATIVITY; n++)
	{
		uint32_t bi = (plru_bits >> i) & 1;
		way = (way << 1) | bi;
		i = (i << 1) + 1 + bi;
	}
//  	cerr << "way=" << way << endl;

	l1_access.way = way;
	l1_access.line_to_evict = &(*l1_access.set)[way];
}

template <class CONFIG>
inline void CPU<CONFIG>::ChooseLineToEvictIL1(CacheAccess<typename CONFIG::IL1_CONFIG>& l1_access)
{
	uint32_t way;
	uint32_t i;
	uint32_t n;
	uint32_t plru_bits = l1_access.set->status.plru_bits;

	if(unlikely(IsVerboseIL1()))
	{
		logger << DebugInfo << "IL1 PLRU bits:";
		for(i = 0; i < CONFIG::IL1_CONFIG::CACHE_ASSOCIATIVITY - 1; i++)
		{
			logger << "B" << i << "=" << ((plru_bits >> i) & 1) << " ";
		}
		logger << endl << EndDebugInfo;
	}
	for(i = 0, way = 0, n = 0; n < CONFIG::IL1_CONFIG::CACHE_LOG_ASSOCIATIVITY; n++)
	{
		uint32_t bi = (plru_bits >> i) & 1;
		way = (way << 1) | bi;
		i = (i << 1) + 1 + bi;
	}
//  	cerr << "way=" << way << endl;

	l1_access.way = way;
	l1_access.line_to_evict = &(*l1_access.set)[way];
}

template <class CONFIG>
void CPU<CONFIG>::ChooseLineToEvictL2(CacheAccess<typename CONFIG::L2_CONFIG>& l2_access)
{
	uint32_t way;
	uint32_t i;
	uint32_t n;
	uint32_t plru_bits = l2_access.set->status.plru_bits;

	if(unlikely(IsVerboseL2()))
	{
		logger << DebugInfo << "L2 PLRU bits:";
		for(i = 0; i < CONFIG::L2_CONFIG::CACHE_ASSOCIATIVITY - 1; i++)
		{
			logger << "B" << i << "=" << ((plru_bits >> i) & 1) << " ";
		}
		logger << endl << EndDebugInfo;
	}
	for(i = 0, way = 0, n = 0; n < CONFIG::L2_CONFIG::CACHE_LOG_ASSOCIATIVITY; n++)
	{
		uint32_t bi = (plru_bits >> i) & 1;
		way = (way << 1) | bi;
		i = (i << 1) + 1 + bi;
	}
//  	cerr << "way=" << way << endl;

	l2_access.way = way;
	l2_access.line_to_evict = &(*l2_access.set)[way];
}

template <class CONFIG>
inline void CPU<CONFIG>::UpdateReplacementPolicyDL1(CacheAccess<typename CONFIG::DL1_CONFIG>& l1_access)
{
	uint32_t i;
	uint32_t n;
	uint32_t way = l1_access.way;
	uint32_t plru_bits = l1_access.set->status.plru_bits;

	for(n = CONFIG::DL1_CONFIG::CACHE_LOG_ASSOCIATIVITY, i = 0; n != 0; n--)
	{
		uint32_t bi = (way >> (n - 1)) & 1;
		plru_bits = (plru_bits & ~(1 << i)) | ((1 ^ bi) << i);
		i = (i << 1) + 1 + bi;
	}

	l1_access.set->status.plru_bits = plru_bits;
}

template <class CONFIG>
inline void CPU<CONFIG>::UpdateReplacementPolicyIL1(CacheAccess<typename CONFIG::IL1_CONFIG>& l1_access)
{
	uint32_t i;
	uint32_t n;
	uint32_t way = l1_access.way;
	uint32_t plru_bits = l1_access.set->status.plru_bits;

	for(n = CONFIG::IL1_CONFIG::CACHE_LOG_ASSOCIATIVITY, i = 0; n != 0; n--)
	{
		uint32_t bi = (way >> (n - 1)) & 1;
		plru_bits = (plru_bits & ~(1 << i)) | ((1 ^ bi) << i);
		i = (i << 1) + 1 + bi;
	}

	l1_access.set->status.plru_bits = plru_bits;
}

template <class CONFIG>
void CPU<CONFIG>::UpdateReplacementPolicyL2(CacheAccess<typename CONFIG::L2_CONFIG>& l2_access)
{
	uint32_t i;
	uint32_t n;
	uint32_t way = l2_access.way;
	uint32_t plru_bits = l2_access.set->status.plru_bits;

	for(n = CONFIG::L2_CONFIG::CACHE_LOG_ASSOCIATIVITY, i = 0; n != 0; n--)
	{
		uint32_t bi = (way >> (n - 1)) & 1;
		plru_bits = (plru_bits & ~(1 << i)) | ((1 ^ bi) << i);
		i = (i << 1) + 1 + bi;
	}

	l2_access.set->status.plru_bits = plru_bits;
}

template <class CONFIG>
inline void CPU<CONFIG>::EmuFillDL1(CacheAccess<typename CONFIG::DL1_CONFIG>& l1_access, typename CONFIG::WIMG wimg, bool rwitm)
{
	l1_access.block = &(*l1_access.line)[l1_access.sector];
	if(unlikely(unlikely(IsVerboseDL1())))
	{
		logger << DebugInfo << "DL1: filling block at 0x" << std::hex << l1_access.block_base_addr << std::dec << endl << EndDebugInfo;
	}
	// DL1 block fill
	if(likely(IsL2CacheEnabled()))
	{
		// DL1 block fill from L2
		CacheAccess<class CONFIG::L2_CONFIG> l2_access;
	
		l2_access.addr = l1_access.block_base_addr;
	
		LookupL2(l2_access);
	
		if(unlikely(!l2_access.line))
		{
			if(unlikely(IsVerboseL2()))
			{
				logger << DebugInfo << "L2: line miss at 0x" << std::hex << l2_access.addr << std::dec << endl << EndDebugInfo;
			}
			
			ChooseLineToEvictL2(l2_access);
			if(unlikely(IsVerboseL2()))
			{
				logger << DebugInfo << "L2 line miss: choosen way=" << l2_access.way << endl << EndDebugInfo;
			}
			
			EmuEvictL2(l2_access);
		}
	
		if(unlikely(!l2_access.block))
		{
			if(unlikely(IsVerboseL2()))
			{
				logger << DebugInfo << "L2: block miss at 0x" << std::hex << l2_access.addr << std::dec << endl << EndDebugInfo;
			}
			EmuFillL2(l2_access, wimg, rwitm);
		}

		memcpy(&(*l1_access.block)[0], &(*l2_access.block)[l2_access.offset], CacheBlock<class CONFIG::DL1_CONFIG>::SIZE);
		//UpdateReplacementPolicyL2(l2_access);

		if(unlikely(l2_power_estimator_import != 0)) l2_power_estimator_import->ReportReadAccess();
	}
	else
	{
		// DL1 block fill from memory
		BusRead(l1_access.block_base_addr, &(*l1_access.block)[0], CacheBlock<class CONFIG::DL1_CONFIG>::SIZE, wimg, rwitm);
	}

	l1_access.line->status.valid = true;
	l1_access.line->SetBaseAddr(l1_access.line_base_addr);
	l1_access.block->status.valid = true;
	l1_access.block->status.dirty = false;
	UpdateReplacementPolicyDL1(l1_access);
}

template <class CONFIG>
inline void CPU<CONFIG>::EmuFillIL1(CacheAccess<typename CONFIG::IL1_CONFIG>& l1_access, typename CONFIG::WIMG wimg)
{
	l1_access.block = &(*l1_access.line)[l1_access.sector];
	if(unlikely(IsVerboseIL1()))
	{
		logger << DebugInfo << "IL1: filling block at 0x" << std::hex << l1_access.block_base_addr << std::dec << endl << EndDebugInfo;
	}

	// DL1 block fill
	if(likely(IsL2CacheEnabled()))
	{
		// DL1 block fill from L2
		CacheAccess<class CONFIG::L2_CONFIG> l2_access;
	
		l2_access.addr = l1_access.block_base_addr;
	
		LookupL2(l2_access);
	
		if(unlikely(!l2_access.line))
		{
			if(unlikely(IsVerboseL2()))
			{
				logger << DebugInfo << "L2: line miss at 0x" << std::hex << l2_access.addr << std::dec << endl << EndDebugInfo;
			}

			ChooseLineToEvictL2(l2_access);
			if(unlikely(IsVerboseL2()))
			{
				logger << DebugInfo << "L2 line miss: choosen way=" << l2_access.way << endl << EndDebugInfo;
			}

			EmuEvictL2(l2_access);
		}
	
		if(unlikely(!l2_access.block))
		{
			if(unlikely(IsVerboseL2()))
			{
				logger << DebugInfo << "L2: block miss at 0x" << std::hex << l2_access.addr << std::dec << endl << EndDebugInfo;
			}
			EmuFillL2(l2_access, wimg, false);
		}
		memcpy(&(*l1_access.block)[0], &(*l2_access.block)[l2_access.offset], CacheBlock<class CONFIG::DL1_CONFIG>::SIZE);
		//UpdateReplacementPolicyL2(l2_access);

		if(unlikely(l2_power_estimator_import != 0)) l2_power_estimator_import->ReportReadAccess();
	}
	else
	{
		// DL1 block fill from memory
		BusRead(l1_access.block_base_addr, &(*l1_access.block)[0], CacheBlock<class CONFIG::DL1_CONFIG>::SIZE, wimg, false);
	}

	l1_access.line->status.valid = true;
	l1_access.line->SetBaseAddr(l1_access.line_base_addr);
	l1_access.block->status.valid = true;
	UpdateReplacementPolicyIL1(l1_access);
}

template <class CONFIG>
void CPU<CONFIG>::EmuFillL2(CacheAccess<typename CONFIG::L2_CONFIG>& l2_access, typename CONFIG::WIMG wimg, bool rwitm)
{
	l2_access.block = &(*l2_access.line)[l2_access.sector];
	if(unlikely(IsVerboseL2()))
	{
		logger << DebugInfo << "L2: filling block at 0x" << std::hex << l2_access.block_base_addr << std::dec << endl << EndDebugInfo;
	}
	BusRead(l2_access.block_base_addr, &(*l2_access.block)[0], CacheBlock<class CONFIG::L2_CONFIG>::SIZE, wimg, rwitm);
	l2_access.line->status.valid = true;
	l2_access.line->SetBaseAddr(l2_access.line_base_addr);
	l2_access.block->status.valid = true;
	l2_access.block->status.dirty = false;
	UpdateReplacementPolicyL2(l2_access);
}

/* Data Cache management */
template <class CONFIG>
void CPU<CONFIG>::Dcba(typename CONFIG::address_t addr)
{
	Dcbz(addr);
}

template <class CONFIG>
void CPU<CONFIG>::Dcbf(typename CONFIG::address_t addr)
{
	Dcbst(addr);
}

template <class CONFIG>
void CPU<CONFIG>::Dcbi(typename CONFIG::address_t addr)
{
	if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();

	typename CONFIG::WIMG wimg;
	typename CONFIG::physical_address_t physical_addr;
	
	if(GetMSR_DR())
	{
		// Address translation
		MMUAccess<CONFIG> mmu_access;
	
		mmu_access.addr = addr;
		mmu_access.privilege_level = GetPrivilegeLevel();
		mmu_access.memory_access_type = CONFIG::MAT_WRITE;
		mmu_access.memory_type = CONFIG::MT_DATA;

		EmuTranslateAddress<false>(mmu_access);

		wimg = mmu_access.wimg;
		physical_addr = mmu_access.physical_addr;
	}
	else
	{
		// W=0: Write through disabled
		// I=0: Cache not inhibited
		// M=1: Memory coherency enforced
		// G=1: Guarded memory access (speculative fetch forbidden)
		wimg = (typename CONFIG::WIMG)(CONFIG::WIMG_MEMORY_COHERENCY_ENFORCED | CONFIG::WIMG_GUARDED_MEMORY);
		physical_addr = addr;
	}

	bool dirty = false;

	// DL1 Access
	CacheAccess<class CONFIG::DL1_CONFIG> l1_access;
	l1_access.addr = physical_addr;

	LookupDL1(l1_access);

	if(l1_access.line)
	{
		uint32_t l1_sector;
	
		for(l1_sector = 0; l1_sector < CacheLine<class CONFIG::DL1_CONFIG>::BLOCKS_PER_LINE; l1_sector++)
		{
			CacheBlock<class CONFIG::DL1_CONFIG>& l1_block_to_flush = (*l1_access.line)[l1_sector];
	
			if(l1_block_to_flush.status.valid && l1_block_to_flush.status.dirty)
			{
				// DL1 block is dirty
				// Write back to L2
				dirty = true;
				CacheAccess<class CONFIG::L2_CONFIG> l2_access;
				l2_access.addr = l1_block_to_flush.GetBaseAddr();
	
				LookupL2(l2_access);
	
				if(l2_access.block)
				{
					memcpy(&(*l2_access.block)[l2_access.offset], &l1_block_to_flush[0], CacheBlock<typename CONFIG::DL1_CONFIG>::SIZE);
					l2_access.block->status.dirty = true;
				}
				else
				{
					if(unlikely(IsVerboseDL1()))
					{
						logger << DebugInfo << "DL1: flushing/invalidating dirty block at 0x" << std::hex << l1_block_to_flush.GetBaseAddr() << std::dec << " into memory" << endl;
					}
					// MPC7450UM, Rev. 5, paragraphe 3.8.3, p3-91: Because cache block castouts and snoop pushes do not require snooping, the GBL signal is not asserted for these operations.
					BusWrite(l1_block_to_flush.GetBaseAddr(), &l1_block_to_flush[0], CacheBlock<class CONFIG::DL1_CONFIG>::SIZE);
				}
			}
			l1_block_to_flush.status.valid = false;
			l1_block_to_flush.status.dirty = false;
		}
		l1_access.line->status.valid = false;
	}

	CacheAccess<class CONFIG::L2_CONFIG> l2_access;
	l2_access.addr = physical_addr;

	LookupL2(l2_access);

	if(l2_access.line)
	{
		uint32_t l2_sector;
	
		for(l2_sector = 0; l2_sector < CacheLine<class CONFIG::L2_CONFIG>::BLOCKS_PER_LINE; l2_sector++)
		{
			CacheBlock<class CONFIG::L2_CONFIG>& l2_block_to_flush = (*l2_access.line)[l2_sector];

			if(l2_block_to_flush.status.valid && l2_block_to_flush.status.dirty)
			{
				dirty = true;
				if(unlikely(IsVerboseL2()))
				{
					logger << DebugInfo << "L2: flushing/invalidating dirty block at 0x" << std::hex << l2_block_to_flush.GetBaseAddr() << std::dec << " into memory" << endl;
				}
				// MPC7450UM, Rev. 5, paragraphe 3.8.3, p3-91: Because cache block castouts and snoop pushes do not require snooping, the GBL signal is not asserted for these operations.
					BusWrite(l2_block_to_flush.GetBaseAddr(), &l2_block_to_flush[0], CacheBlock<class CONFIG::L2_CONFIG>::SIZE);
			}
			l2_block_to_flush.status.valid = false;
			l2_block_to_flush.status.dirty = false;
		}
		l2_access.line->status.valid = false;
	}

	if(dirty || (IsAddressBroadcastEnabled() && (wimg & CONFIG::WIMG_MEMORY_COHERENCY_ENFORCED)))
	{
		BusFlushBlock(physical_addr);
	}
}

template <class CONFIG>
void CPU<CONFIG>::Dcbst(typename CONFIG::address_t addr)
{
	typename CONFIG::WIMG wimg;
	typename CONFIG::physical_address_t physical_addr;
	
	if(GetMSR_DR())
	{
		// Address translation
		MMUAccess<CONFIG> mmu_access;
	
		mmu_access.addr = addr;
		mmu_access.privilege_level = GetPrivilegeLevel();
		mmu_access.memory_access_type = CONFIG::MAT_READ; // 3.4.4.4 Data Cache Block Store (dcbst): This instruction is treated as a load with respect to address translation and memory protection
		mmu_access.memory_type = CONFIG::MT_DATA;

		EmuTranslateAddress<false>(mmu_access);

		wimg = mmu_access.wimg;
		physical_addr = mmu_access.physical_addr;
	}
	else
	{
		// W=0: Write through disabled
		// I=0: Cache not inhibited
		// M=1: Memory coherency enforced
		// G=1: Guarded memory access (speculative fetch forbidden)
		wimg = (typename CONFIG::WIMG)(CONFIG::WIMG_MEMORY_COHERENCY_ENFORCED | CONFIG::WIMG_GUARDED_MEMORY);
		physical_addr = addr;
	}

	bool dirty = false;

	// DL1 Access
	CacheAccess<class CONFIG::DL1_CONFIG> l1_access;
	l1_access.addr = physical_addr;

	LookupDL1(l1_access);

	if(l1_access.line)
	{
		uint32_t l1_sector;
	
		for(l1_sector = 0; l1_sector < CacheLine<class CONFIG::DL1_CONFIG>::BLOCKS_PER_LINE; l1_sector++)
		{
			CacheBlock<class CONFIG::DL1_CONFIG>& l1_block_to_flush = (*l1_access.line)[l1_sector];
	
			if(l1_block_to_flush.status.valid && l1_block_to_flush.status.dirty)
			{
				// DL1 block is dirty
				// Write back to L2
				dirty = true;
				CacheAccess<class CONFIG::L2_CONFIG> l2_access;
				l2_access.addr = l1_block_to_flush.GetBaseAddr();
	
				LookupL2(l2_access);
	
				if(l2_access.block)
				{
					memcpy(&(*l2_access.block)[l2_access.offset], &l1_block_to_flush[0], CacheBlock<typename CONFIG::DL1_CONFIG>::SIZE);
					l2_access.block->status.dirty = true;
				}
				else
				{
					if(unlikely(IsVerboseDL1()))
					{
						logger << DebugInfo << "DL1: flushing/invalidating dirty block at 0x" << std::hex << l1_block_to_flush.GetBaseAddr() << std::dec << " into memory" << endl;
					}
					// MPC7450UM, Rev. 5, paragraphe 3.8.3, p3-91: Because cache block castouts and snoop pushes do not require snooping, the GBL signal is not asserted for these operations.
					BusWrite(l1_block_to_flush.GetBaseAddr(), &l1_block_to_flush[0], CacheBlock<class CONFIG::DL1_CONFIG>::SIZE);
				}
			}
			l1_block_to_flush.status.valid = false;
			l1_block_to_flush.status.dirty = false;
		}
		l1_access.line->status.valid = false;
	}

	CacheAccess<class CONFIG::L2_CONFIG> l2_access;
	l2_access.addr = physical_addr;

	LookupL2(l2_access);

	if(l2_access.line)
	{
		uint32_t l2_sector;
	
		for(l2_sector = 0; l2_sector < CacheLine<class CONFIG::L2_CONFIG>::BLOCKS_PER_LINE; l2_sector++)
		{
			CacheBlock<class CONFIG::L2_CONFIG>& l2_block_to_flush = (*l2_access.line)[l2_sector];

			if(l2_block_to_flush.status.valid && l2_block_to_flush.status.dirty)
			{
				dirty = true;
				if(unlikely(IsVerboseL2()))
				{
					logger << DebugInfo << "L2: flushing/invalidating dirty block at 0x" << std::hex << l2_block_to_flush.GetBaseAddr() << std::dec << " into memory" << endl;
				}
				// MPC7450UM, Rev. 5, paragraphe 3.8.3, p3-91: Because cache block castouts and snoop pushes do not require snooping, the GBL signal is not asserted for these operations.
					BusWrite(l2_block_to_flush.GetBaseAddr(), &l2_block_to_flush[0], CacheBlock<class CONFIG::L2_CONFIG>::SIZE);
			}
			l2_block_to_flush.status.valid = false;
			l2_block_to_flush.status.dirty = false;
		}
		l2_access.line->status.valid = false;
	}

	if(dirty || (IsAddressBroadcastEnabled() && (wimg & CONFIG::WIMG_MEMORY_COHERENCY_ENFORCED)))
	{
		BusFlushBlock(physical_addr);
	}
}

template <class CONFIG>
void CPU<CONFIG>::Dcbz(typename CONFIG::address_t addr)
{
	if(CONFIG::DL1_CONFIG::ENABLE && !IsDataCacheEnabled())
	{
		throw AlignmentException<CONFIG>(addr);
	}

	typename CONFIG::WIMG wimg;
	typename CONFIG::physical_address_t physical_addr;
	
	if(GetMSR_DR())
	{
		// Address translation
		MMUAccess<CONFIG> mmu_access;
	
		mmu_access.addr = addr;
		mmu_access.privilege_level = GetPrivilegeLevel();
		mmu_access.memory_access_type = CONFIG::MAT_WRITE; // 3.4.4.3 Data Cache Block Zero (dcbz): The dcbz instruction is treated as a store to the addressed byte with respect to address translation, protection, and pipelining.
		mmu_access.memory_type = CONFIG::MT_DATA;

		EmuTranslateAddress<false>(mmu_access);

		wimg = mmu_access.wimg;
		physical_addr = mmu_access.physical_addr;
	}
	else
	{
		// W=0: Write through disabled
		// I=0: Cache not inhibited
		// M=1: Memory coherency enforced
		// G=1: Guarded memory access (speculative fetch forbidden)
		wimg = (typename CONFIG::WIMG)(CONFIG::WIMG_MEMORY_COHERENCY_ENFORCED | CONFIG::WIMG_GUARDED_MEMORY);
		physical_addr = addr;
	}

	if((wimg & CONFIG::WIMG_CACHE_INHIBITED) || (wimg & CONFIG::WIMG_WRITE_THROUGH))
	{
		throw AlignmentException<CONFIG>(addr);
	}

	if(CONFIG::DL1_CONFIG::ENABLE)
	{
		// DL1 Access
		CacheAccess<class CONFIG::DL1_CONFIG> l1_access;
	
		l1_access.addr = physical_addr;
		LookupDL1(l1_access);
	
		if(!l1_access.line)
		{
			ChooseLineToEvictDL1(l1_access);
			if(unlikely(IsVerboseDL1()))
			{
				logger << DebugInfo << "DL1 line miss: choosen way=" << l1_access.way << endl << EndDebugInfo;
			}
			
			EmuEvictDL1(l1_access);
		}
	
		if(!l1_access.block)
		{
			l1_access.block = &(*l1_access.line)[l1_access.sector];
		}
		
		// DL1 block zero
		l1_access.line->status.valid = true;
		l1_access.line->SetBaseAddr(l1_access.line_base_addr);
		l1_access.block->Zero();
		l1_access.block->status.valid = true;
		l1_access.block->status.dirty = true;
		UpdateReplacementPolicyDL1(l1_access);
		if(unlikely(dl1_power_estimator_import != 0)) dl1_power_estimator_import->ReportWriteAccess();

		if(IsL2CacheEnabled())
		{
			// L2 Access
			CacheAccess<class CONFIG::L2_CONFIG> l2_access;
		
			l2_access.addr = physical_addr;
			LookupL2(l2_access);
		
			if(!l2_access.line)
			{
				ChooseLineToEvictL2(l2_access);
				if(unlikely(IsVerboseL2()))
				{
					logger << DebugInfo << "L2 line miss: choosen way=" << l2_access.way << endl << EndDebugInfo;
				}

				EmuEvictL2(l2_access);
			}
		
			if(!l2_access.block)
			{
				l2_access.block = &(*l2_access.line)[l2_access.sector];
			}
			
			// L2 block zero
			l2_access.line->status.valid = true;
			l2_access.line->SetBaseAddr(l2_access.line_base_addr);
			l2_access.block->Zero();
			l2_access.block->status.valid = true;
			l2_access.block->status.dirty = true;
			UpdateReplacementPolicyL2(l2_access);
			if(unlikely(l2_power_estimator_import != 0)) l2_power_estimator_import->ReportWriteAccess();
		}
	}
	else
	{
		// just in case we don't have a data cache
		uint8_t zero[32];
		memset(zero, 0, sizeof(zero));
		BusWrite(physical_addr & (~31), zero, sizeof(zero));
	}
	
	// broadcast zero block on the bus
	BusZeroBlock(physical_addr);
}

template <class CONFIG>
void CPU<CONFIG>::Dccci(typename CONFIG::address_t addr)
{
	throw IllegalInstructionException<CONFIG>();
}

template <class CONFIG>
void CPU<CONFIG>::Dcread(typename CONFIG::address_t addr, unsigned int rd)
{
	throw IllegalInstructionException<CONFIG>();
}

/* Instruction Cache Management */
template <class CONFIG>
void CPU<CONFIG>::Icbi(typename CONFIG::address_t addr)
{
//	mmu.InvalidateInsnCacheBlock(addr, GetPrivilegeLevel());

	typename CONFIG::IL1_CONFIG::ADDRESS line_base_addr;
	typename CONFIG::IL1_CONFIG::ADDRESS block_base_addr;
	uint32_t index;
	uint32_t sector;
	uint32_t offset;
	uint32_t size_to_block_boundary;

	// Decode the address
	il1.DecodeAddress(addr, line_base_addr, block_base_addr, index, sector, offset, size_to_block_boundary);

	InvalidateIL1Set(index);

	unisim::component::cxx::processor::powerpc::mpc7447a::Decoder<CONFIG>::InvalidateDecodingCacheEntry(addr);
}

template <class CONFIG>
void CPU<CONFIG>::Icbt(typename CONFIG::address_t addr)
{
	throw IllegalInstructionException<CONFIG>();
}

template <class CONFIG>
void CPU<CONFIG>::Iccci(typename CONFIG::address_t addr)
{
	throw IllegalInstructionException<CONFIG>();
}

template <class CONFIG>
void CPU<CONFIG>::Icread(typename CONFIG::address_t addr)
{
	throw IllegalInstructionException<CONFIG>();
}

} // end of namespace mpc7447a
} // end of namespace powerpc
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
