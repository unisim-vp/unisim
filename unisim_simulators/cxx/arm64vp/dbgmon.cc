/*
 *  Copyright (c) 2019-2024,
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
 * Authors: Gilles Mouchard (gilles.mouchard@cea.fr)
 *          Yves Lhuillier (yves.lhuillier@cea.fr)
 */

#include <architecture.hh>

AArch64::DebugMonitor::DebugMonitor()
  : ss(false)
  , bpe(0)
  , wpe(0)
  , ss_ldex(false)
  , stepping(false)
  , linked_ctx_bp_conds(0)
  , regs()
{
}

// Software-step machine states (see Table D2-17)
//   - inactive           = PSTATE.D or not MDSCR_EL1.SS
//   - active-not-pending = not PSTATE.D and MDSCR_EL1.SS and PSTATE.SS
//   - active-pending     = not PSTATE.D and MDSCR_EL1.SS and not PSTATE.SS

uint32_t AArch64::DebugMonitor::ExceptionReturnSPSR(unsigned from_el, bool pstate_d, uint32_t spsr)
{
  if (unlikely(ss))
    {
      // see table D2-19
      unsigned target_el = (spsr >> 2) & 3;
      bool spsr_d = (spsr >> 9) & 1;
      uint32_t const spsr_ss_mask = uint32_t(1) << 21;
      bool kde = LocalKernelDebugEnabled();
      bool clear_spsr_ss = false;
      if (from_el == 1)
        {
          clear_spsr_ss = ((target_el == 1) and (not kde or not pstate_d or (pstate_d and spsr_d))) or
                           ((target_el == 0) and kde and not pstate_d);
          stepping = (target_el == 1) and kde and not pstate_d; // conditionally step eret (see table D2-19, footnote a)
        }
      else if (from_el == 2)
        {
          clear_spsr_ss = (target_el == 2) or
                          ((target_el == 1) and (not kde or spsr_d));
          stepping = false; // do not step eret
        }
      else if (from_el == 3)
        {
          clear_spsr_ss = (target_el == 3) or
                          (target_el == 2) or
                          ((target_el == 1) and (not kde or spsr_d));
          stepping = false; // do not step eret
        }
      if (clear_spsr_ss)
        {
          if (DEBUG) std::cerr << "DebugMonitor: On exception return, SPSR_ELx.SS <- 0 before restoring PSTATE from SPSR_ELx" << std::endl;
          spsr &= ~spsr_ss_mask;
        }
      else if (DEBUG) std::cerr << "DebugMonitor: On exception return, SPSR_ELx.SS = " << ((spsr & spsr_ss_mask) != 0) << " before restoring PSTATE from SPSR_ELx" << std::endl;

      if (DEBUG and not stepping) std::cerr << "DebugMonitor: not stepping eret" << std::endl;
    }
  return spsr;
}

void AArch64::DebugMonitor::SetExceptionTakenPSTATE_SS(AArch64& cpu, uint64_t preferred_exception_return)
{
  // In active-not-pending state, set PSTATE.SS to 0 or 1 depending on the preferred exception return address (shall occur before PSTATE is saved into SPSR_ELx.SS)
  // Always leave PSTATE.SS as-is when in active-pending state.
  // Also do not alter PSTATE.SS if we're taking an asynchronous exception.
  if (unlikely(ss))
    {
      if (not cpu.pstate.D and cpu.pstate.SS and stepping) // active-not-pending state? stepping? (i.e. not an asynchronous exception)
        {
          if (DEBUG) std::cerr << "DebugMonitor: On taken exception, PSTATE.SS <- " << (cpu.current_insn_addr == preferred_exception_return) << " before saving PSTATE into SPSR_ELx" << std::endl;
          cpu.pstate.SS = (cpu.current_insn_addr == preferred_exception_return); // see table D2-21
        }
      else if (DEBUG) std::cerr << "DebugMonitor: On taken exception, PSTATE.SS = " << cpu.pstate.SS << " before saving PSTATE into SPSR_ELx" << std::endl;
    }
}

void AArch64::DebugMonitor::IntBeforeStep(AArch64& cpu)
{
  stepping = true;
  if (DEBUG) std::cerr << "DebugMonitor: before stepping at 0x" << std::hex << cpu.current_insn_addr << std::dec << ", in " << SoftwareStepStateName(cpu) << " state" << std::endl;
  bool _ss_ldex = ss_ldex;
  ss_ldex = false;
  if (not cpu.pstate.D and not cpu.pstate.SS) // active-pending state?
    {
      if (DEBUG) std::cerr << "DebugMonitor: software step exception at 0x" << std::hex << cpu.current_insn_addr << std::dec << std::endl;
      throw SoftwareStepException(_ss_ldex);
    }
}

