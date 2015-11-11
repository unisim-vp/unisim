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
  , exception(0)
  , sctlr(0)
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
  
  // This implementation of the arm architecture can only run in user mode,
  //   so we can already set CPSR to that mode.
  cpsr.Set( M, USER_MODE );

  // Default value for sctlr (will be overwritten as needed by simulators)
  SCTLR::TE.Set(      sctlr, 0 ); // Thumb Exception enable
  SCTLR::AFE.Set(     sctlr, 0 ); // Access flag enable.
  SCTLR::TRE.Set(     sctlr, 0 ); // TEX remap enable
  SCTLR::NMFI.Set(    sctlr, 1 ); // Non-maskable FIQ (NMFI) support
  SCTLR::EE.Set(      sctlr, 0 ); // Exception Endianness.
  SCTLR::VE.Set(      sctlr, 0 ); // Interrupt Vectors Enable
  SCTLR::U.Set(       sctlr, 0 ); // Alignment Model (up to ARMv6)
  SCTLR::FI.Set(      sctlr, 1 ); // Fast interrupts configuration enable
  SCTLR::UWXN.Set(    sctlr, 0 ); // Unprivileged write permission implies PL1 XN (Virtualization Extensions)
  SCTLR::WXN.Set(     sctlr, 0 ); // Write permission implies XN (Virtualization Extensions)
  SCTLR::HA.Set(      sctlr, 0 ); // Hardware Access flag enable.
  SCTLR::RR.Set(      sctlr, 0 ); // Round Robin select
  SCTLR::V.Set(       sctlr, 0 ); // Vectors bit
  SCTLR::I.Set(       sctlr, 1 ); // Instruction cache enable
  SCTLR::Z.Set(       sctlr, 1 ); // Branch prediction enable.
  SCTLR::SW.Set(      sctlr, 1 ); // SWP and SWPB enable. This bit enables the use of SWP and SWPB instructions.
  SCTLR::B.Set(       sctlr, 0 ); // Endianness model (up to ARMv6)
  SCTLR::CP15BEN.Set( sctlr, 0 ); // CP15 barrier enable.
  SCTLR::C.Set(       sctlr, 1 ); // Cache enable. This is a global enable bit for data and unified caches.
  SCTLR::A.Set(       sctlr, 0 ); // Alignment check enable
  SCTLR::M.Set(       sctlr, 0 ); // MMU enable.
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
            if (SCTLR::C.Get( value ))
              cpu.logger << DebugInfo << "Dcache Enabled !!!!!!!!" << EndDebugInfo;
          }
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
    char const* Describe() { return ""; }
    void Write( CPU&, uint32_t ) {}
    uint32_t Read( CPU& ) { return 0; }
  } err;
  return err;
}
    
/** Unpredictable Instruction Behaviour.
 * This method is just called when an unpredictable behaviour is detected to
 *   notifiy the processor.
 */
template <class CONFIG>
void 
CPU<CONFIG>::UnpredictableInsnBehaviour()
{
  logger << DebugWarning
    << "Trying to execute unpredictable behavior instruction,"
    << "Location: " 
    << __FUNCTION__ << ":" 
    << __FILE__ << ":" 
    << __LINE__ << ": "
    << EndDebugWarning;
  this->Stop( -1 );
}


    
} // end of namespace arm
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_CPU_TCC__
