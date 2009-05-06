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
 * Authors: Reda   Nouacer  (reda.nouacer@cea.fr)
 */

#include <unisim/component/tlm2/processor/hcs12x/pwm.hh>
#include <unisim/component/cxx/processor/hcs12x/config.hh>

namespace unisim {
namespace component {
namespace tlm2 {
namespace processor {
namespace hcs12x {

using unisim::component::cxx::processor::hcs12x::CONFIG;

template <uint8_t PWM_SIZE>
PWM<PWM_SIZE>::PWM(const sc_module_name& name, Object *parent) :
	Object(name, parent),
	sc_module(name),
	master_sock("master_socket"),
	slave_socket("slave_socket"),
	Service<Memory<service_address_t> >(name, parent),
	Client<Memory<service_address_t> >(name, parent),
	memory_export("memory_export", this),
	memory_import("memory_import", this),
	baseAddress(0x0300), // MC9S12XDP512V2 - PWM baseAddress
	param_baseAddress("base-address", this, baseAddress),
	interruptOffset(0x8C),
	param_interruptOffset("interrupt-offset", this, interruptOffset),
	bus_cycle_time_int(0),
	param_bus_cycle_time_int("bus-cycle-time", this, bus_cycle_time_int)

{
	uint8_t channel_number;
	for (int i=0; i<PWM_SIZE; i++) {
#if BYTE_ORDER == BIG_ENDIAN
		channel_number = i;
#else
		channel_number =  PWM_SIZE-i-1;
#endif

		channel[channel_number] = new Channel_t("channel", this, channel_number, &pwmcnt16_register[i], &pwmper16_register[i], &pwmdty16_register_value[i]);

	}

	// Reserved Register for factory testing
	pwmtst_register = pwmprsc_register = pwmscnta_register = pwmscntb_register = 0;

	master_sock(*this);
	interrupt_request(*this);
	slave_socket.register_b_transport(this, &PWM::read_write);

	SC_HAS_PROCESS(PWM);

	SC_THREAD(Run);
	sensitive << refresh_channel_event;

}

template <uint8_t PWM_SIZE>
PWM<PWM_SIZE>::~PWM() {

}

template <uint8_t PWM_SIZE>
void PWM<PWM_SIZE>::start() {
	for (int i=0; i<PWM_SIZE; i++) {
		channel[i]->wakeup();
	}
}

/**
 * This function is called when PWN7IN (when channel[7].mode == INPUT) input pin is asserted
 *
 * remark: PWN7IN is channel[7]
 */
template <uint8_t PWM_SIZE>
bool PWM<PWM_SIZE>::pwm7in_ChangeStatus(bool pwm7in_status) {

	const uint8_t pwm7ena_mask = 0x01;
	const uint8_t pwmlvl_mask = 0x10;
	const uint8_t set_pwm7in_mask = 0x04;
	const uint8_t clear_pwm7in_mask = 0xFB;
	const uint8_t pwm7inl_mask = 0x02;

	bool pwmLVL = ((pwmsdn_register & pwmlvl_mask) != 0);

	/**
	 * Any change from passive to asserted(active) state or from active to passive state
	 * will be flagged by setting the PWMIF flag = 1
	 */
	if (((pwmsdn_register & pwm7inl_mask) != 0) ^ pwm7in_status) {
		setPWMInterruptFlag();
	}

	if (pwm7in_status) {
		pwmsdn_register = pwmsdn_register | set_pwm7in_mask;
	} else {
		pwmsdn_register = pwmsdn_register & clear_pwm7in_mask;
	}

	if (((pwmsdn_register & pwm7inl_mask) != 0) == pwm7in_status) {
		for (int i=0; i<PWM_SIZE; i++) {
			channel[i]->setOutput(pwmLVL);
		}
	}
}

template <uint8_t PWM_SIZE>
bool PWM<PWM_SIZE>::isEmergencyShutdownEnable() {
	const uint8_t pwn7ena_mask = 0x01;

	return ((pwmsdn_register & pwn7ena_mask) != 0);
}

// Master methods
template <uint8_t PWM_SIZE>
tlm_sync_enum PWM<PWM_SIZE>::nb_transport_bw( XINT_Payload& payload, tlm_phase& phase, sc_core::sc_time& t)
{
	if(phase == BEGIN_RESP)
	{
		payload.release();
		return TLM_COMPLETED;
	}
	return TLM_ACCEPTED;
}


template <uint8_t PWM_SIZE>
void PWM<PWM_SIZE>::assertInterrupt() {
	// assert a PWM Emergency Shutdown Interrupt (vector = VectorBase + 0x8C)

	tlm_phase phase = BEGIN_REQ;
	XINT_Payload *payload = xint_payload_fabric.allocate();

	payload->interrupt_offset = interruptOffset;

	sc_time local_time = quantumkeeper.get_local_time();

	tlm_sync_enum ret = interrupt_request->nb_transport_fw(*payload, phase, local_time);

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

template <uint8_t PWM_SIZE>
void PWM<PWM_SIZE>::setPWMInterruptFlag() {

	const uint8_t pwmif_mask = 0x80;

	pwmsdn_register = pwmsdn_register | pwmif_mask;

	const uint8_t pwmie_mask = 0x40;

	if ((pwmsdn_register & pwmie_mask) != 0) {
		assertInterrupt();
	}
}


// Master methods
template <uint8_t PWM_SIZE>
void PWM<PWM_SIZE>::invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range)
{
	// Leave this empty as it is designed for memory mapped buses
}

template <uint8_t PWM_SIZE>
tlm_sync_enum PWM<PWM_SIZE>::nb_transport_bw(PWM_Payload<PWM_SIZE>& payload, tlm_phase& phase, sc_core::sc_time& t)
{
	switch(phase)
	{
		case BEGIN_REQ:
			cout << sc_time_stamp() << ":" << name() << ": received an unexpected phase BEGIN_REQ" << endl;
			sc_stop();
			wait(); // leave control to the SystemC kernel
			break;
		case END_REQ:
			cout << sc_time_stamp() << ":" << name() << ": received an unexpected phase END_REQ" << endl;
			sc_stop();
			wait(); // leave control to the SystemC kernel
			break;
		case BEGIN_RESP:
			payload.release();
			return TLM_COMPLETED;
		case END_RESP:
			cout << sc_time_stamp() << ":" << name() << ": received an unexpected phase END_RESP" << endl;
			sc_stop();
			wait(); // leave control to the SystemC kernel
			break;
		default:
			cout << sc_time_stamp() << ":" << name() << ": received an unexpected phase" << endl;
			sc_stop();
			wait(); // leave control to the SystemC kernel
			break;
	}

	return TLM_ACCEPTED;
}

template <uint8_t PWM_SIZE>
void PWM<PWM_SIZE>::Run() {

	while (true) {
		/**
		 * setup a PWM_Payload to refresh output
		 */
		bool pwmChannelOutput[PWM_SIZE];

		wait();

		for (int i=0; i < PWM_SIZE; i++) {
			pwmChannelOutput[i] = channel[i]->getOutput();
		}

		refreshOutput(pwmChannelOutput);

	}
}

template <uint8_t PWM_SIZE>
void PWM<PWM_SIZE>::refreshOutput(bool pwmValue[PWM_SIZE])
{
	tlm_phase phase = BEGIN_REQ;
	PWM_Payload<PWM_SIZE> *payload = payload_fabric.allocate();

	for (int i=0; i<PWM_SIZE; i++) {
		payload->pwmChannel[i] = pwmValue[i];
	}

	sc_time local_time = sc_time_stamp();

//	cout << sc_time_stamp() << ":" << name() << "(PWMx) : send " << payload->serialize() << endl;

	tlm_sync_enum ret = master_sock->nb_transport_fw(*payload, phase, local_time);

	switch(ret)
	{
		case TLM_ACCEPTED:
			// neither payload, nor phase and local_time have been modified by the callee
			break;
		case TLM_UPDATED:
			// the callee may have modified 'payload', 'phase' and 'local_time'
			break;
		case TLM_COMPLETED:
			// the callee may have modified 'payload', and 'local_time' ('phase' can be ignored)
			break;
	}

	/*
	 * Because we are using a non-blocking prototol,
	 * the payload is released by the slave/receiver.
	 * Hence do not call payload release method
	 * <!-- payload->release(); -->
	 */


}

template <uint8_t PWM_SIZE>
void PWM<PWM_SIZE>::read_write( tlm::tlm_generic_payload& trans, sc_time& delay )
{
	tlm::tlm_command cmd = trans.get_command();
	sc_dt::uint64 address = trans.get_address();
	uint8_t* data_ptr = (uint8_t *)trans.get_data_ptr();

	assert(address >= baseAddress);

	if (cmd == tlm::TLM_READ_COMMAND) {
		read(address - baseAddress, *data_ptr);
	} else if (cmd == tlm::TLM_WRITE_COMMAND) {
		write(address - baseAddress, *data_ptr);
	}

	trans.set_response_status( tlm::TLM_OK_RESPONSE );
}

template <uint8_t PWM_SIZE>
void PWM<PWM_SIZE>::read(uint8_t offset, uint8_t &value) {

	switch (offset) {
		case PWME: value = pwme_register; break;
		case PWMPOL: value = pwmpol_register; break;
		case PWMCLK: value = pwmclk_register; break;
		case PWMPRCLK: {
			const uint8_t pwmprclk_readMask = 0x77;
			value = pwmprclk_register & pwmprclk_readMask;
		}
		break;
		case PWMCAE: value = pwmcae_register; break;
		case PWMCTL: {
			const uint8_t pwmctl_readMask = 0xFC;
			value =	pwmctl_register & pwmctl_readMask;
		}
		break;
		case PWMTST: {
			const uint8_t pwmtst_readMask = 0X00;
			value = pwmtst_register & pwmtst_readMask;
		}
		break;
		case PWMPRSC: {
			const uint8_t pwmprsc_readMask = 0x00;
			value = pwmprsc_register & pwmprsc_readMask;
		}
		break;
		case PWMSCLA: value = pwmscla_register; break;
		case PWMSCLB: value = pwmsclb_register; break;
		case PWMSCNTA: {
			const uint8_t pwmscnta_readMask = 0x00;
			value = pwmscnta_register & pwmscnta_readMask;
		}
		break;
		case PWMSCNTB: {
			const uint8_t pwmscntb_readMask = 0x00;
			value = pwmscntb_register & pwmscntb_readMask;
		}
		break;
		case PWMSDN: {
			const uint8_t pwmsdn_readMask = 0xD7;
			value = pwmsdn_register & pwmsdn_readMask;
		}
		break;
		default: {
			// PWMCNTx
			if ((offset >= PWMCNT0) && (offset <= PWMCNT7)) {
				value = channel[offset-PWMCNT0]->getPwmcnt_register();
			}

			// PWMPERx
			if ((offset >= PWMPER0) && (offset <= PWMPER7)) {
				value = channel[offset-PWMPER0]->getPWMPERValue();
			}

			// PWMDTYx
			if ((offset >= PWMDTY0) && (offset <= PWMDTY7)) {
				value = channel[offset-PWMDTY0]->getPWMDTYValue();
			}
		}
	}

}

template <uint8_t PWM_SIZE>
void PWM<PWM_SIZE>::write(uint8_t offset, uint8_t val) {

	switch (offset) {
		case PWME: {
			bool isEnable;
			uint8_t mask = 0x01;

			for (uint8_t index=0; index<PWM_SIZE; index++) {
				isEnable = ((pwme_register & mask) != 0);
				if ((val & mask) && !isEnable) {
					channel[index]->wakeup();
				}
				if (!(val & mask) && isEnable) {
					channel[index]->disable();
				}
				mask = mask << 1;
			}
			pwme_register = val;

		}
		break;
		case PWMPOL: {
			uint8_t mask = 0x01;

			pwmpol_register = val;

			for (uint8_t i=0; i<PWM_SIZE; i++) {
				channel[i]->setOutput(pwmpol_register & mask);
				mask = mask << 1;
			}
		}
		break;
		case PWMCLK:
			pwmclk_register = val; break;
		case PWMPRCLK: {
			const uint8_t pwmprclk_writeMask = 0x77; // bit 3 and 7 can't be written
			pwmprclk_register = val & pwmprclk_writeMask;
			updateClockAB();
		}
		break;
		case PWMCAE:
			pwmcae_register = val; break;
		case PWMCTL: {
			uint8_t oldValue = pwmctl_register;
			uint8_t conDelta = (oldValue ^ val) & 0xF0;
			uint8_t conDeltaMask = 0x10;
			uint8_t pwmeRegs = pwme_register;
			uint8_t pwmeMask = 0x01;

			uint8_t newValue = val & 0x0C; // bit 0 and 1 can't be written
			for (int i=0; i<4; i++) {
				if (conDelta & conDeltaMask) // control bit has to be changed ?
				{
					if (!((pwmeRegs & pwmeMask) || (pwmeRegs & (pwmeMask << 1)))) // are channels disabled ?
					{
						newValue = ~(oldValue & conDeltaMask) | newValue;
					}
					else {
						newValue = (oldValue & conDeltaMask) | newValue;
					}
				}
				else {
					newValue = (oldValue & conDeltaMask) | newValue;
				}

				conDeltaMask = conDeltaMask << 1;
				pwmeMask = pwmeMask << 2;
			}

			pwmctl_register = newValue;

		}
			break;
		case PWMTST: /* Intended for factory test purposes only */ break;
		case PWMPRSC: /* Intended for factory test purposes only */ break;
		case PWMSCLA: {
			pwmscla_register = val;
			updateScaledClockA();
		}
		break;
		case PWMSCLB: {
			pwmsclb_register = val;
			updateScaledClockB();
		}
		break;
		case PWMSCNTA: /* Intended for factory test purposes only */ break;
		case PWMSCNTB: /* Intended for factory test purposes only */ break;

		case PWMSDN: {

			const uint8_t pwmsdn_writeMask = 0xF3;
			const uint8_t pwmif_mask = 0x80;
			const uint8_t pwm7ena_mask = 0x01;
			const uint8_t pwmrstrt_mask = 0x20;
			const uint8_t pwm7inl_mask = 0x02;
			const uint8_t pwm7in_mask = 0x04;


			/*
			 * The flag PWMIF (bit-7) is cleared by writing a logic 1 to it.
			 * Writing a 0 has no effect.
			 */
			if ((val & pwmif_mask) != 0) {
				val = val & 0x7F;
			} else {
				val = val & ((pwmsdn_register & pwmif_mask) | 0x7F);
			}

			/**
			 * The PWM can only be restarted if the PWM channel input 7 is de-asserted
			 */
			if (((val & pwmrstrt_mask) != 0) && !((pwmsdn_register & pwm7ena_mask) != 0) && !((val & pwm7ena_mask) != 0)) {
				/*
				 * After writing a logic 1 to the PWMRSTRT bit (trigger event)
				 * the PWM channel start running after the corresponding counter passes next "counter=0" phase.
				 */
				start();
			}

			/**
			 * The interrupt flag PWMIF is set when PWMENA is being asserted while the level at PWM7 is active
			 *
			 * i.e. the channel 7 is forced to INPUT mode (emergency enable) and the current pin7 signal level
			 *      is the same as the active level of the emergency shutdown
			 */
			if (((pwmsdn_register & pwm7ena_mask) == 0) && ((val & pwm7ena_mask) != 0) &&
					(((val & pwm7inl_mask) != 0) == channel[7]->getOutput()))
			{
				setPWMInterruptFlag();
			}

			pwmsdn_register = val & pwmsdn_writeMask;
		}
		break;
		default: {
			// PWMCNTx
			if ((offset >= PWMCNT0) && (offset <= PWMCNT7)) {
				// Any external write operation to the counter register sets it to zero (0)
				channel[offset-PWMCNT0]->setPwmcnt_register(0);
			}

			// PWMPERx
			if ((offset >= PWMPER0) && (offset <= PWMPER7)) {

				uint8_t pwmeMask = 0x01;

				channel[offset-PWMPER0]->setPWMPERBuffer(val);

				pwmeMask = pwmeMask << (offset - PWMPER0);
				if (!(pwme_register & pwmeMask)) {
					channel[offset-PWMPER0]->setPWMPERValue(val);
				}

			}

			// PWMDTYx
			if ((offset >= PWMDTY0) && (offset <= PWMDTY7)) {

				uint8_t pwmeMask = 0x01;

				channel[offset-PWMDTY0]->setPWMDTYBuffer(val);

				pwmeMask = pwmeMask << (offset - PWMDTY0);
				if (!(pwme_register & pwmeMask)) {
					channel[offset-PWMDTY0]->setPWMDTYValue(val);
				}

			}

		}
	}
}

template <uint8_t PWM_SIZE>
void PWM<PWM_SIZE>::updateClockAB() {

	uint8_t pckaMask = 0x07;
	uint8_t pcka = pwmprclk_register & pckaMask;

	clockA = clockVector[pcka];

	uint8_t pckbMask = 0x70;
	uint8_t pckb = (pwmprclk_register & pckbMask) >> 4;

	clockB = clockVector[pckb];

	updateScaledClockA();
	updateScaledClockB();
}

template <uint8_t PWM_SIZE>
void PWM<PWM_SIZE>::updateScaledClockA() {

	sc_time clockSA;

	if (pwmscla_register != 0x00) {
		clockSA = getClockA() / (2 * pwmscla_register);
	} else {
		clockSA = getClockA() / (2 * 256);
	}

}

template <uint8_t PWM_SIZE>
void PWM<PWM_SIZE>::updateScaledClockB() {

	sc_time clockSB;

	if (pwmsclb_register != 0x00) {
		clockSB = getClockB() / (2 * pwmsclb_register);
	} else {
		clockSB = getClockB() / (2 * 256);
	}

}

template <uint8_t PWM_SIZE>
sc_time PWM<PWM_SIZE>::getClockA() {
	return clockA;
}

template <uint8_t PWM_SIZE>
sc_time PWM<PWM_SIZE>::getClockB() {
	return clockB;
}

template <uint8_t PWM_SIZE>
sc_time PWM<PWM_SIZE>::getClockSA() {
	return clockSA;
}

template <uint8_t PWM_SIZE>
sc_time PWM<PWM_SIZE>::getClockSB() {
	return clockSB;
}

