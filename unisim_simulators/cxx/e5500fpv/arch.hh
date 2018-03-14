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
#include <unisim/component/cxx/memory/sparse/memory.hh>
#include <unisim/component/cxx/processor/powerpc/isa/book_vle/vle.hh>
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
  };
  
  struct SystemCallInterrupt
  {
    struct SystemCall { typedef SystemCallInterrupt Interrupt; };
    SystemCallInterrupt() {}
    void SetELEV(unsigned _elev) { if (_elev != 0) throw *this; /* Hypervisor interrupt ? */ }
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
  U64  GetGPR( unsigned id ) { return gprs[id]; }
  bool MoveFromSPR( unsigned  id, U64& value ) { return false; }
  bool MoveToSPR( unsigned  id, U64 value ) { return false; }
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

  bool Int8Store(unsigned id, U64 addr) { IntStore( addr, U8(gprs[id]) ); return true; }
  bool Int16Store(unsigned id, U64 addr) { IntStore( addr, U16(gprs[id]) ); return true; }
  bool Int32Store(unsigned id, U64 addr) { IntStore( addr, U32(gprs[id]) ); return true; }
  bool Int64Store(unsigned id, U64 addr) { IntStore( addr, U64(gprs[id]) ); return true; }

  bool Int8Load(unsigned id, U64 addr) { gprs[id] = IntLoad<U8>( addr ); return true; }
  bool Int16Load(unsigned id, U64 addr) { gprs[id] = IntLoad<U16>( addr );return true; }
  bool Int32Load(unsigned id, U64 addr) { gprs[id] = IntLoad<U32>( addr );return true; }
  bool Int64Load(unsigned id, U64 addr) { gprs[id] = IntLoad<U64>( addr );return true; }

  bool SInt8Load(unsigned id, U64 addr) { gprs[id] = IntLoad<S8>( addr ); return true; }
  bool SInt16Load(unsigned id, U64 addr) { gprs[id] = IntLoad<S16>( addr ); return true; }
  bool SInt32Load(unsigned id, U64 addr) { gprs[id] = IntLoad<S32>( addr ); return true; }
  bool SInt64Load(unsigned id, U64 addr) { gprs[id] = IntLoad<S64>( addr ); return true; }

  bool InstructionFetch(uint64_t addr, uint32_t& insn);
  Operation* fetch();
  void commit();

  Decoder         decoder;
  Memory          memory;
  LinuxOS*        linux_os;
  std::map<std::string,unisim::service::interfaces::Register*> regmap;
  U64             gprs[32];
  U64             ctr, lr, cia, nia;
  XER             xer;
  CR              cr;
  double          fprs[32];
  uintptr_t       instcount;
};

typedef Arch CPU;

#endif // __E5500FPV_ARCH_HH__
