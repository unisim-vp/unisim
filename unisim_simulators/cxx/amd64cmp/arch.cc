/*
 *  Copyright (c) 2018-2022,
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

#include "arch.hh"
#include "tracee.hh"
#include "linuxsystem.hh"
#include <unisim/component/cxx/processor/intel/math.hh>
#include <unisim/component/cxx/processor/intel/execute.hh>
// #include <unisim/util/os/linux_os/linux.hh>
// #include <unisim/util/os/linux_os/amd64.hh>
#include <unisim/util/arithmetic/arithmetic.hh>
#include <unisim/util/likely/likely.hh>
#include <sstream>
#include <cmath>
#include <cassert>

struct VMMRegister : public unisim::service::interfaces::Register
{
  typedef unisim::component::cxx::processor::intel::YMM VR;
  enum { BYTECOUNT = Arch::VUConfig::BYTECOUNT };

  VMMRegister(Arch& _cpu, unsigned _reg) : cpu(_cpu), reg(_reg) { std::ostringstream buf; Disasm(buf); name = buf.str().substr(1); } std::string name; Arch& cpu; unsigned reg;
  void Disasm(std::ostream& sink) const { sink << unisim::component::cxx::processor::intel::DisasmV( VR(), reg ); }
  char const* GetName() const override { return name.c_str(); }
  void GetValue(void *buffer) const override
  {
    uint8_t* buf = reinterpret_cast<uint8_t*>(buffer);
    std::fill(&buf[0],&buf[BYTECOUNT],0);
    auto const& ureg = cpu.umms[reg];
    ureg.transfer( &buf[0], &cpu.vmm_storage[reg][0], ureg.size, false );
  }
  void SetValue(const void *buffer) override
  {
    uint8_t* elems = cpu.umms[reg].GetStorage( &cpu.vmm_storage[reg][0], uint8_t(), BYTECOUNT );
    memcpy(elems,buffer,BYTECOUNT);
    cpu.vmm_regcheck(reg);
  }
  int GetSize() const override { return BYTECOUNT; }
};

Arch::Arch(char const* name, unisim::kernel::Object* parent, Tracee const& _tracee, Amd64LinuxOS& _linux_os)
  : unisim::kernel::Object(name, parent)
  , unisim::kernel::Service<unisim::service::interfaces::MemoryInjection<uint64_t>>(name, parent)
  , unisim::kernel::Service<unisim::service::interfaces::Memory<uint64_t>>(name, parent)
  , unisim::kernel::Service<unisim::service::interfaces::Registers>(name, parent)
  , tracee(_tracee)
  , linux_os(_linux_os)
  , segregs(), fs_base(), gs_base()
  , rip(), u64regs(), m_flags(), flagmask(0)
  , m_fregs(), m_ftop(0), m_fcw(0x37f)
  , umms(), vmm_storage(), mxcsr(0x1fa0)
  , latest_instruction(0)
  , hash_table()
  , mru_page(0)
  , enable_disasm(false)
  , accurate(true)
  , instruction_count(0)
{
  //  regmap["rip"] = new unisim::util::debug::SimpleRegister<uint64_t>("rip", &this->rip);
  // regmap["fs_base"] = new unisim::util::debug::SimpleRegister<uint64_t>("fs_base", &this->fs_base);
  // regmap["gs_base"] = new unisim::util::debug::SimpleRegister<uint64_t>("gs_base", &this->fs_base);
  // regmap["fcw"] = new unisim::util::debug::SimpleRegister<uint16_t>("fcw", &this->m_fcw);

  struct EFlagsRegister : public unisim::service::interfaces::Register
  {
    EFlagsRegister(Arch& _cpu, unsigned _size) : cpu(_cpu), size(_size) {}
    Arch& cpu;
    unsigned size;
    char const* GetName() const override
    {
      struct Bad {};
      switch (size) {
      default: throw Bad();
      case 4: return "eflags";
      case 8: return "rflags";
      }
    }
    void GetValue(void* buffer) const override
    {
      uint32_t bits = unisim::component::cxx::processor::intel::eflagsread(cpu);
      struct Bad {};
      switch (size) {
      default: throw Bad();
      case 4: *((uint32_t*)buffer) = bits; break;
      case 8: *((uint64_t*)buffer) = bits; break;
      }
    }
    void SetValue(void const* buffer) override
    {
      struct Bad {};
      uint32_t bits;
      switch (size) {
      default: throw Bad();
      case 4: bits = *((uint32_t*)buffer); break;
      case 8: bits = *((uint64_t*)buffer); break;
      }
      unisim::component::cxx::processor::intel::eflagswrite(cpu, bits);
      uint32_t chk = unisim::component::cxx::processor::intel::eflagsread(cpu);
      if (bits != chk)
        std::cerr << "Warning: imported rflags " << std::hex << bits << " ended with " << chk << std::endl;
    }
    int GetSize() const override { return size; }
  };

  for (unsigned size = 4; size <= 8; size *= 2)
    regmap.insert(new EFlagsRegister(*this, size));

  for (int idx = 0; idx < 16; ++idx)
    regmap.insert(new VMMRegister(*this, idx));

  struct GPRegister : public unisim::service::interfaces::Register
  {
    GPRegister( Arch& _arch, unsigned _reg ) : name(), arch(_arch), reg(_reg) { std::ostringstream buf; Disasm(buf); name = buf.str().substr(1); } std::string name; Arch& arch; unsigned reg;
    void Disasm(std::ostream& sink) const { sink << unisim::component::cxx::processor::intel::DisasmG( GOq(), reg ); }
    char const* GetName() const override { return name.c_str(); }
    void GetValue(void *buffer) const { *((u64_t*)buffer) = arch.regread( GOq(), reg ); }
    void SetValue(const void *buffer) { arch.regwrite( GOq(), reg, *((u64_t*)buffer) ); }
    int GetSize() const { return 8; }
  };

  for (int idx = 0; idx < 16; ++idx)
    regmap.insert(new GPRegister(*this, idx));

  struct SegBaseRegister : public unisim::service::interfaces::Register
  {
    SegBaseRegister( Arch& _arch, char reg ) : arch(_arch), name(std::string(1,reg) + "s_base") {} Arch& arch; std::string name;
    char const* GetName() const override { return name.c_str(); }
    void GetValue(void *buffer) const { *((u64_t*)buffer) = arch.sbaseread( name[0] ); }
    void SetValue(const void *buffer) { arch.sbasewrite( name[0], *((u64_t*)buffer) ); }
    int GetSize() const { return 8; }
  };

  for (char const* p = "fg"; *p; ++p)
    regmap.insert(new SegBaseRegister(*this, *p));

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

  struct X87Register : public unisim::service::interfaces::Register
  {
    X87Register( Arch& _arch, unsigned _idx ) : name(), arch(_arch), idx(_idx) { std::ostringstream buf; buf << "st" << idx; name = buf.str(); } std::string name; Arch& arch; unsigned idx;
    char const* GetName() const { return name.c_str(); }
    void GetValue(void *buffer) const { *((f64_t*)buffer) = arch.fread( idx ); }
    void SetValue(const void *buffer) { arch.fwrite( idx, *((f64_t*)buffer) ); }
    int GetSize() const { return 8; }
  };

  for (int idx = 0; idx < 8; ++idx)
    regmap.insert(new X87Register( *this, idx ));
}

void
Arch::StepInstruction()
{
  uint64_t insn_addr = this->rip;

  Arch::Operation* op = fetch();
  if (not op) throw op;

  this->rip = insn_addr + op->length;

  asm volatile ("operation_execute:");
  op->execute( *this );

  tracee.StepInstruction();

  if (tracee.GetInsnAddr() != this->rip)
    throw 0;

  for (auto const& update : updates)
    {
      update->check(*this, tracee);
    }
  updates.clear();
  accurate = true;
  flagmask = 0;
}

Arch::Operation*
Arch::fetch()
{
  addr_t insn_addr = this->rip;
  asm volatile ("operation_fetch:");
  uint8_t decbuf[15];
  MemRead( &decbuf[0], insn_addr, sizeof (decbuf) );
  unisim::component::cxx::processor::intel::Mode mode( 1, 0, 1 );

  asm volatile ("operation_decode:");
  {
    uint64_t offset;
    IPage* page = GetIPage( insn_addr, offset );

    Operation*& cache_op = page->GetCached(offset, &decbuf[0], mode);

    if (not cache_op)
      {
        if (not (cache_op = this->Decode( mode, insn_addr, &decbuf[0] )))
          return 0;
        memcpy( &page->bytes[offset], &decbuf[0], cache_op->length );
      }

    latest_instruction = cache_op;
  }

  if (enable_disasm)
    {
      std::ios fmt(NULL);
      fmt.copyfmt(std::cout);
      std::cout << std::hex << insn_addr << ":\t";
      latest_instruction->disasm( std::cout );
      std::cout << " (" << unisim::component::cxx::processor::intel::DisasmBytes(&decbuf[0],latest_instruction->length) << ")\n";
      std::cout.copyfmt(fmt);
    }

  ++instruction_count;
  return latest_instruction;
}

void
Arch::xgetbv()
{
  uint32_t a, d, c = this->regread( GOd(), 1 );
  __asm__ ("xgetbv\n\t" : "=a" (a), "=d" (d) : "c" (c));
  this->regwrite( GOd(), 0, a );
  this->regwrite( GOd(), 2, d );
}

void
Arch::GPRPatch::check(Arch& arch, Tracee const& tracee) const
{
  uint64_t  reg_value = tracee.GetReg(reg);
  uint64_t& sim_value = arch.u64regs[reg];

  reg_value &= and_mask;
  reg_value |= or_mask;

  sim_value = reg_value;
  tracee.SetReg(reg, reg_value);
}

int
Arch::GPRPatch::compare(GPRPatch const& rhs) const
{
  for (unsigned shift = 0; shift < 64; shift += 16)
    {
      if (int delta = int16_t(and_mask >> shift) - int16_t(rhs.and_mask >> shift)) return delta;
      if (int delta = int16_t(or_mask >> shift) - int16_t(rhs.or_mask >> shift)) return delta;
    }
  return int(reg) - int(rhs.reg);
}

void
Arch::cpuid()
{
  uint32_t eax = this->regread( GOd(), 0 ), ecx = this->regread( GOd(), 1 );
  std::cout << "CPUID{eax:0x" << std::hex << eax << ",ecx:0x" << std::hex << ecx << "}\n";

  accurate = false;

  for (int idx = 0; idx < 4; ++idx)
    this->regwrite( GOd(), idx, eax ^ ecx );

  switch (eax)
    {
    default:
    case 0x7:
      switch(ecx)
        {
        case 0x0: {
          updates.insert(new GPRPatch(3,~uint64_t(0xdc230000),0)); // b16: AVX512F, b17: AVX512DQ. b21: AVX512_IFMA b26: AVX512PF b27: AVX512ER b28: AVX512CD b30: AVX512BW. b31: AVX512VL
          updates.insert(new GPRPatch(1,~uint64_t(0x00005841),0)); // b1: AVX512_VBMI, b6: AVX512_VBMI2, b11: AVX512_VNNI, b12: AVX512_BITALG, b14: AVX512_VPOPCNTDQ
          updates.insert(new GPRPatch(2,~uint64_t(0x0000010c),0)); // b2: AVX512_4VNNIW, b3: AVX512_4FMAPS, b8: AVX512_VP2INTERSECT
        } break;

        }
    }
}

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

void
Arch::fmemwrite80( unsigned int _seg, u64_t _addr, f80_t _val )
{
  throw 0;
  // _addr += u64_t( segbase(_seg) );
  // union IEEE754_t { double as_f; uint64_t as_u; } word;
  // word.as_f = _val;
  // uint8_t sign = (word.as_u >> 63) & 1;
  // int32_t exponent = (word.as_u >> 52) & 0x7ff;
  // uint64_t mantissa = (word.as_u << 12) >> 1;
  // if (exponent != 0x7ff) {
  //   if (exponent != 0) {
  //     mantissa |= uint64_t( 1 ) << 63;
  //   } else if (mantissa != 0) {
  //     /* normalizing */
  //     int32_t eo = __builtin_clzll( mantissa );
  //     exponent = 1 - eo;
  //     mantissa <<= eo;
  //   } else {
  //     exponent = 1023 - 16383;
  //   }
  //   exponent = exponent - 1023 + 16383;
  //   assert( (exponent >= 0) and (exponent < 0x8000) );
  // } else {
  //   mantissa |= uint64_t( 1 ) << 63;
  //   exponent = 0x7fff;
  // }

  // uintptr_t const buf_size = 10;
  // uint8_t buf[buf_size];

  // buf[9] = (sign << 7) | (uint8_t( exponent >> 8 ) & 0x7f);
  // buf[8] = uint8_t( exponent ) & 0xff;
  // for ( uintptr_t idx = 0; idx < 8; ++idx)
  //   buf[idx] = uint8_t( mantissa >> (idx*8) );

  // addr_t last_addr = _addr, addr = _addr;
  // typename Memory::Page* page = m_mem.getpage( addr );

  // for (uintptr_t idx = 0; idx < buf_size; ++idx, ++addr)
  //   {
  //     uint8_t byte = buf[idx];
  //     if ((last_addr ^ addr) >> Memory::Page::s_bits)
  //       page = m_mem.getpage( addr );
  //     addr_t offset = addr % (1 << Memory::Page::s_bits);
  //     page->m_storage[offset] = byte;
  //   }
}

