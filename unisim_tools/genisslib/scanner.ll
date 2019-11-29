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

#include <parser_defs.hh>
#include <scanner.hh>
#include <strtools.hh>
#include <sourcecode.hh>
#include <comment.hh>
#include <iostream>
#include <cerrno>

#ifdef WIN32
#include <windows.h>

#define YY_NEVER_INTERACTIVE 1
#endif

bool                     Scanner::aborted_scanning = false;
FileLoc                Scanner::fileloc;
FileLoc                Scanner::fileloc_mlt;
int                      Scanner::bracecount = 0;
std::vector<int>         Scanner::scs;
Vector<Comment>        Scanner::comments;
Isa*                     Scanner::s_isa = 0;
ConstStr::Pool           Scanner::symbols;
std::vector<ConstStr>    Scanner::s_lookupdirs;

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

/* This is for column counting. */
#define YY_USER_ACTION Scanner::fileloc.newtoken( yyleng );

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
<string_context>[^\"\n] { Scanner::strbuf().append( yytext, yyleng ); }
<string_context>\n { Scanner::strbuf().append( yytext, yyleng ); Scanner::fileloc.newline(); }
<string_context>\\\" { Scanner::strbuf().append( yytext, yyleng ); }
<string_context>\" { if (Scanner::sc_leave()) { yylval.volatile_string = Scanner::strbuf().c_str(); return TOK_STRING; }
}

