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
#include <map>
#include <set>
#include <iosfwd>
#include <inttypes.h>

struct Generator
{
  enum Error { GenerationError };
  
  Isa&                                source;
  Opts const&                         options;
  
  unsigned int                        m_minwordsize;
  
  Generator( Isa& _source, Opts const& _options );
  virtual ~Generator() {};
  
  Generator&                          init( unsigned int verblevel );
  virtual void                        finalize() = 0;
  
  void                                toposort();
  void                                isastats();
  void                                iss() const;
  /* header file */
  void                                decoder_decl( Product& _product ) const;
  void                                operation_decl( Product& _product ) const;
  /* source file */
  void                                decoder_impl( Product& _product ) const;
  void                                operation_impl( Product& _product ) const;
  void                                isa_operations_decl( Product& _product ) const;
  void                                isa_operations_ctors( Product& _product ) const;
  void                                isa_operations_encoders( Product& _product ) const;
  void                                isa_operations_methods( Product& _product ) const;
  
  unsigned int                        gcd() const;
  
  virtual void                        codetype_decl( Product& _product ) const = 0;
  virtual void                        codetype_impl( Product& _product ) const = 0;
  virtual ConstStr                  codetype_name() const = 0;
  virtual ConstStr                  codetype_ref() const = 0;
  virtual ConstStr                  codetype_constref() const = 0;
  virtual void                        insn_bits_code( Product& _product, Operation const& _op ) const = 0;
  virtual void                        insn_mask_code( Product& _product, Operation const& _op ) const = 0;
  virtual ConstStr                  insn_id_expr( char const* _addrname ) const = 0;
  virtual void                        insn_match_ifexpr( Product& _product, char const* _code, char const* _mask, char const* _bits ) const = 0;
  virtual void                        insn_unchanged_expr( Product& _product, char const* _old, char const* _new ) const = 0;
  virtual void                        insn_decode_impl( Product& _product, Operation const& _op, char const* _codename, char const* _addrname ) const = 0;
  virtual void                        insn_encode_impl( Product& _product, Operation const& _op, char const* _codename ) const = 0;
  virtual void                        additional_impl_includes( Product& _product ) const = 0;
  virtual void                        additional_decl_includes( Product& _product ) const = 0;
  
  virtual void                        insn_destructor_decl( Product& _product, Operation const& _op ) const = 0;
  virtual void                        insn_destructor_impl( Product& _product, Operation const& _op ) const = 0;
  virtual void                        op_getlen_decl( Product& _product ) const = 0;
  virtual void                        insn_getlen_decl( Product& _product, Operation const& _op ) const = 0;
  
  static unsigned int                 least_ctype_size( unsigned int bits );
  
  unsigned                            membersize( unsigned size ) const;
  template <typename T>
  unsigned                            membersize( T const& op ) const { return this->membersize( unsigned( op.dstsize() ) ); }
};

#endif // __GENERATOR_HH__