void
Arch::fxam()
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

void eval_div( Arch& arch, uint64_t& hi, uint64_t& lo, uint64_t divisor )
{
  if (not divisor) arch._DE();
  UInt128 quotient( 0 ), rem( hi, lo );

  for (int pos = 64; pos and rem;)
    {
      int shift = rem.clz();
      if (shift > pos) shift = pos;
      quotient <<= shift;
      rem <<= shift;
      pos = pos - shift;

      if (rem.bits[1] < divisor and pos > 0)
        {
          quotient <<= 1;
          rem <<= 1;
          pos -= 1;
          quotient.bits[0] |= 1;
          rem.bits[1] -= divisor;
        }
      else
        {
          quotient.bits[0] |= rem.bits[1] / divisor;
          rem.bits[1] = rem.bits[1] % divisor;
        }
    }

  if (quotient.bits[1]) arch._DE();

  lo  = quotient.bits[0];
  hi = rem.bits[1];
}

void eval_div( Arch& arch, int64_t& hi, int64_t& lo, int64_t divisor )
{
  uint64_t uhi = std::abs(hi), ulo = std::abs(lo);
  int64_t sign = (hi >> 63) ^ (lo >> 63);
  eval_div( arch, uhi, ulo, uint64_t(divisor) );
  int64_t shi, slo;
  if (sign) { shi = -uhi; slo = -ulo; }
  else      { shi = +uhi; slo = +ulo; }
  if ((slo >> 63) ^ sign) arch._DE();
  hi = shi; lo = slo;
}

