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
#include <action.hh>
#include <variable.hh>
#include <bitfield.hh>
#include <iostream>

using namespace std;

/** 
 *  @brief constraint object constructor
 *
 *  @param _symbol the variable symbol concerned about the specialization
 *  @param _value the specialization value for the target variable.
 *
 */

Constraint_t::Constraint_t( ConstStr_t _symbol, unsigned int _value )
  : m_symbol( _symbol ), m_value( _value )
{}

/**
 *  @brief constraint object destructor
 *
 */

Constraint_t::~Constraint_t() {}

/** Dump the specialization object into a stream
    @param stream a stream
*/
std::ostream&
operator<<( std::ostream& _sink, Constraint_t const& _var ) {
  return (_sink << _var.m_symbol << " = " << _var.m_value);
}

/** 
 *  @brief specialization object constructor
 *
 *  @param _operation the operation being specialized
 *  @param _variables the specialization variables
 *
 */

Specialization_t::Specialization_t( Operation_t* _operation, Vect_t<Constraint_t>& _constraints )
  : m_operation( _operation ), m_constraints( _constraints )
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
  for( Vect_t<Constraint_t>::const_iterator node = _var.m_constraints.begin(); node < _var.m_constraints.end(); ++node, sep = ", " )
    _sink << sep << (**node);
  _sink << " )";
  return _sink;
}

Operation_t*
Specialization_t::newop() {
  ConstStr_t symbol; // The symbol of the operation
  {
    Str::Buf buffer( Str::Buf::Recycle );
    buffer.write( "__spec__" ).write( m_operation->m_symbol );
    for( Vect_t<Constraint_t>::const_iterator expr = m_constraints.begin(); expr < m_constraints.end(); ++ expr )
      buffer.write( Str::fmt( "_%s_%x", (**expr).m_symbol.str(), (**expr).m_value ) );
    symbol = Str::tokenize( buffer.m_storage );
  }

  //  Actions, comments, variables, conditions, and fileloc are
  //  duplicated.
  Operation_t* res = new Operation_t( symbol, *static_cast<Vect_t<BitField_t>*>( 0 ), m_operation->m_comments,
                                      m_operation->m_condition, m_operation->m_fileloc );
  
  res->m_variables = m_operation->m_variables;
  res->m_actions = m_operation->m_actions;
  // Generating new bitfield.
  Vect_t<BitField_t>& bflist = m_operation->m_bitfields;
  
  for( Vect_t<BitField_t>::const_iterator bf = bflist.begin(); bf < bflist.end(); ++ bf ) {
    Constraint_t* expr;
    if( (**bf).type() == BitField_t::Operand and (expr = constraint( (**bf).symbol() )) ) {
      OperandBitField_t const& opbf = dynamic_cast<OperandBitField_t const&>( **bf );
      res->m_bitfields.push_back( new SpOperandBitField_t( opbf, expr->m_value ) );
    } else {
      res->m_bitfields.push_back( *bf );
    }
  }
  
  return res;
}

Constraint_t*
Specialization_t::constraint( ConstStr_t _symbol ) {
  for( Vect_t<Constraint_t>::iterator expr = m_constraints.begin(); expr < m_constraints.end(); ++ expr )
    if( (**expr).m_symbol == _symbol ) return *expr;
  return 0;
}
