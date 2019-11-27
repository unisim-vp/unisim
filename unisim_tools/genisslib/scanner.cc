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
#include <parser_tokens.hh>
#include <strtools.hh>
#include <clex.hh>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <limits>
#include <cstring>
#include <cassert>
#include <unistd.h>

bool                     Scanner::aborted_scanning = false;
FileLoc                  Scanner::fileloc;
FileLoc                  Scanner::fileloc_mlt;
int                      Scanner::bracecount = 0;
std::vector<int>         Scanner::scs;
Vector<Comment>          Scanner::comments;
Isa*                     Scanner::s_isa = 0;
ConstStr::Pool           Scanner::symbols;
std::vector<ConstStr>    Scanner::s_lookupdirs;

static
void
create_action( Operation* _operation, ActionProto const* _actionproto, SourceCode* _actioncode )
{
  Action const* prev_action = _operation->action( _actionproto );

  if (prev_action) {
    Scanner::fileloc.err( "error: action `%s.%s' redefined",
                          _operation->symbol.str(), _actionproto->m_symbol.str() );
    
    prev_action->m_fileloc.err( "action `%s.%s' previously defined here",
                                _operation->symbol.str(), _actionproto->m_symbol.str() );
    exit( -1 );
  }
  
  _operation->add( new Action( _actionproto, _actioncode, Scanner::comments, Scanner::fileloc ) );
}

Scanner::Inclusion* Scanner::include_stack = 0;

std::string& Scanner::strbuf() { static std::string s_buffer; return s_buffer; }

void
Scanner::push()
{
  throw 0;
  // YY_BUFFER_STATE state = YY_CURRENT_BUFFER;
  
  include_stack = new Inclusion( 0, 0, fileloc, include_stack );
}

bool
Scanner::pop()
{
  Inclusion* tail = include_stack;
  
  if (not tail) return false;

  throw 0;
  // YY_BUFFER_STATE state;
  
  // tail->restore( &state, sizeof( YY_BUFFER_STATE ) );
  // yy_delete_buffer( YY_CURRENT_BUFFER );
  // if (yyin) fclose( yyin );
  //   yy_switch_to_buffer( state );
  
  fileloc = tail->fileloc;
  
  include_stack = tail->next;
  tail->next = 0;
  delete tail;
  
  return true;
}

Scanner::Inclusion::Inclusion( uint8_t const* _state, intptr_t _size, FileLoc const& _fileloc, Inclusion* _next )
  : state_backup( 0 ), fileloc( _fileloc ), next( _next )
{
  state_backup = new uint8_t[_size];
  std::copy(&_state[0], &_state[_size], &state_backup[0]);
  //  memcpy( state_backup, _state, _size );
}

Scanner::Inclusion::~Inclusion()
{
  delete [] state_backup;
}

