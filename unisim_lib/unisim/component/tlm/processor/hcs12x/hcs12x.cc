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
#include "unisim/kernel/tlm/tlm.hh"
#include "unisim/component/tlm/message/simple_fsb.hh"
#include "unisim/service/interfaces/logger.hh"
#include "unisim/component/tlm/processor/hcs12x/hcs12x.hh"
#define LOCATION Function << __FUNCTION__ << File << __FILE__ << Line << __LINE__

namespace unisim {
namespace component {
namespace tlm {
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
	master_port("master-port"),
	cpu_cycle_time(),
	bus_cycle_time(),
	cpu_time(),
	bus_time(),
	last_cpu_time(),
	nice_time(),
	next_nice_time(),
	nice_time_int(10),
	ipc(1.0),
	cpu_cycle_time_int(0),
	bus_cycle_time_int(0),
	last_instruction_counter(0),
	param_nice_time("nice-time", this, nice_time_int),
	param_ipc("ipc", this, ipc),
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
	
}
	
void HCS12X ::Wait() {
/* TODO: 
 * Enter a wait state for an integer number of bus clock cycle
 * Only CPU12 clocks are stopped
 * Wait for not masked interrupt
 */

	wait(irq_event);
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

	nice_time = sc_time((double)nice_time_int, SC_PS);
	if( inherited::verbose_setup && inherited::logger_import) {
		(*inherited::logger_import) << DebugInfo << LOCATION
			<< "Setting CPU cycle time to " << cpu_cycle_time.to_string() << Endl
			<< "Setting Bus cycle time to " << bus_cycle_time.to_string() << Endl
			<< "Setting nice time to " << nice_time.to_string() << Endl
			<< "Setting IPC to " << ipc << Endl
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
	
// 20080522 REDA: move 'return' to the end of function 	
//	return;

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

// 20080522 REDA: replace the above moved 'return' 	
	return;

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
	sc_time time_per_instruction = cpu_cycle_time * ipc;
	
	while(1) {
		if(cpu_time >= bus_time) {
			bus_time += bus_cycle_time;
			inherited::OnBusCycle();
			if(cpu_time >= next_nice_time) {
				next_nice_time = cpu_time + nice_time;
				Synchronize();
			}
		}
		if( verbose_tlm_run_thread && inherited::logger_import)
			(*inherited::logger_import) << DebugInfo << LOCATION
				<< "Executing step"
				<< Endl << EndDebugInfo;
		inherited::Step();
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
  
void 
HCS12X :: 
BusRead(physical_address_t addr, 
	void *buffer, 
	uint32_t size) {
	if( verbose_tlm_commands && inherited::logger_import)
		(*inherited::logger_import) << DebugInfo << LOCATION
			<< "Performing BusRead"
			<< Endl << EndDebugInfo;
	// synchonize with bus cycle time
	BusSynchronize();

	// bus transaction request
	Pointer<SimpleFSBRequest<physical_address_t, FSB_WIDTH> > req = 
		new(req) SimpleFSBRequest<physical_address_t, FSB_WIDTH>(); 
	// bus transaction response
	Pointer<SimpleFSBResponse<FSB_WIDTH> > rsp; 
	// event notified when bus transaction response is ready
	sc_event rsp_ev; 
	// message for bus transaction
	Pointer<TlmMessage<SimpleFSBRequest<physical_address_t, FSB_WIDTH>, 
		SimpleFSBResponse<FSB_WIDTH> > > msg =
		new(msg) TlmMessage<SimpleFSBRequest<physical_address_t, FSB_WIDTH>, 
			SimpleFSBResponse<FSB_WIDTH> >(req, rsp_ev); 
			
	// transaction is a READ
	req->type = SimpleFSBRequest<physical_address_t, FSB_WIDTH>::READ; 
	// transaction address
	req->addr = addr; 
	 // actual transaction size
	req->size = size;
			
	// loop until the request succeeds
	if( verbose_tlm_commands && inherited::logger_import)
		(*inherited::logger_import) << DebugInfo << LOCATION
			<< "BusRead: loop until bus transaction request is accepted" 
			<< Endl << EndDebugInfo;
	while(!master_port->Send(msg)) {
		// if bus transaction request is not accepted then retry later
		if( verbose_tlm_commands && inherited::logger_import)
			(*inherited::logger_import) << DebugInfo << LOCATION
				<< "BusRead: retry transaction request"
				<< Endl << EndDebugInfo;
		wait(bus_cycle_time);
		cpu_time += bus_cycle_time;
		bus_time += bus_cycle_time;
		inherited::OnBusCycle();
		if( verbose_tlm_commands && inherited::logger_import)
			(*inherited::logger_import) << DebugInfo << LOCATION
				<< "BusRead: retrying transaction request"
				<< Endl << EndDebugInfo;
	}
			
	// bus transaction request has been accepted at this point
	if( verbose_tlm_commands && inherited::logger_import)
		(*inherited::logger_import) << DebugInfo << LOCATION
			<< "BusRead: transaction request accepted, waiting for response"
			<< Endl << EndDebugInfo;
	
	// wait for the bus transaction response
	sc_time prev_cpu_time = cpu_time;
	wait(rsp_ev);
	cpu_time = sc_time_stamp();
	if(cpu_time == prev_cpu_time) {
	  cerr << "Warning: cpu_time = " << cpu_time
	       << " prev_cpu_time = " << prev_cpu_time << endl;
	}
	while(bus_time < cpu_time) {
		inherited::OnBusCycle();
		bus_time += bus_cycle_time;
	}
	rsp = msg->GetResponse();
	if( verbose_tlm_commands && inherited::logger_import)
		(*inherited::logger_import) << DebugInfo << LOCATION
		 	<< "BusRead: response received"
			<< Endl << EndDebugInfo;
	
	// bus transaction response has been received at this point
	if( verbose_tlm_commands && inherited::logger_import)
		(*inherited::logger_import) << DebugInfo << LOCATION
		 	<< "BusRead: copying response data"
			<< Endl << EndDebugInfo;
	// copy the data from the response
	memcpy(buffer, rsp->read_data, size);
	
	if( verbose_tlm_commands && inherited::logger_import)
		(*inherited::logger_import) << DebugInfo << LOCATION
			<< "BusRead finished"
			<< Endl << EndDebugInfo;
}

  
void 
HCS12X :: 
BusWrite(physical_address_t addr, 
	const void *buffer, 
	uint32_t size) {
	if( verbose_tlm_commands && inherited::logger_import)
		(*inherited::logger_import) << DebugInfo << LOCATION
			<< "Performing BusWrite"
			<< Endl << EndDebugInfo;
	// synchonize with bus cycle time
	BusSynchronize();

	// bus transaction request
	Pointer<SimpleFSBRequest<physical_address_t, FSB_WIDTH> > req = 
		new(req) SimpleFSBRequest<physical_address_t, FSB_WIDTH>(); 
	// message for bus transaction
	Pointer<TlmMessage<SimpleFSBRequest<physical_address_t, FSB_WIDTH>, 
		SimpleFSBResponse<FSB_WIDTH> > > msg =
		new(msg) TlmMessage<SimpleFSBRequest<physical_address_t, FSB_WIDTH>, 
			SimpleFSBResponse<FSB_WIDTH> >(req); 
			
	// transaction is a WRITE
	req->type = SimpleFSBRequest<physical_address_t, FSB_WIDTH>::WRITE; 
	// transaction address
	req->addr = addr; 
	// actual transaction size
	req->size = size;
	// copy the data to the message
	memcpy(req->write_data, buffer, size);

	// loop until the request succeeds
	if( verbose_tlm_commands && inherited::logger_import)
		(*inherited::logger_import) << DebugInfo << LOCATION
			<< "BusWrite: loop until bus transaction request is accepted" 
			<< Endl << EndDebugInfo;
	while(!master_port->Send(msg)) {
		// if bus transaction request is not accepted then retry later
		if( verbose_tlm_commands && inherited::logger_import)
			(*inherited::logger_import) << DebugInfo << LOCATION
				<< "BusWrite: retry transaction request"
				<< Endl << EndDebugInfo;
		wait(bus_cycle_time);
		cpu_time += bus_cycle_time;
		bus_time += bus_cycle_time;
		inherited::OnBusCycle();
		if( verbose_tlm_commands && inherited::logger_import)
			(*inherited::logger_import) << DebugInfo << LOCATION
				<< "BusWrite: retrying transaction request"
				<< Endl << EndDebugInfo;
	}
			
	if( verbose_tlm_commands && inherited::logger_import)
		(*inherited::logger_import) << DebugInfo << LOCATION
			<< "BusWrite finished"
			<< Endl << EndDebugInfo;
}
 

} // end of namespace hcs12x
} // end of namespace processor
} // end of namespace tlm
} // end of namespace component
} // end of namespace unisim

#undef LOCATION

