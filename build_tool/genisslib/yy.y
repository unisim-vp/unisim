%{
/***************************************************************************
                      parser.y  -  syntaxical analyzer
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
#include <stdio.h>
#include <string.h>
#include <genisslib.h>
#include <stdarg.h>

extern int yylex();
extern int yyerrorf(filename_t *filename, int lineno, char *format, ...);
extern int do_include(char *filename);
extern int yyerror(char *s);

extern int lineno;
extern FILE *yyin;
extern filename_t *current_filename;

static action_t *decl_action(operation_t *operation, action_proto_t *action_proto, source_code_t *source_code, comment_list_t *comment_list, filename_t *current_filename, int lineno)
{
	action_t *prev_action;

	prev_action = search_action(operation, action_proto);

	if(prev_action)
	{
		yyerrorf(current_filename, lineno, "warning! action `%s.%s' redefined", build_hierarchical_name(prev_action->operation->symbol_hierarchy), prev_action->action_proto->symbol_hierarchy->symbol->name);
		yyerrorf(prev_action->filename, prev_action->lineno, "action `%s.%s' previously defined here", build_hierarchical_name(prev_action->operation->symbol_hierarchy), prev_action->action_proto->symbol_hierarchy->symbol->name);
		remove_action(prev_action->operation, prev_action);
		delete_action(prev_action);
	}

	return create_action(operation, action_proto, source_code, comment_list, current_filename, lineno);
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
%type<symbol_hierarchy> namespace_declaration
%type<symbol_hierarchy> symbol_hierarchy
%type<symbol_hierarchy> namespace_symbol_hierarchy
%type<source_code> address_declaration
%type<source_code> source_code_decl_declaration
%type<source_code> source_code_impl_declaration
%type<symbol> TOK_IDENT
%type<source_code> TOK_SOURCE_CODE
%type<integer> TOK_INTEGER
%type<string> TOK_STRING
%type<operation> operation_declaration
%type<bitfield_list> bitfield_list
%type<bitfield> bitfield
%type<bitfield_list> bitfield_list_decl
%type<action_proto> action_proto_declaration
%type<source_code> source_code_declaration
%type<action_proto_type> action_proto_type
%type<source_code> op_condition
%type<param_list> param_list
%type<param> param
%type<sext> sext
%type<size_modifier> size_modifier
%type<shift> shift
%type<variable> var
%type<variable_list> var_list
%type<variable_list> var_list_declaration
%type<source_code> returns
%type<variable_list> global_var_list_declaration
%type<group> group_declaration
%type<operation_node_list> operation_list
%type<operation_node> operation
%type<source_code> var_init
%type<template_parameter_list> template_declaration
%type<template_parameter_list> template_parameter_list
%type<template_parameter> template_parameter

%%

input: declaration_list { }

declaration_list: | declaration_list declaration {}
;

namespace_symbol_hierarchy :
      TOK_IDENT
{
	$$ = get_symbol_hierarchy(0, $1);
}
	| namespace_symbol_hierarchy TOK_QUAD_DOT TOK_IDENT
{
	$$ = get_symbol_hierarchy2($1, $3);
}
;

symbol_hierarchy :
      TOK_IDENT
{
	$$ = get_symbol_hierarchy(namespace, $1);
}
	| symbol_hierarchy TOK_QUAD_DOT TOK_IDENT
{
	$$ = get_symbol_hierarchy2($1, $3);
}
;

endian: TOK_LITTLE_ENDIAN
{
	little_endian = 1;
}
       | TOK_BIG_ENDIAN
{
	little_endian = 0;
}
;

template_declaration: TOK_TEMPLATE '<' template_parameter_list '>'
{
	$$ = $3;
}
;

template_parameter: TOK_SOURCE_CODE TOK_SOURCE_CODE
{
	$$ = create_template_parameter($1, $2);
}
; 

template_parameter_list: template_parameter
{
	$$ = create_template_parameter_list($1);
}
	| template_parameter_list ',' template_parameter
{
	$$ = add_template_parameter($1, $3);
}
;

declaration:
             TOK_ENDL {}
           | endian TOK_ENDL
           | operation_declaration TOK_ENDL
{
	operation_t *operation = $1;
	add_operation_global(operation);
}
           | action_proto_declaration TOK_ENDL
{
	action_proto_t *action_proto = $1;
	add_action_proto(action_proto);
}
	   | action_declaration TOK_ENDL
{
}
           | source_code_declaration TOK_ENDL
{
	source_code_t *source_code = $1;
	source_code->global = sc_decl;
}
	| source_code_decl_declaration TOK_ENDL
{
	source_code_t *source_code = $1;
	source_code->global = sc_decl;
}
	| source_code_impl_declaration TOK_ENDL
{
	source_code_t *source_code = $1;
	source_code->global = sc_impl;
}
	   | include {}
	   | global_var_list_declaration
{
	add_variable_list($1);
}
	| group_declaration
{
	add_group($1);
}
	| namespace_declaration
{
	namespace = $1;
}
	| address_declaration
{
	address_type = $1;
}
	| template_declaration
{
	template_parameters = $1;
}
;

namespace_declaration: TOK_NAMESPACE namespace_symbol_hierarchy
{
	$$ = $2;
}
;

address_declaration: TOK_ADDRESS TOK_SOURCE_CODE
{
	$$ = $2;
}
;

source_code_decl_declaration: TOK_DECL TOK_SOURCE_CODE
{
	$$ = $2;
}
;

source_code_impl_declaration: TOK_IMPL TOK_SOURCE_CODE
{
	$$ = $2;
}
;

source_code_declaration: TOK_SOURCE_CODE
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

operation_declaration : op_condition TOK_OP symbol_hierarchy bitfield_list_decl var_list_declaration
{
	source_code_t *op_condition = $1;
	symbol_hierarchy_t *symbol_hierarchy = $3;
	bitfield_list_t *bitfield_list = $4;
	operation_t *prev_operation = search_operation_global(symbol_hierarchy);
	variable_list_t *var_list = $5;
	operation_t *operation;
	operation_t *base_operation = 0;

	if(prev_operation)
	{
		yyerrorf(current_filename, lineno, "warning! operation `%s' redefined", build_hierarchical_name(prev_operation->symbol_hierarchy));
		yyerrorf(prev_operation->filename, prev_operation->lineno, "operation `%s' previously defined here", build_hierarchical_name(prev_operation->symbol_hierarchy));
		remove_operation(prev_operation);
		delete_operation(prev_operation);
	}

	base_operation = search_base_operation(symbol_hierarchy->parent, symbol_hierarchy->symbol);

	if(!bitfield_list && !base_operation)
	{
		yyerrorf(current_filename, lineno, "error! operation `%s' neither has a bit field list, nor a base operation", build_hierarchical_name(symbol_hierarchy));
		YYABORT;
	}

	operation = create_operation(symbol_hierarchy, base_operation ? dup_bitfield_list(base_operation->bitfield_list) : bitfield_list, comment_list, var_list, base_operation, op_condition, current_filename, lineno);

	prev_operation = search_operation_opcode(operation->opcode_mask, operation->opcode, operation->symbol_hierarchy);

	if(prev_operation)
	{
		char *previous_operation_name = strdup(build_hierarchical_name(operation->symbol_hierarchy));
		yyerrorf(current_filename, lineno, "warning! operation `%s' conflicts with operation `%s'", previous_operation_name, build_hierarchical_name(prev_operation->symbol_hierarchy));
		free(previous_operation_name);
		yyerrorf(prev_operation->filename, prev_operation->lineno, "operation `%s' was declared here", build_hierarchical_name(prev_operation->symbol_hierarchy));
	}

	$$ = operation;

	if(comment_list) comment_list = 0;
}
;

bitfield_list : bitfield
{
	$$ = create_bitfield_list($1);
}
           | bitfield_list ':' bitfield
{
	bitfield_list_t *bitfield_list = $1;
	bitfield_t *bitfield = $3;

	if(bitfield->type == bf_operand)
	{
		bitfield_t *prev_bitfield = search_operand_bitfield(bitfield_list, bitfield->u.operand.symbol);

		if(prev_bitfield)
		{
			yyerrorf(current_filename, lineno, "error! duplicated bit field `%s'", bitfield->u.operand.symbol->name);
			YYABORT;
		}
	}
	$$ = add_bitfield($1, $3);
}
;

bitfield: TOK_INTEGER '[' TOK_INTEGER ']'
{
	unsigned int value = $1;
	unsigned int size = $3;
	$$ = create_opcode_bitfield(size, value);
}
        | shift sext size_modifier TOK_IDENT '[' TOK_INTEGER ']'
{
	symbol_t *symbol = $4;
	unsigned int size = $6;
	int shift = $1;
	int sext = $2;
	unsigned int size_modifier = $3;
	$$ = create_operand_bitfield(size, symbol, shift, sext, size_modifier);
}
        | '?' '[' TOK_INTEGER ']'
{
	unsigned int size = $3;
	$$ = create_dontcare_bitfield(size);
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
	$$ = 0;
}
	| TOK_SEXT
{
	$$ = 1;
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
	$$ = create_variable_list($1);
}
	| var_list ',' var
{
	$$ = add_variable($1, $3);
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
	symbol_t *symbol = $1;
	source_code_t *c_type = $3;
	source_code_t *c_init = $4;
	$$ = create_variable(symbol, c_type, c_init);
}
;

action_proto_declaration: action_proto_type TOK_ACTION returns symbol_hierarchy '(' param_list ')' TOK_SOURCE_CODE
{
	action_proto_type_t action_proto_type = $1;
	symbol_hierarchy_t *symbol_hierarchy = $4;
	source_code_t *returns = $3;
	param_list_t *param_list = $6;
	source_code_t *default_source_code = $8;
	action_proto_t *prev_action_proto = search_action_proto(symbol_hierarchy);

	if(prev_action_proto)
	{
		yyerrorf(current_filename, lineno, "warning! action prototype `%s' redefined", build_hierarchical_name(prev_action_proto->symbol_hierarchy));
		yyerrorf(prev_action_proto->filename, prev_action_proto->lineno, "action prototype `%s' previously defined here", build_hierarchical_name(prev_action_proto->symbol_hierarchy));
		remove_action_proto(prev_action_proto);
		delete_action_proto(prev_action_proto);
	}

	if(returns)
	{
		if(action_proto_type == CONSTRUCTOR_ACTION_PROTO)
		{
			yyerrorf(current_filename, lineno, "error! constructor action prototype `%s' must not have a return type (%s)", build_hierarchical_name(symbol_hierarchy), returns->content);
			YYABORT;
		}
		if(action_proto_type == DESTRUCTOR_ACTION_PROTO)
		{
			yyerrorf(current_filename, lineno, "error! destructor action prototype `%s' must not have a return type (%s)", build_hierarchical_name(symbol_hierarchy), returns->content);
			YYABORT;
		}
	}
	
	if(param_list)
	{
		if(action_proto_type == CONSTRUCTOR_ACTION_PROTO)
		{
			yyerrorf(current_filename, lineno, "error! constructor action prototype `%s' must not take any arguments", build_hierarchical_name(symbol_hierarchy));
			YYABORT;
		}
		if(action_proto_type == STATIC_ACTION_PROTO)
		{
			yyerrorf(current_filename, lineno, "error! static action prototype `%s' must not take any arguments", build_hierarchical_name(symbol_hierarchy));
			YYABORT;
		}
		if(action_proto_type == DESTRUCTOR_ACTION_PROTO)
		{
			yyerrorf(current_filename, lineno, "error! destructor action prototype `%s' must not take any arguments", build_hierarchical_name(symbol_hierarchy));
			YYABORT;
		}
	}
	
	$$ = create_action_proto(action_proto_type, symbol_hierarchy, returns, param_list, default_source_code, comment_list, current_filename, lineno);
	if(comment_list) comment_list = 0;
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
	source_code_t *c_type = $1;
	source_code_t *c_symbol = $2;
	$$ = create_param(c_type, c_symbol);
}
;

param_list:
{
	$$ = 0;
}
	| param
{
	param_t *param = $1;
	$$ = create_param_list(param);
}
	| param_list ',' param
{
	param_list_t *param_list = $1;
	param_t *param = $3;
	
	$$ = add_param(param_list, param);
}
;

action_proto_type:
{
	$$ = COMMON_ACTION_PROTO;
}
	| TOK_CONSTRUCTOR
{
	$$ = CONSTRUCTOR_ACTION_PROTO;
}
	| TOK_DESTRUCTOR
{
	$$ = DESTRUCTOR_ACTION_PROTO;
}
	| TOK_STATIC
{
	$$ = STATIC_ACTION_PROTO;
}
;

action_declaration: symbol_hierarchy '.' TOK_IDENT '=' TOK_SOURCE_CODE
{
	symbol_t *action_proto_symbol = $3;
	symbol_hierarchy_t *symbol_hierarchy = $1;
	action_proto_t *action_proto = search_action_proto2(symbol_hierarchy->parent, action_proto_symbol);
	operation_t *operation = 0;
	group_t *group = 0;
	source_code_t *source_code = $5;

	if(!action_proto)
	{
		yyerrorf(current_filename, lineno, "error! undefined action prototype `%s::%s'", build_hierarchical_name(symbol_hierarchy->parent), action_proto_symbol->name);
		YYABORT;
	}

	/* left symbol is either an operation or a group */
	operation = search_operation_global(symbol_hierarchy);

	if(!operation) group = search_group(symbol_hierarchy);

	if(!operation && !group)
	{
		yyerrorf(current_filename, lineno, "error! undefined operation or group `%s'", build_hierarchical_name(symbol_hierarchy));
		YYABORT;
	}
	if(operation && group)
	{
		yyerrorf(current_filename, lineno, "internal error! operation and group `%s' are both defined", build_hierarchical_name(symbol_hierarchy));
		YYABORT;
	}
	if(group)
	{
		operation_node_t *operation_node;

		for(operation_node = group->operation_node_list->first_operation_node; operation_node; operation_node = operation_node->next)
		{
			action_t *action = decl_action(operation_node->operation, action_proto, source_code, comment_list, current_filename, lineno);
			add_action(action->operation, action);
		}
		if(comment_list) comment_list = 0;
	}
	else
	{
		action_t *action = decl_action(operation, action_proto, source_code, comment_list, current_filename, lineno);
		add_action(action->operation, action);
		if(comment_list) comment_list = 0;
	}
}
;

