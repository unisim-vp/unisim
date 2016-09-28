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
struct Vector : std::vector<Ptr<Type_t> >
{
  Vector() {}
  Vector( Type_t* _item ) : std::vector<Ptr<Type_t> >( 1,_item ) {}
  Vector( Vector<Type_t>& _src ) {
    if (not &_src) return;
    *this = _src;
  }
  Vector( uintptr_t _size ) : std::vector<Ptr<Type_t> >( _size, 0 ) {}
  
  Vector<Type_t>& operator=( Vector<Type_t>& _src )
  {
    if (not &_src) return *this;
    this->std::vector<Ptr<Type_t> >::operator=( _src );
    return *this;
  }
  Vector<Type_t>& append( Vector<Type_t>& _src )
  {
    if (not &_src) return *this;
    this->reserve( this->size() + _src.size() );
    typedef typename Vector<Type_t>::const_iterator iter_t;
    for( iter_t iter = _src.begin(); iter < _src.end(); ++ iter ) this->push_back( *iter );
    return *this;
  }
  Vector<Type_t>* append( Type_t* _item ) { this->push_back( _item ); return this; }
};

struct StringVector : std::vector<char const*>
{
  StringVector() {}
  StringVector( char const* _item ) : std::vector<char const*>( 1,_item ) {}
  StringVector( StringVector& _src ) {
    if (not &_src) return;
    *this = _src;
  }
  StringVector( uintptr_t _size ) : std::vector<char const*>( _size, 0 ) {}
  
  StringVector& operator=( StringVector& _src )
  {
    if (not &_src) return *this;
    this->std::vector<char const*>::operator=( _src );
    return *this;
  }
  StringVector& append( StringVector& _src )
  {
    if (not &_src) return *this;
    this->reserve( this->size() + _src.size() );
    typedef StringVector::const_iterator iter_t;
    for (iter_t iter = _src.begin(); iter < _src.end(); ++ iter) push_back( *iter );
    return *this;
  }
  StringVector* append( char const* _item ) { this->push_back( _item ); return this; }

};

struct UIntVector : std::vector<unsigned int>
{
  UIntVector( unsigned int value ) : std::vector<unsigned int>( 1, value ) {}
};

#endif // __VECT_HH__
