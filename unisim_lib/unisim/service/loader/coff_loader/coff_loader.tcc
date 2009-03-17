/*
 *  Copyright (c) 2009,
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
 
#ifndef __UNISIM_SERVICE_LOADER_COFF_LOADER_COFF_LOADER_TCC__
#define __UNISIM_SERVICE_LOADER_COFF_LOADER_COFF_LOADER_TCC__

#include <unisim/util/endian/endian.hh>
#include <iostream>
#include <fstream>
#include <stdlib.h>

namespace unisim {
namespace service {
namespace loader {
namespace coff_loader {

template <class MEMORY_ADDR>
CoffLoader<MEMORY_ADDR>::CoffLoader(const char *name, Object *parent) :
	Object(name, parent),
	Client<Memory<MEMORY_ADDR> >(name, parent),
	Client<SymbolTableBuild<MEMORY_ADDR> >(name, parent),
	Service<Loader<MEMORY_ADDR> >(name, parent),
	memory_import("memory-import", this),
	symbol_table_build_import("symbol-table-build-import", this),
	loader_export("loader-export", this),
	filename(),
	entry_point(0),
	base_addr(0),
	top_addr(0),
	dump_headers(false),
	param_filename("filename", this, filename),
	param_base_addr("base-addr", this, base_addr),
	param_dump_headers("dump-headers", this, dump_headers)
{
	Object::SetupDependsOn(memory_import);
	Object::SetupDependsOn(symbol_table_build_import);
}

template <class MEMORY_ADDR>
CoffLoader<MEMORY_ADDR>::~CoffLoader()
{
}

template <class MEMORY_ADDR>
void CoffLoader<MEMORY_ADDR>::OnDisconnect()
{
}

template <class MEMORY_ADDR>
MEMORY_ADDR CoffLoader<MEMORY_ADDR>::GetEntryPoint() const
{
	return (MEMORY_ADDR) entry_point;
}

template <class MEMORY_ADDR>
MEMORY_ADDR CoffLoader<MEMORY_ADDR>::GetTopAddr() const
{
	return (MEMORY_ADDR) top_addr;
}

template <class MEMORY_ADDR>
MEMORY_ADDR CoffLoader<MEMORY_ADDR>::GetStackBase() const
{
  return 0;
}

template <class MEMORY_ADDR>
bool CoffLoader<MEMORY_ADDR>::Setup()
{
	if(symbol_table_build_import) symbol_table_build_import->Reset();

	bool success = true;

	if(filename.empty()) return true;
	
	ifstream is(filename.c_str(), ifstream::in | ifstream::binary);
	if(is.fail())
	{
		cerr << Object::GetName() << ": ERROR! Can't open executable \"" << filename << "\"" << endl;
		return false;
	}
	
	cerr << Object::GetName() << ": Opening \"" << filename << "\"" << endl;

	success = false;

	return success;
}

template <class MEMORY_ADDR>
void CoffLoader<MEMORY_ADDR>::Reset()
{
	if(memory_import) memory_import->Reset();
}


} // end of namespace coff_loader
} // end of namespace loader
} // end of namespace service
} // end of namespace unisim

#endif
