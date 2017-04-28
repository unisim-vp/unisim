/*
 *  Copyright (c) 2007-2010,
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

#include <unisim/component/tlm2/processor/powerpc/e200z710n3/cpu.hh>
#include <unistd.h>

#ifdef powerpc
#undef powerpc
#endif

namespace unisim {
namespace component {
namespace tlm2 {
namespace processor {
namespace powerpc {
namespace e200z710n3 {

const bool DEBUG_ENABLE = false;

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;

CPU::CPU(const sc_module_name& name, Object *parent)
	: Object(name, parent, "this module implements a E200Z710N3 CPU core")
	, sc_module(name)
	, unisim::component::cxx::processor::powerpc::e200z710n3::CPU(name, parent)
	, ahb_if("ahb_if")
	, m_por("m_por")
	, p_reset_b("p_reset_b")
	, p_nmi_b("p_nmi_b")
	, p_mcp_b("p_mcp_b")
	, p_rstbase("p_rstbase")
	, p_cpuid("p_cpuid")
	, p_extint_b("p_extint_b")
	, p_crint_b("p_crint_b")
	, p_avec_b("p_avec_b")
	, p_voffset("p_voffset")
	, p_iack("p_iack")
	, payload_fabric()
	, cpu_cycle_time()
	, bus_cycle_time()
	, cpu_time()
	, nice_time()
	, run_time()
	, idle_time()
	, enable_host_idle(false)
	, start_event("start_event")
	, external_event("external_event")
	, int_ack_event("int_ack_event")
	, ipc(1.0)
	, one(1.0)
	, enable_dmi(false)
	, debug_dmi(false)
	, param_cpu_cycle_time("cpu-cycle-time", this, cpu_cycle_time, "cpu cycle time")
	, param_bus_cycle_time("bus-cycle-time", this, bus_cycle_time, "bus cycle time")
	, param_nice_time("nice-time", this, nice_time, "maximum time between synchonizations")
	, param_ipc("ipc", this, ipc, "maximum instructions per cycle, typically 1 or 2")
	, param_enable_host_idle("enable-host-idle", this, enable_host_idle, "Enable/Disable host idle periods when target is idle")
	, param_enable_dmi("enable-dmi", this, enable_dmi, "Enable/Disable TLM 2.0 DMI (Direct Memory Access) to speed-up simulation")
	, param_debug_dmi("debug-dmi", this, debug_dmi, "Enable/Disable debugging of DMI (Direct Memory Access)")
	, stat_one("one", this, one, "one")
	, stat_run_time("run-time", this, run_time, "run time")
	, stat_idle_time("idle-time", this, idle_time, "idle time")
	, formula_idle_rate("idle-rate", this, "/", &stat_idle_time, &stat_run_time, "idle rate")
	, formula_load_rate("load-rate", this, "-", &stat_one, &formula_idle_rate, "load rate")
	, dmi_region_cache()
{
	ahb_if(*this);
	
	stat_one.SetMutable(false);
	stat_one.SetSerializable(false);
	stat_one.SetVisible(false);
	
	SC_HAS_PROCESS(CPU);
	
	SC_THREAD(Run);
	
	SC_METHOD(P_RESET_B_Process);
	sensitive << p_reset_b.pos();

	SC_METHOD(P_NMI_B_Process);
	sensitive << p_nmi_b.neg();

	SC_METHOD(P_MCP_B_Process);
	sensitive << p_mcp_b.neg();

	SC_METHOD(P_EXTIN_B_Process);
	sensitive << p_extint_b.neg();

	SC_METHOD(P_CRINT_B_Process);
	sensitive << p_crint_b.neg();
	
	SC_METHOD(P_AVEC_B_Process);
	sensitive << p_avec_b;
	
	SC_METHOD(P_IACK_Process);
	sensitive << int_ack_event;
}

CPU::~CPU()
{
}

bool CPU::EndSetup()
{
	if(!Super::EndSetup()) return false;
	return true;
}

tlm::tlm_sync_enum CPU::nb_transport_bw(tlm::tlm_generic_payload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t)
{
	if(phase == tlm::BEGIN_RESP)
	{
		trans.release();
		return tlm::TLM_COMPLETED;
	}
	return tlm::TLM_ACCEPTED;
}

void CPU::invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range)
{
	dmi_region_cache.Invalidate(start_range, end_range);
	if(DEBUG_ENABLE && debug_dmi)
	{
		Super::logger << DebugInfo << "AHB: invalidate granted access for 0x" << std::hex << start_range << "-0x" << end_range << std::dec << EndDebugInfo;
	}
}

void CPU::Synchronize()
{
	wait(cpu_time);
	cpu_time = SC_ZERO_TIME;
	run_time = sc_time_stamp();
}

inline void CPU::AlignToBusClock()
{
	sc_dt::uint64 bus_cycle_time_tu = bus_cycle_time.value();
	sc_dt::uint64 run_time_tu = run_time.value();
	sc_dt::uint64 modulo = run_time_tu % bus_cycle_time_tu;
	if(!modulo) return; // already aligned
	
	sc_dt::uint64 time_alignment_tu = bus_cycle_time_tu - modulo;
	//sc_time time_alignment(time_alignment_tu, false);
	sc_time time_alignment(sc_get_time_resolution());
	time_alignment *= time_alignment_tu;
	cpu_time += time_alignment;
	run_time += time_alignment;
}

void CPU::AlignToBusClock(sc_time& t)
{
	sc_time modulo(t);
	modulo %= bus_cycle_time;
	if(modulo == SC_ZERO_TIME) return; // already aligned
	t += bus_cycle_time - modulo;
}

void CPU::Idle()
{
	// This is called within thread Run()
	
	// First synchronize with other threads
	Synchronize();
	
	// wait for an external event
	sc_time old_time_stamp(sc_time_stamp());
	wait(external_event);
	sc_time new_time_stamp(sc_time_stamp());
	
	// compute the time spent by the SystemC wait
	sc_time delta_time(new_time_stamp);
	delta_time -= old_time_stamp;
	
	if(enable_host_idle)
	{
		usleep(delta_time.to_seconds() * 1.0e6); // leave host CPU when target CPU is idle
	}
	
	idle_time += delta_time;
	
	// update overall run time
	run_time = new_time_stamp;
}

void CPU::InterruptAcknowledge()
{
	int_ack_event.notify(sc_core::SC_ZERO_TIME);
}

bool CPU::AHBDebugInsnRead(PHYSICAL_ADDRESS physical_addr, void *buffer, uint32_t size, STORAGE_ATTR storage_attr)
{
	if(sc_core::sc_get_status() < sc_core::SC_END_OF_ELABORATION)
	{
		// operator -> of ports is not legal before end of elaboration because
		// an implementation of SystemC can defer complete binding just before end of elaboration
		// Using memory service interface instead
		return memory_import->ReadMemory(physical_addr, buffer, size);
	}

	tlm::tlm_generic_payload *payload = payload_fabric.allocate();
	
	payload->set_address(physical_addr);
	payload->set_read();
	payload->set_data_length(size);
	payload->set_streaming_width(size);
	payload->set_data_ptr((unsigned char *) buffer);
	
	unsigned int read_size = ahb_if->transport_dbg(*payload);

	return (payload->is_response_ok() && (read_size == size));
}

bool CPU::AHBDebugDataRead(PHYSICAL_ADDRESS physical_addr, void *buffer, uint32_t size, STORAGE_ATTR storage_attr)
{
	if(sc_core::sc_get_status() < sc_core::SC_END_OF_ELABORATION)
	{
		// operator -> of ports is not legal before end of elaboration because
		// an implementation of SystemC can defer complete binding just before end of elaboration
		// Using memory service interface instead
		return memory_import->ReadMemory(physical_addr, buffer, size);
	}

	tlm::tlm_generic_payload *payload = payload_fabric.allocate();
	
	payload->set_address(physical_addr);
	payload->set_read();
	payload->set_data_length(size);
	payload->set_streaming_width(size);
	payload->set_data_ptr((unsigned char *) buffer);
	
	unsigned int read_size = ahb_if->transport_dbg(*payload);

	return (payload->is_response_ok() && (read_size == size));
}

bool CPU::AHBDebugDataWrite(PHYSICAL_ADDRESS physical_addr, const void *buffer, uint32_t size, STORAGE_ATTR storage_attr)
{
	if(sc_core::sc_get_status() < sc_core::SC_END_OF_ELABORATION)
	{
		// operator -> of ports is not legal before end of elaboration because
		// an implementation of SystemC can defer complete binding just before end of elaboration
		// Using memory service interface instead
		return memory_import->WriteMemory(physical_addr, buffer, size);
	}

	tlm::tlm_generic_payload *payload = payload_fabric.allocate();
	
	payload->set_address(physical_addr);
	payload->set_write();
	payload->set_data_length(size);
	payload->set_streaming_width(size);
	payload->set_data_ptr((unsigned char *) buffer);
	
	unsigned int write_size = ahb_if->transport_dbg(*payload);

	return (payload->is_response_ok() && (write_size == size));
}

void CPU::P_IACK_Process()
{
	if(p_iack)
	{
		p_iack = false;
	}
	else
	{
		p_iack = true;
		int_ack_event.notify(bus_cycle_time);
	}
}

void CPU::P_RESET_B_Process()
{
	if(p_reset_b.posedge())
	{
		ThrowException<SystemResetInterrupt::Reset>();
		hid0.Set<HID0::NHR>(0);
		external_event.notify(sc_core::SC_ZERO_TIME);
		start_event.notify();
		SetResetAddress(sc_dt::sc_uint<30>(p_rstbase).to_uint64() << 2);
	}
}

void CPU::P_NMI_B_Process()
{
	if(p_nmi_b.negedge())
	{
		ThrowException<MachineCheckInterrupt::NMI>();
		external_event.notify(sc_core::SC_ZERO_TIME);
	}
}

void CPU::P_MCP_B_Process()
{
	if(p_mcp_b.negedge())
	{
		ThrowException<MachineCheckInterrupt::MCP>();
		external_event.notify(sc_core::SC_ZERO_TIME);
	}
}

void CPU::P_EXTIN_B_Process()
{
	if(p_extint_b)
	{
		AckInterrupt<ExternalInputInterrupt>();
	}
	else
	{
		ThrowException<ExternalInputInterrupt::ExternalInput>();
		external_event.notify(sc_core::SC_ZERO_TIME);
	}
}

void CPU::P_CRINT_B_Process()
{
	if(p_crint_b)
	{
		AckInterrupt<CriticalInputInterrupt>();
	}
	else
	{
		ThrowException<CriticalInputInterrupt::CriticalInput>();
		external_event.notify(sc_core::SC_ZERO_TIME);
	}
}

void CPU::P_AVEC_B_Process()
{
	SetAutoVector(!p_avec_b); // if p_avec_b is negated when interrupt signal is asserted, interrupt is not autovectored
	SetVectorOffset(sc_dt::sc_uint<14>(p_voffset).to_uint64() << 2);
}

void CPU::Run()
{
	wait(start_event);
	sc_time time_per_instruction = cpu_cycle_time / ipc;
	
	while(1)
	{
		Super::StepOneInstruction();
		// update local time (relative to sc_time_stamp)
		cpu_time += time_per_instruction;
		// update absolute time (local time + sc_time_stamp)
		run_time += time_per_instruction;
		// Periodically synchronize with other threads
		if(unlikely(cpu_time >= nice_time))
		{
			Synchronize();
		}
	}
}

bool CPU::AHBInsnRead(PHYSICAL_ADDRESS physical_addr, void *buffer, uint32_t size, STORAGE_ATTR storage_attr)
{
	AlignToBusClock();

	unisim::kernel::tlm2::DMIRegion *dmi_region = 0;
	
	if(likely(enable_dmi))
	{
		dmi_region = dmi_region_cache.Lookup(physical_addr, size);
		
		if(likely(dmi_region != 0))
		{
			if(unlikely(DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "AHB Instruction Read: DMI region cache hit for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
			
			if(likely(dmi_region->IsAllowed()))
			{
				if(unlikely(DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "AHB Instruction Read: DMI access allowed for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
				tlm::tlm_dmi *dmi_data = dmi_region->GetDMI();
				if(likely((dmi_data->get_granted_access() & tlm::tlm_dmi::DMI_ACCESS_READ) == tlm::tlm_dmi::DMI_ACCESS_READ))
				{
					if(unlikely(DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "AHB Instruction Read: using granted DMI access " << dmi_data->get_granted_access() << " for 0x" << std::hex << dmi_data->get_start_address() << "-0x" << dmi_data->get_end_address() << std::dec << EndDebugInfo;
					memcpy(buffer, dmi_data->get_dmi_ptr() + (physical_addr - dmi_data->get_start_address()), size);
					const sc_time& read_lat = dmi_region->GetReadLatency(size);
					cpu_time += read_lat;
					run_time += read_lat;
					return true;
				}
				else
				{
					if(unlikely(DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "AHB Instruction Read: granted DMI access does not allow direct read access for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
				}
			}
			else
			{
				if(unlikely(DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "AHB Instruction Read: DMI access denied for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
			}
		}
		else
		{
			if(unlikely(DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "AHB Instruction Read: DMI region cache miss for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
		}
	}
	
	if(unlikely(DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "AHB Instruction Read: traditional way for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
	tlm::tlm_generic_payload *payload = payload_fabric.allocate();
	
	payload->set_address(physical_addr);
	payload->set_command(tlm::TLM_READ_COMMAND);
	payload->set_data_length(size);
	payload->set_data_ptr((unsigned char *) buffer);
	
	ahb_if->b_transport(*payload, cpu_time);
	
	run_time = sc_time_stamp();
	run_time += cpu_time;
	
	tlm::tlm_response_status status = payload->get_response_status();
	
	if(likely(enable_dmi))
	{
		if(likely(!dmi_region && payload->is_dmi_allowed()))
		{
			if(unlikely(DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "AHB Instruction Read: target allows DMI for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
			
			tlm::tlm_dmi *dmi_data = new tlm::tlm_dmi();
			unisim::kernel::tlm2::DMIGrant dmi_grant = ahb_if->get_direct_mem_ptr(*payload, *dmi_data) ? unisim::kernel::tlm2::DMI_ALLOW : unisim::kernel::tlm2::DMI_DENY;
			
			dmi_region_cache.Insert(dmi_grant, dmi_data);
		}
		else
		{
			if(unlikely(DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "AHB Instruction Read: target does not allow DMI for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
		}
	}
	
	payload->release();
	
	return status == tlm::TLM_OK_RESPONSE;
}

bool CPU::AHBDataRead(PHYSICAL_ADDRESS physical_addr, void *buffer, uint32_t size, STORAGE_ATTR storage_attr, bool rwitm)
{
	AlignToBusClock();
	
	unisim::kernel::tlm2::DMIRegion *dmi_region = 0;
	
	if(likely(enable_dmi))
	{
		dmi_region = dmi_region_cache.Lookup(physical_addr, size);
		
		if(likely(dmi_region != 0))
		{
			if(unlikely(DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "AHB Data Read: DMI region cache hit for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
			if(likely(dmi_region->IsAllowed()))
			{
				if(unlikely(DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "AHB Data Read: DMI access allowed for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
				tlm::tlm_dmi *dmi_data = dmi_region->GetDMI();
				if(likely((dmi_data->get_granted_access() & tlm::tlm_dmi::DMI_ACCESS_READ) == tlm::tlm_dmi::DMI_ACCESS_READ))
				{
					if(unlikely(DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "AHB Data Read: using granted DMI access " << dmi_data->get_granted_access() << " for 0x" << std::hex << dmi_data->get_start_address() << "-0x" << dmi_data->get_end_address() << std::dec << EndDebugInfo;
					memcpy(buffer, dmi_data->get_dmi_ptr() + (physical_addr - dmi_data->get_start_address()), size);
					const sc_time& read_lat = dmi_region->GetReadLatency(size);
					cpu_time += read_lat;
					run_time += read_lat;
					return true;
				}
				else
				{
					if(unlikely(DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "AHB Data Read: granted DMI access does not allow direct read access for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
				}
			}
			else
			{
				if(unlikely(DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "AHB Data Read: DMI access denied for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
			}
		}
		else
		{
			if(unlikely(DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "AHB Data Read: DMI region cache miss for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
		}
	}

	if(unlikely(DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "AHB Data Read: traditional way for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
	
	tlm::tlm_generic_payload *payload = payload_fabric.allocate();
	
	payload->set_address(physical_addr);
	payload->set_command(tlm::TLM_READ_COMMAND);
	payload->set_data_length(size);
	payload->set_data_ptr((unsigned char *) buffer);
	
	ahb_if->b_transport(*payload, cpu_time);
	
	run_time = sc_time_stamp();
	run_time += cpu_time;

	tlm::tlm_response_status status = payload->get_response_status();
	
	if(likely(enable_dmi))
	{
		if(likely(!dmi_region && payload->is_dmi_allowed()))
		{
			if(unlikely(DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "AHB Data Read: target allows DMI for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;

			tlm::tlm_dmi *dmi_data = new tlm::tlm_dmi();
			unisim::kernel::tlm2::DMIGrant dmi_grant = ahb_if->get_direct_mem_ptr(*payload, *dmi_data) ? unisim::kernel::tlm2::DMI_ALLOW : unisim::kernel::tlm2::DMI_DENY;
			
			dmi_region_cache.Insert(dmi_grant, dmi_data);
		}
		else
		{
			if(unlikely(DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "AHB Data Read: target does not allow DMI for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
		}
	}

	payload->release();

	return status == tlm::TLM_OK_RESPONSE;
}

bool CPU::AHBDataWrite(PHYSICAL_ADDRESS physical_addr, const void *buffer, uint32_t size, STORAGE_ATTR storage_attr)
{
	AlignToBusClock();
	
	unisim::kernel::tlm2::DMIRegion *dmi_region = 0;

	if(likely(enable_dmi))
	{
		dmi_region = dmi_region_cache.Lookup(physical_addr, size);
		
		if(likely(dmi_region != 0))
		{
			if(unlikely(DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "AHB Data Write: DMI region cache hit for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
			if(likely(dmi_region->IsAllowed()))
			{
				if(unlikely(DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "AHB Data Write: DMI access allowed for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
				tlm::tlm_dmi *dmi_data = dmi_region->GetDMI();
				if(likely((dmi_data->get_granted_access() & tlm::tlm_dmi::DMI_ACCESS_WRITE) == tlm::tlm_dmi::DMI_ACCESS_WRITE))
				{
					if(unlikely(DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "AHB Data Write: using granted DMI access " << dmi_data->get_granted_access() << " for 0x" << std::hex << dmi_data->get_start_address() << "-0x" << dmi_data->get_end_address() << std::dec << EndDebugInfo;
					memcpy(dmi_data->get_dmi_ptr() + (physical_addr - dmi_data->get_start_address()), buffer, size);
					const sc_time& write_lat = dmi_region->GetWriteLatency(size);
					cpu_time += write_lat;
					run_time += write_lat;
					return true;
				}
				else
				{
					if(unlikely(DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "AHB Data Write: granted DMI access does not allow direct write access for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
				}
			}
			else
			{
				if(unlikely(DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "AHB Data Write: DMI access denied for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
			}
		}
		else
		{
			if(unlikely(DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "AHB Data Write: DMI region cache miss for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
		}
	}

	if(unlikely(DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "AHB Data Write: traditional way for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
	
	tlm::tlm_generic_payload *payload = payload_fabric.allocate();
	
	payload->set_address(physical_addr);
	payload->set_command(tlm::TLM_WRITE_COMMAND);
	payload->set_data_length(size);
	payload->set_data_ptr((unsigned char *) buffer);
	
	ahb_if->b_transport(*payload, cpu_time);
	
	run_time = sc_time_stamp();
	run_time += cpu_time;

	tlm::tlm_response_status status = payload->get_response_status();

	if(likely(enable_dmi))
	{
		if(likely(!dmi_region && payload->is_dmi_allowed()))
		{
			if(unlikely(DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "AHB Data Write: target allows DMI for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
			tlm::tlm_dmi *dmi_data = new tlm::tlm_dmi();
			unisim::kernel::tlm2::DMIGrant dmi_grant = ahb_if->get_direct_mem_ptr(*payload, *dmi_data) ? unisim::kernel::tlm2::DMI_ALLOW : unisim::kernel::tlm2::DMI_DENY;
			
			dmi_region_cache.Insert(dmi_grant, dmi_data);
		}
		else
		{
			if(unlikely(DEBUG_ENABLE && debug_dmi)) Super::logger << DebugInfo << "AHB Data Write: target does not allow DMI for 0x" << std::hex << physical_addr << std::dec << EndDebugInfo;
		}
	}

	payload->release();

	return status == tlm::TLM_OK_RESPONSE;
}

} // end of namespace e200z710n3
} // end of namespace powerpc
} // end of namespace processor
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim
