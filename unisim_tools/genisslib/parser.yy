%{ /* -*- C++ -*- */
/***************************************************************************
                      parser.ypp  -  syntaxical analyzer
                             -------------------
    begin                : Thu May 25 2003
    copyright            : (C) 2003 CEA and Universite Paris Sud
    author               : Gilles Mouchard
    email                : mouchard@lri.fr
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include <yystype.hh>
#include <vect.hh>
#include <isa.hh>
#include <sourcecode.hh>
#include <action.hh>
#include <comment.hh>
#include <operation.hh>
#include <subdecoder.hh>
#include <variable.hh>
#include <bitfield.hh>
#include <specialization.hh>
#include <scanner.hh>
#include <conststr.hh>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <vector>
#include <cstdlib>
  
int yylex();
int yyerror( char const* _err );

static
void
create_action( Operation_t* _operation, ActionProto_t const* _actionproto, SourceCode_t* _actioncode ) {
  Action_t const* prev_action = _operation->action( _actionproto );

  if( prev_action ) {
    Scanner::fileloc.err( "error: action `%s.%s' redefined",
                          _operation->m_symbol.str(), _actionproto->m_symbol.str() );
    
    prev_action->m_fileloc.err( "action `%s.%s' previously defined here",
                                _operation->m_symbol.str(), _actionproto->m_symbol.str() );
    exit( -1 );
  }
  
  _operation->add( new Action_t( _actionproto, _actioncode, Scanner::comments, Scanner::fileloc ) );
}

static void
extend_oplist( Vect_t<Operation_t>* _oplist, Operation_t* _op ) {
  // Suppress duplicated entries
  for( Vect_t<Operation_t>::const_iterator node = _oplist->begin(); node < _oplist->end(); ++ node ) {
    if( (*node)->m_symbol != _op->m_symbol ) continue;
    Scanner::fileloc.err( "warning: duplicated operation `%s' in list", _op->m_symbol.str() );
    return;
  }
  
  _oplist->append( _op );
}

static bool
extend_oplist( Vect_t<Operation_t>* _oplist, ConstStr_t _symbol ) {
  /* Symbol points to either an operation or a group */
  Operation_t* operation = Scanner::isa().operation( _symbol );
  if( operation ) {
    /* Symbol points to an operation */
    extend_oplist( _oplist, operation );
  } else {
    Group_t* group = Scanner::isa().group( _symbol );
    if( group ) {
      /* Symbol points to a group */
      for( Vect_t<Operation_t>::iterator gop = group->m_operations.begin(); gop < group->m_operations.end(); ++ gop )
        extend_oplist( _oplist, *gop );
    } else {
      Scanner::fileloc.err( "error: undefined operation `%s'", _symbol.str() );
      return false;
    }
  }
  return true;
}

%}

/*
 * control characters
 */
%token '*'
%token ':'
%token ','
%token '='
%token '-'
%token '.'
%token '('
%token ')'
%token '['
%token ']'
%token '<'
%token '>'
%token TOK_ENDL
%token TOK_TAB
%token TOK_QUAD_DOT
/*
 * keywords
 */
%token TOK_ACTION
%token TOK_CONST
%token TOK_CONSTRUCTOR
%token TOK_DECL
%token TOK_DECODER
%token TOK_DESTRUCTOR
%token TOK_GROUP
%token TOK_IMPL
%token TOK_INCLUDE
%token TOK_INHERITANCE
%token TOK_NAMESPACE
%token TOK_OP
%token TOK_REWIND
%token TOK_SET
%token TOK_SEXT
%token TOK_SHL
%token TOK_SHR
%token TOK_SPECIALIZE
%token TOK_STATIC
%token TOK_SUBDECODER
%token TOK_TEMPLATE
%token TOK_VAR
/*
 * basic types
 */
%token TOK_SOURCE_CODE
%token TOK_IDENT
%token TOK_INTEGER
%token TOK_STRING
/*
 * complex types
 */
