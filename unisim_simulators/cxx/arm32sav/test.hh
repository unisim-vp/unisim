/*
 *  Copyright (c) 2019-2023,
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

#include <top_arm32.hh>
#include <top_thumb.hh>
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

  Interface( std::string const& disasm );
  Interface( unisim::component::cxx::processor::arm::isa::arm32::Operation<Scanner> const& , std::string const& );
  Interface( unisim::component::cxx::processor::arm::isa::thumb::Operation<Scanner> const& , std::string const& );
  void start();
  void memaccess( Expr const& addr, unsigned size, bool iswrite, bool isaligned );
  
  uintptr_t workcells() const;
  uint32_t memspread() const { return memrange[1] - memrange[0]; }
  bool usemem() const { return base_addr.good(); }
  unsigned gindex(unsigned reg) const { return __builtin_popcount(grmap & ((1<<reg)-1)); }
  uint32_t vrmap() const;
  struct Vitr { Vitr(uint32_t _map) : map(_map) {} uint32_t map; unsigned operator*() const; bool operator!=(Vitr const&) const; void operator++ (); };
  struct Veach { Veach(Interface const& _it) : it(_it) {} Interface const& it; Vitr begin() { return Vitr(it.vrmap()); } Vitr end() { return Vitr(0); } };
  Veach vregs_each() const { return Veach(*this); }
  
  std::string asmcode, gilname;
  unisim::util::sav::OperandMap<uint8_t,16> gregs; /* general purpose registers */
  uint32_t grmap;
  unisim::util::sav::OperandMap<uint8_t,32> vregs; /* vector registers */
  //  unisim::util::sav::OperandMap<uint8_t,16> vregs; /* vector registers */
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

struct ExpectedAddress : public unisim::util::symbolic::ExprNode
{
  ExpectedAddress() : unisim::util::symbolic::ExprNode() {}
  virtual ExpectedAddress* Mutate() const override { return new ExpectedAddress( *this ); }
  virtual int cmp(ExprNode const& rhs) const override { return 0; }
  virtual unsigned SubCount() const override { return 0; }
  virtual void Repr( std::ostream& sink ) const override { sink << "ExpectedAddress()"; }
  virtual unisim::util::symbolic::ValueType GetType() const override { return unisim::util::symbolic::CValueType(uint32_t()); }
};
      
#endif // ARM32SAV_REVIEW_HH
