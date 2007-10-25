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

// /** bitfield base object, either an opcode bitfield, an operand or a unused bitfield */
struct BitField_t : virtual ReferenceCounter {
  unsigned int          m_size;
  
  enum Exception_t { InternalError, };
  BitField_t( unsigned int _size ) : m_size( _size ) {}
  virtual ~BitField_t() {}
  virtual BitField_t*   clone() const = 0;
  virtual void          fills( std::ostream& _sink ) const = 0;
  virtual ConstStr_t    symbol() const { return 0; }
  virtual bool          hasopcode() const { return false; }
  virtual uint64_t      opcode() const { throw InternalError; return 0; }
  uint64_t              mask() const { return (uint64_t( 1 ) << m_size)-1; }
  virtual unsigned int  operand_size() const { return 0; }
  virtual char const*   ctype( unsigned int _minsize ) const { return 0; }
  virtual ConstStr_t    cexpr_opaccess( unsigned int _minsize, unsigned int _shift, char const* _codename ) const { return 0; }
};

std::ostream&
operator<<( std::ostream& _sink, BitField_t const& _bf );

struct OpcodeBitField_t : public BitField_t {
  unsigned int          m_value;
  
  OpcodeBitField_t( unsigned int _size, unsigned int _value );
  OpcodeBitField_t( OpcodeBitField_t const& _src );
  
  OpcodeBitField_t*     clone() const { return new OpcodeBitField_t( *this ); }
  void                  fills( std::ostream& _sink ) const;
  uint64_t              opcode() const { return m_value; };
  bool                  hasopcode() const { return true; }
};

struct OperandBitField_t : public BitField_t {
  ConstStr_t            m_symbol;
  int                   m_shift;
  unsigned int          m_size_modifier;
  bool                  m_sext;
  
  OperandBitField_t( unsigned int _size, ConstStr_t _symbol, int _shift, unsigned int _size_modifier, bool _sext );
  OperandBitField_t( OperandBitField_t const& _src );
  
  OperandBitField_t*    clone() const { return new OperandBitField_t( *this ); }
  ConstStr_t            symbol() const { return m_symbol; };
  void                  fills( std::ostream& _sink ) const;
  unsigned int          operand_size() const { return m_size_modifier > m_size ? m_size_modifier : m_size; }
  char const*           ctype( unsigned int _minsize ) const;
  ConstStr_t            cexpr_opaccess( unsigned int _minsize, unsigned int _shift, char const* _codename ) const;
};

struct UnusedBitField_t : public BitField_t {
  UnusedBitField_t( unsigned int _size );
  UnusedBitField_t( UnusedBitField_t const& _src );
  
  UnusedBitField_t*     clone() const { return new UnusedBitField_t( *this ); }
  void                  fills( std::ostream& _sink ) const;
};

#endif // __BITFIELD_HH__
