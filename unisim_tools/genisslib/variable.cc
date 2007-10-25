#include <variable.hh>
#include <sourcecode.hh>
#include <strtools.hh>
#include <iostream>

using namespace std;

Variable_t::Variable_t( ConstStr_t _symbol, SourceCode_t* _ctype, SourceCode_t* _cinit )
  : m_symbol( _symbol ), m_ctype( _ctype ), m_cinit( _cinit )
{}

Variable_t::Variable_t( Variable_t const& _variable )
  : m_symbol( _variable.m_symbol ), m_ctype( _variable.m_ctype ), m_cinit( _variable.m_cinit )
{}

Variable_t::~Variable_t() {};

/** Dump the variable object into a stream
    @param stream a stream
*/
std::ostream&
operator<<( std::ostream& _sink, Variable_t const& _var ) {
  return (_sink << _var.m_symbol << " : " << (*_var.m_ctype));
}
