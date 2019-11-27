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
#include <iostream>
#include <set>
#include <vector>
#include <cassert>
#include <cstdint>

Processor::Processor()
  : pages(), hooks(), disasm(true)
{}

Processor::~Processor()
{
  for (int idx = 0; idx < int(Hook::TYPE_COUNT); ++idx)
    {
      for (auto h : hooks[idx])
        {
          h->release(idx);
        }
      hooks[idx].clear();
    }
}
  
bool
Processor::Hook::check_types()
{
  if (not ( Is<INTR>()           ).check(types)) return false; // uc_cb_hookintr_t
  //      if (not ( Is<INSN>()           ).check(types)) return false; // unimplemented
  if (not ( Is<CODE>() |
            Is<BLOCK>()          ).check(types)) return false; // uc_cb_hookcode_t
  if (not ( Is<MEM_READ_UNMAPPED>() |
            Is<MEM_WRITE_UNMAPPED>() |
            Is<MEM_FETCH_UNMAPPED>() |
            Is<MEM_READ_PROT>() |
            Is<MEM_WRITE_PROT>() |
            Is<MEM_FETCH_PROT>() ).check(types)) return false; // uc_cb_eventmem_t
  if (not ( Is<MEM_READ>() |
            Is<MEM_WRITE>() |
            Is<MEM_FETCH>() |
            Is<MEM_READ_AFTER>() ).check(types)) return false; // uc_cb_hookmem_t
  if (not ( Is<INSN_INVALID>()   ).check(types)) return false; // uc_cb_hookinsn_invalid_t
  return true;
}
    
bool
Processor::add( Processor::Hook* hook )
{
  if (not hook->check_types())
    return false;
  for (int idx = 0; idx < int(Hook::TYPE_COUNT); ++idx)
    {
      if (hook->has_type(Hook::type_t(idx)))
        {
          hooks[idx].push_back(hook);
        }
    }
  return true;
}

void Processor::Abort::dump(std::ostream& sink)
{
  sink << "Abort";
}

void
Processor::mem_overlap_error( Page const& a, Page const& b )
{
  std::cerr << "error: " << a << " and " << b << " overlap.\n";
}

void
Processor::Page::dump(std::ostream& sink) const
{
  std::cerr << "Page[0x" << std::hex << base << " .. 0x" << hi() << "]("
            << ("r "[not (perms & Read)])
            << ("w "[not (perms & Write)])
            << ("x "[not (perms & Execute)])
            << ")";
}
