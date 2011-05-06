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

#ifndef __UNISIM_UTIL_DEBUG_DWARF_DIE_HH__
#define __UNISIM_UTIL_DEBUG_DWARF_DIE_HH__

#include <unisim/util/debug/dwarf/fwd.hh>
#include <unisim/util/debug/stmt.hh>

namespace unisim {
namespace util {
namespace debug {
namespace dwarf {

template <class MEMORY_ADDR>
std::ostream& operator << (std::ostream& os, const DWARF_DIE<MEMORY_ADDR>& dw_die);

template <class MEMORY_ADDR>
class DWARF_DIE // Debug Info Entry
{
public:
	DWARF_DIE(DWARF_CompilationUnit<MEMORY_ADDR> *dw_cu, DWARF_DIE<MEMORY_ADDR> *dw_parent_die = 0);
	virtual ~DWARF_DIE();
	const DWARF_CompilationUnit<MEMORY_ADDR> *GetCompilationUnit() const;
	const DWARF_DIE<MEMORY_ADDR> *GetRootDIE() const;
	const DWARF_DIE<MEMORY_ADDR> *GetParentDIE() const;
	const DWARF_Abbrev *GetAbbrev() const;
	const std::vector<DWARF_DIE<MEMORY_ADDR> *>& GetChildren() const;
	const std::vector<DWARF_Attribute<MEMORY_ADDR> *>& GetAttributes() const;
	const DWARF_Attribute<MEMORY_ADDR> *GetAttribute(uint16_t dw_at) const;
	uint16_t GetTag() const;
	uint64_t GetOffset() const;
	void Add(DWARF_Attribute<MEMORY_ADDR> *dw_attribute);
	void Add(DWARF_DIE<MEMORY_ADDR> *child);
	int64_t Load(const uint8_t *rawdata, uint64_t max_size, uint64_t offset);
	bool IsNull() const;
	void Fix(DWARF_Handler<MEMORY_ADDR> *dw_handler, unsigned int id);
	unsigned int GetId() const;
	std::string GetHREF() const;
	std::ostream& to_XML(std::ostream& os);
	std::ostream& to_HTML(std::ostream& os) const;
	friend std::ostream& operator << <MEMORY_ADDR>(std::ostream& os, const DWARF_DIE<MEMORY_ADDR>& dw_die);
	void BuildStatementMatrix(std::map<MEMORY_ADDR, Statement<MEMORY_ADDR> *>& stmt_matrix);
private:
	DWARF_CompilationUnit<MEMORY_ADDR> *dw_cu;
	DWARF_DIE<MEMORY_ADDR> *dw_parent_die;
	uint64_t offset;
	unsigned int id;
	const DWARF_Abbrev *abbrev;
	std::vector<DWARF_DIE<MEMORY_ADDR> *> children;
	std::vector<DWARF_Attribute<MEMORY_ADDR> *> attributes;
};

} // end of namespace dwarf
} // end of namespace debug
} // end of namespace util
} // end of namespace unisim

#endif
