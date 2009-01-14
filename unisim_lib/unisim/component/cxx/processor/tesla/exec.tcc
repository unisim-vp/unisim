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

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_EXEC_TCC__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_EXEC_TCC__

#include <unisim/component/cxx/processor/tesla/exec.hh>

template <class CONFIG>
VectorRegister<CONFIG> CPU<CONFIG>::FSMad(VectorRegister<CONFIG> const & a, VectorRegister<CONFIG> const & b,
                     VectorRegister<CONFIG> const & c, uint32_t rounding_mode)
{
}

template <class CONFIG>
VectorRegister<CONFIG> CPU<CONFIG>::FSMul(VectorRegister<CONFIG> const & a, VectorRegister<CONFIG> const & b,
                     uint32_t rounding_mode)
{
}

template <class CONFIG>
VectorRegister<CONFIG> CPU<CONFIG>::FSAdd(VectorRegister<CONFIG> const & a, VectorRegister<CONFIG> const & b,
                     uint32_t rounding_mode)
{
}

template <class CONFIG>
void CPU<CONFIG>::FSNegate(VectorRegister<CONFIG> & a)
{
}

template<class CONFIG>
VectorRegister<CONFIG> CPU<CONFIG>::I32Mad24(VectorRegister<CONFIG> const & a, VectorRegister<CONFIG> const & b,
                     VectorRegister<CONFIG> const & c, uint32_t sat, uint32_t ra, uint32_t rb,
                     uint32_t rc)
{
}

template<class CONFIG>
VectorRegister<CONFIG> CPU<CONFIG>::I16Mad24Lo(VectorRegister<CONFIG> const & a, VectorRegister<CONFIG> const & b,
                     VectorRegister<CONFIG> const & c, uint32_t sat, uint32_t ra, uint32_t rb,
                     uint32_t rc)
{
}

template<class CONFIG>
VectorRegister<CONFIG> CPU<CONFIG>::I32Mul24(VectorRegister<CONFIG> const & a, VectorRegister<CONFIG> const & b,
                     uint32_t sat, uint32_t ra, uint32_t rb)
{
}

template<class CONFIG>
VectorRegister<CONFIG> CPU<CONFIG>::I16Mul(VectorRegister<CONFIG> const & a, VectorRegister<CONFIG> const & b,
                     uint32_t sat, uint32_t ra, uint32_t rb)
{
}

template<class CONFIG>
VectorRegister<CONFIG> CPU<CONFIG>::I32Add(VectorRegister<CONFIG> const & a, VectorRegister<CONFIG> const & b,
                     int & carry, int & ovf,
                     uint32_t sat, uint32_t ra, uint32_t rb)
{
}


template <class CONFIG>
void CPU<CONFIG>::I32Negate(VectorRegister<CONFIG> & a)
{
}

template <class CONFIG>
VectorRegister<CONFIG> CPU<CONFIG>::Convert(VectorRegister<CONFIG> & a, uint32_t cvt_round, uint32_t cvt_type)
{
}

template <class CONFIG>
void CPU<CONFIG>::ScatterGlobal(VecReg output, uint32_t dest, uint32_t addr_lo, uint32_t addr_hi, uint32_t addr_imm, uint32_t segment, std::bitset<CONFIG::WARP_SIZE> mask)
{
}


#endif
