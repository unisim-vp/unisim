/*
 *  Copyright (c) 2009,
 *  University of Perpignan (UPVD),
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
 *   - Neither the name of UPVD nor the names of its contributors may be used to
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
 * Authors: Sylvain Collange (sylvain.collange@univ-perp.fr)
 */

#ifndef UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_OPERATION_TCC
#define UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_OPERATION_TCC

#include <unisim/component/cxx/processor/tesla/operation.hh>
#include <unisim/component/cxx/processor/tesla/tesla_opcode.hh>
#include <unisim/component/cxx/processor/tesla/tesla_dest.hh>
#include <unisim/component/cxx/processor/tesla/tesla_src1.hh>
#include <unisim/component/cxx/processor/tesla/tesla_src2.hh>
#include <unisim/component/cxx/processor/tesla/tesla_src3.hh>
#include <unisim/component/cxx/processor/tesla/tesla_control.hh>


namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace tesla {

template <class CONFIG>
isa::src1::Decoder<CONFIG> Operation<CONFIG>::src1_decoder;

template <class CONFIG>
isa::src2::Decoder<CONFIG> Operation<CONFIG>::src2_decoder;

template <class CONFIG>
isa::src3::Decoder<CONFIG> Operation<CONFIG>::src3_decoder;

template <class CONFIG>
isa::dest::Decoder<CONFIG> Operation<CONFIG>::dest_decoder;

template <class CONFIG>
isa::control::Decoder<CONFIG> Operation<CONFIG>::control_decoder;

template <class CONFIG>
Operation<CONFIG>::Operation(typename CONFIG::address_t addr, typename CONFIG::insn_t iw) :
	addr(addr), iw(iw)
{
	src1 = src1_decoder.Decode(addr, iw);
	src2 = src2_decoder.Decode(addr, iw);
	src3 = src3_decoder.Decode(addr, iw);
	dest = dest_decoder.Decode(addr, iw);
	control = control_decoder.Decode(addr, iw);
}

template <class CONFIG>
Operation<CONFIG>::~Operation()
{
}


} // end of namespace tesla
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif