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

#include <architecture.hh>
#include <unisim/component/cxx/processor/arm/isa/arm64/disasm.hh>
#include <unisim/util/debug/simple_register.hh>
#include <unisim/util/os/linux_os/linux.hh>
#include <unisim/util/os/linux_os/aarch64.hh>
#include <unisim/util/likely/likely.hh>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <termios.h>
#include <unistd.h>

AArch64::AArch64(sc_core::sc_module_name const& name, Object* parent)
  : unisim::kernel::Object(name, parent)
  , unisim::kernel::Client<unisim::service::interfaces::DebugYielding>(name, parent)
  , unisim::kernel::Client<unisim::service::interfaces::TrapReporting>(name, parent)
  , unisim::kernel::Client<unisim::service::interfaces::SymbolTableLookup<uint64_t> >(name, parent)
  , unisim::kernel::Client<unisim::service::interfaces::Memory<uint64_t> >(name, parent)
  , unisim::kernel::Client<unisim::service::interfaces::LinuxOS>(name, parent)
  , unisim::kernel::Client<unisim::service::interfaces::MemoryAccessReporting<uint64_t> >(name, parent)
  , unisim::kernel::Service<unisim::service::interfaces::Registers>(name, parent)
  , unisim::kernel::Service<unisim::service::interfaces::Memory<uint64_t> >(name, parent)
  , unisim::kernel::Service<unisim::service::interfaces::Disassembly<uint64_t> >(name, parent)
  , unisim::kernel::Service<unisim::service::interfaces::MemoryAccessReportingControl>(name, parent)
  , unisim::kernel::Service<unisim::service::interfaces::MemoryInjection<uint64_t> >(name, parent)
  , unisim::kernel::Service<unisim::service::interfaces::HttpServer>(name, parent)
  , regmap()
  , pages()
  , excmon()
  , ipb()
  , decoder()
  , gpr()
  , vector_views()
  , vectors()
  , fpcr()
  , fpsr()
  , sp_el()
  , el1()
  , pstate()
  , nzcv()
  , current_insn_addr()
  , next_insn_addr()
  , insn_counter()
  , insn_timer()
  , next_quantum_timer()
  , quantum_cycles(1024)
  , quantum_delay(2500000.0, sc_core::SC_PS)
  , last_insns()
  , TPIDR()
  , CPACR()
  , bdaddr(0)
  , random(0)
  , terminate(false)
  , disasm(true)
  , suspend(false)
  , registers_export("registers-export", this)
  , memory_export("memory-export", this)
  , disasm_export("disasm-export", this)
  , memory_access_reporting_control_export("memory-access-reporting-control-export", this)
  , memory_injection_export("memory-injection-export", this)
  , debug_yielding_import("debug-yielding-import", this)
  , trap_reporting_import("trap-reporting-import", this)
  , symbol_table_lookup_import("symbol-table-lookup-import", this)
  , memory_import("memory-import", this)
  , memory_access_reporting_import("memory-access-reporting-import", this)
  , linux_os_import("linux-os-import", this)
  , http_server_export("http-server-export", this)
{
  struct GPRView : public unisim::service::interfaces::Register
  {
    GPRView(AArch64& core, unsigned idx) : name(), value(core.gpr[idx])
    {
      std::ostringstream regname;
      regname << unisim::component::cxx::processor::arm::isa::arm64::DisasmGSXR(idx);
      name = regname.str();
    }
    virtual const char *GetName() const { return name.c_str(); }
    virtual const char *GetDescription() const { return name.c_str(); }
    virtual void GetValue(void* buffer) const { *(uint64_t*)buffer = value.value; }
    virtual void SetValue(const void* buffer) { value.value = *(uint64_t *)buffer; value.ubits = 0; }
    virtual int GetSize() const { return sizeof(uint64_t); }

    std::string name;
    U64& value;
  };
  for (int idx = 0; idx < 32; ++idx)
    { auto regview = new GPRView(*this, idx); regmap[regview->name] = regview; }

  /** Specific Program Counter Register Debugging Accessor */
  struct ProgramCounterRegister : public unisim::service::interfaces::Register
  {
    ProgramCounterRegister( AArch64& _cpu ) : cpu(_cpu) {}
    virtual const char *GetName() const { return "pc"; }
    virtual void GetValue( void* buffer ) const { *((uint64_t*)buffer) = cpu.current_insn_addr; }
    virtual void SetValue( void const* buffer ) { cpu.BranchTo(U64(*(uint64_t const*)buffer), cpu.B_DBG); }
    virtual int  GetSize() const { return 8; }
    virtual void Clear() { /* Clear is meaningless for PC */ }
    AArch64&        cpu;
  };

  regmap["pc"] = new ProgramCounterRegister( *this );

  SC_THREAD(run);
}

