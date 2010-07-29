/***************************************************************************
                              generator.cc
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

#include <generator.hh>
#include <scanner.hh>
#include <isa.hh>
#include <action.hh>
#include <bitfield.hh>
#include <comment.hh>
#include <operation.hh>
#include <sourcecode.hh>
#include <subdecoder.hh>
#include <variable.hh>
#include <product.hh>
#include <strtools.hh>
#include <vect.hh>
#include <main.hh>
#include <iostream>
#include <limits>

using namespace std;

Generator::Generator()
  : m_isa( 0 ), m_minwordsize( 32 ), m_insn_maxsize( 0 ), m_insn_minsize( 0 )
{}

struct OpProperties {
  unsigned int m_minsize, m_maxsize;

  OpProperties( Operation_t const& op )
    : m_minsize( 0 ), m_maxsize( 0 )
  {
    Vect_t<BitField_t> const& bitfields = op.m_bitfields;
    
    unsigned int minsize = 0, maxsize = 0, lastminsize = 0, lastmaxsize = 0;

    for (Vect_t<BitField_t>::const_iterator bf = bitfields.begin(); bf < bitfields.end(); ++ bf) {
      BitField_t const& bitfield = **bf;
      if (bitfield.type() == BitField_t::Separator) {
        SeparatorBitField_t const& sepbf = dynamic_cast<SeparatorBitField_t const&>( bitfield );
        if (not sepbf.m_rewind) {
          lastminsize = minsize;
          lastmaxsize = maxsize;
          continue;
        }
        if (m_minsize < minsize) m_minsize = minsize;
        if (m_maxsize < maxsize) m_maxsize = maxsize;
        minsize = lastminsize;
        maxsize = lastmaxsize;
      }
      minsize += bitfield.minsize();
      maxsize += bitfield.maxsize();
    }
    if (m_minsize < minsize) m_minsize = minsize;
    if (m_maxsize < maxsize) m_maxsize = maxsize;
  }
};

Generator&
Generator::init( Isa& _isa ) {
  m_isa = &_isa;
  m_minwordsize = least_ctype_size( Opts::shared().minwordsize );
  
  { // change bitfield ordering
    bool rev_forder = isa().m_asc_forder xor isa().m_little_endian;
  
    if (isa().m_asc_worder xor isa().m_little_endian) {
      for( Vect_t<Operation_t>::iterator op = isa().m_operations.begin(); op < isa().m_operations.end(); ++ op ) {
        Vect_t<BitField_t>& bitfields = (**op).m_bitfields;
        uintptr_t lo = 0, hi = bitfields.size();
        if (hi == 0) continue;
      
        for (hi -= 1; lo < hi; lo ++, hi-- ) {
          std::swap( bitfields[lo], bitfields[hi] );
        }
      }
      // this operation has invert fields order
      rev_forder = not rev_forder;
    }
  
    if (rev_forder) {
      for( Vect_t<Operation_t>::iterator op = isa().m_operations.begin(); op < isa().m_operations.end(); ++ op ) {
        Vect_t<BitField_t>& bitfields = (**op).m_bitfields;
      
        uintptr_t fbeg = 0, fend = 0, fmax = bitfields.size();
        for ( ; fbeg < fmax; fbeg = fend = fend + 1) {
          while ((fend < fmax) and (bitfields[fend]->type() != BitField_t::Separator)) fend += 1;
          if ((fend - fbeg) < 2) continue;
          uintptr_t lo = fbeg, hi = fend - 1;
        
          for ( ; lo < hi; lo ++, hi-- ) {
            std::swap( bitfields[lo], bitfields[hi] );
          }
        }
      }
    }
  }
  
  // Compute min/max sizes
  m_insn_maxsize = 0;
  m_insn_minsize = std::numeric_limits<unsigned int>::max();

  for( Vect_t<Operation_t>::const_iterator op = isa().m_operations.begin(); op < isa().m_operations.end(); ++ op ) {
    OpProperties opprops( **op );
    if( m_insn_maxsize < opprops.m_maxsize  ) m_insn_maxsize = opprops.m_maxsize;
    if( m_insn_minsize > opprops.m_minsize  ) m_insn_minsize = opprops.m_minsize;
  }

  std::cerr << "Instruction Size: ";
  if (m_insn_minsize != m_insn_maxsize)
    std::cerr << "[" << m_insn_minsize << ":" << m_insn_maxsize << "]";
  else
    std::cerr << m_insn_maxsize;
  std::cerr << std::endl;
  
  
  return *this;
}

/** Generates one C source file and one C header
    @param output a C string containing the name of the output filenames without the file name extension
    @param word_size define the minimum word size to hold the operand bit field,
    if zero uses the smallest type which hold the operand bit field
    @return non-zero if no error occurs during generation
*/
void
Generator::iss( char const* prefix, bool sourcelines ) const {
  std::cerr << "Instruction Set Encoding: " << (isa().m_little_endian ? "little-endian" : "big-endian") << "\n";
  
  /*******************/
  /*** Header file ***/
  /*******************/
  {
    FProduct_t sink( prefix, ".hh", sourcelines );
    if (not sink.good()) {
      cerr << GENISSLIB << ": can't open header file '" << sink.m_filename << "'.\n";
      throw GenerationError;
    }
    
    sink.code( "/*** File generated by %s version %s: DO NOT MODIFY ***/\n", GENISSLIB, GENISSLIB_VERSION );
    
    ConstStr_t headerid;
    
    {
      Str::Buf ns_header_str( Str::Buf::Recycle );
      char const* sep = "";
      for( vector<ConstStr_t>::const_iterator piece = isa().m_namespace.begin(); piece < isa().m_namespace.end(); ++ piece ) {
        ns_header_str.write( sep ).write( (*piece).str() );
        sep = "__";
      }
      headerid = Str::fmt( "__%s_%s_HH__", Str::tokenize( prefix ).str(), ns_header_str.m_storage );
    }

    sink.code( "#ifndef %s\n", headerid.str() );
    sink.code( "#define %s\n", headerid.str() );
  
    sink.code( "#ifndef __STDC_FORMAT_MACROS\n"
                   "#define __STDC_FORMAT_MACROS\n"
                   "#endif\n\n" );
  
    sink.code( "#include <vector>\n" );
    sink.code( "#include <inttypes.h>\n" );
    sink.code( "#include <cstring>\n" );
  
    //   sink.code( "#ifndef GCC_VERSION\n" );
    //   sink.code( "#define GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)\n" );
    //   sink.code( "#endif\n" );

    additional_decl_includes( sink );
  
    sink.ns_enter( isa().m_namespace );
  
    codetype_decl( sink );
    decoder_decl( sink );
  
    sink.ns_leave( isa().m_namespace );

    for( Vect_t<SourceCode_t>::const_iterator srccode = isa().m_decl_srccodes.begin(); srccode < isa().m_decl_srccodes.end(); ++ srccode ) {
      sink.usercode( **srccode );
    }
  
    sink.ns_enter( isa().m_namespace );
  
    operation_decl( sink );
  
    sink.ns_leave( isa().m_namespace );
  
    sink.code( "#endif\n" );
  
    sink.flush();
  }
  
  /*******************/
  /*** Source file ***/
  /*******************/
  {
    FProduct_t sink( prefix, isa().m_tparams.empty() ? ".cc" : ".tcc", sourcelines );
    if (not sink.good()) {
      cerr << GENISSLIB << ": can't open header file '" << sink.m_filename << "'.\n";
      throw GenerationError;
    }
  
    //  sink.code( "/*** File generated by %s version %s: DO NOT MODIFY ***/\n", GENISSLIB, GENISSLIB_VERSION );
  
    sink.code( "#include \"%s.hh\"\n", prefix );
    sink.code( "#include <cassert>\n" );
    //   sink.code( "#include <cstdlib>\n"
    //                  "#include <cstring>\n\n\n\n" );
    additional_impl_includes( sink );
  
    for( Vect_t<SourceCode_t>::const_iterator srccode = isa().m_impl_srccodes.begin(); srccode < isa().m_impl_srccodes.end(); ++ srccode ) {
      sink.usercode( **srccode );
    }
  
    sink.ns_enter( isa().m_namespace );
  
    codetype_impl( sink );
    operation_impl( sink );
  
    isa_operations_decl( sink );
    isa_operations_methods( sink );
    isa_operations_ctors( sink );
    isa_operations_encoders( sink );
  
    decoder_impl( sink );
  
    sink.ns_leave( isa().m_namespace );
  
    sink.flush();
  }
  
  /******************************/
  /*** Subdecoder header file ***/
  /******************************/
  if (isa().m_is_subdecoder) {
    FProduct_t sink( prefix, "_sub.isa", sourcelines );
    if (not sink.good()) {
      cerr << GENISSLIB << ": can't open header file '" << sink.m_filename << "'.\n";
      throw GenerationError;
    }
    sink.code( "subdecoder" );
    std::vector<ConstStr_t> const& nmspc = isa().m_namespace;
    char const* sep = " ";
    for( std::vector<ConstStr_t>::const_iterator ns = nmspc.begin(); ns < nmspc.end(); sep = "::", ++ ns )
      sink.code( "%s%s", sep, (*ns).str() );
    sink.code( " " );
    subdecoder_bounds( sink );
    sink.code( "\n" );
    
    sink.flush();
  }
}