	//=====================================================================
	//=                  Client/Service setup methods                     =
	//=====================================================================
template <uint8_t PWM_SIZE>
bool PWM<PWM_SIZE>::Setup() {

	if (bus_cycle_time_int < CONFIG::MINIMAL_BUS_CLOCK_TIME)
	{
		cerr << "PWM: Incorrect Bus Clock Value.\n";

		return false;
	}

	bus_cycle_time = sc_time((double)bus_cycle_time_int, SC_PS);

	for (int i=0; i < 8; i++) {
		clockVector[i] = bus_cycle_time/(1 << i);
	}

	Reset();

	return true;
}

template <uint8_t PWM_SIZE>
void PWM<PWM_SIZE>::OnDisconnect() {
}

template <uint8_t PWM_SIZE>
void PWM<PWM_SIZE>::Reset() {

	write(PWME, 0);
	write(PWMPOL, 0);
	write(PWMCLK, 0);
	write(PWMPRCLK, 0);
	write(PWMCAE, 0);
	write(PWMCTL, 0);
	write(PWMSCLA, 0);
	write(PWMSCLB, 0);

	write(PWMCNT0, 0);
	write(PWMCNT1, 0);
	write(PWMCNT2, 0);
	write(PWMCNT3, 0);
	write(PWMCNT4, 0);
	write(PWMCNT5, 0);
	write(PWMCNT6, 0);
	write(PWMCNT7, 0);

	write(PWMPER0, 0);
	write(PWMPER1, 0);
	write(PWMPER2, 0);
	write(PWMPER3, 0);
	write(PWMPER4, 0);
	write(PWMPER5, 0);
	write(PWMPER6, 0);
	write(PWMPER7, 0);

	write(PWMDTY0, 0);
	write(PWMDTY1, 0);
	write(PWMDTY2, 0);
	write(PWMDTY3, 0);
	write(PWMDTY4, 0);
	write(PWMDTY5, 0);
	write(PWMDTY6, 0);
	write(PWMDTY7, 0);

	write(PWMSDN, 0);

}

