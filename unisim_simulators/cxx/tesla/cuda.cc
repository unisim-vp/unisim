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

#include <cuda.h>
#include <driver.hh>
#include <iostream>

#include "config.hh"

#include "driver.tcc"
#include "module.tcc"
#include "device.tcc"

using std::cerr;
using std::endl;

typedef BaseConfig MyConfig;

Driver<MyConfig> driver;

#define CHECK_PTR(p) \
	{ if(!p) { \
		cerr << "Null pointer " #p " at " << __FILE__ << ":" << __LINE__ << endl; \
		return CUDA_ERROR_INVALID_VALUE; } }

/*********************************
 ** Initialization
 *********************************/
CUresult  CUDAAPI cuInit(unsigned int Flags)
{
  return driver.cuInit(Flags);
}
/************************************
 **
 **    Device management
 **
 ***********************************/

CUresult  CUDAAPI cuDeviceGet(CUdevice *device, int ordinal)
{
  return driver.cuDeviceGet(device, ordinal);
}

CUresult  CUDAAPI cuDeviceGetCount(int *count)
{
  return driver.cuDeviceGetCount(count);
}

CUresult  CUDAAPI cuDeviceGetName(char *name, int len, CUdevice dev)
{
  cerr << "function not implemented !!!" << endl;
  abort();
}

CUresult  CUDAAPI cuDeviceComputeCapability(int *major, int *minor, CUdevice dev)
{
  cerr << "function not implemented !!!" << endl;
  abort();
}

CUresult  CUDAAPI cuDeviceTotalMem(unsigned int *bytes, CUdevice dev)
{
  cerr << "function not implemented !!!" << endl;
  abort();
}

CUresult  CUDAAPI cuDeviceGetProperties(CUdevprop *prop, CUdevice dev)
{
  cerr << "function not implemented !!!" << endl;
  abort();
}

CUresult  CUDAAPI cuDeviceGetAttribute(int *pi, CUdevice_attribute attrib, CUdevice dev)
{
  cerr << "function not implemented !!!" << endl;
  abort();
}

        
    /************************************
     **
     **    Context management
     **
     ***********************************/

CUresult  CUDAAPI cuCtxCreate(CUcontext *pctx, unsigned int flags, CUdevice dev )
{
  return driver.cuCtxCreate(pctx, flags, dev);
}

CUresult  CUDAAPI cuCtxDestroy( CUcontext ctx )
{
  return driver.cuCtxDestroy(ctx);
}

CUresult  CUDAAPI cuCtxAttach(CUcontext *pctx, unsigned int flags)
{
  cerr << "function not implemented !!!" << endl;
  abort();
}

CUresult  CUDAAPI cuCtxDetach(CUcontext ctx)
{
  cerr << "function not implemented !!!" << endl;
  abort();
}

CUresult  CUDAAPI cuCtxPushCurrent( CUcontext ctx )
{
  cerr << "function not implemented !!!" << endl;
  abort();
}

CUresult  CUDAAPI cuCtxPopCurrent( CUcontext *pctx )
{
  cerr << "function not implemented !!!" << endl;
  abort();
}

CUresult  CUDAAPI cuCtxGetDevice(CUdevice *device)
{
  cerr << "function not implemented !!!" << endl;
  abort();
}

CUresult cuCtxSynchronize()
{
	return CUDA_SUCCESS;
}



/************************************
 **
 **    Module management
 **
 ***********************************/

CUresult cuModuleLoad(CUmodule *module, const char *fname)
{
	Module<MyConfig>* mod = 0;
	CUresult res = driver.ModuleLoad(mod, fname);
	assert(mod != 0);
	*module = mod;
	return res;
}

CUresult  CUDAAPI cuModuleLoadData(CUmodule *module, const void *image)
{
  cerr << "function not implemented !!!" << endl;
  abort();
}

CUresult  CUDAAPI cuModuleLoadFatBinary(CUmodule *module, const void *fatCubin)
{
  cerr << "function not implemented !!!" << endl;
  abort();
}