%type<sourcecode> TOK_SOURCE_CODE
%type<persistent_string> TOK_IDENT
%type<uinteger> TOK_INTEGER
%type<volatile_string> TOK_STRING
%type<sourcecode> sourcecode_decl_declaration
%type<sourcecode> sourcecode_impl_declaration
%type<operation> operation_declaration
%type<bitfield_list> bitfield_list
%type<bitfield> bitfield
%type<bitfield_list> bitfield_list_decl
%type<actionproto> action_proto_declaration
%type<sourcecode> sourcecode_declaration
%type<sinteger> action_proto_type
%type<sourcecode> op_condition
%type<param_list> param_list
%type<param> param
%type<inheritance> global_inheritance_declaration
%type<boolean> sext
%type<boolean> constness
%type<uinteger> size_modifier
%type<sinteger> shift
%type<variable> var
%type<variable_list> var_list
%type<sourcecode> returns
%type<variable_list> global_var_list_declaration
%type<group> group_declaration
%type<operation_list> operation_list
%type<sourcecode> var_init
%type<param_list> template_declaration
%type<sourcecode> template_scheme
%type<constraint> constraint
%type<constraint_list> constraint_list
%type<specialization> specialization
%type<string_list> namespace_list
%%

input: declaration_list { };

declaration_list: | declaration_list declaration {}
;

namespace_list:
      TOK_IDENT
{
  $$ = new StringVect_t( $1 );
}
  | namespace_list TOK_QUAD_DOT TOK_IDENT
{
  $$ = $1->append( $3 );
}
;

subdecoder_instance:
  TOK_SUBDECODER namespace_list TOK_IDENT template_scheme
{
  StringVect_t* nmspc_in = $2;
  ConstStr_t symbol = ConstStr_t( $3, Scanner::symbols );
  SourceCode_t* template_scheme = $4;
  
  std::vector<ConstStr_t> nmspc( nmspc_in->size(), ConstStr_t() );
  for( intptr_t idx = nmspc_in->size(); (--idx) >= 0; )
    nmspc[idx] = ConstStr_t( (*nmspc_in)[idx], Scanner::symbols );
  delete nmspc_in;
  
  SDClass_t const* sdclass = Scanner::isa().sdclass( nmspc );
  if( not sdclass ) {
    Scanner::fileloc.err( "error: subdecoder has not been declared" );
    YYABORT;
  }
  
  SDInstance_t const* sdinstance = Scanner::isa().sdinstance( symbol );
  if( sdinstance ) {
    Scanner::fileloc.err( "error: subdecoder instance `%s' redefined", symbol.str() );
    sdinstance->m_fileloc.err( "subdecoder instance `%s' previously defined here", symbol.str() );
    YYABORT;
  }
  
  Scanner::isa().m_sdinstances.append( new SDInstance_t( symbol, template_scheme, sdclass, Scanner::fileloc ) );
}
;

template_scheme:
{
  $$ = 0;
}
  | '<' TOK_SOURCE_CODE '>'
{
  $$ = $2;
}
;

subdecoder_class:
  TOK_SUBDECODER namespace_list '[' TOK_INTEGER ':' TOK_INTEGER ']'
{
  StringVect_t* nmspc_in = $2;
  unsigned int minsize = $4, maxsize = $6;

  if( minsize > maxsize ) {
    Scanner::fileloc.err( "error: subdecoder operation range is reversed" );
    YYABORT;
  }
  
  std::vector<ConstStr_t> nmspc( nmspc_in->size(), ConstStr_t() );
  for( intptr_t idx = nmspc_in->size(); (--idx) >= 0; )
    nmspc[idx] = ConstStr_t( (*nmspc_in)[idx], Scanner::symbols );
  delete nmspc_in;
  
  SDClass_t const* sdclass = Scanner::isa().sdclass( nmspc );
  
  if( sdclass ) {
    Scanner::fileloc.err( "error: subdecoder class redeclared." );
    sdclass->m_fileloc.err( "subdecoder class previously declared here." );
    YYABORT;
  }
  
  Scanner::isa().m_sdclasses.append( new SDClass_t( nmspc, minsize, maxsize, Scanner::fileloc ) );
}
;

