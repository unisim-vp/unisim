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

#include "fatformat.hh"
#include <iostream>
#include <cassert>

using namespace std;

bool FatFormat::verbose = true;

FatFormat::FatFormat(void const *fatCubin) :
	fcb(static_cast<cudaFatCudaBinary const *>(fatCubin))
{
}


char const * FatFormat::GetCubin(char const * gpuname)
{
    // TODO: fallback to ELF if Cubin not found
	assert(fcb->cubin[0].cubin != 0 && fcb->cubin[0].gpuProfileName != 0);
	if(verbose) {
	 std::cerr << "Fat: using cubin profile " << fcb->cubin[0].gpuProfileName << std::endl;
	 std::cerr << fcb->cubin[0].cubin << std::endl;
	}
	return fcb->cubin[0].cubin;
}


void FatFormat::Dump()
{
	std::cerr << "Fat file\n";
	std::cerr << " magic=" << hex << fcb->magic << dec << std::endl;
	std::cerr << " version=" << fcb->version << std::endl;
	std::cerr << " gpuInfoVersion=" << hex << fcb->gpuInfoVersion << dec << std::endl;
	std::cerr << " key=\"" << fcb->key << "\"" << std::endl;
	std::cerr << " ident=\"" << fcb->ident << "\"" << std::endl;
	std::cerr << " usageMode=\"" << fcb->usageMode << "\"" << std::endl;

	std::cerr << " ptx:\n";
	cudaFatPtxEntry const * ptx = fcb->ptx;
	while(ptx->ptx != 0)
	{
	 std::cerr << "  gpuProfileName=\"" << ptx->gpuProfileName << "\"\n";
		++ptx;
	}

	std::cerr << " cubin:\n";
	cudaFatCubinEntry const * cubin = fcb->cubin;
	while(cubin->cubin != 0)
	{
	 std::cerr << "  gpuProfileName=\"" << cubin->gpuProfileName << "\"\n";
		++cubin;
	}
	
	std::cerr << " debug:\n";
	cudaFatDebugEntry const * debug = fcb->debug;
	if(debug->debug != 0)	// Second one has invalid pointer??
	{
	 std::cerr << "  gpuProfileName=\"" << debug->gpuProfileName << "\"\n";
	 std::cerr << "  debug = \"" << debug->debug << "\"\n";
		++debug;
	}
	
	std::cerr << " flags=" << fcb->flags << std::endl;
}

