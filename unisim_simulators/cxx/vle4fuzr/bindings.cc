/*
 *  Copyright (c) 2019,
 *  Commissariat a l'Energie Atomique (CEA)
 *  All rights reserved.
 *
 * FUZR RENAULT CEA FILE
 *
 * Authors: Yves Lhuillier (yves.lhuillier@cea.fr), Gilles Mouchard <gilles.mouchard@cea.fr>
 */

#include "emu.hh"
#include "arm.hh"
#include "vle.hh"
#include <unisim/kernel/kernel.hh>
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
#include <cstdint>

char const* emptyargv[] = {"",0};
static unisim::kernel::Simulator simulator(1, (char**)emptyargv);

extern "C"
{
  int emu_open_arm(unsigned is_thumb, void** ucengine)
  {
    static int instance = 0;
    std::ostringstream name;
    name << "cpu_arm" << instance++;
    
    *ucengine = new ArmProcessor( name.str().c_str(), is_thumb );
    return 0;
  }

  int emu_open_vle(void** ucengine)
  {
    *ucengine = new vle::concrete::Processor();
    return 0;
  }

  int emu_close(void* uc)
  {
    Processor* proc = (Processor*)uc;
    delete proc;
    return 0;
  }

  int emu_set_disasm(void* uc, int disasm)
  {
    Processor& proc = *(Processor*)uc;
    return proc.set_disasm(disasm), 0;
  }

  int emu_mem_map(void* uc, uint64_t addr, uintptr_t size, uint32_t perms)
  {
    Processor& proc = *(Processor*)uc;
    return proc.mem_map(addr, size, perms) == proc.pages.end();
  }

  int emu_mem_write(void* uc, uint64_t addr, uint8_t const* bytes, uintptr_t size)
  {
    Processor& proc = *(Processor*)uc;
    return proc.mem_write(addr,bytes,size);
  }
  
  int emu_mem_read(void* uc, uint64_t addr, uint8_t* bytes, uintptr_t size)
  {
    Processor& proc = *(Processor*)uc;
    return proc.mem_read(addr,bytes,size);
  }
  
  int emu_mem_prot(void* uc, uint64_t addr, uint32_t new_perms)
  {
    Processor& proc = *(Processor*)uc;
    return proc.mem_prot(addr, new_perms);
  }
  
  int emu_reg_write(void* uc, char const* id, uintptr_t size, int regid, uint64_t value)
  {
    Processor& proc = *(Processor*)uc;
    return proc.reg_write(id, size, regid, value);
  }

  int emu_reg_read(void* uc, char const* id, uintptr_t size, int regid, uint64_t* value)
  {
    Processor& proc = *(Processor*)uc;
    return proc.reg_read(id, size, regid, value);
  }

  int emu_hook_add(void* uc, uintptr_t* hh, int types, void* callback, uint64_t begin, uint64_t end)
  {
    Processor::Hook* hook = new Processor::Hook(types, callback, begin, end);
    
    if (not ((Processor*)uc)->add(hook))
      {
        std::cerr << "Hook typing error: " << std::hex << types << std::endl;
        delete hook;
        return 8 /*EMU_ERR_HOOK*/;
      }

    *hh = (uintptr_t)hook;
    return 0;
  }

  int emu_start(void* uc, uint64_t begin, uint64_t until, uint64_t timeout, uintptr_t count)
  {
    Processor& proc = *(Processor*)uc;
    return proc.emu_start(begin, until, timeout, count);
  }

}
