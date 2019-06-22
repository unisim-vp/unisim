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

#include <inttypes.h>

namespace ut {
  typedef void (*instruction_test_t)(uint64_t*);

  
} /* end of namespace ut */


#endif // __TESTRUN_HH__


// struct Random
// {
//   uint64_t operand()
//   {
//     int64_t flips = int64_t(generate32()) << 48;
//     uint64_t value = 0;
//     for (int idx = 0; idx < 5; ++idx)
//       {
//         value |= generate();
//         value ^= flips >> 63;
//         flips <<= 1;
//       }
    
//     return value;
//   }

// protected:
//   int32_t state[5];

//   uint32_t generate32()
//   {
//     state[0] = (69069 * state[0]) + 1;
//     state[1] = state[1] ^ (state[1] << 13);
//     state[1] = state[1] ^ (state[1] >> 17);
//     state[1] = state[1] ^ (state[1] << 5);
//     int32_t k = (state[2] >> 2) + (state[3] >> 3) + (state[4] >> 2);
//     int32_t m = state[3] + state[3] + state[2] + state[4];
//     state[2] = state[3];
//     state[3] = m;
//     state[4] = k >> 30;
//     return uint32_t( state[0] + state[1] + state[3] );
//   }

//   uint64_t generate() { return (uint64_t(generate32()) << 32) | generate32(); }
  
// };

// struct Workspace
// {    
//   static unsigned const wordcount = 11;
//   uint64_t data[wordcount];

//   friend std::ostream& operator << (std::ostream& sink, Workspace const& ws)
//   {
//     char const* sep = "";
//     sink << std::hex;
//     for (unsigned idx = 0; idx < ws.wordcount; ++idx)
//       { sink << sep << ws.data[idx]; sep = ", "; }
//     return sink << std::dec;
//   }
// };

// struct Hexa
// {
//   Hexa( uint64_t _value ) : value(_value) {} uint64_t value;
//   friend std::ostream& operator << (std::ostream& sink, Hexa const& x)
//   {
//     char const* sep = "";
//     for (int pos = 16; --pos >= 0; )
//       {
//         sink << (pos & 1 ? sep : "" ) << "0123456789abcdef"[(x.value >> (pos*4)) & 0xf];
//         sep = " ";
//       }
//     return sink;
//   }
// };

// struct FPoint
// {
//   union Interpret { double as_f; uint64_t as_u; };
  
//   FPoint( uint64_t _value ) : value(_value) {} uint64_t value;
//   friend std::ostream& operator << (std::ostream& sink, FPoint const& x)
//   {
//     Interpret trans;
//     trans.as_u = x.value;
//     sink << trans.as_f;
//     return sink;
//   }
// };

// template <typename T>
// void HexStore( std::ostream& sink, T value )
// {
//   unsigned const digits = 2*sizeof (T);
//   char buf[1 + digits];
//   buf[digits] = '\0';
//   for (int idx = digits; --idx >= 0;)
//     {
//       buf[idx] = "0123456789abcdef"[value & 0xf];
//       value >>= 4;
//     }
//   sink << buf;
// }

// template <typename T>
// char const* HexLoad( T& value, char const*& buf )
// {
//   unsigned const digits = 2*sizeof (T);
//   T scratch = 0;
//   char const* ptr = buf;
//   for (char const* end = &buf[digits]; ptr < end; ++ptr)
//     {
//       char ch = *ptr;
//       if      ('0' <= ch and ch <= '9') scratch = (scratch << 4) | T( ch - '0' +  0 );
//       else if ('a' <= ch and ch <= 'f') scratch = (scratch << 4) | T( ch - 'a' + 10 );
//       else if ('A' <= ch and ch <= 'F') scratch = (scratch << 4) | T( ch - 'A' + 10 );
//       else break;
//     }
//   value = scratch;
//   buf = ptr;
// }

