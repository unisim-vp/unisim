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
	, imem_base_addr(0x0)
	, param_imem_base_addr("imem-base-addr", this, imem_base_addr, "IMEM (Local Instruction Memory) base address at reset")
	, imem_size(0)
	, param_imem_size("imem-size", this, imem_size, "size in bytes of IMEM (Local Instruction Memory)")
	, dmem_base_addr(0x0)
	, param_dmem_base_addr("dmem-base-addr", this, dmem_base_addr, "DMEM (Local Data Memory) base address at reset")
	, dmem_size(0)
	, param_dmem_size("dmem-size", this, dmem_size, "size in bytes of DMEM (Local Data Memory)")
	, verbose_instruction_cache(false)
	, param_verbose_instruction_cache("verbose-instruction-cache", this, verbose_instruction_cache, "enable/disable verbosity of instruction cache")
	, verbose_data_cache(false)
	, param_verbose_data_cache("verbose-data-cache", this, verbose_data_cache, "enable/disable verbosity of data cache")
	, verbose_data_load(false)
	, param_verbose_data_load("verbose-data-load", this, verbose_data_load, "enable/disable verbosity of data load")
	, verbose_data_store(false)
	, param_verbose_data_store("verbose-data-store", this, verbose_data_store, "enable/disable verbosity of data store")
	, verbose_instruction_fetch(false)
	, param_verbose_instruction_fetch("verbose-instruction-fetch", this, verbose_instruction_fetch, "enable/disable verbosity of instruction fetch")
	, verbose_data_bus_read(false)
	, param_verbose_data_bus_read("verbose-data-bus-read", this, verbose_data_bus_read, "enable/disable verbosity of data bus read")
	, verbose_data_bus_write(false)
	, param_verbose_data_bus_write("verbose-data-bus-write", this, verbose_data_bus_write, "enable/disable verbosity of data bus write")
	, verbose_instruction_bus_read(false)
	, param_verbose_instruction_bus_read("verbose-instruction-bus-read", this, verbose_instruction_bus_read, "enable/disable verbosity of instruction bus read")
	, enable_auto_vectored_interrupts(false)
	, vector_offset(0x0)
	, instruction_buffer_base_addr(ADDRESS(-1))
	, instruction_buffer()
	, vle_decoder()
	, operation(0)
	, l1i()
	, l1d()
	, imem(0)
	, dmem(0)
	, cur_imem_base_addr(imem_base_addr)
	, cur_dmem_base_addr(dmem_base_addr)
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
	, mas0(this)
	, mas1(this)
	, mas2(this)
	, mas3(this)
	, edbrac0(this)
	, mpu0cfg(this)
	, dmemcfg0(this)
	, imemcfg0(this)
	, l1finv1(this)
	, devent(this)
	, hid0(this)
	, hid1(this)
	, l1csr0(this)
	, l1csr1(this)
	, bucsr(this)
	, mpu0csr0(this)
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
	param_imem_base_addr.SetMutable(false);
	param_imem_size.SetMutable(false);
	param_imem_size.SetFormat(unisim::kernel::service::VariableBase::FMT_DEC);
	param_dmem_base_addr.SetMutable(false);
	param_dmem_size.SetMutable(false);
	param_dmem_size.SetFormat(unisim::kernel::service::VariableBase::FMT_DEC);
	param_instruction_counter.SetFormat(unisim::kernel::service::VariableBase::FMT_DEC);
	param_trap_on_instruction_counter.SetFormat(unisim::kernel::service::VariableBase::FMT_DEC);
	param_max_inst.SetFormat(unisim::kernel::service::VariableBase::FMT_DEC);
	
	imem = new uint8_t[imem_size];
	memset(imem, 0, imem_size);
	dmem = new uint8_t[dmem_size];
	memset(dmem, 0, dmem_size);
	
	cur_imem_base_addr = imem_base_addr;
	cur_dmem_base_addr = dmem_base_addr;
}

CPU::~CPU()
{
	delete[] imem;
	delete[] dmem;
}

bool CPU::EndSetup()
{
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

	if(!halt_on.empty())
	{
		const unisim::util::debug::Symbol<ADDRESS> *halt_on_symbol = symbol_table_lookup_import ? symbol_table_lookup_import->FindSymbolByName(halt_on.c_str(), unisim::util::debug::Symbol<ADDRESS>::SYM_FUNC) : 0;
		
		if(halt_on_symbol)
		{
			halt_on_addr = halt_on_symbol->GetAddress();
			if(verbose_setup)
			{
				logger << DebugInfo << "Simulation will halt at '" << halt_on_symbol->GetName() << "' (0x" << std::hex << halt_on_addr << std::dec << ")" << EndDebugInfo;
			}
		}
		else
		{
			std::stringstream sstr(halt_on);
			sstr >> std::hex;
			if(sstr >> halt_on_addr)
			{
				if(verbose_setup)
				{
					logger << DebugInfo <<  "Simulation will halt at 0x" << std::hex << halt_on_addr << std::dec << EndDebugInfo;
				}
			}
			else
			{
				logger << DebugWarning << "Invalid address (" << halt_on << ") in Parameter " << param_halt_on.GetName() << EndDebugWarning;
				halt_on_addr = (ADDRESS) -1;
			}
		}
	}
	
	//Reset(); // TO BE REMOVED
	//cia = 0x13a0000;
	//esr.MoveTo(0xffffffffUL);
	
	//MoveToSPR(1010, 0x1 | 0x2 | 0x80000);
	//MoveToSPR(1011, 0x1 | 0x2);
	return true;
}

