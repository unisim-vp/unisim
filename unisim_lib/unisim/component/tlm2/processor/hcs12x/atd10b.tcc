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
#include <unisim/component/cxx/processor/hcs12x/config.hh>
#include "unisim/util/debug/simple_register.hh"

namespace unisim {
namespace component {
namespace tlm2 {
namespace processor {
namespace hcs12x {

using unisim::component::cxx::processor::hcs12x::CONFIG;
using unisim::util::debug::SimpleRegister;

template <uint8_t ATD_SIZE>
ATD10B<ATD_SIZE>::ATD10B(const sc_module_name& name, Object *parent) :
	Object(name, parent),
	sc_module(name),
	Service<Memory<service_address_t> >(name, parent),
	Service<Registers>(name, parent),
	Client<Memory<service_address_t> >(name, parent),
	Client<TrapReporting>(name, parent),

	anx_socket("anx_socket"),
	slave_socket("slave_socket"),

	input_anx_payload_queue("input_anx_payload_queue"),

	memory_export("memory_export", this),
	memory_import("memory_import", this),
	registers_export("registers_export", this),
	trap_reporting_import("trap_reproting_import", this),

	baseAddress(0x0080), // MC9S12XDP512V2 - ATD baseAddress
	param_baseAddress("base-address", this, baseAddress),
	interruptOffset(0xD0), // ATD1 - ATDCTL2 (ASCIE)
	param_interruptOffset("interrupt-offset", this, interruptOffset),
	bus_cycle_time_int(0),
	param_bus_cycle_time_int("bus-cycle-time", this, bus_cycle_time_int),
	debug_enabled(false),
	param_debug_enabled("debug-enabled", this, debug_enabled),

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
	abortSequence(false),
	isTriggerModeRunning(false),
	isATDON(false),

