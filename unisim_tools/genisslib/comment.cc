#include <comment.hh>
#include <scanner.hh>

/** Constructor
    @param _content the C String containing the C comment
    @param _filename the filename object where the C/C++ comment was found
    @param _lineno the line number where the C/C++ comment was found
*/
Comment_t::Comment_t( ConstStr_t _content, ConstStr_t _filename, int _lineno )
  : m_content( _content ), m_filename( _filename ), m_lineno( _lineno )
{}

Comment_t::Comment_t( Comment_t const& _comment )
  : m_content( _comment.m_content ), m_filename( _comment.m_filename ), m_lineno( _comment.m_lineno )
{}


/** Destructor
*/
Comment_t::~Comment_t() {}

