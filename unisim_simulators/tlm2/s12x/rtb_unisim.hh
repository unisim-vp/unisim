/*
 *  Copyright (c) 2008,
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
 * Authors: Gilles	Mouchard <gilles.mouchard@cea.fr>
 * 			Reda	Nouacer  <reda.nouacer@cea.fr>
 */



#include <systemc.h>
#include <iostream>
#include <tlm.h>
#include <tlm_utils/tlm_quantumkeeper.h>
#include <tlm_utils/peq_with_get.h>
#include <inttypes.h>
#include <queue>

#include <unisim/component/tlm2/processor/hcs12x/tlm_types.hh>

using namespace std;
using namespace tlm;
using namespace tlm_utils;

using unisim::component::tlm2::processor::hcs12x::PWM_Payload;
using unisim::component::tlm2::processor::hcs12x::ATD_Payload;
using unisim::component::tlm2::processor::hcs12x::UNISIM_PWM_ProtocolTypes;
using unisim::component::tlm2::processor::hcs12x::UNISIM_ATD_ProtocolTypes;

using unisim::kernel::tlm2::PayloadFabric;

#define ATD1_SIZE	16
#define ATD0_SIZE	8
#define PWM_SIZE	8


static const unsigned int UNISIM2RTB_BUS_WIDTH = 32; // in bits (unused in this example)
static const unsigned int RTB2UNISIM_BUS_WIDTH = 32; // in bits (unused in this example)


// A module that should implement communication with RT-Builder
class RTBStub :
	public sc_module,
	virtual public tlm_fw_transport_if<UNISIM_PWM_ProtocolTypes<PWM_SIZE> >,
	virtual public tlm_bw_transport_if<UNISIM_ATD_ProtocolTypes<ATD0_SIZE> >,
	virtual public tlm_bw_transport_if<UNISIM_ATD_ProtocolTypes<ATD1_SIZE> >
{
public:
	tlm_initiator_socket<RTB2UNISIM_BUS_WIDTH, UNISIM_ATD_ProtocolTypes<ATD1_SIZE> > atd1_master_sock;
	tlm_initiator_socket<RTB2UNISIM_BUS_WIDTH, UNISIM_ATD_ProtocolTypes<ATD0_SIZE> > atd0_master_sock;

	tlm_target_socket<UNISIM2RTB_BUS_WIDTH, UNISIM_PWM_ProtocolTypes<PWM_SIZE> > slave_sock;
	tlm_quantumkeeper quantumkeeper;
	peq_with_get<PWM_Payload<PWM_SIZE> > input_payload_queue;
	PayloadFabric<ATD_Payload<ATD1_SIZE> > atd1_payload_fabric;
	PayloadFabric<ATD_Payload<ATD0_SIZE> > atd0_payload_fabric;

	clock_t	bus_cycle_time;
	sc_time		cycle_time;

	RTBStub(const sc_module_name& name, clock_t	bus_cycle_time);

	// Slave methods
	virtual bool get_direct_mem_ptr( PWM_Payload<PWM_SIZE>& payload, tlm_dmi&  dmi_data);
	virtual unsigned int transport_dbg( PWM_Payload<PWM_SIZE>& payload);

	virtual tlm_sync_enum nb_transport_fw( PWM_Payload<PWM_SIZE>& payload, tlm_phase& phase, sc_core::sc_time& t);
	virtual void b_transport( PWM_Payload<PWM_SIZE>& payload, sc_core::sc_time& t);
	// Master methods
	virtual tlm_sync_enum nb_transport_bw( ATD_Payload<ATD1_SIZE>& payload, tlm_phase& phase, sc_core::sc_time& t);
	virtual tlm_sync_enum nb_transport_bw( ATD_Payload<ATD0_SIZE>& payload, tlm_phase& phase, sc_core::sc_time& t);
	virtual void invalidate_direct_mem_ptr( sc_dt::uint64 start_range, sc_dt::uint64 end_range);

	// Implementation
	void Input(bool pwmValue[PWM_SIZE]);
	void Output_ATD1(double anValue[ATD1_SIZE]);
	void Output_ATD0(double anValue[ATD0_SIZE]);
	void Process();

};