/** Returns a type format of the good bit size
    @param size a size in bits
    @param is_signed_type non-zero if the type is signed
    @return a C string containing the format for printf functions
*/
char const*
get_type_format( int size, int is_signed_type ) {
  if( is_signed_type ) {
    if( size <= 8 )  return "%\" PRIi8 \"";
    if( size <= 16 ) return "%\" PRIi16 \"";
    if( size <= 32 ) return "%\" PRIi32 \"";
    if( size <= 64 ) return "%\" PRIi64 \"";
  }
  else {
    if( size <= 8 )  return "%\" PRIu8 \"";
    if( size <= 16 ) return "%\" PRIu16 \"";
    if( size <= 32 ) return "%\" PRIu32 \"";
    if( size <= 64 ) return "%\" PRIu64 \"";
  }
  return 0;
}

void
Generator::decoder_decl( Product_t& _product ) const {
  _product.template_signature( isa().m_tparams );
  _product.code( "class Operation;\n" );
  
  if( not isa().m_is_subdecoder ) {
    _product.code( "const unsigned int NUM_OPERATIONS_PER_PAGE = 4096;\n" );
    _product.template_signature( isa().m_tparams );
    _product.code( "class DecodeMapPage\n" );
    _product.code( "{\n" );
    _product.code( "public:\n" );
    _product.code( " DecodeMapPage(%s key);\n", isa().m_addrtype.str() );
    _product.code( " ~DecodeMapPage();\n" );
    _product.code( " %s key;\n", isa().m_addrtype.str() );
    _product.code( " DecodeMapPage *next;\n" );
    _product.code( " Operation" );
    _product.template_abbrev( isa().m_tparams );
    _product.code( " *operation[NUM_OPERATIONS_PER_PAGE];\n" );
    _product.code( "};\n\n" );
  }

  _product.template_signature( isa().m_tparams );
  _product.code( "class DecodeTableEntry\n" );
  _product.code( "{\n" );
  _product.code( "public:\n" );
  _product.code( " DecodeTableEntry(%s opcode, %s opcode_mask, Operation", codetype_constref().str(), codetype_constref().str() );
  _product.template_abbrev( isa().m_tparams );
  _product.code( " *(*decode)(%s, %s));\n", codetype_constref().str(), isa().m_addrtype.str() );
  _product.code( " %s opcode;\n", codetype_name().str() );
  _product.code( " %s opcode_mask;\n", codetype_name().str() );
  _product.code( " Operation" );
  _product.template_abbrev( isa().m_tparams );
  _product.code( " *(*decode)(%s code, %s addr);\n", codetype_constref().str(), isa().m_addrtype.str() );
  _product.code( "};\n" );

  _product.code( "const unsigned int NUM_DECODE_HASH_TABLE_ENTRIES = 4096;\n" );

  _product.template_signature( isa().m_tparams );
  _product.code( "class Decoder\n" );
  _product.code( "{\n" );
  _product.code( "public:\n" );
  _product.code( " Decoder();\n" );
  _product.code( " virtual ~Decoder();\n" );
  _product.code( "\n" );
  if( not isa().m_is_subdecoder ) {
    _product.code( " Operation" );
    _product.template_abbrev( isa().m_tparams );
    _product.code( "*NCDecode(%s addr);\n", isa().m_addrtype.str() );
  }
  _product.code( " Operation" );
  _product.template_abbrev( isa().m_tparams );
  _product.code( " *NCDecode(%s addr, %s code);\n", isa().m_addrtype.str(), codetype_constref().str() );
  if( not isa().m_is_subdecoder ) {
    _product.code( " Operation" );
    _product.template_abbrev( isa().m_tparams );
    _product.code( " *Decode(%s addr, %s insn);\n", isa().m_addrtype.str(), codetype_constref().str() );
    _product.code( " std::vector<DecodeTableEntry" );
    _product.template_abbrev( isa().m_tparams );
    _product.code( " > const& GetDecodeTable() const { return decode_table; };\n" );
    _product.code( " void InvalidateDecodingCacheEntry(%s addr);\n", isa().m_addrtype.str() );
    _product.code( " void InvalidateDecodingCache();\n\n" );
  }
  _product.code( " void SetLittleEndian();\n" );
  _product.code( " void SetBigEndian();\n" );
  _product.code( "private:\n" );
  _product.code( " bool is_little_endian;\n" );
  _product.code( " std::vector<DecodeTableEntry" );
  _product.template_abbrev( isa().m_tparams );
  _product.code( " > decode_table;\n" );
  if( not isa().m_is_subdecoder ) {
    _product.code( " DecodeMapPage" );
    _product.template_abbrev( isa().m_tparams );
    _product.code( " *mru_page;\n" );
    _product.code( " DecodeMapPage" );
    _product.template_abbrev( isa().m_tparams );
    _product.code( " *decode_hash_table[NUM_DECODE_HASH_TABLE_ENTRIES];\n" );
    _product.code( " DecodeMapPage" );
    _product.template_abbrev( isa().m_tparams );
    _product.code( " *FindPage(%s page_key)\n", isa().m_addrtype.str() );
    _product.code( "#if defined(__GNUC__) && (__GNUC__ >= 3 && (__GNUC__ != 3 || __GNUC_MINOR__ != 4 || __GNUC_PATCHLEVEL__ != 6))\n" );
    _product.code( " __attribute__((always_inline))\n" );
    _product.code( "#endif\n" );
    _product.code( " ;\n" );
  }
  _product.code( "};\n\n" );
  if( isa().m_is_subdecoder ) {
    if( not isa().m_tparams.empty() ) {
      _product.template_signature( isa().m_tparams );
    } else {
      _product.code( "inline\n" );
    }
    _product.code( "Operation *sub_decode(%s addr, %s code) {\n", isa().m_addrtype.str(), codetype_constref().str() );
    _product.code( " static Decoder" );
    _product.template_abbrev( isa().m_tparams );
    _product.code(" decoder;\n" );
    _product.code( " return decoder.NCDecode( addr, code );\n" );
    _product.code( "}\n" );
  }
}

