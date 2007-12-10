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

#include <systemc.h>
#include "unisim/kernel/tlm/tlm.hh"
#include "unisim/component/tlm/message/simple_fsb.hh"
#include "unisim/service/interfaces/logger.hh"

#define LOCATION Function << __FUNCTION__ << File << __FILE__ << Line << __LINE__

namespace unisim {
namespace component {
namespace tlm {
namespace processor {
namespace arm {

//using namespace full_system::processors::arm;
//using full_system::generic::bus::BS_OK;
//using full_system::generic::bus::CS_SHARED;
//using full_system::generic::bus::CS_MISS;
//using full_system::tlm::TlmMessage;
//using full_system::tlm::TlmSendIf;
//using full_system::utils::garbage_collector::Pointer;
//using full_system::utils::services::Parameter;
//using unisim::service::interfaces::operator<<;
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

template<class CONFIG>
ARM<CONFIG> :: 
ARM(const sc_module_name& name, Object *parent) :
	Object(name, parent),
	Client<StatisticReporting>(name, parent),
	Service<StatisticReportingControl>(name, parent),
	statistic_reporting_import("statistic_reporting_import", this),
	statistic_reporting_control_export("statistic_reporting_control_export", this),
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
	stat_reporting_period(),
	last_stat_reporting_time(),
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
	param_verbose_tlm_commands("verbose-tlm-commands", this, verbose_tlm_commands),
	stat_name_instruction_counter("instruction-counter"),
	stat_requires_instruction_counter(true),
	stat_name_diff_instruction_counter("diff-instruction-counter"),
	stat_requires_diff_instruction_counter(true),
	stat_name_ipc("ipc"),
	stat_requires_ipc(true) {
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
	return CONFIG::DEBUG_ENABLE && inherited::logger_import;
}
	
template<class CONFIG>
void 
ARM<CONFIG> ::
Stop(int ret) {
	// Call BusSynchronize to account for the remaining time spent in the cpu core
	BusSynchronize();
	sc_stop();
	wait();
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
		if(inherited::logger_import)
			(*inherited::logger_import) << DebugError << LOCATION
				<< "Error while trying to set up the ARM cpu"
				<< Endl << EndDebugError;
		return false;
	}
	
	/* check verbose settings */
	if(CONFIG::DEBUG_ENABLE && inherited::verbose_all) {
		verbose_tlm_bus_synchronize = true;
		verbose_tlm_run_thread = true;
	} else {
		if(CONFIG::DEBUG_ENABLE && verbose_tlm_bus_synchronize && inherited::logger_import)
			(*inherited::logger_import) << DebugInfo << LOCATION
				<< "verbose-tlm-bus-synchronize = true"
				<< Endl << EndDebugInfo;		
		if(CONFIG::DEBUG_ENABLE && verbose_tlm_run_thread && inherited::logger_import)
			(*inherited::logger_import) << DebugInfo << LOCATION
				<< "verbose-tlm-run-thread = true"
				<< Endl << EndDebugInfo;		
	}
	
	cpu_cycle_time = sc_time((double)cpu_cycle_time_int, SC_PS);
	bus_cycle_time = sc_time((double)bus_cycle_time_int, SC_PS);
	if(stat_reporting_period.value() == 0) 
		stat_reporting_period = sc_time((double)nice_time_int, SC_PS);
	nice_time = sc_time((double)nice_time_int, SC_PS);
	if(CONFIG::DEBUG_ENABLE && inherited::verbose_setup && inherited::logger_import) {
		(*inherited::logger_import) << DebugInfo << LOCATION
			<< "Setting CPU cycle time to " << cpu_cycle_time.to_string() << Endl
			<< "Setting Bus cycle time to " << bus_cycle_time.to_string() << Endl
			<< "Setting nice time to " << nice_time.to_string() << Endl
			<< "Setting IPC to " << ipc << Endl
			<< EndDebugInfo;
	}
	
