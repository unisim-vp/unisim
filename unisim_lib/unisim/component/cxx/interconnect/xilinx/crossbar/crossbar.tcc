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
 
#ifndef __UNISIM_COMPONENT_CXX_INTERCONNECT_XILINX_CROSSBAR_CROSSBAR_TCC__
#define __UNISIM_COMPONENT_CXX_INTERCONNECT_XILINX_CROSSBAR_CROSSBAR_TCC__

namespace unisim {
namespace component {
namespace cxx {
namespace interconnect {
namespace xilinx {
namespace crossbar {

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;

template <class CONFIG>
Crossbar<CONFIG>::Crossbar(const char *name, Object *parent)
	: Object(name, parent)
	, Service<Memory<typename CONFIG::ADDRESS> >(name, parent)
	, Client<Memory<typename CONFIG::ADDRESS> >(name, parent)
	, memory_export("memory-export", this)
	, mci_memory_import("mci-memory-import", this)
	, mplb_memory_import("mplb-memory-import", this)
	, logger(*this)
	, verbose(false)
	, param_verbose("verbose", this, verbose, "Enable/Disable verbosity")
{
}

template <class CONFIG>
Crossbar<CONFIG>::~Crossbar()
{
}

template <class CONFIG>
void Crossbar<CONFIG>::Reset()
{
}

template <class CONFIG>
bool Crossbar<CONFIG>::ReadMemory(typename CONFIG::ADDRESS addr, void *buffer, uint32_t size)
{
	Interface intf = Route(IF_DCURD_PLB, addr);
	
	switch(intf)
	{
		case IF_MCI:
			return mci_memory_import->ReadMemory(addr, buffer, size);
			break;
		case IF_MPLB:
			return mplb_memory_import->ReadMemory(addr, buffer, size);
			break;
		default:
			logger << DebugError << "Internal error" << EndDebugError;
			Object::Stop(-1);
	}
	return false;
}

template <class CONFIG>
bool Crossbar<CONFIG>::WriteMemory(typename CONFIG::ADDRESS addr, const void *buffer, uint32_t size)
{
	Interface intf = Route(IF_DCUWR_PLB, addr);
	
	switch(intf)
	{
		case IF_MCI:
			return mci_memory_import->WriteMemory(addr, buffer, size);
			break;
		case IF_MPLB:
			return mplb_memory_import->WriteMemory(addr, buffer, size);
			break;
		default:
			logger << DebugError << "Internal error" << EndDebugError;
			Object::Stop(-1);
	}
	return false;
}

template <class CONFIG>
uint32_t Crossbar<CONFIG>::ReadDCR(unsigned int dcrn)
{
	uint32_t value;

	switch(dcrn)
	{
		case CONFIG::IST: value = GetIST(); break;
		case CONFIG::IMASK: value = GetIMASK(); break;
		case CONFIG::ARB_XBC: value = GetARB_XBC(); break;
		case CONFIG::FIFOST_XBC: value = GetFIFOST_XBC(); break;
		case CONFIG::SM_ST_XBC: value = GetSM_ST_XBC(); break;
		case CONFIG::MISC_XBC: value = GetMISC_XBC(); break;
		case CONFIG::ARB_XBM: value = GetARB_XBM(); break;
		case CONFIG::FIFOST_XBM: value = GetFIFOST_XBM(); break;
		case CONFIG::MISC_XBM: value = GetMISC_XBM(); break;
		case CONFIG::TMPL0_XBAR_MAP:
		case CONFIG::TMPL1_XBAR_MAP:
		case CONFIG::TMPL2_XBAR_MAP:
		case CONFIG::TMPL3_XBAR_MAP:
			{
				unsigned int reg_num = dcrn - CONFIG::TMPL0_XBAR_MAP;
				value = GetTMPL_XBAR_MAP(reg_num);
			}
			break;
		case CONFIG::TMPL_SEL_REG: value = GetTMPL_SEL_REG(); break;
		case CONFIG::CFG_PLBS0: value = GetCFG_PLBS0(); break;
		case CONFIG::SEAR_U_PLBS0: value = GetSEAR_U_PLBS0(); break;
		case CONFIG::SEAR_L_PLBS0: value = GetSEAR_L_PLBS0(); break;
		case CONFIG::SESR_PLBS0: value = GetSESR_PLBS0(); break;
		case CONFIG::MISC_ST_PLBS0: value = GetMISC_ST_PLBS0(); break;
		case CONFIG::PLBERR_ST_PLBS0: value = GetPLBERR_ST_PLBS0(); break;
		case CONFIG::SM_ST_PLBS0: value = GetSM_ST_PLBS0(); break;
		case CONFIG::MISC_PLBS0: value = GetMISC_PLBS0(); break;
		case CONFIG::CMD_SNIFF_PLBS0: value = GetCMD_SNIFF_PLBS0(); break;
		case CONFIG::CMD_SNIFFA_PLBS0: value = GetCMD_SNIFFA_PLBS0(); break;
		case CONFIG::TMPL0_PLBS0_MAP:
		case CONFIG::TMPL1_PLBS0_MAP:
		case CONFIG::TMPL2_PLBS0_MAP:
		case CONFIG::TMPL3_PLBS0_MAP:
			{
				unsigned int reg_num = dcrn - CONFIG::TMPL0_PLBS0_MAP;
				value = GetTMPL_PLBS0_MAP(reg_num);
			}
			break;
		case CONFIG::CFG_PLBS1: value = GetCFG_PLBS1(); break;
		case CONFIG::SEAR_U_PLBS1: value = GetSEAR_U_PLBS1(); break;
		case CONFIG::SEAR_L_PLBS1: value = GetSEAR_L_PLBS1(); break;
		case CONFIG::SESR_PLBS1: value = GetSESR_PLBS1(); break;
		case CONFIG::MISC_ST_PLBS1: value = GetMISC_ST_PLBS1(); break;
		case CONFIG::PLBERR_ST_PLBS1: value = GetPLBERR_ST_PLBS1(); break;
		case CONFIG::SM_ST_PLBS1: value = GetSM_ST_PLBS1(); break;
		case CONFIG::MISC_PLBS1: value = GetMISC_PLBS1(); break;
		case CONFIG::CMD_SNIFF_PLBS1: value = GetCMD_SNIFF_PLBS1(); break;
		case CONFIG::CMD_SNIFFA_PLBS1: value = GetCMD_SNIFFA_PLBS1(); break;
		case CONFIG::TMPL0_PLBS1_MAP:
		case CONFIG::TMPL1_PLBS1_MAP:
		case CONFIG::TMPL2_PLBS1_MAP:
		case CONFIG::TMPL3_PLBS1_MAP:
			{
				unsigned int reg_num = dcrn - CONFIG::TMPL0_PLBS1_MAP;
				value = GetTMPL_PLBS1_MAP(reg_num);
			}
			break;
		case CONFIG::CFG_PLBM: value = GetCFG_PLBM(); break;
		case CONFIG::FSEAR_U_PLBM: value = GetFSEAR_U_PLBM(); break;
		case CONFIG::FSEAR_L_PLBM: value = GetFSEAR_L_PLBM(); break;
		case CONFIG::FSESR_PLBM: value = GetFSESR_PLBM(); break;
		case CONFIG::MISC_ST_PLBM: value = GetMISC_ST_PLBM(); break;
		case CONFIG::PLBERR_ST_PLBM: value = GetPLBERR_ST_PLBM(); break;
		case CONFIG::SM_ST_PLBM: value = GetSM_ST_PLBM(); break;
		case CONFIG::MISC_PLBM: value = GetMISC_PLBM(); break;
		case CONFIG::CMD_SNIFF_PLBM: value = GetCMD_SNIFF_PLBM(); break;
		case CONFIG::CMD_SNIFFA_PLBM: value = GetCMD_SNIFFA_PLBM(); break;
		default:
			logger << DebugWarning << "Reading an unmapped DCR #0x" << std::hex << dcrn << std::dec << EndDebugWarning;
			return 0;
	}
	
	if(IsVerbose())
	{
		logger << DebugInfo << "Read 0x" << std::hex << value << std::dec << " from DCR #0x" << std::hex << dcrn << std::dec << EndDebugInfo;
	}
	return value;
}

template <class CONFIG>
void Crossbar<CONFIG>::WriteDCR(unsigned int dcrn, uint32_t value)
{
	if(IsVerbose())
	{
		logger << DebugInfo << "Writing 0x" << std::hex << value << std::dec << " into DCR #0x" << std::hex << dcrn << std::dec << EndDebugInfo;
	}
	switch(dcrn)
	{
		case CONFIG::IST: SetIST(value); break;
		case CONFIG::IMASK: SetIMASK(value); break;
		case CONFIG::ARB_XBC: SetARB_XBC(value); break;
		case CONFIG::FIFOST_XBC: SetFIFOST_XBC(value); break;
		case CONFIG::SM_ST_XBC: SetSM_ST_XBC(value); break;
		case CONFIG::MISC_XBC: SetMISC_XBC(value); break;
		case CONFIG::ARB_XBM: SetARB_XBM(value); break;
		case CONFIG::FIFOST_XBM: SetFIFOST_XBM(value); break;
		case CONFIG::MISC_XBM: SetMISC_XBM(value); break;
		case CONFIG::TMPL0_XBAR_MAP:
		case CONFIG::TMPL1_XBAR_MAP:
		case CONFIG::TMPL2_XBAR_MAP:
		case CONFIG::TMPL3_XBAR_MAP:
			{
				unsigned int reg_num = dcrn - CONFIG::TMPL0_XBAR_MAP;
				SetTMPL_XBAR_MAP(reg_num, value);
			}
			break;
		case CONFIG::TMPL_SEL_REG: SetTMPL_SEL_REG(value); break;
		case CONFIG::CFG_PLBS0: SetCFG_PLBS0(value); break;
		case CONFIG::SEAR_U_PLBS0: SetSEAR_U_PLBS0(value); break;
		case CONFIG::SEAR_L_PLBS0: SetSEAR_L_PLBS0(value); break;
		case CONFIG::SESR_PLBS0: SetSESR_PLBS0(value); break;
		case CONFIG::MISC_ST_PLBS0: SetMISC_ST_PLBS0(value); break;
		case CONFIG::PLBERR_ST_PLBS0: SetPLBERR_ST_PLBS0(value); break;
		case CONFIG::SM_ST_PLBS0: SetSM_ST_PLBS0(value); break;
		case CONFIG::MISC_PLBS0: SetMISC_PLBS0(value); break;
		case CONFIG::CMD_SNIFF_PLBS0: SetCMD_SNIFF_PLBS0(value); break;
		case CONFIG::CMD_SNIFFA_PLBS0: SetCMD_SNIFFA_PLBS0(value); break;
		case CONFIG::TMPL0_PLBS0_MAP:
		case CONFIG::TMPL1_PLBS0_MAP:
		case CONFIG::TMPL2_PLBS0_MAP:
		case CONFIG::TMPL3_PLBS0_MAP:
			{
				unsigned int reg_num = dcrn - CONFIG::TMPL0_PLBS0_MAP;
				SetTMPL_PLBS0_MAP(reg_num, value);
			}
			break;
		case CONFIG::CFG_PLBS1: SetCFG_PLBS1(value); break;
		case CONFIG::SEAR_U_PLBS1: SetSEAR_U_PLBS1(value); break;
		case CONFIG::SEAR_L_PLBS1: SetSEAR_L_PLBS1(value); break;
		case CONFIG::SESR_PLBS1: SetSESR_PLBS1(value); break;
		case CONFIG::MISC_ST_PLBS1: SetMISC_ST_PLBS1(value); break;
		case CONFIG::PLBERR_ST_PLBS1: SetPLBERR_ST_PLBS1(value); break;
		case CONFIG::SM_ST_PLBS1: SetSM_ST_PLBS1(value); break;
		case CONFIG::MISC_PLBS1: SetMISC_PLBS1(value); break;
		case CONFIG::CMD_SNIFF_PLBS1: SetCMD_SNIFF_PLBS1(value); break;
		case CONFIG::CMD_SNIFFA_PLBS1: SetCMD_SNIFFA_PLBS1(value); break;
		case CONFIG::TMPL0_PLBS1_MAP:
		case CONFIG::TMPL1_PLBS1_MAP:
		case CONFIG::TMPL2_PLBS1_MAP:
		case CONFIG::TMPL3_PLBS1_MAP:
			{
				unsigned int reg_num = dcrn - CONFIG::TMPL0_PLBS1_MAP;
				SetTMPL_PLBS1_MAP(reg_num, value);
			}
			break;
		case CONFIG::CFG_PLBM: SetCFG_PLBM(value); break;
		case CONFIG::FSEAR_U_PLBM: SetFSEAR_U_PLBM(value); break;
		case CONFIG::FSEAR_L_PLBM: SetFSEAR_L_PLBM(value); break;
		case CONFIG::FSESR_PLBM: SetFSESR_PLBM(value); break;
		case CONFIG::MISC_ST_PLBM: SetMISC_ST_PLBM(value); break;
		case CONFIG::PLBERR_ST_PLBM: SetPLBERR_ST_PLBM(value); break;
		case CONFIG::SM_ST_PLBM: SetSM_ST_PLBM(value); break;
		case CONFIG::MISC_PLBM: SetMISC_PLBM(value); break;
		case CONFIG::CMD_SNIFF_PLBM: SetCMD_SNIFF_PLBM(value); break;
		case CONFIG::CMD_SNIFFA_PLBM: SetCMD_SNIFFA_PLBM(value); break;
		default:
			logger << DebugWarning << "Writing an unmapped DCR #0x" << std::hex << dcrn << std::dec << EndDebugWarning;
	}
}

template <class CONFIG>
bool Crossbar<CONFIG>::IsMappedDCR(uint32_t dcrn) const
{
	return (dcrn >= CONFIG::DCR_LOW) && (dcrn <= CONFIG::DCR_HIGH);
}

template <class CONFIG>
typename Crossbar<CONFIG>::Interface Crossbar<CONFIG>::Route(Interface intf, typename CONFIG::ADDRESS addr)
{
	// addr = UAbus[28:31] || ABus[0:4] || ABus[5:31]
	unsigned int uabus_28_31 = (addr >> 32) & 0xf;
	unsigned int tmpl_reg_num = GetTMPL_SEL_REG() >> (30 - (2 * uabus_28_31)) & 0x3; 
	
	unsigned int abus_0_4 = (addr >> 27) & 0x1f;
	
	uint32_t map;
	switch(intf)
	{
		case IF_ICURD_PLB:
		case IF_DCUWR_PLB:
		case IF_DCURD_PLB:
			map = GetTMPL_XBAR_MAP(tmpl_reg_num);
			break;
		case IF_SPLB0:
			map = GetTMPL_PLBS0_MAP(tmpl_reg_num);
			break;
		case IF_SPLB1:
			map = GetTMPL_PLBS1_MAP(tmpl_reg_num);
			break;
		default:
			logger << DebugError << "Internal error" << EndDebugError;
			Object::Stop(-1);
			return IF_MPLB;
	}
	
	return (map & (1 << (31 - abus_0_4))) ? IF_MCI : IF_MPLB;
}

template <class CONFIG>
const char *Crossbar<CONFIG>::GetInterfaceName(typename Crossbar<CONFIG>::Interface intf) const
{
	switch(intf)
	{
		case IF_ICURD_PLB: return "ICURD PLB";
		case IF_DCUWR_PLB: return "DCUWR PLB";
		case IF_DCURD_PLB: return "DCURD PLB";
		case IF_SPLB0: return "SPLB0";
		case IF_SPLB1: return "SPLB1";
		case IF_MPLB: return "MPLB";
		case IF_MCI: return "MCI";
		case IF_CROSSBAR_DCR: return "CROSSBAR DCR";
		case IF_PLBS0_DCR: return "PLBS0 DCR";
		case IF_PLBS1_DCR: return "PLBS1 DCR";
		case IF_PLBM_DCR: return "PLBM DCR";
	}
	return "?";
}

template <class CONFIG>
bool Crossbar<CONFIG>::IsVerbose() const
{
	return verbose;
}

template <class CONFIG>
uint32_t Crossbar<CONFIG>::GetIST() const
{
	return ist;
}

template <class CONFIG>
uint32_t Crossbar<CONFIG>::GetIMASK() const
{
	return imask;
}

template <class CONFIG>
uint32_t Crossbar<CONFIG>::GetARB_XBC() const
{
	return arb_xbc;
}

template <class CONFIG>
uint32_t Crossbar<CONFIG>::GetFIFOST_XBC() const
{
	return fifost_xbc;
}

template <class CONFIG>
uint32_t Crossbar<CONFIG>::GetSM_ST_XBC() const
{
	return sm_st_xbc;
}

template <class CONFIG>
uint32_t Crossbar<CONFIG>::GetMISC_XBC() const
{
	return misc_xbc;
}

template <class CONFIG>
uint32_t Crossbar<CONFIG>::GetARB_XBM() const
{
	return arb_xbm;
}

template <class CONFIG>
uint32_t Crossbar<CONFIG>::GetFIFOST_XBM() const
{
	return fifost_xbm;
}

template <class CONFIG>
uint32_t Crossbar<CONFIG>::GetMISC_XBM() const
{
	return misc_xbm;
}

template <class CONFIG>
uint32_t Crossbar<CONFIG>::GetTMPL_XBAR_MAP(unsigned int reg_num) const
{
	return tmpl_xbar_map[reg_num];
}

template <class CONFIG>
uint32_t Crossbar<CONFIG>::GetTMPL_SEL_REG() const
{
	return tmpl_sel_reg;
}

template <class CONFIG>
uint32_t Crossbar<CONFIG>::GetCFG_PLBS0() const
{
	return cfg_plbs0;
}

template <class CONFIG>
uint32_t Crossbar<CONFIG>::GetSEAR_U_PLBS0() const
{
	return sear_u_plbs0;
}

template <class CONFIG>
uint32_t Crossbar<CONFIG>::GetSEAR_L_PLBS0() const
{
	return sear_l_plbs0;
}

template <class CONFIG>
uint32_t Crossbar<CONFIG>::GetSESR_PLBS0() const
{
	return sesr_plbs0;
}

template <class CONFIG>
uint32_t Crossbar<CONFIG>::GetMISC_ST_PLBS0() const
{
	return misc_st_plbs0;
}

template <class CONFIG>
uint32_t Crossbar<CONFIG>::GetPLBERR_ST_PLBS0() const
{
	return plberr_st_plbs0;
}

template <class CONFIG>
uint32_t Crossbar<CONFIG>::GetSM_ST_PLBS0() const
{
	return sm_st_plbs0;
}

template <class CONFIG>
uint32_t Crossbar<CONFIG>::GetMISC_PLBS0() const
{
	return misc_plbs0;
}

template <class CONFIG>
uint32_t Crossbar<CONFIG>::GetCMD_SNIFF_PLBS0() const
{
	return cmd_sniff_plbs0;
}

template <class CONFIG>
uint32_t Crossbar<CONFIG>::GetCMD_SNIFFA_PLBS0() const
{
	return cmd_sniffa_plbs0;
}

template <class CONFIG>
uint32_t Crossbar<CONFIG>::GetTMPL_PLBS0_MAP(unsigned int reg_num) const
{
	return tmpl_plbs0_map[reg_num];
}

template <class CONFIG>
uint32_t Crossbar<CONFIG>::GetCFG_PLBS1() const
{
	return cfg_plbs1;
}

template <class CONFIG>
uint32_t Crossbar<CONFIG>::GetSEAR_U_PLBS1() const
{
	return sear_u_plbs1;
}

template <class CONFIG>
uint32_t Crossbar<CONFIG>::GetSEAR_L_PLBS1() const
{
	return sear_l_plbs1;
}

template <class CONFIG>
uint32_t Crossbar<CONFIG>::GetSESR_PLBS1() const
{
	return sesr_plbs1;
}

template <class CONFIG>
uint32_t Crossbar<CONFIG>::GetMISC_ST_PLBS1() const
{
	return misc_st_plbs1;
}

template <class CONFIG>
uint32_t Crossbar<CONFIG>::GetPLBERR_ST_PLBS1() const
{
	return plberr_st_plbs1;
}

template <class CONFIG>
uint32_t Crossbar<CONFIG>::GetSM_ST_PLBS1() const
{
	return sm_st_plbs1;
}

template <class CONFIG>
uint32_t Crossbar<CONFIG>::GetMISC_PLBS1() const
{
	return misc_plbs1;
}

template <class CONFIG>
uint32_t Crossbar<CONFIG>::GetCMD_SNIFF_PLBS1() const
{
	return cmd_sniff_plbs1;
}

template <class CONFIG>
uint32_t Crossbar<CONFIG>::GetCMD_SNIFFA_PLBS1() const
{
	return cmd_sniffa_plbs1;
}

template <class CONFIG>
uint32_t Crossbar<CONFIG>::GetTMPL_PLBS1_MAP(unsigned int reg_num) const
{
	return tmpl_plbs1_map[reg_num];
}

template <class CONFIG>
uint32_t Crossbar<CONFIG>::GetCFG_PLBM() const
{
	return cfg_plbm;
}

template <class CONFIG>
uint32_t Crossbar<CONFIG>::GetFSEAR_U_PLBM() const
{
	return fsear_u_plbm;
}

template <class CONFIG>
uint32_t Crossbar<CONFIG>::GetFSEAR_L_PLBM() const
{
	return fsear_l_plbm;
}

template <class CONFIG>
uint32_t Crossbar<CONFIG>::GetFSESR_PLBM() const
{
	return fsesr_plbm;
}

template <class CONFIG>
uint32_t Crossbar<CONFIG>::GetMISC_ST_PLBM() const
{
	return misc_st_plbm;
}

template <class CONFIG>
uint32_t Crossbar<CONFIG>::GetPLBERR_ST_PLBM() const
{
	return plberr_st_plbm;
}

template <class CONFIG>
uint32_t Crossbar<CONFIG>::GetSM_ST_PLBM() const
{
	return sm_st_plbm;
}

template <class CONFIG>
uint32_t Crossbar<CONFIG>::GetMISC_PLBM() const
{
	return misc_plbm;
}

template <class CONFIG>
uint32_t Crossbar<CONFIG>::GetCMD_SNIFF_PLBM() const
{
	return cmd_sniff_plbm;
}

template <class CONFIG>
uint32_t Crossbar<CONFIG>::GetCMD_SNIFFA_PLBM() const
{
	return cmd_sniffa_plbm;
}

template <class CONFIG>
void Crossbar<CONFIG>::SetIST(uint32_t value)
{
	ist = value;
}

template <class CONFIG>
void Crossbar<CONFIG>::SetIMASK(uint32_t value)
{
	imask = value;
}

template <class CONFIG>
void Crossbar<CONFIG>::SetARB_XBC(uint32_t value)
{
	arb_xbc = value;
}

template <class CONFIG>
void Crossbar<CONFIG>::SetFIFOST_XBC(uint32_t value)
{
	fifost_xbc = value;
}

template <class CONFIG>
void Crossbar<CONFIG>::SetSM_ST_XBC(uint32_t value)
{
	sm_st_xbc = value;
}

template <class CONFIG>
void Crossbar<CONFIG>::SetMISC_XBC(uint32_t value)
{
	misc_xbc = value;
}

template <class CONFIG>
void Crossbar<CONFIG>::SetARB_XBM(uint32_t value)
{
	arb_xbm = value;
}

template <class CONFIG>
void Crossbar<CONFIG>::SetFIFOST_XBM(uint32_t value)
{
	fifost_xbm = value;
}

template <class CONFIG>
void Crossbar<CONFIG>::SetMISC_XBM(uint32_t value)
{
	misc_xbm = value;
}

template <class CONFIG>
void Crossbar<CONFIG>::SetTMPL_XBAR_MAP(unsigned int reg_num, uint32_t value)
{
	tmpl_xbar_map[reg_num] = value;
}

template <class CONFIG>
void Crossbar<CONFIG>::SetTMPL_SEL_REG(uint32_t value)
{
	tmpl_sel_reg = value;
}

template <class CONFIG>
void Crossbar<CONFIG>::SetCFG_PLBS0(uint32_t value)
{
	cfg_plbs0 = value;
}

template <class CONFIG>
void Crossbar<CONFIG>::SetSEAR_U_PLBS0(uint32_t value)
{
	sear_u_plbs0 = value;
}

template <class CONFIG>
void Crossbar<CONFIG>::SetSEAR_L_PLBS0(uint32_t value)
{
	sear_l_plbs0 = value;
}

template <class CONFIG>
void Crossbar<CONFIG>::SetSESR_PLBS0(uint32_t value)
{
	sesr_plbs0 = value;
}

template <class CONFIG>
void Crossbar<CONFIG>::SetMISC_ST_PLBS0(uint32_t value)
{
	misc_st_plbs0 = value;
}

template <class CONFIG>
void Crossbar<CONFIG>::SetPLBERR_ST_PLBS0(uint32_t value)
{
	plberr_st_plbs0 = value;
}

template <class CONFIG>
void Crossbar<CONFIG>::SetSM_ST_PLBS0(uint32_t value)
{
	sm_st_plbs0 = value;
}

template <class CONFIG>
void Crossbar<CONFIG>::SetMISC_PLBS0(uint32_t value)
{
	misc_plbs0 = value;
}

template <class CONFIG>
void Crossbar<CONFIG>::SetCMD_SNIFF_PLBS0(uint32_t value)
{
	cmd_sniff_plbs0 = value;
}

template <class CONFIG>
void Crossbar<CONFIG>::SetCMD_SNIFFA_PLBS0(uint32_t value)
{
	cmd_sniffa_plbs0 = value;
}

template <class CONFIG>
void Crossbar<CONFIG>::SetTMPL_PLBS0_MAP(unsigned int reg_num, uint32_t value)
{
	tmpl_plbs0_map[reg_num] = value;
}

template <class CONFIG>
void Crossbar<CONFIG>::SetCFG_PLBS1(uint32_t value)
{
	cfg_plbs1 = value;
}

template <class CONFIG>
void Crossbar<CONFIG>::SetSEAR_U_PLBS1(uint32_t value)
{
	sear_u_plbs1 = value;
}

template <class CONFIG>
void Crossbar<CONFIG>::SetSEAR_L_PLBS1(uint32_t value)
{
	sear_l_plbs1 = value;
}

template <class CONFIG>
void Crossbar<CONFIG>::SetSESR_PLBS1(uint32_t value)
{
	sesr_plbs1 = value;
}

template <class CONFIG>
void Crossbar<CONFIG>::SetMISC_ST_PLBS1(uint32_t value)
{
	misc_st_plbs1 = value;
}

template <class CONFIG>
void Crossbar<CONFIG>::SetPLBERR_ST_PLBS1(uint32_t value)
{
	plberr_st_plbs1 = value;
}

template <class CONFIG>
void Crossbar<CONFIG>::SetSM_ST_PLBS1(uint32_t value)
{
	sm_st_plbs1 = value;
}

template <class CONFIG>
void Crossbar<CONFIG>::SetMISC_PLBS1(uint32_t value)
{
	misc_plbs1 = value;
}

template <class CONFIG>
void Crossbar<CONFIG>::SetCMD_SNIFF_PLBS1(uint32_t value)
{
	cmd_sniff_plbs1 = value;
}

template <class CONFIG>
void Crossbar<CONFIG>::SetCMD_SNIFFA_PLBS1(uint32_t value)
{
	cmd_sniffa_plbs1 = value;
}

template <class CONFIG>
void Crossbar<CONFIG>::SetTMPL_PLBS1_MAP(unsigned int reg_num, uint32_t value)
{
	tmpl_plbs1_map[reg_num] = value;
}

template <class CONFIG>
void Crossbar<CONFIG>::SetCFG_PLBM(uint32_t value)
{
	cfg_plbm = value;
}

template <class CONFIG>
void Crossbar<CONFIG>::SetFSEAR_U_PLBM(uint32_t value)
{
	fsear_u_plbm = value;
}

template <class CONFIG>
void Crossbar<CONFIG>::SetFSEAR_L_PLBM(uint32_t value)
{
	fsear_l_plbm = value;
}

template <class CONFIG>
void Crossbar<CONFIG>::SetFSESR_PLBM(uint32_t value)
{
	fsesr_plbm = value;
}

template <class CONFIG>
void Crossbar<CONFIG>::SetMISC_ST_PLBM(uint32_t value)
{
	misc_st_plbm = value;
}

template <class CONFIG>
void Crossbar<CONFIG>::SetPLBERR_ST_PLBM(uint32_t value)
{
	plberr_st_plbm = value;
}

template <class CONFIG>
void Crossbar<CONFIG>::SetSM_ST_PLBM(uint32_t value)
{
	sm_st_plbm = value;
}

template <class CONFIG>
void Crossbar<CONFIG>::SetMISC_PLBM(uint32_t value)
{
	misc_plbm = value;
}

template <class CONFIG>
void Crossbar<CONFIG>::SetCMD_SNIFF_PLBM(uint32_t value)
{
	cmd_sniff_plbm = value;
}

template <class CONFIG>
void Crossbar<CONFIG>::SetCMD_SNIFFA_PLBM(uint32_t value)
{
	cmd_sniffa_plbm = value;
}


} // end of namespace crossbar
} // end of namespace xilinx
} // end of namespace interconnect
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_INTERCONNECT_XILINX_CROSSBAR_CROSSBAR_TCC__
