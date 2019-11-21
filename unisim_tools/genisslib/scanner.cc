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
#include <parser_tokens.hh>
#include <strtools.hh>
#include <clex.hh>
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

void parse_binary_number( char const* s, int length, unsigned int *value );

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
  SourceCode GetSourceCode(CLex::Scanner& src)
  {
    FileLoc fl(src.filename, src.lidx, src.cidx );
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
          case InEsc:
            state = InString;
            break;
          }
        if (depth < 0) break;
        buf += ch;
      }

    return SourceCode( buf.c_str(), fl );
  }
}

bool
Scanner::parse( char const* _filename, Isa& _isa )
{
  s_isa = &_isa;
  
  CLex::Scanner source(_filename);

  source.next();

  while (source.lnext != CLex::Scanner::EoF)
    {
      switch (source.lnext)
        {
        case CLex::Scanner::Name:
          {
            CLex::BlocSink<16> cmd;
            if (not source.get_name(cmd))
              { std::cerr << "error: too long cmdifier\n";  throw source.unexpected(); }
        
            if (std::equal(cmd.begin(), cmd.end(), "decl") or
                std::equal(cmd.begin(), cmd.end(), "impl"))
              {
                auto& member = (cmd.buffer[0] == 'd') ? _isa.m_decl_srccodes : _isa.m_impl_srccodes;
                member.push_back( new SourceCode(GetSourceCode(source)) );
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
            else
              {
                cmd.append('\0');
                std::cerr << "error: unknown directive '" << cmd.buffer << "'\n";
                throw source.unexpected();
              }
          
          } break;
      
      
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

void
parse_binary_number( char const* binstr, int length, unsigned int *value )
{
  unsigned int res = 0;
  unsigned int mask = 1;
  for (char const* ptr = binstr + length; (--ptr) >= binstr and *ptr != 'b';) {
    if (*ptr != '0') res |= mask;
    mask <<= 1;
  }
  *value = res;
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

