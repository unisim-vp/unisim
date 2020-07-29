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
AArch64::IPB::get(AArch64& core, uint64_t address)
{
  unsigned idx = address % LINE_SIZE;
  uint64_t req_base_address = address & -LINE_SIZE;
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

      /* Decode current PC */
      unisim::component::cxx::processor::arm::isa::arm64::Operation<AArch64>* op = decoder.Decode(insn_addr, insn);

      this->next_insn_addr += 4;

      std::cerr << "@" << std::hex << insn_addr << ": " << std::hex << std::setfill('0') << std::setw(8) << insn << "; ";
      op->disasm( *this, std::cerr );
      std::cerr << std::endl;

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

void
AArch64::UndefinedInstruction(unisim::component::cxx::processor::arm::isa::arm64::Operation<AArch64>* op)
{
  op->disasm(*this, std::cerr << "Undefined instruction : `"); std::cerr << "`.\n";
  UndefinedInstruction();
}

void
AArch64::UndefinedInstruction()
{
  struct Bad {}; throw Bad ();
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

/** Check access permissions for op1-n system registers
 *
 * @param op1     the "op1" field of the instruction code
 */
void
AArch64::CheckSystemAccess(uint8_t op1)
{

}


// /** Describe the nature of a CP15 coprocessor register
//  *
//  * @param crn     the "crn" field of the instruction code
//  * @param op1     the "op1" field of the instruction code
//  * @param crm     the "crm" field of the instruction code
//  * @param op2     the "op2" field of the instruction code
//  * @return        a C string describing the CP15 register
//  */
// void
// AArch64::DescribeSystemRegister( uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, std::ostream& sink )
// {
//   SysReg const& sysreg = GetSystemRegister( op0, op1, crn, crm, op2 );
//   if (char const* sysreg_desc = sysreg.Describe())
//     sink << sysreg_desc;
//   else
//     sink << "Unknown system register (op0=" << op0 << ", op1=" << op1 << ", crn=" << crn << ", crm=" << crm << ", op2=" << op2 << ")";
// }

// /** Name the nature of a CP15 coprocessor register
//  *
//  * @param crn     the "crn" field of the instruction code
//  * @param op1     the "op1" field of the instruction code
//  * @param crm     the "crm" field of the instruction code
//  * @param op2     the "op2" field of the instruction code
//  * @return        a C string describing the CP15 register
//  */
// void
// AArch64::NameSystemRegister( uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, std::ostream& sink )
// {
//   SysReg const&  sysreg = GetSystemRegister( op0, op1, crn, crm, op2 );

//   if (char const* sysreg_name = sysreg.Name())
//     sink << sysreg_name;
//   else
//     {
//       uint32_t sys_reg_idx = 0;
//       sys_reg_idx = (sys_reg_idx << 1) | op0;
//       sys_reg_idx = (sys_reg_idx << 3) | op1;
//       sys_reg_idx = (sys_reg_idx << 4) | crn;
//       sys_reg_idx = (sys_reg_idx << 4) | crm;
//       sys_reg_idx = (sys_reg_idx << 3) | op2;
//       sink << "#" << std::hex << sys_reg_idx << std::dec;
//   }
// }

#define SYSENCODE( OP0, OP1, CRN, CRM, OP2 ) ((OP0 << 16) | (OP1 << 12) | (CRN << 8) | (CRM << 4) | (OP2 << 0))

/** Get the Internal representation of the CP15 Register
 *
 * @param crn     the "crn" field of the instruction code
 * @param op1     the "op1" field of the instruction code
 * @param crm     the "crm" field of the instruction code
 * @param op2     the "op2" field of the instruction code
 * @return        an internal System Register
 */
AArch64::SysReg const*
AArch64::GetSystemRegister( uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2 )
{
  struct MovedSysReg : public SysReg
  {
    virtual char const* Name() const = 0;
    virtual char const* Describe() const = 0;
    virtual char const* ReadOperation() const { return "mrs"; }
    virtual char const* WriteOperation() const { return "msr"; }
    virtual void DisasmRead(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, std::ostream& sink) const override
    {
      sink << ReadOperation() << '\t' << unisim::component::cxx::processor::arm::isa::arm64::DisasmGZXR(rt) << ", " << Name() << "\t; " << Describe();
    }
    virtual void DisasmWrite(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, std::ostream& sink) const override
    {
      sink << WriteOperation() << '\t' << Name() << ", " << unisim::component::cxx::processor::arm::isa::arm64::DisasmGZXR(rt) << "\t; " << Describe();
    }
    virtual void Write(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, AArch64& cpu, U64 value) const override
    {
      std::cerr << "Unimplemented. Cannot write " << Describe();
      cpu.UndefinedInstruction();
    }
    virtual U64 Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2,  AArch64& cpu) const override
    {
      std::cerr << "Unimplemented. Cannot read " << Describe();
      cpu.UndefinedInstruction();
      return U64();
    }
  };

  struct DCSysReg : public MovedSysReg
  {
    virtual char const* ReadOperation() const override { return "dc<bad-read>"; }
    virtual char const* WriteOperation() const override { return "dc"; }
  };
  
  struct PStateSysReg : public SysReg
  {
    virtual char const* Name() const = 0;
    void DisasmRead(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, std::ostream& sink) const override
    { sink << "msr\t<read-error>, " << Name(); }
    void DisasmWrite(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, std::ostream& sink) const override
    {
      sink << "msr\t" << Name() << ", " << unisim::component::cxx::processor::arm::isa::arm64::DisasmI(crm);
    }
    virtual U64 Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2,  AArch64& cpu) const override
    {
      DisasmRead(op0, op1, crn, crm, op2, 0b11111, std::cerr); std::cerr << std::endl;
      cpu.UndefinedInstruction();
      return U64();
    }
  };
  
  struct BarrierSysReg : public SysReg
  {
    virtual char const* Operation() const = 0;
    virtual void DisasmRead(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, std::ostream& sink) const override
    { sink << Operation() << "\t<read-error>"; }
    virtual void DisasmWrite(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, std::ostream& sink) const override
    { sink << Operation() << '\t' << unisim::component::cxx::processor::arm::isa::arm64::DisasmBarrierOption(crm); }
    virtual U64 Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2,  AArch64& cpu) const override
    {
      DisasmRead(op0, op1, crn, crm, op2, 0b11111, std::cerr); std::cerr << std::endl;
      cpu.UndefinedInstruction();
      return U64();
    }
    virtual void Write(uint8_t, uint8_t, uint8_t, uint8_t crm, uint8_t, AArch64& cpu, U64) const override
    { /* No OoO nor speculative execution, nothing to do */ }
  };

  switch (SYSENCODE( op0, op1, crn, crm, op2 ))
    {
      /*** Data Cache Maintenance ***/
    case SYSENCODE(0b01,0b011,0b0111,0b0100,0b001):
      {
        static struct : public DCSysReg {
          char const* Name() const override { return "zva"; }
          char const* Describe() const override { return "Zero data cache by address"; }
          void Write(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, AArch64& cpu, U64 addr) const override
          {
            if (addr.ubits) { struct Bad {}; throw Bad (); }
            unsigned const size = 64;
            uint8_t buffer[size] = {0};
            if (cpu.modify_page(addr.value).write(addr.value & -size, &buffer[0], &buffer[0], size) != size)
              { struct Bad {}; throw Bad (); }
          }
        } x; return &x;
      } break;
    case SYSENCODE(0b01,0b000,0b0111,0b1110,0b010):
      {
        static struct : public DCSysReg {
          char const* Name() const override { return "cisw"; }
          char const* Describe() const override { return "Clean and Invalidate data cache by set/way"; }
        } x; return &x;
      } break;
    case SYSENCODE(0b01,0b000,0b0111,0b1010,0b010):
      {
        static struct : public DCSysReg {
          char const* Name() const override { return "csw"; }
          char const* Describe() const override { return "Clean data cache by set/way"; }
        } x; return &x;
      } break;
    case SYSENCODE(0b01,0b011,0b0111,0b1010,0b001):
      {
        static struct : public DCSysReg {
          char const* Name() const override { return "cvac"; }
          char const* Describe() const override { return "Clean data cache by address to Point of Coherency"; }
        } x; return &x;
      } break;
    case SYSENCODE(0b01,0b011,0b0111,0b1011,0b001):
      {
        static struct : public DCSysReg {
          char const* Name() const override { return "cvau"; }
          char const* Describe() const override { return "Clean data cache by address to Point of Unification"; }
        } x; return &x;
      } break;
    case SYSENCODE(0b01,0b000,0b0111,0b0110,0b010):
      {
        static struct : public DCSysReg {
          char const* Name() const override { return "isw"; }
          char const* Describe() const override { return "Invalidate data cache by set/way"; }
        } x; return &x;
      } break;
    case SYSENCODE(0b01,0b000,0b0111,0b0110,0b001):
      {
        static struct : public DCSysReg {
          char const* Name() const override { return "ivac"; }
          char const* Describe() const override { return "Invalidate instruction cache by address to Point of Coherency"; }
          void Write(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, AArch64& cpu, U64 addr) const override
          {
            if (addr.ubits) { struct Bad {}; throw Bad (); }
            // No caches, so normally nothing to do...
          }
        } x; return &x;
      } break;

    case SYSENCODE(0b01,0b011,0b0111,0b1110,0b001):
      {
        static struct : public DCSysReg {
          char const* Name() const override { return "civac"; }
          char const* Describe() const override { return "Clean and Invalidate data cache by address to Point of Coherency"; }
          void Write(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, AArch64& cpu, U64 addr) const override
          {
            if (addr.ubits) { struct Bad {}; throw Bad (); }
            // No caches, so normally nothing to do...
          }
        } x; return &x;
      } break;
      
    case SYSENCODE(0b11,0b000,0b0001,0b0000,0b001): // 2.1: ACTLR_EL1, Auxiliary Control Register (EL1)
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "ACTLR_EL1"; }
          char const* Describe() const { return "Auxiliary Control Register (EL1)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b0001,0b0000,0b001): // 2.2: ACTLR_EL2, Auxiliary Control Register (EL2)
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "ACTLR_EL2"; }
          char const* Describe() const { return "Auxiliary Control Register (EL2)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b110,0b0001,0b0000,0b001): // 2.3: ACTLR_EL3, Auxiliary Control Register (EL3)
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "ACTLR_EL3"; }
          char const* Describe() const { return "Auxiliary Control Register (EL3)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0101,0b0001,0b000): // 2.4: AFSR0_EL1, Auxiliary Fault Status Register 0 (EL1)
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "AFSR0_EL1"; }
          char const* Describe() const { return "Auxiliary Fault Status Register 0 (EL1)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b0101,0b0001,0b000): // 2.5: AFSR0_EL2, Auxiliary Fault Status Register 0 (EL2)
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "AFSR0_EL2"; }
          char const* Describe() const { return "Auxiliary Fault Status Register 0 (EL2)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b110,0b0101,0b0001,0b000): // 2.6: AFSR0_EL3, Auxiliary Fault Status Register 0 (EL3)
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "AFSR0_EL3"; }
          char const* Describe() const { return "Auxiliary Fault Status Register 0 (EL3)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0101,0b0001,0b001): // 2.7: AFSR1_EL1, Auxiliary Fault Status Register 1 (EL1)
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "AFSR1_EL1"; }
          char const* Describe() const { return "Auxiliary Fault Status Register 1 (EL1)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b0101,0b0001,0b001): // 2.8: AFSR1_EL2, Auxiliary Fault Status Register 1 (EL2)
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "AFSR1_EL2"; }
          char const* Describe() const { return "Auxiliary Fault Status Register 1 (EL2)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b110,0b0101,0b0001,0b001): // 2.9: AFSR1_EL3, Auxiliary Fault Status Register 1 (EL3)
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "AFSR1_EL3"; }
          char const* Describe() const { return "Auxiliary Fault Status Register 1 (EL3)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b001,0b0000,0b0000,0b111): // 2.10: AIDR_EL1, Auxiliary ID Register
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "AIDR_EL1"; }
          char const* Describe() const { return "Auxiliary ID Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1010,0b0011,0b000): // 2.11: AMAIR_EL1, Auxiliary Memory Attribute Indirection Register (EL1)
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "AMAIR_EL1"; }
          char const* Describe() const { return "Auxiliary Memory Attribute Indirection Register (EL1)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1010,0b0011,0b000): // 2.12: AMAIR_EL2, Auxiliary Memory Attribute Indirection Register (EL2)
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "AMAIR_EL2"; }
          char const* Describe() const { return "Auxiliary Memory Attribute Indirection Register (EL2)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b110,0b1010,0b0011,0b000): // 2.13: AMAIR_EL3, Auxiliary Memory Attribute Indirection Register (EL3)
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "AMAIR_EL3"; }
          char const* Describe() const { return "Auxiliary Memory Attribute Indirection Register (EL3)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b001,0b0000,0b0000,0b000): // 2.14: CCSIDR_EL1, Current Cache Size ID Register
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "CCSIDR_EL1"; }
          char const* Describe() const { return "Current Cache Size ID Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b001,0b0000,0b0000,0b001): // 2.15: CLIDR_EL1, Cache Level ID Register
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "CLIDR_EL1"; }
          char const* Describe() const { return "Cache Level ID Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1101,0b0000,0b001): // 2.16: CONTEXTIDR_EL1, Context ID Register
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "CONTEXTIDR_EL1"; }
          char const* Describe() const { return "Context ID Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0001,0b0000,0b010): // 2.17: CPACR_EL1, Architectural Feature Access Control Register
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "CPACR_EL1"; }
          char const* Describe() const { return "Architectural Feature Access Control Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b0001,0b0001,0b010): // 2.18: CPTR_EL2, Architectural Feature Trap Register (EL2)
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "CPTR_EL2"; }
          char const* Describe() const { return "Architectural Feature Trap Register (EL2)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b110,0b0001,0b0001,0b010): // 2.19: CPTR_EL3, Architectural Feature Trap Register (EL3)
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "CPTR_EL3"; }
          char const* Describe() const { return "Architectural Feature Trap Register (EL3)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b010,0b0000,0b0000,0b000): // 2.20: CSSELR_EL1, Cache Size Selection Register
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "CSSELR_EL1"; }
          char const* Describe() const { return "Cache Size Selection Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b0000,0b0000,0b001): // 2.21: CTR_EL0, Cache Type Register
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "CTR_EL0"; }
          char const* Describe() const { return "Cache Type Register"; }
          U64 Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, AArch64& cpu) const override
          {
            return
              U64(0,0b1)          << 31 | // RES1, Reserved.
              U64(0,0b111)        << 28 | // RES0, Reserved.
              U64(0,0b1111)       << 24 | // CWG, Cache Writeback Granule.
              U64(0,0b1111)       << 20 | // ERG, Exclusives Reservation Granule.
              U64(0b0100)         << 16 | // DminLine (16 words)
              U64(0,0b11)         << 14 | // L1Ip, Level 1 instruction cache policy.
              U64(0,0b1111111111) <<  4 | // RES0, Reserved.
              U64(0b0011)         <<  0;  // IminLine,  (8 words)
            return U64((0b100 << 29) | (3 << 24) | (3 << 20) | (3 << 16) | (0b11 << 14) | (3 <<  0)
                       );
          }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b0011,0b0000,0b000): // 2.22: DACR32_EL2, Domain Access Control Register
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "DACR32_EL2"; }
          char const* Describe() const { return "Domain Access Control Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0101,0b0010,0b000): // 2.24: ESR_EL1, Exception Syndrome Register (EL1)
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "ESR_EL1"; }
          char const* Describe() const { return "Exception Syndrome Register (EL1)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b0101,0b0010,0b000): // 2.25: ESR_EL2, Exception Syndrome Register (EL2)
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "ESR_EL2"; }
          char const* Describe() const { return "Exception Syndrome Register (EL2)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b110,0b0101,0b0010,0b000): // 2.26: ESR_EL3, Exception Syndrome Register (EL3)
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "ESR_EL3"; }
          char const* Describe() const { return "Exception Syndrome Register (EL3)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0110,0b0000,0b000): // 2.27: FAR_EL1, Fault Address Register (EL1)
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "FAR_EL1"; }
          char const* Describe() const { return "Fault Address Register (EL1)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b0110,0b0000,0b000): // 2.28: FAR_EL2, Fault Address Register (EL2)
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "FAR_EL2"; }
          char const* Describe() const { return "Fault Address Register (EL2)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b110,0b0110,0b0000,0b000): // 2.29: FAR_EL3, Fault Address Register (EL3)
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "FAR_EL3"; }
          char const* Describe() const { return "Fault Address Register (EL3)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b0101,0b0011,0b000): // 2.30: FPEXC32_EL2, Floating-point Exception Control register
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "FPEXC32_EL2"; }
          char const* Describe() const { return "Floating-point Exception Control register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b0001,0b0001,0b111): // 2.31: HACR_EL2, Hypervisor Auxiliary Control Register
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "HACR_EL2"; }
          char const* Describe() const { return "Hypervisor Auxiliary Control Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b0001,0b0001,0b000): // 2.32: HCR_EL2, Hypervisor Configuration Register
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "HCR_EL2"; }
          char const* Describe() const { return "Hypervisor Configuration Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b0110,0b0000,0b100): // 2.33: HPFAR_EL2, Hypervisor IPA Fault Address Register
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "HPFAR_EL2"; }
          char const* Describe() const { return "Hypervisor IPA Fault Address Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b0001,0b0001,0b011): // 2.34: HSTR_EL2, Hypervisor System Trap Register
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "HSTR_EL2"; }
          char const* Describe() const { return "Hypervisor System Trap Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0101,0b100): // 2.35: ID_AA64AFR0_EL1, AArch64 Auxiliary Feature Register 0
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "ID_AA64AFR0_EL1"; }
          char const* Describe() const { return "AArch64 Auxiliary Feature Register 0"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0101,0b101): // 2.36: ID_AA64AFR1_EL1, AArch64 Auxiliary Feature Register 1
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "ID_AA64AFR1_EL1"; }
          char const* Describe() const { return "AArch64 Auxiliary Feature Register 1"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0101,0b000): // 2.37: ID_AA64DFR0_EL1, AArch64 Debug Feature Register 0
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "ID_AA64DFR0_EL1"; }
          char const* Describe() const { return "AArch64 Debug Feature Register 0"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0101,0b001): // 2.38: ID_AA64DFR1_EL1, AArch64 Debug Feature Register 1
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "ID_AA64DFR1_EL1"; }
          char const* Describe() const { return "AArch64 Debug Feature Register 1"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0110,0b000): // 2.39: ID_AA64ISAR0_EL1, AArch64 Instruction Set Attribute Register 0
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "ID_AA64ISAR0_EL1"; }
          char const* Describe() const { return "AArch64 Instruction Set Attribute Register 0"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0110,0b001): // 2.40: ID_AA64ISAR1_EL1, AArch64 Instruction Set Attribute Register 1
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "ID_AA64ISAR1_EL1"; }
          char const* Describe() const { return "AArch64 Instruction Set Attribute Register 1"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0111,0b000): // 2.41: ID_AA64MMFR0_EL1, AArch64 Memory Model Feature Register 0
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "ID_AA64MMFR0_EL1"; }
          char const* Describe() const { return "AArch64 Memory Model Feature Register 0"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0111,0b001): // 2.42: ID_AA64MMFR1_EL1, AArch64 Memory Model Feature Register 1
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "ID_AA64MMFR1_EL1"; }
          char const* Describe() const { return "AArch64 Memory Model Feature Register 1"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0100,0b000): // 2.43: ID_AA64PFR0_EL1, AArch64 Processor Feature Register 0
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "ID_AA64PFR0_EL1"; }
          char const* Describe() const { return "AArch64 Processor Feature Register 0"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0100,0b001): // 2.44: ID_AA64PFR1_EL1, AArch64 Processor Feature Register 1
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "ID_AA64PFR1_EL1"; }
          char const* Describe() const { return "AArch64 Processor Feature Register 1"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0001,0b011): // 2.45: ID_AFR0_EL1, AArch32 Auxiliary Feature Register 0
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "ID_AFR0_EL1"; }
          char const* Describe() const { return "AArch32 Auxiliary Feature Register 0"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0001,0b010): // 2.46: ID_DFR0_EL1, AArch32 Debug Feature Register 0
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "ID_DFR0_EL1"; }
          char const* Describe() const { return "AArch32 Debug Feature Register 0"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0010,0b000): // 2.47: ID_ISAR0_EL1, AArch32 Instruction Set Attribute Register 0
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "ID_ISAR0_EL1"; }
          char const* Describe() const { return "AArch32 Instruction Set Attribute Register 0"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0010,0b001): // 2.48: ID_ISAR1_EL1, AArch32 Instruction Set Attribute Register 1
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "ID_ISAR1_EL1"; }
          char const* Describe() const { return "AArch32 Instruction Set Attribute Register 1"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0010,0b010): // 2.49: ID_ISAR2_EL1, AArch32 Instruction Set Attribute Register 2
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "ID_ISAR2_EL1"; }
          char const* Describe() const { return "AArch32 Instruction Set Attribute Register 2"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0010,0b011): // 2.50: ID_ISAR3_EL1, AArch32 Instruction Set Attribute Register 3
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "ID_ISAR3_EL1"; }
          char const* Describe() const { return "AArch32 Instruction Set Attribute Register 3"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0010,0b100): // 2.51: ID_ISAR4_EL1, AArch32 Instruction Set Attribute Register 4
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "ID_ISAR4_EL1"; }
          char const* Describe() const { return "AArch32 Instruction Set Attribute Register 4"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0010,0b101): // 2.52: ID_ISAR5_EL1, AArch32 Instruction Set Attribute Register 5
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "ID_ISAR5_EL1"; }
          char const* Describe() const { return "AArch32 Instruction Set Attribute Register 5"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0001,0b100): // 2.53: ID_MMFR0_EL1, AArch32 Memory Model Feature Register 0
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "ID_MMFR0_EL1"; }
          char const* Describe() const { return "AArch32 Memory Model Feature Register 0"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0001,0b101): // 2.54: ID_MMFR1_EL1, AArch32 Memory Model Feature Register 1
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "ID_MMFR1_EL1"; }
          char const* Describe() const { return "AArch32 Memory Model Feature Register 1"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0001,0b110): // 2.55: ID_MMFR2_EL1, AArch32 Memory Model Feature Register 2
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "ID_MMFR2_EL1"; }
          char const* Describe() const { return "AArch32 Memory Model Feature Register 2"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0001,0b111): // 2.56: ID_MMFR3_EL1, AArch32 Memory Model Feature Register 3
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "ID_MMFR3_EL1"; }
          char const* Describe() const { return "AArch32 Memory Model Feature Register 3"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0001,0b000): // 2.57: ID_PFR0_EL1, AArch32 Processor Feature Register 0
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "ID_PFR0_EL1"; }
          char const* Describe() const { return "AArch32 Processor Feature Register 0"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0001,0b001): // 2.58: ID_PFR1_EL1, AArch32 Processor Feature Register 1
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "ID_PFR1_EL1"; }
          char const* Describe() const { return "AArch32 Processor Feature Register 1"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b0101,0b0000,0b001): // 2.59: IFSR32_EL2, Instruction Fault Status Register (EL2)
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "IFSR32_EL2"; }
          char const* Describe() const { return "Instruction Fault Status Register (EL2)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b0001,0b000): // 2.60: ISR_EL1, Interrupt Status Register
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "ISR_EL1"; }
          char const* Describe() const { return "Interrupt Status Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1010,0b0010,0b000): // 2.61: MAIR_EL1, Memory Attribute Indirection Register (EL1)
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "MAIR_EL1"; }
          char const* Describe() const { return "Memory Attribute Indirection Register (EL1)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1010,0b0010,0b000): // 2.62: MAIR_EL2, Memory Attribute Indirection Register (EL2)
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "MAIR_EL2"; }
          char const* Describe() const { return "Memory Attribute Indirection Register (EL2)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b110,0b1010,0b0010,0b000): // 2.63: MAIR_EL3, Memory Attribute Indirection Register (EL3)
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "MAIR_EL3"; }
          char const* Describe() const { return "Memory Attribute Indirection Register (EL3)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0000,0b000): // 2.64: MIDR_EL1, Main ID Register
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "MIDR_EL1"; }
          char const* Describe() const { return "Main ID Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0000,0b101): // 2.65: MPIDR_EL1, Multiprocessor A0nity Register
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "MPIDR_EL1"; }
          char const* Describe() const { return "Multiprocessor A0nity Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0011,0b000): // 2.66: MVFR0_EL1, Media and VFP Feature Register 0
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "MVFR0_EL1"; }
          char const* Describe() const { return "Media and VFP Feature Register 0"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0011,0b001): // 2.67: MVFR1_EL1, Media and VFP Feature Register 1
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "MVFR1_EL1"; }
          char const* Describe() const { return "Media and VFP Feature Register 1"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0011,0b010): // 2.68: MVFR2_EL1, Media and VFP Feature Register 2
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "MVFR2_EL1"; }
          char const* Describe() const { return "Media and VFP Feature Register 2"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0111,0b0100,0b000): // 2.69: PAR_EL1, Physical Address Register
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "PAR_EL1"; }
          char const* Describe() const { return "Physical Address Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0000,0b110): // 2.70: REVIDR_EL1, Revision ID Register
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "REVIDR_EL1"; }
          char const* Describe() const { return "Revision ID Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b0000,0b010): // 2.71: RMR_EL1, Reset Management Register (if EL2 and EL3 not implemented)
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "RMR_EL1"; }
          char const* Describe() const { return "Reset Management Register (if EL2 and EL3 not implemented)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1100,0b0000,0b010): // 2.72: RMR_EL2, Reset Management Register (if EL3 not implemented)
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "RMR_EL2"; }
          char const* Describe() const { return "Reset Management Register (if EL3 not implemented)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b110,0b1100,0b0000,0b010): // 2.73: RMR_EL3, Reset Management Register (if EL3 implemented)
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "RMR_EL3"; }
          char const* Describe() const { return "Reset Management Register (if EL3 implemented)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b0000,0b001): // 2.74: RVBAR_EL1, Reset Vector Base Address Register (if EL2 and EL3 not implemented)
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "RVBAR_EL1"; }
          char const* Describe() const { return "Reset Vector Base Address Register (if EL2 and EL3 not implemented)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1100,0b0000,0b001): // 2.75: RVBAR_EL2, Reset Vector Base Address Register (if EL3 not implemented)
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "RVBAR_EL2"; }
          char const* Describe() const { return "Reset Vector Base Address Register (if EL3 not implemented)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b110,0b1100,0b0000,0b001): // 2.76: RVBAR_EL3, Reset Vector Base Address Register (if EL3 implemented)
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "RVBAR_EL3"; }
          char const* Describe() const { return "Reset Vector Base Address Register (if EL3 implemented)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b110,0b0001,0b0001,0b000): // 2.78: SCR_EL3, Secure Configuration Register
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "SCR_EL3"; }
          char const* Describe() const { return "Secure Configuration Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0001,0b0000,0b000): // 2.79: SCTLR_EL1, System Control Register (EL1)
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "SCTLR_EL1"; }
          char const* Describe() const { return "System Control Register (EL1)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b0001,0b0000,0b000): // 2.80: SCTLR_EL2, System Control Register (EL2)
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "SCTLR_EL2"; }
          char const* Describe() const { return "System Control Register (EL2)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b110,0b0001,0b0000,0b000): // 2.81: SCTLR_EL3, System Control Register (EL3)
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "SCTLR_EL3"; }
          char const* Describe() const { return "System Control Register (EL3)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0010,0b0000,0b010): // 2.82: TCR_EL1, Translation Control Register (EL1)
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "TCR_EL1"; }
          char const* Describe() const { return "Translation Control Register (EL1)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b0010,0b0000,0b010): // 2.83: TCR_EL2, Translation Control Register (EL2)
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "TCR_EL2"; }
          char const* Describe() const { return "Translation Control Register (EL2)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b110,0b0010,0b0000,0b010): // 2.84: TCR_EL3, Translation Control Register (EL3)
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "TCR_EL3"; }
          char const* Describe() const { return "Translation Control Register (EL3)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b010,0b0000,0b0000,0b000): // 2.85: TEECR32_EL1, T32EE Configuration Register
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "TEECR32_EL1"; }
          char const* Describe() const { return "T32EE Configuration Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b010,0b0001,0b0000,0b000): // 2.86: TEEHBR32_EL1, T32EE Handler Base Register
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "TEEHBR32_EL1"; }
          char const* Describe() const { return "T32EE Handler Base Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1101,0b0000,0b010): // 2.87: TPIDR_EL0, Thread Pointer / ID Register (EL0)
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "TPIDR_EL0"; }
          char const* Describe() const { return "Thread Pointer / ID Register (EL0)"; }
          U64 Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, AArch64& cpu) const override
          { return cpu.TPIDRURW; }
          void Write(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, AArch64& cpu, U64 value) const override
          { cpu.TPIDRURW = value; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1101,0b0000,0b100): // 2.88: TPIDR_EL1, Thread Pointer / ID Register (EL1)
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "TPIDR_EL1"; }
          char const* Describe() const { return "Thread Pointer / ID Register (EL1)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1101,0b0000,0b010): // 2.89: TPIDR_EL2, Thread Pointer / ID Register (EL2)
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "TPIDR_EL2"; }
          char const* Describe() const { return "Thread Pointer / ID Register (EL2)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b110,0b1101,0b0000,0b010): // 2.90: TPIDR_EL3, Thread Pointer / ID Register (EL3)
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "TPIDR_EL3"; }
          char const* Describe() const { return "Thread Pointer / ID Register (EL3)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1101,0b0000,0b011): // 2.91: TPIDRRO_EL0, Thread Pointer / ID Register, Read-Only (EL0)
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "TPIDRRO_EL0"; }
          char const* Describe() const { return "Thread Pointer / ID Register, Read-Only (EL0)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0010,0b0000,0b000): // 2.92: TTBR0_EL1, Translation Table Base Register 0 (EL1)
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "TTBR0_EL1"; }
          char const* Describe() const { return "Translation Table Base Register 0 (EL1)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b0010,0b0000,0b000): // 2.93: TTBR0_EL2, Translation Table Base Register 0 (EL2)
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "TTBR0_EL2"; }
          char const* Describe() const { return "Translation Table Base Register 0 (EL2)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b110,0b0010,0b0000,0b000): // 2.94: TTBR0_EL3, Translation Table Base Register 0 (EL3)
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "TTBR0_EL3"; }
          char const* Describe() const { return "Translation Table Base Register 0 (EL3)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0010,0b0000,0b001): // 2.95: TTBR1_EL1, Translation Table Base Register 1
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "TTBR1_EL1"; }
          char const* Describe() const { return "Translation Table Base Register 1"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b0000,0b000): // 2.96: VBAR_EL1, Vector Base Address Register (EL1)
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "VBAR_EL1"; }
          char const* Describe() const { return "Vector Base Address Register (EL1)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1100,0b0000,0b000): // 2.97: VBAR_EL2, Vector Base Address Register (EL2)
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "VBAR_EL2"; }
          char const* Describe() const { return "Vector Base Address Register (EL2)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b110,0b1100,0b0000,0b000): // 2.98: VBAR_EL3, Vector Base Address Register (EL3)
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "VBAR_EL3"; }
          char const* Describe() const { return "Vector Base Address Register (EL3)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b0000,0b0000,0b101): // 2.99: VMPIDR_EL2, Virtualization Multiprocessor ID Register
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "VMPIDR_EL2"; }
          char const* Describe() const { return "Virtualization Multiprocessor ID Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b0000,0b0000,0b000): // 2.100: VPIDR_EL2, Virtualization Processor ID Register
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "VPIDR_EL2"; }
          char const* Describe() const { return "Virtualization Processor ID Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b0010,0b0001,0b010): // 2.101: VTCR_EL2, Virtualization Translation Control Register
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "VTCR_EL2"; }
          char const* Describe() const { return "Virtualization Translation Control Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b0010,0b0001,0b000): // 2.102: VTTBR_EL2, Virtualization Translation Table Base Register
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "VTTBR_EL2"; }
          char const* Describe() const { return "Virtualization Translation Table Base Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b000,0b0111,0b1110,0b110): // 3.1: DBGAUTHSTATUS_EL1, Debug Authentication Status register
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "DBGAUTHSTATUS_EL1"; }
          char const* Describe() const { return "Debug Authentication Status register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b000,0b0000,0b0000,0b101): // 3.2: DBGBCR<n>_EL1, Debug Breakpoint Control Registers, n = 0 - 15
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "DBGBCR<n>_EL1"; }
          char const* Describe() const { return "Debug Breakpoint Control Registers, n = 0 - 15"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b000,0b0000,0b0000,0b100): // 3.3: DBGBVR<n>_EL1, Debug Breakpoint Value Registers, n = 0 - 15
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "DBGBVR<n>_EL1"; }
          char const* Describe() const { return "Debug Breakpoint Value Registers, n = 0 - 15"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b000,0b0111,0b1001,0b110): // 3.4: DBGCLAIMCLR_EL1, Debug Claim Tag Clear register
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "DBGCLAIMCLR_EL1"; }
          char const* Describe() const { return "Debug Claim Tag Clear register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b000,0b0111,0b1000,0b110): // 3.5: DBGCLAIMSET_EL1, Debug Claim Tag Set register
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "DBGCLAIMSET_EL1"; }
          char const* Describe() const { return "Debug Claim Tag Set register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b011,0b0000,0b0100,0b000): // 3.6: DBGDTR_EL0, Debug Data Transfer Register, half-duplex
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "DBGDTR_EL0"; }
          char const* Describe() const { return "Debug Data Transfer Register, half-duplex"; }
        } x; return &x;
      } break;

    // case SYSENCODE(0b10,0b011,0b0000,0b0101,0b000): 3.7: DBGDTRRX_EL0, Debug Data Transfer Register, Receive
    //   {
    //     static struct : public MovedSysReg {
    //       char const* Name() const { return "DBGDTRRX_EL0"; }
    //       char const* Describe() const { return "Debug Data Transfer Register, Receive"; }
    //     } x; return &x;
    //   } break;

    // case SYSENCODE(0b10,0b011,0b0000,0b0101,0b000): 3.8: DBGDTRTX_EL0, Debug Data Transfer Register, Transmit
    //   {
    //     static struct : public MovedSysReg {
    //       char const* Name() const { return "DBGDTRTX_EL0"; }
    //       char const* Describe() const { return "Debug Data Transfer Register, Transmit"; }
    //     } x; return &x;
    //   } break;

    case SYSENCODE(0b10,0b011,0b0000,0b0101,0b000): // 3.8: DBGDTR_EL0, Debug Data Transfer Register
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "DBGDTR_EL0"; }
          char const* Describe() const { return "Debug Data Transfer Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b000,0b0001,0b0100,0b100): // 3.9: DBGPRCR_EL1, Debug Power Control Register
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "DBGPRCR_EL1"; }
          char const* Describe() const { return "Debug Power Control Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b100,0b0000,0b0111,0b000): // 3.10: DBGVCR32_EL2, Debug Vector Catch Register
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "DBGVCR32_EL2"; }
          char const* Describe() const { return "Debug Vector Catch Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b000,0b0000,0b0000,0b111): // 3.11: DBGWCR<n>_EL1, Debug Watchpoint Control Registers, n = 0 - 15
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "DBGWCR<n>_EL1"; }
          char const* Describe() const { return "Debug Watchpoint Control Registers, n = 0 - 15"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b000,0b0000,0b0000,0b110): // 3.12: DBGWVR<n>_EL1, Debug Watchpoint Value Registers, n = 0 - 15
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "DBGWVR<n>_EL1"; }
          char const* Describe() const { return "Debug Watchpoint Value Registers, n = 0 - 15"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b0100,0b0101,0b001): // 3.13: DLR_EL0, Debug Link Register
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "DLR_EL0"; }
          char const* Describe() const { return "Debug Link Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b0100,0b0101,0b000): // 3.14: DSPSR_EL0, Debug Saved Program Status Register
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "DSPSR_EL0"; }
          char const* Describe() const { return "Debug Saved Program Status Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b000,0b0000,0b0010,0b000): // 3.15: MDCCINT_EL1, Monitor DCC Interrupt Enable Register
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "MDCCINT_EL1"; }
          char const* Describe() const { return "Monitor DCC Interrupt Enable Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b011,0b0000,0b0001,0b000): // 3.16: MDCCSR_EL0, Monitor DCC Status Register
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "MDCCSR_EL0"; }
          char const* Describe() const { return "Monitor DCC Status Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b0001,0b0001,0b001): // 3.17: MDCR_EL2, Monitor Debug Configuration Register (EL2)
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "MDCR_EL2"; }
          char const* Describe() const { return "Monitor Debug Configuration Register (EL2)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b110,0b0001,0b0011,0b001): // 3.18: MDCR_EL3, Monitor Debug Configuration Register (EL3)
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "MDCR_EL3"; }
          char const* Describe() const { return "Monitor Debug Configuration Register (EL3)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b000,0b0001,0b0000,0b000): // 3.19: MDRAR_EL1, Monitor Debug ROM Address Register
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "MDRAR_EL1"; }
          char const* Describe() const { return "Monitor Debug ROM Address Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b000,0b0000,0b0010,0b010): // 3.20: MDSCR_EL1, Monitor Debug System Control Register
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "MDSCR_EL1"; }
          char const* Describe() const { return "Monitor Debug System Control Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b000,0b0001,0b0011,0b100): // 3.21: OSDLR_EL1, OS Double Lock Register
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "OSDLR_EL1"; }
          char const* Describe() const { return "OS Double Lock Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b000,0b0000,0b0000,0b010): // 3.22: OSDTRRX_EL1, OS Lock Data Transfer Register, Receive
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "OSDTRRX_EL1"; }
          char const* Describe() const { return "OS Lock Data Transfer Register, Receive"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b000,0b0000,0b0011,0b010): // 3.23: OSDTRTX_EL1, OS Lock Data Transfer Register, Transmit
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "OSDTRTX_EL1"; }
          char const* Describe() const { return "OS Lock Data Transfer Register, Transmit"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b000,0b0000,0b0110,0b010): // 3.24: OSECCR_EL1, OS Lock Exception Catch Control Register
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "OSECCR_EL1"; }
          char const* Describe() const { return "OS Lock Exception Catch Control Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b000,0b0001,0b0000,0b100): // 3.25: OSLAR_EL1, OS Lock Access Register
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "OSLAR_EL1"; }
          char const* Describe() const { return "OS Lock Access Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b000,0b0001,0b0001,0b100): // 3.26: OSLSR_EL1, OS Lock Status Register
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "OSLSR_EL1"; }
          char const* Describe() const { return "OS Lock Status Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b110,0b0001,0b0001,0b001): // 3.27: SDER32_EL3, AArch32 Secure Debug Enable Register
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "SDER32_EL3"; }
          char const* Describe() const { return "AArch32 Secure Debug Enable Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1110,0b1111,0b111): // 4.1: PMCCFILTR_EL0, Performance Monitors Cycle Count Filter Register
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "PMCCFILTR_EL0"; }
          char const* Describe() const { return "Performance Monitors Cycle Count Filter Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1001,0b1101,0b000): // 4.2: PMCCNTR_EL0, Performance Monitors Cycle Count Register
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "PMCCNTR_EL0"; }
          char const* Describe() const { return "Performance Monitors Cycle Count Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1001,0b1100,0b110): // 4.3: PMCEID0_EL0, Performance Monitors Common Event Identification register 0
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "PMCEID0_EL0"; }
          char const* Describe() const { return "Performance Monitors Common Event Identification register 0"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1001,0b1100,0b111): // 4.4: PMCEID1_EL0, Performance Monitors Common Event Identification register 1
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "PMCEID1_EL0"; }
          char const* Describe() const { return "Performance Monitors Common Event Identification register 1"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1001,0b1100,0b010): // 4.5: PMCNTENCLR_EL0, Performance Monitors Count Enable Clear register
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "PMCNTENCLR_EL0"; }
          char const* Describe() const { return "Performance Monitors Count Enable Clear register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1001,0b1100,0b001): // 4.6: PMCNTENSET_EL0, Performance Monitors Count Enable Set register
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "PMCNTENSET_EL0"; }
          char const* Describe() const { return "Performance Monitors Count Enable Set register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1001,0b1100,0b000): // 4.7: PMCR_EL0, Performance Monitors Control Register
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "PMCR_EL0"; }
          char const* Describe() const { return "Performance Monitors Control Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1110,0b1000,0b000): // 4.8: PMEVCNTR<n>_EL0, Performance Monitors Event Count Registers, n = 0 - 30
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "PMEVCNTR<n>_EL0"; }
          char const* Describe() const { return "Performance Monitors Event Count Registers, n = 0 - 30"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1110,0b1100,0b000): // 4.9: PMEVTYPER<n>_EL0, Performance Monitors Event Type Registers, n = 0 - 30
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "PMEVTYPER<n>_EL0"; }
          char const* Describe() const { return "Performance Monitors Event Type Registers, n = 0 - 30"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1001,0b1110,0b010): // 4.10: PMINTENCLR_EL1, Performance Monitors Interrupt Enable Clear register
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "PMINTENCLR_EL1"; }
          char const* Describe() const { return "Performance Monitors Interrupt Enable Clear register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1001,0b1110,0b001): // 4.11: PMINTENSET_EL1, Performance Monitors Interrupt Enable Set register
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "PMINTENSET_EL1"; }
          char const* Describe() const { return "Performance Monitors Interrupt Enable Set register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1001,0b1100,0b011): // 4.12: PMOVSCLR_EL0, Performance Monitors Overflow Flag Status Clear Register
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "PMOVSCLR_EL0"; }
          char const* Describe() const { return "Performance Monitors Overflow Flag Status Clear Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1001,0b1110,0b011): // 4.13: PMOVSSET_EL0, Performance Monitors Overflow Flag Status Set register
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "PMOVSSET_EL0"; }
          char const* Describe() const { return "Performance Monitors Overflow Flag Status Set register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1001,0b1100,0b101): // 4.14: PMSELR_EL0, Performance Monitors Event Counter Selection Register
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "PMSELR_EL0"; }
          char const* Describe() const { return "Performance Monitors Event Counter Selection Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1001,0b1100,0b100): // 4.15: PMSWINC_EL0, Performance Monitors Software Increment register
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "PMSWINC_EL0"; }
          char const* Describe() const { return "Performance Monitors Software Increment register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1001,0b1110,0b000): // 4.16: PMUSERENR_EL0, Performance Monitors User Enable Register
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "PMUSERENR_EL0"; }
          char const* Describe() const { return "Performance Monitors User Enable Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1001,0b1101,0b010): // 4.17: PMXEVCNTR_EL0, Performance Monitors Selected Event Count Register
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "PMXEVCNTR_EL0"; }
          char const* Describe() const { return "Performance Monitors Selected Event Count Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1001,0b1101,0b001): // 4.18: PMXEVTYPER_EL0, Performance Monitors Selected Event Type Register
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "PMXEVTYPER_EL0"; }
          char const* Describe() const { return "Performance Monitors Selected Event Type Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1110,0b0000,0b000): // 5.1: CNTFRQ_EL0, Counter-timer Frequency register
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "CNTFRQ_EL0"; }
          char const* Describe() const { return "Counter-timer Frequency register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1110,0b0001,0b000): // 5.2: CNTHCTL_EL2, Counter-timer Hypervisor Control register
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "CNTHCTL_EL2"; }
          char const* Describe() const { return "Counter-timer Hypervisor Control register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1110,0b0010,0b001): // 5.3: CNTHP_CTL_EL2, Counter-timer Hypervisor Physical Timer Control register
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "CNTHP_CTL_EL2"; }
          char const* Describe() const { return "Counter-timer Hypervisor Physical Timer Control register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1110,0b0010,0b010): // 5.4: CNTHP_CVAL_EL2, Counter-timer Hypervisor Physical Timer CompareValue register
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "CNTHP_CVAL_EL2"; }
          char const* Describe() const { return "Counter-timer Hypervisor Physical Timer CompareValue register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1110,0b0010,0b000): // 5.5: CNTHP_TVAL_EL2, Counter-timer Hypervisor Physical Timer TimerValue register
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "CNTHP_TVAL_EL2"; }
          char const* Describe() const { return "Counter-timer Hypervisor Physical Timer TimerValue register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1110,0b0001,0b000): // 5.6: CNTKCTL_EL1, Counter-timer Kernel Control register
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "CNTKCTL_EL1"; }
          char const* Describe() const { return "Counter-timer Kernel Control register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1110,0b0010,0b001): // 5.7: CNTP_CTL_EL0, Counter-timer Physical Timer Control register
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "CNTP_CTL_EL0"; }
          char const* Describe() const { return "Counter-timer Physical Timer Control register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1110,0b0010,0b010): // 5.8: CNTP_CVAL_EL0, Counter-timer Physical Timer CompareValue register
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "CNTP_CVAL_EL0"; }
          char const* Describe() const { return "Counter-timer Physical Timer CompareValue register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1110,0b0010,0b000): // 5.9: CNTP_TVAL_EL0, Counter-timer Physical Timer TimerValue register
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "CNTP_TVAL_EL0"; }
          char const* Describe() const { return "Counter-timer Physical Timer TimerValue register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1110,0b0000,0b001): // 5.10: CNTPCT_EL0, Counter-timer Physical Count register
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "CNTPCT_EL0"; }
          char const* Describe() const { return "Counter-timer Physical Count register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b111,0b1110,0b0010,0b001): // 5.11: CNTPS_CTL_EL1, Counter-timer Physical Secure Timer Control register
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "CNTPS_CTL_EL1"; }
          char const* Describe() const { return "Counter-timer Physical Secure Timer Control register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b111,0b1110,0b0010,0b010): // 5.12: CNTPS_CVAL_EL1, Counter-timer Physical Secure Timer CompareValue register
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "CNTPS_CVAL_EL1"; }
          char const* Describe() const { return "Counter-timer Physical Secure Timer CompareValue register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b111,0b1110,0b0010,0b000): // 5.13: CNTPS_TVAL_EL1, Counter-timer Physical Secure Timer TimerValue register
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "CNTPS_TVAL_EL1"; }
          char const* Describe() const { return "Counter-timer Physical Secure Timer TimerValue register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1110,0b0011,0b001): // 5.14: CNTV_CTL_EL0, Counter-timer Virtual Timer Control register
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "CNTV_CTL_EL0"; }
          char const* Describe() const { return "Counter-timer Virtual Timer Control register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1110,0b0011,0b010): // 5.15: CNTV_CVAL_EL0, Counter-timer Virtual Timer CompareValue register
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "CNTV_CVAL_EL0"; }
          char const* Describe() const { return "Counter-timer Virtual Timer CompareValue register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1110,0b0011,0b000): // 5.16: CNTV_TVAL_EL0, Counter-timer Virtual Timer TimerValue register
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "CNTV_TVAL_EL0"; }
          char const* Describe() const { return "Counter-timer Virtual Timer TimerValue register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1110,0b0000,0b010): // 5.17: CNTVCT_EL0, Counter-timer Virtual Count register
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "CNTVCT_EL0"; }
          char const* Describe() const { return "Counter-timer Virtual Count register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1110,0b0000,0b011): // 5.18: CNTVOFF_EL2, Counter-timer Virtual Offset register
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "CNTVOFF_EL2"; }
          char const* Describe() const { return "Counter-timer Virtual Offset register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b1000,0b100): // 6.1: ICC_AP0R0_EL1, Interrupt Controller Active Priorities Register (0,0)
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "ICC_AP0R0_EL1"; }
          char const* Describe() const { return "Interrupt Controller Active Priorities Register (0,0)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b1000,0b101): // 6.2: ICC_AP0R1_EL1, Interrupt Controller Active Priorities Register (0,1)
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "ICC_AP0R1_EL1"; }
          char const* Describe() const { return "Interrupt Controller Active Priorities Register (0,1)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b1000,0b110): // 6.3: ICC_AP0R2_EL1, Interrupt Controller Active Priorities Register (0,2)
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "ICC_AP0R2_EL1"; }
          char const* Describe() const { return "Interrupt Controller Active Priorities Register (0,2)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b1000,0b111): // 6.4: ICC_AP0R3_EL1, Interrupt Controller Active Priorities Register (0,3)
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "ICC_AP0R3_EL1"; }
          char const* Describe() const { return "Interrupt Controller Active Priorities Register (0,3)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b1001,0b000): // 6.5: ICC_AP1R0_EL1, Interrupt Controller Active Priorities Register (1,0)
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "ICC_AP1R0_EL1"; }
          char const* Describe() const { return "Interrupt Controller Active Priorities Register (1,0)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b1001,0b001): // 6.6: ICC_AP1R1_EL1, Interrupt Controller Active Priorities Register (1,1)
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "ICC_AP1R1_EL1"; }
          char const* Describe() const { return "Interrupt Controller Active Priorities Register (1,1)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b1001,0b010): // 6.7: ICC_AP1R2_EL1, Interrupt Controller Active Priorities Register (1,2)
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "ICC_AP1R2_EL1"; }
          char const* Describe() const { return "Interrupt Controller Active Priorities Register (1,2)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b1001,0b011): // 6.8: ICC_AP1R3_EL1, Interrupt Controller Active Priorities Register (1,3)
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "ICC_AP1R3_EL1"; }
          char const* Describe() const { return "Interrupt Controller Active Priorities Register (1,3)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b1011,0b110): // 6.9: ICC_ASGI1R_EL1, Interrupt Controller Alias Software Generated Interrupt group 1 Register
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "ICC_ASGI1R_EL1"; }
          char const* Describe() const { return "Interrupt Controller Alias Software Generated Interrupt group 1 Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b1000,0b011): // 6.10: ICC_BPR0_EL1, Interrupt Controller Binary Point Register 0
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "ICC_BPR0_EL1"; }
          char const* Describe() const { return "Interrupt Controller Binary Point Register 0"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b1100,0b011): // 6.11: ICC_BPR1_EL1, Interrupt Controller Binary Point Register 1
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "ICC_BPR1_EL1"; }
          char const* Describe() const { return "Interrupt Controller Binary Point Register 1"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b1100,0b100): // 6.12: ICC_CTLR_EL1, Interrupt Controller Control Register (EL1)
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "ICC_CTLR_EL1"; }
          char const* Describe() const { return "Interrupt Controller Control Register (EL1)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b110,0b1100,0b1100,0b100): // 6.13: ICC_CTLR_EL3, Interrupt Controller Control Register (EL3)
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "ICC_CTLR_EL3"; }
          char const* Describe() const { return "Interrupt Controller Control Register (EL3)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b1011,0b001): // 6.14: ICC_DIR_EL1, Interrupt Controller Deactivate Interrupt Register
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "ICC_DIR_EL1"; }
          char const* Describe() const { return "Interrupt Controller Deactivate Interrupt Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b1000,0b001): // 6.15: ICC_EOIR0_EL1, Interrupt Controller End Of Interrupt Register 0
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "ICC_EOIR0_EL1"; }
          char const* Describe() const { return "Interrupt Controller End Of Interrupt Register 0"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b1100,0b001): // 6.16: ICC_EOIR1_EL1, Interrupt Controller End Of Interrupt Register 1
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "ICC_EOIR1_EL1"; }
          char const* Describe() const { return "Interrupt Controller End Of Interrupt Register 1"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b1000,0b010): // 6.17: ICC_HPPIR0_EL1, Interrupt Controller Highest Priority Pending Interrupt Register 0
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "ICC_HPPIR0_EL1"; }
          char const* Describe() const { return "Interrupt Controller Highest Priority Pending Interrupt Register 0"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b1100,0b010): // 6.18: ICC_HPPIR1_EL1, Interrupt Controller Highest Priority Pending Interrupt Register 1
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "ICC_HPPIR1_EL1"; }
          char const* Describe() const { return "Interrupt Controller Highest Priority Pending Interrupt Register 1"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b1000,0b000): // 6.19: ICC_IAR0_EL1, Interrupt Controller Interrupt Acknowledge Register 0
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "ICC_IAR0_EL1"; }
          char const* Describe() const { return "Interrupt Controller Interrupt Acknowledge Register 0"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b1100,0b000): // 6.20: ICC_IAR1_EL1, Interrupt Controller Interrupt Acknowledge Register 1
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "ICC_IAR1_EL1"; }
          char const* Describe() const { return "Interrupt Controller Interrupt Acknowledge Register 1"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b1100,0b110): // 6.21: ICC_IGRPEN0_EL1, Interrupt Controller Interrupt Group 0 Enable register
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "ICC_IGRPEN0_EL1"; }
          char const* Describe() const { return "Interrupt Controller Interrupt Group 0 Enable register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b1100,0b111): // 6.22: ICC_IGRPEN1_EL1, Interrupt Controller Interrupt Group 1 Enable register
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "ICC_IGRPEN1_EL1"; }
          char const* Describe() const { return "Interrupt Controller Interrupt Group 1 Enable register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b110,0b1100,0b1100,0b111): // 6.23: ICC_IGRPEN1_EL3, Interrupt Controller Interrupt Group 1 Enable register (EL3)
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "ICC_IGRPEN1_EL3"; }
          char const* Describe() const { return "Interrupt Controller Interrupt Group 1 Enable register (EL3)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0100,0b0110,0b000): // 6.24: ICC_PMR_EL1, Interrupt Controller Interrupt Priority Mask Register
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "ICC_PMR_EL1"; }
          char const* Describe() const { return "Interrupt Controller Interrupt Priority Mask Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b1011,0b011): // 6.25: ICC_RPR_EL1, Interrupt Controller Running Priority Register
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "ICC_RPR_EL1"; }
          char const* Describe() const { return "Interrupt Controller Running Priority Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b1101,0b000): // 6.26: ICC_SEIEN_EL1, Interrupt Controller System Error Interrupt Enable register
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "ICC_SEIEN_EL1"; }
          char const* Describe() const { return "Interrupt Controller System Error Interrupt Enable register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b1011,0b111): // 6.27: ICC_SGI0R_EL1, Interrupt Controller Software Generated Interrupt group 0 Register
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "ICC_SGI0R_EL1"; }
          char const* Describe() const { return "Interrupt Controller Software Generated Interrupt group 0 Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b1011,0b101): // 6.28: ICC_SGI1R_EL1, Interrupt Controller Software Generated Interrupt group 1 Register
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "ICC_SGI1R_EL1"; }
          char const* Describe() const { return "Interrupt Controller Software Generated Interrupt group 1 Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b1100,0b101): // 6.29: ICC_SRE_EL1, Interrupt Controller System Register Enable register (EL1)
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "ICC_SRE_EL1"; }
          char const* Describe() const { return "Interrupt Controller System Register Enable register (EL1)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1100,0b1001,0b101): // 6.30: ICC_SRE_EL2, Interrupt Controller System Register Enable register (EL2)
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "ICC_SRE_EL2"; }
          char const* Describe() const { return "Interrupt Controller System Register Enable register (EL2)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b110,0b1100,0b1100,0b101): // 6.31: ICC_SRE_EL3, Interrupt Controller System Register Enable register (EL3)
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "ICC_SRE_EL3"; }
          char const* Describe() const { return "Interrupt Controller System Register Enable register (EL3)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1100,0b1000,0b000): // 6.32: ICH_AP0R0_EL2, Interrupt Controller Hyp Active Priorities Register (0,0)
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "ICH_AP0R0_EL2"; }
          char const* Describe() const { return "Interrupt Controller Hyp Active Priorities Register (0,0)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1100,0b1000,0b001): // 6.33: ICH_AP0R1_EL2, Interrupt Controller Hyp Active Priorities Register (0,1)
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "ICH_AP0R1_EL2"; }
          char const* Describe() const { return "Interrupt Controller Hyp Active Priorities Register (0,1)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1100,0b1000,0b010): // 6.34: ICH_AP0R2_EL2, Interrupt Controller Hyp Active Priorities Register (0,2)
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "ICH_AP0R2_EL2"; }
          char const* Describe() const { return "Interrupt Controller Hyp Active Priorities Register (0,2)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1100,0b1000,0b011): // 6.35: ICH_AP0R3_EL2, Interrupt Controller Hyp Active Priorities Register (0,3)
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "ICH_AP0R3_EL2"; }
          char const* Describe() const { return "Interrupt Controller Hyp Active Priorities Register (0,3)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1100,0b1001,0b000): // 6.36: ICH_AP1R0_EL2, Interrupt Controller Hyp Active Priorities Register (1,0)
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "ICH_AP1R0_EL2"; }
          char const* Describe() const { return "Interrupt Controller Hyp Active Priorities Register (1,0)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1100,0b1001,0b001): // 6.37: ICH_AP1R1_EL2, Interrupt Controller Hyp Active Priorities Register (1,1)
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "ICH_AP1R1_EL2"; }
          char const* Describe() const { return "Interrupt Controller Hyp Active Priorities Register (1,1)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1100,0b1001,0b010): // 6.38: ICH_AP1R2_EL2, Interrupt Controller Hyp Active Priorities Register (1,2)
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "ICH_AP1R2_EL2"; }
          char const* Describe() const { return "Interrupt Controller Hyp Active Priorities Register (1,2)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1100,0b1001,0b011): // 6.39: ICH_AP1R3_EL2, Interrupt Controller Hyp Active Priorities Register (1,3)
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "ICH_AP1R3_EL2"; }
          char const* Describe() const { return "Interrupt Controller Hyp Active Priorities Register (1,3)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1100,0b1011,0b011): // 6.40: ICH_EISR_EL2, Interrupt Controller End of Interrupt Status Register
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "ICH_EISR_EL2"; }
          char const* Describe() const { return "Interrupt Controller End of Interrupt Status Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1100,0b1011,0b101): // 6.41: ICH_ELSR_EL2, Interrupt Controller Empty List Register Status Register
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "ICH_ELSR_EL2"; }
          char const* Describe() const { return "Interrupt Controller Empty List Register Status Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1100,0b1011,0b000): // 6.42: ICH_HCR_EL2, Interrupt Controller Hyp Control Register
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "ICH_HCR_EL2"; }
          char const* Describe() const { return "Interrupt Controller Hyp Control Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1100,0b1100,0b000): // 6.43: ICH_LR<n>_EL2, Interrupt Controller List Registers, n = 0 - 15
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "ICH_LR<n>_EL2"; }
          char const* Describe() const { return "Interrupt Controller List Registers, n = 0 - 15"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1100,0b1011,0b010): // 6.44: ICH_MISR_EL2, Interrupt Controller Maintenance Interrupt State Register
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "ICH_MISR_EL2"; }
          char const* Describe() const { return "Interrupt Controller Maintenance Interrupt State Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1100,0b1011,0b111): // 6.45: ICH_VMCR_EL2, Interrupt Controller Virtual Machine Control Register
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "ICH_VMCR_EL2"; }
          char const* Describe() const { return "Interrupt Controller Virtual Machine Control Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1100,0b1001,0b100): // 6.46: ICH_VSEIR_EL2, Interrupt Controller Virtual System Error Interrupt Register
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "ICH_VSEIR_EL2"; }
          char const* Describe() const { return "Interrupt Controller Virtual System Error Interrupt Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1100,0b1011,0b001): // 6.47: ICH_VTR_EL2, Interrupt Controller VGIC Type Register
      {
        static struct : public MovedSysReg {
          char const* Name() const { return "ICH_VTR_EL2"; }
          char const* Describe() const { return "Interrupt Controller VGIC Type Register"; }
        } x; return &x;
      } break;

    }

  /*  System instructions with immediate, CRm used as immediate field,
   *  and thus ignored during decode.  (TODO: rt should be 0b11111).
   */
  switch (SYSENCODE(op0, op1, crn, 0, op2))
    {
    case SYSENCODE(0b00,0b011,0b0011,0b0000,0b100):
      {
        static struct : public BarrierSysReg { virtual char const* Operation() const override { return "dsb"; } } x;
        return &x;
      } break;
      
    case SYSENCODE(0b00,0b011,0b0011,0b0000,0b101):
      {
        static struct : public BarrierSysReg { virtual char const* Operation() const override { return "dmb"; } } x;
        return &x;
      } break;
      
    case SYSENCODE(0b00,0b011,0b0011,0b0000,0b110):
      {
        static struct : public BarrierSysReg { virtual char const* Operation() const override { return "isb"; } } x;
        return &x;
      } break;
      
    case SYSENCODE(0b00,0b000,0b0100,0b0000,0b101):
      {
        static struct : public PStateSysReg {
          virtual char const* Name() const override { return "PSTATEField_SP"; }
          virtual void Write(uint8_t, uint8_t, uint8_t, uint8_t crm, uint8_t, AArch64& cpu, U64) const override { throw 0; }
        } x; return &x;
      } break;
      
    case SYSENCODE(0b00,0b011,0b0100,0b0000,0b110):
      {
        static struct : public PStateSysReg {
          virtual char const* Name() const override { return "PSTATEField_DAIFSet"; }
          virtual void Write(uint8_t, uint8_t, uint8_t, uint8_t crm, uint8_t, AArch64& cpu, U64) const override { throw 0; }
        } x; return &x;
      } break;
      
    case SYSENCODE(0b00,0b011,0b0100,0b0000,0b111):
      {
        static struct : public PStateSysReg {
          virtual char const* Name() const override { return "PSTATEField_DAIFClr"; }
          virtual void Write(uint8_t, uint8_t, uint8_t, uint8_t crm, uint8_t, AArch64& cpu, U64) const override { throw 0; }
        } x; return &x;
      } break;
    }
  
  static struct UnknownSysReg : public SysReg
  {
    void fields(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, std::ostream& sink) const
    {
      sink << "op0=" << unsigned(op0) << ", op1=" << unsigned(op1) << ", crn=" << unsigned(crn) << ", crm=" << unsigned(crm) << ", op2=" << unsigned(op2);
    }
    virtual void DisasmRead(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, std::ostream& sink) const override
    {
      sink << "sysl\t" << unisim::component::cxx::processor::arm::isa::arm64::DisasmGZXR(rt) << ", <unknown>\t; ";
      fields(op0, op1, crn, crm, op2, sink);
    }
    virtual void DisasmWrite(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, std::ostream& sink) const override
    {
      sink << "sys\t<unknown>, " << unisim::component::cxx::processor::arm::isa::arm64::DisasmGZXR(rt) << "\t; ";
      fields(op0, op1, crn, crm, op2, sink);
    }
    virtual void Write(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, AArch64& cpu, U64 value) const override
    {
      fields(op0, op1, crn, crm, op2, std::cerr << "Unimplemented. Cannot write ");
      std::cerr << std::endl;
      throw 0; // cpu.UnpredictableInsnBehaviour();
    }
    virtual U64 Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2,  AArch64& cpu) const override
    {
      fields(op0, op1, crn, crm, op2, std::cerr << "Unimplemented. Cannot read ");
      std::cerr << std::endl;
      throw 0; // cpu.UnpredictableInsnBehaviour();
      return U64();
    }
  } err;
  
  err.fields(op0, op1, crn, crm, op2, std::cerr << "Unknown CP15 register: ");
  std::cerr << std::endl;
  return &err;
}

