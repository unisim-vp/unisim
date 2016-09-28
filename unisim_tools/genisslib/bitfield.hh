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
#include <vector>

/**
 * @brief A base bitfield Object
 * 
 * Base bitfield objects are inherited by various bitfield object
 * acting differently according to opcode decoding and data
 * extraction.
 *
 */
struct BitField_t : virtual ReferenceCounter
{
  enum Exception_t { InternalError, };
  
  BitField_t() {}
  virtual ~BitField_t() {}

  virtual ConstStr    symbol() const { return ConstStr(); }
  
  virtual bool          hasopcode() const { return false; }
  virtual uint64_t      bits() const { throw InternalError; return 0; }
  virtual uint64_t      mask() const { throw InternalError; return 0; };
  
  virtual BitField_t*   clone() const = 0;
  virtual void          fills( std::ostream& _sink ) const = 0;
  
  virtual uintptr_t     sizes() const = 0;
  virtual void          sizes( unsigned int* _sizes ) const = 0;
  virtual unsigned int  minsize() const = 0;
  virtual unsigned int  maxsize() const = 0;
};

std::ostream&
operator<<( std::ostream& _sink, BitField_t const& _bf );

struct FixedSizeBitField_t : public BitField_t
{
  unsigned int          m_size;
  
  FixedSizeBitField_t( unsigned int _size ) : m_size( _size ) {}
  virtual ~FixedSizeBitField_t() {}

  uint64_t              mask() const { return (0xffffffffffffffffULL >> (64-m_size)); }
  
  uintptr_t             sizes() const { return 1; }
  void                  sizes( unsigned int* _sizes ) const { _sizes[0] = m_size; }
  unsigned int          minsize() const { return m_size; }
  unsigned int          maxsize() const { return m_size; }
};


/**
 *  @brief  An opcode bitfield 
 *  
 *  An opcode bitfield takes a mandatory value in opcode decoding. 
 *
 */

struct OpcodeBitField_t : public FixedSizeBitField_t
{
  unsigned int          m_value;
  
  OpcodeBitField_t( unsigned int _size, unsigned int _value );
  OpcodeBitField_t( OpcodeBitField_t const& _src );
  
  bool                  hasopcode() const { return true; }
  uint64_t              bits() const { return m_value; };
  
  OpcodeBitField_t*     clone() const { return new OpcodeBitField_t( *this ); }
  void                  fills( std::ostream& _sink ) const;
};

/**
 *  @brief  An operand bitfield 
 *  
 *  An operand bitfield may take any value in opcode decoding. The
 *  value is extracted to a operation member variable.
 *
 */

struct OperandBitField_t : public FixedSizeBitField_t
{
  ConstStr            m_symbol;
  int                   m_shift;
  unsigned int          m_size_modifier;
  bool                  m_sext;
  
  OperandBitField_t( unsigned int _size, ConstStr _symbol, int _shift, unsigned int _size_modifier, bool _sext );
  OperandBitField_t( OperandBitField_t const& _src );

  unsigned int          dstsize() const;
  
  ConstStr            symbol() const { return m_symbol; };
  
  OperandBitField_t*    clone() const { return new OperandBitField_t( *this ); }
  void                  fills( std::ostream& _sink ) const;
};

/**
 *  @brief  A significant-less bitfield
 *  
 *  A unused bitfield may take any value in opcode decoding. No value
 *  is extracted from this field.
 *  
 */

struct UnusedBitField_t : public FixedSizeBitField_t
{
  UnusedBitField_t( unsigned int _size );
  UnusedBitField_t( UnusedBitField_t const& _src );
  
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

struct SeparatorBitField_t : public BitField_t
{
  bool                  m_rewind;
  
  SeparatorBitField_t( bool _rewind );
  SeparatorBitField_t( SeparatorBitField_t const& _src );
  
  SeparatorBitField_t*  clone() const { return new SeparatorBitField_t( *this ); }
  void                  fills( std::ostream& _sink ) const;

  uintptr_t             sizes() const { return 1; }
  void                  sizes( unsigned int* _sizes ) const { _sizes[0] = 0; }
  unsigned int          minsize() const { return 0; }
  unsigned int          maxsize() const { return 0; }
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

struct SubOpBitField_t : public BitField_t
{
  ConstStr            m_symbol;
  SDInstance_t const*   m_sdinstance;
  
  SubOpBitField_t( ConstStr _symbol, SDInstance_t const* _sdinstance );
  SubOpBitField_t( SubOpBitField_t const& _src );

  ConstStr            symbol() const { return m_symbol; };
  
  SubOpBitField_t*      clone() const { return new SubOpBitField_t( *this ); }
  void                  fills( std::ostream& _sink ) const;
  
  uintptr_t             sizes() const;
  void                  sizes( unsigned int* _sizes ) const;
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

struct SpOperandBitField_t : public FixedSizeBitField_t
{
  ConstStr            m_symbol;
  int                   m_shift;
  unsigned int          m_size_modifier;
  bool                  m_sext;
  unsigned int          m_value;
  
  SpOperandBitField_t( OperandBitField_t const& _src, unsigned int _value );
  SpOperandBitField_t( SpOperandBitField_t const& _src );

  unsigned int          dstsize() const;
  ConstStr            constval() const;
  
  ConstStr            symbol() const { return m_symbol; };

  bool                  hasopcode() const { return true; }
  uint64_t              bits() const { return m_value; };
  
  SpOperandBitField_t*  clone() const { return new SpOperandBitField_t( *this ); }
  void                  fills( std::ostream& _sink ) const { throw InternalError; }
};

#endif // __BITFIELD_HH__
