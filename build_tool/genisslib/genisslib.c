/***************************************************************************
         GenISSLib.c  -  Instruction Set Simulator Library Generator
                             -------------------
    begin                : Thu May 25 2003
    copyright            : (C) 2003 CEA and Universite Paris Sud
    author               : Gilles Mouchard
    email                : mouchard@lri.fr, gilles.mouchard@cea.fr
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/** @file GenISSLib.c
	@brief generates the instruction set simulator library
*/
#include <genisslib.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdarg.h>

#define GENISSLIB "GenISSLib"
#define VERSION "2.0"
#define AUTHOR "Gilles Mouchard"
#define EMAIL "gilles.mouchard@cea.fr"
#define COPYRIGHT "Copyright (c) 2003-2007 CEA and Universite Paris Sud"

#define DEFAULT_OUTPUT "iss"
#define DEFAULT_NAMESPACE "iss"
#define DEFAULT_ADDRESS_TYPE "uint32_t"

int little_endian = 1;
symbol_hierarchy_t *namespace = 0;
symbol_hierarchy_t *symbol_hierarchy_root = 0;
source_code_t *address_type = 0;

/** external definitions */
extern int lineno;
extern char *yytext;
extern int yylineno;
extern FILE *yyin;
extern char *current_filename;
extern void add_include_search_dir(char *dir_path);
extern void delete_include_stack();
extern int yyerrorf(filename_t *filename, int lineno, char *format, ...);
extern int yyerror(char *s);
extern int open_file(char *filename);
extern int yyparse();

/** a global list of the filename objects */
filename_t *filename_list = 0;

/** a global list of the directory path where to search include */
search_dir_t *search_dir_list = 0;

/** a global list of the C source code objects */
source_code_t *source_code_list = 0;
source_code_t *last_source_code = 0;

/** a global list of the operation objects */
operation_t *operation_list = 0;
operation_t *last_operation = 0;

/** a global list of the action prototype objects */
action_proto_t *action_proto_list = 0;
action_proto_t *last_action_proto = 0;

/** a global list of the C/C++ comment objects */
comment_list_t *comment_list = 0;

/** a global list of the symbol objects */
static symbol_t *symbol_list = 0;

/** a global list of the symbol hierarchy objects */
static symbol_hierarchy_t *symbol_hierarchy_list = 0;

/** a global variable list objects common to all operations */
variable_list_t variable_list;		/* don't forget to call initialize() */

/** a global list of the group objects */
group_t *group_list = 0;
group_t *last_group = 0;

template_parameter_list_t *template_parameters = 0;

/** a string buffer growing dynamically */
static char *string_buffer = 0;
/** the following character pointer pointing after the string buffer */
static char *current_char = 0;
/** the current string buffer size (in bytes) */
static int string_buffer_size = 0;

/** a file for outputing the generated source code */
static FILE *emit_file = 0;
/** the size of the output buffer */
static unsigned int emit_buffer_size = 0;
/** an output buffer (used for counting \n characters) */
static char *emit_buffer = 0;
/** the current filename of the output file */
static char *emit_filename = 0;
/** the current line number in the output file */
static unsigned int emit_lineno = 1;


/** The token table */
struct
{
	char *name;		/**< the name of the token */
	int token;		/**< the token */
} tokens[] = {
	{"namespace", TOK_NAMESPACE},
	{"address", TOK_ADDRESS},
	{"decl", TOK_DECL},
	{"impl", TOK_IMPL},
	{"op", TOK_OP},
	{"group", TOK_GROUP},
	{"action", TOK_ACTION},
	{"static", TOK_STATIC},
	{"constructor", TOK_CONSTRUCTOR},
	{"destructor", TOK_DESTRUCTOR},
	{"include", TOK_INCLUDE},
	{"sext", TOK_SEXT},
	{"shr", TOK_SHR},
	{"shl", TOK_SHL},
	{"var", TOK_VAR},
	{"little_endian", TOK_LITTLE_ENDIAN},
	{"big_endian", TOK_BIG_ENDIAN},
	{"template", TOK_TEMPLATE},
};

/** Convert a name to a token
	@param name name of the token
	@return the token
*/
int get_token(char *name)
{
	int i;

	for(i = 0; i < sizeof(tokens) / sizeof(tokens[0]); i++)
	{
		if(strcmp(tokens[i].name, name) == 0) return tokens[i].token;
	}

	return TOK_IDENT;
}

char *get_char_name(char c)
{
	switch(c)
	{
		case '\t': return "tab char";
		case '\n': return "return char";
		case '\r': return "carriage return";
		case '\b': return "back space";
		default:
		{
			static char buffer[10];
			if(c < 32)
			{
				sprintf(buffer, "char #%d", c);
			}
			else
			{
				sprintf(buffer, "'\\%d'", c);
			}
			return buffer;
		}
	}
	return 0;
}

/** Return the name of a token
	@param token the token
	@return the name of the token
*/
char *get_token_name(int token)
{
	int i;

	/* search into the token table */
	for(i = 0; i < sizeof(tokens) / sizeof(tokens[0]); i++)
	{
		if(tokens[i].token == token) return tokens[i].name;
	}

	/* return a string for the token not in the token table */
	switch(token)
	{
		case TOK_IDENT: return "identifier";
		case TOK_ENDL: return "end of line";
		case TOK_SOURCE_CODE: return "source code";
		case TOK_INTEGER: return "integer";
		case TOK_STRING: return "string";
		case '(': return "`('";
		case ')': return "`)'";
		case '<': return "`<'";
		case '>': return "`>'";
		case ':' : return "`:'";
		case '=' : return "`='";
		case '.': return "`.'";
		case '[': return "`['";
		case ']': return "`]'";
		case '?': return "`?'";
	}

	/* return a string for a character token */
	if(token < 256)
	{
		return get_char_name(token);
	}

	/* don't know which token it is ! */
	return "unknown token";
}

/** Create a filename object
	@param name the file name
	@return a filename object
*/
filename_t *create_filename(char *name)
{
	filename_t *filename = (filename_t *) malloc(sizeof(filename_t));

	if(!filename)
	{
		yyerrorf(0, 0, "internal error! not enough memory");
		exit(-1);
	}
	filename->name = strdup(name);
	if(!filename->name)
	{
		yyerrorf(0, 0, "internal error! not enough memory");
		exit(-1);
	}
	filename->next = 0;
	return filename;
}

/** Create a search directory object
	@param name the directory path
	@return a search directory object
*/
search_dir_t *create_search_dir(char *path)
{
	search_dir_t *search_dir = (search_dir_t *) malloc(sizeof(search_dir_t));

	if(!search_dir)
	{
		yyerrorf(0, 0, "internal error! not enough memory");
		exit(-1);
	}
	search_dir->path = strdup(path);
	if(!search_dir->path)
	{
		yyerrorf(0, 0, "internal error! not enough memory");
		exit(-1);
	}
	search_dir->next = 0;
	return search_dir;
}

/** Create a C/C++ comment object
	@param string the C String containing the C comment
	@param filename the filename object where the C/C++ comment was found
	@param lineno the line number where the C/C++ comment was found
	@return a C/C++ comment object
*/
comment_t *create_comment(char *string, filename_t *filename, int lineno)
{
	comment_t *comment = (comment_t *) malloc(sizeof(comment_t));

	if(!comment)
	{
		yyerrorf(0, 0, "internal error! not enough memory");
		exit(-1);
	}
	comment->string = strdup(string);
	if(!comment->string)
	{
		yyerrorf(0, 0, "internal error! not enough memory");
		exit(-1);
	}
	comment->prev = 0;
	comment->next = 0;
	return comment;
}

/** Create a C/C++ comment list object
	@param comment a C/C++ comment for initializing the list
	@return a C/C++ comment list object
*/
comment_list_t *create_comment_list(comment_t *comment)
{
	comment_list_t *comment_list = (comment_list_t *) malloc(sizeof(comment_list_t));

	if(!comment_list)
	{
		yyerrorf(0, 0, "internal error! not enough memory");
		exit(-1);
	}

	comment_list->first_comment = comment;
        comment_list->last_comment = comment;
	return comment_list;
}

/** Add a C/C++ comment object into a C/C++ comment list object
	@param comment_list a C/C++ comment list object
	@param comment a C/C++ comment object
*/
void add_comment(comment_list_t *comment_list, comment_t *comment)
{
	if(comment_list->last_comment) comment_list->last_comment->next = comment;
	comment->next = 0;
	comment_list->last_comment = comment;
	if(!comment_list->first_comment) comment_list->first_comment = comment;
}

symbol_hierarchy_t *get_symbol_hierarchy(symbol_hierarchy_t *namespace, symbol_t *symbol)
{
	symbol_hierarchy_t *symbol_hierarchy;

	for(symbol_hierarchy = symbol_hierarchy_list; symbol_hierarchy; symbol_hierarchy = symbol_hierarchy->next)
	{
		if(compare_symbol_hierarchy(symbol_hierarchy->parent, namespace) &&
		  symbol_hierarchy->symbol == symbol) return symbol_hierarchy;
	}

	symbol_hierarchy = (symbol_hierarchy_t *) malloc(sizeof(symbol_hierarchy_t));

	if(!symbol_hierarchy)
	{
		yyerrorf(0, 0, "internal error! not enough memory");
		exit(-1);
	}

	symbol_hierarchy->symbol = symbol;
	symbol_hierarchy->parent = namespace;
	symbol_hierarchy->next = symbol_hierarchy_list;
	if(symbol_hierarchy->parent)
	{
		symbol_hierarchy->sibling = symbol_hierarchy->parent->child;
		symbol_hierarchy->parent->child = symbol_hierarchy;
	}
	else
	{
		symbol_hierarchy->sibling = 0;
	}
	symbol_hierarchy->child = 0;
	symbol_hierarchy->operation = 0;
	symbol_hierarchy->action_proto = 0;

	if(!symbol_hierarchy_root) symbol_hierarchy_root = symbol_hierarchy;

	symbol_hierarchy_list = symbol_hierarchy;
	return symbol_hierarchy;
}

symbol_hierarchy_t *get_symbol_hierarchy2(symbol_hierarchy_t *symbol_hierarchy, symbol_t *symbol)
{
	symbol_hierarchy_t *cur_symbol_hierarchy;

	for(cur_symbol_hierarchy = symbol_hierarchy_list; cur_symbol_hierarchy; cur_symbol_hierarchy = cur_symbol_hierarchy->next)
	{
		if(cur_symbol_hierarchy->symbol == symbol)
		{
			if(compare_symbol_hierarchy(cur_symbol_hierarchy->parent, symbol_hierarchy)) return cur_symbol_hierarchy;
		}
	}

	cur_symbol_hierarchy = (symbol_hierarchy_t *) malloc(sizeof(symbol_hierarchy_t));

	if(!cur_symbol_hierarchy)
	{
		yyerrorf(0, 0, "internal error! not enough memory");
		exit(-1);
	}

	cur_symbol_hierarchy->symbol = symbol;
	cur_symbol_hierarchy->parent = symbol_hierarchy;
	cur_symbol_hierarchy->next = symbol_hierarchy_list;
	if(cur_symbol_hierarchy->parent)
	{
		cur_symbol_hierarchy->sibling = cur_symbol_hierarchy->parent->child;
		cur_symbol_hierarchy->parent->child = cur_symbol_hierarchy;
	}
	else
	{
		cur_symbol_hierarchy->sibling = 0;
	}
	cur_symbol_hierarchy->child = 0;
	cur_symbol_hierarchy->operation = 0;
	cur_symbol_hierarchy->action_proto = 0;
	if(!symbol_hierarchy_root) symbol_hierarchy_root = cur_symbol_hierarchy;
	symbol_hierarchy_list = cur_symbol_hierarchy;
	return cur_symbol_hierarchy;
}

char *build_hierarchical_name(symbol_hierarchy_t *symbol_hierarchy)
{
	int i;
	static char buffer[4096];
	char *p = &buffer[sizeof(buffer) - 1];
	*p-- = 0;

	if(symbol_hierarchy)
	{
		do
		{
			for(i = strlen(symbol_hierarchy->symbol->name) - 1; i >= 0; i--)
			{
				*p-- = symbol_hierarchy->symbol->name[i];
			}
			if(symbol_hierarchy->parent)
			{
				*p-- = ':';
				*p-- = ':';
			}
		} while((symbol_hierarchy = symbol_hierarchy->parent) != 0);
	}
	return p + 1;
}

char *build_preprocessor_hierarchical_name(symbol_hierarchy_t *symbol_hierarchy)
{
	int i;
	static char buffer[4096];
	char *p = &buffer[sizeof(buffer) - 1];
	*p-- = 0;

	if(symbol_hierarchy)
	{
		do
		{
			for(i = strlen(symbol_hierarchy->symbol->name) - 1; i >= 0; i--)
			{
				*p-- = symbol_hierarchy->symbol->name[i];
			}
			if(symbol_hierarchy->parent)
			{
				*p-- = '_';
				*p-- = '_';
			}
		} while((symbol_hierarchy = symbol_hierarchy->parent) != 0);
	}
	return p + 1;
}

char *escape_preprocessor_token(char *pp_token) {
	int i;
	static char buffer[4096];
	char *p = &buffer[sizeof(buffer)];
	*--p = '\0';
	
	if( pp_token == 0 ) return p;
	
	for(i = strlen(pp_token); (--i) >= 0;)
		*--p = isalnum(pp_token[i]) ? pp_token[i] : '_';
	
	return p;
}

int compare_symbol_hierarchy(symbol_hierarchy_t *a, symbol_hierarchy_t *b)
{
	if(!a && !b) return 1;
	if(!a || !b) return 0;
	do
	{
		if(strcmp(a->symbol->name, b->symbol->name) != 0) return 0;
		a = a->parent;
		b = b->parent;
	} while(a && b);
	return a == 0 && b == 0;
}

/** Create an operation object
	@param symbol a symbol object representing the operation
	@param bitfield_list a bit field list object containing the bit fields of the operation
	@param comment_list a C/C++ comment list object containing C/C++ comments relative to the operation
	@param variable_list a variable list object
	@param filename a filename object where the operation was found
	@param lineno a line number where the operation was found
	@return an operation object
*/
operation_t *create_operation(symbol_hierarchy_t *symbol_hierarchy, bitfield_list_t *bitfield_list, comment_list_t *comment_list, variable_list_t *variable_list, operation_t *base, source_code_t *op_condition, filename_t *filename, int lineno)
{
	operation_t *operation = (operation_t *) malloc(sizeof(operation_t));

	if(!operation)
	{
		yyerrorf(0, 0, "internal error! not enough memory");
		exit(-1);
	}

	operation->symbol_hierarchy = symbol_hierarchy;
	operation->bitfield_list = bitfield_list;
	operation->size = bitfield_list->size;
	operation->action_list = 0;
	operation->last_action = 0;
	operation->filename = filename;
	operation->lineno = lineno;
	operation->comment_list = comment_list;
	operation->base = base;
	operation->prev = 0;
	operation->next = 0;
	operation->sibling = 0;
	operation->opcode_mask = create_opcode_mask(bitfield_list);
	operation->opcode = create_opcode(bitfield_list);
	operation->variable_list = variable_list;
	operation->condition = op_condition;
	return operation;
}

