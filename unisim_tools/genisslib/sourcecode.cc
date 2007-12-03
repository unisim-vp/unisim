/***************************************************************************
                                sourcecode.cc
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

#include <sourcecode.hh>
#include <ostream>
using namespace std;

SourceCode_t const* SourceCode_t::s_last_srccode = 0;

/** Create a C source code object
    @param _content a C string containing the C source code
    @param filename a filename object where was found the C source code
    @param lineno a line number where was found the C source code
    @return a C source code object
*/
SourceCode_t::SourceCode_t( ConstStr_t _content, FileLoc_t const& _fileloc )
  : m_content( _content ), m_fileloc( _fileloc )
{
  s_last_srccode = this;
}

/** Dump a C source code object into a stream
    @param source_code a C source code object to dump
    @param _sink a stream
*/
std::ostream&
operator<<( std::ostream& _sink, SourceCode_t const& _sc ) {
  _sink << '{' << _sc.m_content << '}';
  return _sink;
}

/** Create a parameter object
    @param c_type a C type
    @param c_symbol a C symbol
    @returns a parameter object
*/
CodePair_t::CodePair_t( SourceCode_t* _ctype, SourceCode_t* _csymbol )
  : m_ctype( _ctype ), m_csymbol( _csymbol )
{}

CodePair_t::~CodePair_t() {}

/** Dump a parameter object into a stream
    @param _sink a stream
*/
std::ostream&
operator<<( std::ostream& _sink, CodePair_t const& _cp ) {
  return (_sink << (*_cp.m_ctype) << ' ' << (*_cp.m_csymbol));
}
