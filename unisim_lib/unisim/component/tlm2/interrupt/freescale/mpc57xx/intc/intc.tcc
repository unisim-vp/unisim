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

#ifndef __UNISIM_COMPONENT_TLM2_INTERRUPT_FREESCALE_MPC57XX_INTC_INTC_TCC__
#define __UNISIM_COMPONENT_TLM2_INTERRUPT_FREESCALE_MPC57XX_INTC_INTC_TCC__

#include <unisim/component/tlm2/interrupt/freescale/mpc57xx/intc/intc.hh>
#include <unisim/util/reg/core/register.tcc>

namespace unisim {
namespace component {
namespace tlm2 {
namespace interrupt {
namespace freescale {
namespace mpc57xx {
namespace intc {

template <typename CONFIG>
INTC<CONFIG>::INTC(const sc_core::sc_module_name& name, unisim::kernel::service::Object *parent)
	: unisim::kernel::service::Object(name, parent)
	, sc_core::sc_module(name)
	, ahb_if()
	, p_hw_irq()
	, p_iack()
	, p_irq_b()
	, p_avec_b()
	, p_voffset()
	, logger(*this)
	, intc_bcr(this)
	, intc_mprot(this)
	, intc_cpr(this)
	, intc_iack(this)
	, intc_eoir(this)
	, intc_sscir(this)
	, intc_psr_sw(this)
	, intc_psr_hw(this)
	, lifo()
	, irqs()
	, priority_tree()
	, reg_addr_map(logger.DebugWarningStream())
	, schedule()
	, last_irq_b_time_stamp()
	, irq_select_event()
	, gen_irq_b_event()
	, selected_irq_num()
	, irq_b()
	, endian(unisim::util::endian::E_BIG_ENDIAN)
	, param_endian("endian", this, endian, "endian")
	, verbose(false)
	, param_verbose("verbose", this, verbose, "enable/disable verbosity")
	, cycle_time(10.0, sc_core::SC_NS)
	, param_cycle_time("cycle-time", this, cycle_time, "cycle time")
{
	SC_HAS_PROCESS(INTC);

	unsigned int sw_irq_num;
	unsigned int hw_irq_num;
	unsigned int irq_num;
	unsigned int prc_num;
	
	for(prc_num = 0; prc_num < NUM_PROCESSORS; prc_num++)
	{
		irq_b[prc_num] = true;
	}

	for(prc_num = 0; prc_num < NUM_PROCESSORS; prc_num++)
	{
		std::stringstream ahb_if_name_sstr;
		ahb_if_name_sstr << "ahb_if_" << prc_num;
		
		ahb_if[prc_num] = new ahb_slave_if_type(ahb_if_name_sstr.str().c_str());
		
		ahb_if[prc_num]->register_b_transport(this, &INTC<CONFIG>::b_transport, prc_num);
		ahb_if[prc_num]->register_get_direct_mem_ptr(this, &INTC<CONFIG>::get_direct_mem_ptr, prc_num);
		ahb_if[prc_num]->register_transport_dbg(this, &INTC<CONFIG>::transport_dbg, prc_num);
		ahb_if[prc_num]->register_nb_transport_fw(this, &INTC<CONFIG>::nb_transport_fw, prc_num);
	}
	
	for(hw_irq_num = 0; hw_irq_num < NUM_HW_IRQS; hw_irq_num++)
	{
		std::stringstream p_hw_irq_name_sstr;
		p_hw_irq_name_sstr << "p_hw_irq_" << hw_irq_num;
		p_hw_irq[hw_irq_num] = new sc_core::sc_in<bool>(p_hw_irq_name_sstr.str().c_str()); 
	}
	
	for(prc_num = 0; prc_num < NUM_PROCESSORS; prc_num++)
	{
		std::stringstream p_iack_name_sstr;
		p_iack_name_sstr << "p_iack_" << prc_num;
		p_iack[prc_num] = new sc_core::sc_in<bool>(p_iack_name_sstr.str().c_str());
	}
	
	for(prc_num = 0; prc_num < NUM_PROCESSORS; prc_num++)
	{
		std::stringstream p_irq_b_name_sstr;
		p_irq_b_name_sstr << "p_irq_b_" << prc_num;
		p_irq_b[prc_num] = new sc_core::sc_out<bool>(p_irq_b_name_sstr.str().c_str());
	}

	for(prc_num = 0; prc_num < NUM_PROCESSORS; prc_num++)
	{
		std::stringstream p_avec_b_name_sstr;
		p_avec_b_name_sstr << "p_avec_b_" << prc_num;
		p_avec_b[prc_num] = new sc_core::sc_out<bool>(p_avec_b_name_sstr.str().c_str());
	}

	for(prc_num = 0; prc_num < NUM_PROCESSORS; prc_num++)
	{
		std::stringstream p_voffset_name_sstr;
		p_voffset_name_sstr << "p_voffset_" << prc_num;
		p_voffset[prc_num] = new sc_core::sc_out<sc_dt::sc_uint<14> >(p_voffset_name_sstr.str().c_str());
	}
	
	for(prc_num = 0; prc_num < NUM_PROCESSORS; prc_num++)
	{
		std::stringstream irq_select_event_name_sstr;
		irq_select_event_name_sstr << "irq_select_event_" << prc_num;
		irq_select_event[prc_num] = new sc_core::sc_event(irq_select_event_name_sstr.str().c_str());
	}

	for(prc_num = 0; prc_num < NUM_PROCESSORS; prc_num++)
	{
		std::stringstream gen_irq_b_event_name_sstr;
		gen_irq_b_event_name_sstr << "gen_irq_b_event_" << prc_num;
		gen_irq_b_event[prc_num] = new sc_core::sc_event(gen_irq_b_event_name_sstr.str().c_str());
	}

	reg_addr_map.MapRegister(intc_bcr.ADDRESS_OFFSET, &intc_bcr);
	reg_addr_map.MapRegister(intc_mprot.ADDRESS_OFFSET, &intc_mprot);
	reg_addr_map.MapRegisterFile(INTC_CPR::ADDRESS_OFFSET, &intc_cpr);
	reg_addr_map.MapRegisterFile(INTC_IACK::ADDRESS_OFFSET, &intc_iack);
	reg_addr_map.MapRegisterFile(INTC_EOIR::ADDRESS_OFFSET, &intc_eoir);
	reg_addr_map.MapRegisterFile(INTC_SSCIR::ADDRESS_OFFSET, &intc_sscir);
	reg_addr_map.MapRegisterFile(INTC_PSR<SW_IRQ>::ADDRESS_OFFSET, &intc_psr_sw);
	reg_addr_map.MapRegisterFile(INTC_PSR<HW_IRQ>::ADDRESS_OFFSET, &intc_psr_hw);

	if(threaded_model)
	{
		SC_THREAD(Process);
	}
	else
	{
		SC_METHOD(Process);
	}
	
	// Spawn an HW_IRQ_Process for each HW IRQ
	for(hw_irq_num = 0; hw_irq_num < NUM_HW_IRQS; hw_irq_num++)
	{
		sc_core::sc_spawn_options hw_irq_process_spawn_options;
		hw_irq_process_spawn_options.spawn_method();
		//hw_irq_process_spawn_options.dont_initialize();
		hw_irq_process_spawn_options.set_sensitivity(p_hw_irq[hw_irq_num]);
		
		std::stringstream hw_irq_process_name_sstr;
		hw_irq_process_name_sstr << "HW_IRQ_Process_" << hw_irq_num;
		sc_core::sc_spawn(sc_bind(&INTC<CONFIG>::HW_IRQ_Process, this, hw_irq_num), hw_irq_process_name_sstr.str().c_str(), &hw_irq_process_spawn_options);
	}
	
	// Spawn an IACK_Process for each processor
	for(prc_num = 0; prc_num < NUM_PROCESSORS; prc_num++)
	{
		sc_core::sc_spawn_options iack_process_spawn_options;
		iack_process_spawn_options.spawn_method();
		//iack_process_spawn_options.dont_initialize();
		iack_process_spawn_options.set_sensitivity(&p_iack[prc_num]->neg());
		
		std::stringstream iack_process_name_sstr;
		iack_process_name_sstr << "IACK_Process_" << prc_num;
		sc_core::sc_spawn(sc_bind(&INTC<CONFIG>::IACK_Process, this, prc_num), iack_process_name_sstr.str().c_str(), &iack_process_spawn_options);
	}

	// Spawn an IRQ_Select_Process for each processor
	for(prc_num = 0; prc_num < NUM_PROCESSORS; prc_num++)
	{
		sc_core::sc_spawn_options irq_select_process_spawn_options;
		irq_select_process_spawn_options.spawn_method();
		//irq_select_process_spawn_options.dont_initialize();
		irq_select_process_spawn_options.set_sensitivity(irq_select_event[prc_num]);
		
		std::stringstream irq_select_process_name_sstr;
		irq_select_process_name_sstr << "IRQ_Select_Process_" << prc_num;
		sc_core::sc_spawn(sc_bind(&INTC<CONFIG>::IRQ_Select_Process, this, prc_num), irq_select_process_name_sstr.str().c_str(), &irq_select_process_spawn_options);
	}

	// Spawn an IRQ_B_Process for each processor
	for(prc_num = 0; prc_num < NUM_PROCESSORS; prc_num++)
	{
		sc_core::sc_spawn_options irq_b_process_spawn_options;
		irq_b_process_spawn_options.spawn_method();
		//irq_b_process_spawn_options.dont_initialize();
		irq_b_process_spawn_options.set_sensitivity(gen_irq_b_event[prc_num]);
		
		std::stringstream irq_b_process_name_sstr;
		irq_b_process_name_sstr << "IRQ_B_Process_" << prc_num;
		sc_core::sc_spawn(sc_bind(&INTC<CONFIG>::IRQ_B_Process, this, prc_num), irq_b_process_name_sstr.str().c_str(), &irq_b_process_spawn_options);
	}

	// 	intc_bcr.LongPrettyPrint(std::cerr); std::cerr << std::endl;
// 	intc_psr_sw[0].LongPrettyPrint(std::cerr); std::cerr << std::endl;
// 	intc_psr_hw[0].LongPrettyPrint(std::cerr); std::cerr << std::endl;
}

template <typename CONFIG>
INTC<CONFIG>::~INTC()
{
	unsigned int hw_irq_num;
	unsigned int prc_num;
	
	for(prc_num = 0; prc_num < NUM_PROCESSORS; prc_num++)
	{
		delete ahb_if[prc_num];
	}
	for(hw_irq_num = 0; hw_irq_num < NUM_HW_IRQS; hw_irq_num++)
	{
		delete p_hw_irq[hw_irq_num];
	}
	for(prc_num = 0; prc_num < NUM_PROCESSORS; prc_num++)
	{
		delete p_iack[prc_num];
	}
	for(prc_num = 0; prc_num < NUM_PROCESSORS; prc_num++)
	{
		delete p_irq_b[prc_num];
	}
	for(prc_num = 0; prc_num < NUM_PROCESSORS; prc_num++)
	{
		delete p_avec_b[prc_num];
	}
	for(prc_num = 0; prc_num < NUM_PROCESSORS; prc_num++)
	{
		delete p_voffset[prc_num];
	}
	for(prc_num = 0; prc_num < NUM_PROCESSORS; prc_num++)
	{
		delete irq_select_event[prc_num];
	}
	for(prc_num = 0; prc_num < NUM_PROCESSORS; prc_num++)
	{
		delete gen_irq_b_event[prc_num];
	}
}

template <typename CONFIG>
bool INTC<CONFIG>::CheckMasterProtection(unsigned int prc_num) const
{
	if(prc_num == 0) return true; // The processor with master ID zero will be allowed write access
	if(!intc_mprot.template Get<typename INTC_MPROT::MPROT>()) return true; // all processors have write access to this register
	
	return intc_mprot.template Get<typename INTC_MPROT::ID>() == prc_num; // the processor with master ID (0-3) = INTC_MPROT[ID] has write access, otherwise, a termination error is assert
}

template <typename CONFIG>
bool INTC<CONFIG>::CheckWriteProtection_SSCIR_CLR(unsigned int prc_num, unsigned int sw_irq_num) const
{
	unsigned int seln = intc_psr_sw[sw_irq_num].template Get<typename INTC_PSR<SW_IRQ>::PRC_SELN>();
	
	if((seln << prc_num) & 8)
	{
		// processor is selected for that software IRQ
		return true;
	}
	
	return false;
}

template <typename CONFIG>
bool INTC<CONFIG>::IsHardwareVectorEnabled(unsigned int prc_num) const
{
	switch(prc_num)
	{
		case 0: return intc_bcr.template Get<typename INTC_BCR::HVEN0>() != 0;
		case 1: return intc_bcr.template Get<typename INTC_BCR::HVEN1>() != 0;
		case 2: return intc_bcr.template Get<typename INTC_BCR::HVEN2>() != 0;
		case 3: return intc_bcr.template Get<typename INTC_BCR::HVEN3>() != 0;
	}
	return false;
}

template <typename CONFIG>
void INTC<CONFIG>::b_transport(int prc_num, tlm::tlm_generic_payload& payload, sc_core::sc_time& t)
{
	sc_core::sc_event completion_event;
	sc_core::sc_time notify_time_stamp(sc_core::sc_time_stamp());
	notify_time_stamp += t;
	Event *event = schedule.AllocEvent();
	event->SetProcessorNumber(prc_num);
	event->SetPayload(&payload);
	event->SetTimeStamp(notify_time_stamp);
	event->SetCompletionEvent(&completion_event);
	schedule.Notify(event);
	sc_core::wait(completion_event);
	t = sc_core::SC_ZERO_TIME;
}

template <typename CONFIG>
bool INTC<CONFIG>::get_direct_mem_ptr(int prc_num, tlm::tlm_generic_payload& payload, tlm::tlm_dmi& dmi_data)
{
	dmi_data.set_granted_access(tlm::tlm_dmi::DMI_ACCESS_READ_WRITE);
	dmi_data.set_start_address(0);
	dmi_data.set_end_address(sc_dt::uint64(-1));
	return false;
}

template <typename CONFIG>
unsigned int INTC<CONFIG>::transport_dbg(int _prc_num, tlm::tlm_generic_payload& payload)
{
	tlm::tlm_command cmd = payload.get_command();
	unsigned int data_length = payload.get_data_length();
	unsigned char *data_ptr = payload.get_data_ptr();
	sc_dt::uint64 start_addr = payload.get_address();
	
	if(!data_ptr)
	{
		logger << DebugError << "data pointer for TLM-2.0 GP READ/WRITE command is invalid" << EndDebugError;
		unisim::kernel::service::Object::Stop(-1);
		return 0;
	}
	else if(!data_length)
	{
		return 0;
	}
	
	ProcessorNumber prc_num(_prc_num);
	
	switch(cmd)
	{
		case tlm::TLM_WRITE_COMMAND:
			return reg_addr_map.DebugWrite(start_addr, data_ptr, data_length, endian, &prc_num);
		case tlm::TLM_READ_COMMAND:
			return reg_addr_map.DebugRead(start_addr, data_ptr, data_length, endian, &prc_num);
		default:
			break;
	}
	
	return 0;
}

template <typename CONFIG>
tlm::tlm_sync_enum INTC<CONFIG>::nb_transport_fw(int prc_num, tlm::tlm_generic_payload& payload, tlm::tlm_phase& phase, sc_core::sc_time& t)
{
	switch(phase)
	{
		case tlm::BEGIN_REQ:
			{
				sc_core::sc_time notify_time_stamp(sc_core::sc_time_stamp());
				notify_time_stamp += t;
				Event *event = schedule.AllocEvent();
				event->SetProcessorNumber(prc_num);
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
			unisim::kernel::service::Object::Stop(-1);
			break;
	}
	
	return tlm::TLM_COMPLETED;
}

template <typename CONFIG>
void INTC<CONFIG>::ProcessEvent(Event *event)
{
	tlm::tlm_generic_payload *payload = event->GetPayload();
	tlm::tlm_command cmd = payload->get_command();

	if(cmd != tlm::TLM_IGNORE_COMMAND)
	{
		unsigned int streaming_width = payload->get_streaming_width();
		unsigned int data_length = payload->get_data_length();
		unsigned char *data_ptr = payload->get_data_ptr();
		unsigned char *byte_enable_ptr = payload->get_byte_enable_ptr();
		sc_dt::uint64 start_addr = payload->get_address();
		sc_dt::uint64 end_addr = start_addr + ((streaming_width > data_length) ? data_length : streaming_width) - 1;
		
		if(!data_ptr)
		{
			logger << DebugError << "data pointer for TLM-2.0 GP READ/WRITE command is invalid" << EndDebugError;
			unisim::kernel::service::Object::Stop(-1);
			return;
		}
		else if(!data_length)
		{
			logger << DebugError << "data length range for TLM-2.0 GP READ/WRITE command is invalid" << EndDebugError;
			unisim::kernel::service::Object::Stop(-1);
			return;
		}
		else if(byte_enable_ptr)
		{
			// byte enable is unsupported
			logger << DebugWarning << "byte enable for TLM-2.0 GP READ/WRITE command is unsupported" << EndDebugWarning;
			payload->set_response_status(tlm::TLM_BYTE_ENABLE_ERROR_RESPONSE);
		}
		else if(streaming_width < data_length)
		{
			// streaming is unsupported
			logger << DebugWarning << "streaming for TLM-2.0 GP READ/WRITE command is unsupported" << EndDebugWarning;
			payload->set_response_status(tlm::TLM_BURST_ERROR_RESPONSE);
		}
		else if((start_addr & -4) != (end_addr & -4))
		{
			logger << DebugWarning << "access crosses 32-bit boundary" << EndDebugWarning;
			payload->set_response_status(tlm::TLM_BURST_ERROR_RESPONSE);
		}
		else
		{
			ProcessorNumber prc_num(event->GetProcessorNumber());
			ReadWriteStatus rws = RWS_OK;
			
			switch(cmd)
			{
				case tlm::TLM_WRITE_COMMAND:
					rws = reg_addr_map.Write(start_addr, data_ptr, data_length, endian, &prc_num);
					break;
				case tlm::TLM_READ_COMMAND:
					rws = reg_addr_map.Read(start_addr, data_ptr, data_length, endian, &prc_num);
					break;
				default:
					break;
			}
			
			if(IsReadWriteError(rws))
			{
				logger << DebugError << "while mapped read/write access, " << rws << std::endl;
				payload->set_response_status(tlm::TLM_ADDRESS_ERROR_RESPONSE);
			}
			else
			{
				payload->set_response_status(tlm::TLM_OK_RESPONSE);
			}
		}
	}

	payload->set_dmi_allowed(false);
	
	sc_core::sc_time completion_time(sc_core::SC_ZERO_TIME);
	sc_core::sc_event *completion_event = event->GetCompletionEvent();
	
	if(completion_event)
	{
		completion_event->notify(completion_time);
	}
	else
	{
		tlm::tlm_phase phase = tlm::BEGIN_RESP;
		
		unsigned int prc_num = event->GetProcessorNumber();
		tlm::tlm_sync_enum sync = (*ahb_if[prc_num])->nb_transport_bw(*payload, phase, completion_time);
		
		switch(sync)
		{
			case tlm::TLM_ACCEPTED:
				break;
			case tlm::TLM_UPDATED:
				break;
			case tlm::TLM_COMPLETED:
				break;
		}
	}
}

template <typename CONFIG>
void INTC<CONFIG>::ProcessEvents()
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
				unisim::kernel::service::Object::Stop(-1);
			}
			