/** Create an action object
	@param operation an operation object to which belong the action
	@param action_proto an action prototype object which is the prototype of this action
	@param source_code a C source code object which is the implementation of the action
	@param comment_list a C/C++ comment list object containing C/C++ comments relative to the action
	@param filename a filename object where the action was found
	@param lineno a line number where the action was found
	@return an action object
*/
action_t *create_action(operation_t *operation, action_proto_t *action_proto, source_code_t *source_code, comment_list_t *comment_list, filename_t *filename, int lineno)
{
	action_t *action = (action_t *) malloc(sizeof(action_t));

	if(!action)
	{
		yyerrorf(0, 0, "internal error! not enough memory");
		exit(-1);
	}

	action->operation = operation;
	action->action_proto = action_proto;
	action->source_code = source_code;
	action->comment_list = comment_list;
	action->filename = filename;
	action->lineno = lineno;
	action->prev = 0;
	action->next = 0;
	return action;
}

/** Create an action prototype object
	@param type the type of the action prototype (constructor, destructor, static, or common)
	@param symbol a symbol object representing the action prototype
	@param returns a C source code object which is the return type of the action
	@param params a C source code object which is the parameters of the action
	@param default_source_code a C source code object which is the default implementation of the action
	@param comment_list a C/C++ comment list object containing C/C++ comments relative to the action prototype
	@param filename a filename object where the action prototype was found
	@param lineno a line number where the action prototype was found
	@return an action prototype object
*/
action_proto_t *create_action_proto(action_proto_type_t type, symbol_hierarchy_t *symbol_hierarchy, source_code_t *returns, param_list_t *params, source_code_t *default_source_code, comment_list_t *comment_list, filename_t *filename, int lineno)
{
	action_proto_t *action_proto = (action_proto_t *) malloc(sizeof(action_proto_t));

	if(!action_proto)
	{
		yyerrorf(0, 0, "internal error! not enough memory");
		exit(-1);
	}

	action_proto->type = type;
	action_proto->symbol_hierarchy = symbol_hierarchy;
	action_proto->returns = returns;
	action_proto->params = params;
	action_proto->default_source_code = default_source_code;
	action_proto->filename = filename;
	action_proto->lineno = lineno;
	action_proto->comment_list = comment_list;
	action_proto->prev = 0;
	action_proto->next = 0;
	action_proto->sibling = 0;
	return action_proto;
}

/** Create an opcode bit field object
	@param size size in bits
	@param value value of the opcode
	@return an opcode bit field object
*/
bitfield_t *create_opcode_bitfield(unsigned int size, unsigned int value)
{
	bitfield_t *bitfield = (bitfield_t *) malloc(sizeof(bitfield_t));

	if(!bitfield)
	{
		yyerrorf(0, 0, "internal error! not enough memory");
		exit(-1);
	}

	bitfield->type = bf_opcode;
	bitfield->size = size;
	bitfield->u.opcode.value = value;
	bitfield->next = 0;
	return bitfield;
}

/** Create an operand bit field object
	@param size size in bits
	@param symbol a symbol object representing the operand
	@param shift a shift amount to do on the bit field
	@param sext non-zero if a sign extension is required
	@param size_modifier minimum bit size for holding the operand bit field
	@return an operand bit field object
*/
bitfield_t *create_operand_bitfield(unsigned int size, symbol_t *symbol, int shift, int sext, unsigned int size_modifier)
{
	bitfield_t *bitfield = (bitfield_t *) malloc(sizeof(bitfield_t));

	if(!bitfield)
	{
		yyerrorf(0, 0, "internal error! not enough memory");
		exit(-1);
	}

	bitfield->type = bf_operand;
	bitfield->size = size;
	bitfield->u.operand.symbol = symbol;
	bitfield->u.operand.sext = sext;
	bitfield->u.operand.size_modifier = size_modifier;
	bitfield->u.operand.shift = shift;
	bitfield->next = 0;
	return bitfield;
}

/** Create a don't care bit field object
	@param size size in bits
	@return a don't care bit field object
*/
bitfield_t *create_dontcare_bitfield(unsigned int size)
{
	bitfield_t *bitfield = (bitfield_t *) malloc(sizeof(bitfield_t));

	if(!bitfield)
	{
		yyerrorf(0, 0, "internal error! not enough memory");
		exit(-1);
	}

	bitfield->type = bf_dontcare;
	bitfield->size = size;
	bitfield->next = 0;
	return bitfield;
}

/** Create a bit field list object
	@param bitfield a bit field object for initializing the list
	@return a bit field list object
*/
bitfield_list_t *create_bitfield_list(bitfield_t *bitfield)
{
	bitfield_list_t *bitfield_list = (bitfield_list_t *) malloc(sizeof(bitfield_list_t));

	if(!bitfield_list)
	{
		yyerrorf(0, 0, "internal error! not enough memory");
		exit(-1);
	}

	bitfield_list->size = bitfield->size;
	bitfield_list->first_bitfield = bitfield;
	bitfield_list->last_bitfield = bitfield;
	return bitfield_list;
}

/** Create a C source code object
	@param source_code_string a C string containing the C source code
	@param filename a filename object where was found the C source code
	@param lineno a line number where was found the C source code
	@return a C source code object
*/
source_code_t *create_source_code(char *source_code_string, filename_t *filename, int lineno)
{
	source_code_t *source_code = (source_code_t *) malloc(sizeof(source_code_t));

	if(!source_code)
	{
		yyerrorf(0, 0, "internal error! not enough memory");
		exit(-1);
	}

	source_code->global = sc_notglobal;
	source_code->content = strdup(source_code_string);
	if(!source_code->content)
	{
		yyerrorf(0, 0, "internal error! not enough memory");
		exit(-1);
	}
	source_code->filename = filename;
	source_code->lineno = lineno;
	source_code->next = 0;
	return source_code;
}

/** Create a variable object
	@param symbol a symbol object
	@param c_type a C type
	@return a variable object
*/
variable_t *create_variable(symbol_t *symbol, source_code_t *c_type, source_code_t *c_init)
{
	variable_t *variable = (variable_t *) malloc(sizeof(variable_t));

	if(!variable)
	{
		yyerrorf(0, 0, "internal error! not enough memory");
		exit(-1);
	}

	variable->symbol = symbol;
	variable->c_type = c_type;
	variable->c_init = c_init;
	variable->next = 0;
	return variable;
}

/** Create a variable list object
	@param variable a variable object for initializing the list
	@returns a variable list object
*/
variable_list_t *create_variable_list(variable_t *variable)
{
	variable_list_t *variable_list = (variable_list_t *) malloc(sizeof(variable_list_t));

	if(!variable_list)
	{
		yyerrorf(0, 0, "internal error! not enough memory");
		exit(-1);
	}

	variable_list->first_variable = variable;
	variable_list->last_variable = variable;
	return variable_list;
}

/** Create a template parameter object
	@param c_type a C type
	@param c_symbol a C symbol
	@return a template parameter object
 */
template_parameter_t *create_template_parameter(source_code_t *c_type, source_code_t *c_symbol)
{
	template_parameter_t *template_parameter = (template_parameter_t *) malloc(sizeof(template_parameter_t));

	if(!template_parameter)
	{
		yyerrorf(0, 0, "internal error! not enough memory");
		exit(-1);
	}

	template_parameter->c_type = c_type;
	template_parameter->c_symbol = c_symbol;
	template_parameter->next = 0;
	return template_parameter;
}

/** Create a template parameter list object
	@param template_parameter a template parameter object for initializing the list
	@returns a template parameter list object
 */
template_parameter_list_t *create_template_parameter_list(template_parameter_t *template_parameter)
{
	template_parameter_list_t *template_parameter_list = (template_parameter_list_t *) malloc(sizeof(template_parameter_list_t));

	if(!template_parameter_list)
	{
		yyerrorf(0, 0, "internal error! not enough memory");
		exit(-1);
	}

	template_parameter_list->first_template_parameter = template_parameter;
	template_parameter_list->last_template_parameter = template_parameter;
	return template_parameter_list;
}

/** Create an operation node object
	@param operation an operation object to include into the node
	@returns an operation node object
*/
operation_node_t *create_operation_node(operation_t *operation)
{
	operation_node_t *operation_node = (operation_node_t *) malloc(sizeof(operation_node_t));

	if(!operation_node)
	{
		yyerrorf(0, 0, "internal error! not enough memory");
		exit(-1);
	}

	operation_node->operation = operation;
	operation_node->next = 0;
	return operation_node;
}


/** Create an operation node list object
	@param operation_node an operation node object for initializing the list
	@returns an operation node list object
*/
operation_node_list_t *create_operation_node_list(operation_node_t *operation_node)
{
	operation_node_list_t *operation_node_list = (operation_node_list_t *) malloc(sizeof(operation_node_list_t));

	if(!operation_node_list)
	{
		yyerrorf(0, 0, "internal error! not enough memory");
		exit(-1);
	}

	operation_node_list->first_operation_node = operation_node;
	operation_node_list->last_operation_node = operation_node;
	return operation_node_list;
}

/** Create a group object
	@param operation_list the list of the operation of the group
	@returns a group list object
*/
group_t *create_group(symbol_hierarchy_t *symbol_hierarchy, operation_node_list_t *operation_node_list, filename_t *filename, int lineno)
{
	group_t *group = (group_t *) malloc(sizeof(group_t));

	if(!group)
	{
		yyerrorf(0, 0, "internal error! not enough memory");
		exit(-1);
	}

	group->symbol_hierarchy = symbol_hierarchy;
	group->operation_node_list = operation_node_list;
	group->filename = filename;
	group->lineno = lineno;
	group->next = 0;
	return group;
}

/** Create a parameter object
	@param c_type a C type
	@param c_symbol a C symbol
	@returns a parameter object
*/
param_t *create_param(source_code_t *c_type, source_code_t *c_symbol)
{
	param_t *param = (param_t *) malloc(sizeof(param_t));

	if(!param)
	{
		yyerrorf(0, 0, "internal error! not enough memory");
		exit(-1);
	}

	param->c_type = c_type;
	param->c_symbol = c_symbol;
	param->prev = 0;
	param->next = 0;
	return param;
}

/** Create a parameter list object
	@param param a parameter object for initializing the list
	@returns a parameter list object
*/
param_list_t *create_param_list(param_t *param)
{
	param_list_t *param_list = (param_list_t *) malloc(sizeof(param_list_t));

	if(!param_list)
	{
		yyerrorf(0, 0, "internal error! not enough memory");
		exit(-1);
	}

	param_list->first_param = param;
	param_list->last_param = param;
	return param_list;
}







bitfield_list_t *dup_bitfield_list(bitfield_list_t *bitfield_list)
{
	bitfield_list_t *bitfield_list_copy = 0;

	if(bitfield_list)
	{
		bitfield_t *bitfield;
		for(bitfield = bitfield_list->first_bitfield; bitfield; bitfield = bitfield->next)
		{
			bitfield_t *bitfield_copy = dup_bitfield(bitfield);
	
			if(!bitfield_list_copy)
			{
				bitfield_list_copy = create_bitfield_list(bitfield_copy);
			}
			else
			{
				add_bitfield(bitfield_list_copy, bitfield_copy);
			}
		}
	}
	return bitfield_list_copy;

}

bitfield_t *dup_bitfield(bitfield_t *bitfield)
{
	if(!bitfield) return 0;
	bitfield_t *bitfield_copy = (bitfield_t *) malloc(sizeof(bitfield_t));

	if(!bitfield_copy)
	{
		yyerrorf(0, 0, "internal error! not enough memory");
		exit(-1);
	}

	memcpy(bitfield_copy, bitfield, sizeof(bitfield_t));
	bitfield_copy->next = 0;

	return bitfield_copy;
}

comment_list_t *dup_comment_list(comment_list_t *comment_list)
{
	comment_list_t *comment_list_copy = 0;

	if(comment_list)
	{
		comment_t *comment;
	
		for(comment = comment_list->first_comment; comment; comment = comment->next)
		{
			comment_t *comment_copy = dup_comment(comment);
	
			if(!comment_list_copy)
			{
				comment_list_copy = create_comment_list(comment_copy);
			}
			else
			{
				add_comment(comment_list_copy, comment_copy);
			}
		}
	}
	return comment_list_copy;
}

comment_t *dup_comment(comment_t *comment)
{
	return comment ? create_comment(comment->string, comment->filename, comment->lineno) : 0;
}

variable_list_t *dup_variable_list(variable_list_t *variable_list)
{
	variable_list_t *variable_list_copy = 0;

	if(variable_list)
	{
		variable_t *variable;

		for(variable = variable_list->first_variable; variable; variable = variable->next)
		{
			variable_t *variable_copy = dup_variable(variable);
	
			if(!variable_list_copy)
			{
				variable_list_copy = create_variable_list(variable_copy);
			}
			else
			{
				add_variable(variable_list_copy, variable_copy);
			}
		}
	}
	return variable_list_copy;
}

variable_t *dup_variable(variable_t *variable)
{
	return create_variable(variable->symbol, variable->c_type, variable->c_init);
}







/** Add a filename into the global filename list (filename_list)
	@param filename a filename object
*/
void add_filename(filename_t *filename)
{
	filename->next = filename_list;
	filename_list = filename;
}

/** Add a search directory into the global search directory list (search_dir_list)
	@param search_dir a search directory object
*/
void add_search_dir(search_dir_t *search_dir)
{
	search_dir->next = search_dir_list;
	search_dir_list = search_dir;
}

/** Add a C source code object into the global source code list (source_code_list)
	@param source_code a C source code object
*/
void add_source_code(source_code_t *source_code)
{
	if(last_source_code) last_source_code->next = source_code;
	source_code->next = 0;
	last_source_code = source_code;
	if(!source_code_list) source_code_list = source_code;
}

/** Add an operation object into the global operation list (operation_list)
	@param operation an operation object
*/
void add_operation_global(operation_t *operation)
{
	if(last_operation) last_operation->next = operation;
	operation->prev = last_operation;
	operation->next = 0;
	last_operation = operation;
	if(!operation_list) operation_list = operation;
	if(operation->symbol_hierarchy->parent)
	{
		operation->sibling = operation->symbol_hierarchy->parent->operation;
		operation->symbol_hierarchy->parent->operation = operation;
	}
	else
	{
		operation->sibling = 0;
	}	
}

/** Add an action prototype object into the global action prototype list (action_proto_list)
	@param action_proto an action prototype object
*/
void add_action_proto(action_proto_t *action_proto)
{
	if(last_action_proto) last_action_proto->next = action_proto;
	action_proto->prev = last_action_proto;
	action_proto->next = 0;
	last_action_proto = action_proto;
	if(!action_proto_list) action_proto_list = action_proto;
	if(action_proto->symbol_hierarchy->parent)
	{
		action_proto->sibling = action_proto->symbol_hierarchy->parent->action_proto;
		action_proto->symbol_hierarchy->parent->action_proto = action_proto;
	}
	else
	{
		action_proto->sibling = 0;
	}	
}

/** Add a bit field object to a bit field list object
	@param bitfield_list a bit field list object
	@param bitfield a bit field object
	@return the bit field list object
*/
bitfield_list_t *add_bitfield(bitfield_list_t *bitfield_list, bitfield_t *bitfield)
{
	if(bitfield_list->last_bitfield) bitfield_list->last_bitfield->next = bitfield;
	bitfield->next = 0;
	bitfield_list->last_bitfield = bitfield;
	if(!bitfield_list->first_bitfield) bitfield_list->first_bitfield = bitfield;
	bitfield_list->size += bitfield->size;
	return bitfield_list;
}

/** Add a variable object to variable list object
	@param variable_list a variable list object
	@param variable a variable object
	@return the variable list object
*/
variable_list_t *add_variable(variable_list_t *variable_list, variable_t *variable)
{
	if(variable_list->last_variable) variable_list->last_variable->next = variable;
	variable->next = 0;
	variable_list->last_variable = variable;
	if(!variable_list->first_variable) variable_list->first_variable = variable;
	return variable_list;
}

