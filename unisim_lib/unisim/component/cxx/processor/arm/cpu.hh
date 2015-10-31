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
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND 
 *  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF 
 *  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: Yves Lhuillier (yves.lhuillier@cea.fr), Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 */

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_CPU_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_CPU_HH__

#include <unisim/component/cxx/processor/arm/psr.hh>
#include <unisim/component/cxx/processor/arm/extregbank.hh>
#include <unisim/util/endian/endian.hh>
#include <unisim/util/inlining/inlining.hh>
#include <map>
#include <inttypes.h>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace arm {

/** ModeInfo: compile time generation of ARM Modes parameters
 *
 * This meta class generates at compile-time parameters and structure
 * of Banked Registers for ARM Modes.
 */

template <uint32_t MAPPED>
struct ModeInfo
{
  static uint32_t const next = MAPPED & (MAPPED-1);
  static uint32_t const count = 1 + ModeInfo<next>::count;
  static uint32_t const flag = MAPPED ^ next;
  static unsigned GetBRIndex( uint32_t regflag ) { if (regflag == flag) return 0; return 1 + ModeInfo<next>::GetBRIndex( regflag ); }
};

template <> struct ModeInfo<0>
{
  static uint32_t const count = 0;
  static unsigned GetBRIndex( uint32_t regflag ) { throw "Illegal Banked Register"; return 0; }
};

/** Base class for the ARM family.
 *
 * This class is the base for all the cpu's of the ARM processor
 * family, for that purpose it defines the basic registers and
 * different methods to handle them.
 */

template <typename _CONFIG_>
struct CPU
{
  typedef _CONFIG_ CONFIG;
  typedef typename CONFIG::FPSCR fpscr_type;
  typedef typename CONFIG::F64   F64;
  typedef typename CONFIG::F32   F32;
  typedef typename CONFIG::U64   U64;
  typedef typename CONFIG::U32   U32;
  
  //=====================================================================
  //=                    Constructor/Destructor                         =
  //=====================================================================
	
  /** Constructor.
   * Resets the simulator state, requires the endianness of the processor
   *   to set address mungling. By default the processor is set to big
   *   endian, but it can be changed later.
   *
   * @param endianness the endianness to use
   */
  CPU( unisim::util::endian::endian_type endianness = unisim::util::endian::E_LITTLE_ENDIAN )
    : exception(0)
  {
    // Initialize general purpose registers
    for(unsigned int i = 0; i < num_log_gprs; i++)
      gpr[i] = 0;
    this->current_pc = 0;
    this->next_pc = 0;
    
    // Initialize ARM Modes (TODO: modes should be conditionnaly selected based on CONFIG)
    modes[0b10000] = new Mode( "usr" ); // User mode (No banked regs, using main regs)
    modes[0b10001] = new BankedMode<0b0111111100000000>( "fiq" ); // FIQ mode
    modes[0b10010] = new BankedMode<0b0110000000000000>( "irq" ); // IRQ mode
    modes[0b10011] = new BankedMode<0b0110000000000000>( "svc" ); // Supervisor mode
    modes[0b10110] = new BankedMode<0b0110000000000000>( "mon" ); // Monitor mode
    modes[0b10111] = new BankedMode<0b0110000000000000>( "abt" ); // Abort mode
    modes[0b11010] = new BankedMode<0b0110000000000000>( "hyp" ); // Hyp mode
    modes[0b11011] = new BankedMode<0b0110000000000000>( "und" ); // Undefined mode
    modes[0b11111] = new Mode( "sys" ); // System mode (No banked regs, using main regs)
    
    // Initialize address mungling
    cpsr.Set( E, endianness == unisim::util::endian::E_LITTLE_ENDIAN ? 0 : 1 );
  }

  /** Destructor.
   */
  ~CPU()
  {
    for (typename ModeMap::iterator itr = modes.begin(), end = modes.end(); itr != end; ++itr)
      delete itr->second;
  }
  
  /** ARM modes (Banked Registers)
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
  struct Mode
  {
    Mode( char const* _suffix ) : suffix( _suffix ) {} char const* suffix;
    virtual ~Mode() {}
    virtual void     SetBR( unsigned index, uint32_t value ) { throw 0; };
    virtual uint32_t GetBR( unsigned index ) { throw 0; return 0; };
    virtual void     SetSPSR( unsigned index, uint32_t value ) { throw 0; };
    virtual uint32_t GetSPSR( unsigned index ) { throw 0; return 0; };
    virtual void     Swap( CPU& cpu ) {};
  };
  
  template <uint32_t MAPPED>
  struct BankedMode : public Mode
  {
    BankedMode( char const* _suffix ) : Mode( _suffix ) {}
    uint32_t banked_regs[ModeInfo<MAPPED>::count];
    uint32_t spsr;

    virtual void     SetBR( unsigned index, uint32_t value ) { banked_regs[ModeInfo<MAPPED>::GetBRIndex( uint32_t(1) << index )] = value; };
    virtual uint32_t GetBR( unsigned index ) { return banked_regs[ModeInfo<MAPPED>::GetBRIndex( uint32_t(1) << index )]; };
    virtual void     SetSPSR( unsigned index, uint32_t value ) { spsr = value; };
    virtual uint32_t GetSPSR( unsigned index ) { return spsr; };
    virtual void     Swap( CPU& cpu )
    {
      for (unsigned idx = 0, bidx = 0; idx < num_log_gprs; ++idx)
        if ((MAPPED >> idx) & 1)
          std::swap( banked_regs[bidx++], cpu.gpr[idx] );
    }
  };
  
  /**************************************************************/
  /* Endian variables and methods                         START */
  /**************************************************************/

