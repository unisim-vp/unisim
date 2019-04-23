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

#ifndef ARCH_HH
#define ARCH_HH

#include <unisim/component/cxx/processor/intel/isa/intel.hh>
#include <unisim/component/cxx/processor/intel/vectorbank.hh>
#include <unisim/component/cxx/processor/intel/modrm.hh>
#include <unisim/component/cxx/processor/intel/types.hh>
#include <unisim/util/symbolic/symbolic.hh>
#include <vector>
#include <set>
#include <memory>
#include <inttypes.h>

namespace ut
{
  struct Arch;
  
  typedef unisim::component::cxx::processor::intel::Operation<ut::Arch> Operation;

  using unisim::util::symbolic::SmartValue;

  struct UInt128 {};
  struct SInt128 {};


  template <typename T> using VectorTypeInfo = unisim::component::cxx::processor::intel::VectorTypeInfo<T>;
  template <typename A, unsigned S> using TypeFor = typename unisim::component::cxx::processor::intel::TypeFor<A,S>;
  
  struct Untestable
  {
    Untestable(std::string const& _reason) : reason(_reason) {}
    std::string reason;
  };

  struct ActionNode : public unisim::util::symbolic::Choice<ActionNode>  {};

  struct Interface
  {
    Interface( Operation& op );

    int  cmp( Interface const& ) const;
    bool operator < ( Interface const& b ) const { return cmp( b ) < 0; }
    std::shared_ptr<ActionNode> action;
  };

  typedef unisim::util::symbolic::ScalarType ScalarType;
  
  struct Arch
  {
    typedef SmartValue<uint8_t>     u8_t;
    typedef SmartValue<uint16_t>    u16_t;
    typedef SmartValue<uint32_t>    u32_t;
    typedef SmartValue<uint64_t>    u64_t;
    typedef SmartValue<UInt128>     u128_t;
    typedef SmartValue<int8_t>      s8_t;
    typedef SmartValue<int16_t>     s16_t;
    typedef SmartValue<int32_t>     s32_t;
    typedef SmartValue<int64_t>     s64_t;
    typedef SmartValue<SInt128>     s128_t;
    typedef SmartValue<bool>        bit_t;

    typedef u64_t addr_t;
  
    typedef SmartValue<float>       f32_t;
    typedef SmartValue<double>      f64_t;
    typedef SmartValue<long double> f80_t;

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
  
    struct OpHeader
    {
      OpHeader( uint64_t _address ) : address( _address ) {} uint64_t address;
    };

    static Operation* fetch( uint64_t _address, uint8_t const* bytes );

    void noexec( Operation const& op )
    {
      throw Untestable("Not implemented");
    }

    typedef unisim::component::cxx::processor::intel::RMOp<Arch> RMOp;
    
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