void AArch64::DebugMonitor::IntAfterStep(AArch64& cpu)
{
  if (not cpu.pstate.D and cpu.pstate.SS and stepping) // active-not-pending state?
    {
      if (DEBUG) std::cerr << "DebugMonitor: step completed at 0x" << std::hex << cpu.current_insn_addr << std::dec << ", PSTATE.SS <- 0" << std::endl;
      cpu.pstate.SS = 0; // enter active-pending state

      DebugSoftwareStepStateMachine(cpu, std::cerr);
    }

  stepping = false;
}

bool AArch64::DebugMonitor::CheckBreakpointExecutionCondition(unsigned el, unsigned hmc, unsigned ssce, unsigned ssc, unsigned pmc, unsigned sec_state)
{
  struct Bad {};
  if ((sec_state != 0) or (el > 1)) throw Bad();
  // HMC SSCE SSC PMC Security States EL1 EL0
  //  1   0    00 01        NS         Y
  //  1   0    00 11        NS         Y   Y

  //  1   0    01 01        NS         Y
  //  1   0    01 11        NS         Y   Y

  //  1   0    11 01        NS         Y
  //  1   0    11 11        NS         Y   Y
  
  return not ssce and hmc and (ssc != 2) and ((pmc == 3) or ((el == 1) and (pmc == 1)));
}

void AArch64::DebugMonitor::IntBeforeExecute(AArch64& cpu, uint64_t vaddr)
{
  // Scan breakpoints in reverse order to visit linked context-aware breakpoints
  // before address match breakpoints that are linked to context-aware breakpoints
  uint64_t scan = bpe << (64 - BRPS);
  linked_ctx_bp_conds = 0;
  while (scan)
    {
      unsigned pos = unisim::util::arithmetic::BitScanReverse(scan);
      scan &= ~(uint64_t(1) << pos);
      unsigned bp_idx = 63 - pos;
      uint64_t& DBGBCR = regs.DBGBCR_EL1[bp_idx];
      unsigned hmc = (DBGBCR >> 13) & 1; // Higher mode control
      unsigned ssce = (DBGBCR >> 29) & 1; // Security State Control Extended
      unsigned ssc = (DBGBCR >> 14) & 0x3; // Security state control
      unsigned pmc = (DBGBCR >> 1) & 0x3; // Privilege mode control
      unsigned bt = (DBGBCR >> 20) & 0xf; // Breakpoint Type
      uint64_t& DBGBVR = regs.DBGBVR_EL1[bp_idx];
      unsigned lbn = (DBGBCR >> 16) & 0xf; // Linked Breakpoint Number
      bool address_match_breakpoint = (bt & 0xe) == 0;
      bool context_id_match_breakpoint = (bt & 0x2);
      bool address_mismatch_breakpoint = (bt & 0x4) and false; // ignored (RES0) in AArch64 state
      bool vmid_match_breakpoint = (bt & 0x8);
      bool linked = (bt & 1);
      
      if (address_match_breakpoint or address_mismatch_breakpoint)
        {
          // Unlinked/Linked instruction address match. DBGBVR<n>_EL1 is the address of an instruction
          
          if (not CheckBreakpointExecutionCondition(cpu.pstate.GetEL(), hmc, ssce, ssc, pmc, /* sec_state */ 0))
            continue;
          
          unsigned tgt_ctx_bp_idx = 0;
          if (linked)
            {
              if (lbn < (BRPS - CTX_CMPS))                  // Linked Breakpoint Number is out-of-range (only last breakpoints can be linked context-aware breakpoints)
                {
                  if (DEBUG)
                    std::cerr << "DebugMonitor: In breakpoint #" << bp_idx << ", linked Breakpoint Number (" << lbn << ") is out-of-range" << std::endl;

                  continue; // CONSTRAINED UNPREDICTABLE
                }
              if (((regs.DBGBCR_EL1[lbn] >> 20) & 0xe) != 2)  // linked breakpoint is not programmed as context-aware
                {
                  if (DEBUG)
                    std::cerr << "DebugMonitor: In breakpoint #" << bp_idx << ", linked breakpoint (" << lbn << ") is not programmed as context-aware" << std::endl;

                  continue; // CONSTRAINED UNPREDICTABLE
                }
              tgt_ctx_bp_idx = lbn + CTX_CMPS - BRPS;
            }
          
          uint64_t const VA_MASK = (uint64_t(1) << 49) & -uint64_t(4);
          bool match = ((DBGBVR ^ vaddr) & VA_MASK) == 0;
          bool except = ((address_match_breakpoint and match) or (address_mismatch_breakpoint and not match)) and
                        (not linked or ((linked_ctx_bp_conds >> tgt_ctx_bp_idx) & 1));
          if (except)
            {
              if (DEBUG)
                std::cerr << "DebugMonitor: Reached breakpoint #" << bp_idx << " (address match)" << std::endl;

              throw BreakpointException();
            }
        }
      else if (context_id_match_breakpoint or vmid_match_breakpoint)
        {
          if (bp_idx < (BRPS - CTX_CMPS))
            {
              if (DEBUG)
                std::cerr << "DebugMonitor: Breakpoint #" << bp_idx << " is wrongly programmed as context ID match breakpoint or a VMID match breakpoint" << std::endl;

              continue; // Only last breakpoints can be context-aware breakpoints
            }
          if (vmid_match_breakpoint)
            {
              if (DEBUG)
                std::cerr << "DebugMonitor: Breakpoint #" << bp_idx << " is programmed as VMID match breakpoint (unsupported, ignoring)" << std::endl;

              continue; // ignoring. FIXME: VMID match works with VTTBR_EL2 when secure states are implemented
            }
          
          // Unlinked/Linked context-aware breakpoints
          
          unsigned ctx_bp_idx = bp_idx + CTX_CMPS - BRPS;
          
          // Unlinked/Linked context-aware. DBGBVR<n>_EL1 is the context ID
          
          bool match = ((!context_id_match_breakpoint or ((uint32_t(DBGBVR) == uint32_t(regs.CONTEXTIDR_EL1)))) and
                        (!vmid_match_breakpoint or false)); // FIXME: should compare VMID with VTTBR_EL2
          
          if (match)
            {
              if (linked)
                linked_ctx_bp_conds |= 1 << ctx_bp_idx;
              else
                {
                  if (DEBUG)
                    std::cerr << "DebugMonitor: breakpoint #" << bp_idx << " (context-aware match)" << std::endl;

                  throw BreakpointException();
                }
            }
        }
    }
  while (scan);
}

