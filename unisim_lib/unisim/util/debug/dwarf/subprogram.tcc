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

#ifndef __UNISIM_UTIL_DEBUG_DWARF_SUBPROGRAM_TCC__
#define __UNISIM_UTIL_DEBUG_DWARF_SUBPROGRAM_TCC__

#include <unisim/util/debug/subprogram.tcc>

namespace unisim {
namespace util {
namespace debug {
namespace dwarf {

template <class ADDRESS>
DWARF_SubProgram<ADDRESS>::DWARF_SubProgram(char const *_name, bool _external_flag, bool _declaration_flag, uint8_t _inline_code, const unisim::util::debug::Type *_return_type, const unisim::util::debug::DeclLocation *_decl_location)
	: unisim::util::debug::SubProgram<ADDRESS>()
	, name(_name ? _name : "")
	, external_flag(_external_flag)
	, declaration_flag(_declaration_flag)
	, inline_code(_inline_code)
	, return_type(_return_type)
	, formal_params()
	, decl_location(_decl_location)
{
	if(return_type) return_type->Catch();
	if(decl_location) decl_location->Catch();
}

template <class ADDRESS>
DWARF_SubProgram<ADDRESS>::~DWARF_SubProgram()
{
	if(return_type) return_type->Release();
	if(decl_location) decl_location->Release();
	
	unsigned int arity = formal_params.size();
	unsigned int idx;
	
	for(idx = 0; idx < arity; idx++)
	{
		delete formal_params[idx];
	}
}

template <class ADDRESS>
void DWARF_SubProgram<ADDRESS>::AddFormalParameter(const FormalParameter *formal_param)
{
	formal_params.push_back(formal_param);
}

template <class ADDRESS>
const char *DWARF_SubProgram<ADDRESS>::GetName() const
{
	return name.c_str();
}

template <class ADDRESS>
bool DWARF_SubProgram<ADDRESS>::IsExternal() const
{
	return external_flag;
}

template <class ADDRESS>
bool DWARF_SubProgram<ADDRESS>::IsDeclaration() const
{
	return declaration_flag;
}

template <class ADDRESS>
bool DWARF_SubProgram<ADDRESS>::IsInline() const
{
	return (inline_code == DW_INL_declared_inlined) || (inline_code == DW_INL_declared_not_inlined);
}

template <class ADDRESS>
bool DWARF_SubProgram<ADDRESS>::IsInlined() const
{
	return (inline_code == DW_INL_inlined) || (inline_code == DW_INL_declared_inlined);
}

template <class ADDRESS>
const Type *DWARF_SubProgram<ADDRESS>::GetReturnType() const
{
	return return_type;
}

template <class ADDRESS>
unsigned int DWARF_SubProgram<ADDRESS>::GetArity() const
{
	return formal_params.size();
}

template <class ADDRESS>
const FormalParameter *DWARF_SubProgram<ADDRESS>::GetFormalParameter(unsigned int idx) const
{
	if(idx >= formal_params.size()) return 0;
	return formal_params[idx];
}

template <class ADDRESS>
const unisim::util::debug::DeclLocation *DWARF_SubProgram<ADDRESS>::GetDeclLocation() const
{
	return decl_location;
}

} // end of namespace dwarf
} // end of namespace debug
} // end of namespace util
} // end of namespace unisim

#endif // __UNISIM_UTIL_DEBUG_DWARF_SUBPROGRAM_TCC__
