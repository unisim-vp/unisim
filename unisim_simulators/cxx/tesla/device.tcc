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

template<class CONFIG>
Device<CONFIG>::Device() :
	Object("device_0"),
	cpu("gpu_core_0", this),
	memory("memory", this)
{
	cpu.memory_import >> memory.memory_export;
	memory.Setup();

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
void Run(Kernel<CONFIG> const & kernel)
{
}

template<class CONFIG>
void Device<CONFIG>::Reset()
{
}

#endif
