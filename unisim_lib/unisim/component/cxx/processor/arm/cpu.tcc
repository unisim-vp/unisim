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
  , instruction_counter_trap_reporting_import("instruction-counter-trap-reporting-import", this)
  , icache("icache", this)
  , dcache("dcache", this)
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

/** Performs a prefetch access.
 *
 * @param addr the address of the memory prefetch access
 */
template <class CONFIG>
void 
CPU<CONFIG>::PerformPrefetchAccess( uint32_t addr )
{
  if ( likely(dcache.GetSize()) )
    {
      dcache.prefetch_accesses++;
      uint32_t cache_tag = dcache.GetTag(addr);
      uint32_t cache_set = dcache.GetSet(addr);
      uint32_t cache_way;
      bool cache_hit = false;
      if ( dcache.GetWay(cache_tag, cache_set, &cache_way) )
        {
          if ( dcache.GetValid(cache_set, cache_way) )
            {
              // the access is a hit, nothing needs to be done
              cache_hit = true;
            }
        }
      // if the access was a miss, data needs to be fetched from main
      //   memory and placed into the cache
      if ( likely(!cache_hit) )
        {
          // get a way to replace
          cache_way = dcache.GetNewWay(cache_set);
          // get the valid and dirty bits from the way to replace
          bool cache_valid = dcache.GetValid(cache_set,
                                             cache_way);
          bool cache_dirty = dcache.GetDirty(cache_set,
                                             cache_way);
          if ( cache_valid & cache_dirty )
            {
              // the cache line to replace is valid and dirty so it needs
              //   to be sent to the main memory
              uint8_t *rep_cache_data = 0;
              uint32_t rep_cache_address =
                dcache.GetBaseAddress(cache_set,
                                      cache_way);
              dcache.GetData(cache_set, cache_way,
                             &rep_cache_data);
              PrWrite(rep_cache_address, rep_cache_data,
                      dcache.LINE_SIZE);
            }
          // the new data can be requested
          uint8_t *cache_data = 0;
          uint32_t cache_address =
            dcache.GetBaseAddressFromAddress(addr);
          // when getting the data we get the pointer to the cache line
          //   containing the data, so no need to write the cache
          //   afterwards
          uint32_t cache_line_size = dcache.GetData(cache_set,
                                                    cache_way, &cache_data);
          PrRead(cache_address, cache_data,
                 cache_line_size);
          dcache.SetTag(cache_set, cache_way, cache_tag);
          dcache.SetValid(cache_set, cache_way, 1);
          dcache.SetDirty(cache_set, cache_way, 0);
        }
      else
        {
          dcache.prefetch_hits++;
        }
      if ( unlikely(dcache.power_estimator_import != 0) )
        dcache.power_estimator_import->ReportReadAccess();
    }
  else
    {
      /* it is just a cache prefetch, ignore the request if there is 
       * no cache */
    }
  /* CHECK: should we report a memory access for a prefetch???? */
}

/** Performs a write access.
 * @param addr the address of the memory write access
 * @param size the size of the memory write access
 * @param value the value of the memory write access
 */
template <class CONFIG>
void 
CPU<CONFIG>::PerformWriteAccess( uint32_t addr, uint32_t size, uint32_t value )
{
  uint32_t write_addr = addr;
  uint8_t data[4];

  if (size > 4) throw 0; // should never happen

  if (GetEndianness() == unisim::util::endian::E_BIG_ENDIAN) {
    // fix the write address according to request size when big endian
    write_addr ^= ((-size) & 3);
    // TODO: need to check that because it seems astonishingly incorrect
    uint32_t shifter = value;
    for (int byte = size; --byte >= 0;)
      { data[byte] = shifter; shifter >>= 8; }
  } else {
    uint32_t shifter = value;
    for (int byte = 0; byte < int( size ); ++byte)
      { data[byte] = shifter; shifter >>= 8; }
  }

  if ( likely(dcache.GetSize()) )
    {
      dcache.write_accesses++;
      uint32_t cache_tag = dcache.GetTag(write_addr);
      uint32_t cache_set = dcache.GetSet(write_addr);
      uint32_t cache_way;
      bool cache_hit = false;
      if ( dcache.GetWay(cache_tag, cache_set, &cache_way) )
        {
          if ( dcache.GetValid(cache_set, cache_way) != 0 )
            {
              // the access is a hit
              cache_hit = true;
            }
        }
      // if the access was a hit the data needs to be written into
      //   the cache, if the access was a miss the data needs to be
      //   written into memory, but the cache doesn't need to be updated
      if ( likely(cache_hit) )
        {
          dcache.write_hits++;
          uint32_t cache_index = dcache.GetIndex(write_addr);
          dcache.SetData(cache_set, cache_way, cache_index,
                         size, data);
          dcache.SetDirty(cache_set, cache_way, 1);
        }
      else
        {
          PrWrite(write_addr, data, size);
        }

      if ( unlikely(dcache.power_estimator_import != 0) )
        dcache.power_estimator_import->ReportWriteAccess();
    }
  else // there is no data cache
    {
      // there is no data cache, so just send the request to the
      //   memory interface
      PrWrite(write_addr, data, size);
    }

  /* report read memory access if necessary */
  ReportMemoryAccess(unisim::util::debug::MAT_WRITE, unisim::util::debug::MT_DATA, addr, size);
}

/** Performs a read access.
 * @param addr the address of the memory read access
 * @param size the size of the memory read access
 * @param _signed the nature of the memory read access (signed or unsigned)
 */
template <class CONFIG>
uint32_t
CPU<CONFIG>::PerformReadAccess(	uint32_t addr, uint32_t size, bool _signed )
{
  uint32_t read_addr = addr & ~(uint32_t)(size - 1);
  uint8_t data[4];

  if (size > 4) throw 0;
  uint32_t misalignment = addr & (size-1);

  // fix the read address depending on the request size and endianess
  if (GetEndianness() == unisim::util::endian::E_BIG_ENDIAN) {
    // TODO: need to check that because it seems astonishingly
    // incorrect to perform address masking and endianness conversion
    read_addr ^= ((-size) & 3);
  }

  if ( likely(dcache.GetSize()) )
    {
      dcache.read_accesses++;
      uint32_t cache_tag = dcache.GetTag(read_addr);
      uint32_t cache_set = dcache.GetSet(read_addr);
      uint32_t cache_way;
      bool cache_hit = false;
      if ( dcache.GetWay(cache_tag, cache_set, &cache_way) )
        {
          if ( dcache.GetValid(cache_set, cache_way) )
            {
              // the access is a hit, nothing needs to be done
              cache_hit = true;
            }
        }
      // if the access was a miss, data needs to be fetched from main
      //   memory and placed into the cache
      if ( unlikely(!cache_hit) )
        {
          // get a way to replace
          cache_way = dcache.GetNewWay(cache_set);
          // get the valid and dirty bits from the way to replace
          uint8_t cache_valid = dcache.GetValid(cache_set,
                                                cache_way);
          uint8_t cache_dirty = dcache.GetDirty(cache_set,
                                                cache_way);

          if ( (cache_valid != 0) & (cache_dirty != 0) )
            {
              // the cache line to replace is valid and dirty so it needs
              //   to be sent to the main memory
              uint8_t *rep_cache_data = 0;
              uint32_t rep_cache_address =
                dcache.GetBaseAddress(cache_set, cache_way);
              dcache.GetData(cache_set, cache_way, &rep_cache_data);
              PrWrite(rep_cache_address, rep_cache_data,
                      dcache.LINE_SIZE);
            }
          // the new data can be requested
          uint8_t *cache_data = 0;
          uint32_t cache_address =
            dcache.GetBaseAddressFromAddress(read_addr);
          // when getting the data we get the pointer to the cache line
          //   containing the data, so no need to write the cache
          //   afterwards
          uint32_t cache_line_size = dcache.GetData(cache_set,
                                                    cache_way, &cache_data);
          PrRead(cache_address, cache_data,
                 cache_line_size);
          dcache.SetTag(cache_set, cache_way, cache_tag);
          dcache.SetValid(cache_set, cache_way, 1);
          dcache.SetDirty(cache_set, cache_way, 0);
        }
      else
        {
          // cache hit
          dcache.read_hits++;
        }

      // at this point the data is in the cache, we can read it from the
      //   cache
      uint32_t cache_index = dcache.GetIndex(read_addr);
      uint8_t* ptr;
      (void)dcache.GetData(cache_set, cache_way, cache_index, size, &ptr);
      memcpy( &data[0], ptr, size );
    }
  else // there is no data cache
    {
      // just read the data from the memory system
      PrRead(read_addr, &data[0], size);
    }

  uint32_t value;
  if (GetEndianness() == unisim::util::endian::E_LITTLE_ENDIAN) {
    uint32_t shifter = 0;
    for (int byte = size; --byte >= 0;)
      { shifter = (shifter << 8) | uint32_t( data[byte] ); }
    if ((size == 4) and (CONFIG::model < ARMV6) and misalignment)
      shifter = unisim::util::arithmetic::RotateLeft( shifter, misalignment*8 );
    value = shifter;
  } else {
    uint32_t shifter = 0;
    for (int byte = 0; byte < int( size ); ++byte)
      { shifter = (shifter << 8) | uint32_t( data[byte] ); }
    if ((size == 4) and (CONFIG::model < ARMV6) and misalignment)
      shifter = unisim::util::arithmetic::RotateRight( shifter, misalignment*8 );
    value = shifter;
  }

  if ( likely(dcache.GetSize()) )
    if ( unlikely(dcache.power_estimator_import != 0) )
      dcache.power_estimator_import->ReportReadAccess();

  /* report read memory access if necessary */
  ReportMemoryAccess(unisim::util::debug::MAT_READ, unisim::util::debug::MT_DATA, addr, size);
      
  return value;
}


/** Get caches info
 *
 */
template <class CONFIG>
void
CPU<CONFIG>::GetCacheInfo(bool &unified, 
                  uint32_t &isize, uint32_t &iassoc, uint32_t &ilen,
                  uint32_t &dsize, uint32_t &dassoc, uint32_t &dlen)
{
  unified = false;
  isize = icache.GetSize();
  iassoc = icache.GetNumWays();
  ilen = icache.LINE_SIZE;
  dsize = icache.GetSize();
  dassoc = dcache.GetNumWays();
  dlen = dcache.LINE_SIZE;
}

/** Drain write buffer.
 * Perform a memory barrier by draining the write buffer.
 */
template <class CONFIG>
void
CPU<CONFIG>::DrainWriteBuffer()
{
  logger << DebugWarning << "TODO: Drain write buffer once implemented" << EndDebugWarning;
}

/** Invalidate ICache single entry using MVA
 *
 * Perform an invalidation of a single entry in the ICache using the
 *   given address in MVA format.
 *
 * @param mva the address to invalidate
 */
template <class CONFIG>
void 
CPU<CONFIG>::InvalidateICacheSingleEntryWithMVA(uint32_t init_mva)
{
  uint32_t mva = init_mva & ~(uint32_t)(dcache.LINE_SIZE - 1);

  logger << DebugInfo
         << "Invalidating ICache single entry with MVA:" << std::endl
         << " - mva               = 0x" << std::hex << init_mva << std::endl
         << " - cache aligned mva = 0x" << mva << std::dec
         << EndDebugInfo;

  if ( likely(// SCTLR::C.Get( sctlr ) &&
              icache.GetSize()) )
    {
      uint32_t cache_tag = icache.GetTag(mva);
      uint32_t cache_set = icache.GetSet(mva);
      uint32_t cache_way = 0;
      bool cache_hit = false;
      if ( likely(icache.GetWay(cache_tag, cache_set, &cache_way)) )
        cache_hit = true;

      if ( likely(cache_hit) )
        {
          logger << DebugInfo
                 << "ICache hit (set = "
                 << cache_set << ", way = "
                 << cache_way << "), invalidating it"
                 << EndDebugInfo;
          icache.SetValid(cache_set, cache_way, 0);
        }
      else
        {
          logger << DebugInfo
                 << "ICache miss (set = "
                 << cache_set
                 << "), nothing to do"
                 << EndDebugInfo;
        }
    }
}

/** Clean DCache single entry using MVA
 *
 * Perform a clean of a single entry in the DCache using the given
 *   address in MVA format.
 *
 * @param mva the address to clean
 * @param invalidate true if the line needs to be also invalidated
 */
