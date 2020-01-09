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
  : pages(), failpage(0,0,0,0,0,0), hooks(), disasm(true), bblock(true), terminated(false)
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
  if (not ( Is<CODE>() |
            Is<BLOCK>()          ).check(types)) return false; // uc_cb_hookcode_t
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

void
Processor::insn_hooks(uint64_t addr, unsigned insn_length)
{
  if (this->bblock)
    {
      for (auto h : hooks[Hook::BLOCK])
        {
          if (h->bound_check(addr))
            h->cb<Hook::cb_code>()(this, addr, 0);
        }
    }
  for (auto h : this->hooks[Hook::CODE])
    {
      if (h->bound_check(addr))
        h->cb<Hook::cb_code>()(this, addr, insn_length);
    }
}

void
Processor::syscall_hooks(uint64_t insn_addr, unsigned  intno)
{
  for (auto h : hooks[Hook::INTR])
    {
      if (h->bound_check(insn_addr))
        h->cb<Hook::cb_code>()(this, insn_addr, intno);
    }
}

bool
Processor::mem_chprot(uint64_t addr, unsigned perms)
{
  auto page = pages.lower_bound(addr);
  if (page == pages.end() or page->last < addr)
    return error_at("no", addr), false;
  page->chperms( perms );
  return true;
}

bool
Processor::mem_chhook(uint64_t addr, unsigned access_type, Page::hook_t hook)
{
  auto page = pages.lower_bound(addr);
  if (page == pages.end() or page->last < addr)
    return error_at("no", addr), false;
  page->chhook( access_type, hook );
  return true;
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

void
BranchInfo::update( bool branch, bool known, uint64_t naddress )
{
  if (not branch)
    { pass = true; }
  else if (not known)
    {
      if (target == Direct)
        { throw *this; }
      target = Indirect;
    }
  else if (target == BNone)
    {
      target = Direct;
      address = naddress;
    }
  else if (target != Direct or address != naddress)
    { throw *this; }
}


void
Processor::error_mem_overlap( Page const& a, Page const& b )
{
  std::cerr << "error: inserted " << a << " overlaps " << b << "\n";
}

void
Processor::error_at( char const* issue, uint64_t addr )
{
  std::cerr << "error: " << issue << " page at 0x" << std::hex << addr << ".\n";
}

