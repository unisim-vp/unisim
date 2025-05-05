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

#ifndef __UNISIM_KERNEL_SCML2_CLOCK_HH__
#define __UNISIM_KERNEL_SCML2_CLOCK_HH__

#include <scml2.h>
#include <unisim/kernel/tlm2/clock.hh>
#include <unisim/kernel/variable/variable.hh>
#include <unisim/kernel/variable/sc_time/sc_time.hh>
#include <systemc>

namespace unisim {
namespace kernel {
namespace scml2 {

class Clock
	: public scml_clock
	, public unisim::kernel::tlm2::ClockPropertiesInterface
	, virtual public unisim::kernel::Object
	, public unisim::kernel::VariableBaseListener
{
public:
	typedef scml_clock Super;
	
	Clock(const char *name, unisim::kernel::Object *parent = 0);

	virtual ~Clock();
	virtual const char *kind() const;
	
	virtual const sc_core::sc_time& GetClockPeriod() const;
	virtual double GetClockDutyCycle() const;
	virtual const sc_core::sc_time& GetClockStartTime() const;
	virtual bool GetClockPosEdgeFirst() const;
	virtual bool IsClockFrozen() const;
	virtual bool IsClockLazy() const;
	virtual const sc_core::sc_event& GetClockPropertiesChangedEvent() const;

	virtual void VariableBaseNotify(const unisim::kernel::VariableBase *var);
protected:
	virtual void before_end_of_elaboration();
private:
	mutable sc_core::sc_time clock_period;
	mutable double clock_duty_cycle;
	mutable sc_core::sc_time clock_start_time;
	mutable bool clock_posedge_first;
	mutable bool lazy_clock;
	sc_core::sc_event clock_properties_changed_event;
	
	unisim::kernel::variable::Parameter<bool> param_lazy_clock;
	mutable unisim::kernel::variable::Parameter<sc_core::sc_time> param_clock_period;
	unisim::kernel::variable::Parameter<double> param_clock_duty_cycle;
	unisim::kernel::variable::Parameter<sc_core::sc_time> param_clock_start_time;
	unisim::kernel::variable::Parameter<bool> param_clock_posedge_first;
	
	void Update();
};

} // end of namespace scml2
} // end of namespace kernel
} // end of namespace unisim

#endif // __UNISIM_KERNEL_SCML2_CLOCK_HH__
 