/** Append the content of a variable list object into the global variable list (variable_list)
	@param _variable_list a variable list object
*/
void add_variable_list(variable_list_t *_variable_list)
{
	if(variable_list.last_variable) variable_list.last_variable->next = _variable_list->first_variable;
	variable_list.last_variable = _variable_list->last_variable;
	if(!variable_list.first_variable) variable_list.first_variable = _variable_list->first_variable;
	free(_variable_list);
}

/** Add an operation node object to an operation node list object
	@param operation_node_list an operation node list object
	@param operation_node an operation node object
	@return the operation node list object
*/
operation_node_list_t *add_operation_node(operation_node_list_t *operation_node_list, operation_node_t *operation_node)
{
	if(operation_node_list->last_operation_node) operation_node_list->last_operation_node->next = operation_node;
	operation_node->next = 0;
	operation_node_list->last_operation_node = operation_node;
	if(!operation_node_list->first_operation_node) operation_node_list->first_operation_node = operation_node;
	return operation_node_list;
}

/** Add an action object to an operation object
	@param operation an operation object
	@param action an action object
*/
void add_action(operation_t *operation, action_t *action)
{
	if(operation->last_action) operation->last_action->next = action;
	action->prev = operation->last_action;
	action->next = 0;
	operation->last_action = action;
	if(!operation->action_list) operation->action_list = action;
}

/** Add a group object into the global group list (group_list)
	@param group a group object
*/
void add_group(group_t *group)
{
	if(last_group) last_group->next = group;
	group->next = 0;
	last_group = group;
	if(!group_list) group_list = group;
}

/** Add a parameter object to a parameter list object
	@param param_list a parameter list object
	@param param a parameter object
	@return the parameter list object
 */
param_list_t *add_param(param_list_t *param_list, param_t *param)
{
	if(param_list->last_param) param_list->last_param->next = param;
	param->next = 0;
	param_list->last_param = param;
	if(!param_list->first_param) param_list->first_param = param;
	return param_list;
}

/** Add a template parameter object to template_parameter list object
	@param template_parameter_list a template parameter list object
	@param template_parameter a template parameter object
	@return the template parameter list object
 */
template_parameter_list_t *add_template_parameter(template_parameter_list_t *template_parameter_list, template_parameter_t *template_parameter)
{
	if(template_parameter_list->last_template_parameter) template_parameter_list->last_template_parameter->next = template_parameter;
	template_parameter->next = 0;
	template_parameter_list->last_template_parameter = template_parameter;
	if(!template_parameter_list->first_template_parameter) template_parameter_list->first_template_parameter = template_parameter;
	return template_parameter_list;
}

/** Search for an operation which may be in conflict with an opcode/opcode mask pair
	@param opcode_mask an binary mask (64 bits maxi)
	@param opcode an opcode (64 bits maxi)
	@return the matching operation object, null if no operation object matches
*/
operation_t *search_operation_opcode(uint64_t opcode_mask, uint64_t opcode, symbol_hierarchy_t *symbol_hierarchy)
{
	operation_t *operation;

	for(operation = operation_list; operation; operation = operation->next)
	{
		if(compare_symbol_hierarchy(operation->symbol_hierarchy->parent, symbol_hierarchy->parent))
		{
			if((operation->opcode & opcode_mask) == opcode ||
			(opcode & operation->opcode_mask) == operation->opcode) return operation;
		}
	}
	return 0;
}

/** Search for an operation object
	@param operation_symbol a symbol object representing the operation
	@return the matching operation object, null if no operation object matches
*/
operation_t *search_operation_global(symbol_hierarchy_t *operation_symbol_hierarchy)
{
	operation_t *operation;

	for(operation = operation_list; operation; operation = operation->next)
	{
		if(compare_symbol_hierarchy(operation->symbol_hierarchy, operation_symbol_hierarchy)) return operation;
	}
	return 0;
}

operation_t *search_base_operation(symbol_hierarchy_t *symbol_hierarchy, symbol_t *symbol)
{
	operation_t *operation;

	if(!symbol_hierarchy) return 0;
	if(symbol_hierarchy->operation)
	{
		for(operation = symbol_hierarchy->operation; operation; operation = operation->sibling)
		{
			if(operation->symbol_hierarchy->symbol == symbol) return operation;
		}
	}
	return search_base_operation(symbol_hierarchy->parent, symbol);
}

/** Search for an action prototype object
	@param action_proto_symbol a symbol object representing the action prototype
	@return the matching action prototype object, null if no action prototype object matches
*/
action_proto_t *search_action_proto(symbol_hierarchy_t *action_proto_symbol_hierarchy)
{
	action_proto_t *action_proto;

	for(action_proto = action_proto_list; action_proto; action_proto = action_proto->next)
	{
		if(compare_symbol_hierarchy(action_proto->symbol_hierarchy, action_proto_symbol_hierarchy))
		{
			return action_proto;
		}
	}
	return 0;
}

/** Search for an action prototype object
	@param action_proto_symbol a symbol object representing the action prototype
	@return the matching action prototype object, null if no action prototype object matches
*/
action_proto_t *search_action_proto2(symbol_hierarchy_t *symbol_hierarchy, symbol_t *symbol)
{
	action_proto_t *action_proto;

	for(action_proto = action_proto_list; action_proto; action_proto = action_proto->next)
	{
		if(compare_symbol_hierarchy(action_proto->symbol_hierarchy->parent, symbol_hierarchy) && action_proto->symbol_hierarchy->symbol == symbol)
		{
			return action_proto;
		}
	}
	return 0;
}


/** Search for an action object from the operation it belongs to, and the action prototype it implements
	@param operation an operation object
	@param action_proto an action prototype object
	@return the matching action object, null if no action object matches
*/
action_t *search_action(operation_t *operation, action_proto_t *action_proto)
{
	action_t *action;

	for(action = operation->action_list; action; action = action->next)
	{
		if(action->action_proto == action_proto)
		{
			return action;
		}
	}
	return 0;
}

/** Search into a bit field list for an operand bit field object
	@param bitfield_list a bit field list object where to search the bit field
	@param bitfield_symbol a symbol object representing the operand
	@return a bit field object
*/
bitfield_t *search_operand_bitfield(bitfield_list_t *bitfield_list, symbol_t *bitfield_symbol)
{
	bitfield_t *bitfield;

	for(bitfield = bitfield_list->first_bitfield; bitfield; bitfield = bitfield->next)
	{
		if(bitfield->type == bf_operand)
		{
			if(bitfield->u.operand.symbol == bitfield_symbol)
			{
				return bitfield;
			}
		}
	}
	return 0;
}

/** Search into variable list for a variable object
	@param variable_list a variable list object where to search the variable
	@param variable_symbol a symbol object representing the variable
	@return a variable object
*/
variable_t *search_variable(variable_list_t *variable_list, symbol_t *variable_symbol)
{
	variable_t *variable;

	for(variable = variable_list->first_variable; variable; variable = variable->next)
	{
		if(variable->symbol == variable_symbol)
		{
			return variable;
		}
	}
	return 0;
}

/** Search into an operation node list for an operation node object
	@param operation_node_list an operation node list object where to search the operation node
	@param operation_symbol a symbol object representing the operation included into the operation node
	@return an operation node object
*/
operation_node_t *search_operation_node(operation_node_list_t *operation_node_list, symbol_hierarchy_t *operation_symbol_hierarchy)
{
	operation_node_t *operation_node;

	for(operation_node = operation_node_list->first_operation_node; operation_node; operation_node = operation_node->next)
	{
		if(compare_symbol_hierarchy(operation_node->operation->symbol_hierarchy, operation_symbol_hierarchy))
		{
			return operation_node;
		}
	}
	return 0;
}


/** Search for a group object
	@param group_symbol a symbol object representing the group
	@return the matching group object, null if no group object matches
*/
group_t *search_group(symbol_hierarchy_t *group_symbol_hierarchy)
{
	group_t *group;

	for(group = group_list; group; group = group->next)
	{
		if(compare_symbol_hierarchy(group->symbol_hierarchy, group_symbol_hierarchy)) return group;
	}
	return 0;
}

/** Remove an operation object from the global operation object list (operation_list)
	@param operation the operation object to remove
*/
void remove_operation(operation_t *operation)
{
	if(operation->prev)
	{
		operation->prev->next = operation->next;
	}

	if(operation->next)
	{
		operation->next->prev = operation->prev;
	}

	if(operation == operation_list) operation_list = operation->next;
	if(operation == last_operation) last_operation = operation->prev;
	operation->next = 0;
	operation->prev = 0;
}

/** Remove an action prototype object from the global action prototype object list (action_proto_list)
	@param action_proto the action prototype object to remove
*/
void remove_action_proto(action_proto_t *action_proto)
{
	if(action_proto->prev)
	{
		action_proto->prev->next = action_proto->next;
	}

	if(action_proto->next)
	{
		action_proto->next->prev = action_proto->prev;
	}

	if(action_proto == action_proto_list) action_proto_list = action_proto->next;
	if(action_proto == last_action_proto) last_action_proto = action_proto->prev;
	action_proto->next = 0;
	action_proto->prev = 0;
}

/** Remove an action object owned by an operation object
	@param operation the operation object owning the action
	@param action the action object to remove
*/
void remove_action(operation_t *operation, action_t *action)
{
	if(action->prev)
	{
		action->prev->next = action->next;
	}

	if(action->next)
	{
		action->next->prev = action->prev;
	}

	if(action == operation->action_list) operation->action_list = action->next;
	if(action == operation->last_action) operation->last_action = action->prev;
	action->next = 0;
	action->prev = 0;
}

/** Initialize the string buffer */
void init_string_buffer()
{
	string_buffer_size = 16;
	string_buffer = (char *) malloc(string_buffer_size);

	if(!string_buffer)
	{
		yyerrorf(0, 0, "internal error! not enough memory");
		exit(-1);
	}

	current_char = string_buffer;
}

/** Empty the string buffer */
void empty_string_buffer()
{
	*string_buffer = 0;
	current_char = string_buffer;
}

/** Add characters into the string buffer
	@param length number of characters to add
	@param chars characters to add
*/
void add_chars_to_string_buffer(int length, char *chars)
{
	if(current_char - string_buffer + length + 1>= string_buffer_size)
	{
		unsigned int offset = current_char - string_buffer;
		string_buffer_size = (current_char - string_buffer + length + 1) * 2;
		string_buffer = (char *) realloc(string_buffer, string_buffer_size);
		if(!string_buffer)
		{
			yyerrorf(0, 0, "internal error! not enough memory");
			exit(-1);
		}
		current_char = string_buffer + offset;
	}
	strcpy(current_char, chars);
	current_char += length;
}

/** Returns the string contained into the string buffer
	@return a C string
*/
char *get_string_buffer()
{
	return string_buffer;
}

/** Delete the string buffer */
void delete_string_buffer()
{
	if(string_buffer)
		free(string_buffer);
	string_buffer = 0;
	current_char = 0;
}

/** Search for a symbol object. If the symbol object was not found, then creates a new one
	@param name name of the symbol
	@return the symbol object (either found or created)
*/
symbol_t *get_symbol(char *name)
{
	symbol_t *symbol;

	for(symbol = symbol_list; symbol; symbol = symbol->next)
	{
		if(strcmp(symbol->name, name) == 0) return symbol;
	}

	symbol = (symbol_t *) malloc(sizeof(symbol_t));

	if(!symbol)
	{
		yyerrorf(0, 0, "internal error! not enough memory");
		exit(-1);
	}

	symbol->name = strdup(name);
	if(!symbol->name)
	{
		yyerrorf(0, 0, "internal error! not enough memory");
		exit(-1);
	}
	symbol->next = symbol_list;
	symbol_list = symbol;
	return symbol;
}

/** Delete a C/C++ comment object
	@param comment a C/C++ comment object to delete
*/
void delete_comment(comment_t *comment)
{
	free(comment->string);
	free(comment);
}

/** Delete a C/C++ comment list object
	@param comment_list a C/C++ comment list object to delete
*/
void delete_comment_list(comment_list_t *comment_list)
{
	comment_t *comment, *next_comment;

	for(comment = comment_list->first_comment; comment; comment = next_comment)
	{
		next_comment = comment->next;
		delete_comment(comment);
	}
	free(comment_list);
}

/** Delete a filename object
	@param filename a filename object to delete
*/
void delete_filename(filename_t *filename)
{
	free(filename->name);
	free(filename);
}

/** Delete a search directory object
	@param search_dir a search directory object to delete
*/
void delete_search_dir(search_dir_t *search_dir)
{
	free(search_dir->path);
	free(search_dir);
}

/** Delete a symbol object
	@param symbol a symbol object to delete
*/
void delete_symbol(symbol_t *symbol)
{
	free(symbol->name);
	free(symbol);
}

/** Delete a bit field list object
	@param bitfield_list a bit field list to delete
*/
void delete_bitfield_list(bitfield_list_t *bitfield_list)
{
	bitfield_t *bitfield, *next_bitfield;

	for(bitfield = bitfield_list->first_bitfield; bitfield; bitfield = next_bitfield)
	{
		next_bitfield = bitfield->next;
		delete_bitfield(bitfield);
	}
	free(bitfield_list);
}

/** Delete an operation object
	@param operation an operation object to delete
*/
void delete_operation(operation_t *operation)
{
	delete_bitfield_list(operation->bitfield_list);
	if(operation->comment_list) delete_comment_list(operation->comment_list);
	if(operation->variable_list) delete_variable_list(operation->variable_list);
	free(operation);
}

/** Delete an action object
	@param action an action object to delete
*/
void delete_action(action_t *action)
{
	free(action);
}

/** Delete an action prototype object
	@param action_proto an action prototype object to delete
*/
void delete_action_proto(action_proto_t *action_proto)
{
	if(action_proto->params) delete_param_list(action_proto->params);
	free(action_proto);
}

/** Delete a bit field object
	@param bitfield a bit field object to delete
*/
void delete_bitfield(bitfield_t *bitfield)
{
	free(bitfield);
}

/** Delete a C source code object
	@param source_code a C source code object to delete
*/
void delete_source_code(source_code_t *source_code)
{
	free(source_code->content);
	free(source_code);
}

/** Delete a variable object
	@param variable a variable object to delete
*/
void delete_variable(variable_t *variable)
{
	free(variable);
}

/** Delete an operation node object
	@param operation_node an operation node to delete
*/
void delete_operation_node(operation_node_t *operation_node)
{
	free(operation_node);
}

/** Delete a variable list object
	@param variable_list a variable list object to delete
*/
void delete_variable_list(variable_list_t *variable_list)
{
	variable_t *variable, *next_variable;

	for(variable = variable_list->first_variable; variable; variable = next_variable)
	{
		next_variable = variable->next;
		delete_variable(variable);
	}
	free(variable_list);
}

/** Delete an operation node list object
	@param operation_node_list an operation node list object to delete
*/
void delete_operation_node_list(operation_node_list_t *operation_node_list)
{
	operation_node_t *operation_node, *next_operation_node;

	for(operation_node = operation_node_list->first_operation_node; operation_node; operation_node = next_operation_node)
	{
		next_operation_node = operation_node->next;
		delete_operation_node(operation_node);
	}
	free(operation_node_list);
}

/** Delete a group object
	@param group a group object to delete
*/
void delete_group(group_t *group)
{
	delete_operation_node_list(group->operation_node_list);
	free(group);
}

/** Delete a parameter object
	@param param a parameter object to delete
*/
void delete_param(param_t *param)
{
	free(param);
}

