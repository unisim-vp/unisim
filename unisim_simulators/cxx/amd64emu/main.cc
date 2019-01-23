/*
 *  Copyright (c) 2018,
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
 * Authors: Yves Lhuillier (yves.lhuillier@cea.fr)
 */

// #include <unisim/component/cxx/processor/intel/arch.hh>
#include <unisim/component/cxx/processor/intel/isa/intel.hh>
#include <unisim/component/cxx/processor/intel/modrm.hh>
#include <unisim/component/cxx/processor/intel/disasm.hh>
#include <unisim/component/cxx/processor/intel/vectorbank.hh>
#include <unisim/component/cxx/processor/intel/types.hh>
#include <unisim/component/cxx/memory/sparse/memory.hh>
#include <unisim/util/arithmetic/arithmetic.hh>
#include <unisim/util/debug/simple_register.hh>
#include <linuxsystem.hh>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <cmath>
#include <cctype>

// TODO: should handle twice of 64-bit types
template <typename T>
  struct Twice
{
  struct Error {};
  Twice() {}
  Twice( T const& val ) {}
  template <typename X>
  Twice operator << (X) { throw Error(); return Twice(); }
  template <typename X>
  Twice operator >> (X) { throw Error(); return Twice(); }
  Twice operator | (Twice const& val) { throw Error(); return Twice(); }

  Twice& operator |= (Twice const& val) { return *this; }
  operator T () { throw Error(); return T(); }
  void unimplented();
};

template <typename T> using VectorTypeInfo = unisim::component::cxx::processor::intel::VectorTypeInfo<T>;
template <typename A, unsigned S> using TypeFor = typename unisim::component::cxx::processor::intel::TypeFor<A,S>;

