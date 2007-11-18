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
#include <iostream>
#include <cassert>

using namespace std;

RiscGenerator::RiscGenerator( Isa const* _isa )
  : Generator( _isa ), m_codebitsize( 0 ), m_codesize( 0 )
{}

RiscGenerator::~RiscGenerator()
{}

/** Process the isa structure and computes RISC specific data 
    @return true upon success
*/

bool
RiscGenerator::finalize() {
  m_codebitsize = 0;
  // Process the opcodes needed by the decoder
  for( Vect_t<Operation_t>::const_iterator op = isa().m_operations.begin(); op < isa().m_operations.end(); ++ op ) {
    unsigned int size = 0;
    for( Vect_t<BitField_t>::const_iterator bf = (**op).m_bitfields.begin(); bf < (**op).m_bitfields.end(); ++ bf )
      size += (**bf).m_size;
    if( size > 64 ) {
      yyerrorf( (**op).m_filename, (**op).m_lineno, "error: can't process opcode wider than 64 bits (operation `%s`, %u bits).",
                (**op).m_symbol.str(), size );
      return false;
    }
    if( size % 8 ) {
      yyerrorf( (**op).m_filename, (**op).m_lineno, "warning: operation `%s` has un aligned opcode (%d).", size );
    }
    if( size > m_codebitsize ) m_codebitsize = size;
    unsigned int shift = size;
    uint64_t mask = 0, bits = 0;
    for( Vect_t<BitField_t>::const_iterator bf = (**op).m_bitfields.begin(); bf < (**op).m_bitfields.end(); ++ bf ) {
      shift -= (**bf).m_size;
      if( not (**bf).hasopcode() ) continue;
      bits |= (**bf).bits() << shift;
      mask |= (**bf).mask() << shift;
    }
    m_opcodes[*op] = OpCode_t( size, mask, bits );
  }

  if( m_codebitsize <= 8 )       { m_codesize = 1; m_codetype = "uint8_t";  m_immedsuffix = "U"; }
  else if( m_codebitsize <= 16 ) { m_codesize = 2; m_codetype = "uint16_t"; m_immedsuffix = "U"; }
  else if( m_codebitsize <= 32 ) { m_codesize = 4; m_codetype = "uint32_t"; m_immedsuffix = "UL"; }
  else if( m_codebitsize <= 64 ) { m_codesize = 8; m_codetype = "uint64_t"; m_immedsuffix = "ULL"; }
  else assert( false );
  
  return true;
}

