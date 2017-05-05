/*
 *  Copyright (c) 2007-2017,
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

#include <unisim/component/cxx/processor/powerpc/e200z710n3/cpu.hh>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace powerpc {
namespace e200z710n3 {

CPU::CPU(const char *name, unisim::kernel::service::Object *parent)
	: unisim::kernel::service::Object(name, parent)
	, SuperCPU(name, parent)
	, SuperMSS()
	, unisim::kernel::service::Client<unisim::service::interfaces::Memory<PHYSICAL_ADDRESS> >(name, parent)
	, unisim::kernel::service::Service<unisim::service::interfaces::Disassembly<ADDRESS> >(name, parent)
	, unisim::kernel::service::Service<unisim::service::interfaces::Memory<ADDRESS> >(name, parent)
	, memory_import("memory-import", this)
	, disasm_export("disasm-export", this)
	, memory_export("memory-export", this)
	, cpuid(0x0)
	, param_cpuid("cpuid", this, cpuid, "CPU ID at reset")
	, processor_version(0x0)
	, param_processor_version("processor-version", this, processor_version, "Processor Version")
	, system_version(0x0)
	, param_system_version("system-version", this, system_version, "System Version")
	, system_information(0x0)
	, param_system_information("system-information", this, system_information, "System Information")
	, local_memory_base_addr(0)
	, param_local_memory_base_addr(0)
	, local_memory_size(0)
	, param_local_memory_size(0)
	, imem_base_addr(0x0)
	, param_imem_base_addr(0)
	, imem_size(0)
	, param_imem_size(0)
	, dmem_base_addr(0x0)
	, param_dmem_base_addr(0)
	, dmem_size(0)
	, param_dmem_size(0)
	, verbose_instruction_cache(false)
	, param_verbose_instruction_cache(0)
	, verbose_data_cache(false)
	, param_verbose_data_cache(0)
	, verbose_data_load(false)
	, param_verbose_data_load(0)
	, verbose_data_store(false)
	, param_verbose_data_store(0)
	, verbose_instruction_fetch(false)
	, param_verbose_instruction_fetch(0)
	, verbose_data_bus_read(false)
	, param_verbose_data_bus_read(0)
	, verbose_data_bus_write(false)
	, param_verbose_data_bus_write(0)
	, verbose_instruction_bus_read(false)
	, param_verbose_instruction_bus_read(0)
	, enable_auto_vectored_interrupts(false)
	, vector_offset(0x0)
	, instruction_buffer_base_addr(~ADDRESS(0))
	, instruction_buffer()
	, vle_decoder()
	, operation(0)
	, mpu(this, 0x2)
	, l1i()
	, l1d()
	, imem(0)
	, dmem(0)
	, cur_imem_base_addr(0)
	, cur_dmem_base_addr(0)
	, cur_imem_high_addr(0)
	, cur_dmem_high_addr(0)
	, msr(this)
	, srr0(this)
	, srr1(this)
	, pid0(this)
	, csrr0(this)
	, csrr1(this)
	, dear(this)
	, esr(this)
	, ivpr(this)
	, usprg0(this)
	, sprg0(this)
	, sprg1(this)
	, sprg2(this)
	, sprg3(this)
	, pir(this)
	, pvr(this)
	, dbsr(this)
	, dbcr0(this)
	, dbcr1(this)
	, dbcr2(this)
	, iac1(this)
	, iac2(this)
	, iac3(this)
	, iac4(this)
	, dac1(this)
	, dac2(this)
	, dvc1(this)
	, dvc2(this)
	, tir(this)
	, spefscr(this)
	, l1cfg0(this)
	, l1cfg1(this)
	, npidr(this)
	, dbcr3(this)
	, dbcnt(this)
	, dbcr4(this)
	, dbcr5(this)
	, iac5(this)
	, iac6(this)
	, iac7(this)
	, iac8(this)
	, mcsrr0(this)
	, mcsrr1(this)
	, mcsr(this)
	, mcar(this)
	, dsrr0(this)
	, dsrr1(this)
	, ddam(this)
	, dac3(this)
	, dac4(this)
	, dbcr7(this)
	, dbcr8(this)
	, ddear(this)
	, dvc1u(this)
	, dvc2u(this)
	, dbcr6(this)
	, edbrac0(this)
	, dmemcfg0(this)
	, imemcfg0(this)
	, l1finv1(this)
	, devent(this)
	, sir(this)
	, hid0(this)
	, hid1(this)
	, l1csr0(this)
	, l1csr1(this)
	, bucsr(this)
	, l1finv0(this)
	, svr(this)
	, dmemctl0(this)
	, dmemctl1(this)
	, imemctl0(this)
	, imemctl1(this)
	, e2ectl0(this)
	, e2eecsr0(this)
	, pmc0(this)
	, pmc1(this)
	, pmc2(this)
	, pmc3(this)
	, pmgc0(this)
	, pmlca0(this)
	, pmlca1(this)
	, pmlca2(this)
	, pmlca3(this)
	, pmlcb0(this)
	, pmlcb1(this)
	, pmlcb2(this)
	, pmlcb3(this)
	, upmc0(this, &pmc0)
	, upmc1(this, &pmc1)
	, upmc2(this, &pmc2)
	, upmc3(this, &pmc3)
	, upmgc0(this, &pmgc0)
	, upmlca0(this, &pmlca0)
	, upmlca1(this, &pmlca1)
	, upmlca2(this, &pmlca2)
	, upmlca3(this, &pmlca3)
	, upmlcb0(this, &pmlcb0)
	, upmlcb1(this, &pmlcb1)
	, upmlcb2(this, &pmlcb2)
	, upmlcb3(this, &pmlcb3)
	, tmcfg0(this)
{
	param_local_memory_base_addr = new unisim::kernel::service::Parameter<ADDRESS>("local-memory-base-addr", this, local_memory_base_addr, "local memory base address");
	param_local_memory_size = new unisim::kernel::service::Parameter<ADDRESS>("local-memory-size", this, local_memory_size, "local memory size");
	param_imem_base_addr = new unisim::kernel::service::Parameter<ADDRESS>("imem-base-addr", this, imem_base_addr, "IMEM (Local Instruction Memory) base address at reset");
	param_imem_size = new unisim::kernel::service::Parameter<ADDRESS>("imem-size", this, imem_size, "size in bytes of IMEM (Local Instruction Memory)");
	param_dmem_base_addr = new unisim::kernel::service::Parameter<ADDRESS>("dmem-base-addr", this, dmem_base_addr, "DMEM (Local Data Memory) base address at reset");
	param_dmem_size = new unisim::kernel::service::Parameter<ADDRESS>("dmem-size", this, dmem_size, "size in bytes of DMEM (Local Data Memory)");
	param_verbose_instruction_cache = new unisim::kernel::service::Parameter<bool>("verbose-instruction-cache", this, verbose_instruction_cache, "enable/disable verbosity of instruction cache");
	if(HAS_DATA_CACHE)
	{
		param_verbose_data_cache = new unisim::kernel::service::Parameter<bool>("verbose-data-cache", this, verbose_data_cache, "enable/disable verbosity of data cache");
	}
	param_verbose_data_load = new unisim::kernel::service::Parameter<bool>("verbose-data-load", this, verbose_data_load, "enable/disable verbosity of data load");
	param_verbose_data_store = new unisim::kernel::service::Parameter<bool>("verbose-data-store", this, verbose_data_store, "enable/disable verbosity of data store");
	param_verbose_instruction_fetch = new unisim::kernel::service::Parameter<bool>("verbose-instruction-fetch", this, verbose_instruction_fetch, "enable/disable verbosity of instruction fetch");
	param_verbose_data_bus_read = new unisim::kernel::service::Parameter<bool>("verbose-data-bus-read", this, verbose_data_bus_read, "enable/disable verbosity of data bus read");
	param_verbose_data_bus_write = new unisim::kernel::service::Parameter<bool>("verbose-data-bus-write", this, verbose_data_bus_write, "enable/disable verbosity of data bus write");
	param_verbose_instruction_bus_read = new unisim::kernel::service::Parameter<bool>("verbose-instruction-bus-read", this, verbose_instruction_bus_read, "enable/disable verbosity of instruction bus read");

	param_processor_version.SetMutable(false);
	param_system_version.SetMutable(false);
	param_system_information.SetMutable(false);
	param_imem_base_addr->SetMutable(false);
	param_imem_size->SetMutable(false);
	param_imem_size->SetFormat(unisim::kernel::service::VariableBase::FMT_DEC);
	param_dmem_base_addr->SetMutable(false);
	param_dmem_size->SetMutable(false);
	param_dmem_size->SetFormat(unisim::kernel::service::VariableBase::FMT_DEC);
	
	imem = new uint8_t[imem_size];
	memset(imem, 0, imem_size);
	dmem = new uint8_t[dmem_size];
	memset(dmem, 0, dmem_size);
	
	pvr.Initialize(processor_version);
	svr.Initialize(system_version);
	sir.Initialize(system_information);
	
	InstallInterrupt<SystemResetInterrupt>();
	InstallInterrupt<MachineCheckInterrupt>();
	InstallInterrupt<DataStorageInterrupt>();
	InstallInterrupt<InstructionStorageInterrupt>();
	InstallInterrupt<AlignmentInterrupt>();
	InstallInterrupt<ProgramInterrupt>();
	InstallInterrupt<EmbeddedFloatingPointDataInterrupt>();
	InstallInterrupt<EmbeddedFloatingPointRoundInterrupt>();
	InstallInterrupt<SystemCallInterrupt>();
	InstallInterrupt<CriticalInputInterrupt>();
	InstallInterrupt<ExternalInputInterrupt>();
	InstallInterrupt<PerformanceMonitorInterrupt>();
	InstallInterrupt<DebugInterrupt>();

	EnableInterrupt<SystemResetInterrupt>();
	EnableInterrupt<MachineCheckInterrupt::NMI>();
	EnableInterrupt<DataStorageInterrupt>();
	EnableInterrupt<InstructionStorageInterrupt>();
	EnableInterrupt<AlignmentInterrupt>();
	EnableInterrupt<ProgramInterrupt>();
	EnableInterrupt<EmbeddedFloatingPointDataInterrupt>();
	EnableInterrupt<EmbeddedFloatingPointRoundInterrupt>();
	EnableInterrupt<SystemCallInterrupt>();
}

CPU::~CPU()
{
	delete[] imem;
	delete[] dmem;
	
	delete param_local_memory_base_addr;
	delete param_local_memory_size;
	delete param_imem_base_addr;
	delete param_imem_size;
	delete param_dmem_base_addr;
	delete param_dmem_size;
	delete param_verbose_instruction_cache;
	if(HAS_DATA_CACHE)
	{
		delete param_verbose_data_cache;
	}
	delete param_verbose_data_load;
	delete param_verbose_data_store;
	delete param_verbose_instruction_fetch;
	delete param_verbose_data_bus_read;
	delete param_verbose_data_bus_write;
	delete param_verbose_instruction_bus_read;
}

bool CPU::EndSetup()
{
	if(!SuperCPU::EndSetup()) return false;
	
	l1csr0 = 1;
	
	return true;
}

void CPU::Reset()
{
	SuperCPU::Reset();
	msr.Initialize(0x0);
}

void CPU::ProcessInterrupt(SystemResetInterrupt *system_reset_interrupt)
{
	struct B0_29 : Field<void, 0, 29> {};
	
	Reset();
	
	esr = 0;

	msr.Set<SystemResetInterrupt::MSR_CLEARED_FIELDS>(0);
	
	// Vector: p_rstbase[0:29] || 2'b00
	cia = reset_addr & B0_29::GetMask<ADDRESS>();
	
	AckInterrupt<SystemResetInterrupt>();
}

void CPU::ProcessInterrupt(MachineCheckInterrupt *machine_check_interrupt)
{
	struct B0_23  : Field<void,  0, 23> {};
	struct B24_31 : Field<void, 24, 31> {};

	mcsrr0 = cia;
	mcsrr1 = msr;

	if(RecognizedException<MachineCheckInterrupt::NMI>()) mcsr.Set<MCSR::NMI>(1);

	if(machine_check_interrupt->GotEvent(MachineCheckInterrupt::MCE_MCP                                      )) mcsr.Set<MCSR::MCP>(1);
	if(machine_check_interrupt->GotEvent(MachineCheckInterrupt::MCE_EXCEPTION_ISI                            )) mcsr.Set<MCSR::EXCP_ERR>(1);
	if(machine_check_interrupt->GotEvent(MachineCheckInterrupt::MCE_EXCEPTION_BUS_ERROR                      )) mcsr.Set<MCSR::EXCP_ERR>(1);
	if(machine_check_interrupt->GotEvent(MachineCheckInterrupt::MCE_INSTRUCTION_CACHE_DATA_ARRAY_PARITY_ERROR)) mcsr.Set<MCSR::IC_DPERR>(1);
	if(machine_check_interrupt->GotEvent(MachineCheckInterrupt::MCE_DATA_CACHE_DATA_ARRAY_PARITY_ERROR       )) mcsr.Set<MCSR::DC_DPERR>(1);
	if(machine_check_interrupt->GotEvent(MachineCheckInterrupt::MCE_INSTRUCTION_CACHE_TAG_PARITY_ERROR       )) mcsr.Set<MCSR::IC_TPERR>(1);
	if(machine_check_interrupt->GotEvent(MachineCheckInterrupt::MCE_DATA_CACHE_TAG_PARITY_ERROR              )) mcsr.Set<MCSR::DC_TPERR>(1);
	if(machine_check_interrupt->GotEvent(MachineCheckInterrupt::MCE_INSTRUCTION_CACHE_LOCK_ERROR             )) mcsr.Set<MCSR::IC_LKERR>(1);
	if(machine_check_interrupt->GotEvent(MachineCheckInterrupt::MCE_DATA_CACHE_LOCK_ERROR                    )) mcsr.Set<MCSR::DC_LKERR>(1);
	if(machine_check_interrupt->GotEvent(MachineCheckInterrupt::MCE_INSTRUCTION_FETCH_ERROR_REPORT           )) mcsr.Set<MCSR::IF>(1);
	if(machine_check_interrupt->GotEvent(MachineCheckInterrupt::MCE_LOAD_TYPE_INSTRUCTION_ERROR_REPORT       )) mcsr.Set<MCSR::LD>(1);
	if(machine_check_interrupt->GotEvent(MachineCheckInterrupt::MCE_STORE_TYPE_INSTRUCTION_ERROR_REPORT      )) mcsr.Set<MCSR::ST>(1);
	if(machine_check_interrupt->GotEvent(MachineCheckInterrupt::MCE_GUARDED_INSTRUCTION_ERROR_REPORT         )) mcsr.Set<MCSR::G>(1);
	if(machine_check_interrupt->GotEvent(MachineCheckInterrupt::MCE_STACK_LIMIT_CHECK_FAILURE                )) mcsr.Set<MCSR::STACK_ERR>(1);
	if(machine_check_interrupt->GotEvent(MachineCheckInterrupt::MCE_INSTRUCTION_MEMORY_PARITY_ERROR          )) mcsr.Set<MCSR::IMEM_PERR>(1);
	if(machine_check_interrupt->GotEvent(MachineCheckInterrupt::MCE_DATA_MEMORY_READ_PARITY_ERROR            )) mcsr.Set<MCSR::DMEM_RDPERR>(1);
	if(machine_check_interrupt->GotEvent(MachineCheckInterrupt::MCE_DATA_MEMORY_WRITE_PARITY_ERROR           )) mcsr.Set<MCSR::DMEM_WRPERR>(1);
	if(machine_check_interrupt->GotEvent(MachineCheckInterrupt::MCE_INSTRUCTION_READ_BUS_ERROR               )) mcsr.Set<MCSR::BUS_IRERR>(1);
	if(machine_check_interrupt->GotEvent(MachineCheckInterrupt::MCE_DATA_READ_BUS_ERROR                      )) mcsr.Set<MCSR::BUS_DRERR>(1);
	if(machine_check_interrupt->GotEvent(MachineCheckInterrupt::MCE_DATA_WRITE_BUS_ERROR                     )) mcsr.Set<MCSR::BUS_WRERR>(1);
	if(machine_check_interrupt->GotEvent(MachineCheckInterrupt::MCE_DATA_WRITE_BUS_ERROR_DSI                 )) mcsr.Set<MCSR::BUS_WRDSI>(1);
	
	if(machine_check_interrupt->HasAddress())
	{
		mcsr.Set<MCSR::MAV>(1);
		mcsr.Set<MCSR::U>(msr.Get<MSR::PR>());
		mcsr.Set<MCSR::MEA>(0); // 1=effective address, 0=physical address. Note: there is no MMU in this CPU.
		mcar = machine_check_interrupt->GetAddress();
		machine_check_interrupt->ClearAddress();
	}
	else
	{
		mcsr.Set<MCSR::MAV>(0);
	}

	msr.Set<MachineCheckInterrupt::MSR_ALWAYS_CLEARED_FIELDS>(0);
	if(hid0.Get<HID0::MCCLRDE>()) msr.Set<MSR::DE>(0);
	
	// Vector: IVPR[0:23] || 0x10
	cia = (ivpr & B0_23::GetMask<ADDRESS>()) | (MachineCheckInterrupt::OFFSET & B24_31::GetMask<ADDRESS>());
	
	machine_check_interrupt->ClearEvents();
	
	AckInterrupt<MachineCheckInterrupt>();
}

void CPU::ProcessInterrupt(DataStorageInterrupt *data_storage_interrupt)
{
	struct B0_23  : Field<void,  0, 23> {};
	struct B24_31 : Field<void, 24, 31> {};

	srr0 = cia;
	srr1 = msr;
	
	// ESR: [ST], [SPV], VLEMI. All other bits cleared
	esr = 0;
	operation->update_esr_st(this);
	operation->update_esr_spv(this);
	esr.Set<ESR::VLEMI>(1);
	
	assert(data_storage_interrupt->HasAddress());
	dear = data_storage_interrupt->GetAddress();
	
	msr.Set<DataStorageInterrupt::MSR_CLEARED_FIELDS>(0);
	
	// Vector: IVPR[0:23] || 0x20
	cia = (ivpr & B0_23::GetMask<ADDRESS>()) | (DataStorageInterrupt::OFFSET & B24_31::GetMask<ADDRESS>());

	AckInterrupt<DataStorageInterrupt>();
	
	data_storage_interrupt->ClearAddress();
}

void CPU::ProcessInterrupt(InstructionStorageInterrupt *instruction_storage_interrupt)
{
	struct B0_23  : Field<void,  0, 23> {};
	struct B24_31 : Field<void, 24, 31> {};

	srr0 = cia;
	srr1 = msr;
	
	// ESR: VLEMI. All other bits cleared
	esr = 0;
	esr.Set<ESR::VLEMI>(1);
	
	msr.Set<InstructionStorageInterrupt::MSR_CLEARED_FIELDS>(0);

	// Vector: IVPR[0:23] || 0x30
	cia = (ivpr & B0_23::GetMask<ADDRESS>()) | (InstructionStorageInterrupt::OFFSET & B24_31::GetMask<ADDRESS>());

	AckInterrupt<InstructionStorageInterrupt>();
}

void CPU::ProcessInterrupt(AlignmentInterrupt *alignment_interrupt)
{
	struct B0_23  : Field<void,  0, 23> {};
	struct B24_31 : Field<void, 24, 31> {};

	srr0 = cia;
	srr1 = msr;
	
	// ESR: [ST], VLEMI. All other bits cleared
	esr = 0;
	operation->update_esr_st(this);
	esr.Set<ESR::VLEMI>(1);
	
	assert(alignment_interrupt->HasAddress());
	dear = alignment_interrupt->GetAddress();
	
	msr.Set<AlignmentInterrupt::MSR_CLEARED_FIELDS>(0);
	
	// Vector: IVPR[0:23] || 0x50
	cia = (ivpr & B0_23::GetMask<ADDRESS>()) | (AlignmentInterrupt::OFFSET & B24_31::GetMask<ADDRESS>());

	AckInterrupt<AlignmentInterrupt>();
	
	alignment_interrupt->ClearAddress();
}

void CPU::ProcessInterrupt(ProgramInterrupt *program_interrupt)
{
	struct B0_23  : Field<void,  0, 23> {};
	struct B24_31 : Field<void, 24, 31> {};

	srr0 = cia;
	srr1 = msr;
	
	// ESR: VLEMI. All other bits cleared
	esr = 0;
	esr.Set<ESR::VLEMI>(1);
	if(RecognizedException<ProgramInterrupt::IllegalInstruction>() || RecognizedException<ProgramInterrupt::UnimplementedInstruction>())
	{
		esr.Set<ESR::PIL>(1);
	}
	else if(RecognizedException<ProgramInterrupt::PrivilegeViolation>())
	{
		esr.Set<ESR::PPR>(1);
	}
	else if(RecognizedException<ProgramInterrupt::Trap>())
	{
		esr.Set<ESR::PTR>(1);
	}
	
	msr.Set<ProgramInterrupt::MSR_CLEARED_FIELDS>(0);
	
	// Vector: IVPR[0:23] || 0x60
	cia = (ivpr & B0_23::GetMask<ADDRESS>()) | (ProgramInterrupt::OFFSET & B24_31::GetMask<ADDRESS>());

	AckInterrupt<ProgramInterrupt>();
}

void CPU::ProcessInterrupt(EmbeddedFloatingPointDataInterrupt *embedded_floating_point_data_interrupt)
{
	struct B0_23  : Field<void,  0, 23> {};
	struct B24_31 : Field<void, 24, 31> {};

	srr0 = cia;
	srr1 = msr;
	
	// ESR: SPV, VLEMI. All other bits cleared
	esr = 0;
	esr.Set<ESR::SPV>(1);
	esr.Set<ESR::VLEMI>(1);
	
	msr.Set<EmbeddedFloatingPointDataInterrupt::MSR_CLEARED_FIELDS>(0);
	
	// Vector: IVPR[0:23] || 0xA0
	cia = (ivpr & B0_23::GetMask<ADDRESS>()) | (EmbeddedFloatingPointDataInterrupt::OFFSET & B24_31::GetMask<ADDRESS>());
	
	AckInterrupt<EmbeddedFloatingPointDataInterrupt>();
}

void CPU::ProcessInterrupt(EmbeddedFloatingPointRoundInterrupt *embedded_floating_point_round_interrupt)
{
	struct B0_23  : Field<void,  0, 23> {};
	struct B24_31 : Field<void, 24, 31> {};

	srr0 = cia;
	srr1 = msr;
	
	// ESR: SPV, VLEMI. All other bits cleared
	esr = 0;
	esr.Set<ESR::SPV>(1);
	esr.Set<ESR::VLEMI>(1);
	
	msr.Set<EmbeddedFloatingPointDataInterrupt::MSR_CLEARED_FIELDS>(0);
	
	// Vector: IVPR[0:23] || 0xB0
	cia = (ivpr & B0_23::GetMask<ADDRESS>()) | (EmbeddedFloatingPointRoundInterrupt::OFFSET & B24_31::GetMask<ADDRESS>());
	
	AckInterrupt<EmbeddedFloatingPointRoundInterrupt>();
}

void CPU::ProcessInterrupt(SystemCallInterrupt *system_call_interrupt)
{
	struct B0_23  : Field<void,  0, 23> {};
	struct B24_31 : Field<void, 24, 31> {};

	srr0 = nia;
	srr1 = msr;
	
	// ESR: VLEMI. All other bits cleared
	esr = 0;
	esr.Set<ESR::VLEMI>(1);
	
	msr.Set<SystemCallInterrupt::MSR_CLEARED_FIELDS>(0);
	
	// Vector: IVPR[0:23] || 0xB0
	cia = (ivpr & B0_23::GetMask<ADDRESS>()) | (SystemCallInterrupt::OFFSET & B24_31::GetMask<ADDRESS>());
	
	AckInterrupt<SystemCallInterrupt>();
	
	system_call_interrupt->ClearELEV();
}

void CPU::ProcessInterrupt(CriticalInputInterrupt *critical_input_interrupt)
{
	struct B0_23  : Field<void,  0, 23> {};
	struct B0_15  : Field<void,  0, 15> {};
	struct B16_29 : Field<void, 16, 29> {};
	struct B24_31 : Field<void, 24, 31> {};
	
	csrr0 = cia;
	csrr1 = msr;
	
	msr.Set<CriticalInputInterrupt::MSR_ALWAYS_CLEARED_FIELDS>(0);
	if(hid0.Get<HID0::CICLRDE>()) msr.Set<MSR::DE>(0);
	
	if(enable_auto_vectored_interrupts)
	{
		// Vector: IVPR[0:23] || 0x00 (autovectored)
		cia = (ivpr & B0_23::GetMask<ADDRESS>()) | (CriticalInputInterrupt::OFFSET & B24_31::GetMask<ADDRESS>());
	}
	else
	{
		// Vector: IVPR[0:15] || (IVPR[16:29] | p_voffset[0:13]) || 2'b00 (non-autovectored)
		cia = (ivpr & B0_15::GetMask<ADDRESS>()) | ((ivpr | vector_offset) & B16_29::GetMask<ADDRESS>());
		
		InterruptAcknowledge();
	}
	
}

void CPU::ProcessInterrupt(ExternalInputInterrupt *external_input_interrupt)
{
	struct B0_23  : Field<void,  0, 23> {};
	struct B0_15  : Field<void,  0, 15> {};
	struct B16_29 : Field<void, 16, 29> {};
	struct B24_31 : Field<void, 24, 31> {};

	srr0 = cia;
	srr1 = msr;
	
	msr.Set<ExternalInputInterrupt::MSR_CLEARED_FIELDS>(0);
	
	if(enable_auto_vectored_interrupts)
	{
		// Vector: IVPR[0:23] || 0x00 (autovectored)
		cia = (ivpr & B0_23::GetMask<ADDRESS>()) | (ExternalInputInterrupt::OFFSET & B24_31::GetMask<ADDRESS>());
	}
	else
	{
		// Vector: IVPR[0:15] || (IVPR[16:29] | p_voffset[0:13]) || 2'b00 (non-autovectored)
		cia = (ivpr & B0_15::GetMask<ADDRESS>()) | ((ivpr | vector_offset) & B16_29::GetMask<ADDRESS>());
		
		InterruptAcknowledge();
	}
}

void CPU::ProcessInterrupt(PerformanceMonitorInterrupt *performance_monitor_interrupt)
{
	struct B0_23  : Field<void,  0, 23> {};
	struct B24_31 : Field<void, 24, 31> {};

	srr0 = cia;
	srr1 = msr;

#if 0
	if(pmgc0.Get<PMGC0::UDI>())
	{
		dsrr0 = cia;
		dsrr1 = msr;
	}
#endif
	msr.Set<PerformanceMonitorInterrupt::MSR_ALWAYS_CLEARED_FIELDS>(0);
#if 0
	if(pmgc0.Get<PMGC0::UDI>())
	{
		if(hid0.Get<HID0::DCLRCE>())
		{
			msr.Set<MSR::CE>(0);
		}
		
		if(hid0.Get<HID0::DCLREE>())
		{
			msr.Set<MSR::EE>(0);
		}
		
		msr.Set<MSR::DE>(0);
	}
#endif
	
	// Vector: IVPR[0:23] || 0x70
	cia = (ivpr & B0_23::GetMask<ADDRESS>()) | (PerformanceMonitorInterrupt::OFFSET & B24_31::GetMask<ADDRESS>());
	
	AckInterrupt<PerformanceMonitorInterrupt>();
}

void CPU::ProcessInterrupt(DebugInterrupt *debug_interrupt)
{
	struct B0_23  : Field<void,  0, 23> {};
	struct B24_31 : Field<void, 24, 31> {};

	dsrr0 = cia;
	dsrr1 = msr;
	
	if(debug_interrupt->GotEvent(DebugInterrupt::DBG_UNCONDITIONAL_DEBUG_EVENT           )) dbsr.Set<DBSR::UDE>(1);
	if(debug_interrupt->GotEvent(DebugInterrupt::DBG_INSTRUCTION_COMPLETE_DEBUG_EVENT    )) dbsr.Set<DBSR::ICMP>(1);
	if(debug_interrupt->GotEvent(DebugInterrupt::DBG_BRANCH_TAKEN_DEBUG_EVENT            )) dbsr.Set<DBSR::BRT>(1);
	if(debug_interrupt->GotEvent(DebugInterrupt::DBG_INTERRUPT_TAKEN_DEBUG_EVENT         )) dbsr.Set<DBSR::IRPT>(1);
	if(debug_interrupt->GotEvent(DebugInterrupt::DBG_CRITICAL_INTERRUPT_TAKEN_DEBUG_EVENT)) dbsr.Set<DBSR::CIRPT>(1);
	if(debug_interrupt->GotEvent(DebugInterrupt::DBG_TRAP_INSTRUCTION_DEBUG_EVENT        )) dbsr.Set<DBSR::TRAP>(1);
	if(debug_interrupt->GotEvent(DebugInterrupt::DBG_INSTRUCTION_ADDRESS_COMPARE1        )) dbsr.Set<DBSR::IAC1>(1);
	if(debug_interrupt->GotEvent(DebugInterrupt::DBG_INSTRUCTION_ADDRESS_COMPARE2        )) dbsr.Set<DBSR::IAC2>(1);
	if(debug_interrupt->GotEvent(DebugInterrupt::DBG_INSTRUCTION_ADDRESS_COMPARE3        )) dbsr.Set<DBSR::IAC3>(1);
	if(debug_interrupt->GotEvent(DebugInterrupt::DBG_INSTRUCTION_ADDRESS_COMPARE4        )) dbsr.Set<DBSR::IAC4_8>(1);
	if(debug_interrupt->GotEvent(DebugInterrupt::DBG_INSTRUCTION_ADDRESS_COMPARE5        )) dbsr.Set<DBSR::IAC4_8>(1);
	if(debug_interrupt->GotEvent(DebugInterrupt::DBG_INSTRUCTION_ADDRESS_COMPARE6        )) dbsr.Set<DBSR::IAC4_8>(1);
	if(debug_interrupt->GotEvent(DebugInterrupt::DBG_INSTRUCTION_ADDRESS_COMPARE7        )) dbsr.Set<DBSR::IAC4_8>(1);
	if(debug_interrupt->GotEvent(DebugInterrupt::DBG_INSTRUCTION_ADDRESS_COMPARE8        )) dbsr.Set<DBSR::IAC4_8>(1);
	if(debug_interrupt->GotEvent(DebugInterrupt::DBG_DATA_ADDRESS_COMPARE1_READ          )) dbsr.Set<DBSR::DAC1R>(1);
	if(debug_interrupt->GotEvent(DebugInterrupt::DBG_DATA_ADDRESS_COMPARE1_WRITE         )) dbsr.Set<DBSR::DAC1W>(1);
	if(debug_interrupt->GotEvent(DebugInterrupt::DBG_DATA_ADDRESS_COMPARE2_READ          )) dbsr.Set<DBSR::DAC2R>(1);
	if(debug_interrupt->GotEvent(DebugInterrupt::DBG_DATA_ADDRESS_COMPARE2_WRITE         )) dbsr.Set<DBSR::DAC2W>(1);
	if(debug_interrupt->GotEvent(DebugInterrupt::DBG_DEBUG_NOTIFY_INTERRUPT              )) dbsr.Set<DBSR::DNI>(1);
	if(debug_interrupt->GotEvent(DebugInterrupt::DBG_RETURN_DEBUG_EVENT                  )) dbsr.Set<DBSR::RET>(1);
	if(debug_interrupt->GotEvent(DebugInterrupt::DBG_CRITICAL_RETURN_DEBUG_EVENT         )) dbsr.Set<DBSR::CRET>(1);
	if(debug_interrupt->GotEvent(DebugInterrupt::DBG_EXTERNAL_DEBUG_EVENT1               )) dbsr.Set<DBSR::DEVT1>(1);
	if(debug_interrupt->GotEvent(DebugInterrupt::DBG_EXTERNAL_DEBUG_EVENT2               )) dbsr.Set<DBSR::DEVT2>(1);
	if(debug_interrupt->GotEvent(DebugInterrupt::DBG_PERFORMANCE_MONITOR_DEBUG_EVENT     )) dbsr.Set<DBSR::PMI>(1);
	if(debug_interrupt->GotEvent(DebugInterrupt::DBG_MPU_DEBUG_EVENT                     )) dbsr.Set<DBSR::MPU>(1);
	if(debug_interrupt->GotEvent(DebugInterrupt::DBG_IMPRECISE_DEBUG_EVENT               )) dbsr.Set<DBSR::IDE>(1);
	
	msr.Set<DebugInterrupt::MSR_ALWAYS_CLEARED_FIELDS>(0);
	
	if(hid0.Get<HID0::DCLRCE>())
	{
		msr.Set<MSR::CE>(0);
	}
	
	if(hid0.Get<HID0::DCLREE>())
	{
		msr.Set<MSR::EE>(0);
	}
	
	// Vector: IVPR[0:23] || 0xB0
	cia = (ivpr & B0_23::GetMask<ADDRESS>()) | (DebugInterrupt::OFFSET & B24_31::GetMask<ADDRESS>());
	
	debug_interrupt->ClearEvents();
	
	AckInterrupt<DebugInterrupt>();
}

void CPU::UpdateExceptionEnable()
{
	if(msr.Get<MSR::CE>())
	{
		EnableInterrupt<CriticalInputInterrupt>();
	}
	else
	{
		DisableInterrupt<CriticalInputInterrupt>();
	}
	
	if(msr.Get<MSR::EE>())
	{
		EnableInterrupt<ExternalInputInterrupt>();
		EnableInterrupt<PerformanceMonitorInterrupt>();
	}
	else
	{
		DisableInterrupt<ExternalInputInterrupt>();
		DisableInterrupt<PerformanceMonitorInterrupt>();
	}
	
	if(msr.Get<MSR::ME>())
	{
		EnableInterrupt<MachineCheckInterrupt::AsynchronousMachineCheck>();
	}
	else
	{
		DisableInterrupt<MachineCheckInterrupt::AsynchronousMachineCheck>();
	}
	
	if(msr.Get<MSR::DE>())
	{
		EnableInterrupt<DebugInterrupt>();
	}
	else
	{
		DisableInterrupt<DebugInterrupt>();
	}
}

bool CPU::DataBusRead(PHYSICAL_ADDRESS addr, void *buffer, unsigned int size, STORAGE_ATTR storage_attr, bool rwitm)
{
	if(unlikely(!AHBDataRead(addr, buffer, size, storage_attr, rwitm)))
	{
		if(verbose_exception)
		{
			logger << DebugInfo << "Data Read Bus Error at @0x" << std::hex << addr << std::dec << EndDebugInfo;
		}
		ThrowException<MachineCheckInterrupt::AsynchronousMachineCheck>().SetEvent(MachineCheckInterrupt::MCE_DATA_READ_BUS_ERROR);
		return false;
	}
	
	return true;
}

bool CPU::DataBusWrite(PHYSICAL_ADDRESS addr, const void *buffer, unsigned int size, STORAGE_ATTR storage_attr)
{
	if(unlikely(!AHBDataWrite(addr, buffer, size, storage_attr)))
	{
		if(verbose_exception)
		{
			logger << DebugInfo << "Data Write Bus Error at @0x" << std::hex << addr << std::dec << EndDebugInfo;
		}
		ThrowException<MachineCheckInterrupt::AsynchronousMachineCheck>().SetEvent(MachineCheckInterrupt::MCE_INSTRUCTION_READ_BUS_ERROR);
		return false;
	}
	
	return true;
}

bool CPU::InstructionBusRead(PHYSICAL_ADDRESS addr, void *buffer, unsigned int size, STORAGE_ATTR storage_attr)
{
	if(unlikely(!AHBInsnRead(addr, buffer, size, storage_attr)))
	{
		if(verbose_exception)
		{
			logger << DebugInfo << "Instruction Read Bus Error at @0x" << std::hex << addr << std::dec << EndDebugInfo;
		}
		ThrowException<MachineCheckInterrupt::AsynchronousMachineCheck>().SetEvent(MachineCheckInterrupt::MCE_INSTRUCTION_READ_BUS_ERROR);
		return false;
	}
	
	return true;
}

bool CPU::DebugDataBusRead(PHYSICAL_ADDRESS addr, void *buffer, unsigned int size, STORAGE_ATTR storage_attr)
{
	return AHBDebugDataRead(addr, buffer, size, storage_attr);
}

bool CPU::DebugDataBusWrite(PHYSICAL_ADDRESS addr, const void *buffer, unsigned int size, STORAGE_ATTR storage_attr)
{
	return AHBDebugDataWrite(addr, buffer, size, storage_attr);
}

bool CPU::DebugInstructionBusRead(PHYSICAL_ADDRESS addr, void *buffer, unsigned int size, STORAGE_ATTR storage_attr)
{
	return AHBDebugInsnRead(addr, buffer, size, storage_attr);
}

bool CPU::AHBInsnRead(PHYSICAL_ADDRESS physical_addr, void *buffer, uint32_t size, STORAGE_ATTR storage_attr)
{
	return false;
}

bool CPU::AHBDataRead(PHYSICAL_ADDRESS physical_addr, void *buffer, uint32_t size, STORAGE_ATTR storage_attr, bool rwitm)
{
	return false;
}

bool CPU::AHBDataWrite(PHYSICAL_ADDRESS physical_addr, const void *buffer, uint32_t size, STORAGE_ATTR storage_attr)
{
	return false;
}

bool CPU::AHBDebugInsnRead(PHYSICAL_ADDRESS physical_addr, void *buffer, uint32_t size, STORAGE_ATTR storage_attr)
{
	return memory_import->ReadMemory(physical_addr, buffer, size);
}

bool CPU::AHBDebugDataRead(PHYSICAL_ADDRESS physical_addr, void *buffer, uint32_t size, STORAGE_ATTR storage_attr)
{
	return memory_import->ReadMemory(physical_addr, buffer, size);
}

bool CPU::AHBDebugDataWrite(PHYSICAL_ADDRESS physical_addr, const void *buffer, uint32_t size, STORAGE_ATTR storage_attr)
{
	return memory_import->WriteMemory(physical_addr, buffer, size);
}

bool CPU::DataLoad(ADDRESS addr, void *buffer, unsigned int size)
{
	MPU_ENTRY *mpu_entry = mpu.CheckPermissions</* exec */ false, /* write */ false>(addr);

	if(!mpu_entry)
	{
		ThrowException<DataStorageInterrupt::AccessControl>().SetAddress(addr);
		return false;
	}
	
	if((addr >= cur_dmem_base_addr) && (addr < cur_dmem_high_addr))
	{
		// DMEM
		memcpy(buffer, &dmem[addr - cur_dmem_base_addr], size);
	}
	else
	{
		// External
		STORAGE_ATTR storage_attr = STORAGE_ATTR((MAS0::I::Get(mpu_entry->mas0) ? SA_I : 0) | (MAS0::G::Get(mpu_entry->mas0) ? SA_G : 0));
		if(unlikely(!this->SuperMSS::DataLoad(addr, buffer, size, storage_attr))) return false;
	}
	
	return true;
}

