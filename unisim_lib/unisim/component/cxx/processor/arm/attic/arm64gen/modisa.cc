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
  
  bool operator < ( CMV const& rhs ) const { return order < rhs.order; }
  
  enum { EQ, GT, LT, GE, LE, HI, HS } compare_op;
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
    if (not lhe.Decode(lhn) or not rhe.Decode(rhn) or not lhe.comparable( rhe ))
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

// {
//   for (auto&& op : source.m_operations)
//     {
//       std::string opname = op->symbol.str();
      
//       uintptr_t oplen = opname.size();
//       if (oplen < 6)
//         continue;
//       bool isld;
//       if      (opname.find("ld") == 0)
//         {
//           isld = true;
//         }
//       else if (opname.find("st") == 0)
//         {
//           isld = false;
//         }
//       else
//         continue;
      
//       int selem = opname[2] - '0';
//       if ((selem < 1) or (selem > 4))
//         continue;
      
//       if (opname[3] != '_')
//         continue;
      
//       int regcount = 0;
//       switch (opname[4]) {
//       case 's': regcount = 1; break;
//       case 'd': regcount = 2; break;
//       case 't': regcount = 3; break;
//       case 'q': regcount = 4; break;
//       default: break;
//       }
      
//       if (regcount == 0)
//         continue;
      
//       int rpt = 1;
//       if (regcount != selem) {
//         assert( selem == 1 );
//         rpt = regcount;
//       }
      
//       int elements, ebytes;
//       enum { NoWBack, WBackImm, WBackReg } wback;
//       bool half_vector = false;
      
//       {
//         char etype;
//         std::string addrmode;
//         char* after;
//         char const* ecp = &opname[5];
//         elements = strtoul(ecp,&after,10);
//         if (after == ecp)
//           continue;
//         switch (elements) {
//         case 1: case 2: case 4: case 8:
//           assert( oplen >= 7 );
//           etype = opname[6];
//           addrmode = opname.substr(7);
//           break;
//         case 16:
//           assert( oplen >= 8 );
//           etype = opname[7];
//           addrmode = opname.substr(8);
//           break;
//         default:
//           assert( false );
//         }
//         switch (etype) {
//         case 'b': ebytes = 1; break;
//         case 'h': ebytes = 2; break;
//         case 's': ebytes = 4; break;
//         case 'd': ebytes = 8; break;
//         default:    assert( false );
//         }
        
//         if (addrmode.size() == 0) {
//           wback = NoWBack;
//         } else if (addrmode == "_reg") {
//           wback = WBackReg;
//         } else if (addrmode == "_imm") {
//           wback = WBackImm;
//         } else {
//           assert( false );
//         }
//         switch (ebytes*elements) {
//         case 8: half_vector = true; break;
//         case 16: half_vector = false; break;
//         default: assert( false );
//         }
//       }
      
//       std::cerr << opname << DumpBitFields( op->bitfields ) << " (" << isld << selem << regcount << rpt << elements << ebytes << wback << half_vector << ")\n";
      
//       {
//         /* Code Generation */
//         std::stringstream buf;
//         buf << "\n";
//         buf << "  typedef typename ARCH::U64 U64;\n\n";
//         buf << "  U64 addr( cpu.GetGSR(rn) );\n\n";
//         buf << "  for (unsigned r = 0; r < " << (selem*rpt) << "; ++r) { cpu.UseVU" << (ebytes*8) << "((rt+r)%32," << elements << "); }\n\n";
//         if (rpt > 1) {
//           buf << "  for (unsigned r = 0; r < " << rpt << "; ++r) {\n";
//           buf << "    for (unsigned e = 0; e < " << elements << "; ++e) {\n";
//           std::string addrstr;
//           {
//             std::stringstream addrbuf;
//             addrbuf << "addr + U64((r*" << elements << "+e)*" << ebytes << ")";
//             addrstr = addrbuf.str();
//           }
//           if (isld)
//             buf << "      cpu.SetVU" << (ebytes*8) << "((rt+r)%32, e, cpu.MemRead" << (ebytes*8) << "( " << addrstr << " ));\n";
//           else
//             buf << "      cpu.MemWrite32( " << addrstr << ", cpu.GetVU" << (ebytes*8) << "((rt+r)%32, e) );\n";
//           buf << "    }\n";
//           buf << "  }\n";
//         } else /* (selem > 1) */ {
//           buf << "  for (unsigned e = 0; e < " << elements << "; ++e) {\n";
//           buf << "    for (unsigned s = 0; s < " << selem << "; ++s) {\n";
//           std::string addrstr;
//           {
//             std::stringstream addrbuf;
//             addrbuf << "addr + U64((e*" << selem << "+s)*" << ebytes << ")";
//             addrstr = addrbuf.str();
//           }
//           if (isld)
//             buf << "      cpu.SetVU" << (ebytes*8) << "((rt+s)%32, e, cpu.MemRead" << (ebytes*8) << "( " << addrstr << " ));\n";
//           else
//             buf << "      cpu.MemWrite32( " << addrstr << ", cpu.GetVU" << (ebytes*8) << "((rt+s)%32, e) );\n";
//           buf << "    }\n";
//           buf << "  }\n";
//         }
//         switch (wback) {
//         case WBackImm: buf << "  cpu.SetGSR(rn, addr + U64(" << (elements*selem*ebytes) << ") );\n"; break;
//         case WBackReg: buf << "  cpu.SetGSR(rn, addr + cpu.GetGZR(rm) );\n"; break;
//         default: break;
//         }
        
