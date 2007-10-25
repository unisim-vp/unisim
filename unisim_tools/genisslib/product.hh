/***************************************************************************
                                 product.hh
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

#ifndef __PRODUCT_HH__
#define __PRODUCT_HH__

#include <fwd.hh>
#include <iosfwd>
#include <conststr.hh>
#include <vector>

struct Product_t {
  ConstStr_t          m_prefix;
  ConstStr_t          m_filename;
  std::ostream*       m_stream;
  unsigned int        m_lineno;
  
  Product_t( ConstStr_t _filename );
  ~Product_t();
  
  bool                open( char const* _ext );
  void                close();
  std::ostream&       sink() { return *m_stream; };
  void                usercode( ConstStr_t _filename, unsigned int _lineno, char const* _format, ... );
  void                code( char const* _format, ... );
  void                cstring( char const* _string );
  //  void                change_namespace( Hierarchy_t const* target_namespace );
  void                template_signature( Vect_t<CodePair_t> const& _tparams );
  void                template_abrev( Vect_t<CodePair_t> const& _tparams );
  void                ns_enter( std::vector<ConstStr_t> const& _namespace );
  void                ns_leave( std::vector<ConstStr_t> const& _namespace );
};

#endif // __PRODUCT_HH__
