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

/**
 *  @brief  Default constructor, create a RiscGenerator.
 */
RiscGenerator::RiscGenerator( Isa& _source, Opts const& _options )
  : Generator( _source, _options ), m_insn_ctypesize( 0 )
{};

/** Process the isa structure and computes RISC specific data 
*/
void
RiscGenerator::finalize()
{
  // Finalize size information
  if ((*source.m_insnsizes.rbegin()) > 64) {
    std::cerr << "error: can't process encodings wider than 64 bits (" << (*source.m_insnsizes.rbegin()) << " bits).";
    throw GenerationError;
  }
  
  m_insn_ctypesize = least_ctype_size( (*source.m_insnsizes.rbegin()) );
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
RiscGenerator::codetype_decl( Product& _product ) const {
  _product.code( "typedef %s CodeType;\n\n", m_insn_ctype.str() );
}

void
RiscGenerator::insn_encode_impl( Product& _product, Operation const& op, char const* _codename ) const
{
  if (op.vlen)
    {
      _product.code( "assert( \"Encode method does not work with variable length operations.\" and false );\n" );
    }

  uint64_t bits = source.m_little_endian ? op.code.getbits(0, op.size) : op.code.getbits(-op.size, 0);
  _product.code( "%s = 0x%llx%s;\n", _codename, bits, m_insn_cpostfix.str() );
  
  for (FieldIterator fi( op.bitfields, source.m_little_endian ); fi.next(); )
    {
      if      (dynamic_cast<SubOpBitField const*>( &fi.bitfield() ))
        {
          _product.code( "assert( \"Encode method does not work with sub-operations.\" and false );\n" );
        }
      else if (OperandBitField const* opbf = dynamic_cast<OperandBitField const*>( &fi.bitfield() ))
        {
          int opsize = membersize( *opbf );
          ConstStr shiftedop = Str::fmt( "uint%d_t( %s )", opsize, opbf->symbol.str() );
          if (opbf->lshift > 0)
            shiftedop = Str::fmt( "(%s >> %u)", shiftedop.str(), opbf->lshift );
          _product.code( "%s |= ((%s & 0x%llx) << %u);\n", _codename, shiftedop.str(), opbf->mask(), fi.pos( source.maxsize() ) );
        }
    }
}

void
RiscGenerator::insn_decode_impl( Product& product, Operation const& op, char const* codename, char const* addrname ) const
{
  if (op.vlen)
    {
      product.code( "this->gil_length = %u;\n", op.size );
    }
  
  for (FieldIterator fi( op.bitfields, source.m_little_endian ); fi.next();)
    {
      if (SubOpBitField const* sobf = dynamic_cast<SubOpBitField const*>( &fi.bitfield() ))
        {
          SDInstance const* sdinstance = sobf->sdinstance;
          SDClass const* sdclass = sdinstance->sdclass;
          SourceCode const* tpscheme =  sdinstance->template_scheme;
      
          product.code( "%s = ", sobf->symbol.str() ).usercode( sdclass->nmcode ).code("::sub_decode" );
          if (tpscheme)
            product.code( "< " ).usercode( *tpscheme ).code( " >" );
          product.code( "( %s, ((%s >> %u) & 0x%llx) );\n", addrname, codename, fi.pos(source.maxsize()), (1ull<<sdclass->maxsize())-1 );

          if (sdclass->minsize() != sdclass->maxsize()) {
            product.code( "{\nunsigned int shortening = %u - %s->GetLength();\n", sdclass->maxsize(), sobf->symbol.str() );
            product.code( "this->gil_length -= shortening;\n" );
            product.code( "%s %s= shortening;\n}\n", codename, (source.m_little_endian ? "<<" : ">>") );
          }
        }
    
    else if (OperandBitField const* opbf = dynamic_cast<OperandBitField const*>( &fi.bitfield() ))
      {
        product.code( "%s = ", opbf->symbol.str() );

        int opsize = membersize( *opbf );

        ConstStr value = codename;
        unsigned pos = fi.pos(source.maxsize());
        if (pos)
          value = Str::fmt("(%s >> %u)", value.str(), pos);
        value = Str::fmt("%s & 0x%x", value.str(), opbf->mask());
        
        if (opbf->sext)
          {
            int sext_shift = opsize - opbf->size;
            value = Str::fmt("((int%d_t)((%s) << %u) >> %u)", opsize, value.str(), sext_shift, sext_shift );
          }
        else
          value = Str::fmt("uint%d_t(%s)", opsize, value.str() );
    
        if (opbf->lshift > 0)
          value = Str::fmt("%s << %u", value.str(), opbf->lshift);
        
        product.code( "%s;\n", value.str() );
      }
    }
}

void
RiscGenerator::insn_bits_code( Product& product, Operation const& op ) const
{
  unsigned size = source.maxsize();
  uint64_t bits = source.m_little_endian ? op.code.getbits(0, size) : op.code.getbits(-size, 0);
  product.code( "0x%x%s", bits, m_insn_cpostfix.str() );
}

void
RiscGenerator::insn_mask_code( Product& _product, Operation const& op ) const
{
  unsigned size = source.maxsize();
  uint64_t mask = source.m_little_endian ? op.code.getmask(0, size) : op.code.getmask(-size, 0);
  _product.code( "0x%llx%s", mask, m_insn_cpostfix.str() );
}

ConstStr
RiscGenerator::insn_id_expr( char const* addrname ) const
{
  if ((*source.m_insnsizes.rbegin()) != (*source.m_insnsizes.begin()))
    return addrname;
  return Str::fmt( "%s / %d", addrname, ((*source.m_insnsizes.rbegin()) / 8) );
}

void
RiscGenerator::insn_match_ifexpr( Product& _product, char const* _code, char const* _mask, char const* _bits ) const
{
  _product.code( "if((%s & %s) == %s)\n", _code, _mask, _bits );
}

void
RiscGenerator::insn_unchanged_expr( Product& _product, char const* _old, char const* _new ) const
{
  _product.code( "%s == %s", _old, _new );
}

void
RiscGenerator::op_getlen_decl( Product& _product ) const
{
  if ((*source.m_insnsizes.begin()) == (*source.m_insnsizes.rbegin())) {
    _product.code( "inline unsigned int GetLength() const { return %d; }\n", (*source.m_insnsizes.rbegin()) );
  } else {
    _product.code( "virtual unsigned int GetLength() const { return 0; };\n" );
  }
}

void
RiscGenerator::insn_getlen_decl( Product& product, Operation const& op ) const
{
  if ((*source.m_insnsizes.begin()) == (*source.m_insnsizes.rbegin()))
    return;
  
  if (op.vlen)
    {
      product.code( "unsigned int gil_length;\n" );
      product.code( "unsigned int GetLength() const { return this->gil_length; }\n" );
    }
  else
    {
      unsigned int size = op.size;
      product.code( "unsigned int GetLength() const { return %d; }\n", size );
    }
}
