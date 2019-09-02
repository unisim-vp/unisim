/*
 *  Copyright (c) 2007-2019,
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

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_BOOK_E_CPU_TCC__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_BOOK_E_CPU_TCC__

#include <unisim/component/cxx/processor/powerpc/book_e/cpu.hh>
#include <unisim/component/cxx/processor/powerpc/cpu.tcc>
#include <unisim/util/simfloat/floating.tcc>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace powerpc {
namespace book_e {

template <typename TYPES, typename CONFIG>
CPU<TYPES, CONFIG>::CPU(const char *name, unisim::kernel::service::Object *parent)
	: unisim::kernel::service::Object(name, parent)
	, SuperCPU(name, parent)
	, unisim::kernel::service::Service<unisim::service::interfaces::Disassembly<EFFECTIVE_ADDRESS> >(name, parent)
	, disasm_export("disasm-export", this)
	, cpuid(0x0)
	, param_cpuid("cpuid", this, cpuid, "CPU ID at reset")
	, processor_version(0x0)
	, param_processor_version("processor-version", this, processor_version, "Processor Version")
	, trap_critical_input_interrupt(false)
	, param_trap_critical_input_interrupt("trap-critical-input-interrupt", this, trap_critical_input_interrupt, "enable/disable trap (in debugger) of critical input interrupt")
	, trap_machine_check_interrupt(false)
	, param_trap_machine_check_interrupt("trap-machine-check-interrupt", this, trap_machine_check_interrupt, "enable/disable trap (in debugger) of machine check interrupt")
	, trap_data_storage_interrupt(false)
	, param_trap_data_storage_interrupt("trap-data-storage-interrupt", this, trap_data_storage_interrupt, "enable/disable trap (in debugger) of data storage interrupt")
	, trap_instruction_storage_interrupt(false)
	, param_trap_instruction_storage_interrupt("trap-instruction-storage-interrupt", this, trap_instruction_storage_interrupt, "enable/disable trap (in debugger) of instruction storage interrupt")
	, trap_external_input_interrupt(false)
	, param_trap_external_input_interrupt("trap-external-input-interrupt", this, trap_external_input_interrupt, "enable/disable trap (in debugger) of extern input interrupt")
	, trap_alignment_interrupt(false)
	, param_trap_alignment_interrupt("trap-alignment-interrupt", this, trap_alignment_interrupt, "enable/disable trap (in debugger) of alignment interrupt")
	, trap_program_interrupt(false)
	, param_trap_program_interrupt("trap-program-interrupt", this, trap_program_interrupt, "enable/disable trap (in debugger) of program interrupt")
	, trap_floating_point_unavailable_interrupt(false)
	, param_trap_floating_point_unavailable_interrupt("trap-floating-point-unavailable-interrupt", this, trap_floating_point_unavailable_interrupt, "enable/disable trap (in debugger) of floating-point unavailable interrupt")
	, trap_system_call_interrupt(false)
	, param_trap_system_call_interrupt("trap-system-call-interrupt", this, trap_system_call_interrupt, "enable/disable trap (in debugger) of system call interrupt")
	, trap_auxiliary_processor_unavailable_interrupt(false)
	, param_trap_auxiliary_processor_unavailable_interrupt("trap-auxiliary-processor-unavailable-interrupt", this, trap_auxiliary_processor_unavailable_interrupt, "enable/disable trap (in debugger) of auxiliary processor unavailable interrupt")
	, trap_decrementer_interrupt(false)
	, param_trap_decrementer_interrupt("trap-decrementer-interrupt", this, trap_decrementer_interrupt, "enable/disable trap (in debugger) of decrementer interrupt")
	, trap_fixed_interval_timer_interrupt(false)
	, param_trap_fixed_interval_timer_interrupt("trap-fixed-interval-timer-interrupt", this, trap_fixed_interval_timer_interrupt, "enable/disable trap (in debugger) of fixed interval timer interrupt")
	, trap_watchdog_interrupt(false)
	, param_trap_watchdog_interrupt("trap_watchdog_interrupt", this, trap_watchdog_interrupt, "enable/disable trap (in debugger) of watchdog interrupt")
	, trap_data_tlb_error_interrupt(false)
	, param_trap_data_tlb_error_interrupt("trap-data-tlb-error-interrupt", this, trap_data_tlb_error_interrupt, "enable/disable trap (in debugger) of data TLB error interrupt")
	, trap_instruction_tlb_error_interrupt(false)
	, param_trap_instruction_tlb_error_interrupt("trap-instruction-tlb-error-interrupt", this, trap_instruction_tlb_error_interrupt, "enable/disable trap (in debugger) of instruction TLB error interrupt")
	, trap_debug_interrupt(false)
	, param_trap_debug_interrupt("trap-debug-interrupt", this, trap_debug_interrupt, "enable/disable trap (in debugger) of debug interrupt")
	, enable_linux_syscall_snooping(false)
	, param_enable_linux_syscall_snooping("enable-linux-syscall-snooping", this, enable_linux_syscall_snooping, "enable/disable Linux printk snooping")
	, instruction_buffer_base_addr(~EFFECTIVE_ADDRESS(0))
	, instruction_buffer()
	, decoder()
	, operation(0)
	, dcdbtrh(static_cast<typename CONFIG::CPU *>(this))
	, dcdbtrl(static_cast<typename CONFIG::CPU *>(this))
	, icdbdr(static_cast<typename CONFIG::CPU *>(this))
	, icdbtrh(static_cast<typename CONFIG::CPU *>(this))
	, icdbtrl(static_cast<typename CONFIG::CPU *>(this))
	, dac1(static_cast<typename CONFIG::CPU *>(this))
	, dac2(static_cast<typename CONFIG::CPU *>(this))
	, dbcr0(static_cast<typename CONFIG::CPU *>(this))
	, dbcr1(static_cast<typename CONFIG::CPU *>(this))
	, dbcr2(static_cast<typename CONFIG::CPU *>(this))
	, dbdr(static_cast<typename CONFIG::CPU *>(this))
	, dbsr(static_cast<typename CONFIG::CPU *>(this))
	, dvc1(static_cast<typename CONFIG::CPU *>(this))
	, dvc2(static_cast<typename CONFIG::CPU *>(this))
	, iac1(static_cast<typename CONFIG::CPU *>(this))
	, iac2(static_cast<typename CONFIG::CPU *>(this))
	, iac3(static_cast<typename CONFIG::CPU *>(this))
	, iac4(static_cast<typename CONFIG::CPU *>(this))
	, csrr0(static_cast<typename CONFIG::CPU *>(this))
	, csrr1(static_cast<typename CONFIG::CPU *>(this))
	, dear(static_cast<typename CONFIG::CPU *>(this))
	, esr(static_cast<typename CONFIG::CPU *>(this))
	, ivor0(static_cast<typename CONFIG::CPU *>(this))
	, ivor1(static_cast<typename CONFIG::CPU *>(this))
	, ivor2(static_cast<typename CONFIG::CPU *>(this))
	, ivor3(static_cast<typename CONFIG::CPU *>(this))
	, ivor4(static_cast<typename CONFIG::CPU *>(this))
	, ivor5(static_cast<typename CONFIG::CPU *>(this))
	, ivor6(static_cast<typename CONFIG::CPU *>(this))
	, ivor7(static_cast<typename CONFIG::CPU *>(this))
	, ivor8(static_cast<typename CONFIG::CPU *>(this))
	, ivor9(static_cast<typename CONFIG::CPU *>(this))
	, ivor10(static_cast<typename CONFIG::CPU *>(this))
	, ivor11(static_cast<typename CONFIG::CPU *>(this))
	, ivor12(static_cast<typename CONFIG::CPU *>(this))
	, ivor13(static_cast<typename CONFIG::CPU *>(this))
	, ivor14(static_cast<typename CONFIG::CPU *>(this))
	, ivor15(static_cast<typename CONFIG::CPU *>(this))
	, ivpr(static_cast<typename CONFIG::CPU *>(this))
	, mcsr(static_cast<typename CONFIG::CPU *>(this))
	, mcsrr0(static_cast<typename CONFIG::CPU *>(this))
	, mcsrr1(static_cast<typename CONFIG::CPU *>(this))
	, srr0(static_cast<typename CONFIG::CPU *>(this))
	, srr1(static_cast<typename CONFIG::CPU *>(this))
	, ccr0(static_cast<typename CONFIG::CPU *>(this))
	, ccr1(static_cast<typename CONFIG::CPU *>(this))
	, msr(static_cast<typename CONFIG::CPU *>(this))
	, pir(static_cast<typename CONFIG::CPU *>(this))
	, pvr(static_cast<typename CONFIG::CPU *>(this))
	, rstcfg(static_cast<typename CONFIG::CPU *>(this))
	, sprg0(static_cast<typename CONFIG::CPU *>(this))
	, sprg1(static_cast<typename CONFIG::CPU *>(this))
	, sprg2(static_cast<typename CONFIG::CPU *>(this))
	, sprg3(static_cast<typename CONFIG::CPU *>(this))
	, sprg4(static_cast<typename CONFIG::CPU *>(this))
	, sprg5(static_cast<typename CONFIG::CPU *>(this))
	, sprg6(static_cast<typename CONFIG::CPU *>(this))
	, sprg7(static_cast<typename CONFIG::CPU *>(this))
	, usprg0(static_cast<typename CONFIG::CPU *>(this))
	, mmucr(static_cast<typename CONFIG::CPU *>(this))
	, pid(static_cast<typename CONFIG::CPU *>(this))
	, dec(static_cast<typename CONFIG::CPU *>(this))
	, decar(static_cast<typename CONFIG::CPU *>(this))
	, tb(static_cast<typename CONFIG::CPU *>(this))
	, tcr(static_cast<typename CONFIG::CPU *>(this))
	, tsr(static_cast<typename CONFIG::CPU *>(this))
{
	param_processor_version.SetMutable(false);
	
	pvr.Initialize(processor_version);
	
	this->SuperCPU::template InstallInterrupt<CriticalInputInterrupt>(trap_critical_input_interrupt);
	this->SuperCPU::template InstallInterrupt<MachineCheckInterrupt>(trap_machine_check_interrupt);
	this->SuperCPU::template InstallInterrupt<DataStorageInterrupt>(trap_data_storage_interrupt);
	this->SuperCPU::template InstallInterrupt<InstructionStorageInterrupt>(trap_instruction_storage_interrupt);
	this->SuperCPU::template InstallInterrupt<ExternalInputInterrupt>(trap_external_input_interrupt);
	this->SuperCPU::template InstallInterrupt<AlignmentInterrupt>(trap_alignment_interrupt);
	this->SuperCPU::template InstallInterrupt<ProgramInterrupt>(trap_program_interrupt);
	this->SuperCPU::template InstallInterrupt<FloatingPointUnavailableInterrupt>(trap_floating_point_unavailable_interrupt);
	this->SuperCPU::template InstallInterrupt<SystemCallInterrupt>(trap_system_call_interrupt);
	this->SuperCPU::template InstallInterrupt<AuxiliaryProcessorUnavailableInterrupt>(trap_auxiliary_processor_unavailable_interrupt);
	this->SuperCPU::template InstallInterrupt<DecrementerInterrupt>(trap_decrementer_interrupt);
	this->SuperCPU::template InstallInterrupt<FixedIntervalTimerInterrupt>(trap_fixed_interval_timer_interrupt);
	this->SuperCPU::template InstallInterrupt<WatchdogTimerInterrupt>(trap_watchdog_interrupt);
	this->SuperCPU::template InstallInterrupt<DataTLBErrorInterrupt>(trap_data_tlb_error_interrupt);
	this->SuperCPU::template InstallInterrupt<InstructionTLBErrorInterrupt>(trap_instruction_tlb_error_interrupt);
	this->SuperCPU::template InstallInterrupt<DebugInterrupt>(trap_debug_interrupt);

	this->SuperCPU::template EnableInterrupt<DataStorageInterrupt>();
	this->SuperCPU::template EnableInterrupt<InstructionStorageInterrupt>();
	this->SuperCPU::template EnableInterrupt<AlignmentInterrupt>();
	this->SuperCPU::template EnableInterrupt<typename ProgramInterrupt::IllegalInstruction>();
	this->SuperCPU::template EnableInterrupt<typename ProgramInterrupt::PrivilegeViolation>();
	this->SuperCPU::template EnableInterrupt<typename ProgramInterrupt::Trap>();
	this->SuperCPU::template EnableInterrupt<typename ProgramInterrupt::AuxiliaryProcessor>();
	this->SuperCPU::template EnableInterrupt<typename ProgramInterrupt::UnimplementedInstruction>();
	this->SuperCPU::template EnableInterrupt<FloatingPointUnavailableInterrupt>();
	this->SuperCPU::template EnableInterrupt<AuxiliaryProcessorUnavailableInterrupt>();
	this->SuperCPU::template EnableInterrupt<SystemCallInterrupt>();
	this->SuperCPU::template EnableInterrupt<DataTLBErrorInterrupt>();
	this->SuperCPU::template EnableInterrupt<InstructionTLBErrorInterrupt>();
	
	this->Reset();
}

template <typename TYPES, typename CONFIG>
CPU<TYPES, CONFIG>::~CPU()
{
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::EndSetup()
{
	if(!SuperCPU::EndSetup()) return false;
	
	if(this->linux_os_import)
	{
		msr.template Set<typename MSR::FP>(1); // enable FPU
		msr.template Set<typename MSR::PR>(1); // set user privilege level
	}
	
	return true;
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::Reset()
{
	SuperCPU::Reset();
	this->cia = this->reset_addr & -4;
	msr.Initialize(0x0);
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::ProcessInterrupt(CriticalInputInterrupt *critical_input_interrupt)
{
	csrr0 = this->cia;
	csrr1 = msr;
	
	msr.template Set<typename CriticalInputInterrupt::MSR_ALWAYS_CLEARED_FIELDS>(0);
	
	this->cia = ivpr | ivor0;
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::ProcessInterrupt(MachineCheckInterrupt *machine_check_interrupt)
{
	mcsrr0 = this->cia;
	mcsrr1 = msr;
	
	msr.template Set<typename MachineCheckInterrupt::MSR_ALWAYS_CLEARED_FIELDS>(0);
	
	mcsr = 0;
	if(machine_check_interrupt->GotAsynchronousEvents())
	{
		mcsr.template Set<typename MCSR::MCS>(1);
		if(machine_check_interrupt->GotAsynchronousEvent(MachineCheckInterrupt::MCE_INSTRUCTION_PLB_ERROR)) mcsr.template Set<typename MCSR::IB>(1);
		if(machine_check_interrupt->GotAsynchronousEvent(MachineCheckInterrupt::MCE_DATA_READ_PLB_ERROR)) mcsr.template Set<typename MCSR::DRB>(1);
		if(machine_check_interrupt->GotAsynchronousEvent(MachineCheckInterrupt::MCE_DATA_WRITE_PLB_ERROR)) mcsr.template Set<typename MCSR::DWB>(1);
		if(machine_check_interrupt->GotAsynchronousEvent(MachineCheckInterrupt::MCE_TLB_PARITY_ERROR)) mcsr.template Set<typename MCSR::TLBP>(1);
		if(machine_check_interrupt->GotAsynchronousEvent(MachineCheckInterrupt::MCE_INSTRUCTION_CACHE_PARITY_ERROR)) mcsr.template Set<typename MCSR::ICP>(1);
		if(machine_check_interrupt->GotAsynchronousEvent(MachineCheckInterrupt::MCE_DATA_CACHE_SEARCH_PARITY_ERROR)) mcsr.template Set<typename MCSR::DCSP>(1);
		if(machine_check_interrupt->GotAsynchronousEvent(MachineCheckInterrupt::MCE_DATA_CACHE_FLUSH_PARITY_ERROR)) mcsr.template Set<typename MCSR::IMPE>(1);
	}

	this->cia = ivpr | ivor1;
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::ProcessInterrupt(DataStorageInterrupt *data_storage_interrupt)
{
	srr0 = this->cia;
	srr1 = msr;
	
	dear = data_storage_interrupt->GetAddress();

	esr = 0;
	operation->update_esr_fp(static_cast<typename CONFIG::CPU *>(this));
	operation->update_esr_st(static_cast<typename CONFIG::CPU *>(this));
	operation->update_esr_ap(static_cast<typename CONFIG::CPU *>(this));
	if(this->template RecognizedException<typename DataStorageInterrupt::ByteOrdering>())
	{
		esr.template Set<typename ESR::BO>(1);
	}
	else if(this->template RecognizedException<typename DataStorageInterrupt::CacheLocking>())
	{
		operation->update_esr_dlk(static_cast<typename CONFIG::CPU *>(this));
	}

	msr.template Set<typename DataStorageInterrupt::MSR_ALWAYS_CLEARED_FIELDS>(0);
	
	this->cia = ivpr | ivor2;
	
	this->template AckInterrupt<DataStorageInterrupt>();
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::ProcessInterrupt(InstructionStorageInterrupt *instruction_storage_interrupt)
{
	srr0 = this->cia;
	srr1 = msr;
	
	esr = 0;
	if(this->template RecognizedException<typename InstructionStorageInterrupt::ByteOrdering>())
	{
		esr.template Set<typename ESR::BO>(1);
	}
	
	msr.template Set<typename InstructionStorageInterrupt::MSR_ALWAYS_CLEARED_FIELDS>(0);
	
	this->cia = ivpr | ivor3;
	
	this->template AckInterrupt<InstructionStorageInterrupt>();
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::ProcessInterrupt(ExternalInputInterrupt *external_input_interrupt)
{
	srr0 = this->cia;
	srr1 = msr;
	
	msr.template Set<typename ExternalInputInterrupt::MSR_ALWAYS_CLEARED_FIELDS>(0);
	
	this->cia = ivpr | ivor4;
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::ProcessInterrupt(AlignmentInterrupt *alignment_interrupt)
{
	srr0 = this->cia;
	srr1 = msr;
	
	dear = alignment_interrupt->GetAddress();
	
	esr = 0;
	operation->update_esr_fp(static_cast<typename CONFIG::CPU *>(this));
	operation->update_esr_st(static_cast<typename CONFIG::CPU *>(this));
	operation->update_esr_ap(static_cast<typename CONFIG::CPU *>(this));
	
	msr.template Set<typename AlignmentInterrupt::MSR_ALWAYS_CLEARED_FIELDS>(0);
	
	this->cia = ivpr | ivor5;
	
	this->template AckInterrupt<AlignmentInterrupt>();
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::ProcessInterrupt(ProgramInterrupt *program_interrupt)
{
	srr0 = this->cia;
	srr1 = msr;
	
	esr = 0;
	if(this->template RecognizedException<typename ProgramInterrupt::IllegalInstruction>())
	{
		esr.template Set<typename ESR::PIL>(1);
	}
	if(this->template RecognizedException<typename ProgramInterrupt::PrivilegeViolation>())
	{
		esr.template Set<typename ESR::PPR>(1);
	}
	if(this->template RecognizedException<typename ProgramInterrupt::Trap>())
	{
		this->instruction_counter++;  // account for the trap instruction
		esr.template Set<typename ESR::PTR>(1);
	}
	if(this->template RecognizedException<typename ProgramInterrupt::FloatingPoint>())
	{
		esr.template Set<typename ESR::FP>(1);
	}
	if(this->template RecognizedException<typename ProgramInterrupt::AuxiliaryProcessor>())
	{
		esr.template Set<typename ESR::AP>(1);
	}
	if(this->template RecognizedException<typename ProgramInterrupt::UnimplementedInstruction>())
	{
		esr.template Set<typename ESR::PUO>(1);
	}
	
	msr.template Set<typename ProgramInterrupt::MSR_ALWAYS_CLEARED_FIELDS>(0);
	
	this->cia = ivpr | ivor6;
	
	this->template AckInterrupt<ProgramInterrupt>();
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::ProcessInterrupt(FloatingPointUnavailableInterrupt *floating_point_unavailable_interrupt)
{
	srr0 = this->cia;
	srr1 = msr;
	
	msr.template Set<typename FloatingPointUnavailableInterrupt::MSR_ALWAYS_CLEARED_FIELDS>(0);
	
	this->cia = ivpr | ivor7;
	
	this->template AckInterrupt<FloatingPointUnavailableInterrupt>();
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::ProcessInterrupt(SystemCallInterrupt *system_call_interrupt)
{
	if(unlikely(enable_linux_syscall_snooping))
	{
		std::stringstream sstr;
		
		switch(this->gpr[0])
		{
			case 1:
				sstr << "exit(0x" << std::hex << this->gpr[3] << std::dec << ");";
				break;
				
			case 2:
				sstr << "fork()";
				break;
		
			case 3:
				sstr << "read(fd=" << (signed) this->gpr[3] << ",buf=@0x" << std::hex << this->gpr[4] << std::dec
						<< ",count=" << this->gpr[5] << ");";
				break;
		
			case 4:
				{
					int fd; 
	
					fd = this->gpr[3];
					
					sstr << "write(fd=" << (signed) this->gpr[3] << ",buf=@0x" << std::hex << this->gpr[4] << std::dec;
					if((fd == 1) || (fd == 2))
					{
						std::string str = ReadString(this->gpr[4], this->gpr[5]);
						sstr << " " << str;
					}
					
					sstr << ",count=" << this->gpr[5] << ");";
				}
				break;
				
			case 5:
				sstr << "open(pathname=" << ReadString(this->gpr[3]) << ",flags=0x" << std::hex << this->gpr[4] 
							<< ",mode=0x" << this->gpr[5] << std::dec << ");";
				break;
			
			case 6:
				sstr << "close(fd=" << (signed) this->gpr[3] << ");";
				break;
				
			case 7:
				sstr << "waitpid(pid=" << (int32_t) this->gpr[3] << ",status=@0x" << std::hex << this->gpr[4] << std::dec << ",options=0x" << std::hex << this->gpr[5] << std::dec << ");";
				break;
				
			case 8:
				sstr << "creat(filename=" << ReadString(this->gpr[3]) << ",mode=0x" << std::hex << this->gpr[4] << std::dec << ");";
				break;
				
			case 9:
				sstr << "link(old_path=" << ReadString(this->gpr[3]) << ",new_path=" << ReadString(this->gpr[4]) << ");";
				break;
				
			case 10:
				sstr << "unlink(path=" << ReadString(this->gpr[3]) << ");";
				break;

			case 11:
				sstr << "execve(filename=" << ReadString(this->gpr[3]) << ",argv=@0x" << std::hex << this->gpr[4] << ",envp=@0x" << this->gpr[5] << std::dec << ");";
				break;
				
			case 12:
				sstr << "chdir(path=" << ReadString(this->gpr[3]) << ");";
				break;
				
			case 13:
				sstr << "time(t=@0x" << std::hex << this->gpr[3] << std::dec << ");";
				break;

			case 14:
				sstr << "mknod(pathname=" << ReadString(this->gpr[3]) << ",mode=0x" << std::hex << this->gpr[4] << ",dev=0x" << this->gpr[5] << std::dec << ");";
				break;
				
			case 15:
				sstr << "chmod(pathname=" << ReadString(this->gpr[3]) << ",mode=0x" << std::hex << this->gpr[4] << std::dec << ");";
				break;
			
			case 16:
				sstr << "lchown(path=" << ReadString(this->gpr[3]) << ",owner=" << this->gpr[4] << ",group=" << this->gpr[5] << ");";
				break;
				
			case 18:
				sstr << "oldstat(path=" << ReadString(this->gpr[3]) << ",buf=@0x" << std::hex << this->gpr[4] << std::dec << ");";
				break;
				
			case 19:
				sstr << "lseek(fd=" << (signed) this->gpr[3] << ",offset=" << (signed) this->gpr[4] << ",whence=" << (signed) this->gpr[5] << ");";
				break;
				
			case 20:
				sstr << "getpid();";
				break;
				
			case 21:
				sstr << "mount(source=" << ReadString(this->gpr[3]) << ",target=" << ReadString(this->gpr[4]) << ",filesystemtype="
						<< ReadString(this->gpr[5]) << ",mountflags=0x" << std::hex << this->gpr[6] << ",data=@0x"
						<< this->gpr[7] << std::dec << ");";
				break;
			
			case 22:
				sstr << "umount(target=" << ReadString(this->gpr[4]) << ");";
				break;
				
			case 23:
				sstr << "setuid(uid=" << this->gpr[3] << ");";
				break;
				
			case 24:
				sstr << "getuid();";
				break;
				
			case 25:
				sstr << "stime(t=@" << std::hex << this->gpr[3] << std::dec << ");";
				break;
			
			case 26:
				sstr << "ptrace(request=" << this->gpr[3] << ",pid=" << this->gpr[4] << ",addr=@" << std::hex << this->gpr[5] << std::dec << ",data=@" << std::hex << this->gpr[6] << std::dec << ");";
				break;

			case 27:
				sstr << "alarm(seconds=" << this->gpr[3] << ");";
				break;

			case 28:
				sstr << "oldfstat(fd=" << (signed) this->gpr[3] << ",buf=@0x" << std::hex << this->gpr[4] << std::dec << ");";
				break;

			case 29:
				sstr << "pause();";
				break;
				
			case 30:
				sstr << "utime(filename=" << ReadString(this->gpr[3]) << ",times=@0x" << std::hex << this->gpr[4] << std::dec << ");";
				break;

			case 33:
				sstr << "access(pathname=" << ReadString(this->gpr[3])
					<< ",mode=0x" << std::hex << this->gpr[4] << std::dec << ");";
				break;
				
			case 34:
				sstr << "nice(inc=" << (signed) this->gpr[3] << ");";
				break;

			case 35:
				sstr << "ftime(tp=@" << std::hex << this->gpr[3] << std::dec << ");";
				break;
				
			case 36:
				sstr << "sync();";
				break;
				
			case 37:
				sstr << "kill(pid=" << this->gpr[3] << ",sig=" << (signed) this->gpr[4] << ");";
				break;
				
			case 38:
				sstr << "rename(oldpath=" << ReadString(this->gpr[3]) << ",newpath=" << ReadString(this->gpr[4]) << ");";
				break;

			case 39:
				sstr << "mkdir(pathname=" << ReadString(this->gpr[3]) << ",mode=0x" << std::hex << this->gpr[4] << std::dec << ");";
				break;
				
			case 40:
				sstr << "rmdir(pathname=" << ReadString(this->gpr[3]) << ");";
				break;
				
			case 41:
				sstr << "dup(oldfd=" << (signed) this->gpr[3] << ");";
				break;
			
			case 42:
				sstr << "pipe(pipefd=@" << std::hex << this->gpr[3] << std::dec << ");";
				break;
				
			case 43:
				sstr << "times(buf=@" << std::hex << this->gpr[3] << std::dec << ");";
				break;
				
			case 45:
				sstr << "brk(end_data_segment=@0x" << std::hex << this->gpr[3] << std::dec << ");";
				break;
				
			case 46:
				sstr << "setgid(gid=" << this->gpr[3] << ");";
				break;
			
			case 47:
				sstr << "getgid();";
				break;
				
			case 48:
				sstr << "signal(signum=" << (signed) this->gpr[3] << ",handler=@" << std::hex << this->gpr[4] << std::dec << ");";
				break;
				
			case 49:
				sstr << "geteuid();";
				break;
				
			case 50:
				sstr << "getegid();";
				break;
				
			case 51:
				sstr << "acct(filename=" << ReadString(this->gpr[3]) << ");";
				break;
				
			case 52:
				sstr << "umount2(target=" << ReadString(this->gpr[3]) << ",flags=" << (signed) this->gpr[4] << ");";
				break;
				
			case 54:
				sstr << "ioctl(d=" << (signed) this->gpr[3] << ",request=" << (signed) this->gpr[4] << ",argp=@" << std::hex << this->gpr[5] << std::dec << ");";
				break;
			
			case 55:
				sstr << "fcntl(fd=" << (signed) this->gpr[3] << ",cmd=0x" << std::hex << this->gpr[4] << std::dec << ",arg=@0x" << std::hex << this->gpr[5] << std::dec << ");";
				break;
				
			case 57:
				sstr << "setpgid(pid=" << this->gpr[3] << ",pgid=" << this->gpr[4] << ");";
				break;
				
			case 58:
				sstr << "ulimit(cmd=" << (signed) this->gpr[3] << ",newlimit=" << (signed) this->gpr[4] << ");";
				break;

			case 59:
				sstr << "oldolduname(buf=@0x" << std::hex << this->gpr[3] << std::dec << ");";
				break;

			case 60:
				sstr << "umask(mask=0" << std::oct << this->gpr[3] << std::dec << ");";
				break;
				
			case 61:
				sstr << "chroot(path=" << ReadString(this->gpr[3]) << ");";
				break;
				
			case 62:
				sstr << "ustat(dev=" << this->gpr[3] << ",ubuf=@" << this->gpr[4] << ");";
				break;
				
			case 63:
				sstr << "dup2(old=" << (signed) this->gpr[3] << ",new=" << (signed) this->gpr[4] << ");";
				break;
				
			case 64:
				sstr << "getppid();";
				break;
			
			case 65:
				sstr << "getpgrp();";
				break;
				
			case 66:
				sstr << "setsid();";
				break;
				
			case 78:
				sstr << "gettimeofday(tv=@" << this->gpr[3] << ",tz=@" << this->gpr[4] << ");";
				break;
				
			case 88:
				sstr << "reboot(magic=" << (signed) this->gpr[3] << ",magic2=" << (signed) this->gpr[4] << ",cmd=" << (signed) this->gpr[5] << ",arg=@0x" << std::hex << this->gpr[6] << std::dec << ");";
				break;
				
			case 90:
				sstr << "mmap(addr=@0x" << std::hex << this->gpr[3] << std::dec << ",length=" << this->gpr[4] << ",prot=0x"
						<< std::hex << this->gpr[5] << ",flags=0x" << this->gpr[6] << std::dec << ",fd=" << (signed) this->gpr[7]
						<< ",offset=" << this->gpr[8] << ");";
				break;
				
			case 91:
				sstr << "munmap(addr=@0x" << std::hex << this->gpr[3] << std::dec << ",length=" << this->gpr[4] << ");";
				break;
				
			case 102:
				sstr << "socketcall(call=" << (signed) this->gpr[3] << ",args=@0x" << this->gpr[4] << ");";
				break;
				
			case 106:
				sstr << "stat(path=" << ReadString(this->gpr[3]) << ",buf=@0x" << std::hex << this->gpr[4] << std::dec << ");";
				break;
				
			case 114:
				sstr << "wait4(pid=" << this->gpr[3] << ",status=@0x" << std::hex << this->gpr[4] << ",options=0x" << this->gpr[5] << ",rusage=@0x" << this->gpr[6] << std::dec << ");";
				break;
			
			case 120:
				sstr << "clone(fn=0x" << std::hex << this->gpr[3] << ",child_stack=0x" << this->gpr[4] << ",flags=0x" << this->gpr[5] << ",arg=@0x" << this->gpr[6] << std::dec << ");";
				break;
				
			case 122:
				sstr << "uname(buf=@0x" << std::hex << this->gpr[3] << std::dec << ");";
				break;
				
			case 140:
				sstr << "llseek(fd=" << (signed) this->gpr[3] << ",offset_high=" << this->gpr[4] << ",offset_low=" << this->gpr[5] << ",result=@0x" << this->gpr[6] << ",whence=" << this->gpr[7] << ");";
				break;
				
			case 141:
				sstr << "getdents(fd=" << (signed) this->gpr[3] << ",dirp=@0x" << std::hex << this->gpr[4] << std::dec << ",count=" << this->gpr[5] << ");";
				break;
				
			case 146:
				sstr << "writev(fd=" << (signed) this->gpr[3] << ",iov=@0x" << std::hex << this->gpr[4] << std::dec << ",iovcnt=" << (signed) this->gpr[5] << ");";
				break;
				
			case 162:
				sstr << "nanosleep(req=@" << std::hex << this->gpr[3] << std::dec << ",rem=@" << std::hex << this->gpr[4] << std::dec << ");";
				break;
				
			case 167:
				sstr << "poll(fds=@" << std::hex << this->gpr[3] << std::dec << ",nfds=" << this->gpr[4] << ",timeout=" << (signed) this->gpr[5] << ");";
				break;
				
			case 173:
				sstr << "rt_sigaction(signum=" << (signed) this->gpr[3] << ",act=@0x" << std::hex << this->gpr[4] << ",oldact=@0x" << this->gpr[5] << std::dec << ",sigsetsize=" << this->gpr[6] << ");";
				break;
				
			case 174:
				sstr << "rt_sigprocmask(how=" << (signed) this->gpr[3] << ",set=@0x" << std::hex << this->gpr[4] << ",oldset=@0x" << this->gpr[5] << std::dec << ",sigsetsize=" << this->gpr[6] << ");";
				break;
				
			case 182:
				sstr << "getcwd(buf=@0x" << std::hex << this->gpr[3] << std::dec << ",size=" << this->gpr[4] << ");";
				break;
				
			case 189:
				sstr << "vfork();";
				break;
				
			case 195:
				sstr << "stat64(path=" << ReadString(this->gpr[3]) << ",buf=@0x" << std::hex << this->gpr[4] << std::dec << ");";
				break;
				
			case 196:
				sstr << "lstat64(path=" << ReadString(this->gpr[3]) << ",buf=@0x" << std::hex << this->gpr[4] << std::dec << ");";
				break;
				
			case 197:
				sstr << "fstat64(fd=" << (signed) this->gpr[3] << ",buf=@0x" << std::hex << this->gpr[4] << std::dec << ");";
				break;
				
			case 202:
				sstr << "getdents64(fd=" << (signed) this->gpr[3] << ",dirp=@0x" << std::hex << this->gpr[4] << std::dec << ",count=" << this->gpr[5] << ");";
				break;
				
			case 204:
				sstr << "fcntl64(fd=" << (signed) this->gpr[3] << ",cmd=0x" << std::hex << this->gpr[4] << std::dec << ",arg=@0x" << std::hex << this->gpr[5] << std::dec << ");";
				break;
				
			case 207:
				sstr << "gettid();";
				break;
				
			case 208:
				sstr << "tkill(tid=" << (signed) this->gpr[3] << ",sig=" << (signed) this->gpr[4] << ");";
				break;

			case 234:
				sstr << "exit_group(" << (signed) this->gpr[3] << ");";
				break;
				
			case 250:
				sstr << "tgkill(tgid=" << (signed) this->gpr[3] << ",tid=" << (signed) this->gpr[4] << ",sig=" << (signed) this->gpr[5] << ");";
				break;
				
			default:
				sstr << "unknown_syscall_" << this->gpr[0] << "();";
				break;
		}
		
		std::string msg = sstr.str();
		
		this->logger << DebugInfo << msg << EndDebugInfo;
	}
	
	if(this->linux_os_import)
	{
		// Do Linux ABI translation
		this->linux_os_import->ExecuteSystemCall(this->gpr[0]);
	}
	else
	{
		srr0 = this->nia;
		srr1 = msr;
		
		msr.template Set<typename SystemCallInterrupt::MSR_ALWAYS_CLEARED_FIELDS>(0);
		
		this->cia = ivpr | ivor8;
	}
	
	this->template AckInterrupt<SystemCallInterrupt>();
	
	system_call_interrupt->ClearELEV();
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::ProcessInterrupt(AuxiliaryProcessorUnavailableInterrupt *auxiliary_processor_unavailable_interrupt)
{
	srr0 = this->cia;
	srr1 = msr;
	
	msr.template Set<typename AuxiliaryProcessorUnavailableInterrupt::MSR_ALWAYS_CLEARED_FIELDS>(0);
	
	this->cia = ivpr | ivor9;
	
	this->template AckInterrupt<AuxiliaryProcessorUnavailableInterrupt>();
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::ProcessInterrupt(DecrementerInterrupt *decrementer_interrupt)
{
	srr0 = this->cia;
	srr1 = msr;
	
	msr.template Set<typename DecrementerInterrupt::MSR_ALWAYS_CLEARED_FIELDS>(0);
	
	this->cia = ivpr | ivor10;
	
	this->template AckInterrupt<DecrementerInterrupt>();
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::ProcessInterrupt(FixedIntervalTimerInterrupt *fixed_interval_timer_interrupt)
{
	srr0 = this->cia;
	srr1 = msr;
	
	msr.template Set<typename FixedIntervalTimerInterrupt::MSR_ALWAYS_CLEARED_FIELDS>(0);
	
	this->cia = ivpr | ivor11;
	
	this->template AckInterrupt<FixedIntervalTimerInterrupt>();
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::ProcessInterrupt(WatchdogTimerInterrupt *watchdog_timer_interrupt)
{
	srr0 = this->cia;
	srr1 = msr;
	
	msr.template Set<typename WatchdogTimerInterrupt::MSR_ALWAYS_CLEARED_FIELDS>(0);
	
	this->cia = ivpr | ivor12;
	
	this->template AckInterrupt<WatchdogTimerInterrupt>();
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::ProcessInterrupt(DataTLBErrorInterrupt *data_tlb_error_interrupt)
{
	srr0 = this->cia;
	srr1 = msr;
	
	msr.template Set<typename DataTLBErrorInterrupt::MSR_ALWAYS_CLEARED_FIELDS>(0);
	
	dear = data_tlb_error_interrupt->GetAddress();
	
	esr = 0;
	operation->update_esr_fp(static_cast<typename CONFIG::CPU *>(this));
	operation->update_esr_st(static_cast<typename CONFIG::CPU *>(this));
	operation->update_esr_ap(static_cast<typename CONFIG::CPU *>(this));
	
	this->cia = ivpr | ivor13;
	
	this->template AckInterrupt<DataTLBErrorInterrupt>();
	
	data_tlb_error_interrupt->ClearAddress();
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::ProcessInterrupt(InstructionTLBErrorInterrupt *instruction_tlb_error_interrupt)
{
	srr0 = this->cia;
	srr1 = msr;
	
	msr.template Set<typename InstructionTLBErrorInterrupt::MSR_ALWAYS_CLEARED_FIELDS>(0);
	
	this->cia = ivpr | ivor14;
	
	this->template AckInterrupt<InstructionTLBErrorInterrupt>();
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::ProcessInterrupt(DebugInterrupt *debug_interrupt)
{
	srr0 = this->cia;
	srr1 = msr;
	
	msr.template Set<typename DebugInterrupt::MSR_ALWAYS_CLEARED_FIELDS>(0);
	
	this->cia = ivpr | ivor15;
	
	this->template AckInterrupt<DebugInterrupt>();
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::UpdateExceptionEnable()
{
	if(msr.template Get<typename MSR::CE>())
	{
		this->template EnableInterrupt<CriticalInputInterrupt>();
	}
	else
	{
		this->template DisableInterrupt<CriticalInputInterrupt>();
	}
	
	if(msr.template Get<typename MSR::EE>())
	{
		this->template EnableInterrupt<ExternalInputInterrupt>();
	}
	else
	{
		this->template DisableInterrupt<ExternalInputInterrupt>();
	}
	
	if(msr.template Get<typename MSR::ME>())
	{
		this->template EnableInterrupt<MachineCheckInterrupt>();
	}
	else
	{
		this->template DisableInterrupt<MachineCheckInterrupt>();
	}
	
	if(msr.template Get<typename MSR::EE>() && tcr.template Get<typename TCR::DIE>())
	{
		this->template EnableInterrupt<DecrementerInterrupt>();
	}
	else
	{
		this->template DisableInterrupt<DecrementerInterrupt>();
	}

	if(msr.template Get<typename MSR::EE>() && tcr.template Get<typename TCR::FIE>())
	{
		this->template EnableInterrupt<FixedIntervalTimerInterrupt>();
	}
	else
	{
		this->template DisableInterrupt<FixedIntervalTimerInterrupt>();
	}

	if(msr.template Get<typename MSR::CE>() && tcr.template Get<typename TCR::WIE>())
	{
		this->template EnableInterrupt<WatchdogTimerInterrupt>();
	}
	else
	{
		this->template DisableInterrupt<WatchdogTimerInterrupt>();
	}

	if(msr.template Get<typename MSR::DE>())
	{
		this->template EnableInterrupt<DebugInterrupt>();
	}
	else
	{
		this->template DisableInterrupt<DebugInterrupt>();
	}
	
	this->Synchronize(); // Force resampling input signals
}

template <typename TYPES, typename CONFIG>
template <typename T, bool REVERSE, unisim::util::endian::endian_type ENDIAN>
void CPU<TYPES, CONFIG>::ConvertDataLoadStoreEndian(T& value, STORAGE_ATTR storage_attr)
{
#if BYTE_ORDER == LITTLE_ENDIAN
	if((!REVERSE && ((ENDIAN == unisim::util::endian::E_BIG_ENDIAN)    || ((ENDIAN == unisim::util::endian::E_UNKNOWN_ENDIAN) &&   likely(!(storage_attr & TYPES::SA_E))))) ||
	   ( REVERSE && ((ENDIAN == unisim::util::endian::E_LITTLE_ENDIAN) || ((ENDIAN == unisim::util::endian::E_UNKNOWN_ENDIAN) && unlikely(  storage_attr & TYPES::SA_E)))))
#elif BYTE_ORDER == BIG_ENDIAN
	if(( REVERSE && ((ENDIAN == unisim::util::endian::E_LITTLE_ENDIAN) || ((ENDIAN == unisim::util::endian::E_UNKNOWN_ENDIAN) && unlikely(  storage_attr & TYPES::SA_E)))) ||
	   (!REVERSE && ((ENDIAN == unisim::util::endian::E_BIG_ENDIAN)    || ((ENDIAN == unisim::util::endian::E_UNKNOWN_ENDIAN) &&   likely(!(storage_attr & TYPES::SA_E))))))
#else
	if(REVERSE)
#endif
	{
		unisim::util::endian::BSwap(value);
	}
}

template <typename TYPES, typename CONFIG>
template <unsigned int SIZE>
bool CPU<TYPES, CONFIG>::CheckIntLoadStoreAlignment(EFFECTIVE_ADDRESS ea)
{
	if(unlikely(ea % SIZE) && unlikely((ccr0.template Get<typename SuperCPU::CCR0::FLSTA>())))
	{
		this->template ThrowException<typename AlignmentInterrupt::UnalignedIntegerLoadStore>().SetAddress(ea);
		return false;
	}
	return true;
}

template <typename TYPES, typename CONFIG>
template <unsigned int SIZE>
bool CPU<TYPES, CONFIG>::CheckFloatingPointLoadStoreAlignment(EFFECTIVE_ADDRESS ea)
{
	if(unlikely(ea % SIZE) && unlikely((ccr0.template Get<typename SuperCPU::CCR0::FLSTA>())))
	{
		this->template ThrowException<typename AlignmentInterrupt::UnalignedFloatingPointLoadStore>().SetAddress(ea);
		return false;
	}
	return true;
}

template <typename TYPES, typename CONFIG>
std::string CPU<TYPES, CONFIG>::Disasm(EFFECTIVE_ADDRESS addr, EFFECTIVE_ADDRESS& next_addr)
{
	std::stringstream sstr;
	typename CONFIG::OPERATION *operation;
	typename CONFIG::CODE_TYPE insn;

	EFFECTIVE_ADDRESS size_to_protection_boundary;
	ADDRESS virt_addr;
	PHYSICAL_ADDRESS phys_addr;
	STORAGE_ATTR storage_attr;

	if(unlikely((!this->template Translate</* debug */ true, /* exec */ true, /* write */ false>(addr, size_to_protection_boundary, virt_addr, phys_addr, storage_attr))) ||
	   unlikely(!this->SuperMSS::DebugInstructionFetch(virt_addr, phys_addr, &insn, 4, storage_attr)))
	{
		next_addr = addr + 4;
		return std::string("unreadable ?");
	}
	