include : TOK_INCLUDE TOK_STRING
	{
		char *filename = $2;
		if(!do_include(filename)) YYABORT;
	}
;

group_declaration: TOK_GROUP symbol_hierarchy '(' operation_list ')'
{
	symbol_hierarchy_t *group_symbol_hierarchy = $2;
	operation_node_list_t *operation_node_list = $4;
	operation_t *prev_operation = search_operation_global(group_symbol_hierarchy);
	group_t *prev_group = search_group(group_symbol_hierarchy);

	if(prev_operation)
	{
		yyerrorf(current_filename, lineno, "error! group name conflicts with operation `%s'", build_hierarchical_name(prev_operation->symbol_hierarchy));
		yyerrorf(prev_operation->filename, prev_operation->lineno, "operation `%s' previously defined here", build_hierarchical_name(prev_operation->symbol_hierarchy));
		delete_operation_node_list(operation_node_list);
		YYABORT;
	}

	if(prev_group)
	{
		yyerrorf(current_filename, lineno, "warning! group `%s' redefined", build_hierarchical_name(prev_group->symbol_hierarchy));
		yyerrorf(prev_group->filename, prev_group->lineno, "group `%s' previously defined here", build_hierarchical_name(prev_group->symbol_hierarchy));
		delete_operation_node_list(operation_node_list);
		YYABORT;
	}

	$$ = create_group(group_symbol_hierarchy, operation_node_list, current_filename, lineno);
}
;

