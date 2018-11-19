/*
 *  Copyright (c) 2018,
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

#ifndef __UNISIM_COMPONENT_TLM2_COM_BOSCH_M_CAN_M_CAN_HH__
#define __UNISIM_COMPONENT_TLM2_COM_BOSCH_M_CAN_M_CAN_HH__

#include <unisim/kernel/service/service.hh>
#include <unisim/kernel/logger/logger.hh>
#include <unisim/kernel/tlm2/tlm.hh>
#include <unisim/kernel/tlm2/tlm_can.hh>
#include <unisim/kernel/tlm2/clock.hh>
#include <unisim/util/reg/core/register.hh>
#include <unisim/util/likely/likely.hh>
#include <math.h>

#define SWITCH_ENUM_TRAIT(ENUM_TYPE, CLASS_NAME) template <ENUM_TYPE, bool __SWITCH_TRAIT_DUMMY__ = true> struct CLASS_NAME {}
#define CASE_ENUM_TRAIT(ENUM_VALUE, CLASS_NAME) template <bool __SWITCH_TRAIT_DUMMY__> struct CLASS_NAME<ENUM_VALUE, __SWITCH_TRAIT_DUMMY__>
#define ENUM_TRAIT(ENUM_VALUE, CLASS_NAME) CLASS_NAME<ENUM_VALUE>

namespace unisim {
namespace component {
namespace tlm2 {
namespace com {
namespace bosch {
namespace m_can {

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;

using unisim::util::reg::core::Register;
using unisim::util::reg::core::AddressableRegister;
using unisim::util::reg::core::AddressableRegisterHandle;
using unisim::util::reg::core::RegisterAddressMap;
using unisim::util::reg::core::FieldSet;
using unisim::util::reg::core::AddressableRegisterFile;
using unisim::util::reg::core::ReadWriteStatus;
using unisim::util::reg::core::SW_RW;
using unisim::util::reg::core::SW_R;
using unisim::util::reg::core::SW_R_HW_RO;
using unisim::util::reg::core::SW_R_W1C;
using unisim::util::reg::core::SW_W;
using unisim::util::reg::core::RWS_OK;
using unisim::util::reg::core::RWS_ANA;
using unisim::util::reg::core::Access;
using unisim::util::reg::core::IsReadWriteError;
using unisim::util::reg::core::ClearReadWriteStatusFlag;
using unisim::util::reg::core::RWSF_WROB;
using unisim::util::reg::core::RWS_WROR;

using unisim::kernel::tlm2::tlm_can_core;
using unisim::kernel::tlm2::tlm_can_core_config;
using unisim::kernel::tlm2::tlm_can_error;
using unisim::kernel::tlm2::tlm_can_core_activity;
using unisim::kernel::tlm2::tlm_can_phase;
using unisim::kernel::tlm2::tlm_can_message;
using unisim::kernel::tlm2::tlm_can_message_event;
using unisim::kernel::tlm2::tlm_can_state;
using unisim::kernel::tlm2::tlm_can_format;
using unisim::kernel::tlm2::TLM_CAN_SOF_PHASE;
using unisim::kernel::tlm2::TLM_CAN_ERROR_ACTIVE_STATE;
using unisim::kernel::tlm2::TLM_CAN_CORE_SYNCHRONIZING;
using unisim::kernel::tlm2::TLM_CAN_NO_ERROR;
using unisim::kernel::tlm2::TLM_CAN_STD_FMT;
using unisim::kernel::tlm2::TLM_CAN_XTD_FMT;
using unisim::kernel::tlm2::TLM_CAN_START_OF_FRAME_TRANSMISSION_EVENT;
using unisim::kernel::tlm2::TLM_CAN_START_OF_FRAME_RECEPTION_EVENT;
using unisim::kernel::tlm2::TLM_CAN_TRANSMISSION_ERROR_EVENT;
using unisim::kernel::tlm2::TLM_CAN_TRANSMISSION_CANCELLED_EVENT;
using unisim::kernel::tlm2::TLM_CAN_TRANSMISSION_OCCURRED_EVENT;
using unisim::kernel::tlm2::TLM_CAN_RECEPTION_ERROR_EVENT;
using unisim::kernel::tlm2::TLM_CAN_MESSAGE_RECEIVED_EVENT;
using unisim::kernel::tlm2::tlm_can_bus;
using unisim::kernel::tlm2::TLM_CAN_MAX_DATA_LENGTH;

template <typename REGISTER, typename FIELD, int OFFSET1, int OFFSET2 = -1, Access _ACCESS = SW_RW>
struct Field : unisim::util::reg::core::Field<FIELD
                                             , ((OFFSET1 >= 0) && (OFFSET1 < REGISTER::SIZE)) ? ((OFFSET2 >= 0) ? ((OFFSET2 < REGISTER::SIZE) ? ((OFFSET1 < OFFSET2) ? ((REGISTER::SIZE - 1) - OFFSET2) : ((REGISTER::SIZE - 1) - OFFSET1)) : ((REGISTER::SIZE - 1) - OFFSET1)) : ((REGISTER::SIZE - 1) - OFFSET1)) : 0
                                             , ((OFFSET1 >= 0) && (OFFSET1 < REGISTER::SIZE)) ? ((OFFSET2 >= 0) ? ((OFFSET2 < REGISTER::SIZE) ? ((OFFSET1 < OFFSET2) ? (OFFSET2 - OFFSET1 + 1) : (OFFSET1 - OFFSET2 + 1)) : 0) : 1) : 0
                                             , _ACCESS>
{
};

// Activity
enum ACTIVITY
{
	ACT_SYNCHRONIZING = 0, // synchronizing
	ACT_IDLE          = 1, // idle        
	ACT_RECEIVER      = 2, // receiver
	ACT_TRANSMITTER   = 3  // transmitter
};

inline std::ostream& operator << (std::ostream& os, const ACTIVITY& act)
{
	switch(act)
	{
		case ACT_SYNCHRONIZING: os << "synchronizing"; break;
		case ACT_IDLE         : os << "idle"; break;
		case ACT_RECEIVER     : os << "receiver"; break;
		case ACT_TRANSMITTER  : os << "transmitter"; break;
		default               : os << "doing unknown activity"; break;
	}
	
	return os;
}

// Last Error Code
enum LAST_ERROR_CODE
{
	LEC_NO_ERROR     = 0, // no error
	LEC_STUFF_ERROR  = 1, // stuff error
	LEC_FORM_ERROR   = 2, // form error
	LEC_ACK_ERROR    = 3, // acknowledgment error
	LEC_BIT1_ERROR   = 4, // bit error (recessive bit transmitted, dominant bit received)
	LEC_BIT0_ERROR   = 5, // bit error (dominant bit transmitted, recessive bit received)
	LEC_CRC_ERROR    = 6, // CRC error
	LEC_NO_CHANGE    = 7  // no change
};

inline std::ostream& operator << (std::ostream& os, const LAST_ERROR_CODE& lec)
{
	switch(lec)
	{
		case LEC_NO_ERROR   : os << "no error"; break;
		case LEC_STUFF_ERROR: os << "stuff error"; break;
		case LEC_FORM_ERROR : os << "form error"; break;
		case LEC_ACK_ERROR  : os << "acknowledgment error"; break;
		case LEC_BIT1_ERROR : os << "bit error (recessive bit transmitted, dominant bit received)"; break;
		case LEC_BIT0_ERROR : os << "bit error (dominant bit transmitted, recessive bit received)"; break;
		case LEC_CRC_ERROR  : os << "CRC error"; break;
		case LEC_NO_CHANGE  : os << "no change"; break;
		default             : os << "unknown last error code"; break;
	}
	
	return os;
}

// Tx Queue/FIFO Mode
enum TX_QUEUE_FIFO_MODE
{
	TQFM_FIFO  = 0, // FIFO Mode
	TQFM_QUEUE = 1  // Queue Mode
};

inline std::ostream& operator << (std::ostream& os, const TX_QUEUE_FIFO_MODE& tqfm)
{
	switch(tqfm)
	{
		case TQFM_FIFO : os << "FIFO Mode"; break;
		case TQFM_QUEUE: os << "Queue Mode"; break;
		default        : os << "unknown Tx Queue/FIFO mode"; break;
	}
	
	return os;
}

// Timestamp select
enum TIMESTAMP_SELECT
{
	TSS_ALWAYS_ZERO_00 = 0, // always 0
	TSS_INCREMENTED    = 1, // incremented
	TSS_RESERVED       = 2, // reserved
	TSS_ALWAYS_ZERO_11 = 3  // always 0
};

inline std::ostream& operator << (std::ostream& os, const TIMESTAMP_SELECT& tss)
{
	switch(tss)
	{
		 case TSS_ALWAYS_ZERO_00: os << "always 0"; break;
		 case TSS_INCREMENTED   : os << "incremented"; break;
		 case TSS_RESERVED      : os << "reserved"; break;
		 case TSS_ALWAYS_ZERO_11: os << "always 0"; break;
		 default                : os << "unknown timestamp select"; break;
	}
	
	return os;
}

// Timeout Select
enum TIMEOUT_SELECT
{
	TOS_CONTINOUS     = 0, // Continuous
	TOS_TX_EVENT_FIFO = 1, // Tx Event FIFO
	TOS_RX_FIFO_0     = 2, // Rx FIFO #0
	TOS_RX_FIFO_1     = 3  // Rx FIFO #1
};

inline std::ostream& operator << (std::ostream& os, const TIMEOUT_SELECT& tos)
{
	switch(tos)
	{
		case TOS_CONTINOUS    : os << "Continuous"; break;
		case TOS_TX_EVENT_FIFO: os << "Tx Event FIFO"; break;
		case TOS_RX_FIFO_0    : os << "Rx FIFO #0"; break;
		case TOS_RX_FIFO_1    : os << "Rx FIFO #1"; break;
		default               : os << "unknown timeout select"; break;
	}
	
	return os;
}

// Message Store Indicator
enum MESSAGE_STORAGE_INDICATOR
{
	MSI_NO_FIFO_SELECTED        = 0, // No FIFO selected
	MSI_FIFO_OVERRUN            = 1, // FIFO overrun
	MSI_MESSAGE_STORE_IN_FIFO_0 = 2, // Message Store in FIFO #0
	MSI_MESSAGE_STORE_IN_FIFO_1 = 3  // Message Store in FIFO #1
};

inline std::ostream& operator << (std::ostream& os, const MESSAGE_STORAGE_INDICATOR& msi)
{
	switch(msi)
	{
		 case MSI_NO_FIFO_SELECTED       : os << "No FIFO selected"; break;
		 case MSI_FIFO_OVERRUN           : os << "FIFO overrun"; break;
		 case MSI_MESSAGE_STORE_IN_FIFO_0: os << "Message Store in FIFO #0"; break;
		 case MSI_MESSAGE_STORE_IN_FIFO_1: os << "Message Store in FIFO #1"; break;
		 default                         : os << "unknown message store indicator"; break;
	}
	
	return os;
}

// Debug Message Type
enum DEBUG_MESSAGE_TYPE
{
	DBG_MSG_A = 0, // Debug Message A
	DBG_MSG_B = 1, // Debug Message B
	DBG_MSG_C = 3  // Debug Message C
};

inline std::ostream& operator << (std::ostream& os, const DEBUG_MESSAGE_TYPE& t)
{
	switch(t)
	{
		case DBG_MSG_A: os << "Debug Message A"; break;
		case DBG_MSG_B: os << "Debug Message B"; break;
		case DBG_MSG_C: os << "Debug Message C"; break;
		default       : os << "unknown Debug Message"; break;
	}
	
	return os;
}

enum DEBUG_MESSAGE_STATUS
{
	DMS_IDLE                          = 0, // idle state, wait for reception of debug messages, DMA request is cleared
	DMS_DEBUG_MESSAGE_A_RECEIVED      = 1, // Debug Message A received
	DMS_DEBUG_MESSAGES_A_B_RECEIVED   = 2, // Debug Messages A, B received
	DMS_DEBUG_MESSAGES_A_B_C_RECEIVED = 3  // Debug Messages A, B, C received, DMA request is set
};

inline std::ostream& operator << (std::ostream& os, const DEBUG_MESSAGE_STATUS& dms)
{
	switch(dms)
	{
		case DMS_IDLE                         : os << "idle state, wait for reception of debug messages, DMA request is cleared"; break;
		case DMS_DEBUG_MESSAGE_A_RECEIVED     : os << "Debug Message A received"; break;
		case DMS_DEBUG_MESSAGES_A_B_RECEIVED  : os << "Debug Messages A, B received"; break;
		case DMS_DEBUG_MESSAGES_A_B_C_RECEIVED: os << "Debug Messages A, B, C received, DMA request is set"; break;
		default                               : os << "unknown debug message handling status"; break;
	}
	
	return os;
}

// Filter Type (for SFT/EFT in Filter Element)
enum FILTER_TYPE 
{
	RANGE_FILTER   = 0, // Range Filter
	DUAL_ID_FILTER = 1, // Dual ID Filter
	CLASSIC_FILTER = 2  // Classic Filter
};

inline std::ostream& operator << (std::ostream& os, const FILTER_TYPE& ft)
{
	switch(ft)
	{
		case RANGE_FILTER  : os << "Range Filter"; break;
		case DUAL_ID_FILTER: os << "Dual ID Filter"; break;
		case CLASSIC_FILTER: os << "Classic Filter"; break;
		default            : os << "unknown Filter"; break;
	}
	
	return os;
}

// Filter Element Configuration (for SFEC/EFEC in Filter Element)
enum FILTER_ELEMENT_CONFIGURATION
{
	FEC_DISABLE_FILTER_ELEMENT                                = 0, // Disable Filter Element
	FEC_STORE_IN_RX_FIFO_0_IF_FILTER_MATCHES                  = 1, // Store in Rx FIFO #0 if filter matches
	FEC_STORE_IN_RX_FIFO_1_IF_FILTER_MATCHES                  = 2, // Store in Rx FIFO #1 if filter matches
	FEC_REJECT_ID_IF_FILTER_MATCHES                           = 3, // Reject ID if filter matches
	FEC_SET_PRIORITY_IF_FILTER_MATCHES                        = 4, // Set priority if filter matches
	FEC_SET_PRIORITY_AND_STORE_IN_RX_FIFO_0_IF_FILTER_MATCHES = 5, // Set priority and store in Rx FIFO #0 if filter matches
	FEC_SET_PRIORITY_AND_STORE_IN_RX_FIFO_1_IF_FILTER_MATCHES = 6, // Set priority and store in Rx FIFO #1 if filter matches
	FEC_STORE_INTO_RX_BUFFER_OR_AS_DEBUG_MESSAGE              = 7  // Store in Rx Buffer or as a Debug Message
};

inline std::ostream& operator << (std::ostream& os, const FILTER_ELEMENT_CONFIGURATION& fec)
{
	switch(fec)
	{
		case FEC_DISABLE_FILTER_ELEMENT                               : os << "Disable Filter Element"; break;
		case FEC_STORE_IN_RX_FIFO_0_IF_FILTER_MATCHES                 : os << "Store in Rx FIFO #0 if filter matches"; break;
		case FEC_STORE_IN_RX_FIFO_1_IF_FILTER_MATCHES                 : os << "Store in Rx FIFO #1 if filter matches"; break;
		case FEC_REJECT_ID_IF_FILTER_MATCHES                          : os << "Reject ID if filter matches"; break;
		case FEC_SET_PRIORITY_IF_FILTER_MATCHES                       : os << "Set priority if filter matches"; break;
		case FEC_SET_PRIORITY_AND_STORE_IN_RX_FIFO_0_IF_FILTER_MATCHES: os << "Set priority and store in Rx FIFO #0 if filter matches"; break;
		case FEC_SET_PRIORITY_AND_STORE_IN_RX_FIFO_1_IF_FILTER_MATCHES: os << "Set priority and store in Rx FIFO #1 if filter matches"; break;
		case FEC_STORE_INTO_RX_BUFFER_OR_AS_DEBUG_MESSAGE             : os << "Store in Rx Buffer or as a Debug Message"; break;
		default                                                       : os << "Unknown filter element configuration"; break;
	}
	
	return os;
}

// Rx Buffer or Debug Message Selector (for FID2[10:9] in Filter Element)
enum RX_BUFFER_OR_DEBUG_MESSAGE_SELECTOR  // for FID2[10:9]
{
	SEL_STORE_MESSAGE_IN_AN_RX_BUFFER = 0, // Store Message in an Rx Buffer
	SEL_DEBUG_MESSAGE_A               = 1, // Debug Message A
	SEL_DEBUG_MESSAGE_B               = 2, // Debug Message B
	SEL_DEBUG_MESSAGE_C               = 3  // Debug Message C
};

inline std::ostream& operator << (std::ostream& os, const RX_BUFFER_OR_DEBUG_MESSAGE_SELECTOR& s)
{
	switch(s)
	{
		case SEL_STORE_MESSAGE_IN_AN_RX_BUFFER: os << "Store Message in an Rx Buffer"; break;
		case SEL_DEBUG_MESSAGE_A              : os << "Debug Message A"; break;
		case SEL_DEBUG_MESSAGE_B              : os << "Debug Message B"; break;
		case SEL_DEBUG_MESSAGE_C              : os << "Debug Message C"; break;
		default                               : os << "Unknown Rx Buffer or Debug Message Selector"; break;
	}
	
	return os;
}

struct CAN_Message_RAM_Word
{
	static const unsigned int SIZE = 32;
};

// Rx Buffer and FIFO Element
struct Rx_Buffer_FIFO_Element
{
	struct R0 : CAN_Message_RAM_Word
	{
		struct ESI : Field<R0, ESI, 0    > {}; // Error State Indicator
		struct XTD : Field<R0, XTD, 1    > {}; // Extended Indentifier
		struct RTR : Field<R0, RTR, 2    > {}; // Remote Transmission Request
		struct ID  : Field<R0, ID , 3, 31> {}; // Indentifier
	};
	
	struct R1 : CAN_Message_RAM_Word
	{
		struct ANMF : Field<R1, ANMF, 0     > {}; // Accepted Non-Matching Frame
		struct FIDX : Field<R1, FIDX, 1 , 7 > {}; // Filter Index
		struct EDL  : Field<R1, EDL , 10    > {}; // Extended Data Length
		struct BRS  : Field<R1, BRS , 11    > {}; // Bit Rate Switch
		struct DLC  : Field<R1, DLC , 12, 15> {}; // Data Length Code
		struct RXTS : Field<R1, RXTS, 16, 31> {}; // Rx Timestamp
	};
	
	unsigned int GetDataLength() const
	{
		unsigned int dlc = R1::DLC::template Get<uint32_t>(r[1]);
		if(R0::XTD::template Get<uint32_t>(r[0]) && R1::EDL::template Get<uint32_t>(r[1]))
		{
			if(dlc <= 8) return dlc;
			if(dlc <= 12) return 8 + (4 * (dlc - 8));
			if(dlc <= 15) return 16 + (16 * (dlc - 12));
		}
		else
		{
			return (dlc <= 8) ? dlc : 8;
		}
		
		return 0;
	}
	
	unsigned int element_size;
	uint32_t r[2 + (TLM_CAN_MAX_DATA_LENGTH / 4)];
};

inline std::ostream& operator << (std::ostream& os, const Rx_Buffer_FIFO_Element& e)
{
	os << "Rx_Buffer_FIFO_Element(R0=(ESI=" << Rx_Buffer_FIFO_Element::R0::ESI::template Get<uint32_t>(e.r[0]);
	os << ",XTD=" << Rx_Buffer_FIFO_Element::R0::XTD::template Get<uint32_t>(e.r[0]);
	os << ",RTR=" << Rx_Buffer_FIFO_Element::R0::RTR::template Get<uint32_t>(e.r[0]);
	os << ",ID=" << Rx_Buffer_FIFO_Element::R0::ID::template Get<uint32_t>(e.r[0]) << ")";
	os << ",R1=(ANMF=" << Rx_Buffer_FIFO_Element::R1::ANMF::template Get<uint32_t>(e.r[1]);
	os << ",FIDX=" << Rx_Buffer_FIFO_Element::R1::FIDX::template Get<uint32_t>(e.r[1]);
	os << ",EDL=" << Rx_Buffer_FIFO_Element::R1::EDL::template Get<uint32_t>(e.r[1]);
	os << ",BRS=" << Rx_Buffer_FIFO_Element::R1::BRS::template Get<uint32_t>(e.r[1]);
	os << ",DLC=" << Rx_Buffer_FIFO_Element::R1::DLC::template Get<uint32_t>(e.r[1]);
	os << ",RXTS=" << Rx_Buffer_FIFO_Element::R1::RXTS::template Get<uint32_t>(e.r[1]) << ")";
	unsigned int n = e.element_size - 2;
	for(unsigned int i = 0; i < n; i++)
	{
		os << ",R" << (2 + i) << "=0x" << std::hex << e.r[2 + i] << std::dec;
	}
	return os << ")";
}

// Tx Buffer Element
struct Tx_Buffer_Element
{
	struct T0 : CAN_Message_RAM_Word
	{
		struct XTD : Field<T0, XTD, 1    > {}; // Extended Identifier
		struct RTR : Field<T0, RTR, 2    > {}; // Remote Transmission Request
		struct ID  : Field<T0, ID , 3, 31> {}; // Identifier
	};
	
	struct T1 : CAN_Message_RAM_Word
	{
		struct MM  : Field<T1, MM , 0 , 7 > {}; // Message Marker
		struct EFC : Field<T1, EFC, 8     > {}; // Event FIFO Control
		struct DLC : Field<T1, DLC, 12, 15> {}; // Data Length Code
	};
	
	unsigned int element_size;
	uint32_t t[2 + (TLM_CAN_MAX_DATA_LENGTH / 4)];
};

inline std::ostream& operator << (std::ostream& os, const Tx_Buffer_Element& e)
{
	os << "Tx_Buffer_Element(T0=(XTD=" << Tx_Buffer_Element::T0::XTD::template Get<uint32_t>(e.t[0]);
	os << ",RTR=" << Tx_Buffer_Element::T0::RTR::template Get<uint32_t>(e.t[0]);
	os << ",ID=" << Tx_Buffer_Element::T0::ID::template Get<uint32_t>(e.t[0]) << ")";
	os << ",T1=(MM=" << Tx_Buffer_Element::T1::MM::template Get<uint32_t>(e.t[1]);
	os << ",EFC=" << Tx_Buffer_Element::T1::EFC::template Get<uint32_t>(e.t[1]);
	os << ",DLC=" << Tx_Buffer_Element::T1::DLC::template Get<uint32_t>(e.t[1]) << ")";
	unsigned int n = e.element_size - 2;
	for(unsigned int i = 0; i < n; i++)
	{
		os << ",T" << (2 + i) << "=0x" << std::hex << e.t[2 + i] << std::dec;
	}
	return os << ")";
}

// Tx Event FIFO Element
struct Tx_Event_FIFO_Element
{
	static const unsigned int SIZE = 64;
	
	struct E0 : CAN_Message_RAM_Word
	{
		struct ESI : Field<E0, ESI, 0    > {}; // Error State Indicator
		struct XTD : Field<E0, XTD, 1    > {}; // Extended Identifier
		struct RTR : Field<E0, RTR, 2    > {}; // Remote Transmission Request
		struct ID  : Field<E0, ID , 3, 31> {}; // Identifier
	};
	
	struct E1 : CAN_Message_RAM_Word
	{
		struct MM   : Field<E1, MM  , 0 , 7 > {}; // Message Marker
		struct ET   : Field<E1, ET  , 8 , 9 > {}; // Event Type
		struct EDL  : Field<E1, EDL , 10    > {}; // Extended Data Length
		struct BRS  : Field<E1, BRS , 11    > {}; // Bit Rate Switch
		struct DLC  : Field<E1, DLC , 12, 15> {}; // Data Length Code
		struct TXTS : Field<E1, TXTS, 16, 31> {}; // Tx Timestamp
	};
	
	uint32_t e[2];
};

inline std::ostream& operator << (std::ostream& os, const Tx_Event_FIFO_Element& e)
{
	os << "Tx_Event_FIFO_Element(E0=(ESI=" << Tx_Event_FIFO_Element::E0::ESI::template Get<uint32_t>(e.e[0]);
	os << ",XTD=" << Tx_Event_FIFO_Element::E0::XTD::template Get<uint32_t>(e.e[0]);
	os << ",RTR=" << Tx_Event_FIFO_Element::E0::RTR::template Get<uint32_t>(e.e[0]);
	os << ",ID=" << Tx_Event_FIFO_Element::E0::ID::template Get<uint32_t>(e.e[0]) << ")";
	os << ",E1=(MM=" << Tx_Event_FIFO_Element::E1::MM::template Get<uint32_t>(e.e[1]);
	os << ",ET=" << Tx_Event_FIFO_Element::E1::ET::template Get<uint32_t>(e.e[1]);
	os << ",EDL=" << Tx_Event_FIFO_Element::E1::EDL::template Get<uint32_t>(e.e[1]);
	os << ",BRS=" << Tx_Event_FIFO_Element::E1::BRS::template Get<uint32_t>(e.e[1]);
	os << ",DLC=" << Tx_Event_FIFO_Element::E1::DLC::template Get<uint32_t>(e.e[1]);
	os << ",TXTS=" << Tx_Event_FIFO_Element::E1::TXTS::template Get<uint32_t>(e.e[1]) << ")";
	return os << ")";
}

// Standard Message ID Filter Element
struct Standard_Message_ID_Filter_Element
{
	struct S0 : CAN_Message_RAM_Word
	{
		struct SFT        : Field<S0, SFT       , 0 , 1 > {}; // Standard Filter Type
		struct SFEC       : Field<S0, SFEC      , 2 , 4 > {}; // Standard Filter Element Configuration
		struct SFID1      : Field<S0, SFID1     , 5 , 15> {}; // Standard Filter ID 1
		struct SFID2      : Field<S0, SFID2     , 21, 31> {}; // Standard Filter ID 2 when (SFEC != 000b) AND (SFEC != 111b)
		struct SFID2_10_9 : Field<S0, SFID2_10_9, 21, 22> {}; // Rx Buffers or Debug Message when SFEC=111b
		struct SFID2_8_6  : Field<S0, SFID2_8_6 , 23, 25> {}; // m_can filter event pins (mask) for which a pulse is generated when SFEC=111b
		struct SFID2_5_0  : Field<S0, SFID2_5_0 , 26, 31> {}; // Offset relative to RXBC[RBSA] when SFEC=111b
	};
	
	uint32_t s0;
};

inline std::ostream& operator << (std::ostream& os, const Standard_Message_ID_Filter_Element& e)
{
	os << "Standard_Message_ID_Filter_Element(S0=(SFT='" << FILTER_TYPE(Standard_Message_ID_Filter_Element::S0::SFT::template Get<uint32_t>(e.s0)) << "'";
	os << ",SFEC='" << FILTER_ELEMENT_CONFIGURATION(Standard_Message_ID_Filter_Element::S0::SFEC::template Get<uint32_t>(e.s0)) << "'";
	os << ",SFID1=" << Standard_Message_ID_Filter_Element::S0::SFID1::template Get<uint32_t>(e.s0);
	os << ",SFID2=" << Standard_Message_ID_Filter_Element::S0::SFID2::template Get<uint32_t>(e.s0) << ")";
	return os << ")";
}

// Extended Message ID Filter Element
struct Extended_Message_ID_Filter_Element
{
	struct F0 : CAN_Message_RAM_Word
	{
		struct EFEC  : Field<F0, EFEC , 0, 2 > {}; // Extended Filter Element Configuration
		struct EFID1 : Field<F0, EFID1, 3, 31> {}; // Extended Filter ID 1
	};
	
	struct F1 : CAN_Message_RAM_Word
	{
		struct EFT        : Field<F1, EFT       , 0 , 1 > {}; // Extended Filter Type
		struct EFID2      : Field<F1, EFID2     , 3 , 31> {}; // Extended Filter ID 2 when (EFEC != 000b) AND (EFEC != 111b)
		struct EFID2_10_9 : Field<F1, EFID2_10_9, 21, 22> {}; // Rx Buffers or Debug Message when EFEC=111b
		struct EFID2_8_6  : Field<F1, EFID2_8_6 , 23, 25> {}; // m_can filter event pins (mask) for which a pulse is generated when EFEC=111b
		struct EFID2_5_0  : Field<F1, EFID2_5_0 , 26, 31> {}; // Offset relative to RXBC[RBSA] when EFEC=111b
	};
	
	uint32_t f0;
	uint32_t f1;
};

inline std::ostream& operator << (std::ostream& os, const Extended_Message_ID_Filter_Element& e)
{
	os << "Extended_Message_ID_Filter_Element(F0=(EFEC='" << FILTER_ELEMENT_CONFIGURATION(Extended_Message_ID_Filter_Element::F0::EFEC::template Get<uint32_t>(e.f0)) << "'";
	os << ",EFID1=" << Extended_Message_ID_Filter_Element::F0::EFID1::template Get<uint32_t>(e.f0) << ")";
	os << ",F1=(EFT='" << FILTER_TYPE(Extended_Message_ID_Filter_Element::F1::EFT::template Get<uint32_t>(e.f1)) << "'";
	os << ",EFID2=" << Extended_Message_ID_Filter_Element::F1::EFID2::template Get<uint32_t>(e.f1) << ")";
	return os << ")";
}

#if 0
struct CONFIG
{
	// Core Release 3.0.1, May 6th, 2013
	static const unsigned int CORE_RELEASE         = 3; // in [0-9]
	static const unsigned int CORE_RELEASE_STEP    = 0; // in [0-9]
	static const unsigned int CORE_RELEASE_SUBSTEP = 1; // in [0-9]
	static const unsigned int CORE_RELEASE_YEAR    = 3; // in [0-9]
	static const unsigned int CORE_RELEASE_MONTH   = 5; // in [0-99]
	static const unsigned int CORE_RELEASE_DAY     = 6; // in [0-99]
	static const unsigned int BUSWIDTH             = 32;
};
#endif

struct M_CAN_Message : tlm_can_message
{
	typedef tlm_can_message Super;
	
	M_CAN_Message() : Super(), buffer_index(0), event_fifo_control(false), message_marker(0), time_stamp(0) {}
	
	void Reset() { Super::reset(); buffer_index = 0; event_fifo_control = false; message_marker = 0; time_stamp = 0; }
	
	unsigned int GetBufferIndex() const { return buffer_index; }
	bool GetEFC() const { return event_fifo_control; }
	unsigned int GetMessageMarker() const { return message_marker; }
	unsigned int GetTimeStamp() const { return time_stamp; }
	
	void SetBufferIndex(unsigned int _buffer_index) { buffer_index = _buffer_index; }
	void SetEFC(unsigned int efc) { event_fifo_control = efc; }
	void SetMessageMarker(unsigned int _message_marker) { message_marker = _message_marker; }
	void SetTimeStamp(unsigned int _time_stamp) { time_stamp = _time_stamp; }
private:
	unsigned int buffer_index;
	bool event_fifo_control;
	unsigned int message_marker;
	unsigned int time_stamp;
	
	friend std::ostream& operator << (std::ostream& os, const M_CAN_Message& msg);
};

inline std::ostream& operator << (std::ostream& os, const M_CAN_Message& msg)
{
	os << "M_CAN_Message(" << (const tlm_can_message&) msg;
	os << ",buffer_index=" << msg.buffer_index;
	os << ",event_fifo_control=" << msg.event_fifo_control;
	os << ",message_marker=" << msg.message_marker;
	os << ",time_stamp=" << msg.time_stamp;
	return os << ")";
}

struct M_CAN_TYPES
{
	typedef M_CAN_Message CAN_MESSAGE;
};

template <typename CONFIG>
class M_CAN
	: public tlm_can_core<M_CAN<CONFIG>, M_CAN_TYPES>
	, public tlm::tlm_bw_transport_if<>
	, public tlm::tlm_fw_transport_if<>
{
public:
	typedef tlm_can_core<M_CAN<CONFIG>, M_CAN_TYPES> Super;
	
	static const unsigned int TLM2_IP_VERSION_MAJOR    = 1;
	static const unsigned int TLM2_IP_VERSION_MINOR    = 0;
	static const unsigned int TLM2_IP_VERSION_PATCH    = 0;
	static const unsigned int BUSWIDTH                 = CONFIG::BUSWIDTH;
	static const bool threaded_model                   = false;
	static const unsigned int ERROR_WARNING_LIMIT      = 96;
	static const unsigned int MAX_DEDICATED_TX_BUFFERS = 32;
	static const unsigned int MAX_TX_QUEUE_FIFO_SIZE   = 32;
	static const unsigned int MAX_TX_BUFFERS           = 32;
	static const unsigned int MAX_STD_MSG_ID_FILTER_ELEMENTS = 128;
	static const unsigned int MAX_XTD_MSG_ID_FILTER_ELEMENTS = 64;
	static const unsigned int MAX_RX_FIFO_WATERMARK = 64;
	static const unsigned int MAX_RX_FIFO_SIZE = 64;
	static const unsigned int MAX_TX_EVENT_FIFO_SIZE = 32;
	
	
	// TLM socket types
	typedef tlm::tlm_target_socket<BUSWIDTH>         peripheral_slave_if_type;
	typedef tlm::tlm_initiator_socket<BUSWIDTH>      can_message_ram_if_type;
	
	// inputs
	peripheral_slave_if_type                         peripheral_slave_if;         // Peripheral slave interface
	sc_core::sc_in<bool>                             m_clk;                       // clock port
	sc_core::sc_in<bool>                             can_clk;                     // M_CAN Clock port
	sc_core::sc_in<bool>                             reset_b;                     // reset
	sc_core::sc_in<bool>                             DMA_ACK;                     // DMA Acknowledgement
	
	// outputs
	can_message_ram_if_type                          can_message_ram_if;          // CAN message RAM interface
	sc_core::sc_out<bool>                            INT0;                        // interrupt 0
	sc_core::sc_out<bool>                            INT1;                        // interrupt 1
	sc_core::sc_out<bool>                            DMA_REQ;                     // DMA request output
	
	M_CAN(const sc_core::sc_module_name& name, unisim::kernel::service::Object *parent);
	virtual ~M_CAN();
	
	// Backward path (CAN message RAM interface)
	virtual tlm::tlm_sync_enum nb_transport_bw(tlm::tlm_generic_payload& payload, tlm::tlm_phase& phase, sc_core::sc_time& t);
	virtual void invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range);

	// Forward Path (peripheral slave interface)
	virtual void b_transport(tlm::tlm_generic_payload& payload, sc_core::sc_time& t);
	virtual bool get_direct_mem_ptr(tlm::tlm_generic_payload& payload, tlm::tlm_dmi& dmi_data);
	virtual unsigned int transport_dbg(tlm::tlm_generic_payload& payload);
	virtual tlm::tlm_sync_enum nb_transport_fw(tlm::tlm_generic_payload& payload, tlm::tlm_phase& phase, sc_core::sc_time& t);
	
protected:
	friend tlm_can_core<M_CAN<CONFIG>, M_CAN_TYPES>;
	
	bool is_enabled() const;
	const tlm_can_core_config& get_config() const;
	tlm_can_error get_can_error() const;
	void set_can_error(tlm_can_error _can_error);
	tlm_can_core_activity get_core_activity() const;
	void set_core_activity(tlm_can_core_activity _core_activity);
	tlm_can_phase get_phase() const;
	void set_phase(tlm_can_phase _phase);
	uint8_t get_transmit_error_count() const;
	uint8_t get_receive_error_count() const;
	tlm_can_state get_state() const;
	void set_transmit_error_count(uint8_t v);
	void set_receive_error_count(uint8_t v);
	void set_state(tlm_can_state _state);
	bool has_pending_transmission_request() const;
	const M_CAN_Message& fetch_pending_transmission_request();
	M_CAN_Message& get_receive_message();
	void process_message_event(const tlm_can_message_event<M_CAN_TYPES>& msg_event);
	void received_bit(bool value);
	unsigned int get_transmit_pause() const;
	
private:
	virtual void end_of_elaboration();
	
	enum EventType
	{
		EV_NONE = 0,
		EV_WAKE_UP,
		EV_CPU_PAYLOAD
	};
	
	class Event
	{
	public:
		class Key
		{
		public:
			Key()
				: time_stamp(sc_core::SC_ZERO_TIME)
				, event_type()
			{
			}
			
			Key(const sc_core::sc_time& _time_stamp, EventType _event_type)
				: time_stamp(_time_stamp)
				, event_type(_event_type)
			{
			}

			void SetTimeStamp(const sc_core::sc_time& _time_stamp)
			{
				time_stamp = _time_stamp;
			}
			
			void SetEventType(EventType _event_type)
			{
				event_type = _event_type;
			}

			const sc_core::sc_time& GetTimeStamp() const
			{
				return time_stamp;
			}
			
			EventType GetEventType() const
			{
				return event_type;
			}

			void Clear()
			{
				time_stamp = sc_core::SC_ZERO_TIME;
				event_type = EV_NONE;
			}
			
			int operator < (const Key& sk) const
			{
				return (time_stamp < sk.time_stamp) || ((time_stamp == sk.time_stamp) && (event_type < sk.event_type));
			}
			
		private:
			sc_core::sc_time time_stamp;
			EventType event_type;                      // type of event
		};
		
		Event()
			: key()
			, payload(0)
			, release_payload(false)
			, completion_event(0)
		{
		}
		
		~Event()
		{
			Clear();
		}
		
		void Clear()
		{
			key.Clear();
			if(release_payload)
			{
				if(payload && payload->has_mm()) payload->release();
			}
			payload = 0;
			release_payload = false;
			completion_event  = 0;
		}
		
		void SetTimeStamp(const sc_core::sc_time& _time_stamp)
		{
			key.SetTimeStamp(_time_stamp);
		}
		
		void SetPayload(tlm::tlm_generic_payload *_payload, bool _release_payload = false)
		{
			key.SetEventType(EV_CPU_PAYLOAD);
			if(release_payload)
			{
				if(payload && payload->has_mm()) payload->release();
			}
			payload = _payload;
			release_payload = _release_payload;
		}
		
		void WakeUp()
		{
			key.SetEventType(EV_WAKE_UP);
		}
		
		void SetCompletionEvent(sc_core::sc_event *_completion_event)
		{
			completion_event = _completion_event;
		}

		const sc_core::sc_time& GetTimeStamp() const
		{
			return key.GetTimeStamp();
		}
		
		EventType GetType() const
		{
			return key.GetEventType();
		}

		tlm::tlm_generic_payload *GetPayload() const
		{
			return payload;
		}
		
		sc_core::sc_event *GetCompletionEvent() const
		{
			return completion_event;
		}

		const Key& GetKey() const
		{
			return key;
		}
		
	private:
		Key key;                             // schedule key (i.e. time stamp)
		tlm::tlm_generic_payload *payload;   // payload
		bool release_payload;                // whether payload must be released using payload memory management
		sc_core::sc_event *completion_event; // completion event (for blocking transport interface)
	};
	
	// M_CAN Register
	template <typename REGISTER, Access _ACCESS>
	struct M_CAN_Register : AddressableRegister<REGISTER, 32, _ACCESS, sc_core::sc_time>
	{
		typedef AddressableRegister<REGISTER, 32, _ACCESS, sc_core::sc_time> Super;
		
		M_CAN_Register(M_CAN<CONFIG> *_m_can) : Super(), m_can(_m_can) {}
		M_CAN_Register(M_CAN<CONFIG> *_m_can, uint32_t value) : Super(value), m_can(_m_can) {}
		
		inline bool IsVerboseRead() const ALWAYS_INLINE { return m_can->verbose; }
		inline bool IsVerboseWrite() const ALWAYS_INLINE { return m_can->verbose; }
		inline std::ostream& GetInfoStream() ALWAYS_INLINE { return m_can->logger.DebugInfoStream(); }

		using Super::operator =;
	protected:
		M_CAN<CONFIG> *m_can;
	};

	// Core Release Register (M_CAN_CREL)
	struct M_CAN_CREL : M_CAN_Register<M_CAN_CREL, SW_R>
	{
		typedef M_CAN_Register<M_CAN_CREL, SW_R> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x0;
		
		struct REL     : Field<M_CAN_CREL, REL    , 0, 3  , SW_R> {}; // Core Release
		struct STEP    : Field<M_CAN_CREL, STEP   , 4, 7  , SW_R> {}; // Step of Core Release
		struct SUBSTEP : Field<M_CAN_CREL, SUBSTEP, 8, 11 , SW_R> {}; // Sub-step of Core Release
		struct YEAR    : Field<M_CAN_CREL, YEAR   , 12, 15, SW_R> {}; // Time Stamp Year
		struct MON     : Field<M_CAN_CREL, MON    , 16, 23, SW_R> {}; // Time Stamp Month
		struct DAY     : Field<M_CAN_CREL, DAY    , 24, 31, SW_R> {}; // Time Stamp Day
		
		typedef FieldSet<REL, STEP, SUBSTEP, YEAR, MON, DAY> ALL;
		
		M_CAN_CREL(M_CAN<CONFIG> *_m_can) : Super(_m_can) { Init(); }
		M_CAN_CREL(M_CAN<CONFIG> *_m_can, uint32_t value) : Super(_m_can, value) { Init(); }
		
		void Init()
		{
			this->SetName("M_CAN_CREL"); this->SetDescription("Core Release Register");
			
			REL    ::SetName("REL");     REL    ::SetDescription("Core Release");
			STEP   ::SetName("STEP");    STEP   ::SetDescription("Step of Core Release");
			SUBSTEP::SetName("SUBSTEP"); SUBSTEP::SetDescription("Sub-step of Core Release");
			YEAR   ::SetName("YEAR");    YEAR   ::SetDescription("Time Stamp Year");
			MON    ::SetName("MON");     MON    ::SetDescription("Time Stamp Month");
			DAY    ::SetName("DAY");     DAY    ::SetDescription("Time Stamp Day");
		}
		
		void Reset()
		{
			 this->template Set<REL    >(CONFIG::CORE_RELEASE % 10);
			 this->template Set<STEP   >(CONFIG::CORE_RELEASE_STEP % 10);
			 this->template Set<SUBSTEP>(CONFIG::CORE_RELEASE_SUBSTEP % 10);
			 this->template Set<YEAR   >(CONFIG::CORE_RELEASE_YEAR % 10);
			 this->template Set<MON    >((((CONFIG::CORE_RELEASE_MONTH / 10) % 10) << 4) | (CONFIG::CORE_RELEASE_MONTH % 10));
			 this->template Set<DAY    >((((CONFIG::CORE_RELEASE_DAY / 10) % 10) << 4) | (CONFIG::CORE_RELEASE_DAY % 10));
		}
		
		using Super::operator =;
	};
	
	// Endian Register (M_CAN_ENDN)
	struct M_CAN_ENDN : M_CAN_Register<M_CAN_ENDN, SW_R>
	{
		typedef M_CAN_Register<M_CAN_ENDN, SW_R> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x4;
		
		struct ETV : Field<M_CAN_ENDN, ETV, 0, 31, SW_R> {}; // Endianness Test Value
		
		typedef FieldSet<ETV> ALL;
		
		M_CAN_ENDN(M_CAN<CONFIG> *_m_can) : Super(_m_can) { Init(); }
		M_CAN_ENDN(M_CAN<CONFIG> *_m_can, uint32_t value) : Super(_m_can, value) { Init(); }
		
		void Init()
		{
			this->SetName("M_CAN_ENDN"); this->SetDescription("Endian Register");
			
			ETV::SetName("ETV"); ETV::SetDescription("Endianness Test Value");
		}
		
		void Reset()
		{
			this->Initialize(0x87654321);
		}
		
		using Super::operator =;
	};
	
	// Fast Bit Timing and Prescaler Register (M_CAN_FBTP)
	struct M_CAN_FBTP : M_CAN_Register<M_CAN_FBTP, SW_RW>
	{
		typedef M_CAN_Register<M_CAN_FBTP, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0xc;
		
		struct TDCO   : Field<M_CAN_FBTP, TDCO  , 3, 7  , SW_RW> {}; // Transceiver Delay Compensation Offset
		struct TDC    : Field<M_CAN_FBTP, TDC   , 8, 8  , SW_RW> {}; // Transceiver Delay Compensation
		struct FBRP   : Field<M_CAN_FBTP, FBRP  , 11, 15, SW_RW> {}; // Fast Baud Rate Prescaler
		struct FTSEG1 : Field<M_CAN_FBTP, FTSEG1, 20, 23, SW_RW> {}; // Fast time segment before sample point
		struct FTSEG2 : Field<M_CAN_FBTP, FTSEG2, 25, 27, SW_RW> {}; // Fast time segment after sample point
		struct FSJW   : Field<M_CAN_FBTP, FSJW  , 30, 31, SW_RW> {}; // Fast (Re) Synchronization Jump Width
		
		typedef FieldSet<TDCO, TDC, FBRP, FTSEG1, FTSEG2, FSJW> ALL;
		
		M_CAN_FBTP(M_CAN<CONFIG> *_m_can) : Super(_m_can) { Init(); }
		M_CAN_FBTP(M_CAN<CONFIG> *_m_can, uint32_t value) : Super(_m_can, value) { Init(); }
		
		void Init()
		{
			this->SetName("M_CAN_FBTP"); this->SetDescription("Fast Bit Timing and Prescaler Register");
			
			TDCO  ::SetName("TDCO");   TDCO  ::SetDescription("Transceiver Delay Compensation Offset");
			TDC   ::SetName("TDC");    TDC   ::SetDescription("Transceiver Delay Compensation");
			FBRP  ::SetName("FBRP");   FBRP  ::SetDescription("Fast Baud Rate Prescaler");
			FTSEG1::SetName("FTSEG1"); FTSEG1::SetDescription("Fast time segment before sample point");
			FTSEG2::SetName("FTSEG2"); FTSEG2::SetDescription("Fast time segment after sample point");
			FSJW  ::SetName("FSJW");   FSJW  ::SetDescription("Fast (Re) Synchronization Jump Width");
		}
		
		void Reset()
		{
			this->Initialize(0x00000a33);
		}
		
		virtual ReadWriteStatus Write(sc_core::sc_time& time_stamp, const uint32_t& value, const uint32_t& bit_enable)
		{
			return this->m_can->CheckProtectedWrite() ? Super::Write(time_stamp, value, bit_enable) : RWS_WROR;
		}

		using Super::operator =;
	};
	
	// Test Register (M_CAN_TEST)
	struct M_CAN_TEST : M_CAN_Register<M_CAN_TEST, SW_RW>
	{
		typedef M_CAN_Register<M_CAN_TEST, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x10;
		
		struct TDCV : Field<M_CAN_TEST, TDCV, 18, 23, SW_R > {}; // Transceiver Delay Compensation Value
		struct RX   : Field<M_CAN_TEST, RX  , 24, 24, SW_R > {}; // Receive Pin
		struct TX   : Field<M_CAN_TEST, TX  , 25, 26, SW_RW> {}; // Control of Transmit Pin
		struct LBCK : Field<M_CAN_TEST, LBCK, 27, 27, SW_RW> {}; // Loop Back mode

		typedef FieldSet<TDCV, RX, TX, LBCK> ALL;
		
		M_CAN_TEST(M_CAN<CONFIG> *_m_can) : Super(_m_can) { Init(); }
		M_CAN_TEST(M_CAN<CONFIG> *_m_can, uint32_t value) : Super(_m_can, value) { Init(); }
		
		void Init()
		{
			this->SetName("M_CAN_TEST"); this->SetDescription("Test Register");
			
			TDCV::SetName("TDCV"); TDCV::SetDescription("Transceiver Delay Compensation Value");
			RX  ::SetName("RX");   RX  ::SetDescription("Receive Pin");
			TX  ::SetName("TX");   TX  ::SetDescription("Control of Transmit Pin");
			LBCK::SetName("LBCK"); LBCK::SetDescription("Loop Back mode");
		}
		
		void Reset()
		{
			this->Initialize(0x0);
		}
		
		virtual ReadWriteStatus Write(sc_core::sc_time& time_stamp, const uint32_t& value, const uint32_t& bit_enable)
		{
			return this->m_can->CheckProtectedWrite() ? Super::Write(time_stamp, value, bit_enable) : RWS_WROR;
		}

		using Super::operator =;
	};
	
	// RAM Watchdog Register (M_CAN_RWD)
	struct M_CAN_RWD : M_CAN_Register<M_CAN_RWD, SW_RW>
	{
		typedef M_CAN_Register<M_CAN_RWD, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x14;
		
		struct WDV : Field<M_CAN_RWD, WDV, 16, 23, SW_R > {}; // Watchdog Value
		struct WDC : Field<M_CAN_RWD, WDC, 24, 31, SW_RW> {}; // Watchdog Configuration

		typedef FieldSet<WDV, WDC> ALL;
		
		M_CAN_RWD(M_CAN<CONFIG> *_m_can) : Super(_m_can) { Init(); }
		M_CAN_RWD(M_CAN<CONFIG> *_m_can, uint32_t value) : Super(_m_can, value) { Init(); }
		
		void Init()
		{
			this->SetName("M_CAN_RWD"); this->SetDescription("RAM Watchdog Register");
			
			WDV::SetName("WDV"); WDV::SetDescription("Watchdog Value");
			WDC::SetName("WDC"); WDC::SetDescription("Watchdog Configuration");
		}
		
		void Reset()
		{
			this->Initialize(0x0);
		}
		
		virtual ReadWriteStatus Write(sc_core::sc_time& time_stamp, const uint32_t& value, const uint32_t& bit_enable)
		{
			return this->m_can->CheckProtectedWrite() ? Super::Write(time_stamp, value, bit_enable) : RWS_WROR;
		}

		using Super::operator =;
	};
	
	// CC Control Register (M_CAN_CCCR)
	struct M_CAN_CCCR : M_CAN_Register<M_CAN_CCCR, SW_RW>
	{
		typedef M_CAN_Register<M_CAN_CCCR, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x18;
		
		struct TXP  : Field<M_CAN_CCCR, TXP , 17, 17, SW_RW> {}; // Transmit Pause
		struct FDBS : Field<M_CAN_CCCR, FDBS, 18, 18, SW_RW> {}; // CAN FD Bit Rate Switching
		struct FDO  : Field<M_CAN_CCCR, FDO , 19, 19, SW_RW> {}; // Fast Frame Mode Active
		struct CMR  : Field<M_CAN_CCCR, CMR , 20, 21, SW_RW> {}; // CAN Mode Request
		struct CME  : Field<M_CAN_CCCR, CME , 22, 20, SW_RW> {}; // CAN Mode Enable
		struct TEST : Field<M_CAN_CCCR, TEST, 24, 24, SW_RW> {}; // Test Enable Mode
		struct DAR  : Field<M_CAN_CCCR, DAR , 25, 25, SW_RW> {}; // DAR: Disable Automatic Retransmission
		struct MON  : Field<M_CAN_CCCR, MON , 26, 26, SW_RW> {}; // Bus Monitoring Mode
		struct CSR  : Field<M_CAN_CCCR, CSR , 27, 27, SW_RW> {}; // Clock Stop Request
		struct CSA  : Field<M_CAN_CCCR, CSA , 28, 28, SW_RW> {}; // Clock Stop Acknowledge
		struct ASM  : Field<M_CAN_CCCR, ASM , 29, 29, SW_RW> {}; // ASM Restricted Operation Mode
		struct CCE  : Field<M_CAN_CCCR, CCE , 30, 30, SW_RW> {}; // Configuration Change Enable
		struct INIT : Field<M_CAN_CCCR, INIT, 31, 31, SW_RW> {}; // Initialization
		
		typedef FieldSet<TXP, FDBS, FDO, CMR, CME, TEST, DAR, MON, CSR, CSA, ASM, CCE, INIT> ALL;
		
		typedef FieldSet<CME, TEST, DAR, MON, ASM> PROTECTED_WRITE_FIELDS; 
		
		M_CAN_CCCR(M_CAN<CONFIG> *_m_can) : Super(_m_can) { Init(); }
		M_CAN_CCCR(M_CAN<CONFIG> *_m_can, uint32_t value) : Super(_m_can, value) { Init(); }
		
		void Init()
		{
			this->SetName("M_CAN_CCCR"); this->SetDescription("CC Control Register");
			
			TXP ::SetName("TXP");  TXP ::SetDescription("Transmit Pause");
			FDBS::SetName("FDBS"); FDBS::SetDescription("CAN FD Bit Rate Switching");
			FDO ::SetName("FDO");  FDO ::SetDescription("Fast Frame Mode Active");
			CMR ::SetName("CMR");  CMR ::SetDescription("CAN Mode Request");
			CME ::SetName("CME");  CME ::SetDescription("CAN Mode Enable");
			TEST::SetName("TEST"); TEST::SetDescription("Test Enable Mode");
			DAR ::SetName("DAR");  DAR ::SetDescription("DAR: Disable Automatic Retransmission");
			MON ::SetName("MON");  MON ::SetDescription("Bus Monitoring Mode");
			CSR ::SetName("CSR");  CSR ::SetDescription("Clock Stop Request");
			CSA ::SetName("CSA");  CSA ::SetDescription("Clock Stop Acknowledge");
			ASM ::SetName("ASM");  ASM ::SetDescription("ASM Restricted Operation Mode");
			CCE ::SetName("CCE");  CCE ::SetDescription("Configuration Change Enable");
			INIT::SetName("INIT"); INIT::SetDescription("Initialization");
		}
		
		void Reset()
		{
			this->Initialize(0x00000001);
		}
		
		virtual ReadWriteStatus Write(sc_core::sc_time& time_stamp, const uint32_t& value, const uint32_t& bit_enable)
		{
			bool old_init = this->template Get<INIT>();
			bool old_test = this->template Get<TEST>();
			bool old_cce = this->template Get<CCE>();
			
			ReadWriteStatus rws = Super::template WritePreserve<PROTECTED_WRITE_FIELDS>(value, bit_enable);
			
			bool new_init = this->template Get<INIT>();
			bool new_test = this->template Get<TEST>();
			bool new_cce = this->template Get<CCE>();
			
			if(!old_init && new_init)
			{
				// request to enter initialization
				this->m_can->RequestInit();
				this->template Set<INIT>(old_init);
			}
			else if(old_init && !new_init)
			{
				// request to leave initialization
				this->m_can->RequestStart();
				this->template Set<INIT>(old_init);
			}
			
			if(old_test && !new_test)
			{
				this->m_can->m_can_test.Reset();
			}
			
			if(!old_cce && new_cce)
			{
				this->m_can->m_can_hpms.Reset();
				this->m_can->m_can_rxf0s.Reset();
				this->m_can->m_can_rxf1s.Reset();
				this->m_can->m_can_txfqs.Reset();
				this->m_can->m_can_txbrp.Reset();
				this->m_can->m_can_txbto.Reset();
				this->m_can->m_can_txbcf.Reset();
				this->m_can->m_can_txefs.Reset();
			}
			
			return rws;
		}
		
		using Super::operator =;
	};
	
	// Bit Timing and Prescaler Register (M_CAN_BTP)
	struct M_CAN_BTP : M_CAN_Register<M_CAN_BTP, SW_RW>
	{
		typedef M_CAN_Register<M_CAN_BTP, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x1c;
		
		struct BRP   : Field<M_CAN_BTP, BRP  , 6 , 15, SW_RW> {}; // Baud Rate Prescaler
		struct TSEG1 : Field<M_CAN_BTP, TSEG1, 18, 23, SW_RW> {}; // Time segment before sample point
		struct TSEG2 : Field<M_CAN_BTP, TSEG2, 24, 27, SW_RW> {}; // Time segment after sample point
		struct SJW   : Field<M_CAN_BTP, SJW  , 28, 31, SW_RW> {}; // (Re) Synchronization Jump Width
		
		typedef FieldSet<BRP, TSEG1, TSEG2, SJW> ALL;
		
		M_CAN_BTP(M_CAN<CONFIG> *_m_can) : Super(_m_can), can_clock_period(), sample_point(), phase_seg2(), bit_time() { Init(); }
		M_CAN_BTP(M_CAN<CONFIG> *_m_can, uint32_t value) : Super(_m_can, value), can_clock_period(), sample_point(), phase_seg2(), bit_time() { Init(); }
		
		void Init()
		{
			this->SetName("M_CAN_BTP"); this->SetDescription("Bit Timing and Prescaler Register");
			
			BRP  ::SetName("BRP");   BRP  ::SetDescription("Baud Rate Prescaler");
			TSEG1::SetName("TSEG1"); TSEG1::SetDescription("Time segment before sample point");
			TSEG2::SetName("TSEG2"); TSEG2::SetDescription("Time segment after sample point");
			SJW  ::SetName("SJW");   SJW  ::SetDescription("(Re) Synchronization Jump Width");
		}
		
		void Reset()
		{
			this->Initialize(0x00000a33);
		}
		
		virtual ReadWriteStatus Write(sc_core::sc_time& time_stamp, const uint32_t& value, const uint32_t& bit_enable)
		{
			return this->m_can->CheckProtectedWrite() ? Super::Write(time_stamp, value, bit_enable) : RWS_WROR;
		}
		
		const sc_core::sc_time& GetSamplePoint() const
		{
			Update();
			
			return sample_point;
		}
		
		const sc_core::sc_time& GetPhaseSeg2() const
		{
			Update();
			
			return phase_seg2;
		}
		
		const sc_core::sc_time& GetBitTime() const
		{
			Update();
			
			return bit_time;
		}
		
		using Super::operator =;
		
	private:
		mutable sc_core::sc_time can_clock_period;
		mutable sc_core::sc_time sample_point;
		mutable sc_core::sc_time phase_seg2;
		mutable sc_core::sc_time bit_time;
		
		void Update() const
		{
			if(unlikely(can_clock_period != this->m_can->can_clock_period))
			{
				// protocol clock: 1 / f_P
				can_clock_period = this->m_can->can_clock_period;
				
				unsigned int brp = this->template Get<BRP>() + 1; // baud rate prescaler
				unsigned int sjw = this->template Get<SJW>();     // Sync_Seg?
				unsigned int tseg1 = this->template Get<TSEG1>(); // Prop_Seg + Phase_Seg1
				unsigned int tseg2 = this->template Get<TSEG2>(); // Phase_Seg2
				
				if(unlikely(tseg1 == 0))
				{
					this->m_can->logger << DebugWarning << DebugWarning << sc_core::sc_time_stamp() << ": TSEG1 should be non-zero" << EndDebugWarning;
				}
				
				// CAN time quantum: tq = BRP * 1 / f_P
				sc_core::sc_time tq(can_clock_period);
				tq *= brp;
				
				// sample point: Sync_Seg + Prop_Seg + Phase_Seg1
				sample_point = tq;
				sample_point *= (sjw + tseg1);
				
				// phase_seg2: Phase_Seg2
				phase_seg2 = tq;
				phase_seg2 *= tseg2;
				
				// bit time: Sync_Seg + Prop_Seg + Phase_Seg1 + Phase_Seg2
				bit_time = sample_point + phase_seg2;
			}
		}
	};
	
	// Timestamp Counter Configuration Register (M_CAN_TSCC)
	struct M_CAN_TSCC : M_CAN_Register<M_CAN_TSCC, SW_RW>
	{
		typedef M_CAN_Register<M_CAN_TSCC, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x20;
		
		struct TCP : Field<M_CAN_TSCC, TCP, 12, 15, SW_RW> {}; // Timestamp Counter Prescaler
		struct TSS : Field<M_CAN_TSCC, TSS, 30, 31, SW_RW> {}; // Timestamp Select
		
		typedef FieldSet<TCP, TSS> ALL;
		
		M_CAN_TSCC(M_CAN<CONFIG> *_m_can) : Super(_m_can) { Init(); }
		M_CAN_TSCC(M_CAN<CONFIG> *_m_can, uint32_t value) : Super(_m_can, value) { Init(); }
		
		void Init()
		{
			this->SetName("M_CAN_TSCC"); this->SetDescription("Timestamp Counter Configuration Register");
			
			TCP::SetName("TCP"); TCP::SetDescription("Timestamp Counter Prescaler");
			TSS::SetName("TSS"); TSS::SetDescription("Timestamp Select");
		}
		
		void Reset()
		{
			this->Initialize(0x0);
		}
		
		virtual ReadWriteStatus Write(sc_core::sc_time& time_stamp, const uint32_t& value, const uint32_t& bit_enable)
		{
			return this->m_can->CheckProtectedWrite() ? Super::Write(time_stamp, value, bit_enable) : RWS_WROR;
		}

		const sc_core::sc_time& GetTimerPeriod() const
		{
			Update();
			
			return timer_period;
		}
		
		using Super::operator =;
	private:
		mutable sc_core::sc_time timer_period;
		
		void Update() const
		{
			const sc_core::sc_time& bit_time = this->m_can->m_can_btp.GetBitTime();
			
			unsigned int tcp = this->template Get<TCP>() + 1;
			
			timer_period = bit_time;
			timer_period *= tcp;
		}
	};
	
	// Timestamp Counter Value Register (M_CAN_TSCV)
	struct M_CAN_TSCV : M_CAN_Register<M_CAN_TSCV, SW_RW>
	{
		typedef M_CAN_Register<M_CAN_TSCV, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x24;
		
		struct TSC : Field<M_CAN_TSCV, TSC, 16, 31, SW_R_W1C> {}; // Timestamp Counter
		
		typedef FieldSet<TSC> ALL;
		
		M_CAN_TSCV(M_CAN<CONFIG> *_m_can) : Super(_m_can) { Init(); }
		M_CAN_TSCV(M_CAN<CONFIG> *_m_can, uint32_t value) : Super(_m_can, value) { Init(); }
		
		void Init()
		{
			this->SetName("M_CAN_TSCV"); this->SetDescription("Timestamp Counter Value Register");
			
			TSC::SetName("TSC"); TSC::SetDescription("Timestamp Counter");
		}
		
		void Reset()
		{
			this->Initialize(0x0);
		}
		
		using Super::operator =;
	};
	
	// Timeout Counter Configuration Register (M_CAN_TOCC)
	struct M_CAN_TOCC : M_CAN_Register<M_CAN_TOCC, SW_RW>
	{
		typedef M_CAN_Register<M_CAN_TOCC, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x28;
		
		struct TOP  : Field<M_CAN_TOCC, TOP , 0 , 15, SW_RW> {}; // Timeout Period
		struct TOS  : Field<M_CAN_TOCC, TOS , 29, 30, SW_RW> {}; // Timeout Select
		struct ETOC : Field<M_CAN_TOCC, ETOC, 31, 31, SW_RW> {}; // Enable Timeout Counter
		
		typedef FieldSet<TOP, TOS, ETOC> ALL;
		
		M_CAN_TOCC(M_CAN<CONFIG> *_m_can) : Super(_m_can) { Init(); }
		M_CAN_TOCC(M_CAN<CONFIG> *_m_can, uint32_t value) : Super(_m_can, value) { Init(); }
		
		void Init()
		{
			this->SetName("M_CAN_TOCC"); this->SetDescription("Timeout Counter Configuration Register");
			
			TOP ::SetName("TOP");  TOP ::SetDescription("Timeout Period");
			TOS ::SetName("TOS");  TOS ::SetDescription("Timeout Select");
			ETOC::SetName("ETOC"); ETOC::SetDescription("Enable Timeout Counter");
		}
		
		void Reset()
		{
			this->Initialize(0xffff0000);
		}
		
		virtual ReadWriteStatus Write(sc_core::sc_time& time_stamp, const uint32_t& value, const uint32_t& bit_enable)
		{
			return this->m_can->CheckProtectedWrite() ? Super::Write(time_stamp, value, bit_enable) : RWS_WROR;
		}
	};
	
	// Timeout Counter Value Register (M_CAN_TOCV)
	struct M_CAN_TOCV : M_CAN_Register<M_CAN_TOCV, SW_RW>
	{
		typedef M_CAN_Register<M_CAN_TOCV, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x2c;
		
		struct TOC : Field<M_CAN_TOCV, TOC, 16, 31, SW_RW> {}; // Timeout Counter
		
		typedef FieldSet<TOC> ALL;
		
		M_CAN_TOCV(M_CAN<CONFIG> *_m_can) : Super(_m_can) { Init(); }
		M_CAN_TOCV(M_CAN<CONFIG> *_m_can, uint32_t value) : Super(_m_can, value) { Init(); }
		
		void Init()
		{
			this->SetName("M_CAN_TOCV"); this->SetDescription("Timeout Counter Value Register");
			
			TOC::SetName("TOC"); TOC::SetDescription("Timeout Counter");
		}
		
		void Reset()
		{
			this->Initialize(0x0000ffff);
		}
		
		
		virtual ReadWriteStatus Write(sc_core::sc_time& time_stamp, const uint32_t& value, const uint32_t& bit_enable)
		{
			if(this->m_can->m_can_tocc.template Get<typename M_CAN_TOCC::TOS>() == TOS_CONTINOUS)
			{
				ReadWriteStatus rws = Super::Write(time_stamp, value, bit_enable);
			
				if(!IsReadWriteError(rws))
				{
					// When operating in Continuous mode, a write to TOCV presets the counter to the value configured by TOCC[TOP]
					uint32_t top = this->m_can->m_can_tocc.template Get<typename M_CAN_TOCC::TOP>();
					this->template Set<TOC>(top);
				}
				
				return rws;
			}
			
			return RWS_WROR;
		}
		
		using Super::operator =;
	};
	
	// Error Counter Register (M_CAN_ECR)
	struct M_CAN_ECR : M_CAN_Register<M_CAN_ECR, SW_R>
	{
		typedef M_CAN_Register<M_CAN_ECR, SW_R> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x40;
		
		struct CEL : Field<M_CAN_ECR, CEL, 8 , 15, SW_R> {}; // CAN Error Logging
		struct RP  : Field<M_CAN_ECR, RP , 16, 16, SW_R> {}; // Receive Error Passive
		struct REC : Field<M_CAN_ECR, REC, 17, 23, SW_R> {}; // Receive Error Counter
		struct TEC : Field<M_CAN_ECR, TEC, 24, 31, SW_R> {}; // Transmit Error Counter
		
		typedef FieldSet<RP, REC> RECEIVE_ERROR_COUNT;
		
		typedef FieldSet<CEL, RP, REC, TEC> ALL;
		
		M_CAN_ECR(M_CAN<CONFIG> *_m_can) : Super(_m_can) { Init(); }
		M_CAN_ECR(M_CAN<CONFIG> *_m_can, uint32_t value) : Super(_m_can, value) { Init(); }
		
		void Init()
		{
			this->SetName("M_CAN_ECR"); this->SetDescription("Error Counter Register");
			
			CEL::SetName("CEL"); CEL::SetDescription("CAN Error Logging");
			RP ::SetName("RP "); RP ::SetDescription("Receive Error Passive");
			REC::SetName("REC"); REC::SetDescription("Receive Error Counter");
			TEC::SetName("TEC"); TEC::SetDescription("Transmit Error Counter");
		}
		
		void Reset()
		{
			this->Initialize(0x0);
		}
		
		uint8_t GetTransmitErrorCount() const
		{
			return this->template Get<TEC>();
		}
		
		uint8_t GetReceiveErrorCount() const
		{
			return this->template Get<RECEIVE_ERROR_COUNT>();
		}
		
		void SetTransmitErrorCount(uint8_t v)
		{
			if(v > this->template Get<TEC>()) LogError();
			this->template Set<TEC>(v);
		}
		
		void SetReceiveErrorCount(uint8_t v)
		{
			if(v > this->template Get<RECEIVE_ERROR_COUNT>()) LogError();
			this->template Set<RECEIVE_ERROR_COUNT>(v);
		}
		
		void LogError()
		{
			uint32_t cel = this->template Get<CEL>();
			if(cel < CEL::template GetMask<uint32_t>())
			{
				cel = cel + 1;
				this->template Set<CEL>(cel);
				
				this->m_can->m_can_ir.template Set<typename M_CAN_IR::ELO>(1); // Error Logging Overflow
				this->m_can->UpdateInterrupts();
			}
		}

		using Super::operator =;
	};
	
	// Protocol Status Register (M_CAN_PSR)
	struct M_CAN_PSR : M_CAN_Register<M_CAN_PSR, SW_R>
	{
		typedef M_CAN_Register<M_CAN_PSR, SW_R> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x44;
		
		struct REDL : Field<M_CAN_PSR, REDL, 18, 18, SW_R> {}; // Received CAN FD Message with EDL flag
		struct RBRS : Field<M_CAN_PSR, RBRS, 19, 19, SW_R> {}; // BRS flag of last received CAN FD Message
		struct RESI : Field<M_CAN_PSR, RESI, 20, 20, SW_R> {}; // ESI CAN FD Message with ESI flag
		struct FLEC : Field<M_CAN_PSR, FLEC, 21, 23, SW_R> {}; // Fast Last Error Code
		struct BO   : Field<M_CAN_PSR, BO  , 24, 24, SW_R> {}; // Bus_Off Status
		struct EW   : Field<M_CAN_PSR, EW  , 25, 25, SW_R> {}; // Warning Status
		struct EP   : Field<M_CAN_PSR, EP  , 26, 26, SW_R> {}; // Error Passive
		struct ACT  : Field<M_CAN_PSR, ACT , 27, 28, SW_R> {}; // Activity
		struct LEC  : Field<M_CAN_PSR, LEC , 29, 31, SW_R> {}; // Last Error Code
		
		typedef FieldSet<REDL, RBRS, RESI, FLEC, BO, EW, EP, ACT, LEC> ALL; 
		
		M_CAN_PSR(M_CAN<CONFIG> *_m_can) : Super(_m_can) { Init(); }
		M_CAN_PSR(M_CAN<CONFIG> *_m_can, uint32_t value) : Super(_m_can, value) { Init(); }
		
		void Init()
		{
			this->SetName("M_CAN_PSR"); this->SetDescription("Protocol Status Register");
			
			REDL::SetName("REDL"); REDL::SetDescription("Received CAN FD Message with EDL flag");
			RBRS::SetName("RBRS"); RBRS::SetDescription("BRS flag of last received CAN FD Message");
			RESI::SetName("RESI"); RESI::SetDescription("ESI CAN FD Message with ESI flag");
			FLEC::SetName("FLEC"); FLEC::SetDescription("Fast Last Error Code");
			BO  ::SetName("BO");   BO  ::SetDescription("Bus_Off Status");
			EW  ::SetName("EW");   EW  ::SetDescription("Warning Status");
			EP  ::SetName("EP");   EP  ::SetDescription("Error Passive");
			ACT ::SetName("ACT");  ACT ::SetDescription("Activity");
			LEC ::SetName("LEC");  LEC ::SetDescription("Last Error Code");
		}
		
		void Reset()
		{
			this->Initialize(0x00000707);
		}
		
		virtual ReadWriteStatus Read(sc_core::sc_time& time_stamp, uint32_t& value, const uint32_t& bit_enable)
		{
			ReadWriteStatus rws = Super::Read(time_stamp, value, bit_enable);
			this->template Set<LEC>(LEC_NO_CHANGE);
			return rws;
		}
		
		using Super::operator =;
	};
	
	// Interrupt Register (M_CAN_IR)
	struct M_CAN_IR : M_CAN_Register<M_CAN_IR, SW_R_W1C>
	{
		typedef M_CAN_Register<M_CAN_IR, SW_R_W1C> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x50;
		
		struct STE  : Field<M_CAN_IR, STE , 0 , 0 , SW_R_W1C> {}; // Stuff Error
		struct FOE  : Field<M_CAN_IR, FOE , 1 , 1 , SW_R_W1C> {}; // Format Error
		struct ACKE : Field<M_CAN_IR, ACKE, 2 , 2 , SW_R_W1C> {}; // Acknowledge Error
		struct BE   : Field<M_CAN_IR, BE  , 3 , 3 , SW_R_W1C> {}; // Bit Error
		struct CRCE : Field<M_CAN_IR, CRCE, 4 , 4 , SW_R_W1C> {}; // CRC Error
		struct WDI  : Field<M_CAN_IR, WDI , 5 , 5 , SW_R_W1C> {}; // Watchdog Interrupt
		struct BO   : Field<M_CAN_IR, BO  , 6 , 6 , SW_R_W1C> {}; // Bus_Off Status
		struct EW   : Field<M_CAN_IR, EW  , 7 , 7 , SW_R_W1C> {}; // Warning Status
		struct EP   : Field<M_CAN_IR, EP  , 8 , 8 , SW_R_W1C> {}; // Error Passive
		struct ELO  : Field<M_CAN_IR, ELO , 9 , 9 , SW_R_W1C> {}; // Error Logging Overflow
		struct BEU  : Field<M_CAN_IR, BEU , 10, 10, SW_R_W1C> {}; // Bit Error Uncorrected
		struct BEC  : Field<M_CAN_IR, BEC , 11, 11, SW_R_W1C> {}; // Bit Error Corrected 
		struct DRX  : Field<M_CAN_IR, DRX , 12, 12, SW_R_W1C> {}; // Message stored to Dedicated Rx Buffer
		struct TOO  : Field<M_CAN_IR, TOO , 13, 13, SW_R_W1C> {}; // Timeout Occurred
		struct MRAF : Field<M_CAN_IR, MRAF, 14, 14, SW_R_W1C> {}; // Message RAM Access Failure
		struct TSW  : Field<M_CAN_IR, TSW , 15, 15, SW_R_W1C> {}; // Timestamp Wraparound
		struct TEFL : Field<M_CAN_IR, TEFL, 16, 16, SW_R_W1C> {}; // Tx Event FIFO Event Lost
		struct TEFF : Field<M_CAN_IR, TEFF, 17, 17, SW_R_W1C> {}; // Tx Event FIFO Full
		struct TEFW : Field<M_CAN_IR, TEFW, 18, 18, SW_R_W1C> {}; // Tx Event FIFO Watermark Reached
		struct TEFN : Field<M_CAN_IR, TEFN, 19, 19, SW_R_W1C> {}; // Tx Event FIDO New Entry
		struct TFE  : Field<M_CAN_IR, TFE , 20, 20, SW_R_W1C> {}; // Tx FIFO Empty
		struct TCF  : Field<M_CAN_IR, TCF , 21, 21, SW_R_W1C> {}; // Transmission Cancellation Finished
		struct TC   : Field<M_CAN_IR, TC  , 22, 22, SW_R_W1C> {}; // Transmission Completed
		struct HPM  : Field<M_CAN_IR, HPM , 23, 23, SW_R_W1C> {}; // High Priority Message
		struct RF1L : Field<M_CAN_IR, RF1L, 24, 24, SW_R_W1C> {}; // Rx FIFO 1 Message Lost
		struct RF1F : Field<M_CAN_IR, RF1F, 25, 25, SW_R_W1C> {}; // Rx FIFO 1 Full
		struct RF1W : Field<M_CAN_IR, RF1W, 26, 26, SW_R_W1C> {}; // Rx FIFO 1 Watermark Reached
		struct RF1N : Field<M_CAN_IR, RF1N, 27, 27, SW_R_W1C> {}; // Rx FIFO 1 New Message
		struct RF0L : Field<M_CAN_IR, RF0L, 28, 28, SW_R_W1C> {}; // Rx FIFO 0 Message Lost
		struct RF0F : Field<M_CAN_IR, RF0F, 29, 29, SW_R_W1C> {}; // Rx FIFO 0 Full
		struct RF0W : Field<M_CAN_IR, RF0W, 30, 30, SW_R_W1C> {}; // Rx FIFO 0 Watermark Reached
		struct RF0N : Field<M_CAN_IR, RF0N, 31, 31, SW_R_W1C> {}; // Rx FIFO 0 New Message
		
		typedef FieldSet<STE, FOE, ACKE, BE, CRCE, WDI, BO, EW, EP, ELO, BEU, BEC, DRX, TOO, MRAF, TSW, TEFL,
		                 TEFF, TEFW, TEFN, TFE, TCF, TC, HPM, RF1L, RF1F, RF1W, RF1N, RF0L, RF0F, RF0W, RF0N> ALL;
		
		M_CAN_IR(M_CAN<CONFIG> *_m_can) : Super(_m_can) { Init(); }
		M_CAN_IR(M_CAN<CONFIG> *_m_can, uint32_t value) : Super(_m_can, value) { Init(); }
		
		void Init()
		{
			this->SetName("M_CAN_IR"); this->SetDescription("Interrupt Register");
			
			STE ::SetName("STE");  STE ::SetDescription("Stuff Error");
			FOE ::SetName("FOE");  FOE ::SetDescription("Format Error");
			ACKE::SetName("ACKE"); ACKE::SetDescription("Acknowledge Error");
			BE  ::SetName("BE");   BE  ::SetDescription("Bit Error");
			CRCE::SetName("CRCE"); CRCE::SetDescription("CRC Error");
			WDI ::SetName("WDI");  WDI ::SetDescription("Watchdog Interrupt");
			BO  ::SetName("BO");   BO  ::SetDescription("Bus_Off Status");
			EW  ::SetName("EW");   EW  ::SetDescription("Warning Status");
			EP  ::SetName("EP");   EP  ::SetDescription("Error Passive");
			ELO ::SetName("ELO");  ELO ::SetDescription("Error Logging Overflow");
			BEU ::SetName("BEU");  BEU ::SetDescription("Bit Error Uncorrected");
			BEC ::SetName("BEC");  BEC ::SetDescription("Bit Error Corrected ");
			DRX ::SetName("DRX");  DRX ::SetDescription("Message stored to Dedicated Rx Buffer");
			TOO ::SetName("TOO");  TOO ::SetDescription("Timeout Occurred");
			MRAF::SetName("MRAF"); MRAF::SetDescription("Message RAM Access Failure");
			TSW ::SetName("TSW");  TSW ::SetDescription("Timestamp Wraparound");
			TEFL::SetName("TEFL"); TEFL::SetDescription("Tx Event FIFO Event Lost");
			TEFF::SetName("TEFF"); TEFF::SetDescription("Tx Event FIFO Full");
			TEFW::SetName("TEFW"); TEFW::SetDescription("Tx Event FIFO Watermark Reached");
			TEFN::SetName("TEFN"); TEFN::SetDescription("Tx Event FIDO New Entry");
			TFE ::SetName("TFE");  TFE ::SetDescription("Tx FIFO Empty");
			TCF ::SetName("TCF");  TCF ::SetDescription("Transmission Cancellation Finished");
			TC  ::SetName("TC");   TC  ::SetDescription("Transmission Completed");
			HPM ::SetName("HPM");  HPM ::SetDescription("High Priority Message");
			RF1L::SetName("RF1L"); RF1L::SetDescription("Rx FIFO 1 Message Lost");
			RF1F::SetName("RF1F"); RF1F::SetDescription("Rx FIFO 1 Full");
			RF1W::SetName("RF1W"); RF1W::SetDescription("Rx FIFO 1 Watermark Reached");
			RF1N::SetName("RF1N"); RF1N::SetDescription("Rx FIFO 1 New Message");
			RF0L::SetName("RF0L"); RF0L::SetDescription("Rx FIFO 0 Message Lost");
			RF0F::SetName("RF0F"); RF0F::SetDescription("Rx FIFO 0 Full");
			RF0W::SetName("RF0W"); RF0W::SetDescription("Rx FIFO 0 Watermark Reached");
			RF0N::SetName("RF0N"); RF0N::SetDescription("Rx FIFO 0 New Message");
		}
		
		void Reset()
		{
			this->Initialize(0x0);
		}
		
		virtual ReadWriteStatus Write(sc_core::sc_time& time_stamp, const uint32_t& value, const uint32_t& bit_enable)
		{
			ReadWriteStatus rws = Super::Write(time_stamp, value, bit_enable);
			
			if(!IsReadWriteError(rws))
			{
				this->m_can->m_can_txefs.template Set<typename M_CAN_TXEFS::TEFL>(this->template Get<TEFL>());
				this->m_can->UpdateInterrupts();
			}
			
			return rws;
		}
		
		using Super::operator =;
	};
	
	// Interrupt Enable Register (M_CAN_IE)
	struct M_CAN_IE : M_CAN_Register<M_CAN_IE, SW_RW>
	{
		typedef M_CAN_Register<M_CAN_IE, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x54;
		
		struct STEE  : Field<M_CAN_IE, STEE , 0 , 0 , SW_RW> {}; // Stuff Error Interrupt Enable
		struct FOEE  : Field<M_CAN_IE, FOEE , 1 , 1 , SW_RW> {}; // Format Error Interrupt Enable
		struct ACKEE : Field<M_CAN_IE, ACKEE, 2 , 2 , SW_RW> {}; // Acknowledge Error Interrupt Enable
		struct BEE   : Field<M_CAN_IE, BEE  , 3 , 3 , SW_RW> {}; // Bit Error Interrupt Enable
		struct CRCEE : Field<M_CAN_IE, CRCEE, 4 , 4 , SW_RW> {}; // CRC Error Interrupt Enable
		struct WDIE  : Field<M_CAN_IE, WDIE , 5 , 5 , SW_RW> {}; // Watchdog Interrupt Enable
		struct BOE   : Field<M_CAN_IE, BOE  , 6 , 6 , SW_RW> {}; // Bus_Off Status Interrupt Enable
		struct EWE   : Field<M_CAN_IE, EWE  , 7 , 7 , SW_RW> {}; // Warning Status Interrupt Enable
		struct EPE   : Field<M_CAN_IE, EPE  , 8 , 8 , SW_RW> {}; // Error Passive Interrupt Enable
		struct ELOE  : Field<M_CAN_IE, ELOE , 9 , 9 , SW_RW> {}; // Error Logging Overflow Interrupt Enable
		struct BEUE  : Field<M_CAN_IE, BEUE , 10, 10, SW_RW> {}; // Bit Error Uncorrected Interrupt Enable
		struct BECE  : Field<M_CAN_IE, BECE , 11, 11, SW_RW> {}; // Bit Error Corrected Interrupt Enable 
		struct DRXE  : Field<M_CAN_IE, DRXE , 12, 12, SW_RW> {}; // Message stored to Dedicated Rx Buffer Interrupt Enable
		struct TOOE  : Field<M_CAN_IE, TOOE , 13, 13, SW_RW> {}; // Timeout Occurred Interrupt Enable
		struct MRAFE : Field<M_CAN_IE, MRAFE, 14, 14, SW_RW> {}; // Message RAM Access Failure Interrupt Enable
		struct TSWE  : Field<M_CAN_IE, TSWE , 15, 15, SW_RW> {}; // Timestamp Wraparound Interrupt Enable
		struct TEFLE : Field<M_CAN_IE, TEFLE, 16, 16, SW_RW> {}; // Tx Event FIFO Event Lost Interrupt Enable
		struct TEFFE : Field<M_CAN_IE, TEFFE, 17, 17, SW_RW> {}; // Tx Event FIFO Full Interrupt Enable
		struct TEFWE : Field<M_CAN_IE, TEFWE, 18, 18, SW_RW> {}; // Tx Event FIFO Watermark Reached Interrupt Enable
		struct TEFNE : Field<M_CAN_IE, TEFNE, 19, 19, SW_RW> {}; // Tx Event FIDO New Entry Interrupt Enable
		struct TFEE  : Field<M_CAN_IE, TFEE , 20, 20, SW_RW> {}; // Tx FIFO Empty Interrupt Enable
		struct TCFE  : Field<M_CAN_IE, TCFE , 21, 21, SW_RW> {}; // Transmission Cancellation Finished Interrupt Enable
		struct TCE   : Field<M_CAN_IE, TCE  , 22, 22, SW_RW> {}; // Transmission Completed Interrupt Enable
		struct HPME  : Field<M_CAN_IE, HPME , 23, 23, SW_RW> {}; // High Priority Message Interrupt Enable
		struct RF1LE : Field<M_CAN_IE, RF1LE, 24, 24, SW_RW> {}; // Rx FIFO 1 Message Lost Interrupt Enable
		struct RF1FE : Field<M_CAN_IE, RF1FE, 25, 25, SW_RW> {}; // Rx FIFO 1 Full Interrupt Enable
		struct RF1WE : Field<M_CAN_IE, RF1WE, 26, 26, SW_RW> {}; // Rx FIFO 1 Watermark Reached Interrupt Enable
		struct RF1NE : Field<M_CAN_IE, RF1NE, 27, 27, SW_RW> {}; // Rx FIFO 1 New Message Interrupt Enable
		struct RF0LE : Field<M_CAN_IE, RF0LE, 28, 28, SW_RW> {}; // Rx FIFO 0 Message Lost Interrupt Enable
		struct RF0FE : Field<M_CAN_IE, RF0FE, 29, 29, SW_RW> {}; // Rx FIFO 0 Full Interrupt Enable
		struct RF0WE : Field<M_CAN_IE, RF0WE, 30, 30, SW_RW> {}; // Rx FIFO 0 Watermark Reached Interrupt Enable
		struct RF0NE : Field<M_CAN_IE, RF0NE, 31, 31, SW_RW> {}; // Rx FIFO 0 New Message Interrupt Enable
		
		typedef FieldSet<STEE, FOEE, ACKEE, BEE, CRCEE, WDIE, BOE, EWE, EPE, ELOE, BEUE, BECE,
		                 DRXE, TOOE, MRAFE, TSWE, TEFLE, TEFFE, TEFWE, TEFNE, TFEE, TCFE, TCE,
		                 HPME, RF1LE, RF1FE, RF1WE, RF1NE, RF0LE, RF0FE, RF0WE, RF0NE> ALL;
		
		M_CAN_IE(M_CAN<CONFIG> *_m_can) : Super(_m_can) { Init(); }
		M_CAN_IE(M_CAN<CONFIG> *_m_can, uint32_t value) : Super(_m_can, value) { Init(); }
		
		void Init()
		{
			this->SetName("M_CAN_IE"); this->SetDescription("Interrupt Enable Register");
			
			STEE ::SetName("STEE");  STEE ::SetDescription("Stuff Error Interrupt Enable");
			FOEE ::SetName("FOEE");  FOEE ::SetDescription("Format Error Interrupt Enable");
			ACKEE::SetName("ACKEE"); ACKEE::SetDescription("Acknowledge Error Interrupt Enable");
			BEE  ::SetName("BEE");   BEE  ::SetDescription("Bit Error Interrupt Enable");
			CRCEE::SetName("CRCEE"); CRCEE::SetDescription("CRC Error Interrupt Enable");
			WDIE ::SetName("WDIE");  WDIE ::SetDescription("Watchdog Interrupt Enable");
			BOE  ::SetName("BOE");   BOE  ::SetDescription("Bus_Off Status Interrupt Enable");
			EWE  ::SetName("EWE");   EWE  ::SetDescription("Warning Status Interrupt Enable");
			EPE  ::SetName("EPE");   EPE  ::SetDescription("Error Passive Interrupt Enable");
			ELOE ::SetName("ELOE");  ELOE ::SetDescription("Error Logging Overflow Interrupt Enable");
			BEUE ::SetName("BEUE");  BEUE ::SetDescription("Bit Error Uncorrected Interrupt Enable");
			BECE ::SetName("BECE");  BECE ::SetDescription("Bit Error Corrected Interrupt Enable ");
			DRXE ::SetName("DRXE");  DRXE ::SetDescription("Message stored to Dedicated Rx Buffer Interrupt Enable");
			TOOE ::SetName("TOOE");  TOOE ::SetDescription("Timeout Occurred Interrupt Enable");
			MRAFE::SetName("MRAFE"); MRAFE::SetDescription("Message RAM Access Failure Interrupt Enable");
			TSWE ::SetName("TSWE");  TSWE ::SetDescription("Timestamp Wraparound Interrupt Enable");
			TEFLE::SetName("TEFLE"); TEFLE::SetDescription("Tx Event FIFO Event Lost Interrupt Enable");
			TEFFE::SetName("TEFFE"); TEFFE::SetDescription("Tx Event FIFO Full Interrupt Enable");
			TEFWE::SetName("TEFWE"); TEFWE::SetDescription("Tx Event FIFO Watermark Reached Interrupt Enable");
			TEFNE::SetName("TEFNE"); TEFNE::SetDescription("Tx Event FIDO New Entry Interrupt Enable");
			TFEE ::SetName("TFEE");  TFEE ::SetDescription("Tx FIFO Empty Interrupt Enable");
			TCFE ::SetName("TCFE");  TCFE ::SetDescription("Transmission Cancellation Finished Interrupt Enable");
			TCE  ::SetName("TCE");   TCE  ::SetDescription("Transmission Completed Interrupt Enable");
			HPME ::SetName("HPME");  HPME ::SetDescription("High Priority Message Interrupt Enable");
			RF1LE::SetName("RF1LE"); RF1LE::SetDescription("Rx FIFO 1 Message Lost Interrupt Enable");
			RF1FE::SetName("RF1FE"); RF1FE::SetDescription("Rx FIFO 1 Full Interrupt Enable");
			RF1WE::SetName("RF1WE"); RF1WE::SetDescription("Rx FIFO 1 Watermark Reached Interrupt Enable");
			RF1NE::SetName("RF1NE"); RF1NE::SetDescription("Rx FIFO 1 New Message Interrupt Enable");
			RF0LE::SetName("RF0LE"); RF0LE::SetDescription("Rx FIFO 0 Message Lost Interrupt Enable");
			RF0FE::SetName("RF0FE"); RF0FE::SetDescription("Rx FIFO 0 Full Interrupt Enable");
			RF0WE::SetName("RF0WE"); RF0WE::SetDescription("Rx FIFO 0 Watermark Reached Interrupt Enable");
			RF0NE::SetName("RF0NE"); RF0NE::SetDescription("Rx FIFO 0 New Message Interrupt Enable");
		}
		
		void Reset()
		{
			this->Initialize(0x0);
		}
		
		virtual ReadWriteStatus Write(sc_core::sc_time& time_stamp, const uint32_t& value, const uint32_t& bit_enable)
		{
			ReadWriteStatus rws = Super::Write(time_stamp, value, bit_enable);
			
			if(!IsReadWriteError(rws))
			{
				this->m_can->UpdateInterrupts();
			}
			
			return rws;
		}
		
		using Super::operator =;
	};
	
	// Interrupt Line Select Register (M_CAN_ILS)
	struct M_CAN_ILS : M_CAN_Register<M_CAN_ILS, SW_RW>
	{
		typedef M_CAN_Register<M_CAN_ILS, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x58;
		
		struct STEL  : Field<M_CAN_ILS, STEL , 0 , 0 , SW_RW> {}; // Stuff Error Interrupt Line
		struct FOEL  : Field<M_CAN_ILS, FOEL , 1 , 1 , SW_RW> {}; // Format Error Interrupt Line
		struct ACKEL : Field<M_CAN_ILS, ACKEL, 2 , 2 , SW_RW> {}; // Acknowledge Error Interrupt Line
		struct BEL   : Field<M_CAN_ILS, BEL  , 3 , 3 , SW_RW> {}; // Bit Error Interrupt Line
		struct CRCEL : Field<M_CAN_ILS, CRCEL, 4 , 4 , SW_RW> {}; // CRC Error Interrupt Line
		struct WDIL  : Field<M_CAN_ILS, WDIL , 5 , 5 , SW_RW> {}; // Watchdog Interrupt Line
		struct BOL   : Field<M_CAN_ILS, BOL  , 6 , 6 , SW_RW> {}; // Bus_Off Status Interrupt Line
		struct EWL   : Field<M_CAN_ILS, EWL  , 7 , 7 , SW_RW> {}; // Warning Status Interrupt Line
		struct EPL   : Field<M_CAN_ILS, EPL  , 8 , 8 , SW_RW> {}; // Error Passive Interrupt Line
		struct ELOL  : Field<M_CAN_ILS, ELOL , 9 , 9 , SW_RW> {}; // Error Logging Overflow Interrupt Line
		struct BEUL  : Field<M_CAN_ILS, BEUL , 10, 10, SW_RW> {}; // Bit Error Uncorrected Interrupt Line
		struct BECL  : Field<M_CAN_ILS, BECL , 11, 11, SW_RW> {}; // Bit Error Corrected Interrupt Line 
		struct DRXL  : Field<M_CAN_ILS, DRXL , 12, 12, SW_RW> {}; // Message stored to Dedicated Rx Buffer Interrupt Line
		struct TOOL  : Field<M_CAN_ILS, TOOL , 13, 13, SW_RW> {}; // Timeout Occurred Interrupt Line
		struct MRAFL : Field<M_CAN_ILS, MRAFL, 14, 14, SW_RW> {}; // Message RAM Access Failure Interrupt Line
		struct TSWL  : Field<M_CAN_ILS, TSWL , 15, 15, SW_RW> {}; // Timestamp Wraparound Interrupt Line
		struct TEFLL : Field<M_CAN_ILS, TEFLL, 16, 16, SW_RW> {}; // Tx Event FIFO Event Lost Interrupt Line
		struct TEFFL : Field<M_CAN_ILS, TEFFL, 17, 17, SW_RW> {}; // Tx Event FIFO Full Interrupt Line
		struct TEFWL : Field<M_CAN_ILS, TEFWL, 18, 18, SW_RW> {}; // Tx Event FIFO Watermark Reached Interrupt Line
		struct TEFNL : Field<M_CAN_ILS, TEFNL, 19, 19, SW_RW> {}; // Tx Event FIDO New Entry Interrupt Line
		struct TFEL  : Field<M_CAN_ILS, TFEL , 20, 20, SW_RW> {}; // Tx FIFO Empty Interrupt Line
		struct TCFL  : Field<M_CAN_ILS, TCFL , 21, 21, SW_RW> {}; // Transmission Cancellation Finished Interrupt Line
		struct TCL   : Field<M_CAN_ILS, TCL  , 22, 22, SW_RW> {}; // Transmission Completed Interrupt Line
		struct HPML  : Field<M_CAN_ILS, HPML , 23, 23, SW_RW> {}; // High Priority Message Interrupt Line
		struct RF1LL : Field<M_CAN_ILS, RF1LL, 24, 24, SW_RW> {}; // Rx FIFO 1 Message Lost Interrupt Line
		struct RF1FL : Field<M_CAN_ILS, RF1FL, 25, 25, SW_RW> {}; // Rx FIFO 1 Full Interrupt Line
		struct RF1WL : Field<M_CAN_ILS, RF1WL, 26, 26, SW_RW> {}; // Rx FIFO 1 Watermark Reached Interrupt Line
		struct RF1NL : Field<M_CAN_ILS, RF1NL, 27, 27, SW_RW> {}; // Rx FIFO 1 New Message Interrupt Line
		struct RF0LL : Field<M_CAN_ILS, RF0LL, 28, 28, SW_RW> {}; // Rx FIFO 0 Message Lost Interrupt Line
		struct RF0FL : Field<M_CAN_ILS, RF0FL, 29, 29, SW_RW> {}; // Rx FIFO 0 Full Interrupt Line
		struct RF0WL : Field<M_CAN_ILS, RF0WL, 30, 30, SW_RW> {}; // Rx FIFO 0 Watermark Reached Interrupt Line
		struct RF0NL : Field<M_CAN_ILS, RF0NL, 31, 31, SW_RW> {}; // Rx FIFO 0 New Message Interrupt Line
		
		typedef FieldSet<STEL, FOEL, ACKEL, BEL, CRCEL, WDIL, BOL, EWL, EPL, ELOL, BEUL, BECL,
		                 DRXL, TOOL, MRAFL, TSWL, TEFLL, TEFFL, TEFWL, TEFNL, TFEL, TCFL, TCL,
		                 HPML, RF1LL, RF1FL, RF1WL, RF1NL, RF0LL, RF0FL, RF0WL, RF0NL> ALL;
		
		M_CAN_ILS(M_CAN<CONFIG> *_m_can) : Super(_m_can) { Init(); }
		M_CAN_ILS(M_CAN<CONFIG> *_m_can, uint32_t value) : Super(_m_can, value) { Init(); }
		
		void Init()
		{
			this->SetName("M_CAN_ILS"); this->SetDescription("Interrupt Line Select Register");
			
			STEL ::SetName("STEL");  STEL ::SetDescription("Stuff Error Interrupt Line");
			FOEL ::SetName("FOEL");  FOEL ::SetDescription("Format Error Interrupt Line");
			ACKEL::SetName("ACKEL"); ACKEL::SetDescription("Acknowledge Error Interrupt Line");
			BEL  ::SetName("BEL");   BEL  ::SetDescription("Bit Error Interrupt Line");
			CRCEL::SetName("CRCEL"); CRCEL::SetDescription("CRC Error Interrupt Line");
			WDIL ::SetName("WDIL");  WDIL ::SetDescription("Watchdog Interrupt Line");
			BOL  ::SetName("BOL");   BOL  ::SetDescription("Bus_Off Status Interrupt Line");
			EWL  ::SetName("EWL");   EWL  ::SetDescription("Warning Status Interrupt Line");
			EPL  ::SetName("EPL");   EPL  ::SetDescription("Error Passive Interrupt Line");
			ELOL ::SetName("ELOL");  ELOL ::SetDescription("Error Logging Overflow Interrupt Line");
			BEUL ::SetName("BEUL");  BEUL ::SetDescription("Bit Error Uncorrected Interrupt Line");
			BECL ::SetName("BECL");  BECL ::SetDescription("Bit Error Corrected Interrupt Line ");
			DRXL ::SetName("DRXL");  DRXL ::SetDescription("Message stored to Dedicated Rx Buffer Interrupt Line");
			TOOL ::SetName("TOOL");  TOOL ::SetDescription("Timeout Occurred Interrupt Line");
			MRAFL::SetName("MRAFL"); MRAFL::SetDescription("Message RAM Access Failure Interrupt Line");
			TSWL ::SetName("TSWL");  TSWL ::SetDescription("Timestamp Wraparound Interrupt Line");
			TEFLL::SetName("TEFLL"); TEFLL::SetDescription("Tx Event FIFO Event Lost Interrupt Line");
			TEFFL::SetName("TEFFL"); TEFFL::SetDescription("Tx Event FIFO Full Interrupt Line");
			TEFWL::SetName("TEFWL"); TEFWL::SetDescription("Tx Event FIFO Watermark Reached Interrupt Line");
			TEFNL::SetName("TEFNL"); TEFNL::SetDescription("Tx Event FIDO New Entry Interrupt Line");
			TFEL ::SetName("TFEL");  TFEL ::SetDescription("Tx FIFO Empty Interrupt Line");
			TCFL ::SetName("TCFL");  TCFL ::SetDescription("Transmission Cancellation Finished Interrupt Line");
			TCL  ::SetName("TCL");   TCL  ::SetDescription("Transmission Completed Interrupt Line");
			HPML ::SetName("HPML");  HPML ::SetDescription("High Priority Message Interrupt Line");
			RF1LL::SetName("RF1LL"); RF1LL::SetDescription("Rx FIFO 1 Message Lost Interrupt Line");
			RF1FL::SetName("RF1FL"); RF1FL::SetDescription("Rx FIFO 1 Full Interrupt Line");
			RF1WL::SetName("RF1WL"); RF1WL::SetDescription("Rx FIFO 1 Watermark Reached Interrupt Line");
			RF1NL::SetName("RF1NL"); RF1NL::SetDescription("Rx FIFO 1 New Message Interrupt Line");
			RF0LL::SetName("RF0LL"); RF0LL::SetDescription("Rx FIFO 0 Message Lost Interrupt Line");
			RF0FL::SetName("RF0FL"); RF0FL::SetDescription("Rx FIFO 0 Full Interrupt Line");
			RF0WL::SetName("RF0WL"); RF0WL::SetDescription("Rx FIFO 0 Watermark Reached Interrupt Line");
			RF0NL::SetName("RF0NL"); RF0NL::SetDescription("Rx FIFO 0 New Message Interrupt Line");
		}
		
		void Reset()
		{
			this->Initialize(0x0);
		}
		
		virtual ReadWriteStatus Write(sc_core::sc_time& time_stamp, const uint32_t& value, const uint32_t& bit_enable)
		{
			ReadWriteStatus rws = Super::Write(time_stamp, value, bit_enable);
			
			if(!IsReadWriteError(rws))
			{
				this->m_can->UpdateInterrupts();
			}
			
			return rws;
		}

		using Super::operator =;
	};
	
	// Interrupt Line Enable Register (M_CAN_ILE)
	struct M_CAN_ILE : M_CAN_Register<M_CAN_ILE, SW_RW>
	{
		typedef M_CAN_Register<M_CAN_ILE, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x5c;
		
		struct EINT1 : Field<M_CAN_ILE, EINT1, 30, 30, SW_RW> {}; // Enable Interrupt Line 1
		struct EINT0 : Field<M_CAN_ILE, EINT0, 31, 31, SW_RW> {}; // Enable Interrupt Line 0
		
		typedef FieldSet<EINT1, EINT0> ALL;
		
		M_CAN_ILE(M_CAN<CONFIG> *_m_can) : Super(_m_can) { Init(); }
		M_CAN_ILE(M_CAN<CONFIG> *_m_can, uint32_t value) : Super(_m_can, value) { Init(); }
		
		void Init()
		{
			this->SetName("M_CAN_ILE"); this->SetDescription("Interrupt Line Enable Register");
			
			EINT1::SetName("EINT1"); EINT1::SetDescription("Enable Interrupt Line 1");
			EINT0::SetName("EINT0"); EINT0::SetDescription("Enable Interrupt Line 0");
		}
		
		void Reset()
		{
			this->Initialize(0x0);
		}
		
		virtual ReadWriteStatus Write(sc_core::sc_time& time_stamp, const uint32_t& value, const uint32_t& bit_enable)
		{
			ReadWriteStatus rws = Super::Write(time_stamp, value, bit_enable);
			
			if(!IsReadWriteError(rws))
			{
				this->m_can->UpdateInterrupts();
			}
			
			return rws;
		}

		using Super::operator =;
	};
	
	// Global Filter Configuration Register (M_CAN_GFC)
	struct M_CAN_GFC : M_CAN_Register<M_CAN_GFC, SW_RW>
	{
		typedef M_CAN_Register<M_CAN_GFC, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x80;
		
		struct ANFS : Field<M_CAN_GFC, ANFS, 26, 27, SW_RW> {}; // Accept Non-matching Frames Standard
		struct ANFE : Field<M_CAN_GFC, ANFE, 28, 29, SW_RW> {}; // Accept Non-matching Frames Extended
		struct RRFS : Field<M_CAN_GFC, RRFS, 30, 30, SW_RW> {}; // Reject Remote Frames Standard
		struct RRFE : Field<M_CAN_GFC, RRFE, 31, 31, SW_RW> {}; // Reject Remote Frames Extended
		
		typedef FieldSet<ANFS, ANFE, RRFS, RRFE> ALL;
		
		M_CAN_GFC(M_CAN<CONFIG> *_m_can) : Super(_m_can) { Init(); }
		M_CAN_GFC(M_CAN<CONFIG> *_m_can, uint32_t value) : Super(_m_can, value) { Init(); }
		
		void Init()
		{
			this->SetName("M_CAN_GFC"); this->SetDescription("Global Filter Configuration Register");
			
			ANFS::SetName("ANFS"); ANFS::SetDescription("Accept Non-matching Frames Standard");
			ANFE::SetName("ANFE"); ANFE::SetDescription("Accept Non-matching Frames Extended");
			RRFS::SetName("RRFS"); RRFS::SetDescription("Reject Remote Frames Standard");
			RRFE::SetName("RRFE"); RRFE::SetDescription("Reject Remote Frames Extended");
		}
		
		void Reset()
		{
			this->Initialize(0x0);
		}
		
		virtual ReadWriteStatus Write(sc_core::sc_time& time_stamp, const uint32_t& value, const uint32_t& bit_enable)
		{
			return this->m_can->CheckProtectedWrite() ? Super::Write(time_stamp, value, bit_enable) : RWS_WROR;
		}

		using Super::operator =;
	};
	
	// Standard ID Filter Configuration Register (M_CAN_SIDFC)
	struct M_CAN_SIDFC : M_CAN_Register<M_CAN_SIDFC, SW_RW>
	{
		typedef M_CAN_Register<M_CAN_SIDFC, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x84;
		
		struct LSS   : Field<M_CAN_SIDFC, LSS  , 8 ,15, SW_RW> {}; // List Size Standard
		struct FLSSA : Field<M_CAN_SIDFC, FLSSA, 16,29, SW_RW> {}; // Filter List Standard Start Address
		
		typedef FieldSet<LSS, FLSSA> ALL;
		
		M_CAN_SIDFC(M_CAN<CONFIG> *_m_can) : Super(_m_can) { Init(); }
		M_CAN_SIDFC(M_CAN<CONFIG> *_m_can, uint32_t value) : Super(_m_can, value) { Init(); }
		
		void Init()
		{
			this->SetName("M_CAN_SIDFC"); this->SetDescription("Standard ID Filter Configuration Register");
			
			LSS  ::SetName("LSS");   LSS  ::SetDescription("List Size Standard");
			FLSSA::SetName("FLSSA"); FLSSA::SetDescription("Filter List Standard Start Address");
		}
		
		void Reset()
		{
			this->Initialize(0x0);
		}
		
		virtual ReadWriteStatus Write(sc_core::sc_time& time_stamp, const uint32_t& value, const uint32_t& bit_enable)
		{
			return this->m_can->CheckProtectedWrite() ? Super::Write(time_stamp, value, bit_enable) : RWS_WROR;
		}

		using Super::operator =;
	};
	
	// Extended ID Filter Configuration Register (M_CAN_XIDFC)
	struct M_CAN_XIDFC : M_CAN_Register<M_CAN_XIDFC, SW_RW>
	{
		typedef M_CAN_Register<M_CAN_XIDFC, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x88;
		
		struct LSE   : Field<M_CAN_XIDFC, LSE  , 9 ,15, SW_RW> {}; // List Size Extended
		struct FLESA : Field<M_CAN_XIDFC, FLESA, 16,29, SW_RW> {}; // Filter List Extended Start Address
		
		typedef FieldSet<LSE, FLESA> ALL;
		
		M_CAN_XIDFC(M_CAN<CONFIG> *_m_can) : Super(_m_can) { Init(); }
		M_CAN_XIDFC(M_CAN<CONFIG> *_m_can, uint32_t value) : Super(_m_can, value) { Init(); }
		
		void Init()
		{
			this->SetName("M_CAN_XIDFC"); this->SetDescription("Extended ID Filter Configuration Register");
			
			LSE  ::SetName("LSE");   LSE  ::SetDescription("List Size Extended");
			FLESA::SetName("FLESA"); FLESA::SetDescription("Filter List Extended Start Address");
		}
		
		void Reset()
		{
			this->Initialize(0x0);
		}
		
		virtual ReadWriteStatus Write(sc_core::sc_time& time_stamp, const uint32_t& value, const uint32_t& bit_enable)
		{
			return this->m_can->CheckProtectedWrite() ? Super::Write(time_stamp, value, bit_enable) : RWS_WROR;
		}

		using Super::operator =;
	};
	
	// Extended ID and Mask Register (M_CAN_XIDAM)
	struct M_CAN_XIDAM : M_CAN_Register<M_CAN_XIDAM, SW_RW>
	{
		typedef M_CAN_Register<M_CAN_XIDAM, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x90;
		
		struct EIDM : Field<M_CAN_XIDAM, EIDM, 3, 31, SW_RW> {}; // Extended ID Mask
		
		typedef FieldSet<EIDM> ALL;
		
		M_CAN_XIDAM(M_CAN<CONFIG> *_m_can) : Super(_m_can) { Init(); }
		M_CAN_XIDAM(M_CAN<CONFIG> *_m_can, uint32_t value) : Super(_m_can, value) { Init(); }
		
		void Init()
		{
			this->SetName("M_CAN_XIDAM"); this->SetDescription("Extended ID and Mask Register");
			
			EIDM::SetName("EIDM"); EIDM::SetDescription("Extended ID Mask");
		}
		
		void Reset()
		{
			this->Initialize(0x1fffffff);
		}
		
		virtual ReadWriteStatus Write(sc_core::sc_time& time_stamp, const uint32_t& value, const uint32_t& bit_enable)
		{
			return this->m_can->CheckProtectedWrite() ? Super::Write(time_stamp, value, bit_enable) : RWS_WROR;
		}

		using Super::operator =;
	};
	
	// High Priority Message Status Register (M_CAN_HPMS)
	struct M_CAN_HPMS : M_CAN_Register<M_CAN_HPMS, SW_R>
	{
		typedef M_CAN_Register<M_CAN_HPMS, SW_R> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x94;
		
		struct FLST : Field<M_CAN_HPMS, FLST, 16, 16, SW_R> {}; // Filter List
		struct FIDX : Field<M_CAN_HPMS, FIDX, 17, 23, SW_R> {}; // Filter Index
		struct MSI  : Field<M_CAN_HPMS, MSI , 24, 25, SW_R> {}; // Message Storage Indicator
		struct BIDX : Field<M_CAN_HPMS, BIDX, 26, 31, SW_R> {}; // Buffer Index
		
		typedef FieldSet<FLST, FIDX, MSI, BIDX> ALL;
		
		M_CAN_HPMS(M_CAN<CONFIG> *_m_can) : Super(_m_can) { Init(); }
		M_CAN_HPMS(M_CAN<CONFIG> *_m_can, uint32_t value) : Super(_m_can, value) { Init(); }
		
		void Init()
		{
			this->SetName("M_CAN_HPMS"); this->SetDescription("High Priority Message Status Register");
			
			FLST::SetName("FLST"); FLST::SetDescription("Filter List");
			FIDX::SetName("FIDX"); FIDX::SetDescription("Filter Index");
			MSI ::SetName("MSI");  MSI ::SetDescription("Message Storage Indicator");
			BIDX::SetName("BIDX"); BIDX::SetDescription("Buffer Index");
		}
		
		void Reset()
		{
			this->Initialize(0x0);
		}
		
		using Super::operator =;
	};
	
	// New Data 1 Register (M_CAN_NDAT1)
	struct M_CAN_NDAT1 : M_CAN_Register<M_CAN_NDAT1, SW_RW>
	{
		typedef M_CAN_Register<M_CAN_NDAT1, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x98;
		
		struct ND1 : Field<M_CAN_NDAT1, ND1, 0, 31, SW_R_W1C> {}; // New Data[0:31]
		
		typedef FieldSet<ND1> ALL;
		
		M_CAN_NDAT1(M_CAN<CONFIG> *_m_can) : Super(_m_can) { Init(); }
		M_CAN_NDAT1(M_CAN<CONFIG> *_m_can, uint32_t value) : Super(_m_can, value) { Init(); }
		
		void Init()
		{
			this->SetName("M_CAN_NDAT1"); this->SetDescription("New Data 1 Register");
			
			ND1::SetName("ND1"); ND1::SetDescription("New Data[0:31]");
		}
		
		void Reset()
		{
			this->Initialize(0x0);
		}
		
		void NewData(unsigned int i)
		{
			this->Set(Super::SIZE - 1 - i, 1);
		}

		using Super::operator =;
	};

	// New Data 2 Register (M_CAN_NDAT2)
	struct M_CAN_NDAT2 : M_CAN_Register<M_CAN_NDAT2, SW_RW>
	{
		typedef M_CAN_Register<M_CAN_NDAT2, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x9c;
		
		struct ND2 : Field<M_CAN_NDAT2, ND2, 0, 31, SW_R_W1C> {}; // New Data[32:63]
		
		typedef FieldSet<ND2> ALL;
		
		M_CAN_NDAT2(M_CAN<CONFIG> *_m_can) : Super(_m_can) { Init(); }
		M_CAN_NDAT2(M_CAN<CONFIG> *_m_can, uint32_t value) : Super(_m_can, value) { Init(); }
		
		void Init()
		{
			this->SetName("M_CAN_NDAT2"); this->SetDescription("New Data 2 Register");
			
			ND2::SetName("ND2"); ND2::SetDescription("New Data[32:63]");
		}
		
		void Reset()
		{
			this->Initialize(0x0);
		}
		
		void NewData(unsigned int i)
		{
			this->Set(Super::SIZE - 1 - i, 1);
		}

		using Super::operator =;
	};

	// Rx FIFO 0 Configuration (M_CAN_RXF0C)
	struct M_CAN_RXF0C : M_CAN_Register<M_CAN_RXF0C, SW_RW>
	{
		typedef M_CAN_Register<M_CAN_RXF0C, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0xa0;
		
		struct F0OM : Field<M_CAN_RXF0C, F0OM, 0 , 0 , SW_RW > {}; // FIFO 0 Operation Mode
		struct F0WM : Field<M_CAN_RXF0C, F0WM, 1 , 7 , SW_RW> {}; // Rx FIFO 0 Watermark
		struct F0S  : Field<M_CAN_RXF0C, F0S , 10, 15, SW_RW> {}; // Rx FIFO 0 Size
		struct F0SA : Field<M_CAN_RXF0C, F0SA, 16, 29, SW_RW> {}; // Rx FIFO 0 Start Address
		
		typedef FieldSet<F0OM, F0WM, F0S, F0SA> ALL;
		
		M_CAN_RXF0C(M_CAN<CONFIG> *_m_can) : Super(_m_can) { Init(); }
		M_CAN_RXF0C(M_CAN<CONFIG> *_m_can, uint32_t value) : Super(_m_can, value) { Init(); }
		
		void Init()
		{
			this->SetName("M_CAN_RXF0C"); this->SetDescription("Rx FIFO 0 Configuration");
			
			F0OM::SetName("F0OM"); F0OM::SetDescription("FIFO 0 Operation Mode");
			F0WM::SetName("F0WM"); F0WM::SetDescription("Rx FIFO 0 Watermark");
			F0S ::SetName("F0S");  F0S ::SetDescription("Rx FIFO 0 Size");
			F0SA::SetName("F0SA"); F0SA::SetDescription("Rx FIFO 0 Start Address");
		}
		
		void Reset()
		{
			this->Initialize(0x0);
		}
		
		virtual ReadWriteStatus Write(sc_core::sc_time& time_stamp, const uint32_t& value, const uint32_t& bit_enable)
		{
			return this->m_can->CheckProtectedWrite() ? Super::Write(time_stamp, value, bit_enable) : RWS_WROR;
		}

		using Super::operator =;
	};

	// Rx FIFO 0 Status Register (M_CAN_RXF0S)
	struct M_CAN_RXF0S : M_CAN_Register<M_CAN_RXF0S, SW_R>
	{
		typedef M_CAN_Register<M_CAN_RXF0S, SW_R> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0xa4;
		
		struct RF0L : Field<M_CAN_RXF0S, RF0L, 6 , 6 , SW_R> {}; // Rx FIFO 0 Message Lost
		struct F0F  : Field<M_CAN_RXF0S, F0F , 7 , 7 , SW_R> {}; // Rx FIFO 0 Full
		struct F0PI : Field<M_CAN_RXF0S, F0PI, 10, 15, SW_R> {}; // Rx FIFO 0 Put Index
		struct F0GI : Field<M_CAN_RXF0S, F0GI, 18, 23, SW_R> {}; // Rx FIFO 0 Get Index
		struct F0FL : Field<M_CAN_RXF0S, F0FL, 25, 31, SW_R> {}; // Rx FIFO 0 Fill Level
		
		typedef FieldSet<RF0L, F0F, F0PI, F0GI, F0FL> ALL;
		
		M_CAN_RXF0S(M_CAN<CONFIG> *_m_can) : Super(_m_can) { Init(); }
		M_CAN_RXF0S(M_CAN<CONFIG> *_m_can, uint32_t value) : Super(_m_can, value) { Init(); }
		
		void Init()
		{
			this->SetName("M_CAN_RXF0S"); this->SetDescription("Rx FIFO 0 Status Register");
			
			RF0L::SetName("RF0L"); RF0L::SetDescription("Rx FIFO 0 Message Lost");
			F0F ::SetName("F0F");  F0F ::SetDescription("Rx FIFO 0 Full");
			F0PI::SetName("F0PI"); F0PI::SetDescription("Rx FIFO 0 Put Index");
			F0GI::SetName("F0GI"); F0GI::SetDescription("Rx FIFO 0 Get Index");
			F0FL::SetName("F0FL"); F0FL::SetDescription("Rx FIFO 0 Fill Level");
		}
		
		void Reset()
		{
			this->Initialize(0x0);
		}
		
		using Super::operator =;
	};

	// Rx FIFO 0 Acknowledge Register (M_CAN_RXF0A)
	struct M_CAN_RXF0A : M_CAN_Register<M_CAN_RXF0A, SW_RW>
	{
		typedef M_CAN_Register<M_CAN_RXF0A, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0xa8;
		
		struct F0AI : Field<M_CAN_RXF0A, F0AI, 26, 31, SW_RW> {}; // Rx FIFO 0 Acknowledge Index
		
		typedef FieldSet<F0AI> ALL;
		
		M_CAN_RXF0A(M_CAN<CONFIG> *_m_can) : Super(_m_can) { Init(); }
		M_CAN_RXF0A(M_CAN<CONFIG> *_m_can, uint32_t value) : Super(_m_can, value) { Init(); }
		
		void Init()
		{
			this->SetName("M_CAN_RXF0A"); this->SetDescription("Rx FIFO 0 Acknowledge Register");
			
			F0AI::SetName("F0AI"); F0AI::SetDescription("Rx FIFO 0 Acknowledge Index");
		}
		
		void Reset()
		{
			this->Initialize(0x0);
		}
		
		virtual ReadWriteStatus Write(sc_core::sc_time& time_stamp, const uint32_t& value, const uint32_t& bit_enable)
		{
			ReadWriteStatus rws = Super::Write(time_stamp, value, bit_enable);
			
			if(!IsReadWriteError(rws))
			{
				this->m_can->AcknowledgeRxFIFO(0);
			}
			
			return rws;
		}

		using Super::operator =;
	};

	// Rx Buffer Configuration Register (M_CAN_RXBC)
	struct M_CAN_RXBC : M_CAN_Register<M_CAN_RXBC, SW_RW>
	{
		typedef M_CAN_Register<M_CAN_RXBC, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0xac;
		
		struct RBSA : Field<M_CAN_RXBC, RBSA, 16, 29, SW_RW> {}; // Rx Buffer Start Address
		
		typedef FieldSet<RBSA> ALL;
		
		M_CAN_RXBC(M_CAN<CONFIG> *_m_can) : Super(_m_can) { Init(); }
		M_CAN_RXBC(M_CAN<CONFIG> *_m_can, uint32_t value) : Super(_m_can, value) { Init(); }
		
		void Init()
		{
			this->SetName("M_CAN_RXBC"); this->SetDescription("Rx Buffer Configuration Register");
			
			RBSA::SetName("RBSA"); RBSA::SetDescription("Rx Buffer Start Address");
		}
		
		void Reset()
		{
			this->Initialize(0x0);
		}
		
		virtual ReadWriteStatus Write(sc_core::sc_time& time_stamp, const uint32_t& value, const uint32_t& bit_enable)
		{
			return this->m_can->CheckProtectedWrite() ? Super::Write(time_stamp, value, bit_enable) : RWS_WROR;
		}

		using Super::operator =;
	};

	// Rx FIFO 1 Configuration Register (M_CAN_RXF1C)
	struct M_CAN_RXF1C : M_CAN_Register<M_CAN_RXF1C, SW_RW>
	{
		typedef M_CAN_Register<M_CAN_RXF1C, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0xb0;
		
		struct F1OM : Field<M_CAN_RXF1C, F1OM, 0 , 0 , SW_RW> {}; // FIFO 1 Operation Mode
		struct F1WM : Field<M_CAN_RXF1C, F1WM, 1 , 7 , SW_RW> {}; // Rx FIFO 1 Watermark
		struct F1S  : Field<M_CAN_RXF1C, F1S , 9 , 15, SW_RW> {}; // Rx FIFO 1 Size
		struct F1SA : Field<M_CAN_RXF1C, F1SA, 16, 29, SW_RW> {}; // Rx FIFO 1 Start Address
		
		typedef FieldSet<F1OM, F1WM, F1S, F1SA> ALL;
		
		M_CAN_RXF1C(M_CAN<CONFIG> *_m_can) : Super(_m_can) { Init(); }
		M_CAN_RXF1C(M_CAN<CONFIG> *_m_can, uint32_t value) : Super(_m_can, value) { Init(); }
		
		void Init()
		{
			this->SetName("M_CAN_RXF1C"); this->SetDescription("Rx FIFO 1 Configuration Register");
			
			F1OM::SetName("F1OM"); F1OM::SetDescription("FIFO 1 Operation Mode");
			F1WM::SetName("F1WM"); F1WM::SetDescription("Rx FIFO 1 Watermark");
			F1S ::SetName("F1S");  F1S ::SetDescription("Rx FIFO 1 Size");
			F1SA::SetName("F1SA"); F1SA::SetDescription("Rx FIFO 1 Start Address");
		}
		
		void Reset()
		{
			this->Initialize(0x0);
		}
		
		virtual ReadWriteStatus Write(sc_core::sc_time& time_stamp, const uint32_t& value, const uint32_t& bit_enable)
		{
			return this->m_can->CheckProtectedWrite() ? Super::Write(time_stamp, value, bit_enable) : RWS_WROR;
		}

		using Super::operator =;
	};

	// Rx FIFO 1 Status Register (M_CAN_RXF1S)
	struct M_CAN_RXF1S : M_CAN_Register<M_CAN_RXF1S, SW_RW>
	{
		typedef M_CAN_Register<M_CAN_RXF1S, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0xb4;
		
		struct DMS  : Field<M_CAN_RXF1S, DMS , 0 , 1 , SW_RW> {}; // Debug Message Status
		struct RF1L : Field<M_CAN_RXF1S, RF1L, 6 , 6 , SW_RW> {}; // Rx FIFO 1 Message Lost
		struct F1F  : Field<M_CAN_RXF1S, F1F , 7 , 7 , SW_RW> {}; // Rx FIFO 1 Full
		struct F1PI : Field<M_CAN_RXF1S, F1PI, 10, 15, SW_RW> {}; // Rx FIFO 1 Put Index
		struct F1GI : Field<M_CAN_RXF1S, F1GI, 18, 23, SW_RW> {}; // Rx FIFO 1 Get Index
		struct F1FL : Field<M_CAN_RXF1S, F1FL, 25, 31, SW_RW> {}; // Rx FIFO 1 Fill Level
		
		typedef FieldSet<DMS, RF1L, F1F, F1PI, F1GI, F1FL> ALL;
		
		M_CAN_RXF1S(M_CAN<CONFIG> *_m_can) : Super(_m_can) { Init(); }
		M_CAN_RXF1S(M_CAN<CONFIG> *_m_can, uint32_t value) : Super(_m_can, value) { Init(); }
		
		void Init()
		{
			this->SetName("M_CAN_RXF1S"); this->SetDescription("Rx FIFO 1 Status Register");
			
			DMS ::SetName("DMS");  DMS ::SetDescription("Debug Message Status");
			RF1L::SetName("RF1L"); RF1L::SetDescription("Rx FIFO 1 Message Lost");
			F1F ::SetName("F1F");  F1F ::SetDescription("Rx FIFO 1 Full");
			F1PI::SetName("F1PI"); F1PI::SetDescription("Rx FIFO 1 Put Index");
			F1GI::SetName("F1GI"); F1GI::SetDescription("Rx FIFO 1 Get Index");
			F1FL::SetName("F1FL"); F1FL::SetDescription("Rx FIFO 1 Fill Level");
		}
		
		void Reset()
		{
			this->Initialize(0x0);
		}
		
		using Super::operator =;
	};

	// Rx FIFO 1 Acknowledge Register (M_CAN_RXF1A)
	struct M_CAN_RXF1A : M_CAN_Register<M_CAN_RXF1A, SW_RW>
	{
		typedef M_CAN_Register<M_CAN_RXF1A, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0xb8;
		
		struct F1AI : Field<M_CAN_RXF1A, F1AI, 26, 31, SW_RW> {}; // Rx FIFO 1 Acknowledge Index
		
		typedef FieldSet<F1AI> ALL;
		
		M_CAN_RXF1A(M_CAN<CONFIG> *_m_can) : Super(_m_can) { Init(); }
		M_CAN_RXF1A(M_CAN<CONFIG> *_m_can, uint32_t value) : Super(_m_can, value) { Init(); }
		
		void Init()
		{
			this->SetName("M_CAN_RXF1A"); this->SetDescription("Rx FIFO 1 Acknowledge Register");
			
			F1AI::SetName("F1AI"); F1AI::SetDescription("Rx FIFO 1 Acknowledge Index");
		}
		
		void Reset()
		{
			this->Initialize(0x0);
		}
		
		virtual ReadWriteStatus Write(sc_core::sc_time& time_stamp, const uint32_t& value, const uint32_t& bit_enable)
		{
			ReadWriteStatus rws = Super::Write(time_stamp, value, bit_enable);
			
			if(!IsReadWriteError(rws))
			{
				this->m_can->AcknowledgeRxFIFO(1);
			}
			
			return rws;
		}

		using Super::operator =;
	};

	// Rx Buffer / FIFO Element Size Configuration Register (M_CAN_RXESC)
	struct M_CAN_RXESC : M_CAN_Register<M_CAN_RXESC, SW_RW>
	{
		typedef M_CAN_Register<M_CAN_RXESC, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0xbc;
		
		struct RBDS : Field<M_CAN_RXESC, RBDS, 21, 23, SW_RW> {}; // Rx Buffer Data Field Size
		struct F1DS : Field<M_CAN_RXESC, F1DS, 25, 27, SW_RW> {}; // Rx FIFO 1 Data Field Size
		struct F0DS : Field<M_CAN_RXESC, F0DS, 29, 31, SW_RW> {}; // Rx FIFO 0 Data Field Size
		
		typedef FieldSet<RBDS, F1DS, F0DS> ALL;
		
		M_CAN_RXESC(M_CAN<CONFIG> *_m_can) : Super(_m_can) { Init(); }
		M_CAN_RXESC(M_CAN<CONFIG> *_m_can, uint32_t value) : Super(_m_can, value) { Init(); }
		
		void Init()
		{
			this->SetName("M_CAN_RXESC"); this->SetDescription("Rx Buffer / FIFO Element Size Configuration Register");
			
			RBDS::SetName("RBDS"); RBDS::SetDescription("Rx Buffer Data Field Size");
			F1DS::SetName("F1DS"); F1DS::SetDescription("Rx FIFO 1 Data Field Size");
			F0DS::SetName("F0DS"); F0DS::SetDescription("Rx FIFO 0 Data Field Size");
		}
		
		void Reset()
		{
			this->Initialize(0x0);
		}
		
		virtual ReadWriteStatus Write(sc_core::sc_time& time_stamp, const uint32_t& value, const uint32_t& bit_enable)
		{
			return this->m_can->CheckProtectedWrite() ? Super::Write(time_stamp, value, bit_enable) : RWS_WROR;
		}

		using Super::operator =;
	};

	// Tx Buffer Configuration Register (M_CAN_TXBC)
	struct M_CAN_TXBC : M_CAN_Register<M_CAN_TXBC, SW_RW>
	{
		typedef M_CAN_Register<M_CAN_TXBC, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0xc0;
		
		struct TQFM : Field<M_CAN_TXBC, TQFM, 1 , 1 , SW_RW> {}; // Tx FIFO/Queue Mode
		struct TQFS : Field<M_CAN_TXBC, TQFS, 2 , 7 , SW_RW> {}; // Tx FIFO/Queue Size
		struct NDTB : Field<M_CAN_TXBC, NDTB, 10, 15, SW_RW> {}; // Number of Dedicated Transmit Buffers
		struct TBSA : Field<M_CAN_TXBC, TBSA, 16, 29, SW_RW> {}; // Tx Buffers Start Address
		
		typedef FieldSet<TQFM, TQFS, NDTB, TBSA> ALL;
		
		M_CAN_TXBC(M_CAN<CONFIG> *_m_can) : Super(_m_can) { Init(); }
		M_CAN_TXBC(M_CAN<CONFIG> *_m_can, uint32_t value) : Super(_m_can, value) { Init(); }
		
		void Init()
		{
			this->SetName("M_CAN_TXBC"); this->SetDescription("Tx Buffer Configuration Register");
			
			TQFM::SetName("TQFM"); TQFM::SetDescription("Tx FIFO/Queue Mode");
			TQFS::SetName("TQFS"); TQFS::SetDescription("Tx FIFO/Queue Size");
			NDTB::SetName("NDTB"); NDTB::SetDescription("Number of Dedicated Transmit Buffers");
			TBSA::SetName("TBSA"); TBSA::SetDescription("Tx Buffers Start Address");
		}
		
		void Reset()
		{
			this->Initialize(0x0);
		}
		
		virtual ReadWriteStatus Write(sc_core::sc_time& time_stamp, const uint32_t& value, const uint32_t& bit_enable)
		{
			return this->m_can->CheckProtectedWrite() ? Super::Write(time_stamp, value, bit_enable) : RWS_WROR;
		}

		using Super::operator =;
	};

	// Tx FIFO/Queue Status Register (M_CAN_TXFQS)
	struct M_CAN_TXFQS : M_CAN_Register<M_CAN_TXFQS, SW_RW>
	{
		typedef M_CAN_Register<M_CAN_TXFQS, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0xc4;
		
		struct TFQF  : Field<M_CAN_TXFQS, TFQF , 10, 10, SW_RW> {}; // Tx FIFO/Queue Full
		struct TFQPI : Field<M_CAN_TXFQS, TFQPI, 11, 15, SW_RW> {}; // Tx FIFO/Queue Put Index
		struct TFGI  : Field<M_CAN_TXFQS, TFGI , 19, 23, SW_RW> {}; // Tx FIFO Get Index
		struct TFFL  : Field<M_CAN_TXFQS, TFFL , 26, 31, SW_RW> {}; // Tx FIFO Free Level
		
		typedef FieldSet<TFQF, TFQPI, TFGI, TFFL> ALL;
		
		M_CAN_TXFQS(M_CAN<CONFIG> *_m_can) : Super(_m_can) { Init(); }
		M_CAN_TXFQS(M_CAN<CONFIG> *_m_can, uint32_t value) : Super(_m_can, value) { Init(); }
		
		void Init()
		{
			this->SetName("M_CAN_TXFQS"); this->SetDescription("Tx FIFO/Queue Status Register");
			
			TFQF ::SetName("TFQF");  TFQF ::SetDescription("Tx FIFO/Queue Full");
			TFQPI::SetName("TFQPI"); TFQPI::SetDescription("Tx FIFO/Queue Put Index");
			TFGI ::SetName("TFGI");  TFGI ::SetDescription("Tx FIFO Get Index");
			TFFL ::SetName("TFFL");  TFFL ::SetDescription("Tx FIFO Free Level");
		}
		
		void Reset()
		{
			this->Initialize(0x0);
		}
		
		using Super::operator =;
	};

	// Tx Buffer Element Size Configuration (M_CAN_TXESC)
	struct M_CAN_TXESC : M_CAN_Register<M_CAN_TXESC, SW_RW>
	{
		typedef M_CAN_Register<M_CAN_TXESC, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0xc8;
		
		struct TBDS : Field<M_CAN_TXESC, TBDS, 29, 31, SW_RW> {}; // Tx Buffer Data Field Size
		
		typedef FieldSet<TBDS> ALL;
		
		M_CAN_TXESC(M_CAN<CONFIG> *_m_can) : Super(_m_can) { Init(); }
		M_CAN_TXESC(M_CAN<CONFIG> *_m_can, uint32_t value) : Super(_m_can, value) { Init(); }
		
		void Init()
		{
			this->SetName("M_CAN_TXESC"); this->SetDescription("Tx Buffer Element Size Configuration");
			
			TBDS::SetName("TBDS"); TBDS::SetDescription("Tx Buffer Data Field Size");
		}
		
		void Reset()
		{
			this->Initialize(0x0);
		}
		
		virtual ReadWriteStatus Write(sc_core::sc_time& time_stamp, const uint32_t& value, const uint32_t& bit_enable)
		{
			return this->m_can->CheckProtectedWrite() ? Super::Write(time_stamp, value, bit_enable) : RWS_WROR;
		}

		using Super::operator =;
	};

	// Tx Buffer Request Pending Register (M_CAN_TXBRP)
	struct M_CAN_TXBRP : M_CAN_Register<M_CAN_TXBRP, SW_R>
	{
		typedef M_CAN_Register<M_CAN_TXBRP, SW_R> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0xcc;
		
		struct TRP : Field<M_CAN_TXBRP, TRP, 0, 31, SW_R> {}; // Transmission Request Pending
		
		typedef FieldSet<TRP> ALL;
		
		M_CAN_TXBRP(M_CAN<CONFIG> *_m_can) : Super(_m_can) { Init(); }
		M_CAN_TXBRP(M_CAN<CONFIG> *_m_can, uint32_t value) : Super(_m_can, value) { Init(); }
		
		void Init()
		{
			this->SetName("M_CAN_TXBRP"); this->SetDescription("Tx Buffer Request Pending Register");
			
			TRP::SetName("TRP"); TRP::SetDescription("Transmission Request Pending");
		}
		
		void Reset()
		{
			this->Initialize(0x0);
		}
		
		void ClearRequest(unsigned int i)
		{
			this->template Set(Super::SIZE - 1 - i, 0);
		}
		
		using Super::operator =;
	};

	// Tx Buffer Add Request register (M_CAN_TXBAR)
	struct M_CAN_TXBAR : M_CAN_Register<M_CAN_TXBAR, SW_RW>
	{
		typedef M_CAN_Register<M_CAN_TXBAR, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0xd0;
		
		struct AR : Field<M_CAN_TXBAR, AR, 0, 31, SW_RW> {}; // Add Request
		
		typedef FieldSet<AR> ALL;
		
		M_CAN_TXBAR(M_CAN<CONFIG> *_m_can) : Super(_m_can) { Init(); }
		M_CAN_TXBAR(M_CAN<CONFIG> *_m_can, uint32_t value) : Super(_m_can, value) { Init(); }
		
		void Init()
		{
			this->SetName("M_CAN_TXBAR"); this->SetDescription("Tx Buffer Add Request register");
		}
		
		void Reset()
		{
			this->Initialize(0x0);
		}
		
		virtual ReadWriteStatus Write(sc_core::sc_time& time_stamp, const uint32_t& value, const uint32_t& bit_enable)
		{
			if(!this->m_can->m_can_cccr.template Get<typename M_CAN_CCCR::CCE>())
			{
				// TXBAR is only writable while CCR[CCE]=0
				// writing '0' has no impact.
				// TXBAR bits are set only for those TX buffers configured via TXBC
				// If an add request is applied for a Tx Buffer with pending
				// transmission request (corresponding TXBRP bit already set),
				// this add request is ignored.
				uint32_t old_value = this->Get();
				uint32_t new_value = old_value | (value & bit_enable & this->m_can->GetTxBuffersMask() & ~this->m_can->m_can_txbrp);
				
				ReadWriteStatus rws = Super::Write(time_stamp, new_value, bit_enable);
				
				if(!IsReadWriteError(rws))
				{
					this->m_can->AddTxRequests();
				}
				
				return rws;
			}
			
			return RWS_WROR;
		}
		
		void ClearAddRequest(unsigned int i)
		{
			this->Set(Super::SIZE - 1 - i, 0);
		}

		using Super::operator =;
	};

	// Tx Buffer Cancellation Request register (M_CAN_TXBCR)
	struct M_CAN_TXBCR : M_CAN_Register<M_CAN_TXBCR, SW_RW>
	{
		typedef M_CAN_Register<M_CAN_TXBCR, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0xd4;
		
		struct CR : Field<M_CAN_TXBCR, CR, 0, 31, SW_RW> {}; // Cancellation Request
		
		typedef FieldSet<CR> ALL;
		
		M_CAN_TXBCR(M_CAN<CONFIG> *_m_can) : Super(_m_can) { Init(); }
		M_CAN_TXBCR(M_CAN<CONFIG> *_m_can, uint32_t value) : Super(_m_can, value) { Init(); }
		
		void Init()
		{
			this->SetName("M_CAN_TXBCR"); this->SetDescription("Tx Buffer Cancellation Request register");
			
			CR::SetName("CR"); CR::SetDescription("Tx Buffer Cancellation Request register");
		}
		
		void Reset()
		{
			this->Initialize(0x0);
		}
		
		bool IsCancelled(unsigned int tx_buffer_element_index) const
		{
			return Super::Get(Super::SIZE - 1 - tx_buffer_element_index);
		}
		
		void ClearRequest(unsigned int tx_buffer_element_index)
		{
			Super::Set(Super::SIZE - 1 - tx_buffer_element_index, 0);
		}
		
		virtual ReadWriteStatus Write(sc_core::sc_time& time_stamp, const uint32_t& value, const uint32_t& bit_enable)
		{
			if(!this->m_can->m_can_cccr.template Get<typename M_CAN_CCCR::CCE>())
			{
				// TXBCR is only writable while CCR[CCE]=0
				// writing '0' has no impact.
				// TXBCR bits are set only for those TX buffers configured via TXBC
				uint32_t old_value = this->Get();
				uint32_t new_value = old_value | (value & bit_enable & this->m_can->GetTxBuffersMask());
				
				ReadWriteStatus rws = Super::Write(time_stamp, new_value, bit_enable);
				
				if(!IsReadWriteError(rws))
				{
					this->m_can->CancelTxRequests();
				}
				
				return rws;
			}
			
			return RWS_WROR;
		}
		
		using Super::operator =;
	};

	// Tx Buffer Transmission Occurred register (M_CAN_TXBTO)
	struct M_CAN_TXBTO : M_CAN_Register<M_CAN_TXBTO, SW_R>
	{
		typedef M_CAN_Register<M_CAN_TXBTO, SW_R> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0xd8;
		
		struct TO : Field<M_CAN_TXBTO, TO, 0, 31, SW_R> {}; // Transmission Occurred
		
		typedef FieldSet<TO> ALL;
		
		M_CAN_TXBTO(M_CAN<CONFIG> *_m_can) : Super(_m_can) { Init(); }
		M_CAN_TXBTO(M_CAN<CONFIG> *_m_can, uint32_t value) : Super(_m_can, value) { Init(); }
		
		void Init()
		{
			this->SetName("M_CAN_TXBTO"); this->SetDescription("Tx Buffer Transmission Occurred register");
			
			TO::SetName("TO"); TO::SetDescription("Transmission Occurred");
		}
		
		void Reset()
		{
			this->Initialize(0x0);
		}
		
		void TransmissionOccurred(unsigned int tx_buffer_element_index)
		{
			Super::Set(Super::SIZE - 1 - tx_buffer_element_index, 1);
		}
		
		using Super::operator =;
	};

	// Tx Buffer Cancellation Finished register (M_CAN_TXBCF)
	struct M_CAN_TXBCF : M_CAN_Register<M_CAN_TXBCF, SW_RW>
	{
		typedef M_CAN_Register<M_CAN_TXBCF, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0xdc;
		
		struct CF : Field<M_CAN_TXBCF, CF, 0, 31> {}; // Cancellation Finished
		
		typedef FieldSet<CF> ALL;
		
		M_CAN_TXBCF(M_CAN<CONFIG> *_m_can) : Super(_m_can) { Init(); }
		M_CAN_TXBCF(M_CAN<CONFIG> *_m_can, uint32_t value) : Super(_m_can, value) { Init(); }
		
		void Init()
		{
			this->SetName("M_CAN_TXBCF"); this->SetDescription("Tx Buffer Cancellation Finished register");
			
			CF::SetName("CF"); CF::SetDescription("Cancellation Finished");
		}
		
		void Reset()
		{
			this->Initialize(0x0);
		}
		
		void CancellationFinished(unsigned int tx_buffer_element_index)
		{
			Super::Set(Super::SIZE - 1 - tx_buffer_element_index, 1);
		}

		using Super::operator =;
	};

	// Tx Buffer Transmission Interrupt Enable register (M_CAN_TXBTIE)
	struct M_CAN_TXBTIE : M_CAN_Register<M_CAN_TXBTIE, SW_RW>
	{
		typedef M_CAN_Register<M_CAN_TXBTIE, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0xe0;
		
		struct TIE : Field<M_CAN_TXBTIE, TIE, 0, 31, SW_RW> {}; // Transmission Interrupt Enable
		
		typedef FieldSet<TIE> ALL;
		
		M_CAN_TXBTIE(M_CAN<CONFIG> *_m_can) : Super(_m_can) { Init(); }
		M_CAN_TXBTIE(M_CAN<CONFIG> *_m_can, uint32_t value) : Super(_m_can, value) { Init(); }
		
		void Init()
		{
			this->SetName("M_CAN_TXBTIE"); this->SetDescription("Tx Buffer Transmission Interrupt Enable register");
			
			TIE::SetName("TIE"); TIE::SetDescription("Transmission Interrupt Enable");
		}
		
		void Reset()
		{
			this->Initialize(0x0);
		}
		
		bool IsInterruptEnabled(unsigned int tx_buffer_element_index) const
		{
			return Super::Get(Super::SIZE - 1 - tx_buffer_element_index);
		}

		using Super::operator =;
	};

	// Tx Buffer Cancellation Finished Interrupt Enable register (M_CAN_TXBCIE)
	struct M_CAN_TXBCIE : M_CAN_Register<M_CAN_TXBCIE, SW_RW>
	{
		typedef M_CAN_Register<M_CAN_TXBCIE, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0xe4;
		
		struct CFIE : Field<M_CAN_TXBCIE, CFIE, 0, 31, SW_RW> {}; // Cancellation Finished Interrupt Enable
		
		typedef FieldSet<CFIE> ALL;
		
		M_CAN_TXBCIE(M_CAN<CONFIG> *_m_can) : Super(_m_can) { Init(); }
		M_CAN_TXBCIE(M_CAN<CONFIG> *_m_can, uint32_t value) : Super(_m_can, value) { Init(); }
		
		void Init()
		{
			this->SetName("M_CAN_TXBCIE"); this->SetDescription("Tx Buffer Cancellation Finished Interrupt Enable register");
			
			CFIE::SetName("CFIE"); CFIE::SetDescription("Cancellation Finished Interrupt Enable");
		}
		
		void Reset()
		{
			this->Initialize(0x0);
		}
		
		bool IsInterruptEnabled(unsigned int tx_buffer_element_index) const
		{
			return Super::Get(Super::SIZE - 1 - tx_buffer_element_index);
		}

		using Super::operator =;
	};

	// Tx Event FIFO Configuration Register (M_CAN_TXEFC)
	struct M_CAN_TXEFC : M_CAN_Register<M_CAN_TXEFC, SW_RW>
	{
		typedef M_CAN_Register<M_CAN_TXEFC, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0xf0;
		
		struct EFWM : Field<M_CAN_TXEFC, EFWM, 2 , 7 , SW_RW> {}; // Event FIFO Watermark
		struct EFS  : Field<M_CAN_TXEFC, EFS , 10, 15, SW_RW> {}; // Event FIFO Size
		struct EFSA : Field<M_CAN_TXEFC, EFSA, 16, 29, SW_RW> {}; // Event FIFO Start Address
		
		typedef FieldSet<EFWM, EFS, EFSA> ALL;
		
		M_CAN_TXEFC(M_CAN<CONFIG> *_m_can) : Super(_m_can) { Init(); }
		M_CAN_TXEFC(M_CAN<CONFIG> *_m_can, uint32_t value) : Super(_m_can, value) { Init(); }
		
		void Init()
		{
			this->SetName("M_CAN_TXEFC"); this->SetDescription("Tx Event FIFO Configuration Register");
			
			EFWM::SetName("EFWM"); EFWM::SetDescription("Event FIFO Watermark");
			EFS ::SetName("EFS");  EFS ::SetDescription("Event FIFO Size");
			EFSA::SetName("EFSA"); EFSA::SetDescription("Event FIFO Start Address");
		}
		
		void Reset()
		{
			this->Initialize(0x0);
		}
		
		virtual ReadWriteStatus Write(sc_core::sc_time& time_stamp, const uint32_t& value, const uint32_t& bit_enable)
		{
			return this->m_can->CheckProtectedWrite() ? Super::Write(time_stamp, value, bit_enable) : RWS_WROR;
		}

		using Super::operator =;
	};

	// Tx Event FIFO Status register (M_CAN_TXEFS)
	struct M_CAN_TXEFS : M_CAN_Register<M_CAN_TXEFS, SW_R>
	{
		typedef M_CAN_Register<M_CAN_TXEFS, SW_R> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0xf4;
		
		struct TEFL : Field<M_CAN_TXEFS, TEFL, 6 , 6 , SW_R> {}; // Tx Event FIFO Element Lost
		struct EFF  : Field<M_CAN_TXEFS, EFF , 7 , 7 , SW_R> {}; // Event FIFO Full
		struct EFPI : Field<M_CAN_TXEFS, EFPI, 11, 15, SW_R> {}; // Event FIFO Put Index
		struct EFGI : Field<M_CAN_TXEFS, EFGI, 19, 23, SW_R> {}; // Event FIFO Get Index
		struct EFFL : Field<M_CAN_TXEFS, EFFL, 26, 31, SW_R> {}; // Event FIFO Fill Level
		
		typedef FieldSet<TEFL, EFF, EFPI, EFGI, EFFL> ALL;
		
		M_CAN_TXEFS(M_CAN<CONFIG> *_m_can) : Super(_m_can) { Init(); }
		M_CAN_TXEFS(M_CAN<CONFIG> *_m_can, uint32_t value) : Super(_m_can, value) { Init(); }
		
		void Init()
		{
			this->SetName("M_CAN_TXEFS"); this->SetDescription("Tx Event FIFO Status register");
			
			TEFL::SetName("TEFL"); TEFL::SetDescription("Tx Event FIFO Element Lost");
			EFF ::SetName("EFF");  EFF ::SetDescription("Event FIFO Full");
			EFPI::SetName("EFPI"); EFPI::SetDescription("Event FIFO Put Index");
			EFGI::SetName("EFGI"); EFGI::SetDescription("Event FIFO Get Index");
			EFFL::SetName("EFFL"); EFFL::SetDescription("Event FIFO Fill Level");
		}
		
		void Reset()
		{
			this->Initialize(0x0);
		}
		
		using Super::operator =;
	};

	// Tx Event FIFO Acknowledge register (M_CAN_TXEFA)
	struct M_CAN_TXEFA : M_CAN_Register<M_CAN_TXEFA, SW_RW>
	{
		typedef M_CAN_Register<M_CAN_TXEFA, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0xf8;
		
		struct EFAI : Field<M_CAN_TXEFA, EFAI, 27, 31, SW_RW> {}; // Event FIFO Acknowledge Index
		
		typedef FieldSet<EFAI> ALL;
		
		M_CAN_TXEFA(M_CAN<CONFIG> *_m_can) : Super(_m_can) { Init(); }
		M_CAN_TXEFA(M_CAN<CONFIG> *_m_can, uint32_t value) : Super(_m_can, value) { Init(); }
		
		void Init()
		{
			this->SetName("M_CAN_TXEFA"); this->SetDescription("Tx Event FIFO Acknowledge register");
			
			EFAI::SetName("EFAI"); EFAI::SetDescription("Event FIFO Acknowledge Index");
		}
		
		void Reset()
		{
			this->Initialize(0x0);
		}
		
		virtual ReadWriteStatus Write(sc_core::sc_time& time_stamp, const uint32_t& value, const uint32_t& bit_enable)
		{
			ReadWriteStatus rws = Super::Write(time_stamp, value, bit_enable);
			
			if(!IsReadWriteError(rws))
			{
				this->m_can->AcknowledgeTxEventFIFO();
			}
			
			return rws;
		}
		
		using Super::operator =;
	};
	
	unisim::kernel::tlm2::ClockPropertiesProxy m_clk_prop_proxy;    // proxy to get clock properties from master clock port
	unisim::kernel::tlm2::ClockPropertiesProxy can_clk_prop_proxy; // proxy to get clock properties from CAN clock port

	// M_CAN registers
	M_CAN_CREL   m_can_crel;
	M_CAN_ENDN   m_can_endn;
	M_CAN_FBTP   m_can_fbtp;
	M_CAN_TEST   m_can_test;
	M_CAN_RWD    m_can_rwd;
	M_CAN_CCCR   m_can_cccr;
	M_CAN_BTP    m_can_btp;
	M_CAN_TSCC   m_can_tscc;
	M_CAN_TSCV   m_can_tscv;
	M_CAN_TOCC   m_can_tocc;
	M_CAN_TOCV   m_can_tocv;
	M_CAN_ECR    m_can_ecr;
	M_CAN_PSR    m_can_psr;
	M_CAN_IR     m_can_ir;
	M_CAN_IE     m_can_ie;
	M_CAN_ILS    m_can_ils;
	M_CAN_ILE    m_can_ile;
	M_CAN_GFC    m_can_gfc;
	M_CAN_SIDFC  m_can_sidfc;
	M_CAN_XIDFC  m_can_xidfc;
	M_CAN_XIDAM  m_can_xidam;
	M_CAN_HPMS   m_can_hpms;
	M_CAN_NDAT1  m_can_ndat1;
	M_CAN_NDAT2  m_can_ndat2;
	M_CAN_RXF0C  m_can_rxf0c;
	M_CAN_RXF0S  m_can_rxf0s;
	M_CAN_RXF0A  m_can_rxf0a;
	M_CAN_RXBC   m_can_rxbc;
	M_CAN_RXF1C  m_can_rxf1c;
	M_CAN_RXF1S  m_can_rxf1s;
	M_CAN_RXF1A  m_can_rxf1a;
	M_CAN_RXESC  m_can_rxesc;
	M_CAN_TXBC   m_can_txbc;
	M_CAN_TXFQS  m_can_txfqs;
	M_CAN_TXESC  m_can_txesc;
	M_CAN_TXBRP  m_can_txbrp;
	M_CAN_TXBAR  m_can_txbar;
	M_CAN_TXBCR  m_can_txbcr;
	M_CAN_TXBTO  m_can_txbto;
	M_CAN_TXBCF  m_can_txbcf;
	M_CAN_TXBTIE m_can_txbtie;
	M_CAN_TXBCIE m_can_txbcie;
	M_CAN_TXEFC  m_can_txefc;
	M_CAN_TXEFS  m_can_txefs;
	M_CAN_TXEFA  m_can_txefa;
	
	sc_core::sc_event gen_int0_event;
	sc_core::sc_event gen_int1_event;
	sc_core::sc_event gen_dma_req_event;
	
	// M_CAN registers address map
	RegisterAddressMap<sc_dt::uint64, sc_core::sc_time> reg_addr_map;

	unisim::kernel::tlm2::Schedule<Event> schedule;         // Payload (processor requests over AHB interface) schedule
	
	unisim::util::endian::endian_type endian;
	unisim::kernel::service::Parameter<unisim::util::endian::endian_type> param_endian;

	sc_core::sc_time master_clock_period;                 // Master clock period
	sc_core::sc_time master_clock_start_time;             // Master clock start time
	bool master_clock_posedge_first;                      // Master clock posedge first ?
	double master_clock_duty_cycle;                       // Master clock duty cycle

	sc_core::sc_time can_clock_period;                    // CAN clock period
	sc_core::sc_time can_clock_start_time;                // CAN clock start time
	bool can_clock_posedge_first;                         // CAN clock posedge first ?
	double can_clock_duty_cycle;                          // CAN clock duty cycle
	
	sc_core::sc_time max_time_to_next_timers_run;
	sc_core::sc_time last_timers_run_time;               // Last time when timers ran
	bool start_request; // M_CAN_CCCR[INIT]: 1 -> 0
	bool init_request;  // M_CAN_CCCR[INIT]: 0 -> 1
	mutable tlm_can_core_config core_config;
	tlm_can_phase phase;
	tlm_can_error can_error;
	bool can_error_changed;
	sc_core::sc_event tx_scan_event;
	sc_core::sc_event tx_event_store_event;
	Tx_Event_FIFO_Element tx_event;
	sc_core::sc_event rx_filter_event;
	sc_core::sc_event rx_store_event;
	unsigned int flip_flop;
	M_CAN_Message tx_msg_pipe[2];  // A two-stage pipeline: Tx scan | Tx
	M_CAN_Message *pending_tx_msg; // pending CAN message (Tx scan stage)
	M_CAN_Message *curr_tx_msg;    // current CAN message being transmitted (Tx stage)
	M_CAN_Message rx_msg;          // CAN message being received
	
	void Reset();
	void RequestInit();
	void RequestStart();
	void RequestClockStop();
	bool InitMode() const;
	bool ConfigurationChangedEnabled() const;
	bool CheckProtectedWrite() const;
	void UpdateLastErrorCode();
	void UpdateWarningStatus();
	void UpdateMasterClock();
	void UpdateCANClock();
	void MasterClockPropertiesChangedProcess();
	void CANClockPropertiesChangedProcess();
	
	void ProcessEvent(Event *event);
	void ProcessEvents();
	void Process();
	void IncrementTimestampCounter(sc_dt::uint64 delta);
	void DecrementTimeoutCounter(sc_dt::uint64 delta);
	void RunTimersToTime(const sc_core::sc_time& time_stamp);
	sc_dt::uint64 TicksToNextTimersRun();
	sc_core::sc_time TimeToNextTimersRun();
	void ScheduleTimersRun();
	unsigned int GetNumDedicatedTxBuffers() const;
	unsigned int GetTxQueueFIFOSize() const;
	unsigned int GetNumTxBuffers() const;
	unsigned int GetTxEventFIFOSize() const;
	unsigned int GetRxFIFOSize(unsigned int fifo_id) const;
	uint32_t GetTxDedicatedMask() const;
	uint32_t GetTxQueueFIFOMask() const;
	uint32_t GetTxFIFOFillMask() const;
	uint32_t GetTxBuffersMask() const;
	void IncrementTxFIFOQueuePutIndex();
	void IncrementTxFIFOGetIndex();
	void AddTxRequests();
	void CancelTxRequests();
	void EnterInitMode();
	void LeaveInitMode();
	void StoreTxEvent(const M_CAN_Message& msg);
	void AcknowledgeTxEventFIFO();
	void StoreRx(sc_dt::uint64 addr, unsigned int element_size, const M_CAN_Message& msg, bool match, unsigned int filter_index);
	void StoreRxFIFO(unsigned int fifo_id, const M_CAN_Message& msg, bool match, unsigned int filter_index = 0, bool set_priority = false);
	void AcknowledgeRxFIFO(unsigned int fifo_id);
	void StoreRxBuffer(unsigned int rx_buffer_element_index, const M_CAN_Message& msg, bool match, unsigned int filter_index);
	void SetPriority(bool xtd, unsigned int filter_index, MESSAGE_STORAGE_INDICATOR msi, unsigned int buffer_index = 0);
	void GenerateFilterEventPulse(uint32_t filter_event);
	void Filter(const M_CAN_Message& msg);
	bool DebugMessageHandlingFSM(DEBUG_MESSAGE_TYPE dbg_msg_type);
	void ReadWords(sc_dt::uint64 addr, uint32_t *data_ptr, unsigned int n);
	void WriteWords(sc_dt::uint64 addr, uint32_t *data_ptr, unsigned int n);
	void TxScan();
	void UpdateInterrupts();
	void UpdateDMA_REQ();
	void INT0_Process();
	void INT1_Process();
	void DMA_REQ_Process();
	void DMA_ACK_Process();
	
	using Super::logger;
	using Super::verbose;
	using sc_core::sc_module::sensitive;
	using sc_core::sc_module::wait;
};

} // end of namespace m_can
} // end of namespace bosch
} // end of namespace com
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_COM_BOSCH_M_CAN_M_CAN_HH__
