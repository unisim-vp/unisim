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

#ifndef __UNISIM_UTIL_DEBUG_DWARF_UNWIND_CONTEXT_HH__
#define __UNISIM_UTIL_DEBUG_DWARF_UNWIND_CONTEXT_HH__

#include <unisim/util/debug/dwarf/fwd.hh>
#include <unisim/kernel/logger/logger.hh>
#include <unisim/service/interfaces/registers.hh>
#include <unisim/service/interfaces/memory.hh>
#include <unisim/util/endian/endian.hh>
#include <iosfwd>

namespace unisim {
namespace util {
namespace debug {
namespace dwarf {

template <class MEMORY_ADDR>
std::ostream& operator << (std::ostream& os, const DWARF_UnwindContext<MEMORY_ADDR>& dw_reg_set);

template <class MEMORY_ADDR>
class DWARF_UnwindContext
{
public:
	DWARF_UnwindContext(unisim::util::endian::endian_type endianness, unsigned int address_size, unisim::service::interfaces::Memory<MEMORY_ADDR> *mem_if);
	~DWARF_UnwindContext();
	
	bool Load(const DWARF_RegisterNumberMapping *reg_num_mapping);
	bool Compute(const DWARF_CFIRow<MEMORY_ADDR> *cfi_row);
	MEMORY_ADDR ReadRegister(unsigned int reg_num) const;
	void WriteRegister(unsigned int reg_num, const MEMORY_ADDR value);
	
	friend std::ostream& operator << <MEMORY_ADDR>(std::ostream& os, const DWARF_UnwindContext<MEMORY_ADDR>& dw_reg_set);
private:
	unisim::util::endian::endian_type endianness;
	unsigned int address_size;
	unisim::service::interfaces::Memory<MEMORY_ADDR> *mem_if;
	MEMORY_ADDR cfa;
	std::map<unsigned int, MEMORY_ADDR> reg_set;
	
	bool ReadAddrFromMemory(MEMORY_ADDR addr, MEMORY_ADDR& read_addr) const;
};

} // end of namespace dwarf
} // end of namespace debug
} // end of namespace util
} // end of namespace unisim

#endif
