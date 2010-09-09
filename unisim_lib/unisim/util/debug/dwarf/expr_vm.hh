/*
 *  Copyright (c) 2010,
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

#ifndef __UNISIM_UTIL_DEBUG_DWARF_EXPR_VM_HH__
#define __UNISIM_UTIL_DEBUG_DWARF_EXPR_VM_HH__

#include <unisim/util/debug/dwarf/fwd.hh>
#include <unisim/util/debug/register.hh>

namespace unisim {
namespace util {
namespace debug {
namespace dwarf {

const unsigned int DW_LOC_REGISTER = 1;
const unsigned int DW_LOC_MEMORY = 2;

template <class MEMORY_ADDR>
class DWARF_LocationPiece
{
public:
	DWARF_LocationPiece(unsigned int dw_loc_type);
	~DWARF_LocationPiece();
	unsigned int GetType() const;
private:
	unsigned int dw_loc_type;
};

template <class MEMORY_ADDR>
class DWARF_MemoryLocationPiece : public DWARF_LocationPiece<MEMORY_ADDR>
{
public:
	DWARF_MemoryLocationPiece(MEMORY_ADDR dw_addr, unsigned int dw_bit_offset, unsigned int dw_bit_size);
	~DWARF_MemoryLocationPiece();
	MEMORY_ADDR GetAddress() const;
	unsigned int GetBitOffset() const;
	unsigned int GetBitSize() const;
private:
	MEMORY_ADDR dw_addr;
	unsigned int dw_bit_offset;
	unsigned int dw_bit_size;
};

template <class MEMORY_ADDR>
class DWARF_RegisterLocationPiece : public DWARF_LocationPiece<MEMORY_ADDR>
{
public:
	DWARF_RegisterLocationPiece(unisim::util::debug::Register *dw_reg, unsigned int dw_bit_offset, unsigned int dw_bit_size);
	~DWARF_RegisterLocationPiece();
	unsigned int GetRegisterNumber() const;
	unsigned int GetBitOffset() const;
	unsigned int GetBitSize() const;
private:
	unisim::util::debug::Register *dw_reg;
	unsigned int dw_bit_offset;
	unsigned int dw_bit_size;
};

template <class MEMORY_ADDR>
class DWARF_Location
{
public:
	DWARF_Location();
	~DWARF_Location();
	void Add(DWARF_LocationPiece<MEMORY_ADDR> *dw_loc_piece);
	const std::vector<DWARF_LocationPiece<MEMORY_ADDR> *>& GetLocationPieces() const;
private:
	std::vector<DWARF_LocationPiece<MEMORY_ADDR> *> dw_location_pieces;
};

template <class MEMORY_ADDR>
class DWARF_ExpressionVM
{
public:
	DWARF_ExpressionVM(const DWARF_CompilationUnit<MEMORY_ADDR> *dw_cu);
	DWARF_ExpressionVM(const DWARF_CallFrameProgram<MEMORY_ADDR> *dw_cfp);
	~DWARF_ExpressionVM();
	
	bool Disasm(std::ostream& os, const DWARF_Expression<MEMORY_ADDR> *dw_expr);
	bool Execute(const DWARF_Expression<MEMORY_ADDR> *dw_expr, DWARF_Location<MEMORY_ADDR> *dw_location);
private:
	const DWARF_CompilationUnit<MEMORY_ADDR> *dw_cu;
	const DWARF_CallFrameProgram<MEMORY_ADDR> *dw_cfp;
	unisim::util::debug::Register *registers[32];

	bool Run(const DWARF_Expression<MEMORY_ADDR> *dw_expr, std::ostream *os, DWARF_Location<MEMORY_ADDR> *dw_location);

};

} // end of namespace dwarf
} // end of namespace debug
} // end of namespace util
} // end of namespace unisim

#endif
