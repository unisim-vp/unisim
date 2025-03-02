/*
 *  Copyright (c) 2019,
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
 *          Gilles Mouchard (gilles.mouchard@cea.fr)
 */

#include <architecture.hh>
#include <unisim/component/cxx/processor/arm/isa/arm64/disasm.hh>
#include <unisim/component/cxx/processor/opcache/opcache.tcc>
#include <unisim/util/debug/simple_register.hh>
#include <unisim/util/os/linux_os/linux.hh>
#include <unisim/util/os/linux_os/aarch64.hh>
#include <unisim/util/likely/likely.hh>
#include <unisim/util/endian/endian.hh>
#include <iostream>
#include <iomanip>
#include <array>
#include <cassert>

#define MMU_PERM_CHECKER 0

AArch64::AArch64(char const* name)
  : unisim::kernel::Object(name, 0)
  , unisim::kernel::Service<unisim::service::interfaces::Registers>(name, 0)
  , unisim::kernel::Service<unisim::service::interfaces::Memory<uint64_t> >(name, 0)
  , unisim::kernel::Service<unisim::service::interfaces::MemoryInjection<uint64_t> >(name, 0)
  , unisim::kernel::Service<unisim::service::interfaces::Disassembly<uint64_t> >(name, 0)
  , unisim::kernel::Service<unisim::service::interfaces::MemoryAccessReportingControl>(name, 0)
  , unisim::kernel::Client<unisim::service::interfaces::MemoryAccessReporting<uint64_t>>(name, 0)
  , unisim::kernel::Client<unisim::service::interfaces::DebugYielding>(name, 0)
  , recorder("recorder", this)
  , memory_export("memory-export", this)
  , registers_export("registers-export", this)
  , memory_injection_export("memory-injection-export", this)
  , requires_memory_access_reporting(false)
  , requires_fetch_instruction_reporting(false)
  , requires_commit_instruction_reporting(false)
  , memory_access_reporting_import("memory-access-reporting-import", this)
  , debug_yielding_import("debug-yielding-import", this)
  , trap_reporting_import("trap-reporting-import", this)
  , linux_os_import("linux-os-import", this)
  , next_events()
  , next_event(~uint64_t(0))
  , regmap()
  , devices()
  , gic()
  , uart("uart", 0)
  , vt()
  , persistent_disk(false)
  , param_persistent_disk("persistent-disk", this, persistent_disk, "enable/disable disk persistency")
  , pages()
  , mru_pi(pages.end())
  , excmon()
  , dbgmon()
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
  , suspend_at(-1)
  , param_suspend_at("suspend-at", this, suspend_at, "Instruction counter when to suspend execution")
  , last_insns()
  , TPIDR()
  , CPACR()
  , tvreg(-1)
  , bdaddr(0)
  , rng("rng", this)
  , terminate(false)
  , disasm(false)
  , param_disasm("disasm", this, disasm, "enable instruction disassembly before executing instructions")
  , debug_el_mask(3)
  , param_debug_el_mask("debug-el-mask", this, debug_el_mask, "EL mask (one bit per EL) for filtering debug events")
  , suspend(false)
  , tfpstats_filename()
  , param_tfpstats_filename("tfpstats_filename", this, tfpstats_filename, "filename of tfpstats dump")
  , tfp32count(0)
  , tfp64count(0)
  , tfp32loss(0)
  , tfp64loss(0)
  , param_tfp32loss("tfp32loss", this, tfp32loss, "tainted fp32 bit loss count")
  , param_tfp64loss("tfp64loss", this, tfp64loss, "tainted fp64 bit loss count")
  , tfpinsncount()
  , enable_sleep(false)
  , param_enable_sleep("enable-sleep", this, enable_sleep, "enable sleeping")
  , enable_exception_trap(false)
  , param_enable_exception_trap("enable-exception-trap", this, enable_exception_trap, "enable exception trap in debugger")
  , enable_strace(false)
  , param_enable_strace("enable-strace", this, enable_strace, "enable strace")
{
  last_insns = new InstructionInfo[histsize];

  param_tfp32loss.SetFormat(unisim::kernel::VariableBase::FMT_DEC);
  param_tfp64loss.SetFormat(unisim::kernel::VariableBase::FMT_DEC);

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

  /** Specific Current Program Status Register Debugging Accessor */
  struct CurrentProgramStatusRegister : public unisim::service::interfaces::Register
  {
    CurrentProgramStatusRegister( AArch64& _cpu ) : cpu(_cpu) {}
    virtual const char *GetName() const { return "cpsr"; }
    virtual void GetValue( void* buffer ) const { *((uint32_t *)buffer) = cpu.GetPSRFromPSTATE().value; }
    virtual void SetValue( void const* buffer ) { cpu.SetPSTATEFromPSR(U32(*(uint32_t *)buffer)); }
    virtual int  GetSize() const { return 4; }
    virtual void Clear() { cpu.SetPSTATEFromPSR(U32()); }
    AArch64&        cpu;
  };

  regmap["cpsr"] = new CurrentProgramStatusRegister( *this );

  /** Exception Link Register Debugging Accessor */
  struct ExceptionLinkRegister : public unisim::service::interfaces::Register
  {
    ExceptionLinkRegister( AArch64& _cpu ) : cpu(_cpu) {}
    virtual const char *GetName() const { return "elr"; }
    virtual void GetValue( void* buffer ) const { *((uint64_t *)buffer) = cpu.get_el(1).ELR.value; }
    virtual void SetValue( void const* buffer ) { cpu.get_el(1).ELR = U64(*(uint32_t *)buffer); }
    virtual int  GetSize() const { return 8; }
    virtual void Clear() { cpu.get_el(1).ELR = U64(0); }
    AArch64&        cpu;
  };

  regmap["elr"] = new ExceptionLinkRegister( *this );

  /** EL0 Read-Only Software Thread ID register Debugging Accessor */
  struct TPIDRRO_EL0 : public unisim::service::interfaces::Register
  {
    TPIDRRO_EL0( AArch64& _cpu ) : cpu(_cpu) {}
    virtual const char *GetName() const { return "tpidrro_el0"; }
    virtual void GetValue( void* buffer ) const { *((uint64_t *)buffer) = cpu.TPIDRRO.value; }
    virtual void SetValue( void const* buffer ) {}
    virtual int  GetSize() const { return 8; }
    virtual void Clear() {}
    AArch64&        cpu;
  };

  regmap["tpidrro_el0"] = new TPIDRRO_EL0( *this );

  /** EL0 Software Thread ID register Debugging Accessor */
  struct TPIDR_EL0 : public unisim::service::interfaces::Register
  {
    TPIDR_EL0( AArch64& _cpu ) : cpu(_cpu) {}
    virtual const char *GetName() const { return "tpidr_el0"; }
    virtual void GetValue( void* buffer ) const { *((uint64_t *)buffer) = cpu.TPIDR[0].value; }
    virtual void SetValue( void const* buffer ) { cpu.TPIDR[0] = U64(*(uint32_t *)buffer); }
    virtual int  GetSize() const { return 8; }
    virtual void Clear() { cpu.TPIDR[0] = U64(0); }
    AArch64&        cpu;
  };

  regmap["tpidr_el0"] = new TPIDR_EL0( *this );

  /** EL1 Software Thread ID register Debugging Accessor */
  struct TPIDR_EL1 : public unisim::service::interfaces::Register
  {
    TPIDR_EL1( AArch64& _cpu ) : cpu(_cpu) {}
    virtual const char *GetName() const { return "tpidr_el1"; }
    virtual void GetValue( void* buffer ) const { *((uint64_t *)buffer) = cpu.TPIDR[1].value; }
    virtual void SetValue( void const* buffer ) { cpu.TPIDR[1] = U64(*(uint32_t *)buffer); }
    virtual int  GetSize() const { return 8; }
    virtual void Clear() { cpu.TPIDR[1] = U64(0); }
    AArch64&        cpu;
  };

  regmap["tpidr_el1"] = new TPIDR_EL1( *this );

  /** EL2 Software Thread ID register Debugging Accessor */
  struct TPIDR_EL2 : public unisim::service::interfaces::Register
  {
    TPIDR_EL2( AArch64& _cpu ) : cpu(_cpu) {}
    virtual const char *GetName() const { return "tpidr_el2"; }
    virtual void GetValue( void* buffer ) const { *((uint64_t *)buffer) = cpu.TPIDR[2].value; }
    virtual void SetValue( void const* buffer ) { cpu.TPIDR[2] = U64(*(uint32_t *)buffer); }
    virtual int  GetSize() const { return 8; }
    virtual void Clear() { cpu.TPIDR[2] = U64(0); }
    AArch64&        cpu;
  };

  regmap["tpidr_el2"] = new TPIDR_EL2( *this );

  /** EL3 Software Thread ID register Debugging Accessor */
  struct TPIDR_EL3 : public unisim::service::interfaces::Register
  {
    TPIDR_EL3( AArch64& _cpu ) : cpu(_cpu) {}
    virtual const char *GetName() const { return "tpidr_el3"; }
    virtual void GetValue( void* buffer ) const { *((uint64_t *)buffer) = cpu.TPIDR[3].value; }
    virtual void SetValue( void const* buffer ) { cpu.TPIDR[3] = U64(*(uint32_t *)buffer); }
    virtual int  GetSize() const { return 8; }
    virtual void Clear() { cpu.TPIDR[3] = U64(0); }
    AArch64&        cpu;
  };

  regmap["tpidr_el3"] = new TPIDR_EL3( *this );

  struct VectorRegister : unisim::service::interfaces::Register
  {
    VectorRegister( AArch64& _cpu, unsigned _reg, std::string const& _name ) : cpu(_cpu), reg(_reg), name(_name) {}
    virtual const char *GetName() const { return name.c_str(); }
    virtual void GetValue( void* buffer ) const { for(unsigned sub = 0; sub < 2; ++sub) *((uint64_t*)buffer + sub) = cpu.GetVU64(reg, sub).value; }
    virtual void SetValue( void const* buffer ) { for(unsigned sub = 0; sub < 2; ++sub) cpu.SetVU64(reg, sub, typename AArch64Types::U64(*((uint64_t const*)buffer + sub))); }
    virtual int  GetSize() const { return 16; }
    virtual void Clear() { cpu.ClearHighV(reg, 0); }
    AArch64&        cpu;
    unsigned        reg;
    std::string     name;
  };

  for (int idx = 0; idx < 32; ++idx)
    {
      std::ostringstream regname;
      regname << "v" << idx;
      regmap[regname.str()] = new VectorRegister( *this, idx, regname.str());
    }

  regmap["fpcr"] = new unisim::util::debug::SimpleRegister<uint32_t>("fpcr", &fpcr.value);
  regmap["fpsr"] = new unisim::util::debug::SimpleRegister<uint32_t>("fpsr", &fpsr.value);
}

