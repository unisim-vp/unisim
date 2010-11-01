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
 
#define SC_INCLUDE_DYNAMIC_PROCESSES
#include "unisim/component/tlm2/chipset/arm926ejs_pxp/pxp.hh"
#include "unisim/kernel/tlm2/tlm.hh"
#include <inttypes.h>
#include <assert.h>

namespace unisim {
namespace component {
namespace tlm2 {
namespace chipset {
namespace arm926ejs_pxp {

using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugError;

PXP ::
PXP(const sc_module_name &name, Object *parent)
	: unisim::kernel::service::Object(name, parent)
	, sc_module(name)
	, cpu_target_socket("cpu-target-socket")
	, ssmc0_init_socket("ssmc0-init-socket")
	, mpmc0_init_socket("mpmc0-init-socket")
	, eth_init_socket("eth-init-socket")
	, sc_init_socket("sc-init-socket")
	, wd_init_socket("wd-init-socket")
	, eth("ethernet", this)
	, sc("system-controller", this)
	, uart0("uart0", this)
	, dt1("timer1-2", this)
	, wd("watchdog", this)
	, logger(*this)
{
	eth["base-addr"]       = 0x10010000UL;
	sc["base-addr"]        = 0x101e0000UL;
	uart0["base-addr"]     = 0x101f1000UL;
	dt1["base-addr"]       = 0x101e2000UL;
	cpu_target_socket.register_nb_transport_fw(this,
			&PXP::cpu_target_nb_transport_fw);
	cpu_target_socket.register_b_transport(this,
			&PXP::cpu_target_b_transport);
	cpu_target_socket.register_get_direct_mem_ptr(this,
			&PXP::cpu_target_get_direct_mem_ptr);
	cpu_target_socket.register_transport_dbg(this,
			&PXP::cpu_target_transport_dbg);

	ssmc0_init_socket.register_nb_transport_bw(this,
			&PXP::ssmc0_init_nb_transport_bw);
	ssmc0_init_socket.register_invalidate_direct_mem_ptr(this,
			&PXP::ssmc0_init_invalidate_direct_mem_ptr);

	mpmc0_init_socket.register_nb_transport_bw(this,
			&PXP::mpmc0_init_nb_transport_bw);
	mpmc0_init_socket.register_invalidate_direct_mem_ptr(this,
			&PXP::mpmc0_init_invalidate_direct_mem_ptr);

	eth_init_socket.register_nb_transport_bw(this,
			&PXP::eth_init_nb_transport_bw);
	eth_init_socket.register_invalidate_direct_mem_ptr(this,
			&PXP::eth_init_invalidate_direct_mem_ptr);

	sc_init_socket.register_nb_transport_bw(this,
			&PXP::sc_init_nb_transport_bw);
	sc_init_socket.register_invalidate_direct_mem_ptr(this,
			&PXP::sc_init_invalidate_direct_mem_ptr);

	uart0_init_socket.register_nb_transport_bw(this,
			&PXP::uart0_init_nb_transport_bw);
	uart0_init_socket.register_invalidate_direct_mem_ptr(this,
			&PXP::uart0_init_invalidate_direct_mem_ptr);

	wd_init_socket.register_nb_transport_bw(this,
			&PXP::wd_init_nb_transport_bw);
	wd_init_socket.register_invalidate_direct_mem_ptr(this,
			&PXP::wd_init_invalidate_direct_mem_ptr);

	dt1_init_socket.register_nb_transport_bw(this,
			&PXP::dt1_init_nb_transport_bw);
	dt1_init_socket.register_invalidate_direct_mem_ptr(this,
			&PXP::dt1_init_invalidate_direct_mem_ptr);

	dt1.timclk_in_port(sc_to_dt1_signal[0]);
	sc.refclk_out_port(sc_to_dt1_signal[0]);
	dt1.timclken1_in_port(sc_to_dt1_signal[1]);
	sc.timclken0_out_port(sc_to_dt1_signal[1]);
	dt1.timclken2_in_port(sc_to_dt1_signal[2]);
	sc.timclken1_out_port(sc_to_dt1_signal[2]);
	timint0_in_port(timint0_signal);
	dt1.timint1_out_port(timint0_signal);
	timint1_in_port(timint1_signal);
	dt1.timint2_out_port(timint1_signal);
	timintc01_in_port(timintc01_signal);
	dt1.timintc_out_port(timintc01_signal);

	eth_init_socket(eth.bus_target_socket);
	sc_init_socket(sc.bus_target_socket);
	uart0_init_socket(uart0.bus_target_socket);
	wd_init_socket(wd.bus_target_socket);
	dt1_init_socket(dt1.bus_target_socket);
}

PXP ::
~PXP()
{
}

bool 
PXP ::
Setup()
{
	return true;
}

/**************************************************************************/
/* Virtual methods for the target socket for the cpu connection     START */
/**************************************************************************/

tlm::tlm_sync_enum
PXP ::
cpu_target_nb_transport_fw(transaction_type &trans,
		phase_type &phase,
		sc_core::sc_time &time)
{
	assert("TODO" == 0);
	return tlm::TLM_COMPLETED;
}

void 
PXP ::
cpu_target_b_transport(transaction_type &trans, 
		sc_core::sc_time &delay)
{
	bool handled = false;
	if ( trans.get_address() < 0x08000000UL )
	{
		mpmc0_init_socket->b_transport(trans, delay);
		handled = true;
	}
	else if ( trans.get_address() >= 0x10010000UL &&
			trans.get_address()   <  0x10020000UL )
	{
		eth_init_socket->b_transport(trans, delay);
		handled = true;
	}
	else if ( trans.get_address() >= 0x101e0000UL &&
			trans.get_address()   <  0x101e1000UL )
	{
		sc_init_socket->b_transport(trans, delay);
		handled = true;
	}
	else if ( trans.get_address() >= 0x101e1000UL && 
			trans.get_address()   <  0x101e2000UL )
	{
		wd_init_socket->b_transport(trans, delay);
		handled = true;
	}
	else if ( trans.get_address() >= 0x101e2000UL &&
			trans.get_address()   <  0x101e3000UL )
	{
		dt1_init_socket->b_transport(trans, delay);
		handled = true;
	}
	else if ( trans.get_address() >= 0x101f1000UL &&
			trans.get_address()   <  0x101f2000UL )
	{
		uart0_init_socket->b_transport(trans, delay);
		handled = true;
	}
	else if ( trans.get_address() >= 0x34000000UL &&
			trans.get_address()   <  0x38000000UL )
	{
		ssmc1_init_socket->b_transport(trans, delay);
		handled = true;
	}
	if ( !handled )
	{
		logger << DebugError
			<< "Unhandled address 0x" 
			<< std::hex << trans.get_address() << std::dec
			<< EndDebugError;
		assert("TODO" == 0);
	}
}

bool 
PXP ::
cpu_target_get_direct_mem_ptr(transaction_type &trans, 
		tlm::tlm_dmi &dmi_data)
{
	assert("TODO" == 0);
	return false;
}

unsigned int 
PXP ::
cpu_target_transport_dbg(transaction_type &trans)
{
	if ( trans.get_address() < 0x08000000UL )
		return mpmc0_init_socket->transport_dbg(trans);
	else if ( trans.get_address()   >= 0x10010000UL &&
			trans.get_address()     <  0x10020000UL )
		return eth_init_socket->transport_dbg(trans);
	else if ( trans.get_address()   >= 0x101e0000UL &&
			trans.get_address()     <  0x101e1000UL )
		return sc_init_socket->transport_dbg(trans);
	else if ( trans.get_address()   >= 0x101e1000UL && 
			trans.get_address()     <  0x101e2000UL )
		return wd_init_socket->transport_dbg(trans);
	else if ( trans.get_address()   >= 0x101e2000UL &&
			trans.get_address()     <  0x101e3000UL )
		return dt1_init_socket->transport_dbg(trans);
	else if ( trans.get_address()   >= 0x101f1000UL &&
			trans.get_address()     <  0x101f2000UL )
		return uart0_init_socket->transport_dbg(trans);
	assert("TODO" == 0);
}

/**************************************************************************/
/* Virtual methods for the target socket for the cpu connection       END */
/**************************************************************************/

/**************************************************************************/
/* Virtual methods for the initiator socket for                     START */
/*   the SSMC Chip Select 0                                               */
/**************************************************************************/

tlm::tlm_sync_enum
PXP ::
ssmc0_init_nb_transport_bw(transaction_type &trans,
		phase_type &phase,
		sc_core::sc_time &time)
{
	assert("TODO" == 0);
	return tlm::TLM_COMPLETED;
}

void 
PXP ::
ssmc0_init_invalidate_direct_mem_ptr(sc_dt::uint64,
		sc_dt::uint64)
{
	assert("TODO" == 0);
}

/**************************************************************************/
/* Virtual methods for the initiator socket for                           */
/*   the SSMC Chip Select 0                                           END */
/**************************************************************************/

/**************************************************************************/
/* Virtual methods for the initiator socket for                     START */
/*   the MPMC Chip Select 0                                               */
/**************************************************************************/

tlm::tlm_sync_enum
PXP ::
mpmc0_init_nb_transport_bw(transaction_type &trans,
		phase_type &phase,
		sc_core::sc_time &time)
{
	assert("TODO" == 0);
	return tlm::TLM_COMPLETED;
}

void 
PXP ::
mpmc0_init_invalidate_direct_mem_ptr(sc_dt::uint64,
		sc_dt::uint64)
{
	assert("TODO" == 0);
}

/**************************************************************************/
/* Virtual methods for the initiator socket for                           */
/*   the MPMC Chip Select 0                                           END */
/**************************************************************************/

/**************************************************************************/
/* Virtual methods for the initiator socket for                     START */
/*   the Ethernet Controller                                              */
/**************************************************************************/

tlm::tlm_sync_enum
PXP ::
eth_init_nb_transport_bw(transaction_type &trans,
		phase_type &phase,
		sc_core::sc_time &time)
{
	assert("TODO" == 0);
	return tlm::TLM_COMPLETED;
}

void 
PXP ::
eth_init_invalidate_direct_mem_ptr(sc_dt::uint64,
		sc_dt::uint64)
{
	assert("TODO" == 0);
}

/**************************************************************************/
/* Virtual methods for the initiator socket for                           */
/*   the Ethernet Controller                                          END */
/**************************************************************************/

/**************************************************************************/
/* Virtual methods for the initiator socket for                     START */
/*   the System Controller                                                */
/**************************************************************************/

tlm::tlm_sync_enum
PXP ::
sc_init_nb_transport_bw(transaction_type &trans,
		phase_type &phase,
		sc_core::sc_time &time)
{
	assert("TODO" == 0);
	return tlm::TLM_COMPLETED;
}

void 
PXP ::
sc_init_invalidate_direct_mem_ptr(sc_dt::uint64,
		sc_dt::uint64)
{
	assert("TODO" == 0);
}

/**************************************************************************/
/* Virtual methods for the initiator socket for                           */
/*   the System Controller                                            END */
/**************************************************************************/

/**************************************************************************/
/* Virtual methods for the initiator socket for                     START */
/*   the UART 0                                                           */
/**************************************************************************/

tlm::tlm_sync_enum
PXP ::
uart0_init_nb_transport_bw(transaction_type &trans,
		phase_type &phase,
		sc_core::sc_time &time)
{
	assert("TODO" == 0);
	return tlm::TLM_COMPLETED;
}

void 
PXP ::
uart0_init_invalidate_direct_mem_ptr(sc_dt::uint64,
		sc_dt::uint64)
{
	assert("TODO" == 0);
}

/**************************************************************************/
/* Virtual methods for the initiator socket for                           */
/*   the UART 0                                                       END */
/**************************************************************************/

/**************************************************************************/
/* Virtual methods for the initiator socket for                     START */
/*   the Timer 1-2                                                        */
/**************************************************************************/

tlm::tlm_sync_enum
PXP ::
dt1_init_nb_transport_bw(transaction_type &trans,
		phase_type &phase,
		sc_core::sc_time &time)
{
	assert("TODO" == 0);
	return tlm::TLM_COMPLETED;
}

void 
PXP ::
dt1_init_invalidate_direct_mem_ptr(sc_dt::uint64,
		sc_dt::uint64)
{
	assert("TODO" == 0);
}

/**************************************************************************/
/* Virtual methods for the initiator socket for                           */
/*   the Timer 1-2                                                    END */
/**************************************************************************/

/**************************************************************************/
/* Virtual methods for the initiator socket for                     START */
/*   the Watchdog                                                         */
/**************************************************************************/

tlm::tlm_sync_enum
PXP ::
wd_init_nb_transport_bw(transaction_type &trans,
		phase_type &phase,
		sc_core::sc_time &time)
{
	assert("TODO" == 0);
	return tlm::TLM_COMPLETED;
}

void 
PXP ::
wd_init_invalidate_direct_mem_ptr(sc_dt::uint64,
		sc_dt::uint64)
{
	assert("TODO" == 0);
}

/**************************************************************************/
/* Virtual methods for the initiator socket for                           */
/*   the Watchdog                                                     END */
/**************************************************************************/

} // end of namespace arm926ejs_pxp
} // end of namespace chipset
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim


