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

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_PPC440_CPU_CACHE_TCC__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_PPC440_CPU_CACHE_TCC__

#ifdef powerpc
#undef powerpc
#endif

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace powerpc {
namespace ppc440 {

using unisim::util::arithmetic::BitScanForward;

template <class CONFIG>
void CPU<CONFIG>::InvalidateDL1Set(uint32_t index)
{
	uint32_t way;

	CacheSet<typename CONFIG::DL1_CONFIG>& l1_set = dl1[index];

	unsigned ctrl_reg_num = index / 4;
	unsigned ctrl_reg_field_num = index & 3;
	unsigned int ctrl_reg_field_shift = 8 * (3 - ctrl_reg_field_num);
	uint32_t ctrl_reg_field_mask = 0xffUL << ctrl_reg_field_shift;
	
	SetDTV(ctrl_reg_num, GetDTV(ctrl_reg_num) & ~ctrl_reg_field_mask); // reset way to 0
	SetDNV(ctrl_reg_num, GetDNV(ctrl_reg_num) & ~ctrl_reg_field_mask); // reset way to 0
	
	for(way = 0; way < CacheSet<typename CONFIG::DL1_CONFIG>::ASSOCIATIVITY; way++)
	{
		CacheLine<typename CONFIG::DL1_CONFIG>& l1_line = l1_set[way];
		uint32_t sector;

		for(sector = 0; sector < CacheLine<typename CONFIG::DL1_CONFIG>::BLOCKS_PER_LINE; sector++)
		{
			CacheBlock<typename CONFIG::DL1_CONFIG>& l1_block = l1_line[sector];

			l1_block.status.valid = false;
			l1_block.status.dirty = 0;
		}
		l1_line.status.valid = false;
		l1_line.status.way = way;
		l1_line.status.storage_attr = CONFIG::SA_DEFAULT;
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

	unsigned ctrl_reg_num = index / 4;
	unsigned ctrl_reg_field_num = index & 3;
	unsigned int ctrl_reg_field_shift = 8 * (3 - ctrl_reg_field_num);
	uint32_t ctrl_reg_field_mask = 0xffUL << ctrl_reg_field_shift;
	
	SetITV(ctrl_reg_num, GetITV(ctrl_reg_num) & ~ctrl_reg_field_mask); // reset way to 0
	SetINV(ctrl_reg_num, GetINV(ctrl_reg_num) & ~ctrl_reg_field_mask); // reset way to 0

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
		l1_line.status.way = way;
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
void CPU<CONFIG>::ClearAccessDL1(CacheAccess<typename CONFIG::DL1_CONFIG>& l1_access)
{
	l1_access.addr = 0;
	l1_access.line_base_addr = 0;
	l1_access.block_base_addr = 0;
	l1_access.index = 0;
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
	l1_access.index = 0;
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
template <bool DEBUG>
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
	
	if(!DEBUG) num_dl1_accesses++;
	// Decode the address
	dl1.DecodeAddress(addr, line_base_addr, block_base_addr, index, sector, offset, size_to_block_boundary);
	if(unlikely(IsVerboseDL1()))
	{
		logger << DebugInfo << (DEBUG ? "(DEBUG) " : "") << "DL1 Lookup at 0x";
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
					logger << DebugInfo << (DEBUG ? "(DEBUG) " : "") << "DL1 Line hit: index=" << l1_access.index << ", way=" << line->status.way << endl << EndDebugInfo;
				}
				// line hit: block may need a fill if not yet present in the line
				l1_access.line = line;
				l1_access.line_to_evict = 0;
				l1_access.way = line->status.way;
				block = &(*line)[sector];
				l1_access.block = block->status.valid ? block : 0;
				if(unlikely(IsVerboseDL1()))
				{
					logger << DebugInfo << (DEBUG ? "(DEBUG) " : "") << "DL1 block " << (block->status.valid ? "hit" : "miss") << endl << EndDebugInfo;
				}

				if(unlikely(!DEBUG && (l1_access.line->status.prev != 0)))
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
					logger << DebugInfo << (DEBUG ? "(DEBUG) " : "") << "DL1 Line hit: way=" << way << endl << EndDebugInfo;
				}
				// line hit: block may need a fill if not yet present in the line
				l1_access.line = line;
				l1_access.line_to_evict = 0;
				l1_access.way = way;
				block = &(*line)[sector];
				l1_access.block = block->status.valid ? block : 0;
				if(unlikely(IsVerboseDL1()))
				{
					logger << DebugInfo << (DEBUG ? "(DEBUG) " : "") << "DL1 block " << (block->status.valid ? "hit" : "miss") << endl << EndDebugInfo;
				}
				return;
			}
		}
	}
	// line miss
	l1_access.line = 0;
	l1_access.block = 0;
	if(!DEBUG) num_dl1_misses++;
}

