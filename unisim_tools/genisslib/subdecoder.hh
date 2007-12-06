/***************************************************************************
                                 subdecoder.hh
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

#ifndef __SUBDECODER_HH__
#define __SUBDECODER_HH__

#include <inttypes.h>
#include <fwd.hh>
#include <conststr.hh>
#include <errtools.hh>
#include <referencecounting.hh>
#include <vect.hh>

/** An SubDecoder object */
struct SubDecoder_t : virtual ReferenceCounter {
  ConstStr_t                    m_symbol;            /**< The associated symbol */
  Ptr_t<SourceCode_t>           m_namespace;         /**< The namespace in which the decoder is defined */
  Ptr_t<SourceCode_t>           m_template_scheme;   /**< The template scheme of the decoder */
  unsigned int                  m_minsize;           /**< The minimum size (in bytes) of the decoder's operations */
  unsigned int                  m_maxsize;           /**< The maximum size (in bytes) of the decoder's operations */
  FileLoc_t                     m_fileloc;           /**< The file location where subdecoder was declared */
  
  SubDecoder_t( ConstStr_t _symbol, SourceCode_t* _namespace, SourceCode_t* _template_scheme,
                unsigned int _minsize, unsigned int _maxsize, FileLoc_t const& _fileloc );
  ~SubDecoder_t();
  
};

std::ostream& operator<<( std::ostream& _sink, SubDecoder_t const& _op );

#endif // __SUBDECODER_HH__
