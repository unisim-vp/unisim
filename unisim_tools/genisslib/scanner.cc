/***************************************************************************
                                   gilparser.cc
                             -------------------
    begin                : Thu Nov 21 2019
    copyright            : (C) 2019-2020 CEA and Universite Paris Sud
    authors              : Gilles Mouchard, Yves Lhuillier
    email                : gilles.mouchard@cea.fr, yves.lhuillier@cea.fr
***************************************************************************/

#include <scanner.hh>
#include <isa.hh>
#include <comment.hh>
#include <sourcecode.hh>
#include <action.hh>
#include <operation.hh>
#include <bitfield.hh>
#include <variable.hh>
#include <specialization.hh>
#include <strtools.hh>
#include <clex.hh>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <limits>
#include <cstring>
#include <cassert>
#include <unistd.h>

//bool                     Scanner::aborted_scanning = false;
// FileLoc                  Scanner::fileloc;
// FileLoc                  Scanner::fileloc_mlt;
// int                      Scanner::bracecount = 0;
// std::vector<int>         Scanner::scs;
Vector<Comment>          Scanner::comments;
// Isa*                     Scanner::s_isa = 0;
ConstStr::Pool           Scanner::symbols;
std::vector<ConstStr>    Scanner::s_lookupdirs;

// Scanner::Inclusion* Scanner::include_stack = 0;

// std::string& Scanner::strbuf() { static std::string s_buffer; return s_buffer; }

// Scanner::Inclusion::Inclusion( uint8_t const* _state, intptr_t _size, FileLoc const& _fileloc, Inclusion* _next )
//   : state_backup( 0 ), fileloc( _fileloc ), next( _next )
// {
//   state_backup = new uint8_t[_size];
//   std::copy(&_state[0], &_state[_size], &state_backup[0]);
//   //  memcpy( state_backup, _state, _size );
// }

// Scanner::Inclusion::~Inclusion()
// {
//   delete [] state_backup;
// }

// void
// Scanner::Inclusion::restore( uint8_t* _state, intptr_t _size )
// {
//   std::copy(&state_backup[0], &state_backup[_size], _state);
//   //  memcpy( _state, state_backup, _size );
// }

namespace
{
  FileLoc GetFileLoc(CLex::Scanner& s)
  {
    return FileLoc(s.sourcename(), s.lidx, s.cidx );
  }
  SourceCode GetSourceCode(CLex::Scanner& src)
  {
    FileLoc && fl = GetFileLoc(src);
    std::string buf;
    int depth = 0;
    enum { InCode = 0, InString, InEsc } state = InCode;

    for (;;)
      {
        char ch = src.getchar();
        switch (state)
          {
          case InCode:
            if      (ch == '"')  state = InString;
            if      (ch == '{')  depth += 1;
            else if (ch == '}')  depth -= 1;
            break;
            
          case InString:
            if      (ch == '"')  state = InCode;
            else if (ch == '\\') state = InEsc;
            break;
            
          case InEsc:
            state = InString;
            break;
          }
        if (depth < 0) break;
        buf += ch;
      }

    return SourceCode( buf.c_str(), std::move(fl) );
  }
  
  unsigned GetInteger(CLex::Scanner& src)
  {
    unsigned base = 10;
    if (src.lch == '0')
      {
        char d = src.nextchar() ? src.lch : '\0';
        if (isdigit(d))                { base = 10; /*not octal for now*/ }
        else if (d == 'b' or d == 'x') { base = (d == 'b') ? 2 : 16; src.getchar(); }
        else { src.putback = true; return 0; }
      }

    unsigned res = 0;
    for (bool hasdigit = false;; hasdigit = true)
      {
        unsigned digit = 16;
        if      ('0' <= src.lch and src.lch <= '9')
          { digit = src.lch - '0'; }
        else if ('a' <= src.lch and src.lch <= 'f')
          { digit = src.lch - 'a' + 10; }
        else if ('A' <= src.lch and src.lch <= 'F')
          { digit = src.lch - 'A' + 10; }
        else if (hasdigit)
          break;
        
        if (digit >= base)
          throw src.unexpected();

        res = res*base + digit;
        if (not src.nextchar()) break;
      }

    src.putback = true;
    return res;
  }

