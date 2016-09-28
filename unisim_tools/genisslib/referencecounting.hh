/***************************************************************************
                            referencecounting.hh
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

#ifndef __REFERENCECOUNTING_HH__
#define __REFERENCECOUNTING_HH__

#include <inttypes.h>

struct ReferenceCounter
{
  intptr_t              m_count;
  
  ReferenceCounter() : m_count( 0 ) {}
  
  void                  const_check() const;
};
  
template <class Object_t>
class Ptr_t
{
  Object_t*             m_object;
  
  void                  retain() { ++ (m_object->ReferenceCounter::m_count); }
  intptr_t              release() { return -- (m_object->ReferenceCounter::m_count); }
  
public:
  Ptr_t() : m_object( 0 ) {}
  Ptr_t( Object_t* _object ) : m_object( _object ) { if( m_object ) retain(); }
  Ptr_t( Ptr_t const& _ref ) : m_object( _ref.m_object ) { if( m_object ) retain(); }
  
  Ptr_t&                operator=( Ptr_t const& _ref ) {
    if( m_object ) release();
    if( (m_object = _ref.m_object) ) retain();
    return *this;
  }
    
  ~Ptr_t() { if( not m_object or release() > 0 ) return; delete m_object; }
    
  operator Object_t const* () const { if( not m_object ) return 0; return m_object; }
  operator Object_t* () {
    if( not m_object ) return 0;
    // m_object->ReferenceCounter::const_check();
    return m_object;
  }
    
  Object_t const*       operator->() const { if( not m_object ) return 0; return m_object; }
  Object_t*             operator->() {
    if( not m_object ) return 0;
    // m_object->ReferenceCounter::const_check();
    return m_object;
  }
};

template <class Object_t>
class ConstPtr_t {
  Object_t const*       m_object;
  
  typedef ReferenceCounter const* ConstRCP_t;
  typedef ReferenceCounter* RCP_t;
  
  ReferenceCounter*     refcounter() { return const_cast<ReferenceCounter*>( static_cast<ReferenceCounter const*>( m_object ) ); }
  void                  retain() { ++ (refcounter()->m_count); }
  intptr_t              release() { return -- (refcounter()->m_count); }
  
public:
  ConstPtr_t() : m_object( 0 ) {}
  ConstPtr_t( Object_t const* _object ) : m_object( _object ) { if( m_object ) retain(); }
  ConstPtr_t( ConstPtr_t const& _ref ) : m_object( _ref.m_object ) { if( m_object ) retain(); }
  ConstPtr_t( Ptr_t<Object_t> const& _ref ) : m_object( _ref.m_object ) { if( m_object ) retain(); }
  
  ConstPtr_t&                operator=( ConstPtr_t const& _ref ) {
    if( m_object ) release();
    if( (m_object = _ref.m_object) ) retain();
    return *this;
  }
  
  ConstPtr_t&                operator=( Ptr_t<Object_t> const& _ref ) {
    if( m_object ) release();
    if( (m_object = _ref.m_object) ) retain();
    return *this;
  }
  
  ~ConstPtr_t() { if( not m_object or release() > 0 ) return; delete m_object; }
    
  operator Object_t const* () const { if( not m_object ) return 0; return m_object; }
  Object_t const*       operator->() const { if( not m_object ) return 0; return m_object; }
};

#endif // __REFERENCECOUNTING_HH__