template <class CONFIG>
template <bool DEBUG>
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
	
	if(!DEBUG) num_il1_accesses++;
	// Decode the address
	il1.DecodeAddress(addr, line_base_addr, block_base_addr, index, sector, offset, size_to_block_boundary);
	if(unlikely(IsVerboseIL1()))
	{
		logger << DebugInfo << (DEBUG ? "(DEBUG) " : "") << "IL1 Lookup at 0x";
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
					logger << DebugInfo << (DEBUG ? "(DEBUG) " : "") << "IL1 Line hit: " << l1_access.index << ", way=" << line->status.way << endl << EndDebugInfo;
				}
				// line hit: block may need a fill if not yet present in the line
				l1_access.line = line;
				l1_access.line_to_evict = 0;
				l1_access.way = line->status.way;
				block = &(*line)[sector];
				l1_access.block = block->status.valid ? block : 0;
				if(unlikely(IsVerboseIL1()))
				{
					logger << DebugInfo << (DEBUG ? "(DEBUG) " : "") << "IL1 block " << (block->status.valid ? "hit" : "miss") << endl << EndDebugInfo;
				}

				if(unlikely(!DEBUG && (l1_access.line->status.prev != 0)))
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
					logger << DebugInfo << (DEBUG ? "(DEBUG) " : "") << "IL1 Line hit: way=" << way << endl << EndDebugInfo;
				}
				// line hit: block may need a fill if not yet present in the line
				l1_access.line = line;
				l1_access.line_to_evict = 0;
				l1_access.way = way;
				block = &(*line)[sector];
				l1_access.block = block->status.valid ? block : 0;
				if(unlikely(IsVerboseIL1()))
				{
					logger << DebugInfo << (DEBUG ? "(DEBUG) " : "") << "IL1 block " << (block->status.valid ? "hit" : "miss") << endl << EndDebugInfo;
				}
				return;
			}
		}
	}

	// line miss
	l1_access.line = 0;
	l1_access.block = 0;
	if(!DEBUG) num_il1_misses++;
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
				unsigned int dirty_dword_num = 0; // dirty 64-bit word to evict
				if(GetCCR1_FFF() || // force full flush
				   (BitScanForward(dirty_dword_num, (uint32_t) l1_block_to_evict.status.dirty) && ((l1_block_to_evict.status.dirty >> dirty_dword_num) != 1))) // more than one 64-bit word is dirty
				{
					// dirty DL1 block eviction
					if(unlikely(IsVerboseDL1()))
					{
						logger << DebugInfo << "DL1: evicting dirty block at 0x" << std::hex << l1_block_to_evict.GetBaseAddr() << std::dec << " into memory" << endl << EndDebugInfo;
					}
					// dirty DL1 block eviction into memory
					if(unlikely(!PLBDataWrite(l1_block_to_evict.GetBaseAddr(), &l1_block_to_evict[0], CacheBlock<class CONFIG::DL1_CONFIG>::SIZE)))
					{
						throw DataAsynchronousMachineCheckException<CONFIG>();
					}
				}
				else
				{
					uint32_t dirty_dword_offset = dirty_dword_num * 8;
					// dirty 64-bit word eviction
					if(unlikely(IsVerboseDL1()))
					{
						logger << DebugInfo << "DL1: evicting dirty 64-bit word at 0x" << std::hex << (l1_block_to_evict.GetBaseAddr() + dirty_dword_offset) << std::dec << " into memory" << endl << EndDebugInfo;
					}
					// dirty DL1 64-word eviction into memory
					if(unlikely(!PLBDataWrite(l1_block_to_evict.GetBaseAddr() + dirty_dword_offset, &l1_block_to_evict[dirty_dword_offset], 8)))
					{
						throw DataAsynchronousMachineCheckException<CONFIG>();
					}
				}
			}
			l1_block_to_evict.status.valid = false;
			l1_block_to_evict.status.dirty = 0;
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
inline void CPU<CONFIG>::ChooseLineToEvictDL1(CacheAccess<typename CONFIG::DL1_CONFIG>& l1_access)
{
	unsigned ctrl_reg_num = l1_access.index / 4;
	unsigned ctrl_reg_field_num = l1_access.index & 3;
	unsigned int ctrl_reg_field_shift = 8 * (3 - ctrl_reg_field_num);

	// read the cache control register
	uint32_t ctrl_reg_value = (l1_access.storage_attr & CONFIG::SA_U1) ? GetDTV(ctrl_reg_num) : GetDNV(ctrl_reg_num);
	
	// extract way from the control register value
	uint32_t way = (ctrl_reg_value >> ctrl_reg_field_shift) & (CONFIG::DL1_CONFIG::CACHE_ASSOCIATIVITY - 1);
	
	l1_access.way = way;
	l1_access.line_to_evict = &(*l1_access.set)[way];
}

