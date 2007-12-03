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
#include <errtools.hh>
#include <strtools.hh>
#include <vector>

struct Product_t {
  ConstStr_t          m_prefix;
  ConstStr_t          m_filename;
  std::ostream*       m_stream;
  Str::Buf            m_line;
  unsigned int        m_lineno;
  std::vector<int>    m_indentations;
  bool                m_sourcelines;
  
  Product_t( ConstStr_t _filename, bool _sourcelines );
  ~Product_t();
  
  bool                open( char const* _ext );
  void                close();
  std::ostream&       sink() { return *m_stream; };
  void                usercode( FileLoc_t const& _fileloc, char const* _format, ... );
  void                code( char const* _format, ... );
  void                template_signature( Vect_t<CodePair_t> const& _tparams );
  void                template_abbrev( Vect_t<CodePair_t> const& _tparams );
  void                ns_enter( std::vector<ConstStr_t> const& _namespace );
  void                ns_leave( std::vector<ConstStr_t> const& _namespace );
  void                require_newline();
  void                write( char const* _chars );
  void                flatten_indentation();
};

#endif // __PRODUCT_HH__