// struct Testbed : public Random
// {
//   Testbed( char const* seed )
//   {
//     for (unsigned idx = 0; idx < 5; ++idx)
//       HexLoad( state[idx], seed );
//     for (unsigned idx = 0; idx < rnd.wordcount; ++idx)
//       HexLoad( rnd.data[idx], seed );
//     HexLoad( counter, seed );
//   }

//   void serial( std::ostream& sink ) const
//   {
//     for (unsigned idx = 0; idx < 5; ++idx)
//       HexStore(sink,state[idx]);
//     for (unsigned idx = 0; idx < rnd.wordcount; ++idx)
//       HexStore(sink,rnd.data[idx]);
//     HexStore(sink, counter);
//   }

//   void next()
//   {
//     rnd.data[head()] = Random::operand();
//     counter += 1;
//   }
    
//   void load( Workspace& ws ) const
//   {
//     unsigned rnd_idx = head();
//     for (unsigned idx = 0; idx < ws.wordcount; ++idx)
//       {
//         ws.data[idx] = rnd.data[rnd_idx];
//         rnd_idx = (rnd_idx + 1) % rnd.wordcount;
//       }
//   }

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
    
//   //private:
//   Workspace rnd;
//   uint64_t counter;
  
//   uintptr_t head() const { return counter % rnd.wordcount; }
  
//   void error( unsigned field, Workspace const& src, Workspace const& ref, Workspace const& sim, Arch& arch, fut_t const& fut ) const
//   {
//     std::cerr << "id: ";
//     serial( std::cerr );
//     std::cerr << "\n";
//     std::cerr << "op: ";
//     uint64_t addr = 0;
//     for (unsigned idx = 1; idx < 32; ++idx)
//       {
//         uint32_t* insn = &((uint32_t*)fut.test)[idx];
//         if (*insn == fut.insn)
//           { addr = (uint64_t)(insn); break; }
//       }
//     if (not addr) { std::cerr << "<mimic>"; addr = (uint64_t)&fut.insn; }
//     arch.DumpInsnInfo( std::cerr, addr );
//     std::cerr << std::endl;

//     for (unsigned idx = 0; idx < src.wordcount; ++idx)
//       {
//         std::cerr << std::setw(2)  << std::setfill(' ') << std::dec << idx << ": ";
//         std::cerr << std::setw(16) << std::setfill(' ') << std::hex << src.data[idx] << " | ";
//         char const* sym = (ref.data[idx] == sim.data[idx]) ? " = " : " ~ ";
//         std::cerr << std::setw(16) << std::setfill(' ') << std::hex << ref.data[idx] << sym;
//         sym = (idx == field) ? " <== " : "     ";
//         std::cerr << std::setw(16) << std::setfill(' ') << std::hex << sim.data[idx];
//         std::cerr << " (" << FPoint(ref.data[idx]) << ")\n";
//         if (idx != field) continue;
//         std::cerr << "ref:    " << Hexa(ref.data[idx]) << std::endl;
//         std::cerr << "sim:    " << Hexa(sim.data[idx]) << std::endl;
//       }
//   }
// };

// int testbench( char const* seed )
// {
//   static Workspace sim;
//   Workspace ref;
//   Arch arch;
//   int max_error_count = 512;
  
//   /* Loop forever */
//   for (Testbed testbed( seed );; testbed.next())
//     {
//       fut_t const& fut = testbed.select( optests );
//       if ((testbed.counter % 0x100000) == 0)
//         { std::cerr << "checkpoint: "; testbed.serial(std::cerr); std::cerr << std::endl; }
//       // if (((unsigned*)fut.test)[0] & 0x2f0)
//       //   continue;
      
//       testbed.load( sim );
//       testrun( &sim.data[0], fut.test );
//       ref = sim;
      
//       testbed.load( sim );
//       arch.testrun( &sim.data[0], fut.test );

//       if (not testbed.check( ref, sim, arch, fut ))
//         if (--max_error_count <= 0)
//           return 1;
//     }
  
//   return 0;
// }
