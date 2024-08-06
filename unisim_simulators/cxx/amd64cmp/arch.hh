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
#include <unisim/service/interfaces/registers.hh>
#include <unisim/service/interfaces/memory_injection.hh>
#include <unisim/service/interfaces/memory.hh>
#include <unisim/service/interfaces/linux_os.hh>
#include <unisim/kernel/variable/variable.hh>
#include <unisim/kernel/kernel.hh>
#include <unisim/util/debug/simple_register.hh>
#include <unisim/util/identifier/identifier.hh>
#include <unisim/util/arithmetic/integer.hh>
#include <set>
#include <map>

template <typename A, unsigned S> using TypeFor = typename unisim::component::cxx::processor::intel::TypeFor<A,S>;

struct Tracee;
struct Amd64LinuxOS;

struct Arch
  : public unisim::kernel::Service<unisim::service::interfaces::MemoryInjection<uint64_t>>
  , public unisim::kernel::Service<unisim::service::interfaces::Memory<uint64_t>>
  , public unisim::kernel::Service<unisim::service::interfaces::Registers>
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

  typedef void StrictCounterTest;

  struct OpHeader
  {
    OpHeader( addr_t _address ) : address( _address ) {} addr_t address;
  };

  typedef unisim::component::cxx::processor::intel::RMOp<Arch> RMOp;

  struct UpdateNode
  {
    virtual ~UpdateNode() {}
    virtual int cmp(UpdateNode const& rhs) const = 0;
    virtual void check(Arch& arch, Tracee const& tracee) const = 0;
  };
  struct Update
  {
    Update(UpdateNode const* _node) : node(_node) {}
    Update(Update&& other) : node(other.node) { other.node = 0; }
    Update(Update const&) = delete;
    ~Update() { delete node; }

    UpdateNode const* operator -> () const { return node; }

    int compare(Update const& rhs) const;

    bool operator <(Update const& rhs) const { return compare(rhs) < 0; }

    UpdateNode const* node;
  };

  Arch(char const* name, unisim::kernel::Object* parent, Tracee const& tracee, Amd64LinuxOS& linux_os);

  ~Arch()
  {
    for (auto reg : regmap)
      delete reg;
    for (unsigned reg = 0; reg < 16; ++reg)
      umms[reg].Clear(&vmm_storage[reg][0]);
    for(unsigned idx = 0; idx < NUM_HASH_TABLE_ENTRIES; ++idx)
      delete hash_table[idx];
  }

  struct RegisterMapOrdering
  {
    using reg_p = unisim::service::interfaces::Register*;
    using is_transparent = void;
    int cmp(char const* a, char const* b) const;
    bool operator() (reg_p a, reg_p b) const { return cmp(a->GetName(), b->GetName()) < 0; }
    bool operator() (reg_p a, char const* b) const { return cmp(a->GetName(), b) < 0; }
    bool operator() (char const* a, reg_p b) const { return cmp(a, b->GetName()) < 0; }
    //    bool operator() (reg_p a, std::string const& b) const { return b.compare(a->GetName()) > 0; }
  };

  std::set<unisim::service::interfaces::Register*,RegisterMapOrdering> regmap;

  // unisim::service::interfaces::Registers
  unisim::service::interfaces::Register* GetRegister(char const* name);
  void ScanRegisters(unisim::service::interfaces::RegisterScanner& scanner);

  // unisim::service::interfaces::MemoryInjection<ADDRESS>
  bool InjectReadMemory(addr_t addr, void *buffer, uint32_t size);
  bool InjectWriteMemory(addr_t addr, void const* buffer, uint32_t size);

  // unisim::service::interfaces::Memory<addr_t>
  void ResetMemory();
  bool ReadMemory(addr_t addr, void* buffer, uint32_t size );
  bool WriteMemory(addr_t addr, void const* buffer, uint32_t size);

  // Reference Tracee
  Tracee const& tracee;

  // Linux OS Emulation
  Amd64LinuxOS& linux_os;

  // Segment Registers
  uint16_t segregs[6];
  addr_t fs_base, gs_base;
  addr_t segbase(unsigned seg) const { switch(seg){ case 4: return fs_base; case 5: return gs_base; } return 0; }
  addr_t& sbase(char seg) { switch(seg) { default: throw 0; case 'f': return fs_base; case 'g': return gs_base; } return fs_base; }
  addr_t                      sbaseread( char seg ) { return sbase(seg); }
  void                        sbasewrite( char seg, addr_t addr ) { sbase(seg) = addr; sbasecheck(seg); }
  void                        sbasecheck( char seg );
  void                        segregwrite( unsigned idx, uint16_t value )
  {
    throw 0;
    if (idx > 6) throw 0;
    segregs[idx] = value;
  }
  uint16_t                    segregread( unsigned idx )
  {
    throw 0;
    if (idx > 6) throw 0;
    return segregs[idx];
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

  struct MCUpdate : public UpdateNode { void memcheck(Tracee const& tracee, u64_t addr, uint8_t const* bytes, unsigned size) const; };

  template <unsigned OPSIZE>
  void
  memwrite( unsigned seg, u64_t addr, typename TypeFor<Arch,OPSIZE>::u val )
  {
    struct MWUpdate : public MCUpdate
    {
      MWUpdate(u64_t _addr, typename TypeFor<Arch,OPSIZE>::u val) : addr(_addr) { memcpy((void*)&bytes[0], (void const*)&val, OPSIZE/8); }
      virtual void check(Arch& arch, Tracee const& tracee) const override { memcheck(tracee, addr, &bytes[0], OPSIZE/8); }
      virtual int cmp(UpdateNode const& rhs) const override { return compare(dynamic_cast<MWUpdate const&>(rhs)); }
      virtual int compare(MWUpdate const& rhs) const
      {
        for (unsigned idx = 0, end = OPSIZE/8; idx < end; ++idx)
          if (int delta = int(bytes[idx]) - int(rhs.bytes[idx]))
            return delta;
        return addr < rhs.addr ? -1 : addr > rhs.addr ? +1 : 0;
      }
      u64_t addr;
      uint8_t bytes[OPSIZE/8];
    };

    if (accurate)
      updates.insert(new MWUpdate(addr + segbase(seg), val));
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

  void MemRead( uint8_t* bytes, uint64_t addr, unsigned size ) const;

  template <unsigned OPSIZE>
  typename TypeFor<Arch,OPSIZE>::u
  memread( unsigned seg, u64_t addr )
  {
    unsigned const size = OPSIZE/8;
    uint8_t bytes[size];
    MemRead( (uint8_t*)&bytes[0], addr + segbase(seg), size );
    typedef typename TypeFor<Arch,OPSIZE>::u u_type;
    u_type result = u_type( 0 );
    memcpy((void*)&result, (void const*)&bytes[0], OPSIZE/8);
    return result;
  }

  // Implementation of ExecuteSystemCall
  void ExecuteSystemCall( unsigned id );

  addr_t rip;

  enum ipproc_t { ipjmp = 0, ipcall, ipret };
  addr_t                      getnip() { return rip; }
  void                        setnip( addr_t _rip, ipproc_t ipproc = ipjmp ) { rip = _rip; }

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

  void regcheck(unsigned idx);

  template <class GOP>
  void regwrite( GOP const&, unsigned idx, typename TypeFor<Arch,GOP::SIZE>::u value )
  {
    u64regs[idx] = u64_t( value );
    regcheck(idx);
  }

  void regwrite( GObLH const&, unsigned idx, u8_t value )
  {
    unsigned reg = idx%4, sh = idx*2 & 8;
    u64regs[reg] = (u64regs[reg] & ~u64_t(0xff << sh)) | ((value & u64_t(0xff)) << sh);
    regcheck(reg);
  }
  void regwrite( GOb const&, unsigned idx, u8_t value )
  {
    u64regs[idx] = (u64regs[idx] & ~u64_t(0xff)) | ((value & u64_t(0xff)));
    regcheck(idx);
  }
  void regwrite( GOw const&, unsigned idx, u16_t value )
  {
    u64regs[idx] = (u64regs[idx] & ~u64_t(0xffff)) | ((value & u64_t(0xffff)));
    regcheck(idx);
  }

  struct GPRPatch : public UpdateNode
  {
    GPRPatch(unsigned _reg, uint64_t and_m, uint64_t or_m)  : reg(_reg), and_mask(and_m), or_mask(or_m) {} unsigned reg; uint64_t and_mask, or_mask;
    virtual void check(Arch& arch, Tracee const& tracee) const override;
    virtual int cmp(UpdateNode const& rhs) const override { return compare(dynamic_cast<GPRPatch const&>(rhs)); }
    virtual int compare(GPRPatch const& rhs) const;
  };

  struct FLAG : public unisim::util::identifier::Identifier<FLAG>
  {
    enum Code {
      CF = 0, PF,     AF,     ZF,     SF,     DF,     OF,
      C0,     C1,     C2,     C3,
      end
    } code;
    char const* c_str() const
    {
      switch (code)
        {
        default: break;
        case    CF: return "cf";
        case    PF: return "pf";
        case    AF: return "af";
        case    ZF: return "zf";
        case    SF: return "sf";
        case    DF: return "df";
        case    OF: return "of";
        case    C0: return "c0";
        case    C1: return "c1";
        case    C2: return "c2";
        case    C3: return "c3";
        }
      return "NA";
    }

    FLAG() : code(end) {}
    FLAG( Code _code ) : code(_code) {}
    FLAG( char const* _code ) : code(end) { init( _code ); }
  };

  bool                        m_flags[FLAG::end];
  uint32_t                    flagmask;
  static bool is_baseflags(FLAG flag) { switch (flag.code) { case FLAG::CF: case FLAG::PF: case FLAG::AF: case FLAG::ZF: case FLAG::SF: case FLAG::DF: case FLAG::OF: return true; default: break; } return false; }
  static bool is_x87flags(FLAG flag) { switch (flag.code) { case FLAG::C0: case FLAG::C1: case FLAG::C2: case FLAG::C3: return true; default: break; } return false; }

  void                        flagcheck( FLAG flag );
  bit_t                       flagread( FLAG flag ) { return m_flags[flag.idx()]; }
  void                        flagwrite( FLAG flag, bit_t fval )
  {
    m_flags[flag.idx()] = fval;
    if (is_baseflags(flag))
      flagcheck(flag);
    else
      throw 0;
  }
  void                        flagwrite( FLAG flag, bit_t fval, bit_t defined );

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

  u64_t tscread() { accurate = false; return instruction_count; }

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

  void vmm_regcheck(unsigned idx);

  template <class VR, class ELEM>
  void
  vmm_write( VR const& vr, unsigned reg, unsigned sub, ELEM const& e )
  {
    ELEM* elems = umms[reg].GetStorage( &vmm_storage[reg][0], e, vmm_wsize( vr ) );
    elems[sub] = e;
    vmm_regcheck(reg);
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

  void xsave(unisim::component::cxx::processor::intel::XSaveMode mode, bool is64, u64_t bv, RMOp const& rmop);
  void xrstor(unisim::component::cxx::processor::intel::XSaveMode mode, bool is64, u64_t bv, RMOp const& rmop);

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

  bool enable_disasm, accurate;
  uint64_t instruction_count;

  bool Test( bool b ) const { return b; }

  std::set<Update> updates;
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
