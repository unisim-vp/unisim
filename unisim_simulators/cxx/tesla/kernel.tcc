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
 
#ifndef SIMULATOR_CXX_TESLA_KERNEL_TCC
#define SIMULATOR_CXX_TESLA_KERNEL_TCC

#include <module.hh>

#include <fstream>
#include <string>
#include <algorithm>
#include <cstring>
#include <sstream>
#include <cassert>
#include <iostream>

#include "exception.hh"
#include "cuda.h"

using namespace std;
template<class CONFIG>
Kernel<CONFIG>::Kernel(std::istream & is) :
	lmem(0),
	smem(0),
	reg(0),
	bar(0),
	param_size(0),
	dyn_smem(0),
	blockx(1),
	blocky(1),
	blockz(1),
	gridx(1),
	gridy(1)
{
	cerr << " Kernel segment\n";
	typedef string::iterator it_t;
	while(true)
	{
		if(!is) {
			throw ParsingException("Unexpected EOF");
		}
		string line;
		getline(is, line);
		if(Strip(line) != "")
		{
			it_t closebrace = find(line.begin(), line.end(), '}');
			it_t openbrace = find(line.begin(), line.end(), '{');
			it_t equalpos = find(line.begin(), line.end(), '=');
			if(openbrace != line.end())
			{
				string cmd = Strip(string(line.begin(), openbrace));
				if(closebrace != line.end())
				{
					throw ParsingException("Unsupported in-line field");
				}
				else
				{
					if(cmd == "bincode")
					{
						cerr << "  bincode {...}\n";
						ParseBinCode(bincode, is);
					}
					else if(cmd == "const")
					{
						cerr << "  const {...}\n";
						const_segs.push_back(MemSegment<CONFIG>(is, SegmentConst));
					}
					else
					{
						throw ParsingException("Unknown multi-line field : ", cmd);
					}
				}
			}
			else if(equalpos != line.end())
			{
				string valname = Strip(string(line.begin(), equalpos));
				string valvalue = Strip(string(++equalpos, line.end()));
				SetAttribute(valname, valvalue);
			}
			else if(closebrace != line.end())
			{
				break;
			}
			// Else, just ignore
		}
	}
}

template<class CONFIG>
Kernel<CONFIG>::Kernel()
{
}

#if 0
template<class CONFIG>
uint32_t Kernel<CONFIG>::ConstSize() const
{
}

template<class CONFIG>
uint32_t Kernel<CONFIG>::LocalSize() const
{
}
#endif

template<class CONFIG>
uint32_t Kernel<CONFIG>::CodeSize() const
{
	return bincode.size() * 4;
}

template<class CONFIG>
void Kernel<CONFIG>::Load(Service<unisim::service::interfaces::Memory<typename CONFIG::address_t> > & mem, uint32_t offset) const
{
	cerr << "Loading " << CodeSize() << "B code @" << std::hex << CONFIG::CODE_START + offset << std::endl;
	if(!mem.WriteMemory(CONFIG::CODE_START + offset, &bincode[0], CodeSize())) {
		cerr << "Kernel::Load : Cannot write memory??\n";
		throw CudaException(CUDA_ERROR_OUT_OF_MEMORY);	// Generic memory error
	}
	for(typename ConstList::const_iterator it = const_segs.begin();
		it != const_segs.end();
		++it)
	{
		it->Load(mem);
	}
	
}

template<class CONFIG>
void Kernel<CONFIG>::SetAttribute(std::string const & attrname, std::string const & value)
{
	cerr << "  " << attrname << " = " << value << endl;
	if(attrname == "name")
	{
		name = value;
	}
	else if(attrname == "lmem")
	{
		lmem = atoi(value.c_str());
	}
	else if(attrname == "smem")
	{
		smem = atoi(value.c_str());
	}
	else if(attrname == "reg")
	{
		reg = atoi(value.c_str());
	}
	else if(attrname == "bar")
	{
		bar = atoi(value.c_str());
	}
	else
	{
		throw ParsingException("Unknown attribute : ", attrname);
	}
}

template<class CONFIG>
std::string const & Kernel<CONFIG>::Name() const
{
	return name;
}

template<class CONFIG>
void Kernel<CONFIG>::Dump(std::ostream & os) const
{
}

template<class CONFIG>
void Kernel<CONFIG>::SetBlockShape(int x, int y, int z)
{
	if(x < 0 || y < 0 || z < 0)
		throw CudaException(CUDA_ERROR_INVALID_VALUE);
	// TODO: check individual upper bounds
		
	if(x == 0)
		x = 1;
	if(y == 0)
		y = 1;
	if(z == 0)
		z = 1;
	
	if(x * y * z > int(CONFIG::MAX_WARPS_PER_BLOCK * CONFIG::WARP_SIZE))
		throw CudaException(CUDA_ERROR_INVALID_VALUE);
	
	blockx = x;
	blocky = y;
	blockz = z;
}

