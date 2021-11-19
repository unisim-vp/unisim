/*
 *  Copyright (c) 2019-2021,
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

#ifndef __ARM64VP_ARCHITECTURE_HH__
#define __ARM64VP_ARCHITECTURE_HH__

#include "taint.hh"
#include "debug.hh"
#include <unisim/component/cxx/processor/arm/isa_arm64.hh>
#include <unisim/component/cxx/processor/arm/vmsav8/system.hh>
#include <unisim/component/cxx/processor/arm/regs64/cpu.hh>
#include <unisim/component/cxx/processor/arm/cp15.hh>
#include <unisim/component/cxx/processor/arm/exception.hh>
#include <unisim/component/cxx/processor/opcache/opcache.hh>
#include <unisim/component/cxx/vector/vector.hh>
#include <unisim/service/interfaces/registers.hh>
#include <unisim/service/interfaces/memory.hh>
#include <unisim/service/interfaces/memory_injection.hh>
#include <unisim/service/interfaces/debug_yielding.hh>
#include <unisim/service/interfaces/trap_reporting.hh>
#include <unisim/service/interfaces/symbol_table_lookup.hh>
#include <unisim/service/interfaces/linux_os.hh>
#include <unisim/service/interfaces/disassembly.hh>
#include <unisim/service/interfaces/memory_access_reporting.hh>
#include <unisim/service/interfaces/http_server.hh>
#include <unisim/util/netstreamer/netstreamer.hh>
#include <systemc>
#include <iosfwd>
#include <set>
#include <map>
#include <algorithm>
#include <inttypes.h>

void force_throw();

struct AArch64Types
{
  typedef TaintedValue< uint8_t> U8;
  typedef TaintedValue<uint16_t> U16;
  typedef TaintedValue<uint32_t> U32;
  typedef TaintedValue<uint64_t> U64;
  typedef TaintedValue<  int8_t> S8;
  typedef TaintedValue< int16_t> S16;
  typedef TaintedValue< int32_t> S32;
  typedef TaintedValue< int64_t> S64;

  typedef TaintedValue<   float> F32;
  typedef TaintedValue<  double> F64;
  
  typedef TaintedValue<  bool  > BOOL;

  template <typename T> using VectorTypeInfo = TaintedTypeInfo<T>;
  typedef U8 VUByte;
  struct VectorByteShadow { uint8_t data[sizeof (U8)]; };
};

struct AArch64
  : public virtual unisim::kernel::Object
  , public AArch64Types
  , public unisim::component::cxx::processor::arm::regs64::CPU<AArch64, AArch64Types>
  , public sc_core::sc_module
  , public unisim::kernel::Client<unisim::service::interfaces::DebugYielding>
  , public unisim::kernel::Client<unisim::service::interfaces::TrapReporting>
  , public unisim::kernel::Client<unisim::service::interfaces::SymbolTableLookup<uint64_t> >
  , public unisim::kernel::Client<unisim::service::interfaces::Memory<uint64_t> >
  , public unisim::kernel::Client<unisim::service::interfaces::LinuxOS>
  , public unisim::kernel::Client<unisim::service::interfaces::MemoryAccessReporting<uint64_t> >
  , public unisim::kernel::Service<unisim::service::interfaces::Registers>
  , public unisim::kernel::Service<unisim::service::interfaces::Memory<uint64_t> >
  , public unisim::kernel::Service<unisim::service::interfaces::Disassembly<uint64_t> >
  , public unisim::kernel::Service<unisim::service::interfaces::MemoryAccessReportingControl>
  , public unisim::kernel::Service<unisim::service::interfaces::MemoryInjection<uint64_t> >
  , public unisim::kernel::Service<unisim::service::interfaces::HttpServer>
{

  typedef U64 UREG;
  typedef U64 SREG;

  struct DisasmState {};

  enum { ZID=4 };

  typedef unisim::component::cxx::processor::arm::isa::arm64::Decoder<AArch64> A64Decoder;
  typedef unisim::component::cxx::processor::opcache::OpCache<A64Decoder> Decoder;
  typedef unisim::component::cxx::processor::arm::isa::arm64::Operation<AArch64> Operation;

  struct InstructionInfo
  {
    void assign( uint64_t _addr, uint64_t _counter, AArch64::Operation* _op ) { addr = _addr; counter = _counter; op = _op; }
    void dump(std::ostream& sink) const;
    friend std::ostream& operator << (std::ostream& sink, InstructionInfo const& ii) { ii.dump(sink); return sink; }
    uint64_t addr, counter;
    Operation* op;
  };

  SC_HAS_PROCESS(AArch64);
  AArch64(sc_core::sc_module_name const& name, Object* parent);
  ~AArch64();

  // // unisim::service::interfaces::Memory<uint64_t>
  // void ResetMemory() {}
  // bool ReadMemory(uint64_t addr, void* buffer, unsigned size);
  // bool WriteMemory(uint64_t addr, void const* buffer, unsigned size);
  
  // // unisim::service::interfaces::Registers
  // unisim::service::interfaces::Register* GetRegister(char const* name);
  // void ScanRegisters(unisim::service::interfaces::RegisterScanner& scanner);
  
  // // unisim::service::interfaces::MemoryInjection<ADDRESS>
  // bool InjectReadMemory(uint64_t addr, void *buffer, unsigned size);
  // bool InjectWriteMemory(uint64_t addr, void const* buffer, unsigned size);

  void UndefinedInstruction(unisim::component::cxx::processor::arm::isa::arm64::Operation<AArch64> const*);
  void UndefinedInstruction();

  InstructionInfo const& last_insn(int idx) const;
  void breakdance();
  void uninitialized_error(char const* rsrc);

  void TODO();

  enum AccessReport { report_simd_access=0, report_gsr_access=0, report_gzr_access=0 };
  void report(AccessReport, unsigned, bool) const {}

  /// index vector register access
  U8  GetTVU8(unsigned reg0, unsigned elements, unsigned regs, U8 const& index, U8 const& oob_value);

  //=====================================================================
  //=              Special/System Registers access methods              =
  //=====================================================================

  /** Set the value of PSTATE.<N,Z,C,V>
   *
   *  @param n the value to be assignd to PSTATE.<N>
   *  @param z the value to be assignd to PSTATE.<Z>
   *  @param c the value to be assignd to PSTATE.<C>
   *  @param v the value to be assignd to PSTATE.<V>
   */
  template <typename N, typename Z, typename C, typename V>
  void SetNZCV( N const& n, Z const& z, C const& c, V const& v )
  {
    nzcv = (U8(n) << 3) | (U8(z) << 2) | (U8(c) << 1) | (U8(v) << 0);
  }

  U8 GetNZCV() const { return nzcv; }
  U8 GetCarry() const { return (nzcv >> 1) & U8(1); }

  /** Get the current Program Counter */
  U64 GetPC() { return U64(current_insn_addr); }

  /** Get the next Program Counter */
  U64 GetNPC() { return U64(next_insn_addr); }

  /** Manage System Registers **/
  struct SysReg
  {
    static unsigned GetExceptionLevel( uint8_t op1 ) { switch (op1) { case 0: case 1: case 2: return 1; case 4: return 2; case 6: return 3; } return 0; }
    virtual void Write(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, AArch64& cpu, U64 value) const = 0;
    virtual U64 Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2,  AArch64& cpu) const = 0;
    virtual void DisasmRead(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, std::ostream& sink) const = 0;
    virtual void DisasmWrite(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, std::ostream& sink) const = 0;
  };

  static SysReg const* GetSystemRegister( uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2 );
  void                 CheckSystemAccess( uint8_t op1 );

  //=====================================================================
  //=                      Control Transfer methods                     =
  //=====================================================================

  /** Set the next Program Counter */
  void BranchTo( U64 addr, branch_type_t branch_type )
  {
    if (addr.ubits)
      {
        uninitialized_error("branch target");
        struct Bad {}; raise( Bad() );
      }
    next_insn_addr = addr.value;
  }
  bool concretize(bool);
  bool Test( bool cond ) { return cond; }
  // template <typename T> bool Test( TaintedValue<T> const& cond ) { BOOL c(cond); if (c.ubits) return concretize(); return cond.value; }
  bool Test( BOOL const& cond ) { if (cond.ubits) return concretize(cond.value); return cond.value; }
  void CallSupervisor( unsigned imm );
  void CallHypervisor( unsigned imm );

  //=====================================================================
  //=                       Memory access methods                       =
  //=====================================================================

  struct mem_acc_type { enum Code { write = 0, read, exec, cache_maintenance, debug } code; };
    
  template <typename T>
  T
  memory_read(unsigned el, U64 addr)
  {
    struct Bad {};
    if (addr.ubits) { uninitialized_error("address"); raise( Bad() ); }

    unsigned const size = sizeof (typename T::value_type);
    
    uint8_t dbuf[size], ubuf[size];

    if (get_page(addr.value).read(addr.value,&dbuf[0],&ubuf[0],size) != size)
      {
        for (unsigned byte = 0; byte < size; ++byte)
          {
            U8 u8 = memory_read<U8>(el, U64(addr.value+byte));
            dbuf[byte] = u8.value; ubuf[byte] = u8.ubits;
          }
      }
    
    typedef typename T::value_type value_type;
    typedef typename TX<value_type>::as_mask bits_type;

    bits_type value = 0, ubits = 0;
    for (unsigned idx = size; idx-- > 0;)
      {
        value <<= 8; value |= bits_type( dbuf[idx] );
        ubits <<= 8; ubits |= bits_type( ubuf[idx] );
      }
    
    ReportMemoryAccess(unisim::util::debug::MAT_READ, unisim::util::debug::MT_DATA, addr.value, size);
    
    return T(*reinterpret_cast<value_type const*>(&value), ubits);
  }

  void MemDump64(uint64_t addr);
  void PMemDump64(uint64_t addr);
  U64 MemRead64(U64 addr) { return memory_read<U64>(pstate.GetEL(), addr); }
  U32 MemRead32(U64 addr) { return memory_read<U32>(pstate.GetEL(), addr); }
  U16 MemRead16(U64 addr) { return memory_read<U16>(pstate.GetEL(), addr); }
  U8  MemRead8 (U64 addr) { return memory_read<U8> (pstate.GetEL(), addr); }

  void MemRead( U8* buffer, U64 addr, unsigned size );

  template <typename T>
  void
  memory_write(unsigned el, U64 addr, T src)
  {
    struct Bad {};
    if (addr.ubits) { uninitialized_error("address"); raise( Bad() ); }

    unsigned const size = sizeof (typename T::value_type);

    typedef typename TX<typename T::value_type>::as_mask bits_type;

    bits_type value = *reinterpret_cast<bits_type const*>(&src.value), ubits = src.ubits;
    uint8_t dbuf[size], ubuf[size];
    for (unsigned idx = 0; idx < sizeof (bits_type); ++idx)
      {
        dbuf[idx] = value & 0xff; value >>= 8;
        ubuf[idx] = ubits & 0xff; ubits >>= 8;
      }

    if (get_page(addr.value).write(addr.value,&dbuf[0],&ubuf[0],size) != size)
      {
        for (unsigned byte = 0; byte < size; ++byte)
          memory_write(el,U64(addr.value+byte),U8(dbuf[byte],ubuf[byte]));
      }

    ReportMemoryAccess(unisim::util::debug::MAT_WRITE, unisim::util::debug::MT_DATA, addr.value, size);
  }

  void MemWrite64(U64 addr, U64 val) { memory_write(pstate.GetEL(), addr, val); }
  void MemWrite32(U64 addr, U32 val) { memory_write(pstate.GetEL(), addr, val); }
  void MemWrite16(U64 addr, U16 val) { memory_write(pstate.GetEL(), addr, val); }
  void MemWrite8 (U64 addr, U8  val) { memory_write(pstate.GetEL(), addr, val); }

  void MemWrite( U64 addr, U8 const* buffer, unsigned size );

  void PrefetchMemory(unsigned op, U64 addr)
  {
    // No caches, nothing to do
  };

  void     SetExclusiveMonitors( U64 addr, unsigned size );
  bool     ExclusiveMonitorsPass( U64 addr, unsigned size );
  void     ClearExclusiveLocal(); /* TODO: handle global monitors */

  struct ExcMon
  {
    ExcMon() : addr(), size(), valid(false) {}
    void set(uint64_t _addr, unsigned _size) { valid = true; addr = _addr; size = _size; }
    bool pass(uint64_t _addr, unsigned _size) { return valid and size >= _size and (_addr & -size) == addr; }
    void clear() { valid = false; }

    uint64_t addr;
    uint8_t size;
    bool    valid;
  };

  //=====================================================================
  //=                            Exceptions                             =
  //=====================================================================

  struct Abort
  {
    virtual ~Abort() {}
    virtual void proceed( AArch64& cpu ) const {}
  };

  /* AArch32 obsolete arguments
   * - bool taketohypmode
   * - LDFSRformat
   * AArch32 left over arguments:
   * - domain    // Domain number, AArch32 only (UNKNOWN in AArch64)
   * - debugmoe  // Debug method of entry, from AArch32 only (UNKNOWN in AArch64)
   */
  void ExceptionReturn();

  /**********************************************************************
   ***                       Architectural state                      ***
   **********************************************************************/

  struct Zone
  {
    uint64_t hi() const { return last; }

    // bool operator < (Zone const& p) const { return last < p.base; }
    // bool operator > (Zone const& p) const { return p.last < base; }

    struct Above
    {
      using is_transparent = void;
      bool operator() (Zone const& a, Zone const& b) const { return a.base > b.last; }
      bool operator() (Zone const& a, uint64_t b) const { return a.base > b; }
    };

    Zone(uint64_t _base, uint64_t _last) : base(_base), last(_last) {}

    uint64_t size() const { return last - base + 1; }

    uint64_t    base;
    uint64_t    last;
  };

  struct Page : public Zone
  {
    struct Free
    {
      virtual ~Free() {};
      virtual void free (Page&) const {};
      static Free nop;
    };
    /* Standard buffer (de)allocation */
    Page(uint64_t base, uint64_t last);
    /* Custom buffer (de)allocation */
    Page(uint64_t _base, uint64_t _last, uint8_t* _data, uint8_t* _udat, Free* _free)
      : Zone(_base, _last), data(_data), udat(_udat), free(_free)
    {}
    Page(Page&& page)
      : Zone(page), data(page.data), udat(page.udat), free(page.free)
    {
      page.data = 0;
      page.udat = 0;
      page.free = &Free::nop;
    }
    Page() : Zone(0,0), data(0), udat(0), free(&Free::nop) {}
    Page( Page const& ) = delete;
    ~Page();

    uint64_t write(uint64_t addr, uint8_t const* dbuf, uint8_t const* ubuf, uint64_t count) const
    {
      uint64_t cnt = std::min(count,last-addr+1), start = addr-base;
      std::copy( &dbuf[0], &dbuf[cnt], &data[start] );
      std::copy( &ubuf[0], &ubuf[cnt], &udat[start] );
      return cnt;
    }
    uint64_t read(uint64_t addr, uint8_t* dbuf, uint8_t* ubuf, uint64_t count) const
    {
      uint64_t cnt = std::min(count,last-addr+1), start = addr-base;
      std::copy( &data[start], &data[start+cnt], &dbuf[0] );
      std::copy( &udat[start], &udat[start+cnt], &ubuf[0] );
      return cnt;
    }

    uint64_t size_from(uint64_t addr) const { return last - addr + 1; }
    uint8_t* data_beg() const { return &data[0]; }
    uint8_t* data_end() const { return &data[size()]; }
    uint8_t* udat_beg() const { return &udat[0]; }
    uint8_t* udat_end() const { return &udat[size()]; }
    uint8_t* data_abs(uint64_t addr) const { return &data[addr-base]; }
    uint8_t* udat_abs(uint64_t addr) const { return &udat[addr-base]; }
    uint8_t* data_rel(uint64_t offset) const { return &data[offset]; }
    uint8_t* udat_rel(uint64_t offset) const { return &udat[offset]; }
    
    bool has_data() const { return data; }
    void set_data(uint8_t* _data) { data = _data; }
    bool has_udat() const { return data; }
    void set_udat(uint8_t* _udat) { udat = _udat; }
    
    void dump_range(std::ostream&) const;

  private:
    uint8_t*    data;
    uint8_t*    udat;
    Page::Free* free;

    void resize(uint64_t last);
  };

  typedef std::set<Page, Page::Above> Pages;

  Page const& get_page( uint64_t addr )
  {
    auto pi = pages.lower_bound(addr);
    if (pi == pages.end() or pi->last < addr)
      return alloc_page(pi, addr);
    return *pi;
  }

  Page const& alloc_page(Pages::iterator pi, uint64_t addr);
  //  Page make_mem_page(uint64_t base, uint64_t last);

  void error_mem_overlap( Page const& a, Page const& b );

  bool mem_map(Page&& page);
  void mem_unmap(uint64_t base, uint64_t size);

  Operation* fetch_and_decode(uint64_t insn_addr);

  void run();

  struct IPB
  {
    static unsigned const LINE_SIZE = 32; //< IPB size
    uint8_t  bytes[LINE_SIZE];            //< The IPB content
    uint64_t base_address;                //< base address of IPB content (cache line size aligned if valid)
    IPB() : bytes(), base_address( -1 ) {}
    uint8_t* access(AArch64& core, uint64_t address);
  };

  struct PState
  {
    PState() : D(1), A(1), I(1), F(1), SS(0), IL(0), EL(1), SP(1) {}
    
    U64& selsp(AArch64& cpu) { return cpu.sp_el[SP ? EL : 0]; }
    U64 GetDAIF() const { return U64(D << 9 | A << 8 | I << 7 | F << 6); }
    void SetDAIF(AArch64& cpu, U64 const& xt)
    {
      if (xt.ubits) { struct Bad {}; raise( Bad() ); }
      D = xt.value>>9; A = xt.value>>8; I = xt.value>>7; F = xt.value>>6;
    }

    uint32_t AsSPSR() const;

    void SetSP(AArch64& cpu, unsigned sp)
    {
      selsp(cpu) = cpu.gpr[31];
      SP = sp;
      cpu.gpr[31] = selsp(cpu);
    }
    unsigned GetSP() const { return SP; }

    void SetEL(AArch64& cpu, unsigned el)
    {
      selsp(cpu) = cpu.gpr[31];
      EL = el;
      cpu.gpr[31] = selsp(cpu);
    }
    unsigned GetEL() const { return EL; }

    unsigned D  :  1;
    unsigned A  :  1;
    unsigned I  :  1;
    unsigned F  :  1;
    unsigned SS :  1;
    unsigned IL :  1;
  private:
    unsigned EL :  2;
    unsigned SP :  1;
  };

  U32 GetPSRFromPSTATE() { return U32(nzcv) << 28 | U32(pstate.AsSPSR()); }
  void SetPSTATEFromPSR(U32 spsr);

  struct EL
  {
    EL() : SPSR(), ESR(), ELR(), VBAR(), PAR(), FAR(), SCTLR() {}
    U32 SPSR, ESR;
    U64 ELR, VBAR, PAR;
    uint64_t FAR;
    uint32_t SCTLR;
    void SetSCTLR(AArch64& cpu, uint32_t value)
    {
      SCTLR = value;
    }
  };

  EL& get_el(unsigned level) { if (level != 1) { struct No {}; throw No {}; } return el1; }

  U64 RNDR();

  void wfi();


  struct UART
  {
    UART();
    
    uint16_t flags() const;
    uint16_t mis() const { return RIS & IMSC; }
    bool rx_pop(char& ch);
    bool rx_push();
    void tx_push(char ch);
    void tx_pop();
    
    enum { RX_INT = 0x10, TX_INT = 0x20 };
    static unsigned const qsize = 32;

    typedef unisim::util::netstreamer::NetStreamer NetStreamer;
    NetStreamer dterm;
    //HostTerm dterm;
    char rx_value;
    bool rx_valid;
    unsigned tx_count;
    uint16_t IBRD, FBRD, LCR, CR, IFLS, IMSC, RIS;
  };
  void handle_uart();  /* synchronous check of asynchronous events */
  void map_uart(uint64_t base_addr);

  struct Timer
  {
    Timer() : kctl(0), ctl(0), cval(0) {}
    bool activated() const;
    void program(AArch64& cpu);
    uint64_t get_cntfrq() const { return 33600000; }
    uint64_t get_pcount(AArch64& cpu) const { return cpu.insn_timer / (cpu.get_freq() / get_cntfrq()); }
    uint64_t read_ctl(AArch64& cpu) const { return ctl | ((read_tval(cpu) <= 0) << 2); }
    void write_ctl(AArch64& cpu, uint64_t v) { ctl = v & 3; program(cpu); }
    void write_kctl(AArch64& cpu, uint64_t v) { kctl = v & 0x203ff; program(cpu); }
    void write_cval(AArch64& cpu, uint64_t v) { cval = v; program(cpu); }
    int32_t read_tval(AArch64& cpu) const { return cval - get_pcount(cpu); }
    void write_tval(AArch64& cpu, uint64_t v) { cval = get_pcount(cpu) + int32_t(v); program(cpu); }
    uint32_t kctl; /* Kernel Control */
    uint8_t   ctl; /* Control */
    uint64_t cval; /* Compare Value*/
  };
  void handle_vtimer();
  uint64_t get_freq() const { return 1075200000; }

  void map_virtio_placeholder(unsigned id, uint64_t base_addr);
  void map_virtio_disk(char const* filename, uint64_t base_addr, unsigned irq);
  //  void map_virtio_console(uint64_t base_addr, unsigned irq);

  struct Suspend {};
  void handle_suspend();
  
  /* Simulation state */
