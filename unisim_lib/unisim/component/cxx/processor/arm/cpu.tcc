/*
 *  Copyright (c) 2007-2015,
 *  Commissariat a l'Energie Atomique (CEA)
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without 
 *  modification, are permitted provided that the following conditions are met:
 *
 *   - Redistributions of source code must retain the above copyright notice, 
 *     this list of conditions and the following disclaimer.
 *
 *   - Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 *   - Neither the name of CEA nor the names of its contributors may be used to
 *     endorse or promote products derived from this software without specific 
 *     prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED.
 *  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY
 *  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
 *  SERVICES;LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 *  OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF 
 *  SUCH DAMAGE.
 *
 * Authors: Yves Lhuillier (yves.lhuillier@cea.fr), Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 */

 
#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_CPU_TCC__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_CPU_TCC__

#include <unisim/component/cxx/processor/arm/cpu.hh>
#include <unisim/component/cxx/processor/arm/disasm.hh>
#include <unisim/component/cxx/processor/arm/exception.hh>
#include <unisim/component/cxx/processor/arm/models.hh>
#include <unisim/component/cxx/processor/arm/cp15.hh>
#include <unisim/util/likely/likely.hh>
#include <unisim/util/arithmetic/arithmetic.hh>
#include <unisim/util/endian/endian.hh>
#include <unisim/util/debug/simple_register.hh>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace arm {

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugError;

/** ModeInfo: compile time generation for parameters of ARM modes with banked registers
 *
 * This meta class generates at compile-time parameters and structure
 * of Banked Registers for ARM Modes.
 */

template <uint32_t MAPPED>
struct ModeInfo
{
  static uint32_t const next = MAPPED & (MAPPED-1);
  static uint32_t const count = 1 + ModeInfo<next>::count;
};

template <> struct ModeInfo<0> { static uint32_t const count = 0; };

template <class CORE, uint32_t MAPPED>
struct BankedMode : public CORE::Mode
{
  BankedMode( char const* _suffix ) : CORE::Mode( _suffix ) {}
  uint32_t banked_regs[ModeInfo<MAPPED>::count];
  uint32_t spsr;

  virtual bool     HasBR( unsigned idx ) { return (MAPPED >> idx) & 1; }
  virtual bool     HasSPSR() { return true; }
  virtual void     SetSPSR(uint32_t value ) { spsr = value; };
  virtual uint32_t GetSPSR() { return spsr; };
  virtual void     Swap( CORE& cpu )
  {
    for (unsigned idx = 0, bidx = 0; idx < cpu.num_log_gprs; ++idx)
      if ((MAPPED >> idx) & 1) {
        uint32_t regval = cpu.GetGPR(idx);
        cpu.SetGPR( idx, banked_regs[bidx] );
        banked_regs[bidx] = regval;
        bidx += 1;
      }
  }
};

/** Constructor.
 * Initializes CPU
 *
 * @param name the name that will be used by the UNISIM service 
 *   infrastructure and will identify this object
 * @param parent the parent object of this object
 */
