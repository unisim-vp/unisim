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
#include "unisim/service/interfaces/logger.hh"
#include "unisim/component/tlm2/processor/hcs12x/hcs12x.hh"
#include "unisim/component/tlm2/processor/hcs12x/xint.hh"

#define LOCATION Function << __FUNCTION__ << File << __FILE__ << Line << __LINE__

namespace unisim {
namespace component {
namespace tlm2 {
namespace processor {
namespace hcs12x {

using unisim::service::interfaces::Hex;
using unisim::service::interfaces::Dec;
using unisim::service::interfaces::Endl;
using unisim::service::interfaces::DebugInfo;
using unisim::service::interfaces::DebugWarning;
using unisim::service::interfaces::DebugError;
using unisim::service::interfaces::EndDebugInfo;
using unisim::service::interfaces::EndDebugWarning;
using unisim::service::interfaces::EndDebugError;
using unisim::service::interfaces::Function;
using unisim::service::interfaces::File;
using unisim::service::interfaces::Line;


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
	nice_time(),
	next_nice_time(),
	nice_time_int(1000000000000ULL),
	cpu_cycle_time_int(0),
	bus_cycle_time_int(0),
	last_instruction_counter(0),
	param_nice_time("nice-time", this, nice_time_int),
	param_cpu_cycle_time("cpu-cycle-time", this, cpu_cycle_time_int),
	param_bus_cycle_time("bus-cycle-time", this, bus_cycle_time_int),
	verbose_tlm_bus_synchronize(false),
	param_verbose_tlm_bus_synchronize("verbose-tlm-bus-synchronize", this, verbose_tlm_bus_synchronize),
	verbose_tlm_run_thread(false),
	param_verbose_tlm_run_thread("verbose-tlm-run-thread", this, verbose_tlm_run_thread),
	verbose_tlm_commands(false),
	param_verbose_tlm_commands("verbose-tlm-commands", this, verbose_tlm_commands)
{

	SC_HAS_PROCESS(HCS12X);

	SC_THREAD(AsyncIntThread);
	sensitive << interruptRequest;

	SC_THREAD(Run);

}


HCS12X ::
~HCS12X() {
}


bool
 HCS12X ::
DebugEnable() {
	return inherited::logger_import;
}


void
HCS12X ::
Stop(int ret) {
	// Call BusSynchronize to account for the remaining time spent in the cpu core
	BusSynchronize();
	sc_stop();
	// Leave control to the SystemC simulation kernel
	wait();
}


void
HCS12X ::
Sync() {
	sc_time time_spent = cpu_time - last_cpu_time;
	last_cpu_time = cpu_time;
	wait(time_spent);
}

void HCS12X ::Sleep() {
/* TODO:
 * Stop All Clocks and puts the device in standby mode.
 * Asserting the ~RESET, ~XIRQ, or ~IRQ signals ends standby mode.
 */

	wait(irq_event | reset_event | xirq_event);
}

void HCS12X ::Wait() {
/* TODO:
 * Enter a wait state for an integer number of bus clock cycle
 * Only CPU12 clocks are stopped
 * Wait for not masked interrupts or non-masquable interrupts
 */

	wait(irq_event | xirq_event);
}

address_t HCS12X ::GetIntVector(uint8_t &ipl)
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
			if (HasNonMaskableAccessErrorInterrupt())
				address = (address & 0xFF00) | XINT::INT_RAM_ACCESS_VIOLATION_OFFSET;

			if (HasNonMaskableSWIInterrupt())
				address = (address & 0xFF00) | XINT::INT_SWI_OFFSET;

			if (HasTrapInterrupt())
				address = (address & 0xFF00) | XINT::INT_TRAP_OFFSET;

			if (HasSysCallInterrupt())
				address = (address & 0xFF00) | XINT::INT_SYSCALL_OFFSET;
		} break;
		case XINT::INT_SYS_RESET_OFFSET:
			/*
			 * Are mapped to vector 0xFFFE: Pin reset, Power-on reset, low-voltage reset, illegal address reset
			 */
			ReqReset();
			break;
		case XINT::INT_CLK_MONITOR_RESET_OFFSET:
			ReqReset();
			break;
		case XINT::INT_COP_WATCHDOG_RESET_OFFSET:
			ReqReset();
			break;
		case XINT::INT_XIRQ_OFFSET:
			ReqXIRQInterrupt();
			break;
		default:
			ReqIbitInterrupt();
	}

