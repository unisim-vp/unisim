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

#ifndef ARM32SAV_REVIEW_HH
#define ARM32SAV_REVIEW_HH

#include <unisim/util/sav/sav.hh>
#include <unisim/util/symbolic/symbolic.hh>
#include <string>
#include <memory>
#include <inttypes.h>

struct Scanner;

struct Interface
{
  typedef unisim::util::symbolic::Expr Expr;
  typedef void (*testcode_t)(uint32_t*);

  struct Insn { virtual void init( Interface& iif ) const = 0; virtual void exec(Scanner&) const = 0; virtual ~Insn() {} };
  Interface( Insn const&, std::string const& disasm );
  void memaccess( Expr const& addr, unsigned size, bool iswrite, bool isaligned );
  
  uintptr_t workcells() const;
  uint32_t memspread() const { return memrange[1] - memrange[0]; }
  uint32_t grmap() const;
  bool usemem() const { return base_addr.good(); }
  
  std::string asmcode, gilname;
  unisim::util::sav::OperandMap<uint8_t,32> gregs; /* general purpose registers */
  unisim::util::sav::OperandMap<uint8_t,32> vregs; /* vector registers */
  std::shared_ptr<unisim::util::sav::ActionNode> behavior;
  uint32_t memrange[2];
  Expr base_addr;
  unisim::util::sav::Addressings addressings;
  uint32_t insncode;
  uint32_t insnhalf    : 1;
  uint32_t itsensitive : 1;
  uint32_t aligned     : 1;
};



struct TestLess { bool operator () ( Interface const& a, Interface const& b ) const; };
  
typedef std::multiset<Interface, TestLess> TestDB;

#endif // ARM32SAV_REVIEW_HH