AArch64::~AArch64()
{
  for (auto reg : regmap)
    delete reg.second;
}

bool
AArch64::ReadMemory(uint64_t addr, void* buffer, unsigned size)
{
  struct Bad {};

  uint64_t count = size;
  uint8_t* ptr = static_cast<uint8_t*>(buffer);

  while (count > 0)
    {
      Page const& page = get_page(addr);
      uint64_t done = std::min(count, page.size_from(addr));
      uint8_t *udat = page.udat_abs(addr), *data = page.data_abs(addr);
      if (std::any_of(&udat[0], &udat[done], [](uint8_t b) { return b != 0; } )) throw Bad();
      std::copy( &data[0], &data[done], ptr );
      count -= done;
      ptr += done;
      addr += done;
    }

  return true;
}

bool
AArch64::WriteMemory(uint64_t addr, void const* buffer, unsigned size)
{
  struct Bad {};

  uint64_t count = size;
  uint8_t const* ptr = static_cast<uint8_t const*>(buffer);

  while (count > 0)
    {
      Page const& page = get_page(addr);
      uint64_t done = std::min(count, page.size_from(addr));
      uint8_t *udat = page.udat_abs(addr), *data = page.data_abs(addr);
      std::fill(&udat[0], &udat[done], 0);
      std::copy(&ptr[0], &ptr[done], data);
      count -= done;
      ptr += done;
      addr += done;
    }

  return true;
}

unisim::service::interfaces::Register*
AArch64::GetRegister(char const* name)
{
  auto reg = regmap.find( name );
  return (reg == regmap.end()) ? 0 : reg->second;
}

void
AArch64::ScanRegisters(unisim::service::interfaces::RegisterScanner& scanner)
{
  // General purpose registers
  for (unsigned reg = 0; reg < 31; ++reg)
    {
      std::ostringstream buf;
      buf << 'x' << std::dec << reg;
      scanner.Append( GetRegister( buf.str().c_str() ) );
    }
  scanner.Append( GetRegister("sp") );
  scanner.Append( GetRegister("pc") );
}

bool
AArch64::InjectReadMemory(uint64_t addr, void *buffer, unsigned size)
{
  return ReadMemory(addr, buffer, size);
}

bool
AArch64::InjectWriteMemory(uint64_t addr, void const* buffer, unsigned size)
{
  return WriteMemory(addr, buffer, size);
}

AArch64::Page::~Page()
{
  free->free(*this);
}

void AArch64::Page::dump_range( std::ostream& sink ) const
{
  sink << '[' << std::hex << base << ':' << last << std::dec << ']';
}

void
AArch64::error_mem_overlap( Page const& a, Page const& b )
{
  struct TODO {}; throw TODO();
}

bool
AArch64::mem_map(Page&& page)
{
  auto below = pages.lower_bound(page);
  if (below != pages.end() and not Zone::Above()(page, *below))
    {
      error_mem_overlap(page, *below);
      return false;
    }
  if (pages.size() and below != pages.begin() and not Zone::Above()(*std::prev(below), page))
    {
      error_mem_overlap(page, *std::prev(below));
      return false;
    }

  pages.insert(below,std::move(page));
  return true;
}

AArch64::Page const&
AArch64::alloc_page(Pages::iterator pi, uint64_t addr)
{
  uint64_t const PAGE_SIZE = 0x10000;
  uint64_t base = addr & -PAGE_SIZE, last = base + PAGE_SIZE - 1;
  if (pi != pages.end() and pi->last >= base)
    base = pi->last + 1;
  if (pages.size() and pi != pages.begin() and (--pi)->base <= last)
    { last = pi->base - 1; }

  Page res(base, last);
  // Initialize taint
  std::fill(res.udat_beg(), res.udat_end(), -1);
  //std::fill(res.udat_beg(), res.udat_end(), 0);
  std::fill(res.data_beg(), res.data_end(), 0);
  return *pages.insert(pi, std::move(res));
}

