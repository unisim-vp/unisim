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

#include <arch.hh>
#include <testrun.hh>
#include <unisim/component/cxx/processor/arm/isa_arm64.tcc>
#include <unisim/util/random/random.hh>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <set>
#include <memory>
#include <sys/mman.h>
#include <inttypes.h>

struct Checker
{
  typedef unisim::util::symbolic::Expr Expr;
  
  struct TestLess
  {
    bool operator () ( review::Interface const& a, review::Interface const& b ) const
    {
      struct Comparator
      {
        int process( review::ActionNode const& a, review::ActionNode const& b ) const
        {
          if (int delta = a.updates.size() - b.updates.size()) return delta;
          auto rci = b.updates.begin();
          for (Expr const& update : a.updates)
            { if (int delta = process( update,  *rci )) return delta; ++rci; }
          
          if (int delta = process( a.cond, b.cond )) return delta;
          for (int idx = 0; idx < 2; ++idx)
            {
              if     (not a.nexts[idx])
                { if (b.nexts[idx]) return -1; }
              else if(b.nexts[idx])
                { if (int delta = process( *a.nexts[idx], *b.nexts[idx] )) return delta; }
            }
          return 0;
        }

        int process( Expr const& a, Expr const& b ) const
        {
          // Do not compare null expressions
          if (not b.node) return a.node ?  1 : 0;
          if (not a.node) return b.node ? -1 : 0;
      
          /* First compare actual types */
          const std::type_info* til = &typeid(*a.node);
          const std::type_info* tir = &typeid(*b.node);
          if (til < tir) return -1;
          if (til > tir) return +1;
        
          /* Same types, call derived comparator except for Constants (compare popcount)*/
          typedef unisim::util::symbolic::ConstNodeBase ConstNodeBase;
          if (auto an = dynamic_cast<ConstNodeBase const*>(a.node))
            {
              // return 0; /* XXX: temporarily considering all constants equivalent */
              uint64_t av = an->Get(uint64_t()), bv = dynamic_cast<ConstNodeBase const&>(*b.node).Get( uint64_t() );
              if (int delta = __builtin_popcountll(av) - __builtin_popcountll(bv))
                return delta;
            }
          else if (auto vr = dynamic_cast<review::Arch::VirtualRegister const*>(a.node))
            {
              unsigned ai = vr->idx, bi = dynamic_cast<review::Arch::VirtualRegister const&>(*b.node).idx;
              if (int delta = int(ai) - int(bi))
                return delta;
            }
          else if (int delta = a.node->cmp( *b.node ))
            return delta;

          /* Compare sub operands recursively */
          unsigned subcount = a.node->SubCount();
          if (int delta = int(subcount) - int(b.node->SubCount()))
            return delta;
          for (unsigned idx = 0; idx < subcount; ++idx)
            if (int delta = process( a.node->GetSub(idx), b.node->GetSub(idx)))
              return delta;

          /* equal to us*/
          return 0;
        }
      } comparator;

      return comparator.process( *a.behavior, *b.behavior ) < 0;
    }
    
  };
  
  typedef std::multiset<review::Interface, TestLess> TestDB;
  
  unisim::util::random::Random rnd;
  review::Decoder isa;
  TestDB testdb;
  std::map<std::string,uintptr_t> stats;
  
  bool insert( review::Operation const& op, uint32_t code, std::string const& disasm )
  {
    review::Interface iif(op, code, disasm);

    decltype(testdb.begin()) tstbeg, tstend;
    std::tie(tstbeg,tstend) = testdb.equal_range(iif);
 
    auto count = std::distance(tstbeg, tstend);
    if (count >= 2)
        return false;
    testdb.emplace_hint( tstend, std::move( iif ) );
    return true;
  }

  review::Operation* decode( uint64_t addr, uint32_t code, std::string& disasm )
  {
    std::ostringstream buf;
    review::Operation* op = 0;
    try { op = isa.NCDecode(addr,code); }
    catch (unisim::component::cxx::processor::arm::isa::Reject const&) { throw review::Untestable("misencoded"); }
      
    review::Arch::DisasmState das;
    op->disasm(das, buf);
    disasm = buf.str();
    return op;
  }

  bool test( uint32_t trial )
  {
    bool found = false;
    try
      {
        std::string disasm;
        std::unique_ptr<review::Operation> codeop = std::unique_ptr<review::Operation>( decode( 0x4000, trial, disasm ) );
        found = insert( *codeop, trial, disasm );
      }
    catch (review::Untestable const& denial)
      {
        stats[denial.reason] += 1;
      }
    
    return found;
  }
  
  Checker() : rnd( 1, 2, 3, 4 ) {}
  
  void discover( uintptr_t count, uintptr_t ttl_reset )
  {
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

    for (review::Interface const& test : testdb)
      sink << std::hex << test.memcode << "\t" << test.asmcode << '\n';
  }

  struct FileLoc
  {
    std::string name;
    unsigned    line;
    
