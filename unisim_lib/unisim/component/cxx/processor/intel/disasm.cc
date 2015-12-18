/*
 *  Copyright (c) 2007-2015,
 *  Commissariat a l'Energie Atomique (CEA)
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without 
 *  modification, are permitted provided that the following conditions are met:
 *
 *   - Redistributions of source code must retain the above copyright notice, 
 *     this list of conditions and the following disclaimer.
 *
 *   - Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 *   - Neither the name of CEA nor the names of its contributors may be used to
 *     endorse or promote products derived from this software without specific 
 *     prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 *  ARE DISCLAIMED.
 *  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY 
 *  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND 
 *  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF 
 *  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: Yves Lhuillier (yves.lhuillier@cea.fr)
 */

#include <unisim/component/cxx/processor/intel/disasm.hh>
#include <unisim/component/cxx/processor/intel/modrm.hh>
#include <iostream>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace intel {

  std::ostream&
  operator << ( std::ostream& sink, DisasmObject const& dobj )
  {
    dobj( sink );
    return sink;
  }
  
  void
  DisasmRoM::operator() ( std::ostream& sink ) const
  {
    _RMOp rmop( mop );
    if (rmop.is_memory_operand())   rmop->disasm_memory_operand( sink );
    else                            disasm_register( sink, rmop.ereg() );
  }
  
  void DisasmEb:: disasm_register( std::ostream& sink, unsigned reg ) const { sink << DisasmRb(  reg ); }
  void DisasmEw:: disasm_register( std::ostream& sink, unsigned reg ) const { sink << DisasmRw(  reg ); }
  void DisasmEd:: disasm_register( std::ostream& sink, unsigned reg ) const { sink << DisasmRd(  reg ); }
  void DisasmQq:: disasm_register( std::ostream& sink, unsigned reg ) const { sink << DisasmRq(  reg ); }
  void DisasmWdq::disasm_register( std::ostream& sink, unsigned reg ) const { sink << DisasmRdq( reg ); }

  void DisasmM::  disasm_register( std::ostream& sink, unsigned reg ) const { sink << "(bad)"; }

  void DisasmRb::operator()  ( std::ostream& sink ) const { sink << (&"%al\0%cl\0%dl\0%bl\0%ah\0%ch\0%dh\0%bh"[(reg % 8)*4]); }
  void DisasmRw::operator()  ( std::ostream& sink ) const { sink << (&"%ax\0%cx\0%dx\0%bx\0%sp\0%bp\0%si\0%di"[(reg % 8)*4]); }
  void DisasmRd::operator()  ( std::ostream& sink ) const { sink << (&"%eax\0%ecx\0%edx\0%ebx\0%esp\0%ebp\0%esi\0%edi"[(reg % 8)*5]); }
  void DisasmRq::operator()  ( std::ostream& sink ) const { sink << "%mm" << reg; }
  void DisasmRdq::operator() ( std::ostream& sink ) const { sink << "%xmm" << reg; }

  void DisasmCd::operator()  ( std::ostream& sink ) const { sink << "%cr" << reg; }
  void DisasmDd::operator()  ( std::ostream& sink ) const { sink << "%db" << reg; }
  
  void
  DisasmS::operator () ( std::ostream& sink ) const
  {
    if (segment < 6)
      sink << (&"%es\0%cs\0%ss\0%ds\0%fs\0%gs\0"[segment*4]);
    else
      sink << "%?";
  }
  
  void
  DisasmMS::operator () ( std::ostream& sink ) const
  {
    if (segment == DS) return;
    sink << DisasmS( segment ) << ':';
  }
  
  bool has_implicit_size( MOp const* mop ) { return not _RMOp( mop ).is_memory_operand(); }
  
  void PutString( std::ostream& sink, char const* string ) { sink << string; }
  void PutChar( std::ostream& sink, char chr ) { sink << chr; }
  
  void
  DisasmFPR::operator () ( std::ostream& sink ) const
  {
    if (reg == 0) { sink << "%st"; return; }
    sink << "%st(" << unsigned( reg ) << ")";
  }
  
  void
  DisasmBytes::operator() ( std::ostream& sink ) const
  {
    char const* sep = "";
    for (unsigned idx = 0; idx < 2*length; ++idx) {
      unsigned offset = ((idx ^ 1) & 1)*4, byte = idx/2;
      if (offset) { sink << sep; sep = " "; }
      sink << ("0123456789abcdef"[(bytes[byte]>>offset)&0xf]);
    }
  }

} // end of namespace intel
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

