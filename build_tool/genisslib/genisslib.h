/***************************************************************************
          GenISSLib.h  -  Instruction Set Simulator Library Generator
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

#ifndef __GENISSLIB_H__
#define __GENISSLIB_H__

#include <stdio.h>
#include <inttypes.h>

/** A filename object */
typedef struct filename_t
{
	char *name;						/**< A C-String */
	struct filename_t *next;
} filename_t;

/** A Search directory object */
typedef struct search_dir_t
{
	char *path;
	struct search_dir_t *next;
} search_dir_t;

/** A C/C++ comment object */
typedef struct comment_t
{
	char *string;					/**< A C-String */
	filename_t *filename;			/**< A filename */
	unsigned int lineno;			/**< The line number */
	struct comment_t *prev;
	struct comment_t *next;
} comment_t;

/** A list of C/C++ comment object */
typedef struct comment_list_t
{
	comment_t *first_comment;
	comment_t *last_comment;
} comment_list_t;

/** A source code type */
typedef enum sc_type_enum { sc_notglobal, sc_decl, sc_impl } sc_type_t;

/** A C source code object */
typedef struct source_code_t
{
	char *content;					/**< the string containing the C source code */
	sc_type_t global;						/**< non-zero if it is global */
	filename_t *filename;			/**< the filename object where was found the C source code */
	int lineno;						/**< the line number where was found the C source code */
	struct source_code_t *next;	/**< the next source code object */
} source_code_t;

typedef struct template_parameter_t
{
	source_code_t *c_type;
	source_code_t *c_symbol;
	
	struct template_parameter_t *next;
} template_parameter_t;

typedef struct template_parameter_list_t
{
	template_parameter_t *first_template_parameter;
	template_parameter_t *last_template_parameter;
} template_parameter_list_t;

/** A symbol object */
typedef struct symbol_t
{
	char *name;					/**< A C-String */
	struct symbol_t *next;
} symbol_t;

struct operation_t;
struct action_proto_t;

/** A symbol hierarchy object (in fact it's a tree of symbol) */
typedef struct symbol_hierarchy_t
{
	struct symbol_t *symbol;
	struct symbol_hierarchy_t *parent;
	struct symbol_hierarchy_t *next;
	struct symbol_hierarchy_t *child;
	struct symbol_hierarchy_t *sibling;
	struct operation_t *operation;
	struct action_proto_t *action_proto;
} symbol_hierarchy_t;

/** An action prototype type */
typedef enum action_proto_type_t {
	CONSTRUCTOR_ACTION_PROTO,	/* a constructor */
	DESTRUCTOR_ACTION_PROTO,	/* a destructor */
	STATIC_ACTION_PROTO,		/* a static action prototype */
	COMMON_ACTION_PROTO			/* a common action prototype */
} action_proto_type_t;

/** An action parameters object */
typedef struct param_t
{
	source_code_t *c_type;				/**< The C type of the parameter */
	source_code_t *c_symbol;			/**< The C symbol of the parameter */
	struct param_t *prev;
	struct param_t *next;
} param_t;

/** A parameter list object */
typedef struct param_list_t
{
	param_t *first_param;
	param_t *last_param;
} param_list_t;

/** An action prototype object */
typedef struct action_proto_t
{
	action_proto_type_t type;			/**< The type of the action prototype */
	symbol_hierarchy_t *symbol_hierarchy;	/**< The associated symbol hierarchy of the action prototype */
	source_code_t *returns;				/**< The C return type of the action */
	param_list_t *params;				/**< The C parameters of the action */
	source_code_t *default_source_code;	/**< The default C implementation of the action */
	comment_list_t *comment_list;		/**< The list of the C comment associated with the action prototype */
	filename_t *filename;				/**< the filename where the action prototype was declared */
	int lineno;							/**< the line number where the action prototype was declared */
	struct action_proto_t *prev;
	struct action_proto_t *next;
	struct action_proto_t *sibling;
} action_proto_t;

/** An action implementation object */
typedef struct action_t
{
	struct operation_t *operation;	/**< The associated operation  */
	action_proto_t *action_proto;		/**< The associated action prototype */
	source_code_t *source_code;			/**< The C implementation of the action */
	comment_list_t *comment_list;		/**< The list of the C comment associated with the action */
	filename_t *filename;				/**< the filename where the action was declared */
	int lineno;							/**< the line number where the action was declared */
	struct action_t *base;
	struct action_t *prev;
	struct action_t *next;
} action_t;

