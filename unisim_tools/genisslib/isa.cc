/***************************************************************************
                                    isa.cc
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

#include <isa.hh>
#include <operation.hh>
#include <subdecoder.hh>
#include <strtools.hh>
#include <action.hh>
#include <comment.hh>
#include <sourcecode.hh>
#include <bitfield.hh>
#include <variable.hh>
#include <specialization.hh>
#include <scanner.hh>
#include <product.hh>
#include <riscgenerator.hh>
#include <ciscgenerator.hh>
#include <unistd.h>
#include <cassert>
#include <cerrno>
#include <iostream>
#include <algorithm>

/**
 * Construct for Opts (Default global options) */
Opts::Opts(char const* _outprefix)
  : outputprefix(_outprefix)
  , verbosity( 1 )
  , expandname( 0 )
  , depfilename( 0 )
  , minwordsize( 32 )
  , sourcelines( true )
  , privatemembers( true )
  , specialization( true )
  , comments( true )
{}

/**
 *  @brief a null ouput stream useful for ignoring too verbose logs
 */

/**
 *  @brief returns the output stream for a given level of verbosity
 */
std::ostream&
Opts::log( unsigned int level ) const
{
  if (level > verbosity)
    {
      // Return a null ostream
      static struct onullstream : public std::ostream
      {
        struct nullstreambuf: public std::streambuf
        {
          int_type overflow( int_type c ) { return traits_type::not_eof(c); }
        };
        onullstream() : std::ostream(0) { rdbuf(&m_sbuf); }
        nullstreambuf m_sbuf;
      } ons;
      return ons;
    }
  return std::cerr;
}

/** Constructor of Isa instance 
 */
Isa::Isa()
  : m_decoder( RiscDecoder )
  , m_is_subdecoder( false )
  , m_withcache( false )
  , m_withsource( false )
  , m_withencode( false )
  , m_little_endian( false )
  , m_asc_forder( false )
  , m_asc_worder( false )
  , m_minwordsize( 0 )
{}

/** Destructor for isa instance
 *  Where most of the allocated stuff will be released
 */
Isa::~Isa() {}

/** Search for an operation object
    @param operation_symbol a symbol object representing the operation
    @return the matching operation object, null if no operation object matches
*/
Operation*
Isa::operation( ConstStr _symbol )
{
  for (Vector<Operation>::iterator op = m_operations.begin(); op < m_operations.end(); ++ op)
    if ((**op).symbol == _symbol) return *op;
  
  return 0;
}

/** Add an operation object to the global operation object list
    (m_operations) and to active group accumulators (m_group_accs)
    @param operation the operation object to add
*/
void
Isa::add( Operation* _op )
{
  check_name_validity("operation", _op->symbol, _op->fileloc);
  m_operations.append( _op );
  for (GroupAccumulators::iterator itr = m_group_accs.begin(), end = m_group_accs.end(); itr != end; ++itr)
    {
      // Check for duplicates ?
      for (Vector<Operation>::const_iterator found = itr->second->operations.begin(), fend = itr->second->operations.end();; ++found)
        {
          if (found == fend)
            { itr->second->operations.append( _op ); break; }
          if ((**found).symbol == _op->symbol)
            break;
        }
    }
}

/** Search the global group lists for the given symbol
    @param _symbol the group symbol looked for
    @return the found group
*/
Group*
Isa::group( ConstStr _symbol )
{
  for (Vector<Group>::iterator group = m_groups.begin(); group < m_groups.end(); ++ group)
    if ((*group)->symbol == _symbol) return *group;
  
  return 0;
}

/** Search for an action prototype object
    @param action_proto_symbol a symbol object representing the action prototype
    @return the matching action prototype object, null if no action prototype object matches
*/
ActionProto const*
Isa::actionproto( ConstStr _symbol ) const
{
  for (Vector<ActionProto>::const_iterator proto = m_actionprotos.begin(); proto < m_actionprotos.end(); ++ proto)
    if( (*proto)->m_symbol == _symbol ) return *proto;

  return 0;
}

