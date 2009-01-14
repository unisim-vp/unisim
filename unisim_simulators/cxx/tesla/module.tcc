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
 
#ifndef SIMULATOR_CXX_TESLA_MODULE_TCC
#define SIMULATOR_CXX_TESLA_MODULE_TCC

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

inline string Strip(string s)
{
	typedef string::iterator it_t;
	size_t start = s.find_first_not_of(" \t");
	size_t end = s.find_last_not_of(" \t");
	if(start == string::npos || end == string::npos)
		return string();
	else
		return s.substr(start, end - start + 1);
}

inline void ParseBinCode(vector<uint32_t> & v, istream & is)
{
	while(true)
	{
		if(!is) {
			throw ParsingException("Unexpected EOF");
		}
		string line;
		getline(is, line);
		if(Strip(line) != "}")
		{
			istringstream iss(line);
			iss.setf(ios::hex, ios::basefield);
			iss.setf(ios::showbase);
			uint32_t val;
			iss >> val;
			while(iss)
			{
				//std::cerr << std::hex << val << " ";
				v.push_back(val);
				iss >> val;
			}
			//std::cerr << std::endl;
		}
		else
		{
			break;
		}
	}
}

ParsingException::ParsingException() :
	str(0)
{
	assert(false);
}

ParsingException::ParsingException(char const * str) :
	str(str)
{
	std::cerr << str;
	assert(false);
}

ParsingException::ParsingException(char const * str, std::string const & param) :
	str(str), param(param)
{
	std::cerr << str << param;
	assert(false);
}

std::ostream & operator<< (std::ostream & os, ParsingException const & pe)
{
	os << pe.str;
	os << pe.param;
	return os;
}

template<class CONFIG>
ConstSeg<CONFIG>::ConstSeg(std::istream & is)
{
	cerr << " Constant segment\n";
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
					if(cmd == "mem")
					{
						cerr << "  Mem {...}\n";
						ParseBinCode(mem, is);
					}
					else
					{
						throw ParsingException("Unknown field : ", cmd);
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
ConstSeg<CONFIG>::ConstSeg()
{
}

template<class CONFIG>
uint32_t ConstSeg<CONFIG>::Size() const
{
	assert(mem.size() == 0 || 4 * bytes == mem.size());
	return bytes;
}

template<class CONFIG>
void ConstSeg<CONFIG>::SetAttribute(std::string const & attrname, std::string const & value)
{
	cerr << "  " << attrname << " = " << value << endl;
	if(attrname == "name")
	{
		name = value;
	}
	else if(attrname == "segname")
	{
		segname = value;
	}
	else if(attrname == "segnum")
	{
		segnum = atoi(value.c_str());
	}
	else if(attrname == "offset")
	{
		offset = atoi(value.c_str());
	}
	else if(attrname == "bytes")
	{
		bytes = atoi(value.c_str());
	}
	else
	{
		throw ParsingException("Unknown attribute : ", attrname);
	}
}

template<class CONFIG>
void ConstSeg<CONFIG>::Load(Service<Memory<typename CONFIG::address_t> > & memory) const
{
	if(!memory.WriteMemory(Address(), &mem[0], mem.size() * 4)) {
		throw CudaException(CUDA_ERROR_OUT_OF_MEMORY);	// Generic memory error
	}
}

template<class CONFIG>
std::string const & ConstSeg<CONFIG>::Name() const
{
	return name;
}

template<class CONFIG>
typename CONFIG::address_t ConstSeg<CONFIG>::Address() const
{
	return CONFIG::CONST_START
		+ segnum * CONFIG::CONST_SEG_SIZE
		+ offset;
}

template<class CONFIG>
Kernel<CONFIG>::Kernel(std::istream & is)
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
void Kernel<CONFIG>::Load(Service<Memory<typename CONFIG::address_t> > & mem, uint32_t offset) const
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
Module<CONFIG>::Module(char const * fname)
{
	LoadCubin(fname);
}

template<class CONFIG>
Kernel<CONFIG> & Module<CONFIG>::GetKernel(char const * name)
{
	cerr << "GetFunction : \"" << name << "\"\n";
	typename KernelMap::iterator it = kernels.find(string(name));
	if(it == kernels.end()) {
		throw CudaException(CUDA_ERROR_NOT_FOUND);
	}
	return it->second;
}

template<class CONFIG>
ConstSeg<CONFIG> & Module<CONFIG>::GetConstant(char const * name)
{
	typename ConstMap::iterator it = global_constants.find(string(name));
	if(it == global_constants.end()) {
		throw CudaException(CUDA_ERROR_NOT_FOUND);
	}
	return it->second;
}

template<class CONFIG>
void Module<CONFIG>::LoadCubin(char const * fname)
{
	// Mostly adapted from Decuda
	cerr << "Cubin " << fname << endl;
	typedef string::iterator it_t;
	ifstream is(fname, ifstream::in);
	while(is)
	{
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
					string value = string(++openbrace, closebrace);
					SetField(cmd, value);
				}
				else
				{
					if(cmd == "code")
					{
						Kernel<CONFIG> kernel(is);
						//kernels.push_back(Kernel<CONFIG>(is));
						if(kernel.Name().empty()) {
							throw ParsingException("Unnamed code field");
						}
						kernels[kernel.Name()] = kernel;
					}
					else if(cmd == "consts")
					{
						//global_constants.push_back(ConstSeg<CONFIG>(is));
						ConstSeg<CONFIG> cs(is);
						if(cs.Name().empty()) {
							throw ParsingException("Unnamed code field");
						}
						global_constants[cs.Name()] = cs;
					}
					else if(cmd == "samplers")
					{
						// ignore for now
					}
					else
					{
						throw ParsingException("Unknown multi-line field : ", cmd);
					}
				}
			}
		}
	}
}

template<class CONFIG>
void Module<CONFIG>::SetField(std::string const & attrname, std::string const & value)
{
	cerr << " " << attrname << " = " << value << endl;
	if(attrname == "architecture")
	{
		architecture = value;
	}
	else if(attrname == "abiversion")
	{
		abiversion = value;
	}
	else if(attrname == "modname")
	{
		modname = value;
	}
	else
	{
		throw ParsingException("Unknown in-line field : ", attrname);
	}
}

#endif
