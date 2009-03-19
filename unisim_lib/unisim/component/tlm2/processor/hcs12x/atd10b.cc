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
	param_vrh("vrh", this, vrh),
	vil(1.75),
	vih(3.25),
	param_vih("vih", this, vih),
	param_vil("vil", this, vil),
	hasExternalTrigger(false),
	param_hasExternalTrigger("Has-External-Trigger", this, hasExternalTrigger),
	conversionStop(false),
	abortSequence(false)

{

	slave_socket.register_b_transport(this, &ATD10B::read_write);

	SC_HAS_PROCESS(ATD10B);

	SC_THREAD(RunScanMode);
	sensitive << scan_event;

	// TODO: Scan External Trigger Channels. I think to use ANx Channels !!!
	SC_THREAD(RunTriggerMode);
	sensitive << trigger_event;

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

/**
 * Store external analog signals as capacitor charge
 *  1/ Is Multi-Channel or Single-Channel scan
 *  2/ The analog input multiplexer connects one of the external analog input channels to the sample and hold machine
 */

void ATD10B::Input(ATD_Payload& payload, double anValue[ATD_SIZE])
{

	if ((atdctl2_register & 0x80) != 0) // is ATD power ON (enabled)
	{
		for (int i=0; i<ATD_SIZE; i++) {
			anValue[i] = payload.anPort[i];
		}

		scan_event.notify();

	} else {
		cerr << "Warning: ATD10B16C ==> The ATD is OFF. You have to set ATDCTL2::ADPU bit before.\n";
	}

	payload.release();

}

void ATD10B::RunTriggerMode() {

	/**
	 *  TODO: Not Implemented at all.
	 *        Here after is just a try to understand/extract all relevant informations to External-Trigger
	 */

	/**
	 * ATDSTAT0::ETORF External Trigger Overrun Flag
	 *   While in edge trigger mode (ATDCTL2::ETRIGLE=0),
	 *     if additional active edges are detected while a conversion sequence is in process
	 *     then overrun flag is set.
	 */

	while (1)
	{
		wait();

		// Check External Trigger
		/*
		 * if ATDCTL2::ETRIGE is set
		 * then
		 *   - enable the external trigger
		 *   - set ExternalTrigger configuration using ATDCTL2::ETRIGLE and ATDCTL2::ETRIGP
		 * else
		 *   disable external trigger
		 */

/*  NOT usable code as it is.
 *
		// isExternalTriggerEnabled ? get ATDCTL2::ETRIGE
		if ((atdctl2_register & 0x04) != 0) {

			// - check ATDCTL1 to select External Trigger Source
			bool isExternalTriggerSelect = ((atdctl1_register & 0x80) != 0);
			uint8_t currentChannel = atdctl1_register & 0x0F;

			if ((isExternalTriggerSelect) && hasExternalTrigger) {
				// set wait event to External_Trigger Channel (currentChannel)
			} else {
				// set wait event to ANx Channel (currentChannel)
			}

		}
 */
	}

}

void ATD10B::RunScanMode()
{
	uint8_t resultIndex = 0;

	while(1)
	{

		wait();

		// - check ATDCTL0::wrap bits to identify the channel to wrap around
		uint8_t wrapArroundChannel = atdctl0_register & 0x0F;
		if (wrapArroundChannel == 0) {
			cerr << "Warning: ATD10B => WrapArroundChannel=0 is a reserved value. The wrap channel is assumed " << ATD_SIZE-1 << ".\n";
			wrapArroundChannel = ATD_SIZE-1;
		}

		uint8_t sequenceLength = 1;
		// - check ATDCTL3 to determine the sequence length and storage mode of the result
		sequenceLength = atdctl3_register & 0x78; // get S8C S4C S2C S1C
		if (sequenceLength == 0) {
			sequenceLength = 16;
		}

		if (sequenceLength > ATD_SIZE) {
			cerr << "Warning: ATD10B => sequence length is higher than the ATD size.\n";
			sequenceLength = ATD_SIZE;
		}


		uint8_t currentChannel = atdctl5_register & 0x0F; // get CD/CC/CB/CA;
		conversionStop = false;

		do
		{

			/**
			 *  check is FIFO mode
			 *  if (fifo == 0) reset the result index at the start of a sequence
			 *  else the result index will wrap when it rich the last result register
			 */
			if ((atdctl3_register & 0x04) == 0) {
				resultIndex = 0;
			}

			// Store the result of conversion
			uint8_t sequenceIndex = 0;
			abortSequence = false;

			while ((sequenceIndex < sequenceLength) /* && !abortSequence */ ) {

				// set the conversion counter ATDSTAT0::CC (result register which will receive the current conversion)
				atdstat0_register = (atdstat0_register & 0xF0) | resultIndex;

				/**
				 * - scan current channel,
				 * - Find referential potential,
				 * - check sign and justification,
				 * - and store in atddrhl_register[i] register
				 */
				double anSignal;
				const uint8_t scMask = 0x01;
				// is Special Channel Select enabled ?
				if ((atdtest1_register & scMask) != 0) {
					switch (currentChannel) {
					case 4: anSignal = vrh; break;
					case 5: anSignal = vrl; break;
					case 6: anSignal = (vrh + vrl)/2; break;
					default: cerr << "Warning: ATD10B => Reserved value of CD/CC/CB/CA.\n";
					}
				} else {
					anSignal = analog_signal[currentChannel];
				}
				uint16_t digitalToken = getDigitalToken(anSignal);

				uint16_t atdstat21 = ((uint16_t) atdstat2_register << 8) | atdstat1_register;
				uint16_t ccfMask = 0x01;
				// is Result registers out of sync ?
				if ((atdstat21  & (ccfMask << resultIndex)) != 0) {

					// Set the ATDSTAT0::FIFOR FIFO Over Run Flag
					atdstat0_register = atdstat0_register | 0x10;
				}

				atddrhl_register[resultIndex] = digitalToken;
				// set the ATDSTAT1 & ATDSTAT2 CCFx flags
				if (resultIndex < 8) {
					atdstat1_register = atdstat1_register | (0x01 < resultIndex);
				} else {
					atdstat2_register = atdstat2_register | (0x01 < (resultIndex-8));
				}

				// - if Multi-channel then select next channel else keep the same channel
				if ((atdctl5_register & 0x10) != 0) {
					/**
					 * - identify nextChannel
					 * - take in account the wrap channel
					 */
					if (currentChannel == wrapArroundChannel) {
						currentChannel = 0;
					} else {
						currentChannel = (currentChannel+1) % ATD_SIZE;
					}
				}

				resultIndex = (resultIndex + 1) % ATD_SIZE;
				sequenceIndex++;
			}

			wait((first_phase_clock + second_phase_clock) * sequenceLength);

			// The abordSequence flag can be set during the wait state by writing to any control register
			if (!abortSequence) {
				sequenceComplete();
			}

			/* **
			 *  - The conversionStop flag can be set during the wait state by writing to any control register [1-4]
			 *  - writing to control register 5 abort only the current sequence scan but restart a new one
			 */
			// identify scan mode SingleConversion/ContinuousConversion
		} while ((((atdctl5_register & 0x20) != 0) && ((atdctl2_register & 0x04) == 0)) && !conversionStop);

	}

}

void ATD10B::sequenceComplete() {

	conversionStop = true;

	// Set the SCF (Sequence Complete Flag)
	atdstat0_register = atdstat0_register | 0x80;

	/**
	 *  set ATDCTL2::ASCIF bit
	 */
	atdctl2_register = atdctl2_register | 0x01;
	/**
	 *  if ATDCTL2::ASCIE bit is set then assert ATD_SequenceComplete_Interrupt
	 */
	if ((atdctl2_register & 0x02) != 0) {
		// TODO: assert ATD_SequenceComplete_Interrupt (channel ID 0x68)
	}
}

void ATD10B::abortConversion() {

	conversionStop = true;
	abortSequence = true;

	// Clear ATDSTAT0::ETORF and ATDSTAT0::CC
	atdstat0_register = atdstat0_register & 0xD0;

}

void ATD10B::abortAndStartNewConversion() {

	conversionStop = false;
	abortSequence = true;

	// Clear ATDSTAT0,1,2 registers
	atdstat0_register = 0;
	atdstat1_register = 0;
	atdstat2_register = 0;
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
	second_phase_clock = first_phase_clock * (1 << smpValue);
}

/*	===========================
 * ATD Reference
 *  - Input : input signal. Values are between [Vrl = 0 Volts , Vrh = 5.12 Volts]
 *  - Output Code: depend on
 *       ATDCTL4::SRES (resolution 8/10),
 *       ATDCTL5::DJM (justification left/right),
 *       ATDCTL5::DSGN (data sign)
 */

uint16_t ATD10B::getDigitalToken(double analogVoltage) {

	uint16_t digitalToken = 0;

	int resolution = 10; // default resolution is 10-bit

	// - Take in account The A/D Resolution Select ATDCTL4::SRES
	// is 8-bit resolution or 10-bit resolution
	if ((atdctl4_register & 0x80) != 0) {
		resolution = 8;
	}

	// - take in account The A/D Justification ATDCTL5::DJM
	bool isRightJustified = ((atdctl5_register & 0x80) != 0);

	// - take in account The A/D sign ATDCTL5::DSGN
	bool isDataSigned = ((atdctl5_register & 0x40) != 0);

	/*
	 *

			AnalogVoltage = (delta * DigitalToken) + Vrl

			delta = (Vrh – Vrl) / (2^resolution – 1)

			ZeroAnalogVoltage = (Vrh - Vrl)/2 + Vrl
		=>
			Unsigned
			 if ((AnalogVoltage < ZeroAnalogVoltage) ou (AnalogVoltage = Vrh))
				DigitalToken = ARRONDI( (AnalogVoltage – Vrl) / delta )
			 else
				  DigitalToken = ARRONDI( (AnalogVoltage – Vrl + delta) / delta )

			Signed  (setVrl(ZeroAnalogVoltage)
			  if (AnalogVoltage = Vrh)
				 DigitalToken = ARRONDI( (AnalogVoltage – ZeroAnalogVoltage - delta) / delta )
			  else
				 DigitalToken = ARRONDI( (AnalogVoltage – ZeroAnalogVoltage) / delta )

	 *
	 */

	double delta = (vrh - vrl) / ((1 << resolution) - 1);
	double zeroAnalogVoltage = (vrh - vrl)/2 + vrl;

	if (isDataSigned) {
		if (analogVoltage == vrh) {
			digitalToken =  0xFFFF & llround((analogVoltage - zeroAnalogVoltage - delta) / delta);
		} else {
			digitalToken = 0xFFFF & llround((analogVoltage - zeroAnalogVoltage) / delta);
		}
	} else {
		if ((analogVoltage < zeroAnalogVoltage) || (analogVoltage == vrh)) {
			digitalToken = 0xFFFF & llround((analogVoltage - vrl) / delta);
		} else {
			digitalToken = 0xFFFF & llround((analogVoltage - vrl + delta) / delta);
		}
	}

	if (!isRightJustified) {
		digitalToken = digitalToken << (16 - resolution);
	}

	return digitalToken;
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
		case PORTAD0: {
			bool isETRIGE = ((atdctl2_register & 0x04) != 0);
			bool isETRIGCHx = atdctl1_register & 0x0F;
			bool isETRIGSEL = ((atdctl1_register & 0x80) != 0);

			portad0_register = 0;
			uint8_t ienMask = 0x01;
			for (int i=8; i<ATD_SIZE; i++) {
				if (((atddien0_register & ienMask) != 0) || (isETRIGE && (isETRIGCHx == i) && !isETRIGSEL)) {
					if (analog_signal[i] >= vih) {
						portad0_register = portad0_register | ienMask;
					}
				} else {
					portad0_register = portad0_register | ienMask;
				}

				ienMask = ienMask << 1;
			}

			*((uint8_t *) buffer) = portad0_register;
		} break;
		case PORTAD1: {
			bool isETRIGE = ((atdctl2_register & 0x04) != 0);
			bool isETRIGCHx = atdctl1_register & 0x0F;
			bool isETRIGSEL = ((atdctl1_register & 0x80) != 0);

			portad1_register = 0;
			uint8_t ienMask = 0x01;
			for (int i=0; i<8; i++) {
				if (((atddien1_register & ienMask) != 0) || (isETRIGE && (isETRIGCHx == i) && !isETRIGSEL)) {
					if (analog_signal[i] >= vih) {
						portad1_register = portad1_register | ienMask;
					}
				} else {
					portad1_register = portad1_register | ienMask;
				}

				ienMask = ienMask << 1;
			}

			*((uint8_t *) buffer) = portad1_register;
		} break;

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

			// check ATDCTL2::AFFC Fast Clear bit
			if ((atdctl2_register & 0x40) != 0) {
				atdstat0_register = atdstat0_register & 0x7F;
			}
		} else {
			cerr << "ERROR: ATD10B => Wrong offset.\n";
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
			if ((atdctl2_register & 0x04) != 0) {
				cerr << "Warning: ATD10B => Trigger mode not support Yet. \n";
			}
			abortConversion();
		} break;
		case ATDCTL3: {
			atdctl3_register = *((uint8_t *) buffer) & 0x7F;
			abortConversion();
		} break;
		case ATDCTL4: {
			atdctl4_register = *((uint8_t *) buffer);
			abortConversion();

			setATDClock();

		} break;
		case ATDCTL5: {
			atdctl5_register = *((uint8_t *) buffer);
			abortAndStartNewConversion();

		} break;
		case ATDSTAT0: {
			uint8_t highMask = (*((uint8_t *) buffer) & 0xB0);
			if ((highMask & 0x80) != 0) {
				atdstat0_register = atdstat0_register & 0x7F;
			}
			if ((highMask & 0x20) != 0) {
				atdstat0_register = atdstat0_register & 0xDF;
			}
			if ((highMask & 0x10) != 0) {
				atdstat0_register = atdstat0_register & 0xEF;
			}

		} break;
		case UNIMPL0007: break;
		case ATDTEST0:
			// atdtest0_register = *((uint8_t *) buffer);
			cerr << "Warning: ATD10B => Not implemented yet. Write to ATDTEST0 in special modes can alter functionality.\n";
			break;
		case ATDTEST1: {
			const uint8_t scMask = 0x01;
			atdtest1_register = *((uint8_t *) buffer) & scMask;
		} break;
		case ATDSTAT2: {
			/* write has no effect */
		} break;
		case ATDSTAT1: {
			/* write has no effect */
		} break;
		case ATDDIEN0: {
			atddien0_register = *((uint8_t *) buffer);
		} break;
		case ATDDIEN1: {
			atddien1_register = *((uint8_t *) buffer);
		} break;
		case PORTAD0: {
			/* write has no effect */
		} break;
		case PORTAD1: {
			/* write has no effect */
		} break;

		default: if ((offset >= ATDDR0H) && (offset < ATDDR0H+ATD_SIZE)) {
			/* write has no effect */
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


