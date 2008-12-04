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
 
#ifndef __UNISIM_SERVICE_DEBUG_SYMBOL_TABLE_TCC__
#define __UNISIM_SERVICE_DEBUG_SYMBOL_TABLE_TCC__

#include <iostream>
#include <sstream>

namespace unisim {
namespace service {
namespace debug {
namespace symbol_table {

template <class T>
Symbol<T>::Symbol(const char *_name, T _addr, T _size, typename unisim::util::debug::Symbol<T>::Type _type) :
	name(_name),
	addr(_addr),
	size(_size),
	type(_type)
{
}

template <class T>
const char *Symbol<T>::GetName() const
{
	return name.c_str();
}

template <class T>
T Symbol<T>::GetAddress() const
{
	return addr;
}

template <class T>
T Symbol<T>::GetSize() const
{
	return size;
}

template <class T>
typename unisim::util::debug::Symbol<T>::Type Symbol<T>::GetType() const
{
	return type;
}

template <class T>
string Symbol<T>::GetFriendlyName(T addr) const
{
	stringstream sstr;
	
	sstr << name;
	if(type == unisim::util::debug::Symbol<T>::SYM_FUNC)
		sstr << "()";
	if(addr != this->addr && (addr - this->addr) <= size)
		sstr << "+0x" << std::hex << addr - this->addr << std::dec;
	
	return sstr.str();
}

template <class T>
SymbolTable<T>::SymbolTable(const char *name, Object *parent) :
	Object(name, parent),
	Service<SymbolTableLookup<T> >(name, parent),
	Service<SymbolTableBuild<T> >(name, parent),
	symbol_table_lookup_export("symbol-table-lookup-export", this),
	symbol_table_build_export("symbol-table-build-export", this)
{
}

template <class T>
SymbolTable<T>::~SymbolTable()
{
	Reset();
}

template <class T>
void SymbolTable<T>::Reset()
{
	typename list<Symbol<T> *>::iterator symbol_iter;
	unsigned int i;
	
	for(i = 0; i < sizeof(symbol_registries) / sizeof(symbol_registries[0]); i++)
	{
		for(symbol_iter = symbol_registries[i].begin(); symbol_iter != symbol_registries[i].end(); symbol_iter++)
		{
			delete *symbol_iter;
		}
		symbol_registries[i].clear();
	}
}

template <class T>
const typename unisim::util::debug::Symbol<T> *SymbolTable<T>::FindSymbol(const char *name, T addr, typename unisim::util::debug::Symbol<T>::Type type) const
{
	typename list<Symbol<T> *>::const_iterator symbol_iter;
	
	for(symbol_iter = symbol_registries[type].begin(); symbol_iter != symbol_registries[type].end(); symbol_iter++)
	{
		if(strcmp((*symbol_iter)->GetName(), name) == 0 &&
		   addr >= (*symbol_iter)->GetAddress() &&
		   addr < (*symbol_iter)->GetAddress() + (*symbol_iter)->GetSize() &&
		   (*symbol_iter)->GetType() == type)
		{
			return *symbol_iter;
		}
	}
	return 0;
}

template <class T>
const typename unisim::util::debug::Symbol<T> *SymbolTable<T>::FindSymbolByAddr(T addr) const
{
	unsigned int i;
	
	for(i = 0; i < sizeof(symbol_registries) / sizeof(symbol_registries[0]); i++)
	{
		const unisim::util::debug::Symbol<T> *symbol = FindSymbolByAddr(addr, (typename unisim::util::debug::Symbol<T>::Type) i);
		if(symbol) return symbol;
	}
	return 0;
}

template <class T>
const typename unisim::util::debug::Symbol<T> *SymbolTable<T>::FindSymbolByName(const char *name) const
{
	unsigned int i;
	
	for(i = 0; i < sizeof(symbol_registries) / sizeof(symbol_registries[0]); i++)
	{
		const unisim::util::debug::Symbol<T> *symbol = FindSymbolByName(name, (typename unisim::util::debug::Symbol<T>::Type) i);
		if(symbol) return symbol;
	}
	return 0;
}

template <class T>
const typename unisim::util::debug::Symbol<T> *SymbolTable<T>::FindSymbolByName(const char *name, typename unisim::util::debug::Symbol<T>::Type type) const
{
	typename list<Symbol<T> *>::const_iterator symbol_iter;
	
	for(symbol_iter = symbol_registries[type].begin(); symbol_iter != symbol_registries[type].end(); symbol_iter++)
	{
		if(strcmp((*symbol_iter)->GetName(), name) == 0) return *symbol_iter;
	}
	return 0;
}

template <class T>
const typename unisim::util::debug::Symbol<T> *SymbolTable<T>::FindSymbolByAddr(T addr, typename unisim::util::debug::Symbol<T>::Type type) const
{
	typename list<Symbol<T> *>::const_iterator symbol_iter;

	for(symbol_iter = symbol_registries[type].begin(); symbol_iter != symbol_registries[type].end(); symbol_iter++)
	{
		if((*symbol_iter)->GetType() == type && addr >= (*symbol_iter)->GetAddress() && addr < (*symbol_iter)->GetAddress() + (*symbol_iter)->GetSize())
		{
			return *symbol_iter;
		}
	}
	return 0;
}

template <class T>
void SymbolTable<T>::AddSymbol(const char *name, T addr, T size, typename unisim::util::debug::Symbol<T>::Type type)
{
	Symbol<T> *symbol = new Symbol<T>(name, addr, size, type);
	symbol_registries[type].push_back(symbol);
}

template <class T>
void SymbolTable<T>::Dump(ostream& os) const
{
	unsigned int i;
	
	for(i = 0; i < sizeof(symbol_registries) / sizeof(symbol_registries[0]); i++)
	{
		Dump(os, (typename Symbol<T>::Type) i);
	}
}

template <class T>
void SymbolTable<T>::Dump(ostream& os, typename unisim::util::debug::Symbol<T>::Type type) const
{
	typename list<Symbol<T> *>::const_iterator symbol_iter;

	for(symbol_iter = symbol_registries[type].begin(); symbol_iter != symbol_registries[type].end(); symbol_iter++)
	{
		os << std::hex << (*symbol_iter)->GetAddress() << std::dec << ": " << (*symbol_iter)->GetName() << " (" << (*symbol_iter)->GetSize() << " bytes)" << endl;
	}
}

} // end of namespace symbol_table
} // end of namespace debug
} // end of namespace service
} // end of namespace unisim

#endif
