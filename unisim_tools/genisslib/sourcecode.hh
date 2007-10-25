#ifndef SOURCECODE_HH
#define SOURCECODE_HH

#include <fwd.hh>
#include <conststr.hh>
#include <referencecounting.hh>

/** A C source code object */
struct SourceCode_t : virtual ReferenceCounter {
  ConstStr_t                  m_content;    /**< the string containing the C source code */
  ConstStr_t                  m_filename;   /**< the filename object where was found the C source code */
  int                         m_lineno;     /**< the line number where was found the C source code */
  
  SourceCode_t( ConstStr_t _content, ConstStr_t _filename, int _lineno );
  
  static SourceCode_t const*  s_last_srccode;
};

std::ostream& operator<<( std::ostream& _sink, SourceCode_t const& _sc );

/** A C/C++ Code object object */
struct CodePair_t : virtual ReferenceCounter {
  Ptr_t<SourceCode_t>        m_ctype;        /**< The C type of the parameter */
  Ptr_t<SourceCode_t>        m_csymbol;      /**< The C symbol of the parameter */
  
  CodePair_t( SourceCode_t* _ctype, SourceCode_t* _csymbol );
  ~CodePair_t();
};

std::ostream& operator<<( std::ostream& _sink, CodePair_t const& _cp );
#endif // SOURCECODE_HH
