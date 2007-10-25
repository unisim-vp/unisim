/***************************************************************************
                                 conststr.hh
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

#ifndef __STRIMMUTABLE_HH__
#define __STRIMMUTABLE_HH__

/*** Classes an method for handling strings. Here, string refer to the
 *** C-fashioned strings: null-terminated arrays of char.
 ***/

#include <inttypes.h>
#include <map>

/** Ref:
 * Reference to a immutable string (Instance)
 */

struct ConstStr_t {
  /** Instance:
   * Imutable string container
   */
  
  struct Instance {
    char const*                   m_string;
    intptr_t                      m_refcount;
    
    Instance( char const* _string );
    ~Instance();
    
    Instance*                     retain() { if( not this ) return 0; ++m_refcount; return this; }
    void                          release();
  };

  struct Less {
    bool operator()( char const* _s1, char const* _s2 ) const {
      while( *_s1 != '\0' and *_s1 == *_s2 )
        ++_s1, ++_s2;
      return *_s1 < *_s2;
    }
  };

  Instance*                     m_instance;
  typedef std::map<char const*, ConstStr_t, Less> Set;
  
  // Construction / Destruction / Assignation
  ConstStr_t() : m_instance( 0 ) {}
  ConstStr_t( char const* _string );
  ConstStr_t( char const* _string, Set& _set );
  ConstStr_t( ConstStr_t const& _ref );
  ConstStr_t&                   operator=( ConstStr_t const& _ref );
  ConstStr_t&                   init( Instance* _instance );
  ~ConstStr_t();
  
  // C string access (char const*)
  operator                      char const*() const { return m_instance ? m_instance->m_string : 0; }
  char const*                   str() const { return m_instance ? m_instance->m_string : 0; }
};

#endif // __STRIMMUTABLE_HH__
