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

/** Delete a subdecoder object
*/
SDClass_t::~SDClass_t() {}

ConstStr
SDClass_t::qd_namespace() const {
  std::string buffer;
  std::string sep;

  for( std::vector<ConstStr>::const_iterator node = m_namespace.begin(); node != m_namespace.end(); ++ node, sep = "::" )
    buffer += sep + node->str();
  
  return ConstStr( buffer.c_str() );
}

SDInstance_t::SDInstance_t( ConstStr _symbol, SourceCode const* _template_scheme, SDClass_t const* _sdclass, FileLoc_t const& _fileloc )
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
//   if (_sd.m_minsize == _sd.m_maxsize)
//     _sink << '[' << _sd.m_maxsize << ']';
//   else
//     _sink << '[' << _sd.m_minsize << ';' << _sd.m_maxsize << ']';
//   _sink << '\n';

//   return _sink;
// }

