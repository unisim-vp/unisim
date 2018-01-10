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
 */
 
#ifndef __UNISIM_COMPONENT_TLM_PROCESSOR_ARM_TCC__
#define __UNISIM_COMPONENT_TLM_PROCESSOR_ARM_TCC__

#include <systemc>
#include "unisim/kernel/tlm/tlm.hh"
#include "unisim/component/tlm/message/simple_fsb.hh"
#include "unisim/kernel/logger/logger.hh"

#define LOCATION "Location: " << __FUNCTION__ << ":" << __FILE__ << ":" << __LINE__

namespace unisim {
namespace component {
namespace tlm {
namespace processor {
namespace arm {

using namespace unisim::kernel::logger;

template<class CONFIG>
ARM<CONFIG> :: 
ARM(const sc_module_name& name, Object *parent) :
	Object(name, parent),
	sc_module(name),
	CPU<CONFIG>(name, this, parent),
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
	logger(*this),
	param_nice_time("nice-time", this, nice_time_int),
	param_ipc("ipc", this, ipc),
	param_cpu_cycle_time("cpu-cycle-time", this, cpu_cycle_time_int),
	param_bus_cycle_time("bus-cycle-time", this, bus_cycle_time_int),
	verbose_tlm_bus_synchronize(false),
	param_verbose_tlm_bus_synchronize("verbose-tlm-bus-synchronize", this, verbose_tlm_bus_synchronize),
	verbose_tlm_run_thread(false),
	param_verbose_tlm_run_thread("verbose-tlm-run-thread", this, verbose_tlm_run_thread),
	verbose_tlm_commands(false),
	param_verbose_tlm_commands("verbose-tlm-commands", this, verbose_tlm_commands) {
	SC_HAS_PROCESS(ARM);
	
	SC_THREAD(Run);
}

template<class CONFIG>
ARM<CONFIG> ::
~ARM() {
}

template<class CONFIG>
bool
ARM<CONFIG> ::
DebugEnable() {
	return CONFIG::DEBUG_ENABLE;
}
	
template<class CONFIG>
void 
ARM<CONFIG> ::
Stop(int ret) {
	// Call BusSynchronize to account for the remaining time spent in the cpu core
	BusSynchronize();
	Object::Stop(-1);
}

template<class CONFIG>
void
ARM<CONFIG> ::
Sync() {
	sc_time time_spent = cpu_time - last_cpu_time;
	last_cpu_time = cpu_time;
	wait(time_spent);
}

template<class CONFIG>
bool 
ARM<CONFIG> :: 
Setup() {
	if(!inherited::Setup()) {
		inherited::logger << DebugError << LOCATION << endl
				<< "Error while trying to set up the ARM cpu"
				<< endl << EndDebugError;
		return false;
	}
	
	/* check verbose settings */
	if(CONFIG::DEBUG_ENABLE && inherited::verbose_all) {
		verbose_tlm_bus_synchronize = true;
		verbose_tlm_run_thread = true;
	} else {
		if(CONFIG::DEBUG_ENABLE && verbose_tlm_bus_synchronize)
			inherited::logger << DebugInfo
				<< "verbose-tlm-bus-synchronize = true"
				<< endl << EndDebugInfo;		
		if(CONFIG::DEBUG_ENABLE && verbose_tlm_run_thread )
			inherited::logger << DebugInfo
				<< "verbose-tlm-run-thread = true"
				<< endl << EndDebugInfo;		
	}
	
	cpu_cycle_time = sc_time((double)cpu_cycle_time_int, SC_PS);
	bus_cycle_time = sc_time((double)bus_cycle_time_int, SC_PS);
	nice_time = sc_time((double)nice_time_int, SC_PS);
	if(CONFIG::DEBUG_ENABLE && inherited::verbose_setup ) {
		inherited::logger << DebugInfo
			<< "Setting CPU cycle time to " << cpu_cycle_time.to_string() << endl
			<< "Setting Bus cycle time to " << bus_cycle_time.to_string() << endl
			<< "Setting nice time to " << nice_time.to_string() << endl
			<< "Setting IPC to " << ipc << endl
			<< EndDebugInfo;
	}
	
	return true;
}

template<class CONFIG>
void 
ARM<CONFIG> :: 
BusSynchronize() {
//	if(cpu_time < sc_time_stamp()) {
//		cerr << "sc_time_stamp bigger than cpu_time" << endl;
//		cerr << "sc_time_stamp = " << sc_time_stamp() << endl;
//		cerr << "cpu_time = " << cpu_time << endl;
//		Object::Stop(-1);
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

	if(CONFIG::DEBUG_ENABLE && verbose_tlm_bus_synchronize )
		inherited::logger << DebugInfo
			<< "Bus synchro START" 
			<< endl << EndDebugInfo;
	sc_dt::uint64 current_time_tu = sc_time_stamp().value();
	sc_dt::uint64 time_spent_tu = time_spent.value();
	if(CONFIG::DEBUG_ENABLE && verbose_tlm_bus_synchronize )
		inherited::logger << DebugInfo
			<< "time_spent_tu = " << time_spent_tu 
			<< endl << EndDebugInfo;
	sc_dt::uint64 next_time_tu = current_time_tu + time_spent_tu;
	if(CONFIG::DEBUG_ENABLE && verbose_tlm_bus_synchronize )
		inherited::logger << DebugInfo
			<< "next_time_tu = " << next_time_tu 
			<< endl << EndDebugInfo;
	sc_dt::uint64 bus_cycle_time_tu = bus_cycle_time.value();
	if(CONFIG::DEBUG_ENABLE && verbose_tlm_bus_synchronize )
		inherited::logger << DebugInfo
			<< "bus_cycle_time_tu = " << bus_cycle_time_tu 
			<< endl << EndDebugInfo;
	sc_dt::uint64 bus_time_phase_tu = next_time_tu % bus_cycle_time_tu;
	if(CONFIG::DEBUG_ENABLE && verbose_tlm_bus_synchronize )
		inherited::logger << DebugInfo
			<< "bus_time_phase_tu = " << bus_time_phase_tu
			<< endl << EndDebugInfo;
	if(time_spent_tu || bus_time_phase_tu) {
		sc_dt::uint64 delay_tu = next_time_tu - current_time_tu + (bus_cycle_time_tu - bus_time_phase_tu);
		if(CONFIG::DEBUG_ENABLE && verbose_tlm_bus_synchronize )
			inherited::logger << DebugInfo
				<< "delay_tu = " << delay_tu
				<< endl << EndDebugInfo;
		wait(sc_time(delay_tu, false));
		cpu_time = sc_time_stamp();
		bus_time = cpu_time + bus_cycle_time;
	}
	if(CONFIG::DEBUG_ENABLE && verbose_tlm_bus_synchronize )
		inherited::logger << DebugInfo
			<< "Bus synchro END" 
			<< endl << EndDebugInfo;
}
	
template<class CONFIG>
void 
ARM<CONFIG>::Synchronize()
{
	sc_time time_spent = cpu_time - last_cpu_time;
	last_cpu_time = cpu_time;
	wait(time_spent);
	cpu_time = sc_time_stamp();
//	next_nice_sctime = sc_time_stamp() + nice_sctime;
}

template<class CONFIG>
void 
ARM<CONFIG> :: 
Run() {
	sc_time time_per_instruction = cpu_cycle_time * ipc;
//	sc_time stat_refresh_time = stat_reporting_period;
//	sc_time stats_refresh_time_step = stats_refresh_time;
	
	while(1) {
//		if(sc_time_stamp() > stat_refresh_time) {
//			stat_refresh_time = sc_time_stamp() + stat_reporting_period;
//			SendStats();
//		}
		if(cpu_time >= bus_time) {
			bus_time += bus_cycle_time;
			CPU<CONFIG>::OnBusCycle();
			if(cpu_time >= next_nice_time) {
				next_nice_time = cpu_time + nice_time;
				Synchronize();
			}
		}
		if(CONFIG::DEBUG_ENABLE && verbose_tlm_run_thread )
			inherited::logger << DebugInfo
				<< "Executing step"
				<< endl << EndDebugInfo;
		CPU<CONFIG>::StepInstruction();
		if(CONFIG::DEBUG_ENABLE && verbose_tlm_run_thread )
			inherited::logger << DebugInfo
				<< "Finished executing step"
				<< endl << EndDebugInfo;
		cpu_time += time_per_instruction;
	}
}

template<class CONFIG>
void 
ARM<CONFIG> :: 
Reset() {
}
	
template<class CONFIG> 
void 
ARM<CONFIG> :: 
PrRead(address_t addr, 
	uint8_t *buffer, 
	uint32_t size) {
	if(CONFIG::DEBUG_ENABLE && verbose_tlm_commands )
		inherited::logger << DebugInfo
			<< "Performing PrRead"
			<< endl << EndDebugInfo;
	// synchonize with bus cycle time
	BusSynchronize();

	// bus transaction request
	Pointer<SimpleFSBRequest<address_t, CONFIG::FSB_BURST_SIZE> > req = 
		new(req) SimpleFSBRequest<address_t, CONFIG::FSB_BURST_SIZE>(); 
	// bus transaction response
	Pointer<SimpleFSBResponse<CONFIG::FSB_BURST_SIZE> > rsp; 
	// event notified when bus transaction response is ready
	sc_event rsp_ev; 
	// message for bus transaction
	Pointer<TlmMessage<SimpleFSBRequest<address_t, CONFIG::FSB_BURST_SIZE>, 
		SimpleFSBResponse<CONFIG::FSB_BURST_SIZE> > > msg =
		new(msg) TlmMessage<SimpleFSBRequest<address_t, CONFIG::FSB_BURST_SIZE>, 
			SimpleFSBResponse<CONFIG::FSB_BURST_SIZE> >(req, rsp_ev); 
			
	// transaction is a READ
	req->type = SimpleFSBRequest<address_t, CONFIG::FSB_BURST_SIZE>::READ; 
	// transaction address
	req->addr = addr; 
	 // actual transaction size
	req->size = size;
			
	// loop until the request succeeds
	if(CONFIG::DEBUG_ENABLE && verbose_tlm_commands )
		inherited::logger << DebugInfo
			<< "PrRead: loop until bus transaction request is accepted" 
			<< endl << EndDebugInfo;
	while(!master_port->Send(msg)) {
		// if bus transaction request is not accepted then retry later
		if(CONFIG::DEBUG_ENABLE && verbose_tlm_commands )
			inherited::logger << DebugInfo
				<< "PrRead: retry transaction request"
				<< endl << EndDebugInfo;
		wait(bus_cycle_time);
		cpu_time += bus_cycle_time;
		bus_time += bus_cycle_time;
		inherited::OnBusCycle();
		if(CONFIG::DEBUG_ENABLE && verbose_tlm_commands )
			inherited::logger << DebugInfo
				<< "PrRead: retrying transaction request"
				<< endl << EndDebugInfo;
	}
			
	// bus transaction request has been accepted at this point
	if(CONFIG::DEBUG_ENABLE && verbose_tlm_commands )
		inherited::logger << DebugInfo
			<< "PrRead: transaction request accepted, waiting for response"
			<< endl << EndDebugInfo;
	
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
	if(CONFIG::DEBUG_ENABLE && verbose_tlm_commands )
		inherited::logger << DebugInfo
		 	<< "PrRead: response received"
			<< endl << EndDebugInfo;
	
	// bus transaction response has been received at this point
	if(CONFIG::DEBUG_ENABLE && verbose_tlm_commands )
		inherited::logger << DebugInfo
		 	<< "PrRead: copying response data"
			<< endl << EndDebugInfo;
	// copy the data from the response
	memcpy(buffer, rsp->read_data, size);
	
	if(CONFIG::DEBUG_ENABLE && verbose_tlm_commands )
		inherited::logger << DebugInfo
			<< "PrRead finished"
			<< endl << EndDebugInfo;
}

template<class CONFIG> 
void 
ARM<CONFIG> :: 
PrWrite(address_t addr, 
	const uint8_t *buffer, 
	uint32_t size) {
	if(CONFIG::DEBUG_ENABLE && verbose_tlm_commands )
		inherited::logger << DebugInfo
			<< "Performing PrWrite"
			<< endl << EndDebugInfo;
	// synchonize with bus cycle time
	BusSynchronize();

	// bus transaction request
	Pointer<SimpleFSBRequest<address_t, CONFIG::FSB_BURST_SIZE> > req = 
		new(req) SimpleFSBRequest<address_t, CONFIG::FSB_BURST_SIZE>(); 
	// message for bus transaction
	Pointer<TlmMessage<SimpleFSBRequest<address_t, CONFIG::FSB_BURST_SIZE>, 
		SimpleFSBResponse<CONFIG::FSB_BURST_SIZE> > > msg =
		new(msg) TlmMessage<SimpleFSBRequest<address_t, CONFIG::FSB_BURST_SIZE>, 
			SimpleFSBResponse<CONFIG::FSB_BURST_SIZE> >(req); 
			
	// transaction is a WRITE
	req->type = SimpleFSBRequest<address_t, CONFIG::FSB_BURST_SIZE>::WRITE; 
	// transaction address
	req->addr = addr; 
	// actual transaction size
	req->size = size;
	// copy the data to the message
	memcpy(req->write_data, buffer, size);

	// loop until the request succeeds
	if(CONFIG::DEBUG_ENABLE && verbose_tlm_commands )
		inherited::logger << DebugInfo
			<< "PrWrite: loop until bus transaction request is accepted" 
			<< endl << EndDebugInfo;
	while(!master_port->Send(msg)) {
		// if bus transaction request is not accepted then retry later
		if(CONFIG::DEBUG_ENABLE && verbose_tlm_commands )
			inherited::logger << DebugInfo
				<< "PrWrite: retry transaction request"
				<< endl << EndDebugInfo;
		wait(bus_cycle_time);
		cpu_time += bus_cycle_time;
		bus_time += bus_cycle_time;
		inherited::OnBusCycle();
		if(CONFIG::DEBUG_ENABLE && verbose_tlm_commands )
			inherited::logger << DebugInfo
				<< "PrWrite: retrying transaction request"
				<< endl << EndDebugInfo;
	}
			
	if(CONFIG::DEBUG_ENABLE && verbose_tlm_commands )
		inherited::logger << DebugInfo
			<< "PrWrite finished"
			<< endl << EndDebugInfo;
}

template<class CONFIG>
void 
ARM<CONFIG>::
SetLock(uint32_t lock, uint32_t set) {
	inherited::logger << DebugError << LOCATION << endl
		<< "Unsupported command SetLock"
		<< endl << EndDebugError;
	Object::Stop(-1);
	wait();
}

template<class CONFIG>
void 
ARM<CONFIG>::
PrInvalidateBlock(uint32_t set, uint32_t way) {
	inherited::logger << DebugError << LOCATION << endl
		<< "Unsupported command PrInvalidateBlock"
		<< endl << EndDebugError;
	Object::Stop(-1);
	wait();
}

template<class CONFIG>
void 
ARM<CONFIG>::
PrFlushBlock(uint32_t set, uint32_t way) {
	inherited::logger << DebugError << LOCATION << endl
		<< "Unsupported command PrFlushBlock"
		<< endl << EndDebugError;
	Object::Stop(-1);
	wait();
}

template<class CONFIG>
void 
ARM<CONFIG>::
PrCleanBlock(uint32_t set, uint32_t way) {
	inherited::logger << DebugError << LOCATION << endl
		<< "Unsupported command PrCleanBlock"
		<< endl << EndDebugError;
	Object::Stop(-1);
	wait();
}

template<class CONFIG>
uint32_t 
ARM<CONFIG>::
GetCacheSize() {
	inherited::logger << DebugError << LOCATION << endl
		<< "Unsupported command GetCacheSize"
		<< endl << EndDebugError;
	Object::Stop(-1);
	wait();
	
	return 0;
}

template<class CONFIG>
uint32_t 
ARM<CONFIG>::
GetCacheAssociativity() {
	inherited::logger << DebugError << LOCATION << endl
		<< "Unsupported command GetCacheAssociativity"
		<< endl << EndDebugError;
	Object::Stop(-1);
	wait();
	
	return 0;
}

template<class CONFIG>
uint32_t 
ARM<CONFIG>::
GetCacheBlockSize() {
	inherited::logger << DebugError << LOCATION << endl
		<< "Unsupported command GetCacheBlockSize"
		<< endl << EndDebugError;
	Object::Stop(-1);
	wait();
	
	return 0;
}

template<class CONFIG>
void 
ARM<CONFIG>::
Enable() {
	inherited::logger << DebugError << LOCATION << endl
		<< "Unsupported command Enable"
		<< endl << EndDebugError;
	Object::Stop(-1);
	wait();
}

template<class CONFIG>
void 
ARM<CONFIG>::
Disable() {
	inherited::logger << DebugError << LOCATION << endl
		<< "Unsupported command Disable"
		<< endl << EndDebugError;
	Object::Stop(-1);
	wait();
}

template<class CONFIG>
bool
ARM<CONFIG>::
IsEnabled() {
	inherited::logger << DebugError << LOCATION << endl
		<< "Unsupported command IsEnabled"
		<< endl << EndDebugError;
	Object::Stop(-1);
	wait();
	
	return false;
}

template<class CONFIG>
void 
ARM<CONFIG>::
PrReset() {
	inherited::logger << DebugError << LOCATION << endl
		<< "Unsupported command PrReset"
		<< endl << EndDebugError;
	Object::Stop(-1);
	wait();
}

template<class CONFIG>
void 
ARM<CONFIG>::
PrInvalidate() {
	inherited::logger << DebugError << LOCATION << endl
		<< "Unsupported command PrInvalidate"
		<< endl << EndDebugError;
	Object::Stop(-1);
	wait();
}

template<class CONFIG>
void 
ARM<CONFIG>::
PrInvalidateSet(uint32_t set) {
	inherited::logger << DebugError << LOCATION << endl
		<< "Unsupported command PrInvalideSet"
		<< endl << EndDebugError;
	Object::Stop(-1);
	wait();
}

template<class CONFIG>
void 
ARM<CONFIG>::
PrInvalidateBlock(address_t addr) {
	inherited::logger << DebugError << LOCATION << endl
		<< "Unsupported command PrInvalidateBlock"
		<< endl << EndDebugError;
	Object::Stop(-1);
	wait();
}

template<class CONFIG>
void 
ARM<CONFIG>::
PrFlushBlock(address_t addr) {
	inherited::logger << DebugError << LOCATION << endl
		<< "Unsupported command PrFlushBlock"
		<< endl << EndDebugError;
	Object::Stop(-1);
	wait();
}

template<class CONFIG>
void 
ARM<CONFIG>::
PrCleanBlock(address_t addr) {
	inherited::logger << DebugError << LOCATION << endl
		<< "Unsupported command PrCleanBlock"
		<< endl << EndDebugError;
	Object::Stop(-1);
	wait();
}

template<class CONFIG>
void 
ARM<CONFIG>::
PrZeroBlock(address_t addr) {
	inherited::logger << DebugError << LOCATION << endl
		<< "Unsupported command PrZeroBlock"
		<< endl << EndDebugError;
	Object::Stop(-1);
	wait();
}

} // end of namespace arm
} // end of namespace processor
} // end of namespace tlm
} // end of namespace component
} // end of namespace unisim

#undef LOCATION

#endif // __UNISIM_COMPONENT_TLM_PROCESSOR_ARM_TCC__
