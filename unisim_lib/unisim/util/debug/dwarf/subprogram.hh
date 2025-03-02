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

#ifndef __UNISIM_UTIL_DEBUG_DWARF_SUBPROGRAM_HH__
#define __UNISIM_UTIL_DEBUG_DWARF_SUBPROGRAM_HH__

#include <unisim/util/debug/dwarf/fwd.hh>
#include <unisim/util/debug/subprogram.hh>
#include <cstdint>
#include <string>
#include <vector>
#include <iosfwd>

namespace unisim {
namespace util {
namespace debug {
namespace dwarf {

template <class ADDRESS>
class DWARF_FormalParameter : public unisim::util::debug::FormalParameter
{
public:
	DWARF_FormalParameter(const char *name, const Type *type, const DWARF_DIE<ADDRESS> *dw_die);
	const DWARF_DIE<ADDRESS> *GetDIE() const;
private:
	const DWARF_DIE<ADDRESS> *dw_die;
};

template <class ADDRESS>
class DWARF_SubProgram : public unisim::util::debug::SubProgram<ADDRESS>
{
public:
	DWARF_SubProgram(const DWARF_DIE<ADDRESS> *dw_subprogram_die);
	virtual ~DWARF_SubProgram();
	
	virtual const char *GetName() const;
	virtual const char *GetFilename() const;
	virtual bool IsExternal() const;
	virtual bool IsDeclaration() const;
	virtual bool IsInline() const;
	virtual bool IsInlined() const;
	virtual const Type *GetReturnType() const;
	virtual unsigned int GetArity() const;
	virtual const unisim::util::debug::FormalParameter *GetFormalParameter(unsigned int idx) const;
	virtual const unisim::util::debug::DeclLocation *GetDeclLocation() const;
	virtual ADDRESS GetAddress() const;
	
	const DWARF_DIE<ADDRESS> *GetReturnTypeDIE() const;
	const DWARF_DIE<ADDRESS> *GetFormalParameterDIE(unsigned int idx) const;
	bool GetReturnValueLocation(const DWARF_MachineState<ADDRESS> *dw_mach_state, unsigned int prc_num, DWARF_Location<ADDRESS>& loc) const;
private:
	const DWARF_Handler<ADDRESS> *dw_handler;
	const DWARF_DIE<ADDRESS> *dw_subprogram_die;
	const bool& debug;
	std::string name;
	bool external_flag;
	bool declaration_flag;
	uint8_t inline_code;
	const DWARF_DIE<ADDRESS> *dw_return_type_die;
	const Type *return_type;
	std::vector<const DWARF_FormalParameter<ADDRESS> *> formal_params;
	const unisim::util::debug::DeclLocation *decl_location;
	ADDRESS addr;
};

} // end of namespace dwarf
} // end of namespace debug
} // end of namespace util
} // end of namespace unisim

#endif // __UNISIM_UTIL_DEBUG_DWARF_SUBPROGRAM_HH__
