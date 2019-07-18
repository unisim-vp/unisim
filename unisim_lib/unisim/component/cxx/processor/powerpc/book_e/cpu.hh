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

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_BOOK_E_CPU_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_BOOK_E_CPU_HH__

#include <unisim/component/cxx/processor/powerpc/cpu.hh>
#include <unisim/component/cxx/processor/powerpc/floating.hh>
// #include <unisim/util/cache/cache.hh>
#include <unisim/util/endian/endian.hh>
#include <unisim/util/queue/queue.hh>
#include <unisim/util/queue/queue.tcc>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace powerpc {
namespace book_e {

enum BusResponseStatus
{
	BUS_OK_RESPONSE = 1,
	BUS_INCOMPLETE_RESPONSE = 0,
	BUS_GENERIC_ERROR_RESPONSE = -1,
	BUS_ADDRESS_ERROR_RESPONSE = -2,
	BUS_COMMAND_ERROR_RESPONSE = -3,
	BUS_BURST_ERROR_RESPONSE = -4,
	BUS_BYTE_ENABLE_ERROR_RESPONSE = -5
};

template <typename TYPES, typename CONFIG>
class CPU
	: public unisim::component::cxx::processor::powerpc::CPU<TYPES, CONFIG>
	, public unisim::kernel::service::Service<typename unisim::service::interfaces::Disassembly<typename TYPES::EFFECTIVE_ADDRESS> >
{
public:
	typedef typename unisim::component::cxx::processor::powerpc::CPU<TYPES, CONFIG> SuperCPU;
	typedef typename unisim::util::cache::MemorySubSystem<TYPES, typename CONFIG::CPU> SuperMSS;
	typedef typename TYPES::EFFECTIVE_ADDRESS EFFECTIVE_ADDRESS;
	typedef typename TYPES::ADDRESS ADDRESS;
	typedef typename TYPES::PHYSICAL_ADDRESS PHYSICAL_ADDRESS;
	typedef typename TYPES::STORAGE_ATTR STORAGE_ATTR;
	
	typedef typename SuperCPU::MCSR MCSR;
	typedef typename SuperCPU::ESR ESR;
	typedef typename SuperCPU::FPR FPR;
	//typedef typename SuperCPU::TCR TCR;
	typedef typename SuperCPU::TSR TSR;
// 	typedef typename SuperCPU::DBSR DBSR;
// 	typedef typename SuperCPU::FSCR FSCR;
	
	/////////////////////////// service exports ///////////////////////////////

	unisim::kernel::service::ServiceExport<unisim::service::interfaces::Disassembly<EFFECTIVE_ADDRESS> > disasm_export;

	////////////////////////////// constructor ////////////////////////////////
	
	CPU(const char *name, unisim::kernel::service::Object *parent = 0);

	/////////////////////////////// destructor ////////////////////////////////

	virtual ~CPU();
	
	////////////////////////////// setup hooks ////////////////////////////////

	virtual bool EndSetup();

	//////////////////////////////// Reset ////////////////////////////////////

	void Reset();
	
	//////////  unisim::service::interfaces::Disassembly<> ////////////////////
	
	virtual std::string Disasm(EFFECTIVE_ADDRESS addr, EFFECTIVE_ADDRESS& next_addr);
	
#if 0
	/////////////// unisim::service::interfaces::Memory<> /////////////////////
	
	virtual void Reset();
	virtual bool ReadMemory(EFFECTIVE_ADDRESS addr, void *buffer, uint32_t size);
	virtual bool WriteMemory(EFFECTIVE_ADDRESS addr, const void *buffer, uint32_t size);
#endif
	
	///////////////// Interface with SystemC TLM-2.0 wrapper module ///////////
	
	virtual void InterruptAcknowledge() {}
	virtual void Idle() {}
	virtual void Halt() {}
	
	////////////////////////// Machine State Register /////////////////////////
	
	struct MSR : SuperCPU::MSR
	{
		typedef typename SuperCPU::MSR Super;
		
		MSR(typename CONFIG::CPU *_cpu) : Super(_cpu) {}
		MSR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) {}
		
		inline void Set(unsigned int bit_offset, typename MSR::TYPE bit_value) ALWAYS_INLINE
		{
			Super::Set(bit_offset, bit_value);
			Effect();
		}
		
		template <typename FIELD> inline void Set(typename MSR::TYPE field_value)
		{
			Super::template Set<FIELD>(field_value);
			Effect();
		}

		MSR& operator = (const typename MSR::TYPE& value)
		{
			this->Super::operator = (value);
			Effect();
			return *this;
		}
		
		void Effect()
		{
// 			std::cerr << "#" << this->cpu->instruction_counter << ":MSR <- 0x" << std::hex << this->Get() << std::dec << std::endl;
			this->cpu->UpdateExceptionEnable();
			if(this->template Get<typename MSR::WE>())
			{
				this->cpu->Idle();
				this->template Set<typename MSR::WE>(0);
			}
		}
	};
	
	/////////////////////////// Time Control Register /////////////////////////
	
	struct TCR : SuperCPU::TCR
	{
		typedef typename SuperCPU::TCR Super;
		
		TCR(typename CONFIG::CPU *_cpu) : Super(_cpu) {}
		TCR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) {}
		
		inline void Set(unsigned int bit_offset, typename TCR::TYPE bit_value) ALWAYS_INLINE
		{
			Super::Set(bit_offset, bit_value);
			Effect();
		}
		
		template <typename FIELD> inline void Set(typename TCR::TYPE field_value)
		{
			Super::template Set<FIELD>(field_value);
			Effect();
		}

		TCR& operator = (const typename TCR::TYPE& value)
		{
			this->Super::operator = (value);
			Effect();
			return *this;
		}
		
