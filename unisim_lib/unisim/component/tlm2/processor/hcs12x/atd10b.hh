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

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_HCS12X_ATD10B_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_HCS12X_ATD10B_HH__

#include <systemc.h>

#include <inttypes.h>
#include <iostream>

#include <tlm.h>
#include <tlm_utils/tlm_quantumkeeper.h>
#include <tlm_utils/peq_with_get.h>
#include "tlm_utils/simple_target_socket.h"

#include "unisim/kernel/service/service.hh"
#include "unisim/service/interfaces/memory.hh"
#include "unisim/kernel/tlm2/tlm.hh"

#include <unisim/component/cxx/processor/hcs12x/config.hh>
#include <unisim/component/cxx/processor/hcs12x/types.hh>

#include <unisim/component/tlm2/processor/hcs12x/tlm_types.hh>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace hcs12x {

using namespace std;
using namespace tlm;
using namespace tlm_utils;

using unisim::kernel::service::Object;
using unisim::kernel::service::Client;
using unisim::kernel::service::Service;
using unisim::kernel::service::ServiceExport;
using unisim::kernel::service::ServiceImport;
using unisim::kernel::service::Parameter;

using unisim::service::interfaces::Memory;

using unisim::kernel::tlm2::PayloadFabric;

using unisim::component::tlm2::processor::hcs12x::UNISIM_ATD_ProtocolTypes;
using unisim::component::tlm2::processor::hcs12x::ATD_Payload;


template <uint8_t ATD_SIZE>
class ATD10B :
	public sc_module,
	virtual public tlm_fw_transport_if<UNISIM_ATD_ProtocolTypes<ATD_SIZE> >,
	public Service<Memory<service_address_t> >,
	public Client<Memory<service_address_t> >
{
public:

		//=========================================================
		//=                REGISTERS OFFSETS                      =
		//=========================================================

		enum REGS_OFFSETS {ATDCTL0, ATDCTL1, ATDCTL2, ATDCTL3, ATDCTL4, ATDCTL5,
			ATDSTAT0, UNIMPL0007, ATDTEST0, ATDTEST1, ATDSTAT2, ATDSTAT1,
			ATDDIEN0, ATDDIEN1, PORTAD0, PORTAD1, ATDDR0H};

		// interface with bus
		tlm_utils::simple_target_socket<ATD10B> slave_socket;

		ServiceExport<Memory<service_address_t> > memory_export;
		ServiceImport<Memory<service_address_t> > memory_import;

		ATD10B(const sc_module_name& name, Object *parent=0);
		~ATD10B();

		void RunScanMode();
		void RunTriggerMode();

	    //================================================================
	    //=                    tlm2 Interface                            =
	    //================================================================
	    virtual void invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range);
		virtual unsigned int transport_dbg(ATD_Payload<ATD_SIZE>& payload);
		virtual tlm_sync_enum nb_transport_fw(ATD_Payload<ATD_SIZE>& payload, tlm_phase& phase, sc_core::sc_time& t);

		void read_write( tlm::tlm_generic_payload& trans, sc_time& delay );

		//=====================================================================
		//=                  Client/Service setup methods                     =
		//=====================================================================

		virtual bool Setup();
		virtual void OnDisconnect();
		virtual void Reset();

		//=====================================================================
		//=             memory interface methods                              =
		//=====================================================================

		virtual bool ReadMemory(service_address_t addr, void *buffer, uint32_t size);
		virtual bool WriteMemory(service_address_t addr, const void *buffer, uint32_t size);

		//=====================================================================
		//=             registers setters and getters                         =
		//=====================================================================
	    void read(uint8_t offset, void *buffer);
	    void write(uint8_t offset, const void *buffer);

protected:

private:

	PayloadFabric<ATD_Payload<ATD_SIZE> > payload_fabric;

	clock_t	bus_cycle_time_int;
	Parameter<clock_t>	param_bus_cycle_time_int;
	sc_time		bus_cycle_time;

	sc_time		atd_clock;
	sc_time		first_phase_clock;
	sc_time		second_phase_clock;

	sc_event scan_event;
	sc_event trigger_event;

	address_t	baseAddress;
	Parameter<address_t>   param_baseAddress;

	uint8_t interruptOffset;
	Parameter<uint8_t> param_interruptOffset;

	// A/D reference potentials
	double vrl, vrh;
	Parameter <double> param_vrl;
	Parameter <double> param_vrh;

	/**
	 * Vih and Vil are logical levels
	 *  - Vih minimum voltage to model logical "1" the default is 3.25 V (min)
	 *  - vil maximum voltage to model logical "0" the default is 1.75 V (max)
	 */
	double vih, vil;
	Parameter<double> param_vih;
	Parameter<double> param_vil;

	// External Trigger Parameter
	bool			hasExternalTrigger;
	Parameter<bool>	param_hasExternalTrigger;

	bool conversionStop;
	bool abortSequence;

	void Input(ATD_Payload<ATD_SIZE>& payload, double anValue[ATD_SIZE]);
	void abortConversion();
	void abortAndStartNewConversion();
	void sequenceComplete();
	void setExternalTriggerMode();
	void setATDClock();

	/**
	 * Theory
	 *    AnalogVoltage = (Delta * DigitalToken) + VRL
	 *      where
	 *        Delta = (VRH-VRL)/(pow(2, resolution) - 1)
	 *
	 * In the Adapt9S12XD VRL and VRH are set to GND and VDD respectively.
	 * In the MC9S12XDP512, each analog channel is capable of producing 10 bit digital tokens.
	 * Therefore the smallest digital token ('$000') would be equivalent to 0 volts (GND),
	 * the largest digital token ('$3FF') would be equal to 5 volts (VDD)
	 * and an increase by one in the digital token would represent an increase by 5mV in the analog voltage.
	 *
	 * ===========================
	 * ATD Reference
	 *  - Input : input signal. Values are between [Vrl = 0 Volts , Vrh = 5.12 Volts]
	 *  - Output Code: depend on
	 *       ATDCTL4::SRES (resolution 8/10),
	 *       ATDCTL5::DJM (justification left/right),
	 *       ATDCTL5::DSGN (data sign)
	 */
	uint16_t getDigitalToken(double inputVoltage);

	//==============================================
	//=              ATD Registers Set             =
	//==============================================

	uint8_t atdctl0_register, atdctl1_register, atdctl2_register, atdctl3_register, atdctl4_register, atdctl5_register;
	uint8_t atdstat0_register, atdtest0_register, atdtest1_register, atdstat2_register, atdstat1_register;
	uint8_t atddien0_register, atddien1_register, portad0_register, portad1_register;
	uint16_t atddrhl_register[ATD_SIZE];

	/**
	 * Analog signals are modeled as sample potential within VSSA and VDDA given by external tool
	 */
	double analog_signal[ATD_SIZE];

	// Authorised Bus Clock
	struct {
		clock_t maxBusClock;
		clock_t minBusClock;
	} busClockRange[32];


};

} // end of namespace hcs12x
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim


#endif /*__UNISIM_COMPONENT_CXX_PROCESSOR_HCS12X_ATD10B_HH__*/

