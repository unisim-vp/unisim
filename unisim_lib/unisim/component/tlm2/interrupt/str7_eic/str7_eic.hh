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

#ifndef __UNISIM_COMPONENT_TLM2_INTERRUPT_STR7_EIC_STR7_EIC_HH__
#define __UNISIM_COMPONENT_TLM2_INTERRUPT_STR7_EIC_STR7_EIC_HH__

#include <systemc.h>
#include <tlm.h>
#include <tlm_utils/passthrough_target_socket.h>
#include <tlm_utils/simple_initiator_socket.h>
#include <tlm_utils/peq_with_get.h>
#include <inttypes.h>

#include "unisim/kernel/service/service.hh"
#include "unisim/kernel/logger/logger.hh"
#include "unisim/component/tlm2/interrupt/types.hh"

namespace unisim {
namespace component {
namespace tlm2 {
namespace interrupt {
namespace str7_eic {

using unisim::component::tlm2::interrupt::InterruptProtocolTypes;
using unisim::component::tlm2::interrupt::TLMInterruptPayload;
using unisim::kernel::service::Parameter;
using unisim::kernel::service::Object;
using unisim::kernel::logger::Logger;
using unisim::kernel::tlm2::PayloadFabric;

template <typename MODULE>
class IRQTargetSocket :
	public tlm_utils::passthrough_target_socket_tagged<MODULE, 1, InterruptProtocolTypes>
{
public:
	IRQTargetSocket() : 
		tlm_utils::passthrough_target_socket_tagged<MODULE, 1, InterruptProtocolTypes>("in_irq") {}
};

template <typename MODULE>
class FIQTargetSocket :
	public tlm_utils::passthrough_target_socket_tagged<MODULE, 1, InterruptProtocolTypes>
{
public:
	FIQTargetSocket() : 
		tlm_utils::passthrough_target_socket_tagged<MODULE, 1, InterruptProtocolTypes>("in_fiq") {}
};
	
static const unsigned int NUM_IRQ = 32;
static const unsigned int NUM_FIQ = 2;
static const unsigned int STACK_DEPTH = 15;

template <unsigned int BUS_WIDTH = 32,
		  bool VERBOSE = false>
class STR7_EIC :
	public Object,
	public sc_module,
	public tlm::tlm_fw_transport_if<>
{
private:
	typedef STR7_EIC<BUS_WIDTH, VERBOSE> THIS_MODULE;

public:
	/* input socket for incomming IRQs */
	IRQTargetSocket<THIS_MODULE> in_irq[NUM_IRQ];
	/* input socket for incomming FIQs */
	FIQTargetSocket<THIS_MODULE> in_fiq[NUM_FIQ];
	/* output socket for outgoing IRQ */
	tlm_utils::simple_initiator_socket<THIS_MODULE, 1, InterruptProtocolTypes> out_irq;
	/* output socket for outgoing FIQ */
	tlm_utils::simple_initiator_socket<THIS_MODULE, 1, InterruptProtocolTypes> out_fiq;

	/* input socket for memory transactions */
	tlm::tlm_target_socket<BUS_WIDTH> in_mem;

	STR7_EIC(const sc_module_name& name, Object* parent = 0);
	virtual ~STR7_EIC();

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
	tlm::tlm_sync_enum OutIRQNb(TLMInterruptPayload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t);
	void OutIRQDMI(sc_dt::uint64 start_range, sc_dt::uint64 end_range);
	/* END: callback methods for the out_irq sockets */
	
	/* START: callback methods for the out_fiq sockets */
	tlm::tlm_sync_enum OutFIQNb(TLMInterruptPayload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t);
	void OutFIQDMI(sc_dt::uint64 start_range, sc_dt::uint64 end_range);
	/* END: callback methods for the out_fiq sockets */

	/* START: methods implementing the "in_mem" socket */
	virtual void b_transport(tlm::tlm_generic_payload& trans, sc_core::sc_time& t);
	virtual tlm::tlm_sync_enum nb_transport_fw(tlm::tlm_generic_payload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t);
	virtual bool get_direct_mem_ptr(tlm::tlm_generic_payload& trans, tlm::tlm_dmi& dmi_data);
	virtual unsigned int transport_dbg(tlm::tlm_generic_payload& trans);
	/* END: methods implementing the "in_mem" socket */

private:
	/** Process an incomming IRQ signal change.
	 * @param index		the port that received the changed IRQ
	 * @param level		interrupt level: true = interruption; false = no interruption
	 */
	void IRQ(unsigned int index, bool level = true);

	/** Puts an incomming IRQ into the fifo queue to be handled when time has come to do it.
	 * @param index		the port that received the changed IRQ
	 * @param level		interrupt level
	 * @param t			the time that the interrupt is received
	 */
	void IRQ(unsigned int index, bool level, sc_core::sc_time& t);

	/** Handle the incomming irqs at the right time */
	void IRQFifoHandler();

	/** Process an incomming FIQ signal change.
	 * @param index		the port that received the change FIQ
	 * @param level		interrupt level: true = interruption; false = no interruption
	 */
	void FIQ(unsigned int index, bool level = true);

	/** Puts an incomming FIQ into the fifo queue to be handled when time has come to do it.
	 * @param index		the port that received the changed FIQ
	 * @param level		interrupt level
	 * @param t			the time that the interrupt is received
	 */
	void FIQ(unsigned int index, bool level, sc_core::sc_time& t);

	/** Handle the incomming fiqs at the right time */
	void FIQFifoHandler();

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

	/** Read register method and update interrupt controller state as necessary.
	 * @param addr		the register address
	 * @return 			the contents of the register
	 */
	uint32_t ReadRegister(uint32_t addr);
	/** Write register method and update interrupt controller state as necessary.
	 * @param addr		the register address
	 * @param value		the data to write into the register
	 */
	void WriteRegister(uint32_t addr, uint32_t value);

	/* interrupt signals status */
	uint32_t irq_status;
	uint32_t fiq_status;
	struct irq_fifo_t {
		unsigned int index;
		bool level;
	};
	tlm_utils::peq_with_get<irq_fifo_t> irq_fifo;
	tlm_utils::peq_with_get<irq_fifo_t> fiq_fifo;
	
	/* START: FSM states */
	/** The possible FSM states */
	enum fsm_state_t {READY, WAIT};
	/** The current FSM state */
	fsm_state_t fsm_state;
	/** Set FSM to READY state */
	void FSMReady();
	/** Set FSM to WAIT state */
	void FSMWait();
	/** Is the FSM at the READY state? */
	bool IsFSMReady();
	/** Is the FSM at the WAIT state? */
	bool IsFSMWait();
	/** Update the FSM */
	void FSMUpdate();
	/* END: FSM states */

	/* START: stack entries */
	uint32_t new_irq;
	struct stack_entry_t {
		uint32_t cicr;
		uint32_t cipr;
	};
	stack_entry_t stack[STACK_DEPTH];
	unsigned int stack_depth;
	/** Pushes the CICR and the CIPR into the stack */
	void Push();
	/** Pops the top of the stack from the stack */
	void Pop();
	/* END: stack entries */

	/* the interrupt payload fabric */
	PayloadFabric<TLMInterruptPayload> irqPayloadFabric;

	/* START: module parameters */
	uint64_t base_address;
	unisim::kernel::service::Parameter<uint64_t> param_base_address;
	/* END: module parameters */

	/* START: logger and logger methods and verbose parameters/methods */
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
	/* END: logger and logger methods and verbose parameters/methods */
};

} // end of namespace str7_eic
} // end of namespace interrupt
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_INTERRUPT_STR7_EIC_STR7_EIC_HH__