		void Effect()
		{
			this->cpu->UpdateExceptionEnable();
		}
	};
	
	/////////////////////////////// Time Base /////////////////////////////////
	
	void TimeBaseValueChanged(uint64_t old_value);
	
	struct TB : SuperCPU::TB
	{
		typedef typename SuperCPU::TB Super;
		
		TB(typename CONFIG::CPU *_cpu) : Super(_cpu), cpu(_cpu) {}
		TB(typename CONFIG::CPU *_cpu, uint64_t _value) : Super(_cpu, _value), cpu(_cpu) {}
		
		virtual void ValueChanged(uint64_t old_value)
		{
			cpu->TimeBaseValueChanged(old_value);
		}
		
		using Super::operator =;
	private:
		typename CONFIG::CPU *cpu;
	};

	///////////// Interface with .isa behavioral description files ////////////
	
	MSR& GetMSR() { return msr; }
	ESR& GetESR() { return esr; }
	FPSCR& GetFPSCR() { return fpscr; }
	typename SuperCPU::SRR0& GetSRR0() { return srr0; }
	typename SuperCPU::SRR1& GetSRR1() { return srr1; }
	typename SuperCPU::CSRR0& GetCSRR0() { return csrr0; }
	typename SuperCPU::CSRR1& GetCSRR1() { return csrr1; }
	typename SuperCPU::MCSRR0& GetMCSRR0() { return mcsrr0; }
	typename SuperCPU::MCSRR1& GetMCSRR1() { return mcsrr1; }
	typename SuperCPU::RSTCFG& GetRSTCFG() { return rstcfg; }
	typename SuperCPU::PID0& GetPID() { return pid; }
