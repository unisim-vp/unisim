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

#include "unisim/service/interfaces/registers.hh"
#include "unisim/service/interfaces/register.hh"
#include <unisim/kernel/service/service.hh>
#include <stdint.h>

namespace unisim {
namespace service {
namespace tee {
namespace registers {


using unisim::service::interfaces::Registers;
using unisim::service::interfaces::Register;
using unisim::kernel::service::Object;
using unisim::kernel::service::Client;
using unisim::kernel::service::Service;
using unisim::kernel::service::ServiceExport;
using unisim::kernel::service::ServiceImport;

template <uint8_t size = 16 >
class RegistersTee :
	public Service<Registers>,
	public Client<Registers>
{
public:
	ServiceExport<Registers> registers_export;
	ServiceImport<Registers> *registers_import[size];

	RegistersTee(const char* name, Object *parent = 0) :
		Object(name, parent),
		Service<Registers>(name, parent),
		Client<Registers>(name, parent),

		registers_export("registers_export", this)
	{

		for (uint8_t i=0; i<size; i++) {
			std::ostringstream out;
			out << "registers-import-" << i;
			registers_import[i] = new ServiceImport<Registers>(out.str().c_str(), this);
		}

	}

	~RegistersTee() {
		for (uint8_t i=0; i<size; i++) {
			if (registers_import[i]) {
				delete registers_import[i];
				registers_import[i] = NULL;
			}
		}

	}

	Register *GetRegister(const char *name) {
		Register* reg = NULL;

		for (uint8_t i=0; ((reg == NULL) && (i < size)); i++) {
			if (*registers_import[i]) {
				reg = (*registers_import[i])->GetRegister(name);
			}
		}

		return reg;
	}
};

} // end registers
} // end tee 
} // end service
} // end unisim 





