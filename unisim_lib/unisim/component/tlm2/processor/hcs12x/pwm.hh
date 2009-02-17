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

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_HCS12X_PWM_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_HCS12X_PWM_HH__

#include <systemc.h>

#include <inttypes.h>
#include <iostream>
#include <cmath>
#include <assert.h>


#include "unisim/kernel/service/service.hh"
#include "unisim/service/interfaces/memory.hh"


#include <unisim/component/cxx/processor/hcs12x/config.hh>
#include <unisim/component/cxx/processor/hcs12x/types.hh>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace hcs12x {

using unisim::kernel::service::Object;
using unisim::kernel::service::Client;
using unisim::kernel::service::Service;
using unisim::kernel::service::ServiceExport;
using unisim::kernel::service::ServiceImport;
using unisim::kernel::service::Parameter;

using unisim::service::interfaces::Memory;

class PWM : public Service<Memory<service_address_t> >,
	public Client<Memory<service_address_t> >
{
public:

	static const uint8_t MEMORY_MAP_SIZE = 37; // number of registers
	static const uint8_t PWM_SIZE	= 8; // The Number Channel

	//=========================================================
	//=                REGISTERS OFFSETS                      =
	//=========================================================

	enum REGS_OFFSETS {PWME, PWMPOL, PWMCLK, PWMPRCLK, PWMCAE, PWMCTL,
						PWMTST /* Intended for factory test purposes only */,
						PWMPRSC /* Intended for factory test purposes only */,
						PWMSCLA, PWMSCLB,
						PWMSCNTA /* Intended for factory test purposes only */,
						PWMSCNTB /* Intended for factory test purposes only */,
						PWMCNT0, PWMCNT1, PWMCNT2, PWMCNT3, PWMCNT4, PWMCNT5, PWMCNT6, PWMCNT7,
						PWMPER0, PWMPER1, PWMPER2, PWMPER3, PWMPER4, PWMPER5, PWMPER6, PWMPER7,
						PWMDTY0, PWMDTY1, PWMDTY2, PWMDTY3, PWMDTY4, PWMDTY5, PWMDTY6, PWMDTY7, PWMSDN};


	ServiceExport<Memory<service_address_t> > memory_export;
	ServiceImport<Memory<service_address_t> > memory_import;

    PWM(const char *name, Object *parent = 0);
    ~PWM();

    clock_t getClockA();
    clock_t getClockB();
    clock_t getClockSA();
    clock_t getClockSB();

	//=====================================================================
	//=                  Client/Service setup methods                     =
	//=====================================================================

	virtual bool Setup();
	virtual void OnDisconnect();
	virtual void Reset();


	//=====================================================================
	//=             memory interface methods                              =
	//=====================================================================

	virtual bool ReadMemory(service_address_t addr, void *buffer, uint32_t size);
	virtual bool WriteMemory(service_address_t addr, const void *buffer, uint32_t size);

    uint8_t read(address_t address);
    void write(address_t address, uint8_t val);

private:
	uint32_t	bus_cycle_time;
	Parameter<uint32_t>	param_bus_cycle_time;
	address_t	baseAddress;
	Parameter<address_t>   param_baseAddress;
	sc_time nextTime[10];

	uint8_t	pwme_register, pwmpol_register, pwmclk_register, pwmprclk_register;
	uint8_t pwmcae_register, pwmctl_register;
	// Reserved Register for factory testing
	uint8_t pwmtst_register, pwmprsc_register, pwmscnta_register, pwmscntb_register;

	uint8_t pwmscla_register, pwmsclb_register;

	uint8_t pwmcnt16_register[PWM_SIZE];
	uint8_t pwmper16_register[PWM_SIZE];
	uint8_t pwmdty16_register_value[PWM_SIZE];

	class Channel_t : public sc_module {
	public:

		Channel_t(const sc_module_name& name, PWM *parent, const uint8_t channel_number, uint8_t *pwmcnt_ptr, uint8_t *pwmper_ptr, uint8_t *pwmdty_ptr);

		void Run();
		void enable();
		void disable();

		inline uint8_t getChannelMask() { return (0x01 << channel_number);}

		bool isOutput();
		void setOutput(bool val);
		uint8_t getPwmcnt_register();
		void setPwmcnt_register(uint8_t val);

		uint8_t getPWMPERValue();
		void setPWMPERValue(uint8_t val);
		uint8_t getPWMPERBuffer();
		void setPWMPERBuffer(uint8_t val);

		uint8_t getPWMDTYValue();
		void setPWMDTYValue(uint8_t val);
		uint8_t getPWMDTYBuffer();
		void setPWMDTYBuffer(uint8_t val);

	private:
		bool output;
		uint8_t *pwmcnt_register_ptr;
		uint8_t *pwmper_register_value_ptr;
		uint8_t pwmper_register_buffer;
		uint8_t *pwmdty_register_value_ptr;
		uint8_t pwmdty_register_buffer;

		uint8_t channel_number;
		PWM	*pwmParent;
		sc_event wakeup_event;

		template <class T> void checkChangeStateAndWait(const clock_t clk);

	} *channel[PWM_SIZE];

	uint8_t pwmsdn_register;

};

template <class T> void PWM::Channel_t::checkChangeStateAndWait(const clock_t clk) {
	clock_t zeroToDTY, dtyToPeriod;

	T dty = *((T *) pwmdty_register_value_ptr);
	T per = *((T *) pwmper_register_value_ptr);
	T cnt = *((T *) pwmcnt_register_ptr);

	// Check alignment and compute the current period
	if ((pwmParent->pwmcae_register & getChannelMask()) == 0)  // is Left Aligned ?
	{
		per = per - 1;
	}

	if (dty < per) {
		zeroToDTY = clk * dty;
		dtyToPeriod = per - dty;

		*((T *) pwmcnt_register_ptr) = dty;

		wait(sc_time(clk*zeroToDTY, SC_NS)); // TODO: replace this with precomputed sc_time

		output = ~output;

		if (*((T *) pwmcnt_register_ptr) == dty) // The counter can be reset by software during wait
		{
			wait(sc_time(clk*dtyToPeriod, SC_NS)); // TODO: replace this with precomputed sc_time
			setPwmcnt_register(0);	// end of period
		} else {
			// do nothing, just loop.
		}
	} else {
		wait(sc_time(clk*per, SC_NS)); // TODO: replace this with precomputed sc_time
		setPwmcnt_register(0);	// end of period
	}
}

template void PWM::Channel_t::checkChangeStateAndWait<uint8_t>(const clock_t clk);
template void PWM::Channel_t::checkChangeStateAndWait<uint16_t>(const clock_t clk);


} // end of namespace hcs12x
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim


#endif /*__UNISIM_COMPONENT_CXX_PROCESSOR_HCS12X_PWM_HH__*/

