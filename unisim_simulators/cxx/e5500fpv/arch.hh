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
#include <simfloat.hh>
#include <types.hh>
#include <unisim/component/cxx/processor/powerpc/isa/book_vle/vle.hh>
#include <unisim/component/cxx/memory/sparse/memory.hh>
#include <unisim/util/reg/core/register.hh>
#include <unisim/util/reg/core/register.tcc>
#include <unisim/service/interfaces/memory_injection.hh>
#include <unisim/service/interfaces/memory.hh>
#include <unisim/service/interfaces/registers.hh>
#include <unisim/service/interfaces/linux_os.hh>
#include <unisim/service/interfaces/debug_yielding.hh>
#include <unisim/service/interfaces/trap_reporting.hh>
#include <unisim/service/interfaces/memory_access_reporting.hh>
#include <unisim/service/interfaces/disassembly.hh>

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

struct MisInsn
{
  MisInsn( uint64_t _addr, uint32_t _code )
    : addr(_addr), code(_code)
  {}

  uint64_t addr;
  uint32_t code;
};

template <typename FIELD, int OFFSET1, int OFFSET2 = -1, unisim::util::reg::core::Access ACCESS = unisim::util::reg::core::SW_RW>
struct Field : unisim::util::reg::core::Field<FIELD, (OFFSET2 >= 0) ? ((OFFSET1 < OFFSET2) ? (63 - OFFSET2) : (63 - OFFSET1)) : (63 - OFFSET1), (OFFSET2 >= 0) ? ((OFFSET1 < OFFSET2) ? (OFFSET2 - OFFSET1 + 1) : (OFFSET1 - OFFSET2 + 1)) : 1, ACCESS>
{
  typedef unisim::util::reg::core::Field<FIELD, (OFFSET2 >= 0) ? ((OFFSET1 < OFFSET2) ? (63 - OFFSET2) : (63 - OFFSET1)) : (63 - OFFSET1), (OFFSET2 >= 0) ? ((OFFSET1 < OFFSET2) ? (OFFSET2 - OFFSET1 + 1) : (OFFSET1 - OFFSET2 + 1)) : 1, ACCESS> Super;
};

template <typename REGISTER>
struct Register
{
  typedef UINT TYPE;
  
  Register(TYPE _value) : value(_value) {}
  
  template <typename FIELD> void Set( TYPE field_value ) {  FIELD::Set(value, field_value); }
  template <typename FIELD> TYPE Get() const { return FIELD::Get(value); }
  operator TYPE () const { return value; }
  
  TYPE value;
};

// Floating-Point Status and Control Register
struct FPSCR : Register<FPSCR>
{
  typedef Register<FPSCR> Super;

  struct _0_3   : Field<_0_3,   32,35> {};  // FX, FEX, VX, OX bunch
  struct FX     : Field<FX,     32> {};     // Floating-Point Exception Summary
  struct FEX    : Field<FEX,    33> {};     // Floating-Point Enabled Exception Summary
  struct VX     : Field<VX,     34> {};     // Floating-Point Invalid Operation Exception Summary
  struct OX     : Field<OX,     35> {};     // Floating-Point Overflow Exception
  struct UX     : Field<UX,     36> {};     // Floating-Point Underflow Exception
  struct ZX     : Field<ZX,     37> {};     // Floating-Point Zero Divide Exception
  struct XX     : Field<XX,     38> {};     // Floating-Point Inexact Exception
  struct VXSNAN : Field<VXSNAN, 39> {};     // Floating-Point Invalid Operation Exception (SNaN)
  struct VXISI  : Field<VXISI,  40> {};     // Floating-Point Invalid Operation Exception (inf - inf)
  struct VXIDI  : Field<VXIDI,  41> {};     // Floating-Point Invalid Operation Exception (inf / inf)
  struct VXZDZ  : Field<VXZDZ,  42> {};     // Floating-Point Invalid Operation Exception (0 / 0)
  struct VXIMZ  : Field<VXIMZ,  43> {};     // Floating-Point Invalid Operation Exception (inf * 0)
  struct VXVC   : Field<VXVC,   44> {};     // Floating-Point Invalid Operation Exception (Invalid Compare)
  struct FR     : Field<FR,     45> {};     // Floating-Point Fraction Rounded
  struct FI     : Field<FI,     46> {};     // Floating-Point Fraction Inexact
  struct FPRF   : Field<FPRF,   47, 51>     // Floating-Point Result Flags
  {
    static uint64_t          QNAN    () { return 0x11; }
    static uint64_t NEGATIVE_INFINITY() { return 0x9; }
    static uint64_t NEGATIVE_NORMAL  () { return 0x8; }
    static uint64_t NEGATIVE_DENORMAL() { return 0x18; }
    static uint64_t NEGATIVE_ZERO    () { return 0x12; }
    static uint64_t POSITIVE_ZERO    () { return 0x2; }
    static uint64_t POSITIVE_DENORMAL() { return 0x14; }
    static uint64_t POSITIVE_NORMAL  () { return 0x4; }
    static uint64_t POSITIVE_INFINITY() { return 0x5; }
  };
  struct C      : Field<C,      47> {};     // Floating-Point Result Class Descriptor
  struct FPCC   : Field<FPCC,   48, 51> {}; // Floating-Point Condition Code
  struct FL     : Field<FL,     48> {};     // Floating-Point Less Than or Negative
  struct FG     : Field<FG,     49> {};     // Floating-Point Greater Than or Positive
  struct FE     : Field<FE,     50> {};     // Floating-Point Equal or Zero
  struct FU     : Field<FU,     51> {};     // Floating-Point Unordered or NaN
  struct VXSOFT : Field<VXSOFT, 53> {};     // Floating-Point Invalid Operation Exception (Software-Defined Condition)
  struct VXSQRT : Field<VXSQRT, 54> {};     // Floating-Point Invalid Operation Exception (Invalid Square Root)
  struct VXCVI  : Field<VXCVI,  55> {};     // Floating-Point Invalid Operation Exception (Invalid Integer Convert)
  struct VE     : Field<VE,     56> {};     // Floating-Point Invalid Operation Exception Enable
  struct OE     : Field<OE,     57> {};     // Floating-Point Overflow Exception Enable
  struct UE     : Field<UE,     58> {};     // Floating-Point Underflow Exception Enable
  struct ZE     : Field<ZE,     59> {};     // Floating-Point Zero Divide Exception  Enable
  struct XE     : Field<XE,     60> {};     // Floating-Point Inexact Exception Enable
  struct NI     : Field<NI,     61> {};     // Floating-Point Non-IEEE Mode
  struct RN     : Field<RN,     62, 63> {}; // Floating-Point Rounding Control
  