template <class CONFIG>
inline void CPU<CONFIG>::ChooseLineToEvictIL1(CacheAccess<typename CONFIG::IL1_CONFIG>& l1_access)
{
	unsigned ctrl_reg_num = l1_access.index / 4;
	unsigned ctrl_reg_field_num = l1_access.index & 3;
	unsigned int ctrl_reg_field_shift = 8 * (3 - ctrl_reg_field_num);

	// read the cache control register
	uint32_t ctrl_reg_value = (l1_access.storage_attr & CONFIG::SA_U1) ? GetITV(ctrl_reg_num) : GetINV(ctrl_reg_num);
	
	// extract way from the control register value
	uint32_t way = (ctrl_reg_value >> ctrl_reg_field_shift) & (CONFIG::IL1_CONFIG::CACHE_ASSOCIATIVITY - 1);
	
	l1_access.way = way;
	l1_access.line_to_evict = &(*l1_access.set)[way];
}

template <class CONFIG>
inline void CPU<CONFIG>::UpdateReplacementPolicyDL1(CacheAccess<typename CONFIG::DL1_CONFIG>& l1_access)
{
	unsigned ctrl_reg_num = l1_access.index / 4;
	unsigned ctrl_reg_field_num = l1_access.index & 3;
	unsigned int ctrl_reg_field_shift = 8 * (3 - ctrl_reg_field_num);
	uint32_t ctrl_reg_field_mask = 0xffUL << ctrl_reg_field_shift;
	
	// read the cache control register
	uint32_t ctrl_reg_value = (l1_access.storage_attr & CONFIG::SA_U1) ? GetDTV(ctrl_reg_num) : GetDNV(ctrl_reg_num);
	
	// extract way from the control register value
	uint32_t way = (ctrl_reg_value >> ctrl_reg_field_shift) & (CONFIG::DL1_CONFIG::CACHE_ASSOCIATIVITY - 1);
	
	// increment way of victim line
	way = (way + 1) & (CONFIG::DL1_CONFIG::CACHE_ASSOCIATIVITY - 1);
	
	// constraint way
	if(l1_access.storage_attr & CONFIG::SA_U1) // transient
	{
		uint32_t tfloor = GetDVLIM_TFLOOR();
		uint32_t tceiling = GetDVLIM_TCEILING();
		if(way < tfloor) way = tfloor;
		if(way > tceiling) way = tceiling;
	}
	else // normal
	{
		uint32_t nfloor = GetDVLIM_NFLOOR();
		if(way < nfloor) way = nfloor;
	}
	
	// pack way into control register value
	ctrl_reg_value = (ctrl_reg_value & ~ctrl_reg_field_mask) | (way << ctrl_reg_field_shift);
	
	// write back cache control register
	if(l1_access.storage_attr & CONFIG::SA_U1)
		SetDTV(ctrl_reg_num, ctrl_reg_value);
	else
		SetDNV(ctrl_reg_num, ctrl_reg_value);
}