CUresult  CUDAAPI cuModuleUnload(CUmodule hmod)
{
  return driver.ModuleUnload(static_cast<Module<MyConfig>*>(hmod));
}

CUresult cuModuleGetFunction(CUfunction *hfunc, CUmodule hmod, const char *name)
{
	CHECK_PTR(hmod);
	Module<MyConfig>* mod = static_cast<Module<MyConfig>*>(hmod);
	Kernel<MyConfig>* kernel;
	try {
		kernel = &mod->GetKernel(name);
		*hfunc = kernel;
		return CUDA_SUCCESS;
	}
	catch(CudaException e) {
		return e.code;
	}
}

CUresult  CUDAAPI cuModuleGetGlobal(CUdeviceptr *dptr, unsigned int *bytes, CUmodule hmod, const char *name)
{
  cerr << "function not implemented !!!" << endl;
  abort();
}

CUresult  CUDAAPI cuModuleGetTexRef(CUtexref *pTexRef, CUmodule hmod, const char *name)
{
  cerr << "function not implemented !!!" << endl;
  abort();
}


/************************************
 **
 **    Memory management
 **
 ***********************************/

CUresult CUDAAPI cuMemGetInfo(unsigned int *free, unsigned int *total)
{
  cerr << "function not implemented !!!" << endl;
  abort();
}


CUresult CUDAAPI cuMemAlloc( CUdeviceptr *dptr, unsigned int bytesize)
{
	try
	{
		MyConfig::address_t addr = driver.MAlloc(bytesize);
		*dptr = addr;
		return CUDA_SUCCESS;
	}
	catch(CudaException e) {
		return e.code;
	}
}

CUresult CUDAAPI cuMemAllocPitch( CUdeviceptr *dptr, 
				  unsigned int *pPitch,
				  unsigned int WidthInBytes, 
				  unsigned int Height, 
				  // size of biggest r/w to be performed by kernels on this memory
				  // 4, 8 or 16 bytes
				  unsigned int ElementSizeBytes
				  )
{
  cerr << "function not implemented !!!" << endl;
  abort();
}

CUresult CUDAAPI cuMemFree(CUdeviceptr dptr)
{
	try
	{
		driver.Free(dptr);
		return CUDA_SUCCESS;
	}
	catch(CudaException e) {
		return e.code;
	}
}

CUresult CUDAAPI cuMemGetAddressRange( CUdeviceptr *pbase, unsigned int *psize, CUdeviceptr dptr )
{
  cerr << "function not implemented !!!" << endl;
  abort();
}

CUresult CUDAAPI cuMemAllocHost(void **pp, unsigned int bytesize)
{
	try
	{
		*pp = new uint8_t[bytesize];
		return CUDA_SUCCESS;
	}
	catch(std::bad_alloc & ba)
	{
		return CUDA_ERROR_OUT_OF_MEMORY;
	}
}

CUresult CUDAAPI cuMemFreeHost(void *p)
{
	uint8_t * ptr = static_cast<uint8_t*>(p);
	delete [] ptr;
	return CUDA_SUCCESS;
}


/************************************
 **
 **    Synchronous Memcpy
 **
 ** Intra-device memcpy's done with these functions may execute in parallel with the CPU,
 ** but if host memory is involved, they wait until the copy is done before returning.
 **
 ***********************************/

// 1D functions
// system <-> device memory
CUresult  CUDAAPI cuMemcpyHtoD (CUdeviceptr dstDevice, const void *srcHost, unsigned int ByteCount )
{
	try
	{
		driver.CopyHtoD(dstDevice, srcHost, ByteCount);
		return CUDA_SUCCESS;
	}
	catch(CudaException e) {
		return e.code;
	}
}

CUresult  CUDAAPI cuMemcpyDtoH (void *dstHost, CUdeviceptr srcDevice, unsigned int ByteCount )
{
	try
	{
		driver.CopyDtoH(dstHost, srcDevice, ByteCount);
		return CUDA_SUCCESS;
	}
	catch(CudaException e) {
		return e.code;
	}
}


