/***************************************************************************
                                    cli.hh
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

#ifndef __CLI_HH__
#define __CLI_HH__

#include <inttypes.h>

struct CLI {
  char const*            m_displayname;
  char const*            m_callname;
  intptr_t               m_opt_tabstop;
  intptr_t               m_optc;
  
  enum Appearances_t { AtMostOnce, Unlimited, Once };
  
  struct Exit_t { int m_value; Exit_t( int _value ) : m_value( _value ) {} };
  
  struct Args_t {
    virtual ~Args_t() {};
    virtual bool         match( Appearances_t _appearances, ... ) = 0;
    virtual char const*  pop();
  };
  
  CLI();
  virtual ~CLI() {}
  
  void                   set( char const* _displayname, char const* _callname );
  
  void                   process( intptr_t _argc, char** _argv );

  virtual void           help();
  virtual void           prototype();
  virtual void           options();

  virtual void           parse( Args_t& _args ) = 0;
  virtual void           description() = 0;
};

#endif // __CLI_HH__
