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
struct BitField : virtual ReferenceCounter
{
  enum Exception_t { InternalError, };
  
  BitField() {}
  virtual ~BitField() {}

  virtual ConstStr      getsymbol() const { return ConstStr(); }
  
  virtual bool          hasopcode() const { return false; }
  virtual uint64_t      bits() const { throw InternalError; return 0; }
  virtual uint64_t      mask() const { throw InternalError; return 0; };
  
  virtual BitField*     clone() const = 0;
  virtual int           cmp( BitField const& ) const = 0;
  virtual void          fills( std::ostream& _sink ) const = 0;
  
  virtual uintptr_t     sizes() const = 0;
  virtual void          sizes( unsigned int* _sizes ) const = 0;
  virtual unsigned int  minsize() const = 0;
  virtual unsigned int  maxsize() const = 0;
  static int            cmp( BitField const&, BitField const& );
};

std::ostream&
operator<<( std::ostream& _sink, BitField const& _bf );

struct FixedSizeBitField : public BitField
{
  unsigned int          size;
  
  FixedSizeBitField( unsigned int _size ) : size( _size ) {}
  virtual ~FixedSizeBitField() {}

  uint64_t              mask() const override { return (0xffffffffffffffffULL >> (64-size)); }
  
  int                   compare(FixedSizeBitField const& rhs) const { return int(size) - int(rhs.size); }
  uintptr_t             sizes() const override { return 1; }
  void                  sizes( unsigned int* _sizes ) const override { _sizes[0] = size; }
  unsigned int          minsize() const override { return size; }
  unsigned int          maxsize() const override { return size; }
};


/**
 *  @brief  An opcode bitfield 
 *  
 *  An opcode bitfield takes a mandatory value in opcode decoding. 
 *
 */

struct OpcodeBitField : public FixedSizeBitField
{
  unsigned int          value;
  
  OpcodeBitField( unsigned int _size, unsigned int _value );
  OpcodeBitField( OpcodeBitField const& _src );
  
  bool                  hasopcode() const override { return true; }
  uint64_t              bits() const override { return value; };
  
  int                   cmp(BitField const& rhs) const override { return compare(dynamic_cast<OpcodeBitField const&>(rhs)); }
  int                   compare(OpcodeBitField const& rhs) const
  {
    if (int delta = FixedSizeBitField::compare(rhs)) return delta;
    return value > rhs.value ? 1 : value < rhs.value ? -1 : 0;
  }
  OpcodeBitField*       clone() const override { return new OpcodeBitField( *this ); }
  void                  fills( std::ostream& _sink ) const override;
};

/**
 *  @brief  An operand bitfield 
 *  
 *  An operand bitfield may take any value in opcode decoding. The
 *  value is extracted to a operation member variable.
 *
 */

struct OperandBitField : public FixedSizeBitField
{
  ConstStr              symbol;
  int                   shift;
  unsigned int          size_modifier;
  bool                  sext;
  
  OperandBitField( unsigned int _size, ConstStr _symbol, int _shift, unsigned int _size_modifier, bool _sext );
  OperandBitField( OperandBitField const& _src );

  unsigned int          dstsize() const;
  
  ConstStr              getsymbol() const { return symbol; };

  int                   cmp( BitField const& rhs ) const override { return compare( dynamic_cast<OperandBitField const&>(rhs) ); }
  int                   compare( OperandBitField const& rhs ) const
  {
    if (int delta = FixedSizeBitField::compare(rhs)) return delta;
    if (int delta = symbol.cmp(rhs.symbol)) return delta;
    if (int delta = shift - rhs.shift) return delta;
    if (int delta = size_modifier - rhs.size_modifier) return delta;
    return sext - rhs.sext;
  }
  OperandBitField*      clone() const override { return new OperandBitField( *this ); }
  void                  fills( std::ostream& _sink ) const override;
};

