#include <unisim/component/cxx/processor/intel/isa/intel.tcc>
#include <arch.hh>

namespace ut
{

  Operation*
  Arch::fetch( uint64_t address, uint8_t const* bytes )
  {
    typedef  unisim::component::cxx::processor::intel::InputCode<Arch> InputCode;
    return getoperation( InputCode( unisim::component::cxx::processor::intel::Mode( 1, 0, 1 ), bytes, OpHeader( address ) ) );
  }
}
