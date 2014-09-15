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
#include <subdecoder.hh>
#include <scanner.hh>
#include <parser_defs.hh>
#include <cassert>

using namespace std;

/** Create an opcode bitfield object
    @param _size size in bits
    @param _value value of the opcode
*/
OpcodeBitField_t::OpcodeBitField_t( unsigned int _size, unsigned int _value )
  : FixedSizeBitField_t( _size ), m_value( _value )
{}

/** Create an opcode bitfield object (copy constructor)
    @param _src the bitfield object to copy
*/
OpcodeBitField_t::OpcodeBitField_t( OpcodeBitField_t const& _src )
  : FixedSizeBitField_t( _src.m_size ), m_value( _src.m_value )
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
  : FixedSizeBitField_t( _size ), m_symbol( _symbol ), m_shift( _shift ), m_size_modifier( _size_modifier ), m_sext( _sext )
{}

/** Create an operand bitfield object (copy constructor)
    @param _src the source bitfield object to copy
*/
OperandBitField_t::OperandBitField_t( OperandBitField_t const& _src )
  : FixedSizeBitField_t( _src.m_size ),
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

/** Return the size (in bits) of the target word encoded by this field.
    @return the size (in bits) of the target word encoded by this field.
*/
unsigned int
OperandBitField_t::dstsize() const {
  return std::max( m_size, m_size_modifier );
}

/** Create an unused bitfield object
    @param _size size in bits
*/
UnusedBitField_t::UnusedBitField_t( unsigned int _size )
  : FixedSizeBitField_t( _size )
{}

/** Create an unused bitfield object (copy constructor)
    @param _src the source bitfield object to copy
*/
UnusedBitField_t::UnusedBitField_t( UnusedBitField_t const& _src )
  : FixedSizeBitField_t( _src.m_size )
{}

/** Dump an unused bitfield object into a stream
    @param _sink a stream
*/
void
UnusedBitField_t::fills( ostream& _sink ) const {
  _sink << "?[" << m_size << "]";
}

SeparatorBitField_t::SeparatorBitField_t( bool _rewind )
  : m_rewind( _rewind )
{}

SeparatorBitField_t::SeparatorBitField_t( SeparatorBitField_t const& _src )
  : BitField_t(), m_rewind(false)
{}

void
SeparatorBitField_t::fills( std::ostream& _sink ) const {
  if( m_rewind ) _sink << "> " << Scanner::tokenname( TOK_REWIND ) << " <";
  else           _sink << "> <";
}

/** Dump an bitfield object into a stream
    @param _sink a stream
*/
std::ostream&
operator<<( std::ostream& _sink, BitField_t const& _bf ) {
  _bf.fills( _sink );
  return _sink;
}

SubOpBitField_t::SubOpBitField_t( ConstStr_t _symbol, SDInstance_t const* _sdinstance )
  : m_symbol( _symbol ), m_sdinstance( _sdinstance )
{}

SubOpBitField_t::SubOpBitField_t( SubOpBitField_t const& _src )
  : m_symbol( _src.m_symbol ), m_sdinstance( _src.m_sdinstance )
{}

/** Dump an subop bitfield object into a stream
    @param _sink a stream
*/
void
SubOpBitField_t::fills( std::ostream& _sink ) const {
  _sink << m_symbol << '[' << m_sdinstance->m_symbol << ']';
}

uintptr_t
SubOpBitField_t::sizes() const { return m_sdinstance->m_sdclass->m_insnsizes.size(); }

void
SubOpBitField_t::sizes( unsigned int* _sizes ) const
{
  std::copy( m_sdinstance->m_sdclass->m_insnsizes.begin(), m_sdinstance->m_sdclass->m_insnsizes.end(), _sizes );
}

unsigned int SubOpBitField_t::minsize() const { return m_sdinstance->m_sdclass->minsize(); }
unsigned int SubOpBitField_t::maxsize() const { return m_sdinstance->m_sdclass->maxsize(); }


/**
 *  Create a specialized operand bitfield object
 *  @param _src the operand bitfield object to be specialized.
 *  @param _value
 *  @param _shift a shift amount to do on the bitfield
 *  @param _size_modifier minimum bit size for holding the operand bitfield
 *  @param _sext true if a sign extension is required
 */
SpOperandBitField_t::SpOperandBitField_t( OperandBitField_t const& _src, unsigned int _value )
  : FixedSizeBitField_t( _src.m_size ), m_symbol( _src.m_symbol ), m_shift( _src.m_shift ),
    m_size_modifier( _src.m_size_modifier ), m_sext( _src.m_sext ), m_value( _value )
{}

/** Create an operand bitfield object (copy constructor)
 *  @param _src the source bitfield object to copy
 */
SpOperandBitField_t::SpOperandBitField_t( SpOperandBitField_t const& _src )
  : FixedSizeBitField_t( _src.m_size ), m_symbol( _src.m_symbol ), m_shift( _src.m_shift ),
    m_size_modifier( _src.m_size_modifier ), m_sext( _src.m_sext ), m_value( _src.m_value )
{}

/** Return the size (in bits) of the target word encoded by this field.
    @return the size (in bits) of the target word encoded by this field.
*/
unsigned int
SpOperandBitField_t::dstsize() const {
  return std::max( m_size, m_size_modifier );
}

/**
 *  Return the c constant string corresponding to the value encoded.
 */
ConstStr_t
SpOperandBitField_t::constval() const {
  if( not m_sext )
    return Str::fmt( "%#x", m_value );
  
  assert( m_size <= 32 );
  int32_t val = (int32_t( m_value ) << (32-m_size)) >> (32-m_size);
  if( val < 0 )
    return Str::fmt( "-%#x", uint32_t( -val ) );
  
  return Str::fmt( "%#x", uint32_t( m_value ) );
}