\{ { Scanner::sc_enter( source_code_context ); }
<source_code_context>[^/\}\{\"\n\'] {  Scanner::strbuf().append( yytext, yyleng ); }
<source_code_context>"/" { Scanner::strbuf().append( yytext, yyleng ); }
<source_code_context>\' { Scanner::sc_enter( char_context ); Scanner::strbuf().append( yytext, yyleng ); }
<source_code_context>\{ { Scanner::bracecount++; Scanner::strbuf().append( yytext, yyleng ); }
<source_code_context>\" { Scanner::sc_enter( string_context ); Scanner::strbuf().append( yytext, yyleng ); }
<source_code_context>\n { Scanner::strbuf().append( yytext, yyleng ); Scanner::fileloc.newline(); }
<source_code_context>\} {
  if (Scanner::sc_leave()) {
    yylval.sourcecode = new SourceCode( Scanner::strbuf().c_str(), Scanner::fileloc_mlt );
    return TOK_SOURCE_CODE;
  }
}

<char_context>[^\'\n] { Scanner::strbuf().append( yytext, yyleng ); }
<char_context>\n { Scanner::strbuf().append( yytext, yyleng ); Scanner::fileloc.newline(); }
<char_context>\\\' { Scanner::strbuf().append( yytext, yyleng ); }
<char_context>\' { Scanner::sc_leave(); }

<INITIAL,source_code_context>"/*" { Scanner::sc_enter( c_like_comment_context ); Scanner::strbuf().append( yytext, yyleng ); }
<c_like_comment_context>[^*\n] { Scanner::strbuf().append( yytext, yyleng ); }
<c_like_comment_context>"*"+[^/\n] { Scanner::strbuf().append( yytext, yyleng ); }
<c_like_comment_context>"*"+\n { Scanner::strbuf().append( yytext, yyleng ); Scanner::fileloc.newline(); }
<c_like_comment_context>\n { Scanner::strbuf().append( yytext, yyleng ); Scanner::fileloc.newline(); }
<c_like_comment_context>"*"+"/" {
  if (Scanner::sc_leave()) {
    Scanner::strbuf().append( yytext, yyleng );
    Scanner::comments.append( new Comment( Scanner::strbuf().c_str(), Scanner::fileloc_mlt ) );
  }
}

<INITIAL,source_code_context>"//" { Scanner::sc_enter( cpp_like_comment_context ); Scanner::strbuf().append( yytext, yyleng ); }
<cpp_like_comment_context>[^\n] { Scanner::strbuf().append( yytext, yyleng ); }
<cpp_like_comment_context>\n {
  if (Scanner::sc_leave()) {
    Scanner::comments.append( new Comment( Scanner::strbuf().c_str(), Scanner::fileloc_mlt ) );
  }
  Scanner::fileloc.newline();
}

{binary_number} { parse_binary_number( yytext, yyleng, &yylval.uinteger ); return TOK_INTEGER; }
{hexadecimal_number} { sscanf( yytext, "%x", &yylval.uinteger ); return TOK_INTEGER; }
{decimal_number} { sscanf(yytext, "%u", &yylval.uinteger); return TOK_INTEGER; }
{identifier} {
  int token = Scanner::token( yytext );
  if (token == TOK_IDENT)
    yylval.persistent_string = ConstStr( yytext, Scanner::symbols ).str();
  return token;
}
\\\n { Scanner::fileloc.newline(); }
\n { Scanner::fileloc.newline(); return TOK_ENDL; }
";" { return TOK_ENDL; }
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
"=" { return '='; }
"-" { return '-'; }
"?" { return '?'; }
"::" { return TOK_QUAD_DOT; }
" " { }
\t { }
<INITIAL><<EOF>> { if (not Scanner::pop()) yyterminate(); }
<string_context,char_context,c_like_comment_context,cpp_like_comment_context,source_code_context><<EOF>> {
  Scanner::fileloc.err( "error: unexpected end of file" );
  Scanner::aborted_scanning = true;
  yyterminate();
  return 0;
}
<INITIAL,string_context,char_context,c_like_comment_context,cpp_like_comment_context,source_code_context>. {
  Scanner::fileloc.err( "error: unexpected %s", Scanner::charname( yytext[0] ).str() );
  Scanner::aborted_scanning = true;
  yyterminate();
  return 0;
}

%%

#include <conststr.hh>
#include <cstring>

#include <cassert>
#include <isa.hh>

Scanner::Inclusion* Scanner::include_stack = 0;

std::string& Scanner::strbuf() { static std::string s_buffer; return s_buffer; }

void
Scanner::push()
{
  YY_BUFFER_STATE state = YY_CURRENT_BUFFER;
  
  include_stack = new Inclusion( (uint8_t const*)&state, sizeof( YY_BUFFER_STATE ), fileloc, include_stack );
}

bool
Scanner::pop()
{
  Inclusion* tail = include_stack;
  
  if (not tail) return false;
  YY_BUFFER_STATE state;
  tail->restore( (uint8_t*)&state, sizeof( YY_BUFFER_STATE ) );
  yy_delete_buffer( YY_CURRENT_BUFFER );
  if (yyin) fclose( yyin );
  yy_switch_to_buffer( state );
  
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
  memcpy( state_backup, _state, _size );
}

Scanner::Inclusion::~Inclusion()
{
  delete [] state_backup;
}

void
Scanner::Inclusion::restore( uint8_t* _state, intptr_t _size )
{
  memcpy( _state, state_backup, _size );
}

bool
Scanner::parse( char const* _filename, Isa& _isa )
{
  s_isa = &_isa;
  if (not Scanner::open( ConstStr( _filename) ) )
    return false;
  bracecount = 0;
  scs.clear();
  
#if 0
  // This code is only for testing the lexical analyzer
  int c;
    
  while( (c = yylex()) != 0 ) {
    printf( "%s\n", get_token_name(c) );
  }
#endif
  
  aborted_scanning = false;
  int error = yyparse();
  
  if (yyin) {
    fclose( yyin );
    yyin = 0;
  }
  
  yylex_destroy();
  return (error == 0) && !aborted_scanning;
}

bool
Scanner::open( ConstStr _filename )
{
  fprintf( stderr, "Opening %s\n", _filename.str() );
  yyin = fopen( _filename.str(), "r" );
  
  if (not yyin) {
    fileloc.err( "error: can't open file `%s'", _filename.str() );
    return false;
  }
  
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

  yy_switch_to_buffer( yy_create_buffer( yyin, YY_BUF_SIZE ) );
  yylineno = 1;
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

void
Scanner::sc_enter( int _condition )
{
  int sc = YY_START;
  if (sc == INITIAL) {
    fileloc_mlt = fileloc;
    strbuf().clear();
  }
  if (_condition == source_code_context) {
    if (sc == source_code_context) { bracecount += 1; return; }
    else if (sc == INITIAL) bracecount = 1;
    else assert( false );
  }
  scs.push_back( sc );
  BEGIN( _condition );
}

bool
Scanner::sc_leave()
{
  int oldsc = YY_START;
  int newsc = Scanner::scs.back();
  
  if (oldsc == source_code_context and ((--bracecount) > 0)) {
    newsc = source_code_context;
  } else {
    Scanner::scs.pop_back();
    BEGIN( newsc );
  }

  if (newsc == INITIAL) return true;
  strbuf().append( yytext, yyleng );
  return false;
}

int yywrap() { return 1; }

