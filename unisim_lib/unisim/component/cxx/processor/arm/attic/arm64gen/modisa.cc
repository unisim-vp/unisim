#include <isa.hh>
#include <bitfield.hh>
#include <action.hh>
#include <comment.hh>
#include <operation.hh>
#include <sourcecode.hh>
#include <variable.hh>
#include <scanner.hh>
#include <strtools.hh>
#include <streamtools.hh>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <cassert>

struct Corrupted : public std::logic_error { Corrupted() : std::logic_error( "Corrupted" ) {} };

struct Generator
{
  Generator() {}
  
  void transform( Isa& source );
  void writeback( Isa const& source );
};


int
main (int argc, char** argv, char** envp)
{
  Isa isa;
  
  if (not Scanner::parse( "top.isa", isa ))
    throw Corrupted();
  
  // Sanity checks for base parameters that should not move
  // {
  //   std::ostringstream buf;
  //   char const* sep = "";
  //   for (auto&& name: isa.m_namespace) { buf << sep << name; sep = "::"; }
  //   if (buf.str().compare( "unisim::component::cxx::processor::powerpc::e200" ) != 0)
  //     throw Corrupted();
  // }
  // if (not isa.m_tparams.empty())
  //   throw Corrupted();
  // if (isa.m_addrtype != "uint32_t")
  //   throw Corrupted();
  // if (not isa.m_vars.empty())
  //   throw Corrupted();
  
  {
    Generator gen;
    gen.transform( isa );
    gen.writeback( isa );
  }
  
  return 0;
}

struct DumpBitFields : public Ostreamable
{
  DumpBitFields( Vector<BitField> const& _bfs ) : bfs(_bfs) {} Vector<BitField> const& bfs;
  
  void operator () ( std::ostream& sink ) const
  {
    sink << "( ";
    char const* sep = "";
    for (auto const& bf : bfs)
      {
        sink << sep;
        sep = " : ";
        if (OpcodeBitField const* opbf = dynamic_cast<OpcodeBitField const*>( &*bf ))
          sink << Binary( opbf->value, opbf->size ) << '[' << opbf->size << ']';
        else
          sink << *bf;
      }
    
    sink << " )";
  }
};

void
Generator::writeback( Isa const& source )
{
  std::ofstream sink("simd_fp.isa");
  // std::ostream& sink = std::cout;
  
  for (auto const& op : source.m_operations)
    {
      for (auto const& comment : op->comments) {
        sink << comment->content << "\n\n";
      }
      sink << "op " << op->symbol << DumpBitFields( op->bitfields ) << ";\n";
      
      if (op->variables.size()) {
        sink << op->symbol << ".var ";
        char const* sep = "";
        for (auto const& var : op->variables)
          {
            sink << sep << *var;
            sep = ", ";
          }
        sink << ";\n\n";
      }
      
      for (auto const& action : op->actions)
        sink << *action << ";\n\n";
    }
}