    template <class GOP>
    typename TypeFor<Arch,GOP::SIZE>::u
    rmread( GOP const& g, RMOp const& rmop )
    {
      if (not rmop.is_memory_operand())
        return regread( g, rmop.ereg() );

      return memread<GOP::SIZE>( rmop->segment, rmop->effective_address( *this ) );
    }
    
// {

//   struct OpHeader
//   {
//     OpHeader( addr_t _address ) : address( _address ) {} addr_t address;
//   };

//   Arch()
//     : unisim::service::interfaces::MemoryInjection<uint64_t>()
//     , unisim::service::interfaces::Memory<uint64_t>()
//     , unisim::service::interfaces::Registers()
//     , linux_os(0)
//     , m_mem()
//     , segregs(), fs_base(), gs_base()
//     , rip(), u64regs(), m_flags()
//     , m_fregs(), m_ftop(0), m_fcw(0x37f)
//     , umms(), vmm_storage(), mxcsr(0x1fa0)
//     , latest_instruction(0), do_disasm(false)
//     , instruction_count(0)
//       //    , gdbchecker()
//   {
//     regmap["%rip"] = new unisim::util::debug::SimpleRegister<uint64_t>("%rip", &this->rip);
//     regmap["%fs_base"] = new unisim::util::debug::SimpleRegister<uint64_t>("%fs_base", &this->fs_base);
//     regmap["%gs_base"] = new unisim::util::debug::SimpleRegister<uint64_t>("%gs_base", &this->fs_base);
//     regmap["%fcw"] = new unisim::util::debug::SimpleRegister<uint16_t>("%fcw", &this->m_fcw);

//     struct EFlagsRegister  : public unisim::service::interfaces::Register
//     {
//       uint32_t eflagsread() const { return unisim::component::cxx::processor::intel::eflagsread( cpu ); }
//       EFlagsRegister(Arch& _cpu) : cpu(_cpu) {}
//       Arch& cpu;
//       char const* GetName() const override { return "%eflags"; }
//       void GetValue(void* buffer) const override { *((uint32_t*)buffer) = eflagsread(); }
//       void SetValue(void const* buffer) override
//       {
//         uint32_t bits = *((uint32_t*)buffer);
//         struct { unsigned bit; FLAG::Code flag; }
//         ffs[] = {{0, FLAG::CF}, {2, FLAG::PF}, {4, FLAG::AF}, {6, FLAG::ZF}, {7, FLAG::SF}, {10, FLAG::DF}, {11, FLAG::OF}};
//         for (unsigned idx = 0, end = sizeof(ffs)/sizeof(ffs[0]); idx < end; ++idx)
//           cpu.flagwrite( ffs[idx].flag, bit_t((bits >> ffs[idx].bit) & 1u) );
//         uint32_t chk = eflagsread();
//         if (bits != chk)
//           std::cerr << "Warning: imported eflags " << std::hex << bits << " ended with " << chk << std::endl;
//       }
//       int GetSize() const override { return 32; }
//     };

//     {
//       EFlagsRegister* reg = new EFlagsRegister(*this);
//       regmap[reg->GetName()] = reg;
//     }
    
//     struct XmmRegister : public unisim::service::interfaces::Register
//     {
//       XmmRegister(std::string const& _name, Arch& _cpu, unsigned _reg) : name(_name), cpu(_cpu), reg(_reg) {}
//       std::string name;
//       Arch& cpu;
//       unsigned reg;
//       char const* GetName() const override { return name.c_str(); }
//       void GetValue(void *buffer) const override
//       {
//         uint8_t* buf = reinterpret_cast<uint8_t*>(buffer);
//         std::fill(&buf[0],&buf[16],0);
//         auto const& ureg = cpu.umms[reg];
//         ureg.transfer( &buf[0], &cpu.vmm_storage[reg][0], std::min(ureg.size,16u), false );
//       }
//       void SetValue(const void *buffer) override
//       {
//         uint8_t* elems = cpu.umms[reg].GetStorage( &cpu.vmm_storage[reg][0], uint8_t(), VUnion::BYTECOUNT );
//         memcpy(elems,buffer,16);
//       }
//       int GetSize() const override { return 16; }
//     };
    
//     for (int idx = 0; idx < 16; ++idx)
//       {
//         std::ostringstream regname;
//         using unisim::component::cxx::processor::intel::DisasmV;
//         typedef unisim::component::cxx::processor::intel::XMM XMM;
//         regname << DisasmV( XMM(), idx );
//         regmap[regname.str()] = new XmmRegister(regname.str(), *this, idx);
//       }
    
//     for (int idx = 0; idx < 16; ++idx)
//       {
//         std::ostringstream regname;
//         regname << unisim::component::cxx::processor::intel::DisasmG(GOq(), idx);
//         regmap[regname.str()] = new unisim::util::debug::SimpleRegister<uint64_t>(regname.str(), &u64regs[idx]);
//       }
    
//     //   struct SegmentRegister : public unisim::service::interfaces::Register
//     //   {
//     //     SegmentRegister( Arch& _arch, std::string _name, unsigned _idx ) : arch(_arch), name(_name), idx(_idx) {}
//     //     virtual const char *GetName() const { return "pc"; }
//     //     virtual void GetValue( void* buffer ) const { *((uint16_t*)buffer) = arch.segregread( idx ); }
//     //     virtual void SetValue( void const* buffer ) { arch.segregwrite( idx, *((uint16_t*)buffer) ); }
//     //     virtual int  GetSize() const { return 2; }
//     //     virtual void Clear() {} // Clear is meaningless for segment registers
//     //     Arch&        arch;
//     //     std::string  name;
//     //     unsigned     idx;
//     //   };

//     //   for (int idx = 0; idx < 6; ++idx) {
//     //     std::ostringstream regname;
//     //     regname << unisim::component::cxx::processor::intel::DisasmS(idx);
//     //     regmap[regname.str()] = new SegmentRegister(*this, regname.str(), idx);
//     //   }
    
//     //   for (int idx = 0; idx < 4; ++idx) {
//     //     std::ostringstream regname;
//     //     regname << "@gdt[" << idx<< "].base";
//     //     regmap[regname.str()] = new unisim::util::debug::SimpleRegister<uint64_t>(regname.str(), &m_gdt_bases[idx]);
//     //   }
    
//     //   struct X87Register : public unisim::service::interfaces::Register
//     //   {
//     //     X87Register( std::string _name, Arch& _arch, unsigned _idx ) : name(_name), arch(_arch), idx(_idx) {}
//     //     char const* GetName() const { return name.c_str(); }
//     //     void GetValue(void *buffer) const { *((f64_t*)buffer) = arch.fread( idx ); }
//     //     void SetValue(const void *buffer) { arch.fwrite( idx, *((f64_t*)buffer) ); }
//     //     int GetSize() const { return 8; }
//     //     std::string name;
//     //     Arch& arch;
//     //     unsigned idx;
//     //   };
//     //   for (int idx = 0; idx < 8; ++idx) {
//     //     std::ostringstream regname;
//     //     regname << unisim::component::cxx::processor::intel::DisasmFPR(idx);
//     //     regmap[regname.str()] = new X87Register( regname.str(), *this, idx );
//     //   }
//   }
//   ~Arch()
//   {
//     for (auto reg : regmap)
//       delete reg.second;
//   }
  
//   void SetLinuxOS( unisim::service::interfaces::LinuxOS* _linux_os ) { linux_os = _linux_os; }
//   // unisim::service::interfaces::LinuxOS* GetLinuxOS() { return linux_os; }
  
//   unisim::service::interfaces::LinuxOS* linux_os;
//   std::map<std::string,unisim::service::interfaces::Register*> regmap;
  
//   struct ClearMemSet { void operator() ( uint8_t* base, uintptr_t size ) const { __builtin_bzero(base, size); } };
//   typedef typename unisim::component::cxx::memory::sparse::Memory<addr_t,15,15,ClearMemSet> Memory;
//   Memory                      m_mem;

//   // unisim::service::interfaces::Memory<addr_t>
//   void Reset() {}
//   bool ReadMemory(addr_t addr, void* buffer, uint32_t size ) { m_mem.read( (uint8_t*)buffer, addr, size ); return true; }
//   bool WriteMemory(addr_t addr, void const* buffer, uint32_t size) { m_mem.write( addr, (uint8_t*)buffer, size ); return true; }

//   typedef unisim::component::cxx::processor::intel::RMOp<Arch> RMOp;
  
//   // MEMORY STATE
//   // Segment Registers
//   uint16_t segregs[6];
//   addr_t fs_base, gs_base;
//   addr_t segbase(unsigned seg) const { switch(seg){ case 4: return fs_base; case 5: return gs_base; } return 0; }
//   void                        segregwrite( unsigned idx, uint16_t value )
//   {
//     if (idx > 6) throw 0;
//     segregs[idx] = value;
//   }
//   uint16_t                    segregread( unsigned idx )
//   {
//     throw 0;
//     if (idx > 6) throw 0;
//     return segregs[idx];
//   }

//   // Low level memory access routines
//   enum LLAException_t { LLAError };
//   void                        lla_memcpy( addr_t addr, uint8_t const* buf, addr_t size )
//   { m_mem.write( addr, buf, size ); }
//   void                        lla_memcpy( uint8_t* buf, addr_t addr, addr_t size )
//   { m_mem.read( buf, addr, size ); }
//   void                        lla_bzero( addr_t addr, addr_t size )
//   { m_mem.clear( addr, size ); }
//   template<class INT_t>
//   void                        lla_memwrite( addr_t _addr, INT_t _val )
//   {
//     uintptr_t const int_size = sizeof( INT_t );
      
//     addr_t last_addr = _addr, addr = _addr;
//     typename Memory::Page* page = m_mem.getpage( addr );
      
//     for (uintptr_t idx = 0; idx < int_size; ++idx, ++addr)
//       {
//         uint8_t byte = (_val >> (idx*8)) & 0xff;
//         if ((last_addr ^ addr) >> Memory::Page::s_bits)
//           page = m_mem.getpage( addr );
//         addr_t offset = addr % (1 << Memory::Page::s_bits);
//         page->m_storage[offset] = byte;
//       }
//   }
    
//   template<class INT_t>
//   INT_t                       lla_memread( addr_t _addr )
//   {
//     uintptr_t const int_size = sizeof( INT_t );
      
//     addr_t last_addr = _addr, addr = _addr;
//     typename Memory::Page* page = m_mem.getpage( addr );
      
//     INT_t result = 0;
      
//     for (uintptr_t idx = 0; idx < int_size; ++idx, ++ addr)
//       {
//         if ((last_addr ^ addr) >> Memory::Page::s_bits)
//           page = m_mem.getpage( addr );
//         addr_t offset = addr % (1 << Memory::Page::s_bits);
//         result |= (INT_t( page->m_storage[offset] ) << (idx*8));
//       }
      
//     return result;

//   }
  
//   f32_t                       fmemread32( unsigned int _seg, addr_t _addr )
//   {
//     union IEEE754_t { float as_f; uint32_t as_u; } word;
//     word.as_u = memread<32>( _seg, _addr );
//     return word.as_f;
//   }
//   f64_t                       fmemread64( unsigned int _seg, addr_t _addr )
//   {
//     union IEEE754_t { double as_f; uint64_t as_u; } word;
//     word.as_u = memread<64>( _seg, _addr );
//     return word.as_f;
//   }
//   f80_t
//   fmemread80( unsigned int _seg, addr_t _addr )
//   {
//     _addr += addr_t( segbase(_seg) );
//     uintptr_t const buf_size = 10;
//     uint8_t buf[buf_size];
//     {
//       addr_t last_addr = _addr, addr = _addr;
//       typename Memory::Page* page = m_mem.getpage( addr );
      
//       for (uintptr_t idx = 0; idx < buf_size; ++idx, ++ addr)
//         {
//           if ((last_addr ^ addr) >> Memory::Page::s_bits)
//             page = m_mem.getpage( addr );
//           addr_t offset = addr % (1 << Memory::Page::s_bits);
//           buf[idx] = page->m_storage[offset];
//         }
//     }
//     uint8_t sign = (buf[9] >> 7) & 1;
//     int32_t exponent = ((uint16_t(buf[9]) << 8) & 0x7f00) | (uint16_t(buf[8]) & 0x00ff);
//     uint64_t mantissa = 0;
//     for (uintptr_t idx = 0; idx < 8; ++idx) { mantissa |= uint64_t( buf[idx] ) << (idx*8); }
//     union IEEE754_t { double as_f; uint64_t as_u; } word;
    
//     if (exponent != 0x7fff) {
//       if (exponent == 0) {
//         if (mantissa == 0) return sign ? -0.0 : 0.0;
//         exponent += 1;
//       }
//       exponent -= 16383;
//       /* normalizing */
//       int32_t eo = __builtin_clzll( mantissa );
//       mantissa <<= eo;
//       exponent -= eo;
//       if (exponent < 1024) {
//         if (exponent < -1022) {
//           /* denormalizing */
//           eo = (-1022 - exponent);
//           mantissa >>= eo;
//           exponent = -1023;
//         }
//         word.as_u = ((mantissa << 1) >> 12);
//         word.as_u |= uint64_t(exponent + 1023) << 52;
//         word.as_u |= uint64_t(sign) << 63;
//       } else /* exponent >= 1024 */ {
//         /* huge number, convert to infinity */
//         word.as_u = (uint64_t(0x7ff) << 52) | (uint64_t(sign) << 63);
//       }
//     }
    
//     else /* (exponent == 0x7fff) */ {
//       if (mantissa >> 63) {
//         /* IEEE 754 compatible */
//         word.as_u = ((mantissa << 1) >> 12);
//         word.as_u |= uint64_t(0x7ff) << 52;
//         word.as_u |= uint64_t(sign) << 63;
//       } else {
//         /* invalid operand ==> convert to quiet NaN (keep sign) */
//         word.as_u = (uint64_t(0xfff) << 51) | (uint64_t(sign) << 63);
//       }
//     }
    
//     return word.as_f;
//   }

//   template <unsigned OPSIZE>
//   typename TypeFor<Arch,OPSIZE>::f
//   fpmemread( unsigned seg, u64_t addr )
//   {
//     if (OPSIZE==32) return this->fmemread32( seg, addr );
//     if (OPSIZE==64) return this->fmemread64( seg, addr );
//     if (OPSIZE==80) return this->fmemread80( seg, addr );
//     throw 0;        return typename TypeFor<Arch,OPSIZE>::f();
//   }

//   template <unsigned OPSIZE>
//   typename TypeFor<Arch,OPSIZE>::f
//   frmread( RMOp const& rmop )
//   {
//     typedef typename TypeFor<Arch,OPSIZE>::f f_type;
//     if (not rmop.is_memory_operand()) return f_type( this->fread( rmop.ereg() ) );
//     return this->fpmemread<OPSIZE>( rmop->segment, rmop->effective_address( *this ) );
//   }

//   void                        fmemwrite32( unsigned int _seg, u64_t _addr, f32_t _val )
//   {
//     union IEEE754_t { float as_f; uint32_t as_u; } word;
//     word.as_f = _val;
//     memwrite<32>( _seg, _addr, word.as_u );
//   }
//   void                        fmemwrite64( unsigned int _seg, u64_t _addr, f64_t _val )
//   {
//     union IEEE754_t { double as_f; uint64_t as_u; } word;
//     word.as_f = _val;
//     memwrite<64>( _seg, _addr, word.as_u );
//   }
//   void                        fmemwrite80( unsigned int _seg, u64_t _addr, f80_t _val )
//   {
//     _addr += u64_t( segbase(_seg) );
//     union IEEE754_t { double as_f; uint64_t as_u; } word;
//     word.as_f = _val;
//     uint8_t sign = (word.as_u >> 63) & 1;
//     int32_t exponent = (word.as_u >> 52) & 0x7ff;
//     uint64_t mantissa = (word.as_u << 12) >> 1;
//     if (exponent != 0x7ff) {
//       if (exponent != 0) {
//         mantissa |= uint64_t( 1 ) << 63;
//       } else if (mantissa != 0) {
//         /* normalizing */
//         int32_t eo = __builtin_clzll( mantissa );
//         exponent = 1 - eo;
//         mantissa <<= eo;
//       } else {
//         exponent = 1023 - 16383;
//       }
//       exponent = exponent - 1023 + 16383;
//       assert( (exponent >= 0) and (exponent < 0x8000) );
//     } else {
//       mantissa |= uint64_t( 1 ) << 63;
//       exponent = 0x7fff;
//     }

//     uintptr_t const buf_size = 10;
//     uint8_t buf[buf_size];

//     buf[9] = (sign << 7) | (uint8_t( exponent >> 8 ) & 0x7f);
//     buf[8] = uint8_t( exponent ) & 0xff;
//     for ( uintptr_t idx = 0; idx < 8; ++idx)
//       buf[idx] = uint8_t( mantissa >> (idx*8) );

//     addr_t last_addr = _addr, addr = _addr;
//     typename Memory::Page* page = m_mem.getpage( addr );

//     for (uintptr_t idx = 0; idx < buf_size; ++idx, ++addr)
//       {
//         uint8_t byte = buf[idx];
//         if ((last_addr ^ addr) >> Memory::Page::s_bits)
//           page = m_mem.getpage( addr );
//         addr_t offset = addr % (1 << Memory::Page::s_bits);
//         page->m_storage[offset] = byte;
//       }
//   }

//   template <unsigned OPSIZE>
//   void
//   fpmemwrite( unsigned seg, u64_t addr, typename TypeFor<Arch,OPSIZE>::f value )
//   {
//     if (OPSIZE==32) return fmemwrite32( seg, addr, value );
//     if (OPSIZE==64) return fmemwrite64( seg, addr, value );
//     if (OPSIZE==80) return fmemwrite80( seg, addr, value );
//     throw 0;
//   }
  
//   template <unsigned OPSIZE>
//   void
//   frmwrite( RMOp const& rmop, typename TypeFor<Arch,OPSIZE>::f value )
//   {
//     if (not rmop.is_memory_operand()) return fwrite( rmop.ereg(), f64_t( value ) );
//     fpmemwrite<OPSIZE>( rmop->segment, rmop->effective_address( *this ), value );
//   }


//   template <unsigned OPSIZE>
//   void
//   memwrite( unsigned _seg, u64_t _addr, typename TypeFor<Arch,OPSIZE>::u _val )
//   {
//     uintptr_t const int_size = (OPSIZE/8);
//     addr_t addr = _addr + segbase(_seg), last_addr = addr;

//     typename Memory::Page* page = m_mem.getpage( addr );

//     for (uintptr_t idx = 0; idx < int_size; ++idx, ++addr)
//       {
//         uint8_t byte = (_val >> (idx*8)) & 0xff;
//         if ((last_addr ^ addr) >> Memory::Page::s_bits)
//           page = m_mem.getpage( addr );
//         addr_t offset = addr % (1 << Memory::Page::s_bits);
//         page->m_storage[offset] = byte;
//       }
//   }

//   template <unsigned OPSIZE>
//   typename TypeFor<Arch,OPSIZE>::u
//   pop()
//   {
//     // TODO: handle stack address size
//     u64_t sptr = regread( GOq(), 4 );
//     regwrite( GOq(), 4, sptr + u64_t( OPSIZE/8 ) );
//     return memread<OPSIZE>( unisim::component::cxx::processor::intel::SS, sptr );
//   }

//   void shrink_stack( addr_t offset ) { regwrite( GOq(), 4, regread( GOq(), 4 ) + offset ); }
//   void grow_stack( addr_t offset ) { regwrite( GOq(), 4, regread( GOq(), 4 ) - offset ); }

//   template <unsigned OPSIZE>
//   void
//   push( typename TypeFor<Arch,OPSIZE>::u value )
//   {
//     // TODO: handle stack address size
//     u64_t sptr = regread( GOq(), 4 ) - u64_t( OPSIZE/8 );
//     memwrite<OPSIZE>( unisim::component::cxx::processor::intel::SS, sptr, value );
//     regwrite( GOq(), 4, sptr );
//   }


//   template <class GOP>
//   typename TypeFor<Arch,GOP::SIZE>::u
//   rmread( GOP const& g, RMOp const& rmop )
//   {
//     if (not rmop.is_memory_operand())
//       return regread( g, rmop.ereg() );

//     return memread<GOP::SIZE>( rmop->segment, rmop->effective_address( *this ) );
//   }

//   template <class GOP>
//   void
//   rmwrite( GOP const&, RMOp const& rmop, typename TypeFor<Arch,GOP::SIZE>::u value )
//   {
//     if (not rmop.is_memory_operand())
//       return regwrite( GOP(), rmop.ereg(), value );

//     return memwrite<GOP::SIZE>( rmop->segment, rmop->effective_address( *this ), value );
//   }

//   template <unsigned OPSIZE>
//   typename TypeFor<Arch,OPSIZE>::u
//   memread( unsigned _seg, u64_t _addr )
//   {
//     uintptr_t const int_size = (OPSIZE/8);
//     addr_t addr = _addr + segbase(_seg), last_addr = addr;
//     typename Memory::Page* page = m_mem.getpage( addr );

//     typedef typename TypeFor<Arch,OPSIZE>::u u_type;
//     u_type result = 0;

//     for (uintptr_t idx = 0; idx < int_size; ++idx, ++ addr)
//       {
//         if ((last_addr ^ addr) >> Memory::Page::s_bits)
//           page = m_mem.getpage( addr );
//         addr_t offset = addr % (1 << Memory::Page::s_bits);
//         result |= (u_type( page->m_storage[offset] ) << (idx*8));
//       }

//     return result;
//   }
  
  
//   // unisim::service::interfaces::Registers
//   unisim::service::interfaces::Register* GetRegister(char const* name)
//   {
//     auto reg = regmap.find( name );
//     return (reg == regmap.end()) ? 0 : reg->second;
//   }
//   void ScanRegisters(unisim::service::interfaces::RegisterScanner& scanner)
//   {
//     // Program counter
//     scanner.Append( GetRegister( "%rip" ) );
//     // General purpose registers
//     // scanner.Append( GetRegister( "%eax" ) );
//     // scanner.Append( GetRegister( "%ecx" ) );
//     // scanner.Append( GetRegister( "%edx" ) );
//     // scanner.Append( GetRegister( "%ebx" ) );
//     // scanner.Append( GetRegister( "%esp" ) );
//     // scanner.Append( GetRegister( "%ebp" ) );
//     // scanner.Append( GetRegister( "%esi" ) );
//     // scanner.Append( GetRegister( "%edi" ) );
//     // Segments
//     // scanner.Append( GetRegister( "%es" ) );
//     // scanner.Append( GetRegister( "%cs" ) );
//     // scanner.Append( GetRegister( "%ss" ) );
//     // scanner.Append( GetRegister( "%ds" ) );
//     // scanner.Append( GetRegister( "%fs" ) );
//     // scanner.Append( GetRegister( "%gs" ) );
//     // FP registers
//     // scanner.Append( GetRegister( "%st" ) );
//     // scanner.Append( GetRegister( "%st(1)" ) );
//     // scanner.Append( GetRegister( "%st(2)" ) );
//     // scanner.Append( GetRegister( "%st(3)" ) );
//     // scanner.Append( GetRegister( "%st(4)" ) );
//     // scanner.Append( GetRegister( "%st(5)" ) );
//     // scanner.Append( GetRegister( "%st(6)" ) );
//     // scanner.Append( GetRegister( "%st(7)" ) );
//   }
//   // unisim::service::interfaces::MemoryInjection<ADDRESS>

//   bool InjectReadMemory(addr_t addr, void *buffer, uint32_t size) { m_mem.read( (uint8_t*)buffer, addr, size ); return true; }
//   bool InjectWriteMemory(addr_t addr, void const* buffer, uint32_t size) { m_mem.write( addr, (uint8_t*)buffer, size ); return true; }
//   // Implementation of ExecuteSystemCall
  
//   void ExecuteSystemCall( unsigned id )
//   {
//     if (not linux_os)
//       { throw std::logic_error( "No linux OS emulation connected" ); }
//     linux_os->ExecuteSystemCall( id );
//     // auto los = dynamic_cast<LinuxOS*>( linux_os );
//     // los->LogSystemCall( id );
//   }
  
//   addr_t rip;

//   enum ipproc_t { ipjmp = 0, ipcall, ipret };
//   u64_t                       getnip() { return rip; }
//   void                        setnip( u64_t _rip, ipproc_t ipproc = ipjmp ) { rip = _rip; }
//   //void                        addeip( u64_t offset ) { rip += offset; }

//   void                        syscall()
//   {
//     this->ExecuteSystemCall( this->regread( GOq(), 0 ) );
//   }

//   void                        interrupt( uint8_t _exc )
//   {
//     std::cerr << "Unhandled interruption (0x" << std::hex << unsigned( _exc ) << ").\n";
//     exit( 0 );
//   }
  
//   uint64_t u64regs[16]; ///< extended reg

//   struct FLAG
//   {
//     enum Code {
//       CF = 0, PF,     AF,     ZF,     SF,     DF,     OF,
//       C0,     C1,     C2,     C3,
//       end
//     };
//   };

// protected:
//   bool                        m_flags[FLAG::end];

// public:
//   bit_t                       flagread( FLAG::Code flag )
//   { return m_flags[flag]; }
//   void                        flagwrite( FLAG::Code flag, bit_t fval )
//   { m_flags[flag] = fval; }
  
//   // FLOATING POINT STATE
// protected:
//   double                      m_fregs[8];
//   unsigned                    m_ftop;
//   uint16_t                    m_fcw;
    
// public:
//   void                        fnanchk( f64_t value ) {};
//   unsigned                    ftopread() { return m_ftop; }
//   int                         fcwreadRC() const { return int( (m_fcw >> 10) & 3 ); }
//   void                        fpush( f64_t value )
//   { fnanchk( value ); m_ftop = ((m_ftop + 0x7) & 0x7); m_fregs[m_ftop] = value; /*m_dirtfregs |= (1 << m_ftop);*/ }
//   void                        fwrite( unsigned idx, f64_t value )
//   { fnanchk( value ); unsigned fidx = (m_ftop + idx) & 0x7; m_fregs[fidx] = value; /*m_dirtfregs |= (1 << fidx);*/ }
//   f64_t                       fpop()
//   { f64_t value = m_fregs[m_ftop++]; m_ftop &= 0x7; fnanchk( value ); return value; }
//   f64_t                       fread( unsigned idx )
//   { unsigned fidx = (m_ftop + idx) & 0x7; f64_t value = m_fregs[fidx]; fnanchk( value ); return value; }
//   void                        finit()
//   {
//     m_ftop = 0;
//     m_fcw = 0x37f;
//     flagwrite( FLAG::C0, 0 );
//     flagwrite( FLAG::C1, 0 );
//     flagwrite( FLAG::C2, 0 );
//     flagwrite( FLAG::C3, 0 );
//   }
    
//   void
//   fxam()
//   {
//     double val = this->fread( 0 );
      
//     flagwrite( FLAG::C1, __signbit( val ) );
      
//     switch (__fpclassify( val )) {
//     case FP_NAN:
//       flagwrite( FLAG::C3, 0 );
//       flagwrite( FLAG::C2, 0 );
//       flagwrite( FLAG::C0, 1 );
//       break;
//     case FP_NORMAL:
//       flagwrite( FLAG::C3, 0 );
//       flagwrite( FLAG::C2, 1 );
//       flagwrite( FLAG::C0, 0 );
//       break;
//     case FP_INFINITE:
//       flagwrite( FLAG::C3, 0 );
//       flagwrite( FLAG::C2, 1 );
//       flagwrite( FLAG::C0, 1 );
//       break;
//     case FP_ZERO:
//       flagwrite( FLAG::C3, 1 );
//       flagwrite( FLAG::C2, 0 );
//       flagwrite( FLAG::C0, 0 );
//       break;
//     case FP_SUBNORMAL:
//       flagwrite( FLAG::C3, 1 );
//       flagwrite( FLAG::C2, 1 );
//       flagwrite( FLAG::C0, 0 );
//       break;
//     }
//   }
    
//   u16_t
//   fcwread()
//   {
//     return u16_t( m_fcw );
//     // return u16_t( (1 << 0/*IM*/) |
//     //               (1 << 1/*DM*/) |
//     //               (1 << 2/*ZM*/) |
//     //               (1 << 3/*OM*/) |
//     //               (1 << 4/*UM*/) |
//     //               (1 << 5/*PM*/) |
//     //               (2 << 8/*PC*/) |
//     //               (0 << 10/*RC*/) |
//     //               (0 << 12/*X*/) );
//   }
    
//   void
//   fcwwrite( u16_t _value )
//   {
//     m_fcw = _value;
//     struct field { uint16_t offset, mask, expected, err; char const* name; };
//     static field fields[] =
//       {
//         { 0, 1, 1, 0, "IM"},
//         { 1, 1, 1, 0, "DM"},
//         { 2, 1, 1, 0, "ZM"},
//         { 3, 1, 1, 0, "OM"},
//         { 4, 1, 1, 0, "UM"},
//         { 5, 1, 1, 0, "PM"},
//         { 8, 3, 2, 0, "PC"},
//         {10, 3, 0, 0, "RC"},
//         {12, 1, 0, 0, "X"},
//       };
//     for (uintptr_t idx = 0; idx < (sizeof (fields) / sizeof (field)); ++idx)
//       {
//         uint16_t field_val = ((_value >> fields[idx].offset) & fields[idx].mask);
//         if (field_val == fields[idx].expected)
//           continue; /* value is expected one*/
//         if ((fields[idx].err >> field_val) & 1)
//           continue; /* error already reported */
//         fields[idx].err |= (1 << field_val);
//         std::cerr << "Warning: unimplemented FPUControlWord." << fields[idx].name
//                   << " value: " << field_val << ".\n";
//       }
//   }
    
//   u64_t tscread() { return instruction_count; }
    
// public: 
//   struct VUnion
//   {
//     static unsigned const BYTECOUNT = 32;
      
//     typedef void (*transfer_t)( uint8_t* dst, uint8_t const* src, unsigned size, bool destroy );
    
//     template <typename ELEM>
//     static unsigned ItemCount() { return BYTECOUNT / VectorTypeInfo<ELEM>::bytecount; }
    
//     template <typename ELEM>
//     static void Transfer( uint8_t* bytes, uint8_t const* storage, unsigned size, bool destroy )
//     {
//       ELEM const* vec = reinterpret_cast<ELEM const*>( storage );
      
//       uint8_t* dst_end = &bytes[size];
//       for (unsigned idx = 0, end = BYTECOUNT / VectorTypeInfo<ELEM>::bytecount; idx < end; ++idx)
//         {
//           uint8_t* dst = &bytes[idx*VectorTypeInfo<ELEM>::bytecount];
//           if (dst < dst_end)
//             VectorTypeInfo<ELEM>::ToBytes( dst, vec[idx] );
//           if (destroy)
//             VectorTypeInfo<ELEM>::Destroy( vec[idx] );
//         }
//     }

//     template <class ELEM>
//     ELEM*
//     rearrange( uint8_t* storage, unsigned final_size )
//     {
//       ELEM* res = reinterpret_cast<ELEM*>( storage );
//       transfer_t current = &Transfer<ELEM>;
//       unsigned const elem_size = VectorTypeInfo<ELEM>::bytecount;
//       if (transfer != current)
//         {
//           uint8_t buf[BYTECOUNT];
//           unsigned valid_size = std::min(final_size,size);
//           transfer( &buf[0], storage, valid_size, true );
//           uint8_t const* src_end = &buf[valid_size];
//           for (unsigned idx = 0, end = BYTECOUNT / elem_size; idx < end; ++idx)
//             {
//               uint8_t const* src = &buf[idx*elem_size];
//               new (&res[idx]) ELEM();
//               if (src < src_end)
//                 VectorTypeInfo<ELEM>::FromBytes( res[idx], src );
//             }
//           transfer = current;
//         }
//       else if (size < final_size)
//         {
//           for (unsigned idx = size / elem_size, end = final_size / elem_size; idx < end; ++idx)
//             res[idx] = ELEM();
//         }
//       size = final_size;
//       return res;
//     }

//     template <typename ELEM>
//     ELEM*
//     GetStorage( uint8_t* storage, ELEM const&, unsigned final_size )
//     {
//       return rearrange<ELEM>( storage, final_size );
//     }

//     template <class ELEM>
//     ELEM* GetConstStorage( uint8_t* storage, ELEM const&, unsigned required_size )
//     {
//       return rearrange<ELEM>( storage, std::max(size, required_size) );
//     }

//     static void initial( uint8_t*, uint8_t const*, unsigned, bool ) {}
      
//     VUnion() : transfer( &initial ), size(0) {}

//     transfer_t transfer;
//     unsigned   size;
//   } umms[16];
  
//   uint8_t vmm_storage[16][VUnion::BYTECOUNT];
//   uint32_t mxcsr;

//   template <class VR> static unsigned vmm_wsize( VR const& vr ) { return VR::size() / 8; }
//   static unsigned vmm_wsize( unisim::component::cxx::processor::intel::SSE const& ) { return VUnion::BYTECOUNT; }
  
//   template <class VR, class ELEM>
//   ELEM
//   vmm_read( VR const& vr, unsigned reg, unsigned sub, ELEM const& e )
//   {
//     ELEM const* elems = umms[reg].GetConstStorage( &vmm_storage[reg][0], e, vr.size() / 8 );
//     return elems[sub];
//   }
  
//   template <class VR, class ELEM>
//   void
//   vmm_write( VR const& vr, unsigned reg, unsigned sub, ELEM const& e )
//   {
//     ELEM* elems = umms[reg].GetStorage( &vmm_storage[reg][0], e, vmm_wsize( vr ) );
//     elems[sub] = e;
//     //gdbchecker.xmark(reg);
//   }

//   // Integer case
//   template <class TYPE> TYPE vmm_memread( unsigned seg, u64_t addr, TYPE const& e )
//   {
//     typedef unisim::component::cxx::processor::intel::atpinfo<Arch,TYPE> atpinfo;
//     return TYPE(memread<atpinfo::bitsize>(seg,addr));
//   }
  
//   f32_t vmm_memread( unsigned seg, u64_t addr, f32_t const& e ) { return fmemread32( seg, addr ); }
//   f64_t vmm_memread( unsigned seg, u64_t addr, f64_t const& e ) { return fmemread64( seg, addr ); }
//   f80_t vmm_memread( unsigned seg, u64_t addr, f80_t const& e ) { return fmemread80( seg, addr ); }
  
//   // Integer case
//   template <class TYPE> void vmm_memwrite( unsigned seg, u64_t addr, TYPE const& e )
//   {
//     typedef unisim::component::cxx::processor::intel::atpinfo<Arch,TYPE> atpinfo;
//     memwrite<atpinfo::bitsize>(seg,addr,typename atpinfo::utype(e));
//   }
  
//   void vmm_memwrite( unsigned seg, u64_t addr, f32_t const& e ) { return fmemwrite32( seg, addr, e ); }
//   void vmm_memwrite( unsigned seg, u64_t addr, f64_t const& e ) { return fmemwrite64( seg, addr, e ); }
//   void vmm_memwrite( unsigned seg, u64_t addr, f80_t const& e ) { return fmemwrite80( seg, addr, e ); }

//   // void vmm_memwrite( unsigned seg, u64_t addr, u8_t  const& e ) { return memwrite< 8>( seg, addr, e ); }
//   // void vmm_memwrite( unsigned seg, u64_t addr, u16_t const& e ) { return memwrite<16>( seg, addr, e ); }
//   // void vmm_memwrite( unsigned seg, u64_t addr, u32_t const& e ) { return memwrite<32>( seg, addr, e ); }
//   // void vmm_memwrite( unsigned seg, u64_t addr, u64_t const& e ) { return memwrite<64>( seg, addr, e ); }
  
//   template <class VR, class ELEM>
//   ELEM
//   vmm_read( VR const& vr, RMOp const& rmop, unsigned sub, ELEM const& e )
//   {
//     if (not rmop.is_memory_operand()) return vmm_read( vr, rmop.ereg(), sub, e );
//     return vmm_memread( rmop->segment, rmop->effective_address( *this ) + sub*VectorTypeInfo<ELEM>::bytecount, e );
//   }
    
//   template <class VR, class ELEM>
//   void
//   vmm_write( VR const& vr, RMOp const& rmop, unsigned sub, ELEM const& e )
//   {
//     if (not rmop.is_memory_operand()) return vmm_write( vr, rmop.ereg(), sub, e );
//     return vmm_memwrite( rmop->segment, rmop->effective_address( *this ) + sub*VectorTypeInfo<ELEM>::bytecount, e );
//   }

//   void xgetbv();
//   void cpuid();
//   void _DE() { std::cerr << "#DE: Division Error.\n"; throw std::runtime_error("diverr"); }
  
//   typedef unisim::component::cxx::processor::intel::Operation<Arch> Operation;
//   Operation* latest_instruction;
//   Operation* fetch();

//   void stop() { std::cerr << "Processor halt.\n"; throw 0; }

//   struct Unimplemented {};
//   void noexec( Operation const& op )
//   {
//     std::cerr
//       << "error: no execute method in `" << typeid(op).name() << "'\n"
//       << std::hex << op.address << ":\t";
//     op.disasm( std::cerr );
//     std::cerr << '\n';
//     throw Unimplemented();
//   }
  
//   bool do_disasm;
//   uint64_t instruction_count;

//   bool Cond( bool b ) const { return b; }

//   // struct GDBChecker
//   // {
//   //   GDBChecker() : sink("check.gdb"), visited(), nassertid(0), gdirtmask( 0 ), xdirtmask( 0 ) {}
//   //   std::ofstream sink;
//   //   std::map<uint64_t,uint64_t> visited;
//   //   uint64_t nassertid;
//   //   uint32_t gdirtmask;
//   //   uint32_t xdirtmask;
//   //   void gmark( unsigned reg ) { gdirtmask |= (1 << reg); }
//   //   void xmark( unsigned reg ) { xdirtmask |= (1 << reg); }
//   //   uint64_t getnew_aid() { if (++nassertid > 0x80000) throw 0; return nassertid; }
//   //   void start( Arch const& cpu)
//   //   {
//   //     sink << "set pagination off\n\n"
//   //          << "define insn_assert\n  if $arg1 != $arg2\n    printf \"insn_assert %u failed.\\n\", $arg0\n    bad_assertion\n  end\nend\n\n"
//   //          << "break *0x" << std::hex << cpu.rip << "\nrun\n\n";
//   //   }
//   //   void step( Arch const& cpu )
//   //   {
//   //     cpu.latest_instruction->disasm( sink << "# " ); sink << "\n";
//   //     uint64_t cia = cpu.rip;
//   //     uint64_t revisit = visited[cia]++;
//   //     sink << "stepi\n";
//   //     if (revisit == 0)
//   //       sink << "# advance *0x" << std::hex << cia << "\n";
//   //     else
//   //       sink << "# break *0x" << std::hex << cia << "; cont; cont " << std::dec << revisit << "\n";
//   //     sink << "insn_assert " << std::dec << getnew_aid() << " $rip 0x" << std::hex  << cia << '\n';
//   //     for (unsigned reg = 0; reg < 16; ++reg)
//   //       {
//   //         if (not ((gdirtmask>>reg) & 1)) continue;
//   //         uint64_t value = cpu.u64regs[reg];
//   //         std::ostringstream rn; rn << unisim::component::cxx::processor::intel::DisasmG(GOq(),reg);
//   //         sink << "insn_assert " << std::dec << getnew_aid() << " $" << &(rn.str().c_str()[1]) << " 0x" << std::hex << value << '\n';
//   //       }
//   //     gdirtmask = 0;
//   //     for (unsigned reg = 0; reg < 16; ++reg)
//   //       {
//   //         if (not ((xdirtmask>>reg) & 1)) continue;
//   //         uint64_t quads[2] = {0};
//   //         cpu.umms[reg].transfer( (uint8_t*)&quads[0], &cpu.vmm_storage[reg][0], 16, false );
//   //         sink << "insn_assert " << std::dec << getnew_aid() << " $xmm" << reg << ".v2_int64[0] 0x" << std::hex << quads[0] << '\n';
//   //         sink << "insn_assert " << std::dec << getnew_aid() << " $xmm" << reg << ".v2_int64[1] 0x" << std::hex << quads[1] << '\n';
//   //       }
//   //     xdirtmask = 0;
//   //   }
//   // } gdbchecker;
// }
    // typedef unisim::util::symbolic::Expr       Expr;
    // std::map<unsigned,VirtualRegister> irmap;
    // std::vector<unsigned>              iruse;
    // VirtualRegister                    xer, cr, spefscr;
    // std::set<Expr>                     mem_addrs;
    // uint8_t                            base_register;
    // bool                               aligned;
    // bool                               mem_writes;
    // uint8_t                            length;
    // bool                               retfalse;

  
    // typedef unisim::util::symbolic::ExprNode   ExprNode;
    
