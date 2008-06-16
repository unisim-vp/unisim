/***************************************************************************
                              ciscgenerator.cc
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

#include <ciscgenerator.hh>
#include <product.hh>
#include <main.hh>
#include <isa.hh>
#include <scanner.hh>
#include <operation.hh>
#include <variable.hh>
#include <sourcecode.hh>
#include <action.hh>
#include <strtools.hh>
#include <bitfield.hh>
#include <comment.hh>
#include <subdecoder.hh>
#include <conststr.hh>
#include <iostream>
#include <cmath>
#include <cassert>

using namespace std;

/**
 *  @brief  Default constructor, create a RiscGenerator.
 */
CiscGenerator::CiscGenerator()
  : m_insn_min_bitsize( 0 ), m_insn_max_bitsize( 0 ), m_insn_max_bytesize( 0 )
{};

/** Set the size of the OpCode and allocates mask and bits buffers
*/
void
CiscGenerator::OpCode_t::size( unsigned int _size ) {
  assert( m_size == 0 );
  m_size = _size;
  m_mask = new uint8_t[_size*2];
  m_bits = &m_mask[_size];
}

/** Optimize the size of OpCode object: right zero-mask bytes are stripped. */
void
CiscGenerator::OpCode_t::optimize() {
  unsigned int stripped_size = m_size;
  while( stripped_size > 0 and m_mask[stripped_size-1] == 0 ) stripped_size -= 1;
  m_size = stripped_size;
}

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

CiscGenerator::OpCode_t::Location_t
CiscGenerator::OpCode_t::locate( CiscGenerator::OpCode_t const& _that ) const {
  /* this is named A and _that is named B */
  unsigned int asize = m_size, bsize = _that.m_size, maxsize = std::max( asize, bsize );
  bool some_a_outside_B = false, some_b_outside_A = false;

  for( unsigned int idx = 0; idx < maxsize; ++ idx ) {
    uint8_t amask, abits, bmask, bbits;
    if( idx < asize ) { amask = this->m_mask[idx]; abits = this->m_bits[idx]; } else { amask = 0; abits = 0; }
    if( idx < bsize ) { bmask = _that.m_mask[idx]; bbits = _that.m_bits[idx]; } else { bmask = 0; bbits = 0; }
    
    if( (((amask) & (bmask)) & ((abits) ^ (bbits))) != 0 ) return Outside;
    if( ~(amask) & (bmask) ) some_a_outside_B = true;
    if( (amask) & ~(bmask) ) some_b_outside_A = true;
  }
  
  Location_t table[] = {Equal, Inside, Contains, Overlaps};
  
  return table[(some_a_outside_B ? 1 : 0) | (some_b_outside_A ? 2 : 0)];
}

/*
 *  @brief update the topology; linking subject to argument opcode
 *  @param _upper the object being linked to
 */