/** Dump all objects from the global objects list (source_code_list, action_proto_list, operation_list) into a stream
    with the same format as in the original code
    @param _sink a stream
*/
void
Isa::expand( std::ostream& _sink ) const
{
  // dumping namespace
  if( not m_namespace.empty() ) {
    _sink << "namespace ";
    char const* sep = "";
    for (std::vector<ConstStr>::const_iterator piece = m_namespace.begin(); piece < m_namespace.end(); sep = "::", ++ piece)
      _sink << sep << (*piece);
    _sink << '\n';
  }
  
  // dumping template parameters
  if( not m_tparams.empty() ) {
    _sink << "template <";
    char const* sep = "";
    for (Vector<CodePair>::const_iterator iter = m_tparams.begin(); iter < m_tparams.end(); sep = ", ", ++ iter)
      _sink << sep << *(*iter);
    _sink << ">\n";
  }
  _sink << '\n';
  
  // dumping global parameters
  _sink << "set endianness " << (m_little_endian ? "little" : "big") << "\n";
  _sink << "set addressclass {" << m_addrtype << "}\n";
  
  for (Vector<SourceCode>::const_iterator srccode = m_decl_srccodes.begin(); srccode < m_decl_srccodes.end(); ++ srccode)
    _sink << "decl " << *(*srccode) << "\n\n";
  for (Vector<SourceCode>::const_iterator srccode = m_impl_srccodes.begin(); srccode < m_impl_srccodes.end(); ++ srccode)
    _sink << "impl " << *(*srccode) << "\n\n";
  _sink << '\n';

  if( not m_vars.empty() ) {
    _sink << "var ";
    char const* sep = "";
    for (Vector<Variable>::const_iterator var = m_vars.begin(); var < m_vars.end(); sep = ", ", ++ var)
      _sink << sep << *(*var);
    _sink << '\n';
  }
  _sink << '\n';
  
  for (Vector<ActionProto>::const_iterator ap = m_actionprotos.begin(); ap < m_actionprotos.end(); ++ap)
    _sink << *(*ap) << '\n';
  
  _sink << '\n';
  
  for (Vector<Operation>::const_iterator op = m_operations.begin(); op < m_operations.end(); ++ op)
    _sink << *(*op) << '\n';
}

Generator*
Isa::generator( Opts const& _options )
{
  switch (m_decoder)
    {
    case RiscDecoder: return new RiscGenerator( *this, _options );
    case CiscDecoder: return new CiscGenerator( *this, _options );
      // case VliwDecoder: return new VliwGenerator( this ); break;
    default: break;
    }
  
  assert( false );
  
  return 0;
}

bool
Isa::sanity_checks() const
{
  if (not m_addrtype.str())
    {
      std::cerr << "error: no architecture address type found." << std::endl;
      return false;
    }
  
  if (m_operations.size() == 0)
    {
      std::cerr << "error: no operation ins ISA descriptions." << std::endl;
      return false;
    }
  
  // Checking operations
  for (Vector<Operation>::const_iterator op = m_operations.begin(); op < m_operations.end(); ++ op) {
    // Looking for bitfield conflicts
    for (Vector<BitField>::const_iterator bf = (**op).bitfields.begin(); bf < (**op).bitfields.end(); ++ bf) {
      ConstStr bf_symbol = (**bf).getsymbol();
      if (not bf_symbol.str()) continue;
      for (Vector<BitField>::const_iterator pbf = (**op).bitfields.begin(); pbf < bf; ++ pbf) {
        ConstStr pbf_symbol = (**pbf).getsymbol();
        if (pbf_symbol != bf_symbol) continue;
        (**op).fileloc.err( "error: duplicated bit field `%s' in operation `%s'", bf_symbol.str(), (**op).symbol.str() );
        return false;
      }
    }
    // Looking for variable conflicts
    if( not (**op).variables.empty() ) {
      for (Vector<Variable>::const_iterator checked = (**op).variables.begin(); checked < (**op).variables.end(); ++ checked) {
        Vector<Variable>::const_iterator found;
        for (found = m_vars.begin(); found < m_vars.end(); ++ found)
          if ((**found).symbol == (**checked).symbol) break;
        
        if( (found < m_vars.end()) ) {
          (**checked).ctype->fileloc.err( "error: in operation `%s', variable `%s' is already defined as global",
                                              (**op).symbol.str(), (**checked).symbol.str() );
          (**found).ctype->fileloc.err( "variable `%s' previously defined here", (**found).symbol.str() );
          return false;
        }
        
        for (found = (**op).variables.begin(); found < checked; ++ found)
          if( (**found).symbol == (**checked).symbol ) break;

        if (found < checked) {
          (**checked).ctype->fileloc.err( "error: in operation `%s', variable `%s' is defined several times", (**op).symbol.str(), (**checked).symbol.str() );
          return false;
        }
      }
    }
  }
  return true;
}