template <class CONFIG>
CPU<CONFIG>::CPU(const char *name, Object *parent)
  : unisim::kernel::service::Object(name, parent)
  , Service<Registers>(name, parent)
  , logger(*this)
  , verbose(false)
  , sctlr(0)
  , ttbr0(0)
  , registers_export("registers-export", this)
{
  // Initialize general purpose registers
  for(unsigned int i = 0; i < num_log_gprs; i++)
    gpr[i] = 0;
  this->current_pc = 0;
  this->next_pc = 0;
  
  /* ARM modes (Banked Registers)
   * At any given running mode only 16 registers are accessible.
   * The following list indicates the mapping per running modes.
   * - user:           0-14 (R0-R14),                  15 (PC)
   * - system:         0-14 (R0-R14),                  15 (PC)
   * - supervisor:     0-12 (R0-R12), 16-17 (R13-R14), 15 (PC)
   * - abort:          0-12 (R0-R12), 18-19 (R13-R14), 15 (PC)
   * - undefined:      0-12 (R0-R12), 20-21 (R13-R14), 15 (PC)
   * - interrupt:      0-12 (R0-R12), 22-23 (R13-R14), 15 (PC)
   * - fast interrupt: 0-7 (R0-R7),   24-30 (R8-R14),  15 (PC)
   */
  // Initialize ARM Modes (TODO: modes should be conditionnaly selected based on CONFIG)
  modes[0b10000] = new Mode( "usr" ); // User mode (No banked regs, using main regs)
  modes[0b10001] = new BankedMode<CPU,0b0111111100000000>( "fiq" ); // FIQ mode
  modes[0b10010] = new BankedMode<CPU,0b0110000000000000>( "irq" ); // IRQ mode
  modes[0b10011] = new BankedMode<CPU,0b0110000000000000>( "svc" ); // Supervisor mode
  modes[0b10110] = new BankedMode<CPU,0b0110000000000000>( "mon" ); // Monitor mode
  modes[0b10111] = new BankedMode<CPU,0b0110000000000000>( "abt" ); // Abort mode
  modes[0b11010] = new BankedMode<CPU,0b0110000000000000>( "hyp" ); // Hyp mode
  modes[0b11011] = new BankedMode<CPU,0b0110000000000000>( "und" ); // Undefined mode
  modes[0b11111] = new Mode( "sys" ); // System mode (No banked regs, using main regs)

  /*************************************/
  /* Registers Debug Accessors   START */
  /*************************************/
  
  {
    unisim::util::debug::Register* dbg_reg = 0;
    unisim::kernel::service::Register<uint32_t>* var_reg = 0;
  
    /** Specific Banked Register Debugging Accessor */
    struct BankedRegister : public unisim::util::debug::Register
    {
      BankedRegister(CPU& _cpu, std::string _name, uint8_t _mode, uint8_t _idx ) : cpu(_cpu), name(_name), mode(_mode), idx(_idx) {}
      virtual const char *GetName() const { return name.c_str(); }
      virtual void GetValue( void* buffer ) const { *((uint32_t*)buffer) = cpu.GetBankedRegister( mode, idx ); }
      virtual void SetValue( void const* buffer ) { cpu.SetBankedRegister( mode, idx, *((uint32_t*)buffer ) ); }
      virtual int  GetSize() const { return 4; }
      CPU&        cpu;
      std::string name;
      uint8_t     mode, idx;
    };
  
    // initialize the registers debugging interface for the first 15 registers
    for (unsigned idx = 0; idx < 15; idx++) {
      std::string name, pretty_name, description;
      { std::stringstream ss; ss << "r" << idx; name = ss.str(); }
      { std::stringstream ss; ss << DisasmRegister( idx ); pretty_name = ss.str(); }
      { std::stringstream ss; ss << "Logical register #" << idx << ": " << pretty_name; description = ss.str(); }
      dbg_reg = new unisim::util::debug::SimpleRegister<uint32_t>( pretty_name, &gpr[idx] );
      registers_registry[pretty_name] = dbg_reg;
      if (name != pretty_name) {
        registers_registry[name] = dbg_reg;
        description =  description + ", " + name;
      }
      var_reg = new unisim::kernel::service::Register<uint32_t>( pretty_name.c_str(), this, gpr[idx], description.c_str() );
      debug_register_pool.insert( dbg_reg );
      variable_register_pool.insert( var_reg );
      bool is_banked = false;
      for (typename ModeMap::const_iterator itr = modes.begin(), end = modes.end(); itr != end; ++itr) {
        if (not itr->second->HasBR( idx )) continue;
        is_banked = true;
        std::string br_name = name + '_' + itr->second->suffix;
        std::string br_pretty_name = name + '_' + itr->second->suffix;
        dbg_reg = new BankedRegister( *this, br_pretty_name, itr->first, idx );
        registers_registry[br_pretty_name] = dbg_reg;
        if (br_name != br_pretty_name)
          registers_registry[br_name] = dbg_reg;
        debug_register_pool.insert( dbg_reg );
      }
      if (is_banked) {
        std::string br_name = name + "_usr";
        std::string br_pretty_name = name + "_usr";
        dbg_reg = new BankedRegister( *this, br_pretty_name, USER_MODE, idx );
        registers_registry[br_pretty_name] = dbg_reg;
        if (br_name != br_pretty_name)
          registers_registry[br_name] = dbg_reg;
        debug_register_pool.insert( dbg_reg );
      }
    }
  
    /** Specific Program Counter Register Debugging Accessor */
    struct ProgramCounterRegister : public unisim::util::debug::Register
    {
      ProgramCounterRegister( CPU& _cpu ) : cpu(_cpu) {}
      virtual const char *GetName() const { return "pc"; }
      virtual void GetValue( void* buffer ) const { *((uint32_t*)buffer) = cpu.next_pc; }
      virtual void SetValue( void const* buffer ) { uint32_t address = *((uint32_t*)buffer); cpu.BranchExchange( address ); }
      virtual int  GetSize() const { return 4; }
      virtual void Clear() { /* Clear is meaning less for PC */ }
      CPU&        cpu;
    };

    dbg_reg = new ProgramCounterRegister( *this );
    registers_registry["pc"] = registers_registry["r15"] = dbg_reg;
    debug_register_pool.insert( dbg_reg );
    var_reg = new unisim::kernel::service::Register<uint32_t>( "pc", this, this->next_pc, "Logical Register #15: pc, r15" );
    variable_register_pool.insert( var_reg );
    
    // Handling the CPSR register
    dbg_reg = new unisim::util::debug::SimpleRegister<uint32_t>( "cpsr", &cpsr.m_value );
    registers_registry["cpsr"] = dbg_reg;
    debug_register_pool.insert( dbg_reg );
    var_reg = new unisim::kernel::service::Register<uint32_t>( "cpsr", this, this->cpsr.m_value, "Current Program Status Register" );
    variable_register_pool.insert( var_reg );
    
    /** Specific SPSR */
    struct SavedProgramStatusRegister : public unisim::util::debug::Register
    {
      SavedProgramStatusRegister( CPU& _cpu, std::string _name, uint8_t _mode ) : cpu(_cpu), name(_name), mode(_mode) {}
      virtual ~SavedProgramStatusRegister() {}
      virtual const char *GetName() const { return name.c_str(); }
      virtual void GetValue( void* buffer ) const { *((uint32_t*)buffer) = cpu.GetMode(mode).GetSPSR(); }
      virtual void SetValue( void const* buffer ) { cpu.GetMode(mode).SetSPSR( *((uint32_t*)buffer) ); }
      virtual int  GetSize() const { return 4; }
      CPU&        cpu;
      std::string name;
      uint8_t     mode;
    };
    
    for (typename ModeMap::const_iterator itr = modes.begin(), end = modes.end(); itr != end; ++itr) {
      if (not itr->second->HasSPSR()) continue;
      
      std::string name = std::string( "spsr_" ) + itr->second->suffix;
      dbg_reg = new SavedProgramStatusRegister( *this, name, itr->first );
      registers_registry[name] = dbg_reg;
      debug_register_pool.insert( dbg_reg );
    }
  }
  
  this->CPU<CONFIG>::CP15ResetRegisters();
}

