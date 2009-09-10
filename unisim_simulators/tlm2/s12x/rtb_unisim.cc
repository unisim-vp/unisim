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



#include "rtb_unisim.hh"

RTBStub::RTBStub(const sc_module_name& name, Object *parent) :
	ATD_PWM_STUB(name, parent)

{

}

RTBStub::~RTBStub() {

}

void RTBStub::Process() {
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