AArch64::~AArch64()
{
  for (auto reg : regmap)
    delete reg.second;

  for (unsigned reg = 0; reg < VECTORCOUNT; ++reg)
    vector_views[reg].Clear(&vectors[reg]);

  delete[] last_insns;
}

bool
AArch64::ReadMemory(uint64_t addr, void* buffer, unsigned size)
{
  struct Bad {};

  uint8_t* ptr = static_cast<uint8_t*>(buffer);

  try
    {
      for (SliceIterator slice(addr, size, mem_acc_type::debug); slice.vnext(*this);)
        {
          // if (std::any_of(slice.udat, slice.udat + slice.size, [](uint8_t b) { return b != 0; } )) raise( Bad() );
          std::copy( slice.data, slice.data + slice.size, ptr );
          ptr += slice.size;
        }
    }
  catch (DataAbort const& x) { return false; }

  return true;
}

bool
AArch64::WriteMemory(uint64_t addr, void const* buffer, unsigned size)
{
  uint8_t const* ptr = static_cast<uint8_t const*>(buffer);

  try
    {
      for (SliceIterator slice(addr, size, mem_acc_type::debug); slice.vnext(*this);)
        {
          std::fill(slice.udat, slice.udat + slice.size, 0);
          std::copy(ptr,  ptr + slice.size, slice.data);
          ptr += slice.size;
        }
    }
  catch (DataAbort const& x) { return false; }

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
  scanner.Append( GetRegister("cpsr") );
  scanner.Append( GetRegister("fpcr") );
  scanner.Append( GetRegister("fpsr") );
  scanner.Append( GetRegister("elr") );
  scanner.Append( GetRegister("tpidrro_el0") );
  scanner.Append( GetRegister("tpidr_el0") );
  scanner.Append( GetRegister("tpidr_el1") );
  scanner.Append( GetRegister("tpidr_el2") );
  scanner.Append( GetRegister("tpidr_el3") );
  for (unsigned reg = 0; reg < 31; ++reg)
    {
      std::ostringstream buf;
      buf << 'v' << std::dec << reg;
      scanner.Append( GetRegister( buf.str().c_str() ) );
    }
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
  uint64_t const page_size = 0x1000000; // 16 MiB
  uint64_t base = addr & -page_size, last = base + page_size - 1;
  if (pi != pages.end() and pi->last >= base)
    base = pi->last + 1;
  if (pages.size() and pi != pages.begin() and (--pi)->base <= last)
    { last = pi->base - 1; }

  for (auto di = devices.begin(); di != devices.end(); ++di)
    if ( ((di->last < last) ? di->last : last) >= ((di->base < base) ? base : di->base) )
    {
      // Page and device overlap
      struct PageDeviceOverlap {};
      throw PageDeviceOverlap();
    }

  Page res(base, last);
  // Initialize taint
  // std::fill(res.udat_beg(), res.udat_end(), -1);
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
  ::memset(data, 0, size);
  udat = new uint8_t[size];
  ::memset(udat, 0, size);
}

AArch64::Page::Free AArch64::Page::Free::nop;

AArch64::IPB::IPB()
  : current_block(0)
  , blocks()
  , free_blocks()
  , zeroes()
{
  blocks.rehash(BUCKETS);
}

AArch64::IPB::~IPB()
{
  invalidate();
  for (auto free_block : free_blocks) delete free_block;
}

AArch64::IPB::Block::Block()
  : base_vaddress( -1 )
  , exec_perm(0)
  , ops()
  , next(0)
  , remote(0)
  , bytes()
{
}

AArch64::IPB::Block::Block(uint64_t _base_vaddress, unsigned _exec_perm)
  : base_vaddress( _base_vaddress )
  , exec_perm(_exec_perm)
  , ops()
  , next(0)
  , remote(0)
  , bytes()
{
}

AArch64::Operation*
AArch64::IPB::access(AArch64& core, uint64_t vaddr)
{
  // LINE_SIZE should be so MMU page boundaries cannot be crossed
  uint64_t base_vaddress = vaddr & -uint64_t(LINE_SIZE);
  unsigned el = core.pstate.GetEL();
  unsigned exec_perm = 1 << el;
  unsigned idx = vaddr % LINE_SIZE;
  Block *block = current_block;
  if (unlikely(not block or (block->base_vaddress != base_vaddress)))
    {
      // Current block mismatch
      if (unlikely(not block or not block->next or (base_vaddress != (block->base_vaddress + LINE_SIZE))))
        {
          // Next block mismatch
          if (unlikely(not block or not block->remote or (base_vaddress != block->remote->base_vaddress)))
            {
              // Remote block mismatch

              // Retrieve block from cache
              Blocks::iterator itr = blocks.find(base_vaddress);

              if (unlikely(itr == blocks.end()))
                {
                  // Block was never encountered

                  // Translate address and check permission for current EL
                  uint64_t base_paddress;
                  MMU::TLB::Entry tlb_entry;
                  tlb_entry.pa = base_vaddress;
                  core.translate_address(tlb_entry, el, AArch64::mem_acc_type::exec);
                  base_paddress = tlb_entry.pa & -uint64_t(LINE_SIZE);

                  // Allocate a new block
                  block = allocate_block(base_vaddress, exec_perm);

                  // Fetch block
                  uint8_t ubuf[LINE_SIZE];
                  if (core.get_page(base_paddress).read(base_paddress, &block->bytes[0], &ubuf[0], LINE_SIZE) != LINE_SIZE)
                    { struct Bad {}; raise( Bad() ); }

                  // Check block taints
                  if (::memcmp(&ubuf[0], &zeroes[0], LINE_SIZE) != 0)
                    { struct Bad {}; raise( Bad() ); }

                  // Add block into cache
                  blocks.emplace(base_vaddress, block);
                }
              else
                {
                  // Block was already encountered
                  block = (*itr).second;
                }

              // Chain current block to the retrieved block or newly allocated block
              if (likely(current_block))
                {
                  if (likely(base_vaddress == (current_block->base_vaddress + LINE_SIZE)))
                    {
                      // blocks are contiguous
                      if (unlikely(not current_block->next))
                        current_block->next = block;
                    }
                  else
                    {
                      // blocks are far from each other
                      if (unlikely(not current_block->remote))
                        current_block->remote = block;
                    }
                }
            }
          else
            {
              // Remote block address match: go to the remote block
              block = block->remote;
            }
        }
      else
        {
          // Next block address match: go to the next block
          block = block->next;
        }

      // Update the current block
      current_block = block;
    }

  if (unlikely(not (block->exec_perm & exec_perm)))
    {
      // Block permission for current EL mismatch: retranslate address to check execution permission for current EL
      MMU::TLB::Entry tlb_entry;
      tlb_entry.pa = base_vaddress;
      core.translate_address(tlb_entry, el, AArch64::mem_acc_type::exec);
      block->exec_perm |= exec_perm;
    }

  Operation *& op = block->ops[idx / 4];
  return op ? op : (op = core.decode( vaddr, unisim::util::endian::LittleEndian2Host(*(uint32_t *) &block->bytes[idx]) ));
}

