/*
 *  Copyright (c) 2007-2017,
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
 
#include <unisim/util/symbolic/binsec/binsec.hh>
#include <ostream>
#include <sstream>
#include <iomanip>

namespace unisim {
namespace util {
namespace symbolic {
namespace binsec {

  std::ostream&
  operator << ( std::ostream& sink, dbx const& _ )
  {
    sink << "0x" << std::hex << std::setw(_.bytes*2) << std::setfill('0') << _.value << std::dec;
    return sink;
  }

  bool
  Label::subst_next( std::string& s, int next )
  {
    uintptr_t pos = s.find( "<next>" );
    if (pos == std::string::npos)
      return false;
    { std::ostringstream buf; buf << next; s.replace(pos, 6, buf.str()); }
    return true;
  }

  int
  RegWrite::GenCode( Label& label, Variables& vars, std::ostream& sink ) const
  {
    sink << GetRegName() << "<" << std::dec << bitsize << "> := " << GetCode(value, vars, label);
    return 0;
  }

  int
  ASExprNode::GenerateCode(Expr const& expr, Variables& vars, Label& label, std::ostream& sink)
  {
    /*** Pre expression process ***/
    Variables::iterator itr = vars.find( expr );
    if (itr != vars.end())
      {
        return static_cast<ASExprNode const*>( itr->second.node ) ->GenCode( label, vars, sink );
      }
    
    /*** Sub expression process ***/
    if (ConstNodeBase const* node = expr->GetConstNode())
      {
        switch (node->GetType())
          {
          case ScalarType::BOOL: sink << node->GetS32() << "<1>";  return 1;
          case ScalarType::U8:  case ScalarType::S8:  sink << dbx(1, node->GetU8());  return 8;
          case ScalarType::U16: case ScalarType::S16: sink << dbx(2,node->GetU16()); return 16;
          case ScalarType::U32: case ScalarType::S32: sink << dbx(4,node->GetU32()); return 32;
          case ScalarType::U64: case ScalarType::S64: sink << dbx(8,node->GetU64()); return 64;
          default: break;
          }
        throw std::logic_error("can't encode type");
      }
    else if (OpNodeBase const* node = expr->AsOpNode())
      {
        switch (node->SubCount())
          {
          case 2: {
            int retsz = GenerateCode( node->GetSub(0), vars, label, sink << '(' );

            Expr rhs = node->GetSub(1);

            switch (node->op.code)
              {
              default:                sink << " [" << node->op.c_str() << "] "; break;
            
              case Op::Add:     sink << " + "; break;
              case Op::Sub:     sink << " - "; break;
              case Op::Mul:     sink << " * "; break;
        
              case Op::Xor:     sink << " xor "; break;
              case Op::Or:      sink << " or "; break;
              case Op::And:     sink << " and "; break;
        
              case Op::Teq:     sink << " = "; retsz = 1; break;
              case Op::Tne:     sink << " <> "; retsz = 1; break;
        
              case Op::Tle:     sink << " <=s "; retsz = 1; break;
              case Op::Tleu:    sink << " <=u "; retsz = 1; break;
        
              case Op::Tge:     sink << " >=s "; retsz = 1; break;
              case Op::Tgeu:    sink << " >=u "; retsz = 1; break;
        
              case Op::Tlt:     sink << " <s "; retsz = 1; break;
              case Op::Tltu:    sink << " <u "; retsz = 1; break;
        
              case Op::Tgt:     sink << " >s "; retsz = 1; break;
              case Op::Tgtu:    sink << " >u "; retsz = 1; break;

                struct
                {
                  Expr operator() (U8 x, int s)
                  {
                    if (s==16) return U16(x).expr;
                    if (s==32) return U32(x).expr;
                    if (s==64) return U64(x).expr;
                    return x.expr;
                  }
                } fixsh;
            
              case Op::Lsl:     sink << " lshift ";  rhs = fixsh( rhs, retsz ); break;
              case Op::Asr:     sink << " rshifts "; rhs = fixsh( rhs, retsz ); break;
              case Op::Lsr:     sink << " rshiftu "; rhs = fixsh( rhs, retsz ); break;
              case Op::Ror:     sink << " rrotate "; rhs = fixsh( rhs, retsz ); break;
                 
                // case Op::Mod: break;
                // case Op::Div: break;
              }

            sink << GetCode( rhs, vars, label ) << ')';
            return retsz;
          }
            
          case 1: {
            char const* operation = 0;
        
            switch (node->op.code)
              {
              default:              operation = node->op.c_str(); break;
        
              case Op::Not:    operation = "not "; break;
              case Op::Neg:    operation = "- "; break;
        
                // case Op::BSwp:  break;
                // case Op::BSF:   break;
            
              case Op::BSR:
                {
                  Label head(label);
                  int exit = label.allocate(), loop; 
                  {
                    std::ostringstream buffer;
                    buffer << "bsr_in<32> := " << GetCode(node->GetSub(0), vars, head) << " ; goto <next>";
                    head.write( buffer.str() );
                    head.write( "bsr_out<32> := 32<32> ; goto <next>" );
                  }
                  {
                    std::ostringstream buffer;
                    buffer << "if (bsr_in<32> = 0<32>) goto " << exit << " else goto <next>";
                    head.write( buffer.str() );
                    loop = head.write( "bsr_out<32> := bsr_out<32> - 1<32> ; goto <next>" );
                  }
                  {
                    std::ostringstream buffer;
                    buffer << "if ((bsr_in<32> rshiftu bsr_out<32>){0,0}) goto " << exit << " else goto " << loop;
                    head.write( buffer.str() );
                  }
      
                  sink << "bsr_out<32>";
      
                  return 32;
                }
              case Op::Cast:
                {
                  CastNodeBase const& cnb = dynamic_cast<CastNodeBase const&>( *expr.node );
                  ScalarType src( cnb.GetSrcType() ), dst( cnb.GetDstType() );
                  
                  if (dst.is_integer and src.is_integer)
                    {
                      int extend = dst.bitsize - src.bitsize;
                      if      (extend > 0)
                        {
                          sink << '(' << (src.is_signed ? "exts " : "extu ") << GetCode(cnb.src, vars, label) << ' ' << dst.bitsize << ')';
                        }
                      else if (extend < 0)
                        {
                          CastNodeBase const* src_node = dynamic_cast<CastNodeBase const*>( cnb.src.node );
                          if (src_node and (src_node->GetSrcType() == cnb.GetDstType())) {
                            sink << GetCode(src_node->src, vars, label);
                          } else  if (dst.bitsize == 1) {
                            sink << "(" << GetCode(cnb.src, vars, label) << " <> " << dbx(src.bitsize/8,0) << ")";
                          } else {
                            sink << "(" << GetCode(cnb.src, vars, label) << " {0," << (dst.bitsize-1) << "})";
                          }
                        }
                      else
                        sink << GetCode(cnb.src, vars, label);
                    }
                  else
                    {
                      sink << dst.name << "( " << GetCode(cnb.src, vars, label) << " )";
                    }
      
                  return dst.bitsize;
                }
              }
    
            sink << '(' << operation << ' ';
            int retsz = GenerateCode( node->GetSub(0), vars, label, sink );
            sink << ')';
            return retsz;
          }
          default:
            break;
          }
      }
    else if (ASExprNode const* node = dynamic_cast<ASExprNode const*>( expr.node ))
      {
        return node->GenCode( label, vars, sink );
      }
      
    throw std::logic_error("No GenCode method");
    return 0;
  }

  void
  Program::Generate( ActionNode const* action_tree )
  {
    struct Context
    {
      Context() : upper(0) {}
      Context( Context* _up ) : upper(_up)
      {
        if (_up) next_tmp = _up->next_tmp;
        if (_up) vars = _up->vars;
      }
      
      void add_pending( Expr e ) { pendings.push_back(e); }
      bool has_pending() const { return pendings.size() > 0; }
      
      Expr const& addvar( std::string const& name, unsigned size, Expr const& expr )
      {
        Expr& place = vars[expr];
      
        if (place.good())
          throw std::logic_error( "multiple temporary definitions" );
      
        struct TmpVar : public ASExprNode
        {
          TmpVar( std::string const& _ref, unsigned rsz )
            : ref(_ref), dsz(rsz)
          {}
          virtual int GenCode( Label& label, Variables& vars, std::ostream& sink ) const { sink << ref; return dsz; }
          virtual intptr_t cmp( ExprNode const& rhs ) const { return ref.compare( dynamic_cast<TmpVar const&>( rhs ).ref ); }
          virtual unsigned SubCount() const { return 0; }
          virtual void Repr( std::ostream& sink ) const { sink << ref; }
          std::string ref;
          int dsz;
        };
      
        place = new TmpVar( name, size );
        return place;
      }      
      std::string nxtname( char const* rname, unsigned size ) { std::ostringstream buffer; buffer << "nxt_" << rname << "<" << size << ">"; return buffer.str(); }
      std::string tmpname( unsigned size ) { std::ostringstream buffer; buffer << "tmp" << size << '_' << (next_tmp[size]++) << "<" << size << ">"; return buffer.str(); }

      void GenCode( ActionNode const* action_tree, Label const& start, Label const& after )
      {
        Expr nia;
        Branch::type_t bt = Branch::Jump;
    
        // Using a delayed writing mechanism so that the last code line
        // produced is linked to the next code line given by the upper
        // context instead of a fresh new one
        struct Head
        {
          Head( Label const& _start, int _after ) : cur(_start), after(_after) {}
          ~Head()
          {
            if (not pending.size()) return;
            Label::subst_next(pending, after);
            cur.write( pending );
            pending.clear();
          }

          Label& current() { flush(); return cur; }
          void write( std::string const& s )
          {
            flush();
            if (s.size()==0) throw std::runtime_error("xxx");
            pending = s;
          }
          void flush()
          {
            if (not pending.size()) return;
            cur.write( pending );
            pending.clear();
          }
          Label cur;
          int after;
          std::string pending;
        } head( start, after.GetID() );

        {
          /* find common sub expressions */
          struct : public std::map<Expr,unsigned>
          {
            void Process( Expr const& expr )
            {
              for (unsigned idx = 0, end = expr->SubCount(); idx < end; ++idx)
                Recurse( expr->GetSub(idx) );
            }

            void Recurse( Expr const& expr )
            {
              if (not expr->SubCount())
                return;

              if ((*this)[expr]++)
                return;

              Process( expr );
            }
          } sestats;

          std::map<Expr,char const*> rtmps;

          for (std::set<Expr>::const_iterator itr = action_tree->sinks.begin(), end = action_tree->sinks.end(); itr != end; ++itr)
            {
              sestats.Process( *itr );
              if (RegWrite const* rw = dynamic_cast<RegWrite const*>( itr->node ))
                rtmps[rw->value] = rw->GetRegName();
            }

          // if (cond.good())
          //   sestats.Process( cond );

          struct CSE : public std::multimap<unsigned,Expr>
          {
            void Process( Expr const& expr )
            {
              insert( std::make_pair( CountSubs( expr ), expr ) );
            }
            unsigned CountSubs( Expr const& expr )
            {
              unsigned sum = 1;
              for (unsigned idx = 0, end = expr->SubCount(); idx < end; ++idx)
                sum += CountSubs( expr->GetSub(idx) );
              return sum;
            }
          } cse;

          for (std::map<Expr,unsigned>::iterator itr = sestats.begin(), end = sestats.end(); itr != end; ++itr)
            {
              if (itr->second < 2)
                continue; // No reuse
              if (this->vars.count(itr->first))
                continue; // Already defined
              cse.Process(itr->first);
            }

          for (std::multimap<unsigned,Expr>::const_iterator itr = cse.begin(), end = cse.end(); itr != end; ++itr)
            {
              std::string tmp_src, tmp_dst;
              int retsize;
              {
                std::ostringstream buffer;
                retsize = ASExprNode::GenerateCode( itr->second, this->vars, head.current(), buffer );
                tmp_src = buffer.str();
                std::map<Expr,char const*>::const_iterator rtmp = rtmps.find(itr->second);
                tmp_dst = rtmp != rtmps.end() ? this->nxtname( rtmp->second, retsize ) : this->tmpname( retsize );
                this->addvar( tmp_dst, retsize, itr->second );
              }
              {
                std::ostringstream buffer;
                buffer << tmp_dst << " := " << tmp_src << "; goto <next>";
                head.write( buffer.str() );
              }
            }
        }

        for (std::set<Expr>::const_iterator itr = action_tree->sinks.begin(), end = action_tree->sinks.end(); itr != end; ++itr)
          {
            if (RegWrite const* rw = dynamic_cast<RegWrite const*>( itr->node ))
              {
                char const* rname = rw->GetRegName();
                unsigned    rsize = rw->bitsize;
                Expr        value = rw->value;

                if (not value.ConstSimplify() and not this->vars.count(value))
                  {
                    std::ostringstream buffer;
                    buffer << "nxt_" << rname << "<" << rsize << ">";
                    std::string vname = buffer.str();
                    buffer << " := " << GetCode(value, this->vars, head.current()) << "; goto <next>";
                    head.write( buffer.str() );
                    this->addvar( vname, rsize, value );
                  }

                if (Branch const* branch = dynamic_cast<Branch const*>( rw ))
                  { nia = value; bt = branch->type; }
                else
                  this->add_pending( *itr );
              }
            else
              {
                std::ostringstream buffer;
                if (ASExprNode::GenerateCode( *itr, this->vars, head.current(), buffer ))
                  throw 0;

                buffer << "; goto <next>";
                head.write( buffer.str() );
              }
          }

        if (not action_tree->cond.good())
          {
            if (action_tree->sinks.size() == 0)
              throw std::logic_error( "empty leaf" );
          }
        else
          {
            std::ostringstream buffer;
            Label ifinsn(head.current());
            buffer << "if " << GetCode(action_tree->cond, this->vars, ifinsn) << " ";

            // Preparing room for if then else code
            Label endif( after );
            if (nia.good() or (after.valid() and (this->has_pending())))
              endif.allocate();

            if (not action_tree->false_nxt) {
              Label ifthen(ifinsn);
              buffer << " goto " << ifthen.allocate() << " else goto " << endif.GetID();
              ifinsn.write( buffer.str() );
              Context nxt(this);
              nxt.GenCode( action_tree->true_nxt, ifthen, endif );
            } else if (not action_tree->true_nxt) {
              Label ifelse(ifinsn);
              buffer << " goto " << endif.GetID() << " else goto " << ifelse.allocate();
              ifinsn.write( buffer.str() );
              Context nxt(this);
              nxt.GenCode( action_tree->false_nxt, ifelse, endif );
            } else {
              Label ifthen(ifinsn), ifelse(ifinsn);
              buffer << " goto " << ifthen.allocate() << " else goto " << ifelse.allocate();
              ifinsn.write( buffer.str() );
              {
                Context nxt(this);
                nxt.GenCode( action_tree->true_nxt, ifthen, endif );
              }
              {
                Context nxt(this);
                nxt.GenCode( action_tree->false_nxt, ifelse, endif );
              }
            }

            head.current() = endif;
          }

        for (Pendings::iterator itr = this->pendings.begin(), end = this->pendings.end(); itr != end; ++itr)
          {
            std::ostringstream buffer;
            if (ASExprNode::GenerateCode( *itr, this->vars, head.current(), buffer ))
              throw 0;

            buffer << "; goto <next>";
            head.write( buffer.str() );
          }

        if (not nia.good())
          return;

        Label current( head.current() );

        for (Context* uc = this->upper; uc; uc = uc->upper)
          {
            for (Pendings::iterator itr = uc->pendings.begin(), end = uc->pendings.end(); itr != end; ++itr)
              {
                std::ostringstream buffer;
                if (ASExprNode::GenerateCode( *itr, this->vars, current, buffer ))
                  throw 0;

                buffer << "; goto <next>";
                current.write( buffer.str() );
              }
          }

        std::ostringstream buffer;
        buffer << "goto (" << GetCode(nia, this->vars, current) << (nia.ConstSimplify() ? ",0" : "") << ")";
        if (bt == Branch::Call) buffer << " // call";
        current.write( buffer.str() );
      }

      Context* upper;
      typedef std::vector<Expr> Pendings;
      Pendings pendings;
      Variables vars;
      std::map<unsigned,unsigned> next_tmp;
    };
  
    Label beglabel(*this), endlabel(*this);
    beglabel.allocate();
    Context ctx;
    ctx.GenCode( action_tree, beglabel, endlabel );
  }
  

} /* end of namespace binsec */
} /* end of namespace symbolic */
} /* end of namespace util */
} /* end of namespace unisim */

