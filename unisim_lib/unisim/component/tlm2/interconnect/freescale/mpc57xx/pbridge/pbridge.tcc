/*
 *  Copyright (c) 2018,
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

#ifndef __UNISIM_COMPONENT_TLM2_INTERCONNECT_FREESCALE_MPC57XX_PBRIDGE_PBRIDGE_TCC__
#define __UNISIM_COMPONENT_TLM2_INTERCONNECT_FREESCALE_MPC57XX_PBRIDGE_PBRIDGE_TCC__

#include <unisim/component/tlm2/interconnect/freescale/mpc57xx/pbridge/pbridge.hh>
#include <unisim/component/tlm2/interconnect/programmable_router/router.tcc>
#include <unisim/kernel/tlm2/master_id.hh>
#include <unisim/util/likely/likely.hh>

namespace unisim {
namespace component {
namespace tlm2 {
namespace interconnect {
namespace freescale {
namespace mpc57xx {
namespace pbridge {

template <typename CONFIG>
PBRIDGE<CONFIG>::PBRIDGE(const sc_core::sc_module_name& name, unisim::kernel::service::Object *parent)
	: unisim::kernel::service::Object(name, parent)
	, Super(name, parent)
	, pbridge_mpra(this)
	, pbridge_mprb(this)
	, pbridge_pacra(this)
	, pbridge_pacrb(this)
	, pbridge_pacrc(this)
	, pbridge_pacrd(this)
	, pbridge_pacre(this)
	, pbridge_pacrf(this)
	, pbridge_pacrg(this)
	, pbridge_pacrh(this)
	, pbridge_opacra(this)
	, pbridge_opacrb(this)
	, pbridge_opacrc(this)
	, pbridge_opacrd(this)
	, pbridge_opacre(this)
	, pbridge_opacrf(this)
	, pbridge_opacrg(this)
	, pbridge_opacrh(this)
	, pbridge_opacri(this)
	, pbridge_opacrj(this)
	, pbridge_opacrk(this)
	, pbridge_opacrl(this)
	, pbridge_opacrm(this)
	, pbridge_opacrn(this)
	, pbridge_opacro(this)
	, pbridge_opacrp(this)
	, pbridge_opacrq(this)
	, pbridge_opacrr(this)
	, pbridge_opacrs(this)
	, pbridge_opacrt(this)
	, pbridge_opacru(this)
	, pbridge_opacrv(this)
	, pbridge_opacrw(this)
	, pbridge_opacrx(this)
	, pbridge_opacry(this)
	, pbridge_opacrz(this)
	, pbridge_opacraa(this)
	, pbridge_opacrab(this)
	, pbridge_opacrac(this)
	, pbridge_opacrad(this)
	, pbridge_opacrae(this)
	, pbridge_opacraf(this)
	, acr_mapping()
	, param_acr_mapping()
	, verbose(false)
	, param_verbose("verbose", this, verbose, "enable/disable verbosity")
{
	unsigned int i;
	
	std::stringstream description_sstr;
	description_sstr << "MPC57XX Peripheral Bridge (PBRIDGE):" << std::endl;
	description_sstr << "  - " << CONFIG::INPUT_SOCKETS << " Master ports" << std::endl;
	description_sstr << "  - " << CONFIG::INPUT_BUSWIDTH << "-bit master data bus" << std::endl;
	description_sstr << "  - " << CONFIG::OUTPUT_SOCKETS << " Slave ports" << std::endl;
	description_sstr << "  - " << CONFIG::OUTPUT_BUSWIDTH << "-bit slave data bus" << std::endl;
	description_sstr << "  - " << CONFIG::BUSWIDTH << "-bit peripheral data bus" << std::endl;
	description_sstr << "  - SystemC TLM-2.0 IP Version: " << TLM2_IP_VERSION_MAJOR << "." << TLM2_IP_VERSION_MINOR << "." << TLM2_IP_VERSION_PATCH << std::endl;
	description_sstr << "  - SystemC TLM-2.0 IP Authors: Gilles Mouchard (gilles.mouchard@cea.fr)" << std::endl;
	description_sstr << "  - Hardware reference manual: MPC5777M Reference Manual, MPC5777MRM, Rev. 4.3, 01/2017, Chapter 19" << std::endl;
	this->SetDescription(description_sstr.str().c_str());
	
	for(i = 0; i < CONFIG::OUTPUT_SOCKETS; i++)
	{
		std::stringstream sstr_name;
		sstr_name << "acr_mapping_" << i;
		std::stringstream sstr_desc;
		sstr_desc << "access control register for output port #" << i;
		param_acr_mapping[i] = new unisim::kernel::service::Parameter<AccessControlRegisterMapping>(sstr_name.str().c_str(), this, acr_mapping[i], sstr_desc.str().c_str());
		
		param_acr_mapping[i]->SetMutable(false);
	}
	
	this->MapRegister(pbridge_mpra.ADDRESS_OFFSET, &pbridge_mpra);
	this->MapRegister(pbridge_mprb.ADDRESS_OFFSET, &pbridge_mprb);
	this->MapRegister(pbridge_pacra.ADDRESS_OFFSET,   &pbridge_pacra);
	this->MapRegister(pbridge_pacrb.ADDRESS_OFFSET,   &pbridge_pacrb);
	this->MapRegister(pbridge_pacrc.ADDRESS_OFFSET,   &pbridge_pacrc);
	this->MapRegister(pbridge_pacrd.ADDRESS_OFFSET,   &pbridge_pacrd);
	this->MapRegister(pbridge_pacre.ADDRESS_OFFSET,   &pbridge_pacre);
	this->MapRegister(pbridge_pacrf.ADDRESS_OFFSET,   &pbridge_pacrf);
	this->MapRegister(pbridge_pacrg.ADDRESS_OFFSET,   &pbridge_pacrg);
	this->MapRegister(pbridge_pacrh.ADDRESS_OFFSET,   &pbridge_pacrh);
	this->MapRegister(pbridge_opacra.ADDRESS_OFFSET,  &pbridge_opacra);
	this->MapRegister(pbridge_opacrb.ADDRESS_OFFSET,  &pbridge_opacrb);
	this->MapRegister(pbridge_opacrc.ADDRESS_OFFSET,  &pbridge_opacrc);
	this->MapRegister(pbridge_opacrd.ADDRESS_OFFSET,  &pbridge_opacrd);
	this->MapRegister(pbridge_opacre.ADDRESS_OFFSET,  &pbridge_opacre);
	this->MapRegister(pbridge_opacrf.ADDRESS_OFFSET,  &pbridge_opacrf);
	this->MapRegister(pbridge_opacrg.ADDRESS_OFFSET,  &pbridge_opacrg);
	this->MapRegister(pbridge_opacrh.ADDRESS_OFFSET,  &pbridge_opacrh);
	this->MapRegister(pbridge_opacri.ADDRESS_OFFSET,  &pbridge_opacri);
	this->MapRegister(pbridge_opacrj.ADDRESS_OFFSET,  &pbridge_opacrj);
	this->MapRegister(pbridge_opacrk.ADDRESS_OFFSET,  &pbridge_opacrk);
	this->MapRegister(pbridge_opacrl.ADDRESS_OFFSET,  &pbridge_opacrl);
	this->MapRegister(pbridge_opacrm.ADDRESS_OFFSET,  &pbridge_opacrm);
	this->MapRegister(pbridge_opacrn.ADDRESS_OFFSET,  &pbridge_opacrn);
	this->MapRegister(pbridge_opacro.ADDRESS_OFFSET,  &pbridge_opacro);
	this->MapRegister(pbridge_opacrp.ADDRESS_OFFSET,  &pbridge_opacrp);
	this->MapRegister(pbridge_opacrq.ADDRESS_OFFSET,  &pbridge_opacrq);
	this->MapRegister(pbridge_opacrr.ADDRESS_OFFSET,  &pbridge_opacrr);
	this->MapRegister(pbridge_opacrs.ADDRESS_OFFSET,  &pbridge_opacrs);
	this->MapRegister(pbridge_opacrt.ADDRESS_OFFSET,  &pbridge_opacrt);
	this->MapRegister(pbridge_opacru.ADDRESS_OFFSET,  &pbridge_opacru);
	this->MapRegister(pbridge_opacrv.ADDRESS_OFFSET,  &pbridge_opacrv);
	this->MapRegister(pbridge_opacrw.ADDRESS_OFFSET,  &pbridge_opacrw);
	this->MapRegister(pbridge_opacrx.ADDRESS_OFFSET,  &pbridge_opacrx);
	this->MapRegister(pbridge_opacry.ADDRESS_OFFSET,  &pbridge_opacry);
	this->MapRegister(pbridge_opacrz.ADDRESS_OFFSET,  &pbridge_opacrz);
	this->MapRegister(pbridge_opacraa.ADDRESS_OFFSET, &pbridge_opacraa);
	this->MapRegister(pbridge_opacrab.ADDRESS_OFFSET, &pbridge_opacrab);
	this->MapRegister(pbridge_opacrac.ADDRESS_OFFSET, &pbridge_opacrac);
	this->MapRegister(pbridge_opacrad.ADDRESS_OFFSET, &pbridge_opacrad);
	this->MapRegister(pbridge_opacrae.ADDRESS_OFFSET, &pbridge_opacrae);
	this->MapRegister(pbridge_opacraf.ADDRESS_OFFSET, &pbridge_opacraf);
}

template <typename CONFIG>
PBRIDGE<CONFIG>::~PBRIDGE()
{
	unsigned int i;
	
	for(i = 0; i < CONFIG::OUTPUT_SOCKETS; i++)
	{
		delete param_acr_mapping[i];
	}
}

template <typename CONFIG>
void PBRIDGE<CONFIG>::end_of_elaboration()
{
	Super::end_of_elaboration();
	
	this->logger << DebugInfo << this->GetDescription() << EndDebugInfo;
}

template <typename CONFIG>
void PBRIDGE<CONFIG>::Reset()
{
	Super::Reset();

	pbridge_mpra.Reset();
	pbridge_mprb.Reset();
	pbridge_pacra.Reset();
	pbridge_pacrb.Reset();
	pbridge_pacrc.Reset();
	pbridge_pacrd.Reset();
	pbridge_pacre.Reset();
	pbridge_pacrf.Reset();
	pbridge_pacrg.Reset();
	pbridge_pacrh.Reset();
	pbridge_opacra.Reset();
	pbridge_opacrb.Reset();
	pbridge_opacrc.Reset();
	pbridge_opacrd.Reset();
	pbridge_opacre.Reset();
	pbridge_opacrf.Reset();
	pbridge_opacrg.Reset();
	pbridge_opacrh.Reset();
	pbridge_opacri.Reset();
	pbridge_opacrj.Reset();
	pbridge_opacrk.Reset();
	pbridge_opacrl.Reset();
	pbridge_opacrm.Reset();
	pbridge_opacrn.Reset();
	pbridge_opacro.Reset();
	pbridge_opacrp.Reset();
	pbridge_opacrq.Reset();
	pbridge_opacrr.Reset();
	pbridge_opacrs.Reset();
	pbridge_opacrt.Reset();
	pbridge_opacru.Reset();
	pbridge_opacrv.Reset();
	pbridge_opacrw.Reset();
	pbridge_opacrx.Reset();
	pbridge_opacry.Reset();
	pbridge_opacrz.Reset();
	pbridge_opacraa.Reset();
	pbridge_opacrab.Reset();
	pbridge_opacrac.Reset();
	pbridge_opacrad.Reset();
	pbridge_opacrae.Reset();
	pbridge_opacraf.Reset();
}

template <typename CONFIG>
bool PBRIDGE<CONFIG>::GetMasterPrivilege(int master_id, MasterPrivilege& mpriv) const
{
	//std::cerr << "master_id=" << master_id << std::endl;
	unsigned int pbridge_mpr_num = master_id / 8;
	unsigned int mprot_shift = 4 * (7 - (master_id % 8));
	
	uint32_t pbridge_mpr = 0;
	switch(pbridge_mpr_num)
	{
		case 0: pbridge_mpr = (uint32_t) pbridge_mpra; break;
		case 1: pbridge_mpr = (uint32_t) pbridge_mprb; break;
		default: return false;
	}
	unsigned int mprot = pbridge_mpr >> mprot_shift;
	mpriv.mtr = MPROT::MTR::Get(mprot);
	mpriv.mtw = MPROT::MTW::Get(mprot);
	mpriv.mpl = MPROT::MPL::Get(mprot);
	//std::cerr << "mtr=" << mpriv.mtr << ",mtw=" << mpriv.mtw << ",mpl=" << mpriv.mpl << std::endl;
	
	return true;
}

template <typename CONFIG>
bool PBRIDGE<CONFIG>::GetAccessControl(unsigned int output_port, AccessControl& access_ctrl) const
{
	//std::cerr << "output_port=" << output_port << std::endl;
	const AccessControlRegisterMapping& _acr_mapping = acr_mapping[output_port];
	bool off_platform = _acr_mapping.off_platform;
	unsigned int acr_reg_num = _acr_mapping.reg_num;
	//std::cerr << (off_platform ? "o" : "") << "pacr" << acr_reg_num << std::endl;
	unsigned int reg_num = acr_reg_num / 8;
	//std::cerr << "reg_num=" << reg_num << std::endl;
	unsigned int acr_shift = 4 * (7 - (acr_reg_num % 8));
	//std::cerr << "acr_shift=" << acr_shift << std::endl;
	
	if(off_platform)
	{
		uint32_t pbridge_opacr = 0;
		switch(reg_num)
		{
			case 0: pbridge_opacr = (uint32_t) pbridge_opacra; break;
			case 1: pbridge_opacr = (uint32_t) pbridge_opacrb; break;
			case 2: pbridge_opacr = (uint32_t) pbridge_opacrc; break;
			case 3: pbridge_opacr = (uint32_t) pbridge_opacrd; break;
			case 4: pbridge_opacr = (uint32_t) pbridge_opacre; break;
			case 5: pbridge_opacr = (uint32_t) pbridge_opacrf; break;
			case 6: pbridge_opacr = (uint32_t) pbridge_opacrg; break;
			case 7: pbridge_opacr = (uint32_t) pbridge_opacrh; break;
			case 8: pbridge_opacr = (uint32_t) pbridge_opacri; break;
			case 9: pbridge_opacr = (uint32_t) pbridge_opacrj; break;
			case 10: pbridge_opacr = (uint32_t) pbridge_opacrk; break;
			case 11: pbridge_opacr = (uint32_t) pbridge_opacrl; break;
			case 12: pbridge_opacr = (uint32_t) pbridge_opacrm; break;
			case 13: pbridge_opacr = (uint32_t) pbridge_opacrn; break;
			case 14: pbridge_opacr = (uint32_t) pbridge_opacro; break;
			case 15: pbridge_opacr = (uint32_t) pbridge_opacrp; break;
			case 16: pbridge_opacr = (uint32_t) pbridge_opacrq; break;
			case 17: pbridge_opacr = (uint32_t) pbridge_opacrr; break;
			case 18: pbridge_opacr = (uint32_t) pbridge_opacrs; break;
			case 19: pbridge_opacr = (uint32_t) pbridge_opacrt; break;
			case 20: pbridge_opacr = (uint32_t) pbridge_opacru; break;
			case 21: pbridge_opacr = (uint32_t) pbridge_opacrv; break;
			case 22: pbridge_opacr = (uint32_t) pbridge_opacrw; break;
			case 23: pbridge_opacr = (uint32_t) pbridge_opacrx; break;
			case 24: pbridge_opacr = (uint32_t) pbridge_opacry; break;
			case 25: pbridge_opacr = (uint32_t) pbridge_opacrz; break;
			case 26: pbridge_opacr = (uint32_t) pbridge_opacraa; break;
			case 27: pbridge_opacr = (uint32_t) pbridge_opacrab; break;
			case 28: pbridge_opacr = (uint32_t) pbridge_opacrac; break;
			case 29: pbridge_opacr = (uint32_t) pbridge_opacrad; break;
			case 30: pbridge_opacr = (uint32_t) pbridge_opacrae; break;
			case 31: pbridge_opacr = (uint32_t) pbridge_opacraf; break;
			default:
				return false;
		}
		unsigned int opacr = pbridge_opacr >> acr_shift;
		access_ctrl.sp = OPACR::SP::Get(opacr);
		access_ctrl.wp = OPACR::WP::Get(opacr);
		access_ctrl.tp = OPACR::TP::Get(opacr);
	}
	else
	{
		uint32_t pbridge_pacr = 0;
		switch(reg_num)
		{
			case 0: pbridge_pacr = (uint32_t) pbridge_pacra; break;
			case 1: pbridge_pacr = (uint32_t) pbridge_pacrb; break;
			case 2: pbridge_pacr = (uint32_t) pbridge_pacrc; break;
			case 3: pbridge_pacr = (uint32_t) pbridge_pacrd; break;
			case 4: pbridge_pacr = (uint32_t) pbridge_pacre; break;
			case 5: pbridge_pacr = (uint32_t) pbridge_pacrf; break;
			case 6: pbridge_pacr = (uint32_t) pbridge_pacrg; break;
			case 7: pbridge_pacr = (uint32_t) pbridge_pacrh; break;
			default:
				return false;
		}
		unsigned int pacr = pbridge_pacr >> acr_shift;
		access_ctrl.sp = PACR::SP::Get(pacr);
		access_ctrl.wp = PACR::WP::Get(pacr);
		access_ctrl.tp = PACR::TP::Get(pacr);
	}
	
	//std::cerr << "sp=" << access_ctrl.sp << ", wp=" << access_ctrl.wp << ",tp=" << access_ctrl.tp << std::endl;
	return true;
}

template <typename CONFIG>
bool PBRIDGE<CONFIG>::ApplyMap(const transaction_type &trans, Mapping const *&applied_mapping) const
{
	if(likely(Super::ApplyMap(trans, applied_mapping)))
	{
		bool is_ignore = trans.get_command() == tlm::TLM_IGNORE_COMMAND;
		bool is_read = trans.is_read();
		bool is_write = trans.is_write();

		unisim::kernel::tlm2::tlm_master_id *master_id_ext = trans.template get_extension<unisim::kernel::tlm2::tlm_master_id>();
		
		if(likely(master_id_ext != 0))
		{
			int master_id = (int)(*master_id_ext);
		
			unsigned int output_port = applied_mapping->output_port;
			
			MasterPrivilege mpriv;
			if(unlikely(!GetMasterPrivilege(master_id, mpriv))) return false;
			
			AccessControl access_ctrl;
			if(unlikely(!GetAccessControl(output_port, access_ctrl))) return false;
			
			return likely((!access_ctrl.sp || mpriv.mpl) && ((is_read && (!access_ctrl.tp || mpriv.mtr)) || (is_write && !access_ctrl.wp && (!access_ctrl.tp || mpriv.mtw)) || unlikely(is_ignore)));
		}
		
		return true;
	}
	
	return false;
}

} // end of namespace pbridge
} // end of namespace mpc57xx
} // end of namespace freescale
} // end of namespace interconnect
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_INTERCONNECT_FREESCALE_MPC57XX_PBRIDGE_PBRIDGE_TCC__