void
Scanner::Inclusion::restore( uint8_t* _state, intptr_t _size )
{
  std::copy(&state_backup[0], &state_backup[_size], _state);
  //  memcpy( _state, state_backup, _size );
}

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
  s_isa = &_isa;

  std::cerr << "Opening " << _filename << '\n';
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
                    if (Operation* prev_op = Scanner::isa().operation( symbol ))
                      {
                        symfl.err( "error: group name conflicts with operation `%s'", symbol.str() );
                        prev_op->fileloc.err( "operation `%s' previously defined here", symbol.str() );
                        throw CLex::Scanner::Unexpected();
                      }
                    if (Group* prev_grp = Scanner::isa().group( symbol ))
                      {
                        symfl.err( "error: group name conflicts with group `%s'", symbol.str() );
                        prev_grp->fileloc.err( "group `%s' previously defined here", symbol.str() );
                        throw CLex::Scanner::Unexpected();
                      }
                    
                    Group* res = new Group( symbol, symfl );

                    for (;;)
                      {
                        if (source.next() != CLex::Scanner::Name)
                          throw source.unexpected();
                        std::string buf;
                        FileLoc && errfl = GetFileLoc(source);
                        source.get(buf, &CLex::Scanner::get_name);
                        if (not _isa.operations( ConstStr(buf,Scanner::symbols), res->operations ))
                          {
                            errfl.err( "error: undefined operation or group `%s'", buf.c_str() );
                            throw CLex::Scanner::Unexpected();
                          }
                        if (source.next() != CLex::Scanner::Comma)
                          break;
                      }
                    
                    if (source.lnext != CLex::Scanner::GroupClosing)
                      throw source.unexpected();
                    
                    _isa.m_groups.push_back( res );
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
                {
                  std::string buf;
                  source.get(buf, &CLex::Scanner::get_name);
                  attribute = ConstStr(buf.c_str(), Scanner::symbols);
                }

                static ConstStr   var( "var",        Scanner::symbols );
                static ConstStr   spe( "specialize", Scanner::symbols );
                
                if (attribute == var)
                  {
                    Vector<Variable> var_list;

                    GetVarList(source, var_list);
                    
                    if (Operation* operation = Scanner::isa().operation( symbol ))
                      {
                        operation->variables.append( var_list );
                      }
                    else if (Group* group = Scanner::isa().group( symbol ))
                      {
                        for (Vector<Operation>::iterator gop = group->operations.begin(); gop < group->operations.end(); ++ gop)
                          (**gop).variables.append( var_list );
                      }
                    else
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
                    Scanner::isa().m_user_orderings.push_back( Isa::Ordering() );
                    Isa::Ordering& order = Scanner::isa().m_user_orderings.back();
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
                    if (source.next() != CLex::Scanner::ObjectOpening)
                      throw source.unexpected();
                    SourceCode* actioncode = new SourceCode(GetSourceCode(source));
                    /* Actions belongs to an action prototype */
                    ActionProto const* actionproto = Scanner::isa().actionproto( attribute );

                    if (not actionproto)
                      {
                        cmdfl.err( "error: undefined action prototype `%s'", attribute.str() );
                        throw CLex::Scanner::Unexpected();
                      }

                    if (Operation* operation = Scanner::isa().operation( symbol ))
                      {
                        create_action( operation, actionproto, actioncode );
                      }
                    else if (Group* group = Scanner::isa().group( symbol ))
                      {
                        for (Vector<Operation>::iterator gop = group->operations.begin(); gop < group->operations.end(); ++ gop)
                          create_action( *gop, actionproto, actioncode );
                      }
                    else
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
  //   if (not Scanner::open( ConstStr( _filename) ) )
  //     return false;
  //   bracecount = 0;
  //   scs.clear();
  
  // #if 0
  //   // This code is only for testing the lexical analyzer
  //   int c;
    
  //   while( (c = yylex()) != 0 ) {
  //     printf( "%s\n", get_token_name(c) );
  //   }
  // #endif
  
  //   aborted_scanning = false;
  //   int error = yyparse();

  //   throw 0;
  //   // if (yyin) {
  //   //   fclose( yyin );
  //   //   yyin = 0;
  //   // }
  
  //   //  yylex_destroy();
  //   return (error == 0) && !aborted_scanning;
  return true;
}

bool
Scanner::open( ConstStr _filename )
{
  fprintf( stderr, "Opening %s\n", _filename.str() );
  throw 0;
  // yyin = fopen( _filename.str(), "r" );
  
  // if (not yyin) {
  //   fileloc.err( "error: can't open file `%s'", _filename.str() );
  //   return false;
  // }
  
  isa().m_includes.push_back( _filename );
  fileloc.assign( _filename, 1, 1 );
  return true;
}

bool
Scanner::include( char const* _filename )
{
  ConstStr filename = Scanner::locate( _filename );
  
  Scanner::push();

  if (not Scanner::open( filename))
    return false;

  throw 0;
  // yy_switch_to_buffer( yy_create_buffer( yyin, YY_BUF_SIZE ) );
  // yylineno = 1;
  return true;
}

