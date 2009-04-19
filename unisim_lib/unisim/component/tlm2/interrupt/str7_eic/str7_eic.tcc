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

#ifndef __UNISIM_COMPONENT_TLM2_INTERRUPT_STR7_EIC_STR7_EIC_TCC__
#define __UNISIM_COMPONENT_TLM2_INTERRUPT_STR7_EIC_STR7_EIC_TCC__

namespace unisim {
namespace component {
namespace tlm2 {
namespace interrupt {
namespace str7_eic {

template <unsigned int BUS_WIDTH, bool VERBOSE>
STR7_EIC<BUS_WIDTH, VERBOSE> ::
STR7_EIC(const sc_module_name& name, Object* parent) :
		Object(name, parent),
		sc_module(name),
		out_irq("out_irq"),
		out_fiq("out_fiq"),
		in_mem("in_mem"),
		icr(0),
		cicr(0),
		cipr(0),
		ivr(0),
		fir(0),
		ier0(0),
		ipr0(0),
		base_address(0),
		param_base_address("base-address", this, base_address),
		logger(*this),
		verbose_all(false),
		verbose_setup(false),
		verbose_run(false)
{
	if (VERBOSE)
	{
		param_verbose_all = new unisim::kernel::service::Parameter<bool>("verbose-all", this, verbose_all);
		param_verbose_setup = new unisim::kernel::service::Parameter<bool>("verbose-setup", this, verbose_setup);
		param_verbose_run = new unisim::kernel::service::Parameter<bool>("verbose-run", this, verbose_run);
	}

	/* bind the in_mem socket to the methods implementations provided by the module */
	in_mem.bind(*this);
	/* register the in_irq sockets to the methods implementations provided by the module */
	for (unsigned int i = 0; i < NUM_IRQ; i++)
	{
		in_irq[i].register_nb_transport_fw(this, &THIS_MODULE::InIRQNb, i);
		in_irq[i].register_b_transport(this, &THIS_MODULE::InIRQB, i);
		in_irq[i].register_transport_dbg(this, &THIS_MODULE::InIRQDbg, i);
		in_irq[i].register_get_direct_mem_ptr(this, &THIS_MODULE::InIRQDmi, i);
	}
	/* register the in_fiq sockets to the methods implementations provided by the module */
	for (unsigned int i = 0; i < NUM_FIQ; i++)
	{
		in_fiq[i].register_nb_transport_fw(this, &THIS_MODULE::InFIQNb, i);
		in_fiq[i].register_b_transport(this, &THIS_MODULE::InFIQB, i);
		in_fiq[i].register_transport_dbg(this, &THIS_MODULE::InFIQDbg, i);
		in_fiq[i].register_get_direct_mem_ptr(this, &THIS_MODULE::InFIQDmi, i);
	}
	/* register the out_irq socket to the methods implementation provided by the module */
	out_irq.register_nb_transport_bw(this, &THIS_MODULE::OutIRQNb);
	out_irq.register_invalidate_direct_mem_ptr(this, &THIS_MODULE::OutIRQDMI);
	/* register the out_fiq socket to the methods implementation provided by the module */
	out_fiq.register_nb_transport_bw(this, &THIS_MODULE::OutFIQNb);
	out_fiq.register_invalidate_direct_mem_ptr(this, &THIS_MODULE::OutFIQDMI);
}

template <unsigned int BUS_WIDTH = 32,
		  bool VERBOSE = false>
class STR7_EIC :
	public Object,
	public sc_module
{
public:
	typedef STR7_EIC<BUS_WIDTH, VERBOSE> THIS_MODULE;

	/* input socket for incomming IRQs */
	IRQTargetSocket<THIS_MODULE> in_irq[32];
	/* input socket for incomming FIQs */
	FIQTargetSocket<THIS_MODULE> in_fiq[2];
	/* output socket for outgoing IRQ */
	tlm_utils::simple_initiator_socket<THIS_MODULE, 1, InterruptProtocolTypes> out_irq;
	/* output socket for outgoing FIQ */
	tlm_utils::simple_initiator_socket<THIS_MODULE, 1, InterruptProtocolTypes> out_fiq;

	/* input socket for memory transactions */
	tlm::tlm_target_socket<BUS_WIDTH> in_mem;

	STR7_ECI(const sc_module_name& name, Object* parent = 0);
	virtual ~STR7_XTI();

	virtual bool Setup();

	/* START: callback methods for the in_irq sockets */
	tlm::tlm_sync_enum InIRQNb(int index, TLMInterruptPayload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t);
	void InIRQB(int index, TLMInterruptPayload& trans, sc_core::sc_time& t);
	unsigned int InIRQDbg(int index, TLMInterruptPayload& trans);
	bool InIRQDmi(int index, TLMInterruptPayload& trans, tlm::tlm_dmi& dmi);
	/* END: callback methods for the in_irq sockets */

	/* START: callback methods for the in_fiq sockets */
	tlm::tlm_sync_enum InFIQNb(int index, TLMInterruptPayload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t);
	void InFIQB(int index, TLMInterruptPayload& trans, sc_core::sc_time& t);
	unsigned int InFIQDbg(int index, TLMInterruptPayload& trans);
	bool InFIQDmi(int index, TLMInterruptPayload& trans, tlm::tlm_dmi& dmi);
	/* END: callback methods for the in_fiq sockets */

	/* START: callback methods for the out_irq sockets */
	tlm::tlm_sync_enum OutIRQNb(int index, TLMInterruptPayload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t);
	void OutIRQDMI(int index, sc_dt::uint64 start_range, sc_dt::uint64 end_range);
	/* END: callback methods for the out_irq sockets */
	
	/* START: callback methods for the out_fiq sockets */
	tlm::tlm_sync_enum OutFIQNb(int index, TLMInterruptPayload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t);
	void OutFIQDMI(int index, sc_dt::uint64 start_range, sc_dt::uint64 end_range);
	/* END: callback methods for the out_fiq sockets */

	/* START: methods implementing the "in_mem" socket */
	virtual void b_transport(tlm::tlm_generic_payload& trans, sc_core::sc_time& t);
	virtual tlm::tlm_sync_enum nb_transport_fw(tlm::tlm_generic_payload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t);
	virtual bool get_direct_mem_ptr(tlm::tlm_generic_payload& trans, tlm::tlm_dmi& dmi_data);
	virtual unsigned int transport_dbg(tlm::tlm_generic_payload& trans);
	/* END: methods implementing the "in_mem" socket */

private:
	/* interrupt controller registers */
	uint32_t icr;
	uint32_t cicr;
	uint32_t cipr;
	uint32_t ivr;
	uint32_t fir;
	uint32_t ier0;
	uint32_t ipr0;
	uint32_t sir[32];

	/* interrupt controller accessors */
	uint32_t ICR();
	uint32_t CICR();
	uint32_t CIPR();
	uint32_t IVR();
	uint32_t FIR();
	uint32_t IER0();
	uint32_t IPR0();
	uint32_t SIR(unsigned int index);

	bool FIQ_EN();
	bool IRQ_EN();
	uint32_t CIC();
	uint32_t CIP();
	uint32_t IVRhigh();
	uint32_t IVRlow();
	uint32_t FIP();
	uint32_t FIE();
	uint32_t SIV(unsigned int index);
	uint32_t SIPL(unsigned int index);

	/* module parameters */
	uint64_t base_address;
	unisim::kernel::service::Parameter<uint64_t> param_base_address;

	/* logger methods */
	unisim::kernel::logger::Logger logger;
	bool verbose_all;
	unisim::kernel::service::Parameter<bool> *param_verbose_all;
	bool verbose_setup;
	unisim::kernel::service::Parameter<bool> *param_verbose_setup;
	bool verbose_run;
	unisim::kernel::service::Parameter<bool> *param_verbose_run;
	inline bool VerboseAll();
	inline bool VerboseSetup();
	inline bool VerboseRun();
};

} // end of namespace str7_eic
} // end of namespace interrupt
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_INTERRUPT_STR7_EIC_STR7_EIC_HH__


