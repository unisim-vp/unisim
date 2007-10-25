#ifndef ISA_HH
#define ISA_HH

#include <fwd.hh>
#include <vect.hh>
#include <conststr.hh>
#include <referencecounting.hh>

struct Isa {
  bool                          m_little_endian;  /**< Endianness of isa */
  std::vector<ConstStr_t>       m_namespace;      /**< Encapsulating namespace of the iss */
  Vect_t<CodePair_t>            m_tparams;        /**< Template parameters of the iss */
  Vect_t<Variable_t>            m_vars;           /**< Global variables used by the iss */
  Vect_t<ActionProto_t>         m_actionprotos;  /**< Action prototypes of operations */
  Vect_t<Operation_t>           m_operations;     /**< Defined instructions */
  Vect_t<Group_t>               m_groups;         /**< Defined groups */
  Vect_t<SourceCode_t>          m_decl_srccodes;  /**< Code to insert in header file */
  Vect_t<SourceCode_t>          m_impl_srccodes;  /**< Code to insert in source file */

  ConstStr_t                    m_addrtype;
  unsigned int                  m_maxopsize;
  unsigned int                  m_codesize;
  ConstStr_t                    m_codetype;
  ConstStr_t                    m_immedsuffix;

  Isa();
  ~Isa();
  
  void                          remove( Operation_t* _op );
  void                          remove( ActionProto_t const* _ap );
  Operation_t const*            operation_with_opcode( uint64_t _opmask, uint64_t _opcode ) const;
  Operation_t*                  operation( ConstStr_t _symbol );
  Group_t*                      group( ConstStr_t _symbol );
  ActionProto_t const*          actionproto( ConstStr_t _symbol ) const;
  Isa&                          computetypes();
  
  bool                          sanity_checks() const;
  
  unsigned int                  get_max_operation_size() const;
  void                          generate_decoder_class_definition( Product_t& _product ) const;
  void                          generate_operation_class_definition( Product_t& _product, unsigned int word_size ) const;
  void                          generate_operation_impl_class_definition( Product_t& _product, unsigned int word_size ) const;
  void                          generate_operation_constructor( Product_t& _product ) const;
  void                          generate_operation_destructor( Product_t& _product ) const;
  void                          generate_default_actions( Product_t& _product ) const;
  void                          generate_operation_member_methods( Product_t& _product, unsigned int word_size ) const;
  void                          generate_operation_constructors( Product_t& _product, unsigned int word_size ) const;
  void                          generate_decoder( Product_t& _product, unsigned int word_size ) const;
  bool                          generate_iss( Product_t& _product, unsigned int word_size ) const;

  void                          expand( std::ostream& _sink ) const;
};

#endif // ISA_HH