operation_list:
	operation
{
	operation_node_t *operation_node = $1;
	$$ = create_operation_node_list(operation_node);
}
	| operation_list ',' operation
{
	operation_node_list_t *operation_node_list = $1;
	operation_node_t *operation_node = $3;
	operation_node_t *prev_operation_node;

	prev_operation_node = search_operation_node(operation_node_list, operation_node->operation->symbol_hierarchy);

	if(prev_operation_node)
	{
		yyerrorf(current_filename, lineno, "error! duplicated operation in list `%s'", build_hierarchical_name(operation_node->operation->symbol_hierarchy));
		YYABORT;
	}
	$$ = add_operation_node(operation_node_list, operation_node);
}
;

operation: symbol_hierarchy
{
	symbol_hierarchy_t *operation_symbol_hierarchy = $1;
	operation_t *operation = search_operation_global(operation_symbol_hierarchy);

	if(!operation)
	{
		yyerrorf(current_filename, lineno, "error! undefined operation `%s'", build_hierarchical_name(operation_symbol_hierarchy));
		YYABORT;
	}

	$$ = create_operation_node(operation);
}
;


%%

int yypanicf(filename_t *filename, int lineno, char *format, ...)
{
	char s[1024];
	va_list args;

	va_start(args, format);
	vsprintf(s, format, args);
	if(current_filename && filename)
		fprintf(stderr, "%s:%d: %s\n", filename->name, lineno, s);
	else
		fprintf(stderr, "%s\n", s);
	if(last_action_proto)
		fprintf(stderr, "%s:%d: last action prototype `%s' was declared here\n", last_action_proto->filename->name, last_action_proto->lineno, build_hierarchical_name(last_action_proto->symbol_hierarchy));
	if(last_operation)
		fprintf(stderr, "%s:%d: last action `%s' was declared here\n", last_operation->filename->name, last_operation->lineno, build_hierarchical_name(last_operation->symbol_hierarchy));
	if(last_source_code)
		fprintf(stderr, "%s:%d: last source code was declared here\n", last_source_code->filename->name, last_source_code->lineno);
	va_end(args);
	return 0;
}


int yyerrorf(filename_t *filename, int lineno, char *format, ...)
{
	char s[1024];
	va_list args;

	va_start(args, format);
	vsprintf(s, format, args);
	if(current_filename && filename)
		fprintf(stderr, "%s:%d: %s\n", filename->name, lineno, s);
	else
		fprintf(stderr, "%s\n", s);
	va_end(args);
	return 0;
}

int yyerror(char *s)
{
	if(current_filename)
		fprintf(stderr, "%s:%d: %s! unexpected %s\n", current_filename->name, lineno, s, get_token_name(yychar));
	else
		fprintf(stderr, "%s! unexpected %s\n", s, get_token_name(yychar));
	return 1;
}
