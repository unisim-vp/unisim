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
#include <iostream>

using namespace std;

/** Constructor: Create a Product object
    @param _filename the filename attached to the output code
*/
Product_t::Product_t( ConstStr_t filename, bool sourcelines )
  : m_filename( filename ), m_line( Str::Buf::Recycle ), m_lineno( 1 ), m_sourcelines( sourcelines )
{
  m_indentations.push_back( 0 );
}

/** Constructor: Create a FProduct object
    @param _prefix the filename prefix attached to the output code
*/
FProduct_t::FProduct_t( char const* prefix, char const* suffix, bool sourcelines )
  : Product_t( Str::fmt( "%s%s", prefix, suffix ), sourcelines ),
    m_sink( new std::ofstream( m_filename ) )
{}

/** Constructor: Create a SProduct object
    @param _prefix the filename prefix attached to the output code
*/
SProduct_t::SProduct_t( ConstStr_t _filename, bool _sourcelines )
  : Product_t( _filename, _sourcelines ), m_content( Str::Buf::Recycle )
{}

/** Destructor: Close the Product object */
FProduct_t::~FProduct_t() { delete m_sink; }

/** Output source code into the output file
    Also generate #line in the output file to link the C compiler error to the original source code
    @param _source the ScourceCode_t object to dump
 */
Product_t&
Product_t::usercode( SourceCode_t const& _source ) {
  return usercode( _source.m_fileloc, "%s", _source.m_content.str() );
}

/** Output source code with surrounding braces into the output file
    Also generate #line in the output file to link the C compiler error to the original source code
    @param _source the ScourceCode_t object to dump
 */
Product_t&
Product_t::usercode( SourceCode_t const& _source, char const* _fmt ) {
  return usercode( _source.m_fileloc, _fmt, _source.m_content.str() );
}

/** Output source code into the output file
    Also generate #line in the output file to link the C compiler error to the original source code
    @param filename the name of the file where source code was found
    @param lineno the line number where source code was found
    @param format a C string with format specifier (like in printf), referenced arguments in the format string must follow
*/
Product_t&
Product_t::usercode( FileLoc_t const& _fileloc, char const* _fmt, ... ) {
  if( m_sourcelines ) {
    require_newline();
    code( "#line %u \"%s\"\n", _fileloc.m_line, _fileloc.m_name.str() );
  }
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
    write( storage );
    break;
  }
  
  if( m_sourcelines ) {
    require_newline();
    code( "#line %u \"%s\"\n", m_lineno + 1, m_filename.str() );
  }
  return *this;
}

/** Output a string into the output file
    @param format a C string with format specifier (like in printf), referenced arguments in the format string must follow
*/

Product_t&
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
    write( storage );
    break;
  }
  return *this;
}

Product_t&
Product_t::require_newline() {
  if( m_line.empty() ) return *this;
  write( "\n" );
  return *this;
}

Product_t&
Product_t::ns_leave( std::vector<ConstStr_t> const& _namespace ) {
  for( intptr_t idx = _namespace.size(); (--idx) >= 0; )
    code( "} " );
  code( "\n" );
  return *this;
}

Product_t&
Product_t::ns_enter( std::vector<ConstStr_t> const& _namespace ) {
  char const* sep = "";
  for( std::vector<ConstStr_t>::const_iterator ns = _namespace.begin(); ns < _namespace.end(); sep = " ", ++ ns ) {
    code( "%snamespace %s {", sep, (*ns).str() );
  }
  code( "\n" );
  flatten_indentation();
  return *this;
}

Product_t&
Product_t::template_signature( Vect_t<CodePair_t> const& _tparams ) {
  if( _tparams.empty() ) return *this;

  code( "template <" );
  
  bool intra = false;
  for( Vect_t<CodePair_t>::const_iterator tp = _tparams.begin(); tp < _tparams.end(); ++ tp ) {
    if( intra ) code( "," ); else intra = true;
    usercode( (**tp).m_ctype->m_fileloc, "\t%s", (**tp).m_ctype->m_content.str() );
    usercode( (**tp).m_csymbol->m_fileloc, "\t%s", (**tp).m_csymbol->m_content.str() );
  }
  
  code( ">\n" );
  return *this;
}


Product_t&
Product_t::template_abbrev( Vect_t<CodePair_t> const& _tparams ) {
  if( _tparams.empty() ) return *this;

  code( "<" );

  bool intra = false;
  for( Vect_t<CodePair_t>::const_iterator tp = _tparams.begin(); tp < _tparams.end(); ++ tp ) {
    if( intra ) code( "," ); else intra = true;
    usercode( (**tp).m_csymbol->m_fileloc, "\t%s", (**tp).m_csymbol->m_content.str() );
  }
  
  code( ">" );
  return *this;
}

Product_t&
Product_t::flatten_indentation() {
  if( m_indentations.empty() ) return *this;
  int indentation = m_indentations.back();
  vector<int>::reverse_iterator prev = m_indentations.rbegin();
  for( vector<int>::reverse_iterator ind = m_indentations.rbegin(); ind < m_indentations.rend(); ++ind ) {
    if( *ind == indentation ) continue;
    prev = ind;
    break;
  }
  indentation = *prev;
  for( vector<int>::reverse_iterator ind = m_indentations.rbegin(); ind < prev; ++ind )
    *ind = indentation;
  return *this;
}


Product_t&
Product_t::write( char const* _ptr ) {
  if( not _ptr ) return *this;
  for( char chr = *_ptr; chr; chr = *++_ptr ) {
    if( chr == '\n' ) {
      int current_indentation = m_indentations.back();
      int braces = 0;
      // Computing brace depth and right stripping blank characters.
      char const* rstrip = m_line.m_storage;
      for( char const* pchr = rstrip; *pchr; ++pchr ) {
        if( *pchr <= ' ' ) continue;
        rstrip = pchr + 1;
        if(      *pchr == '{' ) ++braces;
        else if( *pchr == '}' ) --braces;
      }
      m_line.truncate( rstrip-m_line.m_storage );
      if( m_line.empty() ) { xwrite( "\n" ); m_lineno += 1; continue; }
      
      if( braces > 0 ) {
        while( (--braces) > 0) m_indentations.push_back( current_indentation );
        m_indentations.push_back( current_indentation + 1 );
      } else if( braces < 0 ) {
        int nlength = m_indentations.size() + braces;
        if( nlength > 0 ) {
          m_indentations.resize( nlength );
        } else {
          cerr << "Indentation error (line " << m_lineno << ").\n";
          m_indentations.clear();
          m_indentations.push_back( 0 );
        }
      }
      
      char first_char = m_line.m_storage[0], last_char = m_line.m_storage[m_line.m_index-1];
      if( first_char == '#' ) current_indentation = 0;
      else if( /*'{'*/ first_char == '}' ) current_indentation = m_indentations.back();
      else if( last_char == ':' ) --current_indentation;
      
      while( (--current_indentation) >= 0 ) xwrite( "\t" );
      xwrite( m_line.m_storage );
      xwrite( "\n" );
      m_line.clear();
      m_lineno += 1;
      continue;
    }
    if( m_line.empty() and chr <= ' ' ) continue;
    m_line.write( chr );
  }
  return *this;
}

/** \brief flush the line buffer
 *
 */
void Product_t::flush() { if( not m_line.empty() ) write( "\n" ); }

void FProduct_t::xwrite( char const* _ptr ) { (*m_sink) << _ptr; }

bool FProduct_t::good() const { return m_sink->good(); };

void SProduct_t::xwrite( char const* _ptr ) { m_content.write( _ptr ); }