void CPU::Reset()
{
	SuperCPU::Reset();
	msr.Initialize(0x0);
	cur_imem_base_addr = imem_base_addr;
	cur_dmem_base_addr = dmem_base_addr;
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
	
	//esr.Set<ESR::ST>(operation->IsStore());
	//esr.Set<ESR::SPV)(operation->IsSignalProcessing());
	//esr.Set<ESR::VLEMI>(operation->IsVLEModeInstruction());
	
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
	
	esr = 0;
	//esr.Set<ESR::VLEMI>(operation->IsVLEModeInstruction());
	
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
	
	esr = 0;
	//esr.Set<ESR::ST>(operation->IsStore());
	//esr.Set<ESR::VLEMI>(operation->IsVLEModeInstruction());
	
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
	
	esr = 0;
	//esr.Set<ESR::VLEMI>(operation->IsVLEModeInstruction());
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
	
	esr = 0;
	//esr.Set<ESR::VLEMI>(operation->IsVLEModeInstruction());
	//esr.Set<ESR::SPV>(1);   // Signal Processing
	
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
	
	esr = 0;
	//esr.Set<ESR::VLEMI>(operation->IsVLEModeInstruction());
	//esr.Set<ESR::SPV>(1);   // Signal Processing
	
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
	
	esr = 0;
	//esr.Set<ESR::VLEMI>(operation->IsVLEModeInstruction());
	
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
		EnableInterrupt<CriticalInputInterrupt>();
	else
		DisableInterrupt<CriticalInputInterrupt>();
	
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

	if(msr.Get<MSR::ME>() && hid0.Get<HID0::EMCP>())
	{
		EnableInterrupt<MachineCheckInterrupt::MCP>();
	}
	else
	{
		DisableInterrupt<MachineCheckInterrupt::MCP>();
	}
	
	if(msr.Get<MSR::DE>())
		EnableInterrupt<DebugInterrupt>();
	else
		DisableInterrupt<DebugInterrupt>();
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
	// TODO: check MPU access rights
	
	ADDRESS cur_dmem_high_addr = cur_dmem_base_addr + dmem_size; 

	if((addr >= cur_dmem_base_addr) && (addr < cur_dmem_high_addr))
	{
		// DMEM
		memcpy(buffer, &dmem[addr - cur_dmem_base_addr], size);
	}
	else
	{
		// Data Cache
		if(unlikely(!this->SuperMSS::DataLoad(addr, buffer, size, 0))) return false;
	}
	
	return true;
}

bool CPU::DataStore(ADDRESS addr, void *buffer, unsigned int size)
{
	// TODO: check MPU access rights
	
	ADDRESS cur_dmem_high_addr = cur_dmem_base_addr + dmem_size; 

	if((addr >= cur_dmem_base_addr) && (addr < cur_dmem_high_addr))
	{
		// DMEM
		memcpy(&dmem[addr - cur_dmem_base_addr], buffer, size);
	}
	else
	{
		// Data Cache
		if(unlikely(!this->SuperMSS::DataStore(addr, buffer, size, 0))) return false;
	}
	
	return false;
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
			throw std::runtime_error("Internal Error !");
	}
	
	return sstr.str();
}

