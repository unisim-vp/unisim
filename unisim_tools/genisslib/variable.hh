#ifndef VARIABLE_HH
#define VARIABLE_HH

#include <fwd.hh>
#include <iosfwd>

#include <conststr.hh>
#include <referencecounting.hh>

/** A variable object */
struct Variable_t : virtual ReferenceCounter {
  ConstStr_t             m_symbol; /**< the symbol object representing the variable */
  Ptr_t<SourceCode_t>    m_ctype; /**< the C type of the variable */
  Ptr_t<SourceCode_t>    m_cinit; /**< the C expression used to initialized the variable */

  Variable_t( ConstStr_t _symbol, SourceCode_t* _ctype, SourceCode_t* _cinit );
  Variable_t( Variable_t const& _variable );
  ~Variable_t();
};

std::ostream& operator<<( std::ostream& _sink, Variable_t const& _var );

#endif // VARIABLE_HH