template <class CONFIG>
void 
CPU<CONFIG>::CleanDCacheSingleEntryWithMVA(uint32_t init_mva, bool invalidate)
{
  uint32_t mva = init_mva & ~(uint32_t)(dcache.LINE_SIZE - 1);
  uint32_t pa = mva;

  logger << DebugInfo
         << "Cleaning DCache single entry with MVA:" << std::endl
         << " - mva               = 0x" << std::hex << init_mva << std::endl
         << " - cache aligned mva = 0x" << mva << std::dec
         << EndDebugInfo;

  if ( likely(// cp15.IsDCacheEnabled() &&
              dcache.GetSize()) )
    {
      uint32_t cache_tag = dcache.GetTag(mva);
      uint32_t cache_set = dcache.GetSet(mva);
      uint32_t cache_way;
      bool cache_hit = false;
      if ( likely(dcache.GetWay(cache_tag, cache_set, &cache_way)) )
        cache_hit = true;

      if ( likely(cache_hit) )
        {
          logger << DebugInfo
                 << "Cache hit (set = "
                 << cache_set << ", way = "
                 << cache_way << ")"
                 << EndDebugInfo;
          uint8_t cache_dirty = dcache.GetDirty(cache_set, cache_way);

          if ( cache_dirty != 0 )
            {
              logger << DebugInfo
                     << "Line is dirty, performing a cleaning"
                     << EndDebugInfo;
              uint8_t *data = 0;
              // translate the address
              // if ( likely(cp15.IsMMUEnabled()) )
              //   TranslateMVA(mva, pa);
              dcache.GetData(cache_set, cache_way, &data);
              {
                logger << DebugInfo
                       << "Cleaning cache line:" << std::endl
                       << " - mva = 0x" << std::hex << mva << std::endl
                       << " - pa  = 0x" << pa << std::endl
                       << " - tag = 0x" << cache_tag << std::endl
                       << " - set = " << std::dec << cache_set << std::endl
                       << " - way = " << cache_way << std::endl
                       << " - data =" << std::hex;
                for ( unsigned int i = 0; i < dcache.LINE_SIZE; i++ )
                  logger << " " << (unsigned int)data[i];
                logger << std::dec
                       << EndDebugInfo;
              }
              PrWrite(pa, data, dcache.LINE_SIZE);
              dcache.SetDirty(cache_set, cache_way, 0);
            }
          else
            {
              logger << DebugInfo
                     << "Line is already cleaned, doing nothing"
                     << EndDebugInfo;
            }
          if ( invalidate )
            dcache.SetValid(cache_set, cache_way, 0);
        }
      else
        {
          logger << DebugInfo
                 << "Cache miss (set = "
                 << cache_set
                 << ")"
                 << EndDebugInfo;
        }
    }
}

/** Invalidate the caches.
 * Perform a complete invalidation of the instruction cache and/or the 
 *   data cache.
 *
 * @param insn_cache whether or not the instruction cache should be 
 *   invalidated
 * @param data_cache whether or not the data cache should be invalidated
 */
template <class CONFIG>
void 
CPU<CONFIG>::InvalidateCache(bool insn_cache, bool data_cache)
{
  if ( insn_cache )
    {
      icache.Invalidate();
    }
  if ( data_cache )
    {
      dcache.Invalidate();
    }
}

/** Invalidate the TLBs.
 * Perform a complete invalidation of the instruction TLB and/or the 
 *   data TLB.
 *
 * @param insn_tlb whether or not the instruction tlb should be invalidated
 * @param data_tlb whether or not the data tlb should be invalidated
 */
template <class CONFIG>
void 
CPU<CONFIG>::InvalidateTLB()
{
  // only the tlb needs to be invalidated, do not touch the lockdown tlb
  // tlb.Invalidate();
}

/** Test and clean DCache.
 * Perform a test and clean operation of the DCache.
 *
 * @return return true if the complete cache is clean, false otherwise
 */