void
Generator::operation_decl( Product_t& _product ) const {
  _product.template_signature( isa().m_tparams );
  _product.code( "class Operation\n" );
  if( isa().m_inheritances.size() > 0 ) {
    char const* sep = ": ";
    for( Vect_t<Inheritance_t>::const_iterator inh = isa().m_inheritances.begin(); inh != isa().m_inheritances.end(); ++inh ) {
      _product.code( sep ).usercode( *(**inh).m_modifier ).code( " " ).usercode( *(**inh).m_typename ).code( "\n" );
      sep = ", ";
    }
  }
  _product.code( "{\n" );
  _product.code( "public:\n" );

  _product.code( " Operation(%s code, %s addr, const char *name);\n", codetype_constref().str(), isa().m_addrtype.str() );
  _product.code( " virtual ~Operation();\n" );
  _product.code( " virtual void Encode(%s code) const {}\n", codetype_ref().str() );
  _product.code( " inline %s GetAddr() const { return addr; }\n", isa().m_addrtype.str() );
  _product.code( " inline void SetAddr(%s _addr) { this->addr = _addr; }\n", isa().m_addrtype.str() );
  _product.code( " inline %s GetEncoding() const { return encoding; }\n", codetype_constref().str() );
  op_getlen_decl( _product );
  _product.code( " inline const char *GetName() const { return name; }\n" );
  
  _product.code( " static unsigned int const minsize = %d;\n", m_insn_minsize );
  _product.code( " static unsigned int const maxsize = %d;\n", m_insn_maxsize );
  
  for( Vect_t<Variable_t>::const_iterator var = isa().m_vars.begin(); var < isa().m_vars.end(); ++ var ) {
    _product.usercode( (**var).m_ctype->m_fileloc, " %s %s;", (**var).m_ctype->m_content.str(), (**var).m_symbol.str() );
  }
  
  for( intptr_t idx = isa().m_actionprotos.size(); (--idx) >= 0; ) {
    if( isa().m_actionprotos[idx]->m_type != ActionProto_t::Static or not isa().m_actionprotos[idx]->m_returns ) continue;
    _product.code( " %s %s_result;\n", isa().m_actionprotos[idx]->m_returns->m_content.str(), isa().m_actionprotos[idx]->m_symbol.str() );
  }
  
  if (isa().m_withsource) {
    /* base declaration for internal encoding and decoding code
     * introspection methods */
    _product.code( "virtual char const* Encode_text() const;\n" );
    _product.code( "virtual char const* Decode_text() const;\n" );
  }
  
  for( intptr_t idx = isa().m_actionprotos.size(); (--idx) >= 0; ) {
    if( isa().m_withsource ) {
      /* base declaration for user-defined code introspection
       * methods */
      _product.code( " virtual char const* %s_text() const;\n", isa().m_actionprotos[idx]->m_symbol.str() );
    }
    _product.code( " virtual " );
    if( isa().m_actionprotos[idx]->m_returns ) {
      _product.usercode( *(isa().m_actionprotos[idx]->m_returns) );
    } else {
      _product.code( "void" );
    }

    _product.code( " %s(", isa().m_actionprotos[idx]->m_symbol.str() );

    if( not isa().m_actionprotos[idx]->m_params.empty() ) {
      char const* sep = " ";
      for( Vect_t<CodePair_t>::const_iterator param = isa().m_actionprotos[idx]->m_params.begin();
           param < isa().m_actionprotos[idx]->m_params.end(); ++ param, sep = ",\n" )
        {
          _product.code( sep ).usercode( *(**param).m_ctype ).code( " " ).usercode( *(**param).m_csymbol );
        }
    }

    _product.code( " )%s;\n", (isa().m_actionprotos[idx]->m_constness ? " const" : "") );
  }

  _product.code( "protected:\n" );

  _product.code( " %s encoding;\n", codetype_name().str() );
  _product.code( " %s addr;\n", isa().m_addrtype.str() );
  _product.code( " const char *name;\n" );

  _product.code( "};\n\n" );
}


