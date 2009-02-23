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

#include "kernel.tcc"
#include "fatformat.hh"

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
	std::cerr << str << std::endl;
	assert(false);
}

ParsingException::ParsingException(char const * str, std::string const & param) :
	str(str), param(param)
{
	std::cerr << str << param<< std::endl;
	assert(false);
}

std::ostream & operator<< (std::ostream & os, ParsingException const & pe)
{
	os << pe.str;
	os << pe.param;
	return os;
}

template<class CONFIG>
MemSegment<CONFIG>::MemSegment(std::istream & is, SegmentType st) :
	type(st)
{
	if(st == SegmentConst) {
		cerr << " Constant segment\n";
	}
	else {
		cerr << " Reloc segment\n";
	}
	
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
MemSegment<CONFIG>::MemSegment()
{
}

template<class CONFIG>
uint32_t MemSegment<CONFIG>::Size() const
{
	assert(mem.size() == 0 || 4 * bytes == mem.size());
	return bytes;
}

template<class CONFIG>
void MemSegment<CONFIG>::SetAttribute(std::string const & attrname, std::string const & value)
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
void MemSegment<CONFIG>::Load(Service<Memory<typename CONFIG::address_t> > & memory) const
{
	if(!memory.WriteMemory(Address(), &mem[0], mem.size() * 4)) {
		throw CudaException(CUDA_ERROR_OUT_OF_MEMORY);	// Generic memory error
	}
}

template<class CONFIG>
std::string const & MemSegment<CONFIG>::Name() const
{
	return name;
}

template<class CONFIG>
typename CONFIG::address_t MemSegment<CONFIG>::Address() const
{
	if(type == SegmentConst) {
		return CONFIG::CONST_START
			+ segnum * CONFIG::CONST_SEG_SIZE
			+ offset;
	}
	else {
		assert(segnum == 14);
		return CONFIG::GLOBAL_RELOC_START
	//		+ segnum * CONFIG::RELOC_SEG_SIZE
			+ offset * 1024;
	}
}

//////////////////////////////////////////////////////////////////////


template<class CONFIG>
Module<CONFIG>::Module(char const * fname)
{
	cerr << "Cubin " << fname << endl;
	ifstream is(fname, ifstream::in);
	LoadCubin(is);
}

template<class CONFIG>
Module<CONFIG>::Module(void const *fatCubin, int)
{
	FatFormat ff(fatCubin);
	ff.Dump();
	char const * cubin = ff.GetCubin(0);
	istringstream is(cubin);
	LoadCubin(is);
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
MemSegment<CONFIG> & Module<CONFIG>::GetGlobal(char const * name)
{
	typename SegMap::iterator it = global_segments.find(string(name));
	if(it == global_segments.end()) {
		throw CudaException(CUDA_ERROR_NOT_FOUND);
	}
	return it->second;
}

template<class CONFIG>
void Module<CONFIG>::LoadCubin(istream & is)
{
	// Mostly adapted from Decuda
	typedef string::iterator it_t;
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
						//global_constants.push_back(MemSegment<CONFIG>(is));
						MemSegment<CONFIG> cs(is, SegmentConst);
						if(cs.Name().empty()) {
							throw ParsingException("Unnamed code field");
						}
						global_segments[cs.Name()] = cs;
					}
					else if(cmd == "samplers")
					{
						// ignore for now
						cerr << "Warning: skipping section: samplers\n";
					}
					else if(cmd == "reloc")
					{
						MemSegment<CONFIG> cs(is, SegmentReloc);
						if(cs.Name().empty()) {
							throw ParsingException("Unnamed code field");
						}
						global_segments[cs.Name()] = cs;
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
