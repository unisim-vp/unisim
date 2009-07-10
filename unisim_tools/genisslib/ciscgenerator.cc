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
#include <cassert>
#include <cstring>
#include <limits>

using namespace std;

/**
 *  @brief  Default constructor, create a CiscGenerator.
 */
CiscGenerator::CiscGenerator()
  : m_code_capacity( 0 )
{};

/** Set the size of the OpCode and allocates mask and bits buffers
*/
void

CiscGenerator::OpCode_t::size_attrs( unsigned int prefixsize, unsigned int fullsize, bool vlen ) {
  assert( m_prefixsize == 0 );
  m_prefixsize = prefixsize;
  unsigned int mbsz = maskbytesize();
  m_fullsize = fullsize;
  m_mask = new uint8_t[mbsz*2];
  m_bits = &m_mask[mbsz];
  memset( m_mask, 0, mbsz*2 );
  m_vlen = vlen;
}

/** Optimize the size of OpCode object: upper zero-mask bits are stripped. */
void
CiscGenerator::OpCode_t::optimize( bool is_little_endian ) {
  unsigned int stripped_size = m_prefixsize;
  if (is_little_endian) {
    for (; stripped_size > 0; stripped_size -= 1) {
      unsigned int byte = (stripped_size-1)/8, bit = (stripped_size-1)%8;
      if (((m_mask[byte] >> bit) & 1) != 0) break;
    }
  } else {
    for (; stripped_size > 0; stripped_size -= 1) {
      unsigned int byte = (stripped_size-1)/8, bit = (-stripped_size)%8;
      if (((m_mask[byte] >> bit) & 1) != 0) break;
    }
  }
  m_prefixsize = stripped_size;
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
  unsigned int abytesize = maskbytesize(), bbytesize = _that.maskbytesize(), maxsize = std::max( abytesize, bbytesize );
  bool some_a_outside_B = false, some_b_outside_A = false;

  for( unsigned int idx = 0; idx < maxsize; ++ idx ) {
    uint8_t amask, abits, bmask, bbits;
    if( idx < abytesize ) { amask = this->m_mask[idx]; abits = this->m_bits[idx]; } else { amask = 0; abits = 0; }
    if( idx < bbytesize ) { bmask = _that.m_mask[idx]; bbits = _that.m_bits[idx]; } else { bmask = 0; bbits = 0; }
    
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
  
/**  @brief matches (or not) OpCode bytes according to the OpCode instance
 *   @param _oc the opcode which bytes are to be matched by the OpCode instance
 *   @return true on successful match
 */
bool
CiscGenerator::OpCode_t::match( OpCode_t const& _oc ) const {
  unsigned int bytesize = std::min( maskbytesize(), _oc.maskbytesize() );
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
  _sink << "{size: " << _oc.maskbytesize() << ", mask: 0x";
  for( unsigned int idx = 0; idx < _oc.maskbytesize(); ++ idx ) { byte = _oc.m_mask[idx]; _sink << hex[byte/16] << hex[byte%16]; }
  _sink << ", bits: 0x";
  for( unsigned int idx = 0; idx < _oc.maskbytesize(); ++ idx ) { byte = _oc.m_bits[idx]; _sink << hex[byte/16] << hex[byte%16]; }
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
  bool                               m_rewind, m_has_operand, m_has_subop;
  
  BFWordIterator( Vect_t<BitField_t> const& _bitfields )
    : m_right( _bitfields.begin() - 1 ), m_end( _bitfields.end() ),
      m_count( 0 ), m_minsize( 0 ), m_maxsize( 0 ), m_rewind( false ),
      m_has_operand( false ), m_has_subop( false )
  {}
  
  bool
  next() {
    m_left = m_right + 1;
    if( m_left >= m_end ) return false;
    m_count = 0; m_minsize = 0; m_maxsize = 0;
    m_has_operand = false; m_has_subop = false;
    for( m_right = m_left; (m_right < m_end) and ((**m_right).type() != BitField_t::Separator); ++m_right ) {
      m_count += 1;
      m_minsize += (**m_right).minsize();
      m_maxsize += (**m_right).maxsize();
      switch ((**m_right).type()) {
      case BitField_t::Operand: m_has_operand = true; break;
      case BitField_t::SubOp:   m_has_subop = true; break;
      default: break;
      }
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
  // Finalize size information
  m_code_capacity = (m_insn_maxsize + 7) / 8;
  
  // Process the opcodes needed by the decoder
  for( Vect_t<Operation_t>::const_iterator op = isa().m_operations.begin(); op < isa().m_operations.end(); ++ op ) {
    // compute prefix size
    unsigned int prefixsize = 0, insn_size = 0;
    bool vlen = false, outprefix = false, vword = false;
    
    for (FieldIterator fi( isa().m_little_endian, (**op).m_bitfields, m_insn_maxsize ); fi.next(); ) {
      if (fi.item().type() == BitField_t::Separator) {
        if (dynamic_cast<SeparatorBitField_t const&>( fi.item() ).m_rewind and vword) {
          (**op).m_fileloc.err( "error: operation `%s' rewinds a variable length word.", (**op).m_symbol.str() );
          throw GenerationError;
        }
        vword = false;
      }
      
      if (fi.item().minsize() != fi.item().maxsize()) {
        vword = true;
        outprefix = true;
        vlen = true;
      }
      
      insn_size = std::max( insn_size, fi.insn_size() );
      if (outprefix) continue;
      if (prefixsize < fi.insn_size()) prefixsize = fi.insn_size();
    }
    m_opcodes[*op].size_attrs( prefixsize, insn_size, vlen );
    
    // compute opcode
    OpCode_t& oc = opcode( *op );
    for (FieldIterator fi( isa().m_little_endian, (**op).m_bitfields, m_insn_maxsize ); fi.next(); ) {
      if (fi.item().minsize() != fi.item().maxsize()) break;
      if (not fi.item().hasopcode()) continue;
      uint64_t mask = fi.item().mask(), bits = fi.item().bits();
      bool little_endian = isa().m_little_endian;
      unsigned int pos = little_endian ? fi.pos() : fi.insn_size();
      while (mask) {
        unsigned int shift = little_endian ? (pos % 8) : (-pos % 8);
        unsigned int offset = little_endian ? (pos / 8) : ((pos-1) / 8);
        uint8_t partial_mask = mask << shift;
        uint8_t partial_bits = bits << shift;
        oc.m_mask[offset] |= partial_mask;
        oc.m_bits[offset] = (oc.m_bits[offset] & ~partial_mask) | (partial_bits & partial_mask);
        shift = 8 - shift;
        mask >>= shift;
        bits >>= shift;
        pos = little_endian ? pos + shift : pos - shift;
      }
    }
    oc.optimize( isa().m_little_endian );
  }
  
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
  bool little_endian = isa().m_little_endian;
  char const* dsh = little_endian ? ">>" : "<<";
  char const* ash = little_endian ? "<<" : ">>";
  _product.code( "struct CodeType {\n" );
  _product.code( " static const unsigned int capacity = %d;\n", m_code_capacity );
  _product.code( " unsigned int              size;\n" );
  _product.code( " uint8_t                   str[capacity];\n" );
  _product.code( " enum Exception_t { NotEnoughBytes };\n" );
  _product.code( " CodeType() : size( 0 ) {};\n" );
  _product.code( " CodeType( uint8_t* src, unsigned int sz )\n" );
  _product.code( " : size( std::min( sz, capacity*8 ) )\n{\n" );
  _product.code( "   for (int idx = (size+7)/8; (--idx) >= 0;) str[idx] = src[idx];\n" );
  _product.code( " }\n" );
  _product.code( " CodeType( CodeType const& ct )\n" );
  _product.code( " : size( ct.size )\n{\n" );
  for (unsigned int idx = 0; idx < m_code_capacity; ++idx)
    _product.code( " str[%u] = ct.str[%u];", idx, idx );
  _product.code( "\n }\n" );
  _product.code( " bool match( CodeType const& bits, CodeType const& mask ) const {\n" );
  _product.code( "  unsigned int maskbound = (mask.size+7)/8, codebound = (size+7)/8;\n" );
  _product.code( "  for (unsigned int idx = 0; idx < maskbound; ++idx) {\n" );
  _product.code( "   if (idx >= codebound) throw NotEnoughBytes;\n" );
  _product.code( "   if ((str[idx] & mask.str[idx]) != bits.str[idx]) return false;\n" );
  _product.code( "  };\n" );
  _product.code( "  return true;\n" );
  _product.code( " }\n" );
  _product.code( " bool match( CodeType const& bits ) const {\n" );
  _product.code( "  if (size < bits.size) throw NotEnoughBytes;\n" );
  _product.code( "  unsigned int end = bits.size/8;\n" );
  _product.code( "  for (unsigned int idx = 0; idx < end; idx += 1)\n" );
  _product.code( "   if (str[idx] != bits.str[idx]) return false;\n" );
  _product.code( "  unsigned int tail = (bits.size % 8);\n" );
  _product.code( "  if (tail == 0); return true;" );
  _product.code( "  uint8_t tailmask = 0xff %s (8-tail);\n", isa().m_little_endian ? ">>" : "<<" );
  _product.code( "  return ((str[end] ^ bits.str[end]) & tailmask) == 0;\n" );
  _product.code( " }\n" );
  _product.code( " CodeType& stretch_front( unsigned int shift ) {\n" );
  _product.code( "  int hish = shift / 8, losh = shift %% 8; \n" );
  _product.code( "  for (int dst = %u, src = dst - hish; dst >= 0; dst-=1, src-=1) {\n",
                 m_code_capacity - 1 );
  _product.code( "   if (src > 0) str[dst] = (str[src] %s losh) | (str[src-1] %s (8-losh));\n",
                 ash, dsh );
  _product.code( "   else if (src == 0) str[dst] = (str[src] %s losh);\n", ash );
  _product.code( "   else str[dst] = 0;\n" );
  _product.code( "  }\n" );
  _product.code( "  return *this;\n" );
  _product.code( " }\n" );
  _product.code( " CodeType& shrink_front( unsigned int shift ) {\n" );
  _product.code( "  int hish = shift / 8, losh = shift %% 8; \n" );
  _product.code( "  for (unsigned int dst = 0, src = dst + hish; dst < %u; dst+=1, src+=1) {\n",
                 m_code_capacity);
  _product.code( "   if (src < %u) str[dst] = (str[src] %s losh) | (str[src-1] %s (8-losh));\n",
                 m_code_capacity - 1, dsh, ash );
  _product.code( "   else if (src == %u) str[dst] = (str[src] %s losh);\n",
                 m_code_capacity - 1, dsh );
  _product.code( "   else str[dst] = 0;\n" );
  _product.code( "  }\n" );
  _product.code( "  return *this;\n" );
  _product.code( " }\n" );
  _product.code( " CodeType& extend( uint8_t* src, unsigned int sz ) {\n" );
  _product.code( "  CodeType tail( src, sz );\n" );
  _product.code( "  unsigned int mod = this->size %% 8; \n" );
  _product.code( "  if (mod) {\n" );
  _product.code( "   tail.size = std::min( tail.size + mod, capacity*8 );\n" );
  _product.code( "   tail.stretch_front( mod );\n" );
  _product.code( "   tail.str[0] = (tail.str[0] & (0xff %s mod)) | "
                 "(this->str[this->size/8] & (0xff %s (8-mod)));\n", ash, dsh );
  _product.code( "   this->size -= mod;\n" );
  _product.code( "  }\n" );
  _product.code( "  for (unsigned int src = 0, dst = this->size/8; "
                 "(src < this->capacity) and (dst < this->capacity); src+=1, dst+=1) {\n" );
  _product.code( "   this->str[dst] = tail.str[src];\n" );
  _product.code( "  }\n" );
  _product.code( "  this->size = std::min( this->size + tail.size, capacity*8 );\n" );
  _product.code( "  return *this;\n" );
  _product.code( " }\n" );
  _product.code( " friend std::ostream& operator << ( std::ostream& _sink, CodeType const& _ct );\n" );
  _product.code( "};\n" );
}

void
CiscGenerator::codetype_impl( Product_t& _product ) const {
  _product.code( "unsigned int const CodeType::capacity;\n" );
  _product.code( "std::ostream& operator << ( std::ostream& _sink, CodeType const& _ct ) {\n" );
  _product.code( " if (_ct.size % 8) {\n" );
  if (isa().m_little_endian) {
    _product.code( "  for (int idx = _ct.size; (--idx) >= 0; ) {\n" );
    _product.code( "   _sink << (((_ct.str[idx/8] >> (idx%8)) & 1) ? '1' : '0');\n" );
    _product.code( "  }\n" );
  } else {
    _product.code( "  for (unsigned int idx = 0; idx < _ct.size; ++idx ) {\n" );
    _product.code( "   _sink << (((_ct.str[idx/8] >> ((7-idx)%8)) & 1) ? '1' : '0');" );
    _product.code( "  }\n" );
  }
  _product.code( " } else {\n" );
  _product.code( "  char const* xrepr = \"0123456789abcdef\";\n" );
  if (isa().m_little_endian) {
    _product.code( "  for (int idx = _ct.size/8; (--idx) >= 0; ) {\n" );
  } else {
    _product.code( "  for (unsigned int idx = 0; idx < _ct.size/8; ++idx ) {\n" );
  }
  _product.code( "    uint8_t byte = _ct.str[idx/8];\n" );
  _product.code( "   _sink << xrepr[(byte >> 4) & 0xf] << xrepr[(byte >> 0) & 0xf];\n" );
  _product.code( "  }\n" );
  _product.code( " }\n" );
  _product.code( " return _sink;\n" );
  _product.code( "}\n" );
}

void
CiscGenerator::insn_bits_code( Product_t& _product, Operation_t const& _op ) const {
  OpCode_t const& oc = opcode( &_op );
  _product.code( "CodeType( (uint8_t*)( \"" );
  char const* hex = "0123456789abcdef";
  for( unsigned int idx = 0; idx < oc.maskbytesize(); ++idx )
    _product.code( "\\x%c%c", hex[oc.m_bits[idx]/16], hex[oc.m_bits[idx]%16] );
  _product.code( "\" ), %u )", oc.m_prefixsize );
}

void
CiscGenerator::insn_mask_code( Product_t& _product, Operation_t const& _op ) const {
  OpCode_t const& oc = opcode( &_op );
  _product.code( "CodeType( (uint8_t*)( \"" );
  char const* hex = "0123456789abcdef";
  for( unsigned int idx = 0; idx < oc.maskbytesize(); ++idx )
    _product.code( "\\x%c%c", hex[oc.m_mask[idx]/16], hex[oc.m_mask[idx]%16] );
  _product.code( "\" ), %u )", oc.m_prefixsize );
}

void
CiscGenerator::insn_match_ifexpr( Product_t& _product, char const* _code, char const* _mask, char const* _bits ) const {
  _product.code( "if( %s.match( %s, %s) )\n", _code, _bits, _mask );
}

void
CiscGenerator::insn_decode_impl( Product_t& _product, Operation_t const& _op, char const* _codename, char const* _addrname ) const
{
  OpCode_t const& oc = opcode( &_op );
  _product.code( "this->encoding.size = %u;\n", oc.m_fullsize );
  if (oc.m_vlen) {
    char const* ncodename = "_code_";
    _product.code( "CodeType %s( %s );\n", ncodename, _codename );
    _codename = ncodename;
  }
  
  bool little_endian = isa().m_little_endian;
  char const* dsh = little_endian ? ">>" : "<<";
  char const* ash = little_endian ? "<<" : ">>";
  
  for (FieldIterator fi( isa().m_little_endian, _op.m_bitfields, m_insn_maxsize ); fi.next(); ) {
    if (fi.item().type() == BitField_t::SubOp) {
      SubOpBitField_t const& sobf = dynamic_cast<SubOpBitField_t const&>( fi.item() );
      SDInstance_t const* sdinstance = sobf.m_sdinstance;
      SDClass_t const* sdclass = sdinstance->m_sdclass;
      SourceCode_t const* tpscheme =  sdinstance->m_template_scheme;
      
      _product.code( "{\n" );
      _product.code( "%s::CodeType _subcode_;\n", sdclass->qd_namespace().str() );
      _product.code( "_subcode_.size = %u;\n", sdclass->m_maxsize );
      
      unsigned int shift = fi.insn_size() - fi.m_size;
      unsigned int byteshift = shift / 8;
      shift = shift % 8;
      unsigned int subbytes = (sdclass->m_maxsize + 7) / 8;
      
      for (unsigned int idx = 0; idx < subbytes; ++idx) {
        unsigned int didx = idx + byteshift;
        _product.code( "_subcode_.str[%u] = ", idx );
        _product.code( "uint8_t( %s.str[%u] %s %u )", _codename, didx, dsh, shift );
        if (shift > 0 and (didx + 1) < m_code_capacity)
          _product.code( "| uint8_t( %s.str[%u] %s %u )", _codename, didx + 1, ash, 8 - shift );
        _product.code( ";\n" );
      }
      
      _product.code( "%s = %s::sub_decode", sobf.m_symbol.str(), sdclass->qd_namespace().str() );
      if( tpscheme )
        _product.usercode( tpscheme->m_fileloc, "< %s >", tpscheme->m_content.str() );
      _product.code( "( %s, _subcode_ );\n", _addrname );
      _product.code( "unsigned int shortening = %u - %s->GetLength();\n",
                     sdclass->m_maxsize, sobf.m_symbol.str() );
      _product.code( "this->encoding.size -= shortening;\n" );
      _product.code( "%s.stretch_front( shortening );\n", _codename );
      _product.code( "}\n" );
    }
    
    else if (fi.item().type() == BitField_t::Operand) {
      OperandBitField_t const& opbf = dynamic_cast<OperandBitField_t const&>( fi.item() );
      _product.code( "%s = ", opbf.m_symbol.str() );
      unsigned int opsize = std::max( least_ctype_size( opbf.dstsize() ), m_minwordsize );
      char const* sep = "";
      
      for (unsigned int end = fi.insn_size(), start = end - fi.m_size,
             substart = start, subend; substart < end; substart = subend)
        {
          subend = std::min( (substart + 8) & -8, end);
          unsigned int
            bound = 1 << (subend - substart),
            pos = little_endian ? (substart % 8) : ((-subend) % 8),
            bytepos = substart / 8,
            dstpos = little_endian ? (substart - start) : (end - subend);
          
          _product.code( "%s(((uint%u_t( %s.str[%u] ) >> %u) %% %u) << %u)", sep, opsize, _codename, bytepos, pos, bound, dstpos );
          sep = " | ";
        }
      _product.code( ";\n" );
      
      if( opbf.m_sext ) {
        int sext_shift = opsize - opbf.m_size;
        _product.code( "%s = (%s << %u) >> %u;\n", opbf.m_symbol.str(), opbf.m_symbol.str(), sext_shift, sext_shift );
      }
    
      if( opbf.m_shift > 0 )
        _product.code( "%s >>= %u;\n", opbf.m_symbol.str(), +opbf.m_shift );
      if( opbf.m_shift < 0 )
        _product.code( "%s <<= %u;\n", opbf.m_symbol.str(), -opbf.m_shift );
    }
  }
}

void
CiscGenerator::insn_unchanged_expr( Product_t& _product, char const* _ref, char const* _bytes ) const {
  _product.code( "%s.match( %s )", _bytes, _ref );
}

void
CiscGenerator::subdecoder_bounds( Product_t& _product ) const {
  _product.code( "[%d:%d]", m_insn_minsize, m_insn_maxsize );
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
  Vect_t<BitField_t> const& bfs = _op.m_bitfields;
  
  for( Vect_t<BitField_t>::const_iterator bf = bfs.begin(); bf < bfs.end(); ++bf ) {
    if ((**bf).type() == BitField_t::SubOp) { subops = true; break; }
  }
  
  if( not subops ) return;
  _product.code( "~Op%s();\n", Str::capitalize( _op.m_symbol ).str() );
}

void
CiscGenerator::insn_destructor_impl( Product_t& _product, Operation_t const& _op ) const {
  std::vector<ConstStr_t> subops;
  Vect_t<BitField_t> const& bfs = _op.m_bitfields;
  
  for( Vect_t<BitField_t>::const_iterator bf = bfs.begin(); bf < bfs.end(); ++bf ) {
    if ((**bf).type() != BitField_t::SubOp) continue;
    SubOpBitField_t const& sobf = dynamic_cast<SubOpBitField_t const&>( **bf );
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

void
CiscGenerator::op_getlen_decl( Product_t& _product ) const {
  _product.code( "inline unsigned int GetLength() const { return this->encoding.size; }\n" );
}
