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



#include "atd_pwm_stub.hh"

ATD_PWM_STUB::ATD_PWM_STUB(const sc_module_name& name, Object *parent) :
	Object(name),
	sc_module(name),
	atd1_master_sock("atd1_master_sock"),
	atd0_master_sock("atd0_master_sock"),
	slave_sock("slave_sock"),
	input_payload_queue("input_payload_queue"),

	anx_stimulus_period(80000000), // 80 us
	param_anx_stimulus_period("anx-stimulus-period", this, anx_stimulus_period)


{
	atd1_master_sock(*this);
	atd0_master_sock(*this);
	slave_sock(*this);

	SC_HAS_PROCESS(ATD_PWM_STUB);

	SC_THREAD(Process);

	atd0_output_file.open ("atd0_output.txt");
	atd1_output_file.open ("atd1_output.txt");
	pwm_output_file.open ("pwm_output.txt");


}

ATD_PWM_STUB::~ATD_PWM_STUB() {

	atd0_output_file.close();
	atd1_output_file.close();
	pwm_output_file.close();

}

// Slave methods
bool ATD_PWM_STUB::get_direct_mem_ptr( PWM_Payload<PWM_SIZE>& payload, tlm_dmi&  dmi_data)
{
	// Leave this empty as it is designed for memory mapped buses
	return false;
}

unsigned int ATD_PWM_STUB::transport_dbg( PWM_Payload<PWM_SIZE>& payload)
{
	// Leave this empty as it is designed for memory mapped buses
	return 0;
}

tlm_sync_enum ATD_PWM_STUB::nb_transport_fw( PWM_Payload<PWM_SIZE>& payload, tlm_phase& phase, sc_core::sc_time& t)
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

void ATD_PWM_STUB::b_transport( PWM_Payload<PWM_SIZE>& payload, sc_core::sc_time& t)
{
	input_payload_queue.notify(payload, t);
}

// Master methods
tlm_sync_enum ATD_PWM_STUB::nb_transport_bw( ATD_Payload<ATD1_SIZE>& payload, tlm_phase& phase, sc_core::sc_time& t)
{
	if(phase == BEGIN_RESP)
	{
		payload.release();
		return TLM_COMPLETED;
	}
	return TLM_ACCEPTED;
}

tlm_sync_enum ATD_PWM_STUB::nb_transport_bw( ATD_Payload<ATD0_SIZE>& payload, tlm_phase& phase, sc_core::sc_time& t)
{
	if(phase == BEGIN_RESP)
	{
		payload.release();
		return TLM_COMPLETED;
	}
	return TLM_ACCEPTED;
}

void ATD_PWM_STUB::invalidate_direct_mem_ptr( sc_dt::uint64 start_range, sc_dt::uint64 end_range)
{
}

// Implementation
void ATD_PWM_STUB::Input(bool pwmValue[PWM_SIZE])
{
	PWM_Payload<PWM_SIZE> *last_payload = NULL;
	PWM_Payload<PWM_SIZE> *payload = NULL;

	do
	{
		last_payload = payload;
		payload = input_payload_queue.get_next_transaction();

//				pwm_output_file <<  "[" << name() << "::PWM::Receive] " << payload->serialize() << " " << sc_time_stamp() << endl;
	} while(payload);

	payload = last_payload;

	pwm_output_file <<  "[" << name() << "::PWM::Receive] " << payload->serialize() << " " << sc_time_stamp() << endl;

	for (int i=0; i<PWM_SIZE; i++) {
		pwmValue[i] = payload->pwmChannel[i];
	}

}

void ATD_PWM_STUB::Output_ATD1(double anValue[ATD1_SIZE])
{

	tlm_phase phase = BEGIN_REQ;
	ATD_Payload<ATD1_SIZE> *payload = atd1_payload_fabric.allocate();

	for (int i=0; i<ATD1_SIZE; i++) {
		payload->anPort[i] = anValue[i];
	}

	atd1_output_file << "[" << name() << "::ATD1::send]" << payload->serialize() << " " << sc_time_stamp() << endl;

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

void ATD_PWM_STUB::Output_ATD0(double anValue[ATD0_SIZE])
{
	tlm_phase phase = BEGIN_REQ;
	ATD_Payload<ATD0_SIZE> *payload = atd0_payload_fabric.allocate();

	for (int i=0; i<ATD0_SIZE; i++) {
		payload->anPort[i] = anValue[i];
	}

	atd0_output_file << "[" << name() << "::ATD0::send]" << payload->serialize() << " " << sc_time_stamp() << endl;

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

void ATD_PWM_STUB::Process() {
	unsigned long num_cycles;

	srand(12345);

	sc_time delay(anx_stimulus_period, SC_PS);

	int atd0_data_index = 0;
	int atd1_data_index = 0;

	while(1)
	{
		double atd1_anValue[ATD1_SIZE];
		double atd0_anValue[ATD0_SIZE];
		bool pwmValue[PWM_SIZE];


		for (uint8_t i=0; i < ATD0_SIZE; i++) {
			atd0_anValue[i] = 5.2 * ((double) rand() / (double) RAND_MAX); // Compute a random value: 0 Volts <= anValue[i] < 5 Volts
		}

		for (uint8_t i=0; i < ATD1_SIZE; i++) {
			atd1_anValue[i] = 5.2 * ((double) rand() / (double) RAND_MAX); // Compute a random value: 0 Volts <= anValue[i] < 5 Volts
		}

		wait(input_payload_queue.get_event());

		Input(pwmValue);

		Output_ATD1(atd1_anValue);
		Output_ATD0(atd0_anValue);

		quantumkeeper.inc(delay);
		quantumkeeper.sync();
	}

}