    // struct VirtualRegister
    // {
    //   VirtualRegister() : vindex(0), source(false), destination(false), bad(true) { throw 0; }
    //   VirtualRegister( unsigned _index ) : vindex(_index), source(false), destination(false), bad(false) {}
    
    //   uint8_t vindex      : 5;
    //   uint8_t source      : 1;
    //   uint8_t destination : 1;
    //   uint8_t bad         : 1;
    
    //   void addaccess( bool w ) { source |= not w; destination |= w; }
    
    //   int  cmp( VirtualRegister const& ) const;
    // };

    
    
    // void irappend( uint8_t _index, bool w );
    
    // //    bool usemem() const { return mem_addrs.size(); }
    
    
    // struct Prologue
    // {
    //   struct Error {};
      
    //   typedef std::map<unsigned,uint32_t> Regs;
    //   Prologue( Regs const& _regs, uint32_t _offset, bool _sign, uint8_t _base )
    //     : regs( _regs ), offset( _offset ), sign( _sign ), base( _base )
    //   {}
    //   Regs regs; uint32_t offset; bool sign; uint8_t base;
    // };
    
    // Prologue GetPrologue() const;
    
    // void PCRelPrologue( Prologue const& pc ) const;
    
    //    void load( Expr const& addr ) { mem_addrs.insert( addr ); }
    //    void store( Expr const& addr ) { mem_addrs.insert( addr ); mem_writes = true; }
  };

} // end of namespace ut

