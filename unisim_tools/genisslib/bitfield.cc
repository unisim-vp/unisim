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

/** Create an opcode bitfield object
    @param _size size in bits
    @param _value value of the opcode
*/
OpcodeBitField::OpcodeBitField( unsigned int _size, unsigned int _value )
  : FixedSizeBitField( _size ), m_value( _value )
{}

/** Create an opcode bitfield object (copy constructor)
    @param _src the bitfield object to copy
*/
OpcodeBitField::OpcodeBitField( OpcodeBitField const& _src )
  : FixedSizeBitField( _src.m_size ), m_value( _src.m_value )
{}

/** Dump an opcode bitfield object into a stream
    @param sink a stream
*/
void
OpcodeBitField::fills( std::ostream& sink ) const
{
  sink << Str::fmt( "0x%x[%u]", m_value, m_size );
}

/** Create an operand bitfield object
    @param _size size in bits
    @param _symbol a symbol object representing the operand
    @param _shift a shift amount to do on the bitfield
    @param _size_modifier minimum bit size for holding the operand bitfield
    @param _sext true if a sign extension is required
*/
OperandBitField::OperandBitField( unsigned int _size, ConstStr _symbol, int _shift, unsigned int _size_modifier, bool _sext )
  : FixedSizeBitField( _size ), m_symbol( _symbol ), m_shift( _shift ), m_size_modifier( _size_modifier ), m_sext( _sext )
{}

/** Create an operand bitfield object (copy constructor)
    @param _src the source bitfield object to copy
*/
OperandBitField::OperandBitField( OperandBitField const& _src )
  : FixedSizeBitField( _src.m_size ),
    m_symbol( _src.m_symbol ), m_shift( _src.m_shift ), m_size_modifier( _src.m_size_modifier ), m_sext( _src.m_sext )
{}

/** Dump an operand bitfield object into a stream
    @param sink a stream
*/
void
OperandBitField::fills( std::ostream& sink ) const
{
  sink << ( m_sext ? "sext" : "" );
  
  if (m_size_modifier)
    sink << '<' << m_size_modifier << '>';

  if (m_sext or m_size_modifier)
    sink << " ";

  sink << Str::fmt( "%s[%u]", m_symbol.str(), m_size );
}

/** Return the size (in bits) of the target word encoded by this field.
    @return the size (in bits) of the target word encoded by this field.
*/
unsigned int
OperandBitField::dstsize() const
{
  return std::max( (unsigned int)(m_size + ((m_shift < 0) ? -m_shift : 0)), m_size_modifier );
}

/** Create an unused bitfield object
    @param _size size in bits
*/
UnusedBitField::UnusedBitField( unsigned int _size )
  : FixedSizeBitField( _size )
{}

/** Create an unused bitfield object (copy constructor)
    @param _src the source bitfield object to copy
*/
UnusedBitField::UnusedBitField( UnusedBitField const& _src )
  : FixedSizeBitField( _src.m_size )
{}

/** Dump an unused bitfield object into a stream
    @param sink a stream
*/
void
UnusedBitField::fills( std::ostream& sink ) const
{
  sink << "?[" << m_size << "]";
}

SeparatorBitField::SeparatorBitField( bool _rewind )
  : m_rewind( _rewind )
{}

SeparatorBitField::SeparatorBitField( SeparatorBitField const& _src )
  : BitField(), m_rewind(false)
{}

void
SeparatorBitField::fills( std::ostream& sink ) const
{
  if (m_rewind)  sink << "> " << Scanner::tokenname( TOK_REWIND ) << " <";
  else           sink << "> <";
}

/** Dump an bitfield object into a stream
    @param sink a stream
*/
std::ostream&
operator << ( std::ostream& sink, BitField const& bf )
{
  bf.fills( sink );
  return sink;
}

SubOpBitField::SubOpBitField( ConstStr _symbol, SDInstance_t const* _sdinstance )
  : m_symbol( _symbol ), m_sdinstance( _sdinstance )
{}

SubOpBitField::SubOpBitField( SubOpBitField const& _src )
  : m_symbol( _src.m_symbol ), m_sdinstance( _src.m_sdinstance )
{}

/** Dump an subop bitfield object into a stream
    @param sink a stream
*/
void
SubOpBitField::fills( std::ostream& sink ) const
{
  sink << m_symbol << '[' << m_sdinstance->m_symbol << ']';
}

uintptr_t
SubOpBitField::sizes() const { return m_sdinstance->m_sdclass->m_insnsizes.size(); }

void
SubOpBitField::sizes( unsigned int* _sizes ) const
{
  std::copy( m_sdinstance->m_sdclass->m_insnsizes.begin(), m_sdinstance->m_sdclass->m_insnsizes.end(), _sizes );
}

unsigned int SubOpBitField::minsize() const { return m_sdinstance->m_sdclass->minsize(); }
unsigned int SubOpBitField::maxsize() const { return m_sdinstance->m_sdclass->maxsize(); }


/**
 *  Create a specialized operand bitfield object
 *  @param _src the operand bitfield object to be specialized.
 *  @param _value
 *  @param _shift a shift amount to do on the bitfield
 *  @param _size_modifier minimum bit size for holding the operand bitfield
 *  @param _sext true if a sign extension is required
 */
SpOperandBitField::SpOperandBitField( OperandBitField const& _src, unsigned int _value )
  : FixedSizeBitField( _src.m_size ), m_symbol( _src.m_symbol ), m_shift( _src.m_shift ),
    m_size_modifier( _src.m_size_modifier ), m_sext( _src.m_sext ), m_value( _value )
{}

/** Create an operand bitfield object (copy constructor)
 *  @param _src the source bitfield object to copy
 */
SpOperandBitField::SpOperandBitField( SpOperandBitField const& _src )
  : FixedSizeBitField( _src.m_size ), m_symbol( _src.m_symbol ), m_shift( _src.m_shift ),
    m_size_modifier( _src.m_size_modifier ), m_sext( _src.m_sext ), m_value( _src.m_value )
{}

/** Return the size (in bits) of the target word encoded by this field.
    @return the size (in bits) of the target word encoded by this field.
*/
unsigned int
SpOperandBitField::dstsize() const
{
  return std::max( (unsigned int)(m_size + ((m_shift < 0) ? -m_shift : 0)), m_size_modifier );
}

/**
 *  Return the c constant string corresponding to the value encoded.
 */
ConstStr
SpOperandBitField::constval() const
{
  if( not m_sext )
    return Str::fmt( "%#x", m_value );
  
  assert( m_size <= 32 );
  int32_t val = (int32_t( m_value ) << (32-m_size)) >> (32-m_size);
  if( val < 0 )
    return Str::fmt( "-%#x", uint32_t( -val ) );
  
  return Str::fmt( "%#x", uint32_t( m_value ) );
}
