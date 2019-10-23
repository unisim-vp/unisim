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
 * Authors: Yves Lhuillier (yves.lhuillier@cea.fr), Gilles Mouchard <gilles.mouchard@cea.fr>
 */

#include "bindings.hh"
#include "core.hh"

#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <sstream>
#include <cstdlib>
#include <cstdio>
#include <cstdarg>
#include <cassert>

struct Processor
  : public Core
{

  Processor( char const* name, unisim::kernel::Object* parent, bool is_thumb )
    : unisim::kernel::Object(name, parent)
    , Core( name, parent, is_thumb )
    , pages()
    , hooks()
  {
  }
    
  struct Page
  {
    std::vector<uint8_t> storage;
    uint32_t base;
    unsigned perms;

    uint32_t hi() const { return base + storage.size(); }

    bool operator < (Page const& p) const { return hi() <= p.base; }
    bool operator > (Page const& p) const { return p.hi() <= base; }

    struct Above
    {
      using is_transparent = void;
      bool operator() (Page const& a, Page const& b) const { return a > b; }
      bool operator() (Page const& a, unsigned b) const { return b < a.base; }
    };

    Page( uint32_t addr, uint32_t size, unsigned _perms )
      : storage( size ), base(addr), perms(_perms)
    {}
    uint32_t len() const { return storage.size(); }
    uint32_t write(uint32_t addr, uint8_t const* bytes, uint32_t size) const
    {
      if (addr < base) throw 0;
      addr -= base;
      if (addr > len()) throw 0;
      uint32_t written = std::min(len()-addr,size);
      std::copy( &bytes[0], &bytes[written], at(addr) );
      return written;
    }
    uint8_t* at(uint32_t x) const { return const_cast<uint8_t*>(&storage[x]); }
  };

  std::set<Page, Page::Above> pages;

  int mem_map(uint32_t addr, uint32_t size, unsigned perms)
  {
    Page page(addr, size, perms);
    auto below = pages.lower_bound(page);
    if (below != pages.end() and not (*below < page))
      {
        return -1;
      }
    if (pages.size())
      {
        auto above = std::prev(below);
        if (above != pages.end() and not (page < *above))
          {
            return -1;
          }
      }
    pages.insert(below,std::move(page));
    return 0;
  }

  int
  mem_write(uint32_t addr, uint8_t const* bytes, uintptr_t size)
  {
    for (auto pi = pages.lower_bound(addr); pi != pages.end(); --pi)
      {
        uintptr_t written = pi->write(addr, bytes, size);
        if (written >= size)
          return 0;
        addr += written;
        bytes += written;
        size -= written;
      }
    
    throw 0;
    return 0;
  }

  struct RegView
  {
    virtual ~RegView() {}
    virtual void write( Processor& proc, int id, uint8_t const* bytes ) const = 0;
    virtual void read( Processor& proc, int id, uint8_t* bytes ) const = 0;
  };

  RegView const*
  get_reg(int regid)
  {
    switch (regid)
      {
      case 0: /* UC_ARM_REG_INVALID */
        break;
      case 1: /* UC_ARM_REG_APSR */
        static struct : public RegView
        {
          static uint32_t apsr_mask() { return 0xf80f0000; }
          virtual void write( Processor& proc, int id, uint8_t const* bytes ) const { proc.SetCPSR(*(uint32_t*)bytes, apsr_mask()); }
          virtual void read( Processor& proc, int id, uint8_t* bytes ) const { *(uint32_t*)bytes = proc.GetCPSR() & apsr_mask(); }
        } _;
        return &_;
        break;
      case 2: /* UC_ARM_REG_APSR_NZCV */
        break;
      case 3: /* UC_ARM_REG_CPSR */
        break;
      case 4: /* UC_ARM_REG_FPEXC */
        break;
      case 5: /* UC_ARM_REG_FPINST */
        break;
      case 6: /* UC_ARM_REG_FPSCR */
        break;
      case 7: /* UC_ARM_REG_FPSCR_NZCV */
        break;
      case 8: /* UC_ARM_REG_FPSID */
        break;
      case 9: /* UC_ARM_REG_ITSTATE */
        break;
      case 10: /* UC_ARM_REG_LR, UC_ARM_REG_R14 */
        break;
      case 11: /* UC_ARM_REG_PC, UC_ARM_REG_R15 */
        break;
      case 12: /* UC_ARM_REG_SP, UC_ARM_REG_R13 */
        break;
      case 13: /* UC_ARM_REG_SPSR */
        break;
      case 14: /* UC_ARM_REG_D0 */
        break;
      case 15: /* UC_ARM_REG_D1 */
        break;
      case 16: /* UC_ARM_REG_D2 */
        break;
      case 17: /* UC_ARM_REG_D3 */
        break;
      case 18: /* UC_ARM_REG_D4 */
        break;
      case 19: /* UC_ARM_REG_D5 */
        break;
      case 20: /* UC_ARM_REG_D6 */
        break;
      case 21: /* UC_ARM_REG_D7 */
        break;
      case 22: /* UC_ARM_REG_D8 */
        break;
      case 23: /* UC_ARM_REG_D9 */
        break;
      case 24: /* UC_ARM_REG_D10 */
        break;
      case 25: /* UC_ARM_REG_D11 */
        break;
      case 26: /* UC_ARM_REG_D12 */
        break;
      case 27: /* UC_ARM_REG_D13 */
        break;
      case 28: /* UC_ARM_REG_D14 */
        break;
      case 29: /* UC_ARM_REG_D15 */
        break;
      case 30: /* UC_ARM_REG_D16 */
        break;
      case 31: /* UC_ARM_REG_D17 */
        break;
      case 32: /* UC_ARM_REG_D18 */
        break;
      case 33: /* UC_ARM_REG_D19 */
        break;
      case 34: /* UC_ARM_REG_D20 */
        break;
      case 35: /* UC_ARM_REG_D21 */
        break;
      case 36: /* UC_ARM_REG_D22 */
        break;
      case 37: /* UC_ARM_REG_D23 */
        break;
      case 38: /* UC_ARM_REG_D24 */
        break;
      case 39: /* UC_ARM_REG_D25 */
        break;
      case 40: /* UC_ARM_REG_D26 */
        break;
      case 41: /* UC_ARM_REG_D27 */
        break;
      case 42: /* UC_ARM_REG_D28 */
        break;
      case 43: /* UC_ARM_REG_D29 */
        break;
      case 44: /* UC_ARM_REG_D30 */
        break;
      case 45: /* UC_ARM_REG_D31 */
        break;
      case 46: /* UC_ARM_REG_FPINST2 */
        break;
      case 47: /* UC_ARM_REG_MVFR0 */
        break;
      case 48: /* UC_ARM_REG_MVFR1 */
        break;
      case 49: /* UC_ARM_REG_MVFR2 */
        break;
      case 50: /* UC_ARM_REG_Q0 */
        break;
      case 51: /* UC_ARM_REG_Q1 */
        break;
      case 52: /* UC_ARM_REG_Q2 */
        break;
      case 53: /* UC_ARM_REG_Q3 */
        break;
      case 54: /* UC_ARM_REG_Q4 */
        break;
      case 55: /* UC_ARM_REG_Q5 */
        break;
      case 56: /* UC_ARM_REG_Q6 */
        break;
      case 57: /* UC_ARM_REG_Q7 */
        break;
      case 58: /* UC_ARM_REG_Q8 */
        break;
      case 59: /* UC_ARM_REG_Q9 */
        break;
      case 60: /* UC_ARM_REG_Q10 */
        break;
      case 61: /* UC_ARM_REG_Q11 */
        break;
      case 62: /* UC_ARM_REG_Q12 */
        break;
      case 63: /* UC_ARM_REG_Q13 */
        break;
      case 64: /* UC_ARM_REG_Q14 */
        break;
      case 65: /* UC_ARM_REG_Q15 */
        break;
      case 66: case 67: case 68: case 69: case 70: case 71: case 72: case 73: case 74: case 75: case 76: case 77: case 78:
        /* UC_ARM_REG_R0,  UC_ARM_REG_R1,  UC_ARM_REG_R2,  UC_ARM_REG_R3,  UC_ARM_REG_R4,  UC_ARM_REG_R5, UC_ARM_REG_R6, UC_ARM_REG_R7,
         * UC_ARM_REG_R8,  UC_ARM_REG_R9,  UC_ARM_REG_R10, UC_ARM_REG_R11, UC_ARM_REG_R12,
         * UC_ARM_REG_SB=R9, UC_ARM_REG_SL=R10, UC_ARM_REG_FP=R11 */
        {
          static struct : public RegView
          {
            void write( Processor& proc, int id, uint8_t const* bytes ) const { proc.SetGPR(id - 66, *(uint32_t*)bytes); }
            void read( Processor& proc, int id, uint8_t* bytes ) const { *(uint32_t*)bytes = proc.GetGPR(id - 66); }
          } _;
          return &_;
        } break;
      case 79: /* UC_ARM_REG_S0 */
        break;
      case 80: /* UC_ARM_REG_S1 */
        break;
      case 81: /* UC_ARM_REG_S2 */
        break;
      case 82: /* UC_ARM_REG_S3 */
        break;
      case 83: /* UC_ARM_REG_S4 */
        break;
      case 84: /* UC_ARM_REG_S5 */
        break;
      case 85: /* UC_ARM_REG_S6 */
        break;
      case 86: /* UC_ARM_REG_S7 */
        break;
      case 87: /* UC_ARM_REG_S8 */
        break;
      case 88: /* UC_ARM_REG_S9 */
        break;
      case 89: /* UC_ARM_REG_S10 */
        break;
      case 90: /* UC_ARM_REG_S11 */
        break;
      case 91: /* UC_ARM_REG_S12 */
        break;
      case 92: /* UC_ARM_REG_S13 */
        break;
      case 93: /* UC_ARM_REG_S14 */
        break;
      case 94: /* UC_ARM_REG_S15 */
        break;
      case 95: /* UC_ARM_REG_S16 */
        break;
      case 96: /* UC_ARM_REG_S17 */
        break;
      case 97: /* UC_ARM_REG_S18 */
        break;
      case 98: /* UC_ARM_REG_S19 */
        break;
      case 99: /* UC_ARM_REG_S20 */
        break;
      case 100: /* UC_ARM_REG_S21 */
        break;
      case 101: /* UC_ARM_REG_S22 */
        break;
      case 102: /* UC_ARM_REG_S23 */
        break;
      case 103: /* UC_ARM_REG_S24 */
        break;
      case 104: /* UC_ARM_REG_S25 */
        break;
      case 105: /* UC_ARM_REG_S26 */
        break;
      case 106: /* UC_ARM_REG_S27 */
        break;
      case 107: /* UC_ARM_REG_S28 */
        break;
      case 108: /* UC_ARM_REG_S29 */
        break;
      case 109: /* UC_ARM_REG_S30 */
        break;
      case 110: /* UC_ARM_REG_S31 */
        break;
      case 111: /* UC_ARM_REG_C1_C0_2 */
        break;
      case 112: /* UC_ARM_REG_C13_C0_2 */
        break;
      case 113: /* UC_ARM_REG_C13_C0_3 */
        break;
      case 114: /* UC_ARM_REG_ENDING */
        break;
      }
    std::cerr << "Register: " << regid << " not found.\n";
    return 0;
  }
  
  int
  reg_write(int regid, uint8_t const* bytes)
  {
    RegView const* rv = get_reg(regid);
    if (not rv) return -1;
    rv->write(*this, regid, bytes);
    return 0;
  }
  int
  reg_read(int regid, uint8_t* bytes)
  {
    RegView const* rv = get_reg(regid);
    if (not rv) return -1;
    rv->read(*this, regid, bytes);
    return 0;
  }
  
  void Sync() override {};
  
  struct Hook
  {
    Hook(int tp, void* cb, void* ud, uint64_t _begin, uint64_t _end)
      : refs(0), begin(_begin), end(_end), types(tp), callback(cb), user_data(ud), insn()
    {}

    enum type_t
      {
       INTR = 0, // Hook all interrupt/syscall events
       INSN = 1, // Hook a particular instruction - only a very small subset of instructions supported here
       CODE = 2, // Hook a range of code
       BLOCK = 3, // Hook basic blocks
       MEM_READ_UNMAPPED = 4, // Hook for memory read on unmapped memory
       MEM_WRITE_UNMAPPED = 5, // Hook for invalid memory write events
       MEM_FETCH_UNMAPPED = 6, // Hook for invalid memory fetch for execution events
       MEM_READ_PROT = 7, // Hook for memory read on read-protected memory
       MEM_WRITE_PROT = 8, // Hook for memory write on write-protected memory
       MEM_FETCH_PROT = 9, // Hook for memory fetch on non-executable memory
       MEM_READ = 10, // Hook memory read events.
       MEM_WRITE = 11, // Hook memory write events.
       MEM_FETCH = 12, // Hook memory fetch for execution events
       MEM_READ_AFTER = 13, // Hook memory after successful read events.
       TYPE_COUNT
      };

    bool has_type( type_t type ) { return (types >> int(type)) & 1; }
    void Retain() { refs++; }
    void Release() { if (refs-- <= 1) delete this; }
    bool has_refs() const { return refs; }

  private:
    uintptr_t refs;
  public:
    uint64_t begin, end;
    int types;
    void* callback;
    void* user_data;
    int insn;
  };


  void add( Hook* hook )
  {
    for (int idx = 0; idx < int(Hook::TYPE_COUNT); ++idx)
      {
        if (hook->has_type(Hook::type_t(idx)))
          {
            hook->Retain();
            hooks[idx].push_back(hook);
          }
      }
  }
  
  std::vector<Hook*> hooks[Hook::TYPE_COUNT];

  int emu_start( uint64_t begin, uint64_t until, uint64_t timeout, uintptr_t count )
  {
    throw 0;
    return 0;
  }
  
};


