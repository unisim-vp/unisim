#ifndef ACTION_HH
#define ACTION_HH

#include <fwd.hh>
#include <conststr.hh>
#include <referencecounting.hh>
#include <vect.hh>

/** An action implementation object */
struct Action_t : virtual ReferenceCounter {
  Operation_t const*       m_operation;    /**< The associated operation  */
  ActionProto_t const*     m_actionproto; /**< The associated action prototype */
  Ptr_t<SourceCode_t>      m_source_code;  /**< The C implementation of the action */
  Vect_t<Comment_t>        m_comments;     /**< The list of the C comment associated with the action */
  ConstStr_t               m_filename;     /**< the filename where the action was declared */
  int                      m_lineno;       /**< the line number where the action was declared */
  Action_t*                m_base;

  Action_t( ActionProto_t const* _actionproto,
            SourceCode_t* _source_code,
            Vect_t<Comment_t>& _comments,
            ConstStr_t _filename, int _lineno );
  ~Action_t();
  
};

std::ostream& operator<<( std::ostream& _sink, Action_t const& _act );

/** An action prototype object */
struct ActionProto_t : virtual ReferenceCounter {
  enum type_t { Constructor, Destructor, Static, Common };
  type_t                   m_type;                /**< The type of the action prototype */
  ConstStr_t               m_symbol;              /**< The associated symbol */
  Ptr_t<SourceCode_t>      m_returns;             /**< The C return type of the action */
  Vect_t<CodePair_t>       m_params;              /**< The C parameters of the action */
  Ptr_t<SourceCode_t>      m_default_source_code; /**< The default C implementation of the action */
  Vect_t<Comment_t>        m_comments;            /**< The list of the C comment associated with the action prototype */
  ConstStr_t               m_filename;            /**< the filename where the action prototype was declared */
  int                      m_lineno;              /**< the line number where the action prototype was declared */
  
  ActionProto_t( type_t _type, ConstStr_t _symbol,
                 SourceCode_t* _returns, Vect_t<CodePair_t>& _params, SourceCode_t* _default_source_code,
                 Vect_t<Comment_t>& _comments, ConstStr_t _filename, int _lineno );
  ~ActionProto_t();
  
  char const*              returntype() const;
};

std::ostream& operator<<( std::ostream& _sink, ActionProto_t const& _ap );

#endif // ACTION_HH
