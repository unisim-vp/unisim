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

#include <unisim/component/cxx/processor/arm/psr.hh>
#include <unisim/component/cxx/processor/arm/isa/constants.hh>
#include <sstream>
#include <inttypes.h>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace arm {

  void PSR::Print(std::ostream& sink) const
  {
    std::ostringstream buf;
    buf << std::hex
        <<    "N=" << this->Get(N)
        <<  ", Z=" << this->Get(Z)
        <<  ", C=" << this->Get(C)
        <<  ", V=" << this->Get(V)
        <<  ", Q=" << this->Get(Q)
        << ", GE=" << this->Get(GE)
        <<  ", E=" << this->Get(E)
        <<  ", J=" << this->Get(J)
        <<  ", T=" << this->Get(T)
        << ", IT=" << this->ITGetState()
        <<  ", A=" << this->Get(A)
        <<  ", I=" << this->Get(I)
        <<  ", F=" << this->Get(F)
        <<  ", M=" << this->Get(M);
    sink << buf.str();
  }

} // end of namespace arm
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim
