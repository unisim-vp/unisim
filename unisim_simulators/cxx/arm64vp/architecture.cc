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

#include <architecture.hh>
#include <unisim/component/cxx/processor/arm/isa_arm64.tcc>
#include <iostream>
#include <iomanip>

AArch64::AArch64()
  : devices()
  , gic()
  , pages()
  , mmu()
  , ipb()
  , decoder()
  , gpr()
  , sp_el()
  , el1()
  , pstate()
  , nzcv()
  , current_insn_addr()
  , next_insn_addr()
  , TPIDR()
  , CPACR()
  // , event()
  , disasm(false)
{
  std::cout << "Let's go!\n";
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
  unsigned const PAGE_SIZE = 0x10000;
  uint64_t base = addr & -PAGE_SIZE, last = base + PAGE_SIZE - 1;
  if (pi != pages.end() and pi->last >= base)
    base = pi->last + 1;
  if (pages.size() and pi != pages.begin() and (--pi)->base <= last)
    { last = pi->base - 1; }

  unsigned size = last-base+1;
  auto udat = new uint8_t[size];
  std::fill(&udat[0], &udat[size], -1);
  static struct : Page::Free { void free (Page& page) const override { delete [] page.get_data(); delete [] page.get_udat(); } } free;
  return *pages.insert(pi, Page(0, base, last, new uint8_t[size], udat, &free));
}

bool
AArch64::new_page(uint64_t addr, uint64_t size)
{
  auto udat = new uint8_t[size];
  std::fill(&udat[0], &udat[size], -1);
  static struct : Page::Free { void free (Page& page) const override { delete [] page.get_data(); delete [] page.get_udat(); } } free;
  return this->mem_map( Page(0, addr, addr+size-1, new uint8_t[size], udat, &free) );
}

AArch64::Page::Free AArch64::Page::Free::nop;

uint8_t*
AArch64::IPB::get(AArch64& core, uint64_t vaddr)
{
  uint64_t address = core.translate_address(vaddr, mat_exec, LINE_SIZE);
  uint64_t req_base_address = address & -LINE_SIZE;
  unsigned idx = vaddr % LINE_SIZE;
  if (base_address != req_base_address)
    {
      uint8_t ubuf[LINE_SIZE];
      if (core.access_page(address).read(req_base_address, &bytes[0], &ubuf[0], LINE_SIZE) != LINE_SIZE)
        { struct Bad {}; throw Bad(); }
      if (not std::all_of( &ubuf[0], &ubuf[LINE_SIZE], [](unsigned char const byte) { return byte == 0; } ))
        { struct Bad {}; throw Bad(); }
      base_address = req_base_address;
    }

  return &bytes[idx];
}

void
AArch64::step_instruction()
{
  /* Instruction boundary next_insn_addr becomes current_insn_addr */
  uint64_t insn_addr = this->current_insn_addr = this->next_insn_addr;

  //if (insn_addr == halt_on_addr) { Stop(0); return; }

  try
    {
      // Instruction Fetch Decode and Execution (may generate exceptions
      // known as synchronous aborts since their occurences are a direct
      // consequence of the instruction execution).

      // Fetch
      unisim::component::cxx::processor::arm::isa::arm64::CodeType insn = 0;
      for (uint8_t *beg = ipb.get(*this, insn_addr), *itr = &beg[4]; --itr >= beg;)
        insn = insn << 8 | *itr;

      /* Decode current PC. TODO: should provide physical address for caching purpose */
      Operation* op = current_insn_op = decoder.Decode(insn_addr, insn);

      this->next_insn_addr += 4;

      // if (current_insn_addr == 0xffffffc01058cc18)
      //   {
      //     breakdance();
      //     disasm = true;
      //   }
      if (disasm)
        {
          std::cerr << "@" << std::hex << insn_addr << ": " << std::hex << std::setfill('0') << std::setw(8) << insn << "; ";
          op->disasm( *this, std::cerr );
          std::cerr << std::endl;
        }

      /* Execute instruction */
      asm volatile( "arm64_operation_execute:" );
      op->execute( *this );

      // uint64_t newspval = GetGSR(31);

      // if (newspval != oldspval) {
      //   std::cerr << std::hex  << "  SP: 0x" << oldspval << " => 0x" << newspval << std::endl;
      // }

      // if (unlikely(requires_commit_instruction_reporting and memory_access_reporting_import))
      //   memory_access_reporting_import->ReportCommitInstruction(this->current_insn_addr, 4);

      // instruction_counter++; /* Instruction regularly finished */
    }

  catch (Abort const&)
    {
      /* Instruction aborted, proceed to next */
      //   if (unlikely(trap_reporting_import))
      //     trap_reporting_import->ReportTrap( *this, "Data Abort Exception" );
    }
  
  catch (std::exception const& exc)
    {
      std::cerr << "Unimplemented exception <" << exc.what() << "> @pc=" << std::hex << current_insn_addr << std::dec << std::endl;
      throw exc;
    }
}

