/***************************************************************************
                                 strtools.cc
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

#include <strtools.hh>
#include <cstdarg>
#include <cstring>
#include <cassert>
#include <cctype>
//#include <iostream>

namespace Str {
  Buf::Buf( Policy_t _policy )
    : m_storage( 0 ), m_capacity( 0 ), m_index( 0 ), m_policy( _policy )
  {
    RecycleChunk* chunk = RecycleChunk::s_pool;
    if( not chunk ) {
      m_storage = new char[s_chunksize];
      m_capacity = s_chunksize;
    } else {
      RecycleChunk::s_pool = chunk->m_next;
      m_storage = (char*)( chunk ); // dirty cast
      m_capacity = chunk->m_capacity;
    }
    m_storage[0] = '\0';
  }
  
  Buf::~Buf() {
    switch( m_policy ) {
    case Delete: delete m_storage; break;
    case Recycle: recycle(); break;
    default: throw m_policy;
    }
  }

  void
  Buf::recycle() {
    RecycleChunk::recycle( m_storage, m_capacity );
    m_storage = 0;
    m_capacity = 0;
  }
  
  Buf::RecycleChunk* Buf::RecycleChunk::s_pool = 0;
  
  void
  Buf::RecycleChunk::recycle( char* _storage, intptr_t _capacity ) {
    RecycleChunk* chunk = (RecycleChunk*)( _storage ); // dirty cast
    chunk->m_capacity = _capacity;
    chunk->m_next = s_pool;
    s_pool = chunk;
  }
  
  Buf&
  Buf::write( char const* _str ) {
    intptr_t len = strlen( _str );
    intptr_t needed_capacity = len + m_index + 1; //< Trailing '\0'
    if( m_capacity < needed_capacity )
      capacity( needed_capacity );
    strcpy( &m_storage[m_index], _str );
    m_index += len;
    return *this;
  }

  Buf&
  Buf::write( intptr_t _length, char const* _str ) {
    intptr_t needed_capacity = _length + m_index + 1; //< Trailing '\0'
    if( m_capacity < needed_capacity )
      capacity( needed_capacity );
    memcpy( &m_storage[m_index], _str, _length );
    m_index += _length;
    m_storage[m_index] = '\0';
    return *this;
  }

  Buf&
  Buf::write( char _ch ) {
    intptr_t needed_capacity = m_index + 2; //< (char) + (trailing '\0')
    if( m_capacity < needed_capacity )
      capacity( needed_capacity );
    m_storage[m_index] = _ch;
    m_storage[++m_index] = '\0';
    return *this;
  }

  Buf&
  Buf::capacity( intptr_t _capacity ) {
    intptr_t ncapacity = (_capacity + s_chunksize - 1) & ~(s_chunksize - 1);
    if( ncapacity <= m_capacity ) return *this;
    char* nstorage = new char[ncapacity];
    strcpy( nstorage, m_storage );
    delete [] m_storage;
    m_storage = nstorage;
    m_capacity = ncapacity;
    return *this;
  }
  
  Buf::Scope_t Buf::s_scope;
  
  Buf::Scope_t::~Scope_t() {
    while( RecycleChunk::s_pool ) {
      RecycleChunk* tmp = RecycleChunk::s_pool;
      RecycleChunk::s_pool = tmp->m_next;
      char* storage = (char*)tmp;
      delete [] storage;
    }
  }
  
  ConstStr_t
  fmt( char const* _fmt, ... ) {
    va_list ap;

    for( intptr_t capacity = 128, size; true; capacity = (size > -1) ? size + 1 : capacity * 2 ) {
      /* allocation */
      char storage[capacity];
      /* Try to print in the allocated space. */
      va_start( ap, _fmt );
      size = vsnprintf( storage, capacity, _fmt, ap );
      va_end( ap );
      /* If it worked, return */
      if( size >= 0 and size < capacity )
        return ConstStr_t( storage );
    }
    assert( false );
  }

  ConstStr_t
  upper( char const* _str ) {
    Buf buffer( Buf::Recycle );
    buffer.write( _str );
    for( char* ptr = buffer.m_storage; *ptr; ++ ptr ) {
      char ch = *ptr;
      if( ch >= 'a' and ch <= 'z' )
        *ptr = (ch - 'a' + 'A');
    }
    
    return ConstStr_t( buffer.m_storage );
  }
  
  ConstStr_t
  capitalize( char const* _str ) {
    Buf buffer( Buf::Recycle );
    buffer.write( _str );
    
    char& ch = *buffer.m_storage;
    if( ch >= 'a' and ch <= 'z')
        ch = (ch - 'a' + 'A');
    
    return ConstStr_t( buffer.m_storage );
  }
  
  ConstStr_t
  tokenize( char const* _str ) {
    Buf buffer( Buf::Recycle );
    buffer.write( _str );
    
    for( char* ptr = buffer.m_storage; *ptr; ++ ptr ) {
      char ch = *ptr;
      *ptr = isalnum( ch ) ? ch : '_';
    }

    return ConstStr_t( buffer.m_storage );
  }

  ConstStr_t
  dqcstring( char const* _str ) {
    Buf buffer( Buf::Recycle );
    buffer.write( '"' );
    
    static char const* hextab = "0123456789abcdef";
    
    for( char const* ptr = _str; *ptr; ++ ptr ) {
      char ch = *ptr;
      switch( ch ) {
      case '\n': buffer.write( "\\n" ); break;
      case '\r': buffer.write( "\\r" ); break;
      case  '"': buffer.write( "\\\"" ); break;
      case '\\': buffer.write( "\\\\" ); break;
      default:
        {
          if( ch >= 127 or ch < 32 ) {
            buffer.write( "\\x" );
            buffer.write( hextab[unsigned( ch ) / 256] );
            buffer.write( hextab[unsigned( ch ) % 256] );
          } else {
            buffer.write( ch );
          }
        }
        break;
      }
    }
    buffer.write( '"' );
    
    return ConstStr_t( buffer.m_storage );
  }

} // end of namespace Str