char const*  pfs[] = {"_s8b", "_s16b", "_s4h", "_s8h", "_s2s", "_s4s", "_s1d", "_s2d",
                      "_d8b", "_d16b", "_d4h", "_d8h", "_d2s", "_d4s", "_d1d", "_d2d",
                      "_t8b", "_t16b", "_t4h", "_t8h", "_t2s", "_t4s", "_t1d", "_t2d",
                      "_q8b", "_q16b", "_q4h", "_q8h", "_q2s", "_q4s", "_q1d", "_q2d",
                      "_s8b_imm", "_s8b_reg", "_s16b_imm", "_s16b_reg", "_s4h_imm", "_s4h_reg", "_s8h_imm", "_s8h_reg",
                      "_s2s_imm", "_s2s_reg", "_s4s_imm", "_s4s_reg", "_s1d_imm", "_s1d_reg", "_s2d_imm", "_s2d_reg",
                      "_d8b_imm", "_d8b_reg", "_d16b_imm", "_d16b_reg", "_d4h_imm", "_d4h_reg", "_d8h_imm", "_d8h_reg",
                      "_d2s_imm", "_d2s_reg", "_d4s_imm", "_d4s_reg", "_d1d_imm", "_d1d_reg", "_d2d_imm", "_d2d_reg",
                      "_t8b_imm", "_t8b_reg", "_t16b_imm", "_t16b_reg", "_t4h_imm", "_t4h_reg", "_t8h_imm", "_t8h_reg",
                      "_t2s_imm", "_t2s_reg", "_t4s_imm", "_t4s_reg", "_t1d_imm", "_t1d_reg", "_t2d_imm", "_t2d_reg",
                      "_q8b_imm", "_q8b_reg", "_q16b_imm", "_q16b_reg", "_q4h_imm", "_q4h_reg", "_q8h_imm", "_q8h_reg",
                      "_q2s_imm", "_q2s_reg", "_q4s_imm", "_q4s_reg", "_q1d_imm", "_q1d_reg", "_q2d_imm", "_q2d_reg",
                      
                      "_sbi", "_shi", "_ssi", "_sdi",
                      "_sbi_imm", "_sbi_reg", "_shi_imm", "_shi_reg", "_ssi_imm", "_ssi_reg", "_sdi_imm", "_sdi_reg",
                      
                      "_dbi", "_dhi", "_dsi", "_ddi",
                      "_dbi_imm", "_dbi_reg", "_dhi_imm", "_dhi_reg", "_dsi_imm", "_dsi_reg", "_ddi_imm", "_ddi_reg",
                      
                      "_tbi", "_thi", "_tsi", "_tdi",
                      "_tbi_imm", "_tbi_reg", "_thi_imm", "_thi_reg", "_tsi_imm", "_tsi_reg", "_tdi_imm", "_tdi_reg",
                      
                      "_qbi", "_qhi", "_qsi", "_qdi",
                      "_qbi_imm", "_qbi_reg", "_qhi_imm", "_qhi_reg", "_qsi_imm", "_qsi_reg", "_qdi_imm", "_qdi_reg",
                      
                      "r_s8b", "r_s16b", "r_s4h", "r_s8h", 
                      "r_s2s", "r_s4s", "r_s1d", "r_s2d",
                      "r_s8b_imm", "r_s8b_reg", "r_s16b_imm", "r_s16b_reg", "r_s4h_imm", "r_s4h_reg", "r_s8h_imm", "r_s8h_reg",
                      "r_s2s_imm", "r_s2s_reg", "r_s4s_imm", "r_s4s_reg", "r_s1d_imm", "r_s1d_reg", "r_s2d_imm", "r_s2d_reg",
                      
                      "r_d8b", "r_d16b", "r_d4h", "r_d8h", 
                      "r_d2s", "r_d4s", "r_d1d", "r_d2d",
                      "r_d8b_imm", "r_d8b_reg", "r_d16b_imm", "r_d16b_reg", "r_d4h_imm", "r_d4h_reg", "r_d8h_imm", "r_d8h_reg",
                      "r_d2s_imm", "r_d2s_reg", "r_d4s_imm", "r_d4s_reg", "r_d1d_imm", "r_d1d_reg", "r_d2d_imm", "r_d2d_reg",

                      "r_t8b", "r_t16b", "r_t4h", "r_t8h", 
                      "r_t2s", "r_t4s", "r_t1d", "r_t2d",
                      "r_t8b_imm", "r_t8b_reg", "r_t16b_imm", "r_t16b_reg", "r_t4h_imm", "r_t4h_reg", "r_t8h_imm", "r_t8h_reg",
                      "r_t2s_imm", "r_t2s_reg", "r_t4s_imm", "r_t4s_reg", "r_t1d_imm", "r_t1d_reg", "r_t2d_imm", "r_t2d_reg",

                      "r_q8b", "r_q16b", "r_q4h", "r_q8h", 
                      "r_q2s", "r_q4s", "r_q1d", "r_q2d",
                      "r_q8b_imm", "r_q8b_reg", "r_q16b_imm", "r_q16b_reg", "r_q4h_imm", "r_q4h_reg", "r_q8h_imm", "r_q8h_reg",
                      "r_q2s_imm", "r_q2s_reg", "r_q4s_imm", "r_q4s_reg", "r_q1d_imm", "r_q1d_reg", "r_q2d_imm", "r_q2d_reg",
                      
};

struct CMV
{
  bool Decode( std::string opname )
  {
    uintptr_t oplen = opname.size();
      
    if (oplen < 7)
      return false;
    if ((opname[0] != 'c') or (opname[1] != 'm') or (opname[4] != '_'))
      return false;
      
    {
      std::string cmpop( opname.substr(2,2) );
      if      (cmpop == "eq") { compare_op = EQ; }
      else if (cmpop == "gt") { compare_op = GT; }
      else if (cmpop == "lt") { compare_op = LT; }
      else if (cmpop == "ge") { compare_op = GE; }
      else if (cmpop == "le") { compare_op = LE; }
      else if (cmpop == "hi") { compare_op = HI; }
      else if (cmpop == "hs") { compare_op = HS; }
      else {
        std::cerr << "Unknown comparison" << cmpop << std::endl;
        throw 0;
      }
    }
    
    if      (opname.substr(oplen-4) == "_reg")   is_reg = true;
    else if (opname.substr(oplen-5) == "_zero")  is_reg = false;
    else throw 0;
      
    {
      std::string optype( opname.substr(5, oplen - (is_reg ? 9 : 10)) );
      if      (optype == "v8b")  { elements = 8;  ebytes = 1; order = 1; }
      else if (optype == "v16b") { elements = 16; ebytes = 1; order = 2; }
      else if (optype == "v4h")  { elements = 4;  ebytes = 2; order = 3; }
      else if (optype == "v8h")  { elements = 8;  ebytes = 2; order = 4; }
      else if (optype == "v2s")  { elements = 2;  ebytes = 4; order = 5; }
      else if (optype == "v4s")  { elements = 4;  ebytes = 4; order = 6; }
      else if (optype == "d")    { elements = 1;  ebytes = 8; order = 0; }
      else if (optype == "v2d")  { elements = 2;  ebytes = 8; order = 7; }
      else throw 0;

    }
    
    switch (elements*ebytes) {
    case 8: case 16: break;
    default: throw 0;
    }
    
    return true;
  }
  