	resultIndex(0)

{

	anx_socket(*this);
	interrupt_request(*this);
	slave_socket.register_b_transport(this, &ATD10B::read_write);
	bus_clock_socket.register_b_transport(this, &ATD10B::UpdateBusClock);

	SC_HAS_PROCESS(ATD10B);

	SC_THREAD(Process);

	// TODO: Scan External Trigger Channels. I think to use ANx Channels !!!
	SC_THREAD(RunTriggerMode);
	sensitive << trigger_event;

}

template <uint8_t ATD_SIZE>
ATD10B<ATD_SIZE>::~ATD10B() {

	// Release registers_registry
	map<string, unisim::util::debug::Register *>::iterator reg_iter;

	for(reg_iter = registers_registry.begin(); reg_iter != registers_registry.end(); reg_iter++)
	{
		if(reg_iter->second)
			delete reg_iter->second;
	}

	registers_registry.clear();

}


// Master methods
template <uint8_t ATD_SIZE>
void ATD10B<ATD_SIZE>::invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range)
{
	// Leave this empty as it is designed for memory mapped buses
}

template <uint8_t ATD_SIZE>
unsigned int ATD10B<ATD_SIZE>::transport_dbg(ATD_Payload<ATD_SIZE>& payload)
{
	// Leave this empty as it is designed for memory mapped buses
	return 0;
}

template <uint8_t ATD_SIZE>
void ATD10B<ATD_SIZE>::b_transport(ATD_Payload<ATD_SIZE>& payload, sc_core::sc_time& t) {
	input_anx_payload_queue.notify(payload, t);
}

template <uint8_t ATD_SIZE>
bool ATD10B<ATD_SIZE>::get_direct_mem_ptr(ATD_Payload<ATD_SIZE>& payload, tlm_dmi&  dmi_data) {
	return false;
}

/**
 * Name : nb_transport_fw()
 *
 * Role :
 * 1/ Sample and Hold machine accepts analog signals from external world and stores them as capacitor charge on a storage node.
 */
template <uint8_t ATD_SIZE>
tlm_sync_enum ATD10B<ATD_SIZE>::nb_transport_fw(ATD_Payload<ATD_SIZE>& payload, tlm_phase& phase, sc_core::sc_time& t)
{
	switch(phase)
	{
		case BEGIN_REQ:
			// accepts analog signals modeled by payload
			phase = END_REQ; // update the phase
			input_anx_payload_queue.notify(payload, t); // queue the payload and the associative time

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

template <uint8_t ATD_SIZE>
void ATD10B<ATD_SIZE>::Process()
{
	while(1)
	{
		quantumkeeper.sync();
		wait(input_anx_payload_queue.get_event());

		InputANx(analog_signal);

		// TODO: see in details how to use quantumkeeper in the case of ATD

		quantumkeeper.inc(bus_cycle_time); // Processing the input takes one cycle
		if(quantumkeeper.need_sync()) quantumkeeper.sync(); // synchronize if needed

	}
}

/**
 * Store external analog signals as capacitor charge
 *  1/ Is Multi-Channel or Single-Channel scan
 *  2/ The analog input multiplexer connects one of the external analog input channels to the sample and hold machine
 */

template <uint8_t ATD_SIZE>
void ATD10B<ATD_SIZE>::InputANx(double anValue[ATD_SIZE])
{
	ATD_Payload<ATD_SIZE> *last_payload = NULL;
	ATD_Payload<ATD_SIZE> *payload = NULL;

	do
	{
		last_payload = payload;
		payload = input_anx_payload_queue.get_next_transaction();

		if (debug_enabled && payload) {
			cout << name() << ":: Receive " << payload->serialize() << " - " << sc_time_stamp() << endl;
		}

	} while(payload);

	payload = last_payload;

	if (debug_enabled) {
		cout << name() << ":: Last Receive " << payload->serialize() << " - " << sc_time_stamp() << endl;
	}

	if ((atdctl2_register & 0x80) != 0) // is ATD power ON (enabled)
	{
		for (int i=0; i<ATD_SIZE; i++) {
			anValue[i] = payload->anPort[i];
		}

		RunScanMode();

	} else {
		cerr << "Warning: " << name() << " => The ATD is OFF. You have to set ATDCTL2::ADPU bit before.\n";
	}

}

template <uint8_t ATD_SIZE>
void ATD10B<ATD_SIZE>::RunTriggerMode() {

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

template <uint8_t ATD_SIZE>
void ATD10B<ATD_SIZE>::RunScanMode()
{

	// - check ATDCTL0::wrap bits to identify the channel to wrap around
	uint8_t wrapArroundChannel = atdctl0_register & 0x0F;
	if (wrapArroundChannel == 0) {
		if (debug_enabled) {
			cerr << "Warning: " << name() << " => WrapArroundChannel=0 is a reserved value. The wrap channel is assumed " << ATD_SIZE-1 << ".\n";
		}

		wrapArroundChannel = ATD_SIZE-1;
	}

	uint8_t sequenceLength = 1;
	// - check ATDCTL3 to determine the sequence length and storage mode of the result
	sequenceLength = (atdctl3_register & 0x78) >> 3; // get S8C S4C S2C S1C
	if (sequenceLength == 0) {
		sequenceLength = 16;
	}

	uint8_t currentChannel = atdctl5_register & 0x0F; // get CD/CC/CB/CA;
	conversionStop = false;

	/**
	 *  check is FIFO mode
	 *  FIFO = 0 : Conversion results are placed in the corresponding result register up to the selected sequence length
	 *  FIFO = 1 : Conversion results are placed in consecutive result registers (wrap around at end)
	 */
	bool isFIFO = ((atdctl3_register & 0x04) != 0);

	do
	{

		// Store the result of conversion
		uint8_t sequenceIndex = 0;
		abortSequence = false;

		while ((sequenceIndex < sequenceLength)  && !abortSequence ) {

			// set the conversion counter ATDSTAT0::CC (result register which will receive the current conversion)
			atdstat0_register = (atdstat0_register & 0xF0) | (resultIndex & 0x0F);

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
					default:
						if (debug_enabled) {
							cerr << "Warning: " << name() << " => Reserved value of CD/CC/CB/CA.\n";
						}
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

			if (!isFIFO) {
				resultIndex = currentChannel;
			}

			atddrhl_register[resultIndex] = digitalToken;
			// set the ATDSTAT1 & ATDSTAT2 CCFx flags
			if (resultIndex < 8) {
				atdstat1_register = atdstat1_register | (0x01 < resultIndex);
			} else {
				atdstat2_register = atdstat2_register | (0x01 < (resultIndex-8));
			}

			if (isFIFO) {
				resultIndex = (resultIndex + 1) % ATD_SIZE;
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

			sequenceIndex++;
			wait(first_phase_clock + second_phase_clock);
		}

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

// Master methods
template <uint8_t ATD_SIZE>
tlm_sync_enum ATD10B<ATD_SIZE>::nb_transport_bw( XINT_Payload& payload, tlm_phase& phase, sc_core::sc_time& t)
{
	if(phase == BEGIN_RESP)
	{
		payload.release();
		return TLM_COMPLETED;
	}
	return TLM_ACCEPTED;
}


template <uint8_t ATD_SIZE>
void ATD10B<ATD_SIZE>::assertInterrupt() {
	// assert ATD_SequenceComplete_Interrupt

	tlm_phase phase = BEGIN_REQ;
	XINT_Payload *payload = xint_payload_fabric.allocate();

	payload->interrupt_offset = interruptOffset;

	sc_time local_time = quantumkeeper.get_local_time();

	tlm_sync_enum ret = interrupt_request->nb_transport_fw(*payload, phase, local_time);

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

template <uint8_t ATD_SIZE>
void ATD10B<ATD_SIZE>::sequenceComplete() {

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
		assertInterrupt();
	}

}

template <uint8_t ATD_SIZE>
void ATD10B<ATD_SIZE>::abortConversion() {

	conversionStop = true;
	abortSequence = true;

	// Clear ATDSTAT0::ETORF and ATDSTAT0::CC
	atdstat0_register = atdstat0_register & 0xD0;

	resultIndex = 0;

}

template <uint8_t ATD_SIZE>
void ATD10B<ATD_SIZE>::abortAndStartNewConversion() {

	conversionStop = false;
	abortSequence = true;

	// Clear ATDSTAT0,1,2 registers
	atdstat0_register = 0;
	atdstat1_register = 0;
	atdstat2_register = 0;

	resultIndex = 0;

//	/**
//	 *  if external trigger is enabled (ETRIGE=1) an initial write to ATDCTL5 is required to allow
//	 *  starting of conversion sequence which will then occur on each trigger event.
//	 */
//	if ((atdctl2_register & 0x04) != 0) {
//		if (!isTriggerModeRunning) {
//			isTriggerModeRunning = true;
//			RunTriggerMode();
//		}
//	} else {
//		isTriggerModeRunning = false;
//	}
}

template <uint8_t ATD_SIZE>
void ATD10B<ATD_SIZE>::setATDClock() {

	uint8_t smpMask = 0x60;
	uint8_t smpValue = (atdctl4_register & smpMask) >> 5;
	uint8_t prsMask = 0x1F;
	uint8_t prsValue = atdctl4_register & prsMask;

	atd_clock = bus_cycle_time / (prsValue + 1) * 0.5;
	first_phase_clock = atd_clock * 2;
	second_phase_clock = atd_clock * 2 * (1 << smpValue);
}

/*	===========================
 * ATD Reference
 *  - InputANx : input signal. Values are between [Vrl = 0 Volts , Vrh = 5.12 Volts]
 *  - Output Code: depend on
 *       ATDCTL4::SRES (resolution 8/10),
 *       ATDCTL5::DJM (justification left/right),
 *       ATDCTL5::DSGN (data sign)
 */
template <uint8_t ATD_SIZE>
uint16_t ATD10B<ATD_SIZE>::getDigitalToken(double analogVoltage) {

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
template <uint8_t ATD_SIZE>
bool ATD10B<ATD_SIZE>::read(uint8_t offset, void *buffer) {

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
			int isETRIGCHx = atdctl1_register & 0x0F;
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
			int isETRIGCHx = atdctl1_register & 0x0F;
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

		default: if ((offset >= ATDDR0H) && (offset <= ATDDR15L)) {
			*((uint16_t *) buffer) = atddrhl_register[(offset-ATDDR0H)/2] & 0xFFC0;
			uint8_t index = (offset - ATDDR0H)/2;
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
			return false;
		}
	}

	return true;
}

template <uint8_t ATD_SIZE>
bool ATD10B<ATD_SIZE>::write(uint8_t offset, const void *buffer) {

	switch (offset) {
		case ATDCTL0: {
			atdctl0_register = *((uint8_t *) buffer) & (ATD_SIZE - 1);
			abortConversion();
		} break;
		case ATDCTL1: {
			atdctl1_register = *((uint8_t *) buffer) & (0x80 | (ATD_SIZE - 1));
			abortConversion();
		} break;
		case ATDCTL2: {
			atdctl2_register = (*((uint8_t *) buffer) & 0xFE) | (atdctl2_register & 0x01);
			if ((atdctl2_register & 0x04) != 0) {
				if (debug_enabled) {
					cerr << "Warning: " << name() << " => Trigger mode not support Yet. \n";
				}
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
			atdctl5_register = *((uint8_t *) buffer) & (0xF0 | (ATD_SIZE - 1));
			abortAndStartNewConversion();

		} break;
		case ATDSTAT0: {
			uint8_t highMask = (*((uint8_t *) buffer) & 0xB0);
			if ((highMask & 0x80) != 0) {
				atdstat0_register = atdstat0_register & (0x70 | (ATD_SIZE - 1));
			}
			if ((highMask & 0x20) != 0) {
				atdstat0_register = atdstat0_register & (0xD0 | (ATD_SIZE - 1));
			}
			if ((highMask & 0x10) != 0) {
				atdstat0_register = atdstat0_register & (0xE0 | (ATD_SIZE - 1));
			}

		} break;
		case UNIMPL0007: break;
		case ATDTEST0:
			// ATDTEST0 is intended for factory test purposes only.

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

		default: if ((offset >= ATDDR0H) && (offset <= ATDDR15L)) {
			/* write has no effect */
		} else {
			return false;
		}
	}

	return true;
}

template <uint8_t ATD_SIZE>
void ATD10B<ATD_SIZE>::read_write( tlm::tlm_generic_payload& trans, sc_time& delay )
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

template <uint8_t ATD_SIZE>
void ATD10B<ATD_SIZE>::ComputeInternalTime() {

	bus_cycle_time = sc_time((double)bus_cycle_time_int, SC_PS);

}

template <uint8_t ATD_SIZE>
void ATD10B<ATD_SIZE>::UpdateBusClock(tlm::tlm_generic_payload& trans, sc_time& delay) {

	sc_dt::uint64*   external_bus_clock = (sc_dt::uint64*) trans.get_data_ptr();
    trans.set_response_status( tlm::TLM_OK_RESPONSE );

	bus_cycle_time_int = *external_bus_clock;

	ComputeInternalTime();
}


//=====================================================================
//=                  Client/Service setup methods                     =
//=====================================================================
template <uint8_t ATD_SIZE>
bool ATD10B<ATD_SIZE>::Setup() {

	char buf[20];

	sprintf(buf, "%s.ATDCTL0",name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &atdctl0_register);

	sprintf(buf, "%s.ATDCTL1",name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &atdctl1_register);

	sprintf(buf, "%s.ATDCTL2",name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &atdctl2_register);

	sprintf(buf, "%s.ATDCTL3",name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &atdctl3_register);

	sprintf(buf, "%s.ATDCTL4",name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &atdctl4_register);

	sprintf(buf, "%s.ATDCTL5",name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &atdctl5_register);

	sprintf(buf, "%s.ATDSTAT0",name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &atdstat0_register);

	sprintf(buf, "%s.ATDTEST0",name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &atdtest0_register);

	sprintf(buf, "%s.ATDTEST1",name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &atdtest1_register);

	sprintf(buf, "%s.ATDSTAT2",name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &atdstat2_register);

	sprintf(buf, "%s.ATDSTAT1",name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &atdstat1_register);

	sprintf(buf, "%s.ATDDIEN0",name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &atddien0_register);

	sprintf(buf, "%s.ATDDIEN1",name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &atddien1_register);

	sprintf(buf, "%s.PORTAD0",name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &portad0_register);

	sprintf(buf, "%s.PORTAD1",name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &portad1_register);

	for (uint8_t i=0; i < ATD_SIZE; i++) {

		sprintf(buf, "%s.ATDDR%d", name(), i);

		registers_registry[buf] = new SimpleRegister<uint16_t>(buf, &atddrhl_register[i]);

	}

	if (vrh <= vrl) {
		if (debug_enabled) {
			cerr << "Warning: " << name() << " : Wrong Values of Vrl and Vrh.\n";
		}

		return false;
	}

	Reset();

	ComputeInternalTime();

	// the index 'i' model BusClock in MHz
	for (int i=0; i<32; i++) {
		busClockRange[i].maxBusClock = 1e6/(i+1); // busClock is modeled in PS
		busClockRange[i].minBusClock = 1e6/((i+1)*4);
	}

	return true;
}

template <uint8_t ATD_SIZE>
Register* ATD10B<ATD_SIZE>::GetRegister(const char *name)
{
	if(registers_registry.find(string(name)) != registers_registry.end())
		return registers_registry[string(name)];
	else
		return NULL;

}

template <uint8_t ATD_SIZE>
void ATD10B<ATD_SIZE>::OnDisconnect() {
}

template <uint8_t ATD_SIZE>
void ATD10B<ATD_SIZE>::Reset() {

	atdctl0_register = ATD_SIZE - 1;
	atdctl1_register = ATD_SIZE - 1;
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

	for (int i=0; i < ATD_SIZE; i++)
		atddrhl_register[ATDDR0H + i] = 0x00;

	setATDClock();

}


//=====================================================================
//=             memory interface methods                              =
//=====================================================================
template <uint8_t ATD_SIZE>
bool ATD10B<ATD_SIZE>::ReadMemory(service_address_t addr, void *buffer, uint32_t size) {

	service_address_t offset = addr-baseAddress;

	if (offset <= ATDDR15L) {

		switch (offset) {
			case ATDCTL0: *((uint8_t *) buffer) = atdctl0_register; break;
			case ATDCTL1: *((uint8_t *) buffer) = atdctl1_register; break;
			case ATDCTL2: *((uint8_t *) buffer) = atdctl2_register; break;
			case ATDCTL3: *((uint8_t *) buffer) = atdctl3_register; break;
			case ATDCTL4: *((uint8_t *) buffer) = atdctl4_register; break;
			case ATDCTL5: *((uint8_t *) buffer) = atdctl5_register; break;
			case ATDSTAT0: *((uint8_t *) buffer) = atdstat0_register; break;
			case UNIMPL0007: *((uint8_t *) buffer) = 0x00; break;
			case ATDTEST0: *((uint8_t *) buffer) = atdtest0_register; break;
			case ATDTEST1: *((uint8_t *) buffer) = atdtest1_register; break;
			case ATDSTAT2: *((uint8_t *) buffer) = atdstat2_register; break;
			case ATDSTAT1: *((uint8_t *) buffer) = atdstat1_register; break;
			case ATDDIEN0: {
				if (size == sizeof(uint8_t)) {
					*((uint8_t *) buffer) = atddien0_register;
				} else {
					uint16_t val = ((uint16_t) atddien0_register << 8) & atddien1_register;
					*((uint16_t *) buffer) = val;
				}
			} break;

			case ATDDIEN1: *((uint8_t *) buffer) = atddien1_register; break;
			case PORTAD0: {
				if (size == sizeof(uint8_t)) {
					*((uint8_t *) buffer) = portad0_register;
				} else {
					uint16_t val = ((uint16_t) portad0_register << 8) & portad1_register;
					*((uint16_t *) buffer) = val;
				}


			} break;
			case PORTAD1: *((uint8_t *) buffer) = portad1_register; break;

			default: if ((offset >= ATDDR0H) && (offset <= ATDDR15L)) {
				if (size == sizeof(uint8_t)) {

					if (((offset-ATDDR0H) % 2) == 0) {
						*((uint8_t *) buffer) = (uint8_t) ((atddrhl_register[(offset-ATDDR0H)/2] & 0xFF00) >> 8);
					} else {
						*((uint8_t *) buffer) = (uint8_t) (atddrhl_register[(offset-ATDDR0H)/2] & 0x00FF);
					}

				} else {
					*((uint16_t *) buffer) = atddrhl_register[(offset-ATDDR0H)/2];
				}

			} else {
				return false;
			}
		}

		return true;

	}

	return false;
}

template <uint8_t ATD_SIZE>
bool ATD10B<ATD_SIZE>::WriteMemory(service_address_t addr, const void *buffer, uint32_t size) {

	service_address_t offset = addr-baseAddress;

	if (offset <= ATDDR15L) {
		return write(offset, buffer);
	}

	return false;

}


} // end of namespace hcs12x
} // end of namespace processor
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim


