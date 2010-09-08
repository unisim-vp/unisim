/*
 *  Copyright (c) 2010,
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
 * Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 */

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_MASKS_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_MASKS_HH__

#include <inttypes.h>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace arm {

/* masks for the different running modes */
static const uint32_t RUNNING_MODE_MASK = 0x1F;
static const uint32_t USER_MODE = 0x10;
static const uint32_t FIQ_MODE = 0x11;
static const uint32_t IRQ_MODE = 0x12;
static const uint32_t SUPERVISOR_MODE = 0x13;
static const uint32_t ABORT_MODE = 0x17;
static const uint32_t UNDEFINED_MODE = 0x1B;
static const uint32_t SYSTEM_MODE = 0x1F;
/* masks for the different CPSR status bits */
static const uint32_t CPSR_N_MASK = 0x80000000;
static const uint32_t CPSR_Z_MASK = 0x40000000;
static const uint32_t CPSR_C_MASK = 0x20000000;
static const uint32_t CPSR_V_MASK = 0x10000000;
static const uint32_t CPSR_Q_MASK = 0x01000000;
static const uint32_t CPSR_I_MASK = 0x00000080;
static const uint32_t CPSR_F_MASK = 0x00000040;
static const uint32_t CPSR_T_MASK = 0x00000020;
static const uint32_t CPSR_RUNNING_MODE_MASK = 0x0000001F;
/* masks for the different SPSR status bits */
static const uint32_t SPSR_N_MASK = CPSR_N_MASK;
static const uint32_t SPSR_Z_MASK = CPSR_Z_MASK;
static const uint32_t SPSR_C_MASK = CPSR_C_MASK;
static const uint32_t SPSR_V_MASK = CPSR_V_MASK;
static const uint32_t SPSR_Q_MASK = CPSR_Q_MASK;
static const uint32_t SPSR_I_MASK = CPSR_I_MASK;
static const uint32_t SPSR_F_MASK = CPSR_F_MASK;
static const uint32_t SPSR_T_MASK = CPSR_T_MASK;
static const uint32_t SPSR_RUNNING_MODE_MASK = CPSR_RUNNING_MODE_MASK;
/* masks for the different condition codes */
static const uint32_t COND_EQ = 0x00;
static const uint32_t COND_NE = 0x01;
static const uint32_t COND_CS_HS = 0x02;
static const uint32_t COND_CC_LO = 0x03;
static const uint32_t COND_MI = 0x04;
static const uint32_t COND_PL = 0x05;
static const uint32_t COND_VS = 0x06;
static const uint32_t COND_VC = 0x07;
static const uint32_t COND_HI = 0x08;
static const uint32_t COND_LS = 0x09;
static const uint32_t COND_GE = 0x0a;
static const uint32_t COND_LT = 0x0b;
static const uint32_t COND_GT = 0x0c;
static const uint32_t COND_LE = 0x0d;
static const uint32_t COND_AL = 0x0e;

} // end of namespace arm
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif /* __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_MASKS_HH__ */
