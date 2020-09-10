#ifndef __MIPSDECODER_INSTRUCTIONS_HH__
#define __MIPSDECODER_INSTRUCTIONS_HH__

#include <unisim/util/identifier/identifier.hh>
#include <iosfwd>
#include <set>
#include <map>
#include <memory>
#include <inttypes.h>

struct _DomainElementFunctions;
namespace unisim { namespace util { namespace forbint { namespace debug {
  struct Iteration;
  struct MemoryState;
  struct Target;
  struct MemoryFlags;
} } } }
namespace unisim { namespace util { namespace forbint { namespace contract {
  struct MemoryState;
} } } }

namespace Mips
{
  struct Instruction
  {
    virtual ~Instruction() {}
    virtual unsigned get_family() const = 0;
    virtual Instruction* clone() const = 0;
    virtual void disasm(std::ostream& sink) const = 0;
    virtual unsigned getSize() const = 0;
    virtual bool match(uint32_t const* words) const = 0;
    /**DEBUG**/
    virtual void retrieveTargets(unisim::util::forbint::debug::Iteration&) const = 0;
    virtual void interpretForward(uint32_t,
                                  unisim::util::forbint::debug::MemoryState&,
                                  unisim::util::forbint::debug::Target&,
                                  unisim::util::forbint::debug::MemoryFlags&) const = 0;
    /**CONTRACT**/
    virtual bool next_addresses(std::set<unsigned int>&,
                                unisim::util::forbint::contract::MemoryState&,
                                struct _DomainElementFunctions*) const = 0;
    virtual void interpret(uint32_t addr, uint32_t next_addr,
                           unisim::util::forbint::contract::MemoryState&,
                           struct _DomainElementFunctions*) const = 0;
  };
  
  struct RegisterIndex : public unisim::util::identifier::Identifier<RegisterIndex>
  {
    enum Code
      {
       zero = 0
       ,   at, v0, v1, a0, a1, a2, a3,
       t0, t1, t2, t3, t4, t5, t6, t7,
       s0, s1, s2, s3, s4, s5, s6, s7,
       t8, t9, k0, k1, gp, sp, fp, ra,
       hi, lo,
       end
      } code;

    char const* c_str() const
    {
      switch (code)
        {
        case at:  return "at";
        case v0:  return "v0";
        case v1:  return "v1";
        case a0:  return "a0";
        case a1:  return "a1";
        case a2:  return "a2";
        case a3:  return "a3";
        case t0:  return "t0";
        case t1:  return "t1";
        case t2:  return "t2";
        case t3:  return "t3";
        case t4:  return "t4";
        case t5:  return "t5";
        case t6:  return "t6";
        case t7:  return "t7";
        case s0:  return "s0";
        case s1:  return "s1";
        case s2:  return "s2";
        case s3:  return "s3";
        case s4:  return "s4";
        case s5:  return "s5";
        case s6:  return "s6";
        case s7:  return "s7";
        case t8:  return "t8";
        case t9:  return "t9";
        case k0:  return "k0";
        case k1:  return "k1";
        case gp:  return "gp";
        case sp:  return "sp";
        case fp:  return "fp";
        case ra:  return "ra";
        case hi:  return "hi";
        case lo:  return "lo";
        case zero:
        case end: break;
        }
      
      return "NA";
    }
    
    RegisterIndex() : code(end) {}
    RegisterIndex( Code _code ) : code(_code) {}
    RegisterIndex( char const* _code ) : code(end) { init( _code ); }
  };
  
  struct Decoder
  {
    Decoder();
    Instruction* decode(uint32_t const* words, uint32_t addr);
  };
}

#endif /*__MIPSDECODER_INSTRUCTIONS_HH__*/
