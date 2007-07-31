%{
/***************************************************************************
                      scanner.lex  -  lexical analyzer
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

#include <GenISSLib.h>
#include <yy.h>
#include <string.h>

int open_brace_count = 0;
int lineno = 1;
int source_code_lineno;
filename_t *current_filename = 0;

void include_stack_push(YY_BUFFER_STATE *buffer_state);
int include_stack_pop(YY_BUFFER_STATE *buffer_state);
int do_include(char *filename);
void delete_include_stack();

extern int yypanicf(filename_t *filename, int lineno, char *format, ...);
extern int yyerrorf(filename_t *filename, int lineno, char *format, ...);
extern int yyerror(char *s);

void parse_binary_number(char *s, int length, unsigned int *value);

static int top = 4;
static int call_stack[4];

%}

identifier [a-zA-Z_][a-zA-Z0-9_]*
binary_number 0b[0-1]+
hexadecimal_number 0x[0-9a-fA-F]+
decimal_number [0-9]+

%x string_context
%x source_code_context
%x c_like_comment_context
%x cpp_like_comment_context
%x char_context

%%

\" { call_stack[--top] = YY_START; BEGIN(string_context); empty_string_buffer(); }
<string_context>[^\"\n] { add_chars_to_string_buffer(yyleng, yytext); }
<string_context>\n { add_chars_to_string_buffer(yyleng, yytext); lineno++; }
<string_context>\\\" { add_chars_to_string_buffer(yyleng, yytext); }
<string_context>\" {
                        BEGIN(call_stack[top]);
                        if(call_stack[top++] == INITIAL)
                        {
                            yylval.string = get_string_buffer();
                            return TOK_STRING;
                        }
                        else
                        {
                            add_chars_to_string_buffer(yyleng, yytext);
                        }
                    }

\{ { call_stack[--top] = YY_START; BEGIN(source_code_context); open_brace_count = 1; source_code_lineno = lineno; empty_string_buffer(); }
<source_code_context>[^/\}\{\"\n'] {  add_chars_to_string_buffer(yyleng, yytext); }
<source_code_context>"/" { add_chars_to_string_buffer(yyleng, yytext); }
<source_code_context>' { call_stack[--top] = YY_START; BEGIN(char_context); add_chars_to_string_buffer(yyleng, yytext); }
<source_code_context>\{ { open_brace_count++; add_chars_to_string_buffer(yyleng, yytext); }
<source_code_context>\" { call_stack[--top] = YY_START; BEGIN(string_context); add_chars_to_string_buffer(yyleng, yytext); }
<source_code_context>\n { add_chars_to_string_buffer(yyleng, yytext); lineno++; }
<source_code_context>\} {
                           if(--open_brace_count == 0)
                           {
                              source_code_t *source_code;
                              BEGIN(call_stack[top++]);
                              source_code = create_source_code(get_string_buffer(), current_filename, source_code_lineno);
                              add_source_code(source_code);
                              yylval.source_code = source_code;
                              return TOK_SOURCE_CODE;
                           }
                           else
                           {
                              add_chars_to_string_buffer(yyleng, yytext);
                           }
                        }

<char_context>[^'\n] {  if(call_stack[top] != INITIAL) add_chars_to_string_buffer(yyleng, yytext); }
<char_context>\n { if(call_stack[top] != INITIAL) add_chars_to_string_buffer(yyleng, yytext); lineno++; }
<char_context>\\' { if(call_stack[top] != INITIAL) add_chars_to_string_buffer(yyleng, yytext); }
<char_context>' { BEGIN(call_stack[top]); if(call_stack[top++] != INITIAL) add_chars_to_string_buffer(yyleng, yytext); }

<INITIAL,source_code_context>"/*" {
        call_stack[--top] = YY_START;
        BEGIN(c_like_comment_context);
        if(call_stack[top] == INITIAL)
        {
            empty_string_buffer();
        }
        add_chars_to_string_buffer(yyleng, yytext);
     }
<c_like_comment_context>[^*\n] { add_chars_to_string_buffer(yyleng, yytext); }
<c_like_comment_context>"*"+[^/\n] { add_chars_to_string_buffer(yyleng, yytext); }
<c_like_comment_context>"*"+\n { add_chars_to_string_buffer(yyleng, yytext); lineno++; }
<c_like_comment_context>\n { add_chars_to_string_buffer(yyleng, yytext); lineno++; }
<c_like_comment_context>"*"+"/" {
                                comment_t *comment;
                                BEGIN(call_stack[top]);
                                add_chars_to_string_buffer(yyleng, yytext);
                                if(call_stack[top++] == INITIAL)
                                {
                                    comment = create_comment(get_string_buffer(), current_filename, lineno);
                                    if(!comment_list)
                                        comment_list = create_comment_list(comment);
                                    else
                                        add_comment(comment_list, comment);
                                }
                              }


<INITIAL,source_code_context>"//" {
            call_stack[--top] = YY_START;
            BEGIN(cpp_like_comment_context);
            if(call_stack[top] == INITIAL)
            {
                empty_string_buffer();
            }
            add_chars_to_string_buffer(yyleng, yytext);
        }
<cpp_like_comment_context>[^\n] { add_chars_to_string_buffer(yyleng, yytext); }
<cpp_like_comment_context>\n {
                                comment_t *comment;
                                BEGIN(call_stack[top]);
                                if(call_stack[top++] == INITIAL)
                                {
                                    comment = create_comment(get_string_buffer(), current_filename, lineno);
                                    if(!comment_list)
                                        comment_list = create_comment_list(comment);
                                    else
                                        add_comment(comment_list, comment);
                                }
                                else
                                {
                                    add_chars_to_string_buffer(yyleng, yytext);
                                }
                                lineno++;
                             }

{binary_number} { parse_binary_number(yytext, yyleng, &yylval.integer); return TOK_INTEGER; }
{hexadecimal_number} { sscanf(yytext, "%x", &yylval.integer); return TOK_INTEGER; }
{decimal_number} { sscanf(yytext, "%u", &yylval.integer); return TOK_INTEGER; }
{identifier} { int token = get_token(yytext); if(token == TOK_IDENT) { yylval.symbol = get_symbol(yytext); } return token; }
\\\n { lineno++; }
\n { lineno++; return TOK_ENDL; }
"*" { return '*'; }
"." { return '.'; }
"[" { return '['; }
"]" { return ']'; }
"(" { return '('; }
")" { return ')'; }
"<"  { return '<'; }
">"  { return '>'; }
"," { return ','; }
":" { return ':'; }
"=" { return '='; }
"?" { return '?'; }
"::" { return TOK_QUAD_DOT; }
" " { }
\t { }
<INITIAL><<EOF>> {
                    YY_BUFFER_STATE buffer_state;
                    if(!include_stack_pop(&buffer_state))
                    {
                        yyterminate();
                    }
                    else
                    {
                        yy_delete_buffer(YY_CURRENT_BUFFER);
                        if(yyin) fclose(yyin);
                        yy_switch_to_buffer(buffer_state);
                    }
                 }

<string_context,char_context,c_like_comment_context,cpp_like_comment_context,source_code_context><<EOF>> { yypanicf(current_filename, lineno, "error! unexpected end of file");  yyterminate(); return 0;}
<INITIAL,string_context,char_context,c_like_comment_context,cpp_like_comment_context,source_code_context>. { yyerrorf(current_filename, lineno, "error! unexpected %s", get_char_name(yytext[0])); yyterminate(); return 0; }

%%

typedef struct include_struct
{
	YY_BUFFER_STATE buffer_state;
	filename_t *filename;
	int lineno;
	struct include_struct *next;
} include_t;

include_t *include_stack = 0;

int open_file(char *filename)
{
	char path[256];
	search_dir_t *search_dir = search_dir_list;
	
	fprintf(stderr, "Opening %s\n", filename);
	yyin = 0;
	while(search_dir)
	{
		sprintf(path, "%s/%s", search_dir->path, filename);
		yyin = fopen(path, "r");
		if(yyin)
		{
			break;
		}
		search_dir = search_dir->next;
	}
	
	if(!yyin)
	{
		yyin = fopen(filename, "r");
	}
	
	if(!yyin)
	{
		yyerrorf(current_filename, lineno, "error! can't open file `%s'", filename);
		return 0;
	}
	
	current_filename = create_filename(filename);
	add_filename(current_filename);
	return 1;
}

void include_stack_push(YY_BUFFER_STATE *buffer_state)
{
	include_t *include = (include_t *) malloc(sizeof(include_t));
	if(!include)
	{
		yyerrorf(0, 0, "internal error! not enough memory");
		exit(-1);
	}
	memcpy(&include->buffer_state, buffer_state, sizeof(YY_BUFFER_STATE));
	include->filename = current_filename;
	include->lineno = lineno;
	include->next = include_stack;
	include_stack = include;
}

int include_stack_pop(YY_BUFFER_STATE *buffer_state)
{
	if(include_stack)
	{
		include_t *include;
		memcpy(buffer_state, &include_stack->buffer_state, sizeof(YY_BUFFER_STATE));
		lineno = include_stack->lineno;
		current_filename = include_stack->filename;
		include = include_stack;
		include_stack = include_stack->next;
		free(include);
		return 1;
	}
	return 0;
}

int do_include(char *filename)
{
	YY_BUFFER_STATE state = YY_CURRENT_BUFFER;
	include_stack_push(&state /*&YY_CURRENT_BUFFER*/);
	if(!open_file(filename)) return 0;
	lineno = 1;

	yy_switch_to_buffer(yy_create_buffer(yyin, YY_BUF_SIZE));
	yylineno = 1;
	return 1;
}

void delete_include_stack()
{
	include_t *include, *next_include;

	for(include = include_stack; include; include = next_include)
	{
		next_include = include->next;
		free(include);
	}
}

void parse_binary_number(char *s, int length, unsigned int *value)
{
    char *p = s + length - 1;
    unsigned int v = 0;
    unsigned int mask = 1;

    while(p >= s && *p != 'b')
    {
        if(*p - '0') v |= mask;
        mask <<= 1;
        p--;
    }
    *value = v;
}


int yywrap()
{
	return 1;
}

