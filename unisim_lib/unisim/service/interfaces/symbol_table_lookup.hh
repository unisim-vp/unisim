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
 
#ifndef __UNISIM_SERVICE_INTERFACES_SYMBOL_TABLE_LOOKUP_HH__
#define __UNISIM_SERVICE_INTERFACES_SYMBOL_TABLE_LOOKUP_HH__

#include <unisim/service/interfaces/interface.hh>
#include <list>
#include <unisim/util/debug/symbol.hh>

namespace unisim {
namespace service {
namespace interfaces {

template <typename ADDRESS>
struct SymbolTableScanner : ServiceInterface
{
	virtual void Append(const unisim::util::debug::Symbol<ADDRESS> *symbol) = 0;
};

template <typename ADDRESS>
struct SymbolTableLookup : ServiceInterface
{
public:
	virtual void ScanSymbols(SymbolTableScanner<ADDRESS>& scanner) const = 0;
	virtual void ScanSymbols(SymbolTableScanner<ADDRESS>& scanner, typename unisim::util::debug::SymbolBase::Type type) const = 0;
	virtual const typename unisim::util::debug::Symbol<ADDRESS> *FindSymbolByAddr(ADDRESS addr) const = 0;
	virtual const typename unisim::util::debug::Symbol<ADDRESS> *FindSymbolByName(const char *name) const = 0;
	virtual const typename unisim::util::debug::Symbol<ADDRESS> *FindSymbolByName(const char *name, typename unisim::util::debug::SymbolBase::Type type) const = 0;
	virtual const typename unisim::util::debug::Symbol<ADDRESS> *FindSymbolByAddr(ADDRESS addr, typename unisim::util::debug::SymbolBase::Type type) const = 0;
};

} // end of namespace interfaces
} // end of namespace service
} // end of namespace unisim

#endif
