/*
 *  Copyright (c) 2025,
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

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_CFG_AARCH32_AARCH32_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_CFG_AARCH32_AARCH32_HH__

#include <unisim/component/cxx/processor/arm/isa/arm32/arm32.hh>
#include <unisim/component/cxx/processor/arm/isa/thumb2/thumb.hh>
#include <unisim/util/cfg/intro/intro.hh>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace arm {
namespace cfg {
namespace aarch32 {

template <bool IS_THUMB>
struct BranchAnalyzer
{
  void GetInfo(unisim::util::cfg::intro::BranchInfo& branch, uint32_t insn_addr, uint32_t insn, unsigned insn_length, uint8_t it_cond);
};

template <class Insn>
void ComputeBranchInfo(Insn* op, uint8_t it_cond)
{
  if (not op->branch.startupdate())
    return; // Already computed

  enum { is_thumb = Insn::decoder_type::minsize == 16 };
  return BranchAnalyzer<is_thumb>().GetInfo(op->branch, op->GetAddr(), op->GetEncoding(), op->GetLength(), it_cond);
}

} // end of namespace aarch32
} // end of namespace cfg
} // end of namespace arm
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_CFG_AARCH32_AARCH32_HH__