Arch::f80_t
Arch::fmemread80( unsigned int _seg, addr_t _addr )
{
  throw 0;
  return f80_t();
  // _addr += addr_t( segbase(_seg) );
  // uintptr_t const buf_size = 10;
  // uint8_t buf[buf_size];
  // {
  //   addr_t last_addr = _addr, addr = _addr;
  //   typename Memory::Page* page = m_mem.getpage( addr );

  //   for (uintptr_t idx = 0; idx < buf_size; ++idx, ++ addr)
  //     {
  //       if ((last_addr ^ addr) >> Memory::Page::s_bits)
  //         page = m_mem.getpage( addr );
  //       addr_t offset = addr % (1 << Memory::Page::s_bits);
  //       buf[idx] = page->m_storage[offset];
  //     }
  // }
  // uint8_t sign = (buf[9] >> 7) & 1;
  // int32_t exponent = ((uint16_t(buf[9]) << 8) & 0x7f00) | (uint16_t(buf[8]) & 0x00ff);
  // uint64_t mantissa = 0;
  // for (uintptr_t idx = 0; idx < 8; ++idx) { mantissa |= uint64_t( buf[idx] ) << (idx*8); }
  // union IEEE754_t { double as_f; uint64_t as_u; } word;

  // if (exponent != 0x7fff) {
  //   if (exponent == 0) {
  //     if (mantissa == 0) return sign ? -0.0 : 0.0;
  //     exponent += 1;
  //   }
  //   exponent -= 16383;
  //   /* normalizing */
  //   int32_t eo = __builtin_clzll( mantissa );
  //   mantissa <<= eo;
  //   exponent -= eo;
  //   if (exponent < 1024) {
  //     if (exponent < -1022) {
  //       /* denormalizing */
  //       eo = (-1022 - exponent);
  //       mantissa >>= eo;
  //       exponent = -1023;
  //     }
  //     word.as_u = ((mantissa << 1) >> 12);
  //     word.as_u |= uint64_t(exponent + 1023) << 52;
  //     word.as_u |= uint64_t(sign) << 63;
  //   } else /* exponent >= 1024 */ {
  //     /* huge number, convert to infinity */
  //     word.as_u = (uint64_t(0x7ff) << 52) | (uint64_t(sign) << 63);
  //   }
  // }

  // else /* (exponent == 0x7fff) */ {
  //   if (mantissa >> 63) {
  //     /* IEEE 754 compatible */
  //     word.as_u = ((mantissa << 1) >> 12);
  //     word.as_u |= uint64_t(0x7ff) << 52;
  //     word.as_u |= uint64_t(sign) << 63;
  //   } else {
  //     /* invalid operand ==> convert to quiet NaN (keep sign) */
  //     word.as_u = (uint64_t(0xfff) << 51) | (uint64_t(sign) << 63);
  //   }
  // }

  // return word.as_f;
}

