/*
 *  Copyright (c) 2007,
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

#ifndef __UNISIM_COMPONENT_TLM_PCI_DEBUG_PCI_STUB_TCC__
#define __UNISIM_COMPONENT_TLM_PCI_DEBUG_PCI_STUB_TCC__

namespace unisim {
namespace component {
namespace tlm {
namespace pci {
namespace debug {

template <class ADDRESS_TYPE, uint32_t MAX_DATA_SIZE>
PCIStub<ADDRESS_TYPE, MAX_DATA_SIZE>::PCIStub(const sc_module_name& name, Object *parent) :
	Object(name, parent),
	unisim::component::cxx::pci::debug::PCIStub<ADDRESS_TYPE>(name, parent),
	sc_module(name),
	bus_port("bus-port"),
	cpu_irq_port("cpu-irq-port"),
	pci_bus_cycle_time(),
	bus_cycle_time(),
	has_intr(false)
{
	SC_HAS_PROCESS(PCIStub);
	
	bus_port(*this);
	
	SC_THREAD(TriggerInterrupt);
	SC_THREAD(Process);
}

template <class ADDRESS_TYPE, uint32_t MAX_DATA_SIZE>
PCIStub<ADDRESS_TYPE, MAX_DATA_SIZE>::~PCIStub()
{
}

template <class ADDRESS_TYPE, uint32_t MAX_DATA_SIZE>
bool PCIStub<ADDRESS_TYPE, MAX_DATA_SIZE>::Send(const Pointer<TlmMessage<PCIReq, PCIRsp> > &message)
{
	const Pointer<PCIRequest<ADDRESS_TYPE, MAX_DATA_SIZE> >& pci_req = message->GetRequest();
	ADDRESS_TYPE pci_addr = pci_req->addr;
	PCISpace pci_space = pci_req->space;
	unsigned int pci_req_size = pci_req->size;
	TransactionType pci_transaction_type = pci_req->type;
	
	switch(pci_space)
	{
		case SP_MEM:
			switch(pci_transaction_type)
			{
				case TT_READ:
				{
					Pointer<PCIRsp> pci_rsp = new(pci_rsp) PCIRsp();
				
					inherited::Read(pci_addr, pci_rsp->read_data, pci_req_size);
				
					message->rsp = pci_rsp;
					sc_event *rsp_ev = message->GetResponseEvent();
					rsp_ev->notify(pci_bus_cycle_time);
				}
				break;
				case TT_WRITE:
					inherited::Write(pci_addr, pci_req->write_data, pci_req_size);
					break;
			}
			break;
		case SP_IO:
			if(inherited::logger_import)
				(*inherited::logger_import) << DebugWarning << "I don't have I/O space" << Endl << EndDebugWarning;
			break;
		case SP_CONFIG:
			if(((pci_addr >> 11) & 31) != inherited::pci_device_number)
			{
				if(inherited::logger_import)
				{
					(*inherited::logger_import) << DebugWarning;
					(*inherited::logger_import) << "out of range configuration space access\n" << Endl;
					(*inherited::logger_import) << "PCI config base address is 0x" << Hex << inherited::pci_conf_base_addr << Dec << Endl;
					(*inherited::logger_import) << "Requested address is 0x" << Hex << pci_addr << Dec << Endl;
					(*inherited::logger_import) << EndDebugWarning;
				}
			}
			
			switch(pci_transaction_type)
			{
				case TT_READ:
					{
						Pointer<PCIRsp> pci_rsp = new(pci_rsp) PCIRsp();
					
						unsigned int byte_num, pci_conf_offset;
						for(byte_num = 0, pci_conf_offset = pci_addr & 0xff; byte_num < pci_req_size; byte_num++, pci_conf_offset++)
						{
							pci_rsp->read_data[byte_num] = inherited::ReadConfigByte(pci_conf_offset);
							if(inherited::logger_import)
								(*inherited::logger_import) << DebugInfo << "Reading PCI config at 0x" << Hex << pci_conf_offset << ": value=0x" << (unsigned int) pci_rsp->read_data[byte_num] << Dec << Endl << EndDebugInfo;
						}
					
						message->rsp = pci_rsp;
						sc_event *rsp_ev = message->GetResponseEvent();
						rsp_ev->notify(pci_bus_cycle_time);
					}
					break;
				case TT_WRITE:
					{
						unsigned int byte_num, pci_conf_offset;
						unsigned int pci_req_size = pci_req->size;
						for(byte_num = 0, pci_conf_offset = pci_addr & 0xff; byte_num < pci_req_size; byte_num++, pci_conf_offset++)
						{
							if(inherited::logger_import)
								(*inherited::logger_import) << DebugInfo << "Writing PCI config at 0x" << Hex << pci_conf_offset << ": value=0x" << (unsigned int) pci_req->write_data[byte_num] << Dec << Endl << EndDebugInfo;
							inherited::WriteConfigByte(pci_conf_offset, pci_req->write_data[byte_num]);
						}
					}
					break;
			}
			break;
	}
		
	return true;
}

template <class ADDRESS_TYPE, uint32_t MAX_DATA_SIZE>
bool PCIStub<ADDRESS_TYPE, MAX_DATA_SIZE>::Setup()
{
	if(!inherited::Setup()) return false;
	pci_bus_cycle_time = sc_time(1.0 / (double) (*this)["pci-bus-frequency"], SC_US);
	bus_cycle_time = sc_time(1.0 / (double) (*this)["bus-frequency"], SC_US);
	return true;
}

template <class ADDRESS_TYPE, uint32_t MAX_DATA_SIZE>
void PCIStub<ADDRESS_TYPE, MAX_DATA_SIZE>::Start()
{
}

template <class ADDRESS_TYPE, uint32_t MAX_DATA_SIZE>
void PCIStub<ADDRESS_TYPE, MAX_DATA_SIZE>::Stop()
{
	sc_stop();
	wait();
}

template <class ADDRESS_TYPE, uint32_t MAX_DATA_SIZE>
void PCIStub<ADDRESS_TYPE, MAX_DATA_SIZE>::Intr(uint32_t intr_id)
{
	has_intr = true;
}

template <class ADDRESS_TYPE, uint32_t MAX_DATA_SIZE>
void PCIStub<ADDRESS_TYPE, MAX_DATA_SIZE>::Run(uint64_t duration, typename inherited::inherited::TIME_UNIT tu)
{
	if(has_intr)
	{
		has_intr = false;
		ev_trigger_intr.notify(SC_ZERO_TIME);
		wait(SC_ZERO_TIME);
	}
	
	if(duration)
	{
		sc_time_unit sc_tu = SC_MS;
		
		switch(tu)
		{
			case inherited::inherited::TU_FS: sc_tu = SC_FS; break;
			case inherited::inherited::TU_PS: sc_tu = SC_PS; break;
			case inherited::inherited::TU_NS: sc_tu = SC_NS; break;
			case inherited::inherited::TU_US: sc_tu = SC_US; break;
			case inherited::inherited::TU_MS: sc_tu = SC_MS; break;
			case inherited::inherited::TU_S: sc_tu = SC_SEC; break;
		}
		
		wait(sc_time((double) duration, sc_tu), trap);
	}
	else
	{
		wait(trap);
	}
}

template <class ADDRESS_TYPE, uint32_t MAX_DATA_SIZE>
void PCIStub<ADDRESS_TYPE, MAX_DATA_SIZE>::TriggerInterrupt()
{
	while(1)
	{
		wait(ev_trigger_intr);
		
		if(inherited::logger_import)
		{
			(*inherited::logger_import) << DebugInfo;
			(*inherited::logger_import) << "interrupt at " << sc_time_stamp().to_string() << Endl;
			(*inherited::logger_import) << EndDebugInfo;
		}

		Pointer<TlmMessage<InterruptRequest> > message = new(message) TlmMessage<InterruptRequest>();
		Pointer<InterruptRequest> irq = new (irq) InterruptRequest();
		irq->level = true;
		irq->serial_id = 0;
		message->req = irq;
		
		while(!cpu_irq_port->Send(message))
		{
			wait(bus_cycle_time);
		}
	}
}

template <class ADDRESS_TYPE, uint32_t MAX_DATA_SIZE>
void PCIStub<ADDRESS_TYPE, MAX_DATA_SIZE>::Process()
{
	while(1)
	{
		inherited::inherited::Step();
	}
}

template <class ADDRESS_TYPE, uint32_t MAX_DATA_SIZE>
void PCIStub<ADDRESS_TYPE, MAX_DATA_SIZE>::Trap(uint64_t& t, typename inherited::inherited::TIME_UNIT& tu)
{
	trap.notify(SC_ZERO_TIME);

	sc_time time_res = sc_get_time_resolution();
	double time_res_sec = time_res.to_seconds();

	if(::fabs(time_res_sec - 1.0) <= 0.5) tu = inherited::inherited::TU_S; else
	if(::fabs(time_res_sec - 1.0e-3) <= 0.5e-3) tu = inherited::inherited::TU_MS; else
	if(::fabs(time_res_sec - 1.0e-6) <= 0.5e-6) tu = inherited::inherited::TU_US; else
	if(::fabs(time_res_sec - 1.0e-9) <= 0.5e-9) tu = inherited::inherited::TU_NS; else
	if(::fabs(time_res_sec - 1.0e-12) <= 0.5e-12) tu = inherited::inherited::TU_PS; else
	if(::fabs(time_res_sec - 1.0e-15) <= 0.5e-15) tu = inherited::inherited::TU_FS; else Stop();
	
	t = sc_time_stamp().value();

	wait(SC_ZERO_TIME);
}

} // end of namespace debug
} // end of namespace pci
} // end of namespace tlm
} // end of namespace component
} // end of namespace unisim

#endif /* __UNISIM_COMPONENT_TLM_PCI_DEBUG_PCI_STUB_HH__ */