template <class CONFIG>
inline void CPU<CONFIG>::UpdateReplacementPolicyIL1(CacheAccess<typename CONFIG::IL1_CONFIG>& l1_access)
{
	unsigned ctrl_reg_num = l1_access.index / 4;
	unsigned ctrl_reg_field_num = l1_access.index & 3;
	unsigned int ctrl_reg_field_shift = 8 * (3 - ctrl_reg_field_num);
	uint32_t ctrl_reg_field_mask = 0xffUL << ctrl_reg_field_shift;
	
	// read the cache control register
	uint32_t ctrl_reg_value = (l1_access.storage_attr & CONFIG::SA_U1) ? GetITV(ctrl_reg_num) : GetINV(ctrl_reg_num);
	
	// extract way from the control register value
	uint32_t way = (ctrl_reg_value >> ctrl_reg_field_shift) & (CONFIG::IL1_CONFIG::CACHE_ASSOCIATIVITY - 1);
	
	// increment way of victim line
	way = (way + 1) & (CONFIG::IL1_CONFIG::CACHE_ASSOCIATIVITY - 1);
	
	// constraint way
	if(l1_access.storage_attr & CONFIG::SA_U1) // transient
	{
		uint32_t tfloor = GetIVLIM_TFLOOR();
		uint32_t tceiling = GetIVLIM_TCEILING();
		if(way < tfloor) way = tfloor;
		if(way > tceiling) way = tceiling;
	}
	else // normal
	{
		uint32_t nfloor = GetIVLIM_NFLOOR();
		if(way < nfloor) way = nfloor;
	}

	// pack way into control register value
	ctrl_reg_value = (ctrl_reg_value & ~ctrl_reg_field_mask) | (way << ctrl_reg_field_shift);
	
	// write back cache control register
	if(l1_access.storage_attr & CONFIG::SA_U1)
		SetITV(ctrl_reg_num, ctrl_reg_value);
	else
		SetINV(ctrl_reg_num, ctrl_reg_value);
}

template <class CONFIG>
inline void CPU<CONFIG>::EmuFillDL1(CacheAccess<typename CONFIG::DL1_CONFIG>& l1_access)
{
	l1_access.block = &(*l1_access.line)[l1_access.sector];
	if(unlikely(IsVerboseDL1()))
	{
		logger << DebugInfo << "DL1: filling block at 0x" << std::hex << l1_access.block_base_addr << std::dec << endl << EndDebugInfo;
	}
	// DL1 block fill from memory
	if(unlikely(!PLBDataRead(l1_access.block_base_addr, &(*l1_access.block)[0], CacheBlock<class CONFIG::DL1_CONFIG>::SIZE, l1_access.storage_attr)))
	{
		throw DataAsynchronousMachineCheckException<CONFIG>();
	}

	l1_access.line->status.valid = true;
	l1_access.line->status.storage_attr = l1_access.storage_attr;
	l1_access.line->SetBaseAddr(l1_access.line_base_addr);
	l1_access.block->status.valid = true;
	l1_access.block->status.dirty = 0;
	UpdateReplacementPolicyDL1(l1_access);
}

template <class CONFIG>
inline void CPU<CONFIG>::EmuFillIL1(CacheAccess<typename CONFIG::IL1_CONFIG>& l1_access, MMUAccess<CONFIG>& mmu_access)
{
	l1_access.block = &(*l1_access.line)[l1_access.sector];
	if(unlikely(IsVerboseIL1()))
	{
		logger << DebugInfo << "IL1: filling block at 0x" << std::hex << l1_access.block_base_addr << std::dec << endl << EndDebugInfo;
	}

	// As instruction cache is virtually indexed and tagged, we compute block base physical address from MMU informations to access memory
	typename CONFIG::physical_address_t block_physical_base_addr = mmu_access.physical_addr & (~(CONFIG::IL1_CONFIG::CACHE_BLOCK_SIZE - 1));
	
	// DL1 block fill from memory
	if(unlikely(!PLBInsnRead(block_physical_base_addr, &(*l1_access.block)[0], CacheBlock<class CONFIG::DL1_CONFIG>::SIZE, l1_access.storage_attr)))
	{
		throw InstructionAsynchronousMachineCheckException<CONFIG>();
	}

	l1_access.line->status.valid = true;
	l1_access.line->SetBaseAddr(l1_access.line_base_addr);
	l1_access.block->status.valid = true;
	UpdateReplacementPolicyIL1(l1_access);
}

/* Data Cache management */
template <class CONFIG>
void CPU<CONFIG>::Dcba(typename CONFIG::address_t addr)
{
	// Dcba is treated as a no-op by PPC440x5
}

