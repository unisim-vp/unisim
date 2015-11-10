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

#include <unisim/component/cxx/processor/arm/cache.hh>
#include <unisim/component/cxx/processor/arm/extregbank.hh>
#include <unisim/component/cxx/processor/arm/psr.hh>
#include <unisim/component/cxx/processor/arm/cp15.hh>
#include <unisim/component/cxx/processor/arm/hostfloat.hh>
#include <unisim/service/interfaces/memory_access_reporting.hh>
#include <unisim/service/interfaces/trap_reporting.hh>
#include <unisim/kernel/logger/logger.hh>
#include <unisim/util/endian/endian.hh>
#include <unisim/util/inlining/inlining.hh>
#include <unisim/service/interfaces/registers.hh>
#include <unisim/util/debug/register.hh>
#include <map>
#include <inttypes.h>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace arm {

/** Base class for the ARM family.
 *
 * This class is the base for all the cpu's of the ARM processor
 * family, for that purpose it defines the basic registers and
 * different methods to handle them.
 */

template <typename CONFIG>
struct CPU
  : public virtual unisim::kernel::service::Object
  , public unisim::kernel::service::Service<unisim::service::interfaces::Registers>
{
  typedef CONFIG Config;
  typedef unisim::component::cxx::processor::arm::hostfloat::FPSCR fpscr_type;
  typedef double   F64;
  typedef float    F32;
  typedef uint8_t  U8;
  typedef uint16_t U16;
  typedef uint32_t U32;
  typedef uint64_t U64;
  typedef int8_t   S8;
  typedef int16_t  S16;
  typedef int32_t  S32;
  typedef int64_t  S64;
  
  /*
   * ARM architecture constants
   */
    
  /* specific register indices */
  static unsigned int const PC_reg = 15;
  static unsigned int const LR_reg = 14;
  static unsigned int const SP_reg = 13;

  /* masks for the different running modes */
  static uint32_t const USER_MODE = 0b10000;
  static uint32_t const FIQ_MODE = 0b10001;
  static uint32_t const IRQ_MODE = 0b10010;
  static uint32_t const SUPERVISOR_MODE = 0b10011;
  static uint32_t const MONITOR_MODE = 0b10110;
  static uint32_t const ABORT_MODE = 0b10111;
  static uint32_t const HYPERVISOR_MODE = 0b11010;
  static uint32_t const UNDEFINED_MODE = 0b11011;
  static uint32_t const SYSTEM_MODE = 0b11111;
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
  /* mask for valid bits in processor control and status registers */
  static uint32_t const PSR_UNALLOC_MASK = 0xff0fffff;
  /* Number of logic registers */
  static unsigned const num_log_gprs = 16;
  
  
  
  /** Base class for the ARM Modes
   *
   * This class is the base for all ARM Modes specifying the interface
   * of Mode related operation. Basically it specifies accessors for
   * banked registers and SPSR. It also introduce an internal swaping
   * mechanism to save/restore bancked registers.
   */
  
  struct Mode
  {
    Mode( char const* _suffix ) : suffix( _suffix ) {} char const* suffix;
    virtual ~Mode() {}
    virtual void     SetBR( unsigned index, uint32_t value ) { throw 0; };
    virtual uint32_t GetBR( unsigned index ) { throw 0; return 0; };
    virtual void     SetSPSR(uint32_t value ) { throw 0; };
    virtual uint32_t GetSPSR() { throw 0; return 0; };
    virtual void     Swap( CPU& cpu ) {};
  };
  
  //=====================================================================
  //=                       Logger                                      =
  //=====================================================================
  
  /** Unisim logging services. */
  unisim::kernel::logger::Logger logger;
  
  //=====================================================================
  //=                  public service imports/exports                   =
  //=====================================================================
		
  /** Instruction counter trap reporting service import. */
  unisim::kernel::service::ServiceImport<unisim::service::interfaces::TrapReporting>   instruction_counter_trap_reporting_import;
  
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
  CPU(const char* name, Object* parent);

  /** Destructor. */
  ~CPU()
  {
    for (typename ModeMap::iterator itr = modes.begin(), end = modes.end(); itr != end; ++itr)
      delete itr->second;
  }
  
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
  
  Mode& CurrentMode() { return GetMode(cpsr.Get(M)); }
  
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
  bool itblock() const { return CONFIG::insnsT2 ? cpsr.InITBlock() : false; }
  
  /** Return the current condition associated to the IT state of the
   * processor.
   */
  uint32_t itcond() const { return CONFIG::insnsT2 ? cpsr.ITGetCondition() : COND_AL; }
  
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

  /*************************/
  /* Memory access methods */
  /*************************/

  /** Processor external memory write.
   * Perform an external write memory access, that is an access that is not
   *   in cache (or cache absent).
   *
   * @param addr the address of the access
   * @param buffer byte buffer with the data to write
   * @param size the size of the access
   */
  virtual void PrWrite( uint32_t addr, uint8_t const* buffer, uint32_t size ) = 0;
  /** Processor external memory read.
   * Perform an external read memory access, that is an access that is not
   *   in cache (or cache absent).
   *
   * @param addr the address of the access
   * @param buffer byte buffer that will be filled with the read data
   * @param size the size of the access
   */
  virtual void PrRead( uint32_t addr, uint8_t* buffer, uint32_t size ) = 0;

  /** 32bits memory read.
   *
   * This method reads 32bits from memory and returns a
   * corresponding pending memory operation.
   * 
   * @param address the base address of the 32bits read
   * 
   * @return a pointer to the pending memory operation
   */
  uint32_t MemRead32( uint32_t address ) { return PerformReadAccess( address, 4, false ); }
  
  /** 16bits memory read.
   * 
   * This method reads 16bits from memory and returns a
   * corresponding pending memory operation.
   * 
   * @param address the base address of the 16bits read
   * 
   * @return a pointer to the pending memory operation
   */
  uint32_t MemRead16( uint32_t address ) { return PerformReadAccess( address, 2, false ); }
  /** Signed 16bits memory read.
   *
   * This method reads 16bits from memory and return a
   * corresponding signed pending memory operation.
   * 
   * @param address the base address of the 16bits read
   * 
   * @return a pointer to the pending memory operation
   */
  uint32_t MemReadS16( uint32_t address ) { return PerformReadAccess( address, 2, true ); }
  /** 8bits memory read.
   *
   * This method reads 8bits from memory and returns a
   * corresponding pending memory operation.
   * 
   * @param address the base address of the 8bits read
   * 
   * @return a pointer to the pending memory operation
   */
  uint32_t MemRead8( uint32_t address ) { return PerformReadAccess( address, 1, false ); }
  /** Signed 8bits memory read.
   *
   * This method reads 8bits from memory and returns a
   * corresponding signed pending memory operation.
   * 
   * @param address the base address of the 8bits read
   * 
   * @return a pointer to the pending memory operation
   */
  uint32_t MemReadS8( uint32_t address ) { return PerformReadAccess( address, 1, true ); }

  /** 32bits memory write.
   * This method write the giving 32bits value into the memory system.
   * 
   * @param address the base address of the 32bits write
   * @param value the value to write into memory
   */
  void MemWrite32( uint32_t address, uint32_t value ) { PerformWriteAccess( address, 4, value ); }
  /** 16bits memory write.
   * This method write the giving 16bits value into the memory system.
   * 
   * @param address the base address of the 16bits write
   * @param value the value to write into memory
   */
  void MemWrite16( uint32_t address, uint16_t value ) { PerformWriteAccess( address, 2, value ); }
  /** 8bits memory write.
   * This method write the giving 8bits value into the memory system.
   * 
   * @param address the base address of the 8bits write
   * @param value the value to write into memory
   */
  void MemWrite8( uint32_t address, uint8_t value ) { PerformWriteAccess( address, 1, value ); }

  virtual void ReportMemoryAccess( unisim::util::debug::MemoryAccessType mat, unisim::util::debug::MemoryType mtp, uint32_t addr, uint32_t size ) = 0;
  
  /**********************/
  /* Exception handling */
  /**********************/
  
  /** Unpredictable Instruction Behaviour.
   * This method is just called when an unpredictable behaviour is detected to
   *   notifiy the processor.
   */
  void UnpredictableInsnBehaviour();

  /** Mark an exception in the virtual exception vector.
   * This marks an new exception in the virtual exception vector for 
   *   later treatment.
   *   NOTE: exception types are available at cxx/processor/arm/exception.hh
   *
   * @param except the exception to mark
   */
  template <class RF>
  void MarkVirtualExceptionVector( RF const& rf )
  {
    rf.Set( exception, uint32_t( 1 ) );
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
  
  /**************************/
  /* CP15 Interface   START */
  /**************************/

public:
  /** Read the value of a CP15 coprocessor register
   *
   * @param crn     the "crn" field of the instruction code
   * @param opcode1 the "opcode1" field of the instruction code 
   * @param crm     the "crm" field of the instruction code
   * @param opcode2 the "opcode2" field of the instruction code
   * @return        the read value
   */
  uint32_t CP15ReadRegister( uint8_t crn, uint8_t opcode1, uint8_t crm, uint8_t opcode2 );
  
  /** Write a value in a CP15 coprocessor register
   * 
   * @param crn     the "crn" field of the instruction code
   * @param opcode1 the "opcode1" field of the instruction code
   * @param crm     the "crm" field of the instruction code
   * @param opcode2 the "opcode2" field of the instruction code
   * @param val     value to be written to the register
   */
  void     CP15WriteRegister( uint8_t crn, uint8_t opcode1, uint8_t crm, uint8_t opcode2, uint32_t value );
    
  /** Describe the nature of a CP15 coprocessor register
   * 
   * @param crn     the "crn" field of the instruction code
   * @param opcode1 the "opcode1" field of the instruction code
   * @param crm     the "crm" field of the instruction code
   * @param opcode2 the "opcode2" field of the instruction code
   * @return        a C string describing the CP15 register
   */
  char const* CP15DescribeRegister( uint8_t crn, uint8_t opcode1, uint8_t crm, uint8_t opcode2 );
  
  /** Get caches info
   *
   */
  void GetCacheInfo( bool &unified, uint32_t &isize, uint32_t &iassoc, uint32_t &ilen, uint32_t &dsize, uint32_t &dassoc, uint32_t &dlen );
  /** Drain write buffer.
   * Perform a memory barrier by draining the write buffer.
   */
  void DrainWriteBuffer();
  /** Invalidate ICache single entry using MVA
   *
   * Perform an invalidation of a single entry in the ICache using the
   *   given address in MVA format.
   *
   * @param mva the address to invalidate
   */
  void InvalidateICacheSingleEntryWithMVA(uint32_t mva);
  /** Clean DCache single entry using MVA
   *
   * Perform a clean of a single entry in the DCache using the given
   *   address in MVA format.
   *
   * @param mva the address to clean
   * @param invalidate true if the line needs to be also invalidated
   */
  void CleanDCacheSingleEntryWithMVA(uint32_t mva, bool invalidate);
  /** Invalidate the caches.
   * Perform a complete invalidation of the instruction cache and/or the 
   *   data cache.
   *
   * @param insn_cache whether or not the instruction cache should be 
   *   invalidated
   * @param data_cache whether or not the data cache should be invalidated
   */
  void InvalidateCache(bool insn_cache, bool data_insn);
  /** Invalidate the TLBs.
   * Perform a complete invalidation of the unified TLB.
   */
  void InvalidateTLB();
  /** Test and clean DCache.
   * Perform a test and clean operation of the DCache.
   *
   * @return return true if the complete cache is clean, false otherwise
   */
  bool TestAndCleanDCache();
  /** Test, clean and invalidate DCache.
   * Perform a test and clean operation of the DCache, and invalidate the
   *   complete cache if it is clean.
   *
   * @return return true if the complete cache is clean, false otherwise
   */
  bool TestCleanAndInvalidateDCache();
  
protected:
  struct CP15Reg
  {
    virtual ~CP15Reg() {}
    virtual void Write( CPU& cpu, uint32_t value ) = 0;
    virtual uint32_t Read( CPU& cpu ) = 0;
    virtual char const* Describe() = 0;
  };
  
  /** Get the Internal representation of the CP15Register
   * 
   * @param crn     the "crn" field of the instruction code
   * @param opcode1 the "opcode1" field of the instruction code
   * @param crm     the "crm" field of the instruction code
   * @param opcode2 the "opcode2" field of the instruction code
   * @return        an internal CP15Reg
   */
  virtual CP15Reg& CP15GetRegister( uint8_t crn, uint8_t opcode1, uint8_t crm, uint8_t opcode2 );
    
  /**************************/
  /* CP15 Interface     END */
  /**************************/

public:
  /** Instruction cache */
  Cache icache;
  /** Data cache */
  Cache dcache;
		
protected:
  /*
   * Memory access variables
   */
  
  /** Performs a prefetch access.
   * @param addr the address of the memory prefetch access
   */
  void PerformPrefetchAccess( uint32_t addr );


  /** Performs a write access.
   * @param addr the address of the memory write access
   * @param size the size of the memory write access
   * @param value the value of the memory write access
   */
  void PerformWriteAccess( uint32_t addr, uint32_t size, uint32_t value );


  /** Performs a read access.
   * @param addr the address of the memory read access
   * @param size the size of the memory read access
   * @param _signed the nature of the memory read access (signed or unsigned)
   */
  uint32_t PerformReadAccess( uint32_t addr, uint32_t size, bool _signed );
  
  /* Storage for Modes and banked registers */
  typedef std::map<uint8_t, Mode*> ModeMap;
  ModeMap modes;
  
  /** Storage for the logical registers */
  uint32_t gpr[num_log_gprs];
  uint32_t current_pc, next_pc;
  
  /** PSR registers */
  PSR      cpsr;
  
  /** Exception vector.
   * This is a virtual exception vector (it doesn't exists as such in the arm
   *   architecture) to rapidly set and check exceptions.
   *   NOTE: exceptions are defined at cxx/arm/exception.hh
   */
  uint32_t exception;

  // /** CP15 */
  // CP15 cp15;
  uint32_t sctlr;

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

  /*************************************/
  /* Debug Registers             START */
  /*************************************/
  
public:

  virtual unisim::util::debug::Register* GetRegister( const char* name );
		
  unisim::kernel::service::ServiceExport<unisim::service::interfaces::Registers> registers_export;
  
protected:
  /** The registers interface for debugging purpose */
  typedef std::map<std::string, unisim::util::debug::Register*> RegistersRegistry;
  RegistersRegistry registers_registry;
  
  typedef std::set<unisim::util::debug::Register*> DebugRegisterPool;
  DebugRegisterPool debug_register_pool;
  typedef std::set<unisim::kernel::service::VariableBase*> VariableRegisterPool;
  VariableRegisterPool variable_register_pool;
  
  /*************************************/
  /* Debug Registers              END  */
  /*************************************/
  
  
};
	
} // end of namespace arm
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#define CP15ENCODE( CRN, OPC1, CRM, OPC2 ) ((OPC1 << 12) | (CRN << 8) | (CRM << 4) | (OPC2 << 0))

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_CPU_HH__
