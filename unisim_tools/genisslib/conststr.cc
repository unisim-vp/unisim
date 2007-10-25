#include <conststr.hh>

ConstStr_t::ConstStr_t( char const* _string )
  : m_instance( _string ? (new Instance( _string ))->retain() : 0 )
{}
  
ConstStr_t::ConstStr_t( char const* _string, Set& _set )
  : m_instance( 0 )
{
  if( not _string ) return;
  Set::iterator iter = _set.find( _string );
  Instance* inst = 0;
  if( iter != _set.end() ) {
    inst = iter->second.m_instance;
  } else {
    inst = new Instance( _string );
    _set[inst->m_string].init( inst );
  }
  m_instance = inst->retain();
}

ConstStr_t::ConstStr_t( ConstStr_t const& _ref )
  : m_instance( _ref.m_instance->retain() )
{}

ConstStr_t&
ConstStr_t::init( Instance* _instance ) {
  m_instance->release();
  m_instance = _instance->retain();
  return *this;
}
  
ConstStr_t::~ConstStr_t() {
  m_instance->release();
}

ConstStr_t&
ConstStr_t::operator=( ConstStr_t const& _ref ) {
  m_instance->release();
  m_instance = _ref.m_instance->retain();
  return *this;
}
  
ConstStr_t::Instance::Instance( char const* _string )
  : m_string( 0 ), m_refcount( 0 )
{
  intptr_t len = strlen( _string );
  //std::cout << "Creating: " << _string << "\n";
  char* storage = new char[len+1];
  strcpy( storage, _string );
  m_string = storage;
    
}

ConstStr_t::Instance::~Instance () {
  //std::cout << "Deleting: " << m_string << "\n";
  delete [] m_string;
}
  
void
ConstStr_t::Instance::release() {
  if( not this ) return;
  if( (--m_refcount) > 0 ) return;
  delete this;
}