// device <-> device memory
CUresult  CUDAAPI cuMemcpyDtoD (CUdeviceptr dstDevice, CUdeviceptr srcDevice, unsigned int ByteCount )
{
  cerr << "function not implemented !!!" << endl;
  abort();
}


// device <-> array memory
CUresult  CUDAAPI cuMemcpyDtoA ( CUarray dstArray, unsigned int dstIndex, CUdeviceptr srcDevice, unsigned int ByteCount )
{
  cerr << "function not implemented !!!" << endl;
  abort();
}

CUresult  CUDAAPI cuMemcpyAtoD ( CUdeviceptr dstDevice, CUarray hSrc, unsigned int SrcIndex, unsigned int ByteCount )
{
  cerr << "function not implemented !!!" << endl;
  abort();
}


// system <-> array memory
CUresult  CUDAAPI cuMemcpyHtoA( CUarray dstArray, unsigned int dstIndex, const void *pSrc, unsigned int ByteCount )
{
  cerr << "function not implemented !!!" << endl;
  abort();
}


CUresult  CUDAAPI cuMemcpyAtoH( void *dstHost, CUarray srcArray, unsigned int srcIndex, unsigned int ByteCount )
{
  cerr << "function not implemented !!!" << endl;
  abort();
}



// array <-> array memory
CUresult  CUDAAPI cuMemcpyAtoA( CUarray dstArray, unsigned int dstIndex, CUarray srcArray, unsigned int srcIndex, unsigned int ByteCount )
{
  cerr << "function not implemented !!!" << endl;
  abort();
}



// 2D memcpy
CUresult  CUDAAPI cuMemcpy2D( const CUDA_MEMCPY2D *pCopy )
{
  cerr << "function not implemented !!!" << endl;
  abort();
}


CUresult  CUDAAPI cuMemcpy2DUnaligned( const CUDA_MEMCPY2D *pCopy )
{
  cerr << "function not implemented !!!" << endl;
  abort();
}



// 3D memcpy
CUresult  CUDAAPI cuMemcpy3D( const CUDA_MEMCPY3D *pCopy )
{
  cerr << "function not implemented !!!" << endl;
  abort();
}



/************************************
 **
 **    Asynchronous Memcpy
 **
 ** Any host memory involved must be DMA'able (e.g., allocated with cuMemAllocHost).
 ** memcpy's done with these functions execute in parallel with the CPU and, if
 ** the hardware is available, may execute in parallel with the GPU.
 ** Asynchronous memcpy must be accompanied by appropriate stream synchronization.
 **
 ***********************************/

// 1D functions
// system <-> device memory
CUresult  CUDAAPI cuMemcpyHtoDAsync (CUdeviceptr dstDevice, 
				     const void *srcHost, unsigned int ByteCount, CUstream hStream )
{
  cerr << "function not implemented !!!" << endl;
  abort();
}


CUresult  CUDAAPI cuMemcpyDtoHAsync (void *dstHost, 
				     CUdeviceptr srcDevice, unsigned int ByteCount, CUstream hStream )
{
  cerr << "function not implemented !!!" << endl;
  abort();
}



// system <-> array memory
CUresult  CUDAAPI cuMemcpyHtoAAsync( CUarray dstArray, unsigned int dstIndex, 
				     const void *pSrc, unsigned int ByteCount, CUstream hStream )
{
  cerr << "function not implemented !!!" << endl;
  abort();
}


CUresult  CUDAAPI cuMemcpyAtoHAsync( void *dstHost, CUarray srcArray, unsigned int srcIndex, 
				     unsigned int ByteCount, CUstream hStream )
{
  cerr << "function not implemented !!!" << endl;
  abort();
}



// 2D memcpy
CUresult  CUDAAPI cuMemcpy2DAsync( const CUDA_MEMCPY2D *pCopy, CUstream hStream )
{
  cerr << "function not implemented !!!" << endl;
  abort();
}