bool CPU::DataStore(ADDRESS addr, void *buffer, unsigned int size)
{
	MPU_ENTRY *mpu_entry = mpu.CheckPermissions</* exec */ false, /* write */ true>(addr);

	if(!mpu_entry)
	{
		ThrowException<DataStorageInterrupt::AccessControl>().SetAddress(addr);
		return false;
	}
	
	if((addr >= cur_dmem_base_addr) && (addr < cur_dmem_high_addr))
	{
		// DMEM
		memcpy(&dmem[addr - cur_dmem_base_addr], buffer, size);
	}
	else
	{
		// External
		STORAGE_ATTR storage_attr = STORAGE_ATTR((MAS0::I::Get(mpu_entry->mas0) ? SA_I : 0) | (MAS0::G::Get(mpu_entry->mas0) ? SA_G : 0));
		if(unlikely(!this->SuperMSS::DataStore(addr, buffer, size, storage_attr))) return false;
	}
	
	return true;
}

unsigned int CPU::ExternalLoad(PHYSICAL_ADDRESS addr, void *buffer, unsigned int size)
{
	unsigned int read_bytes = 0;
	
	if(size)
	{
		do
		{
			unsigned int sz = 0;
			
			if((addr >= cur_imem_base_addr) && (addr < cur_imem_high_addr))
			{
				// IMEM
				sz = cur_imem_high_addr - addr;
				if(sz > size) sz = size;
				memcpy(buffer, &imem[addr - cur_imem_base_addr], sz);
			}
			else if((addr >= cur_dmem_base_addr) && (addr < cur_dmem_high_addr))
			{
				// DMEM
				sz = cur_dmem_high_addr - addr;
				if(sz > size) sz = size;
				memcpy(buffer, &dmem[addr - cur_dmem_base_addr], sz);
			}
			else
			{
				return read_bytes;
			}
			
			buffer = (uint8_t *) buffer + sz;
			addr += sz;
			size -= sz;
			read_bytes += sz;
		}
		while(size);
	}
	
	return read_bytes;
}

