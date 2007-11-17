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
#include <variable.hh>
#include <bitfield.hh>
#include <scanner.hh>
#include <conststr.hh>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <vector>
  
extern int yylex();

static
void
create_action( Operation_t* _operation, ActionProto_t const* _actionproto, SourceCode_t* _actioncode ) {
  Action_t const* prev_action = _operation->action( _actionproto );

  if( prev_action ) {
    // FIXME: this was downgraded from warning to error
    yyerrorf( Scanner::filename, Scanner::lineno, "error: action `%s.%s' redefined",
              _operation->m_symbol.str(), _actionproto->m_symbol.str() );
    yyerrorf( prev_action->m_filename, prev_action->m_lineno, "action `%s.%s' previously defined here",
              _operation->m_symbol.str(), _actionproto->m_symbol.str() );
    
    exit( -1 );
  }
  
  _operation->add( new Action_t( _actionproto, _actioncode, Scanner::comments, Scanner::filename, Scanner::lineno ) );
}

%}

%token TOK_NAMESPACE
%token TOK_ADDRESS
%token TOK_DECL
%token TOK_IMPL
%token TOK_GROUP
%token TOK_STATIC
%token TOK_CONSTRUCTOR
%token TOK_DESTRUCTOR
%token TOK_OP
%token TOK_SOURCE_CODE
%token '*'
%token '('
%token ')'
%token ':'
%token ','
%token TOK_ACTION
%token '='
%token TOK_IDENT
%token TOK_INTEGER
%token TOK_ENDL
%token TOK_TAB
%token TOK_STRING
%token '.'
%token '['
%token ']'
%token '<'
%token '>'
%token TOK_QUAD_DOT
%token TOK_INCLUDE
%token TOK_SEXT
%token TOK_SHL
%token TOK_SHR
%token TOK_VAR
%token TOK_BIG_ENDIAN
%token TOK_LITTLE_ENDIAN
%token TOK_TEMPLATE
%type<sourcecode> address_declaration
%type<sourcecode> sourcecode_decl_declaration
%type<sourcecode> sourcecode_impl_declaration
%type<persistent_string> TOK_IDENT
%type<sourcecode> TOK_SOURCE_CODE
%type<uinteger> TOK_INTEGER
%type<volatile_string> TOK_STRING
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
%type<boolean> sext
%type<uinteger> size_modifier
%type<sinteger> shift
%type<variable> var
%type<variable_list> var_list
%type<variable_list> var_list_declaration
%type<sourcecode> returns
%type<variable_list> global_var_list_declaration
%type<group> group_declaration
%type<operation_list> operation_list
%type<operation> operation
%type<sourcecode> var_init
%type<param_list> template_declaration

%%

input: declaration_list { }

declaration_list: | declaration_list declaration {}
;

namespace_list:
      TOK_IDENT
{
  Scanner::isa().m_namespace.push_back( ConstStr_t( $1, Scanner::symbols ) );
}
  | namespace_list TOK_QUAD_DOT TOK_IDENT
{
  Scanner::isa().m_namespace.push_back( ConstStr_t( $3, Scanner::symbols ) );
}
;

endian: TOK_LITTLE_ENDIAN
{
  Scanner::isa().m_little_endian = true;
}
       | TOK_BIG_ENDIAN
{
  Scanner::isa().m_little_endian = false;
}
;

template_declaration: TOK_TEMPLATE '<' param_list '>'
{
  $$ = $3;
}
;

declaration:
             TOK_ENDL {}
           | endian TOK_ENDL
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
  | group_declaration
{
  Scanner::isa().m_groups.push_back( $1 );
}
  | namespace_declaration
{
}
  | address_declaration
{
  Scanner::isa().m_addrtype = $1->m_content;
  delete $1;
}
  | template_declaration
{
  if( not Scanner::isa().m_tparams.empty() ) {
    yyerrorf( Scanner::filename, Scanner::lineno, "error: template parameters defined more than once" );
    YYABORT;
  }
  Scanner::isa().m_tparams = *$1;
  delete $1;
}
;

namespace_declaration: TOK_NAMESPACE namespace_list
{
}
;

