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
  ConstStr_t          m_filename;
  Str::Buf            m_line;
  unsigned int        m_lineno;
  std::vector<int>    m_indentations;
  bool                m_sourcelines;
  
  Product_t( ConstStr_t _filename, bool _sourcelines );
  virtual ~Product_t() {};
  
  Product_t&          usercode( FileLoc_t const& _fileloc, char const* _format, ... );
  Product_t&          usercode( SourceCode_t const& _source );
  Product_t&          usercode( SourceCode_t const& _source, char const* _fmt );
  Product_t&          code( char const* _format, ... );
  Product_t&          template_signature( Vect_t<CodePair_t> const& _tparams );
  Product_t&          template_abbrev( Vect_t<CodePair_t> const& _tparams );
  Product_t&          ns_enter( std::vector<ConstStr_t> const& _namespace );
  Product_t&          ns_leave( std::vector<ConstStr_t> const& _namespace );
  Product_t&          require_newline();
  Product_t&          write( char const* _chars );
  Product_t&          flatten_indentation();
  
  virtual void        xwrite( char const* chrs ) = 0;
};

struct FProduct_t : public Product_t {
  ConstStr_t          m_prefix;
  std::ostream*       m_stream;
  
  FProduct_t( ConstStr_t _prefix, bool _sourcelines );
  ~FProduct_t();
  
  bool                open( char const* _ext );
  void                close();
  std::ostream&       sink() { return *m_stream; };
  
  void                xwrite( char const* chrs );
};

#endif // __PRODUCT_HH__
