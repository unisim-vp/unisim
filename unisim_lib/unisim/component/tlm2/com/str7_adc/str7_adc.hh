/*
 *  Copyright (c) 2009,
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
 * Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 */

#ifndef __UNISIM_COMPONENT_TLM2_COM_STR7_ADC_STR7_ADC_HH__
#define __UNISIM_COMPONENT_TLM2_COM_STR7_ADC_STR7_ADC_HH__

#include <systemc.h>
#include <tlm.h>
#include <tlm_utils/passthrough_target_socket.h>
#include <tlm_utils/simple_initiator_socket.h>
#include <inttypes.h>

#include "unisim/kernel/service/service.hh"
#include "unisim/kernel/logger/logger.hh"

namespace unisim {
namespace component {
namespace tlm2 {
namespace com {
namespace str7_adc {
	
	template <unsigned int BUS_WIDTH = 32>
	class STR7_ADC :
	public sc_module,
	public unisim::kernel::service::Object,
	public tlm::tlm_fw_transport_if<>
	{
public:
	/* input socket for memory transactions */
	tlm::tlm_target_socket<BUS_WIDTH> in_mem;
	
	STR7_ADC(const sc_module_name& name, Object* parent = 0);
	virtual ~STR7_ADC();
	
	virtual bool Setup();
	
private:
	typedef STR7_ADC<BUS_WIDTH> THIS_MODULE;
	
	/* START: ADC registers and methods */
	uint16_t adc_data0;
	uint16_t adc_data1;
	uint16_t adc_data2;
	uint16_t adc_data3;
	uint16_t adc_csr;
        uint16_t adc_cpr;
        unisim::kernel::service::Register<uint16_t> reg_adc_data0;
	unisim::kernel::service::Register<uint16_t> reg_adc_data1;
	unisim::kernel::service::Register<uint16_t> reg_adc_data2;
	unisim::kernel::service::Register<uint16_t> reg_adc_data3;
        unisim::kernel::service::Register<uint16_t> reg_adc_csr;
	unisim::kernel::service::Register<uint16_t> reg_adc_cpr;

	uint16_t ReadRegister(uint64_t address, bool update = true);
	void WriteRegister(uint64_t address, uint16_t value, bool update = true);
	/* END: ADC registers and methods */
	
	/* Base address that will be used by the ADC and its parameter to set it up */
	uint64_t base_address;
	unisim::kernel::service::Parameter<uint64_t> param_base_address;
	
	/* START: methods implementing the "in_mem" socket */
	virtual void b_transport(tlm::tlm_generic_payload& trans, sc_core::sc_time& t);
	virtual tlm::tlm_sync_enum nb_transport_fw(tlm::tlm_generic_payload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t);
	virtual bool get_direct_mem_ptr(tlm::tlm_generic_payload& trans, tlm::tlm_dmi& dmi_data);
	virtual unsigned int transport_dbg(tlm::tlm_generic_payload& trans);
	/* END: methods implementing the "in_mem" socket */

	/* START: verbose options */
	bool verbose_all;
	unisim::kernel::service::Parameter<bool> param_verbose_all;
	bool verbose_setup;
	unisim::kernel::service::Parameter<bool> param_verbose_setup;
	bool VerboseSetup();
	bool verbose_tlm;
	unisim::kernel::service::Parameter<bool> param_verbose_tlm;
	bool VerboseTLM();
	/* END: verbose options */
	
	/* the logger */
	unisim::kernel::logger::Logger logger;
};
	
} // end of namespace str7_adc
} // end of namespace com
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_COM_STR7_ADC_STR7_ADC_HH__
