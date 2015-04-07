/*
 *  Copyright (c) 2013,
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
 * Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr), Yves Lhuillier (yves.lhuillier@cea.fr)
 */

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_STATUS_REGISTER_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_STATUS_REGISTER_HH__

#include <inttypes.h>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace arm {

/** Program Status Register (PSR).
 * Organization:
 * - bit 31: N
 * - bit 30: Z
 * - bit 29: C
 * - bit 28: V
 * - bit 27: Q
 * - bits 26-25: IT[1:0]
 * - bit 24: J
 * - bits 23-20: RAZ/SBZP
 * - bits 19-16: GE
 * - bits 15-10: IT[7:2]
 * - bit 9: E
 * - bit 8: A
 * - bit 7: I
 * - bit 6: F
 * - bit 5: T
 * - bits 4-0: M bits, running mode, see bellow
 * Running modes:
 * - 0b10000: User
 * - 0b10001: FIQ (Fast Interrupt)
 * - 0b10010: IRQ (Interrupt) 
 * - 0b10011: Supervisor
 * - 0b10111: Abort
 * - 0b11011: Undefined
 * - 0b11111: System
 */
  struct PSR {
    PSR() : m_value( 0 ) {}
  
    template <uint32_t posT, uint32_t sizeT>
    struct Field {
      Field( uint32_t& _value ) : m_value( _value ) {}
      uint32_t Get() const
      { return (m_value >> posT) & ((uint32_t( 1 ) << sizeT) - 1); }
      void     Set( uint32_t field_value )
      { m_value = (m_value & ~(((uint32_t( 1 ) << sizeT) - 1) << posT)) | (field_value << posT); }
      uint32_t&  m_value;
    };
    /* Raw bits */
    uint32_t&   bits() { return m_value; }
    /* Condition code flags */
    Field<31,1> N() { return Field<31,1>( m_value ); }
    Field<30,1> Z() { return Field<30,1>( m_value ); }
    Field<29,1> C() { return Field<29,1>( m_value ); }
    Field<28,1> V() { return Field<28,1>( m_value ); }
    Field<28,4> NZCV() { return Field<28,4>( m_value ); }
    /* Cumulative saturation flag */
    Field<27,1> Q() { return Field<27,1>( m_value ); }
    /* Jazelle execution state bit */
    Field<24,1> J() { return Field<24,1>( m_value ); }
    /* Greater than or Equal flags, for SIMD instructions */
    Field<16,4> GE() { return Field<16,4>( m_value ); }
    /* Endianness execution state */
    Field<9,1> E() { return Field<9,1>( m_value ); }
    /* Exception mask bits, asynchronous exceptions disabling */
    Field<8,1> A() { return Field<8,1>( m_value ); }
    Field<7,1> I() { return Field<7,1>( m_value ); }
    Field<6,1> F() { return Field<6,1>( m_value ); }
    /* Thumb execution state bit */
    Field<5,1> T() { return Field<5,1>( m_value ); }
    /* Mode field */
    Field<0,5> M() { return Field<0,5>( m_value ); }
  
    void ITSetState( uint32_t cond, uint32_t mask ) {
      Field<12,4>( m_value ).Set( cond );
      Field<10,2>( m_value ).Set( (mask >> 2) & 3 );
      Field<25,2>( m_value ).Set( (mask >> 0) & 3 );
    }
  
    bool InITBlock() { return Field<10,2>( m_value ).Get() || Field<25,2>( m_value ).Get(); }
  
    uint32_t ITGetCondition() { return this->InITBlock() ? Field<12,4>( m_value ).Get() : 14; }
  
    void ITAdvance() {
      uint32_t state = (Field<10,6>( m_value ).Get() << 2) | Field<25,2>( m_value ).Get();
      state = (state & 7) ? ((state & -32) | ((state << 1) & 31)) : 0;
      Field<10,6>( m_value ).Set( state >> 2 );
      Field<25,2>( m_value ).Set( state & 3 );
    }
  
    uint32_t m_value;
  };

// struct InsnCond {
//   // uint32_t ITPop()
//   // {
//   //   uint32_t cond = ITCondition();
//   //   this->ITAdvance();
//   //   return cond;
//   // }
// };

// /* masks for the different running modes */
// static uint32_t const RUNNING_MODE_MASK = 0x1F;
// static uint32_t const USER_MODE = 0x10;
// static uint32_t const FIQ_MODE = 0x11;
// static uint32_t const IRQ_MODE = 0x12;
// static uint32_t const SUPERVISOR_MODE = 0x13;
// static uint32_t const ABORT_MODE = 0x17;
// static uint32_t const UNDEFINED_MODE = 0x1B;
// static uint32_t const SYSTEM_MODE = 0x1F;

// uint32_t GetSPSRIndex()
// {
//   uint32_t rm = 0;
//   uint32_t run_mode = cpsr & CPSR_RUNNING_MODE_MASK;
//   switch (run_mode)
//     {
//     case USER_MODE: case SYSTEM_MODE: {
//       /* In user or system mode, access to SPSR are unpredictable,
//        * thus the code whould never try to access SPSR in such
//        * modes. */
      
//       logger << DebugWarning
//              << "trying to access SPSR while running in "
//              << ((run_mode == USER_MODE) ? "user" : "system")
//              << " mode with the following instruction: "
//              << std::endl
//              << "Location: " << __FUNCTION__
//              << ":" << __FILE__
//              << ":" << __LINE__
//              << EndDebugWarning;
//       Stop(-1);
//     } break;
//     case SUPERVISOR_MODE:
//       rm = 0;
//       break;
//     case ABORT_MODE:
//       rm = 1;
//       break;
//     case UNDEFINED_MODE:
//       rm = 2;
//       break;
//     case IRQ_MODE:
//       rm = 3;
//       break;
//     case FIQ_MODE:
//       rm = 4;
//       break;
//     default:
//       assert(0);
//       break;
//     }
  
//   return rm;
// }

// /** Copy the value of current SPSR register into CPSR.
//  */
// void 
// MoveSPSRtoCPSR()
// {
//   /* SPSR needs to be moved to CPSR
//    * This means that we need to change the register mapping if the running mode has changed
//    */
//   uint32_t src_mode = CPSR().M().Get();
//   uint32_t dst_mode = SPSR().M().Get();
//   uint32_t cur_spsr = SPSR().bits();
//   CPSR().bits() = SPSR().bits();
//   if (src_mode != dst_mode)
//     SetGPRMapping(src_mode, dst_mode);
// }

} // end of namespace arm
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif /* __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_STATUS_REGISTER_HH__ */
