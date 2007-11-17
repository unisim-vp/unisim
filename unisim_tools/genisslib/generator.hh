/***************************************************************************
                                   generator.hh
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

#ifndef __GENERATOR_HH__
#define __GENERATOR_HH__

#include <fwd.hh>

struct Generator {
  Isa const*                          m_isa;
  
  Generator( Isa const* _isa );
  virtual ~Generator() {};
  
  virtual bool                  finalize() = 0;
  virtual bool                  sanity_checks() const = 0;
  virtual bool                  generate_iss( Product_t& _product, unsigned int word_size ) const = 0;
  
  Isa const&                    isa() const { return *m_isa; }
};

#endif // __GENERATOR_HH__