/** bitfield type */
typedef enum bf_type_enum { bf_opcode, bf_operand, bf_dontcare } bf_type_t;

/** A bitfield object, either an opcode bitfield or an operand bitfield */
typedef struct bitfield_t
{
	bf_type_t type;			/**< the type of the bitfield (opcode or operand) */
	unsigned int size;		/**< the number of bits of the bitfield */

	/** an union */
	union
	{
		/** an opcode */
		struct
		{
			unsigned int value;	/**< the value for an opcode bit field */
		} opcode;

		/** an operand */
		struct
		{
			symbol_t *symbol;			/**< the symbol of an operand bit field */
			int sext;					/**< non-zero if a sign extension must be performed */
			unsigned int size_modifier;	/**< define the minimum bit count to hold the operand bit field */
			int shift;					/**< shift amount (positive is right, negative is left) */
		} operand;
	} u;
	struct bitfield_t *next;
} bitfield_t;

/** A bit field list object */
typedef struct bitfield_list_t
{
	unsigned int size;			/**< the total number of bits in the bit fields */
	bitfield_t *first_bitfield;
	bitfield_t *last_bitfield;
} bitfield_list_t;

/** A variable object */
typedef struct variable_t
{
	symbol_t *symbol;			/**< the symbol object representing the variable */
	source_code_t *c_type;		/**< the C type of the variable */
	source_code_t *c_init;		/**< the C expression used to initialized the variable */
	struct variable_t *next;
} variable_t;

/** A variable list object */
typedef struct variable_list_t
{
	variable_t *first_variable;
	variable_t *last_variable;
} variable_list_t;

/** An operation object */
typedef struct operation_t
{
	symbol_hierarchy_t *symbol_hierarchy;	/**< The associated symbol hierarchy */
	bitfield_list_t *bitfield_list;	/**< The bit field list of the operation */
	action_t *action_list;			/**< The list of actions of the operation */
	action_t *last_action;			/**< The last action of the operation */
	unsigned int size;				/**< Number of bits of the operation */
	uint64_t opcode_mask;			/**< the opcode mask */
	uint64_t opcode;				/**< the opcode */
	filename_t *filename;			/**< the filename where the operation was declared */
	int lineno;						/**< the line number where the operation was declared */
	comment_list_t *comment_list;	/**< The list of the C comment associated with the operation */
	variable_list_t *variable_list;	/**< The list of variables associated with the operation */
	source_code_t *condition;
	struct operation_t *base;
	struct operation_t *prev;
	struct operation_t *next;
	struct operation_t *sibling;
} operation_t;

/** A node of an operation list
	This is needed for the groups */
typedef struct operation_node_t
{
	operation_t *operation;			/**< The operation object contained by the node */
	struct operation_node_t *next;
} operation_node_t;

/** An operation list object */
typedef struct operation_node_list_t
{
	operation_node_t *first_operation_node;
	operation_node_t *last_operation_node;
} operation_node_list_t;

/** A group object */
typedef struct group_t
{
	symbol_hierarchy_t *symbol_hierarchy;	/**< a symbol hierarchy object representing the group */
	operation_node_list_t *operation_node_list;	/**< an operation node list object containing the operations of the group */
	filename_t *filename;					/**< the filename where the group was declared */
	int lineno;								/**< the line number where the group was declared */
	struct group_t *next;
} group_t;

/** The type of the token, either terminal or not terminal */
typedef union
{
	char *string;							/**< C-String */
	unsigned int integer;					/**< An integer value */
	symbol_t *symbol;						/**< A symbol */
	source_code_t *source_code;				/**< A C source code */
	operation_t *operation;					/**< An operation */
	action_proto_t *action_proto;			/**< An action prototype */
	action_t *action;						/**< An action */
	bitfield_t *bitfield;					/**< A bit field */
	bitfield_list_t *bitfield_list;			/**< A bit field list */
	action_proto_type_t action_proto_type;	/**< the type of an action prototype */
	int sext;								/**< non-zero if a sign extension must be performed */
	unsigned int size_modifier;				/**< define the minimum bit count to hold an operand bit field */
	int shift;								/**< define the shift to perform on an operand bitfield */
	variable_t *variable;					/**< A variable object */
	variable_list_t *variable_list;			/**< A variable list object */
	operation_node_t *operation_node;		/**< An operation node object */
	operation_node_list_t *operation_node_list;		/**< An operation node list object */
	group_t *group;							/**< A group object */
	param_list_t *param_list;				/**< A parameter list object */
	param_t *param;							/**< A parameter object */
	symbol_hierarchy_t *symbol_hierarchy;	/**< A symbol hierarchy object */
	template_parameter_t *template_parameter;
	template_parameter_list_t *template_parameter_list;
} yylval_t;

