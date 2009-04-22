/*
 *  Copyright (c) 2007,
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
 * Authors: Gilles Mouchard (gilles.mouchard@cea.fr)
 */
 
#ifndef __UNISIM_COMPONENT_TLM2_MEMORY_RAM_MEMORY_TCC__
#define __UNISIM_COMPONENT_TLM2_MEMORY_RAM_MEMORY_TCC__

#define LOCATION __FUNCTION__ << ":" << __FILE__ << ":" <<  __LINE__

namespace unisim {
namespace component {
namespace tlm2 {
namespace memory {
namespace ram {

//using unisim::service::interfaces::operator<<;
using unisim::kernel::logger::Logger;
using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;

/* Constructor */
template <unsigned int BUSWIDTH, uint32_t PAGE_SIZE, bool DEBUG>
Memory<BUSWIDTH, PAGE_SIZE, DEBUG>::
Memory(const sc_module_name& name, Object *parent) :
	Object(name, parent),
	sc_module(name),
	unisim::component::cxx::memory::ram::Memory<uint64_t, PAGE_SIZE>(name, parent),
	slave_sock("slave-sock"),
	verbose(false),
	cycle_time(0),
	cycle_sctime(),
	param_cycle_time("cycle-time", this, cycle_time, "memory cycle time in picoseconds"),
	param_verbose("verbose", this, verbose),
	logger(*this)
{
	slave_sock(*this);
}

/* Destructor */
template <unsigned int BUSWIDTH, uint32_t PAGE_SIZE, bool DEBUG>
Memory<BUSWIDTH, PAGE_SIZE, DEBUG>::
~Memory() {
}

/* ClientIndependentSetup */
template <unsigned int BUSWIDTH, uint32_t PAGE_SIZE, bool DEBUG>
bool Memory<BUSWIDTH, PAGE_SIZE, DEBUG>::
Setup() {
	if(IsVerbose())
		logger << DebugInfo << LOCATION
			<< " cycle time of " << cycle_time << " ps" 
			<< std::endl << EndDebugInfo;
	if(!cycle_time) {
		logger << DebugError << LOCATION
				<< "cycle time must be different than 0" << std::endl
				<< EndDebugError;
		return false;
	}
	cycle_sctime = sc_time(cycle_time, SC_PS);
	return unisim::component::cxx::memory::ram::Memory<uint64_t, PAGE_SIZE>::Setup();
}

/* TLM2 Slave methods */
template <unsigned int BUSWIDTH, uint32_t PAGE_SIZE, bool DEBUG>
bool Memory<BUSWIDTH, PAGE_SIZE, DEBUG>::get_direct_mem_ptr(tlm::tlm_generic_payload& payload, tlm::tlm_dmi& dmi_data)
{
	tlm::tlm_command cmd = payload.get_command();
	uint64_t addr = payload.get_address();
	uint64_t dmi_start_addr;
	uint64_t dmi_end_addr;
	sc_core::sc_time dmi_read_latency = cycle_sctime;
	sc_core::sc_time dmi_write_latency = cycle_sctime;
	tlm::tlm_dmi::dmi_access_e dmi_granted_access = tlm::tlm_dmi::DMI_ACCESS_READ_WRITE;

	unsigned char *dmi_ptr = (unsigned char *) inherited::GetDirectAccess(addr, dmi_start_addr, dmi_end_addr);

	if(!dmi_ptr) return false;

	dmi_data.set_dmi_ptr(dmi_ptr);
	dmi_data.set_start_address(dmi_start_addr);
	dmi_data.set_end_address(dmi_end_addr);
	dmi_data.set_read_latency(dmi_read_latency);
	dmi_data.set_write_latency(dmi_write_latency);
	dmi_data.set_granted_access(dmi_granted_access);
	return true;
}

template <unsigned int BUSWIDTH, uint32_t PAGE_SIZE, bool DEBUG>
unsigned int Memory<BUSWIDTH, PAGE_SIZE, DEBUG>::transport_dbg(tlm::tlm_generic_payload& payload)
{
	payload.set_dmi_allowed(true);

	tlm::tlm_command cmd = payload.get_command();
	uint64_t addr = payload.get_address();
	unsigned char *data_ptr = payload.get_data_ptr();
	unsigned int data_length = payload.get_data_length();
	unsigned char *byte_enable_ptr = payload.get_byte_enable_ptr();
	unsigned int byte_enable_length = byte_enable_ptr ? payload.get_byte_enable_length() : 0;
	unsigned int streaming_width = payload.get_streaming_width();
	bool status;

	switch(cmd)
	{
		case tlm::TLM_READ_COMMAND:
			if(IsVerbose())
			{
				logger << DebugInfo << LOCATION
					<< ":" << sc_time_stamp().to_string()
					<< ": received a TLM_READ_COMMAND payload at 0x"
					<< std::hex << addr << std::dec
					<< " of " << data_length << " bytes in length" << std::endl
					<< EndDebugInfo;
			}

			if(byte_enable_length ||(streaming_width && (streaming_width != data_length)) )
				status = inherited::ReadMemory(addr, data_ptr, data_length, byte_enable_ptr, byte_enable_length, streaming_width);
			else
				status = inherited::ReadMemory(addr, data_ptr, data_length);
			break;
		case tlm::TLM_WRITE_COMMAND:
			if(IsVerbose())
			{
				logger << DebugInfo << LOCATION
					<< ":" << sc_time_stamp().to_string()
					<< ": received a TLM_WRITE_COMMAND payload at 0x"
					<< std::hex << addr << std::dec
					<< " of " << data_length << " bytes in length" << std::endl
					<< EndDebugInfo;
			}
			if(byte_enable_length ||(streaming_width && (streaming_width != data_length)) )
				status = inherited::WriteMemory(addr, data_ptr, data_length, byte_enable_ptr, byte_enable_length, streaming_width);
			else
				status = inherited::WriteMemory(addr, data_ptr, data_length);
			break;
		case tlm::TLM_IGNORE_COMMAND:
			// transport_dbg should not receive such a command
			logger << DebugInfo << LOCATION
					<< ":" << sc_time_stamp().to_string() 
					<< " : received an unexpected TLM_IGNORE_COMMAND payload at 0x"
					<< std::hex << addr << std::dec
					<< " of " << data_length << " bytes in length" << std::endl
					<< EndDebugInfo;
			sc_stop();
			wait(); // leave control to the SystemC kernel
			break;
	}

	if(status)
		payload.set_response_status(tlm::TLM_OK_RESPONSE);
	else
		payload.set_response_status(tlm::TLM_ADDRESS_ERROR_RESPONSE);

	return status ? data_length : 0;
}

template <unsigned int BUSWIDTH, uint32_t PAGE_SIZE, bool DEBUG>
tlm::tlm_sync_enum Memory<BUSWIDTH, PAGE_SIZE, DEBUG>::nb_transport_fw(tlm::tlm_generic_payload& payload, tlm::tlm_phase& phase, sc_core::sc_time& t)
{
	if(phase != tlm::BEGIN_REQ)
	{
		logger << DebugInfo << LOCATION
				<< ":" << (sc_time_stamp() + t).to_string() 
				<< " : received an unexpected phase " << phase << std::endl
				<< EndDebugInfo;
		sc_stop();
		wait(); // leave control to the SystemC kernel
	}

	payload.set_dmi_allowed(true);

	tlm::tlm_command cmd = payload.get_command();
	uint64_t addr = payload.get_address();
	unsigned char *data_ptr = payload.get_data_ptr();
	unsigned int data_length = payload.get_data_length();
	unsigned char *byte_enable_ptr = payload.get_byte_enable_ptr();
	unsigned int byte_enable_length = byte_enable_ptr ? payload.get_byte_enable_length() : 0;
	unsigned int streaming_width = payload.get_streaming_width();
	bool status;

	switch(cmd)
	{
		case tlm::TLM_READ_COMMAND:
			if(IsVerbose())
			{
				logger << DebugInfo << LOCATION
					<< ":" << (sc_time_stamp() + t).to_string()
					<< ": received a TLM_READ_COMMAND payload at 0x"
					<< std::hex << addr << std::dec
					<< " of " << data_length << " bytes in length" << std::endl
					<< EndDebugInfo;
			}

			if(byte_enable_length ||(streaming_width && (streaming_width != data_length)))
				status = inherited::ReadMemory(addr, data_ptr, data_length, byte_enable_ptr, byte_enable_length, streaming_width);
			else
				status = inherited::ReadMemory(addr, data_ptr, data_length);
			break;
			break;
		case tlm::TLM_WRITE_COMMAND:
			if(IsVerbose())
			{
				logger << DebugInfo << LOCATION
					<< ":" << (sc_time_stamp() + t).to_string()
					<< ": received a TLM_WRITE_COMMAND payload at 0x"
					<< std::hex << addr << std::dec
					<< " of " << data_length << " bytes in length" << std::endl
					<< EndDebugInfo;
			}
			if(byte_enable_length ||(streaming_width && (streaming_width != data_length)))
				status = inherited::WriteMemory(addr, data_ptr, data_length, byte_enable_ptr, byte_enable_length, streaming_width);
			else
				status = inherited::WriteMemory(addr, data_ptr, data_length);
			break;
		case tlm::TLM_IGNORE_COMMAND:
			if(IsVerbose())
			{
				logger << DebugInfo << LOCATION
					<< ":" << (sc_time_stamp() + t).to_string()
					<< ": received a TLM_IGNORE_COMMAND payload at 0x"
					<< std::hex << addr << std::dec
					<< " of " << data_length << " bytes in length" << std::endl
					<< EndDebugInfo;
			}
			status = true;
			break;
	}

	payload.set_response_status(tlm::TLM_OK_RESPONSE);
	phase = tlm::BEGIN_RESP;

	if(status)
		payload.set_response_status(tlm::TLM_OK_RESPONSE);
	else
		payload.set_response_status(tlm::TLM_ADDRESS_ERROR_RESPONSE);

	t = t + cycle_sctime;
	return tlm::TLM_COMPLETED;
}

template <unsigned int BUSWIDTH, uint32_t PAGE_SIZE, bool DEBUG>
void Memory<BUSWIDTH, PAGE_SIZE, DEBUG>::b_transport(tlm::tlm_generic_payload& payload, sc_core::sc_time& t)
{
	payload.set_dmi_allowed(true);

	tlm::tlm_command cmd = payload.get_command();
	uint64_t addr = payload.get_address();
	unsigned char *data_ptr = payload.get_data_ptr();
	unsigned int data_length = payload.get_data_length();
	unsigned char *byte_enable_ptr = payload.get_byte_enable_ptr();
	unsigned int byte_enable_length = byte_enable_ptr ? payload.get_byte_enable_length() : 0;
	unsigned int streaming_width = payload.get_streaming_width();
	bool status;

	switch(cmd)
	{
		case tlm::TLM_READ_COMMAND:
			if(IsVerbose())
			{
				logger << DebugInfo << LOCATION
					<< ":" << (sc_time_stamp() + t).to_string()
					<< ": received a TLM_READ_COMMAND payload at 0x"
					<< std::hex << addr << std::dec
					<< " of " << data_length << " bytes in length"
					<< EndDebugInfo;
			}

			if(byte_enable_length ||(streaming_width && (streaming_width != data_length)) )
				status = inherited::ReadMemory(addr, data_ptr, data_length, byte_enable_ptr, byte_enable_length, streaming_width);
			else
				status = inherited::ReadMemory(addr, data_ptr, data_length);
			if (status && IsVerbose())
			{
				logger << DebugInfo << LOCATION
					<< ": raw data read (0x" << std::hex << addr << std::dec
					<< ", " << data_length << ") = 0x";
				for (unsigned int i = 0; i < data_length; i++)
					logger << (unsigned int)(uint8_t)data_ptr[i] << " ";
				logger << EndDebugInfo;
			}
			break;
		case tlm::TLM_WRITE_COMMAND:
			if(IsVerbose())
			{
				logger << DebugInfo << LOCATION
					<< ":" << (sc_time_stamp() + t).to_string()
					<< ": received a TLM_WRITE_COMMAND payload at 0x"
					<< std::hex << addr << std::dec
					<< " of " << data_length << " bytes in length" << std::endl
					<< EndDebugInfo;
			}
			if(byte_enable_length ||(streaming_width && (streaming_width != data_length)) )
				status = inherited::WriteMemory(addr, data_ptr, data_length, byte_enable_ptr, byte_enable_length, streaming_width);
			else
				status = inherited::WriteMemory(addr, data_ptr, data_length);
			break;
		case tlm::TLM_IGNORE_COMMAND:
			if(IsVerbose())
			{
				logger << DebugInfo << LOCATION
					<< ":" << (sc_time_stamp() + t).to_string()
					<< ": received a TLM_IGNORE_COMMAND payload at 0x"
					<< std::hex << addr << std::dec
					<< " of " << data_length << " bytes in length" << std::endl
					<< EndDebugInfo;
			}
			status = true;
			break;
	}

	if(status)
		payload.set_response_status(tlm::TLM_OK_RESPONSE);
	else
		payload.set_response_status(tlm::TLM_ADDRESS_ERROR_RESPONSE);

	t = t + cycle_sctime;
}

} // end of namespace ram
} // end of namespace memory
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#undef LOCATION

#endif // __UNISIM_COMPONENT_TLM2_MEMORY_RAM_MEMORY_TCC__
