/*
 *  Copyright (c) 2009,
 *  University of Perpignan (UPVD),
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
 *   - Neither the name of UPVD nor the names of its contributors may be used to
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
 * Authors: 
 *     Sylvain Collange (sylvain.collange@univ-perp.fr)
 *
 */
 
#ifndef SIMULATOR_CXX_TESLA_DRIVER_DEVICE_TCC
#define SIMULATOR_CXX_TESLA_DRIVER_DEVICE_TCC

#include "device.hh"
#include <string>
#include <ostream>

using std::string;
using std::endl;
using std::hex;
using std::dec;

template<class CONFIG>
Allocator<CONFIG>::Allocator(typename CONFIG::address_t base, size_t size) :
	base(base), limit(base), max_size(size)
{
	limit = base;
}


template<class CONFIG>
typename CONFIG::address_t Allocator<CONFIG>::Alloc(size_t size)
{
	if(limit + size > base + max_size)
	{
		throw CudaException(CUDA_ERROR_OUT_OF_MEMORY);
	}
	typename CONFIG::address_t addr = limit;
	limit += size;
	return addr;
}

template<class CONFIG>
void Allocator<CONFIG>::Free(typename CONFIG::address_t addr)
{
}

template<class CONFIG>
Device<CONFIG>::Device() :
	Object("device_0"),
	cpu("gpu_core_0", this),
	memory("memory", this),
	global_allocator(CONFIG::GLOBAL_START, CONFIG::GLOBAL_SIZE)
{
	cpu.memory_import >> memory.memory_export;
	memory.Setup();
	cpu.Setup();

}

template<class CONFIG>
void Device<CONFIG>::Load(Kernel<CONFIG> const & kernel)
{
	kernel.Load(memory);
}

template<class CONFIG>
void Device<CONFIG>::DumpCode(Kernel<CONFIG> const & kernel, std::ostream & os)
{
	Load(kernel);
	typename CONFIG::address_t pc = CONFIG::CODE_START;
	os << "Dumping code @" << std::hex << pc << endl;
	while(pc < CONFIG::CODE_START + kernel.CodeSize())
	{
		string s = cpu.Disasm(pc, pc);
		os << s << endl;
	}
}

template<class CONFIG>
void Device<CONFIG>::Run(Kernel<CONFIG> const & kernel)
{
	Load(kernel);
	cpu.Reset(kernel.ThreadsPerBlock(), 1);
	SetThreadIDs(kernel);
	cpu.Run();	// Multiple cores??
}

template<class CONFIG>
typename CONFIG::address_t Device<CONFIG>::MAlloc(size_t size)
{
	typename CONFIG::address_t addr = global_allocator.Alloc(size);
	cerr << "Device malloc: " << size << "B @" << hex << addr << dec << endl;
	return addr;
}

template<class CONFIG>
void Device<CONFIG>::Free(typename CONFIG::address_t addr)
{
	cerr << "Device free: @" << hex << addr << dec << endl;
	global_allocator.Free(addr);
}

template<class CONFIG>
void Device<CONFIG>::CopyHtoD(typename CONFIG::address_t dest, void const * src, size_t size)
{
	cerr << "Device copy " << size << "B to @" << hex << dest << dec << endl;
	if(!memory.WriteMemory(dest, src, size)) {
		throw CudaException(CUDA_ERROR_INVALID_VALUE);
	}
}

template<class CONFIG>
void Device<CONFIG>::CopyDtoH(void * dest, typename CONFIG::address_t src, size_t size)
{
	cerr << "Device copy " << size << "B from @" << hex << src << dec << endl;
	if(!memory.ReadMemory(src, dest, size)) {
		throw CudaException(CUDA_ERROR_INVALID_VALUE);
	}
}

template<class CONFIG>
void Device<CONFIG>::CopyDtoD(void * dest, typename CONFIG::address_t src, size_t size)
{
	// TODO
}

template<class CONFIG>
void Device<CONFIG>::Memset(typename CONFIG::address_t dest, uint32_t val, size_t n)
{
	cerr << "Device memset: " << n * sizeof(val) << "B @" << hex << dest << dec << endl;
	for(int i = 0; i != n; ++i)
	{
		if(!memory.WriteMemory(dest + i * sizeof(val), &val, sizeof(val))) {
			throw CudaException(CUDA_ERROR_INVALID_VALUE);
		}
	}
}

template<class CONFIG>
void Device<CONFIG>::Memset(typename CONFIG::address_t dest, uint16_t val, size_t n)
{
	cerr << "Device memset: " << n * sizeof(val) << "B @" << hex << dest << dec << endl;
	for(int i = 0; i != n; ++i)
	{
		if(!memory.WriteMemory(dest + i * sizeof(val), &val, sizeof(val))) {
			throw CudaException(CUDA_ERROR_INVALID_VALUE);
		}
	}
}

template<class CONFIG>
void Device<CONFIG>::Memset(typename CONFIG::address_t dest, uint8_t val, size_t n)
{
	cerr << "Device memset: " << n * sizeof(val) << "B @" << hex << dest << dec << endl;
	for(int i = 0; i != n; ++i)
	{
		if(!memory.WriteMemory(dest + i * sizeof(val), &val, sizeof(val))) {
			throw CudaException(CUDA_ERROR_INVALID_VALUE);
		}
	}
}


template<class CONFIG>
void Device<CONFIG>::Reset()
{
}

template<class CONFIG>
void Device<CONFIG>::SetThreadIDs(Kernel<CONFIG> const & kernel)
{
	// Set register 0 of each thread
	
	// TODO: multiple blocks
	// TODO: multiple cores
	for(int z = 0; z != kernel.BlockZ(); ++z)
	{
		for(int y = 0; y != kernel.BlockY(); ++y)
		{
			for(int x = 0; x != kernel.BlockX(); ++x)
			{
				int tid = ((z * kernel.BlockY()) + y) * kernel.BlockX() + x;
				int warpid = tid / CONFIG::WARP_SIZE;
				int lane = tid % CONFIG::WARP_SIZE;
				
				uint32_t reg = BuildTID(x, y, z);
				cpu.GetGPR(warpid, 0).WriteLane(reg, lane);	// TID on r0
			}
		}
	}
}

template<class CONFIG>
uint32_t Device<CONFIG>::BuildTID(int x, int y, int z)
{
	// TODO: check it matches with actual hardware
	// TODO: and use config
	return (z << 26) | (y << 16) | x;
}


#endif
