#include <disasm.hh>
#include <mipsel/disasm.hh>
#include <streamtools.hh>
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <map>
#include <set>
#include <list>
#include <cctype>
#include <inttypes.h>

template <uintptr_t N>
bool pop( char const*& ptr, char const (&ref)[N] )
{
  uintptr_t const end = N-1;
  for (uintptr_t idx = 0; idx < end; ++idx)
    if (ptr[idx] != ref[idx])
      return false;
  ptr = &ptr[end];
  return true;
}

struct SinkStream : public std::ostringstream
{
  std::string& sink;
  SinkStream( std::string& _sink ) : std::ostringstream(), sink(_sink) {}
  SinkStream( SinkStream const& ss ) : std::ostringstream(), sink( ss.sink ) {}
  ~SinkStream() { sink = str(); }
};

SinkStream sinkstream( std::string& sink ) { return sink; }

void
Recorder::ins( char const* mnemo, char const* args, uint32_t bits, uint32_t mask )
{
  families[Format(args,mask)].instances.push_back( Format(mnemo,bits) );
}
  
void
Recorder::fmt( std::string args, uint32_t mask, std::string postfix, std::string encoding, std::string vars, std::string disasm )
{
  Group& group = families[Format(args,mask)];
  if (group.encoding.size())
    throw 0;
  group.postfix = postfix;
  group.encoding = encoding;
  group.vars = vars;
  group.disasm = disasm;
}

std::string
Recorder::Group::getopname( Format const& insn ) const
{
  std::ostringstream result;
  if (postfix.size())
    result << insn.text << '_' << postfix;
  else
    result << insn.text;
  return result.str();
}

std::string
Recorder::Group::getdisasm( Format const& insn ) const
{
  std::string res = disasm;
  res.replace( 1,1, insn.text );
  return res;
}

void
Recorder::Group::sanitychecks() const
{
  // disassembly has a pretty constrained encoding
  if ((disasm[0] != '"') or (disasm[1] != '!'))
    throw 0;
}


 
std::string
Recorder::Group::getencoding( Format const& insn ) const
{
  std::ostringstream result;
  int bit = 32;
  
  for (uintptr_t pos = 0;;) {
    uintptr_t nxt = encoding.find( '[', pos );
    if (nxt == std::string::npos) {
      result << encoding.substr(pos);
      break;
    }
    char const* ptr = encoding.c_str() + nxt;
    if (*ptr++ != '[') throw 0;
    int bitcount = strtol( ptr, (char**)&ptr, 10 );
    if (*ptr++ != ']') throw 0;
    
    uintptr_t end = ptr - encoding.c_str();
    bit -= bitcount;
    
    if ((nxt > 0) and (encoding[nxt-1] == '!')) {
      result << encoding.substr( pos, nxt - pos - 1 );
      result << "0b";
      for (int bitidx=bitcount; --bitidx >= 0;)
        result << "01"[(insn.code >> (bit+bitidx)) & 1];
      result << '[' << bitcount << ']';
    } else {
      result << encoding.substr( pos, end - pos );
    }
    
    pos = end;
  }
  
  if (bit != 0) {
    std::cerr << "Error in: " << OStr( encoding ) << std::endl;
    throw 0;
  }
  
  return result.str();
}
 


struct VectorReg
{
  unsigned number;
  char letter;
  char const* parse_end;

  VectorReg( char const* src )
    : number(0), letter('\0'), parse_end(0)
  {
    if (not pop( src, "%V." ))
      return;
    number = strtoul( src, (char**)&src, 10 );
    char l = *src++;
    switch (l) {
    case 'b': case 'h': case 's': case 'd': case 'q':
      letter = l; break;
    default: return;
    }
    parse_end = src;
  }
  
  unsigned scale() const
  {
    switch (letter) {
    case 'b': return 0;
    case 'h': return 1;
    case 's': return 2;
    case 'd': return 3;
    case 'q': return 4;
    }
    return unsigned(-1);
  }
  
  struct Disasm : public Ostreamable
  {
    Disasm( VectorReg const& _p, char const* _r ) : p(_p), r(_r) {} VectorReg const& p; char const* r;
    void operator () ( std::ostream& sink ) const { sink << "DisasmTV(" << r << ',' << p.number << ',' << p.scale() << ")"; }
  };
  
  Disasm disasm( char const* regidx ) const { return Disasm(*this,regidx); }
  
  struct Code : public Ostreamable
  {
    Code( VectorReg const& _p ) : p(_p) {} VectorReg const& p;
    void operator () ( std::ostream& sink ) const {
      sink << 'v';
      if (p.number)
        sink << p.number;
      sink << p.letter;
    }
  };
  
  Code code() const { return Code(*this); }
  
  char const* end() const { return parse_end; }
  operator bool() const { return parse_end; }
};

struct VectorRegSet
{
  VectorReg element;
  unsigned count;
  char const* parse_end;
  
