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
 
#ifndef SIMULATOR_CXX_TESLA_DRIVER_DEVICE_HH
#define SIMULATOR_CXX_TESLA_DRIVER_DEVICE_HH

#include "driver_objects.hh"
#include "module.hh"
#include <iosfwd>


// Trivial allocator, no deallocation at all
// for the moment
// TODO: implement cuMemGetAddressRange
template<class CONFIG>
struct Allocator
{
	Allocator(typename CONFIG::address_t base, size_t max_size);
	
	typename CONFIG::address_t Alloc(size_t size);
	void Free(typename CONFIG::address_t addr);

private:
	typename CONFIG::address_t base;
	typename CONFIG::address_t limit;
	size_t max_size;

};

template<class CONFIG>
struct Device: CUdevice_st, Object
{
	Device();

	void DumpCode(Kernel<CONFIG> const & kernel, std::ostream & os);
	void Run(Kernel<CONFIG> const & kernel);
	
	typename CONFIG::address_t MAlloc(size_t size);
	void Free(typename CONFIG::address_t addr);

	void CopyHtoD(typename CONFIG::address_t dest, void const * src, size_t size);
	void CopyDtoH(void * dest, typename CONFIG::address_t src, size_t size);
	void CopyDtoD(void * dest, typename CONFIG::address_t src, size_t size);

	void Memset(typename CONFIG::address_t dest, uint32_t val, size_t n);
	void Memset(typename CONFIG::address_t dest, uint16_t val, size_t n);
	void Memset(typename CONFIG::address_t dest, uint8_t val, size_t n);

private:
	void Load(Kernel<CONFIG> const & kernel);
	void Reset();
	void SetThreadIDs(Kernel<CONFIG> const & kernel);
	uint32_t BuildTID(int x, int y, int z);

	CPU<CONFIG> cpu;
	unisim::component::cxx::memory::ram::Memory<typename CONFIG::address_t> memory;
	Allocator<CONFIG> global_allocator;
};



#endif