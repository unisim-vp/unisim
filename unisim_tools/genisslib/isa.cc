/***************************************************************************
                                    isa.cc
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

#include <isa.hh>
#include <main.hh>
#include <operation.hh>
#include <subdecoder.hh>
#include <strtools.hh>
#include <action.hh>
#include <comment.hh>
#include <sourcecode.hh>
#include <bitfield.hh>
#include <variable.hh>
#include <specialization.hh>
#include <scanner.hh>
#include <product.hh>
#include <unistd.h>
#include <cassert>
#include <cerrno>
#include <iostream>
#include <riscgenerator.hh>
#include <ciscgenerator.hh>

using namespace std;

/** Constructor of Isa instance 
 */
Isa::Isa()
  : m_decoder( RiscDecoder ), m_is_subdecoder( false ), m_withsource( false ),
    m_little_endian( false ), m_rev_forder( false ), m_rev_worder( false )
{}

/** Destructor for isa instance
 *  Where most of the allocated stuff will be released
 */
Isa::~Isa() {}

/** Search for an operation object
    @param operation_symbol a symbol object representing the operation
    @return the matching operation object, null if no operation object matches
*/
Operation_t*
Isa::operation( ConstStr_t _symbol ) {
  for( Vect_t<Operation_t>::iterator op = m_operations.begin(); op < m_operations.end(); ++ op )
    if( (*op)->m_symbol == _symbol ) return *op;
  
  return 0;
}

/** Remove an operation object from the global operation object list (operation_list)
    @param operation the operation object to remove
*/
void
Isa::remove( Operation_t* _op ) {
  for( Vect_t<Operation_t>::iterator iter = m_operations.begin(); iter < m_operations.end(); ++ iter ) {
    if( *iter != _op ) continue;
    m_operations.erase( iter );
    return;
  }
}

/** Search the global group lists for the given symbol
    @param _symbol the group symbol looked for
    @return the found group
*/
Group_t*
Isa::group( ConstStr_t _symbol ) {
  for( Vect_t<Group_t>::iterator group = m_groups.begin(); group < m_groups.end(); ++ group )
    if( (*group)->m_symbol == _symbol ) return *group;
  
  return 0;
}

/** Search for an action prototype object
    @param action_proto_symbol a symbol object representing the action prototype
    @return the matching action prototype object, null if no action prototype object matches
*/
ActionProto_t const*
Isa::actionproto( ConstStr_t _symbol ) const {
  for( Vect_t<ActionProto_t>::const_iterator proto = m_actionprotos.begin(); proto < m_actionprotos.end(); ++ proto )
    if( (*proto)->m_symbol == _symbol ) return *proto;

  return 0;
}

/** Remove an action prototype object from the global action prototype object list (action_proto_list)
    @param actionproto the action prototype object to remove
*/
void
Isa::remove( ActionProto_t const* _ap ) {
  for( Vect_t<ActionProto_t>::iterator iter = m_actionprotos.begin(); iter < m_actionprotos.end(); ++ iter ) {
    if( *iter != _ap ) continue;
    m_actionprotos.erase( iter );
    return;
  }
}

