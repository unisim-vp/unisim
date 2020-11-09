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
 
#include <hw/console.hh>
#include <utils/beaccess.hh>
#include <iostream>
#include <cstring>

namespace SSv8 {
  Console::Console()
    : Peripheral( 0x80000100, 0x8000010f ), ctrl(0)
  {}
  
  /*
   * uart { data:32, status:32, control:32, scaler:32 }
   *
   * Data register allow to read/write from/to console (big endian, 8-bits).
   * 
   */

  bool
  Console::read( uint32_t addr, uint32_t size, uint8_t* value )
  {
    if (addr == 0x80000100 and size == 4) // data register
      {
        char ch;
        if (not std::cin.get(ch)) { struct Bad {}; throw Bad (); }
        *((uint32_t*)value) = 0;
        value[3] = ch;
        return true;
      }
    if (addr == 0x80000104 and size == 4) // status register
      {
        // bit 2: fifo empty
        // bit 9: fifo full
        static const uint8_t status_ready[4] = {0,0,0,5};
        std::copy(&status_ready[0], &status_ready[4], value);
        return true;
      }
    if (addr == 0x80000108 and size == 4) // control register
      {
        SSv8::be_write( value, ctrl );
        return true;
      }

    std::cerr << "Console: unimplemented read @" << std::hex << addr << ":" << size << std::endl;
    return false;
  }
  bool
  Console::write( uint32_t addr, uint32_t size, uint8_t const* value )
  {
    if (addr == 0x80000100 and size == 4) // data register
      {
        std::cout << char( value[3] );
        return true;
      }
    if (addr == 0x80000108 and size == 4) // control register
      {
        return true;
      }
    
    std::cerr << "Console: unimplemented Write @" << std::hex << addr << ":" << size << std::endl;
    return true;
  };
};
