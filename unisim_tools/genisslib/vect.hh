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

template <class TP>
struct Vector : std::vector<Ptr<TP> >
{
  Vector() {}
  Vector( TP* _item ) : std::vector<Ptr<TP> >( 1,_item ) {}
  Vector( Vector<TP> const& _src ) {
    if (not &_src) return;
    *this = _src;
  }
  Vector( uintptr_t _size ) : std::vector<Ptr<TP> >( _size, (TP*)(0) ) {}
  
  Vector<TP>& operator=( Vector<TP> const& _src )
  {
    if (not &_src) return *this;
    this->std::vector<Ptr<TP> >::operator=( _src );
    return *this;
  }
  Vector<TP>& append( Vector<TP> const& _src )
  {
    if (not &_src) return *this;
    this->reserve( this->size() + _src.size() );
    typedef typename std::vector<Ptr<TP> >::const_iterator const_iterator;
    for (const_iterator iter = _src.begin(); iter != _src.end(); ++ iter)
      this->push_back( *iter );
    return *this;
  }
  Vector<TP>* append( TP* _item ) { this->push_back( _item ); return this; }
};

struct StringVector : std::vector<char const*>
{
  StringVector() {}
  StringVector( char const* _item ) : std::vector<char const*>( 1,_item ) {}
  StringVector( StringVector& _src )
  {
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
    typedef typename std::vector<char const*>::const_iterator const_iterator;
    for (const_iterator iter = _src.begin(); iter != _src.end(); ++ iter) push_back( *iter );
    return *this;
  }
  StringVector* append( char const* _item ) { this->push_back( _item ); return this; }

};

struct UIntVector : std::vector<unsigned int>
{
  UIntVector( unsigned int value ) : std::vector<unsigned int>( 1, value ) {}
};

#endif // __VECT_HH__
