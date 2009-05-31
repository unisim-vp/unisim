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
 
#ifndef SIMULATOR_CXX_TESLA_KERNEL_HH
#define SIMULATOR_CXX_TESLA_KERNEL_HH

#include <string>
#include <iosfwd>
#include <vector>
#include <list>
#include <map>
#include <unisim/kernel/service/service.hh>
#include <unisim/service/interfaces/memory.hh>
#include <unisim/component/cxx/processor/tesla/stats.hh>

using unisim::kernel::service::Service;
using unisim::component::cxx::processor::tesla::CPU;
using unisim::component::cxx::processor::tesla::Stats;
//using unisim::service::interfaces::Memory;
// Conflicts with ram::Memory, do NOT use in headers

template<class CONFIG>
struct MemSegment;

template<class CONFIG>
struct Module;

template<class CONFIG>
struct Sampler;

template<class CONFIG>
struct Allocator;

template<class CONFIG>
struct Kernel : CUfunc_st
{
	Kernel(Module<CONFIG> * module, std::istream & is);
	Kernel();	// Default constructor needed for std containers
//	uint32_t ConstSize() const;
//	uint32_t LocalSize() const;
	uint32_t CodeSize() const;

	void Load(Service<unisim::service::interfaces::Memory<typename CONFIG::address_t> > & mem,
		Allocator<CONFIG> & allocator, uint32_t offset = 0);
	std::string const & Name() const;
	void Dump(std::ostream & os) const;
	void SetBlockShape(int x, int y, int z);
	void SetGridShape(int x, int y);
	int BlockX() const;
	int BlockY() const;
	int BlockZ() const;
	int ThreadsPerBlock() const;
	int WarpsPerBlock() const;
	int GPRs() const;
	
	void ParamSeti(int offset, uint32_t value);
	void ParamSetf(int offest, float value);
	void ParamSetv(int offset, void * data, int size);
	void ParamSetSize(int size);
	void SetSharedSize(int size);
	void SetTexRef(::Sampler<CONFIG> * sampler);
	void LoadSamplers(CPU<CONFIG> & cpu);
	
	uint32_t SharedTotal() const;
	void InitShared(Service<unisim::service::interfaces::Memory<typename CONFIG::address_t> > & mem, int index = 0,
		int bidx = 0, int bidy = 0, int core = 0) const;
	
	int BlocksPerCore() const;	// Max blocks that can run on a SM of target architecture
	
	Stats<CONFIG> stats;
	
	static bool trace_loading;
	static bool trace_parsing;
private:
	void SetAttribute(std::string const & name, std::string const & value);

	Module<CONFIG> * module;
	std::string name;
	int lmem;
	int smem;
	int reg;
	int bar;
	std::vector<uint32_t> bincode;
	//std::vector<uint32_t> const_mem;
	typedef std::list<MemSegment<CONFIG> > ConstList;
	ConstList const_segs;
	std::vector<uint8_t> parameters;
	int param_size;
	int dyn_smem;
	
	int blockx, blocky, blockz;
	int gridx, gridy;
	::Sampler<CONFIG> * samplers[CONFIG::MAX_SAMPLERS];
};



#endif
