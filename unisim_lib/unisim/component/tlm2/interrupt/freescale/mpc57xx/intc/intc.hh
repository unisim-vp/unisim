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
#include <stack>

#define SWITCH_ENUM_TRAIT(ENUM_TYPE, CLASS_NAME) template <ENUM_TYPE, bool __SWITCH_TRAIT_DUMMY__ = true> struct CLASS_NAME {}
#define CASE_ENUM_TRAIT(ENUM_VALUE, CLASS_NAME) template <bool __SWITCH_TRAIT_DUMMY__> struct CLASS_NAME<ENUM_VALUE, __SWITCH_TRAIT_DUMMY__>
#define ENUM_TRAIT(ENUM_VALUE, CLASS_NAME) CLASS_NAME<ENUM_VALUE>

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
using unisim::util::reg::core::AddressableRegisterFile;
using unisim::util::reg::core::ReadWriteStatus;
using unisim::util::reg::core::SW_RW;
using unisim::util::reg::core::SW_W;
using unisim::util::reg::core::RWS_OK;
using unisim::util::reg::core::RWS_ANA;

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
	static const unsigned int BUSWIDTH = 32;
	static const unsigned int VOFFSET_WIDTH = 14;
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
	static const unsigned int NUM_PRIORITY_LEVELS = 64;
	static const unsigned int MAX_PRIORITY_LEVEL = NUM_PRIORITY_LEVELS - 1;
	static const unsigned int NUM_IRQS_PER_PRIORITY_LEVEL = NUM_IRQS / NUM_PRIORITY_LEVELS;
	static const unsigned int BUSWIDTH = CONFIG::BUSWIDTH;
	static const unsigned int VOFFSET_WIDTH = CONFIG::VOFFSET_WIDTH;
	static const bool threaded_model = false;
	
	enum IRQ_Type
	{
		SW_IRQ = 0,
		HW_IRQ = 1
	};
	
	typedef tlm_utils::passthrough_target_socket_tagged<INTC, BUSWIDTH> ahb_slave_if_type;

	ahb_slave_if_type *ahb_if[NUM_PROCESSORS]; // AHB slave interface   FIXME: final design will have only one common port with a master ID in a tlm gp extension
	sc_core::sc_in<bool> *p_hw_irq[NUM_HW_IRQS];
	sc_core::sc_in<bool> *p_iack[NUM_PROCESSORS];
	sc_core::sc_out<bool> *p_irq_b[NUM_PROCESSORS];
	sc_core::sc_out<bool> *p_avec_b[NUM_PROCESSORS];
	sc_core::sc_out<sc_dt::sc_uint<VOFFSET_WIDTH> > *p_voffset[NUM_PROCESSORS];
	
	INTC(const sc_core::sc_module_name& name, unisim::kernel::service::Object *parent);
	virtual ~INTC();
	
	void b_transport(int prc_num, tlm::tlm_generic_payload& payload, sc_core::sc_time& t);
	bool get_direct_mem_ptr(int prc_num, tlm::tlm_generic_payload& payload, tlm::tlm_dmi& dmi_data);
	unsigned int transport_dbg(int prc_num, tlm::tlm_generic_payload& payload);
	tlm::tlm_sync_enum nb_transport_fw(int prc_num, tlm::tlm_generic_payload& payload, tlm::tlm_phase& phase, sc_core::sc_time& t);