template <class CONFIG>
void CPU<CONFIG>::Dcbf(typename CONFIG::address_t addr)
{
	// Address translation
	MMUAccess<CONFIG> mmu_access;
	
	mmu_access.addr = addr;
	mmu_access.pid = GetProcessID();
	mmu_access.as = GetDataAddressSpace();
	mmu_access.privilege_level = GetPrivilegeLevel();
	mmu_access.memory_access_type = CONFIG::MAT_READ; // Dcbf is treated as a Load
	mmu_access.memory_type = CONFIG::MT_DATA;

	EmuTranslateAddress<false>(mmu_access);

	// DL1 Access
	if(!CONFIG::DL1_CONFIG::ENABLE) return;
	CacheAccess<class CONFIG::DL1_CONFIG> l1_access;
	l1_access.addr = mmu_access.physical_addr;
	l1_access.storage_attr = mmu_access.storage_attr;

	LookupDL1<false>(l1_access);

	if(l1_access.line)
	{
		uint32_t l1_sector;
	
		for(l1_sector = 0; l1_sector < CacheLine<class CONFIG::DL1_CONFIG>::BLOCKS_PER_LINE; l1_sector++)
		{
			CacheBlock<class CONFIG::DL1_CONFIG>& l1_block_to_flush = (*l1_access.line)[l1_sector];
	
			if(l1_block_to_flush.status.valid && l1_block_to_flush.status.dirty)
			{
				// DL1 block is dirty
				// Copy back to memory
				unsigned int dirty_dword_num = 0; // dirty 64-bit word to flush
				if(GetCCR1_FFF() || // force full flush
				   (BitScanForward(dirty_dword_num, (uint32_t) l1_block_to_flush.status.dirty) && ((l1_block_to_flush.status.dirty >> dirty_dword_num) != 1))) // more than one 64-bit word is dirty
				{
					// dirty DL1 block eviction
					if(unlikely(IsVerboseDL1()))
					{
						logger << DebugInfo << "DL1: flushing dirty block at 0x" << std::hex << l1_block_to_flush.GetBaseAddr() << std::dec << " into memory" << endl << EndDebugInfo;
					}
					// dirty DL1 block eviction into memory
					if(unlikely(!PLBDataWrite(l1_block_to_flush.GetBaseAddr(), &l1_block_to_flush[0], CacheBlock<class CONFIG::DL1_CONFIG>::SIZE)))
					{
						throw DataAsynchronousMachineCheckException<CONFIG>();
					}
				}
				else
				{
					uint32_t dirty_dword_offset = dirty_dword_num * 8;
					// dirty 64-bit word eviction
					if(unlikely(IsVerboseDL1()))
					{
						logger << DebugInfo << "DL1: flushing dirty 64-bit word at 0x" << std::hex << (l1_block_to_flush.GetBaseAddr() + dirty_dword_offset) << std::dec << " into memory" << endl << EndDebugInfo;
					}
					// dirty DL1 64-word eviction into memory
					if(unlikely(!PLBDataWrite(l1_block_to_flush.GetBaseAddr() + dirty_dword_offset, &l1_block_to_flush[dirty_dword_offset], 8)))
					{
						throw DataAsynchronousMachineCheckException<CONFIG>();
					}
				}
			}
			l1_block_to_flush.status.valid = false;
			l1_block_to_flush.status.dirty = 0;
		}
		l1_access.line->status.valid = false;
	}
}

template <class CONFIG>
void CPU<CONFIG>::Dcbi(typename CONFIG::address_t addr)
{
	if(GetMSR_PR()) throw PrivilegeViolationException<CONFIG>();

	// Address translation
	MMUAccess<CONFIG> mmu_access;

	mmu_access.addr = addr;
	mmu_access.pid = GetProcessID();
	mmu_access.as = GetDataAddressSpace();
	mmu_access.privilege_level = GetPrivilegeLevel();
	mmu_access.memory_access_type = CONFIG::MAT_WRITE;
	mmu_access.memory_type = CONFIG::MT_DATA;

	EmuTranslateAddress<false>(mmu_access);

	// DL1 Access
	if(!CONFIG::DL1_CONFIG::ENABLE) return;
	CacheAccess<class CONFIG::DL1_CONFIG> l1_access;
	l1_access.addr = mmu_access.physical_addr;

	LookupDL1<false>(l1_access);

	if(l1_access.line)
	{
		uint32_t l1_sector;
	
		for(l1_sector = 0; l1_sector < CacheLine<class CONFIG::DL1_CONFIG>::BLOCKS_PER_LINE; l1_sector++)
		{
			CacheBlock<class CONFIG::DL1_CONFIG>& l1_block_to_invalidate = (*l1_access.line)[l1_sector];
	
			l1_block_to_invalidate.status.valid = false;
			l1_block_to_invalidate.status.dirty = 0;
		}
		l1_access.line->status.valid = false;
	}
}

