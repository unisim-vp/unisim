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
 
#include <hw/pnp.hh>
#include <iostream>

#define PNP(VENDORID, DEVICEID, VERSION, IRQ) (((VENDORID & 0x00ff) << 24) | ((DEVICEID & 0x0fff) << 12) | ((VERSION & 0x001f) <<  5) | ((IRQ & 0x001f) <<  0))
#define ADDR( VALUE )      ((VALUE & 0x0fff) << 20)
#define PREFETCHABLE       (0x01 << 17)
#define CACHEABLE          (0x01 << 16)
#define MASK( VALUE )      ((VALUE & 0x0fff) <<  4)
#define TYPE( VALUE )      ((VALUE & 0x000f) <<  0)

namespace SSv8
{

  void
  PNP::setreg( uint32_t _value )
  {
    m_word[0] = (_value >> 24) & 0xff;
    m_word[1] = (_value >> 16) & 0xff;
    m_word[2] = (_value >>  8) & 0xff;
    m_word[3] = (_value >>  0) & 0xff;
  }
 
  bool
  AHBPNP::getreg( uint32_t _idx )
  {
    uint32_t unit = _idx / 8, reg = _idx % 8;
    switch (unit)
      {
      default: // Unallocated entry
        setreg(0);
        return true;
    
      case 64: // European Space Agency; Leon2 Memory Controller
        switch (reg)
          {
          case 0: setreg( PNP( 4, 15, 1, 0) ); return true;
          default:
            std::cerr << "Warning: unimplemented AHB.PNP.entry.reg["<< reg<<"] for Leon2 Memory Controller.\n";
            setreg( 0 );
            return true;
          }
        break;
        
      case 65: // Gaisler Research, AHB/APB Bridge
        switch (reg)
          {
          case 0: setreg( PNP( 1, 6, 0, 0) ); return true;
          case 4: setreg( ADDR( 0x0800 ) | MASK( 0xfff ) | TYPE( 2 ) ); return true;
          default:
            std::cerr << "Warning: unimplemented AHB.PNP.entry.reg["<< reg<<"] for AHB/APB Bridge.\n";
            setreg( 0 );
            return true;
          }
        break;
      }
    
    std::cerr << "Warning AHB.PNP unimplemented access {unit=" << unit << ", reg=" << reg << "}\n";
    return false;
  }
  
  bool
  APBPNP::getreg( uint32_t _idx )
  {
    uint32_t unit = _idx / 2, reg = _idx % 2;
    switch (unit)
      {
      default: /* Unallocated entry */
        setreg(0);
        return true;
    
      case 0: // European Space Agency; Leon2 Memory Controller
        switch (reg)
          {
          case 0: setreg( PNP( 4, 15, 1, 0) ); return true;
          default:
            std::cerr << "Warning: unimplemented APB.PNP.entry.reg["<< reg<<"] for Leon2 Memory Controller.\n";
            setreg( 0 );
            return true;
          }
        break;
        
      case 1: // Gaisler Research, Generic UART
        switch (reg)
          {
          case 0: setreg( PNP( 1, 12, 1, 2) ); return true;
          case 1: setreg( ADDR( 0x0001 ) | MASK( 0xfff ) | TYPE( 1 ) ); return true;
          default:
            std::cerr << "Warning: unimplemented APB.PNP.entry.reg["<< reg<<"] for Generic UART.\n";
            setreg( 0 );
            return true;
          }
        break;
        
      // case 2: // Gaisler Research, Multi-processor Interrupt Ctrl.
      //   switch( reg ) {
      //   case 0: setreg( PNP( 1, 13, 3, 0) ); return true;
      //   case 1: setreg( ADDR( 0x0002 ) | MASK( 0xfff ) | TYPE( 1 ) ); return true;
      //   }
      //   break;
      // case 3: // Gaisler Research, Modular Timer Unit
      //   switch( reg ) {
      //   case 0: setreg( PNP( 1, 17, 0, 8) ); return true;
      //   case 1: setreg( ADDR( 0x0003 ) | MASK( 0xfff ) | TYPE( 1 ) ); return true;
      //   }
      //   break;
      }
    
    std::cerr << "Warning APB PNP access {unit=" << unit << ", reg=" << reg << "}\n";
    return false;
  }
};

#undef VENDORID
#undef DEVICEID
#undef VERSION
#undef IRQ
#undef ADDR
#undef PREFETCHABLE
#undef CACHEABLE
#undef MASK
#undef TYPE
