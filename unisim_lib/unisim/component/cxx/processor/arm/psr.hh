/*
 *  Copyright (c) 2013-2023,
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
 * Authors: Yves Lhuillier (yves.lhuillier@cea.fr), Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 */

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_PSR_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_PSR_HH__

#include <unisim/util/arithmetic/bitfield.hh>
#include <iosfwd>
#include <inttypes.h>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace arm {

  struct PSR
  {
    template <typename RF>  uint32_t     Get( RF const& rf ) const { return rf.Get( m_value ); }
    template <typename RF>  void  Set( RF const& rf, uint32_t value ) { return rf.Set( m_value, value ); }
    uint32_t& bits() { return m_value; }

    void ITSetState( uint32_t cond, uint32_t mask )
    {
      unisim::util::arithmetic::BitField<12,4>().Set( m_value, cond );
      unisim::util::arithmetic::BitField<10,2>().Set( m_value, (mask >> 2) & 3 );
      unisim::util::arithmetic::BitField<25,2>().Set( m_value, (mask >> 0) & 3 );
    }
    
    uint32_t ITGetState() const
    {
      return (unisim::util::arithmetic::BitField<10,6>().Get( m_value ) << 2) | unisim::util::arithmetic::BitField<25,2>().Get( m_value );
    }
  
    bool InITBlock() const
    { return unisim::util::arithmetic::BitField<10,2>().Get( m_value ) or unisim::util::arithmetic::BitField<25,2>().Get( m_value ); }
  
    uint32_t ITGetCondition() const
    { return this->InITBlock() ? unisim::util::arithmetic::BitField<12,4>().Get( m_value ) : 14; }
  
    void ITAdvance()
    {
      uint32_t state = (unisim::util::arithmetic::BitField<10,6>().Get( m_value ) << 2) | unisim::util::arithmetic::BitField<25,2>().Get( m_value );
      state = (state & 7) ? ((state & -32) | ((state << 1) & 31)) : 0;
      unisim::util::arithmetic::BitField<10,6>().Set( m_value, state >> 2 );
      unisim::util::arithmetic::BitField<25,2>().Set( m_value, state & 3 );
    }

    void Print(std::ostream& sink) const;
    friend std::ostream& operator << (std::ostream& sink , PSR const& psr) { psr.Print(sink); return sink; }
    
    uint32_t m_value;
  };

} // end of namespace arm
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif /* __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_PSR_HH__ */
