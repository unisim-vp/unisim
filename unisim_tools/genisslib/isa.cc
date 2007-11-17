/***************************************************************************
                                    isa.cc
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

#include <isa.hh>
#include <main.hh>
#include <operation.hh>
#include <strtools.hh>
#include <action.hh>
#include <comment.hh>
#include <sourcecode.hh>
#include <bitfield.hh>
#include <variable.hh>
#include <scanner.hh>
#include <product.hh>
#include <unistd.h>
#include <cassert>
#include <cerrno>
#include <iostream>
#include <riscgenerator.hh>

using namespace std;

/** Constructor of Isa instance 
 */
Isa::Isa()
  : m_little_endian( 0 )
{}


/** Destructor for isa instance
 *  Where most of the allocated stuff will be released
 */
Isa::~Isa() {}

/** Search for an operation object
    @param operation_symbol a symbol object representing the operation
    @return the matching operation object, null if no operation object matches
*/
Operation_t*
Isa::operation( ConstStr_t _symbol ) {
  for( Vect_t<Operation_t>::iterator op = m_operations.begin(); op < m_operations.end(); ++ op )
    if( (*op)->m_symbol == _symbol ) return *op;
  
  return 0;
}

/** Remove an operation object from the global operation object list (operation_list)
    @param operation the operation object to remove
*/
void
Isa::remove( Operation_t* _op ) {
  for( Vect_t<Operation_t>::iterator iter = m_operations.begin(); iter < m_operations.end(); ++ iter ) {
    if( *iter != _op ) continue;
    m_operations.erase( iter );
    return;
  }
}

/** Search the global group lists for the given symbol
    @param _symbol the group symbol looked for
    @return the found group
*/
Group_t*
Isa::group( ConstStr_t _symbol ) {
  for( Vect_t<Group_t>::iterator group = m_groups.begin(); group < m_groups.end(); ++ group )
    if( (*group)->m_symbol == _symbol ) return *group;
  
  return 0;
}

/** Search for an action prototype object
    @param action_proto_symbol a symbol object representing the action prototype
    @return the matching action prototype object, null if no action prototype object matches
*/
ActionProto_t const*
Isa::actionproto( ConstStr_t _symbol ) const {
  for( Vect_t<ActionProto_t>::const_iterator proto = m_actionprotos.begin(); proto < m_actionprotos.end(); ++ proto )
    if( (*proto)->m_symbol == _symbol ) return *proto;

  return 0;
}

/** Remove an action prototype object from the global action prototype object list (action_proto_list)
    @param actionproto the action prototype object to remove
*/
void
Isa::remove( ActionProto_t const* _ap ) {
  for( Vect_t<ActionProto_t>::iterator iter = m_actionprotos.begin(); iter < m_actionprotos.end(); ++ iter ) {
    if( *iter != _ap ) continue;
    m_actionprotos.erase( iter );
    return;
  }
}

/** Dump all objects from the global objects list (source_code_list, action_proto_list, operation_list) into a stream
    with the same format as in the original code
    @param _sink a stream
*/
void
Isa::expand( ostream& _sink ) const {
  // dumping endianness
  _sink << (m_little_endian ? "little_endian" : "big_endian") << '\n';
  
  // dumping namespace
  if( not m_namespace.empty() ) {
    _sink << "namespace ";
    char const* sep = "";
    for( std::vector<ConstStr_t>::const_iterator piece = m_namespace.begin(); piece < m_namespace.end(); sep = "::", ++ piece )
      _sink << sep << (*piece);
    _sink << '\n';
  }
  
  // dumping address type
  _sink << "address {" << m_addrtype << "}\n";
  
  // dumping template parameters
  if( not m_tparams.empty() ) {
    _sink << "template <";
    char const* sep = "";
    for( Vect_t<CodePair_t>::const_iterator iter = m_tparams.begin(); iter < m_tparams.end(); sep = ", ", ++ iter )
      _sink << sep << *(*iter);
    _sink << ">\n";
  }
  _sink << '\n';
  
  for( Vect_t<SourceCode_t>::const_iterator srccode = m_decl_srccodes.begin(); srccode < m_decl_srccodes.end(); ++ srccode )
    _sink << "decl " << *(*srccode) << "\n\n";
  for( Vect_t<SourceCode_t>::const_iterator srccode = m_impl_srccodes.begin(); srccode < m_impl_srccodes.end(); ++ srccode )
    _sink << "impl " << *(*srccode) << "\n\n";
  _sink << '\n';

  if( not m_vars.empty() ) {
    _sink << "var ";
    char const* sep = "";
    for( Vect_t<Variable_t>::const_iterator var = m_vars.begin(); var < m_vars.end(); sep = ", ", ++ var )
      _sink << sep << *(*var);
    _sink << '\n';
  }
  _sink << '\n';
  
  for( Vect_t<ActionProto_t>::const_iterator ap = m_actionprotos.begin(); ap < m_actionprotos.end(); ++ap )
    _sink << *(*ap) << '\n';
  
  _sink << '\n';
  
  for( Vect_t<Operation_t>::const_iterator op = m_operations.begin(); op < m_operations.end(); ++ op )
    _sink << *(*op) << '\n';
}


bool
Isa::compile( Product_t& _product, unsigned int _wordsize ) const {
  auto_ptr<Generator> generator;
  
  generator = auto_ptr<Generator>( new RiscGenerator( this ) );
  
  return
    generator->finalize() and
    generator->sanity_checks() and
    generator->generate_iss( _product, _wordsize );
}