  FPSCR(UINT _value) : Super(_value) {}
};

// Machine State Register
struct MSR : Register<MSR>
{
  typedef Register<MSR> Super;
		
  struct SPV : Field<SPV,38> {}; // SP/Embedded FP/Vector available
  struct WE  : Field<WE ,45> {}; // Wait state (Power management) enable
  struct CE  : Field<CE ,46> {}; // Critical Interrupt Enable
  struct EE  : Field<EE ,48> {}; // External Interrupt Enable
  struct PR  : Field<PR ,49> {}; // Problem State
  struct FP  : Field<FP ,50> {}; // Floating-Point Available
  struct ME  : Field<ME ,51> {}; // Machine Check Enable
  struct FE0 : Field<FE0,52> {}; // Floating-point exception mode 0 (not used)
  struct DE  : Field<DE ,54> {}; // Debug Interrupt Enable
  struct FE1 : Field<FE1,55> {}; // Floating-point exception mode 1 (not used)
  struct IS  : Field<IS ,58> {}; // Instruction Address Space
  struct DS  : Field<DS ,59> {}; // Data Address Space
  struct PMM : Field<PMM,61> {}; // PMM Performance monitor mark bit
  struct RI  : Field<RI ,62> {}; // Recoverable Interrupt
		
  MSR(UINT _value) : Super(_value) {}
};

// Integer Exception Register
struct XER : Register<XER>
{
  typedef Register<XER> Super;
	
  struct SO         : Field<SO        ,32> {};      // Summary Overflow
  struct OV         : Field<OV        ,33> {};      // Overflow
  struct CA         : Field<CA        ,34> {};      // Carry
  struct BYTE_COUNT : Field<BYTE_COUNT,57, 63> {}; // Byte count
   
  struct _0_3       : Field<_0_3,32,35> {}; // XER[0-3]
	
  XER(UINT _value) : Super(_value) {}
};

// Condition Register
struct CR : Register<CR>
{
  typedef Register<CR> Super;
	
  struct CR0 : Field<CR0,32,35> { struct LT : Field<LT,32> {}; struct GT : Field<GT,33> {}; struct EQ : Field<EQ,34> {}; struct SO : Field<SO,35> {}; };
  struct CR1 : Field<CR1,36,39> { struct LT : Field<LT,36> {}; struct GT : Field<GT,37> {}; struct EQ : Field<EQ,38> {}; struct SO : Field<SO,39> {}; };
  struct CR2 : Field<CR2,40,43> { struct LT : Field<LT,40> {}; struct GT : Field<GT,41> {}; struct EQ : Field<EQ,42> {}; struct SO : Field<SO,43> {}; };
  struct CR3 : Field<CR3,44,47> { struct LT : Field<LT,44> {}; struct GT : Field<GT,45> {}; struct EQ : Field<EQ,46> {}; struct SO : Field<SO,47> {}; };
  struct CR4 : Field<CR4,48,51> { struct LT : Field<LT,48> {}; struct GT : Field<GT,49> {}; struct EQ : Field<EQ,50> {}; struct SO : Field<SO,51> {}; };
  struct CR5 : Field<CR5,52,55> { struct LT : Field<LT,52> {}; struct GT : Field<GT,53> {}; struct EQ : Field<EQ,54> {}; struct SO : Field<SO,55> {}; };
  struct CR6 : Field<CR6,56,59> { struct LT : Field<LT,56> {}; struct GT : Field<GT,57> {}; struct EQ : Field<EQ,58> {}; struct SO : Field<SO,59> {}; };
  struct CR7 : Field<CR7,60,63> { struct LT : Field<LT,60> {}; struct GT : Field<GT,61> {}; struct EQ : Field<EQ,62> {}; struct SO : Field<SO,63> {}; };
	
