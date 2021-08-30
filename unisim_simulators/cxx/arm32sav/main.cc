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

#include <scanner.hh>
#include <runner.hh>
#include <test.hh>
#include <unisim/component/cxx/processor/arm/exception.hh>
#include <unisim/component/cxx/processor/arm/models.hh>
#include <unisim/component/cxx/processor/arm/isa/decode.hh>
#include <unisim/component/cxx/processor/arm/disasm.hh>
#include <unisim/util/sav/sav.hh>
#include <unisim/util/random/random.hh>
#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <set>
#include <memory>
#include <sys/mman.h>
#include <inttypes.h>

struct Todo {};

struct TestCode
{
  virtual ~TestCode() {}
  virtual void write(unsigned bytes, uint32_t code) = 0;
  void writ2(unsigned bytes, uint32_t code) { if (bytes > 2) write(2,code>>16); write(2,code); }
};

static uint32_t vsize(unsigned n)
{
  switch (n) {
  case 1: return 0x7 << 8;
  case 2: return 0xa << 8;
  case 3: return 0x6 << 8;
  case 4: return 0x2 << 8;
  }
  return 0;
}

struct ARM32
{
  typedef Scanner::Arm32 ISET;
  // typedef ISET::Decoder Decoder;
  // typedef ISET::CodeType CodeType;
  typedef ISET::Operation Operation;
  
  static uint32_t mkcode( uint32_t code )
  {
    if ((code >> 28) == 15)
      return uint32_t( code );
    return uint32_t( (0x0fffffff & code) | 0xe0000000 );
  }
  static uintptr_t mkfunc(uint8_t* addr) { return reinterpret_cast<uintptr_t>(addr); }
  static char const* Name() { return "Arm32"; }
  static void emit_prologue(TestCode& tc, uint32_t regs) { if (regs) tc.write(4, 0xe92d4000 | regs); else tc.write(4, 0xe52de004); }
  static void emit_set_igprs(TestCode& tc, uint32_t regs) { tc.write(4, 0xe8b00002 | regs); }
  static void emit_set_vgprs(TestCode& tc, unsigned v, unsigned n) { tc.write(4, 0xf42000cd | vsize(n) | ((v & 15) << 12) | ((v & -16) << 18) ); }
  static void emit_set_flags(TestCode& tc) { /*msr psr*/tc.write(4, 0xe12ef001); }
  static void emit_get_flags(TestCode& tc) { /*mrs psr*/tc.write(4, 0xe10f1000); /*setend le*/tc.write(4, 0xf1010000); }
  static void emit_get_ogprs(TestCode& tc, uint32_t regs) { tc.write(4, 0xe8a00002 | regs); }
  static void emit_get_vgprs(TestCode& tc, unsigned v, unsigned n) { tc.write(4, 0xf40000cd | vsize(n) | ((v & 15) << 12) | ((v & -16) << 18) ); }
  static void emit_epilogue(TestCode& tc, uint32_t regs) { if (regs) tc.write(4, 0xe8bd8000 | regs); else tc.write(4, 0xe49df004); }
};

struct THUMB
{
  typedef Scanner::Thumb2 ISET;
  // typedef ISET::Decoder Decoder;
  // typedef ISET::CodeType uint32_t;
  typedef ISET::Operation Operation;
  
