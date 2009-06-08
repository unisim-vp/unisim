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
 *
 * TODO:
 * - Add support for PWM (Pulse Width Modulation)
 * - Add support for output compare A and B
 */
 
#ifndef __UNISIM_COMPONENT_TLM2_TIMER_STR7_TIM_HH__
#define __UNISIM_COMPONENT_TLM2_TIMER_STR7_TIM_HH__

#include <systemc.h>
#include <tlm.h>
#include <tlm_utils/simple_initiator_socket.h>
#include <tlm_utils/passthrough_target_socket.h>
#include <tlm_utils/peq_with_get.h>
#include "unisim/kernel/service/service.hh"
#include "unisim/kernel/logger/logger.hh"
#include "unisim/component/tlm2/interrupt/types.hh"
#include "unisim/service/interfaces/trap_reporting.hh"

namespace unisim {
namespace component {
namespace tlm2 {
namespace timer {
namespace str7_timer {

using unisim::component::tlm2::interrupt::InterruptProtocolTypes;
using unisim::component::tlm2::interrupt::TLMInterruptPayload;
using unisim::kernel::service::Object;
using unisim::kernel::service::Client;
using unisim::kernel::service::ServiceImport;
using unisim::service::interfaces::TrapReporting;
using unisim::kernel::tlm2::PayloadFabric;

template<unsigned int BUSWIDTH = 32, bool VERBOSE = false>
class TIM :
	public sc_module,
	public tlm::tlm_fw_transport_if<>,
	public Client<TrapReporting>
{
private:
	typedef TIM<BUSWIDTH, VERBOSE> THIS_MODULE;

public:
	/* output timer global interrupt socket */
	tlm_utils::simple_initiator_socket<THIS_MODULE, 1, InterruptProtocolTypes> timeri_irq;
	/* output timer overflow interrupt socket */
	tlm_utils::simple_initiator_socket<THIS_MODULE, 1, InterruptProtocolTypes> toi_irq;
	/* output timer input capture A interrupt socket */
	tlm_utils::simple_initiator_socket<THIS_MODULE, 1, InterruptProtocolTypes> icia_irq;
	/* output timer input capture B interrupt socket */
	tlm_utils::simple_initiator_socket<THIS_MODULE, 1, InterruptProtocolTypes> icib_irq;
	/* output timer output compare A interrupt socket */
	tlm_utils::simple_initiator_socket<THIS_MODULE, 1, InterruptProtocolTypes> ocia_irq;
	/* output timer output compare B interrupt socket */
	tlm_utils::simple_initiator_socket<THIS_MODULE, 1, InterruptProtocolTypes> ocib_irq;

	/* output compare A (OCMPA) edge socket */
	tlm_utils::simple_initiator_socket<THIS_MODULE, 1, InterruptProtocolTypes> ocmpa_edge;
	/* output compare B (OCMPB) edge socket */
	tlm_utils::simple_initiator_socket<THIS_MODULE, 1, InterruptProtocolTypes> ocmpb_edge;
	
	/* input capture A (ICAPA) edge socket */
	tlm_utils::passthrough_target_socket<THIS_MODULE, 1, InterruptProtocolTypes> icapa_edge;
	/* input capture B (ICAPB) edge socket */
	tlm_utils::passthrough_target_socket<THIS_MODULE, 1, InterruptProtocolTypes> icapb_edge;

	/* input memory socket */
	tlm::tlm_target_socket<BUSWIDTH> in_mem;

	/* trap reporting import */
	ServiceImport<TrapReporting> trap_reporting_import;

	/* constructor */
	TIM(const sc_module_name &name, Object *parent = 0);

	/* destructor */
	~TIM();

	/* object setup */
	bool Setup();

private:
	/* START: output timer global interrupt socket methods */
	tlm::tlm_sync_enum 	TimeriNbTransportBw(TLMInterruptPayload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t);
	void 				TimeriInvalidateDirectMemPtr(sc_dt::uint64 start_range, sc_dt::uint64 end_range);
	/* END: output timer interrupt socket methods */

	/* START: output timer overflow interrupt (TOI) socket methods */
	tlm::tlm_sync_enum 	TOINbTransportBw(TLMInterruptPayload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t);
	void 				TOIInvalidateDirectMemPtr(sc_dt::uint64 start_range, sc_dt::uint64 end_range);
	/* END: output timer overflow interrupt (TOI) socket methods */

	/* START: output timer input capture A interrupt (ICIA) socket methods */
	tlm::tlm_sync_enum 	ICIANbTransportBw(TLMInterruptPayload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t);
	void 				ICIAInvalidateDirectMemPtr(sc_dt::uint64 start_range, sc_dt::uint64 end_range);
	/* END: output timer input capture A interrupt (ICIA) socket methods */

	/* START: output timer input capture B interrupt (ICIB) socket methods */
	tlm::tlm_sync_enum 	ICIBNbTransportBw(TLMInterruptPayload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t);
	void 				ICIBInvalidateDirectMemPtr(sc_dt::uint64 start_range, sc_dt::uint64 end_range);
	/* END: output timer input capture B interrupt (ICIB) socket methods */

	/* START: output timer output compare A interrupt (OCIA) socket methods */
	tlm::tlm_sync_enum 	OCIANbTransportBw(TLMInterruptPayload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t);
	void 				OCIAInvalidateDirectMemPtr(sc_dt::uint64 start_range, sc_dt::uint64 end_range);
	/* END: output timer output compare A interrupt (OCIA) socket methods */

	/* START: output timer output compare B interrupt (OCIB) socket methods */
	tlm::tlm_sync_enum 	OCIBNbTransportBw(TLMInterruptPayload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t);
	void 				OCIBInvalidateDirectMemPtr(sc_dt::uint64 start_range, sc_dt::uint64 end_range);
	/* END: output timer output compare B interrupt (OCIB) socket methods */

	/* START: output compare A (OCMPA) edge socket methods */
	tlm::tlm_sync_enum 	OCMPANbTransportBw(TLMInterruptPayload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t);
	void 				OCMPAInvalidateDirectMemPtr(sc_dt::uint64 start_range, sc_dt::uint64 end_range);
	/* END: output compare A (OCMPA) edge socket methods */

	/* START: output compare A (OCMPB) edge socket methods */
	tlm::tlm_sync_enum 	OCMPBNbTransportBw(TLMInterruptPayload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t);
	void 				OCMPBInvalidateDirectMemPtr(sc_dt::uint64 start_range, sc_dt::uint64 end_range);
	/* END: output compare A (OCMPB) edge socket methods */

	/* START: input capture A (ICAPA) edge socket methods */
	tlm::tlm_sync_enum	ICAPANbTransportFw(TLMInterruptPayload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t);
	void				ICAPABTransport(TLMInterruptPayload& trans, sc_core::sc_time& t);
	bool				ICAPAGetDirectMemPtr(TLMInterruptPayload& trans, tlm::tlm_dmi& dmi_data);
	unsigned int		ICAPATransportDbg(TLMInterruptPayload& trans);
	/* END: input capture A (ICAPA) edge socket methods */
	
	/* START: input capture A (ICAPA) edge socket methods */
	tlm::tlm_sync_enum	ICAPBNbTransportFw(TLMInterruptPayload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t);
	void				ICAPBBTransport(TLMInterruptPayload& trans, sc_core::sc_time& t);
	bool				ICAPBGetDirectMemPtr(TLMInterruptPayload& trans, tlm::tlm_dmi& dmi_data);
	unsigned int		ICAPBTransportDbg(TLMInterruptPayload& trans);
	/* END: input capture A (ICAPA) edge socket methods */
	
	/* START: input memory socket methods */
	virtual tlm::tlm_sync_enum	nb_transport_fw(tlm::tlm_generic_payload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t);
	virtual void				b_transport(tlm::tlm_generic_payload& trans, sc_core::sc_time& t);
	virtual bool				get_direct_mem_ptr(tlm::tlm_generic_payload& trans, tlm::tlm_dmi& dmi_data);
	virtual unsigned int 		transport_dbg(tlm::tlm_generic_payload& trans);
	/* END: input memory socket methods */

	/* START: TIM registers */
	uint16_t icar;
	uint16_t icbr;
	uint16_t ocar;
	uint16_t ocbr;
	uint16_t cntr;
	uint16_t cr1;
	uint16_t cr2;
	uint16_t sr;

	inline uint16_t ICAR() const { return icar; }
	inline uint16_t ICBR() const { return icbr; }
	inline uint16_t OCAR() const { return ocar; }
	inline uint16_t OCBR() const { return ocbr; }
	inline uint16_t CNTR() const { return cntr; }
	inline uint16_t CR1() const { return cr1; }
	inline uint16_t CR2() const { return cr2; }
	inline uint16_t SR() const { return sr; }

	inline bool EN() const { return (CR1() & ((uint16_t)1 << 15)) != 0; }
	inline bool PWMI() const { return (CR1() & ((uint16_t)1 << 14)) != 0; }
	inline bool FOLVB() const { return (CR1() & ((uint16_t)1 << 11)) != 0; }
	inline bool FOLVA() const { return (CR1() & ((uint16_t)1 << 10)) != 0; }
	inline bool OLVLB() const { return (CR1() & ((uint16_t)1 << 9)) != 0; }
	inline bool OLVLA() const { return (CR1() & ((uint16_t)1 << 8)) != 0; }
	inline bool OCBE() const { return (CR1() & ((uint16_t)1 << 7)) != 0; }
	inline bool OCAE() const { return (CR1() & ((uint16_t)1 << 6)) != 0; }
	inline bool OPM() const { return (CR1() & ((uint16_t)1 << 5)) != 0; }
	inline bool PWM() const { return (CR1() & ((uint16_t)1 << 4)) != 0; }
	inline bool IEDGB() const { return (CR1() & ((uint16_t)1 << 3)) != 0; }
	inline bool IEDGA() const { return (CR1() & ((uint16_t)1 << 2)) != 0; }
	inline bool EXEDG() const { return (CR1() & ((uint16_t)1 << 1)) != 0; }
	inline bool ECKEN() const { return (CR1() & ((uint16_t)1)) != 0; }
	inline bool ICAIE() const { return (CR2() & ((uint16_t)1 << 15)) != 0; }
	inline bool OCAIE() const { return (CR2() & ((uint16_t)1 << 14)) != 0; }
	inline bool TOIE() const { return (CR2() & ((uint16_t)1 << 13)) != 0; }
	inline bool ICBIE() const { return (CR2() & ((uint16_t)1 << 12)) != 0; }
	inline bool OCBIE() const { return (CR2() & ((uint16_t)1 << 11)) != 0; }
	inline bool CC(unsigned int index) { if (index > 7) return false; return (CR2() & ((uint16_t)1 << index)) != 0; }
	inline unsigned int CC() { return CR2() & (uint16_t)0x0ff; }
	inline bool ICFA() const { return (SR() & ((uint16_t)1 << 15)) != 0; }
	inline bool OCFA() const { return (SR() & ((uint16_t)1 << 14)) != 0; }
	inline bool TOF() const { return (SR() & ((uint16_t)1 << 13)) != 0; }
	inline bool ICFB() const { return (SR() & ((uint16_t)1 << 12)) != 0; }
	inline bool OCFB() const { return (SR() & ((uint16_t)1 << 11)) != 0; }

	inline void SetOCAR(uint16_t val) { ocar = val; }
	inline void SetOCBR(uint16_t val) { ocbr = val; }
	inline void SetCNTR(uint16_t val) { cntr = val; }

	tlm::tlm_response_status ReadRegister(uint64_t addr, uint16_t &value);
	tlm::tlm_response_status WriteRegister(uint64_t addr, uint16_t value);

	void ProcessICAPA(bool level);
	void ProcessICAPB(bool level);

	/* END: TIM registers */

	/* the interrupt payload fabric */
	PayloadFabric<TLMInterruptPayload> irqPayloadFabric;

	/* queue of pending memory requests */
	tlm_utils::peq_with_get<tlm::tlm_generic_payload> mem_fifo;
	void MemFifoHandler();
	/* structure for keeping irqs in fifo and queue of empty structures */
	struct irq_fifo_t {
		bool level;
	};
	std::queue<irq_fifo_t *> free_irq_fifo;
	/* queue of pending icapa requests */
	tlm_utils::peq_with_get<irq_fifo_t> icapa_fifo;
	void IcapaFifoHandler();
	/* queue of pending icapb requests */
	tlm_utils::peq_with_get<irq_fifo_t> icapb_fifo;
	void IcapbFifoHandler();

	/* START: interrupt transaction generation methods */
	void SendInputCaptureAInterrupt();
	void SendOutputCompareAInterrupt();
	void SendTimerOverflowInterrupt();
	void SendInputCaptureBInterrupt();
	void SendOutputCompareBInterrupt();
	void SendGlobalTimerInterrupt();
	/* END: interrupt transaction generation methods */

	/* START: timer overflow variables and handler method */
	sc_core::sc_event to_event;
	sc_core::sc_time cntr_update_time;
	void TimerOverflowHandler();
	/* END: timer overflow variables and handler method*/

	/* START: output compare variables and handler method */
	sc_core::sc_event oca_event;
	sc_core::sc_event ocb_event;
	void OutputCompareAHandler();
	void OutputCompareBHandler();
	/* END: output compare variables and handler method */
	
	/* START: module parameters */
	uint64_t base_address;
	unisim::kernel::service::Parameter<uint64_t> param_base_address;
	double pclk2_cycle_time_double; // in PS
	sc_core::sc_time pclk2_cycle_time; // in PS
	unisim::kernel::service::Parameter<double> param_pclk2_cycle_time_double;
	double extclk_cycle_time_double; // in PS
	sc_core::sc_time extclk_cycle_time; // in PS
	unisim::kernel::service::Parameter<double> param_extclk_cycle_time_double;
	bool inverse_icapa;
	unisim::kernel::service::Parameter<bool> param_inverse_icapa;
	bool inverse_icapb;
	unisim::kernel::service::Parameter<bool> param_inverse_icapb;
	/* END: module parameters */

	/* START: logger and logger methods and verbose/trap parameters/methods */
	unisim::kernel::logger::Logger logger;
	bool verbose_all;
	unisim::kernel::service::Parameter<bool> *param_verbose_all;
	bool VerboseAll();
	bool verbose_setup;
	unisim::kernel::service::Parameter<bool> *param_verbose_setup;
	bool VerboseSetup();
	bool verbose_run;
	unisim::kernel::service::Parameter<bool> *param_verbose_run;
	bool VerboseRun();
	bool trap_on_verbose_run;
	unisim::kernel::service::Parameter<bool> *param_trap_on_verbose_run;
	bool verbose_tlm;
	unisim::kernel::service::Parameter<bool> *param_verbose_tlm;
	bool VerboseTLM();
	bool trap_on_verbose_tlm;
	unisim::kernel::service::Parameter<bool> *param_trap_on_verbose_tlm;
	bool trap_on_warning;
	unisim::kernel::service::Parameter<bool> param_trap_on_warning;
	void TrapOnWarning();
	/* END: logger and logger methods and verbose parameters/methods */
};


} // end of namespace str7_timer
} // end of namespace timer
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_TIMER_STR7_TIMER_HH__