void
Generator::isa_operations_decl( Product_t& _product ) const {
  for( Vect_t<Operation_t>::const_iterator op = isa().m_operations.begin(); op < isa().m_operations.end(); ++ op ) {
    _product.template_signature( isa().m_tparams );
    _product.code( "class Op%s : public Operation", Str::capitalize( (**op).m_symbol ).str() );
    _product.template_abbrev( isa().m_tparams );
    _product.code( "\n" );

    _product.code( "{\n" );
    _product.code( "public:\n" );
    _product.code( " Op%s(%s code, %s addr);\n", Str::capitalize( (**op).m_symbol ).str(),
                   codetype_constref().str(), isa().m_addrtype.str() );
    insn_destructor_decl( _product, **op );
    insn_getlen_decl( _product, **op );
    _product.code( " void Encode(%s code) const;\n", codetype_ref().str() );
    
    for( Vect_t<BitField_t>::const_iterator bf = (**op).m_bitfields.begin(); bf < (**op).m_bitfields.end(); ++ bf ) {
      BitField_t::Type_t bftype = (**bf).type();
      if( bftype == BitField_t::Operand ) {
        OperandBitField_t const& opbf = dynamic_cast<OperandBitField_t const&>( **bf );
        int dstsize = std::max( m_minwordsize, least_ctype_size( opbf.dstsize() ) );
        _product.code( "%sint%d_t %s;\n", (opbf.m_sext ? "" : "u"), dstsize, opbf.m_symbol.str() );
      } else if( bftype == BitField_t::SpecializedOperand ) {
        SpOperandBitField_t const& sopbf = dynamic_cast<SpOperandBitField_t const&>( **bf );
        ConstStr_t constval = sopbf.constval();
        int dstsize = std::max( m_minwordsize, least_ctype_size( sopbf.dstsize() ) );
        _product.code( "static %sint%d_t const %s = %s;\n",
                       (sopbf.m_sext ? "" : "u"), dstsize, sopbf.m_symbol.str(), sopbf.constval().str() );
      } else if( bftype == BitField_t::SubOp ) {
        SubOpBitField_t const& sobf = dynamic_cast<SubOpBitField_t const&>( **bf );
        SDInstance_t const* sdinstance = sobf.m_sdinstance;
        SDClass_t const* sdclass = sdinstance->m_sdclass;
        SourceCode_t const* tpscheme =  sdinstance->m_template_scheme;
        
        _product.usercode( sdclass->m_fileloc, " %s::Operation", sdclass->qd_namespace().str() );
        if( tpscheme )
          _product.usercode( *tpscheme, "< %s >" );
        _product.code( "* %s;\n", sobf.m_symbol.str() );
      }
    }

    if( not (**op).m_variables.empty() ) {
      for( Vect_t<Variable_t>::const_iterator var = (**op).m_variables.begin(); var < (**op).m_variables.end(); ++ var ) {
        _product.usercode( (**var).m_ctype->m_fileloc, "   %s %s;", (**var).m_ctype->m_content.str(), (**var).m_symbol.str() );
      }
    }
    
    if (isa().m_withsource) {
      /* insn declaration for internal encoding and decoding code
       * introspection methods */
      _product.code( "char const* Encode_text() const;\n" );
      _product.code( "char const* Decode_text() const;\n" );
    }


    for( Vect_t<Action_t>::const_iterator action = (**op).m_actions.begin(); action < (**op).m_actions.end(); ++ action ) {
      ActionProto_t const* actionproto = (**action).m_actionproto;
      
      if( not actionproto->m_comments.empty() ) {
        for( Vect_t<Comment_t>::const_iterator comm = actionproto->m_comments.begin(); comm < actionproto->m_comments.end(); ++ comm )
          _product.code( " %s\n", (**comm).m_content.str() );
      }
      
      if( isa().m_withsource ) {
        // for cstring version of the action
        _product.code( " virtual char const* %s_text() const;\n", actionproto->m_symbol.str() );
      }

      _product.code( " virtual\n " );

      if( actionproto->m_returns ) {
        _product.usercode( *actionproto->m_returns );
      } else {
        _product.code( "void\n" );
      }

      _product.code( " %s(", actionproto->m_symbol.str() );

      if( not actionproto->m_params.empty() ) {
        char const* sep = " ";
        for( Vect_t<CodePair_t>::const_iterator param = actionproto->m_params.begin();
             param < actionproto->m_params.end(); ++ param, sep = ",\n" )
          {
            _product.code( sep ).usercode( *(**param).m_ctype ).code( " " ).usercode( *(**param).m_csymbol );
          }
        _product.code( " " );
      }

      _product.code( ")%s;\n", (actionproto->m_constness ? " const" : "") );
    }

    _product.code( "private:\n" );

    _product.code( "};\n\n" );
  }
}