	//=====================================================
	//=             PWM Channel methods                   =
	//=====================================================
template <uint8_t PWM_SIZE>
PWM<PWM_SIZE>::Channel_t::Channel_t(const sc_module_name& name, PWM *parent, const uint8_t channel_num, uint8_t *pwmcnt_ptr, uint8_t *pwmper_ptr, uint8_t *pwmdty_ptr) :
	sc_module(name),
	pwmParent(parent),
	channel_number(channel_num),
	pwmcnt_register_ptr(pwmcnt_ptr),
	pwmper_register_value_ptr(pwmper_ptr),
	pwmdty_register_value_ptr(pwmdty_ptr),
	output(false)

{

	channelMask = (0x01 << channel_number);

	SC_HAS_PROCESS(Channel_t);

	SC_THREAD(Run);
	sensitive << wakeup_event;
}

template <uint8_t PWM_SIZE>
void PWM<PWM_SIZE>::Channel_t::Run() {

	sc_time clk;

	uint8_t bit2Mask = 0x02;


	uint8_t ctl_register;
	uint8_t conMask = 0x10 << (channel_number / 2);
	/*
	 * PWMCTL bits interpretation
	 * bit7: CON67, bit6: CON45, bit5: CON23, bit4: CON01
	 * when
	 *  - CONxy=1 then channel x high order and channel y low order
	 *  - only channel y registers and output are used to control the behavior of the concatenated channel
	 */

	bool isEnable = false;

	while (true) {
		isEnable = ((pwmParent->pwme_register & channelMask) != 0);
		ctl_register = pwmParent->pwmctl_register;

		/*
		 * - Take in account the channel state (Enable or Disable)
		 * - Is Channel concatenated ?
		 */
		while (((ctl_register & conMask) && (channel_number % 2 == 0)) || (!isEnable))
		{
			wait(wakeup_event);

			isEnable = ((pwmParent->pwme_register & channelMask) != 0);
			ctl_register = pwmParent->pwmctl_register;
		}

		/*
	 	 *  - Select clock source using PCLKx
	 	 *     channel 0, 1, 4, 5 then clock A or SA
	 	 *     channel 2, 3, 6, 7 then clock B or SB
		 */
		if ((pwmParent->pwmclk_register & channelMask)  != 0) {
			if (channel_number & bit2Mask) {
				clk = pwmParent->getClockB();
			} else {
				clk = pwmParent->getClockA();
			}

		} else {
			if (channel_number & bit2Mask) {
				clk = pwmParent->getClockSB();
			} else {
				clk = pwmParent->getClockSA();
			}

		}

		if ((ctl_register & conMask) && (channel_number % 2 != 0)) { // 16-bit mode
			checkChangeStateAndWait<uint16_t>(clk);
		} else { // 8-bit mode
			checkChangeStateAndWait<uint8_t>(clk);
		}
	}
}

template <uint8_t PWM_SIZE>
template <class T> void PWM<PWM_SIZE>::Channel_t::checkChangeStateAndWait(const sc_time clk) {

	clock_t toPeriod;
	bool	isCenterAligned = true;
	int8_t increment = 1;

	T dty = *((T *) pwmdty_register_value_ptr);
	T period = *((T *) pwmper_register_value_ptr);
	T cnt = *((T *) pwmcnt_register_ptr);
	bool isPolarityHigh = ((pwmParent->pwmpol_register & channelMask) != 0);

	// PWM Boundary Cases
	if (((dty == 0x00) && (period > 0x00) && !isPolarityHigh) ||
			((period == 0x00) && isPolarityHigh) ||
			((dty >= period) && isPolarityHigh))
	{
		output = true;
	} else if (((dty == 0x00) && (period > 0x00) && isPolarityHigh) ||
			((period == 0x00) && !isPolarityHigh) ||
			((dty >= period) && !isPolarityHigh))
	{
		output = false;
	} else if (isPolarityHigh) {
		output = true;
	} else {
		output = false;
	}

	pwmParent->refresh_channel_event.notify();

	if (period == 0x00) 	// Counter = 0x00 and does not count
	{
		setPwmcnt_register(0);
		wait(wakeup_event);
		return;
	}

	// Check alignment and compute the current period
	isCenterAligned = ((pwmParent->pwmcae_register & channelMask) != 0);  // is Center Aligned ?

	toPeriod = period - dty;

	*((T *) pwmcnt_register_ptr) = dty;

	wait(dty*clk);

	output = ~output;
	pwmParent->refresh_channel_event.notify();

	if (*((T *) pwmcnt_register_ptr) == dty) // The counter can be reset by software during wait
	{
		*((T *) pwmcnt_register_ptr) = period;

		wait(toPeriod*clk);

		if (*((T *) pwmcnt_register_ptr) == period) // The counter can be reset by software during wait
		{
			if (isCenterAligned) {
				// count in decrement mode

				*((T *) pwmcnt_register_ptr) = dty;

				wait(toPeriod*clk);
			}

			output = ~output;
			pwmParent->refresh_channel_event.notify();

			if (isCenterAligned) {

				if (*((T *) pwmcnt_register_ptr) == dty) // The counter can be reset by software during wait
				{
					wait(dty*clk);
				}

			}

			setPwmcnt_register(0);	// end of period

		}

	}

}

template <uint8_t PWM_SIZE>
void PWM<PWM_SIZE>::Channel_t::wakeup() {
	wakeup_event.notify();
}

template <uint8_t PWM_SIZE>
void PWM<PWM_SIZE>::Channel_t::disable() {
	/*
	 *  Buffered registers are updated :
	 *  - The effective period ends
	 *  - The counter is written (counter reset to $00)
	 *  - The channel is disabled
	 */

	setPWMPERValue(getPWMPERBuffer());
	setPWMDTYValue(getPWMDTYBuffer());
}

template <uint8_t PWM_SIZE>
bool PWM<PWM_SIZE>::Channel_t::getOutput() { return output; }
/**
 * The channel output is meaningful only if the channel is enabled
 */
template <uint8_t PWM_SIZE>
void PWM<PWM_SIZE>::Channel_t::setOutput(bool val) {

	if ((pwmParent->pwme_register & channelMask) != 0) output = val;
}

template <uint8_t PWM_SIZE>
uint8_t PWM<PWM_SIZE>::Channel_t::getPwmcnt_register() { return *pwmcnt_register_ptr; }

template <uint8_t PWM_SIZE>
void PWM<PWM_SIZE>::Channel_t::setPwmcnt_register(uint8_t val) {
	*pwmcnt_register_ptr = val;
	if (*pwmcnt_register_ptr == 0) {
		setPWMPERValue(getPWMPERBuffer());
		setPWMDTYValue(getPWMDTYBuffer());
	}
}

template <uint8_t PWM_SIZE>
uint8_t PWM<PWM_SIZE>::Channel_t::getPWMPERValue() { return *pwmper_register_value_ptr; }

template <uint8_t PWM_SIZE>
void PWM<PWM_SIZE>::Channel_t::setPWMPERValue(uint8_t val) {
	*pwmper_register_value_ptr = val;
}

template <uint8_t PWM_SIZE>
uint8_t PWM<PWM_SIZE>::Channel_t::getPWMPERBuffer() { return pwmper_register_buffer; }

template <uint8_t PWM_SIZE>
void PWM<PWM_SIZE>::Channel_t::setPWMPERBuffer(uint8_t val) { pwmper_register_buffer = val; }

template <uint8_t PWM_SIZE>
uint8_t PWM<PWM_SIZE>::Channel_t::getPWMDTYValue() { return *pwmdty_register_value_ptr; }

template <uint8_t PWM_SIZE>
void PWM<PWM_SIZE>::Channel_t::setPWMDTYValue(uint8_t val) { *pwmdty_register_value_ptr = val; }

template <uint8_t PWM_SIZE>
uint8_t PWM<PWM_SIZE>::Channel_t::getPWMDTYBuffer() { return pwmdty_register_buffer; }

template <uint8_t PWM_SIZE>
void PWM<PWM_SIZE>::Channel_t::setPWMDTYBuffer(uint8_t val) { pwmdty_register_buffer = val; }


	//=====================================================================
	//=             memory interface methods                              =
	//=====================================================================
template <uint8_t PWM_SIZE>
bool PWM<PWM_SIZE>::ReadMemory(service_address_t addr, void *buffer, uint32_t size) {

	if ((addr >= baseAddress) && (addr <= (baseAddress+PWMSDN))){
		read(addr-baseAddress, *((uint8_t *)buffer));
		return true;
	}

	return false;
}

template <uint8_t PWM_SIZE>
bool PWM<PWM_SIZE>::WriteMemory(service_address_t addr, const void *buffer, uint32_t size) {

	if ((addr >= baseAddress) && (addr <= (baseAddress+PWMSDN))){
		write(addr-baseAddress, *((uint8_t *)buffer));
		return true;
	}

	return false;
}


} // end of namespace hcs12x
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

