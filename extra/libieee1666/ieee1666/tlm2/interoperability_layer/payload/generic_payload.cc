#include <ieee1666/tlm2/interoperability_layer/payload/generic_payload.h>

namespace tlm {

tlm_generic_payload::tlm_generic_payload()
{
}

tlm_generic_payload::tlm_generic_payload( tlm_mm_interface* )
{
}

tlm_generic_payload::~tlm_generic_payload()
{
}

tlm_generic_payload::tlm_generic_payload( const tlm_generic_payload& )
{
}

tlm_generic_payload& tlm_generic_payload::operator= ( const tlm_generic_payload& )
{
}

// Memory management
void tlm_generic_payload::set_mm( tlm_mm_interface* )
{
}

bool tlm_generic_payload::has_mm() const
{
}

void tlm_generic_payload::acquire()
{
}

void tlm_generic_payload::release()
{
}

int tlm_generic_payload::get_ref_count() const
{
}

void tlm_generic_payload::reset()
{
}

void tlm_generic_payload::deep_copy_from( const tlm_generic_payload & )
{
}

void tlm_generic_payload::update_original_from( const tlm_generic_payload & , bool use_byte_enable_on_read)
{
}

void tlm_generic_payload::update_extensions_from( const tlm_generic_payload & )
{
}

void tlm_generic_payload::free_all_extensions()
{
}

// Access methods
tlm_gp_option tlm_generic_payload::get_gp_option() const
{
}

void tlm_generic_payload::set_gp_option( const tlm_gp_option )
{
}

tlm_command tlm_generic_payload::get_command() const
{
}

void tlm_generic_payload::set_command( const tlm_command )
{
}

bool tlm_generic_payload::is_read()
{
}

void tlm_generic_payload::set_read()
{
}

bool tlm_generic_payload::is_write()
{
}

void tlm_generic_payload::set_write()
{
}

sc_dt::uint64 tlm_generic_payload::get_address() const
{
}

void tlm_generic_payload::set_address( const sc_dt::uint64 )
{
}

unsigned char* tlm_generic_payload::get_data_ptr() const
{
}

void tlm_generic_payload::set_data_ptr( unsigned char* )
{
}

unsigned int tlm_generic_payload::get_data_length() const
{
}

void tlm_generic_payload::set_data_length( const unsigned int )
{
}

unsigned int tlm_generic_payload::get_streaming_width() const
{
}

void tlm_generic_payload::set_streaming_width( const unsigned int )
{
}

unsigned char* tlm_generic_payload::get_byte_enable_ptr() const
{
}

void tlm_generic_payload::set_byte_enable_ptr( unsigned char* )
{
}

unsigned int tlm_generic_payload::get_byte_enable_length() const
{
}

void tlm_generic_payload::set_byte_enable_length( const unsigned int )
{
}

// DMI hint
void tlm_generic_payload::set_dmi_allowed( bool )
{
}

bool tlm_generic_payload::is_dmi_allowed() const
{
}

tlm_response_status tlm_generic_payload::get_response_status() const
{
}

void tlm_generic_payload::set_response_status( const tlm_response_status )
{
}

std::string tlm_generic_payload::get_response_string()
{
}

bool tlm_generic_payload::is_response_ok()
{
}

bool tlm_generic_payload::is_response_error()
{
}

tlm_extension_base* tlm_generic_payload::set_extension( unsigned int , tlm_extension_base* )
{
}

tlm_extension_base* tlm_generic_payload::set_auto_extension( unsigned int , tlm_extension_base* )
{
}

tlm_extension_base* tlm_generic_payload::get_extension( unsigned int ) const
{
}

void tlm_generic_payload::resize_extensions()
{
}

} // end of namespace tlm
