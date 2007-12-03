/***************************************************************************
                                 bitfield.hh
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

#ifndef __BITFIELD_HH__
#define __BITFIELD_HH__

#include <fwd.hh>
#include <iosfwd>

#include <conststr.hh>
#include <referencecounting.hh>

/** bitfield base object, either an opcode bitfield, an operand or a unused bitfield */
struct BitField_t : virtual ReferenceCounter {
  unsigned int          m_size;
  
  enum Exception_t { InternalError, };
  enum Type_t { Opcode, Operand, Unused, Separator, SubOp };
  
  BitField_t( unsigned int _size ) : m_size( _size ) {}
  virtual ~BitField_t() {}
  virtual Type_t        type() const = 0;
  virtual BitField_t*   clone() const = 0;
  virtual void          fills( std::ostream& _sink ) const = 0;
  virtual ConstStr_t    symbol() const { return 0; }
  virtual bool          hasopcode() const { return false; }
  virtual uint64_t      bits() const { throw InternalError; return 0; }
  uint64_t              mask() const { return (0xffffffffffffffffULL >> (64-m_size)); }
  virtual unsigned int  minsize() const { return m_size; }
  virtual unsigned int  maxsize() const { return m_size; }
};

std::ostream&
operator<<( std::ostream& _sink, BitField_t const& _bf );

struct OpcodeBitField_t : public BitField_t {
  unsigned int          m_value;
  
  OpcodeBitField_t( unsigned int _size, unsigned int _value );
  OpcodeBitField_t( OpcodeBitField_t const& _src );
  
  Type_t                type() const { return Opcode; };
  OpcodeBitField_t*     clone() const { return new OpcodeBitField_t( *this ); }
  void                  fills( std::ostream& _sink ) const;
  uint64_t              bits() const { return m_value; };
  bool                  hasopcode() const { return true; }
};

struct OperandBitField_t : public BitField_t {
  ConstStr_t            m_symbol;
  int                   m_shift;
  unsigned int          m_size_modifier;
  bool                  m_sext;
  
  OperandBitField_t( unsigned int _size, ConstStr_t _symbol, int _shift, unsigned int _size_modifier, bool _sext );
  OperandBitField_t( OperandBitField_t const& _src );

  int                   wordsize() const;
  
  Type_t                type() const { return Operand; };
  OperandBitField_t*    clone() const { return new OperandBitField_t( *this ); }
  ConstStr_t            symbol() const { return m_symbol; };
  void                  fills( std::ostream& _sink ) const;
};

struct UnusedBitField_t : public BitField_t {
  UnusedBitField_t( unsigned int _size );
  UnusedBitField_t( UnusedBitField_t const& _src );
  
  Type_t                type() const { return Unused; };
  UnusedBitField_t*     clone() const { return new UnusedBitField_t( *this ); }
  void                  fills( std::ostream& _sink ) const;
};

/**
 *  @brief  A separator bitfield (useful only for CISC)
 *  
 *  A separator bitfield used to separate word in byte decoding of
 *  CISC decoders. An optional rewind properties allow to rewind in
 *  bytes of the previous word.
 *  
 */

struct SeparatorBitField_t : public BitField_t {
  unsigned int          m_rewind;
  
  SeparatorBitField_t( unsigned int m_rewind );
  SeparatorBitField_t( SeparatorBitField_t const& _src );
  
  Type_t                type() const { return Separator; };
  SeparatorBitField_t*  clone() const { return new SeparatorBitField_t( *this ); }
  void                  fills( std::ostream& _sink ) const;
};

struct SubOpBitField_t : public BitField_t {
  ConstStr_t            m_symbol;
  SubDecoder_t const*   m_subdecoder;
  
  SubOpBitField_t( ConstStr_t _symbol, SubDecoder_t const* _subdecoder );
  SubOpBitField_t( SubOpBitField_t const& _src );

  Type_t                type() const { return SubOp; };
  ConstStr_t            symbol() const { return m_symbol; };
  SubOpBitField_t*      clone() const { return new SubOpBitField_t( *this ); }
  void                  fills( std::ostream& _sink ) const;
  unsigned int          minsize() const;
  unsigned int          maxsize() const;
};


#endif // __BITFIELD_HH__
