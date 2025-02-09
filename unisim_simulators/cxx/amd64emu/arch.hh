/*
 *  Copyright (c) 2022,
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

#ifndef __AMD64EMU_ARCH_HH__
#define __AMD64EMU_ARCH_HH__

//template <typename T> using VectorTypeInfo = unisim::component::cxx::processor::intel::VectorTypeInfo<T>;
#include <unisim/component/cxx/processor/intel/isa/intel.hh>
#include <unisim/component/cxx/processor/intel/types.hh>
#include <unisim/component/cxx/processor/intel/modrm.hh>
#include <unisim/component/cxx/memory/sparse/memory.hh>
#include <unisim/component/cxx/vector/vector.hh>
#include <unisim/service/interfaces/linux_os.hh>
#include <unisim/service/interfaces/memory_injection.hh>
#include <unisim/service/interfaces/memory.hh>
#include <unisim/service/interfaces/memory_access_reporting.hh>
#include <unisim/service/interfaces/registers.hh>
#include <unisim/service/interfaces/debug_yielding.hh>
#include <unisim/service/interfaces/disassembly.hh>
#include <unisim/kernel/variable/variable.hh>
#include <unisim/kernel/kernel.hh>
#include <unisim/util/debug/simple_register.hh>
#include <unisim/util/arithmetic/integer.hh>
#include <map>
#include <unisim/util/trace/callstack.hh>

template <typename A, unsigned S> using TypeFor = typename unisim::component::cxx::processor::intel::TypeFor<A,S>;

struct Arch
  : public unisim::kernel::Service<unisim::service::interfaces::MemoryInjection<uint64_t>>
  , public unisim::kernel::Service<unisim::service::interfaces::Memory<uint64_t>>
  , public unisim::kernel::Service<unisim::service::interfaces::Registers>
  , public unisim::kernel::Service<unisim::service::interfaces::Disassembly<uint64_t>>
  , public unisim::kernel::Service<unisim::service::interfaces::MemoryAccessReportingControl>
  , public unisim::kernel::Client<unisim::service::interfaces::MemoryAccessReporting<uint64_t>>
  , public unisim::kernel::Client<unisim::service::interfaces::DebugYielding>
{
  typedef uint8_t      u8_t;
  typedef uint16_t     u16_t;
  typedef uint32_t     u32_t;
  typedef uint64_t     u64_t;
  typedef unisim::util::arithmetic::Integer<4,false> u128_t;
  typedef int8_t       s8_t;
  typedef int16_t      s16_t;
  typedef int32_t      s32_t;
  typedef int64_t      s64_t;
  typedef unisim::util::arithmetic::Integer<4,true> s128_t;
  typedef bool         bit_t;
  typedef uint64_t     addr_t;
  typedef float        f32_t;
  typedef double       f64_t;
  typedef long double  f80_t;
  typedef u8_t         rc_t;

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
    OpHeader( addr_t _address ) : address( _address ) {} addr_t address;
  };

  Arch(char const* name, unisim::kernel::Object* parent, unisim::service::interfaces::LinuxOS*);

  ~Arch()
  {
    for (auto reg : regmap)
      delete reg.second;
    for (unsigned reg = 0; reg < 16; ++reg)
      umms[reg].Clear(&vmm_storage[reg][0]);
    for(unsigned idx = 0; idx < NUM_HASH_TABLE_ENTRIES; ++idx)
      delete hash_table[idx];
  }

  // unisim::service::interfaces::LinuxOS* GetLinuxOS() { return linux_os; }

  unisim::service::interfaces::LinuxOS* linux_os;
  std::map<std::string,unisim::service::interfaces::Register*> regmap;

  struct ClearMemSet { void operator() ( uint8_t* base, uintptr_t size ) const { __builtin_bzero(base, size); } };
  typedef typename unisim::component::cxx::memory::sparse::Memory<addr_t,15,15,ClearMemSet> Memory;
  Memory                      m_mem;

  // unisim::service::interfaces::Memory<addr_t>
  void ResetMemory() {}
  bool ReadMemory(addr_t addr, void* buffer, uint32_t size ) { m_mem.read( (uint8_t*)buffer, addr, size ); return true; }
  bool WriteMemory(addr_t addr, void const* buffer, uint32_t size) { m_mem.write( addr, (uint8_t*)buffer, size ); return true; }

  typedef unisim::component::cxx::processor::intel::RMOp<Arch> RMOp;

  // MEMORY STATE
  // Segment Registers
  uint16_t segregs[6];
  addr_t fs_base, gs_base;
  addr_t segbase(unsigned seg) const { switch(seg){ case 4: return fs_base; case 5: return gs_base; } return 0; }
  void                        segregwrite( unsigned idx, uint16_t value )
  {
    if (idx > 6) throw 0;
    segregs[idx] = value;
  }
  uint16_t                    segregread( unsigned idx )
  {
    throw 0;
    if (idx > 6) throw 0;
    return segregs[idx];
  }

  // Low level memory access routines
  enum LLAException_t { LLAError };
  void                        lla_memcpy( addr_t addr, uint8_t const* buf, addr_t size )
  { m_mem.write( addr, buf, size ); }
  void                        lla_memcpy( uint8_t* buf, addr_t addr, addr_t size )
  { m_mem.read( buf, addr, size ); }
  void                        lla_bzero( addr_t addr, addr_t size )
  { m_mem.clear( addr, size ); }
  template<class INT_t>
  void                        lla_memwrite( addr_t _addr, INT_t _val )
  {
    uintptr_t const int_size = sizeof( INT_t );

    addr_t last_addr = _addr, addr = _addr;
    typename Memory::Page* page = m_mem.getpage( addr );

    for (uintptr_t idx = 0; idx < int_size; ++idx, ++addr)
      {
        uint8_t byte = (_val >> (idx*8)) & 0xff;
        if ((last_addr ^ addr) >> Memory::Page::s_bits)
          page = m_mem.getpage( addr );
        addr_t offset = addr % (1 << Memory::Page::s_bits);
        page->m_storage[offset] = byte;
      }
  }

  template<class INT_t>
  INT_t                       lla_memread( addr_t _addr )
  {
    uintptr_t const int_size = sizeof( INT_t );

    addr_t last_addr = _addr, addr = _addr;
    typename Memory::Page* page = m_mem.getpage( addr );

    INT_t result = 0;

    for (uintptr_t idx = 0; idx < int_size; ++idx, ++ addr)
      {
        if ((last_addr ^ addr) >> Memory::Page::s_bits)
          page = m_mem.getpage( addr );
        addr_t offset = addr % (1 << Memory::Page::s_bits);
        result |= (INT_t( page->m_storage[offset] ) << (idx*8));
      }

    return result;

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
  f80_t                       fmemread80( unsigned int _seg, addr_t _addr );

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
    if (not rmop.ismem()) return f_type( this->fread( rmop.ereg() ) );
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
  void                        fmemwrite80( unsigned int _seg, u64_t _addr, f80_t _val );

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
    if (not rmop.ismem()) return fwrite( rmop.ereg(), f64_t( value ) );
    fpmemwrite<OPSIZE>( rmop->segment, rmop->effective_address( *this ), value );
  }


  template <unsigned OPSIZE>
  void
  memwrite( unsigned _seg, u64_t _addr, typename TypeFor<Arch,OPSIZE>::u _val )
  {
    uintptr_t const int_size = (OPSIZE/8);
    addr_t addr = _addr + segbase(_seg), last_addr = addr;

    typename Memory::Page* page = m_mem.getpage( addr );

    for (uintptr_t idx = 0; idx < int_size; ++idx, ++addr)
      {
        uint8_t byte = (_val >> (idx*8)) & 0xff;
        if ((last_addr ^ addr) >> Memory::Page::s_bits)
          page = m_mem.getpage( addr );
        addr_t offset = addr % (1 << Memory::Page::s_bits);
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


  template <class GOP>
  typename TypeFor<Arch,GOP::SIZE>::u
  rmread( GOP const& g, RMOp const& rmop )
  {
    if (not rmop.ismem())
      return regread( g, rmop.ereg() );

    return memread<GOP::SIZE>( rmop->segment, rmop->effective_address( *this ) );
  }

  template <class GOP>
  void
  rmwrite( GOP const&, RMOp const& rmop, typename TypeFor<Arch,GOP::SIZE>::u value )
  {
    if (not rmop.ismem())
      return regwrite( GOP(), rmop.ereg(), value );

    return memwrite<GOP::SIZE>( rmop->segment, rmop->effective_address( *this ), value );
  }

  template <unsigned OPSIZE>
  typename TypeFor<Arch,OPSIZE>::u
  memread( unsigned _seg, u64_t _addr )
  {
    uintptr_t const int_size = (OPSIZE/8);
    addr_t addr = _addr + segbase(_seg), last_addr = addr;
    typename Memory::Page* page = m_mem.getpage( addr );

    typedef typename TypeFor<Arch,OPSIZE>::u u_type;
    u_type result = u_type(0);

    for (uintptr_t idx = 0; idx < int_size; ++idx, ++ addr)
      {
        if ((last_addr ^ addr) >> Memory::Page::s_bits)
          page = m_mem.getpage( addr );
        addr_t offset = addr % (1 << Memory::Page::s_bits);
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
  void ScanRegisters(unisim::service::interfaces::RegisterScanner& scanner);

  // unisim::service::interfaces::MemoryInjection<ADDRESS>
  bool InjectReadMemory(addr_t addr, void *buffer, uint32_t size) { m_mem.read( (uint8_t*)buffer, addr, size ); return true; }
  bool InjectWriteMemory(addr_t addr, void const* buffer, uint32_t size) { m_mem.write( addr, (uint8_t*)buffer, size ); return true; }

  // unisim::service::interfaces::Disassembly<uint64_t>
  virtual std::string Disasm(uint64_t addr, uint64_t& next_addr) override;
  typedef unisim::kernel::ServiceExport<unisim::service::interfaces::Disassembly<uint64_t> > disasm_export_t;
  //  disasm_export_t disasm_export() { return disasm_export_t("disasm-export",this); };

  // unisim::service::interfaces::MemoryAccessReportingControl
  virtual void RequiresMemoryAccessReporting(unisim::service::interfaces::MemoryAccessReportingType type, bool report);
  // unisim::kernel::ServiceExport<unisim::service::interfaces::MemoryAccessReportingControl> memory_access_reporting_control_export;
  bool requires_memory_access_reporting;      //< indicates if the memory accesses require to be reported
  bool requires_fetch_instruction_reporting;  //< indicates if the fetched instructions require to be reported
  bool requires_commit_instruction_reporting; //< indicates if the committed instructions require to be reported

  // unisim::service::interfaces::MemoryAccessReporting<uint64_t>
  unisim::kernel::ServiceImport<unisim::service::interfaces::MemoryAccessReporting<uint64_t> > memory_access_reporting_import;

  // unisim::service::interfaces::DebugYielding
  unisim::kernel::ServiceImport<unisim::service::interfaces::DebugYielding> debug_yielding_import;


  // Implementation of ExecuteSystemCall

  void ExecuteSystemCall( unsigned id )
  {
    if (not linux_os)
      { throw std::logic_error( "No linux OS emulation connected" ); }
    linux_os->ExecuteSystemCall( id );
    // auto los = dynamic_cast<LinuxOS*>( linux_os );
    // los->LogSystemCall( id );
  }

  addr_t rip;
  unisim::util::trace::CallStack<unisim::util::trace::SimpleLoc<addr_t>> callstack;

  enum ipproc_t { ipjmp = 0, ipcall, ipret };
  addr_t                      getnip() { return rip; }
  void                        csupdate( ipproc_t ipproc, addr_t retaddr, addr_t dstaddr );
  void                        setnip( addr_t _rip, ipproc_t ipproc = ipjmp ) { csupdate(ipproc, rip, _rip); rip = _rip; }

  void                        syscall()
  {
    this->ExecuteSystemCall( this->regread( GR(), 0 ) );
  }

  void                        interrupt( uint8_t op, uint8_t code )
  {
    std::cerr << "Unhandled interruption (0x" << std::hex << unsigned( op ) << ", 0x" << unsigned( code ) << ").\n";
    exit( 0 );
  }

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
  bit_t                       flagread( FLAG::Code flag ) { return m_flags[flag]; }
  void                        flagwrite( FLAG::Code flag, bit_t fval )  { m_flags[flag] = fval; }
  void                        flagwrite( FLAG::Code flag, bit_t fval, bit_t def )  { m_flags[flag] = fval; }

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

  void fxam();

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

  u64_t tscread() { return instruction_count; }

public:
  struct VUConfig
  {
    static unsigned const BYTECOUNT = 32;
    template <typename T> using TypeInfo = unisim::component::cxx::vector::VectorTypeInfo<T,0>;
    typedef u8_t Byte;
  };
  static unsigned const VREGCOUNT = 16;

  unisim::component::cxx::vector::VUnion<VUConfig> umms[VREGCOUNT];
  uint8_t vmm_storage[16][VUConfig::BYTECOUNT];

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
    if (not rmop.ismem()) return vmm_read( vr, rmop.ereg(), sub, e );
    return vmm_memread( rmop->segment, rmop->effective_address( *this ) + sub*VUConfig::TypeInfo<ELEM>::bytecount, e );
  }

  template <class VR, class ELEM>
  void
  vmm_write( VR const& vr, RMOp const& rmop, unsigned sub, ELEM const& e )
  {
    if (not rmop.ismem()) return vmm_write( vr, rmop.ereg(), sub, e );
    return vmm_memwrite( rmop->segment, rmop->effective_address( *this ) + sub*VUConfig::TypeInfo<ELEM>::bytecount, e );
  }

  void xgetbv();
  void cpuid();
  void _DE() { std::cerr << "#DE: Division Error.\n"; throw std::runtime_error("diverr"); }
  void unimplemented() { std::cerr << "Unimplemented.\n"; throw 0; }

  void xsave(unisim::component::cxx::processor::intel::XSaveMode mode, bool is64, u64_t bv, RMOp const& rmop) { unimplemented(); }
  void xrstor(unisim::component::cxx::processor::intel::XSaveMode mode, bool is64, u64_t bv, RMOp const& rmop) { unimplemented(); }

  typedef unisim::component::cxx::processor::intel::Operation<Arch> Operation;
  Operation* latest_instruction;
  struct IPage
  {
    static unsigned const NUM_OPERATIONS_PER_PAGE = 0x1000;
    IPage* next;
    uint64_t key;
    Operation* operations[NUM_OPERATIONS_PER_PAGE];
    uint8_t bytes[NUM_OPERATIONS_PER_PAGE+15];

    IPage( IPage* _next, uint64_t _key ) : next( _next ), key( _key ), operations() {}
    ~IPage() { for( unsigned idx = 0; idx < NUM_OPERATIONS_PER_PAGE; ++idx ) delete operations[idx]; delete next; }
    Operation*& GetCached(uint64_t offset, uint8_t const* opbytes, unisim::component::cxx::processor::intel::Mode mode)
    {
      Operation*& cache_op = operations[offset];
      if (cache_op and (cache_op->mode != mode or memcmp( opbytes, &bytes[offset], cache_op->length ) != 0))
        {
          delete cache_op;
          cache_op = 0;
        }
      return cache_op;
    }
  };

  static unsigned const NUM_HASH_TABLE_ENTRIES = 0x1000;
  IPage* hash_table[NUM_HASH_TABLE_ENTRIES];
  IPage* mru_page;

  IPage*
  GetIPage( uint64_t address, uint64_t& offset )
  {
    uint64_t page_key = address / IPage::NUM_OPERATIONS_PER_PAGE;
    offset = address & (IPage::NUM_OPERATIONS_PER_PAGE-1);

    if (mru_page and (mru_page->key == page_key)) return mru_page;

    unsigned index = page_key % NUM_HASH_TABLE_ENTRIES; // hash the key

    IPage* cur = hash_table[index];
    IPage** ref = &hash_table[index];

    while (cur) {
      if (cur->key == page_key) {

        *ref = cur->next;
        cur->next = hash_table[index];
        return (mru_page = hash_table[index] = cur);
      }
      ref = &cur->next;
      cur = cur->next;
    }

    return (mru_page = hash_table[index] = new IPage( hash_table[index], page_key ));
  }

  Operation* fetch();
  Operation* Decode( unisim::component::cxx::processor::intel::Mode mode, uint64_t address, uint8_t* bytes );
  Operation* GetInsn( unisim::component::cxx::processor::intel::Mode mode, uint64_t address, uint8_t* bytes );

  void StepInstruction();

  void stop() { std::cerr << "Processor halt.\n"; throw 0; }

  struct Unimplemented {};
  void noexec( Operation const& op );

  bool enable_disasm;
  unisim::kernel::variable::Parameter<bool> param_enable_disasm;
  uint64_t instruction_count;

  bool Test( bool b ) const { return b; }
};

void eval_div( Arch& arch, uint64_t& hi, uint64_t& lo, uint64_t divisor );
void eval_div( Arch& arch, int64_t& hi, int64_t& lo, int64_t divisor );

Arch::f64_t sine( Arch::f64_t );
Arch::f64_t cosine( Arch::f64_t );
Arch::f64_t tangent( Arch::f64_t );
Arch::f64_t arctan( Arch::f64_t angle );
Arch::f64_t fmodulo( Arch::f64_t, Arch::f64_t );
Arch::f64_t firound( Arch::f64_t, int x87frnd_mode );
Arch::f64_t power( Arch::f64_t, Arch::f64_t );
Arch::f64_t logarithm( Arch::f64_t );
Arch::f64_t square_root( Arch::f64_t );


#endif /* __AMD64EMU_ARCH_HH__ */
