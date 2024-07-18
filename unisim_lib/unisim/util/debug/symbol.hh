/*
 *  Copyright (c) 2007,
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
 
#ifndef __UNISIM_UTIL_DEBUG_SYMBOL_HH__
#define __UNISIM_UTIL_DEBUG_SYMBOL_HH__

#include <string>

namespace unisim {
namespace util {
namespace debug {

struct SymbolBase
{
	enum Type
	{
		SYM_NOTYPE  = 0, // unspecified type
		SYM_OBJECT  = 1, // data object
		SYM_FUNC    = 2, // function
		SYM_SECTION = 3, // section
		SYM_FILE    = 4, // source file
		SYM_COMMON  = 5, // uninitialized common block
		SYM_TLS     = 6, // TLS object
		SYM_OS      = 7, // operating system specific
		SYM_PROC    = 8  // processor specific
	};
	
	static Type Types[9];
	
	SymbolBase(const char *_name, Type _type);
	
	const char *GetName() const { return name.c_str(); }
	Type GetType() const { return type; }
protected:
	std::string name;
	Type type;
};

template <class T>
struct Symbol : SymbolBase
{
	Symbol(const char *name, T addr, T size, typename unisim::util::debug::Symbol<T>::Type type, T memory_atom_size);

	T GetAddress() const { return addr; }
	T GetSize() const { return size; }
	std::string GetFriendlyName(T addr) const;
private:
	T addr;
	T size;
	T memory_atom_size;
};

} // end of namespace debug
} // end of namespace util
} // end of namespace unisim

std::ostream& operator << (std::ostream& stream, unisim::util::debug::SymbolBase::Type sym_type);

std::istream& operator >> (std::istream& stream, unisim::util::debug::SymbolBase::Type& sym_type);

#endif
