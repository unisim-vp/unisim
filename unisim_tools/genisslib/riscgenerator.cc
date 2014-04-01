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
#include <limits>

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
  : m_insn_ctypesize( 0 )
{};

/** Process the isa structure and computes RISC specific data 
*/
void
RiscGenerator::finalize() {
  // Finalize size information
  if( (*m_insnsizes.rbegin()) > 64 ) {
    std::cerr << "error: can't process encodings wider than 64 bits (" << (*m_insnsizes.rbegin()) << " bits).";
    throw GenerationError;
  }
  
  m_insn_ctypesize = least_ctype_size( (*m_insnsizes.rbegin()) );
  m_insn_ctype = Str::fmt( "uint%d_t", m_insn_ctypesize );
  switch( m_insn_ctypesize ) {
  case 8:  m_insn_cpostfix = "U"; break;
  case 16: m_insn_cpostfix = "U"; break;
  case 32: m_insn_cpostfix = "UL"; break;
  case 64: m_insn_cpostfix = "ULL"; break;
  default: m_insn_cpostfix = "";
  }
  
  Vect_t<Operation_t> const& operations = isa().m_operations;
  
  // Process the opcodes needed by the decoder
  for( Vect_t<Operation_t>::const_iterator op = operations.begin(); op < operations.end(); ++ op ) {
    Vect_t<BitField_t> const& bitfields = (**op).m_bitfields;
    
    bool vlen = false, outprefix = false, vword = false;
    unsigned int insn_size = 0;
    uint64_t mask = 0, bits = 0;
    
    for (FieldIterator fi( isa().m_little_endian, bitfields, (*m_insnsizes.rbegin()) ); fi.next(); ) {
      if (SeparatorBitField_t const* sbf = dynamic_cast<SeparatorBitField_t const*>( &(fi.item()) )) {
        if (sbf->m_rewind and vword) {
          (**op).m_fileloc.err( "error: operation `%s' rewinds a variable length word.", (**op).m_symbol.str() );
          throw GenerationError;
        }
        vword = false;
      }
      
      if (fi.item().sizes() > 1) {
        vword = true;
        vlen = true;
        outprefix = true;
      }
      
      insn_size = std::max( insn_size, fi.insn_size() );
      
      if (outprefix or not fi.item().hasopcode()) continue;
      bits |= fi.item().bits() << fi.pos();
      mask |= fi.item().mask() << fi.pos();
    }
    m_opcodes[*op] = OpCode_t( mask, bits, insn_size, vlen );
  }
  
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
      case OpCode_t::Overlaps:
        if      (isa().m_user_orderings.count( std::make_pair( *op1, *op2 ) ) > 0)
          { opcode1.setupper( &opcode2 ); break; }
        else if (isa().m_user_orderings.count( std::make_pair( *op2, *op1 ) ) > 0)
          { opcode2.setupper( &opcode1 ); break; }
      case OpCode_t::Equal:
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

void
RiscGenerator::codetype_decl( Product_t& _product ) const {
  _product.code( "typedef %s CodeType;\n\n", m_insn_ctype.str() );
}

void
RiscGenerator::insn_encode_impl( Product_t& _product, Operation_t const& _op, char const* _codename ) const
{
  OpCode_t const& oc = opcode( &_op );
  
  if (oc.m_vlen) {
    _product.code( "assert( \"Encode method does not work with variable length operations.\" and false );\n" );
  }
  
  _product.code( "%s = 0x%llx%s;\n", _codename, oc.m_bits, m_insn_cpostfix.str() );
  
  for (FieldIterator fi( isa().m_little_endian, _op.m_bitfields, (*m_insnsizes.rbegin()) ); fi.next(); ) {
    if      (dynamic_cast<SubOpBitField_t const*>( &fi.item() ))
      {
        _product.code( "assert( \"Encode method does not work with sub-operations.\" and false );\n" );
      }
    else if (OperandBitField_t const* opbf = dynamic_cast<OperandBitField_t const*>( &fi.item() ))
      {
        int opsize = std::max( least_ctype_size( opbf->dstsize() ), m_minwordsize );
        ConstStr_t shiftedop;
        if      (opbf->m_shift > 0)
          shiftedop = Str::fmt( "(uint%d_t( %s ) << %u)", opsize, opbf->m_symbol.str(), +opbf->m_shift );
        else if (opbf->m_shift < 0)
          shiftedop = Str::fmt( "(uint%d_t( %s ) >> %u)", opsize, opbf->m_symbol.str(), -opbf->m_shift );
        else
          shiftedop = Str::fmt( "uint%d_t( %s )", opsize, opbf->m_symbol.str() );
        
        _product.code( "%s |= ((%s & 0x%llx) << %u);\n", _codename, shiftedop.str(), opbf->mask(), fi.pos() );
      }
  }
}

void
RiscGenerator::insn_decode_impl( Product_t& _product, Operation_t const& _op, char const* _codename, char const* _addrname ) const
{
  OpCode_t const& oc = opcode( &_op );
  
  if (oc.m_vlen) {
    _product.code( "this->gil_length = %u;\n", oc.m_size );
  }
  
  for (FieldIterator fi( isa().m_little_endian, _op.m_bitfields, (*m_insnsizes.rbegin()) ); fi.next(); ) {
    if (SubOpBitField_t const* sobf = dynamic_cast<SubOpBitField_t const*>( &fi.item() )) {
      SDInstance_t const* sdinstance = sobf->m_sdinstance;
      SDClass_t const* sdclass = sdinstance->m_sdclass;
      SourceCode_t const* tpscheme =  sdinstance->m_template_scheme;
      
      _product.code( "%s = %s::sub_decode", sobf->m_symbol.str(), sdclass->qd_namespace().str() );
      if (tpscheme)
        _product.usercode( tpscheme->m_fileloc, "< %s >", tpscheme->m_content.str() );
      _product.code( "( %s, ((%s >> %u) & 0x%llx) );\n",
                     _addrname, _codename, fi.pos(), sobf->mask() );

      if (sdclass->minsize() != sdclass->maxsize()) {
        _product.code( "{\nunsigned int shortening = %u - %s->GetLength();\n",
                       sdclass->maxsize(), sobf->m_symbol.str() );
        _product.code( "this->gil_length -= shortening;\n" );
        _product.code( "%s %s= shortening;\n}\n", _codename, (isa().m_little_endian ? "<<" : ">>") );
      }
    }
    
    else if (OperandBitField_t const* opbf = dynamic_cast<OperandBitField_t const*>( &fi.item() )) {
      _product.code( "%s = ", opbf->m_symbol.str() );
      
      if( opbf->m_sext ) {
        int opsize = std::max( least_ctype_size( opbf->dstsize() ), m_minwordsize );
        int sext_shift = opsize - opbf->m_size;
        _product.code( "(((((int%d_t)(%s >> %u)) & 0x%llx) << %u) >> %u)",
                       opsize, _codename, fi.pos(),
                       opbf->mask(), sext_shift, sext_shift );
      } else {
        // FIXME: a cast from the instruction type to the operand type
        // may be wiser...
        _product.code( "((%s >> %u) & 0x%llx)", _codename, fi.pos(), opbf->mask() );
      }
    
      if( opbf->m_shift > 0 )
        _product.code( " >> %u", +opbf->m_shift );
      if( opbf->m_shift < 0 )
        _product.code( " << %u", -opbf->m_shift );
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

ConstStr_t
RiscGenerator::insn_id_expr( char const* _addrname ) const {
  if ((*m_insnsizes.rbegin()) != (*m_insnsizes.begin()))
    return _addrname;
  return Str::fmt( "%s / %d", _addrname, ((*m_insnsizes.rbegin()) / 8) );
}

void
RiscGenerator::insn_match_ifexpr( Product_t& _product, char const* _code, char const* _mask, char const* _bits ) const {
  _product.code( "if((%s & %s) == %s)\n", _code, _mask, _bits );
}

void
RiscGenerator::insn_unchanged_expr( Product_t& _product, char const* _old, char const* _new ) const {
  _product.code( "%s == %s", _old, _new );
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
  if ((*m_insnsizes.begin()) == (*m_insnsizes.rbegin())) {
    _product.code( "inline unsigned int GetLength() const { return %d; }\n", (*m_insnsizes.rbegin()) );
  } else {
    _product.code( "virtual unsigned int GetLength() const { return 0; };\n" );
  }
}

void
RiscGenerator::insn_getlen_decl( Product_t& _product, Operation_t const& _op ) const {
  if ((*m_insnsizes.begin()) == (*m_insnsizes.rbegin()))
    return;
  
  OpCode_t const& oc = opcode( &_op );
  
  if (oc.m_vlen) {
    _product.code( "unsigned int gil_length;\n" );
    _product.code( "unsigned int GetLength() const { return this->gil_length; }\n" );
  } else {
    unsigned int size = oc.m_size;
    _product.code( "unsigned int GetLength() const { return %d; }\n", size );
  }
}
