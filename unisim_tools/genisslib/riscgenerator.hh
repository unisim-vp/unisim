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

struct RiscOpCode_t : public OpCode_t
{
  uint64_t                    m_mask;
  uint64_t                    m_bits;
  unsigned int                m_size;
  bool                        m_vlen;
    
  // RiscOpCode_t()
  //   : OpCode_t( ConstStr_t() ), m_mask( 0 ), m_bits( 0 ), m_size( 0 ), m_vlen( false )
  // {}
  RiscOpCode_t( ConstStr_t _symbol, uint64_t mask, uint64_t bits, unsigned int size, bool vlen )
    : OpCode_t( _symbol ), m_mask( mask ), m_bits( bits ), m_size( size ), m_vlen( vlen )
  {}

  virtual ~RiscOpCode_t() {}

  // Topology methods
  Location_t                  locate( OpCode_t const& _oc ) const;
  
  std::ostream&               details( std::ostream& _sink ) const;
};

struct RiscGenerator : public Generator
{
  unsigned int                  m_insn_ctypesize;
  ConstStr_t                    m_insn_ctype;
  ConstStr_t                    m_insn_cpostfix;
  
  RiscGenerator();
  ~RiscGenerator() {};
  
  RiscOpCode_t const&           riscopcode( Operation_t const* _op ) const { return dynamic_cast<RiscOpCode_t const&>( opcode( _op ) ); }
  RiscOpCode_t&                 riscopcode( Operation_t const* _op ) { return dynamic_cast<RiscOpCode_t&>( opcode( _op ) ); };
  
  /* Risc specific Utilities */
  void                          finalize();
  void                          codetype_decl( Product_t& _product ) const;
  void                          codetype_impl( Product_t& _product ) const {}
  ConstStr_t                    codetype_name() const { return "CodeType"; }
  ConstStr_t                    codetype_ref() const { return "CodeType&"; }
  ConstStr_t                    codetype_constref() const { return "CodeType"; }
  void                          insn_bits_code( Product_t& _product, Operation_t const& _op ) const;
  void                          insn_mask_code( Product_t& _product, Operation_t const& _op ) const;
  ConstStr_t                    insn_id_expr( char const* _addrname ) const;
  void                          insn_match_ifexpr( Product_t& _product, char const* _code, char const* _mask, char const* _bits ) const;
  void                          insn_unchanged_expr( Product_t& _product, char const* _old, char const* _new ) const;
  void                          insn_decode_impl( Product_t& _product, Operation_t const& _op, char const* _codename, char const* _addrname ) const;
  void                          insn_encode_impl( Product_t& _product, Operation_t const& _op, char const* _codename ) const;
  void                          additional_impl_includes( Product_t& _product ) const {}
  void                          additional_decl_includes( Product_t& _product ) const {}
  
  void                          insn_destructor_decl( Product_t& _product, Operation_t const& _op ) const {};
  void                          insn_destructor_impl( Product_t& _product, Operation_t const& _op ) const {};
  
  void                          op_getlen_decl( Product_t& _product ) const;
  void                          insn_getlen_decl( Product_t& _product, Operation_t const& _op ) const;
};

#endif // __RISCGENERATOR_HH__
