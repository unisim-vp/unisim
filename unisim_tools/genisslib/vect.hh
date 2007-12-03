/***************************************************************************
                                   vect.hh
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

#ifndef __VECT_HH__
#define __VECT_HH__

#include <inttypes.h>
#include <vector>
#include <referencecounting.hh>

template <class Type_t>
struct Vect_t : std::vector<Ptr_t<Type_t> > {
  Vect_t() {}
  Vect_t( Type_t* _item ) : std::vector<Ptr_t<Type_t> >( 1,_item ) {}
  Vect_t( Vect_t<Type_t>& _src ) {
    if( not &_src ) return;
    *this = _src;
  }
  Vect_t( uintptr_t _size ) : std::vector<Ptr_t<Type_t> >( _size, 0 ) {}
  
  Vect_t<Type_t>& operator=( Vect_t<Type_t>& _src ) {
    if( not &_src ) return *this;
    this->std::vector<Ptr_t<Type_t> >::operator=( _src );
    return *this;
  }
  Vect_t<Type_t>& append( Vect_t<Type_t>& _src ) {
    if( not &_src ) return *this;
    reserve( this->size() + _src.size() );
    typedef typename Vect_t<Type_t>::const_iterator iter_t;
    for( iter_t iter = _src.begin(); iter < _src.end(); ++ iter ) push_back( *iter );
    return *this;
  }
  Vect_t<Type_t>* append( Type_t* _item ) { this->push_back( _item ); return this; }
};

#endif // __VECT_HH__