  bool comparable( CMV const& rhs ) const { return (rhs.compare_op == compare_op) and (rhs.is_reg == is_reg); }
  
  bool operator < ( CMV const& rhs ) const
  {
    if (int delta = int(compare_op) - int(rhs.compare_op)) return (delta < 0);
    if (int delta = int(rhs.is_reg) - int(is_reg)) return (delta < 0);
    return order < rhs.order;
  }
  
  char const*
  GetElemType() const
  {
    switch (compare_op)
      {
      case EQ: case GT: case LT: case GE: case LE:
        switch (ebytes)
          {
          case 1: return "S8";  break;
          case 2: return "S16"; break;
          case 4: return "S32"; break;
          case 8: return "S64"; break;
          default: throw 0;
          }
        break;
      case HI: case HS:
        switch (ebytes)
          {
          case 1: return "U8";  break;
          case 2: return "U16"; break;
          case 4: return "U32"; break;
          case 8: return "U64"; break;
          default: throw 0;
          }
        break;
      }
    return "??";
  }
  
  std::string
  GetOp2() const
  {
    if (not is_reg)
      return "0";
    std::ostringstream buf;
    buf << "cpu.GetV" << GetElemType() << "(rm,e)";
    return "0";
  }
  
  std::string
  GetCode() const
  {
    std::ostringstream buf;
    buf << "\n";
    buf << "  typedef typename ARCH::" << GetElemType() << " " << GetElemType() << ";\n\n";
    buf << "  for (unsigned e=0; e < " << elements << "; e += 1) {\n";
    
    buf << "    " << GetElemType() << " op1( cpu.GetV" << GetElemType() << "(rn,e) ), op2( " << GetOp2() << " ), res;\n";
    buf << "    res =  " << GetElemType() << "(not (op1 ";
    switch (compare_op)
      {
      case EQ:          buf << "=="; break;
      case LT:          buf << "<"; break;
      case LE:          buf << "<="; break;
      case GT: case HI: buf << ">"; break;
      case GE: case HS: buf << ">="; break;
      default: break;
      }
    buf << " op2)) - " << GetElemType() << "(1);\n";
    buf << "    cpu.SetV" << GetElemType() << "(rd,e,res);\n";
    buf << "  }\n";
    buf << "  cpu.ClearHighV(rd," << (elements*ebytes) << ");\n";
    
    return buf.str();
  }
  
  enum { EQ = 0, GE = 1, GT = 2, HI = 3, HS = 4, LE = 5, LT = 6 } compare_op;
  bool is_reg;
  int elements, ebytes, order;
};

struct Order
{
  static std::map<std::string,int> original;
  
  bool operator () ( Ptr<Operation> const& lhs, Ptr<Operation> const& rhs )
  {
    std::string lhn(lhs->symbol.str()), rhn(rhs->symbol.str());
    CMV lhe, rhe;
    if (not lhe.Decode(lhn) or not rhe.Decode(rhn))
      return original[lhn] < original[rhn];
    return lhe < rhe;
  }
};

std::map<std::string,int> Order::original;

void
Generator::transform( Isa& source )
{
  for (auto&& op : source.m_operations)
    {
      std::string opname = op->symbol.str();
      { uintptr_t idx = Order::original.size(); Order::original[opname] = idx; }
      
      CMV cmv;
      if (not cmv.Decode(opname))
        continue;
      
      std::cerr << opname << DumpBitFields( op->bitfields ) << " (" << cmv.compare_op << cmv.is_reg << cmv.elements << cmv.ebytes << ")\n";
    }
  
  std::sort( source.m_operations.begin(), source.m_operations.end(), Order() );
}


// void
// Generator::transform( Isa& source )
// {
//   for (auto&& op : source.m_operations)
//     {
//       std::string opname = op->symbol.str();
      
//       CMV cmv;
//       if (not cmv.Decode(opname))
//         continue;
      
//       Action* exec = 0;
//       for (auto&& act : op->actions) {
//         if (act->m_actionproto->m_symbol == "execute") {
//           std::cerr << "WARNING: execute method already existing: overwriting.\n";
//           exec = &*act;
//         }
//       }
      
//       if (not exec) {
//         ActionProto const* actionproto = source.actionproto( "execute" );
//         exec = new Action( actionproto, new SourceCode("",FileLoc()), Vector<Comment>(), FileLoc() );
//         op->add( exec );
//       }
      
//       exec->m_source_code->content = cmv.GetCode();
      
//       std::cerr << opname << DumpBitFields( op->bitfields ) << " (" << cmv.compare_op << cmv.is_reg << cmv.elements << cmv.ebytes << ")\n";
//     }
// }