/** Destructor.
 *
 * Takes care of releasing:
 *  - Debug Registers
 */
template <class CONFIG>
CPU<CONFIG>::~CPU()
{
  for (typename DebugRegisterPool::iterator itr = debug_register_pool.begin(), end = debug_register_pool.end(); itr != end; ++itr)
    delete *itr;
  for (typename VariableRegisterPool::iterator itr = variable_register_pool.begin(), end = variable_register_pool.end(); itr != end; ++itr)
    delete *itr;
}

/** Get a register by its name.
 * Gets a register interface to the register specified by name.
 *
 * @param name the name of the requested register
 *
 * @return a pointer to the RegisterInterface corresponding to name
 */
template <class CONFIG>
unisim::util::debug::Register*
CPU<CONFIG>::GetRegister(const char *name)
{
  RegistersRegistry::iterator itr = registers_registry.find( name );
  if (itr != registers_registry.end())
    return itr->second;
  else
    return 0;
}

/** Scan available registers for the Registers interface
 * 
 *  Allows clients of the Registers interface to scan available
 * register by providing a suitable RegisterScanner interface.
 */
template <class CONFIG>
void
CPU<CONFIG>::ScanRegisters( unisim::util::debug::RegisterScanner& scanner )
{
  scanner.Append( this->GetRegister( "r0" ) );
  scanner.Append( this->GetRegister( "r1" ) );
  scanner.Append( this->GetRegister( "r2" ) );
  scanner.Append( this->GetRegister( "r3" ) );
  scanner.Append( this->GetRegister( "r4" ) );
  scanner.Append( this->GetRegister( "r5" ) );
  scanner.Append( this->GetRegister( "r6" ) );
  scanner.Append( this->GetRegister( "r7" ) );
  scanner.Append( this->GetRegister( "r8" ) );
  scanner.Append( this->GetRegister( "r9" ) );
  scanner.Append( this->GetRegister( "r10" ) );
  scanner.Append( this->GetRegister( "r11" ) );
  scanner.Append( this->GetRegister( "r12" ) );
  scanner.Append( this->GetRegister( "sp" ) );
  scanner.Append( this->GetRegister( "lr" ) );
  scanner.Append( this->GetRegister( "pc" ) );
  // TODO: should expose CPSR (and most probably the APSR view)
  // scanner.Append( this->GetRegister( "cpsr" ) );
}