void
CiscGenerator::OpCode_t::setupper( OpCode_t* _upper ) {
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
CiscGenerator::OpCode_t::unsetupper() {
  if( not m_upper ) return;
  m_upper->m_lowercount -= 1;
  m_upper = 0;
}
  
/**
 *  @brief  Sets the %CiscGenerator instruction bit size to the specified bit size.
 *  @param  _bitsize  Bit size of %CiscGenerator instructions.
 *
 *  This function will sets the %CiscGenerator instruction bit size to
 *  the specified bit size.  Byte size, misalign, postfix and ctype
 *  are updated accordingly.
 */
void
CiscGenerator::bitsize( unsigned int _min_bitsize, unsigned int _max_bitsize ) {
  m_insn_min_bitsize = _min_bitsize;
  m_insn_max_bitsize = _max_bitsize;
  m_insn_max_bytesize = _max_bitsize / 8;
}

/**  @brief matches (or not) OpCode bytes according to the OpCode instance
 *   @param _oc the opcode which bytes are to be matched by the OpCode instance
 *   @return true on successful match
 */
bool
CiscGenerator::OpCode_t::match( OpCode_t const& _oc ) const {
  unsigned int bytesize = std::min( m_size, _oc.m_size );
  for( unsigned int byte = 0; byte < bytesize; ++byte ) {
    if( (_oc.m_bits[byte] & m_mask[byte]) != m_bits[byte] )
      return false;
  }
  return true;
}

/** Logs the content of the OpCode to an ostream
    @param _sink the ostream where the OpCode is logged.
    @return a reference to the ostream
*/
std::ostream&
operator<<( std::ostream& _sink, CiscGenerator::OpCode_t const& _oc ) {
  char hex[] = "0123456789abcdef";
  uint32_t byte;
  _sink << "{size: " << _oc.m_size << ", mask: 0x";
  for( unsigned int idx = 0; idx < _oc.m_size; ++ idx ) { byte = _oc.m_mask[idx]; _sink << hex[byte/16] << hex[byte%16]; }
  _sink << ", bits: 0x";
  for( unsigned int idx = 0; idx < _oc.m_size; ++ idx ) { byte = _oc.m_bits[idx]; _sink << hex[byte/16] << hex[byte%16]; }
  _sink << "}";
  return _sink;
}

/**
 *  @brief  A word iterator Object.
 *
 *  This class implements an iterator on words in a bitfield list
 *  (bitfield separated by separators). It computes bitfield bounds by
 *  mean of iterators, and size for current word.
 *  
 */

struct BFWordIterator {
  Vect_t<BitField_t>::const_iterator m_left, m_right, m_end;
  unsigned int                       m_count, m_minsize, m_maxsize;
  bool m_rewind;
  
  BFWordIterator( Vect_t<BitField_t> const& _bitfields )
    : m_right( _bitfields.begin() - 1 ), m_end( _bitfields.end() ),
      m_count( 0 ), m_minsize( 0 ), m_maxsize( 0 ), m_rewind( false )
  {}
  
  bool
  next() {
    m_left = m_right + 1;
    if( m_left >= m_end ) return false;
    m_count = 0; m_minsize = 0; m_maxsize = 0;
    for( m_right = m_left; (m_right < m_end) and ((**m_right).type() != BitField_t::Separator); ++m_right ) {
      m_count += 1;
      m_minsize += (**m_right).minsize();
      m_maxsize += (**m_right).maxsize();
    }
    if( (m_right < m_end) and ((**m_right).type() == BitField_t::Separator) ) {
      SeparatorBitField_t const& sepbf = dynamic_cast<SeparatorBitField_t const&>( **m_right );
      m_rewind = sepbf.m_rewind;
    } else
      m_rewind = false;
        
    return true;
  }
  
};

/** Process the isa structure and computes CISC specific data 
*/
void
CiscGenerator::finalize() {
  unsigned int insn_max_bitsize = 0;
  unsigned int insn_min_bitsize = std::numeric_limits<unsigned int>::max();
  
  // Process the opcodes needed by the decoder
  for( Vect_t<Operation_t>::const_iterator op = isa().m_operations.begin(); op < isa().m_operations.end(); ++ op ) {
    unsigned int count = 0, minsize = 0, maxsize = 0, prefixsize = 0;
    for( BFWordIterator bfword( (**op).m_bitfields ); bfword.next(); ) {
      count   += bfword.m_count;
      minsize += bfword.m_minsize;
      maxsize += bfword.m_maxsize;
      if( minsize == maxsize ) prefixsize = maxsize;
      if( (minsize % 8) != 0 or (maxsize % 8) != 0 ) {
        (**op).m_fileloc.err( "unaligned word separator (%dth field) in operation `%s`.", count+1, (**op).m_symbol.str() );
        throw GenerationError;
      }
      bool subop = false;
      for( Vect_t<BitField_t>::const_iterator bf = bfword.m_left; bf < bfword.m_right; ++ bf )
        if( (**bf).type() == BitField_t::SubOp ) subop = true;
      if( subop and bfword.m_count > 1 ) {
        (**op).m_fileloc.err( "subdecoder fields must be surrounded by word separator (operation `%s`).", (**op).m_symbol.str() );
        throw GenerationError;
      }
      if( not subop and (bfword.m_maxsize & (bfword.m_maxsize - 1)) ) {
        assert( bfword.m_maxsize == bfword.m_minsize );
        (**op).m_fileloc.err( "subword size must be a power of two (operation `%s`).", (**op).m_symbol.str() );
        throw GenerationError;
      }
    }
    m_opcodes[*op].size( prefixsize / 8 );
    if( maxsize > insn_max_bitsize ) insn_max_bitsize = maxsize;
    if( minsize < insn_min_bitsize ) insn_min_bitsize = minsize;
    OpCode_t& oc = opcode( *op );
    int byteoffset = 0;
    for( BFWordIterator bfword( (**op).m_bitfields ); bfword.next(); ) {
      if( bfword.m_minsize != bfword.m_maxsize ) break;
      int bytesize = bfword.m_maxsize / 8;
      unsigned int shift = bfword.m_maxsize;
      uint64_t mask = 0, bits = 0;
      for( Vect_t<BitField_t>::const_iterator bf = bfword.m_left; bf < bfword.m_right; ++ bf ) {
        shift -= (**bf).m_size;
        if( not (**bf).hasopcode() ) continue;
        mask |= (**bf).mask() << shift;
        bits |= (**bf).bits() << shift;
      }
      if( isa().m_little_endian ) {
        for( int byte = 0; byte < bytesize; ++ byte ) {
          oc.m_mask[byteoffset+byte] = mask & 0xff;
          oc.m_bits[byteoffset+byte] = bits & 0xff;
          mask = mask >> 8;
          bits = bits >> 8;
        }
      } else {
        for( int byte = bytesize; (--byte) >= 0; ) {
          oc.m_mask[byteoffset+byte] = mask & 0xff;
          oc.m_bits[byteoffset+byte] = bits & 0xff;
          mask = mask >> 8;
          bits = bits >> 8;
        }
      }
      byteoffset += bytesize;
    }
    oc.optimize();
    //    cout << (**op).m_symbol << ": " << oc << endl;
  }
  
  bitsize( insn_min_bitsize, insn_max_bitsize );
  cerr << "Instruction Size: [" << insn_min_bitsize << ";" << insn_max_bitsize << "]" << endl;
  
  /* Generating the topological graph of operations, and checking for
   * conflicts (overlapping encodings) 
   */
  for( Vect_t<Operation_t>::const_iterator op1 = isa().m_operations.begin(); op1 < isa().m_operations.end(); ++op1 ) {
    OpCode_t& opcode1 = opcode( *op1 );
    for( Vect_t<Operation_t>::const_iterator op2 = isa().m_operations.begin(); op2 < op1; ++ op2 ) {
      OpCode_t& opcode2 = opcode( *op2 );

      switch( opcode1.locate( opcode2 ) ) {
      case OpCode_t::Outside: break; // No problem
      case OpCode_t::Overlaps: case OpCode_t::Equal:
        (**op1).m_fileloc.err( "error: operation `%s' conflicts with operation `%s'", (**op1).m_symbol.str(), (**op2).m_symbol.str() );
        (**op2).m_fileloc.err( "operation `%s' was declared here", (**op2).m_symbol.str() );
        cerr << (**op1).m_symbol << ": " << opcode1 << endl;
        cerr << (**op2).m_symbol << ": " << opcode2 << endl;
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
    Vect_t<Operation_t> operations( opcount );
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
      operations[--dopidx] = op;
      isa().m_operations[sopidx] = 0;
      oc.unsetupper();
    }
    isa().m_operations = operations;
  }
}

void
CiscGenerator::additional_decl_includes( Product_t& _product ) const {
  _product.code( "#include <iosfwd>\n" );
}

void
CiscGenerator::additional_impl_includes( Product_t& _product ) const {
  _product.code( "#include <ostream>\n" );
  _product.code( "#include <cstring>\n" );
}

void
CiscGenerator::codetype_decl( Product_t& _product ) const {
  _product.code( "struct CodeType {\n" );
  _product.code( " static const unsigned int maxsize = %d;\n", m_insn_max_bytesize );
  _product.code( " unsigned int              size;\n" );
  _product.code( " uint8_t                   str[maxsize];\n" );
  _product.code( " enum Exception_t { NotEnoughBytes };\n" );
  _product.code( " CodeType() : size( 0 ) {};\n" );
  _product.code( " CodeType( uint8_t* _src, unsigned int _size )\n" );
  _product.code( " : size( std::min( _size, maxsize ) ) { memcpy( str, _src, size ); }\n" );
  _product.code( " CodeType( CodeType const& _ct )\n" );
  _product.code( " : size( _ct.size ) { memcpy( str, _ct.str, _ct.size ); }\n" );
  _product.code( " bool match( CodeType const& _bits, CodeType const& _mask ) const {\n" );
  _product.code( "  for( unsigned int idx = 0; idx < _mask.size; ++idx ) {\n" );
  _product.code( "   if( idx >= size ) throw NotEnoughBytes;\n" );
  _product.code( "   if( (str[idx] & _mask.str[idx]) != _bits.str[idx] ) return false;\n" );
  _product.code( "  };\n" );
  _product.code( "  return true;\n" );
  _product.code( " };\n" );
  _product.code( " bool match( CodeType const& _bits ) const {\n" );
  _product.code( "  if( size < _bits.size ) throw NotEnoughBytes;\n" );
  _product.code( "  return memcmp( str, _bits.str, _bits.size ) == 0;\n" );
  _product.code( " }\n" );
  _product.code( " void pop( unsigned int _bytes ) {\n" );
  _product.code( "  if( size < _bytes ) throw NotEnoughBytes;\n" );
  _product.code( "  size -= _bytes;\n" );
  _product.code( "  memmove( str, str + _bytes, size );\n" );
  _product.code( " }\n" );
  _product.code( " friend std::ostream& operator << ( std::ostream& _sink, CodeType const& _ct );\n" );
  _product.code( "};\n" );
}

void
CiscGenerator::codetype_impl( Product_t& _product ) const {
  _product.code( "unsigned int const CodeType::maxsize;\n" );
  _product.code( "std::ostream& operator << ( std::ostream& _sink, CodeType const& _ct ) {\n" );
  _product.code( " char const* xrepr = \"0123456789abcdef\";\n" );
  _product.code( " char const* sep = \"\";\n" );
  _product.code( " for( unsigned int idx = 0; idx < _ct.size; ++idx, sep = \" \" ) {\n" );
  _product.code( "  uint32_t byte = _ct.str[idx];\n" );
  _product.code( "  _sink << sep << xrepr[byte/16] << xrepr[byte%16];\n" );
  _product.code( " }\n" );
  _product.code( " return _sink;\n" );
  _product.code( "}\n" );
}

void
CiscGenerator::insn_bits_code( Product_t& _product, Operation_t const& _op ) const {
  OpCode_t const& oc = opcode( &_op );
  _product.code( "CodeType( (uint8_t*)( \"" );
  char const* hex = "0123456789abcdef";
  for( unsigned int idx = 0; idx < oc.m_size; ++idx )
    _product.code( "\\x%c%c", hex[oc.m_bits[idx]/16], hex[oc.m_bits[idx]%16] );
  _product.code( "\" ), %u )", oc.m_size );
}

void
CiscGenerator::insn_mask_code( Product_t& _product, Operation_t const& _op ) const {
  OpCode_t const& oc = opcode( &_op );
  _product.code( "CodeType( (uint8_t*)( \"" );
  char const* hex = "0123456789abcdef";
  for( unsigned int idx = 0; idx < oc.m_size; ++idx )
    _product.code( "\\x%c%c", hex[oc.m_mask[idx]/16], hex[oc.m_mask[idx]%16] );
  _product.code( "\" ), %u )", oc.m_size );
}

void
CiscGenerator::insn_fetch_impl( Product_t& _product, char const* _codename ) const {
  _product.code( "CodeType %s;\n", _codename );
  _product.code( "Fetch( %s, addr );\n", _codename, _codename );
}

void
CiscGenerator::insn_fetch_protoargs( Product_t& _product ) const {
  _product.code( "( CodeType&, %s )", isa().m_addrtype.str() );
}

void
CiscGenerator::insn_match_ifexpr( Product_t& _product, char const* _code, char const* _mask, char const* _bits ) const {
  _product.code( "if( %s.match( %s, %s) )\n", _code, _bits, _mask );
}

void
CiscGenerator::insn_decode_impl( Product_t& _product, Operation_t const& _op, char const* _codename, char const* _addrname ) const
{
  _product.code( "CodeType _code_( %s );\n", _codename );
  for( BFWordIterator bfword( _op.m_bitfields ); bfword.next(); ) {
    if( (**bfword.m_left).type() == BitField_t::SubOp ) {
      SubOpBitField_t const& sobf = dynamic_cast<SubOpBitField_t const&>( **bfword.m_left );
      SDInstance_t const* sdinstance = sobf.m_sdinstance;
      SDClass_t const* sdclass = sdinstance->m_sdclass;
      SourceCode_t const* tpscheme =  sdinstance->m_template_scheme;
        
      _product.code( "%s = %s::sub_decode", sobf.m_symbol.str(), sdclass->qd_namespace().str() );
      if( tpscheme )
        _product.usercode( tpscheme->m_fileloc, "< %s >", tpscheme->m_content.str() );
      _product.code( "( %s, %s::CodeType( _code_.str, _code_.size ) );\n", _addrname, sdclass->qd_namespace().str() );
      if( not bfword.m_rewind )
        _product.code( "_code_.pop( %s->GetEncoding().size );\n", sobf.m_symbol.str() );
      continue;
    }
    bool used = false;
    for( Vect_t<BitField_t>::const_iterator bf = bfword.m_left; bf < bfword.m_right; ++ bf )
      if( (**bf).type() == BitField_t::Operand ) { used = true; break; }
    
    if( used ) _product.code( "{\n" );
    unsigned int bytesize = bfword.m_maxsize / 8;
    ConstStr_t subwordtype = Str::fmt( "uint%d_t", bfword.m_maxsize );
    if( used ) {
      _product.code( "%s _subword_ = ", subwordtype.str() );
      char const* sep = "";
      for( unsigned int byteidx = 0; byteidx < bytesize; ++byteidx, sep = " | " ) {
        unsigned int shift = isa().m_little_endian ? 8*byteidx : 8*(bytesize - byteidx - 1);
        _product.code( "%s(%s( _code_.str[%u] ) << %u)", sep, subwordtype.str(), byteidx, shift );
      }
      _product.code( ";\n" );
    }
    unsigned int shift = bfword.m_maxsize;
    for( Vect_t<BitField_t>::const_iterator bf = bfword.m_left; bf < bfword.m_right; ++ bf ) {
      shift -= (**bf).m_size;
      if( (**bf).type() == BitField_t::Operand ) {
        OperandBitField_t const& opbf = dynamic_cast<OperandBitField_t const&>( **bf );
        _product.code( "%s = ", opbf.m_symbol.str() );
      
        if( opbf.m_sext ) {
          int sizeofop = std::max( opbf.wordsize(), m_minwordsize );
          int sext_shift = 8*sizeofop - opbf.m_size;
          _product.code( "(((int%d_t((_subword_ >> %u) & 0x%llxLL)) << %u) >> %u)",
                         8*sizeofop, shift, opbf.mask(), sext_shift, sext_shift );
        } else {
          // FIXME: a cast from the instruction type to the operand type
          // may be wiser...
          _product.code( "((_subword_ >> %u) & 0x%llxULL)", shift, opbf.mask() );
        }
        if( opbf.m_shift > 0 )
          _product.code( " >> %u", +opbf.m_shift );
        if( opbf.m_shift < 0 )
          _product.code( " << %u", -opbf.m_shift );
        _product.code( ";\n" );
      }
    }
    if( used )_product.code( "}\n" );
    if( not bfword.m_rewind )
      _product.code( "_code_.pop( %u );\n", bytesize );
  }
  _product.code( "this->encoding.size -= _code_.size;\n" );
}

void
CiscGenerator::insn_unchanged_expr( Product_t& _product, char const* _ref, char const* _bytes ) const {
  _product.code( "%s.match( %s )", _bytes, _ref );
}

void
CiscGenerator::subdecoder_bounds( Product_t& _product ) const {
  _product.code( "[%d;%d]", m_insn_min_bitsize, m_insn_max_bitsize );
}

CiscGenerator::OpCode_t const&
CiscGenerator::opcode( Operation_t const* _op ) const {
  OpCodes_t::const_iterator res = m_opcodes.find( _op );
  assert( res != m_opcodes.end() );
  return res->second;
}

CiscGenerator::OpCode_t&
CiscGenerator::opcode( Operation_t const* _op ) {
  OpCodes_t::iterator res = m_opcodes.find( _op );
  assert( res != m_opcodes.end() );
  return res->second;
}

void
CiscGenerator::insn_destructor_decl( Product_t& _product, Operation_t const& _op ) const {
  bool subops = false;
  
  for( BFWordIterator bfword( _op.m_bitfields ); bfword.next(); ) {
    if( (**bfword.m_left).type() == BitField_t::SubOp ) { subops = true; break; }
  }
  
  if( not subops ) return;
  _product.code( "~Op%s();\n", Str::capitalize( _op.m_symbol ).str() );
}

void
CiscGenerator::insn_destructor_impl( Product_t& _product, Operation_t const& _op ) const {
  std::vector<ConstStr_t> subops;
  
  for( BFWordIterator bfword( _op.m_bitfields ); bfword.next(); ) {
    if( (**bfword.m_left).type() != BitField_t::SubOp ) continue;
    SubOpBitField_t const& sobf = dynamic_cast<SubOpBitField_t const&>( **bfword.m_left );
    subops.push_back( sobf.m_symbol );
  }
  
  if( subops.size() == 0 ) return;
  
  _product.template_signature( isa().m_tparams );
  _product.code( "Op%s", Str::capitalize( _op.m_symbol ).str() );
  _product.template_abbrev( isa().m_tparams );
  _product.code( "::~Op%s()\n", Str::capitalize( _op.m_symbol ).str() );
  
  _product.code( "{\n" );
  for( std::vector<ConstStr_t>::const_iterator name = subops.begin(); name != subops.end(); ++name ) {
    _product.code( "delete %s;\n", name->str() );
  }
  _product.code( "}\n\n" );
  
}