/** Do some additionnal checks after the parsing
    @return true upon success
*/
bool
RiscGenerator::sanity_checks() const {
  if( not isa().m_addrtype ) {
    // FIXME: Downgraded warning to error (default pointer size for
    // target architecture is nonsense)
    yyerrorf( 0, 0, "error: no architecture address type found." );
    return false;
  }
  
  // Checking operations
  for( Vect_t<Operation_t>::const_iterator op= isa().m_operations.begin(); op < isa().m_operations.end(); ++ op ) {
    /* Search for opcode conflicting operations */
    for( Vect_t<Operation_t>::const_iterator o2 = isa().m_operations.begin(); o2 < op; ++ o2 ) {
      bool op1in2 = ((opcode( *op ).m_bits & opcode( *o2 ).m_mask) == opcode( *o2 ).m_bits);
      bool op2in1 = ((opcode( *o2 ).m_bits & opcode( *op ).m_mask) == opcode( *op ).m_bits);
      // FIXME: We should allow for an operation to be a strict subset
      // of another operation in order to allow special meaning of an
      // instruction with precise operands.
      if( op1in2 or op2in1 ) {
        cerr << "op1mask: " << std::hex << opcode( *op ).m_mask << endl;
        cerr << "op1bits: " << std::hex << opcode( *op ).m_bits << endl;
        cerr << "op2mask: " << std::hex << opcode( *o2 ).m_mask << endl;
        cerr << "op2bits: " << std::hex << opcode( *o2 ).m_bits << endl;
        cerr << "op1in2:  " << std::hex << (opcode( *op ).m_bits & opcode( *o2 ).m_mask) << " == " << (opcode( *o2 ).m_bits) << endl;
        cerr << "op2in1:  " << std::hex << (opcode( *o2 ).m_bits & opcode( *op ).m_mask) << " == " << (opcode( *op ).m_bits) << endl;
        yyerrorf( (**op).m_filename, (**op).m_lineno, "error: operation `%s' conflicts with operation `%s'",
                  (**op).m_symbol.str(), (**o2).m_symbol.str() );
        yyerrorf( (**o2).m_filename, (**o2).m_lineno, "operation `%s' was declared here", (**o2).m_symbol.str() );
        return false;
      }
    }

    if( opcode( *op ).m_size != m_codebitsize )
      yyerrorf( (**op).m_filename, (**op).m_lineno, "warning: operation `%s' is %u-bit long instead of %u-bit",
                (**op).m_symbol.str(), opcode( *op ).m_size, m_codebitsize );
    
    // Looking for bitfield conflicts
    for( Vect_t<BitField_t>::const_iterator bf = (**op).m_bitfields.begin(); bf < (**op).m_bitfields.end(); ++ bf ) {
      ConstStr_t bf_symbol = (**bf).symbol();
      if( not bf_symbol ) continue;
      for( Vect_t<BitField_t>::const_iterator pbf = (**op).m_bitfields.begin(); pbf < bf; ++ pbf ) {
        ConstStr_t pbf_symbol = (**pbf).symbol();
        if( pbf_symbol != bf_symbol ) continue;
        yyerrorf( (**op).m_filename, (**op).m_lineno, "error: duplicated bit field `%s' in operation `%s'",
                  bf_symbol.str(), (**op).m_symbol.str() );
        return false;
      }
    }
    
    if( not (**op).m_variables.empty() ) {
      for( Vect_t<Variable_t>::const_iterator checked = (**op).m_variables.begin(); checked < (**op).m_variables.end(); ++ checked ) {
        Vect_t<Variable_t>::const_iterator found;
        for( found = isa().m_vars.begin(); found < isa().m_vars.end(); ++ found )
          if( (**found).m_symbol == (**checked).m_symbol ) break;
        
        if( (found < isa().m_vars.end()) ) {
          yyerrorf( (**checked).m_ctype->m_filename, (**checked).m_ctype->m_lineno,
                    "error: in operation `%s', variable `%s' is already defined as global",
                    (**op).m_symbol.str(), (**checked).m_symbol.str() );
          yyerrorf( (**found).m_ctype->m_filename, (**found).m_ctype->m_lineno,
                    "variable `%s' previously defined here", (**found).m_symbol.str() );
          return false;
        }
        
        for( found = (**op).m_variables.begin(); found < checked; ++ found )
          if( (**found).m_symbol == (**checked).m_symbol ) break;

        if( found < checked ) {
          yyerrorf( (**checked).m_ctype->m_filename, (**checked).m_ctype->m_lineno,
                    "error: in operation `%s', variable `%s' is defined several times",
                    (**op).m_symbol.str(), (**checked).m_symbol.str() );
          return false;
        }
      }
    }
  }
  return true;
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
RiscGenerator::generate_decoder_class_definition( Product_t& _product ) const {
  _product.template_signature( isa().m_tparams );
  _product.code( "class Operation;\n" );

  _product.code( "const unsigned int NUM_OPERATIONS_PER_PAGE = 4096;\n" );
  _product.template_signature( isa().m_tparams );
  _product.code( "class DecodeMapPage\n" );
  _product.code( "{\n" );
  _product.code( "public:\n" );
  _product.code( "\tDecodeMapPage(%s key);\n", isa().m_addrtype.str() );
  _product.code( "\t~DecodeMapPage();\n" );
  _product.code( "\t%s key;\n", isa().m_addrtype.str() );
  _product.code( "\tDecodeMapPage *next;\n" );
  _product.code( "\tOperation" );
  _product.template_abrev( isa().m_tparams );
  _product.code( " *operation[NUM_OPERATIONS_PER_PAGE];\n" );
  _product.code( "};\n\n" );


  _product.template_signature( isa().m_tparams );
  _product.code( "class DecodeTableEntry\n" );
  _product.code( "{\n" );
  _product.code( "public:\n" );
  _product.code( "\tDecodeTableEntry(CodeType opcode, CodeType opcode_mask, Operation" );
  _product.template_abrev( isa().m_tparams );
  _product.code( " *(*decode)(CodeType, %s));\n", isa().m_addrtype.str() );
  _product.code( "\tCodeType opcode;\n" );
  _product.code( "\tCodeType opcode_mask;\n" );
  _product.code( "\tOperation" );
  _product.template_abrev( isa().m_tparams );
  _product.code( " *(*decode)(CodeType code, %s addr);\n", isa().m_addrtype.str() );
  _product.code( "};\n" );

  _product.code( "const unsigned int NUM_DECODE_HASH_TABLE_ENTRIES = 4096;\n" );

  _product.template_signature( isa().m_tparams );
  _product.code( "class Decoder\n" );
  _product.code( "{\n" );
  _product.code( "public:\n" );
  _product.code( "\tDecoder();\n" );
  _product.code( "\tvirtual ~Decoder();\n" );
  _product.code( "\n" );
  _product.code( "\tOperation" );
  _product.template_abrev( isa().m_tparams );
  _product.code( "*NCDecode(%s addr);\n", isa().m_addrtype.str() );
  _product.code( "\tOperation" );
  _product.template_abrev( isa().m_tparams );
  _product.code( " *NCDecode(%s addr, CodeType code);\n", isa().m_addrtype.str() );
  _product.code( "\tOperation" );
  _product.template_abrev( isa().m_tparams );
  _product.code( " *Decode(%s addr);\n", isa().m_addrtype.str() );
  _product.code( "\tOperation" );
  _product.template_abrev( isa().m_tparams );
  _product.code( " *Decode(%s addr, CodeType insn);\n", isa().m_addrtype.str() );
  _product.code( "\tvirtual void Fetch(void *, %s, uint32_t) ;\n", isa().m_addrtype.str() );
  _product.code( "\tvoid InvalidateDecodingCacheEntry(%s addr);\n", isa().m_addrtype.str() );
  _product.code( "\tvoid InvalidateDecodingCache();\n\n" );
  _product.code( "\tvoid SetLittleEndian();\n" );
  _product.code( "\tvoid SetBigEndian();\n" );
  _product.code( "private:\n" );
  _product.code( "\tbool little_endian;\n" );
  _product.code( "\tvector<DecodeTableEntry" );
  _product.template_abrev( isa().m_tparams );
  _product.code( " > decode_table;\n" );
  _product.code( "\tDecodeMapPage" );
  _product.template_abrev( isa().m_tparams );
  _product.code( " *mru_page;\n" );
  _product.code( "\tDecodeMapPage" );
  _product.template_abrev( isa().m_tparams );
  _product.code( " *decode_hash_table[NUM_DECODE_HASH_TABLE_ENTRIES];\n" );
  _product.code( "\tDecodeMapPage" );
  _product.template_abrev( isa().m_tparams );
  _product.code( " *FindPage(%s page_key)\n", isa().m_addrtype.str() );
  _product.code( "#if defined(__GNUC__) && (__GNUC__ >= 3)\n" );
  _product.code( "\t__attribute__((always_inline))\n" );
  _product.code( "#endif\n" );
  _product.code( "\t;\n" );
  _product.code( "};\n\n" );
}

void
RiscGenerator::generate_operation_class_definition( Product_t& _product, unsigned int word_size ) const {
  _product.template_signature( isa().m_tparams );
  _product.code( "class Operation\n" );
  _product.code( "{\n" );
  _product.code( "public:\n" );

  _product.code( "\tOperation(CodeType code, %s addr, const char *name);\n", isa().m_addrtype.str() );
  _product.code( "\tvirtual ~Operation();\n" );
  _product.code( "\tinline %s GetAddr() { return addr; }\n", isa().m_addrtype.str() );
  _product.code( "\tinline %s GetEncoding() { return encoding; }\n", m_codetype.str() );
  _product.code( "\tinline const char *GetName() { return name; }\n" );
  
  for( Vect_t<Variable_t>::const_iterator var = isa().m_vars.begin(); var < isa().m_vars.end(); ++ var ) {
    _product.usercode( (**var).m_ctype->m_filename.str(),
                       (**var).m_ctype->m_lineno,
                       "\t%s %s;",
                       (**var).m_ctype->m_content.str(),
                       (**var).m_symbol.str() );
  }
  
  for( intptr_t idx = isa().m_actionprotos.size(); (--idx) >= 0; ) {
    if( isa().m_actionprotos[idx]->m_type != ActionProto_t::Static or not isa().m_actionprotos[idx]->m_returns ) continue;
    _product.code( "\t%s %s_result;\n", isa().m_actionprotos[idx]->m_returns->m_content.str(), isa().m_actionprotos[idx]->m_symbol.str() );
  }

  for( intptr_t idx = isa().m_actionprotos.size(); (--idx) >= 0; ) {
    _product.code( "\tvirtual " );
    if( isa().m_actionprotos[idx]->m_returns ) {
      _product.usercode(  isa().m_actionprotos[idx]->m_returns->m_filename.str(), isa().m_actionprotos[idx]->m_returns->m_lineno,
                          "\t%s", isa().m_actionprotos[idx]->m_returns->m_content.str() );
    } else {
      _product.code( "void" );
    }

    _product.code( "\t%s(", isa().m_actionprotos[idx]->m_symbol.str() );

    if( not isa().m_actionprotos[idx]->m_params.empty() ) {
      bool intra = false;
      for( Vect_t<CodePair_t>::const_iterator param = isa().m_actionprotos[idx]->m_params.begin(); param < isa().m_actionprotos[idx]->m_params.end(); ++ param ) {
        if( intra ) _product.code( ",\n" ); else intra = true;
        _product.usercode( (**param).m_ctype->m_filename.str(), (**param).m_ctype->m_lineno,
                           "\t%s", (**param).m_ctype->m_content.str() );
          
        _product.usercode( (**param).m_csymbol->m_filename.str(), (**param).m_csymbol->m_lineno,
                           "\t%s", (**param).m_csymbol->m_content.str() );
      }
    }

    _product.code( "\t);\n" );
  }

  _product.code( "private:\n" );

  _product.code( "\tCodeType encoding;\n" );
  _product.code( "\t%s addr;\n", isa().m_addrtype.str() );
  _product.code( "\tconst char *name;\n" );

  _product.code( "};\n\n" );
}


void
RiscGenerator::generate_operation_impl_class_definition( Product_t& _product, unsigned int word_size ) const {
  for( Vect_t<Operation_t>::const_reverse_iterator op = isa().m_operations.rbegin(); op < isa().m_operations.rend(); ++ op ) {
    _product.template_signature( isa().m_tparams );
    _product.code( "class Op%s : public Operation", Str::capitalize( (**op).m_symbol ).str() );
    _product.template_abrev( isa().m_tparams );
    _product.code( "\n" );

    _product.code( "{\n" );
    _product.code( "public:\n" );
    _product.code( "\tOp%s(CodeType code, %s addr);\n", Str::capitalize( (**op).m_symbol ).str(), isa().m_addrtype.str() );
      
    for( Vect_t<BitField_t>::const_iterator bf = (**op).m_bitfields.begin(); bf < (**op).m_bitfields.end(); ++ bf ) {
      ConstStr_t bf_symbol = (**bf).symbol();
      if( not bf_symbol ) continue;
      _product.code( "\t%s %s;\n", (**bf).ctype( word_size ), bf_symbol.str() );
    }

    if( not (**op).m_variables.empty() ) {
      for( Vect_t<Variable_t>::const_iterator var = (**op).m_variables.begin(); var < (**op).m_variables.end(); ++ var ) {
        _product.usercode( (**var).m_ctype->m_filename.str(),
                           (**var).m_ctype->m_lineno,
                           "\t\t\t%s %s;",
                           (**var).m_ctype->m_content.str(),
                           (**var).m_symbol.str() );
      }
    }
      
    for( Vect_t<Action_t>::const_iterator action = (**op).m_actions.begin(); action < (**op).m_actions.end(); ++ action ) {
      ActionProto_t const* actionproto = (**action).m_actionproto;
      
      if( not actionproto->m_comments.empty() ) {
        for( Vect_t<Comment_t>::const_iterator comm = actionproto->m_comments.begin(); comm < actionproto->m_comments.end(); ++ comm )
          _product.code( "\t%s\n", (**comm).m_content.str() );
      }
          
      _product.code( "\tvirtual\n" );

      if(actionproto->m_returns) {
        _product.usercode( actionproto->m_returns->m_filename.str(), actionproto->m_returns->m_lineno,
                           "\t%s", actionproto->m_returns->m_content.str() );
      }
      else {
        _product.code( "\tvoid\n" );
      }

      _product.code( "\t%s(", actionproto->m_symbol.str() );

      if( not actionproto->m_params.empty() ) {
        bool intra = false;
        for( Vect_t<CodePair_t>::const_iterator param = actionproto->m_params.begin(); param < actionproto->m_params.end(); ++ param ) {
          if( intra ) _product.code( ",\n" ); else intra = true;
          _product.usercode( (**param).m_ctype->m_filename.str(), (**param).m_ctype->m_lineno,
                             "\t%s", (**param).m_ctype->m_content.str() );
          _product.usercode( (**param).m_csymbol->m_filename.str(), (**param).m_csymbol->m_lineno,
                             "\t%s", (**param).m_csymbol->m_content.str() );
        }
        _product.code( "\t" );
      }

      _product.code( ");\n" );
    }

    _product.code( "private:\n" );

    _product.code( "};\n\n" );
  }
}

void
RiscGenerator::generate_operation_constructor( Product_t& _product ) const {
  _product.template_signature( isa().m_tparams );
  _product.code( "Operation" );
  _product.template_abrev( isa().m_tparams );
  _product.code( "::Operation(CodeType code, %s addr, const char *name)\n", isa().m_addrtype.str() );

  _product.code( "{\n" );
  _product.code( "\tthis->encoding = code;\n" );
  _product.code( "\tthis->addr = addr;\n" );
  _product.code( "\tthis->name = name;\n" );
  _product.code( "}\n" );
}

void
RiscGenerator::generate_operation_destructor( Product_t& _product ) const {
  _product.template_signature( isa().m_tparams );
  _product.code( "Operation" );
  _product.template_abrev( isa().m_tparams );
  _product.code( "::~Operation()\n" );
  _product.code( "{\n" );
  
  for( intptr_t idx = isa().m_actionprotos.size(); (--idx) >= 0; ) {
    if( isa().m_actionprotos[idx]->m_type != ActionProto_t::Destructor ) continue;
    _product.code( "\t%s();\n", isa().m_actionprotos[idx]->m_symbol.str() );
  }
  _product.code( "}\n" );
}

void
RiscGenerator::generate_default_actions( Product_t& _product ) const {
  for( intptr_t idx = isa().m_actionprotos.size(); (--idx) >= 0; ) {
    _product.template_signature( isa().m_tparams );

    if(isa().m_actionprotos[idx]->m_returns) {
      _product.usercode( isa().m_actionprotos[idx]->m_returns->m_filename.str(), isa().m_actionprotos[idx]->m_returns->m_lineno,
                         "%s", isa().m_actionprotos[idx]->m_returns->m_content.str() );
    }
    else {
      _product.code( "\nvoid\n" );
    }

    _product.code( "Operation" );
    _product.template_abrev( isa().m_tparams );
    _product.code( "::%s(", isa().m_actionprotos[idx]->m_symbol.str() );

    if( not isa().m_actionprotos[idx]->m_params.empty() ) {
      bool intra = false;
      for( Vect_t<CodePair_t>::const_iterator param = isa().m_actionprotos[idx]->m_params.begin(); param < isa().m_actionprotos[idx]->m_params.end(); ++ param ) {
        if( intra ) _product.code( ",\n" ); else intra = true;
        _product.usercode( (**param).m_ctype->m_filename.str(), (**param).m_ctype->m_lineno,
                           "\t%s", (**param).m_ctype->m_content.str() );
        _product.usercode( (**param).m_csymbol->m_filename.str(), (**param).m_csymbol->m_lineno,
                           "\t%s", (**param).m_csymbol->m_content.str() );
      }
    }

    _product.code(
                  ")\n{\n" );

    _product.usercode( isa().m_actionprotos[idx]->m_filename.str(),
                       isa().m_actionprotos[idx]->m_default_source_code->m_lineno,
                       "{%s}",
                       isa().m_actionprotos[idx]->m_default_source_code->m_content.str() );

    _product.code( "}\n" );
  }
}

void
RiscGenerator::generate_operation_member_methods( Product_t& _product, unsigned int word_size ) const {
  for( Vect_t<Operation_t>::const_reverse_iterator op = isa().m_operations.rbegin(); op < isa().m_operations.rend(); ++ op ) {
    if( not (**op).m_comments.empty() ) {
      for( Vect_t<Comment_t>::const_iterator comm = (**op).m_comments.begin(); comm < (**op).m_comments.end(); ++ comm )
        _product.code( "%s\n", (**comm).m_content.str() );
    }
      
    for( Vect_t<Action_t>::const_iterator action = (**op).m_actions.begin(); action < (**op).m_actions.end(); ++ action ) {
      ActionProto_t const* actionproto = (**action).m_actionproto;

      if( not (**action).m_comments.empty() ) {
        for( Vect_t<Comment_t>::const_iterator comm = (**action).m_comments.begin(); comm < (**action).m_comments.end(); ++ comm )
          _product.code( "%s\n", (**comm).m_content.str() );
      }

      _product.template_signature( isa().m_tparams );

      if( actionproto->m_returns ) {
        _product.usercode(
                          actionproto->m_returns->m_filename.str(),
                          actionproto->m_returns->m_lineno,
                          "%s",
                          actionproto->m_returns->m_content.str() );
      }
      else {
        _product.code( "\nvoid\n" );
      }

      _product.code( "Op%s", Str::capitalize( (**op).m_symbol ).str() );
      _product.template_abrev( isa().m_tparams );
      _product.code( "::%s(", actionproto->m_symbol.str() );

      if( not actionproto->m_params.empty() ) {
        bool intra = false;            
        for( Vect_t<CodePair_t>::const_iterator param = actionproto->m_params.begin(); param < actionproto->m_params.end(); ++ param ) {
          if( intra ) _product.code( ",\n" ); else intra = true;
          _product.usercode( (**param).m_ctype->m_filename.str(), (**param).m_ctype->m_lineno,
                             "\t%s", (**param).m_ctype->m_content.str() );
          _product.usercode( (**param).m_csymbol->m_filename.str(), (**param).m_csymbol->m_lineno,
                             "\t%s", (**param).m_csymbol->m_content.str() );
        }
      }

      _product.code( ")\n{\n" );
      _product.usercode( (**action).m_source_code->m_filename.str(), (**action).m_source_code->m_lineno,
                         "{%s}", (**action).m_source_code->m_content.str() );
      _product.code( "}\n" );
    }

    _product.code( "\n" );

    _product.template_signature( isa().m_tparams );
    _product.code( "static Operation" );
    _product.template_abrev( isa().m_tparams );
    _product.code( " *DecodeOp%s(", Str::capitalize( (**op).m_symbol ).str() );
    _product.code( "CodeType code, %s addr)\n", isa().m_addrtype.str() );
    _product.code( "{\n" );
    _product.code( "\treturn new Op%s", Str::capitalize( (**op).m_symbol ).str() );
    _product.template_abrev( isa().m_tparams );
    _product.code( "(code, addr);\n" );
    _product.code( "}\n\n" );
  }
}

void
RiscGenerator::generate_operation_constructors( Product_t& _product, unsigned int word_size ) const {
  for( Vect_t<Operation_t>::const_reverse_iterator op = isa().m_operations.rbegin(); op < isa().m_operations.rend(); ++ op ) {
    _product.template_signature( isa().m_tparams );
    _product.code( "Op%s", Str::capitalize( (**op).m_symbol ).str() );
    _product.template_abrev( isa().m_tparams );
    _product.code( "::Op%s(CodeType code, %s addr) : Operation",
                   Str::capitalize( (**op).m_symbol ).str(),
                   isa().m_addrtype.str() );
    _product.template_abrev( isa().m_tparams );
    _product.code( "(code, addr, \"%s\")\n",
                   (**op).m_symbol.str() );
    _product.code( "{\n" );
    
    for( Vect_t<Variable_t>::const_iterator var = isa().m_vars.begin(); var < isa().m_vars.end(); ++ var ) {
      if( not (**var).m_cinit ) continue;
      _product.code( "\t%s = ", (**var).m_symbol.str() );
      _product.usercode((**var).m_cinit->m_filename.str(), (**var).m_cinit->m_lineno, "%s;\n", (**var).m_cinit->m_content.str() );
    }

    if( not (**op).m_variables.empty() ) {
      for( Vect_t<Variable_t>::const_iterator var = (**op).m_variables.begin(); var < (**op).m_variables.end(); ++ var ) {
        if( (**var).m_cinit ) {
          _product.code( "\t%s = ", (**var).m_symbol.str() );
          _product.usercode( (**var).m_cinit->m_filename.str(), (**var).m_cinit->m_lineno, "%s;\n", (**var).m_cinit->m_content.str() );
        }
      }
    }
    
    unsigned int shift = opcode( *op ).m_size;
    for( Vect_t<BitField_t>::const_iterator bf = (**op).m_bitfields.begin(); bf < (**op).m_bitfields.end(); ++ bf ) {
      shift -= (**bf).m_size;
      ConstStr_t bf_symbol = (**bf).symbol();
      if( not bf_symbol ) continue;
        
      _product.code( "\t%s = %s;\n", bf_symbol.str(), (**bf).cexpr_opaccess( word_size, shift, "code" ).str() );
    }

    _product.code( "}\n\n" );
  }
}

void
RiscGenerator::generate_decoder( Product_t& _product, unsigned int word_size ) const {
  _product.template_signature( isa().m_tparams );
  _product.code( "DecodeMapPage" );
  _product.template_abrev( isa().m_tparams );
  _product.code( "::DecodeMapPage(%s key)\n", isa().m_addrtype.str() );
  _product.code( "{\n" );
  _product.code( "\tthis->key = key;\n" );
  _product.code( "\tmemset(operation, 0, sizeof(operation));\n" );
  _product.code( "\tnext = 0;\n" );
  _product.code( "}\n\n" );

  _product.template_signature( isa().m_tparams );
  _product.code( "DecodeMapPage" );
  _product.template_abrev( isa().m_tparams );
  _product.code( "::~DecodeMapPage()\n" );
  _product.code( "{\n" );
  _product.code( "\tunsigned int idx;\n" );
  _product.code( "\tfor(idx = 0; idx < NUM_OPERATIONS_PER_PAGE; idx++)\n" );
  _product.code( "\t\tdelete operation[idx];\n" );
  _product.code( "}\n\n" );

  _product.template_signature( isa().m_tparams );
  _product.code( "\tDecodeTableEntry" );
  _product.template_abrev( isa().m_tparams );
  _product.code( "::DecodeTableEntry(CodeType opcode, CodeType opcode_mask, Operation" );
  _product.template_abrev( isa().m_tparams );
  _product.code( " *(*decode)(CodeType, %s))\n", isa().m_addrtype.str() );
  _product.code( "\t{\n" );
  _product.code( "\t\tthis->opcode = opcode;\n" );
  _product.code( "\t\tthis->opcode_mask = opcode_mask;\n" );
  _product.code( "\t\tthis->decode = decode;\n" );
  _product.code( "\t}\n\n" );

  _product.template_signature( isa().m_tparams );
  _product.code( "Decoder" );
  _product.template_abrev( isa().m_tparams );
  _product.code( "::Decoder()\n" );
  _product.code( "{\n" );
  _product.code( "\tlittle_endian = %s;\n", isa().m_little_endian ? "true" : "false" );
  _product.code( "\tmru_page = 0;\n" );
  _product.code( "\tmemset(decode_hash_table, 0, sizeof(decode_hash_table));\n" );
  
  for( Vect_t<Operation_t>::const_reverse_iterator op = isa().m_operations.rbegin(); op < isa().m_operations.rend(); ++ op ) {
    if( (**op).m_condition ) {
      _product.code( "if(" );
      _product.usercode( (**op).m_condition->m_filename.str(), (**op).m_condition->m_lineno,
                        "%s", (**op).m_condition->m_content.str() );
      _product.code( ") " );
    }
    _product.code( "\tdecode_table.push_back(DecodeTableEntry" );
    _product.template_abrev( isa().m_tparams );
    _product.code( "(0x%llx%s, 0x%llx%s, DecodeOp%s",
                   opcode( *op ).m_bits, m_immedsuffix.str(),
                   opcode( *op ).m_mask, m_immedsuffix.str(),
                   Str::capitalize( (**op).m_symbol ).str() );
    _product.template_abrev( isa().m_tparams );
    _product.code( "));\n" );
  }
  _product.code( "}\n\n" );

  _product.template_signature( isa().m_tparams );
  _product.code( "Decoder" );
  _product.template_abrev( isa().m_tparams );
  _product.code( "::~Decoder()\n" );
  _product.code( "{\n" );
  _product.code( "\tInvalidateDecodingCache();\n" );
  _product.code( "}\n\n" );
  
  _product.template_signature( isa().m_tparams );
  _product.code( "void Decoder" );
  _product.template_abrev( isa().m_tparams );
  _product.code( "::Fetch(void *, %s, uint32_t)\n", isa().m_addrtype.str() );
  _product.code( "{\n" );
  _product.code( "\tcerr << \"Calling unimplemented virtual method Fetch\" << endl; \n" );
  _product.code( "\tabort(); \n" );
  _product.code( "}\n\n" );
  

  _product.template_signature( isa().m_tparams );
  _product.code( "Operation" );
  _product.template_abrev( isa().m_tparams );
  _product.code( " *Decoder" );
  _product.template_abrev( isa().m_tparams );
  _product.code( "::NCDecode(%s addr)\n", isa().m_addrtype.str() );
  _product.code( "{\n" );
  _product.code( "\tOperation" );
  _product.template_abrev( isa().m_tparams );
  _product.code( " *operation;\n" );
  _product.code( "\t" );
  if( not isa().m_tparams.empty() ) _product.code( "typename " );
  _product.code( "vector<DecodeTableEntry" );
  _product.template_abrev( isa().m_tparams );
  _product.code( " >::iterator iter;\n" );
  _product.code( "\tCodeType code;\n" );

  _product.code( "\tFetch(&code, addr, sizeof(code));\n" );

  _product.code( "#if BYTE_ORDER == LITTLE_ENDIAN\n" );
  _product.code( "\tif(!little_endian)\n" );
  _product.code( "#else\n" );
  _product.code( "\tif(little_endian)\n" );
  _product.code( "#endif\n" );
  _product.code( "\t{\n" );

  switch( m_codesize ) {
  case 2: _product.code( "\t\tcode = ((code & 0x00ff) << 8) | ((code & 0xff00) >> 8);\n" ); break;
  case 4: _product.code( "\t\tcode = ((code & 0xff000000UL) >> 24) | ((code & 0x00ff0000UL) >> 8) | ((code & 0x0000ff00UL) << 8) | ((code & 0x000000ffUL) << 24);\n" ); break;
  case 8: _product.code(
                        "\t\tcode = ((code & 0xff00000000000000ULL) >> 56)\n"
                        "\t\t     | ((code & 0x00ff000000000000ULL) >> 40)\n"
                        "\t\t     | ((code & 0x0000ff0000000000ULL) >> 24)\n"
                        "\t\t     | ((code & 0x000000ff00000000ULL) >> 8)\n"
                        "\t\t     | ((code & 0x00000000ff000000ULL) << 8)\n"
                        "\t\t     | ((code & 0x0000000000ff0000ULL) << 24)\n"
                        "\t\t     | ((code & 0x000000000000ff00ULL) << 40)\n"
                        "\t\t     | ((code & 0x00000000000000ffULL) << 56);\n" ); break;
  }
  if( 8 * m_codesize > m_codebitsize ) {
    _product.code( "\tcode = code >> %u;\n", 8 * m_codesize - m_codebitsize );
  }

  _product.code( "\t}\n" );

  _product.code( "\tunsigned int count = decode_table.size();\n" );
  _product.code( "\tunsigned int idx;\n" );
  _product.code( "\tfor(idx = 0; idx < count; idx++)\n" );
  _product.code( "\t{\n" );
  _product.code( "\t\tif((code & decode_table[idx].opcode_mask) == decode_table[idx].opcode)\n" );
  _product.code( "\t\t{\n" );
  _product.code( "\t\t\toperation = decode_table[idx].decode(code, addr);\n" );

  for( intptr_t idx = isa().m_actionprotos.size();  (--idx) >= 0; ) {
    switch( isa().m_actionprotos[idx]->m_type ) {
    case ActionProto_t::Constructor:
      _product.code( "\t\t\toperation->%s();\n", isa().m_actionprotos[idx]->m_symbol.str() );
      break;
      
    case ActionProto_t::Static:
      _product.code( "\t\t\t" );
      if( isa().m_actionprotos[idx]->m_returns )
        _product.code( "operation->%s_result = ", isa().m_actionprotos[idx]->m_symbol.str() );
      _product.code( "operation->%s();\n", isa().m_actionprotos[idx]->m_symbol.str() );
      break;
        
    default: break;
    }
  }

  _product.code( "\t\t\treturn operation;\n" );
  _product.code( "\t\t}\n" );
  _product.code( "\t}\n" );

  _product.code( "\toperation = new Operation" );
  _product.template_abrev( isa().m_tparams );
  _product.code( "(code, addr, \"???\");\n" );
    
    
  for( intptr_t idx = isa().m_actionprotos.size();  (--idx) >= 0; ) {
    switch( isa().m_actionprotos[idx]->m_type ) {
    case ActionProto_t::Constructor:
      _product.code( "\toperation->%s();\n", isa().m_actionprotos[idx]->m_symbol.str() );
      break;
    case ActionProto_t::Static:
      _product.code( "\t" );
      if( isa().m_actionprotos[idx]->m_returns )
        _product.code( "operation->%s_result = ", isa().m_actionprotos[idx]->m_symbol.str() );
      _product.code( "operation->%s();\n", isa().m_actionprotos[idx]->m_symbol.str() );
      break;
    default: break;
    }
  }

  _product.code( "\treturn operation;\n" );
  _product.code( "}\n\n" );



  _product.template_signature( isa().m_tparams );
  _product.code( "Operation" );
  _product.template_abrev( isa().m_tparams );
  _product.code( " *Decoder" );
  _product.template_abrev( isa().m_tparams );
  _product.code( "::NCDecode(%s addr, CodeType code)\n", isa().m_addrtype.str() );
  _product.code( "{\n" );
  _product.code( "\tOperation" );
  _product.template_abrev( isa().m_tparams );
  _product.code( " *operation;\n" );
  _product.code( "\t" );
  if( not isa().m_tparams.empty() ) _product.code( "typename " );
  _product.code( "vector<DecodeTableEntry" );
  _product.template_abrev( isa().m_tparams );
  _product.code( " >::iterator iter;\n" );

  _product.code( "\tunsigned int count = decode_table.size();\n" );
  _product.code( "\tunsigned int idx;\n" );
  _product.code( "\tfor(idx = 0; idx < count; idx++)\n" );
  _product.code( "\t{\n" );
  _product.code( "\t\tif((code & decode_table[idx].opcode_mask) == decode_table[idx].opcode)\n" );
  _product.code( "\t\t{\n" );
  _product.code( "\t\t\toperation = decode_table[idx].decode(code, addr);\n" );

  for( intptr_t idx = isa().m_actionprotos.size();  (--idx) >= 0; ) {
    switch( isa().m_actionprotos[idx]->m_type ) {
    case ActionProto_t::Constructor:
      _product.code( "\t\t\toperation->%s();\n", isa().m_actionprotos[idx]->m_symbol.str() );
      break;
    case ActionProto_t::Static:
      _product.code( "\t\t\t" );
      if( isa().m_actionprotos[idx]->m_returns )
        _product.code( "operation->%s_result = ", isa().m_actionprotos[idx]->m_symbol.str() );
      _product.code( "operation->%s();\n", isa().m_actionprotos[idx]->m_symbol.str() );
      break;
    default: break;
    }
  }

  _product.code( "\t\t\treturn operation;\n" );
  _product.code( "\t\t}\n" );
  _product.code( "\t}\n" );

  _product.code( "\toperation = new Operation" );
  _product.template_abrev( isa().m_tparams );
  _product.code( "(code, addr, \"???\");\n" );

  for( intptr_t idx = isa().m_actionprotos.size();  (--idx) >= 0; ) {
    switch( isa().m_actionprotos[idx]->m_type ) {
    case ActionProto_t::Constructor:
      _product.code( "\toperation->%s();\n", isa().m_actionprotos[idx]->m_symbol.str() );
      break;
    case ActionProto_t::Static:
      _product.code( "\t" );
      if( isa().m_actionprotos[idx]->m_returns )
        _product.code( "operation->%s_result = ", isa().m_actionprotos[idx]->m_symbol.str() );
      _product.code( "operation->%s();\n", isa().m_actionprotos[idx]->m_symbol.str() );
    default: break;
    }
  }

  _product.code( "\treturn operation;\n" );
  _product.code( "}\n\n" );


  _product.template_signature( isa().m_tparams );
  _product.code( "void Decoder" );
  _product.template_abrev( isa().m_tparams );
  _product.code( "::InvalidateDecodingCache()\n" );
  _product.code( "{\n" );
  _product.code( "\tuint32_t index;\n" );
  _product.code( "\tmru_page = 0;\n" );
  _product.code( "\tfor(index = 0; index < NUM_DECODE_HASH_TABLE_ENTRIES; index++)\n" );
  _product.code( "\t{\n" );
  _product.code( "\t\tDecodeMapPage" );
  _product.template_abrev( isa().m_tparams );
  _product.code( " *page, *next_page;\n" );

  _product.code( "\t\tpage = decode_hash_table[index];\n" );
  _product.code( "\t\tif(page)\n" );
  _product.code( "\t\t{\n" );
  _product.code( "\t\t\tdo\n" );
  _product.code( "\t\t\t{\n" );
  _product.code( "\t\t\tnext_page = page->next;\n" );
  _product.code( "\t\t\tdelete page;\n" );
  _product.code( "\t\t\tpage = next_page;\n" );
  _product.code( "\t\t\t} while(page);\n" );
  _product.code( "\t\tdecode_hash_table[index] = 0;\n" );
  _product.code( "\t\t}\n" );
  _product.code( "\t}\n" );
  _product.code( "}\n\n" );

  _product.template_signature( isa().m_tparams );
  _product.code( "void Decoder" );
  _product.template_abrev( isa().m_tparams );
  _product.code( "::InvalidateDecodingCacheEntry(%s addr)\n", isa().m_addrtype.str() );
  _product.code( "{\n" );
  _product.code( "\t%s page_key = addr / %u / NUM_OPERATIONS_PER_PAGE;\n", isa().m_addrtype.str(), m_codesize );
  _product.code( "\tif(mru_page && mru_page->key == page_key) mru_page = 0;\n" );
  _product.code( "\tuint32_t index = page_key %% NUM_DECODE_HASH_TABLE_ENTRIES; // hash the key\n" );
  _product.code( "\tDecodeMapPage" );
  _product.template_abrev( isa().m_tparams );
  _product.code( " *prev, *cur;\n" );
  _product.code( "\tcur = decode_hash_table[index];\n" );
  _product.code( "\tif(cur)\n" );
  _product.code( "\t{\n" );
  _product.code( "\t\tif(cur->key == page_key)\n" );
  _product.code( "\t\t{\n" );
  _product.code( "\t\t\tdecode_hash_table[index] = cur->next;\n" );
  _product.code( "\t\t\tdelete cur;\n" );
  _product.code( "\t\t\treturn;\n" );
  _product.code( "\t\t}\n" );
  _product.code( "\t\tprev = cur;\n" );
  _product.code( "\t\tcur = cur->next;\n" );
  _product.code( "\t\tif(cur)\n" );
  _product.code( "\t\t{\n" );
  _product.code( "\t\t\tdo\n" );
  _product.code( "\t\t\t{\n" );
  _product.code( "\t\t\t\tif(cur->key == page_key)\n" );
  _product.code( "\t\t\t\t{\n" );
  _product.code( "\t\t\t\t\tprev->next = cur->next;\n" );
  _product.code( "\t\t\t\t\tcur->next = 0;\n" );
  _product.code( "\t\t\t\t\tdelete cur;\n" );
  _product.code( "\t\t\t\t\treturn;\n" );
  _product.code( "\t\t\t\t}\n" );
  _product.code( "\t\t\t\tprev = cur;\n" );
  _product.code( "\t\t\t} while((cur = cur->next) != 0);\n" );
  _product.code( "\t\t}\n" );
  _product.code( "\t}\n" );
  _product.code( "}\n\n" );

  _product.template_signature( isa().m_tparams );
  _product.code( "inline DecodeMapPage" );
  _product.template_abrev( isa().m_tparams );
  _product.code( " *Decoder" );
  _product.template_abrev( isa().m_tparams );
  _product.code( "::FindPage(%s page_key)\n", isa().m_addrtype.str() );
  _product.code( "{\n" );
  _product.code( "\tif(mru_page && mru_page->key == page_key) return mru_page;\n" );
  _product.code( "\tuint32_t index = page_key %% NUM_DECODE_HASH_TABLE_ENTRIES; // hash the key\n" );
  _product.code( "\tDecodeMapPage" );
  _product.template_abrev( isa().m_tparams );
  _product.code( " *prev, *cur;\n" );
  _product.code( "\tcur = decode_hash_table[index];\n" );
  _product.code( "\tif(cur)\n" );
  _product.code( "\t{\n" );
  _product.code( "\t\tif(cur->key == page_key)\n" );
  _product.code( "\t\t{\n" );
  _product.code( "\t\t\tmru_page = cur;\n" );
  _product.code( "\t\t\treturn cur;\n" );
  _product.code( "\t\t}\n" );
  _product.code( "\t\tprev = cur;\n" );
  _product.code( "\t\tcur = cur->next;\n" );
  _product.code( "\t\tif(cur)\n" );
  _product.code( "\t\t{\n" );
  _product.code( "\t\t\tdo\n" );
  _product.code( "\t\t\t{\n" );
  _product.code( "\t\t\t\tif(cur->key == page_key)\n" );
  _product.code( "\t\t\t\t{\n" );
  _product.code( "\t\t\t\t\tprev->next = cur->next;\n" );
  _product.code( "\t\t\t\t\tcur->next= decode_hash_table[index];\n" );
  _product.code( "\t\t\t\t\tdecode_hash_table[index] = cur;\n" );
  _product.code( "\t\t\t\t\tmru_page = cur;\n" );
  _product.code( "\t\t\t\t\treturn cur;\n" );
  _product.code( "\t\t\t\t}\n" );
  _product.code( "\t\t\t\tprev = cur;\n" );
  _product.code( "\t\t\t} while((cur = cur->next) != 0);\n" );
  _product.code( "\t\t}\n" );
  _product.code( "\t}\n" );
  _product.code( "\treturn 0;\n" );
  _product.code( "}\n\n" );




  _product.template_signature( isa().m_tparams );
  _product.code( "Operation" );
  _product.template_abrev( isa().m_tparams );
  _product.code( " *Decoder" );
  _product.template_abrev( isa().m_tparams );
  _product.code( "::Decode(%s addr)\n", isa().m_addrtype.str() );
  _product.code( "{\n" );
  _product.code( "\tOperation" );
  _product.template_abrev( isa().m_tparams );
  _product.code( " *operation;\n" );
  _product.code( "\t%s page_key = addr / %u / NUM_OPERATIONS_PER_PAGE;\n", isa().m_addrtype.str(), m_codesize );
  _product.code( "\tDecodeMapPage" );
  _product.template_abrev( isa().m_tparams );
  _product.code( " *page;\n" );
  _product.code( "\tpage = FindPage(page_key);\n" );
  _product.code( "\tif(!page)\n" );
  _product.code( "\t{\n" );
  // _product.code( "\t\t\tfprintf(stderr, \"page miss at 0x%%08x\\n\", addr);\n" );
  _product.code( "\t\tpage = new DecodeMapPage" );
  _product.template_abrev( isa().m_tparams );
  _product.code( "(page_key);\n" );
  _product.code( "\t\tuint32_t index = page_key %% NUM_DECODE_HASH_TABLE_ENTRIES; // hash the key\n" );
  _product.code( "\t\tpage->next = decode_hash_table[index];\n" );
  _product.code( "\t\tdecode_hash_table[index] = page;\n" );
  _product.code( "\t\tmru_page = page;\n" );
  _product.code( "\t}\n" );
  _product.code( "\toperation = page->operation[(addr / %u) & (NUM_OPERATIONS_PER_PAGE - 1)];\n", m_codesize );
  _product.code( "\tif(operation)\n" );
  _product.code( "\t{\n" );
  // _product.code( "\t\tfprintf(stderr, \"hit at 0x%%08x\\n\", addr);\n" );
  _product.code( "\t\treturn operation;\n" );
  _product.code( "\t}\n" );
  // _product.code( "\tfprintf(stderr, \"miss at 0x%%08x\\n\", addr);\n" );
  _product.code( "\toperation = NCDecode(addr);\n" );
  _product.code( "\tpage->operation[(addr / %u) & (NUM_OPERATIONS_PER_PAGE - 1)] = operation;\n", m_codesize );
  _product.code( "\treturn operation;\n" );
  _product.code( "}\n\n" );

  _product.template_signature( isa().m_tparams );
  _product.code( "Operation" );
  _product.template_abrev( isa().m_tparams );
  _product.code( " *Decoder" );
  _product.template_abrev( isa().m_tparams );
  _product.code( "::Decode(%s addr, CodeType insn)\n", isa().m_addrtype.str() );
  _product.code( "{\n" );
  _product.code( "\tOperation" );
  _product.template_abrev( isa().m_tparams );
  _product.code( " *operation;\n" );
  _product.code( "\t%s page_key = addr / %u / NUM_OPERATIONS_PER_PAGE;\n", isa().m_addrtype.str(), m_codesize );
  _product.code( "\tDecodeMapPage" );
  _product.template_abrev( isa().m_tparams );
  _product.code( " *page;\n" );
  _product.code( "\tpage = FindPage(page_key);\n" );
  _product.code( "\tif(!page)\n" );
  _product.code( "\t{\n" );
  // _product.code( "\t\t\tfprintf(stderr, \"page miss at 0x%%08x\\n\", addr);\n" );
  _product.code( "\t\tpage = new DecodeMapPage" );
  _product.template_abrev( isa().m_tparams );
  _product.code( " (page_key);\n" );
  _product.code( "\t\tuint32_t index = page_key %% NUM_DECODE_HASH_TABLE_ENTRIES; // hash the key\n" );
  _product.code( "\t\tpage->next = decode_hash_table[index];\n" );
  _product.code( "\t\tdecode_hash_table[index] = page;\n" );
  _product.code( "\t\tmru_page = page;\n" );
  _product.code( "\t}\n" );
  _product.code( "\toperation = page->operation[(addr / %u) & (NUM_OPERATIONS_PER_PAGE - 1)];\n", m_codesize );
  _product.code( "\tif(operation)\n" );
  _product.code( "\t{\n" );
  // _product.code( "\t\tfprintf(stderr, \"hit at 0x%%08x\\n\", addr);\n" );
  _product.code( "\t\tif(operation->GetEncoding() == insn && operation->GetAddr() == addr)\n" );
  _product.code( "\t\t\treturn operation;\n" );
  _product.code( "\t\tdelete operation;\n" );
  _product.code( "\t}\n" );
  // _product.code( "\tfprintf(stderr, \"miss at 0x%%08x\\n\", addr);\n" );
  _product.code( "\toperation = NCDecode(addr, insn);\n" );
  _product.code( "\tpage->operation[(addr / %u) & (NUM_OPERATIONS_PER_PAGE - 1)] = operation;\n", m_codesize );
  _product.code( "\treturn operation;\n" );
  _product.code( "}\n\n" );

  _product.template_signature( isa().m_tparams );
  _product.code( "void Decoder" );
  _product.template_abrev( isa().m_tparams );
  _product.code( "::SetLittleEndian()\n" );
  _product.code( "{\n" );
  _product.code( "\tlittle_endian = true;\n" );
  _product.code( "}\n\n" );

  _product.template_signature( isa().m_tparams );
  _product.code( "void Decoder" );
  _product.template_abrev( isa().m_tparams );
  _product.code( "::SetBigEndian()\n" );
  _product.code( "{\n" );
  _product.code( "\tlittle_endian = false;\n" );
  _product.code( "}\n\n" );
}

/** Generates one C source file and one C header
    @param output a C string containing the name of the output filenames without the file name extension
    @param word_size define the minimum word size to hold the operand bit field,
    if zero uses the smallest type which hold the operand bit field
    @return non-zero if no error occurs during generation
*/
bool
RiscGenerator::generate_iss( Product_t& _product, unsigned int _wordsize ) const {
  cerr << "Instruction Set Encoding: " << (isa().m_little_endian ? "little-endian" : "big-endian") << "\n";
  
  /*******************/
  /*** Header file ***/
  /*******************/
  if( not _product.open( "hh" ) ) {
    cerr << GENISSLIB << ": can't open header file '" << _product.m_filename << "'.\n";
    return false;
  }
    
  _product.code( "/*** File generated by %s version %s: DO NOT MODIFY ***/\n", GENISSLIB, GENISSLIB_VERSION );
    
  ConstStr_t headerid;
    
  {
    Str::Buf ns_header_str( Str::Buf::Recycle );
    char const* sep = "";
    for( vector<ConstStr_t>::const_iterator piece = isa().m_namespace.begin(); piece < isa().m_namespace.end(); ++ piece ) {
      ns_header_str.write( sep ).write( (*piece).str() );
      sep = "__";
    }
    headerid = Str::fmt( "__%s_%s_HH__", Str::tokenize( _product.m_prefix ).str(), ns_header_str.m_storage );
  }

  _product.code( "#ifndef %s\n", headerid.str() );
  _product.code( "#define %s\n", headerid.str() );
  
  _product.code( "#ifndef __STDC_FORMAT_MACROS\n"
                 "#define __STDC_FORMAT_MACROS\n"
                 "#endif\n\n" );
  
  _product.code( "#include <vector>\n" );
  _product.code( "#include <inttypes.h>\n" );
  
  _product.ns_enter( isa().m_namespace );
  _product.code( "using namespace std;\n" );
  _product.code( "typedef %s CodeType;\n\n", m_codetype.str() );
  
  generate_decoder_class_definition( _product );
  _product.ns_leave( isa().m_namespace );
  
  for( Vect_t<SourceCode_t>::const_iterator srccode = isa().m_decl_srccodes.begin(); srccode < isa().m_decl_srccodes.end(); ++ srccode ) {
    _product.usercode( (**srccode).m_filename.str(), (**srccode).m_lineno, "%s", (**srccode).m_content.str() );
  }
  
  _product.code( "#ifndef ROTL\n" );
  _product.code( "#define ROTL(v, n) (((v) << (n)) | (((v) >> ((sizeof(v) * 8) - (n))) & ((1 << (n)) - 1)))\n" );
  _product.code( "#endif\n" );
  _product.code( "#ifndef ROTR\n" );
  _product.code( "#define ROTR(v, n) (((v) >> (n)) | (((v) & ((1 << (n)) - 1)) << ((sizeof(v) * 8) - (n))))\n" );
  _product.code( "#endif\n" );
  _product.code( "#ifndef SEXT\n" );
  _product.code( "#define SEXT(m, n, v) (((int##m##_t) (v) << (n)) >> (n))\n" );
  _product.code( "#endif\n" );
  
  _product.ns_enter( isa().m_namespace );
  generate_operation_class_definition( _product, _wordsize );
  _product.ns_leave( isa().m_namespace );
  
  _product.code( "#endif\n" );
  
  /*******************/
  /*** Source file ***/
  /*******************/
  if( not _product.open( isa().m_tparams.empty() ? "cc" : "tcc" ) ) {
    cerr << GENISSLIB << ": can't open header file '" << _product.m_filename << "'.\n";
    return false;
  }
  
  //  _product.code( "/*** File generated by %s version %s: DO NOT MODIFY ***/\n", GENISSLIB, GENISSLIB_VERSION );
  
  _product.code( "#include \"%s.hh\"\n"
                 "#include <stdlib.h>\n"
                 "#include <stdio.h>\n\n"
                 "#include <string.h>\n\n"
                 , _product.m_prefix.str() );
  
  for( Vect_t<SourceCode_t>::const_iterator srccode = isa().m_impl_srccodes.begin(); srccode < isa().m_impl_srccodes.end(); ++ srccode )
    _product.usercode( (**srccode).m_filename.str(), (**srccode).m_lineno, "%s", (**srccode).m_content.str() );
  
  _product.ns_enter( isa().m_namespace );
  
  generate_operation_constructor( _product );
  generate_operation_destructor( _product );
  generate_default_actions( _product );
  
  generate_operation_impl_class_definition( _product, _wordsize );
  generate_operation_member_methods( _product, _wordsize );
  generate_operation_constructors( _product, _wordsize );
  generate_decoder( _product, _wordsize );
  
  _product.ns_leave( isa().m_namespace );
  _product.close();
  
  return true;
}