/** Software Interrupt
 *  This method is called by SWI instructions to handle software interrupts.
 */
template <class CONFIG>
void
CPU<CONFIG>::CallSupervisor( uint16_t imm )
{
  throw exception::SVCException();
}

/** Process Synchronous Exceptions
 *
 * Processes pending asynchronous exceptions and returns true if an
 * exception were taken. Note 1: this function should be called just
 * before PC update; next_pc should point at the next instruction to
 * execute whereas current_pc should point ot the instruction that
 * triggered the synchronous exception.
 *
 * @return true if an exception were taken, false if not (exception masked)
 */
template <class CONFIG>
void
CPU<CONFIG>::HandleSynchronousException()
{
  // if (unisim::component::cxx::processor::arm::exception::RESET.Get( masked_exception ))
  //   {
  //   }

  // else if (unisim::component::cxx::processor::arm::exception::PABRT.Get( masked_exception ))
  //   {
  //   }
  
  // else if (unisim::component::cxx::processor::arm::exception::UNDEF.Get( masked_exception ))
  //   {
  //   }

  // if (unisim::component::cxx::processor::arm::exception::DABRT.Get( masked_exception ))
  //   {
  //   }
  
  // else if (unisim::component::cxx::processor::arm::exception::SWI.Get( masked_exception ))
  //   {
  //   }

  
}

/** Process Asynchronous Exceptions
 *
 * Processes pending asynchronous exceptions and return processed
 * exception (at most one). Note 1: this is designed to be called just
 * before PC update; next_pc should point at the next instruction to
 * execute whereas current_pc is ignored (but should point to a
 * finished instruction, except PC update).
 *
 * @param exception the A|I|F exception vector (as in CPSR)
 * @return the exception vector corresponding to the processed esception
 */
template <class CONFIG>
uint32_t
CPU<CONFIG>::HandleAsynchronousException( uint32_t exceptions )
{
  // Asynchronous exceptions
  // - Asynchronous Abort
  // - FIQ
  // - IRQ
  
  // If we reached this point at least one exception is pending (but maybe masked).
  exceptions &= ~(cpsr.Get(ALL32));
  
  if (A.Get( exceptions ))
    {
      logger << DebugError << "Exception not handled (Asynchronous Abort)" << EndDebugError;
      
      unisim::kernel::service::Simulator::simulator->Stop(this, __LINE__);
      
      return A.Mask( exceptions );
    }
  
  if (I.Get( exceptions ) or F.Get( exceptions ))
    {
      // FIQs have higher priority
      bool isIRQ = not F.Get( exceptions );
      if (this->verbose)
        logger << DebugInfo << "Received " << (isIRQ ? "IRQ" : "FIQ") << " interrupt, handling it." << EndDebugInfo;
      
      TakePhysicalFIQorIRQException( isIRQ );
      
      return isIRQ ? I.Mask( exceptions ) : F.Mask( exceptions );
    }
  
  return 0;
}

