/***************************************************************************
                                 operation.hh
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

#ifndef __OPERATION_HH__
#define __OPERATION_HH__

#include <inttypes.h>
#include <fwd.hh>
#include <conststr.hh>
#include <errtools.hh>
#include <referencecounting.hh>
#include <vect.hh>
#include <iosfwd>

/** An operation object */
struct Operation_t : virtual ReferenceCounter
{
  ConstStr                      m_symbol;            /**< The associated symbol */
  Vect_t<BitField>            m_bitfields;         /**< The bit field list of the operation */
  Vect_t<Action>                m_actions;           /**< The list of actions of the operation */
  Vect_t<Comment_t>             m_comments;          /**< The list of the C comment associated with the operation */
  Vect_t<Variable_t>            m_variables;         /**< The list of variables associated with the operation */
  Ptr_t<SourceCode_t>           m_condition;         /**< The condition associated with the operation */
  FileLoc_t                     m_fileloc;           /**< The file location where the operation was declared */
  
  Operation_t( ConstStr _symbol, Vect_t<BitField>& _bitfields, Vect_t<Comment_t>& _comments,
               SourceCode_t* _op_condition, FileLoc_t const& _fileloc );
  ~Operation_t();
  
  void                          add( Action* _action );
  Action const*                 action( ActionProto const* _actionproto ) const;
};

std::ostream& operator<<( std::ostream& _sink, Operation_t const& _op );

/** A group object */
struct Group_t : virtual ReferenceCounter
{
  ConstStr                    m_symbol;         /**< The associated symbol */
  Vect_t<Operation_t>           m_operations;     /**< an operation node list object containing the operations of the group */
  FileLoc_t                     m_fileloc;        /**< The file location where the group was declared */

  Group_t( ConstStr _symbol, Vect_t<Operation_t>& _operations, FileLoc_t const& _fileloc );
  Group_t( ConstStr _symbol, FileLoc_t const& _fileloc );
};

#endif // __OPERATION_HH__
