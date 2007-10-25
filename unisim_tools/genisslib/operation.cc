#include <operation.hh>
#include <scanner.hh>
#include <comment.hh>
#include <variable.hh>
#include <sourcecode.hh>
#include <action.hh>
#include <bitfield.hh>
#include <action.hh>
#include <isa.hh>
#include <cassert>
#include <ostream>

using namespace std;

/** Create an operation object
    @param symbol a symbol object representing the operation
    @param bitfield_list a bit field list object containing the bit fields of the operation
    @param comment_list a C/C++ comment list object containing C/C++ comments relative to the operation
    @param variable_list a variable list object
    @param filename a filename object where the operation was found
    @param lineno a line number where the operation was found
    @return an operation object
*/
Operation_t::Operation_t( ConstStr_t _symbol, Vect_t<BitField_t>& _bitfields, Vect_t<Comment_t>& _comments,
                          Vect_t<Variable_t>& _vars, SourceCode_t* _op_condition, ConstStr_t _filename, int _lineno )
  : m_symbol( _symbol ), m_bitfields( _bitfields ), m_size( 0 ), m_opmask( 0 ), m_opcode( 0 ),
    m_filename( _filename ), m_lineno( _lineno ),
    m_comments( _comments ), m_variables( _vars ), m_condition( _op_condition )
{
  for( Vect_t<BitField_t>::const_iterator iter = _bitfields.begin(); iter < _bitfields.end(); ++ iter )
    m_size += (*iter)->m_size;
  
  unsigned int shift = m_size;
  for( Vect_t<BitField_t>::const_iterator iter = _bitfields.begin(); iter < _bitfields.end(); ++ iter ) {
    shift -= (*iter)->m_size;
    if( not (*iter)->hasopcode() ) continue;
    m_opcode |= (*iter)->opcode() << shift;
    m_opmask |= (*iter)->mask() << shift;
  }
}

/** Delete an operation object
    @param operation an operation object to delete
*/
Operation_t::~Operation_t() {}

/** Create a group object
    @param operation_list the list of the operation of the group
    @returns a group list object
*/
Group_t::Group_t( ConstStr_t _symbol, Vect_t<Operation_t>& _oplist, ConstStr_t _filename, int _lineno )
  : m_symbol( _symbol ), m_operations( _oplist ),
    m_filename( _filename ), m_lineno( _lineno ), m_next( 0 )
{}

/** Search the operation for an action implementing an action prototype
    @param actionproto an action prototype object
    @return the matching action object, null if no action object matches
*/
Action_t const*
Operation_t::action( ActionProto_t const* _actionproto ) const {
  for( Vect_t<Action_t>::const_iterator iter = m_actions.begin(); iter < m_actions.end(); ++ iter )
    if( (*iter)->m_actionproto == _actionproto ) return *iter;
  
  return 0;
}

void
Operation_t::add( Action_t* _action ) {
  m_actions.push_back( _action );
  assert( not _action->m_operation );
  _action->m_operation = this;
}

/** Dump an operation object into a stream
    @param operation an operation object to dump
    @param _sink a stream
*/

ostream&
operator<<( ostream& _sink, Operation_t const& _op ) {
  char const* sep;
  _sink << "op " << _op.m_symbol << '(';
  
  sep = "";
  for( Vect_t<BitField_t>::const_iterator iter = _op.m_bitfields.begin(); iter < _op.m_bitfields.end(); sep = ":", ++ iter )
    _sink << sep << *(*iter);
  
  _sink << ')';
  
  if( not _op.m_variables.empty() ) {
    _sink << " var ";
    sep = "";
    for( Vect_t<Variable_t>::const_iterator var = _op.m_variables.begin(); var < _op.m_variables.end(); sep = ", ", ++ var )
      _sink << sep << *(*var);
  }
  _sink << "\n\n";
  
  for( Vect_t<Action_t>::const_iterator action = _op.m_actions.begin(); action < _op.m_actions.end(); ++ action )
    _sink << *(*action) << '\n';
  
  return _sink;
}