/** Take Reset Exception
 */
template <class CONFIG>
void
CPU<CONFIG>::TakeReset()
{
  // Enter Supervisor mode and (if relevant) Secure state, and reset CP15. This affects
  // the Banked versions and values of various registers accessed later in the code.
  // Also reset other system components.
  cpsr.Set( M, SUPERVISOR_MODE );
  //if HaveSecurityExt() then SCR.NS = '0';
  
  //this->CP14ResetRegisters();
  this->CP15ResetRegisters();
  
  //if HaveAdvSIMDorVFP() then FPEXC.EN = '0'; SUBARCHITECTURE_DEFINED further resetting;
  //if HaveThumbEE() then TEECR.XED = '0';
  //if HaveJazelle() then JMCR.JE = '0'; SUBARCHITECTURE_DEFINED further resetting;
  // Further CPSR changes: all interrupts disabled, IT state reset, instruction set
  // and endianness according to the SCTLR values produced by the above call to
  // ResetControlRegisters().
  cpsr.Set( I, 1 );
  cpsr.Set( F, 1 );
  cpsr.Set( A, 1 );
  cpsr.ITSetState( 0b0000, 0b0000 ); // IT state reset
  cpsr.Set( J, 0 );
  cpsr.Set( T, SCTLR::TE.Get( sctlr ) );
  cpsr.Set( E, SCTLR::EE.Get( sctlr ) );
  // All registers, bits and fields not reset by the above pseudocode or by the
  // BranchTo() call below are UNKNOWN bitstrings after reset. In particular, the
  // return information registers R14_svc and SPSR_svc have UNKNOWN values, so that
  // it is impossible to return from a reset in an architecturally defined way.
  // Branch to Reset vector.
  uint32_t exc_vector_base = SCTLR::V.Get( sctlr ) ? 0xffff0000 : 0x00000000;
  Branch(exc_vector_base + 0);
}

/** Take Reset Exception
 */
