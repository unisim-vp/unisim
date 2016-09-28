/***************************************************************************
                                  action.cc
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

#include <action.hh>
#include <operation.hh>
#include <sourcecode.hh>
#include <comment.hh>
#include <strtools.hh>
#include <iostream>

/** Constructor
    @param operation an operation object to which belong the action
    @param actionproto an action prototype object which is the prototype of this action
    @param source_code a C source code object which is the implementation of the action
    @param comment_list a C/C++ comment list object containing C/C++ comments relative to the action
    @param filename a filename object where the action was found
    @param lineno a line number where the action was found
*/
Action::Action( ActionProto const* _actionproto, SourceCode_t* _source_code,
                    Vect_t<Comment_t>& _comments, FileLoc_t const& _fileloc )
  : m_operation( 0 ), m_actionproto( _actionproto ), m_source_code( _source_code ),
    m_comments( _comments ), m_fileloc( _fileloc )
{}

/** Destructor
*/
Action::~Action() {}

/** Dump an action object into a stream
    @param action an action object to dump
    @param _sink a stream
*/
std::ostream&
operator<<( std::ostream& _sink, Action const& _act ) {
  return (_sink << _act.m_operation->m_symbol << '.' << _act.m_actionproto->m_symbol << " = " << (*_act.m_source_code) << '\n');
}


/** Create an action prototype object
    @param type the type of the action prototype (constructor, destructor, static, or common)
    @param symbol a symbol object representing the action prototype
    @param returns a C source code object which is the return type of the action
    @param params a C source code pair object which is the parameters of the action
    @param default_source_code a C source code object which is the default implementation of the action
    @param comment_list a C/C++ comment list object containing C/C++ comments relative to the action prototype
    @param filename a filename object where the action prototype was found
    @param lineno a line number where the action prototype was found
*/
ActionProto::ActionProto( ActionProto::type_t _type, ConstStr _symbol, SourceCode_t* _returns,
                              Vect_t<CodePair_t>& _params, bool _constness, SourceCode_t* _defaultcode,
                              Vect_t<Comment_t>& _comments, FileLoc_t const& _fileloc )
  : m_type( _type ), m_symbol( _symbol ), m_returns( _returns ),
    m_params( _params ), m_constness( _constness ), m_defaultcode( _defaultcode ),
    m_comments( _comments ), m_fileloc( _fileloc )
{}

/** Dump an action prototype object into a stream
    @param _sink a stream
*/
std::ostream&
operator<<( std::ostream& _sink, ActionProto const& _ap ) {
  switch( _ap.m_type ) {
  case ActionProto::Constructor:  _sink << "constructor "; break;
  case ActionProto::Static:       _sink << "static "; break;
  case ActionProto::Destructor:   _sink << "destructor "; break;
  case ActionProto::Common:       /* do nothing */ break;
  }
  
  _sink << "action ";
  
  if( _ap.m_returns ) _sink << (*_ap.m_returns) << ' ';

  _sink << _ap.m_symbol << "(";
  char const* sep = "";
  for( Vect_t<CodePair_t>::const_iterator codepair = _ap.m_params.begin(); codepair < _ap.m_params.end(); sep = ", ", ++ codepair )
    _sink << sep << *(*codepair);
  
  _sink << ") " << (_ap.m_constness ? "const " : "")<< (*_ap.m_defaultcode) << '\n';
  return _sink;
}

ActionProto::~ActionProto() {}

char const*
ActionProto::returntype() const {
  if( m_returns ) return m_returns->m_content.str();
  return "void";
}
