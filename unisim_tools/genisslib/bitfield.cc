/***************************************************************************
                                 bitfield.cc
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

#include <bitfield.hh>
#include <ostream>
#include <strtools.hh>

using namespace std;

/** Create an opcode bitfield object
    @param _size size in bits
    @param _value value of the opcode
*/
OpcodeBitField_t::OpcodeBitField_t( unsigned int _size, unsigned int _value )
  : BitField_t( _size ), m_value( _value )
{}

/** Create an opcode bitfield object (copy constructor)
    @param _src the bitfield object to copy
*/
OpcodeBitField_t::OpcodeBitField_t( OpcodeBitField_t const& _src )
  : BitField_t( _src.m_size ), m_value( _src.m_value )
{}

/** Dump an opcode bitfield object into a stream
    @param _sink a stream
*/
void
OpcodeBitField_t::fills( ostream& _sink ) const {
  _sink << Str::fmt( "0x%x[%u]", m_value, m_size );
}

/** Create an operand bitfield object
    @param _size size in bits
    @param _symbol a symbol object representing the operand
    @param _shift a shift amount to do on the bitfield
    @param _size_modifier minimum bit size for holding the operand bitfield
    @param _sext true if a sign extension is required
*/
OperandBitField_t::OperandBitField_t( unsigned int _size, ConstStr_t _symbol, int _shift, unsigned int _size_modifier, bool _sext )
  : BitField_t( _size ), m_symbol( _symbol ), m_shift( _shift ), m_size_modifier( _size_modifier ), m_sext( _sext )
{}

/** Create an operand bitfield object (copy constructor)
    @param _src the source bitfield object to copy
*/
OperandBitField_t::OperandBitField_t( OperandBitField_t const& _src )
  : BitField_t( _src.m_size ),
    m_symbol( _src.m_symbol ), m_shift( _src.m_shift ), m_size_modifier( _src.m_size_modifier ), m_sext( _src.m_sext )
{}

/** Dump an operand bitfield object into a stream
    @param _sink a stream
*/
void
OperandBitField_t::fills( ostream& _sink ) const {
  _sink << ( m_sext ? "sext" : "" );
  
  if( m_size_modifier )
    _sink << '<' << m_size_modifier << '>';

  if( m_sext or m_size_modifier )
    _sink << " ";

  _sink << Str::fmt( "%s[%u]", m_symbol.str(), m_size );
}

/** Returns the type name of the bitfield operand storage
    @param _minsize minimum size for an operand storage
*/
char const*
OperandBitField_t::ctype( unsigned int _minsize ) const {
  unsigned int size = std::max( _minsize, std::max( m_size, m_size_modifier ) );
  
  if( m_sext ) {
    if( size <= 8 ) return "int8_t";
    if( size <= 16 ) return "int16_t";
    if( size <= 32 ) return "int32_t";
    if( size <= 64 ) return "int64_t";
  }
  else {
    if( size <= 8 ) return "uint8_t";
    if( size <= 16 ) return "uint16_t";
    if( size <= 32 ) return "uint32_t";
    if( size <= 64 ) return "uint64_t";
  }
  return "bigint_t";
}

/** Computes the code requiered to extract the data from operand source
    @param _minsize the minimum size used for operand storage
    @param _shift the shift of the current bitfield
    @param _codename the name of the code C variable used by the generator
    @return the C code
*/
ConstStr_t
OperandBitField_t::cexpr_opaccess( unsigned int _minsize, unsigned int _shift, char const* _codename ) const {
  ConstStr_t begin;
  
  if( m_sext ) {
    unsigned int type_size = operand_size();
    type_size = _minsize > type_size ? _minsize : type_size;
    unsigned int sign_shift = type_size - m_size;
    // FIXME: the first cast should apply to the shifted instruction (not to the instruction itself)
    begin = Str::fmt( "(((((%s)(%s >> %u)) & 0x%llx) << %u) >> %u)",
                      ctype( _minsize ), _codename, _shift, mask(), sign_shift, sign_shift );
  } else {
    begin = Str::fmt( "((%s >> %u) & 0x%llx)", _codename, _shift, mask() );
  }
  
  if( m_shift > 0 )
    return Str::fmt( "%s >> %u", begin.str(), m_shift  );
  if( m_shift < 0 )
    return Str::fmt( "%s << %u", begin.str(), -m_shift );
  return begin;
}

/** Create an unused bitfield object
    @param _size size in bits
*/
UnusedBitField_t::UnusedBitField_t( unsigned int _size )
  : BitField_t( _size )
{}

/** Create an unused bitfield object (copy constructor)
    @param _src the source bitfield object to copy
*/
UnusedBitField_t::UnusedBitField_t( UnusedBitField_t const& _src )
  : BitField_t( _src.m_size )
{}

/** Dump an unused bitfield object into a stream
    @param _sink a stream
*/
void
UnusedBitField_t::fills( ostream& _sink ) const {
  _sink << "?[" << m_size << "]";
}

/** Dump an bitfield object into a stream
    @param _sink a stream
*/
std::ostream&
operator<<( std::ostream& _sink, BitField_t const& _bf ) {
  _bf.fills( _sink );
  return _sink;
}
