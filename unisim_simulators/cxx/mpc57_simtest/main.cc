/*
 *  Copyright (c) 2015-2017,
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

#include <top_mpc57.hh>
#include <arch.hh>
#include <testutils.hh>
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
  uint8_t offset;
  
  TestConfig( ut::Interface const& _iif, std::string const& _ident, std::string const& _disasm )
    : iif( _iif ), ident( _ident ), disasm( _disasm ), offset( 0 )
  {}
  
  std::string prologue() const
  {
    if (iif.usemem()) {
      std::ostringstream sink;
      try {
        auto const& p = iif.GetPrologue();
        mpc57::GPRPrint br( p.base );
        int32_t voff = p.offset + uint32_t(offset)*(p.sign ? -1 : +1);
        {
          mpc57::HexPrint xoff( voff );
          sink << "\te_lis\t" << br << ", " << xoff << "@h\n";
          sink << "\te_or2i\t" << br << ", " << xoff << "@l\n";
        }
        if (p.sign)
          sink << "\tsub\t" << br << ", " << br << ", r3\n";
        else
          sink << "\tadd\t" << br << ", " << br << ", r3\n";
        for (auto reg : p.regs) {
          mpc57::GPRPrint rname( reg.first );
          mpc57::HexPrint rvalue( reg.second );
          sink << "\te_lis\t" << rname << ", " << rvalue << "@h\n";
          sink << "\te_or2i\t" << rname << ", " << rvalue << "@l\n";
        }
      } catch (ut::Interface::Prologue::Error const& x) {
        std::cerr << "Prologue error in: " << disasm << ".\n";
        throw x;
      }
      return sink.str();
    }
    return "";
  }
  
  std::string epilogue() const { return ""; }
  
  TestConfig& altmem( unsigned _offset )
  {
    offset = _offset;
    switch (_offset) {
    case 1: ident += "_o1"; break;
    case 2: ident += "_o2"; break;
    case 3: ident += "_o3"; break;
    }
    return *this;
  }
};

struct MPC57 : mpc57::Decoder
{
  typedef mpc57::DecodeTableEntry DecodeTableEntry;
  typedef mpc57::CodeType         CodeType;
  typedef mpc57::Operation        Operation;
  
  static CodeType mkcode( uint32_t code ) { return CodeType( code ); }
  static char const* Name() { return "VLE"; }
  void
  write_test( Sink& sink, TestConfig const& cfg, CodeType code )
  {
    std::string hexcode;
    { std::ostringstream oss; oss << std::hex << code; hexcode = oss.str(); }
    std::string opfunc_name = sink.test_prefix() + cfg.ident + '_' + hexcode;
    
    sink.macros << "ENTRY(" << opfunc_name << ",0x" << hexcode << ")\n";
    
    sink.sources << "\t.text\n\t.align\t2\n\t.global\t" << opfunc_name
                 << "\n\t.type\t" << opfunc_name
                 << ", @function\n" << opfunc_name
                 << ":\n"
                 << cfg.prologue()
                 << "\t.long\t0x" << hexcode << '\t' << "/* " << cfg.disasm << " */\n"
                 << cfg.epilogue()
                 << "\tse_blr\n"
                 << "\t.size\t" << opfunc_name
                 << ", .-" << opfunc_name << "\n\n";
  }
  void
  write_tests( Sink& sink, TestConfig const& cfg, CodeType code )
  {
    write_test( sink, cfg, code );
  }
};

template <typename ISA>
struct Checker
{
  typedef typename ISA::CodeType CodeType;
  typedef typename ISA::Operation Operation;
  typedef std::multimap<ut::Interface, CodeType> CodeClass;
  typedef std::map<std::string, CodeClass> TestClasses;
  
  unisim::util::random::Random random;
  ISA isa;
  
  TestClasses testclasses;
  
  Checker() : random( 1, 2, 3, 4 ) {}
  