template <class CONFIG>
void CPU<CONFIG>::Dcbst(typename CONFIG::address_t addr)
{
	// Address translation
	MMUAccess<CONFIG> mmu_access;

	mmu_access.addr = addr;
	mmu_access.pid = GetProcessID();
	mmu_access.as = GetDataAddressSpace();
	mmu_access.privilege_level = GetPrivilegeLevel();
	mmu_access.memory_access_type = CONFIG::MAT_READ; // 3.4.4.4 Data Cache Block Store (dcbst): This instruction is treated as a load with respect to address translation and memory protection
	mmu_access.memory_type = CONFIG::MT_DATA;

	EmuTranslateAddress<false>(mmu_access);

	// DL1 Access
	if(!CONFIG::DL1_CONFIG::ENABLE) return;
	CacheAccess<class CONFIG::DL1_CONFIG> l1_access;
	l1_access.addr = mmu_access.physical_addr;

	LookupDL1<false>(l1_access);

	if(l1_access.line)
	{
		uint32_t l1_sector;
	
		for(l1_sector = 0; l1_sector < CacheLine<class CONFIG::DL1_CONFIG>::BLOCKS_PER_LINE; l1_sector++)
		{
			CacheBlock<class CONFIG::DL1_CONFIG>& l1_block_to_copy_back = (*l1_access.line)[l1_sector];
	
			if(l1_block_to_copy_back.status.valid && l1_block_to_copy_back.status.dirty)
			{
				// DL1 block is dirty
				// Copy back to memory
				unsigned int dirty_dword_num = 0; // dirty 64-bit word to copy back
				if(GetCCR1_FFF() || // force full flush
				   (BitScanForward(dirty_dword_num, (uint32_t) l1_block_to_copy_back.status.dirty) && ((l1_block_to_copy_back.status.dirty >> dirty_dword_num) != 1))) // more than one 64-bit word is dirty
				{
					// dirty DL1 block eviction
					if(unlikely(IsVerboseDL1()))
					{
						logger << DebugInfo << "DL1: copy back dirty block at 0x" << std::hex << l1_block_to_copy_back.GetBaseAddr() << std::dec << " into memory" << endl << EndDebugInfo;
					}
					// dirty DL1 block eviction into memory
					if(unlikely(!PLBDataWrite(l1_block_to_copy_back.GetBaseAddr(), &l1_block_to_copy_back[0], CacheBlock<class CONFIG::DL1_CONFIG>::SIZE)))
					{
						throw DataAsynchronousMachineCheckException<CONFIG>();
					}
				}
				else
				{
					uint32_t dirty_dword_offset = dirty_dword_num * 8;
					// dirty 64-bit word eviction
					if(unlikely(IsVerboseDL1()))
					{
						logger << DebugInfo << "DL1: copy back dirty 64-bit word at 0x" << std::hex << (l1_block_to_copy_back.GetBaseAddr() + dirty_dword_offset) << std::dec << " into memory" << endl << EndDebugInfo;
					}
					// dirty DL1 64-word eviction into memory
					if(unlikely(!PLBDataWrite(l1_block_to_copy_back.GetBaseAddr() + dirty_dword_offset, &l1_block_to_copy_back[dirty_dword_offset], 8)))
					{
						throw DataAsynchronousMachineCheckException<CONFIG>();
					}
				}
			}
			l1_block_to_copy_back.status.dirty = 0;
		}
	}
}