void
AArch64::UndefinedInstruction(unisim::component::cxx::processor::arm::isa::arm64::Operation<AArch64> const* op)
{
  op->disasm(*this, std::cerr << "Undefined instruction : `");
  std::cerr << "` (" << op->GetName() << ", " << std::hex << op->GetEncoding()
            << "@" << std::hex << op->GetAddr() << ").\n";
  UndefinedInstruction();
}

void
AArch64::UndefinedInstruction()
{
  struct Bad {}; throw Bad ();
}

void
AArch64::TODO()
{
  //struct Bad {}; throw Bad ();
  //  std::cerr << "!TODO!\n";
}

/** CallSupervisor
 *
 *  This method is called by SWI/SVC instructions to handle software interrupts.
 * @param imm     the "imm" field of the instruction code
 */
void
AArch64::CallSupervisor( uint32_t imm )
{
  // if (verbose) {
  //   static struct ArmLinuxOS : public unisim::util::os::linux_os::Linux<uint32_t, uint32_t>
  //   {
  //     typedef unisim::util::os::linux_os::ARMTS<unisim::util::os::linux_os::Linux<uint32_t,uint32_t> > ArmTarget;

  //     ArmLinuxOS( CPU* _cpu )
  //       : unisim::util::os::linux_os::Linux<uint32_t, uint32_t>( _cpu->logger, _cpu, _cpu, _cpu )
  //     {
  //       SetTargetSystem(new ArmTarget( "arm-eabi", *this ));
  //     }
  //     ~ArmLinuxOS() { delete GetTargetSystem(); }
  //   } arm_linux_os( this );

  //   logger << DebugInfo << "PC: 0x" << std::hex << GetCIA() << EndDebugInfo;
  //   arm_linux_os.LogSystemCall( imm );
  // }

  // we are executing on full system mode
  //  throw SVCException();
  struct Bad {}; throw Bad();
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
        throw Bad();
      
      switch (arg0.value)
        {
        default:
          throw Bad();
        case 0x84000000: // PSCI_0_2_FN_VERSION
          gpr[0] = U64(2); // VERSION=0.2
          return;
        case 0x84000006: // PSCI_0_2_FN_MIGRATE_INFO_TYPE:
          /* Trusted OS is not present */
          gpr[0] = U64(2); // PSCI_0_2_TOS_MP;
          return;
        }
    }

  throw Bad();
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
AArch64::MMU::TLB::invalidate(bool nis, bool ll, unsigned type, AArch64& cpu, AArch64::U64 const& arg)
{
  if (not nis)
    {
      cpu.TODO(); // Should send to all Inner Shareable PE...
      return invalidate(true, ll, type, cpu, arg);
    }

  struct Bad {};

  uint64_t ubit_issue = 0;
  switch (type)
    {
    case 1: ubit_issue = 0xffff0fffffffffffull; break;
    case 2: ubit_issue = 0xffff000000000000ull; break;
    case 3: ubit_issue = 0x00000fffffffffffull; break;
    }
  if (arg.ubits & ubit_issue) throw Bad {};

  uint64_t vakey = (((arg.value << 12) << 15 ) >> 16) | ((arg.value >> 48) << 48) | 1;

  for (unsigned idx = 0; idx < kcount; ++idx)
    {
      uint64_t key = keys[idx];
      unsigned rsh = key & 63;

      if (ll and rsh > 15) /* rsh = blocksize - 1; */
        continue;

      if (type == 2 and ((vakey ^ key) >> 48)) // by asid
        continue;

      if ((type & 1) and (((vakey ^ key) >> rsh) << (rsh + (((key >> 2) | (2 << type)) & 16)))) // by va
        continue;

      // key matches -> invalidate
      keys[idx] = 0;
    }
}

template <class POLICY>
bool
AArch64::MMU::TLB::GetTranslation( AArch64::MMU::TLB::Entry& result, uint64_t vaddr, unsigned asid )
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

  if (not POLICY::DEBUG)
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

uint64_t
AArch64::translate_address(uint64_t vaddr, mem_acc_type_t mat, unsigned size)
{
  if (not unisim::component::cxx::processor::arm::sctlr::M.Get(el1.SCTLR))
    return vaddr;

  MMU::TLB::Entry entry;
  // Stage 1 MMU enabled
  unsigned asid = mmu.GetASID();
  if (unlikely(not mmu.tlb.GetTranslation<PlainAccess>( entry, vaddr, asid )))
    {
      translation_table_walk<PlainAccess>( entry, vaddr, mat, size );
      mmu.tlb.AddTranslation( entry, vaddr, asid );
    }
  // else {
  //   // Check if hit is coherent
  //   TLB::Entry tlbe_chk;
  //   translation_table_walk<QuietAccess>( tlbe_chk, mva, mat, size );
  //   if (tlbe_chk.pa != tlbe.pa)
  //     trap_reporting_import->ReportTrap( *this, "Incoherent TLB access" );
  // }
  return entry.pa;
}

