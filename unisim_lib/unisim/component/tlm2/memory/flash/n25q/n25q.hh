/*
 *  Copyright (c) 2016,
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
 
#ifndef __UNISIM_COMPONENT_TLM2_MEMORY_FLASH_N25Q_N25Q_HH__
#define __UNISIM_COMPONENT_TLM2_MEMORY_FLASH_N25Q_N25Q_HH__

#include <unisim/kernel/service/service.hh>
#include <unisim/kernel/logger/logger.hh>
#include <unisim/kernel/tlm2/tlm.hh>
#include <unisim/component/tlm2/memory/flash/n25q/register.hh>
#include <unisim/component/tlm2/memory/flash/n25q/qspi_protocol.hh>

namespace unisim {
namespace component {
namespace tlm2 {
namespace memory {
namespace flash {
namespace n25q {

enum WrapMode
{
	WRAP_16B,
	WRAP_32B,
	WRAP_64B,
	WRAP_SEQ
};

template <typename CONFIG>
class N25Q
	: public unisim::kernel::service::Object
	, public sc_core::sc_module
	, public tlm::tlm_fw_transport_if<>
{
public:
	static const bool threaded_model = false;

	typedef tlm_qspi_protocol_types::tlm_payload_type tlm_payload_type;
	typedef tlm_qspi_protocol_types::tlm_phase_type tlm_phase_type;
	
	tlm::tlm_target_socket<4, tlm_qspi_protocol_types> qspi_slave_socket;
	sc_core::sc_in<bool> HOLD_RESET_N; // shared port: HOLD# or RESET#
	sc_core::sc_in<bool> W_N_VPP;      // shared port: W# or WPP
	
	N25Q(const sc_core::sc_module_name& name, unisim::kernel::service::Object *parent = 0);
	virtual ~N25Q();

	virtual bool get_direct_mem_ptr(tlm_payload_type& payload, tlm::tlm_dmi& dmi_data);
	virtual unsigned int transport_dbg(tlm_payload_type& payload);
	virtual tlm::tlm_sync_enum nb_transport_fw(tlm_payload_type& payload, tlm_phase_type& phase, sc_core::sc_time& t);
	virtual void b_transport(tlm_payload_type& payload, sc_core::sc_time& t);
private:
	struct StatusRegister : Register<8, ACC_RW, ACC_RW, BO_LITTLE_ENDIAN>
	{
		typedef Register<8, ACC_RW, ACC_RW, BO_LITTLE_ENDIAN> BaseType;
		
		BitField<StatusRegister, ACC_RW, ACC_RW, 7>    Status_register_write_enable_disable;  // nonvolatile
		BitField<StatusRegister, ACC_RW, ACC_RW, 5>    Top_bottom;                            // nonvolatile
		BitField<StatusRegister, ACC_RW, ACC_RW, 4, 2> Block_protect_2_0;                     // nonvolatile
		BitField<StatusRegister, ACC_RW, ACC_RO, 1>    Write_enable_latch;                    // volatile
		BitField<StatusRegister, ACC_RW, ACC_RO, 0>    Write_in_progress;                     // volatile

		StatusRegister(const std::string& name, const std::string& long_name, N25Q<CONFIG> *_n25q)
			: BaseType(name, long_name, 0xfc)
			, Status_register_write_enable_disable("Status_register_write_enable_disable", this, 0x1)
			, Top_bottom("Top_bottom", this, 0x1)
			, Block_protect_2_0("Block_protect_2_0", this, 0x7)
			, Write_enable_latch("Write_enable_latch", this, 0x0)
			, Write_in_progress("Write_in_progress", this, 0x0)
			, n25q(_n25q)
		{
		}
		
		using BaseType::operator const STORAGE_TYPE;
		using BaseType::operator = ;
		
		bool Write(const STORAGE_TYPE& value)
		{
			if(!BaseType::Write(value)) return false;
			
			if(Status_register_write_enable_disable && !n25q->W_N_VPP) // When the enable/disable bit is set to 1 and W#/VPP signal is driven LOW
			{
				// nonvolatile bits become read-only for SW
				SetSW_WriteMask(
					Write_enable_latch.SW_WriteMask() |
					Write_in_progress.SW_WriteMask()
				);
			}
			else
			{
				SetSW_WriteMask(
					Status_register_write_enable_disable.SW_WriteMask() |
					Top_bottom.SW_WriteMask() |
					Block_protect_2_0.SW_WriteMask() |
					Write_enable_latch.SW_WriteMask() |
					Write_in_progress.SW_WriteMask()
				);
			}
			
			return true;
		}
		
	private:
		N25Q<CONFIG> *n25q;
	};
	
	struct NonVolatileConfigurationRegister : Register<16, ACC_RW, ACC_RW, BO_LITTLE_ENDIAN>
	{
		typedef Register<16, ACC_RW, ACC_RW, BO_LITTLE_ENDIAN> BaseType;
		
		BitField<NonVolatileConfigurationRegister, ACC_RW, ACC_RW, 15, 12> Number_of_dummy_clock_cycles;
		BitField<NonVolatileConfigurationRegister, ACC_RW, ACC_RW, 11,  9> XIP_mode_at_power_on_reset;
		BitField<NonVolatileConfigurationRegister, ACC_RW, ACC_RW,  8,  6> Output_driver_strength;
		BitField<NonVolatileConfigurationRegister, ACC_RW, ACC_RW,  4>     Reset_hold;
		BitField<NonVolatileConfigurationRegister, ACC_RW, ACC_RW,  3>     Quad_IO_protocol;
		BitField<NonVolatileConfigurationRegister, ACC_RW, ACC_RW,  2>     Dual_IO_protocol;
		
		NonVolatileConfigurationRegister(const std::string& name, const std::string& long_name)
			: BaseType(name, long_name, 0xffff)
			, Number_of_dummy_clock_cycles("Number_of_dummy_clock_cycles", this, 0xf)
			, XIP_mode_at_power_on_reset("XIP_mode_at_power_on_reset", this, 0x7)
			, Output_driver_strength("Output_driver_strength", this, 0x7)
			, Reset_hold("Reset_hold", this, 0x1)
			, Quad_IO_protocol("Quad_IO_protocol", this, 0x1)
			, Dual_IO_protocol("Dual_IO_protocol", this, 0x1)
		{
		}
		
		using BaseType::operator const STORAGE_TYPE;
		using BaseType::operator = ;
	};
	
	struct VolatileConfigurationRegister : Register<8, ACC_RW, ACC_RW, BO_LITTLE_ENDIAN>
	{
		typedef Register<8, ACC_RW, ACC_RW, BO_LITTLE_ENDIAN> BaseType;
		
		BitField<VolatileConfigurationRegister, ACC_RW, ACC_RW, 7, 4> Number_of_dummy_clock_cycles;
		BitField<VolatileConfigurationRegister, ACC_RW, ACC_RW, 3>    XIP;
		BitField<VolatileConfigurationRegister, ACC_RW, ACC_RW, 1, 0> Wrap;
		
		VolatileConfigurationRegister(const std::string& name, const std::string& long_name)
			: BaseType(name, long_name, 0x0)
			, Number_of_dummy_clock_cycles("Number_of_dummy_clock_cycles", this, 0xf)
			, XIP("XIP", this, 0x1)
			, Wrap("Wrap", this, 0x3)
		{
		}
		
		using BaseType::operator const STORAGE_TYPE;
		using BaseType::operator = ;
	private:
		N25Q<CONFIG> *n25q;
	};
	
	struct EnhancedVolatileConfigurationRegister : Register<8, ACC_RW, ACC_RW, BO_LITTLE_ENDIAN>
	{
		typedef Register<8, ACC_RW, ACC_RW, BO_LITTLE_ENDIAN> BaseType;
		
		BitField<EnhancedVolatileConfigurationRegister, ACC_RW, ACC_RW, 7>    Quad_IO_protocol;
		BitField<EnhancedVolatileConfigurationRegister, ACC_RW, ACC_RW, 6>    Dual_IO_protocol;
		BitField<EnhancedVolatileConfigurationRegister, ACC_RW, ACC_RW, 4>    Reset_hold;
		BitField<EnhancedVolatileConfigurationRegister, ACC_RW, ACC_RW, 3>    VPP_accelerator;
		BitField<EnhancedVolatileConfigurationRegister, ACC_RW, ACC_RW, 2, 0> Output_driver_strength;
		
		EnhancedVolatileConfigurationRegister(const std::string& name, const std::string& long_name)
			: BaseType(name, long_name, 0x0)
			, Quad_IO_protocol("Quad_IO_protocol", this, 0x1)
			, Dual_IO_protocol("Dual_IO_protocol", this, 0x1)
			, Reset_hold("Reset_hold", this, 0x1)
			, VPP_accelerator("VPP_accelerator", this, 0x1)
			, Output_driver_strength("Output_driver_strength", this, 0x7)
		{
		}
		
		using BaseType::operator const STORAGE_TYPE;
		using BaseType::operator = ;
	};
	
	struct FlagStatusRegister : Register<8, ACC_RW, ACC_RW, BO_LITTLE_ENDIAN>
	{
		typedef Register<8, ACC_RW, ACC_RW, BO_LITTLE_ENDIAN> BaseType;
		
		BitField<FlagStatusRegister, ACC_RW, ACC_RW, 7> Program_or_erase_controller;
		BitField<FlagStatusRegister, ACC_RW, ACC_RW, 6> Erase_suspend;
		BitField<FlagStatusRegister, ACC_RW, ACC_RW, 5> Erase;
		BitField<FlagStatusRegister, ACC_RW, ACC_RW, 4> Program;
		BitField<FlagStatusRegister, ACC_RW, ACC_RW, 3> VPP;
		BitField<FlagStatusRegister, ACC_RW, ACC_RW, 2> Program_suspend;
		BitField<FlagStatusRegister, ACC_RW, ACC_RW, 1> Protection;
		
		FlagStatusRegister(const std::string& name, const std::string& long_name)
			: BaseType(name, long_name, 0xff)
			, Program_or_erase_controller("Program_or_erase_controller", this, 0x1)
			, Erase_suspend("Erase_suspend", this, 0x1)
			, Erase("Erase", this, 0x1)
			, Program("Program", this, 0x1)
			, VPP("VPP", this, 0x1)
			, Program_suspend("Program_suspend", this, 0x1)
			, Protection("Protection", this, 0x1)
		{
		}
		
		using BaseType::operator const STORAGE_TYPE;
		using BaseType::operator = ;
	};
	
	struct InternalConfigurationRegister : Register<16, ACC_RW, ACC_NONE, BO_LITTLE_ENDIAN>
	{
		typedef Register<16, ACC_RW, ACC_NONE, BO_LITTLE_ENDIAN> BaseType;
		
		BitField<InternalConfigurationRegister, ACC_RW, ACC_NONE, 15, 12> Number_of_dummy_clock_cycles;
		BitField<InternalConfigurationRegister, ACC_RW, ACC_NONE,  8,  6> Output_driver_strength;
		BitField<InternalConfigurationRegister, ACC_RW, ACC_NONE,  5>     VPP_accelerator;
		BitField<InternalConfigurationRegister, ACC_RW, ACC_NONE,  4>     Reset_hold;
		BitField<InternalConfigurationRegister, ACC_RW, ACC_NONE,  3>     Quad_IO_protocol;
		BitField<InternalConfigurationRegister, ACC_RW, ACC_NONE,  2>     Dual_IO_protocol;
		BitField<InternalConfigurationRegister, ACC_RW, ACC_NONE,  1, 0>  Wrap;
		
		InternalConfigurationRegister(const std::string& name, const std::string& long_name)
			: BaseType(name, long_name, 0xffff)
			, Number_of_dummy_clock_cycles("Number_of_dummy_clock_cycles", this, 0xf)
			, Output_driver_strength("Output_driver_strength", this, 0x7)
			, VPP_accelerator("VPP_accelerator", this, 0x1)
			, Reset_hold("Reset_hold", this, 0x1)
			, Quad_IO_protocol("Quad_IO_protocol", this, 0x1)
			, Dual_IO_protocol("Dual_IO_protocol", this, 0x1)
			, Wrap("Wrap", this, 0x3)
		{
		}

		using BaseType::operator const STORAGE_TYPE;
		using BaseType::operator = ;
	};
	
	struct LockRegister : Register<2, ACC_RW, ACC_NONE, BO_LITTLE_ENDIAN>
	{
		typedef Register<2, ACC_RW, ACC_NONE, BO_LITTLE_ENDIAN> BaseType;
		
		BitField<LockRegister, ACC_RW, ACC_NONE, 1> Sector_lock_down;
		BitField<LockRegister, ACC_RW, ACC_NONE, 0> Sector_write_lock;
		
		LockRegister(const std::string& name, const std::string& long_name)
			: BaseType(name, long_name, 0x3)
			, Sector_lock_down("Sector_lock_down", this, 0x1)
			, Sector_write_lock("Sector_write_lock", this, 0x1)
		{
		}
		
		using BaseType::operator const STORAGE_TYPE;
		using BaseType::operator = ;
	};
	
	enum Stage
	{
		START_OF_OPERATION,
		END_OF_OPERATION
	};
	
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
			, stage(START_OF_OPERATION)
			, post_time(sc_core::SC_ZERO_TIME)
			, elapsed_time(sc_core::SC_ZERO_TIME)
			, duration(sc_core::SC_ZERO_TIME)
			, suspended(false)
			, completion_event(0)
		{
		}
		
		~Event()
		{
			Clear();
		}
		
		void Clear()
		{
			if(payload) payload->release();
			key.Clear();
			payload = 0;
			stage = START_OF_OPERATION;
			post_time = sc_core::SC_ZERO_TIME;
			elapsed_time = sc_core::SC_ZERO_TIME;
			duration = sc_core::SC_ZERO_TIME;
			suspended  = false;
			completion_event  = 0;
		}
		
		void SetTimeStamp(const sc_core::sc_time& _time_stamp)
		{
			key.SetTimeStamp(_time_stamp);
		}
		
		void SetPayload(tlm::tlm_generic_payload *_payload)
		{
			payload = _payload;
		}
		
		void SetStage(Stage _stage)
		{
			stage = _stage;
		}
		
		void SetPostTime(const sc_core::sc_time& _post_time)
		{
			post_time = _post_time;
		}
		
		void Suspend()
		{
			suspended = true;
		}
		
		void Resume()
		{
			suspended = false;
		}
		
		void UpdateElapsedTime(const sc_core::sc_time& current_time)
		{
			elapsed_time += current_time;
			elapsed_time -= post_time;
		}
		
		void SetDuration(const sc_core::sc_time& t)
		{
			duration = t;
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
		
		Stage GetStage() const
		{
			return stage;
		}
		
		const sc_core::sc_time& GetDuration() const
		{
			return duration;
		}
		
		const sc_core::sc_time& GetElapsedTime() const
		{
			return elapsed_time;
		}

		bool Suspended() const
		{
			return suspended;
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
		tlm::tlm_generic_payload *payload;
		Stage stage;
		sc_core::sc_time post_time;              // the time when the event has been posted
		sc_core::sc_time elapsed_time;           // elapsed time of command
		sc_core::sc_time duration;               // duration of command
		bool suspended;
		sc_core::sc_event *completion_event;
	};
	
	unisim::kernel::logger::Logger logger;
	
	unisim::kernel::tlm2::PayloadFabric<tlm::tlm_generic_payload> payload_fabric;
	unisim::kernel::tlm2::Schedule<Event> schedule;
	std::stack<Event *> suspend_stack;              // at most one program and one erase
	Event *write_operation_in_progress_event;       // write operation in progress event
	
	sc_core::sc_time erase_resume_time_stamp;       // sector or bulk erase resume time stamp
	sc_core::sc_time program_resume_time_stamp;     // program resume time stamp
	sc_core::sc_time subsector_erase_resume_time_stamp; // subsector erase resume time stamp
	
	tlm_qspi_mode qspi_mode;
	tlm_qspi_command qspi_xip_cmd;               // QSPI FAST READ command that confirmed XIP mode
	
	StatusRegister                        SR;    // status register
	NonVolatileConfigurationRegister      NVCR;  // non volatile configuration register
	VolatileConfigurationRegister         VCR;   // volatile configuration register
	EnhancedVolatileConfigurationRegister VECR;  // enhanced volatile configuration register
	FlagStatusRegister                    FSR;   // flag status register
	InternalConfigurationRegister         ICR;   // internal configuration register (not visible to SW)
	
	LockRegister *LR[CONFIG::SIZE / CONFIG::SECTOR_SIZE]; // sector lock registers
	uint8_t *storage; // data array of CONFIG::SIZE bytes size
	uint8_t *OTP; // OTP array of CONFIG::OTP_ARRAY_SIZE bytes + 1 OTP control byte
	
	sc_core::sc_time tWNVCR;
	sc_core::sc_time tW;
	sc_core::sc_time tCFSR;
	sc_core::sc_time tWVCR;
	sc_core::sc_time tWRVECR;
	sc_core::sc_time tSHSL2;
	sc_core::sc_time tPOTP;
	sc_core::sc_time tPP;
	sc_core::sc_time tPP_VPP;
	sc_core::sc_time tSSE;
	sc_core::sc_time tSE;
	sc_core::sc_time tSE_VPP;
	sc_core::sc_time tBE;
	sc_core::sc_time tBE_VPP;
	sc_core::sc_time erase_resume_to_suspend;
	sc_core::sc_time program_resume_to_suspend;
	sc_core::sc_time subsector_erase_resume_to_suspend;
	sc_core::sc_time suspend_latency_program;
	sc_core::sc_time suspend_latency_subsector_erase;
	sc_core::sc_time suspend_latency_erase;
	
	void Reset();
	void Combine(uint8_t *dest, const uint8_t *source, uint32_t size);
	
	const tlm_qspi_protocol GetProtocol() const;
	uint32_t GetWrap() const;
	bool IsSectorProtected(sc_dt::uint64 addr);
	bool IsDeviceProtected();
	sc_core::sc_time ComputeRequestTime(tlm::tlm_generic_payload *payload);
	sc_core::sc_time ComputeResponseTime(tlm::tlm_generic_payload *payload);
	const sc_core::sc_time ComputeWriteOperationTime(tlm::tlm_generic_payload *payload);
	unsigned int DummyCycles(tlm_qspi_command qspi_cmd);
	Event *PostEndOfWriteOperation(tlm::tlm_generic_payload *payload);

	void ReadID(Event *event) const;
	void MultipleIOReadID(Event *event) const;
	void ReadSFDP(Event *event) const;
	void Read(Event *event) const;
	void WriteEnable();
	void WriteDisable();
	void ReadStatusRegister(Event *event) const;
	void ReadFlagStatusRegister(Event *event) const;
	void ReadNonVolatileConfigurationRegister(Event *event) const;
	void ReadVolatileConfigurationRegister(Event *event) const;
	void ReadEnhancedVolatileConfigurationRegister(Event *event) const;
	void ReadLockRegister(Event *event) const;
	void ReadOTPArray(Event *event) const;
	
	void WriteNonVolatileConfigurationRegister(Event *event);
	void WriteVolatileConfigurationRegister(Event *event);
	void WriteEnhancedVolatileConfigurationRegister(Event *event);
	void WriteStatusRegister(Event *event);
	void WriteLockRegister(Event *event);
	void ClearFlagStatusRegister();
	void ProgramOTPArray(Event *event);
	void Program(Event *event);
	void SubSectorErase(Event *event);
	void SectorErase(Event *event);
	void BulkErase(Event *event);
	void ProgramEraseSuspend(Event *suspend_event);
	void ProgramEraseResume(Event *resume_event);

	void Read(tlm::tlm_generic_payload *payload);
	void Write(tlm::tlm_generic_payload *payload);
	
	void ProcessEvents();
	void ProcessEvent(Event *event);
	void Process();
	void RESET_N_Process();
	void HOLD_N_Process();
};

} // end of namespace n25q
} // end of namespace flash
} // end of namespace memory
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_MEMORY_FLASH_N25Q_N25Q_HH__