	return address;
}

bool
HCS12X ::
Setup() {
	if(!inherited::Setup()) {
		if(inherited::logger_import)
			(*inherited::logger_import) << DebugError << LOCATION
				<< "Error while trying to set up the HCS12X cpu"
				<< Endl << EndDebugError;
		return false;
	}

	/* check verbose settings */
	if( inherited::verbose_all) {
		verbose_tlm_bus_synchronize = true;
		verbose_tlm_run_thread = true;
	} else {
		if( verbose_tlm_bus_synchronize && inherited::logger_import)
			(*inherited::logger_import) << DebugInfo << LOCATION
				<< "verbose-tlm-bus-synchronize = true"
				<< Endl << EndDebugInfo;
		if( verbose_tlm_run_thread && inherited::logger_import)
			(*inherited::logger_import) << DebugInfo << LOCATION
				<< "verbose-tlm-run-thread = true"
				<< Endl << EndDebugInfo;
	}

	cpu_cycle_time = sc_time((double)cpu_cycle_time_int, SC_PS);
	bus_cycle_time = sc_time((double)bus_cycle_time_int, SC_PS);

	tlm2_btrans_time = sc_time((double)0, SC_PS);

	// ***
	for (int i=0; i<32; i++) opCyclesArray[i] = cpu_cycle_time * i;
	// ****

	nice_time = sc_time((double)nice_time_int, SC_PS);
	if( inherited::verbose_setup && inherited::logger_import) {
		(*inherited::logger_import) << DebugInfo << LOCATION
			<< "Setting CPU cycle time to " << cpu_cycle_time.to_string() << Endl
			<< "Setting Bus cycle time to " << bus_cycle_time.to_string() << Endl
			<< "Setting nice time to " << nice_time.to_string() << Endl
//			<< "Setting IPC to " << ipc << Endl
			<< EndDebugInfo;
	}

	return true;
}


void
 HCS12X ::
BusSynchronize() {
//	if(cpu_time < sc_time_stamp()) {
//		cerr << "sc_time_stamp bigger than cpu_time" << endl;
//		cerr << "sc_time_stamp = " << sc_time_stamp() << endl;
//		cerr << "cpu_time = " << cpu_time << endl;
//		sc_stop();
//		wait();
//	}

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

	if( verbose_tlm_bus_synchronize && inherited::logger_import)
		(*inherited::logger_import) << DebugInfo << LOCATION
			<< "Bus synchro START"
			<< Endl << EndDebugInfo;
	sc_dt::uint64 current_time_tu = sc_time_stamp().value();
	sc_dt::uint64 time_spent_tu = time_spent.value();
	if( verbose_tlm_bus_synchronize && inherited::logger_import)
		(*inherited::logger_import) << DebugInfo << LOCATION
			<< "time_spent_tu = " << time_spent_tu
			<< Endl << EndDebugInfo;
	sc_dt::uint64 next_time_tu = current_time_tu + time_spent_tu;
	if( verbose_tlm_bus_synchronize && inherited::logger_import)
		(*inherited::logger_import) << DebugInfo << LOCATION
			<< "next_time_tu = " << next_time_tu
			<< Endl << EndDebugInfo;
	sc_dt::uint64 bus_cycle_time_tu = bus_cycle_time.value();
	if( verbose_tlm_bus_synchronize && inherited::logger_import)
		(*inherited::logger_import) << DebugInfo << LOCATION
			<< "bus_cycle_time_tu = " << bus_cycle_time_tu
			<< Endl << EndDebugInfo;
	sc_dt::uint64 bus_time_phase_tu = next_time_tu % bus_cycle_time_tu;
	if( verbose_tlm_bus_synchronize && inherited::logger_import)
		(*inherited::logger_import) << DebugInfo << LOCATION
			<< "bus_time_phase_tu = " << bus_time_phase_tu
			<< Endl << EndDebugInfo;
	if(time_spent_tu || bus_time_phase_tu) {
		sc_dt::uint64 delay_tu = next_time_tu - current_time_tu + (bus_cycle_time_tu - bus_time_phase_tu);
		if( verbose_tlm_bus_synchronize && inherited::logger_import)
			(*inherited::logger_import) << DebugInfo << LOCATION
				<< "delay_tu = " << delay_tu
				<< Endl << EndDebugInfo;
		wait(sc_time(delay_tu, false));
		cpu_time = sc_time_stamp();
		bus_time = cpu_time + bus_cycle_time;
	}
	if( verbose_tlm_bus_synchronize && inherited::logger_import)
		(*inherited::logger_import) << DebugInfo << LOCATION
			<< "Bus synchro END"
			<< Endl << EndDebugInfo;

}


void
HCS12X::Synchronize()
{
	sc_time time_spent = cpu_time - last_cpu_time;
	last_cpu_time = cpu_time;
	wait(time_spent);
	cpu_time = sc_time_stamp();
//	next_nice_sctime = sc_time_stamp() + nice_sctime;
}


void
HCS12X ::
Run() {
	uint8_t opCycles = 0;

	sc_time &time_per_instruction = opCyclesArray[0];

	while(1) {

		if(cpu_time >= next_nice_time) {
			next_nice_time = cpu_time + nice_time;
			Synchronize();
		}

		if( verbose_tlm_run_thread && inherited::logger_import)
			(*inherited::logger_import) << DebugInfo << LOCATION
				<< "Executing step"
				<< Endl << EndDebugInfo;

		opCycles = inherited::Step();

		time_per_instruction = opCyclesArray[opCycles];

		if( verbose_tlm_run_thread && inherited::logger_import)
			(*inherited::logger_import) << DebugInfo << LOCATION
				<< "Finished executing step"
				<< Endl << EndDebugInfo;
		cpu_time += time_per_instruction;
	}
}


void
HCS12X ::
Reset() {
}

void HCS12X::BusWrite(address_t addr, const void *buffer, uint32_t size)
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

void HCS12X::BusRead(address_t addr, void *buffer, uint32_t size)
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

void HCS12X::AsyncIntThread()
{
	// The XINT wake-up the CPU to handle asynchronous interrupt

	while (true) {
		wait();

		ReqAsynchronousInterrupt();
	}

}

} // end of namespace hcs12x
} // end of namespace processor
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#undef LOCATION

