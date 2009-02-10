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

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace hcs12x {


PWM::PWM(const char *name, Object *parent) :
	Object(name, parent),
	Service<Memory<service_address_t> >(name, parent),
	Client<Memory<service_address_t> >(name, parent),
	memory_export("memory_export", this),
	memory_import("memory_import", this),
	param_baseAddress("base-address", this, baseAddress),
	param_bus_cycle_time("bus-cycle-time", this, bus_cycle_time)
{

	for (int i=0; i<8; i++) channel[i] = new Channel_t("jhdsq");

	// Reserved Register for factory testing
	pwmtst_register = pwmprsc_register = pwmscnta_register = pwmscntb_register = 0;

}

PWM::~PWM() {

}

uint8_t PWM::read(address_t address) {

	assert((address >= baseAddress) && (address - baseAddress < MEMORY_MAP_SIZE));

	uint8_t offset = address - baseAddress;

	switch (offset) {
		case PWME: return pwme_register; break;
		case PWMPOL: return pwmpol_register; break;
		case PWMCLK: return pwmclk_register; break;
		case PWMPRCLK: {
			const uint8_t pwmprclk_readMask = 0x77;
			return pwmprclk_register & pwmprclk_readMask;
		}
		break;
		case PWMCAE: return pwmcae_register; break;
		case PWMCTL: {
			const uint8_t pwmctl_readMask = 0xFC;
			return	pwmctl_register & pwmctl_readMask;
		}
		break;
		case PWMTST: {
			const uint8_t pwmtst_readMask = 0X00;
			return pwmtst_register & pwmtst_readMask;
		}
		break;
		case PWMPRSC: {
			const uint8_t pwmprsc_readMask = 0x00;
			return pwmprsc_register & pwmprsc_readMask;
		}
		break;
		case PWMSCLA: return pwmscla_register; break;
		case PWMSCLB: return pwmsclb_register; break;
		case PWMSCNTA: {
			const uint8_t pwmscnta_readMask = 0x00;
			return pwmscnta_register & pwmscnta_readMask;
		}
		break;
		case PWMSCNTB: {
			const uint8_t pwmscntb_readMask = 0x00;
			return pwmscntb_register & pwmscntb_readMask;
		}
		break;
		case PWMSDN: {
			const uint8_t pwmsdn_readMask = 0xD7;
			return pwmsdn_register & pwmsdn_readMask;
		}
		break;
		default: {
			// PWMCNTx
			if ((offset >= PWMCNT0) && (offset <= PWMCNT7)) {
				return channel[offset-PWMCNT0]->getPwmcnt_register();
			}

			// PWMPERx
			if ((offset >= PWMPER0) && (offset <= PWMPER7)) {
				return channel[offset-PWMPER0]->getPWMPERValue();
			}

			// PWMDTYx
			if ((offset >= PWMDTY0) && (offset <= PWMDTY7)) {
				return channel[offset-PWMDTY0]->getPWMDTYValue();
			}
		}
	}

}

void PWM::write(address_t address, uint8_t val) {

	assert((address >= baseAddress) && (address - baseAddress < MEMORY_MAP_SIZE));

	uint8_t offset = address - baseAddress;

	switch (offset) {
		case PWME: {

			uint8_t mask = 0x01;

			pwme_register = val;
			for (uint8_t index=0; index<8; index++) {
				if (pwme_register & mask) {
					channel[index]->start();
				} else {
					channel[index]->stop();
				}
			}
		}
		break;
		case PWMPOL: {
			uint8_t mask = 0x01;

			pwmpol_register = val;

			for (uint8_t i=0; i<8; i++) {
				channel[i]->setOutput(pwmpol_register & mask);
				mask << 1;
			}
		}
		break;
		case PWMCLK:
			pwmclk_register = val; break;
		case PWMPRCLK: {
			const uint8_t pwmprclk_writeMask = 0x77; // bit 3 and 7 can't be written
			pwmprclk_register = val & pwmprclk_writeMask;
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
		case PWMSCLA: pwmscla_register = val; break;
		case PWMSCLB: pwmsclb_register = val; break;
		case PWMSCNTA: /* Intended for factory test purposes only */ break;
		case PWMSCNTB: /* Intended for factory test purposes only */ break;

		case PWMSDN: {
			// TODO: not finalized
			const uint8_t pwmsdn_writeMask = 0xF3;
			pwmsdn_register = val & pwmsdn_writeMask;
		}
		break;
		default: {
			// PWMCNTx
			if ((offset >= PWMCNT0) && (offset <= PWMCNT7)) {
				channel[offset-PWMCNT0]->setPwmcnt_register(val);
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

uint16_t PWM::getClockA() {

	uint8_t pckaMask = 0x07;
	uint8_t pcka = pwmprclk_register & pckaMask;

	if (pcka == 0) {
		return bus_cycle_time;
	} else {
		return bus_cycle_time/(2 << pcka);
	}

}

uint16_t PWM::getClockB() {

	uint8_t pckbMask = 0x70;
	uint8_t pckb = (pwmprclk_register & pckbMask) >> 4;

	if (pckb == 0) {
		return bus_cycle_time;
	} else {
		return bus_cycle_time/(2 << pckb);
	}
}

uint16_t PWM::getClockSA() {

	uint16_t clockSA;

	if (pwmscla_register != 0x00) {
		clockSA = getClockA() / (2 * pwmscla_register);
	} else {
		clockSA = getClockA() / (2 * 256);
	}

	return clockSA;
}

uint16_t PWM::getClockSB() {

	uint16_t clockSB;

	if (pwmsclb_register != 0x00) {
		clockSB = getClockB() / (2 * pwmsclb_register);
	} else {
		clockSB = getClockB() / (2 * 256);
	}

	return clockSB;
}

	//=====================================================================
	//=                  Client/Service setup methods                     =
	//=====================================================================

bool PWM::Setup() {
	return true;
}

void PWM::OnDisconnect() {
}

void PWM::Reset() {

	write(baseAddress+PWME, 0);
	write(baseAddress+PWMPOL, 0);
	write(baseAddress+PWMCLK, 0);
	write(baseAddress+PWMPRCLK, 0);
	write(baseAddress+PWMCAE, 0);
	write(baseAddress+PWMCTL, 0);
	write(baseAddress+PWMSCLA, 0);
	write(baseAddress+PWMSCLB, 0);

	write(baseAddress+PWMCNT0, 0);
	write(baseAddress+PWMCNT1, 0);
	write(baseAddress+PWMCNT2, 0);
	write(baseAddress+PWMCNT3, 0);
	write(baseAddress+PWMCNT4, 0);
	write(baseAddress+PWMCNT5, 0);
	write(baseAddress+PWMCNT6, 0);
	write(baseAddress+PWMCNT7, 0);

	write(baseAddress+PWMPER0, 0);
	write(baseAddress+PWMPER1, 0);
	write(baseAddress+PWMPER2, 0);
	write(baseAddress+PWMPER3, 0);
	write(baseAddress+PWMPER4, 0);
	write(baseAddress+PWMPER5, 0);
	write(baseAddress+PWMPER6, 0);
	write(baseAddress+PWMPER7, 0);

	write(baseAddress+PWMDTY0, 0);
	write(baseAddress+PWMDTY1, 0);
	write(baseAddress+PWMDTY2, 0);
	write(baseAddress+PWMDTY3, 0);
	write(baseAddress+PWMDTY4, 0);
	write(baseAddress+PWMDTY5, 0);
	write(baseAddress+PWMDTY6, 0);
	write(baseAddress+PWMDTY7, 0);

	write(baseAddress+PWMSDN, 0);

}

	//=====================================================
	//=             PWM Channel methods                   =
	//=====================================================

PWM::Channel_t::Channel_t(const sc_module_name& name) : sc_module(name) {

	state = STOP;
	setOutput(false);

	SC_HAS_PROCESS(Channel_t);

	SC_THREAD(Run);

}

void PWM::Channel_t::Run() {

	/* TODO:
	 *  - Take in account the channel state (COUNT or STOP)
	 *  - Take in account the channel Output Alignment
	 *  - Is Channel concatenated ?
	 *  - Select clock source using PCLKx
	 *     channel 0, 1, 4, 5 then clock A or SA
	 *     channel 2, 3, 6, 7 then clock B or SB
	 */
}

void PWM::Channel_t::start() { state = COUNT; }

void PWM::Channel_t::stop() { state = STOP; }

bool PWM::Channel_t::isOutput() { return output; }
void PWM::Channel_t::setOutput(bool val) { output = val; }

uint8_t PWM::Channel_t::getPwmcnt_register() { return pwmcnt_register; }
void PWM::Channel_t::setPwmcnt_register(uint8_t val) {
	pwmcnt_register = 0;
	setPWMPERValue(getPWMPERBuffer());
	setPWMDTYValue(getPWMDTYBuffer());
}

uint8_t PWM::Channel_t::getPWMPERValue() { return pwmper_register_value; }
void PWM::Channel_t::setPWMPERValue(uint8_t val) {
	pwmper_register_value = val;
	if (pwmper_register_value == 0) setPwmcnt_register(0);
}
uint8_t PWM::Channel_t::getPWMPERBuffer() { return pwmper_register_buffer; }
void PWM::Channel_t::setPWMPERBuffer(uint8_t val) { pwmper_register_buffer = val; }

uint8_t PWM::Channel_t::getPWMDTYValue() { return pwmdty_register_value; }
void PWM::Channel_t::setPWMDTYValue(uint8_t val) { pwmdty_register_value = val; }
uint8_t PWM::Channel_t::getPWMDTYBuffer() { return pwmdty_register_buffer; }
void PWM::Channel_t::setPWMDTYBuffer(uint8_t val) { pwmdty_register_buffer = val; }


	//=====================================================================
	//=             memory interface methods                              =
	//=====================================================================

bool PWM::ReadMemory(service_address_t addr, void *buffer, uint32_t size) {

	// TODO: read PWM registers (!à voir si ce n'est pas mieux de passer par le bus interne!)

	return false;
}

bool PWM::WriteMemory(service_address_t addr, const void *buffer, uint32_t size) {

	// TODO: write to PWM registers (!à voir si ce n'est pas mieux de passer par le bus interne!)

	return false;
}


} // end of namespace hcs12x
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

