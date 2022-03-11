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
#include <unisim/component/cxx/processor/opcache/opcache.tcc>
#include <unisim/util/debug/simple_register.hh>
#include <unisim/util/os/linux_os/linux.hh>
#include <unisim/util/os/linux_os/aarch64.hh>
#include <unisim/util/likely/likely.hh>
#include <iostream>
#include <iomanip>
#include <termios.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

AArch64::AArch64()
  : regmap()
  , devices()
  , gic()
  , uart("uart", 0)
  , vt()
  , pages()
  , excmon()
  , mmu()
  , ipb()
  , decoder()
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
  , last_insns()
  , TPIDR()
  , CPACR()
  , tvreg(-1)
  , bdaddr(0)
  , random(0)
  , terminate(false)
  , disasm(false)
  , suspend(false)
{
  for (int idx = 0; idx < 32; ++idx)
    {
      std::ostringstream regname;
      regname << unisim::component::cxx::processor::arm::isa::arm64::DisasmGSXR(idx);
      regmap[regname.str()] = new unisim::util::debug::SimpleRegister<uint64_t>(regname.str(), &gpr[idx].value);
    }

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
}

AArch64::~AArch64()
{
  for (auto reg : regmap)
    delete reg.second;

  for (unsigned reg = 0; reg < VECTORCOUNT; ++reg)
    vector_views[reg].Clear(&vectors[reg]);
}

