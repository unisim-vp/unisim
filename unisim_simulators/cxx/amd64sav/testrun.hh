/*
 *  Copyright (c) 2019,
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

#ifndef __TESTRUN_HH__
#define __TESTRUN_HH__

#include <arch.hh>
#include <unisim/util/random/random.hh>
#include <unisim/component/cxx/processor/intel/types.hh>
#include <unisim/util/arithmetic/arithmetic.hh>
#include <iosfwd>
#include <cstdlib>
#include <cassert>
#include <inttypes.h>

namespace test
{
  template <typename A, unsigned S> using TypeFor = typename unisim::component::cxx::processor::intel::TypeFor<A,S>;

  struct Arch
  {
    typedef uint8_t      u8_t;
    typedef uint16_t     u16_t;
    typedef uint32_t     u32_t;
    typedef uint64_t     u64_t;
    typedef int8_t       s8_t;
    typedef int16_t      s16_t;
    typedef int32_t      s32_t;
    typedef int64_t      s64_t;
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

    typedef unisim::component::cxx::processor::intel::SSE SSE;
    typedef unisim::component::cxx::processor::intel::XMM XMM;
    typedef unisim::component::cxx::processor::intel::YMM YMM;

    typedef GOq   GR;
    typedef u64_t gr_type;

    void run(review::Interface::testcode_t testcode, uint64_t* data);

    struct OpHeader
    {
      OpHeader( addr_t _address ) : address( _address ) {} addr_t address;
    };

    Arch()
      : rip()
      , u64regs()
      , m_flags()
        //, m_fregs()
        // , m_ftop(0)
        // , m_fcw(0x37f)
      , umms()
      , vmm_storage()
      , mxcsr(0x1fa0)
      , latest_instruction(0)
        //        , do_disasm(false)
        //        , instruction_count(0)
    {}

    ~Arch()
    {
      for (unsigned reg = 0; reg < 16; ++reg)
        umms[reg].Clear(&vmm_storage[reg][0]);
    }


    typedef unisim::component::cxx::processor::intel::RMOp<Arch> RMOp;

    // MEMORY STATE
    // Segment Registers
    void                        segregwrite( unsigned idx, uint16_t value );
    uint16_t                    segregread( unsigned idx );

    addr_t segbase( unsigned seg ) { if (seg >= 4) throw 0; return 0; }

    template <unsigned OPSIZE>
    void
    memwrite( unsigned _seg, u64_t _addr, typename TypeFor<Arch,OPSIZE>::u _val )
    {
      typedef typename TypeFor<Arch,OPSIZE>::u value_type;
      addr_t addr = _addr + segbase(_seg);
      value_type* ptr = reinterpret_cast<value_type*>(addr);
      *ptr = _val;
    }

    template <unsigned OPSIZE>
    typename TypeFor<Arch,OPSIZE>::u
    memread( unsigned _seg, u64_t _addr )
    {
      typedef typename TypeFor<Arch,OPSIZE>::u value_type;
      addr_t addr = _addr + segbase(_seg);
      value_type const* ptr = reinterpret_cast<value_type const*>(addr);
      return *ptr;
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

    void shrink_stack( addr_t offset ) { regwrite( GR(), 4, regread( GR(), 4 ) + offset ); }
    void grow_stack( addr_t offset ) { regwrite( GR(), 4, regread( GR(), 4 ) - offset ); }

    template <unsigned OPSIZE>
    void
    push( typename TypeFor<Arch,OPSIZE>::u value )
    {
      // TODO: handle stack address size
      u64_t sptr = regread( GR(), 4 ) - u64_t( OPSIZE/8 );
      memwrite<OPSIZE>( unisim::component::cxx::processor::intel::SS, sptr, value );
      regwrite( GR(), 4, sptr );
    }

    f32_t                       fmemread32( unsigned int _seg, addr_t _addr )
    {
      union IEEE754_t { float as_f; uint32_t as_u; } word;
      word.as_u = memread<32>( _seg, _addr );
      return word.as_f;
    }
    f64_t                       fmemread64( unsigned int _seg, addr_t _addr )
    {
      union IEEE754_t { double as_f; uint64_t as_u; } word;
      word.as_u = memread<64>( _seg, _addr );
      return word.as_f;
    }
    f80_t fmemread80( unsigned int _seg, addr_t _addr )
    {
      addr_t addr = _addr + segbase(_seg);
      uint8_t buf[10];
      memcpy(&buf[0], reinterpret_cast<uint8_t const*>(addr), sizeof buf);
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
    fpmemread( unsigned seg, addr_t addr )
    {
      if (OPSIZE==32) return this->fmemread32( seg, addr );
      if (OPSIZE==64) return this->fmemread64( seg, addr );
      if (OPSIZE==80) return this->fmemread80( seg, addr );
      throw 0;        return typename TypeFor<Arch,OPSIZE>::f();
    }

    template <unsigned OPSIZE>
    typename TypeFor<Arch,OPSIZE>::f
    frmread( RMOp const& rmop )
    {
      typedef typename TypeFor<Arch,OPSIZE>::f f_type;
      if (not rmop.is_memory_operand()) return f_type( this->fread( rmop.ereg() ) );
      return this->fpmemread<OPSIZE>( rmop->segment, rmop->effective_address( *this ) );
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
    void                        fmemwrite80( unsigned int _seg, u64_t _addr, f80_t _val )
    {
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

      uint8_t buf[10];

      buf[9] = (sign << 7) | (uint8_t( exponent >> 8 ) & 0x7f);
      buf[8] = uint8_t( exponent ) & 0xff;
      for ( uintptr_t idx = 0; idx < 8; ++idx)
        buf[idx] = uint8_t( mantissa >> (idx*8) );


      addr_t addr = _addr + segbase(_seg);
      memcpy(reinterpret_cast<uint8_t*>(addr), &buf[0], sizeof buf);
    }

    template <unsigned OPSIZE>
    void
    fpmemwrite( unsigned seg, addr_t addr, typename TypeFor<Arch,OPSIZE>::f value )
    {
      if (OPSIZE==32) return fmemwrite32( seg, addr, value );
      if (OPSIZE==64) return fmemwrite64( seg, addr, value );
      if (OPSIZE==80) return fmemwrite80( seg, addr, value );
      throw 0;
    }

    template <unsigned OPSIZE>
    void
    frmwrite( RMOp const& rmop, typename TypeFor<Arch,OPSIZE>::f value )
    {
      if (not rmop.is_memory_operand()) return fwrite( rmop.ereg(), f64_t( value ) );
      fpmemwrite<OPSIZE>( rmop->segment, rmop->effective_address( *this ), value );
    }

    template <class GOP>
    typename TypeFor<Arch,GOP::SIZE>::u
    rmread( GOP const& g, RMOp const& rmop )
    {
      if (not rmop.is_memory_operand())
        return regread( g, rmop.ereg() );

      return memread<GOP::SIZE>( rmop->segment, rmop->effective_address( *this ) );
    }

    template <class GOP>
    void
    rmwrite( GOP const&, RMOp const& rmop, typename TypeFor<Arch,GOP::SIZE>::u value )
    {
      if (not rmop.is_memory_operand())
        return regwrite( GOP(), rmop.ereg(), value );

      return memwrite<GOP::SIZE>( rmop->segment, rmop->effective_address( *this ), value );
    }

    addr_t rip;

    enum ipproc_t { ipjmp = 0, ipcall, ipret };
    addr_t                      getnip() { return rip; }
    void                        setnip( addr_t _rip, ipproc_t ipproc = ipjmp ) { rip = _rip; }

    void                        syscall();
    void                        interrupt( int op, int code );

    uint64_t u64regs[16]; ///< extended reg

    template <class GOP>
    typename TypeFor<Arch,GOP::SIZE>::u regread( GOP const&, unsigned idx )
    {
      return typename TypeFor<Arch,GOP::SIZE>::u( u64regs[idx] );
    }

    u8_t regread( GObLH const&, unsigned idx )
    {
      unsigned reg = idx%4, sh = idx*2 & 8;
      return u8_t( u64regs[reg] >> sh );
    }

    template <class GOP>
    void regwrite( GOP const&, unsigned idx, typename TypeFor<Arch,GOP::SIZE>::u value )
    {
      u64regs[idx] = u64_t( value );
      //gdbchecker.gmark( idx );
    }

    void regwrite( GObLH const&, unsigned idx, u8_t value )
    {
      unsigned reg = idx%4, sh = idx*2 & 8;
      u64regs[reg] = (u64regs[reg] & ~u64_t(0xff << sh)) | ((value & u64_t(0xff)) << sh);
      //gdbchecker.gmark( reg );
    }
    void regwrite( GOb const&, unsigned idx, u8_t value )
    {
      u64regs[idx] = (u64regs[idx] & ~u64_t(0xff)) | ((value & u64_t(0xff)));
      //gdbchecker.gmark( idx );
    }
    void regwrite( GOw const&, unsigned idx, u16_t value )
    {
      u64regs[idx] = (u64regs[idx] & ~u64_t(0xffff)) | ((value & u64_t(0xffff)));
      //gdbchecker.gmark( idx );
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
    unsigned                    m_ftop;
    uint16_t                    m_fcw;

  public:
    void                        fnanchk( f64_t value ) {};
    unsigned                    ftopread() { return m_ftop; }
    int                         fcwreadRC() const { return int( (m_fcw >> 10) & 3 ); }
    void                        fpush( f64_t value )
    { fnanchk( value ); m_ftop = ((m_ftop + 0x7) & 0x7); m_fregs[m_ftop] = value; /*m_dirtfregs |= (1 << m_ftop);*/ }
    void                        fwrite( unsigned idx, f64_t value )
    { fnanchk( value ); unsigned fidx = (m_ftop + idx) & 0x7; m_fregs[fidx] = value; /*m_dirtfregs |= (1 << fidx);*/ }
    f64_t                       fpop()
    { f64_t value = m_fregs[m_ftop++]; m_ftop &= 0x7; fnanchk( value ); return value; }
    f64_t                       fread( unsigned idx )
    { unsigned fidx = (m_ftop + idx) & 0x7; f64_t value = m_fregs[fidx]; fnanchk( value ); return value; }
    void                        finit()
    {
      m_ftop = 0;
      m_fcw = 0x37f;
      flagwrite( FLAG::C0, 0 );
      flagwrite( FLAG::C1, 0 );
      flagwrite( FLAG::C2, 0 );
      flagwrite( FLAG::C3, 0 );
    }

    void      fxam();

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

    void  fcwwrite( u16_t _value );

    u64_t tscread();

  public:
    struct VUConfig
    {
      static unsigned const BYTECOUNT = 32;
      template <typename T> using TypeInfo = unisim::component::cxx::vector::VectorTypeInfo<T,0>;
      typedef u8_t Byte;
    };
    static unsigned const VREGCOUNT = 16;

    unisim::component::cxx::vector::VUnion<VUConfig> umms[VREGCOUNT];
    uint8_t vmm_storage[VREGCOUNT][VUConfig::BYTECOUNT];

    uint32_t mxcsr;

    void mxcswrite(uint32_t v) { mxcsr = v; }
    uint32_t mxcsread() { return mxcsr; }

    template <class VR> static unsigned vmm_wsize( VR const& vr ) { return VR::size() / 8; }
    static unsigned vmm_wsize( unisim::component::cxx::processor::intel::SSE const& ) { return VUConfig::BYTECOUNT; }

    template <class VR, class ELEM>
    ELEM
    vmm_read( VR const& vr, unsigned reg, unsigned sub, ELEM const& e )
    {
      ELEM const* elems = umms[reg].GetConstStorage( &vmm_storage[reg][0], e, vr.size() / 8 );
      return elems[sub];
    }

    template <class VR, class ELEM>
    void
    vmm_write( VR const& vr, unsigned reg, unsigned sub, ELEM const& e )
    {
      ELEM* elems = umms[reg].GetStorage( &vmm_storage[reg][0], e, vmm_wsize( vr ) );
      elems[sub] = e;
      //gdbchecker.xmark(reg);
    }

    // Integer case
    template <class TYPE> TYPE vmm_memread( unsigned seg, addr_t addr, TYPE const& e )
    {
      typedef unisim::component::cxx::processor::intel::atpinfo<Arch,TYPE> atpinfo;
      return TYPE(memread<atpinfo::bitsize>(seg,addr));
    }

    f32_t vmm_memread( unsigned seg, addr_t addr, f32_t const& e ) { return fmemread32( seg, addr ); }
    f64_t vmm_memread( unsigned seg, addr_t addr, f64_t const& e ) { return fmemread64( seg, addr ); }
    f80_t vmm_memread( unsigned seg, addr_t addr, f80_t const& e ) { return fmemread80( seg, addr ); }

    // Integer case
    template <class TYPE> void vmm_memwrite( unsigned seg, addr_t addr, TYPE const& e )
    {
      typedef unisim::component::cxx::processor::intel::atpinfo<Arch,TYPE> atpinfo;
      memwrite<atpinfo::bitsize>(seg,addr,typename atpinfo::utype(e));
    }

    void vmm_memwrite( unsigned seg, addr_t addr, f32_t const& e ) { return fmemwrite32( seg, addr, e ); }
    void vmm_memwrite( unsigned seg, addr_t addr, f64_t const& e ) { return fmemwrite64( seg, addr, e ); }
    void vmm_memwrite( unsigned seg, addr_t addr, f80_t const& e ) { return fmemwrite80( seg, addr, e ); }

    template <class VR, class ELEM>
    ELEM
    vmm_read( VR const& vr, RMOp const& rmop, unsigned sub, ELEM const& e )
    {
      if (not rmop.is_memory_operand()) return vmm_read( vr, rmop.ereg(), sub, e );
      return vmm_memread( rmop->segment, rmop->effective_address( *this ) + sub*VUConfig::TypeInfo<ELEM>::bytecount, e );
    }

    template <class VR, class ELEM>
    void
    vmm_write( VR const& vr, RMOp const& rmop, unsigned sub, ELEM const& e )
    {
      if (not rmop.is_memory_operand()) return vmm_write( vr, rmop.ereg(), sub, e );
      return vmm_memwrite( rmop->segment, rmop->effective_address( *this ) + sub*VUConfig::TypeInfo<ELEM>::bytecount, e );
    }

    void xgetbv();
    void cpuid();
    void _DE();
    void unimplemented();

    typedef unisim::component::cxx::processor::intel::Operation<Arch> Operation;
    Operation* latest_instruction;
    Operation* fetch();

    void stop();
    struct Unimplemented {};
    void noexec( Operation const& op );
    bool Test( bool b ) const { return b; }
  };

  struct UInt128
  {
    UInt128() : bits{0,0} {}
    UInt128( uint64_t value ) : bits{value,0} {}
    UInt128( uint64_t hi, uint64_t lo ) : bits{lo,hi} {}

    template <typename T> UInt128 operator << (T lshift) const { UInt128 res(*this); res <<= lshift; return res; }
    template <typename T> UInt128 operator >> (T rshift) const { UInt128 res(*this); res >>= rshift; return res; }

    template <typename T>
    UInt128& operator <<= (T lshift)
    {
      if (lshift >= 128) { bits[1] = 0; bits[0] = 0; return *this; }
      if (lshift >= 64) { bits[1] = bits[0]; bits[0] = 0; lshift -= 64; }
      T rshift = 63 - lshift;
      bits[1] = (bits[1] << lshift) | (bits[0] >> rshift);
      bits[0] = (bits[0] << lshift);
      return *this;
    }

    template <typename T>
    UInt128& operator >>= (T rshift)
    {
      if (rshift >= 128) { bits[1] = 0; bits[0] = 0; return *this; }
      if (rshift >= 64) { bits[0] = bits[1]; bits[1] = 0; rshift -= 64; }
      T lshift = 63 - rshift;
      bits[0] = (bits[0] >> rshift) | (bits[1] << lshift);
      bits[1] >>= rshift;
      return *this;
    }

    UInt128 operator | (UInt128 const& rhs) { UInt128 res(*this); res |= rhs; return res; }
    UInt128& operator |= (UInt128 const& rhs) { bits[1] |= rhs.bits[1]; bits[0] |= rhs.bits[0]; return *this; }
    UInt128 operator & (UInt128 const& rhs) { UInt128 res(*this); res &= rhs; return res; }
    UInt128& operator &= (UInt128 const& rhs) { bits[1] &= rhs.bits[1]; bits[0] &= rhs.bits[0]; return *this; }

    unsigned clz() const
    {
      return bits[1] ?
        63 - unisim::util::arithmetic::BitScanReverse(bits[1]) :
        127 - unisim::util::arithmetic::BitScanReverse(bits[0]);
    }
    UInt128& neg()
    {
      bits[0] = ~bits[0] + 1;
      bits[1] = ~bits[1] + uint64_t(not bits[0]);
      return *this;
    }

    explicit operator uint64_t () const { return bits[0]; }
    explicit operator bool () const { return bits[0] or bits[1]; }

    uint64_t bits[2];
  };

  struct SInt128 : public UInt128
  {
    SInt128( int64_t value ) : UInt128( value, value >> 63 ) {}
    SInt128( uint64_t value ) : UInt128( value, 0 ) {}
    SInt128( UInt128 const& value ) : UInt128( value ) {}
    template <typename T> UInt128 operator >> (T rshift) const { UInt128 res(*this); res >>= rshift; return res; }

    template <typename T>
    UInt128& operator >>= (T rshift)
    {
      if (rshift >= 128) { bits[0] = int64_t(bits[1]) >> 63; bits[1] = bits[0]; return *this; }
      if (rshift >= 64) { bits[0] = bits[1]; bits[1] = int64_t(bits[1]) >> 63; rshift -= 64; }
      T lshift = 63 - rshift;
      bits[0] = (bits[0] >> rshift) | (bits[1] << lshift);
      bits[1] = int64_t(bits[1]) >> rshift;
      return *this;
    }

  };

  void eval_div( Arch& arch, uint64_t& hi, uint64_t& lo, uint64_t divisor );
  void eval_div( Arch& arch, int64_t& hi, int64_t& lo, int64_t divisor );
  void eval_mul( Arch& arch, uint64_t& hi, uint64_t& lo, uint64_t multiplier );
  void eval_mul( Arch& arch, int64_t& hi, int64_t& lo, int64_t multiplier );

  Arch::f64_t sine( Arch::f64_t );
  Arch::f64_t cosine( Arch::f64_t );
  Arch::f64_t tangent( Arch::f64_t );
  Arch::f64_t arctan( Arch::f64_t angle );
  Arch::f64_t fmodulo( Arch::f64_t, Arch::f64_t );
  Arch::f64_t firound( Arch::f64_t, int x87frnd_mode );
  Arch::f64_t power( Arch::f64_t, Arch::f64_t );
  Arch::f64_t logarithm( Arch::f64_t );
  Arch::f64_t square_root( Arch::f64_t );


} /* end of namespace test */


#endif // __TESTRUN_HH__
