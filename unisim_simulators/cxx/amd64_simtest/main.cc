/*
 *  Copyright (c) 2019,
 *  Commissariat a l'Energie Atomique (CEA)
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without modification,
 *  are permitted provided that the following conditions are met:
 *
 *   - Redistributions of source code must retain the above copyright notice, this
 *     list of conditions and the following disclaimer.
 *
 *   - Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 *   - Neither the name of CEA nor the names of its contributors may be used to
 *     endorse or promote products derived from this software without specific prior
 *     written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 *  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *  DISCLAIMED.
 *  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 *  OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 *  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: Yves Lhuillier (yves.lhuillier@cea.fr)
 */

#include <arch.hh>
#include <unisim/component/cxx/processor/intel/isa/intel.hh>
// #include <unisim/component/cxx/processor/intel/modrm.hh>
// #include <unisim/component/cxx/processor/intel/disasm.hh>
// #include <unisim/component/cxx/processor/intel/vectorbank.hh>
// #include <unisim/component/cxx/processor/intel/types.hh>
// #include <unisim/component/cxx/processor/intel/execute.hh>
#include <unisim/util/random/random.hh>
#include <fstream>
#include <iostream>
#include <sstream>
#include <set>
#include <memory>
#include <inttypes.h>

struct Sink
{
  Sink( std::string const& gendir )
    : macros( gendir + ".inc" ), sources( gendir + ".S" )
  {
  }
  
  ~Sink() {}
  
  std::ofstream macros;
  std::ofstream sources;
  
  char const* test_prefix() const { return "iut_"; }
};

struct TestConfig
{
  ut::Interface const& iif;
  std::string ident, disasm;
  
  TestConfig( ut::Interface const& _iif, std::string const& _ident, std::string const& _disasm )
    : iif( _iif ), ident( _ident ), disasm( _disasm )
  {}
  
  // bool wide() const { return iif.length == 32; }
  
  std::string prologue() const {}
  // {
  //   if (iif.usemem()) {
  //     std::ostringstream sink;
  //     try {
  //       auto const& p = iif.GetPrologue();
  //       mpc57::GPRPrint br( p.base );
  //       if      ((p.offset & -128) == 0)
  //         {
  //           // a suitable 7 bit unsigned immediate (cost 2 bytes)
  //           sink << "\tse_li\t" << br << ", " << p.offset << "\n";
  //         }
  //       else if (((p.offset + 0x80000) >> 20) == 0)
  //         {
  //           // a suitable 20 bit signed immediate (cost 4 bytes)
  //           sink << "\te_li\t" << br << ", " << int32_t(p.offset) << "\n";
  //         }
  //       else
  //         {
  //           // a full n raw 32 bit immediate (8 bytes)
  //           mpc57::HexPrint xoff( p.offset );
  //           sink << "\te_lis\t" << br << ", " << xoff << "@h\n";
  //           sink << "\te_or2i\t" << br << ", " << xoff << "@l\n";
  //         }
        
  //       mpc57::GPRPrint buffer( iif.aligned ? 3 : 0 );

  //       sink << "\t" << (p.sign ? "sub" : "add") << "\t" << br << ", " << br << ", " << buffer << "\n";
        
  //       for (auto reg : p.regs) {
  //         mpc57::GPRPrint rname( reg.first );
  //         mpc57::HexPrint rvalue( reg.second );
  //         if (reg.second & -128) { std::cerr << "IE immediate generation.\n"; throw 0; }
  //         sink << "\tse_li\t" << rname << ", " << rvalue << "\n";
  //       }
  //     } catch (ut::Interface::Prologue::Error const& x) {
  //       std::cerr << "Prologue error in: " << disasm << ".\n";
  //       throw x;
  //     }
  //     return sink.str();
  //   }
  //   return "";
  // }
  
  std::string epilogue() const { return ""; }
};

struct AMD64
{
  struct MemCode
  {
    uint8_t     bytes[15];
    
    bool get( std::istream& source )
    {
      uintptr_t bytecount = 0; bool nibble = false;
      for (;;)
        {
          char ch;
          if (not source.get( ch ).good()) return false;
          if (ch == ' ')     { if (nibble) return false; continue; }
          if (ch == '\t')    { if (nibble) return false; break; }

          uint8_t nval;
          if      ('0' <= ch and ch <= '9') nval = ch - '0';
          else if ('a' <= ch and ch <= 'f') nval = ch - 'a' + 10;
          else return false;
        
          if (nibble) { bytes[bytecount] = bytes[bytecount] << 4 | nval; nibble = false; }
          else        { bytes[bytecount] =                         nval; nibble =  true; }
        }

      return true;
    }
    friend std::ostream& operator << ( std::ostream& sink, MemCode const& mc )
    {
      for (unsigned idx = 0; idx < sizeof(bytes); ++idx)
        {
          uint8_t byte = mc.bytes[idx];
          for (unsigned nibble = 0; nibble < 2; ++nibble)
            sink << ("0123456789abcdef"[(byte >> nibble*4)]);
          sink << ' ';
        }
    }
  };
  
