/*
 *  Copyright (c) 2019-2020,
 *  Commissariat a l'Energie Atomique (CEA),
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
 
#ifndef __UNISIM_UTIL_SAV_SAV_HH__
#define __UNISIM_UTIL_SAV_SAV_HH__

#include <unisim/util/symbolic/symbolic.hh>
#include <unisim/util/random/random.hh>
#include <map>
#include <set>
#include <inttypes.h>

namespace unisim {
namespace util {
namespace sav {

  struct Untestable
  {
    Untestable(std::string const& _reason) : reason(_reason) {}
    std::string reason;
  };

  struct ActionNode : public unisim::util::symbolic::Conditional<ActionNode>
  {
    typedef unisim::util::symbolic::Expr Expr;

    ActionNode() : Conditional<ActionNode>(), updates() {}

    friend std::ostream& operator << (std::ostream& sink, ActionNode const& an)
    {
      for (auto && update : an.updates)
        sink << update << ";\n";
      if (not an.cond.node)
        return sink;
      sink << "if (" << an.cond << ")\n{\n";
      if (auto nxt = an.nexts[0]) sink << *nxt;
      sink << "}\nelse\n{\n";
      if (auto nxt = an.nexts[1]) sink << *nxt;
      sink << "}\n";
      return sink;
    }

    void add_update( Expr expr ) { expr.ConstSimplify(); updates.insert( expr ); }

    void simplify();

    std::set<Expr>           updates;
  };

  template <typename T>
  struct Operand
  {
    Operand() : raw(0) {}

    template <typename F> T Get( F const& f ) const { return (raw >> f.bit) & f.mask(); }
    template <typename F> void Set( F const& f, T v ) { raw &= ~(f.mask() << f.bit); raw |= (v & f.mask()) << f.bit; }

    bool access( bool w )
    {
      bool src = Get( Src() ), dst = Get( Dst() );
      Set( Src(), src or not w );
      Set( Dst(), dst or w );
      return src or dst;
    }

    bool is_accessed() const { return Get( Dst() ) | Get( Src() ); }
    bool is_modified() const { return Get( Dst() ); }

    T get_index() const { return Get( Idx() ); }
    T set_index( T idx ) { Set( Idx(), idx ); return idx; }

  private:
    struct Src { unsigned const bit = 0; static T mask() { return T( 1); } };
    struct Dst { unsigned const bit = 1; static T mask() { return T( 1); } };
    struct Idx { unsigned const bit = 2; static T mask() { return T(-1); } };

    T raw;
  };

  template <typename T, unsigned COUNT>
  struct OperandMap
  {
    typedef Operand<T> Op;

    OperandMap() : omap(), allocated() {}

    T touch(unsigned idx, bool w)
    {
      if (idx >= COUNT) throw "ouch";
      Operand<T>& op = omap[idx];
      if (op.access(w))
        return op.get_index();
      return op.set_index( allocated++ );
    }

    bool modified(unsigned idx) const
    {
      if (idx >= COUNT) throw "ouch";
      return omap[idx].is_modified();
    }

    bool accessed(unsigned idx) const
    {
      if (idx >= COUNT) throw "ouch";
      return omap[idx].is_accessed();
    }

    T index(unsigned idx) const
    {
      if (idx >= COUNT) throw "ouch";
      return omap[idx].get_index();
    }

    unsigned count() const { return COUNT; }
    unsigned used() const { return allocated; }

  private:
    Op omap[COUNT];
    T allocated;
  };

  struct VirtualRegister
  {
    VirtualRegister( unsigned _reg,  unsigned _idx ) : reg(_reg), idx(_idx) {} unsigned reg, idx;
    int compare(VirtualRegister const& rhs) const
    {
      if (int delta = int(idx) - int(rhs.idx)) return delta;
      return int(reg) - int(rhs.reg);
    }
  };
  
  struct Addressings
  {
    typedef unisim::util::symbolic::Expr Expr;
    typedef std::map<Expr,Expr> Solutions;

    struct Source {};
    
    Addressings() {}
    
    bool solve( Expr const& base_addr, Expr const& expected_address );
    
    Solutions solutions;
  };

  template <typename T> struct opcfg {};
  template <> struct opcfg<uint32_t> { typedef int32_t flips_t; enum { logsize=5 }; };
  template <> struct opcfg<uint64_t> { typedef int64_t flips_t; enum { logsize=6 }; };

  struct Random : public unisim::util::random::Random
  {
    Random() : unisim::util::random::Random(0,0,0,0) {}
    //uint32_t generate32() { return Generate(); }
    //    uint64_t generate64() { return (uint64_t(generate32()) << 32) | generate32(); }
    template <typename T>
    T generate()
    {
      T value = 0;
      for (unsigned idx = sizeof (T) / sizeof (uint32_t); idx > 0; --idx)
        value = value << 32 | uint32_t(Generate());
      return value;
    }
    template <typename UOP>
    UOP operand()
    {
      typedef typename opcfg<UOP>::flips_t flips_t;
      enum { logsize = opcfg<UOP>::logsize, flipshift = (1<<logsize) - 1 };
      
      flips_t flips = generate<UOP>();
      UOP value = 0;
      for (int idx = 0; idx < logsize; ++idx)
        {
          value |= generate<UOP>();
          value ^= flips >> flipshift;
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

  template <typename CELL, uintptr_t COUNT>
  struct Testbed : public TestbedBase
  {
    typedef CELL cell_t;
    Testbed( char const* seed ) : TestbedBase( seed, &buffer[0], COUNT ) {}

    void serial( std::ostream& sink ) { TestbedBase::serial( sink, &buffer[0], COUNT ); }

    uintptr_t head(uintptr_t idx=0) const { return (counter+idx) % COUNT; }

    void next()
    {
      buffer[head()] = rng.operand<cell_t>();
      counter += 1;
    }
    template <typename T> typename T::value_type const& select( T const& tests ) { return tests[counter%tests.size()]; }

    cell_t buffer[COUNT];

    void load( cell_t* ws, unsigned size ) const
    {
      if (size >= COUNT) throw 0;
      unsigned rnd_idx = head();
      for (unsigned idx = 0; idx < size; ++idx)
        {
          ws[idx] = buffer[rnd_idx];
          rnd_idx = (rnd_idx + 1) % COUNT;
        }
    }
  };


} /* end of namespace sav */
} /* end of namespace util */
} /* end of namespace unisim */

#endif /* __UNISIM_UTIL_SAV_SAV_HH__ */
