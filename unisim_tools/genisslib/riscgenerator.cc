/***************************************************************************
                              riscgenerator.cc
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

#include <riscgenerator.hh>
#include <operation.hh>
#include <isa.hh>
#include <bitfield.hh>
#include <scanner.hh>
#include <subdecoder.hh>
#include <sourcecode.hh>
#include <strtools.hh>
#include <iostream>
#include <product.hh>
#include <cassert>

using namespace std;

/**
 *  @brief  Locate argument opcode from the subject opcode point of view.
 *  @param  _that the opcode to locate
 *  @return the argument object location
 *
 *  This function will return the argument opcode location according
 *  to the subject opcode location. Relative locations may be:
 *    - Outside
 *    - Overlaps
 *    - Inside
 *    - Contains
 *    - Equal
 */

RiscGenerator::OpCode_t::Location_t
RiscGenerator::OpCode_t::locate( RiscGenerator::OpCode_t const& _that ) const {
  /* this is named A and _that is named B */
  bool some_a_outside_B = false, some_b_outside_A = false;

  if( (((this->m_mask) & (_that.m_mask)) & ((this->m_bits) ^ (_that.m_bits))) != 0 )
      return Outside;
    
  if( ~(this->m_mask) & (_that.m_mask) )
    some_a_outside_B = true;
  
  if( (this->m_mask) & ~(_that.m_mask) )
    some_b_outside_A = true;
  
  Location_t table[] = {Equal, Inside, Contains, Overlaps};
  
  return table[(some_a_outside_B ? 1 : 0) | (some_b_outside_A ? 2 : 0)];
}

/*
 *  @brief update the topology; linking subject to argument opcode
 *  @param _upper the object being linked to
 */

void
RiscGenerator::OpCode_t::setupper( OpCode_t* _upper ) {
  if( not m_upper ) {
    m_upper = _upper;
    _upper->m_lowercount += 1;
  } else if( m_upper->locate( *_upper ) == Inside ) {
    m_upper->m_lowercount -= 1;
    m_upper = _upper;
    _upper->m_lowercount += 1;
  }
}
  
/*
 *  @brief update the topology; unlink subject opcode
 */

void
RiscGenerator::OpCode_t::unsetupper() {
  if( not m_upper ) return;
  m_upper->m_lowercount -= 1;
  m_upper = 0;
}
  
/**
 *  @brief  Default constructor, create a RiscGenerator.
 */
RiscGenerator::RiscGenerator()
  : m_insn_maxsize( 0 ), m_insn_minsize( 0 ), m_insn_ctypesize( 0 )
{};

/** Process the isa structure and computes RISC specific data 
*/
void
RiscGenerator::finalize() {
  Vect_t<Operation_t> const& operations = isa().m_operations;
  
  // Process the opcodes needed by the decoder
  unsigned int insn_maxsize = 0, insn_minsize = 64;
  for( Vect_t<Operation_t>::const_iterator op = operations.begin(); op < operations.end(); ++ op ) {
    unsigned int size = 0;
    Vect_t<BitField_t> const& bitfields = (**op).m_bitfields;
    
    for( Vect_t<BitField_t>::const_iterator bf = bitfields.begin(); bf < bitfields.end(); ++ bf ) {
      if ((**bf).minsize() != (**bf).maxsize()) {
        (**op).m_fileloc.err( "error: not (yet?) supported variable length field\n"
                              "error: in operation `%s` at %u bits",
                              (**op).m_symbol.str(), size );
        throw GenerationError;
      }
      size += (**bf).m_size;
    }
    if( size > 64 ) {
      (**op).m_fileloc.err( "error: can't process encodings wider than 64 bits (operation `%s`, %u bits).",
                            (**op).m_symbol.str(), size );
      throw GenerationError;
    }
    
    if( insn_maxsize < size ) insn_maxsize = size;
    if( insn_minsize > size ) insn_minsize = size;
    unsigned int shift = size;
    uint64_t mask = 0, bits = 0;
    for( Vect_t<BitField_t>::const_iterator bf = bitfields.begin(); bf < bitfields.end(); ++ bf ) {
      shift -= (**bf).m_size;
      if( not (**bf).hasopcode() ) continue;
      bits |= (**bf).bits() << shift;
      mask |= (**bf).mask() << shift;
    }
    m_opcodes[*op] = OpCode_t( size, mask, bits );
  }
  
  bitsize( insn_minsize, insn_maxsize );
  
  // Left padding variable length operations (big-endian)
  if (not isa().m_little_endian) {
    for (OpCodes_t::iterator itr = m_opcodes.begin(); itr != m_opcodes.end(); ++itr) {
      unsigned int leftpad = m_insn_ctypesize - itr->second.m_size;
      if (leftpad == 0) continue;
      itr->second.m_mask <<= leftpad;
      itr->second.m_bits <<= leftpad;
    }
  }
  
  if (insn_minsize != insn_maxsize)
    cerr << "Instruction Size: [" << insn_minsize << ":" << insn_maxsize << "]" << endl;
  else
    cerr << "Instruction Size: " << insn_maxsize << endl;
  
  /* Generating the topological graph of operations, checking for
   * conflicts (overlapping encodings), and checking for operations
   * with weird size.
   */
  for( Vect_t<Operation_t>::const_iterator op1 = operations.begin(); op1 < operations.end(); ++op1 ) {
    OpCode_t& opcode1 = opcode( *op1 );
    for( Vect_t<Operation_t>::const_iterator op2 = operations.begin(); op2 < op1; ++ op2 ) {
      OpCode_t& opcode2 = opcode( *op2 );

      switch( opcode1.locate( opcode2 ) ) {
      case OpCode_t::Outside: break; // No problem
      case OpCode_t::Overlaps: case OpCode_t::Equal:
        (**op1).m_fileloc.err( "error: operation `%s' conflicts with operation `%s'", (**op1).m_symbol.str(), (**op2).m_symbol.str() );
        (**op2).m_fileloc.err( "operation `%s' was declared here", (**op2).m_symbol.str() );
        throw GenerationError;
        break;
      case OpCode_t::Inside:
        opcode2.setupper( &opcode1 );
        cerr << "operation `" << (**op2).m_symbol << "' is a specialization of operation `" << (**op1).m_symbol << "'" << endl;
        break;
      case OpCode_t::Contains:
        opcode1.setupper( &opcode2 );
        cerr << "operation `" << (**op1).m_symbol << "' is a specialization of operation `" << (**op2).m_symbol << "'" << endl;
        break;
      }
    }
  }
  
  // Topological sort to fix potential precedence problems
  {
    intptr_t opcount = isa().m_operations.size();
    Vect_t<Operation_t> noperations( opcount );
    intptr_t
      sopidx = opcount, // operation source table index
      dopidx = opcount, // operation destination table index
      inf_loop_tracker = opcount; // counter tracking infinite loop
    
    while( dopidx > 0 ) {
      sopidx = (sopidx + opcount - 1) % opcount;
      Operation_t* op = isa().m_operations[sopidx];
      if( not op ) continue;
      
      OpCode_t& oc = opcode( op );
      if( oc.m_lowercount > 0 ) {
        // There is some operations to be placed before this one 
        --inf_loop_tracker;
        assert( inf_loop_tracker >= 0 );
        continue;
      }
      inf_loop_tracker = opcount;
      noperations[--dopidx] = op;
      isa().m_operations[sopidx] = 0;
      oc.unsetupper();
    }
    isa().m_operations = noperations;
  }
}

