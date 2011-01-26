/*
 *  Copyright (c) 2011,
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

#ifndef __UNISIM_COMPONENT_TLM2_INTERRUPT_XILINX_XPS_INTC_XPS_INTC_HH__
#define __UNISIM_COMPONENT_TLM2_INTERRUPT_XILINX_XPS_INTC_XPS_INTC_HH__

#include <systemc.h>
#include <unisim/kernel/tlm2/tlm.hh>
#include <unisim/component/cxx/interrupt/xilinx/xps_intc/xps_intc.hh>
#include <unisim/component/tlm2/interrupt/types.hh>
#include <tlm_utils/peq_with_get.h>

namespace unisim {
namespace component {
namespace tlm2 {
namespace interrupt {
namespace xilinx {
namespace xps_intc {

using unisim::kernel::service::Object;
using unisim::kernel::service::Parameter;
using unisim::kernel::service::Service;
using unisim::kernel::service::ServiceExport;
using unisim::kernel::service::ServiceExportBase;
using unisim::component::tlm2::interrupt::InterruptProtocolTypes;
using unisim::component::tlm2::interrupt::TLMInterruptPayload;
using unisim::kernel::tlm2::PayloadFabric;

template <class CONFIG>
class XPS_IntC
	: public sc_module
	, public unisim::component::cxx::interrupt::xilinx::xps_intc::XPS_IntC<CONFIG>
	, public tlm::tlm_fw_transport_if<>
	, public tlm::tlm_bw_transport_if<InterruptProtocolTypes>
{
public:
	typedef unisim::component::cxx::interrupt::xilinx::xps_intc::XPS_IntC<CONFIG> inherited;
	typedef tlm::tlm_target_socket<0, InterruptProtocolTypes> irq_slave_socket;
	typedef tlm::tlm_initiator_socket<0, InterruptProtocolTypes> irq_master_socket;
	
	// PLB slave interface
	tlm::tlm_target_socket<CONFIG::C_SPLB_DWITH> slave_sock;
	
	// Input interrupts
	irq_slave_socket *irq_slave_sock[CONFIG::C_NUM_INTR_INPUTS];
	
	// Output interrupt request port
	irq_master_socket irq_master_sock;

	XPS_IntC(const sc_module_name& name, Object *parent = 0);
	virtual ~XPS_IntC();
	
	virtual bool get_direct_mem_ptr(tlm::tlm_generic_payload& payload, tlm::tlm_dmi& dmi_data);
	virtual unsigned int transport_dbg(tlm::tlm_generic_payload& payload);
	virtual tlm::tlm_sync_enum nb_transport_fw(tlm::tlm_generic_payload& payload, tlm::tlm_phase& phase, sc_core::sc_time& t);
	virtual void b_transport(tlm::tlm_generic_payload& payload, sc_core::sc_time& t);

	virtual tlm::tlm_sync_enum nb_transport_bw(TLMInterruptPayload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t);
	virtual void invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range);

	virtual void b_transport(unsigned int irq, TLMInterruptPayload& trans, sc_core::sc_time& t);
	virtual tlm::tlm_sync_enum nb_transport_fw(unsigned int irq, TLMInterruptPayload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t);

	void Process();
protected:
	virtual void SetOutputLevel(bool level);
	virtual void SetOutputEdge(bool final_level);
private:
	bool IsVerbose() const;

	class IRQForwarder : public tlm::tlm_fw_transport_if<InterruptProtocolTypes>
	{
	public:
		IRQForwarder(unsigned int irq, XPS_IntC<CONFIG> *xps_intc);
		
		virtual void b_transport(TLMInterruptPayload& trans, sc_core::sc_time& t);

		virtual tlm::tlm_sync_enum nb_transport_fw(TLMInterruptPayload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t);

		virtual unsigned int transport_dbg(TLMInterruptPayload& trans);
		
		virtual bool get_direct_mem_ptr(TLMInterruptPayload& trans, tlm::tlm_dmi& dmi_data);

	private:
		unsigned int irq;
		XPS_IntC<CONFIG> *xps_intc;
	};
	
	/** Enable/Disable the verbosity */
	bool verbose;
	sc_time local_time;
	/** Cycle time */
	sc_time cycle_time;
	/** Latencies */
	sc_time read_latency;
	sc_time write_latency;

	
	/** The parameter to enable/disable the verbosity */
	Parameter<bool> param_verbose;
	
	/** The parameter for the cycle time */
	Parameter<sc_time> param_cycle_time;
	/** The parameters to set the latencies */
	Parameter<sc_time> param_read_latency;
	Parameter<sc_time> param_write_latency;

	IRQForwarder *irq_forwarder[CONFIG::C_NUM_INTR_INPUTS];
	bool output_level;
	
	class UnifiedPayload : public unisim::kernel::tlm2::ManagedPayload
	{
	public:
		typedef enum
		{
			CPU_PAYLOAD,
			INTR_PAYLOAD
		} Type;

		UnifiedPayload();
		void SetPayload(tlm::tlm_generic_payload *cpu_payload);
		void SetPayload(TLMInterruptPayload *intr_payload);
		void SetNonBlocking();
		void SetBlocking(sc_event *ev_completed);
		virtual ~UnifiedPayload();
		
		Type type;
		bool blocking;
		tlm::tlm_generic_payload *cpu_payload;
		TLMInterruptPayload *intr_payload;
		unsigned int irq;
		sc_event *ev_completed;
	};
	
	tlm_utils::peq_with_get<UnifiedPayload> unified_payload_queue;
	PayloadFabric<UnifiedPayload> unified_payload_fabric;
	PayloadFabric<TLMInterruptPayload> interrupt_payload_fabric;

	void ProcessCPUPayload(UnifiedPayload *unified_payload);
	void ProcessIntrPayload(UnifiedPayload *unified_payload);
};

} // end of namespace xps_intc
} // end of namespace xilinx
} // end of namespace interrupt
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif
