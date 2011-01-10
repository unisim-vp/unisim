/*
 *  Copyright (c) 2010,
 *  Commissariat a l'Energie Atomique (CEA)
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without 
 *  modification, are permitted provided that the following conditions are met:
 *
 *   - Redistributions of source code must retain the above copyright notice, 
 *     this list of conditions and the following disclaimer.
 *
 *   - Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 *   - Neither the name of CEA nor the names of its contributors may be used to
 *     endorse or promote products derived from this software without specific 
 *     prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED.
 *  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY
 *  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF 
 *  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 */
 
#ifndef __UNISIM_COMPONENT_TLM2_CHIPSET_ARM926EJS_PXP_PXP_HH__
#define __UNISIM_COMPONENT_TLM2_CHIPSET_ARM926EJS_PXP_PXP_HH__

#include <systemc.h>
#include <tlm.h>
#include <tlm_utils/simple_initiator_socket.h>
#include <tlm_utils/passthrough_target_socket.h>
#include "unisim/kernel/service/service.hh"
#include "unisim/kernel/logger/logger.hh"
#include "unisim/kernel/tlm2/tlm.hh"
#include <inttypes.h>
#include "unisim/component/tlm2/chipset/arm926ejs_pxp/system_controller/sc.hh"
#include "unisim/component/tlm2/chipset/arm926ejs_pxp/uart/uart.hh"
#include "unisim/component/tlm2/chipset/arm926ejs_pxp/ethernet/smsc_lan91c111.hh"
#include "unisim/component/tlm2/chipset/arm926ejs_pxp/dual_timer/dt.hh"
#include "unisim/component/tlm2/chipset/arm926ejs_pxp/vic/vic.hh"
#include "unisim/component/tlm2/chipset/arm926ejs_pxp/vic/vic_stubs.hh"
#include "unisim/component/tlm2/chipset/arm926ejs_pxp/sic/sic.hh"
#include "unisim/component/tlm2/chipset/arm926ejs_pxp/watchdog/watchdog.hh"
#include "unisim/component/tlm2/chipset/arm926ejs_pxp/util/time_signal_splitter.hh"

namespace unisim {
namespace component {
namespace tlm2 {
namespace chipset {
namespace arm926ejs_pxp {

class PXP
	: public unisim::kernel::service::Object
	, public sc_module
{
public:
	typedef tlm::tlm_base_protocol_types::tlm_payload_type transaction_type;
	typedef tlm::tlm_base_protocol_types::tlm_phase_type   phase_type;
	typedef tlm::tlm_sync_enum                             sync_enum_type;

	/** Target socket for the cpu connection */
	tlm_utils::passthrough_target_socket<PXP, 32>
		cpu_target_socket;
	/** Initiator socket for the SSMC Chip Select 0, NOR flash 2 */
	tlm_utils::simple_initiator_socket<PXP, 32>
		ssmc0_init_socket;
	/** Initiator socket for the SSMC Chip Select 1, NOR flash 1 */
	tlm_utils::simple_initiator_socket<PXP, 32>
		ssmc1_init_socket;
	/** Initiator socket for the MPMC Chip Select 0, SDRAM 128MB */
	tlm_utils::simple_initiator_socket<PXP, 32>
		mpmc0_init_socket;
	/** Initiator socket for the Ethernet Controller */
	tlm_utils::simple_initiator_socket<PXP, 32>
		eth_init_socket;
	/** Initiator socket for the System Controller */
	tlm_utils::simple_initiator_socket<PXP, 32>
		sc_init_socket;
	/** Initiator socket for the UART0 */
	tlm_utils::simple_initiator_socket<PXP, 32>
		uart0_init_socket;
	/** Initiator socket for the Timer 1-2 */
	tlm_utils::simple_initiator_socket<PXP, 32>
		dt1_init_socket;
	/** Initiator socket for the Timer 3-4 */
	tlm_utils::simple_initiator_socket<PXP, 32>
		dt2_init_socket;
	/** Initiator socket for the Watchdog */
	tlm_utils::simple_initiator_socket<PXP, 32>
		wd_init_socket;
	/** Initiator socket for the PIC (VIC, Vectored Interrupt Controller) */
	tlm_utils::simple_initiator_socket<PXP, 32>
		pic_init_socket;
	/** Initiator socket for the SIC (Secondry Interrupt Controller) */
	tlm_utils::simple_initiator_socket<PXP, 32>
		sic_init_socket;

	/** The Ethernet Controller */
	unisim::component::tlm2::chipset::arm926ejs_pxp::ethernet::SMSC_LAN91C111 eth;
	/** The System Controller */
	unisim::component::tlm2::chipset::arm926ejs_pxp::system_controller::SystemController sc;
	/** The UART 0 */
	unisim::component::tlm2::chipset::arm926ejs_pxp::uart::UART uart0;
	/** Dual Timer for Timer 1 and 2 */
	unisim::component::tlm2::chipset::arm926ejs_pxp::dual_timer::DualTimer dt1;
	/** Dual Timer for Timer 3 and 4 */
	unisim::component::tlm2::chipset::arm926ejs_pxp::dual_timer::DualTimer dt2;
	/** Watchdog module */
	unisim::component::tlm2::chipset::arm926ejs_pxp::watchdog::Watchdog wd;
	/** VIC (Vectored Interrupt Controller) module */
	unisim::component::tlm2::chipset::arm926ejs_pxp::vic::VIC pic;
	/** SIC (Secondary Interrupt Controller) module */
	unisim::component::tlm2::chipset::arm926ejs_pxp::sic::SIC sic;

	/** Tims signal splitter */
	unisim::component::tlm2::chipset::arm926ejs_pxp::util::TSS tss;
	sc_signal<uint64_t> sc_to_dt_signal;
	sc_signal<uint64_t> sc_to_dt1_signal[3];
	sc_signal<uint64_t> sc_to_dt2_signal[3];

	/* PIC stubs  START */
	vic::VICIntSourceStub *intsource_stub[21];
	sc_signal<bool> intsource_signal[32];
	vic::VICIntSourceStub nvicfiqin_stub;
	sc_signal<bool> nvicfiqin_signal;
	vic::VICIntSourceStub nvicirqin_stub;
	sc_signal<bool> nvicirqin_signal;
	vic::VICAddrSourceStub vicvectaddrin_stub;
	sc_signal<uint32_t> vicvectaddrin_signal;
	
	sc_signal<bool> nvicfiq_signal;
	sc_signal<bool> nvicirq_signal;
	vic::VICAddrTargetStub vicvectaddrout_stub;
	sc_signal<uint32_t> vicvectaddrout_signal;
	/* PIC stubs    END */

	/* SIC stubs  START */
	vic::VICIntSourceStub *sic_intsource_stub[31];
	sc_signal<bool> sic_intsource_signal[31];
	vic::VICIntSourceStub *sic_ptintsource_stub[11];
	sc_signal<bool> sic_ptintsource_signal[11];

	vic::VICIntTargetStub *sic_inttarget_stub[11];
	// sc_signal<bool> sic_inttarget_stub[11];
	/* SIC stubs    END */

	sc_in<bool> timint1_in_port;
	sc_in<bool> timint2_in_port;
	sc_in<bool> timintc12_in_port;
	sc_signal<bool> timint1_signal, timint2_signal, timintc12_signal;
	sc_in<bool> timint3_in_port;
	sc_in<bool> timint4_in_port;
	sc_in<bool> timintc34_in_port;
	sc_signal<bool> timint3_signal, timint4_signal, timintc34_signal;


	SC_HAS_PROCESS(PXP);
	PXP(const sc_module_name &name, Object *parent = 0);
	~PXP();

	virtual bool Setup();

private:

	/** Blocking access to the system registers.
	 *
	 * @param trans the transaction
	 * @param delay current delta time
	 */
	void AccessSystemRegisters(transaction_type &trans,
			sc_core::sc_time &delay);
	/** Access to the 24MHz counter register.
	 *
	 * @param trans the transaction
	 * @param delay current delta time
	 */
	void Access24MHzCounter(transaction_type & trans,
			sc_core::sc_time &delay);

	/**************************************************************************/
	/* Virtual methods for the target socket for the cpu connection     START */
	/**************************************************************************/

	sync_enum_type cpu_target_nb_transport_fw(transaction_type &trans,
			phase_type &phase,
			sc_core::sc_time &time);
	void cpu_target_b_transport(transaction_type &trans, 
			sc_core::sc_time &delay);
	bool cpu_target_get_direct_mem_ptr(transaction_type &trans, 
			tlm::tlm_dmi &dmi_data);
	unsigned int cpu_target_transport_dbg(transaction_type &trans);

	/**************************************************************************/
	/* Virtual methods for the target socket for the cpu connection       END */
	/**************************************************************************/
	
	/**************************************************************************/
	/* Virtual methods for the initiator socket for                     START */
	/*   the SSMC Chip Select 0                                               */
	/**************************************************************************/

	sync_enum_type ssmc0_init_nb_transport_bw(transaction_type &trans,
			phase_type &phase,
			sc_core::sc_time &time);
	void ssmc0_init_invalidate_direct_mem_ptr(sc_dt::uint64,
			sc_dt::uint64);

	/**************************************************************************/
	/* Virtual methods for the initiator socket for                           */
	/*   the SSMC Chip Select 0                                           END */
	/**************************************************************************/

	/**************************************************************************/
	/* Virtual methods for the initiator socket for                     START */
	/*   the MPMC Chip Select 0                                               */
	/**************************************************************************/

	sync_enum_type mpmc0_init_nb_transport_bw(transaction_type &trans,
			phase_type &phase,
			sc_core::sc_time &time);
	void mpmc0_init_invalidate_direct_mem_ptr(sc_dt::uint64,
			sc_dt::uint64);

	/**************************************************************************/
	/* Virtual methods for the initiator socket for                           */
	/*   the MPMC Chip Select 0                                           END */
	/**************************************************************************/

	/**************************************************************************/
	/* Virtual methods for the initiator socket for                     START */
	/*   the Ethernet Controller                                              */
	/**************************************************************************/

	sync_enum_type eth_init_nb_transport_bw(transaction_type &trans,
			phase_type &phase,
			sc_core::sc_time &time);
	void eth_init_invalidate_direct_mem_ptr(sc_dt::uint64,
			sc_dt::uint64);

	/**************************************************************************/
	/* Virtual methods for the initiator socket for                           */
	/*   the Ethenet Controller                                           END */
	/**************************************************************************/

	/**************************************************************************/
	/* Virtual methods for the initiator socket for                     START */
	/*   the System Controller                                                */
	/**************************************************************************/

	sync_enum_type sc_init_nb_transport_bw(transaction_type &trans,
			phase_type &phase,
			sc_core::sc_time &time);
	void sc_init_invalidate_direct_mem_ptr(sc_dt::uint64,
			sc_dt::uint64);

	/**************************************************************************/
	/* Virtual methods for the initiator socket for                           */
	/*   the System Controller                                            END */
	/**************************************************************************/

	/**************************************************************************/
	/* Virtual methods for the initiator socket for                     START */
	/*   the UART 0                                                           */
	/**************************************************************************/

	sync_enum_type uart0_init_nb_transport_bw(transaction_type &trans,
			phase_type &phase,
			sc_core::sc_time &time);
	void uart0_init_invalidate_direct_mem_ptr(sc_dt::uint64,
			sc_dt::uint64);

	/**************************************************************************/
	/* Virtual methods for the initiator socket for                           */
	/*   the UART 0                                                       END */
	/**************************************************************************/

	/**************************************************************************/
	/* Virtual methods for the initiator socket for                     START */
	/*   the Dual Timers                                                      */
	/**************************************************************************/

	sync_enum_type dt1_init_nb_transport_bw(transaction_type &trans,
			phase_type &phase,
			sc_core::sc_time &time);
	void dt1_init_invalidate_direct_mem_ptr(sc_dt::uint64,
			sc_dt::uint64);
	sync_enum_type dt2_init_nb_transport_bw(transaction_type &trans,
			phase_type &phase,
			sc_core::sc_time &time);
	void dt2_init_invalidate_direct_mem_ptr(sc_dt::uint64,
			sc_dt::uint64);

	/**************************************************************************/
	/* Virtual methods for the initiator socket for                           */
	/*   the Dual Timers                                                  END */
	/**************************************************************************/

	/**************************************************************************/
	/* Virtual methods for the initiator socket for                     START */
	/*   the Watchdog                                                         */
	/**************************************************************************/

	sync_enum_type wd_init_nb_transport_bw(transaction_type &trans,
			phase_type &phase,
			sc_core::sc_time &time);
	void wd_init_invalidate_direct_mem_ptr(sc_dt::uint64,
			sc_dt::uint64);

	/**************************************************************************/
	/* Virtual methods for the initiator socket for                           */
	/*   the Watchdog                                                     END */
	/**************************************************************************/

	/**************************************************************************/
	/* Virtual methods for the initiator socket for                     START */
	/*   the PIC                                                              */
	/**************************************************************************/

	sync_enum_type pic_init_nb_transport_bw(transaction_type &trans,
			phase_type &phase,
			sc_core::sc_time &time);
	void pic_init_invalidate_direct_mem_ptr(sc_dt::uint64,
			sc_dt::uint64);

	/**************************************************************************/
	/* Virtual methods for the initiator socket for                           */
	/*   the PIC                                                          END */
	/**************************************************************************/

	/**************************************************************************/
	/* Virtual methods for the initiator socket for                     START */
	/*   the SIC                                                              */
	/**************************************************************************/

	sync_enum_type sic_init_nb_transport_bw(transaction_type &trans,
			phase_type &phase,
			sc_core::sc_time &time);
	void sic_init_invalidate_direct_mem_ptr(sc_dt::uint64,
			sc_dt::uint64);

	/**************************************************************************/
	/* Virtual methods for the initiator socket for                           */
	/*   the SIC                                                          END */
	/**************************************************************************/
	/** Interface to the UNISIM logger */
	unisim::kernel::logger::Logger logger;
};

} // end of namespace arm926ejs_pxp
} // end of namespace chipset
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_CHIPSET_ARM926EJS_PXP_PXP_HH__

