/*
 *  Copyright (c) 2019-2020,
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

#include <scanner.hh>
#include <runner.hh>
#include <unisim/util/random/random.hh>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <set>
#include <memory>
#include <sys/mman.h>
#include <inttypes.h>

struct Checker
{
  typedef unisim::util::symbolic::Expr Expr;
    
  Scanner::ISA isa;
  TestDB testdb;
  std::map<std::string,uintptr_t> stats;
  
  bool insert( Scanner::Operation const& op, uint32_t code, std::string const& disasm )
  {
    Interface iif(op, code, disasm);

    decltype(testdb.begin()) tstbeg, tstend;
    std::tie(tstbeg,tstend) = testdb.equal_range(iif);
 
    auto count = std::distance(tstbeg, tstend);
    if (count >= 2)
        return false;
    testdb.emplace_hint( tstend, std::move( iif ) );
    return true;
  }

  bool test( uint32_t trial )
  {
    bool found = false;
    try
      {
        std::string disasm;
        std::unique_ptr<Scanner::Operation> codeop = std::unique_ptr<Scanner::Operation>( isa.decode( 0x4000, trial, disasm ) );
        found = insert( *codeop, trial, disasm );
      }
    catch (unisim::util::sav::Untestable const& denial)
      {
        stats[denial.reason] += 1;
      }
    
    return found;
  }
  
  Checker() {}
  
  void discover( uintptr_t count, uintptr_t ttl_reset )
  {
    int seed = testdb.size();
    unisim::util::random::Random rnd(seed,seed,seed,seed);
    uintptr_t ttl = ttl_reset, trial = 0;
    while (count)
      {
        for (auto format : isa.GetDecodeTable())
          {
            uint32_t code = (uint32_t(rnd.Generate()) & ~format.opcode_mask) | format.opcode;

            if ((++trial % 1024) == 0)
              {
                std::cerr << "\r\e[K#" << trial <<  " (total=" << testdb.size() << ", left=" << count << ")";
                std::cerr.flush();
              }

            if (((test( code ) and ((ttl = ttl_reset) != 0) and (--count == 0))) or
                ((--ttl == 0) and ((count = 0) == 0)))
              break;
          }
      }
    std::cerr << '\n';

    {
      std::ofstream sink( "stats.csv" );
      for (auto kv: stats)
        {
          sink << kv.first << ',' << kv.second << '\n';
        }
      sink.close();
    }
  }
  
  void
  write_repos( std::string const& reposname )
  {
    std::ofstream sink( reposname );

    for (Interface const& test : testdb)
      sink << std::hex << test.memcode << "\t" << test.asmcode << '\n';
  }

  struct FileLoc
  {
    std::string name;
    unsigned    line;
    
    FileLoc( std::string const& _name ) : name( _name ), line(0) {}
    void newline() { line += 1; }
    friend std::ostream& operator << (std::ostream& sink, FileLoc const& fl) { sink << fl.name << ':' << std::dec << fl.line << ": "; return sink; }
  };
  
  bool
  read_repos( std::string const& reposname )
  {
    // Parsing incoming repository
    FileLoc fl( reposname );
    std::ifstream source( fl.name );
    bool updated = false;
    
    while (source)
      {
        fl.newline();

        uint32_t code;
        source >> std::hex >> code;
        { char tab; if (not source.get(tab) or tab != '\t') break; }
        std::string disasm;
        std::getline( source, disasm, '\n' );
        
        try
          {
            std::string updated_disasm;
            std::unique_ptr<Scanner::Operation> codeop = std::unique_ptr<Scanner::Operation>( isa.decode( 0x4000, code, updated_disasm ) );
            if (disasm != updated_disasm)
              {
                std::cerr << fl << ": warning, assembly code divergence (" << std::hex << code << ").\n   new: " << updated_disasm << "\n   old: " << disasm << "\n";
                updated = true;
              }
            if (not insert( *codeop, code, updated_disasm ))
              {
                std::cerr << fl << ": warning " << code << ", " << disasm << " not inserted\n";
                updated = true;
              }
          }
        catch (unisim::util::sav::Untestable const& denial)
          {
            std::cerr << fl << ": behavioral rejection for " << std::hex << code << "\t" << disasm << " <" << denial.reason << ">\n";
            updated = true;
            continue;
          }
      }
    
    return updated;
  }

  void run_tests(char const* seed)
  {
    /* First pass; computing memory requirements */
    uintptr_t textsize, workcells = 0;
    {
      struct Text : public Interface::Text
      {
        Text() : size() {} uintptr_t size;
        void write(uint32_t) override { size += 4; }
        void process( Interface const& t ) { t.gencode(*this); size = (size + 7) & -8; }
      } text;
      
      for (Interface const& test : testdb)
        {
          workcells = std::max( workcells, test.workcells() * (test.usemem() ? 3 : 2) );
          text.process( test );
        }
      
      textsize = text.size;
    }

    /* Manage executable code zone */
    struct TextZone
    {
      TextZone(uintptr_t size)
        : p(mmap(0, size, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0)), sz(size)
      { if (p == MAP_FAILED) throw 0; }
      void activate() { mprotect(p, sz, PROT_EXEC); }
      ~TextZone() { munmap(p, sz); }
      uint8_t* chunk(uintptr_t idx) { return &((uint8_t*)p)[idx]; }
      void* p; uintptr_t sz;
    } textzone(textsize);

    typedef unisim::util::sav::Testbed<uint64_t,4096> Testbed;

    /* Organize test data */
    struct Test
    {
      typedef Interface::testcode_t testcode_t;
      typedef unisim::util::symbolic::Expr Expr;
      
      Test(Interface const& _tif, testcode_t _code)
        : tif(_tif), code(_code), relval(), relreg(), workcells(_tif.workcells())
      {}
      Test(Interface const& _tif, testcode_t _code, Expr const& _relreg, Expr const& _relval)
        : tif(_tif), code(_code), relval(_relval),
          relreg(dynamic_cast<Scanner::GRegRead const*>(_relreg.node)->idx),
          workcells(_tif.workcells())
      {}
      uint64_t get_reloc(uint64_t const* ws) const
      {
        if (not relval.good()) return 0;
        
        uint64_t value;
        if (auto v = relval.Eval( Scanner::RelocEval(&ws[data_index(0)], uint64_t(ws)) ))
          { Expr dispose(v); value = v->Get( uint64_t() ); }
        else
          throw "WTF";

        return value;
      }
      void patch(uint64_t* ws, uint64_t reloc) const
      {
        if (relval.good())
          ws[data_index(relreg)] = reloc;
      }
      void load(uint64_t* ws, Testbed const& testbed) const
      {
        testbed.load(ws, relval.good() ? 2*workcells : workcells );
      }
      void check(Testbed const& tb, uint64_t* ref, uint64_t* sim) const
      {
        for (unsigned idx = 0, end = sink_index(workcells); idx < end; ++idx)
          {
            if (ref[idx] != sim[idx])
              error(tb,ref,sim);
          }
      }
      void error(Testbed const& tb, uint64_t const* ref, uint64_t const* sim) const
      {
        std::cerr << tb.counter << ": " << tif.gilname << '@' << ((void*)code) << ": error in " << tif.asmcode << '\n';
        std::cerr << "ref | sim\n";
        for (unsigned idx = 0, end = sink_index(workcells); idx < end; ++idx)
          {
            uint64_t r = ref[idx], s = sim[idx];
            field_name(idx, std::cerr) << ": " << std::hex << r;
            if (r != s)
              std::cerr << " <> " << s << '\n';
            else
              std::cerr << '\n';
          }
        throw 0;
      }

      std::string const& getasm() const { return tif.asmcode; }

      void run( uint64_t* ws ) const
      {
        code( &ws[data_index(0)] ); /*< native code execution */
      }
      void run( Runner& sim, uint64_t* ws ) const
      {
        sim.run( code, &ws[data_index(0)] ); /* code simulation */
      }
      
    private:
      uintptr_t data_index( uintptr_t idx ) const { return (relval.good() ? workcells : 0) + idx; }
      uintptr_t sink_index( uintptr_t idx ) const { return workcells + data_index(idx); }
      std::ostream& field_name(unsigned idx, std::ostream& sink) const
      {
        unsigned sub = idx % workcells, part = (idx / workcells) + (relval.good() ? 0 : 1);
        if (part == 0)
          {
            sink << "mem@" << std::right << std::setw(2) << std::dec << (sub*8) << "  ";
            return sink;
          }
        sink << (part == 1 ? " in." : "out.");
        std::ostringstream buf;
        tif.field_name( sub, buf );
        sink << std::left << std::setw(4) << buf.str();
        return sink;
      }

      Interface const& tif;
      Interface::testcode_t code;
      Expr relval;
      unsigned relreg, workcells;
    };

    std::vector<Test> tests;
    
    /* 2nd pass; generating tests (data and code)*/
    textsize = 0;
    for (Interface const& test : testdb)
      {
        /* generating test code */
        struct Text : public Interface::Text
        {
          Text(uint8_t* _mem) : mem(_mem), size(0) {}
          virtual void write(uint32_t insn)
          {
            uint8_t const* bytes = reinterpret_cast<uint8_t const*>(&insn);
            unsigned const sz = 4;
            std::copy( bytes, bytes+sz, &mem[size] );
            size += sz;
          }
          Interface::testcode_t code() const { return (Interface::testcode_t)mem; }
          uint8_t* mem;
          uintptr_t size;
        } text( textzone.chunk(textsize) );
        test.gencode( text );
        textsize = (textsize + text.size + 7) & -8;

        /* Fill out test data */
        if (test.usemem())
          for (auto const& solution : test.addressings.solutions)
            tests.push_back( Test(test, text.code(), solution.first, solution.second) );
        else
          tests.push_back( Test(test, text.code()) );

      }

    /* Now, undergo real tests */
    textzone.activate();

    Runner arm64;
    
    std::vector<uint64_t> reference(workcells), workspace(workcells);
    for (Testbed testbed(seed);; testbed.next())
      {
        Test const& test = testbed.select(tests);
        if ((testbed.counter % 0x100000) == 0)
          std::cout << testbed.counter << ": " << test.getasm() << std::endl;
        
        /* Perform native test */
        test.load( &workspace[0], testbed );
        // TODO: handle alignment policy [+ ((testbed.counter % tests.size()) % 8)]
        uint64_t reloc = test.get_reloc( &workspace[0] );
        test.patch( &workspace[0],reloc );
        test.run( &workspace[0] );
        std::copy( workspace.begin(), workspace.end(),reference.begin() );
        
        /* Perform simulation test */
        test.load( &workspace[0], testbed );
        test.patch( &workspace[0], reloc );
        test.run( arm64, &workspace[0] );
        
        /* Check for differences */
        test.check( testbed, &reference[0], &workspace[0]);
      }
  }
};