  void discover( uintptr_t ttl )
  {
    uint64_t step = 0;
    auto const& dectable = isa.GetDecodeTable();
    for (auto&& opc : isa.GetDecodeTable())
      {
        uint32_t mask = opc.opcode_mask, bits = opc.opcode & mask;
        auto testclass = testclasses.end();
        std::map<std::string,uintptr_t> fails;
        
        for (uintptr_t trial = 0; trial < ttl; ++trial)
          {
            step += 1;
            try
              {
                CodeType code = ISA::mkcode( (random.Generate() & ~mask) | bits );
                
                std::unique_ptr<Operation> codeop( opc.decode( code, 0 ) );
                
                {
                  std::string name( codeop->GetName() );
                  if (testclass == testclasses.end()) {
                    testclasses[name];
                    testclass = testclasses.find(name);
                    std::cerr << "Tests[" << ISA::Name() << "::" << name << "]: ";
                  } else if (testclass->first != name) {
                    std::cerr << "Incoherent Operation names: " << testclass->first << " and " << name << std::endl;
                    throw 0;
                  }
                }
                
                if (codeop->donttest())
                  {
                    fails["not under test"] += 1;
                    break;
                  }
                
                {
                  std::unique_ptr<Operation> realop( isa.NCDecode( 0, code ) );
                  if (strcmp( realop->GetName(), codeop->GetName() ) != 0)
                    continue; /* Not the op we were looking for. */
                }
                
                if (testclass->second.size() >= 256) {
                  std::cerr << "Possible issue: too many tests for" << testclass->first << "...\n";
                  throw 0;
                }
                
                // We need to perform an abstract execution of the
                // instruction to 1/ further check testability and 2/
                // compute operation interface.
                ut::Interface interface( *codeop );

                // At this point, code corresponds to valid operation
                // to be tested. Nevertheless, if the interface of
                // this operation matches an existing test, we don't
                // add the operation since the new test is unlikely to
                // reveal new bugs.
                if (testclass->second.find( interface ) != testclass->second.end()) continue;
                testclass->second.insert( std::make_pair( interface, code ) );
                trial = 0;
              }
            catch (ut::Untestable const& denial)
              {
                fails[denial.reason] += 1;
                if (denial.reason == "not implemented")
                  break;
              }
          }
        if (testclass == testclasses.end())
          std::cerr << "Tests[" << ISA::Name() << "::?]: nothing found...\n";
        else if (testclass->second.size() == 0)
          {
            std::cerr << "nothing found";
            for (auto&& reason : fails)
              std::cerr << " <" << reason.first << ">";
            std::cerr << "\n";
          }
        else
          std::cerr << testclass->second.size() << " patterns found." << std::endl;
      }
  }
  
  void
  write_repos( std::string const& reposname )
  {
    std::ofstream sink( reposname );
    
    typedef typename TestClasses::value_type TCItem;
    typedef typename CodeClass::value_type CCItem;
    
    for (TCItem const& tcitem : testclasses)
      {
        std::string name = tcitem.first;
        
        for (CCItem const& ccitem : tcitem.second)
          {
            sink << name << '\t' << std::hex << uint32_t( ccitem.second ) << std::dec
                 << "\t# " << this->disasm( ccitem.second ) << '\n';
          }
      }
  }

  std::string disasm( CodeType code )
  {
    std::unique_ptr<Operation> operation( isa.NCDecode( 0, code ) );
    std::ostringstream oss;
    operation->disasm( 0, oss );
    return oss.str();
  }
  
  struct FileLoc
  {
    std::string name;
    unsigned    line;
    
    FileLoc( std::string const& _name ) : name( _name ) {}
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
        std::string name;
        if (not (source >> name)) break;
        uint32_t rawcode;
        if (not (source >> std::hex >> rawcode)) break;
        std::string extra;
        std::getline( source, extra, '\n' );
        // Decoding operation and checking that given operation name is coherent
        CodeType code = ISA::mkcode( rawcode );
        
        std::unique_ptr<Operation> codeop( isa.NCDecode( 0, code ) );
        if (name != codeop->GetName()) {
          std::cerr << "Operation '" << std::hex << rawcode << std::dec << "' "
                    << "is said to be: '" << name << "' "
                    << "whereas it is: '" << codeop->GetName() << "'\n";
          throw 0;
        }
        
        if (codeop->donttest())
          {
            std::cerr << fl << " explicit rejection ('donttest') for " << codeop->GetName() << "\n";
            continue;
          }
          
        // Performing an abstract execution to check the validity of
        // the opcode, and to compute the interface of the operation
        try
          {
            ut::Interface interface( *codeop );
            // Finally recording the operation test
            testclasses[name].insert( std::make_pair( interface, code ) );
          }
        catch (ut::Untestable const& denial)
          {
            std::cerr << fl << ": behavioral rejection for " << this->disasm( code ) << " <" << denial.reason << ">\n";
            continue;
          }
        
      }
    
  }
  
  void
  write_tests( Sink& sink )
  {
    typedef typename TestClasses::value_type TCItem;
    typedef typename CodeClass::value_type CCItem;
    
    for (TCItem const& tcitem : testclasses)
      {
        std::string name = tcitem.first;
        
        for (CCItem const& ccitem : tcitem.second)
          {
            TestConfig cfg( ccitem.first, std::string( ISA::Name() ) + '_' + name, disasm( ccitem.second ) );
            if (cfg.iif.usemem()) {
              unsigned offset_end = cfg.iif.aligned ? 1 : 4;
              for (unsigned offset = 0; offset < offset_end; ++offset) {
                isa.write_tests( sink, TestConfig( cfg ).altmem( offset ), ccitem.second );
              }
            } else {
              isa.write_tests( sink, cfg, ccitem.second );
            }
          }
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
  
  std::string basename( reposname.substr( 0, reposname.size() - suffix.size() ) );
  
  Sink sink( basename );
      
  Checker<T> checker;
  checker.read_repos( reposname );
  checker.discover( ttl );
  checker.write_repos( reposname );
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
      
      Update<MPC57>( argv[1] );
    }
  catch (...)
    {
      std::cerr << argv[0] << " <path_to_test_file>\n";
      return 1;
    }
  
  return 0;
}