  typedef unisim::component::cxx::processor::intel::Operation<ut::Arch> Operation;

  Operation* decode( uint64_t addr, MemCode const& ct, std::string& disasm )
  {
    Operation* op = ut::Arch::fetch( addr, &ct.bytes[0] );
    std::ostringstream buf;
    op->disasm(buf);
    disasm = buf.str();
  }
  
  static char const* Name() { return "amd64"; }
  
  // void
  // write_test( Sink& sink, TestConfig const& cfg, CodeType code )
  // {
  //   std::string hexcode;
  //   {
  //     uint32_t codebits = code >> (cfg.wide() ? 0 : 16);
  //     std::ostringstream oss;
  //     oss << std::hex << codebits;
  //     hexcode = oss.str();
  //   }
  //   std::string opfunc_name = sink.test_prefix() + cfg.ident + '_' + hexcode;
    
  //   sink.macros << "ENTRY(" << opfunc_name << ",0x" << hexcode << ")\n";
    
  //   sink.sources << "\t.text\n\t.align\t2\n\t.global\t" << opfunc_name
  //                << "\n\t.type\t" << opfunc_name
  //                << ", @function\n" << opfunc_name
  //                << ":\n"
  //                << cfg.prologue()
  //                << "\t." << (cfg.wide() ? "long" : "short") << "\t0x" << hexcode << '\t' << "/* " << cfg.disasm << " */\n"
  //                << cfg.epilogue()
  //                << "\tse_blr\n"
  //                << "\t.size\t" << opfunc_name
  //                << ", .-" << opfunc_name << "\n\n";
  // }
};

template <typename ISA>
struct Checker
{
  typedef typename ISA::MemCode MemCode;
  typedef typename ISA::Operation Operation;
  struct InsnCode
  {
    InsnCode( MemCode const& mc, std::string const& ac ) : memcode(mc), asmcode(ac) {}
    MemCode memcode;
    std::string asmcode;
  };
  typedef std::multimap<ut::Interface, InsnCode> TestDB;
  
  unisim::util::random::Random random;
  ISA isa;

  TestDB testdb;
  std::ofstream logger;
  
  Checker()
    : random( 1, 2, 3, 4 )
    , logger((std::string( ISA::Name() ) + ".log").c_str())
  {}
  
  void discover( uintptr_t ttl )
  {
    uint64_t step = 0;
    // auto const& dectable = isa.GetDecodeTable();
    throw 0;
    //    for (auto&& opc : isa.GetDecodeTable())
      // {
      //   uint32_t mask = opc.opcode_mask, bits = opc.opcode & mask;
      //   auto testclass = testclasses.end();
      //   std::map<std::string,uintptr_t> fails;
        
      //   for (uintptr_t trial = 0; trial < ttl; ++trial)
      //     {
      //       step += 1;
      //       try
      //         {
      //           CodeType code;
      //           ISA::mkcode( code, (random.Generate() & ~mask) | bits );
                
      //           std::unique_ptr<Operation> codeop( opc.decode( code, 0 ) );
                
      //           {
      //             std::string name( codeop->GetName() );
      //             if (testclass == testclasses.end()) {
      //               testclasses[name];
      //               testclass = testclasses.find(name);
      //               std::cerr << "Tests[" << ISA::Name() << "::" << name << "]: ";
      //             } else if (testclass->first != name) {
      //               std::cerr << "Incoherent Operation names: " << testclass->first << " and " << name << std::endl;
      //               throw 0;
      //             }
      //           }
                
      //           if (codeop->donttest())
      //             {
      //               fails["not under test"] += 1;
      //               break;
      //             }
                
      //           {
      //             std::unique_ptr<Operation> realop( isa.NCDecode( 0, code ) );
      //             if (strcmp( realop->GetName(), codeop->GetName() ) != 0)
      //               {
      //                 fails["hidden"] += 1;
      //                 continue; /* Not the op we were looking for. */
      //               }
      //             code = ISA::cleancode( *codeop );
      //           }
                
      //           if (testclass->second.size() >= 256) {
      //             std::cerr << "Possible issue: too many tests for" << testclass->first << "...\n";
      //             throw 0;
      //           }
                
      //           // We need to perform an abstract execution of the
      //           // instruction to 1/ further check testability and 2/
      //           // compute operation interface.
      //           ut::Interface interface( *codeop );

      //           // At this point, code corresponds to valid operation
      //           // to be tested. Nevertheless, if the interface of
      //           // this operation matches an existing test, we don't
      //           // add the operation since the new test is unlikely to
      //           // reveal new bugs.
      //           if (testclass->second.find( interface ) != testclass->second.end()) continue;
      //           testclass->second.insert( std::make_pair( interface, code ) );
      //           trial = 0;
      //         }
      //       catch (ut::Untestable const& denial)
      //         {
      //           fails[denial.reason] += 1;
      //           if (denial.reason == "not implemented")
      //             break;
      //         }
      //     }
      //   if (testclass == testclasses.end())
      //     std::cerr << "Tests[" << ISA::Name() << "::?]: nothing found...\n";
      //   else if (testclass->second.size() == 0)
      //     {
      //       std::ostringstream msg;
            
      //       msg << "nothing found";
      //       for (auto&& reason : fails)
      //         msg << " <" << reason.first << ">";
      //       msg << "\n";
      //       std::cerr << msg.str();
      //       logger << testclass->first << " : " << msg.str();
      //     }
      //   else
      //     std::cerr << testclass->second.size() << " patterns found." << std::endl;
      // }
  }
  
