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
 
#include "unisim/util/garbage_collector/garbage_collector.hh"
#include <stdlib.h>

using std::cerr;
using std::endl;

void *operator new(std::size_t size, unisim::util::garbage_collector::PointerBase)
{
//	cerr << "GarbageCollector::operator new " << size << endl;
	return unisim::util::garbage_collector::GarbageCollector::Allocate(size);
}

void *operator new[](std::size_t size, unisim::util::garbage_collector::PointerBase)
{
//	cerr << "GarbageCollector::operator new[] " << size << endl;
	return unisim::util::garbage_collector::GarbageCollector::Allocate(size);
}

namespace unisim {
namespace util {
namespace garbage_collector {

MemoryBlock *GarbageCollector::free_blocks[GarbageCollector::MAX_MEMORY_BLOCK_SIZE / GarbageCollector::MIN_MEMORY_BLOCK_SIZE];

void GarbageCollector::AtExit() {
	Reset();
}

void GarbageCollector::Setup()
{
	atexit(AtExit);
	memset(free_blocks, 0, sizeof(free_blocks));
}

void GarbageCollector::Reset()
{
	unsigned int i;
//	cerr << "GarbageCollector::Reset()" << endl;
	for(i = 0; i < MAX_MEMORY_BLOCK_SIZE / MIN_MEMORY_BLOCK_SIZE; i++)
	{
		MemoryBlock * block = free_blocks[i];
		if(block)
		{
			MemoryBlock *next_block;
			do
			{
				next_block = block->next;
				//cerr << "GarbageCollector::Reset() free" << endl;
				free(block);
			} while((block = next_block) != 0);
		}
		free_blocks[i] = 0;
	}
}

void *GarbageCollector::Allocate(unsigned int size)
{
	MemoryBlock *block;
	unsigned int index = size/ MIN_MEMORY_BLOCK_SIZE;
	if(index >= MAX_MEMORY_BLOCK_SIZE / MIN_MEMORY_BLOCK_SIZE)
	{
		block = (MemoryBlock *) malloc(sizeof(MemoryBlock) + size);
		block->magic = MAGIC;
		block->refcount = 0;
		block->size = size;
		
//		cerr << "GarbageCollector::Allocate(" << size << ") on big blocks: " << block << endl;

 		return block + 1;
	}
		
	if(free_blocks[index])
	{
		block = free_blocks[index];
		free_blocks[index] = block->next;
		
//		cerr << "GarbageCollector::Allocate(" << size << ") on free blocks: " << block << endl;		
 		return block + 1;
	}
//		(index + 1) * MIN_MEMORY_BLOCK_SIZE
	block = (MemoryBlock *) malloc(sizeof(MemoryBlock) + ((index + 1) * MIN_MEMORY_BLOCK_SIZE));
//	block = (MemoryBlock *) malloc(sizeof(MemoryBlock) + ((size + MIN_MEMORY_BLOCK_SIZE - 1) & ~(MIN_MEMORY_BLOCK_SIZE - 1)));
	block->magic = MAGIC;
	block->refcount = 0;
	block->size = size;
//	cerr << "GarbageCollector::Allocate(" << size << ") on a new block: " << block << endl;
	
	return block + 1;
}

bool GarbageCollector::Collect(void *p)
{
	MemoryBlock *block = (MemoryBlock *) p - 1;
	if(block->magic != MAGIC)
	{
		cerr << "WARNING! Doing collect on unmanaged block" << endl;
		abort();
	}
	//cerr << "GarbageCollector::Collect(0x" << p << "): refcount=" << block->refcount << endl;
	if(--block->refcount != 0) return false;
	unsigned int index = block->size/ MIN_MEMORY_BLOCK_SIZE;
	if(index >= MAX_MEMORY_BLOCK_SIZE / MIN_MEMORY_BLOCK_SIZE)
	{
		free(block);
	}
	else
	{
		block->next = free_blocks[index];
		free_blocks[index] = block;
	}
	return true;
}

void GarbageCollector::Catch(void *p)
{
	MemoryBlock *block = (MemoryBlock *) p - 1;
	if(block->magic != MAGIC)
	{
		cerr << "WARNING! Doing catch on unmanaged block" << endl;
		abort();
	}
	//cerr << "GarbageCollector::Catch(0x" << p << "): refcount=" << block->refcount << endl;
	block->refcount++;
}

} // end of namespace garbage_collector
} // end of namespace util
} // end of namespace unisim