	/* setup statistic reporting */
	if(statistic_reporting_import) {
		stat_handler_instruction_counter = 
			statistic_reporting_import->RegisterStat(stat_name_instruction_counter.c_str());
		stat_handler_diff_instruction_counter =
			statistic_reporting_import->RegisterStat(stat_name_diff_instruction_counter.c_str());
		stat_handler_ipc =
			statistic_reporting_import->RegisterStat(stat_name_ipc.c_str());
	}
	return true;
}

template<class CONFIG>
void 
ARM<CONFIG> ::
SetPreferredStatReportingPeriod(uint64_t time_hint) {
	stat_reporting_period = sc_time((double)time_hint, SC_PS);
}

template<class CONFIG>
void 
ARM<CONFIG> ::
RequiresStatReporting(const char *name, bool required) {
	if(stat_name_instruction_counter.compare(name) == 0) {
		stat_requires_instruction_counter = required;
		return;
	}
	if(stat_name_diff_instruction_counter.compare(name) == 0) {
		stat_requires_diff_instruction_counter = required;
		return;
	}
	if(stat_name_ipc.compare(name) == 0) {
		stat_requires_ipc = required;
		return;
	}
}

template<class CONFIG>
void 
ARM<CONFIG> :: 
BusSynchronize() {
	sc_time time_spent = cpu_time - last_cpu_time;
	last_cpu_time = cpu_time;
	
	if(CONFIG::DEBUG_ENABLE && verbose_tlm_bus_synchronize && inherited::logger_import)
		(*inherited::logger_import) << DebugInfo << LOCATION
			<< "Bus synchro START" 
			<< Endl << EndDebugInfo;
	sc_dt::uint64 current_time_tu = sc_time_stamp().value();
	sc_dt::uint64 time_spent_tu = time_spent.value();
	if(CONFIG::DEBUG_ENABLE && verbose_tlm_bus_synchronize && inherited::logger_import)
		(*inherited::logger_import) << DebugInfo << LOCATION
			<< "time_spent_tu = " << time_spent_tu 
			<< Endl << EndDebugInfo;
	sc_dt::uint64 next_time_tu = current_time_tu + time_spent_tu;
	if(CONFIG::DEBUG_ENABLE && verbose_tlm_bus_synchronize && inherited::logger_import)
		(*inherited::logger_import) << DebugInfo << LOCATION
			<< "next_time_tu = " << next_time_tu 
			<< Endl << EndDebugInfo;
	sc_dt::uint64 bus_cycle_time_tu = bus_cycle_time.value();
	if(CONFIG::DEBUG_ENABLE && verbose_tlm_bus_synchronize && inherited::logger_import)
		(*inherited::logger_import) << DebugInfo << LOCATION
			<< "bus_cycle_time_tu = " << bus_cycle_time_tu 
			<< Endl << EndDebugInfo;
	sc_dt::uint64 bus_time_phase_tu = next_time_tu % bus_cycle_time_tu;
	if(CONFIG::DEBUG_ENABLE && verbose_tlm_bus_synchronize && inherited::logger_import)
		(*inherited::logger_import) << DebugInfo << LOCATION
			<< "bus_time_phase_tu = " << bus_time_phase_tu
			<< Endl << EndDebugInfo;
	if(time_spent_tu || bus_time_phase_tu) {
		sc_dt::uint64 delay_tu = next_time_tu - current_time_tu + (bus_cycle_time_tu - bus_time_phase_tu);
		if(CONFIG::DEBUG_ENABLE && verbose_tlm_bus_synchronize && inherited::logger_import)
			(*inherited::logger_import) << DebugInfo << LOCATION
				<< "delay_tu = " << delay_tu
				<< Endl << EndDebugInfo;
		wait(sc_time(delay_tu, false));
	}
	if(CONFIG::DEBUG_ENABLE && verbose_tlm_bus_synchronize && inherited::logger_import)
		(*inherited::logger_import) << DebugInfo << LOCATION
			<< "Bus synchro END" 
			<< Endl << EndDebugInfo;
}
	
template<class CONFIG>
void 
ARM<CONFIG>::Synchronize()
{
	sc_time time_spent = cpu_time - last_cpu_time;
	last_cpu_time = cpu_time;
	wait(time_spent);
//	next_nice_sctime = sc_time_stamp() + nice_sctime;
}

template<class CONFIG>
void 
ARM<CONFIG> :: 
Run() {
	sc_time time_per_instruction = cpu_cycle_time * ipc;
	sc_time stat_refresh_time = stat_reporting_period;
//	sc_time stats_refresh_time_step = stats_refresh_time;
	
	while(1) {
		if(sc_time_stamp() > stat_refresh_time) {
			stat_refresh_time = sc_time_stamp() + stat_reporting_period;
			SendStats();
		}
		if(cpu_time >= bus_time) {
			bus_time += bus_cycle_time;
			CPU<CONFIG>::OnBusCycle();
			if(cpu_time >= next_nice_time) {
				next_nice_time = cpu_time + nice_time;
				Synchronize();
			}
		}
		if(CONFIG::DEBUG_ENABLE && verbose_tlm_run_thread && inherited::logger_import)
			(*inherited::logger_import) << DebugInfo << LOCATION
				<< "Executing step"
				<< Endl << EndDebugInfo;
		CPU<CONFIG>::Step();
		if(CONFIG::DEBUG_ENABLE && verbose_tlm_run_thread && inherited::logger_import)
			(*inherited::logger_import) << DebugInfo << LOCATION
				<< "Finished executing step"
				<< Endl << EndDebugInfo;
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
	if(CONFIG::DEBUG_ENABLE && verbose_tlm_commands && inherited::logger_import)
		(*inherited::logger_import) << DebugInfo << LOCATION
			<< "Performing PrRead"
			<< Endl << EndDebugInfo;
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
	if(CONFIG::DEBUG_ENABLE && verbose_tlm_commands && inherited::logger_import)
		(*inherited::logger_import) << DebugInfo << LOCATION
			<< "PrRead: loop until bus transaction request is accepted" 
			<< Endl << EndDebugInfo;
	while(!master_port->Send(msg)) {
		// if bus transaction request is not accepted then retry later
		if(CONFIG::DEBUG_ENABLE && verbose_tlm_commands && inherited::logger_import)
			(*inherited::logger_import) << DebugInfo << LOCATION
				<< "PrRead: retry transaction request"
				<< Endl << EndDebugInfo;
		wait(bus_cycle_time);
		if(CONFIG::DEBUG_ENABLE && verbose_tlm_commands && inherited::logger_import)
			(*inherited::logger_import) << DebugInfo << LOCATION
				<< "PrRead: retrying transaction request"
				<< Endl << EndDebugInfo;
	}
			
	// bus transaction request has been accepted at this point
	if(CONFIG::DEBUG_ENABLE && verbose_tlm_commands && inherited::logger_import)
		(*inherited::logger_import) << DebugInfo << LOCATION
			<< "PrRead: transaction request accepted, waiting for response"
			<< Endl << EndDebugInfo;
	
	// wait for the bus transaction response
	wait(rsp_ev);
	rsp = msg->GetResponse();
	if(CONFIG::DEBUG_ENABLE && verbose_tlm_commands && inherited::logger_import)
		(*inherited::logger_import) << DebugInfo << LOCATION
		 	<< "PrRead: response received"
			<< Endl << EndDebugInfo;
	
	// bus transaction response has been received at this point
	if(CONFIG::DEBUG_ENABLE && verbose_tlm_commands && inherited::logger_import)
		(*inherited::logger_import) << DebugInfo << LOCATION
		 	<< "PrRead: copying response data"
			<< Endl << EndDebugInfo;
	// copy the data from the response
	memcpy(buffer, rsp->read_data, size);
	
	if(CONFIG::DEBUG_ENABLE && verbose_tlm_commands && inherited::logger_import)
		(*inherited::logger_import) << DebugInfo << LOCATION
			<< "PrRead finished"
			<< Endl << EndDebugInfo;
}

template<class CONFIG> 
void 
ARM<CONFIG> :: 
PrWrite(address_t addr, 
	const uint8_t *buffer, 
	uint32_t size) {
	if(CONFIG::DEBUG_ENABLE && verbose_tlm_commands && inherited::logger_import)
		(*inherited::logger_import) << DebugInfo << LOCATION
			<< "Performing PrWrite"
			<< Endl << EndDebugInfo;
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
	if(CONFIG::DEBUG_ENABLE && verbose_tlm_commands && inherited::logger_import)
		(*inherited::logger_import) << DebugInfo << LOCATION
			<< "PrWrite: loop until bus transaction request is accepted" 
			<< Endl << EndDebugInfo;
	while(!master_port->Send(msg)) {
		// if bus transaction request is not accepted then retry later
		if(CONFIG::DEBUG_ENABLE && verbose_tlm_commands && inherited::logger_import)
			(*inherited::logger_import) << DebugInfo << LOCATION
				<< "PrWrite: retry transaction request"
				<< Endl << EndDebugInfo;
		wait(bus_cycle_time);
		if(CONFIG::DEBUG_ENABLE && verbose_tlm_commands && inherited::logger_import)
			(*inherited::logger_import) << DebugInfo << LOCATION
				<< "PrWrite: retrying transaction request"
				<< Endl << EndDebugInfo;
	}
			
	if(CONFIG::DEBUG_ENABLE && verbose_tlm_commands && inherited::logger_import)
		(*inherited::logger_import) << DebugInfo << LOCATION
			<< "PrWrite finished"
			<< Endl << EndDebugInfo;
}

template<class CONFIG>
void 
ARM<CONFIG>::
SetLock(uint32_t lock, uint32_t set) {
	if(inherited::logger_import) 
		(*inherited::logger_import) << DebugError << LOCATION
			<< "Unsupported command SetLock"
			<< Endl << EndDebugError;
	sc_stop();
	wait();
}

template<class CONFIG>
void 
ARM<CONFIG>::
PrInvalidateBlock(uint32_t set, uint32_t way) {
	if(inherited::logger_import) 
		(*inherited::logger_import) << DebugError << LOCATION
			<< "Unsupported command PrInvalidateBlock"
			<< Endl << EndDebugError;
	sc_stop();
	wait();
}

template<class CONFIG>
void 
ARM<CONFIG>::
PrFlushBlock(uint32_t set, uint32_t way) {
	if(inherited::logger_import) 
		(*inherited::logger_import) << DebugError << LOCATION
			<< "Unsupported command PrFlushBlock"
			<< Endl << EndDebugError;
	sc_stop();
	wait();
}

template<class CONFIG>
void 
ARM<CONFIG>::
PrCleanBlock(uint32_t set, uint32_t way) {
	if(inherited::logger_import) 
		(*inherited::logger_import) << DebugError << LOCATION
			<< "Unsupported command PrCleanBlock"
			<< Endl << EndDebugError;
	sc_stop();
	wait();
}

template<class CONFIG>
uint32_t 
ARM<CONFIG>::
GetCacheSize() {
	if(inherited::logger_import) 
		(*inherited::logger_import) << DebugError << LOCATION
			<< "Unsupported command GetCacheSize"
			<< Endl << EndDebugError;
	sc_stop();
	wait();
	
	return 0;
}

template<class CONFIG>
uint32_t 
ARM<CONFIG>::
GetCacheAssociativity() {
	if(inherited::logger_import) 
		(*inherited::logger_import) << DebugError << LOCATION
			<< "Unsupported command GetCacheAssociativity"
			<< Endl << EndDebugError;
	sc_stop();
	wait();
	
	return 0;
}

template<class CONFIG>
uint32_t 
ARM<CONFIG>::
GetCacheBlockSize() {
	if(inherited::logger_import) 
		(*inherited::logger_import) << DebugError << LOCATION
			<< "Unsupported command GetCacheBlockSize"
			<< Endl << EndDebugError;
	sc_stop();
	wait();
	
	return 0;
}

template<class CONFIG>
void 
ARM<CONFIG>::
Enable() {
	if(inherited::logger_import) 
		(*inherited::logger_import) << DebugError << LOCATION
			<< "Unsupported command Enable"
			<< Endl << EndDebugError;
	sc_stop();
	wait();
}

template<class CONFIG>
void 
ARM<CONFIG>::
Disable() {
	if(inherited::logger_import) 
		(*inherited::logger_import) << DebugError << LOCATION
			<< "Unsupported command Disable"
			<< Endl << EndDebugError;
	sc_stop();
	wait();
}

template<class CONFIG>
bool
ARM<CONFIG>::
IsEnabled() {
	if(inherited::logger_import) 
		(*inherited::logger_import) << DebugError << LOCATION
			<< "Unsupported command IsEnabled"
			<< Endl << EndDebugError;
	sc_stop();
	wait();
	
	return false;
}

template<class CONFIG>
void 
ARM<CONFIG>::
PrReset() {
	if(inherited::logger_import) 
		(*inherited::logger_import) << DebugError << LOCATION
			<< "Unsupported command PrReset"
			<< Endl << EndDebugError;
	sc_stop();
	wait();
}

template<class CONFIG>
void 
ARM<CONFIG>::
PrInvalidate() {
	if(inherited::logger_import) 
		(*inherited::logger_import) << DebugError << LOCATION
			<< "Unsupported command PrInvalidate"
			<< Endl << EndDebugError;
	sc_stop();
	wait();
}

template<class CONFIG>
void 
ARM<CONFIG>::
PrInvalidateSet(uint32_t set) {
	if(inherited::logger_import) 
		(*inherited::logger_import) << DebugError << LOCATION
			<< "Unsupported command PrInvalideSet"
			<< Endl << EndDebugError;
	sc_stop();
	wait();
}

template<class CONFIG>
void 
ARM<CONFIG>::
PrInvalidateBlock(address_t addr) {
	if(inherited::logger_import) 
		(*inherited::logger_import) << DebugError << LOCATION
			<< "Unsupported command PrInvalidateBlock"
			<< Endl << EndDebugError;
	sc_stop();
	wait();
}

template<class CONFIG>
void 
ARM<CONFIG>::
PrFlushBlock(address_t addr) {
	if(inherited::logger_import) 
		(*inherited::logger_import) << DebugError << LOCATION
			<< "Unsupported command PrFlushBlock"
			<< Endl << EndDebugError;
	sc_stop();
	wait();
}

template<class CONFIG>
void 
ARM<CONFIG>::
PrCleanBlock(address_t addr) {
	if(inherited::logger_import) 
		(*inherited::logger_import) << DebugError << LOCATION
			<< "Unsupported command PrCleanBlock"
			<< Endl << EndDebugError;
	sc_stop();
	wait();
}

template<class CONFIG>
void 
ARM<CONFIG>::
PrZeroBlock(address_t addr) {
	if(inherited::logger_import) 
		(*inherited::logger_import) << DebugError << LOCATION
			<< "Unsupported command PrZeroBlock"
			<< Endl << EndDebugError;
	sc_stop();
	wait();
}

template<class CONFIG>
void
ARM<CONFIG>::
SendStats() {
	sc_time cur_sc_time = sc_time_stamp();
	uint64_t cur_time = cur_sc_time.value();
	if(statistic_reporting_import) {
		uint64_t diff_instruction_counter =
			inherited::instruction_counter - last_instruction_counter;
		sc_time diff_stat_reporting_time =
			cur_sc_time - last_stat_reporting_time;
		last_stat_reporting_time = cur_sc_time;
		double cycle_count = diff_stat_reporting_time/cpu_cycle_time;
		last_instruction_counter = inherited::instruction_counter;
		double ipc = (double)diff_instruction_counter/cycle_count;
		if(stat_requires_instruction_counter)
			statistic_reporting_import->ReportStat(stat_handler_instruction_counter,
					cur_time,
					inherited::instruction_counter);
		if(stat_requires_ipc)
			statistic_reporting_import->ReportStat(stat_handler_ipc,
					cur_time,
					ipc);
		if(stat_requires_diff_instruction_counter)
			statistic_reporting_import->ReportStat(stat_handler_diff_instruction_counter,
					cur_time,
					diff_instruction_counter);
	}
}

} // end of namespace arm
} // end of namespace processor
} // end of namespace tlm
} // end of namespace component
} // end of namespace unisim

#undef LOCATION

#endif // __UNISIM_COMPONENT_TLM_PROCESSOR_ARM_TCC__