void
Arch::fcwwrite( u16_t _value )
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

void
Arch::noexec( Operation const& op )
{
  std::cerr
    << "error: no execute method in `" << typeid(op).name() << "'\n"
    << std::hex << op.address << ":\t";
  op.disasm( std::cerr );
  std::cerr << '\n';
  throw Unimplemented();
}

void
Arch::ScanRegisters(unisim::service::interfaces::RegisterScanner& scanner)
{
  // Program counter
  scanner.Append( GetRegister( "rip" ) );
  // General purpose registers
  // scanner.Append( GetRegister( "eax" ) );
  // scanner.Append( GetRegister( "ecx" ) );
  // scanner.Append( GetRegister( "edx" ) );
  // scanner.Append( GetRegister( "ebx" ) );
  // scanner.Append( GetRegister( "esp" ) );
  // scanner.Append( GetRegister( "ebp" ) );
  // scanner.Append( GetRegister( "esi" ) );
  // scanner.Append( GetRegister( "edi" ) );
  // Segments
  // scanner.Append( GetRegister( "es" ) );
  // scanner.Append( GetRegister( "cs" ) );
  // scanner.Append( GetRegister( "ss" ) );
  // scanner.Append( GetRegister( "ds" ) );
  // scanner.Append( GetRegister( "fs" ) );
  // scanner.Append( GetRegister( "gs" ) );
  // FP registers
  // scanner.Append( GetRegister( "st0" ) );
  // scanner.Append( GetRegister( "st1" ) );
  // scanner.Append( GetRegister( "st2" ) );
  // scanner.Append( GetRegister( "st3" ) );
  // scanner.Append( GetRegister( "st4" ) );
  // scanner.Append( GetRegister( "st5" ) );
  // scanner.Append( GetRegister( "st6" ) );
  // scanner.Append( GetRegister( "st7" ) );
}