  /** Get the endian configuration of the processor.
   *
   * @return the endian being used
   */
  unisim::util::endian::endian_type
  GetEndianness()
  {
    return (this->cpsr.Get( E ) == 0) ? unisim::util::endian::E_LITTLE_ENDIAN : unisim::util::endian::E_BIG_ENDIAN;
  }

  /**************************************************************/
  /* Endian variables and methods                           END */
  /**************************************************************/

  /**************************************************************/
  /* Registers access methods    START                          */
  /**************************************************************/
		
  /* GPR access functions */
  /** Get the value contained by a GPR.
   *
   * @param id the register index
   * @return the value contained by the register
   */
  uint32_t GetGPR(uint32_t id) const
  {
    return gpr[id];
  }

  /** Assign a GPR with a value coming from the Execute stage (See
   * ARM's ALUWritePC).  In ARMv7 architectures this is interworking
   * except in thumb state.
   *
   * @param id the register index
   * @param val the value to set
   */
  void SetGPR(uint32_t id, uint32_t val)
  {
    if (id != 15) gpr[id] = val;
    else if (cpsr.Get( T )) this->Branch( val );
    else this->BranchExchange( val );
  }
	
  /** Assign a GPR with a value coming from the Memory stage.  From
   * ARMv5T architectures, this is always interworking (exchanging
   * branch when destination register is PC).
   *
   * @param id the register index
   * @param val the value to set
   */
  void SetGPR_mem(uint32_t id, uint32_t val)
  {
    if (id != 15) gpr[id] = val;
    else this->BranchExchange( val );
  }
  
  /** Sets the PC (and potentially exchanges mode ARM<->Thumb)
   *
   * @param val the value to set PC
   */
  void BranchExchange(uint32_t target)
  {
    this->cpsr.Set( T, target & 1 );
    this->Branch( target );
  }
	
  /** Sets the PC (and preserve mode)
   *
   * @param val the value to set PC
   */
  void Branch(uint32_t target)
  {
    this->next_pc = target & (this->cpsr.Get( T ) ? -2 : -4);
  }
	
  /** Gets the updated PC value (next PC as currently computed)
   *
   */
  uint32_t GetNPC()
  { return this->next_pc; }
	
  /* PSR access functions */
  
  /** Get the CPSR register.
   *
   * @return the CPSR structured register.
   */
  PSR&  CPSR() { return cpsr; };
  
  /*********************************************/
  /* Modes and Banked Registers access methods */
  /*********************************************/
  
  Mode& GetMode(uint8_t mode)
  {
    typename ModeMap::iterator itr = modes.find(mode);
    if (itr == modes.end()) throw 0;
    return *(itr->second);
  }
  
  /** Get the value contained by a banked register GPR.  Returns the
   * value contained by a banked register.  It is the same than GetGPR
   * but mode can be different from the running mode.
   *
   * @param mode the mode of banked register
   * @param idx the register index
   * @return the value contained by the register
   */
  uint32_t GetBankedRegister( uint8_t foreign_mode, uint32_t idx )
  {
    uint8_t running_mode = cpsr.Get( M );
    if (running_mode == foreign_mode) return GetGPR( idx );
    GetMode(running_mode).Swap(*this); // OUT
    GetMode(foreign_mode).Swap(*this); // IN
    uint32_t value = GetGPR( idx );
    GetMode(foreign_mode).Swap(*this); // OUT
    GetMode(running_mode).Swap(*this); // IN
    return value;
  }
  
  /** Set the value contained by a user GPR.
   * Sets the value contained by a user GPR. It is the same than SetGPR byt
   *   restricting the index from 0 to 15 (only the first 16 registers).
   *
   * @param mode the mode of banked register
   * @param idx the register index
   * @param val the value to set
   */
  void SetBankedRegister( uint8_t foreign_mode, uint32_t idx, uint32_t value )
  {
    uint8_t running_mode = cpsr.Get( M );
    if (running_mode == foreign_mode) return SetGPR( idx, value );
    GetMode(running_mode).Swap(*this); // OUT
    GetMode(foreign_mode).Swap(*this); // IN
    SetGPR( idx, value );
    GetMode(foreign_mode).Swap(*this); // OUT
    GetMode(running_mode).Swap(*this); // IN
  }
  
  /*************************************/
  /* IT Conditional State manipulation */
  /*************************************/
  
  /** Determine wether the processor instruction stream is inside an
   * IT block.
   */
  bool itblock() const { return cpsr.InITBlock(); }
  