/** Delete a parameter list object
	@param param a parameter list object to delete
 */
void delete_param_list(param_list_t *param_list)
{
	param_t *param, *next_param;

	for(param = param_list->first_param; param; param = next_param)
	{
		next_param = param->next;
		delete_param(param);
	}
	free(param_list);
}


/** Delete all filename objects from the global filename object list (filename_list) */
void delete_filenames()
{
	filename_t *filename, *next_filename;

	for(filename = filename_list; filename; filename = next_filename)
	{
		next_filename = filename->next;
		delete_filename(filename);
	}
	filename_list = 0;
}

/** Delete all search directory objects from the global search directory object list (search_dir_list) */
void delete_search_dirs()
{
	search_dir_t *search_dir, *next_search_dir;

	for(search_dir = search_dir_list; search_dir; search_dir = next_search_dir)
	{
		next_search_dir = search_dir->next;
		delete_search_dir(search_dir);
	}
	search_dir_list = 0;
}

/** Delete all C source code objects from the global C source code object list (source_code_list) */
void delete_source_codes()
{
	source_code_t *source_code, *next_source_code;

	for(source_code = source_code_list; source_code; source_code = next_source_code)
	{
		next_source_code = source_code->next;
		delete_source_code(source_code);
	}
	source_code_list = 0;
	last_source_code = 0;
}

/** Delete all action prototype objects from the global action prototype object list (action_proto_list) */
void delete_action_protos()
{
	action_proto_t *action_proto, *next_action_proto;

	for(action_proto = action_proto_list; action_proto; action_proto = next_action_proto)
	{
		next_action_proto = action_proto->next;
		delete_action_proto(action_proto);
	}
	action_proto_list = 0;
	last_action_proto = 0;
}

/** Delete all operation objects from the global operation object list (operation_list) */
void delete_operations()
{
	operation_t *operation, *next_operation;

	for(operation = operation_list; operation; operation = next_operation)
	{
		next_operation = operation->next;
		delete_operation(operation);
	}
	operation_list = 0;
	last_operation = 0;
}

/** Delete all symbol objects from the global symbol object list (symbol_list) */
void delete_symbols()
{
	symbol_t *symbol, *next_symbol;

	for(symbol = symbol_list; symbol; symbol = next_symbol)
	{
		next_symbol = symbol->next;
		delete_symbol(symbol);
	}
	symbol_list = 0;
}

/** Delete all variable objects from the global variable list object (variable_list) */
void delete_variables()
{
	variable_t *variable, *next_variable;

	for(variable = variable_list.first_variable; variable; variable = next_variable)
	{
		next_variable = variable->next;
		delete_variable(variable);
	}
	variable_list.first_variable = 0;
	variable_list.last_variable = 0;
}

/** Dump a bit field list object into a stream
	@param bitfield_list a bit field list object to dump
	@param stream a stream
*/
void dump_bitfield_list(bitfield_list_t *bitfield_list, FILE *stream)
{
	bitfield_t *bitfield;

	for(bitfield = bitfield_list->first_bitfield; bitfield; bitfield = bitfield->next)
	{
		dump_bitfield(bitfield, stream);
		if(bitfield->next) fprintf(stream,":");
	}
}

/** Dump an operation object into a stream
	@param operation an operation object to dump
	@param stream a stream
*/
void dump_operation(operation_t *operation, FILE *stream)
{
	action_t *action;

	fprintf(stream, "op %s(", build_hierarchical_name(operation->symbol_hierarchy));
	dump_bitfield_list(operation->bitfield_list, stream);
	fprintf(stream, ")");
	if(operation->variable_list)
	{
		fprintf(stream, " var ");
		dump_variable_list(operation->variable_list, stream);
	}
	fprintf(stream, "\n\n");

	for(action = operation->action_list; action; action = action->next)
	{
		dump_action(action, stream);
		fprintf(stream, "\n");
	}
}

/** Dump a bit field object into a stream
	@param bitfield a bit field object to dump
	@param stream a stream
*/
void dump_bitfield(bitfield_t *bitfield, FILE *stream)
{
	switch(bitfield->type)
	{
		case bf_opcode:
			fprintf(stream, "0x%x[%u]", bitfield->u.opcode.value, bitfield->size);
			break;

		case bf_operand:
			if(bitfield->u.operand.sext)
				fprintf(stream, "sext");

			if(bitfield->u.operand.size_modifier)
				fprintf(stream, "<%u>", bitfield->u.operand.size_modifier);

			if(bitfield->u.operand.sext || bitfield->u.operand.size_modifier)
				fprintf(stream, " ");

			fprintf(stream, "%s[%u]", bitfield->u.operand.symbol->name, bitfield->size);

			break;

		case bf_dontcare:
			fprintf(stream, "?[%u]", bitfield->size);
			break;
	}
}

/** Dump an action prototype object into a stream
	@param action_proto an action prototype object to dump
	@param stream a stream
*/
void dump_action_proto(action_proto_t *action_proto, FILE *stream)
{
	switch(action_proto->type)
	{
		case CONSTRUCTOR_ACTION_PROTO:
			fprintf(stream, "constructor ");
			break;

		case STATIC_ACTION_PROTO:
			fprintf(stream, "static ");
			break;

		case DESTRUCTOR_ACTION_PROTO:
			fprintf(stream, "destructor ");
			break;
			
		case COMMON_ACTION_PROTO:
			/* do nothing */
			break;
	}
	fprintf(stream, "action");
	if(action_proto->returns)
		fprintf(stream, " {%s}", action_proto->returns->content);

	fprintf(stream, " %s(", build_hierarchical_name(action_proto->symbol_hierarchy));
	if(action_proto->params)
	{
		dump_param_list(action_proto->params, stream);
//		fprintf(stream, "{%s}", action_proto->params->content);
	}

	fprintf(stream, ") {%s}\n", action_proto->default_source_code->content);
}

/** Dump an action object into a stream
	@param action an action object to dump
	@param stream a stream
*/
void dump_action(action_t *action, FILE *stream)
{
	fprintf(stream, "%s.%s = {", build_hierarchical_name(action->operation->symbol_hierarchy), action->action_proto->symbol_hierarchy->symbol->name);
	dump_source_code(action->source_code, stream);
	fprintf(stream, "}\n");
}

/** Dump a C source code object into a stream
	@param source_code a C source code object to dump
	@param stream a stream
*/
void dump_source_code(source_code_t *source_code, FILE *stream)
{
	fprintf(stream, "%s", source_code->content);
}

/** Dump a variable object into a stream
	@param variable a variable object to dump
	@param stream a stream
*/
void dump_variable(variable_t *variable, FILE *stream)
{
	fprintf(stream, "%s : {%s}", variable->symbol->name, variable->c_type->content);
}

/** Dump a variable list object into a stream
	@param variable_list a variable list object to dump
	@param stream a stream
*/
void dump_variable_list(variable_list_t *variable_list, FILE *stream)
{
	variable_t *variable;

	for(variable = variable_list->first_variable; variable; variable = variable->next)
	{
		dump_variable(variable, stream);
		if(variable->next) fprintf(stream,", ");
	}
}

/** Dump a parameter object into a stream
	@param param a parameter object
	@param stream a stream
*/
void dump_param(param_t *param, FILE *stream)
{
	fprintf(stream, "{");
	dump_source_code(param->c_type, stream);
	fprintf(stream, "} {");
	dump_source_code(param->c_symbol, stream);
	fprintf(stream, "}");
}

/** Dump a parameter list object into a stream
	@param param a parameter list object
	@param stream a stream
 */
void dump_param_list(param_list_t *param_list, FILE *stream)
{
	param_t *param;

	for(param = param_list->first_param; param; param = param->next)
	{
		dump_param(param, stream);
		if(param->next) fprintf(stream,", ");
	}
}

/** Dump a template parameter object into a stream
	@param param a template parameter object
	@param stream a stream
 */
void dump_template_parameter(template_parameter_t *template_parameter, FILE *stream)
{
	fprintf(stream, "{");
	dump_source_code(template_parameter->c_type, stream);
	fprintf(stream, "} {");
	dump_source_code(template_parameter->c_symbol, stream);
	fprintf(stream, "}");
}

/** Dump a template parameter list object into a stream
	@template_parameter template parameter a template parameter list object
	@template_parameter stream a stream
 */
void dump_template_parameter_list(template_parameter_list_t *template_parameter_list, FILE *stream)
{
	template_parameter_t *template_parameter;

	for(template_parameter = template_parameter_list->first_template_parameter; template_parameter; template_parameter = template_parameter->next)
	{
		dump_template_parameter(template_parameter, stream);
		if(template_parameter->next) fprintf(stream,", ");
	}
}


/** Dump the global list of variable objects (variable_list) into a stream
	@param stream a stream
*/
void dump_global_variable_list(FILE *stream)
{
	variable_t *variable;

	fprintf(stream, "var ");
	for(variable = variable_list.first_variable; variable; variable = variable->next)
	{
		dump_variable(variable, stream);
		if(variable->next) fprintf(stream,", ");
	}
	fprintf(stream, "\n");
}

/** Dump all action prototype objects from the global action prototype object list (action_proto_list) into a stream
	@param stream a stream
*/
void dump_action_protos(FILE *stream)
{
	action_proto_t *action_proto;

	for(action_proto = action_proto_list; action_proto; action_proto = action_proto->next)
	{
		dump_action_proto(action_proto, stream);
		fprintf(stream, "\n");
	}
}

/** Dump all operation objects from the global operation object list (operation_list) into a stream
	@param stream a stream
*/
void dump_operations(FILE *stream)
{
	operation_t *operation;

	for(operation = operation_list; operation; operation = operation->next)
	{
		dump_operation(operation, stream);
		fprintf(stream, "\n");
	}
}

/** Dump all C source code objects from the global C source code object list (source_code_list) into a stream
	@param stream a stream
*/
void dump_source_codes(FILE *stream)
{
	source_code_t *source_code;

	for(source_code = source_code_list; source_code; source_code = source_code->next)
	{
		if(source_code->global == sc_decl)
		{
			fprintf(stream, "{\n");
			dump_source_code(source_code, stream);
			fprintf(stream, "\n}\n\n");
		}
	}
}

void dump_global_definitions(FILE *stream)
{
	fprintf(stream, "%s\n", little_endian ? "little_endian" : "big_endian");
	fprintf(stream, "address {");
	dump_source_code(address_type, stream);
	fprintf(stream, "}\n");
	fprintf(stream, "template <");
	dump_template_parameter_list(template_parameters, stream);
	fprintf(stream, ">\n");
}

/** Dump all objects from the global objects list (source_code_list, action_proto_list, operation_list) into a stream
    with the same format as in the original code
	@param stream a stream
*/
void dump(FILE *stream)
{
	dump_global_definitions(stream);
	fprintf(stream, "\n");
	dump_source_codes(stream);
	fprintf(stream, "\n");
	dump_global_variable_list(stream);
	fprintf(stream, "\n");
	dump_action_protos(stream);
	fprintf(stream, "\n");
	dump_operations(stream);
}

/** Returns the maximum bit size of operations from the global operation object list (operation_list)
	@return the maximum bit size of operations
*/
unsigned int get_max_operation_size()
{
	unsigned int max_operation_size = 0;
	operation_t *operation;

	for(operation = operation_list; operation; operation = operation->next)
	{
		if(operation->size > max_operation_size) max_operation_size = operation->size;
	}
	return max_operation_size;
}

/** Computes the opcode from a bit field list object
	@param bitfield_list a bit field list object
	@return the opcode (max 64 bits)
*/
uint64_t create_opcode(bitfield_list_t *bitfield_list)
{
	unsigned int shift = bitfield_list->size;
	bitfield_t *bitfield;

	uint64_t opcode = 0;

	for(bitfield = bitfield_list->first_bitfield; bitfield; bitfield = bitfield->next)
	{
		shift -= bitfield->size;
		if(bitfield->type == bf_opcode)
		{
			/* oups: I forgot the uint64_t cast ! */
			opcode |= (uint64_t) bitfield->u.opcode.value << shift;
		}
	}
	return opcode;
}

/** Computes the opcode mask from a bit field list object
	@param bitfield_list a bit field list object
	@return a binary mask for the opcode (64 bits max)
*/
uint64_t create_opcode_mask(bitfield_list_t *bitfield_list)
{
	unsigned int shift = bitfield_list->size;
	bitfield_t *bitfield;

	uint64_t opcode_mask = 0;

	for(bitfield = bitfield_list->first_bitfield; bitfield; bitfield = bitfield->next)
	{
		shift -= bitfield->size;
		if(bitfield->type == bf_opcode) opcode_mask |= (((uint64_t) 1 << bitfield->size) -1) << shift;
	}
	return opcode_mask;
}

/** Returns a type size of the good bit size
	@param size a size in bits
	@return a size in bytes
*/
int get_code_type_size(int size)
{
	if(size <= 8) return 1;
	if(size <= 16) return 2;
	if(size <= 32) return 4;
	if(size <= 64) return 8;
	return 0;
}


/** Returns a type name of the good bit size
	@param size a size in bits
	@param is_signed_type non-zero if the type is signed
	@return a C string containing the type name
*/
char *get_type_name(int size, int is_signed_type)
{
	if(is_signed_type)
	{
		if(size <= 8) return "int8_t";
		if(size <= 16) return "int16_t";
		if(size <= 32) return "int32_t";
		if(size <= 64) return "int64_t";
	}
	else
	{
		if(size <= 8) return "uint8_t";
		if(size <= 16) return "uint16_t";
		if(size <= 32) return "uint32_t";
		if(size <= 64) return "uint64_t";
	}
	return 0;
}

/** Returns a modifier to add after a integer constant
	@param size a size in bits
	@param is_signed_type non-zero if the type is signed
	@return a C string containing the modifier
*/
char *get_const_modifier_name(int size, int is_signed_type)
{
	if(is_signed_type)
	{
		if(size <= 8) return "";
		if(size <= 16) return "";
		if(size <= 32) return "L";
		if(size <= 64) return "LL";
	}
	else
	{
		if(size <= 8) return "U";
		if(size <= 16) return "U";
		if(size <= 32) return "UL";
		if(size <= 64) return "ULL";
	}
	return 0;
}

/** Returns a type format of the good bit size
	@param size a size in bits
	@param is_signed_type non-zero if the type is signed
	@return a C string containing the format for printf functions
*/
char *get_type_format(int size, int is_signed_type)
{
	if(is_signed_type)
	{
		if(size <= 8) return "%\" PRIi8 \"";
		if(size <= 16) return "%\" PRIi16 \"";
		if(size <= 32) return "%\" PRIi32 \"";
		if(size <= 64) return "%\" PRIi64 \"";
	}
	else
	{
		if(size <= 8) return "%\" PRIu8 \"";
		if(size <= 16) return "%\" PRIu16 \"";
		if(size <= 32) return "%\" PRIu32 \"";
		if(size <= 64) return "%\" PRIu64 \"";
	}
	return 0;
}

/** Returns the type size to hold an operand bit field
	@param bitfield_size the size in bits of the operand bit field
	@param word_size the minimum size in bits to hold an operand bit field
	@return the type size
*/
unsigned int get_bitfield_type_size(unsigned int bitfield_size, unsigned int word_size)
{
	return word_size > 0 ? (word_size > bitfield_size ? word_size : bitfield_size) : bitfield_size;
}