/** Output a rule file (<filename>) suitable for make describing the dependencies of the main source file.
    @param _sink a stream
*/
void
Isa::deps( std::ostream& _sink, char const* _prefix ) const
{
  if( m_tparams.empty() ) {
    _sink << _prefix << ".cc " << _prefix << ".hh:";
  } else {
    _sink << _prefix << ".tcc " << _prefix << ".hh:";
  }
  for (std::vector<ConstStr>::const_iterator inc = m_includes.begin(); inc < m_includes.end(); ++ inc)
    _sink << " \\\n " << *inc;
  _sink << "\n\n";
}

void
Isa::specialize()
{
  for (Vector<Specialization>::iterator spec = m_specializations.begin(); spec < m_specializations.end(); ++ spec) {
    m_operations.push_back( (**spec).newop() );
  }
}

void
Isa::setparam( ConstStr key, ConstStr value )
{
  static ConstStr    codetype( "codetype",        Scanner::symbols );
  static ConstStr      scalar( "scalar",          Scanner::symbols );
  static ConstStr      buffer( "buffer",          Scanner::symbols );
  static ConstStr  subdecoder( "subdecoder_p",    Scanner::symbols );
  static ConstStr   withcache( "withcache_p",     Scanner::symbols );
  static ConstStr  withsource( "withsource_p",    Scanner::symbols );
  static ConstStr withcomment( "withcomment_p",   Scanner::symbols );
  static ConstStr  withencode( "withencode_p",    Scanner::symbols );
  static ConstStr      istrue( "true",            Scanner::symbols );
  static ConstStr     isfalse( "false",           Scanner::symbols );
  static ConstStr  endianness( "endianness",      Scanner::symbols );
  static ConstStr       isbig( "big",             Scanner::symbols );
  static ConstStr    islittle( "little",          Scanner::symbols );
  static ConstStr      forder( "fields_order",    Scanner::symbols );
  static ConstStr      worder( "words_order",     Scanner::symbols );
  static ConstStr      isdesc( "descending",      Scanner::symbols );
  static ConstStr       isasc( "ascending",       Scanner::symbols );
  
  if      (key == codetype) {
    if      (value == scalar) m_decoder = RiscDecoder;
    else if (value == buffer) m_decoder = CiscDecoder;
    else throw UnknownIdent( value );
  }
  
  else if (key == subdecoder) {
    if      (value == istrue)  { m_is_subdecoder = true; m_withcache = false; }
    else if (value == isfalse) { m_is_subdecoder = false; }
  }
  
  else if (key == withcache) {
    if      (value == istrue)  { m_withcache = true; m_is_subdecoder = false; }
    else if (value == isfalse) { m_withcache = false; }
  }
  
  else if (key == withsource) {
    if      (value == istrue)  m_withsource = true;
    else if (value == isfalse) m_withsource = false;
    else throw UnknownIdent( value );
  }
  
  else if (key == withencode) {
    if      (value == istrue)  m_withencode = true;
    else if (value == isfalse) m_withencode = false;
    else throw UnknownIdent( value );
  }
  
  else if (key == endianness) {
    if      (value == isbig)    m_little_endian = false;
    else if (value == islittle) m_little_endian = true;
    else throw UnknownIdent( value );
  }
  
  else if (key == forder) {
    if      (value == isdesc) m_asc_forder = false;
    else if (value == isasc)  m_asc_forder = true;
    else throw UnknownIdent( value );
  }
  
  else if (key == worder) {
    if      (value == isdesc) m_asc_worder = false;
    else if (value == isasc)  m_asc_worder = true;
    else throw UnknownIdent( value );
  }
  
  else throw UnknownIdent( key );
}

void
Isa::setparam( ConstStr key, SourceCode const& value )
{
  static ConstStr  addressclass( "addressclass",  Scanner::symbols );
  static ConstStr codetypeclass( "codetypeclass", Scanner::symbols );
  
  if        (key == addressclass) {
    m_addrtype = value.content;
  } else if (key == codetypeclass) {
    //m_codetype = value->content;
  }
  
  else throw UnknownIdent( key );
}

void
Isa::setparam( ConstStr key, unsigned int value )
{
  static ConstStr  minwordsize( "minwordsize", Scanner::symbols );
  
  if        (key == minwordsize) {
    m_minwordsize = value;
  }
  
  else throw UnknownIdent( key );
}

