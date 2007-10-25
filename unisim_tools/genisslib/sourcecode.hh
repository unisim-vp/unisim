/***************************************************************************
                               sourcecode.hh
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

#ifndef __SOURCECODE_HH__
#define __SOURCECODE_HH__

#include <fwd.hh>
#include <conststr.hh>
#include <referencecounting.hh>

/** A C source code object */
struct SourceCode_t : virtual ReferenceCounter {
  ConstStr_t                  m_content;    /**< the string containing the C source code */
  ConstStr_t                  m_filename;   /**< the filename object where was found the C source code */
  int                         m_lineno;     /**< the line number where was found the C source code */
  
  SourceCode_t( ConstStr_t _content, ConstStr_t _filename, int _lineno );
  
  static SourceCode_t const*  s_last_srccode;
};

std::ostream& operator<<( std::ostream& _sink, SourceCode_t const& _sc );

/** A C/C++ Code object object */
struct CodePair_t : virtual ReferenceCounter {
  Ptr_t<SourceCode_t>        m_ctype;        /**< The C type of the parameter */
  Ptr_t<SourceCode_t>        m_csymbol;      /**< The C symbol of the parameter */
  
  CodePair_t( SourceCode_t* _ctype, SourceCode_t* _csymbol );
  ~CodePair_t();
};

std::ostream& operator<<( std::ostream& _sink, CodePair_t const& _cp );
#endif // __SOURCECODE_HH__