global_ident_parameter: TOK_SET TOK_IDENT TOK_IDENT
{
  Scanner::isa().setparam( ConstStr_t( $2, Scanner::symbols ), ConstStr_t( $3, Scanner::symbols ) );
}

global_sourcecode_parameter: TOK_SET TOK_IDENT TOK_SOURCE_CODE
{
  Scanner::isa().setparam( ConstStr_t( $2, Scanner::symbols ), $3 );
}

template_declaration: TOK_TEMPLATE '<' param_list '>'
{
  $$ = $3;
}
;

declaration:
             TOK_ENDL {}
           | global_ident_parameter TOK_ENDL
           | global_sourcecode_parameter TOK_ENDL
           | subdecoder_class TOK_ENDL
           | subdecoder_instance TOK_ENDL
           | operation_declaration TOK_ENDL
{
  Scanner::isa().m_operations.append( $1 );
}
           | action_proto_declaration TOK_ENDL
{
  Scanner::isa().m_actionprotos.push_back( $1 );
}
     | action_declaration TOK_ENDL
{
}
           | sourcecode_declaration TOK_ENDL
{
  Scanner::isa().m_decl_srccodes.push_back( $1 );
}
  | sourcecode_decl_declaration TOK_ENDL
{
  Scanner::isa().m_decl_srccodes.push_back( $1 );
}
  | sourcecode_impl_declaration TOK_ENDL
{
  Scanner::isa().m_impl_srccodes.push_back( $1 );
}
     | include {}
     | global_var_list_declaration
{
  Scanner::isa().m_vars.append( *$1 );
  delete $1;
}
     | op_var_list_declaration {}
     | global_inheritance_declaration TOK_ENDL
{
  Scanner::isa().m_inheritances.push_back( $1 );
}
  | group_declaration
{
  Scanner::isa().m_groups.push_back( $1 );
}
  | namespace_declaration
{
}
  | template_declaration
{
  if( not Scanner::isa().m_tparams.empty() ) {
    Scanner::fileloc.err( "error: template parameters defined more than once" );
    YYABORT;
  }
  Scanner::isa().m_tparams = *$1;
  delete $1;
}
  | specialization TOK_ENDL
{
  Scanner::isa().m_specializations.push_back( $1 );
}
;

namespace_declaration: TOK_NAMESPACE namespace_list
{
  StringVect_t* nmspc = $2;
  
  for( StringVect_t::const_iterator ident = nmspc->begin(); ident != nmspc->end(); ++ident ) {
    Scanner::isa().m_namespace.push_back( ConstStr_t( *ident, Scanner::symbols ) );
  }
  
  delete nmspc;
}
;

sourcecode_decl_declaration: TOK_DECL TOK_SOURCE_CODE
{
  $$ = $2;
}
;

sourcecode_impl_declaration: TOK_IMPL TOK_SOURCE_CODE
{
  $$ = $2;
}
;

sourcecode_declaration: TOK_SOURCE_CODE
{
  $$ = $1;
}
;

bitfield_list_decl: '(' bitfield_list ')'
{
  $$ = $2;
}
  |
{
  $$ = 0;
}
;

op_condition: TOK_SOURCE_CODE ':'
{
  $$ = $1;
}
  |
{
  $$ = 0;
}
;

operation_declaration : op_condition TOK_OP TOK_IDENT bitfield_list_decl
{
  SourceCode_t*       op_cond = $1;
  ConstStr_t          symbol = ConstStr_t( $3, Scanner::symbols );
  Vect_t<BitField_t>* bitfields = $4;
  
  {
    Operation_t const* prev_op = Scanner::isa().operation( symbol );
    if( prev_op ) {
      Scanner::fileloc.err( "error: operation `%s' redefined", symbol.str() );
      prev_op->m_fileloc.err( "operation `%s' previously defined here", symbol.str() );
      YYABORT;
    }
    
    Group_t const* prev_grp = Scanner::isa().group( symbol );
    if( prev_grp ) {
      Scanner::fileloc.err( "error: operation `%s' redefined", symbol.str() );
      prev_grp->m_fileloc.err( "group `%s' previously defined here", symbol.str() );
    }
  }

  Operation_t* operation = new Operation_t( symbol, *bitfields, Scanner::comments, op_cond, Scanner::fileloc );
  delete bitfields;
  Scanner::comments.clear();

  $$ = operation;
}
;

