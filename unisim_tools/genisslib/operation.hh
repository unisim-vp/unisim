#ifndef OPERATION_HH
#define OPERATION_HH

#include <inttypes.h>
#include <fwd.hh>
#include <conststr.hh>
#include <referencecounting.hh>
#include <vect.hh>

/** An operation object */
struct Operation_t : virtual ReferenceCounter {
  ConstStr_t                    m_symbol;            /**< The associated symbol */
  Vect_t<BitField_t>            m_bitfields;         /**< The bit field list of the operation */
  Vect_t<Action_t>              m_actions;           /**< The list of actions of the operation */
  unsigned int                  m_size;              /**< Number of bits of the operation */
  uint64_t                      m_opmask;            /**< the opcode mask */
  uint64_t                      m_opcode;            /**< the opcode */
  ConstStr_t                    m_filename;          /**< the filename where the operation was declared */
  int                           m_lineno;            /**< the line number where the operation was declared */
  Vect_t<Comment_t>             m_comments;          /**< The list of the C comment associated with the operation */
  Vect_t<Variable_t>            m_variables;         /**< The list of variables associated with the operation */
  Ptr_t<SourceCode_t>           m_condition;         /**< The condition associated with the operation */
  
  Operation_t( ConstStr_t _symbol,
               Vect_t<BitField_t>& _bitfields,
               Vect_t<Comment_t>& _comments,
               Vect_t<Variable_t>& _variables,
               SourceCode_t* _op_condition, ConstStr_t _filename, int _lineno );
  ~Operation_t();
  
  void                          add( Action_t* _action );
  Action_t const*               action( ActionProto_t const* _actionproto ) const;
};

std::ostream& operator<<( std::ostream& _sink, Operation_t const& _op );

/** A group object */
struct Group_t : virtual ReferenceCounter {
  ConstStr_t                    m_symbol;         /**< The associated symbol */
  Vect_t<Operation_t>           m_operations;     /**< an operation node list object containing the operations of the group */
  ConstStr_t                    m_filename;       /**< the filename where the group was declared */
  int                           m_lineno;         /**< the line number where the group was declared */
  Group_t*                      m_next;

  Group_t( ConstStr_t _symbol, Vect_t<Operation_t>& _operations, ConstStr_t _filename, int _lineno );
};

#endif // OPERATION_HH
