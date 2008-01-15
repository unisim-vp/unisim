/***************************************************************************
                                   generator.hh
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

#ifndef __GENERATOR_HH__
#define __GENERATOR_HH__

#include <fwd.hh>
#include <conststr.hh>

struct Generator {
  Isa*                                m_isa;
  int                                 m_minwordsize;
  
  enum Exception_t { GenerationError };
  Generator();
  virtual ~Generator() {};
  
  Generator&                          init( Isa& _isa );

  virtual void                        finalize() = 0;

  void                                iss( Product_t& _product ) const;
  /* header file */
  void                                decoder_decl( Product_t& _product ) const;
  void                                operation_decl( Product_t& _product ) const;
  /* source file */
  void                                decoder_impl( Product_t& _product ) const;
  void                                operation_impl( Product_t& _product ) const;
  void                                isa_operations_decl( Product_t& _product ) const;
  void                                isa_operations_ctors( Product_t& _product ) const;
  void                                isa_operations_methods( Product_t& _product ) const;
  
  Isa const&                          isa() const { return *m_isa; }
  Isa&                                isa() { return *m_isa; }
  
  virtual void                        codetype_decl( Product_t& _product ) const = 0;
  virtual void                        codetype_impl( Product_t& _product ) const = 0;
  virtual ConstStr_t                  codetype_name() const = 0;
  virtual ConstStr_t                  codetype_ref() const = 0;
  virtual ConstStr_t                  codetype_constref() const = 0;
  virtual void                        insn_bits_code( Product_t& _product, Operation_t const& _op ) const = 0;
  virtual void                        insn_mask_code( Product_t& _product, Operation_t const& _op ) const = 0;
  virtual void                        insn_fetch_impl( Product_t& _product, char const* _codename ) const = 0;
  virtual void                        insn_fetch_protoargs( Product_t& _product ) const = 0;
  virtual ConstStr_t                  insn_id_expr( char const* _addrname ) const = 0;
  virtual void                        insn_match_ifexpr( Product_t& _product, char const* _code, char const* _mask, char const* _bits ) const = 0;
  virtual void                        insn_unchanged_expr( Product_t& _product, char const* _old, char const* _new ) const = 0;
  virtual void                        insn_decode_impl( Product_t& _product, Operation_t const& _op, char const* _codename, char const* _addrname ) const = 0;
  virtual void                        additional_impl_includes( Product_t& _product ) const = 0;
  virtual void                        additional_decl_includes( Product_t& _product ) const = 0;
};

#endif // __GENERATOR_HH__