int
main( int argc, char** argv )
{
  if (argc != 2)
    {
      std::cerr << "Wrong number of argument.\n";
      std::cerr << argv[0] << " <path_to_test_file>\n";
      return 1;
    }

  std::string reposname(argv[1]), suffix(".tests");
  
  if ((suffix.size() >= reposname.size()) or not std::equal(suffix.rbegin(), suffix.rend(), reposname.rbegin()))
    {
      std::cerr << "Bad test repository name (should ends with " << suffix << ").\n";
      return 1;
    }

  uintptr_t ttl_reset, insn_scan;
  struct {
    char const* name;
    uintptr_t& value;
    uintptr_t init;
  } params [] =
      {
       {"INSN_SCAN", insn_scan, 0},
       {"TTL_RESET", ttl_reset, 10000},
      };

  for (unsigned idx = 0, end = sizeof params / sizeof params[0]; idx < end; ++idx)
    {
      if (char const* env = getenv(params[idx].name))
        params[idx].value = strtoull(env,0,0);
      else
        params[idx].value = params[idx].init; 
      std::cerr << "Using " << params[idx].name << " of: " << params[idx].value << std::endl;
    }
  
  
  Checker checker;
  
  bool updated = checker.read_repos( reposname );

  if (insn_scan != 0)
    {
      checker.discover( insn_scan, ttl_reset );
      updated = true;
    }
  
  if (updated)
      checker.write_repos( reposname );

  checker.run_tests("01234567890123456789012345678901000000000000000000000000");

  return 0;
}
