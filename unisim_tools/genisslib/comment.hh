/***************************************************************************
                                 comment.hh
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

#ifndef __COMMENT_HH__
#define __COMMENT_HH__

#include <fwd.hh>
#include <conststr.hh>
#include <errtools.hh>
#include <referencecounting.hh>
#include <vector>

/** A C/C++ comment object */
struct Comment_t : virtual ReferenceCounter
{
  ConstStr              m_content;
  FileLoc_t               m_fileloc;
  
  Comment_t( ConstStr _content, FileLoc_t const& _fileloc );
  Comment_t( Comment_t const& _comment );
  ~Comment_t();
};

#endif // __COMMENT_HH__