bool AArch64::DebugMonitor::CheckWatchpointExecutionCondition(unsigned el, unsigned hmc, unsigned ssce, unsigned ssc, unsigned pac, unsigned sec_state)
{
  struct Bad {};
  if ((sec_state != 0) or (el > 1)) throw Bad();
  
  // HMC SSCE SSC PAC Security States EL1 EL0
  //  0   0    00 01        NS         Y   
  //  0   0    00 10        NS             Y
  //  0   0    00 11        NS         Y   Y
  
  //  0   0    01 01        NS         Y    
  //  0   0    01 10        NS             Y
  //  0   0    01 11        NS         Y   Y
  
  //  1   0    00 01        NS         Y   
  //  1   0    00 11        NS         Y   Y
  
  //  1   0    01 01        NS         Y   
  //  1   0    01 11        NS         Y   Y
  
  //  1   0    11 01        NS         Y   
  //  1   0    11 11        NS         Y   Y
  
  return not ssce and ((hmc and (ssc != 2) and ((pac == 3) or ((el == 1) and (pac == 1)))) or
                       (not hmc and not (ssc & 2) and ((pac == 3) or ((el == 1) and (pac == 1)) or ((el == 0) and (pac == 2)))));
}

void AArch64::DebugMonitor::IntMemAccess(unsigned el, uint64_t vaddr, uint64_t syndrome_vaddr, unsigned size, bool cache_maintenance, bool write)
{
  for(uint64_t scan = wpe; scan; scan = (scan - 1) & scan)
    {
      unsigned wp_idx = unisim::util::arithmetic::BitScanForward(scan);
      uint64_t& DBGWCR = regs.DBGWCR_EL1[wp_idx];
      
      unsigned lsc = (DBGWCR >> 3) & 3; // Load/Store Control
      if ( not (((!write * 0b01) | (write * 0b10)) & lsc) ) continue; // access type (load or store) mismatch
      
      unsigned hmc = (DBGWCR >> 13) & 1; // Higher mode control
      unsigned ssce = (DBGWCR >> 29) & 1; // Security State Control Extended
      unsigned ssc = (DBGWCR >> 14) & 0x3; // Security state control
      unsigned pac = (DBGWCR >> 1) & 0x3; // Privilege of access control
      
      if (not CheckWatchpointExecutionCondition(el, hmc, ssce, ssc, pac, /* sec_state */ 0))
        continue;
      
      unsigned wt = (DBGWCR >> 20) & 1; // Watchpoint Type
      bool linked = wt != 0;
      
      unsigned tgt_ctx_bp_idx = 0;
      if (linked)
        {
          unsigned lbn = (DBGWCR >> 16) & 0xf; // Linked Breakpoint Number
          if (lbn < (BRPS - CTX_CMPS))                  // Linked Breakpoint Number is out-of-range (only last breakpoints can be linked context-aware breakpoints)
            {
              if (DEBUG)
                std::cerr << "DebugMonitor: In watchpoint #" << wp_idx << ", linked Breakpoint Number (" << lbn << ") is out-of-range" << std::endl;

              continue; // CONSTRAINED UNPREDICTABLE
            }
          if (((regs.DBGBCR_EL1[lbn] >> 20) & 0xe) != 2)  // linked breakpoint is not programmed as context-aware
            {
              if (DEBUG)
                std::cerr << "DebugMonitor: In watchpoint #" << wp_idx << ", linked breakpoint (" << lbn << ") is not programmed as context-aware" << std::endl;

              continue; // CONSTRAINED UNPREDICTABLE
            }
          tgt_ctx_bp_idx = lbn + CTX_CMPS - BRPS;
        }

      unsigned mask = (DBGWCR >> 24) & 0x1f;
      if ((mask > 0) and (mask <= 2))
        {
          if (DEBUG)
            std::cerr << "DebugMonitor: In watchpoint #" << wp_idx << ", MASK contains a reserved value" << std::endl;

          continue; // MASK contains a reserved value
        }
          
      unsigned bas = (DBGWCR >> 12) & 0xff;
      if (mask > 0 && (bas != 0xff))
        {
          if (DEBUG)
            std::cerr << "DebugMonitor: In watchpoint #" << wp_idx << ", MASK is set to zero but BAS is not set to 0b11111111" << std::endl;

          continue; // if MASK is not zero then BAS must be 0b11111111
        }
      
      uint64_t& DBGWVR = regs.DBGWVR_EL1[wp_idx];
      bool word = (DBGWVR >> 2) & 1; // Word or doubleword?
      bas &= word ? 0xf : 0xff; // keep only meaningful bits of BAS
      unsigned lsb = bas & ~(bas - 1);
      unsigned msb = bas + lsb;
      if (msb & (msb - 1))
        {
          if (DEBUG)
            std::cerr << "DebugMonitor: In watchpoint #" << wp_idx << ", BAS contains a reserved value (not a contigous set of ones)" << std::endl;

          continue; // BAS contains a reserved value (not a contigous set of ones)
        }

      unsigned const top = 48;
      // Note:
      //  - in this implementation RESS bits are simply ignored even if RESS do not contains a copy of VA top bit
      
      uint64_t va_mask = (~uint64_t(0) << mask) & (~uint64_t(0) >> (63 - top));
      
      for (unsigned byte = 0; byte < size; ++byte)
        {
          uint64_t byte_vaddr = vaddr + byte;
          bool byte_select_match = ((bas >> (byte_vaddr & (word ? 3 : 7))) & 1) != 0;
          if (!byte_select_match) continue; // byte do not match
                    
          bool match = ((DBGWVR ^ byte_vaddr) & va_mask) == 0;
          bool except = match and
                        (not linked or ((linked_ctx_bp_conds >> tgt_ctx_bp_idx) & 1));

          if (except)
            {
              if (DEBUG)
                std::cerr << "DebugMonitor: Reached watchpoint #" << wp_idx << std::endl;

              throw WatchpointException(syndrome_vaddr, wp_idx, cache_maintenance, write);
            }
        }
    }
}