AArch64::Page::Page(uint64_t base, uint64_t last)
  : Zone(base, last), data(), udat(), free()
{
  uint64_t size = last-base+1;
  if (size >= 0x4000000) { struct OverSizedPage {}; raise( OverSizedPage() ); }
  static struct : Page::Free { void free(Page& page) const override { delete [] page.data; delete [] page.udat; } } lefree;
  free = &lefree;
  data = new uint8_t[size];
  udat = new uint8_t[size];
}

AArch64::Page::Free AArch64::Page::Free::nop;

uint8_t*
AArch64::IPB::access(AArch64& core, uint64_t paddr)
{
  // LINE_SIZE should be so MMU page boundaries cannot be crossed
  uint64_t req_base_address = paddr & -uint64_t(LINE_SIZE);
  unsigned idx = paddr % LINE_SIZE;
  if (base_address != req_base_address)
    {
      uint8_t ubuf[LINE_SIZE];
      if (core.get_page(req_base_address).read(req_base_address, &bytes[0], &ubuf[0], LINE_SIZE) != LINE_SIZE)
        { struct Bad {}; raise( Bad() ); }
      if (not std::all_of( &ubuf[0], &ubuf[LINE_SIZE], [](unsigned char const byte) { return byte == 0; } ))
        { struct Bad {}; raise( Bad() ); }
      base_address = req_base_address;
      
      core.ReportMemoryAccess(unisim::util::debug::MAT_READ, unisim::util::debug::MT_INSN, base_address, LINE_SIZE);
    }

  return &bytes[idx];
}

AArch64::InstructionInfo const& AArch64::last_insn( int idx ) const
{
  return last_insns[(insn_counter + histsize + idx) % histsize];
}


void
AArch64::UndefinedInstruction(unisim::component::cxx::processor::arm::isa::arm64::Operation<AArch64> const* op)
{
  DisasmState ds;
  op->disasm(ds, std::cerr << "Undefined instruction : `");
  std::cerr << "` (" << op->GetName() << ", " << std::hex << op->GetEncoding()
            << "@" << std::hex << op->GetAddr() << ").\n";
  UndefinedInstruction();
}

void
AArch64::UndefinedInstruction()
{
  struct Bad {}; raise( Bad() );
}

void
AArch64::TODO()
{
  //struct Bad {}; raise( Bad() );
  //  std::cerr << "!TODO!\n";
}

/** CallSupervisor
 *
 *  This method is called by SWI/SVC instructions to handle software interrupts.
 * @param imm     the "imm" field of the instruction code
 */
void
AArch64::CallSupervisor( unsigned imm )
{
  if (linux_os_import)
    {
      // we are executing on linux emulation mode, use linux_os_import
      try
        {
          linux_os_import->ExecuteSystemCall(imm);
          if (trap_reporting_import)
            trap_reporting_import->ReportTrap(*this, "CallSupervisor");
        }
      catch (std::exception const& e)
        {
          std::cerr << e.what() << std::endl;
          this->Stop( -1 );
        }
    }
  else
    {
      struct SVCException{}; throw SVCException();
    }
}

/** CallHypervisor
 *
 *  This method is called by HVC instructions to handle software interrupts.
 * @param imm     the "imm" field of the instruction code
 */
void
AArch64::CallHypervisor( uint32_t imm )
{
  struct Bad {};

  if (imm == 0)
    {
      U64 const& arg0 = gpr[0];

      if (arg0.ubits)
        raise( Bad() );

      switch (arg0.value)
        {
        default:
          raise( Bad() );
        case 0x84000000: // PSCI_0_2_FN_VERSION
          gpr[0] = U64(2); // VERSION=0.2
          return;
        case 0x84000006: // PSCI_0_2_FN_MIGRATE_INFO_TYPE:
          /* Trusted OS is not present */
          gpr[0] = U64(2); // PSCI_0_2_TOS_MP;
          return;
        }
    }

  raise( Bad() );
}

struct DebugAccess { static bool const DEBUG = true; static bool const VERBOSE = true; };
struct QuietAccess { static bool const DEBUG = true; static bool const VERBOSE = false; };
struct PlainAccess { static bool const DEBUG = false; static bool const VERBOSE = false; };

void
AArch64::SetPSTATEFromPSR(U32 spsr)
{
  struct Bad {};
  nzcv = U8(spsr >> 28);
  spsr = (spsr << 4) >> 4;
  if (spsr.ubits) raise( Bad() );
  uint32_t psr = spsr.value;

  if (psr & 0x10) raise( Bad() ); /* No AArch32 */
  /* spsr checks causing an "Illegal return" */
  if ((psr >> 2 & 3) > pstate.GetEL()) raise( Bad() );
  if (psr & 2) raise( Bad() );

  // Return an SPSR value which represents thebits(32) spsr = Zeros();
  pstate.IL = psr >> 21;
  pstate.SS = psr >> 20;
  /* AArch64*/
  pstate.D = psr >> 9;
  pstate.A = psr >> 8;
  pstate.I = psr >> 7;
  pstate.F = psr >> 6;
  pstate.SetEL(*this, psr >> 2);
  pstate.SetSP(*this, psr >> 0);
}


void
AArch64::ExceptionReturn()
{
  U32 spsr = get_el(pstate.GetEL()).SPSR;
  U64 elr = get_el(pstate.GetEL()).ELR;
  SetPSTATEFromPSR(spsr);
  // if (pstate.GetEL() == 0)
  //   std::cerr << "ERET(" << pstate.GetEL() << ") " << std::hex << current_insn_addr << " => " << elr.value << std::endl;
  // ClearExclusiveLocal();
  // SendEventLocal();

  BranchTo(elr, B_ERET);
}

uint32_t
AArch64::PState::AsSPSR() const
{
  // Return an SPSR value which represents thebits(32) spsr = Zeros();
  uint32_t spsr = 0
    | IL << 21
    | SS << 20
    /* AArch64*/
    | D << 9
    | A << 8
    | I << 7
    | F << 6
    | EL << 2
    | SP << 0;

  return spsr;
}

bool
AArch64::concretize(bool possible)
{
  static std::set<uint64_t> seen;
  if (seen.insert(current_insn_addr).second)
    uninitialized_error("condition");
  return possible;
 
  struct ShouldNotHappen {};
  
  if (   current_insn_addr == 0xffffffc0105c70f8
    //or current_insn_addr == 0xffffffc0109fb270
         )
    { // <__pi_strlen>:
      // ...
      // ffffffc0105c70f8:       fa4008a0        ccmp    x5, #0x0, #0x0, eq  // eq = none
      // ffffffc0105c70fc:       54ffff00        b.eq    ffffffc0109fb250 <__pi_strlen+0x10>  // b.none
      gpr[5].ubits = 0;
      gpr[6].ubits = 0;
      return possible;
    }

  if (current_insn_addr == 0xffffffc0102868fc or
      current_insn_addr == 0xffffffc010286914)
    { // <hashlen_string>:
      // ...
      // ffffffc0102868fc:       54fffe80        b.eq    ffffffc010285e1c <hashlen_string+0x34>  // b.none
      // ...
      // ffffffc010286914:       b40000e2        cbz     x2, ffffffc010285e80 <hashlen_string+0x98>
      gpr[2].ubits = 0;
      gpr[3].ubits = 0;
      return possible;
    }

  if (current_insn_addr == 0xffffffc010238bdc)
    {
      // link_path_walk.part.0:
      // ...
      // ffffffc010238bd8:  f201c042  ands    x2, x2, #0x8080808080808080
      // ffffffc010238bdc:  54fffde0  b.eq
      gpr[2].ubits = 0;
      gpr[3].ubits = 0;
      gpr[5].ubits = 0;
      nzcv.ubits = 0;
      if ((gpr[2].value == 0) != possible) raise( ShouldNotHappen() );
      return possible;
    }

  if ((current_insn_addr | 0x380)  == 0xffffffc01008238c or
      (current_insn_addr | 0x180)  == 0xffffffc010082594)
    {
      // <vectors>:
      // ...
      // tbnz    w0, #14
      /* we're forced to definitely lose x0 undefined bits... */
      gpr[31].ubits = 0;
      gpr[0].ubits = 0;
      if (bool(gpr[0].value >> 14 & 1) != possible) raise( ShouldNotHappen() );
      return possible;
    }

  // if (current_insn_addr == 0xffffffc0109fb120)
  //   {
  //     // <__pi_strcmp>:
  //     // ...
  //     // orr     x6, x5, x4
  //     // cbz     x6, ffffffc0109f3f04 <__pi_strcmp+0x18>
  //     gpr[2].ubits = 0;
  //     gpr[3].ubits = 0;
  //     gpr[6].ubits = 0;
  //     return possible;
  //   }

  // if (current_insn_addr == 0xffffffc01062b630)
  //   {
  //     // <amba_device_try_add>:
  //     // ...
  //     // cbnz    x3, ffffffc01062b630 <amba_device_try_add+0xf8>
  //     gpr[3].ubits = 0;
  //     if (possible) raise( ShouldNotHappen() );
  //     return possible;
  //   }

  if (current_insn_addr == 0xffffffc010008cbc)
    {
      return (gpr[0].ubits == gpr[8].ubits) and (gpr[0].value == gpr[8].value);
    }

  if (current_insn_addr == 0xffffffc0105c70c4)
    {
      return possible;
    }

  // if (current_insn_addr == 0xffffffc0105c70c4)
  //   {
  //     gpr[0].ubits = 0;
  //     return possible;
  //   }

  // static struct { uint64_t address; bool result; }
  // exceptions [] =
  //   {
  //    {0xff3, false}, /* has_mismatched_cache_type */
  //   };

  // static uintptr_t idx =0;
  // uintptr_t const end = sizeof (exceptions) / sizeof (exceptions[0]);

  // if (idx < end and exceptions[idx].address == current_insn_addr)
  //   return exceptions[idx++].result;

  uninitialized_error("condition");

  struct Bad {}; raise( Bad() );
  return possible;
}