void
Generator::operation_impl( Product_t& _product ) const {
  _product.template_signature( isa().m_tparams );
  _product.code( "Operation" );
  _product.template_abbrev( isa().m_tparams );
  _product.code( "::Operation(%s _code, %s _addr, const char *_name)\n", codetype_constref().str(), isa().m_addrtype.str() );
  _product.code( ": \n");
  
  for( Vect_t<Inheritance_t>::const_iterator inh = isa().m_inheritances.begin(); inh != isa().m_inheritances.end(); ++ inh ) {
    if( not (**inh).m_initargs ) continue;
    _product.usercode( *(**inh).m_typename ).code( "( " ).usercode( *(**inh).m_initargs ).code( " ),\n" );
  }

  for( Vect_t<Variable_t>::const_iterator var = isa().m_vars.begin(); var < isa().m_vars.end(); ++ var ) {
    if( not (**var).m_cinit ) continue;
    _product.code( " %s(", (**var).m_symbol.str() ).usercode( *(**var).m_cinit ).code( "),\n" );
  }

  _product.code( " encoding(_code),\n" );
  _product.code( " addr(_addr),\n" );
  _product.code( " name(_name)\n" );
  _product.code( "{\n" );
  _product.code( "}\n\n" );
  _product.template_signature( isa().m_tparams );
  _product.code( "Operation" );
  _product.template_abbrev( isa().m_tparams );
  _product.code( "::~Operation()\n" );
  _product.code( "{\n" );
  
  for( intptr_t idx = isa().m_actionprotos.size(); (--idx) >= 0; ) {
    if( isa().m_actionprotos[idx]->m_type != ActionProto_t::Destructor ) continue;
    _product.code( " %s();\n", isa().m_actionprotos[idx]->m_symbol.str() );
  }
  _product.code( "}\n\n" );

  if (isa().m_withsource) {
    /* base implemantation for internal encoding and decoding code
     * introspection methods */
    char const* xxcode[2] = {"Encode", "Decode"};
    for (int step = 0; step < 2; ++step) {
      _product.template_signature( isa().m_tparams );
      _product.code( " char const* Operation" );
      _product.template_abbrev( isa().m_tparams );
      _product.code( "::%s_text() const { return\"\"; }\n", xxcode[step] );
    }
  }
  
  for( intptr_t idx = isa().m_actionprotos.size(); (--idx) >= 0; ) {
    if( isa().m_withsource ) {
      // for cstring version of the method
      _product.template_signature( isa().m_tparams );
      _product.code( " char const* Operation" );
      _product.template_abbrev( isa().m_tparams );
      _product.code( "::%s_text() const\n", isa().m_actionprotos[idx]->m_symbol.str() );
      _product.code( " { return %s; }", Str::dqcstring( isa().m_actionprotos[idx]->m_defaultcode->m_content ).str() );
    }
    
    _product.template_signature( isa().m_tparams );

    if(isa().m_actionprotos[idx]->m_returns) {
      _product.usercode( *(isa().m_actionprotos[idx]->m_returns) );
    }
    else {
      _product.code( "\nvoid\n" );
    }

    _product.code( " Operation" );
    _product.template_abbrev( isa().m_tparams );
    _product.code( "::%s(", isa().m_actionprotos[idx]->m_symbol.str() );

    if( not isa().m_actionprotos[idx]->m_params.empty() ) {
      char const* sep = " ";
      for( Vect_t<CodePair_t>::const_iterator param = isa().m_actionprotos[idx]->m_params.begin();
           param < isa().m_actionprotos[idx]->m_params.end(); ++ param, sep = ",\n" )
        {
          _product.code( sep ).usercode( *(**param).m_ctype ).code( " " ).usercode( *(**param).m_csymbol );
        }
    }

    _product.code( ")%s\n{\n", (isa().m_actionprotos[idx]->m_constness ? " const" : "") );
    _product.usercode( *isa().m_actionprotos[idx]->m_defaultcode, "{%s}\n" );
    _product.code( "}\n" );
  }
}

void
Generator::isa_operations_methods( Product_t& _product ) const {
  for( Vect_t<Operation_t>::const_iterator op = isa().m_operations.begin(); op < isa().m_operations.end(); ++ op ) {
    if( not (**op).m_comments.empty() ) {
      for( Vect_t<Comment_t>::const_iterator comm = (**op).m_comments.begin(); comm < (**op).m_comments.end(); ++ comm )
        _product.code( "%s\n", (**comm).m_content.str() );
    }
    
    if( isa().m_withsource ) {
      /* insn implemantation for internal encoding and decoding code
       * introspection methods */
      char const* xxcode[2] = {"Encode", "Decode"};
      for (int step = 0; step < 2; ++step) {
        SProduct_t xxcode_text( _product.m_filename, _product.m_sourcelines );
        switch (step) {
        case 0: insn_decode_impl( xxcode_text, **op, "code", "addr" ); break;
        case 1: insn_encode_impl( xxcode_text, **op, "code" ); break;
        }
        _product.template_signature( isa().m_tparams );
        _product.code( " char const* Op%s", Str::capitalize( (**op).m_symbol ).str() );
        _product.template_abbrev( isa().m_tparams );
        _product.code( "::%s_text() const\n", xxcode[step] );
        _product.code( " { return %s; }\n", Str::dqcstring( xxcode_text.m_content.m_storage ).str() );
      }
    }
    
    for( Vect_t<Action_t>::const_iterator action = (**op).m_actions.begin(); action < (**op).m_actions.end(); ++ action ) {
      ActionProto_t const* actionproto = (**action).m_actionproto;

      if( not (**action).m_comments.empty() ) {
        for( Vect_t<Comment_t>::const_iterator comm = (**action).m_comments.begin(); comm < (**action).m_comments.end(); ++ comm )
          _product.code( "%s\n", (**comm).m_content.str() );
      }

      if( isa().m_withsource ) {
        // for cstring version of the method
        _product.template_signature( isa().m_tparams );
        _product.code( " char const* Op%s", Str::capitalize( (**op).m_symbol ).str() );
        _product.template_abbrev( isa().m_tparams );
        _product.code( "::%s_text() const\n", actionproto->m_symbol.str() );
        _product.code( " { return %s; }", Str::dqcstring( (**action).m_source_code->m_content.str() ).str() );
      }

      _product.template_signature( isa().m_tparams );

      if( actionproto->m_returns ) {
        _product.usercode( *actionproto->m_returns );
      }
      else {
        _product.code( "\nvoid\n" );
      }

      _product.code( " Op%s", Str::capitalize( (**op).m_symbol ).str() );
      _product.template_abbrev( isa().m_tparams );
      _product.code( "::%s(", actionproto->m_symbol.str() );

      if( not actionproto->m_params.empty() ) {
        char const* sep = " ";
        for( Vect_t<CodePair_t>::const_iterator param = actionproto->m_params.begin();
             param < actionproto->m_params.end(); ++ param, sep = ",\n" )
          {
            _product.code( sep ).usercode( *(**param).m_ctype ).code( " " ).usercode( *(**param).m_csymbol );
          }
      }

      _product.code( ")%s\n{\n", (actionproto->m_constness ? " const" : "") );
      _product.usercode( *(**action).m_source_code, "{%s}\n" );
      _product.code( "}\n" );
    }

    _product.code( "\n" );

    _product.template_signature( isa().m_tparams );
    _product.code( "static Operation" );
    _product.template_abbrev( isa().m_tparams );
    _product.code( " *DecodeOp%s(", Str::capitalize( (**op).m_symbol ).str() );
    _product.code( "%s code, %s addr)\n", codetype_constref().str(), isa().m_addrtype.str() );
    _product.code( "{\n" );
    _product.code( " return new Op%s", Str::capitalize( (**op).m_symbol ).str() );
    _product.template_abbrev( isa().m_tparams );
    _product.code( "(code, addr);\n" );
    _product.code( "}\n\n" );
  }
}