bool CPU::DebugDataLoad(ADDRESS addr, void *buffer, unsigned int size)
{
	bool status = true;
	
	if(size)
	{
		ADDRESS cur_imem_high_addr = cur_imem_base_addr + imem_size;
		ADDRESS cur_dmem_high_addr = cur_dmem_base_addr + dmem_size;

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
				// Data Cache
				sz = L1D::BLOCK_SIZE - (addr % L1D::BLOCK_SIZE);
				if(sz > size) sz = size;
				if(!this->SuperMSS::DebugDataLoad(addr, buffer, sz, 0))
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
		ADDRESS cur_imem_high_addr = cur_imem_base_addr + imem_size;
		ADDRESS cur_dmem_high_addr = cur_dmem_base_addr + dmem_size;

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
				// Data Cache
				sz = L1D::BLOCK_SIZE - (addr % L1D::BLOCK_SIZE);
				if(sz > size) sz = size;
				if(!this->SuperMSS::DebugDataStore(addr, buffer, sz, 0))
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
		ADDRESS cur_imem_high_addr = cur_imem_base_addr + imem_size;
		ADDRESS cur_dmem_high_addr = cur_dmem_base_addr + dmem_size;

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
				// Instruction Cache
				sz = L1D::BLOCK_SIZE - (addr % L1D::BLOCK_SIZE);
				if(sz > size) sz = size;
				if(!this->SuperMSS::DebugInstructionFetch(addr, buffer, sz, 0))
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
	if(verbose_data_cache)
	{
		logger << DebugInfo << "Invalidating Data Cache" << EndDebugInfo;
	}
	InvalidateAll<L1D>();
}

void CPU::InvalidateDataCacheBySetAndWay(unsigned int index, unsigned int way)
{
	if(verbose_data_cache)
	{
		logger << DebugInfo << "Invalidating Data Cache by Set #" << index << " and Way #" << way << EndDebugInfo;
	}
	InvalidateLineBySetAndWay<L1D>(index, way);
}

void CPU::ClearDataCacheLockoutBySetAndWay(unsigned int index, unsigned int way)
{
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
	
	// TODO: check access right
	InvalidateLineByAddress<L1D>(addr);
	return true;
}

bool CPU::Dcbf(ADDRESS addr)
{
	// TODO: check access right
	return GlobalWriteBackLineByAddress<DATA_CACHE_HIERARCHY, L1D, L1D, /* invalidate */ true>(addr);
}

bool CPU::Dcbst(ADDRESS addr)
{
	// TODO: check access right
	return GlobalWriteBackLineByAddress<DATA_CACHE_HIERARCHY, L1D, L1D, /* invalidate */ true>(addr);
}

bool CPU::Dcbt(ADDRESS addr)
{
	// TODO: check MPU access right
	// No-op: no architecturally visible
	return true;
}

bool CPU::Dcbtst(ADDRESS addr)
{
	// TODO: check MPU access right
	// No-op: no architecturally visible
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
	
	// TODO: check MPU access right
	InvalidateLineByAddress<L1I>(addr);
	return true;
}

bool CPU::Icbt(ADDRESS addr)
{
	// TODO: check MPU access right
	// No-op: no architecturally visible
	return true;
}

bool CPU::Lbarx(unsigned int rd, ADDRESS addr)
{
	ThrowException<ProgramInterrupt::UnimplementedInstruction>();
	return false;
}

bool CPU::Lharx(unsigned int rd, ADDRESS addr)
{
	ThrowException<ProgramInterrupt::UnimplementedInstruction>();
	return false;
}

bool CPU::Lwarx(unsigned int rd, ADDRESS addr)
{
	ThrowException<ProgramInterrupt::UnimplementedInstruction>();
	return false;
}

bool CPU::Mbar(ADDRESS addr)
{
	ThrowException<ProgramInterrupt::UnimplementedInstruction>();
	return false;
}

bool CPU::Stbcx(unsigned int rs, ADDRESS addr)
{
	ThrowException<ProgramInterrupt::UnimplementedInstruction>();
	return false;
}

bool CPU::Sthcx(unsigned int rs, ADDRESS addr)
{
	ThrowException<ProgramInterrupt::UnimplementedInstruction>();
	return false;
}

bool CPU::Stwcx(unsigned int rs, ADDRESS addr)
{
	ThrowException<ProgramInterrupt::UnimplementedInstruction>();
	return false;
}

bool CPU::Wait()
{
	ThrowException<ProgramInterrupt::UnimplementedInstruction>();
	return false;
}

bool CPU::Msync()
{
	return true;
}

bool CPU::Isync()
{
	return true;
}

bool CPU::Rfi()
{
	if(msr.Get<MSR::PR>())
	{
		ThrowException<ProgramInterrupt::PrivilegeViolation>();
		return false;
	}
	
	Branch(srr0 & 0xfffffffe);
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
	
	Branch(csrr0 & 0xfffffffe);
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
	
	Branch(dsrr0 & 0xfffffffe);
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
	
	Branch(mcsrr0 & 0xfffffffe);
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
	// TODO: Check MPU access rights
	
	ADDRESS cur_imem_high_addr = cur_imem_base_addr + imem_size; 

	if((addr >= cur_imem_base_addr) && (addr < cur_imem_high_addr))
	{
		// IMEM
		memcpy(buffer, imem + addr, size);
	}
	else
	{
		// Instruction Cache
		if(unlikely(!this->SuperMSS::InstructionFetch(addr, buffer, size, 0))) return false;
	}
	
	return true;
}

void CPU::FlushInstructionBuffer()
{
	instruction_buffer_base_addr = ADDRESS(-1);
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

		if(!this->SuperMSS::InstructionFetch(base_addr, instruction_buffer, L1I::BLOCK_SIZE, 0)) return false;

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
	
	//DL1SanityCheck();
	//IL1SanityCheck();
}

} // end of namespace e200z710n3
} // end of namespace powerpc
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim
