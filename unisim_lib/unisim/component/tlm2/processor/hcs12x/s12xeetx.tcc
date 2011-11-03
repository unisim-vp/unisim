/*
 * s12xeetx.tcc
 *
 *  Created on: 10 oct. 2011
 *      Author: rnouacer
 */

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_HCS12X_S12XEETX_TCC_
#define __UNISIM_COMPONENT_CXX_PROCESSOR_HCS12X_S12XEETX_TCC_


#include <unisim/component/tlm2/processor/hcs12x/s12xeetx.hh>

#define LOCATION __FUNCTION__ << ":" << __FILE__ << ":" <<  __LINE__ << ": "

namespace unisim {
namespace component {
namespace tlm2 {
namespace processor {
namespace hcs12x {

using unisim::kernel::logger::Logger;
using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;

/**
 * Constructor.
 *
 * @param name the name of the module
 * @param parent the parent service
 */
/* Constructor */
template <unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
S12XEETX<BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::
S12XEETX(const sc_module_name& name, Object *parent) :
	Object(name, parent, "this module implements a memory")
	, unisim::component::tlm2::memory::ram::Memory<BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>(name, parent)

	, slave_socket("slave_socket")

	, oscillator_cycle_time_int(250000)
	, param_oscillator_cycle_time_int("oscillator-cycle-time", this, oscillator_cycle_time_int)

	, baseAddress(0x0110)
	, param_baseAddress("base-address", this, baseAddress)

	, cmd_interruptOffset(0xBA)
	, param_cmd_interruptOffset("command-interrupt", this, cmd_interruptOffset)

	, global_start_address(0x13EFFF)
	, param_global_start_address("global-start-address", this, global_start_address)
	, global_end_address(0x140000)
	, param_global_end_address("global-end-address", this, global_end_address)

	, protected_area_start_address(0x13FDFF)
	, param_protected_area_start_address("protected-area-start-address", this, protected_area_start_address)

	, protection_enabled(true)
	, param_protection_enabled("protection-enabled", this, protection_enabled)

	, eclkdiv_reg(0)
	, reserved1_reg(0)
	, reserved2_reg(0)
	, ecnfg_reg(0)
	, eprot_reg(0xFF)
	, estat_reg(0)
	, ecmd_reg(0)
	, reserved3_reg(0)
	, eaddr_reg(0)
	, edata_reg(0)

	, min_eeclk_time(6667, SC_PS)

	, is_write_aborted(false)
	, write_aligned_word(false)


{

	interrupt_request(*this);
	slave_socket.register_b_transport(this, &S12XEETX::read_write);

	SC_HAS_PROCESS(S12XEETX);

	SC_THREAD(Process);

}


/**
 * Destructor
 */
template <unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
S12XEETX<BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::
~S12XEETX() {

	// Release registers_registry
	map<string, unisim::util::debug::Register *>::iterator reg_iter;

	for(reg_iter = registers_registry.begin(); reg_iter != registers_registry.end(); reg_iter++)
	{
		if(reg_iter->second)
			delete reg_iter->second;
	}

	registers_registry.clear();

	unsigned int i;
	unsigned int n = extended_registers_registry.size();
	for (i=0; i<n; i++) {
		delete extended_registers_registry[i];
	}

}

template <unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
void S12XEETX<BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::Process()
{

}


/* Service methods */
/** BeginSetup
 * Initializes the service interface. */
template <unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
bool S12XEETX<BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::BeginSetup()
{
	char buf[80];

	sprintf(buf, "%s.ECLKDIV",inherited::name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &eclkdiv_reg);
	extended_registers_registry.push_back(new unisim::kernel::service::Register<uint8_t>("ECLKDIV", this, eclkdiv_reg, "EEPROM Clock Divider Register (ECLKDIV)"));

	sprintf(buf, "%s.RESERVED1",inherited::name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &reserved1_reg);
	extended_registers_registry.push_back(new unisim::kernel::service::Register<uint8_t>("RESERVED1", this, reserved1_reg, "RESERVED1"));

	sprintf(buf, "%s.RESERVED2",inherited::name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &reserved2_reg);
	extended_registers_registry.push_back(new unisim::kernel::service::Register<uint8_t>("RESERVED2", this, reserved2_reg, "RESERVED2"));

	sprintf(buf, "%s.ECNFG",inherited::name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &ecnfg_reg);
	extended_registers_registry.push_back(new unisim::kernel::service::Register<uint8_t>("ECNFG", this, ecnfg_reg, "EEPROM Configuration Register (ECNFG)"));

	sprintf(buf, "%s.EPROT",inherited::name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &eprot_reg);
	extended_registers_registry.push_back(new unisim::kernel::service::Register<uint8_t>("EPROT", this, eprot_reg, "EEPROM Protection Register (EPROT)"));

	sprintf(buf, "%s.ESTAT",inherited::name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &estat_reg);
	extended_registers_registry.push_back(new unisim::kernel::service::Register<uint8_t>("ESTAT", this, estat_reg, "EEPROM Status Register (ESTAT)"));

	sprintf(buf, "%s.ECMD",inherited::name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &ecmd_reg);
	extended_registers_registry.push_back(new unisim::kernel::service::Register<uint8_t>("ECMD", this, ecmd_reg, "EEPROM Command Register (ECMD)"));

	sprintf(buf, "%s.RESERVED3",inherited::name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &reserved3_reg);
	extended_registers_registry.push_back(new unisim::kernel::service::Register<uint8_t>("RESERVED3", this, reserved3_reg, "RESERVED3"));

	sprintf(buf, "%s.EADDR",inherited::name());
	registers_registry[buf] = new SimpleRegister<uint16_t>(buf, &eaddr_reg);
	extended_registers_registry.push_back(new unisim::kernel::service::Register<uint16_t>("EADDR", this, eaddr_reg, "EEPROM Address Register"));

	sprintf(buf, "%s.EDATA",inherited::name());
	registers_registry[buf] = new SimpleRegister<uint16_t>(buf, &edata_reg);
	extended_registers_registry.push_back(new unisim::kernel::service::Register<uint16_t>("EDATA", this, edata_reg, "EEPROM Data Register"));

	oscillator_cycle_time = sc_time(oscillator_cycle_time_int, SC_PS);

	Reset();

	return inherited::BeginSetup();
}

/**
 * TLM2 Slave methods
 */
template <unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
void S12XEETX<BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range)
{
	// Leave this empty as it is designed for memory mapped buses
}

template <unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
bool S12XEETX<BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::get_direct_mem_ptr(tlm::tlm_generic_payload& payload, tlm::tlm_dmi& dmi_data)
{
	return inherited::get_direct_mem_ptr(payload, dmi_data);
}

template <unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
unsigned int S12XEETX<BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::transport_dbg(tlm::tlm_generic_payload& payload)
{

	tlm::tlm_command cmd = payload.get_command();
	sc_dt::uint64 address = payload.get_address();
	unsigned int data_length = payload.get_data_length();

	if ((address < protected_area_start_address) || (!protection_enabled)) {
		return inherited::transport_dbg(payload);
	}
	else {
		if (cmd == tlm::TLM_READ_COMMAND) {
			if (inherited::IsVerbose()) {
				inherited::logger << DebugInfo << LOCATION << " : " << inherited::name() << ":: Reading " << data_length << " bytes @ " << std::hex << address
					<< std::endl << EndDebugInfo;
				std::cerr << "Info::" << inherited::name() << ":: Reading " << data_length << " bytes @ " << std::hex << address << std::endl;
			}
			return inherited::transport_dbg(payload);
		} else {
			inherited::logger << DebugWarning << LOCATION << " : " << inherited::name() << ":: Try to write " << data_length << " bytes @ " << std::hex << address
				<< std::endl << EndDebugWarning;
			std::cerr << "Warning::" << inherited::name() << ":: Try to write " << data_length << " bytes @ " << std::hex << address << std::endl;

			payload.set_response_status(tlm::TLM_OK_RESPONSE);

			return 0;
		}
	}

}

template <unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
tlm::tlm_sync_enum S12XEETX<BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::nb_transport_fw(tlm::tlm_generic_payload& payload, tlm::tlm_phase& phase, sc_core::sc_time& t)
{
	tlm::tlm_command cmd = payload.get_command();
	sc_dt::uint64 address = payload.get_address();
	unsigned int data_length = payload.get_data_length();

	if ((address < protected_area_start_address) || (!protection_enabled)) {
		return inherited::nb_transport_fw(payload, phase, t);
	}
	else {
		if (cmd == tlm::TLM_READ_COMMAND) {
			if (inherited::IsVerbose()) {
				inherited::logger << DebugInfo << LOCATION << " : " << inherited::name() << ":: Reading " << data_length << " bytes @ " << std::hex << address
					<< std::endl << EndDebugInfo;
				std::cerr << "Info::" << inherited::name() << ":: Reading " << data_length << " bytes @ " << std::hex << address << std::endl;
			}
			return inherited::nb_transport_fw(payload, phase, t);
		} else {
			inherited::logger << DebugWarning << LOCATION << " : " << inherited::name() << ":: Try to write " << data_length << " bytes @ " << std::hex << address
				<< std::endl << EndDebugWarning;
			std::cerr << "Warning::" << inherited::name() << ":: Try to write " << data_length << " bytes @ " << std::hex << address << std::endl;

			if (phase == BEGIN_REQ) {
				phase = END_REQ; // update the phase
				payload.acquire();

				return TLM_UPDATED;
			} else {
				inherited::logger << DebugError << sc_time_stamp() << ":" << inherited::name() << ": received an unexpected phase" << std::endl << EndDebugError;
				cerr << "Error: " << sc_time_stamp() << ":" << inherited::name() << ": received an unexpected phase" << endl;
				Object::Stop(-1);
			}

		}
	}

}

template <unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
void S12XEETX<BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::b_transport(tlm::tlm_generic_payload& payload, sc_core::sc_time& t)
{

	tlm::tlm_command cmd = payload.get_command();
	sc_dt::uint64 address = payload.get_address();
	unsigned int data_length = payload.get_data_length();

	if ((address < protected_area_start_address) || (!protection_enabled)) {
		inherited::b_transport(payload, t);
	}
	else {
		if (cmd == tlm::TLM_READ_COMMAND) {
			if (inherited::IsVerbose()) {
				inherited::logger << DebugInfo << LOCATION << " : " << inherited::name() << ":: Reading " << data_length << " bytes @ " << std::hex << address
					<< std::endl << EndDebugInfo;
				std::cerr << "Info::" << inherited::name() << ":: Reading " << data_length << " bytes @ " << std::hex << address << std::endl;
			}
			inherited::b_transport(payload, t);
		} else {
			inherited::logger << DebugWarning << LOCATION << " : " << inherited::name() << ":: Try to write " << data_length << " bytes @ " << std::hex << address
				<< std::endl << EndDebugWarning;
			std::cerr << "Warning::" << inherited::name() << ":: Try to write " << data_length << " bytes @ " << std::hex << address << std::endl;

			payload.set_response_status( tlm::TLM_OK_RESPONSE );
		}
	}

}

// Master methods
template <unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
tlm_sync_enum S12XEETX<BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::nb_transport_bw( XINT_Payload& payload, tlm_phase& phase, sc_core::sc_time& t)
{
	if(phase == BEGIN_RESP)
	{
		payload.release();
		return TLM_COMPLETED;
	}
	return TLM_ACCEPTED;
}

template <unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
void S12XEETX<BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::assertInterrupt() {

	// assert EEPROM_Command_Interrupt

	tlm_phase phase = BEGIN_REQ;
	XINT_Payload *payload = xint_payload_fabric.allocate();

	payload->interrupt_offset = cmd_interruptOffset;

	sc_time local_time = quantumkeeper.get_local_time();

	tlm_sync_enum ret = interrupt_request->nb_transport_fw(*payload, phase, local_time);

	payload->release();

	switch(ret)
	{
		case TLM_ACCEPTED:
			// neither payload, nor phase and local_time have been modified by the callee
			quantumkeeper.sync(); // synchronize to leave control to the callee
			break;
		case TLM_UPDATED:
			// the callee may have modified 'payload', 'phase' and 'local_time'
			quantumkeeper.set(local_time); // increase the time
			if(quantumkeeper.need_sync()) quantumkeeper.sync(); // synchronize if needed

			break;
		case TLM_COMPLETED:
			// the callee may have modified 'payload', and 'local_time' ('phase' can be ignored)
			quantumkeeper.set(local_time); // increase the time
			if(quantumkeeper.need_sync()) quantumkeeper.sync(); // synchronize if needed
			break;
	}

}

template <unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
void S12XEETX<BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::Reset() {

	eclkdiv_reg = 0x00;
	reserved1_reg = 0x00;
	reserved2_reg = 0x00;
	ecnfg_reg = 0x00;
	eprot_reg = 0xFF;
	estat_reg = 0x00;
	ecmd_reg = 0x00;
	reserved3_reg = 0x00;
	eaddr_reg = 0x00;
	edata_reg = 0x00;

	setEEPROMClock();

}

template <unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
void S12XEETX<BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::setEEPROMClock() {

	// if ECLKDIV::PRDIV8 set then enable a prescalar by 8
	eeclk_time = oscillator_cycle_time * ((eclkdiv_reg & 0x3F) + 1) * (((eclkdiv_reg & 0x40) != 0)? 8: 1);

	if (eeclk_time < min_eeclk_time) {
		inherited::logger << DebugWarning << inherited::name() << ":: Setting EECLK to " << 1/eeclk_time.to_seconds() << " Hz can destroy the EEPROM. (EECLK < 150 kHz should be avoided)" << std::endl << EndDebugWarning;
	}
}


//=====================================================================
//=             memory interface methods                              =
//=====================================================================

template <unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
bool S12XEETX<BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::ReadMemory(service_address_t addr, void *buffer, uint32_t size)
{
	if ((addr >= baseAddress) && (addr <= (baseAddress+EDATALO))) {
		service_address_t offset = addr-baseAddress;
		switch (offset) {
			case ECLKDIV: *((uint8_t *) buffer) = eclkdiv_reg; break;
			case RESERVED1: *((uint8_t *) buffer) = reserved1_reg; break;
			case RESERVED2: *((uint8_t *) buffer) = reserved2_reg; break;
			case ECNFG: *((uint8_t *) buffer) = ecnfg_reg; break;
			case EPROT: *((uint8_t *) buffer) = eprot_reg; break;
			case ESTAT: *((uint8_t *) buffer) = estat_reg; break;
			case ECMD: *((uint8_t *) buffer) = ecmd_reg; break;
			case RESERVED3:	*((uint8_t *) buffer) = reserved3_reg; break;
			case EADDRHI: {
				if (size == 2) {
					*((uint16_t *) buffer) = eaddr_reg;
				} else {
					*((uint8_t *) buffer) = (uint8_t) ((eaddr_reg & 0xFF00) >> 8);
				}
			}
			break;
			case EADDRLO: *((uint8_t *) buffer) = (uint8_t) (eaddr_reg & 0x00FF); break;
			case EDATAHI: {
				if (size == 2) {
					*((uint16_t *) buffer) = edata_reg;
				} else {
					*((uint8_t *) buffer) = (uint8_t) ((edata_reg & 0xFF00) >> 8);
				}
			}
			break;
			case EDATALO: *((uint8_t *) buffer) = (uint8_t) (edata_reg & 0x00FF); break;
		}

		return true;
	}

	return inherited::ReadMemory(addr, buffer, size);
}

//template <unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
//bool S12XEETX<BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::WriteMemory(service_address_t addr, const void *buffer, uint32_t size)
//{
//
//	if ((addr >= baseAddress) && (addr <= (baseAddress+EDATALO))) {
//
//		if (size == 0) {
//			return true;
//		}
//
//		service_address_t offset = addr-baseAddress;
//
//		return write(offset, buffer, size);
//	}
//
//	return inherited::WriteMemory(addr, buffer, size);
//}

template <unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
bool S12XEETX<BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::WriteMemory(service_address_t addr, const void *buffer, uint32_t size)
{

	if ((addr >= baseAddress) && (addr <= (baseAddress+EDATALO))) {

		if (size == 0) {
			return true;
		}

		service_address_t offset = addr-baseAddress;
		switch (offset) {
			case ECLKDIV:  eclkdiv_reg = *((uint8_t *) buffer); break;
			case RESERVED1: reserved1_reg = *((uint8_t *) buffer); break;
			case RESERVED2: reserved2_reg = *((uint8_t *) buffer); break;
			case ECNFG: ecnfg_reg = *((uint8_t *) buffer); break;
			case EPROT: eprot_reg = *((uint8_t *) buffer); break;
			case ESTAT: estat_reg = *((uint8_t *) buffer); break;
			case ECMD: ecmd_reg = *((uint8_t *) buffer); break;
			case RESERVED3:	ecmd_reg = *((uint8_t *) buffer); break;
			case EADDRHI: {
				if (size == 2) {
					eaddr_reg = *((uint16_t *) buffer);
				} else {
					eaddr_reg = (eaddr_reg & 0x00FF) | ((uint16_t) *((uint8_t *) buffer) << 8);
				}

			}
			break;
			case EADDRLO: eaddr_reg = (eaddr_reg & 0xFF00) | *((uint8_t *) buffer);	break;
			case EDATAHI: {
				if (size == 2) {
					edata_reg = *((uint16_t *) buffer);
				} else {
					edata_reg = (edata_reg & 0x00FF) | ((uint16_t) *((uint8_t *) buffer) << 8);
				}
			}
			break;
			case EDATALO: edata_reg= (edata_reg & 0xFF00) | *((uint8_t *) buffer); break;
		}

		return true;
	}

	return inherited::WriteMemory(addr, buffer, size);
}

//=====================================================================
//=             EEPROM Registers Interface interface methods               =
//=====================================================================

/**
 * Gets a register interface to the register specified by name.
 *
 * @param name The name of the requested register.
 * @return A pointer to the RegisterInterface corresponding to name.
 */
template <unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
Register * S12XEETX<BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::GetRegister(const char *name) {

	if(registers_registry.find(string(name)) != registers_registry.end())
		return registers_registry[string(name)];
	else
		return NULL;

}

//=====================================================================
//=             registers setters and getters                         =
//=====================================================================
template <unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
bool S12XEETX<BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::read(uint8_t offset, void *buffer, unsigned int data_length)
{
	switch (offset) {
		case ECLKDIV: *((uint8_t *) buffer) = eclkdiv_reg; break;
		case RESERVED1: *((uint8_t *) buffer) = reserved1_reg; break;
		case RESERVED2: *((uint8_t *) buffer) = reserved2_reg; break;
		case ECNFG: *((uint8_t *) buffer) = ecnfg_reg & 0xC0; break;
		case EPROT: *((uint8_t *) buffer) = eprot_reg; break;
		case ESTAT: *((uint8_t *) buffer) = estat_reg & 0xF6; break;
		case ECMD: *((uint8_t *) buffer) = ecmd_reg & 0x7F; break;
		case RESERVED3:	*((uint8_t *) buffer) = reserved3_reg; break;
		case EADDRHI: {
			if (data_length == 2) {
				*((uint16_t *) buffer) = eaddr_reg;
			} else {
				*((uint8_t *) buffer) = (uint8_t) ((eaddr_reg & 0xFF00) >> 8);
			}
		}
		break;
		case EADDRLO: *((uint8_t *) buffer) = (uint8_t) (eaddr_reg & 0x00FF); break;
		case EDATAHI: {
			if (data_length == 2) {
				*((uint16_t *) buffer) = edata_reg;
			} else {
				*((uint8_t *) buffer) = (uint8_t) ((edata_reg & 0xFF00) >> 8);
			}
		}
		break;
		case EDATALO: *((uint8_t *) buffer) = (uint8_t) (edata_reg & 0x00FF); break;

		default: return false;
	}

	return true;
}

template <unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
bool S12XEETX<BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::write(uint8_t offset, const void *buffer, unsigned int data_length)
{
	switch (offset) {
		case ECLKDIV: {
			uint8_t edivld_mask = 0x80;
			uint8_t value = *((uint8_t *) buffer);
			if ((eclkdiv_reg & edivld_mask) == 0) {
				// if ECLKDIV register hasn't written yet then accept write and set EDIVLD bit
				eclkdiv_reg = value | edivld_mask;

				// compute EECLK
				setEEPROMClock();
			}
		}
		break;
		case RESERVED1: reserved1_reg = *((uint8_t *) buffer); break;
		case RESERVED2: reserved2_reg = *((uint8_t *) buffer); break;
		case ECNFG: {
			uint8_t value = *((uint8_t *) buffer) & 0xC0;
			ecnfg_reg = value;
		}
		break;
		case EPROT: {
			/*
			 inherited::logger << DebugWarning << LOCATION << " : " << inherited::name() << ":: Try to write " << data_length << " bytes @ " << std::hex << address
				<< std::endl << EndDebugWarning;
			 */
			uint8_t value = *((uint8_t *) buffer);

			// RNV[6:4] bits should remain in the erased state "1" for future enhancements.
			value = value | 0x70;

			if (((eprot_reg & 0x80) == 0) && ((value & 0x80) != 0)) {
				inherited::logger << DebugWarning << LOCATION << " : " << inherited::name() << ":: Try writing to EPOPEN. EERPOM is in protected mode." << std::endl << EndDebugWarning;
				value = value & 0x7F;
			}

			if (((eprot_reg & 0x08) == 0) && ((value & 0x08) != 0)) {
				inherited::logger << DebugWarning << LOCATION << " : " << inherited::name() << ":: Try writing to EPOPEN. EERPOM is in protected mode." << std::endl << EndDebugWarning;
				value = value & 0xF7;
			}

			// the EPS[2:0] bits can be written anytime until bit EPDIS is cleared
			if ((eprot_reg & 0x04) == 0) {
				inherited::logger << DebugWarning << LOCATION << " : " << inherited::name() << ":: Try writing to EPS[2:0] bits. EPDIS is cleared." << std::endl << EndDebugWarning;
				value = (value & 0xF8) | (eprot_reg & 0x07);
			}

			eprot_reg = value;
			uint16_t protected_size = ((eprot_reg & 0x03) + 1) * 64;

			protected_area_start_address = global_end_address - protected_size + 1;
			/**
			 * EPROT::EPDIS = 0 protection enabled
			 * EPROT::EPDIS = 1 protection disabled
			 */
			protection_enabled = ((eprot_reg & 0x04) == 0);

		}
		break;
		case ESTAT: {
			uint8_t value = *((uint8_t *) buffer);

			if ((value & 0x80) != 0) {
				// clear CBEIF
				// CCIF is cleared automatically when CBEIF is cleared
				value = value & 0x3F;
				// BLANK is cleared automatically when CBEIF is cleared
				value = value & 0xFB;
			} else {
				/**
				 * Writing a 0 to CBEIF after writing an aligned word to the EEPROM address space
				 * but before CBEIF is cleared will abort a command write sequence and cause the ACCERR flag to be set.
				 */
				if ((write_aligned_word) && ((estat_reg & 0x80) != 0)) {
					abort_write();
					value = value | 0x10;
				}
			}

			// clear PVIOL
			if ((value & 0x20) != 0) {
				value = value & 0xDF;
			}

			// clear ACCERR
			if ((value & 0x10) != 0) {
				value = value & 0xEF;
			}

			// clear FAIL
			if ((value & 0x02) != 0) {
				value = value & 0xFD;
			}


			estat_reg = value;

		}
		break;
		case ECMD: {
			uint8_t value = *((uint8_t *) buffer);
			value = (value & 0x7F) | (ecmd_reg & 0x80);
			ecmd_reg = value;
			if ((eclkdiv_reg & 0x80) == 0) {
				/**
				 *  If the ECLKDIV register has not been written to,
				 *  the EEPROM command loaded during a command write sequence will not executed
				 *  and the ACCERR flag is set.
				 */
				// set the ACCERR flag in the ESTAT register
				estat_reg = estat_reg | 0x10;
				break;
			}
			switch (ecmd_reg) {
				case 0x05: {
					/* Erase Verify */
					start_erase_verify();
				}
				break;
				case 0x20: {
					/* Word Program */
					word_program();
				}
				break;
				case 0x40: {
					/* Sector Erase */
					sector_erase();
				}
				break;
				case 0x41: {
					/* Mass Erase */
					mass_erase();
				}
				break;
				case 0x47: {
					/* Sector Erase Abort */
					sector_erase_abort();
				}
				break;
				case 0x60: {
					/* Sector Modify */
					sector_modify();
				}
				break;
				default: {
					/* unknown command */
					// set the ACCERR flag in the ESTAT register
					estat_reg = estat_reg | 0x10;
				}
			}
		}
		break;
		case RESERVED3:	ecmd_reg = *((uint8_t *) buffer); break;
		case EADDRHI: {
			if (data_length == 2) {
				eaddr_reg = *((uint16_t *) buffer);
			} else {
				eaddr_reg = (eaddr_reg & 0x00FF) | ((uint16_t) *((uint8_t *) buffer) << 8);
			}

		}
		break;
		case EADDRLO: eaddr_reg = (eaddr_reg & 0xFF00) | *((uint8_t *) buffer);	break;
		case EDATAHI: {
			if (data_length == 2) {
				edata_reg = *((uint16_t *) buffer);
			} else {
				edata_reg = (edata_reg & 0x00FF) | ((uint16_t) *((uint8_t *) buffer) << 8);
			}
		}
		break;
		case EDATALO: edata_reg= (edata_reg & 0xFF00) | *((uint8_t *) buffer); break;

		default: return false;
	}

	return true;
}

template <unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
void S12XEETX<BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::read_write( tlm::tlm_generic_payload& trans, sc_time& delay )
{
	tlm::tlm_command cmd = trans.get_command();
	sc_dt::uint64 address = trans.get_address();
	uint8_t* data_ptr = (uint8_t *)trans.get_data_ptr();
	unsigned int data_length = trans.get_data_length();

	assert(address >= baseAddress);

	if (cmd == tlm::TLM_READ_COMMAND) {
		memset(data_ptr, 0, data_length);
		read(address - baseAddress, data_ptr, data_length);
	} else if (cmd == tlm::TLM_WRITE_COMMAND) {
		write(address - baseAddress, data_ptr, data_length);
	}

	trans.set_response_status( tlm::TLM_OK_RESPONSE );
}

} // end of hcs12x
} // end of processor
} // end of tlm2
} // end of component
} // end of unisim

#endif /* __UNISIM_COMPONENT_CXX_PROCESSOR_HCS12X_S12XEETX_TCC_ */
