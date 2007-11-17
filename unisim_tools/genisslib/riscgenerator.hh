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
  unsigned int                  m_codebitsize;
  unsigned int                  m_codesize;
  ConstStr_t                    m_codetype;
  ConstStr_t                    m_immedsuffix;
  
  struct OpCode_t {
    unsigned int                m_size;
    uint64_t                    m_mask;
    uint64_t                    m_bits;
    OpCode_t() : m_size( 0 ), m_mask( 0 ), m_bits( 0 ) {}
    OpCode_t( unsigned int _size, uint64_t _mask, uint64_t _bits )
      : m_size( _size ), m_mask( _mask ), m_bits( _bits ) {}
  };
  typedef std::map<Operation_t const*,OpCode_t> OpCodes_t;
  OpCodes_t                     m_opcodes;
  
  RiscGenerator( Isa const* _isa );
  ~RiscGenerator();
  
  bool                          finalize();
  bool                          sanity_checks() const;
  bool                          generate_iss( Product_t& _product, unsigned int word_size ) const;
  
  unsigned int                  get_max_operation_size() const;
  void                          generate_decoder_class_definition( Product_t& _product ) const;
  void                          generate_operation_class_definition( Product_t& _product, unsigned int word_size ) const;
  void                          generate_operation_impl_class_definition( Product_t& _product, unsigned int word_size ) const;
  void                          generate_operation_constructor( Product_t& _product ) const;
  void                          generate_operation_destructor( Product_t& _product ) const;
  void                          generate_default_actions( Product_t& _product ) const;
  void                          generate_operation_member_methods( Product_t& _product, unsigned int word_size ) const;
  void                          generate_operation_constructors( Product_t& _product, unsigned int word_size ) const;
  void                          generate_decoder( Product_t& _product, unsigned int word_size ) const;
  
  OpCode_t const&               opcode( Operation_t const* _op ) const { return m_opcodes.at( _op ); }
  OpCode_t&                     opcode( Operation_t const* _op ) { return m_opcodes.at( _op ); }
};

#endif // __RISCGENERATOR_HH__
