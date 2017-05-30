/*
 *  Copyright (c) 2017,
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

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_E200Z710N3_TYPES_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_E200Z710N3_TYPES_HH__

#include <unisim/component/cxx/processor/powerpc/cpu.hh>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace powerpc {
namespace e200z710n3 {

class CPU;

typedef uint32_t ADDRESS;
typedef uint32_t PHYSICAL_ADDRESS;
const unsigned int FSB_WIDTH = 8;
const bool DEBUG_ENABLE = false;
const bool HAS_DATA_CACHE = true;

enum STORAGE_ATTR
{
	SA_DEFAULT = 0,           // not cache inhibited and not guarded
	SA_I       = 1,           // cache inhibited
	SA_G       = 2,           // guarded
	SA_IG      = SA_I | SA_G, // cache inhibited and guarded
};

struct CONFIG
{
	typedef CPU STATE;

	static const Model MODEL = E200Z710N3;
	static const unsigned int NUM_EXCEPTIONS = 32;
	static const unsigned int FSB_WIDTH = unisim::component::cxx::processor::powerpc::e200z710n3::FSB_WIDTH;
	static const bool DEBUG_ENABLE = unisim::component::cxx::processor::powerpc::e200z710n3::DEBUG_ENABLE;
	static const unsigned int MSR_SPV = 38 - 32;
	static const uint32_t MSR_SPV_MASK = ((uint32_t) (1U)) << MSR_SPV;

	// High-Word Error Bits
	static const unsigned int SPEFSCR_FINVH = 36 - 32;
	static const uint32_t SPEFSCR_FINVH_MASK = ((uint32_t) (1U)) << SPEFSCR_FINVH;
	static const unsigned int SPEFSCR_FDBZH = 37 - 32;
	static const uint32_t SPEFSCR_FDBZH_MASK = ((uint32_t) (1U)) << SPEFSCR_FDBZH;
	static const unsigned int SPEFSCR_FUNFH = 38 - 32;
	static const uint32_t SPEFSCR_FUNFH_MASK = ((uint32_t) (1U)) << SPEFSCR_FUNFH;
	static const unsigned int SPEFSCR_FOVFH = 39 - 32;
	static const uint32_t SPEFSCR_FOVFH_MASK = ((uint32_t) (1U)) << SPEFSCR_FOVFH;

	// Status Bits
	static const unsigned int SPEFSCR_FINXS = 42 - 32;
	static const uint32_t SPEFSCR_FINXS_MASK = ((uint32_t) (1U)) << SPEFSCR_FINXS;

	static const unsigned int SPEFSCR_FG = 50 - 32;
	static const uint32_t SPEFSCR_FG_MASK = ((uint32_t) (1U)) << SPEFSCR_FG;
	static const unsigned int SPEFSCR_FX = 51 - 32;
	static const uint32_t SPEFSCR_FX_MASK = ((uint32_t) (1U)) << SPEFSCR_FX;
	static const unsigned int SPEFSCR_FINV = 52 - 32;
	static const uint32_t SPEFSCR_FINV_MASK = ((uint32_t) (1U)) << SPEFSCR_FINV;
	static const unsigned int SPEFSCR_FDBZ = 53 - 32;
	static const uint32_t SPEFSCR_FDBZ_MASK = ((uint32_t) (1U)) << SPEFSCR_FDBZ;
	static const unsigned int SPEFSCR_FUNF = 54 - 32;
	static const uint32_t SPEFSCR_FUNF_MASK = ((uint32_t) (1U)) << SPEFSCR_FUNF;
	static const unsigned int SPEFSCR_FOVF = 55 - 32;
	static const uint32_t SPEFSCR_FOVF_MASK = ((uint32_t) (1U)) << SPEFSCR_FOVF;

	// Enable Bits
	static const unsigned int SPEFSCR_FINXE = 57 - 32;
	static const uint32_t SPEFSCR_FINXE_MASK = ((uint32_t) (1U)) << SPEFSCR_FINXE;
	static const unsigned int SPEFSCR_FINVE = 58 - 32;
	static const uint32_t SPEFSCR_FINVE_MASK = ((uint32_t) (1U)) << SPEFSCR_FINVE;
	static const unsigned int SPEFSCR_FDBZE = 59 - 32;
	static const uint32_t SPEFSCR_FDBZE_MASK = ((uint32_t) (1U)) << SPEFSCR_FDBZE;
	static const unsigned int SPEFSCR_FUNFE = 60 - 32;
	static const uint32_t SPEFSCR_FUNFE_MASK = ((uint32_t) (1U)) << SPEFSCR_FUNFE;
	static const unsigned int SPEFSCR_FOVFE = 61 - 32;
	static const uint32_t SPEFSCR_FOVFE_MASK = ((uint32_t) (1U)) << SPEFSCR_FOVFE;
	
	static const unsigned int SPEFSCR_FRMC_OFFSET = 62 - 32;
	static const unsigned int SPEFSCR_FRMC_LENGTH = 2;
	static const uint32_t SPEFSCR_FRMC_MASK
		= ((uint32_t) ~(~0U << (SPEFSCR_FRMC_LENGTH))) << SPEFSCR_FRMC_OFFSET;

	typedef unisim::component::cxx::processor::powerpc::e200z710n3::ADDRESS ADDRESS;
	typedef unisim::component::cxx::processor::powerpc::e200z710n3::PHYSICAL_ADDRESS PHYSICAL_ADDRESS;
	typedef unisim::component::cxx::processor::powerpc::e200z710n3::STORAGE_ATTR STORAGE_ATTR;
};

struct MSS_TYPES
{
	typedef CONFIG::PHYSICAL_ADDRESS ADDRESS;
	typedef CONFIG::STORAGE_ATTR STORAGE_ATTR;
};

struct MPU_CONFIG
{
	typedef unisim::component::cxx::processor::powerpc::e200z710n3::CPU CPU;
	typedef unisim::component::cxx::processor::powerpc::e200z710n3::ADDRESS ADDRESS;
	static const unsigned int NUM_INST_MPU_ENTRIES = 6;
	static const unsigned int NUM_DATA_MPU_ENTRIES = 12;
	static const unsigned int NUM_SHARED_MPU_ENTRIES = 6;
};

} // end of namespace e200z710n3
} // end of namespace powerpc
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim


#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_E200Z710N3_TYPES_HH__
