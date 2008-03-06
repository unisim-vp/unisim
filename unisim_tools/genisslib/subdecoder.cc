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
#include <strtools.hh>
#include <cassert>
#include <ostream>

using namespace std;

/** Create a subdecoder class object
    @param _namespace a namespace in which the decoder is defined
    @param _minsize a minimum bit size for the decoder operations
    @param _maxsize a maximum bit size for the decoder operations
    @param _fileloc a fileloc pointing at the subdecoder declaration
*/
SDClass_t::SDClass_t( std::vector<ConstStr_t>& _namespace, unsigned int _minsize, unsigned int _maxsize, FileLoc_t const& _fileloc )
  : m_namespace( _namespace ), m_minsize( _minsize ), m_maxsize( _maxsize ), m_fileloc( _fileloc )
{
  assert( m_minsize <= m_maxsize  );
}

/** Delete a subdecoder object
*/
SDClass_t::~SDClass_t() {}

ConstStr_t
SDClass_t::qd_namespace() const {
  Str::Buf buffer( Str::Buf::Recycle );
  char const* sep = "";

  for( std::vector<ConstStr_t>::const_iterator node = m_namespace.begin(); node != m_namespace.end(); ++ node, sep = "::" )
    buffer.write( sep ).write( *node );
  
  return ConstStr_t( buffer.m_storage );
}

SDInstance_t::SDInstance_t( ConstStr_t _symbol, SourceCode_t const* _template_scheme, SDClass_t const* _sdclass, FileLoc_t const& _fileloc )
  : m_symbol( _symbol ), m_template_scheme( _template_scheme ), m_sdclass( _sdclass ), m_fileloc( _fileloc )
{}

SDInstance_t::~SDInstance_t() {}

// /** Dump an subdecoder object into a stream
//     @param subdecoder an subdecoder object to dump
//     @param _sink a stream
// */

// ostream&
// operator<<( ostream& _sink, SubDecoder_t const& _sd ) {
//   _sink << "subdecoder " << _sd.m_symbol << '{' << (*_sd.m_namespace) << '}' << ' ';
//   if( _sd.m_minsize == _sd.m_maxsize )
//     _sink << '[' << _sd.m_maxsize << ']';
//   else
//     _sink << '[' << _sd.m_minsize << ';' << _sd.m_maxsize << ']';
//   _sink << '\n';

//   return _sink;
// }