// /** Disasm an instruction address.
//  * Returns a string with the disassembling of the instruction found
//  *   at address addr.
//  *
//  * @param addr the address of the instruction to disassemble
//  * @param next_addr the address following the requested instruction
//  *
//  * @return the disassembling of the requested instruction address
//  */
// std::string
// Arch::Disasm(uint64_t addr, uint64_t& next_addr)
// {
//   uint8_t decbuf[15];
//   MemRead( &decbuf[0], addr, sizeof (decbuf) );
//   unisim::component::cxx::processor::intel::Mode mode( 1, 0, 1 );

//   struct
//   {
//     uint64_t extract(Operation* op, uint64_t addr)
//     {
//       op->disasm( buf );
//       return op->length;
//     }
//     std::ostringstream buf;
//   } _;

//   uint64_t offset;
//   IPage* page = GetIPage( addr, offset );

//   if (Operation* cache_op = page->GetCached(offset, &decbuf[0], mode))
//     {
//       next_addr = addr + _.extract(cache_op, addr);
//     }
//   else if (Operation* new_op = this->Decode( mode, addr, &decbuf[0] ))
//     {
//       next_addr = addr + _.extract(new_op, addr);
//       delete new_op;
//     }
//   else
//     {
//       next_addr = addr;
//       _.buf << "<bad>";
//     }
//   return _.buf.str();
// }

