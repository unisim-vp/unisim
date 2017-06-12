/*
 *  Copyright (c) 2017,
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

#ifndef __UNISIM_COMPONENT_TLM2_INTERRUPT_FREESCALE_MPC57XX_INTC_INTC_HH__
#define __UNISIM_COMPONENT_TLM2_INTERRUPT_FREESCALE_MPC57XX_INTC_INTC_HH__

#include <unisim/kernel/service/service.hh>
#include <unisim/kernel/logger/logger.hh>
#include <unisim/kernel/tlm2/tlm.hh>
#include <unisim/util/reg/core/register.hh>
#include <tlm_utils/passthrough_target_socket.h>

namespace unisim {
namespace component {
namespace tlm2 {
namespace interrupt {
namespace freescale {
namespace mpc57xx {
namespace intc {

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
using unisim::util::reg::core::RegisterFile;
using unisim::util::reg::core::WarningStatus;
using unisim::util::reg::core::SW_RW;
using unisim::util::reg::core::SW_W;

template <typename FIELD, int OFFSET1, int OFFSET2 = -1>
struct Field8 : unisim::util::reg::core::Field<FIELD, (OFFSET2 >= 0) ? ((OFFSET1 < OFFSET2) ? (7 - OFFSET2) : (7 - OFFSET1)) : (7 - OFFSET1), (OFFSET2 >= 0) ? ((OFFSET1 < OFFSET2) ? (OFFSET2 - OFFSET1 + 1) : (OFFSET1 - OFFSET2 + 1)) : 1>
{
	typedef unisim::util::reg::core::Field<FIELD, (OFFSET2 >= 0) ? ((OFFSET1 < OFFSET2) ? (7 - OFFSET2) : (7 - OFFSET1)) : (7 - OFFSET1), (OFFSET2 >= 0) ? ((OFFSET1 < OFFSET2) ? (OFFSET2 - OFFSET1 + 1) : (OFFSET1 - OFFSET2 + 1)) : 1> Super;
};

template <typename FIELD, int OFFSET1, int OFFSET2 = -1>
struct Field16 : unisim::util::reg::core::Field<FIELD, (OFFSET2 >= 0) ? ((OFFSET1 < OFFSET2) ? (15 - OFFSET2) : (15 - OFFSET1)) : (15 - OFFSET1), (OFFSET2 >= 0) ? ((OFFSET1 < OFFSET2) ? (OFFSET2 - OFFSET1 + 1) : (OFFSET1 - OFFSET2 + 1)) : 1>
{
	typedef unisim::util::reg::core::Field<FIELD, (OFFSET2 >= 0) ? ((OFFSET1 < OFFSET2) ? (15 - OFFSET2) : (15 - OFFSET1)) : (15 - OFFSET1), (OFFSET2 >= 0) ? ((OFFSET1 < OFFSET2) ? (OFFSET2 - OFFSET1 + 1) : (OFFSET1 - OFFSET2 + 1)) : 1> Super;
};

template <typename FIELD, int OFFSET1, int OFFSET2 = -1>
struct Field32 : unisim::util::reg::core::Field<FIELD, (OFFSET2 >= 0) ? ((OFFSET1 < OFFSET2) ? (31 - OFFSET2) : (31 - OFFSET1)) : (31 - OFFSET1), (OFFSET2 >= 0) ? ((OFFSET1 < OFFSET2) ? (OFFSET2 - OFFSET1 + 1) : (OFFSET1 - OFFSET2 + 1)) : 1>
{
	typedef unisim::util::reg::core::Field<FIELD, (OFFSET2 >= 0) ? ((OFFSET1 < OFFSET2) ? (31 - OFFSET2) : (31 - OFFSET1)) : (31 - OFFSET1), (OFFSET2 >= 0) ? ((OFFSET1 < OFFSET2) ? (OFFSET2 - OFFSET1 + 1) : (OFFSET1 - OFFSET2 + 1)) : 1> Super;
};

#if 0
struct CONFIG
{
	static const unsigned int NUM_PROCESSORS = 4;
	static const unsigned int NUM_HW_IRQS = 992;
};
#endif

template <typename CONFIG>
class INTC
	: unisim::kernel::service::Object
	, public sc_core::sc_module
{
public:
	static const unsigned int NUM_PROCESSORS = CONFIG::NUM_PROCESSORS;
	static const unsigned int NUM_SW_IRQS = 32;
	static const unsigned int NUM_HW_IRQS = CONFIG::NUM_HW_IRQS;
	static const unsigned int NUM_IRQS = NUM_SW_IRQS + NUM_HW_IRQS;
	static const unsigned int NUM_PRIORITIES = 64;
	
	typedef tlm_utils::passthrough_target_socket_tagged<INTC, 64> ahb_slave_if_type;

	ahb_slave_if_type *ahb_if[NUM_PROCESSORS]; // AHB slave interface
	sc_core::sc_in<bool> *p_hw_irq[NUM_HW_IRQS];
	sc_core::sc_in<bool> *p_iack[NUM_PROCESSORS];
	sc_core::sc_out<bool> *p_avec_b[NUM_PROCESSORS];
	sc_core::sc_out<sc_dt::sc_uint<14> > *p_voffset[NUM_PROCESSORS];
	
	INTC(const sc_core::sc_module_name& name, unisim::kernel::service::Object *parent);
	virtual ~INTC();
	
	void b_transport(int prc_num, tlm::tlm_generic_payload& payload, sc_core::sc_time& t);
	bool get_direct_mem_ptr(int prc_num, tlm::tlm_generic_payload& payload, tlm::tlm_dmi& dmi_data);
	unsigned int transport_dbg(int prc_num, tlm::tlm_generic_payload& payload);
	tlm::tlm_sync_enum nb_transport_fw(int prc_num, tlm::tlm_generic_payload& payload, tlm::tlm_phase& phase, sc_core::sc_time& t);

protected:
	unisim::kernel::logger::Logger logger;
	
	class Event
	{
	public:
		class Key
		{
		public:
			Key()
				: time_stamp(sc_core::SC_ZERO_TIME)
			{
			}
			
			Key(const sc_core::sc_time& _time_stamp)
				: time_stamp(_time_stamp)
			{
			}

			void SetTimeStamp(const sc_core::sc_time& _time_stamp)
			{
				time_stamp = _time_stamp;
			}

			const sc_core::sc_time& GetTimeStamp() const
			{
				return time_stamp;
			}

			void Clear()
			{
				time_stamp = sc_core::SC_ZERO_TIME;
			}
			
			int operator < (const Key& sk) const
			{
				return time_stamp < sk.time_stamp;
			}
			
		private:
			sc_core::sc_time time_stamp;
		};
		
		Event()
			: key()
			, prc_num(0)
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
		
		void SetProcessorNumber(unsigned int _prc_num)
		{
			prc_num = _prc_num;
		}
		
		void SetPayload(tlm::tlm_generic_payload *_payload, bool _release_payload = false)
		{
			if(release_payload)
			{
				if(payload && payload->has_mm()) payload->release();
			}
			payload = _payload;
			release_payload = _release_payload;
		}
		
		void SetCompletionEvent(sc_core::sc_event *_completion_event)
		{
			completion_event = _completion_event;
		}

		const sc_core::sc_time& GetTimeStamp() const
		{
			return key.GetTimeStamp();
		}
		
		unsigned int GetProcessorNumber() const
		{
			return prc_num;
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
		Key key;
		unsigned int prc_num;
		tlm::tlm_generic_payload *payload;
		bool release_payload;
		sc_core::sc_event *completion_event;
	};

	// INTC Block Configuration Register (INTC_BCR)
	struct INTC_BCR : AddressableRegister<INTC_BCR, 32, SW_RW>
	{
		typedef AddressableRegister<INTC_BCR, 32, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x0;
		
		struct HVEN3 : Field32<HVEN3, 19> {}; // Hardware vector enable for processor 3
		struct HVEN2 : Field32<HVEN3, 23> {}; // Hardware vector enable for processor 2
		struct HVEN1 : Field32<HVEN3, 27> {}; // Hardware vector enable for processor 1
		struct HVEN0 : Field32<HVEN3, 31> {}; // Hardware vector enable for processor 0
		
		typedef FieldSet<HVEN3, HVEN2, HVEN1, HVEN0> ALL;
		
		INTC_BCR() : Super() { Init(); }
		INTC_BCR(uint32_t value) : Super(value) { Init(); }
		
		void Init()
		{
			this->SetName("INTC_BCR"); this->SetDescription("INTC Block Configuration Register");
			HVEN3::SetName("HVEN3"); HVEN3::SetDescription("Hardware vector enable for processor 3");
			HVEN2::SetName("HVEN2"); HVEN2::SetDescription("Hardware vector enable for processor 2");
			HVEN1::SetName("HVEN1"); HVEN1::SetDescription("Hardware vector enable for processor 1");
			HVEN0::SetName("HVEN0"); HVEN0::SetDescription("Hardware vector enable for processor 0");
		}
		
		using Super::operator =;
	};
	
	// INTC Master Protection Register (INTC_MPROT)
	struct INTC_MPROT : AddressableRegister<INTC_MPROT, 32, SW_RW>
	{
		typedef AddressableRegister<INTC_MPROT, 32, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x4;

		struct ID    : Field32<ID, 26, 27> {}; // ID
		struct MPROT : Field32<MPROT, 31>  {}; // Master Protection
		
		INTC_MPROT() : Super() { Init(); }
		INTC_MPROT(uint32_t value) : Super(value) { Init(); }
		
		typedef FieldSet<ID, MPROT> ALL;
		
		void Init()
		{
			this->SetName("INTC_MPROT"); this->SetDescription("INTC Master Protection Register");
			ID   ::SetName("ID");    ID   ::SetDescription("ID");
			MPROT::SetName("MPROT"); MPROT::SetDescription("Master Protection");
		}
		
		using Super::operator =;
	};
	
	// INTC Current Priority Register for Processor 0-3 (INTC_CPR0-3)
	struct INTC_CPR : AddressableRegister<INTC_CPR, 32, SW_RW>
	{
		typedef AddressableRegister<INTC_CPR, 32, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x10;

		struct PRI : Field32<PRI, 26, 31> {}; // Priority of the currently executing ISR
		
		typedef FieldSet<PRI> ALL;
		
		INTC_CPR() : Super() {}
		INTC_CPR(uint32_t value) : Super(value) {}
		void WithinRegisterFileCtor(unsigned int _reg_num)
		{
			reg_num = _reg_num;
			
			std::stringstream name_sstr;
			name_sstr << "INTC_CPR" << reg_num;
			this->SetName(name_sstr.str());
			std::stringstream description_sstr;
			description_sstr << "INTC Current Priority Register for Processor " << reg_num;
			this->SetDescription(name_sstr.str());
			
			PRI::SetName("PRI");
			PRI::SetDescription("Priority of the currently executing ISR");
		}
		
		using Super::operator =;
	private:
		unsigned int reg_num;
	};

	// INTC Interrupt Acknowledge Register for Processor 0-3 (INTC_IACKR0-3)
	struct INTC_IACK : AddressableRegister<INTC_IACK, 32, SW_RW>
	{
		typedef AddressableRegister<INTC_IACK, 32, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x20;

		struct VTBA   : Field32<VTBA, 0, 19>    {};
		struct INTVEC : Field32<INTVEC, 20, 29> {};
		
		typedef FieldSet<VTBA, INTVEC> ALL;
		
		INTC_IACK() : Super() {}
		INTC_IACK(uint32_t value) : Super(value) {}
		void WithinRegisterFileCtor(unsigned int _reg_num)
		{
			reg_num = _reg_num;
			
			std::stringstream name_sstr;
			name_sstr << "INTC_IACK" << reg_num;
			this->SetName(name_sstr.str());
			std::stringstream description_sstr;
			description_sstr << "INTC Interrupt Acknowledge Register for Processor " << reg_num;
			this->SetDescription(name_sstr.str());
			
			VTBA  ::SetName("VTBA");   VTBA  ::SetDescription("Vector table base address");
			INTVEC::SetName("INTVEC"); INTVEC::SetDescription("Interrupt vector");
		}
		
		using Super::operator =;
	private:
		unsigned int reg_num;
	};

	// INTC End Of Interrupt Register for Processor 0-3 (INTC_EOIR0-3)
	struct INTC_EOIR : AddressableRegister<INTC_EOIR, 32, SW_W>
	{
		typedef AddressableRegister<INTC_EOIR, 32, SW_W> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x30;

		struct EOI : Field32<EOI, 0, 31> {}; // End of Interrupt
		
		typedef FieldSet<EOI> ALL;
		
		INTC_EOIR() : Super() {}
		INTC_EOIR(uint32_t value) : Super(value) {}
		void WithinRegisterFileCtor(unsigned int _reg_num)
		{
			reg_num = _reg_num;
			
			std::stringstream name_sstr;
			name_sstr << "INTC_EOIR" << reg_num;
			this->SetName(name_sstr.str());
			std::stringstream description_sstr;
			description_sstr << "INTC End Of Interrupt Register for Processor " << reg_num;
			this->SetDescription(name_sstr.str());
			
			EOI::SetName("EOI");
			EOI::SetDescription("End of Interrupt");
		}
		
		using Super::operator =;
	private:
		unsigned int reg_num;
	};

	// INTC Software Set/Clear Interrupt Register 0-31 (INTC_SSCIR0-31)
	struct INTC_SSCIR : AddressableRegister<INTC_SSCIR, 8, SW_RW>
	{
		typedef AddressableRegister<INTC_SSCIR, 8, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x40;

		struct SET : Field8<SET, 6> {}; // Set flag bits
		struct CLR : Field8<SET, 7> {}; // Clear flag bits; CLR is the flag bit
		
		typedef FieldSet<SET, CLR> ALL;
		
		INTC_SSCIR() : Super() {}
		INTC_SSCIR(uint8_t value) : Super(value) {}
		void WithinRegisterFileCtor(unsigned int _reg_num)
		{
			reg_num = _reg_num;
			
			std::stringstream name_sstr;
			name_sstr << "INTC_SSCIR" << reg_num;
			this->SetName(name_sstr.str());
			std::stringstream description_sstr;
			description_sstr << "INTC Software Set/Clear Interrupt Register " << reg_num;
			this->SetDescription(name_sstr.str());
			
			SET::SetName("SET"); SET::SetDescription("Set flag bits");
			CLR::SetName("CLR"); CLR::SetDescription("Clear flag bits; CLR is the flag bit");
		}
		
		using Super::operator =;
	private:
		unsigned int reg_num;
	};

	// INTC Priority Select Register 0-1023 (INTC_PSR0-1023)
	struct INTC_PSR : AddressableRegister<INTC_PSR, 16, SW_RW>
	{
		typedef AddressableRegister<INTC_PSR, 16, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x60;

		struct PRC_SELN0 : Field16<PRC_SELN0, 0> {}; // Processor select 0
		struct PRC_SELN1 : Field16<PRC_SELN1, 1> {}; // Processor select 1
		struct PRC_SELN2 : Field16<PRC_SELN2, 2> {}; // Processor select 2
		struct PRC_SELN3 : Field16<PRC_SELN3, 3> {}; // Processor select 3
		struct SWTN      : Field16<SWTN, 7>      {}; // Software trigger
		struct PRIN      : Field16<PRIN, 10, 15> {}; // Priority Select
		
		typedef FieldSet<PRC_SELN0, PRC_SELN1, PRC_SELN2, PRC_SELN3, SWTN, PRIN> ALL;
		
		INTC_PSR() : Super() {}
		INTC_PSR(uint8_t value) : Super(value) {}
		void WithinRegisterFileCtor(unsigned int _reg_num)
		{
			reg_num = _reg_num;
			
			std::stringstream name_sstr;
			name_sstr << "INTC_PSR" << reg_num;
			this->SetName(name_sstr.str());
			std::stringstream description_sstr;
			description_sstr << "INTC Priority Select Register " << reg_num;
			this->SetDescription(name_sstr.str());
			
			PRC_SELN0::SetName("PRC_SELN0"); PRC_SELN0::SetDescription("Processor select 0");
			PRC_SELN1::SetName("PRC_SELN1"); PRC_SELN1::SetDescription("Processor select 1");
			PRC_SELN2::SetName("PRC_SELN2"); PRC_SELN2::SetDescription("Processor select 2");
			PRC_SELN3::SetName("PRC_SELN3"); PRC_SELN3::SetDescription("Processor select 3");
			SWTN     ::SetName("SWTN");      SWTN     ::SetDescription("Software trigger");
			PRIN     ::SetName("PRIN");      PRIN     ::SetDescription("Priority Select");
			
		}
		
		INTC_PSR& operator = (uint16_t value)
		{
			if(reg_num < 32)
			{
				SWTN::Set(value, uint16_t(0)); // clear SWTN as first 32 INTC_PSR registers have this bit stuck to zero
			}
			Super::operator = (value);
			return *this;
		}
		
		WarningStatus Write(uint16_t value, uint32_t bit_enable = 0xffff)
		{
			if(reg_num < 32)
			{
				SWTN::Set(value, uint16_t(0)); // clear SWTN as first 32 INTC_PSR registers have this bit stuck to zero
			}
			return Super::Write(value, bit_enable);
		}
	private:
		unsigned int reg_num;
	};
	
	INTC_BCR intc_bcr;
	INTC_MPROT intc_mprot;
	RegisterFile<INTC_CPR, NUM_PROCESSORS> intc_cpr;
	RegisterFile<INTC_IACK, NUM_PROCESSORS> intc_iack;
	RegisterFile<INTC_EOIR, NUM_PROCESSORS> intc_eoir;
	RegisterFile<INTC_SSCIR, NUM_SW_IRQS> intc_sscir;
	RegisterFile<INTC_PSR, NUM_IRQS> intc_psr;
private:
	RegisterAddressMap<sc_dt::uint64> reg_addr_map;
	
	unisim::kernel::tlm2::Schedule<Event> schedule;
	
	unisim::util::endian::endian_type endian;
	unisim::kernel::service::Parameter<unisim::util::endian::endian_type> param_endian;
	
	void ProcessEvent(Event *event);
	void ProcessEvents();
};

} // end of namespace intc
} // end of namespace mpc57xx
} // end of namespace freescale
} // end of namespace interrupt
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_INTERRUPT_FREESCALE_MPC57XX_INTC_INTC_HH__
