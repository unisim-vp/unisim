/***************************************************************************
                                subdecoder.cc
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

#include <subdecoder.hh>
#include <scanner.hh>
#include <comment.hh>
#include <variable.hh>
#include <sourcecode.hh>
#include <action.hh>
#include <bitfield.hh>
#include <action.hh>
#include <isa.hh>
#include <cassert>
#include <ostream>

using namespace std;

/** Create a subdecoder object
    @param symbol a symbol object representing the subdecoder
    @param bitfield_list a bit field list object containing the bit fields of the subdecoder
    @param comment_list a C/C++ comment list object containing C/C++ comments relative to the subdecoder
    @param variable_list a variable list object
    @param filename a filename object where the subdecoder was found
    @param lineno a line number where the subdecoder was found
    @return an subdecoder object
*/
SubDecoder_t::SubDecoder_t( ConstStr_t _symbol, SourceCode_t* _namespace, SourceCode_t* _template_scheme,
                            unsigned int _minsize, unsigned int _maxsize, FileLoc_t const& _fileloc )
  : m_symbol( _symbol ), m_namespace( _namespace ), m_template_scheme( _template_scheme ),
    m_minsize( _minsize ), m_maxsize( _maxsize ), m_fileloc( _fileloc )
{
  assert( m_minsize <= m_maxsize  );
}

/** Delete a subdecoder object
    @param subdecoder an subdecoder object to delete
*/
SubDecoder_t::~SubDecoder_t() {}

/** Dump an subdecoder object into a stream
    @param subdecoder an subdecoder object to dump
    @param _sink a stream
*/

ostream&
operator<<( ostream& _sink, SubDecoder_t const& _sd ) {
  _sink << "subdecoder " << _sd.m_symbol << '{' << (*_sd.m_namespace) << '}' << ' ';
  if( _sd.m_minsize == _sd.m_maxsize )
    _sink << '[' << _sd.m_maxsize << ']';
  else
    _sink << '[' << _sd.m_minsize << ';' << _sd.m_maxsize << ']';
  _sink << '\n';

  return _sink;
}