void
Generator::isa_operations_ctors( Product_t& _product ) const {
  for( Vect_t<Operation_t>::const_iterator op = isa().m_operations.begin(); op < isa().m_operations.end(); ++ op ) {
    _product.template_signature( isa().m_tparams );
    _product.code( "Op%s", Str::capitalize( (**op).m_symbol ).str() );
    _product.template_abbrev( isa().m_tparams );
    _product.code( "::Op%s(%s code, %s addr) : Operation", Str::capitalize( (**op).m_symbol ).str(),
                   codetype_constref().str(), isa().m_addrtype.str() );
    _product.template_abbrev( isa().m_tparams );
    _product.code( "(code, addr, \"%s\")\n",
                   (**op).m_symbol.str() );

    _product.code( "{\n" );
    
    insn_decode_impl( _product, **op, "code", "addr" );

    if( not (**op).m_variables.empty() ) {
      for( Vect_t<Variable_t>::const_iterator var = (**op).m_variables.begin(); var < (**op).m_variables.end(); ++ var ) {
        if( (**var).m_cinit ) {
          _product.code( "%s = ", (**var).m_symbol.str() ).usercode( *(**var).m_cinit ).code( ";\n" );
        }
      }
    }

    _product.code( "}\n\n" );
    
    insn_destructor_impl( _product, **op );
  }
}

void
Generator::isa_operations_encoders( Product_t& _product ) const {
  for( Vect_t<Operation_t>::const_iterator op = isa().m_operations.begin(); op < isa().m_operations.end(); ++ op ) {
    _product.template_signature( isa().m_tparams );
    _product.code( "void Op%s", Str::capitalize( (**op).m_symbol ).str() );
    _product.template_abbrev( isa().m_tparams );
    _product.code( "::Encode(%s code) const\n", codetype_ref().str() );

    _product.code( "{\n" );
    
    insn_encode_impl( _product, **op, "code" );

    _product.code( "}\n\n" );
  }
}

