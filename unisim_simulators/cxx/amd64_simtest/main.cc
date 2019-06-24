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
#include <testrun.hh>
#include <unisim/component/cxx/processor/intel/isa/intel.hh>
#include <unisim/util/random/random.hh>
#include <fstream>
#include <iostream>
#include <set>
#include <memory>
#include <sys/mman.h>
#include <inttypes.h>

struct Checker
{
  typedef typename ut::AMD64::MemCode MemCode;
  typedef typename ut::AMD64::Operation Operation;
  typedef unisim::util::symbolic::Expr Expr;
  
  struct TestLess
  {
    bool operator () ( ut::Interface const& a, ut::Interface const& b ) const
    {
      struct Comparator
      {
        int process( ut::ActionNode const& a, ut::ActionNode const& b ) const
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
              uint64_t av = an->Get(uint64_t()), bv = dynamic_cast<ConstNodeBase const&>(*b.node).Get( uint64_t() );
              if (int delta = __builtin_popcountll(av) - __builtin_popcountll(bv))
                return delta;
            }
          else if (auto vr = dynamic_cast<ut::Arch::VirtualRegister const*>(a.node))
            {
              unsigned ai = vr->idx, bi = dynamic_cast<ut::Arch::VirtualRegister const&>(*b.node).idx;
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
  
  typedef std::multiset<ut::Interface, TestLess> TestDB;
  
  unisim::util::random::Random random;
  ut::AMD64 isa;
  std::set<MemCode> done;
  TestDB testdb;

  bool insert( Operation const& op, MemCode const& code, std::string const& disasm )
  {
    if (done.count(code))
      throw ut::Untestable("duplicate");
    
    ut::Interface iif(op, code, disasm);

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
    static uintptr_t counter = 0;
    
    counter += 1;
    std::cerr << "\r\e[K#" << testdb.size() <<  " / " << counter;
    std::cerr.flush();

    bool found = false;
    try
      {
        std::string disasm;
        std::unique_ptr<Operation> codeop = std::unique_ptr<Operation>( isa.decode( 0x4000, trial, disasm ) );
        found = insert( *codeop, trial, disasm );
      }
    catch (ut::Untestable const& denial) {}
    catch (unisim::component::cxx::processor::intel::CodeBase::PrefixError const& denial) {}
    
    done.insert( trial );
    
    return found;
  }
  
  Checker() : random( 1, 2, 3, 4 ) {}
  
  void discover( uintptr_t maxttl, uintptr_t maxtrials )
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

    for (uintptr_t ttl = maxttl; ttl-- > 0 and done.size() < maxtrials;)
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
        uint32_t x = (random.Generate() >> 11) & 0xffff, y= 0x10000-x, carry = 0;
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
        if (test( std::move(mc) ))
          ttl = maxttl;
      }

    std::cerr << '\n';
  }
  
  void
  write_repos( std::string const& reposname )
  {
    std::ofstream sink( reposname );

    for (ut::Interface const& test : testdb)
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
  
  void
  read_repos( std::string const& reposname )
  {
    // Parsing incoming repository
    FileLoc fl( reposname );
    std::ifstream source( fl.name );
    
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
              std::cerr << fl << ": warning, assembly code divergence (" << code << ").\n   new: " << updated_disasm << "\n   old: " << disasm << "\n";
            if (not insert( *codeop, code, updated_disasm ))
              std::cerr << fl << ": warning " << code << ", " << disasm << " not inserted\n";
            
          }
        catch (ut::Untestable const& denial)
          {
            std::cerr << fl << ": behavioral rejection for " << disasm << " <" << denial.reason << ">\n";
            continue;
          }
        done.insert( code );
      }
  }

  void run_tests(char const* seed)
  {
    /* First pass; computing memory requirements */
    uintptr_t textsize, workquads = 0;
    {
      struct Text : public ut::Interface::Text
      {
        Text() : size() {} uintptr_t size;
        virtual void write(uint8_t const*, unsigned sz) { size += sz; }
        void process( ut::Interface const& t ) { t.gencode(*this); size = (size + 7) & -8; }
      } text;
      
      for (ut::Interface const& test : testdb)
        {
          workquads = std::max( workquads, 2*test.workquads() );
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

    /* Organize test data */
    struct Test
    {
      typedef ut::Interface::testcode_t testcode_t;
      
      Test(ut::Interface const* _test, testcode_t _code)
        : code(_code), relval(), relreg(), workquads(_test->workquads())
      {}
      Test(ut::Interface const* _test, testcode_t _code, unsigned _relreg, unisim::util::symbolic::Expr const& _relval)
        : code(_code), relval(_relval), relreg(_test->gregs.index(_relreg)), workquads(_test->workquads())
      {}
      uint64_t get_reloc(std::vector<uint64_t> const& ws, uint64_t address) const
      {
        if (not relval.good()) return 0;
        
        uint64_t value;
        if (auto v = relval.Eval( ut::Arch::RelocEval(ws, address) ))
          { Expr dispose(v); value = v->Get( uint64_t() ); }
        else
          throw "WTF";

        return value;
      }
      void patch(std::vector<uint64_t>& ws, uint64_t reloc) const
      {
        if (relval.good())
          ws[relreg] = reloc;
      }
      void load(std::vector<uint64_t>& ws, ut::Testbed<4096ul> const& testbed) const
      {
        testbed.load(&ws[0], workquads);
      }

      ut::Interface::testcode_t code;
      Expr relval;
      unsigned relreg, workquads;
    };

    std::vector<Test> tests;
    
    /* 2nd pass; generating tests (data and code)*/
    textsize = 0;
    for (ut::Interface const& test : testdb)
      {
        /* generating test code */
        struct Text : public ut::Interface::Text
        {
          Text(uint8_t* _mem) : mem(_mem), size(0) {}
          virtual void write(uint8_t const* bytes, unsigned sz)
          {
            std::copy( bytes, bytes+sz, &mem[size] );
            size += sz;
          }
          ut::Interface::testcode_t code() const { return (ut::Interface::testcode_t)mem; }
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
    
    std::vector<uint64_t> reference(workquads), workspace(workquads);
    
    for (ut::Testbed<4096> testbed( seed );; testbed.next())
      {
        Test const& test = testbed.select(tests);
        uint64_t* data = &workspace[0];
        
        /* Perform native test */
        test.load(workspace, testbed);
        // TODO: handle alignment policy [+ ((testbed.counter % tests.size()) % 8)]
        uint64_t reloc = test.get_reloc( workspace, uint64_t(data) );
        test.patch(workspace,reloc);
        test.code(data);
        std::copy(workspace.begin(),workspace.end(),reference.begin());
        
        /* Perform simulation test */
        test.load(workspace, testbed);
        test.patch(workspace,reloc);
        /* TODO: Perform test here */
        
        /* Check for differences */
        if (testbed.counter >= tests.size()) break;
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
  
  uintptr_t ttl = 10000;
  if (char const* ttl_cfg = getenv("TTL_CFG")) { ttl = strtoull(ttl_cfg,0,0); }
  
  Checker checker;
  
  checker.read_repos( reposname );

  if (getenv("INSN_SCAN"))
    {
      checker.discover( ttl, 16384 );
      
      checker.write_repos( reposname );
    }

  checker.run_tests("01234567890123456789012345678901000000000000000000000000");

  return 0;
}