#if BYTE_ORDER == BIG_ENDIAN
	if(unlikely(storage_attr & TYPES::SA_E)) // little-endian ?
#elif BYTE_ORDER == LITTLE_ENDIAN
	if(likely(!(storage_attr & TYPES::SA_E))) // big-endian ?
#else
	if(0)
#endif
	{
		unisim::util::endian::BSwap(insn);
	}

	operation = decoder.Decode(addr, insn);
	
	// disassemble the instruction
	
	operation->disasm(static_cast<typename CONFIG::CPU *>(this), sstr);
	next_addr = addr + (operation->GetLength() / 8);
	
	return sstr.str();
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::Lwarx(unsigned int rd, EFFECTIVE_ADDRESS addr)
{
	// TODO: reservation logic
	return this->Int32Load(rd, addr);
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::Mbar(EFFECTIVE_ADDRESS addr)
{
	return true;
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::Stwcx(unsigned int rs, EFFECTIVE_ADDRESS addr)
{
	// TODO: reservation logic
	// Note: Address match with prior lbarx, lharx, or lwarx not required for store to be performed
	if(!this->Int32Store(rs, addr)) return false;
	// Clear CR0[LT][GT], setCR0[EQ] and copy XER[SO] to CR0[SO]
	this->cr.template Set<typename CR::CR0::LT>(0);
	this->cr.template Set<typename CR::CR0::GT>(0);
	this->cr.template Set<typename CR::CR0::EQ>(1);
	this->cr.template Set<typename CR::CR0::SO>(this->xer.template Get<typename XER::SO>());
	
	return true;
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::Wait()
{
	Idle();
	return true;
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::Msync()
{
	return true;
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::Isync()
{
	return true;
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::Rfi()
{
	if(msr.template Get<typename MSR::PR>())
	{
		this->template ThrowException<typename ProgramInterrupt::PrivilegeViolation>();
		return false;
	}
	
	struct B0_30 : Field<void, 0, 30> {};

	this->Branch(srr0 & B0_30::template GetMask<EFFECTIVE_ADDRESS>());
	msr = srr1;
	
	if(unlikely(this->verbose_interrupt))
	{
		this->logger << DebugInfo << "Returning from interrupt to 0x" << std::hex << this->nia << std::dec << EndDebugInfo;
	}

	this->FlushInstructionBuffer();
	this->Synchronize();
	
	return true;
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::Rfci()
{
	if(msr.template Get<typename MSR::PR>())
	{
		this->template ThrowException<typename ProgramInterrupt::PrivilegeViolation>();
		return false;
	}
	
	struct B0_30 : Field<void, 0, 30> {};

	this->Branch(csrr0 & B0_30::template GetMask<EFFECTIVE_ADDRESS>());
	msr = csrr1;
	
	if(unlikely(this->verbose_interrupt))
	{
		this->logger << DebugInfo << "Returning from critical interrupt to 0x" << std::hex << this->nia << std::dec << EndDebugInfo;
	}

	this->FlushInstructionBuffer();
	this->Synchronize();

	return true;
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::Rfmci()
{
	if(msr.template Get<typename MSR::PR>())
	{
		this->template ThrowException<typename ProgramInterrupt::PrivilegeViolation>();
		return false;
	}
	
	struct B0_30 : Field<void, 0, 30> {};
	
	this->Branch(mcsrr0 & B0_30::template GetMask<EFFECTIVE_ADDRESS>());
	msr = mcsrr1;
	
	if(unlikely(this->verbose_interrupt))
	{
		this->logger << DebugInfo << "Returning from machine check interrupt to 0x" << std::hex << this->nia << std::dec << EndDebugInfo;
	}

	this->FlushInstructionBuffer();
	this->Synchronize();

	return true;
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::FlushInstructionBuffer()
{
	instruction_buffer_base_addr = ~EFFECTIVE_ADDRESS(0);
}

template <typename TYPES, typename CONFIG>
bool CPU<TYPES, CONFIG>::InstructionFetch(EFFECTIVE_ADDRESS addr, typename CONFIG::CODE_TYPE& insn)
{
	EFFECTIVE_ADDRESS base_addr = addr & ~(CONFIG::INSTRUCTION_BUFFER_SIZE - 1);
	int instruction_buffer_index = (addr / 4) % (CONFIG::INSTRUCTION_BUFFER_SIZE / 4);
	
	if(base_addr != instruction_buffer_base_addr)
	{
		EFFECTIVE_ADDRESS size_to_protection_boundary;
		ADDRESS base_virt_addr;
		PHYSICAL_ADDRESS base_phys_addr;
		STORAGE_ATTR storage_attr;
		
		if(unlikely((!this->template Translate</* debug */ false, /* exec */ true, /* write */ false>(base_addr, size_to_protection_boundary, base_virt_addr, base_phys_addr, storage_attr)))) return false;
		
		if(unlikely(!this->SuperMSS::InstructionFetch(base_virt_addr, base_phys_addr, instruction_buffer, CONFIG::INSTRUCTION_BUFFER_SIZE, storage_attr))) return false;
		
#if BYTE_ORDER == BIG_ENDIAN
		if(unlikely(storage_attr & TYPES::SA_E)) // little-endian ?
#elif BYTE_ORDER == LITTLE_ENDIAN
		if(likely(!(storage_attr & TYPES::SA_E))) // big-endian ?
#else
		if(0)
#endif
		{
			unisim::util::endian::BSwap(instruction_buffer, CONFIG::INSTRUCTION_BUFFER_SIZE / 4);
		}
		
		instruction_buffer_base_addr = base_addr;
	}
	else
	{
		// Account for level 1 cache access
		typename CONFIG::CPU::INSTRUCTION_CACHE_HIERARCHY::L1CACHE *l1i = static_cast<typename CONFIG::CPU *>(this)->GetCache((typename CONFIG::CPU::INSTRUCTION_CACHE_HIERARCHY::L1CACHE *) 0);
		if(l1i && l1i->IsEnabled())
		{
			l1i->Access();
		}
	}
	
	insn = instruction_buffer[instruction_buffer_index];
	
	return true;
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::StepOneInstruction()
{
	this->ProcessExceptions();

	if(unlikely(this->requires_fetch_instruction_reporting))
	{
		if(unlikely(this->memory_access_reporting_import != 0))
		{
			this->memory_access_reporting_import->ReportFetchInstruction(this->cia);
		}
	}
	
	if(unlikely(this->debug_yielding_import != 0))
	{
		this->debug_yielding_import->DebugYield();
	}

	EFFECTIVE_ADDRESS addr = this->cia;
	typename CONFIG::CODE_TYPE insn = 0;
	if(likely(InstructionFetch(addr, insn)))
	{
		operation = decoder.Decode(addr, insn);

		unsigned int length = operation->GetLength() / 8;
		this->nia = this->cia + length;
		if(unlikely(this->enable_insn_trace))
		{
			std::stringstream sstr;
			operation->disasm(static_cast<typename CONFIG::CPU *>(this), sstr);
			this->logger << DebugInfo << "executing instruction #" << this->instruction_counter << ":0x" << std::hex << addr << std::dec << ":" << sstr.str() << EndDebugInfo;
		}
		
		/* execute the instruction */
		if(likely(operation->execute(static_cast<typename CONFIG::CPU *>(this))))
		{
			/* update the instruction counter */
			this->instruction_counter++;
			
			/* report a finished instruction */
			if(unlikely(this->requires_commit_instruction_reporting))
			{
				if(unlikely(this->memory_access_reporting_import != 0))
				{
					this->memory_access_reporting_import->ReportCommitInstruction(this->cia, length);
				}
			}

			/* go to the next instruction */
			this->cia = this->nia;

			if(unlikely(this->trap_reporting_import && (this->instruction_counter == this->trap_on_instruction_counter)))
			{
				this->trap_reporting_import->ReportTrap();
			}
			
			if(unlikely((this->instruction_counter >= this->max_inst) || (this->cia == this->halt_on_addr))) this->Halt();
		}
		else if(unlikely(this->verbose_exception))
		{
			std::stringstream sstr;
			operation->disasm(static_cast<typename CONFIG::CPU *>(this), sstr);
			this->logger << DebugInfo << "Aborted instruction #" << this->instruction_counter << ":0x" << std::hex << addr << std::dec << ":" << sstr.str() << EndDebugInfo;
		}
	}
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::TimeBaseValueChanged(uint64_t old_value)
{
	//std::cerr << "tb: " << old_value << " -> " << uint64_t(tb) << std::endl;
	uint64_t new_value = uint64_t(tb);
	uint32_t tb_fit_mask = 4096 << (4 * tcr.template Get<typename TCR::FP>());
	if(!(old_value & tb_fit_mask) && (new_value & tb_fit_mask)) // selected bit toggle from 0 to 1
	{
		//std::cerr << "tb_fit_mask=0x" << std::hex << tb_fit_mask << std::dec << std::endl;
		if(tsr.template Get<typename TSR::ENW>())
		{
			if(tsr.template Get<typename TSR::WIS>())
			{
				if(tcr.template Get<typename TCR::WRC>() != 0)
				{
					tsr.template Set<typename TSR::WRS>(tcr.template Get<typename TCR::WRC>()); // TSR[WRS]=TCR[WRC]
					this->logger << DebugWarning << "A reset because of watchdog timer occured" << EndDebugWarning;
					this->Stop(0);
				}
				else
				{
					// nothing
				}
			}
			else
			{
				tsr.template Set<typename TSR::WIS>(1);
			}
		}
		else
		{
			tsr.template Set<typename TSR::ENW>(1);
		}
		
		this->template ThrowException<typename FixedIntervalTimerInterrupt::FixedIntervalTimer>();
	}
	
	uint64_t tb_watchdog_mask = uint64_t(1048576) << (4 * tcr.template Get<typename TCR::WP>());
	if(!(old_value & tb_watchdog_mask) && (new_value & tb_watchdog_mask)) // selected bit toggle from 0 to 1
	{
		//std::cerr << "cnt=" << cnt << ", tb_watchdog_mask=0x" << std::hex << tb_watchdog_mask << std::dec << std::endl;
		tsr.template Set<typename TSR::WIS>(1);
		this->template ThrowException<typename WatchdogTimerInterrupt::WatchdogTimer>();
	}
}

template <typename TYPES, typename CONFIG>
void CPU<TYPES, CONFIG>::RunTimers(uint64_t delta)
{
	tb = uint64_t(tb) + delta;
	
	uint32_t old_dec_value = uint32_t(dec);
	if(old_dec_value > 0)
	{
		uint32_t new_dec_value = (delta <= old_dec_value) ? old_dec_value - delta : 0;
		
		dec = new_dec_value;
		
		if(unlikely(new_dec_value == 0))
		{
			tsr.template Set<typename TSR::DIS>(1);
			this->template ThrowException<typename DecrementerInterrupt::Decrementer>();
			
			if(tcr.template Get<typename TCR::ARE>())
			{
				//std::cerr << "autoreloading" << std::endl;
				if((delta > old_dec_value) && uint32_t(decar)) // several autoreload occured during instruction execution
				{
					dec = uint32_t(decar) - ((delta - old_dec_value) % uint32_t(decar));
				}
				else
				{
					dec = uint32_t(decar);
				}
			}
		}
	}
}

template <typename TYPES, typename CONFIG>
uint64_t CPU<TYPES, CONFIG>::GetMaxIdleTime() const
{
	uint64_t delay_dec = 0xffffffffffffffffULL;
	uint64_t delay_fit = 0xffffffffffffffffULL;
	uint64_t delay_watchdog = 0xffffffffffffffffULL;
	
	if(msr.template Get<typename MSR::EE>())
	{
		if(tcr.template Get<typename TCR::DIE>())
		{
			uint32_t dec_value = uint32_t(dec);
			if(dec_value) delay_dec = dec_value;
			//std::cerr << "Time for DEC to reach zero: " << delay_dec << std::endl;
		}
		
		if(tcr.template Get<typename TCR::FIE>())
		{
			uint64_t tb_value = uint64_t(tb);
			
			uint32_t tb_fit_mask = 4096 << (4 * tcr.template Get<typename TCR::FP>());
			
			if(tb_value & tb_fit_mask)
			{
				// time to toogle from 1 to 0
				delay_fit = tb_fit_mask - (tb_value & (tb_fit_mask - 1));
				// time to toogle from 0 to 1
				delay_fit += tb_fit_mask;
			}
			else
			{
				// time to toogle from 0 to 1
				delay_fit = tb_fit_mask - (tb_value & (tb_fit_mask - 1));
			}
			//std::cerr << "Time for FIT: " << delay_fit << " (tb=0x" << std::hex << tb_value << std::dec << ", FIT mask=" << std::hex << tb_fit_mask << std::dec << ")" << std::endl;
		}
	}
	
	if(tcr.template Get<typename TCR::WIE>() && msr.template Get<typename MSR::CE>())
	{
		uint64_t tb_value = uint64_t(tb);

		uint64_t tb_watchdog_mask = uint64_t(1048576) << (4 * tcr.template Get<typename TCR::WP>());
		
		if(tb & tb_watchdog_mask)
		{
			// time to toogle from 1 to 0
			delay_watchdog = tb_watchdog_mask - (tb_value & (tb_watchdog_mask - 1));
			// time to toogle from 0 to 1
			delay_watchdog += tb_watchdog_mask;
		}
		else
		{
			// time to toogle from 0 to 1
			delay_watchdog = tb_watchdog_mask - (tb_value & (tb_watchdog_mask - 1));
		}
		//std::cerr << "Time for WDG: " << delay_watchdog << " (tb=0x" << std::hex << GetTB() << std::dec << ", WDG mask=" << std::hex << tb_watchdog_mask << std::dec << ")" << std::endl;
	}
	
	uint64_t max_idle_time = delay_dec < delay_fit ? (delay_dec < delay_watchdog ? delay_dec : delay_watchdog) : (delay_fit < delay_watchdog ? delay_fit : delay_watchdog);
	return max_idle_time;
}

template <typename TYPES, typename CONFIG>
uint64_t CPU<TYPES, CONFIG>::GetTimersDeadline() const
{
	uint64_t delay_dec = 0xffffffffffffffffULL;
	uint64_t delay_fit = 0xffffffffffffffffULL;
	uint64_t delay_watchdog = 0xffffffffffffffffULL;
	
	uint32_t dec_value = uint32_t(dec);
	if(dec_value) delay_dec = dec_value;
	
	uint64_t tb_value = uint64_t(tb);
	
	uint32_t tb_fit_mask = 4096 << (4 * tcr.template Get<typename TCR::FP>());
	
	if(tb_value & tb_fit_mask)
	{
		// time to toogle from 1 to 0
		delay_fit = tb_fit_mask - (tb_value & (tb_fit_mask - 1));
		// time to toogle from 0 to 1
		delay_fit += tb_fit_mask;
	}
	else
	{
		// time to toogle from 0 to 1
		delay_fit = tb_fit_mask - (tb_value & (tb_fit_mask - 1));
	}
	
	uint32_t tb_watchdog_mask = 1048576 << (4 * tcr.template Get<typename TCR::WP>());
	
	if(tb & tb_watchdog_mask)
	{
		// time to toogle from 1 to 0
		delay_watchdog = tb_watchdog_mask - (tb_value & (tb_watchdog_mask - 1));
		// time to toogle from 0 to 1
		delay_watchdog += tb_watchdog_mask;
	}
	else
	{
		// time to toogle from 0 to 1
		delay_watchdog = tb_watchdog_mask - (tb_value & (tb_watchdog_mask - 1));
	}
	
	uint64_t timers_deadline = delay_dec < delay_fit ? (delay_dec < delay_watchdog ? delay_dec : delay_watchdog) : (delay_fit < delay_watchdog ? delay_fit : delay_watchdog);
	return timers_deadline;
}

template <typename TYPES, typename CONFIG>
int CPU<TYPES, CONFIG>::StringLength(EFFECTIVE_ADDRESS addr)
{
	int len = 0;
	char buffer;

	while(1)
	{
		buffer = 0;
		this->ReadMemory(addr, &buffer, 1);
		if(buffer == 0) return len;
		len++;
		addr += 1;
	}
}

template <typename TYPES, typename CONFIG>
std::string CPU<TYPES, CONFIG>::ReadString(EFFECTIVE_ADDRESS addr, unsigned int count)
{
	if(!count)
	{
		count = StringLength(addr);
	}
	
	char *buf = new char[count + 1]();
	this->ReadMemory(addr, buf, count);

	std::stringstream sstr;

	bool open_string = false;
	unsigned int i;
	for(i = 0; i <= count; i++)
	{
		char c = buf[i];
		if(c < ' ') // control character
		{
			switch(c)
			{
				case '\n':
				case '\r':
				case '\e':
				case '\a':
				case '\f':
				case '\t':
				case '\v':
				case '\'':
				case '\\':
					if(!open_string)
					{
						sstr << "\"";
						open_string = true;
					}
					switch(c)
					{
						case '\n': sstr << "\\n"; break;
						case '\r': sstr << "\\r"; break;
						case '\e': sstr << "\\e"; break;
						case '\a': sstr << "\\a"; break;
						case '\f': sstr << "\\f"; break;
						case '\t': sstr << "\\t"; break;
						case '\v': sstr << "\\v"; break;
						case '\'': sstr << "\\'"; break;
						case '\\': sstr << "\\\\"; break;
					}
					if(i == count) sstr << "\"";
					break;
				default:
					if(open_string) sstr << "\"";
					sstr << " \\0x" << std::hex << (unsigned int) c << std::dec;
					if((i != count) && open_string) sstr << "\"";
					break;
			}
		}
		else
		{
			if(!open_string)
			{
				sstr << "\"";
				open_string = true;
			}
			sstr << c;
			if(i == count) sstr << "\"";
		}
	}
		
	delete[] buf;
	
	return sstr.str();
}

} // end of namespace book_e
} // end of namespace powerpc
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_BOOK_E_CPU_TCC__