SDClass const*
Isa::sdclass( std::vector<ConstStr>& _namespace ) const
{
  for (Vector<SDClass>::const_iterator sdc = m_sdclasses.begin(); sdc != m_sdclasses.end(); ++ sdc) {
    if( (**sdc).m_namespace == _namespace ) return *sdc;
  }
  return 0;
}

SDInstance const*
Isa::sdinstance( ConstStr _symbol ) const
{
  for (Vector<SDInstance>::const_iterator sdi = m_sdinstances.begin(); sdi != m_sdinstances.end(); ++ sdi) {
    if( (**sdi).symbol == _symbol ) return *sdi;
  }
  return 0;
}

void
Isa::group_command( ConstStr group_symbol, ConstStr _command, FileLoc const& fl )
{
  static ConstStr  group_begin( "begin",  Scanner::symbols );
  static ConstStr  group_end  ( "end",    Scanner::symbols );
  
  if (_command == group_begin)
    {
      m_group_accs[group_symbol] = new Group( group_symbol, fl );
    }
  else if (_command == group_end)
    {
      GroupAccumulators::iterator ga = m_group_accs.find( group_symbol );

      /* group accumulator should exist */
      if (ga == m_group_accs.end()) {
        fl.err( "error: no corresponding `group %s %s' to `group %s %s' directive.",
                group_symbol.str(), group_begin.str(), group_symbol.str(), group_end.str() );
        throw ParseError();
      }

      // check group name for conflicts
      check_name_validity("group", group_symbol, fl);
      
      m_groups.push_back( ga->second );
      m_group_accs.erase( ga );
    }
}

void
Isa::check_name_validity(char const* kind, ConstStr symbol, FileLoc const& fl)
{
  /* Operations and groups name should not conflict */
  if (Operation* prev_op = operation( symbol ))
    {
      fl.err( "error: %s name conflicts with operation `%s'", kind, symbol.str() );
      prev_op->fileloc.err( "operation `%s' previously defined here", symbol.str() );
      throw ParseError();
    }

  if (Group* prev_gr = group( symbol ))
    {
      fl.err( "error: %s name conflicts with group `%s'", kind, symbol.str() );
      prev_gr->fileloc.err( "group `%s' previously defined here", symbol.str() );
      throw ParseError();
    }
}

bool
Isa::for_ops( ConstStr group_symbol, OOG& oog )
{
  if (Operation* op = operation( group_symbol ))
    {
      oog.with(*op);
    }
  else if (Group* gr = group( group_symbol ))
    {
      for (Vector<Operation>::iterator gop = gr->operations.begin(); gop < gr->operations.end(); ++ gop)
        oog.with(**gop);
    }
  else
    return false;
  return true;
}

void
Isa::reorder( Vector<BitField>& bitfields )
{
  bool rev_forder = m_asc_forder xor m_little_endian;
  bool rev_worder = m_asc_worder xor m_little_endian;
  
  // Reorder bitfield words if needed
  if (rev_worder)
    {
      std::reverse(bitfields.begin(), bitfields.end());
      // This operation has invert fields order
      rev_forder = not rev_forder;
    }

  // Reorder bitfield within words if needed
  if (rev_forder)
    {
      for (Vector<BitField>::iterator fbeg = bitfields.begin(), fend = fbeg, fmax = bitfields.end(); fbeg < fmax; fbeg = fend = fend + 1)
        {
          while (fend < fmax and (not dynamic_cast<SeparatorBitField const*>( &**fend ))) ++fend;
          std::reverse(fbeg, fend);
        }
    }
}

