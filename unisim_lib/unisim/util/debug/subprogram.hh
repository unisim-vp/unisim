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

#ifndef __UNISIM_UTIL_DEBUG_SUBPROGRAM_HH__
#define __UNISIM_UTIL_DEBUG_SUBPROGRAM_HH__

#include <unisim/util/debug/type.hh>
#include <unisim/util/debug/decl_location.hh>
#include <string>

namespace unisim {
namespace util {
namespace debug {

template <class ADDRESS> class SubProgram;

template <class ADDRESS> bool operator <  (SubProgram<ADDRESS> const& sp1, SubProgram<ADDRESS> const& sp2);
template <class ADDRESS> bool operator <= (SubProgram<ADDRESS> const& sp1, SubProgram<ADDRESS> const& sp2);
template <class ADDRESS> bool operator == (SubProgram<ADDRESS> const& sp1, SubProgram<ADDRESS> const& sp2);
template <class ADDRESS> bool operator != (SubProgram<ADDRESS> const& sp1, SubProgram<ADDRESS> const& sp2);
template <class ADDRESS> bool operator >= (SubProgram<ADDRESS> const& sp1, SubProgram<ADDRESS> const& sp2);
template <class ADDRESS> bool operator >  (SubProgram<ADDRESS> const& sp1, SubProgram<ADDRESS> const& sp2);

template <class ADDRESS>
class SubProgram
{
public:
	SubProgram();
	virtual ~SubProgram();
	virtual const char *GetName() const = 0;
	virtual const char *GetFilename() const = 0;
	virtual bool IsExternal() const = 0;
	virtual bool IsDeclaration() const = 0;
	virtual bool IsInline() const = 0;
	virtual bool IsInlined() const = 0;
	virtual const Type *GetReturnType() const = 0;
	virtual unsigned int GetArity() const = 0;
	virtual const FormalParameter *GetFormalParameter(unsigned int idx) const = 0;
	virtual const DeclLocation *GetDeclLocation() const = 0;
	virtual ADDRESS GetAddress() const = 0;
	const std::string& GetCDecl(bool no_array_subscripts = false) const;
	void Catch() const;
	void Release() const;
	template <typename VISITOR, typename T = bool> T Scan(VISITOR& visitor) const;
private:
	mutable unsigned int ref_count;
	mutable std::string *c_decl[2];
};

} // end of namespace debug
} // end of namespace util
} // end of namespace unisim

#endif // __UNISIM_UTIL_DEBUG_SUBPROGRAM_HH__
