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
 
#ifndef __UNISIM_UTIL_DEBUG_SIMPLE_SYMBOL_TABLE_TCC__
#define __UNISIM_UTIL_DEBUG_SIMPLE_SYMBOL_TABLE_TCC__

#include <unisim/util/debug/simple_symbol_table.hh>
#include <iostream>
#include <sstream>
#include <string.h>

namespace unisim {
namespace util {
namespace debug {

template <typename ADDRESS>
SimpleSymbolTable<ADDRESS>::SimpleSymbolTable()
	: SymbolTable<ADDRESS>()
	, symbol_registries()
	, types()
	, lookup_table_by_addr()
	, lookup_table_by_name()
{
	types.push_back(unisim::util::debug::SymbolBase::SYM_NOTYPE);
	types.push_back(unisim::util::debug::SymbolBase::SYM_OBJECT);
	types.push_back(unisim::util::debug::SymbolBase::SYM_FUNC);
	types.push_back(unisim::util::debug::SymbolBase::SYM_SECTION);
	types.push_back(unisim::util::debug::SymbolBase::SYM_FILE);
	types.push_back(unisim::util::debug::SymbolBase::SYM_COMMON);
	types.push_back(unisim::util::debug::SymbolBase::SYM_TLS);
	types.push_back(unisim::util::debug::SymbolBase::SYM_OS);
	types.push_back(unisim::util::debug::SymbolBase::SYM_PROC);
}

template <typename ADDRESS>
SimpleSymbolTable<ADDRESS>::~SimpleSymbolTable()
{
	Reset();
}

template <typename ADDRESS>
void SimpleSymbolTable<ADDRESS>::Reset()
{
	typename SymbolRegistry::iterator symbol_iter;
	unsigned int i;
	
	for(i = 0; i < sizeof(symbol_registries) / sizeof(symbol_registries[0]); i++)
	{
		for(symbol_iter = symbol_registries[i].begin(); symbol_iter != symbol_registries[i].end(); symbol_iter++)
		{
			delete *symbol_iter;
		}
		symbol_registries[i].clear();
		lookup_table_by_addr[i].Reset();
		lookup_table_by_name[i].clear();
	}
}

template <typename ADDRESS>
void SimpleSymbolTable<ADDRESS>::ScanSymbols(unisim::service::interfaces::SymbolTableScanner<ADDRESS>& scanner) const
{
	for(typename Types::const_iterator it = types.begin(); it != types.end(); ++it)
	{
		typename unisim::util::debug::SymbolBase::Type type = *it;
		typename SymbolRegistry::const_iterator symbol_iter;
			
		for(symbol_iter = symbol_registries[type].begin(); symbol_iter != symbol_registries[type].end(); symbol_iter++)
		{
			scanner.Append(*symbol_iter);
		}
	}
}

template <typename ADDRESS>
void SimpleSymbolTable<ADDRESS>::ScanSymbols(unisim::service::interfaces::SymbolTableScanner<ADDRESS>& scanner, typename unisim::util::debug::SymbolBase::Type type) const
{
	typename SymbolRegistry::const_iterator symbol_iter;
		
	for(symbol_iter = symbol_registries[type].begin(); symbol_iter != symbol_registries[type].end(); symbol_iter++)
	{
		scanner.Append(*symbol_iter);
	}
}

template <typename ADDRESS>
const typename unisim::util::debug::Symbol<ADDRESS> *SimpleSymbolTable<ADDRESS>::FindSymbolByAddr(ADDRESS addr) const
{
	for(typename Types::const_iterator it = types.begin(); it != types.end(); ++it)
	{
		typename unisim::util::debug::SymbolBase::Type type = *it;
		const unisim::util::debug::Symbol<ADDRESS> *symbol = FindSymbolByAddr(addr, type);
		if(symbol)
		{
			Use(type);
			return symbol;
		}
	}
	return 0;
}

template <typename ADDRESS>
const typename unisim::util::debug::Symbol<ADDRESS> *SimpleSymbolTable<ADDRESS>::FindSymbolByName(const char *name) const
{
	for(typename Types::const_iterator it = types.begin(); it != types.end(); ++it)
	{
		typename unisim::util::debug::SymbolBase::Type type = *it;
		const unisim::util::debug::Symbol<ADDRESS> *symbol = FindSymbolByName(name, type);
		if(symbol)
		{
			Use(type);
			return symbol;
		}
	}
	return 0;
}

template <typename ADDRESS>
const typename unisim::util::debug::Symbol<ADDRESS> *SimpleSymbolTable<ADDRESS>::FindSymbolByName(const char *name, typename unisim::util::debug::SymbolBase::Type type) const
{
	typename LookupTableByName::const_iterator it = lookup_table_by_name[type].find(name);
	return (it != lookup_table_by_name[type].end()) ? (*it).second : 0;
}

template <typename ADDRESS>
const typename unisim::util::debug::Symbol<ADDRESS> *SimpleSymbolTable<ADDRESS>::FindSymbolByAddr(ADDRESS addr, typename unisim::util::debug::SymbolBase::Type type) const
{
	LookupTableByAddrEntry *entry = lookup_table_by_addr[type].Find(addr);
	return entry ? entry->symbol : 0;
}

template <typename ADDRESS>
void SimpleSymbolTable<ADDRESS>::AddSymbol(const char *name, ADDRESS addr, ADDRESS size, typename unisim::util::debug::SymbolBase::Type type, ADDRESS memory_atom_size)
{
	Symbol<ADDRESS> *symbol = new Symbol<ADDRESS>(name, addr, size, type, memory_atom_size);
	symbol_registries[type].push_back(symbol);
	
	for(ADDRESS i = 0; i < size; i++)
	{
		lookup_table_by_addr[type].Insert(new LookupTableByAddrEntry(addr + i, symbol));
	}
	
	lookup_table_by_name[type].insert(std::pair<std::string, Symbol<ADDRESS> *>(name, symbol));
}

template <typename ADDRESS>
void SimpleSymbolTable<ADDRESS>::Dump(std::ostream& os) const
{
	unsigned int i;
	
	for(i = 0; i < sizeof(symbol_registries) / sizeof(symbol_registries[0]); i++)
	{
		Dump(os, (typename Symbol<ADDRESS>::Type) i);
	}
}

template <typename ADDRESS>
void SimpleSymbolTable<ADDRESS>::Dump(std::ostream& os, typename unisim::util::debug::SymbolBase::Type type) const
{
	typename SymbolRegistry::const_iterator symbol_iter;

	for(symbol_iter = symbol_registries[type].begin(); symbol_iter != symbol_registries[type].end(); symbol_iter++)
	{
		os << type << ":" << std::hex << (*symbol_iter)->GetAddress() << std::dec << ": " << (*symbol_iter)->GetName() << " (" << (*symbol_iter)->GetSize() << " bytes)" << std::endl;
	}
}

template <typename ADDRESS>
void SimpleSymbolTable<ADDRESS>::Use(typename unisim::util::debug::SymbolBase::Type type) const
{
	if(types.front() != type)
	{
		for(typename Types::iterator it = types.begin(); it != types.end(); ++it)
		{
			if((*it) == type)
			{
				types.erase(it);
				types.insert(types.begin(), type);
				return;
			}
		}
	}
}

} // end of namespace debug
} // end of namespace service
} // end of namespace unisim

#endif