  void
  write_repos( std::string const& reposname )
  {
    std::ofstream sink( reposname );

    for (typename TestDB::value_type const& test : testdb)
      {
        sink << test.second.memcode << "\t" << test.second.asmcode << '\n';
      }
  }

  struct FileLoc
  {
    std::string name;
    unsigned    line;
    
    FileLoc( std::string const& _name ) : name( _name ), line(0) {}
    void newline() { line += 1; }
    friend std::ostream& operator << (std::ostream& sink, FileLoc const& fl) { sink << fl.name << ':' << fl.line << ": "; return sink; }
  };
  
  void
  read_repos( std::string const& reposname )
  {
    FileLoc fl( reposname );
    std::ifstream source( fl.name );
    
    while (source)
      {
        fl.newline();
        // Parsing incoming repository
        MemCode code;
        if (not code.get(source)) break;
        std::unique_ptr<Operation> codeop;
        std::string disasm;
        {
          std::string old_disasm;
          std::getline( source, old_disasm, '\n' );
          codeop = std::unique_ptr<Operation>( isa.decode( 0x4000, code, disasm ) );
          if (old_disasm != disasm)
            std::cerr << "Warning: assembly code divergence.\n   new: " << disasm << "\n   old: " << old_disasm << "\n";
        }
        
        // Performing an abstract execution to check the validity of
        // the opcode, and to compute the interface of the operation
        try
          {
            ut::Interface interface( *codeop );
            // Finally recording the operation test
            testdb.emplace( std::piecewise_construct,
                            std::forward_as_tuple( std::move( interface ) ),
                            std::forward_as_tuple( code, disasm ) );
          }
        catch (ut::Untestable const& denial)
          {
            std::cerr << fl << ": behavioral rejection for " << disasm << " <" << denial.reason << ">\n";
            continue;
          }
      }
  }
  
  void
  write_tests( Sink& sink )
  {
    for (auto && test : testdb)
      {
        // std::string name = tcitem.first;
        
        // TestConfig cfg( ccitem.first, std::string( ISA::Name() ) + '_' + name, disasm( ccitem.second ) );
        // isa.write_test( sink, cfg, ccitem.second );
      }
  }
};

template <typename T>
void Update( std::string const& reposname )
{
  uintptr_t ttl = 10000;
  if (char const* ttl_cfg = getenv("TTL_CFG")) { ttl = strtoull(ttl_cfg,0,0); }
  
  std::string suffix(".tests");
  
  if ((suffix.size() >= reposname.size()) or not std::equal(suffix.rbegin(), suffix.rend(), reposname.rbegin()))
    {
      std::cerr << "Bad test repository name (should ends with " << suffix << ").\n";
      throw 0;
    }
  
  Checker<T> checker;
  checker.read_repos( reposname );
  checker.discover( ttl );
  checker.write_repos( reposname );
  std::string basename( reposname.substr( 0, reposname.size() - suffix.size() ) );
  Sink sink( basename );
  checker.write_tests( sink );
}

int
main( int argc, char** argv )
{
  try
    {
      if (argc != 2) {
        std::cerr << "Wrong number of argument.\n";
        throw 0;
      }
      
      Update<AMD64>( argv[1] );
    }
  catch (...)
    {
      std::cerr << argv[0] << " <path_to_test_file>\n";
      return 1;
    }

  std::cout << "nothing.\n";
  
  return 0;
}
