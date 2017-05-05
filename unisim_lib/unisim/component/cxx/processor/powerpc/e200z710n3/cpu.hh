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

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_E200Z710N3_CPU_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_E200Z710N3_CPU_HH__

#include <unisim/component/cxx/processor/powerpc/cpu.tcc>
#include <unisim/component/cxx/processor/powerpc/e200z710n3/mpu.hh>
#include <unisim/component/cxx/processor/powerpc/e200z710n3/types.hh>
#include <unisim/component/cxx/processor/powerpc/e200z710n3/isa/vle.hh>
#include <unisim/util/cache/cache.hh>
#include <unisim/util/endian/endian.hh>
#include <unisim/util/queue/queue.hh>
#include <unisim/util/queue/queue.tcc>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace powerpc {
namespace e200z710n3 {

class CPU
	: public unisim::component::cxx::processor::powerpc::CPU<CONFIG>
	, public unisim::util::cache::MemorySubSystem<MSS_TYPES, CPU >
	, public unisim::kernel::service::Client<unisim::service::interfaces::Memory<PHYSICAL_ADDRESS> >
	, public unisim::kernel::service::Service<unisim::service::interfaces::Disassembly<ADDRESS> >
	, public unisim::kernel::service::Service<unisim::service::interfaces::Memory<ADDRESS> >
{
public:
	typedef CPU ThisCPU;
	typedef unisim::component::cxx::processor::powerpc::CPU<CONFIG> SuperCPU;
	typedef unisim::util::cache::MemorySubSystem<MSS_TYPES, CPU> SuperMSS;
	
	/////////////////////////// service imports ///////////////////////////////
	
	unisim::kernel::service::ServiceImport<unisim::service::interfaces::Memory<PHYSICAL_ADDRESS> > memory_import;

	/////////////////////////// service exports ///////////////////////////////

	unisim::kernel::service::ServiceExport<unisim::service::interfaces::Disassembly<ADDRESS> > disasm_export;
	unisim::kernel::service::ServiceExport<unisim::service::interfaces::Memory<ADDRESS> > memory_export;

	////////////////////////////// constructor ////////////////////////////////
	
	CPU(const char *name, unisim::kernel::service::Object *parent = 0);

	/////////////////////////////// destructor ////////////////////////////////

	virtual ~CPU();
	
	////////////////////////////// setup hooks ////////////////////////////////

	virtual bool EndSetup();

	//////////  unisim::service::interfaces::Disassembly<> ////////////////////
	
	virtual std::string Disasm(ADDRESS addr, ADDRESS& next_addr);
	
	/////////////// unisim::service::interfaces::Memory<> /////////////////////
	
	virtual void Reset();
	virtual bool ReadMemory(ADDRESS addr, void *buffer, uint32_t size);
	virtual bool WriteMemory(ADDRESS addr, const void *buffer, uint32_t size);
	
	///////////////// Interface with SystemC TLM-2.0 wrapper module ///////////
	
	virtual void InterruptAcknowledge() {}
	virtual void Idle() {}
	
	////////////////////////// Machine State Register /////////////////////////
	
	struct MSR : SuperCPU::MSR
	{
		typedef SuperCPU::MSR Super;
		
		MSR(CPU *_cpu) : Super(_cpu) {}
		MSR(CPU *_cpu, uint32_t _value) : Super(_cpu, _value) {}
		
		MSR& operator = (const uint32_t& value)
		{
			this->Super::operator = (value);
			cpu->UpdateExceptionEnable();
			return *this;
		}
	};

	///////////// Interface with .isa behavioral description files ////////////
	
	MSR& GetMSR() { return msr; }
	ESR& GetESR() { return esr; }

	bool Dcba(ADDRESS addr);
	bool Dcbi(ADDRESS addr);
	bool Dcbf(ADDRESS addr);
	bool Dcbst(ADDRESS addr);
	bool Dcbt(ADDRESS addr);
	bool Dcbtst(ADDRESS addr);
	bool Dcbz(ADDRESS addr);
	bool Icbi(ADDRESS addr);
	bool Icbt(ADDRESS addr);
	bool Lbarx(unsigned int rd, ADDRESS addr);
	bool Lharx(unsigned int rd, ADDRESS addr);
	bool Lwarx(unsigned int rd, ADDRESS addr);
	bool Mbar(ADDRESS addr);
	bool Stbcx(unsigned int rs, ADDRESS addr);
	bool Sthcx(unsigned int rs, ADDRESS addr);
	bool Stwcx(unsigned int rs, ADDRESS addr);
	bool Wait();
	bool Msync();
	bool Isync();
	bool Rfi();
	bool Rfci();
	bool Rfdi();
	bool Rfmci();
	bool Mpure();
	bool Mpuwe();
	bool Mpusync();
	
	////////////////////////// Special Purpose Registers //////////////////////
	
	// Processor ID Register
	struct PIR : SuperCPU::PIR
	{
		typedef SuperCPU::PIR Super;
		
		PIR(CPU *_cpu) : Super(_cpu) {}
		PIR(CPU *_cpu, uint32_t _value) : Super(_cpu, _value) {}
		
		virtual void Reset()
		{
			Set<ID_0_23>(0);
			Set<ID_24_31>(cpu->cpuid);
		}
	};

	// Hardware Implementation Dependent Register 0
	struct HID0 : SuperCPU::HID0
	{
		typedef SuperCPU::HID0 Super;
		
		HID0(CPU *_cpu) : Super(_cpu) {}
		HID0(CPU *_cpu, uint32_t _value) : Super(_cpu, _value) {}
		
		void Effect()
		{
			cpu->UpdateExceptionEnable();
		}
		
		HID0& operator = (const uint32_t& value) { this->Super::operator = (value); Effect(); return *this; }
		virtual bool MoveTo(uint32_t value) { if(!this->Super::MoveTo(value)) return false; Effect(); return true; }
	};
	
	// L1 Cache Control and Status Register 0
	struct L1CSR0 : SuperCPU::L1CSR0
	{
		typedef SuperCPU::L1CSR0 Super;
		
		L1CSR0(CPU *_cpu) : Super(_cpu) {}
		L1CSR0(CPU *_cpu, uint32_t _value) : Super(_cpu, _value) {}
		
		void Effect()
		{
			if(HAS_DATA_CACHE)
			{
				if(cpu->verbose_data_cache)
				{
					cpu->GetDebugInfoStream() << "Data Cache is " << (Get<L1CSR0::DCE>() ? "enabled" : "disabled") << std::endl;
					cpu->GetDebugInfoStream() << "Data Cache Write Allocation Policy is " << (Get<L1CSR0::DCWA>() ? "enabled" : "disabled") << std::endl;
					cpu->GetDebugInfoStream() << "Replacement of Instruction Cache Way #0 is " << ((Get<L1CSR0::WID>() & 1) ? "disabled" : "enabled") << std::endl;
					cpu->GetDebugInfoStream() << "Replacement of Instruction Cache Way #1 is " << ((Get<L1CSR0::WID>() & 2) ? "disabled" : "enabled") << std::endl;
					cpu->GetDebugInfoStream() << "Replacement of Data Cache Way #0 is " << ((Get<L1CSR0::WDD>() & 1) ? "disabled" : "enabled") << std::endl;
					cpu->GetDebugInfoStream() << "Replacement of Data Cache Way #1 is " << ((Get<L1CSR0::WDD>() & 2) ? "disabled" : "enabled") << std::endl;
				}
				if(Get<L1CSR0::DCINV>())
				{
					cpu->InvalidateDataCache();
					Set<L1CSR0::DCINV>(0);
				}
			}
		}
		
		L1CSR0& operator = (const uint32_t& value) { this->Super::operator = (value); Effect(); return *this; }
		virtual bool MoveTo(uint32_t value) { if(!this->Super::MoveTo(value)) return false; Effect(); return true; }
	};
	
	// L1 Cache Control and Status Register 1
	struct L1CSR1 : SuperCPU::L1CSR1
	{
		typedef SuperCPU::L1CSR1 Super;
		
		L1CSR1(CPU *_cpu) : Super(_cpu) {}
		L1CSR1(CPU *_cpu, uint32_t _value) : Super(_cpu, _value) {}
		
		void Effect()
		{
			if(cpu->verbose_instruction_cache)
			{
				cpu->GetDebugInfoStream() << "Instruction Cache is " << (Get<L1CSR1::ICE>() ? "enabled" : "disabled") << std::endl;
			}
			if(Get<L1CSR1::ICINV>())
			{
				cpu->InvalidateInstructionCache();
				Set<L1CSR1::ICINV>(0);
			}
		}
		
		L1CSR1& operator = (const uint32_t& value) { this->Super::operator = (value); Effect(); return *this; }
		virtual bool MoveTo(uint32_t value) { if(!this->Super::MoveTo(value)) return false; Effect(); return true; }
	};

	// L1 Cache Configuration Register 0
	struct L1CFG0 : SuperCPU::L1CFG0
	{
		typedef SuperCPU::L1CFG0 Super;
		
		L1CFG0(CPU *_cpu) : Super(_cpu) {}
		L1CFG0(CPU *_cpu, uint32_t _value) : Super(_cpu, _value) {}
		
		virtual void Reset()
		{
			if(HAS_DATA_CACHE)
			{
				Set<CARCH>(CARCH_HARVARD);
				Set<CWPA>(1);
				Set<DCFAHA>(0);
				Set<DCFISWA>(1);
				Set<DCBSIZE>(CEIL_LOG2<L1D::BLOCK_SIZE>::VALUE - 5);
				Set<DCREPL>(CREPL_FIFO);
				Set<DCLA>(0);
				Set<DCECA>(1);
				Set<DCNWAY>(L1D::ASSOCIATIVITY - 1);
				Set<DCSIZE>(L1D::SIZE / 1024);
			}
			else
			{
				Set<CARCH>(CARCH_INSTRUCTION_ONLY);
				Set<CWPA>(0);
				Set<DCFAHA>(0);
				Set<DCFISWA>(0);
				Set<DCBSIZE>(0);
				Set<DCREPL>(0);
				Set<DCLA>(0);
				Set<DCECA>(0);
				Set<DCNWAY>(0);
				Set<DCSIZE>(0);
			}
		}
	};
	
	// L1 Cache Configuration Register 1
	struct L1CFG1 : SuperCPU::L1CFG1
	{
		typedef SuperCPU::L1CFG1 Super;
		
		L1CFG1(CPU *_cpu) : Super(_cpu) {}
		L1CFG1(CPU *_cpu, uint32_t _value) : Super(_cpu, _value) {}
		
		virtual void Reset()
		{
			Set<ICFISWA>(1);  // The instruction cache supports invalidation by Set and Way via L1FINV1
			Set<ICBSIZE>(CEIL_LOG2<L1I::BLOCK_SIZE>::VALUE - 5);  // The instruction cache implements a block size of 32 bytes
			Set<ICREPL>(CREPL_FIFO);   // The instruction cache implements a FIFO replacement policy
			Set<ICLA>(0);     // The instruction cache does not implement the line locking APU
			Set<ICECA>(1);    // The instruction cache implements error checking
			Set<ICNWAY>(L1I::ASSOCIATIVITY - 1);   // The instruction cache is 2-way set-associative
			Set<ICSIZE>(L1I::SIZE / 1024);         // The size of the instruction cache is 16 KB
		}
	};
	
	// L1 Flush and Invalidate Control Register 0
	struct L1FINV0 : SuperCPU::L1FINV0
	{
		typedef SuperCPU::L1FINV0 Super;
		
		L1FINV0(CPU *_cpu) : Super(_cpu) {}
		L1FINV0(CPU *_cpu, uint32_t _value) : Super(_cpu, _value) {}
		
		virtual bool CheckMoveToLegality()
		{
			if(HAS_DATA_CACHE)
			{
				return Super::CheckMoveToLegality();
			}
			
			// Illegal Instruction
			if(cpu->verbose_move_to_slr)
			{
				cpu->GetDebugWarningStream() << "Move to " << this->GetName() << " is an illegal operation (no data cache present)" << std::endl;
			}
			cpu->ThrowException<CONFIG::STATE::ProgramInterrupt::IllegalInstruction>();
			return false;
		}
		
		virtual bool CheckMoveFromLegality()
		{
			if(HAS_DATA_CACHE)
			{
				return Super::CheckMoveFromLegality();
			}
			
			// Illegal Instruction
			if(cpu->verbose_move_to_slr)
			{
				cpu->GetDebugWarningStream() << "Move to " << this->GetName() << " is an illegal operation (no data cache present)" << std::endl;
			}
			cpu->ThrowException<CONFIG::STATE::ProgramInterrupt::IllegalInstruction>();
			return false;
		}

		void Effect()
		{
			switch(Get<CCMD>())
			{
				case 0x5:
					cpu->ClearDataCacheLockoutBySetAndWay(Get<CSET>(), Get<CWAY>());
					break;
				case 0x0:
					cpu->InvalidateDataCacheBySetAndWay(Get<CSET>(), Get<CWAY>());
					break;
				default:
					cpu->GetDebugWarningStream() << "Cache Command is invalid, see " << GetName() << " content below for detail:" << std::endl;
					LongPrettyPrint(cpu->GetDebugWarningStream());
					cpu->GetDebugWarningStream() << std::endl;
					break;
			}
		}
		
		L1FINV0& operator = (const uint32_t& value) { if(HAS_DATA_CACHE) { this->Super::operator = (value); Effect(); } return *this; }
		virtual bool MoveTo(uint32_t value) { if(!this->Super::MoveTo(value)) return false; Effect(); return true; }
	};
	
	// L1 Flush and Invalidate Control Register 1
	struct L1FINV1 : SuperCPU::L1FINV1
	{
		typedef SuperCPU::L1FINV1 Super;
		
		L1FINV1(CPU *_cpu) : Super(_cpu) {}
		L1FINV1(CPU *_cpu, uint32_t _value) : Super(_cpu, _value) {}
		
		void Effect()
		{
			switch(Get<CCMD>())
			{
				case 0x5:
					cpu->ClearInstructionCacheLockoutBySetAndWay(Get<CSET>(), Get<CWAY>());
					break;
				case 0x0:
					cpu->InvalidateInstructionCacheBySetAndWay(Get<CSET>(), Get<CWAY>());
					break;
				default:
					cpu->GetDebugWarningStream() << "Cache Command is invalid, see " << GetName() << " content below for detail:" << std::endl;
					LongPrettyPrint(cpu->GetDebugWarningStream());
					cpu->GetDebugWarningStream() << std::endl;
					break;
			}
		}
		
		L1FINV1& operator = (const uint32_t& value) { this->Super::operator = (value); Effect(); return *this; }
		virtual bool MoveTo(uint32_t value) { if(!this->Super::MoveTo(value)) return false; Effect(); return true; }
	};

	// DMEM Configuration Register 0
	struct DMEMCFG0 : SuperCPU::DMEMCFG0
	{
		typedef SuperCPU::DMEMCFG0 Super;
		
		DMEMCFG0(CPU *_cpu) : Super(_cpu) {}
		DMEMCFG0(CPU *_cpu, uint32_t _value) : Super(_cpu, _value) {}
		
		virtual void Reset()
		{
			Set<DMEM_BASE_ADDR>(DMEM_BASE_ADDR::Get(cpu->cur_dmem_base_addr));
			Set<DECUA>(1);
			Set<DECA>(1);
			Set<DMSIZE>(cpu->dmem_size / 4096);
		}
	};
	
	// DMEM Control Register 0
	struct DMEMCTL0 : SuperCPU::DMEMCTL0
	{
		typedef SuperCPU::DMEMCTL0 Super;
		
		DMEMCTL0(CPU *_cpu) : Super(_cpu) {}
		DMEMCTL0(CPU *_cpu, uint32_t _value) : Super(_cpu, _value) {}
		
		void Effect()
		{
			if(Get<DBAPD>())
			{
				cpu->cur_dmem_base_addr = 0; // clear lower bits
				DMEM_BASE_ADDR::Set<ADDRESS>(cpu->cur_dmem_base_addr, Get<DMEM_BASE_ADDR>()); // copy higher bits
			}
			else
			{
				cpu->cur_dmem_base_addr = cpu->dmem_base_addr;
			}
			cpu->cur_dmem_high_addr = cpu->cur_dmem_base_addr + cpu->dmem_size;
			
			cpu->dmemcfg0.Reset(); // regenerate DMEMCFG0 read-only register
		}
		
		virtual void Reset()
		{
			Set<DMEM_BASE_ADDR>(0);
			Set<DBYPCB        >(0);
			Set<DBYPAT        >(0);
			Set<DBYPDEC       >(1);
			Set<DECUE         >(0);
			Set<DBAPD         >(0);
			Set<DPEIE         >(0);
			Set<DICWE         >(0);
			Set<DSWCE         >(3);
			Set<DDAUEC        >(0);
			Set<DCPECE        >(1);
			Set<DSECE         >(1);
			
			Effect();
		}
		
		DMEMCTL0& operator = (const uint32_t& value) { this->Super::operator = (value); Effect(); return *this; }
		virtual bool MoveTo(uint32_t value) { if(!this->Super::MoveTo(value)) return false; Effect(); return true; }
	};
	
	// IMEM Configuration Register 0
	struct IMEMCFG0 : SuperCPU::IMEMCFG0
	{
		typedef SuperCPU::IMEMCFG0 Super;
		
		IMEMCFG0(CPU *_cpu) : Super(_cpu) {}
		IMEMCFG0(CPU *_cpu, uint32_t _value) : Super(_cpu, _value) {}
		
		virtual void Reset()
		{
			Set<IMEM_BASE_ADDR>(IMEM_BASE_ADDR::Get(cpu->cur_imem_base_addr));
			Set<IECUA>(1);
			Set<IECA>(1);
			Set<IMSIZE>(cpu->imem_size / 4096);
		}
	};
	
	// IMEM Control Register 0
	struct IMEMCTL0 : SuperCPU::IMEMCTL0
	{
		typedef SuperCPU::IMEMCTL0 Super;
		
		IMEMCTL0(CPU *_cpu) : Super(_cpu) {}
		IMEMCTL0(CPU *_cpu, uint32_t _value) : Super(_cpu, _value) {}
		
		void Effect()
		{
			if(Get<IBAPD>())
			{
				cpu->cur_imem_base_addr = 0; // clear lower bits
				IMEM_BASE_ADDR::Set<ADDRESS>(cpu->cur_imem_base_addr, Get<IMEM_BASE_ADDR>()); // copy higher bits
			}
			else
			{
				cpu->cur_imem_base_addr = cpu->imem_base_addr;
			}
			cpu->cur_imem_high_addr = cpu->cur_imem_base_addr + cpu->imem_size;
			
			cpu->imemcfg0.Reset(); // regenerate IMEMCFG0 read-only register
		}
		
		virtual void Reset()
		{
			Set<IMEM_BASE_ADDR>(0);
			Set<IECUE         >(0);
			Set<IBAPD         >(0);
			Set<ISWCE         >(3);
			Set<IDAUEC        >(0);
			Set<ICPECE        >(1);
			Set<ISECE         >(1);
			
			Effect();
		}
		
		IMEMCTL0& operator = (const uint32_t& value) { this->Super::operator = (value); Effect(); return *this; }
		virtual bool MoveTo(uint32_t value) { if(!this->Super::MoveTo(value)) return false; Effect(); return true; }
	};

	//////////////////////////////// Interrupts ///////////////////////////////
	
	// Priorities:
	//  0   SystemResetInterrupt                 Reset
	//  1   MachineCheckInterrupt                NMI
	//  2   MachineCheckInterrupt                ErrorReport
	//  3   MachineCheckInterrupt                AsynchronousMachineCheck
	//  4   ExternalInputInterrupt               ExternalInput
	//  5   CriticalInputInterrupt               CriticalInput
	//  6   DebugInterrupt                       AsynchronousDebugEvent
	//  7   PerformanceMonitorInterrupt          PerformanceCounterOverflow
	//  8   PerformanceMonitorInterrupt          DebugEvent
	//  9   InstructionStorageInterrupt          AccessControl
	// 10   ProgramInterrupt                     IllegalInstruction
	// 11   ProgramInterrupt                     PrivilegeViolation
	// 12   ProgramInterrupt                     Trap
	// 13   ProgramInterrupt                     UnimplementedInstruction
	// 14   SystemCallInterrupt                  SystemCall
	// 15   DataStorageInterrupt                 AccessControl
	// 16   AlignmentInterrupt                   UnalignedLoadStoreMultiple
	// 17   AlignmentInterrupt                   UnalignedLoadLinkStoreConditional
	// 18   AlignmentInterrupt                   WriteThroughDCBZ
	// 19   EmbeddedFloatingPointDataInterrupt   EmbeddedFloatingPointData
	// 20   EmbeddedFloatingPointRoundInterrupt  EmbeddedFloatingPointRound
	// 21   DebugInterrupt                       SynchronousDebugEvent
	
	struct SystemResetInterrupt : Interrupt<SystemResetInterrupt, 0x00 /* p_rstbase[0:29] */>
	{
		struct Reset                             : Exception<SystemResetInterrupt, 0> { static const char *GetName() { return "System Reset Interrupt/Reset Exception"; } };                  // reset
		
		typedef ExceptionSet<Reset> ALL;
		
		typedef FieldSet< MSR::SPV
		                , MSR::WE
		                , MSR::CE
		                , MSR::EE
		                , MSR::PR
		                , MSR::FP
		                , MSR::ME
		                , MSR::FE0
		                , MSR::DE
		                , MSR::FE1
		                , MSR::IS
		                , MSR::DS
		                , MSR::PMM
		                , MSR::RI > MSR_CLEARED_FIELDS;
		
		static const char *GetName() { return "System Reset Interrupt"; }
	};

	struct CriticalInputInterrupt : Interrupt<CriticalInputInterrupt, 0x00>
	{
		struct CriticalInput                        : Exception<CriticalInputInterrupt,5> { static const char *GetName() { return "Critical Input Interrupt/Critical Input Exception"; } };             //  p_critint_b is asserted and MSR[CE] = 1
		
		typedef ExceptionSet<CriticalInput> ALL;
		
		typedef FieldSet< MSR::SPV
		                , MSR::WE
		                , MSR::CE
		                , MSR::EE
		                , MSR::PR
		                , MSR::FP
		                , MSR::FE0
		                , MSR::FE1
		                , MSR::IS
		                , MSR::DS
		                , MSR::PMM > MSR_ALWAYS_CLEARED_FIELDS;
		
		static const char *GetName() { return "Critical Input Interrupt"; }
	};

	struct MachineCheckInterrupt : InterruptWithAddress<MachineCheckInterrupt, 0x10>
	{
		typedef InterruptWithAddress<MachineCheckInterrupt, 0x10> Super;
		
		enum MachineCheckEventType
		{
			MCE_MCP                                       = 0x00000001, // p_mcp_b transitions from negated to asserted
			MCE_EXCEPTION_ISI                             = 0x00000002, // ISI on first instruction fetch for an exception handler    
			MCE_EXCEPTION_BUS_ERROR                       = 0x00000004, // Bus Error on first instruction fetch for an exception handler
			MCE_INSTRUCTION_CACHE_DATA_ARRAY_PARITY_ERROR = 0x00000008, // Instruction cache data array parity error
			MCE_DATA_CACHE_DATA_ARRAY_PARITY_ERROR        = 0x00000010, // Data cache data array parity error
			MCE_INSTRUCTION_CACHE_TAG_PARITY_ERROR        = 0x00000020, // Instruction cache tag parity error
			MCE_DATA_CACHE_TAG_PARITY_ERROR               = 0x00000040, // Data cache tag parity error
			MCE_INSTRUCTION_CACHE_LOCK_ERROR              = 0x00000080, // Instruction cache lock error
			MCE_DATA_CACHE_LOCK_ERROR                     = 0x00000100, // Data cache lock error
			MCE_INSTRUCTION_FETCH_ERROR_REPORT            = 0x00000200, // Instruction Fetch Error Report
			MCE_LOAD_TYPE_INSTRUCTION_ERROR_REPORT        = 0x00000400, // Load type instruction Error Report
			MCE_STORE_TYPE_INSTRUCTION_ERROR_REPORT       = 0x00000800, // Store type instruction Error Report
			MCE_GUARDED_INSTRUCTION_ERROR_REPORT          = 0x00001000, // Guarded instruction Error Report
			MCE_STACK_LIMIT_CHECK_FAILURE                 = 0x00002000, // Stack limit check failure
			MCE_INSTRUCTION_MEMORY_PARITY_ERROR           = 0x00004000, // Instruction Memory Parity Error
			MCE_DATA_MEMORY_READ_PARITY_ERROR             = 0x00008000, // Data Memory Read Parity Error
			MCE_DATA_MEMORY_WRITE_PARITY_ERROR            = 0x00010000, // Data Memory Write Parity Error
			MCE_INSTRUCTION_READ_BUS_ERROR                = 0x00020000, // Instruction Read Bus Error
			MCE_DATA_READ_BUS_ERROR                       = 0x00040000, // Data Read Bus Error
			MCE_DATA_WRITE_BUS_ERROR                      = 0x00080000, // Data Write Bus Error
			MCE_DATA_WRITE_BUS_ERROR_DSI                  = 0x00100000  // Data Write Bus Error with DSI
		};

		struct NMI                                  : Exception<MachineCheckInterrupt,1> { static const char *GetName() { return "Machine Check Interrupt/NMI Exception"; } };               // Non-Maskable Interrupt: p_nmi_b transitions from negated to asserted.
		struct ErrorReport                          : Exception<MachineCheckInterrupt,2> { static const char *GetName() { return "Machine Check Interrupt/Error Report Exception"; } };      // Non-Maskable Interrupt: Error report
		struct AsynchronousMachineCheck             : Exception<MachineCheckInterrupt,3> { static const char *GetName() { return "Machine Check Interrupt/Asynchronous Machine Check Exception"; } };   // Maskable with MSR[ME]
		
		typedef ExceptionSet<NMI, ErrorReport, AsynchronousMachineCheck> ALL;
		
		typedef FieldSet< MSR::SPV
		                , MSR::WE
		                , MSR::CE
		                , MSR::EE
		                , MSR::PR
		                , MSR::FP
		                , MSR::ME
		                , MSR::FE0
		                , MSR::IS
		                , MSR::DS
		                , MSR::PMM
		                , MSR::RI > MSR_ALWAYS_CLEARED_FIELDS;
		
		MachineCheckInterrupt() : Super(), machine_check_events(0) {}
		bool GotEvent(MachineCheckEventType machine_check_event) const { return machine_check_events & machine_check_event; }
		MachineCheckInterrupt& SetEvent(MachineCheckEventType machine_check_event) { machine_check_events |= machine_check_event; return *this; }
		void ClearEvents() { machine_check_events = 0; }
		static const char *GetName() { return "Machine Check Interrupt"; }
	private:
		unsigned int machine_check_events;
	};
	
	struct DataStorageInterrupt : InterruptWithAddress<DataStorageInterrupt, 0x20>
	{
		struct AccessControl                        : Exception<DataStorageInterrupt,15> { static const char *GetName() { return "Data Storage Interrupt/Access Control Exception"; } };               // Access control
		
		typedef ExceptionSet<AccessControl> ALL;
		
		typedef FieldSet< MSR::SPV
		                , MSR::WE
		                , MSR::EE
		                , MSR::PR
		                , MSR::FP
		                , MSR::FE0
		                , MSR::FE1
		                , MSR::IS
		                , MSR::DS
		                , MSR::PMM > MSR_CLEARED_FIELDS;

		static const char *GetName() { return "Data Storage Interrupt"; }
	};

	struct InstructionStorageInterrupt : Interrupt<InstructionStorageInterrupt, 0x30>
	{
		struct AccessControl                        : Exception<InstructionStorageInterrupt,9> { static const char *GetName() { return "Instruction Storage Interrupt/Access Control Exception"; } };        // Access control
		
		typedef ExceptionSet<AccessControl> ALL;
		
		typedef FieldSet< MSR::SPV
		                , MSR::WE
		                , MSR::EE
		                , MSR::PR
		                , MSR::FP
		                , MSR::ME
		                , MSR::FE0
		                , MSR::FE1
		                , MSR::IS
		                , MSR::DS
		                , MSR::PMM > MSR_CLEARED_FIELDS;

		static const char *GetName() { return "Instruction Storage Interrupt"; }
	};

	struct ExternalInputInterrupt : Interrupt<ExternalInputInterrupt, 0x40>
	{
		struct ExternalInput                        : Exception<ExternalInputInterrupt,4> { static const char *GetName() { return "External Input Interrupt/External Input Exception"; } };             // Interrupt Controller interrupt and MSR[EE] = 1
		
		typedef ExceptionSet<ExternalInput> ALL;
		
		typedef FieldSet< MSR::SPV
		                , MSR::WE
		                , MSR::EE
		                , MSR::PR
		                , MSR::FP
		                , MSR::FE0
		                , MSR::FE1
		                , MSR::IS
		                , MSR::DS
		                , MSR::PMM > MSR_CLEARED_FIELDS;
		
		static const char *GetName() { return "External Input Interrupt"; }
	};

	struct AlignmentInterrupt : InterruptWithAddress<AlignmentInterrupt, 0x50>
	{
		struct UnalignedLoadStoreMultiple           : Exception<AlignmentInterrupt,16> { static const char *GetName() { return "Alignment Interrupt/Unaligned Load/Store Multiple Exception"; } };                 // lmw, stmw not word aligned
		struct UnalignedLoadLinkStoreConditional    : Exception<AlignmentInterrupt,17> { static const char *GetName() { return "Alignment Interrupt/Unaligned Load Link/Store Conditional Exception"; } };                 // lwarx or stwcx. not word aligned, lharx or sthcx. not halfword aligned
		struct WriteThroughDCBZ                     : Exception<AlignmentInterrupt,18> { static const char *GetName() { return "Alignment Interrupt/Write Through DCBZ Exception"; } };                 // dcbz
		
		typedef ExceptionSet<UnalignedLoadStoreMultiple, UnalignedLoadLinkStoreConditional, WriteThroughDCBZ> ALL;
		
		typedef FieldSet< MSR::SPV
		                , MSR::WE
		                , MSR::EE
		                , MSR::PR
		                , MSR::FP
		                , MSR::FE0
		                , MSR::FE1
		                , MSR::IS
		                , MSR::DS
		                , MSR::PMM > MSR_CLEARED_FIELDS;

		static const char *GetName() { return "Alignment Interrupt"; }
	};
	
	struct ProgramInterrupt : Interrupt<ProgramInterrupt, 0x60>
	{
		struct IllegalInstruction                   : Exception<ProgramInterrupt,10>  { static const char *GetName() { return "Program Interrupt/Illegal Instruction Exception"; } };                   // illegal instruction
		struct PrivilegeViolation                   : Exception<ProgramInterrupt,11>  { static const char *GetName() { return "Program Interrupt/Privilege Violation Exception"; } };                   // privilege violation
		struct Trap                                 : Exception<ProgramInterrupt,12> { static const char *GetName() { return "Program Interrupt/Trap Exception"; } };                   // trap instruction
		struct UnimplementedInstruction             : Exception<ProgramInterrupt,13> { static const char *GetName() { return "Program Interrupt/Unimplemented Instruction Exception"; } };                   // unimplemented instruction
		
		typedef ExceptionSet<IllegalInstruction, PrivilegeViolation, Trap> ALL;
		
		typedef FieldSet< MSR::SPV
		                , MSR::WE
		                , MSR::EE
		                , MSR::PR
		                , MSR::FP
		                , MSR::FE0
		                , MSR::FE1
		                , MSR::IS
		                , MSR::DS
		                , MSR::PMM > MSR_CLEARED_FIELDS;

		static const char *GetName() { return "Program Interrupt"; }
	};

	struct PerformanceMonitorInterrupt : Interrupt<PerformanceMonitorInterrupt, 0x70>
	{
		struct PerformanceCounterOverflow           : Exception<PerformanceMonitorInterrupt,7> { static const char *GetName() { return "Performance Monitor Interrupt/Performance Counter Overflow Exception"; } };        // PMC register overflow
		struct DebugEvent                           : Exception<PerformanceMonitorInterrupt,8> { static const char *GetName() { return "Performance Monitor Interrupt/Debug Event Exception"; } };        // Event w/PMGC0[UDI]=0
		
		typedef ExceptionSet<PerformanceCounterOverflow, DebugEvent> ALL;
		
		typedef FieldSet< MSR::SPV
		                , MSR::WE
		                , MSR::PR
		                , MSR::FP
		                , MSR::FE0
		                , MSR::FE1
		                , MSR::IS
		                , MSR::DS
		                , MSR::PMM > MSR_ALWAYS_CLEARED_FIELDS;
		
		static const char *GetName() { return "Performance Monitor Interrupt"; }
	};
	
	struct SystemCallInterrupt : Interrupt<SystemCallInterrupt, 0x80>
	{
		struct SystemCall                           : Exception<SystemCallInterrupt,14> { static const char *GetName() { return "System Call Interrupt/System Call Exception"; } };                // Execution of the System Call (se_sc) instruction
		
		typedef ExceptionSet<SystemCall> ALL;
		
		typedef FieldSet< MSR::SPV
		                , MSR::WE
		                , MSR::EE
		                , MSR::PR
		                , MSR::FP
		                , MSR::FE0
		                , MSR::FE1
		                , MSR::IS
		                , MSR::DS
		                , MSR::PMM > MSR_CLEARED_FIELDS;
		
		static const char *GetName() { return "System Call Interrupt"; }
		
		SystemCallInterrupt() : elev(0) {}
		void SetELEV(unsigned int _elev) { elev = _elev; }
		unsigned int GetELEV() const { return elev; }
		void ClearELEV() { elev = 0; }
	private:
		unsigned int elev;
	};

	struct DebugInterrupt : Interrupt<DebugInterrupt, 0x90>
	{
		typedef Interrupt<DebugInterrupt, 0x90> Super;
		
		enum DebugEventType
		{
			DBG_UNCONDITIONAL_DEBUG_EVENT            = 0x00000001, // Async
			DBG_INSTRUCTION_COMPLETE_DEBUG_EVENT     = 0x00000002, // Sync
			DBG_BRANCH_TAKEN_DEBUG_EVENT             = 0x00000004, // Sync
			DBG_INTERRUPT_TAKEN_DEBUG_EVENT          = 0x00000008, // Sync
			DBG_CRITICAL_INTERRUPT_TAKEN_DEBUG_EVENT = 0x00000010, // Async
			DBG_TRAP_INSTRUCTION_DEBUG_EVENT         = 0x00000020, // Sync
			DBG_INSTRUCTION_ADDRESS_COMPARE1         = 0x00000040, // Sync
			DBG_INSTRUCTION_ADDRESS_COMPARE2         = 0x00000080, // Sync
			DBG_INSTRUCTION_ADDRESS_COMPARE3         = 0x00000100, // Sync
			DBG_INSTRUCTION_ADDRESS_COMPARE4         = 0x00000200, // Sync
			DBG_INSTRUCTION_ADDRESS_COMPARE5         = 0x00000400, // Sync
			DBG_INSTRUCTION_ADDRESS_COMPARE6         = 0x00000800, // Sync
			DBG_INSTRUCTION_ADDRESS_COMPARE7         = 0x00001000, // Sync
			DBG_INSTRUCTION_ADDRESS_COMPARE8         = 0x00002000, // Sync
			DBG_DATA_ADDRESS_COMPARE1_READ           = 0x00004000, // Sync
			DBG_DATA_ADDRESS_COMPARE1_WRITE          = 0x00008000, // Sync
			DBG_DATA_ADDRESS_COMPARE2_READ           = 0x00010000, // Sync
			DBG_DATA_ADDRESS_COMPARE2_WRITE          = 0x00020000, // Sync
			DBG_DEBUG_NOTIFY_INTERRUPT               = 0x00040000, // Sync
			DBG_RETURN_DEBUG_EVENT                   = 0x00080000, // Sync
			DBG_CRITICAL_RETURN_DEBUG_EVENT          = 0x00100000, // Async
			DBG_EXTERNAL_DEBUG_EVENT1                = 0x00200000, // Async
			DBG_EXTERNAL_DEBUG_EVENT2                = 0x00400000, // Async
			DBG_PERFORMANCE_MONITOR_DEBUG_EVENT      = 0x00800000, // Async
			DBG_MPU_DEBUG_EVENT                      = 0x01000000, // Sync
			DBG_IMPRECISE_DEBUG_EVENT                = 0x02000000  // Sync
		};
		
		struct AsynchronousDebugEvent                   : Exception<DebugInterrupt,6> { static const char *GetName() { return "Debug Interrupt/Asynchronous Debug Event Exception"; } };
		struct SynchronousDebugEvent                    : Exception<DebugInterrupt,21> { static const char *GetName() { return "Debug Interrupt/Synchronous Debug Event Exception"; } };
		
		typedef ExceptionSet< AsynchronousDebugEvent, SynchronousDebugEvent > ALL;
		
		typedef FieldSet< MSR::SPV
		                , MSR::WE
		                , MSR::PR
		                , MSR::FP
		                , MSR::FE0
		                , MSR::DE
		                , MSR::FE1
		                , MSR::IS
		                , MSR::DS
		                , MSR::PMM > MSR_ALWAYS_CLEARED_FIELDS;
		
		DebugInterrupt() : Super(), dbg_events(0) {}
		bool GotEvent(DebugEventType dbg_event) const { return dbg_events & dbg_event; }
		DebugInterrupt& SetEvent(DebugEventType dbg_event) { dbg_events |= dbg_event; return *this; }
		void ClearEvents() { dbg_events = 0; }
		
		static const char *GetName() { return "Debug Interrupt"; }
	private:
		uint32_t dbg_events;
	};

	struct EmbeddedFloatingPointDataInterrupt : Interrupt<EmbeddedFloatingPointDataInterrupt, 0xa0>
	{
		struct EmbeddedFloatingPointData            : Exception<EmbeddedFloatingPointDataInterrupt,19> { static const char *GetName() { return "Embedded Floating-Point Data Interrupt/Embedded Floating-Point Data Exception"; } }; // Embedded Floating-point Data Exception
		
		typedef ExceptionSet<EmbeddedFloatingPointData> ALL;
		
		typedef FieldSet< MSR::SPV
		                , MSR::WE
		                , MSR::EE
		                , MSR::PR
		                , MSR::FP
		                , MSR::FE0
		                , MSR::FE1
		                , MSR::IS
		                , MSR::DS
		                , MSR::PMM > MSR_CLEARED_FIELDS;
		
		static const char *GetName() { return "Embedded Floating-Point Data Interrupt"; }
	};
	
	struct EmbeddedFloatingPointRoundInterrupt : Interrupt<EmbeddedFloatingPointRoundInterrupt, 0xb0>
	{
		struct EmbeddedFloatingPointRound           : Exception<EmbeddedFloatingPointRoundInterrupt,20> { static const char *GetName() { return "Embedded Floating-Point Round Interrupt/Embedded Floating-Point Round Exception"; } };// Embedded Floating-point Round Exception
		
		typedef ExceptionSet<EmbeddedFloatingPointRound> ALL;
		
		typedef FieldSet< MSR::SPV
		                , MSR::WE
		                , MSR::EE
		                , MSR::PR
		                , MSR::FP
		                , MSR::FE0
		                , MSR::FE1
		                , MSR::IS
		                , MSR::DS
		                , MSR::PMM > MSR_CLEARED_FIELDS;

		static const char *GetName() { return "Embedded Floating-Point Round Interrupt"; }
	};

	void ProcessInterrupt(SystemResetInterrupt *);
	void ProcessInterrupt(MachineCheckInterrupt *);
	void ProcessInterrupt(DataStorageInterrupt *);
	void ProcessInterrupt(InstructionStorageInterrupt *);
	void ProcessInterrupt(AlignmentInterrupt *);
	void ProcessInterrupt(ProgramInterrupt *);
	void ProcessInterrupt(EmbeddedFloatingPointDataInterrupt *);
	void ProcessInterrupt(EmbeddedFloatingPointRoundInterrupt *);
	void ProcessInterrupt(SystemCallInterrupt *);
	void ProcessInterrupt(CriticalInputInterrupt *);
	void ProcessInterrupt(ExternalInputInterrupt *);
	void ProcessInterrupt(PerformanceMonitorInterrupt *);
	void ProcessInterrupt(DebugInterrupt *);
	
	void UpdateExceptionEnable();
	
	void SetAutoVector(bool value) { if(verbose_interrupt) logger << DebugInfo << (value ? "Enabling" : "Disabling") << " auto vector" << EndDebugInfo; enable_auto_vectored_interrupts = value; }
	void SetVectorOffset(ADDRESS value) { vector_offset = value & 0xfffffffcUL; }
	
	//////////////////////////// Memory SubSystem /////////////////////////////
	
	// Level 1 Instruction Cache
	struct L1I_CONFIG
	{
		static const uint32_t SIZE                                          = 16384;
		static const unisim::util::cache::CacheWritingPolicy WRITING_POLICY = unisim::util::cache::CACHE_WRITE_THROUGH_AND_NO_WRITE_ALLOCATE_POLICY;
		static const unisim::util::cache::CacheType TYPE                    = unisim::util::cache::INSTRUCTION_CACHE;
		static const unsigned int ASSOCIATIVITY                             = 2;
		static const unsigned int BLOCK_SIZE                                = 32;
		static const unsigned int BLOCKS_PER_LINE                           = 1;
		
		struct SET_STATUS : unisim::util::cache::CacheSetStatus
		{
			SET_STATUS() : victim_way(0), lockout(0) {}
			
			unsigned int victim_way;
			unsigned int lockout;
		};
		
		struct LINE_STATUS : unisim::util::cache::CacheLineStatus {};
		
		struct BLOCK_STATUS : unisim::util::cache::CacheBlockStatus {};
	};
	
	struct L1I : unisim::util::cache::Cache<MSS_TYPES, L1I_CONFIG, L1I> {};

	// Level 1 Data Cache
	struct L1D_CONFIG
	{
		static const uint32_t SIZE                                          = 4096;
		static const unisim::util::cache::CacheWritingPolicy WRITING_POLICY = unisim::util::cache::CACHE_WRITE_THROUGH_AND_NO_WRITE_ALLOCATE_POLICY;
		static const unisim::util::cache::CacheType TYPE                    = unisim::util::cache::DATA_CACHE;
		static const unsigned int ASSOCIATIVITY                             = 2;
		static const unsigned int BLOCK_SIZE                                = 32;
		static const unsigned int BLOCKS_PER_LINE                           = 1;
		
		struct SET_STATUS : unisim::util::cache::CacheSetStatus
		{
			SET_STATUS() : victim_way(0), lockout(0) {}

			unsigned int victim_way;
			unsigned int lockout;
		};
		
		struct LINE_STATUS : unisim::util::cache::CacheLineStatus {};
		
		struct BLOCK_STATUS : unisim::util::cache::CacheBlockStatus {};
	};
	
	struct L1D : unisim::util::cache::Cache<MSS_TYPES, L1D_CONFIG, L1D> {};

	IF_COND_TRAIT(_) { typedef unisim::util::cache::CacheHierarchy<MSS_TYPES, L1D> DATA_CACHE_HIERARCHY; };
	ELSE_COND_TRAIT(_) { typedef unisim::util::cache::CacheHierarchy<MSS_TYPES> DATA_CACHE_HIERARCHY; };
	typedef COND_TRAIT(HAS_DATA_CACHE, _)::DATA_CACHE_HIERARCHY DATA_CACHE_HIERARCHY;
	
	typedef unisim::util::cache::CacheHierarchy<MSS_TYPES, L1I> INSTRUCTION_CACHE_HIERARCHY;
	
	inline std::ostream& GetDebugInfoStream() ALWAYS_INLINE { return this->SuperCPU::GetDebugInfoStream(); }
	inline std::ostream& GetDebugWarningStream() ALWAYS_INLINE { return this->SuperCPU::GetDebugWarningStream(); }
	inline std::ostream& GetDebugErrorStream() ALWAYS_INLINE { return this->SuperCPU::GetDebugErrorStream(); }

	inline L1I *GetCache(const L1I *) ALWAYS_INLINE { return &l1i; }
	inline bool IsCacheVerbose(const L1I *) ALWAYS_INLINE { return verbose_instruction_cache; }
	inline bool IsCacheEnabled(const L1I *) ALWAYS_INLINE { return l1csr1.Get<L1CSR1::ICE>(); }
	inline bool IsCacheWriteAllocate(const L1I *) ALWAYS_INLINE { return false; }
	inline bool ChooseLineToEvict(unisim::util::cache::CacheAccess<MSS_TYPES, L1I>& access) ALWAYS_INLINE
	{
		if(l1csr0.Get<L1CSR0::WID>() == ((1 << L1I::ASSOCIATIVITY) - 1)) return false; // all instruction cache ways are locked
		
		unsigned int& lockout = access.set->Status().lockout;
		if(lockout == ((1 << L1I::ASSOCIATIVITY) - 1)) return false; // all data cache ways are locked-out
		
		unsigned int& victim_way = access.set->Status().victim_way;
		while((l1csr0.Get<L1CSR0::WID>() & ((1 << (L1I::ASSOCIATIVITY - 1)) >> victim_way)) || (lockout & ((1 << (L1I::ASSOCIATIVITY - 1)) >> victim_way))) victim_way++;
		access.way = victim_way;
		return true;
	}
	inline void UpdateReplacementPolicyOnAccess(unisim::util::cache::CacheAccess<MSS_TYPES, L1I>& access) ALWAYS_INLINE {}
	inline void UpdateReplacementPolicyOnFill(unisim::util::cache::CacheAccess<MSS_TYPES, L1I>& access) ALWAYS_INLINE
	{
		access.set->Status().victim_way++;
	}

	inline L1D *GetCache(const L1D *) ALWAYS_INLINE { return HAS_DATA_CACHE ? &l1d : 0; }
	inline bool IsCacheVerbose(const L1D *) ALWAYS_INLINE { return HAS_DATA_CACHE ? verbose_data_cache : false; }
	inline bool IsCacheEnabled(const L1D *) ALWAYS_INLINE { return HAS_DATA_CACHE ? l1csr0.Get<L1CSR0::DCE>() : false; }
	inline bool IsCacheWriteAllocate(const L1D *) ALWAYS_INLINE { return HAS_DATA_CACHE ? l1csr0.Get<L1CSR0::DCWA>() : false; }
	inline bool ChooseLineToEvict(unisim::util::cache::CacheAccess<MSS_TYPES, L1D>& access) ALWAYS_INLINE
	{
		if(!HAS_DATA_CACHE) return false;
		
		if(l1csr0.Get<L1CSR0::WDD>() == ((1 << L1D::ASSOCIATIVITY) - 1)) return false; // all data cache ways are locked

		unsigned int& lockout = access.set->Status().lockout;
		if(lockout == ((1 << L1D::ASSOCIATIVITY) - 1)) return false; // all data cache ways are locked-out
		
		unsigned int& victim_way = access.set->Status().victim_way;
		while((l1csr0.Get<L1CSR0::WDD>() & ((1 << (L1D::ASSOCIATIVITY - 1)) >> victim_way)) || (lockout & ((1 << (L1D::ASSOCIATIVITY - 1)) >> victim_way))) victim_way++;
		
		access.way = victim_way;
		return true;
	}
	
	inline void UpdateReplacementPolicyOnAccess(unisim::util::cache::CacheAccess<MSS_TYPES, L1D>& access) ALWAYS_INLINE {}
	inline void UpdateReplacementPolicyOnFill(unisim::util::cache::CacheAccess<MSS_TYPES, L1D>& access) ALWAYS_INLINE
	{
		if(HAS_DATA_CACHE)
		{
			access.set->Status().victim_way++;
		}
	}
	
	inline bool IsVerboseDataLoad() const ALWAYS_INLINE { return verbose_data_load; }
	inline bool IsVerboseDataStore() const ALWAYS_INLINE { return verbose_data_store; }
	inline bool IsVerboseInstructionFetch() const ALWAYS_INLINE { return verbose_instruction_fetch; }
	inline bool IsVerboseDataBusRead() const ALWAYS_INLINE { return verbose_data_bus_read; }
	inline bool IsVerboseDataBusWrite() const ALWAYS_INLINE { return verbose_data_bus_write; }
	inline bool IsVerboseInstructionBusRead() const ALWAYS_INLINE { return verbose_instruction_bus_read; }
	bool IsStorageCacheable(STORAGE_ATTR storage_attr) const { return HAS_DATA_CACHE ? !(storage_attr & SA_I) : false; }
	bool IsDataWriteAccessWriteThrough(STORAGE_ATTR storage_attr) const { return HAS_DATA_CACHE ? true : false; }

	bool DataBusRead(PHYSICAL_ADDRESS addr, void *buffer, unsigned int size, STORAGE_ATTR storage_attr, bool rwitm);
	bool DataBusWrite(PHYSICAL_ADDRESS addr, const void *buffer, unsigned int size, STORAGE_ATTR storage_attr);
	bool InstructionBusRead(PHYSICAL_ADDRESS addr, void *buffer, unsigned int size, STORAGE_ATTR storage_attr);

	bool DebugDataBusRead(PHYSICAL_ADDRESS addr, void *buffer, unsigned int size, STORAGE_ATTR storage_attr);
	bool DebugDataBusWrite(PHYSICAL_ADDRESS addr, const void *buffer, unsigned int size, STORAGE_ATTR storage_attr);
	bool DebugInstructionBusRead(PHYSICAL_ADDRESS addr, void *buffer, unsigned int size, STORAGE_ATTR storage_attr);

	virtual bool AHBInsnRead(PHYSICAL_ADDRESS physical_addr, void *buffer, uint32_t size, STORAGE_ATTR storage_attr);
	virtual bool AHBDataRead(PHYSICAL_ADDRESS physical_addr, void *buffer, uint32_t size, STORAGE_ATTR storage_attr, bool rwitm);
	virtual bool AHBDataWrite(PHYSICAL_ADDRESS physical_addr, const void *buffer, uint32_t size, STORAGE_ATTR storage_attr);

	virtual bool AHBDebugInsnRead(PHYSICAL_ADDRESS physical_addr, void *buffer, uint32_t size, STORAGE_ATTR storage_attr);
	virtual bool AHBDebugDataRead(PHYSICAL_ADDRESS physical_addr, void *buffer, uint32_t size, STORAGE_ATTR storage_attr);
	virtual bool AHBDebugDataWrite(PHYSICAL_ADDRESS physical_addr, const void *buffer, uint32_t size, STORAGE_ATTR storage_attr);

	bool DataLoad(ADDRESS addr, void *buffer, unsigned int size);
	bool DataStore(ADDRESS addr, void *buffer, unsigned int size);
	template <typename T, bool REVERSE, bool FORCE_BIG_ENDIAN> bool DataLoad(T& value, ADDRESS ea);
	template <typename T, bool REVERSE, bool FORCE_BIG_ENDIAN> bool DataStore(T value, ADDRESS ea);
	
	unsigned int ExternalLoad(PHYSICAL_ADDRESS addr, void *buffer, unsigned int size);
	unsigned int ExternalStore(PHYSICAL_ADDRESS addr, void *buffer, unsigned int size);
	unsigned char *GetDirectMemPtr(PHYSICAL_ADDRESS addr, PHYSICAL_ADDRESS& start_addr, PHYSICAL_ADDRESS& end_addr);
	virtual void InvalidateDirectMemPtr(PHYSICAL_ADDRESS start_addr, PHYSICAL_ADDRESS end_addr) {}

	void InvalidateDataCache();
	void InvalidateDataCacheBySetAndWay(unsigned int index, unsigned int way);
	void ClearDataCacheLockoutBySetAndWay(unsigned int index, unsigned int way);
	void InvalidateInstructionCache();
	void InvalidateInstructionCacheBySetAndWay(unsigned int index, unsigned int way);
	void ClearInstructionCacheLockoutBySetAndWay(unsigned int index, unsigned int way);
public:

	bool InstructionFetch(ADDRESS addr, void *buffer, unsigned int size);
	void FlushInstructionBuffer();
	bool InstructionFetch(ADDRESS addr, unisim::component::cxx::processor::powerpc::e200z710n3::isa::vle::CodeType& insn);
	void StepOneInstruction();
	
	bool DebugDataLoad(ADDRESS addr, void *buffer, unsigned int size);
	bool DebugDataStore(ADDRESS addr, const void *buffer, unsigned int size);
	bool DebugInstructionFetch(ADDRESS addr, void *buffer, unsigned int size);
	
protected:
	////////////////////////// Run-time parameters ////////////////////////////
	
	uint8_t cpuid;
	unisim::kernel::service::Parameter<uint8_t> param_cpuid;

	uint32_t processor_version;
	unisim::kernel::service::Parameter<uint32_t> param_processor_version;
	
	uint32_t system_version;
	unisim::kernel::service::Parameter<uint32_t> param_system_version;

	uint32_t system_information;
	unisim::kernel::service::Parameter<uint32_t> param_system_information;

	ADDRESS local_memory_base_addr;
	unisim::kernel::service::Parameter<ADDRESS> *param_local_memory_base_addr;

	ADDRESS local_memory_size;
	unisim::kernel::service::Parameter<ADDRESS> *param_local_memory_size;

	ADDRESS imem_base_addr;
	unisim::kernel::service::Parameter<ADDRESS> *param_imem_base_addr;

	ADDRESS imem_size;
	unisim::kernel::service::Parameter<ADDRESS> *param_imem_size;

	ADDRESS dmem_base_addr;
	unisim::kernel::service::Parameter<ADDRESS> *param_dmem_base_addr;

	ADDRESS dmem_size;
	unisim::kernel::service::Parameter<ADDRESS> *param_dmem_size;

	bool verbose_instruction_cache;
	unisim::kernel::service::Parameter<bool> *param_verbose_instruction_cache;
	
	bool verbose_data_cache;
	unisim::kernel::service::Parameter<bool> *param_verbose_data_cache;
	
	bool verbose_data_load;
	unisim::kernel::service::Parameter<bool> *param_verbose_data_load;
	
	bool verbose_data_store;
	unisim::kernel::service::Parameter<bool> *param_verbose_data_store;
	
	bool verbose_instruction_fetch;
	unisim::kernel::service::Parameter<bool> *param_verbose_instruction_fetch;
	
	bool verbose_data_bus_read;
	unisim::kernel::service::Parameter<bool> *param_verbose_data_bus_read;
	
	bool verbose_data_bus_write;
	unisim::kernel::service::Parameter<bool> *param_verbose_data_bus_write;
	
	bool verbose_instruction_bus_read;
	unisim::kernel::service::Parameter<bool> *param_verbose_instruction_bus_read;
	
	///////////////////////////// Interrupts //////////////////////////////////
	
	bool enable_auto_vectored_interrupts;
	ADDRESS vector_offset;

	////////////////////////// Instruction Buffer /////////////////////////////

	ADDRESS instruction_buffer_base_addr;
	uint16_t instruction_buffer[L1I::BLOCK_SIZE / 2];
	
	/////////////////////// VLE Instruction decoder ///////////////////////////
	
	unisim::component::cxx::processor::powerpc::e200z710n3::isa::vle::Decoder vle_decoder;
	unisim::component::cxx::processor::powerpc::e200z710n3::isa::vle::Operation *operation;
	
	///////////////////////// Memory Protection Unit //////////////////////////

	friend struct MPU<MPU_CONFIG>;
	
	MPU<MPU_CONFIG> mpu;

	/////////////////////////// Memory sub-system /////////////////////////////
	
	L1I l1i;                     // L1 instruction cache
	L1D l1d;                     // L1 data cache
	
	uint8_t *imem;               // I-MEM
	uint8_t *dmem;               // D-MEM
	
	ADDRESS cur_imem_base_addr;
	ADDRESS cur_dmem_base_addr;
	ADDRESS cur_imem_high_addr;
	ADDRESS cur_dmem_high_addr;

	/////////////////////////// Machine State Register ////////////////////////
	
	MSR msr;
	
	////////////////////////// Special Purpose Registers //////////////////////
	
	SRR0 srr0;
	SRR1 srr1;
	PID0 pid0;
	CSRR0 csrr0;
	CSRR1 csrr1;
	DEAR dear;
	ESR esr;
	IVPR ivpr;
	USPRG0 usprg0;
	SPRG0 sprg0;
	SPRG1 sprg1;
	SPRG2 sprg2;
	SPRG3 sprg3;
	PIR pir;
	PVR pvr;
	DBSR dbsr;
	DBCR0 dbcr0;
	DBCR1 dbcr1;
	DBCR2 dbcr2;
	IAC1 iac1;
	IAC2 iac2;
	IAC3 iac3;
	IAC4 iac4;
	DAC1 dac1;
	DAC2 dac2;
	DVC1 dvc1;
	DVC2 dvc2;
	TIR tir;
	SPEFSCR spefscr;
	L1CFG0 l1cfg0;
	L1CFG1 l1cfg1;
	NPIDR npidr;
	DBCR3 dbcr3;
	DBCNT dbcnt;
	DBCR4 dbcr4;
	DBCR5 dbcr5;
	IAC5 iac5;
	IAC6 iac6;
	IAC7 iac7;
	IAC8 iac8;
	MCSRR0 mcsrr0;
	MCSRR1 mcsrr1;
	MCSR mcsr;
	MCAR mcar;
	DSRR0 dsrr0;
	DSRR1 dsrr1;
	DDAM ddam;
	DAC3 dac3;
	DAC4 dac4;
	DBCR7 dbcr7;
	DBCR8 dbcr8;
	DDEAR ddear;
	DVC1U dvc1u;
	DVC2U dvc2u;
	DBCR6 dbcr6;
	EDBRAC0 edbrac0;
	DMEMCFG0 dmemcfg0;
	IMEMCFG0 imemcfg0;
	L1FINV1 l1finv1;
	DEVENT devent;
	SIR sir;
	HID0 hid0;
	HID1 hid1;
	L1CSR0 l1csr0;
	L1CSR1 l1csr1;
	BUCSR bucsr;
	L1FINV0 l1finv0;
	SVR svr;
	
	////////////////////////// Device Control Registers ///////////////////////
	
	DMEMCTL0 dmemctl0;
	DMEMCTL1 dmemctl1;
	IMEMCTL0 imemctl0;
	IMEMCTL1 imemctl1;
	E2ECTL0 e2ectl0;
	E2EECSR0 e2eecsr0;
	
	//////////////////////// Performance Monitor Registers /////////////////////
	
	PMC0 pmc0;
	PMC1 pmc1;
	PMC2 pmc2;
	PMC3 pmc3;
	PMGC0 pmgc0;
	PMLCa0 pmlca0;
	PMLCa1 pmlca1;
	PMLCa2 pmlca2;
	PMLCa3 pmlca3;
	PMLCb0 pmlcb0;
	PMLCb1 pmlcb1;
	PMLCb2 pmlcb2;
	PMLCb3 pmlcb3;
	
	UPMC0 upmc0;
	UPMC1 upmc1;
	UPMC2 upmc2;
	UPMC3 upmc3;
	UPMGC0 upmgc0;
	UPMLCa0 upmlca0;
	UPMLCa1 upmlca1;
	UPMLCa2 upmlca2;
	UPMLCa3 upmlca3;
	UPMLCb0 upmlcb0;
	UPMLCb1 upmlcb1;
	UPMLCb2 upmlcb2;
	UPMLCb3 upmlcb3;
	
	///////////////////////// Thread Management Registers /////////////////////
	
	TMCFG0 tmcfg0;
};

template <typename T, bool REVERSE, bool FORCE_BIG_ENDIAN>
inline bool CPU::DataLoad(T& value, typename CONFIG::ADDRESS ea)
{
	uint32_t size_to_fsb_boundary = CONFIG::FSB_WIDTH - (ea & (CONFIG::FSB_WIDTH - 1));

	// Ensure that memory access does not cross a FSB boundary
	if(likely(size_to_fsb_boundary >= sizeof(T)))
	{
		// Memory load does not cross a FSB boundary
		if(unlikely(!DataLoad(ea, &value, sizeof(T)))) return false;
	}
	else
	{
		// Memory load crosses a FSB boundary

		if(unlikely(!DataLoad(ea, &value, size_to_fsb_boundary))) return false;
		
		typename CONFIG::ADDRESS ea2 = ea + size_to_fsb_boundary;
		
		if(unlikely(!DataLoad(ea2, ((uint8_t *) &value) + size_to_fsb_boundary, sizeof(T) - size_to_fsb_boundary))) return false;
	}

#if BYTE_ORDER == LITTLE_ENDIAN
	if(!REVERSE || FORCE_BIG_ENDIAN)
#else
	if(REVERSE && !FORCE_BIG_ENDIAN)
#endif
	{
		unisim::util::endian::BSwap(value);
	}
	
	return true;
}

template <typename T, bool REVERSE, bool FORCE_BIG_ENDIAN>
inline bool CPU::DataStore(T value, typename CONFIG::ADDRESS ea)
{
#if BYTE_ORDER == LITTLE_ENDIAN
	if(!REVERSE || FORCE_BIG_ENDIAN)
#else
	if(REVERSE && !FORCE_BIG_ENDIAN)
#endif
	{
		unisim::util::endian::BSwap(value);
	}

	uint32_t size_to_fsb_boundary = CONFIG::FSB_WIDTH - (ea & (CONFIG::FSB_WIDTH - 1));

	// Ensure that memory access does not cross a FSB boundary
	if(likely(size_to_fsb_boundary >= sizeof(T)))
	{
		// Memory store does not cross a FSB boundary
		if(unlikely(!DataStore(ea, &value, sizeof(T)))) return false;
	}
	else
	{
		// Memory store crosses a FSB boundary

		if(unlikely(!DataStore(ea, &value, size_to_fsb_boundary))) return false;
		
		typename CONFIG::ADDRESS ea2 = ea + size_to_fsb_boundary;
		
		if(unlikely(!DataStore(ea2, ((uint8_t *) &value) + size_to_fsb_boundary, sizeof(T) - size_to_fsb_boundary))) return false;
	}
	
	return true;
}

} // end of namespace e200z710n3
} // end of namespace powerpc
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim


#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_E200Z710N3_CPU_HH__