/* define the type of yylval */
#define YYSTYPE yylval_t

/** yylval is owned by the parser generated by YACC or bison */
extern YYSTYPE yylval;

/** Define all global lists */
extern source_code_t *source_code_list;
extern source_code_t *last_source_code;
extern operation_t *operation_list;
extern operation_t *last_operation;
extern action_proto_t *action_proto_list;
extern action_proto_t *last_action_proto;
extern comment_list_t *comment_list;
extern variable_list_t variable_list;
extern group_t *group_list;
extern search_dir_t *search_dir_list;
extern template_parameter_list_t *template_parameters;

extern int get_token(char *name);
extern char *get_char_name(char c);
extern char *get_token_name(int token);

extern comment_t *create_comment(char *string, filename_t *filename, int lineno);
extern comment_list_t *create_comment_list(comment_t *comment);
extern void add_comment(comment_list_t *comment_list, comment_t *comment);
extern filename_t *create_filename(char *name);
extern search_dir_t *create_search_dir(char *path);
extern bitfield_list_t *create_bitfield_list(bitfield_t *bitfield);
extern operation_t *create_operation(symbol_hierarchy_t *symbol_hierarchy, bitfield_list_t *bitfield_list, comment_list_t *comment_list, variable_list_t *variable_list, operation_t *base_operation, source_code_t *op_condition, filename_t *filename, int lineno);
extern action_t *create_action(operation_t *operation, action_proto_t *action_proto, source_code_t *source_code, comment_list_t *comment_list, filename_t *filename, int lineno);
extern action_proto_t *create_action_proto(action_proto_type_t type, symbol_hierarchy_t *symbol_hierarchy, source_code_t *returns, param_list_t *params, source_code_t *default_source_code, comment_list_t *comment_list, filename_t *filename, int lineno);
extern bitfield_t *create_opcode_bitfield(unsigned int size, unsigned int value);
extern bitfield_t *create_operand_bitfield(unsigned int size, symbol_t *symbol, int shift, int sext, unsigned int size_modifier);
extern bitfield_t *create_dontcare_bitfield(unsigned int size);
extern source_code_t *create_source_code(char *source_code_string, filename_t *filename, int lineno);
extern variable_t *create_variable(symbol_t *symbol, source_code_t *c_type, source_code_t *c_init);
extern variable_list_t *create_variable_list(variable_t *variable);
extern operation_node_t *create_operation_node(operation_t *operation);
extern operation_node_list_t *create_operation_node_list(operation_node_t *operation);
extern group_t *create_group(symbol_hierarchy_t *symbol_hierarchy, operation_node_list_t *operation_node_list, filename_t *filename, int lineno);
extern param_t *create_param(source_code_t *c_type, source_code_t *c_symbol);
extern param_list_t *create_param_list(param_t *param);
extern template_parameter_t *create_template_parameter(source_code_t *c_type, source_code_t *c_symbol);
extern template_parameter_list_t *create_template_parameter_list(template_parameter_t *template_parameter);

extern symbol_hierarchy_t *get_symbol_hierarchy(symbol_hierarchy_t *namespace, symbol_t *symbol);
extern symbol_hierarchy_t *get_symbol_hierarchy2(symbol_hierarchy_t *symbol_hierarchy, symbol_t *symbol);
extern char *build_hierarchical_name(symbol_hierarchy_t *symbol_hierarchy);
extern int compare_symbol_hierarchy(symbol_hierarchy_t *a, symbol_hierarchy_t *b);

extern uint64_t create_opcode_mask(bitfield_list_t *bitfield_list);
extern uint64_t create_opcode(bitfield_list_t *bitfield_list);

extern bitfield_list_t *dup_bitfield_list(bitfield_list_t *bitfield_list);
extern bitfield_t *dup_bitfield(bitfield_t *bitfield);
extern comment_list_t *dup_comment_list(comment_list_t *comment_list);
extern comment_t *dup_comment(comment_t *comment);
extern variable_list_t *dup_variable_list(variable_list_t *variable_list);
extern variable_t *dup_variable(variable_t *variable);

