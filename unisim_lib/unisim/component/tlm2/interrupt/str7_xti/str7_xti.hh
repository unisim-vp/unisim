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

#ifndef __UNISIM_COMPONENT_TLM2_INTERRUPT_STR7_XTI_XTI_HH__
#define __UNISIM_COMPONENT_TLM2_INTERRUPT_STR7_XTI_XTI_HH__

#include <systemc.h>
#include <tlm.h>
#include <tlm_utils/multi_passthrough_target_socket.h>
#include <inttypes.h>

#include "unisim/kernel/service/service.hh"
#include "unisim/kernel/logger/logger.hh"
#include "unisim/component/tlm2/interrupt/types.hh"

namespace unisim {
namespace component {
namespace tlm2 {
namespace interrupt {
namespace str7_xti {

using unisim::component::tlm2::interrupt::InterruptProtocolTypes;
using unisim::component::tlm2::interrupt::TLMInterruptPayload;
using unisim::kernel::service::Parameter;
using unisim::kernel::service::Object;
using unisim::kernel::logger::Logger;

template   <unsigned int BUS_WIDTH = 32,
		 	bool DEBUG = false>
class STR7_XTI :
	public Object,
	public sc_module,
	public tlm::tlm_bw_transport_if<InterruptProtocolTypes>,
	public tlm::tlm_fw_transport_if<>
{
public:
	tlm_utils::multi_passthrough_target_socket<unisim::component::tlm2::interrupt::str7_xti::STR7_XTI<BUS_WIDTH, DEBUG>, 0, InterruptProtocolTypes, 16, sc_core::SC_ZERO_OR_MORE_BOUND> in_irq;
	tlm::tlm_initiator_socket<0, InterruptProtocolTypes> out_irq;
	
	tlm::tlm_target_socket<BUS_WIDTH> slave_socket;

	STR7_XTI(const sc_module_name& name, Object* parent = 0);
	virtual ~STR7_XTI();

	virtual bool Setup();

	/* START: methods implementing the "target" socket */
	virtual void b_transport(tlm::tlm_generic_payload& trans, sc_core::sc_time& t);
	virtual tlm::tlm_sync_enum nb_transport_fw(tlm::tlm_generic_payload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t);
	virtual bool get_direct_mem_ptr(tlm::tlm_generic_payload& trans, tlm::tlm_dmi& dmi_data);
	virtual unsigned int transport_dbg(tlm::tlm_generic_payload& trans);
	/* END: methods implementing the "target" socket */

	/* START: methods implementing the "out_irq" socket */
	virtual tlm::tlm_sync_enum tlm_bw_nonblocking_transport_bw_if(TLMInterruptPayload &trans, tlm::tlm_phase& phase, sc_core::sc_time& t);
	virtual void invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range);
	/* END: methods implementing the "out_irq" socket */

	/* START: callbacks for the multi_passthrough_target_socket in_irq */
	tlm::tlm_sync_enum in_irq_nb(int index, TLMInterruptPayload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t);
	void in_irq_b(int index, TLMInterruptPayload& trans, sc_core::sc_time& t);
	unsigned int in_irq_dbg(int index, TLMInterruptPayload &trans);
	bool in_irq_dmi(int index, TLMInterruptPayload& trans, tlm::tlm_dmi& dmi);
	/* END: callbacks for the multi_passthrough_target_socket in_irq */

private:
	/* interrupt controller registers */
	uint8_t sr;
	uint8_t ctrl;
	uint16_t mr;
	uint16_t tr;
	uint16_t pr;