//         Action* exec = 0;
//         for (auto&& act : op->actions) {
//           if (act->m_actionproto->m_symbol == "execute") {
//             std::cerr << "WARNING: execute method already existing: overwriting.\n";
//             exec = &*act;
//           }
//         }
      
//         if (not exec) {
//           ActionProto const* actionproto = source.actionproto( "execute" );
//           exec = new Action( actionproto, new SourceCode("",FileLoc()), Vector<Comment>(), FileLoc() );
//           op->add( exec );
//         }
      
//         exec->m_source_code->content = buf.str();
//       }
      
      
//       // Action* exec = 0;
//       // for (auto&& act : op->actions) {
//       //   if (act->m_actionproto->m_symbol == "execute")
//       //     exec = &*act;
//       // }
      
//       // if (not exec) continue;
      
//       // std::string exec_code = exec->m_source_code->content.str();

//       // bool x = query_replace( exec_code, "+ U64(imm)", "+ U64(S64(imm))" );
//       // if (not x)
//       //   continue;
      
//       // // std::cerr << "(" << isld << (rsz==64) << xsof << xoff << sext << __builtin_ctz( size ) << ")";
//       // for (unsigned pad = opname.size(); pad < 12; ++pad)
//       //   std::cerr << ' ';
//       // std::cerr << opname << DumpBitFields( op->bitfields ) << "\n";
      
//       // exec->m_source_code->content = exec_code;
      
      
//       // std::cerr << isld << std::endl;
      
//       // bool imm_is_signed = false;
//       // for (auto const& bf : op->bitfields)
//       // {
//       //   if (OperandBitField const* opbf = dynamic_cast<OperandBitField const*>( &*bf )) {
//       //     if ((opbf->symbol == "imm") and (opbf->sext)) {
//       //       imm_is_signed = true;
//       //       break;
//       //     }
//       //   }
//       // }
//       // if (not imm_is_signed)
//       //   continue;
      
//       continue;
      
//       // {
//       //   uintptr_t sep = opname.find("_");
//       //   if ((sep == std::string::npos) or ((opname.size()-sep) != 5))
//       //     continue;
//       //   if (opname[sep+2] != 'x')
//       //     continue;
//       //   switch (opname[sep+1]) { case 'x': rsz =   64; break; case 'w': rsz =    32; break; default: throw 0; }
//       //   switch (opname[sep+3]) { case 'x': xoff = true; break; case 'w': xoff = false; break; default: throw 0; }
//       //   switch (opname[sep+4]) { case 's': xsof = true; break; case 'u': xsof = false; break; default: throw 0; }
        
//       //   uintptr_t pos = 3;
//       //   if (opname[pos] == 's') { sext = true; pos += 1; }
//       //   switch (opname[pos++]) {
//       //   case 'b': size =  8; break;
//       //   case 'h': size = 16; break;
//       //   case 'w': size = 32; break;
//       //   default: size = rsz; pos -= 1; break;
//       //   }
//       //   if (opname[pos] != '_') {
//       //     std::cerr << "error while parsing: " << opname << "\n";
//       //     throw 0;
//       //   }
//       // }
      
      
//       // char sch = xsof ? 'S' : 'U';
//       // unsigned tsz = xoff ? 64 : 32;
      
