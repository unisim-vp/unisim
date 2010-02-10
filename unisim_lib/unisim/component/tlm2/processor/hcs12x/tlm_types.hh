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


#ifndef __UNISIM_COMPONENT_TLM2_PROCESSOR_TLM_TYPES_HH__
#define __UNISIM_COMPONENT_TLM2_PROCESSOR_TLM_TYPES_HH__

#include <inttypes.h>
#include <string>

#include <tlm.h>

#include "unisim/kernel/tlm2/tlm.hh"

#include <unisim/component/cxx/processor/hcs12x/types.hh>

namespace unisim {
namespace component {
namespace tlm2 {
namespace processor {
namespace hcs12x {

using namespace std;
using namespace tlm;

using unisim::kernel::tlm2::ManagedPayload;

using unisim::component::cxx::processor::hcs12x::address_t;

struct INT_TRANS_T {
	uint8_t ipl;
	address_t vectorAddress;
};

class XINT_Payload : public ManagedPayload
{
public:
	uint8_t	interrupt_offset;

};

class XINT_REQ_ProtocolTypes
{
public:
  typedef XINT_Payload tlm_payload_type;
  typedef tlm_phase tlm_phase_type;

};

template <uint8_t PWM_SIZE>
class PWM_Payload : public ManagedPayload
{
public:
	bool pwmChannel[PWM_SIZE];

	friend std::ostream& operator << (std::ostream& os, const PWM_Payload& payload) {

		for (int i=0; i<PWM_SIZE; i++) {
			os << payload.pwmChannel[i] << " ";
		}

		return os;
	}

};

template <uint8_t PWM_SIZE>
class UNISIM_PWM_ProtocolTypes
{
public:
  typedef PWM_Payload<PWM_SIZE> tlm_payload_type;
  typedef tlm_phase tlm_phase_type;
};

template <uint8_t ATD_SIZE>
class ATD_Payload : public ManagedPayload
{
public:
	double anPort[ATD_SIZE];

	friend std::ostream& operator << (std::ostream& os, const ATD_Payload& payload) {

		os.precision(3);

		for (int i=0; i<ATD_SIZE; i++) {
			os << fixed << payload.anPort[i] << " ";
		}

		os << std::dec;

		return os;
	}

};

template <uint8_t ATD_SIZE>
class UNISIM_ATD_ProtocolTypes
{
public:
  typedef ATD_Payload<ATD_SIZE> tlm_payload_type;
  typedef tlm_phase tlm_phase_type;
};


} // end of namespace hcs12x
} // end of namespace processor
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif /*__UNISIM_COMPONENT_TLM2_PROCESSOR_TLM_TYPES_HH__*/