unisim::service::interfaces::Register*
Arch::GetRegister(char const* name)
{
  auto reg = regmap.find( name );
  return (reg == regmap.end()) ? 0 : *reg;
}

bool
Arch::InjectReadMemory(addr_t addr, void *buffer, uint32_t size)
{
  MemRead( (uint8_t*)buffer, addr, size );
  return true;
}

bool
Arch::InjectWriteMemory(addr_t addr, void const* buffer, uint32_t size)
{
  std::cerr << "Tracee should not be written by tracer\n";
  throw 0;
  return true;
}

void
Arch::ResetMemory()
{
}

bool
Arch::ReadMemory(addr_t addr, void* buffer, uint32_t size )
{
  MemRead( (uint8_t*)buffer, addr, size );
  return true;
}

bool
Arch::WriteMemory(addr_t addr, void const* buffer, uint32_t size)
{
  std::cerr << "Tracee should not be written by tracer\n";
  throw 0;
  return true;
}

void
Arch::MemRead( uint8_t* bytes, uint64_t addr, unsigned size ) const
{
  struct { uint8_t* dst; void Do(uint64_t i, uint8_t src) { dst[i] = src; } } read {bytes};
  tracee.MemAccess(read, addr, size);
}

int
Arch::Update::compare(Update const& rhs) const
{
  // Do not compare null expressions
  if (not rhs.node) return     node ?  1 : 0;
  if (not     node) return rhs.node ? -1 : 0;

  /* First compare actual types */
  const std::type_info* til = &typeid(*node);
  const std::type_info* tir = &typeid(*rhs.node);
  if (til < tir) return -1;
  if (til > tir) return +1;

  /* Same types, call derived comparator */
  return node->cmp( *rhs.node );
}

void
Arch::MCUpdate::memcheck(Tracee const& tracee, u64_t addr, uint8_t const* bytes, unsigned size) const
{
  struct { uint8_t const* ref; void Do(uint64_t i, uint8_t src) { if (ref[i] != src) throw 0; } } cmp {bytes};
  tracee.MemAccess(cmp, addr, size);
}