AArch64::IPB::Block *
AArch64::IPB::allocate_block( uint64_t base_vaddress, unsigned exec_perm )
{
  Block *block;
  if (likely(free_blocks.size()))
    {
      // There are some free blocks
      block = free_blocks.back();
      free_blocks.pop_back();
      block->base_vaddress = base_vaddress;
      block->exec_perm = exec_perm;
      block->next = block->remote = 0;
      ::memset(block->ops, 0, sizeof(block->ops));
    }
  else
    {
      // There are no free block: allocate a new one
      block = new Block(base_vaddress, exec_perm);
    }
  return block;
}

void AArch64::IPB::invalidate()
{
  current_block = 0;
  for (Blocks::iterator itr = blocks.begin(); itr != blocks.end(); ++itr)
    {
      Block *block = (*itr).second;
      free_blocks.push_back(block);
    }
  blocks.clear();
}

AArch64::InstructionInfo const& AArch64::last_insn( int idx ) const
{
  return last_insns[(insn_counter + histsize + idx) % histsize];
}

void AArch64::UndefinedInstructionException::proceed( AArch64& cpu ) const
{
  if (op)
    cpu.TakeUndefinedInstruction(op);
  else
    cpu.TakeUndefinedInstruction();
}

void
AArch64::UndefinedInstruction(unisim::component::cxx::processor::arm::isa::arm64::Operation<AArch64> const* op)
{
  throw UndefinedInstructionException(op);
}

void
AArch64::UndefinedInstruction()
{
  throw UndefinedInstructionException();
}

void
AArch64::TakeUndefinedInstruction(unisim::component::cxx::processor::arm::isa::arm64::Operation<AArch64> const* op)
{
  DisasmState ds;
  op->disasm(ds, std::cerr << "Undefined instruction : `");
  std::cerr << "` (" << op->GetName() << ", " << std::hex << op->GetEncoding()
            << "@" << std::hex << op->GetAddr() << ").\n";
  TakeUndefinedInstruction();
}