void AArch64::DebugMonitor::SetDBGBCR_EL1(unsigned idx, uint64_t value)
{
  regs.DBGBCR_EL1[idx] = value;
  uint64_t const bpe_mask = uint64_t(1) << idx;
  bpe &= ~bpe_mask; // clear bit
  if (MonitorDebugEvents()) // Monitor debug events?
    {
      bpe |= (value & 1) << idx; // mirror DBGBCR_EL1[idx].E
    }

  if (DEBUG)
    Status(std::cerr);
}

void AArch64::DebugMonitor::SetDBGWCR_EL1(unsigned idx, uint64_t value)
{
  regs.DBGWCR_EL1[idx] = value;
  uint64_t const wpe_mask = uint64_t(1) << idx;
  wpe &= ~wpe_mask; // clear bit
  if (MonitorDebugEvents()) // Monitor debug events?
    {
      wpe |= (value & 1) << idx; // mirror DBGWCR_EL1[idx].E
    }

  if (DEBUG)
    Status(std::cerr);
}

void AArch64::DebugMonitor::SetMDSCR_EL1(uint64_t value)
{
  regs.MDSCR_EL1 = value;

  if ((ss = SoftwareStepEnabled()))
    {
      ss_ldex = false;
    }

  if (MonitorDebugEvents()) // Monitor debug events?
    {
      for (unsigned idx = 0; idx < BRPS; ++idx)
        {
          uint64_t& DBGBCR = regs.DBGBCR_EL1[idx];
          if (DBGBCR & 1) // breakpoint enabled?
            bpe |= 1 << idx; // mirror DBGBCR_EL1[idx].E
        }
      for (unsigned idx = 0; idx < WRPS; ++idx)
        {
          uint64_t& DBGWCR = regs.DBGWCR_EL1[idx];
          if (DBGWCR & 1) // watchpoint enabled?
            wpe |= 1 << idx; // mirror DBGWCR_EL1[idx].E
        }
    }
  else
    {
      bpe = 0;
      wpe = 0;
    }
  
  if (DEBUG)
    Status(std::cerr);
}

