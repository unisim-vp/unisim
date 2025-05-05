/*
 *  Copyright (c) 2017,
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
 * Authors: Gilles Mouchard (gilles.mouchard@cea.fr)
 */

#include <unisim/kernel/scml2/clock.hh>

namespace unisim {
namespace kernel {
namespace scml2 {

Clock::Clock(const char *name_, unisim::kernel::Object *parent)
	: unisim::kernel::Object(name_, parent, "Clock")
	, Super(name_)
	, clock_period(sc_core::sc_time(1.0, sc_core::SC_NS))
	, clock_duty_cycle(0.5)
	, clock_start_time(sc_core::SC_ZERO_TIME)
	, clock_posedge_first(true)
	, lazy_clock(false)
	, clock_properties_changed_event(sc_core::sc_gen_unique_name("clock_properties_changed_event"))
	, param_lazy_clock("lazy-clock", this, lazy_clock, "whether clock is lazy (fast) or really toggles (painfully slow)")
	, param_clock_period("clock-period", this, clock_period, "clock period")
	, param_clock_duty_cycle("clock-duty-cycle", this, clock_duty_cycle, "clock duty cycle (between 0.0 and 1.0 exclusive)")
	, param_clock_start_time("clock-start-time", this, clock_start_time, "clock start time")
	, param_clock_posedge_first("clock-posedge-first", this, clock_posedge_first, "whether clocks starts with posedge (rising edge) or falling edge (negedge)")
{
	param_lazy_clock.SetMutable(false);
	param_clock_period.SetMutable(false);
	param_clock_duty_cycle.SetMutable(false);
	param_clock_start_time.SetMutable(false);
	param_clock_posedge_first.SetMutable(false);
}

Clock::~Clock()
{
}

const char *Clock::kind() const
{
	return "unisim::kernel::scml2::Clock";
}

void Clock::before_end_of_elaboration()
{
	Super::before_end_of_elaboration();

	Update();
	
	param_lazy_clock.AddListener(this);
	param_clock_period.AddListener(this);
	param_clock_duty_cycle.AddListener(this);
	param_clock_start_time.AddListener(this);
	param_clock_posedge_first.AddListener(this);
}

const sc_core::sc_time& Clock::GetClockPeriod() const
{
	return clock_period = Super::get_period();
}

double Clock::GetClockDutyCycle() const
{
	return clock_duty_cycle = Super::get_duty_cycle();
}

const sc_core::sc_time& Clock::GetClockStartTime() const
{
	return clock_start_time = Super::get_start_time();
}

bool Clock::GetClockPosEdgeFirst() const
{
	return clock_posedge_first = Super::get_posedge_first();
}

bool Clock::IsClockFrozen() const
{
	return !Super::running();
}

bool Clock::IsClockLazy() const
{
	return Super::disabled();
}

const sc_core::sc_event& Clock::GetClockPropertiesChangedEvent() const
{
	return clock_properties_changed_event;
}

void Clock::VariableBaseNotify(const unisim::kernel::VariableBase *var)
{
	Update();
	clock_properties_changed_event.notify(sc_core::SC_ZERO_TIME);
}

void Clock::Update()
{
	this->set_period(clock_period);
	this->set_posedge_first(clock_posedge_first);
	this->set_duty_cycle(clock_duty_cycle);
	this->set_start_time(clock_start_time);
}

} // end of namespace tlm2
} // end of namespace kernel
} // end of namespace unisim
 