address_declaration: TOK_ADDRESS TOK_SOURCE_CODE
{
  $$ = $2;
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

operation_declaration : op_condition TOK_OP TOK_IDENT bitfield_list_decl var_list_declaration
{
  SourceCode_t*       op_cond = $1;
  ConstStr_t          symbol = ConstStr_t( $3, Scanner::symbols );
  Vect_t<BitField_t>* bitfields = $4;
  Vect_t<Variable_t>* vars = $5;
  
  {
    Operation_t const* prev_op = Scanner::isa().operation( symbol );
    if( prev_op ) {
      // FIXME: warning downgraded to error
      yyerrorf( Scanner::filename, Scanner::lineno, "error: operation `%s' redefined", prev_op->m_symbol.str() );
      yyerrorf( prev_op->m_filename, prev_op->m_lineno, "operation `%s' previously defined here", prev_op->m_symbol.str() );
      YYABORT;
    }
  }

  Operation_t* operation = new Operation_t( symbol, *bitfields, Scanner::comments, *vars, op_cond, Scanner::filename, Scanner::lineno );
  delete bitfields;
  delete vars;
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
  if( $3->symbol() ) {
    Vect_t<BitField_t>::const_iterator node;
    for( node = $1->begin(); node < $1->end(); ++ node )
      if( (*node)->symbol() == $3->symbol() ) break;
    if( node < $1->end() ) { // found one
      yyerrorf( Scanner::filename, Scanner::lineno, "error: duplicated bit field `%s'", $3->symbol().str() );
      YYABORT;
    }
  }
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

var_list_declaration:
{
  $$ = 0;
}
  | TOK_VAR var_list
{
  $$ = $2;
}

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

action_proto_declaration: action_proto_type TOK_ACTION returns TOK_IDENT '(' param_list ')' TOK_SOURCE_CODE
{
  ActionProto_t::type_t action_proto_type = ActionProto_t::type_t( $1 );
  SourceCode_t*         returns = $3;
  ConstStr_t            symbol = ConstStr_t( $4, Scanner::symbols );
  Vect_t<CodePair_t>*   param_list = $6;
  SourceCode_t*         default_sourcecode = $8;

  { /* action protype name should be unique */
    ActionProto_t const*  prev_proto = Scanner::isa().actionproto( symbol );
    if( prev_proto ) {
      // FIXME: downgraded warning to error
      yyerrorf( Scanner::filename, Scanner::lineno, "error: action prototype `%s' redefined", prev_proto->m_symbol.str() );
      yyerrorf( prev_proto->m_filename, prev_proto->m_lineno, "action prototype `%s' previously defined here", prev_proto->m_symbol.str() );
      YYABORT;
    }
  }
    
  if( returns ) {
    switch( action_proto_type ) {
    case ActionProto_t::Constructor:
      yyerrorf( Scanner::filename, Scanner::lineno, "error: constructor action prototype `%s' must not have a return type (%s)",
                symbol.str(), returns->m_content.str() );
      YYABORT;
      break;
    case ActionProto_t::Destructor:
      yyerrorf( Scanner::filename, Scanner::lineno, "error: destructor action prototype `%s' must not have a return type (%s)",
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
      yyerrorf( Scanner::filename, Scanner::lineno, "error: constructor action prototype `%s' must not take any arguments", symbol.str() );
      YYABORT;
      break;
    case ActionProto_t::Static:
      yyerrorf( Scanner::filename, Scanner::lineno, "error: static action prototype `%s' must not take any arguments", symbol.str() );
      YYABORT;
      break;
    case ActionProto_t::Destructor:
      yyerrorf( Scanner::filename, Scanner::lineno, "error: destructor action prototype `%s' must not take any arguments", symbol.str() );
      YYABORT;
      break;
    default: break;
    }
  }
  
  ActionProto_t* actionproto =
    new ActionProto_t( action_proto_type, symbol, returns, *param_list, default_sourcecode, Scanner::comments, Scanner::filename, Scanner::lineno );
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
    yyerrorf( Scanner::filename, Scanner::lineno, "error: undefined action prototype `%s'", action_proto_symbol.str() );
    YYABORT;
  }

  /* target symbol is either an operation or a group */
  Operation_t *operation = Scanner::isa().operation( target_symbol );
  Group_t *group = operation ? 0 : Scanner::isa().group( target_symbol );

  if( not operation and not group ) {
    yyerrorf( Scanner::filename, Scanner::lineno, "error: undefined operation or group `%s'", target_symbol.str() );
    YYABORT;
  }
  else if( operation and group ) {
    yyerrorf( Scanner::filename, Scanner::lineno, "internal error: operation and group `%s' are both defined", target_symbol.str() );
    YYABORT;
  }
  else if( group ) {
    for( Vect_t<Operation_t>::iterator gop = group->m_operations.begin(); gop < group->m_operations.end(); ++ gop )
      create_action( *gop, actionproto, actioncode );
  }
  else /* operation */ {
    create_action( operation, actionproto, actioncode );
  }
  
  Scanner::comments.clear();
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
  
  { /* Actions and group name should not conflict */
    Operation_t* prev_op = Scanner::isa().operation( group_symbol );
    if( prev_op ) {
      yyerrorf( Scanner::filename, Scanner::lineno, "error: group name conflicts with operation `%s'", group_symbol.str() );
      yyerrorf( prev_op->m_filename, prev_op->m_lineno, "operation `%s' previously defined here", group_symbol.str() );
      YYABORT;
    }
    
    Group_t* prev_grp = Scanner::isa().group( group_symbol );
    if( prev_grp ) {
      yyerrorf( Scanner::filename, Scanner::lineno, "error: group `%s' redefined", group_symbol.str() );
      yyerrorf( prev_grp->m_filename, prev_grp->m_lineno, "group `%s' previously defined here", group_symbol.str() );
      YYABORT;
    }
  }
  
  $$ = new Group_t( group_symbol, *operation_list, Scanner::filename, Scanner::lineno );
  delete operation_list;
}
;

operation_list:
  operation
{
  $$ = new Vect_t<Operation_t>( $1 );
}
  | operation_list ',' operation
{
  Vect_t<Operation_t>* oplist = $1;
  Operation_t*         opitem = $3;
  
  // Suppressing duplications
  Vect_t<Operation_t>::const_iterator node;
  for( node = oplist->begin(); node < oplist->end(); ++ node )
    if( (*node)->m_symbol == opitem->m_symbol ) break;
  
  if( node < oplist->end() ) { // found one
    yyerrorf( Scanner::filename, Scanner::lineno, "warning: duplicated operation `%s' in list", opitem->m_symbol.str() );
    $$ = oplist;
  }
  else {
    $$ = oplist->append( opitem );
  }
}
;

operation:
  TOK_IDENT
{
  ConstStr_t op_symbol = ConstStr_t( $1, Scanner::symbols );
  
  Operation_t *operation = Scanner::isa().operation( op_symbol );

  if( not operation ) {
    yyerrorf( Scanner::filename, Scanner::lineno, "error: undefined operation `%s'", op_symbol.str() );
    YYABORT;
  }
  
  $$ = operation;
}
;


%%

int
yypanicf( char const* filename, int lineno, char const* format, ... ) {
  char s[1024];
  va_list args;

  va_start( args, format );
  vsprintf( s, format, args );
  if( Scanner::filename and filename )
    fprintf( stderr, "%s:%d: %s\n", filename, Scanner::lineno, s );
  else
    fprintf( stderr, "%s\n", s );
  if( not Scanner::isa().m_actionprotos.empty() ) {
    ActionProto_t const* last = Scanner::isa().m_actionprotos.back();
    fprintf( stderr, "%s:%d: last action prototype `%s' was declared here\n",
             last->m_filename.str(), last->m_lineno,
             last->m_symbol.str() );
  }
  if( not Scanner::isa().m_operations.empty() ) {
    Operation_t const* last = Scanner::isa().m_operations.back();
    fprintf( stderr, "%s:%d: last action `%s' was declared here\n",
             last->m_filename.str(), last->m_lineno,
             last->m_symbol.str() );
  }
  if( SourceCode_t::s_last_srccode ) {
    SourceCode_t const* last = SourceCode_t::s_last_srccode;
    fprintf( stderr, "%s:%d: last source code was declared here\n", last->m_filename.str(), last->m_lineno );
  }
  
  va_end( args );
  return 0;
}


int
yyerrorf( char const* _filename, int _lineno, char const* _fmt, ... ) {
  va_list args;
  for( intptr_t capacity = 128, size; true; capacity = (size > -1) ? size + 1 : capacity * 2 ) {
    /* stack allocation */
    char storage[capacity];
    /* Try to print in the allocated space. */
    va_start( args, _fmt );
    size = vsnprintf( storage, capacity, _fmt, args );
    va_end( args );
    /* If it didn't work, retry */
    if( size < 0 or size >= capacity ) continue;
    
    /* Now storage is ok... */
    if( _filename )   fprintf( stderr, "%s:%d: %s\n", _filename, _lineno, storage );
    else              fprintf( stderr, "%s\n", storage );
    break;
  }
  
  return 0;
}

int
yyerror( char const* s ) {
  if( Scanner::filename )
    fprintf( stderr, "%s:%d: %s! unexpected %s\n", Scanner::filename.str(), Scanner::lineno, s, Scanner::tokenname( yychar ).str() );
  else
    fprintf( stderr, "%s! unexpected %s\n", s, Scanner::tokenname( yychar ).str() );
  return 1;
}
