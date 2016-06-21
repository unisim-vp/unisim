#include <top_arm32.tcc>
#include <top_thumb.tcc>

#include <iostream>
#include <cstdlib>

struct State
{
  struct Config
  {
  };
  void dump()
  {
    std::cout << "hello\n";
  }
};

struct ARMISA : public unisim::component::cxx::processor::arm::isa::arm32::Decoder<State>
{
  typedef unisim::component::cxx::processor::arm::isa::arm32::CodeType CodeType;
  typedef unisim::component::cxx::processor::arm::isa::arm32::Operation<State> Operation;
  static CodeType mkcode( uint32_t code ) { return CodeType( code ); }
};

struct THUMBISA : public unisim::component::cxx::processor::arm::isa::thumb2::Decoder<State>
{
  typedef unisim::component::cxx::processor::arm::isa::thumb2::CodeType CodeType;
  typedef unisim::component::cxx::processor::arm::isa::thumb2::Operation<State> Operation;
  static CodeType mkcode( uint32_t code ) {
    if ((code >> 28) == 15)
      return CodeType( code );
    return CodeType( (0x0fffffff & code) | 0xe0000000 );
  }
};


struct Decoder
{
  ARMISA armisa;
  THUMBISA thumbisa;
  
  template <class ISA>
  void
  do_isa( ISA& isa, uint32_t code )
  {
    typename ISA::Operation* op = isa.NCDecode( 0, ISA::mkcode( code ) );
    State state;
    op->execute( state );
    state.dump();
  }
  void  do_arm( uint32_t code ) { do_isa( armisa, code ); }
  void  do_thumb( uint32_t code ) { do_isa( thumbisa, code ); }
};

int
main( int argc, char** argv )
{
  if (argc != 3) {
    std::cerr << "usage: <prog> [arm|thumb] <code>\n";
    return 1;
  }
  
  uint32_t code;
  {
    char const* beg = argv[2]; char *end;
    code = strtoul( argv[2], &end, 0 );
    if ((*beg == '\0') or (*end != '\0')) {
      std::cerr << "Invalid code argument\n";
      return 1;
    }
  }
  
  Decoder decoder;
  if        (std::string(argv[1]) == std::string("arm")) {
    decoder.do_arm( code );
  } else if (std::string(argv[1]) == std::string("thumb")) {
    decoder.do_thumb( code );
  }
  
  return 0;
}