// 	typename SuperCPU::TBL& GetTLB() { return tbl; }
// 	typename SuperCPU::TBU& GetTLU() { return tbu; }
	SoftDouble& GetFPR(unsigned int n) { return fpr[n]; }
	void SetFPR(unsigned int n, const SoftDouble& v) { fpr[n] = v; }

	bool Lwarx(unsigned int rd, EFFECTIVE_ADDRESS addr);
	bool Mbar(EFFECTIVE_ADDRESS addr);
	bool Stwcx(unsigned int rs, EFFECTIVE_ADDRESS addr);
	bool Wait();
	bool Msync();
	bool Isync();
	bool Rfi();
	bool Rfci();
	bool Rfmci();
	
	bool CheckFloatingPointException()
	{
		// Check for floating point exception condition: FPSCR[FEX] and (MSR[FE0] or MSR[FE1])
		if((fpscr.template Get<typename FPSCR::FEX>()) and (msr.template Get<typename MSR::FE0>() or msr.template Get<typename MSR::FE1>()))
		{
			// Raise a floating point exception if FPSCR[FEX] is set
			this->template ThrowException<typename ProgramInterrupt::FloatingPoint>();
			return false;
		}
		return true;
	}
	
	////////////////////////// Special Purpose Registers //////////////////////
	
	// Processor ID Register
	struct PIR : SuperCPU::PIR
	{
		typedef typename SuperCPU::PIR Super;
		
		PIR(typename CONFIG::CPU *_cpu) : Super(_cpu) {}
		PIR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) {}
		
		virtual void Reset()
		{
			this->template Set<typename PIR::PIN>(this->cpu->cpuid);
		}
	};
	
	//////////////////////////////// Interrupts ///////////////////////////////
	
	// Priorities:
	//  0  MachineCheckInterrupt                  InstructionAsynchronousMachineCheck
	//  1  MachineCheckInterrupt                  TLBAsynchronousMachineCheck
	//  2  MachineCheckInterrupt                  DataAsynchronousMachineCheck
	//  3  MachineCheckInterrupt                  InstructionSynchronousMachineCheck
	//  4  DebugInterrupt                         DebugEvent
	//  5  CriticalInputInterrupt                 CriticalInput
	//  6  WatchdogTimerInterrupt                 WatchdogTimer
	//  7  ExternalInputInterrupt                 ExternalInput
	//  8  FixedIntervalTimerInterrupt            FixedIntervalTimer
	//  9  DecrementerInterrupt                   Decrementer
	// 10  DataStorageInterrupt                   AccessControl
	// 11  DataStorageInterrupt                   ByteOrdering
	// 12  DataStorageInterrupt                   CacheLocking
	// 13  InstructionStorageInterrupt            AccessControl
	// 14  InstructionStorageInterrupt            ByteOrdering
	// 15  AlignmentInterrupt                     UnalignedIntegerLoadStore
	// 16  AlignmentInterrupt                     UnalignedLoadStoreMultiple
	// 17  AlignmentInterrupt                     UnalignedFloatingPointLoadStore
	// 18  AlignmentInterrupt                     UnalignedLoadLinkStoreConditional
	// 19  AlignmentInterrupt                     WriteThroughDCBZ
	// 20  ProgramInterrupt                       IllegalInstruction
	// 21  ProgramInterrupt                       PrivilegeViolation
	// 22  ProgramInterrupt                       Trap
	// 23  ProgramInterrupt                       FloatingPoint
	// 24  ProgramInterrupt                       AuxiliaryProcessor
	// 25  ProgramInterrupt                       UnimplementedInstruction
	// 26  FloatingPointUnavailableInterrupt      FloatingPointUnavailable
	// 27  SystemCallInterrupt                    SystemCall
	// 28  AuxiliaryProcessorUnavailableInterrupt AuxiliaryProcessorUnavailable
	// 29  DataTLBErrorInterrupt                  DataTLBError
	// 30  InstructionTLBErrorInterrupt           InstructionTLBError
	
	
	struct CriticalInputInterrupt : SuperCPU::template Interrupt<CriticalInputInterrupt, 0>
	{
		struct CriticalInput                        : SuperCPU::template Exception<CriticalInputInterrupt,5> { static const char *GetName() { return "Critical Input Interrupt/Critical Input Exception"; } };             //  p_critint_b is asserted and MSR[CE] = 1
		
		typedef typename SuperCPU::template ExceptionSet<CriticalInput> ALL;
		
		typedef FieldSet< typename MSR::WE
		                , typename MSR::CE
		                , typename MSR::EE
		                , typename MSR::PR
		                , typename MSR::FP
		                , typename MSR::FE0
		                , typename MSR::DWE
		                , typename MSR::DE
		                , typename MSR::FE1
		                , typename MSR::IS
		                , typename MSR::DS > MSR_ALWAYS_CLEARED_FIELDS;
		
		static const char *GetName() { return "Critical Input Interrupt"; }
	};

	struct MachineCheckInterrupt : SuperCPU::template InterruptWithAddress<MachineCheckInterrupt, 1>
	{
		typedef typename SuperCPU::template InterruptWithAddress<MachineCheckInterrupt, 1> Super;
		
		enum MachineCheckAsynchronousEventType
		{
			MCE_INSTRUCTION_PLB_ERROR          = 0x01, // Instruction PLB Error
			MCE_DATA_READ_PLB_ERROR            = 0x02, // Data Read PLB Error
			MCE_DATA_WRITE_PLB_ERROR           = 0x03, // Data Write PLB Error
			MCE_TLB_PARITY_ERROR               = 0x04, // Translation Lookaside Buffer Parity Error
			MCE_INSTRUCTION_CACHE_PARITY_ERROR = 0x08, // Instruction Cache Parity Error
			MCE_DATA_CACHE_SEARCH_PARITY_ERROR = 0x10, // Data Cache Search Parity Error
			MCE_DATA_CACHE_FLUSH_PARITY_ERROR  = 0x20, // Data Cache Flush Parity Error
		};

		struct InstructionSynchronousMachineCheck : SuperCPU::template Exception<MachineCheckInterrupt, 3> { static const char *GetName() { return "Machine Check Interrupt/Instruction Synchronous Machine Check Exception"; } };
		struct InstructionAsynchronousMachineCheck : SuperCPU::template Exception<MachineCheckInterrupt, 0> { static const char *GetName() { return "Machine Check Interrupt/Instruction Asynchronous Machine Check Exception"; } };
		struct DataAsynchronousMachineCheck : SuperCPU::template Exception<MachineCheckInterrupt, 2> { static const char *GetName() { return "Machine Check Interrupt/Data Asynchronous Machine Check Exception"; } };
		struct TLBAsynchronousMachineCheck : SuperCPU::template Exception<MachineCheckInterrupt, 1> { static const char *GetName() { return "Machine Check Interrupt/TLB Aynchronous Machine Check Exception"; } };

		typedef typename SuperCPU::template ExceptionSet< InstructionSynchronousMachineCheck
		                                                , InstructionAsynchronousMachineCheck
		                                                , DataAsynchronousMachineCheck
		                                                , TLBAsynchronousMachineCheck> ALL;
		
		typedef FieldSet< typename MSR::WE
		                , typename MSR::CE
		                , typename MSR::EE
		                , typename MSR::PR
		                , typename MSR::FP
		                , typename MSR::ME
		                , typename MSR::FE0
		                , typename MSR::DWE
		                , typename MSR::DE
		                , typename MSR::FE1
		                , typename MSR::IS
		                , typename MSR::DS > MSR_ALWAYS_CLEARED_FIELDS;
		
		MachineCheckInterrupt() : Super(), machine_check_async_events(0) {}
		bool GotAsynchronousEvents() const { return machine_check_async_events; }
		bool GotAsynchronousEvent(MachineCheckAsynchronousEventType machine_check_event) const { return machine_check_async_events & machine_check_event; }
		MachineCheckInterrupt& SetEvent(MachineCheckAsynchronousEventType machine_check_event) { machine_check_async_events |= machine_check_event; return *this; }
		void ClearAsynchronousEvents() { machine_check_async_events = 0; }
		static const char *GetName() { return "Machine Check Interrupt"; }
	private:
		unsigned int machine_check_async_events;
	};
	
	struct DataStorageInterrupt : SuperCPU::template InterruptWithAddress<DataStorageInterrupt, 2>
	{
		struct AccessControl                        : SuperCPU::template Exception<DataStorageInterrupt,10> { static const char *GetName() { return "Data Storage Interrupt/Access Control Exception"; } }; // Access control
		struct ByteOrdering                         : SuperCPU::template Exception<DataStorageInterrupt,11> { static const char *GetName() { return "Data Storage Interrupt/Byte Ordering Exception"; } };  // Byte ordering
		struct CacheLocking                         : SuperCPU::template Exception<DataStorageInterrupt,12> { static const char *GetName() { return "Data Storage Interrupt/Cache Locking Exception"; } };  // Cache locking
		
		typedef typename SuperCPU::template ExceptionSet<AccessControl> ALL;
		
		typedef FieldSet< typename MSR::WE
		                , typename MSR::EE
		                , typename MSR::PR
		                , typename MSR::FP
		                , typename MSR::FE0
		                , typename MSR::DWE
		                , typename MSR::FE1
		                , typename MSR::IS
		                , typename MSR::DS > MSR_ALWAYS_CLEARED_FIELDS;

		static const char *GetName() { return "Data Storage Interrupt"; }
		
		DataStorageInterrupt& SetWrite(bool v) { write = v; return *this; }
		bool IsWrite() const { return write; }
	private:
		bool write;
	};

	struct InstructionStorageInterrupt : SuperCPU::template Interrupt<InstructionStorageInterrupt, 3>
	{
		struct AccessControl : SuperCPU::template Exception<InstructionStorageInterrupt,13> { static const char *GetName() { return "Instruction Storage Interrupt/Access Control Exception"; } }; // Access control
		struct ByteOrdering  : SuperCPU::template Exception<InstructionStorageInterrupt,14> { static const char *GetName() { return "Instruction Storage Interrupt/Byte Ordering Exception"; } };  // Byte Ordering
		
		typedef typename SuperCPU::template ExceptionSet<AccessControl, ByteOrdering> ALL;
		
		typedef FieldSet< typename MSR::WE
		                , typename MSR::EE
		                , typename MSR::PR
		                , typename MSR::FP
		                , typename MSR::FE0
		                , typename MSR::DWE
		                , typename MSR::FE1
		                , typename MSR::IS
		                , typename MSR::DS > MSR_ALWAYS_CLEARED_FIELDS;

		static const char *GetName() { return "Instruction Storage Interrupt"; }
	};

	struct ExternalInputInterrupt : SuperCPU::template Interrupt<ExternalInputInterrupt, 4>
	{
		struct ExternalInput                        : SuperCPU::template Exception<ExternalInputInterrupt,7> { static const char *GetName() { return "External Input Interrupt/External Input Exception"; } };             // Interrupt Controller interrupt and MSR[EE] = 1
		
		typedef typename SuperCPU::template ExceptionSet<ExternalInput> ALL;
		
		typedef FieldSet< typename MSR::WE
		                , typename MSR::EE
		                , typename MSR::PR
		                , typename MSR::FP
		                , typename MSR::FE0
		                , typename MSR::DWE
		                , typename MSR::FE1
		                , typename MSR::IS
		                , typename MSR::DS > MSR_ALWAYS_CLEARED_FIELDS;
		
		static const char *GetName() { return "External Input Interrupt"; }
	};

	struct AlignmentInterrupt : SuperCPU::template InterruptWithAddress<AlignmentInterrupt, 5>
	{
		struct UnalignedIntegerLoadStore            : SuperCPU::template Exception<AlignmentInterrupt,15> { static const char *GetName() { return "Alignment Interrupt/Unaligned Integer Load/Store Exception"; } };                 // integer load/store
		struct UnalignedLoadStoreMultiple           : SuperCPU::template Exception<AlignmentInterrupt,16> { static const char *GetName() { return "Alignment Interrupt/Unaligned Load/Store Multiple Exception"; } };                // lmw, stmw not word aligned
		struct UnalignedFloatingPointLoadStore      : SuperCPU::template Exception<AlignmentInterrupt,17> { static const char *GetName() { return "Alignment Interrupt/Unaligned Floating-Point Load/Store Exception"; } };          // floating-point load/store
		struct UnalignedLoadLinkStoreConditional    : SuperCPU::template Exception<AlignmentInterrupt,18> { static const char *GetName() { return "Alignment Interrupt/Unaligned Load Link/Store Conditional Exception"; } };        // lwarx or stwcx. not word aligned, lharx or sthcx. not halfword aligned
		struct WriteThroughDCBZ                     : SuperCPU::template Exception<AlignmentInterrupt,19> { static const char *GetName() { return "Alignment Interrupt/Write Through DCBZ Exception"; } };                           // dcbz
		
		typedef typename SuperCPU::template ExceptionSet<UnalignedIntegerLoadStore, UnalignedLoadStoreMultiple, UnalignedFloatingPointLoadStore, UnalignedLoadLinkStoreConditional, WriteThroughDCBZ> ALL;
		
		typedef FieldSet< typename MSR::WE
		                , typename MSR::EE
		                , typename MSR::PR
		                , typename MSR::FP
		                , typename MSR::FE0
		                , typename MSR::DWE
		                , typename MSR::FE1
		                , typename MSR::IS
		                , typename MSR::DS > MSR_ALWAYS_CLEARED_FIELDS;

		static const char *GetName() { return "Alignment Interrupt"; }
	};
	
	struct ProgramInterrupt : SuperCPU::template Interrupt<ProgramInterrupt, 6>
	{
		struct IllegalInstruction                   : SuperCPU::template Exception<ProgramInterrupt,20>  { static const char *GetName() { return "Program Interrupt/Illegal Instruction Exception"; } };                   // illegal instruction
		struct PrivilegeViolation                   : SuperCPU::template Exception<ProgramInterrupt,21>  { static const char *GetName() { return "Program Interrupt/Privilege Violation Exception"; } };                   // privilege violation
		struct Trap                                 : SuperCPU::template Exception<ProgramInterrupt,22> { static const char *GetName() { return "Program Interrupt/Trap Exception"; } };                   // trap instruction
		struct FloatingPoint                        : SuperCPU::template Exception<ProgramInterrupt,23> { static const char *GetName() { return "Program Interrupt/Floating-point"; } };
		struct AuxiliaryProcessor                   : SuperCPU::template Exception<ProgramInterrupt,24> { static const char *GetName() { return "Program Interrupt/Auxiliary Processor"; } };
		struct UnimplementedInstruction             : SuperCPU::template Exception<ProgramInterrupt,25> { static const char *GetName() { return "Program Interrupt/Unimplemented Instruction Exception"; } };                   // unimplemented instruction
		
		typedef typename SuperCPU::template ExceptionSet<IllegalInstruction, PrivilegeViolation, Trap, FloatingPoint, UnimplementedInstruction> ALL;
		
		typedef FieldSet< typename MSR::WE
		                , typename MSR::EE
		                , typename MSR::PR
		                , typename MSR::FP
		                , typename MSR::FE0
		                , typename MSR::DWE
		                , typename MSR::FE1
		                , typename MSR::IS
		                , typename MSR::DS > MSR_ALWAYS_CLEARED_FIELDS;

		static const char *GetName() { return "Program Interrupt"; }
	};

	struct FloatingPointUnavailableInterrupt : SuperCPU::template Interrupt<FloatingPointUnavailableInterrupt, 7>
	{
		struct FloatingPointUnavailable : SuperCPU::template Exception<FloatingPointUnavailableInterrupt,26> { static const char *GetName() { return "Floating-point Unavailable Interrupt/Floating-point Unavailable Interrupt"; } };
		
		typedef typename SuperCPU::template ExceptionSet<FloatingPointUnavailable> ALL;
		
		typedef FieldSet< typename MSR::WE
		                , typename MSR::EE
		                , typename MSR::PR
		                , typename MSR::FP
		                , typename MSR::FE0
		                , typename MSR::DWE
		                , typename MSR::FE1
		                , typename MSR::IS
		                , typename MSR::DS > MSR_ALWAYS_CLEARED_FIELDS;
		
		static const char *GetName() { return "Floating-point Unavailable Interrupt"; }
	};
	
	struct SystemCallInterrupt : SuperCPU::template Interrupt<SystemCallInterrupt, 8>
	{
		struct SystemCall                           : SuperCPU::template Exception<SystemCallInterrupt,27> { static const char *GetName() { return "System Call Interrupt/System Call Exception"; } };                // Execution of the System Call (se_sc) instruction
		
		typedef typename SuperCPU::template ExceptionSet<SystemCall> ALL;
		
		typedef FieldSet< typename MSR::WE
		                , typename MSR::EE
		                , typename MSR::PR
		                , typename MSR::FP
		                , typename MSR::FE0
		                , typename MSR::DWE
		                , typename MSR::FE1
		                , typename MSR::IS
		                , typename MSR::DS > MSR_ALWAYS_CLEARED_FIELDS;
		
		static const char *GetName() { return "System Call Interrupt"; }
		
		SystemCallInterrupt() : elev(0) {}
		void SetELEV(unsigned int _elev) { elev = _elev; }
		unsigned int GetELEV() const { return elev; }
		void ClearELEV() { elev = 0; }
	private:
		unsigned int elev;
	};

	struct AuxiliaryProcessorUnavailableInterrupt : SuperCPU::template Interrupt<AuxiliaryProcessorUnavailableInterrupt, 9>
	{
		struct AuxiliaryProcessorUnavailable : SuperCPU::template Exception<AuxiliaryProcessorUnavailableInterrupt,28> { static const char *GetName() { return "Auxiliary Processor Unavailable Interrupt/Auxiliary Processor Unavailable Exception"; } };
		
		typedef typename SuperCPU::template ExceptionSet<AuxiliaryProcessorUnavailable> ALL;
		
		typedef FieldSet< typename MSR::WE
		                , typename MSR::EE
		                , typename MSR::PR
		                , typename MSR::FP
		                , typename MSR::FE0
		                , typename MSR::DWE
		                , typename MSR::FE1
		                , typename MSR::IS
		                , typename MSR::DS > MSR_ALWAYS_CLEARED_FIELDS;
		
		static const char *GetName() { return "Auxiliary Processor Unavailable Interrupt"; }
	};

	struct DecrementerInterrupt : SuperCPU::template Interrupt<DecrementerInterrupt, 10>
	{
		struct Decrementer : SuperCPU::template Exception<DecrementerInterrupt,9> { static const char *GetName() { return "Decrementer Interrupt/Decrementer Exception"; } };
		
		typedef typename SuperCPU::template ExceptionSet<Decrementer> ALL;
		
		typedef FieldSet< typename MSR::WE
		                , typename MSR::EE
		                , typename MSR::PR
		                , typename MSR::FP
		                , typename MSR::FE0
		                , typename MSR::DWE
		                , typename MSR::FE1
		                , typename MSR::IS
		                , typename MSR::DS > MSR_ALWAYS_CLEARED_FIELDS;
		
		static const char *GetName() { return "Decrementer Interrupt"; }
	};
	
	struct FixedIntervalTimerInterrupt : SuperCPU::template Interrupt<FixedIntervalTimerInterrupt, 11>
	{
		struct FixedIntervalTimer : SuperCPU::template Exception<FixedIntervalTimerInterrupt,8> { static const char *GetName() { return "Fixed Interval Timer Interrupt/Fixed Interval Timer Exception"; } };
		
		typedef typename SuperCPU::template ExceptionSet<FixedIntervalTimer> ALL;
		
		typedef FieldSet< typename MSR::WE
		                , typename MSR::EE
		                , typename MSR::PR
		                , typename MSR::FP
		                , typename MSR::FE0
		                , typename MSR::DWE
		                , typename MSR::FE1
		                , typename MSR::IS
		                , typename MSR::DS > MSR_ALWAYS_CLEARED_FIELDS;
		
		static const char *GetName() { return "Fixed Interval Timer Interrupt"; }
	};
	
	struct WatchdogTimerInterrupt : SuperCPU::template Interrupt<WatchdogTimerInterrupt, 12>
	{
		struct WatchdogTimer : SuperCPU::template Exception<WatchdogTimerInterrupt,6> { static const char *GetName() { return "Watchdog Timer Interrupt/Watchdog Timer Exception"; } };
		
		typedef typename SuperCPU::template ExceptionSet<WatchdogTimer> ALL;
		
		typedef FieldSet< typename MSR::WE
		                , typename MSR::EE
		                , typename MSR::PR
		                , typename MSR::FP
		                , typename MSR::FE0
		                , typename MSR::DWE
		                , typename MSR::FE1
		                , typename MSR::IS
		                , typename MSR::DS > MSR_ALWAYS_CLEARED_FIELDS;
		
		static const char *GetName() { return "Watchdog Timer Interrupt"; }
	};
	
	struct DataTLBErrorInterrupt : SuperCPU::template InterruptWithAddress<DataTLBErrorInterrupt, 13>
	{
		struct DataTLBError : SuperCPU::template Exception<DataTLBErrorInterrupt,29> { static const char *GetName() { return "Data TLB Error Interrupt/Data TLB Error Exception"; } };
		
		typedef typename SuperCPU::template ExceptionSet<DataTLBError> ALL;
		
		typedef FieldSet< typename MSR::WE
		                , typename MSR::EE
		                , typename MSR::PR
		                , typename MSR::FP
		                , typename MSR::FE0
		                , typename MSR::DWE
		                , typename MSR::FE1
		                , typename MSR::IS
		                , typename MSR::DS > MSR_ALWAYS_CLEARED_FIELDS;
		
		static const char *GetName() { return "Data TLB Error Interrupt"; }
		
		DataTLBErrorInterrupt& SetWrite(bool v) { write = v; return *this; }
		bool IsWrite() const { return write; }
	private:
		bool write;
	};

	struct InstructionTLBErrorInterrupt : SuperCPU::template InterruptWithAddress<InstructionTLBErrorInterrupt, 14>
	{
		struct InstructionTLBError : SuperCPU::template Exception<InstructionTLBErrorInterrupt,30> { static const char *GetName() { return "Instruction TLB Error Interrupt/Instruction TLB Error Exception"; } };
		
		typedef typename SuperCPU::template ExceptionSet<InstructionTLBError> ALL;
		
		typedef FieldSet< typename MSR::WE
		                , typename MSR::EE
		                , typename MSR::PR
		                , typename MSR::FP
		                , typename MSR::FE0
		                , typename MSR::DWE
		                , typename MSR::FE1
		                , typename MSR::IS
		                , typename MSR::DS > MSR_ALWAYS_CLEARED_FIELDS;
		
		static const char *GetName() { return "Instruction TLB Error Interrupt"; }
	};
	
	struct DebugInterrupt : SuperCPU::template Interrupt<DebugInterrupt, 15>
	{
		typedef typename SuperCPU::template Interrupt<DebugInterrupt, 15> Super;
		
		enum DebugEventType
		{
			DBG_UNCONDITIONAL_DEBUG_EVENT        = 0x00000001,
			DBG_INSTRUCTION_COMPLETE_DEBUG_EVENT = 0x00000002,
			DBG_BRANCH_TAKEN_DEBUG_EVENT         = 0x00000004,
			DBG_INTERRUPT_TAKEN_DEBUG_EVENT      = 0x00000008,
			DBG_TRAP_INSTRUCTION_DEBUG_EVENT     = 0x00000010,
			DBG_INSTRUCTION_ADDRESS_COMPARE      = 0x00000020,
			DBG_DATA_ADDRESS_COMPARE             = 0x00000040,
			DBG_DATA_VALUE_COMPARE               = 0x00000080
		};
		
		struct DebugEvent : SuperCPU::template Exception<DebugInterrupt,4> { static const char *GetName() { return "Debug Interrupt/Debug Event Exception"; } };
		
		typedef typename SuperCPU::template ExceptionSet<DebugEvent> ALL;
		
		typedef FieldSet< typename MSR::WE
		                , typename MSR::CE
		                , typename MSR::EE
		                , typename MSR::PR
		                , typename MSR::FP
		                , typename MSR::FE0
		                , typename MSR::DWE
		                , typename MSR::DE
		                , typename MSR::FE1
		                , typename MSR::IS
		                , typename MSR::DS > MSR_ALWAYS_CLEARED_FIELDS;
		
		DebugInterrupt() : Super(), dbg_events(0) {}
		bool GotEvent(DebugEventType dbg_event) const { return dbg_events & dbg_event; }
		DebugInterrupt& SetEvent(DebugEventType dbg_event) { dbg_events |= dbg_event; return *this; }
		void ClearEvents() { dbg_events = 0; }
		
		static const char *GetName() { return "Debug Interrupt"; }
	private:
		uint32_t dbg_events;
	};
	
	void ProcessInterrupt(CriticalInputInterrupt *);
	void ProcessInterrupt(MachineCheckInterrupt *);
	void ProcessInterrupt(DataStorageInterrupt *);
	void ProcessInterrupt(InstructionStorageInterrupt *);
	void ProcessInterrupt(ExternalInputInterrupt *);
	void ProcessInterrupt(AlignmentInterrupt *);
	void ProcessInterrupt(ProgramInterrupt *);
	void ProcessInterrupt(FloatingPointUnavailableInterrupt *);
	void ProcessInterrupt(SystemCallInterrupt *);
	void ProcessInterrupt(AuxiliaryProcessorUnavailableInterrupt *);
	void ProcessInterrupt(DecrementerInterrupt *);
	void ProcessInterrupt(FixedIntervalTimerInterrupt *);
	void ProcessInterrupt(WatchdogTimerInterrupt *);
	void ProcessInterrupt(DataTLBErrorInterrupt *);
	void ProcessInterrupt(InstructionTLBErrorInterrupt *);
	void ProcessInterrupt(DebugInterrupt *);
	
	void UpdateExceptionEnable();
	
	inline std::ostream& GetDebugInfoStream() ALWAYS_INLINE { return this->SuperCPU::GetDebugInfoStream(); }
	inline std::ostream& GetDebugWarningStream() ALWAYS_INLINE { return this->SuperCPU::GetDebugWarningStream(); }
	inline std::ostream& GetDebugErrorStream() ALWAYS_INLINE { return this->SuperCPU::GetDebugErrorStream(); }

	inline bool IsVerboseDataLoad() const ALWAYS_INLINE { return verbose_data_load; }
	inline bool IsVerboseDataStore() const ALWAYS_INLINE { return verbose_data_store; }
	inline bool IsVerboseInstructionFetch() const ALWAYS_INLINE { return verbose_instruction_fetch; }
	inline bool IsVerboseDataBusRead() const ALWAYS_INLINE { return verbose_data_bus_read; }
	inline bool IsVerboseDataBusWrite() const ALWAYS_INLINE { return verbose_data_bus_write; }
	inline bool IsVerboseInstructionBusRead() const ALWAYS_INLINE { return verbose_instruction_bus_read; }
	bool IsStorageCacheable(STORAGE_ATTR storage_attr) const { return !(storage_attr & TYPES::SA_I); }

	bool DataBusRead(PHYSICAL_ADDRESS addr, void *buffer, unsigned int size, STORAGE_ATTR storage_attr, bool rwitm);
	bool DataBusWrite(PHYSICAL_ADDRESS addr, const void *buffer, unsigned int size, STORAGE_ATTR storage_attr);
	bool InstructionBusRead(PHYSICAL_ADDRESS addr, void *buffer, unsigned int size, STORAGE_ATTR storage_attr);

	bool DebugDataBusRead(PHYSICAL_ADDRESS addr, void *buffer, unsigned int size, STORAGE_ATTR storage_attr);
	bool DebugDataBusWrite(PHYSICAL_ADDRESS addr, const void *buffer, unsigned int size, STORAGE_ATTR storage_attr);
	bool DebugInstructionBusRead(PHYSICAL_ADDRESS addr, void *buffer, unsigned int size, STORAGE_ATTR storage_attr);

	virtual BusResponseStatus PLBInsnRead(PHYSICAL_ADDRESS physical_addr, void *buffer, uint32_t size, STORAGE_ATTR storage_attr);
	virtual BusResponseStatus PLBDataRead(PHYSICAL_ADDRESS physical_addr, void *buffer, uint32_t size, STORAGE_ATTR storage_attr, bool rwitm);
	virtual BusResponseStatus PLBDataWrite(PHYSICAL_ADDRESS physical_addr, const void *buffer, uint32_t size, STORAGE_ATTR storage_attr);

	virtual bool PLBDebugInsnRead(PHYSICAL_ADDRESS physical_addr, void *buffer, uint32_t size, STORAGE_ATTR storage_attr);
	virtual bool PLBDebugDataRead(PHYSICAL_ADDRESS physical_addr, void *buffer, uint32_t size, STORAGE_ATTR storage_attr);
	virtual bool PLBDebugDataWrite(PHYSICAL_ADDRESS physical_addr, const void *buffer, uint32_t size, STORAGE_ATTR storage_attr);

	template <typename T, bool REVERSE, bool FORCE_BIG_ENDIAN> void ConvertDataLoadStoreEndian(T& value, STORAGE_ATTR storage_attr);
	
