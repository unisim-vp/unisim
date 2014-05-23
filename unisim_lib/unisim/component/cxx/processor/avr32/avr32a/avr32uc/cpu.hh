/*
 *  Copyright (c) 2014,
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
 * Authors: Julien Lisita (julien.lisita@cea.fr)
 *          Gilles Mouchard (gilles.mouchard@cea.fr)
 */
 
#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_AVR32_AVR32A_AVR32UC_CPU_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_AVR32_AVR32A_AVR32UC_CPU_HH__

#include <unisim/kernel/logger/logger.hh>
#include <unisim/component/cxx/processor/avr32/avr32a/avr32uc/isa.hh>
#include <unisim/component/cxx/processor/avr32/avr32a/avr32uc/config.hh>
#include <unisim/service/interfaces/memory.hh>
#include <unisim/service/interfaces/memory_injection.hh>
#include <unisim/service/interfaces/debug_control.hh>
#include <unisim/service/interfaces/memory_access_reporting.hh>
#include <unisim/service/interfaces/disassembly.hh>
#include <unisim/util/debug/simple_register.hh>
#include <unisim/util/endian/endian.hh>
#include <unisim/util/arithmetic/arithmetic.hh>
#include <unisim/kernel/service/service.hh>
#include <unisim/service/interfaces/memory.hh>
#include <unisim/service/interfaces/loader.hh>
#include <unisim/service/interfaces/symbol_table_lookup.hh>
#include <unisim/service/interfaces/synchronizable.hh>
#include <unisim/service/interfaces/trap_reporting.hh>
#include <unisim/service/interfaces/registers.hh>
#include <unisim/service/interfaces/avr32_t2h_syscalls.hh>
#include <unisim/util/likely/likely.hh>
#include <unisim/util/inlining/inlining.hh>
#include <map>
#include <iosfwd>