/** Do some additionnal checks after the parsing
	@return non-zero if successful
*/
int sanity_checks()
{
	int success = 1;
	unsigned int max_operation_size = get_max_operation_size();

	operation_t *operation;

/*
	if(!namespace)
	{
		yyerrorf(0, 0, "warning! no user defined name space found, using name space \"%s\"", DEFAULT_NAMESPACE);
		symbol_hierarchy_root = get_symbol_hierarchy(0, get_symbol(DEFAULT_NAMESPACE));
	}
*/
	if(!address_type)
	{
		yyerrorf(0, 0, "warning! no user defined address type found, using type %s", DEFAULT_ADDRESS_TYPE);
	}

	for(operation = operation_list; operation; operation = operation->next)
	{
		variable_t *variable, *found_variable;

		//yyerrorf(operation->filename, operation->lineno, "operation `%s' is %u-bit long", operation->symbol->name, operation->size);

		if(operation->size != max_operation_size)
			yyerrorf(operation->filename, operation->lineno, "warning! operation `%s' is %u-bit long instead of %u-bit", build_hierarchical_name(operation->symbol_hierarchy), operation->size, max_operation_size);

		for(variable = variable_list.first_variable; variable; variable = variable->next)
		{
			found_variable = search_variable(&variable_list, variable->symbol);
			
			if(found_variable && variable != found_variable)
			{
				yyerrorf(variable->c_type->filename, variable->c_type->lineno, "error! global variable `%s' is already defined", variable->symbol->name);
				yyerrorf(found_variable->c_type->filename, found_variable->c_type->lineno, "variable `%s' previously defined here", found_variable->symbol->name);
				success = 0;
			}
		}

		if(operation->variable_list)
		{
			for(variable = operation->variable_list->first_variable; variable; variable = variable->next)
			{
				found_variable = search_variable(&variable_list, variable->symbol);

				if(found_variable && variable != found_variable)
				{
					yyerrorf(variable->c_type->filename, variable->c_type->lineno, "error! in operation `%s', variable `%s' is already defined as global", build_hierarchical_name(operation->symbol_hierarchy), variable->symbol->name);
					yyerrorf(found_variable->c_type->filename, found_variable->c_type->lineno, "variable `%s' previously defined here", found_variable->symbol->name);
					success = 0;
				}

				found_variable = search_variable(operation->variable_list, variable->symbol);

				if(found_variable && variable != found_variable)
				{
					yyerrorf(variable->c_type->filename, variable->c_type->lineno, "error! in operation `%s', variable `%s' is defined several times", build_hierarchical_name(operation->symbol_hierarchy), variable->symbol->name);
					success = 0;
				}
			}
		}
	}
	return success;
}

void finalize_operation(symbol_hierarchy_t *node, operation_t *operation)
{
	if(node->child)
	{
		symbol_hierarchy_t *child;
		symbol_hierarchy_t *symbol_hierarchy = get_symbol_hierarchy2(node, operation->symbol_hierarchy->symbol);
		operation_t *clone_operation;

		clone_operation = search_operation_global(symbol_hierarchy);

		if(clone_operation)
		{
			action_t *action;

			for(action = operation->action_list; action; action = action->next)
			{
				action_t *clone_action = search_action(clone_operation, action->action_proto);

				if(!clone_action)
				{
					clone_action = create_action(clone_operation, action->action_proto, action->source_code, action->comment_list, action->filename, action->lineno);
					//fprintf(stderr, "autocreate %s\n", build_hierarchical_name(clone_action->action_proto->symbol_hierarchy));
					//fprintf(stderr, "for operation %s\n", build_hierarchical_name(clone_operation->symbol_hierarchy));
			
					add_action(clone_operation, clone_action);
				}
			}
		}
		else
		{
			clone_operation = create_operation(symbol_hierarchy, dup_bitfield_list(operation->bitfield_list), dup_comment_list(operation->comment_list), dup_variable_list(operation->variable_list), operation, operation->condition, operation->filename, operation->lineno);

			if(!search_operation_opcode(clone_operation->opcode_mask, clone_operation->opcode, clone_operation->symbol_hierarchy))
			{
				char *operation_name = strdup(build_hierarchical_name(operation->symbol_hierarchy));
				//yyerrorf(operation->filename, operation->lineno, "warning! operation `%s' is implicitely a clone of operation '%s'", build_hierarchical_name(clone_operation->symbol_hierarchy), operation_name);
				free(operation_name);

				add_operation_global(clone_operation);
	
				action_t *action;
	
				for(action = operation->action_list; action; action = action->next)
				{
					action_proto_t *action_proto = search_action_proto2(clone_operation->symbol_hierarchy->parent, action->action_proto->symbol_hierarchy->symbol);

					if(action_proto)
					{
						action_t *clone_action = create_action(clone_operation, action_proto, action->source_code, action->comment_list, action->filename, action->lineno);
						//yyerrorf(action->filename, action->lineno, "warning! cloning action `%s'", build_hierarchical_name(action->action_proto->symbol_hierarchy));
						//yyerrorf(clone_action->filename, clone_action->lineno, "clone action `%s' is here", build_hierarchical_name(clone_action->action_proto->symbol_hierarchy));
								
						add_action(clone_operation, clone_action);
					}
					else
					{
						char clone_operation_name[256];
						strcpy(clone_operation_name, build_hierarchical_name(clone_operation->symbol_hierarchy));
						yyerrorf(0, 0, "Internal error: %s has no action %s", clone_operation_name, build_hierarchical_name(action->action_proto->symbol_hierarchy));
						exit(1);
					}
				}
			}
			else
			{
				delete_operation(clone_operation);
			}
		}
	
		for(child = node->child; child; child = child->sibling)
		{
			finalize_operation(child, operation);
		}
	}
}

void finalize(symbol_hierarchy_t *node)
{
	if(node->child)
	{
		symbol_hierarchy_t *child;
		operation_t *operation;

		for(operation = node->operation; operation; operation = operation->sibling)
		{
			finalize_operation(node, operation);
		}

		for(child = node->child; child; child = child->sibling)
		{
			finalize(child);
		}
	}
}

/** Counts the number of \n characters into a string
	@param s a C string
	@return the number of \n characters
*/
static unsigned int count_end_of_line(char *s)
{
	unsigned int count = 0;
	if(s)
	{
		do
		{
			if(*s == '\n') count++;
		} while(*(++s));
	}
	return count;
}

/** Initializes the emit buffer */
static void emit_init()
{
	emit_buffer_size = 1024;
	emit_buffer = (char *) malloc(emit_buffer_size);

	if(!emit_buffer)
	{
		yyerrorf(0, 0, "internal error! not enough memory");
		exit(-1);
	}
}

/** Release the emit buffer */
static void emit_release()
{
	emit_buffer_size = 0;
	if(emit_buffer) free(emit_buffer);
	emit_buffer = 0;
	if(emit_filename) free(emit_filename);
	emit_filename = 0;
}

/** Open an output file
	@param filename the name of the file
*/
static int emit_open_file(char *filename)
{
	if(emit_filename) free(emit_filename);
	emit_filename = strdup(filename);
	if(!emit_filename)
	{
		yyerrorf(0, 0, "internal error! not enough memory");
		exit(-1);
	}
	emit_lineno = 1;
	emit_file = fopen(filename, "w");
	if(!emit_file)
	{
		yyerrorf(0, 0, "error! can't open output file `%s'", filename);
		return 0;
	}
	return 1;
}

/** Close the output file */
static void emit_close_file()
{
	if(emit_file) fclose(emit_file);
}

/** Output source code into the output file
	Also generate #line in the output file to link the C compiler error to the original source code
	@param filename the name of the file where source code was found
	@param lineno the line number where source code was found
	@param format a C string with format specifier (like in printf), referenced arguments in the format string must follow
*/
static void emit_source_code(char *filename, unsigned int lineno, char *format, ...)
{
	va_list args1, args2;
	int length;

	va_start(args1, format);
	fprintf(emit_file, "\n#line %u \"%s\"\n", lineno, filename);
	emit_lineno+=2;
	length = vfprintf(emit_file, format, args1);

	if(length < 0)
	{
		yyerrorf(0, 0, "internal error! can't write into \"%s\"", emit_filename);
		exit(-1);
	}	

	if(length + 1 > emit_buffer_size)
	{
		emit_buffer_size = 2 * (length + 1);
		emit_buffer = (char *) realloc(emit_buffer, emit_buffer_size);
		if(!emit_buffer)
		{
			yyerrorf(0, 0, "internal error! not enough memory");
			exit(-1);
		}
	}
	va_end(args1);

	va_start(args2, format);
	vsprintf(emit_buffer, format, args2);
	emit_lineno += count_end_of_line(emit_buffer);

	fprintf(emit_file, "\n#line %u \"%s\"\n", emit_lineno + 2, emit_filename);
	emit_lineno+=2;
	va_end(args2);
}

/** Output a string into the output file
	@param format a C string with format specifier (like in printf), referenced arguments in the format string must follow
*/
static void emit(char *format, ...)
{
	va_list args1, args2;
	int length;

	va_start(args1, format);
	length = vfprintf(emit_file, format, args1);

	if(length < 0)
	{
		yyerrorf(0, 0, "internal error! can't write into \"%s\"", emit_filename);
		exit(-1);
	}

	if(length + 1 > emit_buffer_size)
	{
		emit_buffer_size = 2 * (length + 1);
		emit_buffer = (char *) realloc(emit_buffer, emit_buffer_size);
		if(!emit_buffer)
		{
			yyerrorf(0, 0, "internal error! not enough memory");
			exit(-1);
		}
	}
	va_end(args1);
	
	va_start(args2, format);
	vsprintf(emit_buffer, format, args2);
	emit_lineno += count_end_of_line(emit_buffer);
	va_end(args2);
}

/** Output a C string into the output file
	@param format a C string with format specifier (like in printf), referenced arguments in the format string must follow
*/
static void emit_c_string(char *s)
{
	int length;
	char *start, *current;

	fflush(emit_file);
	length = strlen(s);
	if(length + 1 > emit_buffer_size)
	{
		emit_buffer_size = 2 * (length + 1);
		emit_buffer = (char *) realloc(emit_buffer, emit_buffer_size);
		if(!emit_buffer)
		{
			yyerrorf(0, 0, "internal error! not enough memory");
			exit(-1);
		}
	}
	strcpy(emit_buffer, s);
	fprintf(emit_file, "\"");
	if(length > 0)
	{
		start = current = emit_buffer;
		do
		{
			if(*current == '\n')
			{
				*current = 0;
				fprintf(emit_file, "%s\\n", start);
				start = current + 1;
			}
			else
			{
				if(*current == '\"')
				{
					*current = 0;
					fprintf(emit_file, "%s\\\"", start);
					start = current + 1;
				}
				else
				{
					if(*current == '\\')
					{
						*current = 0;
						fprintf(emit_file, "%s\\\\", start);
						start = current + 1;
					}
					else
					{
						if(*current == '\t')
						{
							*current = 0;
							fprintf(emit_file, "%s\\t", start);
							start = current + 1;
						}
					}
				}
			}
		} while(++current, --length);
		if(start != current)
		{
			fprintf(emit_file, "%s", start);
		}
	}
	fprintf(emit_file, "\"");
}

symbol_hierarchy_t *find_common_prefix(symbol_hierarchy_t *a, symbol_hierarchy_t *b)
{
	if(a == b) return a;
	if(!a)
	{
		a = b;
		b = 0;
	}
	if(a)
	{
		do
		{
			if(a == b) return a;
			if(b)
			{
				symbol_hierarchy_t *t = b;
				do
				{
					if(a == t) return a;
				} while((t = t->parent) != 0);
			}
		} while((a = a->parent) != 0);
	}
	return 0;
}

static void emit_change_namespace(symbol_hierarchy_t *target_namespace)
{
	static symbol_hierarchy_t *last_namespace = 0;
	static char buffer[4096];
	int i;

	if(target_namespace != last_namespace)
	{
		symbol_hierarchy_t *common_namespace = find_common_prefix(last_namespace, target_namespace);

// 		printf("common prefix of %s", build_hierarchical_name(last_namespace));
// 		printf(" and %s", build_hierarchical_name(target_namespace));
// 		printf(" is %s\n", build_hierarchical_name(common_namespace));

		if(last_namespace && last_namespace != common_namespace)
		{
			symbol_hierarchy_t *namespace = last_namespace;
	
			char *p = &buffer[sizeof(buffer) - 1];
			*p-- = 0;
			*p-- = '\n';
			do
			{
				*p-- = ' ';
				*p-- = '}';
				namespace = namespace->parent;
			} while(namespace && namespace != common_namespace);
		
			emit("%s", p + 1);
		}

		if(target_namespace && target_namespace != common_namespace)
		{
			symbol_hierarchy_t *namespace = target_namespace;
			int i;
			char *p = &buffer[sizeof(buffer) - 1];
			*p-- = 0;
			*p-- = '\n';
			do
			{
				*p-- = ' ';
				*p-- = '{';
				*p-- = ' ';
				for(i = strlen(namespace->symbol->name) - 1; i >= 0; i--)
				{
					*p-- = namespace->symbol->name[i];
				}
				*p-- = ' ';
				*p-- = 'e';
				*p-- = 'c';
				*p-- = 'a';
				*p-- = 'p';
				*p-- = 's';
				*p-- = 'e';
				*p-- = 'm';
				*p-- = 'a';
				*p-- = 'n';
				namespace = namespace->parent;
			} while(namespace && namespace != common_namespace);
		
			emit("%s", p + 1);
		}
		last_namespace = target_namespace;
	}
}

static void emit_template_signature()
{
	if(!template_parameters) return;
		
	template_parameter_t *template_parameter;
		
	emit("template <");
		
	template_parameter = template_parameters->first_template_parameter;
	do
	{
		emit_source_code(
			template_parameter->c_type->filename->name,
			template_parameter->c_type->lineno,
			"\t%s",
			template_parameter->c_type->content
		);
		emit_source_code(
			template_parameter->c_symbol->filename->name,
			template_parameter->c_symbol->lineno,
			"\t%s",
			template_parameter->c_symbol->content
		);
		
		template_parameter = template_parameter->next;
				
		if(template_parameter) emit(",");
	} while(template_parameter);
	emit(">\n");
}

static void emit_template_abrev()
{
	if(!template_parameters) return;
		
	template_parameter_t *template_parameter;
		
	emit("<");
		
	template_parameter = template_parameters->first_template_parameter;
	do
	{
		emit_source_code(
			template_parameter->c_symbol->filename->name,
			template_parameter->c_symbol->lineno,
			"\t%s",
			template_parameter->c_symbol->content
		);
		
		template_parameter = template_parameter->next;
				
		if(template_parameter) emit(",");
	} while(template_parameter);
	emit(">");
}


char *capitalize(char *s)
{
	static char buffer[256];
	if(*s)
	{
		*buffer = (*s >= 'a' && *s <= 'z') ? *s + ('A' - 'a') : *s;
		strcpy(buffer + 1, s + 1);
	}
	else
	{
		*buffer = 0;
	}
	return buffer;
}

