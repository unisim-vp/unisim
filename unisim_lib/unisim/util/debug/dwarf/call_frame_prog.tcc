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

namespace unisim {
namespace util {
namespace debug {
namespace dwarf {

template <class MEMORY_ADDR>
DWARF_CallFrameProgram<MEMORY_ADDR>::DWARF_CallFrameProgram(DWARF_Handler<MEMORY_ADDR> *_dw_handler, uint64_t _length, const uint8_t *_program)
	: dw_handler(_dw_handler)
	, length(_length)
	, program(_program)
{
}

template <class MEMORY_ADDR>
DWARF_CallFrameProgram<MEMORY_ADDR>::~DWARF_CallFrameProgram()
{
}

template <class MEMORY_ADDR>
endian_type DWARF_CallFrameProgram<MEMORY_ADDR>::GetEndianness() const
{
	return dw_handler->GetEndianness();
}

template <class MEMORY_ADDR>
uint8_t DWARF_CallFrameProgram<MEMORY_ADDR>::GetAddressSize() const
{
	return dw_handler->GetAddressSize();
}


template <class MEMORY_ADDR>
uint64_t DWARF_CallFrameProgram<MEMORY_ADDR>::GetLength() const
{
	return length;
}

template <class MEMORY_ADDR>
const uint8_t *DWARF_CallFrameProgram<MEMORY_ADDR>::GetProgram() const
{
	return program;
}

template <class MEMORY_ADDR>
std::ostream& operator << (std::ostream& os, const DWARF_CallFrameProgram<MEMORY_ADDR>& dw_call_frame_prog)
{
	DWARF_CallFrameVM<MEMORY_ADDR> dw_call_frame_vm;
	if(!dw_call_frame_vm.Disasm(os, dw_call_frame_prog))
	{
		os << "Invalid opcode";
	}
	return os;
}

} // end of namespace dwarf
} // end of namespace debug
} // end of namespace util
} // end of namespace unisim