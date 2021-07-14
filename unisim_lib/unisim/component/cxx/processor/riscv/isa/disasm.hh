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
 */

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_RISCV_ISA_DISASM_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_RISCV_ISA_DISASM_HH__

#include <iosfwd>
#include <inttypes.h>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace riscv {
namespace isa {

struct Ostreamable
{
  virtual void operator() ( std::ostream& sink ) const = 0;
  virtual ~Ostreamable() {};
};

std::ostream& operator << ( std::ostream& sink, Ostreamable const& dobj );

struct PrintGPR : public Ostreamable
{
  PrintGPR(unsigned _num) : num(_num) {} unsigned num;
  void operator () (std::ostream& sink ) const override;
};

struct PrintFPR : public Ostreamable
{
  PrintFPR(unsigned _num) : num(_num) {} unsigned num;
  void operator () ( std::ostream& sink ) const override;
};

struct PrintRM : public Ostreamable
{
  PrintRM(unsigned _num ) : num(_num) {} unsigned num;
  void operator () ( std::ostream& sink ) const override;
};

struct PrintAQRL : public Ostreamable
{
  PrintAQRL(unsigned _aqrl ) : aqrl(_aqrl) {} unsigned aqrl;
  void operator () ( std::ostream& sink ) const override;
};

struct PrintIORW : public Ostreamable
{
  PrintIORW(unsigned _iorw ) : iorw(_iorw) {} unsigned iorw;
  void operator () ( std::ostream& sink ) const override;
};

struct PrintHex : public Ostreamable
{
  PrintHex( uint64_t _num ) : num(_num) {} uint64_t num;
  void operator () ( std::ostream& sink ) const override;
};

struct PrintI : public Ostreamable
{
  PrintI( int32_t _num ) : num(_num) {} int32_t num;
  void operator () ( std::ostream& sink ) const override;
};

// struct PrintCond : public Ostreamable
// {
//   PrintCond( unsigned _rid ) : rid(_rid) {} unsigned rid;
//   void operator () ( std::ostream& sink ) const
//   {
//     char const* condnames[] = {"f",    "un",   "eq",   "ueq",  "olt",  "ult",  "ole",  "ule",  "sf",
//                                "ngle", "seq",  "ngl",  "lt",   "nge",  "le",   "ngt"};
//     sink << condnames[rid];
//   }
// };

} /* end of naemspace isa */
} /* end of naemspace riscv */
} /* end of naemspace processor */
} /* end of naemspace cxx */
} /* end of naemspace component */
} /* end of naemspace unisim */

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_RISCV_ISA_DISASM_HH__