//       // std::set<std::string> types;
//       // struct TpStr
//       // {
//       //   TpStr( char _sgn, int _sz ) { std::ostringstream buf; buf << _sgn << _sz; data[buf.str().copy( &data[0], 7 )] = '\0'; } char data[8];
//       //   char const* get() const { return &data[0]; }
//       // };
      
//       // std::string address;
//       // { std::ostringstream buf; buf << "U64 offset( " << sch << "64(" << sch << tsz << "(cpu.GetGZR(rm))) << shift ), addr( cpu.GetGSR(rn) + offset );"; address = buf.str(); }
//       // types.insert( "U64" );
//       // types.insert( TpStr(sch,64).get() );
//       // types.insert( TpStr(sch,tsz).get() );
      
//       // if (isld) {
//       //   types.insert( TpStr('U',rsz).get() );
//       //   if (sext) {
//       //     types.insert( TpStr('S',rsz).get() );
//       //     types.insert( TpStr('S',size).get() );
//       //   }
//       // } else {
//       //   types.insert( TpStr('U',size).get() );
//       // }

//       // std::ostringstream buf;
//       // buf << "\n";
//       // for (auto&& tp : types)
//       //   {
//       //     buf << "  typedef typename ARCH::" << tp << " " << tp << ";\n";
//       //   }
//       // buf << "\n  " << address << "\n";
      
//       // if (isld)
//       //   {
//       //     buf << "  U" << rsz << " data( ";
        
//       //     std::string memread; { std::ostringstream mrbuf; mrbuf << "cpu.MemRead" << size << "(addr)"; memread = mrbuf.str(); }
//       //     if (sext)  buf << "S" << rsz << "(S" << size << "(" << memread << "))";
//       //     else       buf << memread;
        
//       //     buf << " );\n"
//       //         << "  cpu.SetGZR(rt, data);\n";
//       //   }
//       // else
//       //   {
//       //     buf << "  U" << size <<  " data( cpu.GetGZR(rt) );\n"
//       //         << "  cpu.MemWrite" << size << "(addr, data);\n";
//       //   }
      
//       // Vector<Comment> com;
          
//       // ActionProto const* actionproto = source.actionproto( "execute" );
          
//       // op->add( new Action( actionproto, new SourceCode(buf.str(),FileLoc()), com, FileLoc() ) );
      
      
//       // struct OpEnv { char const* ext; unsigned size; bool setstatus; };
//       // std::vector<OpEnv> openvs{ {"_wsxtx", 32, false}, {"_xsxtx", 64, false}, {"s_wsxtx", 32, true}, {"s_xsxtx", 64, true} };
      
//       // struct OpCore { char const* name; char const* op; };
//       // std::vector<OpCore> opcores{ {"add","+ "}, {"and", "& "}, {"bic", "& ~"}, {"sub", "- "},
//       //                              {"eon", "^ ~"}, {"eor", "^ "}, {"orn", "| ~"}, {"orr", "| "} };
      
//       // for (auto&& openv : openvs) {
//       //   for (auto&& opcore : opcores) {
//       //     std::string on( opcore.name );
//       //     on += openv.ext;
//       //     if (on != opname)
//       //       continue;
//       //     std::cerr << opname << DumpBitFields( op->bitfields ) << '\n';
          
//       //     std::ostringstream oss;
//       //     if (openv.size != srcsize)
//       //       oss << "\n  typedef typename ARCH::S" << srcsize << " S" << srcsize << ";";
//       //     oss
//       //       << "\n  typedef typename ARCH::S" << openv.size << " S" << openv.size << ';'
//       //       << "\n  typedef typename ARCH::U" << openv.size << " U" << openv.size << ';'
//       //       << "\n  U" << openv.size << " op1( cpu.GetGZR(rn) ), op2( S" << openv.size << "(S" << srcsize << "(cpu.GetGZR(rm))) << imm ), res = op1 " << opcore.op << "op2;"
//       //       << "\n  cpu.SetGZR(rd, res);";
//       //     if (openv.setstatus)
//       //       oss << "\n  // XXX: set status flags";
//       //     oss << "\n";
          
//       //     Vector<Comment> com;
          
