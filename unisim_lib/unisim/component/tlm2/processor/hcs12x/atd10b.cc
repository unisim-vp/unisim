/*
 *  Copyright (c) 2008,
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
 * Authors: Reda   Nouacer  (reda.nouacer@cea.fr)
 */

#include <unisim/component/tlm2/processor/hcs12x/atd10b.hh>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace hcs12x {



ATD10B::ATD10B(const sc_module_name& name, Object *parent) :
	Object(name, parent),
	sc_module(name),
	Service<Memory<service_address_t> >(name, parent),
	Client<Memory<service_address_t> >(name, parent),
	memory_export("memory_export", this),
	memory_import("memory_import", this),
	baseAddress(0x0080), // MC9S12XDP512V2 - ATD baseAddress
	param_baseAddress("base-address", this, baseAddress),
	bus_cycle_time_int(250), // 250ns => 4 MHz
	param_bus_cycle_time_int("bus-cycle-time", this, bus_cycle_time_int),
	vrl(0),
	vrh(5.12),
	param_vrl("vrl", this, vrl),
	param_vrh("vrh", this, vrh)

{

	slave_socket.register_b_transport(this, &ATD10B::read_write);

	SC_HAS_PROCESS(ATD10B);

	SC_THREAD(Run);
	sensitive << scan_event;
}


ATD10B::~ATD10B() {

}


// Master methods

void ATD10B::invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range)
{
	// Leave this empty as it is designed for memory mapped buses
}


unsigned int ATD10B::transport_dbg(ATD_Payload& payload)
{
	// Leave this empty as it is designed for memory mapped buses
	return 0;
}

/**
 * Name : nb_transport_fw()
 *
 * Role :
 * 1/ Sample and Hold machine accepts analog signals from external world and stores them as capacitor charge on a storage node.
 * 2/ TODO: The analog input multiplexer connects one of the external analog input channels to the sample and hold machine
 * 3/
 */
tlm_sync_enum ATD10B::nb_transport_fw(ATD_Payload& payload, tlm_phase& phase, sc_core::sc_time& t)
{
	switch(phase)
	{
		case BEGIN_REQ:
			// accepts analog signals modeled by payload
			phase = END_REQ; // update the phase

			Input(payload, analog_signal);

			return TLM_UPDATED;
		case END_REQ:
			cout << sc_time_stamp() << ":" << name() << ": received an unexpected phase END_REQ" << endl;
			sc_stop();
			wait(); // leave control to the SystemC kernel
			break;
		case BEGIN_RESP:
			cout << sc_time_stamp() << ":" << name() << ": received an unexpected phase BEGIN_RESP" << endl;
			sc_stop();
			wait(); // leave control to the SystemC kernel
			break;
		case END_RESP:
			cout << sc_time_stamp() << ":" << name() << ": received an unexpected phase END_RESP" << endl;
			sc_stop();
			wait(); // leave control to the SystemC kernel
			break;
		default:
			cout << sc_time_stamp() << ":" << name() << ": received an unexpected phase" << endl;
			sc_stop();
			wait(); // leave control to the SystemC kernel
			break;
	}

	return TLM_ACCEPTED;
}

// Store external analog signals as capacitor charge

void ATD10B::Input(ATD_Payload& payload, double anValue[ATD_SIZE])
{

	for (int i=0; i<ATD_SIZE; i++) {
		anValue[i] = payload.anPort[i];
	}

	payload.release();

	scan_event.notify();
}


void ATD10B::Run()
{
	while(1)
	{
		wait();

		/**
		 * TODO:
		 *  - check ATDCTL0::wrap bits to identify the channel to wrap around
		 *  - check ATDCTL1 to identify which channel to scan
		 *  - check ATDCTL3 to determine the sequence length and storage mode of the result
		 *  - take in account The A/D Resolution Select ATDCTL4::SRES
		 */
		// Find referential potential
		for (int i=0; i < ATD_SIZE; i++) {
			for (int j=0; j < REFERENCIAL_SIZE; j++ ) {
				if (analog_signal[i] < reference_potentials[j]) {
					// TODO: take in account signed/unsigned and 8-10 bits
					atddrhl_register[i] = reference_potentials[j];
				}
			}
		}

		sequenceComplete();

	}
}

void ATD10B::sequenceComplete() {
	/** TODO:
	 *  1/ set ATDCTL2::ASCIF bit
	 *  2/ if ATDCTL2::ASCIE bit is set then assert ATD_Interrupt
	 */
}

void ATD10B::setExternalTriggerMode() {
	/** TODO:
	 *  1/ if ATDCTL2::ETRIGE is set
	 *     then
	 *       - enable the external trigger
	 *       - set ExternalTrigger configuration using ATDCTL2::ETRIGLE and ATDCTL2::ETRIGP
	 *     else disable external trigger
	 */
}

void ATD10B::abortConversion() {
	// TODO:
}

void ATD10B::abortAndStartNewConversion() {
	// TODO:
}

void ATD10B::setATDClock() {

	uint8_t smpMask = 0x60;
	uint8_t smpValue = (atdctl4_register & smpMask) >> 5;
	uint8_t prsMask = 0x1F;
	uint8_t prsValue = atdctl4_register & prsMask;
	if ((bus_cycle_time_int < busClockRange[prsValue].minBusClock) ||
			(bus_cycle_time_int > busClockRange[prsValue].maxBusClock)) {
		cerr << "ATD10B: unallowed prescaler value.\n";
	}

	atd_clock = bus_cycle_time / prsValue;
	first_phase_clock = atd_clock * 2;
	if (smpValue == 0) {
		second_phase_clock = first_phase_clock;
	} else {
		second_phase_clock = first_phase_clock * (2 << smpValue);
	}
}

//=====================================================================
//=             registers setters and getters                         =
//=====================================================================

void ATD10B::read(uint8_t offset, void *buffer) {

	switch (offset) {
		case ATDCTL0: *((uint8_t *) buffer) = atdctl0_register & 0x0F; break;
		case ATDCTL1: *((uint8_t *) buffer) = atdctl1_register & 0x8F; break;
		case ATDCTL2: *((uint8_t *) buffer) = atdctl2_register; break;
		case ATDCTL3: *((uint8_t *) buffer) = atdctl3_register & 0x7F; break;
		case ATDCTL4: *((uint8_t *) buffer) = atdctl4_register; break;
		case ATDCTL5: *((uint8_t *) buffer) = atdctl5_register; break;
		case ATDSTAT0: *((uint8_t *) buffer) = atdstat0_register & 0xBF; break;
		case UNIMPL0007: *((uint8_t *) buffer) = 0x00; break;
		case ATDTEST0: *((uint8_t *) buffer) = 0x00; break;
		case ATDTEST1: *((uint8_t *) buffer) = atdtest1_register & 0x01; break;
		case ATDSTAT2: *((uint8_t *) buffer) = atdstat2_register; break;
		case ATDSTAT1: *((uint8_t *) buffer) = atdstat1_register; break;
		case ATDDIEN0: *((uint8_t *) buffer) = atddien0_register; break;
		case ATDDIEN1: *((uint8_t *) buffer) = atddien1_register; break;
		case PORTAD0: *((uint8_t *) buffer) = portad0_register; break;
		case PORTAD1: *((uint8_t *) buffer) = portad1_register; break;

		default: if ((offset >= ATDDR0H) && (offset < ATDDR0H + ATD_SIZE)) {
			*((uint16_t *) buffer) = atddrhl_register[offset] & 0xFFC0;
			uint8_t index = offset - ATDDR0H;
			uint8_t clearMask = 0xFF;

			if (index < 8) {
				clearMask = clearMask ^ (0x01 < index);
				atdstat1_register = atdstat1_register & clearMask;
			} else {
				clearMask = clearMask ^ (0x01 < (index-8));
				atdstat2_register = atdstat2_register & clearMask;
			}
		} else {
			// TODO: wrong offset
		}
	}

}