/** Dump all objects from the global objects list (source_code_list, action_proto_list, operation_list) into a stream
    with the same format as in the original code
    @param _sink a stream
*/
void
Isa::expand( ostream& _sink ) const {
  // dumping namespace
  if( not m_namespace.empty() ) {
    _sink << "namespace ";
    char const* sep = "";
    for( std::vector<ConstStr_t>::const_iterator piece = m_namespace.begin(); piece < m_namespace.end(); sep = "::", ++ piece )
      _sink << sep << (*piece);
    _sink << '\n';
  }
  
  // dumping template parameters
  if( not m_tparams.empty() ) {
    _sink << "template <";
    char const* sep = "";
    for( Vect_t<CodePair_t>::const_iterator iter = m_tparams.begin(); iter < m_tparams.end(); sep = ", ", ++ iter )
      _sink << sep << *(*iter);
    _sink << ">\n";
  }
  _sink << '\n';
  
  // dumping global parameters
  _sink << "set endianness " << (m_little_endian ? "little" : "big") << "\n";
  _sink << "set addressclass {" << m_addrtype << "}\n";
  
  for( Vect_t<SourceCode_t>::const_iterator srccode = m_decl_srccodes.begin(); srccode < m_decl_srccodes.end(); ++ srccode )
    _sink << "decl " << *(*srccode) << "\n\n";
  for( Vect_t<SourceCode_t>::const_iterator srccode = m_impl_srccodes.begin(); srccode < m_impl_srccodes.end(); ++ srccode )
    _sink << "impl " << *(*srccode) << "\n\n";
  _sink << '\n';

  if( not m_vars.empty() ) {
    _sink << "var ";
    char const* sep = "";
    for( Vect_t<Variable_t>::const_iterator var = m_vars.begin(); var < m_vars.end(); sep = ", ", ++ var )
      _sink << sep << *(*var);
    _sink << '\n';
  }
  _sink << '\n';
  
  for( Vect_t<ActionProto_t>::const_iterator ap = m_actionprotos.begin(); ap < m_actionprotos.end(); ++ap )
    _sink << *(*ap) << '\n';
  
  _sink << '\n';
  
  for( Vect_t<Operation_t>::const_iterator op = m_operations.begin(); op < m_operations.end(); ++ op )
    _sink << *(*op) << '\n';
}

auto_ptr<Generator>
Isa::generator() const {
  switch( m_decoder ) {
  case RiscDecoder: return auto_ptr<Generator>( new RiscGenerator() );
  case CiscDecoder: return auto_ptr<Generator>( new CiscGenerator() );
    // case VliwDecoder: return new VliwGenerator( this ); break;
  default: break;
  }
  assert( false );
  return auto_ptr<Generator>( 0 );
}

bool
Isa::sanity_checks() const {
  if( not m_addrtype ) {
    // FIXME: Downgraded warning to error (default pointer size for
    // target architecture is nonsense)
    cerr << "error: no architecture address type found." << endl;
    return false;
  }
  
  // Checking operations
  for( Vect_t<Operation_t>::const_iterator op = m_operations.begin(); op < m_operations.end(); ++ op ) {
    // Looking for bitfield conflicts
    for( Vect_t<BitField_t>::const_iterator bf = (**op).m_bitfields.begin(); bf < (**op).m_bitfields.end(); ++ bf ) {
      ConstStr_t bf_symbol = (**bf).symbol();
      if( not bf_symbol ) continue;
      for( Vect_t<BitField_t>::const_iterator pbf = (**op).m_bitfields.begin(); pbf < bf; ++ pbf ) {
        ConstStr_t pbf_symbol = (**pbf).symbol();
        if( pbf_symbol != bf_symbol ) continue;
        (**op).m_fileloc.err( "error: duplicated bit field `%s' in operation `%s'", bf_symbol.str(), (**op).m_symbol.str() );
        return false;
      }
    }
    // Looking for variable conflicts
    if( not (**op).m_variables.empty() ) {
      for( Vect_t<Variable_t>::const_iterator checked = (**op).m_variables.begin(); checked < (**op).m_variables.end(); ++ checked ) {
        Vect_t<Variable_t>::const_iterator found;
        for( found = m_vars.begin(); found < m_vars.end(); ++ found )
          if( (**found).m_symbol == (**checked).m_symbol ) break;
        
        if( (found < m_vars.end()) ) {
          (**checked).m_ctype->m_fileloc.err( "error: in operation `%s', variable `%s' is already defined as global",
                                              (**op).m_symbol.str(), (**checked).m_symbol.str() );
          (**found).m_ctype->m_fileloc.err( "variable `%s' previously defined here", (**found).m_symbol.str() );
          return false;
        }
        
        for( found = (**op).m_variables.begin(); found < checked; ++ found )
          if( (**found).m_symbol == (**checked).m_symbol ) break;

        if( found < checked ) {
          (**checked).m_ctype->m_fileloc.err( "error: in operation `%s', variable `%s' is defined several times", (**op).m_symbol.str(), (**checked).m_symbol.str() );
          return false;
        }
      }
    }
  }
  return true;
}

