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

#include <main.hh>
#include <fwd.hh>
#include <vect.hh>
#include <conststr.hh>
#include <errtools.hh>
#include <map>
#include <memory>
#include <iosfwd>
#include <referencecounting.hh>

struct Isa
{
  enum DecoderType_t { RiscDecoder = 0, CiscDecoder, VliwDecoder };
  DecoderType_t                 m_decoder;         /**< Decoder Type */
  bool                          m_is_subdecoder;   /**< Subdecoder or full decoder */
  bool                          m_withsource;      /**< Action source code accessible or not */
  bool                          m_withencode;      /**< Action source code accessible or not */
  bool                          m_little_endian;   /**< Endianness of isa (false: big endian, true: little endian) */
  bool                          m_asc_forder;      /**< bitfields ordering (false: descending)*/
  bool                          m_asc_worder;      /**< words ordering (false: descending)*/
  unsigned int                  m_minwordsize;     /**< minimum C type size for operand fields */
  std::vector<ConstStr>       m_namespace;       /**< Encapsulating namespace of the iss */
  Vect_t<CodePair_t>            m_tparams;         /**< Template parameters of the iss */
  Vect_t<Variable_t>            m_vars;            /**< Global variables used by the iss */
  Vect_t<ActionProto>           m_actionprotos;    /**< Action prototypes of operations */
  Vect_t<Operation_t>           m_operations;      /**< Defined instructions */
  Vect_t<Group_t>               m_groups;          /**< Defined groups */
  Vect_t<SDClass_t>             m_sdclasses;       /**< Defined subdecoder classes */
  Vect_t<SDInstance_t>          m_sdinstances;     /**< Defined subdecoder instances */
  Vect_t<SourceCode_t>          m_decl_srccodes;   /**< Code to insert in header file */
  Vect_t<SourceCode_t>          m_impl_srccodes;   /**< Code to insert in source file */
  ConstStr                    m_addrtype;        /**< C type for instructions addresses */
  std::vector<ConstStr>       m_includes;        /**< files included by the isa main file */
  Vect_t<Specialization_t>      m_specializations; /**< Requested specializations */
  Vect_t<Inheritance_t>         m_inheritances;    /**< Defined inheritances for operation class */
  
  typedef std::map<ConstStr,Group_t*> GroupAccumulators;
  GroupAccumulators             m_group_accs;      /**< Active group accumulators */
  
  struct Ordering { FileLoc_t fileloc; std::vector<ConstStr> symbols; };
  typedef std::vector<Ordering> Orderings;
  Orderings                     m_user_orderings;

  Isa();
  ~Isa();
  
  void                          remove( Operation_t* _op );
  void                          add( Operation_t* _op );
  void                          remove( ActionProto const* _ap );
  Operation_t*                  operation( ConstStr _symbol );
  bool                          operations( ConstStr _symbol, Vect_t<Operation_t>& _opvec );
  Group_t*                      group( ConstStr _symbol );
  ActionProto const*            actionproto( ConstStr _symbol ) const;
  SDClass_t const*              sdclass( std::vector<ConstStr>& _namespace ) const;
  SDInstance_t const*           sdinstance( ConstStr _symbol ) const;

  Generator*                    generator( Isa& _source, Opts const& _options ) const;
  
  void                          expand( std::ostream& _sink ) const;
  void                          deps( std::ostream& _sink, char const* _prefix ) const;
  bool                          sanity_checks() const;
  void                          specialize();
  
  struct UnknownIdent
  {
    ConstStr m_ident;
    UnknownIdent( ConstStr ident ) : m_ident( ident ) {}
  };
  
  void                          setparam( ConstStr key, ConstStr value );
  void                          setparam( ConstStr key, SourceCode_t* value );
  void                          setparam( ConstStr key, unsigned int value );
  
  struct ParseError {};
  
  void                          group_command( ConstStr _symbol, ConstStr _command, FileLoc_t const& fl );
};

#endif // __ISA_HH__
