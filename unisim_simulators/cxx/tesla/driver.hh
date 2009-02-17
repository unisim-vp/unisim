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
 *     David Parello (david.parello@univ-perp.fr)
 *     Sylvain Collange (sylvain.collange@univ-perp.fr)
 *
 */
 
#ifndef SIMULATOR_CXX_TESLA_DRIVER_HH
#define SIMULATOR_CXX_TESLA_DRIVER_HH

#include <driver_objects.hh>
#include <module.hh>
#include <device.hh>

/*****************************************
 *                DRIVER                 *
 *****************************************/
template<class CONFIG>
struct Driver
{
public:
	//  const usigned int MAX_CONTEXT=1;
	static const unsigned int MAXDEVICE=1;

	Driver();

	//    Initialization
	CUresult  cuInit(unsigned int&Flags);

	//    Device management
	CUresult  cuDeviceGet(CUdevice *device, int ordinal);

	CUresult  cuDeviceGetCount(int *count);

	//    Context management
	CUresult  cuCtxCreate(CUcontext *pctx, unsigned int flags, CUdevice dev );
	CUresult  cuCtxDestroy( CUcontext ctx );

	//    Module management
	CUresult ModuleLoad(Module<CONFIG> * & module, const char *fname);
	CUresult ModuleUnload(Module<CONFIG> * hmod);
	
	//  CUresult cuModuleGetFunction(Kernel<CONFIG> **hfunc, Module<CONFIG> * hmod, const char *name);

	//    Memory management
	typename CONFIG::address_t MAlloc(size_t size);
	void Free(typename CONFIG::address_t addr);
 
	void FunctionDump(Kernel<CONFIG> const & kernel);
	void Launch(Kernel<CONFIG> & kernel);
	void LaunchGrid(Kernel<CONFIG> & kernel, int width, int height);
	
	void CopyHtoD(typename CONFIG::address_t dest, void const * src, size_t size);
	void CopyDtoH(void * dest, typename CONFIG::address_t src, size_t size);
	void CopyDtoD(void * dest, typename CONFIG::address_t src, size_t size);
	
	void Memset(typename CONFIG::address_t dest, uint32_t val, size_t n);
	void Memset(typename CONFIG::address_t dest, uint16_t val, size_t n);
	void Memset(typename CONFIG::address_t dest, uint8_t val, size_t n);
	
	Device<CONFIG> const & Dev(int dev) const;
	Device<CONFIG> & Dev(int dev);

private:
	//CUcontext context_list; // TODO: have to be a list of context.
	CUcontext current_context; // TODO: to be a per thread current context.
	Device<CONFIG> device[MAXDEVICE];	// TODO: one config per device

};

#endif

