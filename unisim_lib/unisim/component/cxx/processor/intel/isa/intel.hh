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

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_INTEL_ISA_INTEL_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_INTEL_ISA_INTEL_HH__

#include <typeinfo>
#include <inttypes.h>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace intel {

  struct Mode
  {
    // TODO: should handle non protected mode (where cs_l, cs_d and ss_b are ignored)
    uint8_t cs_l     : 1; // 64-bit modes
    uint8_t cs_d     : 1; // default size
    uint8_t ss_b     : 1; // stack address size
    uint8_t pad      : 5;
    
    Mode() : cs_l( 0 ), cs_d( 0 ), ss_b( 0 ), pad( 0 ) {}
    Mode( uint8_t _csl, uint8_t _csd, uint8_t _ssb ) : cs_l( _csl ), cs_d( _csd ), ss_b( _ssb ), pad(0) {}
    
    bool operator == ( Mode const& m ) const { return (cs_l == m.cs_l) and (cs_d == m.cs_d) and (ss_b == m.ss_b); }
  };
  
  struct OpBase
  {
    uint32_t address;
    Mode     mode;
    uint8_t  length;
    
    OpBase( uint32_t _address, Mode _mode, uint8_t _length ) : address( _address ), mode( _mode ), length( _length ) {}
  };
  
  struct Operation : public OpBase
  {
    Operation( OpBase const& _opbase ) : OpBase( _opbase ) {}
    virtual ~Operation() {}
    virtual void disasm( std::ostream& _sink ) const = 0;
    virtual void execute( Arch& arch ) const
    {
      std::cerr << "error: no execute method for `";
      this->disasm( std::cerr );
      std::cerr << "' in " << (typeid(*this).name()) << "\n";
      throw 0;
    };
  };

  struct CodeBase
  {
    uint8_t const* bytes;
    uint32_t address;
    Mode     mode;
    
    /*** PreDecoded ***/
    uint32_t adsz_67  : 1; /* grp4 */
    uint32_t opsz_66  : 1; /* grp3 */
    uint32_t lock_f0  : 1; /* grp1.lock */
    uint32_t segment  : 3; /* grp2 {0:ES, 1:CS, 2:SS, 3:DS, 4:FS, 5:GS}*/
    uint32_t rep      : 2; /* grp1.rep {0: None, 2: F2, 3: F3} */
    uint32_t opc_idx  : 4;
    
    CodeBase( Mode _mode, uint32_t _address, uint8_t const* _bytes )
      : bytes( _bytes ), address( _address ), mode( _mode ),
        adsz_67( 0 ), opsz_66( 0 ), lock_f0( 0 ), segment( DS ), rep( 0 ), opc_idx( 0 )
    {
      uint8_t const* bptr = _bytes;
      for (bool inprfx = true; inprfx; ++bptr)
        {
          switch (*bptr) {
          default: inprfx = false; opc_idx = (bptr - _bytes); break;
            // Group1::lock
          case 0xf0: lock_f0 = true; break;
            // Group1::repeat
          case 0xf2: rep = 2; break;
          case 0xf3: rep = 3; break;
            // Group2 (segments)
          case /*046*/ 0x26: segment = ES; break;
          case /*056*/ 0x2e: segment = CS; break;
          case /*066*/ 0x36: segment = SS; break;
          case /*076*/ 0x3e: segment = DS; break;
          case /*144*/ 0x64: segment = FS; break;
          case /*145*/ 0x65: segment = GS; break;
            // Group3 (alternative operand size)
          case 0x66: opsz_66 = true; break;
            // Group4 (alternative address size)
          case 0x67: adsz_67 = true; break;
          }
        }
    }
    
    unsigned opsize() const { return opsz_66 ? 16 : 32; }
    unsigned addrsize() const { return adsz_67 ? 16 : 32; }
    bool f0() const { return lock_f0 == 1; }
    bool f3() const { return rep == 3; }
    bool f2() const { return rep == 2; }
    bool _66() const { return opsz_66; }
    bool mode64() const { return mode.cs_l != 0; }
  };
  
} // end of namespace intel
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_INTEL_ISA_INTEL_HH__