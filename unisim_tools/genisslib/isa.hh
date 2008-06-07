/***************************************************************************
                                   isa.hh
                             -------------------
    begin                : Thu May 25 2003
    copyright            : (C) 2003-2007 CEA and Universite Paris Sud
    authors              : Gilles Mouchard, Yves Lhuillier
    email                : gilles.mouchard@cea.fr, yves.lhuillier@cea.fr
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License version 2        *
 *   as published by the Free Software Foundation.                         *
 *                                                                         *
 ***************************************************************************/

#ifndef __ISA_HH__
#define __ISA_HH__

#include <fwd.hh>
#include <vect.hh>
#include <conststr.hh>
#include <memory>
#include <referencecounting.hh>

struct Isa {
  enum DecoderType_t { RiscDecoder = 0, CiscDecoder, VliwDecoder };
  DecoderType_t                 m_decoder;         /**< Decoder Type */
  bool                          m_is_subdecoder;   /**< Subdecoder or full decoder */
  bool                          m_withsource;      /**< Action source code accessible or not */
  bool                          m_little_endian;   /**< Endianness of isa */
  std::vector<ConstStr_t>       m_namespace;       /**< Encapsulating namespace of the iss */
  Vect_t<CodePair_t>            m_tparams;         /**< Template parameters of the iss */
  Vect_t<Variable_t>            m_vars;            /**< Global variables used by the iss */
  Vect_t<ActionProto_t>         m_actionprotos;    /**< Action prototypes of operations */
  Vect_t<Operation_t>           m_operations;      /**< Defined instructions */
  Vect_t<Group_t>               m_groups;          /**< Defined groups */
  Vect_t<SDClass_t>             m_sdclasses;       /**< Defined subdecoder classes */
  Vect_t<SDInstance_t>          m_sdinstances;     /**< Defined subdecoder instances */
  Vect_t<SourceCode_t>          m_decl_srccodes;   /**< Code to insert in header file */
  Vect_t<SourceCode_t>          m_impl_srccodes;   /**< Code to insert in source file */
  ConstStr_t                    m_addrtype;        /**< C type for instructions addresses */
  std::vector<ConstStr_t>       m_includes;        /**< files included by the isa main file */
  Vect_t<Specialization_t>      m_specializations; /**< Requested specializations */
  Vect_t<Inheritance_t>         m_inheritances;    /**< Defined inheritances for operation class */

  Isa();
  ~Isa();
  
  void                          remove( Operation_t* _op );
  void                          remove( ActionProto_t const* _ap );
  Operation_t*                  operation( ConstStr_t _symbol );
  Group_t*                      group( ConstStr_t _symbol );
  ActionProto_t const*          actionproto( ConstStr_t _symbol ) const;
  SDClass_t const*              sdclass( std::vector<ConstStr_t>& _namespace ) const;
  SDInstance_t const*           sdinstance( ConstStr_t _symbol ) const;

  std::auto_ptr<Generator>      generator() const;
  
  void                          expand( std::ostream& _sink ) const;
  void                          deps( std::ostream& _sink, char const* _prefix ) const;
  bool                          sanity_checks() const;
  void                          specialize();
  void                          setparam( ConstStr_t _param );
};

#endif // __ISA_HH__
