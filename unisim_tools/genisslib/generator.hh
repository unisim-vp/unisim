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
#include <set>
#include <iosfwd>
#include <inttypes.h>

struct OpCode_t {
  // Topology methods
  enum Location_t { Outside, Overlaps, Inside, Contains, Equal };
  virtual Location_t          locate( OpCode_t const& _oc ) const = 0;
  void                        setbelow( OpCode_t* _below );
  void                        forcebelow( OpCode_t* _below );
  void                        unsetbelow();
  bool                        above( OpCode_t* below );
  // Topology information
  typedef std::set<OpCode_t*> BelowList;
  BelowList                   m_belowlist;
  intptr_t                    m_abovecount;
  // Debugging information
  ConstStr_t                  m_symbol;
  
  OpCode_t( ConstStr_t _symbol ) : m_abovecount( 0 ), m_symbol( _symbol ) {}
  virtual ~OpCode_t() {}
  
  virtual std::ostream&       details( std::ostream& _sink ) const = 0;
  friend std::ostream&        operator << ( std::ostream& _sink, OpCode_t const& _oc );
};

struct Generator
{
  enum Exception_t { GenerationError };
  
  Isa*                                m_isa;
  unsigned int                        m_minwordsize;
  std::set<unsigned int>              m_insnsizes;
  unsigned int                        m_verblevel;
  typedef std::map<Operation_t const*,OpCode_t*> OpCodeMap;
  OpCodeMap                           m_opcodes;
  
  Generator();
  virtual ~Generator() {};
  
  Generator&                          init( Isa& _isa, unsigned int verblevel );
  virtual void                        finalize() = 0;
  
  OpCode_t const&                     opcode( Operation_t const* _op ) const;
  OpCode_t&                           opcode( Operation_t const* _op );
  
  void                                toposort();
  void                                isastats();
  void                                iss( char const* prefix, bool sourcelines ) const;
  /* header file */
  void                                decoder_decl( Product_t& _product ) const;
  void                                operation_decl( Product_t& _product ) const;
  /* source file */
  void                                decoder_impl( Product_t& _product ) const;
  void                                operation_impl( Product_t& _product ) const;
  void                                isa_operations_decl( Product_t& _product ) const;
  void                                isa_operations_ctors( Product_t& _product ) const;
  void                                isa_operations_encoders( Product_t& _product ) const;
  void                                isa_operations_methods( Product_t& _product ) const;
  
  Isa const&                          isa() const { return *m_isa; }
  Isa&                                isa() { return *m_isa; }
  
  unsigned int                        gcd() const;
  
  virtual void                        codetype_decl( Product_t& _product ) const = 0;
  virtual void                        codetype_impl( Product_t& _product ) const = 0;
  virtual ConstStr_t                  codetype_name() const = 0;
  virtual ConstStr_t                  codetype_ref() const = 0;
  virtual ConstStr_t                  codetype_constref() const = 0;
  virtual void                        insn_bits_code( Product_t& _product, Operation_t const& _op ) const = 0;
  virtual void                        insn_mask_code( Product_t& _product, Operation_t const& _op ) const = 0;
  virtual ConstStr_t                  insn_id_expr( char const* _addrname ) const = 0;
  virtual void                        insn_match_ifexpr( Product_t& _product, char const* _code, char const* _mask, char const* _bits ) const = 0;
  virtual void                        insn_unchanged_expr( Product_t& _product, char const* _old, char const* _new ) const = 0;
  virtual void                        insn_decode_impl( Product_t& _product, Operation_t const& _op, char const* _codename, char const* _addrname ) const = 0;
  virtual void                        insn_encode_impl( Product_t& _product, Operation_t const& _op, char const* _codename ) const = 0;
  virtual void                        additional_impl_includes( Product_t& _product ) const = 0;
  virtual void                        additional_decl_includes( Product_t& _product ) const = 0;
  
  virtual void                        insn_destructor_decl( Product_t& _product, Operation_t const& _op ) const = 0;
  virtual void                        insn_destructor_impl( Product_t& _product, Operation_t const& _op ) const = 0;
  virtual void                        op_getlen_decl( Product_t& _product ) const = 0;
  virtual void                        insn_getlen_decl( Product_t& _product, Operation_t const& _op ) const = 0;
  
  static unsigned int                 least_ctype_size( unsigned int bits );
  
  unsigned                            membersize( unsigned size ) const;
  template <typename T>
  unsigned                            membersize( T const& op ) const { return this->membersize( unsigned( op.dstsize() ) ); }
  
  std::ostream&                       log( unsigned int level ) const;
};

struct FieldIterator {
  Vect_t<BitField_t> const& m_bitfields;
  unsigned int              m_idx;
  unsigned int              m_ref;
  unsigned int              m_pos, m_size;
  unsigned int              m_chkpt_pos, m_chkpt_size;
  
  FieldIterator( bool little_endian, Vect_t<BitField_t> const& bitfields, unsigned int maxsize );
  
  unsigned int      pos() { return m_pos; }
  unsigned int      insn_size() { return (m_ref == 0) ? m_pos + m_size : m_ref - m_pos; }
  BitField_t const& item();
  bool              next();
};

#endif // __GENERATOR_HH__
