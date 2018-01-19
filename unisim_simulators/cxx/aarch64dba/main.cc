#include <aarch64dec.tcc>
#include <iostream>

struct ARMISA : public unisim::component::cxx::processor::arm::isa::arm64::Decoder<armsec::State>
{
  typedef unisim::component::cxx::processor::arm::isa::arm32::CodeType CodeType;
  typedef unisim::component::cxx::processor::arm::isa::arm32::Operation<armsec::State> Operation;
  static CodeType mkcode( uint32_t code ) { return CodeType( code ); }
  static bool const is_thumb = false;
  static armsec::InstructionSet const iset = armsec::Arm;
};


char const* usage()
{
  return
    "usage: <program> [<flags>] <address> <encoding>\n";
}

int
main( int argc, char** argv )
{
  if (argc < 3)
    {
      std::cerr << "Wrong number of CLI arguments.\n" << usage();
      return 1;
    }
  
  uint32_t addr, code;

  if (not getu32(addr, argv[argc-2]) or not getu32(code, argv[argc-1]))
    {
      std::cerr << "<addr> and <code> should be 32bits numeric values.\n" << usage();
      return 1;
    }

  Decoder decoder;
  
  decoder.translate( addr, code );

  return 0;
}