template <class CONFIG>
void
CPU<CONFIG>::TakeSVCException()
{
  // Quote from the ARM doc:
  //
  //   Determine return information. SPSR is to be the current CPSR,
  // after changing the IT[] bits to give them the correct values for
  // the following instruction, and LR is to be the current PC minus 2
  // for Thumb or 4 for ARM, to change the PC offsets of 4 or 8
  // respectively from the address of the current instruction into the
  // required address of the next instruction, the SVC instruction
  // having size 2bytes for Thumb or 4 bytes for ARM.
  // 
  // Now what does that mean ?!?!?
  //
  //   Whatever the instruction set, the exception handler should
  // return to LR, in other words, LR should be next instruction.
  
  ITAdvance(); //< Finalize SVC instruction
  
  uint32_t new_lr_value = GetNPC();
  uint32_t new_spsr_value = cpsr.Get( ALL32 );
  uint32_t vect_offset = 0x8;
  
  // Check whether to take exception to Hyp mode
  // if in Hyp mode then stay in Hyp mode
  //take_to_hyp = (HaveVirtExt() && HaveSecurityExt() && SCR.NS == '1' && CPSR.M == '11010');
  // if HCR.TGE is set to 1, take to Hyp mode through Hyp Trap vector
  //route_to_hyp = (HaveVirtExt() && HaveSecurityExt() && !IsSecure() && HCR.TGE == '1'
  //                && CPSR.M == '10000'); // User mode
  // if HCR.TGE == '1' and in a Non-secure PL1 mode, the effect is UNPREDICTABLE
  // preferred_exceptn_return = new_lr_value;
  // if take_to_hyp then
  //     EnterHypMode(new_spsr_value, preferred_exceptn_return, vect_offset);
  // elsif route_to_hyp then
  //     EnterHypMode(new_spsr_value, preferred_exceptn_return, 20);
  // else
  
  // Enter Supervisor ('10011') mode, and ensure Secure state if initially in Monitor
  // ('10110') mode. This affects the Banked versions of various registers accessed later
  // in the code.
  
  // if CPSR.M == '10110' then SCR.NS = '0';
  CurrentMode().Swap( *this ); // OUT
  cpsr.Set( M, SUPERVISOR_MODE ); // CPSR.M = '10011';
  Mode& newmode = CurrentMode();
  newmode.Swap( *this ); // IN
  
  // Write return information to registers, and make further CPSR changes: IRQs disabled,
  // IT state reset, instruction set and endianness set to SCTLR-configured values.
  newmode.SetSPSR( new_spsr_value );
  SetGPR( 14, new_lr_value );
  cpsr.Set( I, 1 );
  cpsr.ITSetState( 0b0000, 0b0000 );
  cpsr.Set( J, 0 );
  cpsr.Set( T, SCTLR::TE.Get( sctlr ) ); // TE=0: ARM, TE=1: Thumb
  cpsr.Set( E, SCTLR::EE.Get( sctlr ) ); // EE=0: little-endian, EE=1: big-endian
  // Branch to SVC vector.
  uint32_t exc_vector_base = SCTLR::V.Get( sctlr ) ? 0xffff0000 : 0x00000000;
  Branch(exc_vector_base + vect_offset);
}

/** Take Physical FIQ or IRQ Exception
 *
 * @param isIRQ   whether the Exception is an IRQ (true) or an FIQ (false)
 */
template <class CONFIG>
void
CPU<CONFIG>::TakePhysicalFIQorIRQException( bool isIRQ )
{
  // Quote from the ARM doc:
  //
  //   Determine return information. SPSR is to be the current
  // CPSR, and LR is to be the current PC minus 0 for Thumb or 4
  // for ARM, to change the PC offsets of 4 or 8 respectively from
  // the address of the current instruction into the required
  // address of the instruction boundary at which the interrupt
  // occurred plus 4. For this purpose, the PC and CPSR are
  // considered to have already moved on to their values for the
  // instruction following that boundary.
  //
  // Now what does that mean ?!?!?
  //
  //   Whatever the instruction set, the exception handler should
  // return to LR-4, in other words, LR should be next instruction +4.
  uint32_t new_lr_value = GetNPC() + 4;
  uint32_t new_spsr_value = cpsr.Get( ALL32 );
  uint32_t vect_offset = isIRQ ? 0x18 : 0x1C;
      
  // TODO: [IRQ|FIQ]s may be routed to monitor (if
  // HaveSecurityExt() and SCR.[IRQ|FIQ]) or to Hypervisor (if
  // (HaveVirtExt() && HaveSecurityExt() && SCR.[IRQ|FIQ] == '0'
  // && HCR.[IMO|FMO] == '1' && !IsSecure()) || CPSR.M ==
  // '11010');
      
  // Handle in [IRQ|FIQ] mode. TODO: Ensure Secure state if
  // initially in Monitor mode. This affects the Banked versions
  // of various registers accessed later in the code.
  CurrentMode().Swap( *this ); // OUT
  cpsr.Set( M, isIRQ ? IRQ_MODE : FIQ_MODE );
  Mode& newmode = CurrentMode();
  newmode.Swap( *this ); // IN
  // Write return information to registers, and make further CPSR
  // changes: IRQs disabled, other interrupts disabled if
  // appropriate, IT state reset, instruction set and endianness
  // set to SCTLR-configured values.
  newmode.SetSPSR( new_spsr_value );
  SetGPR( 14, new_lr_value );
  // IRQs disabled
  cpsr.Set( I, 1 );
  // When taking FIQ, FIQs masked (if !HaveSecurityExt() || HaveVirtExt() || SCR.NS == '0' || SCR.FW == '1')
  if (not isIRQ)
    cpsr.Set( F, 1 );
  // Async Abort disabled (if !HaveSecurityExt() || HaveVirtExt() || SCR.NS == '0' || SCR.AW == '1')
  cpsr.Set( A, 1 );
  cpsr.ITSetState( 0b0000, 0b0000 ); // IT state reset
  cpsr.Set( J, 0 );
  cpsr.Set( T, SCTLR::TE.Get( sctlr ) );
  cpsr.Set( E, SCTLR::EE.Get( sctlr ) );
  // Branch to correct [IRQ|FIQ] vector ("implementation defined" if SCTLR.VE == '1').
  uint32_t exc_vector_base = SCTLR::V.Get( sctlr ) ? 0xffff0000 : 0x00000000;
  Branch(exc_vector_base + vect_offset);
}

