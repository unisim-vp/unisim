/*
 *  Copyright (c) 2017-2023,
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

#ifndef __UNISIM_UTIL_SYMBOLIC_CCODE_CCODE_HH__
#define __UNISIM_UTIL_SYMBOLIC_CCODE_CCODE_HH__

#include <unisim/util/symbolic/symbolic.hh>
#include <map>
#include <set>

namespace unisim {
namespace util {
namespace symbolic {
namespace ccode {

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

  struct ActionNode : public Conditional<ActionNode>
  {
    ActionNode() : Conditional<ActionNode>() {}

    void                     Repr( std::ostream& sink ) const;
    void                     simplify();
    void                     commit_stats();

    std::map<Expr,unsigned>  sestats;
  };

  struct SrcMgr
  {
    enum exception_t { IndentError, TextFormatingError };

    SrcMgr( std::ostream& _sink ) : sink( _sink ), indent( 0 ) {}

    void                  putchar( char );

    SrcMgr&               operator << ( char ch ) { putchar( ch ); return *this; }
    SrcMgr&               operator << ( char const* p ) { while (*p) putchar(*p++); return *this; }
    SrcMgr&               operator << ( std::string const& s ) { for (auto && ch : s) putchar( ch ); return *this; }

  private:
    // Text buffers
    std::ostream&         sink;

    // Buffering, Formatting and Indentation
    std::string           line, blank;
    int                   indent;
  };

  template <typename T>
  struct Hex
  {
    Hex( T _value ) : value(_value) {} T value;
    friend SrcMgr& operator << (SrcMgr& s, Hex const& h)
    {
      s << "0x";
      struct { void _( SrcMgr& s, T v ) { if (T u = v >> 4) _(s, u); s.putchar( "0123456789abcdef"[v % 16] ); } } rec;
      rec._(s,h.value);
      return s;
    }
  };
  template <typename T> Hex<T> hex( T value ) { return Hex<T>( value ); }
  template <typename T>
  struct Dec
  {
    Dec( T _value ) : value(_value) {} T value;
    friend SrcMgr& operator << (SrcMgr& s, Dec const& h)
    {
      uint64_t value;
      if (h.value >= 0) value =  h.value;
      else  { s << '-'; value = -h.value; }

      struct { void _( SrcMgr& s, uint64_t v ) { if (uint64_t u = v / 10) _(s, u); s.putchar( "0123456789"[v % 10] ); } } rec;
      rec._(s,value);
      return s;
    }
  };
  template <typename T> Dec<T> dec( T value ) { return Dec<T>( value ); }
  struct fpt { fpt(long double _value) : value(_value) {} long double value; };

  SrcMgr& operator << (SrcMgr& s, fpt f);


  struct Variable
  {
    std::string name;
    bool operator < ( Variable const& rhs ) const { return name < rhs.name; }
    char const* GetType() const;
    char const* GetName() const { return name.c_str(); }

    Variable( Variable const& ) = default;
    Variable( Variable&& ) = default;
    Variable( std::string && _name ) : name(std::forward<std::string>(_name)) {}
    Variable( char const* _name ) : name(_name) {}
  };

  struct Variables : public std::set<Variable>
  {
    friend SrcMgr& operator << (SrcMgr&, Variables const&);
  };

  struct CCode
  {
    CCode() : tmps() {}

    struct ExprPrinter
    {
      ExprPrinter(CCode& c, Expr const& e) : ccode(c), expr(e) {}
      CCode& ccode; Expr const& expr; friend SrcMgr& operator << (SrcMgr& sink, ExprPrinter const& ep);
    };
    ExprPrinter operator() (Expr const& e) { return ExprPrinter(*this,e); }
    void make_temp( SrcMgr& sink, Expr const& e );

    std::map<Expr,Variable> tmps;
  };

  struct CNode : public ExprNode
  {
    virtual void translate( SrcMgr& srcmgr, CCode& ccode ) const = 0;
    virtual int cmp( ExprNode const& rhs ) const override { return compare( dynamic_cast<CNode const&>( rhs ) ); }
    int compare( CNode const& rhs ) const { return 0; }
  };

  struct Update : public CNode
  {
    virtual ValueType GetType() const override { return NoValueType(); }
  };

  struct RegReadBase : public CNode
  {
    virtual void GetRegName(std::ostream&) const = 0;
    virtual unsigned SubCount() const override { return 0; }
    virtual void Repr( std::ostream& sink ) const override;
    virtual void translate( SrcMgr& srcmgr, CCode& ccode ) const override;
  };

  template <typename RID>
  struct RegRead : public RegReadBase
  {
    RegRead( RID _id ) : id(_id) {}
    virtual RegRead* Mutate() const override { return new RegRead( *this ); }
    virtual void GetRegName( std::ostream& sink ) const override { return id.GetName(sink, true); }
    virtual ValueType GetType() const { return RID::GetType(); }
    virtual int cmp( ExprNode const& rhs ) const override { return compare( dynamic_cast<RegRead const&>( rhs ) ); }
    int compare( RegRead const& rhs ) const { return id.cmp( rhs.id ); }

    RID id;
  };

  struct RegWriteBase : public Update
  {
    virtual void GetRegName(std::ostream& sink) const = 0;
    RegWriteBase( Expr const& _value ) : value(_value) {}
    virtual Expr const& GetSub(unsigned idx) const { if (idx != 0) return ExprNode::GetSub(idx); return value; }
    virtual unsigned SubCount() const { return 1; }
    virtual void Repr( std::ostream& sink ) const override;
    virtual void translate( SrcMgr& srcmgr, CCode& ccode ) const override;

    Expr value;
  };

  template <typename RID>
  struct RegWrite : public RegWriteBase
  {
    RegWrite( RID _id, Expr const& _value ) : RegWriteBase(_value), id(_id) {}
    virtual RegWrite* Mutate() const override { return new RegWrite( *this ); }
    virtual void GetRegName( std::ostream& sink ) const { return id.GetName(sink, false); }
    virtual int cmp( ExprNode const& rhs ) const override { return compare( dynamic_cast<RegWrite const&>( rhs ) ); }
    int compare( RegWrite const& rhs ) const { return id.cmp( rhs.id ); }

    RID id;
  };

} /* end of namespace ccode */
} /* end of namespace symbolic */
} /* end of namespace util */
} /* end of namespace unisim */

#endif // __UNISIM_UTIL_SYMBOLIC_CCODE_CCODE_HH__
