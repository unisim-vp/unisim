/*
 *  Copyright (c) 2019-2020,
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
#include <unisim/component/cxx/processor/arm/isa_arm64.hh>
#include <iosfwd>
#include <set>
#include <algorithm>
#include <inttypes.h>

struct AArch64
{
  typedef TaintedValue< uint8_t> U8;
  typedef TaintedValue<uint16_t> U16;
  typedef TaintedValue<uint32_t> U32;
  typedef TaintedValue<uint64_t> U64;
  typedef TaintedValue<  int8_t> S8;
  typedef TaintedValue< int16_t> S16;
  typedef TaintedValue< int32_t> S32;
  typedef TaintedValue< int64_t> S64;
  
  AArch64();

  void UndefinedInstruction(unisim::component::cxx::processor::arm::isa::arm64::Operation<AArch64>*);

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
  void SetGSR(unsigned id, U64 val)
  {
    gpr[id] = val;
  }
	
  /** Set the value of a General-purpose or Zero Register
   *
   * @param id the register index
   * @param val the value to set
   */
  void SetGZR(unsigned id, U64 val)
  {
    if (id != 31) gpr[id] = val;
  }
  
  //====================================================================
  //=                 Special  Registers access methods                 
  //====================================================================

  U8  GetVU8 ( unsigned reg, unsigned sub ) { return VectorStorage<U8> (reg)[sub]; }
  U16 GetVU16( unsigned reg, unsigned sub ) { return VectorStorage<U16>(reg)[sub]; }
  U32 GetVU32( unsigned reg, unsigned sub ) { return VectorStorage<U32>(reg)[sub]; }
  U64 GetVU64( unsigned reg, unsigned sub ) { return VectorStorage<U64>(reg)[sub]; }
  S8  GetVS8 ( unsigned reg, unsigned sub ) { return VectorStorage<S8> (reg)[sub]; }
  S16 GetVS16( unsigned reg, unsigned sub ) { return VectorStorage<S16>(reg)[sub]; }
  S32 GetVS32( unsigned reg, unsigned sub ) { return VectorStorage<S32>(reg)[sub]; }
  S64 GetVS64( unsigned reg, unsigned sub ) { return VectorStorage<S64>(reg)[sub]; }
  
  void SetVU8 ( unsigned reg, unsigned sub, U8  value ) { VectorStorage<U8> (reg)[sub] = value; }
  void SetVU16( unsigned reg, unsigned sub, U16 value ) { VectorStorage<U16>(reg)[sub] = value; }
  void SetVU32( unsigned reg, unsigned sub, U32 value ) { VectorStorage<U32>(reg)[sub] = value; }
  void SetVU64( unsigned reg, unsigned sub, U64 value ) { VectorStorage<U64>(reg)[sub] = value; }
  void SetVS8 ( unsigned reg, unsigned sub, S8  value ) { VectorStorage<S8> (reg)[sub] = value; }
  void SetVS16( unsigned reg, unsigned sub, S16 value ) { VectorStorage<S16>(reg)[sub] = value; }
  void SetVS32( unsigned reg, unsigned sub, S32 value ) { VectorStorage<S32>(reg)[sub] = value; }
  void SetVS64( unsigned reg, unsigned sub, S64 value ) { VectorStorage<S64>(reg)[sub] = value; }
  
  void SetVU8 ( unsigned reg, U8 value )  { VectorZeroedStorage<U8> (reg)[0] = value; }
  void SetVU16( unsigned reg, U16 value ) { VectorZeroedStorage<U16>(reg)[0] = value; }
  void SetVU32( unsigned reg, U32 value ) { VectorZeroedStorage<U32>(reg)[0] = value; }
  void SetVU64( unsigned reg, U64 value ) { VectorZeroedStorage<U64>(reg)[0] = value; }
  void SetVS8 ( unsigned reg, S8 value )  { VectorZeroedStorage<S8> (reg)[0] = value; }
  void SetVS16( unsigned reg, S16 value ) { VectorZeroedStorage<S16>(reg)[0] = value; }
  void SetVS32( unsigned reg, S32 value ) { VectorZeroedStorage<S32>(reg)[0] = value; }
  void SetVS64( unsigned reg, S64 value ) { VectorZeroedStorage<S64>(reg)[0] = value; }
  
  void ClearHighV( unsigned reg, unsigned bytes ) { for (unsigned idx = bytes; idx < VUnion::BYTECOUNT; idx+=1 ) vector_data[reg][idx] = 0; }

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
  void SetNZCV( U32 n, U32 z, U32 c, U32 v )
  {
    nzcv = (n << 3) | (z << 2) | (c << 1) | (v << 0);
  }
  
  U8 GetNZCV() const { return nzcv; }
  U8 GetCarry() const { return (nzcv >> 1) & 1; }
  
  /** Get the current Program Counter */
  U64 GetPC() { return current_insn_addr; }
  
  /** Get the next Program Counter */
  U64 GetNPC() { return next_insn_addr; }
  
  void        CheckSystemAccess( U8 op1 );
  U64    ReadSystemRegister( U8 op0, U8 op1, U8 crn, U8 crm, U8 op2 );
  void        WriteSystemRegister( U8 op0, U8 op1, U8 crn, U8 crm, U8 op2, U64 value );
  void        DescribeSystemRegister( U8 op0, U8 op1, U8 crn, U8 crm, U8 op2, std::ostream& sink );
  void        NameSystemRegister( U8 op0, U8 op1, U8 crn, U8 crm, U8 op2, std::ostream& sink );
  
  //=====================================================================
  //=                      Control Transfer methods                     =
  //=====================================================================
  
  /** Set the next Program Counter */
  enum branch_type_t { B_JMP = 0, B_CALL, B_RET };
  void BranchTo( U64 addr, branch_type_t branch_type ) { next_insn_addr = addr; }
  bool Test( bool cond ) { return cond; }
  void CallSupervisor( U32 imm );
  
  //=====================================================================
  //=                       Memory access methods                       =
  //=====================================================================
  
  template <typename T>
  T
  MemReadT(U64 addr)
  {
    unsigned const size = sizeof (T);
    U8 buffer[size];
    MemRead( &buffer[0], addr, size );
    T res(0);
    for (unsigned idx = size; --idx < size; )
      res = (res << 8) | T(buffer[idx]);
    return res;
  }

  U64 MemRead64(U64 addr) { return MemReadT<U64>(addr); }
  U32 MemRead32(U64 addr) { return MemReadT<U32>(addr); }
  uint16_t MemRead16(U64 addr) { return MemReadT<uint16_t>(addr); }
  U8  MemRead8 (U64 addr) { return MemReadT<U8> (addr); }
  
  void MemRead( U8* buffer, U64 addr, unsigned size );
  
  template <typename T>
  void
  MemWriteT(U64 addr, T val)
  {
    unsigned const size = sizeof (T);
    U8 buffer[size];
    for (unsigned idx = 0; idx < size; ++idx)
      { buffer[idx] = val; val >>= 8; }
    MemWrite( addr, &buffer[0], size );
  }
  
  void MemWrite64(U64 addr, U64 val) { MemWriteT(addr, val); }
  void MemWrite32(U64 addr, U32 val) { MemWriteT(addr, val); }
  void MemWrite16(U64 addr, uint16_t val) { MemWriteT(addr, val); }
  void MemWrite8 (U64 addr, U8  val) { MemWriteT(addr, val); }
  
  void MemWrite( U64 addr, U8 const* buffer, unsigned size );

  void     SetExclusiveMonitors( U64 addr, unsigned size ) { /*TODO: MP support*/ }
  bool     ExclusiveMonitorsPass( U64 addr, unsigned size ) { /*TODO: MP support*/ return true; }
  void     ClearExclusiveLocal() {}
  
  /**********************************************************************
   ***                       Architectural state                      ***
   **********************************************************************/
  
  struct SysReg
  {
    virtual            ~SysReg() {}
    virtual void        Write( AArch64& cpu, U64 value ) const {
      cpu.logger << unisim::kernel::logger::DebugWarning << "Writing " << Describe() << unisim::kernel::logger::EndDebugWarning;
      throw 0; // cpu.UnpredictableInsnBehaviour();
    }
    virtual U64    Read( AArch64& cpu ) const {
      cpu.logger << unisim::kernel::logger::DebugWarning << "Reading " << Describe() << unisim::kernel::logger::EndDebugWarning;
      throw 0; // cpu.UnpredictableInsnBehaviour();
      return 0;
    }
    virtual char const* Describe() const = 0;
    virtual char const* Name() const = 0;
  };
  
  virtual SysReg const&  GetSystemRegister( U8 op0, U8 op1, U8 crn, U8 crm, U8 op2 );
  virtual void     ResetSystemRegisters();
  
  static unsigned const VECTORCOUNT = 32;
  

  struct 
  U8 vector_data[VECTORCOUNT][VUnion::BYTECOUNT];
  
  template <typename T> T* VectorStorage( unsigned reg ) { return vector_view[reg].template GetStorage<T>( &vector_data[reg][0] ); }
  template <typename T> T* VectorZeroedStorage( unsigned reg ) { return vector_view[reg].template GetZeroedStorage<T>( &vector_data[reg][0] ); }
  
  struct Page
  {
    U64 hi() const { return last; }

    bool operator < (Page const& p) const { return last < p.base; }
    bool operator > (Page const& p) const { return p.last < base; }

    struct Above
    {
      using is_transparent = void;
      bool operator() (Page const& a, Page const& b) const { return a > b; }
      bool operator() (Page const& a, U64 b) const { return a.base > b; }
    };
    struct Free
    {
      virtual ~Free() {};
      virtual void free (Page&) const {};
      static Free nop;
    };
    Page( U64 _base, U64 _last, U8* _data, Free* _free )
      : base(_base), last(_last), data(_data), free(_free)
    {}
    Page( Page&& page )
      : base(page.base), last(page.last), data(page.data), free(page.free)
    {
      page.data = 0;
      page.free = &Free::nop;
    }
    Page( Page const& ) = delete;
    ~Page();
    
    U64 write(U64 addr, U8 const* bytes, U64 count) const
    {
      U64 cnt = std::min(count,last-addr+1), start = addr-base;
      std::copy( &bytes[0], &bytes[cnt], &data[start] );
      return cnt;
    }
    bool write(U64 addr, unsigned count, unsigned endianness, U64 value) const
    {
      U8 bytes[8];
      for (unsigned idx = 0; idx < count; ++idx)
        { bytes[idx^endianness] = value; value >>= 8; }
      return write( addr, &bytes[0], count ) == count;
    }
    U64 read(U64 addr, U8* bytes, U64 count) const
    {
      U64 cnt = std::min(count,last-addr+1), start = addr-base;
      std::copy( &data[start], &data[start+cnt], &bytes[0] );
      return cnt;
    }
    bool read(U64 addr, unsigned count, unsigned endianness, U64* value) const
    {
      U8 bytes[8];
      if (read(addr, &bytes[0], count) != count) return false;
      U64 res = 0;
      for (unsigned idx = count; idx-- != 0;)
        { res <<= 8; res |= bytes[idx^endianness]; }
      *value = res;
      return true;
    }
    bool has_data() const { return data; }
    U8 const* get_data() const { return data; }
    U64 size() const { return last - base + 1; }

    // void dump(std::ostream&) const;
    // friend std::ostream& operator << ( std::ostream& sink, Page const& p ) { p.dump(sink); return sink; }


  public:
    U64    base;
    U64    last;
  private:
    U8*    data;
    Page::Free* free;

    void resize(U64 last);
  };

  typedef std::set<Page, Page::Above> Pages;

  Page const& access_page( U64 addr )
  {
    auto pi = pages.lower_bound(addr);
    if (pi == pages.end() or pi->last < addr)
      { struct Bad {}; throw Bad(); }
    return *pi;
  }
  
  void free_read_memory(U64 address, U8* buffer, unsigned size)
  {
    Page const& page = access_page(address);
    page.read(address, buffer, size);
  }

  struct IPB
  {
    static unsigned const LINE_SIZE = 32; //< IPB size
    U8 bytes[LINE_SIZE];             //< The IPB content
    U32 base_address;                //< base address of IPB content (cache line size aligned if valid)
    IPB() : bytes(), base_address( -1 ) {}
    U8* get(AArch64& core, U64 address);
  };
  
  void error_mem_overlap( Page const& a, Page const& b );
  
  bool mem_map(U64 addr, U64 size);
  bool mem_map(Page&& page);

  void step_instruction();

  Pages    pages;
  IPB      ipb;
  unisim::component::cxx::processor::arm::isa::arm64::Decoder<AArch64> decoder;
  
  U64      gpr[32];
  U32      nzcv;
  U64      current_insn_addr, next_insn_addr;
  U64      TPIDRURW; //< User Read/Write Thread ID Register
};

#endif /* __ARM64VP_ARCHITECTURE_HH__ */
