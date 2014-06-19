/***************************************************************************
                                 strtools.hh
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

#ifndef __STRTOOLS_HH__
#define __STRTOOLS_HH__

/*** Classes an method for handling strings. Here, string refer to the
 *** C-fashioned strings: null-terminated arrays of char.
 ***/
#include <inttypes.h>
#include <conststr.hh>

namespace Str {
  ConstStr_t                      fmt( char const* _fmt, ... );
  ConstStr_t                      upper( char const* _str );
  ConstStr_t                      capitalize( char const* _str );
  ConstStr_t                      tokenize( char const* _str );
  ConstStr_t                      dqcstring( char const* _str );
};

#endif // __STRTOOLS_HH__
