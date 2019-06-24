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
 * Authors: Yves Lhuillier (yves.lhuillier@cea.fr)
 */

#ifndef __TESTRUN_HH__
#define __TESTRUN_HH__

#include <unisim/util/random/random.hh>
#include <iosfwd>
#include <inttypes.h>

namespace ut {
  struct Random : public unisim::util::random::Random
  {
    Random() : unisim::util::random::Random(0,0,0,0) {}
    uint32_t generate32() { return Generate(); }
    uint64_t generate64() { return (uint64_t(generate32()) << 32) | generate32(); }
    uint64_t operand()
    {
      int64_t flips = int64_t(generate32()) << 48;
      uint64_t value = 0;
      for (int idx = 0; idx < 5; ++idx)
        {
          value |= generate64();
          value ^= flips >> 63;
          flips <<= 1;
        }
      return value;
    }
  };

  struct TestbedBase
  {
    TestbedBase( char const* seed, uint64_t* elems, uintptr_t count );

    void serial( std::ostream& sink, uint64_t* elems, uintptr_t count ) const;

    Random rng;
    uintptr_t counter;
  };
  
  template <uintptr_t COUNT>
  struct Testbed : public TestbedBase
  {
    Testbed( char const* seed ) : TestbedBase( seed, &buffer[0], COUNT ) {}
    
    void serial( std::ostream& sink ) { serial( sink, &buffer[0], COUNT ); }
      
    uintptr_t head(uintptr_t idx=0) const { return (counter+idx) % COUNT; }
    
    void next()
    {
      buffer[head()] = rng.operand();
      counter += 1;
    }
    template <typename T> typename T::value_type const& select( T const& tests ) { return tests[counter%tests.size()]; }
    
    uint64_t buffer[COUNT];
    uintptr_t counter;
    
    void load( uint64_t* ws, unsigned size ) const
    {
      if (size >= COUNT) throw 0;
      unsigned rnd_idx = head();
      for (unsigned idx = 0; idx < size; ++idx)
        {
          ws[idx] = buffer[rnd_idx];
          rnd_idx = (rnd_idx + 1) % COUNT;
        }
      
    }

    //   bool check( Workspace& ref, Workspace& sim, Arch& arch, fut_t const& fut ) const
    //   {
    //     for (unsigned idx = 0; idx < (ref.wordcount-1); ++idx)
    //       {
    //         if (ref.data[idx] != sim.data[idx])
    //           {
    //             Workspace src;
    //             load( src );
    //             return error(idx, src, ref, sim, arch, fut), false;
    //           }
    //       }
    //     return true;
    //   }

    //   template <uintptr_t N>
    //   fut_t const& select( fut_t const (&tests)[N] ) const
    //   {
    //     return tests[counter%N];
    //   }
    
    // private:
    // Workspace rnd;
    // uint64_t counter;
  
    // void error( unsigned field, Workspace const& src, Workspace const& ref, Workspace const& sim, Arch& arch, fut_t const& fut ) const
    // {
    //   std::cerr << "id: ";
    //   serial( std::cerr );
    //   std::cerr << "\n";
    //   std::cerr << "op: ";
    //   uint64_t addr = 0;
    //   for (unsigned idx = 1; idx < 32; ++idx)
    //     {
    //       uint32_t* insn = &((uint32_t*)fut.test)[idx];
    //       if (*insn == fut.insn)
    //         { addr = (uint64_t)(insn); break; }
    //     }
    //   if (not addr) { std::cerr << "<mimic>"; addr = (uint64_t)&fut.insn; }
    //   arch.DumpInsnInfo( std::cerr, addr );
    //   std::cerr << std::endl;

    //   for (unsigned idx = 0; idx < src.wordcount; ++idx)
    //     {
    //       std::cerr << std::setw(2)  << std::setfill(' ') << std::dec << idx << ": ";
    //       std::cerr << std::setw(16) << std::setfill(' ') << std::hex << src.data[idx] << " | ";
    //       char const* sym = (ref.data[idx] == sim.data[idx]) ? " = " : " ~ ";
    //       std::cerr << std::setw(16) << std::setfill(' ') << std::hex << ref.data[idx] << sym;
    //       sym = (idx == field) ? " <== " : "     ";
    //       std::cerr << std::setw(16) << std::setfill(' ') << std::hex << sim.data[idx];
    //       std::cerr << " (" << FPoint(ref.data[idx]) << ")\n";
    //       if (idx != field) continue;
    //       std::cerr << "ref:    " << Hexa(ref.data[idx]) << std::endl;
    //       std::cerr << "sim:    " << Hexa(sim.data[idx]) << std::endl;
    //     }
    // }
  };
  
  
} /* end of namespace ut */


#endif // __TESTRUN_HH__