void
Isa::finalize( Opts const& options )
{
  // if (m_is_subdecoder)
  //   m_withcache = false;
  
  // Computing instruction size properties
  m_insnsizes.clear();
  for (auto& op : m_operations)
    {
      bool vlen = false;
      unsigned common = 0;
      std::set<unsigned> insnsizes = {0};
      for (auto const& bf : op->bitfields)
        {
          if      (FixedSizeBitField const* fbf = bf->as_fixedsize())
            common += fbf->size;
          else if (VariableSizeBitField const* vbf = bf->as_variablesize())
            {
              vlen = true;
              std::set<unsigned> nis;
              for (auto s1 : insnsizes)
                for (auto s2 : vbf->sizes())
                  nis.insert(s1+s2);
              std::swap(insnsizes, nis);
            }
        }
      op->size = *insnsizes.rbegin() + common;
      op->vlen = vlen;
      for (auto altsize : insnsizes)
        m_insnsizes.insert(altsize + common);
    }

  // Creating OpCode structures
  for (auto op : m_operations)
    {
      op->code.setcapacity(maxsize());
      Vector<BitField> const& bitfields = op->bitfields;
      bool vlen = false;
      for (FieldIterator fi(bitfields, m_little_endian); fi.next(); )
        {
          if (fi.bitfield().as_variablesize())
            {
              vlen = true;
              continue;
            }
        
          if (FixedSizeBitField const* fbf = fi.bitfield().as_fixedsize())
            {
              if (not fbf->hasopcode())
                continue;
              if (vlen)
                {
                  op->fileloc.loc( std::cerr ) << " error: cannot have opcodes appearing in variable length part." << std::endl;
                  throw ParseError();
                }
              op->code.setbits(fi.beg, fi.end, fbf->bits());
              op->code.setmask(fi.beg, fi.end, fbf->mask());
            }
        }
    }

  // Performing Topological Sort
  struct OpNode
  {
    OpNode(Operation const* _op)
      : op(_op), belowlist(), abovecount(0)
    {}
    Operation const* op;
    std::set<OpNode*> belowlist;
    uintptr_t abovecount;
    
    void ontopof(OpNode* nbelow)
    {
      for (auto below : belowlist)
        {
          if (below == nbelow)
            return;
          auto loc = nbelow->op->locate( *below->op );
          if (loc == Operation::Code::Inside)
            return;
          if (loc != Operation::Code::Contains)
            continue;
          // edge is redundant
          below->abovecount -= 1;
          belowlist.erase(below);
          // No more related edges
          break;
        }
      belowlist.insert(nbelow);
      nbelow->abovecount += 1;
    }

    bool above(OpNode* op)
    {
      std::set<OpNode*> seen, front(belowlist);

      while (front.size())
        {
          seen.insert(front.begin(), front.end());
          std::set<OpNode*> visit;
          std::swap(front, visit);

          for (auto pivot : visit)
            {
              if (pivot == op)
                return true;
              for (auto node : pivot->belowlist)
                {
                  if (seen.count(node))
                    continue;
                  front.insert(node);
                }
            }
        }
      return false;
    }
    
    void unlink()
    {
      for (auto below : belowlist)
        below->abovecount -= 1;
      belowlist = std::set<OpNode*>();
    }
  };

  std::map<Operation*, OpNode> nodes;
  for (auto op : m_operations)
    {
      nodes.emplace(op, op);
    }
    
  // 1. Computing implicit edges
  for (auto& n1 : nodes)
    {    
      for (auto& n2 : nodes)
        {
          if (&n2 == &n1)
            break;
          switch (n1.first->locate(*n2.first))
            {
            default: break;
            case Operation::Code::Equal:
              n1.first->fileloc.err( "error: operation `%s' duplicates operation `%s'", n1.first->symbol.str(), n2.first->symbol.str() );
              n2.first->fileloc.err( "operation `%s' was declared here", n2.first->symbol.str() );
              throw ParseError();
              break;
            case Operation::Code::Contains: n1.second.ontopof( &n2.second ); break;
            case Operation::Code::Inside:   n2.second.ontopof( &n1.second ); break;
            }
        }
    }
  
  // 2. log if verbose enough
  if (options.verbosity >= 2)
    {
      for (auto& node : nodes)
        {
          uintptr_t count = node.second.belowlist.size();
          if (count == 0) continue;
          options.log(2) << "operation `" << node.first->symbol.str() << "' is a specialization of operation" << (count>1?"s ":" ");
          char const* sep = "";
          for (auto below : node.second.belowlist)
            {
              options.log(2) << sep << "`" << below->op->symbol.str() << "'";
              sep = ", ";
            }
          options.log(2) << std::endl;
        }
    }
  // 3. Adding explicit edges
  for (auto const& user_order : m_user_orderings)
    {
      // Unrolling specialization relations
      typedef Vector<Operation> OpV;
      
      struct : Isa::OOG { void with( Operation& operation ) { ops.append( &operation ); } OpV ops; } above, below;
    
      if (not for_ops( user_order.top_op, above ))
        {
          user_order.fileloc.loc( std::cerr ) << "error: no such operation or group `" << user_order.top_op.str() << "'" << std::endl;
          throw ParseError();
        }
      
      for (auto under_op : user_order.under_ops)
        {
          if (not for_ops( under_op, below ))
            {
              user_order.fileloc.loc( std::cerr ) << "error: no such operation or group `" << under_op.str() << "'" << std::endl;
              throw ParseError();
            }
        }
      
      // Check each user specialization and insert when valid
      FileLoc const& sloc = user_order.fileloc;
      for (auto aop : above.ops)
        {
          for (auto bop : below.ops)
            {
              ConstStr action = Str::fmt("specialization of {!r} with {!r}", bop->symbol.str(), aop->symbol.str());
              switch (aop->locate( *bop ))
                {
                default: break;
                case Operation::Code::Outside:
                  sloc.loc( options.log(1) ) << Str::fmt("warning: useless %s (no relation)\n", action.str()).str();
                  break;
                case Operation::Code::Contains:
                  sloc.loc( options.log(1) ) << Str::fmt("warning: useless %s (implicit specialization)\n", action.str()).str();
                  break;
                case Operation::Code::Inside:
                  sloc.loc( std::cerr ) << Str::fmt("error: illegal %s (would hide the former)\n", action.str()).str();
                  throw ParseError();
                  break;
                case Operation::Code::Overlaps:
                  nodes.find(aop)->second.ontopof(&nodes.find(bop)->second);
                  options.log(2) << Str::fmt("info: forced %s\n", action.str()).str();
                  break;
                }          
            }
        }
    }
  
  // 4. Finally check if overlaps are resolved
  for (auto& n1 : nodes)
    {
      for (auto& n2 : nodes)
        {
          if (&n1 == &n2)
            break;
          if (n1.first->locate( *n2.first ) != Operation::Code::Overlaps) continue;
          if (n1.second.above( &n2.second )) continue;
          if (n2.second.above( &n1.second )) continue;
          n1.first->fileloc.err( "error: operation `%s' conflicts with operation `%s'", n1.first->symbol.str(), n2.first->symbol.str() );
          n2.first->fileloc.err( "operation `%s' was declared here", n2.first->symbol.str() );
          std::cerr << "  " << n1.first->match_info(m_little_endian) << ": " << n1.first->symbol.str() << std::endl;
          std::cerr << "  " << n2.first->match_info(m_little_endian) << ": " << n2.first->symbol.str() << std::endl;
          throw ParseError();
        }
    }
  
  // 5. perform the topological sort itself
  {
    intptr_t opcount = m_operations.size();
    Vector<Operation> noperations( opcount );
    intptr_t
      sopidx = opcount, // operation source table index
      dopidx = opcount, // operation destination table index
      inf_loop_tracker = opcount; // counter tracking infinite loop
    
    while( dopidx > 0 ) {
      sopidx = (sopidx + opcount - 1) % opcount;
      Operation* op = m_operations[sopidx];
      if (not op) continue;

      auto node = nodes.find(op);
      assert (node != nodes.end());
      if (node->second.abovecount > 0)
        {
          // There is some operations to be placed before this one 
          --inf_loop_tracker;
          assert( inf_loop_tracker >= 0 );
          continue;
        }
      inf_loop_tracker = opcount;
      noperations[--dopidx] = op;
      m_operations[sopidx] = 0;
      node->second.unlink();
    }
    m_operations = noperations;
  }
}