void
Generator::decoder_impl( Product_t& _product ) const {
  if( not isa().m_is_subdecoder ) {
    _product.template_signature( isa().m_tparams );
    _product.code( "DecodeMapPage" );
    _product.template_abbrev( isa().m_tparams );
    _product.code( "::DecodeMapPage(%s key)\n", isa().m_addrtype.str() );
    _product.code( "{\n" );
    _product.code( " this->key = key;\n" );
    _product.code( " memset(operation, 0, sizeof(operation));\n" );
    _product.code( " next = 0;\n" );
    _product.code( "}\n\n" );
    
    _product.template_signature( isa().m_tparams );
    _product.code( "DecodeMapPage" );
    _product.template_abbrev( isa().m_tparams );
    _product.code( "::~DecodeMapPage()\n" );
    _product.code( "{\n" );
    _product.code( " unsigned int idx;\n" );
    _product.code( " for(idx = 0; idx < NUM_OPERATIONS_PER_PAGE; idx++)\n" );
    _product.code( "  delete operation[idx];\n" );
    _product.code( "}\n\n" );
  }
  _product.template_signature( isa().m_tparams );
  _product.code( " DecodeTableEntry" );
  _product.template_abbrev( isa().m_tparams );
  _product.code( "::DecodeTableEntry(%s opcode, %s opcode_mask, Operation", codetype_constref().str(), codetype_constref().str() );
  _product.template_abbrev( isa().m_tparams );
  _product.code( " *(*decode)(%s, %s))\n", codetype_constref().str(), isa().m_addrtype.str() );
  _product.code( " {\n" );
  _product.code( "  this->opcode = opcode;\n" );
  _product.code( "  this->opcode_mask = opcode_mask;\n" );
  _product.code( "  this->decode = decode;\n" );
  _product.code( " }\n\n" );
  
  
  _product.template_signature( isa().m_tparams );
  _product.code( "Decoder" );
  _product.template_abbrev( isa().m_tparams );
  _product.code( "::Decoder()\n" );
  _product.code( ": is_little_endian( %s )", isa().m_little_endian ? "true" : "false" );
  if( not isa().m_is_subdecoder)
    _product.code( ", mru_page( 0 )" );
  _product.code( "\n{\n" );
  if( not isa().m_is_subdecoder )
    _product.code( " memset(decode_hash_table, 0, sizeof(decode_hash_table));\n" );
  
  for( Vect_t<Operation_t>::const_reverse_iterator op = isa().m_operations.rbegin(); op < isa().m_operations.rend(); ++ op ) {
    if( (**op).m_condition ) {
      _product.code( "if(" ).usercode( *(**op).m_condition ).code( ")" );
    }
    _product.code( " decode_table.push_back(DecodeTableEntry" );
    _product.template_abbrev( isa().m_tparams );
    _product.code( "(" );
    insn_bits_code( _product, **op );
    _product.code( ", " );
    insn_mask_code( _product, **op );
    _product.code( ", DecodeOp%s", Str::capitalize( (**op).m_symbol ).str() );
    _product.template_abbrev( isa().m_tparams );
    _product.code( "));\n" );
  }
  _product.code( "}\n\n" );

  _product.template_signature( isa().m_tparams );
  _product.code( "Decoder" );
  _product.template_abbrev( isa().m_tparams );
  _product.code( "::~Decoder()\n" );
  _product.code( "{\n" );
  if( not isa().m_is_subdecoder ) {
    _product.code( " InvalidateDecodingCache();\n" );
  }
  _product.code( "}\n\n" );
  
  /*** NCDecode( Address_t addr, CodeType_t code ) ***/
  _product.template_signature( isa().m_tparams );
  _product.code( "Operation" );
  _product.template_abbrev( isa().m_tparams );
  _product.code( " *Decoder" );
  _product.template_abbrev( isa().m_tparams );
  _product.code( "::NCDecode(%s addr, %s code)\n", isa().m_addrtype.str(), codetype_constref().str() );
  _product.code( "{\n" );
  _product.code( " Operation" );
  _product.template_abbrev( isa().m_tparams );
  _product.code( " *operation;\n" );
  _product.code( " unsigned int count = decode_table.size();\n" );
  _product.code( " unsigned int idx;\n" );
  _product.code( " for(idx = 0; idx < count; idx++)\n" );
  _product.code( " {\n" );
  insn_match_ifexpr( _product, "code", "decode_table[idx].opcode_mask", "decode_table[idx].opcode" );
  _product.code( "  {\n" );
  _product.code( "   operation = decode_table[idx].decode(code, addr);\n" );

  for( intptr_t idx = isa().m_actionprotos.size();  (--idx) >= 0; ) {
    switch( isa().m_actionprotos[idx]->m_type ) {
    case ActionProto_t::Constructor:
      _product.code( "   operation->%s();\n", isa().m_actionprotos[idx]->m_symbol.str() );
      break;
        
    case ActionProto_t::Static:
      _product.code( "   " );
      if( isa().m_actionprotos[idx]->m_returns )
        _product.code( "operation->%s_result = ", isa().m_actionprotos[idx]->m_symbol.str() );
      _product.code( "operation->%s();\n", isa().m_actionprotos[idx]->m_symbol.str() );
      break;
        
    default: break;
    }
  }
    
  _product.code( "   return operation;\n" );
  _product.code( "  }\n" );
  _product.code( " }\n" );
  
  _product.code( " operation = new Operation" );
  _product.template_abbrev( isa().m_tparams );
  _product.code( "(code, addr, \"???\");\n" );
    
    
  for( intptr_t idx = isa().m_actionprotos.size();  (--idx) >= 0; ) {
    switch( isa().m_actionprotos[idx]->m_type ) {
    case ActionProto_t::Constructor:
      _product.code( " operation->%s();\n", isa().m_actionprotos[idx]->m_symbol.str() );
      break;
    case ActionProto_t::Static:
      _product.code( " " );
      if( isa().m_actionprotos[idx]->m_returns )
        _product.code( "operation->%s_result = ", isa().m_actionprotos[idx]->m_symbol.str() );
      _product.code( "operation->%s();\n", isa().m_actionprotos[idx]->m_symbol.str() );
      break;
    default: break;
    }
  }
    
  _product.code( " return operation;\n" );
  _product.code( "}\n\n" );
  
  if( not isa().m_is_subdecoder ) {
    /*** InvalidateDecodingCache() ***/
    _product.template_signature( isa().m_tparams );
    _product.code( "void Decoder" );
    _product.template_abbrev( isa().m_tparams );
    _product.code( "::InvalidateDecodingCache()\n" );
    _product.code( "{\n" );
    _product.code( " uint32_t index;\n" );
    _product.code( " mru_page = 0;\n" );
    _product.code( " for(index = 0; index < NUM_DECODE_HASH_TABLE_ENTRIES; index++)\n" );
    _product.code( " {\n" );
    _product.code( "  DecodeMapPage" );
    _product.template_abbrev( isa().m_tparams );
    _product.code( " *page, *next_page;\n" );

    _product.code( "  page = decode_hash_table[index];\n" );
    _product.code( "  if(page)\n" );
    _product.code( "  {\n" );
    _product.code( "   do\n" );
    _product.code( "   {\n" );
    _product.code( "   next_page = page->next;\n" );
    _product.code( "   delete page;\n" );
    _product.code( "   page = next_page;\n" );
    _product.code( "   } while(page);\n" );
    _product.code( "  decode_hash_table[index] = 0;\n" );
    _product.code( "  }\n" );
    _product.code( " }\n" );
    _product.code( "}\n\n" );
  
    ConstStr_t instid = insn_id_expr( "addr" );

    /*** InvalidateDecodingCacheEntry( Address_t addr ) ***/
    _product.template_signature( isa().m_tparams );
    _product.code( "void Decoder" );
    _product.template_abbrev( isa().m_tparams );
    _product.code( "::InvalidateDecodingCacheEntry(%s addr)\n", isa().m_addrtype.str() );
    _product.code( "{\n" );
    _product.code( " %s page_key = %s / NUM_OPERATIONS_PER_PAGE;\n", isa().m_addrtype.str(), instid.str() );
    _product.code( " if(mru_page && mru_page->key == page_key) mru_page = 0;\n" );
    _product.code( " uint32_t index = page_key %% NUM_DECODE_HASH_TABLE_ENTRIES; // hash the key\n" );
    _product.code( " DecodeMapPage" );
    _product.template_abbrev( isa().m_tparams );
    _product.code( " *prev, *cur;\n" );
    _product.code( " cur = decode_hash_table[index];\n" );
    _product.code( " if(cur)\n" );
    _product.code( " {\n" );
    _product.code( "  if(cur->key == page_key)\n" );
    _product.code( "  {\n" );
    _product.code( "   decode_hash_table[index] = cur->next;\n" );
    _product.code( "   delete cur;\n" );
    _product.code( "   return;\n" );
    _product.code( "  }\n" );
    _product.code( "  prev = cur;\n" );
    _product.code( "  cur = cur->next;\n" );
    _product.code( "  if(cur)\n" );
    _product.code( "  {\n" );
    _product.code( "   do\n" );
    _product.code( "   {\n" );
    _product.code( "    if(cur->key == page_key)\n" );
    _product.code( "    {\n" );
    _product.code( "     prev->next = cur->next;\n" );
    _product.code( "     cur->next = 0;\n" );
    _product.code( "     delete cur;\n" );
    _product.code( "     return;\n" );
    _product.code( "    }\n" );
    _product.code( "    prev = cur;\n" );
    _product.code( "   } while((cur = cur->next) != 0);\n" );
    _product.code( "  }\n" );
    _product.code( " }\n" );
    _product.code( "}\n\n" );

    /*** FindPage( Address_t page_key ) ***/
    _product.template_signature( isa().m_tparams );
    _product.code( "inline DecodeMapPage" );
    _product.template_abbrev( isa().m_tparams );
    _product.code( " *Decoder" );
    _product.template_abbrev( isa().m_tparams );
    _product.code( "::FindPage(%s page_key)\n", isa().m_addrtype.str() );
    _product.code( "{\n" );
    _product.code( " if(mru_page && mru_page->key == page_key) return mru_page;\n" );
    _product.code( " uint32_t index = page_key %% NUM_DECODE_HASH_TABLE_ENTRIES; // hash the key\n" );
    _product.code( " DecodeMapPage" );
    _product.template_abbrev( isa().m_tparams );
    _product.code( " *prev, *cur;\n" );
    _product.code( " cur = decode_hash_table[index];\n" );
    _product.code( " if(cur)\n" );
    _product.code( " {\n" );
    _product.code( "  if(cur->key == page_key)\n" );
    _product.code( "  {\n" );
    _product.code( "   mru_page = cur;\n" );
    _product.code( "   return cur;\n" );
    _product.code( "  }\n" );
    _product.code( "  prev = cur;\n" );
    _product.code( "  cur = cur->next;\n" );
    _product.code( "  if(cur)\n" );
    _product.code( "  {\n" );
    _product.code( "   do\n" );
    _product.code( "   {\n" );
    _product.code( "    if(cur->key == page_key)\n" );
    _product.code( "    {\n" );
    _product.code( "     prev->next = cur->next;\n" );
    _product.code( "     cur->next= decode_hash_table[index];\n" );
    _product.code( "     decode_hash_table[index] = cur;\n" );
    _product.code( "     mru_page = cur;\n" );
    _product.code( "     return cur;\n" );
    _product.code( "    }\n" );
    _product.code( "    prev = cur;\n" );
    _product.code( "   } while((cur = cur->next) != 0);\n" );
    _product.code( "  }\n" );
    _product.code( " }\n" );
    _product.code( " return 0;\n" );
    _product.code( "}\n\n" );

    /*** Decode( Address_t addr, CodeType_t const& insn ) ***/
    _product.template_signature( isa().m_tparams );
    _product.code( "Operation" );
    _product.template_abbrev( isa().m_tparams );
    _product.code( " *Decoder" );
    _product.template_abbrev( isa().m_tparams );
    _product.code( "::Decode(%s addr, %s insn)\n", isa().m_addrtype.str(), codetype_constref().str() );
    _product.code( "{\n" );
    _product.code( " Operation" );
    _product.template_abbrev( isa().m_tparams );
    _product.code( " *operation;\n" );
    _product.code( " %s page_key = %s / NUM_OPERATIONS_PER_PAGE;\n", isa().m_addrtype.str(), instid.str() );
    _product.code( " DecodeMapPage" );
    _product.template_abbrev( isa().m_tparams );
    _product.code( " *page;\n" );
    _product.code( " page = FindPage(page_key);\n" );
    _product.code( " if(!page)\n" );
    _product.code( " {\n" );
    // _product.code( "   fprintf(stderr, \"page miss at 0x%%08x\\n\", addr);\n" );
    _product.code( "  page = new DecodeMapPage" );
    _product.template_abbrev( isa().m_tparams );
    _product.code( " (page_key);\n" );
    _product.code( "  uint32_t index = page_key %% NUM_DECODE_HASH_TABLE_ENTRIES; // hash the key\n" );
    _product.code( "  page->next = decode_hash_table[index];\n" );
    _product.code( "  decode_hash_table[index] = page;\n" );
    _product.code( "  mru_page = page;\n" );
    _product.code( " }\n" );
    _product.code( " operation = page->operation[(%s) & (NUM_OPERATIONS_PER_PAGE - 1)];\n", instid.str() );
    _product.code( " if(operation)\n" );
    _product.code( " {\n" );
    // _product.code( "  fprintf(stderr, \"hit at 0x%%08x\\n\", addr);\n" );
    _product.code( "  if(");
    insn_unchanged_expr( _product, "operation->GetEncoding()", "insn" );
    _product.code( " && operation->GetAddr() == addr)\n" );
    _product.code( "   return operation;\n" );
    _product.code( "  delete operation;\n" );
    _product.code( " }\n" );
    // _product.code( " fprintf(stderr, \"miss at 0x%%08x\\n\", addr);\n" );
    _product.code( " operation = NCDecode(addr, insn);\n" );
    _product.code( " page->operation[(%s) & (NUM_OPERATIONS_PER_PAGE - 1)] = operation;\n", instid.str() );
    _product.code( " return operation;\n" );
    _product.code( "}\n\n" );
  }
  
  /*** SetLittleEndian() ***/
  _product.template_signature( isa().m_tparams );
  _product.code( "void Decoder" );
  _product.template_abbrev( isa().m_tparams );
  _product.code( "::SetLittleEndian()\n" );
  _product.code( "{\n" );
  _product.code( " is_little_endian = true;\n" );
  _product.code( "}\n\n" );

  /*** SetBigEndian() ***/
  _product.template_signature( isa().m_tparams );
  _product.code( "void Decoder" );
  _product.template_abbrev( isa().m_tparams );
  _product.code( "::SetBigEndian()\n" );
  _product.code( "{\n" );
  _product.code( " is_little_endian = false;\n" );
  _product.code( "}\n\n" );
}