  unsigned GetArraySize(CLex::Scanner& src)
  {
    if (src.next() != CLex::Scanner::ArrayOpening)
      throw src.unexpected();
    if (src.next() != CLex::Scanner::Number)
      throw src.unexpected();
    unsigned size = GetInteger(src);
    if (src.next() != CLex::Scanner::ArrayClosing)
      throw src.unexpected();
    return size;
  }
  
  Ptr<Operation>
  GetOp(CLex::Scanner& src, Isa& _isa)
  {
    if (src.next() != CLex::Scanner::Name)
      throw src.unexpected();
    
    ConstStr symbol;
    FileLoc && symfl = GetFileLoc(src);
    {
      std::string buf;
      src.get(buf,&CLex::Scanner::get_name);
      symbol = ConstStr(buf.c_str(),Scanner::symbols);
    }

    if (src.next() != CLex::Scanner::GroupOpening)
      throw src.unexpected();
    
    Vector<BitField> bitfields;
    for (;;)
      {
        switch (src.next())
          {
          case CLex::Scanner::More:
            {
              // Separator
              bool rewind = src.next() != CLex::Scanner::Less;
              if (rewind)
                {
                  if (src.lnext != CLex::Scanner::Name or
                      not src.expect("rewind", &CLex::Scanner::get_name) or
                      src.next() != CLex::Scanner::Less)
                    throw src.unexpected();
                }
              bitfields.push_back( new SeparatorBitField( rewind ) );
            }
            break;
            
          case CLex::Scanner::Number:
            {
              // OpCode
              unsigned bits = GetInteger(src), size = GetArraySize(src);
              bitfields.push_back( new OpcodeBitField( size, bits ) );
            }
            break;

          case CLex::Scanner::QuestionMark:
            {
              // Unused
              unsigned size = GetArraySize(src);
              bitfields.push_back( new UnusedBitField( size ) );
            }
            break;

          case CLex::Scanner::Star:
            {
              // SubOp
              if (src.next() != CLex::Scanner::Name)
                throw src.unexpected();
              ConstStr symbol;
              {
                std::string buf;
                src.get(buf, &CLex::Scanner::get_name);
                symbol = ConstStr( buf.c_str(), Scanner::symbols );
              }
              if (src.next() != CLex::Scanner::ArrayOpening)
                throw src.unexpected();
              if (src.next() != CLex::Scanner::Name)
                throw src.unexpected();
              ConstStr sdinstance_symbol;
              {
                std::string buf;
                src.get(buf, &CLex::Scanner::get_name); 
                sdinstance_symbol = ConstStr( buf.c_str(), Scanner::symbols );
              }
              if (src.next() != CLex::Scanner::ArrayClosing)
                throw src.unexpected();
              if (SDInstance const* sdinstance = _isa.sdinstance( sdinstance_symbol ))
                {
                  bitfields.push_back( new SubOpBitField( symbol, sdinstance ) );
                }
              else
                {
                  std::cerr << "error: subdecoder instance `" << sdinstance_symbol.str() << "' not declared.\n";
                  throw src.unexpected();
                }
            }
            break;

          default:
            {
              // Operand
              int shift = 0;
              bool sext = false;
              std::string buf;
              
              if (src.lnext == CLex::Scanner::Name)
                {
                  for (;;)
                    {
                      src.get(buf, &CLex::Scanner::get_name);
                      if (buf == "shl")
                        {
                          if (src.next() != CLex::Scanner::Less)
                            throw src.unexpected();
                          if (src.next() != CLex::Scanner::Number)
                            throw src.unexpected();
                          shift = -GetInteger(src);
                          if (src.next() != CLex::Scanner::More)
                            throw src.unexpected();
                        }
                      else if (buf == "sext")
                        {
                          sext = true;
                        }
                      else
                        break;
                      
                      if (src.next() != CLex::Scanner::Name)
                        {
                          if (src.lnext != CLex::Scanner::Less)
                            throw src.unexpected();
                          break;
                        }
                    }
                }

              int final_size = 0;

              if (src.lnext == CLex::Scanner::Less)
                {
                  if (src.next() != CLex::Scanner::Number)
                    throw src.unexpected();
                  final_size = GetInteger(src);
                  if (src.next() != CLex::Scanner::More)
                    throw src.unexpected();
                  if (src.next() != CLex::Scanner::Name)
                    throw src.unexpected();
                  src.get(buf, &CLex::Scanner::get_name);
                }

              if (src.lnext != CLex::Scanner::Name)
                throw src.unexpected();
                
              ConstStr symbol( buf.c_str(), Scanner::symbols );
              
              unsigned size = GetArraySize(src);

              bitfields.push_back( new OperandBitField( size, symbol, shift, final_size, sext ) );
            }
            break;
          }

        if (src.next() == CLex::Scanner::GroupClosing)
          break;

        if (src.lnext != CLex::Scanner::Colon)
          throw src.unexpected();
      }
    
    auto res = new Operation(symbol, bitfields, Scanner::comments, 0, symfl);
    Scanner::comments.clear();
    return res;
  }

