/***************************************************************************
                                 variable.hh
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

#ifndef __VARIABLE_HH__
#define __VARIABLE_HH__

#include <fwd.hh>
#include <iosfwd>

#include <conststr.hh>
#include <referencecounting.hh>
#include <vector>

/** A variable object */
struct Variable_t : virtual ReferenceCounter {
  ConstStr_t             m_symbol; /**< the symbol object representing the variable */
  Ptr_t<SourceCode_t>    m_ctype; /**< the C type of the variable */
  Ptr_t<SourceCode_t>    m_cinit; /**< the C expression used to initialized the variable */

  Variable_t( ConstStr_t _symbol, SourceCode_t* _ctype, SourceCode_t* _cinit );
  Variable_t( Variable_t const& _variable );
  ~Variable_t();
};

std::ostream& operator<<( std::ostream& _sink, Variable_t const& _var );

struct Inheritance_t : virtual ReferenceCounter {
  Ptr_t<SourceCode_t>           m_modifier;
  Ptr_t<SourceCode_t>           m_typename;
  Ptr_t<SourceCode_t>           m_initargs;
  
  Inheritance_t( SourceCode_t* _modifier, SourceCode_t* _typename, SourceCode_t* _initargs );
  Inheritance_t( Inheritance_t const& _variable );
  ~Inheritance_t();
};

#endif // __VARIABLE_HH__
