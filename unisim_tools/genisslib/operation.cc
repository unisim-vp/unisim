/***************************************************************************
                                operation.cc
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

#include <operation.hh>
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

/** Create an operation object
    @param symbol a symbol object representing the operation
    @param bitfield_list a bit field list object containing the bit fields of the operation
    @param comment_list a C/C++ comment list object containing C/C++ comments relative to the operation
    @param variable_list a variable list object
    @param filename a filename object where the operation was found
    @param lineno a line number where the operation was found
    @return an operation object
*/
Operation_t::Operation_t( ConstStr_t _symbol, Vect_t<BitField_t>& _bitfields, Vect_t<Comment_t>& _comments,
                          SourceCode_t* _op_condition, FileLoc_t const& _fileloc )
  : m_symbol( _symbol ), m_bitfields( _bitfields ), m_comments( _comments ),
    m_condition( _op_condition ), m_fileloc( _fileloc )
{
}

/** Delete an operation object
    @param operation an operation object to delete
*/
Operation_t::~Operation_t() {}

/** Create a group object
    @param operation_list the list of the operation of the group
    @returns a group list object
*/
Group_t::Group_t( ConstStr_t _symbol, Vect_t<Operation_t>& _oplist, FileLoc_t const& _fileloc )
  : m_symbol( _symbol ), m_operations( _oplist ), m_fileloc( _fileloc )
{}

/** Search the operation for an action implementing an action prototype
    @param actionproto an action prototype object
    @return the matching action object, null if no action object matches
*/
Action_t const*
Operation_t::action( ActionProto_t const* _actionproto ) const {
  for( Vect_t<Action_t>::const_iterator iter = m_actions.begin(); iter < m_actions.end(); ++ iter )
    if( (*iter)->m_actionproto == _actionproto ) return *iter;
  
  return 0;
}

void
Operation_t::add( Action_t* _action ) {
  m_actions.push_back( _action );
  assert( not _action->m_operation );
  _action->m_operation = this;
}

/** Dump an operation object into a stream
    @param operation an operation object to dump
    @param _sink a stream
*/

ostream&
operator<<( ostream& _sink, Operation_t const& _op ) {
  _sink << "op " << _op.m_symbol << '(';
  
  char const* sep = "";
  for( Vect_t<BitField_t>::const_iterator bf = _op.m_bitfields.begin(); bf < _op.m_bitfields.end(); ++ bf, sep = ":" )
    _sink << sep << (**bf);
  
  _sink << ')';
  
  if( not _op.m_variables.empty() ) {
    _sink << " var ";
    sep = "";
    for( Vect_t<Variable_t>::const_iterator var = _op.m_variables.begin(); var < _op.m_variables.end(); sep = ", ", ++ var )
      _sink << sep << *(*var);
  }
  _sink << "\n\n";
  
  for( Vect_t<Action_t>::const_iterator action = _op.m_actions.begin(); action < _op.m_actions.end(); ++ action )
    _sink << *(*action) << '\n';
  
  return _sink;
}

