#include <referencecounting.hh>
#include <iostream>
using namespace std;

void
ReferenceCounter::const_check() const {
  if( not this or m_count > 1 ) {
    asm volatile( "shared_object_modification:" );
    cerr << "Modifying a shared object...\n";
  }
}
