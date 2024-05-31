/*
 *  Copyright (c) 2007-2021,
 *  Commissariat a l'Energie Atomique (CEA)
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or withou
 *  modification, are permitted provided that the following conditions are met:
 *
 *   - Redistributions of source code must retain the above copyright notice
 *     this list of conditions and the following disclaimer.
 *
 *   - Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 *   - Neither the name of CEA nor the names of its contributors may be used to
 *     endorse or promote products derived from this software without specifi
 *     prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, TH
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOS
 *  ARE DISCLAIMED.
 *  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR AN
 *  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGE
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AN
 *  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TOR
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE O
 *  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: Yves Lhuillier (yves.lhuillier@cea.fr)
 */

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_VMSAV8_CPU_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_VMSAV8_CPU_HH__

#include <unisim/component/cxx/processor/arm/isa_arm64.hh>
#include <unisim/component/cxx/processor/arm/regs64/cpu.hh>
#include <unisim/component/cxx/processor/opcache/opcache.hh>
#include <unisim/component/cxx/vector/vector.hh>
#include <unisim/service/interfaces/memory_access_reporting.hh>
#include <unisim/service/interfaces/trap_reporting.hh>
#include <unisim/kernel/logger/logger.hh>
#include <unisim/util/endian/endian.hh>
#include <unisim/util/inlining/inlining.hh>
#include <unisim/util/debug/simple_register_registry.hh>
#include <unisim/service/interfaces/registers.hh>
#include <unisim/service/interfaces/register.hh>
#include <unisim/service/interfaces/debug_yielding.hh>
#include <unisim/service/interfaces/symbol_table_lookup.hh>
#include <unisim/service/interfaces/memory.hh>
#include <unisim/service/interfaces/disassembly.hh>
#include <unisim/service/interfaces/linux_os.hh>
#include <unisim/service/interfaces/memory_injection.hh>
#include <map>
#include <set>
#include <stdexcept>
#include <iosfwd>
#include <inttypes.h>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace arm {
namespace vmsav8 {

struct ArchTypes
{
  // typedef simfloat::FP FP;
  // typedef FP::F64  F64;
  // typedef FP::F32  F32;
  typedef uint8_t      U8;
  typedef uint16_t     U16;
  typedef uint32_t     U32;
  typedef uint64_t     U64;
  typedef int8_t       S8;
  typedef int16_t      S16;
  typedef int32_t      S32;
  typedef int64_t      S64;
  typedef bool         BOOL;
  typedef float        F32;
  typedef double       F64;