#endif // ARCH_HH

// namespace ut
// {
//   struct SourceReg : public unisim::util::symbolic::ExprNode
//   {
//     SourceReg( unsigned _reg ) : reg( _reg ) {}
//     virtual SourceReg* Mutate() const { return new SourceReg(*this); }
//     virtual ScalarType::id_t GetType() const { return ScalarType::U32; }
//     virtual void Repr( std::ostream& sink ) const;
//     virtual unsigned SubCount() const { return 0; };
//     virtual intptr_t cmp( unisim::util::symbolic::ExprNode const& brhs ) const
//     {
//       unsigned ref = dynamic_cast<SourceReg const&>( brhs ).reg;
//       return (reg < ref) ? -1 : (reg > ref) ? +1 : 0;
//     }
//     unsigned reg;
//   };
  
//   struct CPU;

//   struct MixNode : public unisim::util::symbolic::ExprNode
//   {
//     typedef unisim::util::symbolic::Expr Expr;
//     typedef unisim::util::symbolic::ExprNode ExprNode;
    
//     MixNode( Expr const& _left, Expr const& _right ) : left(_left), right(_right) {}
//     virtual MixNode* Mutate() const { return new MixNode(*this); }
//     virtual ScalarType::id_t GetType() const { return left->GetType(); }
//     virtual void Repr( std::ostream& sink ) const;
//     virtual unsigned SubCount() const { return 2; };
//     virtual Expr const& GetSub(unsigned idx) const { switch (idx) { case 0: return left; case 1: return right; } return ExprNode::GetSub(idx); };
//     virtual intptr_t cmp( ExprNode const& brhs ) const { return compare(static_cast<MixNode const&>( brhs )); }
//     intptr_t compare( MixNode const& rhs ) const
//     {
//       if (intptr_t delta = left.cmp( rhs.left )) return delta;
//       return right.cmp( rhs.right );
//     }
//     Expr left, right;
//   };
  