bitfield_list : bitfield
{
  $$ = new Vect_t<BitField_t>( $1 );
}
  | bitfield_list ':' bitfield
{
  $$ = $1->append( $3 );
}
;

bitfield: TOK_INTEGER '[' TOK_INTEGER ']'
{
  $$ = new OpcodeBitField_t( $3, $1 );
}
  | shift sext size_modifier TOK_IDENT '[' TOK_INTEGER ']'
{
  $$ = new OperandBitField_t( $6, ConstStr_t( $4, Scanner::symbols ), $1, $3, $2 );
}
  | '?' '[' TOK_INTEGER ']'
{
  $$ = new UnusedBitField_t( $3 );
}
  | '*' TOK_IDENT '[' TOK_IDENT ']'
{
  ConstStr_t symbol = ConstStr_t( $2, Scanner::symbols );
  ConstStr_t sdinstance_symbol = ConstStr_t( $4, Scanner::symbols );
  SDInstance_t const* sdinstance = Scanner::isa().sdinstance( sdinstance_symbol );
  
  if( not sdinstance ) {
    Scanner::fileloc.err( "error: subdecoder instance `%s' not declared", sdinstance_symbol.str() );
    YYABORT;
  }
  
  $$ = new SubOpBitField_t( symbol, sdinstance );
}
  | '>' '<'
{
  $$ = new SeparatorBitField_t( false );
}
  | '>' TOK_REWIND '<'
{
  $$ = new SeparatorBitField_t( true );
}
;

shift:
{
  $$ = 0;
}
  | TOK_SHR '<' TOK_INTEGER '>'
{
  $$ = $3;
}
  | TOK_SHL '<' TOK_INTEGER '>'
{
  $$ = -$3;
}
;

sext:
{
  $$ = false;
}
  | TOK_SEXT
{
  $$ = true;
}
;

size_modifier:
{
  $$ = 0;
}
  | '<' TOK_INTEGER '>'
{
  $$ = $2;
}
  | '<' '>'
{
  $$ = 0;
}
;

global_inheritance_declaration:
TOK_INHERITANCE TOK_SOURCE_CODE TOK_SOURCE_CODE '=' TOK_SOURCE_CODE
{
  $$ = new Inheritance_t( $2, $3, $5 );
}
| TOK_INHERITANCE TOK_SOURCE_CODE TOK_SOURCE_CODE
{
  $$ = new Inheritance_t( $2, $3, 0 );
}
;

op_var_list_declaration: TOK_IDENT '.' TOK_VAR var_list
{
  ConstStr_t    target_symbol = ConstStr_t( $1, Scanner::symbols );
  Vect_t<Variable_t>* var_list = $4;
  
  /* Target symbol points to either an operation or a group */
  Operation_t* operation = Scanner::isa().operation( target_symbol );
  if( operation ) {
    /* Target symbol points to an operation */
    operation->m_variables.append( *var_list );
  } else {
    Group_t* group = Scanner::isa().group( target_symbol );
    if( group ) {
      /* Target symbol points to a group */
      for( Vect_t<Operation_t>::iterator gop = group->m_operations.begin(); gop < group->m_operations.end(); ++ gop )
        (**gop).m_variables.append( *var_list );
    } else {
      /* Target symbol doesn't point to anything */
      Scanner::fileloc.err( "error: undefined operation or group `%s'", target_symbol.str() );
      YYABORT;
    }
  }
  delete var_list;
}
;

global_var_list_declaration: TOK_VAR var_list
{
  $$ = $2;
}
;

var_list: var
{
  $$ = new Vect_t<Variable_t>( $1 );
}
  | var_list ',' var
{
  $$ = $1->append( $3 );
}
;