  /** Return the current condition associated to the IT state of the
   * processor.
   */
  uint32_t itcond() const { return cpsr.ITGetCondition(); }
  
  bool m_isit; /* determines wether current instruction is an IT one. */
  void ITSetState( uint32_t cond, uint32_t mask )
  {
    this->cpsr.ITSetState( cond, mask );
    m_isit = true;
  }
  void ITAdvance()
  {
    if (m_isit)
      this->m_isit = false;
    else if (this->itblock())
      this->cpsr.ITAdvance();
  }

  /**************************************************************/
  /* Registers access methods    END                            */
  /**************************************************************/

  /** Mark an exception in the virtual exception vector.
   * This marks an new exception in the virtual exception vector for 
   *   later treatment.
   *   NOTE: exception types are available at cxx/processor/arm/exception.hh
   *
   * @param except the exception to mark
   */
  void MarkVirtualExceptionVector(uint32_t except)
  {
    exception |= except;
  }
  
  /** Get the virtual exception vector.
   * This returns the value of the virtual exception vector.
   *
   * @return the value of the exception vector
   */
  uint32_t GetVirtualExceptionVector()
  {
    return exception;
  }
  
  /** Reset the value of the virtual exception vector.
   *
   * @param mask the value to set at reset
   */
  void ResetVirtualExceptionVector(uint32_t mask = 0)
  {
    exception = mask;
  }
  
  /*
   * ARM architecture constants
   */
    
  /* specific register indices */
  static unsigned int const PC_reg = 15;
  static unsigned int const LR_reg = 14;
  static unsigned int const SP_reg = 13;

  /* masks for the different running modes */
  static uint32_t const RUNNING_MODE_MASK = 0x1F;
  static uint32_t const USER_MODE = 0x10;
  static uint32_t const FIQ_MODE = 0x11;
  static uint32_t const IRQ_MODE = 0x12;
  static uint32_t const SUPERVISOR_MODE = 0x13;
  static uint32_t const ABORT_MODE = 0x17;
  static uint32_t const UNDEFINED_MODE = 0x1B;
  static uint32_t const SYSTEM_MODE = 0x1F;
  /* values of the different condition codes */
  static uint32_t const COND_EQ = 0x00;
  static uint32_t const COND_NE = 0x01;
  static uint32_t const COND_CS_HS = 0x02;
  static uint32_t const COND_CC_LO = 0x03;
  static uint32_t const COND_MI = 0x04;
  static uint32_t const COND_PL = 0x05;
  static uint32_t const COND_VS = 0x06;
  static uint32_t const COND_VC = 0x07;
  static uint32_t const COND_HI = 0x08;
  static uint32_t const COND_LS = 0x09;
  static uint32_t const COND_GE = 0x0a;
  static uint32_t const COND_LT = 0x0b;
  static uint32_t const COND_GT = 0x0c;
  static uint32_t const COND_LE = 0x0d;
  static uint32_t const COND_AL = 0x0e;

protected:
  /*
   * Memory access variables
   */
  
  /* Storage for Modes and banked registers */
  typedef std::map<uint8_t, Mode*> ModeMap;
  ModeMap modes;
  
  /** Storage for the logical registers */
  const static uint32_t num_log_gprs = 16;
  uint32_t gpr[num_log_gprs];
  uint32_t current_pc, next_pc;
  
  /** PSR registers. */
  PSR      cpsr;
  
  /** Exception vector.
   * This is a virtual exception vector (it doesn't exists as such in the arm
   *   architecture) to rapidly set and check exceptions.
   *   NOTE: exceptions are defined at cxx/arm/exception.hh
   */
  uint32_t exception;

public:
  // VFP/NEON registers
  fpscr_type fpscr;
  fpscr_type& FPSCR() { return fpscr; }

  struct ExtRegBank
  {
    ExtRegCache<U32,64> eu32;
    ExtRegCache<U64,32> eu64;
    ExtRegCache<F32,32> ef32;
    ExtRegCache<F64,32> ef64;

    template <typename CMD>
    void DoAll( CMD& cmd )
    {
      eu32.Do( cmd );
      eu64.Do( cmd );
      ef32.Do( cmd );
      ef64.Do( cmd );
    }
  } erb;

  U32  GetVU32( unsigned idx ) { return erb.eu32.GetReg( erb, idx ); }
  void SetVU32( unsigned idx, U32 val ) { erb.eu32.SetReg( erb, idx, val ); }
  U64  GetVU64( unsigned idx ) { return erb.eu64.GetReg( erb, idx ); }
  void SetVU64( unsigned idx, U64 val ) { erb.eu64.SetReg( erb, idx, val ); }
  F32  GetVSR( unsigned idx ) { return erb.ef32.GetReg( erb, idx ); }
  void SetVSR( unsigned idx, F32 val )    { erb.ef32.SetReg( erb, idx, val ); }
  F64  GetVDR( unsigned idx ) { return erb.ef64.GetReg( erb, idx ); }
  void SetVDR( unsigned idx, F64 val )   { erb.ef64.SetReg( erb, idx, val ); }

};
	
} // end of namespace arm
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_CPU_HH__

