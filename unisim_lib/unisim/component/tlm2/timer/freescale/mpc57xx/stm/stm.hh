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

#ifndef __UNISIM_COMPONENT_TLM2_TIMER_FREESCALE_MPC57XX_STM_STM_HH__
#define __UNISIM_COMPONENT_TLM2_TIMER_FREESCALE_MPC57XX_STM_STM_HH__

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
namespace timer {
namespace freescale {
namespace mpc57xx {
namespace stm {

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
using unisim::util::reg::core::Access;
using unisim::util::reg::core::SW_RW;
using unisim::util::reg::core::SW_W;
using unisim::util::reg::core::SW_R_W1C;
using unisim::util::reg::core::RWS_OK;
using unisim::util::reg::core::RWS_ANA;

template <typename FIELD, int OFFSET1, int OFFSET2 = -1, Access _ACCESS = SW_RW>
struct Field : unisim::util::reg::core::Field<FIELD, (OFFSET2 >= 0) ? ((OFFSET1 < OFFSET2) ? (31 - OFFSET2) : (31 - OFFSET1)) : (31 - OFFSET1), (OFFSET2 >= 0) ? ((OFFSET1 < OFFSET2) ? (OFFSET2 - OFFSET1 + 1) : (OFFSET1 - OFFSET2 + 1)) : 1, _ACCESS>
{
	typedef unisim::util::reg::core::Field<FIELD, (OFFSET2 >= 0) ? ((OFFSET1 < OFFSET2) ? (31 - OFFSET2) : (31 - OFFSET1)) : (31 - OFFSET1), (OFFSET2 >= 0) ? ((OFFSET1 < OFFSET2) ? (OFFSET2 - OFFSET1 + 1) : (OFFSET1 - OFFSET2 + 1)) : 1, _ACCESS> Super;
};

#if 0
struct CONFIG
{
	static const unsigned int NUM_CHANNELS = 4;
	static const unsigned int BUSWIDTH = 32
};
#endif

template <typename CONFIG>
class STM
	: unisim::kernel::service::Object
	, public sc_core::sc_module
	, public tlm::tlm_fw_transport_if<>
{
public:
	static const unsigned int TLM2_IP_VERSION_MAJOR = 1;
	static const unsigned int TLM2_IP_VERSION_MINOR = 0;
	static const unsigned int TLM2_IP_VERSION_PATCH = 0;
	static const unsigned int NUM_CHANNELS          = CONFIG::NUM_CHANNELS;
	static const unsigned int BUSWIDTH              = CONFIG::BUSWIDTH;
	static const bool threaded_model                = false;
	
	typedef tlm::tlm_target_socket<BUSWIDTH> peripheral_slave_if_type;

	peripheral_slave_if_type  peripheral_slave_if;  // peripheral slave interface
	sc_core::sc_in<bool>      m_clk;                // Clock port
	sc_core::sc_in<bool>      debug;                // debug port
	sc_core::sc_out<bool>    *irq[NUM_CHANNELS];    // IRQ outputs
	
	STM(const sc_core::sc_module_name& name, unisim::kernel::service::Object *parent);
	virtual ~STM();
	
	void b_transport(tlm::tlm_generic_payload& payload, sc_core::sc_time& t);
	bool get_direct_mem_ptr(tlm::tlm_generic_payload& payload, tlm::tlm_dmi& dmi_data);
	unsigned int transport_dbg(tlm::tlm_generic_payload& payload);
	tlm::tlm_sync_enum nb_transport_fw(tlm::tlm_generic_payload& payload, tlm::tlm_phase& phase, sc_core::sc_time& t);

private:
	virtual void end_of_elaboration();

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
	
	// Common STM register representation
	template <typename REGISTER>
	struct STM_Register : AddressableRegister<REGISTER, 32, SW_RW, sc_core::sc_time>
	{
		typedef AddressableRegister<REGISTER, 32, SW_RW, sc_core::sc_time> Super;
		
		STM_Register(STM<CONFIG> *_stm) : Super(), stm(_stm) {}
		STM_Register(STM<CONFIG> *_stm, uint32_t value) : Super(value), stm(_stm) {}

		inline bool IsVerboseRead() const ALWAYS_INLINE { return stm->verbose; }
		inline bool IsVerboseWrite() const ALWAYS_INLINE { return stm->verbose; }
		inline std::ostream& GetInfoStream() ALWAYS_INLINE { return stm->logger.DebugInfoStream(); }

		virtual ReadWriteStatus Read(sc_core::sc_time& time_stamp, uint32_t& value, const uint32_t& bit_enable)
		{
			// Run counter until read time so that registers reflect precise state of STM
			stm->RunCounterToTime(time_stamp);
			// Read register
			return this->Super::Read(time_stamp, value, bit_enable);
		}
		
		virtual ReadWriteStatus Write(sc_core::sc_time& time_stamp, const uint32_t& value, const uint32_t& bit_enable)
		{
			// Run counter until write time
			stm->RunCounterToTime(time_stamp);
			// Write register
			ReadWriteStatus rws = this->Super::Write(time_stamp, value, bit_enable);
			// STM register changes affect IRQ output, so schedule a run of counter and compare that may trigger an IRQ output
			stm->ScheduleCounterRun();
			return rws;
		}

		using Super::operator =;
		
	protected:
		STM<CONFIG> *stm;
	};

	// STM Control Register (STM_CR)
	struct STM_CR : STM_Register<STM_CR>
	{
		typedef STM_Register<STM_CR> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x0;

		struct CPS : Field<CPS, 16, 23> {}; // Counter Prescaler
		struct FRZ : Field<FRZ, 30>     {}; // Freeze
		struct TEN : Field<TEN, 31>     {}; // Timer counter enabled
		
		typedef FieldSet<CPS, FRZ, TEN> ALL;
		
		STM_CR(STM<CONFIG> *_stm) : Super(_stm) { Init(); }
		STM_CR(STM<CONFIG> *_stm, uint32_t value) : Super(_stm, value) { Init(); }
		
		void Init()
		{
			this->SetName("STM_CR"); this->SetDescription("STM Control Register");
			CPS::SetName("CPS"); CPS::SetDescription("Counter Prescaler");
			FRZ::SetName("FRZ"); FRZ::SetDescription("Freeze");
			TEN::SetName("TEN"); TEN::SetDescription("Timer counter enabled");
		}
		
		virtual ReadWriteStatus Write(sc_core::sc_time& time_stamp, const uint32_t& value, const uint32_t& bit_enable)
		{
			// Write STM register
			ReadWriteStatus rws = this->Super::Write(time_stamp, value, bit_enable);
			
			// FRZ may have changed
			this->stm->RefreshFreeze();
			return rws;
		}

		using Super::operator =;
	};

	// STM Count Register (STM_CNT)
	struct STM_CNT : STM_Register<STM_CNT>
	{
		typedef STM_Register<STM_CNT> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x4;

		struct CNT : Field<CNT, 0, 31> {}; // Timer count value used as the time base for all channels
		
		typedef FieldSet<CNT> ALL;
		
		STM_CNT(STM<CONFIG> *_stm) : Super(_stm) { Init(); }
		STM_CNT(STM<CONFIG> *_stm, uint32_t value) : Super(_stm, value) { Init(); }
		
		void Init()
		{
			this->SetName("STM_CNT"); this->SetDescription("STM Count Register");
			CNT::SetName("CNT"); CNT::SetDescription("Timer count value used as the time base for all channels");
		}

		using Super::operator =;
	};

	// STM Channel Control Register (STM_CCRn)
	struct STM_CCR : STM_Register<STM_CCR>
	{
		typedef STM_Register<STM_CCR> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x10;

		struct CEN : Field<CEN, 31> {}; // Channel Enable
		
		typedef FieldSet<CEN> ALL;
		
		STM_CCR() : Super(0), reg_num(0) {}
		STM_CCR(STM<CONFIG> *_stm) : Super(_stm) {}
		STM_CCR(STM<CONFIG> *_stm, uint32_t value) : Super(_stm, value) {}
		
		void WithinRegisterFileCtor(unsigned int _reg_num, STM<CONFIG> *_stm)
		{
			this->stm = _stm;
			reg_num = _reg_num;
			
			std::stringstream name_sstr;
			name_sstr << "STM_CCR" << reg_num;
			this->SetName(name_sstr.str());
			std::stringstream description_sstr;
			description_sstr << "STM Channel Control Register #" << reg_num;
			this->SetDescription(name_sstr.str());
			
			CEN::SetName("CEN");
			CEN::SetDescription("Channel Enable");
		}

		using Super::operator =;
	private:
		unsigned int reg_num;
	};

	// STM Channel Interrupt Register (STM_CIRn)
	struct STM_CIR : STM_Register<STM_CIR>
	{
		typedef STM_Register<STM_CIR> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x14;

		struct CIF : Field<CIF, 31, 31, SW_R_W1C> {}; // Channel Enable
		
		typedef FieldSet<CIF> ALL;
		
		STM_CIR() : Super(0), reg_num(0) {}
		STM_CIR(STM<CONFIG> *_stm) : Super(_stm) {}
		STM_CIR(STM<CONFIG> *_stm, uint32_t value) : Super(_stm, value) {}
		
		void WithinRegisterFileCtor(unsigned int _reg_num, STM<CONFIG> *_stm)
		{
			this->stm = _stm;
			reg_num = _reg_num;
			
			std::stringstream name_sstr;
			name_sstr << "STM_CIR" << reg_num;
			this->SetName(name_sstr.str());
			std::stringstream description_sstr;
			description_sstr << "STM Channel Interrupt Register #" << reg_num;
			this->SetDescription(name_sstr.str());
			
			CIF::SetName("CIF");
			CIF::SetDescription("Channel Interrupt Flag");
		}

		virtual ReadWriteStatus Write(sc_core::sc_time& time_stamp, const uint32_t& value, const uint32_t& bit_enable)
		{
			// Write STM register
			ReadWriteStatus rws = this->Super::Write(time_stamp, value, bit_enable); // this may clear CIF
			
			unsigned int channel_num = reg_num;
			this->stm->SetIRQLevel(channel_num, this->template Get<typename STM_CIR::CIF>());
			return rws;
		}

		using Super::operator =;
	private:
		unsigned int reg_num;
	};

	// STM Channel Compare Register (STM_CMPn)
	struct STM_CMP : STM_Register<STM_CMP>
	{
		typedef STM_Register<STM_CMP> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x18;

		struct CMP : Field<CMP, 0, 31> {}; // Compare value for channel n
		
		typedef FieldSet<CMP> ALL;
		
		STM_CMP() : Super(0), reg_num(0) {}
		STM_CMP(STM<CONFIG> *_stm) : Super(_stm) {}
		STM_CMP(STM<CONFIG> *_stm, uint32_t value) : Super(_stm, value) {}
		
		void WithinRegisterFileCtor(unsigned int _reg_num, STM<CONFIG> *_stm)
		{
			this->stm = _stm;
			reg_num = _reg_num;
			
			std::stringstream name_sstr;
			name_sstr << "STM_CMP" << reg_num;
			this->SetName(name_sstr.str());
			std::stringstream description_sstr;
			description_sstr << "STM Channel Compare Register #" << reg_num;
			this->SetDescription(name_sstr.str());
			
			CMP::SetName("CMP");
			std::stringstream cmp_description_sstr;
			cmp_description_sstr << "Compare value for channel #" << reg_num;
			CMP::SetDescription(cmp_description_sstr.str());
		}

		using Super::operator =;
	private:
		unsigned int reg_num;
	};
	
	unisim::kernel::tlm2::ClockPropertiesProxy m_clk_prop_proxy; // proxy to get clock properties from clock port

	// STM Registers
	STM_CR                                                                        stm_cr;  // STM_CR
	STM_CNT                                                                       stm_cnt; // STM_CNT
	AddressableRegisterFile<STM_CCR, NUM_CHANNELS, STM<CONFIG>, sc_core::sc_time> stm_ccr; // STM_CCRn
	AddressableRegisterFile<STM_CIR, NUM_CHANNELS, STM<CONFIG>, sc_core::sc_time> stm_cir; // STM_CIRn
	AddressableRegisterFile<STM_CMP, NUM_CHANNELS, STM<CONFIG>, sc_core::sc_time> stm_cmp; // STM_CMPn
	
	// STM registers address map
	RegisterAddressMap<sc_dt::uint64, sc_core::sc_time> reg_addr_map;
	
	unisim::kernel::tlm2::Schedule<Event> schedule; // Payload (processor requests over AHB interface) schedule
	
	unisim::util::endian::endian_type endian;
	unisim::kernel::service::Parameter<unisim::util::endian::endian_type> param_endian;
	bool verbose;
	unisim::kernel::service::Parameter<bool> param_verbose;
	
	bool irq_level[NUM_CHANNELS];                   // IRQ output level for each channel
	sc_core::sc_event *gen_irq_event[NUM_CHANNELS]; // Event to trigger IRQ output (IRQ_Process)
	sc_core::sc_time last_counter_run_time;         // Last time when counter ran
	sc_core::sc_time prescaled_clock_period;        // Prescaled clock period
	sc_core::sc_event counter_run_event;            // Event to trigger counter run (RunCounterProcess)
	bool freeze;                                    // Latched value for internal "freeze"
	
	void ProcessEvent(Event *event);
	void ProcessEvents();
	void Process();
	void UpdatePrescaledClockPeriod();
	void IRQ_Process(unsigned int channel_num);
	
	void SetIRQLevel(unsigned int channel_num, bool level);
	void CompareCounter();
	void IncrementCounter(sc_dt::uint64 delta);
	sc_dt::uint64 TicksToNextCounterRun();
	sc_core::sc_time TimeToNextCounterRun();
	void RunCounterToTime(const sc_core::sc_time& time_stamp);
	void ScheduleCounterRun();
	void RunCounterProcess();
	void RefreshFreeze();
};

} // end of namespace stm
} // end of namespace mpc57xx
} // end of namespace freescale
} // end of namespace timer
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_TIMER_FREESCALE_MPC57XX_STM_STM_HH__