/**
 *  @brief computes the greatest common divisor of instruction lengths (in bits).
 */
unsigned
Isa::gcd() const
{
  unsigned int res = *m_insnsizes.begin();
  for (std::set<unsigned int>::const_iterator itr = m_insnsizes.begin(); itr != m_insnsizes.end(); ++itr) {
    unsigned int cur = *itr;
    for (;;) {
      unsigned int rem = cur % res;
      if (rem == 0) break;
      cur = res;
      res = rem;
    }
  }
  return res;
}

unsigned
Isa::maxsize() const
{
  return *m_insnsizes.rbegin();
}

/** Constructor of the FieldIterator object
 *  @brief Initialize the FieldIterator
 *  @param _bitfields the bitfields to iterate over
 *  @param _little_endian the ISA encoding endianness
 */
FieldIterator::FieldIterator( BitFields const& _bitfields, bool _little_endian )
  : bitfields(_bitfields), little_endian(_little_endian), nbf(_bitfields.begin()), bf(_bitfields.end()), beg(0), end(0)
{}

/** Move the iterator to the next item
 *  @return iterator validity (false if moved past-the-end)
 */
bool
FieldIterator::next()
{
  if (nbf == bitfields.end())
    return false;
  bf = nbf;
  ++nbf;
  int size = (**bf).maxsize();
  
  if (little_endian) { beg = end; end = end + size; }
  else               { end = beg; beg = beg - size; }
  return true;
}
