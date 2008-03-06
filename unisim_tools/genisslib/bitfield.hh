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

/**
 * @brief A base bitfield Object
 * 
 * Base bitfield objects are inherited by various bitfield object
 * acting differently according to opcode decoding and data
 * extraction.
 *
 */
struct BitField_t : virtual ReferenceCounter {
  unsigned int          m_size;
  
  enum Exception_t { InternalError, };
  enum Type_t { Opcode, Operand, Unused, Separator, SubOp, SpecializedOperand };
  
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

/**
 *  @brief  An opcode bitfield 
 *  
 *  An opcode bitfield takes a mandatory value in opcode decoding. 
 *
 */

struct OpcodeBitField_t : public BitField_t {
  unsigned int          m_value;
  
  OpcodeBitField_t( unsigned int _size, unsigned int _value );
  OpcodeBitField_t( OpcodeBitField_t const& _src );
  
  Type_t                type() const { return Opcode; };
  OpcodeBitField_t*     clone() const { return new OpcodeBitField_t( *this ); }
  void                  fills( std::ostream& _sink ) const;
  bool                  hasopcode() const { return true; }
  uint64_t              bits() const { return m_value; };
};

/**
 *  @brief  An operand bitfield 
 *  
 *  An operand bitfield may take any value in opcode decoding. The
 *  value is extracted to a operation member variable.
 *
 */

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
  void                  fills( std::ostream& _sink ) const;
  ConstStr_t            symbol() const { return m_symbol; };
};

/**
 *  @brief  A significant-less bitfield
 *  
 *  A unused bitfield may take any value in opcode decoding. No value
 *  is extracted from this field.
 *  
 */

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
  bool                  m_rewind;
  
  SeparatorBitField_t( bool _rewind );
  SeparatorBitField_t( SeparatorBitField_t const& _src );
  
  Type_t                type() const { return Separator; };
  SeparatorBitField_t*  clone() const { return new SeparatorBitField_t( *this ); }
  void                  fills( std::ostream& _sink ) const;
};

/**
 *  @brief  A sub operation bitfield (useful only for CISC)
 *  
 *  A suboperation bitfield may take any value in opcode decoding. The
 *  content of the field is passed to a subdecoder for further
 *  process. The returned operation (from the sub decoder) is stored
 *  in an operation member.
 *  
 */

struct SubOpBitField_t : public BitField_t {
  ConstStr_t            m_symbol;
  SDInstance_t const*   m_sdinstance;
  
  SubOpBitField_t( ConstStr_t _symbol, SDInstance_t const* _sdinstance );
  SubOpBitField_t( SubOpBitField_t const& _src );

  Type_t                type() const { return SubOp; };
  ConstStr_t            symbol() const { return m_symbol; };
  SubOpBitField_t*      clone() const { return new SubOpBitField_t( *this ); }
  void                  fills( std::ostream& _sink ) const;
  unsigned int          minsize() const;
  unsigned int          maxsize() const;
};

/**
 *  @brief  A specialized operand bitfield 
 *  
 *  A specialized operand bitfield takes a mandatory value in opcode
 *  decoding. Though the value is constant, it is extracted to a
 *  operation member variable.
 *
 *  This special kind of bitfield may not be directly emited by the
 *  user. This field is generated at compile-time whenever the user
 *  instanciates a particular operation for specific values of the
 *  field.
 *
 */

struct SpOperandBitField_t : public BitField_t {
  ConstStr_t            m_symbol;
  int                   m_shift;
  unsigned int          m_size_modifier;
  bool                  m_sext;
  unsigned int          m_value;
  
  SpOperandBitField_t( OperandBitField_t const& _src, unsigned int _value );
  SpOperandBitField_t( SpOperandBitField_t const& _src );

  int                   wordsize() const;
  ConstStr_t            constval() const;
  
  Type_t                type() const { return SpecializedOperand; };
  SpOperandBitField_t*  clone() const { return new SpOperandBitField_t( *this ); }
  void                  fills( std::ostream& _sink ) const { throw InternalError; }
  ConstStr_t            symbol() const { return m_symbol; };
  bool                  hasopcode() const { return true; }
  uint64_t              bits() const { return m_value; };
};

#endif // __BITFIELD_HH__