void
Arch::vmm_regcheck(unsigned idx)
{
  struct VRCUpdate : public UpdateNode
  {
    VRCUpdate(unsigned _reg, bool _test)  : reg(_reg), test(_test) {} unsigned reg; bool test;
    virtual void check(Arch& arch, Tracee const& tracee) const override
    {
      uint8_t ref_bytes[VMMRegister::BYTECOUNT];
      tracee.GetVec(reg, &ref_bytes[0]);

      if (not test)
        {
          for (unsigned idx = 0, end = VMMRegister::BYTECOUNT; idx < end; ++idx)
            arch.vmm_write(VMMRegister::VR(), reg, idx, ref_bytes[idx]);
          return;
        }

      uint8_t sim_bytes[VMMRegister::BYTECOUNT];
      VMMRegister vreg(arch, reg);
      vreg.GetValue(&sim_bytes[0]);
      for (unsigned idx = 0, end = VMMRegister::BYTECOUNT; idx < end; ++idx)
        if (sim_bytes[idx] != ref_bytes[idx])
          throw 0;

      // if (sim_value != ref_value)
      //   {
      //     std::cerr << unisim::component::cxx::processor::intel::DisasmG(unisim::component::cxx::processor::intel::GOq(), reg) << ": 0x" << std::hex << sim_value << " != 0x" << ref_value << "\n";
      //     throw 0;
      //   }
    }
    virtual int cmp(UpdateNode const& rhs) const override { return compare(dynamic_cast<VRCUpdate const&>(rhs)); }
    virtual int compare(VRCUpdate const& rhs) const
    {
      if (int delta = int(reg) - int(rhs.reg))
        return delta;
      return int(test) - int(rhs.test);
    }
  };

  updates.insert(new VRCUpdate(idx, accurate));
}

void
Arch::regcheck(unsigned reg)
{
  struct RCUpdate : public UpdateNode
  {
    RCUpdate(unsigned _reg, bool _test)  : reg(_reg), test(_test) {} unsigned reg; bool test;
    virtual void check(Arch& arch, Tracee const& tracee) const override
    {
      uint64_t  ref_value = tracee.GetReg(reg);
      uint64_t& sim_value = arch.u64regs[reg];

      if (not test)
        {
          sim_value = ref_value;
          return;
        }
      if (sim_value != ref_value)
        {
          std::cerr << unisim::component::cxx::processor::intel::DisasmG(unisim::component::cxx::processor::intel::GOq(), reg) << ": 0x" << std::hex << sim_value << " != 0x" << ref_value << "\n";
          throw 0;
        }
    }
    virtual int cmp(UpdateNode const& rhs) const override { return compare(dynamic_cast<RCUpdate const&>(rhs)); }
    virtual int compare(RCUpdate const& rhs) const
    {
      if (int delta = int(reg) - int(rhs.reg))
        return delta;
      return int(test) - int(rhs.test);
    }
  };

  updates.insert(new RCUpdate(reg, accurate));
}

void
Arch::ExecuteSystemCall( unsigned id )
{
  linux_os.LogSystemCall( id );
  accurate = false;
  regcheck(1); // rcx
  regcheck(11); // r11
  struct { void Do( Arch& a, typename FLAG::Code flag, unsigned bit ) { a.flagcheck( flag ); } } flags_affected;
  unisim::component::cxx::processor::intel::eflagsaccess( *this, flags_affected );
  linux_os.ExecuteSystemCall(id);
  // regwrite( GR(), 0, 0 );
}

