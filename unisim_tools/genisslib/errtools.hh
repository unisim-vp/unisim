/***************************************************************************
                                 errtools.hh
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

#ifndef __ERRTOOLS_HH__
#define __ERRTOOLS_HH__

/*** Classes an method for handling error log, file locations...
 ***/
#include <inttypes.h>
#include <conststr.hh>

struct FileLoc_t {
  ConstStr_t m_name;
  intptr_t   m_line;
  
  FileLoc_t() : m_line( 0 ) {}
  FileLoc_t( ConstStr_t _name, intptr_t _line ) : m_name( _name ), m_line( _line ) {}
  FileLoc_t( FileLoc_t const& _fl ) : m_name( _fl.m_name ), m_line( _fl.m_line ) {}
  
  FileLoc_t& operator=( FileLoc_t const& _fl ) { m_name = _fl.m_name; m_line = _fl.m_line; return *this; }
  FileLoc_t& assign( ConstStr_t _name, intptr_t _line ) { m_name = _name; m_line = _line; return *this; }
  
  void       err( char const* _fmt, ... ) const;
};


#endif // __ERRTOOLS_HH__
