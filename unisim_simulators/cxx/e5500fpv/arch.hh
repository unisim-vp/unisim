/*
 *  Copyright (c) 2018,
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

#ifndef __E5500FPV_ARCH_HH__
#define __E5500FPV_ARCH_HH__

#include <top_ppc64.hh>
#include <unisim/component/cxx/processor/powerpc/floating.hh>
#include <unisim/component/cxx/processor/powerpc/isa/book_vle/vle.hh>
#include <unisim/component/cxx/memory/sparse/memory.hh>
#include <unisim/util/reg/core/register.hh>
#include <unisim/util/reg/core/register.tcc>
#include <unisim/service/interfaces/memory_injection.hh>
#include <unisim/service/interfaces/memory.hh>
#include <unisim/service/interfaces/registers.hh>
#include <unisim/service/interfaces/linux_os.hh>

typedef uint8_t  U8;
typedef uint16_t U16;
typedef uint32_t U32;
typedef uint64_t U64;
typedef  int8_t  S8;
typedef  int16_t S16;
typedef  int32_t S32;
typedef  int64_t S64;
typedef  bool    BOOL;

typedef uint64_t UINT;
typedef int64_t  SINT;
typedef uint64_t ADDRESS;

typedef unisim::component::cxx::processor::powerpc::SoftDouble SoftDouble;
typedef unisim::component::cxx::processor::powerpc::SoftFloat SoftFloat;

struct MisInsn
{
  MisInsn( uint64_t _addr, uint32_t _code )
    : addr(_addr), code(_code)
  {}

  uint64_t addr;
  uint32_t code;
};

template <typename FIELD, int OFFSET1, int OFFSET2 = -1, unisim::util::reg::core::Access ACCESS = unisim::util::reg::core::SW_RW>
struct Field : unisim::util::reg::core::Field<FIELD, (OFFSET2 >= 0) ? ((OFFSET1 < OFFSET2) ? (31 - OFFSET2) : (31 - OFFSET1)) : (31 - OFFSET1), (OFFSET2 >= 0) ? ((OFFSET1 < OFFSET2) ? (OFFSET2 - OFFSET1 + 1) : (OFFSET1 - OFFSET2 + 1)) : 1, ACCESS>
{
  typedef unisim::util::reg::core::Field<FIELD, (OFFSET2 >= 0) ? ((OFFSET1 < OFFSET2) ? (31 - OFFSET2) : (31 - OFFSET1)) : (31 - OFFSET1), (OFFSET2 >= 0) ? ((OFFSET1 < OFFSET2) ? (OFFSET2 - OFFSET1 + 1) : (OFFSET1 - OFFSET2 + 1)) : 1, ACCESS> Super;
};

template <typename REGISTER>
struct Register
{
  Register(UINT _value) : value(_value) {}
  
  template <typename FIELD> void Set( UINT field_value ) {  FIELD::Set(value, field_value); }
  template <typename FIELD> UINT Get() const { return FIELD::Get(value); }
  operator UINT () const { return value; }
  
  UINT value;
};

// Floating-Point Status and Control Register
struct FPSCR : Register<FPSCR>
{
  typedef Register<FPSCR> Super;

  struct _0_3   : Field<_0_3,    0, 3> {};  // FX, FEX, VX, OX bunch
  struct FX     : Field<FX,      0> {};     // Floating-Point Exception Summary
  struct FEX    : Field<FEX,     1> {};     // Floating-Point Enabled Exception Summary
  struct VX     : Field<VX,      2> {};     // Floating-Point Invalid Operation Exception Summary
  struct OX     : Field<OX,      3> {};     // Floating-Point Overflow Exception
  struct UX     : Field<UX,      4> {};     // Floating-Point Underflow Exception
  struct ZX     : Field<ZX,      5> {};     // Floating-Point Zero Divide Exception
  struct XX     : Field<XX,      6> {};     // Floating-Point Inexact Exception
  struct VXSNAN : Field<VXSNAN,  7> {};     // Floating-Point Invalid Operation Exception (SNaN)
  struct VXISI  : Field<VXISI,   8> {};     // Floating-Point Invalid Operation Exception (inf - inf)
  struct VXIDI  : Field<VXIDI,   9> {};     // Floating-Point Invalid Operation Exception (inf / inf)
  struct VXZDZ  : Field<VXZDZ,  10> {};     // Floating-Point Invalid Operation Exception (0 / 0)
  struct VXIMZ  : Field<VXIMZ,  11> {};     // Floating-Point Invalid Operation Exception (inf * 0)
  struct VXVC   : Field<VXVC,   12> {};     // Floating-Point Invalid Operation Exception (Invalid Compare)
  struct FR     : Field<FR,     13> {};     // Floating-Point Fraction Rounded
  struct FI     : Field<FI,     14> {};     // Floating-Point Fraction Inexact
  struct FPRF   : Field<FPRF,   15, 19> {}; // Floating-Point Result Flags
  struct C      : Field<C,      15> {};     // Floating-Point Result Class Descriptor
  struct FPCC   : Field<FPCC,   16, 19> {}; // Floating-Point Condition Code
  struct FL     : Field<FL,     16> {};     // Floating-Point Less Than or Negative
  struct FG     : Field<FG,     17> {};     // Floating-Point Greater Than or Positive
  struct FE     : Field<FE,     18> {};     // Floating-Point Equal or Zero
  struct FU     : Field<FU,     19> {};     // Floating-Point Unordered or NaN
  struct VXSOFT : Field<VXSOFT, 21> {};     // Floating-Point Invalid Operation Exception (Software-Defined Condition) 
  struct VXSQRT : Field<VXSQRT, 22> {};     // Floating-Point Invalid Operation Exception (Invalid Square Root)
  struct VXCVI  : Field<VXCVI,  23> {};     // Floating-Point Invalid Operation Exception (Invalid Integer Convert)
  struct VE     : Field<VE,     24> {};     // Floating-Point Invalid Operation Exception Enable
  struct OE     : Field<OE,     25> {};     // Floating-Point Overflow Exception Enable 
  struct UE     : Field<UE,     26> {};     // Floating-Point Underflow Exception Enable 
  struct ZE     : Field<ZE,     27> {};     // Floating-Point Zero Divide Exception  Enable
  struct XE     : Field<XE,     28> {};     // Floating-Point Inexact Exception Enable 
  struct NI     : Field<NI,     29> {};     // Floating-Point Non-IEEE Mode
  struct RN     : Field<RN,     30, 31> {}; // Floating-Point Rounding Control
  
  FPSCR(UINT _value) : Super(_value) {}
};

// Machine State Register
struct MSR : Register<MSR>
{
  typedef Register<MSR> Super;
		
  struct SPV : Field<SPV,6>  {}; // SP/Embedded FP/Vector available
  struct WE  : Field<WE ,13> {}; // Wait state (Power management) enable
  struct CE  : Field<CE ,14> {}; // Critical Interrupt Enable
  struct EE  : Field<EE ,16> {}; // External Interrupt Enable
  struct PR  : Field<PR ,17> {}; // Problem State
  struct FP  : Field<FP ,18> {}; // Floating-Point Available
  struct ME  : Field<ME ,19> {}; // Machine Check Enable
  struct FE0 : Field<FE0,20> {}; // Floating-point exception mode 0 (not used)
  struct DE  : Field<DE ,22> {}; // Debug Interrupt Enable
  struct FE1 : Field<FE1,23> {}; // Floating-point exception mode 1 (not used)
  struct IS  : Field<IS ,26> {}; // Instruction Address Space
  struct DS  : Field<DS ,27> {}; // Data Address Space
  struct PMM : Field<PMM,29> {}; // PMM Performance monitor mark bit
  struct RI  : Field<RI ,30> {}; // Recoverable Interrupt
		
  MSR(UINT _value) : Super(_value) {}
};

// Integer Exception Register
struct XER : Register<XER>
{
  typedef Register<XER> Super;
	
  struct SO         : Field<SO        ,0> {};      // Summary Overflow
  struct OV         : Field<OV        ,1> {};      // Overflow
  struct CA         : Field<CA        ,2> {};      // Carry
  struct BYTE_COUNT : Field<BYTE_COUNT,25, 31> {}; // Byte count
   
  struct _0_3       : Field<_0_3, 0, 3> {}; // XER[0-3]
	
  XER(UINT _value) : Super(_value) {}
};

// Condition Register
struct CR : Register<CR>
{
  typedef Register<CR> Super;
	
  struct CR0 : Field<CR0, 0, 3> { struct LT : Field<LT, 0> {}; struct GT : Field<GT, 1> {}; struct EQ : Field<EQ, 2> {}; struct SO : Field<SO, 3> {}; };
  struct CR1 : Field<CR1, 4, 7> { struct LT : Field<LT, 4> {}; struct GT : Field<GT, 5> {}; struct EQ : Field<EQ, 6> {}; struct SO : Field<SO, 7> {}; };
  struct CR2 : Field<CR2, 8,11> { struct LT : Field<LT, 8> {}; struct GT : Field<GT, 9> {}; struct EQ : Field<EQ,10> {}; struct SO : Field<SO,11> {}; };
  struct CR3 : Field<CR3,12,15> { struct LT : Field<LT,12> {}; struct GT : Field<GT,13> {}; struct EQ : Field<EQ,14> {}; struct SO : Field<SO,15> {}; };
  struct CR4 : Field<CR4,16,19> { struct LT : Field<LT,16> {}; struct GT : Field<GT,17> {}; struct EQ : Field<EQ,18> {}; struct SO : Field<SO,19> {}; };
  struct CR5 : Field<CR5,20,23> { struct LT : Field<LT,20> {}; struct GT : Field<GT,21> {}; struct EQ : Field<EQ,22> {}; struct SO : Field<SO,23> {}; };
  struct CR6 : Field<CR6,24,27> { struct LT : Field<LT,24> {}; struct GT : Field<GT,25> {}; struct EQ : Field<EQ,26> {}; struct SO : Field<SO,27> {}; };
  struct CR7 : Field<CR7,28,31> { struct LT : Field<LT,28> {}; struct GT : Field<GT,29> {}; struct EQ : Field<EQ,30> {}; struct SO : Field<SO,31> {}; };
	
  CR(UINT _value) : Super(_value) {}
};

// Link Register
typedef U64 LR;

// Count Register
typedef U64 CTR;
  
struct Arch
  : public unisim::service::interfaces::MemoryInjection<uint64_t>
  , public unisim::service::interfaces::Memory<uint64_t>
  , public unisim::service::interfaces::Registers
{
  
  typedef unisim::component::cxx::processor::powerpc::ppc64::Decoder                             Decoder;
  typedef unisim::component::cxx::processor::powerpc::ppc64::Operation                           Operation;
  typedef unisim::service::interfaces::LinuxOS                                                   LinuxOS;
  
  struct ClearMemSet { void operator() ( uint8_t* base, uintptr_t size ) const { for (uintptr_t idx = 0; idx < size; ++idx) base[idx] = 0; } };
  typedef typename unisim::component::cxx::memory::sparse::Memory<uint64_t,12,12,ClearMemSet>   Memory;

  struct ProgramInterrupt
  {
    struct IllegalInstruction { typedef ProgramInterrupt Interrupt; };
    struct UnimplementedInstruction { typedef ProgramInterrupt Interrupt; };
    struct FloatingPoint { typedef ProgramInterrupt Interrupt; };
  };
  
  struct SystemCallInterrupt
  {
    struct SystemCall { typedef SystemCallInterrupt Interrupt; };
    SystemCallInterrupt() {}
    void SetELEV(unsigned _elev) { if (_elev != 0) throw *this; /* Hypervisor interrupt ? */ }
  };

  struct FloatingPointUnavailableInterrupt
  {
    struct FloatingPointUnavailable { typedef FloatingPointUnavailableInterrupt Interrupt; };
  };
  
  Arch();
  ~Arch();
  
  void SetLinuxOS( LinuxOS* _linux_os ) { linux_os = _linux_os; }
  LinuxOS* GetLinuxOS() { return linux_os; }
  
  // unisim::service::interfaces::Memory<uint64_t>
  void Reset() {}
  
  bool ReadMemory(uint64_t addr, void* buffer, unsigned size );
  
  bool WriteMemory(uint64_t addr, void const* buffer, unsigned size);
  
  // unisim::service::interfaces::Registers
  unisim::service::interfaces::Register* GetRegister(char const* name);
  
  void ScanRegisters(unisim::service::interfaces::RegisterScanner& scanner);
  
  // unisim::service::interfaces::MemoryInjection<uint64_t>
  bool InjectReadMemory(uint64_t addr, void* buffer, unsigned size);
  
  bool InjectWriteMemory(uint64_t addr, void const* buffer, unsigned size);
  
  template <class EXC> void ThrowException( EXC const& ) {}
  
  template <class EXC>
  typename EXC::Interrupt ThrowException() { ThrowException( EXC() ); return typename EXC::Interrupt(); }

  void ThrowException( SystemCallInterrupt::SystemCall const& );

  CR&  GetCR() { return cr; }
  U64& GetLR() { return lr; }
  U64& GetCTR() { return ctr; }
  U64  GetCIA() { return cia; }
  void Branch( U64 addr ) { nia = addr; }
  
  XER& GetXER() { return xer; }
  MSR&        GetMSR() { return msr; }
  bool MoveFromSPR( unsigned  id, U64& value );
  bool MoveToSPR( unsigned  id, U64 value ) { return false; }
  
  U64  GetGPR( unsigned id ) { return gprs[id]; }
  void SetGPR( unsigned id, U64 value ) { gprs[id] = value; }

  template <typename T>
  void
  IntStore( U64 addr, T value )
  {
    int const size = sizeof (T);
    uint8_t buf[size];
    for (int idx = size; --idx>=0;)
      {
        buf[idx] = value;
        value >>= 8;
      }
    memory.write( addr, &buf[0], size );
  }

  template <typename T>
  T
  IntLoad( U64 addr )
  {
    int const size = sizeof (T);
    uint8_t buf[size];
    memory.read( &buf[0], addr, size );
    T value = 0;
    for (int idx = 0; idx < size; ++idx)
      value = (value << 8) | T(buf[idx]);
    return value;
  }

  bool Int8Store (unsigned id, U64 addr) { IntStore( addr, U8 (gprs[id]) ); return true; }
  bool Int16Store(unsigned id, U64 addr) { IntStore( addr, U16(gprs[id]) ); return true; }
  bool Int32Store(unsigned id, U64 addr) { IntStore( addr, U32(gprs[id]) ); return true; }
  bool Int64Store(unsigned id, U64 addr) { IntStore( addr, U64(gprs[id]) ); return true; }

  bool Int8Load (unsigned id, U64 addr) { gprs[id] = IntLoad<U8> ( addr ); return true; }
  bool Int16Load(unsigned id, U64 addr) { gprs[id] = IntLoad<U16>( addr ); return true; }
  bool Int32Load(unsigned id, U64 addr) { gprs[id] = IntLoad<U32>( addr ); return true; }
  bool Int64Load(unsigned id, U64 addr) { gprs[id] = IntLoad<U64>( addr ); return true; }

  bool SInt8Load (unsigned id, U64 addr) { gprs[id] = IntLoad<S8> ( addr ); return true; }
  bool SInt16Load(unsigned id, U64 addr) { gprs[id] = IntLoad<S16>( addr ); return true; }
  bool SInt32Load(unsigned id, U64 addr) { gprs[id] = IntLoad<S32>( addr ); return true; }
  bool SInt64Load(unsigned id, U64 addr) { gprs[id] = IntLoad<S64>( addr ); return true; }

  bool Lbarx(unsigned id, U64 addr) { return Int8Load ( id, addr ); }
  bool Lharx(unsigned id, U64 addr) { return Int16Load( id, addr ); }
  bool Lwarx(unsigned id, U64 addr) { return Int32Load( id, addr ); }
  bool Ldarx(unsigned id, U64 addr) { return Int64Load( id, addr ); }


  bool Stbcx(unsigned id, U64 addr) { cr.Set<CR::CR0>(0b0010 | xer.Get<XER::SO>()); return Int8Store ( id, addr ); }
  bool Sthcx(unsigned id, U64 addr) { cr.Set<CR::CR0>(0b0010 | xer.Get<XER::SO>()); return Int16Store( id, addr ); }
  bool Stwcx(unsigned id, U64 addr) { cr.Set<CR::CR0>(0b0010 | xer.Get<XER::SO>()); return Int32Store( id, addr ); }
  bool Stdcx(unsigned id, U64 addr) { cr.Set<CR::CR0>(0b0010 | xer.Get<XER::SO>()); return Int64Store( id, addr ); }

  bool        GetMSR_FP() { return true; }
  SoftDouble& GetFPR(unsigned id) { return fprs[id]; }
  void        SetFPR(unsigned id, SoftDouble const& value ) { fprs[id] = value; }
  FPSCR&      GetFPSCR() { return fpscr; }
  void        SetFPSCR(UINT value) { fpscr = value; }
  
  void Isync() {}
  
  bool InstructionFetch(uint64_t addr, uint32_t& insn);
  
  Operation*   fetch();
  void         commit();

  Decoder         decoder;
  Memory          memory;
  LinuxOS*        linux_os;
  std::map<std::string,unisim::service::interfaces::Register*> regmap;
  U64             gprs[32];
  U64             ctr, lr, cia, nia;
  XER             xer;
  CR              cr;
  MSR             msr;
  
  SoftDouble      fprs[32];
  FPSCR           fpscr;
  
  uintptr_t       instcount;
  U64             time_base;

  static bool const HAS_FPU = true;
};

typedef Arch CPU;
typedef Arch CONFIG;

U64 UnsignedMultiplyHigh( U64 lop, U64 rop );
S64 SignedMultiplyHigh( S64 lop, S64 rop );

#endif // __E5500FPV_ARCH_HH__