// 	bool DebugDataLoad(EFFECTIVE_ADDRESS ea, void *buffer, unsigned int size);
// 	bool DebugDataStore(EFFECTIVE_ADDRESS ea, const void *buffer, unsigned int size);

	virtual void InvalidateDirectMemPtr(PHYSICAL_ADDRESS start_addr, PHYSICAL_ADDRESS end_addr) {}

// 	template <bool DEBUG, bool EXEC, bool WRITE>
// 	inline bool Translate(EFFECTIVE_ADDRESS ea, EFFECTIVE_ADDRESS& size_to_protection_boundary, ADDRESS& virt_addr, PHYSICAL_ADDRESS& phys_addr, STORAGE_ATTR& storage_attr);

	void RunTimers(uint64_t delta);
	uint64_t GetMaxIdleTime() const;
	uint64_t GetTimersDeadline() const;
	
	bool CheckInt16LoadAlignment(EFFECTIVE_ADDRESS ea);
	bool CheckInt32LoadAlignment(EFFECTIVE_ADDRESS ea);
	bool CheckInt16StoreAlignment(EFFECTIVE_ADDRESS ea);
	bool CheckInt32StoreAlignment(EFFECTIVE_ADDRESS ea);

	bool Fp32Load(unsigned int fd, EFFECTIVE_ADDRESS ea);
	bool Fp64Load(unsigned int fd, EFFECTIVE_ADDRESS ea);
	bool Fp32Store(unsigned int fs, EFFECTIVE_ADDRESS ea);
	bool Fp64Store(unsigned int fs, EFFECTIVE_ADDRESS ea);
	bool FpStoreLSW(unsigned int fs, EFFECTIVE_ADDRESS ea);
	
	int StringLength(EFFECTIVE_ADDRESS addr);
	std::string ReadString(EFFECTIVE_ADDRESS addr, unsigned int count = 0);

