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

#define LOCATION 	" - location = " << __FUNCTION__ << ":unisim_lib/unisim/component/tlm2/interrupt/str7_eic/str7_eic.tcc:" << __LINE__
#define TIME(X) 	" - time = " << sc_time_stamp() + (X)
#define PHASE(X) 	" - phase = " << 	( (X) == tlm::BEGIN_REQ  ? 	"BEGIN_REQ" : \
										( (X) == tlm::END_REQ    ? 	"END_REQ" : \
										( (X) == tlm::BEGIN_RESP ? 	"BEGIN_RESP" : \
										( (X) == tlm::END_RESP   ?  "END_RESP" : \
										  							"UNINITIALIZED_PHASE"))))
#define TRANS(X)	" - trans.level = " << (X).level;

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
		irq_status(0),
		fiq_status(0),
		fsm_state(READY),
		stack_depth(0),
		irq_fifo("irq_fifo"),
		fiq_fifo("fiq_fifo"),
		base_address(0),
		param_base_address("base-address", this, base_address),
		logger(*this),
		verbose_all(false),
		verbose_setup(false),
		verbose_run(false)
{
	SC_HAS_PROCESS(STR7_EIC);

	SC_THREAD(IRQFifoHandler);
	SC_THREAD(FIQFifoHandler);

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

template <unsigned int BUS_WIDTH, bool VERBOSE>
STR7_EIC<BUS_WIDTH, VERBOSE> ::
~STR7_XTI()
{
	if (param_verbose_all) delete param_verbose_all;
	if (param_verbose_setup) delete param_verbose_setup;
	if (param_verbose_run) delete param_verbose_run;
}
	
template <unsigned int BUS_WIDTH, bool VERBOSE>
bool
STR7_EIC<BUS_WIDTH, VERBOSE> ::
Setup()
{
	if (VERBOSE)
	{
		if (verbose_all) verbose_setup = verbose_run = true;
	}

	if (VerboseSetup())
	{
		logger << DebugInfo << "Setup:" << endl;
		if (VerboseAll()) logger << " - verbose-all activated" << endl;
		if (VerboseSetup()) logger << " - verbose-setup activated" << endl;
		if (VerboseRun()) logger << " - verbose-run activated" << endl;

		logger << " - base address = 0x" << hex << base_address << dec;
		logger << EndDebugInfo;
	}

	return true;
}

/* START: callback methods for the in_irq sockets */

template <unsigned int BUS_WIDTH, bool VERBOSE>
tlm::tlm_sync_enum
STR7_EIC<BUS_WIDTH, VERBOSE> ::
InIRQNb(int index, TLMInterruptPayload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t)
{
	switch (phase)
	{
		case tlm::BEGIN_REQ:
			IRQ(index, trans.level, t);
			break;
		default:
			logger << DebugWarning << "Received unexpected transaction phase on in_irq[" << index << "]:" << endl
				<< LOCATION << endl
				<< TRANS(trans) << endl
				<< PHASE(phase) << endl
				<< TIME(t)
				<< EndDebugWarning;
			break;
	}

	return tlm::TLM_COMPLETED;
}

template <unsigned int BUS_WIDTH, bool VERBOSE>
void
STR7_EIC<BUS_WIDTH, VERBOSE> ::
InIRQB(int index, TLMInterruptPayload& trans, sc_core::sc_time& t)
{
	IRQ(index, trans.level, t);
}

template <unsigned int BUS_WIDTH, bool VERBOSE>
unsigned int
STR7_EIC<BUS_WIDTH, VERBOSE> ::
InIRQDbg(int index, TLMInterruptPayload& trans)
{
	/* This should never be called */
	return 0;
}

template <unsigned int BUS_WIDTH, bool VERBOSE>
bool
STR7_EIC<BUS_WIDTH, VERBOSE> ::
InIRQDmi(int index, TLMInterruptPayload& trans, tlm::tlm_dmi& dmi)
{
	/* This should never be called */
	return false;
}

/* END: callback methods for the in_irq sockets */

/* START: callback methods for the in_fiq sockets */

template <unsigned int BUS_WIDTH, bool VERBOSE>
tlm::tlm_sync_enum
STR7_EIC<BUS_WIDTH, VERBOSE> ::
InFIQNb(int index, TLMInterruptPayload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t)
{
	switch (phase)
	{
		case tlm::BEGIN_REQ:
			FIQ(index, trans.level, t);
			break;
		default:
			logger << DebugWarning << "Received unexpected transaction phase on in_fiq[" << index << "]:" << endl
				<< LOCATION << endl
				<< TRANS(trans) << endl
				<< PHASE(phase) << endl
				<< TIME(t)
				<< EndDebugWarning;
			break;
	}

	return tlm::TLM_COMPLETED;
}

template <unsigned int BUS_WIDTH, bool VERBOSE>
void
STR7_EIC<BUS_WIDTH, VERBOSE> ::
InFIQB(int index, TLMInterruptPayload& trans, sc_core::sc_time& t)
{
	FIQ(index, trans.level, t);
}

template <unsigned int BUS_WIDTH, bool VERBOSE>
unsigned int
STR7_EIC<BUS_WIDTH, VERBOSE> ::
InFIQDbg(int index, TLMInterruptPayload& trans)
{
	/* This should never be called */
	return 0;
}

template <unsigned int BUS_WIDTH, bool VERBOSE>
bool
STR7_EIC<BUS_WIDTH, VERBOSE> ::
InFIQDmi(int index, TLMInterruptPayload& trans, tlm::tlm_dmi& dmi)
{
	/* This should never be called */
	return 0;
}

/* END: callback methods for the in_fiq sockets */

/* START: callback methods for the out_irq sockets */

template <unsigned int BUS_WIDTH, bool VERBOSE>
tlm::tlm_sync_enum
STR7_EIC<BUS_WIDTH, VERBOSE> ::
OutIRQNb(TLMInterruptPayload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t)
{
	switch (phase)
	{
		case tlm::END_REQ:
			/* nothing to do, just indicate that the transaction has been completed */
			trans.release();
			return tlm::TLM_COMPLETED;
			break;
		case tlm::BEGIN_REQ:
		case tlm::BEGIN_RESP:
		case tlm::END_RESP:
			/* unexpected phase received */
			logger << DebugError << "Unexpected phase received on out_irq:" << endl
				<< LOCATION << endl
				<< TRANS(trans) << endl
				<< PHASE(phase) << endl
				<< TIME(t) << EndDebugError;
			sc_stop();
			wait();
			break;
	}
	// this code should never be executed
	logger << DebugError << "This code should never be executed" << endl
		<< LOCATION << endl
		<< TRANS(trans) << endl
		<< PHASE(phase) << endl
		<< TIME(t) << EndDebugError;
	sc_stop();
	wait();
	return tlm::TLM_COMPLETED;
}

template <unsigned int BUS_WIDTH, bool VERBOSE>
void
STR7_EIC<BUS_WIDTH, VERBOSE> ::
OutIRQDMI(sc_dt::uint64 start_range, sc_dt::uint64 end_range)
{
	/* This method shoud never be used */
}

/* END: callback methods for the out_irq sockets */
	
/* START: callback methods for the out_fiq sockets */

template <unsigned int BUS_WIDTH, bool VERBOSE>
tlm::tlm_sync_enum
STR7_EIC<BUS_WIDTH, VERBOSE> ::
OutFIQNb(TLMInterruptPayload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t)
{
	switch (phase)
	{
		case tlm::END_REQ:
			/* nothing to do, just indicate that the transaction has been completed */
			trans.release();
			return tlm::TLM_COMPLETED;
			break;
		case tlm::BEGIN_REQ:
		case tlm::BEGIN_RESP:
		case tlm::END_RESP:
			/* unexpected phase received */
			logger << DebugError << "Unexpected phase received on out_fiq:" << endl
				<< LOCATION << endl
				<< TRANS(trans) << endl
				<< PHASE(phase) << endl
				<< TIME(t) << EndDebugError;
			sc_stop();
			wait();
			break;
	}
	// this code should never be executed
	logger << DebugError << "This code should never be executed" << endl
		<< LOCATION << endl
		<< TRANS(trans) << endl
		<< PHASE(phase) << endl
		<< TIME(t) << EndDebugError;
	sc_stop();
	wait();
	return tlm::TLM_COMPLETED;
}

template <unsigned int BUS_WIDTH, bool VERBOSE>
void
STR7_EIC<BUS_WIDTH, VERBOSE> ::
OutFIQDMI(sc_dt::uint64 start_range, sc_dt::uint64 end_range)
{
	/* This method should never be used */
}

/* END: callback methods for the out_fiq sockets */

/* START: methods implementing the "in_mem" socket */

template <unsigned int BUS_WIDTH, bool VERBOSE>
void
STR7_EIC<BUS_WIDTH, VERBOSE> ::
b_transport(tlm::tlm_generic_payload& trans, sc_core::sc_time& t)
{
	/* TODO */
}

template <unsigned int BUS_WIDTH, bool VERBOSE>
tlm::tlm_sync_enum
STR7_EIC<BUS_WIDTH, VERBOSE> ::
nb_transport_fw(tlm::tlm_generic_payload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t)
{
	/* TODO */
	return tlm::TLM_COMPLETED;
}

template <unsigned int BUS_WIDTH, bool VERBOSE>
bool
STR7_EIC<BUS_WIDTH, VERBOSE> ::
get_direct_mem_ptr(tlm::tlm_generic_payload& trans, tlm::tlm_dmi& dmi_data)
{
	/* This method should never be used */
	return false;
}

template <unsigned int BUS_WIDTH, bool VERBOSE>
unsigned int
STR7_EIC<BUS_WIDTH, VERBOSE> ::
transport_dbg(tlm::tlm_generic_payload& trans)
{
	/* This method should never be used */
	return 0;
}

/* END: methods implementing the "in_mem" socket */

/** Puts an incomming IRQ into the fifo queue to be handled when time has come to do it.
 * @param index		the port that received the changed IRQ
 * @param level		interrupt level
 * @param t			the time that the interrupt is received
 */
template <unsigned int BUS_WIDTH, bool VERBOSE>
void
STR7_EIC<BUS_WIDTH, VERBOSE> ::
IRQ(unsigned int index, bool level, sc_core::sc_time& t)
{
	struct irq_fifo_t *entry = new irq_fifo_t();
	entry->index = index;
	entry->level = level;
	irq_fifo.notify(*entry, t);
}

/** Handle the incomming irqs at the right time */
template <unsigned int BUS_WIDTH, bool VERBOSE>
void
STR7_EIC<BUS_WIDTH, VERBOSE> ::
IRQFifoHandler()
{
	sc_core::sc_event &ev = irq_fifo.get_event();
	struct irq_fifo_t *entry = 0;

	while (1)
	{
		wait(ev);
		entry = irq_fifo.get_next_transaction();
		if (entry != 0)
		{
			unsigned int index = entry->index;
			bool level = entry->level;
			delete entry;
			IRQ(index, level);
		}
	}
}

/** Puts an incomming FIQ into the fifo queue to be handled when time has come to do it.
 * @param index		the port that received the changed FIQ
 * @param level		interrupt level
 * @param t			the time that the interrupt is received
 */
template <unsigned int BUS_WIDTH, bool VERBOSE>
void
STR7_EIC<BUS_WIDTH, VERBOSE> ::
FIQ(unsigned int index, bool level, sc_core::sc_time& t)
{
	struct irq_fifo_t *entry = new irq_fifo_t();
	entry->index = index;
	entry->level = level;
	fiq_fifo.notify(*entry, t);
}

/** Handle the incomming fiqs at the right time */
template <unsigned int BUS_WIDTH, bool VERBOSE>
void
STR7_EIC<BUS_WIDTH, VERBOSE> ::
FIQFifoHandler()
{
	sc_core::sc_event &ev = fiq_fifo.get_event();
	struct irq_fifo_t *entry = 0;

	while (1)
	{
		wait(ev);
		entry = fiq_fifo.get_next_transaction();
		if (entry != 0)
		{
			unsigned int index = entry->index;
			bool level = entry->level;
			delete entry;
			FIQ(index, level);
		}
	}
}

/** Process an incomming IRQ signal change.
 * @param index		the port that received the changed IRQ
 * @param level		interrupt level: true = interruption; false = no interruption
 */
template <unsigned int BUS_WIDTH, bool VERBOSE>
void
STR7_EIC<BUS_WIDTH, VERBOSE> ::
IRQ(unsigned int index, bool level)
{
	if (level)
		irq_status |= ((uint32_t)1 << index);
	else
		irq_status &= ~((uint32_t)1 << index);
	FSMUpdate();
}

/** Process an incomming FIQ signal change.
 * @param index		the port that received the change FIQ
 * @param level		interrupt level: true = interruption; false = no interruption
 */
template <unsigned int BUS_WIDTH, bool VERBOSE>
void
STR7_EIC<BUS_WIDTH, VERBOSE> ::
FIQ(unsigned int index, bool level)
{
	if (level)
		fiq_status |= ((uint32_t)1 << index);
	else
		fiq_status &= ~((uint32_t)1 << index);
	FSMUpdate();
}

/** Update the FSM. */
template <unsigned int BUS_WIDTH, bool VERBOSE>
void
STR7_EIC<BUS_WIDTH, VERBOSE> ::
FSMUpdate()
{
	/* update the pending interrupt register 0 (IPR0) */
	ipr0 |= IER0() & irq_status;

	/* get the highest priority interrupt if any */
	bool has_irq = false;
	unsigned int index;
	if (ipr0)
	{
		for (int i = 0; i < NUM_IRQ; i++)
		{
			if (ipr0 & ((uint32_t)1 << i))
			{
				if (SIPL(i) > CIPR() &&
					has_irq ? SIPL(i) >= SIPL(index) : true)
				{
					has_irq = true;
					index = i;
				}
			}
		}
	}

	if (has_irq) 
	{
		/* update the IVR registers */
		ivr &= (uint32_t)0xFFFF0000;
		ivr |= SIV(index);

		/* change the FSM state when an irq is detected */
		FSMWait();

		/* the interrupt is signaled only if global irqs (IRQ_EN) are activated */
		has_irq = IRQ_EN();
	}

	/* send the interrupt level through the out_irq port */
	TLMInterruptPayload *trans = irqPayloadFabric.allocate();
	trans->level = has_irq;
	tlm::tlm_phase phase = tlm::BEGIN_REQ;
	sc_core::sc_time t = SC_ZERO_TIME;
	switch (out_irq->nb_transport_fw(*trans, phase, t))
	{
		case tlm::TLM_ACCEPTED:
		case tlm::TLM_UPDATED:
			/* nothing else to do wait for the response (which does nothing) */
			break;
		case tlm::TLM_COMPLETED:
			/* release the transaction */
			trans->release();
			break;
	}

	/* the same process is done for the FIQ */
	/* update the pending interrupt register 0 (IPR0) */
	fir = fir | ((FIE() & fiq_status) << 2);

	/* send the interrupt level through the out_fiq port */
	TLMInterruptPayload *trans_fiq = irqPayloadFabric.allocate();
	trans_fiq->level = FIQ_EN() && FIP();
	phase = tlm::BEGIN_REQ;
	t = SC_ZERO_TIME;
	switch (out_fiq->nb_transport_fw(*trans_fiq, phase, t))
	{
		case tlm::TLM_ACCEPTED:
		case tlm::TLM_UPDATED:
			/* nothing else to do, wait for the response (which does nothing) */
			break
		case tlm::TLM_COMPLETED:
			/* release the transaction */
			trans->release();
			break;
	}
}

/** Read register method and update interrupt controller state as necessary.
 * @param addr		the register address
 * @return 			the contents of the register
 */
template <unsigned int BUS_WIDTH, bool VERBOSE>
uint32_t 
STR7_EIC<BUS_WIDTH, VERBOSE> ::
ReadRegister(uint32_t const addr)
{
	uint32_t index = addr;
	if (addr >= 0x60 && addr <= 0xdc) index = 0x60;
	switch (addr)
	{
		case 0x0: // ICR always reads 0
			return 0;
			break;
		case 0x04: // CICR
			return CICR();
			break;
		case 0x08: // CIPR
			return CIPR();
			break;
		case 0x18: // IVR
			if (IsFSMWait())
			{
				Push(CICR, CIPR);
				cicr = new_irq;
				cipr = SIPL(new_irq);
			}
			FSMReady();
			return IVR();
			break;
		case 0x1c: // FIR
			return FIR();
			break;
		case 0x20: // IER0
			return IER0();
			break;
		case 0x40: // IPR
			return IPR();
			break;
		case 0x60: // SIR
			unsigned int i = (addr - 0x60) / 4;
			return SIR(i);
			break;
		default:
			return 0;
			break;
	}
	return 0;
}

/** Write register method and update interrupt controller state as necessary.
 * @param addr		the register address
 * @param value		the data to write into the register
 */
template <unsigned int BUS_WIDTH, bool VERBOSE>
void 
STR7_EIC<BUS_WIDTH, VERBOSE> ::
WriteRegister(uint32_t addr, uint32_t value)
{
	unsigned int irq;
	unsigned int has_irq = false;

	uint32_t index = addr;
	if (addr >= 0x60 && addr <= 0xdc) index = 0x60;
	switch (addr)
	{
		case 0x0: // ICR
			icr = value & 0x3;
			FSMUpdate();
			break;
		case 0x04: // CICR
			/* can not be modified, ignore write */
			break;
		case 0x08: // CIPR
			if ((value & 0x0f) > CIPR())
				cipr = (value & 0x0f);
			break;
		case 0x18: // IVR
			ivr = value & (uint32_t)0xffff0000ULL;
			break;
		case 0x1c: // FIR
			if (value & 0x3) // modifies FIE
			{
				fir &= ~(uint32_t)0x3;
				fir |= (value & (uint32_t)0x3);
			}
			if ((value >> 2) & 0x3) // modifies FIP
			{
				for (unsigned int i = 0; i < NUM_FIQ; i++)
				{
					if (value & (0x01 << (i + 2)) && FIP() & (0x01 << i))
						fir = fir & ~((uint32_t)0x01 << (i + 2));
				}
				FSMUpdate();
			}
			break;
		case 0x20: // IER0
			ier0 = value;
			break;
		case 0x40: // IPR0
			if (value)
			{
				for (unsigned int i = 0; i < NUM_IRQ; i++)
				{
					if (value & ((uint32_t)0x01 << i)) && (IPR0() & ((uint32_t)0x01 << i))
					{
						if (!has_irq)
						{
							has_irq = true;
							irq = i;
						}
						ipr0 = ipr0 & ~((uint32_t)0x01 << i);
					}
				}
			}
			if (has_irq)
			{
				if (irq == CICR())
				{
					Pop(cicr, cipr);
				}
			}
			FSMUpdate();
			break;
		case 0x60: // SIR
			break;
		default:
			break;
	}
}

/* START: interrupt controller accessors */

template <unsigned int BUS_WIDTH, bool VERBOSE>
uint32_t
STR7_EIC<BUS_WIDTH, VERBOSE> ::
ICR()
{
	return icr;
}

template <unsigned int BUS_WIDTH, bool VERBOSE>
uint32_t
STR7_EIC<BUS_WIDTH, VERBOSE> ::
CICR()
{
	return cicr;
}

template <unsigned int BUS_WIDTH, bool VERBOSE>
uint32_t
STR7_EIC<BUS_WIDTH, VERBOSE> ::
CIPR()
{
	return cipr;
}

template <unsigned int BUS_WIDTH, bool VERBOSE>
uint32_t
STR7_EIC<BUS_WIDTH, VERBOSE> ::
IVR()
{
	return ivr;
}

template <unsigned int BUS_WIDTH, bool VERBOSE>
uint32_t
STR7_EIC<BUS_WIDTH, VERBOSE> ::
FIR()
{
	return fir;
}

template <unsigned int BUS_WIDTH, bool VERBOSE>
uint32_t
STR7_EIC<BUS_WIDTH, VERBOSE> ::
IER0()
{
	return ier0;
}

template <unsigned int BUS_WIDTH, bool VERBOSE>
uint32_t
STR7_EIC<BUS_WIDTH, VERBOSE> ::
IPR0()
{
	return ipr0;
}

template <unsigned int BUS_WIDTH, bool VERBOSE>
uint32_t
STR7_EIC<BUS_WIDTH, VERBOSE> ::
SIR(unsigned int index)
{
	return sir[index];
}

template <unsigned int BUS_WIDTH, bool VERBOSE>
bool
STR7_EIC<BUS_WIDTH, VERBOSE> ::
FIQ_EN()
{
	return ICR() & (0x01 << 1);
}

template <unsigned int BUS_WIDTH, bool VERBOSE>
bool
STR7_EIC<BUS_WIDTH, VERBOSE> ::
IRQ_EN()
{
	return ICR() & 0x01;
}

template <unsigned int BUS_WIDTH, bool VERBOSE>
uint32_t
STR7_EIC<BUS_WIDTH, VERBOSE> ::
CIC()
{
	return CICR() & (uint32_t)0x01f;
}

template <unsigned int BUS_WIDTH, bool VERBOSE>
uint32_t
STR7_EIC<BUS_WIDTH, VERBOSE> ::
CIP()
{
	return CIPR() & (uint32_t)0x0f;
}

template <unsigned int BUS_WIDTH, bool VERBOSE>
uint32_t
STR7_EIC<BUS_WIDTH, VERBOSE> ::
IVRhigh()
{
	return IVR() >> 16;
}

template <unsigned int BUS_WIDTH, bool VERBOSE>
uint32_t
STR7_EIC<BUS_WIDTH, VERBOSE> ::
IVRlow()
{
	return IVR() & (uint32_t)0x0ffff;
}

template <unsigned int BUS_WIDTH, bool VERBOSE>
uint32_t
STR7_EIC<BUS_WIDTH, VERBOSE> ::
FIP()
{
	return (FIR() >> 2) & (uint32_t)0x03;
}

template <unsigned int BUS_WIDTH, bool VERBOSE>
uint32_t
STR7_EIC<BUS_WIDTH, VERBOSE> ::
FIE()
{
	return FIR() & (uint32_t)0x03;
}

template <unsigned int BUS_WIDTH, bool VERBOSE>
uint32_t
STR7_EIC<BUS_WIDTH, VERBOSE> ::
SIV(unsigned int index)
{
	return SIR(index) >> 16;
}

template <unsigned int BUS_WIDTH, bool VERBOSE>
uint32_t
STR7_EIC<BUS_WIDTH, VERBOSE> ::
SIPL(unsigned int index)
{
	return SIR(index) & (uint32_t)0x0f;
}

/* END: interrupt controller accessors */

/* START: verbose methods */

template <unsigned int BUS_WIDTH, bool VERBOSE>
bool
STR7_EIC<BUS_WIDTH, VERBOSE> ::
VerboseAll()
{
	return VERBOSE && verbose_all;
}

template <unsigned int BUS_WIDTH, bool VERBOSE>
bool
STR7_EIC<BUS_WIDTH, VERBOSE> ::
VerboseSetup()
{
	return VERBOSE && verbose_setup;
}

template <unsigned int BUS_WIDTH, bool VERBOSE>
bool
STR7_EIC<BUS_WIDTH, VERBOSE> ::
VerboseRun()
{
	return VERBOSE && verbose_run;
}

/* END: verbose methods */

} // end of namespace str7_eic
} // end of namespace interrupt
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_INTERRUPT_STR7_EIC_STR7_EIC_TCC__


