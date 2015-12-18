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

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_INTEL_MODRM_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_INTEL_MODRM_HH__

#include <unisim/component/cxx/processor/intel/types.hh>
#include <unisim/component/cxx/processor/intel/segments.hh>
#include <unisim/component/cxx/processor/intel/disasm.hh>
#include <unisim/component/cxx/processor/intel/fwd.hh>
#include <iosfwd>
#include <iostream>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace intel {

  struct MOp
  {
    MOp( uint8_t _segment ) : segment( _segment ) {} uint8_t segment;
    virtual ~MOp() {}
    virtual void  disasm_memory_operand( std::ostream& _sink ) const { throw 0; };
    virtual u32_t effective_address( Arch& _arch ) const { throw 0; return u32_t( 0 ); };
  };
  
  struct _RMOp
  {
    MOp const* mop;
    _RMOp( MOp const* _mop ) : mop( _mop ) {}
    _RMOp( _RMOp const& _rmop ) : mop( _rmop.mop ) {}
    bool is_memory_operand() const { return (uintptr_t( mop ) > 0x1000); }
    MOp const* operator -> () const { return mop; }
    unsigned ereg() const { return unsigned( uintptr_t( mop ) ); }
    operator MOp const* () const { return mop; }
  };
    
  struct RMOp : public _RMOp
  {
    RMOp( MOp* _mop ) : _RMOp( _mop ) {}
    RMOp( RMOp const& ) = delete;
    RMOp( _RMOp const& ) = delete;
    ~RMOp() { if (is_memory_operand()) delete mop; }
  };
  
} // end of namespace intel
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_INTEL_MODRM_HH__