struct Arch
  : public unisim::service::interfaces::MemoryInjection<uint64_t>
  , public unisim::service::interfaces::Memory<uint64_t>
  , public unisim::service::interfaces::Registers
{
  typedef uint8_t      u8_t;
  typedef uint16_t     u16_t;
  typedef uint32_t     u32_t;
  typedef uint64_t     u64_t;
  typedef Twice<u64_t> u128_t;
  typedef int8_t       s8_t;
  typedef int16_t      s16_t;
  typedef int32_t      s32_t;
  typedef int64_t      s64_t;
  typedef Twice<s64_t> s128_t;
  typedef bool         bit_t;
  typedef uint64_t     addr_t;

  typedef float        f32_t;
  typedef double       f64_t;
  typedef long double  f80_t;

  typedef unisim::component::cxx::processor::intel::GObLH GObLH;
  typedef unisim::component::cxx::processor::intel::GOb GOb;
  typedef unisim::component::cxx::processor::intel::GOw GOw;
  typedef unisim::component::cxx::processor::intel::GOd GOd;
  typedef unisim::component::cxx::processor::intel::GOq GOq;
  
  struct OpHeader
  {
    OpHeader( addr_t _address ) : address( _address ) {} addr_t address;
  };

  Arch()
    : unisim::service::interfaces::MemoryInjection<uint64_t>()
    , unisim::service::interfaces::Memory<uint64_t>()
    , unisim::service::interfaces::Registers()
    , linux_os(0)
    , rip()
    , u64regs()
    , do_disasm(false)
    , instruction_count(0)
  {
    regmap["%rip"] = new unisim::util::debug::SimpleRegister<uint64_t>("%rip", &this->rip);
    
    for (int idx = 0; idx < 16; ++idx)
      {
        std::ostringstream regname;
        regname << unisim::component::cxx::processor::intel::DisasmGq(idx);
        regmap[regname.str()] = new unisim::util::debug::SimpleRegister<uint64_t>(regname.str(), &u64regs[idx]);
      }
    
    //   struct SegmentRegister : public unisim::service::interfaces::Register
    //   {
    //     SegmentRegister( Arch& _arch, std::string _name, unsigned _idx ) : arch(_arch), name(_name), idx(_idx) {}
    //     virtual const char *GetName() const { return "pc"; }
    //     virtual void GetValue( void* buffer ) const { *((uint16_t*)buffer) = arch.segregread( idx ); }
    //     virtual void SetValue( void const* buffer ) { arch.segregwrite( idx, *((uint16_t*)buffer) ); }
    //     virtual int  GetSize() const { return 2; }
    //     virtual void Clear() {} // Clear is meaningless for segment registers
    //     Arch&        arch;
    //     std::string  name;
    //     unsigned     idx;
    //   };

    //   for (int idx = 0; idx < 6; ++idx) {
    //     std::ostringstream regname;
    //     regname << unisim::component::cxx::processor::intel::DisasmS(idx);
    //     regmap[regname.str()] = new SegmentRegister(*this, regname.str(), idx);
    //   }
    
    //   for (int idx = 0; idx < 4; ++idx) {
    //     std::ostringstream regname;
    //     regname << "@gdt[" << idx<< "].base";
    //     regmap[regname.str()] = new unisim::util::debug::SimpleRegister<uint64_t>(regname.str(), &m_gdt_bases[idx]);
    //   }
    
    //   struct X87Register : public unisim::service::interfaces::Register
    //   {
    //     X87Register( std::string _name, Arch& _arch, unsigned _idx ) : name(_name), arch(_arch), idx(_idx) {}
    //     char const* GetName() const { return name.c_str(); }
    //     void GetValue(void *buffer) const { *((f64_t*)buffer) = arch.fread( idx ); }
    //     void SetValue(const void *buffer) { arch.fwrite( idx, *((f64_t*)buffer) ); }
    //     int GetSize() const { return 8; }
    //     std::string name;
    //     Arch& arch;
    //     unsigned idx;
    //   };
    //   for (int idx = 0; idx < 8; ++idx) {
    //     std::ostringstream regname;
    //     regname << unisim::component::cxx::processor::intel::DisasmFPR(idx);
    //     regmap[regname.str()] = new X87Register( regname.str(), *this, idx );
    //   }
  }
  ~Arch()
  {
    for (auto reg : regmap)
      delete reg.second;
  }
  
  void SetLinuxOS( unisim::service::interfaces::LinuxOS* _linux_os ) { linux_os = _linux_os; }
  // unisim::service::interfaces::LinuxOS* GetLinuxOS() { return linux_os; }
  
  unisim::service::interfaces::LinuxOS* linux_os;
  std::map<std::string,unisim::service::interfaces::Register*> regmap;
  
  struct ClearMemSet { void operator() ( uint8_t* base, uintptr_t size ) const { __builtin_bzero(base, size); } };
  typedef typename unisim::component::cxx::memory::sparse::Memory<uint64_t,15,15,ClearMemSet> Memory;
  Memory                      m_mem;

  // unisim::service::interfaces::Memory<uint64_t>
  void Reset() {}
  bool ReadMemory(uint64_t addr, void* buffer, uint32_t size ) { m_mem.read( (uint8_t*)buffer, addr, size ); return true; }
  bool WriteMemory(uint64_t addr, void const* buffer, uint32_t size) { m_mem.write( addr, (uint8_t*)buffer, size ); return true; }

  typedef unisim::component::cxx::processor::intel::RMOp<Arch> RMOp;
  
  // MEMORY STATE
  // Segment Registers
  typedef unisim::component::cxx::processor::intel::SegmentReg SegmentReg;
  SegmentReg            m_srs[8];

  uint32_t m_gdt_bases[4]; // fake GDT registers used under Linux OS emulation

  void                        segregwrite( uint32_t idx, uint16_t _value )
  {
    uint32_t id = (_value >> 3) & 0x1fff; // entry number
    uint32_t ti = (_value >> 2) & 0x0001; // global or local
    uint32_t pl = (_value >> 0) & 0x0003; // requested privilege level

          //if (not GetLinuxOS()) throw 0;
    if ((idx >= 6) or (id == 0) or (id >= 4) or (ti != 0) or (pl != 3)) throw 0;

    m_srs[idx].update( id, ti, pl, m_gdt_bases[id] );
  }

  uint16_t                    segregread( unsigned num ) { throw 0; }

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
  typename TypeFor<Arch,OPSIZE>::f
  frmread( RMOp const& rmop )
  {
    typedef typename TypeFor<Arch,OPSIZE>::f f_type;
    if (not rmop.is_memory_operand()) return f_type( this->fread( rmop.ereg() ) );

    if (OPSIZE==32) return this->fmemread32( rmop->segment, rmop->effective_address( *this ) );
    if (OPSIZE==64) return this->fmemread64( rmop->segment, rmop->effective_address( *this ) );
    if (OPSIZE==80) return this->fmemread80( rmop->segment, rmop->effective_address( *this ) );
    throw 0;
    return f_type();
  }

  void                        fmemwrite32( unsigned int _seg, u64_t _addr, f32_t _val )
  {
    union IEEE754_t { float as_f; uint32_t as_u; } word;
    word.as_f = _val;
    memwrite<32>( _seg, _addr, word.as_u );
  }
  void                        fmemwrite64( unsigned int _seg, u64_t _addr, f64_t _val )
  {
    union IEEE754_t { double as_f; uint64_t as_u; } word;
    word.as_f = _val;
    memwrite<64>( _seg, _addr, word.as_u );
  }
  void                        fmemwrite80( unsigned int _seg, u64_t _addr, f64_t _val )
  {
    _addr += u64_t( m_srs[_seg].m_base );
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
  frmwrite( RMOp const& rmop,
            typename TypeFor<Arch,OPSIZE>::f value )
  {
    if (not rmop.is_memory_operand()) return fwrite( rmop.ereg(), f64_t( value ) );

    if (OPSIZE==32) return fmemwrite32( rmop->segment, rmop->effective_address( *this ), f64_t( value ) );
    if (OPSIZE==64) return fmemwrite64( rmop->segment, rmop->effective_address( *this ), f64_t( value ) );
    if (OPSIZE==80) return fmemwrite80( rmop->segment, rmop->effective_address( *this ), f64_t( value ) );
    throw 0;
  }


  template <unsigned OPSIZE>
      void
  memwrite( unsigned _seg, u64_t _addr, typename TypeFor<Arch,OPSIZE>::u _val )
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
  typename TypeFor<Arch,OPSIZE>::u
  pop()
  {
    // TODO: handle stack address size
    u64_t sptr = regread( GOq(), 4 );
    regwrite( GOq(), 4, sptr + u64_t( OPSIZE/8 ) );
    return memread<OPSIZE>( unisim::component::cxx::processor::intel::SS, sptr );
  }

  void shrink_stack( addr_t offset ) { regwrite( GOq(), 4, regread( GOq(), 4 ) + offset ); }
  void grow_stack( addr_t offset ) { regwrite( GOq(), 4, regread( GOq(), 4 ) - offset ); }

  template <unsigned OPSIZE>
  void
  push( typename TypeFor<Arch,OPSIZE>::u value )
  {
    // TODO: handle stack address size
    u64_t sptr = regread( GOq(), 4 ) - u64_t( OPSIZE/8 );
    memwrite<OPSIZE>( unisim::component::cxx::processor::intel::SS, sptr, value );
    regwrite( GOq(), 4, sptr );
  }


  template <class GOP>
  typename TypeFor<Arch,GOP::OPSIZE>::u
  rmread( GOP const&, RMOp const& rmop )
  {
    if (not rmop.is_memory_operand())
      return regread( GOP(), rmop.ereg() );

    return memread<GOP::OPSIZE>( rmop->segment, rmop->effective_address( *this ) );
  }

  template <class GOP>
  void
  rmwrite( GOP const&, RMOp const& rmop, typename TypeFor<Arch,GOP::OPSIZE>::u value )
  {
    if (not rmop.is_memory_operand())
      return regwrite( GOP(), rmop.ereg(), value );

    return memwrite<GOP::OPSIZE>( rmop->segment, rmop->effective_address( *this ), value );
  }

  template<unsigned OPSIZE>
  typename TypeFor<Arch,OPSIZE>::u
  memread( unsigned _seg, u64_t _addr )
  {
    uintptr_t const int_size = (OPSIZE/8);
    uint32_t addr = _addr + m_srs[_seg].m_base, last_addr = addr;
    typename Memory::Page* page = m_mem.getpage( addr );

    typedef typename TypeFor<Arch,OPSIZE>::u u_type;
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
  
  
  // unisim::service::interfaces::Registers
  unisim::service::interfaces::Register* GetRegister(char const* name)
  {
    auto reg = regmap.find( name );
    return (reg == regmap.end()) ? 0 : reg->second;
  }
  void ScanRegisters(unisim::service::interfaces::RegisterScanner& scanner)
  {
    // Program counter
    scanner.Append( GetRegister( "%rip" ) );
    // General purpose registers
    // scanner.Append( GetRegister( "%eax" ) );
    // scanner.Append( GetRegister( "%ecx" ) );
    // scanner.Append( GetRegister( "%edx" ) );
    // scanner.Append( GetRegister( "%ebx" ) );
    // scanner.Append( GetRegister( "%esp" ) );
    // scanner.Append( GetRegister( "%ebp" ) );
    // scanner.Append( GetRegister( "%esi" ) );
    // scanner.Append( GetRegister( "%edi" ) );
    // Segments
    // scanner.Append( GetRegister( "%es" ) );
    // scanner.Append( GetRegister( "%cs" ) );
    // scanner.Append( GetRegister( "%ss" ) );
    // scanner.Append( GetRegister( "%ds" ) );
    // scanner.Append( GetRegister( "%fs" ) );
    // scanner.Append( GetRegister( "%gs" ) );
    // FP registers
    // scanner.Append( GetRegister( "%st" ) );
    // scanner.Append( GetRegister( "%st(1)" ) );
    // scanner.Append( GetRegister( "%st(2)" ) );
    // scanner.Append( GetRegister( "%st(3)" ) );
    // scanner.Append( GetRegister( "%st(4)" ) );
    // scanner.Append( GetRegister( "%st(5)" ) );
    // scanner.Append( GetRegister( "%st(6)" ) );
    // scanner.Append( GetRegister( "%st(7)" ) );
  }
  // unisim::service::interfaces::MemoryInjection<ADDRESS>

  bool InjectReadMemory(uint64_t addr, void *buffer, uint32_t size) { m_mem.read( (uint8_t*)buffer, addr, size ); return true; }
  bool InjectWriteMemory(uint64_t addr, void const* buffer, uint32_t size) { m_mem.write( addr, (uint8_t*)buffer, size ); return true; }
  // Implementation of ExecuteSystemCall
  
  void ExecuteSystemCall( unsigned id )
  {
    if (not linux_os)
      { throw std::logic_error( "No linux OS emulation connected" ); }
    linux_os->ExecuteSystemCall( id );
  }
  
  uint64_t rip;

  enum ipproc_t { ipjmp = 0, ipcall, ipret };
  u64_t                       getnip() { return rip; }
  void                        setnip( u64_t _rip, ipproc_t ipproc = ipjmp ) { rip = _rip; }
  //void                        addeip( u64_t offset ) { rip += offset; }

  void                        syscall()
  {
    this->ExecuteSystemCall( this->regread( GOq(), 0 ) );
  }
  
  void                        interrupt( uint8_t _exc )
  {
    std::cerr << "Unhandled interruption (0x" << std::hex << uint32_t( _exc ) << ").\n";
    exit( 0 );
  }
  
  uint64_t u64regs[16]; ///< extended reg

  template <class GOP>
  typename TypeFor<Arch,GOP::OPSIZE>::u regread( GOP const&, unsigned idx )
  {
    return typename TypeFor<Arch,GOP::OPSIZE>::u( u64regs[idx] );
  }

  u8_t regread( GObLH const&, unsigned idx )
  {
    unsigned reg = idx%4, sh = idx*2 & 8;
    return u8_t( u64regs[reg] >> sh );
  }

  template <class GOP>
  void regwrite( GOP const&, unsigned idx, typename TypeFor<Arch,GOP::OPSIZE>::u value )
  {
    u64regs[idx] = u64_t( value );
    gdbchecker.mark( idx );
  }

  void regwrite( GObLH const&, unsigned idx, u8_t value )
  {
    uint32_t reg = idx%4, sh = idx*2 & 8;
    u64regs[reg] = (u64regs[reg] & ~u64_t(0xff << sh)) | ((value & u64_t(0xff)) << sh);
    gdbchecker.mark( reg );
  }
  void regwrite( GOb const&, unsigned idx, u8_t value )
  {
    u64regs[idx] = (u64regs[idx] & ~u64_t(0xff)) | ((value & u64_t(0xff)));
    gdbchecker.mark( idx );
  }
  void regwrite( GOw const&, unsigned idx, u16_t value )
  {
    u64regs[idx] = (u64regs[idx] & ~u64_t(0xffff)) | ((value & u64_t(0xffff)));
    gdbchecker.mark( idx );
  }

  struct FLAG
  {
    enum Code {
      CF = 0, PF,     AF,     ZF,     SF,     DF,     OF,
      C0,     C1,     C2,     C3,
      end
    };
  };

protected:
  bool                        m_flags[FLAG::end];

public:
  bit_t                       flagread( FLAG::Code flag )
  { return m_flags[flag]; }
  void                        flagwrite( FLAG::Code flag, bit_t fval )
  { m_flags[flag] = fval; }
  
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
  void                        finit()
  {
    m_ftop = 0;
    m_fcw = 0x37f;
    flagwrite( FLAG::C0, 0 );
    flagwrite( FLAG::C1, 0 );
    flagwrite( FLAG::C2, 0 );
    flagwrite( FLAG::C3, 0 );
  }
    
  void
  fxam()
  {
    double val = this->fread( 0 );
      
    flagwrite( FLAG::C1, __signbit( val ) );
      
    switch (__fpclassify( val )) {
    case FP_NAN:
      flagwrite( FLAG::C3, 0 );
      flagwrite( FLAG::C2, 0 );
      flagwrite( FLAG::C0, 1 );
      break;
    case FP_NORMAL:
      flagwrite( FLAG::C3, 0 );
      flagwrite( FLAG::C2, 1 );
      flagwrite( FLAG::C0, 0 );
      break;
    case FP_INFINITE:
      flagwrite( FLAG::C3, 0 );
      flagwrite( FLAG::C2, 1 );
      flagwrite( FLAG::C0, 1 );
      break;
    case FP_ZERO:
      flagwrite( FLAG::C3, 1 );
      flagwrite( FLAG::C2, 0 );
      flagwrite( FLAG::C0, 0 );
      break;
    case FP_SUBNORMAL:
      flagwrite( FLAG::C3, 1 );
      flagwrite( FLAG::C2, 1 );
      flagwrite( FLAG::C0, 0 );
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
    
  u64_t tscread() { return instruction_count; }
    
public: 
  struct VUnion
  {
    static unsigned const BYTECOUNT = 16;
      
    typedef void (*arrangement_t)( uint8_t* storage );
    arrangement_t arrangement;
    
    template <typename T>
    static unsigned ItemCount() { return BYTECOUNT / VectorTypeInfo<T>::bytecount; }

    template <typename T>
    static void ToBytes( uint8_t* storage )
    {
      T const* vec = reinterpret_cast<T const*>( storage );

      for (unsigned idx = 0, stop = ItemCount<T>(); idx < stop; ++idx) {
        T val = vec[idx]; 
        VectorTypeInfo<T>::ToBytes( &storage[idx*VectorTypeInfo<T>::bytecount], val );
      }
    }

    template <typename T>
    T*
    GetStorage( uint8_t* storage )
    {
      T* res = reinterpret_cast<T*>( storage );

      arrangement_t current = &ToBytes<T>;
      if (arrangement != current) {
        arrangement( storage );   
        for (int idx = ItemCount<T>(); --idx >= 0;) {
          T val;
          VectorTypeInfo<T>::FromBytes( val, &storage[idx*VectorTypeInfo<T>::bytecount] );
          res[idx] = val;
        }
        arrangement = current;
      }  
     
      return res;
    }
      
    VUnion() : arrangement( &ToBytes<uint8_t> ) {}
  } umms[16];

  uint8_t vmm_storage[16][16];
    
  template<unsigned OPSIZE>
  typename TypeFor<Arch,OPSIZE>::u
  xmm_uread( unsigned reg, unsigned sub )
  {
    typedef typename TypeFor<Arch,OPSIZE>::u u_type;
      
    u_type* u_array = umms[reg].GetStorage<u_type>( &vmm_storage[reg][0] );
      
    return u_array[sub];
  }
    
  template<unsigned OPSIZE>
  void
  xmm_uwrite( unsigned reg, unsigned sub, typename TypeFor<Arch,OPSIZE>::u val )
  {
    typedef typename TypeFor<Arch,OPSIZE>::u u_type;
      
    u_type* u_array = umms[reg].GetStorage<u_type>( &vmm_storage[reg][0] );
      
    u_array[sub] = val;
  }
    
  template<unsigned OPSIZE>
  typename TypeFor<Arch,OPSIZE>::u
  xmm_uread( RMOp const& rmop, unsigned sub )
  {
    if (not rmop.is_memory_operand()) return xmm_uread<OPSIZE>( rmop.ereg(), sub );
    return memread<OPSIZE>( rmop->segment, rmop->effective_address( *this ) + (sub*OPSIZE/8) );
  }
    
  template<unsigned OPSIZE>
  void
  xmm_uwrite( RMOp const& rmop, unsigned sub, typename TypeFor<Arch,OPSIZE>::u val )
  {
    if (not rmop.is_memory_operand()) return xmm_uwrite<OPSIZE>( rmop.ereg(), sub, val );
    return memwrite<OPSIZE>( rmop->segment, rmop->effective_address( *this ) + (sub*OPSIZE/8), val );
  }

  void cpuid();
  
  typedef unisim::component::cxx::processor::intel::Operation<Arch> Operation;
  Operation* latest_instruction;
  Operation* fetch();
  
  bool do_disasm;
  uint64_t instruction_count;

  bool Cond( bool b ) const { return b; }

  struct GDBChecker
  {
    GDBChecker() : dirtmask( 0 ), visited() {}
    uint32_t dirtmask;
    std::map<uint64_t,uint64_t> visited;
    void mark( unsigned reg ) { dirtmask |= (1 << reg); }
    void step( Arch* cpu )
    {
      uint64_t cia = cpu->rip;
      uint64_t revisit = visited[cia]++;
      sink() << "stepi\n";
      if (revisit == 0)
        sink() << "# advance *0x" << std::hex << cia << "\n";
      else
        sink() << "# break *0x" << std::hex << cia << "; cont; cont " << std::dec << revisit << "\n";
      sink() << "insn_assert " << std::dec << aid(cpu) << " $pc 0x" << std::hex  << cia << '\n';
      for (unsigned reg = 0; reg < 16; ++reg)
        {
          if (not ((dirtmask>>reg) & 1)) continue;
          uint64_t value = cpu->u64regs[reg];
          sink() << "insn_assert " << std::dec << aid(cpu) << " " << unisim::component::cxx::processor::intel::DisasmGq(reg) << " 0x" << std::hex << value << '\n';
        }
      dirtmask = 0;
    }
    uint64_t aid( Arch* cpu )
    {
      static uint64_t assertion_id = 0;
      if (++assertion_id > 0x80000) throw 0;
      return assertion_id;
    }
    struct Sink
    {
      Sink() : stream("check.gdb")
      {
        stream << "define insn_assert\n  if $arg1 != $arg3\n    printf \"insn_assert %u failed.\n\", $arg0\n    quit\n  end\nend\n\n";
      }
      std::ostream& operator() () { return stream; }
      std::ofstream stream;
    } sink;
    
  } gdbchecker;
};

Arch::f64_t sine( Arch::f64_t );
Arch::f64_t cosine( Arch::f64_t );
Arch::f64_t tangent( Arch::f64_t );
Arch::f64_t arctan( Arch::f64_t angle );
Arch::f64_t fmodulo( Arch::f64_t, Arch::f64_t );
Arch::f64_t firound( Arch::f64_t, int x87frnd_mode );
Arch::f64_t power( Arch::f64_t, Arch::f64_t );
Arch::f64_t logarithm( Arch::f64_t );
Arch::f64_t square_root( Arch::f64_t );

using unisim::util::arithmetic::BitScanForward;
using unisim::util::arithmetic::BitScanReverse;

#include <unisim/component/cxx/processor/intel/isa/intel.tcc>
#include <unisim/component/cxx/processor/intel/math.hh>

template <typename DECODER>
struct ICache : public DECODER
{
  typedef unisim::component::cxx::processor::intel::Operation<Arch> Operation;

  struct Page
  {
    static unsigned const NUM_OPERATIONS_PER_PAGE = 0x1000;
    Page* next;
    uint64_t key;
    Operation* operations[NUM_OPERATIONS_PER_PAGE];
    uint8_t bytes[NUM_OPERATIONS_PER_PAGE+15];
    
    Page( Page* _next, uint64_t _key ) : next( _next ), key( _key ) { memset( operations, 0, sizeof (operations) ); }
    ~Page() { for( unsigned idx = 0; idx < NUM_OPERATIONS_PER_PAGE; ++idx ) delete operations[idx]; delete next; }
  };
    
  static unsigned const NUM_HASH_TABLE_ENTRIES = 0x1000;
  Page* hash_table[NUM_HASH_TABLE_ENTRIES];
  Page* mru_page;
  ICache()
    : mru_page( 0 )
  {
    memset( hash_table, 0, sizeof (hash_table) );
    std::ifstream source( "certs.txt" );
    for (std::string line; std::getline( source, line );)
      {
        line.erase(std::find_if(line.rbegin(), line.rend(), [](int c) { return not isspace(c); }).base(), line.end());
        certs.insert( line );
      }
  }
  ~ICache()
  {
    for(unsigned idx = 0; idx < NUM_HASH_TABLE_ENTRIES; ++idx)
      delete hash_table[idx];
  }
  
  Operation* Get( unisim::component::cxx::processor::intel::Mode mode, uint64_t address, uint8_t* bytes )
  {
    uint64_t offset;
    Page* page = GetPage( address, offset );
    Operation* operation = page->operations[offset];
      
    if (operation)
      {
        if ((operation->mode == mode) and (memcmp( &bytes[0], &page->bytes[offset], operation->length ) == 0))
          return operation;
        delete operation;
      }
      
    page->operations[offset] = operation = this->Decode( mode, address, bytes );
    memcpy( &page->bytes[offset], bytes, operation->length );
    
    {
      std::ostringstream buf;
      buf << std::hex << address << ":\t" << unisim::component::cxx::processor::intel::DisasmBytes(bytes,operation->length) << '\t';
      operation->disasm( buf );
      if (certs.insert( buf.str() ).second)
        {
          std::cerr << "unknown instruction: " << buf.str() << '\n';
          throw 0;
        }
    }
      
    return operation;
  }

  std::set<std::string> certs;
    
private:
  Page*
  GetPage( uint64_t address, uint64_t& offset )
  {
    uint64_t page_key = address / Page::NUM_OPERATIONS_PER_PAGE;
    offset = address & (Page::NUM_OPERATIONS_PER_PAGE-1);
      
    if (mru_page and (mru_page->key == page_key)) return mru_page;
      
    unsigned index = page_key % NUM_HASH_TABLE_ENTRIES; // hash the key
      
    Page* cur = hash_table[index];
    Page** ref = &hash_table[index];
      
    while (cur) {
      if (cur->key == page_key) {

        *ref = cur->next;
        cur->next = hash_table[index];
        return (mru_page = hash_table[index] = cur);
      }
      ref = &cur->next;
      cur = cur->next;
    }
      
    return (mru_page = hash_table[index] = new Page( hash_table[index], page_key ));
  }
};

struct Decoder
{
  typedef unisim::component::cxx::processor::intel::Operation<Arch> Operation;
  typedef unisim::component::cxx::processor::intel::Mode Mode;
  
  Operation* Decode( Mode mode, uint64_t address, uint8_t* bytes )
  {
    if (Operation* op = getoperation( unisim::component::cxx::processor::intel::InputCode<Arch>( mode, bytes, Arch::OpHeader( address ) ) ))
      return op;
      
    std::cerr << "No decoding for " << std::hex << address << ": " << unisim::component::cxx::processor::intel::DisasmBytes( bytes, 15 ) << std::dec << std::endl;
    throw 0;
    return 0;
  }
};

Arch::Operation*
Arch::fetch()
{
  static ICache<Decoder> icache;
  uint64_t insn_addr = this->rip;
  uint8_t decbuf[15];
  lla_memcpy( decbuf, insn_addr, sizeof (decbuf) );
  Decoder::Mode mode( 1, 0, 1 );
    
  Operation* operation = icache.Get( mode, insn_addr, &decbuf[0] );
    
  this->rip = insn_addr + operation->length;
    
  if (do_disasm) {
    std::ios fmt(NULL);
    fmt.copyfmt(std::cerr);
    std::cerr << std::hex << insn_addr << ":\t";
    operation->disasm( std::cerr );
    std::cerr << " (" << unisim::component::cxx::processor::intel::DisasmBytes(&decbuf[0],operation->length) << ")\n";
    std::cerr.copyfmt(fmt);
  }
    
  ++instruction_count;
  return operation;
}

void
Arch::cpuid()
{
  switch (this->regread( GOd(), 0 )) {
  case 0: {
    this->regwrite( GOd(), 0, u32_t( 1 ) );
  
    char const* name = "GenuineIntel";
    { uint32_t word = 0;
      int idx = 12;
      while (--idx >= 0) {
        word = (word << 8) | name[idx];
        if (idx % 4) continue;
        this->regwrite( GOd(), 3 - (idx/4), u32_t( word ) );
        word = 0;
      }
    }
  } break;
  case 1: {
    uint32_t const eax =
      (6  << 0 /* stepping id */) |
      (0  << 4 /* model */) |
      (15 << 8 /* family */) |
      (0  << 12 /* processor type */) |
      (0  << 16 /* extended model */) |
      (0  << 20 /* extended family */);
    this->regwrite( GOd(), 0, u32_t( eax ) );
    
    uint32_t const ebx =
      (0 <<  0 /* Brand index */) |
      (4 <<  8 /* Cache line size (/ 64bits) */) |
      (1 << 16 /* Maximum number of addressable IDs for logical processors in this physical package* */) |
      (0 << 24 /* Initial APIC ID */);
    this->regwrite( GOd(), 3, u32_t( ebx ) );
    
    uint32_t const ecx =
      (0 << 0x00 /* Streaming SIMD Extensions 3 (SSE3) */) |
      (0 << 0x01 /* PCLMULQDQ Available */) |
      (0 << 0x02 /* 64-bit DS Area */) |
      (0 << 0x03 /* MONITOR/MWAIT */) |
      (0 << 0x04 /* CPL Qualified Debug Store */) |
      (0 << 0x05 /* Virtual Machine Extensions */) |
      (0 << 0x06 /* Safer Mode Extensions */) |
      (0 << 0x07 /* Enhanced Intel SpeedStep® technology */) |
      (0 << 0x08 /* Thermal Monitor 2 */) |
      (0 << 0x09 /* Supplemental Streaming SIMD Extensions 3 (SSSE3) */) |
      (0 << 0x0a /* L1 Context ID */) |
      (0 << 0x0b /* Reserved */) |
      (0 << 0x0c /* FMA */) |
      (0 << 0x0d /* CMPXCHG16B Available */) |
      (0 << 0x0e /* xTPR Update Control */) |
      (0 << 0x0f /* Perfmon and Debug Capability */) |
      (0 << 0x10 /* Reserved */) |
      (0 << 0x11 /* Process-context identifiers */) |
      (0 << 0x12 /* DCA */) |
      (0 << 0x13 /* SSE4.1 */) |
      (0 << 0x14 /* SSE4.2 */) |
      (0 << 0x15 /* x2APIC */) |
      (1 << 0x16 /* MOVBE Available */) |
      (1 << 0x17 /* POPCNT Available */) |
      (0 << 0x18 /* TSC-Deadline */) |
      (0 << 0x19 /* AESNI */) |
      (0 << 0x1a /* XSAVE */) |
      (0 << 0x1b /* OSXSAVE */) |
      (0 << 0x1c /* AVX */) |
      (1 << 0x1d /* F16C */) |
      (1 << 0x1e /* RDRAND Available */) |
      (1 << 0x1f /* Is virtual machine */);
    this->regwrite( GOd(), 1, u32_t( ecx ) );
    
    uint32_t const edx =
      (1 << 0x00 /* Floating Point Unit On-Chip */) |
      (0 << 0x01 /* Virtual 8086 Mode Enhancements */) |
      (0 << 0x02 /* Debugging Extensions */) |
      (0 << 0x03 /* Page Size Extension */) |
      (0 << 0x04 /* Time Stamp Counter */) |
      (0 << 0x05 /* Model Specific Registers RDMSR and WRMSR Instructions */) |
      (0 << 0x06 /* Physical Address Extension */) |
      (0 << 0x07 /* Machine Check Exception */) |
      (0 << 0x08 /* CMPXCHG8B Available */) |
      (0 << 0x09 /* APIC On-Chip */) |
      (0 << 0x0a /* Reserved */) |
      (0 << 0x0b /* SYSENTER and SYSEXIT Instructions */) |
      (0 << 0x0c /* Memory Type Range Registers */) |
      (0 << 0x0d /* Page Global Bit */) |
      (0 << 0x0e /* Machine Check ARCHitecture */) |
      (1 << 0x0f /* Conditional Move Instructions */) |
      (0 << 0x10 /* Page Attribute Table */) |
      (0 << 0x11 /* 36-Bit Page Size Extension */) |
      (0 << 0x12 /* Processor Serial Number */) |
      (0 << 0x13 /* CLFLUSH Instruction */) |
      (0 << 0x14 /* Reserved */) |
      (0 << 0x15 /* Debug Store */) |
      (0 << 0x16 /* Thermal Monitor and Software Controlled Clock Facilities */) |
      (0 << 0x17 /* Intel MMX Technology */) |
      (0 << 0x18 /* FXSAVE and FXRSTOR Instructions */) |
      (0 << 0x19 /* SSE */) |
      (0 << 0x1a /* SSE2 */) |
      (0 << 0x1b /* Self Snoop */) |
      (0 << 0x1c /* Max APIC IDs reserved field is Valid */) |
      (0 << 0x1d /* Thermal Monitor */) |
      (0 << 0x1e /* Resrved */) |
      (0 << 0x1f /* Pending Break Enable */);
    this->regwrite( GOd(), 2, u32_t( edx ) );
    
  } break;
  case 2: {
    this->regwrite( GOd(), 0, u32_t( 0 ) );
    this->regwrite( GOd(), 3, u32_t( 0 ) );
    this->regwrite( GOd(), 1, u32_t( 0 ) );
    this->regwrite( GOd(), 2, u32_t( 0 ) );
  } break;
  case 4: {
    // Small cache config
    switch (this->regread( GOd(), 1 )) { // %ecx holds requested cache id
    case 0: { // L1 D-CACHE
      this->regwrite( GOd(), 0, u32_t( (1 << 26) | (0 << 14) | (1 << 8) | (1 << 5) | (1 << 0) ) ); // 0x4000121
      this->regwrite( GOd(), 3, u32_t( (0 << 26) | (3 << 22) | (0 << 12) | (0x3f << 0) ) ); // 0x1c0003f
      this->regwrite( GOd(), 1, u32_t( (0 << 22) | (0x03f << 0) ) ); // 0x000003f
      this->regwrite( GOd(), 2, u32_t( 0x0000001 ) ); // 0x0000001
    } break;
    case 1: { // L1 I-CACHE
      this->regwrite( GOd(), 0, u32_t( (1 << 26) | (0 << 14) | (1 << 8) | (1 << 5) | (2 << 0) ) ); // 0x4000122
      this->regwrite( GOd(), 3, u32_t( (0 << 26) | (3 << 22) | (0 << 12) | (0x3f << 0) ) ); // 0x1c0003f
      this->regwrite( GOd(), 1, u32_t( (0 << 22) | (0x03f << 0) ) ); // 0x000003f
      this->regwrite( GOd(), 2, u32_t( 0x0000001 ) ); // 0x0000001
    } break;
    case 2: { // L2 U-CACHE
      this->regwrite( GOd(), 0, u32_t( (1 << 26) | (1 << 14) | (1 << 8) | (2 << 5) | (3 << 0) ) ); // 0x4000143
      this->regwrite( GOd(), 3, u32_t( (1 << 26) | (3 << 22) | (0 << 12) | (0x3f << 0) ) ); // 0x5c0003f
      this->regwrite( GOd(), 1, u32_t( (0 << 22) | (0xfff << 0) ) ); // 0x0000fff
      this->regwrite( GOd(), 2, u32_t( 0x0000001 ) ); // 0x0000001
    } break;
    case 3: { // TERMINATING NULL ENTRY
      // 0, 0, 0, 0
      this->regwrite( GOd(), 0, u32_t( 0 ) );
      this->regwrite( GOd(), 3, u32_t( 0 ) );
      this->regwrite( GOd(), 1, u32_t( 0 ) );
      this->regwrite( GOd(), 2, u32_t( 0 ) );
    } break;
    }
  } break;
  
  case 0x80000000: {
    this->regwrite( GOd(), 0, u32_t( 0x80000001 ) );
    this->regwrite( GOd(), 3, u32_t( 0 ) );
    this->regwrite( GOd(), 1, u32_t( 0 ) );
    this->regwrite( GOd(), 2, u32_t( 0 ) );
  } break;
  case 0x80000001: {
    this->regwrite( GOd(), 0, u32_t( 0 ) );
    this->regwrite( GOd(), 3, u32_t( 0 ) );
    this->regwrite( GOd(), 1, u32_t( 0 ) );
    this->regwrite( GOd(), 2, u32_t( 0 ) );
  } break;
  default:
    std::cerr << "Unknown cmd for cpuid, " << std::hex
              << "%eax=0x" << this->regread( GOd(), 0 ) << ", "
              << "%eip=0x" << latest_instruction->address << "\n";
    throw "not implemented";
    break;
  }
}

struct FPException : public std::exception {};

Arch::f64_t sine( Arch::f64_t angle ) { return sin( angle ); }

Arch::f64_t cosine( Arch::f64_t angle ) { return cos( angle ); }

Arch::f64_t tangent( Arch::f64_t angle ) { return tan( angle ); }

Arch::f64_t arctan( Arch::f64_t angle ) { return atan( angle ); }

Arch::f64_t fmodulo( Arch::f64_t dividend, Arch::f64_t modulus ) { return fmod( dividend, modulus ); }

Arch::f64_t firound( Arch::f64_t value, int x87frnd_mode )
{
  // double floor_res = floor( value );

  double result = unisim::component::cxx::processor::intel::x87frnd( value, unisim::component::cxx::processor::intel::x87frnd_mode_t( x87frnd_mode ) );

  // if (floor_res != result) {
  //   std::cerr << "Rounding result fixed; old: " << floor_res << ", new: " << result << std::endl;
  // }

  return result;
}

Arch::f64_t power( Arch::f64_t exponent, Arch::f64_t value ) { return pow( exponent, value ); }

Arch::f64_t logarithm( Arch::f64_t value ) { return log2( value ); }

Arch::f64_t square_root( Arch::f64_t value ) { return sqrt( value ); }


int
main( int argc, char *argv[] )
{
  uintptr_t simargs_idx = 1;
  std::vector<std::string> simargs(&argv[simargs_idx], &argv[argc]);
  
  {
    std::cerr << "arguments:\n";
    unsigned idx = 0;
    for (std::string const& arg : simargs) {
      std::cerr << "  args[" << idx << "]: " << arg << '\n';
    }
  }
  
  if (simargs.size() == 0) {
    std::cerr << "Simulation command line empty." << std::endl;
    return 1;
  }

  Arch cpu;
  LinuxOS linux64( std::cerr, &cpu, &cpu, &cpu );
  cpu.SetLinuxOS( &linux64 );
  cpu.do_disasm = true;
  
  // Loading image
  std::cerr << "*** Loading elf image: " << simargs[0] << " ***" << std::endl;
  // std::vector<std::string> envs;
  // envs.push_back( "LANG=C" );
  // linux64.Process( simargs, envs );
  linux64.Core( simargs[0] );
  
  std::cerr << "\n*** Run ***" << std::endl;
  while (not linux64.exited)
    {
      Arch::Operation* op = cpu.fetch();
      // op->disasm( std::cerr );
      // std::cerr << std::endl;
      asm volatile ("operation_execute:");
      op->execute( cpu );
      //{ uint64_t chksum = 0; for (unsigned idx = 0; idx < 8; ++idx) chksum ^= cpu.regread( GOd(), idx ); std::cerr << '[' << std::hex << chksum << std::dec << ']'; }
      
      // if ((cpu.instruction_count % 0x1000000) == 0)
      //   { std::cerr << "Executed instructions: " << std::dec << cpu.instruction_count << " (" << std::hex << op->address << std::dec << ")"<< std::endl; }
    }
  
  std::cerr << "Program exited with status:" << linux64.app_ret_status << std::endl;
  
  
  return 0;
}