void
AArch64::uninitialized_error( char const* rsrc )
{
  std::cerr << "Error: " << rsrc << " depends on unitialized value.\n" << last_insn(0) << '\n';
}

void
AArch64::InstructionInfo::dump(std::ostream& sink) const
{
  sink << std::dec << counter << "@" << std::hex << addr << ',' << op->GetAddr() << ": "
       << std::hex << std::setfill('0') << std::setw(8) << op->GetEncoding() << "; ";
  DisasmState ds;
  op->disasm( ds, sink );
}

AArch64::U64
AArch64::RNDR()
{
  uint64_t rval =       (random = random * 22695477 + 1);
  rval = (rval << 32) | (random = random * 22695477 + 1);
  nzcv = U8(0);
  
  return U64(rval);
}

void
AArch64::run()
{
  for (;;)
    {
      random = random * 22695477 + 1;
      insn_timer += 1;// + ((random >> 16 & 3) == 3);
      if (insn_timer > next_quantum_timer)
        {
          next_quantum_timer += quantum_cycles;
          wait(quantum_delay);
        }

      /* Instruction boundary next_insn_addr becomes current_insn_addr */
      uint64_t insn_addr = this->current_insn_addr = this->next_insn_addr;

      //if (insn_addr == halt_on_addr) { Stop(0); return; }

      if (unlikely(requires_fetch_instruction_reporting and memory_access_reporting_import))
        memory_access_reporting_import->ReportFetchInstruction(insn_addr);

      if (debug_yielding_import)
        {
          debug_yielding_import->DebugYield();
        }

      try
        {
          Operation* op = fetch_and_decode(insn_addr);

          this->next_insn_addr += 4;

          if (current_insn_addr == bdaddr)
            {
              breakdance();
              // disasm = true;
            }

          if (terminate) { force_throw(); }

          if (disasm)
            {
              uint64_t chk = 0;
              for (unsigned idx = 0; idx < 32; ++idx)
                chk ^= gpr[idx].value ^ gpr[idx].ubits;
              //static std::ofstream dbgtrace("dbgtrace");
              std::ostream& sink( std::cerr );
              sink << "@" << std::hex << insn_addr << ": " << std::setfill('0') << std::setw(8) << op->GetEncoding() << "; ";
              DisasmState ds;
              op->disasm( ds, sink );
              sink << '\t' << chk << std::endl;
            }

          /* Execute instruction */
          asm volatile( "arm64_operation_execute:" );
          op->execute( *this );

          if (unlikely(requires_commit_instruction_reporting and memory_access_reporting_import))
            memory_access_reporting_import->ReportCommitInstruction(this->current_insn_addr, 4);

          insn_counter++; /* Instruction regularly finished */
        }

      catch (Abort const& abort)
        {
          abort.proceed(*this);
          /* Instruction aborted, proceed to next */
          //   if (unlikely(trap_reporting_import))
          //     trap_reporting_import->ReportTrap( *this, "Data Abort Exception" );
        }
      
    }
}