private:
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

	typedef unsigned int ProcessorNumber;
	
	// INTC Block Configuration Register (INTC_BCR)
	struct INTC_BCR : AddressableRegister<INTC_BCR, 32, SW_RW, ProcessorNumber>
	{
		typedef AddressableRegister<INTC_BCR, 32, SW_RW, ProcessorNumber> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x0;
		
		struct HVEN3 : Field32<HVEN3, 19>    {}; // Hardware vector enable for processor 3
		struct HVEN2 : Field32<HVEN3, 23>    {}; // Hardware vector enable for processor 2
		struct HVEN1 : Field32<HVEN3, 27>    {}; // Hardware vector enable for processor 1
		struct HVEN0 : Field32<HVEN3, 31>    {}; // Hardware vector enable for processor 0
		
		SWITCH_ENUM_TRAIT(unsigned int, _);
		CASE_ENUM_TRAIT(1, _) { typedef FieldSet<HVEN0> ALL; };
		CASE_ENUM_TRAIT(2, _) { typedef FieldSet<HVEN1, HVEN0> ALL; };
		CASE_ENUM_TRAIT(3, _) { typedef FieldSet<HVEN2, HVEN1, HVEN0> ALL; };
		CASE_ENUM_TRAIT(4, _) { typedef FieldSet<HVEN3, HVEN2, HVEN1, HVEN0> ALL; };
		typedef typename ENUM_TRAIT(NUM_PROCESSORS, _)::ALL ALL;
		
		INTC_BCR(INTC<CONFIG> *_intc) : Super(), intc(_intc) { Init(); }
		INTC_BCR(INTC<CONFIG> *_intc, uint32_t value) : Super(value), intc(_intc) { Init(); }
		
		void Init()
		{
			this->SetName("INTC_BCR"); this->SetDescription("INTC Block Configuration Register");
			HVEN3::SetName("HVEN3"); HVEN3::SetDescription("Hardware vector enable for processor 3");
			HVEN2::SetName("HVEN2"); HVEN2::SetDescription("Hardware vector enable for processor 2");
			HVEN1::SetName("HVEN1"); HVEN1::SetDescription("Hardware vector enable for processor 1");
			HVEN0::SetName("HVEN0"); HVEN0::SetDescription("Hardware vector enable for processor 0");
		}
		
		virtual ReadWriteStatus Write(ProcessorNumber& writter_prc_num, const uint32_t& value, const uint32_t& bit_enable)
		{
			if(!intc->CheckMasterProtection(writter_prc_num))
			{
				// CPU is not allowed to write INTC_BCR
				return RWS_ANA;
			}
			
			return Super::Write(writter_prc_num, value, bit_enable);
		}
		
		using Super::operator =;
	private:
		INTC<CONFIG> *intc;
	};
	
	// INTC Master Protection Register (INTC_MPROT)
	struct INTC_MPROT : AddressableRegister<INTC_MPROT, 32, SW_RW, ProcessorNumber>
	{
		typedef AddressableRegister<INTC_MPROT, 32, SW_RW, ProcessorNumber> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x4;

		struct ID    : Field32<ID, 26, 27> {}; // ID
		struct MPROT : Field32<MPROT, 31>  {}; // Master Protection
		
		INTC_MPROT(INTC<CONFIG> *_intc) : Super(), intc(_intc) { Init(); }
		INTC_MPROT(INTC<CONFIG> *_intc, uint32_t value) : Super(value), intc(_intc) { Init(); }
		
		typedef FieldSet<ID, MPROT> ALL;
		
		void Init()
		{
			this->SetName("INTC_MPROT"); this->SetDescription("INTC Master Protection Register");
			ID   ::SetName("ID");    ID   ::SetDescription("ID");
			MPROT::SetName("MPROT"); MPROT::SetDescription("Master Protection");
		}
		
		virtual ReadWriteStatus Write(ProcessorNumber& writter_prc_num, const uint32_t& value, const uint32_t& bit_enable)
		{
			if(!intc->CheckMasterProtection(writter_prc_num))
			{
				// CPU is not allowed to write INTC_MPROT
				return RWS_ANA;
			}
			
			return Super::Write(writter_prc_num, value, bit_enable);
		}

		using Super::operator =;
	private:
		INTC<CONFIG> *intc;
	};
	
	// INTC Current Priority Register for Processor 0-3 (INTC_CPR0-3)
	struct INTC_CPR : AddressableRegister<INTC_CPR, 32, SW_RW, ProcessorNumber>
	{
		typedef AddressableRegister<INTC_CPR, 32, SW_RW, ProcessorNumber> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x10;

		struct PRI : Field32<PRI, 26, 31> {}; // Priority of the currently executing ISR
		
		typedef FieldSet<PRI> ALL;
		
		INTC_CPR() : Super(), intc(0), reg_num(0) {}
		INTC_CPR(INTC<CONFIG> *_intc) : Super(), intc(_intc), reg_num(0) {}
		INTC_CPR(INTC<CONFIG> *_intc, uint32_t value) : Super(value), intc(_intc), reg_num(0) {}
		void WithinRegisterFileCtor(unsigned int _reg_num, INTC<CONFIG> *_intc)
		{
			intc = _intc;
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
		
		virtual ReadWriteStatus Write(ProcessorNumber& writter_prc_num, const uint32_t& value, const uint32_t& bit_enable)
		{
			if(!intc->CheckMasterProtection(writter_prc_num))
			{
				// CPU is not allowed to write INTC_CPR
				return RWS_ANA;
			}
			
			return Super::Write(writter_prc_num, value, bit_enable);
		}

		using Super::operator =;
	private:
		INTC<CONFIG> *intc;
		unsigned int reg_num;
	};

	// INTC Interrupt Acknowledge Register for Processor 0-3 (INTC_IACKR0-3)
	struct INTC_IACK : AddressableRegister<INTC_IACK, 32, SW_RW, ProcessorNumber>
	{
		typedef AddressableRegister<INTC_IACK, 32, SW_RW, ProcessorNumber> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x20;

		struct VTBA   : Field32<VTBA, 0, 19>    {};
		struct INTVEC : Field32<INTVEC, 20, 29> {};
		
		typedef FieldSet<VTBA, INTVEC> ALL;
		
		INTC_IACK() : Super(), intc(0), reg_num(0) {}
		INTC_IACK(INTC<CONFIG> *_intc) : Super(), intc(_intc), reg_num(0) {}
		INTC_IACK(INTC<CONFIG> *_intc, uint32_t value) : Super(value), intc(_intc), reg_num(0) {}
		void WithinRegisterFileCtor(unsigned int _reg_num, INTC<CONFIG> *_intc)
		{
			intc = _intc;
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

		virtual ReadWriteStatus Read(ProcessorNumber& reader_prc_num, uint32_t& value, const uint32_t& bit_enable)
		{
			if(!intc->CheckMasterProtection(reader_prc_num))
			{
				// CPU is not allowed to read INTC_IACK
				return RWS_ANA;
			}
			
			unsigned prc_num = reg_num;
			if(!intc->IsHardwareVectorEnabled(prc_num))
			{
				intc->IRQAcknowledge(prc_num);
				intc->SetIRQOutput(prc_num, false);
			}
			
			return Super::Read(reader_prc_num, value, bit_enable);
		}
		
		virtual ReadWriteStatus Write(ProcessorNumber& writter_prc_num, const uint32_t& value, const uint32_t& bit_enable)
		{
			if(!intc->CheckMasterProtection(writter_prc_num))
			{
				// CPU is not allowed to write INTC_IACK
				return RWS_ANA;
			}
			
			return Super::Write(writter_prc_num, value, bit_enable);
		}

		using Super::operator =;
	private:
		INTC<CONFIG> *intc;
		unsigned int reg_num;
	};

	// INTC End Of Interrupt Register for Processor 0-3 (INTC_EOIR0-3)
	struct INTC_EOIR : AddressableRegister<INTC_EOIR, 32, SW_W, ProcessorNumber>
	{
		typedef AddressableRegister<INTC_EOIR, 32, SW_W, ProcessorNumber> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x30;

		struct EOI : Field32<EOI, 0, 31> {}; // End of Interrupt
		
		typedef FieldSet<EOI> ALL;
		
		INTC_EOIR() : Super(), intc(0), reg_num(0) {}
		INTC_EOIR(INTC<CONFIG> *_intc) : Super(), intc(_intc), reg_num(0) {}
		INTC_EOIR(INTC<CONFIG> *_intc, uint32_t value) : Super(value), intc(_intc), reg_num(0) {}
		void WithinRegisterFileCtor(unsigned int _reg_num, INTC<CONFIG> *_intc)
		{
			intc = _intc;
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
		
		virtual ReadWriteStatus Write(ProcessorNumber& writter_prc_num, const uint32_t& value, const uint32_t& bit_enable)
		{
			if(!intc->CheckMasterProtection(writter_prc_num))
			{
				// CPU is not allowed to write INTC_EOIR
				return RWS_ANA;
			}
			
			// Writing four zeroes ?
			if((value == 0) && (bit_enable == ~uint32_t(0)))
			{
				unsigned int prc_num = reg_num;
				intc->EndOfInterrupt(prc_num);
			}
			
			// INTC_EOIR is never updated
			return RWS_OK;
		}

		using Super::operator =;
	private:
		INTC<CONFIG> *intc;
		unsigned int reg_num;
	};

	// INTC Software Set/Clear Interrupt Register 0-31 (INTC_SSCIR0-31)
	struct INTC_SSCIR : AddressableRegister<INTC_SSCIR, 8, SW_RW, ProcessorNumber>
	{
		typedef AddressableRegister<INTC_SSCIR, 8, SW_RW, ProcessorNumber> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x40;

		struct SET : Field8<SET, 6> {}; // Set flag bits
		struct CLR : Field8<SET, 7> {}; // Clear flag bits; CLR is the flag bit
		
		typedef FieldSet<SET, CLR> ALL;
		
		INTC_SSCIR() : Super(), intc(0), reg_num(0) {}
		INTC_SSCIR(INTC<CONFIG> *_intc) : Super(), intc(_intc), reg_num(0) {}
		INTC_SSCIR(INTC<CONFIG> *_intc, uint8_t value) : Super(value), intc(_intc), reg_num(0) {}
		void WithinRegisterFileCtor(unsigned int _reg_num, INTC<CONFIG> *_intc)
		{
			intc = _intc;
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
		
		void Effect()
		{
			if(this->template Get<typename INTC_SSCIR::SET>())
			{
				// Writing a to SET will leave SET unchanged at 0 but will set CLR
				// If a 1 is written to a pair of SET and CLR bits at the same time,
				// CLR is asserted, regardless of whether CLR was asserted before the write
				this->template Set<typename INTC_SSCIR::CLR>(1);
				this->template Set<typename INTC_SSCIR::SET>(0);
			}
			else if(this->template Get<typename INTC_SSCIR::CLR>())
			{
				// Writing a 1 to CLR will clear it
				this->template Set<typename INTC_SSCIR::CLR>(0);
			}
			
			// CLR is the flag bit
			unsigned int sw_irq_num = reg_num;
			intc->SetIRQInputStatus(intc->SW_IRQ2IRQ(sw_irq_num), this->template Get<typename INTC_SSCIR::CLR>() != 0);
		}
		
		INTC_SSCIR& operator = (const uint8_t& value)
		{
			Super::operator = (value);
			Effect();
			return *this;
		}
		
		virtual ReadWriteStatus Write(ProcessorNumber& writter_prc_num, const uint8_t& value, const uint8_t& bit_enable)
		{
			unsigned int sw_irq_num = reg_num;
			
			if(INTC_SSCIR::CLR::Get(value) && !intc->CheckWriteProtection_SSCIR_CLR(writter_prc_num, sw_irq_num))
			{
				// CPU is not allowed to write 1 into CLR field
				return RWS_ANA;
			}
			
			ReadWriteStatus rws = Super::Write(writter_prc_num, value, bit_enable);

			Effect();
			
			return rws;
		}

		using Super::operator =;
	private:
		INTC<CONFIG> *intc;
		unsigned int reg_num;
	};

	// INTC Priority Select Register 0-1023 (INTC_PSR0-1023)
	template <IRQ_Type IRQ_TYPE>
	struct INTC_PSR : AddressableRegister<INTC_PSR<IRQ_TYPE>, 16, SW_RW, ProcessorNumber>
	{
		typedef AddressableRegister<INTC_PSR<IRQ_TYPE>, 16, SW_RW, ProcessorNumber> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = (IRQ_TYPE == HW_IRQ) ? (0x60 + (NUM_SW_IRQS * 4)) : 0x60;

		struct PRC_SELN0 : Field16<PRC_SELN0, 0>   {};  // Processor select 0
		struct PRC_SELN1 : Field16<PRC_SELN1, 1>   {};  // Processor select 1
		struct PRC_SELN2 : Field16<PRC_SELN2, 2>   {};  // Processor select 2
		struct PRC_SELN3 : Field16<PRC_SELN3, 3>   {};  // Processor select 3
		struct PRC_SELN  : Field16<PRC_SELN, 0, 3> {};  // Processor Select mask
		struct SWTN      : Field16<SWTN, 7>        {};  // Software trigger
		struct PRIN      : Field16<PRIN, 10, 15>   {};  // Priority Select
		
		// Note: Actual PRC_SELN bits depends on the effective number of processor in the system
		SWITCH_ENUM_TRAIT(unsigned int, _N);
		CASE_ENUM_TRAIT(1, _N) { typedef FieldSet<PRC_SELN0, PRIN> ALL; };
		CASE_ENUM_TRAIT(2, _N) { typedef FieldSet<PRC_SELN0, PRC_SELN1, PRIN> ALL; };
		CASE_ENUM_TRAIT(3, _N) { typedef FieldSet<PRC_SELN0, PRC_SELN1, PRC_SELN2, PRIN> ALL; };
		CASE_ENUM_TRAIT(4, _N) { typedef FieldSet<PRC_SELN0, PRC_SELN1, PRC_SELN2, PRC_SELN3, PRIN> ALL; };

		// Note: SWTN is only available in PSRs that do not correcpsond to the SSCIRs (aka. first PSR registers)
		SWITCH_ENUM_TRAIT(IRQ_Type, _);
		CASE_ENUM_TRAIT(SW_IRQ, _) { typedef typename ENUM_TRAIT(NUM_PROCESSORS, _N)::ALL ALL; };
		CASE_ENUM_TRAIT(HW_IRQ, _) { typedef FieldSet<typename ENUM_TRAIT(NUM_PROCESSORS, _N)::ALL, SWTN> ALL; };

		typedef typename ENUM_TRAIT(IRQ_TYPE, _)::ALL ALL;
		
		INTC_PSR() : Super(), intc(0), reg_num(0) {}
		INTC_PSR(INTC<CONFIG> *_intc) : Super(), intc(_intc), reg_num(0) {}
		INTC_PSR(INTC<CONFIG> *_intc, uint8_t value) : Super(value), intc(_intc), reg_num(0) {}
		void WithinRegisterFileCtor(unsigned int _reg_num, INTC<CONFIG> *_intc)
		{
			intc = _intc;
			reg_num = _reg_num;
			
			std::stringstream name_sstr;
			name_sstr << "INTC_PSR" << ((IRQ_TYPE == HW_IRQ)  ? (NUM_SW_IRQS + reg_num) : reg_num);
			this->SetName(name_sstr.str());
			std::stringstream description_sstr;
			description_sstr << "INTC Priority Select Register " << reg_num;
			this->SetDescription(name_sstr.str());
			
			PRC_SELN0::SetName("PRC_SELN0"); PRC_SELN0::SetDescription("Processor select 0");
			PRC_SELN1::SetName("PRC_SELN1"); PRC_SELN1::SetDescription("Processor select 1");
			PRC_SELN2::SetName("PRC_SELN2"); PRC_SELN2::SetDescription("Processor select 2");
			PRC_SELN3::SetName("PRC_SELN3"); PRC_SELN3::SetDescription("Processor select 3");
			PRC_SELN3::SetName("PRC_SELN");  PRC_SELN ::SetDescription("Processor select mask");
			SWTN     ::SetName("SWTN");      SWTN     ::SetDescription("Software trigger");
			PRIN     ::SetName("PRIN");      PRIN     ::SetDescription("Priority Select");
			
		}
		
		virtual ReadWriteStatus Write(ProcessorNumber& writter_prc_num, const uint16_t& value, const uint16_t& bit_enable)
		{
			if(!intc->CheckMasterProtection(writter_prc_num))
			{
				// CPU is not allowed to write INTC_PSR
				return RWS_ANA;
			}
			
			unsigned int old_priority = this->template Get<typename INTC_PSR::PRIN>();
			unsigned int old_prc_seln = this->template Get<typename INTC_PSR::PRC_SELN>();
			unsigned int hw_sw_irq_num = reg_num;
			unsigned int irq_num = (IRQ_TYPE == HW_IRQ) ? intc->HW_IRQ2IRQ(hw_sw_irq_num) : intc->SW_IRQ2IRQ(hw_sw_irq_num);
			
			ReadWriteStatus rws = Super::Write(writter_prc_num, value, bit_enable);
			
			unsigned int new_priority = this->template Get<typename INTC_PSR::PRIN>();
			unsigned int new_prc_seln = this->template Get<typename INTC_PSR::PRC_SELN>();
			
			unsigned int prc_num;
			for(prc_num = 0; prc_num < NUM_PROCESSORS; prc_num++)
			{
				bool old_sel = (8 >> prc_num) & old_prc_seln;
				bool new_sel = (8 >> prc_num) & new_prc_seln;
				
				if(new_sel)
				{
					// CPU is selected
					if(old_sel)
					{
						// CPU was already selected
						if(old_priority != new_priority)
						{
							// IRQ priority changed
							intc->ChangeIRQInputPriority(prc_num, irq_num, old_priority, new_priority);
						}
					}
					else
					{
						// CPU gets selected
						intc->EnableIRQInput(prc_num, irq_num, new_priority);
					}
				}
				else
				{
					if(old_sel)
					{
						// CPU gets deselected
						intc->DisableIRQInput(prc_num, irq_num, old_priority);
					}
					else
					{
						// CPU was already deselected
					}
				}
			}
			
			if(IRQ_TYPE == HW_IRQ)
			{
				if(this->template Get<typename INTC_PSR::SWTN>())
				{
					intc->SetIRQInputStatus(irq_num, true);
				}
			}
			
			return rws;
		}
	private:
		INTC<CONFIG> *intc;
		unsigned int reg_num;
	};
	
	INTC_BCR intc_bcr;
	INTC_MPROT intc_mprot;
	AddressableRegisterFile<INTC_CPR, NUM_PROCESSORS, INTC<CONFIG>, ProcessorNumber> intc_cpr;
	AddressableRegisterFile<INTC_IACK, NUM_PROCESSORS, INTC<CONFIG>, ProcessorNumber> intc_iack;
	AddressableRegisterFile<INTC_EOIR, NUM_PROCESSORS, INTC<CONFIG>, ProcessorNumber> intc_eoir;
	AddressableRegisterFile<INTC_SSCIR, NUM_SW_IRQS, INTC<CONFIG>, ProcessorNumber> intc_sscir;
	AddressableRegisterFile<INTC_PSR<SW_IRQ>, NUM_SW_IRQS, INTC<CONFIG>, ProcessorNumber> intc_psr_sw;
	AddressableRegisterFile<INTC_PSR<HW_IRQ>, NUM_HW_IRQS, INTC<CONFIG>, ProcessorNumber> intc_psr_hw;
	
	std::stack<unsigned int> lifo[NUM_PROCESSORS];

	class IRQPriority
	{
	public:
		IRQPriority() : key(0) {}
		IRQPriority(unsigned int irq_num, unsigned int priority) : key(((MAX_PRIORITY_LEVEL - priority) * NUM_IRQS) + irq_num) {}
		
		int operator < (const IRQPriority& ip) const { return key < ip.key; }
		
		unsigned int GetIRQ() const { return key % NUM_IRQS; }
		unsigned int GetPriority() const { return MAX_PRIORITY_LEVEL - (key / NUM_IRQS); }
	private:
		unsigned int key;
	};
	
	uint64_t irqs[NUM_IRQS / 64];
	std::set<IRQPriority> priority_tree[NUM_PROCESSORS];
	
	void UpdateIRQSelect(unsigned int prc_num);
	void SetIRQOutput(unsigned int prc_num, bool value);
	void SetIRQInputStatus(unsigned int irq_num, bool value);
	bool GetIRQInputStatus(unsigned int irq_num) const;
	void EnableIRQInput(unsigned int prc_num, unsigned int irq_num, unsigned int new_priority);
	void DisableIRQInput(unsigned int prc_num, unsigned int irq_num, unsigned int old_priority);
	void ChangeIRQInputPriority(unsigned int prc_num, unsigned int irq_num, unsigned int old_priority, unsigned int new_priority);
	bool SelectIRQInput(unsigned int prc_num, unsigned int& irq_num) const;
	void IRQAcknowledge(unsigned int prc_num);
	void EndOfInterrupt(unsigned int prc_num);
	RegisterAddressMap<sc_dt::uint64, ProcessorNumber> reg_addr_map;
	
	unisim::kernel::tlm2::Schedule<Event> schedule;
	sc_core::sc_time last_irq_b_time_stamp[NUM_PROCESSORS];
	sc_core::sc_event *irq_select_event[NUM_PROCESSORS];
	sc_core::sc_event *gen_irq_b_event[NUM_PROCESSORS];
	unsigned int selected_irq_num[NUM_PROCESSORS];
	bool irq_b[NUM_PROCESSORS];
	
	unisim::util::endian::endian_type endian;
	unisim::kernel::service::Parameter<unisim::util::endian::endian_type> param_endian;
	bool verbose;
	unisim::kernel::service::Parameter<bool> param_verbose;
	sc_core::sc_time cycle_time;
	unisim::kernel::service::Parameter<sc_core::sc_time> param_cycle_time;
	
	bool CheckMasterProtection(unsigned int prc_num) const;
	bool CheckWriteProtection_SSCIR_CLR(unsigned int prc_num, unsigned int sw_irq_num) const;
	bool IsHardwareVectorEnabled(unsigned int prc_num) const;
	void ProcessEvent(Event *event);
	void ProcessEvents();
	void Process();
	unsigned int SW_IRQ2IRQ(unsigned int sw_irq_num) const;
	unsigned int HW_IRQ2IRQ(unsigned int hw_irq_num) const;
	unsigned int IRQ2SW_IRQ(unsigned int irq_num) const;
	unsigned int IRQ2HW_IRQ(unsigned int irq_num) const;
	bool IsSW_IRQ(unsigned int irq_num) const;
	bool IsHW_IRQ(unsigned int irq_num) const;
	void HW_IRQ_Process(unsigned int hw_irq_num);
	void IRQ_B_Process(unsigned int prc_num);
	void IACK_Process(unsigned int prc_num);
	void IRQ_Select_Process(unsigned int prc_num);
};

} // end of namespace intc
} // end of namespace mpc57xx
} // end of namespace freescale
} // end of namespace interrupt
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_INTERRUPT_FREESCALE_MPC57XX_INTC_INTC_HH__