  void
  GetVarList( CLex::Scanner& source, Vector<Variable>& var_list )
  {
    for (;;)
      {
        if (source.next() != CLex::Scanner::Name)
          throw source.unexpected();

        ConstStr varname;
        {
          std::string buf;
          source.get(buf, &CLex::Scanner::get_name);
          varname = ConstStr(buf.c_str(), Scanner::symbols);
        }
        if (source.next() != CLex::Scanner::Colon)
          throw source.unexpected();
        if (source.next() != CLex::Scanner::ObjectOpening)
          throw source.unexpected();
        SourceCode* c_type = new SourceCode(GetSourceCode(source));
        if (source.next() != CLex::Scanner::Assign)
          { var_list.push_back( new Variable( varname, c_type, 0 ) ); break; }
        if (source.next() != CLex::Scanner::ObjectOpening)
          throw source.unexpected();
        SourceCode* c_init = new SourceCode(GetSourceCode(source));
        var_list.push_back( new Variable( varname, c_type, c_init ) );
        if (source.next() != CLex::Scanner::Comma)
          break;
      }
  }
}

bool
Scanner::parse( char const* _filename, Isa& _isa )
{
  std::cerr << "Opening " << _filename << '\n';
  _isa.m_includes.push_back( _filename );
  
  struct FileScanner : public CLex::Scanner
  {
    FileScanner(char const* f) : CLex::Scanner(), ifs(f), filename(f), esc('\n') {}
    virtual char const* sourcename() const override { return filename.c_str(); }
    virtual bool sourcegood() const override { return ifs.good(); }
    virtual bool sourceget(char& lch) override
    {
      if (esc != '\n') { lch = esc; esc = '\n'; return true; }
      for (;;)
        {
          bool good = ifs.get(lch).good();
          if (not good or lch != '\\') return good;
          if (not ifs.get(esc).good()) throw Unexpected();
          if (esc != '\n') break;
          lidx += 1;
          cidx = 0;
        }
      return true;
    }
    
    std::ifstream ifs;
    std::string filename;
    char esc;
  } source(_filename);
  
  if (not source.ifs.good())
    {
      std::cerr << "error: cannot open `" << _filename << "'\n";
      return false;
    }
  
  
  source.next();

  while (source.lnext != CLex::Scanner::EoF)
    {
      switch (source.lnext) /* global statement */
        {
        case CLex::Scanner::Name:
          {
            FileLoc && cmdfl = GetFileLoc(source);
            std::string cmd;
            source.get(cmd, &CLex::Scanner::get_name);
        
            if (std::equal(cmd.begin(), cmd.end(), "decl") or
                std::equal(cmd.begin(), cmd.end(), "impl"))
              {
                auto& member = (cmd[0] == 'd') ? _isa.m_decl_srccodes : _isa.m_impl_srccodes;
                if (source.next() != CLex::Scanner::ObjectOpening)
                  throw source.unexpected();
                member.push_back( new SourceCode(GetSourceCode(source)) );
                source.next();
              }
            else if (std::equal(cmd.begin(), cmd.end(), "action"))
              {
                SourceCode* returns = 0;
                if (source.next() == CLex::Scanner::ObjectOpening)
                  {
                    returns = new SourceCode(GetSourceCode(source));
                    source.next();
                  }
                
                ConstStr symbol;
                FileLoc && symfl = GetFileLoc(source);
                if (source.lnext != CLex::Scanner::Name)
                  throw source.unexpected();
                {
                  std::string buf;
                  source.get(buf, &CLex::Scanner::get_name);
                  symbol = ConstStr(buf.c_str(),Scanner::symbols);
                  ActionProto const*  prev_proto = _isa.actionproto( symbol );
                  if (prev_proto)
                    {
                      symfl.err( "error: action prototype `%s' redefined", prev_proto->m_symbol.str() );
                      prev_proto->m_fileloc.err( "action prototype `%s' previously defined here", prev_proto->m_symbol.str() );
                      throw CLex::Scanner::Unexpected();
                    }
                }

                Vector<CodePair> param_list;
                if (source.next() != CLex::Scanner::GroupOpening)
                  throw source.unexpected();

                if (source.next() != CLex::Scanner::GroupClosing)
                  {
                    source.putback = true;
                    for (;;)
                      {
                        if (source.next() != CLex::Scanner::ObjectOpening)
                          throw source.unexpected();
                        SourceCode&& tp = GetSourceCode(source);
                        if (source.next() != CLex::Scanner::ObjectOpening)
                          throw source.unexpected();
                        SourceCode&& nm = GetSourceCode(source);
                        param_list.append( new CodePair( new SourceCode(std::move(tp)), new SourceCode(std::move(nm)) ) );
                        if (source.next() == CLex::Scanner::GroupClosing)
                          break;
                        if (source.lnext != CLex::Scanner::Comma)
                          throw source.unexpected();
                      }
                  }

                bool isconst = false;
                if (source.next() == CLex::Scanner::Name)
                  {
                    if (not source.expect( "const", &CLex::Scanner::get_name ))
                      throw source.unexpected();
                    isconst = true;
                    source.next();
                  }
                
                if (source.lnext != CLex::Scanner::ObjectOpening)
                  throw source.unexpected();

                SourceCode* default_sourcecode = new SourceCode(GetSourceCode(source));
                ActionProto* ap = new ActionProto( ActionProto::Common, symbol, returns, param_list, isconst, default_sourcecode, comments, symfl );
                comments.clear();
                _isa.m_actionprotos.push_back( ap );
                source.next();
              }
            else if (std::equal(cmd.begin(), cmd.end(), "include"))
              {
                if (source.next() != CLex::Scanner::StringQuotes)
                  throw source.unexpected();
                
                std::string buf;
                source.get(buf, &CLex::Scanner::get_string);
                
                ConstStr filename = Scanner::locate( buf.c_str() );

                if (not parse( filename.str(), _isa ))
                  {
                    std::cerr << source.loc() << ": parse error.\n";
                    return false;
                  }
                
                source.next();
              }
            else if (std::equal(cmd.begin(), cmd.end(), "namespace"))
              {
                for (;;)
                  {
                    if (source.next() != CLex::Scanner::Name)
                      throw source.unexpected();
                    std::string cmd;
                    source.get(cmd, &CLex::Scanner::get_name);
                    _isa.m_namespace.push_back( ConstStr( cmd.c_str(), Scanner::symbols ) );
                    if (source.next() != CLex::Scanner::Colon)
                      break;
                    if (source.getchar() != ':')
                      throw source.unexpected();
                  }
                
              }
            else if (std::equal(cmd.begin(), cmd.end(), "set"))
              {
                if (source.next() != CLex::Scanner::Name)
                  throw source.unexpected();

                ConstStr key;
                {
                  CLex::BlocSink<16> ident;
                  if (not source.get_name(ident))
                    { std::cerr << "error: too long identifier\n";  throw source.unexpected(); }
                  ident.append('\0');
                  key = ConstStr(ident.buffer, Scanner::symbols);
                }

                switch (source.next())
                  {
                  default:
                    std::cerr << "error: not a set-value\n";
                    throw source.unexpected();

                  case CLex::Scanner::Name:
                    {
                      std::string value;
                      source.get(value, &CLex::Scanner::get_name);
                      _isa.setparam( key, ConstStr(value.c_str(), Scanner::symbols) );
                    }
                    break;

                  case CLex::Scanner::ObjectOpening:
                    _isa.setparam( key, GetSourceCode(source) );
                    break;
                  }
                source.next();
              }
            else if (std::equal(cmd.begin(), cmd.end(), "template"))
              {
                if (source.next() != CLex::Scanner::Less)
                  throw source.unexpected();
                for (;;)
                  {
                    if (source.next() != CLex::Scanner::ObjectOpening)
                      throw source.unexpected();
                    SourceCode&& tp = GetSourceCode(source);
                    if (source.next() != CLex::Scanner::ObjectOpening)
                      throw source.unexpected();
                    SourceCode&& nm = GetSourceCode(source);
                    _isa.m_tparams.append( new CodePair( new SourceCode(std::move(tp)), new SourceCode(std::move(nm)) ) );
                    if (source.next() == CLex::Scanner::More)
                      break;
                    if (source.lnext != CLex::Scanner::Comma)
                      throw source.unexpected();
                  }
                source.next();
              }
            else if (std::equal(cmd.begin(), cmd.end(), "op"))
              {
                _isa.add(GetOp(source, _isa));
                source.next();
              }
            else if (std::equal(cmd.begin(), cmd.end(), "specialize"))
              {
                if (source.next() != CLex::Scanner::Name)
                  throw source.unexpected();

                Specialization* spec = 0;
                FileLoc && symfl = GetFileLoc(source);
                {
                  std::string buf;
                  source.get(buf, &CLex::Scanner::get_name);
                  if (Operation* operation = _isa.operation( ConstStr(buf.c_str(), Scanner::symbols) ))
                    {
                      Vector<Constraint> none;
                      spec = new Specialization(operation, none);
                    }
                  else
                    {
                      symfl.err( "error: operation `%s' not defined", buf.c_str() );
                      throw CLex::Scanner::Unexpected();
                    }
                }
                
                if (source.next() != CLex::Scanner::GroupOpening)
                  throw source.unexpected();
                
                for (;;)
                  {
                    if (source.next() != CLex::Scanner::Name)
                      throw source.unexpected();
                    std::string buf;
                    source.get(buf, &CLex::Scanner::get_name);
                    
                    if (source.next() != CLex::Scanner::Assign)
                      throw source.unexpected();

                    if (source.next() != CLex::Scanner::Number)
                      throw source.unexpected();
                    unsigned value = GetInteger(source);
                    
                    spec->m_constraints.push_back( new Constraint( ConstStr(buf.c_str(), Scanner::symbols), value ) );
                    
                    if (source.next() != CLex::Scanner::Comma)
                      break;
                  }
                    
                if (source.lnext != CLex::Scanner::GroupClosing)
                  throw source.unexpected();

                _isa.m_specializations.push_back( spec );
                
                source.next();
              }
            else if (std::equal(cmd.begin(), cmd.end(), "var"))
              {
                Vector<Variable> var_list;
                GetVarList(source, var_list);
                _isa.m_vars.append( var_list );
              }
            else if (std::equal(cmd.begin(), cmd.end(), "group"))
              {
                if (source.next() != CLex::Scanner::Name)
                  throw source.unexpected();
                ConstStr symbol;
                FileLoc && symfl = GetFileLoc(source);
                {
                  std::string buf;
                  source.get(buf, &CLex::Scanner::get_name);
                  symbol = ConstStr(buf.c_str(), Scanner::symbols);
                }

                source.next();
                if (source.lnext == CLex::Scanner::Name)
                  {
                    CLex::BlocSink<16> grpcmd;
                    if (not source.get_name(grpcmd))
                      throw source.unexpected();
                    grpcmd.append('\0');
                    _isa.group_command( symbol, ConstStr(grpcmd.buffer,Scanner::symbols), symfl );
                  }
                else if (source.lnext == CLex::Scanner::GroupOpening)
                  {
                    do {
                      FileLoc def; char const* err = 0;
                      if  (Operation* prev_op = _isa.operation( symbol ))
                        { err = "operation"; def = prev_op->fileloc; }
                      else if (Group* prev_gr = _isa.group( symbol ))
                        { err =     "group"; def = prev_gr->fileloc; }
                      else
                        break;
                      
                      symfl.err( "error: %s name conflicts with %s `%s'", err, err, symbol.str() );
                      def.err( "%s `%s' previously defined here", err, symbol.str() );
                      throw CLex::Scanner::Unexpected();
                      
                    } while (0);
                    
                    struct : Isa::OOG
                    {
                      void with( Operation& operation )
                      {
                        if (opsyms.insert(operation.symbol).second)
                          group->operations.append( &operation );
                      }
                      std::set<ConstStr> opsyms;
                      Group* group;
                    } oog;
                    oog.group = new Group( symbol, symfl );
                    
                    for (;;)
                      {
                        if (source.next() != CLex::Scanner::Name)
                          throw source.unexpected();
                        FileLoc&& errfl = GetFileLoc(source);
                        std::string buf;
                        source.get(buf, &CLex::Scanner::get_name);
                        if (not _isa.for_ops(ConstStr(buf,Scanner::symbols), oog))
                          {
                            errfl.err( "error: undefined operation or group `%s'", buf.c_str() );
                            throw CLex::Scanner::Unexpected();
                          }
                        if (source.next() != CLex::Scanner::Comma)
                          break;
                      }
                    
                    if (source.lnext != CLex::Scanner::GroupClosing)
                      throw source.unexpected();
                    
                    _isa.m_groups.push_back( oog.group );
                  }
                else
                  throw source.unexpected();

                source.next();
              }
            else
              {
                ConstStr symbol(cmd.c_str(), Scanner::symbols);
                if (source.next() != CLex::Scanner::Dot)
                  throw source.unexpected();
                ConstStr attribute;
                if (source.next() != CLex::Scanner::Name)
                  throw source.unexpected();
                FileLoc && attrfl = GetFileLoc(source);
                {
                  std::string buf;
                  source.get(buf, &CLex::Scanner::get_name);
                  attribute = ConstStr(buf.c_str(), Scanner::symbols);
                }

                static ConstStr   var( "var",        Scanner::symbols );
                static ConstStr   spe( "specialize", Scanner::symbols );
                
                if (attribute == var)
                  {
                    struct : public Isa::OOG
                    {
                      void with( Operation& operation ) override { operation.variables.append(var_list); }
                      Vector<Variable> var_list;
                    } oog;
                    
                    GetVarList(source, oog.var_list);

                    if (not _isa.for_ops( symbol, oog ))
                      {
                        cmdfl.err( "error: undefined operation or group `%s'", symbol.str() );
                        throw CLex::Scanner::Unexpected();
                      }
                  }
                else if (attribute == spe)
                  {
                    if (source.next() != CLex::Scanner::GroupOpening)
                      throw source.unexpected();
                    
                    ConstStr symbol(cmd.c_str(), Scanner::symbols);
                    _isa.m_user_orderings.push_back( Isa::Ordering() );
                    Isa::Ordering& order = _isa.m_user_orderings.back();
                    order.fileloc = cmdfl;
                    order.top_op = symbol;

                    for (;;)
                      {
                        if (source.next() != CLex::Scanner::Name)
                          throw source.unexpected();
                        std::string buf;
                        source.get(buf, &CLex::Scanner::get_name);
                        order.under_ops.push_back( ConstStr(buf.c_str(), Scanner::symbols) );
                        
                        if (source.next() != CLex::Scanner::Comma)
                          break;
                      }
                    
                    if (source.lnext != CLex::Scanner::GroupClosing)
                      throw source.unexpected();
                    
                    source.next();
                  }
                else
                  {
                    if (source.next() != CLex::Scanner::Assign)
                      throw source.unexpected();
                    
                    struct : Isa::OOG
                    {
                      SourceCode* actioncode;
                      ActionProto const* actionproto;
                      FileLoc actloc;
                      void with( Operation& operation ) override
                      {
                        if (Action const* prev_action = operation.action( actionproto ))
                          {
                            actloc.err( "error: action `%s.%s' redefined", operation.symbol.str(), actionproto->m_symbol.str() );
                            prev_action->m_fileloc.err( "action `%s.%s' previously defined here", operation.symbol.str(), actionproto->m_symbol.str() );
                            exit( -1 );
                          }
                        
                        operation.add( new Action( actionproto, actioncode, Scanner::comments, actloc ) );
                      }
                    } oog;
                    
                    if (source.next() != CLex::Scanner::ObjectOpening)
                      throw source.unexpected();
                    oog.actioncode = new SourceCode(GetSourceCode(source));
                    oog.actloc = attrfl;
                    /* Action must belong to an action prototype */
                    if (not (oog.actionproto = _isa.actionproto( attribute )))
                      {
                        cmdfl.err( "error: undefined action prototype `%s'", attribute.str() );
                        throw CLex::Scanner::Unexpected();
                      }

                    if (not _isa.for_ops( symbol, oog ))
                      {
                        cmdfl.err( "error: undefined operation or group `%s'", symbol.str() );
                        throw CLex::Scanner::Unexpected();
                      }
                    source.next();
                  }
              }
          
          }
          break;

        case CLex::Scanner::ObjectOpening:
          {
            Ptr<SourceCode> sc = new SourceCode(GetSourceCode(source));
            if (source.next() == CLex::Scanner::Colon)
              {
                if (source.next() != CLex::Scanner::Name or not source.expect("op", &CLex::Scanner::get_name))
                  throw source.unexpected();
                auto op = GetOp(source, _isa);
                op->condition = sc;
                _isa.add(op);
              }
            else
              _isa.m_decl_srccodes.push_back(sc);
            source.next();
          }
          break;

        case CLex::Scanner::SemiColon:
          source.next();
          break;
      
        default:
          throw source.unexpected();
        }
    }
  
  return true;
}

