#include <ieee1666/tlm2/interoperability_layer/global_quantum/global_quantum.h>

namespace tlm {

tlm_global_quantum& tlm_global_quantum::instance()
{
}

tlm_global_quantum::~tlm_global_quantum()
{
}

void tlm_global_quantum::set( const sc_core::sc_time& )
{
}

const sc_core::sc_time& tlm_global_quantum::get() const
{
}

sc_core::sc_time tlm_global_quantum::compute_local_quantum()
{
}

tlm_global_quantum::tlm_global_quantum()
{
}

} // end of namespace tlm
