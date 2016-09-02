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
 * Authors: Yves Lhuillier (yves.lhuillier@cea.fr)
 */

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_INTEL_ARCH_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_INTEL_ARCH_HH__

#include <unisim/component/cxx/processor/intel/memory.hh>
#include <unisim/component/cxx/processor/intel/segments.hh>
#include <unisim/component/cxx/processor/intel/modrm.hh>
#include <unisim/component/cxx/processor/intel/vectorbank.hh>
#include <unisim/service/interfaces/linux_os.hh>
#include <unisim/kernel/logger/logger.hh>
#include <unisim/util/endian/endian.hh>
#include <unisim/util/inlining/inlining.hh>
#include <unisim/service/interfaces/registers.hh>
#include <unisim/service/interfaces/register.hh>
#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <iosfwd>
#include <cmath>
#include <cassert>
#include <inttypes.h>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace intel {
  
  template <class ARCH> struct Operation;

  struct Arch
  {
    typedef uint8_t  u8_t;
    typedef uint16_t u16_t;
    typedef uint32_t u32_t;
    typedef uint64_t u64_t;
    typedef int8_t   s8_t;
    typedef int16_t  s16_t;
    typedef int32_t  s32_t;
    typedef int64_t  s64_t;
    typedef bool     bit_t;
    typedef float    f32_t;
    typedef double   f64_t;
    typedef bool     mkbool;
    typedef int      mkint;
    
    struct OpHeader
    {
       OpHeader( uint32_t _address ) : address( _address ) {} uint32_t address;
    };
    
    // CONSTRUCTORS/DESTRUCTORS
    Arch();
  
    // PROCESSOR META STATE
    bool                        m_running;      ///< Processors on/off
    int64_t                     m_instcount;    ///< Instruction count from the start
    bool                        m_disasm;       ///< Instruction disassembly on/off
    Operation<Arch>*            m_latest_insn;  ///< latest (current) instruction
    
    void                        stop() { m_running = false; }

    Operation<Arch>*            fetch();
    
    virtual unisim::service::interfaces::LinuxOS* GetLinuxOS() { return 0; };
    
    void                        interrupt( uint8_t _exc )
    {
      switch (_exc) {
      case 0x80: {
        unisim::service::interfaces::LinuxOS* linux_os = GetLinuxOS();
        if (not linux_os) throw 0;
        linux_os->ExecuteSystemCall( int( m_regs[0] ) );
      } break;
      default:
        std::cerr << "Unhandled interruption (0x" << std::hex << uint32_t( _exc ) << ").\n";
        exit( 0 );
      }
    }
    
    // PROCESSOR STATE
  protected:
    uint32_t                    m_EIP;
    
  public:
    u32_t                       geteip() { return m_EIP; }
    void                        seteip( u32_t _eip ) { m_EIP = _eip; }
    void                        addeip( u32_t offset ) { m_EIP += offset; }

    // INTEGER STATE
    // EFLAGS
    // bool m_ID, m_VIP, m_VIF, m_AC, m_VM, m_RF, m_NT, m_IOPL, m_IF, m_TF;
    
    enum flag_t { 
      CF = 0, PF,     AF,     ZF,     SF,     DF,     OF,
      C0,     C1,     C2,     C3,
      flag_upperbound
    };
    
  protected:
    bool                        m_flags[flag_upperbound];
    
  public:
    bit_t                       flagread( flag_t flag )
    { return m_flags[flag]; }
    void                        flagwrite( flag_t flag, bit_t fval )
    { m_flags[flag] = fval; }
    
    // Registers
  protected:
    uint32_t                    m_regs[8]; ///< extended reg
    
  public:
    u8_t                        regread8( uint32_t _idx )
    {
      uint32_t idx=_idx%4, sh=_idx*2 & 8;
      return u8_t( m_regs[idx] >> sh );
    }
    u16_t                       regread16( uint32_t _idx )
    {
      return u16_t( m_regs[_idx] );
    }
    u32_t                       regread32( uint32_t _idx )
    {
      return u32_t( m_regs[_idx] );
    }
    
    template <unsigned OPSIZE>
    typename TypeFor<OPSIZE>::u
    regread( unsigned idx )
    {
      if (OPSIZE==8) return regread8( idx );
      if (OPSIZE==16) return regread16( idx );
      if (OPSIZE==32) return regread32( idx );
      throw 0;
      return 0;
    }
    
    template <unsigned OPSIZE>
    void
    regwrite( unsigned idx, typename TypeFor<OPSIZE>::u value )
    {
      if (OPSIZE==8) return regwrite8( idx, value );
      if (OPSIZE==16) return regwrite16( idx, value );
      if (OPSIZE==32) return regwrite32( idx, value );
      throw 0;
    }
    
    void                        regwrite8( uint32_t _idx, u8_t _val )
    {
      uint32_t idx=_idx%4, sh=_idx*2 & 8;
      m_regs[idx] = (m_regs[idx] & ~(0xff << sh)) | ((_val & 0xff) << sh);
    }
    void                        regwrite16( uint32_t _idx, u16_t _val )
    {
      m_regs[_idx] = (m_regs[_idx] & 0xffff0000) | (_val & 0x0000ffff);
    }
    void                        regwrite32( uint32_t _idx, u32_t _val )
    {
      m_regs[_idx] = _val;
    }
    
    // low level register access routines
    uint32_t                    lla_regread32( uint32_t idx ) const { return m_regs[idx]; }
    void                        lla_regwrite32( uint32_t idx, uint32_t value ) { m_regs[idx] = value; }
    
    // MEMORY STATE
    // Segment Registers
    intel::SegmentReg            m_srs[8];
    
    uint32_t m_gdt_bases[4]; // fake GDT registers used under Linux OS emulation
    
    void                        segregwrite( uint32_t idx, uint16_t _value )
    {
      uint32_t id = (_value >> 3) & 0x1fff; // entry number
      uint32_t ti = (_value >> 2) & 0x0001; // global or local
      uint32_t pl = (_value >> 0) & 0x0003; // requested privilege level
      
      if (not GetLinuxOS()) throw 0;
      if ((idx >= 6) or (id == 0) or (id >= 4) or (ti != 0) or (pl != 3)) throw 0;
      
      m_srs[idx].update( id, ti, pl, m_gdt_bases[id] );
    }
    
    uint16_t                    segregread( unsigned num ) { throw 0; }
    
    // Memory content
  protected:
    struct ClearMemSet {
      void operator() ( uint8_t* base, uintptr_t size ) const {
        for (uintptr_t idx = 0; idx < size; ++idx) base[idx] = 0;
      }
    };
    typedef typename intel::Memory<12,12,ClearMemSet> Memory;
    Memory                      m_mem;
    // static uint32_t const       s_bits = 12;
    // Page*                       m_pages[1<<s_bits];
    
  public:
    // Memory access functions
    uint32_t
    mem_upperbound() const
    {
      uint32_t upperbound = 0;
      for (uint32_t idx = 0; idx < (1<<Memory::s_bits); ++idx) {
        for (typename Memory::Page* page = m_mem.m_pages[idx]; page; page = page->m_next) {
          if (page->m_address <= upperbound) continue;
          upperbound = page->m_address;
        }
      }
      uint32_t const pagesize = (1 << Memory::Page::s_bits);
      return (upperbound + pagesize) & -pagesize;
    }
    
  public:
    u8_t                        memread8( unsigned int _seg, u32_t const& _addr )
    { return memread<8>( _seg, _addr ); }
    u16_t                       memread16( unsigned int _seg, u32_t const& _addr )
    { return memread<16>( _seg, _addr ); }
    u32_t                       memread32( unsigned int _seg, u32_t const& _addr )
    { return memread<32>( _seg, _addr ); }
    u64_t                       memread64( unsigned int _seg, u32_t const& _addr )
    { return memread<64>( _seg, _addr ); }
    
    f32_t                       fmemread32( unsigned int _seg, u32_t _addr )
    {
      union IEEE754_t { float as_f; uint32_t as_u; } word;
      word.as_u = memread<32>( _seg, _addr );
      return word.as_f;
    }
    f64_t                       fmemread64( unsigned int _seg, u32_t _addr )
    {
      union IEEE754_t { double as_f; uint64_t as_u; } word;
      word.as_u = memread<64>( _seg, _addr );
      return word.as_f;
    }
    f64_t
    fmemread80( unsigned int _seg, u32_t _addr )
    {
      _addr += u32_t( m_srs[_seg].m_base );
      uintptr_t const buf_size = 10;
      uint8_t buf[buf_size];
      {
        uint32_t last_addr = _addr, addr = _addr;
        typename Memory::Page* page = m_mem.getpage( addr );
      
        for (uintptr_t idx = 0; idx < buf_size; ++idx, ++ addr)
          {
            if ((last_addr ^ addr) >> Memory::Page::s_bits)
              page = m_mem.getpage( addr );
            uint32_t offset = addr % (1 << Memory::Page::s_bits);
            buf[idx] = page->m_storage[offset];
          }
      }
      uint8_t sign = (buf[9] >> 7) & 1;
      int32_t exponent = ((uint16_t(buf[9]) << 8) & 0x7f00) | (uint16_t(buf[8]) & 0x00ff);
      uint64_t mantissa = 0;
      for (uintptr_t idx = 0; idx < 8; ++idx) { mantissa |= uint64_t( buf[idx] ) << (idx*8); }
      union IEEE754_t { double as_f; uint64_t as_u; } word;
    
      if (exponent != 0x7fff) {
        if (exponent == 0) {
          if (mantissa == 0) return sign ? -0.0 : 0.0;
          exponent += 1;
        }
        exponent -= 16383;
        /* normalizing */
        int32_t eo = __builtin_clzll( mantissa );
        mantissa <<= eo;
        exponent -= eo;
        if (exponent < 1024) {
          if (exponent < -1022) {
            /* denormalizing */
            eo = (-1022 - exponent);
            mantissa >>= eo;
            exponent = -1023;
          }
          word.as_u = ((mantissa << 1) >> 12);
          word.as_u |= uint64_t(exponent + 1023) << 52;
          word.as_u |= uint64_t(sign) << 63;
        } else /* exponent >= 1024 */ {
          /* huge number, convert to infinity */
          word.as_u = (uint64_t(0x7ff) << 52) | (uint64_t(sign) << 63);
        }
      }
    
      else /* (exponent == 0x7fff) */ {
        if (mantissa >> 63) {
          /* IEEE 754 compatible */
          word.as_u = ((mantissa << 1) >> 12);
          word.as_u |= uint64_t(0x7ff) << 52;
          word.as_u |= uint64_t(sign) << 63;
        } else {
          /* invalid operand ==> convert to quiet NaN (keep sign) */
          word.as_u = (uint64_t(0xfff) << 51) | (uint64_t(sign) << 63);
        }
      }
    
      return word.as_f;
    }
    
    template <unsigned OPSIZE>
    typename TypeFor<OPSIZE>::f
    frmread( RMOp<Arch> const& rmop )
    {
      typedef typename TypeFor<OPSIZE>::f f_type;
      if (not rmop.is_memory_operand()) return f_type( fread( rmop.ereg() ) );
      
      if (OPSIZE==32) return fmemread32( rmop->segment, rmop->effective_address( *this ) );
      if (OPSIZE==64) return fmemread64( rmop->segment, rmop->effective_address( *this ) );
      if (OPSIZE==80) return fmemread80( rmop->segment, rmop->effective_address( *this ) );
      throw 0;
      return f_type();
    }

    void                        memwrite8( unsigned int _seg, u32_t _addr, u8_t _val )
    { memwrite<8>( _seg, _addr, _val ); }
    void                        memwrite16( unsigned int _seg, u32_t _addr, u16_t _val )
    { memwrite<16>( _seg, _addr, _val ); }
    void                        memwrite32( unsigned int _seg, u32_t _addr, u32_t _val )
    { memwrite<32>( _seg, _addr, _val ); }
    void                        memwrite64( unsigned int _seg, u32_t _addr, u64_t _val )
    { memwrite<64>( _seg, _addr, _val ); }
    void                        fmemwrite32( unsigned int _seg, u32_t _addr, f32_t _val )
    {
      union IEEE754_t { float as_f; uint32_t as_u; } word;
      word.as_f = _val;
      memwrite<32>( _seg, _addr, word.as_u );
    }
    void                        fmemwrite64( unsigned int _seg, u32_t _addr, f64_t _val )
    {
      union IEEE754_t { double as_f; uint64_t as_u; } word;
      word.as_f = _val;
      memwrite<64>( _seg, _addr, word.as_u );
    }
    void                        fmemwrite80( unsigned int _seg, u32_t _addr, f64_t _val )
    {
      _addr += u32_t( m_srs[_seg].m_base );
      union IEEE754_t { double as_f; uint64_t as_u; } word;
      word.as_f = _val;
      uint8_t sign = (word.as_u >> 63) & 1;
      int32_t exponent = (word.as_u >> 52) & 0x7ff;
      uint64_t mantissa = (word.as_u << 12) >> 1;
      if (exponent != 0x7ff) {
        if (exponent != 0) {
          mantissa |= uint64_t( 1 ) << 63;
        } else if (mantissa != 0) {
          /* normalizing */
          int32_t eo = __builtin_clzll( mantissa );
          exponent = 1 - eo;
          mantissa <<= eo;
        } else {
          exponent = 1023 - 16383;
        }
        exponent = exponent - 1023 + 16383;
        assert( (exponent >= 0) and (exponent < 0x8000) );
      } else {
        mantissa |= uint64_t( 1 ) << 63;
        exponent = 0x7fff;
      }

      uintptr_t const buf_size = 10;
      uint8_t buf[buf_size];
    
      buf[9] = (sign << 7) | (uint8_t( exponent >> 8 ) & 0x7f);
      buf[8] = uint8_t( exponent ) & 0xff;
      for ( uintptr_t idx = 0; idx < 8; ++idx)
        buf[idx] = uint8_t( mantissa >> (idx*8) );
    
      uint32_t last_addr = _addr, addr = _addr;
      typename Memory::Page* page = m_mem.getpage( addr );
      
      for (uintptr_t idx = 0; idx < buf_size; ++idx, ++addr)
        {
          uint8_t byte = buf[idx];
          if ((last_addr ^ addr) >> Memory::Page::s_bits)
            page = m_mem.getpage( addr );
          uint32_t offset = addr % (1 << Memory::Page::s_bits);
          page->m_storage[offset] = byte;
        }
    }

    template <unsigned OPSIZE>
    void
    frmwrite( RMOp<Arch> const& rmop, typename TypeFor<OPSIZE>::f value )
    {
      if (not rmop.is_memory_operand()) return fwrite( rmop.ereg(), f64_t( value ) );
      
      if (OPSIZE==32) return fmemwrite32( rmop->segment, rmop->effective_address( *this ), f64_t( value ) );
      if (OPSIZE==64) return fmemwrite64( rmop->segment, rmop->effective_address( *this ), f64_t( value ) );
      if (OPSIZE==80) return fmemwrite80( rmop->segment, rmop->effective_address( *this ), f64_t( value ) );
      throw 0;
    }

    
    template <unsigned OPSIZE>
    void
    memwrite( unsigned _seg, u32_t _addr, typename TypeFor<OPSIZE>::u _val )
    {
      uintptr_t const int_size = (OPSIZE/8);
      uint32_t addr = _addr + m_srs[_seg].m_base, last_addr = addr;
      
      typename Memory::Page* page = m_mem.getpage( addr );
      
      for (uintptr_t idx = 0; idx < int_size; ++idx, ++addr)
        {
          uint8_t byte = (_val >> (idx*8)) & 0xff;
          if ((last_addr ^ addr) >> Memory::Page::s_bits)
            page = m_mem.getpage( addr );
          uint32_t offset = addr % (1 << Memory::Page::s_bits);
          page->m_storage[offset] = byte;
        }
    }
    
    template <unsigned OPSIZE>
    typename TypeFor<OPSIZE>::u
    pop()
    {
      // TODO: handle stack address size
      u32_t sptr = regread32( 4 );
      regwrite32( 4, sptr + u32_t( OPSIZE/8 ) );
      return memread<OPSIZE>( SS, sptr );
    }
    
    template <unsigned OPSIZE>
    void
    push( typename TypeFor<OPSIZE>::u value )
    {
      // TODO: handle stack address size
      u32_t sptr = regread32( 4 ) - u32_t( OPSIZE/8 );
      memwrite<OPSIZE>( SS, sptr, value );
      regwrite32( 4, sptr );
    }
    
    
    template <unsigned OPSIZE>
    typename TypeFor<OPSIZE>::u
    rmread( RMOp<Arch> const& rmop )
    {
      if (not rmop.is_memory_operand())
        return regread<OPSIZE>( rmop.ereg() );
      
      return memread<OPSIZE>( rmop->segment, rmop->effective_address( *this ) );
    }
    
    template <unsigned OPSIZE>
    void
    rmwrite( RMOp<Arch> const& rmop, typename TypeFor<OPSIZE>::u value )
    {
      if (not rmop.is_memory_operand())
        return regwrite<OPSIZE>( rmop.ereg(), value );
      
      return memwrite<OPSIZE>( rmop->segment, rmop->effective_address( *this ), value );
    }
    
    // void
    // Arch::fnanchk( f64_t value )
    // {
    //   if (not m_fnanchk) return;
    //   switch (__fpclassify( value )) {
    //   case FP_NAN:      std::cerr << "[FPNAN]"; break;
    //   case FP_INFINITE: std::cerr << "[FPINF]"; break;
    //   default:          return;
    //   }
    //   this->fpdump();
    //   throw FPException();
    // }
  
    template<unsigned OPSIZE>
    typename TypeFor<OPSIZE>::u
    memread( unsigned _seg, u32_t _addr )
    {
      uintptr_t const int_size = (OPSIZE/8);
      uint32_t addr = _addr + m_srs[_seg].m_base, last_addr = addr;
      typename Memory::Page* page = m_mem.getpage( addr );
      
      typedef typename TypeFor<OPSIZE>::u u_type;
      u_type result = 0;
      
      for (uintptr_t idx = 0; idx < int_size; ++idx, ++ addr)
        {
          if ((last_addr ^ addr) >> Memory::Page::s_bits)
            page = m_mem.getpage( addr );
          uint32_t offset = addr % (1 << Memory::Page::s_bits);
          result |= (u_type( page->m_storage[offset] ) << (idx*8));
        }
      
      return result;
    }
    
    // Low level memory access routines
    enum LLAException_t { LLAError };
    void                        lla_memcpy( uint32_t addr, uint8_t const* buf, uint32_t size )
    { m_mem.write( addr, buf, size ); }
    void                        lla_memcpy( uint8_t* buf, uint32_t addr, uint32_t size )
    { m_mem.read( buf, addr, size ); }
    void                        lla_bzero( uint32_t addr, uint32_t size )
    { m_mem.clear( addr, size ); }
    template<class INT_t>
    void                        lla_memwrite( uint32_t _addr, INT_t _val )
    {
      uintptr_t const int_size = sizeof( INT_t );
      
      uint32_t last_addr = _addr, addr = _addr;
      typename Memory::Page* page = m_mem.getpage( addr );
      
      for (uintptr_t idx = 0; idx < int_size; ++idx, ++addr)
        {
          uint8_t byte = (_val >> (idx*8)) & 0xff;
          if ((last_addr ^ addr) >> Memory::Page::s_bits)
            page = m_mem.getpage( addr );
          uint32_t offset = addr % (1 << Memory::Page::s_bits);
          page->m_storage[offset] = byte;
        }
    }
    
    template<class INT_t>
    INT_t                       lla_memread( uint32_t _addr )
    {
      uintptr_t const int_size = sizeof( INT_t );
      
      uint32_t last_addr = _addr, addr = _addr;
      typename Memory::Page* page = m_mem.getpage( addr );
      
      INT_t result = 0;
      
      for (uintptr_t idx = 0; idx < int_size; ++idx, ++ addr)
        {
          if ((last_addr ^ addr) >> Memory::Page::s_bits)
            page = m_mem.getpage( addr );
          uint32_t offset = addr % (1 << Memory::Page::s_bits);
          result |= (INT_t( page->m_storage[offset] ) << (idx*8));
        }
      
      return result;

    }
    
    uint8_t                     lla_memread8( uint32_t _addr ) { return lla_memread<uint8_t>( _addr ); }
    uint16_t                    lla_memread16( uint32_t _addr ) { return lla_memread<uint16_t>( _addr ); }
    uint32_t                    lla_memread32( uint32_t _addr ) { return lla_memread<uint32_t>( _addr ); }
    void                        lla_memwrite8( uint32_t _addr, uint8_t _val ) { lla_memwrite<uint8_t>( _addr, _val ); }
    void                        lla_memwrite16( uint32_t _addr, uint16_t _val ) { lla_memwrite<uint16_t>( _addr, _val ); }
    void                        lla_memwrite32( uint32_t _addr, uint32_t _val ) { lla_memwrite<uint32_t>( _addr, _val ); }

    // FLOATING POINT STATE
  protected:
    double                      m_fregs[8];
    uint32_t                    m_ftop;
    uint16_t                    m_fcw;
    
  public:
    void                        fnanchk( f64_t value ) {};
    uint32_t                    ftopread() { return m_ftop; }
    int                         fcwreadRC() const { return int( (m_fcw >> 10) & 3 ); }
    void                        fpush( f64_t value )
    { fnanchk( value ); m_ftop = ((m_ftop + 0x7) & 0x7); m_fregs[m_ftop] = value; /*m_dirtfregs |= (1 << m_ftop);*/ }
    void                        fwrite( uint32_t idx, f64_t value )
    { fnanchk( value ); uint32_t fidx = (m_ftop + idx) & 0x7; m_fregs[fidx] = value; /*m_dirtfregs |= (1 << fidx);*/ }
    f64_t                       fpop()
    { f64_t value = m_fregs[m_ftop++]; m_ftop &= 0x7; fnanchk( value ); return value; }
    f64_t                       fread( uint32_t idx )
    { uint32_t fidx = (m_ftop + idx) & 0x7; f64_t value = m_fregs[fidx]; fnanchk( value ); return value; }
    
    void
    fxam()
    {
      double val = this->fread( 0 );
      
      flagwrite( C1, __signbit( val ) );
      
      switch (__fpclassify( val )) {
      case FP_NAN:
        flagwrite( C3, 0 );
        flagwrite( C2, 0 );
        flagwrite( C0, 1 );
        break;
      case FP_NORMAL:
        flagwrite( C3, 0 );
        flagwrite( C2, 1 );
        flagwrite( C0, 0 );
        break;
      case FP_INFINITE:
        flagwrite( C3, 0 );
        flagwrite( C2, 1 );
        flagwrite( C0, 1 );
        break;
      case FP_ZERO:
        flagwrite( C3, 1 );
        flagwrite( C2, 0 );
        flagwrite( C0, 0 );
        break;
      case FP_SUBNORMAL:
        flagwrite( C3, 1 );
        flagwrite( C2, 1 );
        flagwrite( C0, 0 );
        break;
      }
    }
    
    u16_t
    fcwread()
    {
      return u16_t( m_fcw );
      // return u16_t( (1 << 0/*IM*/) |
      //               (1 << 1/*DM*/) |
      //               (1 << 2/*ZM*/) |
      //               (1 << 3/*OM*/) |
      //               (1 << 4/*UM*/) |
      //               (1 << 5/*PM*/) |
      //               (2 << 8/*PC*/) |
      //               (0 << 10/*RC*/) |
      //               (0 << 12/*X*/) );
    }
    
    void
    fcwwrite( u16_t _value )
    {
      m_fcw = _value;
      struct field { uint16_t offset, mask, expected, err; char const* name; };
      static field fields[] =
        {
          { 0, 1, 1, 0, "IM"},
          { 1, 1, 1, 0, "DM"},
          { 2, 1, 1, 0, "ZM"},
          { 3, 1, 1, 0, "OM"},
          { 4, 1, 1, 0, "UM"},
          { 5, 1, 1, 0, "PM"},
          { 8, 3, 2, 0, "PC"},
          {10, 3, 0, 0, "RC"},
          {12, 1, 0, 0, "X"},
        };
      for (uintptr_t idx = 0; idx < (sizeof (fields) / sizeof (field)); ++idx)
        {
          uint16_t field_val = ((_value >> fields[idx].offset) & fields[idx].mask);
          if (field_val == fields[idx].expected)
            continue; /* value is expected one*/
          if ((fields[idx].err >> field_val) & 1)
            continue; /* error already reported */
          fields[idx].err |= (1 << field_val);
          std::cerr << "Warning: unimplemented FPUControlWord." << fields[idx].name
                    << " value: " << field_val << ".\n";
        }
    }
    
    // void                        fpdump()
    // {
    //   std::cerr << ": #insn: " << std::dec << m_instcount
    //             << ", @insn: " << std::hex << m_latest_insn->GetAddr()
    //             << ", insn: ";
    //   m_latest_insn->disasm( std::cerr );
    //   std::cerr << " (" << m_latest_insn->GetEncoding() << ")" << std::endl;
    //   for (int reg = 0; reg < 8; ++reg) {
    //     dtlib::osprintf( std::cerr, "%s: %010x\n", intel::regname( 4, reg ), this->regread32( reg ) );
    //   }
    //   for (int reg = 0; reg < 8; ++reg) {
    //     dtlib::osprintf( std::cerr, "st(%d): %f\n", reg, this->fread( reg ) );
    //   }
    // }
    
    // Debug methods
  protected:
    uint32_t                    m_dirtfregs;
    
  public:
    uint32_t                    pop_dirtfregs() { uint32_t r = m_dirtfregs; m_dirtfregs = 0; return r; }
    
    
  public:
    struct
    {
      VectorBankCache<u64_t,16,16> u64regs;
      VectorBankCache<f32_t,16,16> f32regs;
      VectorBankCache<f64_t,16,16> f64regs;
      
      template <typename CMD>
      void DoAll( CMD& cmd )
      {
        u64regs.Do( cmd );
        f32regs.Do( cmd );
        f64regs.Do( cmd );
      }
    } vect;
    
    template<unsigned OPSIZE>
    typename TypeFor<OPSIZE>::u
    xmm_uread( unsigned reg, unsigned sub )
    {
      unsigned regbit = OPSIZE*sub, blocsub = regbit/64, blocbit = regbit%64;
      u64_t bloc = vect.u64regs.GetReg(vect, reg, blocsub);
      return typename TypeFor<OPSIZE>::u(bloc >> blocbit);
    }
    
    template<unsigned OPSIZE>
    void
    xmm_uwrite( unsigned reg, unsigned sub, typename TypeFor<OPSIZE>::u val )
    {
      typedef typename TypeFor<OPSIZE>::u utype;
      unsigned regbit = OPSIZE*sub, blocsub = regbit/64, blocbit = regbit%64;
      u64_t bloc = vect.u64regs.GetReg(vect, reg, blocsub);
      bloc = (bloc & ~(u64_t(~utype(0)) << blocbit)) | (u64_t( val ) << blocbit);
      vect.u64regs.SetReg(vect, reg, blocsub, bloc);
    }
    
    template<unsigned OPSIZE>
    typename TypeFor<OPSIZE>::u
    xmm_uread( RMOp<Arch> const& rmop, unsigned sub )
    {
      if (not rmop.is_memory_operand()) return xmm_uread<OPSIZE>( rmop.ereg(), sub );
      return memread<OPSIZE>( rmop->segment, rmop->effective_address( *this ) + (sub*OPSIZE/8) );
    }
    
    template<unsigned OPSIZE>
    void
    xmm_uwrite( RMOp<Arch> const& rmop, unsigned sub, typename TypeFor<OPSIZE>::u val )
    {
      if (not rmop.is_memory_operand()) return xmm_uwrite<OPSIZE>( rmop.ereg(), sub, val );
      return memwrite<OPSIZE>( rmop->segment, rmop->effective_address( *this ) + (sub*OPSIZE/8), val );
    }
    

  };
  

  struct FPException : public std::exception {};

  f64_t sine( f64_t );
  f64_t cosine( f64_t );
  f64_t tangent( f64_t );
  f64_t arctan( f64_t angle );
  f64_t fmodulo( f64_t, f64_t );
  f64_t firound( f64_t, int x87frnd_mode );
  f64_t power( f64_t, f64_t );
  f64_t logarithm( f64_t );
  f64_t square_root( f64_t );

} // end of namespace intel
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_INTEL_ARCH_HH__