  VectorRegSet( char const* src )
    : element(""), count(0), parse_end(0)
  {
    if (*src++ != '{') return;
    
    std::map<VectorReg,unsigned> tmp;
    for (;;) {
      VectorReg vr = src;
      src = vr.end();
      if (not vr) return;
      tmp[vr] += 1;
      if (*src == '}') break;
      if (not pop( src, ", " )) return;
    }
    if (tmp.size() != 1) return;
    auto pair = tmp.begin();
    element = pair->first;
    count =   pair->second;
    parse_end = src+1;
  }
  
  struct Disasm : public Ostreamable
  {
    Disasm( VectorRegSet const& _p, char const* _r ) : p(_p), r(_r) {} VectorRegSet const& p; char const* r;
    void operator () ( std::ostream& sink ) const {
      sink << "DisasmBunch(" << r << ',' << p.count << ',' << p.element.number << ',' << p.element.letter << ")";
    }
  };
  Disasm disasm( char const* regidx ) const { return Disasm(*this,regidx); }
  
  struct Code : public Ostreamable
  {
    Code( VectorRegSet const& _p ) : p(_p) {} VectorRegSet const& p;
    void operator () ( std::ostream& sink ) const {
      for (unsigned idx = 0; idx < p.count; ++idx) sink << p.element.code();
    }
  };
  
  Code code() const { return Code(*this); }
  
  char const* end() const { return parse_end; }
  operator bool() const { return parse_end; }
};

struct ScalarReg
{
  char letter;
  char const* parse_end;
  
  ScalarReg( char const* src )
    : letter('\0'), parse_end(0)
  {
    if (*src++ != '%') return;
    
    char l = *src++;
    switch (l) {
    case 'B': case 'H': case 'S': case 'D': case 'Q':
    case 'W': case 'X':
      letter = l;
      break;
    default: return;
    }
    parse_end = src;
  }
  
  unsigned scale()
  {
    switch (letter) {
    case 'B':
      return 0;
    case 'H':
      return 1;
    case 'S': case 'W':
      return 2;
    case 'X': case 'D':
      return 3;
    case 'Q':
      return 4;
    }
    return unsigned(-1);
  }
  
  struct Disasm : public Ostreamable
  {
    Disasm( ScalarReg const& _p, char const* _r ) : p(_p), r(_r) {} ScalarReg const& p; char const* r;
    void operator () ( std::ostream& sink ) const
    {
      switch (p.letter) {
      case 'B': case 'H': case 'S': case 'D': case 'Q':
        sink << "Disasm" << p.letter;
        break;
      case 'W': case 'X':
        sink << "DisasmGZ" << p.letter << "R";
        break;
      }
      sink << "(" << r << ")";
    }
  };
  
  Disasm disasm( char const* regidx ) const { return Disasm(*this,regidx); }
  
  struct Code : public Ostreamable
  {
    Code( ScalarReg const& _p ) : p(_p) {} ScalarReg const& p;
    void operator () ( std::ostream& sink ) const { sink << char( tolower( p.letter ) ); }
  };
  
  Code code() const { return Code(*this); }
  
  char const* end() const { return parse_end; }
  operator bool() const { return parse_end; }
};

struct FullReg
{
  ScalarReg sr;
  VectorReg vr;
  
  FullReg( char const* src )
    : sr(""), vr("")
  {
    if ((sr = src)) return;
    if ((vr = src)) return;
  }
  
  struct Disasm : public Ostreamable
  {
    Disasm( FullReg const& _p, char const* _r ) : p(_p), r(_r) {} FullReg const& p; char const* r;
    void operator () ( std::ostream& sink ) const { if (p.sr) sink << p.sr.disasm(r); else sink << p.vr.disasm(r); }
  };
  
  Disasm disasm( char const* regidx ) const { return Disasm(*this,regidx); }
  
  struct Code : public Ostreamable
  {
    Code( FullReg const& _p ) : p(_p) {} FullReg const& p;
    void operator () ( std::ostream& sink ) const { if (p.sr) sink << p.sr.code(); else sink << p.vr.code(); }
  };
  
  Code code() const { return Code(*this); }
  
  char const* end() const { return sr ? sr.end() : vr.end(); }
  operator bool() const { return sr or vr; }
};


struct VIEnc
{
  unsigned bitsize, pos;
  VIEnc( char const* src )
    : bitsize(0), pos(0)
  {
    char const* ptr = src;
    if (not pop( ptr, "![" )) return;
    unsigned opcbits = strtoul( ptr, (char**)&ptr, 10 );
    if (not pop( ptr, "] : " )) return;
    unsigned immpos = ptr - src;
    if (not pop( ptr, "?[" )) return;
    unsigned immbits = strtoul( ptr, (char**)&ptr, 10 );
    if (not pop( ptr, "] : ![6] : rn[5] : rd[5]" )) return;
    if (*ptr) return;
    if ((opcbits + immbits) != 16) return;
    bitsize = immbits;
    pos = immpos;
  }
  
  operator bool() const { return bitsize; }
};