var_init:
{
  $$ = 0;
}
  | '=' TOK_SOURCE_CODE
{
  $$ = $2;
}
;

var: TOK_IDENT ':' TOK_SOURCE_CODE var_init
{
  ConstStr_t     symbol = ConstStr_t( $1, Scanner::symbols );
  SourceCode_t*  c_type = $3;
  SourceCode_t*  c_init = $4;
  
  $$ = new Variable_t( symbol, c_type, c_init );
}
;

action_proto_declaration: action_proto_type TOK_ACTION returns TOK_IDENT '(' param_list ')' constness TOK_SOURCE_CODE
{
  ActionProto_t::type_t action_proto_type = ActionProto_t::type_t( $1 );
  SourceCode_t*         returns = $3;
  ConstStr_t            symbol = ConstStr_t( $4, Scanner::symbols );
  Vect_t<CodePair_t>*   param_list = $6;
  SourceCode_t*         default_sourcecode = $9;

  { /* action protype name should be unique */
    ActionProto_t const*  prev_proto = Scanner::isa().actionproto( symbol );
    if( prev_proto ) {
      Scanner::fileloc.err( "error: action prototype `%s' redefined", prev_proto->m_symbol.str() );
      prev_proto->m_fileloc.err( "action prototype `%s' previously defined here", prev_proto->m_symbol.str() );
      YYABORT;
    }
  }
    
  if( returns ) {
    switch( action_proto_type ) {
    case ActionProto_t::Constructor:
      Scanner::fileloc.err( "error: constructor action prototype `%s' must not have a return type (%s)",
                symbol.str(), returns->m_content.str() );
      YYABORT;
      break;
    case ActionProto_t::Destructor:
      Scanner::fileloc.err( "error: destructor action prototype `%s' must not have a return type (%s)",
                symbol.str(), returns->m_content.str() );
      YYABORT;
      break;
    default: break;
    }
  }
  
  if( param_list )
  {
    switch( action_proto_type ) {
    case ActionProto_t::Constructor:
      Scanner::fileloc.err( "error: constructor action prototype `%s' must not take any arguments", symbol.str() );
      YYABORT;
      break;
    case ActionProto_t::Static:
      Scanner::fileloc.err( "error: static action prototype `%s' must not take any arguments", symbol.str() );
      YYABORT;
      break;
    case ActionProto_t::Destructor:
      Scanner::fileloc.err( "error: destructor action prototype `%s' must not take any arguments", symbol.str() );
      YYABORT;
      break;
    default: break;
    }
  }
  
  ActionProto_t* actionproto =
    new ActionProto_t( action_proto_type, symbol, returns, *param_list, $8, default_sourcecode, Scanner::comments, Scanner::fileloc );
  Scanner::comments.clear();
  delete param_list;
  $$ = actionproto;
}
;

returns:
{
  $$ = 0;
}
  | TOK_SOURCE_CODE
{
  $$ = $1;
}
;

constness:
{
  $$ = false;
}
| TOK_CONST
{
  $$ = true;
}
;

param: TOK_SOURCE_CODE TOK_SOURCE_CODE
{
  SourceCode_t *c_type = $1;
  SourceCode_t *c_symbol = $2;
  $$ = new CodePair_t( c_type, c_symbol );
}
;

param_list:
{
  $$ = 0;
}
  | param
{
  $$ = new Vect_t<CodePair_t>( $1 );
}
  | param_list ',' param
{
  $$ = $1->append( $3 );
}
;

action_proto_type:
{
  $$ = ActionProto_t::Common;
}
  | TOK_CONSTRUCTOR
{
  $$ = ActionProto_t::Constructor;
}
  | TOK_DESTRUCTOR
{
  $$ = ActionProto_t::Destructor;
}
  | TOK_STATIC
{
  $$ = ActionProto_t::Static;
}
;

