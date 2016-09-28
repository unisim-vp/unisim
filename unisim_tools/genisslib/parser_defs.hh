/***************************************************************************
                                  parser.hh
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

#ifndef __PARSER_DEFS_HH__
#define __PARSER_DEFS_HH__

#include <fwd.hh>

/** The type of the token, either terminal or not terminal */
union  yylval_t
{
  char const*                  volatile_string;        /**< a volatile C-String */
  char const*                  persistent_string;      /**< a persistent C-String */
  StringVect_t*                string_list;            /**< a C-String list */
  signed int                   sinteger;               /**< A signed integer value */
  unsigned int                 uinteger;               /**< An unsigned integer value */
  bool                         boolean;                /**< A boolean value */
  SourceCode_t*                sourcecode;             /**< A C source code */
  CodePair_t*                  param;                  /**< A parameter object */
  Vect_t<CodePair_t>*          param_list;             /**< A parameter list object */
  Operation_t*                 operation;              /**< An operation */
  Vect_t<Operation_t>*         operation_list;         /**< An operation node list object */
  Group_t*                     group;                  /**< A group object */
  ActionProto*                 actionproto;            /**< An action prototype */
  Action*                      action;                 /**< An action */
  BitField*                  bitfield;               /**< A bit field */
  Vect_t<BitField>*          bitfield_list;          /**< A bit field list */
  Variable_t*                  variable;               /**< A variable object */
  Vect_t<Variable_t>*          variable_list;          /**< A variable list object */
  Constraint_t*                constraint;             /**< A constraint object */
  Vect_t<Constraint_t>*        constraint_list;        /**< A constraint list object */
  Specialization_t*            specialization;         /**< A specialization object */
  Inheritance_t*               inheritance;            /**< An inheritance descriptor */
  UIntVect_t*                  uint_list;              /**< A unsigned integer list object */
};

/* define the type of yylval */
#define YYSTYPE yylval_t

/* include token definitions */
#include <parser_tokens.hh>

#endif // __PARSER_DEFS_HH__
