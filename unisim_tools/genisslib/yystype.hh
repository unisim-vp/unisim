/***************************************************************************
          GenISSLib.h  -  Instruction Set Simulator Library Generator
                             -------------------
    begin                : Thu May 25 2003
    copyright            : (C) 2003 CEA and Universite Paris Sud
    author               : Gilles Mouchard
    email                : mouchard@lri.fr
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef __GENISSLIB_H__
#define __GENISSLIB_H__

#include <fwd.hh>

/** The type of the token, either terminal or not terminal */
union  yylval_t {
  char const*                  volatile_string;        /**< a volatile C-String */
  char const*                  persistent_string;      /**< a persistent C-String */
  signed int                   sinteger;               /**< A signed integer value */
  unsigned int                 uinteger;               /**< An unsigned integer value */
  bool                         boolean;                /**< A boolean value */
  SourceCode_t*                sourcecode;             /**< A C source code */
  CodePair_t*                  param;                  /**< A parameter object */
  Vect_t<CodePair_t>*          param_list;             /**< A parameter list object */
  Operation_t*                 operation;              /**< An operation */
  Vect_t<Operation_t>*         operation_list;         /**< An operation node list object */
  Group_t*                     group;                  /**< A group object */
  ActionProto_t*               actionproto;            /**< An action prototype */
  Action_t*                    action;                 /**< An action */
  BitField_t*                  bitfield;               /**< A bit field */
  Vect_t<BitField_t>*          bitfield_list;          /**< A bit field list */
  Variable_t*                  variable;               /**< A variable object */
  Vect_t<Variable_t>*          variable_list;          /**< A variable list object */
};

/* define the type of yylval */
#define YYSTYPE yylval_t

#endif