void
Arch::sbasecheck( char seg )
{
  struct SBUpdate : public UpdateNode
  {
    SBUpdate(char _reg, bool _test)  : reg(_reg), test(_test) {} char reg; bool test;
    virtual void check(Arch& arch, Tracee const& tracee) const override
    {
      uint64_t  ref_sbase = tracee.GetSBase(reg);
      uint64_t& sim_sbase = arch.sbase(reg);

      if (not test)
        {
          sim_sbase = ref_sbase;
          return;
        }
      if (sim_sbase != ref_sbase)
        {
          std::cerr << unisim::component::cxx::processor::intel::DisasmG(unisim::component::cxx::processor::intel::GOq(), reg) << ": 0x" << std::hex << sim_sbase << " != 0x" << ref_sbase << "\n";
          throw 0;
        }
    }
    virtual int cmp(UpdateNode const& rhs) const override { return compare(dynamic_cast<SBUpdate const&>(rhs)); }
    virtual int compare(SBUpdate const& rhs) const
    {
      if (int delta = int(reg) - int(rhs.reg))
        return delta;
      return int(test) - int(rhs.test);
    }
  };

  updates.insert(new SBUpdate(seg, accurate));
}

int Arch::RegisterMapOrdering::cmp(char const* a, char const* b) const { return strcmp(a, b); }

void
Arch::xsave(unisim::component::cxx::processor::intel::XSaveMode mode, bool is64, u64_t u64, RMOp const& rmop)
{
  accurate = false;
}

void
Arch::xrstor(unisim::component::cxx::processor::intel::XSaveMode mode, bool is64, u64_t u64, RMOp const& rmop)
{
  accurate = false;

  for (int reg = 0; reg < 16; ++reg)
    {
      regcheck(reg);
      vmm_regcheck(reg);
    }

  for (char const* reg = "fg"; *reg; ++reg)
    sbasecheck(*reg);

  flagcheck( FLAG() );
}

void
Arch::flagcheck(FLAG flag)
{
  if (flag.code == flag.PF or flag.code == flag.AF)
    return;

  struct EFUpdate : public UpdateNode
  {
    EFUpdate() {}
    virtual void check(Arch& arch, Tracee const& tracee) const override
    {
      uint64_t ref_bits = tracee.GetRFlags();

      if (not arch.accurate)
        {
          unisim::component::cxx::processor::intel::eflagswrite(arch, ref_bits);
          return;
        }

      struct { void Do( Arch& a, typename FLAG::Code flag, unsigned bit ) { bits |= u32_t(a.flagread( flag )) << bit; mask |= uint32_t(1) << bit; } u32_t bits, mask; } sim {0,0};
      unisim::component::cxx::processor::intel::eflagsaccess( arch, sim );

      // Not considering AF and PF flags for now
      arch.flagwrite(Arch::FLAG::AF, false, false);
      arch.flagwrite(Arch::FLAG::PF, false, false);
      uint32_t flagmask = sim.mask & ~arch.flagmask;

      uint32_t diff = (sim.bits ^ ref_bits) & flagmask;
      if (diff)
        {
          if (diff == 0x40)
            {
              std::ostringstream buf;
              arch.latest_instruction->disasm(buf);
              if (buf.str().compare(0, 11, "repz cmpsb ", 11) == 0)
                {
                  std::cerr << "Warning, ignoring ZF error in " << buf.str() << std::endl;
                  return;
                }
            }
          std::cerr << "eflags: 0x" << std::hex << (sim.bits & flagmask) << " != 0x" << (ref_bits & flagmask) << " (sim=0x" << sim.bits << ", ref=0x" << ref_bits << ")\n";
          throw 0;
        }
    }
    virtual int cmp(UpdateNode const& rhs) const override { return compare(dynamic_cast<EFUpdate const&>(rhs)); }
    virtual int compare(EFUpdate const& rhs) const { return 0; }
  };

  updates.insert(new EFUpdate());
}

void
Arch::flagwrite( FLAG flag, bit_t fval, bit_t defined )
{
  if (defined)
    return flagwrite( flag, fval );

  struct { void Do( Arch& arch, FLAG flag, unsigned bit) { if (uflag == flag) arch.flagmask |= uint32_t(1) << bit; } FLAG uflag; } access {flag};
  unisim::component::cxx::processor::intel::eflagsaccess(*this, access);
}