// SubDecoder_t const*
// Isa::subdecoder( ConstStr_t _symbol ) const {
//   for( Vect_t<SubDecoder_t>::const_iterator sd = m_subdecoders.begin(); sd < m_subdecoders.end(); ++ sd )
//     if( (**sd).m_symbol == _symbol ) return *sd;
//   return 0;
// }


/** Output a rule file (<filename>) suitable for make describing the dependencies of the main source file.
    @param _sink a stream
*/
void
Isa::deps( ostream& _sink, char const* _prefix ) const {
  if( m_tparams.empty() ) {
    _sink << _prefix << ".cc " << _prefix << ".hh:";
  } else {
    _sink << _prefix << ".tcc " << _prefix << ".hh:";
  }
  for( std::vector<ConstStr_t>::const_iterator inc = m_includes.begin(); inc < m_includes.end(); ++ inc )
    _sink << " \\\n " << *inc;
  _sink << "\n\n";
}

void
Isa::specialize() {
  for( Vect_t<Specialization_t>::iterator spec = m_specializations.begin(); spec < m_specializations.end(); ++ spec ) {
    m_operations.push_back( (**spec).newop() );
  }
}

void
Isa::setparam( ConstStr_t key, ConstStr_t value ) {
  static ConstStr_t   codetype( "codetype",        Scanner::symbols );
  static ConstStr_t     scalar( "scalar",          Scanner::symbols );
  static ConstStr_t     buffer( "buffer",          Scanner::symbols );
  static ConstStr_t subdecoder( "subdecoder_p",    Scanner::symbols );
  static ConstStr_t withsource( "withsource_p",    Scanner::symbols );
  static ConstStr_t     istrue( "true",            Scanner::symbols );
  static ConstStr_t    isfalse( "false",           Scanner::symbols );
  static ConstStr_t endianness( "endianness",      Scanner::symbols );
  static ConstStr_t      isbig( "big",             Scanner::symbols );
  static ConstStr_t   islittle( "little",          Scanner::symbols );
  static ConstStr_t     forder( "fields_order",    Scanner::symbols );
  static ConstStr_t     worder( "words_order",     Scanner::symbols );
  static ConstStr_t   isnormal( "normal",          Scanner::symbols );
  static ConstStr_t  isreverse( "reverse",         Scanner::symbols );
  
  if        (key == codetype) {
    if      (value == scalar) m_decoder = RiscDecoder;
    else if (value == buffer) m_decoder = CiscDecoder;
  } else if (key == subdecoder) {
    if      (value == istrue)  m_is_subdecoder = true;
    else if (value == isfalse) m_is_subdecoder = false;
  } else if (key == withsource) {
    if      (value == istrue)  m_withsource = true;
    else if (value == isfalse) m_withsource = false;
  } else if (key == endianness) {
    if      (value == isbig)    m_little_endian = false;
    else if (value == islittle) m_little_endian = true;
  } else if (key == forder) {
    if      (value == isnormal)  m_rev_forder = false;
    else if (value == isreverse) m_rev_forder = true;
  } else if (key == worder) {
    if      (value == isnormal)  m_rev_worder = false;
    else if (value == isreverse) m_rev_worder = false;
  }
}

void
Isa::setparam( ConstStr_t key, SourceCode_t* value ) {
  static ConstStr_t  addressclass( "addressclass",  Scanner::symbols );
  static ConstStr_t codetypeclass( "codetypeclass", Scanner::symbols );
  
  if        (key == addressclass) {
    m_addrtype = value->m_content;
    delete value;
  } else if (key == codetypeclass) {
    //m_codetype = value->m_content;
    delete value;
  }
}

SDClass_t const*
Isa::sdclass( std::vector<ConstStr_t>& _namespace ) const {
  for( Vect_t<SDClass_t>::const_iterator sdc = m_sdclasses.begin(); sdc != m_sdclasses.end(); ++ sdc ) {
    if( (**sdc).m_namespace == _namespace ) return *sdc;
  }
  return 0;
}

SDInstance_t const*
Isa::sdinstance( ConstStr_t _symbol ) const {
  for( Vect_t<SDInstance_t>::const_iterator sdi = m_sdinstances.begin(); sdi != m_sdinstances.end(); ++ sdi ) {
    if( (**sdi).m_symbol == _symbol ) return *sdi;
  }
  return 0;
}
