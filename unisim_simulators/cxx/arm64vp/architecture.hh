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
#include "viodisk.hh"
#include "debug.hh"
#include "snapshot.hh"
#include <unisim/component/cxx/processor/arm/isa_arm64.hh>
#include <unisim/component/cxx/processor/arm/vmsav8/system.hh>
#include <unisim/component/cxx/processor/arm/cp15.hh>
#include <unisim/component/cxx/processor/arm/exception.hh>
#include <unisim/component/cxx/vector/vector.hh>
#include <unisim/service/interfaces/registers.hh>
#include <unisim/service/interfaces/memory.hh>
#include <unisim/service/interfaces/memory_injection.hh>
#include <unisim/util/netstreamer/netstreamer.hh>
#include <hostterm.hh>
#include <iosfwd>
#include <set>
#include <map>
#include <algorithm>
#include <inttypes.h>

void force_throw();

struct AArch64
  : public unisim::service::interfaces::Registers
  , public unisim::service::interfaces::Memory<uint64_t>
  , unisim::service::interfaces::MemoryInjection<uint64_t>
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

  typedef U64 UREG;
  typedef U64 SREG;

  typedef AArch64 DisasmState;

  enum { ZID=4 };

  typedef unisim::component::cxx::processor::arm::isa::arm64::Decoder<AArch64> Decoder;
  typedef unisim::component::cxx::processor::arm::isa::arm64::Operation<AArch64> Operation;

  struct InstructionInfo
  {
    void assign( uint64_t _addr, uint64_t _counter, AArch64::Operation* _op ) { addr = _addr; counter = _counter; op = _op; }
    void dump(std::ostream& sink) const;
    friend std::ostream& operator << (std::ostream& sink, InstructionInfo const& ii) { ii.dump(sink); return sink; }
    uint64_t addr, counter;
    Operation* op;
  };

  typedef void (AArch64::*event_handler_t)();

  AArch64();
  ~AArch64();

  // unisim::service::interfaces::Memory<uint64_t>
  void ResetMemory() {}
  bool ReadMemory(uint64_t addr, void* buffer, unsigned size);
  bool WriteMemory(uint64_t addr, void const* buffer, unsigned size);
  
  // unisim::service::interfaces::Registers
  unisim::service::interfaces::Register* GetRegister(char const* name);
  void ScanRegisters(unisim::service::interfaces::RegisterScanner& scanner);
  
  // unisim::service::interfaces::MemoryInjection<ADDRESS>
  bool InjectReadMemory(uint64_t addr, void *buffer, unsigned size);
  bool InjectWriteMemory(uint64_t addr, void const* buffer, unsigned size);

  void UndefinedInstruction(unisim::component::cxx::processor::arm::isa::arm64::Operation<AArch64> const*);
  void UndefinedInstruction();

  InstructionInfo const& last_insn(int idx) const;
  void breakdance();
  void uninitialized_error(char const* rsrc);

  void TODO();

  /** Get the value contained by a General-purpose or Stack Register.
   *
   * @param id the register index
   * @return the value contained by the register
   */
  U64 GetGSR(unsigned id) const
  {
    return gpr[id];
  }

  /** Get the value contained by a General-purpose or Zero Register.
   *
   * @param id the register index
   * @return the value contained by the register
   */
  U64 GetGZR(unsigned id) const
  {
    return (id != 31) ? gpr[id] : U64(0);
  }

  /** Set the value of a General-purpose or Stack Register
   *
   * @param id the register index
   * @param val the value to set
   */
  template <typename T>
  void SetGSR(unsigned id, T const& val)
  {
    gpr[id] = U64(val);
  }
	
  /** Set the value of a General-purpose or Zero Register
   *
   * @param id the register index
   * @param val the value to set
   */
  template <typename T>
  void SetGZR(unsigned id, T const& val)
  {
    if (id != 31) gpr[id] = U64( val );
  }

  //====================================================================
  //=                 Special  Registers access methods
  //====================================================================

  template <typename T>
  T vector_read(unsigned reg, unsigned sub)
  {
    return (vector_views[reg].GetConstStorage(&vectors[reg], T(), VUConfig::BYTECOUNT))[sub];
  }

  U8  GetVU8 ( unsigned reg, unsigned sub ) { return vector_read<U8> (reg, sub); }
  U16 GetVU16( unsigned reg, unsigned sub ) { return vector_read<U16>(reg, sub); }
  U32 GetVU32( unsigned reg, unsigned sub ) { return vector_read<U32>(reg, sub); }
  U64 GetVU64( unsigned reg, unsigned sub ) { return vector_read<U64>(reg, sub); }
  S8  GetVS8 ( unsigned reg, unsigned sub ) { return vector_read<S8> (reg, sub); }
  S16 GetVS16( unsigned reg, unsigned sub ) { return vector_read<S16>(reg, sub); }
  S32 GetVS32( unsigned reg, unsigned sub ) { return vector_read<S32>(reg, sub); }
  S64 GetVS64( unsigned reg, unsigned sub ) { return vector_read<S64>(reg, sub); }
  F32 GetVF32( unsigned reg, unsigned sub ) { return vector_read<F32>(reg, sub); }
  F64 GetVF64( unsigned reg, unsigned sub ) { return vector_read<F64>(reg, sub); }

  U8  GetTVU8(unsigned reg0, unsigned elements, unsigned regs, U8 const& index, U8 const& oob_value);
  
  template <typename T>
  void vector_write(unsigned reg, unsigned sub, T value )
  {
    (vector_views[reg].GetStorage(&vectors[reg], value, VUConfig::BYTECOUNT))[sub] = value;
  }

  void SetVU8 ( unsigned reg, unsigned sub, U8  value ) { vector_write( reg, sub, value ); }
  void SetVU16( unsigned reg, unsigned sub, U16 value ) { vector_write( reg, sub, value ); }
  void SetVU32( unsigned reg, unsigned sub, U32 value ) { vector_write( reg, sub, value ); }
  void SetVU64( unsigned reg, unsigned sub, U64 value ) { vector_write( reg, sub, value ); }
  void SetVS8 ( unsigned reg, unsigned sub, S8  value ) { vector_write( reg, sub, value ); }
  void SetVS16( unsigned reg, unsigned sub, S16 value ) { vector_write( reg, sub, value ); }
  void SetVS32( unsigned reg, unsigned sub, S32 value ) { vector_write( reg, sub, value ); }
  void SetVS64( unsigned reg, unsigned sub, S64 value ) { vector_write( reg, sub, value ); }
  void SetVF32( unsigned reg, unsigned sub, F32 value ) { vector_write( reg, sub, value ); }
  void SetVF64( unsigned reg, unsigned sub, F64 value ) { vector_write( reg, sub, value ); }

  template <typename T>
  void vector_write(unsigned reg, T value )
  {
    *(vector_views[reg].GetStorage(&vectors[reg], value, VUConfig::template TypeInfo<T>::bytecount)) = value;
  }

  void SetVU8 ( unsigned reg, U8 value )  { vector_write(reg, value); }
  void SetVU16( unsigned reg, U16 value ) { vector_write(reg, value); }
  void SetVU32( unsigned reg, U32 value ) { vector_write(reg, value); }
  void SetVU64( unsigned reg, U64 value ) { vector_write(reg, value); }
  void SetVS8 ( unsigned reg, S8 value )  { vector_write(reg, value); }
  void SetVS16( unsigned reg, S16 value ) { vector_write(reg, value); }
  void SetVS32( unsigned reg, S32 value ) { vector_write(reg, value); }
  void SetVS64( unsigned reg, S64 value ) { vector_write(reg, value); }
  void SetVF32( unsigned reg, F32 value ) { vector_write(reg, value); }
  void SetVF64( unsigned reg, F64 value ) { vector_write(reg, value); }
  
  void ClearHighV( unsigned reg, unsigned bytes )
  {
    vector_views[reg].Truncate(bytes);
  }

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
  enum branch_type_t { B_JMP = 0, B_CALL, B_RET, B_EXC, B_ERET, B_DBG };
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
    
  struct MMU
  {
    MMU() : MAIR_EL1(), TCR_EL1(), TTBR0_EL1(), TTBR1_EL1() {}

    uint64_t MAIR_EL1;
    uint64_t TCR_EL1;
    uint64_t TTBR0_EL1;
    uint64_t TTBR1_EL1;

    void sync(SnapShot&);
    unsigned GetASID() const
    {
      return (unisim::component::cxx::processor::arm::vmsav8::tcr::A1.Get(TCR_EL1) ? TTBR1_EL1 : TTBR0_EL1) >> 48;
    }

    // MMU() : ttbcr(), ttbr0(0), ttbr1(0), dacr() { refresh_attr_cache( false ); }
    // uint32_t ttbcr; /*< Translation Table Base Control Register */
    // uint32_t ttbr0; /*< Translation Table Base Register 0 */
    // uint32_t ttbr1; /*< Translation Table Base Register 1 */
    // uint32_t prrr;  /*< PRRR, Primary Region Remap Register */
    // uint32_t nmrr;  /*< NMRR, Normal Memory Remap Register */
    // uint32_t dacr;

    // uint16_t attr_cache[64];

    // void refresh_attr_cache( bool tre );

    struct TLB
    {
      struct Entry
      {
        uint64_t pa;
        uint32_t sh         : 2;  // 2
        uint32_t attridx    : 3;  // 5
        uint32_t domain     : 4;  // 9
        // <Permission>
        uint32_t ap         : 3;  // 12
        uint32_t xn         : 1;  // 13
        uint32_t pxn        : 1;  // 14
        // </Permission>
        uint32_t NS         : 1;  // 15
        uint32_t nG         : 1;  // 16
        uint32_t level      : 2;  // 18
        uint32_t blocksize  : 6;  // 24

        Entry() : pa(), sh(), attridx(), domain(), ap(), xn(), pxn(), NS(), nG(), level(), blocksize() {}
        Entry( uint64_t addr ) : pa(addr), sh(), attridx(), domain(), ap(), xn(), pxn(), NS(), nG(), level(), blocksize(48) {}
        uint64_t size_after() const { return -(uint64_t(-1) << blocksize | pa); }

        // uint32_t   asid     : 8;
        // Permissions perms,
        // bit nG,
        // bits(4) domain,
        // boolean contiguous,
        // integer level,
        // integer blocksize,
        // AddressDescriptor addrdesc
      };

      bool GetTranslation( Entry& tlbe, uint64_t vaddr, unsigned asid, bool update );
      void AddTranslation( Entry const& tlbe, uint64_t vaddr, unsigned asid );

      enum { khibit = 12, klobit = 5, kcount = 1 << (khibit-klobit) };
      void invalidate(bool nis, bool ll, unsigned cond, AArch64& cpu, U64 const& arg);

      TLB();

      /* 64-bit-keys:
       * asid[16] : varange[1] : input bits[36] : ?[4] : global[1] : significant[6]
       *   asid   :   va<48>   :   va<47:12>    :      :  !nG      :  blocksize-1
       */
      uint64_t keys[kcount];
      unsigned indices[kcount];
      Entry    entries[kcount];
    } tlb;
  };

  void translation_table_walk( MMU::TLB::Entry& entry, uint64_t vaddr, mem_acc_type::Code mat );

  void translate_address(MMU::TLB::Entry& entry, unsigned el, mem_acc_type::Code mat);
  
  void CheckPermission(MMU::TLB::Entry const& trans, uint64_t vaddress, unsigned el, mem_acc_type::Code mat);
  
  struct MemFault { MemFault(char const* _op) : op(_op) {} MemFault() : op() {} char const* op; };
  void memory_fault(MemFault const& mf, char const* operation, uint64_t vaddr, uint64_t paddr, unsigned size);
  
  template <typename T>
  T
  memory_read(unsigned el, U64 addr)
  {
    struct Bad {};
    if (addr.ubits) { uninitialized_error("address"); raise( Bad() ); }

    unsigned const size = sizeof (typename T::value_type);
    MMU::TLB::Entry entry( addr.value );
    translate_address(entry, el, mem_acc_type::read);
    
    uint8_t dbuf[size], ubuf[size];

    try
      {
        if (entry.size_after() < size or get_page(entry.pa).read(entry.pa,&dbuf[0],&ubuf[0],size) != size)
          {
            for (unsigned byte = 0; byte < size; ++byte)
              {
                U8 u8 = memory_read<U8>(el, U64(addr.value+byte));
                dbuf[byte] = u8.value; ubuf[byte] = u8.ubits;
              }
          }
      }
    catch (Device const& device)
      {
        if (not device.read(*this, entry.pa - device.base, &dbuf[0], &ubuf[0], size))
          throw MemFault("device");
      }
    catch (MemFault const& mf)
      {
        memory_fault(mf, "read", addr.value, entry.pa, size);
      }
    
    typedef typename T::value_type value_type;
    typedef typename TX<value_type>::as_mask bits_type;

    bits_type value = 0, ubits = 0;
    for (unsigned idx = size; idx-- > 0;)
      {
        value <<= 8; value |= bits_type( dbuf[idx] );
        ubits <<= 8; ubits |= bits_type( ubuf[idx] );
      }
    
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
    MMU::TLB::Entry entry( addr.value );
    translate_address(entry, el, mem_acc_type::write);

    typedef typename TX<typename T::value_type>::as_mask bits_type;

    bits_type value = *reinterpret_cast<bits_type const*>(&src.value), ubits = src.ubits;
    uint8_t dbuf[size], ubuf[size];
    for (unsigned idx = 0; idx < sizeof (bits_type); ++idx)
      {
        dbuf[idx] = value & 0xff; value >>= 8;
        ubuf[idx] = ubits & 0xff; ubits >>= 8;
      }

    try
      {
        if (entry.size_after() < size or get_page(entry.pa).write(entry.pa,&dbuf[0],&ubuf[0],size) != size)
          {
            for (unsigned byte = 0; byte < size; ++byte)
              memory_write(el,U64(addr.value+byte),U8(dbuf[byte],ubuf[byte]));
          }
      }
    catch (Device const& device)
      {
        if (not device.write(*this, entry.pa - device.base, &dbuf[0], &ubuf[0], size))
          memory_fault(MemFault("device"), "write", addr.value, entry.pa, size);
      }
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

  struct DataAbort : public Abort
  {
    typedef unisim::component::cxx::processor::arm::DAbort DAbort;
    DataAbort(DAbort _type, uint64_t _va, uint64_t _ipa, mem_acc_type::Code _mat, unsigned _level,
              bool _ipavalid, bool _secondstage, bool _s2fs1walk )
      : type(_type), va(_va), ipa(_ipa), mat(_mat), level(_level),
        ipavalid(_ipavalid), secondstage(_secondstage), s2fs1walk(_s2fs1walk)
    {}
    virtual void proceed( AArch64& cpu ) const override;
    unisim::component::cxx::processor::arm::DAbort type; uint64_t va; uint64_t ipa;  mem_acc_type::Code mat; unsigned level;
    bool ipavalid, secondstage, s2fs1walk;
  };
  
  /* AArch32 obsolete arguments
   * - bool taketohypmode
   * - LDFSRformat
   * AArch32 left over arguments:
   * - domain    // Domain number, AArch32 only (UNKNOWN in AArch64)
   * - debugmoe  // Debug method of entry, from AArch32 only (UNKNOWN in AArch64)
   */
  void TakePhysicalIRQException();
  void TakeException(unsigned target_el, unsigned vect_offset, uint64_t preferred_exception_return);

  void ExceptionReturn();

  /**********************************************************************
   ***                       Architectural state                      ***
   **********************************************************************/

  struct VUConfig
  {
    static unsigned const BYTECOUNT = 16;
    template <typename T> using TypeInfo = TaintedTypeInfo<T>;
    typedef U8 Byte;
  };
  static unsigned const VECTORCOUNT = 32;
  struct Vector { uint8_t data[sizeof (U8)][VUConfig::BYTECOUNT]; };

  typedef unisim::component::cxx::vector::VUnion<VUConfig> VecView;
  
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
    Page( SnapShot& snapshot ) : Zone(0,0), data(0), udat(0), free(&Free::nop)  { sync(snapshot); }
    ~Page();

    void sync( SnapShot& );
    void save( SnapShot& ) const;
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

  struct Device : public Zone
  {
    struct Effect;

    Device( uint64_t _base, uint64_t _last, Effect const* _effect, unsigned _id )
      : Zone( _base, _last )
      , effect( _effect ), id(_id)
    {}

    bool write(AArch64& arch, uint64_t addr, uint8_t const* dbuf, uint8_t const* ubuf, uint64_t count) const;
    bool read(AArch64& arch, uint64_t addr, uint8_t* dbuf, uint8_t* ubuf, uint64_t count) const;

    struct Request;

    struct Effect
    {
      virtual ~Effect() {};
      virtual bool access(AArch64& arch, Request& req) const = 0;
      virtual void get_name(unsigned dev_id, std::ostream& sink) const = 0;
      bool error(unsigned dev_id, char const* msg) const;
    };

    Effect const* effect;
    unsigned id;
  };


  typedef std::set<Page, Page::Above> Pages;
  typedef std::set<Device, Device::Above> Devices;

  // Page const& access_page( uint64_t addr )
  // {
  //   auto pi = pages.lower_bound(addr);
  //   if (pi == pages.end() or pi->last < addr)
  //     {
  //       auto di = devices.lower_bound(addr);
  //       if (di != devices.end() and addr <= di->last)
  //         throw *di;
  //       throw MemFault();
  //     }
  //   return *pi;
  // }
  // Page const& modify_page(uint64_t addr)
  Page const& get_page( uint64_t addr )
  {
    auto pi = pages.lower_bound(addr);
    if (pi == pages.end() or pi->last < addr)
      {
        auto di = devices.lower_bound(addr);
        if (di != devices.end() and addr <= di->last)
          throw *di;
        return alloc_page(pi, addr);
      }
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
      cpu.gic.program(cpu);
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
    void sync(SnapShot&);

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

  struct GIC
  {
    enum { ITLinesNumber = 2, ITLinesCount = 32*(ITLinesNumber+1) };
    GIC();

    void sync( SnapShot& );
    bool activated(AArch64& cpu) const;
    void program(AArch64& cpu);
    unsigned HighestPriorityPendingInterrupt() const { return ScanPendingInterruptsFor( C_PMR, 0 ); }
    bool HasPendingInterrupt() const { return ScanPendingInterruptsFor( C_PMR, C_PMR ) < ITLinesCount; }
    unsigned ScanPendingInterruptsFor( uint8_t required, uint8_t enough ) const;
    void set_interrupt(unsigned idx) { uint32_t word = idx/32, bit = (1ul << (idx%32)); D_IPENDING[word] |=  bit; }
    void ack_interrupt(unsigned idx) { uint32_t word = idx/32, bit = (1ul << (idx%32)); D_IPENDING[word] &= ~bit; D_IACTIVE[word] |= bit; }
    void eoi_interrupt(unsigned idx) { uint32_t word = idx/32, bit = (1ul << (idx%32)); D_IACTIVE [word] &= ~bit; }
    // CPU interface
    uint32_t C_CTLR;
    uint32_t C_PMR;
    // Distributor
    uint32_t D_CTLR;
    uint32_t D_IENABLE[ITLinesCount/32];
    uint32_t D_IPENDING[ITLinesCount/32];
    uint32_t D_IACTIVE[ITLinesCount/32];
    uint8_t  D_IPRIORITYR[ITLinesCount];
    uint32_t D_ICFGR[ITLinesCount/16];
  };
  void map_gic(uint64_t base_addr);
  bool has_irqs() const;
  void handle_irqs();
  void wfi();

  struct RTC
  {
    RTC() : LR(0), MR(), load_insn_time(0), started(false), masked(true) {}
    void     sync(SnapShot&);
    uint32_t get_counter(AArch64& cpu) const { return LR + get_gap(cpu); }
    uint64_t get_gap(AArch64& cpu) const { return (cpu.insn_timer - load_insn_time) / (cpu.get_freq() / get_cntfrq()); }
    uint64_t get_cntfrq() const { return 1; }
    void     flush_lr(AArch64& cpu) { load_insn_time = cpu.insn_timer; }
    void     program(AArch64& cpu);
    bool     matching(AArch64& cpu) { return get_counter(cpu) == MR; }

    uint32_t LR, MR;
    uint64_t load_insn_time;
    bool started;
    bool masked;
  };
  void map_rtc(uint64_t base_addr);
  void handle_rtc();

  struct UART
  {
    UART();
    
    void sync( SnapShot& );
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
    void sync( SnapShot& );
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
  
  void sync( SnapShot& );
  void save_snapshot( char const* );
  void load_snapshot( char const* );
  
  /* Simulation state */
public:
  typedef std::multimap<uint64_t, event_handler_t> Events;
  void silence( event_handler_t method );
  void notify( uint64_t delay, event_handler_t method );

private:
  //  struct NELock { NELock(AArch64& a); ~NELock(); pthread_mutex_t* m; };

  event_handler_t pop_next_event();
  void reload_next_event();
  
  Events   next_events;
  uint64_t next_event;
  //pthread_mutex_t next_event_mutex;
  
  /** Architectural state **/
public:
  std::map<std::string,unisim::service::interfaces::Register*> regmap;
  Devices     devices;
  GIC         gic;
  UART        uart;
  Timer       vt;
  RTC         rtc;
  VIODisk     viodisk;
  //VIOConsole  vioconsole; 

  Pages    pages;
  ExcMon   excmon;

  MMU      mmu;
  IPB      ipb;
  Decoder  decoder;

  U64      gpr[32];
  VecView  vector_views[VECTORCOUNT];
  Vector   vectors[VECTORCOUNT];
  U32      fpcr, fpsr;

  U64      sp_el[2];
  EL       el1;
  PState   pstate;
  U8       nzcv;
  uint64_t current_insn_addr, next_insn_addr, insn_counter, insn_timer;

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

  // /*QESCAPTURE*/
  // bool QESCapture();
  // struct QES { uint16_t desc; uint16_t flags; } qes[2];
  // void viocapture(uint64_t base, uint64_t size);
  // void checkvio(uint64_t base, unsigned size);
  // std::set<Zone, Zone::Above> diskpages;
};

template <typename T>
T FPMulAdd(AArch64& cpu, T const& acc, T const& op1, T const& op2)
{
  return acc + (op1 * op2);
}

#endif /* __ARM64VP_ARCHITECTURE_HH__ */