action_declaration: TOK_IDENT '.' TOK_IDENT '=' TOK_SOURCE_CODE
{
  ConstStr_t    target_symbol = ConstStr_t( $1, Scanner::symbols );
  ConstStr_t    action_proto_symbol = ConstStr_t( $3, Scanner::symbols );
  SourceCode_t* actioncode = $5;
  
  /* Actions belongs to an action prototype */
  ActionProto_t const* actionproto = Scanner::isa().actionproto( action_proto_symbol );

  if( not actionproto ) {
    Scanner::fileloc.err( "error: undefined action prototype `%s'", action_proto_symbol.str() );
    YYABORT;
  }
  
  /* Target symbol points to either an operation or a group */
  Operation_t* operation = Scanner::isa().operation( target_symbol );
  if( operation ) {
    /* Target symbol points to an operation */
    create_action( operation, actionproto, actioncode );
  } else {
    Group_t* group = Scanner::isa().group( target_symbol );
    if( group ) {
      /* Target symbol points to a group */
      for( Vect_t<Operation_t>::iterator gop = group->m_operations.begin(); gop < group->m_operations.end(); ++ gop )
        create_action( *gop, actionproto, actioncode );
    } else {
      /* Target symbol doesn't point to anything */
      Scanner::fileloc.err( "error: undefined operation or group `%s'", target_symbol.str() );
      YYABORT;
    }
  }
  
  Scanner::comments.clear();
}
;

specialization: TOK_SPECIALIZE TOK_IDENT '(' constraint_list ')'
{
  ConstStr_t            symbol = ConstStr_t( $2, Scanner::symbols );
  Vect_t<Constraint_t>* constraint_list = $4;
  Operation_t*          operation = Scanner::isa().operation( symbol );
  if( not operation ) {
    Scanner::fileloc.err( "error: operation `%s' not defined", symbol.str() );
    YYABORT;
  }
  
  $$ = new Specialization_t( operation, *constraint_list );
  delete constraint_list;
}
;

constraint_list:
  constraint
{
  $$ = new Vect_t<Constraint_t>( $1 );
}
  | constraint_list ',' constraint
{
  $$ = $1->append( $3 );
}
;

constraint: TOK_IDENT '=' TOK_INTEGER
{
  $$ = new Constraint_t( ConstStr_t( $1, Scanner::symbols ), $3 );
}
;

include : TOK_INCLUDE TOK_STRING
{
  if( not Scanner::include( $2 ) )
    YYABORT;
}
;

group_declaration: TOK_GROUP TOK_IDENT '(' operation_list ')'
{
  ConstStr_t           group_symbol = ConstStr_t( $2, Scanner::symbols );
  Vect_t<Operation_t>* operation_list = $4;
  
  { /* Operations and groups name should not conflict */
    Operation_t* prev_op = Scanner::isa().operation( group_symbol );
    if( prev_op ) {
      Scanner::fileloc.err( "error: group name conflicts with operation `%s'", group_symbol.str() );
      prev_op->m_fileloc.err( "operation `%s' previously defined here", group_symbol.str() );
      YYABORT;
    }
    
    Group_t* prev_grp = Scanner::isa().group( group_symbol );
    if( prev_grp ) {
      Scanner::fileloc.err( "error: group `%s' redefined", group_symbol.str() );
      prev_grp->m_fileloc.err( "group `%s' previously defined here", group_symbol.str() );
      YYABORT;
    }
  }
  
  $$ = new Group_t( group_symbol, *operation_list, Scanner::fileloc );
  delete operation_list;
}
;

operation_list:
  TOK_IDENT
{
  ConstStr_t symbol = ConstStr_t( $1, Scanner::symbols );
  Vect_t<Operation_t>* oplist = new Vect_t<Operation_t>;
  if( not extend_oplist( oplist, symbol ) ) { YYABORT; }
  $$ = oplist;
}
  | operation_list ',' TOK_IDENT
{
  Vect_t<Operation_t>* oplist = $1;
  ConstStr_t symbol = ConstStr_t( $3, Scanner::symbols );
  if( not extend_oplist( oplist, symbol ) ) { YYABORT; }
  $$ = oplist;
}
;

%%

int yyerror( char const* _err ) { Scanner::fileloc.err( "%s! unexpected %s\n", _err, Scanner::tokenname( yychar ).str() ); return 1; }