			ProcessEvent(event);
			schedule.FreeEvent(event);
		}
		while((event = schedule.GetNextEvent()) != 0);
	}
}

template <typename CONFIG>
void INTC<CONFIG>::Process()
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
void INTC<CONFIG>::UpdateIRQSelect(unsigned int prc_num)
{
	irq_select_event[prc_num]->notify(sc_core::SC_ZERO_TIME);
}

template <typename CONFIG>
void INTC<CONFIG>::SetIRQOutput(unsigned int prc_num, bool value)
{
	irq_b[prc_num] = !value;
	
	sc_core::sc_time notify_delay(sc_core::sc_time_stamp());
	unisim::kernel::tlm2::AlignToClock(notify_delay, cycle_time);
	notify_delay -= sc_core::sc_time_stamp();
	gen_irq_b_event[prc_num]->notify(notify_delay);
}

template <typename CONFIG>
unsigned int INTC<CONFIG>::SW_IRQ2IRQ(unsigned int sw_irq_num) const
{
	return sw_irq_num;
}

template <typename CONFIG>
unsigned int INTC<CONFIG>::HW_IRQ2IRQ(unsigned int hw_irq_num) const
{
	return NUM_SW_IRQS + hw_irq_num;
}

template <typename CONFIG>
unsigned int INTC<CONFIG>::IRQ2SW_IRQ(unsigned int irq_num) const
{
	return irq_num;
}

template <typename CONFIG>
unsigned int INTC<CONFIG>::IRQ2HW_IRQ(unsigned int irq_num) const
{
	return irq_num - NUM_SW_IRQS;
}

template <typename CONFIG>
bool INTC<CONFIG>::IsSW_IRQ(unsigned int irq_num) const
{
	return irq_num < NUM_SW_IRQS;
}

template <typename CONFIG>
bool INTC<CONFIG>::IsHW_IRQ(unsigned int irq_num) const
{
	return irq_num >= NUM_SW_IRQS;
}

template <typename CONFIG>
void INTC<CONFIG>::HW_IRQ_Process(unsigned int hw_irq_num)
{
	unsigned int irq_num = HW_IRQ2IRQ(hw_irq_num);
	SetIRQInputStatus(irq_num, p_hw_irq[hw_irq_num]);
}

template <typename CONFIG>
void INTC<CONFIG>::IACK_Process(unsigned int prc_num)
{
	if(p_iack[prc_num]->negedge())
	{
		IRQAcknowledge(prc_num);
	}
}

template <typename CONFIG>
void INTC<CONFIG>::IRQ_Select_Process(unsigned int prc_num)
{
	if(SelectIRQInput(prc_num, selected_irq_num[prc_num]))
	{
		SetIRQOutput(prc_num, true);
	}
}

template <typename CONFIG>
void INTC<CONFIG>::IRQ_B_Process(unsigned int prc_num)
{
	if(!irq_b[prc_num])
	{
		// IRQ must be asserted
		bool cur_irq_b_value = *p_irq_b[prc_num];
	
		if(cur_irq_b_value)
		{
			// IRQ is currently negated, but IRQ shall negate for at least one clock cycle
			sc_core::sc_time time_since_last_irq_b(sc_core::sc_time_stamp());
			time_since_last_irq_b -= last_irq_b_time_stamp[prc_num];
			if(time_since_last_irq_b >= cycle_time)
			{
				// Assert IRQ
				*p_irq_b[prc_num] = !true;
				
				// When the interrupt request to the processor asserts, the INTVEC is updated,
				// whether the INTC is in software or hardware vector mode.
				intc_iack[prc_num].template Set<typename INTC_IACK::INTVEC>(selected_irq_num[prc_num]);

				if(IsHardwareVectorEnabled(prc_num))
				{
					// Tell CPU that we're operating in hardware vector mode (aka. autovector mode)
					*p_avec_b[prc_num] = !true;
					
					// The vector value matches the value of the INTVEC field in the INTC_IACKRn,
					// depending on which processor was assigned to handle a given interrupt source.
					*p_voffset[prc_num] = selected_irq_num[prc_num];
				}
				else
				{
					// Tell CPU that we're operating in software vector mode (aka. non-autovector mode)
					*p_avec_b[prc_num] = !false;
				}
			}
		}
	}
	else
	{
		// negate IRQ
		*p_irq_b[prc_num] = !false;
		*p_avec_b[prc_num] = !false;
		
		// keep time stamp when IRQ was negated, because IRQ shall negate for at least one clock cycle
		last_irq_b_time_stamp[prc_num] = sc_core::sc_time_stamp();
	}
}

template <typename CONFIG>
void INTC<CONFIG>::SetIRQInputStatus(unsigned int irq_num, bool value)
{
	uint64_t& s = irqs[irq_num / 64];
	uint64_t m = 1 << (irq_num % 64);
	
	if(value)
	{
		s = s | m;
	}
	else
	{
		s = s & ~m;
	}
	
	unsigned int prc_num;
	for(prc_num = 0; prc_num < NUM_PROCESSORS; prc_num++)
	{
		UpdateIRQSelect(prc_num);
	}
}

template <typename CONFIG>
bool INTC<CONFIG>::GetIRQInputStatus(unsigned int irq_num) const
{
	uint64_t s = irqs[irq_num / 64];
	uint64_t m = 1 << (irq_num % 64);
	
	return (s & m) != 0;
}

template <typename CONFIG>
void INTC<CONFIG>::EnableIRQInput(unsigned int prc_num, unsigned int irq_num, unsigned int new_priority)
{
	IRQPriority new_irq_priority(irq_num, new_priority);

	priority_tree[prc_num].insert(new_irq_priority);
	
	UpdateIRQSelect(prc_num);
}

template <typename CONFIG>
void INTC<CONFIG>::DisableIRQInput(unsigned int prc_num, unsigned int irq_num, unsigned int old_priority)
{
	IRQPriority old_irq_priority(irq_num, old_priority);
	typename std::set<IRQPriority>::iterator it = priority_tree[prc_num].find(old_irq_priority);
	
	if(it != priority_tree[prc_num].end())
	{
		priority_tree[prc_num].erase(it);
	}
	
	UpdateIRQSelect(prc_num);
}

template <typename CONFIG>
void INTC<CONFIG>::ChangeIRQInputPriority(unsigned int prc_num, unsigned int irq_num, unsigned int old_priority, unsigned int new_priority)
{
	IRQPriority old_irq_priority(irq_num, old_priority);
	typename std::set<IRQPriority>::iterator it = priority_tree[prc_num].find(old_irq_priority);
	
	if(it != priority_tree[prc_num].end())
	{
		priority_tree[prc_num].erase(it);
	}
	
	IRQPriority new_irq_priority(irq_num, new_priority);

	priority_tree[prc_num].insert(new_irq_priority);
	
	UpdateIRQSelect(prc_num);
}

template <typename CONFIG>
bool INTC<CONFIG>::SelectIRQInput(unsigned int prc_num, unsigned int& selected_irq_num) const
{
	unsigned int current_priority = intc_cpr[prc_num].template Get<typename INTC_CPR::PRI>();
	
	typename std::set<IRQPriority>::const_iterator it;
	
	for(it = priority_tree[prc_num].begin(); it != priority_tree[prc_num].end(); it++)
	{
		const IRQPriority& irq_priority = *it;
		unsigned int priority = irq_priority.GetPriority();
		
		if(priority > current_priority)
		{
			unsigned int irq_num = irq_priority.GetIRQ();
			
			if(GetIRQInputStatus(irq_num))
			{
				selected_irq_num = irq_num;
				return true;
			}
		}
	}
	
	return false;
}

template <typename CONFIG>
void INTC<CONFIG>::IRQAcknowledge(unsigned int prc_num)
{
	SetIRQOutput(prc_num, false);

	unsigned int current_priority = intc_cpr[prc_num].template Get<typename INTC_CPR::PRI>();

	lifo[prc_num].push(current_priority);
	
	unsigned int irq_num = selected_irq_num[prc_num];
	
	unsigned int priority = IsSW_IRQ(irq_num) ? intc_psr_sw[IRQ2SW_IRQ(irq_num)].template Get<typename INTC_PSR<SW_IRQ>::PRIN>()
	                                          : intc_psr_hw[IRQ2HW_IRQ(irq_num)].template Get<typename INTC_PSR<HW_IRQ>::PRIN>();
	
	intc_cpr[prc_num].template Set<typename INTC_CPR::PRI>(priority);
	
	if(IsHW_IRQ(irq_num))
	{
		intc_psr_hw[IRQ2HW_IRQ(irq_num)].template Set<typename INTC_PSR<HW_IRQ>::SWTN>(0);
	}
	
	SetIRQInputStatus(irq_num, false);
}

template <typename CONFIG>
void INTC<CONFIG>::EndOfInterrupt(unsigned int prc_num)
{
	unsigned int priority = lifo[prc_num].empty() ? 0 : lifo[prc_num].top();
	
	intc_cpr[prc_num].template Set<typename INTC_CPR::PRI>(priority);

	lifo[prc_num].pop();
	
	UpdateIRQSelect(prc_num);
}

} // end of namespace intc
} // end of namespace mpc57xx
} // end of namespace freescale
} // end of namespace interrupt
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_INTERRUPT_FREESCALE_MPC57XX_INTC_INTC_TCC__