// 3D memcpy
CUresult  CUDAAPI cuMemcpy3DAsync( const CUDA_MEMCPY3D *pCopy, CUstream hStream )
{
  cerr << "function not implemented !!!" << endl;
  abort();
}



/************************************
 **
 **    Memset
 **
 ***********************************/
CUresult  CUDAAPI cuMemsetD8( CUdeviceptr dstDevice, unsigned char uc, unsigned int N )
{
  cerr << "function not implemented !!!" << endl;
  abort();
}


CUresult  CUDAAPI cuMemsetD16( CUdeviceptr dstDevice, unsigned short us, unsigned int N )
{
  cerr << "function not implemented !!!" << endl;
  abort();
}


CUresult  CUDAAPI cuMemsetD32( CUdeviceptr dstDevice, unsigned int ui, unsigned int N )
{
	try
	{
		driver.Memset(dstDevice, ui, N);
		return CUDA_SUCCESS;
	}
	catch(CudaException e) {
		return e.code;
	}
}



CUresult  CUDAAPI cuMemsetD2D8( CUdeviceptr dstDevice, unsigned int dstPitch, unsigned char uc, unsigned int Width, unsigned int Height )
{
  cerr << "function not implemented !!!" << endl;
  abort();
}


CUresult  CUDAAPI cuMemsetD2D16( CUdeviceptr dstDevice, unsigned int dstPitch, unsigned short us, unsigned int Width, unsigned int Height )
{
  cerr << "function not implemented !!!" << endl;
  abort();
}


CUresult  CUDAAPI cuMemsetD2D32( CUdeviceptr dstDevice, unsigned int dstPitch, unsigned int ui, unsigned int Width, unsigned int Height )
{
  cerr << "function not implemented !!!" << endl;
  abort();
}



/************************************
 **
 **    Function management
 **
 ***********************************/


CUresult cuFuncSetBlockShape(CUfunction hfunc, int x, int y, int z)
{
	Kernel<MyConfig> * kernel = static_cast<Kernel<MyConfig> *>(hfunc);
	try {
		kernel->SetBlockShape(x, y, z);
		return CUDA_SUCCESS;
	}
	catch(CudaException e) {
		return e.code;
	}
	return CUDA_SUCCESS;
}


CUresult cuFuncSetSharedSize(CUfunction hfunc, unsigned int bytes)
{
	Kernel<MyConfig> * kernel = static_cast<Kernel<MyConfig> *>(hfunc);
	try {
		kernel->SetSharedSize(bytes);
		return CUDA_SUCCESS;
	}
	catch(CudaException e) {
		return e.code;
	}
}



/************************************
 **
 **    Array management 
 **
 ***********************************/
CUresult  CUDAAPI cuArrayCreate( CUarray *pHandle, const CUDA_ARRAY_DESCRIPTOR *pAllocateArray )
{
  cerr << "function not implemented !!!" << endl;
  abort();
}


CUresult  CUDAAPI cuArrayGetDescriptor( CUDA_ARRAY_DESCRIPTOR *pArrayDescriptor, CUarray hArray )
{
  cerr << "function not implemented !!!" << endl;
  abort();
}


CUresult  CUDAAPI cuArrayDestroy( CUarray hArray )
{
  cerr << "function not implemented !!!" << endl;
  abort();
}



CUresult  CUDAAPI cuArray3DCreate( CUarray *pHandle, const CUDA_ARRAY3D_DESCRIPTOR *pAllocateArray )
{
  cerr << "function not implemented !!!" << endl;
  abort();
}


CUresult  CUDAAPI cuArray3DGetDescriptor( CUDA_ARRAY3D_DESCRIPTOR *pArrayDescriptor, CUarray hArray )
{
  cerr << "function not implemented !!!" << endl;
  abort();
}



/************************************
 **
 **    Texture reference management
 **
 ***********************************/