template <class CONFIG>
bool 
CPU<CONFIG>::TestAndCleanDCache()
{
  bool cleaned = true;
  uint32_t num_sets = dcache.GetNumSets();
  uint32_t num_ways = dcache.GetNumWays();
  uint32_t set_index = 0;
  uint32_t way_index = 0;

  uint32_t dirty_set = 0;
  uint32_t dirty_way = 0;

  for ( set_index = 0; 
        cleaned && (set_index < num_sets);
        set_index++ )
    {
      for ( way_index = 0;
            cleaned && (way_index < num_ways); 
            way_index++ )
        {
          if ( dcache.GetValid(set_index, way_index) != 0 )
			{
              if ( dcache.GetDirty(set_index, way_index) != 0 )
                {
                  cleaned = false;
                  dirty_set = set_index;
                  dirty_way = way_index;
                }
			}
        }
    }

  if ( !cleaned )
    {
      /* Get the address and data of the dirty line */
      uint32_t mva = dcache.GetBaseAddress(dirty_set, dirty_way);
      uint32_t pa = mva;
      // if ( likely(cp15.IsMMUEnabled()) )
      //   TranslateMVA(mva, pa);
      uint8_t *data = 0;
      dcache.GetData(dirty_set, dirty_way, &data);

      /* Write the data into memory */
      PrWrite(pa, data, dcache.LINE_SIZE);
      /* Set the line as clean */
      dcache.SetDirty(dirty_set, dirty_way, 0);

      /* Check if there are still dirty lines */
      cleaned = true;
      for ( set_index = dirty_set;
            cleaned && (set_index < num_sets);
            set_index++ )
        {
          for ( way_index = 0; // we might be doing some useless work
                cleaned && (way_index < num_ways);
                way_index++ )
			{
              if ( (dcache.GetValid(set_index, way_index) != 0)
                   && (dcache.GetDirty(set_index, way_index) != 0) )
                {
                  cleaned = false;
                }
			}
        }
    }

  return cleaned;
}

/** Test, clean and invalidate DCache.
 * Perform a test and clean operation of the DCache, and invalidate the
 *   complete cache if it is clean.
 *
 * @return return true if the complete cache is clean, false otherwise
 */