unsigned int CPU::ExternalStore(PHYSICAL_ADDRESS addr, void *buffer, unsigned int size)
{
	unsigned int written_bytes = 0;
	
	if(size)
	{
		do
		{
			unsigned int sz = 0;
			
			if((addr >= cur_imem_base_addr) && (addr < cur_imem_high_addr))
			{
				// IMEM
				sz = cur_imem_high_addr - addr;
				if(sz > size) sz = size;
				memcpy(&imem[addr - cur_imem_base_addr], buffer, sz);
			}
			else if((addr >= cur_dmem_base_addr) && (addr < cur_dmem_high_addr))
			{
				// DMEM
				sz = cur_dmem_high_addr - addr;
				if(sz > size) sz = size;
				memcpy(&dmem[addr - cur_dmem_base_addr], buffer, sz);
			}
			else
			{
				return written_bytes;
			}
			
			buffer = (uint8_t *) buffer + sz;
			addr += sz;
			size -= sz;
			written_bytes += sz;
		}
		while(size);
	}
	
	return written_bytes;
}

unsigned char *CPU::GetDirectMemPtr(PHYSICAL_ADDRESS addr, PHYSICAL_ADDRESS& start_addr, PHYSICAL_ADDRESS& end_addr)
{
	if((addr >= cur_imem_base_addr) && (addr < cur_imem_high_addr))
	{
		// IMEM
		start_addr = cur_imem_base_addr;
		end_addr = cur_imem_high_addr;
		return imem;
	}
	else if((addr >= cur_dmem_base_addr) && (addr < cur_dmem_high_addr))
	{
		// DMEM
		start_addr = cur_dmem_base_addr;
		end_addr = cur_dmem_high_addr;
		return dmem;
	}
	return 0;
}