bool
AArch64::ReadMemory(uint64_t addr, void* buffer, unsigned size)
{
  struct Bad {};

  uint64_t count = size;
  uint8_t* ptr = static_cast<uint8_t*>(buffer);

  while (count > 0)
    {
      MMU::TLB::Entry entry(addr);
      try { translate_address(entry, 1, mem_acc_type::debug); }
      catch (DataAbort const& x) { return false; }
      Page const& page = get_page(entry.pa);
      uint64_t done = std::min(std::min(count, entry.size_after()), page.size_from(entry.pa));
      uint8_t *udat = page.udat_abs(entry.pa), *data = page.data_abs(entry.pa);
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
      MMU::TLB::Entry entry(addr);
      try { translate_address(entry, 1, mem_acc_type::debug); }
      catch (DataAbort const& x) { return false; }
      Page const& page = get_page(entry.pa);
      uint64_t done = std::min(std::min(count, entry.size_after()), page.size_from(entry.pa));
      uint8_t *udat = page.udat_abs(entry.pa), *data = page.data_abs(entry.pa);
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
  uint64_t const page_size = 0x10000;
  uint64_t base = addr & -page_size, last = base + page_size - 1;
  if (pi != pages.end() and pi->last >= base)
    base = pi->last + 1;
  if (pages.size() and pi != pages.begin() and (--pi)->base <= last)
    { last = pi->base - 1; }

  Page res(base, last);
  // Initialize taint
  // std::fill(res.udat_beg(), res.udat_end(), -1);
  std::fill(res.udat_beg(), res.udat_end(), 0);
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
  //  static std::ofstream strace("strace.log");
  // static struct Arm64LinuxOS : public unisim::util::os::linux_os::Linux<uint64_t, uint64_t>
  // {
  //   typedef unisim::util::os::linux_os::Linux<uint64_t, uint64_t> ThisLinux;
  //   typedef unisim::util::os::linux_os::AARCH64TS<ThisLinux> Arm64Target;

  //   Arm64LinuxOS( AArch64* cpu )
  //     : ThisLinux( AArch64::ptlog(), AArch64::ptlog(), AArch64::ptlog(), cpu, cpu, cpu )
  //   {
  //     SetTargetSystem(new Arm64Target(*this));
  //   }
  //   ~Arm64LinuxOS() { delete GetTargetSystem(); }
  // } arm64_linux_os( this );

  // AArch64::ptlog() << last_insn(0) << "\n\t";
  // //  strace << "SVC@" << std::hex << current_insn_addr;
  // arm64_linux_os.LogSystemCall( imm );

  //  if UsingAArch32() then AArch32.ITAdvance();
  // SSAdvance();
  pstate.SS = 0;

  // route_to_el2 = AArch64.GeneralExceptionsToEL2();

  // ReportException
  unsigned const target_el = 1;
  // ESR[target_el] = ec<5:0>:il:syndrome;
  get_el(target_el).ESR = U32(0)
    | U32(0x15) << 26 // exception class AArch64.SVC
    | U32(1) << 25 // IL Instruction Length == 32 bits
    | PartiallyDefined<uint32_t>(0,0b111111111) << 16 // Res0
    | U32(imm);

  TakeException(1, 0x0, next_insn_addr);
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

AArch64::MMU::TLB::TLB()
  : keys()
{
  for (unsigned idx = 0; idx < kcount; ++idx)
    indices[idx] = idx;
}

void
AArch64::MMU::TLB::AddTranslation( AArch64::MMU::TLB::Entry const& tlbe, uint64_t vaddr, unsigned asid )
{
  // LRU Replacement
  unsigned lru_idx = indices[kcount-1];
  for (unsigned idx = kcount; --idx > 0;)
    {
      keys[idx] = keys[idx-1];
      indices[idx] = indices[idx-1];
    }

  unsigned significant = tlbe.blocksize - 1, global = not tlbe.nG;
  keys[0] = (((vaddr >> 12) << 27) >> 16) | (uint64_t(asid) << 48) | (global << 6) | significant;
  indices[0] = lru_idx;
  entries[lru_idx] = tlbe;
}

void
AArch64::MMU::TLB::Invalidate(AArch64& cpu, bool nis, bool ll, unsigned type, AArch64::U64 const& arg)
{
  if (not nis)
    {
      cpu.TODO(); // Should send to all Inner Shareable PE...
      return Invalidate(cpu, true, ll, type, arg);
    }

  struct Bad {};

  uint64_t ubit_issue = 0;
  switch (type)
    {
    case 1: ubit_issue = 0xffff0fffffffffffull; break;
    case 2: ubit_issue = 0xffff000000000000ull; break;
    case 3: ubit_issue = 0x00000fffffffffffull; break;
    }
  if (arg.ubits & ubit_issue) raise( Bad() );

  //  AArch64::ptlog() << cpu.last_insn(0) << " (X=" << std::hex << arg.value << std::dec << ")\n";

  uint64_t vakey = (((arg.value << 12) << 15 ) >> 16) | ((arg.value >> 48) << 48) | 1;

  for (unsigned idx = 0; idx < kcount; ++idx)
    {
      uint64_t key = keys[idx];
      unsigned rsh = key & 63;

      if (ll and rsh > 15) /* rsh = blocksize - 1; last level in {12,14,16} (see TTBR.TG) */
        continue;

      if (type == 2 and ((vakey ^ key) >> 48)) // by asid
        continue;

      if ((type & 1) and (((vakey ^ key) >> rsh) << (rsh + (((key >> 2) | (2 << type)) & 16)))) // by va
        continue;

      // key matches -> invalidate
      keys[idx] = 0;
    }
}

bool
AArch64::MMU::TLB::GetTranslation( AArch64::MMU::TLB::Entry& result, uint64_t vaddr, unsigned asid, bool update )
{
  unsigned rsh = 0, hit;
  uint64_t key = 0;
  /* 64-bit-keys:
   * asid[16] : varange[1] : input bits[36] : ?[4] : global[1] : significant[6]
   *   asid   :   va<48>   :   va<47:12>    :      :  !nG      :  blocksize-1
   */

  uint64_t vakey = ((vaddr << 15) >> 16) | (uint64_t(asid) << 48) | 1;

  for (hit = 0; hit < kcount; ++hit)
    {
      key = keys[hit];
      rsh = key & 63;
      if (not (((vakey ^ key) >> rsh) << (rsh + ((key >> 2) & 16))))
        break;
    }
  if (hit >= kcount)
    return false; // TLB miss

  // TLB hit
  unsigned idx = indices[hit];
  Entry& entry = entries[idx];

  if (update)
    {
      // MRU sort
      for (unsigned idx = hit; idx > 0; idx -= 1)
        {
          keys[idx] = keys[idx-1];
          indices[idx] = indices[idx-1];
        }
      keys[0] = key;
      indices[0] = idx;
    }

  // Address translation and attributes
  result = entry;

  uint64_t himask = uint64_t(-2) << rsh; /* blocksize = rsh + 1; */
  result.pa = (result.pa & himask) | (vaddr & ~himask);
  return true;
}

struct DebugAccess { static bool const DEBUG = true; static bool const VERBOSE = true; };
struct QuietAccess { static bool const DEBUG = true; static bool const VERBOSE = false; };
struct PlainAccess { static bool const DEBUG = false; static bool const VERBOSE = false; };

void
AArch64::translate_address(AArch64::MMU::TLB::Entry& entry, unsigned el, mem_acc_type::Code mat)
{
  if (not unisim::component::cxx::processor::arm::sctlr::M.Get(el1.SCTLR))
    return;

  uint64_t vaddr = entry.pa;

  // Stage 1 MMU enabled
  bool update = mat != mem_acc_type::debug;
  unsigned asid = mmu.GetASID();
  if (unlikely(not mmu.tlb.GetTranslation( entry, vaddr, asid, update )))
    {
      translation_table_walk( entry, vaddr, mat );
      if (update)
        mmu.tlb.AddTranslation( entry, vaddr, asid );
    }
  // else {
  //   // Check if hit is coherent
  //   TLB::Entry tlbe_chk;
  //   translation_table_walk<QuietAccess>( tlbe_chk, mva, mat, size );
  //   if (tlbe_chk.pa != tlbe.pa)
  //     trap_reporting_import->ReportTrap( *this, "Incoherent TLB access" );
  // }

  CheckPermission(entry, vaddr, el, mat);
}

void
AArch64::DataAbort::proceed( AArch64& cpu ) const
{
  // ReportException
  unsigned const target_el = 1;
  // ESR[target_el] = ec<5:0>:il:syndrome;
  U32 esr = U32(0)
    | U32((mat == mem_acc_type::exec ? 0x20 : 0x24) + (cpu.pstate.GetEL() == target_el)) << 26 // exception class
    | U32(1) << 25 // IL Instruction Length
    | PartiallyDefined<uint32_t>(0,0b11111111111) << 14 // extended syndrome information for a second stage fault.
    | PartiallyDefined<uint32_t>(0,0b1111) << 10
    | U32(0) << 9 // IsExternalAbort(fault)
    | U32(mat == mem_acc_type::cache_maintenance) << 8 // IN {AccType_DC, AccType_IC} Cache maintenance
    | U32(s2fs1walk) << 7
    | U32(mat == mem_acc_type::write or mat == mem_acc_type::cache_maintenance) << 6
    | U32(unisim::component::cxx::processor::arm::EncodeLDFSC(type, level)) << 0;

  // Print(std::cerr << "DataAbort: ", esr);
  // std::cerr << std::endl;
  cpu.get_el(target_el).ESR = esr;
  // FAR[target_el] = exception.vaddress;
  cpu.get_el(target_el).FAR = va;
  //     if target_el == EL2 && exception.ipavalid then
  //     HPFAR_EL2<39:4> = exception.ipaddress<47:12>;

  cpu.TakeException(target_el, 0x0, cpu.current_insn_addr);
}

void
AArch64::translation_table_walk( AArch64::MMU::TLB::Entry& entry, uint64_t vaddr, mem_acc_type::Code mat )
{
  bool tbi, epd;
  unsigned grainsize, tsz;
  uint64_t ttbr;

  struct Bad {};

  struct Descriptor
  {

    Descriptor(AArch64& _cpu)
      : cpu(_cpu)
      , endianness(unisim::component::cxx::processor::arm::sctlr::EE.Get(cpu.el1.SCTLR) ? 0b111 : 0b000)
    {}

    AArch64& cpu;
    unsigned endianness;
    enum {size = 8};

    uint64_t read(uint64_t paddr)
    {
      uint8_t dbuf[size], ubuf[size];
      if (cpu.get_page(paddr).read(paddr,&dbuf[0],&ubuf[0],size) != size) raise( Bad() );
      uint64_t value = 0;
      for (unsigned idx = size; idx-- > 0;)
      { if (ubuf[idx]) raise( Bad() ); value = (value << 8) | dbuf[idx ^ endianness]; }
      return value;
    }
  } descriptor(*this);
  // top = AddrTop(inputaddr); bit 55 or 63 (depending on TCR.TBI)
  // should be conserved down to vaddr::size (tablesize).  Because ,
  // tablesize max at 48, at least bits 55 downto 48 should always be
  // identical.  ARM says that bit 55 is used to determine the value
  // of these hyptothesized identical bits, and thus determine the VA range.


  if ((vaddr >> 55) & 1)
    {
      /* high VA range */
      ttbr =  mmu.TTBR1_EL1;
      tbi = unisim::component::cxx::processor::arm::vmsav8::tcr::TBI1.Get(mmu.TCR_EL1);
      switch (unisim::component::cxx::processor::arm::vmsav8::tcr::TG1.Get(mmu.TCR_EL1))
        { // TG1 and TG0 encodings differ
        default: grainsize = 12; break;
        case 1:  grainsize = 14; break;
        case 3:  grainsize = 16; break;
        }
      epd = unisim::component::cxx::processor::arm::vmsav8::tcr::EPD1.Get(mmu.TCR_EL1);
      tsz = unisim::component::cxx::processor::arm::vmsav8::tcr::T1SZ.Get(mmu.TCR_EL1);
      // descaddr.memattrs = WalkAttrDecode(TCR_EL1.SH1, TCR_EL1.ORGN1, TCR_EL1.IRGN1);
    }
  else
    {
      /* low VA range */
      ttbr =  mmu.TTBR0_EL1;
      tbi = unisim::component::cxx::processor::arm::vmsav8::tcr::TBI0.Get(mmu.TCR_EL1);
      switch (unisim::component::cxx::processor::arm::vmsav8::tcr::TG0.Get(mmu.TCR_EL1))
        { // TG1 and TG0 encodings differ
        default: grainsize = 12; break;
        case 1:  grainsize = 16; break;
        case 2:  grainsize = 14; break;
        }
      epd = unisim::component::cxx::processor::arm::vmsav8::tcr::EPD0.Get(mmu.TCR_EL1);
      tsz = unisim::component::cxx::processor::arm::vmsav8::tcr::T0SZ.Get(mmu.TCR_EL1);
      // descaddr.memattrs = WalkAttrDecode(TCR_EL1.SH0, TCR_EL1.ORGN0, TCR_EL1.IRGN0);
    }

  if (epd)
    raise( Bad() );

  if (tbi)
    {
      // TODO: extract tag here ?
      vaddr = (int64_t(vaddr) << 8) >> 8;
    }

  // firstblocklevel = first level where a block entry is allowed
  // firstblocklevel = {4K =>1, 64K => 2, 16K => 2} <=> blocks < 2^32

  unsigned stride = grainsize - 3, tablesize = 64 - tsz;
  if (tablesize > 48) tablesize = 48;
  if (tablesize < 25) tablesize = 25;

  unsigned walkdepth = (tablesize - grainsize - 1) / stride;
  // level = 3 - walkdepth
  // psize = Physical Address size as encoded in TCR_EL1.IPS or TCR_ELx/VTCR_EL2.PS
  // assert IsSameBits(vaddr<top:tablesize>)
  // reversedescriptors = SCTLR_EL1.EE == ‘1’
  // singlepriv = FALSE;

  uint64_t tbladdr, desc;
  {
    // baseaddress = base<47:baselowerbound>:Zeros(baselowerbound);
    unsigned baselo = tablesize - stride*(walkdepth+1);
    tbladdr = ((ttbr >> baselo) << (baselo+16)) >> 16;
  }
  // assert uint64_t((int64_t(vaddr) << tsz) >> tsz) == vaddr
  unsigned direct_bits = walkdepth*stride + grainsize, level;

  /* attr_table is NS[1] : ap_table[2] : UX[1] (=!UXN) : PX[1] (=!PXN) */
  unsigned attr_table = 0b11111; // should be 0b11010 for non EL0&1 regimes and 0b10000 should depend on `lookupsecure`

  {
    uint64_t walkidx = (vaddr << tsz) >> tsz, walkmask = (1 << stride)-1;
    for (level = 3 - walkdepth; true; ++level, direct_bits -= stride)
      {
        unsigned index = (walkidx >> direct_bits) & walkmask;

        desc = descriptor.read(tbladdr + 8*index);

        if (not (desc & 1))
          throw DataAbort(unisim::component::cxx::processor::arm::DAbort_Translation, vaddr, 0, mat, level, /*ipavalid*/false, /*secondstage*/false, /*s2fs1walk*/false);

        if (not (desc & 2))
          {
            /*illegal at page grain*/
            if (level == 3) raise( Bad() );
            break; /* a block */
          }
        else if (level == 3)
          break; /* a page*/

        /* a table */
        tbladdr = ((desc >> grainsize) << (grainsize+16)) >> 16;
        attr_table &= ((desc >> 59) ^ 0b00011);
      }
  }
  if (direct_bits > 32) raise( Bad() );

  unsigned vaclr = 64 - direct_bits;
  entry.pa = (((desc >> direct_bits) << (direct_bits + 16)) >> 16) | ((vaddr << vaclr) >> vaclr);
  entry.blocksize = direct_bits;
  entry.ap = (((desc >> 5 ^ 2) | (attr_table >> 1) | 1) ^ 2) & 7;
  unsigned xperms = ((desc >> 53) | (attr_table ^ 3)) & 3;
  entry.xn = (xperms >> 1) & 1;
  entry.pxn = (xperms >> 0) & 1;
  entry.level = level;
  entry.nG = (desc >> 11) & 1;
  entry.NS = 1;
  entry.sh = (desc >> 8) & 3;
  entry.attridx = (desc >> 2) & 7;
}

void
AArch64::TakeException(unsigned target_el, unsigned vect_offset, uint64_t preferred_exception_return)
{
  // std::cerr << "Taking Exception to EL" << target_el << " from EL" << pstate.EL
  //           << " with vect_offset=0x" << std::hex << vect_offset
  //           << " at " << std::dec << insn_counter << '/' << insn_timer << "\n";

  if (target_el > pstate.GetEL())
    vect_offset += 0x400; /* + 0x600 if lower uses AArch32 */
  else if (pstate.GetSP())
    vect_offset += 0x200;

  U32 spsr = GetPSRFromPSTATE();

  pstate.SetEL(*this, target_el);
  pstate.SetSP(*this, 1);

  get_el(target_el).SPSR = spsr;
  get_el(target_el).ELR = U64(preferred_exception_return);

  pstate.SS = 0;
  pstate.IL = 0;

  pstate.D = 1;
  pstate.A = 1;
  pstate.I = 1;
  pstate.F = 1;

  // BranchTo(VBAR[]<63:11>:vect_offset<10:0>, BranchType_EXCEPTION);
  U64 target = (get_el(target_el).VBAR & U64(-0x800)) | U64(vect_offset);
  BranchTo( target, B_EXC );
  //  std::cerr << "EXC: " << std::hex << target.value << std::endl;
}

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

void
AArch64::TakePhysicalIRQException()
{
  TakeException(1, 0x80, current_insn_addr);
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
  return possible;
}

// bool
// AArch64::concretize(bool possible)
// {
//   // static std::set<uint64_t> seen;
//   // if (seen.insert(current_insn_addr).second)
//   //   uninitialized_error("condition");
//   // return possible;
 
//   struct ShouldNotHappen {};
  
//   if (   current_insn_addr == 0xffffffc0105c70f8
//     //or current_insn_addr == 0xffffffc0109fb270
//          )
//     { // <__pi_strlen>:
//       // ...
//       // ffffffc0105c70f8:       fa4008a0        ccmp    x5, #0x0, #0x0, eq  // eq = none
//       // ffffffc0105c70fc:       54ffff00        b.eq    ffffffc0109fb250 <__pi_strlen+0x10>  // b.none
//       gpr[5].ubits = 0;
//       gpr[6].ubits = 0;
//       return possible;
//     }

//   if (current_insn_addr == 0xffffffc0102868fc or
//       current_insn_addr == 0xffffffc010286914)
//     { // <hashlen_string>:
//       // ...
//       // ffffffc0102868fc:       54fffe80        b.eq    ffffffc010285e1c <hashlen_string+0x34>  // b.none
//       // ...
//       // ffffffc010286914:       b40000e2        cbz     x2, ffffffc010285e80 <hashlen_string+0x98>
//       gpr[2].ubits = 0;
//       gpr[3].ubits = 0;
//       return possible;
//     }

//   if (current_insn_addr == 0xffffffc010238bdc)
//     {
//       // link_path_walk.part.0:
//       // ...
//       // ffffffc010238bd8:	f201c042	ands    x2, x2, #0x8080808080808080
//       // ffffffc010238bdc:	54fffde0	b.eq
//       gpr[2].ubits = 0;
//       gpr[3].ubits = 0;
//       gpr[5].ubits = 0;
//       nzcv.ubits = 0;
//       if ((gpr[2].value == 0) != possible) raise( ShouldNotHappen() );
//       return possible;
//     }

//   if ((current_insn_addr | 0x380)  == 0xffffffc01008238c or
//       (current_insn_addr | 0x180)  == 0xffffffc010082594)
//     {
//       // <vectors>:
//       // ...
//       // tbnz    w0, #14
//       /* we're forced to definitely lose x0 undefined bits... */
//       gpr[31].ubits = 0;
//       gpr[0].ubits = 0;
//       if (bool(gpr[0].value >> 14 & 1) != possible) raise( ShouldNotHappen() );
//       return possible;
//     }

//   // if (current_insn_addr == 0xffffffc0109fb120)
//   //   {
//   //     // <__pi_strcmp>:
//   //     // ...
//   //     // orr     x6, x5, x4
//   //     // cbz     x6, ffffffc0109f3f04 <__pi_strcmp+0x18>
//   //     gpr[2].ubits = 0;
//   //     gpr[3].ubits = 0;
//   //     gpr[6].ubits = 0;
//   //     return possible;
//   //   }

//   // if (current_insn_addr == 0xffffffc01062b630)
//   //   {
//   //     // <amba_device_try_add>:
//   //     // ...
//   //     // cbnz    x3, ffffffc01062b630 <amba_device_try_add+0xf8>
//   //     gpr[3].ubits = 0;
//   //     if (possible) raise( ShouldNotHappen() );
//   //     return possible;
//   //   }

//   if (current_insn_addr == 0xffffffc010008cbc)
//     {
//       return (gpr[0].ubits == gpr[8].ubits) and (gpr[0].value == gpr[8].value);
//     }

//   if (current_insn_addr == 0xffffffc0105c70c4)
//     {
//       return possible;
//     }

//   // if (current_insn_addr == 0xffffffc0105c70c4)
//   //   {
//   //     gpr[0].ubits = 0;
//   //     return possible;
//   //   }

//   // static struct { uint64_t address; bool result; }
//   // exceptions [] =
//   //   {
//   //    {0xff3, false}, /* has_mismatched_cache_type */
//   //   };

//   // static uintptr_t idx =0;
//   // uintptr_t const end = sizeof (exceptions) / sizeof (exceptions[0]);

//   // if (idx < end and exceptions[idx].address == current_insn_addr)
//   //   return exceptions[idx++].result;

//   uninitialized_error("condition");

//   struct Bad {}; raise( Bad() );
//   return possible;
// }

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

AArch64::GIC::GIC()
  : D_CTLR()
{}

unsigned
AArch64::GIC::ScanPendingInterruptsFor( uint8_t required, uint8_t enough ) const
{
  unsigned hppi = 1023;

  if ((C_CTLR & D_CTLR & 1) == 0)
    return hppi;

  // Preemption not supported
  for (unsigned word = 0; word < ITLinesCount/32; ++word)
    if (D_IACTIVE[word])
      return hppi;

  for (unsigned word = 0; word < ITLinesCount/32; ++word)
    {
      uint32_t valid = D_IPENDING[word] & D_IENABLE[word];
      for (;valid; valid = (valid-1) & valid)
        {
          unsigned int_id = word*32 + unisim::util::arithmetic::BitScanForward(valid);
          uint8_t priority = D_IPRIORITYR[int_id];
          if (priority < enough)
            return int_id;
          if (priority < required)
            {
              hppi = int_id;
              required = priority;
            }
        }
    }

  return hppi;
}

struct AArch64::Device::Request
{
  Request(unsigned _dev, uint64_t _addr, uint8_t _size, bool _write) : dev(_dev), addr(_addr), size(_size), write(_write) {}
  virtual ~Request() {}
  virtual void transfer(uint8_t*, uint8_t*, unsigned) = 0;

  template <class T>
  bool tainted_access( T& reg )
  {
    typedef typename T::value_type value_type;
    static unsigned const bytecount = sizeof(value_type);
    if (bytecount > size) return false;
    uint8_t vbytes[bytecount], ubytes[bytecount];
    if (not write) for (unsigned idx = 0; idx < bytecount; ++idx) { vbytes[idx] = reg.value >> 8*idx; ubytes[idx] = reg.ubits >> int(8*idx); }
    transfer(&vbytes[0], &ubytes[0], bytecount);
    addr += bytecount; size -= bytecount;
    if (    write) for (unsigned idx = bytecount; idx-- > 0;) { reg.value = reg.value << 8 | vbytes[idx]; reg.ubits = reg.ubits << 8 | ubytes[idx]; }
    return true;
  }
  template <typename T>
  bool access( T& reg )
  {
    struct Undefined {};
    struct _
    {
      typedef T value_type; _(value_type& _v) : value(_v) {}
      T& value; struct U {
        U& operator >> (int) { return *this; } U& operator << (int) { return *this; }
        operator uint8_t() { return 0; } U& operator = (uint8_t u) { if (u) throw Undefined(); return *this; }
      } ubits;
    } _(reg);
    try { return tainted_access ( _ ); } catch(Undefined const&) {}
    return false;
  }
  template <typename T> bool ro(T  value) { return not write and access(value); }
  template <typename T> bool wo(T& value) { return     write and access(value); }
  static void location(std::ostream& sink, uint64_t addr, unsigned size)
  {
    sink << std::hex << addr << std::dec << "[" << size << "]";
  }
  unsigned dev; uint64_t addr; uint8_t size; bool write;
};

bool
AArch64::Device::write(AArch64& arch, uint64_t addr, uint8_t const* dbuf, uint8_t const* ubuf, uint64_t size) const
{
  struct IRequest : public Request
  {
    typedef uint8_t const* Src;
    IRequest(unsigned dev, uint64_t addr, Src _vsrc, Src _usrc, uint8_t size )
      : Request(dev, addr, size, true), vsrc(_vsrc) , usrc(_usrc) {} Src vsrc, usrc;
    void transfer(uint8_t* vdst, uint8_t* udst, unsigned n) override
    { std::copy(vsrc, vsrc + n, vdst); std::copy(usrc, usrc + n, udst); vsrc += n; usrc += n; }
  } idata(id, addr, dbuf, ubuf, size);

  do
    {
      if (effect->access(arch, idata))
        continue;
      effect->get_name(id, std::cerr << "Unmapped ");
      idata.location(std::cerr << " register write @", addr, size);
      std::cerr << '\n';
      return false;
    }
  while (idata.size);

  return true;
}

bool
AArch64::Device::read(AArch64& arch, uint64_t addr, uint8_t* dbuf, uint8_t* ubuf, uint64_t size) const
{
  struct ORequest : public Request
  {
    typedef uint8_t* Dst;
    ORequest(unsigned dev, uint64_t addr, Dst _vdst, Dst _udst, uint8_t size )
      : Request(dev, addr, size, false), vdst(_vdst), udst(_udst) {} Dst vdst, udst;
    void transfer(uint8_t* vsrc, uint8_t* usrc, unsigned n) override
    { std::copy(vsrc, vsrc + n, vdst); std::copy(usrc, usrc + n, udst); vdst += n; udst += n; }
  } odata(id, addr, dbuf, ubuf, size);

  do
    {
      if (effect->access(arch, odata))
        continue;
      effect->get_name(id, std::cerr << "Unmapped ");
      odata.location( std::cerr << " register read @", addr, size );
      std::cerr << '\n';
      return false;
    }
  while (odata.size);

  return true;
}

bool
AArch64::Device::Effect::error(unsigned dev, char const* msg) const
{
  this->get_name(dev, std::cerr);
  std::cerr << " error: " << msg << "\n";
  return false;
}

void
AArch64::handle_rtc()
{
  if (not rtc.matching(*this) or rtc.masked)
    return;
  gic.set_interrupt(34);
  gic.program(*this);
}

void
AArch64::RTC::program(AArch64& cpu)
{
  uint64_t delay = (MR - LR)*cpu.get_freq()/get_cntfrq() - (cpu.insn_timer - load_insn_time) + 1;

  cpu.notify( delay, &AArch64::handle_rtc );
}

void
AArch64::map_rtc(uint64_t base_addr)
{
  static struct : public Device::Effect
  {
    virtual void get_name(unsigned id, std::ostream& sink) const override { sink << "Real Time Clock (PL031)"; }

    virtual bool access(AArch64& arch, Device::Request& req) const override
    {
      // req.location(std::cerr << "<=o=>  register " << (req.write ? "write" : "read") << " @", req.addr, req.size);
      // std::cerr << '\n';
      auto& rtc = arch.rtc;

      if (req.addr == 0x0) // Data Register, RTCDR
        return req.ro(rtc.get_counter(arch));
      if (req.addr == 0x4) // Match Register, RTCMR
        {
          if (not req.access(rtc.MR)) return false;
          if (not req.write) return true;
          rtc.program(arch);
          return true;
        }
      if (req.addr == 0x8) // Load Register, RTCLR
        {
          if (not req.access(rtc.LR)) return false;
          if (not req.write) return true;
          rtc.flush_lr(arch);
          rtc.program(arch);
          return true;
        }
      if (req.addr == 0xc) // Control Register, RTCCR
        {
          uint32_t started = rtc.started;
          if (not req.write) return req.ro(started);
          if (not req.access(started)) return false;
          if (started & rtc.started) { rtc.LR = 0; rtc.flush_lr(arch); }
          rtc.started = started;
          rtc.program(arch);
          return true;
        }
      if (req.addr == 0x10) // Interrupt Mask Set or Clear register, RTCIMSC
        {
          uint32_t masked = rtc.masked;
          if (not req.write) return req.ro(masked);
          if (not req.access(masked)) return false;
          if (masked and not rtc.masked) arch.handle_rtc();
          rtc.masked = masked;
          return true;
        }
      if (req.addr == 0x14) // Raw Interrupt Status, RTCRIS
        return req.ro<uint32_t>(rtc.matching(arch));
      if (req.addr == 0x18) // Masked Interrupt Status, RTCMIS
        return req.ro<uint32_t>(rtc.matching(arch) and not rtc.masked);
      if (req.addr == 0x1c) // Interrupt Clear Register, RTCICR
        { uint32_t icr; return req.wo(icr); }
      if ((req.addr & -16) == 0xfe0)
        {
          if (req.write) return error( req.dev, "Cannot write PeriphID" );
          /* ARM PrimeCell (R) Real Time Clock (PL031) Revision: r1p3 */
          uint32_t value(0x00041031);
          uint8_t byte = (req.addr & 3) ? uint8_t(0) : uint8_t(value >> 8*(req.addr >> 2 & 3));
          return req.access(byte);
        }

      if ((req.addr & -16) == 0xff0)
        {
          if (req.write) return error( req.dev, "Cannot write PCellID" );

          uint8_t byte = (req.addr & 3) ? 0 : (0xB105F00D >> 8*(req.addr >> 2 & 3));
          return req.access(byte);
        }

      return false;
    }

  } rtc_effect;

  devices.insert( Device( base_addr, base_addr + 0xfff, &rtc_effect, 0) );
}

void
AArch64::map_virtio_placeholder(unsigned id, uint64_t base_addr)
{
  static struct : public Device::Effect
  {
    virtual void get_name(unsigned id, std::ostream& sink) const override { sink << "Virtio Empty Placeholder (" << id << ")"; }

    virtual bool access(AArch64& arch, Device::Request& req) const override
    {
      if (req.addr % 4 or req.size != 4) return false;
      switch (req.addr / 4)
        {
        case 0: return req.ro<uint32_t>(0x74726976); /* Magic Value */
        case 1: return req.ro<uint32_t>(0x2); /* Device version number */
        case 2: return req.ro<uint32_t>(0); /* Virtio Subsystem Device ID */
        }
      return false;
    }
  } virtio_placeholder_effect;

  devices.insert( Device( base_addr, base_addr + 0x1ff, &virtio_placeholder_effect, id) );
}

namespace {
  //  virtual Slice access(uint64_t addr, uint64_t size, bool is_write) const = 0;
  
  struct VIOA : public VIOAccess
  {
    VIOA(AArch64& _core, unsigned _irq) : core(_core), irq(_irq) {} AArch64& core; unsigned irq;

    uint64_t read(uint64_t addr, unsigned size) const override
    {
      struct Bad {};
      uint8_t dbuf[size], ubuf[size];
      if (core.get_page(addr).read(addr,&dbuf[0],&ubuf[0],size) != size) raise( Bad() );
      uint64_t res = 0;
      for (unsigned idx = size; idx-- > 0;)
        { if (ubuf[idx]) raise( Bad() ); res = (res << 8) | dbuf[idx]; }
      return res;
    }
    
    void write(uint64_t addr, unsigned size, uint64_t value) const override
    {
      struct Bad {};
      uint8_t dbuf[size], ubuf[size];
      for (unsigned idx = 0; idx < size; ++idx)
        { dbuf[idx] = value & 0xff; value >>= 8; }
      std::fill(&ubuf[0], &ubuf[size], 0);
      if (core.get_page(addr).write(addr,&dbuf[0],&ubuf[0],size) != size)
        throw Bad {};
    }
  
    struct Iterator
    {
      Iterator(uint64_t addr, uint64_t size) : page(), ptr(addr), left(size), pbytes(0) {}
      AArch64::Page const* page;
      uint64_t ptr, left, pbytes;
      bool next(AArch64& core)
      {
        if (uint64_t nleft = left - pbytes)
          { left = nleft; ptr += pbytes; }
        else
          return false;
        page = &core.get_page(ptr);
        pbytes = std::min(page->size_from(ptr), left);
        return true;
      }
      uint8_t*  slice_data() const { return page->data_abs(ptr); }
      uint8_t*  slice_udat() const { return page->udat_abs(ptr); }
      uintptr_t slice_size() const { return pbytes; }
    };

    // void flag(uint64_t addr) const override
    // {
    //   AArch64::Page const& page = core.get_page(addr);
    //   *page.udat_abs(addr) = 0xff;
    // }

    void notify() const override
    {
      core.gic.set_interrupt(irq);
      core.gic.program(core);
    }
  };
}

void
ArchDisk::uread(uint8_t* udat, uint64_t size)
{
  uint64_t pos = tell();
  auto itr = upages.lower_bound(Page::index(pos));

  struct { char const* sep = "[Tainted] Disk read "; auto next() { auto r = sep; sep = " "; return r; } } sep;
  for (uint64_t left = size, psize = 0; left; left -= psize, udat += psize, pos += psize)
    {
      psize = std::min(Page::size_from(pos), left);
      
      if (itr == upages.end() or itr->base > pos)
        std::fill(&udat[0], &udat[psize], 0);
      else
        {
          std::cerr << sep.next() << '.' << std::hex << pos;
          itr->read(pos, psize, udat);
          ++itr;
        }
    }
  if (*sep.sep == ' ')
    std::cerr << " (" << std::hex << tell() << ", " << size << ")\n";
}

void
ArchDisk::read(VIOAccess const& vioa, uint64_t addr, uint64_t size)
{
  AArch64& core = dynamic_cast<VIOA const&>(vioa).core;
  for (VIOA::Iterator itr(addr, size); itr.next(core);)
    {
      uread(itr.slice_udat(), itr.slice_size());
      dread(itr.slice_data(), itr.slice_size());
    }
}

void
ArchDisk::uwrite(uint8_t const* udat, uint64_t size)
{
  uint64_t pos = tell();
  auto itr = upages.lower_bound(Page::index(pos));
  
  struct { char const* sep = "[Tainted] Disk write "; auto next() { auto r = sep; sep = " "; return r; } } sep;
  for (uint64_t left = size, psize = 0; left; left -= psize, udat += psize, pos += psize)
    {
      psize = std::min(Page::size_from(pos), left);
      bool has_udat = std::any_of(&udat[0], &udat[psize], [](uint8_t b) { return b != 0; } );

      if (itr == upages.end() or itr->base > pos)
        {
          if (not has_udat) continue;
          std::cerr << sep.next() << '+' << std::hex << pos;
          itr = upages.insert(itr, pos);
        }
      else if (not has_udat and Page::index(psize))
        {
          std::cerr << sep.next() << '-' << std::hex << pos;
          itr = upages.erase(itr);
          continue;
        }
      else
        {
          std::cerr << sep.next() << '!' << std::hex << pos;
        }
      itr->write(pos, psize, udat);
      ++itr;
    }
  if (*sep.sep == ' ')
    std::cerr << " (" << std::hex << tell() << ", " << size << ") -> " << std::dec << upages.size() << " pages\n";
}

void
ArchDisk::write(VIOAccess const& vioa, uint64_t addr, uint64_t size)
{
  AArch64& core = dynamic_cast<VIOA const&>(vioa).core;
  for (VIOA::Iterator itr(addr, size); itr.next(core);)
    {
      uwrite(itr.slice_udat(), itr.slice_size());
      dwrite(itr.slice_data(), itr.slice_size());
    }
}

VolatileDisk::~VolatileDisk()
{
  munmap(storage, disksize);
}

void
VolatileDisk::open(char const* filename)
{
  struct Error {};
  int fd = ::open(filename,O_RDONLY);
  struct stat info;
  if (fstat(fd, &info) < 0)
    throw Error();
  Capacity = (info.st_size + BLKSIZE - 1) / BLKSIZE;
  disksize = Capacity * BLKSIZE;
  storage = (uint8_t*)mmap(0, disksize, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
  ::close(fd);
  if (not storage)
    throw Error();
}

void
VolatileDisk::sync(SnapShot& snapshot)
{
  std::cerr << "VolatileDisk storage lost...\n";
  VIODisk::sync(snapshot);
}

void
StreamDisk::open(char const* filename)
{
  storage.open(filename);
  if (not storage or not storage.is_open()) { struct Bad {}; raise( Bad() ); }
  uint64_t size = storage.seekg( 0, std::ios::end ).tellg();
  Capacity = size / BLKSIZE;
}

void
AArch64::map_virtio_disk(char const* filename, uint64_t base_addr, unsigned irq)
{
  disk.open(filename);

  static struct : public Device::Effect
  {
    virtual void get_name(unsigned id, std::ostream& sink) const override { sink << "Virtio Block Device (disk)"; }

    virtual bool access(AArch64& arch, Device::Request& req) const override
    {
      //      req.location(std::cerr << "<=o=>  register " << (req.write ? "write" : "read") << " @", req.addr, req.size);
      //      std::cerr << '\n';

      VIODisk& viodisk = arch.disk;
      if ((req.addr|req.size) & (req.size-1)) /* alignment issue */
        return false;

      VIOA vioa(arch, req.dev);

      if      (req.size == 1)
        {
          switch (req.addr)
            {
            case 0x118: return req.ro<uint8_t>(0); // logical blocks per physical block (log)
            case 0x119: return req.ro<uint8_t>(0); // offset of first aligned logical block
            case 0x120: return req.access(viodisk.WriteBack);
            }
        }

      else if (req.size == 2)
        {
          switch (req.addr)
            {
            case 0x11a: return req.ro<uint16_t>(0); // suggested minimum I/O size in blocks
            }
        }

      else if (req.size == 4)
        {
          uint32_t tmp;
          switch (req.addr)
            {
            case 0x00: return req.ro<uint32_t>(0x74726976);              /* Magic Value: 'virt' */
            case 0x04: return req.ro<uint32_t>(0x2);                     /* Device version number: Virtio 1 - 1.1 */
            case 0x08: return req.ro<uint32_t>(2);                       /* Virtio Subsystem Device ID: block device */
            case 0x0c: return req.ro(viodisk.Vendor());                  /* Virtio Subsystem Vendor ID: 'usvp' */
            case 0x10: return req.ro(viodisk.ClaimedFeatures());         /* features supported by the device */
            case 0x14: return req.wo(viodisk.DeviceFeaturesSel);         /* Device (host) features word selection. */
            case 0x20: return req.wo(tmp) and viodisk.UsedFeatures(tmp); /* features used by the driver  */
            case 0x24: return req.wo(viodisk.DriverFeaturesSel);         /* Activated (guest) features word selection */
            case 0x30: return req.wo(tmp) and (tmp == 0);                /* Virtual queue index (only 0: rq) */
            case 0x34: return req.ro(viodisk.QueueNumMax());             /* Maximum virtual queue size */
            case 0x38: return req.wo(viodisk.rq.size);                   /* Virtual queue size */
            case 0x44: return req.access(viodisk.rq.ready)               /* Virtual queue ready bit */
                and (not req.write or viodisk.SetupQueue(vioa));
            case 0x50: return req.wo(tmp) and viodisk.ReadQueue(vioa);   /* Queue notifier */
            case 0x60: return req.ro(viodisk.InterruptStatus);           /* Interrupt status */
            case 0x64: return req.wo(tmp) and viodisk.InterruptAck(tmp); /* Interrupt status */
            case 0x70: return req.access(viodisk.Status)                 /* Device status */
                and (not req.write or viodisk.CheckStatus());
            case 0x80:
            case 0x84: return req.wo((reinterpret_cast<uint32_t*>(&viodisk.rq.desc_area))[req.addr >> 2 & 1]);
            case 0x90:
            case 0x94: return req.wo((reinterpret_cast<uint32_t*>(&viodisk.rq.driver_area))[req.addr >> 2 & 1]);
            case 0xa0:
            case 0xa4: return req.wo((reinterpret_cast<uint32_t*>(&viodisk.rq.device_area))[req.addr >> 2 & 1]);
            case 0xfc: return req.ro(viodisk.ConfigGeneration);
              //            case 0x100: return req.ro(viodisk.Capacity);
            case 0x100:
            case 0x104: return req.access( (reinterpret_cast<uint32_t*>(&viodisk.Capacity))[req.addr >> 2 & 1] );
              // case 0x108: return req.ro<uint32_t>(viodisk.SizeMax());
            case 0x10c: return req.ro(viodisk.SegMax());
            case 0x114: return req.ro(viodisk.BlkSize());
            case 0x11c: return req.ro<uint32_t>(0); // optimal (suggested maximum) I/O size in blocks
            case 0x124: return req.ro(viodisk.MaxDiscardSectors());
            case 0x128: return req.ro(viodisk.MaxDiscardSeg());
            case 0x12c: return req.ro(viodisk.DiscardSectorAlignment());
            case 0x130: return req.ro(viodisk.MaxWriteZeroesSectors());
            }
        }

      return false;
    }
  } virtio_disk_effect;

  devices.insert( Device( base_addr, base_addr + 0x1ff, &virtio_disk_effect, irq) );
}

void
AArch64::map_uart(uint64_t base_addr)
{
  static struct : public Device::Effect
  {
    virtual void get_name(unsigned, std::ostream& sink) const override { sink << "UART (PL011)"; }

    virtual bool access(AArch64& arch, Device::Request& req) const override
    {
      AArch64::UART& uart = arch.uart;

      if ((req.addr & -4) == 0x0)
        {
          if (req.addr & 3) { uint8_t dummy=0; return req.access(dummy); }
          char ch = '\0';
          if (not req.write and not uart.rx_pop( ch )) { U8 x = PartiallyDefined<uint8_t>(0,-1); return req.tainted_access(x);  }
          if (not req.access( ch ))                    { return false; }
          if (    req.write)                           { uart.tx_push( ch ); }
          return true;
        }

      if (req.size == 2)
        {
          switch (req.addr)
            {
            case 0x18: return req.ro(uart.flags());  /* Flag Register */
            case 0x24: return req.access(uart.IBRD); /* Integer Baud Rate Register */
            case 0x28: return req.access(uart.FBRD); /* Fractional Baud Rate Register */
            case 0x2c: return req.access(uart.LCR);  /* Line Control Register */
            case 0x30: return req.access(uart.CR);   /* Control Register */
            case 0x34: return req.access(uart.IFLS); /* Interrupt FIFO Level Select Register */
            case 0x38: return req.access(uart.IMSC); /* Interrupt Mask Set/Clear Register */
            case 0x3c: return req.ro(uart.RIS);      /* Raw Interrupt Status Register */
            case 0x40: return req.ro(uart.mis());    /* Masked Interrupt Status Register */
            }
        }

      if (req.addr == 0x44)
        {
          if (not req.write) return error(req.dev, "Cannot read Interrupt Clear Register" );
          uint16_t icr = 0;
          if (not req.access(icr)) return false;
          uart.RIS &= ~icr;
          return true;
        }

      if ((req.addr & -16) == 0xfe0)
        {
          if (req.write) return error(req.dev, "Cannot write PeriphID" );
          /* ARM PrimeCell (R) UART (PL011) Revision: r1p5 */
          uint32_t value(0x00341011);
          uint8_t byte = (req.addr & 3) ? uint8_t(0) : uint8_t(value >> 8*(req.addr >> 2 & 3));
          return req.access(byte);
        }

      if ((req.addr & -16) == 0xff0)
        {
          if (req.write) return error(req.dev, "Cannot write PCellID" );

          uint8_t byte = (req.addr & 3) ? 0 : (0xB105F00D >> 8*(req.addr >> 2 & 3));
          return req.access(byte);
        }

      return false;
    }

  } uart_effect;

  devices.insert( Device( base_addr, base_addr + 0xfff, &uart_effect, 0) );

  handle_uart();
}

void
AArch64::handle_uart()
{
  uart.rx_push();
  uart.tx_pop();

  if (uart.mis())
    {
      gic.set_interrupt(33);
      gic.program(*this);
    }

  uint64_t const insns_per_char = 84000;
  notify( insns_per_char, &AArch64::handle_uart );
}

// void
// AArch64::uart_tx()
// {
//   uart.tx_pop();
//   handle_uart();
// }

void
AArch64::UART::tx_push(char ch)
{
  tx_count += 1;
  char_io_import->PutChar(ch);
}

void
AArch64::UART::tx_pop()
{
  if (tx_count == 0)
    return;

  char_io_import->FlushChars();

  tx_count = 0; // Infinite throughput
  RIS |= TX_INT;
}

void
AArch64::map_gic(uint64_t base_addr)
{
  static struct : public Device::Effect
  {
    virtual void get_name(unsigned id, std::ostream& sink) const override { sink << "Generic Interrupt Controller (GICv2)"; }

    virtual bool access(AArch64& arch, Device::Request& req) const override
    {
      uint32_t raz_wi_32(0);

      if (req.addr == 0x100) /* CPU Interface Control Register */
        return req.access(arch.gic.C_CTLR);

      if (req.addr == 0x104) /* Interrupt Priority Mask Register */
        return req.access(arch.gic.C_PMR);

      if (req.addr == 0x10c) /* Interrupt Acknowledge Register */
        {
          if (req.write) return error(req.dev, "Cannot write GICC_IAR" );

          unsigned int_id = arch.gic.HighestPriorityPendingInterrupt();
          if (int_id < arch.gic.ITLinesCount)
            arch.gic.ack_interrupt(int_id);

          U32 reg = U32(0)
            | PartiallyDefined<uint32_t>(0,0x7ffff)     << 13  // Reserved
            | PartiallyDefined<uint32_t>(0,0x7)         << 10  // CPUID
            | U32(int_id)        <<  0; // An product identifier (linux wants 0bx..x0000)

          return req.tainted_access(reg);
        }

      if (req.addr == 0x110) /* End of Interrupt Register */
        {
          if (not req.write) return error(req.dev, "Cannot read GICC_EOIR" );
          U32 reg;
          if (not req.tainted_access(reg))
            return false;
          unsigned int_id = 0xffff;
          { U32 field = reg & U32(0x3ff); if (field.ubits) { struct Bad {}; raise( Bad() ); }; int_id = field.value; }
          arch.gic.eoi_interrupt(int_id);
          return true;
        }

      if (0x1d0 <= req.addr and req.addr < 0x1f0)
        {
          if (req.addr & 3) return false;
          /* Active Priorities Registers (secure and not secure) */
          return req.access(raz_wi_32);
        }

      if (req.addr == 0x1fc) /* Identification Register, GICC_IIDR */
        {
          if (req.write) return error(req.dev, "Cannot write GICC_IIDR" );
          U32 reg = U32(0)
            | PartiallyDefined<uint32_t>(0,0xff0) << 20  // An product identifier (linux wants 0bx..x0000)
            | U32(2)                              << 16  // Architecture version (GICv2)
            | PartiallyDefined<uint32_t>(0,0xf)   << 12  // Revision
            | U32(0x43b)                          <<  0; // The JEP106 identity and continuation code of ARM.

          return req.tainted_access(reg);
        }

      if (req.addr == 0x1000) /* Distributor Control Register */
        return req.access( arch.gic.D_CTLR );

      if (req.addr == 0x1004) /* Interrupt Controller Type Register */
        {
          if (req.write) return error(req.dev, "Cannot write GICD_TYPER" );
          U32 reg = U32(0)
            | PartiallyDefined<uint32_t>(0,0xffff) << 16  // Reserved
            | PartiallyDefined<uint32_t>(0,0x1f)   << 11  // Maximum number of implemented lockable SPIs (Sec. Ext.)
            | PartiallyDefined<uint32_t>(0,1)      << 10  // Indicates whether the GIC implements the Security Extensions
            | PartiallyDefined<uint32_t>(0,3)      <<  8  // Reserved
            | U32(0)                               <<  5  // Indicates the number of implemented CPU interfaces
            | U32(arch.gic.ITLinesNumber)          <<  0; // Indicates the maximum number of interrupts that the GIC supports
          return req.tainted_access(reg);
        }

      if (0x1100 <= req.addr and req.addr < 0x1400)
        {
          /* The GICD_I[S|C][ENABLER|PENDR|ACTIVER]n registers provide
           * a [Set-|Clear-][enable|pending|active] bit for each
           * interrupt supported by the GIC.
           */
          if (req.addr % 4) return false;
          unsigned idx = (req.addr - 0x1100)/4, cns = idx / 32, rtp = cns / 2;
          idx %= 32; cns %= 2;
          if (32*idx >= arch.gic.ITLinesCount)
            return req.access(raz_wi_32);
          auto iscreg = rtp == 0 ? &AArch64::GIC::D_IENABLE : rtp == 1 ? &AArch64::GIC::D_IPENDING : &AArch64::GIC::D_IACTIVE;
          uint32_t& reg = (arch.gic.*iscreg)[idx];
          uint32_t value = reg;
          if (not req.access(value))
            return false;
          if (req.write) { if (cns) reg &= ~value; else reg |= value; }
          return true;
        }

      if (0x1400 <= req.addr and req.addr < 0x1800)
        {
          /* The GICD_IPRIORITYRs provide an 8-bit priority field for
           * each interrupt supported by the GIC. This field stores
           * the priority of the corresponding interrupt.
           */
          unsigned idx = req.addr - 0x1400;
          if (idx < arch.gic.ITLinesCount)
            return req.access( arch.gic.D_IPRIORITYR[idx] );
          uint8_t raz_wi(0);
          return req.access(raz_wi);
        }

      if (0x1800 <= req.addr and req.addr < 0x1c00)
        {
          /* The GICD_ITARGETSRs provide an 8-bit CPU targets. Note:
           * In a uniprocessor implementation, all interrupts target
           * the one processor, and the GICD_ITARGETSRs are RAZ/WI.
           */
          uint8_t raz_wi(0);
          return req.access(raz_wi);
        }

      if (0x1c00 <= req.addr and req.addr < 0x1d00)
        {
          /* The GICD_ICFGRs provide a 2-bit Int_config field for each
           * interrupt supported by the GIC.
           */
          unsigned idx = (req.addr - 0x1c00)/4;
          if (16*idx < arch.gic.ITLinesCount)
            return req.access( arch.gic.D_ICFGR[idx] );
          return req.access(raz_wi_32);
        }
      return false;
    }
  } gic_effect;

  devices.insert( Device( base_addr, base_addr + 0x1fff, &gic_effect, 0) );
}

bool
AArch64::Timer::activated() const
{
  /* Timer enabled and interruption not masked */
  return ctl % 4 == 1;
}

void
AArch64::handle_vtimer()
{
  if (not vt.activated())
    return;
  if (vt.read_tval(*this) > 0)
    return vt.program(*this); /* Condition not met */

  gic.set_interrupt(27);
  gic.program(*this);
}


void
AArch64::Timer::program(AArch64& cpu)
{
  if (not activated())
    return;

  int32_t delay = read_tval(cpu);
  if (delay < 1) delay = 1;

  cpu.notify( delay * cpu.get_freq() / get_cntfrq(), &AArch64::handle_vtimer );
}

bool
AArch64::has_irqs() const
{
  if (pstate.I)
    return false; /* IRQs masked */
  return gic.HasPendingInterrupt();
}

void
AArch64::handle_irqs()
{
  if (not has_irqs())
    return;

  TakePhysicalIRQException();
  throw Abort();
}

void
AArch64::GIC::program(AArch64& cpu)
{
  if (not cpu.has_irqs())
    return;

  cpu.notify(1, &AArch64::handle_irqs);
}

void
AArch64::reload_next_event()
{
  if (next_events.size())
    next_event = next_events.begin()->first;
  else
    next_event = insn_timer + 0x1000000;
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
AArch64::run( uint64_t suspend_at )
{
  for (;;)
    {
      random = random * 22695477 + 1;
      insn_timer += 1;// + ((random >> 16 & 3) == 3);

      /* Instruction boundary next_insn_addr becomes current_insn_addr */
      uint64_t insn_addr = this->current_insn_addr = this->next_insn_addr;

      //if (insn_addr == halt_on_addr) { Stop(0); return; }

      try
        {
          /* Handle asynchronous events */
          while (next_event <= insn_timer)
            {
              event_handler_t method = pop_next_event();
              if (not method) break;
              (this->*method)();
            }

          if (insn_counter == suspend_at)
            throw Suspend();
          
          Operation* op = fetch_and_decode(insn_addr);

          this->next_insn_addr += 4;

          if (current_insn_addr == bdaddr)
            {
              breakdance();
              // disasm = true;
            }

          if (current_insn_addr == 0x7fba767d38)
            {
              std::cerr << "Buggy loop entered at instruction #" << std::dec << insn_counter << " and tick #"  << insn_timer << "\n";
              force_throw();
            }

          if (terminate) { force_throw(); }

          if (disasm)
            {
              //static std::ofstream dbgtrace("dbgtrace");
              std::ostream& sink( std::cerr );
              sink << "@" << std::hex << insn_addr << ": " << std::setfill('0') << std::setw(8) << op->GetEncoding() << "; ";
              DisasmState ds;
              op->disasm( ds, sink );
              sink << std::endl;
            }

          /* Execute instruction */
          asm volatile( "arm64_operation_execute:" );
          op->execute( *this );

          // if (unlikely(requires_commit_instruction_reporting and memory_access_reporting_import))
          //   memory_access_reporting_import->ReportCommitInstruction(this->current_insn_addr, 4);

          insn_counter++; /* Instruction regularly finished */
        }

      catch (Abort const& abort)
        {
          abort.proceed(*this);
          /* Instruction aborted, proceed to next */
          //   if (unlikely(trap_reporting_import))
          //     trap_reporting_import->ReportTrap( *this, "Data Abort Exception" );
        }
      
      catch (Suspend const&)
        {
          std::cerr << "Suspend...\n";
          save_snapshot("uvp.shot");
          return;
        }
    }
}

void
AArch64::wfi()
{
  struct Bad {};

  /* Check for pending interrupts regardless of pstate.{D,A,I,F} */
  if (gic.HasPendingInterrupt())
    return; /* CPU has at least one pending interrupt, job's done */

  if (next_events.size() == 0)
    raise( Bad() );

  /* advance to next event and notify back to check progress*/
  insn_timer = next_events.begin()->first;
  notify( 0, &AArch64::wfi );
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

  try
    {
      /* 8 bytes access should not cross MMU page boundaries. */
      MMU::TLB::Entry entry( addr );
      translate_address(entry, 1, mem_acc_type::read);
      PMemDump64(entry.pa);
    }
  catch (DataAbort const&)
    {
      std::cerr << "<translation error>.\n";
    }
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
  /* TODO: should be an Alignment-related DataAbort */
  MMU::TLB::Entry entry(addr.value);
  translate_address(entry, pstate.GetEL(), mem_acc_type::read);
  // By design, access cannot cross mmu page boundaries
  excmon.set(entry.pa, size);
}

bool
AArch64::ExclusiveMonitorsPass( U64 addr, unsigned size )
{
  struct Bad {};
  if (addr.ubits) raise( Bad() );
  if ((size | addr.value) & (size - 1)) raise( Bad() ); /* TODO: should be an Alignment-related DataAbort */
  MMU::TLB::Entry entry(addr.value);
  translate_address(entry, pstate.GetEL(), mem_acc_type::write);
  // By design, access cannot cross mmu page boundaries
  bool passed = excmon.pass( entry.pa, size );
  excmon.clear();
  return passed;
}

void
AArch64::ClearExclusiveLocal()
{
  excmon.clear();
}

void
AArch64::memory_fault(MemFault const& mf, char const* operation, uint64_t vaddr, uint64_t paddr, unsigned size)
{
  std::cerr << std::hex << current_insn_addr << " error during " << operation;
  if (mf.op) std::cerr << " (" << mf.op << ")";
  std::cerr << " operation at {vaddr= " << vaddr << ", paddr= " << paddr << ", size= " << std::dec << size << "}\n";
  struct Bad {}; raise(Bad());
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

// Function used for permission checking from AArch64 stage 1 translations
void
AArch64::CheckPermission(MMU::TLB::Entry const& trans, uint64_t vaddress, unsigned el, mem_acc_type::Code mat)
{
  // AArch64
  bool wxn = unisim::component::cxx::processor::arm::RegisterField<19,1>().Get(el1.SCTLR); // Cacheable

  unsigned ap = trans.ap | 2*(el != 0);

  bool perm_r = (ap & 2) == 2;
  bool perm_w = (ap & 6) == 2;
  bool perm_x = not ((perm_w and wxn) or (el ? trans.pxn or (trans.ap & 6) == 2 : trans.xn));

  bool fail = true;
  switch (mat)
    {
    case mem_acc_type::read:               fail = not perm_r; break;
    case mem_acc_type::cache_maintenance:
    case mem_acc_type::write:              fail = not perm_w; break;
    case mem_acc_type::exec:               fail = not perm_x; break;
    case mem_acc_type::debug:              fail = false; break;
    default: throw 0;
    }

  if (fail)
    throw DataAbort(unisim::component::cxx::processor::arm::DAbort_Permission,
                    vaddress, trans.pa, mat, trans.level, /*ipavalid*/true, /*secondstage*/false, /*s2fs1walk*/false);
}

AArch64::UART::UART(char const* name, unisim::kernel::Object* parent)
  : unisim::kernel::Object(name, parent)
  , unisim::kernel::Client<unisim::service::interfaces::CharIO>(name,parent)
  , char_io_import("char-io-import",this)
  , rx_value('\0'), rx_valid(false), tx_count()
  , IBRD(0), FBRD(0), LCR(0), CR(0x0300), IFLS(0x12), IMSC(0), RIS(0)
{
}

bool
AArch64::UART::rx_push()
{
  if (not rx_valid) rx_valid = char_io_import->GetChar(rx_value);
  if (rx_valid) RIS |= RX_INT;
  return rx_valid;
}

bool
AArch64::UART::rx_pop(char& ch)
{
  if (not rx_push())
    return false;
  ch = rx_value;
  rx_valid = false;
  RIS &= ~RX_INT;
  return true;
}

// AArch64::NELock::NELock(AArch64& a)
//   : m(&a.next_event_mutex)
// {
//   pthread_mutex_lock(m);
// }

// AArch64::NELock::~NELock()
// {
//   pthread_mutex_unlock(m);
// }

AArch64::event_handler_t
AArch64::pop_next_event()
{
  //  NELock nel(*this);
  event_handler_t method = event_handler_t();
  auto evt = next_events.begin();
  if (evt != next_events.end())
    {
      method = evt->second;
      next_events.erase(evt);
    }
  reload_next_event();
  return method;
}

void
AArch64::notify( uint64_t delay, event_handler_t method )
{
  //  NELock nel(*this);
  next_events.emplace(std::piecewise_construct, std::forward_as_tuple( insn_timer+delay ), std::forward_as_tuple( method ));
  reload_next_event();
}

void
AArch64::silence( event_handler_t method )
{
  for (auto itr = next_events.begin(), end = next_events.begin(); itr != end;)
    {
      itr = itr->second == method ? next_events.erase(itr) : std::next( itr );
    }
}

uint16_t
AArch64::UART::flags() const
{
  return
    int(tx_count ==     0) << 7 |
    int(tx_count >= qsize) << 5 |
    int(rx_valid == false) << 4;
}

void
AArch64::handle_suspend()
{
  if (suspend)
    throw Suspend();

  notify( 0x800000, &AArch64::handle_suspend );
}

void
AArch64::save_snapshot( char const* filename )
{
  std::unique_ptr<SnapShot> snapshot = SnapShot::gzsave(filename);

  sync( *snapshot );
}

void
AArch64::load_snapshot( char const* filename )
{
  std::unique_ptr<SnapShot> snapshot = SnapShot::gzload(filename);

  sync( *snapshot );
}

void
AArch64::PState::sync(SnapShot& snapshot)
{
  uint8_t d = D; snapshot.sync(d); D = d;
  uint8_t a = A; snapshot.sync(a); A = a;
  uint8_t i = I; snapshot.sync(i); I = i;
  uint8_t f = F; snapshot.sync(f); F = f;
  uint8_t ss = SS; snapshot.sync(ss); SS = ss;
  uint8_t il = IL; snapshot.sync(il); IL = il;
  uint8_t el = EL; snapshot.sync(el); EL = el;
  uint8_t sp = SP; snapshot.sync(sp); SP = sp;
}

void
AArch64::Page::sync(SnapShot& snapshot)
{
  snapshot.sync(base);
  snapshot.sync(last);
  if (snapshot.is_load())
    {
      Page pg(base, last);
      std::swap(pg.data,data);
      std::swap(pg.udat,udat);
      std::swap(pg.free,free);
    }
  snapshot.sync(data, size());
  snapshot.sync(udat, size());
}

void
AArch64::RTC::sync(SnapShot& snapshot)
{
  snapshot.sync(LR);
  snapshot.sync(MR);
  snapshot.sync(load_insn_time);
  snapshot.sync(started);
  snapshot.sync(masked);
}

void
AArch64::Timer::sync(SnapShot& snapshot)
{
  snapshot.sync(kctl);
  snapshot.sync(ctl);
  snapshot.sync(cval);
}

void
AArch64::UART::sync(SnapShot& snapshot)
{
  for (auto reg : {&IBRD, &FBRD, &LCR, &CR, &IFLS, &IMSC, &RIS})
    snapshot.sync(*reg);
}

void
AArch64::GIC::sync(SnapShot& snapshot)
{
  snapshot.sync(C_CTLR);
  snapshot.sync(C_PMR);
  snapshot.sync(D_CTLR);
  for (unsigned idx = ITLinesCount/32; idx-->0;)
    {
      snapshot.sync(D_IENABLE[idx]);
      snapshot.sync(D_IPENDING[idx]);
      snapshot.sync(D_IACTIVE[idx]);
    }
  snapshot.sync(&D_IPRIORITYR[0], ITLinesCount);
  for (unsigned idx = ITLinesCount/16; idx-->0;)
    snapshot.sync(D_ICFGR[idx]);
}

AArch64::U8
AArch64::GetTVU8(unsigned reg0, unsigned elements, unsigned regs, U8 const& index, U8 const& oob_value)
{
  struct Bad {};
  if (index.ubits) raise( Bad() );
  unsigned e = index.value % elements, r = index.value / elements;
  return r < regs ? GetVU8((reg0+r)%32, e) : oob_value;
}

void
AArch64::MMU::sync(SnapShot& snapshot)
{
  for (auto reg : {&MAIR_EL1, &TCR_EL1, &TTBR0_EL1, &TTBR1_EL1})
    snapshot.sync(*reg);
}

namespace
{
  template <typename T> void tvsync(SnapShot& snapshot, TaintedValue<T>& tv)
  {
    snapshot.sync(tv.value);
    snapshot.sync(tv.ubits);
  }
}

void
AArch64::sync(SnapShot& snapshot)
{
  //  NELock nel(*this);
  {
    std::array<event_handler_t,6> methods =
      {
        &AArch64::wfi,
        &AArch64::handle_irqs,
        &AArch64::handle_vtimer,
        &AArch64::handle_rtc,
        &AArch64::handle_uart,
        &AArch64::handle_suspend,
      };

    for (auto evt = next_events.begin();; ++evt)
      {
        uint64_t date = snapshot.is_save() and evt != next_events.end() ? evt->first : 0;
        snapshot.sync(date);
        if (not date) break;
      
        if (snapshot.is_load())
          {
            int mid; snapshot.sync(mid);
            evt = next_events.emplace_hint( evt, std::piecewise_construct,
                                            std::forward_as_tuple( date ), std::forward_as_tuple( methods[mid] ));
          }
        else
          {
            auto itr = std::find(methods.begin(), methods.end(), evt->second);
            if (itr == methods.end()) { struct Bad{}; raise( Bad() ); }
            int mid = itr - methods.begin();
            snapshot.sync(mid);
          }
      }
  }

  gic.sync(snapshot);
  uart.sync(snapshot);
  vt.sync(snapshot);
  rtc.sync(snapshot);
  disk.sync(snapshot);

  if (snapshot.is_save())
    {
      snapshot.save(pages.size());
      for (auto& pg : pages)
        const_cast<Page&>(pg).sync(snapshot);
    }
  else
    {
      uint64_t page_count; snapshot.sync(page_count);
      while (page_count-->0)
        pages.insert(pages.end(), snapshot);
    }

  snapshot.sync(excmon.addr);
  snapshot.sync(excmon.size);
  snapshot.sync(excmon.valid);

  mmu.sync(snapshot);
    
  for (unsigned reg = 0; reg < 32; ++reg)
    tvsync(snapshot, gpr[reg]);

  for (unsigned reg = 0; reg < 32; ++reg)
    for (unsigned idx = 0; idx < 2; ++idx)
      tvsync(snapshot, (vector_views[reg].GetStorage(&vectors[reg], U64(), VUConfig::BYTECOUNT))[idx]);

  tvsync(snapshot, fpcr);
  tvsync(snapshot, fpsr);
  tvsync(snapshot, fpcr);
  tvsync(snapshot, sp_el[0]);
  tvsync(snapshot, sp_el[1]);

  for (auto reg : {&el1.SPSR, &el1.ESR})
    tvsync(snapshot, *reg);
  for (auto reg : {&el1.ELR, &el1.VBAR, &el1.PAR})
    tvsync(snapshot, *reg);
  snapshot.sync(el1.FAR);
  snapshot.sync(el1.SCTLR);
  
  pstate.sync(snapshot);
  
  tvsync(snapshot, nzcv);
  for (auto reg : {&current_insn_addr, &next_insn_addr, &insn_counter, &insn_timer})
    snapshot.sync(*reg);
  tvsync(snapshot, TPIDR[0]);
  tvsync(snapshot, TPIDR[1]);
  tvsync(snapshot, TPIDRRO);
  snapshot.sync(CPACR);
  snapshot.sync(random);
}

// std::ofstream&
// AArch64::ptlog()
// {
//   static std::ofstream sink("ptlog");
//   return sink;
// }

