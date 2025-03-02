/*
 *  Copyright (c) 2007-2020,
 *  Commissariat a l'Energie Atomique (CEA),
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
 
#ifndef STAR_ARCH_HPP
#define STAR_ARCH_HPP

#include <fpu.hh>
#include <fwd.hh>
#include <hw/peripheral.hh>
#include <unisim/component/cxx/processor/sparc/isa/sv8/arch.hh>
#include <unisim/component/cxx/processor/sparc/isa_sv8.hh>
#include <unisim/component/cxx/processor/sparc/trap.hh>
#include <unisim/component/cxx/processor/sparc/asi.hh>
#include <unisim/component/cxx/processor/opcache/opcache.tcc>
#include <unisim/util/truth_table/truth_table.hh>
#include <iosfwd>
#include <inttypes.h>

namespace Star
{
  template <unsigned SIZE> struct TypeFor {};
  template <> struct TypeFor<8> { typedef uint8_t U; };
  template <> struct TypeFor<16> { typedef uint16_t U; };
  template <> struct TypeFor<32> { typedef uint32_t U; typedef float F; };
  template <> struct TypeFor<64> { typedef uint64_t U; typedef double F; };

  template <typename T> struct TypeInfo {};
  template <> struct TypeInfo<uint8_t> { enum {is_signed=0, byte_count=1, bit_count=8}; };
  template <> struct TypeInfo<uint16_t> { enum {is_signed=0, byte_count=2, bit_count=16}; };
  template <> struct TypeInfo<uint32_t> { enum {is_signed=0, byte_count=4, bit_count=32}; };
  template <> struct TypeInfo<uint64_t> { enum {is_signed=0, byte_count=8, bit_count=64}; };
  template <> struct TypeInfo<int8_t> { enum {is_signed=1, byte_count=1, bit_count=8}; };
  template <> struct TypeInfo<int16_t> { enum {is_signed=1, byte_count=2, bit_count=16}; };
  template <> struct TypeInfo<int32_t> { enum {is_signed=1, byte_count=4, bit_count=32}; };
  template <> struct TypeInfo<int64_t> { enum {is_signed=1, byte_count=8, bit_count=64}; };

  template <> struct TypeInfo<float> { typedef uint32_t as_bits; };
  template <> struct TypeInfo<double> { typedef uint64_t as_bits; };
    
  struct BitField_t
  {
    uint32_t&           m_value;
    uint32_t            m_shift;
    uint32_t            m_bitcount;
    
    BitField_t( uint32_t& _value, uint32_t _shift, uint32_t _bitcount )
      : m_value( _value ), m_shift( _shift ), m_bitcount( _bitcount ) {}
    
    operator            uint32_t() const { return (m_value >> m_shift) & ((1 << m_bitcount)-1); }
    void                operator = ( uint32_t _newbits ) {
      uint32_t oldbits = (m_value >> m_shift) & ((1 << m_bitcount)-1);
      m_value ^= ((oldbits ^ _newbits) << m_shift);
    }
    
    BitField_t&         operator = ( BitField_t const& _bf ) {
      uint32_t oldbits = (m_value >> m_shift) & ((1 << m_bitcount)-1);
      uint32_t newbits = (_bf.m_value >> _bf.m_shift) & ((1 << _bf.m_bitcount)-1);
      m_value ^= ((oldbits ^ newbits) << m_shift);
      return *this;
    }
  };
  
  struct Arch : public unisim::component::cxx::processor::sparc::isa::sv8::Arch<Arch>
  {
    typedef uint8_t  U8;
    typedef uint16_t U16;
    typedef uint32_t U32;
    typedef uint64_t U64;
    typedef int8_t   S8;
    typedef int16_t  S16;
    typedef int32_t  S32;
    typedef int64_t  S64;
    typedef float    F32;
    typedef double   F64;
    typedef bool     BOOL;
    
    typedef unisim::component::cxx::processor::sparc::Trap_t Trap_t;
    typedef unisim::component::cxx::processor::sparc::ASI ASI;
    
    // Decoder
    typedef unisim::component::cxx::processor::sparc::isa::sv8::Decoder<Arch> SV8Decoder;
    typedef unisim::component::cxx::processor::opcache::OpCache<SV8Decoder> Decoder;
    typedef unisim::component::cxx::processor::sparc::isa::sv8::Operation<Arch> Operation;

    // Memory system
    struct Page
    {
      static uint32_t const       s_bits = 12;
      
      Page*                       m_next;
      uint32_t                    m_address;
      uint8_t                     m_storage[1<<s_bits];
      
      Page( Page* _next, uint32_t _address )
        : m_next( _next ), m_address( _address & uint32_t(-1 << s_bits) ) {}
      ~Page() { delete m_next; }
    };
    
    Arch();
    ~Arch();

    void                          step();
    void                          pc( uint32_t _pc ) { m_pc = _pc; m_npc = _pc + 4; m_nnpc = _pc + 8; };
    void                          dumptrap( std::ostream& _sink );
    void                          take_trap();

    /* General-Purpose registers */
    uint32_t                      GetGPR(unsigned id) { return m_gpr[id]; }
    void                          SetGPR(unsigned id, uint32_t value) { if (id) m_gpr[id] = value; }
    /* Special-Purpose Register */
    uint32_t                      GetY() { return m_y; }
    void                          SetY(uint32_t value) { m_y = value; }
    uint32_t                      GetWIM() { return m_wim; }
    void                          SetWIM(uint32_t value) { m_wim = value; }
    uint32_t                      GetTBR() { return m_tbr; }
    void                          SetTBR(uint32_t value) { m_tbr = value; }
    
    
    /* Floating-Point registers */
    F32                           GetF32(unsigned id) { return m_fp32[id]; }
    void                          SetF32(unsigned id, F32 value) { m_fp32[id] = value; }
    F64                           GetF64(unsigned id) { return m_fp64[id]; }
    void                          SetF64(unsigned id, F64 value) { m_fp64[id] = value; }
    S32                           GetS32(unsigned id) { return m_fpint[id]; }
    void                          SetS32(unsigned id, S32 value) { m_fp32[id] = value; }

    U32                           GetFSR() { return m_fsr; }
    void                          SetFSR(U32 value) { m_fsr = value; }

    /* Flags */
    bool                          GetCarry() { return (m_psr >> 20) & 1; }
    bool                          GetN() { return (m_psr >> 23) & 1; }
    bool                          GetV() { return (m_psr >> 21) & 1; }
    bool                          GetET() { return et(); }
    void                          SetET(bool _et) { et() = _et; }
    void                          SetS(bool _s) { s() = _s; }
    bool                          GetPS() { return ps(); }
    uint32_t                      GetPSR() { return m_psr; }
    void                          SetPSR(uint32_t _psr) { m_psr = _psr; }
    /* FP Flags */
    void                          SetFCC(bool nle, bool nge) { fcc() = nle*2 | nge*1; }

    /* Control */
    void                          DelayBranch( uint32_t _target, branch_type_t ) { m_nnpc = _target; }
    uint32_t                      GetPC() { return m_pc; }
    void                          StopFetch() { m_execute_mode = false; }
    void                          SetAnnul( bool _annul ) { m_annul = _annul; }
    
    void                          Abort( Trap_t::TrapType_t  _trap );
    void                          SWTrap( uint32_t _idx );
    uint32_t                      nwindows() { return s_nwindows; }
    //    void                          jmp( uint32_t _nnpc, uint32_t _pcreg );

    // PSR access functions
    BitField_t                    impl()  { return BitField_t( m_psr, 28, 4 ); }
    BitField_t                    ver()   { return BitField_t( m_psr, 24, 4 ); }
    BitField_t                    icc()   { return BitField_t( m_psr, 20, 4 ); }
    BitField_t                    n()     { return BitField_t( m_psr, 23, 1 ); }
    BitField_t                    z()     { return BitField_t( m_psr, 22, 1 ); }
    BitField_t                    v()     { return BitField_t( m_psr, 21, 1 ); }
    BitField_t                    c()     { return BitField_t( m_psr, 20, 1 ); }
    BitField_t                    ec()    { return BitField_t( m_psr, 13, 1 ); }
    BitField_t                    ef()    { return BitField_t( m_psr, 12, 1 ); }
    BitField_t                    pil()   { return BitField_t( m_psr,  8, 4 ); }
    BitField_t                    s()     { return BitField_t( m_psr,  7, 1 ); }
    BitField_t                    ps()    { return BitField_t( m_psr,  6, 1 ); }
    BitField_t                    et()    { return BitField_t( m_psr,  5, 1 ); }
    BitField_t                    cwp()   { return BitField_t( m_psr,  0, 5 ); }
    // + read only functions
    bool                          IsSuper() { return (m_psr & (1ul << 7)); }
    ASI                           rqasi() const { return (m_psr & (1ul << 7)) ? ASI::supervisor_data : ASI::user_data; }
    ASI                           insn_asi() { return (m_psr & (1ul << 7)) ? ASI::supervisor_instruction : ASI::user_instruction; }

    bool                          Test(bool cond) { return cond; }

    // common conditions
    struct Conditions
    {
      template <unsigned BIT> using Bit = unisim::util::truth_table::InBit<uint16_t,BIT>;
      template <unsigned CST> using All = unisim::util::truth_table::Always<uint16_t,CST>;
      
      auto N() const { return Bit<3>(); }
      auto Z() const { return Bit<2>(); }
      auto V() const { return Bit<1>(); }
      auto C() const { return Bit<0>(); }
      auto F() const { return All<false>(); }
      auto T() const { return All<true>(); }

      Conditions(Arch& _arch) : arch(_arch) {} Arch& arch;

      template <class TT> bool Eval( TT const& tt ) { unsigned nzcv = (arch.m_psr >> 20) & 15; return (tt.tt >> nzcv) & 1; }
    };
    
    // + write only functions
    void                          SetNZVC(uint32_t _n, uint32_t _z, uint32_t _v, uint32_t _c)
    {
      m_psr &= ~(0xf << 20); // clearing modified bits
      m_psr |= (((_n << 3) | (_z << 2) | (_v << 1) | (_c << 0)) & 0xf) << 20; // applying modification
    }
    
    // TBR access functions
    BitField_t                    tba()   { return BitField_t( m_tbr, 12, 20 ); }
    BitField_t                    tt()    { return BitField_t( m_tbr, 4, 8 ); }
    BitField_t                    zero()  { return BitField_t( m_tbr, 0, 4 ); }
    
    // Register window access functions
    void                          RotateWindow( int32_t _off );
    bool                          InvalidWindow( int32_t _off ) { return ((m_wim >> (uint32_t(cwp() + s_nwindows + _off) % s_nwindows)) & 1) != 0; }
    
    // Ancillary state registers (ASR) access functions
    bool                          asr_perm( uint32_t _idx ) { return IsSuper(); }
    uint32_t                      rdasr( uint32_t _idx );
    void                          wrasr( uint32_t _idx, uint32_t _value );
    
    // Floating-point Status Register (FSR) access function 
    BitField_t                    frd()   { return BitField_t( m_fsr, 30, 2 ); }
    BitField_t                    ftem()  { return BitField_t( m_fsr, 23, 5 ); }
    BitField_t                    fns()   { return BitField_t( m_fsr, 22, 1 ); }
    BitField_t                    fver()  { return BitField_t( m_fsr, 17, 3 ); }
    BitField_t                    ftt()   { return BitField_t( m_fsr, 14, 3 ); }
    BitField_t                    fqne()  { return BitField_t( m_fsr, 13, 1 ); }
    BitField_t                    fcc()   { return BitField_t( m_fsr, 10, 2 ); }
    BitField_t                    aexc()  { return BitField_t( m_fsr,  5, 5 ); }
    BitField_t                    cexc()  { return BitField_t( m_fsr,  0, 5 ); }
    // + read only functions
    struct FConditions
    {
      template <unsigned BIT> using Bit = unisim::util::truth_table::InBit<uint16_t,BIT>;
      template <unsigned CST> using All = unisim::util::truth_table::Always<uint16_t,CST>;
      
      auto E() const { return not Bit<1>() and not Bit<0>(); }
      auto L() const { return not Bit<1>() and     Bit<0>(); }
      auto G() const { return     Bit<1>() and not Bit<0>(); }
      auto U() const { return     Bit<1>() and     Bit<0>(); }
      auto F() const { return                  All<false>(); }
      auto T() const { return                  All <true>(); }

      FConditions(Arch& _arch) : arch(_arch) {} Arch& arch;

      template <class TT> bool Eval( TT const& tt ) { unsigned fcc = (arch.m_fsr >> 10) & 3; return (tt.tt >> fcc) & 1; }
    };

    // bool                          fconde()   { return (0x1/*0b0001*/ >> ((m_fsr >> 10) & 3)) & 1; }
    // bool                          fcondl()   { return (0x2/*0b0010*/ >> ((m_fsr >> 10) & 3)) & 1; }
    // bool                          fcondle()  { return (0x3/*0b0011*/ >> ((m_fsr >> 10) & 3)) & 1; }
    // bool                          fcondg()   { return (0x4/*0b0100*/ >> ((m_fsr >> 10) & 3)) & 1; }
    // bool                          fcondge()  { return (0x5/*0b0101*/ >> ((m_fsr >> 10) & 3)) & 1; }
    // bool                          fcondlg()  { return (0x6/*0b0110*/ >> ((m_fsr >> 10) & 3)) & 1; }
    // bool                          fcondo()   { return (0x7/*0b0111*/ >> ((m_fsr >> 10) & 3)) & 1; }
    // bool                          fcondu()   { return (0x8/*0b1000*/ >> ((m_fsr >> 10) & 3)) & 1; }
    // bool                          fcondue()  { return (0x9/*0b1001*/ >> ((m_fsr >> 10) & 3)) & 1; }
    // bool                          fcondul()  { return (0xa/*0b1010*/ >> ((m_fsr >> 10) & 3)) & 1; }
    // bool                          fcondule() { return (0xb/*0b1011*/ >> ((m_fsr >> 10) & 3)) & 1; }
    // bool                          fcondug()  { return (0xc/*0b1100*/ >> ((m_fsr >> 10) & 3)) & 1; }
    // bool                          fconduge() { return (0xd/*0b1101*/ >> ((m_fsr >> 10) & 3)) & 1; }
    // bool                          fcondne()  { return (0xe/*0b1110*/ >> ((m_fsr >> 10) & 3)) & 1; }
    
    Page*                         getpage( uint32_t _addr );
    
    SSv8::Peripheral*             peripheral( uint32_t addr );
    void                          add( SSv8::Peripheral& _peripheral );
    
    void                          uninitialized_data( ASI _asi, uint32_t _addr, uint32_t _size );
    
    bool                          read( ASI _asi, uint32_t _addr, uint32_t _size, uint8_t* _value )
    {
      m_asi_accesses[_asi.code&0xff] ++;
      if ((_addr % _size) or _size > (1 << Page::s_bits)) return false;
      uint32_t pageidx = _addr >> Page::s_bits;
      uint32_t pageaddr = pageidx << Page::s_bits;
      pageidx = pageidx % (1 << s_bits);
      uint32_t offset = _addr - pageaddr;

      Page* match = m_pages[pageidx];
      if (match and match->m_address != pageaddr)
        {
          match = 0;
          for (Page* node = m_pages[pageidx]; node->m_next; node = node->m_next)
            {
              if (node->m_next->m_address != pageaddr) continue;
              match = node->m_next;
              node->m_next = match->m_next;
              match->m_next = m_pages[pageidx];
              m_pages[pageidx] = match;
              break;
            }
        }
      
      if (not match)
        {
          SSv8::Peripheral* target = peripheral( _addr );
          if (target and target->read( _addr, _size, _value ))
            return true;
          return uninitialized_data( _asi, _addr, _size ), false;
        }
      
      uint8_t const* storage = &(match->m_storage[offset]);
      
      for (uint32_t idx = 0; idx < _size; ++ idx )
        _value[idx] = storage[idx];
      
      return true;
    }
    
    bool
    write( ASI _asi, uint32_t _addr, uint32_t _size, uint8_t const* _value )
    {
      m_asi_accesses[_asi.code&0xff] ++;
      if( (_addr % _size) or _size > (1 << Page::s_bits) ) return false;
      uint32_t pageidx = _addr >> Page::s_bits;
      uint32_t pageaddr = pageidx << Page::s_bits;
      pageidx = pageidx % (1 << s_bits);
      uint32_t offset = _addr - pageaddr;

      Page* match = m_pages[pageidx];
      if (match and match->m_address != pageaddr)
        {
          match = 0;
          for (Page* node = m_pages[pageidx]; node->m_next; node = node->m_next)
            {
              if (node->m_next->m_address != pageaddr) continue;
              match = node->m_next;
              node->m_next = match->m_next;
              match->m_next = m_pages[pageidx];
              m_pages[pageidx] = match;
              break;
            }
        }
      
      if (not match)
        {
          SSv8::Peripheral* target = peripheral( _addr );
          if (target) return target->write( _addr, _size, _value );
          match = new Page( m_pages[pageidx], pageaddr );
          m_pages[pageidx] = match;
        }
      
      uint8_t* storage = &(match->m_storage[offset]);
      
      for (uint32_t idx = 0; idx < _size; ++ idx)
        storage[idx] = _value[idx];
      
      return true;
    }

    template <typename U>
    U
    MemRead(U const&, ASI asi, uint32_t addr)
    {
      unsigned const bytecount = TypeInfo<U>::byte_count;
      
      if (addr & (bytecount-1))
        Abort( Trap_t::mem_address_not_aligned );
      
      uint8_t storage[bytecount];
      if (not read( asi, addr, bytecount, &storage[0] ))
        Abort( (asi.code == asi.user_instruction or asi.code == asi.supervisor_instruction) ?
               Trap_t::instruction_access_exception : Trap_t::data_access_exception );
      
      U res(0);
      for (unsigned idx = 0; idx < bytecount; ++idx)
        res = (res << 8) | U(storage[idx]);
      return res;
    }

    template <typename U>
    void
    MemWrite(ASI asi, uint32_t addr, U value)
    {
      unsigned const bytecount = TypeInfo<U>::byte_count;
      
      if (addr & (bytecount-1))
        Abort( Trap_t::mem_address_not_aligned );
      
      uint8_t storage[bytecount];
      for (unsigned idx = bytecount; idx-- > 0;)
        { storage[idx] = uint8_t( value ); value >>= 8; }
      
      if (not write( asi, addr, bytecount, &storage[0] ))
        Abort( Trap_t::data_access_exception );
    }
    
    // Hopefully native float type is IEEE.754 and its memory
    // endianess is consistent with that of native uint32_t. This
    // way, we can reinterpret IEEE.754 as uint32_t and use portable
    // shift operations.
    template <typename T>
    T
    FMemRead(T const&, ASI asi, uint32_t addr)
    {
      auto res = MemRead(typename TypeInfo<T>::as_bits(), asi, addr);
      return *reinterpret_cast<T*>(&res);
    }
    
    template <typename T>
    void
    FMemWrite(ASI asi, uint32_t addr, T const& val)
    {
      MemWrite(asi, addr, *reinterpret_cast<typename TypeInfo<T>::as_bits const*>(&val));
    }
    
    void memcpy( uint32_t _addr, uint8_t const* _buffer, uint32_t _size );

    void UndefinedInstruction( Operation const& op );

    struct AbortInstruction {};

    void WithPrivilege()
    {
      if (not IsSuper())
        Abort( Trap_t::privileged_instruction );
    }
    
    // Processor State
    bool                          m_execute_mode;
    bool                          m_annul;
    Trap_t                        m_trap;
    uint32_t                      m_tbr;
    uint32_t                      m_psr;
    uint32_t                      m_pc;
    uint32_t                      m_npc;
    uint32_t                      m_nnpc;
    int64_t                       m_instcount;
    
    // Integer Registers
    static
    uint32_t const                s_nwindows = 8;
    uint32_t                      m_gpr[32]; // Current window
    uint32_t                      m_wgpr[s_nwindows*16]; // Windowed regs
    uint32_t                      m_wim;
    uint32_t                      m_y;
    
    // Floating Point Registers
    uint32_t                      m_fsr;
    FPRawBank                     m_fprawbank;
    FP32Bank                      m_fp32;
    FP64Bank                      m_fp64;
    FP128Bank                     m_fp128;
    FPIntBank                     m_fpint;

    Decoder                       decoder;
    Operation*                    lastoperation;
    bool                          disasm;
    
    static uint32_t const         s_bits = 12;
    Page*                         m_pages[1<<s_bits];
    
    // Peripherals
    int                           m_peripheralcount;
    SSv8::Peripheral**            m_peripherals;
    
    // Memory stats
    int64_t                       m_asi_accesses[256];
  };
} // end of namespace Star

#endif // STAR_ARCH_HPP

/*
  Emacs lisp utility functions
  
  (defun bintohex (beg end)
  "Maintains a dual hex/bin constant."
    (interactive "*r")
      (save-excursion
        (goto-char beg)
        (while (re-search-forward "0x\\([A-Fa-f0-9]+\\)/\\*0b\\([01]+\\)\\*\/" end t)
          (replace-match (format "0x%x/\*0b\\2*\/" (string-to-int (match-string 2) 2)))
          )
      )
    )
*/