std::string CPU::Disasm(ADDRESS addr, ADDRESS& next_addr)
{
	std::stringstream sstr;
	unisim::component::cxx::processor::powerpc::e200z710n3::isa::vle::Operation *operation;
	uint32_t insn;

	if(!DebugInstructionFetch(addr, &insn, 4))
	{
		next_addr = addr + 2;
		return std::string("unreadable ?");
	}
	
#if BYTE_ORDER == LITTLE_ENDIAN
	unisim::util::endian::BSwap(insn);
#endif

	operation = vle_decoder.Decode(addr, insn);
	
	// disassemble the instruction
	sstr << "0x" << std::hex;
	sstr.fill('0');
	switch(operation->GetLength())
	{
		case 0:
		{
			sstr.width(8); 
			sstr << insn << std::dec << " ";
			operation->disasm(this, sstr);

			next_addr = addr + 4;
			break;
		}
			
		case 16:
		{
			sstr.width(4); 
			sstr << (operation->GetEncoding() >> 16) << std::dec << " ";
			operation->disasm(this, sstr);

			next_addr = addr + 2;
			break;
		}
		
		case 32:
		{
			sstr.width(8); 
			sstr << operation->GetEncoding() << std::dec << " ";
			operation->disasm(this, sstr);

			next_addr = addr + 4;
			break;
		}
		
		default:
			throw std::runtime_error("Internal Error! Bad operation length from VLE decoder");
	}
	
	return sstr.str();
}