void
AArch64::wfi()
{
  struct Bad {}; throw Bad {};
}

void force_throw()
{
  struct Force {};
  throw Force();
}

void
AArch64::MemDump64(uint64_t addr)
{
  addr &= -8;

  std::cerr << std::hex << "virt: " << addr << ", ";

  PMemDump64(addr);
}

void
AArch64::PMemDump64(uint64_t paddr)
{
  paddr &= -8;

  unsigned const size = 8;
  uint8_t dbuf[size], ubuf[size];

  Page const& page = get_page(paddr);
  unsigned vsize = page.read(paddr,&dbuf[0],&ubuf[0],size);
  uintptr_t host_addr = (uintptr_t)page.data_abs(paddr);

  std::cerr << "phys: " << std::hex << paddr << ", host: " << host_addr << std::dec;
  std::cerr << "\n\t0x";
  for (unsigned byte = vsize; byte-- > 0;)
    {
      Print(std::cerr, 2, 4, dbuf[byte], ubuf[byte] );
      // uint8_t dbyte = dbuf[byte], ubyte = ubuf[byte];
      // for (unsigned nibble = 2; nibble-- > 0;)
      //   std::cerr << ((ubyte >> 4*nibble & 15) ? 'x' : "0123456789abcdef"[dbyte >> 4*nibble & 15]);
    }

  std::cerr << "\n\t0b";
  for (unsigned byte = vsize; byte-- > 0;)
    {
      Print(std::cerr, 8, 1, dbuf[byte], ubuf[byte]);
      // uint8_t dbyte = dbuf[byte], ubyte = ubuf[byte];
      // for (unsigned bit = 8; bit-- > 0;)
      //   std::cerr << "01xx"[2*(ubyte >> bit & 1) | (dbyte >> bit & 1)];
    }
  std::cerr << "\n\t" << '"';
  for (unsigned byte = 0; byte < vsize; ++byte)
    {
      uint8_t dbyte = dbuf[byte], ubyte = ubuf[byte];
      std::cerr << (ubyte or dbyte < 32 or dbyte > 126 ? '.' : char(dbyte));
    }
  std::cerr << '"';
  std::cerr << std::endl;
}

void
AArch64::SetExclusiveMonitors( U64 addr, unsigned size )
{
  struct Bad {};
  if (addr.ubits) raise( Bad() );
  if ((size | addr.value) & (size - 1)) raise( Bad() );
  excmon.set(addr.value, size);
}

bool
AArch64::ExclusiveMonitorsPass( U64 addr, unsigned size )
{
  struct Bad {};
  if (addr.ubits) raise( Bad() );
  if ((size | addr.value) & (size - 1)) raise( Bad() ); /* TODO: should be an Alignment-related DataAbort */
  bool passed = excmon.pass( addr.value, size );
  excmon.clear();
  return passed;
}

void
AArch64::ClearExclusiveLocal()
{
  excmon.clear();
}

void
AArch64::mem_unmap(uint64_t base, uint64_t last)
{
  struct { void if_not_empty(Pages& m, Page&& p) { if (p.data_beg()) m.insert(std::move(p)); } } insert;

  auto pi = pages.lower_bound(last);
  if (pi == pages.end())
    return;

  Page above;
  if (pi->last > last)
    {
      new (&above) Page(last+1,pi->last);
      std::copy(pi->data_abs(last+1), pi->data_end(), above.data_beg());
      std::copy(pi->udat_abs(last+1), pi->udat_end(), above.udat_beg());
    }

  while (pi->base >= base)
    {
      pi = pages.erase(pi);
      if (pi == pages.end())
        { insert.if_not_empty(pages, std::move(above)); return; }
    }

  if (pi->last < base)
    { insert.if_not_empty(pages, std::move(above)); return; }

  Page below(pi->base, base - 1);
  std::copy(pi->data_beg(), pi->data_abs(base), below.data_beg());
  std::copy(pi->udat_beg(), pi->udat_abs(base), below.udat_beg());

  pi = pages.erase(pi);

  insert.if_not_empty(pages, std::move(above));
  pages.insert(pi, std::move(below));
}


