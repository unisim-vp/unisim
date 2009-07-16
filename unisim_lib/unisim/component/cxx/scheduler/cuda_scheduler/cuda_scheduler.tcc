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
 * Authors: Sylvain Collange (sylvain.collange@univ-perp.fr)
 */
 
#ifndef UNISIM_COMPONENT_CXX_SCHEDULER_CUDA_SCHEDULER_TCC
#define UNISIM_COMPONENT_CXX_SCHEDULER_CUDA_SCHEDULER_TCC

#include <unisim/component/cxx/scheduler/cuda_scheduler/cuda_scheduler.hh>
#include <boost/thread.hpp>

namespace unisim {
namespace component {
namespace cxx {
namespace scheduler {
namespace cuda_scheduler {

using boost::thread;
using boost::thread_group;


template<class CPUCONFIG>
CoreSocket<CPUCONFIG>::CoreSocket(const char *name, Object *parent) :
	Object(name, parent),
	Client<TypedRegisters<uint32_t, GPRID> >(name, parent),
	Client<TypedRegisters<uint32_t, ConfigurationRegisterID> >(name, parent),
	Client<Memory<SMAddress> >(name, parent),
	Client<Resetable>(name, parent),
	Client<Runnable>(name, parent),
	registers_import("registers-import", this),
	configuration_import("configuration-import", this),
	shared_memory_import("shared-memory-import", this),
	reset_import("reset-import", this),
	run_import("run-import", this)
{
}

template<class CPUCONFIG>
bool CoreSocket<CPUCONFIG>::Setup()
{
	return true;
}

inline
int ThreadsPerBlock(Schedulable const & kernel)
{
	return kernel.BlockZ() * kernel.BlockY() * kernel.BlockX();
}

template<class CONFIG>
int WarpsPerBlock(Schedulable const & kernel)
{
	return (ThreadsPerBlock(kernel) + CONFIG::WARP_SIZE - 1) / CONFIG::WARP_SIZE;
}


template<class CONFIG>
int BlocksPerCore(Schedulable const & kernel)
{
	int blocksreg = CONFIG::MAX_VGPR / (kernel.GPRs() * WarpsPerBlock<CONFIG>(kernel));
	int blockssm = CONFIG::SHARED_SIZE / kernel.SharedTotal(); // Already aligned
	int blockswarps = CONFIG::MAX_WARPS / WarpsPerBlock<CONFIG>(kernel);
	
	return std::min(std::min(blocksreg, blockssm), std::min(int(CONFIG::MAX_CTAS), blockswarps));
}

inline uint32_t BuildTID(int x, int y, int z)
{
	return (z << 26) | (y << 16) | x;
}

template<class CONFIG>
void SetThreadIDs(Schedulable const & kernel, int bnum, CoreSocket<CONFIG> & cpu)
{
	// Set register 0 of each thread
	int warpsperblock = WarpsPerBlock<CONFIG>(kernel);
	int blockstart = bnum * warpsperblock;

	for(int z = 0; z != kernel.BlockZ(); ++z)
	{
		for(int y = 0; y != kernel.BlockY(); ++y)
		{
			for(int x = 0; x != kernel.BlockX(); ++x)
			{
				int tid = ((z * kernel.BlockY()) + y) * kernel.BlockX() + x;
				int warpid = blockstart + tid / CONFIG::WARP_SIZE;
				int lane = tid % CONFIG::WARP_SIZE;
				
				uint32_t reg = BuildTID(x, y, z);
				GPRID r0id(warpid, 0, lane);
				cpu.registers_import->WriteTypedRegister(r0id, reg);
			}
		}
	}
}

template<class CONFIG>
struct Runner
{
	CoreSocket<CONFIG> & core;
	int c;
	Schedulable const & kernel;
	unsigned int core_count;
	
	Runner(CoreSocket<CONFIG> & core, Schedulable const & kernel,
		int core_count, int c) :
		core(core), c(c), kernel(kernel),
		core_count(core_count) {}
	void operator() ();
};

template<class CONFIG>
void Runner<CONFIG>::operator() ()
{
	int blocksPerCore = BlocksPerCore<CONFIG>(kernel);
	int blocksPerGPU = blocksPerCore * core_count;
	int width = kernel.GridX();
	int height = kernel.GridY();
	
	// Simple round-robin scheduling policy
	for(int i = c * blocksPerCore; i < height * width; i += blocksPerGPU)
	{
		int blocks = std::min(blocksPerCore, height * width - i);
		
		if(blocks > 0)
		{
			// Set configuration registers
			core.configuration_import->WriteTypedRegister(
				ConfigurationRegisterID(ConfigurationRegisterID::ThreadsPerBlock),
				ThreadsPerBlock(kernel));
			core.configuration_import->WriteTypedRegister(
				ConfigurationRegisterID(ConfigurationRegisterID::Blocks),
				blocks);
			core.configuration_import->WriteTypedRegister(
				ConfigurationRegisterID(ConfigurationRegisterID::GPRsPerWarp),
				kernel.GPRs());
			core.configuration_import->WriteTypedRegister(
				ConfigurationRegisterID(ConfigurationRegisterID::SMSize),
				kernel.SharedTotal());
			
			//cpu->Reset(kernel.ThreadsPerBlock(), blocks, kernel.GPRs(), kernel.SharedTotal());
			core.reset_import->Reset();

			for(int j = 0; j < blocks; ++j)
			{
				int bidx = (i+j) % width;
				int bidy = (i+j) / width;
				kernel.InitShared(*core.shared_memory_import, j, bidx, bidy, c);
				SetThreadIDs(kernel, j, core);
			}

			// Start execution
			core.run_import->Run();
		}
		
		// TODO: Sync
	}
}


template<class CONFIG>
CUDAScheduler<CONFIG>::CUDAScheduler(unsigned int cores, const char *name, Object *parent) :
	Object(name, parent),
	core_count(cores),
	sockets(cores)
{
	for(int i = 0; i != cores; ++i)
	{
		std::ostringstream name;
		name << "scheduler_socket_" << i;
		sockets[i] = shared_ptr<CoreSocket<CONFIG> >(new CoreSocket<CONFIG>(name.str().c_str(), this));
	}
}


template<class CONFIG>
bool CUDAScheduler<CONFIG>::Setup()
{
	for(typename sockets_t::iterator it = sockets.begin();
	    it != sockets.end();
	    ++it)
	{
		(*it)->Setup();
	}
	return true;
}


template<class CONFIG>
void CUDAScheduler<CONFIG>::Run(Schedulable const & kernel)
{
	thread_group GPUThreads;
	
	for(int c = 1; c < core_count; ++c)
	{
		Runner<CONFIG> runner(Socket(c), kernel, core_count, c);
		GPUThreads.create_thread(runner);
	}
	
	// Run first block on current thread
	Runner<CONFIG>(Socket(0), kernel, core_count, 0)();
	
	GPUThreads.join_all();
}

template<class CONFIG>
CoreSocket<CONFIG> & CUDAScheduler<CONFIG>::Socket(unsigned int i)
{
	return *sockets[i];
}

} // end of namespace cuda_scheduler
} // end of namespace scheduler
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