/** Resets the internal values of corresponding CP15 Registers
 */
// void
// AArch64::ResetSystemRegisters()
// {
//   // // Base default values for SCTLR (may be overwritten by memory architectures)
//   // SCTLR = 0x00c50058; // SBO mask
//   // sctlr::TE.Set(      SCTLR, 0 ); // Thumb Exception enable
//   // sctlr::NMFI.Set(    SCTLR, 0 ); // Non-maskable FIQ (NMFI) support
//   // sctlr::EE.Set(      SCTLR, 0 ); // Exception Endianness.
//   // sctlr::VE.Set(      SCTLR, 0 ); // Interrupt Vectors Enable
//   // sctlr::U.Set(       SCTLR, 1 ); // Alignment Model (0 before ARMv6, 0 or 1 in ARMv6, 1 in armv7)
//   // sctlr::FI.Set(      SCTLR, 0 ); // Fast interrupts configuration enable
//   // sctlr::RR.Set(      SCTLR, 0 ); // Round Robin select
//   // sctlr::V.Set(       SCTLR, 0 ); // Vectors bit
//   // sctlr::I.Set(       SCTLR, 0 ); // Instruction cache enable
//   // sctlr::Z.Set(       SCTLR, 0 ); // Branch prediction enable.
//   // sctlr::SW.Set(      SCTLR, 0 ); // SWP and SWPB enable. This bit enables the use of SWP and SWPB instructions.
//   // sctlr::B.Set(       SCTLR, 0 ); // Endianness model (up to ARMv6)
//   // sctlr::CP15BEN.Set( SCTLR, 1 ); // CP15 barrier enable.
//   // sctlr::C.Set(       SCTLR, 0 ); // Cache enable. This is a global enable bit for data and unified caches.
//   // sctlr::A.Set(       SCTLR, 0 ); // Alignment check enable
//   // sctlr::M.Set(       SCTLR, 0 ); // MMU enable.

//   // CPACR = 0x0;
// }