  static uint32_t mkcode( uint32_t code )
  {
    if (((code >> 11) & 0b11111) < 0b11101) 
      return uint32_t( code & 0xffff );
    return uint32_t( code );
  }
  static uintptr_t mkfunc(uint8_t* addr) { return reinterpret_cast<uintptr_t>(addr)+1; }
  static char const* Name() { return "Thumb2"; }
  static void emit_prologue(TestCode& tc, uint32_t regs) { if (regs & -256) tc.writ2(4, 0xe92d4000 | regs); else tc.writ2(2, 0xb500 | regs); }
  static void emit_set_igprs(TestCode& tc, uint32_t regs) { if (regs & -256) tc.writ2(4, 0xe8b00002 | regs); else tc.writ2(2, 0xc802 | regs); }
  static void emit_set_vgprs(TestCode& tc, unsigned v, unsigned n) { tc.writ2(4, 0xf92000cd | vsize(n) | ((v & 15) << 12) | ((v & -16) << 18) ); }
  static void emit_set_flags(TestCode& tc) { /*msr psr*/tc.writ2(4,0xf3818e00); }
  static void emit_get_flags(TestCode& tc) { /*mrs psr*/tc.writ2(4,0xf3ef8100); /*setend le*/tc.writ2(2, 0xb650); }
  static void emit_get_ogprs(TestCode& tc, uint32_t regs) { if (regs & -256) tc.writ2(4, 0xe8a00002 | regs); else tc.writ2(2, 0xc002 | regs); }
  static void emit_get_vgprs(TestCode& tc, unsigned v, unsigned n) { tc.writ2(4, 0xf90000cd | vsize(n) | ((v & 15) << 12) | ((v & -16) << 18) ); }
  static void emit_epilogue(TestCode& tc, uint32_t regs) { if (regs & -256) tc.writ2(4, 0xe8bd8000 | regs); else tc.writ2(2, 0xbd00 | regs); }
};

template <typename ISA>
struct Checker
{
  typedef typename ISA::Operation Operation;

  typename ISA::ISET isa;
  TestDB testdb;
  std::map<std::string,uintptr_t> stats;

  Checker() : isa(), testdb() {}

  bool discover( uintptr_t target, uintptr_t ttl_reset )
  {
    uintptr_t count = target - testdb.size();
    if (count > target) return false;
    int seed = testdb.size();
    unisim::util::random::Random rnd(seed,seed,seed,seed);
    uintptr_t ttl = ttl_reset, trial = 0;
    while (count)
      {
        for (auto const& format : isa.GetDecodeTable())
          {
            uint32_t code = ISA::mkcode( (uint32_t(rnd.Generate()) & ~format.opcode_mask) | format.opcode );

            if ((++trial % 0x1000) == 0)
              {
                std::cerr << "\r\e[K#" << std::dec << trial <<  " (total=" << testdb.size() << ")";
                std::cerr.flush();
              }

            if (((test( code ) and ((ttl = ttl_reset) != 0) and (--count == 0))) or
                ((--ttl == 0) and ((count = 0) == 0)))
              break;
          }
      }
    std::cerr << '\n';

    // {
    //   std::ofstream sink( "stats.csv" );
    //   for (auto kv: stats)
    //     {
    //       sink << kv.first << ',' << kv.second << '\n';
    //     }
    //   sink.close();
    // }
    return true;
  }
  bool test( uint32_t trial )
  {
    bool found = false;
    try
      {
        std::string disasm;
        std::unique_ptr<Operation> codeop = std::unique_ptr<Operation>( isa.decode( 0x4000, trial, disasm ) );
        found = insert( *codeop, disasm );
      }
    catch (unisim::util::sav::Untestable const& denial)
      {
        stats[denial.reason] += 1;
      }
    
    return found;
  }
  
  void
  write_repos( std::string const& reposname )
  {
    std::ofstream sink( reposname );

    for (Interface const& test : testdb)
      {
        sink << std::hex << test.insncode << "\t" << test.asmcode << '\n';
      }
  }

  struct FileLoc
  {
    std::string name;
    unsigned    line;
    
    FileLoc( std::string const& _name ) : name(_name), line(0) {}
    void newline() { line += 1; }
    friend std::ostream& operator << (std::ostream& sink, FileLoc const& fl) { return sink << fl.name << ':' << std::dec << fl.line << ": "; }
  };

  bool insert( Operation const& op, std::string const& disasm )
  {
    if (op.donttest()) return false;
    
    Interface iif(op, disasm);

    decltype(testdb.begin()) tstbeg, tstend;
    std::tie(tstbeg,tstend) = testdb.equal_range(iif);
 
    auto count = std::distance(tstbeg, tstend);
    if (count >= 2)
      return false;
    testdb.emplace_hint( tstend, std::move( iif ) );
    return true;
  }
  
  bool
  read_repos( std::string const& reposname )
  {
    FileLoc fl( reposname );
    std::ifstream source( fl.name );
    bool updated = false;
    std::set<std::string> gilcoverage;
    
    while (source)
      {
        fl.newline();
        // Parsing incoming repository
        uint32_t code;
        if (not (source >> std::hex >> code)) break;
        { char tab; if (not source.get(tab) or tab != '\t') { std::cerr << fl << "parse error.\n"; break; } }
        std::string disasm;
        std::getline( source, disasm, '\n' );

        try
          {
            std::string updated_disasm;
            
            // Registering operation
            std::unique_ptr<Operation> codeop( isa.decode( 0x4000, ISA::mkcode(code), updated_disasm ) );
            if (disasm != updated_disasm)
              {
                std::cerr << fl << ": warning, assembly code divergence (" << std::hex << code << ").\n"
                          << "   new: " << updated_disasm << "\n   old: " << disasm << "\n";
                updated = true;
              }
            if (code != codeop->GetEncoding())
              {
                std::cerr << fl << ": warning, encoding divergence (" << disasm << ").\n"
                          << "   new: " << std::hex << codeop->GetEncoding() << "\n   old: " << code << "\n";
                code = codeop->GetEncoding();
                updated = true;
              }
            
            if (not insert( *codeop, updated_disasm ))
              {
                std::cerr << fl << ": warning " << std::hex << code << std::dec << ", " << disasm << " not inserted\n";
                updated = true;
              }
            else
              gilcoverage.insert(codeop->GetName());
          } 
        catch (unisim::util::sav::Untestable const& denial)
          {
            std::cerr << fl << "behavioral rejection for " << std::hex << code << "\t" << disasm << " (reason: " << denial.reason << ")\n";
            updated = true;
            continue;
          }
      }
    
    std::cerr << "Instruction coverage: " << std::dec << gilcoverage.size() << " (genisslib instructions).\n";
    return updated;
  }
  void gencode( TestCode& code, Interface const& test )
  {
    uint16_t grmap = test.grmap;
    uint16_t saved = grmap & 0x0ff0;
    
    ISA::emit_prologue(code, saved);
    ISA::emit_set_igprs(code, grmap);
    for (unsigned v : test.vregs_each())
      ISA::emit_set_vgprs(code, v, 1);
    ISA::emit_set_flags(code);
    code.write(test.insnhalf ? 2 : 4, test.insncode);
    ISA::emit_get_flags(code);
    ISA::emit_get_ogprs(code, grmap);
    for (unsigned v : test.vregs_each())
      ISA::emit_get_vgprs(code, v, 1);
    ISA::emit_epilogue(code, saved);
  }
  
  void gen_tests(std::ostream& sink)
  {
    for (Interface const& test : testdb)
      {
        struct Code : public TestCode
        {
          Code(std::ostream& _sink) : sink(_sink) {} std::ostream& sink;
          void write(unsigned sz, uint32_t insn) override
          {
            uint8_t bytes[4];
            for (unsigned idx = 0; idx < sz; ++idx)
              { bytes[idx] = insn; insn >>= 8; }
            sink.write((char const*)&bytes[0], 4);
          }
        } code(sink);
        gencode(code, test);
      }
  }