CUresult  CUDAAPI cuTexRefCreate( CUtexref *pTexRef )
{
  cerr << "function not implemented !!!" << endl;
  abort();
}


CUresult  CUDAAPI cuTexRefDestroy( CUtexref hTexRef )
{
  cerr << "function not implemented !!!" << endl;
  abort();
}



CUresult  CUDAAPI cuTexRefSetArray( CUtexref hTexRef, CUarray hArray, unsigned int Flags )
{
  cerr << "function not implemented !!!" << endl;
  abort();
}


// override the texref format with a format inferred from the array
//        #define CU_TRSA_OVERRIDE_FORMAT 0x01
CUresult  CUDAAPI cuTexRefSetAddress( unsigned int *ByteOffset, CUtexref hTexRef, CUdeviceptr dptr, unsigned int bytes )
{
  cerr << "function not implemented !!!" << endl;
  abort();
}


CUresult  CUDAAPI cuTexRefSetFormat( CUtexref hTexRef, CUarray_format fmt, int NumPackedComponents )
{
  cerr << "function not implemented !!!" << endl;
  abort();
}



CUresult  CUDAAPI cuTexRefSetAddressMode( CUtexref hTexRef, int dim, CUaddress_mode am )
{
  cerr << "function not implemented !!!" << endl;
  abort();
}


CUresult  CUDAAPI cuTexRefSetFilterMode( CUtexref hTexRef, CUfilter_mode fm )
{
  cerr << "function not implemented !!!" << endl;
  abort();
}


CUresult  CUDAAPI cuTexRefSetFlags( CUtexref hTexRef, unsigned int Flags )
{
  cerr << "function not implemented !!!" << endl;
  abort();
}


// read the texture as integers rather than promoting the values
// to floats in the range [0,1]
//        #define CU_TRSF_READ_AS_INTEGER         0x01

// use normalized texture coordinates in the range [0,1) instead of [0,dim)
//        #define CU_TRSF_NORMALIZED_COORDINATES  0x02

CUresult  CUDAAPI cuTexRefGetAddress( CUdeviceptr *pdptr, CUtexref hTexRef )
{
  cerr << "function not implemented !!!" << endl;
  abort();
}


CUresult  CUDAAPI cuTexRefGetArray( CUarray *phArray, CUtexref hTexRef )
{
  cerr << "function not implemented !!!" << endl;
  abort();
}


CUresult  CUDAAPI cuTexRefGetAddressMode( CUaddress_mode *pam, CUtexref hTexRef, int dim )
{
  cerr << "function not implemented !!!" << endl;
  abort();
}


CUresult  CUDAAPI cuTexRefGetFilterMode( CUfilter_mode *pfm, CUtexref hTexRef )
{
  cerr << "function not implemented !!!" << endl;
  abort();
}


CUresult  CUDAAPI cuTexRefGetFormat( CUarray_format *pFormat, int *pNumChannels, CUtexref hTexRef )
{
  cerr << "function not implemented !!!" << endl;
  abort();
}


CUresult  CUDAAPI cuTexRefGetFlags( unsigned int *pFlags, CUtexref hTexRef )
{
  cerr << "function not implemented !!!" << endl;
  abort();
}



/************************************
 **
 **    Parameter management
 **
 ***********************************/

CUresult cuParamSetSize(CUfunction hfunc, unsigned int numbytes)
{
	Kernel<MyConfig> * kernel = static_cast<Kernel<MyConfig> *>(hfunc);
	try {
		kernel->ParamSetSize(numbytes);
		return CUDA_SUCCESS;
	}
	catch(CudaException e) {
		return e.code;
	}
}

CUresult cuParamSeti(CUfunction hfunc, int offset, unsigned int value)
{
	Kernel<MyConfig> * kernel = static_cast<Kernel<MyConfig> *>(hfunc);
	try {
		kernel->ParamSeti(offset, value);
		return CUDA_SUCCESS;
	}
	catch(CudaException e) {
		return e.code;
	}
}