Scanner::Token Scanner::s_tokens[] = {
  {"action", TOK_ACTION},
  {"const", TOK_CONST},
  {"constructor", TOK_CONSTRUCTOR},
  {"decl", TOK_DECL},
  {"decoder", TOK_DECODER},
  {"destructor", TOK_DESTRUCTOR},
  {"group", TOK_GROUP},
  {"impl", TOK_IMPL},
  {"include", TOK_INCLUDE},
  {"inheritance", TOK_INHERITANCE},
  {"namespace", TOK_NAMESPACE},
  {"op", TOK_OP},
  {"rewind", TOK_REWIND},
  {"set", TOK_SET},
  {"sext", TOK_SEXT},
  {"shl", TOK_SHL},
  {"shr", TOK_SHR},
  {"specialize", TOK_SPECIALIZE},
  {"static", TOK_STATIC},
  {"subdecoder", TOK_SUBDECODER},
  {"template", TOK_TEMPLATE},
  {"var", TOK_VAR},
  {0,0}
};

/** Convert a name to a token
    @param name name of the token
    @return the token
*/
int
Scanner::token( char const* _text )
{
  for (int idx = 0; s_tokens[idx].name; ++ idx)
    if (strcmp( s_tokens[idx].name, _text) == 0 )
      return s_tokens[idx].token;

  return TOK_IDENT;
}


ConstStr
Scanner::charname( char _char )
{
  switch( _char ) {
  case '\t': return "tab char";
  case '\n': return "return char";
  case '\r': return "carriage return";
  case '\b': return "back space";
  default: break;
  }
  
  if (_char < 32 or _char >= 126)
    return Str::fmt( "char #%d", _char );

  return Str::fmt( "'%c'", _char );
}

/** Return the name of a token
    @param token the token
    @return the name of the token
*/
ConstStr
Scanner::tokenname( int _token )
{
  /* search into the token table */
  for (int idx = 0; s_tokens[idx].name; ++ idx)
    if (s_tokens[idx].token == _token)
      return s_tokens[idx].name;
  
  /* return a string for the token not in the token table */
  switch( _token ) {
  case TOK_IDENT: return "identifier";
  case TOK_ENDL: return "end of line";
  case TOK_SOURCE_CODE: return "source code";
  case TOK_INTEGER: return "integer";
  case TOK_STRING: return "string";
  case '(': return "`('";
  case ')': return "`)'";
  case '<': return "`<'";
  case '>': return "`>'";
  case ':' : return "`:'";
  case '=' : return "`='";
  case '.': return "`.'";
  case '[': return "`['";
  case ']': return "`]'";
  case '?': return "`?'";
  }

  /* return a string for a character token */
  if (_token < 256)
    return charname( _token );
  
  /* don't know which token it is ! */
  return "unknown token";
}

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

// void
// Scanner::sc_enter( int _condition )
// {
//   int sc = YY_START;
//   if (sc == INITIAL) {
//     fileloc_mlt = fileloc;
//     strbuf().clear();
//   }
//   if (_condition == source_code_context) {
//     if (sc == source_code_context) { bracecount += 1; return; }
//     else if (sc == INITIAL) bracecount = 1;
//     else assert( false );
//   }
//   scs.push_back( sc );
//   BEGIN( _condition );
// }

// bool
// Scanner::sc_leave()
// {
//   int oldsc = YY_START;
//   int newsc = Scanner::scs.back();
  
//   if (oldsc == source_code_context and ((--bracecount) > 0)) {
//     newsc = source_code_context;
//   } else {
//     Scanner::scs.pop_back();
//     BEGIN( newsc );
//   }

//   if (newsc == INITIAL) return true;
//   strbuf().append( yytext, yyleng );
//   return false;
// }

ConstStr
Scanner::all_operations()
{
  static ConstStr all_ops( "all_operations", symbols );
  
  return all_ops;
}

int yywrap() { return 1; }

