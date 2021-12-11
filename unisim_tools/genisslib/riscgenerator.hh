/***************************************************************************
                                   riscgenerator.hh
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

#ifndef __RISCGENERATOR_HH__
#define __RISCGENERATOR_HH__

#include <fwd.hh>
#include <generator.hh>
#include <conststr.hh>
#include <map>

struct RiscGenerator : public Generator
{
  unsigned int                  m_insn_ctypesize;
  ConstStr                      m_insn_ctype;
  ConstStr                      m_insn_cpostfix;
  
  RiscGenerator( Isa& _source, Opts const& _options );
  ~RiscGenerator() {}
  
  /* Risc specific Utilities */
  void                          finalize();
  void                          codetype_decl( Product& _product ) const;
  void                          codetype_impl( Product& _product ) const {}
  ConstStr                      codetype_name() const { return "CodeType"; }
  ConstStr                      codetype_ref() const { return "CodeType&"; }
  ConstStr                      codetype_constref() const { return "CodeType"; }
  void                          insn_bits_code( Product& _product, Operation const& _op ) const;
  void                          insn_mask_code( Product& _product, Operation const& _op ) const;
  ConstStr                      insn_id_expr( char const* _addrname ) const;
  void                          insn_match_ifexpr( Product& _product, char const* _code, char const* _mask, char const* _bits ) const;
  void                          insn_decode_impl( Product& _product, Operation const& _op, char const* _codename, char const* _addrname ) const;
  void                          insn_encode_impl( Product& _product, Operation const& _op, char const* _codename ) const;
  void                          additional_impl_includes( Product& _product ) const {}
  void                          additional_decl_includes( Product& _product ) const {}
  
  void                          insn_destructor_decl( Product& _product, Operation const& _op ) const {};
  void                          insn_destructor_impl( Product& _product, Operation const& _op ) const {};
  
  void                          op_getlen_decl( Product& _product ) const;
  void                          op_match( Product& _product, char const* codename ) const;
  void                          insn_getlen_decl( Product& _product, Operation const& _op ) const;
};

#endif // __RISCGENERATOR_HH__
