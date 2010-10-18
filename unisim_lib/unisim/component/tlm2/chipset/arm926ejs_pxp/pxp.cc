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
	, sc_init_socket("sc-init-socket")
	, wd_init_socket("wd-init-socket")
	, sc("system-controller", this)
	, wd("watchdog", this)
	, logger(*this)
{
	sc["base-addr"] = 0x101e0000UL;
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

	wd_init_socket.register_nb_transport_bw(this,
			&PXP::wd_init_nb_transport_bw);
	wd_init_socket.register_invalidate_direct_mem_ptr(this,
			&PXP::wd_init_invalidate_direct_mem_ptr);

	sc_init_socket(sc.bus_target_socket);
	wd_init_socket(wd.bus_target_socket);
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
	else if ( trans.get_address() >= 0x101e0000UL &&
			trans.get_address() < 0x101e1000UL )
	{
		sc_init_socket->b_transport(trans, delay);
		handled = true;
	}
	else if ( trans.get_address() >= 0x101e1000UL && 
			trans.get_address() < 0x101e2000UL )
	{
		wd_init_socket->b_transport(trans, delay);
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
	else if ( trans.get_address() >= 0x101e0000UL &&
			trans.get_address() < 0x101e1000UL )
		return sc_init_socket->transport_dbg(trans);
	else if ( trans.get_address() >= 0x101e1000UL && 
			trans.get_address() < 0x101e2000UL )
		return wd_init_socket->transport_dbg(trans);
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


