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
 * Authors: Reda   Nouacer  (reda.nouacer@cea.fr)
 */

#include <inttypes.h>

#include "unisim/service/interfaces/memory.hh"
#include <unisim/kernel/service/service.hh>


namespace unisim {
namespace service {
namespace tee {
namespace memory_import_export {


using unisim::service::interfaces::Memory;

using unisim::kernel::service::Object;
using unisim::kernel::service::Client;
using unisim::kernel::service::Service;
using unisim::kernel::service::ServiceExport;
using unisim::kernel::service::ServiceImport;

template <class service_address_t, uint8_t width = 16 >
class MemoryImportExportTee :
	public Service<Memory<service_address_t> >,
	public Client<Memory<service_address_t> >
{
public:
	ServiceExport<Memory<service_address_t> > memory_export;
	ServiceImport<Memory<service_address_t> > *memory_import[width];

	MemoryImportExportTee(const char* name, Object *parent = 0) :
		Object(name, parent),
		Service<Memory<service_address_t> >(name, parent),
		Client<Memory<service_address_t> >(name, parent),

		memory_export("memory_export", this)
	{

		for (uint8_t i=0; i<width; i++) {
			std::ostringstream out;
			out << "memory-import-" << i;
			memory_import[i] = new ServiceImport<Memory<service_address_t> >(out.str().c_str(), this);
		}

	}

	~MemoryImportExportTee() {
		for (uint8_t i=0; i<width; i++) {
			if (memory_import[i]) {
				delete memory_import[i];
				memory_import[i] = NULL;
			}
		}

	}

	virtual void Reset() {};

	virtual bool ReadMemory(service_address_t addr, void *buffer, uint32_t size) {

		bool result = false;

		for (uint8_t i=0; ((!result) && (i < width)); i++) {
			if (*memory_import[i]) {
				result = (*memory_import[i])->ReadMemory(addr, buffer, size);
			}
		}

		return result;
	};

	virtual bool WriteMemory(service_address_t addr, const void *buffer, uint32_t size) {

		bool result = false;

		for (uint8_t i=0; ((!result) && (i < width)); i++) {
			if (*memory_import[i]) {
				result = (*memory_import[i])->WriteMemory(addr, buffer, size);
			}
		}

		return result;
	};

};


} // end memory_import_export
} // end tee 
} // end service
} // end unisim 