CUresult cuParamSetf(CUfunction hfunc, int offset, float value)
{
	Kernel<MyConfig> * kernel = static_cast<Kernel<MyConfig> *>(hfunc);
	try {
		kernel->ParamSetf(offset, value);
		return CUDA_SUCCESS;
	}
	catch(CudaException e) {
		return e.code;
	}
}

CUresult cuParamSetv(CUfunction hfunc, int offset, void * ptr, unsigned int numbytes)
{
	Kernel<MyConfig> * kernel = static_cast<Kernel<MyConfig> *>(hfunc);
	try {
		kernel->ParamSetv(offset, ptr, numbytes);
		return CUDA_SUCCESS;
	}
	catch(CudaException e) {
		return e.code;
	}
}


CUresult cuParamSetTexRef(CUfunction hfunc, int texunit, CUtexref hTexRef)
{
  cerr << "function not implemented !!!" << endl;
  abort();
}


// for texture references loaded into the module,
// use default texunit from texture reference
//        #define CU_PARAM_TR_DEFAULT -1

/************************************
 **
 **    Launch functions
 **
 ***********************************/

CUresult cuLaunch (CUfunction f)
{
	Kernel<MyConfig> * kernel = static_cast<Kernel<MyConfig> *>(f);
	try {
		driver.Launch(*kernel);
		return CUDA_SUCCESS;
	}
	catch(CudaException e) {
		return e.code;
	}
}


CUresult CUDAAPI cuLaunchGrid (CUfunction f, int grid_width, int grid_height)
{
  cerr << "function not implemented !!!" << endl;
  abort();
}


CUresult CUDAAPI cuLaunchGridAsync( CUfunction f, int grid_width, int grid_height, CUstream hStream )
{
  cerr << "function not implemented !!!" << endl;
  abort();
}



/************************************
 **
 **    Events
 **
 ***********************************/
CUresult CUDAAPI cuEventCreate( CUevent *phEvent, unsigned int Flags )
{
  cerr << "function not implemented !!!" << endl;
  abort();
}


CUresult CUDAAPI cuEventRecord( CUevent hEvent, CUstream hStream )
{
  cerr << "function not implemented !!!" << endl;
  abort();
}


CUresult CUDAAPI cuEventQuery( CUevent hEvent )
{
  cerr << "function not implemented !!!" << endl;
  abort();
}


CUresult CUDAAPI cuEventSynchronize( CUevent hEvent )
{
  cerr << "function not implemented !!!" << endl;
  abort();
}


CUresult CUDAAPI cuEventDestroy( CUevent hEvent )
{
  cerr << "function not implemented !!!" << endl;
  abort();
}


CUresult CUDAAPI cuEventElapsedTime( float *pMilliseconds, CUevent hStart, CUevent hEnd )
{
  cerr << "function not implemented !!!" << endl;
  abort();
}



/************************************
 **
 **    Streams
 **
 ***********************************/
CUresult CUDAAPI  cuStreamCreate( CUstream *phStream, unsigned int Flags )
{
  cerr << "function not implemented !!!" << endl;
  abort();
}


CUresult CUDAAPI  cuStreamQuery( CUstream hStream )
{
  cerr << "function not implemented !!!" << endl;
  abort();
}


CUresult CUDAAPI  cuStreamSynchronize( CUstream hStream )
{
  cerr << "function not implemented !!!" << endl;
  abort();
}


CUresult CUDAAPI  cuStreamDestroy( CUstream hStream )
{
  cerr << "function not implemented !!!" << endl;
  abort();
}

/************************************
 **
 **    Barra extensions (debug, instrumentation...)
 **
 ***********************************/


CUresult barFunctionDump(CUfunction f)
{
	try {
		driver.FunctionDump(*static_cast<Kernel<MyConfig>*>(f));
		return CUDA_SUCCESS;
	}
	catch(CudaException e) {
		return e.code;
	}
}


//#endif /* __cuda_cuda_cc__ */