//       //     ActionProto const* actionproto = source.actionproto( "execute" );
          
//       //     // std::cerr << oss.str() << std::endl;
          
//       //     op->add( new Action( actionproto, new SourceCode(oss.str(),FileLoc()), com, FileLoc() ) );
//       //     break;
//       //   }
//       // }
      
//       // bool x = query_replace( disasm_code, "DisasmGZXR(rd)", "DisasmGSXR(rd)" );
//       // bool y = query_replace( disasm_code, "DisasmGZWR(rd)", "DisasmGSWR(rd)" );
//       // query_replace( disasm_code, "  <<", " <<" );
//       // assert( x^y );
      
//       // bool chk;
      
//       // chk = query_replace( disasm_code, "\n  sink << \"ands\\t\" << DisasmG", "" );
//       // assert( chk );
//       // char zs = disasm_code[0];
//       // assert( (zs == 'Z') or (zs == 'S') );
//       // char xw = disasm_code[1];
//       // assert( toupper( opname[5] ) == xw );
//       // disasm_code = disasm_code.substr( 2 );
//       // chk = query_replace( disasm_code, "R(rd) << \", \"", "" );
//       // assert( chk );
      
//       // disasm_code = std::string("\n  if (rd == 31)\n    sink << \"tst\\t\"") + disasm_code + "  else\n    sink << \"ands\\t\" << DisasmGZ" + xw + "R(rd) << \", \"" + disasm_code;
        
//       // std::cerr << disasm_code << std::endl;
      
//       // disasm.m_source_code->content = disasm_code.c_str();
      
//       // auto&& itr = op->bitfields.begin();
      
//       // if (opname.substr(opname.size()-5) != "xxror")
//       //   continue;
      
//       // try {
//       //   if (op->bitfields.size() != 7) continue;
//       //   if (dynamic_cast<OpcodeBitField const&>( **itr++ ).size != 11) continue;
//       //   {
//       //     OperandBitField& obf = dynamic_cast<OperandBitField&>( **itr++ );
//       //     if (obf.size != 5) continue;
//       //     if (obf.symbol != "rm") continue;
//       //   }
        
//       //   if (dynamic_cast<OpcodeBitField const&>( **itr++ ).size != 3) continue;
//       //   {
//       //     OperandBitField& obf = dynamic_cast<OperandBitField&>( **itr++ );
//       //     if (obf.size != 1) continue;
//       //     if (obf.symbol != "s") continue;
//       //   }

//       //   if (dynamic_cast<OpcodeBitField const&>( **itr++ ).size != 2) continue;
       
//       //   unsigned amount = strtoul( disasm_code.substr(disasm_code.size()-5,1).c_str(), 0, 10 );
//       //   disasm_code.replace( disasm_code.size()-12,8,"DisasmRor(shift) << \"", 21 );
        
//       //   Variable* v = new Variable( "shift", new SourceCode( "uint8_t", FileLoc() ), new SourceCode( Str::fmt("s*%u",amount), FileLoc() ) );
        
//       //   op->variables.push_back( v );
        
//       //   // uintptr_t pos = disasm_code.find("DisasmS");
//       //   // if (pos == std::string::npos)
//       //   //   continue;
      
//       //   // disasm_code.replace( pos, 7, "DisasmD", 7 );
        
//       //   disasm.m_source_code->content = disasm_code.c_str();
        
//       // }
//       // catch (std::bad_cast const&) { continue; }
      
//       // if (replace_ror_const( disasm_code ))
//       // uintptr_t xtlpos = disasm_code.find( "xtl" );
//       // if (xtlpos != std::string::npos)
//       //   continue;
      
//       // uintptr_t end = disasm_code.find( " << \", \" << DisasmI(imm);\n" );
//       // if (end == std::string::npos)
//       //   continue;
//       // disasm_code.erase(end);
//       // std::string toto("\n  sink << \"sshll");
//       // if (disasm_code.find(toto) != 0)
//       //   throw 0;
//       // disasm_code = disasm_code.substr(toto.size());
//       // disasm_code = std::string("\n  if (imm) { sink << \"sshll") + disasm_code + " << \", \" << DisasmI(imm); } else { sink << \"sxtl" + disasm_code + "; }\n";
      
//       // auto&& itr = op->bitfields.begin();
//     }
// }

