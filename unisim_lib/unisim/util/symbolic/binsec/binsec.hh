/*
 *  Copyright (c) 20017-2018,
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
 
#ifndef __UNISIM_UTIL_SYMBOLIC_BINSEC_BINSEC_HH__
#define __UNISIM_UTIL_SYMBOLIC_BINSEC_BINSEC_HH__

#include <unisim/util/symbolic/symbolic.hh>
#include <map>
#include <vector>

namespace unisim {
namespace util {
namespace symbolic {
namespace binsec {
  
  typedef SmartValue<double>   F64;
  typedef SmartValue<float>    F32;
  typedef SmartValue<bool>     BOOL;
  typedef SmartValue<uint8_t>  U8;
  typedef SmartValue<uint16_t> U16;
  typedef SmartValue<uint32_t> U32;
  typedef SmartValue<uint64_t> U64;
  typedef SmartValue<int8_t>   S8;
  typedef SmartValue<int16_t>  S16;
  typedef SmartValue<int32_t>  S32;
  typedef SmartValue<int64_t>  S64;

  struct dbx
  {
    dbx( unsigned _bytes, uint64_t _value ) : value(_value), bytes(_bytes) {} uint64_t value; unsigned bytes;
    friend std::ostream& operator << ( std::ostream& sink, dbx const& _ );
  };

  struct Program : public std::map<int,std::string>
  {
    typedef std::map<int,std::string> MapType;
    typedef MapType::iterator iterator;
    typedef MapType::const_iterator const_iterator;
    
    Program() : insn_count(0) {}
    
    void        Generate( ActionNode const* action_tree );
    
    int         next_insn() const { return insn_count; }
    int         allocate() { return insn_count++; }
    iterator    write(int idx, std::string const& s)
    {
      iterator itr = lower_bound(idx);
      if (itr != end() and itr->first == idx)
        throw std::runtime_error("overwriting statement");
      return insert( itr, std::make_pair(idx,s) );
    }
    
    int insn_count;
  };
  
  struct Label
  {
    Label( Program& _program ) : program(_program), id(-1) {}
    
    Label& operator= (Label const& l)
    {
      if (&program != &l.program) throw std::runtime_error("label programs must aggree");
      id = l.id;
      return *this;
    }
    
    int allocate() { return (id = program.allocate()); }
    
    bool valid() const { return id >= 0; }
    
    static bool subst_next( std::string& s, int next );
    
    int write( std::string const& src )
    {
      Program::iterator insn = program.write( id, src );
      if (subst_next(insn->second, program.next_insn()))
        id = program.allocate();
      return insn->first;
    }
    
    int GetID() const { return id; }
    
  private:
    Program& program;
    int id;
  };
  
  typedef std::map<Expr,Expr> Variables;
  
  struct ASExprNode : public ExprNode
  {
    virtual int GenCode( Label& label, Variables& vars, std::ostream& sink ) const = 0;
    static  int GenerateCode( Expr const& expr, Variables& vars, Label& label, std::ostream& sink );
  };

  struct GetCode
  {
    GetCode(Expr const& _expr, Variables& _vars, Label& _label) : expr(_expr), vars(_vars), label(_label) {} Expr const& expr; Variables& vars; Label& label;
    friend std::ostream& operator << ( std::ostream& sink, GetCode const& gc )
    {
      ASExprNode::GenerateCode( gc.expr, gc.vars, gc.label, sink );
      return sink;
    }
  };
  
  struct RegWrite : public ASExprNode
  {
    RegWrite( Expr const& _value, unsigned _bitsize ) : value(_value), bitsize(_bitsize) {}
      
    virtual unsigned SubCount() const { return 1; }
    virtual Expr const& GetSub(unsigned idx) const { if (idx != 0) return ExprNode::GetSub(idx); return value; }
      
    virtual intptr_t cmp( ExprNode const& brhs ) const
    {
      RegWrite const& rhs = dynamic_cast<RegWrite const&>( brhs );
      if (intptr_t delta = int(bitsize) - int(rhs.bitsize)) return delta;
      return value.cmp( rhs.value );
    }
      
    virtual int GenCode( Label& label, Variables& vars, std::ostream& sink ) const;
      
    virtual void Repr( std::ostream& sink ) const { sink << GetRegName() << " := "; value->Repr(sink); }
      
    virtual char const* GetRegName() const = 0;
      
    Expr value;
    unsigned bitsize;
  };

  struct Branch : public RegWrite
  {
    enum type_t { Jump = 0, Call, Return } type;
    Branch( Expr const& value, unsigned bitsize, type_t bt ) : RegWrite( value, bitsize ), type(bt) {}
  };
  
  struct AssertFalse : public ASExprNode
  {
    AssertFalse() {}
    virtual int GenCode( Label& label, Variables& vars, std::ostream& sink ) const
    {
      sink << "assert (false)";
      return 0;
    }

    virtual intptr_t cmp( ExprNode const& brhs ) const { return 0; }
    virtual unsigned SubCount() const { return 0; }
    virtual void Repr( std::ostream& sink ) const { sink << "assert (false)"; }
  };
    
} /* end of namespace binsec */
} /* end of namespace symbolic */
} /* end of namespace util */
} /* end of namespace unisim */

#endif // __UNISIM_UTIL_SYMBOLIC_BINSEC_BINSEC_HH__
