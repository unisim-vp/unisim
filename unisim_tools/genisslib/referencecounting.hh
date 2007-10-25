#ifndef REFERENCECOUNTING_HH
#define REFERENCECOUNTING_HH

#include <inttypes.h>

struct ReferenceCounter {
  intptr_t              m_count;
  
  ReferenceCounter() : m_count( 0 ) {}
  
  void                  const_check() const;
};
  
template <class Object_t>
class Ptr_t {
  Object_t*             m_object;
  
  
  void                  retain() { ++ (m_object->ReferenceCounter::m_count); }
  
  intptr_t              release() { return -- (m_object->ReferenceCounter::m_count); }
  
public:
  Ptr_t() : m_object( 0 ) {}
  Ptr_t( Object_t* _object ) : m_object( _object ) { if( m_object ) retain(); }
  
  Ptr_t( Ptr_t const& _ref ) : m_object( _ref.m_object ) { if( m_object ) retain(); }
  
  Ptr_t&                  operator=( Ptr_t const& _ref )
  {
    if( m_object ) release();
    m_object = _ref.m_object;
    if( m_object ) retain();
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

#endif // REFERENCECOUNTING_HH