#include <unisim/kernel/debug/debug.hh>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace avr32 {
namespace avr32a {
namespace avr32uc {

using unisim::service::interfaces::DebugControl;
using unisim::service::interfaces::Disassembly;
using unisim::service::interfaces::MemoryAccessReporting;
using unisim::service::interfaces::MemoryAccessReportingControl;
using unisim::service::interfaces::Memory;
using unisim::service::interfaces::MemoryInjection;
using unisim::service::interfaces::Registers;
using unisim::service::interfaces::AVR32_T2H_Syscalls;
using namespace unisim::util::endian;
using unisim::kernel::service::Client;
using unisim::kernel::service::Service;
using unisim::kernel::service::ServiceImport;
using unisim::kernel::service::ServiceExport;
using unisim::kernel::service::Object;
using unisim::service::interfaces::Loader;
using unisim::util::debug::Symbol;
using unisim::service::interfaces::SymbolTableLookup;
using unisim::service::interfaces::Synchronizable;
using unisim::service::interfaces::TrapReporting;
using unisim::kernel::service::Parameter;
using unisim::kernel::service::Statistic;
using unisim::kernel::service::ParameterArray;
using unisim::kernel::service::Formula;
using unisim::kernel::logger::Logger;
using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;
using namespace std;

template <class CONFIG>
class CPU :
	public unisim::component::cxx::processor::avr32::avr32a::avr32uc::Decoder<CONFIG>,
	public Client<Loader>,
	public Client<SymbolTableLookup<typename CONFIG::address_t> >,
	public Client<DebugControl<typename CONFIG::address_t> >,
	public Client<MemoryAccessReporting<typename CONFIG::address_t> >,
	public Client<TrapReporting>,
	public Service<MemoryAccessReportingControl>,
	public Service<Disassembly<typename CONFIG::address_t> >,
	public Service<unisim::service::interfaces::Registers>,
	public Service<Memory<typename CONFIG::address_t> >,
	public Service<MemoryInjection<typename CONFIG::address_t> >,
	public Client<Memory<typename CONFIG::physical_address_t> >,
	public Service<Synchronizable>,
	public Client<AVR32_T2H_Syscalls>
{
public:
	//=====================================================================
	//=                  public service imports/exports                   =
	//=====================================================================
	
	ServiceExport<Disassembly<typename CONFIG::address_t> > disasm_export;
	ServiceExport<unisim::service::interfaces::Registers> registers_export;
	ServiceExport<Memory<typename CONFIG::address_t> > memory_export;
	ServiceExport<MemoryInjection<typename CONFIG::address_t> > memory_injection_export;
	ServiceExport<Synchronizable> synchronizable_export;
	ServiceExport<MemoryAccessReportingControl> memory_access_reporting_control_export;

	ServiceImport<Loader> loader_import;
	ServiceImport<DebugControl<typename CONFIG::address_t> > debug_control_import;
	ServiceImport<MemoryAccessReporting<typename CONFIG::address_t> > memory_access_reporting_import;
	ServiceImport<SymbolTableLookup<typename CONFIG::address_t> > symbol_table_lookup_import;
	ServiceImport<Memory<typename CONFIG::physical_address_t> > memory_import;
	ServiceImport<TrapReporting> trap_reporting_import;
	ServiceImport<AVR32_T2H_Syscalls> avr32_t2h_syscalls_import;
	
	//=====================================================================
	//=                    Constructor/Destructor                         =
	//=====================================================================

	CPU(const char *name, Object *parent = 0);
	virtual ~CPU();
	
        //=====================================================================
	//=                        Getters//Setters                           =
	//=====================================================================    

	uint32_t GetGPR(unsigned int reg_num) { return gpr[reg_num]; }
        uint32_t GetPC(){return gpr[15];}    //return adress of current instruction
	uint32_t GetSP(){return gpr[13];}
   	uint32_t GetLR(){return gpr[14];}
	uint32_t GetSP_app(){return sp_app;}
	uint32_t GetSP_sys(){return sp_sys;}

	uint32_t GetSR() { return sr; }
	uint32_t GetSR_SS() { return (GetSR() & CONFIG::SR_SS_MASK) >> CONFIG::SR_SS_OFFSET; }
	uint32_t GetSR_H() { return (GetSR() & CONFIG::SR_H_MASK) >> CONFIG::SR_H_OFFSET; }
	uint32_t GetSR_J() { return (GetSR() & CONFIG::SR_J_MASK) >> CONFIG::SR_J_OFFSET; }
	uint32_t GetSR_DM() { return (GetSR() & CONFIG::SR_DM_MASK) >> CONFIG::SR_DM_OFFSET; }
	uint32_t GetSR_D() { return (GetSR() & CONFIG::SR_D_MASK) >> CONFIG::SR_D_OFFSET; }
	uint32_t GetSR_M2() { return (GetSR() & CONFIG::SR_M2_MASK) >> CONFIG::SR_M2_OFFSET; }
	uint32_t GetSR_M1() { return (GetSR() & CONFIG::SR_M1_MASK) >> CONFIG::SR_M1_OFFSET; }
	uint32_t GetSR_M0() { return (GetSR() & CONFIG::SR_M0_MASK) >> CONFIG::SR_M0_OFFSET; }
	uint32_t GetSR_M() { return (GetSR() & CONFIG::SR_M_MASK) >> CONFIG::SR_M_OFFSET; }
	bool IsPrivilegedMode() { return GetSR_M() != CONFIG::EXEC_MODE_APPLICATION; }
	uint32_t GetSR_EM() { return (GetSR() & CONFIG::SR_EM_MASK) >> CONFIG::SR_EM_OFFSET; }
	uint32_t GetSR_I3M() { return (GetSR() & CONFIG::SR_I3M_MASK) >> CONFIG::SR_I3M_OFFSET; }
	uint32_t GetSR_I2M() { return (GetSR() & CONFIG::SR_I2M_MASK) >> CONFIG::SR_I2M_OFFSET; }
	uint32_t GetSR_I1M() { return (GetSR() & CONFIG::SR_I1M_MASK) >> CONFIG::SR_I1M_OFFSET; }
	uint32_t GetSR_I0M() { return (GetSR() & CONFIG::SR_I0M_MASK) >> CONFIG::SR_I0M_OFFSET; }
	uint32_t GetSR_GM() { return (GetSR() & CONFIG::SR_GM_MASK) >> CONFIG::SR_GM_OFFSET; }
	uint32_t GetSR_R() { return (GetSR() & CONFIG::SR_R_MASK) >> CONFIG::SR_R_OFFSET; }
	uint32_t GetSR_T() { return (GetSR() & CONFIG::SR_T_MASK) >> CONFIG::SR_T_OFFSET; }
	uint32_t GetSR_L() { return (GetSR() & CONFIG::SR_L_MASK) >> CONFIG::SR_L_OFFSET; }
	uint32_t GetSR_Q() { return (GetSR() & CONFIG::SR_Q_MASK) >> CONFIG::SR_Q_OFFSET; }
	uint32_t GetSR_V() { return (GetSR() & CONFIG::SR_V_MASK) >> CONFIG::SR_V_OFFSET; }
	uint32_t GetSR_N() { return (GetSR() & CONFIG::SR_N_MASK) >> CONFIG::SR_N_OFFSET; }
	uint32_t GetSR_Z() { return (GetSR() & CONFIG::SR_Z_MASK) >> CONFIG::SR_Z_OFFSET; }
	uint32_t GetSR_C() { return (GetSR() & CONFIG::SR_C_MASK) >> CONFIG::SR_C_OFFSET; }
	
	uint32_t GetEvba() {return evba;}
	uint32_t GetAcba(){return acba;}
	uint32_t GetCpucr(){return cpucr;}
	uint32_t GetConfig0(){return config0;}
	uint32_t GetConfig1(){return config1;}
	uint32_t GetCount(){return count;}
	uint32_t GetCompare(){return compare;}
		
	void SetGPR(unsigned int reg_num, uint32_t val) { if(reg_num != 15) gpr[reg_num] = val; else Branch(val); }
	void SetSP(uint32_t val) { gpr[13] = val; }
	void SetLR(uint32_t val) { gpr[14]= val; }
	void SetSP_app(uint32_t val){sp_app=val;}
	void SetSP_sys(uint32_t val){sp_sys=val;}

	      
        
	void Branch(uint32_t target_addr) { npc = target_addr; }

	void SetHW_SR(uint32_t val) { sr = (val & ~CONFIG::HW_SR_MASK) | (val & CONFIG::HW_SR_MASK); }
	void SetHW_SR_SS(uint32_t val) { SetHW_SR((GetSR() & ~CONFIG::SR_SS_MASK) | ((val << CONFIG::SR_SS_OFFSET) & CONFIG::SR_SS_MASK)); }
	void SetHW_SR_H(uint32_t val) { SetHW_SR((GetSR() & ~CONFIG::SR_H_MASK) | ((val << CONFIG::SR_H_OFFSET) & CONFIG::SR_H_MASK)); }
	void SetHW_SR_J(uint32_t val) { SetHW_SR((GetSR() & ~CONFIG::SR_J_MASK) | ((val << CONFIG::SR_J_OFFSET) & CONFIG::SR_J_MASK)); }
	void SetHW_SR_DM(uint32_t val) { SetHW_SR((GetSR() & ~CONFIG::SR_DM_MASK) | ((val << CONFIG::SR_DM_OFFSET) & CONFIG::SR_DM_MASK)); }
	void SetHW_SR_D(uint32_t val) { SetHW_SR((GetSR() & ~CONFIG::SR_D_MASK) | ((val << CONFIG::SR_D_OFFSET) & CONFIG::SR_D_MASK)); }
	void SetHW_SR_M2(uint32_t val) { SetHW_SR((GetSR() & ~CONFIG::SR_M2_MASK) | ((val << CONFIG::SR_M2_OFFSET) & CONFIG::SR_M2_MASK)); }
	void SetHW_SR_M1(uint32_t val) { SetHW_SR((GetSR() & ~CONFIG::SR_M1_MASK) | ((val << CONFIG::SR_M1_OFFSET) & CONFIG::SR_M1_MASK)); }
	void SetHW_SR_M0(uint32_t val) { SetHW_SR((GetSR() & ~CONFIG::SR_M0_MASK) | ((val << CONFIG::SR_M0_OFFSET) & CONFIG::SR_M0_MASK)); }
	void SetHW_SR_M(uint32_t val) { SetHW_SR((GetSR() & ~CONFIG::SR_M_MASK) | ((val << CONFIG::SR_M_OFFSET) & CONFIG::SR_M_MASK)); }
	void SetHW_SR_EM(uint32_t val) { SetHW_SR((GetSR() & ~CONFIG::SR_EM_MASK) | ((val << CONFIG::SR_EM_OFFSET) & CONFIG::SR_EM_MASK)); }
	void SetHW_SR_I3M(uint32_t val) { SetHW_SR((GetSR() & ~CONFIG::SR_I3M_MASK) | ((val << CONFIG::SR_I3M_OFFSET) & CONFIG::SR_I3M_MASK)); }
	void SetHW_SR_I2M(uint32_t val) { SetHW_SR((GetSR() & ~CONFIG::SR_I2M_MASK) | ((val << CONFIG::SR_I2M_OFFSET) & CONFIG::SR_I2M_MASK)); }
	void SetHW_SR_I1M(uint32_t val) { SetHW_SR((GetSR() & ~CONFIG::SR_I1M_MASK) | ((val << CONFIG::SR_I1M_OFFSET) & CONFIG::SR_I1M_MASK)); }
	void SetHW_SR_I0M(uint32_t val) { SetHW_SR((GetSR() & ~CONFIG::SR_I0M_MASK) | ((val << CONFIG::SR_I0M_OFFSET) & CONFIG::SR_I0M_MASK)); }
	void SetHW_SR_GM(uint32_t val) { SetHW_SR((GetSR() & ~CONFIG::SR_GM_MASK) | ((val << CONFIG::SR_GM_OFFSET) & CONFIG::SR_GM_MASK)); }
	void SetHW_SR_R(uint32_t val) { SetHW_SR((GetSR() & ~CONFIG::SR_R_MASK) | ((val << CONFIG::SR_R_OFFSET) & CONFIG::SR_R_MASK)); }
	void SetHW_SR_T(uint32_t val) { SetHW_SR((GetSR() & ~CONFIG::SR_T_MASK) | ((val << CONFIG::SR_T_OFFSET) & CONFIG::SR_T_MASK)); }
	void SetHW_SR_L(uint32_t val) { SetHW_SR((GetSR() & ~CONFIG::SR_L_MASK) | ((val << CONFIG::SR_L_OFFSET) & CONFIG::SR_L_MASK)); }
	void SetHW_SR_Q(uint32_t val) { SetHW_SR((GetSR() & ~CONFIG::SR_Q_MASK) | ((val << CONFIG::SR_Q_OFFSET) & CONFIG::SR_Q_MASK)); }
	void SetHW_SR_V(uint32_t val) { SetHW_SR((GetSR() & ~CONFIG::SR_V_MASK) | ((val << CONFIG::SR_V_OFFSET) & CONFIG::SR_V_MASK)); }
	void SetHW_SR_N(uint32_t val) { SetHW_SR((GetSR() & ~CONFIG::SR_N_MASK) | ((val << CONFIG::SR_N_OFFSET) & CONFIG::SR_N_MASK)); }
	void SetHW_SR_Z(uint32_t val) { SetHW_SR((GetSR() & ~CONFIG::SR_Z_MASK) | ((val << CONFIG::SR_Z_OFFSET) & CONFIG::SR_Z_MASK)); }
	void SetHW_SR_C(uint32_t val) { SetHW_SR((GetSR() & ~CONFIG::SR_C_MASK) | ((val << CONFIG::SR_C_OFFSET) & CONFIG::SR_C_MASK)); }

	void SetSR(uint32_t val)
	{
		if(IsPrivilegedMode())
		{
			SetHW_SR((GetSR() & ~CONFIG::SR_MASK) | (val & CONFIG::SR_MASK));
		}
		else
		{
			SetHW_SR((GetSR() & ~CONFIG::SR_APP_MASK) | (val & CONFIG::SR_APP_MASK));
		}
	}

	void SetSR_SS(uint32_t val) { SetSR((GetSR() & ~CONFIG::SR_SS_MASK) | ((val << CONFIG::SR_SS_OFFSET) & CONFIG::SR_SS_MASK)); }
	void SetSR_H(uint32_t val) { SetSR((GetSR() & ~CONFIG::SR_H_MASK) | ((val << CONFIG::SR_H_OFFSET) & CONFIG::SR_H_MASK)); }
	void SetSR_J(uint32_t val) { SetSR((GetSR() & ~CONFIG::SR_J_MASK) | ((val << CONFIG::SR_J_OFFSET) & CONFIG::SR_J_MASK)); }
	void SetSR_DM(uint32_t val) { SetSR((GetSR() & ~CONFIG::SR_DM_MASK) | ((val << CONFIG::SR_DM_OFFSET) & CONFIG::SR_DM_MASK)); }
	void SetSR_D(uint32_t val) { SetSR((GetSR() & ~CONFIG::SR_D_MASK) | ((val << CONFIG::SR_D_OFFSET) & CONFIG::SR_D_MASK)); }
	void SetSR_M2(uint32_t val) { SetSR((GetSR() & ~CONFIG::SR_M2_MASK) | ((val << CONFIG::SR_M2_OFFSET) & CONFIG::SR_M2_MASK)); }
	void SetSR_M1(uint32_t val) { SetSR((GetSR() & ~CONFIG::SR_M1_MASK) | ((val << CONFIG::SR_M1_OFFSET) & CONFIG::SR_M1_MASK)); }
	void SetSR_M0(uint32_t val) { SetSR((GetSR() & ~CONFIG::SR_M0_MASK) | ((val << CONFIG::SR_M0_OFFSET) & CONFIG::SR_M0_MASK)); }
	void SetSR_M(uint32_t val) { SetSR((GetSR() & ~CONFIG::SR_M_MASK) | ((val << CONFIG::SR_M_OFFSET) & CONFIG::SR_M_MASK)); }
	void SetSR_EM(uint32_t val) { SetSR((GetSR() & ~CONFIG::SR_EM_MASK) | ((val << CONFIG::SR_EM_OFFSET) & CONFIG::SR_EM_MASK)); }
	void SetSR_I3M(uint32_t val) { SetSR((GetSR() & ~CONFIG::SR_I3M_MASK) | ((val << CONFIG::SR_I3M_OFFSET) & CONFIG::SR_I3M_MASK)); }
	void SetSR_I2M(uint32_t val) { SetSR((GetSR() & ~CONFIG::SR_I2M_MASK) | ((val << CONFIG::SR_I2M_OFFSET) & CONFIG::SR_I2M_MASK)); }
	void SetSR_I1M(uint32_t val) { SetSR((GetSR() & ~CONFIG::SR_I1M_MASK) | ((val << CONFIG::SR_I1M_OFFSET) & CONFIG::SR_I1M_MASK)); }
	void SetSR_I0M(uint32_t val) { SetSR((GetSR() & ~CONFIG::SR_I0M_MASK) | ((val << CONFIG::SR_I0M_OFFSET) & CONFIG::SR_I0M_MASK)); }
	void SetSR_GM(uint32_t val) { SetSR((GetSR() & ~CONFIG::SR_GM_MASK) | ((val << CONFIG::SR_GM_OFFSET) & CONFIG::SR_GM_MASK)); }
	void SetSR_R(uint32_t val) { SetSR((GetSR() & ~CONFIG::SR_R_MASK) | ((val << CONFIG::SR_R_OFFSET) & CONFIG::SR_R_MASK)); }
	void SetSR_T(uint32_t val) { SetSR((GetSR() & ~CONFIG::SR_T_MASK) | ((val << CONFIG::SR_T_OFFSET) & CONFIG::SR_T_MASK)); }
	void SetSR_L(uint32_t val) { SetSR((GetSR() & ~CONFIG::SR_L_MASK) | ((val << CONFIG::SR_L_OFFSET) & CONFIG::SR_L_MASK)); }
	void SetSR_Q(uint32_t val) { SetSR((GetSR() & ~CONFIG::SR_Q_MASK) | ((val << CONFIG::SR_Q_OFFSET) & CONFIG::SR_Q_MASK)); }
	void SetSR_V(uint32_t val) { SetSR((GetSR() & ~CONFIG::SR_V_MASK) | ((val << CONFIG::SR_V_OFFSET) & CONFIG::SR_V_MASK)); }
	void SetSR_N(uint32_t val) { SetSR((GetSR() & ~CONFIG::SR_N_MASK) | ((val << CONFIG::SR_N_OFFSET) & CONFIG::SR_N_MASK)); }
	void SetSR_Z(uint32_t val) { SetSR((GetSR() & ~CONFIG::SR_Z_MASK) | ((val << CONFIG::SR_Z_OFFSET) & CONFIG::SR_Z_MASK)); }
	void SetSR_C(uint32_t val) { SetSR((GetSR() & ~CONFIG::SR_C_MASK) | ((val << CONFIG::SR_C_OFFSET) & CONFIG::SR_C_MASK)); }
	
	      
        void SetEvba(uint32_t val) { evba=val;}
	void SetAcba(uint32_t val) { acba=val;}
	void SetCpucr(uint32_t val){cpucr=val;}
	void SetConfig0(uint32_t val){config0=val;}
	void SetConfig1(uint32_t val){config1=val;}
	void SetCount(uint32_t val){count=val;}
	void SetCompare(uint32_t val){compare=val;}
	void SetSS_RAR(uint32_t val){ss_rar=val;}
	void SetSS_RSR(uint32_t val){ss_rsr=val;}
	void SwitchExecutionMode(uint32_t execution_mode);
	uint32_t GetPriorityLevel(uint32_t execution_mode);
	
	
	//=====================================================================
	//=                  Client/Service setup methods                     =
	//=====================================================================
	
	virtual bool BeginSetup();
	virtual bool EndSetup();
	virtual void OnDisconnect();
	
	//=====================================================================
	//=                    execution handling methods                     =
	//=====================================================================

	void StepOneInstruction();
	virtual void Synchronize();
	virtual void Reset();
	virtual void Idle();

	//=====================================================================
	//=             memory access reporting control interface methods     =
	//=====================================================================

	virtual void RequiresMemoryAccessReporting(bool report);
	virtual void RequiresFinishedInstructionReporting(bool report) ;

	//=====================================================================
	//=               Programmer view memory access methods               =
	//=====================================================================
	
	virtual bool ReadMemory(typename CONFIG::address_t addr, void *buffer, uint32_t size);
	virtual bool WriteMemory(typename CONFIG::address_t addr, const void *buffer, uint32_t size);
	virtual bool InjectReadMemory(typename CONFIG::address_t addr, void *buffer, uint32_t size);
	virtual bool InjectWriteMemory(typename CONFIG::address_t addr, const void *buffer, uint32_t size);

	//=====================================================================
	//=                        Debugging stuffs                           =
	//=====================================================================

	virtual unisim::util::debug::Register *GetRegister(const char *name);
	virtual string Disasm(typename CONFIG::address_t addr, typename CONFIG::address_t& next_addr);
	string GetObjectFriendlyName(typename CONFIG::address_t addr);
	string GetFunctionFriendlyName(typename CONFIG::address_t addr);

	//=====================================================================
	//=               Hardware check/acknowledgement methods              =
	//=====================================================================
	
	void SetNMIREQ();
	void ResetNMIREQ();
	void SetIRQ(unsigned int irq);
	void ResetIRQ(unsigned int irq);

	//=====================================================================
	//=                     Memory access methods                         =
	//=====================================================================
	
	bool Fetch(typename CONFIG::address_t addr,void *buffer, uint32_t size);
	
	bool UintLoadByte(unsigned int rd,uint32_t addr);
	bool UintLoadHalfWord(unsigned int rd,uint32_t addr,bool swap);
	bool IntLoadWord(unsigned int rd,uint32_t addr,bool swap);
	bool SintLoadByte(unsigned int,uint32_t addr);
	bool SintLoadHalfWord(unsigned int rd,uint32_t addr,bool swap);
	bool LoadAndInsertByte(unsigned int rd,uint32_t address,uint8_t part);
	bool LoadAndInsertHalfWord(unsigned int rd,uint32_t address,uint8_t part);

	bool IntStoreByte(unsigned int rs,uint32_t addr);
	bool IntStoreHalfWord(unsigned int rs,uint32_t addr);
	bool IntStoreWord(unsigned int rs,uint32_t addr);

	bool MemReadByte(uint32_t adress,uint32_t& value);
	bool MemReadHalfWord(uint32_t adress,uint32_t& value);
	bool MemReadWord(uint32_t adress,uint32_t& value);
	bool MemWriteByte(uint32_t value,uint32_t adress);
	bool MemWriteHalfWord(uint32_t value,uint32_t adress);
	bool MemWriteWord(uint32_t value,uint32_t adress);
	
	//=====================================================================
	//=                         System call                               =
	//=====================================================================

	bool Breakpoint();

	//=====================================================================
	//=                                                                   =
	//=====================================================================

	bool EvaluateCond(uint8_t cond);	
	
protected:

	
	//=====================================================================
	//=                         Timers handling                           =
	//=====================================================================
	
	uint64_t GetMaxIdleTime() const
	{
		// TODO: compute the maximum idle time, i.e. the time without any internal timer interrupts
		return 0xffffffffffffffffULL;
	}

	uint64_t GetTimersDeadline() const
	{
		// TODO: compute the timers deadline, i.e. the time until an internal timer expires
		return 0xffffffffffffffffULL;
	}

	inline void RunTimers(uint64_t delta) ALWAYS_INLINE
	{
		// TODO: run timers
	}
	
	virtual void RunInternalTimers();
	
	//=====================================================================
	//=                        Debugging stuff                            =
	//=====================================================================

	Logger logger;
    /** indicates if the memory accesses require to be reported */
    bool requires_memory_access_reporting;
    /** indicates if the finished instructions require to be reported */
    bool requires_finished_instruction_reporting;
	
	inline bool IsVerboseSetup() const ALWAYS_INLINE { return CONFIG::DEBUG_ENABLE && CONFIG::DEBUG_SETUP_ENABLE && (verbose_all || verbose_setup); }
	inline bool IsVerboseInterrupt() const ALWAYS_INLINE { return CONFIG::DEBUG_ENABLE && CONFIG::DEBUG_INTERRUPT_ENABLE && (verbose_all || verbose_interrupt); }
	inline bool IsVerboseStep() const ALWAYS_INLINE { return CONFIG::DEBUG_ENABLE && CONFIG::DEBUG_STEP_ENABLE && (verbose_all || verbose_step); }
	
	//=====================================================================
	//=                      Bus access methods                           =
	//=====================================================================

	virtual bool IHSBRead(typename CONFIG::physical_address_t physical_addr, void *buffer, uint32_t size) = 0;
	virtual bool DHSBRead(typename CONFIG::physical_address_t physical_addr, void *buffer, uint32_t size) = 0;
	virtual bool DHSBWrite(typename CONFIG::physical_address_t physical_addr, const void *buffer, uint32_t size) = 0;

public:
	inline void ProcessExceptions(unisim::component::cxx::processor::avr32::avr32a::avr32uc::Operation<CONFIG> *operation) ALWAYS_INLINE;



private:	
	//=====================================================================
	//=                      Debugging stuffs                             =
	//=====================================================================
	bool verbose_all;
	bool verbose_setup;
	bool verbose_interrupt;
	bool verbose_step;
	uint64_t trap_on_instruction_counter;
	bool enable_trap_on_exception;
	typename CONFIG::address_t halt_on_addr;
	std::string halt_on;
	uint64_t max_inst;                                         //!< maximum number of instructions to execute

	map<string, unisim::util::debug::Register *> registers_registry;       //!< Every CPU register interfaces
	uint64_t instruction_counter;                              //!< Number of executed instructions

	inline uint64_t GetInstructionCounter() const ALWAYS_INLINE { return instruction_counter; }
	inline void MonitorLoad(typename CONFIG::address_t ea, uint32_t size) ALWAYS_INLINE;
	inline void MonitorStore(typename CONFIG::address_t ea, uint32_t size) ALWAYS_INLINE;

	//=====================================================================
	//=                         AVR32A registers                          =
	//=====================================================================


	uint32_t gpr[16];   // register file gpr[15]=pc,gpr[14]=lr , gpr[13]=sp
	uint32_t npc;       //address of next instruction
	uint32_t sp_app;      // sp shadow register for application mode
	uint32_t sp_sys;      // sp shadow register for privileged mode

        //required system registers

	uint32_t sr;      // stack register
        uint32_t evba;
	uint32_t acba;
	uint32_t cpucr;
	uint32_t config0;
	uint32_t config1;
	uint32_t count;
	uint32_t compare;
	 

	//optional system registers
	uint32_t ecr;
	uint32_t rsr_dbg;
	uint32_t rar_dbg;	
	uint32_t jecr;
	
 	uint32_t josp;
	uint32_t java_lv0,java_lv1,java_lv2,java_lv3,java_lv4,java_lv5,java_lv6,java_lv7;
	uint32_t jtba;
	uint32_t jtcr;
	uint32_t tlbehi;
	uint32_t tlbelo;
	uint32_t ptbr;
	uint32_t tlbear;
	uint32_t mmucr;
	uint32_t tlbarlo;
	uint32_t tlbarhi;
	uint32_t pccnt;
	uint32_t pcnt0,pcnt1;
	uint32_t pccr;
	uint32_t bear;
	uint32_t mpuar0,mpuar1,mpuar2,mpuar3,mpuar4,mpuar5,mpuar6,mpuar7;
	uint32_t mpupsr0,mpupsr1,mpupsr2,mpupsr3,mpupsr4,mpupsr5,mpupsr6,mpupsr7;
	uint32_t mpucra,mpucrb;
	uint32_t mpubra,mpubrb;
	uint32_t mpuapra,mpuaprb;
	uint32_t mpcr;
	uint32_t ss_status;
	uint32_t ss_adrf;
	uint32_t ss_adrr;
	uint32_t ss_adr0,ss_adr1;
	uint32_t ss_sp_sys;
	uint32_t ss_sp_app;
	uint32_t ss_rar;
	uint32_t ss_rsr;

	
	//=====================================================================
	//=                    CPU run-time parameters                        =
	//=====================================================================
	
	Parameter<uint64_t> param_max_inst;                   //!< linked to member max_inst
	Parameter<bool> param_verbose_all;
	Parameter<bool> param_verbose_setup;
	Parameter<bool> param_verbose_interrupt;
	Parameter<bool> param_verbose_step;
	Parameter<uint64_t> param_trap_on_instruction_counter;
	Parameter<bool> param_enable_trap_on_exception;
	Parameter<std::string> param_halt_on;

	//=====================================================================
	//=                    CPU run-time statistics                        =
	//=====================================================================

	Statistic<uint64_t> stat_instruction_counter;
};

} // end of namespace avr32uc
} // end of namespace avr32a
} // end of namespace avr32
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
