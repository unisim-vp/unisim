/***************************************************************************
                                 specialization.cc
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

#include <specialization.hh>
#include <operation.hh>
#include <strtools.hh>
#include <iostream>

using namespace std;

/** Dump the specialization object into a stream
    @param stream a stream
*/
std::ostream&
operator<<( std::ostream& _sink, Assignment_t const& _var ) {
  return (_sink << _var.m_symbol << " = " << _var.m_value);
}

/** 
 *  @brief specialization object constructor
 *  @param _operation the operation being specialized
 *  @param _variables the specialization variables
 *
 */

Specialization_t::Specialization_t( Operation_t* _operation, Vect_t<Assignment_t>& _assignments )
  : m_operation( _operation ), m_assignments( _assignments )
{}

/** 
 *  @brief specialization object destructor
 *
 */

Specialization_t::~Specialization_t() {};

/** Dump the specialization object into a stream
    @param stream a stream
*/
std::ostream&
operator<<( std::ostream& _sink, Specialization_t const& _var ) {
  _sink << _var.m_operation->m_symbol << " ( ";
  char const* sep = "";
  for( Vect_t<Assignment_t>::const_iterator node = _var.m_assignments.begin(); node < _var.m_assignments.end(); ++node, sep = ", " )
    _sink << sep << (**node);
  _sink << " )";
  return _sink;
}
