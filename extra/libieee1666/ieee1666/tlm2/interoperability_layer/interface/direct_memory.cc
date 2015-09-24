#include <systemc>
#include <tlm>
#include <ieee1666/tlm2/interoperability_layer/interface/direct_memory.h>

namespace tlm {

void tlm_dmi::init()
{
}

unsigned char *tlm_dmi::get_dmi_ptr() const
{
}

sc_dt::uint64 tlm_dmi::get_start_address() const
{
}

sc_dt::uint64 tlm_dmi::get_end_address() const
{
}

sc_core::sc_time tlm_dmi::get_read_latency() const
{
}

sc_core::sc_time tlm_dmi::get_write_latency() const
{
}

tlm_dmi::dmi_access_e tlm_dmi::get_granted_access() const
{
}

bool tlm_dmi::is_none_allowed() const
{
}

bool tlm_dmi::is_read_allowed() const
{
}

bool tlm_dmi::is_write_allowed() const
{
}

bool tlm_dmi::is_read_write_allowed() const
{
}

void tlm_dmi::set_dmi_ptr(unsigned char* p)
{
}

void tlm_dmi::set_start_address(sc_dt::uint64 addr)
{
}

void tlm_dmi::set_end_address(sc_dt::uint64 addr)
{
}

void tlm_dmi::set_read_latency(sc_core::sc_time t)
{
}

void tlm_dmi::set_write_latency(sc_core::sc_time t)
{
}

void tlm_dmi::set_granted_access(tlm_dmi::dmi_access_e t)
{
}

void tlm_dmi::allow_none()
{
}

void tlm_dmi::allow_read()
{
}

void tlm_dmi::allow_write()
{
}

void tlm_dmi::allow_read_write()
{
}

} // end of namespace tlm