//   struct UnknownBase : public unisim::util::symbolic::ExprNode
//   {
//     typedef unisim::util::symbolic::Expr Expr;
//     typedef unisim::util::symbolic::ExprNode ExprNode;
    
//     UnknownBase() {}
//     virtual void Repr( std::ostream& sink ) const { sink << "?"; }
//     virtual unsigned SubCount() const { return 0; };
//     virtual Expr const& GetSub(unsigned idx) const { return ExprNode::GetSub(idx); };
//     virtual intptr_t cmp( ExprNode const& brhs ) const { return 0; }
//   };

//   template <typename T>
//   struct Unknown : public UnknownBase
//   {
//     virtual UnknownBase* Mutate() const { return new Unknown<T>(*this); }
//     virtual ScalarType::id_t GetType() const { return unisim::util::symbolic::TypeInfo<T>::GetType(); }
//   };
  
//   template <class T>
//   T make_unknown() { return T( UnknownBase::Expr( new Unknown<typename T::value_type>() ) ); }
  
//   struct XER
//   {
//     struct OV {};
//     struct SO {};
//     struct CA {};
//     struct _0_3 {};
    
//     typedef unisim::util::symbolic::Expr Expr;
//     typedef unisim::util::symbolic::ExprNode ExprNode;
    
//     template <typename PART,typename T> void Set( SmartValue<T> const& value ) { XERAccess(true); xer_value = Expr( new MixNode( xer_value.expr, value.expr ) ); }
//     template <typename PART> void Set( uint32_t value ) { Set<PART,uint32_t>( unisim::util::symbolic::make_const(value) ); }
//     template <typename PART> U32 Get() { XERAccess(false); return xer_value; }
//     operator U32 () { XERAccess(false); return xer_value; }
//     XER& operator= ( U32 const& value ) { XERAccess(true); xer_value = value; return *this; }
//     XER& GetXER() { return *this; }
    
