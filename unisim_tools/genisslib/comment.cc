/***************************************************************************
                                  comment.cc
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

#include <comment.hh>
#include <scanner.hh>

/** Constructor
    @param _content the C String containing the C comment
    @param _filename the filename object where the C/C++ comment was found
    @param _lineno the line number where the C/C++ comment was found
*/
Comment_t::Comment_t( ConstStr _content, FileLoc_t const& _fileloc )
  : m_content( _content ), m_fileloc( _fileloc )
{}

Comment_t::Comment_t( Comment_t const& _comment )
  : m_content( _comment.m_content ), m_fileloc( _comment.m_fileloc )
{}


/** Destructor
*/
Comment_t::~Comment_t() {}