template <class CONFIG>
void CPU<CONFIG>::Dcbz(typename CONFIG::address_t addr)
{
	// Address translation
	MMUAccess<CONFIG> mmu_access;

	mmu_access.addr = addr;
	mmu_access.pid = GetProcessID();
	mmu_access.as = GetDataAddressSpace();
	mmu_access.privilege_level = GetPrivilegeLevel();
	mmu_access.memory_access_type = CONFIG::MAT_WRITE; // 3.4.4.3 Data Cache Block Zero (dcbz): The dcbz instruction is treated as a store to the addressed byte with respect to address translation, protection, and pipelining.
	mmu_access.memory_type = CONFIG::MT_DATA;

	EmuTranslateAddress<false>(mmu_access);

	if(CONFIG::DL1_CONFIG::ENABLE)
	{
		// DL1 Access
		CacheAccess<class CONFIG::DL1_CONFIG> l1_access;
	
		l1_access.addr = mmu_access.physical_addr;
		l1_access.storage_attr = mmu_access.storage_attr;
		LookupDL1<false>(l1_access);
	
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
		l1_access.block->status.dirty = (1 << (CONFIG::DL1_CONFIG::CACHE_BLOCK_SIZE / 8)) - 1; // all 64-bit words are dirty
		UpdateReplacementPolicyDL1(l1_access);
		if(unlikely(dl1_power_estimator_import != 0)) dl1_power_estimator_import->ReportWriteAccess();
	}
	else
	{
		// just in case we don't have a data cache
		uint8_t zero[32];
		memset(zero, 0, sizeof(zero));
		if(unlikely(!PLBDataWrite(mmu_access.physical_addr & (~31), zero, sizeof(zero))))
		{
			throw DataAsynchronousMachineCheckException<CONFIG>();
		}
	}
}

template <class CONFIG>
void CPU<CONFIG>::Dccci(typename CONFIG::address_t addr)
{
	// Note: it's normal to ignore 'addr' as it's unused on PPC440
	if(GetMSR_PR())
	{
		throw PrivilegeViolationException<CONFIG>();
	}

	if(!CONFIG::DL1_CONFIG::ENABLE) return;
	
	InvalidateDL1();
}

template <class CONFIG>
void CPU<CONFIG>::Dcread(typename CONFIG::address_t addr, unsigned int rd)
{
	if(CONFIG::DL1_CONFIG::ENABLE)
	{
		uint32_t way = addr / CONFIG::DL1_CONFIG::CACHE_BLOCK_SIZE;
		uint32_t offset = (addr % CONFIG::DL1_CONFIG::CACHE_BLOCK_SIZE) & 0xfffffffcUL;
		uint32_t index = 0;
		uint32_t sector = 0;
		
		CacheSet<typename CONFIG::DL1_CONFIG>& l1_set = dl1[index];
		CacheLine<typename CONFIG::DL1_CONFIG>& l1_line = l1_set[way];
		CacheBlock<typename CONFIG::DL1_CONFIG>& l1_block = l1_line[sector];
		
		uint32_t data;
		memcpy(&data, &l1_block[offset], sizeof(data));
		
		SetGPR(rd, data);
		
		typename CONFIG::physical_address_t base_physical_addr = l1_block.GetBaseAddr();
		
		uint32_t tagh = (((base_physical_addr >> 8) << CONFIG::DCDBTRH_TRA_OFFSET) & CONFIG::DCDBTRH_TRA_MASK)
					| ((((sizeof(typename CONFIG::physical_address_t) > 32) ? base_physical_addr >> 32 : 0) << CONFIG::DCDBTRH_TERA_OFFSET) & CONFIG::DCDBTRH_TERA_MASK)
					| (l1_block.status.valid ? CONFIG::DCDBTRH_V_MASK : 0);
		
		uint32_t tagl = (((uint32_t) l1_block.status.dirty << CONFIG::DCDBTRL_D_OFFSET) & CONFIG::DCDBTRL_D_MASK)
					| ((l1_line.status.storage_attr & CONFIG::SA_U0) ? CONFIG::DCDBTRL_U0_MASK : 0)
					| ((l1_line.status.storage_attr & CONFIG::SA_U1) ? CONFIG::DCDBTRL_U1_MASK : 0)
					| ((l1_line.status.storage_attr & CONFIG::SA_U2) ? CONFIG::DCDBTRL_U2_MASK : 0)
					| ((l1_line.status.storage_attr & CONFIG::SA_U3) ? CONFIG::DCDBTRL_U3_MASK : 0);
		
		SetDCDBTRH(tagh);
		SetDCDBTRL(tagl);
	}
	else
	{
		SetGPR(rd, 0);
		SetDCDBTRH(0);
		SetDCDBTRL(0);
	}
}