public:

	void FlushInstructionBuffer();
	bool InstructionFetch(EFFECTIVE_ADDRESS addr, typename CONFIG::CODE_TYPE& insn);
	void StepOneInstruction();
	
protected:
	/////////////////////////////// Statistics ////////////////////////////////
	
	unisim::kernel::service::Statistic<uint64_t> stat_num_data_load_accesses;
	unisim::kernel::service::Statistic<uint64_t> stat_num_data_store_accesses;
	unisim::kernel::service::Statistic<uint64_t> stat_num_instruction_fetch_accesses;
	unisim::kernel::service::Statistic<uint64_t> stat_num_incoming_load_accesses;
	unisim::kernel::service::Statistic<uint64_t> stat_num_incoming_store_accesses;
	unisim::kernel::service::Statistic<uint64_t> stat_num_data_bus_read_accesses;
	unisim::kernel::service::Statistic<uint64_t> stat_num_data_bus_write_accesses;
	unisim::kernel::service::Statistic<uint64_t> stat_num_instruction_bus_read_accesses;
	unisim::kernel::service::Statistic<uint64_t> stat_num_data_load_xfered_bytes;
	unisim::kernel::service::Statistic<uint64_t> stat_num_data_store_xfered_bytes;
	unisim::kernel::service::Statistic<uint64_t> stat_num_instruction_fetch_xfered_bytes;
	unisim::kernel::service::Statistic<uint64_t> stat_num_incoming_load_xfered_bytes;
	unisim::kernel::service::Statistic<uint64_t> stat_num_incoming_store_xfered_bytes;
	unisim::kernel::service::Statistic<uint64_t> stat_num_data_bus_read_xfered_bytes;
	unisim::kernel::service::Statistic<uint64_t> stat_num_data_bus_write_xfered_bytes;
	unisim::kernel::service::Statistic<uint64_t> stat_num_instruction_bus_read_xfered_bytes;

	////////////////////////// Run-time parameters ////////////////////////////
	
	uint8_t cpuid;
	unisim::kernel::service::Parameter<uint8_t> param_cpuid;

	uint32_t processor_version;
	unisim::kernel::service::Parameter<uint32_t> param_processor_version;
	
	bool verbose_data_load;
	unisim::kernel::service::Parameter<bool> param_verbose_data_load;
	
	bool verbose_data_store;
	unisim::kernel::service::Parameter<bool> param_verbose_data_store;
	
	bool verbose_instruction_fetch;
	unisim::kernel::service::Parameter<bool> param_verbose_instruction_fetch;
	
	bool verbose_data_bus_read;
	unisim::kernel::service::Parameter<bool> param_verbose_data_bus_read;
	
	bool verbose_data_bus_write;
	unisim::kernel::service::Parameter<bool> param_verbose_data_bus_write;
	
	bool verbose_instruction_bus_read;
	unisim::kernel::service::Parameter<bool> param_verbose_instruction_bus_read;
	
	bool trap_critical_input_interrupt;
	unisim::kernel::service::Parameter<bool> param_trap_critical_input_interrupt;

	bool trap_machine_check_interrupt;
	unisim::kernel::service::Parameter<bool> param_trap_machine_check_interrupt;
	
	bool trap_data_storage_interrupt;
	unisim::kernel::service::Parameter<bool> param_trap_data_storage_interrupt;
	
	bool trap_instruction_storage_interrupt;
	unisim::kernel::service::Parameter<bool> param_trap_instruction_storage_interrupt;
	
	bool trap_external_input_interrupt;
	unisim::kernel::service::Parameter<bool> param_trap_external_input_interrupt;

	bool trap_alignment_interrupt;
	unisim::kernel::service::Parameter<bool> param_trap_alignment_interrupt;
	
	bool trap_program_interrupt;
	unisim::kernel::service::Parameter<bool> param_trap_program_interrupt;
	
	bool trap_floating_point_unavailable_interrupt;
	unisim::kernel::service::Parameter<bool> param_trap_floating_point_unavailable_interrupt;
	
	bool trap_system_call_interrupt;
	unisim::kernel::service::Parameter<bool> param_trap_system_call_interrupt;
	
	bool trap_auxiliary_processor_unavailable_interrupt;
	unisim::kernel::service::Parameter<bool> param_trap_auxiliary_processor_unavailable_interrupt;
	
	bool trap_decrementer_interrupt;
	unisim::kernel::service::Parameter<bool> param_trap_decrementer_interrupt;
	
	bool trap_fixed_interval_timer_interrupt;
	unisim::kernel::service::Parameter<bool> param_trap_fixed_interval_timer_interrupt;
	
	bool trap_watchdog_interrupt;
	unisim::kernel::service::Parameter<bool> param_trap_watchdog_interrupt;
	
	bool trap_data_tlb_error_interrupt;
	unisim::kernel::service::Parameter<bool> param_trap_data_tlb_error_interrupt;
	
	bool trap_instruction_tlb_error_interrupt;
	unisim::kernel::service::Parameter<bool> param_trap_instruction_tlb_error_interrupt;
	
	bool trap_debug_interrupt;
	unisim::kernel::service::Parameter<bool> param_trap_debug_interrupt;
	
	bool enable_linux_syscall_snooping;
	unisim::kernel::service::Parameter<bool> param_enable_linux_syscall_snooping;
	
	////////////////////////// Instruction Buffer /////////////////////////////

	EFFECTIVE_ADDRESS instruction_buffer_base_addr;
	uint32_t instruction_buffer[CONFIG::INSTRUCTION_BUFFER_SIZE / 4];
	
	/////////////////////// VLE Instruction decoder ///////////////////////////
	
	typename CONFIG::DECODER decoder;
	typename CONFIG::OPERATION *operation;
	
	/////////////////////////////// Registers /////////////////////////////////
	
	// cache debug
	typename SuperCPU::DCDBTRH dcdbtrh;
	typename SuperCPU::DCDBTRL dcdbtrl;
	typename SuperCPU::ICDBDR  icdbdr;
	typename SuperCPU::ICDBTRH icdbtrh;
	typename SuperCPU::ICDBTRL icdbtrl;
	
	// debug
	typename SuperCPU::DAC1 dac1;
	typename SuperCPU::DAC2 dac2;
	typename SuperCPU::DBCR0 dbcr0;
	typename SuperCPU::DBCR1 dbcr1;
	typename SuperCPU::DBCR2 dbcr2;
	typename SuperCPU::DBDR dbdr;
	typename SuperCPU::DBSR dbsr;
	typename SuperCPU::DVC1 dvc1;
	typename SuperCPU::DVC2 dvc2;
	typename SuperCPU::IAC1 iac1;
	typename SuperCPU::IAC2 iac2;
	typename SuperCPU::IAC3 iac3;
	typename SuperCPU::IAC4 iac4;
	
	// floating point
	FPR fpr[32];   // floating point registers (C++ objects implementing IEEE 754 floating point numbers)
	FPSCR fpscr;   // floating point status and control register
	
	// interrupt processing
	typename SuperCPU::CSRR0 csrr0;
	typename SuperCPU::CSRR1 csrr1;
	typename SuperCPU::DEAR dear;
	typename SuperCPU::ESR esr;
	typename SuperCPU::IVOR0 ivor0;
	typename SuperCPU::IVOR1 ivor1;
	typename SuperCPU::IVOR2 ivor2;
	typename SuperCPU::IVOR3 ivor3;
	typename SuperCPU::IVOR4 ivor4;
	typename SuperCPU::IVOR5 ivor5;
	typename SuperCPU::IVOR6 ivor6;
	typename SuperCPU::IVOR7 ivor7;
	typename SuperCPU::IVOR8 ivor8;
	typename SuperCPU::IVOR9 ivor9;
	typename SuperCPU::IVOR10 ivor10;
	typename SuperCPU::IVOR11 ivor11;
	typename SuperCPU::IVOR12 ivor12;
	typename SuperCPU::IVOR13 ivor13;
	typename SuperCPU::IVOR14 ivor14;
	typename SuperCPU::IVOR15 ivor15;
	typename SuperCPU::IVPR ivpr;
	typename SuperCPU::MCSR mcsr;
	typename SuperCPU::MCSRR0 mcsrr0;
	typename SuperCPU::MCSRR1 mcsrr1;
	typename SuperCPU::SRR0 srr0;
	typename SuperCPU::SRR1 srr1;
	
	// processor control
	typename SuperCPU::CCR0 ccr0;
	typename SuperCPU::CCR1 ccr1;
	MSR msr;
	typename SuperCPU::PIR pir;
	typename SuperCPU::PVR pvr;
	typename SuperCPU::RSTCFG rstcfg;
	typename SuperCPU::SPRG0 sprg0;
	typename SuperCPU::SPRG1 sprg1;
	typename SuperCPU::SPRG2 sprg2;
	typename SuperCPU::SPRG3 sprg3;
	typename SuperCPU::SPRG4 sprg4;
	typename SuperCPU::SPRG5 sprg5;
	typename SuperCPU::SPRG6 sprg6;
	typename SuperCPU::SPRG7 sprg7;
	typename SuperCPU::USPRG0 usprg0;
	
	// storage control
	typename SuperCPU::MMUCR mmucr;
	typename SuperCPU::PID0 pid;
	
	// timer
	typename SuperCPU::DEC dec;
	typename SuperCPU::DECAR decar;
	TB tb;
	TCR tcr;
	TSR tsr;
};

} // end of namespace book_e
} // end of namespace powerpc
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim


#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_BOOK_E_CPU_HH__