public:
  std::map<std::string,unisim::service::interfaces::Register*> regmap;

  Pages    pages;
  ExcMon   excmon;

  IPB      ipb;
  Decoder  decoder;

  U32      fpcr, fpsr;

  U64      sp_el[2];
  EL       el1;
  PState   pstate;
  U8       nzcv;
  uint64_t current_insn_addr, next_insn_addr, insn_counter, insn_timer, next_quantum_timer, quantum_cycles;
  sc_core::sc_time quantum_delay;
  

  static unsigned const histsize = 1024;
  InstructionInfo last_insns[histsize];

  U64      TPIDR[2]; //<  Thread Pointer / ID Register
  U64      TPIDRRO;
  uint32_t CPACR;

  uint64_t bdaddr;
  uint32_t random;
  bool     terminate;
  bool     disasm;
  bool     suspend;  

  /**********************************************************************
   ***                  Service methods and members                   ***
   **********************************************************************/


  //=====================================================================
  //=                    Registers interface methods                    =
  //=====================================================================

  unisim::kernel::ServiceExport<unisim::service::interfaces::Registers> registers_export;
  virtual unisim::service::interfaces::Register* GetRegister( const char* name );
  virtual void ScanRegisters( unisim::service::interfaces::RegisterScanner& scanner );


  //=====================================================================
  //=              Memory interface methods (non intrusive)             =
  //=====================================================================

  unisim::kernel::ServiceExport<unisim::service::interfaces::Memory<uint64_t> > memory_export;
  virtual bool ReadMemory( uint64_t addr, void* buffer, uint32_t size );
  virtual bool WriteMemory( uint64_t addr, void const* buffer, uint32_t size );
  virtual void ResetMemory() {}

  //=====================================================================
  //=                   Disassembly interface methods                   =
  //=====================================================================

  unisim::kernel::ServiceExport<unisim::service::interfaces::Disassembly<uint64_t> > disasm_export;
  virtual std::string Disasm( uint64_t addr, uint64_t& next_addr );

  //=====================================================================
  //=             Memory access reporting interface methods             =
  //=====================================================================

  unisim::kernel::ServiceExport<unisim::service::interfaces::MemoryAccessReportingControl> memory_access_reporting_control_export;
  virtual void RequiresMemoryAccessReporting( unisim::service::interfaces::MemoryAccessReportingType type, bool report );
  bool  requires_memory_access_reporting = false;
  bool  requires_fetch_instruction_reporting = false;
  bool requires_commit_instruction_reporting = false;

  //=====================================================================
  //=                Memory injection interface methods                 =
  //=====================================================================

  unisim::kernel::ServiceExport<unisim::service::interfaces::MemoryInjection<uint64_t> > memory_injection_export;
  virtual bool InjectReadMemory( uint64_t addr, void* buffer, uint32_t size );
  virtual bool InjectWriteMemory( uint64_t addr, void const* buffer, uint32_t size );

  //=====================================================================
  //=                          Service imports                          =
  //=====================================================================

  unisim::kernel::ServiceImport<unisim::service::interfaces::DebugYielding>                    debug_yielding_import;
  unisim::kernel::ServiceImport<unisim::service::interfaces::TrapReporting>                    trap_reporting_import;
  unisim::kernel::ServiceImport<unisim::service::interfaces::SymbolTableLookup<uint64_t> >     symbol_table_lookup_import;
  unisim::kernel::ServiceImport<unisim::service::interfaces::Memory<uint64_t> >                memory_import;
  unisim::kernel::ServiceImport<unisim::service::interfaces::MemoryAccessReporting<uint64_t> > memory_access_reporting_import;
  unisim::kernel::ServiceImport<unisim::service::interfaces::LinuxOS>                          linux_os_import;

  void ReportMemoryAccess( unisim::util::debug::MemoryAccessType mat, unisim::util::debug::MemoryType mtp,
                           uint64_t addr, uint32_t size )
  {
    if (requires_memory_access_reporting and memory_access_reporting_import)
      memory_access_reporting_import->ReportMemoryAccess(mat, mtp, addr, size);
  }

  // Exports to HTTP server                                                                                                                                  
  unisim::kernel::ServiceExport<unisim::service::interfaces::HttpServer>                   http_server_export;

  // unisim::service::interfaces::HttpServer                                                                                                                 
  virtual bool ServeHttpRequest(unisim::util::hypapp::HttpRequest const& req, unisim::util::hypapp::ClientConnection const& conn);

  virtual void ScanWebInterfaceModdings(unisim::service::interfaces::WebInterfaceModdingScanner& scanner);
};

template <typename T>
T FPMulAdd(AArch64& cpu, T const& acc, T const& op1, T const& op2)
{
  return acc + (op1 * op2);
}

template <typename T>
T FPMulSub(AArch64& cpu, T const& acc, T const& op1, T const& op2)
{
  return acc - (op1 * op2);
}

#endif /* __ARM64VP_ARCHITECTURE_HH__ */