void generate_decoder_class_definition(symbol_hierarchy_t *node, char *addr_type, char *code_type)
{
	if(node->child)
	{
		symbol_hierarchy_t *child;
	
		emit_change_namespace(node);
		emit_template_signature();
		emit("class Operation;\n");
	
		emit("const unsigned int NUM_OPERATIONS_PER_PAGE = 4096;\n");
		emit_template_signature();
		emit("class DecodeMapPage\n");
		emit("{\n");
		emit("public:\n");
		emit("\tDecodeMapPage(%s key);\n", addr_type);
		emit("\t~DecodeMapPage();\n");
		emit("\t%s key;\n", addr_type);
		emit("\tDecodeMapPage *next;\n");
		emit("\tOperation");
		emit_template_abrev();
		emit(" *operation[NUM_OPERATIONS_PER_PAGE];\n");
		emit("};\n\n");
		
	
		emit_template_signature();
		emit("class DecodeTableEntry\n");
		emit("{\n");
		emit("public:\n");
		emit("\tDecodeTableEntry(CodeType opcode, CodeType opcode_mask, Operation");
		emit_template_abrev();
		emit(" *(*decode)(CodeType, %s));\n", addr_type);
		emit("\tCodeType opcode;\n");
		emit("\tCodeType opcode_mask;\n");
		emit("\tOperation");
		emit_template_abrev();
		emit(" *(*decode)(CodeType code, %s addr);\n", addr_type);
		emit("};\n");
	
		emit("const unsigned int NUM_DECODE_HASH_TABLE_ENTRIES = 4096;\n");
	
		emit_template_signature();
		emit("class Decoder\n");
		emit("{\n");
		emit("public:\n");
		emit("\tDecoder();\n");
		emit("\tvirtual ~Decoder();\n");
		emit("\n");
		emit("\tOperation");
		emit_template_abrev();
		emit("*NCDecode(%s addr);\n", addr_type);
		emit("\tOperation");
		emit_template_abrev();
		emit(" *NCDecode(%s addr, CodeType code);\n", addr_type);
		emit("\tOperation");
		emit_template_abrev();
		emit(" *Decode(%s addr);\n", addr_type);
		emit("\tOperation");
		emit_template_abrev();
		emit(" *Decode(%s addr, CodeType insn);\n", addr_type);
		emit("\tvirtual void Fetch(void *, %s, uint32_t) = 0;\n", addr_type);
		emit("\tvoid InvalidateDecodingCacheEntry(%s addr);\n", addr_type);
		emit("\tvoid InvalidateDecodingCache();\n\n");
		emit("\tvoid SetLittleEndian();\n");
		emit("\tvoid SetBigEndian();\n");
		emit("private:\n");
		emit("\tbool little_endian;\n");
		emit("\tvector<DecodeTableEntry");
		emit_template_abrev();		
		emit(" > decode_table;\n");
		emit("\tDecodeMapPage");
		emit_template_abrev();
		emit(" *mru_page;\n");
		emit("\tDecodeMapPage");
		emit_template_abrev();
		emit(" *decode_hash_table[NUM_DECODE_HASH_TABLE_ENTRIES];\n");
		emit("\tDecodeMapPage");
		emit_template_abrev();
		emit(" *FindPage(%s page_key)\n", addr_type);
		emit("#if defined(__GNUC__) && (__GNUC__ >= 3)\n");
		emit("\t__attribute__((always_inline))\n");
		emit("#endif\n");
		emit("\t;\n");
		emit("};\n\n");
	
		for(child = node->child; child; child = child->sibling)
		{
			generate_decoder_class_definition(child, addr_type, code_type);
		}
	}
}

void generate_operation_class_definition(symbol_hierarchy_t *node, char *addr_type, char *code_type, unsigned int word_size)
{
	if(node->child)
	{
		action_proto_t *action_proto;
		variable_t *variable;
		comment_t *comment;
		operation_t *operation;
		symbol_hierarchy_t *child;
	
		emit_change_namespace(node);
		emit_template_signature();
		emit("class Operation\n");
		emit("{\n");
		emit("public:\n");

		emit("\tOperation(CodeType code, %s addr, const char *name);\n", addr_type);
		emit("\tvirtual ~Operation();\n");
		emit("\tinline %s GetAddr() { return addr; }\n", addr_type);
		emit("\tinline %s GetEncoding() { return encoding; }\n", code_type);
		emit("\tinline const char *GetName() { return name; }\n");
	
		for(variable = variable_list.first_variable; variable; variable = variable->next)
		{
			emit_source_code(
			variable->c_type->filename->name,
			variable->c_type->lineno,
			"\t%s %s;",
			variable->c_type->content,
			variable->symbol->name);
		}

		for(action_proto = node->action_proto; action_proto; action_proto = action_proto->sibling)
		{
			if(action_proto->type == STATIC_ACTION_PROTO && action_proto->returns)
			{
				emit("\t%s %s_result;\n",
				action_proto->returns->content,
				action_proto->symbol_hierarchy->symbol->name);
			}
		}

		for(action_proto = node->action_proto; action_proto; action_proto = action_proto->sibling)
		{
			emit("\tvirtual ");
			if(action_proto->returns)
			{
				emit_source_code(
				action_proto->returns->filename->name,
				action_proto->returns->lineno,
				"\t%s",
				action_proto->returns->content);
			}
			else
			{
				emit("void");
			}
			
			emit(
			"\t%s(",
			action_proto->symbol_hierarchy->symbol->name);
	
			if(action_proto->params)
			{
				param_t *param;
				
				for(param = action_proto->params->first_param; param; param = param->next)
				{
					emit_source_code(
					param->c_type->filename->name,
					param->c_type->lineno,
					"\t%s",
					param->c_type->content);
					
					emit_source_code(
					param->c_symbol->filename->name,
					param->c_symbol->lineno,
					"\t%s",
					param->c_symbol->content);
					
					if(param->next) emit(",\n");
				}
			}
			
			emit("\t);\n");
		}

		emit("private:\n");

		emit("\tCodeType encoding;\n");
		emit("\t%s addr;\n", addr_type);
		emit("\tconst char *name;\n");

		emit("};\n\n");

		for(child = node->child; child; child = child->sibling)
		{
			generate_operation_class_definition(child, addr_type, code_type, word_size);
		}
	}
}


void generate_operation_implementation_class_definition(symbol_hierarchy_t *node, char *addr_type, char *code_type, unsigned int word_size)
{
	if(node->child)
	{
		symbol_hierarchy_t *child;
		action_proto_t *action_proto;
		action_t *action;
		variable_t *variable;
		comment_t *comment;
		operation_t *operation;
		bitfield_t *bitfield;

		emit_change_namespace(node);

		for(operation = node->operation; operation; operation = operation->sibling)
		{
			emit_template_signature();
			emit("class Op%s : public Operation", capitalize(operation->symbol_hierarchy->symbol->name));
			emit_template_abrev();
			emit("\n");

			emit("{\n");
			emit("public:\n");
			emit("\tOp%s(%s::CodeType code, %s addr);\n", capitalize(operation->symbol_hierarchy->symbol->name), build_hierarchical_name(namespace), addr_type);
			
			for(bitfield = operation->bitfield_list->first_bitfield; bitfield; bitfield = bitfield->next)
			{
				if(bitfield->type == bf_operand)
				{
					emit(
					"\t%s %s;\n",
					get_type_name(get_bitfield_type_size(bitfield->u.operand.size_modifier > bitfield->size ? bitfield->u.operand.size_modifier : bitfield->size, word_size), bitfield->u.operand.sext),
					bitfield->u.operand.symbol->name);
				}
			}
	
			if(operation->variable_list)
			{
				for(variable = operation->variable_list->first_variable; variable; variable = variable->next)
				{
					emit_source_code(
					variable->c_type->filename->name,
					variable->c_type->lineno,
					"\t\t\t%s %s;",
					variable->c_type->content,
					variable->symbol->name);
				}
			}
			
			for(action = operation->action_list; action; action = action->next)
			{
				action_proto_t *action_proto = action->action_proto;

				if(compare_symbol_hierarchy(action_proto->symbol_hierarchy->parent, node))
				{
					if(action_proto->comment_list)
					{
						for(comment = action_proto->comment_list->first_comment; comment; comment = comment->next)
						{
							emit("\t%s\n", comment->string);
						}
					}
				
					emit("\tvirtual\n");
	
					if(action_proto->returns)
					{
						emit_source_code(
						action_proto->returns->filename->name,
						action_proto->returns->lineno,
						"\t%s",
						action_proto->returns->content);
					}
					else
					{
						emit("\tvoid\n");
					}
					
					emit("\t%s(",
					action_proto->symbol_hierarchy->symbol->name);
			
					if(action_proto->params)
					{
						param_t *param;
				
						for(param = action_proto->params->first_param; param; param = param->next)
						{
							emit_source_code(
							param->c_type->filename->name,
							param->c_type->lineno,
							"\t%s",
							param->c_type->content);
					
							emit_source_code(
							param->c_symbol->filename->name,
							param->c_symbol->lineno,
							"\t%s",
							param->c_symbol->content);
					
							if(param->next) emit(",\n");
						}
						emit("\t");
					}
			
					emit(");\n");
				}
				
			}

			emit("private:\n");
	
			emit("};\n\n");	
		}

		for(child = node->child; child; child = child->sibling)
		{
			generate_operation_implementation_class_definition(child, addr_type, code_type, word_size);
		}
	}
}

void generate_operation_constructor(symbol_hierarchy_t *node, char *addr_type)
{
	if(node->child)
	{
		symbol_hierarchy_t *child;
		action_proto_t *action_proto;

		emit_change_namespace(node);
		emit_template_signature();
		emit("Operation");
		emit_template_abrev();
		emit("::Operation(CodeType code, %s addr, const char *name)\n", addr_type);

		emit("{\n");
		emit("\tthis->encoding = code;\n");
		emit("\tthis->addr = addr;\n");
		emit("\tthis->name = name;\n");

		emit("}\n");

		for(child = node->child; child; child = child->sibling)
		{
			generate_operation_constructor(child, addr_type);
		}
	}
}

void generate_operation_destructor(symbol_hierarchy_t *node)
{
	if(node->child)
	{
		symbol_hierarchy_t *child;

		action_proto_t *action_proto;

		emit_change_namespace(node);

		emit_template_signature();
		emit("Operation");
		emit_template_abrev();
		emit("::~Operation()\n");
		emit("{\n");
		for(action_proto = node->action_proto; action_proto; action_proto = action_proto->sibling)
		{
			if(action_proto->type == DESTRUCTOR_ACTION_PROTO)
			{
				emit(
				"\t%s();\n",
				action_proto->symbol_hierarchy->symbol->name);
			}
		}
		emit("}\n");

		for(child = node->child; child; child = child->sibling)
		{
			generate_operation_destructor(child);
		}
	}
}

void generate_default_actions(symbol_hierarchy_t *node)
{
	if(node->child)
	{
		symbol_hierarchy_t *child;
		action_proto_t *action_proto;
	
		emit_change_namespace(node);
		
		for(action_proto = node->action_proto; action_proto; action_proto = action_proto->sibling)
		{
			emit_template_signature();
			
			if(action_proto->returns)
			{
				emit_source_code(
				action_proto->returns->filename->name,
				action_proto->returns->lineno,
				"%s",
				action_proto->returns->content);
			}
			else
			{
				emit("\nvoid\n");
			}
	
			emit(
			"Operation");
			emit_template_abrev();
			emit("::%s(",
			action_proto->symbol_hierarchy->symbol->name);
	
			if(action_proto->params)
			{
				param_t *param;
				
				for(param = action_proto->params->first_param; param; param = param->next)
				{
					emit_source_code(
					param->c_type->filename->name,
					param->c_type->lineno,
					"\t%s",
					param->c_type->content);
					
					emit_source_code(
					param->c_symbol->filename->name,
					param->c_symbol->lineno,
					"\t%s",
					param->c_symbol->content);
					
					if(param->next) emit(",\n");
				}
			}
	
			emit(
			")\n{\n");

			emit_source_code(action_proto->filename->name,
			action_proto->default_source_code->lineno,
			"{%s}",
			action_proto->default_source_code->content);
	
			emit("}\n");
		}

		for(child = node->child; child; child = child->sibling)
		{
			generate_default_actions(child);
		}
	}
}

void generate_operation_member_methods(symbol_hierarchy_t *node, char *addr_type, char *code_type, unsigned int word_size)
{
	if(node->child)
	{
		symbol_hierarchy_t *child;
		action_proto_t *action_proto;
		variable_t *variable;
		comment_t *comment;
		operation_t *operation;
		bitfield_t *bitfield;
		action_t *action;
	
		emit_change_namespace(node);

		for(operation = node->operation; operation; operation = operation->sibling)
		{
			if(operation->comment_list)
			{
				for(comment = operation->comment_list->first_comment; comment; comment = comment->next)
				{
					emit(
					"%s\n",
					comment->string);
				}
			}
	
			for(action = operation->action_list; action; action = action->next)
			{
				action_proto_t *action_proto = action->action_proto;

				if(compare_symbol_hierarchy(action_proto->symbol_hierarchy->parent, node))
				{
					if(action->comment_list)
					{
						for(comment = action->comment_list->first_comment; comment; comment = comment->next)
						{
							emit(
							"%s\n",
							comment->string);
						}
					}
		
					emit_template_signature();
					
					if(action->action_proto->returns)
					{
						emit_source_code(
						action->action_proto->returns->filename->name,
						action->action_proto->returns->lineno,
						"%s",
						action->action_proto->returns->content);
					}
					else
					{
						emit("\nvoid\n");
					}
		
					emit(
					"Op%s",
					capitalize(operation->symbol_hierarchy->symbol->name));
					emit_template_abrev();
					emit(
					"::%s(",
					action->action_proto->symbol_hierarchy->symbol->name);
					
					if(action->action_proto->params)
					{
						param_t *param;
					
						for(param = action->action_proto->params->first_param; param; param = param->next)
						{
							emit_source_code(
							param->c_type->filename->name,
							param->c_type->lineno,
							"\t%s",
							param->c_type->content);
						
							emit_source_code(
							param->c_symbol->filename->name,
							param->c_symbol->lineno,
							"\t%s",
							param->c_symbol->content);
						
							if(param->next) emit(",\n");
						}
					}
		
					emit( ")\n{\n");
		
					emit_source_code(action->source_code->filename->name,
					action->source_code->lineno,
					"{%s}",
					action->source_code->content);
		
					emit("}\n");
				}
			}

			emit( "\n");	

			emit_template_signature();
			emit("static %s::Operation", build_hierarchical_name(node));
			emit_template_abrev();
			emit(" *DecodeOp%s(", capitalize(operation->symbol_hierarchy->symbol->name));
			emit("%s::CodeType code, %s addr)\n", build_hierarchical_name(namespace), addr_type);
			emit("{\n");
			emit("\treturn new Op%s", capitalize(operation->symbol_hierarchy->symbol->name));
			emit_template_abrev();
			emit("(code, addr);\n");
			emit("}\n\n");
		}
	
		for(child = node->child; child; child = child->sibling)
		{
			generate_operation_member_methods(child, addr_type, code_type, word_size);
		}
	}
}

