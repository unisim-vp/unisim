/*
 *  Copyright (c) 2007-2017,
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
 
#include <unisim/util/symbolic/symbolic.hh>
#include <unisim/util/endian/endian.hh>
#include <iostream>

namespace unisim {
namespace util {
namespace symbolic {

  std::ostream& ConstNodeBase::warn() { return std::cerr; }
  
  double BinaryXor( double l, double r ) { throw std::logic_error( "No ^ for double." ); }
  float BinaryXor( float l, float r ) { throw std::logic_error( "No ^ for float." ); }
  
  double BinaryAnd( double l, double r ) { throw std::logic_error( "No & for double." ); }
  float BinaryAnd( float l, float r ) { throw std::logic_error( "No & for float." ); }
  
  double BinaryOr( double l, double r ) { throw std::logic_error( "No | for double." ); }
  float BinaryOr( float l, float r ) { throw std::logic_error( "No | for float." ); }
  
  bool BinaryNot( bool val ) { return not val; }
  double BinaryNot( double val ) { throw std::logic_error( "No ~ for double." ); }
  float BinaryNot( float val ) { throw std::logic_error( "No ~ for float." ); }
  
  double BinarySHL( double, uint8_t ) { throw std::logic_error( "No << for double." ); }
  float BinarySHL( float, uint8_t ) { throw std::logic_error( "No << for float." ); }
  
  double BinarySHR( double, uint8_t ) { throw std::logic_error( "No >> for double." ); }
  float BinarySHR( float, uint8_t ) { throw std::logic_error( "No >> for float." ); }
  
  uint32_t BSwp( uint32_t v ) { return unisim::util::endian::ByteSwap( v ); }
  uint16_t BSwp( uint16_t v ) { return unisim::util::endian::ByteSwap( v ); }
  

} /* end of namespace symbolic */
} /* end of namespace util */
} /* end of namespace unisim */

