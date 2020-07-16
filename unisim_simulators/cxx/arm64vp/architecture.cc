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

AArch64::AArch64()
{
  std::cout << "Let's go!\n";
}

AArch64::Page::~Page()
{
  free->free(*this);
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
  if (below != pages.end() and not (*below < page))
    {
      error_mem_overlap(page, *below);
      return false;
    }
  if (pages.size() and below != pages.begin() and not (page < *std::prev(below)))
    {
      error_mem_overlap(page, *std::prev(below));
      return false;
    }
    
  pages.insert(below,std::move(page));
  return true;
}

bool
AArch64::mem_map(uint64_t addr, uint64_t size)
{
  static struct : Page::Free { void free (Page& page) const override { delete [] page.get_data(); } } free;
  
  return this->mem_map( Page(addr, size, new uint8_t[size], &free) );
}

AArch64::Page::Free AArch64::Page::Free::nop;

uint8_t*
AArch64::IPB::get(AArch64& core, uint64_t address)
{
  unsigned idx = address % LINE_SIZE;
  uint64_t req_base_address = address & -LINE_SIZE;

  if (base_address != req_base_address)
    {
      core.free_read_memory(req_base_address, &bytes[0], LINE_SIZE);
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
      
      /* Decode current PC */
      unisim::component::cxx::processor::arm::isa::arm64::Operation<AArch64>* op = decoder.Decode(insn_addr, insn);
    
      this->next_insn_addr += 4;
    
      // std::cerr << "@" << std::hex << insn_addr << ": " << std::hex << std::setfill('0') << std::setw(8) << insn << "; ";
      // op->disasm( *this, std::cerr );
      // std::cerr << std::endl;
    
      // uint64_t oldspval = GetGSR(31);
    
      /* Execute instruction */
      asm volatile( "arm64_operation_execute:" );
      op->execute( *this );
    
      // uint64_t newspval = GetGSR(31);
        
      // if (newspval != oldspval) {
      //   std::cerr << std::hex  << "  SP: 0x" << oldspval << " => 0x" << newspval << std::endl;
      // }
        
      // if (unlikely(requires_commit_instruction_reporting and memory_access_reporting_import))
      //   memory_access_reporting_import->ReportCommitInstruction(this->current_insn_addr, 4);
        
      //instruction_counter++; /* Instruction regularly finished */
      
  
  // catch (SVCException const& svexc) {
  //   /* Resuming execution, since SVC exceptions are explicitly
  //    * requested from regular instructions. ITState will be updated as
  //    * needed by TakeSVCException (as done in the ARM spec). */
  //   if (unlikely( requires_finished_instruction_reporting and memory_access_reporting_import ))
  //     memory_access_reporting_import->ReportCommitInstruction(this->current_insn_addr);

  //   instruction_counter++; /* Instruction regularly finished */
    
  //   this->TakeSVCException();
  // }
  
  // catch (DataAbortException const& daexc) {
  //   /* Abort execution, and take processor to data abort handler */
    
  //   if (unlikely(trap_reporting_import))
  //     trap_reporting_import->ReportTrap( *this, "Data Abort Exception" );
    
  //   this->TakeDataOrPrefetchAbortException(true); // TakeDataAbortException
  // }
  
  // catch (PrefetchAbortException const& paexc) {
  //   /* Abort execution, and take processor to prefetch abort handler */
    
  //   if (unlikely(trap_reporting_import))
  //     trap_reporting_import->ReportTrap( *this, "Prefetch Abort Exception" );
    
  //   this->TakeDataOrPrefetchAbortException(false); // TakePrefetchAbortException
  // }
  
  // catch (UndefInstrException const& undexc) {
  //   /* Abort execution, and take processor to undefined handler */
    
  //   if (unlikely( trap_reporting_import))
  //     trap_reporting_import->ReportTrap( *this, "Undefined Exception" );
    
  //   this->TakeUndefInstrException();
  // }
    }
  
  catch (std::exception const& exc)
    {
      std::cerr << "Unimplemented exception <" << exc.what() << "> @pc=" << std::hex << current_insn_addr << std::dec << std::endl;
      throw exc;
    }
}
