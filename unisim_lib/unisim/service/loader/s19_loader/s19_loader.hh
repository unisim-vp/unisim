/*
 *  Copyright (c) 2008,
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
 * Authors: Reda   Nouacer  (reda.nouacer@cea.fr)
 */

#ifndef UNISIM_SERVICE_LOADER_S19_LOADER_HH_
#define UNISIM_SERVICE_LOADER_S19_LOADER_HH_

#include <unisim/service/interfaces/memory.hh>
#include <unisim/service/interfaces/loader.hh>
#include <unisim/service/interfaces/symbol_table_build.hh>

#include <unisim/util/endian/endian.hh>
#include <unisim/kernel/service/service.hh>

#include <iostream>
#include <fstream>

#include <inttypes.h>
#include  <stdio.h>
#include  <stdlib.h>
#include  <string.h>

typedef uint32_t physical_address_t;
typedef uint16_t address_t;
typedef uint32_t s19_address_t;
typedef uint8_t page_t;

namespace unisim {
namespace service {
namespace loader {
namespace s19_loader {

using namespace std;
using unisim::service::interfaces::Memory;
using namespace unisim::util::endian;
using unisim::kernel::service::Service;
using unisim::kernel::service::Client;
using unisim::kernel::service::Object;
using unisim::kernel::service::ServiceImport;
using unisim::kernel::service::ServiceExport;
using unisim::kernel::service::Parameter;
using unisim::util::debug::Symbol;
using unisim::service::interfaces::SymbolTableBuild;
using unisim::service::interfaces::Loader;

#define S_RECORD_SIZE	515		// s2_record_size = 2+2+255*2 +1 ("+1" is for \0 char)
	
class S19_Loader :
	public Client<Memory<physical_address_t> >,
	public Service<Loader<physical_address_t> >,
	public Client<SymbolTableBuild<physical_address_t> > 
{
public:

	enum MODE {BANKED=0, LINEAR=1, GNUGCC=2};

	enum {ERR_NOFILE, ERR_BADREC, ERR_NOSUPPORT, ERR_BADADDR, ERR_BADCHKSUM, ERR_BADFILENAME};

/* ********* S-Record Types *********************** 
 * Record	Description		Address Bytes	Data Sequence
 * ------	-----------		-------------	-------------
 * S0		Block header	2				Yes
 * S1		Data Sequence	2				Yes
 * S2		Data Sequence	3				Yes
 * S3		Data Sequence	4				Yes
 * S5		Record Count	2				Yes
 * S7		End of Block	4				No
 * S8		End of Block	3				No
 * S9		End of Block	2				No
 */
	enum {S0='0', S1='1', S2='2', S3='3', S5='5', S7='7', S8='8', S9='9'};

	ServiceImport<Memory<physical_address_t> > memory_import;
	ServiceImport<SymbolTableBuild<physical_address_t> > symbol_table_build_import;
	ServiceExport<Loader<physical_address_t> > loader_export;

	virtual void OnDisconnect();
	virtual bool Setup();
	virtual void Reset();
	virtual physical_address_t GetEntryPoint() const;
	virtual physical_address_t GetTopAddr() const;
	virtual physical_address_t GetStackBase() const;

	S19_Loader(char const *name, S19_Loader::MODE memMode, Object *parent = 0);
	virtual ~S19_Loader();	
	void GetPagedAddress(s19_address_t s19_addr, page_t &page, address_t &cpu_address);

private:
	string				filename;
	physical_address_t	entry_point;
	physical_address_t	top_addr;
	physical_address_t	base_addr;
	bool				force_use_virtual_address;
	
	Parameter<string>	param_filename;
	Parameter<physical_address_t>	param_base_addr;
	Parameter<bool>		param_force_use_virtual_address;
	
	bool				isFirstDataRec;
	int					memoryMode;

	physical_address_t GetFlashAddress(page_t page, address_t cpu_address);
	bool	ProcessRecord(int linenum, char srec[S_RECORD_SIZE]);
	void	ShowError(int  errnum, int linenum, char srec[S_RECORD_SIZE]);
	bool	memWrite(physical_address_t addr, const void *buffer, uint32_t size);

};



} // end UNISIM namespace
} // end SERVICE namespace
} // end LOADER namespace
} // end S19_LOADER

#endif /*UNISIM_SERVICE_LOADER_S19_LOADER_HH_*/