  template <typename T>
  using VectorTypeInfo = unisim::component::cxx::vector::VectorTypeInfo<T,0>;
  using VectorByte = U8;
  using VectorByteShadow = U8;
};

/** Armv8 cpu
 *
 * Defines and implements architectural state of armv8 processors.
 */

template <typename CPU_IMPL>
struct CPU
  : public virtual unisim::kernel::Object
  , public ArchTypes
  , regs64::CPU<CPU_IMPL, ArchTypes>
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
{
  typedef CPU<CPU_IMPL> this_type;
  
  typedef isa::arm64::Operation<CPU_IMPL> Operation;
  typedef isa::arm64::Decoder<CPU_IMPL> A64Decoder;
  typedef opcache::OpCache<A64Decoder>  Decoder;

  typedef U64      UREG;
  typedef S64      SREG;

  typedef this_type DisasmState;

  using typename regs64::CPU<CPU_IMPL, ArchTypes>::branch_type_t;
  using          regs64::CPU<CPU_IMPL, ArchTypes>::gpr;

  /**********************************************************************
   ***                   Constructors / Destructors                   ***
   **********************************************************************/

  CPU(const char* name, Object* parent);
  ~CPU();

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

  void Setup(unisim::service::interfaces::Memory<uint64_t>*) override { memory_import.RequireSetup(); }
  unisim::kernel::ServiceExport<unisim::service::interfaces::Memory<uint64_t> > memory_export;
  virtual bool ReadMemory( uint64_t addr, void* buffer, uint32_t size ) { return static_cast<CPU_IMPL*>(this)->ExternalReadMemory( addr, (uint8_t*)buffer, size ); }
  virtual bool WriteMemory( uint64_t addr, void const* buffer, uint32_t size ) { return static_cast<CPU_IMPL*>(this)->ExternalWriteMemory( addr, (uint8_t*)buffer, size ); }

  //=====================================================================
  //=                   Disassembly interface methods                   =
  //=====================================================================

  void Setup(unisim::service::interfaces::Disassembly<uint64_t>*) override { memory_import.RequireSetup(); }
  unisim::kernel::ServiceExport<unisim::service::interfaces::Disassembly<uint64_t> > disasm_export;
  virtual std::string Disasm( uint64_t addr, uint64_t& next_addr );

  //=====================================================================
  //=             Memory access reporting interface methods             =
  //=====================================================================

  unisim::kernel::ServiceExport<unisim::service::interfaces::MemoryAccessReportingControl> memory_access_reporting_control_export;
  virtual void RequiresMemoryAccessReporting( unisim::service::interfaces::MemoryAccessReportingType type, bool report );

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

  /**********************************************************************
   ***                Functional methods and members                  ***
   **********************************************************************/

  void StepInstruction();

  void UndefinedInstruction( Operation const* insn );
  void UndefinedInstruction();

  bool Cond( bool cond ) { return cond; }

  //====================================================================
  //=                    Registers access methods
  //====================================================================

  enum AccessReport { report_none = 0, report_simd_access = report_none, report_gsr_access = report_none, report_gzr_access = report_none };
  void report(AccessReport, unsigned, bool) const {}

  U8  GetTVU8(unsigned r0, unsigned elts, unsigned regs, U8 idx, U8 oob) { auto r = idx/elts; return r < regs ? this->GetVU8((r0 + r) % 32, idx % elts) : oob; }

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
  void SetNZCV( uint32_t n, uint32_t z, uint32_t c, uint32_t v )
  {
    nzcv = (n << 3) | (z << 2) | (c << 1) | (v << 0);
  }

  uint8_t GetNZCV() const { return nzcv; }
  uint8_t GetCarry() const { return (nzcv >> 1) & 1; }
  
  /** Set FPCR */
  void SetFPCR( U32 v ) { fpcr = v; }
  
  /** Get FPCR */
  U32 GetFPCR() const { return fpcr; }
  
  /* Get FPCR Floating-point control bits */
  BOOL DN() const { return ((fpcr >> 25) & U32(1)) != U32(0); }
  BOOL FZ() const { return ((fpcr >> 24) & U32(1)) != U32(0); }
  U32 RMode() const { return (fpcr >> 22) & U32(3); }
  BOOL RoundToNearest() const { return RMode() == U32(0); }
  BOOL RoundTowardPlusInfinity() const { return RMode() == U32(1); }
  BOOL RoundTowardMinusInfinity() const { return RMode() == U32(2); }
  BOOL RoundTowardZero() const { return RMode() == U32(3); }
  BOOL IDE() const { return ((fpcr >> 15) & U32(1)) != U32(0); }
  BOOL IXE() const { return ((fpcr >> 12) & U32(1)) != U32(0); }
  BOOL UFE() const { return ((fpcr >> 11) & U32(1)) != U32(0); }
  BOOL OFE() const { return ((fpcr >> 10) & U32(1)) != U32(0); }
  BOOL DZE() const { return ((fpcr >> 9) & U32(1)) != U32(0); }
  BOOL IOE() const { return ((fpcr >> 9) & U32(1)) != U32(0); }

  /** Set FPSR */
  void SetFPSR( U32 v ) { fpsr = v; }
  
  /** Set FPSR cumulative bits
   * 
   * @param q Saturation flag
   * @param id Input Denormal flag
   * @param ix Inexact flag
   * @param uf Underflow flag
   * @param of Overflow flag
   * @param dz Divide by Zero flag
   * @param io Invalid Operation flag
   */
  void SetFPSR( U32 q, U32 id, U32 ix, U32 uf, U32 of, U32 dz, U32 io )
  {
    fpsr |= (q << 27) | (id << 7) | (ix << 4) | (uf << 3) | (of << 2) | (dz << 1) | (io << 0);
  }
  
  /** Get FPSR */
  U32 GetFPSR() const { return fpsr; }
  
  /** Get the current Program Counter */
  uint64_t GetPC() { return current_insn_addr; }

  /** Get the next Program Counter */
  uint64_t GetNPC() { return next_insn_addr; }

  /** Manage System Registers **/
  struct SysReg
  {
    struct Encoding { unsigned op0 : 4; unsigned op1 : 4; unsigned crn : 4; unsigned crm : 4; unsigned op2 : 4; };
    virtual void Name(Encoding, std::ostream&) const {};
    virtual void Describe(Encoding, std::ostream&) const {};
    virtual void Describe(Encoding, char const* prefix, std::ostream&) const;
    virtual char const* ReadOperation() const { return "mrs"; }
    virtual char const* WriteOperation() const { return "msr"; };
    virtual void Write(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, CPU_IMPL& cpu, U64 value) const;
    virtual U64 Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2,  CPU_IMPL& cpu) const;
    virtual void DisasmRead(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, std::ostream& sink) const;
    virtual void DisasmWrite(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, std::ostream& sink) const;
  };

  static SysReg const* GetSystemRegister( uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2 );
  void        CheckSystemAccess( uint8_t op1 );

  //=====================================================================
  //=                      Control Transfer methods                     =
  //=====================================================================

  /** Set the next Program Counter */
  void BranchTo( uint64_t addr, branch_type_t branch_type ) { next_insn_addr = addr; }
  bool Test( bool cond ) { return cond; }
  void SoftwareBreakpoint( uint32_t imm );
  void CallSupervisor( uint32_t imm );
  void CallHypervisor( uint32_t imm );
  void ExceptionReturn();

  //=====================================================================
  //=                       Memory access methods                       =
  //=====================================================================