/**
 *  @brief  A significant-less bitfield
 *  
 *  A unused bitfield may take any value in opcode decoding. No value
 *  is extracted from this field.
 *  
 */

struct UnusedBitField : public FixedSizeBitField
{
  UnusedBitField( unsigned int _size );
  UnusedBitField( UnusedBitField const& _src );

  int                   cmp(BitField const& rhs) const override { return compare(dynamic_cast<FixedSizeBitField const&>(rhs)); }
  UnusedBitField*       clone() const override { return new UnusedBitField( *this ); }
  void                  fills( std::ostream& _sink ) const override;
};

/**
 *  @brief  A separator bitfield (useful only for CISC)
 *  
 *  A separator bitfield used to separate word in byte decoding of
 *  CISC decoders. An optional rewind properties allow to rewind in
 *  bytes of the previous word.
 *  
 */

struct SeparatorBitField : public BitField
{
  bool                  rewind;
  
  SeparatorBitField( bool _rewind );
  SeparatorBitField( SeparatorBitField const& _src );

  int                   cmp(BitField const& rhs) const override { return compare(dynamic_cast<SeparatorBitField const&>(rhs)); }
  int                   compare(SeparatorBitField const& rhs) const { return rewind - rhs.rewind; }
  SeparatorBitField*    clone() const override { return new SeparatorBitField( *this ); }
  void                  fills( std::ostream& _sink ) const override;

  uintptr_t             sizes() const override { return 1; }
  void                  sizes( unsigned int* _sizes ) const override { _sizes[0] = 0; }
  unsigned int          minsize() const override { return 0; }
  unsigned int          maxsize() const override { return 0; }
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

struct SubOpBitField : public BitField
{
  ConstStr              symbol;
  SDInstance const*     sdinstance;
  
  SubOpBitField( ConstStr _symbol, SDInstance const* _sdinstance );
  SubOpBitField( SubOpBitField const& _src );

  ConstStr              getsymbol() const { return symbol; };
  
  int                   cmp(BitField const& rhs) const override { return compare(dynamic_cast<SubOpBitField const&>(rhs)); }
  int                   compare(SubOpBitField const& rhs) const
  {
    if (int delta = symbol.cmp(rhs.symbol)) return delta;
    return sdinstance > rhs.sdinstance ? 1 : sdinstance < rhs.sdinstance ? -1 : 0;
  }
  SubOpBitField*        clone() const override { return new SubOpBitField( *this ); }
  void                  fills( std::ostream& _sink ) const override;
  
  uintptr_t             sizes() const override;
  void                  sizes( unsigned int* _sizes ) const override;
  unsigned int          minsize() const override;
  unsigned int          maxsize() const override;
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

struct SpOperandBitField : public FixedSizeBitField
{
  ConstStr              symbol;
  int                   shift;
  unsigned int          size_modifier;
  bool                  sext;
  unsigned int          value;
  
  SpOperandBitField( OperandBitField const& _src, unsigned int _value );
  SpOperandBitField( SpOperandBitField const& _src );

  unsigned int          dstsize() const;
  ConstStr              constval() const;
  
  ConstStr              getsymbol() const { return symbol; };

  bool                  hasopcode() const override { return true; }
  uint64_t              bits() const override { return value; };
  
  int                   cmp(BitField const& rhs) const override { return compare(dynamic_cast<SpOperandBitField const&>(rhs)); }
  int                   compare(SpOperandBitField const& rhs) const
  {
    if (int delta = symbol.cmp(rhs.symbol)) return delta;
    if (int delta = shift - rhs.shift) return delta;
    if (int delta = size_modifier - rhs.size_modifier) return delta;
    if (int delta = sext - rhs.sext) return delta;
    return value > rhs.value ? 1 : value < rhs.value ? -1 : 0;
  }
  SpOperandBitField*    clone() const override { return new SpOperandBitField( *this ); }
  void                  fills( std::ostream& _sink ) const override { throw InternalError; }
};

#endif // __BITFIELD_HH__