/** Read the value of a CP15 coprocessor register
 *
 * @param crn     the "crn" field of the instruction code
 * @param opcode1 the "opcode1" field of the instruction code 
 * @param crm     the "crm" field of the instruction code
 * @param opcode2 the "opcode2" field of the instruction code
 * @return        the read value
 */
template <class CONFIG>
uint32_t
CPU<CONFIG>::CP15ReadRegister( uint8_t crn, uint8_t opcode1, uint8_t crm, uint8_t opcode2 )
{
  return CP15GetRegister( crn, opcode1, crm, opcode2 ).Read( *this );
}

/** Write a value in a CP15 coprocessor register
 * 
 * @param crn     the "crn" field of the instruction code
 * @param opcode1 the "opcode1" field of the instruction code
 * @param crm     the "crm" field of the instruction code
 * @param opcode2 the "opcode2" field of the instruction code
 * @param val     value to be written to the register
 */
template <class CONFIG>
void
CPU<CONFIG>::CP15WriteRegister( uint8_t crn, uint8_t opcode1, uint8_t crm, uint8_t opcode2, uint32_t value )
{
  return CP15GetRegister( crn, opcode1, crm, opcode2 ).Write( *this, value );
}

/** Describe the nature of a CP15 coprocessor register
 * 
 * @param crn     the "crn" field of the instruction code
 * @param opcode1 the "opcode1" field of the instruction code
 * @param crm     the "crm" field of the instruction code
 * @param opcode2 the "opcode2" field of the instruction code
 * @return        a C string describing the CP15 register
 */
template <class CONFIG>
char const*
CPU<CONFIG>::CP15DescribeRegister( uint8_t crn, uint8_t opcode1, uint8_t crm, uint8_t opcode2 )
{
  return CP15GetRegister( crn, opcode1, crm, opcode2 ).Describe();
}

/** Get the Internal representation of the CP15 Register
 * 
 * @param crn     the "crn" field of the instruction code
 * @param opcode1 the "opcode1" field of the instruction code
 * @param crm     the "crm" field of the instruction code
 * @param opcode2 the "opcode2" field of the instruction code
 * @return        an internal CP15Reg
 */
