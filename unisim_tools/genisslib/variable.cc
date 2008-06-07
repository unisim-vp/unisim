/***************************************************************************
                                 variable.cc
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

#include <variable.hh>
#include <sourcecode.hh>
#include <strtools.hh>
#include <iostream>

using namespace std;

Variable_t::Variable_t( ConstStr_t _symbol, SourceCode_t* _ctype, SourceCode_t* _cinit )
  : m_symbol( _symbol ), m_ctype( _ctype ), m_cinit( _cinit )
{}

Variable_t::Variable_t( Variable_t const& _variable )
  : m_symbol( _variable.m_symbol ), m_ctype( _variable.m_ctype ), m_cinit( _variable.m_cinit )
{}

Variable_t::~Variable_t() {};

/** Dump the variable object into a stream
    @param stream a stream
*/
std::ostream&
operator<<( std::ostream& _sink, Variable_t const& _var ) {
  return (_sink << _var.m_symbol << " : " << (*_var.m_ctype));
}

Inheritance_t::Inheritance_t( SourceCode_t* _modifier, SourceCode_t* _typename, SourceCode_t* _initargs )
  : m_modifier( _modifier ), m_typename( _typename ), m_initargs( _initargs )
{}

Inheritance_t::Inheritance_t( Inheritance_t const& _inh )
  : m_modifier( _inh.m_modifier ), m_typename( _inh.m_typename ), m_initargs( _inh.m_initargs )
{}

Inheritance_t::~Inheritance_t() {};
