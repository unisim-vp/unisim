/*
 *  Copyright (c) 2007,
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
 * Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 *          Reda NOUACER (reda.nouacer@cea.fr)
 */


#include <systemc.h>
#include "unisim/kernel/tlm2/tlm.hh"
#include "unisim/component/tlm2/processor/hcs12x/hcs12x.hh"
#include "unisim/component/tlm2/processor/hcs12x/xint.hh"

namespace unisim {
namespace component {
namespace tlm2 {
namespace processor {
namespace hcs12x {

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugError;
using unisim::kernel::logger::EndDebug;


HCS12X::
HCS12X(const sc_module_name& name, Object *parent) :
	Object(name, parent),

	sc_module(name),
	CPU(name, parent),
	cpu_cycle_time(),
	bus_cycle_time(),
	cpu_time(),
	bus_time(),
	last_cpu_time(),
	nice_time(1.0, SC_US),
	next_nice_time(),
	core_clock_int(250000),
	param_nice_time("nice-time", this, nice_time),
	param_core_clock("core-clock", this, core_clock_int),
	enable_fine_timing(false),
	param_enable_fine_timing("enable-fine-timing", this, enable_fine_timing),
	verbose_tlm_bus_synchronize(false),
	param_verbose_tlm_bus_synchronize("verbose-tlm-bus-synchronize", this, verbose_tlm_bus_synchronize),
	verbose_tlm_run_thread(false),
	param_verbose_tlm_run_thread("verbose-tlm-run-thread", this, verbose_tlm_run_thread),
	verbose_tlm_commands(false),
	param_verbose_tlm_commands("verbose-tlm-commands", this, verbose_tlm_commands),
	last_instruction_counter(0)
{

	param_nice_time.SetFormat(unisim::kernel::service::VariableBase::FMT_DEC);
	param_core_clock.SetFormat(unisim::kernel::service::VariableBase::FMT_DEC);

	SC_HAS_PROCESS(HCS12X);

	interrupt_request.register_b_transport(this, &HCS12X::asyncIntThread);
	bus_clock_socket.register_b_transport(this, &HCS12X::updateCRGClock);

	SC_THREAD(Run);

	this->Reset();

}


HCS12X ::
~HCS12X() {
}

void
HCS12X ::
Stop(int ret) {
	// Call BusSynchronize to account for the remaining time spent in the cpu core
	BusSynchronize();
	Object::Stop(-1);
}


void
HCS12X ::
Sync() {
	sc_time time_spent = cpu_time - last_cpu_time;
	last_cpu_time = cpu_time;
	wait(time_spent);
}

void HCS12X ::sleep() {
/* TODO:
 * Stop All Clocks and puts the device in standby mode.
 * Asserting the ~RESET, ~XIRQ, or ~IRQ signals ends standby mode.
 */

	inherited::setState(STOP);

	/*
	 * Don't do wait because all clocks are stopped and there is no way to reset/resume the simulation.
	 * The simulation control is done by the debugger engine.
	 */
	reportTrap();

//	wait(irq_event | reset_event);

}

void HCS12X ::wai() {
/* TODO:
 * Enter a wait state for an integer number of bus clock cycle
 * Only CPU12 clocks are stopped
 * Wait for not masked interrupts or non-masquable interrupts
 */

	inherited::setState(WAIT);

	wait(irq_event | reset_event);

}

address_t HCS12X ::getIntVector(uint8_t &ipl)
	/*
	 * The CPU issues a signal that tells the interrupt module to drive
	 * the vector address of the highest priority pending exception onto the system address bus
	 * (the CPU12 does not provide this address)
	 *
	 * Priority is as follow: reset => sw-interrupt => hw-interrupt => spurious interrupt
	 *
	 * If (RAM_ACC_VIOL | SYS || SWI || TRAP) return IVBR;
	 * Else return INT_Vector
	 */
{

	address_t address; // get INT_VECTOR from XINT

	// The XINT module need the current IPL as input
	// The XINT return a newIPL and the interrupt vector (IVBR if there is no Hardware interrupts)

	INT_TRANS_T buffer;

	buffer.ipl = ipl;
	buffer.vectorAddress = 0;

	tlm::tlm_generic_payload* trans = payloadFabric.allocate();


	trans->set_command( tlm::TLM_READ_COMMAND );
	trans->set_address( 0 );
	trans->set_data_ptr( (unsigned char *) &buffer );

	trans->set_data_length( sizeof(INT_TRANS_T) );
	trans->set_streaming_width( sizeof(INT_TRANS_T) );

	trans->set_byte_enable_ptr( 0 );
	trans->set_dmi_allowed( false );
	trans->set_response_status( tlm::TLM_INCOMPLETE_RESPONSE );

	toXINT->b_transport( *trans, tlm2_btrans_time );

	if (trans->is_response_error() )
		SC_REPORT_ERROR("HCS12X : ", "Unable to compute interrupt vector");

	ipl = buffer.ipl;
	address = buffer.vectorAddress;

	trans->release();

	/*
	 * - Set HasReset, HasXIRQ, HasIbit interrupts flag to be used by Handle asynchronous interrupts
	 * - or compute the interrupt vector if is a Software interrupt
	 * */

	switch (address & 0x00FF)
	{
		case 0x00: { // The CPU is the initiator of the interrupt and only need the value of IVBR (interrupt vector base register)
			if (hasNonMaskableAccessErrorInterrupt())
				address = (address & 0xFF00) | XINT::INT_RAM_ACCESS_VIOLATION_OFFSET;

			if (hasNonMaskableSWIInterrupt())
				address = (address & 0xFF00) | XINT::INT_SWI_OFFSET;

			if (hasTrapInterrupt())
				address = (address & 0xFF00) | XINT::INT_TRAP_OFFSET;

			if (hasSysCallInterrupt())
				address = (address & 0xFF00) | XINT::INT_SYSCALL_OFFSET;
		} break;
		case XINT::INT_SYS_RESET_OFFSET:
			/*
			 * Are mapped to vector 0xFFFE: Pin reset, Power-on reset, low-voltage reset, illegal address reset
			 */
			reqReset();
			break;
		case XINT::INT_CLK_MONITOR_RESET_OFFSET:
			reqReset();
			break;
		case XINT::INT_COP_WATCHDOG_RESET_OFFSET:
			reqReset();
			break;
		case XINT::INT_XIRQ_OFFSET:
			reqXIRQInterrupt();
			break;
		default:
			reqIbitInterrupt();
			break;
	}

	return (address);
}

bool
HCS12X ::BeginSetup() {

	if(!inherited::BeginSetup()) {
		if(debug_enabled && verbose_step)
			*inherited::logger << DebugError
				<< "Error while trying to set up the HCS12X cpu"
				<< std::endl << EndDebugError;
		return (false);
	}

	/* check verbose settings */
	if( debug_enabled && inherited::verbose_all) {
		verbose_tlm_bus_synchronize = true;
		verbose_tlm_run_thread = true;
	} else {
		if( verbose_tlm_bus_synchronize)
			*inherited::logger << DebugInfo
				<< "verbose-tlm-bus-synchronize = true"
				<< std::endl << EndDebugInfo;
		if( verbose_tlm_run_thread)
			*inherited::logger << DebugInfo
				<< "verbose-tlm-run-thread = true"
				<< std::endl << EndDebugInfo;
	}

	computeInternalTime();

	if(debug_enabled &&  inherited::verbose_setup) {
		*inherited::logger << DebugInfo
			<< "Setting CPU cycle time to " << cpu_cycle_time.to_string() << std::endl
			<< "Setting Bus cycle time to " << bus_cycle_time.to_string() << std::endl
			<< "Setting nice time to " << nice_time.to_string() << std::endl
//			<< "Setting IPC to " << ipc << std::endl
			<< EndDebugInfo;
	}

	return (true);

}

bool
HCS12X ::Setup(ServiceExportBase *srv_export) {
	return (inherited::Setup(srv_export));
}

bool
HCS12X ::EndSetup() {
	return (inherited::EndSetup());
}

void HCS12X::computeInternalTime() {

	/*
	 * From CRG specification (page 100) :
	 * The core clock is twice the bus clock as shown in Figure 2-18.
	 * But note that a CPU cycle corresponds to one bus clock.
	 */
	core_clock_time = sc_time((double)core_clock_int, SC_PS);

	bus_cycle_time = core_clock_time * 2;

	cpu_cycle_time = bus_cycle_time;

	tlm2_btrans_time = sc_time((double)0, SC_PS);

	for (int i=0; i<32; i++) opCyclesArray[i] = cpu_cycle_time * i;

}


void
 HCS12X ::
BusSynchronize() {

	sc_time time_spent = cpu_time - last_cpu_time;
	last_cpu_time = cpu_time;

	while(cpu_time >= bus_time) {
		bus_time += bus_cycle_time;
	}

	sc_time sleep_time = bus_time - sc_time_stamp();
	wait(sleep_time);

	cpu_time = sc_time_stamp();
	last_cpu_time = sc_time_stamp();
	bus_time = sc_time_stamp();

	return;

	if(debug_enabled && verbose_tlm_bus_synchronize)
		*inherited::logger << DebugInfo
			<< "Bus synchro START"
			<< std::endl << EndDebugInfo;
	sc_dt::uint64 current_time_tu = sc_time_stamp().value();
	sc_dt::uint64 time_spent_tu = time_spent.value();
	if(debug_enabled && verbose_tlm_bus_synchronize)
		*inherited::logger << DebugInfo
			<< "time_spent_tu = " << time_spent_tu
			<< std::endl << EndDebugInfo;
	sc_dt::uint64 next_time_tu = current_time_tu + time_spent_tu;
	if(debug_enabled && verbose_tlm_bus_synchronize)
		*inherited::logger << DebugInfo
			<< "next_time_tu = " << next_time_tu
			<< std::endl << EndDebugInfo;
	sc_dt::uint64 bus_cycle_time_tu = bus_cycle_time.value();
	if(debug_enabled && verbose_tlm_bus_synchronize)
		*inherited::logger << DebugInfo
			<< "bus_cycle_time_tu = " << bus_cycle_time_tu
			<< std::endl << EndDebugInfo;
	sc_dt::uint64 bus_time_phase_tu = next_time_tu % bus_cycle_time_tu;
	if(debug_enabled && verbose_tlm_bus_synchronize)
		*inherited::logger << DebugInfo
			<< "bus_time_phase_tu = " << bus_time_phase_tu
			<< std::endl << EndDebugInfo;
	if(time_spent_tu || bus_time_phase_tu) {
		sc_dt::uint64 delay_tu = next_time_tu - current_time_tu + (bus_cycle_time_tu - bus_time_phase_tu);
		if(debug_enabled && verbose_tlm_bus_synchronize)
			*inherited::logger << DebugInfo
				<< "delay_tu = " << delay_tu
				<< std::endl << EndDebugInfo;
		wait(sc_time(delay_tu, false));
		cpu_time = sc_time_stamp();
		bus_time = cpu_time + bus_cycle_time;
	}
	if(debug_enabled && verbose_tlm_bus_synchronize)
		*inherited::logger << DebugInfo
			<< "Bus synchro END"
			<< std::endl << EndDebugInfo;

}


void
HCS12X::Synchronize()
{
	sc_time time_spent = cpu_time - last_cpu_time;
	last_cpu_time = cpu_time;
	if (debug_enabled) {
		std::cerr << "HCS12X: time_spent=" << time_spent << std::endl;
	}
	wait(time_spent);
	cpu_time = sc_time_stamp();
//	next_nice_sctime = sc_time_stamp() + nice_sctime;
}


void
HCS12X ::
Run() {
	uint8_t opCycles = 0;

	while(1) {


		if(debug_enabled && verbose_tlm_run_thread)
			*inherited::logger << DebugInfo
				<< "Executing step"
				<< std::endl << EndDebugInfo;

		opCycles = inherited::step();

		if(debug_enabled && verbose_tlm_run_thread)
			*inherited::logger << DebugInfo
				<< "Finished executing step"
				<< std::endl << EndDebugInfo;

		sc_time & time_per_instruction = opCyclesArray[opCycles];

		if (enable_fine_timing) {
			if (debug_enabled) {
				std::cerr << "HCS12X: time_per_instruction=" << time_per_instruction << std::endl;
			}
			
			wait(time_per_instruction);
		} else {
			cpu_time += time_per_instruction;
			if(cpu_time >= next_nice_time) {
				next_nice_time = cpu_time + nice_time;
				Synchronize();
			}
		}

	}
}

double
HCS12X ::
GetSimulatedTime() {
//	return cpu_time.to_seconds();
	return (cpu_time.to_default_time_units()/1e6);
}

void
HCS12X ::
Reset() {
	inherited::Reset();
}

void HCS12X::busWrite(address_t addr, const void *buffer, uint32_t size)
{
	tlm::tlm_generic_payload* trans = payloadFabric.allocate();

	trans->set_command( tlm::TLM_WRITE_COMMAND );
	trans->set_address( addr );
	trans->set_data_ptr( (unsigned char *)buffer );

	trans->set_data_length( size );
	trans->set_streaming_width( size );

	trans->set_byte_enable_ptr( 0 );
	trans->set_dmi_allowed( false );
	trans->set_response_status( tlm::TLM_INCOMPLETE_RESPONSE );

	socket->b_transport( *trans, tlm2_btrans_time );

	if (trans->is_response_error() )
		SC_REPORT_ERROR("HCS12X : ", "Response error from b_transport");

	trans->release();
}

void HCS12X::busRead(address_t addr, void *buffer, uint32_t size)
{
	tlm::tlm_generic_payload* trans = payloadFabric.allocate();

	trans->set_command( tlm::TLM_READ_COMMAND );
	trans->set_address( addr );
	trans->set_data_ptr( (unsigned char *)buffer );

	trans->set_data_length( size );
	trans->set_streaming_width( size );

	trans->set_byte_enable_ptr( 0 );
	trans->set_dmi_allowed( false );
	trans->set_response_status( tlm::TLM_INCOMPLETE_RESPONSE );

	socket->b_transport( *trans, tlm2_btrans_time );

	if (trans->is_response_error() )
		SC_REPORT_ERROR("HCS12X : ", "Response error from b_transport");

	trans->release();
}

void HCS12X::asyncIntThread(tlm::tlm_generic_payload& trans, sc_time& delay)
{
	// The XINT wake-up the CPU to handle asynchronous interrupt

	irq_event.notify();

	reqAsynchronousInterrupt();

	trans.set_response_status( tlm::TLM_OK_RESPONSE );

}

void HCS12X::updateCRGClock(tlm::tlm_generic_payload& trans, sc_time& delay) {

	sc_dt::uint64*   external_bus_clock = (sc_dt::uint64*) trans.get_data_ptr();
    trans.set_response_status( tlm::TLM_OK_RESPONSE );

    // From CRG specification (page 100) : BusClock = 2 * CoreClock
	core_clock_int = *external_bus_clock / 2;

	computeInternalTime();
}

} // end of namespace hcs12x
} // end of namespace processor
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#undef LOCATION