/**
 *  @brief  Setup the %RiscGenerator instruction sizes in bits
 *  @param  minsize  The minimum size of %RiscGenerator instructions (in bits).
 *  @param  maxsize  The maximum size of %RiscGenerator instructions (in bits).
 *
 *  This function will setup the %RiscGenerator instruction sizes in
 *  bits. Byte size, misalign, postfix and ctype are updated
 *  accordingly.
 */
void
RiscGenerator::bitsize( unsigned int minsize, unsigned int maxsize ) {
  m_insn_minsize = minsize;
  m_insn_maxsize = maxsize;
  m_insn_ctypesize = least_ctype_size( maxsize );
  m_insn_ctype = Str::fmt( "uint%d_t", m_insn_ctypesize );
  switch( m_insn_ctypesize ) {
  case 8:  m_insn_cpostfix = "U"; break;
  case 16: m_insn_cpostfix = "U"; break;
  case 32: m_insn_cpostfix = "UL"; break;
  case 64: m_insn_cpostfix = "ULL"; break;
  default: m_insn_cpostfix = "";
  }
}

void
RiscGenerator::codetype_decl( Product_t& _product ) const {
  _product.code( "typedef %s CodeType;\n\n", m_insn_ctype.str() );
}

void
RiscGenerator::insn_decode_impl( Product_t& _product, Operation_t const& _op, char const* _codename, char const* _addrname ) const
{
  unsigned int shift = isa().m_little_endian ? opcode( &_op ).m_size : m_insn_ctypesize;
  for( Vect_t<BitField_t>::const_iterator bf = _op.m_bitfields.begin(); bf < _op.m_bitfields.end(); ++ bf ) {
    shift -= (**bf).m_size;
    if( (**bf).type() == BitField_t::SubOp ) {
      
      SubOpBitField_t const& sobf = dynamic_cast<SubOpBitField_t const&>( **bf );
      SDInstance_t const* sdinstance = sobf.m_sdinstance;
      SDClass_t const* sdclass = sdinstance->m_sdclass;
      SourceCode_t const* tpscheme =  sdinstance->m_template_scheme;
      
      _product.code( "%s = %s::sub_decode", sobf.m_symbol.str(), sdclass->qd_namespace().str() );
      if( tpscheme )
        _product.usercode( tpscheme->m_fileloc, "< %s >", tpscheme->m_content.str() );
      _product.code( "( %s, ((%s >> %u) & 0x%llx) );\n", _addrname, _codename, shift, sobf.mask() );
    }
    
    else if( (**bf).type() == BitField_t::Operand ) {
      
      OperandBitField_t const& opbf = dynamic_cast<OperandBitField_t const&>( **bf );
      _product.code( "%s = ", opbf.m_symbol.str() );
      
      if( opbf.m_sext ) {
        int sizeofop = std::max( opbf.dstsize(), m_minwordsize );
        int sext_shift = sizeofop - opbf.m_size;
        _product.code( "(((((int%d_t)(%s >> %u)) & 0x%llx) << %u) >> %u)",
                       sizeofop, _codename, shift, opbf.mask(), sext_shift, sext_shift );
      } else {
        // FIXME: a cast from the instruction type to the operand type
        // may be wiser...
        _product.code( "((%s >> %u) & 0x%llx)", _codename, shift, opbf.mask() );
      }
    
      if( opbf.m_shift > 0 )
        _product.code( " >> %u", +opbf.m_shift );
      if( opbf.m_shift < 0 )
        _product.code( " << %u", -opbf.m_shift );
      _product.code( ";\n" );
    }
  }
}

