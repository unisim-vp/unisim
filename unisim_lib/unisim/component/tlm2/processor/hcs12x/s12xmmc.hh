/*
 *  Copyright (c) 2008,
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
 * Authors: Reda   Nouacer  (reda.nouacer@cea.fr)
 */

/*
 * This module implement the S12XMMC_V2 controler
 */

#ifndef __UNISIM_COMPONENT_TLM2_PROCESSOR_S12XMMC_HH__
#define __UNISIM_COMPONENT_TLM2_PROCESSOR_S12XMMC_HH__

#include <inttypes.h>
#include <iostream>
#include <cmath>

#include <systemc.h>

#include "tlm_utils/simple_initiator_socket.h"
#include "tlm_utils/simple_target_socket.h"
#include "tlm_utils/multi_passthrough_target_socket.h"

#include "unisim/service/interfaces/trap_reporting.hh"
#include <unisim/kernel/service/service.hh>
#include "unisim/kernel/tlm2/tlm.hh"

#include <unisim/component/cxx/processor/hcs12x/config.hh>
#include <unisim/component/cxx/processor/hcs12x/types.hh>
#include <unisim/component/cxx/processor/hcs12x/mmc.hh>

namespace unisim {
namespace component {
namespace tlm2 {
namespace processor {
namespace hcs12x {

using unisim::kernel::service::ServiceImport;
using unisim::kernel::service::Client;
using unisim::service::interfaces::TrapReporting;

using unisim::component::cxx::processor::hcs12x::ADDRESS;
using unisim::component::cxx::processor::hcs12x::MMC;
using unisim::component::cxx::processor::hcs12x::MMC_DATA;
using unisim::component::cxx::processor::hcs12x::address_t;
using unisim::component::cxx::processor::hcs12x::physical_address_t;
using unisim::component::cxx::processor::hcs12x::service_address_t;
using unisim::component::cxx::processor::hcs12x::CONFIG;

using unisim::kernel::service::Object;
using unisim::kernel::tlm2::PayloadFabric;

class S12XMMC :
	public sc_module,
	public MMC,
	public Client<TrapReporting >

{
public:
	typedef MMC inherited;

	ServiceImport<TrapReporting > trap_reporting_import;
	
	tlm_utils::simple_target_socket<S12XMMC> cpu_socket;
	tlm_utils::simple_initiator_socket<S12XMMC> local_socket;
	tlm_utils::simple_initiator_socket<S12XMMC> external_socket;

	S12XMMC(const sc_module_name& name, Object *parent = 0);
	virtual ~S12XMMC();

	void Run();

	virtual void b_transport( tlm::tlm_generic_payload& trans, sc_time& delay );

private:

	sc_time tlm2_btrans_time;
	PayloadFabric<tlm::tlm_generic_payload> payloadFabric;

	// TODO: complete by integrating the routing functionality to the MMC
	static const uint8_t DEVICE_MAP_SIZE = 6;
	struct {
		address_t	start_address;
		address_t	end_address;
	} deviceMap[DEVICE_MAP_SIZE];

}; /* end class S12XMMC */

} // end of namespace hcs12x
} // end of namespace processor
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif /*__UNISIM_COMPONENT_TLM2_PROCESSOR_S12XMMC_HH__*/