  template <typename T>
  T
  MemReadT(uint64_t addr)
  {
    unsigned const size = sizeof (T);
    uint8_t buffer[size];
    MemRead( &buffer[0], addr, size );
    T res(0);
    for (unsigned idx = size; --idx < size; )
      res = (res << 8) | T(buffer[idx]);
    return res;
  }

  uint64_t MemRead64(uint64_t addr) { return MemReadT<uint64_t>(addr); }
  uint32_t MemRead32(uint64_t addr) { return MemReadT<uint32_t>(addr); }
  uint16_t MemRead16(uint64_t addr) { return MemReadT<uint16_t>(addr); }
  uint8_t  MemRead8 (uint64_t addr) { return MemReadT<uint8_t> (addr); }

  void MemRead( uint8_t* buffer, uint64_t addr, unsigned size );

  template <typename T>
  void
  MemWriteT(uint64_t addr, T val)
  {
    unsigned const size = sizeof (T);
    uint8_t buffer[size];
    for (unsigned idx = 0; idx < size; ++idx)
      { buffer[idx] = val; val >>= 8; }
    MemWrite( addr, &buffer[0], size );
  }

  void MemWrite64(uint64_t addr, uint64_t val) { MemWriteT(addr, val); }
  void MemWrite32(uint64_t addr, uint32_t val) { MemWriteT(addr, val); }
  void MemWrite16(uint64_t addr, uint16_t val) { MemWriteT(addr, val); }
  void MemWrite8 (uint64_t addr, uint8_t  val) { MemWriteT(addr, val); }

  void MemWrite( uint64_t addr, uint8_t const* buffer, unsigned size );

  void     SetExclusiveMonitors( uint64_t addr, unsigned size ) { /*TODO: MP support*/ }
  bool     ExclusiveMonitorsPass( uint64_t addr, unsigned size ) { /*TODO: MP support*/ return true; }
  void     ClearExclusiveLocal() {}

protected:

  /**********************************************************************
   ***                   Non functional variables                     ***
   **********************************************************************/

  //=====================================================================
  //=                              Logger                               =
  //=====================================================================

  /** Unisim logging services. */
  unisim::kernel::logger::Logger logger;

  /** Verbosity of the CPU implementation */
  bool verbose;

  //=====================================================================
  //=                  Registers interface structures                   =
  //=====================================================================

  /** The registers interface for debugging purpose */
  typedef unisim::util::debug::SimpleRegisterRegistry RegistersRegistry;
  RegistersRegistry registers_registry;

  typedef std::set<unisim::kernel::VariableBase*> VariableRegisterPool;
  VariableRegisterPool variable_register_pool;

  /**********************************************************************
   ***                       Architectural state                      ***
   **********************************************************************/

  uint32_t nzcv;
  U32 fpcr;
  U32 fpsr;
  uint64_t current_insn_addr, next_insn_addr;

  uint64_t TPIDRURW; //< User Read/Write Thread ID Register

  virtual void          ResetSystemRegisters();

protected:
  virtual void Sync() = 0;

  //=====================================================================
  //=                          Memory Accesses                          =
  //=====================================================================

  struct IPB
  {
    static unsigned const LINE_SIZE = 32; //< IPB: Instruction prefetch buffer
    uint8_t bytes[LINE_SIZE];             //< The instruction prefetch buffer
    uint64_t base_address;                //< base address of IPB content (cache line size aligned if valid)
    IPB() : bytes(), base_address( -1 ) {}
    uint8_t* get(this_type& core, U64 address);
  } ipb;

  void ReadInsn( uint64_t address, isa::arm64::CodeType& insn );

  /** Decoder for the ARM32 instruction set. */
  Decoder decoder;

  // // Intrusive memory accesses
  // virtual bool  PhysicalReadMemory( uint64_t addr, uint8_t*       buffer, unsigned size ) = 0;
  // virtual bool PhysicalWriteMemory( uint64_t addr, uint8_t const* buffer, unsigned size ) = 0;
  // Non-intrusive memory accesses
  bool  ExternalReadMemory( uint64_t addr, uint8_t*       buffer, unsigned size );
  bool ExternalWriteMemory( uint64_t addr, uint8_t const* buffer, unsigned size );

  bool requires_memory_access_reporting;      //< indicates if the memory accesses require to be reported
  bool requires_fetch_instruction_reporting;  //< indicates if the fetched instructions require to be reported
  bool requires_commit_instruction_reporting; //< indicates if the committed instructions require to be reported

  void ReportMemoryAccess( unisim::util::debug::MemoryAccessType mat, unisim::util::debug::MemoryType mtp,
                           uint64_t addr, uint32_t size )
  {
    if (requires_memory_access_reporting and memory_access_reporting_import)
      memory_access_reporting_import->ReportMemoryAccess(mat, mtp, addr, size);
  }
};

template <typename CPU, typename T>
T FPMulAdd(CPU& cpu, T acc, T op1, T op2)
{
  return acc + (op1 * op2);
}

template <typename CPU, typename T>
T FPMulSub(CPU& cpu, T acc, T op1, T op2)
{
  return acc - (op1 * op2);
}

} // end of namespace vmsav8
} // end of namespace arm
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_VMSAV8_CPU_HH__
