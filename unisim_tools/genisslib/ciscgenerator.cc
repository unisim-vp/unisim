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

/**
 *  @brief  Default constructor, create a CiscGenerator.
 */
CiscGenerator::CiscGenerator( Isa& _source, Opts const& _options )
  : Generator( _source, _options ), m_code_capacity( 0 )
{};

/** Process the isa structure and computes CISC specific data 
*/
void
CiscGenerator::finalize()
{
  // Finalize size information
  m_code_capacity = ((*source.m_insnsizes.rbegin()) + 7) / 8;
}

void
CiscGenerator::additional_decl_includes( Product& _product ) const {
  _product.code( "#include <iosfwd>\n" );
}

void
CiscGenerator::additional_impl_includes( Product& _product ) const {
  _product.code( "#include <ostream>\n" );
  _product.code( "#include <cstring>\n" );
}

void
CiscGenerator::codetype_decl( Product& _product ) const {
  char const* dsh = source.m_little_endian ? ">>" : "<<";
  char const* ash = source.m_little_endian ? "<<" : ">>";
  _product.code( "struct CodeType {\n" );
  _product.code( " enum { capacity = %d };\n", m_code_capacity );
  _product.code( " unsigned int              size;\n" );
  _product.code( " uint8_t                   str[capacity];\n" );
  _product.code( " enum Exception_t { NotEnoughBytes };\n" );
  _product.code( " CodeType() : size( 0 ) { for (int idx = capacity; (--idx) >= 0;) str[idx] = 0; }\n" );
  _product.code( " CodeType( unsigned int sz )\n" );
  _product.code( " : size( std::min( sz, capacity*8u ) )\n{\n" );
  _product.code( "   for (int idx = capacity; (--idx) >= 0;) str[idx] = 0;\n" );
  _product.code( " }\n" );
  _product.code( " CodeType( uint8_t const* src, unsigned int sz )\n" );
  _product.code( " : size( std::min( sz, capacity*8u ) )\n{\n" );
  _product.code( "   for (int idx = capacity; (--idx) >= 0;) str[idx] = 0;\n" );
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
  _product.code( "  }\n" );
  _product.code( "  return true;\n" );
  _product.code( " }\n" );
  _product.code( " bool match( CodeType const& bits ) const {\n" );
  _product.code( "  if (size < bits.size) throw NotEnoughBytes;\n" );
  _product.code( "  unsigned int end = bits.size/8;\n" );
  _product.code( "  for (unsigned int idx = 0; idx < end; idx += 1)\n" );
  _product.code( "   if (str[idx] != bits.str[idx]) return false;\n" );
  _product.code( "  unsigned int tail = (bits.size % 8);\n" );
  _product.code( "  if (tail == 0) return true;\n" );
  _product.code( "  uint8_t tailmask = 0xff %s (8-tail);\n", dsh );
  _product.code( "  return ((str[end] ^ bits.str[end]) & tailmask) == 0;\n" );
  _product.code( " }\n" );
  _product.code( " CodeType& stretch_front( unsigned int shift ) {\n" );
  _product.code( "  int hish = shift / 8, losh = shift %% 8; \n" );
  _product.code( "  for (int dst = %u, src = dst - hish; dst >= 0; dst-=1, src-=1) {\n", m_code_capacity - 1 );
  _product.code( "   if (src > 0) str[dst] = (str[src] %s losh) | (str[src-1] %s (8-losh));\n", ash, dsh );
  _product.code( "   else if (src == 0) str[dst] = (str[src] %s losh);\n", ash );
  _product.code( "   else str[dst] = 0;\n" );
  _product.code( "  }\n" );
  _product.code( "  return *this;\n" );
  _product.code( " }\n" );
  _product.code( " CodeType& shrink_front( unsigned int shift ) {\n" );
  _product.code( "  int hish = shift / 8, losh = shift %% 8; \n" );
  _product.code( "  for (unsigned int dst = 0, src = dst + hish; dst < %u; dst+=1, src+=1) {\n", m_code_capacity);
  _product.code( "   if (src < %u) str[dst] = (str[src] %s losh) | (str[src-1] %s (8-losh));\n", m_code_capacity - 1, dsh, ash );
  _product.code( "   else if (src == %u) str[dst] = (str[src] %s losh);\n", m_code_capacity - 1, dsh );
  _product.code( "   else str[dst] = 0;\n" );
  _product.code( "  }\n" );
  _product.code( "  return *this;\n" );
  _product.code( " }\n" );
  _product.code( " CodeType& extend( uint8_t* src, unsigned int sz ) {\n" );
  _product.code( "  CodeType tail( src, sz );\n" );
  _product.code( "  unsigned int mod = this->size %% 8; \n" );
  _product.code( "  if (mod) {\n" );
  _product.code( "   tail.size = std::min( tail.size + mod, capacity*8u );\n" );
  _product.code( "   tail.stretch_front( mod );\n" );
  _product.code( "   tail.str[0] = (tail.str[0] & (0xff %s mod)) | (this->str[this->size/8] & (0xff %s (8-mod)));\n", ash, dsh );
  _product.code( "   this->size -= mod;\n" );
  _product.code( "  }\n" );
  _product.code( "  for (unsigned int src = 0, dst = this->size/8; (src < capacity*1u) and (dst < capacity*1u); src+=1, dst+=1) {\n" );
  _product.code( "   this->str[dst] = tail.str[src];\n" );
  _product.code( "  }\n" );
  _product.code( "  this->size = std::min( this->size + tail.size, capacity*8u );\n" );
  _product.code( "  return *this;\n" );
  _product.code( " }\n" );
  _product.code( " friend std::ostream& operator << ( std::ostream& _sink, CodeType const& _ct );\n" );
  _product.code( "};\n" );
}

