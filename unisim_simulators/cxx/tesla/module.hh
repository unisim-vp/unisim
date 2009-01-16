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
 
#ifndef SIMULATOR_CXX_TESLA_MODULE_HH
#define SIMULATOR_CXX_TESLA_MODULE_HH

#include <string>
#include <iosfwd>
#include <vector>
#include <list>
#include <map>
#include <unisim/kernel/service/service.hh>
#include <unisim/service/interfaces/memory.hh>

#include "kernel.hh"

using unisim::kernel::service::Service;
using unisim::service::interfaces::Memory;

struct ParsingException
{
	ParsingException();
	ParsingException(char const * str);
	ParsingException(char const * str, std::string const & param);

	friend std::ostream & operator<< (std::ostream & os, ParsingException const & pe);
private:
	char const * str;
	std::string param;
};

std::ostream & operator<< (std::ostream & os, ParsingException const & pe);

template<class CONFIG>
struct ConstSeg
{
	ConstSeg(std::istream & is);
	ConstSeg();
	uint32_t Size() const;
	void Load(Service<Memory<typename CONFIG::address_t> > & mem) const;
	std::string const & Name() const;
	typename CONFIG::address_t Address() const;
	
private:
	void SetAttribute(std::string const & name, std::string const & value);

	std::string name;
	std::string segname;
	int segnum;
	uint32_t offset;
	uint32_t bytes;
	std::vector<uint32_t> mem;
};


template<class CONFIG>
struct Module : CUmod_st
{
	Module(char const * fname);
	
	Kernel<CONFIG> & GetKernel(char const * name);
	ConstSeg<CONFIG> & GetConstant(char const * name);

private:
	void LoadCubin(char const * fname);
	void SetField(std::string const & name, std::string const & value);

	std::string architecture;
	std::string abiversion;
	std::string modname;
	
	typedef std::map<std::string, Kernel<CONFIG> > KernelMap;
	KernelMap kernels;

	typedef std::map<std::string, ConstSeg<CONFIG> > ConstMap;
	ConstMap global_constants;

//	std::list<CUfunction> functions;
	
};

inline string Strip(string s);
inline void ParseBinCode(vector<uint32_t> & v, istream & is);


#endif