/* Instruction Cache Management */
template <class CONFIG>
void CPU<CONFIG>::Icbi(typename CONFIG::address_t addr)
{
	// Address translation
	MMUAccess<CONFIG> mmu_access;

	mmu_access.addr = addr;
	mmu_access.pid = GetProcessID();
	mmu_access.as = GetDataAddressSpace(); // that's normal that MSR[DS] is used instead of MSR[IS]
	mmu_access.privilege_level = GetPrivilegeLevel();
	mmu_access.memory_access_type = CONFIG::MAT_READ;
	mmu_access.memory_type = CONFIG::MT_DATA; // instruction is considered as a "load" with respect to data storage exceptions

	EmuTranslateAddress<false>(mmu_access);

	if(CONFIG::IL1_CONFIG::ENABLE)
	{
		// invalidation operation is performed whether or not the access is cacheable
		CacheAccess<class CONFIG::IL1_CONFIG> l1_access;

		l1_access.addr = mmu_access.virtual_addr;
		l1_access.storage_attr = mmu_access.storage_attr;
		LookupIL1<false>(l1_access);
		
		InvalidateIL1Set(l1_access.index);
	}

	unisim::component::cxx::processor::powerpc::ppc440::Decoder<CONFIG>::InvalidateDecodingCacheEntry(addr);
}

template <class CONFIG>
void CPU<CONFIG>::Icbt(typename CONFIG::address_t addr)
{
	// Nothing to do: no architecturally visible changed 
}

template <class CONFIG>
void CPU<CONFIG>::Iccci(typename CONFIG::address_t addr)
{
	// Note: it's normal to ignore 'addr' as it's unused on PPC440
	if(GetMSR_PR())
	{
		throw PrivilegeViolationException<CONFIG>();
	}

	if(!CONFIG::IL1_CONFIG::ENABLE) return;

	InvalidateIL1();
}

template <class CONFIG>
void CPU<CONFIG>::Icread(typename CONFIG::address_t addr)
{
	if(CONFIG::IL1_CONFIG::ENABLE)
	{
		uint32_t way = addr / CONFIG::IL1_CONFIG::CACHE_BLOCK_SIZE;
		uint32_t offset = (addr % CONFIG::IL1_CONFIG::CACHE_BLOCK_SIZE) & 0xfffffffcUL;
		uint32_t index = 0;
		uint32_t sector = 0;
		
		CacheSet<typename CONFIG::IL1_CONFIG>& l1_set = il1[index];
		CacheLine<typename CONFIG::IL1_CONFIG>& l1_line = l1_set[way];
		CacheBlock<typename CONFIG::IL1_CONFIG>& l1_block = l1_line[sector];
		
		uint32_t data;
		memcpy(&data, &l1_block[offset], sizeof(data));
		
		SetICDBDR(data);
		
		typename CONFIG::virtual_address_t base_virtual_addr = l1_block.GetBaseAddr();
		typename CONFIG::address_t ea = (base_virtual_addr & CONFIG::VADDR_EA_MASK) >> CONFIG::VADDR_EA_OFFSET;
		typename CONFIG::process_id_t pid = (base_virtual_addr & CONFIG::VADDR_PID_MASK) >> CONFIG::VADDR_PID_OFFSET;
		typename CONFIG::address_space_t as = (typename CONFIG::address_space_t)((base_virtual_addr & CONFIG::VADDR_AS_MASK) >> CONFIG::VADDR_AS_OFFSET);
		
		uint32_t tagh = (((ea >> 8) << CONFIG::ICDBTRH_TEA_OFFSET) & CONFIG::ICDBTRH_TEA_MASK)
					| (l1_block.status.valid ? CONFIG::ICDBTRH_V_MASK : 0);
		
		uint32_t tagl = (((uint32_t) as << CONFIG::ICDBTRL_TS_OFFSET) & CONFIG::ICDBTRL_TS_MASK)
					| ((pid << CONFIG::ICDBTRL_TID_OFFSET) & CONFIG::ICDBTRL_TID_MASK)
					| (pid ? 0 : CONFIG::ICDBTRL_TD_MASK);
		
		SetICDBTRH(tagh);
		SetICDBTRL(tagl);
	}
	else
	{
		SetICDBDR(0);
		SetICDBTRH(0);
		SetICDBTRL(0);
	}
}

} // end of namespace ppc440
} // end of namespace powerpc
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
