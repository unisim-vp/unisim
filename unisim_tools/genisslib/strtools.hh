#ifndef STRMUTABLE_HH
#define STRMUTABLE_HH

/*** Classes an method for handling strings. Here, string refer to the
 *** C-fashioned strings: null-terminated arrays of char.
 ***/
#include <inttypes.h>
#include <conststr.hh>

namespace Str {
  /** Buf:
   * Mutable string container
   */
  struct Buf {
    char*                         m_storage;
    intptr_t                      m_capacity;
    intptr_t                      m_index;
    enum Policy_t { Delete = 0, Recycle };
    Policy_t                      m_policy;
    static intptr_t const         s_chunksize = 32;
  
    Buf( Policy_t _policy = Delete );
    ~Buf();
  
    Buf&                          capacity( intptr_t _capacity );
    Buf&                          write( char const* _str );
    Buf&                          write( intptr_t _length, char const* _str );
    Buf&                          clear() { m_storage[0] = '\0'; m_index = 0; return *this; }
    
    // Recycle tools
    struct RecycleChunk {
      RecycleChunk*               m_next;
      intptr_t                    m_capacity;
      static RecycleChunk*        s_pool;
      static void                 recycle( char* _storage, intptr_t _capacity );
    };
    void                          recycle();
    struct Scope_t { ~Scope_t(); };
    static Scope_t                s_scope;
  };
  
  ConstStr_t                      fmt( char const* _fmt, ... );
  ConstStr_t                      upper( char const* _str );
  ConstStr_t                      capitalize( char const* _str );
  ConstStr_t                      tokenize( char const* _str );
};

#endif // STRMUTABLE_HH