	/* interrupt controller accessors */
	inline uint8_t SR() { return sr; }
	inline uint8_t SetSR(uint8_t data) { sr = data; return SR(); }
	bool XTIS(unsigned int index) { return (sr >> index) & 1; }
	inline uint8_t CTRL() { return ctrl; }
	inline uint8_t SetCTRL(uint8_t data) { ctrl = data & (uint8_t)0x07; return CTRL(); }
	static const uint8_t STOP_MASK = (uint8_t)0x04;
	inline bool STOP() { return ctrl & STOP_MASK; }
	static const uint8_t ID1S_MASK = (uint8_t)0x02;
	inline bool ID1S() { return ctrl & ID1S_MASK; }
	static const uint8_t WKUPINT_MASK = (uint8_t)0x01;
	inline bool WKUPINT() { return ctrl & WKUPINT_MASK; }
	inline uint16_t MR() { return mr; }
	inline uint8_t MRH() { return (uint8_t)(mr >> 8); }
	inline uint8_t SetMRH(uint8_t data) { mr = (mr & (uint16_t)0x00ff) | (uint16_t)data << 8; return MRH(); }
	inline uint8_t MRL() { return (uint8_t)mr; }
	inline uint8_t SetMRL(uint8_t data) { mr = (mr & (uint16_t)0xff00) | (uint16_t)data; return MRL(); }
	inline bool XTIM(unsigned int index) { return (mr >> index) & 1; } 
	inline uint16_t TR() { return tr; }
	inline uint8_t TRH() { return (uint8_t)(tr >> 8); }
	inline uint8_t SetTRH(uint8_t data) { tr = (tr & (uint16_t)0x00ff) | (uint16_t)data << 8; return TRH(); }
	inline uint8_t TRL() { return (uint8_t)tr; } 
	inline uint8_t SetTRL(uint8_t data) { tr = (tr & (uint16_t)0xff00) | (uint16_t)data; return TRL(); }
	inline bool XTIT(unsigned int index) { return (tr >> index) & 1; }
	inline uint16_t PR() { return pr; }
	inline uint8_t PRH() { return (uint8_t)(pr >> 8); }
	inline uint8_t SetPRH(uint8_t data) { pr = (pr & (uint16_t)0x00ff) | (uint16_t)data << 8; return PRH(); }
	inline uint8_t PRL() { return (uint8_t)pr; }
	inline uint8_t SetPRL(uint8_t data) { pr = (pr & (uint16_t)0xff00) | (uint16_t)data; return PRL(); }
	inline bool XTIP(unsigned int index) { return (pr >> index) & 1; }

	/* interrupt controller registers addresses */
	static const uint32_t SR_ADDR = 0x001c;
	static const uint32_t CTRL_ADDR = 0x0024;
	static const uint32_t MRH_ADDR = 0x0028;
	static const uint32_t MRL_ADDR = 0x002c;
	static const uint32_t TRH_ADDR = 0x0030;
	static const uint32_t TRL_ADDR = 0x0034;
	static const uint32_t PRH_ADDR = 0x0038;
	static const uint32_t PRL_ADDR = 0x003c;

	/* interrupt controller registers names */
	static const char* const SR_NAME;
	static const char* const CTRL_NAME;
	static const char* const MRH_NAME;
	static const char* const MRL_NAME;
	static const char* const TRH_NAME;
	static const char* const TRL_NAME;
	static const char* const PRH_NAME;
	static const char* const PRL_NAME;

	tlm::tlm_response_status ReadRegister(uint64_t addr, uint8_t& data, sc_core::sc_time& t);
	tlm::tlm_response_status WriteRegister(uint64_t addr, uint8_t& data, sc_core::sc_time& t);

	/* module parameters */
	uint64_t base_address;
	unisim::kernel::service::Parameter<uint64_t> param_base_address;

	/* logger methods */
	unisim::kernel::logger::Logger logger;
	bool verbose_all;
	unisim::kernel::service::Parameter<bool> param_verbose_all;
	bool verbose_setup;
	unisim::kernel::service::Parameter<bool> param_verbose_setup;
	bool verbose_run;
	unisim::kernel::service::Parameter<bool> param_verbose_run;
	inline bool VerboseAll();
	inline bool VerboseSetup();
	inline bool VerboseRun();
};

template <unsigned int BUS_WIDTH , bool DEBUG>
const char* const STR7_XTI<BUS_WIDTH, DEBUG>::SR_NAME = "SR";
template <unsigned int BUS_WIDTH , bool DEBUG>
const char* const STR7_XTI<BUS_WIDTH, DEBUG>::CTRL_NAME = "CTRL";
template <unsigned int BUS_WIDTH , bool DEBUG>
const char* const STR7_XTI<BUS_WIDTH, DEBUG>::MRH_NAME = "MRH";
template <unsigned int BUS_WIDTH , bool DEBUG>
const char* const STR7_XTI<BUS_WIDTH, DEBUG>::MRL_NAME = "MRL";
template <unsigned int BUS_WIDTH , bool DEBUG>
const char* const STR7_XTI<BUS_WIDTH, DEBUG>::TRH_NAME = "TRH";
template <unsigned int BUS_WIDTH , bool DEBUG>
const char* const STR7_XTI<BUS_WIDTH, DEBUG>::TRL_NAME = "TRL";
template <unsigned int BUS_WIDTH , bool DEBUG>
const char* const STR7_XTI<BUS_WIDTH, DEBUG>::PRH_NAME = "PRH";
template <unsigned int BUS_WIDTH , bool DEBUG>
const char* const STR7_XTI<BUS_WIDTH, DEBUG>::PRL_NAME = "PRL";

} // end of namespace str7_xti 
} // end of namespace interrupt
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_INTERRUPT_STR7_XTI_XTI_HH__