void generate_operation_constructors(symbol_hierarchy_t *node, char *addr_type, char *code_type, unsigned int word_size)
{
	if(node->child)
	{
		symbol_hierarchy_t *child;
		action_proto_t *action_proto;
		variable_t *variable;
		comment_t *comment;
		operation_t *operation;
		bitfield_t *bitfield;
		action_t *action;
	
		emit_change_namespace(node);

		for(operation = node->operation; operation; operation = operation->sibling)
		{
			unsigned int shift = operation->bitfield_list->size;
			uint64_t mask;

			emit_template_signature();
			emit(
			"Op%s",
			capitalize(operation->symbol_hierarchy->symbol->name));
			emit_template_abrev();
			emit(
			"::Op%s(%s::CodeType code, %s addr) : Operation",
			capitalize(operation->symbol_hierarchy->symbol->name),
			build_hierarchical_name(namespace),
			addr_type);
			emit_template_abrev();
			emit(
			"(code, addr, \"%s\")\n",
			operation->symbol_hierarchy->symbol->name);
			emit("{\n");
			
			for(variable = variable_list.first_variable; variable; variable = variable->next)
			{
				if(variable->c_init)
				{
					emit(
					"\t%s = ",
					variable->symbol->name);
					emit_source_code(variable->c_init->filename->name, variable->c_init->lineno, "%s;\n", variable->c_init->content);
				}
			}
		
			if(operation->variable_list)
			{
				for(variable = operation->variable_list->first_variable; variable; variable = variable->next)
				{
					if(variable->c_init)
					{
						emit(
						"\t%s = ",
						variable->symbol->name);
						emit_source_code(variable->c_init->filename->name, variable->c_init->lineno, "%s;\n", variable->c_init->content);
					}
				}
			}
			
			for(bitfield = operation->bitfield_list->first_bitfield; bitfield; bitfield = bitfield->next)
			{
				shift -= bitfield->size;
				if(bitfield->type == bf_operand)
				{
					mask = (1ULL << bitfield->size) - 1;
	
					emit("\t%s = ", bitfield->u.operand.symbol->name);
	
					if(bitfield->u.operand.sext)
					{
						unsigned int type_size = get_bitfield_type_size(bitfield->u.operand.size_modifier > bitfield->size ? bitfield->u.operand.size_modifier : bitfield->size, word_size);
						emit(
						"(((((%s) code >> %u) & 0x%llx) << %u) >> %u)",
						get_type_name(type_size, 1),
						shift,
						mask,
						type_size - bitfield->size,
						type_size - bitfield->size);
					}
					else
					{
						emit(
						"((code >> %u) & 0x%llx)",
						shift,
						mask);
					}
	
					if(bitfield->u.operand.shift)
					{
						if(bitfield->u.operand.shift > 0)
						{
							// right shift
							emit(" >> %u;\n", bitfield->u.operand.shift);
						}
						else
						{
							// left shift
							emit(" << %u;\n", -bitfield->u.operand.shift);
						}
					}
					else
					{
						emit(";\n", bitfield->u.operand.shift);
					}
				}
			}
	
			emit("}\n\n");
		}
	
		for(child = node->child; child; child = child->sibling)
		{
			generate_operation_constructors(child, addr_type, code_type, word_size);
		}
	}
}

void generate_decoder(symbol_hierarchy_t *node, char *addr_type, char *code_type, unsigned int word_size, char *const_modifier, int little_endian, unsigned int code_size, unsigned int max_operation_size)
{
	if(node->child)
	{
		action_proto_t *action_proto;
		operation_t *operation;
		symbol_hierarchy_t *child;
	
		emit_change_namespace(node);
	
		emit_template_signature();
		emit("DecodeMapPage");
		emit_template_abrev();
		emit("::DecodeMapPage(%s key)\n", addr_type);
		emit("{\n");
		emit("\tthis->key = key;\n");
		emit("\tmemset(operation, 0, sizeof(operation));\n");
		emit("\tnext = 0;\n");
		emit("}\n\n");
	
		emit_template_signature();
		emit("DecodeMapPage");
		emit_template_abrev();
		emit("::~DecodeMapPage()\n");
		emit("{\n");
		emit("\tunsigned int i;\n");
		emit("\tfor(i = 0; i < NUM_OPERATIONS_PER_PAGE; i++)\n");
		emit("\t\tdelete operation[i];\n");
		emit("}\n\n");

		emit_template_signature();
		emit("\tDecodeTableEntry");
		emit_template_abrev();
		emit("::DecodeTableEntry(CodeType opcode, CodeType opcode_mask, Operation");
		emit_template_abrev();
		emit(" *(*decode)(CodeType, %s))\n", addr_type);
		emit("\t{\n");
		emit("\t\tthis->opcode = opcode;\n");
		emit("\t\tthis->opcode_mask = opcode_mask;\n");
		emit("\t\tthis->decode = decode;\n");
		emit("\t}\n\n");
	
		emit_template_signature();
		emit("Decoder");
		emit_template_abrev();
		emit("::Decoder()\n");
		emit("{\n");
		emit("\tlittle_endian = %s;\n", little_endian ? "true" : "false");
		emit("\tmru_page = 0;\n");
		emit("\tmemset(decode_hash_table, 0, sizeof(decode_hash_table));\n");
		for(operation = node->operation; operation; operation = operation->sibling)
		{
			if(operation->condition)
			{
				emit("if(");
				emit_source_code(
					operation->condition->filename->name,
					operation->condition->lineno,
					"%s",
					operation->condition->content);
				emit(") ");
			}
			emit("\tdecode_table.push_back(DecodeTableEntry");
			emit_template_abrev();
			emit("(0x%llx%s, 0x%llx%s, DecodeOp%s",
			operation->opcode,
			const_modifier,
			operation->opcode_mask,
			const_modifier,
			capitalize(operation->symbol_hierarchy->symbol->name));
			emit_template_abrev();
			emit("));\n");
		}
		emit("}\n\n");
	
		emit_template_signature();
		emit("Decoder");
		emit_template_abrev();
		emit("::~Decoder()\n");
		emit("{\n");
		emit("\tInvalidateDecodingCache();\n");
		emit("}\n\n");
	
		emit_template_signature();
		emit("Operation");
		emit_template_abrev();
		emit(" *Decoder");
		emit_template_abrev();
		emit("::NCDecode(%s addr)\n", addr_type);
		emit("{\n");
		emit("\tOperation");
		emit_template_abrev();
		emit(" *operation;\n");
		emit("\t");
		if(template_parameters) emit("typename ");
		emit("vector<DecodeTableEntry");
		emit_template_abrev();
		emit(" >::iterator iter;\n");
		emit("\tCodeType code;\n");
	
		emit("\tFetch(&code, addr, sizeof(code));\n");

		emit("#if BYTE_ORDER == LITTLE_ENDIAN\n");
		emit("\tif(!little_endian)\n");
		emit("#else\n");
		emit("\tif(little_endian)\n");
		emit("#endif\n");
		emit("\t{\n");

		switch(code_size)
		{
			case 2: emit("\t\tcode = ((code & 0x00ff) << 8) | ((code & 0xff00) >> 8);\n"); break;
			case 4: emit("\t\tcode = ((code & 0xff000000UL) >> 24) | ((code & 0x00ff0000UL) >> 8) | ((code & 0x0000ff00UL) << 8) | ((code & 0x000000ffUL) << 24);\n"); break;
			case 8: emit(
					"\t\tcode = ((code & 0xff00000000000000ULL) >> 56)\n"
					"\t\t     | ((code & 0x00ff000000000000ULL) >> 40)\n"
					"\t\t     | ((code & 0x0000ff0000000000ULL) >> 24)\n"
					"\t\t     | ((code & 0x000000ff00000000ULL) >> 8)\n"
					"\t\t     | ((code & 0x00000000ff000000ULL) << 8)\n"
					"\t\t     | ((code & 0x0000000000ff0000ULL) << 24)\n"
					"\t\t     | ((code & 0x000000000000ff00ULL) << 40)\n"
					"\t\t     | ((code & 0x00000000000000ffULL) << 56);\n"); break;
		}
		if(8 * code_size > max_operation_size)
		{
			emit("\tcode = code >> %u;\n", 8 * code_size - max_operation_size);
		}

		emit("\t}\n");

		emit("\tunsigned int count = decode_table.size();\n");
		emit("\tunsigned int i;\n");
		emit("\tfor(i = 0; i < count; i++)\n");
		emit("\t{\n");
		emit("\t\tif((code & decode_table[i].opcode_mask) == decode_table[i].opcode)\n");
		emit("\t\t{\n");
		emit("\t\t\toperation = decode_table[i].decode(code, addr);\n");

		for(action_proto = node->action_proto; action_proto; action_proto = action_proto->sibling)
		{
			if(action_proto->type == CONSTRUCTOR_ACTION_PROTO)
			{
				emit("\t\t\toperation->%s();\n", action_proto->symbol_hierarchy->symbol->name);
			}

			if(action_proto->type == STATIC_ACTION_PROTO)
			{
				emit( "\t\t\t");
				if(action_proto->returns)
				{
					emit(
					"operation->%s_result = ",
					action_proto->symbol_hierarchy->symbol->name);
				}
				emit("operation->%s();\n", action_proto->symbol_hierarchy->symbol->name);
			}
		}

		emit("\t\t\treturn operation;\n");
		emit("\t\t}\n");
		emit("\t}\n");
		
		emit("\toperation = new Operation");
		emit_template_abrev();
		emit("(code, addr, \"???\");\n");

		for(action_proto = node->action_proto; action_proto; action_proto = action_proto->sibling)
		{
			if(action_proto->type == CONSTRUCTOR_ACTION_PROTO)
			{
				emit("\toperation->%s();\n", action_proto->symbol_hierarchy->symbol->name);
			}

			if(action_proto->type == STATIC_ACTION_PROTO)
			{
				emit( "\t");
				if(action_proto->returns)
				{
					emit(
					"operation->%s_result = ",
					action_proto->symbol_hierarchy->symbol->name);
				}
				emit("operation->%s();\n", action_proto->symbol_hierarchy->symbol->name);
			}
		}

		emit("\treturn operation;\n");
		emit("}\n\n");



		emit_template_signature();
		emit("Operation");
		emit_template_abrev();
		emit(" *Decoder");
		emit_template_abrev();
		emit("::NCDecode(%s addr, CodeType code)\n", addr_type);
		emit("{\n");
		emit("\tOperation");
		emit_template_abrev();
		emit(" *operation;\n");
		emit("\t");
		if(template_parameters) emit("typename ");
		emit("vector<DecodeTableEntry");
		emit_template_abrev();
		emit(" >::iterator iter;\n");
	
		emit("\tunsigned int count = decode_table.size();\n");
		emit("\tunsigned int i;\n");
		emit("\tfor(i = 0; i < count; i++)\n");
		emit("\t{\n");
		emit("\t\tif((code & decode_table[i].opcode_mask) == decode_table[i].opcode)\n");
		emit("\t\t{\n");
		emit("\t\t\toperation = decode_table[i].decode(code, addr);\n");

		for(action_proto = node->action_proto; action_proto; action_proto = action_proto->sibling)
		{
			if(action_proto->type == CONSTRUCTOR_ACTION_PROTO)
			{
				emit("\t\t\toperation->%s();\n", action_proto->symbol_hierarchy->symbol->name);
			}

			if(action_proto->type == STATIC_ACTION_PROTO)
			{
				emit( "\t\t\t");
				if(action_proto->returns)
				{
					emit(
					"operation->%s_result = ",
					action_proto->symbol_hierarchy->symbol->name);
				}
				emit("operation->%s();\n", action_proto->symbol_hierarchy->symbol->name);
			}
		}

		emit("\t\t\treturn operation;\n");
		emit("\t\t}\n");
		emit("\t}\n");
		
		emit("\toperation = new Operation");
		emit_template_abrev();
		emit("(code, addr, \"???\");\n");

		for(action_proto = node->action_proto; action_proto; action_proto = action_proto->sibling)
		{
			if(action_proto->type == CONSTRUCTOR_ACTION_PROTO)
			{
				emit("\toperation->%s();\n", action_proto->symbol_hierarchy->symbol->name);
			}

			if(action_proto->type == STATIC_ACTION_PROTO)
			{
				emit( "\t");
				if(action_proto->returns)
				{
					emit(
					"operation->%s_result = ",
					action_proto->symbol_hierarchy->symbol->name);
				}
				emit("operation->%s();\n", action_proto->symbol_hierarchy->symbol->name);
			}
		}

		emit("\treturn operation;\n");
		emit("}\n\n");

	
		emit_template_signature();
		emit("void Decoder");
		emit_template_abrev();
		emit("::InvalidateDecodingCache()\n");
		emit("{\n");
		emit("\tuint32_t index;\n");
		emit("\tmru_page = 0;\n");
		emit("\tfor(index = 0; index < NUM_DECODE_HASH_TABLE_ENTRIES; index++)\n");
		emit("\t{\n");
		emit("\t\tDecodeMapPage");
		emit_template_abrev();
		emit(" *page, *next_page;\n");
			
		emit("\t\tpage = decode_hash_table[index];\n");
		emit("\t\tif(page)\n");
		emit("\t\t{\n");
		emit("\t\t\tdo\n");
		emit("\t\t\t{\n");
		emit("\t\t\tnext_page = page->next;\n");
		emit("\t\t\tdelete page;\n");
		emit("\t\t\tpage = next_page;\n");
		emit("\t\t\t} while(page);\n");
		emit("\t\tdecode_hash_table[index] = 0;\n");
		emit("\t\t}\n");
		emit("\t}\n");
		emit("}\n\n");
	
		emit_template_signature();
		emit("void Decoder");
		emit_template_abrev();
		emit("::InvalidateDecodingCacheEntry(%s addr)\n", addr_type);
		emit("{\n");
		emit("\t%s page_key = addr / %u / NUM_OPERATIONS_PER_PAGE;\n", addr_type, code_size);
		emit("\tif(mru_page && mru_page->key == page_key) mru_page = 0;\n");
		emit("\tuint32_t index = page_key %% NUM_DECODE_HASH_TABLE_ENTRIES; // hash the key\n");
		emit("\tDecodeMapPage");
		emit_template_abrev();
		emit(" *prev, *cur;\n");
		emit("\tcur = decode_hash_table[index];\n");
		emit("\tif(cur)\n");
		emit("\t{\n");
		emit("\t\tif(cur->key == page_key)\n");
		emit("\t\t{\n");
		emit("\t\t\tdecode_hash_table[index] = cur->next;\n");
		emit("\t\t\tdelete cur;\n");
		emit("\t\t\treturn;\n");
		emit("\t\t}\n");
		emit("\t\tprev = cur;\n");
		emit("\t\tcur = cur->next;\n");
		emit("\t\tif(cur)\n");
		emit("\t\t{\n");
		emit("\t\t\tdo\n");
		emit("\t\t\t{\n");
		emit("\t\t\t\tif(cur->key == page_key)\n");
		emit("\t\t\t\t{\n");
		emit("\t\t\t\t\tprev->next = cur->next;\n");
		emit("\t\t\t\t\tcur->next = 0;\n");
		emit("\t\t\t\t\tdelete cur;\n");
		emit("\t\t\t\t\treturn;\n");
		emit("\t\t\t\t}\n");
		emit("\t\t\t\tprev = cur;\n");
		emit("\t\t\t} while((cur = cur->next) != 0);\n");
		emit("\t\t}\n");
		emit("\t}\n");
		emit("}\n\n");

		emit_template_signature();
		emit("inline DecodeMapPage");
		emit_template_abrev();
		emit(" *Decoder");
		emit_template_abrev();
		emit("::FindPage(%s page_key)\n", addr_type);
		emit("{\n");
		emit("\tif(mru_page && mru_page->key == page_key) return mru_page;\n");
		emit("\tuint32_t index = page_key %% NUM_DECODE_HASH_TABLE_ENTRIES; // hash the key\n");
		emit("\tDecodeMapPage");
		emit_template_abrev();
		emit(" *prev, *cur;\n");
		emit("\tcur = decode_hash_table[index];\n");
		emit("\tif(cur)\n");
		emit("\t{\n");
		emit("\t\tif(cur->key == page_key)\n");
		emit("\t\t{\n");
		emit("\t\t\tmru_page = cur;\n");
		emit("\t\t\treturn cur;\n");
		emit("\t\t}\n");
		emit("\t\tprev = cur;\n");
		emit("\t\tcur = cur->next;\n");
		emit("\t\tif(cur)\n");
		emit("\t\t{\n");
		emit("\t\t\tdo\n");
		emit("\t\t\t{\n");
		emit("\t\t\t\tif(cur->key == page_key)\n");
		emit("\t\t\t\t{\n");
		emit("\t\t\t\t\tprev->next = cur->next;\n");
		emit("\t\t\t\t\tcur->next= decode_hash_table[index];\n");
		emit("\t\t\t\t\tdecode_hash_table[index] = cur;\n");
		emit("\t\t\t\t\tmru_page = cur;\n");
		emit("\t\t\t\t\treturn cur;\n");
		emit("\t\t\t\t}\n");
		emit("\t\t\t\tprev = cur;\n");
		emit("\t\t\t} while((cur = cur->next) != 0);\n");
		emit("\t\t}\n");
		emit("\t}\n");
		emit("\treturn 0;\n");
		emit("}\n\n");



	
		emit_template_signature();
		emit("Operation");
		emit_template_abrev();
		emit(" *Decoder");
		emit_template_abrev();
		emit("::Decode(%s addr)\n", addr_type);
		emit("{\n");
		emit("\tOperation");
		emit_template_abrev();
		emit(" *operation;\n");
		emit("\t%s page_key = addr / %u / NUM_OPERATIONS_PER_PAGE;\n", addr_type, code_size);
		emit("\tDecodeMapPage");
		emit_template_abrev();
		emit(" *page;\n");
		emit("\tpage = FindPage(page_key);\n");
		emit("\tif(!page)\n");
		emit("\t{\n");
	//	emit("\t\t\tfprintf(stderr, \"page miss at 0x%%08x\\n\", addr);\n");
		emit("\t\tpage = new DecodeMapPage");
		emit_template_abrev();
		emit("(page_key);\n");
		emit("\t\tuint32_t index = page_key %% NUM_DECODE_HASH_TABLE_ENTRIES; // hash the key\n");
		emit("\t\tpage->next = decode_hash_table[index];\n");
		emit("\t\tdecode_hash_table[index] = page;\n");
		emit("\t\tmru_page = page;\n");
		emit("\t}\n");
		emit("\toperation = page->operation[(addr / %u) & (NUM_OPERATIONS_PER_PAGE - 1)];\n", code_size);
		emit("\tif(operation)\n");
		emit("\t{\n");
	//	emit("\t\tfprintf(stderr, \"hit at 0x%%08x\\n\", addr);\n");
		emit("\t\treturn operation;\n");
		emit("\t}\n");
	//	emit("\tfprintf(stderr, \"miss at 0x%%08x\\n\", addr);\n");
		emit("\toperation = NCDecode(addr);\n");
		emit("\tpage->operation[(addr / %u) & (NUM_OPERATIONS_PER_PAGE - 1)] = operation;\n", code_size);
		emit("\treturn operation;\n");
		emit("}\n\n");

		emit_template_signature();
		emit("Operation");
		emit_template_abrev();
		emit(" *Decoder");
		emit_template_abrev();
		emit("::Decode(%s addr, CodeType insn)\n", addr_type);
		emit("{\n");
		emit("\tOperation");
		emit_template_abrev();
		emit(" *operation;\n");
		emit("\t%s page_key = addr / %u / NUM_OPERATIONS_PER_PAGE;\n", addr_type, code_size);
		emit("\tDecodeMapPage");
		emit_template_abrev();
		emit(" *page;\n");
		emit("\tpage = FindPage(page_key);\n");
		emit("\tif(!page)\n");
		emit("\t{\n");
	//	emit("\t\t\tfprintf(stderr, \"page miss at 0x%%08x\\n\", addr);\n");
		emit("\t\tpage = new DecodeMapPage");
		emit_template_abrev();
		emit(" (page_key);\n");
		emit("\t\tuint32_t index = page_key %% NUM_DECODE_HASH_TABLE_ENTRIES; // hash the key\n");
		emit("\t\tpage->next = decode_hash_table[index];\n");
		emit("\t\tdecode_hash_table[index] = page;\n");
		emit("\t\tmru_page = page;\n");
		emit("\t}\n");
		emit("\toperation = page->operation[(addr / %u) & (NUM_OPERATIONS_PER_PAGE - 1)];\n", code_size);
		emit("\tif(operation)\n");
		emit("\t{\n");
	//	emit("\t\tfprintf(stderr, \"hit at 0x%%08x\\n\", addr);\n");
		emit("\t\tif(operation->GetEncoding() == insn && operation->GetAddr() == addr)\n");
		emit("\t\t\treturn operation;\n");
		emit("\t\tdelete operation;\n");
		emit("\t}\n");
	//	emit("\tfprintf(stderr, \"miss at 0x%%08x\\n\", addr);\n");
		emit("\toperation = NCDecode(addr, insn);\n");
		emit("\tpage->operation[(addr / %u) & (NUM_OPERATIONS_PER_PAGE - 1)] = operation;\n", code_size);
		emit("\treturn operation;\n");
		emit("}\n\n");

		emit_template_signature();
		emit("void Decoder");
		emit_template_abrev();
		emit("::SetLittleEndian()\n");
		emit("{\n");
		emit("\tlittle_endian = true;\n");
		emit("}\n\n");

		emit_template_signature();
		emit("void Decoder");
		emit_template_abrev();
		emit("::SetBigEndian()\n");
		emit("{\n");
		emit("\tlittle_endian = false;\n");
		emit("}\n\n");

		for(child = node->child; child; child = child->sibling)
		{
			generate_decoder(child, addr_type, code_type, word_size, const_modifier, little_endian, code_size, max_operation_size);
		}
	}
}

