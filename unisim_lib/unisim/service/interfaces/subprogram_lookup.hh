/*
 *  Copyright (c) 2012,
 *  Commissariat a l'Energie Atomique (CEA)
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
 *   - Neither the name of CEA nor the names of its contributors may be used to
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
 * Authors: Gilles Mouchard (gilles.mouchard@cea.fr)
 */
 
#ifndef __UNISIM_SERVICE_INTERFACES_SUBPROGRAM_LOOKUP_HH__
#define __UNISIM_SERVICE_INTERFACES_SUBPROGRAM_LOOKUP_HH__

#include <unisim/service/interfaces/interface.hh>
#include <unisim/util/debug/subprogram.hh>
#include <set>
#include <string>

namespace unisim {
namespace service {
namespace interfaces {

template <class ADDRESS>
class SubProgramLookup : public ServiceInterface
{
public:
	//virtual void EnumerateSubProgramNames(std::set<std::string>& name_set) const = 0;
	virtual const unisim::util::debug::SubProgram<ADDRESS> *FindSubProgram(const char *subprogram_name, const char *filename = 0, const char *compilation_unit_name = 0) const = 0;
	virtual const unisim::util::debug::SubProgram<ADDRESS> *FindSubProgram(ADDRESS pc, const char *filename = 0) const = 0;
	const unisim::util::debug::SubProgram<ADDRESS> *FindSubProgram(const std::string& subprogram_name, const std::string& filename, const std::string& compilation_unit_name = 0) const { return FindSubProgram(subprogram_name.c_str(), filename.length() ? filename.c_str() : 0, compilation_unit_name.length() ? compilation_unit_name.c_str() : 0); }
	const unisim::util::debug::SubProgram<ADDRESS> *FindSubProgram(ADDRESS pc, const std::string& filename = 0) const { return FindSubProgram(pc, filename.length() ? filename.c_str() : 0); };
};

} // end of namespace interfaces
} // end of namespace service
} // end of namespace unisim

#endif // __UNISIM_SERVICE_INTERFACES_SUBPROGRAM_LOOKUP_HH__
