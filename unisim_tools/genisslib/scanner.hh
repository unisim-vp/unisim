/***************************************************************************
                                 scanner.hh
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

#ifndef __SCANNER_HH__
#define __SCANNER_HH__

#include <fwd.hh>
#include <conststr.hh>
#include <referencecounting.hh>
#include <errtools.hh>
#include <vect.hh>

struct Scanner {
  static FileLoc_t                fileloc;        ///< file location in scanned file
  static FileLoc_t                fileloc_mlt;    ///< Starting line number of multi-line tokens
  static int                      bracecount;     ///< Global opened braces count
  static std::vector<int>         scs;
  static ConstStr_t::Set          symbols;        ///< The symbol database
  static Vect_t<Comment_t>        comments;       ///< Comments accumulator;
  static Str::Buf*                s_stringbuffer;
  static Isa*                     s_isa;
  static std::vector<ConstStr_t>  s_lookupdirs;   ///< Directory searched when opening files

  struct Include_t {
    uint8_t*                      m_state_backup;
    FileLoc_t                     m_fileloc;
    Include_t*                    m_next;
    
    Include_t( void const* _state, intptr_t _size, FileLoc_t const& _fileloc, Include_t* _next );
    ~Include_t();
    void                          restore( void* _state, intptr_t _size );
  };
  
  static Include_t* include_stack;
  
  struct Token_t {
    char const*                   m_name;         ///< the name of the token
    int                           m_token;        ///< the token
  };
  
  static Token_t s_tokens[];
  
  enum Exception_t { CWDError };
  
  static void                     push();
  static bool                     pop();

  static bool                     include( char const* _filename );
  static bool                     open( ConstStr_t _filename );
  static bool                     parse( char const* _filename, Isa& _isa );
  static Str::Buf&                strbuf() { return *s_stringbuffer; }
  static Isa&                     isa() { return *s_isa; }
  static int                      token( char const* _text );
  static ConstStr_t               charname( char _ch );
  static ConstStr_t               tokenname( int _token );
  static ConstStr_t               locate( char const* _name );
  static void                     add_lookupdir( char const* _dir );
  static void                     sc_enter( int _condition );
  static bool                     sc_leave();
};


// int yyerror( char const* _s );
// int yypanicf( char const *_filename, int _lineno, char const* _format, ... );
int yyparse();

#endif // __SCANNER_HH__
