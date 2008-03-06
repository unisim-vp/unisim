%{ /* -*- C++ -*- */
/***************************************************************************
                      scanner.lex  -  lexical analyzer
                             -------------------
    begin                : Thu May 25 2003
    copyright            : (C) 2003 CEA and Universite Paris Sud
    author               : Gilles Mouchard
    email                : mouchard@lri.fr
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <yystype.hh>
#include <parser.hh>
#include <scanner.hh>
#include <strtools.hh>
#include <sourcecode.hh>
#include <comment.hh>
#include <iostream>
#include <cerrno>

using namespace std;

FileLoc_t                Scanner::fileloc;
FileLoc_t                Scanner::fileloc_mlt;
int                      Scanner::bracecount = 0;
std::vector<int>         Scanner::scs;
Vect_t<Comment_t>        Scanner::comments;
Isa*                     Scanner::s_isa = 0;
Str::Buf*                Scanner::s_stringbuffer;
ConstStr_t::Set          Scanner::symbols;
std::vector<ConstStr_t>  Scanner::s_lookupdirs;

void parse_binary_number( char const* s, int length, unsigned int *value );

/* This is for Flex < 2.5.9 (where yylex_destroy is not defined) */
#if !defined(YY_FLEX_MAJOR_VERSION) || YY_FLEX_MAJOR_VERSION < 2 \
    || (YY_FLEX_MAJOR_VERSION == 2 \
        && (!defined(YY_FLEX_MINOR_VERSION) || YY_FLEX_MINOR_VERSION < 5 \
            || (YY_FLEX_MINOR_VERSION == 5 \
                && (!defined(YY_FLEX_SUBMINOR_VERSION) \
                    || YY_FLEX_SUBMINOR_VERSION < 9))))
# define yylex_destroy() yy_delete_buffer (YY_CURRENT_BUFFER)
#endif

%}

identifier [a-zA-Z_][a-zA-Z0-9_]*
binary_number 0b[0-1]+
hexadecimal_number 0x[0-9a-fA-F]+
decimal_number [0-9]+

%x string_context
%x source_code_context
%x c_like_comment_context
%x cpp_like_comment_context
%x char_context

%%

\" { Scanner::sc_enter( string_context ); }
<string_context>[^\"\n] { Scanner::strbuf().write( yyleng, yytext ); }
<string_context>\n { Scanner::strbuf().write( yyleng, yytext ); Scanner::fileloc.m_line++; }
<string_context>\\\" { Scanner::strbuf().write( yyleng, yytext ); }
<string_context>\" { if( Scanner::sc_leave() ) { yylval.volatile_string = Scanner::strbuf().m_storage; return TOK_STRING; }
}

\{ { Scanner::sc_enter( source_code_context ); }
<source_code_context>[^/\}\{\"\n\'] {  Scanner::strbuf().write( yyleng, yytext ); }
<source_code_context>"/" { Scanner::strbuf().write( yyleng, yytext ); }
<source_code_context>\' { Scanner::sc_enter( char_context ); Scanner::strbuf().write( yyleng, yytext ); }
<source_code_context>\{ { Scanner::bracecount++; Scanner::strbuf().write( yyleng, yytext ); }
<source_code_context>\" { Scanner::sc_enter( string_context ); Scanner::strbuf().write( yyleng, yytext ); }
<source_code_context>\n { Scanner::strbuf().write( yyleng, yytext ); Scanner::fileloc.m_line++; }
<source_code_context>\} {
  if( Scanner::sc_leave() ) {
    yylval.sourcecode = new SourceCode_t( Scanner::strbuf().m_storage, Scanner::fileloc_mlt );
    return TOK_SOURCE_CODE;
  }
}

<char_context>[^\'\n] { Scanner::strbuf().write( yyleng, yytext ); }
<char_context>\n { Scanner::strbuf().write( yyleng, yytext ); Scanner::fileloc.m_line++; }
<char_context>\\\' { Scanner::strbuf().write( yyleng, yytext ); }
<char_context>\' { Scanner::sc_leave(); }

<INITIAL,source_code_context>"/*" { Scanner::sc_enter( c_like_comment_context ); Scanner::strbuf().write( yyleng, yytext ); }
<c_like_comment_context>[^*\n] { Scanner::strbuf().write( yyleng, yytext ); }
<c_like_comment_context>"*"+[^/\n] { Scanner::strbuf().write( yyleng, yytext ); }
<c_like_comment_context>"*"+\n { Scanner::strbuf().write( yyleng, yytext ); Scanner::fileloc.m_line++; }
<c_like_comment_context>\n { Scanner::strbuf().write( yyleng, yytext ); Scanner::fileloc.m_line++; }
<c_like_comment_context>"*"+"/" {
  if( Scanner::sc_leave() ) {
    Scanner::strbuf().write( yyleng, yytext );
    Scanner::comments.append( new Comment_t( Scanner::strbuf().m_storage, Scanner::fileloc_mlt ) );
  }
}

<INITIAL,source_code_context>"//" { Scanner::sc_enter( cpp_like_comment_context ); Scanner::strbuf().write( yyleng, yytext ); }
<cpp_like_comment_context>[^\n] { Scanner::strbuf().write( yyleng, yytext ); }
<cpp_like_comment_context>\n {
  if( Scanner::sc_leave() ) {
    Scanner::comments.append( new Comment_t( Scanner::strbuf().m_storage, Scanner::fileloc_mlt ) );
  }
  Scanner::fileloc.m_line++;
}

{binary_number} { parse_binary_number( yytext, yyleng, &yylval.uinteger ); return TOK_INTEGER; }
{hexadecimal_number} { sscanf( yytext, "%x", &yylval.uinteger ); return TOK_INTEGER; }
{decimal_number} { sscanf(yytext, "%u", &yylval.uinteger); return TOK_INTEGER; }
{identifier} {
  int token = Scanner::token( yytext );
  if( token == TOK_IDENT )
    yylval.persistent_string = ConstStr_t( yytext, Scanner::symbols );
  return token;
}
\\\n { Scanner::fileloc.m_line++; }
\n { Scanner::fileloc.m_line++; return TOK_ENDL; }
"*" { return '*'; }
"." { return '.'; }
"[" { return '['; }
"]" { return ']'; }
"(" { return '('; }
")" { return ')'; }
"<"  { return '<'; }
">"  { return '>'; }
"," { return ','; }
":" { return ':'; }
";" { return ';'; }
"=" { return '='; }
"-" { return '-'; }
"?" { return '?'; }
"::" { return TOK_QUAD_DOT; }
" " { }
\t { }
<INITIAL><<EOF>> { if( not Scanner::pop() ) yyterminate(); }
<string_context,char_context,c_like_comment_context,cpp_like_comment_context,source_code_context><<EOF>> {
  Scanner::fileloc.err( "error: unexpected end of file" );
  yyterminate();
  return 0;
}
<INITIAL,string_context,char_context,c_like_comment_context,cpp_like_comment_context,source_code_context>. {
  Scanner::fileloc.err( "error: unexpected %s", Scanner::charname( yytext[0] ).str() );
  yyterminate();
  return 0;
}

%%

#include <conststr.hh>
#include <cstring>

#include <cassert>
#include <isa.hh>

Scanner::Include_t* Scanner::include_stack = 0;

void
Scanner::push() {
  YY_BUFFER_STATE state = YY_CURRENT_BUFFER;
  
  include_stack = new Include_t( &state, sizeof( YY_BUFFER_STATE ), fileloc, include_stack );
}

bool
Scanner::pop() {
  Include_t* tail = include_stack;
  
  if( not tail ) return false;
  YY_BUFFER_STATE state;
  tail->restore( &state, sizeof( YY_BUFFER_STATE ) );
  yy_delete_buffer( YY_CURRENT_BUFFER );
  if( yyin ) fclose( yyin );
  yy_switch_to_buffer( state );
  
  fileloc = tail->m_fileloc;
  
  include_stack = tail->m_next;
  tail->m_next = 0;
  delete tail;
  
  return true;
}

Scanner::Include_t::Include_t( void const* _state, intptr_t _size, FileLoc_t const& _fileloc, Include_t* _next )
  : m_state_backup( 0 ), m_fileloc( _fileloc ), m_next( _next )
{
  m_state_backup = new uint8_t[_size];
  memcpy( m_state_backup, _state, _size );
}

Scanner::Include_t::~Include_t() {
  delete [] m_state_backup;
}

void
Scanner::Include_t::restore( void* _state, intptr_t _size ) {
  memcpy( _state, m_state_backup, _size );
}

bool
Scanner::parse( char const* _filename, Isa& _isa ) {
  s_isa = &_isa;
  if( not Scanner::open( ConstStr_t( _filename ) ) )
    return false;
  bracecount = 0;
  scs.clear();
  Str::Buf buffer( Str::Buf::Recycle );
  s_stringbuffer = &buffer;
  
#if 0
  // This code is only for testing the lexical analyzer
  int c;
    
  while( (c = yylex()) != 0 ) {
    printf( "%s\n", get_token_name(c) );
  }
#endif
  
  int error = yyparse();
  
  if( yyin ) {
    fclose( yyin );
    yyin = 0;
  }
  
  yylex_destroy();
  return (error == 0);
}

bool
Scanner::open( ConstStr_t _filename ) {
  fprintf( stderr, "Opening %s\n", _filename.str() );
  yyin = fopen( _filename, "r" );

  if( not yyin ) {
    fileloc.err( "error: can't open file `%s'", _filename.str() );
    return false;
  }
  
  isa().m_includes.push_back( _filename );
  fileloc.assign( _filename, 1 );
  return true;
}

bool
Scanner::include( char const* _filename ) {
  ConstStr_t filename = Scanner::locate( _filename );
  
  Scanner::push();

  if( not Scanner::open( filename ))
    return false;

  yy_switch_to_buffer( yy_create_buffer( yyin, YY_BUF_SIZE ) );
  yylineno = 1;
  return true;
}

void
parse_binary_number( char const* binstr, int length, unsigned int *value ) {
  unsigned int res = 0;
  unsigned int mask = 1;
  for( char const* ptr = binstr + length; (--ptr) >= binstr and *ptr != 'b'; ) {
    if( *ptr != '0' ) res |= mask;
    mask <<= 1;
  }
  *value = res;
}

Scanner::Token_t Scanner::s_tokens[] = {
  {"namespace", TOK_NAMESPACE},
  {"address", TOK_ADDRESS},
  {"subdecoder", TOK_SUBDECODER},
  {"decl", TOK_DECL},
  {"impl", TOK_IMPL},
  {"op", TOK_OP},
  {"group", TOK_GROUP},
  {"action", TOK_ACTION},
  {"static", TOK_STATIC},
  {"constructor", TOK_CONSTRUCTOR},
  {"destructor", TOK_DESTRUCTOR},
  {"include", TOK_INCLUDE},
  {"sext", TOK_SEXT},
  {"shr", TOK_SHR},
  {"shl", TOK_SHL},
  {"rewind", TOK_REWIND},
  {"var", TOK_VAR},
  {"little_endian", TOK_LITTLE_ENDIAN},
  {"big_endian", TOK_BIG_ENDIAN},
  {"template", TOK_TEMPLATE},
  {"decoder", TOK_DECODER},
  {"specialize", TOK_SPECIALIZE},
  {0,0}
};

/** Convert a name to a token
    @param name name of the token
    @return the token
*/
int
Scanner::token( char const* _text ) {
  for( int idx = 0; s_tokens[idx].m_name; ++ idx )
    if( strcmp( s_tokens[idx].m_name, _text ) == 0 )
      return s_tokens[idx].m_token;

  return TOK_IDENT;
}


ConstStr_t
Scanner::charname( char _char ) {
  switch( _char ) {
  case '\t': return "tab char";
  case '\n': return "return char";
  case '\r': return "carriage return";
  case '\b': return "back space";
  default: break;
  }
  
  if( _char < 32 or _char >= 126 )
    return Str::fmt( "char #%d", _char );

  return Str::fmt( "'%c'", _char );
}

/** Return the name of a token
    @param token the token
    @return the name of the token
*/
ConstStr_t
Scanner::tokenname( int _token ) {
  /* search into the token table */
  for( int idx = 0; s_tokens[idx].m_name; ++ idx )
    if( s_tokens[idx].m_token == _token )
      return s_tokens[idx].m_name;
  
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
  if( _token < 256 )
    return charname( _token );
  
  /* don't know which token it is ! */
  return "unknown token";
}

void
Scanner::add_lookupdir( char const* _dir ) {
  if( *_dir == '/' ) {
    s_lookupdirs.push_back( _dir );
    return;
  }
  
  Str::Buf buffer( Str::Buf::Recycle );
  for( intptr_t capacity = 128; true; capacity *= 2 ) {
    char storage[capacity];
    if( not getcwd( storage, capacity ) ) {
      if( errno != ERANGE ) throw CWDError;
      continue; 
    }
    assert( storage[0] == '/' );
    buffer.write( storage );
    break;
  }
  
  buffer.write( "/" ).write( _dir );
  s_lookupdirs.push_back( buffer.m_storage );
}

ConstStr_t
Scanner::locate( char const* _name ) {
  Str::Buf buffer( Str::Buf::Recycle );
  
  for( std::vector<ConstStr_t>::iterator iter = s_lookupdirs.begin(); iter != s_lookupdirs.end(); iter++ ) {
    buffer.clear().write( iter->str() ).write( "/" ).write( _name );
    if( access( buffer.m_storage, R_OK ) != 0 ) continue;
    return buffer.m_storage;
  }
  return _name;
}

void
Scanner::sc_enter( int _condition ) {
  int sc = YY_START;
  if( sc == INITIAL ) {
    fileloc_mlt = fileloc;
    strbuf().clear();
  }
  if( _condition == source_code_context ) {
    if( sc == source_code_context ) { bracecount += 1; return; }
    else if( sc == INITIAL ) bracecount = 1;
    else assert( false );
  }
  scs.push_back( sc );
  BEGIN( _condition );
}

bool
Scanner::sc_leave() {
  int oldsc = YY_START;
  int newsc = Scanner::scs.back();
  
  if( oldsc == source_code_context and ((--bracecount) > 0) ) {
    newsc = source_code_context;
  } else {
    Scanner::scs.pop_back();
    BEGIN( newsc );
  }

  if( newsc == INITIAL ) return true;
  strbuf().write( yyleng, yytext );
  return false;
}

ConstStr_t
Scanner::all_operations() {
  static ConstStr_t all_ops;
  
  if( not all_ops ) {
    all_ops = ConstStr_t( "all_operations", Scanner::symbols );
  }
  return all_ops;
}

int yywrap() { return 1; }

