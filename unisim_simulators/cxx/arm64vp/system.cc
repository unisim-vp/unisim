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
 *          Gilles Mouchard (gilles.mouchard@cea.fr)
 */

#include <architecture.hh>
#include <unisim/component/cxx/processor/arm/isa/arm64/disasm.hh>
#include <iostream>
#include <iomanip>

/** Check access permissions for op1-n system registers
 *
 * @param op1     the "op1" field of the instruction code
 */
void
AArch64::CheckSystemAccess(uint8_t op1)
{

}

#define SYSENCODE( OP0, OP1, CRN, CRM, OP2 ) ((OP0 << 16) | (OP1 << 12) | (CRN << 8) | (CRM << 4) | (OP2 << 0))

/** Get the Internal representation of the system register
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
  struct BaseSysReg : public SysReg
  {
    struct Encoding { unsigned op0 : 4; unsigned op1 : 4; unsigned crn : 4; unsigned crm : 4; unsigned op2 : 4; };
    virtual void Name(Encoding, std::ostream& sink) const {}
    virtual void Describe(Encoding, std::ostream& sink) const {}
    virtual char const* ReadOperation() const { return "mrs"; }
    virtual char const* WriteOperation() const { return "msr"; }
    void Describe(Encoding e, char const* prefix, std::ostream& sink) const
    {
      std::ostringstream buf;
      this->Describe(e, buf);
      auto && s = buf.str();
      if (not s.size())
        return;
      sink << prefix << s;
    }

    void DisasmRead(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, std::ostream& sink) const override
    {
      Encoding e{op0, op1, crn, crm, op2};
      sink << ReadOperation() << '\t' << unisim::component::cxx::processor::arm::isa::arm64::DisasmGZXR(rt) << ", ";
      Name(e, sink);
      Describe(e, "\t; ", sink);
    }

    void DisasmWrite(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, std::ostream& sink) const override
    {
      Encoding e{op0, op1, crn, crm, op2};
      sink << WriteOperation() << '\t';
      Name(e, sink);
      sink << ", " << unisim::component::cxx::processor::arm::isa::arm64::DisasmGZXR(rt);
      Describe(e, "\t; ", sink);
    }

    void Write(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu, U64 value) const override
    {
      std::cerr << "error : undefined system register write in:\n" << std::hex << cpu.current_insn_addr << std::dec << ":\t";
      DisasmWrite(op0, op1, crn, crm, op2, rt, std::cerr);
      std::cerr << "\n";
      throw AArch64::UndefinedInstructionException();
    }

    U64 Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu) const override
    {
      std::cerr << "error : undefined system register read in:\n" << std::hex << cpu.current_insn_addr << std::dec << ":\t";
      DisasmRead(op0, op1, crn, crm, op2, rt, std::cerr);
      std::cerr << "\n";
      throw AArch64::UndefinedInstructionException();
      return U64();
    }
  };

  struct DCSysReg : public BaseSysReg
  {
    virtual char const* ReadOperation() const override { return "dc<bad-read>"; }
    virtual char const* WriteOperation() const override { return "dc"; }
  };

  struct ICSysReg : public BaseSysReg
  {
    virtual char const* ReadOperation() const override { return "ic<bad-read>"; }
    virtual char const* WriteOperation() const override { return "ic"; }
  };

  struct TLBISysReg : public BaseSysReg
  {
    virtual char const* ReadOperation() const override { return "tlbi<bad-read>"; }
    virtual char const* WriteOperation() const override { return "tlbi"; }
    virtual bool hasArg(Encoding) const = 0;
    void DisasmWrite(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, std::ostream& sink) const override
    {
      Encoding e{op0, op1, crn, crm, op2};
      if (hasArg(e))
        BaseSysReg::DisasmWrite(op0, op1, crn, crm, op2, rt, sink);
      else {
        Name(e, sink << "tlbi\t");
        Describe(e, "\t; ", sink);
      }
    }
  };

  struct ATSysReg : public BaseSysReg
  {
    virtual char const* ReadOperation() const override { return "at<bad-read>"; }
    virtual char const* WriteOperation() const override { return "at"; }
  };

  struct PStateSysReg : public BaseSysReg
  {
    char const* ReadOperation() const override { return "mrs<bad-read>"; }
    void DisasmWrite(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, std::ostream& sink) const override
    {
      Encoding e{op0, op1, crn, crm, op2};
      Name(e, sink << "msr\t");
      sink << ", " << unisim::component::cxx::processor::arm::isa::arm64::DisasmI(crm);
      Describe(e, "\t; ", sink);
    }
  };

  struct WOOpSysReg : public BaseSysReg
  {
    virtual void Disasm(Encoding, std::ostream&) const = 0;
    void DisasmWrite(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, std::ostream& sink) const override
    { Encoding e{op0, op1, crn, crm, op2}; Disasm(e, sink << (rt == 31 ? "" : "<illegal write>")); }
    void DisasmRead(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, std::ostream& sink) const override
    { Encoding e{op0, op1, crn, crm, op2}; Disasm(e, sink << "<illegal read>"); }
    /* TODO: most of this instructions (if not all), doesn't accept rt != 31, and we cannot test that for now */
    virtual void Write(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu, U64 value) const override
    { /* Most of this operations have no side effect on instruction-level simulations. Thus, we provide a default empty implementation. */ }
  };

  struct IDSysReg : public BaseSysReg
  {
    void CheckReadAccess(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu) const
    {
      if (cpu.pstate.GetEL() == 0)
        {
          if (cpu.FEAT_IDST)
            throw AArch64::SystemAccessTrapException(op0, op1, crn, crm, op2, rt, /* direction */ 1);
          else
            throw AArch64::UndefinedInstructionException();
        }
    }
  };

  switch (SYSENCODE( op0, op1, crn, crm, op2 ))
    {
    case SYSENCODE(0b11,0b011,0b0100,0b0010,0b000): // NZCV
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "NZCV"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "NZCV Condition flags"; }
          void Write(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu, U64 value) const override { cpu.nzcv = U8(value >> 28) & U8(0xf); }
          U64 Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu) const override { return U64(cpu.nzcv) << 28; }
        } x; return &x;
      }
      /***  Architectural hints instructions ***/
    case SYSENCODE( 0b00, 0b011, 0b0010, 0b0000, 0b001 ):
      {
        static struct : public WOOpSysReg
        {
          void Disasm(Encoding, std::ostream& sink) const override { sink << "yield"; }
        } x; return &x;
      }

    case SYSENCODE( 0b00, 0b011, 0b0010, 0b0000, 0b010 ):
      {
        static struct : public WOOpSysReg {
          void Disasm(Encoding, std::ostream& sink) const override { sink << "wfe\t; Wait For Event"; }
        } x; return &x;
      }

    case SYSENCODE( 0b00, 0b011, 0b0010, 0b0000, 0b011 ):
      {
        static struct : public WOOpSysReg {
          void Disasm(Encoding, std::ostream& sink) const override { sink << "wfi\t; Wait For Interrupt"; }
          void Write(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu, U64 value) const override { cpu.notify(1,&AArch64::wfi); }
        } x; return &x;
      }

    case SYSENCODE( 0b00, 0b011, 0b0010, 0b0000, 0b100 ):
      {
        static struct : public WOOpSysReg {
          void Disasm(Encoding, std::ostream& sink) const override { sink << "sev\t; Send Event"; }
        } x; return &x;
      }

    case SYSENCODE( 0b00, 0b011, 0b0010, 0b0000, 0b101 ):
      {
        static struct : public WOOpSysReg {
          void Disasm(Encoding, std::ostream& sink) const override { sink << "sevl\t; Send Event Local"; }
          // void Write(uint8_t a, uint8_t b, uint8_t c, uint8_t d, uint8_t e, AArch64& cpu, U64 v) const override { BaseSysReg::Write(a,b,c,d,e,cpu,v); }
        } x; return &x;
      }

    case SYSENCODE( 0b00, 0b011, 0b0010, 0b0000, 0b111 ):
      {
        static struct : public WOOpSysReg {
          void Disasm(Encoding, std::ostream& sink) const override { sink << "xpaclri\t; Strip Pointer Authentication Code from LR"; }
        } x; return &x;
      }

    case SYSENCODE( 0b00, 0b011, 0b0010, 0b0010, 0b100 ):
      {
        static struct : public WOOpSysReg {
          void Disasm(Encoding, std::ostream& sink) const override { sink << "csdb\t; Consumption of Speculative Data Barrier"; }
        } x; return &x;
      } break;

    case SYSENCODE( 0b00, 0b011, 0b0010, 0b0011, 0b001 ):
      {
        static struct : public WOOpSysReg {
          void Disasm(Encoding, std::ostream& sink) const override { sink << "paciasp\t; Pointer Authentication Code for Instruction address"; }
        } x; return &x;
      } break;

    case SYSENCODE( 0b00, 0b011, 0b0010, 0b0011, 0b101 ):
      {
        static struct : public WOOpSysReg {
          void Disasm(Encoding, std::ostream& sink) const override { sink << "autiasp\t; Authenticate Instruction address, using key A"; }
        } x; return &x;
      } break;

    case SYSENCODE( 0b00, 0b011, 0b0010, 0b0100, 0b000 ):
    case SYSENCODE( 0b00, 0b011, 0b0010, 0b0100, 0b010 ):
    case SYSENCODE( 0b00, 0b011, 0b0010, 0b0100, 0b100 ):
    case SYSENCODE( 0b00, 0b011, 0b0010, 0b0100, 0b110 ):
      {
        static struct : public WOOpSysReg
        {
          void Disasm(Encoding e, std::ostream& sink) const override
          {
            sink << "bti\t" << (e.op2 & 4 ? "j" : "") << (e.op2 & 2 ? "c" : "") << "; Branch Target Identification";
          }
        } x; return &x;
      } break;


      /*** Instruction Cache Maintenance ***/
    case SYSENCODE( 0b01, 0b000, 0b0111, 0b0101, 0b000 ):
      {
        static struct : public ICSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "iallu"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Invalidate all instruction caches to Point of Unification"; }
          void Write(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu, U64 addr) const override
          {
            if (cpu.pstate.GetEL() == 0)
              throw AArch64::UndefinedInstructionException();

            cpu.ipb.invalidate();
          }
        } x; return &x;
      } break;

    case SYSENCODE( 0b01, 0b000, 0b0111, 0b0001, 0b000 ):
      {
        static struct : public ICSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ialluis"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Invalidate all instruction caches in Inner Shareable domain to Point of Unification"; }
          void Write(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu, U64 addr) const override
          {
            if (cpu.pstate.GetEL() == 0)
              throw AArch64::UndefinedInstructionException();

            cpu.ipb.invalidate();
          }
        } x; return &x;
      } break;

    case SYSENCODE( 0b01, 0b011, 0b0111, 0b0101, 0b001 ):
      {
        static struct : public ICSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ivau"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Invalidate instruction cache by address to Point of Unification"; }
          void Write(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu, U64 addr) const override
          {
            cpu.untaint(AddrTV(), addr);
            cpu.ipb.invalidate(); /* systematic invalidation is cheaper than va translation ;-) */
          }
        } x; return &x;
      } break;

      /*** Data Cache Maintenance ***/
    case SYSENCODE(0b01,0b011,0b0111,0b0100,0b001):
      {
        static struct : public DCSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "zva"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Zero data cache by address"; }
          void Write(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu, U64 addr) const override
          {
            uint64_t const zsize = 4 << cpu.ZID;
            uint64_t const vaddr = cpu.untaint(AddrTV(), addr & U64(-zsize));
            // ZID value should be so MMU page bounds are not crossed
            MMU::TLB::Entry entry(vaddr);
            cpu.translate_address(entry, cpu.pstate.GetEL(), mem_acc_type::write);
            cpu.dbgmon.MemWrite(cpu.pstate.GetEL(), vaddr, zsize);
            uint8_t buffer[zsize];
            std::fill(&buffer[0], &buffer[zsize], uint8_t(0));
            if (cpu.get_page(entry.pa).write(entry.pa, &buffer[0], &buffer[0], zsize) != zsize)
              { struct Bad {}; raise( Bad () ); }
          }
        } x; return &x;
      } break;

    case SYSENCODE(0b01,0b000,0b0111,0b1110,0b010):
      {
        static struct : public DCSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "cisw"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Clean and Invalidate data cache by set/way"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b01,0b000,0b0111,0b1010,0b010):
      {
        static struct : public DCSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "csw"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Clean data cache by set/way"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b01,0b011,0b0111,0b1010,0b001):
      {
        static struct : public DCSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "cvac"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Clean data cache by address to Point of Coherency"; }
          void Write(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu, U64 addr) const override
          { uint64_t vaddr = cpu.untaint(AddrTV(), addr); /* No caches, so normally nothing to do... */ cpu.dbgmon.CacheMaintenance(cpu.pstate.GetEL(), vaddr); }
        } x; return &x;
      } break;

    case SYSENCODE(0b01,0b011,0b0111,0b1011,0b001):
      {
        static struct : public DCSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "cvau"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Clean data cache by address to Point of Unification"; }
          void Write(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu, U64 addr) const override
          { uint64_t vaddr = cpu.untaint(AddrTV(), addr); /* No caches, so normally nothing to do... */ cpu.dbgmon.CacheMaintenance(cpu.pstate.GetEL(), vaddr); }
        } x; return &x;
      } break;

    case SYSENCODE(0b01,0b000,0b0111,0b0110,0b010):
      {
        static struct : public DCSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "isw"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Invalidate data cache by set/way"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b01,0b000,0b0111,0b0110,0b001):
      {
        static struct : public DCSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ivac"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Invalidate data cache by address to Point of Coherency"; }
          void Write(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu, U64 addr) const override
          { uint64_t vaddr = cpu.untaint(AddrTV(), addr); /* No caches, so normally nothing to do... */ cpu.dbgmon.CacheMaintenance(cpu.pstate.GetEL(), vaddr); }
        } x; return &x;
      } break;

    case SYSENCODE(0b01,0b011,0b0111,0b1110,0b001):
      {
        static struct : public DCSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "civac"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Clean and Invalidate data cache by address to Point of Coherency"; }
          void Write(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu, U64 addr) const override
          { uint64_t vaddr = cpu.untaint(AddrTV(), addr); /* No caches, so normally nothing to do... */ cpu.dbgmon.CacheMaintenance(cpu.pstate.GetEL(), vaddr); }
        } x; return &x;
      } break;

      /*** TLB Maintenance ***/
    case SYSENCODE(0b01,0b100,0b1000,0b0111,0b100):
      {
        static struct : public TLBISysReg {
          bool hasArg(Encoding) const override { return false; }
          void Name(Encoding, std::ostream& sink) const override { sink << "alle1"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b01,0b100,0b1000,0b0011,0b100):
      {
        static struct : public TLBISysReg {
          bool hasArg(Encoding) const override { return false; }
          void Name(Encoding, std::ostream& sink) const override { sink << "alle1is"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b01,0b100,0b1000,0b0111,0b000):
      {
        static struct : public TLBISysReg {
          bool hasArg(Encoding) const override { return false; }
          void Name(Encoding, std::ostream& sink) const override { sink << "alle2"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b01,0b100,0b1000,0b0011,0b000):
      {
        static struct : public TLBISysReg {
          bool hasArg(Encoding) const override { return false; }
          void Name(Encoding, std::ostream& sink) const override { sink << "alle2is"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b01,0b110,0b1000,0b0111,0b000):
      {
        static struct : public TLBISysReg {
          bool hasArg(Encoding) const override { return false; }
          void Name(Encoding, std::ostream& sink) const override { sink << "alle3"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b01,0b110,0b1000,0b0011,0b000):
      {
        static struct : public TLBISysReg {
          bool hasArg(Encoding) const override { return false; }
          void Name(Encoding, std::ostream& sink) const override { sink << "alle3is"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b01,0b100,0b1000,0b0100,0b001):
      {
        static struct : public TLBISysReg {
          bool hasArg(Encoding) const override { return true; }
          void Name(Encoding, std::ostream& sink) const override { sink << "ipas2e1"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b01,0b100,0b1000,0b0000,0b001):
      {
        static struct : public TLBISysReg {
          bool hasArg(Encoding) const override { return true; }
          void Name(Encoding, std::ostream& sink) const override { sink << "ipas2e1is"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b01,0b100,0b1000,0b0100,0b101):
      {
        static struct : public TLBISysReg {
          bool hasArg(Encoding) const override { return true; }
          void Name(Encoding, std::ostream& sink) const override { sink << "ipas2le1"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b01,0b100,0b1000,0b0000,0b101):
      {
        static struct : public TLBISysReg {
          bool hasArg(Encoding) const override { return true; }
          void Name(Encoding, std::ostream& sink) const override { sink << "ipas2le1is"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b01,0b100,0b1000,0b0111,0b001):
      {
        static struct : public TLBISysReg {
          bool hasArg(Encoding) const override { return true; }
          void Name(Encoding, std::ostream& sink) const override { sink << "vae2"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b01,0b100,0b1000,0b0011,0b001):
      {
        static struct : public TLBISysReg {
          bool hasArg(Encoding) const override { return true; }
          void Name(Encoding, std::ostream& sink) const override { sink << "vae2is"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b01,0b110,0b1000,0b0111,0b001):
      {
        static struct : public TLBISysReg {
          bool hasArg(Encoding) const override { return true; }
          void Name(Encoding, std::ostream& sink) const override { sink << "vae3"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b01,0b110,0b1000,0b0011,0b001):
      {
        static struct : public TLBISysReg {
          bool hasArg(Encoding) const override { return true; }
          void Name(Encoding, std::ostream& sink) const override { sink << "vae3is"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b01,0b100,0b1000,0b0111,0b101):
      {
        static struct : public TLBISysReg {
          bool hasArg(Encoding) const override { return true; }
          void Name(Encoding, std::ostream& sink) const override { sink << "vale2"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b01,0b100,0b1000,0b0011,0b101):
      {
        static struct : public TLBISysReg {
          bool hasArg(Encoding) const override { return true; }
          void Name(Encoding, std::ostream& sink) const override { sink << "vale2is"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b01,0b110,0b1000,0b0111,0b101):
      {
        static struct : public TLBISysReg {
          bool hasArg(Encoding) const override { return true; }
          void Name(Encoding, std::ostream& sink) const override { sink << "vale3"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b01,0b110,0b1000,0b0011,0b101):
      {
        static struct : public TLBISysReg {
          bool hasArg(Encoding) const override { return true; }
          void Name(Encoding, std::ostream& sink) const override { sink << "vale3is"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b01,0b000,0b1000,0b0011,0b000): // TLBI VMALLE1IS
    case SYSENCODE(0b01,0b000,0b1000,0b0111,0b000): // TLBI VMALLE1
    case SYSENCODE(0b01,0b000,0b1000,0b0011,0b001): // TLBI VA1IS
    case SYSENCODE(0b01,0b000,0b1000,0b0111,0b001): // TLBI VA1
    case SYSENCODE(0b01,0b000,0b1000,0b0011,0b010): // TLBI ASIDE1IS
    case SYSENCODE(0b01,0b000,0b1000,0b0111,0b010): // TLBI ASIDE1
    case SYSENCODE(0b01,0b000,0b1000,0b0011,0b011): // TLBI VAAE1IS
    case SYSENCODE(0b01,0b000,0b1000,0b0111,0b011): // TLBI VAAE1
    case SYSENCODE(0b01,0b000,0b1000,0b0011,0b101): // TLBI VALE1IS
    case SYSENCODE(0b01,0b000,0b1000,0b0111,0b101): // TLBI VALE1
    case SYSENCODE(0b01,0b000,0b1000,0b0011,0b111): // TLBI VAALE1IS
    case SYSENCODE(0b01,0b000,0b1000,0b0111,0b111): // TLBI VAALE1
      {
        static struct : public TLBISysReg
        {
          bool hasArg(Encoding e) const override { return e.op2 != 0; }
          void Name(Encoding e, std::ostream& sink) const override
          {
            switch (e.op2&3) // <type>
              {
              case 0: sink << "vmall"; break;
              case 1: sink << "va"; break;
              case 2: sink << "asid"; break;
              case 3: sink << "vaa"; break;
              }
            if (e.op2&4) // <last-level>
              sink << "l";
            sink << "e1";
            if (not (e.crm&4)) // <inner-shareable>
              sink << "is";
          }
          void Describe(Encoding e, std::ostream& sink) const override
          {
            sink << "TLB Invalidate ";
            switch (e.op2&3) // <type>
              {
              case 0: sink << "by VMID, All entries at stage 1"; break;
              case 1: sink << "by VA"; break;
              case 2: sink << "by ASID"; break;
              case 3: sink << "by VA, All ASID"; break;
              }

            if (e.op2&4) // <last-level>
              sink << ", Last level";
            sink << ", EL1";
            if (not (e.crm&4)) // <inner-shareable>)
              sink << ", Inner Shareable";
          }
          virtual void Write(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu, U64 arg) const override
          {
            if (cpu.pstate.GetEL() == 0)
              throw AArch64::UndefinedInstructionException();

            cpu.mmu.tlb.Invalidate(cpu, crm&4, op2&4, op2&3, arg);
            cpu.ipb.invalidate();
          }
        } x; return &x;
      } break;

    case SYSENCODE(0b01,0b100,0b1000,0b0111,0b110):
      {
        static struct : public TLBISysReg {
          bool hasArg(Encoding) const override { return false; }
          void Name(Encoding, std::ostream& sink) const override { sink << "vmalls12e1"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b01,0b100,0b1000,0b0011,0b110):
      {
        static struct : public TLBISysReg {
          bool hasArg(Encoding) const override { return false; }
          void Name(Encoding, std::ostream& sink) const override { sink << "vmalls12e1is"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0100,0b0010,0b010): // CurrentEL, Current Exception Level
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "CurrentEL"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Current Exception Level"; }
          U64 Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu) const override
          {
            if (cpu.pstate.GetEL() == 0)
              throw AArch64::UndefinedInstructionException();

            return U64(cpu.pstate.GetEL() << 2);
            //return cpu.PartlyDefined<uint64_t>(cpu.pstate.EL << 2, ~uint64_t(0b11 << 2));
          }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0100,0b0010,0b011): // PAN
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "PAN"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Privileged Access Never"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0100,0b0010,0b100): // UAO
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "UAO"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "User Access Override"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0100,0b0011,0b000): // ALLINT
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "ALLINT"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "All Interrupt Mask Bit"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1101,0b0000,0b101): // ACCDATA_EL1 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "ACCDATA_EL1"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Accelerator Data"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1101,0b0000,0b111): // SCXTNUM_EL1 (RW)
    case SYSENCODE(0b11,0b101,0b1101,0b0000,0b111): // SCXTNUM_EL1 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "SCXTNUM_EL1"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "EL1 Read/Write Software Context Number"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1101,0b0000,0b111): // SCXTNUM_EL2 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "SCXTNUM_EL2"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "EL2 Read/Write Software Context Number"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b110,0b1101,0b0000,0b111): // SCXTNUM_EL3 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "SCXTNUM_EL3"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "EL3 Read/Write Software Context Number"; }
        } x; return &x;
      } break;
      /*** Adress translations ***/

    case SYSENCODE(0b01,0b100,0b0111,0b1000,0b110):
      {
        static struct : public ATSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "s12e0r"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Performs stage 12 address translation, with permissions as if reading from EL0"; }
        } x; return &x;
      } break;


    case SYSENCODE(0b01,0b100,0b0111,0b1000,0b111):
      {
        static struct : public ATSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "s12e0w"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Performs stage 12 address translation, with permissions as if writing from EL0"; }
        } x; return &x;
      } break;


    case SYSENCODE(0b01,0b100,0b0111,0b1000,0b100):
      {
        static struct : public ATSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "s12e1r"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Performs stage 12 address translation, with permissions as if reading from EL1"; }
        } x; return &x;
      } break;


    case SYSENCODE(0b01,0b100,0b0111,0b1000,0b101):
      {
        static struct : public ATSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "s12e1w"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Performs stage 12 address translation, with permissions as if writing from EL1"; }
        } x; return &x;
      } break;


    case SYSENCODE(0b01,0b000,0b0111,0b1000,0b010):
      {
        static struct : public ATSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "s1e0r"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Performs stage 1 address translation, with permissions as if reading from EL0"; }
        } x; return &x;
      } break;


    case SYSENCODE(0b01,0b000,0b0111,0b1000,0b011):
      {
        static struct : public ATSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "s1e0w"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Performs stage 1 address translation, with permissions as if writing from EL0"; }
        } x; return &x;
      } break;


    case SYSENCODE(0b01,0b000,0b0111,0b1000,0b000):
    case SYSENCODE(0b01,0b000,0b0111,0b1000,0b001):
      {
        static struct : public ATSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "s1e1" << (e.op2 ? 'w' : 'r'); }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Stage 1 address translation, as if " << (e.op2 ? "writing to" : "reading from") << " EL1"; }
          void Write(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu, U64 addr) const override
          {
            if (cpu.pstate.GetEL() == 0)
              throw AArch64::UndefinedInstructionException();

            try
              {
                MMU::TLB::Entry entry(cpu.untaint(AddrTV(), addr));
                cpu.translate_address(entry, cpu.pstate.GetEL(), op2 ? mem_acc_type::write : mem_acc_type::read);
                cpu.el1.PAR = cpu.PartlyDefined<uint64_t>(entry.pa & 0x000ffffffffff000ull, 0xfff0000000000380ull);
              }
            catch (AArch64::DataAbort const& x)
              {
                cpu.el1.PAR = U64(0x801 | (x.type.EncodeLDFSC(x.level) << 1));
              }
          }
        } x; return &x;
      } break;

    case SYSENCODE(0b01,0b100,0b0111,0b1000,0b000):
      {
        static struct : public ATSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "s1e2r"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Performs stage 1 address translation, with permissions as if reading from EL2"; }
        } x; return &x;
      } break;


    case SYSENCODE(0b01,0b100,0b0111,0b1000,0b001):
      {
        static struct : public ATSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "s1e2w"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Performs stage 1 address translation, with permissions as if writing from EL2"; }
        } x; return &x;
      } break;


    case SYSENCODE(0b01,0b110,0b0111,0b1000,0b000):
      {
        static struct : public ATSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "s1e3r"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Performs stage 1 address translation, with permissions as if reading from EL3"; }
        } x; return &x;
      } break;


    case SYSENCODE(0b01,0b110,0b0111,0b1000,0b001):
      {
        static struct : public ATSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "s1e3w"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Performs stage 1 address translation, with permissions as if writing from EL3"; }
        } x; return &x;
      } break;


      /* ===== */

    case SYSENCODE(0b11,0b011,0b0100,0b0010,0b001): // DAIF, Interrupt Mask Bits
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "DAIF"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Interrupt Mask Bits"; }
          U64  Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu) const override
          {
            if (cpu.pstate.GetEL() == 0)
              {
                bool uma = unisim::util::arithmetic::BitField<9,1>().Get(cpu.get_el(1).SCTLR);
                if (!uma)
                  throw AArch64::SystemAccessTrapException(op0, op1, crn, crm, op2, rt, /* direction */ 1);
              }

            return cpu.pstate.GetDAIF();
          }
          void Write(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu, U64 value) const override
          {
            if (cpu.pstate.GetEL() == 0)
              {
                bool uma = unisim::util::arithmetic::BitField<9,1>().Get(cpu.get_el(1).SCTLR);
                if (!uma)
                  throw AArch64::SystemAccessTrapException(op0, op1, crn, crm, op2, rt, /* direction */ 0);
              }

            cpu.pstate.SetDAIF(cpu,value);
          }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b0100,0b0010,0b010): // SVCR
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "SVCR"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "SMCR_EL1, or SMCR_EL2 System registers to EL2, when EL2 is enabled in the current"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b0100,0b0010,0b101): // DIT
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "DIT"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Data Independent Timing"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b0100,0b0010,0b110): // SSBS
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "SSBS"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Speculative Store Bypass Safe"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b0100,0b0010,0b111): // TCO
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "TCO"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Tag Check Override"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b0100,0b0100,0b000): // FPCR, Floating-point Control Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "FPCR"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Floating-point Control Register"; }
          U64  Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu) const override { return U64(cpu.fpcr); }
          void Write(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu, U64 value) const override { cpu.fpcr = U32(value) & U32(AArch64::FPCR_MASK); }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b0100,0b0100,0b001): // FPSR, Floating-point Status Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "FPSR"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Floating-point Status Register"; }
          U64  Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu) const override { return U64(cpu.fpsr); }
          void Write(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu, U64 value) const override { cpu.fpsr = U32(value) & U32(AArch64::FPSR_MASK); }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0100,0b0001,0b000): // SP_EL0, Stack Pointer (EL0)
    case SYSENCODE(0b11,0b100,0b0100,0b0001,0b000): // SP_EL1, Stack Pointer (EL1)
    case SYSENCODE(0b11,0b110,0b0100,0b0001,0b000): // SP_EL2, Stack Pointer (EL2)
      {
        static struct : public BaseSysReg {
          U64& GetSP(AArch64& cpu, unsigned op1) const
          {
            unsigned lvl = GetExceptionLevel(op1) - 1;
            if (cpu.pstate.GetSP() == 0 and lvl == 0)
              throw AArch64::UndefinedInstructionException();
            return cpu.sp_el[lvl];
          }
          void Name(Encoding e, std::ostream& sink) const override { sink << "SP_EL" << (GetExceptionLevel(e.op1)-1); }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Stack Pointer (EL" << (GetExceptionLevel(e.op1)-1) << ")"; }
          U64  Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu) const override { return GetSP(cpu, op1); }
          void Write(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu, U64 value) const override { GetSP(cpu, op1) = value; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0100,0b0010,0b000): // SPSel
      {
        static struct : public PStateSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "SPSel"; }
          virtual U64 Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu) const override
          {
            if (cpu.pstate.GetEL() == 0)
              throw AArch64::UndefinedInstructionException();

            return U64(cpu.pstate.GetSP());
          }
          virtual void Write(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu, U64 value) const override
          {
            if (cpu.pstate.GetEL() == 0)
              throw AArch64::UndefinedInstructionException();

            struct Bad {}; cpu.pstate.SetSP(cpu, cpu.untaint(Bad(), value & U64(1)));
          }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0001,0b0000,0b001): // 2.1: ACTLR_EL1, Auxiliary Control Register (EL1)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ACTLR_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Auxiliary Control Register (EL1)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b0001,0b0000,0b001): // 2.2: ACTLR_EL2, Auxiliary Control Register (EL2)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ACTLR_EL2"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Auxiliary Control Register (EL2)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b0001,0b0000,0b011): // SCTLR2_EL2 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "SCTLR2_EL2"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "System Control Register (EL2)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b110,0b0001,0b0000,0b011): // SCTLR2_EL3 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "SCTLR2_EL3"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "System Control Register (EL3)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b110,0b0001,0b0000,0b001): // 2.3: ACTLR_EL3, Auxiliary Control Register (EL3)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ACTLR_EL3"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Auxiliary Control Register (EL3)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0101,0b0001,0b000): // 2.4: AFSR0_EL1, Auxiliary Fault Status Register 0 (EL1)
    case SYSENCODE(0b11,0b101,0b0101,0b0001,0b000): // AFSR0_EL1 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "AFSR0_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Auxiliary Fault Status Register 0 (EL1)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b0101,0b0001,0b000): // 2.5: AFSR0_EL2, Auxiliary Fault Status Register 0 (EL2)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "AFSR0_EL2"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Auxiliary Fault Status Register 0 (EL2)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b110,0b0101,0b0001,0b000): // 2.6: AFSR0_EL3, Auxiliary Fault Status Register 0 (EL3)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "AFSR0_EL3"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Auxiliary Fault Status Register 0 (EL3)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0101,0b0001,0b001): // 2.7: AFSR1_EL1, Auxiliary Fault Status Register 1 (EL1)
    case SYSENCODE(0b11,0b101,0b0101,0b0001,0b001): // AFSR1_EL1 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "AFSR1_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Auxiliary Fault Status Register 1 (EL1)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b0101,0b0001,0b001): // 2.8: AFSR1_EL2, Auxiliary Fault Status Register 1 (EL2)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "AFSR1_EL2"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Auxiliary Fault Status Register 1 (EL2)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b110,0b0101,0b0001,0b001): // 2.9: AFSR1_EL3, Auxiliary Fault Status Register 1 (EL3)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "AFSR1_EL3"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Auxiliary Fault Status Register 1 (EL3)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b001,0b0000,0b0000,0b111): // 2.10: AIDR_EL1, Auxiliary ID Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "AIDR_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Auxiliary ID Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1010,0b0011,0b000): // 2.11: AMAIR_EL1, Auxiliary Memory Attribute Indirection Register (EL1)
    case SYSENCODE(0b11,0b101,0b1010,0b0011,0b000): // AMAIR_EL1 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "AMAIR_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Auxiliary Memory Attribute Indirection Register (EL1)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1010,0b0011,0b000): // 2.12: AMAIR_EL2, Auxiliary Memory Attribute Indirection Register (EL2)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "AMAIR_EL2"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Auxiliary Memory Attribute Indirection Register (EL2)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b110,0b1010,0b0011,0b000): // 2.13: AMAIR_EL3, Auxiliary Memory Attribute Indirection Register (EL3)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "AMAIR_EL3"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Auxiliary Memory Attribute Indirection Register (EL3)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1010,0b0100,0b000): // LORSA_EL1 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "LORSA_EL1"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "LORegion Start Address (EL1)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1010,0b0100,0b001): // LOREA_EL1 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "LOREA_EL1"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "LORegion End Address (EL1)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1010,0b0100,0b010): // LORN_EL1 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "LORN_EL1"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "LORegion Number (EL1)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1010,0b0100,0b011): // LORC_EL1 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "LORC_EL1"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "LORegion Control (EL1)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1010,0b0100,0b100): // MPAMIDR_EL1 (RO)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "MPAMIDR_EL1"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "MPAMIDR_EL1"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1010,0b0100,0b111): // LORID_EL1 (RO)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "LORID_EL1"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "LORegionID (EL1)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1010,0b0101,0b000): // MPAM1_EL1 (RW)
    case SYSENCODE(0b11,0b101,0b1010,0b0101,0b000): // MPAM1_EL1 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "MPAM1_EL1"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "MPAM1_EL1"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1010,0b0101,0b001): // MPAM0_EL1 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "MPAM0_EL1"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "MPAM0_EL1"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b001,0b0000,0b0000,0b000): // 2.14: CCSIDR_EL1, Current Cache Size ID Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "CCSIDR_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Current Cache Size ID Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b001,0b0000,0b0000,0b001): // 2.15: CLIDR_EL1, Cache Level ID Register
      {
        static struct : public IDSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "CLIDR_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Cache Level ID Register"; }
          U64 Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu) const override
          {
            CheckReadAccess(op0, op1, crn, crm, op2, rt, cpu);

            return
               U64(0b000) << 30| // ICB, Inner cache boundary
               U64(0b001) << 27| // LoUU, Level of Unification Uniprocessor
               U64(0b010) << 24| // LoC, Level of Coherence
               U64(0b001) << 21| // LoUIS, Level of Unification Inner Shareable
               U64(0b000) << 18| // Cache Type #7
               U64(0b000) << 15| // Cache Type #6
               U64(0b000) << 12| // Cache Type #2
               U64(0b000) <<  9| // Cache Type #2
               U64(0b000) <<  6| // Cache Type #2
               U64(0b100) <<  3| // Cache Type #2
               U64(0b011) <<  0; // Cache Type #1
          }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b001,0b0000,0b0000,0b010): // CCSIDR2_EL1 (RO)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "CCSIDR2_EL1"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Current Cache Size ID Register 2"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b001,0b0000,0b0000,0b100): // GMID_EL1 (RO)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "GMID_EL1"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Multiple tag transfer ID register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b001,0b0000,0b0000,0b110): // SMIDR_EL1 (RO)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "SMIDR_EL1"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Streaming Mode Identification Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1101,0b0000,0b001): // 2.16: CONTEXTIDR_EL1, Context ID Register
    case SYSENCODE(0b11,0b101,0b1101,0b0000,0b001): // CONTEXTIDR_EL1 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "CONTEXTIDR_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Context ID Register"; }
          void Write(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu, U64 value) const override
          {
            if (cpu.pstate.GetEL() == 0)
              throw AArch64::UndefinedInstructionException();

            struct Bad {}; cpu.dbgmon.SetCONTEXTIDR_EL1(cpu.untaint(Bad(), value));
          }
          U64 Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu) const override
          {
            if (cpu.pstate.GetEL() == 0)
              throw AArch64::UndefinedInstructionException();

            return U64(cpu.dbgmon.CONTEXTIDR_EL1());
          }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1101,0b0000,0b001): // CONTEXTIDR_EL2 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "CONTEXTIDR_EL2"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Context ID Register (EL2)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0001,0b0000,0b010): // 2.17: CPACR_EL1, Architectural Feature Access Control Register
    case SYSENCODE(0b11,0b101,0b0001,0b0000,0b010): // CPACR_EL1 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "CPACR_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Architectural Feature Access Control Register"; }
          void Write(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu, U64 value) const override
          {
            if (cpu.pstate.GetEL() == 0)
              throw AArch64::UndefinedInstructionException();

            struct Bad {}; cpu.CPACR = cpu.untaint(Bad(), value);
          }
          U64 Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu) const override
          {
            if (cpu.pstate.GetEL() == 0)
              throw AArch64::UndefinedInstructionException();

            return U64(cpu.CPACR);
          }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0001,0b0000,0b011): // SCTLR2_EL1 (RW)
    case SYSENCODE(0b11,0b101,0b0001,0b0000,0b011): // SCTLR2_EL1 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "SCTLR2_EL1"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "SCTLR2_EL1, System Control Register (EL1)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0001,0b0000,0b101): // RGSR_EL1 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "RGSR_EL1"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Random Allocation Tag Seed Register."; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0001,0b0000,0b110): // GCR_EL1 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "GCR_EL1"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Tag Control Register."; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0001,0b0010,0b000): // ZCR_EL1 (RW)
    case SYSENCODE(0b11,0b101,0b0001,0b0010,0b000): // ZCR_EL1 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "ZCR_EL1"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "SVE Control Register (EL1)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0001,0b0010,0b001): // TRFCR_EL1 (RW)
    case SYSENCODE(0b11,0b101,0b0001,0b0010,0b001): // TRFCR_EL1 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "TRFCR_EL1"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Trace Filter Control Register (EL1)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0001,0b0010,0b100): // SMPRI_EL1 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "SMPRI_EL1"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Streaming Mode Priority Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0001,0b0010,0b110): // SMCR_EL1 (RW)
    case SYSENCODE(0b11,0b101,0b0001,0b0010,0b110): // SMCR_EL1 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "SMCR_EL1"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "SME Control Register (EL1)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b0001,0b0001,0b010): // 2.18: CPTR_EL2, Architectural Feature Trap Register (EL2)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "CPTR_EL2"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Architectural Feature Trap Register (EL2)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b110,0b0001,0b0001,0b010): // 2.19: CPTR_EL3, Architectural Feature Trap Register (EL3)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "CPTR_EL3"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Architectural Feature Trap Register (EL3)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b010,0b0000,0b0000,0b000): // 2.20: CSSELR_EL1, Cache Size Selection Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "CSSELR_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Cache Size Selection Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b0000,0b0000,0b001): // 2.21: CTR_EL0, Cache Type Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "CTR_EL0"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Cache Type Register"; }
          U64 Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu) const override
          {
            return
              U64(1)              << 31 | // RES1, Reserved.
              U64(0)              << 30 | // RES0, Reserved.
              U64(1)              << 29 | // DIC, Instruction cache invalidation requirements for data to instruction coherence (0 == required).
              U64(1)              << 28 | // IDC, Data cache clean requirements for instruction to data coherence (0 == required).
              U64(4)              << 24 | // CWG, Cache Writeback Granule.
              U64(ExcMon::ERG)    << 20 | // ERG, Exclusives Reservation Granule.
              U64(DminLine)       << 16 | // DminLine (16 words)
              U64(0b11)           << 14 | // L1Ip, Level 1 instruction cache policy (PIPT).
              U64(0b0000000000)   <<  4 | // RES0, Reserved.
              U64(IminLine)       <<  0;  // IminLine, (16 words)
          }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b0011,0b0000,0b000): // 2.22: DACR32_EL2, Domain Access Control Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "DACR32_EL2"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Domain Access Control Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b0011,0b0001,0b100): // HDFGRTR_EL2 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "HDFGRTR_EL2"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Hypervisor Debug Fine-Grained Read Trap Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b0011,0b0001,0b101): // HDFGWTR_EL2 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "HDFGWTR_EL2"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Hypervisor Debug Fine-Grained Write Trap Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b0011,0b0001,0b110): // HAFGRTR_EL2 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "HAFGRTR_EL2"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Hypervisor Activity Monitors Fine-Grained Read Trap Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b0000,0b0000,0b111): // 2.23: DCZID_EL0, Data Cache Zero ID register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "DCZID_EL0"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Data Cache Zero ID register"; }
          U64 Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu) const override
          {
            return U64((not unisim::component::cxx::processor::arm::vmsav8::sctlr::DZE.Get(cpu.el1.SCTLR)) << 4 | cpu.ZID);
          }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b101,0b0101,0b0010,0b000): // ESR_EL1 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "ESR_EL1"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Exception Syndrome Register (EL1)"; }
          U64  Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu) const override
          {
            if (cpu.pstate.GetEL() == 0)
              throw AArch64::UndefinedInstructionException();

            return U64(cpu.get_el(1).ESR);
          }
          void Write(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu, U64 value) const override
          {
            if (cpu.pstate.GetEL() == 0)
              throw AArch64::UndefinedInstructionException();

            cpu.get_el(1).ESR = U32(value);
          }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0101,0b0010,0b000): // 2.24: ESR_EL1, Exception Syndrome Register (EL1)
    case SYSENCODE(0b11,0b100,0b0101,0b0010,0b000): // 2.25: ESR_EL2, Exception Syndrome Register (EL2)
    case SYSENCODE(0b11,0b110,0b0101,0b0010,0b000): // 2.26: ESR_EL3, Exception Syndrome Register (EL3)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "ESR_EL" << GetExceptionLevel(e.op1); }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Exception Syndrome Register (EL" << GetExceptionLevel(e.op1) << ")"; }
          U64  Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu) const override
          {
            if (cpu.pstate.GetEL() == 0)
              throw AArch64::UndefinedInstructionException();

            return U64(cpu.get_el(GetExceptionLevel(op1)).ESR);
          }
          void Write(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu, U64 value) const override
          {
            if (cpu.pstate.GetEL() == 0)
              throw AArch64::UndefinedInstructionException();

            cpu.get_el(GetExceptionLevel(op1)).ESR = U32(value);
          }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b0101,0b0010,0b011): // VSESR_EL2 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "VSESR_EL2"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Virtual SError Exception Syndrome Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1100,0b0001,0b001): // VDISR_EL2 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "VDISR_EL2"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Virtual Deferred Interrupt Status Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b101,0b0110,0b0000,0b000): // FAR_EL1 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "FAR_EL1"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Fault Address Register (EL1)"; }
          U64  Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu) const override
          {
            if (cpu.pstate.GetEL() == 0)
              throw AArch64::UndefinedInstructionException();

            return U64(cpu.get_el(1).FAR);
          }
          void Write(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu, U64 value) const override
          {
            if (cpu.pstate.GetEL() == 0)
              throw AArch64::UndefinedInstructionException();

            struct Bad {}; cpu.get_el(1).FAR = cpu.untaint(Bad(), value);
          }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0110,0b0000,0b000): // 2.27: FAR_EL1, Fault Address Register (EL1)
    case SYSENCODE(0b11,0b100,0b0110,0b0000,0b000): // 2.28: FAR_EL2, Fault Address Register (EL2)
    case SYSENCODE(0b11,0b110,0b0110,0b0000,0b000): // 2.29: FAR_EL3, Fault Address Register (EL3)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "FAR_EL" << GetExceptionLevel(e.op1); }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Exception Syndrome Register (EL" << GetExceptionLevel(e.op1) << ")"; }
          U64  Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu) const override
          {
            if (cpu.pstate.GetEL() == 0)
              throw AArch64::UndefinedInstructionException();

            return U64(cpu.get_el(GetExceptionLevel(op1)).FAR);
          }
          void Write(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu, U64 value) const override
          {
            if (cpu.pstate.GetEL() == 0)
              throw AArch64::UndefinedInstructionException();

            struct Bad {}; cpu.get_el(GetExceptionLevel(op1)).FAR = cpu.untaint(Bad(), value);
          }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b110,0b0110,0b0000,0b101): // MFAR_EL3 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "MFAR_EL3"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Physical Fault Address Register (EL3)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b0101,0b0011,0b000): // 2.30: FPEXC32_EL2, Floating-point Exception Control register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "FPEXC32_EL2"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Floating-point Exception Control register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b0001,0b0001,0b111): // 2.31: HACR_EL2, Hypervisor Auxiliary Control Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "HACR_EL2"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Hypervisor Auxiliary Control Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b0001,0b0001,0b000): // 2.32: HCR_EL2, Hypervisor Configuration Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "HCR_EL2"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Hypervisor Configuration Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b0110,0b0000,0b100): // 2.33: HPFAR_EL2, Hypervisor IPA Fault Address Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "HPFAR_EL2"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Hypervisor IPA Fault Address Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b0001,0b0001,0b011): // 2.34: HSTR_EL2, Hypervisor System Trap Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "HSTR_EL2"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Hypervisor System Trap Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b0001,0b0001,0b100): // HFGRTR_EL2 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "HFGRTR_EL2"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Hypervisor Fine-Grained Read Trap Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b0001,0b0001,0b101): // HFGWTR_EL2 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "HFGWTR_EL2"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Hypervisor Fine-Grained Write Trap Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b0001,0b0001,0b110): // HFGITR_EL2 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "HFGITR_EL2"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Hypervisor Fine-Grained Instruction Trap Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b0001,0b0010,0b000): // ZCR_EL2 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "ZCR_EL2"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "SVE Control Register (EL2)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b110,0b0001,0b0010,0b000): // ZCR_EL3 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "ZCR_EL3"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "SVE Control Register (EL3)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b0001,0b0010,0b001): // TRFCR_EL2 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "TRFCR_EL2"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Trace Filter Control Register (EL2)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b0001,0b0010,0b010): // HCRX_EL2 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "HCRX_EL2"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Extended Hypervisor Configuration Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b0001,0b0010,0b101): // SMPRIMAP_EL2 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "SMPRIMAP_EL2"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Streaming Mode Priority Mapping Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b0001,0b0010,0b110): // SMCR_EL2 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "SMCR_EL2"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "SME Control Register (EL2)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b110,0b0001,0b0010,0b110): // SMCR_EL3 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "SMCR_EL3"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "SME Control Register (EL3)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b0001,0b0011,0b001): // SDER32_EL2 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "SDER32_EL2"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "AArch32 Secure Debug Enable Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0101,0b100): // 2.35: ID_AA64AFR0_EL1, AArch64 Auxiliary Feature Register 0
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ID_AA64AFR0_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "AArch64 Auxiliary Feature Register 0"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0101,0b101): // 2.36: ID_AA64AFR1_EL1, AArch64 Auxiliary Feature Register 1
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ID_AA64AFR1_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "AArch64 Auxiliary Feature Register 1"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0101,0b010): // Reserved, RAZ (RO)
    case SYSENCODE(0b11,0b000,0b0000,0b0101,0b011): // Reserved, RAZ (RO)
    case SYSENCODE(0b11,0b000,0b0000,0b0101,0b110): // Reserved, RAZ (RO)
    case SYSENCODE(0b11,0b000,0b0000,0b0101,0b111): // Reserved, RAZ (RO)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "-"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Reserved, RAZ"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0101,0b000): // 2.37: ID_AA64DFR0_EL1, AArch64 Debug Feature Register 0
      {
        static struct : public IDSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ID_AA64DFR0_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "AArch64 Debug Feature Register 0"; }
          U64 Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu) const override
          {
            CheckReadAccess(op0, op1, crn, crm, op2, rt, cpu);

            return
              U64(DebugMonitor::CTX_CMPS - 1) << 28 | // Number of hardware breakpoints - 1 that (the last ones) can be used for context matching
              U64(DebugMonitor::WRPS - 1)     << 20 | // Number of hardware watchpoints - 1
              U64(DebugMonitor::BRPS - 1)     << 12 | // Number of hardware breakpoints - 1
              U64(0x6);                               // The processor implements ARMv8 Debug architecture.
          }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0101,0b001): // 2.38: ID_AA64DFR1_EL1, AArch64 Debug Feature Register 1
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ID_AA64DFR1_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "AArch64 Debug Feature Register 1"; }
          U64 Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu) const override { cpu.TODO(); return U64(0); }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0110,0b000): // 2.39: ID_AA64ISAR0_EL1, AArch64 Instruction Set Attribute Register 0
      {
        static struct : public IDSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ID_AA64ISAR0_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "AArch64 Instruction Set Attribute Register 0"; }
          U64 Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu) const override
          {
            CheckReadAccess(op0, op1, crn, crm, op2, rt, cpu);

            return
              U64(cpu.FEAT_RNG ? 0b0001 : 0b0000)   << 60 | // Indicates support for Random Number instructions
              U64(0b0000)                           << 56 | // Indicates support for Outer shareable and TLB range maintenance instructions
              U64(0b0000)                           << 52 | // Indicates support for flag manipulation instructions
              U64(0b0000)                           << 48 | // Indicates support for FMLAL and FMLSL instructions
              U64(0b0000)                           << 44 | // Indicates support for Dot Product instructions
              U64(0b0000)                           << 40 | // Indicates support for SM4 instructions
              U64(0b0000)                           << 36 | // Indicates support for SM3 instructions
              U64(0b0000)                           << 32 | // Indicates support for SHA3 instructions
              U64(0b0000)                           << 28 | // Indicates support for SQRDMLAH and SQRDMLSH instructions
              cpu.PartlyDefined<uint64_t>(0,0b1111) << 24 | // Reserved, RES0.
              U64(cpu.FEAT_LSE ? 0b0010 : 0b0000)   << 20 | // Indicates support for Atomic instructions
              U64(cpu.FEAT_CRC32 ? 0b0001 : 0b0000) << 16 | // Indicates support for CRC32 instructions
              U64(0b0000)                           << 12 | // Indicates support for SHA2 instructions (TODO)
              U64(0b0000)                           <<  8 | // Indicates support for SHA1 instructions (TODO)
              U64(0b0000)                           <<  4 | // Indicates support for AES instructions (TODO)
              cpu.PartlyDefined<uint64_t>(0,0b1111) <<  0;  // Reserved, RES0.
          }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0110,0b001): // 2.40: ID_AA64ISAR1_EL1, AArch64 Instruction Set Attribute Register 1
      {
        static struct : public IDSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ID_AA64ISAR1_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "AArch64 Instruction Set Attribute Register 1"; }
          U64 Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu) const override
          {
            CheckReadAccess(op0, op1, crn, crm, op2, rt, cpu);

            return U64(0);
          }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0110,0b010): // 2.40: ID_AA64ISAR2_EL1, AArch64 Instruction Set Attribute Register 2
      {
        static struct : public IDSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ID_AA64ISAR2_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "AArch64 Instruction Set Attribute Register 2"; }
          U64 Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu) const override
          {
            CheckReadAccess(op0, op1, crn, crm, op2, rt, cpu);

            return U64(0);
          }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0110,0b011): // Reserved, RAZ (RO)
    case SYSENCODE(0b11,0b000,0b0000,0b0110,0b100): // Reserved, RAZ (RO)
    case SYSENCODE(0b11,0b000,0b0000,0b0110,0b101): // Reserved, RAZ (RO)
    case SYSENCODE(0b11,0b000,0b0000,0b0110,0b110): // Reserved, RAZ (RO)
    case SYSENCODE(0b11,0b000,0b0000,0b0110,0b111): // Reserved, RAZ (RO)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "-"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Reserved, RAZ"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0111,0b000): // 2.41: ID_AA64MMFR0_EL1, AArch64 Memory Model Feature Register 0
      {
        static struct : public IDSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ID_AA64MMFR0_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "AArch64 Memory Model Feature Register 0"; }
          U64 Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu) const override
          {
            CheckReadAccess(op0, op1, crn, crm, op2, rt, cpu);

            return
              U64(0)                << 32 | // RES0
              U64(0b0000)           << 28 | // TGran4, Support for 4 Kbyte memory translation granule size
              U64(0b0000)           << 24 | // TGran64, Support for 64 Kbyte memory translation granule size
              U64(0b0001)           << 20 | // TGran16, Support for 16 Kbyte memory translation granule size
              U64(0b0000)           << 16 | // BigEndEL0, Mixed-endian configuration support
              U64(0b0000)           << 12 | // SNSMem, Secure versus Non-secure Memory distinction
              U64(0b0000)           <<  8 | // BigEnd, Mixed-endian configuration support
              U64(0b0010)           <<  4 | // ASIDBits, Number of ASID bits
              U64(0b0101)           <<  0;  // PARange, Physical Address range supported
          }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0111,0b001): // 2.42: ID_AA64MMFR1_EL1, AArch64 Memory Model Feature Register 1
      {
        static struct : public IDSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ID_AA64MMFR1_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "AArch64 Memory Model Feature Register 1"; }
          U64 Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu) const override
          {
            CheckReadAccess(op0, op1, crn, crm, op2, rt, cpu);

            /* RES0, Reserved for future expansion of the information
             * about the implemented memory model and memory
             * management support in AArch64. */
            return U64(0);
          }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0111,0b010): // 2.42: ID_AA64MMFR2_EL1, AArch64 Memory Model Feature Register 2
      {
        static struct : public IDSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ID_AA64MMFR2_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "AArch64 Memory Model Feature Register 2"; }
          U64 Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu) const override
          {
            CheckReadAccess(op0, op1, crn, crm, op2, rt, cpu);

            /* RES0, Reserved for future expansion of the information
             * about the implemented memory model and memory
             * management support in AArch64. */
            return U64(cpu.FEAT_IDST ? 0b0001 : 0b0000) << 36; // All exceptions generated by an AArch64 read access to the feature ID space are reported by ESR_ELx.EC == 0x18.
          }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0111,0b011): // ID_AA64MMFR3_EL1 (RO)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "ID_AA64MMFR3_EL1"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "AArch64 Memory Model Feature Register 3"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0111,0b100): // ID_AA64MMFR4_EL1 (RO)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "ID_AA64MMFR4_EL1"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "AArch64 Memory Model Feature Register 4"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0111,0b101): // Reserved, RAZ (RO)
    case SYSENCODE(0b11,0b000,0b0000,0b0111,0b110): // Reserved, RAZ (RO)
    case SYSENCODE(0b11,0b000,0b0000,0b0111,0b111): // Reserved, RAZ (RO)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "-"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Reserved, RAZ"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0100,0b000): // 2.43: ID_AA64PFR0_EL1, AArch64 Processor Feature Register 0
      {
        static struct : public IDSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ID_AA64PFR0_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "AArch64 Processor Feature Register 0"; }
          U64 Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu) const override
          {
            CheckReadAccess(op0, op1, crn, crm, op2, rt, cpu);

            return
              U64(0b0000)                              << 60 | // Speculative use of faulting data
              U64(0b0000)                              << 56 | // Speculative use of out of context branch targets
              cpu.PartlyDefined<uint64_t>(0,0b1111)    << 52 | // RES0
              U64(0b0000)                              << 48 | // Data Independent Timing
              U64(0b0000)                              << 44 | // Indicates support for Activity Monitors Extension
              U64(0b0000)                              << 40 | // Indicates support for MPAM Extension
              U64(0b0000)                              << 36 | // Secure EL2
              U64(0b0000)                              << 32 | // Scalable Vector Extension
              U64(0b0000)                              << 28 | // RAS Extension version
              U64(0b0000)                              << 24 | // System register GIC CPU interface
              U64(cpu.FEAT_FP16 ? 0b0001 : 0b0000)     << 20 | // Advanced SIMD (implemented and also includes support for half-precision floating-point arithmetic)
              U64(cpu.FEAT_FP16 ? 0b0001 : 0b0000)     << 16 | // Floating-point (implemented and also includes support for half-precision floating-point arithmetic)
              U64(0b0000)                              << 12 | // EL3 Exception level handling (not implemented)
              U64(0b0001)                              <<  8 | // EL2 Exception level handling (executed in AArch64 state only)
              U64(0b0001)                              <<  4 | // EL1 Exception level handling (executed in AArch64 state only)
              U64(0b0001)                              <<  0;  // EL0 Exception level handling (executed in AArch64 state only)
          }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0100,0b001): // 2.44: ID_AA64PFR1_EL1, AArch64 Processor Feature Register 1
      {
        static struct : public IDSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ID_AA64PFR1_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "AArch64 Processor Feature Register 1"; }
          U64 Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu) const override
          {
            CheckReadAccess(op0, op1, crn, crm, op2, rt, cpu);

            return
              cpu.PartlyDefined<uint64_t>(0,-1) << 20 | // Reserved, RES0
              U64(0b0000)                      << 16 | // MPAM Extension fractional field
              U64(0b0000)                      << 12 | // RAS Extension fractional field
              U64(0b0000)                      <<  8 | // Support for the Memory Tagging Extension
              U64(0b0000)                      <<  4 | // Speculative Store Bypassing controls in AArch64 state
              U64(0b0000)                      <<  0;  // Branch Target Identification mechanism support
          }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0100,0b010): // ID_AA64PFR2_EL1 (RO)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "ID_AA64PFR2_EL1"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "AArch64 Processor Feature Register 2"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0100,0b100): // 2.44: ID_AA64ZFR0_EL1, SVE Feature ID register 0
      {
        static struct : public IDSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ID_AA64ZFR0_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "AArch64 SVE Processor Feature Register"; }
          U64 Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu) const override
          {
            CheckReadAccess(op0, op1, crn, crm, op2, rt, cpu);

            return U64(0);
          }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0100,0b101): // ID_AA64SMFR0_EL1, SME Feature ID register 0
      {
        static struct : public IDSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ID_AA64SMFR0_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "AArch64 SME Feature ID register 0"; }
          U64 Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu) const override
          {
            CheckReadAccess(op0, op1, crn, crm, op2, rt, cpu);

            return U64(0);
          }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0100,0b011): // Reserved, RAZ. (RO)
    case SYSENCODE(0b11,0b000,0b0000,0b0100,0b110): // Reserved, RAZ. (RO)
    case SYSENCODE(0b11,0b000,0b0000,0b0100,0b111): // Reserved, RAZ. (RO)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "-"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Reserved, RAZ"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0001,0b011): // 2.45: ID_AFR0_EL1, AArch32 Auxiliary Feature Register 0
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ID_AFR0_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "AArch32 Auxiliary Feature Register 0"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0001,0b010): // 2.46: ID_DFR0_EL1, AArch32 Debug Feature Register 0
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ID_DFR0_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "AArch32 Debug Feature Register 0"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0010,0b000): // 2.47: ID_ISAR0_EL1, AArch32 Instruction Set Attribute Register 0
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ID_ISAR0_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "AArch32 Instruction Set Attribute Register 0"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0010,0b001): // 2.48: ID_ISAR1_EL1, AArch32 Instruction Set Attribute Register 1
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ID_ISAR1_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "AArch32 Instruction Set Attribute Register 1"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0010,0b010): // 2.49: ID_ISAR2_EL1, AArch32 Instruction Set Attribute Register 2
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ID_ISAR2_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "AArch32 Instruction Set Attribute Register 2"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0010,0b011): // 2.50: ID_ISAR3_EL1, AArch32 Instruction Set Attribute Register 3
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ID_ISAR3_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "AArch32 Instruction Set Attribute Register 3"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0010,0b100): // 2.51: ID_ISAR4_EL1, AArch32 Instruction Set Attribute Register 4
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ID_ISAR4_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "AArch32 Instruction Set Attribute Register 4"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0010,0b101): // 2.52: ID_ISAR5_EL1, AArch32 Instruction Set Attribute Register 5
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ID_ISAR5_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "AArch32 Instruction Set Attribute Register 5"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0010,0b110): // ID_MMFR4_EL1 (RO)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "ID_MMFR4_EL1"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "AArch32 Memory Model Feature Register 4"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0010,0b111): // ID_ISAR6_EL1 (RO)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "ID_ISAR6_EL1"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "AArch32 Instruction Set Attribute Register 6"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0001,0b100): // 2.53: ID_MMFR0_EL1, AArch32 Memory Model Feature Register 0
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ID_MMFR0_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "AArch32 Memory Model Feature Register 0"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0001,0b101): // 2.54: ID_MMFR1_EL1, AArch32 Memory Model Feature Register 1
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ID_MMFR1_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "AArch32 Memory Model Feature Register 1"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0001,0b110): // 2.55: ID_MMFR2_EL1, AArch32 Memory Model Feature Register 2
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ID_MMFR2_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "AArch32 Memory Model Feature Register 2"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0001,0b111): // 2.56: ID_MMFR3_EL1, AArch32 Memory Model Feature Register 3
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ID_MMFR3_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "AArch32 Memory Model Feature Register 3"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0001,0b000): // 2.57: ID_PFR0_EL1, AArch32 Processor Feature Register 0
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ID_PFR0_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "AArch32 Processor Feature Register 0"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0001,0b001): // 2.58: ID_PFR1_EL1, AArch32 Processor Feature Register 1
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ID_PFR1_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "AArch32 Processor Feature Register 1"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b0101,0b0000,0b001): // 2.59: IFSR32_EL2, Instruction Fault Status Register (EL2)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "IFSR32_EL2"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Instruction Fault Status Register (EL2)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b0001,0b000): // 2.60: ISR_EL1, Interrupt Status Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ISR_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Interrupt Status Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b0001,0b001): // DISR_EL1 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "DISR_EL1"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Deferred Interrupt Status Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1010,0b0010,0b000): // 2.61: MAIR_EL1, Memory Attribute Indirection Register (EL1)
    case SYSENCODE(0b11,0b101,0b1010,0b0010,0b000): // MAIR_EL1 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "MAIR_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Memory Attribute Indirection Register (EL1)"; }
          void Write(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu, U64 value) const override
          {
            if (cpu.pstate.GetEL() == 0)
              throw AArch64::UndefinedInstructionException();

            struct Bad {}; cpu.mmu.MAIR_EL1 = cpu.untaint(Bad(), value);
          }
          U64 Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu) const override { return U64(cpu.mmu.MAIR_EL1); }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1010,0b0010,0b000): // 2.62: MAIR_EL2, Memory Attribute Indirection Register (EL2)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "MAIR_EL2"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Memory Attribute Indirection Register (EL2)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b110,0b1010,0b0010,0b000): // 2.63: MAIR_EL3, Memory Attribute Indirection Register (EL3)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "MAIR_EL3"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Memory Attribute Indirection Register (EL3)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0000,0b000): // 2.64: MIDR_EL1, Main ID Register
      {
        static struct : public IDSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "MIDR_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Main ID Register"; }
          U64 Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu) const override
          {
            CheckReadAccess(op0, op1, crn, crm, op2, rt, cpu);

            if (cpu.current_insn_addr == 0xffffffc010c94bc8)
              return U64(0xaceface5);
            cpu.TODO();
            return
              U64(0x41)             << 24 | // Implementer, The Implementer code (ARM LTD).
              U64(1)                << 20 | // Variant, An IMPLEMENTATION DEFINED variant number.
              U64(0xf)              << 16 | // Architecture (v8: Architectural features are individually identified in the ID_* registers)
              U64(0xd07)            <<  4 | // PartNum, An IMPLEMENTATION DEFINED primary part number for the device.
              U64(0)                <<  0;  // Revision, An IMPLEMENTATION DEFINED revision number for the device.
          }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0000,0b101): // 2.65: MPIDR_EL1, Multiprocessor Affinity Register
      {
        static struct : public IDSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "MPIDR_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Multiprocessor Affinity Register"; }
          U64 Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu) const override
          {
            CheckReadAccess(op0, op1, crn, crm, op2, rt, cpu);

            return U64(0)
              | cpu.PartlyDefined<uint64_t>(0, 0xffffff) << 40  // Reserved, RES0.
              | U64(0)                                   << 32  // Aff3, Affinity level 3.
              | cpu.PartlyDefined<uint64_t>(1, 1)        << 31  // Reserved, RES1.
              | U64(1)                                   << 30  // U, bit [30] Uniprocessor system.
              | cpu.PartlyDefined<uint64_t>(0, 0x1f)     << 25  // Bits [29:25] Reserved, RES0.
              | U64(0)                                   << 24  // MT, bit [24] Lowest level of affinity consists of multi-threaded logical processors.
              | U64(0)                                   << 16  // Aff2, Affinity level 2.
              | U64(0)                                   <<  8  // Aff1, Affinity level 1.
              | U64(0)                                   <<  0  // Aff0, Affinity level 0.
              ;
          }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0011,0b000): // 2.66: MVFR0_EL1, Media and VFP Feature Register 0
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "MVFR0_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Media and VFP Feature Register 0"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0011,0b001): // 2.67: MVFR1_EL1, Media and VFP Feature Register 1
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "MVFR1_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Media and VFP Feature Register 1"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0011,0b010): // 2.68: MVFR2_EL1, Media and VFP Feature Register 2
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "MVFR2_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Media and VFP Feature Register 2"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0011,0b100): // ID_PFR2_EL1 (RO)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "ID_PFR2_EL1"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "AArch32 Processor Feature Register 2"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0011,0b101): // ID_DFR1_EL1 (RO)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "ID_DFR1_EL1"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Debug Feature Register 1"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0011,0b110): // ID_MMFR5_EL1 (RO)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "ID_MMFR5_EL1"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "AArch32 Memory Model Feature Register 5"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0011,0b011): // Reserved, RAZ (RO)
    case SYSENCODE(0b11,0b000,0b0000,0b0011,0b111): // Reserved, RAZ (RO)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "-"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Reserved, RAZ"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0111,0b0100,0b000): // 2.69: PAR_EL1, Physical Address Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "PAR_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Physical Address Register"; }
          U64  Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu) const override { return U64(cpu.get_el(GetExceptionLevel(op1)).PAR); }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0000,0b110): // 2.70: REVIDR_EL1, Revision ID Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "REVIDR_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Revision ID Register"; }
          U64  Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu) const override { return U64(0); }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b0000,0b010): // 2.71: RMR_EL1, Reset Management Register (if EL2 and EL3 not implemented)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "RMR_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Reset Management Register (if EL2 and EL3 not implemented)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1100,0b0000,0b010): // 2.72: RMR_EL2, Reset Management Register (if EL3 not implemented)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "RMR_EL2"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Reset Management Register (if EL3 not implemented)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b110,0b1100,0b0000,0b010): // 2.73: RMR_EL3, Reset Management Register (if EL3 implemented)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "RMR_EL3"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Reset Management Register (if EL3 implemented)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b0000,0b001): // 2.74: RVBAR_EL1, Reset Vector Base Address Register (if EL2 and EL3 not implemented)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "RVBAR_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Reset Vector Base Address Register (if EL2 and EL3 not implemented)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1100,0b0000,0b001): // 2.75: RVBAR_EL2, Reset Vector Base Address Register (if EL3 not implemented)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "RVBAR_EL2"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Reset Vector Base Address Register (if EL3 not implemented)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b110,0b1100,0b0000,0b001): // 2.76: RVBAR_EL3, Reset Vector Base Address Register (if EL3 implemented)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "RVBAR_EL3"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Reset Vector Base Address Register (if EL3 implemented)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b110,0b0001,0b0001,0b000): // 2.78: SCR_EL3, Secure Configuration Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "SCR_EL3"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Secure Configuration Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0001,0b0000,0b000): // 2.79: SCTLR_EL1, System Control Register (EL1)
    case SYSENCODE(0b11,0b101,0b0001,0b0000,0b000): // SCTLR_EL1 (RW)
    case SYSENCODE(0b11,0b100,0b0001,0b0000,0b000): // 2.80: SCTLR_EL2, System Control Register (EL2)
    case SYSENCODE(0b11,0b110,0b0001,0b0000,0b000): // 2.81: SCTLR_EL3, System Control Register (EL3)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "SCTLR_EL" << GetExceptionLevel(e.op1); }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "System Control Register (EL" << GetExceptionLevel(e.op1) << ")"; }
          U64  Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu) const override
          {
            if (cpu.pstate.GetEL() == 0)
              throw AArch64::UndefinedInstructionException();

            return U64(cpu.get_el(GetExceptionLevel(op1)).SCTLR);
          }
          void Write(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu, U64 value) const override
          {
            if (cpu.pstate.GetEL() == 0)
              throw AArch64::UndefinedInstructionException();

            struct Bad {};
            cpu.get_el(GetExceptionLevel(op1)).SetSCTLR(cpu, cpu.untaint(Bad(), value));
          }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0010,0b0000,0b010): // 2.82: TCR_EL1, Translation Control Register (EL1)
    case SYSENCODE(0b11,0b101,0b0010,0b0000,0b010): // TCR_EL1 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "TCR_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Translation Control Register (EL1)"; }
          U64 Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu) const override
          {
            if (cpu.pstate.GetEL() == 0)
              throw AArch64::UndefinedInstructionException();

            return U64(cpu.mmu.TCR_EL1);
          }
          void Write(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu, U64 value) const override
          {
            if (cpu.pstate.GetEL() == 0)
              throw AArch64::UndefinedInstructionException();

            cpu.mmu.SetTCR(cpu, value);
            cpu.ipb.invalidate();
          }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0010,0b0000,0b011): // TCR2_EL1 (RW)
    case SYSENCODE(0b11,0b101,0b0010,0b0000,0b011): // TCR2_EL1 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "TCR2_EL1"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Extended Translation Control Register (EL1)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0010,0b0001,0b000): // APIAKeyLo_EL1 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "APIAKeyLo_EL1"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Pointer Authentication Key A for Instruction (bits[63:0])"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0010,0b0001,0b001): // APIAKeyHi_EL1 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "APIAKeyHi_EL1"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Pointer Authentication Key A for Instruction (bits[127:64])"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0010,0b0001,0b010): // APIBKeyLo_EL1 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "APIBKeyLo_EL1"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Pointer Authentication Key B for Instruction (bits[63:0])"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0010,0b0001,0b011): // APIBKeyHi_EL1 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "APIBKeyHi_EL1"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Pointer Authentication Key B for Instruction (bits[127:64])"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0010,0b0010,0b000): // APDAKeyLo_EL1 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "APDAKeyLo_EL1"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Pointer Authentication Key A for Data (bits[63:0])"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0010,0b0010,0b001): // APDAKeyHi_EL1 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "APDAKeyHi_EL1"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Pointer Authentication Key A for Data (bits[127:64])"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0010,0b0010,0b010): // APDBKeyLo_EL1 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "APDBKeyLo_EL1"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Pointer Authentication Key B for Data (bits[63:0])"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0010,0b0010,0b011): // APDBKeyHi_EL1 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "APDBKeyHi_EL1"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Pointer Authentication Key B for Data (bits[127:64])"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0010,0b0011,0b000): // APGAKeyLo_EL1 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "APGAKeyLo_EL1"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Pointer Authentication Key A for Code (bits[63:0])"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0010,0b0011,0b001): // APGAKeyHi_EL1 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "APGAKeyHi_EL1"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Pointer Authentication Key A for Code (bits[127:64])"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b0010,0b0000,0b010): // 2.83: TCR_EL2, Translation Control Register (EL2)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "TCR_EL2"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Translation Control Register (EL2)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b0010,0b0000,0b011): // TCR2_EL2 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "TCR2_EL2"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Extended Translation Control Register (EL2)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b110,0b0010,0b0000,0b010): // 2.84: TCR_EL3, Translation Control Register (EL3)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "TCR_EL3"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Translation Control Register (EL3)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b110,0b0010,0b0001,0b100): // GPTBR_EL3 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "GPTBR_EL3"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Granule Protection Table Base Register (EL3)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b110,0b0010,0b0001,0b110): // GPCCR_EL3 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "GPCCR_EL3"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Granule Protection Check Control Register (EL3)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b010,0b0000,0b0000,0b000): // 2.85: TEECR32_EL1, T32EE Configuration Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "TEECR32_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "T32EE Configuration Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b010,0b0001,0b0000,0b000): // 2.86: TEEHBR32_EL1, T32EE Handler Base Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "TEEHBR32_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "T32EE Handler Base Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b0010,0b0100,0b000): // RNDR, Random Number
    case SYSENCODE(0b11,0b011,0b0010,0b0100,0b001): // RNDRRS, Reseeded Random Number
      {
        if(AArch64::FEAT_RNG)
          {
            static struct : public BaseSysReg {
              void Name(Encoding e, std::ostream& sink) const override { sink << (e.op2 == 0 ? "rndr" : e.op2 == 0 ? "rndrrs" : "?"); }
              void Describe(Encoding e, std::ostream& sink) const override { sink << (e.op2 == 1 ? "Reseeded " : "") << "Random Number"; }
              U64 Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu) const override { return cpu.RNDR(); }
            } x; return &x;
          }
        else
          {
            static BaseSysReg x; return &x;
          }
      }

    case SYSENCODE(0b11,0b011,0b1101,0b0000,0b010): // 2.87: TPIDR_EL0, Thread Pointer / ID Register (EL0)
    case SYSENCODE(0b11,0b000,0b1101,0b0000,0b100): // 2.88: TPIDR_EL1, Thread Pointer / ID Register (EL1)
    case SYSENCODE(0b11,0b100,0b1101,0b0000,0b010): // 2.89: TPIDR_EL2, Thread Pointer / ID Register (EL2)
    case SYSENCODE(0b11,0b110,0b1101,0b0000,0b010): // 2.90: TPIDR_EL3, Thread Pointer / ID Register (EL3)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "TPIDR_EL" << GetExceptionLevel(e.op1); }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Thread Pointer / ID Register (EL" << GetExceptionLevel(e.op1) << ")"; }
          U64 Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu) const override
          {
            if (cpu.pstate.GetEL() < GetExceptionLevel(op1))
              throw AArch64::UndefinedInstructionException();

            return cpu.TPIDR[GetExceptionLevel(op1)];
          }
          void Write(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu, U64 value) const override
          {
            if (cpu.pstate.GetEL() < GetExceptionLevel(op1))
              throw AArch64::UndefinedInstructionException();

            cpu.TPIDR[GetExceptionLevel(op1)] = value;
          }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1101,0b0000,0b011): // 2.91: TPIDRRO_EL0, Thread Pointer / ID Register, Read-Only (EL0)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "TPIDRRO_EL0"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Thread Pointer / ID Register, Read-Only (EL0)"; }
          U64 Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu) const override { return cpu.TPIDRRO; }
          void Write(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu, U64 value) const override
          {
            if (cpu.pstate.GetEL() == 0)
              throw AArch64::UndefinedInstructionException();

            cpu.TPIDRRO = value;
          }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1101,0b0000,0b101): // TPIDR2_EL0 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "TPIDR2_EL0"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "EL0 Read/Write Software Thread ID Register 2"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1101,0b0000,0b111): // SCXTNUM_EL0 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "SCXTNUM_EL0"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "EL0 Read/Write Software Context Number"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1101,0b0010,0b000): // AMCR_EL0 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "AMCR_EL0"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Activity Monitors Control Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1101,0b0010,0b001): // AMCFGR_EL0 (RO)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "AMCFGR_EL0"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Activity Monitors Configuration Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1101,0b0010,0b010): // AMCGCR_EL0 (RO)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "AMCGCR_EL0"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Activity Monitors Counter Group Configuration Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1101,0b0010,0b011): // AMUSERENR_EL0 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "AMUSERENR_EL0"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Activity Monitors User Enable Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1101,0b0010,0b100): // AMCNTENCLR0_EL0 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "AMCNTENCLR0_EL0"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Activity Monitors Count Enable Clear Register 0"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1101,0b0010,0b101): // AMCNTENSET0_EL0 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "AMCNTENSET0_EL0"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Activity Monitors Count Enable Set Register 0"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1101,0b0010,0b110): // AMCG1IDR_EL0 (RO)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "AMCG1IDR_EL0"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Activity Monitors Counter Group 1 Identification Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1101,0b0011,0b000): // AMCNTENCLR1_EL0 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "AMCNTENCLR1_EL0"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Activity Monitors Count Enable Clear Register 1"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1101,0b0011,0b001): // AMCNTENSET1_EL0 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "AMCNTENSET1_EL0"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Activity Monitors Count Enable Set Register 1"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1101,0b0100,0b000): // AMEVCNTR00_EL0 (RW)
    case SYSENCODE(0b11,0b011,0b1101,0b0100,0b001): // AMEVCNTR01_EL0 (RW)
    case SYSENCODE(0b11,0b011,0b1101,0b0100,0b010): // AMEVCNTR02_EL0 (RW)
    case SYSENCODE(0b11,0b011,0b1101,0b0100,0b011): // AMEVCNTR03_EL0 (RW)
    case SYSENCODE(0b11,0b011,0b1101,0b0100,0b100): // AMEVCNTR04_EL0 (RW)
    case SYSENCODE(0b11,0b011,0b1101,0b0100,0b101): // AMEVCNTR05_EL0 (RW)
    case SYSENCODE(0b11,0b011,0b1101,0b0100,0b110): // AMEVCNTR06_EL0 (RW)
    case SYSENCODE(0b11,0b011,0b1101,0b0100,0b111): // AMEVCNTR07_EL0 (RW)
    case SYSENCODE(0b11,0b011,0b1101,0b0101,0b000): // AMEVCNTR08_EL0 (RW)
    case SYSENCODE(0b11,0b011,0b1101,0b0101,0b001): // AMEVCNTR09_EL0 (RW)
    case SYSENCODE(0b11,0b011,0b1101,0b0101,0b010): // AMEVCNTR010_EL0 (RW)
    case SYSENCODE(0b11,0b011,0b1101,0b0101,0b011): // AMEVCNTR011_EL0 (RW)
    case SYSENCODE(0b11,0b011,0b1101,0b0101,0b100): // AMEVCNTR012_EL0 (RW)
    case SYSENCODE(0b11,0b011,0b1101,0b0101,0b101): // AMEVCNTR013_EL0 (RW)
    case SYSENCODE(0b11,0b011,0b1101,0b0101,0b110): // AMEVCNTR014_EL0 (RW)
    case SYSENCODE(0b11,0b011,0b1101,0b0101,0b111): // AMEVCNTR015_EL0 (RW)
      {
        static struct : public BaseSysReg {
          unsigned GetIdx(Encoding e) const { return ((e.crm & 1) << 3) | e.op2; }
          void Name(Encoding e, std::ostream& sink) const override { sink << "AMEVCNTR0" << GetIdx(e) <<  "_EL0"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Activity Monitors Event Counter Registers 0, " << GetIdx(e); }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1101,0b0110,0b000): // AMEVTYPER00_EL0 (RW)
    case SYSENCODE(0b11,0b011,0b1101,0b0110,0b001): // AMEVTYPER01_EL0 (RW)
    case SYSENCODE(0b11,0b011,0b1101,0b0110,0b010): // AMEVTYPER02_EL0 (RW)
    case SYSENCODE(0b11,0b011,0b1101,0b0110,0b011): // AMEVTYPER03_EL0 (RW)
    case SYSENCODE(0b11,0b011,0b1101,0b0110,0b100): // AMEVTYPER04_EL0 (RW)
    case SYSENCODE(0b11,0b011,0b1101,0b0110,0b101): // AMEVTYPER05_EL0 (RW)
    case SYSENCODE(0b11,0b011,0b1101,0b0110,0b110): // AMEVTYPER06_EL0 (RW)
    case SYSENCODE(0b11,0b011,0b1101,0b0110,0b111): // AMEVTYPER07_EL0 (RW)
    case SYSENCODE(0b11,0b011,0b1101,0b0111,0b000): // AMEVTYPER08_EL0 (RW)
    case SYSENCODE(0b11,0b011,0b1101,0b0111,0b001): // AMEVTYPER09_EL0 (RW)
    case SYSENCODE(0b11,0b011,0b1101,0b0111,0b010): // AMEVTYPER010_EL0 (RW)
    case SYSENCODE(0b11,0b011,0b1101,0b0111,0b011): // AMEVTYPER011_EL0 (RW)
    case SYSENCODE(0b11,0b011,0b1101,0b0111,0b100): // AMEVTYPER012_EL0 (RW)
    case SYSENCODE(0b11,0b011,0b1101,0b0111,0b101): // AMEVTYPER013_EL0 (RW)
    case SYSENCODE(0b11,0b011,0b1101,0b0111,0b110): // AMEVTYPER014_EL0 (RW)
    case SYSENCODE(0b11,0b011,0b1101,0b0111,0b111): // AMEVTYPER015_EL0 (RW)
      {
        static struct : public BaseSysReg {
          unsigned GetIdx(Encoding e) const { return ((e.crm & 1) << 3) | e.op2; }
          void Name(Encoding e, std::ostream& sink) const override { sink << "AMEVTYPER0" << GetIdx(e) <<  "_EL0"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Activity Monitors Event Type Registers 0, " << GetIdx(e); }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1101,0b1100,0b000): // AMEVCNTR10_EL0 (RW)
    case SYSENCODE(0b11,0b011,0b1101,0b1100,0b001): // AMEVCNTR11_EL0 (RW)
    case SYSENCODE(0b11,0b011,0b1101,0b1100,0b010): // AMEVCNTR12_EL0 (RW)
    case SYSENCODE(0b11,0b011,0b1101,0b1100,0b011): // AMEVCNTR13_EL0 (RW)
    case SYSENCODE(0b11,0b011,0b1101,0b1100,0b100): // AMEVCNTR14_EL0 (RW)
    case SYSENCODE(0b11,0b011,0b1101,0b1100,0b101): // AMEVCNTR15_EL0 (RW)
    case SYSENCODE(0b11,0b011,0b1101,0b1100,0b110): // AMEVCNTR16_EL0 (RW)
    case SYSENCODE(0b11,0b011,0b1101,0b1100,0b111): // AMEVCNTR17_EL0 (RW)
    case SYSENCODE(0b11,0b011,0b1101,0b1101,0b000): // AMEVCNTR18_EL0 (RW)
    case SYSENCODE(0b11,0b011,0b1101,0b1101,0b001): // AMEVCNTR19_EL0 (RW)
    case SYSENCODE(0b11,0b011,0b1101,0b1101,0b010): // AMEVCNTR110_EL0 (RW)
    case SYSENCODE(0b11,0b011,0b1101,0b1101,0b011): // AMEVCNTR111_EL0 (RW)
    case SYSENCODE(0b11,0b011,0b1101,0b1101,0b100): // AMEVCNTR112_EL0 (RW)
    case SYSENCODE(0b11,0b011,0b1101,0b1101,0b101): // AMEVCNTR113_EL0 (RW)
    case SYSENCODE(0b11,0b011,0b1101,0b1101,0b110): // AMEVCNTR114_EL0 (RW)
    case SYSENCODE(0b11,0b011,0b1101,0b1101,0b111): // AMEVCNTR115_EL0 (RW)
      {
        static struct : public BaseSysReg {
          unsigned GetIdx(Encoding e) const { return ((e.crm & 1) << 3) | e.op2; }
          void Name(Encoding e, std::ostream& sink) const override { sink << "AMEVCNTR1" << GetIdx(e) <<  "_EL0"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Activity Monitors Event Counter Registers 1, " << GetIdx(e); }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1101,0b1110,0b000): // AMEVTYPER10_EL0 (RW)
    case SYSENCODE(0b11,0b011,0b1101,0b1110,0b001): // AMEVTYPER11_EL0 (RW)
    case SYSENCODE(0b11,0b011,0b1101,0b1110,0b010): // AMEVTYPER12_EL0 (RW)
    case SYSENCODE(0b11,0b011,0b1101,0b1110,0b011): // AMEVTYPER13_EL0 (RW)
    case SYSENCODE(0b11,0b011,0b1101,0b1110,0b100): // AMEVTYPER14_EL0 (RW)
    case SYSENCODE(0b11,0b011,0b1101,0b1110,0b101): // AMEVTYPER15_EL0 (RW)
    case SYSENCODE(0b11,0b011,0b1101,0b1110,0b110): // AMEVTYPER16_EL0 (RW)
    case SYSENCODE(0b11,0b011,0b1101,0b1110,0b111): // AMEVTYPER17_EL0 (RW)
    case SYSENCODE(0b11,0b011,0b1101,0b1111,0b000): // AMEVTYPER18_EL0 (RW)
    case SYSENCODE(0b11,0b011,0b1101,0b1111,0b001): // AMEVTYPER19_EL0 (RW)
    case SYSENCODE(0b11,0b011,0b1101,0b1111,0b010): // AMEVTYPER110_EL0 (RW)
    case SYSENCODE(0b11,0b011,0b1101,0b1111,0b011): // AMEVTYPER111_EL0 (RW)
    case SYSENCODE(0b11,0b011,0b1101,0b1111,0b100): // AMEVTYPER112_EL0 (RW)
    case SYSENCODE(0b11,0b011,0b1101,0b1111,0b101): // AMEVTYPER113_EL0 (RW)
    case SYSENCODE(0b11,0b011,0b1101,0b1111,0b110): // AMEVTYPER114_EL0 (RW)
    case SYSENCODE(0b11,0b011,0b1101,0b1111,0b111): // AMEVTYPER115_EL0 (RW)
      {
        static struct : public BaseSysReg {
          unsigned GetIdx(Encoding e) const { return ((e.crm & 1) << 3) | e.op2; }
          void Name(Encoding e, std::ostream& sink) const override { sink << "AMEVTYPER1" << GetIdx(e) <<  "_EL0"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Activity Monitors Event Type Registers 1, " << GetIdx(e); }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1101,0b1000,0b000): // AMEVCNTVOFF00_EL2 (RW)
    case SYSENCODE(0b11,0b100,0b1101,0b1000,0b001): // AMEVCNTVOFF01_EL2 (RW)
    case SYSENCODE(0b11,0b100,0b1101,0b1000,0b010): // AMEVCNTVOFF02_EL2 (RW)
    case SYSENCODE(0b11,0b100,0b1101,0b1000,0b011): // AMEVCNTVOFF03_EL2 (RW)
    case SYSENCODE(0b11,0b100,0b1101,0b1000,0b100): // AMEVCNTVOFF04_EL2 (RW)
    case SYSENCODE(0b11,0b100,0b1101,0b1000,0b101): // AMEVCNTVOFF05_EL2 (RW)
    case SYSENCODE(0b11,0b100,0b1101,0b1000,0b110): // AMEVCNTVOFF06_EL2 (RW)
    case SYSENCODE(0b11,0b100,0b1101,0b1000,0b111): // AMEVCNTVOFF07_EL2 (RW)
    case SYSENCODE(0b11,0b100,0b1101,0b1001,0b000): // AMEVCNTVOFF08_EL2 (RW)
    case SYSENCODE(0b11,0b100,0b1101,0b1001,0b001): // AMEVCNTVOFF09_EL2 (RW)
    case SYSENCODE(0b11,0b100,0b1101,0b1001,0b010): // AMEVCNTVOFF010_EL2 (RW)
    case SYSENCODE(0b11,0b100,0b1101,0b1001,0b011): // AMEVCNTVOFF011_EL2 (RW)
    case SYSENCODE(0b11,0b100,0b1101,0b1001,0b100): // AMEVCNTVOFF012_EL2 (RW)
    case SYSENCODE(0b11,0b100,0b1101,0b1001,0b101): // AMEVCNTVOFF013_EL2 (RW)
    case SYSENCODE(0b11,0b100,0b1101,0b1001,0b110): // AMEVCNTVOFF014_EL2 (RW)
    case SYSENCODE(0b11,0b100,0b1101,0b1001,0b111): // AMEVCNTVOFF015_EL2 (RW)
      {
        static struct : public BaseSysReg {
          unsigned GetIdx(Encoding e) const { return ((e.crm & 1) << 3) | e.op2; }
          void Name(Encoding e, std::ostream& sink) const override { sink << "AMEVCNTVOFF0" << GetIdx(e) <<  "_EL2"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Activity Monitors Event Counter Virtual Offset Registers 0, " << GetIdx(e); }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1101,0b1010,0b000): // AMEVCNTVOFF10_EL2 (RW)
    case SYSENCODE(0b11,0b100,0b1101,0b1010,0b001): // AMEVCNTVOFF11_EL2 (RW)
    case SYSENCODE(0b11,0b100,0b1101,0b1010,0b010): // AMEVCNTVOFF12_EL2 (RW)
    case SYSENCODE(0b11,0b100,0b1101,0b1010,0b011): // AMEVCNTVOFF13_EL2 (RW)
    case SYSENCODE(0b11,0b100,0b1101,0b1010,0b100): // AMEVCNTVOFF14_EL2 (RW)
    case SYSENCODE(0b11,0b100,0b1101,0b1010,0b101): // AMEVCNTVOFF15_EL2 (RW)
    case SYSENCODE(0b11,0b100,0b1101,0b1010,0b110): // AMEVCNTVOFF16_EL2 (RW)
    case SYSENCODE(0b11,0b100,0b1101,0b1010,0b111): // AMEVCNTVOFF17_EL2 (RW)
    case SYSENCODE(0b11,0b100,0b1101,0b1011,0b000): // AMEVCNTVOFF18_EL2 (RW)
    case SYSENCODE(0b11,0b100,0b1101,0b1011,0b001): // AMEVCNTVOFF19_EL2 (RW)
    case SYSENCODE(0b11,0b100,0b1101,0b1011,0b010): // AMEVCNTVOFF110_EL2 (RW)
    case SYSENCODE(0b11,0b100,0b1101,0b1011,0b011): // AMEVCNTVOFF111_EL2 (RW)
    case SYSENCODE(0b11,0b100,0b1101,0b1011,0b100): // AMEVCNTVOFF112_EL2 (RW)
    case SYSENCODE(0b11,0b100,0b1101,0b1011,0b101): // AMEVCNTVOFF113_EL2 (RW)
    case SYSENCODE(0b11,0b100,0b1101,0b1011,0b110): // AMEVCNTVOFF114_EL2 (RW)
    case SYSENCODE(0b11,0b100,0b1101,0b1011,0b111): // AMEVCNTVOFF115_EL2 (RW)
      {
        static struct : public BaseSysReg {
          unsigned GetIdx(Encoding e) const { return ((e.crm & 1) << 3) | e.op2; }
          void Name(Encoding e, std::ostream& sink) const override { sink << "AMEVCNTVOFF1" << GetIdx(e) <<  "_EL2"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Activity Monitors Event Counter Virtual Offset Registers 1, " << GetIdx(e); }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0010,0b0000,0b000): // 2.92: TTBR0_EL1, Translation Table Base Register 0 (EL1)
    case SYSENCODE(0b11,0b101,0b0010,0b0000,0b000): // TTBR0_EL1 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "TTBR0_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Translation Table Base Register 0 (EL1)"; }
          U64 Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu) const override
          {
            if (cpu.pstate.GetEL() == 0)
              throw AArch64::UndefinedInstructionException();

            return U64(cpu.mmu.TTBR0_EL1);
          }
          void Write(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu, U64 value) const override
          {
            if (cpu.pstate.GetEL() == 0)
              throw AArch64::UndefinedInstructionException();

            cpu.mmu.SetTTBR0(cpu, value);
            cpu.ipb.invalidate();
          }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b0010,0b0000,0b000): // 2.93: TTBR0_EL2, Translation Table Base Register 0 (EL2)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "TTBR0_EL2"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Translation Table Base Register 0 (EL2)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b0010,0b0000,0b001): // TTBR1_EL2 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "TTBR1_EL2"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Translation Table Base Register 1 (EL2)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b110,0b0010,0b0000,0b000): // 2.94: TTBR0_EL3, Translation Table Base Register 0 (EL3)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "TTBR0_EL3"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Translation Table Base Register 0 (EL3)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0010,0b0000,0b001): // 2.95: TTBR1_EL1, Translation Table Base Register 1
    case SYSENCODE(0b11,0b101,0b0010,0b0000,0b001): // TTBR1_EL1 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "TTBR1_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Translation Table Base Register 1"; }
          U64 Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu) const override
          {
            if (cpu.pstate.GetEL() == 0)
              throw AArch64::UndefinedInstructionException();

            return U64(cpu.mmu.TTBR1_EL1);
          }
          void Write(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu, U64 value) const override
          {
            if (cpu.pstate.GetEL() == 0)
              throw AArch64::UndefinedInstructionException();

            cpu.mmu.SetTTBR1(cpu, value);
            cpu.ipb.invalidate();
          }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b101,0b0100,0b0000,0b000): // SPSR_EL1 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "SPSR_EL1"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Vector Base Address Register (EL1)"; }
          U64  Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu) const override
          {
            if (cpu.pstate.GetEL() == 0)
              throw AArch64::UndefinedInstructionException();

            return U64(cpu.get_el(1).SPSR);
          }
          void Write(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu, U64 value) const override
          {
            if (cpu.pstate.GetEL() == 0)
              throw AArch64::UndefinedInstructionException();

            cpu.get_el(1).SPSR = U32(value);
          }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0100,0b0000,0b000): // SPSR_EL1
    case SYSENCODE(0b11,0b100,0b0100,0b0000,0b000): // SPSR_EL2
    case SYSENCODE(0b11,0b110,0b0100,0b0000,0b000): // SPSR_EL3
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "SPSR_EL" << GetExceptionLevel(e.op1); }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Vector Base Address Register (EL" << GetExceptionLevel(e.op1) << ")"; }
          U64  Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu) const override
          {
            if (cpu.pstate.GetEL() == 0)
              throw AArch64::UndefinedInstructionException();

            return U64(cpu.get_el(GetExceptionLevel(op1)).SPSR);
          }
          void Write(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu, U64 value) const override
          {
            if (cpu.pstate.GetEL() == 0)
              throw AArch64::UndefinedInstructionException();

            cpu.get_el(GetExceptionLevel(op1)).SPSR = U32(value);
          }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b0100,0b0011,0b000): // SPSR_irq (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "SPSR_irq"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Saved Program Status Register (IRQ mode)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b0100,0b0011,0b001): // SPSR_abt (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "SPSR_abt"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Saved Program Status Register (Abort mode)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b0100,0b0011,0b010): // SPSR_und (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "SPSR_und"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Saved Program Status Register (Undefined mode)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b0100,0b0011,0b011): // SPSR_fiq (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "SPSR_fiq"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Saved Program Status Register (FIQ mode)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b101,0b0100,0b0000,0b001): // ELR_EL1 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "ELR_EL1"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Vector Base Address Register (EL1)"; }
          U64  Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu) const override
          {
            if (cpu.pstate.GetEL() == 0)
              throw AArch64::UndefinedInstructionException();

            return cpu.get_el(1).ELR;
          }
          void Write(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu, U64 value) const override
          {
            if (cpu.pstate.GetEL() == 0)
              throw AArch64::UndefinedInstructionException();

            cpu.get_el(1).ELR = value;
          }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0100,0b0000,0b001): // ELR_EL1
    case SYSENCODE(0b11,0b100,0b0100,0b0000,0b001): // ELR_EL2
    case SYSENCODE(0b11,0b110,0b0100,0b0000,0b001): // ELR_EL3
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "ELR_EL" << GetExceptionLevel(e.op1); }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Vector Base Address Register (EL" << GetExceptionLevel(e.op1) << ")"; }
          U64  Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu) const override
          {
            if (cpu.pstate.GetEL() == 0)
              throw AArch64::UndefinedInstructionException();

            return cpu.get_el(GetExceptionLevel(op1)).ELR;
          }
          void Write(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu, U64 value) const override
          {
            if (cpu.pstate.GetEL() == 0)
              throw AArch64::UndefinedInstructionException();

            cpu.get_el(GetExceptionLevel(op1)).ELR = value;
          }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b101,0b1100,0b0000,0b000): // VBAR_EL1 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "VBAR_EL1"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Vector Base Address Register (EL1)"; }
          U64  Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu) const override
          {
            if (cpu.pstate.GetEL() == 0)
              throw AArch64::UndefinedInstructionException();

            return cpu.get_el(1).VBAR;
          }
          void Write(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu, U64 value) const override
          {
            if (cpu.pstate.GetEL() == 0)
              throw AArch64::UndefinedInstructionException();

            cpu.get_el(1).VBAR = value;
          }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b0000,0b000): // 2.96: VBAR_EL1, Vector Base Address Register (EL1)
    case SYSENCODE(0b11,0b100,0b1100,0b0000,0b000): // 2.97: VBAR_EL2, Vector Base Address Register (EL2)
    case SYSENCODE(0b11,0b110,0b1100,0b0000,0b000): // 2.98: VBAR_EL3, Vector Base Address Register (EL3)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "VBAR_EL" << GetExceptionLevel(e.op1); }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Vector Base Address Register (EL" << GetExceptionLevel(e.op1) << ")"; }
          U64  Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu) const override
          {
            if (cpu.pstate.GetEL() == 0)
              throw AArch64::UndefinedInstructionException();

            return cpu.get_el(GetExceptionLevel(op1)).VBAR;
          }
          void Write(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu, U64 value) const override
          {
            if (cpu.pstate.GetEL() == 0)
              throw AArch64::UndefinedInstructionException();

            cpu.get_el(GetExceptionLevel(op1)).VBAR = value;
          }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b0000,0b0000,0b101): // 2.99: VMPIDR_EL2, Virtualization Multiprocessor ID Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "VMPIDR_EL2"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Virtualization Multiprocessor ID Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b0000,0b0000,0b000): // 2.100: VPIDR_EL2, Virtualization Processor ID Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "VPIDR_EL2"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Virtualization Processor ID Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b0010,0b0001,0b010): // 2.101: VTCR_EL2, Virtualization Translation Control Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "VTCR_EL2"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Virtualization Translation Control Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b0010,0b0001,0b000): // 2.102: VTTBR_EL2, Virtualization Translation Table Base Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "VTTBR_EL2"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Virtualization Translation Table Base Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b0010,0b0010,0b000): // VNCR_EL2 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "VNCR_EL2"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Virtual Nested Control Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b0010,0b0110,0b000): // VSTTBR_EL2 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "VSTTBR_EL2"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Virtualization Secure Translation Table Base Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b0010,0b0110,0b010): // VSTCR_EL2 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "VSTCR_EL2"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Virtualization Secure Translation Control Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b000,0b0111,0b1110,0b110): // 3.1: DBGAUTHSTATUS_EL1, Debug Authentication Status register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "DBGAUTHSTATUS_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Debug Authentication Status register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0101,0b0011,0b000): // ERRIDR_EL1 (RO)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "ERRIDR_EL1"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Error Record ID Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0101,0b0011,0b001): // ERRSELR_EL1 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "ERRSELR_EL1"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Error Record Select Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0101,0b0100,0b000): // ERXFR_EL1 (RO)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "ERXFR_EL1"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Selected Error Record Feature Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0101,0b0100,0b001): // ERXCTLR_EL1 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "ERXCTLR_EL1"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Selected Error Record Control Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0101,0b0100,0b010): // ERXSTATUS_EL1 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "ERXSTATUS_EL1"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Selected Error Record Primary Status Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0101,0b0100,0b011): // ERXADDR_EL1 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "ERXADDR_EL1"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Selected Error Record Address Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0101,0b0100,0b100): // ERXPFGF_EL1 (RO)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "ERXPFGF_EL1"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Selected Pseudo-fault Generation Feature register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0101,0b0100,0b101): // ERXPFGCTL_EL1 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "ERXPFGCTL_EL1"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Selected Pseudo-fault Generation Control register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0101,0b0100,0b110): // ERXPFGCDN_EL1 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "ERXPFGCDN_EL1"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Selected Pseudo-fault Generation Countdown register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0101,0b0101,0b000): // ERXMISC0_EL1 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "ERXMISC0_EL1"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Selected Error Record Miscellaneous Register 0"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0101,0b0101,0b001): // ERXMISC1_EL1 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "ERXMISC1_EL1"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Selected Error Record Miscellaneous Register 1"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0101,0b0101,0b010): // ERXMISC2_EL1 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "ERXMISC2_EL1"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Selected Error Record Miscellaneous Register 2"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0101,0b0101,0b011): // ERXMISC3_EL1 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "ERXMISC3_EL1"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Selected Error Record Miscellaneous Register 3"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0101,0b0110,0b000): // TFSR_EL1 (RW)
    case SYSENCODE(0b11,0b100,0b0101,0b0110,0b000): // TFSR_EL1 (RW)
    case SYSENCODE(0b11,0b101,0b0101,0b0110,0b000): // TFSR_EL1 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "TFSR_EL1"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Tag Fault Status Register (EL1)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0101,0b0110,0b001): // TFSRE0_EL1 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "TFSRE0_EL1"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Tag Fault Status Register (EL0)."; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b110,0b0101,0b0110,0b000): // TFSR_EL3 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "TFSR_EL3"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Tag Fault Status Register (EL3)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b001,0b0000,0b0000,0b001): // TRCTRACEIDR (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "TRCTRACEIDR"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Trace ID Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b001,0b0000,0b0000,0b010): // TRCVICTLR (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "TRCVICTLR"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "ViewInst Main Control Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b001,0b0000,0b0000,0b110): // TRCIDR8 (RO)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "TRCIDR8"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "ID Register 8"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b001,0b0000,0b0001,0b000): // TRCPRGCTLR (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "TRCPRGCTLR"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Programming Control Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b001,0b0000,0b0001,0b001): // TRCQCTLR (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "TRCQCTLR"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Q Element Control Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b001,0b0000,0b0001,0b010): // TRCVIIECTLR (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "TRCVIIECTLR"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "ViewInst Include/Exclude Control Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b001,0b0000,0b0001,0b110): // TRCIDR9 (RO)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "TRCIDR9"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "ID Register 9"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b001,0b0000,0b0010,0b010): // TRCVISSCTLR (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "TRCVISSCTLR"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "ViewInst Start/Stop Control Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b001,0b0000,0b0010,0b110): // TRCIDR10 (RO)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "TRCIDR10"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "ID Register 10"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b001,0b0000,0b0011,0b000): // TRCSTATR (RO)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "TRCSTATR"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Trace Status Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b001,0b0000,0b0011,0b010): // TRCVIPCSSCTLR (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "TRCVIPCSSCTLR"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "ViewInst Start/Stop PE Comparator Control Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b001,0b0000,0b0011,0b110): // TRCIDR11 (RO)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "TRCIDR11"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "ID Register 11"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b001,0b0000,0b0000,0b100): // TRCSEQEVR0 (RW)
    case SYSENCODE(0b10,0b001,0b0000,0b0001,0b100): // TRCSEQEVR1 (RW)
    case SYSENCODE(0b10,0b001,0b0000,0b0010,0b100): // TRCSEQEVR2 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "TRCSEQEVR" << std::dec << int(e.crm); }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Sequencer State Transition Control Register " << std::dec << int(e.crm); }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b001,0b0000,0b0000,0b101): // TRCCNTRLDVR0 (RW)
    case SYSENCODE(0b10,0b001,0b0000,0b0001,0b101): // TRCCNTRLDVR1 (RW)
    case SYSENCODE(0b10,0b001,0b0000,0b0010,0b101): // TRCCNTRLDVR2 (RW)
    case SYSENCODE(0b10,0b001,0b0000,0b0011,0b101): // TRCCNTRLDVR3 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "TRCCNTRLDVR" << std::dec << int(e.crm); }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Counter Reload Value Register " << std::dec << int(e.crm); }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b001,0b0000,0b0100,0b000): // TRCCONFIGR (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "TRCCONFIGR"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Trace Configuration Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b001,0b0000,0b0100,0b110): // TRCIDR12 (RO)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "TRCIDR12"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "ID Register 12"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b001,0b0000,0b0101,0b110): // TRCIDR13 (RO)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "TRCIDR13"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "ID Register 13"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b001,0b0000,0b0110,0b000): // TRCAUXCTLR (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "TRCAUXCTLR"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Auxiliary Control Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b001,0b0000,0b0110,0b100): // TRCSEQRSTEVR (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "TRCSEQRSTEVR"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Sequencer Reset Control Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b001,0b0000,0b0111,0b100): // TRCSEQSTR (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "TRCSEQSTR"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Sequencer State Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b001,0b0000,0b0100,0b101): // TRCCNTCTLR0 (RW)
    case SYSENCODE(0b10,0b001,0b0000,0b0101,0b101): // TRCCNTCTLR1 (RW)
    case SYSENCODE(0b10,0b001,0b0000,0b0110,0b101): // TRCCNTCTLR2 (RW)
    case SYSENCODE(0b10,0b001,0b0000,0b0111,0b101): // TRCCNTCTLR3 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "TRCCNTCTLR" << std::dec << int(e.crm); }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Counter Control Register " << std::dec << int(e.crm); }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b001,0b0000,0b0000,0b111): // TRCIMSPEC0 (RW)
    case SYSENCODE(0b10,0b001,0b0000,0b0001,0b111): // TRCIMSPEC1 (RW)
    case SYSENCODE(0b10,0b001,0b0000,0b0010,0b111): // TRCIMSPEC2 (RW)
    case SYSENCODE(0b10,0b001,0b0000,0b0011,0b111): // TRCIMSPEC3 (RW)
    case SYSENCODE(0b10,0b001,0b0000,0b0100,0b111): // TRCIMSPEC4 (RW)
    case SYSENCODE(0b10,0b001,0b0000,0b0101,0b111): // TRCIMSPEC5 (RW)
    case SYSENCODE(0b10,0b001,0b0000,0b0110,0b111): // TRCIMSPEC6 (RW)
    case SYSENCODE(0b10,0b001,0b0000,0b0111,0b111): // TRCIMSPEC7 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "TRCIMSPEC" << std::dec << int(e.crm); }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "IMP DEF Register " << std::dec << int(e.crm); }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b001,0b0000,0b1000,0b000): // TRCEVENTCTL0R (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "TRCEVENTCTL0R"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Event Control 0 Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b001,0b0000,0b1000,0b111): // TRCIDR0 (RO)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "TRCIDR0"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "ID Register 0"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b001,0b0000,0b1001,0b000): // TRCEVENTCTL1R (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "TRCEVENTCTL1R"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Event Control 1 Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b001,0b0000,0b1001,0b111): // TRCIDR1 (RO)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "TRCIDR1"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "ID Register 1"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b001,0b0000,0b1010,0b000): // TRCRSR (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "TRCRSR"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Resources Status Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b001,0b0000,0b1010,0b111): // TRCIDR2 (RO)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "TRCIDR2"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "ID Register 2"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b001,0b0000,0b1011,0b000): // TRCSTALLCTLR (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "TRCSTALLCTLR"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Stall Control Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b001,0b0000,0b1011,0b111): // TRCIDR3 (RO)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "TRCIDR3"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "ID Register 3"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b001,0b0000,0b1000,0b100): // TRCEXTINSELR0 (RW)
    case SYSENCODE(0b10,0b001,0b0000,0b1001,0b100): // TRCEXTINSELR1 (RW)
    case SYSENCODE(0b10,0b001,0b0000,0b1010,0b100): // TRCEXTINSELR2 (RW)
    case SYSENCODE(0b10,0b001,0b0000,0b1011,0b100): // TRCEXTINSELR3 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "TRCEXTINSELR" << std::dec << int(e.crm); }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "External Input Select Register " << std::dec << int(e.crm); }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b001,0b0000,0b1000,0b101): // TRCCNTVR0 (RW)
    case SYSENCODE(0b10,0b001,0b0000,0b1001,0b101): // TRCCNTVR1 (RW)
    case SYSENCODE(0b10,0b001,0b0000,0b1010,0b101): // TRCCNTVR2 (RW)
    case SYSENCODE(0b10,0b001,0b0000,0b1011,0b101): // TRCCNTVR3 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "TRCCNTVR" << std::dec << int(e.crm); }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Counter Value Register " << std::dec << int(e.crm); }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b001,0b0000,0b1100,0b000): // TRCTSCTLR (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "TRCTSCTLR"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Timestamp Control Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b001,0b0000,0b1100,0b111): // TRCIDR4 (RO)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "TRCIDR4"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "ID Register 4"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b001,0b0000,0b1101,0b000): // TRCSYNCPR (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "TRCSYNCPR"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Synchronization Period Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b001,0b0000,0b1101,0b111): // TRCIDR5 (RO)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "TRCIDR5"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "ID Register 5"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b001,0b0000,0b1110,0b000): // TRCCCCTLR (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "TRCCCCTLR"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Cycle Count Control Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b001,0b0000,0b1110,0b111): // TRCIDR6 (RO)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "TRCIDR6"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "ID Register 6"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b001,0b0000,0b1111,0b000): // TRCBBCTLR (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "TRCBBCTLR"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Branch Broadcast Control Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b001,0b0000,0b1111,0b111): // TRCIDR7 (RO)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "TRCIDR7"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "ID Register 7"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b001,0b0001,0b0001,0b100): // TRCOSLSR (RO)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "TRCOSLSR"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Trace OS Lock Status Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b001,0b0001,0b0000,0b010): // TRCSSCCR0 (RW)
    case SYSENCODE(0b10,0b001,0b0001,0b0001,0b010): // TRCSSCCR1 (RW)
    case SYSENCODE(0b10,0b001,0b0001,0b0010,0b010): // TRCSSCCR2 (RW)
    case SYSENCODE(0b10,0b001,0b0001,0b0011,0b010): // TRCSSCCR3 (RW)
    case SYSENCODE(0b10,0b001,0b0001,0b0100,0b010): // TRCSSCCR4 (RW)
    case SYSENCODE(0b10,0b001,0b0001,0b0101,0b010): // TRCSSCCR5 (RW)
    case SYSENCODE(0b10,0b001,0b0001,0b0110,0b010): // TRCSSCCR6 (RW)
    case SYSENCODE(0b10,0b001,0b0001,0b0111,0b010): // TRCSSCCR7 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "TRCSSCCR" << std::dec << int(e.crm); }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Single-shot Comparator Control Register " << std::dec << int(e.crm); }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b001,0b0001,0b0000,0b011): // TRCSSPCICR0 (RW)
    case SYSENCODE(0b10,0b001,0b0001,0b0001,0b011): // TRCSSPCICR1 (RW)
    case SYSENCODE(0b10,0b001,0b0001,0b0010,0b011): // TRCSSPCICR2 (RW)
    case SYSENCODE(0b10,0b001,0b0001,0b0011,0b011): // TRCSSPCICR3 (RW)
    case SYSENCODE(0b10,0b001,0b0001,0b0100,0b011): // TRCSSPCICR4 (RW)
    case SYSENCODE(0b10,0b001,0b0001,0b0101,0b011): // TRCSSPCICR5 (RW)
    case SYSENCODE(0b10,0b001,0b0001,0b0110,0b011): // TRCSSPCICR6 (RW)
    case SYSENCODE(0b10,0b001,0b0001,0b0111,0b011): // TRCSSPCICR7 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "TRCSSPCICR" << std::dec << int(e.crm); }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Single-shot Processing Element Comparator Input Control Register " << std::dec << int(e.crm); }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b001,0b0001,0b1000,0b010): // TRCSSCSR0 (RW)
    case SYSENCODE(0b10,0b001,0b0001,0b1001,0b010): // TRCSSCSR1 (RW)
    case SYSENCODE(0b10,0b001,0b0001,0b1010,0b010): // TRCSSCSR2 (RW)
    case SYSENCODE(0b10,0b001,0b0001,0b1011,0b010): // TRCSSCSR3 (RW)
    case SYSENCODE(0b10,0b001,0b0001,0b1100,0b010): // TRCSSCSR4 (RW)
    case SYSENCODE(0b10,0b001,0b0001,0b1101,0b010): // TRCSSCSR5 (RW)
    case SYSENCODE(0b10,0b001,0b0001,0b1110,0b010): // TRCSSCSR6 (RW)
    case SYSENCODE(0b10,0b001,0b0001,0b1111,0b010): // TRCSSCSR7 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "TRCSSCSR" << std::dec << int(e.crm); }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Single-shot Comparator Control Status Register " << std::dec << int(e.crm); }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b001,0b0001,0b0000,0b000): // TRCRSCTLR0  (RW)
    case SYSENCODE(0b10,0b001,0b0001,0b0001,0b000): // TRCRSCTLR1  (RW)
    case SYSENCODE(0b10,0b001,0b0001,0b0010,0b000): // TRCRSCTLR2  (RW)
    case SYSENCODE(0b10,0b001,0b0001,0b0011,0b000): // TRCRSCTLR3  (RW)
    case SYSENCODE(0b10,0b001,0b0001,0b0100,0b000): // TRCRSCTLR4  (RW)
    case SYSENCODE(0b10,0b001,0b0001,0b0101,0b000): // TRCRSCTLR5  (RW)
    case SYSENCODE(0b10,0b001,0b0001,0b0110,0b000): // TRCRSCTLR6  (RW)
    case SYSENCODE(0b10,0b001,0b0001,0b0111,0b000): // TRCRSCTLR7  (RW)
    case SYSENCODE(0b10,0b001,0b0001,0b1000,0b000): // TRCRSCTLR8  (RW)
    case SYSENCODE(0b10,0b001,0b0001,0b1001,0b000): // TRCRSCTLR9  (RW)
    case SYSENCODE(0b10,0b001,0b0001,0b1010,0b000): // TRCRSCTLR10 (RW)
    case SYSENCODE(0b10,0b001,0b0001,0b1011,0b000): // TRCRSCTLR11 (RW)
    case SYSENCODE(0b10,0b001,0b0001,0b1100,0b000): // TRCRSCTLR12 (RW)
    case SYSENCODE(0b10,0b001,0b0001,0b1101,0b000): // TRCRSCTLR13 (RW)
    case SYSENCODE(0b10,0b001,0b0001,0b1110,0b000): // TRCRSCTLR14 (RW)
    case SYSENCODE(0b10,0b001,0b0001,0b1111,0b000): // TRCRSCTLR15 (RW)
    case SYSENCODE(0b10,0b001,0b0001,0b0000,0b001): // TRCRSCTLR16 (RW)
    case SYSENCODE(0b10,0b001,0b0001,0b0001,0b001): // TRCRSCTLR17 (RW)
    case SYSENCODE(0b10,0b001,0b0001,0b0010,0b001): // TRCRSCTLR18 (RW)
    case SYSENCODE(0b10,0b001,0b0001,0b0011,0b001): // TRCRSCTLR19 (RW)
    case SYSENCODE(0b10,0b001,0b0001,0b0100,0b001): // TRCRSCTLR20 (RW)
    case SYSENCODE(0b10,0b001,0b0001,0b0101,0b001): // TRCRSCTLR21 (RW)
    case SYSENCODE(0b10,0b001,0b0001,0b0110,0b001): // TRCRSCTLR22 (RW)
    case SYSENCODE(0b10,0b001,0b0001,0b0111,0b001): // TRCRSCTLR23 (RW)
    case SYSENCODE(0b10,0b001,0b0001,0b1000,0b001): // TRCRSCTLR24 (RW)
    case SYSENCODE(0b10,0b001,0b0001,0b1001,0b001): // TRCRSCTLR25 (RW)
    case SYSENCODE(0b10,0b001,0b0001,0b1010,0b001): // TRCRSCTLR26 (RW)
    case SYSENCODE(0b10,0b001,0b0001,0b1011,0b001): // TRCRSCTLR27 (RW)
    case SYSENCODE(0b10,0b001,0b0001,0b1100,0b001): // TRCRSCTLR28 (RW)
    case SYSENCODE(0b10,0b001,0b0001,0b1101,0b001): // TRCRSCTLR29 (RW)
    case SYSENCODE(0b10,0b001,0b0001,0b1110,0b001): // TRCRSCTLR30 (RW)
    case SYSENCODE(0b10,0b001,0b0001,0b1111,0b001): // TRCRSCTLR31 (RW)
      {
        static struct : public BaseSysReg {
          unsigned GetIdx(Encoding e) const { return e.crm | ((e.op2 & 1) << 4); }
          void Name(Encoding e, std::ostream& sink) const override { sink << "TRCRSCTLR" << std::dec << GetIdx(e); }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Resource Selection Control Register " << std::dec << GetIdx(e); }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b001,0b0010,0b0000,0b000): // TRCACVR0  (RW)
    case SYSENCODE(0b10,0b001,0b0010,0b0010,0b000): // TRCACVR1  (RW)
    case SYSENCODE(0b10,0b001,0b0010,0b0100,0b000): // TRCACVR2  (RW)
    case SYSENCODE(0b10,0b001,0b0010,0b0110,0b000): // TRCACVR3  (RW)
    case SYSENCODE(0b10,0b001,0b0010,0b1000,0b000): // TRCACVR4  (RW)
    case SYSENCODE(0b10,0b001,0b0010,0b1010,0b000): // TRCACVR5  (RW)
    case SYSENCODE(0b10,0b001,0b0010,0b1100,0b000): // TRCACVR6  (RW)
    case SYSENCODE(0b10,0b001,0b0010,0b1110,0b000): // TRCACVR7  (RW)
    case SYSENCODE(0b10,0b001,0b0010,0b0000,0b001): // TRCACVR8  (RW)
    case SYSENCODE(0b10,0b001,0b0010,0b0010,0b001): // TRCACVR9  (RW)
    case SYSENCODE(0b10,0b001,0b0010,0b0100,0b001): // TRCACVR10 (RW)
    case SYSENCODE(0b10,0b001,0b0010,0b0110,0b001): // TRCACVR11 (RW)
    case SYSENCODE(0b10,0b001,0b0010,0b1000,0b001): // TRCACVR12 (RW)
    case SYSENCODE(0b10,0b001,0b0010,0b1010,0b001): // TRCACVR13 (RW)
    case SYSENCODE(0b10,0b001,0b0010,0b1100,0b001): // TRCACVR14 (RW)
    case SYSENCODE(0b10,0b001,0b0010,0b1110,0b001): // TRCACVR15 (RW)
      {
        static struct : public BaseSysReg {
          unsigned GetIdx(Encoding e) const { return ((e.crm >> 1) & 7) | ((e.op2 & 1) << 3); }
          void Name(Encoding e, std::ostream& sink) const override { sink << "TRCACVR" << std::dec << GetIdx(e); }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Address Comparator Value Register " << std::dec << GetIdx(e); }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b001,0b0010,0b0000,0b010): // TRCACATR0  (RW)
    case SYSENCODE(0b10,0b001,0b0010,0b0010,0b010): // TRCACATR1  (RW)
    case SYSENCODE(0b10,0b001,0b0010,0b0100,0b010): // TRCACATR2  (RW)
    case SYSENCODE(0b10,0b001,0b0010,0b0110,0b010): // TRCACATR3  (RW)
    case SYSENCODE(0b10,0b001,0b0010,0b1000,0b010): // TRCACATR4  (RW)
    case SYSENCODE(0b10,0b001,0b0010,0b1010,0b010): // TRCACATR5  (RW)
    case SYSENCODE(0b10,0b001,0b0010,0b1100,0b010): // TRCACATR6  (RW)
    case SYSENCODE(0b10,0b001,0b0010,0b1110,0b010): // TRCACATR7  (RW)
    case SYSENCODE(0b10,0b001,0b0010,0b0000,0b011): // TRCACATR8  (RW)
    case SYSENCODE(0b10,0b001,0b0010,0b0010,0b011): // TRCACATR9  (RW)
    case SYSENCODE(0b10,0b001,0b0010,0b0100,0b011): // TRCACATR10 (RW)
    case SYSENCODE(0b10,0b001,0b0010,0b0110,0b011): // TRCACATR11 (RW)
    case SYSENCODE(0b10,0b001,0b0010,0b1000,0b011): // TRCACATR12 (RW)
    case SYSENCODE(0b10,0b001,0b0010,0b1010,0b011): // TRCACATR13 (RW)
    case SYSENCODE(0b10,0b001,0b0010,0b1100,0b011): // TRCACATR14 (RW)
    case SYSENCODE(0b10,0b001,0b0010,0b1110,0b011): // TRCACATR15 (RW)
      {
        static struct : public BaseSysReg {
          unsigned GetIdx(Encoding e) const { return ((e.crm >> 1) & 7) | ((e.op2 & 1) << 3); }
          void Name(Encoding e, std::ostream& sink) const override { sink << "TRCACATR" << std::dec << GetIdx(e); }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Address Comparator Access Type Register " << std::dec << GetIdx(e); }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b001,0b0011,0b0000,0b010): // TRCCIDCCTLR0 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "TRCCIDCCTLR0"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Context Identifier Comparator Control Register 0"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b001,0b0011,0b0001,0b010): // TRCCIDCCTLR1 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "TRCCIDCCTLR1"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Context Identifier Comparator Control Register 1"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b001,0b0011,0b0010,0b010): // TRCVMIDCCTLR0 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "TRCVMIDCCTLR0"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Virtual Context Identifier Comparator Control Register 0"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b001,0b0011,0b0011,0b010): // TRCVMIDCCTLR1 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "TRCVMIDCCTLR1"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Virtual Context Identifier Comparator Control Register 1"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b001,0b0011,0b0000,0b000): // TRCCIDCVR0 (RW)
    case SYSENCODE(0b10,0b001,0b0011,0b0010,0b000): // TRCCIDCVR1 (RW)
    case SYSENCODE(0b10,0b001,0b0011,0b0100,0b000): // TRCCIDCVR2 (RW)
    case SYSENCODE(0b10,0b001,0b0011,0b0110,0b000): // TRCCIDCVR3 (RW)
    case SYSENCODE(0b10,0b001,0b0011,0b1000,0b000): // TRCCIDCVR4 (RW)
    case SYSENCODE(0b10,0b001,0b0011,0b1010,0b000): // TRCCIDCVR5 (RW)
    case SYSENCODE(0b10,0b001,0b0011,0b1100,0b000): // TRCCIDCVR6 (RW)
    case SYSENCODE(0b10,0b001,0b0011,0b1110,0b000): // TRCCIDCVR7 (RW)
      {
        static struct : public BaseSysReg {
          unsigned GetIdx(Encoding e) const { return (e.crm >> 1) & 7; }
          void Name(Encoding e, std::ostream& sink) const override { sink << "TRCCIDCVR" << std::dec << GetIdx(e); }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Context Identifier Comparator Value Registers " << std::dec << GetIdx(e); }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b001,0b0011,0b0000,0b001): // TRCVMIDCVR0 (RW)
    case SYSENCODE(0b10,0b001,0b0011,0b0010,0b001): // TRCVMIDCVR1 (RW)
    case SYSENCODE(0b10,0b001,0b0011,0b0100,0b001): // TRCVMIDCVR2 (RW)
    case SYSENCODE(0b10,0b001,0b0011,0b0110,0b001): // TRCVMIDCVR3 (RW)
    case SYSENCODE(0b10,0b001,0b0011,0b1000,0b001): // TRCVMIDCVR4 (RW)
    case SYSENCODE(0b10,0b001,0b0011,0b1010,0b001): // TRCVMIDCVR5 (RW)
    case SYSENCODE(0b10,0b001,0b0011,0b1100,0b001): // TRCVMIDCVR6 (RW)
    case SYSENCODE(0b10,0b001,0b0011,0b1110,0b001): // TRCVMIDCVR7 (RW)
      {
        static struct : public BaseSysReg {
          unsigned GetIdx(Encoding e) const { return (e.crm >> 1) & 7; }
          void Name(Encoding e, std::ostream& sink) const override { sink << "TRCVMIDCVR" << std::dec << GetIdx(e); }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Virtual Context Identifier Comparator Value Register " << std::dec << GetIdx(e); }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b001,0b0111,0b0010,0b111): // TRCDEVID (RO)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "TRCDEVID"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Device Configuration Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b001,0b0111,0b1000,0b110): // TRCCLAIMSET (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "TRCCLAIMSET"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Claim Tag Set Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b001,0b0111,0b1001,0b110): // TRCCLAIMCLR (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "TRCCLAIMCLR"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Claim Tag Clear Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b001,0b0111,0b1110,0b110): // TRCAUTHSTATUS (RO)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "TRCAUTHSTATUS"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Authentication Status Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b001,0b0111,0b1111,0b110): // TRCDEVARCH (RO)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "TRCDEVARCH"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Device Architecture Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b001,0b1000,0b0000,0b000): // BRBINF0_EL1  (RO)
    case SYSENCODE(0b10,0b001,0b1000,0b0001,0b000): // BRBINF1_EL1  (RO)
    case SYSENCODE(0b10,0b001,0b1000,0b0010,0b000): // BRBINF2_EL1  (RO)
    case SYSENCODE(0b10,0b001,0b1000,0b0011,0b000): // BRBINF3_EL1  (RO)
    case SYSENCODE(0b10,0b001,0b1000,0b0100,0b000): // BRBINF4_EL1  (RO)
    case SYSENCODE(0b10,0b001,0b1000,0b0101,0b000): // BRBINF5_EL1  (RO)
    case SYSENCODE(0b10,0b001,0b1000,0b0110,0b000): // BRBINF6_EL1  (RO)
    case SYSENCODE(0b10,0b001,0b1000,0b0111,0b000): // BRBINF7_EL1  (RO)
    case SYSENCODE(0b10,0b001,0b1000,0b1000,0b000): // BRBINF8_EL1  (RO)
    case SYSENCODE(0b10,0b001,0b1000,0b1001,0b000): // BRBINF9_EL1  (RO)
    case SYSENCODE(0b10,0b001,0b1000,0b1010,0b000): // BRBINF10_EL1 (RO)
    case SYSENCODE(0b10,0b001,0b1000,0b1011,0b000): // BRBINF11_EL1 (RO)
    case SYSENCODE(0b10,0b001,0b1000,0b1100,0b000): // BRBINF12_EL1 (RO)
    case SYSENCODE(0b10,0b001,0b1000,0b1101,0b000): // BRBINF13_EL1 (RO)
    case SYSENCODE(0b10,0b001,0b1000,0b1110,0b000): // BRBINF14_EL1 (RO)
    case SYSENCODE(0b10,0b001,0b1000,0b1111,0b000): // BRBINF15_EL1 (RO)
    case SYSENCODE(0b10,0b001,0b1000,0b0000,0b100): // BRBINF16_EL1 (RO)
    case SYSENCODE(0b10,0b001,0b1000,0b0001,0b100): // BRBINF17_EL1 (RO)
    case SYSENCODE(0b10,0b001,0b1000,0b0010,0b100): // BRBINF18_EL1 (RO)
    case SYSENCODE(0b10,0b001,0b1000,0b0011,0b100): // BRBINF19_EL1 (RO)
    case SYSENCODE(0b10,0b001,0b1000,0b0100,0b100): // BRBINF20_EL1 (RO)
    case SYSENCODE(0b10,0b001,0b1000,0b0101,0b100): // BRBINF21_EL1 (RO)
    case SYSENCODE(0b10,0b001,0b1000,0b0110,0b100): // BRBINF22_EL1 (RO)
    case SYSENCODE(0b10,0b001,0b1000,0b0111,0b100): // BRBINF23_EL1 (RO)
    case SYSENCODE(0b10,0b001,0b1000,0b1000,0b100): // BRBINF24_EL1 (RO)
    case SYSENCODE(0b10,0b001,0b1000,0b1001,0b100): // BRBINF25_EL1 (RO)
    case SYSENCODE(0b10,0b001,0b1000,0b1010,0b100): // BRBINF26_EL1 (RO)
    case SYSENCODE(0b10,0b001,0b1000,0b1011,0b100): // BRBINF27_EL1 (RO)
    case SYSENCODE(0b10,0b001,0b1000,0b1100,0b100): // BRBINF28_EL1 (RO)
    case SYSENCODE(0b10,0b001,0b1000,0b1101,0b100): // BRBINF29_EL1 (RO)
    case SYSENCODE(0b10,0b001,0b1000,0b1110,0b100): // BRBINF30_EL1 (RO)
    case SYSENCODE(0b10,0b001,0b1000,0b1111,0b100): // BRBINF31_EL1 (RO)
      {
        static struct : public BaseSysReg {
          unsigned GetIdx(Encoding e) const { return e.crm | ((e.op2 & 0b100) << 2); }
          void Name(Encoding e, std::ostream& sink) const override { sink << "BRBINF" << GetIdx(e) << "_EL1"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Branch Record Buffer Information Register " << GetIdx(e); }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b001,0b1000,0b0000,0b001): // BRBSRC0_EL1  (RO)
    case SYSENCODE(0b10,0b001,0b1000,0b0001,0b001): // BRBSRC1_EL1  (RO)
    case SYSENCODE(0b10,0b001,0b1000,0b0010,0b001): // BRBSRC2_EL1  (RO)
    case SYSENCODE(0b10,0b001,0b1000,0b0011,0b001): // BRBSRC3_EL1  (RO)
    case SYSENCODE(0b10,0b001,0b1000,0b0100,0b001): // BRBSRC4_EL1  (RO)
    case SYSENCODE(0b10,0b001,0b1000,0b0101,0b001): // BRBSRC5_EL1  (RO)
    case SYSENCODE(0b10,0b001,0b1000,0b0110,0b001): // BRBSRC6_EL1  (RO)
    case SYSENCODE(0b10,0b001,0b1000,0b0111,0b001): // BRBSRC7_EL1  (RO)
    case SYSENCODE(0b10,0b001,0b1000,0b1000,0b001): // BRBSRC8_EL1  (RO)
    case SYSENCODE(0b10,0b001,0b1000,0b1001,0b001): // BRBSRC9_EL1  (RO)
    case SYSENCODE(0b10,0b001,0b1000,0b1010,0b001): // BRBSRC10_EL1 (RO)
    case SYSENCODE(0b10,0b001,0b1000,0b1011,0b001): // BRBSRC11_EL1 (RO)
    case SYSENCODE(0b10,0b001,0b1000,0b1100,0b001): // BRBSRC12_EL1 (RO)
    case SYSENCODE(0b10,0b001,0b1000,0b1101,0b001): // BRBSRC13_EL1 (RO)
    case SYSENCODE(0b10,0b001,0b1000,0b1110,0b001): // BRBSRC14_EL1 (RO)
    case SYSENCODE(0b10,0b001,0b1000,0b1111,0b001): // BRBSRC15_EL1 (RO)
    case SYSENCODE(0b10,0b001,0b1000,0b0000,0b101): // BRBSRC16_EL1 (RO)
    case SYSENCODE(0b10,0b001,0b1000,0b0001,0b101): // BRBSRC17_EL1 (RO)
    case SYSENCODE(0b10,0b001,0b1000,0b0010,0b101): // BRBSRC18_EL1 (RO)
    case SYSENCODE(0b10,0b001,0b1000,0b0011,0b101): // BRBSRC19_EL1 (RO)
    case SYSENCODE(0b10,0b001,0b1000,0b0100,0b101): // BRBSRC20_EL1 (RO)
    case SYSENCODE(0b10,0b001,0b1000,0b0101,0b101): // BRBSRC21_EL1 (RO)
    case SYSENCODE(0b10,0b001,0b1000,0b0110,0b101): // BRBSRC22_EL1 (RO)
    case SYSENCODE(0b10,0b001,0b1000,0b0111,0b101): // BRBSRC23_EL1 (RO)
    case SYSENCODE(0b10,0b001,0b1000,0b1000,0b101): // BRBSRC24_EL1 (RO)
    case SYSENCODE(0b10,0b001,0b1000,0b1001,0b101): // BRBSRC25_EL1 (RO)
    case SYSENCODE(0b10,0b001,0b1000,0b1010,0b101): // BRBSRC26_EL1 (RO)
    case SYSENCODE(0b10,0b001,0b1000,0b1011,0b101): // BRBSRC27_EL1 (RO)
    case SYSENCODE(0b10,0b001,0b1000,0b1100,0b101): // BRBSRC28_EL1 (RO)
    case SYSENCODE(0b10,0b001,0b1000,0b1101,0b101): // BRBSRC29_EL1 (RO)
    case SYSENCODE(0b10,0b001,0b1000,0b1110,0b101): // BRBSRC30_EL1 (RO)
    case SYSENCODE(0b10,0b001,0b1000,0b1111,0b101): // BRBSRC31_EL1 (RO)
      {
        static struct : public BaseSysReg {
          unsigned GetIdx(Encoding e) const { return e.crm | ((e.op2 & 0b100) << 2); }
          void Name(Encoding e, std::ostream& sink) const override { sink << "BRBSRC" << GetIdx(e) << "_EL1"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Branch Record Buffer Source Address Register " << GetIdx(e); }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b001,0b1000,0b0000,0b010): // BRBTGT0_EL1  (RO)
    case SYSENCODE(0b10,0b001,0b1000,0b0001,0b010): // BRBTGT1_EL1  (RO)
    case SYSENCODE(0b10,0b001,0b1000,0b0010,0b010): // BRBTGT2_EL1  (RO)
    case SYSENCODE(0b10,0b001,0b1000,0b0011,0b010): // BRBTGT3_EL1  (RO)
    case SYSENCODE(0b10,0b001,0b1000,0b0100,0b010): // BRBTGT4_EL1  (RO)
    case SYSENCODE(0b10,0b001,0b1000,0b0101,0b010): // BRBTGT5_EL1  (RO)
    case SYSENCODE(0b10,0b001,0b1000,0b0110,0b010): // BRBTGT6_EL1  (RO)
    case SYSENCODE(0b10,0b001,0b1000,0b0111,0b010): // BRBTGT7_EL1  (RO)
    case SYSENCODE(0b10,0b001,0b1000,0b1000,0b010): // BRBTGT8_EL1  (RO)
    case SYSENCODE(0b10,0b001,0b1000,0b1001,0b010): // BRBTGT9_EL1  (RO)
    case SYSENCODE(0b10,0b001,0b1000,0b1010,0b010): // BRBTGT10_EL1 (RO)
    case SYSENCODE(0b10,0b001,0b1000,0b1011,0b010): // BRBTGT11_EL1 (RO)
    case SYSENCODE(0b10,0b001,0b1000,0b1100,0b010): // BRBTGT12_EL1 (RO)
    case SYSENCODE(0b10,0b001,0b1000,0b1101,0b010): // BRBTGT13_EL1 (RO)
    case SYSENCODE(0b10,0b001,0b1000,0b1110,0b010): // BRBTGT14_EL1 (RO)
    case SYSENCODE(0b10,0b001,0b1000,0b1111,0b010): // BRBTGT15_EL1 (RO)
    case SYSENCODE(0b10,0b001,0b1000,0b0000,0b110): // BRBTGT16_EL1 (RO)
    case SYSENCODE(0b10,0b001,0b1000,0b0001,0b110): // BRBTGT17_EL1 (RO)
    case SYSENCODE(0b10,0b001,0b1000,0b0010,0b110): // BRBTGT18_EL1 (RO)
    case SYSENCODE(0b10,0b001,0b1000,0b0011,0b110): // BRBTGT19_EL1 (RO)
    case SYSENCODE(0b10,0b001,0b1000,0b0100,0b110): // BRBTGT20_EL1 (RO)
    case SYSENCODE(0b10,0b001,0b1000,0b0101,0b110): // BRBTGT21_EL1 (RO)
    case SYSENCODE(0b10,0b001,0b1000,0b0110,0b110): // BRBTGT22_EL1 (RO)
    case SYSENCODE(0b10,0b001,0b1000,0b0111,0b110): // BRBTGT23_EL1 (RO)
    case SYSENCODE(0b10,0b001,0b1000,0b1000,0b110): // BRBTGT24_EL1 (RO)
    case SYSENCODE(0b10,0b001,0b1000,0b1001,0b110): // BRBTGT25_EL1 (RO)
    case SYSENCODE(0b10,0b001,0b1000,0b1010,0b110): // BRBTGT26_EL1 (RO)
    case SYSENCODE(0b10,0b001,0b1000,0b1011,0b110): // BRBTGT27_EL1 (RO)
    case SYSENCODE(0b10,0b001,0b1000,0b1100,0b110): // BRBTGT28_EL1 (RO)
    case SYSENCODE(0b10,0b001,0b1000,0b1101,0b110): // BRBTGT29_EL1 (RO)
    case SYSENCODE(0b10,0b001,0b1000,0b1110,0b110): // BRBTGT30_EL1 (RO)
    case SYSENCODE(0b10,0b001,0b1000,0b1111,0b110): // BRBTGT31_EL1 (RO)
      {
        static struct : public BaseSysReg {
          unsigned GetIdx(Encoding e) const { return e.crm | ((e.op2 & 0b100) << 2); }
          void Name(Encoding e, std::ostream& sink) const override { sink << "BRBTGT" << GetIdx(e) << "_EL1"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Branch Record Buffer Target Address Register " << GetIdx(e); }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b001,0b1001,0b0000,0b000): // BRBCR_EL1 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "BRBCR_EL1"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Branch Record Buffer Control Register (EL1)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b001,0b1001,0b0000,0b001): // BRBFCR_EL1 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "BRBFCR_EL1"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Branch Record Buffer Function Control Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b001,0b1001,0b0000,0b010): // BRBTS_EL1 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "BRBTS_EL1"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Branch Record Buffer Timestamp Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b001,0b1001,0b0001,0b000): // BRBINFINJ_EL1 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "BRBINFINJ_EL1"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Branch Record Buffer Information Injection Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b001,0b1001,0b0001,0b001): // BRBSRCINJ_EL1 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "BRBSRCINJ_EL1"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Branch Record Buffer Source Address Injection Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b001,0b1001,0b0001,0b010): // BRBTGTINJ_EL1 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "BRBTGTINJ_EL1"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Branch Record Buffer Target Address Injection Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b001,0b1001,0b0010,0b000): // BRBIDR0_EL1 (RO)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "BRBIDR0_EL1"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Branch Record Buffer ID0 Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b000,0b0111,0b1001,0b110): // 3.4: DBGCLAIMCLR_EL1, Debug Claim Tag Clear register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "DBGCLAIMCLR_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Debug Claim Tag Clear register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b000,0b0111,0b1000,0b110): // 3.5: DBGCLAIMSET_EL1, Debug Claim Tag Set register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "DBGCLAIMSET_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Debug Claim Tag Set register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b011,0b0000,0b0100,0b000): // 3.6: DBGDTR_EL0, Debug Data Transfer Register, half-duplex
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "DBGDTR_EL0"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Debug Data Transfer Register, half-duplex"; }
        } x; return &x;
      } break;

    // case SYSENCODE(0b10,0b011,0b0000,0b0101,0b000): 3.7: DBGDTRRX_EL0, Debug Data Transfer Register, Receive
    //   {
    //     static struct : public BaseSysReg {
    //       void Name(Encoding, std::ostream& sink) const override { sink << "DBGDTRRX_EL0"; }
    //       void Describe(Encoding, std::ostream& sink) const override { sink << "Debug Data Transfer Register, Receive"; }
    //     } x; return &x;
    //   } break;

    // case SYSENCODE(0b10,0b011,0b0000,0b0101,0b000): 3.8: DBGDTRTX_EL0, Debug Data Transfer Register, Transmit
    //   {
    //     static struct : public BaseSysReg {
    //       void Name(Encoding, std::ostream& sink) const override { sink << "DBGDTRTX_EL0"; }
    //       void Describe(Encoding, std::ostream& sink) const override { sink << "Debug Data Transfer Register, Transmit"; }
    //     } x; return &x;
    //   } break;

    case SYSENCODE(0b10,0b011,0b0000,0b0101,0b000): // 3.8: DBGDTR_EL0, Debug Data Transfer Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "DBGDTR_EL0"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Debug Data Transfer Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b000,0b0001,0b0100,0b100): // 3.9: DBGPRCR_EL1, Debug Power Control Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "DBGPRCR_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Debug Power Control Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b100,0b0000,0b0111,0b000): // 3.10: DBGVCR32_EL2, Debug Vector Catch Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "DBGVCR32_EL2"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Debug Vector Catch Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b100,0b1001,0b0000,0b000): // BRBCR_EL2 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "BRBCR_EL2"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Branch Record Buffer Control Register (EL2)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b101,0b1001,0b0000,0b000): // BRBCR_EL1 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "BRBCR_EL1"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Branch Record Buffer Control Register (EL1)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b0100,0b0101,0b001): // 3.13: DLR_EL0, Debug Link Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "DLR_EL0"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Debug Link Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b0100,0b0101,0b000): // 3.14: DSPSR_EL0, Debug Saved Program Status Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "DSPSR_EL0"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Debug Saved Program Status Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b000,0b0000,0b0010,0b000): // 3.15: MDCCINT_EL1, Monitor DCC Interrupt Enable Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "MDCCINT_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Monitor DCC Interrupt Enable Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b011,0b0000,0b0001,0b000): // 3.16: MDCCSR_EL0, Monitor DCC Status Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "MDCCSR_EL0"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Monitor DCC Status Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b0001,0b0001,0b001): // 3.17: MDCR_EL2, Monitor Debug Configuration Register (EL2)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "MDCR_EL2"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Monitor Debug Configuration Register (EL2)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b110,0b0001,0b0011,0b001): // 3.18: MDCR_EL3, Monitor Debug Configuration Register (EL3)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "MDCR_EL3"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Monitor Debug Configuration Register (EL3)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b000,0b0001,0b0000,0b000): // 3.19: MDRAR_EL1, Monitor Debug ROM Address Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "MDRAR_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Monitor Debug ROM Address Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b000,0b0000,0b0010,0b010): // 3.20: MDSCR_EL1, Monitor Debug System Control Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "MDSCR_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Monitor Debug System Control Register"; }
          void Write(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu, U64 value) const override
          {
            if (cpu.pstate.GetEL() == 0)
              throw AArch64::UndefinedInstructionException();

            struct Bad {}; cpu.dbgmon.SetMDSCR_EL1(cpu.untaint(Bad(), value));
          }
          U64 Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu) const override
          {
            if (cpu.pstate.GetEL() == 0)
              throw AArch64::UndefinedInstructionException();

            return U64(cpu.dbgmon.MDSCR_EL1());
          }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b000,0b0001,0b0011,0b100): // 3.21: OSDLR_EL1, OS Double Lock Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "OSDLR_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "OS Double Lock Register"; }
          void Write(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu, U64 value) const override { cpu.untaint(SBZTV(), value); }
          U64 Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu) const override { return U64(0); }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b000,0b0000,0b0000,0b010): // 3.22: OSDTRRX_EL1, OS Lock Data Transfer Register, Receive
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "OSDTRRX_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "OS Lock Data Transfer Register, Receive"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b000,0b0000,0b0011,0b010): // 3.23: OSDTRTX_EL1, OS Lock Data Transfer Register, Transmit
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "OSDTRTX_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "OS Lock Data Transfer Register, Transmit"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b000,0b0000,0b0110,0b010): // 3.24: OSECCR_EL1, OS Lock Exception Catch Control Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "OSECCR_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "OS Lock Exception Catch Control Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b000,0b0001,0b0000,0b100): // 3.25: OSLAR_EL1, OS Lock Access Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "OSLAR_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "OS Lock Access Register"; }
          void Write(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu, U64 value) const override { cpu.untaint(SBZTV(), value); }
          U64 Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu) const override { return U64(0); }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b000,0b0001,0b0001,0b100): // 3.26: OSLSR_EL1, OS Lock Status Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "OSLSR_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "OS Lock Status Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b110,0b0001,0b0001,0b001): // 3.27: SDER32_EL3, AArch32 Secure Debug Enable Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "SDER32_EL3"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "AArch32 Secure Debug Enable Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1001,0b1001,0b000): // PMSCR_EL1 (RW)
    case SYSENCODE(0b11,0b101,0b1001,0b1001,0b000): // PMSCR_EL1 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "PMSCR_EL1"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Statistical Profiling Control Register (EL1)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1001,0b1001,0b000): // PMSCR_EL2 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "PMSCR_EL2"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Statistical Profiling Control Register (EL2)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1001,0b1001,0b001): // PMSNEVFR_EL1 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "PMSNEVFR_EL1"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Sampling Inverted Event Filter Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1001,0b1001,0b010): // PMSICR_EL1 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "PMSICR_EL1"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Sampling Interval Counter Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1001,0b1001,0b011): // PMSIRR_EL1 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "PMSIRR_EL1"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Sampling Interval Reload Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1001,0b1001,0b100): // PMSFCR_EL1 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "PMSFCR_EL1"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Sampling Filter Control Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1001,0b1001,0b101): // PMSEVFR_EL1 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "PMSEVFR_EL1"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Sampling Event Filter Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1001,0b1001,0b110): // PMSLATFR_EL1 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "PMSLATFR_EL1"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Sampling Latency Filter Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1001,0b1001,0b111): // PMSIDR_EL1 (RO)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "PMSIDR_EL1"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Sampling Profiling ID Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1001,0b1010,0b000): // PMBLIMITR_EL1 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "PMBLIMITR_EL1"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Profiling Buffer Limit Address Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1001,0b1010,0b001): // PMBPTR_EL1 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "PMBPTR_EL1"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Profiling Buffer Write Pointer Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1001,0b1010,0b011): // PMBSR_EL1 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "PMBSR_EL1"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Profiling Buffer Status/syndrome Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1001,0b1010,0b111): // PMBIDR_EL1 (RO)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "PMBIDR_EL1"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Profiling Buffer ID Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1001,0b1011,0b000): // TRBLIMITR_EL1 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "TRBLIMITR_EL1"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Trace Buffer Limit Address Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1001,0b1011,0b001): // TRBPTR_EL1 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "TRBPTR_EL1"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "TRBSR_EL1, and TRBTRG_EL1."; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1001,0b1011,0b010): // TRBBASER_EL1 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "TRBBASER_EL1"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Trace Buffer Base Address Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1001,0b1011,0b011): // TRBSR_EL1 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "TRBSR_EL1"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Trace Buffer Status/syndrome Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1001,0b1011,0b100): // TRBMAR_EL1 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "TRBMAR_EL1"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Trace Buffer Memory Attribute Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1001,0b1011,0b110): // TRBTRG_EL1 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "TRBTRG_EL1"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Trace Buffer Trigger Counter Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1001,0b1011,0b111): // TRBIDR_EL1 (RO)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "TRBIDR_EL1"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Trace Buffer ID Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1110,0b1111,0b111): // 4.1: PMCCFILTR_EL0, Performance Monitors Cycle Count Filter Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "PMCCFILTR_EL0"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Performance Monitors Cycle Count Filter Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1001,0b1101,0b000): // 4.2: PMCCNTR_EL0, Performance Monitors Cycle Count Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "PMCCNTR_EL0"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Performance Monitors Cycle Count Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1001,0b1100,0b110): // 4.3: PMCEID0_EL0, Performance Monitors Common Event Identification register 0
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "PMCEID0_EL0"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Performance Monitors Common Event Identification register 0"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1001,0b1100,0b111): // 4.4: PMCEID1_EL0, Performance Monitors Common Event Identification register 1
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "PMCEID1_EL0"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Performance Monitors Common Event Identification register 1"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1001,0b1100,0b010): // 4.5: PMCNTENCLR_EL0, Performance Monitors Count Enable Clear register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "PMCNTENCLR_EL0"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Performance Monitors Count Enable Clear register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1001,0b1100,0b001): // 4.6: PMCNTENSET_EL0, Performance Monitors Count Enable Set register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "PMCNTENSET_EL0"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Performance Monitors Count Enable Set register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1001,0b1100,0b000): // 4.7: PMCR_EL0, Performance Monitors Control Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "PMCR_EL0"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Performance Monitors Control Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1001,0b1110,0b010): // 4.10: PMINTENCLR_EL1, Performance Monitors Interrupt Enable Clear register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "PMINTENCLR_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Performance Monitors Interrupt Enable Clear register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1001,0b1110,0b001): // 4.11: PMINTENSET_EL1, Performance Monitors Interrupt Enable Set register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "PMINTENSET_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Performance Monitors Interrupt Enable Set register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1001,0b1100,0b011): // 4.12: PMOVSCLR_EL0, Performance Monitors Overflow Flag Status Clear Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "PMOVSCLR_EL0"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Performance Monitors Overflow Flag Status Clear Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1001,0b1110,0b011): // 4.13: PMOVSSET_EL0, Performance Monitors Overflow Flag Status Set register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "PMOVSSET_EL0"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Performance Monitors Overflow Flag Status Set register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1001,0b1100,0b101): // 4.14: PMSELR_EL0, Performance Monitors Event Counter Selection Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "PMSELR_EL0"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Performance Monitors Event Counter Selection Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1001,0b1100,0b100): // 4.15: PMSWINC_EL0, Performance Monitors Software Increment register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "PMSWINC_EL0"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Performance Monitors Software Increment register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1001,0b1110,0b000): // 4.16: PMUSERENR_EL0, Performance Monitors User Enable Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "PMUSERENR_EL0"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Performance Monitors User Enable Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1001,0b1101,0b010): // 4.17: PMXEVCNTR_EL0, Performance Monitors Selected Event Count Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "PMXEVCNTR_EL0"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Performance Monitors Selected Event Count Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1001,0b1101,0b001): // 4.18: PMXEVTYPER_EL0, Performance Monitors Selected Event Type Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "PMXEVTYPER_EL0"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Performance Monitors Selected Event Type Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1001,0b1110,0b110): // PMMIR_EL1 (RO)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "PMMIR_EL1"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Performance Monitors Machine Identification Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1110,0b0000,0b000): // 5.1: CNTFRQ_EL0, Counter-timer Frequency register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "CNTFRQ_EL0"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Counter-timer Frequency register"; }
          U64 Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu) const override { return U64(cpu.vt.get_cntfrq()); }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1110,0b0001,0b000): // 5.2: CNTHCTL_EL2, Counter-timer Hypervisor Control register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "CNTHCTL_EL2"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Counter-timer Hypervisor Control register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1110,0b0010,0b001): // 5.3: CNTHP_CTL_EL2, Counter-timer Hypervisor Physical Timer Control register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "CNTHP_CTL_EL2"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Counter-timer Hypervisor Physical Timer Control register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1110,0b0010,0b010): // 5.4: CNTHP_CVAL_EL2, Counter-timer Hypervisor Physical Timer CompareValue register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "CNTHP_CVAL_EL2"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Counter-timer Hypervisor Physical Timer CompareValue register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1110,0b0010,0b000): // 5.5: CNTHP_TVAL_EL2, Counter-timer Hypervisor Physical Timer TimerValue register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "CNTHP_TVAL_EL2"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Counter-timer Hypervisor Physical Timer TimerValue register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1110,0b0011,0b000): // CNTHV_TVAL_EL2
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "CNTHV_TVAL_EL2"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Counter-timer Virtual Timer TimerValue Register (EL2)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1110,0b0011,0b001): // CNTHV_CTL_EL2 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "CNTHV_CTL_EL2"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Counter-timer Virtual Timer Control register (EL2)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1110,0b0011,0b010): // CNTHV_CVAL_EL2 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "CNTHV_CVAL_EL2"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Counter-timer Virtual Timer CompareValue register (EL2)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1110,0b0100,0b000): // CNTHVS_TVAL_EL2
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "CNTHVS_TVAL_EL2"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Counter-timer Secure Virtual Timer TimerValue register (EL2)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1110,0b0100,0b001): // CNTHVS_CTL_EL2 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "CNTHVS_CTL_EL2"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Counter-timer Secure Virtual Timer Control register (EL2)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1110,0b0100,0b010): // CNTHVS_CVAL_EL2 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "CNTHVS_CVAL_EL2"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Counter-timer Secure Virtual Timer CompareValue register (EL2)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1110,0b0101,0b000): // CNTHPS_TVAL_EL2
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "CNTHPS_TVAL_EL2"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Counter-timer Secure Physical Timer TimerValue register (EL2)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1110,0b0101,0b001): // CNTHPS_CTL_EL2 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "CNTHPS_CTL_EL2"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Counter-timer Secure Physical Timer Control register (EL2)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1110,0b0101,0b010): // CNTHPS_CVAL_EL2 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "CNTHPS_CVAL_EL2"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Counter-timer Secure Physical Timer CompareValue register (EL2)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1110,0b0001,0b000): // 5.6: CNTKCTL_EL1, Counter-timer Kernel Control register
    case SYSENCODE(0b11,0b101,0b1110,0b0001,0b000): // CNTKCTL_EL1 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "CNTKCTL_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Counter-timer Kernel Control register"; }
          U64 Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu) const override
          {
            if (cpu.pstate.GetEL() == 0)
              throw AArch64::UndefinedInstructionException();

            return U64(cpu.vt.kctl);
          }
          void Write(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu, U64 value) const override
          {
            if (cpu.pstate.GetEL() == 0)
              throw AArch64::UndefinedInstructionException();

            struct Bad {}; cpu.vt.write_kctl( cpu, cpu.untaint(Bad(), value) );
          }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1110,0b0010,0b001): // 5.7: CNTP_CTL_EL0, Counter-timer Physical Timer Control register
    case SYSENCODE(0b11,0b101,0b1110,0b0010,0b001): // CNTP_CTL_EL0 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "CNTP_CTL_EL0"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Counter-timer Physical Timer Control register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1110,0b0010,0b010): // 5.8: CNTP_CVAL_EL0, Counter-timer Physical Timer CompareValue register
    case SYSENCODE(0b11,0b101,0b1110,0b0010,0b010): // CNTP_CVAL_EL0 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "CNTP_CVAL_EL0"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Counter-timer Physical Timer CompareValue register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1110,0b0010,0b000): // 5.9: CNTP_TVAL_EL0, Counter-timer Physical Timer TimerValue register
    case SYSENCODE(0b11,0b101,0b1110,0b0010,0b000): // CNTP_TVAL_EL0
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "CNTP_TVAL_EL0"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Counter-timer Physical Timer TimerValue register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1110,0b0000,0b001): // 5.10: CNTPCT_EL0, Counter-timer Physical Count register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "CNTPCT_EL0"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Counter-timer Physical Count register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b111,0b1110,0b0010,0b001): // 5.11: CNTPS_CTL_EL1, Counter-timer Physical Secure Timer Control register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "CNTPS_CTL_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Counter-timer Physical Secure Timer Control register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b111,0b1110,0b0010,0b010): // 5.12: CNTPS_CVAL_EL1, Counter-timer Physical Secure Timer CompareValue register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "CNTPS_CVAL_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Counter-timer Physical Secure Timer CompareValue register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b111,0b1110,0b0010,0b000): // 5.13: CNTPS_TVAL_EL1, Counter-timer Physical Secure Timer TimerValue register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "CNTPS_TVAL_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Counter-timer Physical Secure Timer TimerValue register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1110,0b0011,0b001): // 5.14: CNTV_CTL_EL0, Counter-timer Virtual Timer Control register
    case SYSENCODE(0b11,0b101,0b1110,0b0011,0b001): // CNTV_CTL_EL0 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "CNTV_CTL_EL0"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Counter-timer Virtual Timer Control register"; }
          U64 Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu) const override { return U64(cpu.vt.read_ctl(cpu)); }
          void Write(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu, U64 value) const override { struct Bad {}; cpu.vt.write_ctl(cpu, cpu.untaint(Bad(), value)); }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1110,0b0011,0b010): // 5.15: CNTV_CVAL_EL0, Counter-timer Virtual Timer CompareValue register
    case SYSENCODE(0b11,0b101,0b1110,0b0011,0b010): // CNTV_CVAL_EL0 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "CNTV_CVAL_EL0"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Counter-timer Virtual Timer CompareValue register"; }
          U64 Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu) const override { return U64(cpu.vt.cval); }
          void Write(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu, U64 value) const override { struct Bad {}; cpu.vt.write_cval(cpu, cpu.untaint(Bad(), value)); }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1110,0b0011,0b000): // 5.16: CNTV_TVAL_EL0, Counter-timer Virtual Timer TimerValue register
    case SYSENCODE(0b11,0b101,0b1110,0b0011,0b000): // CNTV_TVAL_EL0
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "CNTV_TVAL_EL0"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Counter-timer Virtual Timer TimerValue register"; }
          U64 Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu) const override { return U64(uint32_t(cpu.vt.read_tval(cpu))); }
          void Write(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu, U64 value) const override { struct Bad {}; cpu.vt.write_tval(cpu, cpu.untaint(Bad(), value)); }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1110,0b0000,0b010): // 5.17: CNTVCT_EL0, Counter-timer Virtual Count register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "CNTVCT_EL0"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Counter-timer Virtual Count register"; }
          U64 Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu) const override
          {
            /* The virtual count value is equal to the physical count
             * value minus the virtual offset visible in CNTVOFF_EL2.
             * If EL2 not implemented, the offset is fixed as 0.
             */
            return U64(cpu.vt.get_pcount(cpu));
          }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1110,0b0000,0b101): // CNTPCTSS_EL0
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "CNTPCTSS_EL0"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Counter-timer Self-Synchronized Physical Count register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1110,0b0000,0b110): // CNTVCTSS_EL0
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "CNTVCTSS_EL0"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Counter-timer Self-Synchronized Virtual Count register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1110,0b0000,0b011): // 5.18: CNTVOFF_EL2, Counter-timer Virtual Offset register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "CNTVOFF_EL2"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Counter-timer Virtual Offset register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1110,0b0000,0b110): // CNTPOFF_EL2 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "CNTPOFF_EL2"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Counter-timer Physical Offset register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b1000,0b100): // 6.1: ICC_AP0R0_EL1, Interrupt Controller Active Priorities Register (0,0)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ICC_AP0R0_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Interrupt Controller Active Priorities Register (0,0)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b1000,0b101): // 6.2: ICC_AP0R1_EL1, Interrupt Controller Active Priorities Register (0,1)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ICC_AP0R1_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Interrupt Controller Active Priorities Register (0,1)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b1000,0b110): // 6.3: ICC_AP0R2_EL1, Interrupt Controller Active Priorities Register (0,2)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ICC_AP0R2_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Interrupt Controller Active Priorities Register (0,2)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b1000,0b111): // 6.4: ICC_AP0R3_EL1, Interrupt Controller Active Priorities Register (0,3)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ICC_AP0R3_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Interrupt Controller Active Priorities Register (0,3)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b1001,0b000): // 6.5: ICC_AP1R0_EL1, Interrupt Controller Active Priorities Register (1,0)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ICC_AP1R0_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Interrupt Controller Active Priorities Register (1,0)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b1001,0b001): // 6.6: ICC_AP1R1_EL1, Interrupt Controller Active Priorities Register (1,1)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ICC_AP1R1_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Interrupt Controller Active Priorities Register (1,1)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b1001,0b010): // 6.7: ICC_AP1R2_EL1, Interrupt Controller Active Priorities Register (1,2)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ICC_AP1R2_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Interrupt Controller Active Priorities Register (1,2)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b1001,0b011): // 6.8: ICC_AP1R3_EL1, Interrupt Controller Active Priorities Register (1,3)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ICC_AP1R3_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Interrupt Controller Active Priorities Register (1,3)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b1011,0b110): // 6.9: ICC_ASGI1R_EL1, Interrupt Controller Alias Software Generated Interrupt group 1 Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ICC_ASGI1R_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Interrupt Controller Alias Software Generated Interrupt group 1 Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b1000,0b011): // 6.10: ICC_BPR0_EL1, Interrupt Controller Binary Point Register 0
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ICC_BPR0_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Interrupt Controller Binary Point Register 0"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b1100,0b011): // 6.11: ICC_BPR1_EL1, Interrupt Controller Binary Point Register 1
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ICC_BPR1_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Interrupt Controller Binary Point Register 1"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b1100,0b100): // 6.12: ICC_CTLR_EL1, Interrupt Controller Control Register (EL1)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ICC_CTLR_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Interrupt Controller Control Register (EL1)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b110,0b1100,0b1100,0b100): // 6.13: ICC_CTLR_EL3, Interrupt Controller Control Register (EL3)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ICC_CTLR_EL3"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Interrupt Controller Control Register (EL3)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b1001,0b101): // ICC_NMIAR1_EL1 (RO)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "ICC_NMIAR1_EL1"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Interrupt Controller Non-maskable Interrupt Acknowledge Register 1"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b1011,0b001): // 6.14: ICC_DIR_EL1, Interrupt Controller Deactivate Interrupt Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ICC_DIR_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Interrupt Controller Deactivate Interrupt Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b1000,0b001): // 6.15: ICC_EOIR0_EL1, Interrupt Controller End Of Interrupt Register 0
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ICC_EOIR0_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Interrupt Controller End Of Interrupt Register 0"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b1100,0b001): // 6.16: ICC_EOIR1_EL1, Interrupt Controller End Of Interrupt Register 1
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ICC_EOIR1_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Interrupt Controller End Of Interrupt Register 1"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b1000,0b010): // 6.17: ICC_HPPIR0_EL1, Interrupt Controller Highest Priority Pending Interrupt Register 0
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ICC_HPPIR0_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Interrupt Controller Highest Priority Pending Interrupt Register 0"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b1100,0b010): // 6.18: ICC_HPPIR1_EL1, Interrupt Controller Highest Priority Pending Interrupt Register 1
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ICC_HPPIR1_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Interrupt Controller Highest Priority Pending Interrupt Register 1"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b1000,0b000): // 6.19: ICC_IAR0_EL1, Interrupt Controller Interrupt Acknowledge Register 0
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ICC_IAR0_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Interrupt Controller Interrupt Acknowledge Register 0"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b1100,0b000): // 6.20: ICC_IAR1_EL1, Interrupt Controller Interrupt Acknowledge Register 1
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ICC_IAR1_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Interrupt Controller Interrupt Acknowledge Register 1"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b1100,0b110): // 6.21: ICC_IGRPEN0_EL1, Interrupt Controller Interrupt Group 0 Enable register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ICC_IGRPEN0_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Interrupt Controller Interrupt Group 0 Enable register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b1100,0b111): // 6.22: ICC_IGRPEN1_EL1, Interrupt Controller Interrupt Group 1 Enable register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ICC_IGRPEN1_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Interrupt Controller Interrupt Group 1 Enable register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b110,0b1100,0b1100,0b111): // 6.23: ICC_IGRPEN1_EL3, Interrupt Controller Interrupt Group 1 Enable register (EL3)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ICC_IGRPEN1_EL3"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Interrupt Controller Interrupt Group 1 Enable register (EL3)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0100,0b0110,0b000): // 6.24: ICC_PMR_EL1, Interrupt Controller Interrupt Priority Mask Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ICC_PMR_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Interrupt Controller Interrupt Priority Mask Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b1011,0b011): // 6.25: ICC_RPR_EL1, Interrupt Controller Running Priority Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ICC_RPR_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Interrupt Controller Running Priority Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b1101,0b000): // 6.26: ICC_SEIEN_EL1, Interrupt Controller System Error Interrupt Enable register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ICC_SEIEN_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Interrupt Controller System Error Interrupt Enable register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b1011,0b111): // 6.27: ICC_SGI0R_EL1, Interrupt Controller Software Generated Interrupt group 0 Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ICC_SGI0R_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Interrupt Controller Software Generated Interrupt group 0 Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b1011,0b101): // 6.28: ICC_SGI1R_EL1, Interrupt Controller Software Generated Interrupt group 1 Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ICC_SGI1R_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Interrupt Controller Software Generated Interrupt group 1 Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b1100,0b101): // 6.29: ICC_SRE_EL1, Interrupt Controller System Register Enable register (EL1)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ICC_SRE_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Interrupt Controller System Register Enable register (EL1)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1100,0b1001,0b101): // 6.30: ICC_SRE_EL2, Interrupt Controller System Register Enable register (EL2)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ICC_SRE_EL2"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Interrupt Controller System Register Enable register (EL2)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b110,0b1100,0b1100,0b101): // 6.31: ICC_SRE_EL3, Interrupt Controller System Register Enable register (EL3)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ICC_SRE_EL3"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Interrupt Controller System Register Enable register (EL3)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1100,0b1000,0b000): // 6.32: ICH_AP0R0_EL2, Interrupt Controller Hyp Active Priorities Register (0,0)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ICH_AP0R0_EL2"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Interrupt Controller Hyp Active Priorities Register (0,0)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1100,0b1000,0b001): // 6.33: ICH_AP0R1_EL2, Interrupt Controller Hyp Active Priorities Register (0,1)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ICH_AP0R1_EL2"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Interrupt Controller Hyp Active Priorities Register (0,1)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1100,0b1000,0b010): // 6.34: ICH_AP0R2_EL2, Interrupt Controller Hyp Active Priorities Register (0,2)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ICH_AP0R2_EL2"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Interrupt Controller Hyp Active Priorities Register (0,2)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1100,0b1000,0b011): // 6.35: ICH_AP0R3_EL2, Interrupt Controller Hyp Active Priorities Register (0,3)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ICH_AP0R3_EL2"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Interrupt Controller Hyp Active Priorities Register (0,3)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1100,0b1001,0b000): // 6.36: ICH_AP1R0_EL2, Interrupt Controller Hyp Active Priorities Register (1,0)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ICH_AP1R0_EL2"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Interrupt Controller Hyp Active Priorities Register (1,0)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1100,0b1001,0b001): // 6.37: ICH_AP1R1_EL2, Interrupt Controller Hyp Active Priorities Register (1,1)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ICH_AP1R1_EL2"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Interrupt Controller Hyp Active Priorities Register (1,1)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1100,0b1001,0b010): // 6.38: ICH_AP1R2_EL2, Interrupt Controller Hyp Active Priorities Register (1,2)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ICH_AP1R2_EL2"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Interrupt Controller Hyp Active Priorities Register (1,2)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1100,0b1001,0b011): // 6.39: ICH_AP1R3_EL2, Interrupt Controller Hyp Active Priorities Register (1,3)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ICH_AP1R3_EL2"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Interrupt Controller Hyp Active Priorities Register (1,3)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1100,0b1011,0b011): // 6.40: ICH_EISR_EL2, Interrupt Controller End of Interrupt Status Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ICH_EISR_EL2"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Interrupt Controller End of Interrupt Status Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1100,0b1011,0b101): // 6.41: ICH_ELSR_EL2, Interrupt Controller Empty List Register Status Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ICH_ELSR_EL2"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Interrupt Controller Empty List Register Status Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1100,0b1011,0b000): // 6.42: ICH_HCR_EL2, Interrupt Controller Hyp Control Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ICH_HCR_EL2"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Interrupt Controller Hyp Control Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1100,0b1011,0b010): // 6.44: ICH_MISR_EL2, Interrupt Controller Maintenance Interrupt State Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ICH_MISR_EL2"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Interrupt Controller Maintenance Interrupt State Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1100,0b1011,0b111): // 6.45: ICH_VMCR_EL2, Interrupt Controller Virtual Machine Control Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ICH_VMCR_EL2"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Interrupt Controller Virtual Machine Control Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1100,0b1001,0b100): // 6.46: ICH_VSEIR_EL2, Interrupt Controller Virtual System Error Interrupt Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ICH_VSEIR_EL2"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Interrupt Controller Virtual System Error Interrupt Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1100,0b1011,0b001): // 6.47: ICH_VTR_EL2, Interrupt Controller VGIC Type Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ICH_VTR_EL2"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Interrupt Controller VGIC Type Register"; }
        } x; return &x;
      } break;

      /*** IMPLEMENTATION DEFINED registers ***/
    case SYSENCODE(0b11,0b011,0b1111,0b0001,0b000): // s3_3_c15_c1_0: File Transfer In
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "USVP_GET_EL0"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "File Transfer In"; }
          U64 Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu) const override { return U64(cpu.transfer.get(cpu)); }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1111,0b0001,0b001): // s3_3_c15_c1_1: File Transfer Out
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "USVP_PUT_EL0"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "File Transfer Out"; }
          U64 Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu) const override { return U64(cpu.transfer.put(cpu)); }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1111,0b0001,0b110): // s3_3_c15_c1_1: cmd
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "USVP_CMD"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Sim Command"; }
          void Write(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu, U64 value) const override
          {
            struct Bad {};
            uint64_t cmd_id = cpu.untaint(Bad(), value);
            struct Cmd { virtual ~Cmd() {} virtual char const* Name() const = 0; virtual void Run() const = 0; } *cmd = 0;

            switch (cmd_id)
              {
              default: throw 0;
              case 0: { static struct SuspCmd : Cmd { char const* Name() const { return "suspend"; } void Run() const { throw Suspend(); } } _; cmd = &_; };
              case 1: { static struct StopCmd : Cmd { char const* Name() const { return "stop"; } void Run() const { throw Stop(); } } _; cmd = &_; };
              }

            std::cerr << "Received <usvp-cmd " << cmd->Name() << ">\n";
            cmd->Run();
          }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1111,0b0001,0b111): // s3_3_c15_c1_7: Taint Swap
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "IMP_TAINT_SWAP_EL0"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Taint Swap register"; }
          U64 Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu) const override { return cpu.tvreg; }
          void Write(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu, U64 value) const override { std::swap(value.value, value.ubits); cpu.tvreg = value; }
        } x; return &x;
      } break;
    }

  /*  System instructions with immediate, CRm used as immediate field,
   *  and thus ignored during decode.
   */
  switch (SYSENCODE(op0, op1, crn, 0, op2))
    {
    case SYSENCODE(0b00,0b011,0b0011,0b0000,0b100):
    case SYSENCODE(0b00,0b011,0b0011,0b0000,0b101):
    case SYSENCODE(0b00,0b011,0b0011,0b0000,0b110):
      {
        static struct : public WOOpSysReg
        {
          virtual void Disasm(Encoding e, std::ostream& sink) const override
          {
            char const* ops[] = {"dsb", "dmb", "isb"};
            sink << ops[e.op2&3] << '\t' << unisim::component::cxx::processor::arm::isa::arm64::DisasmBarrierOption(e.crm);
          }
        } x; return &x;
      } break;

    case SYSENCODE(0b00,0b000,0b0100,0b0000,0b101):
      {
        static struct : public PStateSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "SPSel"; }
          virtual void Write(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu, U64 value) const override
          {
            if (cpu.pstate.GetEL() == 0)
              throw AArch64::UndefinedInstructionException();

            cpu.pstate.SetSP(cpu, crm);
          }
        } x; return &x;
      } break;

    case SYSENCODE(0b00,0b011,0b0100,0b0000,0b110):
      {
        static struct : public PStateSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "DAIFSet"; }
          virtual void Write(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu, U64 value) const override
          {
            if (cpu.pstate.GetEL() == 0)
              {
                bool uma = unisim::util::arithmetic::BitField<9,1>().Get(cpu.get_el(1).SCTLR);
                if (!uma)
                  throw AArch64::SystemAccessTrapException(op0, op1, crn, crm, op2, rt, /* direction */ 0);
              }

            cpu.pstate.D |= crm >> 3;
            cpu.pstate.A |= crm >> 2;
            cpu.pstate.I |= crm >> 1;
            cpu.pstate.F |= crm >> 0;
          }
        } x; return &x;
      } break;

    case SYSENCODE(0b00,0b011,0b0100,0b0000,0b111):
      {
        static struct : public PStateSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "DAIFClr"; }
          virtual void Write(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu, U64 value) const override
          {
            if (cpu.pstate.GetEL() == 0)
              {
                bool uma = unisim::util::arithmetic::BitField<9,1>().Get(cpu.get_el(1).SCTLR);
                if (!uma)
                  throw AArch64::SystemAccessTrapException(op0, op1, crn, crm, op2, rt, /* direction */ 0);
              }

            cpu.pstate.D &= ~crm >> 3;
            cpu.pstate.A &= ~crm >> 2;
            cpu.pstate.I &= ~crm >> 1;
            cpu.pstate.F &= ~crm >> 0;
            cpu.gic.program(cpu);
          }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b000,0b0000,0b0000,0b101): // 3.2: DBGBCR<n>_EL1, Debug Breakpoint Control Registers, n = 0 - 15
    case SYSENCODE(0b10,0b000,0b0000,0b0001,0b101):
    case SYSENCODE(0b10,0b000,0b0000,0b0010,0b101):
    case SYSENCODE(0b10,0b000,0b0000,0b0011,0b101):
    case SYSENCODE(0b10,0b000,0b0000,0b0100,0b101):
    case SYSENCODE(0b10,0b000,0b0000,0b0101,0b101):
    case SYSENCODE(0b10,0b000,0b0000,0b0110,0b101):
    case SYSENCODE(0b10,0b000,0b0000,0b0111,0b101):
    case SYSENCODE(0b10,0b000,0b0000,0b1000,0b101):
    case SYSENCODE(0b10,0b000,0b0000,0b1001,0b101):
    case SYSENCODE(0b10,0b000,0b0000,0b1010,0b101):
    case SYSENCODE(0b10,0b000,0b0000,0b1011,0b101):
    case SYSENCODE(0b10,0b000,0b0000,0b1100,0b101):
    case SYSENCODE(0b10,0b000,0b0000,0b1101,0b101):
    case SYSENCODE(0b10,0b000,0b0000,0b1110,0b101):
    case SYSENCODE(0b10,0b000,0b0000,0b1111,0b101):
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "DBGBCR" << std::dec << int(e.crm) << "_EL1"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Debug Breakpoint Control Register #" << std::dec << int(e.crm); }
          void Write(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu, U64 value) const override
          {
            if (crm >= DebugMonitor::BRPS)
              BaseSysReg::Write(op0, op1, crn, crm, op2, rt, cpu, value);

            if (cpu.pstate.GetEL() == 0)
              throw AArch64::UndefinedInstructionException();

            struct Bad {}; cpu.dbgmon.SetDBGBCR_EL1(crm, cpu.untaint(Bad(), value));
          }
          U64 Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu) const override
          {
            if (crm >= DebugMonitor::BRPS)
              return BaseSysReg::Read(op0, op1, crn, crm, op2, rt, cpu);

            if (cpu.pstate.GetEL() == 0)
              throw AArch64::UndefinedInstructionException();

            return U64(cpu.dbgmon.DBGBCR_EL1(crm));
          }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b000,0b0000,0b0000,0b100): // 3.3: DBGBVR<n>_EL1, Debug Breakpoint Value Registers, n = 0 - 15
    case SYSENCODE(0b10,0b000,0b0000,0b0001,0b100):
    case SYSENCODE(0b10,0b000,0b0000,0b0010,0b100):
    case SYSENCODE(0b10,0b000,0b0000,0b0011,0b100):
    case SYSENCODE(0b10,0b000,0b0000,0b0100,0b100):
    case SYSENCODE(0b10,0b000,0b0000,0b0101,0b100):
    case SYSENCODE(0b10,0b000,0b0000,0b0110,0b100):
    case SYSENCODE(0b10,0b000,0b0000,0b0111,0b100):
    case SYSENCODE(0b10,0b000,0b0000,0b1000,0b100):
    case SYSENCODE(0b10,0b000,0b0000,0b1001,0b100):
    case SYSENCODE(0b10,0b000,0b0000,0b1010,0b100):
    case SYSENCODE(0b10,0b000,0b0000,0b1011,0b100):
    case SYSENCODE(0b10,0b000,0b0000,0b1100,0b100):
    case SYSENCODE(0b10,0b000,0b0000,0b1101,0b100):
    case SYSENCODE(0b10,0b000,0b0000,0b1110,0b100):
    case SYSENCODE(0b10,0b000,0b0000,0b1111,0b100):
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "DBGBVR" << std::dec << int(e.crm) << "_EL1"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Debug Breakpoint Value Register #" << std::dec << int(e.crm); }
          void Write(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu, U64 value) const override
          {
            if (crm >= DebugMonitor::BRPS)
              BaseSysReg::Write(op0, op1, crn, crm, op2, rt, cpu, value);

            if (cpu.pstate.GetEL() == 0)
              throw AArch64::UndefinedInstructionException();

            struct Bad {}; cpu.dbgmon.SetDBGBVR_EL1(crm, cpu.untaint(Bad(), value));
          }
          U64 Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu) const override
          {
            if (crm >= DebugMonitor::BRPS)
              return BaseSysReg::Read(op0, op1, crn, crm, op2, rt, cpu);

            if (cpu.pstate.GetEL() == 0)
              throw AArch64::UndefinedInstructionException();

            return U64(cpu.dbgmon.DBGBVR_EL1(crm));
          }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b000,0b0000,0b0000,0b111): // 3.11: DBGWCR<n>_EL1, Debug Watchpoint Control Registers, n = 0 - 15
    case SYSENCODE(0b10,0b000,0b0000,0b0001,0b111):
    case SYSENCODE(0b10,0b000,0b0000,0b0010,0b111):
    case SYSENCODE(0b10,0b000,0b0000,0b0011,0b111):
    case SYSENCODE(0b10,0b000,0b0000,0b0100,0b111):
    case SYSENCODE(0b10,0b000,0b0000,0b0101,0b111):
    case SYSENCODE(0b10,0b000,0b0000,0b0110,0b111):
    case SYSENCODE(0b10,0b000,0b0000,0b0111,0b111):
    case SYSENCODE(0b10,0b000,0b0000,0b1000,0b111):
    case SYSENCODE(0b10,0b000,0b0000,0b1001,0b111):
    case SYSENCODE(0b10,0b000,0b0000,0b1010,0b111):
    case SYSENCODE(0b10,0b000,0b0000,0b1011,0b111):
    case SYSENCODE(0b10,0b000,0b0000,0b1100,0b111):
    case SYSENCODE(0b10,0b000,0b0000,0b1101,0b111):
    case SYSENCODE(0b10,0b000,0b0000,0b1110,0b111):
    case SYSENCODE(0b10,0b000,0b0000,0b1111,0b111):
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "DBGWCR" << std::dec << int(e.crm) << "_EL1"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Debug Watchpoint Control Register#" << std::dec << int(e.crm); }
          void Write(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu, U64 value) const override
          {
            if (crm >= DebugMonitor::WRPS)
              BaseSysReg::Write(op0, op1, crn, crm, op2, rt, cpu, value);

            if (cpu.pstate.GetEL() == 0)
              throw AArch64::UndefinedInstructionException();

            struct Bad {}; cpu.dbgmon.SetDBGWCR_EL1(crm, cpu.untaint(Bad(), value));
          }
          U64 Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu) const override
          {
            if (crm >= DebugMonitor::WRPS)
              return BaseSysReg::Read(op0, op1, crn, crm, op2, rt, cpu);

            if (cpu.pstate.GetEL() == 0)
              throw AArch64::UndefinedInstructionException();

            return U64(cpu.dbgmon.DBGWCR_EL1(crm));
          }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b000,0b0000,0b0000,0b110): // 3.12: DBGWVR<n>_EL1, Debug Watchpoint Value Registers, n = 0 - 15
    case SYSENCODE(0b10,0b000,0b0000,0b0001,0b110):
    case SYSENCODE(0b10,0b000,0b0000,0b0010,0b110):
    case SYSENCODE(0b10,0b000,0b0000,0b0011,0b110):
    case SYSENCODE(0b10,0b000,0b0000,0b0100,0b110):
    case SYSENCODE(0b10,0b000,0b0000,0b0101,0b110):
    case SYSENCODE(0b10,0b000,0b0000,0b0110,0b110):
    case SYSENCODE(0b10,0b000,0b0000,0b0111,0b110):
    case SYSENCODE(0b10,0b000,0b0000,0b1000,0b110):
    case SYSENCODE(0b10,0b000,0b0000,0b1001,0b110):
    case SYSENCODE(0b10,0b000,0b0000,0b1010,0b110):
    case SYSENCODE(0b10,0b000,0b0000,0b1011,0b110):
    case SYSENCODE(0b10,0b000,0b0000,0b1100,0b110):
    case SYSENCODE(0b10,0b000,0b0000,0b1101,0b110):
    case SYSENCODE(0b10,0b000,0b0000,0b1110,0b110):
    case SYSENCODE(0b10,0b000,0b0000,0b1111,0b110):
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "DBGWVR" << std::dec << int(e.crm) << "_EL1"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Debug Watchpoint Value Register#" << std::dec << int(e.crm); }
          void Write(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu, U64 value) const override
          {
            if (crm >= DebugMonitor::WRPS)
              BaseSysReg::Write(op0, op1, crn, crm, op2, rt, cpu, value);

            if (cpu.pstate.GetEL() == 0)
              throw AArch64::UndefinedInstructionException();

            struct Bad {}; cpu.dbgmon.SetDBGWVR_EL1(crm, cpu.untaint(Bad(), value));
          }
          U64 Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu) const override
          {
            if (crm >= DebugMonitor::WRPS)
              return BaseSysReg::Read(op0, op1, crn, crm, op2, rt, cpu);

            if (cpu.pstate.GetEL() == 0)
              throw AArch64::UndefinedInstructionException();

            return U64(cpu.dbgmon.DBGWVR_EL1(crm));
          }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1010,0b0100,0b000): // MPAMHCR_EL2 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "MPAMHCR_EL2"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "MPAMHCR_EL2"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1010,0b0100,0b001): // MPAMVPMV_EL2 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "MPAMVPMV_EL2"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "MPAMVPMV_EL2"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1010,0b0101,0b000): // MPAM2_EL2 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "MPAM2_EL2"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "MPAM2_EL2"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b110,0b1010,0b0101,0b000): // MPAM3_EL3 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "MPAM3_EL3"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "MPAM3_EL3"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1010,0b0110,0b000): // MPAMVPM0_EL2 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "MPAMVPM0_EL2"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "MPAMVPM0_EL2"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1010,0b0110,0b001): // MPAMVPM1_EL2 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "MPAMVPM1_EL2"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "MPAMVPM1_EL2"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1010,0b0110,0b010): // MPAMVPM2_EL2 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "MPAMVPM2_EL2"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "MPAMVPM2_EL2"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1010,0b0110,0b011): // MPAMVPM3_EL2 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "MPAMVPM3_EL2"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "MPAMVPM3_EL2"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1010,0b0110,0b100): // MPAMVPM4_EL2 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "MPAMVPM4_EL2"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "MPAMVPM4_EL2"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1010,0b0110,0b101): // MPAMVPM5_EL2 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "MPAMVPM5_EL2"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "MPAMVPM5_EL2"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1010,0b0110,0b110): // MPAMVPM6_EL2 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "MPAMVPM6_EL2"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "MPAMVPM6_EL2"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1010,0b0110,0b111): // MPAMVPM7_EL2 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "MPAMVPM7_EL2"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "MPAMVPM7_EL2"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1010,0b1000,0b000): // MECID_P0_EL2 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "MECID_P0_EL2"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "MECID_P1_EL2, MECIDR_EL2, VMECID_A_EL2,"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1010,0b1000,0b001): // MECID_A0_EL2 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "MECID_A0_EL2"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Alternate MECID for EL2 and EL2&0 translation regimes"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1010,0b1000,0b010): // MECID_P1_EL2 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "MECID_P1_EL2"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Primary MECID for EL2&0 translation regimes"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1010,0b1000,0b011): // MECID_A1_EL2 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "MECID_A1_EL2"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Alternate MECID for EL2&0 translation regimes."; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1010,0b1000,0b111): // MECIDR_EL2 (RO)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "MECIDR_EL2"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "MEC Identification Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1010,0b1001,0b000): // VMECID_P_EL2 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "VMECID_P_EL2"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Primary MECID for EL1&0 stage 2 translation regime"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1010,0b1001,0b001): // VMECID_A_EL2 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "VMECID_A_EL2"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Alternate MECID for EL1&0 stage 2 translation regime"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b110,0b1010,0b1010,0b001): // MECID_RL_A_EL3 (RW)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "MECID_RL_A_EL3"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Realm PA space Alternate MECID for EL3 stage 1 translation regime"; }
        } x; return &x;
      } break;
    }

  if (SYSENCODE(op0, op1, crn, unsigned(crm&-2), 0) == SYSENCODE(0b11,0b100,0b1100,0b1100,0b000))
    { // 6.43: ICH_LR<n>_EL2, Interrupt Controller List Registers, n = 0 - 15
      {
        static struct : public BaseSysReg {
          static int GetIdx(Encoding e) { return (e.crm << 3 & 0x8) | e.op2; }
          void Name(Encoding e, std::ostream& sink) const override { sink << "ICH_LR" << std::dec << GetIdx(e) << "_EL2"; }
          void Describe(Encoding e, std::ostream& sink) const override { sink << "Interrupt Controller List Register #" << std::dec << GetIdx(e); }
        } x; return &x;
      }
    }

  if (SYSENCODE(op0, op1, crn, crm&8, 0) == SYSENCODE(0b11,0b011,0b1110,0b1000,0b000))
    {
      if (crm & 4) // 4.9: PMEVTYPER<n>_EL0, Performance Monitors Event Type Registers, n = 0 - 30
        {
          static struct : public BaseSysReg {
            static int GetIdx(Encoding e) { return (e.crm << 3 & 0x18) | e.op2; }
            void Name(Encoding e, std::ostream& sink) const override { sink << "PMEVTYPER" << std::dec << GetIdx(e) << "_EL0"; }
            void Describe(Encoding e, std::ostream& sink) const override { sink << "Performance Monitors Event Type Register #" << std::dec << GetIdx(e); }
          } x; return &x;
        }
      else         // 4.8: PMEVCNTR<n>_EL0, Performance Monitors Event Count Registers, n = 0 - 30
        {
          static struct : public BaseSysReg {
            static int GetIdx(Encoding e) { return (e.crm << 3 & 0x18) | e.op2; }
            void Name(Encoding e, std::ostream& sink) const override { sink << "PMEVCNTR" << std::dec << GetIdx(e) << "_EL0"; }
            void Describe(Encoding e, std::ostream& sink) const override { sink << "Performance Monitors Event Count Register #" << std::dec << GetIdx(e); }
          } x; return &x;
        }
    }

  static struct UnknownSysReg : public BaseSysReg
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
    virtual void Write(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu, U64 value) const override
    {
      fields(op0, op1, crn, crm, op2, std::cerr << std::hex << cpu.current_insn_addr << ": unimplemented, cannot write " << std::dec);
      std::cerr << std::endl;
      struct Bad {};
      throw Bad(); // cpu.UnpredictableInsnBehaviour();
    }
    virtual U64 Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, AArch64& cpu) const override
    {
      fields(op0, op1, crn, crm, op2, std::cerr << std::hex << cpu.current_insn_addr << ": unimplemented, cannot read " << std::dec);
      std::cerr << std::endl;
      struct Bad {};
      throw Bad(); // cpu.UnpredictableInsnBehaviour();
      return U64();
    }
  } err;

  err.fields(op0, op1, crn, crm, op2, std::cerr << "Unknown system register: ");
  std::cerr << std::endl;
  return &err;
}