//     struct XERNode : public ExprNode
//     {
//       virtual XERNode* Mutate() const { return new XERNode(*this); }
//       virtual ScalarType::id_t GetType() const { return ScalarType::U32; }
//       virtual void Repr( std::ostream& sink ) const;
//       virtual unsigned SubCount() const { return 0; };
//       virtual intptr_t cmp( ExprNode const& brhs ) const { return 0; }
//     };
    
//     XER() : xer_value( new XERNode ) {}
    
//     U32 xer_value;
//     virtual void XERAccess( bool is_write ) = 0;
//   };

//   struct CR
//   {
//     struct CR0 { struct OV {}; struct SO {}; struct LT {}; struct GT {}; struct EQ {}; struct ALL {}; };
//     struct CR1 { struct OV {}; struct SO {}; struct LT {}; struct GT {}; struct EQ {}; struct ALL {}; };
//     struct CR2 { struct OV {}; struct SO {}; struct LT {}; struct GT {}; struct EQ {}; struct ALL {}; };
//     struct CR3 { struct OV {}; struct SO {}; struct LT {}; struct GT {}; struct EQ {}; struct ALL {}; };
//     struct CR4 { struct OV {}; struct SO {}; struct LT {}; struct GT {}; struct EQ {}; struct ALL {}; };
//     struct CR5 { struct OV {}; struct SO {}; struct LT {}; struct GT {}; struct EQ {}; struct ALL {}; };
//     struct CR6 { struct OV {}; struct SO {}; struct LT {}; struct GT {}; struct EQ {}; struct ALL {}; };
//     struct CR7 { struct OV {}; struct SO {}; struct LT {}; struct GT {}; struct EQ {}; struct ALL {}; };
    
//     typedef unisim::util::symbolic::Expr Expr;
//     typedef unisim::util::symbolic::ExprNode ExprNode;
    
//     template <typename PART,typename T> void Set( SmartValue<T> const& value ) { CRAccess(true); cr_value = Expr( new MixNode( cr_value.expr, value.expr ) ); }
//     template <typename PART> void Set( uint32_t value ) { Set<PART,uint32_t>( unisim::util::symbolic::make_const(value) ); }
//     template <typename PART> U32 Get() { CRAccess(false); return cr_value; }
//     operator U32 () { CRAccess(false); return cr_value; }
//     CR& operator= ( U32 const& value ) { CRAccess(true); cr_value = value; return *this; }
//     CR& GetCR() { return *this; }
    
//     struct CRNode : public ExprNode
//     {
//       virtual CRNode* Mutate() const { return new CRNode(*this); }
//       virtual ScalarType::id_t GetType() const { return ScalarType::U32; }
//       virtual void Repr( std::ostream& sink ) const;
//       virtual unsigned SubCount() const { return 0; };
//       virtual intptr_t cmp( ExprNode const& brhs ) const { return 0; }
//     };

//     CR() : cr_value( new CRNode ) {}

//     U32 cr_value;
//     virtual void CRAccess( bool is_write ) = 0;
//   };
  
//   struct LR
//   {
//     template <typename PART,typename T> void Set( SmartValue<T> const& value ) { throw Untestable("LR access");  }
//     template <typename PART> void Set( unsigned ) { throw Untestable("LR access");  }
//     template <typename PART> U32 Get() { return U32(0); }
//     operator U32 () { return U32(0); }
//     LR& operator= (U32 const& v) { throw Untestable("LR access"); return *this; }
//     LR& GetLR() { return *this; }
//     void SetLR(U32 const& v) { throw Untestable("LR access"); }
//   };
  
//   struct CTR
//   {
//     template <typename PART,typename T> void Set( SmartValue<T> const& value ) { throw Untestable("CTR access"); }
//     template <typename PART> void Set( unsigned ) { throw Untestable("CTR access"); }
//     template <typename PART> U32 Get() { return U32(0); }
//     operator U32 () { return U32(0); }
//     CTR& operator= (U32 const& v) { throw Untestable("CTR access"); return *this; }
//     CTR& GetCTR() { return *this; }
//     void SetCTR(U32 const& v) { throw Untestable("CTR access"); }
//   };
  
//   struct MSR
//   {
//     struct PR {};
//     struct EE {};
//     template <typename PART,typename T> void Set( SmartValue<T> const& value ) { throw Untestable("MSR access"); }
//     template <typename PART> void Set( unsigned ) { throw Untestable("MSR access"); }
//     template <typename PART> U32 Get() { throw Untestable("MSR access"); return U32(0); }
//     operator U32 () { throw Untestable("MSR access"); return U32(0); }
//     MSR& operator= (U32 const& v) { throw Untestable("MSR access"); return *this; }
//     MSR& GetMSR() { throw Untestable("MSR access"); return *this; }
//   };

//   struct SPEFSCR
//   {
//     struct FRMC
//     {
//       static uint32_t NEAREST() { return 0; }
//       static uint32_t ZERO()    { return 1; }
//       static uint32_t UP()      { return 2; }
//       static uint32_t DOWN()    { return 3; }
//     };
//     struct FINVE {};
//     struct FINV {};
//     struct FINVS {};
    
//     typedef unisim::util::symbolic::Expr Expr;
//     typedef unisim::util::symbolic::ExprNode ExprNode;
    
//     template <typename PART,typename T> void Set( SmartValue<T> const& value ) { SPEFSCRAccess(true); spefscr_value = Expr( new MixNode( spefscr_value.expr, value.expr ) ); }
//     template <typename PART> void Set( uint32_t value ) { Set<PART,uint32_t>( unisim::util::symbolic::make_const(value) ); }
//     template <typename PART> U32 Get() { SPEFSCRAccess(false); return spefscr_value; }
//     operator U32 () { SPEFSCRAccess(false); return spefscr_value; }
//     SPEFSCR& operator= ( U32 const& value ) { SPEFSCRAccess(true); spefscr_value = value; return *this; }
//     SPEFSCR& GetSPEFSCR() { return *this; }
    
//     struct SPEFSCRNode : public ExprNode
//     {
//       virtual SPEFSCRNode* Mutate() const { return new SPEFSCRNode(*this); }
//       virtual ScalarType::id_t GetType() const { return ScalarType::U32; }
//       virtual void Repr( std::ostream& sink ) const;
//       virtual unsigned SubCount() const { return 0; };
//       virtual intptr_t cmp( ExprNode const& brhs ) const { return 0; }
//     };
    
//     BOOL SetInvalid( bool inv, bool invh=false )
//     {
//       this->template Set<FINV>( U32(inv) );
//       //this->template Set<FINVH>( U32(invh) );
//       return make_unknown<BOOL>();
//     }
//     BOOL SetUnderflow( bool uf ) { return make_unknown<BOOL>(); }
//     BOOL SetOverflow( bool uf ) { return make_unknown<BOOL>(); }
//     BOOL SetDivideByZero( bool uf ) { return make_unknown<BOOL>(); }
    
//     SPEFSCR() : spefscr_value( new SPEFSCRNode ) {}
    
//     U32 spefscr_value;
//     virtual void SPEFSCRAccess( bool is_write ) = 0;
//   };
    
//   struct CPU : public XER, public CR, public MSR, public LR, public CTR, public SPEFSCR
//   {
//     typedef ut::BOOL BOOL;
//     typedef ut::U8   U8;
//     typedef ut::U16  U16;
//     typedef ut::U32  U32;
//     typedef ut::U64  U64;
//     typedef ut::S8   S8;
//     typedef ut::S16  S16;
//     typedef ut::S32  S32;
//     typedef ut::S64  S64;

//     typedef ut::UINT UINT;
//     typedef ut::SINT SINT;
//     typedef ut::ADDRESS ADDRESS;
    
    
//     typedef unisim::util::symbolic::Expr       Expr;
//     typedef unisim::util::symbolic::ExprNode   ExprNode;
    
//     typedef MSR MSR;
//     typedef SPEFSCR SPEFSCR;
    
//     CPU( Interface& _interface, ActionNode& root )
//       : interface(_interface), path(&root), cia( new CIA )
//     {     
//       for (unsigned reg = 0; reg < 32; ++reg)
//         reg_values[reg] = U32( new SourceReg( reg ) );
//     }
    
//     virtual void XERAccess( bool is_write ) { interface.xer.addaccess(is_write); }
//     virtual void CRAccess( bool is_write ) { interface.cr.addaccess(is_write); }
//     virtual void SPEFSCRAccess( bool is_write ) { interface.spefscr.addaccess(is_write); }
    