    FileLoc( std::string const& _name ) : name( _name ), line(0) {}
    void newline() { line += 1; }
    friend std::ostream& operator << (std::ostream& sink, FileLoc const& fl) { sink << fl.name << ':' << fl.line << ": "; return sink; }
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
            std::unique_ptr<review::Operation> codeop = std::unique_ptr<review::Operation>( decode( 0x4000, code, updated_disasm ) );
            if (disasm != updated_disasm)
              {
                std::cerr << fl << ": warning, assembly code divergence (" << code << ").\n   new: " << updated_disasm << "\n   old: " << disasm << "\n";
                updated = true;
              }
            if (not insert( *codeop, code, updated_disasm ))
              {
                std::cerr << fl << ": warning " << code << ", " << disasm << " not inserted\n";
                updated = true;
              }
          }
        catch (review::Untestable const& denial)
          {
            std::cerr << fl << ": behavioral rejection for " << code << disasm << " <" << denial.reason << ">\n";
            updated = true;
            continue;
          }
      }
    
    return updated;
  }

  void run_tests(char const* seed)
  {
    /* First pass; computing memory requirements */
    uintptr_t textsize, workquads = 0;
    {
      struct Text : public review::Interface::Text
      {
        Text() : size() {} uintptr_t size;
        virtual void write(uint8_t const*, unsigned sz) { size += sz; }
        void process( review::Interface const& t ) { t.gencode(*this); size = (size + 7) & -8; }
      } text;
      
      for (review::Interface const& test : testdb)
        {
          workquads = std::max( workquads, test.workquads() * (test.relocs.size() ? 3 : 2) );
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

    typedef test::Testbed<4096> Testbed;

    /* Organize test data */
    struct Test
    {
      typedef review::Interface::testcode_t testcode_t;
      
      Test(review::Interface const* _test, testcode_t _code)
        : disasm(_test->asmcode), code(_code), relval(), relreg(), workquads(_test->workquads())
      {}
      Test(review::Interface const* _test, testcode_t _code, unsigned _relreg, unisim::util::symbolic::Expr const& _relval)
        : disasm(_test->asmcode), code(_code), relval(_relval), relreg(_test->gregs.index(_relreg)), workquads(_test->workquads())
      {}
      uint64_t get_reloc(uint64_t const* ws) const
      {
        if (not relval.good()) return 0;
        
        uint64_t value;
        if (auto v = relval.Eval( review::Arch::RelocEval(&ws[data_index(1)], uint64_t(ws)) ))
          { Expr dispose(v); value = v->Get( uint64_t() ); }
        else
          throw "WTF";

        return value;
      }
      static void fixflags( uint64_t& flags ) { flags = (flags & 0xcef) | 2; }
      void patch(uint64_t* ws, uint64_t reloc) const
      {
        fixflags(ws[data_index(0)]);
        if (relval.good())
          ws[workquads+1+relreg] = reloc;
      }
      void load(uint64_t* ws, Testbed const& testbed) const
      {
        testbed.load(ws, relval.good() ? 2*workquads : workquads );
      }
      void check(Testbed const& tb, uint64_t* ref, uint64_t* sim) const
      {
        fixflags(ref[sink_index(0)]);
        fixflags(sim[sink_index(0)]);
        for (unsigned idx = 0, end = sink_index(workquads); idx < end; ++idx)
          {
            if (ref[idx] != sim[idx])
              error(tb,ref,sim);
          }
      }
      void error(Testbed const& tb, uint64_t const* ref, uint64_t const* sim) const
      {
        std::cerr << tb.counter << ": error in " << disasm << '\n';
        std::cerr << "ref | sim\n";
        for (unsigned idx = 0, end = sink_index(workquads); idx < end; ++idx)
          {
            uint64_t r = ref[idx], s = sim[idx];
            std::cerr << idx << ": " << std::hex << r;
            if (r != s)
              std::cerr << " <> " << s << '\n';
            else
              std::cerr << '\n';
          }
        throw 0;
      }

      std::string const& getasm() const { return disasm; }

      void run( uint64_t* ws ) const
      {
        code( &ws[data_index(0)] ); /*< native code execution */
        __asm__ volatile( "cld" ); /*< Fix DF */
      }
      void run( test::Arch& sim, uint64_t* ws ) const
      {
        struct TODO {};
        throw TODO();
        // sim.run( code, &ws[data_index(0)] ); /* code simulation */
        // sim.flagwrite( test::Arch::FLAG::DF, false ); /*< Fix DF */
      }
      
    private:
      uintptr_t data_index( uintptr_t idx ) const { return (relval.good() ? workquads : 0) + idx; }
      uintptr_t sink_index( uintptr_t idx ) const { return workquads + data_index(idx); }
      
      std::string const& disasm;
      review::Interface::testcode_t code;
      Expr relval;
      unsigned relreg, workquads;
    };

    std::vector<Test> tests;
    
    /* 2nd pass; generating tests (data and code)*/
    textsize = 0;
    for (review::Interface const& test : testdb)
      {
        /* generating test code */
        struct Text : public review::Interface::Text
        {
          Text(uint8_t* _mem) : mem(_mem), size(0) {}
          virtual void write(uint8_t const* bytes, unsigned sz)
          {
            std::copy( bytes, bytes+sz, &mem[size] );
            size += sz;
          }
          review::Interface::testcode_t code() const { return (review::Interface::testcode_t)mem; }
          uint8_t* mem;
          uintptr_t size;
        } text( textzone.chunk(textsize) );
        test.gencode( text );
        textsize = (textsize + text.size + 7) & -8;

        /* Fill out test data */
        if (test.base_addr.good())
          for (auto const& reloc : test.relocs)
            tests.push_back( Test(&test, text.code(), reloc.first, reloc.second) );
        else
          tests.push_back( Test(&test, text.code()) );

      }

    /* Now, undergo real tests */
    textzone.activate();

    test::Arch arm64;
    
    std::vector<uint64_t> reference(workquads), workspace(workquads);
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