void ATD10B::write(uint8_t offset, const void *buffer) {

	switch (offset) {
		case ATDCTL0: {
			atdctl0_register = *((uint8_t *) buffer) & 0x0F;
			abortConversion();
		} break;
		case ATDCTL1: {
			atdctl1_register = *((uint8_t *) buffer) & 0x8F;
			abortConversion();
		} break;
		case ATDCTL2: {
			atdctl2_register = (*((uint8_t *) buffer) & 0xFE) | (atdctl2_register & 0x01);
			abortConversion();
			setExternalTriggerMode();
		} break;
		case ATDCTL3: {
			atdctl3_register = *((uint8_t *) buffer) & 0x7F;
			abortConversion();
		} break;
		case ATDCTL4: {
			atdctl4_register = *((uint8_t *) buffer);
			abortConversion();

			setATDClock();
			// TODO: take in account The A/D Resolution Select ATDCTL4::SRES

		} break;
		case ATDCTL5: {
			atdctl5_register = *((uint8_t *) buffer);
			abortAndStartNewConversion();
			// TODO: see specifications
		} break;
		case ATDSTAT0: {
			atdstat0_register = (*((uint8_t *) buffer) & 0xB0) | (atdstat0_register & 0x0F);
			// TODO: see specifications
		} break;
		case UNIMPL0007: break;
		case ATDTEST0: atdtest0_register = *((uint8_t *) buffer); break;
		case ATDTEST1: atdtest1_register = *((uint8_t *) buffer); break;
		case ATDSTAT2: {
			/* write has no effect */
			// TODO: see specifications
		} break;
		case ATDSTAT1: {
			/* write has no effect */
			// TODO: see specifications
		} break;
		case ATDDIEN0: {
			atddien0_register = *((uint8_t *) buffer);
			// TODO: see specifications
		} break;
		case ATDDIEN1: {
			atddien1_register = *((uint8_t *) buffer);
			// TODO: see specifications
		} break;
		case PORTAD0: {
			/* write has no effect */
			/* TODO
			 *   - PORTAD0 is mapped to external input pins
			 *   - The A/D input channels may be used for general-purpose digital input
			 */
		} break;
		case PORTAD1: {
			/* write has no effect */
			/* TODO
			 *   - PORTAD1 is mapped to external input pins
			 *   - The A/D input channels may be used for general-purpose digital input
			 */
		} break;

		default: if ((offset >= ATDDR0H) && (offset < ATDDR0H+ATD_SIZE)) {
			/* write has no effect */
			// TODO: see specifications
		} else {
		}
	}

}

void ATD10B::read_write( tlm::tlm_generic_payload& trans, sc_time& delay )
{
	tlm::tlm_command cmd = trans.get_command();
	sc_dt::uint64 address = trans.get_address();
	uint8_t* data_ptr = (uint8_t *)trans.get_data_ptr();

	assert(address >= baseAddress);

	if (cmd == tlm::TLM_READ_COMMAND) {
		read(address - baseAddress, data_ptr);
	} else if (cmd == tlm::TLM_WRITE_COMMAND) {
		write(address - baseAddress, data_ptr);
	}

	trans.set_response_status( tlm::TLM_OK_RESPONSE );
}


//=====================================================================
//=                  Client/Service setup methods                     =
//=====================================================================

bool ATD10B::Setup() {

	if (vrh <= vrl) {
		cerr << "ATD10B: Incorrect Values of Vrl and Vrh.\n";
		return false;
	}


	for (int i=0; i<32; i++) {
		busClockRange[i].minBusClock = i+1;
		busClockRange[i].maxBusClock = (i+1)*4;
	}

	double refInc = (vrh - vrl) / REFERENCIAL_SIZE;
	double val = vrl;
	for (int i=0; i < REFERENCIAL_SIZE; i++) {
		val += refInc;
		reference_potentials[i] = val;
	}

	Reset();

	return true;
}

void ATD10B::OnDisconnect() {
}

void ATD10B::Reset() {

	atdctl0_register = 0x0F;
	atdctl1_register = 0x0F;
	atdctl2_register = 0x00;
	atdctl3_register = 0x20;
	atdctl4_register = 0x05;
	atdctl5_register = 0x00;
	atdstat0_register = 0x00;
	atdtest0_register = 0x80;
	atdtest1_register = 0x00;
	atdstat2_register = 0x00;
	atdstat1_register = 0x00;
	atddien0_register = 0x00;
	atddien1_register = 0x00;
	portad0_register = 0xFF;
	portad1_register = 0xFF;

	for (int i=0; i<ATD_SIZE; i++)
		atddrhl_register[ATDDR0H + i] = 0x00;

	setATDClock();

}


//=====================================================================
//=             memory interface methods                              =
//=====================================================================

bool ATD10B::ReadMemory(service_address_t addr, void *buffer, uint32_t size) {

	read(addr-baseAddress, buffer);
	return true;
}

bool ATD10B::WriteMemory(service_address_t addr, const void *buffer, uint32_t size) {

	write(addr-baseAddress, buffer);
	return true;
}


} // end of namespace hcs12x
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim


