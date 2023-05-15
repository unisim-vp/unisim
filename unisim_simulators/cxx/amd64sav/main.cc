/*
 *  Copyright (c) 2019-2023,
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
#include <unisim/component/cxx/processor/intel/isa/intel.hh>
#include <unisim/util/random/random.hh>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <vector>
#include <set>
#include <memory>
#include <sys/mman.h>
#include <inttypes.h>

struct Checker
{
  typedef typename review::AMD64::MemCode MemCode;
  typedef typename review::AMD64::Operation Operation;
  typedef unisim::util::symbolic::Expr Expr;

  struct TestLess
  {
    bool operator () ( review::Interface const& a, review::Interface const& b ) const
    {
      return unisim::util::sav::Comparator().process( *a.behavior, *b.behavior ) < 0;
    }
  };

  typedef std::multiset<review::Interface, TestLess> TestDB;

  unisim::util::random::Random rnd;
  review::AMD64
  isa;
  std::set<MemCode> done;
  TestDB testdb;
  std::map<std::string,uintptr_t> stats;

  bool insert( Operation const& op, MemCode const& code, std::string const& disasm )
  {
    if (done.count(code))
      throw unisim::util::sav::Untestable("duplicate");

    review::Interface iif(op, code, disasm);

    decltype(testdb.begin()) tstbeg, tstend;
    std::tie(tstbeg,tstend) = testdb.equal_range(iif);

    auto count = std::distance(tstbeg, tstend);
    if (count > 1)
        return false;
    testdb.emplace_hint( tstend, std::move( iif ) );
    return true;
  }

  bool test( MemCode&& trial )
  {
    bool found = false;
    try
      {
        std::string disasm;
        std::unique_ptr<Operation> codeop = std::unique_ptr<Operation>( isa.decode( 0x4000, trial, disasm ) );
        found = insert( *codeop, trial, disasm );
      }
    catch (unisim::util::sav::Untestable const& denial)
      {
        stats[denial.reason] += 1;
      }
    catch (unisim::component::cxx::processor::intel::CodeBase::PrefixError const& denial) {}

    done.insert( trial );

    return found;
  }

  Checker() : rnd( 1, 2, 3, 4 ) {}

  void discover( uintptr_t trials_limit, uintptr_t ttl_reset )
  {
    test( 0 );
    test( -1 );

    // a priori intel knowledge
    done.emplace("\x06",1);
    done.emplace("\x07",1);
    done.emplace("\x0e",1);
    done.emplace("\x16",1);
    done.emplace("\x17",1);
    done.emplace("\x1e",1);
    done.emplace("\x1f",1);
    done.emplace("\x27",1);
    done.emplace("\x2f",1);
    done.emplace("\x37",1);
    done.emplace("\x3f",1);
    done.emplace("\x60",1);
    done.emplace("\x61",1);
    done.emplace("\x62",1);
    done.emplace("\x82",1);
    done.emplace("\x9a",1);
    done.emplace("\xce",1);
    done.emplace("\xd4",1);
    done.emplace("\xd5",1);
    done.emplace("\xea",1);

    for (uintptr_t ttl = ttl_reset, trials = 0; ttl-- > 0 and trials < trials_limit; ++trials)
      {
        MemCode candidate( 0 );
        typedef decltype (done.begin()) Iterator;
        Iterator best;
        for (Iterator itr, next = done.begin(), end = best = done.end(); itr = next, ++next != end;)
          if (candidate.maximize( *itr, *next ))
            best = itr;
        if (best == done.end()) throw 0;
        MemCode const& a = *best++;
        MemCode const& b = *best++;
        /* Computing a randomly weighted mean to draw a random value
         * in between the two bounds.  The weighted mean is computed
         * using fix point arithmetic with 16 fractional bits.
         */
        uint32_t x = (rnd.Generate() >> 11) & 0xffff, y= 0x10000-x, carry = 0;
        if (x == 0) { x = y = 0x8000; }

        uint8_t bytes[sizeof (MemCode::bytes) + 2];
        for (unsigned idx = sizeof (MemCode::bytes); idx-- > 0;)
          {
            uint32_t ax = x * a.getbyte(idx, 0xff), by = y * b.getbyte(idx, 0x00);
            carry = carry + ax + by;
            bytes[idx+2] = carry;
            carry >>= 8;
          }
        bytes[1] = carry; carry >>= 8;
        bytes[0] = carry; carry >>= 8;
        if (carry) throw 0;
        MemCode mc( &bytes[0], sizeof (MemCode::bytes) );
        if (not (a < mc and mc < b))
          continue;

        if ((trials % 1024) == 0)
          {
            std::cerr << "\r\e[K#" << testdb.size() <<  " / " << done.size();
            std::cerr.flush();
            if ((trials % 8192) == 0)
              {
                static std::ofstream coverage("coverage.csv");
                coverage << testdb.size() << ',' << done.size() << std::endl;
              }
          }

        if (test( std::move(mc) ))
          ttl = ttl_reset;
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
      sink << test.memcode << "\t" << test.asmcode << '\n';
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

        MemCode code;
        if (not code.get(source)) break;
        std::string disasm;
        std::getline( source, disasm, '\n' );

        try
          {
            std::string updated_disasm;
            std::unique_ptr<Operation> codeop = std::unique_ptr<Operation>( isa.decode( 0x4000, code, updated_disasm ) );
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
        catch (unisim::util::sav::Untestable const& denial)
          {
            std::cerr << fl << ": behavioral rejection for " << code << disasm << " <" << denial.reason << ">\n";
            updated = true;
            continue;
          }
        done.insert( code );
      }

    return updated;
  }

  void run_tests(char const* seed)
  {
    /* First pass; computing memory requirements */
    uintptr_t textsize, workcells = 0;
    {
      struct Text : public review::Interface::Text
      {
        Text() : size() {} uintptr_t size;
        virtual void write(uint8_t const*, unsigned sz) { size += sz; }
        void process( review::Interface const& t ) { t.gencode(*this); size = (size + 7) & -8; }
      } text;

      for (review::Interface const& test : testdb)
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
      void activate() { mprotect(p, sz, PROT_READ|PROT_EXEC); }
      ~TextZone() { munmap(p, sz); }
      uint8_t* chunk(uintptr_t idx) { return &((uint8_t*)p)[idx]; }
      void* p; uintptr_t sz;
    } textzone(textsize);

    typedef unisim::util::sav::Testbed<uint64_t,4096> Testbed;

    /* Organize test data */
    struct Test
    {
      typedef unisim::util::symbolic::Expr Expr;
      typedef review::Interface::testcode_t testcode_t;

      Test(review::Interface const* _test, testcode_t _code)
        : disasm(_test->asmcode), code(_code), relval(), relreg(), workcells(_test->workcells()), flagsmask(_test->flagsmask())
      {}
      Test(review::Interface const* _test, testcode_t _code, Expr const& _relreg, Expr const& _relval)
        : disasm(_test->asmcode), code(_code), relval(_relval),
          relreg(dynamic_cast<review::Arch::GRegRead const*>(_relreg.node)->idx), workcells(_test->workcells()), flagsmask(_test->flagsmask())
      {}
      uint64_t get_reloc(uint64_t const* ws) const
      {
        if (not relval.good()) return 0;

        struct GetReloc : public unisim::util::symbolic::Evaluator
        {
          GetReloc( uint64_t const* _regvalues, uint64_t _address ) : regvalues(_regvalues), address(_address) {}
          using ConstNodeBase = unisim::util::symbolic::ConstNodeBase;

          ConstNodeBase const* Simplify( unsigned, Expr& expr ) const override
          {
            if (auto reg = dynamic_cast<review::Arch::GRegRead const*>(expr.node))
              return new unisim::util::symbolic::ConstNode<uint64_t>( regvalues[reg->idx] );
            if (dynamic_cast<review::Arch::ExpectedAddress const*>(expr.node))
              return new unisim::util::symbolic::ConstNode<uint64_t>( address );
            if (ConstNodeBase const* res = expr.Simplify(*this))
              return res;
            throw Failure();
            return 0;
          }

          uint64_t const* regvalues;
          uint64_t address;
        } evaluator(&ws[data_index(1)], uint64_t(ws));

        Expr scratch = relval;
        return dynamic_cast<unisim::util::symbolic::ConstNode<uint64_t> const&>(*evaluator.Simplify(0, scratch)).value;
      }
      void fixflags( uint64_t& flags ) const { flags = (flags & flagsmask & 0xcef) | 2; }
      void patch(uint64_t* ws, uint64_t reloc) const
      {
        fixflags(ws[data_index(0)]);
        if (relval.good())
          ws[data_index(1+relreg)] = reloc;
      }
      void load(uint64_t* ws, Testbed const& testbed) const
      {
        testbed.load(ws, relval.good() ? 2*workcells : workcells );
      }
      void check(Testbed const& tb, uint64_t* ref, uint64_t* sim, test::Arch& arch) const
      {
        if (arch.hasnan)
          {
            arch.hasnan = false;
            return;
          }
        fixflags(ref[sink_index(0)]);
        fixflags(sim[sink_index(0)]);
        for (unsigned idx = 0, end = sink_index(workcells); idx < end; ++idx)
          {
            if (ref[idx] != sim[idx])
              {
                error(tb,ref,sim,arch);
              }
          }
      }
      void error(Testbed const& tb, uint64_t const* ref, uint64_t const* sim, test::Arch& arch) const
      {
        std::cerr << tb.counter << ": error in " << disasm << '\n';
        std::cerr << "ref | sim @ " << (void*)sim << "\n";

        for (unsigned idx = 0, end = sink_index(workcells); idx < end; ++idx)
          {
            std::ostringstream buf;
            if      (idx < data_index(0))
              buf << "mem.";
            else if (idx < sink_index(0))
              buf << "in.";
            else
              buf << "out.";
            buf << (idx % workcells) << ": ";
            uint64_t r = ref[idx], s = sim[idx];
            std::cerr << std::setw(8) << buf.str() << std::hex << r;
            if (r != s)
              std::cerr << " <> " << s << '\n';
            else
              std::cerr << '\n';
          }

        for (auto op : arch.trace_insns)
          {
            op->disasm(std::cerr << std::hex << op->address << "\t");
            std::cerr << std::endl;
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
        sim.run( code, &ws[data_index(0)] ); /* code simulation */
        sim.flagwrite( test::Arch::FLAG::DF, false ); /*< Fix DF */
      }

    private:
      uintptr_t data_index( uintptr_t idx ) const { return (relval.good() ? workcells : 0) + idx; }
      uintptr_t sink_index( uintptr_t idx ) const { return workcells + data_index(idx); }

      std::string const& disasm;
      review::Interface::testcode_t code;
      Expr relval;
      unsigned relreg, workcells;
      uint32_t flagsmask;
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
        if (test.usemem())
          for (auto const& solution : test.addressings.solutions)
            tests.push_back( Test(&test, text.code(), solution.first, solution.second) );
        else
          tests.push_back( Test(&test, text.code()) );

      }

    /* Now, undergo real tests */
    textzone.activate();

    test::Arch amd64;

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
        test.run( amd64, &workspace[0] );

        /* Check for differences */
        test.check( testbed, &reference[0], &workspace[0], amd64 );
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

  uintptr_t ttl_reset, insn_scan, skip_test;
  struct {
    char const* name;
    uintptr_t& value;
    uintptr_t init;
  } params [] =
      {
       {"INSN_SCAN", insn_scan, 0},
       {"TTL_RESET", ttl_reset, 10000},
       {"SKIP_TEST", skip_test, 0},
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

  if (not skip_test)
    checker.run_tests("01234567890123456789012345678901000000000000000000000000");

  return 0;
}
