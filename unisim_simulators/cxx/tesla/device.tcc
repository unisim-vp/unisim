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
#include "kernel.hh"

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

bool ParseBool(char const * str)
{
	if(str == 0 || strcmp(str, "0") == 0
		|| strcmp(str, "false") == 0) {
		return false;
	}
	return true;
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

	// Parse environment variables and set GPU parameters accordingly
	char const * env;
	env = getenv("TRACE_INSN");
	if(env != 0)
		cpu["trace-insn"] = ParseBool(env);

	env = getenv("TRACE_MASK");
	if(env != 0)
		cpu["trace-mask"] = ParseBool(env);

	env = getenv("TRACE_REG");
	if(env != 0)
		cpu["trace-reg"] = ParseBool(env);

	env = getenv("TRACE_REG_FLOAT");
	if(env != 0)
		cpu["trace-reg-float"] = ParseBool(env);

	env = getenv("TRACE_LOADSTORE");
	if(env != 0)
		cpu["trace-loadstore"] = ParseBool(env);

	env = getenv("TRACE_BRANCH");
	if(env != 0)
		cpu["trace-branch"] = ParseBool(env);

	env = getenv("TRACE_SYNC");
	if(env != 0)
		cpu["trace-sync"] = ParseBool(env);

	env = getenv("TRACE_RESET");
	if(env != 0)
		cpu["trace-reset"] = ParseBool(env);

	env = getenv("TRACE_KERNEL_PARSING");
	if(env != 0)
		Kernel<CONFIG>::trace_parsing = ParseBool(env);

	env = getenv("TRACE_KERNEL_LOADING");
	if(env != 0)
		Kernel<CONFIG>::trace_loading = ParseBool(env);

}

template<class CONFIG>
void Device<CONFIG>::Load(Kernel<CONFIG> & kernel)
{
	kernel.Load(memory, global_allocator);
}

