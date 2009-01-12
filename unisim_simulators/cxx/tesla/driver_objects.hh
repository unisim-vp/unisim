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
 *
 */

//#include <unisim/component/cxx/tesla/cpu.hh>
//#include <unisim/component/cxx/tesla/cpu.tcc>

#include <inttypes.h>
#include <cuda.h>

class CUdevice_st
{
public:
  CUdevice_st();
  
private:
  //  CPU<CPU_CONFIG> cpu;
  
};

class CUctx_st
{
public:
  CUctx_st(CUdevice dev);
  
private:
  CUdevice device;
  uint32_t usage_count;
  
};
 
//   typedef struct CUmod_st *CUmodule;
class CUmod_st
{
public:
  CUmod_st();
private:
  CUfunction function;
};

//    typedef struct CUfunc_st *CUfunction;
class CUfunc_st
{
public:
  CUfunc_st();
  CUresult cuLaunch();
private:
  uint32_t entrypoint;
};

//    typedef struct CUarray_st *CUarray;
class CUarray_st
{
public:

private:

};
//    typedef struct CUtexref_st *CUtexref;
class CUtexref_st
{
public:

private:

};
//    typedef struct CUevent_st *CUevent;
class CUevent_st
{
public:

private:

};
//    typedef struct CUstream_st *CUstream;
class CUstream_st
{
public:

private:

};
