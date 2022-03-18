/*
 *  Copyright (c) 2009-2021,
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
 * Authors: Reda Nouacer (reda.nouacer@cea.fr), Yves Lhuillier (yves.lhuillier@cea.fr)
 */

#ifndef __UNISIM_SERVICE_TEE_REGISTERS_HH__
#define __UNISIM_SERVICE_TEE_REGISTERS_HH__

#include <unisim/service/interfaces/registers.hh>
#include <unisim/service/interfaces/register.hh>
#include <unisim/kernel/kernel.hh>
#include <string>
#include <cstdint>

namespace unisim {
namespace service {
namespace tee {
namespace registers {

  using unisim::service::interfaces::Registers;

  template <unsigned NUM_EXPORTS=16>
  struct RegistersTee
    : public unisim::kernel::Service<Registers>
    , public unisim::kernel::Client<Registers>
  {
    unisim::kernel::ServiceExport<Registers> registers_export;
    unisim::kernel::ServiceImport<Registers> *registers_import[NUM_EXPORTS];
    unisim::kernel::ServiceImport<Registers>& registers_import_n(unsigned idx) { return *registers_import[idx]; }

    RegistersTee(const char* name, unisim::kernel::Object *parent = 0)
      : unisim::kernel::Object(name, parent)
      , Service<Registers>(name, parent)
      , Client<Registers>(name, parent)
      , registers_export("registers_export", this)
      , registers_import()
    {
      for (unsigned i=0; i<NUM_EXPORTS; i++)
        registers_import[i] = new unisim::kernel::ServiceImport<Registers>((std::string("registers-import-") + std::to_string(i)).c_str(), this);
    }

    ~RegistersTee()
    {
      for (unsigned i=0; i<NUM_EXPORTS; i++)
        delete registers_import[i];
    }

    void Setup(Registers*) override
    {
      for (unsigned i=0; i<NUM_EXPORTS; i++)
        registers_import_n(i).RequireSetup();
    }


    unisim::service::interfaces::Register* GetRegister(const char *name)
    {
      for (unsigned i=0; i<NUM_EXPORTS; i++)
        {
          if (not registers_import_n(i))
            continue;
          if (unisim::service::interfaces::Register* reg = registers_import_n(i)->GetRegister(name))
            return reg;
        }
      return 0;
    }

    void ScanRegisters( unisim::service::interfaces::RegisterScanner& scanner )
    {
      for (unsigned i=0; i<NUM_EXPORTS; i++)
        {
          if (not registers_import_n(i))
            continue;
          registers_import_n(i)->ScanRegisters( scanner );
        }
    }
  };

} // end registers
} // end tee
} // end service
} // end unisim

#endif