extern void delete_comment(comment_t *comment);
extern void delete_comment_list(comment_list_t *comment_list);
extern void delete_operation(operation_t *operation);
extern void delete_action(action_t *action);
extern void delete_action_proto(action_proto_t *action_proto);
extern void delete_bitfield(bitfield_t *bitfield);
extern void delete_source_code(source_code_t *source_code);
extern void delete_filename(filename_t *filename);
extern void delete_search_dir(search_dir_t *search_dir);
extern void delete_variable(variable_t *variable);
extern void delete_variable_list(variable_list_t *variable);
extern void delete_operation_node(operation_node_t *operation_node);
extern void delete_operation_node_list(operation_node_list_t *operation_node_list);
extern void delete_group(group_t *group);
extern void delete_param(param_t *param);
extern void delete_param_list(param_list_t *param_list);

extern void delete_filenames();
extern void delete_search_dirs();
extern void delete_source_codes();
extern void delete_action_protos();
extern void delete_operations();
extern void delete_symbols();
extern void delete_variables();

extern void cleanup();


extern void add_filename(filename_t *filename);
extern void add_search_directory(search_dir_t *search_dir);
extern void add_operation_global(operation_t *operation);
extern void add_action_proto(action_proto_t *action_proto);
extern bitfield_list_t *add_bitfield(bitfield_list_t *bitfield_list, bitfield_t *bitfield);
extern void add_action(operation_t *operation, action_t *action);
extern variable_list_t *add_variable(variable_list_t *variable_list, variable_t *variable);
extern void add_variable_list(variable_list_t *variable_list);
extern operation_node_list_t *add_operation_node(operation_node_list_t *operation_node_list, operation_node_t *operation_node);
extern void add_group(group_t *group);
extern param_list_t *add_param(param_list_t *param_list, param_t *param);
extern template_parameter_list_t *add_template_parameter(template_parameter_list_t *template_parameter_list, template_parameter_t *template_parameter);


extern operation_t *search_operation_opcode(uint64_t opcode_mask, uint64_t opcode, symbol_hierarchy_t *symbol_hierarchy);
extern operation_t *search_operation_global(symbol_hierarchy_t *operation_symbol_hierarchy);
extern operation_t *search_base_operation(symbol_hierarchy_t *symbol_hierarchy, symbol_t *symbol);
extern action_proto_t *search_action_proto(symbol_hierarchy_t *action_proto_symbol_hierarchy);
extern action_proto_t *search_action_proto2(symbol_hierarchy_t *symbol_hierarchy, symbol_t *symbol);
extern action_t *search_action(operation_t *operation, action_proto_t *action_proto);
extern bitfield_t *search_operand_bitfield(bitfield_list_t *bitfield_list, symbol_t *bitfield_symbol);
extern variable_t *search_variable(variable_list_t *variable_list, symbol_t *variable_symbol);
extern group_t *search_group(symbol_hierarchy_t *group_symbol_hierarchy);
extern operation_node_t *search_operation_node(operation_node_list_t *operation_node_list, symbol_hierarchy_t *operation_symbol_hierarchy);
extern void remove_operation(operation_t *operation);
extern void remove_action_proto(action_proto_t *action_proto);
extern void remove_action(operation_t *operation, action_t *action);

extern void empty_string_buffer();
extern void add_chars_to_string_buffer(int length, char *chars);
extern char *get_string_buffer();
extern void delete_string_buffer();

extern void add_source_code(source_code_t *source_code);

extern symbol_t *get_symbol(char *name);
extern void delete_symbol(symbol_t *symbol);

extern void dump_operation(operation_t *operation, FILE *stream);
extern void dump_bitfield_list(bitfield_list_t *bitfield_list, FILE *stream);
extern void dump_bitfield(bitfield_t *bitfield, FILE *stream);
extern void dump_action_proto(action_proto_t *action_proto, FILE *stream);
extern void dump_action(action_t *action, FILE *stream);
extern void dump_source_code(source_code_t *source_code, FILE *stream);
extern void dump_variable(variable_t *variable, FILE *stream);
extern void dump_variable_list(variable_list_t *variable_list, FILE *stream);
extern void dump_param(param_t *param, FILE *stream);
extern void dump_param_list(param_list_t *param_list, FILE *stream);

extern void dump_action_protos(FILE *stream);
extern void dump_operations(FILE *stream);
extern void dump_source_codes(FILE *stream);
extern void dump_global_definitions(FILE *stream);

extern void dump(FILE *stream);

extern int generate_iss(char *prefix, unsigned int word_size, unsigned int addr_size);
extern int sanity_checks();

extern void initialize();

extern int little_endian;
extern symbol_hierarchy_t *namespace;
extern source_code_t *address_type;

#include <yy.h>

#endif
