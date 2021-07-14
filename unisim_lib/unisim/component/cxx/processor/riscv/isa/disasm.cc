#include <unisim/component/cxx/processor/riscv/isa/disasm.hh>
#include <iostream>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace riscv {
namespace isa {

std::ostream& operator << ( std::ostream& sink, Ostreamable const& dobj )
{
  //  GetOstreamableUsage( typeid(dobj).name() ) += 1;
  dobj( sink );
  return sink;
}

void PrintGPR::operator () (std::ostream& sink) const
{
  switch (num)
    {
    case 0: sink << "zero"; break;
    case 1: sink << "ra"; break;
    case 2: sink << "sp"; break;
    case 3: sink << "gp"; break;
    case 4: sink << "tp"; break;
    case 5: case 6: case 7:
      sink << "t" << std::dec << (num-5); break;
      //case 8: sink << "fp"; break;
    case 8: case 9:
      sink << "s" << std::dec << (num-8); break;
    case 10: case 11: case 12: case 13: case 14: case 15: case 16: case 17:
      sink << "a" << std::dec << (num-10); break;
    case 18: case 19: case 20: case 21: case 22: case 23: case 24: case 25: case 26: case 27:
      sink << "s" << std::dec << (num-16); break;
    case 28: case 29: case 30: case 31:
      sink << "t" << std::dec << (num-25); break;
    default:
      sink << "x" << std::dec << num; break;
    }
}

void PrintFPR::operator () ( std::ostream& sink ) const
{
  switch(num)
    {
    case 0: case 1: case 2: case 3: case 4: case 5: case 6: case 7:
      sink << "ft" << std::dec << num; break;
    case 8: case 9:
      sink << "fs" << std::dec << (num-8); break;
    case 10: case 11: case 12: case 13: case 14: case 15: case 16: case 17:
      sink << "fa" << std::dec << (num-10); break;
    case 18: case 19: case 20: case 21: case 22: case 23: case 24: case 25: case 26: case 27:
      sink << "fs" << std::dec << (num-16); break;
    case 28: case 29: case 30: case 31:
      sink << "ft" << std::dec << (num-20); break;
    default:
      sink << "f" << std::dec << num; break;
    }
}

void PrintHex::operator () ( std::ostream& sink ) const { sink << "0x" << std::hex << num; }

void PrintI::operator () ( std::ostream& sink ) const { sink << std::dec << num; }

void PrintAQRL::operator () ( std::ostream& sink ) const
{
  sink << (aqrl ? "." : "") << (aqrl & 2 ? "aq" : "") << (aqrl & 1 ? "rl" : "");
}

void PrintIORW::operator () ( std::ostream& sink ) const
{
  if (iorw)
    for (unsigned bit = 4; bit-- > 0;)
      { if (iorw >> bit & 1) sink << "wroi"[bit]; }
  else
    sink << "unknown";
}

void PrintRM::operator () ( std::ostream& sink ) const
{
  switch (num)
    {
    case 0: sink << "rne"; break;
    case 1: sink << "rtz"; break;
    case 2: sink << "rdn"; break;
    case 3: sink << "rup"; break;
    case 4: sink << "rmm"; break;
    case 5: sink << "unknown"; break;
    case 6: sink << "unknown"; break;
    case 7: sink << "unknown"; break;
    }
}

} /* end of naemspace isa */
} /* end of naemspace riscv */
} /* end of naemspace processor */
} /* end of naemspace cxx */
} /* end of naemspace component */
} /* end of naemspace unisim */

