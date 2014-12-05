/***************************************************************************
                                   ciscgenerator.hh
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

#ifndef __CISCGENERATOR_HH__
#define __CISCGENERATOR_HH__

#include <fwd.hh>
#include <generator.hh>
#include <conststr.hh>
#include <map>
#include <iosfwd>

struct CiscOpCode_t : public OpCode_t
{
  uint8_t*                    m_mask;
  uint8_t*                    m_bits;
  unsigned int                m_prefixsize;
  unsigned int                m_fullsize;
  bool                        m_vlen;
    
  CiscOpCode_t( ConstStr_t _symbol, unsigned int _prefixsize, unsigned int _fullsize, bool _vlen );
  ~CiscOpCode_t() { delete [] m_mask; }
    
  bool                        match( CiscOpCode_t const& _oc ) const;
  void                        optimize( bool is_little_endian );
  unsigned int                maskbytesize() const { return (m_prefixsize+7)/8; };
  unsigned int                fullbytesize() const { return (m_fullsize+7)/8; };
    
  // Topology methods
  Location_t                  locate( OpCode_t const& _oc ) const;
  
  std::ostream&               details( std::ostream& _sink ) const;
};

struct CiscGenerator : public Generator
{
  unsigned int                  m_code_capacity;
  
  CiscGenerator();
  ~CiscGenerator() {};
  
  CiscOpCode_t const&           ciscopcode( Operation_t const* _op ) const { return dynamic_cast<CiscOpCode_t const&>( opcode( _op ) ); }
  CiscOpCode_t&                 ciscopcode( Operation_t const* _op ) { return dynamic_cast<CiscOpCode_t&>( opcode( _op ) ); };
  
  /* Cisc specific instructions */
  void                          finalize();
  void                          codetype_decl( Product_t& _product ) const;
  void                          codetype_impl( Product_t& _product ) const;
  ConstStr_t                    codetype_name() const { return "CodeType"; }
  ConstStr_t                    codetype_ref() const { return "CodeType&"; }
  ConstStr_t                    codetype_constref() const { return "CodeType const&"; }
  void                          insn_bits_code( Product_t& _product, Operation_t const& _op ) const;
  void                          insn_mask_code( Product_t& _product, Operation_t const& _op ) const;
  ConstStr_t                    insn_id_expr( char const* _addrname ) const { return _addrname; }
  void                          insn_match_ifexpr( Product_t& _product, char const* _code, char const* _mask, char const* _bits ) const;
  void                          insn_unchanged_expr( Product_t& _product, char const* _old, char const* _new ) const;
  void                          insn_decode_impl( Product_t& _product, Operation_t const& _op, char const* _codename, char const* _addrname ) const;
  void                          insn_encode_impl( Product_t& _product, Operation_t const& _op, char const* _codename ) const;
  void                          additional_decl_includes( Product_t& _product ) const;
  void                          additional_impl_includes( Product_t& _product ) const;

  void                          insn_destructor_decl( Product_t& _product, Operation_t const& _op ) const;
  void                          insn_destructor_impl( Product_t& _product, Operation_t const& _op ) const;

  void                          op_getlen_decl( Product_t& _product ) const;
  void                          insn_getlen_decl( Product_t& _product, Operation_t const& _op ) const { return; }
};

#endif // __CISCGENERATOR_HH__
