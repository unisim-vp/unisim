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

#ifndef __UNISIM_COMPONENT_TLM2_INTERCONNECT_FREESCALE_MPC57XX_XBAR_XBAR_TCC__
#define __UNISIM_COMPONENT_TLM2_INTERCONNECT_FREESCALE_MPC57XX_XBAR_XBAR_TCC__

#include <unisim/component/tlm2/interconnect/freescale/mpc57xx/xbar/xbar.hh>
#include <unisim/component/tlm2/interconnect/programmable_router/router.tcc>

namespace unisim {
namespace component {
namespace tlm2 {
namespace interconnect {
namespace freescale {
namespace mpc57xx {
namespace xbar {

template <typename CONFIG>
XBAR<CONFIG>::XBAR(const sc_core::sc_module_name& name, unisim::kernel::service::Object *parent)
	: unisim::kernel::service::Object(name, parent)
	, Super(name, parent)
	, xbar_prs(this)
	, xbar_crs(this)
	, verbose(false)
	, param_verbose("verbose", this, verbose, "enable/disable verbosity")
{
	std::stringstream description_sstr;
	description_sstr << "MPC57XX Crossbar Switch (XBAR):" << std::endl;
	description_sstr << "  - " << CONFIG::INPUT_SOCKETS << " Master ports" << std::endl;
	description_sstr << "  - " << CONFIG::INPUT_BUSWIDTH << "-bit master data bus" << std::endl;
	description_sstr << "  - " << CONFIG::OUTPUT_SOCKETS << " Slave ports" << std::endl;
	description_sstr << "  - " << CONFIG::OUTPUT_BUSWIDTH << "-bit slave data bus" << std::endl;
	description_sstr << "  - " << CONFIG::BUSWIDTH << "-bit peripheral data bus" << std::endl;
	description_sstr << "  - SystemC TLM-2.0 IP Version: " << TLM2_IP_VERSION_MAJOR << "." << TLM2_IP_VERSION_MINOR << "." << TLM2_IP_VERSION_PATCH << std::endl;
	description_sstr << "  - SystemC TLM-2.0 IP Authors: Gilles Mouchard (gilles.mouchard@cea.fr)" << std::endl;
	description_sstr << "  - Hardware reference manual: MPC5777M Reference Manual, MPC5777MRM, Rev. 4.3, 01/2017, Chapter 17" << std::endl;
	this->SetDescription(description_sstr.str().c_str());

	this->MapRegisterFile(XBAR_PRS::ADDRESS_OFFSET, &xbar_prs, 4, XBAR_PRS::ADDRESS_STRIDE);
	this->MapRegisterFile(XBAR_CRS::ADDRESS_OFFSET, &xbar_crs, 4, XBAR_CRS::ADDRESS_STRIDE);
}

template <typename CONFIG>
XBAR<CONFIG>::~XBAR()
{
}

template <typename CONFIG>
void XBAR<CONFIG>::end_of_elaboration()
{
	Super::end_of_elaboration();
	
	this->logger << DebugInfo << this->GetDescription() << EndDebugInfo;
}

template <typename CONFIG>
void XBAR<CONFIG>::Reset()
{
	unsigned int i;
	
	for(i = 0; i < CONFIG::OUTPUT_SOCKETS; i++)
	{
		xbar_prs[i].Reset();
		xbar_crs[i].Reset();
	}
	
	Super::Reset();
}

template <typename CONFIG>
bool XBAR<CONFIG>::ApplyMap(const transaction_type &trans, Mapping const *&applied_mapping) const
{
	return Super::ApplyMap(trans, applied_mapping) && (!trans.is_write() || !xbar_crs[applied_mapping->output_port].template Get<typename XBAR_CRS::RO>());
}

} // end of namespace xbar
} // end of namespace mpc57xx
} // end of namespace freescale
} // end of namespace interconnect
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_INTERCONNECT_FREESCALE_MPC57XX_XBAR_XBAR_TCC__
