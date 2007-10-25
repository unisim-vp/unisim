#ifndef COMMENT_HH
#define COMMENT_HH

#include <fwd.hh>
#include <conststr.hh>
#include <referencecounting.hh>
#include <vector>

/** A C/C++ comment object */
struct Comment_t : virtual ReferenceCounter {
  ConstStr_t              m_content;
  ConstStr_t              m_filename;
  unsigned int          m_lineno;
  
  Comment_t( ConstStr_t _content, ConstStr_t _filename, int _lineno );
  Comment_t( Comment_t const& _comment );
  ~Comment_t();
};

#endif // COMMENT_HH
