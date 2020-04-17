/***************************************************************************
                                 main.hh
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

#ifndef __MAIN_HH__
#define __MAIN_HH__

#define GENISSLIB "GenISSLib"

#ifndef GENISSLIB_VERSION
#define GENISSLIB_VERSION "???"
#endif

#define AUTHORS "Gilles Mouchard, Yves Lhuillier"
#define EMAILS "gilles.mouchard@cea.fr, yves.lhuillier@cea.fr"
#define COPYRIGHT "Copyright (c) 2003-2010 Commissariat a l'Energie Atomique (CEA) and Universite Paris Sud"

#include <conststr.hh>
#include <vector>

struct Opts
{
  Opts();
  
  char const*             outputprefix;
  unsigned int            verbosity;
  char const*             expandname;
  char const*             depfilename;
  unsigned int            minwordsize;
  bool                    sourcelines;
  bool                    privatemembers;
  bool                    specialization;
  bool                    comments;
  std::vector<ConstStr>   lookupdirs;
  
  ConstStr                locate( char const* _name );
  bool                    add_lookupdir( char const* _dir );
};

#endif // __MAIN_HH__