char const* emptyargv[] = {"",0};
static unisim::kernel::Simulator simulator(1, (char**)emptyargv);

extern "C"
{
  int uc_open(unsigned arm_arch, unsigned is_thumb, void** ucengine)
  {
    assert( arm_arch == 0 );

    *ucengine = new Processor( "cpu", 0, is_thumb );

    return 0;
  }


  int uc_mem_map(void* uc, uint64_t addr, uintptr_t size, uint32_t perms)
  {
    Processor& proc = *(Processor*)uc;
    return proc.mem_map(addr, size, perms);
  }

  int uc_mem_write(void* uc, uint64_t addr, void const* bytes, uintptr_t size)
  {
    Processor& proc = *(Processor*)uc;
    return proc.mem_write(addr,(uint8_t const*)bytes,size);
  }
  
  int uc_reg_write(void* uc, int regid, void const* bytes)
  {
    Processor& proc = *(Processor*)uc;
    return proc.reg_write(regid, (uint8_t const*)bytes);
  }

  int uc_reg_read(void* uc, int regid, void* bytes)
  {
    Processor& proc = *(Processor*)uc;
    return proc.reg_read(regid, (uint8_t*)bytes);
  }

  int uc_hook_add(void* uc, uintptr_t* hh, int type, void* callback, void* user_data, uint64_t begin, uint64_t end, ...)
  {
    Processor::Hook* hook = new Processor::Hook(type, callback, user_data, begin, end);
    
    // UC_HOOK_INSN has an extra argument for instruction ID
    if (hook->has_type(Processor::Hook::INSN))
      {
        va_list valist;
        va_start(valist, end);
        hook->insn = va_arg(valist, int);
        va_end(valist);
      }

    ((Processor*)uc)->add(hook);
    if (not hook->has_refs())
      {
        /* failure */
        delete hook;
        return 8 /*UC_ERR_HOOK*/;
      }

    *hh = (uintptr_t)hook;
    return 0;
  }

  int uc_emu_start(void* uc, uint64_t begin, uint64_t until, uint64_t timeout, uintptr_t count)
  {
    Processor& proc = *(Processor*)uc;
    return proc.emu_start(begin, until, timeout, count);
  }

}