  void run_tests(char const* seed)
  {
    /* First pass; computing memory requirements */
    uintptr_t textsize, workcells = 0;
    {
      struct Code : public TestCode
      {
        Code() : size() {} uintptr_t size;
        void write(unsigned bytes, uint32_t) override { size += bytes; }
        void align() { size = (size + 3) & -4; }
      } code;
      
      for (Interface const& test : testdb)
        {
          workcells = std::max( workcells, test.workcells() * (test.usemem() ? 3 : 2) );
          gencode(code, test);
          // TODO: itsensitive
          code.align();
        }
      
      textsize = code.size;
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

       typedef unisim::util::sav::Testbed<uint32_t,4096> Testbed;

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
          relreg(_tif.gindex(dynamic_cast<Scanner::GRegRead const*>(_relreg.node)->reg)),
          workcells(_tif.workcells())
      {}
      uint32_t get_reloc(uint32_t const* ws) const
      {
        if (not relval.good()) return 0;
        
        uint32_t value;
        if (auto v = relval.Eval(Scanner::RelocEval(&ws[data_index(1)], tif, uintptr_t(ws))))
          { Expr dispose(v); value = v->Get( uint32_t() ); }
        else
          throw "WTF";

        return value;
      }
      void patch(uint32_t* ws, uint32_t reloc) const
      {
        ws[data_index(0)] &= 0xf80f0200;
        if (relval.good())
          ws[data_index(1+relreg)] = reloc;
      }
      void load(uint32_t* ws, Testbed const& testbed) const
      {
        testbed.load(ws, relval.good() ? 2*workcells : workcells );
      }
      void fixflags(uint32_t& psr) const
      {
        // normalize to APSR
        psr &= 0xf80f0000;
      }
      void check(Testbed const& tb, uint32_t* ref, uint32_t* sim) const
      {
        fixflags(ref[sink_index(0)]);
        fixflags(sim[sink_index(0)]);
        for (unsigned idx = 0, end = sink_index(workcells); idx < end; ++idx)
          {
            if (ref[idx] != sim[idx])
              error(tb,ref,sim);
          }
      }
      void error(Testbed const& tb, uint32_t const* ref, uint32_t const* sim) const
      {
        std::cerr << tb.counter << ": " << tif.gilname << '@' << ((void*)code) << ": error in " << tif.asmcode << '\n';
        std::cerr << "ref | sim\n";
        for (unsigned idx = 0, end = sink_index(workcells); idx < end; ++idx)
          {
            uint32_t r = ref[idx], s = sim[idx];
            field_name(idx, std::cerr) << ": " << std::hex << r;
            if (r != s)
              std::cerr << " <> " << s << '\n';
            else
              std::cerr << '\n';
          }
        throw 0;
      }

      std::string const& getasm() const { return tif.asmcode; }

      void run( uint32_t* ws ) const
      {
        code( &ws[data_index(0)] ); /*< native code execution */
      }
      void run( Runner& sim, uint32_t* ws ) const
      {
        sim.run( code, &ws[data_index(0)] ); /* code simulation */
      }
      
    private:
      uintptr_t data_index( uintptr_t idx ) const { return (relval.good() ? workcells : 0) + idx; }
      uintptr_t sink_index( uintptr_t idx ) const { return workcells + data_index(idx); }
      
      std::ostream& sub_field_name(unsigned idx, std::ostream& sink) const
      {
        if (idx-- == 0)
          return sink << "cpsr";
        for (unsigned reg = 0; reg < tif.gregs.count(); ++reg)
          {
            if (not tif.gregs.accessed(reg))
              continue;
            if (idx-- == 0)
              return sink << unisim::component::cxx::processor::arm::DisasmRegister(reg);
          }
        return sink << "spare";
      }
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
        sub_field_name(sub, sink);
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
        struct Code : public TestCode
        {
          Code(uint8_t* _mem) : mem(_mem), size(0) {}
          virtual void write(unsigned sz, uint32_t insn)
          {
            uint8_t const* bytes = reinterpret_cast<uint8_t const*>(&insn);
            std::copy( bytes, bytes+sz, &mem[size] );
            size += sz;
          }
          Interface::testcode_t code() const { return (Interface::testcode_t)(ISA::mkfunc(mem)); }
          uint8_t* mem;
          uintptr_t size;
        } code( textzone.chunk(textsize) );
        gencode(code, test);
        textsize = (textsize + code.size + 3) & -4;

        /* Fill out test data */
        if (test.usemem())
          for (auto const& solution : test.addressings.solutions)
            tests.push_back( Test(test, code.code(), solution.first, solution.second) );
        else
          tests.push_back( Test(test, code.code()) );
      }

    /* Now, undergo real tests */
    textzone.activate();

    struct SimEnv : public unisim::kernel::Simulator
    {
      typedef unisim::kernel::Simulator BaseSimulator;
      struct Bad {};
  
      SimEnv() : BaseSimulator(0, 0, SimEnv::DefaultConfiguration) {}
      BaseSimulator::SetupStatus Setup() override { if (BaseSimulator::Setup() != ST_OK_TO_START) throw Bad(); return ST_OK_TO_START; }
      void Stop(unisim::kernel::Object *object, int exit_status, bool asynchronous = false) override { throw Bad (); }
      static void DefaultConfiguration(unisim::kernel::Simulator* sim) {}
  
      //      unisim::kernel::logger::console::Printer logger_console_printer;
    } simenv;
    
    Runner proc("proc");
    
    std::vector<uint32_t> reference(workcells), workspace(workcells);
    for (Testbed testbed(seed);; testbed.next())
      {
        Test const& test = testbed.select(tests);
        if ((testbed.counter % 0x100000) == 0)
          std::cout << testbed.counter << ": " << test.getasm() << std::endl;
        
        /* Perform native test */
        test.load( &workspace[0], testbed );
        // TODO: handle alignment policy [+ ((testbed.counter % tests.size()) % 8)]
        uint32_t reloc = test.get_reloc( &workspace[0] );
        test.patch( &workspace[0],reloc );
        test.run( proc, &workspace[0] );
        std::copy( workspace.begin(), workspace.end(),reference.begin() );
        
        /* Perform simulation test */
        test.load( &workspace[0], testbed );
        test.patch( &workspace[0], reloc );
        test.run( &workspace[0] );
        
        /* Check for differences */
        test.check( testbed, &reference[0], &workspace[0] );
      }
  }
};