void
CiscGenerator::codetype_impl( Product& _product ) const
{
  if (not source.m_tparams.empty())
    _product.code( "inline\n" );
  _product.code( "std::ostream& operator << ( std::ostream& _sink, CodeType const& _ct ) {\n" );
  _product.code( " if (_ct.size % 8) {\n" );
  if (source.m_little_endian) {
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
  if (source.m_little_endian) {
    _product.code( "  for (int idx = _ct.size/8; (--idx) >= 0; ) {\n" );
  } else {
    _product.code( "  for (unsigned int idx = 0; idx < _ct.size/8; ++idx ) {\n" );
  }
  _product.code( "    uint8_t byte = _ct.str[idx];\n" );
  _product.code( "   _sink << xrepr[(byte >> 4) & 0xf] << xrepr[(byte >> 0) & 0xf];\n" );
  _product.code( "  }\n" );
  _product.code( " }\n" );
  _product.code( " return _sink;\n" );
  _product.code( "}\n" );
}

unsigned
CiscGenerator::prefix_size( Operation const& op ) const
{
  bool little_endian = source.m_little_endian;
  
  for (int end = op.size; end > 0; --end)
    if (little_endian ? op.code.getmask(end-1, end) : op.code.getmask(-end, -end+1))
      return end;
  return 0;
}

void
CiscGenerator::insn_bits_code( Product& product, Operation const& op ) const
{
  unsigned codesize = prefix_size(op);
  product.code( "CodeType( (uint8_t*)( \"" );
  for( unsigned int idx = 0, end = (codesize + 7) / 8; idx < end; ++idx )
    product.code( "\\x%02x", op.code.bits[idx] );
  product.code( "\" ), %u )", codesize );
}

void
CiscGenerator::insn_mask_code( Product& product, Operation const& op ) const
{
  unsigned codesize = prefix_size(op);
  product.code( "CodeType( (uint8_t*)( \"" );
  for( unsigned int idx = 0, end = (codesize + 7) / 8; idx < end; ++idx )
    product.code( "\\x%02x", op.code.mask[idx] );
  product.code( "\" ), %u )", codesize );
}

void
CiscGenerator::insn_match_ifexpr( Product& product, char const* _code, char const* _mask, char const* _bits ) const
{
  product.code( "if( %s.match( %s, %s) )\n", _code, _bits, _mask );
}

void
CiscGenerator::insn_encode_impl( Product& product, Operation const& op, char const* codename ) const
{
  if (op.vlen)
    {
      product.code( "assert( \"Encode method does not work with variable length operations.\" and false );\n" );
    }
  
  { /* writing  common operation bits */
    product.code( "%s = CodeType( (uint8_t*)( \"", codename );
    for (unsigned idx = 0, end = (op.size + 7) / 8; idx < end; ++idx)
      {
        product.code( "\\x%02x", op.code.bits[idx] );
      }
    product.code( "\" ), %u );\n", op.size );
  }
  
  bool little_endian = source.m_little_endian;
  
  for (FieldIterator fi( op.bitfields, little_endian ); fi.next(); )
    {
      if (dynamic_cast<SubOpBitField const*>( &fi.bitfield() ))
        {
          product.code( "assert( \"Encode method does not work with sub-operations.\" and false );\n" );
        }
    
      else if (OperandBitField const* opbf = dynamic_cast<OperandBitField const*>( &fi.bitfield() ))
        {
          unsigned int opsize = membersize( *opbf );
          ConstStr shiftedop = Str::fmt( "uint%u_t( %s )", opsize, opbf->symbol.str() );
          if (opbf->lshift > 0)
            shiftedop = Str::fmt( "(%s >> %u)", shiftedop.str(), opbf->lshift );

          for (int src = fi.beg; src < fi.end; )
            {
              int nsrc = std::min((src+8) & -8, fi.end);
              unsigned shift = src & 7;
              unsigned byte = (src >> 3) ^ ((src < 0) * -1);
              unsigned mask = (1 << (nsrc-src)) - 1;
              assert (mask);

              product.code( "%s.str[%u] |= (((%s >> %u) & 0x%x) << %u);\n", codename, byte, shiftedop.str(), src-fi.beg, mask, shift );
              
              src = nsrc;
            }
        }
    }
}

void
CiscGenerator::insn_decode_impl( Product& product, Operation const& op, char const* codename, char const* addrname ) const
{
  product.code( "if (this->encoding.size < %u) throw CodeType::NotEnoughBytes;\n", op.size );
  product.code( "this->encoding.size = %u;\n", op.size );
  if (op.vlen)
    {
      char const* ncodename = "_code_";
      product.code( "CodeType %s( %s );\n", ncodename, codename );
      codename = ncodename;
    }
  
  bool little_endian = source.m_little_endian;
  char const* dsh = little_endian ? ">>" : "<<";
  char const* ash = little_endian ? "<<" : ">>";
  
  for (FieldIterator fi( op.bitfields, little_endian ); fi.next(); )
    {
      if (SubOpBitField const* sobf = dynamic_cast<SubOpBitField const*>( &fi.bitfield() ))
        {
          SDInstance const* sdinstance = sobf->sdinstance;
          SDClass const* sdclass = sdinstance->sdclass;
          SourceCode const* tpscheme =  sdinstance->template_scheme;
      
          product.code( "{\n" );
          product.usercode(sdclass->nmcode).code( "::CodeType _subcode_;\n" );
          product.code( "_subcode_.size = %u;\n", sdclass->maxsize() );
      
          unsigned int shift = source.m_little_endian ? fi.beg : -fi.end;
          unsigned int byteshift = shift / 8;
          shift = shift % 8;
          unsigned int subbytes = (sdclass->maxsize() + 7) / 8;
      
          for (unsigned int idx = 0; idx < subbytes; ++idx) {
            unsigned int didx = idx + byteshift;
            product.code( "_subcode_.str[%u] = ", idx );
            product.code( "uint8_t( %s.str[%u] %s %u )", codename, didx, dsh, shift );
            if (shift > 0 and (didx + 1) < m_code_capacity)
              product.code( "| uint8_t( %s.str[%u] %s %u )", codename, didx + 1, ash, 8 - shift );
            product.code( ";\n" );
          }
      
          product.code( "%s = ", sobf->symbol.str() ).usercode( sdclass->nmcode ).code( "::sub_decode" );
          if (tpscheme)
            product.code( "< " ).usercode( *tpscheme ).code( " >" );
          product.code( "( %s, _subcode_ );\n", addrname );
          product.code( "unsigned int shortening = %u - %s->GetLength();\n", sdclass->maxsize(), sobf->symbol.str() );
          product.code( "this->encoding.size -= shortening;\n" );
          product.code( "%s.stretch_front( shortening );\n", codename );
          product.code( "}\n" );
        }
    
      else if (OperandBitField const* opbf = dynamic_cast<OperandBitField const*>( &fi.bitfield() ))
        {
          product.code( "%s = ", opbf->symbol.str() );
          unsigned int opsize = membersize( *opbf );

          char const* sep = "";
          for (int src = fi.beg; src < fi.end; )
            {
              int nsrc = std::min((src+8) & -8, fi.end);
              unsigned shift = src & 7;
              unsigned byte = (src >> 3) ^ ((src < 0) * -1);
              unsigned mask = (1 << (nsrc-src)) - 1;
              assert (mask);
              unsigned dst = src-fi.beg;

              ConstStr component = Str::fmt("uint%u_t( %s.str[%u] )", opsize, codename, byte );
              if (shift) component = Str::fmt("(%s >> %u)", component.str(), shift);
              component = Str::fmt("(%s & 0x%x)", component.str(), mask);
              if (dst) component = Str::fmt("(%s << %u)", component.str(), dst);
              product.code( sep ).write( component.str() );
              
              src = nsrc;
              sep = " | ";
            }
          
          product.code( ";\n" );
      
          if (opbf->sext) {
            int sext_shift = opsize - opbf->size;
            product.code( "%s = (int%u_t)(%s << %u) >> %u;\n", opbf->symbol.str(), opsize, opbf->symbol.str(), sext_shift, sext_shift );
          }
    
          if (opbf->lshift > 0)
            product.code( "%s <<= %u;\n", opbf->symbol.str(), opbf->lshift );
        }
    }
}

void
CiscGenerator::op_match( Product& _product, char const* codename ) const
{
  _product.code( "%s.match( GetEncoding() )", codename );
}

void
CiscGenerator::insn_destructor_decl( Product& _product, Operation const& _op ) const
{
  bool subops = false;
  Vector<BitField> const& bfs = _op.bitfields;
  
  for( Vector<BitField>::const_iterator bf = bfs.begin(); bf < bfs.end(); ++bf ) {
    if (dynamic_cast<SubOpBitField const*>( &**bf )) { subops = true; break; }
  }
  
  if (not subops) return;
  _product.code( "~Op%s();\n", Str::capitalize( _op.symbol.str() ).str() );
}

void
CiscGenerator::insn_destructor_impl( Product& _product, Operation const& _op ) const
{
  std::vector<ConstStr> subops;
  Vector<BitField> const& bfs = _op.bitfields;
  
  for( Vector<BitField>::const_iterator bf = bfs.begin(); bf < bfs.end(); ++bf ) {
    if (SubOpBitField const* sobf = dynamic_cast<SubOpBitField const*>( &**bf ))
      subops.push_back( sobf->symbol );
  }
  
  if( subops.size() == 0 ) return;
  
  _product.template_signature( source.m_tparams );
  _product.code( "Op%s", Str::capitalize( _op.symbol.str() ).str() );
  _product.template_abbrev( source.m_tparams );
  _product.code( "::~Op%s()\n", Str::capitalize( _op.symbol.str() ).str() );
  
  _product.code( "{\n" );
  for( std::vector<ConstStr>::const_iterator name = subops.begin(); name != subops.end(); ++name ) {
    _product.code( "delete %s;\n", name->str() );
  }
  _product.code( "}\n\n" );
}

void
CiscGenerator::op_getlen_decl( Product& _product ) const {
  _product.code( "inline unsigned int GetLength() const { return this->encoding.size; }\n" );
}