template <class POLICY>
void
AArch64::translation_table_walk( AArch64::MMU::TLB::Entry& entry, uint64_t vaddr, mem_acc_type_t mat, unsigned size )
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
      if (cpu.access_page(paddr).read(paddr,&dbuf[0],&ubuf[0],size) != size) throw Bad();
      uint64_t value = 0;
      for (unsigned idx = size; idx-- > 0;)
      { if (ubuf[idx]) throw Bad(); value = (value << 8) | dbuf[idx ^ endianness]; }
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
    { struct Bad {}; throw Bad(); }

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
          DataAbort(unisim::component::cxx::processor::arm::DAbort_Translation, vaddr, 0, mat, level, /*ipavalid*/false, /*secondstage*/false, /*s2fs1walk*/false);

        if (not (desc & 2))
          {
            /*illegal at page grain*/
            if (level == 3) { struct Bad {}; throw Bad(); }
            break; /* a block */
          }
        else if (level == 3)
          break; /* a page*/

        /* a table */
        tbladdr = ((desc >> grainsize) << (grainsize+16)) >> 16;
        attr_table &= ((desc >> 59) ^ 0b00011);
      }
  }
  if (direct_bits > 32) { struct Bad {}; throw Bad(); }

  unsigned vaclr = 64 - direct_bits;
  entry.pa = (((desc >> direct_bits) << (direct_bits + 16)) >> 16) | ((vaddr << vaclr) >> vaclr);
  entry.blocksize = direct_bits;
  entry.ap = (((desc >> 4) | (attr_table ^ 4) | 2) >> 1) & 7;
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
AArch64::DataAbort(unisim::component::cxx::processor::arm::DAbort type, uint64_t va, uint64_t ipa, mem_acc_type_t mat, unsigned level, bool ipavalid, bool secondstage, bool s2fs1walk)
{
  unsigned const target_el = 1;

  unsigned vect_offset = 0x0;
  if (target_el > pstate.EL)
    vect_offset += 0x400; /* + 0x600 if lower uses AArch32 */
  else if (pstate.SP)
    vect_offset += 0x200;


  // spsr = GetSPSRFromPSTATE();
  U32 spsr = GetSPSRFromPSTATE();
  // ESR[target_el] = ec<5:0>:il:syndrome;

  get_el(target_el).ESR = U32(0)
    | U32((mat == mat_exec ? 0x20 : 0x24) + (pstate.EL == target_el)) << 26 // exception class
    | U32(1) << 25 // IL
    | U32(0,0b11111111111) << 14 // extended syndrome information for a second stage fault.
    | U32(0,0b1111) << 10
    | U32(0,0b1) << 9 // IsExternalAbort(fault)
    | U32(0,0b1) << 8 // IN {AccType_DC, AccType_IC}
    | U32(s2fs1walk) << 7
    | U32(mat == mat_write) << 6
    | U32(unisim::component::cxx::processor::arm::EncodeLDFSC(type, level)) << 0;
  // FAR[target_el] = exception.vaddress;
  get_el(target_el).FAR = U64(va);
  //     if target_el == EL2 && exception.ipavalid then
  //     HPFAR_EL2<39:4> = exception.ipaddress<47:12>;

  pstate.EL = target_el;
  SetPStateSP(1);

  get_el(target_el).SPSR = spsr;
  get_el(target_el).ELR = U64(current_insn_addr);

  pstate.D = 1;
  pstate.A = 1;
  pstate.I = 1;
  pstate.F = 1;

  {
    U32 nRW(1);
    nRW &= spsr >> 4;
    if ((nRW.ubits | nRW.value) & 1) { struct NoAArch32 {}; throw NoAArch32(); }
  }

  // BranchTo(VBAR[] + vect_offset, BranchType_EXCEPTION);
  BranchTo( get_el(target_el).VBAR + U64(vect_offset), B_EXC );

  EndOfInstruction();
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

void
AArch64::breakdance()
{
  std::cerr << "Debug time!\n";
}

bool
AArch64::concretize()
{
  static struct { uint64_t address; bool result; }
  exceptions [] =
    {
     {0xff3, false}, /* has_mismatched_cache_type */
    };

  static uintptr_t idx =0;
  uintptr_t const end = sizeof (exceptions) / sizeof (exceptions[0]);

  if ((idx >= end) or (exceptions[idx].address != current_insn_addr))
    { struct Bad {}; throw Bad (); }
  
  return exceptions[idx++].result;
}

AArch64::GIC::GIC()
  : D_CTLR()
{}

namespace
{
  struct Request
  {
    virtual ~Request() {}
    virtual void transfer(uint8_t*, uint8_t*, unsigned) = 0;
    Request(uint64_t _addr, uint8_t _size, bool _write) : addr(_addr), size(_size), write(_write) {}
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
    uint64_t addr; uint8_t size; bool write;
  };

  struct IRequest : public Request
  {
    typedef uint8_t const* Src;
    IRequest(uint64_t _addr, Src _vsrc, Src _usrc, uint8_t _size ) : Request( _addr, _size, true ), vsrc(_vsrc) , usrc(_usrc) {} Src vsrc, usrc;
    void transfer(uint8_t* vdst, uint8_t* udst, unsigned n) override
    { std::copy(vsrc, vsrc + n, vdst); std::copy(usrc, usrc + n, udst); vsrc += n; usrc += n; }
  };
  struct ORequest : public Request
  {
    typedef uint8_t* Dst;
    ORequest(uint64_t _addr, Dst _vdst, Dst _udst, uint8_t _size ) : Request( _addr, _size, false ), vdst(_vdst), udst(_udst) {} Dst vdst, udst;
    void transfer(uint8_t* vsrc, uint8_t* usrc, unsigned n) override
    { std::copy(vsrc, vsrc + n, vdst); std::copy(usrc, usrc + n, udst); vdst += n; udst += n; }
  };
}

void
AArch64::map_gic(uint64_t base_addr)
{
  static struct GICEffect : public Device::Effect
  {
    static bool error( char const* msg ) { std::cerr << "GIC error: " << msg << "\n"; return false; }

    static bool access(AArch64& arch, Request& req)
    {
      uint32_t raz_wi_32(0);

      if (req.addr == 0x100) /* CPU Interface Control Register */
        return req.access(arch.gic.C_CTLR);
      
      if (req.addr == 0x104) /* Interrupt Priority Mask Register */
        return req.access(arch.gic.C_PMR);

      if (0x1d0 <= req.addr and req.addr < 0x1f0)
        {
          /* Active Priorities Registers (secure and not secure) */
          return req.access(raz_wi_32);
        }

      if (req.addr == 0x1fc) /* Identification Register, GICC_IIDR */
        {
          if (req.write) return error( "Cannot write GICC_IIDR" );
          U32 reg = U32(0)
            | U32(0,0xff0)       << 20  // An product identifier (linux wants 0bx..x0000)
            | U32(2)             << 16  // Architecture version (GICv2)
            | U32(0,0xf)         << 12  // Revision
            | U32(0x43b)         <<  0; // The JEP106 identity and continuation code of ARM.

          return req.tainted_access(reg);
        }
      
      if (req.addr == 0x1000) /* Distributor Control Register */
        return req.access( arch.gic.D_CTLR );
      
      if (req.addr == 0x1004) /* Interrupt Controller Type Register */
        {
          if (req.write) return error( "Cannot write GICD_TYPER" );
          U32 reg = U32(0)
            | U32(0,0xffff)      << 16  // Reserved
            | U32(0,0x1f)        << 11  // Maximum number of implemented lockable SPIs (Sec. Ext.)
            | U32(0,1)           << 10  // Indicates whether the GIC implements the Security Extensions
            | U32(0,3)           <<  8  // Reserved
            | U32(0)             <<  5  // Indicates the number of implemented CPU interfaces
            | U32(arch.gic.ITLinesNumber) <<  0; // Indicates the maximum number of interrupts that the GIC supports
          return req.tainted_access(reg);
        }
      
      if (0x1100 <= req.addr and req.addr < 0x1400)
        {
          /* The GICD_I[S|C][ENABLER|PENDR|ACTIVER]n registers provide
           * a [Set-|Clear-][enable|pending|active] bit for each
           * interrupt supported by the GIC.
           */
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

      std::cerr << "Unmapped GIC register @" << std::hex << req.addr << "\n";
      return false;
    }
    
    virtual    bool write(AArch64& arch, uint64_t addr, uint8_t const* dbuf, uint8_t const* ubuf, uint64_t size) const override
    {
      IRequest idata(addr, dbuf, ubuf, size);
      do {} while (idata.size and access(arch, idata));
      return not idata.size;
    }
    
    virtual uint64_t read(AArch64& arch, uint64_t addr, uint8_t* dbuf, uint8_t* ubuf, uint64_t size) const override
    {
      ORequest odata(addr, dbuf, ubuf, size);
      do {} while (odata.size and access(arch, odata));
      return not odata.size;
    }
  } gic_effect;

  devices.insert( Device( base_addr, base_addr + 0x1fff, &gic_effect) );
}