struct Args
{
  Args() : repos(0), dump_tests(0), scan_ttl(10000), insn_count(2500), run(false) {}
  char const *repos, *dump_tests;
  uintptr_t scan_ttl, insn_count;
  bool run;

  void dump(std::ostream& sink)
  {
    sink << "repos=" << repos << "\n";
    sink << "dump_tests=" << (dump_tests ? dump_tests : "") << "\n";
    sink << "insn_count=" << insn_count << "\n";
    sink << "scan_ttl=" << scan_ttl << "\n";
    sink << "run=" << (run ? "yes" : "no") << "\n";
  }
};

template <typename T>
void Update( Args const& args )
{
  Checker<T> checker;
  bool updated = checker.read_repos( args.repos );
  
  updated |= checker.discover( args.insn_count, args.scan_ttl );
  if (updated)
    checker.write_repos( args.repos );

  if (args.dump_tests)
    {
      std::cerr << "Dumping tests to " << args.dump_tests << std::endl;
      std::ofstream sink(args.dump_tests);
      checker.gen_tests(sink);
    }

  if (args.run)
    checker.run_tests("01234567890123456789012345678901000000000000000000000000");
}

int
main( int argc, char** argv )
{
  struct Usage {};
  
  try
    {
      Args args;

      char const* arch = 0;
    
      for (int idx = 1; idx < argc; ++idx)
        {
          std::string param(argv[idx]);
          uintptr_t pos = param.find('=');
          if (pos == std::string::npos)
            {
              if      (not arch)       arch = argv[idx];
              else if (not args.repos) args.repos = argv[idx];
              else
                {
                  std::cerr << "error: extra argument `" << param << "`\n";
                  throw Usage();
                }
            }
          else if (param.compare(0,pos,"insn_count") == 0)
            args.insn_count = strtoull(&param[pos+1],0,0);
          else if (param.compare(0,pos,"scan_ttl") == 0)
            args.scan_ttl = strtoull(&param[pos+1],0,0);
          else if (param.compare(0,pos,"dump_tests") == 0)
            args.dump_tests = argv[idx] + pos + 1;
          else if (param.compare("run=yes") == 0)
            args.run = true;
          else
            {
              std::cerr << "Unknown header entry: " << param << std::endl;
              throw Usage();
            }
        }

      if (not arch) { std::cerr << "No arch secified\n"; throw Usage(); }
      if (not args.repos) { std::cerr << "No repository secified (option: <repos=...>)\n"; throw Usage(); }
  
      args.dump(std::cerr);
      
      if      (strcmp("arm32",arch) == 0)
        Update<ARM32>( args );
      else if (strcmp("thumb",arch) == 0)
        Update<THUMB>( args );
      else
        {
          std::cerr << "Wrong arch argument: " << arch << "\n";
          throw Usage();
        }
    }
  catch (Usage const&)
    {
      std::cerr << argv[0] << " [arm32|thumb] <path_to_test_file>\n";
      return 1;
    }
  
  return 0;
}
