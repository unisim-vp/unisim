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

#include <unisim/component/cxx/processor/hcs12x/config.hh>

#include <unisim/component/tlm2/processor/hcs12x/tlm_types.hh>

using namespace std;
using namespace tlm;
using namespace tlm_utils;

using unisim::component::cxx::processor::hcs12x::CONFIG;

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

	RTBStub(const sc_module_name& name, clock_t	bus_cycle_time) :
		sc_module(name),
		atd1_master_sock("atd1_master_sock"),
		atd0_master_sock("atd0_master_sock"),
		slave_sock("slave_sock"),
		input_payload_queue("input_payload_queue"),
		cycle_time(bus_cycle_time, SC_PS)
	{
		atd1_master_sock(*this);
		atd0_master_sock(*this);
		slave_sock(*this);

		SC_HAS_PROCESS(RTBStub);

		SC_THREAD(Process);
	}

	// Slave methods
	virtual bool get_direct_mem_ptr( PWM_Payload<PWM_SIZE>& payload, tlm_dmi&  dmi_data)
	{
		// Leave this empty as it is designed for memory mapped buses
		return false;
	}

	virtual unsigned int transport_dbg( PWM_Payload<PWM_SIZE>& payload)
	{
		// Leave this empty as it is designed for memory mapped buses
		return 0;
	}

	virtual tlm_sync_enum nb_transport_fw( PWM_Payload<PWM_SIZE>& payload, tlm_phase& phase, sc_core::sc_time& t)
	{
		if(phase == BEGIN_REQ)
		{
			phase = END_REQ; // update the phase
			input_payload_queue.notify(payload, t); // queue the payload and the associative time
			return TLM_UPDATED;
		}

		// we received an unexpected phase, so we return TLM_ACCEPTED
		return TLM_ACCEPTED;
	}

	virtual void b_transport( PWM_Payload<PWM_SIZE>& payload, sc_core::sc_time& t)
	{
		input_payload_queue.notify(payload, t);
	}

	// Master methods
	virtual tlm_sync_enum nb_transport_bw( ATD_Payload<ATD1_SIZE>& payload, tlm_phase& phase, sc_core::sc_time& t)
	{
		if(phase == BEGIN_RESP)
		{
			payload.release();
			return TLM_COMPLETED;
		}
		return TLM_ACCEPTED;
	}

	virtual tlm_sync_enum nb_transport_bw( ATD_Payload<ATD0_SIZE>& payload, tlm_phase& phase, sc_core::sc_time& t)
	{
		if(phase == BEGIN_RESP)
		{
			payload.release();
			return TLM_COMPLETED;
		}
		return TLM_ACCEPTED;
	}

	virtual void invalidate_direct_mem_ptr( sc_dt::uint64 start_range, sc_dt::uint64 end_range)
	{
	}

	// Implementation
	void Input(bool pwmValue[PWM_SIZE])
	{
		PWM_Payload<PWM_SIZE> *last_payload = NULL;
		PWM_Payload<PWM_SIZE> *payload = NULL;

		do
		{
			last_payload = payload;
			payload = input_payload_queue.get_next_transaction();

//			if (CONFIG::DEBUG_ENABLE && payload) {
//				cout << sc_time_stamp() << ":" << name() << "::PWM:: Receive " << payload->serialize() << endl;
//			}
		} while(payload);

		payload = last_payload;

		if (CONFIG::DEBUG_ENABLE) {

			cout << sc_time_stamp() << ":" << name() << "::PWM:: Last Receive " << payload->serialize() << endl;
		}

		for (int i=0; i<PWM_SIZE; i++) {
			pwmValue[i] = payload->pwmChannel[i];
		}

	}

	void Output_ATD1(double anValue[ATD1_SIZE])
	{

		tlm_phase phase = BEGIN_REQ;
		ATD_Payload<ATD1_SIZE> *payload = atd1_payload_fabric.allocate();

		for (int i=0; i<ATD1_SIZE; i++) {
			payload->anPort[i] = anValue[i];
		}

		if (CONFIG::DEBUG_ENABLE) {
			cout << sc_time_stamp() << ":" << name() << "::ATD1::send " << payload->serialize() << endl;
		}

		sc_time local_time = quantumkeeper.get_local_time();

		tlm_sync_enum ret = atd1_master_sock->nb_transport_fw(*payload, phase, local_time);

		switch(ret)
		{
			case TLM_ACCEPTED:
				// neither payload, nor phase and local_time have been modified by the callee
				quantumkeeper.sync(); // synchronize to leave control to the callee
				break;
			case TLM_UPDATED:
				// the callee may have modified 'payload', 'phase' and 'local_time'
				quantumkeeper.set(local_time); // increase the time
				if(quantumkeeper.need_sync()) quantumkeeper.sync(); // synchronize if needed

				break;
			case TLM_COMPLETED:
				// the callee may have modified 'payload', and 'local_time' ('phase' can be ignored)
				quantumkeeper.set(local_time); // increase the time
				if(quantumkeeper.need_sync()) quantumkeeper.sync(); // synchronize if needed
				break;
		}
	}

	void Output_ATD0(double anValue[ATD0_SIZE])
	{
		tlm_phase phase = BEGIN_REQ;
		ATD_Payload<ATD0_SIZE> *payload = atd0_payload_fabric.allocate();

		for (int i=0; i<ATD0_SIZE; i++) {
			payload->anPort[i] = anValue[i];
		}

		if (CONFIG::DEBUG_ENABLE) {
			cout << sc_time_stamp() << ":" << name() << "::ATD0::send " << payload->serialize() << endl;
		}

		sc_time local_time = quantumkeeper.get_local_time();

		tlm_sync_enum ret = atd0_master_sock->nb_transport_fw(*payload, phase, local_time);

		switch(ret)
		{
			case TLM_ACCEPTED:
				// neither payload, nor phase and local_time have been modified by the callee
				quantumkeeper.sync(); // synchronize to leave control to the callee
				break;
			case TLM_UPDATED:
				// the callee may have modified 'payload', 'phase' and 'local_time'
				quantumkeeper.set(local_time); // increase the time
				if(quantumkeeper.need_sync()) quantumkeeper.sync(); // synchronize if needed

				break;
			case TLM_COMPLETED:
				// the callee may have modified 'payload', and 'local_time' ('phase' can be ignored)
				quantumkeeper.set(local_time); // increase the time
				if(quantumkeeper.need_sync()) quantumkeeper.sync(); // synchronize if needed
				break;
		}
	}

	void Process()
	{
		unsigned int num_cycles;

		srand(12345);

/*
		cout << "delay (#cycles) ? : ";

		cin >> num_cycles;
*/
		num_cycles = 1;
		sc_time delay(num_cycles * cycle_time);


		while(1)
		{
			double atd1_anValue[ATD1_SIZE];
			double atd0_anValue[ATD0_SIZE];
			bool pwmValue[PWM_SIZE];

			for (int i=0; i<ATD1_SIZE; i++) {
				atd1_anValue[i] = 5.0 * ((double) rand() / (double) RAND_MAX); // Compute a random value: 0 Volts <= anValue[i] < 5 Volts
			}

			for (int i=0; i<ATD0_SIZE; i++) {
				atd0_anValue[i] = 5.0 * ((double) rand() / (double) RAND_MAX); // Compute a random value: 0 Volts <= anValue[i] < 5 Volts
			}

			quantumkeeper.inc(delay);

			Output_ATD1(atd1_anValue);
			Output_ATD0(atd0_anValue);

			quantumkeeper.sync();
			wait(input_payload_queue.get_event());

			Input(pwmValue);

		}

	}
};