unsigned int
Generator::least_ctype_size( unsigned int bits ) {
  if (bits > 64) throw GenerationError;
  unsigned int size = 8;
  while (size < bits) size *= 2;
  return size;
}

FieldIterator::FieldIterator( bool little_endian, Vect_t<BitField_t> const& bitfields, unsigned int maxsize )
  : m_bitfields( bitfields ), m_idx( (unsigned int)(-1) ),
    m_ref( little_endian ? 0 : maxsize ),
    m_pos( m_ref ), m_size( 0 ),
    m_chkpt_pos( m_pos ), m_chkpt_size( m_size )
{}
  
BitField_t const& FieldIterator::item() { return *(m_bitfields[m_idx]); }

bool
FieldIterator::next() {
  if ((m_idx+1) >= m_bitfields.size()) return false;
  m_idx += 1;
  BitField_t const& field = *(m_bitfields[m_idx]);
  if (m_ref == 0) m_pos += m_size;
  else            m_pos -= field.m_size;
  m_size = field.m_size;
  if (field.type() == BitField_t::Separator) {
    SeparatorBitField_t const& sepbf = dynamic_cast<SeparatorBitField_t const&>( field );
    if (sepbf.m_rewind) { m_pos = m_chkpt_pos; m_size = m_chkpt_size; }
    else                { m_chkpt_pos = m_pos; m_chkpt_size = m_size; }
  }
  
  return true;
}