bool CPU::DebugDataLoad(ADDRESS addr, void *buffer, unsigned int size)
{
	bool status = true;
	
	if(size)
	{
		do
		{
			unsigned int sz = 0;
			
			if((addr >= cur_dmem_base_addr) && (addr < cur_dmem_high_addr))
			{
				// DMEM
				sz = cur_dmem_high_addr - addr;
				if(sz > size) sz = size;
				memcpy(buffer, &dmem[addr - cur_dmem_base_addr], sz);
			}
			else
			{
				// External
				MPU_ENTRY *mpu_entry = mpu.CheckPermissions</* exec */ false, /* write */ false>(addr);
				
				STORAGE_ATTR storage_attr = mpu_entry ? STORAGE_ATTR((MAS0::I::Get(mpu_entry->mas0) ? SA_I : 0) | (MAS0::G::Get(mpu_entry->mas0) ? SA_G : 0)) : SA_DEFAULT;
					
				sz = HAS_DATA_CACHE ? (L1D::BLOCK_SIZE - (addr % L1D::BLOCK_SIZE)) : size;
				if(sz > size) sz = size;
				if(!this->SuperMSS::DebugDataLoad(addr, buffer, sz, storage_attr))
				{
					status = false;
					memset(buffer, 0, sz);
				}
			}
			
			buffer = (uint8_t *) buffer + sz;
			addr += sz;
			size -= sz;
		}
		while(size);
	}
	
	return status;
}