AArch64::U8
AArch64::GetTVU8(unsigned reg0, unsigned elements, unsigned regs, U8 const& index, U8 const& oob_value)
{
  struct Bad {};
  if (index.ubits) raise( Bad() );
  unsigned e = index.value % elements, r = index.value / elements;
  return r < regs ? GetVU8((reg0+r)%32, e) : oob_value;
}

/** Disasm an instruction address.
 * Returns a string with the disassembling of the instruction found
 *   at address addr.
 *
 * @param addr the address of the instruction to disassemble
 * @param next_addr the address following the requested instruction
 *
 * @return the disassembling of the requested instruction address
 */
std::string
AArch64::Disasm(uint64_t addr, uint64_t& next_addr)
{
  std::stringstream buffer;

  uint32_t insn;
  if (not ReadMemory(addr, &insn, 4))
    {
      buffer << "Could not read from memory";
      return buffer.str();
    }

  insn = unisim::util::endian::LittleEndian2Host(insn);

  auto op = decoder.Decode(addr, insn);

  buffer << std::hex << std::setfill('0') << std::setw(8) << op->GetEncoding() << std::dec << " ";
  DisasmState ds;
  op->disasm(ds, buffer);

  next_addr = addr + 4;

  return buffer.str();
}

/** Set/unset the reporting of memory accesses.
 *
 * @param report if true/false sets/unsets the reporting of memory
 *        acesseses
 */
void
AArch64::RequiresMemoryAccessReporting( unisim::service::interfaces::MemoryAccessReportingType type, bool report )
{
  switch (type) {
  case unisim::service::interfaces::REPORT_MEM_ACCESS:  requires_memory_access_reporting = report; break;
  case unisim::service::interfaces::REPORT_FETCH_INSN:  requires_fetch_instruction_reporting = report; break;
  case unisim::service::interfaces::REPORT_COMMIT_INSN: requires_commit_instruction_reporting = report; break;
  default: throw 0;
  }
}