//     struct Interrupt { void SetELEV(unsigned x); };
    
//     struct SystemCallInterrupt
//     {
//       struct SystemCall {};
//     };
    
//     struct AlignmentInterrupt
//     {
//       struct UnalignedLoadStoreMultiple {};
//     };
    
//     struct ProgramInterrupt
//     {
//       struct UnimplementedInstruction {};
//       struct IllegalInstruction {};
//       struct Trap {};
//       struct PrivilegeViolation {};
//     };
    
//     template <class T> Interrupt ThrowException() { DispatchException( T() ); return Interrupt(); }
    
//     template <class T> void DispatchException( T const& exc ) { donttest_system(); }
//     void DispatchException( ProgramInterrupt::UnimplementedInstruction const& exc ) { throw Untestable("not implemented"); }
//     void DispatchException( AlignmentInterrupt::UnalignedLoadStoreMultiple const& exc ) { interface.aligned = true; }
    
    
//     template <typename T> bool Cond( SmartValue<T> const& cond )
//     {
//       if (not cond.expr.good())
//         throw std::logic_error( "Not a valid condition" );

//       Expr cexp( BOOL(cond).expr );
//       if (unisim::util::symbolic::ConstNodeBase const* cnode = cexp.ConstSimplify())
//         return cnode->GetBoolean();

//       bool predicate = path->proceed( cexp );
//       path = path->next( predicate );
//       return predicate;
//     }
    
//     bool Cond(bool c) { return c; }
    
//     bool close() { return path->close(); }

//     Interface&   interface;
//     ActionNode*  path;
//     U32          reg_values[32];
//     U32          cia;
    
    
//     struct CIA : public ExprNode
//     {
//       CIA() {}
//       virtual CIA* Mutate() const { return new CIA(*this) ; }
//       virtual ScalarType::id_t GetType() const { return ScalarType::U32; }
//       virtual void Repr( std::ostream& sink ) const { sink << "CIA"; }
//       virtual unsigned SubCount() const { return 0; };
//       virtual intptr_t cmp( unisim::util::symbolic::ExprNode const& brhs ) const { return 0; }
//     };
    
//     U32 GetCIA() { return cia; };
//     bool EqualCIA(uint32_t pc) { return false; };
//     U32 GetGPR(unsigned n) { gpr_append(n,false); return reg_values[n]; };
//     void SetGPR(unsigned n, U32 value) { gpr_append(n,true); reg_values[n] = value; }
    
//     static void LoadRepr( std::ostream& sink, Expr const& _addr, unsigned bits );
    
//     template <unsigned BITS>
//     struct Load : public ExprNode
//     {
//       Load( Expr const& _addr ) : addr(_addr) {}
//       virtual Load* Mutate() const { return new Load(*this); }
//       virtual ScalarType::id_t GetType() const { return ScalarType::IntegerType(false,BITS); }
//       virtual void Repr( std::ostream& sink ) const { LoadRepr( sink, addr, BITS ); }
//       virtual unsigned SubCount() const { return 2; };
//       virtual Expr const& GetSub(unsigned idx) const { switch (idx) { case 0: return addr; } return ExprNode::GetSub(idx); };
//       virtual intptr_t cmp( unisim::util::symbolic::ExprNode const& brhs ) const
//       { return addr.cmp( dynamic_cast<Load<BITS> const&>( brhs ).addr ); }
//       Expr addr;
//     };
    
//     template <unsigned BITS> Expr MemRead( U32 const& _addr )
//     {
//       interface.load( _addr.expr );
//       return new Load<BITS>( _addr.expr );
//     }
//     template <unsigned BITS> void MemWrite( U32 const& _addr, typename TypeFor<BITS>::U const& _val )
//     {
//       interface.store( _addr.expr );
//     }
    
//     bool Int8Load(unsigned n, U32 const& address) { SetGPR(n, CPU::U32(CPU::U8(MemRead<8>(address)))); return true; }
//     bool Int16Load(unsigned n, U32 const& address) { SetGPR(n, CPU::U32(CPU::U16(MemRead<16>(address)))); return true; }
//     bool Int32Load(unsigned n, U32 const& address) { SetGPR(n, CPU::U32(MemRead<32>(address))); return true; }
    
//     bool SInt8Load(unsigned n, U32 const& address) { SetGPR(n, CPU::U32(CPU::S8(MemRead<8>(address)))); return true; }
//     bool SInt16Load(unsigned n, U32 const& address) { SetGPR(n, CPU::U32(CPU::S16(MemRead<16>(address)))); return true; }

//     bool Int16LoadByteReverse(unsigned n, U32 const& address) { SetGPR(n, ByteSwap(CPU::U32(CPU::U16(MemRead<16>(address))))); return true; }
//     bool Int32LoadByteReverse(unsigned n, U32 const& address) { SetGPR(n, ByteSwap(CPU::U32(MemRead<32>(address)))); return true; }
    
//     bool Int8Store(unsigned n, U32 const& address ) { MemWrite<8>( address, U8(GetGPR(n)) ); return true; }
//     bool Int16Store(unsigned n, U32 const& address ) { MemWrite<16>( address, U16(GetGPR(n)) ); return true; }
//     bool Int32Store(unsigned n, U32 const& address ) { MemWrite<32>( address, U32(GetGPR(n)) ); return true; }

//     bool Int16StoreByteReverse(unsigned n, U32 const& address ) { MemWrite<16>( address, ByteSwap(U16(GetGPR(n))) ); return true; }
//     bool Int32StoreByteReverse(unsigned n, U32 const& address ) { MemWrite<32>( address, ByteSwap(U32(GetGPR(n))) ); return true; }

//     void gpr_append( unsigned idx, bool w ) { interface.irappend( idx, w ); }

//     void donttest_system();
//     void donttest_branch();
//     void donttest_illegal();
    
//     char const* GetObjectFriendlyName(U32) { return "???"; }
    
//     bool Branch(U32 const& addr) { donttest_branch(); return false; }
    
//     bool Rfmci() { donttest_system(); return false; }
//     bool Rfci() { donttest_system(); return false; }
//     bool Rfdi() { donttest_system(); return false; }
//     bool Rfi() { donttest_system(); return false; }

//     bool Dcba(U32 const& addr) { donttest_system(); return false; }
//     bool Dcbf(U32 const& addr) { donttest_system(); return false; }
//     bool Dcbst(U32 const& addr) { donttest_system(); return false; }
//     bool Dcbz(U32 const& addr) { donttest_system(); return false; }
//     bool Dcbi(U32 const& addr) { donttest_system(); return false; }
//     bool Icbi(U32 const& addr) { donttest_system(); return false; }
//     bool Icbt(U32 const& addr) { donttest_system(); return false; }
    
//     bool Msync() { donttest_system(); return false; }
//     bool Isync() { donttest_system(); return false; }
//     bool Mpure() { donttest_system(); return false; }
//     bool Mpuwe() { donttest_system(); return false; }
//     bool Mpusync() { donttest_system(); return false; }
    
//     bool Lbarx(unsigned n, U32 const& addr) { donttest_system(); return false; }
//     bool Lharx(unsigned n, U32 const& addr) { donttest_system(); return false; }
//     bool Lwarx(unsigned n, U32 const& addr) { donttest_system(); return false; }
//     bool Stbcx(unsigned n, U32 const& addr) { donttest_system(); return false; }
//     bool Sthcx(unsigned n, U32 const& addr) { donttest_system(); return false; }
//     bool Stwcx(unsigned n, U32 const& addr) { donttest_system(); return false; }
//     bool MoveFromDCR(unsigned dcrn, U32& result) { donttest_system(); return false; }
//     bool MoveFromSPR(unsigned dcrn, U32& result) { donttest_system(); return false; }
//     bool MoveToSPR(unsigned dcrn, U32 const& result) { donttest_system(); return false; }
    
//     bool CheckSPV() { return true; }
//     bool Wait() { return false; }

//     struct __EFPProcessInput__
//     {
//       __EFPProcessInput__( CPU& _cpu ) : cpu(_cpu), finv(false) {};
//       template <class FLOAT>
//       __EFPProcessInput__& set( FLOAT& input ) { finv |= not check_input( input ) ; return *this; }
//       template <class FLOAT>
//       static bool check_input( FLOAT& input )
//       {
//         // if (unlikely(input.isDenormalized()))
//         //   {
//         //     input.setZero(input.isNegative());
//         //     return false;
//         //   }
			
//         // if (unlikely(input.hasInftyExponent()))
//         //   return false;
			
//         return true;
//       }
//       bool proceed()
//       {
//         // cpu.GetSPEFSCR().Set<typename SPEFSCR::FINV>(finv);
//         // if (finv)
//         //   {
//         //     cpu.GetSPEFSCR().Set<typename SPEFSCR::FINVS>(true);
//         //     if (cpu.GetSPEFSCR().Get<typename SPEFSCR::FINVE>())
//         //       {
//         //         cpu.ThrowException<typename ProgramInterrupt::UnimplementedInstruction>();
//         //         return false;
//         //       }
//         //   }
//         return true;
//       }
		
//       CPU& cpu;
//       bool finv;
//     };
	
//     __EFPProcessInput__
//     EFPProcessInput()
//     {
//       // GetSPEFSCR().Set<typename SPEFSCR::FG>(false);
//       // GetSPEFSCR().Set<typename SPEFSCR::FX>(false);
//       // GetSPEFSCR().Set<typename SPEFSCR::FGH>(false);
//       // GetSPEFSCR().Set<typename SPEFSCR::FXH>(false);
//       // GetSPEFSCR().Set<typename SPEFSCR::FDBZ>(false);
//       // GetSPEFSCR().Set<typename SPEFSCR::FDBZH>(false);
//       // GetSPEFSCR().SetDivideByZero( false );
//       return __EFPProcessInput__( *this );
//     }
	
