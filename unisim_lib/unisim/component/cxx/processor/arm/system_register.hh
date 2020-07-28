/*
 *  Copyright (c) 2013-2020,
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
 * Authors: Yves Lhuillier (yves.lhuillier@cea.fr)
 */

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_SYSTEM_REGISTER_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_SYSTEM_REGISTER_HH__

#include <inttypes.h>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace arm {
  
  /** SystemRegister
   *
   * This structure is an interface to ARM system registers.  In ARMv7
   * and earlier versions of the ARM architecture, system register
   * provided through conceptual coprocessors CP14 and CP15, and in
   * part through CP10 and CP11. Some system registers may also be
   * accessed through MSR/MRS with a 7-bits identifier. For
   * compatibility: ARMv8 AArch32 state retains this conceptual
   * coprocessor model. However, it adds register and operation
   * aliases, to simplify access to this functionality.  In the
   * instruction encoding descriptions, AArch64 state retains the
   * naming of the instruction arguments as Op1, CRn, CRm, and
   * Op2. However, there is no functional distinction between the Opn
   * arguments and the CRx arguments. AArch64 adds an OP0
   * field. Altogether, Op0 has 2 bits, Op1&Op2 have 3 bits, and
   * CRn&CRm have 4 bits.  Using 4 bits variables and proper
   * conventions, all arm registers can share this same SystemRegister interface.
   */
  struct srid_t
  {
    uint32_t op2 : 4;
    uint32_t crn : 4;
    uint32_t crm : 4;
    uint32_t op1 : 4;
    uint32_t op0 : 4;
  };
  
  template <class ARCH>
  struct SystemRegister
  {
    typedef typename ARCH::UREG UREG;
    virtual ~SystemRegister() {}
    virtual void DisasmRead(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, std::ostream& sink) const = 0;
    virtual void DisasmWrite(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, std::ostream& sink) const = 0;
    virtual UREG Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, ARCH& arch) const = 0;
    virtual void Write(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, ARCH& arch, UREG value) const = 0;
  };
  
} // end of namespace arm
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif /* __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_SYSTEM_REGISTER_HH__ */
