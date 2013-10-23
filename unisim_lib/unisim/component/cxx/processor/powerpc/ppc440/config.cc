/*
 *  Copyright (c) 2007,
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
 * Authors: Gilles Mouchard (gilles.mouchard@cea.fr)
 */

#include <unisim/component/cxx/processor/powerpc/ppc440/config.hh>

#ifdef powerpc
#undef powerpc
#endif

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace powerpc {
namespace ppc440 {

const uint32_t TLBELayout::TLBE0_EPN_MASK;
const uint32_t TLBELayout::TLBE0_V_MASK;
const uint32_t TLBELayout::TLBE0_TS_MASK;
const uint32_t TLBELayout::TLBE0_SIZE_MASK;
const uint32_t TLBELayout::TLBE0_TPAR_MASK;

const uint32_t TLBELayout::TLBE1_RPN_MASK;
const uint32_t TLBELayout::TLBE1_PAR1_MASK;
const uint32_t TLBELayout::TLBE1_ERPN_MASK;

const uint32_t TLBELayout::TLBE2_PAR2_MASK;
const uint32_t TLBELayout::TLBE2_U0_MASK;
const uint32_t TLBELayout::TLBE2_U1_MASK;
const uint32_t TLBELayout::TLBE2_U2_MASK;
const uint32_t TLBELayout::TLBE2_U3_MASK;
const uint32_t TLBELayout::TLBE2_W_MASK;
const uint32_t TLBELayout::TLBE2_I_MASK;
const uint32_t TLBELayout::TLBE2_M_MASK;
const uint32_t TLBELayout::TLBE2_G_MASK;
const uint32_t TLBELayout::TLBE2_E_MASK;
const uint32_t TLBELayout::TLBE2_STORAGE_ATTR_MASK;
const uint32_t TLBELayout::TLBE2_UX_MASK;
const uint32_t TLBELayout::TLBE2_UW_MASK;
const uint32_t TLBELayout::TLBE2_UR_MASK;
const uint32_t TLBELayout::TLBE2_SX_MASK;
const uint32_t TLBELayout::TLBE2_SW_MASK;
const uint32_t TLBELayout::TLBE2_SR_MASK;
const uint32_t TLBELayout::TLBE2_ACCESS_CTRL_MASK;

const uint32_t TLBELayout::TLBE0_TAG_MASK;

const uint32_t TLBELayout::TLBE0_MASK;

const uint32_t TLBELayout::TLBE1_DATA_MASK;

const uint32_t TLBELayout::TLBE1_MASK;

const uint32_t TLBELayout::TLBE2_DATA_MASK;

const uint32_t TLBELayout::TLBE2_MASK;

const uint32_t Config::EXC_EXTERNAL_INPUT;
const uint32_t Config::EXC_CRITICAL_INPUT;
const unsigned int Config::EXC_DATA_STORAGE_READ_ACCESS_CONTROL;
const unsigned int Config::EXC_DATA_STORAGE_WRITE_ACCESS_CONTROL;

} // end of namespace ppc440
} // end of namespace powerpc
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim
