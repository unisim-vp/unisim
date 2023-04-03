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
#include <iostream>
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

    ActionNode() : unisim::util::symbolic::Conditional<ActionNode>(), updates() {}

    friend std::ostream& operator << (std::ostream& sink, ActionNode const& an);

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
    struct Ouch {};

    OperandMap() : omap(), allocated() {}

    T touch(unsigned idx, bool w)
    {
      if (idx >= COUNT) throw Ouch();
      Operand<T>& op = omap[idx];
      if (op.access(w))
        return op.get_index();
      return op.set_index( allocated++ );
    }

    bool modified(unsigned idx) const
    {
      if (idx >= COUNT) throw Ouch();
      return omap[idx].is_modified();
    }

    bool accessed(unsigned idx) const
    {
      if (idx >= COUNT) throw Ouch();
      return omap[idx].is_accessed();
    }

    T index(unsigned idx) const
    {
      if (idx >= COUNT) throw Ouch();
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
  
  struct Comparator
  {
    int process( unisim::util::sav::ActionNode const& a, unisim::util::sav::ActionNode const& b ) const
    {
      if (int delta = a.updates.size() - b.updates.size()) return delta;
      auto rci = b.updates.begin();
      for (Expr const& update : a.updates)
        { if (int delta = process( update,  *rci )) return delta; ++rci; }
          
      if (int delta = process( a.cond, b.cond )) return delta;
      for (int idx = 0; idx < 2; ++idx)
        {
          if     (not a.nexts[idx])
            { if (b.nexts[idx]) return -1; }
          else if(b.nexts[idx])
            { if (int delta = process( *a.nexts[idx], *b.nexts[idx] )) return delta; }
        }
      return 0;
    }

    typedef unisim::util::symbolic::Expr Expr;

    int process( Expr const& a, Expr const& b ) const
    {
      // Do not compare null expressions
      if (not b.node) return a.node ?  1 : 0;
      if (not a.node) return b.node ? -1 : 0;
      
      /* First compare actual types */
      const std::type_info* til = &typeid(*a.node);
      const std::type_info* tir = &typeid(*b.node);
      if (til < tir) return -1;
      if (til > tir) return +1;
        
      /* Same types, call derived comparator except for Constants (compare popcount)*/
      typedef unisim::util::symbolic::ConstNodeBase ConstNodeBase;
      if (auto an = dynamic_cast<ConstNodeBase const*>(a.node))
        {
          int abits = 0, bbits = 0;
          for (unsigned idx = 0, end = (an->GetType()->GetBitSize()+63)/64; idx < end; ++idx)
            {
              uint64_t av = an->GetBits( idx ), bv = dynamic_cast<ConstNodeBase const&>(*b.node).GetBits( idx );
              abits += __builtin_popcountll(av);
              bbits += __builtin_popcountll(bv);
            }
          if (int delta = abits - bbits)
            return  delta;
        }
      else if (auto vr = dynamic_cast<unisim::util::sav::VirtualRegister const*>(a.node))
        {
          unsigned ai = vr->idx, bi = dynamic_cast<unisim::util::sav::VirtualRegister const&>(*b.node).idx;
          if (int delta = int(ai) - int(bi))
            return delta;
        }
      else if (int delta = a.node->cmp( *b.node ))
        return delta;
      
      /* Compare sub operands recursively */
      unsigned subcount = a.node->SubCount();
      if (int delta = int(subcount) - int(b.node->SubCount()))
        return delta;
      for (unsigned idx = 0; idx < subcount; ++idx)
        if (int delta = process( a.node->GetSub(idx), b.node->GetSub(idx)))
          return delta;

      /* equal to us */
      return 0;
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
    uint32_t generate32() { return Generate(); }
    //    uint64_t generate64() { return (uint64_t(generate32()) << 32) | generate32(); }
    template <typename T>
    T generate()
    {
      typedef uint32_t chunk_t;
      T value = 0;
      for (unsigned idx = sizeof (T) / sizeof (chunk_t); idx-- > 0;)
        value |= T(chunk_t(Generate())) << (8*sizeof (chunk_t)*idx);
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

  template <unsigned SUBCOUNT, class OP>
  struct WeirdOpBase : public unisim::util::symbolic::ExprNode
  {
    typedef unisim::util::symbolic::ValueType ValueType;
    typedef unisim::util::symbolic::Expr Expr;
    typedef WeirdOpBase<SUBCOUNT,OP> this_type;
    WeirdOpBase( OP && _op ) : op(_op) {}
    virtual void Repr(std::ostream& sink) const override
    {
      sink << "WeirdOp<";
      GetType()->GetName(sink);
      sink << "," << SUBCOUNT << "," << op << ">( ";
      char const* sep = "";
      for (unsigned idx = 0; idx < SUBCOUNT; sep = ", ", ++idx)
        sink << sep << subs[idx];
      sink << " )";
    }
    virtual unsigned SubCount() const override { return SUBCOUNT; }
    virtual Expr const& GetSub(unsigned idx) const override { if (idx >= SUBCOUNT) return ExprNode::GetSub(idx); return subs[idx]; }
    virtual int cmp( ExprNode const& brhs ) const override { return compare( dynamic_cast<WeirdOpBase const&>( brhs )); }
    int compare( this_type const& rhs ) const { return strcmp( op, rhs.op ); }

    OP op;
    Expr subs[SUBCOUNT];
  };

  template <class T, unsigned SUBCOUNT, class OP>
  struct WeirdOp : public WeirdOpBase<SUBCOUNT,OP>
  {
    typedef WeirdOpBase<SUBCOUNT,OP> base_type;
    typedef WeirdOp<T,SUBCOUNT,OP>   this_type;

    WeirdOp( OP && name ) : base_type(std::move(name)) {}
    virtual this_type* Mutate() const override { return new this_type( *this ); }
    virtual typename base_type::ValueType const* GetType() const { return unisim::util::symbolic::CValueType(typename T::value_type()); }
  };

  template <class OUT, class OP, class T1>
  unisim::util::symbolic::Expr
  make_weirdop( OP && op, T1 const& op1 )
  {
    auto x = new WeirdOp<OUT,1,OP>(std::move(op));
    x->subs[0] = op1.expr;
    return x;
  }

  template <class OUT, class OP, class T1, class T2>
  unisim::util::symbolic::Expr
  make_weirdop( OP && op, T1 const& op1, T2 const& op2 )
  {
    auto x = new WeirdOp<OUT,2,OP>(std::move(op));
    x->subs[0] = op1.expr;
    x->subs[1] = op2.expr;
    return x;
  }

  template <class OUT, class OP, class T1, class T2, class T3>
  unisim::util::symbolic::Expr
  make_weirdop( OP && op, T1 const& op1, T2 const& op2, T3 const& op3 )
  {
    auto x = new WeirdOp<OUT,3,OP>(std::move(op));
    x->subs[0] = op1.expr;
    x->subs[1] = op2.expr;
    x->subs[2] = op3.expr;
    return x;
  }

  struct TestbedBase
  {
    TestbedBase( char const* seed, uint8_t* elems, uintptr_t count );

    void serial( std::ostream& sink, uint8_t* elems, uintptr_t count ) const;

    Random rng;
    uintptr_t counter;
  };

  template <typename CELL, uintptr_t COUNT>
  struct Testbed : public TestbedBase
  {
    typedef CELL cell_t;
    Testbed( char const* seed ) : TestbedBase( seed, reinterpret_cast<uint8_t*>(&buffer[0]), COUNT ), buffer() {}

    void serial( std::ostream& sink ) { TestbedBase::serial( sink, reinterpret_cast<uint8_t*>(&buffer[0]), COUNT ); }

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
      if (size >= COUNT)
        { struct Bad {}; throw Bad (); }
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