// Scanner::Token Scanner::s_tokens[] = { {0,0} };
// int Scanner::token( char const* _text ) { throw "impossible"; return 0; }
// ConstStr Scanner::charname( char _char ) { return Str::fmt( "'%c'", _char ); }
// ConstStr Scanner::tokenname( int _token ) { throw "impossible"; return 0; }


void
Scanner::add_lookupdir( char const* _dir )
{
#ifdef WIN32
  if ((((_dir[0] >= 'a' && _dir[0] <= 'z') || (_dir[0] >= 'A' && _dir[0] <= 'Z')) && (_dir[1] == ':') && ((_dir[2] == '\\') || (_dir[2] == '/'))) || (*_dir == '/'))
  {
     // convert '\' into '/' to have a UNIX friendly path as gcc doesn't like '\' in filenames in #line directives
     int len = strlen(_dir);
     char cv_dir[len + 1];
     const char *pch;
     char *cv_pch;
     for(pch = _dir, cv_pch = cv_dir; *pch; pch++, cv_pch++)
     {
        if(*pch == '\\') *cv_pch = '/'; else *cv_pch = *pch;
     }
     cv_dir[len] = 0;
     s_lookupdirs.push_back( cv_dir );
  }
#else
  if (*_dir == '/')
  {
    s_lookupdirs.push_back( _dir );
    return;
  }
#endif
  
  std::string buffer;
  for (intptr_t capacity = 128; true; capacity *= 2) {
    char storage[capacity];
    if (not getcwd( storage, capacity )) {
      if (errno != ERANGE) throw CWDError;
      continue; 
    }
#ifdef WIN32
    assert((((storage[0] >= 'a' && storage[0] <= 'z') || (storage[0] >= 'A' && storage[0] <= 'Z')) && (storage[1] == ':') && ((storage[2] == '\\') || (storage[2] == '/'))) || (*storage == '/'));
    // convert '\' into '/' to have a UNIX friendly path as gcc doesn't like '\' in filenames in #line directives
    {
      char *pch;
      for(pch = storage; *pch; pch++)
      {
         if(*pch == '\\') *pch = '/';
      }
    }
    int len = strlen(_dir);
    char cv_dir[len + 1];
    {
      const char *pch;
      char *cv_pch;
      for(pch = _dir, cv_pch = cv_dir; *pch; pch++, cv_pch++)
      {
         if(*pch == '\\') *cv_pch = '/'; else *cv_pch = *pch;
      }
      cv_dir[len] = 0;
    }
    buffer = buffer + storage + "/" + cv_dir;
#else
    assert( storage[0] == '/' ); // a directory path does not start with '/' on a windows host !
    buffer = buffer + storage + "/" + _dir;
#endif
    s_lookupdirs.push_back( buffer.c_str() );
    break;
  }
  
}

ConstStr
Scanner::locate( char const* _name )
{
  for (std::vector<ConstStr>::iterator iter = s_lookupdirs.begin(); iter != s_lookupdirs.end(); iter++)
    {
      std::string buffer = std::string() + iter->str() + "/" + _name;
      if (access( buffer.c_str(), R_OK ) != 0) continue;
      return buffer.c_str();
    }
  return _name;
}

int yywrap() { return 1; }