bool CPU::DebugDataStore(ADDRESS addr, const void *buffer, unsigned int size)
{
	bool status = true;

	if(size)
	{
		do
		{
			unsigned int sz = 0;
			
			if((addr >= cur_dmem_base_addr) && (addr < cur_dmem_high_addr))
			{
				// DMEM
				sz = cur_dmem_high_addr - addr;
				if(sz > size) sz = size;
				memcpy(&dmem[addr - cur_dmem_base_addr], buffer, sz);
			}
			else
			{
				// External
				MPU_ENTRY *mpu_entry = mpu.CheckPermissions</* exec */ false, /* write */ true>(addr);
				
				STORAGE_ATTR storage_attr = mpu_entry ? STORAGE_ATTR((MAS0::I::Get(mpu_entry->mas0) ? SA_I : 0) | (MAS0::G::Get(mpu_entry->mas0) ? SA_G : 0)) : SA_DEFAULT;

				sz = HAS_DATA_CACHE ? (L1D::BLOCK_SIZE - (addr % L1D::BLOCK_SIZE)) : size;
				if(sz > size) sz = size;
				if(!this->SuperMSS::DebugDataStore(addr, buffer, sz, storage_attr))
				{
					status = false;
				}
			}
			
			buffer = (uint8_t *) buffer + sz;
			addr += sz;
			size -= sz;
		}
		while(size);
	}
	
	return status;
}

bool CPU::DebugInstructionFetch(ADDRESS addr, void *buffer, unsigned int size)
{
	bool status = true;

	if(size)
	{
		do
		{
			unsigned int sz = 0;
			
			if((addr >= cur_imem_base_addr) && (addr < cur_imem_high_addr))
			{
				// IMEM
				sz = cur_imem_high_addr - addr;
				if(sz > size) sz = size;
				memcpy(buffer, &imem[addr - cur_imem_base_addr], sz);
			}
			else
			{
				// External
				MPU_ENTRY *mpu_entry = mpu.CheckPermissions</* exec */ true, /* write */ false>(addr);
				
				STORAGE_ATTR storage_attr = mpu_entry ? STORAGE_ATTR((MAS0::I::Get(mpu_entry->mas0) ? SA_I : 0) | (MAS0::G::Get(mpu_entry->mas0) ? SA_G : 0)) : SA_DEFAULT;

				sz = HAS_DATA_CACHE ? (L1D::BLOCK_SIZE - (addr % L1D::BLOCK_SIZE)) : size;
				if(sz > size) sz = size;
				if(!this->SuperMSS::DebugInstructionFetch(addr, buffer, sz, storage_attr))
				{
					status = false;
					memset(buffer, 0, sz);
				}
			}
			
			buffer = (uint8_t *) buffer + sz;
			addr += sz;
			size -= sz;
		}
		while(size);
	}
	
	return status;
}


bool CPU::ReadMemory(ADDRESS addr, void *buffer, uint32_t size)
{
	return DebugDataLoad(addr, buffer, size);
}

bool CPU::WriteMemory(ADDRESS addr, const void *buffer, uint32_t size)
{
	return DebugDataStore(addr, buffer, size);
}