void
AArch64::DebugMonitor::sync(SnapShot& snapshot)
{
  snapshot.sync(ss);
  snapshot.sync(bpe);
  snapshot.sync(wpe);
  snapshot.sync(ss_ldex);
  snapshot.sync(stepping);
  snapshot.sync(linked_ctx_bp_conds);
  snapshot.sync(regs.MDSCR_EL1);
  snapshot.sync(regs.CONTEXTIDR_EL1);
  for (unsigned idx = 0; idx < BRPS; ++idx)
    {
      snapshot.sync(regs.DBGBCR_EL1[idx]);
      snapshot.sync(regs.DBGBVR_EL1[idx]);
    }
  for (unsigned idx = 0; idx < WRPS; ++idx)
    {
      snapshot.sync(regs.DBGWCR_EL1[idx]);
      snapshot.sync(regs.DBGWVR_EL1[idx]);
    }
}

const char *AArch64::DebugMonitor::SoftwareStepStateName(AArch64 const& cpu) const
{
  return (cpu.pstate.D or not ss) ? "inactive"
                                  : ( cpu.pstate.SS ? "active-not-pending"
                                                    : "active-pending"     );
}

void AArch64::DebugMonitor::Status(std::ostream& sink)
{
  sink << "DebugMonitor: "
       << "ss_ldex=" << ss_ldex
       << ",bpe=" << bpe
       << ",wpe=" << wpe
       << ",linked_ctx_bp_conds=" << linked_ctx_bp_conds
       << std::endl;
  sink << "DebugMonitor: MDSCR_EL1=0x" << std::hex << regs.MDSCR_EL1 << std::dec << std::endl;
  sink << "DebugMonitor: CONTEXTIDR_EL1=0x" << std::hex << regs.CONTEXTIDR_EL1 << std::dec << std::endl;
  for (unsigned idx = 0; idx < BRPS; ++idx)
    {
      if ((bpe >> idx) & 1)
        {
          sink << "DebugMonitor: DBGBCR" << idx << "_EL1=0x" << std::hex << regs.DBGBCR_EL1[idx] << std::dec << std::endl;
          sink << "DebugMonitor: DBGBVR" << idx << "_EL1=0x" << std::hex << regs.DBGBVR_EL1[idx] << std::dec << std::endl;
        }
    }
  for (unsigned idx = 0; idx < WRPS; ++idx)
    {
      if ((wpe >> idx) & 1)
        {
          sink << "DebugMonitor: DBGWCR" << idx << "_EL1=0x" << std::hex << regs.DBGWCR_EL1[idx] << std::dec << std::endl;
          sink << "DebugMonitor: DBGWVR" << idx << "_EL1=0x" << std::hex << regs.DBGWVR_EL1[idx] << std::dec << std::endl;
        }
    }
}
