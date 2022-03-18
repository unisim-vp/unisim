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
#include <sstream>
#include <cassert>

/** Delete a subdecoder object
*/
SDClass::~SDClass() {}

std::string
SDClass::qd_namespace() const
{
  std::ostringstream buf;
  char const* sep = "";

  for (auto const& node : m_namespace)
    buf << sep << node.str(), sep = "::";
  
  return buf.str();
}

SDInstance::SDInstance( ConstStr _symbol, SourceCode const* _template_scheme, SDClass const* _sdclass, FileLoc const& _fileloc )
  : symbol( _symbol ), template_scheme( _template_scheme ), sdclass( _sdclass ), fileloc( _fileloc )
{}

SDInstance::~SDInstance() {}