void CPU::InvalidateDataCache()
{
	if(!HAS_DATA_CACHE) return;

	if(verbose_data_cache)
	{
		logger << DebugInfo << "Invalidating Data Cache" << EndDebugInfo;
	}
	InvalidateAll<L1D>();
}

void CPU::InvalidateDataCacheBySetAndWay(unsigned int index, unsigned int way)
{
	if(!HAS_DATA_CACHE) return;

	if(verbose_data_cache)
	{
		logger << DebugInfo << "Invalidating Data Cache by Set #" << index << " and Way #" << way << EndDebugInfo;
	}
	InvalidateLineBySetAndWay<L1D>(index, way);
}

void CPU::ClearDataCacheLockoutBySetAndWay(unsigned int index, unsigned int way)
{
	if(!HAS_DATA_CACHE) return;
	
	if(verbose_data_cache)
	{
		logger << DebugInfo << "Clearing Data Cache Lockout by Set #" << index << " and Way #" << way << EndDebugInfo;
	}

	assert(way < L1D::ASSOCIATIVITY);
	unisim::util::cache::CacheSet<MSS_TYPES, L1D>& set = l1d[index];
	
	unsigned int& lockout = set.Status<L1D::SET_STATUS>().lockout;
	lockout = lockout & ~((1 << (L1I::ASSOCIATIVITY - 1)) >> way);
}

void CPU::InvalidateInstructionCache()
{
	if(verbose_instruction_cache)
	{
		logger << DebugInfo << "Invalidating Instruction Cache" << EndDebugInfo;
	}
	InvalidateAll<L1I>();
}

void CPU::InvalidateInstructionCacheBySetAndWay(unsigned int index, unsigned int way)
{
	if(verbose_data_cache)
	{
		logger << DebugInfo << "Invalidating Instruction Cache by Set #" << index << " and Way #" << way << EndDebugInfo;
	}
	InvalidateLineBySetAndWay<L1I>(index, way);
}

void CPU::ClearInstructionCacheLockoutBySetAndWay(unsigned int index, unsigned int way)
{
	if(verbose_data_cache)
	{
		logger << DebugInfo << "Clearing Instruction Cache Lockout by Set #" << index << " and Way #" << way << EndDebugInfo;
	}
	
	assert(way < L1I::ASSOCIATIVITY);
	unisim::util::cache::CacheSet<MSS_TYPES, L1I>& set = l1i[index];
	
	unsigned int& lockout = set.Status<L1I::SET_STATUS>().lockout;
	lockout = lockout & ~((1 << (L1I::ASSOCIATIVITY - 1)) >> way);
}

bool CPU::Dcba(ADDRESS addr)
{
	// dcba is treated as no-op on e200z
	return true;
}

bool CPU::Dcbi(ADDRESS addr)
{
	if(msr.Get<MSR::PR>())
	{
		ThrowException<ProgramInterrupt::PrivilegeViolation>();
		return false;
	}
	
	if(!IsCacheEnabled(&l1d)) return true; // dcbi is treated as a no-op in supervisor mode if the data cache is disabled
	
	MPU_ENTRY *mpu_entry = mpu.CheckPermissions</* exec */ false, /* write */ true>(addr); // dcbi is treated as a store for the purpose of access protection
	
	if(!mpu_entry)
	{
		ThrowException<DataStorageInterrupt::AccessControl>().SetAddress(addr);
		return false;
	}
	
	InvalidateLineByAddress<L1D>(addr);
	return true;
}

bool CPU::Dcbf(ADDRESS addr)
{
	if(!IsCacheEnabled(&l1d)) return true; // dcbf is treated as a no-op if the data cache is disabled
	
	MPU_ENTRY *mpu_entry = mpu.CheckPermissions</* exec */ false, /* write */ false>(addr); // dcbf is treated as a load for the purpose of access protection
	
	if(!mpu_entry)
	{
		ThrowException<DataStorageInterrupt::AccessControl>().SetAddress(addr);
		return false;
	}
	
	return GlobalWriteBackLineByAddress<DATA_CACHE_HIERARCHY, L1D, L1D, /* invalidate */ true>(addr);
}

bool CPU::Dcbst(ADDRESS addr)
{
	if(!IsCacheEnabled(&l1d)) return true; // dcbst is treated as a no-op if the data cache is disabled
	
	MPU_ENTRY *mpu_entry = mpu.CheckPermissions</* exec */ false, /* write */ false>(addr); // dcbst is treated as a load for the purpose of access protection
	
	if(!mpu_entry)
	{
		ThrowException<DataStorageInterrupt::AccessControl>().SetAddress(addr);
		return false;
	}

	return GlobalWriteBackLineByAddress<DATA_CACHE_HIERARCHY, L1D, L1D, /* invalidate */ false>(addr); // line is not invalidated
}

bool CPU::Dcbt(ADDRESS addr)
{
	if(hid0.Get<HID0::NOPTI>()) return true; // icbt is treated as a no-op if HID0[NOPTI]=1
	
	// TODO: data cache software prefetching
	return true;
}

bool CPU::Dcbtst(ADDRESS addr)
{
	if(hid0.Get<HID0::NOPTI>()) return true; // icbt is treated as a no-op if HID0[NOPTI]=1

	// TODO: data cache software prefetching
	return true;
}

bool CPU::Dcbz(ADDRESS addr)
{
	// dcbz with writethrough cache results in an alignment interrupt
	ThrowException<AlignmentInterrupt::WriteThroughDCBZ>().SetAddress(addr);
	return false;
}

bool CPU::Icbi(ADDRESS addr)
{
	if(msr.Get<MSR::PR>())
	{
		ThrowException<ProgramInterrupt::PrivilegeViolation>();
		return false;
	}
	
	MPU_ENTRY *mpu_entry = mpu.CheckPermissions</* exec */ false, /* write */ false>(addr); // icbi is treated as a load for the purpose of access protection
	
	if(!mpu_entry)
	{
		ThrowException<DataStorageInterrupt::AccessControl>().SetAddress(addr);
		return false;
	}

	InvalidateLineByAddress<L1I>(addr);
	return true;
}

bool CPU::Icbt(ADDRESS addr)
{
	if(hid0.Get<HID0::NOPTI>()) return true; // icbt is treated as a no-op if HID0[NOPTI]=1
	
	MPU_ENTRY *mpu_entry = mpu.CheckPermissions</* exec */ false, /* write */ false>(addr); // icbt is treated as a load for the purpose of access protection
	
	if(!mpu_entry)
	{
		ThrowException<DataStorageInterrupt::AccessControl>().SetAddress(addr);
		return false;
	}

	// TODO: instruction cache software prefetching
	return true;
}

bool CPU::Lbarx(unsigned int rd, ADDRESS addr)
{
	// TODO: reservation logic
	return Int8Load(rd, addr);
}

bool CPU::Lharx(unsigned int rd, ADDRESS addr)
{
	// TODO: reservation logic
	return Int16Load(rd, addr);
}

bool CPU::Lwarx(unsigned int rd, ADDRESS addr)
{
	// TODO: reservation logic
	return Int16Load(rd, addr);
}

bool CPU::Mbar(ADDRESS addr)
{
	ThrowException<ProgramInterrupt::UnimplementedInstruction>();
	return false;
}

bool CPU::Stbcx(unsigned int rs, ADDRESS addr)
{
	// TODO: reservation logic
	// Note: Address match with prior lbarx, lharx, or lwarx not required for store to be performed
	if(!Int8Store(rs, addr)) return false;
	// Clear CR0[LT][GT], setCR0[EQ] and copy XER[SO] to CR0[SO]
	cr.Set<CR::CR0::LT>(0);
	cr.Set<CR::CR0::GT>(0);
	cr.Set<CR::CR0::EQ>(1);
	cr.Set<CR::CR0::SO>(xer.Get<XER::SO>());
	
	return true;
}

bool CPU::Sthcx(unsigned int rs, ADDRESS addr)
{
	// TODO: reservation logic
	// Note: Address match with prior lbarx, lharx, or lwarx not required for store to be performed
	if(!Int16Store(rs, addr)) return false;
	// Clear CR0[LT][GT], setCR0[EQ] and copy XER[SO] to CR0[SO]
	cr.Set<CR::CR0::LT>(0);
	cr.Set<CR::CR0::GT>(0);
	cr.Set<CR::CR0::EQ>(1);
	cr.Set<CR::CR0::SO>(xer.Get<XER::SO>());
	
	return true;
}

