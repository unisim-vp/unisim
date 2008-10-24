/***************************************************************************
                                 action.hh
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

#ifndef __ACTION_HH__
#define __ACTION_HH__

#include <fwd.hh>
#include <conststr.hh>
#include <referencecounting.hh>
#include <errtools.hh>
#include <vect.hh>
#include <iosfwd>

/** An action implementation object */
struct Action_t : virtual ReferenceCounter {
  Operation_t const*       m_operation;    /**< The associated operation  */
  ActionProto_t const*     m_actionproto;  /**< The associated action prototype */
  Ptr_t<SourceCode_t>      m_source_code;  /**< The C implementation of the action */
  Vect_t<Comment_t>        m_comments;     /**< The list of the C comment associated with the action */
  FileLoc_t                m_fileloc;      /**< File location of the declaration */
  //  Action_t*                m_base;

  Action_t( ActionProto_t const* _actionproto, SourceCode_t* _source_code,
            Vect_t<Comment_t>& _comments, FileLoc_t const& _fileloc );
  ~Action_t();
  
};

std::ostream& operator<<( std::ostream& _sink, Action_t const& _act );

/** An action prototype object */
struct ActionProto_t : virtual ReferenceCounter {
  enum type_t { Constructor, Destructor, Static, Common };
  type_t                   m_type;                /**< The type of the action prototype */
  ConstStr_t               m_symbol;              /**< The associated symbol */
  Ptr_t<SourceCode_t>      m_returns;             /**< The C return type of the action */
  Vect_t<CodePair_t>       m_params;              /**< The C parameters of the action */
  bool                     m_constness;           /**< The constness of the action */
  Ptr_t<SourceCode_t>      m_defaultcode;         /**< The default C implementation of the action */
  Vect_t<Comment_t>        m_comments;            /**< The list of the C comment associated with the action prototype */
  FileLoc_t                m_fileloc;             /**< The file location where the action prototype was declared */
  
  ActionProto_t( type_t _type, ConstStr_t _symbol, SourceCode_t* _returns, Vect_t<CodePair_t>& _params,
                 bool _constness, SourceCode_t* _defaultcode, Vect_t<Comment_t>& _comments, FileLoc_t const& _fileloc );
  ~ActionProto_t();
  
  char const*              returntype() const;
};

std::ostream& operator<<( std::ostream& _sink, ActionProto_t const& _ap );

#endif // __ACTION_HH__