// unisim::service::interfaces::HttpServer
bool
AArch64::ServeHttpRequest(unisim::util::hypapp::HttpRequest const& req, unisim::util::hypapp::ClientConnection const& conn)
{
  unisim::util::hypapp::HttpResponse response;
	
  //  std::cerr << req.GetPath() << std::endl;
  
  switch(req.GetRequestType())
    {
    case unisim::util::hypapp::Request::OPTIONS:
      response.Allow("OPTIONS, GET, HEAD");
      break;
				
    case unisim::util::hypapp::Request::GET:
    case unisim::util::hypapp::Request::HEAD:
      {
        response << "<!DOCTYPE html>" << std::endl;
        response << "<html>" << std::endl;
        response << "\t<head>" << std::endl;
        response << "\t\t<title>" << unisim::util::hypapp::HTML_Encoder::Encode(this->GetName()) << "</title>" << std::endl;
        response << "\t\t<meta name=\"description\" content=\"user interface for TOTO\">" << std::endl;
        response << "\t\t<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">" << std::endl;
        //response << "\t\t<link rel=\"stylesheet\" href=\"/unisim/component/cxx/processor/powerpc/e600/mmu_style.css\" type=\"text/css\" />" << std::endl;
        response << "\t\t<script>document.domain='" << req.GetDomain() << "';</script>" << std::endl;
        response << "\t\t<script src=\"/unisim/service/http_server/uri.js\"></script>" << std::endl;
        response << "\t\t<script src=\"/unisim/service/http_server/embedded_script.js\"></script>" << std::endl;
        //response << "\t\t<script src=\"/unisim/component/cxx/processor/powerpc/e600/mmu_script.js\"></script>" << std::endl;
        response << "\t</head>" << std::endl;
        response << "\t<body onload=\"gui.auto_reload(1000, 'self-refresh-when-active')\">" << std::endl;
        //Print(response, PFMT_HTML);
        response << "\t\t<h2>Unitialized value map</h2>\n";
        response << "\t\t<p>Registers: ";
        {
          char const* sep = "";
          for (unsigned idx = 0; idx < 32; ++idx)
            {
              if (not gpr[idx].ubits) continue;
              response << sep << unisim::component::cxx::processor::arm::isa::arm64::DisasmGSXR(idx);
            }
        }
        response << "</p>\n";
        response << "<table>";
        static unsigned pushup = 1;
        for (auto const& page : pages)
          {
            response << "<tr>";
            response << "<td style=\"text-align: right\">" << std::hex << std::setw(16) << std::setfill('0') << page.base << "</td>";
            response << "<td style=\"text-align: right\">" << std::hex << std::setw(16) << std::setfill('0') << page.last << "</td>";
            response << "<td style=\"width:256px;background-color:black;\">";
            uint64_t ubs = 0;
            for (uint8_t const* uptr = page.udat_beg(), *end = page.udat_end(); uptr < end; uptr++)
              {
                ubs += bool(*uptr);
              }
            uint64_t tbs = page.size();

            ubs = (256*ubs/tbs);
            ubs = std::min(ubs, uint64_t(256));
            response << "<div style=\"width:" << std::dec << ubs << "px;height:16px;background-color:red;\"></div>";
            response << "</td>";
            response << "</tr>";
          }

        pushup = pushup * 7;
        response << "</table>";
        response << "\t</body>" << std::endl;
        response << "</html>" << std::endl;
				
        break;
      }
			
    default:
      response.SetStatus(unisim::util::hypapp::HttpResponse::METHOD_NOT_ALLOWED);
      response.Allow("OPTIONS, GET, HEAD");
				
      response << "<!DOCTYPE html>" << std::endl;
      response << "<html>" << std::endl;
      response << "\t<head>" << std::endl;
      response << "\t\t<title>Error 405 (Method Not Allowed)</title>" << std::endl;
      response << "\t\t<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">" << std::endl;
      response << "\t\t<meta name=\"description\" content=\"Error 405 (Method Not Allowed)\">" << std::endl;
      response << "\t\t<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">" << std::endl;
      response << "\t\t<script>document.domain='" << req.GetDomain() << "';</script>" << std::endl;
      response << "\t\t<style>" << std::endl;
      response << "\t\t\tbody { font-family:Arial,Helvetica,sans-serif; font-style:normal; font-size:14px; text-align:left; font-weight:400; color:black; background-color:white; }" << std::endl;
      response << "\t\t</style>" << std::endl;
      response << "\t</head>" << std::endl;
      response << "\t<body>" << std::endl;
      response << "\t\t<p>HTTP Method not allowed</p>" << std::endl;
      response << "\t</body>" << std::endl;
      response << "</html>" << std::endl;
      break;
    }
  
  // else
  //   {
  //     response.SetStatus(unisim::util::hypapp::HttpResponse::NOT_FOUND);
		
  //     response << "<!DOCTYPE html>" << std::endl;
  //     response << "<html>" << std::endl;
  //     response << "\t<head>" << std::endl;
  //     response << "\t\t<title>Error 404 (Not Found)</title>" << std::endl;
  //     response << "\t\t<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">" << std::endl;
  //     response << "\t\t<meta name=\"description\" content=\"Error 404 (Not Found)\">" << std::endl;
  //     response << "\t\t<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">" << std::endl;
  //     response << "\t\t<script>document.domain='" << req.GetDomain() << "';</script>" << std::endl;
  //     response << "\t\t<style>" << std::endl;
  //     response << "\t\t\tbody { font-family:Arial,Helvetica,sans-serif; font-style:normal; font-size:14px; text-align:left; font-weight:400; color:black; background-color:white; }" << std::endl;
  //     response << "\t\t</style>" << std::endl;
  //     response << "\t</head>" << std::endl;
  //     response << "\t<body>" << std::endl;
  //     response << "\t\t<p>Unavailable</p>" << std::endl;
  //     response << "\t</body>" << std::endl;
  //     response << "</html>" << std::endl;
  //   }
	
  return conn.Send(response.ToString((req.GetRequestType() == unisim::util::hypapp::Request::HEAD) || (req.GetRequestType() == unisim::util::hypapp::Request::OPTIONS)));
}

void
AArch64::ScanWebInterfaceModdings(unisim::service::interfaces::WebInterfaceModdingScanner& scanner)
{
  //http_server->AddJSAction
  scanner.Append(
     unisim::service::interfaces::BrowserOpenTabAction(
       /* name        */ std::string("secu-") + this->GetName(),
       /* object name */ this->GetName(),
       /* label       */ "Show Security Map",
       /* tile        */ unisim::service::interfaces::OpenTabAction::TOP_MIDDLE_TILE,
       /* uri         */ this->URI() + "/secu"
    ));
}