bool CPU::Stwcx(unsigned int rs, ADDRESS addr)
{
	// TODO: reservation logic
	// Note: Address match with prior lbarx, lharx, or lwarx not required for store to be performed
	if(!Int32Store(rs, addr)) return false;
	// Clear CR0[LT][GT], setCR0[EQ] and copy XER[SO] to CR0[SO]
	cr.Set<CR::CR0::LT>(0);
	cr.Set<CR::CR0::GT>(0);
	cr.Set<CR::CR0::EQ>(1);
	cr.Set<CR::CR0::SO>(xer.Get<XER::SO>());
	
	return true;
}

bool CPU::Wait()
{
	Idle();
	return true;
}

bool CPU::Msync()
{
	return true;
}

bool CPU::Isync()
{
	return true;
}

bool CPU::Mpure()
{
	mpu.ReadEntry();
	return true;
}

bool CPU::Mpuwe()
{
	mpu.WriteEntry();
	return true;
}

bool CPU::Mpusync()
{
	// mpusync is treated as no-op in this architecture
	return true;
}

bool CPU::Rfi()
{
	if(msr.Get<MSR::PR>())
	{
		ThrowException<ProgramInterrupt::PrivilegeViolation>();
		return false;
	}
	
	struct B0_30 : Field<void, 0, 30> {};

	Branch(srr0 & B0_30::GetMask<ADDRESS>());
	msr = srr1;
	
	if(unlikely(verbose_interrupt))
	{
		logger << DebugInfo << "Returning from interrupt to 0x" << std::hex << nia << std::dec << EndDebugInfo;
	}

	Synchronize();
	
	return true;
}

bool CPU::Rfci()
{
	if(msr.Get<MSR::PR>())
	{
		ThrowException<ProgramInterrupt::PrivilegeViolation>();
		return false;
	}
	
	struct B0_30 : Field<void, 0, 30> {};

	Branch(csrr0 & B0_30::GetMask<ADDRESS>());
	msr = csrr1;
	
	if(unlikely(verbose_interrupt))
	{
		logger << DebugInfo << "Returning from critical interrupt to 0x" << std::hex << nia << std::dec << EndDebugInfo;
	}

	Synchronize();

	return true;
}

bool CPU::Rfdi()
{
	if(msr.Get<MSR::PR>())
	{
		ThrowException<ProgramInterrupt::PrivilegeViolation>();
		return false;
	}
	
	struct B0_30 : Field<void, 0, 30> {};

	Branch(dsrr0 & B0_30::GetMask<ADDRESS>());
	msr = dsrr1;
	
	if(unlikely(verbose_interrupt))
	{
		logger << DebugInfo << "Returning from " << DebugInterrupt::GetName() << " to 0x" << std::hex << nia << std::dec << EndDebugInfo;
	}

	Synchronize();

	return true;
}

bool CPU::Rfmci()
{
	if(msr.Get<MSR::PR>())
	{
		ThrowException<ProgramInterrupt::PrivilegeViolation>();
		return false;
	}
	
	struct B0_30 : Field<void, 0, 30> {};
	
	Branch(mcsrr0 & B0_30::GetMask<ADDRESS>());
	msr = mcsrr1;
	
	if(unlikely(verbose_interrupt))
	{
		logger << DebugInfo << "Returning from machine check interrupt to 0x" << std::hex << nia << std::dec << EndDebugInfo;
	}

	Synchronize();

	return true;
}

bool CPU::InstructionFetch(ADDRESS addr, void *buffer, unsigned int size)
{
	MPU_ENTRY *mpu_entry = mpu.CheckPermissions</* exec */ true, /* write */ false>(addr);

	if(!mpu_entry)
	{
		ThrowException<InstructionStorageInterrupt::AccessControl>();
		return false;
	}

	STORAGE_ATTR storage_attr = STORAGE_ATTR((MAS0::I::Get(mpu_entry->mas0) ? SA_I : 0) | (MAS0::G::Get(mpu_entry->mas0) ? SA_G : 0));
	
	if((addr >= cur_imem_base_addr) && (addr < cur_imem_high_addr))
	{
		// IMEM
		memcpy(buffer, imem + addr, size);
	}
	else
	{
		// External
		if(unlikely(!this->SuperMSS::InstructionFetch(addr, buffer, size, storage_attr))) return false;
	}
	
	return true;
}

void CPU::FlushInstructionBuffer()
{
	instruction_buffer_base_addr = ~ADDRESS(0);
}

bool CPU::InstructionFetch(ADDRESS addr, unisim::component::cxx::processor::powerpc::e200z710n3::isa::vle::CodeType& insn)
{
	ADDRESS base_addr = addr & ~(L1I::BLOCK_SIZE - 1);
	int instruction_buffer_index = (addr / 2) % (L1I::BLOCK_SIZE / 2);
	
	if(base_addr != instruction_buffer_base_addr)
	{
		if(!InstructionFetch(base_addr, instruction_buffer, L1I::BLOCK_SIZE)) return false;
		
		instruction_buffer_base_addr = base_addr;
	}
	
	uint16_t insn_hi = instruction_buffer[instruction_buffer_index];
#if BYTE_ORDER == LITTLE_ENDIAN
	unisim::util::endian::BSwap(insn_hi);
#endif

	if(unlikely(((unsigned int) instruction_buffer_index + 1) >= (L1I::BLOCK_SIZE / 2)))
	{
		base_addr += L1I::BLOCK_SIZE;

		if(!InstructionFetch(base_addr, instruction_buffer, L1I::BLOCK_SIZE)) return false;

		instruction_buffer_base_addr = base_addr;
		instruction_buffer_index = -1;
	}

	uint16_t insn_lo = instruction_buffer[instruction_buffer_index + 1];
#if BYTE_ORDER == LITTLE_ENDIAN
	unisim::util::endian::BSwap(insn_lo);
#endif
	
	insn = ((uint32_t) insn_hi << 16) | insn_lo;
	
	return true;
}

void CPU::StepOneInstruction()
{
	ProcessExceptions();

	if(unlikely(debug_control_import != 0))
	{
		do
		{
			unisim::service::interfaces::DebugControl<ADDRESS>::DebugCommand dbg_cmd;

			dbg_cmd = debug_control_import->FetchDebugCommand(cia);
	
			if(dbg_cmd == unisim::service::interfaces::DebugControl<ADDRESS>::DBG_STEP) break;
			if(dbg_cmd == unisim::service::interfaces::DebugControl<ADDRESS>::DBG_SYNC)
			{
				//Synchronize();
				continue;
			}

			if(dbg_cmd == unisim::service::interfaces::DebugControl<ADDRESS>::DBG_KILL) Stop(0);
			if(dbg_cmd == unisim::service::interfaces::DebugControl<ADDRESS>::DBG_RESET)
			{
			}
		} while(1);
	}

	ADDRESS addr = cia;
	uint32_t insn = 0;
	if(likely(InstructionFetch(addr, insn)))
	{
		operation = vle_decoder.Decode(addr, insn);

		nia = cia + (operation->GetLength() / 8);
		/* execute the instruction */
		if(likely(operation->execute(this)))
		{
			/* update the instruction counter */
			instruction_counter++;
			
			/* report a finished instruction */
			if(unlikely(requires_finished_instruction_reporting))
			{
				if(unlikely(memory_access_reporting_import != 0))
				{
					memory_access_reporting_import->ReportFinishedInstruction(cia, nia);
				}
			}

			/* go to the next instruction */
			cia = nia;

			if(unlikely(trap_reporting_import && (instruction_counter == trap_on_instruction_counter)))
			{
				trap_reporting_import->ReportTrap();
			}
			
			if(unlikely((instruction_counter >= max_inst) || (GetCIA() == halt_on_addr))) Stop(0);
		}
		else if(unlikely(verbose_exception))
		{
			std::stringstream sstr;
			operation->disasm(this, sstr);
			logger << DebugInfo << "Aborted instruction #" << instruction_counter << ":0x" << std::hex << addr << std::dec << ":" << sstr.str() << EndDebugInfo;
		}
	}
}

} // end of namespace e200z710n3
} // end of namespace powerpc
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim
