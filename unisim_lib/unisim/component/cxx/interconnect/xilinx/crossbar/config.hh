/*
 *  Copyright (c) 2010-2011,
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
 
#ifndef __UNISIM_COMPONENT_CXX_INTERCONNECT_XILINX_CROSSBAR_CONFIG_HH__
#define __UNISIM_COMPONENT_CXX_INTERCONNECT_XILINX_CROSSBAR_CONFIG_HH__

#include <unisim/kernel/service/service.hh>
#include <unisim/kernel/logger/logger.hh>

namespace unisim {
namespace component {
namespace cxx {
namespace interconnect {
namespace xilinx {
namespace crossbar {

class DCRMap
{
public:
	static const uint32_t DCR_LOW          = 0x20;
	static const uint32_t DCR_HIGH         = 0x5f;
	
	static const uint32_t IST              = 0x20;
	static const uint32_t IMASK            = 0x21;
	static const uint32_t ARB_XBC          = 0x23;
	static const uint32_t FIFOST_XBC       = 0x24;
	static const uint32_t SM_ST_XBC        = 0x25;
	static const uint32_t MISC_XBC         = 0x26;
	static const uint32_t ARB_XBM          = 0x28;
	static const uint32_t FIFOST_XBM       = 0x29;
	static const uint32_t MISC_XBM         = 0x2b;
	static const uint32_t TMPL0_XBAR_MAP   = 0x2d;
	static const uint32_t TMPL1_XBAR_MAP   = 0x2e;
	static const uint32_t TMPL2_XBAR_MAP   = 0x2f;
	static const uint32_t TMPL3_XBAR_MAP   = 0x30;
	static const uint32_t TMPL_SEL_REG     = 0x31;
	static const uint32_t CFG_PLBS0        = 0x34;
	static const uint32_t SEAR_U_PLBS0     = 0x36;
	static const uint32_t SEAR_L_PLBS0     = 0x37;
	static const uint32_t SESR_PLBS0       = 0x38;
	static const uint32_t MISC_ST_PLBS0    = 0x39;
	static const uint32_t PLBERR_ST_PLBS0  = 0x3a;
	static const uint32_t SM_ST_PLBS0      = 0x3b;
	static const uint32_t MISC_PLBS0       = 0x3c;
	static const uint32_t CMD_SNIFF_PLBS0  = 0x3d;
	static const uint32_t CMD_SNIFFA_PLBS0 = 0x3e;
	static const uint32_t TMPL0_PLBS0_MAP  = 0x40;
	static const uint32_t TMPL1_PLBS0_MAP  = 0x41;
	static const uint32_t TMPL2_PLBS0_MAP  = 0x42;
	static const uint32_t TMPL3_PLBS0_MAP  = 0x43;
	static const uint32_t CFG_PLBS1        = 0x44;
	static const uint32_t SEAR_U_PLBS1     = 0x46;
	static const uint32_t SEAR_L_PLBS1     = 0x47;
	static const uint32_t SESR_PLBS1       = 0x48;
	static const uint32_t MISC_ST_PLBS1    = 0x49;
	static const uint32_t PLBERR_ST_PLBS1  = 0x4a;
	static const uint32_t SM_ST_PLBS1      = 0x4b;
	static const uint32_t MISC_PLBS1       = 0x4c;
	static const uint32_t CMD_SNIFF_PLBS1  = 0x4d;
	static const uint32_t CMD_SNIFFA_PLBS1 = 0x4e;
	static const uint32_t TMPL0_PLBS1_MAP  = 0x50;
	static const uint32_t TMPL1_PLBS1_MAP  = 0x51;
	static const uint32_t TMPL2_PLBS1_MAP  = 0x52;
	static const uint32_t TMPL3_PLBS1_MAP  = 0x53;
	static const uint32_t CFG_PLBM         = 0x54;
	static const uint32_t FSEAR_U_PLBM     = 0x56;
	static const uint32_t FSEAR_L_PLBM     = 0x57;
	static const uint32_t FSESR_PLBM       = 0x58;
	static const uint32_t MISC_ST_PLBM     = 0x59;
	static const uint32_t PLBERR_ST_PLBM   = 0x5a;
	static const uint32_t SM_ST_PLBM       = 0x5b;
	static const uint32_t MISC_PLBM        = 0x5c;
	static const uint32_t CMD_SNIFF_PLBM   = 0x5d;
	static const uint32_t CMD_SNIFFA_PLBM  = 0x5e;
};

class Config
	: public DCRMap
{
public:
	typedef uint64_t ADDRESS;
	static const unsigned int PLB_WIDTH = 16; // PLB Width in bytes
	static const unsigned int MCI_WIDTH = 16; // PLB Width in bytes
};

} // end of namespace crossbar
} // end of namespace xilinx
} // end of namespace interconnect
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_INTERCONNECT_XILINX_CROSSBAR_CONFIG_HH__