  CR(UINT _value) : Super(_value) {}
};

// Link Register
typedef U64 LR;

// Count Register
typedef U64 CTR;
  
struct Arch
  : public unisim::kernel::service::Service<unisim::service::interfaces::Registers>
  , public unisim::kernel::service::Service<unisim::service::interfaces::MemoryInjection<uint64_t> >
  , public unisim::kernel::service::Service<unisim::service::interfaces::Memory<uint64_t> >
  , public unisim::kernel::service::Service<unisim::service::interfaces::Disassembly<uint64_t> >
  , public unisim::kernel::service::Service<unisim::service::interfaces::MemoryAccessReportingControl>
  , public unisim::kernel::service::Client<unisim::service::interfaces::DebugYielding>
  , public unisim::kernel::service::Client<unisim::service::interfaces::TrapReporting>
  , public unisim::kernel::service::Client<unisim::service::interfaces::MemoryAccessReporting<uint64_t> >
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
  
  // unisim::service::interfaces::Registers
  virtual unisim::service::interfaces::Register* GetRegister( const char* name );
  virtual void ScanRegisters( unisim::service::interfaces::RegisterScanner& scanner );
  unisim::kernel::service::ServiceExport<unisim::service::interfaces::Registers> registers_export;

  // unisim::service::interfaces::MemoryInjection<uint64_t>
  virtual bool InjectReadMemory(uint64_t addr, void* buffer, unsigned size);
  virtual bool InjectWriteMemory(uint64_t addr, void const* buffer, unsigned size);
  unisim::kernel::service::ServiceExport<unisim::service::interfaces::MemoryInjection<uint64_t> > memory_injection_export;

  // unisim::service::interfaces::Memory<uint64_t>
  virtual void Reset() {}
  virtual bool ReadMemory(uint64_t addr, void* buffer, unsigned size );
  virtual bool WriteMemory(uint64_t addr, void const* buffer, unsigned size);
  unisim::kernel::service::ServiceExport<unisim::service::interfaces::Memory<uint64_t> > memory_export;
  
  // unisim::service::interfaces::Disassembly<uint64_t>
  virtual std::string Disasm(ADDRESS addr, ADDRESS& next_addr);
  unisim::kernel::service::ServiceExport<unisim::service::interfaces::Disassembly<uint64_t> > disasm_export;
  
  // unisim::service::interfaces::MemoryAccessReportingControl
  virtual void RequiresMemoryAccessReporting(unisim::service::interfaces::MemoryAccessReportingType type, bool report);
  unisim::kernel::service::ServiceExport<unisim::service::interfaces::MemoryAccessReportingControl> memory_access_reporting_control_export;
  bool requires_memory_access_reporting;      //< indicates if the memory accesses require to be reported
  bool requires_fetch_instruction_reporting;  //< indicates if the fetched instructions require to be reported
  bool requires_commit_instruction_reporting; //< indicates if the committed instructions require to be reported
  
  // unisim::service::interfaces::DebugYielding
  unisim::kernel::service::ServiceImport<unisim::service::interfaces::DebugYielding> debug_yielding_import;

  // unisim::service::interfaces::TrapReporting
  unisim::kernel::service::ServiceImport<unisim::service::interfaces::TrapReporting> trap_reporting_import;

  // unisim::service::interfaces::MemoryAccessReporting<uint64_t>
  unisim::kernel::service::ServiceImport<unisim::service::interfaces::MemoryAccessReporting<uint64_t> > memory_access_reporting_import;
  
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

  bool Fp64Load(unsigned id, U64 addr) { fprs[id].fromRawBitsAssign(IntLoad<U64>( addr )); return true; }
  bool Fp64Store(unsigned id, U64 addr) { IntStore( addr, U64(fprs[id].queryRawBits()) ); return true; }
  bool Fp32Load(unsigned id, U64 addr);
  bool Fp32Store(unsigned id, U64 addr);
  bool FpStoreLSW(unsigned id, U64 addr);
  
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
  
  uintptr_t       insn_count;
  U64             time_base;

  static bool const HAS_FPU = true;
  static bool const HAS_FLOATING_POINT_GRAPHICS_INSTRUCTIONS = true;
  static bool const HAS_FLOATING_POINT_SQRT = true;

  // unisim::kernel::service::ServiceImport<unisim::service::interfaces::SymbolTableLookup<uint64_t> > symbol_table_lookup_import;
  // unisim::kernel::service::ServiceImport<unisim::service::interfaces::LinuxOS> linux_os_import;

};

typedef Arch CPU;
typedef Arch CONFIG;

U64 UnsignedMultiplyHigh( U64 lop, U64 rop );
S64 SignedMultiplyHigh( S64 lop, S64 rop );

#endif // __E5500FPV_ARCH_HH__
