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
#include <unisim/component/cxx/vector/vector.hh>
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
  
  typedef TaintedValue<  bool  > BOOL;
  
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
  
  void        CheckSystemAccess( uint8_t op1 );
  U64         ReadSystemRegister( uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2 );
  void        WriteSystemRegister( uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, U64 value );
  void        DescribeSystemRegister( uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, std::ostream& sink );
  void        NameSystemRegister( uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, std::ostream& sink );
  
  //=====================================================================
  //=                      Control Transfer methods                     =
  //=====================================================================
  
  /** Set the next Program Counter */
  enum branch_type_t { B_JMP = 0, B_CALL, B_RET };
  void BranchTo( U64 addr, branch_type_t branch_type ) { if (addr.ubits) { struct Bad {}; throw Bad(); } next_insn_addr = addr.value; }
  bool Test( bool cond ) { return cond; }
  template <typename T>
  bool Test( TaintedValue<T> const& cond )
  {
    if (cond.ubits)
      { struct Bad {}; throw Bad (); }
    return cond.value;
  }  
  void CallSupervisor( uint32_t imm );
  
  //=====================================================================
  //=                       Memory access methods                       =
  //=====================================================================
  
  template <typename T>
  T
  memory_read(U64 addr)
  {
    unsigned const size = sizeof (typename T::value_type);
    uint8_t dbuf[size], ubuf[size];
    if (access_page(addr).read(addr.value,&dbuf[0],&ubuf[0],size) != size)
      { struct Bad {}; throw Bad (); }

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

  U64 MemRead64(U64 addr) { return memory_read<U64>(addr); }
  U32 MemRead32(U64 addr) { return memory_read<U32>(addr); }
  U16 MemRead16(U64 addr) { return memory_read<U16>(addr); }
  U8  MemRead8 (U64 addr) { return memory_read<U8> (addr); }
  
  void MemRead( U8* buffer, U64 addr, unsigned size );
  
  template <typename T>
  void
  memory_write(U64 addr, T src)
  {
    if (addr.ubits) { struct Bad {}; throw Bad(); }
    unsigned const size = sizeof (typename T::value_type);
    typedef typename TX<typename T::value_type>::as_mask bits_type;

    bits_type value = *reinterpret_cast<bits_type const*>(&src.value), ubits = src.ubits;
    uint8_t dbuf[size], ubuf[size];

    for (unsigned idx = 0; idx < sizeof (bits_type); ++idx)
      {
        dbuf[idx] = value & 0xff; value >>= 8;
        ubuf[idx] = ubits & 0xff; ubits >>= 8;
      }
    
    if (modify_page(addr.value).write(addr.value,&dbuf[0],&ubuf[0],size) != size)
      { struct Bad {}; throw Bad (); }
  }
  
  void MemWrite64(U64 addr, U64 val) { memory_write(addr, val); }
  void MemWrite32(U64 addr, U32 val) { memory_write(addr, val); }
  void MemWrite16(U64 addr, U16 val) { memory_write(addr, val); }
  void MemWrite8 (U64 addr, U8  val) { memory_write(addr, val); }
  
  void MemWrite( U64 addr, U8 const* buffer, unsigned size );

  void     SetExclusiveMonitors( U64 addr, unsigned size ) { /*TODO: MP support*/ }
  bool     ExclusiveMonitorsPass( U64 addr, unsigned size ) { /*TODO: MP support*/ return true; }
  void     ClearExclusiveLocal() {}
  

  // Cache operation
  void dc_zva(U64 addr);
  
  /**********************************************************************
   ***                       Architectural state                      ***
   **********************************************************************/
  
  struct SysReg
  {
    virtual            ~SysReg() {}
    virtual void        Write( AArch64& cpu, U64 const& value ) const;
    virtual U64         Read( AArch64& cpu ) const;
    virtual char const* Describe() const = 0;
    virtual char const* Name() const = 0;
  };
  
  virtual SysReg const&  GetSystemRegister( uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2 );
  virtual void           ResetSystemRegisters();
  
  struct VUConfig
  {
    static unsigned const BYTECOUNT = 16;
    template <typename T> using TypeInfo = TaintedTypeInfo<T>;
    typedef U8 Byte;
  };
  static unsigned const VECTORCOUNT = 32;
  struct Vector { uint8_t data[sizeof (U8)][VUConfig::BYTECOUNT]; };

  unisim::component::cxx::vector::VUnion<VUConfig> vector_views[VECTORCOUNT];
  Vector vectors[VECTORCOUNT];
  
  struct Page
  {
    uint64_t hi() const { return last; }

    bool operator < (Page const& p) const { return last < p.base; }
    bool operator > (Page const& p) const { return p.last < base; }

    struct Above
    {
      using is_transparent = void;
      bool operator() (Page const& a, Page const& b) const { return a > b; }
      bool operator() (Page const& a, uint64_t b) const { return a.base > b; }
    };
    struct Free
    {
      virtual ~Free() {};
      virtual void free (Page&) const {};
      static Free nop;
    };
    Page( Free*, uint64_t _base, uint64_t _last, uint8_t* _data, uint8_t* _udat, Free* _free )
      : base(_base), last(_last), data(_data), udat(_udat), free(_free)
    {}
    Page( Page&& page )
      : base(page.base), last(page.last), data(page.data), udat(page.udat), free(page.free)
    {
      page.data = 0;
      page.udat = 0;
      page.free = &Free::nop;
    }
    Page( Page const& ) = delete;
    ~Page();
    
    uint64_t write(uint64_t addr, uint8_t const* dbuf, uint8_t const* ubuf, uint64_t count) const
    {
      uint64_t cnt = std::min(count,last-addr+1), start = addr-base;
      std::copy( &dbuf[0], &dbuf[cnt], &data[start] );
      std::copy( &ubuf[0], &ubuf[cnt], &udat[start] );
      return cnt;
    }
    // bool write(uint64_t addr, unsigned count, unsigned endianness, uint64_t value) const
    // {
    //   uint8_t bytes[8];
    //   for (unsigned idx = 0; idx < count; ++idx)
    //     { bytes[idx^endianness] = value; value >>= 8; }
    //   return write( addr, &bytes[0], count ) == count;
    // }
    uint64_t read(uint64_t addr, uint8_t* dbuf, uint8_t* ubuf, uint64_t count) const
    {
      uint64_t cnt = std::min(count,last-addr+1), start = addr-base;
      std::copy( &data[start], &data[start+cnt], &dbuf[0] );
      std::copy( &udat[start], &udat[start+cnt], &ubuf[0] );
      return cnt;
    }
    // bool read(uint64_t addr, unsigned count, unsigned endianness, uint64_t* value) const
    // {
    //   uint8_t bytes[8];
    //   if (read(addr, &bytes[0], count) != count) return false;
    //   uint64_t res = 0;
    //   for (unsigned idx = count; idx-- != 0;)
    //     { res <<= 8; res |= bytes[idx^endianness]; }
    //   *value = res;
    //   return true;
    // }
    bool has_data() const { return data; }
    uint8_t const* get_data() const { return data; }
    bool has_udat() const { return data; }
    uint8_t const* get_udat() const { return udat; }
    uint64_t size() const { return last - base + 1; }

    // void dump(std::ostream&) const;
    // friend std::ostream& operator << ( std::ostream& sink, Page const& p ) { p.dump(sink); return sink; }


  public:
    uint64_t    base;
    uint64_t    last;
  private:
    uint8_t*    data;
    uint8_t*    udat;
    Page::Free* free;

    void resize(uint64_t last);
  };

  typedef std::set<Page, Page::Above> Pages;

  Page const& access_page( U64 addr )
  {
    if (addr.ubits) { struct Bad {}; throw Bad(); }
    return access_page(addr.value);
  }
  Page const& access_page( uint64_t addr )
  {
    auto pi = pages.lower_bound(addr);
    if (pi == pages.end() or pi->last < addr)
      { struct Bad {}; throw Bad(); }
    return *pi;
  }
  Page const& modify_page(uint64_t addr)
  {
    auto pi = pages.lower_bound(addr);
    if (pi == pages.end() or pi->last < addr)
      return alloc_page(pi, addr);
    return *pi;
  }

  Page const& alloc_page(Pages::iterator pi, uint64_t addr);
  
  struct IPB
  {
    static unsigned const LINE_SIZE = 32; //< IPB size
    uint8_t  bytes[LINE_SIZE];             //< The IPB content
    uint64_t base_address;                //< base address of IPB content (cache line size aligned if valid)
    IPB() : bytes(), base_address( -1 ) {}
    uint8_t* get(AArch64& core, uint64_t address);
  };
  
  void error_mem_overlap( Page const& a, Page const& b );
  
  bool new_page(uint64_t addr, uint64_t size);
  bool mem_map(Page&& page);

  void step_instruction();

  Pages    pages;
  IPB      ipb;
  unisim::component::cxx::processor::arm::isa::arm64::Decoder<AArch64> decoder;
  
  U64      gpr[32];
  U8       nzcv;
  uint64_t current_insn_addr, next_insn_addr;
  U64      TPIDRURW; //< User Read/Write Thread ID Register
};

#endif /* __ARM64VP_ARCHITECTURE_HH__ */