template<class CONFIG>
void Kernel<CONFIG>::SetGridShape(int x, int y)
{
	if(x < 0 || y < 0)
		throw CudaException(CUDA_ERROR_INVALID_VALUE);
	// TODO: check individual upper bounds
		
	if(x == 0)
		x = 1;
	if(y == 0)
		y = 1;
	
	gridx = x;
	gridy = y;
}

template<class CONFIG>
int Kernel<CONFIG>::ThreadsPerBlock() const
{
	return blockz * blocky * blockx;
}

template<class CONFIG>
int Kernel<CONFIG>::WarpsPerBlock() const
{
	return (ThreadsPerBlock() + CONFIG::WARP_SIZE - 1) / CONFIG::WARP_SIZE;
}

template<class CONFIG>
int Kernel<CONFIG>::GPRs() const
{
	return reg;
}


template<class CONFIG>
int Kernel<CONFIG>::BlockX() const
{
	return blockx;
}

template<class CONFIG>
int Kernel<CONFIG>::BlockY() const
{
	return blocky;
}

template<class CONFIG>
int Kernel<CONFIG>::BlockZ() const
{
	return blockz;
}

template<class CONFIG>
void Kernel<CONFIG>::ParamSeti(int offset, uint32_t value)
{
	cerr << "ParamSeti @" << offset << " = " << value << endl; 
	ParamSetv(offset, &value, sizeof(value));
}

template<class CONFIG>
void Kernel<CONFIG>::ParamSetf(int offset, float value)
{
	cerr << "ParamSetf @" << offset << " = " << value << endl; 
	ParamSetv(offset, &value, sizeof(value));
}

template<class CONFIG>
void Kernel<CONFIG>::ParamSetv(int offset, void * data, int size)
{
	cerr << "ParamSetv @" << offset << "(" << size << ")" << endl; 
	if(offset + size > int(parameters.size())) {
		parameters.resize(offset + size);
	}
	uint8_t * bytedata = static_cast<uint8_t *>(data);
	std::copy(bytedata, bytedata + size, &parameters[offset]);
}

template<class CONFIG>
void Kernel<CONFIG>::ParamSetSize(int size)
{
	param_size = size;
	parameters.resize(size);
}

template<class CONFIG>
void Kernel<CONFIG>::SetSharedSize(int size)
{
	dyn_smem = size;
}

template<class CONFIG>
uint32_t Kernel<CONFIG>::SharedTotal() const
{
	// Align to DWORD boundary
	return (16 + param_size + smem + dyn_smem + 3) & (~3);
}

template<class CONFIG>
void Kernel<CONFIG>::InitShared(Service<unisim::service::interfaces::Memory<typename CONFIG::address_t> > & mem, int index,
	int bidx, int bidy) const
{
	// TODO: multiple cores
	// Header
	uint16_t header[8];	// 10?
	header[0] = 0;	// flags? gridid?
	header[1] = blockx;
	header[2] = blocky;
	header[3] = blockz;
	header[4] = gridx;	// gridx
	header[5] = gridy;	// gridy
	header[6] = bidx;	// bidx
	header[7] = bidy;	// bidy

	cerr << "Init block " << index << " (" << bidx << ", " << bidy << ") / ("
	<< gridx << ", " << gridy << ") shared memory\n";
	typename CONFIG::address_t shared_base = CONFIG::SHARED_START + index * SharedTotal();

	if(!mem.WriteMemory(shared_base, header, 16)) {
		throw CudaException(CUDA_ERROR_OUT_OF_MEMORY);
	}
	
	cerr << "Loading " << param_size << "B parameters @" << hex << shared_base + 16 << dec << endl;
	// Parameters
	if(!mem.WriteMemory(shared_base + 16, &parameters[0], param_size)) {
		throw CudaException(CUDA_ERROR_OUT_OF_MEMORY);
	}
}

template<class CONFIG>
int Kernel<CONFIG>::BlocksPerCore() const
{
	int blocksreg = CONFIG::MAX_VGPR / (reg * WarpsPerBlock());
	int blockssm = CONFIG::SHARED_SIZE / SharedTotal();	// TODO: align
	int blockswarps = CONFIG::MAX_WARPS / WarpsPerBlock();
	
	return std::min(std::min(blocksreg, blockssm), std::min(int(CONFIG::MAX_BLOCKS), blockswarps));
}

#endif
