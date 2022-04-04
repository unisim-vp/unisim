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
DWARF_SubProgram<ADDRESS>::DWARF_SubProgram(const DWARF_DIE<ADDRESS> *_dw_subprogram_die)
	: unisim::util::debug::SubProgram<ADDRESS>()
	, dw_handler(_dw_subprogram_die->GetHandler())
	, dw_subprogram_die(_dw_subprogram_die)
	, debug_info_stream(dw_handler->GetDebugInfoStream())
	, debug_warning_stream(dw_handler->GetDebugWarningStream())
	, debug_error_stream(dw_handler->GetDebugErrorStream())
	, debug(dw_handler->GetOptionFlag(OPT_DEBUG))
	, name()
	, external_flag(false)
	, declaration_flag(false)
	, inline_code(0)
	, dw_return_type_die(0)
	, return_type(0)
	, formal_params()
	, decl_location(0)
	, addr(0)
{
	const char *_name = dw_subprogram_die->GetName();
	name = _name ? _name : "";
	external_flag = dw_subprogram_die->GetExternalFlag(external_flag) && external_flag;
	declaration_flag = dw_subprogram_die->GetDeclarationFlag(declaration_flag) && declaration_flag;
	dw_subprogram_die->GetInlineCode(inline_code);
	dw_return_type_die = dw_subprogram_die->GetTypeDIE();
	return_type = dw_return_type_die ? dw_return_type_die->GetType(0) : new unisim::util::debug::UnspecifiedType();
	decl_location = dw_subprogram_die->GetDeclLocation();
	
	const DWARF_DIE<ADDRESS> *dw_at_abstract_origin = dw_subprogram_die->GetAbstractOrigin();
	
	const std::vector<DWARF_DIE<ADDRESS> *>& dw_children = dw_at_abstract_origin ? dw_at_abstract_origin->GetChildren() : dw_subprogram_die->GetChildren();
	unsigned int num_children = dw_children.size();
	unsigned int i;
	for(i = 0; i < num_children; i++)
	{
		const DWARF_DIE<ADDRESS> *child = dw_children[i];
		
		if(child->GetTag() == DW_TAG_formal_parameter)
		{
			const char *formal_param_name = child->GetName();
			const DWARF_DIE<ADDRESS> *dw_die_formal_param_type = child->GetTypeDIE();
			
			formal_params.push_back(new unisim::util::debug::FormalParameter(formal_param_name ? formal_param_name :  "", dw_die_formal_param_type ? dw_die_formal_param_type->GetType(0) : new unisim::util::debug::UnspecifiedType()));
		}
	}
	
	const unisim::util::debug::Symbol<ADDRESS> *symbol = 0;
	if(external_flag && _name)
	{
		const unisim::util::debug::SymbolTable<ADDRESS> *symbol_table = dw_handler->GetSymbolTable();
		if(symbol_table)
		{
			symbol = symbol_table->FindSymbolByName(_name, unisim::util::debug::Symbol<ADDRESS>::SYM_FUNC);
		}
	}
	
	if(symbol)
	{
		addr = symbol->GetAddress();
	}
	else
	{
		dw_subprogram_die->GetLowPC(addr);
	}
	
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

template <class ADDRESS>
ADDRESS DWARF_SubProgram<ADDRESS>::GetAddress() const
{
	return addr;
}

template <class ADDRESS>
const DWARF_DIE<ADDRESS> *DWARF_SubProgram<ADDRESS>::GetReturnTypeDIE() const
{
	return dw_return_type_die;
}

template <class ADDRESS>
bool DWARF_SubProgram<ADDRESS>::GetReturnValueLocation(const DWARF_MachineState<ADDRESS> *dw_mach_state, unsigned int prc_num, DWARF_Location<ADDRESS>& loc) const
{
	const unisim::util::debug::Type *resolved_returned_type = unisim::util::debug::TypeResolver::Resolve(return_type);
	
	if(resolved_returned_type->IsUnspecified()) return false;
	
	const DWARF_Frame<ADDRESS> *dw_curr_frame = dw_mach_state->GetCurrentFrame(prc_num);
	const DWARF_CompilationUnit<ADDRESS> *dw_cu = dw_subprogram_die->GetCompilationUnit();
	std::string architecture = dw_mach_state->GetArchitecture(prc_num);
	DWARF_Expression<ADDRESS> *dw_loc_expr = 0;
	
	if(architecture == "powerpc")
	{
		if(resolved_returned_type->IsFloat())
		{
			uint8_t expr_value[] = { DW_OP_regx, 32 }; // f1
			dw_loc_expr = new DWARF_Expression<ADDRESS>(dw_cu, sizeof(expr_value), expr_value);
		}
		else
		{
			uint64_t dw_return_value_bit_size = 0;
			if(dw_return_type_die->GetBitSize(dw_curr_frame, dw_return_value_bit_size))
			{
				if(dw_return_value_bit_size <= 32)
				{
					uint8_t expr_value[] = { DW_OP_reg3 }; // r3
					dw_loc_expr = new DWARF_Expression<ADDRESS>(dw_cu, sizeof(expr_value), expr_value);
				}
				else if(dw_return_value_bit_size <= 64)
				{
					uint8_t expr_value[] = { DW_OP_reg3, DW_OP_piece, 4, DW_OP_reg4, DW_OP_piece, 4 }; // r3, r4
					dw_loc_expr = new DWARF_Expression<ADDRESS>(dw_cu, sizeof(expr_value), expr_value);
				}
				else
				{
					uint8_t expr_value[] = { DW_OP_breg3, 0 }; // @r3
					dw_loc_expr = new DWARF_Expression<ADDRESS>(dw_cu, sizeof(expr_value), expr_value);
				}
			}
		}
	}
	else if(architecture == "powerpc-e500")
	{
		uint64_t dw_return_value_bit_size = 0;
		if(dw_return_type_die->GetBitSize(dw_curr_frame, dw_return_value_bit_size))
		{
			if(dw_return_value_bit_size <= 32)
			{
				uint8_t expr_value[] = { DW_OP_reg3 }; // r3
				dw_loc_expr = new DWARF_Expression<ADDRESS>(dw_cu, sizeof(expr_value), expr_value);
			}
			else if(dw_return_value_bit_size <= 64)
			{
				uint8_t expr_value[] = { DW_OP_reg3, DW_OP_piece, 4, DW_OP_reg4, DW_OP_piece, 4 }; // r3, r4
				dw_loc_expr = new DWARF_Expression<ADDRESS>(dw_cu, sizeof(expr_value), expr_value);
			}
			else
			{
				uint8_t expr_value[] = { DW_OP_breg3, 0 }; // @r3
				dw_loc_expr = new DWARF_Expression<ADDRESS>(dw_cu, sizeof(expr_value), expr_value);
			}
		}
	}
	else if(architecture == "arm")
	{
		uint64_t dw_return_value_bit_size = 0;
		if(dw_return_type_die->GetBitSize(dw_curr_frame, dw_return_value_bit_size))
		{
			if(resolved_returned_type->IsBase())
			{
				if(dw_return_value_bit_size <= 32)
				{
					// A Half-precision Floating Point Type is returned in the least significant 16 bits of r0
					// A Fundamental Data Type that is smaller than 4 bytes is zero- or sign-extended to a word and returned in r0
					// A word-sized Fundamental Data Type (e.g., int, float) is returned in r0
					uint8_t expr_value[] = { DW_OP_reg0 }; // r0
					dw_loc_expr = new DWARF_Expression<ADDRESS>(dw_cu, sizeof(expr_value), expr_value);
				}
				else if(dw_return_value_bit_size <= 64)
				{
					// A double-word sized Fundamental Data Type (e.g., long long, double and 64-bit containerized vectors) is returned in r0 and r1
					uint8_t expr_value[] = { DW_OP_reg0, DW_OP_piece, 4, DW_OP_reg1, DW_OP_piece, 4 }; // r0, r1
					dw_loc_expr = new DWARF_Expression<ADDRESS>(dw_cu, sizeof(expr_value), expr_value);
				}
			}
			else if(resolved_returned_type->IsComposite())
			{
				// A Composite Type not larger than 4 bytes is returned in r0. The format is as if the result had been stored in memory at a word-aligned address and then loaded into r0 with an LDR instruction. Any bits in r0 that lie outside the bounds of the result have unspecified values
				uint8_t expr_value[] = { DW_OP_breg0, 0 }; // @r0
				dw_loc_expr = new DWARF_Expression<ADDRESS>(dw_cu, sizeof(expr_value), expr_value);
			}
		}
	}
	else if(architecture == "aarch64")
	{
		if(resolved_returned_type->IsFloat())
		{
			uint8_t expr_value[] = { DW_OP_regx, 64 }; // v0
			dw_loc_expr = new DWARF_Expression<ADDRESS>(dw_cu, sizeof(expr_value), expr_value);
		}
		else
		{
			uint64_t dw_return_value_bit_size = 0;
			if(dw_return_type_die->GetBitSize(dw_curr_frame, dw_return_value_bit_size))
			{
				if(dw_return_value_bit_size <= 64)
				{
					uint8_t expr_value[] = { DW_OP_reg0 }; // x0
					dw_loc_expr = new DWARF_Expression<ADDRESS>(dw_cu, sizeof(expr_value), expr_value);
				}
				else if(dw_return_value_bit_size <= 128)
				{
					uint8_t expr_value[] = { DW_OP_reg0, DW_OP_piece, 8, DW_OP_reg1, DW_OP_piece, 8 }; // x0, x1
					dw_loc_expr = new DWARF_Expression<ADDRESS>(dw_cu, sizeof(expr_value), expr_value);
				}
				else
				{
					uint8_t expr_value[] = { DW_OP_breg8, 0 }; // @x8
					dw_loc_expr = new DWARF_Expression<ADDRESS>(dw_cu, sizeof(expr_value), expr_value);
				}
			}
		}
	}
	else if(architecture == "sparc")
	{
		// see http://icps.u-strasbg.fr/people/loechner/public_html/enseignement/SPARC/sparcstack.html
		//
		//             %g0  (r00)       always zero
		//             %g1  (r01)  [1]  temporary value
		//             %g2  (r02)  [2]  global 2
		// global      %g3  (r03)  [2]  global 3
		//             %g4  (r04)  [2]  global 4
		//             %g5  (r05)       reserved for SPARC ABI
		//             %g6  (r06)       reserved for SPARC ABI
		//             %g7  (r07)       reserved for SPARC ABI
		//
		//             %o0  (r08)  [3]  outgoing parameter 0 / return value from callee   
		//             %o1  (r09)  [1]  outgoing parameter 1
		//             %o2  (r10)  [1]  outgoing parameter 2
		// out         %o3  (r11)  [1]  outgoing parameter 3
		//             %o4  (r12)  [1]  outgoing parameter 4
		//             %o5  (r13)  [1]  outgoing parameter 5
		//        %sp, %o6  (r14)  [1]  stack pointer
		//             %o7  (r15)  [1]  temporary value / address of CALL instruction
		//
		//             %l0  (r16)  [3]  local 0
		//             %l1  (r17)  [3]  local 1
		//             %l2  (r18)  [3]  local 2
		// local       %l3  (r19)  [3]  local 3
		//             %l4  (r20)  [3]  local 4
		//             %l5  (r21)  [3]  local 5
		//             %l6  (r22)  [3]  local 6
		//             %l7  (r23)  [3]  local 7
		//
		//             %i0  (r24)  [3]  incoming parameter 0 / return value to caller
		//             %i1  (r25)  [3]  incoming parameter 1
		//             %i2  (r26)  [3]  incoming parameter 2
		// in          %i3  (r27)  [3]  incoming parameter 3
		//             %i4  (r28)  [3]  incoming parameter 4
		//             %i5  (r29)  [3]  incoming parameter 5
		//        %fp, %i6  (r30)  [3]  frame pointer
		//             %i7  (r31)  [3]  return address - 8
		//
		//            Figure 2 - Sparc register semantics
		//
		//---------------------------------------------------------------------------------
		//
		//                 high addresses
		//
		//            +-------------------------+         
		//  %sp  -->  | 16 words for storing    |
		//            | LOCAL and IN registers  |
		//            +-------------------------+
		//            |  one-word pointer to    |
		//            | aggregate return value  |
		//            +-------------------------+
		//            |   6 words for callee    |
		//            |   to store register     |
		//            |       arguments         |
		//            +-------------------------+
		//            |  outgoing parameters    |
		//            |  past the 6th, if any   |
		//            +-------------------------+
		//            |  space, if needed, for  |
		//            |  compiler temporaries   |
		//            |   and saved floating-   |
		//            |    point registers      |
		//            +-------------------------+
		//
		//            +-------------------------+
		//            |    space dynamically    |
		//            |    allocated via the    |
		//            |  alloca() library call  |
		//            +-------------------------+
		//            |  space, if needed, for  |
		//            |    automatic arrays,    |
		//            |    aggregates, and      |
		//            |   addressable scalar    |
		//            |       automatics        |
		//            +-------------------------+
		// %fp  -->
		//                  low addresses
		//
		//           Figure 3 - Stack frame contents
		//
		uint64_t dw_return_value_bit_size = 0;
		if(dw_return_type_die->GetBitSize(dw_curr_frame, dw_return_value_bit_size))
		{
			if(dw_return_value_bit_size <= 32)
			{
				uint8_t expr_value[] = { DW_OP_reg8 }; // %o0
				dw_loc_expr = new DWARF_Expression<ADDRESS>(dw_cu, sizeof(expr_value), expr_value);
			}
			else
			{
				uint8_t expr_value[] = { DW_OP_breg14, 64, DW_OP_deref  }; // @(@(%sp + 64))    
				dw_loc_expr = new DWARF_Expression<ADDRESS>(dw_cu, sizeof(expr_value), expr_value);
			}
		}
	}
	
	if(!dw_loc_expr) return false;
	
	ADDRESS addr;
	DWARF_ExpressionVM<ADDRESS> dw_loc_expr_vm = DWARF_ExpressionVM<ADDRESS>(dw_handler, dw_curr_frame);
	bool dw_loc_expr_vm_status = dw_loc_expr_vm.Execute(dw_loc_expr, addr, &loc);
	delete dw_loc_expr;
	if(!dw_loc_expr_vm_status)
	{
		if(debug)
		{
			debug_info_stream << "In File \"" << dw_handler->GetFilename() << "\", evaluation of \"" << name << "\" subprogram return value location expression failed" << std::endl;
		}
	}
	
	// Determine the size in bytes (including padding bits)
	uint64_t dw_byte_size;
	if(!dw_return_type_die->GetByteSize(dw_curr_frame, dw_byte_size))
	{
		if(debug)
		{
			debug_info_stream << "In File \"" << dw_handler->GetFilename() << "\", can't determine byte size (with padding) of \"" << name << "\" subprogram return value" << std::endl;
		}
	}
	loc.SetByteSize(dw_byte_size);
	
	// Determine the actual size in bits (excluding padding bits)
	uint64_t dw_bit_size = 0;
	if(!dw_return_type_die->GetBitSize(dw_curr_frame, dw_bit_size))
	{
		if(debug)
		{
			debug_info_stream << "In File \"" << dw_handler->GetFilename() << "\", can't determine bit size of \"" << name << "\" subprogram return value" << std::endl;
		}
	}
	loc.SetBitSize(dw_bit_size);

	uint8_t dw_at_encoding = 0;
	
	if(!dw_return_type_die->GetEncoding(dw_at_encoding))
	{
		dw_at_encoding = 0;
	}
	loc.SetEncoding(dw_at_encoding);
	
	return true;
}

} // end of namespace dwarf
} // end of namespace debug
} // end of namespace util
} // end of namespace unisim

#endif // __UNISIM_UTIL_DEBUG_DWARF_SUBPROGRAM_TCC__