//     template <class FLOAT, class FLAGS>
//     bool
//     EFPProcessOutput( FLOAT& output, FLAGS const& flags )
//     {
//       // if (output.hasInftyExponent())
//       //   {
//       //     bool neg = output.isNegative();
//       //     output.setInfty();
//       //     output.setToPrevious();
//       //     output.setNegative(neg);
//       //   }
//       // bool inexact = flags.isApproximate() and not spefscr.template Get<typename CPU::SPEFSCR::FINV>();
//       // bool overflow = inexact and flags.isOverflow();
//       // if (not GetSPEFSCR().SetOverflow( overflow ))
//       //   return false;
//       // bool underflow = inexact and flags.isUnderflow();
//       // if (output.isDenormalized())
//       //   {
//       //     output.setZero(output.isNegative());
//       //     inexact = true, underflow = true;
//       //   }
//       // if (not GetSPEFSCR().SetUnderflow( underflow ))
//       //   return false;

//       // if (inexact)
//       //   {
//       //     // Compute inexact flags (FG, FX)
//       //     GetSPEFSCR().Set<typename SPEFSCR::FINXS>(true);
//       //     if (spefscr.template Get<typename SPEFSCR::FINXE>())
//       //       {
//       //         this->template ThrowException<typename ProgramInterrupt::UnimplementedInstruction>();
//       //         return false;
//       //       }
//       //   }

//       return true;
//     }
	
    
//   };
  
//   struct Flags
//   {
//     struct RoundingMode { RoundingMode(U32 const& rm) {} RoundingMode(unsigned rm) {} };
//     Flags( RoundingMode const& rm ) {}
//     Flags() {}
//     void setRoundingMode(U32 const&) {}
//     void setRoundingMode(unsigned) {}
//     void setUnderflow() {}
//     void setOverflow() {}
//     void setDownApproximate() {}
//     void setUpApproximate() {}
//     void clearFlowException() {}
//     BOOL isUpApproximate() { return make_unknown<BOOL>(); }
//     BOOL isDownApproximate() { return make_unknown<BOOL>(); }
//     BOOL isOverflow() { return make_unknown<BOOL>(); }
//     BOOL takeOverflow() { return make_unknown<BOOL>(); }
//   };
  
//   struct BigInt
//   {
//     BigInt() {}
//     BigInt& sub(int) { return *this; }
//     BigInt& add(int) { return *this; }
//     BOOL hasCarry() { return make_unknown<BOOL>(); }
//     BOOL isZero() { return make_unknown<BOOL>(); }
//   };
  
//   struct SoftHalfFloat;
  
//   struct SoftFloat
//   {
//     enum __FromRawBits__ { FromRawBits };
//     SoftFloat( __FromRawBits__, U32 const& source ) {}
    
//     SoftFloat( U32 const&, Flags& ) {}
//     SoftFloat( S32 const&, Flags& ) {}
//     enum __FromFraction__ { FromFraction };
//     SoftFloat( __FromFraction__, U32 const&, Flags& ) {}
//     SoftFloat( __FromFraction__, S32 const&, Flags& ) {}
//     SoftFloat( SoftHalfFloat const&, Flags const& ) {}

//     U32 queryU32( Flags& flags, unsigned fbits=0 ) { return make_unknown<U32>(); }
//     S32 queryS32( Flags& flags, unsigned fbits=0 ) { return make_unknown<S32>(); }

//     // SoftFloat& convertAssign( SoftHalfFloat const&, Flags& );
//     // SoftFloat& fromRawBitsAssign( U32 const& );
    
//     U32 queryRawBits() { return make_unknown<U32>(); }
//     BOOL isNaN() { return make_unknown<BOOL>(); }
//     BOOL operator == (SoftFloat const&) { return make_unknown<BOOL>(); }
//     BOOL operator < (SoftFloat const&) { return make_unknown<BOOL>(); }
//     BOOL operator > (SoftFloat const&) { return make_unknown<BOOL>(); }
//     // void retrieveInteger(IntConversion const&, Flags const&) {}
//     // BOOL isPositive() { return make_unknown<BOOL>(); }
//     BOOL isNegative() { return make_unknown<BOOL>(); }
//     void setNegative(bool=false) {}
//     void setNegative(BOOL) {}
//     void plusAssign(SoftFloat const&, Flags const& flags) {}
//     void divAssign(SoftFloat const&, Flags const& flags) {}
//     void multAssign(SoftFloat const&, Flags const& flags) {}
//     void minusAssign(SoftFloat const&, Flags const& flags) {}
//     void sqrtAssign(Flags const& flags) {}
//     // void squareAssign(Flags const& flags) {}
//     void multAndAddAssign(SoftFloat const&, SoftFloat const&, Flags const& flags) {}
//     void multAndSubAssign(SoftFloat const&, SoftFloat const&, Flags const& flags) {}
//     // void multNegativeAndAddAssign(SoftFloat const&, SoftFloat const&, Flags const& flags) {}
//     // void multNegativeAndSubAssign(SoftFloat const&, SoftFloat const&, Flags const& flags) {}
//     void maxAssign(SoftFloat const&) {}
//     void minAssign(SoftFloat const&) {}
    
//     // BigInt querySBasicExponent() { return BigInt(); }
//     // BigInt queryBasicExponent() { return BigInt(); }
//     BOOL isZero() { return make_unknown<BOOL>(); }
//     BOOL isNormalized() { return make_unknown<BOOL>(); }
//     // void clear() {}
    
//     // BOOL hasInftyExponent() { return make_unknown<BOOL>(); }
//     // BOOL isDenormalized() { return make_unknown<BOOL>(); }
//   };

//   struct SoftHalfFloat
//   {
//     enum __FromRawBits__ { FromRawBits };
//     SoftHalfFloat( __FromRawBits__, U16 const& ) {}
//     SoftHalfFloat( SoftFloat const&, Flags const& ) {}
//     U16 queryRawBits() { return make_unknown<U16>(); }
//   };
  
//   static const unsigned int RN_NEAREST = 0;
//   static const unsigned int RN_ZERO = 1;
//   static const unsigned int RN_UP = 2;
//   static const unsigned int RN_DOWN = 3;

//   inline void GenSPEFSCR_FOVF(SPEFSCR& spefscr, const Flags& flags) { spefscr = make_unknown<U32>(); }
//   inline void GenSPEFSCR_FUNF(SPEFSCR& spefscr, const Flags& flags) { spefscr = make_unknown<U32>(); }
//   inline void GenSPEFSCR_FINXS(SPEFSCR& spefscr, const Flags& flags) { spefscr = make_unknown<U32>(); }
//   inline void GenSPEFSCR_FDBZ(SPEFSCR& spefscr, const Flags& flags) { spefscr = make_unknown<U32>(); }
//   inline void GenSPEFSCR_FG(SPEFSCR& spefscr, const Flags& flags, bool x=false) { spefscr = make_unknown<U32>(); }
//   inline void GenSPEFSCR_FX(SPEFSCR& spefscr, const Flags& flags, bool x=false) { spefscr = make_unknown<U32>(); }
//   inline void GenSPEFSCR_FG(SPEFSCR& spefscr, const SoftFloat& result) { spefscr = make_unknown<U32>(); }
//   inline BOOL HasSPEFSCR_InvalidInput(const SoftFloat& input) { return make_unknown<BOOL>(); }
//   template <class RESULT>
//   inline void GenSPEFSCR_DefaultResults(SPEFSCR& spefscr, RESULT& result) { spefscr = make_unknown<U32>(); }
//   template <class FLOAT>
//   inline void GenSPEFSCR_FINV(SPEFSCR& spefscr, FLOAT& first, FLOAT* second = 0, FLOAT* third = 0, bool x=false, bool y=false) { spefscr = make_unknown<U32>(); }

//   inline BOOL DoesSPEFSCR_TriggerException(const SPEFSCR& spefscr) { return BOOL(false); }
  
//   inline void GenSPEFSCR_TriggerException(CPU* cpu) { cpu->donttest_system(); }
  
//   extern void SignedAdd32(U32& result, U8& carry_out, U8& overflow, U8& sign, U32 x, U32 y, U8 carry_in);
//   extern inline void SignedAdd32(U32& result, U8& carry_out, U8& overflow, U8& sign, U32 x, U32 y, int carry_in)
//   { return SignedAdd32(result, carry_out, overflow, sign, x, y, U8(carry_in)); }

//   struct MaskNode : public unisim::util::symbolic::ExprNode
//   {
//     typedef unisim::util::symbolic::Expr Expr;
    
//     MaskNode( Expr const& _mb, Expr const& _me ) : mb(_mb), me(_me) {}
//     virtual MaskNode* Mutate() const { return new MaskNode(*this); }
//     virtual ScalarType::id_t GetType() const { return ScalarType::U32; }
//     virtual void Repr( std::ostream& sink ) const;
//     virtual unsigned SubCount() const { return 2; };
//     virtual Expr const& GetSub(unsigned idx) const { switch (idx) { case 0: return mb; case 1: return me; } return ExprNode::GetSub(idx); };
//     virtual intptr_t cmp( ExprNode const& brhs ) const
//     {
//       MaskNode const& rhs = dynamic_cast<MaskNode const&>( brhs );
//       if (intptr_t delta = mb.cmp( rhs.mb )) return delta;
//       return me.cmp( rhs.me );
//     }
//     Expr mb, me;
//   };
  
//   extern U32 Mask(U32 mb, U32 me);
  
// }

// #endif // ARCH_HH
