/*
 *  Copyright (c) 2007-2020,
 *  Commissariat a l'Energie Atomique (CEA),
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
 
/* SSv8::Controller templated methods (-*- C++ -*-) */
#ifndef SPARCISS_HW_CONTROLLER_TCC
#define SPARCISS_HW_CONTROLLER_TCC

#include <iostream>
#include <hw/controller.hh>
#include <unisim/component/cxx/processor/sparc/isa_sv8.tcc>
#include <utils/trace.hh>

namespace SSv8 {
  template <class ARCH>
  Controller<ARCH>::Controller()
    : m_disasm( false ), m_lastoperation( 0 ), m_lastpc( 0 )
  {
    m_arch.m_trap.clear();
    // memset( m_gpr, 0, sizeof( m_gpr ) );
  }

  template <class ARCH>
  void
  Controller<ARCH>::dumptrap( ostream& _sink ) {
    _sink << "! Trap " << m_arch.m_trap.m_name << " (" << hex << uint32_t( m_arch.m_trap.m_traptype ) << ")\n";
    _sink << "  Instruction @" << hex << m_arch.m_pc << ": ";
    m_lastoperation->disasm( _sink, m_arch.m_pc );
    _sink << endl;
  }

  template <class ARCH>
  void
  Controller<ARCH>::take_trap()
  {
    //    this->dumptrap( Trace::chan( m_disasm ? "cpu" : "trap") );
    if( m_arch.et() == 0 ) {
      Trace::chan( m_disasm ? "cpu" : "trap" ) << "! Fatal Error\n" << endl;
      m_arch.m_execute_mode = false;
      return;
    }
    m_arch.tt() = m_arch.m_trap.m_traptype;
    m_arch.m_trap.clear();
      
    m_arch.et() = 0;
    m_arch.ps() = m_arch.s();
    m_arch.s() = 1;
    m_arch.rotate( -1 );
    if( not m_arch.m_annul ) {
      m_arch.m_gpr[17] = m_arch.m_pc;
      m_arch.m_gpr[18] = m_arch.m_npc;
    } else {
      m_arch.m_gpr[17] = m_arch.m_npc;
      m_arch.m_gpr[18] = m_arch.m_npc + 4;
      m_arch.m_annul = false;
    }
    m_arch.m_pc = m_arch.m_tbr;
    m_arch.m_npc = m_arch.m_pc + 4;
    m_arch.m_nnpc = m_arch.m_pc + 8;
  }
  
  template <class ARCH>
  void
  Controller<ARCH>::step()
  {
    // Handling traps
    if (m_arch.m_trap)
      this->take_trap();

    m_arch.step()
    
  }
  
  template <class ARCH>
  void
  Controller<ARCH>::Fetch( void *_buffer, uint32_t _addr, uint32_t _size ) {
//     uint8_t const* storage = m_arch.read( m_arch.super() ? SSv8::supervisor_instruction : SSv8::user_instruction, m_arch.m_pc, 4 );
//     if( not storage )
//       m_arch.hwtrap( Trap_t::instruction_access_exception );
//     *((uint32_t*)_buffer) = *((uint32_t*)storage);
  }
}; // end of namespace SSv8

#endif // SPARCISS_HW_CONTROLLER_TCC
