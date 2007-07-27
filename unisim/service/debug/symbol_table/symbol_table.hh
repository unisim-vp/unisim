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
 
#ifndef __FS_UTILS_DEBUG_SYMBOL_TABLE_HH__
#define __FS_UTILS_DEBUG_SYMBOL_TABLE_HH__

#include <utils/debug/symbol_interface.hh>
#include <utils/debug/symbol_table_interface.hh>
#include <utils/services/service.hh>
#include <string>
#include <list>
#include <iostream>
#include <sstream>

namespace full_system {
namespace utils {
namespace debug {
	
using std::string;
using std::list;
using std::cerr;
using std::ostream;
using std::endl;
using std::hex;
using std::dec;
using std::stringstream;
using full_system::utils::services::Service;
using full_system::utils::services::Client;
using full_system::utils::services::Object;
using full_system::utils::services::ServiceImport;
using full_system::utils::services::ServiceExport;

template <class T>
class Symbol : public SymbolInterface<T>
{
public:
	Symbol(const char *name, T addr, T size, typename SymbolInterface<T>::Type type);

	virtual const char *GetName() const;
	virtual T GetAddress() const;
	virtual T GetSize() const;
	virtual typename SymbolInterface<T>::Type GetType() const;
	virtual string GetFriendlyName(T addr) const;
private:
	string name;
	T addr;
	T size;
	typename SymbolInterface<T>::Type type;
};

template <class T>
class SymbolTable :
	public Service<SymbolTableLookupInterface<T> >,
	public Service<SymbolTableBuildInterface<T> >
{
public:
	ServiceExport<SymbolTableLookupInterface<T> > symbol_table_lookup_export;
	ServiceExport<SymbolTableBuildInterface<T> > symbol_table_build_export;
	
	SymbolTable(const char *name, Object *parent = 0);
	virtual ~SymbolTable();
	virtual void Reset();
	virtual const SymbolInterface<T> *FindSymbol(const char *name, T addr, typename SymbolInterface<T>::Type type) const;
	virtual const SymbolInterface<T> *FindSymbolByAddr(T addr) const;
	virtual const SymbolInterface<T> *FindSymbolByName(const char *name) const;
	virtual const SymbolInterface<T> *FindSymbolByName(const char *name, typename SymbolInterface<T>::Type type) const;
	virtual const SymbolInterface<T> *FindSymbolByAddr(T addr, typename SymbolInterface<T>::Type type) const ;
	virtual void AddSymbol(const char *name, T addr, T size, typename SymbolInterface<T>::Type type);
	virtual void Dump(ostream& os) const;
	virtual void Dump(ostream& os, typename SymbolInterface<T>::Type type) const;
private:
	
	list<Symbol<T> *> symbol_registries[SymbolInterface<T>::SYM_HIPROC + 1];
};

} // end of namespace debug
} // end of namespace utils
} // end of namespace full_system

#endif