template <class CONFIG>
bool 
CPU<CONFIG>::TestCleanAndInvalidateDCache()
{
  bool cleaned = TestAndCleanDCache();

  if ( cleaned ) dcache.Invalidate();

  return cleaned;
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
    case CP15ENCODE( 0, 0, 0, 1 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "CTR, Cache Type Register"; }
          void Write( CPU& cpu, uint32_t value ) { throw 0; }
          uint32_t Read( CPU& cpu ) {
            bool unified = false;
            uint32_t isize = 0, iassoc = 0, ilen = 0, dsize = 0, dassoc = 0, dlen = 0;
            cpu.GetCacheInfo( unified, isize, iassoc, ilen, dsize, dassoc, dlen );
            uint32_t result = 0x1c000000UL;
            if (not unified) result |= 0x01000000UL;
            else            { dsize = isize; dassoc = iassoc; dlen = ilen; }
            
            switch ( isize / 1024 )
              {
              case 4:        result |= (0x03UL << 6); break;
              case 8:        result |= (0x04UL << 6); break;
              case 16:       result |= (0x05UL << 6); break;
              case 32:       result |= (0x06UL << 6); break;
              case 64:       result |= (0x07UL << 6); break;
              case 128:      result |= (0x08UL << 6); break;
              default: throw 0;
              }
            switch ( dsize / 1024 )
              {
              case 4:        result |= ((0x03UL << 6) << 12); break;
              case 8:        result |= ((0x04UL << 6) << 12); break;
              case 16:       result |= ((0x05UL << 6) << 12); break;
              case 32:       result |= ((0x06UL << 6) << 12); break;
              case 64:       result |= ((0x07UL << 6) << 12); break;
              case 128:      result |= ((0x08UL << 6) << 12); break;
              default: throw 0;
              }
            switch ( iassoc )
              {
              case 4:        result |= (0x02UL << 3); break;
              default: throw 0;
              }
            switch ( dassoc )
              {
              case 4:        result |= ((0x02UL << 3) << 12); break;
              default: throw 0;
              }
            switch ( ilen )
              {
              case 32:       result |= 0x02UL; break;
              default: throw 0;
              }
            switch ( dlen )
              {
              case 32:       result |= (0x02UL << 12); break;
              default: throw 0;
              }
            // No need to set the M bit which should be always 0
            //   meaning that the caches are present
            return result;
          }
        } x;
        return x;
      } break;
      
    case CP15ENCODE( 1, 0, 0, 0 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "SCTLR, System Control Register"; }
          /* TODO: handle SBO(DGP=0x00050078UL) and SBZ(DGP=0xfffa0c00UL)... */
          void Write( CPU& cpu, uint32_t value ) {
            // cpu.sctlr = value;
            if (SCTLR::C.Get( value ))
              cpu.logger << DebugInfo << "Dcache Enabled !!!!!!!!" << EndDebugInfo;
          }
          uint32_t Read( CPU& cpu ) { return cpu.sctlr; }
        } x;
        return x;
      } break;
      
    case CP15ENCODE( 2, 0, 0, 0 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "TTBR0, Translation Table Base Register 0"; }
          /* TODO: handle SBZ(DGP=0x00003fffUL)... */
          void Write( CPU& cpu, uint32_t value ) { throw 0; /* cpu.ttbr0 = value; */ }
          uint32_t Read( CPU& cpu ) { throw 0; return 0 /* cpu.ttbr0 */; }
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
      
    case CP15ENCODE( 7, 0, 5, 0 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "ICIALLU, Invalidate all instruction caches to PoU"; }
          uint32_t Read( CPU& cpu ) { throw 0; return 0; }
          void Write( CPU& cpu, uint32_t value ) { cpu.InvalidateCache(true, false); }
        } x;
        return x;
      } break;
      
    case CP15ENCODE( 7, 0, 5, 1 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "ICIMVAU, Invalidate instruction caches by MVA to PoU"; }
          uint32_t Read( CPU& cpu ) { throw 0; return 0; }
          void Write( CPU& cpu, uint32_t value ) { cpu.InvalidateICacheSingleEntryWithMVA(value); }
        } x;
        return x;
      } break;

    case CP15ENCODE( 7, 0, 10, 1 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "DCCMVAC, Clean data cache line by MVA to PoC"; }
          uint32_t Read( CPU& cpu ) { throw 0; return 0; }
          void Write( CPU& cpu, uint32_t value ) { cpu.CleanDCacheSingleEntryWithMVA(value, false); }
        } x;
        return x;
      } break;

    case CP15ENCODE( 7, 0, 10, 4 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "CP15DSB, Data Synchronization Barrier operation"; }
          uint32_t Read( CPU& cpu ) { throw 0; return 0; }
          void Write( CPU& cpu, uint32_t value ) { cpu.DrainWriteBuffer(); }
        } x;
        return x;
      } break;
      
    case CP15ENCODE( 7, 0, 14, 1 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "DCCIMVAC, Clean and invalidate data cache line by MVA to PoC"; }
          uint32_t Read( CPU& cpu ) { throw 0; return 0; }
          void Write( CPU& cpu, uint32_t value ) { cpu.CleanDCacheSingleEntryWithMVA(value, true); }
        } x;
        return x;
      } break;
      
    case CP15ENCODE( 8, 0, 5, 0 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "ITLBIALL, invalidate instruction TLB"; }
          uint32_t Read( CPU& cpu ) { throw 0; return 0; }
          void Write( CPU& cpu, uint32_t value ) { cpu.InvalidateTLB(); }
        } x;
        return x;
      } break;
      
    case CP15ENCODE( 8, 0, 6, 0 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "DTLBIALL, invalidate data TLB"; }
          uint32_t Read( CPU& cpu ) { throw 0; return 0; }
          void Write( CPU& cpu, uint32_t value ) { cpu.InvalidateTLB(); }
        } x;
        return x;
      } break;
      
    case CP15ENCODE( 8, 0, 7, 0 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "TLBIALL, invalidate unified TLB"; }
          uint32_t Read( CPU& cpu ) { throw 0; return 0; }
          void Write( CPU& cpu, uint32_t value ) { cpu.InvalidateTLB(); }
        } x;
        return x;
      } break;
      
    case CP15ENCODE( 13, 0, 0, 3 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "TPIDRURO, Thread Id Privileged Read Write Only Register"; }
          void Write( CPU& cpu, uint32_t value ) { throw 0; }
          uint32_t Read( CPU& cpu ) {
            /* TODO: the following only works in linux os
             * emulation. We should really access the TPIDRURO
             * register. */
            return cpu.MemRead32( 0xffff0ff0 );
          }
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
  instruction_counter_trap_reporting_import->ReportTrap();
}


    
} // end of namespace arm
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_CPU_TCC__