void
RiscGenerator::insn_bits_code( Product_t& _product, Operation_t const& _op ) const {
  _product.code( "0x%llx%s", opcode( &_op ).m_bits, m_insn_cpostfix.str() );
}

void
RiscGenerator::insn_mask_code( Product_t& _product, Operation_t const& _op ) const {
  _product.code( "0x%llx%s", opcode( &_op ).m_mask, m_insn_cpostfix.str() );
}

void
RiscGenerator::insn_fetch_impl( Product_t& _product, char const* _codename ) const {
  _product.code( "CodeType %s;\n", _codename );
  _product.code( "Fetch(&%s, addr, sizeof(%s));\n", _codename, _codename );
  _product.code( "#if BYTE_ORDER == LITTLE_ENDIAN\n" );
  _product.code( "if(!is_little_endian)\n" );
  _product.code( "#else\n" );
  _product.code( "if(is_little_endian)\n" );
  _product.code( "#endif\n" );
  _product.code( "{\n" );
  
  _product.code( " %s = ", _codename );
  
  char const* sep = "";
  unsigned int insn_bytesize = m_insn_maxsize / 8;
  for( int byteindex = insn_bytesize; (--byteindex) >= 0; ) {
    _product.code( "%s((%s & 0x", sep, _codename );
    sep = " | ";
    for( int byteindex1 = insn_bytesize; (--byteindex1) >= 0; )
      _product.code( byteindex1 == byteindex ? "ff" : "00" );
    int byteshift = 8*(insn_bytesize - 2*byteindex - 1);
    if( byteshift > 0 ) _product.code( "%s) << %d)", m_insn_cpostfix.str(), +byteshift );
    else                _product.code( "%s) >> %d)", m_insn_cpostfix.str(), -byteshift );
  }
  _product.code( ";\n" );
  
  unsigned int insn_misalign = m_insn_ctypesize - m_insn_maxsize;
  if( insn_misalign )
    _product.code( " %s = %s >> %u;\n", _codename, _codename, insn_misalign );
  
  _product.code( "}\n" );
}

void
RiscGenerator::insn_fetch_protoargs( Product_t& _product ) const {
  _product.code( "(void *, %s, uint32_t)", isa().m_addrtype.str() );
}

ConstStr_t
RiscGenerator::insn_id_expr( char const* _addrname ) const {
  if (m_insn_maxsize != m_insn_minsize)
    return _addrname;
  return Str::fmt( "%s / %d", _addrname, (m_insn_maxsize / 8) );
}

void
RiscGenerator::insn_match_ifexpr( Product_t& _product, char const* _code, char const* _mask, char const* _bits ) const {
  _product.code( "if((%s & %s) == %s)\n", _code, _mask, _bits );
}

void
RiscGenerator::insn_unchanged_expr( Product_t& _product, char const* _old, char const* _new ) const {
  _product.code( "%s == %s", _old, _new );
}

void
RiscGenerator::subdecoder_bounds( Product_t& _product ) const {
  _product.code( "[%d:%d]", m_insn_minsize, m_insn_maxsize );
}

RiscGenerator::OpCode_t const&
RiscGenerator::opcode( Operation_t const* _op ) const {
  OpCodes_t::const_iterator res = m_opcodes.find( _op );
  assert( res != m_opcodes.end() );
  return res->second;
}

RiscGenerator::OpCode_t&
RiscGenerator::opcode( Operation_t const* _op ) {
  OpCodes_t::iterator res = m_opcodes.find( _op );
  assert( res != m_opcodes.end() );
  return res->second;
}

void
RiscGenerator::op_getlen_decl( Product_t& _product ) const {
  if (m_insn_minsize == m_insn_maxsize) {
    _product.code( "inline unsigned int GetLength() const { return %d; }\n", m_insn_maxsize );
  } else {
    _product.code( "virtual unsigned int GetLength() const { return 0; };\n" );
  }
}

void
RiscGenerator::insn_getlen_decl( Product_t& _product, Operation_t const& _op ) const {
  if (m_insn_minsize == m_insn_maxsize)
    return;
  unsigned int size = opcode( &_op ).m_size;
  _product.code( "unsigned int GetLength() const { return %d; }\n", size );
}
