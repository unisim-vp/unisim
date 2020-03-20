#ifndef __DISASM_HH__
#define __DISASM_HH__

#include <string>
#include <list>
#include <map>
#include <inttypes.h>

struct Recorder
{
  Recorder() {}
  
  void ins( char const* mnemo, char const* args, uint32_t bits, uint32_t mask );
  void fmt( std::string args, uint32_t mask, std::string postfix, std::string encoding, std::string vars, std::string disasm );
  
  void process();
  
  struct Format
  {
    Format( std::string _text, uint32_t _code ) : text( _text ), code( _code ) {}
    bool operator < ( Format const& rhs ) const;
    std::string text; uint32_t code;
  };
  
  struct Group
  {
    Group() {}
    std::string getopname( Format const& insn ) const;
    std::string getdisasm( Format const& insn ) const;
    std::string getencoding( Format const& insn ) const;
    void sanitychecks() const;
    
    std::string postfix, encoding, vars, disasm;
    std::list<Format> instances;
  };
  
  std::map<Format,Group> families;
};

#endif // __DISASM_HH__