template<class CONFIG>
void Device<CONFIG>::DumpCode(Kernel<CONFIG> & kernel, std::ostream & os)
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
void Device<CONFIG>::Run(Kernel<CONFIG> & kernel, int width, int height)
{
	DumpCode(kernel, cerr);

	Load(kernel);
	kernel.SetGridShape(width, height);
	
	int blockspercore = kernel.BlocksPerCore();
	
	// TODO: multiple cores
	// Blocks are scheduled sequentially on available resources
	int bidy = 0;
	int bidx = 0;
	for(int i = 0; i < height * width; i += blockspercore)
	{
		int blocks = std::min(blockspercore, height * width - i);
		cpu.Reset(kernel.ThreadsPerBlock(), blocks, kernel.GPRs(), kernel.SharedTotal());

		for(int j = 0; j < blocks; ++j)
		{
			kernel.InitShared(memory, j, bidx, bidy);
			SetThreadIDs(kernel, j);

			++bidx;
			if(bidx == width) {
				bidx = 0;
				++bidy;
			}
		}
		cpu.Run();
	}
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
	for(unsigned int i = 0; i != n; ++i)
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
	for(unsigned int i = 0; i != n; ++i)
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
	for(unsigned int i = 0; i != n; ++i)
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
void Device<CONFIG>::SetThreadIDs(Kernel<CONFIG> const & kernel, int bnum)
{
	// Set register 0 of each thread
	
	// TODO: multiple cores
	int warpsperblock = kernel.WarpsPerBlock();
	int blockstart = bnum * warpsperblock;
	//cerr << "Warpsperblocks = " << warpsperblock << ", blockstart = " << blockstart << endl;
	for(int z = 0; z != kernel.BlockZ(); ++z)
	{
		for(int y = 0; y != kernel.BlockY(); ++y)
		{
			for(int x = 0; x != kernel.BlockX(); ++x)
			{
				int tid = ((z * kernel.BlockY()) + y) * kernel.BlockX() + x;
				int warpid = blockstart + tid / CONFIG::WARP_SIZE;
				int lane = tid % CONFIG::WARP_SIZE;
				
				//cerr << "Setting r0 of warp " << warpid << endl;
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

template<class CONFIG>
std::string Device<CONFIG>::Name()
{
	return "Barra Simulator";
}

template<class CONFIG>
unsigned int Device<CONFIG>::TotalMem()
{
	cerr << "TotalMem: " << CONFIG::GLOBAL_SIZE << endl;
	return CONFIG::GLOBAL_SIZE;
}

template<class CONFIG>
int Device<CONFIG>::ComputeCapabilityMajor()
{
	return CONFIG::COMPUTE_CAP_MAJOR;
}

template<class CONFIG>
int Device<CONFIG>::ComputeCapabilityMinor()
{
	return CONFIG::COMPUTE_CAP_MINOR;
}

template<class CONFIG>
CUdevprop Device<CONFIG>::Properties()
{
	CUdevprop prop = {
		CONFIG::MAX_THREADS,	// maxThreadsPerBlock
		{ 512, 512, 64 },		// maxThreadsDim[3]
		{ 65535, 65535, 1 },	// maxGridSize[3]
		CONFIG::SHARED_SIZE,	// sharedMemPerBlock
		CONFIG::CONST_SEG_SIZE,	// totalConstantMemory
		CONFIG::WARP_SIZE,		// SIMDWidth
		262144,					// memPitch
		CONFIG::MAX_VGPR * CONFIG::WARP_SIZE,	// regsPerBlock
		CONFIG::SHADER_CLOCK_KHZ,	// clockRate
		256	// textureAlign
	};
	return prop;
}

template<class CONFIG>
int Device<CONFIG>::Attribute(int attrib)
{
	switch(attrib)
	{
	case CU_DEVICE_ATTRIBUTE_MAX_THREADS_PER_BLOCK:
		return CONFIG::MAX_THREADS;
	case CU_DEVICE_ATTRIBUTE_MAX_BLOCK_DIM_X:
		return 512;
    case CU_DEVICE_ATTRIBUTE_MAX_BLOCK_DIM_Y:
    	return 512;
    case CU_DEVICE_ATTRIBUTE_MAX_BLOCK_DIM_Z:
    	return 64;
    case CU_DEVICE_ATTRIBUTE_MAX_GRID_DIM_X:
    	return 65535;
    case CU_DEVICE_ATTRIBUTE_MAX_GRID_DIM_Y:
    	return 65535;
    case CU_DEVICE_ATTRIBUTE_MAX_GRID_DIM_Z:
    	return 1;
    case CU_DEVICE_ATTRIBUTE_MAX_SHARED_MEMORY_PER_BLOCK:
    //case CU_DEVICE_ATTRIBUTE_SHARED_MEMORY_PER_BLOCK:
   		return CONFIG::SHARED_SIZE;
    case CU_DEVICE_ATTRIBUTE_TOTAL_CONSTANT_MEMORY:
    	return CONFIG::CONST_SEG_SIZE;
    case CU_DEVICE_ATTRIBUTE_WARP_SIZE:
    	return CONFIG::WARP_SIZE;
    case CU_DEVICE_ATTRIBUTE_MAX_PITCH:
    	return 262144;
    case CU_DEVICE_ATTRIBUTE_MAX_REGISTERS_PER_BLOCK:
    //case CU_DEVICE_ATTRIBUTE_REGISTERS_PER_BLOCK:
   		return CONFIG::MAX_VGPR * CONFIG::WARP_SIZE;
    case CU_DEVICE_ATTRIBUTE_CLOCK_RATE:
    	return CONFIG::SHADER_CLOCK_KHZ;
    case CU_DEVICE_ATTRIBUTE_TEXTURE_ALIGNMENT:
    	return 256;
	case CU_DEVICE_ATTRIBUTE_GPU_OVERLAP:
		return 0;
    case CU_DEVICE_ATTRIBUTE_MULTIPROCESSOR_COUNT:
    	return CONFIG::CORE_COUNT;
	case CU_DEVICE_ATTRIBUTE_KERNEL_EXEC_TIMEOUT:
		return 0;  // No timeout
	case 0x20080403:
		//return 1;    // Happy birthday!
	default:
    	throw CudaException(CUDA_ERROR_INVALID_VALUE);
	}
}

template<class CONFIG>
Allocator<CONFIG> & Device<CONFIG>::GlobalAllocator()
{
	return global_allocator;
}

template<class CONFIG>
void Device<CONFIG>::LoadSegment(MemSegment<CONFIG> & seg)
{
	seg.Load(memory, global_allocator);
}

#endif
