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

struct RiscGenerator : public Generator {
  struct OpCode_t {
    unsigned int                m_size;
    uint64_t                    m_mask;
    uint64_t                    m_bits;
    // Topology information
    OpCode_t*                   m_upper;
    intptr_t                    m_lowercount;
    
    OpCode_t() : m_size( 0 ), m_mask( 0 ), m_bits( 0 ), m_upper( 0 ), m_lowercount( 0 ) {}
    OpCode_t( unsigned int _size, uint64_t _mask, uint64_t _bits )
      : m_size( _size ), m_mask( _mask ), m_bits( _bits ), m_upper( 0 ), m_lowercount( 0 ) {}
    
    // Topology methods
    enum Location_t { Outside, Overlaps, Inside, Contains, Equal };
    Location_t                  locate( OpCode_t const& _oc ) const;
    void                        setupper( OpCode_t* _upper );
    void                        unsetupper();
  };
  typedef std::map<Operation_t const*,OpCode_t> OpCodes_t;
  
  OpCodes_t                     m_opcodes;
  unsigned int                  m_insn_maxsize;
  unsigned int                  m_insn_minsize;
  unsigned int                  m_insn_ctypesize;
  ConstStr_t                    m_insn_ctype;
  ConstStr_t                    m_insn_cpostfix;
  
  RiscGenerator();
  ~RiscGenerator() {};
  
  /* Risc specific Utilities */
  void                          bitsize( unsigned int minsize, unsigned int maxsize );
  OpCode_t const&               opcode( Operation_t const* _op ) const;
  OpCode_t&                     opcode( Operation_t const* _op );
  
  void                          finalize();
  void                          codetype_decl( Product_t& _product ) const;
  void                          codetype_impl( Product_t& _product ) const {}
  ConstStr_t                    codetype_name() const { return "CodeType"; }
  ConstStr_t                    codetype_ref() const { return "CodeType&"; }
  ConstStr_t                    codetype_constref() const { return "CodeType"; }
  void                          insn_bits_code( Product_t& _product, Operation_t const& _op ) const;
  void                          insn_mask_code( Product_t& _product, Operation_t const& _op ) const;
  void                          insn_fetch_impl( Product_t& _product, char const* _codename ) const;
  void                          insn_fetch_protoargs( Product_t& _product ) const;
  ConstStr_t                    insn_id_expr( char const* _addrname ) const;
  void                          insn_match_ifexpr( Product_t& _product, char const* _code, char const* _mask, char const* _bits ) const;
  void                          insn_unchanged_expr( Product_t& _product, char const* _old, char const* _new ) const;
  void                          insn_decode_impl( Product_t& _product, Operation_t const& _op, char const* _codename, char const* _addrname ) const;
  void                          additional_impl_includes( Product_t& _product ) const {}
  void                          additional_decl_includes( Product_t& _product ) const {}
  void                          subdecoder_bounds( Product_t& _product ) const;
  void                          insn_destructor_decl( Product_t& _product, Operation_t const& _op ) const {};
  void                          insn_destructor_impl( Product_t& _product, Operation_t const& _op ) const {};
  void                          op_getlen_decl( Product_t& _product ) const;
  void                          insn_getlen_decl( Product_t& _product, Operation_t const& _op ) const;
};

#endif // __RISCGENERATOR_HH__