bool
Recorder::Format::operator < ( Format const& rhs ) const
{
  unsigned
    lbits = __builtin_popcount( code ),
    rbits = __builtin_popcount( rhs.code );
      
  if (lbits < rbits) return true;
  if (lbits > rbits) return false;
  if (code < rhs.code) return true;
  if (code > rhs.code) return false;
  return text < rhs.text;
}

bool
chksafe( std::string const& s )
{
  for (char c : s) {
    if (uint8_t(c - '0') < 10) continue;
    if (uint8_t(c - 'a') < 26) continue;
    if (uint8_t(c - 'A') < 26) continue;
    if (c == '_') continue;
    return false;
  }
  return true;
}

static
bool
is_simd_or_fp( std::string const& format )
{
  char last = ' ';
  for (char c: format) {
    bool isreg = last == '%';
    last = c;
    if (not isreg) continue;
    switch (c) {
    case 'V': case 'B': case 'H': case 'S': case 'D': case 'Q':
      return true;
    }
  }
  return false;
}

void
Recorder::process()
{
  unsigned missdis_fmt = 0, missdis_isn = 0, total_isn = 0;

  for (auto& family : families) {
    Format const& fmt( family.first );
    Group& grp( family.second );
    
    total_isn += grp.instances.size();
    
    if (grp.disasm.size() == 0)
      { missdis_fmt += 1; missdis_isn += grp.instances.size(); }
    
    char const* ptr = grp.encoding.c_str();
    
    if (not pop( ptr, "0b"))
      continue;
    
    std::string insns;
    { char const * sep = ""; for (auto && insn : grp.instances) { insns = insns + sep + insn.text; sep = ", "; } }
    std::cerr << "  isa.fmt"
              << "( " << OStr( fmt.text )
              << ", " << Ox32( fmt.code )
              << ", " << OStr( grp.postfix )
              << ", " << OStr( grp.encoding )
              << ", " << OStr( grp.vars )
              << ", " << OStr( grp.disasm )
              << " ); " << insns << "\n";
  }
    
  {
    uintptr_t total_fmt = families.size(), disasm_fmt = total_fmt - missdis_fmt, disasm_isn = total_isn - missdis_isn;
    std::cerr << "Progress: ";
    std::cerr << disasm_fmt << '/' << total_fmt << " (" << (disasm_fmt * 100 / total_fmt) << "%), ";
    std::cerr << disasm_isn << '/' << total_isn << " (" << (disasm_isn * 100 / total_isn) << "%)\n";
  }
  
  std::map<std::string,unsigned> opname_stats;

  std::set<std::string> base_isa;
  std::set<std::string> simd_isa;
  
  {
    std::ofstream isasrc( "isa.cc" );
    
    isasrc << "#include <disasm.hh>\n\nvoid\nrecord( Recorder& isa )\n{\n\n";
  
    uint32_t format = 0;
    for (auto const& family : families)
      {
        Format const& fmt( family.first );
        Group const& grp( family.second );
    
        grp.sanitychecks();
      
        unsigned bits = __builtin_popcount( family.first.code );
      
        isasrc << "  // " << std::dec << bits << ", " << Ox32( fmt.code ) << ", " << OStr( fmt.text ) << '\n';
        isasrc << "  isa.fmt"
             << "( " << OStr( fmt.text )
             << ", " << Ox32( fmt.code )
             << ", " << OStr( grp.postfix )
             << ", " << OStr( grp.encoding )
             << ", " << OStr( grp.vars )
             << ", " << OStr( grp.disasm )
             << " );\n";
      
        for (auto const& insn : grp.instances) {
          std::string opname = grp.getopname(insn);
      
          if ((opname_stats[opname] += 1) > 1) {
            std::ostringstream errstr;
            errstr << "Warning: opname " << OStr( opname ) << " will be used more than once.\n";
            isasrc << "  // " << errstr.str();
            std::cerr << errstr.str();
          }
      
          isasrc << "  isa.ins"
                 << "( " << OStr( insn.text )
                 << ", " << OStr( fmt.text )
                 << ", " << Ox32( insn.code )
                 << ", " << Ox32( fmt.code )
                 << " ); // " << opname << "\n";
          
          std::stringstream sink;
          std::string encoding = grp.getencoding(insn);
          sink << "op " << opname << "( " << encoding << " );\n";
          sink << opname << ".var ";
          if (grp.vars.size())
            sink << grp.vars << ", ";
          sink << "format : {char const*} = {\"" << ++format << "\"};\n\n";
      
          std::string disasm = grp.getdisasm(insn);
          sink << opname << ".disasm = {\n  sink << " << disasm << ";\n};\n\n";
      
          if (is_simd_or_fp( fmt.text ))
            simd_isa.insert( sink.str() );
          else
            base_isa.insert( sink.str() );
        }
      }

    isasrc << "\n}\n\n";
  }
  
  {
    std::ofstream sink( "base.isa" );
    for (auto&& code : base_isa)
      sink << code;
  }
  
  {
    std::ofstream sink( "simd_fp.isa" );
    for (auto&& code : simd_isa)
      sink << code;
  }
}

void record( Recorder& );

int
main()
{
  Recorder isa;
  record( isa );
  isa.process();
  return 0;
}