/** Generates one C source file and one C header
	@param output a C string containing the name of the output filenames without the file name extension
	@param word_size define the minimum word size to hold the operand bit field,
	if zero uses the smallest type which hold the operand bit field
	@return non-zero if no error occurs during generation
*/
int generate_iss(char *output, unsigned int word_size, unsigned int addr_size)
{
	comment_t *comment;
	operation_t *operation;
	bitfield_t *bitfield;
	action_t *action;
	action_proto_t *action_proto;
	source_code_t *source_code;
	variable_t *variable;
	unsigned int max_operation_size = get_max_operation_size();
	int code_size;
	char *addr_type = address_type ? address_type->content : DEFAULT_ADDRESS_TYPE;
	char code_type[256];
	char const_modifier[256];
	char filename[256];
	unsigned int i;

	finalize(namespace);

	emit_init();

	fprintf(stderr, "Instruction Set Encoding: %s\n", little_endian ? "little-endian" : "big-endian");
	sprintf(code_type, "%s", get_type_name(max_operation_size, 0));
	sprintf(const_modifier, "%s", get_const_modifier_name(max_operation_size, 0));
	code_size = get_code_type_size(max_operation_size);

	sprintf(filename, "%s.hh", output);
	if(!emit_open_file(filename))
	{
		emit_release();
		return 0;
	}

	emit("/*** File generated by %s version %s: DO NOT MODIFY ***/\n", GENISSLIB, VERSION);

	emit("#ifndef __%s_%s_HH__\n", escape_preprocessor_token( output ), build_preprocessor_hierarchical_name(namespace));
	emit("#define __%s_%s_HH__\n", escape_preprocessor_token( output ), build_preprocessor_hierarchical_name(namespace));
	
	emit(
	"#ifndef __STDC_FORMAT_MACROS\n"
	"#define __STDC_FORMAT_MACROS\n"
	"#endif\n\n");
	
	emit("#include <vector>\n");
	emit("#include <inttypes.h>\n");

	emit_change_namespace(namespace);
	emit("using namespace std;\n");
	emit("typedef %s CodeType;\n\n", code_type);

	generate_decoder_class_definition(namespace, addr_type, code_type);
	emit_change_namespace(0);

	for(source_code = source_code_list; source_code; source_code = source_code->next)
	{
		if(source_code->global == sc_decl)
		{
			emit_source_code(source_code->filename->name, source_code->lineno, "%s", source_code->content);
		}
	}

	emit("#ifndef ROTL\n");
	emit("#define ROTL(v, n) (((v) << (n)) | (((v) >> ((sizeof(v) * 8) - (n))) & ((1 << (n)) - 1)))\n");
	emit("#endif\n");
	emit("#ifndef ROTR\n");
	emit("#define ROTR(v, n) (((v) >> (n)) | (((v) & ((1 << (n)) - 1)) << ((sizeof(v) * 8) - (n))))\n");
	emit("#endif\n");
	emit("#ifndef SEXT\n");
	emit("#define SEXT(m, n, v) (((int##m##_t) (v) << (n)) >> (n))\n");
	emit("#endif\n");

	generate_operation_class_definition(namespace, addr_type, code_type, word_size);
	emit_change_namespace(0);	



	emit("#endif\n");

	emit_close_file();

	sprintf(filename, "%s.%s", output, template_parameters ? "tcc" : "cc");

	if(!emit_open_file(filename))
	{
		emit_release();
		return 0;
	}

	emit(
	"#include \"%s.hh\"\n"
	"#include <stdlib.h>\n"
	"#include <stdio.h>\n\n"
	"#include <string.h>\n\n"
	, output);

	for(source_code = source_code_list; source_code; source_code = source_code->next)
	{
		if(source_code->global == sc_impl)
		{
			emit_source_code(source_code->filename->name, source_code->lineno, "%s", source_code->content);
		}
	}

	generate_operation_constructor(namespace, addr_type);
	generate_operation_destructor(namespace);
	generate_default_actions(namespace);

	generate_operation_implementation_class_definition(namespace, addr_type, code_type, word_size);
	generate_operation_member_methods(namespace, addr_type, code_type, word_size);
	generate_operation_constructors(namespace, addr_type, code_type, word_size);
	generate_decoder(namespace, addr_type, code_type, word_size, const_modifier, little_endian, code_size, max_operation_size);

	emit_change_namespace(0);

	emit_close_file();
	emit_release();

	return 1;
}

/** Initialize all global lists and buffers */
void initialize()
{
	filename_list = 0;
	source_code_list = 0;
	last_source_code = 0;
	operation_list = 0;
	last_operation = 0;
	action_proto_list = 0;
	last_action_proto = 0;
	comment_list = 0;
	symbol_list = 0;
	variable_list.first_variable = 0;
	variable_list.last_variable = 0;
	group_list = 0;
	last_group = 0;
	string_buffer = 0;
	current_char = 0;
	string_buffer_size = 0;
	emit_file = 0;
	emit_buffer_size = 0;
	emit_buffer = 0;
	emit_filename = 0;
	emit_lineno = 1;

	init_string_buffer();
}

/** Release all memory resources used during parsing and source code generation */
void cleanup()
{
	delete_operations();
	delete_action_protos();
	delete_source_codes();
	delete_string_buffer();
	delete_symbols();
	delete_include_stack();
	delete_filenames();
	delete_search_dirs();
	delete_variables();
	if(comment_list) delete_comment_list(comment_list);
	emit_release();

	filename_list = 0;
	source_code_list = 0;
	last_source_code = 0;
	operation_list = 0;
	last_operation = 0;
	action_proto_list = 0;
	last_action_proto = 0;
	comment_list = 0;
	symbol_list = 0;
	variable_list.first_variable = 0;
	variable_list.last_variable = 0;
	group_list = 0;
	last_group = 0;
	string_buffer = 0;
	current_char = 0;
	string_buffer_size = 0;
	emit_file = 0;
	emit_buffer_size = 0;
	emit_buffer = 0;
	emit_filename = 0;
	emit_lineno = 1;
	yyin = 0;
	current_filename = 0;
}

void version()
{
	fprintf(stderr,
	"       %s v%s\n"
	"built date : %s\n"
	"copyright  : %s\n"
	"author     : %s\n"
	"email      : %s\n",
	GENISSLIB, VERSION, __DATE__, COPYRIGHT, AUTHOR, EMAIL);
}

void usage(char *genisslib_name)
{
	fprintf(stderr, "usage: %s [-I <directory name>] [-o <output>] [-w <word_size>] [-v] [-E <output file name>] <input file name>\n"
	"\t-I\tinclude a directory into the search path for includes\n"
	"\t-o\tOutputs the instruction set simulator source code into <output>.h and <output>.c\n"
	"\t\tThe default output is \"%s\", so the default output files are %s.hh and %s.cc/%s.tcc.\n"
	"\t-w\tUses <word size> as the minimum bit size for holding an operand bit field\n"
	"\t-v\tDisplays the version of %s\n"
	"\t-E\tExpands the file <input file name> into <output file name>\n",
	genisslib_name,
	DEFAULT_OUTPUT,
	DEFAULT_OUTPUT,
	DEFAULT_OUTPUT,
	DEFAULT_OUTPUT,
	GENISSLIB);
}

/** The entry point of the program */
int main(int argc, char *argv[])
{
	int i;
	int error = 0;
	int got_version = 0;
	int got_expand = 0;
	char *expand_output_filename = 0;
	unsigned int word_size = 0;
	unsigned int addr_size = 32;
	char *output = 0;
	char *input_filename = 0;

	for(i = 1; i < argc; i++)
	{
		if(strcmp(argv[i], "-o") == 0)
		{
			if(++i >= argc)
			{
				error = 1;
				break;
			}
			output = argv[i];
		}
		else if(strcmp(argv[i], "-w") == 0)
		{
			if(++i >= argc)
			{
				error = 1;
				break;
			}
			word_size = atoi(argv[i]);
		}
		else if(strcmp(argv[i], "-v") == 0)
		{
			got_version = 1;
		}
		else if(strcmp(argv[i], "-E") == 0)
		{
			if(++i >= argc)
			{
				error = 1;
				break;
			}
			got_expand = 1;
			expand_output_filename = argv[i];
		}
		else if(strcmp(argv[i], "-a") == 0)
		{
			if(++i >= argc)
			{
				error = 1;
				break;
			}
			addr_size = atoi(argv[i]);
		}
		else if(strncmp(argv[i], "-I", 2) == 0)
		{
			search_dir_t *search_dir = create_search_dir(argv[i] + 2);
			add_search_dir(search_dir);
		}
		else
		{
			input_filename = argv[i++];
			break;
		}
	}

	if(got_version)
	{
		version();
		return 0;
	}

	if(!output)
	{
		output = DEFAULT_OUTPUT;
		fprintf(stderr, "Warning! missing output on command line\n");
		fprintf(stderr, "using `%s' as output\n", output);
	}

	if(!input_filename)
	{
		error = 1;
		fprintf(stderr, "error! missing input file name on command line\n");
	}

	if(i > argc)
	{
		error = 1;
		fprintf(stderr, "error! too many arguments on command line\n");
	}

	if(!error && i == argc && output && input_filename)
	{
		if(open_file(input_filename))
		{
			initialize();
#if 0
            // This code is only for testing the lexical analyzer
			{
				int c;

				while((c = yylex()) != 0)
				{
					printf("%s\n", get_token_name(c));
				}
#endif
			error = yyparse();
			if(!error && sanity_checks())
			{
				generate_iss(output, word_size, addr_size);
			}
			else
			{
				fprintf(stderr, "compilation aborted.\n");
			}
			if(got_expand && expand_output_filename)
			{
				FILE *expand_output_file = fopen(expand_output_filename, "wt");

				if(expand_output_file)
				{
					dump(expand_output_file);
					fclose(expand_output_file);
				}
				else
				{
					error = 1;
					fprintf(stderr, "Can't open output file `%s'\n", expand_output_filename);
				}
			}
			if(yyin)
			{
				fclose(yyin);
				yyin = 0;
			}
			cleanup();
		}
		else
		{
			error = 1;
		}
	}
	else
	{
		error = 1;
		usage(argv[0]);
	}
	return error;
}