void
AArch64::TakeUndefinedInstruction()
{
  if (unlikely(enable_exception_trap and trap_reporting_import and ((1 << pstate.GetEL()) & debug_el_mask)))
    trap_reporting_import->ReportTrap(*this,"Undefined instruction");
  else if(unlikely(linux_os_import))
    terminate = true;
  else
    {
      unsigned const target_el = 1;

      get_el(target_el).ESR = U32(0)
        | U32(0) << 26 // EC: Unknown reason
        | U32(1) << 25 // IL Instruction Length == 32 bits
        | PartlyDefined<uint32_t>(0,0b1111111111111111111111111); // ISS: Res0

      TakeException(target_el, 0x0, current_insn_addr);
    }
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
AArch64::CallSupervisor( uint32_t imm )
{
  if (unlikely(enable_strace))
    {
      static std::ofstream strace("strace.log");
      static struct Arm64LinuxOS : public unisim::util::os::linux_os::Linux<uint64_t, uint64_t>
      {
        typedef unisim::util::os::linux_os::Linux<uint64_t, uint64_t> ThisLinux;
        typedef unisim::util::os::linux_os::AARCH64TS<ThisLinux> Arm64Target;

        Arm64LinuxOS( AArch64* cpu, std::ostream& debug_info_stream, std::ostream& debug_warning_stream, std::ostream& debug_error_stream)
          : ThisLinux( debug_info_stream, debug_warning_stream, debug_error_stream )
        {
          SetInterfaces(cpu, cpu, cpu);
          SetTargetSystem(new Arm64Target(*this));
        }
        ~Arm64LinuxOS() { delete GetTargetSystem(); }
      } arm64_linux_os( this, strace, std::cerr, std::cerr );

      // AArch64::ptlog() << last_insn(0) << "\n\t";
      strace << "SVC@" << std::hex << current_insn_addr << "\n\t";
      arm64_linux_os.LogSystemCall( imm );
    }

  if (unlikely(linux_os_import))
    {
      // we are executing on linux emulation mode, use linux_os_import
      linux_os_import->ExecuteSystemCall(imm);
    }
  else
    {
      //  if UsingAArch32() then AArch32.ITAdvance();
      // SSAdvance();
      pstate.SS = 0;

      // route_to_el2 = AArch64.GeneralExceptionsToEL2();

      unsigned const target_el = 1;

      // ReportException (ESR[target_el] = ec<5:0>:il:syndrome)
      get_el(target_el).ESR = U32(0)
        | U32(0x15) << 26 // exception class AArch64.SVC
        | U32(1) << 25 // IL Instruction Length == 32 bits
        | PartlyDefined<uint32_t>(0,0b111111111) << 16 // Res0
        | U32(imm);

      TakeException(1, 0x0, next_insn_addr);
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
      struct HVCTV {};
      switch (untaint(HVCTV(), gpr[0]))
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

void
AArch64::IllegalStateException::proceed( AArch64& cpu ) const
{
    cpu.IllegalState();
}

void
AArch64::IllegalState()
{
    Fault();
}

void
AArch64::PCAlignmentException::proceed( AArch64& cpu ) const
{
    cpu.PCAlignmentFault();
}

void
AArch64::PCAlignmentFault()
{
    Fault();
}

void
AArch64::SPAlignmentException::proceed( AArch64& cpu ) const
{
    cpu.SPAlignmentFault();
}

void
AArch64::SPAlignmentFault()
{
    Fault();
}

void
AArch64::Fault()
{
  unsigned const target_el = 1;

  // ReportException (ESR[target_el] = ec<5:0>:il:syndrome)
  get_el(target_el).ESR = U32(0)
    | U32(0x22) << 26 // exception class Illegal State/AArch64.PCAlignmentFault/AArch64.SPAlignmentFault
    | U32(1) << 25 // IL Instruction Length == 32 bits
    | PartlyDefined<uint32_t>(0,0b111111111111111111111111); // Res0

  TakeException(1, 0x0, current_insn_addr);
}

/** SoftwareBreakpoint
 *
 *  This method is called by BRK instructions to handle software breakpoints.
 * @param imm     the "imm" field of the instruction code
 */
void
AArch64::SoftwareBreakpoint( uint32_t imm )
{
  unsigned const target_el = 1;

  // ReportException (ESR[target_el] = ec<5:0>:il:syndrome)
  get_el(target_el).ESR = U32(0)
    | U32(0x38) << 26 // exception class AArch64.SoftwareBreakpoint !from_32
    | U32(1) << 25 // IL Instruction Length == 32 bits
    | PartlyDefined<uint32_t>(0,0b111111111) << 16 // Res0
    | U32(imm);

  TakeException(1, 0x0, current_insn_addr);
}

void AArch64::BreakpointException::proceed( AArch64& cpu ) const
{
  cpu.Breakpoint();
}

void
AArch64::Breakpoint()
{
  unsigned const target_el = 1;

  // ReportException (ESR[target_el] = ec<5:0>:il:syndrome)
  get_el(target_el).ESR = U32(0)
    | U32((pstate.GetEL() < target_el) ? 0x30 : 0x31) << 26 // exception class AArch64.BreakpointException !from_32
    | U32(1) << 25 // IL Instruction Length == 32 bits
    | PartlyDefined<uint32_t>(0,0b1111111111111111111) << 6 // Res0
    | U32(0x22); // Instruction Fault Status Code for debug exception

  TakeException(1, 0x0, current_insn_addr);
}


void AArch64::SoftwareStepException::proceed( AArch64& cpu ) const
{
  cpu.SoftwareStep(ldex);
}

void
AArch64::SoftwareStep(bool ldex)
{
  unsigned const target_el = 1;

  // ReportException (ESR[target_el] = ec<5:0>:il:syndrome)
  get_el(target_el).ESR = U32(0)
    | U32((pstate.GetEL() < target_el) ? 0x32 : 0x33) << 26 // exception class AArch64.SoftwareStepException !from_32
    | U32(1) << 25 // IL Instruction Length == 32 bits
    | U32(1) << 24 // ISV (EX is valid)
    | PartlyDefined<uint32_t>(0,0b11111111111111111) << 7 // Res0
    | U32(ldex) << 6 // EX
    | U32(0x22); // Instruction Fault Status Code for debug exception

  TakeException(1, 0x0, current_insn_addr);
}

void AArch64::WatchpointException::proceed( AArch64& cpu ) const
{
  cpu.Watchpoint(addr, wpt, cache_maintenance, write);
}

void
AArch64::Watchpoint(uint64_t addr, unsigned wpt, bool cache_maintenance, bool write)
{
  unsigned const target_el = 1;

  // ReportException (ESR[target_el] = ec<5:0>:il:syndrome)
  get_el(target_el).ESR = U32(0)
    | U32((pstate.GetEL() < target_el) ? 0x34 : 0x35) << 26 // exception class AArch64.WatchpointException !from_32
    | U32(1) << 25 // IL Instruction Length == 32 bits
    | PartlyDefined<uint32_t>(0,0b1) << 24 // Res0
    | U32(wpt) << 18 // Watchpoint number
    | U32(1) << 17   // WPT valid
    | U32(0) << 16   // False positive (no)
    | U32(0) << 15   // FAR not Precise (no)
    | PartlyDefined<uint32_t>(0,0b1) << 14 // Res0
    | U32(0) << 13   // not from use of of VNCR_EL2 register by EL1 code
    | PartlyDefined<uint32_t>(0,0b11) << 11 // Res0
    | U32(1) << 10   // FAR is valid
    | PartlyDefined<uint32_t>(0,0b1) << 9 // Res0
    | U32(cache_maintenance) << 8  // cache maintenance
    | PartlyDefined<uint32_t>(0,0b1) << 7 // Res0
    | U32(write) << 6 // Write not Read
    | U32(0x22); // Data Fault Status Code for debug exception

  get_el(target_el).FAR = addr;

  TakeException(1, 0x0, current_insn_addr);
}

void
AArch64::SystemAccessTrapException::proceed( AArch64& cpu ) const
{
    cpu.SystemAccessTrap(op0, op1, crn, crm, op2, rt, direction);
}

void
AArch64::SystemAccessTrap( uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, uint8_t direction )
{
  unsigned const target_el = 1;

  // ReportException (ESR[target_el] = ec<5:0>:il:syndrome)
  get_el(target_el).ESR = U32(0)
    | U32(0x18) << 26 // exception class AArch64.SystemAccessTrap (i.e. Trapped MSR, MRS or System instruction execution) !from_32
    | U32(1) << 25 // IL Instruction Length == 32 bits
    | PartlyDefined<uint32_t>(0,0b11) << 22 // Res0
    | U32(op0) << 20
    | U32(op2) << 17
    | U32(op1) << 14
    | U32(crn) << 10
    | U32(rt) << 5
    | U32(crm) << 1
    | U32(direction);

  TakeException(1, 0x0, current_insn_addr);
}

AArch64::MMU::TLB::TLB()
  : keys()
{
  for (unsigned set = 0; set < sets; ++set)
    for (unsigned idx = 0; idx < assoc; ++idx)
      indices[set][idx] = idx;
}

void
AArch64::MMU::TLB::AddTranslation( AArch64::MMU::TLB::Entry const& tlbe, uint64_t vaddr, unsigned asid )
{
  // LRU Replacement
  unsigned set = (vaddr >> 12) % sets;
  unsigned lru_idx = indices[set][assoc - 1];
  if (assoc <= 8)
    {
      for (unsigned idx = assoc; --idx > 0;)
        {
          keys[set][idx] = keys[set][idx-1];
          indices[set][idx] = indices[set][idx-1];
        }
    }
  else
    {
      ::memmove(&keys[set][1], &keys[set][0], (assoc - 1) * sizeof(keys[set][0]));
      ::memmove(&indices[set][1], &indices[set][0], (assoc - 1) * sizeof(indices[set][0]));
    }

  unsigned significant = tlbe.blocksize - 1, global = not tlbe.nG;
  keys[set][0] = (((vaddr >> 12) << 27) >> 16) | (uint64_t(asid) << 48) | (global << 6) | significant;
  indices[set][0] = lru_idx;
  entries[set][lru_idx] = tlbe;
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
  struct TLBITV {};
  uint64_t argval = cpu.untaint(TLBITV(), arg & AArch64::U64(ubit_issue));

  //  AArch64::ptlog() << cpu.last_insn(0) << " (X=" << std::hex << argval << std::dec << ")\n";

  uint64_t vakey = (((argval << 12) << 15 ) >> 16) | ((argval >> 48) << 48) | 1;

  for (unsigned set = 0; set < sets; ++set)
    for (unsigned idx = 0; idx < assoc; ++idx)
      {
        uint64_t key = keys[set][idx];
        unsigned rsh = key & 63;

        if (ll and rsh > 15) /* rsh = blocksize - 1; last level in {12,14,16} (see TTBR.TG) */
          continue;

        if (type == 2 and ((vakey ^ key) >> 48)) // by asid
          continue;

        if ((type & 1) and (((vakey ^ key) >> rsh) << (rsh + (((key >> 2) | (2 << type)) & 16)))) // by va
          continue;

        // key matches -> invalidate
        keys[set][idx] = 0;
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

  unsigned set = (vaddr >> 12) % sets;
  uint64_t vakey = ((vaddr << 15) >> 16) | (uint64_t(asid) << 48) | 1;

  for (hit = 0; hit < assoc; ++hit)
    {
      key = keys[set][hit];
      rsh = key & 63;
      if (likely(not (((vakey ^ key) >> rsh) << (rsh + ((key >> 2) & 16)))))
        break;
    }
  if (unlikely(hit >= assoc))
    return false; // TLB miss

  // TLB hit
  unsigned idx = indices[set][hit];
  Entry& entry = entries[set][idx];

  if (unlikely(update && hit))
    {
      // MRU sort
      if (assoc <= 8)
        {
          for (unsigned idx = hit; idx > 0; idx -= 1)
            {
              keys[set][idx] = keys[set][idx-1];
              indices[set][idx] = indices[set][idx-1];
            }
        }
      else
        {
          ::memmove(&keys[set][1], &keys[set][0], hit * sizeof(keys[set][0]));
          ::memmove(&indices[set][1], &indices[set][0], hit * sizeof(indices[set][0]));
        }

      keys[set][0] = key;
      indices[set][0] = idx;
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
  if (unlikely(not unisim::component::cxx::processor::arm::sctlr::M.Get(el1.SCTLR)))
    return;

  uint64_t vaddr = entry.pa;

  // Stage 1 MMU enabled
  bool update = mat != mem_acc_type::debug;
  unsigned asid = mmu.GetASID();
  if (likely(not mmu.tlb.GetTranslation( entry, vaddr, asid, update )))
    {
      translation_table_walk( entry, vaddr, mat );
      if (likely(update))
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
    | cpu.PartlyDefined<uint32_t>(0,0b11111111111) << 14 // extended syndrome information for a second stage fault.
    | cpu.PartlyDefined<uint32_t>(0,0b1111) << 10
    | U32(0) << 9 // IsExternalAbort(fault)
    | U32(mat == mem_acc_type::cache_maintenance) << 8 // IN {AccType_DC, AccType_IC} Cache maintenance
    | U32(s2fs1walk) << 7
    | U32(mat == mem_acc_type::write or mat == mem_acc_type::cache_maintenance) << 6
    | U32(type.EncodeLDFSC(level)) << 0;

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

#if MMU_PERM_CHECKER
  // APTable[1:0]     Effect at subsequent lookup levels
  //   00               No effect on permissions.
  //   01            Unprivileged access not permitted.
  //   10             Write access not permitted.
  //   11             Write access not permitted.
  //               Unprivileged read access not permitted.

  unsigned xn = 0;
  unsigned pxn = 0;
  unsigned ap_table = 0b00;
#endif

  {
    uint64_t walkidx = (vaddr << tsz) >> tsz, walkmask = (1 << stride)-1;
    for (level = 3 - walkdepth; true; ++level, direct_bits -= stride)
      {
        unsigned index = (walkidx >> direct_bits) & walkmask;

        desc = descriptor.read(tbladdr + 8*index);

        if (not (desc & 1))
          throw DataAbort(unisim::component::cxx::processor::arm::DAbort::Translation, vaddr, 0, mat, level, /*ipavalid*/false, /*secondstage*/false, /*s2fs1walk*/false);

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
#if MMU_PERM_CHECKER
        ap_table |= (desc >> 61) & 3;
        xn |= (desc >> 60) & 1;
        pxn |= (desc >> 59) & 1;
#endif
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
#if MMU_PERM_CHECKER
  // Note: The Armv8 Block descriptor and Page descriptor format defines the data Access Permissions bits, AP[2:1], and does not define an AP[0] bit.
  unsigned ap = ((desc >> 5) | 1) & ~(ap_table << 1) & 7;
  xn |= (desc >> 54) & 1;
  pxn |= (desc >> 53) & 1;
  assert(ap == entry.ap);
  assert(xn == entry.xn);
  assert(pxn == entry.pxn);
#endif
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

  dbgmon.SetExceptionTakenPSTATE_SS(*this, preferred_exception_return); // This may set PSTATE.SS to 0 or 1 before saving PSTATE into SPSR_ELx.SS

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

  dbgmon.DebugSoftwareStepStateMachine(*this, std::cerr);
}

void
AArch64::SetPSTATEFromPSR(U32 spsr)
{
  struct Bad {};
  nzcv = U8(spsr >> 28);

  struct PSRTV {};
  uint32_t psr = dbgmon.ExceptionReturnSPSR(pstate.GetEL(), pstate.D, untaint(PSRTV(), (spsr << 4) >> 4) ); // This may return an SPSR value with SPSR.SS cleared

  if (psr & 0x10) raise( Bad() ); /* No AArch32 */
  /* spsr checks causing an "Illegal return" */
  if ((psr >> 2 & 3) > pstate.GetEL()) raise( Bad() );
  if (psr & 2) raise( Bad() );

  // Return an SPSR value which represents thebits(32) spsr = Zeros();
  pstate.SS = psr >> 21;
  pstate.IL = psr >> 20;
  /* AArch64*/
  pstate.D = psr >> 9;
  pstate.A = psr >> 8;
  pstate.I = psr >> 7;
  pstate.F = psr >> 6;
  pstate.SetEL(*this, psr >> 2);
  pstate.SetSP(*this, psr >> 0);

  dbgmon.DebugSoftwareStepStateMachine(*this, std::cerr);
}


void
AArch64::ExceptionReturn()
{
  if (pstate.GetEL() == 0)
    throw UndefinedInstructionException();

  U32 spsr = get_el(pstate.GetEL()).SPSR;
  U64 elr = get_el(pstate.GetEL()).ELR;
  SetPSTATEFromPSR(spsr);
  // if (pstate.GetEL() == 0)
  //   std::cerr << "ERET(" << pstate.GetEL() << ") " << std::hex << current_insn_addr << " => " << elr.value << std::endl;
  ClearExclusiveLocal();
  // SendEventLocal();

  BranchTo(elr, B_ERET);
}

uint32_t
AArch64::PState::AsSPSR() const
{
  // Return an SPSR value which represents thebits(32) spsr = Zeros();
  uint32_t spsr = 0
    | SS << 21
    | IL << 20
    /* AArch64*/
    | D << 9
    | A << 8
    | I << 7
    | F << 6
    | EL << 2
    | SP << 0;

  return spsr;
}

// bool
// AArch64::untaint(CondTV const&, BOOL const& possible)
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
  : C_CTLR()
  , C_PMR()
  , D_CTLR()
  , D_IENABLE()
  , D_IPENDING()
  , D_IACTIVE()
  , D_IPRIORITYR()
  , D_ICFGR()
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
  bool rw( T& reg )
  {
    struct Undefined {};
    struct _
    {
      typedef T value_type;
      struct Ubits
      {
        Ubits& operator >> (int) { return *this; } Ubits& operator << (int) { return *this; }
        operator uint8_t() { return 0; } Ubits& operator = (uint8_t u) { if (u) throw Undefined(); return *this; }
      };
      _(value_type& _v) : value(_v) {} T& value; Ubits ubits;
    } _(reg);
    try { return tainted_access ( _ ); } catch(Undefined const&) {}
    return false;
  }
  template <typename T> bool ro(T  value) { return not write and rw(value); }
  template <typename T> bool wo(T& value) { return     write and rw(value); }
  bool rd() const { return not write; }
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
  uint64_t delay = (MR - LR)*(cpu.get_freq() * cpu.get_ipc())/get_cntfrq() - (cpu.insn_timer - load_insn_time) + 1;

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
          if (not req.rw(rtc.MR)) return false;
          if (not req.write) return true;
          rtc.program(arch);
          return true;
        }
      if (req.addr == 0x8) // Load Register, RTCLR
        {
          if (not req.rw(rtc.LR)) return false;
          if (not req.write) return true;
          rtc.flush_lr(arch);
          rtc.program(arch);
          return true;
        }
      if (req.addr == 0xc) // Control Register, RTCCR
        {
          uint32_t started = rtc.started;
          if (not req.write) return req.ro(started);
          if (not req.rw(started)) return false;
          if (started & rtc.started) { rtc.LR = 0; rtc.flush_lr(arch); }
          rtc.started = started;
          rtc.program(arch);
          return true;
        }
      if (req.addr == 0x10) // Interrupt Mask Set or Clear register, RTCIMSC
        {
          uint32_t masked = rtc.masked;
          if (not req.write) return req.ro(masked);
          if (not req.rw(masked)) return false;
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
          return req.rw(byte);
        }

      if ((req.addr & -16) == 0xff0)
        {
          if (req.write) return error( req.dev, "Cannot write PCellID" );

          uint8_t byte = (req.addr & 3) ? 0 : (0xB105F00D >> 8*(req.addr >> 2 & 3));
          return req.rw(byte);
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

  struct VIOA : public VIODisk::Access
  {
    VIOA(AArch64& _core, unsigned _irq) : core(_core), irq(_irq) {} AArch64& core; unsigned irq;

    AArch64& GetCore() const override { return core; }

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
AArch64::map_virtio_disk(char const* filename, uint64_t base_addr, unsigned irq)
{
  disk.open(filename, persistent_disk);

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
            case 0x118: return req.ro<uint8_t>(0); /* ________ logical blocks per physical block (log) */
            case 0x119: return req.ro<uint8_t>(0); /* ________ offset of first aligned logical block */
            case 0x120: return req.rw(viodisk.WriteBack); /* _ write back*/
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
          // convenience method to handle sub register accesses (TODO: host endianness)
          struct { uint32_t& sub32(uint64_t& reg, unsigned addr) { return (reinterpret_cast<uint32_t*>(&reg))[addr >> 2 & 1]; } } r64;
          uint32_t tmp;


          switch (req.addr)
            {
            case 0x00:  return req.ro<uint32_t>(0x74726976); /* ____________________________________________ Magic Value: 'virt' */
            case 0x04:  return req.ro<uint32_t>(0x2); /* ___________________________________________________ Device version number: Virtio 1 - 1.1 */
            case 0x08:  return req.ro<uint32_t>(2); /* _____________________________________________________ Virtio Subsystem Device ID: block device */
            case 0x0c:  return req.ro(viodisk.Vendor()); /* ________________________________________________ Virtio Subsystem Vendor ID: 'usvp' */
            case 0x10:  return req.ro(viodisk.ClaimedFeatures()); /* _______________________________________ features supported by the device */
            case 0x14:  return req.wo(viodisk.DeviceFeaturesSel); /* _______________________________________ Device (host) features word selection */
            case 0x20:  return req.wo(tmp) and viodisk.UsedFeatures(tmp); /* _______________________________ features used by the driver  */
            case 0x24:  return req.wo(viodisk.DriverFeaturesSel); /* _______________________________________ Activated (guest) features word selection */
            case 0x30:  return req.wo(tmp) and viodisk.QueueSel(tmp); /* ___________________________________ Virtual queue index (only 1 queue := 0) */
            case 0x34:  return req.ro(viodisk.QueueNumMax()); /* ___________________________________________ Maximum virtual queue size */
            case 0x38:  return req.wo(viodisk.QueueNum()); /* ______________________________________________ Virtual queue size */
            case 0x44:  return req.rw(viodisk.QueueReady()) and (req.rd() or viodisk.QueueReady(vioa)); /* _ Virtual queue ready bit */
            case 0x50:  return req.wo(tmp) and viodisk.ReadQueue(vioa); /* _________________________________ Queue notifier */
            case 0x60:  return req.ro(viodisk.InterruptStatus); /* _________________________________________ Interrupt status */
            case 0x64:  return req.wo(tmp) and viodisk.InterruptAck(tmp); /* _______________________________ Interrupt acknowledgment */
            case 0x70:  return req.rw(viodisk.Status) and (req.rd() or viodisk.CheckStatus()); /* __________ Device status */
            case 0x80:
            case 0x84:  return req.wo(r64.sub32(viodisk.QueueDesc(), req.addr)); /* ________________________ Descriptor Area */
            case 0x90:
            case 0x94:  return req.wo(r64.sub32(viodisk.QueueDriver(), req.addr)); /* ______________________ Driver Area */
            case 0xa0:
            case 0xa4:  return req.wo(r64.sub32(viodisk.QueueDevice(), req.addr)); /* ______________________ Device Area */
            case 0xfc:  return req.ro(viodisk.ConfigGeneration); /* ________________________________________ ConfigGeneration */
            case 0x100:
            case 0x104: return req.rw(r64.sub32(viodisk.Capacity, req.addr)); /* ___________________________ Disk Capacity */
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
          if (req.addr & 3) { uint8_t dummy=0; return req.rw(dummy); }
          U8 chr = U8('\0');
          if (not req.write)                 { uart.rx_pop( arch, chr );  }
          if (not req.tainted_access( chr )) { return false; }
          if (    req.write)                 { uart.tx_push( arch, chr ); }
          return true;
        }

      if (req.size == 2)
        {
          switch (req.addr)
            {
            case 0x18: return req.ro(uart.flags()); /* _ Flag Register */
            case 0x24: return req.rw(uart.IBRD); /* ____ Integer Baud Rate Register */
            case 0x28: return req.rw(uart.FBRD); /* ____ Fractional Baud Rate Register */
            case 0x2c: return req.rw(uart.LCR); /* _____ Line Control Register */
            case 0x30: return req.rw(uart.CR); /* ______ Control Register */
            case 0x34: return req.rw(uart.IFLS); /* ____ Interrupt FIFO Level Select Register */
            case 0x38: return req.rw(uart.IMSC); /* ____ Interrupt Mask Set/Clear Register */
            case 0x3c: return req.ro(uart.RIS); /* _____ Raw Interrupt Status Register */
            case 0x40: return req.ro(uart.mis()); /* ___ Masked Interrupt Status Register */
            }
        }

      if (req.addr == 0x44)
        {
          if (not req.write) return error(req.dev, "Cannot read Interrupt Clear Register" );
          uint16_t icr = 0;
          if (not req.rw(icr)) return false;
          uart.RIS &= ~icr;
          return true;
        }

      if ((req.addr & -16) == 0xfe0)
        {
          if (req.write) return error(req.dev, "Cannot write PeriphID" );
          /* ARM PrimeCell (R) UART (PL011) Revision: r1p5 */
          uint32_t value(0x00341011);
          uint8_t byte = (req.addr & 3) ? uint8_t(0) : uint8_t(value >> 8*(req.addr >> 2 & 3));
          return req.rw(byte);
        }

      if ((req.addr & -16) == 0xff0)
        {
          if (req.write) return error(req.dev, "Cannot write PCellID" );

          uint8_t byte = (req.addr & 3) ? 0 : (0xB105F00D >> 8*(req.addr >> 2 & 3));
          return req.rw(byte);
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
AArch64::map_gic(uint64_t base_addr)
{
  static struct : public Device::Effect
  {
    virtual void get_name(unsigned id, std::ostream& sink) const override { sink << "Generic Interrupt Controller (GICv2)"; }

    virtual bool access(AArch64& arch, Device::Request& req) const override
    {
      uint32_t raz_wi_32(0);

      if (req.addr == 0x100) /* CPU Interface Control Register */
        return req.rw(arch.gic.C_CTLR);

      if (req.addr == 0x104) /* Interrupt Priority Mask Register */
        return req.rw(arch.gic.C_PMR);

      if (req.addr == 0x10c) /* Interrupt Acknowledge Register */
        {
          if (req.write) return error(req.dev, "Cannot write GICC_IAR" );

          unsigned int_id = arch.gic.HighestPriorityPendingInterrupt();
          if (int_id < arch.gic.ITLinesCount)
            arch.gic.ack_interrupt(int_id);

          U32 reg = U32(0)
            | arch.PartlyDefined<uint32_t>(0,0x7ffff) << 13  // Reserved
            | arch.PartlyDefined<uint32_t>(0,0x7)     << 10  // CPUID
            | U32(int_id)                             <<  0; // An product identifier (linux wants 0bx..x0000)

          return req.tainted_access(reg);
        }

      if (req.addr == 0x110) /* End of Interrupt Register */
        {
          if (not req.write) return error(req.dev, "Cannot read GICC_EOIR" );
          U32 reg;
          if (not req.tainted_access(reg))
            return false;

          struct GICLine {};
          arch.gic.eoi_interrupt( arch.untaint(GICLine(), reg & U32(0x3ff)) );
          return true;
        }

      if (0x1d0 <= req.addr and req.addr < 0x1f0)
        {
          if (req.addr & 3) return false;
          /* Active Priorities Registers (secure and not secure) */
          return req.rw(raz_wi_32);
        }

      if (req.addr == 0x1fc) /* Identification Register, GICC_IIDR */
        {
          if (req.write) return error(req.dev, "Cannot write GICC_IIDR" );
          U32 reg = U32(0)
            | arch.PartlyDefined<uint32_t>(0,0xff0) << 20  // An product identifier (linux wants 0bx..x0000)
            | U32(2)                                << 16  // Architecture version (GICv2)
            | arch.PartlyDefined<uint32_t>(0,0xf)   << 12  // Revision
            | U32(0x43b)                            <<  0; // The JEP106 identity and continuation code of ARM.

          return req.tainted_access(reg);
        }

      if (req.addr == 0x1000) /* Distributor Control Register */
        return req.rw( arch.gic.D_CTLR );

      if (req.addr == 0x1004) /* Interrupt Controller Type Register */
        {
          if (req.write) return error(req.dev, "Cannot write GICD_TYPER" );
          U32 reg = U32(0)
            | arch.PartlyDefined<uint32_t>(0,0xffff) << 16  // Reserved
            | arch.PartlyDefined<uint32_t>(0,0x1f)   << 11  // Maximum number of implemented lockable SPIs (Sec. Ext.)
            | arch.PartlyDefined<uint32_t>(0,1)      << 10  // Indicates whether the GIC implements the Security Extensions
            | arch.PartlyDefined<uint32_t>(0,3)      <<  8  // Reserved
            | U32(0)                                 <<  5  // Indicates the number of implemented CPU interfaces
            | U32(arch.gic.ITLinesNumber)            <<  0; // Indicates the maximum number of interrupts that the GIC supports
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
            return req.rw(raz_wi_32);
          auto iscreg = rtp == 0 ? &AArch64::GIC::D_IENABLE : rtp == 1 ? &AArch64::GIC::D_IPENDING : &AArch64::GIC::D_IACTIVE;
          uint32_t& reg = (arch.gic.*iscreg)[idx];
          uint32_t value = reg;
          if (not req.rw(value))
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
            return req.rw( arch.gic.D_IPRIORITYR[idx] );
          uint8_t raz_wi(0);
          return req.rw(raz_wi);
        }

      if (0x1800 <= req.addr and req.addr < 0x1c00)
        {
          /* The GICD_ITARGETSRs provide an 8-bit CPU targets. Note:
           * In a uniprocessor implementation, all interrupts target
           * the one processor, and the GICD_ITARGETSRs are RAZ/WI.
           */
          uint8_t raz_wi(0);
          return req.rw(raz_wi);
        }

      if (0x1c00 <= req.addr and req.addr < 0x1d00)
        {
          /* The GICD_ICFGRs provide a 2-bit Int_config field for each
           * interrupt supported by the GIC.
           */
          unsigned idx = (req.addr - 0x1c00)/4;
          if (16*idx < arch.gic.ITLinesCount)
            return req.rw( arch.gic.D_ICFGR[idx] );
          return req.rw(raz_wi_32);
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

  cpu.notify( delay * (cpu.get_freq() * cpu.get_ipc()) / get_cntfrq(), &AArch64::handle_vtimer );
}

bool
AArch64::has_irqs() const
{
  if (pstate.I)
    return false; /* IRQs masked */
  return gic.HasPendingInterrupt();
}

void
AArch64::PhysicalIRQException::proceed(AArch64& cpu) const
{
  // TakePhysicalIRQException
  cpu.TakeException(1, 0x80, cpu.current_insn_addr);
}

void
AArch64::handle_irqs()
{
  if (not has_irqs())
    return;

  throw PhysicalIRQException();
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

AArch64::RNG::RNG(char const* name, unisim::kernel::Object* parent)
  : unisim::kernel::Object(name, parent)
  , seed_x(SEED_X)
  , seed_y(SEED_Y)
  , seed_z(SEED_Z)
  , seed_c(SEED_C)
  , param_seed_x("seed-x", this, seed_x, "Seed for X of George Marsaglia's 64-bit KISS random number generator")
  , param_seed_y("seed-y", this, seed_y, "Seed for Y of George Marsaglia's 64-bit KISS random number generator")
  , param_seed_z("seed-z", this, seed_z, "Seed for Z of George Marsaglia's 64-bit KISS random number generator")
  , param_seed_c("seed-c", this, seed_c, "Seed for C of George Marsaglia's 64-bit KISS random number generator")
{
  for (auto param : { &param_seed_x, &param_seed_y, &param_seed_z, &param_seed_c })
    param->SetFormat(unisim::kernel::VariableBase::FMT_DEC);

  for( int self_test = 1; self_test >= 0; --self_test)
    {
      Seed(
        self_test ? SEED_X : seed_x,
        self_test ? SEED_Y : seed_y,
        self_test ? SEED_Z : seed_z,
        self_test ? SEED_C : seed_c
      );
      if (self_test)
        SelfTest();
    }
}

void AArch64::RNG::Seed(uint64_t _x, uint64_t _y, uint64_t _z, uint64_t _c)
{
  x = _x; y = _y; z = _z; c = _c; t = 0;
}

uint64_t AArch64::RNG::Get() { return t = KISS(); }

uint64_t AArch64::RNG::MWC() { return t = (x << 58) + c, c = (x >> 6), x += t, c += (x < t), x; }

uint64_t AArch64::RNG::XSH() { return y ^= (y << 13), y ^= (y >> 17), y ^= (y << 43); }

uint64_t AArch64::RNG::CNG() { return z = 6906969069LL * z + 1234567; }

uint64_t AArch64::RNG::KISS() { return MWC() + XSH() + CNG(); }

void AArch64::RNG::SelfTest()
{
  for(unsigned i = 0; i < 100000000; Get(), ++i);

  if (t != 1666297717051644203ULL)
    { struct Bad {}; throw Bad(); }
}

void AArch64::RNG::sync( SnapShot& snapshot )
{
  for (auto var : { &x, &y, &z, &c, &t })
    snapshot.sync(*var);
}

AArch64::U64
AArch64::RNDR()
{
  uint64_t rval = rng.Get();
  nzcv = U8(0);

  return U64(rval);
}

void
AArch64::before_fetch()
{
  if (unlikely(insn_counter == suspend_at))
    throw Suspend();

  /*** Handle debugging interface ***/
  if (unlikely(requires_fetch_instruction_reporting and memory_access_reporting_import and ((1 << pstate.GetEL()) & debug_el_mask)))
    memory_access_reporting_import->ReportFetchInstruction(this->current_insn_addr);

  // if (unlikely(trap_reporting_import and (trap_on_instruction_counter == instruction_counter)))
  //   trap_reporting_import->ReportTrap(*this,"Reached instruction counter");

  if (unlikely(debug_yielding_import and ((1 << pstate.GetEL()) & debug_el_mask)))
    debug_yielding_import->DebugYield();
}

void
AArch64::before_execute(Operation *op)
{
  last_insns[insn_counter % histsize].assign(this->current_insn_addr, this->insn_counter, op);

  if (unlikely(this->current_insn_addr == bdaddr))
    {
      breakdance();
      // disasm = true;
    }

  if (unlikely(disasm and ((1 << pstate.GetEL()) & debug_el_mask)))
    {
      // static std::ofstream dbgtrace("dbgtrace");
      std::ostream& sink( std::cerr );
      sink << "@" << std::hex << this->current_insn_addr << ": " << std::setfill('0') << std::setw(8) << op->GetEncoding() << "; ";
      DisasmState ds;
      op->disasm( ds, sink );
      sink << std::endl;
    }

  if (RECORDER)
    recorder.Begin(op);
}

void
AArch64::after_execute()
{
  if (unlikely(requires_commit_instruction_reporting and memory_access_reporting_import and ((1 << pstate.GetEL()) & debug_el_mask)))
    memory_access_reporting_import->ReportCommitInstruction(this->current_insn_addr, 4);
}

void
AArch64::before_except(Exception const& exception)
{
  if (RECORDER)
    recorder.Cancel();

  if (unlikely(enable_exception_trap and trap_reporting_import and ((1 << pstate.GetEL()) & debug_el_mask))) {
    std::ostringstream sstr;
    exception.print(sstr);
    sstr << " at PC=0x" << std::hex << current_insn_addr << ", instruction #" << insn_counter;
    trap_reporting_import->ReportTrap( *this, sstr.str() );
  }
}

template <bool HEAVY_EXEC_LOOP>
void
AArch64::exec_loop()
{
  for (;;)
    {
      insn_timer += 1;// + ((rng.Get() >> 16 & 3) == 3);

      /* Instruction boundary next_insn_addr becomes current_insn_addr */
      this->current_insn_addr = this->next_insn_addr;

      //  if (this->current_insn_addr == halt_on_addr) { Stop(0); return; }

      try
        {
          /*** Handle asynchronous events ***/
          while (unlikely(next_event <= insn_timer))
            {
              event_handler_t method = pop_next_event();
              if (not method) break;
              (this->*method)();
            }

          if (HEAVY_EXEC_LOOP)
            before_fetch();

          dbgmon.BeforeStep(*this);

          if (unlikely(this->current_insn_addr & 3))
              throw PCAlignmentException();

          /*** Fetch and decode instruction ***/
          Operation* op = ipb.access(*this, this->current_insn_addr);

          this->next_insn_addr += 4;

          if (HEAVY_EXEC_LOOP)
            before_execute(op);

          if (unlikely(terminate))
            {
              // force_throw();
              throw Stop();
            }

          dbgmon.BeforeExecute(*this, this->current_insn_addr);

          if (HEAVY_EXEC_LOOP)
            asm volatile( "arm64_heavy_exec_loop_operation_execute:" );
          else
            asm volatile( "arm64_operation_execute:" );

          /*** Execution instruction ***/
          op->execute( *this );

          this->insn_counter++; /* Instruction regularly finished */

          dbgmon.AfterStep(*this);

          if(HEAVY_EXEC_LOOP) after_execute();
        }

      catch (Exception const& exception)
        {
          /* Instruction aborted, proceed to next step */

          if (HEAVY_EXEC_LOOP)
            before_except(exception);

          exception.proceed(*this);
        }

      catch (Suspend const&)
        {
          std::cerr << "Suspending...\n";
          save_snapshot("uvp.shot");
          return;
        }

      catch (Stop const&)
        {
          std::cerr << "Stopping...\n";
          return;
        }
    }
}

void
AArch64::run()
{
  if((suspend_at != uint64_t(-1)) or
     (memory_access_reporting_import) or
     (debug_yielding_import) or
     (bdaddr != 0) or
     RECORDER)
    {
      // heavy execution loop
      exec_loop<true>();
    }
  else
    {
      // light execution loop
      exec_loop<false>();
    }
}

void
AArch64::show_exec_stats(std::ostream& sink)
{
  sink << std::dec
       << "Executed up to instruction #" << insn_counter << ".\n"
       << "Written " << tfp32count << " tainted 32-bits floating-point values to register.\n"
       << "Written " << tfp64count << " tainted 64-bits floating-point values to register.\n"
       << std::flush;
}

void
AArch64::write_tfpstats()
{
  if (tfpstats_filename.size() == 0)
    return;

  std::ofstream sink(tfpstats_filename.c_str());
  sink << "opsize,count\n";
  sink << "32," << tfp32count << '\n';
  sink << "64," << tfp64count << '\n';
  sink << "opsize,exp,count\n";
  for (auto const& kv : tfp32expcount)
    sink << "32," << kv.first << ',' << kv.second << '\n';
  for (auto const& kv : tfp64expcount)
    sink << "64," << kv.first << ',' << kv.second << '\n';
  sink << "insn,count\n";
  for (auto const& kv : tfpinsncount)
    sink << kv.first << ',' << kv.second << '\n';
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
  if (enable_sleep) usleep(0);
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

  uint64_t va = untaint(AddrTV(), addr);
  if (unlikely((size | va) & (size - 1)))
    throw DataAbort(unisim::component::cxx::processor::arm::DAbort::Alignment,
                    va, 0, mem_acc_type::read, /*level (don't care)*/ 0, /*ipavalid*/false, /*secondstage*/false, /*s2fs1walk*/false);
  MMU::TLB::Entry entry(va);
  translate_address(entry, pstate.GetEL(), mem_acc_type::read);
  // By design, access cannot cross mmu page boundaries
  excmon.set(entry.pa);
  dbgmon.NotifyLoadExclusive();
}

bool
AArch64::ExclusiveMonitorsPass( U64 addr, unsigned size )
{
  struct Bad {};

  uint64_t va = untaint(AddrTV(), addr);
  if (unlikely((size | va) & (size - 1)))
    throw DataAbort(unisim::component::cxx::processor::arm::DAbort::Alignment,
                    va, 0, mem_acc_type::write, /*level (don't care)*/ 0, /*ipavalid*/false, /*secondstage*/false, /*s2fs1walk*/false);
  MMU::TLB::Entry entry(va);
  translate_address(entry, pstate.GetEL(), mem_acc_type::write);
  // By design, access cannot cross mmu page boundaries
  bool passed = excmon.pass( entry.pa );
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
      if (pi == mru_pi)
        mru_pi = pages.end();
      pi = pages.erase(pi);
      if (pi == pages.end())
        { insert.if_not_empty(pages, std::move(above)); return; }
    }

  if (pi->last < base)
    { insert.if_not_empty(pages, std::move(above)); return; }

  Page below(pi->base, base - 1);
  std::copy(pi->data_beg(), pi->data_abs(base), below.data_beg());
  std::copy(pi->udat_beg(), pi->udat_abs(base), below.udat_beg());

  if (pi == mru_pi)
    mru_pi = pages.end();
  pi = pages.erase(pi);

  insert.if_not_empty(pages, std::move(above));
  pages.insert(pi, std::move(below));
}

// Function used for permission checking from AArch64 stage 1 translations
void
AArch64::CheckPermission(MMU::TLB::Entry const& trans, uint64_t vaddress, unsigned el, mem_acc_type::Code mat)
{
  // AArch64

  bool wxn = unisim::util::arithmetic::BitField<19,1>().Get(el1.SCTLR); // Cacheable

  unsigned ap = trans.ap | 2*(el != 0);

  bool perm_r = (ap & 2) == 2;
  bool perm_w = (ap & 6) == 2;
  bool perm_x = not ((perm_w and wxn) or (el ? trans.pxn or (trans.ap & 6) == 2 : trans.xn));

#if MMU_PERM_CHECKER
  {
    // AP[2:1]   Access from higher Exception level    Access from EL0
    //   00                    Read/write                     None
    //   01                    Read/write                  Read/write
    //   10                    Read-only                      None
    //   11                    Read-only                   Read-only

    unsigned ap = trans.ap;

    // R = (el != 0) or AP[1]
    bool r = (el != 0) or (ap & 2);
    // W = !AP[2] and ((el != 0) or AP[1])
    bool w = !(ap & 4) and ((el != 0) or (ap & 2));
    // !X = (W and WXN) or (PXN or ((AP[2:1]==0b01) if (el != 0) else XN))
    bool xn = (w and wxn) or (el ? (trans.pxn or ((ap & 6) == 2)) : trans.xn);
    assert(perm_r == r);
    assert(perm_w == w);
    assert(perm_x == !xn);
  }
#endif

  bool fail = true;
  switch (mat)
    {
    case mem_acc_type::read:               fail = not perm_r; break;
    case mem_acc_type::cache_maintenance:
    case mem_acc_type::write:              fail = not perm_w; break;
    case mem_acc_type::exec:               fail = not perm_x; break;
    case mem_acc_type::debug:              fail = false; break;
    default: { struct Bad {}; raise( Bad() ); }
    }

  if (unlikely(fail))
    throw DataAbort(unisim::component::cxx::processor::arm::DAbort::Permission,
                    vaddress, trans.pa, mat, trans.level, /*ipavalid*/true, /*secondstage*/false, /*s2fs1walk*/false);
}

void AArch64::CheckSPAlignment(U64 const& addr)
{
  if (unlikely(untaint(AddrTV(), addr) & 0xf))
    {
      bool stack_align_check = (pstate.GetEL() == 0) ? unisim::util::arithmetic::BitField<4,1>().Get(el1.SCTLR)  // SCTLR.SA0
                                                     : unisim::util::arithmetic::BitField<3,1>().Get(el1.SCTLR); // SCTLR.SA
      if (stack_align_check)
        throw SPAlignmentException();
    }
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

void
AArch64::UART::rx_pop(AArch64& arch, AArch64::U8& chr)
{
  if (rx_push())
    {
      chr = U8(rx_value);
      rx_valid = false;
      RIS &= ~RX_INT;
    }
  else
    chr = arch.PartlyDefined<uint8_t>(0,-1);
}

void
AArch64::UART::tx_push(AArch64& arch, U8 chr)
{
  tx_count += 1;
  char_io_import->PutChar(arch.untaint(SerialTV(), chr));
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
  unsigned idx = untaint(TVU8TV(), index), e = idx % elements, r = idx / elements;
  U8 res = (r < regs ? GetVU8((reg0+r)%32, e) : oob_value);
  if (index.ubits) res.ubits = -1;
  return res;
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
  snapshot.sync(excmon.valid);

  mmu.sync(snapshot);

  for (unsigned reg = 0; reg < 32; ++reg)
    tvsync(snapshot, gpr[reg]);

  for (unsigned reg = 0; reg < 32; ++reg)
    for (unsigned idx = 0; idx < 2; ++idx)
      tvsync(snapshot, (vector_views[reg].GetStorage(&vectors[reg], U64(), VUConfig::BYTECOUNT))[idx]);

  tvsync(snapshot, fpcr);
  tvsync(snapshot, fpsr);
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
  rng.sync(snapshot);

  dbgmon.sync(snapshot);
}

// std::ofstream&
// AArch64::ptlog()
// {
//   static std::ofstream sink("ptlog");
//   return sink;
// }

void
AArch64::RequiresMemoryAccessReporting( unisim::service::interfaces::MemoryAccessReportingType type, bool report )
{
  switch (type) {
  case unisim::service::interfaces::REPORT_MEM_ACCESS:  requires_memory_access_reporting = report; break;
  case unisim::service::interfaces::REPORT_FETCH_INSN:  requires_fetch_instruction_reporting = report; break;
  case unisim::service::interfaces::REPORT_COMMIT_INSN: requires_commit_instruction_reporting = report; break;
  default: { struct Bad {}; raise( Bad() ); }
  }
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
  MMU::TLB::Entry tlb_entry(addr);

  try { translate_address(tlb_entry, 1, AArch64::mem_acc_type::debug); }
  catch (DataAbort const& x)
    {
      buffer << "Could not read from memory";
      return buffer.str();
    }

  unsigned const insn_size = 4;
  uint8_t bytes[insn_size], ubuf[insn_size];
  if (get_page(tlb_entry.pa).read(tlb_entry.pa, &bytes[0], &ubuf[0], insn_size) != insn_size)
    {
      buffer << "Could not read from memory";
      return buffer.str();
    }

  uint32_t insn = 0;
  for (uint8_t *beg = &bytes[0], *itr = &beg[insn_size]; --itr >= beg;)
    insn = insn << 8 | *itr;

  Operation* op = decode(tlb_entry.pa, insn);

  buffer << std::hex << std::setfill('0') << std::setw(8) << op->GetEncoding() << std::dec << " ";
  DisasmState ds;
  op->disasm(ds, buffer);

  next_addr = addr + insn_size;

  if (not std::all_of( &ubuf[0], &ubuf[insn_size], [](unsigned char const byte) { return byte == 0; } ))
    buffer << " /* undefined bits in instruction */";

  return buffer.str();
}

