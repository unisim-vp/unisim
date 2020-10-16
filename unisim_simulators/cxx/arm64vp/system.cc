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
    virtual void Describe(Encoding, char const* prefix, std::ostream& sink) const {}
    virtual char const* ReadOperation() const { return "mrs"; }
    virtual char const* WriteOperation() const { return "msr"; }

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

    void Write(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, AArch64& cpu, U64 value) const override
    {
      std::cerr << std::hex << cpu.current_insn_addr << ": System register undefined write in `" << std::dec;
      DisasmWrite(op0, op1, crn, crm, op2, 0b11111, std::cerr);
      std::cerr << "`.\n";
      cpu.UndefinedInstruction();
    }

    U64 Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, AArch64& cpu) const override
    {
      std::cerr << std::hex << cpu.current_insn_addr << ": System register undefined read in `" << std::dec;
      DisasmRead(op0, op1, crn, crm, op2, 0b11111, std::cerr);
      std::cerr << "`.\n";
      cpu.UndefinedInstruction();
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

  struct PStateSysReg : public BaseSysReg
  {
    char const* ReadOperation() const { return "mrs<bad-read>"; }
    void DisasmWrite(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, std::ostream& sink) const override
    {
      Encoding e{op0, op1, crn, crm, op2};
      Name(e, sink << "msr\t");
      sink << ", " << unisim::component::cxx::processor::arm::isa::arm64::DisasmI(crm);
      Describe(e, "\t; ", sink);
    }
  };

  struct BarrierSysReg : public BaseSysReg
  {
    virtual void DisasmRead(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, std::ostream& sink) const override
    { sink << WriteOperation() << "<bad-read>"; }
    virtual void DisasmWrite(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, std::ostream& sink) const override
    { sink << WriteOperation() << '\t' << unisim::component::cxx::processor::arm::isa::arm64::DisasmBarrierOption(crm); }
    virtual void Write(uint8_t, uint8_t, uint8_t, uint8_t crm, uint8_t, AArch64& cpu, U64) const override
    { /* No OoO nor speculative execution, nothing to do */ }
  };


  switch (SYSENCODE( op0, op1, crn, crm, op2 ))
    {
      /***  Architectural hints instructions ***/
    case SYSENCODE( 0b00, 0b011, 0b0010, 0b0000, 0b001 ):
      {
        static struct : public BaseSysReg
        {
          char const* description() const { return "Yield"; }
          void DisasmRead(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, std::ostream& sink) const override { sink << "yield<bad-read>\t; " << description(); }
          void DisasmWrite(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, std::ostream& sink) const override { sink << "yield\t; " << description(); }
          virtual void Write(uint8_t, uint8_t, uint8_t, uint8_t crm, uint8_t, AArch64& cpu, U64) const override
          { /* Don't know what to do here */ cpu.TODO(); }
        } x; return &x;
      }

    case SYSENCODE( 0b00, 0b011, 0b0010, 0b0000, 0b010 ):
      {
        static struct : public BaseSysReg
        {
          char const* description() const { return "Wait For Event"; }
          void DisasmRead(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, std::ostream& sink) const override { sink << "wfe<bad-read>\t; " << description(); }
          void DisasmWrite(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, std::ostream& sink) const override { sink << "wfe\t; " << description(); }
        } x; return &x;
      }

    case SYSENCODE( 0b00, 0b011, 0b0010, 0b0000, 0b011 ):
      {
        static struct : public BaseSysReg
        {
          char const* description() const { return "Wait For Interrupt"; }
          void DisasmRead(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, std::ostream& sink) const override { sink << "wfi<bad-read>\t; " << description(); }
          void DisasmWrite(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, std::ostream& sink) const override { sink << "wfi\t; " << description(); }
          void Write(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, AArch64& cpu, U64) const override { cpu.notify(1,&AArch64::wfi); }
        } x; return &x;
      }

    case SYSENCODE( 0b00, 0b011, 0b0010, 0b0010, 0b100 ):
      {
        static struct : public BaseSysReg
        {
          char const* description() const { return "Consumption of Speculative Data Barrier"; }
          void DisasmRead(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, std::ostream& sink) const override { sink << "csdb (read error)\t; " << description(); }
          void DisasmWrite(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, std::ostream& sink) const override { sink << "csdb\t; " << description(); }
          void Write(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, AArch64&, U64) const override { /* No OoO nor speculative execution, nothing to do */ }
        } x; return &x;
      } break;

      /*** Instruction Cache Maintenance ***/
    case SYSENCODE( 0b01, 0b000, 0b0111, 0b0101, 0b000 ):
      {
        static struct : public ICSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "iallu"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Invalidate all instruction caches to Point of Unification"; }
          void Write(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, AArch64& cpu, U64 addr) const override
          {
            cpu.ipb.base_address = -1;
          }
        } x; return &x;
      } break;

    case SYSENCODE( 0b01, 0b000, 0b0111, 0b0001, 0b000 ):
      {
        static struct : public ICSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ialluis"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Invalidate all instruction caches in Inner Shareable domain to Point of Unification"; }
          void Write(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, AArch64& cpu, U64 addr) const override
          {
            cpu.ipb.base_address = -1;
          }
        } x; return &x;
      } break;

    case SYSENCODE( 0b01, 0b011, 0b0111, 0b0101, 0b001 ):
      {
        static struct : public ICSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ivau"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Invalidate instruction cache by address to Point of Unification"; }
          void Write(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, AArch64& cpu, U64 addr) const override
          {
            if (addr.ubits) { struct Bad {}; raise( Bad () ); }
            cpu.ipb.base_address = -1; /* systematic invalidation is cheaper than va translation ;-) */
          }
        } x; return &x;
      } break;

      /*** Data Cache Maintenance ***/
    case SYSENCODE(0b01,0b011,0b0111,0b0100,0b001):
      {
        static struct : public DCSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "zva"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Zero data cache by address"; }
          void Write(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, AArch64& cpu, U64 addr) const override
          {
            if (addr.ubits) { struct Bad {}; raise( Bad () ); }
            unsigned const zsize = 4 << cpu.ZID;
            uint64_t paddr = cpu.translate_address(addr.value, mat_write, zsize);
            uint8_t buffer[zsize] = {0};
            if (cpu.modify_page(paddr).write(paddr & -zsize, &buffer[0], &buffer[0], zsize) != zsize)
              { struct Bad {}; raise( Bad () ); }
          }
        } x; return &x;
      } break;
    case SYSENCODE(0b01,0b000,0b0111,0b1110,0b010):
      {
        static struct : public DCSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "cisw"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Clean and Invalidate data cache by set/way"; }
        } x; return &x;
      } break;
    case SYSENCODE(0b01,0b000,0b0111,0b1010,0b010):
      {
        static struct : public DCSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "csw"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Clean data cache by set/way"; }
        } x; return &x;
      } break;
    case SYSENCODE(0b01,0b011,0b0111,0b1010,0b001):
      {
        static struct : public DCSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "cvac"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Clean data cache by address to Point of Coherency"; }
        } x; return &x;
      } break;
    case SYSENCODE(0b01,0b011,0b0111,0b1011,0b001):
      {
        static struct : public DCSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "cvau"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Clean data cache by address to Point of Unification"; }
          void Write(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, AArch64& cpu, U64 addr) const override
          {
            if (addr.ubits) { struct Bad {}; raise( Bad () ); }
            // No caches, so normally nothing to do...
          }
        } x; return &x;
      } break;
    case SYSENCODE(0b01,0b000,0b0111,0b0110,0b010):
      {
        static struct : public DCSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "isw"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Invalidate data cache by set/way"; }
        } x; return &x;
      } break;
    case SYSENCODE(0b01,0b000,0b0111,0b0110,0b001):
      {
        static struct : public DCSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ivac"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Invalidate data cache by address to Point of Coherency"; }
          void Write(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, AArch64& cpu, U64 addr) const override
          {
            if (addr.ubits) { struct Bad {}; raise( Bad () ); }
            // No caches, so normally nothing to do...
          }
        } x; return &x;
      } break;

    case SYSENCODE(0b01,0b011,0b0111,0b1110,0b001):
      {
        static struct : public DCSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "civac"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Clean and Invalidate data cache by address to Point of Coherency"; }
          void Write(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, AArch64& cpu, U64 addr) const override
          {
            if (addr.ubits) { struct Bad {}; raise( Bad () ); }
            // No caches, so normally nothing to do...
          }
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
          void Describe(Encoding e, char const* p, std::ostream& sink) const override
          {
            sink << p << "TLB Invalidate ";
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
          virtual void Write(uint8_t, uint8_t, uint8_t, uint8_t crm, uint8_t op2, AArch64& cpu, U64 arg) const override
          {
            cpu.mmu.tlb.invalidate(crm&4,op2&4,op2&3,cpu,arg);
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
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Current Exception Level"; }
          U64 Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, AArch64& cpu) const override
          {
            return U64(cpu.pstate.EL << 2);
            //return U64(cpu.pstate.EL << 2, ~uint64_t(0b11 << 2));
          }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b0100,0b0010,0b001): // DAIF, Interrupt Mask Bits
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "DAIF"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Interrupt Mask Bits"; }
          U64  Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, AArch64& cpu) const override { return cpu.pstate.GetDAIF(); }
          void Write(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, AArch64& cpu, U64 value) const override { cpu.pstate.SetDAIF(cpu,value); }
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
            if (cpu.pstate.SP == 0 and lvl == 0)
              cpu.UndefinedInstruction();
            return cpu.sp_el[lvl];
          }
          void Name(Encoding e, std::ostream& sink) const override { sink << "SP_EL" << (GetExceptionLevel(e.op1)-1); }
          void Describe(Encoding e, char const* prefix, std::ostream& sink) const override { sink << prefix << "Stack Pointer (EL" << (GetExceptionLevel(e.op1)-1) << ")"; }
          U64  Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, AArch64& cpu) const override { return GetSP(cpu, op1); }
          void Write(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, AArch64& cpu, U64 value) const override { GetSP(cpu, op1) = value; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0001,0b0000,0b001): // 2.1: ACTLR_EL1, Auxiliary Control Register (EL1)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ACTLR_EL1"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Auxiliary Control Register (EL1)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b0001,0b0000,0b001): // 2.2: ACTLR_EL2, Auxiliary Control Register (EL2)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ACTLR_EL2"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Auxiliary Control Register (EL2)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b110,0b0001,0b0000,0b001): // 2.3: ACTLR_EL3, Auxiliary Control Register (EL3)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ACTLR_EL3"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Auxiliary Control Register (EL3)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0101,0b0001,0b000): // 2.4: AFSR0_EL1, Auxiliary Fault Status Register 0 (EL1)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "AFSR0_EL1"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Auxiliary Fault Status Register 0 (EL1)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b0101,0b0001,0b000): // 2.5: AFSR0_EL2, Auxiliary Fault Status Register 0 (EL2)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "AFSR0_EL2"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Auxiliary Fault Status Register 0 (EL2)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b110,0b0101,0b0001,0b000): // 2.6: AFSR0_EL3, Auxiliary Fault Status Register 0 (EL3)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "AFSR0_EL3"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Auxiliary Fault Status Register 0 (EL3)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0101,0b0001,0b001): // 2.7: AFSR1_EL1, Auxiliary Fault Status Register 1 (EL1)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "AFSR1_EL1"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Auxiliary Fault Status Register 1 (EL1)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b0101,0b0001,0b001): // 2.8: AFSR1_EL2, Auxiliary Fault Status Register 1 (EL2)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "AFSR1_EL2"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Auxiliary Fault Status Register 1 (EL2)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b110,0b0101,0b0001,0b001): // 2.9: AFSR1_EL3, Auxiliary Fault Status Register 1 (EL3)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "AFSR1_EL3"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Auxiliary Fault Status Register 1 (EL3)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b001,0b0000,0b0000,0b111): // 2.10: AIDR_EL1, Auxiliary ID Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "AIDR_EL1"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Auxiliary ID Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1010,0b0011,0b000): // 2.11: AMAIR_EL1, Auxiliary Memory Attribute Indirection Register (EL1)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "AMAIR_EL1"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Auxiliary Memory Attribute Indirection Register (EL1)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1010,0b0011,0b000): // 2.12: AMAIR_EL2, Auxiliary Memory Attribute Indirection Register (EL2)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "AMAIR_EL2"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Auxiliary Memory Attribute Indirection Register (EL2)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b110,0b1010,0b0011,0b000): // 2.13: AMAIR_EL3, Auxiliary Memory Attribute Indirection Register (EL3)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "AMAIR_EL3"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Auxiliary Memory Attribute Indirection Register (EL3)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b001,0b0000,0b0000,0b000): // 2.14: CCSIDR_EL1, Current Cache Size ID Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "CCSIDR_EL1"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Current Cache Size ID Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b001,0b0000,0b0000,0b001): // 2.15: CLIDR_EL1, Cache Level ID Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "CLIDR_EL1"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Cache Level ID Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1101,0b0000,0b001): // 2.16: CONTEXTIDR_EL1, Context ID Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "CONTEXTIDR_EL1"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Context ID Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0001,0b0000,0b010): // 2.17: CPACR_EL1, Architectural Feature Access Control Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "CPACR_EL1"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override
          { sink << prefix << "Architectural Feature Access Control Register"; }
          void Write(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, AArch64& cpu, U64 value) const override
          { if (value.ubits) { struct Bad {}; raise( Bad () ); } cpu.CPACR = value.value; }
          U64 Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, AArch64& cpu) const override
          { return U64(cpu.CPACR); }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b0001,0b0001,0b010): // 2.18: CPTR_EL2, Architectural Feature Trap Register (EL2)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "CPTR_EL2"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Architectural Feature Trap Register (EL2)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b110,0b0001,0b0001,0b010): // 2.19: CPTR_EL3, Architectural Feature Trap Register (EL3)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "CPTR_EL3"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Architectural Feature Trap Register (EL3)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b010,0b0000,0b0000,0b000): // 2.20: CSSELR_EL1, Cache Size Selection Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "CSSELR_EL1"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Cache Size Selection Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b0000,0b0000,0b001): // 2.21: CTR_EL0, Cache Type Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "CTR_EL0"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Cache Type Register"; }
          U64 Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, AArch64& cpu) const override
          {
            return
              U64(1)              << 31 | // RES1, Reserved.
              U64(0)              << 30 | // RES0, Reserved.
              U64(1)              << 29 | // DIC, Instruction cache invalidation requirements for data to instruction coherence (0 == required).
              U64(1)              << 28 | // IDC, Data cache clean requirements for instruction to data coherence (0 == required).
              U64(4)              << 24 | // CWG, Cache Writeback Granule.
              U64(4)              << 20 | // ERG, Exclusives Reservation Granule.
              U64(0b0100)         << 16 | // DminLine (16 words)
              U64(0b11)           << 14 | // L1Ip, Level 1 instruction cache policy (PIPT).
              U64(0b0000000000)   <<  4 | // RES0, Reserved.
              U64(0b0100)         <<  0;  // IminLine, (16 words)
          }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b0011,0b0000,0b000): // 2.22: DACR32_EL2, Domain Access Control Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "DACR32_EL2"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Domain Access Control Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b0000,0b0000,0b111): // 2.23: DCZID_EL0, Data Cache Zero ID register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "DCZID_EL0"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Data Cache Zero ID register"; }
          U64 Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, AArch64& cpu) const override
          {
            return U64((not unisim::component::cxx::processor::arm::vmsav8::sctlr::DZE.Get(cpu.el1.SCTLR)) << 4 | cpu.ZID);
          }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0101,0b0010,0b000): // 2.24: ESR_EL1, Exception Syndrome Register (EL1)
    case SYSENCODE(0b11,0b100,0b0101,0b0010,0b000): // 2.25: ESR_EL2, Exception Syndrome Register (EL2)
    case SYSENCODE(0b11,0b110,0b0101,0b0010,0b000): // 2.26: ESR_EL3, Exception Syndrome Register (EL3)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "ESR_EL" << GetExceptionLevel(e.op1); }
          void Describe(Encoding e, char const* prefix, std::ostream& sink) const override { sink << prefix << "Exception Syndrome Register (EL" << GetExceptionLevel(e.op1) << ")"; }
          U64  Read(uint8_t, uint8_t op1, uint8_t, uint8_t, uint8_t, AArch64& cpu) const override { return U64(cpu.get_el(GetExceptionLevel(op1)).ESR); }
          void Write(uint8_t, uint8_t op1, uint8_t, uint8_t, uint8_t, AArch64& cpu, U64 value) const override { cpu.get_el(GetExceptionLevel(op1)).ESR = U32(value); }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0110,0b0000,0b000): // 2.27: FAR_EL1, Fault Address Register (EL1)
    case SYSENCODE(0b11,0b100,0b0110,0b0000,0b000): // 2.28: FAR_EL2, Fault Address Register (EL2)
    case SYSENCODE(0b11,0b110,0b0110,0b0000,0b000): // 2.29: FAR_EL3, Fault Address Register (EL3)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "FAR_EL" << GetExceptionLevel(e.op1); }
          void Describe(Encoding e, char const* prefix, std::ostream& sink) const override { sink << prefix << "Exception Syndrome Register (EL" << GetExceptionLevel(e.op1) << ")"; }
          U64  Read(uint8_t, uint8_t op1, uint8_t, uint8_t, uint8_t, AArch64& cpu) const override { return cpu.get_el(GetExceptionLevel(op1)).FAR; }
          void Write(uint8_t, uint8_t op1, uint8_t, uint8_t, uint8_t, AArch64& cpu, U64 value) const override { cpu.get_el(GetExceptionLevel(op1)).FAR = value; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b0101,0b0011,0b000): // 2.30: FPEXC32_EL2, Floating-point Exception Control register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "FPEXC32_EL2"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Floating-point Exception Control register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b0001,0b0001,0b111): // 2.31: HACR_EL2, Hypervisor Auxiliary Control Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "HACR_EL2"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Hypervisor Auxiliary Control Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b0001,0b0001,0b000): // 2.32: HCR_EL2, Hypervisor Configuration Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "HCR_EL2"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Hypervisor Configuration Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b0110,0b0000,0b100): // 2.33: HPFAR_EL2, Hypervisor IPA Fault Address Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "HPFAR_EL2"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Hypervisor IPA Fault Address Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b0001,0b0001,0b011): // 2.34: HSTR_EL2, Hypervisor System Trap Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "HSTR_EL2"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Hypervisor System Trap Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0101,0b100): // 2.35: ID_AA64AFR0_EL1, AArch64 Auxiliary Feature Register 0
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ID_AA64AFR0_EL1"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "AArch64 Auxiliary Feature Register 0"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0101,0b101): // 2.36: ID_AA64AFR1_EL1, AArch64 Auxiliary Feature Register 1
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ID_AA64AFR1_EL1"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "AArch64 Auxiliary Feature Register 1"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0101,0b000): // 2.37: ID_AA64DFR0_EL1, AArch64 Debug Feature Register 0
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ID_AA64DFR0_EL1"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "AArch64 Debug Feature Register 0"; }
          U64 Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, AArch64& cpu) const override { cpu.TODO(); return U64(0); }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0101,0b001): // 2.38: ID_AA64DFR1_EL1, AArch64 Debug Feature Register 1
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ID_AA64DFR1_EL1"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "AArch64 Debug Feature Register 1"; }
          U64 Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, AArch64& cpu) const override { cpu.TODO(); return U64(0); }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0110,0b000): // 2.39: ID_AA64ISAR0_EL1, AArch64 Instruction Set Attribute Register 0
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ID_AA64ISAR0_EL1"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "AArch64 Instruction Set Attribute Register 0"; }
          U64 Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, AArch64& cpu) const override
          {
            return
              U64(0b0001)         << 60 | // Indicates support for Random Number instructions
              U64(0b0010)         << 56 | // Indicates support for Outer shareable and TLB range maintenance instructions
              U64(0b0010)         << 52 | // Indicates support for flag manipulation instructions
              U64(0b0000)         << 48 | // Indicates support for FMLAL and FMLSL instructions
              U64(0b0000)         << 44 | // Indicates support for Dot Product instructions
              U64(0b0000)         << 40 | // Indicates support for SM4 instructions
              U64(0b0000)         << 36 | // Indicates support for SM3 instructions
              U64(0b0000)         << 32 | // Indicates support for SHA3 instructions
              U64(0b0001)         << 28 | // Indicates support for SQRDMLAH and SQRDMLSH instructions
              U64(0,0b1111)       << 24 | // Reserved, RES0.
              U64(0b0010)         << 20 | // Indicates support for Atomic instructions
              U64(0b0001)         << 16 | // Indicates support for CRC32 instructions
              U64(0b0000)         << 12 | // Indicates support for SHA2 instructions (TODO)
              U64(0b0000)         <<  8 | // Indicates support for SHA1 instructions (TODO)
              U64(0b0000)         <<  4 | // Indicates support for AES instructions (TODO)
              U64(0,0b1111)       <<  0;  // Reserved, RES0.
          }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0110,0b001): // 2.40: ID_AA64ISAR1_EL1, AArch64 Instruction Set Attribute Register 1
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ID_AA64ISAR1_EL1"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "AArch64 Instruction Set Attribute Register 1"; }
          U64 Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, AArch64& cpu) const override { return U64(0); }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0111,0b000): // 2.41: ID_AA64MMFR0_EL1, AArch64 Memory Model Feature Register 0
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ID_AA64MMFR0_EL1"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "AArch64 Memory Model Feature Register 0"; }
          U64 Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, AArch64& cpu) const override
          {
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
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ID_AA64MMFR1_EL1"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "AArch64 Memory Model Feature Register 1"; }
          U64 Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, AArch64& cpu) const override
          {
            /* RES0, Reserved for future expansion of the information
             * about the implemented memory model and memory
             * management support in AArch64. */
            return U64(0);
          }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0111,0b010): // 2.42: ID_AA64MMFR2_EL1, AArch64 Memory Model Feature Register 2
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ID_AA64MMFR2_EL1"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "AArch64 Memory Model Feature Register 2"; }
          U64 Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, AArch64& cpu) const override
          {
            /* RES0, Reserved for future expansion of the information
             * about the implemented memory model and memory
             * management support in AArch64. */
            return U64(0);
          }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0100,0b000): // 2.43: ID_AA64PFR0_EL1, AArch64 Processor Feature Register 0
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ID_AA64PFR0_EL1"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "AArch64 Processor Feature Register 0"; }
          U64 Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, AArch64& cpu) const override
          {
            return
              U64(0b0000)           << 60 | // Speculative use of faulting data
              U64(0b0000)           << 56 | // Speculative use of out of context branch targets
              U64(0,0b1111)         << 52 | // RES0
              U64(0b0000)           << 48 | // Data Independent Timing
              U64(0b0000)           << 44 | // Indicates support for Activity Monitors Extension
              U64(0b0000)           << 40 | // Indicates support for MPAM Extension
              U64(0b0000)           << 36 | // Secure EL2
              U64(0b0000)           << 32 | // Scalable Vector Extension
              U64(0b0000)           << 28 | // RAS Extension version
              U64(0b0000)           << 24 | // System register GIC CPU interface
              U64(0b0000)           << 20 | // Advanced SIMD (implemented)
              U64(0b0000)           << 16 | // Floating-point (implemented)
              U64(0b0000)           << 12 | // EL3 Exception level handling (not implemented)
              U64(0b0001)           <<  8 | // EL2 Exception level handling (executed in AArch64 state only)
              U64(0b0001)           <<  4 | // EL1 Exception level handling (executed in AArch64 state only) 
              U64(0b0001)           <<  0;  // EL0 Exception level handling (executed in AArch64 state only) 
          }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0100,0b001): // 2.44: ID_AA64PFR1_EL1, AArch64 Processor Feature Register 1
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ID_AA64PFR1_EL1"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "AArch64 Processor Feature Register 1"; }
          U64 Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, AArch64& cpu) const override
          {
            return
              U64(0,-1)             << 20 | // Reserved, RES0
              U64(0b0000)           << 16 | // MPAM Extension fractional field
              U64(0b0000)           << 12 | // RAS Extension fractional field
              U64(0b0000)           <<  8 | // Support for the Memory Tagging Extension
              U64(0b0000)           <<  4 | // Speculative Store Bypassing controls in AArch64 state
              U64(0b0000)           <<  0;  // Branch Target Identification mechanism support
          }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0100,0b100): // 2.44: ID_AA64ZFR0_EL1, 
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ID_AA64ZFR0_EL1"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "AArch64 SVE Processor Feature Register"; }
          U64 Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, AArch64& cpu) const override { return U64(0); }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0001,0b011): // 2.45: ID_AFR0_EL1, AArch32 Auxiliary Feature Register 0
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ID_AFR0_EL1"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "AArch32 Auxiliary Feature Register 0"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0001,0b010): // 2.46: ID_DFR0_EL1, AArch32 Debug Feature Register 0
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ID_DFR0_EL1"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "AArch32 Debug Feature Register 0"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0010,0b000): // 2.47: ID_ISAR0_EL1, AArch32 Instruction Set Attribute Register 0
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ID_ISAR0_EL1"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "AArch32 Instruction Set Attribute Register 0"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0010,0b001): // 2.48: ID_ISAR1_EL1, AArch32 Instruction Set Attribute Register 1
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ID_ISAR1_EL1"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "AArch32 Instruction Set Attribute Register 1"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0010,0b010): // 2.49: ID_ISAR2_EL1, AArch32 Instruction Set Attribute Register 2
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ID_ISAR2_EL1"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "AArch32 Instruction Set Attribute Register 2"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0010,0b011): // 2.50: ID_ISAR3_EL1, AArch32 Instruction Set Attribute Register 3
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ID_ISAR3_EL1"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "AArch32 Instruction Set Attribute Register 3"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0010,0b100): // 2.51: ID_ISAR4_EL1, AArch32 Instruction Set Attribute Register 4
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ID_ISAR4_EL1"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "AArch32 Instruction Set Attribute Register 4"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0010,0b101): // 2.52: ID_ISAR5_EL1, AArch32 Instruction Set Attribute Register 5
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ID_ISAR5_EL1"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "AArch32 Instruction Set Attribute Register 5"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0001,0b100): // 2.53: ID_MMFR0_EL1, AArch32 Memory Model Feature Register 0
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ID_MMFR0_EL1"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "AArch32 Memory Model Feature Register 0"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0001,0b101): // 2.54: ID_MMFR1_EL1, AArch32 Memory Model Feature Register 1
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ID_MMFR1_EL1"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "AArch32 Memory Model Feature Register 1"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0001,0b110): // 2.55: ID_MMFR2_EL1, AArch32 Memory Model Feature Register 2
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ID_MMFR2_EL1"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "AArch32 Memory Model Feature Register 2"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0001,0b111): // 2.56: ID_MMFR3_EL1, AArch32 Memory Model Feature Register 3
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ID_MMFR3_EL1"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "AArch32 Memory Model Feature Register 3"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0001,0b000): // 2.57: ID_PFR0_EL1, AArch32 Processor Feature Register 0
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ID_PFR0_EL1"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "AArch32 Processor Feature Register 0"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0001,0b001): // 2.58: ID_PFR1_EL1, AArch32 Processor Feature Register 1
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ID_PFR1_EL1"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "AArch32 Processor Feature Register 1"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b0101,0b0000,0b001): // 2.59: IFSR32_EL2, Instruction Fault Status Register (EL2)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "IFSR32_EL2"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Instruction Fault Status Register (EL2)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b0001,0b000): // 2.60: ISR_EL1, Interrupt Status Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ISR_EL1"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Interrupt Status Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1010,0b0010,0b000): // 2.61: MAIR_EL1, Memory Attribute Indirection Register (EL1)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "MAIR_EL1"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Memory Attribute Indirection Register (EL1)"; }
          void Write(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, AArch64& cpu, U64 value) const override
          { if (value.ubits) { struct Bad {}; raise( Bad () ); } cpu.mmu.MAIR_EL1 = value.value; }
          U64 Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, AArch64& cpu) const override
          { return U64(cpu.mmu.MAIR_EL1); }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1010,0b0010,0b000): // 2.62: MAIR_EL2, Memory Attribute Indirection Register (EL2)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "MAIR_EL2"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Memory Attribute Indirection Register (EL2)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b110,0b1010,0b0010,0b000): // 2.63: MAIR_EL3, Memory Attribute Indirection Register (EL3)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "MAIR_EL3"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Memory Attribute Indirection Register (EL3)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0000,0b000): // 2.64: MIDR_EL1, Main ID Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "MIDR_EL1"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Main ID Register"; }
          U64 Read(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, AArch64& cpu) const override
          {
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
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "MPIDR_EL1"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Multiprocessor Affinity Register"; }
          U64 Read(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, AArch64& cpu) const override
          {
            return U64(0)
              | U64(0, 0xffffff)  << 40  // Reserved, RES0.
              | U64(0)            << 32  // Aff3, Affinity level 3.
              | U64(1, 1)         << 31  // Reserved, RES1.
              | U64(1)            << 30  // U, bit [30] Uniprocessor system.
              | U64(0, 0x1f)      << 25  // Bits [29:25] Reserved, RES0.
              | U64(0)            << 24  // MT, bit [24] Lowest level of affinity consists of multi-threaded logical processors.
              | U64(0)            << 16  // Aff2, Affinity level 2.
              | U64(0)            <<  8  // Aff1, Affinity level 1.
              | U64(0)            <<  0  // Aff0, Affinity level 0.
              ;
          }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0011,0b000): // 2.66: MVFR0_EL1, Media and VFP Feature Register 0
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "MVFR0_EL1"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Media and VFP Feature Register 0"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0011,0b001): // 2.67: MVFR1_EL1, Media and VFP Feature Register 1
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "MVFR1_EL1"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Media and VFP Feature Register 1"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0011,0b010): // 2.68: MVFR2_EL1, Media and VFP Feature Register 2
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "MVFR2_EL1"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Media and VFP Feature Register 2"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0111,0b0100,0b000): // 2.69: PAR_EL1, Physical Address Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "PAR_EL1"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Physical Address Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0000,0b110): // 2.70: REVIDR_EL1, Revision ID Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "REVIDR_EL1"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Revision ID Register"; }
          U64  Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, AArch64& cpu) const override { return U64(0); }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b0000,0b010): // 2.71: RMR_EL1, Reset Management Register (if EL2 and EL3 not implemented)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "RMR_EL1"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Reset Management Register (if EL2 and EL3 not implemented)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1100,0b0000,0b010): // 2.72: RMR_EL2, Reset Management Register (if EL3 not implemented)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "RMR_EL2"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Reset Management Register (if EL3 not implemented)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b110,0b1100,0b0000,0b010): // 2.73: RMR_EL3, Reset Management Register (if EL3 implemented)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "RMR_EL3"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Reset Management Register (if EL3 implemented)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b0000,0b001): // 2.74: RVBAR_EL1, Reset Vector Base Address Register (if EL2 and EL3 not implemented)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "RVBAR_EL1"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Reset Vector Base Address Register (if EL2 and EL3 not implemented)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1100,0b0000,0b001): // 2.75: RVBAR_EL2, Reset Vector Base Address Register (if EL3 not implemented)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "RVBAR_EL2"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Reset Vector Base Address Register (if EL3 not implemented)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b110,0b1100,0b0000,0b001): // 2.76: RVBAR_EL3, Reset Vector Base Address Register (if EL3 implemented)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "RVBAR_EL3"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Reset Vector Base Address Register (if EL3 implemented)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b110,0b0001,0b0001,0b000): // 2.78: SCR_EL3, Secure Configuration Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "SCR_EL3"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Secure Configuration Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0001,0b0000,0b000): // 2.79: SCTLR_EL1, System Control Register (EL1)
    case SYSENCODE(0b11,0b100,0b0001,0b0000,0b000): // 2.80: SCTLR_EL2, System Control Register (EL2)
    case SYSENCODE(0b11,0b110,0b0001,0b0000,0b000): // 2.81: SCTLR_EL3, System Control Register (EL3)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "SCTLR_EL" << GetExceptionLevel(e.op1); }
          void Describe(Encoding e, char const* prefix, std::ostream& sink) const override { sink << prefix << "System Control Register (EL" << GetExceptionLevel(e.op1) << ")"; }
          U64  Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, AArch64& cpu) const override { return U64(cpu.get_el(GetExceptionLevel(op1)).SCTLR); }
          void Write(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, AArch64& cpu, U64 value) const override
          {
            if (value.ubits) { struct Bad {}; raise( Bad() ); }
            cpu.get_el(GetExceptionLevel(op1)).SetSCTLR(cpu, value.value);
          }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0010,0b0000,0b010): // 2.82: TCR_EL1, Translation Control Register (EL1)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "TCR_EL1"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Translation Control Register (EL1)"; }
          U64 Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, AArch64& cpu) const override { return U64(cpu.mmu.TCR_EL1); }
          void Write(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, AArch64& cpu, U64 value) const override
          {
            if (value.ubits) { struct Bad {}; raise( Bad () ); }
            cpu.mmu.TCR_EL1 = value.value;
          }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b0010,0b0000,0b010): // 2.83: TCR_EL2, Translation Control Register (EL2)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "TCR_EL2"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Translation Control Register (EL2)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b110,0b0010,0b0000,0b010): // 2.84: TCR_EL3, Translation Control Register (EL3)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "TCR_EL3"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Translation Control Register (EL3)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b010,0b0000,0b0000,0b000): // 2.85: TEECR32_EL1, T32EE Configuration Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "TEECR32_EL1"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "T32EE Configuration Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b010,0b0001,0b0000,0b000): // 2.86: TEEHBR32_EL1, T32EE Handler Base Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "TEEHBR32_EL1"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "T32EE Handler Base Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1101,0b0000,0b010): // 2.87: TPIDR_EL0, Thread Pointer / ID Register (EL0)
    case SYSENCODE(0b11,0b000,0b1101,0b0000,0b100): // 2.88: TPIDR_EL1, Thread Pointer / ID Register (EL1)
    case SYSENCODE(0b11,0b100,0b1101,0b0000,0b010): // 2.89: TPIDR_EL2, Thread Pointer / ID Register (EL2)
    case SYSENCODE(0b11,0b110,0b1101,0b0000,0b010): // 2.90: TPIDR_EL3, Thread Pointer / ID Register (EL3)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "TPIDR_EL" << GetExceptionLevel(e.op1); }
          void Describe(Encoding e, char const* prefix, std::ostream& sink) const override { sink << prefix << "Thread Pointer / ID Register (EL" << GetExceptionLevel(e.op1) << ")"; }
          U64 Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, AArch64& cpu) const override { return cpu.TPIDR[GetExceptionLevel(op1)]; }
          void Write(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, AArch64& cpu, U64 value) const override { cpu.TPIDR[GetExceptionLevel(op1)] = value; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1101,0b0000,0b011): // 2.91: TPIDRRO_EL0, Thread Pointer / ID Register, Read-Only (EL0)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "TPIDRRO_EL0"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Thread Pointer / ID Register, Read-Only (EL0)"; }
          U64 Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, AArch64& cpu) const override { return cpu.TPIDRRO; }
          void Write(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, AArch64& cpu, U64 value) const override { cpu.TPIDRRO = value; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0010,0b0000,0b000): // 2.92: TTBR0_EL1, Translation Table Base Register 0 (EL1)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "TTBR0_EL1"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Translation Table Base Register 0 (EL1)"; }
          U64 Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, AArch64& cpu) const override { return U64(cpu.mmu.TTBR0_EL1); }
          void Write(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, AArch64& cpu, U64 value) const override
          {
            if (value.ubits) { struct Bad {}; raise( Bad () ); }
            cpu.mmu.TTBR0_EL1 = value.value;
          }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b0010,0b0000,0b000): // 2.93: TTBR0_EL2, Translation Table Base Register 0 (EL2)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "TTBR0_EL2"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Translation Table Base Register 0 (EL2)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b110,0b0010,0b0000,0b000): // 2.94: TTBR0_EL3, Translation Table Base Register 0 (EL3)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "TTBR0_EL3"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Translation Table Base Register 0 (EL3)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0010,0b0000,0b001): // 2.95: TTBR1_EL1, Translation Table Base Register 1
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "TTBR1_EL1"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Translation Table Base Register 1"; }
          U64 Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, AArch64& cpu) const override { return U64(cpu.mmu.TTBR1_EL1); }
          void Write(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, AArch64& cpu, U64 value) const override
          {
            if (value.ubits) { struct Bad {}; raise( Bad () ); }
            cpu.mmu.TTBR1_EL1 = value.value;
          }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0100,0b0000,0b000): // SPSR_EL1
    case SYSENCODE(0b11,0b100,0b0100,0b0000,0b000): // SPSR_EL2
    case SYSENCODE(0b11,0b110,0b0100,0b0000,0b000): // SPSR_EL3
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "SPSR_EL" << GetExceptionLevel(e.op1); }
          void Describe(Encoding e, char const* prefix, std::ostream& sink) const override { sink << prefix << "Vector Base Address Register (EL" << GetExceptionLevel(e.op1) << ")"; }
          U64  Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, AArch64& cpu) const override { return U64(cpu.get_el(GetExceptionLevel(op1)).SPSR); }
          void Write(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, AArch64& cpu, U64 value) const override { cpu.get_el(GetExceptionLevel(op1)).SPSR = U32(value); }
        } x; return &x;
      } break;
      
    case SYSENCODE(0b11,0b000,0b0100,0b0000,0b001): // ELR_EL1
    case SYSENCODE(0b11,0b100,0b0100,0b0000,0b001): // ELR_EL2
    case SYSENCODE(0b11,0b110,0b0100,0b0000,0b001): // ELR_EL3
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "ELR_EL" << GetExceptionLevel(e.op1); }
          void Describe(Encoding e, char const* prefix, std::ostream& sink) const override { sink << prefix << "Vector Base Address Register (EL" << GetExceptionLevel(e.op1) << ")"; }
          U64  Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, AArch64& cpu) const override { return cpu.get_el(GetExceptionLevel(op1)).ELR; }
          void Write(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, AArch64& cpu, U64 value) const override { cpu.get_el(GetExceptionLevel(op1)).ELR = value; }
        } x; return &x;
      } break;
    
    case SYSENCODE(0b11,0b000,0b1100,0b0000,0b000): // 2.96: VBAR_EL1, Vector Base Address Register (EL1)
    case SYSENCODE(0b11,0b100,0b1100,0b0000,0b000): // 2.97: VBAR_EL2, Vector Base Address Register (EL2)
    case SYSENCODE(0b11,0b110,0b1100,0b0000,0b000): // 2.98: VBAR_EL3, Vector Base Address Register (EL3)
      {
        static struct : public BaseSysReg {
          void Name(Encoding e, std::ostream& sink) const override { sink << "VBAR_EL" << GetExceptionLevel(e.op1); }
          void Describe(Encoding e, char const* prefix, std::ostream& sink) const override { sink << prefix << "Vector Base Address Register (EL" << GetExceptionLevel(e.op1) << ")"; }
          U64  Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, AArch64& cpu) const override { return cpu.get_el(GetExceptionLevel(op1)).VBAR; }
          void Write(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, AArch64& cpu, U64 value) const override { cpu.get_el(GetExceptionLevel(op1)).VBAR = value; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b0000,0b0000,0b101): // 2.99: VMPIDR_EL2, Virtualization Multiprocessor ID Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "VMPIDR_EL2"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Virtualization Multiprocessor ID Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b0000,0b0000,0b000): // 2.100: VPIDR_EL2, Virtualization Processor ID Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "VPIDR_EL2"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Virtualization Processor ID Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b0010,0b0001,0b010): // 2.101: VTCR_EL2, Virtualization Translation Control Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "VTCR_EL2"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Virtualization Translation Control Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b0010,0b0001,0b000): // 2.102: VTTBR_EL2, Virtualization Translation Table Base Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "VTTBR_EL2"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Virtualization Translation Table Base Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b000,0b0111,0b1110,0b110): // 3.1: DBGAUTHSTATUS_EL1, Debug Authentication Status register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "DBGAUTHSTATUS_EL1"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Debug Authentication Status register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b000,0b0000,0b0000,0b101): // 3.2: DBGBCR<n>_EL1, Debug Breakpoint Control Registers, n = 0 - 15
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "DBGBCR<n>_EL1"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Debug Breakpoint Control Registers, n = 0 - 15"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b000,0b0000,0b0000,0b100): // 3.3: DBGBVR<n>_EL1, Debug Breakpoint Value Registers, n = 0 - 15
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "DBGBVR<n>_EL1"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Debug Breakpoint Value Registers, n = 0 - 15"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b000,0b0111,0b1001,0b110): // 3.4: DBGCLAIMCLR_EL1, Debug Claim Tag Clear register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "DBGCLAIMCLR_EL1"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Debug Claim Tag Clear register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b000,0b0111,0b1000,0b110): // 3.5: DBGCLAIMSET_EL1, Debug Claim Tag Set register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "DBGCLAIMSET_EL1"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Debug Claim Tag Set register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b011,0b0000,0b0100,0b000): // 3.6: DBGDTR_EL0, Debug Data Transfer Register, half-duplex
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "DBGDTR_EL0"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Debug Data Transfer Register, half-duplex"; }
        } x; return &x;
      } break;

    // case SYSENCODE(0b10,0b011,0b0000,0b0101,0b000): 3.7: DBGDTRRX_EL0, Debug Data Transfer Register, Receive
    //   {
    //     static struct : public BaseSysReg {
    //       void Name(Encoding, std::ostream& sink) const override { sink << "DBGDTRRX_EL0"; }
    //       void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Debug Data Transfer Register, Receive"; }
    //     } x; return &x;
    //   } break;

    // case SYSENCODE(0b10,0b011,0b0000,0b0101,0b000): 3.8: DBGDTRTX_EL0, Debug Data Transfer Register, Transmit
    //   {
    //     static struct : public BaseSysReg {
    //       void Name(Encoding, std::ostream& sink) const override { sink << "DBGDTRTX_EL0"; }
    //       void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Debug Data Transfer Register, Transmit"; }
    //     } x; return &x;
    //   } break;

    case SYSENCODE(0b10,0b011,0b0000,0b0101,0b000): // 3.8: DBGDTR_EL0, Debug Data Transfer Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "DBGDTR_EL0"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Debug Data Transfer Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b000,0b0001,0b0100,0b100): // 3.9: DBGPRCR_EL1, Debug Power Control Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "DBGPRCR_EL1"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Debug Power Control Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b100,0b0000,0b0111,0b000): // 3.10: DBGVCR32_EL2, Debug Vector Catch Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "DBGVCR32_EL2"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Debug Vector Catch Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b000,0b0000,0b0000,0b111): // 3.11: DBGWCR<n>_EL1, Debug Watchpoint Control Registers, n = 0 - 15
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "DBGWCR<n>_EL1"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Debug Watchpoint Control Registers, n = 0 - 15"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b000,0b0000,0b0000,0b110): // 3.12: DBGWVR<n>_EL1, Debug Watchpoint Value Registers, n = 0 - 15
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "DBGWVR<n>_EL1"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Debug Watchpoint Value Registers, n = 0 - 15"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b0100,0b0101,0b001): // 3.13: DLR_EL0, Debug Link Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "DLR_EL0"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Debug Link Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b0100,0b0101,0b000): // 3.14: DSPSR_EL0, Debug Saved Program Status Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "DSPSR_EL0"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Debug Saved Program Status Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b000,0b0000,0b0010,0b000): // 3.15: MDCCINT_EL1, Monitor DCC Interrupt Enable Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "MDCCINT_EL1"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Monitor DCC Interrupt Enable Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b011,0b0000,0b0001,0b000): // 3.16: MDCCSR_EL0, Monitor DCC Status Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "MDCCSR_EL0"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Monitor DCC Status Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b0001,0b0001,0b001): // 3.17: MDCR_EL2, Monitor Debug Configuration Register (EL2)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "MDCR_EL2"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Monitor Debug Configuration Register (EL2)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b110,0b0001,0b0011,0b001): // 3.18: MDCR_EL3, Monitor Debug Configuration Register (EL3)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "MDCR_EL3"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Monitor Debug Configuration Register (EL3)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b000,0b0001,0b0000,0b000): // 3.19: MDRAR_EL1, Monitor Debug ROM Address Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "MDRAR_EL1"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Monitor Debug ROM Address Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b000,0b0000,0b0010,0b010): // 3.20: MDSCR_EL1, Monitor Debug System Control Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "MDSCR_EL1"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Monitor Debug System Control Register"; }
          void Write(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, AArch64& cpu, U64 value) const override
          { if (value.ubits) { struct Bad {}; raise( Bad () ); } cpu.TODO(); }
          U64 Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, AArch64& cpu) const override
          { return U64(); }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b000,0b0001,0b0011,0b100): // 3.21: OSDLR_EL1, OS Double Lock Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "OSDLR_EL1"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "OS Double Lock Register"; }
          void Write(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, AArch64& cpu, U64 value) const override { if (value.ubits | value.value) { struct Bad {}; raise( Bad () ); } }
          U64 Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, AArch64& cpu) const override { return U64(0); }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b000,0b0000,0b0000,0b010): // 3.22: OSDTRRX_EL1, OS Lock Data Transfer Register, Receive
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "OSDTRRX_EL1"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "OS Lock Data Transfer Register, Receive"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b000,0b0000,0b0011,0b010): // 3.23: OSDTRTX_EL1, OS Lock Data Transfer Register, Transmit
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "OSDTRTX_EL1"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "OS Lock Data Transfer Register, Transmit"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b000,0b0000,0b0110,0b010): // 3.24: OSECCR_EL1, OS Lock Exception Catch Control Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "OSECCR_EL1"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "OS Lock Exception Catch Control Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b000,0b0001,0b0000,0b100): // 3.25: OSLAR_EL1, OS Lock Access Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "OSLAR_EL1"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "OS Lock Access Register"; }
          void Write(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, AArch64& cpu, U64 value) const override { if (value.ubits | value.value) { struct Bad {}; raise( Bad () ); } }
          U64 Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, AArch64& cpu) const override { return U64(0); }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b000,0b0001,0b0001,0b100): // 3.26: OSLSR_EL1, OS Lock Status Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "OSLSR_EL1"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "OS Lock Status Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b110,0b0001,0b0001,0b001): // 3.27: SDER32_EL3, AArch32 Secure Debug Enable Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "SDER32_EL3"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "AArch32 Secure Debug Enable Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1110,0b1111,0b111): // 4.1: PMCCFILTR_EL0, Performance Monitors Cycle Count Filter Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "PMCCFILTR_EL0"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Performance Monitors Cycle Count Filter Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1001,0b1101,0b000): // 4.2: PMCCNTR_EL0, Performance Monitors Cycle Count Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "PMCCNTR_EL0"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Performance Monitors Cycle Count Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1001,0b1100,0b110): // 4.3: PMCEID0_EL0, Performance Monitors Common Event Identification register 0
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "PMCEID0_EL0"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Performance Monitors Common Event Identification register 0"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1001,0b1100,0b111): // 4.4: PMCEID1_EL0, Performance Monitors Common Event Identification register 1
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "PMCEID1_EL0"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Performance Monitors Common Event Identification register 1"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1001,0b1100,0b010): // 4.5: PMCNTENCLR_EL0, Performance Monitors Count Enable Clear register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "PMCNTENCLR_EL0"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Performance Monitors Count Enable Clear register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1001,0b1100,0b001): // 4.6: PMCNTENSET_EL0, Performance Monitors Count Enable Set register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "PMCNTENSET_EL0"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Performance Monitors Count Enable Set register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1001,0b1100,0b000): // 4.7: PMCR_EL0, Performance Monitors Control Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "PMCR_EL0"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Performance Monitors Control Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1110,0b1000,0b000): // 4.8: PMEVCNTR<n>_EL0, Performance Monitors Event Count Registers, n = 0 - 30
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "PMEVCNTR<n>_EL0"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Performance Monitors Event Count Registers, n = 0 - 30"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1110,0b1100,0b000): // 4.9: PMEVTYPER<n>_EL0, Performance Monitors Event Type Registers, n = 0 - 30
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "PMEVTYPER<n>_EL0"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Performance Monitors Event Type Registers, n = 0 - 30"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1001,0b1110,0b010): // 4.10: PMINTENCLR_EL1, Performance Monitors Interrupt Enable Clear register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "PMINTENCLR_EL1"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Performance Monitors Interrupt Enable Clear register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1001,0b1110,0b001): // 4.11: PMINTENSET_EL1, Performance Monitors Interrupt Enable Set register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "PMINTENSET_EL1"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Performance Monitors Interrupt Enable Set register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1001,0b1100,0b011): // 4.12: PMOVSCLR_EL0, Performance Monitors Overflow Flag Status Clear Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "PMOVSCLR_EL0"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Performance Monitors Overflow Flag Status Clear Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1001,0b1110,0b011): // 4.13: PMOVSSET_EL0, Performance Monitors Overflow Flag Status Set register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "PMOVSSET_EL0"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Performance Monitors Overflow Flag Status Set register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1001,0b1100,0b101): // 4.14: PMSELR_EL0, Performance Monitors Event Counter Selection Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "PMSELR_EL0"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Performance Monitors Event Counter Selection Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1001,0b1100,0b100): // 4.15: PMSWINC_EL0, Performance Monitors Software Increment register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "PMSWINC_EL0"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Performance Monitors Software Increment register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1001,0b1110,0b000): // 4.16: PMUSERENR_EL0, Performance Monitors User Enable Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "PMUSERENR_EL0"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Performance Monitors User Enable Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1001,0b1101,0b010): // 4.17: PMXEVCNTR_EL0, Performance Monitors Selected Event Count Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "PMXEVCNTR_EL0"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Performance Monitors Selected Event Count Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1001,0b1101,0b001): // 4.18: PMXEVTYPER_EL0, Performance Monitors Selected Event Type Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "PMXEVTYPER_EL0"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Performance Monitors Selected Event Type Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1110,0b0000,0b000): // 5.1: CNTFRQ_EL0, Counter-timer Frequency register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "CNTFRQ_EL0"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Counter-timer Frequency register"; }
          U64 Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2,  AArch64& cpu) const override { return U64(cpu.vt.get_cntfrq()); }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1110,0b0001,0b000): // 5.2: CNTHCTL_EL2, Counter-timer Hypervisor Control register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "CNTHCTL_EL2"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Counter-timer Hypervisor Control register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1110,0b0010,0b001): // 5.3: CNTHP_CTL_EL2, Counter-timer Hypervisor Physical Timer Control register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "CNTHP_CTL_EL2"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Counter-timer Hypervisor Physical Timer Control register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1110,0b0010,0b010): // 5.4: CNTHP_CVAL_EL2, Counter-timer Hypervisor Physical Timer CompareValue register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "CNTHP_CVAL_EL2"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Counter-timer Hypervisor Physical Timer CompareValue register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1110,0b0010,0b000): // 5.5: CNTHP_TVAL_EL2, Counter-timer Hypervisor Physical Timer TimerValue register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "CNTHP_TVAL_EL2"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Counter-timer Hypervisor Physical Timer TimerValue register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1110,0b0001,0b000): // 5.6: CNTKCTL_EL1, Counter-timer Kernel Control register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "CNTKCTL_EL1"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Counter-timer Kernel Control register"; }
          U64 Read(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, AArch64& cpu) const override { return U64(cpu.vt.kctl); }
          void Write(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, AArch64& cpu, U64 value) const override
          { if (value.ubits) { struct Bad {}; raise( Bad () ); } cpu.vt.write_kctl( cpu, value.value ); }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1110,0b0010,0b001): // 5.7: CNTP_CTL_EL0, Counter-timer Physical Timer Control register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "CNTP_CTL_EL0"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Counter-timer Physical Timer Control register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1110,0b0010,0b010): // 5.8: CNTP_CVAL_EL0, Counter-timer Physical Timer CompareValue register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "CNTP_CVAL_EL0"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Counter-timer Physical Timer CompareValue register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1110,0b0010,0b000): // 5.9: CNTP_TVAL_EL0, Counter-timer Physical Timer TimerValue register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "CNTP_TVAL_EL0"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Counter-timer Physical Timer TimerValue register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1110,0b0000,0b001): // 5.10: CNTPCT_EL0, Counter-timer Physical Count register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "CNTPCT_EL0"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Counter-timer Physical Count register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b111,0b1110,0b0010,0b001): // 5.11: CNTPS_CTL_EL1, Counter-timer Physical Secure Timer Control register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "CNTPS_CTL_EL1"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Counter-timer Physical Secure Timer Control register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b111,0b1110,0b0010,0b010): // 5.12: CNTPS_CVAL_EL1, Counter-timer Physical Secure Timer CompareValue register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "CNTPS_CVAL_EL1"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Counter-timer Physical Secure Timer CompareValue register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b111,0b1110,0b0010,0b000): // 5.13: CNTPS_TVAL_EL1, Counter-timer Physical Secure Timer TimerValue register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "CNTPS_TVAL_EL1"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Counter-timer Physical Secure Timer TimerValue register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1110,0b0011,0b001): // 5.14: CNTV_CTL_EL0, Counter-timer Virtual Timer Control register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "CNTV_CTL_EL0"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Counter-timer Virtual Timer Control register"; }
          U64 Read(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, AArch64& cpu) const override { return U64(cpu.vt.read_ctl(cpu)); }
          void Write(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, AArch64& cpu, U64 value) const override
          { if (value.ubits) { struct Bad {}; raise( Bad () ); } cpu.vt.write_ctl(cpu, value.value); }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1110,0b0011,0b010): // 5.15: CNTV_CVAL_EL0, Counter-timer Virtual Timer CompareValue register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "CNTV_CVAL_EL0"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Counter-timer Virtual Timer CompareValue register"; }
          U64 Read(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, AArch64& cpu) const override { return U64(cpu.vt.cval); }
          void Write(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, AArch64& cpu, U64 value) const override
          { if (value.ubits) { struct Bad {}; raise( Bad () ); } cpu.vt.write_cval(cpu, value.value); }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1110,0b0011,0b000): // 5.16: CNTV_TVAL_EL0, Counter-timer Virtual Timer TimerValue register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "CNTV_TVAL_EL0"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Counter-timer Virtual Timer TimerValue register"; }
          U64 Read(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, AArch64& cpu) const override { return U64(uint32_t(cpu.vt.read_tval(cpu))); }
          void Write(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, AArch64& cpu, U64 value) const override
          { if (value.ubits) { struct Bad {}; raise( Bad () ); } cpu.vt.write_tval(cpu, value.value); }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1110,0b0000,0b010): // 5.17: CNTVCT_EL0, Counter-timer Virtual Count register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "CNTVCT_EL0"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Counter-timer Virtual Count register"; }
          U64 Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, AArch64& cpu) const override
          {
            /* The virtual count value is equal to the physical count
             * value minus the virtual offset visible in CNTVOFF_EL2.
             * If EL2 not implemented, the offset is fixed as 0.
             */
            return U64(cpu.vt.get_pcount(cpu));
          }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1110,0b0000,0b011): // 5.18: CNTVOFF_EL2, Counter-timer Virtual Offset register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "CNTVOFF_EL2"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Counter-timer Virtual Offset register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b1000,0b100): // 6.1: ICC_AP0R0_EL1, Interrupt Controller Active Priorities Register (0,0)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ICC_AP0R0_EL1"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Interrupt Controller Active Priorities Register (0,0)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b1000,0b101): // 6.2: ICC_AP0R1_EL1, Interrupt Controller Active Priorities Register (0,1)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ICC_AP0R1_EL1"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Interrupt Controller Active Priorities Register (0,1)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b1000,0b110): // 6.3: ICC_AP0R2_EL1, Interrupt Controller Active Priorities Register (0,2)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ICC_AP0R2_EL1"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Interrupt Controller Active Priorities Register (0,2)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b1000,0b111): // 6.4: ICC_AP0R3_EL1, Interrupt Controller Active Priorities Register (0,3)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ICC_AP0R3_EL1"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Interrupt Controller Active Priorities Register (0,3)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b1001,0b000): // 6.5: ICC_AP1R0_EL1, Interrupt Controller Active Priorities Register (1,0)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ICC_AP1R0_EL1"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Interrupt Controller Active Priorities Register (1,0)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b1001,0b001): // 6.6: ICC_AP1R1_EL1, Interrupt Controller Active Priorities Register (1,1)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ICC_AP1R1_EL1"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Interrupt Controller Active Priorities Register (1,1)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b1001,0b010): // 6.7: ICC_AP1R2_EL1, Interrupt Controller Active Priorities Register (1,2)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ICC_AP1R2_EL1"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Interrupt Controller Active Priorities Register (1,2)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b1001,0b011): // 6.8: ICC_AP1R3_EL1, Interrupt Controller Active Priorities Register (1,3)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ICC_AP1R3_EL1"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Interrupt Controller Active Priorities Register (1,3)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b1011,0b110): // 6.9: ICC_ASGI1R_EL1, Interrupt Controller Alias Software Generated Interrupt group 1 Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ICC_ASGI1R_EL1"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Interrupt Controller Alias Software Generated Interrupt group 1 Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b1000,0b011): // 6.10: ICC_BPR0_EL1, Interrupt Controller Binary Point Register 0
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ICC_BPR0_EL1"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Interrupt Controller Binary Point Register 0"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b1100,0b011): // 6.11: ICC_BPR1_EL1, Interrupt Controller Binary Point Register 1
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ICC_BPR1_EL1"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Interrupt Controller Binary Point Register 1"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b1100,0b100): // 6.12: ICC_CTLR_EL1, Interrupt Controller Control Register (EL1)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ICC_CTLR_EL1"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Interrupt Controller Control Register (EL1)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b110,0b1100,0b1100,0b100): // 6.13: ICC_CTLR_EL3, Interrupt Controller Control Register (EL3)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ICC_CTLR_EL3"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Interrupt Controller Control Register (EL3)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b1011,0b001): // 6.14: ICC_DIR_EL1, Interrupt Controller Deactivate Interrupt Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ICC_DIR_EL1"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Interrupt Controller Deactivate Interrupt Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b1000,0b001): // 6.15: ICC_EOIR0_EL1, Interrupt Controller End Of Interrupt Register 0
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ICC_EOIR0_EL1"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Interrupt Controller End Of Interrupt Register 0"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b1100,0b001): // 6.16: ICC_EOIR1_EL1, Interrupt Controller End Of Interrupt Register 1
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ICC_EOIR1_EL1"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Interrupt Controller End Of Interrupt Register 1"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b1000,0b010): // 6.17: ICC_HPPIR0_EL1, Interrupt Controller Highest Priority Pending Interrupt Register 0
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ICC_HPPIR0_EL1"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Interrupt Controller Highest Priority Pending Interrupt Register 0"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b1100,0b010): // 6.18: ICC_HPPIR1_EL1, Interrupt Controller Highest Priority Pending Interrupt Register 1
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ICC_HPPIR1_EL1"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Interrupt Controller Highest Priority Pending Interrupt Register 1"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b1000,0b000): // 6.19: ICC_IAR0_EL1, Interrupt Controller Interrupt Acknowledge Register 0
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ICC_IAR0_EL1"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Interrupt Controller Interrupt Acknowledge Register 0"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b1100,0b000): // 6.20: ICC_IAR1_EL1, Interrupt Controller Interrupt Acknowledge Register 1
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ICC_IAR1_EL1"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Interrupt Controller Interrupt Acknowledge Register 1"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b1100,0b110): // 6.21: ICC_IGRPEN0_EL1, Interrupt Controller Interrupt Group 0 Enable register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ICC_IGRPEN0_EL1"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Interrupt Controller Interrupt Group 0 Enable register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b1100,0b111): // 6.22: ICC_IGRPEN1_EL1, Interrupt Controller Interrupt Group 1 Enable register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ICC_IGRPEN1_EL1"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Interrupt Controller Interrupt Group 1 Enable register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b110,0b1100,0b1100,0b111): // 6.23: ICC_IGRPEN1_EL3, Interrupt Controller Interrupt Group 1 Enable register (EL3)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ICC_IGRPEN1_EL3"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Interrupt Controller Interrupt Group 1 Enable register (EL3)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0100,0b0110,0b000): // 6.24: ICC_PMR_EL1, Interrupt Controller Interrupt Priority Mask Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ICC_PMR_EL1"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Interrupt Controller Interrupt Priority Mask Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b1011,0b011): // 6.25: ICC_RPR_EL1, Interrupt Controller Running Priority Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ICC_RPR_EL1"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Interrupt Controller Running Priority Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b1101,0b000): // 6.26: ICC_SEIEN_EL1, Interrupt Controller System Error Interrupt Enable register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ICC_SEIEN_EL1"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Interrupt Controller System Error Interrupt Enable register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b1011,0b111): // 6.27: ICC_SGI0R_EL1, Interrupt Controller Software Generated Interrupt group 0 Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ICC_SGI0R_EL1"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Interrupt Controller Software Generated Interrupt group 0 Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b1011,0b101): // 6.28: ICC_SGI1R_EL1, Interrupt Controller Software Generated Interrupt group 1 Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ICC_SGI1R_EL1"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Interrupt Controller Software Generated Interrupt group 1 Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b1100,0b101): // 6.29: ICC_SRE_EL1, Interrupt Controller System Register Enable register (EL1)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ICC_SRE_EL1"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Interrupt Controller System Register Enable register (EL1)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1100,0b1001,0b101): // 6.30: ICC_SRE_EL2, Interrupt Controller System Register Enable register (EL2)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ICC_SRE_EL2"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Interrupt Controller System Register Enable register (EL2)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b110,0b1100,0b1100,0b101): // 6.31: ICC_SRE_EL3, Interrupt Controller System Register Enable register (EL3)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ICC_SRE_EL3"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Interrupt Controller System Register Enable register (EL3)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1100,0b1000,0b000): // 6.32: ICH_AP0R0_EL2, Interrupt Controller Hyp Active Priorities Register (0,0)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ICH_AP0R0_EL2"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Interrupt Controller Hyp Active Priorities Register (0,0)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1100,0b1000,0b001): // 6.33: ICH_AP0R1_EL2, Interrupt Controller Hyp Active Priorities Register (0,1)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ICH_AP0R1_EL2"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Interrupt Controller Hyp Active Priorities Register (0,1)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1100,0b1000,0b010): // 6.34: ICH_AP0R2_EL2, Interrupt Controller Hyp Active Priorities Register (0,2)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ICH_AP0R2_EL2"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Interrupt Controller Hyp Active Priorities Register (0,2)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1100,0b1000,0b011): // 6.35: ICH_AP0R3_EL2, Interrupt Controller Hyp Active Priorities Register (0,3)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ICH_AP0R3_EL2"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Interrupt Controller Hyp Active Priorities Register (0,3)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1100,0b1001,0b000): // 6.36: ICH_AP1R0_EL2, Interrupt Controller Hyp Active Priorities Register (1,0)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ICH_AP1R0_EL2"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Interrupt Controller Hyp Active Priorities Register (1,0)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1100,0b1001,0b001): // 6.37: ICH_AP1R1_EL2, Interrupt Controller Hyp Active Priorities Register (1,1)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ICH_AP1R1_EL2"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Interrupt Controller Hyp Active Priorities Register (1,1)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1100,0b1001,0b010): // 6.38: ICH_AP1R2_EL2, Interrupt Controller Hyp Active Priorities Register (1,2)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ICH_AP1R2_EL2"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Interrupt Controller Hyp Active Priorities Register (1,2)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1100,0b1001,0b011): // 6.39: ICH_AP1R3_EL2, Interrupt Controller Hyp Active Priorities Register (1,3)
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ICH_AP1R3_EL2"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Interrupt Controller Hyp Active Priorities Register (1,3)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1100,0b1011,0b011): // 6.40: ICH_EISR_EL2, Interrupt Controller End of Interrupt Status Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ICH_EISR_EL2"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Interrupt Controller End of Interrupt Status Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1100,0b1011,0b101): // 6.41: ICH_ELSR_EL2, Interrupt Controller Empty List Register Status Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ICH_ELSR_EL2"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Interrupt Controller Empty List Register Status Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1100,0b1011,0b000): // 6.42: ICH_HCR_EL2, Interrupt Controller Hyp Control Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ICH_HCR_EL2"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Interrupt Controller Hyp Control Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1100,0b1100,0b000): // 6.43: ICH_LR<n>_EL2, Interrupt Controller List Registers, n = 0 - 15
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ICH_LR<n>_EL2"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Interrupt Controller List Registers, n = 0 - 15"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1100,0b1011,0b010): // 6.44: ICH_MISR_EL2, Interrupt Controller Maintenance Interrupt State Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ICH_MISR_EL2"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Interrupt Controller Maintenance Interrupt State Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1100,0b1011,0b111): // 6.45: ICH_VMCR_EL2, Interrupt Controller Virtual Machine Control Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ICH_VMCR_EL2"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Interrupt Controller Virtual Machine Control Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1100,0b1001,0b100): // 6.46: ICH_VSEIR_EL2, Interrupt Controller Virtual System Error Interrupt Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ICH_VSEIR_EL2"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Interrupt Controller Virtual System Error Interrupt Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1100,0b1011,0b001): // 6.47: ICH_VTR_EL2, Interrupt Controller VGIC Type Register
      {
        static struct : public BaseSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ICH_VTR_EL2"; }
          void Describe(Encoding, char const* prefix, std::ostream& sink) const override { sink << prefix << "Interrupt Controller VGIC Type Register"; }
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
        static struct : public BarrierSysReg { virtual char const* WriteOperation() const override { return "dsb"; } } x;
        return &x;
      } break;

    case SYSENCODE(0b00,0b011,0b0011,0b0000,0b101):
      {
        static struct : public BarrierSysReg { virtual char const* WriteOperation() const override { return "dmb"; } } x;
        return &x;
      } break;

    case SYSENCODE(0b00,0b011,0b0011,0b0000,0b110):
      {
        static struct : public BarrierSysReg { virtual char const* WriteOperation() const override { return "isb"; } } x;
        return &x;
      } break;

    case SYSENCODE(0b00,0b000,0b0100,0b0000,0b101):
      {
        static struct : public PStateSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "SPSel"; }
          virtual void Write(uint8_t, uint8_t, uint8_t, uint8_t crm, uint8_t, AArch64& cpu, U64) const override
          {
            cpu.SetPStateSP(crm);
          }
        } x; return &x;
      } break;

    case SYSENCODE(0b00,0b011,0b0100,0b0000,0b110):
      {
        static struct : public PStateSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "DAIFSet"; }
          virtual void Write(uint8_t, uint8_t, uint8_t, uint8_t crm, uint8_t, AArch64& cpu, U64) const override
          {
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
          virtual void Write(uint8_t, uint8_t, uint8_t, uint8_t crm, uint8_t, AArch64& cpu, U64) const override
          {
            cpu.pstate.D &= ~crm >> 3;
            cpu.pstate.A &= ~crm >> 2;
            cpu.pstate.I &= ~crm >> 1;
            cpu.pstate.F &= ~crm >> 0;
            cpu.gic.program(cpu);
          }
        } x; return &x;
      } break;
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
    virtual void Write(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, AArch64& cpu, U64 value) const override
    {
      fields(op0, op1, crn, crm, op2, std::cerr << std::hex << cpu.current_insn_addr << ": unimplemented, cannot write " << std::dec);
      std::cerr << std::endl;
      throw 0; // cpu.UnpredictableInsnBehaviour();
    }
    virtual U64 Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, AArch64& cpu) const override
    {
      fields(op0, op1, crn, crm, op2, std::cerr << std::hex << cpu.current_insn_addr << ": unimplemented, cannot read " << std::dec);
      std::cerr << std::endl;
      throw 0; // cpu.UnpredictableInsnBehaviour();
      return U64();
    }
  } err;

  err.fields(op0, op1, crn, crm, op2, std::cerr << "Unknown system register: ");
  std::cerr << std::endl;
  return &err;
}

