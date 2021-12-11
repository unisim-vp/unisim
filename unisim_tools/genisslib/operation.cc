/***************************************************************************
                                operation.cc
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

/** Create an operation object
    @param symbol a symbol object representing the operation
    @param bitfield_list a bit field list object containing the bit fields of the operation
    @param comment_list a C/C++ comment list object containing C/C++ comments relative to the operation
    @param variable_list a variable list object
    @param filename a filename object where the operation was found
    @param lineno a line number where the operation was found
    @return an operation object
*/
Operation::Operation( ConstStr _symbol, Vector<BitField> const& _bitfields, Vector<Comment> const& _comments, SourceCode* cond, FileLoc const& floc )
  : symbol( _symbol )
  , bitfields( _bitfields )
  , comments( _comments )
  , condition( cond )
  , fileloc( floc )
  , size(0)
  , vlen(false)
{}

/** Delete an operation object
    @param operation an operation object to delete
*/
Operation::~Operation() {}

/** Create a group object
    @param operation_list the list of the operation of the group
    @returns a group list object
*/
Group::Group( ConstStr _symbol, Vector<Operation>& _oplist, FileLoc const& _fileloc )
  : symbol( _symbol ), operations( _oplist ), fileloc( _fileloc )
{}

/** Create a group object
    @param _symbol the symbol naming the group
    @param _fileloc the source file location of the group declaration
    @returns a group list object
*/
Group::Group( ConstStr _symbol, FileLoc const& _fileloc )
  : symbol( _symbol ), fileloc( _fileloc )
{}

/** Search the operation for an action implementing an action prototype
    @param actionproto an action prototype object
    @return the matching action object, null if no action object matches
*/
Action const*
Operation::action( ActionProto const* proto ) const
{
  for (Vector<Action>::const_iterator iter = actions.begin(); iter < actions.end(); ++ iter)
    if ((**iter).m_actionproto == proto) return *iter;
  
  return 0;
}

void
Operation::add( Action* _action )
{
  actions.push_back( _action );
  assert( not _action->m_operation );
  _action->m_operation = this;
}

/** Dump an operation object into a stream
    @param operation an operation object to dump
    @param sink a stream
*/

std::ostream&
operator<<( std::ostream& sink, Operation const& _op )
{
  sink << "op " << _op.symbol << "( ";
  
  char const* sep = "";
  for (Vector<BitField>::const_iterator bf = _op.bitfields.begin(); bf < _op.bitfields.end(); sep = " : ", ++ bf)
    sink << sep << (**bf);
  
  sink << " )\n";
  
  if (not _op.variables.empty())
    {
      sink << _op.symbol << ".var ";
      sep = "";
      for( Vector<Variable>::const_iterator var = _op.variables.begin(); var < _op.variables.end(); sep = ", ", ++ var )
        sink << sep << *(*var);
    }
  sink << "\n\n";
  
  for( Vector<Action>::const_iterator action = _op.actions.begin(); action < _op.actions.end(); ++ action )
    sink << *(*action) << "\n\n";
  
  return sink;
}

/** Dump an operation object into a stream
    @param operation an operation object to dump
    @param sink a stream
*/

Operation::Code::Code()
  : mask(0), bits(0)
{
  
}

Operation::Code::~Code()
{
  delete [] mask;
}

void
Operation::Code::setcapacity( unsigned int capacity )
{
  unsigned bytecount = (capacity + 7) / 8;
  mask = new uint8_t[2*bytecount];
  std::fill(&mask[0], &mask[2*bytecount], 0);
  bits = &mask[bytecount];
}

uint64_t
Operation::Code::get(int beg, int end, member_t member) const
{
  uint64_t result = 0;
  for (int pos = beg; pos < end;)
    {
      int npos = std::min<int>((pos+8) & -8, end);
      unsigned shift = pos & 7;
      unsigned byte = (pos >> 3) ^ ((pos < 0) * -1);
      uint64_t mask = (1 << (npos-pos)) - 1;
      assert (mask);
          
      result |= ((uint64_t((this->*member)[byte]) >> shift) & mask) << (pos - beg);
      pos = npos;
    }
  return result;
}

void
Operation::Code::set(int beg, int end, uint64_t value, member_t member)
{
  for (int pos = beg; pos < end;)
    {
      int npos = std::min<int>((pos+8) & -8, end);
      unsigned shift = pos & 7;
      unsigned byte = (pos >> 3) ^ ((pos < 0) * -1);
      uint64_t mask = (1 << (npos-pos)) - 1;
      assert (mask);
          
      (this->*member)[byte] |= (((value >> (pos - beg)) & mask) << shift);
      pos = npos;
    }
}


/**
 *  @brief  Locate argument opcode from the subject opcode point of view.
 *  @param  rhs the opcode to locate
 *  @param  size the bits considered for the comparison 
 *  @return the argument object location
 *
 *  This function will return the argument opcode location according
 *  to the subject opcode location. Relative locations may be:
 *    - Outside
 *    - Overlaps
 *    - Inside
 *    - Contains
 *    - Equal
 */
Operation::Code::location_t
Operation::Code::locate(Code const& rhs, unsigned size) const
{
  bool some_a_outside_B = false;
  bool some_b_outside_A = false;

  for (unsigned idx = 0, end = (size+7)/8; idx < end; ++idx)
    {
      if ((mask[idx] & rhs.mask[idx]) & (bits[idx] ^ rhs.bits[idx]))
        return Outside;
      if (~mask[idx] & rhs.mask[idx])
        some_a_outside_B = true;
      if (mask[idx] & ~rhs.mask[idx])
        some_b_outside_A = true;
    }
      
  static location_t const table[] = {Equal, Inside, Contains, Overlaps};
  return table[1*some_a_outside_B | 2*some_b_outside_A];
}

Operation::Code::location_t
Operation::locate(Operation const& rhs) const
{
  return code.locate(rhs.code, std::max(size, rhs.size));
}

void
Operation::match_info(std::ostream& sink, bool little_endian) const
{
  struct
  {
    void print(std::ostream& sink, unsigned bits, unsigned mask, unsigned size)
    {
      for (unsigned bit = size; bit-->0;)
        sink << "..01"[((bits >> bit) & 1)*1 + ((mask >> bit) & 1)*2];
    }
  } bin;

  for (unsigned byte = 0, end = size >> 3; byte < end; ++byte)
    bin.print(sink << ' ', code.bits[byte], code.mask[byte], 8);

  unsigned rem = size & 7, cut = size & -8;
  int beg, end;
  if (little_endian) { beg =   cut; end = size; }
  else               { beg = -size; end = -cut; }
  bin.print(sink << ' ', code.getbits(beg, end), code.getmask(beg, end), rem);
}

