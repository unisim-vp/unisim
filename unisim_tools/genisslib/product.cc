/***************************************************************************
                                 product.cc
                             -------------------
    begin                : Thu May 25 2003
    copyright            : (C) 2003-2007 CEA and Universite Paris Sud
    authors              : Gilles Mouchard, Yves Lhuillier
    email                : gilles.mouchard@cea.fr, yves.lhuillier@cea.fr
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License version 2        *
 *   as published by the Free Software Foundation.                         *
 *                                                                         *
 ***************************************************************************/

#include <product.hh>
#include <scanner.hh>
#include <cstdarg>
#include <isa.hh>
#include <sourcecode.hh>
#include <strtools.hh>
#include <fstream>

using namespace std;

/** Counts the number of \n characters into a string
    @param s a C string
    @return the number of \n characters
*/
static
unsigned int
count_end_of_line( char const* _str ) {
  unsigned int count = 0;
  for( char const* ptr = _str; *ptr; ptr++ )
    if( *ptr == '\n' ) count += 1;
  return count;
}

/** Constructor: Create a Product object
    @param _prefix the prefix of output files
*/
Product_t::Product_t( ConstStr_t _prefix )
  : m_prefix( _prefix ), m_stream( 0 ), m_lineno( 1 )
{}

/** Open a file and redirects all output to this file
    @param _suffix the suffix of the new output file 
*/
bool
Product_t::open( char const* _suffix ) {
  delete m_stream;
  m_filename = Str::fmt( "%s.%s", m_prefix.str(), _suffix );
  m_lineno = 1;
  m_stream = new ofstream( m_filename );
  return m_stream->good();
}

/** Close the current output file 
 */
void
Product_t::close() {
  delete m_stream;
  m_stream = 0;
  m_filename = 0;
}

/** Destructor: Close the Product object */
Product_t::~Product_t() {
  delete m_stream;
}

/** Output source code into the output file
    Also generate #line in the output file to link the C compiler error to the original source code
    @param filename the name of the file where source code was found
    @param lineno the line number where source code was found
    @param format a C string with format specifier (like in printf), referenced arguments in the format string must follow
*/
void
Product_t::usercode( ConstStr_t _filename, unsigned int _lineno, char const* _fmt, ... ) {
  code( "\n#line %u \"%s\"\n", _lineno, _filename.str() );
  
  va_list args;
  for( intptr_t capacity = 128, size; true; capacity = (size > -1) ? size + 1 : capacity * 2 ) {
    /* stack allocation */
    char storage[capacity];
    /* Try to print in the allocated space. */
    va_start( args, _fmt );
    size = vsnprintf( storage, capacity, _fmt, args );
    va_end( args );
    /* If it didn't work, retry */
    if( size < 0 or size >= capacity ) continue;
    
    /* Now storage is ok... */
    sink() << storage;
    m_lineno += count_end_of_line( storage );
    
    break;
  }
  
  code( "\n#line %u \"%s\"\n", m_lineno + 2, m_filename.str() );
}

/** Output a string into the output file
    @param format a C string with format specifier (like in printf), referenced arguments in the format string must follow
*/

void
Product_t::code( char const* _fmt, ... ) {
  va_list args;
  for( intptr_t capacity = 128, size; true; capacity = (size > -1) ? size + 1 : capacity * 2 ) {
    /* stack allocation */
    char storage[capacity];
    /* Try to print in the allocated space. */
    va_start( args, _fmt );
    size = vsnprintf( storage, capacity, _fmt, args );
    va_end( args );
    /* If it didn't work, retry */
    if( size < 0 or size >= capacity ) continue;
    
    /* Now storage is ok... */
    sink() << storage;
    m_lineno += count_end_of_line( storage );
    break;
  }
}

/** Output a C string into the output file
    @param format a C string with format specifier (like in printf), referenced arguments in the format string must follow
*/
void
Product_t::cstring( char const* _string ) {
  sink() << std::flush << '"';

  int length = strlen( _string );
  char emit_buffer[length + 1];
  strcpy( emit_buffer, _string );
  
  char const* start = emit_buffer;
  
  for( char* current = emit_buffer; ; ++current ) {
    switch( *current ) {
    case '\n': *current = 0; sink() << start << "\\n"; break;
    case '\"': *current = 0; sink() << start << "\\\""; break;
    case '\\': *current = 0; sink() << start << "\\\\"; break;
    case '\t': *current = 0; sink() << start << "\\t"; break;
    case '\0': *current = 0; sink() << start << '\"'; return;
    start = current + 1;
    }
  }
}


void
Product_t::ns_leave( std::vector<ConstStr_t> const& _namespace ) {
  for( intptr_t idx = _namespace.size(); (--idx) >= 0; )
    code( "} " );
  code( "\n" );
}

void
Product_t::ns_enter( std::vector<ConstStr_t> const& _namespace ) {
  char const* sep = "";
  for( std::vector<ConstStr_t>::const_iterator ns = _namespace.begin(); ns < _namespace.end(); sep = " ", ++ ns ) {
    code( "%snamespace %s {", sep, (*ns).str() );
  }
  code( "\n" );
}

void
Product_t::template_signature( Vect_t<CodePair_t> const& _tparams ) {
  if( _tparams.empty() ) return;

  code( "template <" );
  
  bool intra = false;
  for( Vect_t<CodePair_t>::const_iterator iter = _tparams.begin(); iter < _tparams.end(); ++ iter ) {
    if( intra ) code( "," ); else intra = true;
    usercode( (*iter)->m_ctype->m_filename.str(), (*iter)->m_ctype->m_lineno,
              "\t%s", (*iter)->m_ctype->m_content.str() );
    
    usercode( (*iter)->m_csymbol->m_filename.str(), (*iter)->m_csymbol->m_lineno,
              "\t%s", (*iter)->m_csymbol->m_content.str() );
  }
  
  code( ">\n" );
}


void
Product_t::template_abrev( Vect_t<CodePair_t> const& _tparams ) {
  if( _tparams.empty() ) return;

  code( "<" );

  bool intra = false;
  for( Vect_t<CodePair_t>::const_iterator iter = _tparams.begin(); iter < _tparams.end(); ++ iter ) {
    if( intra ) code( "," ); else intra = true;
    usercode( (*iter)->m_csymbol->m_filename.str(), (*iter)->m_csymbol->m_lineno,
              "\t%s", (*iter)->m_csymbol->m_content.str() );
  }
  
  code( ">" );
}