template <class CONFIG>
typename CPU<CONFIG>::CP15Reg&
CPU<CONFIG>::CP15GetRegister( uint8_t crn, uint8_t opcode1, uint8_t crm, uint8_t opcode2 )
{

  switch (CP15ENCODE( crn, opcode1, crm, opcode2 ))
    {
    case CP15ENCODE( 1, 0, 0, 0 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "SCTLR, System Control Register"; }
          /* TODO: handle SBO(DGP=0x00050078UL) and SBZ(DGP=0xfffa0c00UL)... */
          uint32_t Read( CPU& cpu ) { return cpu.sctlr; }
          void Write( CPU& cpu, uint32_t value ) {
            cpu.sctlr = value;
            if (SCTLR::C.Get( value ) and cpu.verbose)
              cpu.logger << DebugInfo << "Dcache Enabled !!!!!!!!" << EndDebugInfo;
          }
        } x;
        return x;
      } break;
      
    case CP15ENCODE( 2, 0, 0, 0 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "TTBR0, Translation Table Base Register 0"; }
          /* TODO: handle SBZ(DGP=0x00003fffUL)... */
          void Write( CPU& cpu, uint32_t value ) { cpu.ttbr0 = value; }
          uint32_t Read( CPU& cpu ) { return cpu.ttbr0; }
        } x;
        return x;
      } break;
      
    case CP15ENCODE( 3, 0, 0, 0 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "DACR, Domain Access Control Register"; }
          uint32_t Read( CPU& cpu ) { throw 0; return 0 /* cpu.dacr */; }
          void Write( CPU& cpu, uint32_t value ) { throw 0; /*cpu.dacr = value;*/ }
        } x;
        return x;
      } break;
      
    }

  logger << DebugError << "Unknown CP15 instruction: crn=" << crn << ", opc1=" << opcode1 << ", crm=" << crm << ", opc2=" << opcode2 << EndDebugError;
  this->Stop( -1 );
  
  static struct CP15Error : public CP15Reg {
    char const* Describe() { return "Unknown CP15 register"; }
    void Write( CPU&, uint32_t ) {}
    uint32_t Read( CPU& ) { return 0; }
  } err;
  return err;
}

/** Resets the internal values of corresponding CP15 Registers
 */
template <class CONFIG>
void
CPU<CONFIG>::CP15ResetRegisters()
{
  // Default value for sctlr (will be overwritten as needed by simulators)
  sctlr = 0x00c50058; // SBO mask
  SCTLR::TE.Set(      sctlr, 0 ); // Thumb Exception enable
  SCTLR::AFE.Set(     sctlr, 0 ); // Access flag enable.
  SCTLR::TRE.Set(     sctlr, 0 ); // TEX remap enable
  SCTLR::NMFI.Set(    sctlr, 0 ); // Non-maskable FIQ (NMFI) support
  SCTLR::EE.Set(      sctlr, 0 ); // Exception Endianness.
  SCTLR::VE.Set(      sctlr, 0 ); // Interrupt Vectors Enable
  SCTLR::U.Set(       sctlr, 1 ); // Alignment Model (0 before ARMv6, 0 or 1 in ARMv6, 1 in armv7)
  SCTLR::FI.Set(      sctlr, 0 ); // Fast interrupts configuration enable
  SCTLR::UWXN.Set(    sctlr, 0 ); // Unprivileged write permission implies PL1 XN (Virtualization Extensions)
  SCTLR::WXN.Set(     sctlr, 0 ); // Write permission implies XN (Virtualization Extensions)
  SCTLR::HA.Set(      sctlr, 0 ); // Hardware Access flag enable.
  SCTLR::RR.Set(      sctlr, 0 ); // Round Robin select
  SCTLR::V.Set(       sctlr, 0 ); // Vectors bit
  SCTLR::I.Set(       sctlr, 0 ); // Instruction cache enable
  SCTLR::Z.Set(       sctlr, 0 ); // Branch prediction enable.
  SCTLR::SW.Set(      sctlr, 0 ); // SWP and SWPB enable. This bit enables the use of SWP and SWPB instructions.
  SCTLR::B.Set(       sctlr, 0 ); // Endianness model (up to ARMv6)
  SCTLR::CP15BEN.Set( sctlr, 1 ); // CP15 barrier enable.
  SCTLR::C.Set(       sctlr, 0 ); // Cache enable. This is a global enable bit for data and unified caches.
  SCTLR::A.Set(       sctlr, 0 ); // Alignment check enable
  SCTLR::M.Set(       sctlr, 0 ); // MMU enable.
}
    
/** Unpredictable Instruction Behaviour.
 * This method is just called when an unpredictable behaviour is detected to
 *   notifiy the processor.
 */
template <class CONFIG>
void 
CPU<CONFIG>::UnpredictableInsnBehaviour()
{
  logger << DebugWarning << "Trying to execute unpredictable behavior instruction" << EndDebugWarning;
  this->Stop( -1 );
}


    
} // end of namespace arm
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_CPU_TCC__
