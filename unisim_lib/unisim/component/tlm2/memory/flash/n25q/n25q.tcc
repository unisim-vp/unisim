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
 
#ifndef __UNISIM_COMPONENT_TLM2_MEMORY_FLASH_N25Q_N25Q_TCC__
#define __UNISIM_COMPONENT_TLM2_MEMORY_FLASH_N25Q_N25Q_TCC__

#include <string.h>
#include <math.h>

namespace unisim {
namespace component {
namespace tlm2 {
namespace memory {
namespace flash {
namespace n25q {

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugError;

template <typename CONFIG>
N25Q<CONFIG>::N25Q(const sc_core::sc_module_name& name, unisim::kernel::service::Object *parent)
	: unisim::kernel::service::Object(name, parent)
	, sc_core::sc_module(name)
	, qspi_slave_socket("qspi_slave_socket")
	, HOLD_RESET_N("HOLD_RESET_N")
	, W_N_VPP("W_N_VPP")
	, logger(*this)
	, payload_fabric()
	, schedule()
	, suspend_stack()
	, write_operation_in_progress_event(0)
	, erase_resume_time_stamp(sc_core::SC_ZERO_TIME)
	, program_resume_time_stamp(sc_core::SC_ZERO_TIME)
	, subsector_erase_resume_time_stamp(sc_core::SC_ZERO_TIME)
	, qspi_mode(TLM_QSPI_STD_MODE)
	, qspi_xip_cmd(TLM_QSPI_UNKNOWN_COMMAND)
	, SR("SR", "Status Register", this)
	, NVCR("NVCR", "Nonvolatile Configuration Register")
	, VCR("VCR", "Volatile Configuration Register")
	, VECR("VECR", "Enhanced Volatile Configuration Register")
	, FSR("FSR", "Flag Status Register")
	, ICR("ICR", "Internal Configuration Register")
	, LR()
	, storage(0)
	, OTP(0)
	, tWNVCR(CONFIG::tWNVCR, sc_core::SC_SEC)
	, tW(CONFIG::tW, sc_core::SC_MS)
	, tCFSR(CONFIG::tCFSR, sc_core::SC_NS)
	, tWVCR(CONFIG::tWVCR, sc_core::SC_NS)
	, tWRVECR(CONFIG::tWRVECR, sc_core::SC_NS)
	, tSHSL2(CONFIG::tSHSL2, sc_core::SC_NS)
	, tPOTP(CONFIG::tPOTP, sc_core::SC_MS)
	, tPP(CONFIG::tPP, sc_core::SC_MS)
	, tPP_VPP(CONFIG::tPP_VPP, sc_core::SC_MS)
	, tSSE(CONFIG::tSSE, sc_core::SC_SEC)
	, tSE(CONFIG::tSE, sc_core::SC_SEC)
	, tSE_VPP(CONFIG::tSE_VPP, sc_core::SC_SEC)
	, tBE(CONFIG::tBE, sc_core::SC_SEC)
	, tBE_VPP(CONFIG::tBE_VPP, sc_core::SC_SEC)
	, erase_resume_to_suspend(CONFIG::erase_resume_to_suspend, sc_core::SC_US)
	, program_resume_to_suspend(CONFIG::program_resume_to_suspend, sc_core::SC_US)
	, subsector_erase_resume_to_suspend(CONFIG::subsector_erase_resume_to_suspend, sc_core::SC_US)
	, suspend_latency_program(CONFIG::suspend_latency_program, sc_core::SC_US)
	, suspend_latency_subsector_erase(CONFIG::suspend_latency_subsector_erase, sc_core::SC_US)
	, suspend_latency_erase(CONFIG::suspend_latency_erase, sc_core::SC_US)
{
	unsigned int idx;
	
	for(idx = 0; idx < CONFIG::SIZE / CONFIG::SECTOR_SIZE; idx++)
	{
		std::stringstream lr_name_sstr;
		lr_name_sstr << "LR_" << idx;
		std::stringstream lr_long_name_sstr;
		lr_long_name_sstr << "Lock Register #" << idx;
		LR[idx] = new LockRegister(lr_name_sstr.str(), lr_long_name_sstr.str());
	}
	
	storage = new uint8_t[CONFIG::SIZE];
	memset(storage, 0xff, CONFIG::SIZE); // bits are all 1's
	OTP = new uint8_t[CONFIG::OTP_ARRAY_SIZE + 1];
	
	Reset();
	
	SC_HAS_PROCESS(N25Q<CONFIG>);
	
	if(threaded_model)
	{
		SC_THREAD(Process);
	}
	else
	{
		SC_METHOD(Process);
	}
	
	if(CONFIG::HOLD_RST_PIN_F == CONFIG::RST_PIN)
	{
		SC_METHOD(RESET_N_Process);
		sensitive << HOLD_RESET_N.neg();
	}
	else if(CONFIG::HOLD_RST_PIN_F == CONFIG::HOLD_PIN)
	{
		SC_METHOD(HOLD_N_Process);
		sensitive << HOLD_RESET_N;
	}
}

template <typename CONFIG>
N25Q<CONFIG>::~N25Q()
{
	unsigned int idx;
	
	for(idx = 0; idx < CONFIG::SIZE / CONFIG::SECTOR_SIZE; idx++)
	{
		delete LR[idx];
	}
	
	delete storage;
	delete OTP;
}

template <typename CONFIG>
void N25Q<CONFIG>::Reset()
{
	SR.Reset();
	NVCR.Reset();
	VCR.Reset();
	VECR.Reset();
	FSR.Reset();
	ICR.Reset();
	
	// NVCR   VCR VECR
	//   |       |
	//   +---+---+
	//       |
	//       V
	//      ICR
	ICR.Number_of_dummy_clock_cycles = NVCR.Number_of_dummy_clock_cycles;
	ICR.Output_driver_strength = NVCR.Output_driver_strength;
	ICR.VPP_accelerator = VECR.VPP_accelerator;
	ICR.Reset_hold = NVCR.Reset_hold;
	ICR.Quad_IO_protocol = NVCR.Quad_IO_protocol;
	ICR.Dual_IO_protocol = NVCR.Dual_IO_protocol;
	ICR.Wrap = VCR.Wrap;
	
	switch(NVCR.XIP_mode_at_power_on_reset)
	{
		case 0x0:
			qspi_mode = TLM_QSPI_XIP_MODE;
			qspi_xip_cmd = TLM_QSPI_FAST_READ_COMMAND;
			break;
		case 0x1:
			qspi_mode = TLM_QSPI_XIP_MODE;
			qspi_xip_cmd = TLM_QSPI_DUAL_OUTPUT_FAST_READ_COMMAND;
			break;
		case 0x2:
			qspi_mode = TLM_QSPI_XIP_MODE;
			qspi_xip_cmd = TLM_QSPI_DUAL_INPUT_OUTPUT_FAST_READ_COMMAND;
			break;
		case 0x3:
			qspi_mode = TLM_QSPI_XIP_MODE;
			qspi_xip_cmd = TLM_QSPI_QUAD_OUTPUT_FAST_READ_COMMAND;
			break;
		case 0x4:
			qspi_mode = TLM_QSPI_XIP_MODE;
			qspi_xip_cmd = TLM_QSPI_QUAD_INPUT_OUTPUT_FAST_READ_COMMAND;
			break;
		default:
			qspi_mode = TLM_QSPI_STD_MODE;
			qspi_xip_cmd = TLM_QSPI_UNKNOWN_COMMAND;
			break;
	}
}

template <typename CONFIG>
bool N25Q<CONFIG>::get_direct_mem_ptr(tlm_payload_type& payload, tlm::tlm_dmi& dmi_data)
{
	dmi_data.set_granted_access(tlm::tlm_dmi::DMI_ACCESS_READ_WRITE);
	dmi_data.set_start_address(0);
	dmi_data.set_end_address(CONFIG::SIZE - 1);
	dmi_data.set_dmi_ptr(storage);
	return true;
}

template <typename CONFIG>
unsigned int N25Q<CONFIG>::transport_dbg(tlm_payload_type& payload)
{
	return 0;
}

template <typename CONFIG>
tlm::tlm_sync_enum N25Q<CONFIG>::nb_transport_fw(tlm_payload_type& payload, tlm_phase_type& phase, sc_core::sc_time& t)
{
	switch(phase)
	{
		case tlm::BEGIN_REQ:
			{
				t += ComputeRequestTime(&payload);
				
				sc_time notify_time_stamp(sc_core::sc_time_stamp());
				notify_time_stamp += t;
				Event *event = schedule.AllocEvent();
				event->SetPayload(&payload);
				event->SetTimeStamp(notify_time_stamp);
				schedule.Notify(event);
				phase = tlm::END_REQ;
				return tlm::TLM_UPDATED;
			}
			break;
		case tlm::END_RESP:
			return tlm::TLM_COMPLETED;
		default:
			logger << DebugError << "protocol error" << EndDebugError;
			Object::Stop(-1);
			break;
	}
	
	return tlm::TLM_COMPLETED;
}

template <typename CONFIG>
void N25Q<CONFIG>::b_transport(tlm_payload_type& payload, sc_core::sc_time& t)
{
	sc_core::sc_event completion_event;
	sc_core::sc_time notify_time_stamp(sc_core::sc_time_stamp());
	notify_time_stamp += t;
	Event *event = schedule.AllocEvent();
	event->SetPayload(&payload);
	event->SetTimeStamp(notify_time_stamp);
	event->SetCompletionEvent(&completion_event);
	schedule.Notify(event);
	sc_core::wait(completion_event);
	t = sc_core::SC_ZERO_TIME;
}

template <typename CONFIG>
void N25Q<CONFIG>::Combine(uint8_t *dest, const uint8_t *source, uint32_t size)
{
	// Do a AND between destination (flash memory content) and source (data to be programmed)
	// A 1 can be transformed into a 0, but a zero can't be transformed into a 1
	if(size > 0)
	{
		uint8_t *dst = dest;
		const uint8_t *src = source;
		do
		{
			*dst = (*dst) & (*src);
		} while(++dst, ++src, --size);
	}
}

template <typename CONFIG>
const tlm_qspi_protocol N25Q<CONFIG>::GetProtocol() const
{
	if(ICR.Quad_IO_protocol == 0) return TLM_QSPI_QUAD_IO_PROTOCOL;
	if(ICR.Dual_IO_protocol == 0) return TLM_QSPI_DUAL_IO_PROTOCOL;
	return TLM_QSPI_EXTENDED_PROTOCOL;
}

template <typename CONFIG>
uint32_t N25Q<CONFIG>::GetWrap() const
{
	if(ICR.Wrap == WRAP_16B) return 16;
	if(ICR.Wrap == WRAP_32B) return 32;
	if(ICR.Wrap == WRAP_64B) return 64;
	return CONFIG::SIZE;
}

template <typename CONFIG>
bool N25Q<CONFIG>::IsSectorProtected(sc_dt::uint64 addr)
{
	if(LR[addr / CONFIG::SECTOR_SIZE]->Sector_write_lock != 0) return true;
	
	return CONFIG::IsProtected(addr, SR.Top_bottom, SR.Block_protect_2_0);
}

template <typename CONFIG>
bool N25Q<CONFIG>::IsDeviceProtected()
{
	unsigned int i;
	
	for(i = 0; i < (CONFIG::SIZE / CONFIG::SECTOR_SIZE); i++)
	{
		if(LR[i]->Sector_write_lock != 0) return true;
	}
	
	return SR.Block_protect_2_0 != 0; // protected area: *NOT* none
}

template <typename CONFIG>
sc_core::sc_time N25Q<CONFIG>::ComputeRequestTime(tlm::tlm_generic_payload *payload)
{
	tlm_qspi_extension *qspi_ext = payload->get_extension<tlm_qspi_extension>();
	
	if(qspi_ext)
	{
		tlm_qspi_protocol qspi_protocol = GetProtocol();
		const sc_core::sc_time& cycle_period = qspi_ext->get_cycle_period();
		
		tlm_qspi_command qspi_cmd = (qspi_mode == TLM_QSPI_XIP_MODE) ? qspi_xip_cmd : qspi_ext->get_command();
		
		sc_core::sc_time request_time(cycle_period);
		
		unsigned int qspi_command_cycles = tlm_qspi_command_cycles(qspi_cmd, qspi_protocol, qspi_mode);
		unsigned int qspi_address_cycles = tlm_qspi_address_cycles(qspi_cmd, qspi_protocol, qspi_mode);
		unsigned int request_cycles = qspi_command_cycles + qspi_address_cycles;
		
		if(payload->is_write())
		{
			unsigned int data_length = payload->get_data_length();
			unsigned int qspi_data_cycles = data_length * tlm_qspi_data_cycles_per_byte(qspi_cmd, qspi_protocol, qspi_mode);
			request_cycles += qspi_data_cycles;
		}
		
		request_time *= request_cycles;
		
		return request_time;
	}
	
	return sc_core::SC_ZERO_TIME;
}

template <typename CONFIG>
sc_core::sc_time N25Q<CONFIG>::ComputeResponseTime(tlm::tlm_generic_payload *payload)
{
	if(payload->is_read())
	{
		tlm_qspi_extension *qspi_ext = payload->get_extension<tlm_qspi_extension>();
		
		if(qspi_ext)
		{
			tlm_qspi_protocol qspi_protocol = GetProtocol();
			const sc_core::sc_time& cycle_period = qspi_ext->get_cycle_period();
			tlm_qspi_command qspi_cmd = qspi_ext->get_command();
			
			sc_core::sc_time response_time(cycle_period);
			
			unsigned int data_length = payload->get_data_length();
			
			unsigned int qspi_dummy_cycles = DummyCycles(qspi_cmd);
			unsigned int qspi_data_cycles = data_length * tlm_qspi_data_cycles_per_byte(qspi_cmd, qspi_protocol, qspi_mode);
			unsigned int response_cycles = qspi_dummy_cycles + qspi_data_cycles;
			response_time *= response_cycles;
			
			return response_time;
		}
	}
	
	return sc_core::SC_ZERO_TIME;
}

template <typename CONFIG>
const sc_core::sc_time N25Q<CONFIG>::ComputeWriteOperationTime(tlm::tlm_generic_payload *payload)
{
	tlm_qspi_extension *qspi_ext = payload->get_extension<tlm_qspi_extension>();
	
	if(qspi_ext)
	{
		tlm_qspi_command qspi_cmd = qspi_ext->get_command();
		
		switch(qspi_cmd)
		{
			case TLM_QSPI_WRITE_NONVOLATILE_CONFIGURATION_REGISTER_COMMAND: return tWNVCR;
			case TLM_QSPI_WRITE_STATUS_REGISTER_COMMAND: return tW;
			case TLM_QSPI_CLEAR_FLAG_STATUS_REGISTER_COMMAND: return tCFSR;
			case TLM_QSPI_WRITE_VOLATILE_CONFIGURATION_REGISTER_COMMAND: return tWVCR;
			case TLM_QSPI_WRITE_ENHANCED_VOLATILE_CONFIGURATION_REGISTER_COMMAND: return tWRVECR;
			case TLM_QSPI_WRITE_LOCK_REGISTER_COMMAND: return tSHSL2;
			case TLM_QSPI_PROGRAM_OTP_ARRAY_COMMAND: return tPOTP;
			case TLM_QSPI_PAGE_PROGRAM_COMMAND:
			case TLM_QSPI_DUAL_INPUT_FAST_PROGRAM_COMMAND:
			case TLM_QSPI_EXTENDED_DUAL_INPUT_FAST_PROGRAM_COMMAND:
			case TLM_QSPI_QUAD_INPUT_FAST_PROGRAM_COMMAND:
			case TLM_QSPI_EXTENDED_QUAD_INPUT_FAST_PROGRAM_COMMAND: return ceil(payload->get_data_length() / 8) * (W_N_VPP ? tPP_VPP : tPP);
			case TLM_QSPI_SUBSECTOR_ERASE_COMMAND: return tSSE;
			case TLM_QSPI_SECTOR_ERASE_COMMAND: return W_N_VPP ? tSE_VPP : tSE;
			case TLM_QSPI_BULK_ERASE_COMMAND: return W_N_VPP ? tBE_VPP : tBE;
			default: return sc_core::SC_ZERO_TIME;
		}
	}
	
	return sc_core::SC_ZERO_TIME;
}

template <typename CONFIG>
unsigned int N25Q<CONFIG>::DummyCycles(tlm_qspi_command qspi_cmd)
{
	switch(qspi_cmd)
	{
		case TLM_QSPI_FAST_READ_COMMAND:
		case TLM_QSPI_DUAL_OUTPUT_FAST_READ_COMMAND:
		case TLM_QSPI_DUAL_INPUT_OUTPUT_FAST_READ_COMMAND:
		case TLM_QSPI_QUAD_OUTPUT_FAST_READ_COMMAND:
		case TLM_QSPI_QUAD_INPUT_OUTPUT_FAST_READ_COMMAND:
			{
				unsigned int number_of_dummy_clock_cycles = ICR.Number_of_dummy_clock_cycles;
				if(number_of_dummy_clock_cycles != 0) return number_of_dummy_clock_cycles;
			}
			break;
		default:
			break;
	}
	
	tlm_qspi_protocol qspi_protocol = GetProtocol();
	
	return tlm_qspi_dummy_cycles(qspi_cmd, qspi_protocol, qspi_mode);
}

template <typename CONFIG>
typename N25Q<CONFIG>::Event *N25Q<CONFIG>::PostEndOfWriteOperation(tlm::tlm_generic_payload *payload)
{
	tlm_qspi_extension *qspi_ext = payload->get_extension<tlm_qspi_extension>();
	
	if(qspi_ext)
	{
		Event *end_of_operation_event = schedule.AllocEvent();
		
		tlm::tlm_generic_payload *clone_payload = payload_fabric.allocate();
		clone_payload->deep_copy_from(*payload);
		
		sc_core::sc_time operation_duration = ComputeWriteOperationTime(payload);
		
		sc_core::sc_time end_of_operation_time_stamp(sc_core::sc_time_stamp());
		end_of_operation_time_stamp += operation_duration;

		end_of_operation_event->SetTimeStamp(end_of_operation_time_stamp);
		end_of_operation_event->SetPayload(clone_payload);
		end_of_operation_event->SetStage(END_OF_OPERATION);
		end_of_operation_event->SetPostTime(sc_core::sc_time_stamp());
		
		schedule.Notify(end_of_operation_event);
		
		return end_of_operation_event;
	}
	
	return 0;
}

template <typename CONFIG>
void N25Q<CONFIG>::ReadID(Event *event) const
{
	tlm::tlm_generic_payload *payload = event->GetPayload();
	unsigned int data_length = payload->get_data_length();
	unsigned char *data_ptr = payload->get_data_ptr();

	unsigned int data_to_copy = data_length;
	if(data_to_copy > CONFIG::ID_SIZE) data_to_copy = CONFIG::ID_SIZE;
	memcpy(data_ptr, CONFIG::ID, data_to_copy);
	if(data_length > data_to_copy)
	{
		memset(data_ptr + data_to_copy, 0, data_length - data_to_copy);
	}
}

template <typename CONFIG>
void N25Q<CONFIG>::MultipleIOReadID(Event *event) const
{
	tlm::tlm_generic_payload *payload = event->GetPayload();
	unsigned int data_length = payload->get_data_length();
	unsigned char *data_ptr = payload->get_data_ptr();

	unsigned int data_to_copy = data_length;
	if(data_to_copy > 3) data_to_copy = 3; // Note: Multiple I/O READ ID command can only read first 3 bytes of ID
	memcpy(data_ptr, CONFIG::ID, data_to_copy);
	if(data_length > data_to_copy)
	{
		memset(data_ptr + data_to_copy, 0, data_length - data_to_copy);
	}
}

template <typename CONFIG>
void N25Q<CONFIG>::ReadSFDP(Event *event) const
{
	tlm::tlm_generic_payload *payload = event->GetPayload();
	sc_dt::uint64 addr = payload->get_address();
	unsigned int data_length = payload->get_data_length();
	unsigned char *data_ptr = payload->get_data_ptr();

	unsigned int offset;
	unsigned int boundary = 2048; // When the 2048-byte boundary is reached,
	                              // the data output wraps to address 0 of the serial
	                              // Flash discovery parameter table.
	
	if(addr >= boundary) addr = 0;
	
	unsigned int moved = 0;
	
	for(offset = 0; offset < data_length; offset += moved)
	{
		uint32_t data_to_copy = data_length - offset;
		uint32_t data_to_zero = 0;
		if(data_to_copy > (CONFIG::SFDP_SIZE - addr))
		{
			data_to_zero = data_length - data_to_copy;
			if(data_to_zero > (boundary - CONFIG::SFDP_SIZE)) data_to_zero = boundary - CONFIG::SFDP_SIZE;
			data_to_copy = CONFIG::SFDP_SIZE - addr;
		}
		
		memcpy(data_ptr + offset, CONFIG::SFDP + addr, data_to_copy);
		if(data_to_zero)
		{
			memset(data_ptr + offset + data_to_copy, 0, data_to_zero);
		}
		
		moved = data_to_copy + data_to_zero;
	}
}

template <typename CONFIG>
void N25Q<CONFIG>::Read(Event *event) const
{
	tlm::tlm_generic_payload *payload = event->GetPayload();
	sc_dt::uint64 addr = payload->get_address();
	unsigned int data_length = payload->get_data_length();
	unsigned char *data_ptr = payload->get_data_ptr();

	unsigned int offset = 0;
	uint32_t wrap = GetWrap();
	
	unsigned int copied = 0;
	
	for(offset = 0; offset < data_length; offset += copied)
	{
		uint32_t wrapped_addr_offset = addr & (wrap - 1);
		uint32_t size_to_copy = wrap - wrapped_addr_offset;
		if(size_to_copy > (data_length - offset)) size_to_copy = data_length - offset;
		if(size_to_copy > (CONFIG::SIZE - addr)) size_to_copy = CONFIG::SIZE - addr;
		
		memcpy(data_ptr + offset, storage + addr, size_to_copy);
		
		copied = size_to_copy;
		
		if(wrapped_addr_offset)
		{
			addr += copied;
		}
	}
}

template <typename CONFIG>
void N25Q<CONFIG>::WriteEnable()
{
	SR.Write_enable_latch = 1;
}

template <typename CONFIG>
void N25Q<CONFIG>::WriteDisable()
{
	SR.Write_enable_latch = 0;
}

template <typename CONFIG>
void N25Q<CONFIG>::ReadStatusRegister(Event *event) const
{
	tlm::tlm_generic_payload *payload = event->GetPayload();
	unsigned int data_length = payload->get_data_length();
	unsigned char *data_ptr = payload->get_data_ptr();

	uint8_t sr_value = SR;
	memset(data_ptr, sr_value, data_length);
}

template <typename CONFIG>
void N25Q<CONFIG>::ReadFlagStatusRegister(Event *event) const
{
	tlm::tlm_generic_payload *payload = event->GetPayload();
	unsigned int data_length = payload->get_data_length();
	unsigned char *data_ptr = payload->get_data_ptr();

	uint8_t fsr_value = FSR;
	memset(data_ptr, fsr_value, data_length);
}

template <typename CONFIG>
void N25Q<CONFIG>::ReadNonVolatileConfigurationRegister(Event *event) const
{
	tlm::tlm_generic_payload *payload = event->GetPayload();
	unsigned int data_length = payload->get_data_length();
	unsigned char *data_ptr = payload->get_data_ptr();

	uint16_t nvcr_value = NVCR;
	if(data_length > 0) data_ptr[0] = nvcr_value;
	if(data_length > 1) data_ptr[1] = nvcr_value >> 8;
	if(data_length > 2) memset(data_ptr + 2, 0, data_length - 2);
}

template <typename CONFIG>
void N25Q<CONFIG>::ReadVolatileConfigurationRegister(Event *event) const
{
	tlm::tlm_generic_payload *payload = event->GetPayload();
	unsigned int data_length = payload->get_data_length();
	unsigned char *data_ptr = payload->get_data_ptr();

	uint8_t vcr_value = VCR;
	memset(data_ptr, vcr_value, data_length);
}

template <typename CONFIG>
void N25Q<CONFIG>::ReadEnhancedVolatileConfigurationRegister(Event *event) const
{
	tlm::tlm_generic_payload *payload = event->GetPayload();
	unsigned int data_length = payload->get_data_length();
	unsigned char *data_ptr = payload->get_data_ptr();

	uint8_t vecr_value = VECR;
	memset(data_ptr, vecr_value, data_length);
}

template <typename CONFIG>
void N25Q<CONFIG>::ReadLockRegister(Event *event) const
{
	tlm::tlm_generic_payload *payload = event->GetPayload();
	sc_dt::uint64 addr = payload->get_address();
	unsigned int data_length = payload->get_data_length();
	unsigned char *data_ptr = payload->get_data_ptr();

	uint8_t lr_value = (addr < CONFIG::SIZE) ? (const uint8_t) *LR[addr / CONFIG::SECTOR_SIZE] : 0;
	memset(data_ptr, lr_value, data_length);
}

template <typename CONFIG>
void N25Q<CONFIG>::ReadOTPArray(Event *event) const
{
	tlm::tlm_generic_payload *payload = event->GetPayload();
	sc_dt::uint64 addr = payload->get_address();
	unsigned int data_length = payload->get_data_length();
	unsigned char *data_ptr = payload->get_data_ptr();

	if(addr < CONFIG::OTP_ARRAY_SIZE)
	{
		unsigned int data_to_copy = data_length;
		if(data_to_copy > (CONFIG::OTP_ARRAY_SIZE - addr)) data_to_copy = CONFIG::OTP_ARRAY_SIZE - addr;
		
		memcpy(data_ptr, OTP + addr, data_to_copy);
		
		if(data_length > data_to_copy)
		{
			unsigned int dup_length = data_length - data_to_copy;
			memset(data_ptr + data_to_copy, OTP[CONFIG::OTP_ARRAY_SIZE - 1], dup_length);
		}
	}
	else
	{
		memset(data_ptr, 0, data_length);
	}
}

template <typename CONFIG>
void N25Q<CONFIG>::WriteNonVolatileConfigurationRegister(Event *event)
{
	tlm::tlm_generic_payload *payload = event->GetPayload();
	unsigned int data_length = payload->get_data_length();
	
	switch(event->GetStage())
	{
		case START_OF_OPERATION:
			{
				if(data_length != 2)
				{
					logger << DebugWarning << "data length for " << TLM_QSPI_WRITE_NONVOLATILE_CONFIGURATION_REGISTER_COMMAND << " should be 2" << EndDebugWarning;
				}

				if(SR.Write_enable_latch)
				{
					SR.Write_enable_latch = 0;
					PostEndOfWriteOperation(payload);
					// When the operation is in progress, the write in progress bit is set to 1.
					SR.Write_in_progress = 1;
					FSR.Program_or_erase_controller = 0; // busy
				}
				else
				{
					logger << DebugWarning << "Attempt to write " << NVCR.GetLongName() << " while " << SR.Write_enable_latch.GetName() << " = '0'" << EndDebugWarning;
				}
			}
			break;
			
		case END_OF_OPERATION:
			{
				unsigned char *data_ptr = payload->get_data_ptr();

				if(!data_length)
				{
					logger << DebugError << "data length for " << TLM_QSPI_WRITE_NONVOLATILE_CONFIGURATION_REGISTER_COMMAND << " shall be > 0" << EndDebugError;
					Object::Stop(-1);
					return;
				}
				
				uint16_t new_nvcr_value = data_ptr[0];
				if(data_length >= 2) new_nvcr_value |= ((uint16_t) data_ptr[1] << 8);
				NVCR.Write(new_nvcr_value);
				// When the operation completes, the write in progress bit is cleared to 0, whether the operation is successful or not.
				SR.Write_in_progress = 0;
				FSR.Program_or_erase_controller = 1; // ready
			}
			break;
	}
}

template <typename CONFIG>
void N25Q<CONFIG>::WriteVolatileConfigurationRegister(Event *event)
{
	tlm::tlm_generic_payload *payload = event->GetPayload();
	unsigned int data_length = payload->get_data_length();

	switch(event->GetStage())
	{
		case START_OF_OPERATION:
			{
				if(data_length != 1)
				{
					logger << DebugWarning << "data length for " << TLM_QSPI_WRITE_VOLATILE_CONFIGURATION_REGISTER_COMMAND << " should be 1" << EndDebugWarning;
				}
				
				if(SR.Write_enable_latch)
				{
					PostEndOfWriteOperation(payload);
				}
				else
				{
					logger << DebugWarning << "Attempt to write " << VCR.GetLongName() << " while " << SR.Write_enable_latch.GetName() << " = '0'" << EndDebugWarning;
				}
			}
			break;
		case END_OF_OPERATION:
			{
				if(!data_length)
				{
					logger << DebugError << "data length for " << TLM_QSPI_WRITE_VOLATILE_CONFIGURATION_REGISTER_COMMAND << " shall be > 0" << EndDebugError;
					Object::Stop(-1);
					return;
				}

				unsigned char *data_ptr = payload->get_data_ptr();
	
				uint8_t new_vcr_value = *data_ptr;
				VCR.Write(new_vcr_value);
				ICR.Number_of_dummy_clock_cycles = VCR.Number_of_dummy_clock_cycles;
				ICR.Wrap = VCR.Wrap;
			}
			break;
	}
}

template <typename CONFIG>
void N25Q<CONFIG>::WriteEnhancedVolatileConfigurationRegister(Event *event)
{
	tlm::tlm_generic_payload *payload = event->GetPayload();
	unsigned int data_length = payload->get_data_length();
	
	switch(event->GetStage())
	{
		case START_OF_OPERATION:
			{
				if(data_length != 1)
				{
					logger << DebugWarning << "data length for " << TLM_QSPI_WRITE_ENHANCED_VOLATILE_CONFIGURATION_REGISTER_COMMAND << " should be 1" << EndDebugWarning;
				}

				if(SR.Write_enable_latch)
				{
					PostEndOfWriteOperation(payload);
				}
				else
				{
					logger << DebugWarning << "Attempt to write " << VECR.GetLongName() << " while " << SR.Write_enable_latch.GetName() << " = '0'" << EndDebugWarning;
				}
			}
			break;
			
		case END_OF_OPERATION:
			{
				unsigned char *data_ptr = payload->get_data_ptr();

				if(!data_length)
				{
					logger << DebugError << "data length for " << TLM_QSPI_WRITE_ENHANCED_VOLATILE_CONFIGURATION_REGISTER_COMMAND << " shall be > 0" << EndDebugError;
					Object::Stop(-1);
					return;
				}
	
				uint8_t new_vecr_value = *data_ptr;
				VECR.Write(new_vecr_value);
				ICR.Output_driver_strength = VECR.Output_driver_strength;
				ICR.VPP_accelerator = VECR.VPP_accelerator;
				ICR.Reset_hold = VECR.Reset_hold;
				ICR.Quad_IO_protocol = VECR.Quad_IO_protocol;
				ICR.Dual_IO_protocol = VECR.Dual_IO_protocol;
			}
			break;
	}
}

template <typename CONFIG>
void N25Q<CONFIG>::WriteStatusRegister(Event *event)
{
	tlm::tlm_generic_payload *payload = event->GetPayload();
	unsigned int data_length = payload->get_data_length();

	switch(event->GetStage())
	{
		case START_OF_OPERATION:
			{
				if(data_length != 1)
				{
					logger << DebugWarning << "data length for " << TLM_QSPI_WRITE_STATUS_REGISTER_COMMAND << " should be 1" << EndDebugWarning;
				}
				
				if(SR.Write_enable_latch)
				{
					// The write enable latch bit is cleared to 0, whether the operation is successful or not.
					SR.Write_enable_latch = 0;
					PostEndOfWriteOperation(payload);
					// When the operation is in progress, the write in progress bit is set to 1.
					SR.Write_in_progress = 1;
					FSR.Program_or_erase_controller = 0; // busy
				}
				else
				{
					logger << DebugWarning << "Attempt to write " << SR.GetLongName() << " while " << SR.Write_enable_latch.GetName() << " = '0'" << EndDebugWarning;
				}
			}
			break;
		case END_OF_OPERATION:
			{
				if(!data_length)
				{
					logger << DebugError << "data length for " << TLM_QSPI_WRITE_STATUS_REGISTER_COMMAND << " shall be > 0" << EndDebugError;
					Object::Stop(-1);
					return;
				}

				unsigned char *data_ptr = payload->get_data_ptr();
					
				uint8_t new_sr_value = *data_ptr;
				SR.Write(new_sr_value);
				// When the operation completes, the write in progress bit is cleared to 0, whether the operation is successful or not.
				SR.Write_in_progress = 0;
				FSR.Program_or_erase_controller = 1; // ready
			}
			break;
	}
}

template <typename CONFIG>
void N25Q<CONFIG>::WriteLockRegister(Event *event)
{
	switch(event->GetStage())
	{
		case START_OF_OPERATION:
			{
				tlm::tlm_generic_payload *payload = event->GetPayload();
				sc_dt::uint64 addr = payload->get_address();
				unsigned int data_length = payload->get_data_length();
				unsigned char *data_ptr = payload->get_data_ptr();
				
				LockRegister *lr = LR[addr / CONFIG::SECTOR_SIZE];

				if(data_length != 2)
				{
					logger << DebugWarning << "data length for " << TLM_QSPI_WRITE_LOCK_REGISTER_COMMAND << " should be 1" << EndDebugWarning;
				}
				
				if(SR.Write_enable_latch)
				{
					SR.Write_enable_latch = 0;
					if(lr->Sector_lock_down)
					{
						// Because lock register bits are volatile, change to the bits is immediate.
						uint8_t new_lr_value = *data_ptr;
						lr->Write(new_lr_value);
						
						PostEndOfWriteOperation(payload);
					}
					else
					{
						logger << DebugWarning << "Attempt to write " << lr->GetLongName() << " while " << lr->Sector_lock_down.GetName() << " = '1'" << EndDebugWarning;
					}
				}
				else
				{
					logger << DebugWarning << "Attempt to write " << lr->GetLongName() << " while " << SR.Write_enable_latch.GetName() << " = '0'" << EndDebugWarning;
				}
			}
			break;
			
		case END_OF_OPERATION:
			// When execution is complete, the write enable latch bit is cleared within tSHSL2 and no error bits are set.
			SR.Write_enable_latch = 0;
			break;
	}
}

template <typename CONFIG>
void N25Q<CONFIG>::ClearFlagStatusRegister()
{
	FSR.Erase = 0;
	FSR.Program = 0;
	FSR.Protection = 0;
}

template <typename CONFIG>
void N25Q<CONFIG>::ProgramOTPArray(Event *event)
{
	tlm::tlm_generic_payload *payload = event->GetPayload();
	unsigned int data_length = payload->get_data_length();

	switch(event->GetStage())
	{
		case START_OF_OPERATION:
			{
				if(SR.Write_enable_latch)
				{
					// The write enable latch bit is cleared to 0, whether the operation is successful or not, and
					// the status register and flag status register can be polled for the operation status.
					SR.Write_enable_latch = 0;
					
					if(OTP[CONFIG::OTP_ARRAY_SIZE] & 1)
					{
						// unlocked
						PostEndOfWriteOperation(payload);
						
						// When the operation is in progress, the write in progress bit is set to 1.
						SR.Write_in_progress = 1;
					}
					else
					{
						// permanently locked
						logger << DebugWarning << "Attempt to program permanently locked OTP Array" << EndDebugWarning;
					}
				}
				else
				{
					// To initiate the PROGRAM OTP ARRAY command, the WRITE ENABLE command must
					// be issued to set the write enable latch bit to 1; otherwise, the PROGRAM OTP ARRAY
					// command is ignored and flag status register bits are not set.
					logger << DebugWarning << "Attempt to program OTP Array while " << SR.Write_enable_latch.GetName() << " = '0'" << EndDebugWarning;
				}
			}
			break;
		case END_OF_OPERATION:
			{
				if(!data_length)
				{
					logger << DebugError << "data length for " << TLM_QSPI_WRITE_STATUS_REGISTER_COMMAND << " shall be > 0" << EndDebugError;
					Object::Stop(-1);
					return;
				}

				sc_dt::uint64 addr = payload->get_address();
				unsigned char *data_ptr = payload->get_data_ptr();

				if(addr < 65)
				{
					unsigned int data_to_copy = data_length;
					if(data_to_copy > (65 - addr)) data_to_copy = 65 - addr;
					
					memcpy(OTP + addr, data_ptr, data_to_copy);
				}
				
				// When the operation completes, the write in progress bit is cleared to 0.
				SR.Write_in_progress = 0;
			}
			break;
	}
}

template <typename CONFIG>
void N25Q<CONFIG>::Program(Event *event)
{
	tlm::tlm_generic_payload *payload = event->GetPayload();
	sc_dt::uint64 addr = payload->get_address();
	unsigned int data_length = payload->get_data_length();

	switch(event->GetStage())
	{
		case START_OF_OPERATION:
			{
				if(SR.Write_enable_latch)
				{
					if(!IsSectorProtected(addr))
					{
						// The write enable latch bit is cleared to 0, whether the operation is successful or not.
						SR.Write_enable_latch = 0;

						Event *end_of_operation_event = PostEndOfWriteOperation(payload);
						write_operation_in_progress_event = end_of_operation_event;
						
						// When the operation is in progress, the write in progress bit is set to 1.
						SR.Write_in_progress = 1;
						FSR.Program_or_erase_controller = 0; // busy
					}
					else
					{
						// When a command is applied to a protected sector,
						// the command is not executed, the write enable latch bit remains
						// set to 1, and flag status register bits 1 and 4 are set.
						FSR.Protection = 1; // protection error
						FSR.Program = 1; // protection error
						logger << DebugWarning << "Attempt to program a page within a locked sector" << EndDebugWarning;
					}
				}
				else
				{
					logger << DebugWarning << "Attempt to program while " << SR.Write_enable_latch.GetName() << " = '0'" << EndDebugWarning;
				}
			}
			break;
			
		case END_OF_OPERATION:
			{
				unsigned char *data_ptr = payload->get_data_ptr();
				unsigned int data_offset = 0;

				sc_dt::uint64 page_offset = addr & (CONFIG::PAGE_SIZE - 1);
		
				unsigned int data_to_program = data_length;
				if(data_to_program > CONFIG::PAGE_SIZE)
				{
					// If the number of bytes sent to the device exceed the maximum page size,
					// previously latched data is discarded and only the last maximum page size
					// number of data bytes are guaranteed to be programmed correctly within the same page.
					data_offset = data_length - CONFIG::PAGE_SIZE;
					data_to_program = CONFIG::PAGE_SIZE;
				}
				
				sc_dt::uint64 size_to_page_boundary = CONFIG::PAGE_SIZE - page_offset;
				
				if(page_offset && (data_to_program > size_to_page_boundary))
				{
					// If the bits of the least significant address, which is the starting address, are not all zero,
					// all data transmitted beyond the end of the current page is programmed from the starting address of the same page.
					sc_dt::uint64 page_base_addr = addr & ~(CONFIG::PAGE_SIZE - 1);
					
					Combine(storage + addr, data_ptr + data_offset, size_to_page_boundary);
					Combine(storage + page_base_addr, data_ptr + data_offset + size_to_page_boundary, data_to_program - size_to_page_boundary);
				}
				else
				{
					Combine(storage + addr, data_ptr + data_offset, data_to_program);
				}
				
				// When the operation completes, the write in progress bit is cleared to 0.
				SR.Write_in_progress = 0;
				FSR.Program_or_erase_controller = 1; // ready
			}
			break;
	}
}

template <typename CONFIG>
void N25Q<CONFIG>::SubSectorErase(Event *event)
{
	tlm::tlm_generic_payload *payload = event->GetPayload();
	sc_dt::uint64 addr = payload->get_address();
	unsigned int data_length = payload->get_data_length();
	
	switch(event->GetStage())
	{
		case START_OF_OPERATION:
			{
				if(SR.Write_enable_latch)
				{
					if(!IsSectorProtected(addr))
					{
						// The write enable latch bit is cleared to 0, whether the operation is successful or not.
						SR.Write_enable_latch = 0;
						Event *end_of_operation_event = PostEndOfWriteOperation(payload);
						write_operation_in_progress_event = end_of_operation_event;
						// When the operation is in progress, the write in progress bit is set to 1. 
						SR.Write_in_progress = 1;
						FSR.Program_or_erase_controller = 0; // busy
					}
					else
					{
						// When a command is applied to a protected subsector, the command is not executed.
						// Instead, the write enable latch bit remains set to 1, and flag status register bits 1 and 5 are set.
						FSR.Protection = 1; // protection error
						FSR.Erase = 1; // protection error
						logger << DebugWarning << "Attempt to erase a subsector within a locked sector" << EndDebugWarning;
					}
				}
				else
				{
					// If the write enable latch bit is not set, the device ignores the SUBSECTOR ERASE command
					// and no error bits are set to indicate operation failure.
					logger << DebugWarning << "Attempt to perform " << TLM_QSPI_SUBSECTOR_ERASE_COMMAND << " while " << SR.Write_enable_latch.GetName() << " = '0'" << EndDebugWarning;
				}
			}
			break;
		
		case END_OF_OPERATION:
			{
				sc_dt::uint64 sector_base_addr = addr & ~(CONFIG::SUBSECTOR_SIZE - 1);
				memset(storage + sector_base_addr, 0xff, CONFIG::SUBSECTOR_SIZE);
				FSR.Program_or_erase_controller = 1; // ready
			}
			break;
	}
}

template <typename CONFIG>
void N25Q<CONFIG>::SectorErase(Event *event)
{
	tlm::tlm_generic_payload *payload = event->GetPayload();
	sc_dt::uint64 addr = payload->get_address();
	
	switch(event->GetStage())
	{
		case START_OF_OPERATION:
			{
				if(SR.Write_enable_latch)
				{
					if(!IsSectorProtected(addr))
					{
						// The write enable latch bit is cleared to 0, whether the operation is successful or not.
						SR.Write_enable_latch = 0;
						Event *end_of_operation_event = PostEndOfWriteOperation(payload);
						write_operation_in_progress_event = end_of_operation_event;
						// When the operation is in progress, the write in progress bit is set to 1. 
						SR.Write_in_progress = 1;
						FSR.Program_or_erase_controller = 0; // busy
					}
					else
					{
						// When a command is applied to a protected subsector, the command is not executed.
						// Instead, the write enable latch bit remains set to 1, and flag status register bits 1 and 5 are set.
						FSR.Protection = 1; // protection error
						FSR.Erase = 1; // protection error
						logger << DebugWarning << "Attempt to erase a locked sector" << EndDebugWarning;
					}
				}
				else
				{
					// If the write enable latch bit is not set, the device ignores the SECTOR ERASE command
					// and no error bits are set to indicate operation failure.
					logger << DebugWarning << "Attempt to perform " << TLM_QSPI_SECTOR_ERASE_COMMAND << " while " << SR.Write_enable_latch.GetName() << " = '0'" << EndDebugWarning;
				}
			}
			break;
		
		case END_OF_OPERATION:
			{
				sc_dt::uint64 sector_base_addr = addr & ~(CONFIG::SECTOR_SIZE - 1);
				memset(storage + sector_base_addr, 0xff, CONFIG::SECTOR_SIZE);
				FSR.Program_or_erase_controller = 1; // ready
			}
			break;
	}
}

template <typename CONFIG>
void N25Q<CONFIG>::BulkErase(Event *event)
{
	tlm::tlm_generic_payload *payload = event->GetPayload();
	
	switch(event->GetStage())
	{
		case START_OF_OPERATION:
			{
				if(SR.Write_enable_latch)
				{
					if(!IsDeviceProtected())
					{
						// The write enable latch bit is cleared to 0, whether the operation is successful or not.
						SR.Write_enable_latch = 0;
						Event *end_of_operation_event = PostEndOfWriteOperation(payload);
						write_operation_in_progress_event = end_of_operation_event;
						// When the operation is in progress, the write in progress bit is set to 1. 
						SR.Write_in_progress = 1;
						FSR.Program_or_erase_controller = 0; // busy
					}
					else
					{
						// When a command is applied to a protected subsector, the command is not executed.
						// Instead, the write enable latch bit remains set to 1, and flag status register bits 1 and 5 are set.
						FSR.Protection = 1; // protection error
						FSR.Erase = 1; // protection error
						logger << DebugWarning << "Attempt to erase a locked sector during a bulk erase" << EndDebugWarning;
					}
				}
				else
				{
					// If the write enable latch bit is not set, the device ignores the BULK ERASE command
					// and no error bits are set to indicate operation failure.
					logger << DebugWarning << "Attempt to perform " << TLM_QSPI_BULK_ERASE_COMMAND << " while " << SR.Write_enable_latch.GetName() << " = '0'" << EndDebugWarning;
				}
			}
			break;
		
		case END_OF_OPERATION:
			{
				memset(storage, 0xff, CONFIG::SIZE);
				FSR.Program_or_erase_controller = 1; // ready
			}
			break;
	}
}

template <typename CONFIG>
void N25Q<CONFIG>::ProgramEraseSuspend(Event *suspend_event)
{
	if(suspend_stack.size() > CONFIG::MAX_PROGRAM_SUSPEND_OPERATION_NESTING_LEVEL)
	{
		switch(CONFIG::MAX_PROGRAM_SUSPEND_OPERATION_NESTING_LEVEL)
		{
			case 0:
				logger << DebugWarning << "It is not possible to nest PROGRAM/ERASE SUSPEND operation inside a PROGRAM/ERASE SUSPEND" << EndDebugWarning;
				break;
			case 1:
				logger << DebugWarning << "It is possible to nest PROGRAM/ERASE SUSPEND operation inside a PROGRAM/ERASE SUSPEND operation just once" << EndDebugWarning;
				break;
			default:
				logger << DebugWarning << "It is possible to nest PROGRAM/ERASE SUSPEND operation inside a PROGRAM/ERASE SUSPEND operation just " << CONFIG::MAX_PROGRAM_SUSPEND_OPERATION_NESTING_LEVEL << " times" << EndDebugWarning;
				break;
		}
		return;
	}
	
	tlm::tlm_generic_payload *payload = write_operation_in_progress_event->GetPayload();
	tlm_qspi_extension *qspi_ext = payload->get_extension<tlm_qspi_extension>();
	
	tlm_qspi_command qspi_cmd = qspi_ext->get_command();
	
	switch(qspi_cmd)
	{
		case TLM_QSPI_PAGE_PROGRAM_COMMAND:
		case TLM_QSPI_DUAL_INPUT_FAST_PROGRAM_COMMAND:
		case TLM_QSPI_EXTENDED_DUAL_INPUT_FAST_PROGRAM_COMMAND:
		case TLM_QSPI_QUAD_INPUT_FAST_PROGRAM_COMMAND:
		case TLM_QSPI_EXTENDED_QUAD_INPUT_FAST_PROGRAM_COMMAND:
			// program operation
			{
				sc_core::sc_time time_since_last_program_resume(suspend_event->GetTimeStamp());
				time_since_last_program_resume -= program_resume_time_stamp;
				
				if(time_since_last_program_resume < program_resume_to_suspend)
				{
					logger << DebugWarning << "Program resume to program suspend timing constraint is not met" << EndDebugWarning;
				}
			}
			break;
		
		case TLM_QSPI_SECTOR_ERASE_COMMAND:
		case TLM_QSPI_BULK_ERASE_COMMAND:
			// sector or bulk erase operation
			{
				sc_core::sc_time time_since_last_erase_resume(suspend_event->GetTimeStamp());
				time_since_last_erase_resume -= erase_resume_time_stamp;
				
				if(time_since_last_erase_resume < erase_resume_to_suspend)
				{
					logger << DebugWarning << "Erase resume to erase suspend timing constraint is not met" << EndDebugWarning;
				}
			}
			break;
		case TLM_QSPI_SUBSECTOR_ERASE_COMMAND:
			// subsector erase operation
			{
				sc_core::sc_time time_since_last_subsector_erase_resume(suspend_event->GetTimeStamp());
				time_since_last_subsector_erase_resume -= subsector_erase_resume_time_stamp;
				
				if(time_since_last_subsector_erase_resume < subsector_erase_resume_to_suspend)
				{
					logger << DebugWarning << "Subsector erase resume to erase suspend timing constraint is not met" << EndDebugWarning;
				}
			}
			break;
			
		default:
			// not a program/erase operation
			break;
	}
	
	write_operation_in_progress_event->UpdateElapsedTime(suspend_event->GetTimeStamp());
	
	// compute whether it worths to suspend operation (i.e. suspend latency > time to finish operation)
	bool do_suspend = true;
	
	const sc_core::sc_time& write_operation_duration = write_operation_in_progress_event->GetDuration();
	const sc_core::sc_time& write_operation_elapsed_time = write_operation_in_progress_event->GetElapsedTime();
	sc_core::sc_time time_to_end_of_write_operation(write_operation_duration);
	time_to_end_of_write_operation -= write_operation_elapsed_time;

	switch(qspi_cmd)
	{
		case TLM_QSPI_PAGE_PROGRAM_COMMAND:
		case TLM_QSPI_DUAL_INPUT_FAST_PROGRAM_COMMAND:
		case TLM_QSPI_EXTENDED_DUAL_INPUT_FAST_PROGRAM_COMMAND:
		case TLM_QSPI_QUAD_INPUT_FAST_PROGRAM_COMMAND:
		case TLM_QSPI_EXTENDED_QUAD_INPUT_FAST_PROGRAM_COMMAND:
			if(suspend_latency_program <= time_to_end_of_write_operation)
			{
				do_suspend = false;
			}
			break;
		case TLM_QSPI_SUBSECTOR_ERASE_COMMAND:
		case TLM_QSPI_SECTOR_ERASE_COMMAND:
		case TLM_QSPI_BULK_ERASE_COMMAND:
			if(suspend_latency_erase <= time_to_end_of_write_operation)
			{
				do_suspend = false;
			}
			break;
	}
	
	if(do_suspend)
	{
		write_operation_in_progress_event->Suspend();
		suspend_stack.push(write_operation_in_progress_event);
		write_operation_in_progress_event = 0;
		switch(qspi_cmd)
		{
			case TLM_QSPI_PAGE_PROGRAM_COMMAND:
			case TLM_QSPI_DUAL_INPUT_FAST_PROGRAM_COMMAND:
			case TLM_QSPI_EXTENDED_DUAL_INPUT_FAST_PROGRAM_COMMAND:
			case TLM_QSPI_QUAD_INPUT_FAST_PROGRAM_COMMAND:
			case TLM_QSPI_EXTENDED_QUAD_INPUT_FAST_PROGRAM_COMMAND:
				FSR.Program_suspend = 1;
				break;
			case TLM_QSPI_SUBSECTOR_ERASE_COMMAND:
			case TLM_QSPI_SECTOR_ERASE_COMMAND:
			case TLM_QSPI_BULK_ERASE_COMMAND:
				FSR.Erase_suspend = 1;
				break;
		}
	}
}

template <typename CONFIG>
void N25Q<CONFIG>::ProgramEraseResume(Event *resume_event)
{
	if(suspend_stack.size())
	{
		Event *suspended_operation_event = suspend_stack.top();
		suspend_stack.pop();
	
		if(suspended_operation_event)
		{
			tlm::tlm_generic_payload *payload = suspended_operation_event->GetPayload();
			tlm_qspi_extension *qspi_ext = payload->get_extension<tlm_qspi_extension>();
			
			switch(qspi_ext->get_command())
			{
				case TLM_QSPI_PAGE_PROGRAM_COMMAND:
				case TLM_QSPI_DUAL_INPUT_FAST_PROGRAM_COMMAND:
				case TLM_QSPI_EXTENDED_DUAL_INPUT_FAST_PROGRAM_COMMAND:
				case TLM_QSPI_QUAD_INPUT_FAST_PROGRAM_COMMAND:
				case TLM_QSPI_EXTENDED_QUAD_INPUT_FAST_PROGRAM_COMMAND:
					program_resume_time_stamp = resume_event->GetTimeStamp();
					break;
				
				case TLM_QSPI_SECTOR_ERASE_COMMAND:
				case TLM_QSPI_BULK_ERASE_COMMAND:
					erase_resume_time_stamp = resume_event->GetTimeStamp();
					break;
				case TLM_QSPI_SUBSECTOR_ERASE_COMMAND:
					subsector_erase_resume_time_stamp = resume_event->GetTimeStamp();
					break;
			}
			
			suspended_operation_event->Resume();
			
			const sc_core::sc_time& operation_duration = suspended_operation_event->GetDuration();
			const sc_core::sc_time& operation_elapsed_time = suspended_operation_event->GetElapsedTime();
			sc_core::sc_time time_to_end_of_operation(operation_duration);
			time_to_end_of_operation -= operation_elapsed_time;
			
			sc_core::sc_time end_of_operation_time_stamp(resume_event->GetTimeStamp());
			end_of_operation_time_stamp += time_to_end_of_operation;
			
			schedule.Notify(suspended_operation_event);
			
			switch(qspi_ext->get_command())
			{
				case TLM_QSPI_PAGE_PROGRAM_COMMAND:
				case TLM_QSPI_DUAL_INPUT_FAST_PROGRAM_COMMAND:
				case TLM_QSPI_EXTENDED_DUAL_INPUT_FAST_PROGRAM_COMMAND:
				case TLM_QSPI_QUAD_INPUT_FAST_PROGRAM_COMMAND:
				case TLM_QSPI_EXTENDED_QUAD_INPUT_FAST_PROGRAM_COMMAND:
					write_operation_in_progress_event = suspended_operation_event;
					FSR.Program_suspend = 0;
					break;
				
				case TLM_QSPI_SECTOR_ERASE_COMMAND:
				case TLM_QSPI_BULK_ERASE_COMMAND:
				case TLM_QSPI_SUBSECTOR_ERASE_COMMAND:
					write_operation_in_progress_event = suspended_operation_event;
					FSR.Erase_suspend = 0;
			}
		}
	}
}

template <typename CONFIG>
void N25Q<CONFIG>::Read(tlm::tlm_generic_payload *payload)
{
	sc_dt::uint64 addr = payload->get_address();
	unsigned int data_length = payload->get_data_length();
	unsigned char *data_ptr = payload->get_data_ptr();

	unsigned int data_size_to_copy = data_length;
	if(data_size_to_copy > (CONFIG::SIZE - addr)) data_size_to_copy = CONFIG::SIZE - addr;

	memcpy(data_ptr, storage + addr, data_size_to_copy);
}

template <typename CONFIG>
void N25Q<CONFIG>::Write(tlm::tlm_generic_payload *payload)
{
	sc_dt::uint64 addr = payload->get_address();
	unsigned int data_length = payload->get_data_length();
	unsigned char *data_ptr = payload->get_data_ptr();

	unsigned int data_size_to_copy = data_length;
	if(data_size_to_copy > (CONFIG::SIZE - addr)) data_size_to_copy = CONFIG::SIZE - addr;

	memcpy(storage + addr, data_ptr, data_size_to_copy);
}

template <typename CONFIG>
void N25Q<CONFIG>::ProcessEvents()
{
	const sc_core::sc_time& time_stamp = sc_core::sc_time_stamp();
	Event *event = schedule.GetNextEvent();
	
	if(event)
	{
		do
		{
			if(event->GetTimeStamp() != time_stamp)
			{
				logger << DebugError << "Internal error: unexpected event time stamp (" << event->GetTimeStamp() << " instead of " << time_stamp << ")" << EndDebugError;
				Object::Stop(-1);
			}
			
			if(!event->Suspended())
			{
				ProcessEvent(event);
				schedule.FreeEvent(event);
			}
		}
		while((event = schedule.GetNextEvent()) != 0);
	}
}

template <typename CONFIG>
void N25Q<CONFIG>::ProcessEvent(Event *event)
{
	tlm::tlm_generic_payload *payload = event->GetPayload();
	tlm::tlm_command cmd = payload->get_command();
	
	if(cmd != tlm::TLM_IGNORE_COMMAND)
	{
		tlm_qspi_extension *qspi_ext = payload->get_extension<tlm_qspi_extension>();
		
		if(qspi_ext)
		{
			// custom QSPI protocol
			
			tlm_qspi_command qspi_cmd = qspi_ext->get_command();
			sc_dt::uint64 addr = payload->get_address();
			
			// TODO: check address range
			if(!tlm_qspi_command_has_address(qspi_cmd) || (addr >= CONFIG::SIZE))
			{
				// address is not valid
				logger << DebugWarning << "address for " << qspi_cmd << " is out-of-range" << EndDebugWarning;
				payload->set_response_status(tlm::TLM_ADDRESS_ERROR_RESPONSE);
			}
			else
			{
				// address is valid
				
				switch(qspi_mode)
				{
					case TLM_QSPI_STD_MODE:
						// Standard mode
						
						switch(cmd)
						{
							case tlm::TLM_READ_COMMAND:
								switch(qspi_cmd)
								{
									// IDENTIFICATION Operations
									case TLM_QSPI_READ_ID_COMMAND:
										ReadID(event);
										break;
										
									case TLM_QSPI_MULTIPLE_IO_READ_ID_COMMAND:
										MultipleIOReadID(event);
										break;
										
									case TLM_QSPI_READ_SERIAL_FLASH_DISCOVERY_PARAMETER_COMMAND:
										ReadSFDP(event);
										break;
										
									// READ Operations
									case TLM_QSPI_READ_COMMAND:
										Read(event);
										break;
										
									case TLM_QSPI_FAST_READ_COMMAND:
									case TLM_QSPI_DUAL_OUTPUT_FAST_READ_COMMAND:
									case TLM_QSPI_DUAL_INPUT_OUTPUT_FAST_READ_COMMAND:
									case TLM_QSPI_QUAD_OUTPUT_FAST_READ_COMMAND:
									case TLM_QSPI_QUAD_INPUT_OUTPUT_FAST_READ_COMMAND:
										Read(event);
										
										if(((CONFIG::XIP_F == CONFIG::BASIC_XIP) || !VCR.XIP) && !qspi_ext->get_xip_confirmation_bit()) // XIP=0 and Xb=0
										{
											qspi_mode = TLM_QSPI_XIP_MODE;
											qspi_xip_cmd = qspi_cmd;
										}
										break;

									// REGISTER Operations
									case TLM_QSPI_READ_STATUS_REGISTER_COMMAND:
										ReadStatusRegister(event);
										break;
										
									case TLM_QSPI_READ_LOCK_REGISTER_COMMAND:
										ReadLockRegister(event);
										break;
										
									case TLM_QSPI_READ_FLAG_STATUS_REGISTER_COMMAND:
										ReadFlagStatusRegister(event);
										break;
										
									case TLM_QSPI_READ_NONVOLATILE_CONFIGURATION_REGISTER_COMMAND:
										ReadNonVolatileConfigurationRegister(event);
										break;
										
									case TLM_QSPI_READ_VOLATILE_CONFIGURATION_REGISTER_COMMAND:
										ReadVolatileConfigurationRegister(event);
										break;
										
									case TLM_QSPI_WRITE_VOLATILE_CONFIGURATION_REGISTER_COMMAND:
										WriteVolatileConfigurationRegister(event);
										break;
										
									case TLM_QSPI_READ_ENHANCED_VOLATILE_CONFIGURATION_REGISTER_COMMAND:
										ReadEnhancedVolatileConfigurationRegister(event);
										break;
										
									case TLM_QSPI_WRITE_ENHANCED_VOLATILE_CONFIGURATION_REGISTER_COMMAND:
										WriteEnhancedVolatileConfigurationRegister(event);
										break;
										
									// ONE-TIME PROGRAMMABLE (OTP) Operations
									case TLM_QSPI_READ_OTP_ARRAY_COMMAND:
										ReadOTPArray(event);
										break;
										
									default:
										payload->set_response_status(tlm::TLM_COMMAND_ERROR_RESPONSE);
										// bad combination
										break;
								}
								break;
							case tlm::TLM_WRITE_COMMAND:
								switch(qspi_cmd)
								{
									// WRITE Operations
									case TLM_QSPI_WRITE_ENABLE_COMMAND:
										WriteEnable();
										break;
										
									case TLM_QSPI_WRITE_DISABLE_COMMAND:
										WriteDisable();
										break;
										
									case TLM_QSPI_WRITE_STATUS_REGISTER_COMMAND:
										WriteStatusRegister(event);
										break;
										
									case TLM_QSPI_WRITE_LOCK_REGISTER_COMMAND:
										WriteLockRegister(event);
										break;
										
									case TLM_QSPI_CLEAR_FLAG_STATUS_REGISTER_COMMAND:
										ClearFlagStatusRegister();
										break;
										
									case TLM_QSPI_WRITE_NONVOLATILE_CONFIGURATION_REGISTER_COMMAND:
										WriteNonVolatileConfigurationRegister(event);
										break;
										
									case TLM_QSPI_WRITE_VOLATILE_CONFIGURATION_REGISTER_COMMAND:
										WriteVolatileConfigurationRegister(event);
										break;
										
									case TLM_QSPI_WRITE_ENHANCED_VOLATILE_CONFIGURATION_REGISTER_COMMAND:
										WriteEnhancedVolatileConfigurationRegister(event);
										break;
										
									// PROGRAM Operations
									case TLM_QSPI_PAGE_PROGRAM_COMMAND:
									case TLM_QSPI_DUAL_INPUT_FAST_PROGRAM_COMMAND:
									case TLM_QSPI_EXTENDED_DUAL_INPUT_FAST_PROGRAM_COMMAND:
									case TLM_QSPI_QUAD_INPUT_FAST_PROGRAM_COMMAND:
									case TLM_QSPI_EXTENDED_QUAD_INPUT_FAST_PROGRAM_COMMAND:
										Program(event);
										break;
										
									// ERASE Operations
									case TLM_QSPI_SUBSECTOR_ERASE_COMMAND:
										SubSectorErase(event);
										break;
									case TLM_QSPI_SECTOR_ERASE_COMMAND:
										SectorErase(event);
										break;
									case TLM_QSPI_BULK_ERASE_COMMAND:
										BulkErase(event);
										break;
									case TLM_QSPI_PROGRAM_ERASE_RESUME_COMMAND:
										ProgramEraseResume(event);
										break;
									case TLM_QSPI_PROGRAM_ERASE_SUSPEND_COMMAND:
										ProgramEraseSuspend(event);
										break;
										
									// ONE-TIME PROGRAMMABLE (OTP) Operations
									case TLM_QSPI_PROGRAM_OTP_ARRAY_COMMAND:
										ProgramOTPArray(event);
										break;
										
									default:
										// bad combination
										payload->set_response_status(tlm::TLM_COMMAND_ERROR_RESPONSE);
										break;
								}
								break;
								
							case tlm::TLM_IGNORE_COMMAND:
								break;
						}
						break;
						
					case TLM_QSPI_XIP_MODE:
						// XIP mode
						
						switch(cmd)
						{
							case tlm::TLM_READ_COMMAND:
								// Note: QSPI command in the payload extension is ignored when in XIP mode
								
								Read(event);
								
								if(qspi_ext->get_xip_confirmation_bit()) // XIP confirmation bit=1
								{
									// XIP is terminated by driving the XIP confirmation bit to 1.
									qspi_mode = TLM_QSPI_STD_MODE;
									
									// The device automatically resets volatile configuration register bit 3 to 1.
									VCR.XIP = 1;
								}
								break;
								
							case tlm::TLM_WRITE_COMMAND:
								// write not allowed in XIP mode
								payload->set_response_status(tlm::TLM_COMMAND_ERROR_RESPONSE);
								break;
								
							case tlm::TLM_IGNORE_COMMAND:
								break;
						}
				}
			}
		}
		else
		{
			// TLM-2.0 base protocol
			switch(cmd)
			{
				case tlm::TLM_READ_COMMAND:
					Read(payload);
					break;
				case tlm::TLM_WRITE_COMMAND:
					Write(payload);
					break;
				case tlm::TLM_IGNORE_COMMAND:
					break;
			}
		}
	}

	payload->set_dmi_allowed(true);
	
	sc_core::sc_time completion_time = ComputeResponseTime(payload);
	sc_core::sc_event *completion_event = event->GetCompletionEvent();
	
	if(completion_event)
	{
		completion_event->notify(completion_time);
	}
	else
	{
		tlm::tlm_phase phase = tlm::BEGIN_RESP;
		
		tlm::tlm_sync_enum sync = qspi_slave_socket->nb_transport_bw(*payload, phase, completion_time);
		
		switch(sync)
		{
			case tlm::TLM_UPDATED:
				break;
			case tlm::TLM_COMPLETED:
				break;
		}
	}
}

template <typename CONFIG>
void N25Q<CONFIG>::Process()
{
	if(threaded_model)
	{
		while(1)
		{
			wait(schedule.GetKernelEvent());
			ProcessEvents();
		}
	}
	else
	{
		ProcessEvents();
		next_trigger(schedule.GetKernelEvent());
	}
}

template <typename CONFIG>
void N25Q<CONFIG>::RESET_N_Process()
{
	if((GetProtocol() != TLM_QSPI_QUAD_IO_PROTOCOL) && ICR.Reset_hold) // reset enabled
	{
		Reset();
	}
}

template <typename CONFIG>
void N25Q<CONFIG>::HOLD_N_Process()
{
	if((GetProtocol() != TLM_QSPI_QUAD_IO_PROTOCOL) && ICR.Reset_hold) // hold enabled
	{
		if(HOLD_RESET_N)
			schedule.Unpause();
		else
			schedule.Pause();
	}
}


} // end of namespace n25q
} // end of namespace flash
} // end of namespace memory
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_MEMORY_FLASH_N25Q_N25Q_TCC__
