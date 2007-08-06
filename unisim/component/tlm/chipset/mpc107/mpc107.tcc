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
 * Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 */

namespace full_system {
namespace tlm {
namespace chipsets {
namespace mpc107 {

#define LOCATION	File << __FILE__ << Function << __FUNCTION__ << Line << __LINE__

template <class PHYSICAL_ADDR, 
		uint32_t MAX_TRANSACTION_DATA_SIZE,
		class PCI_ADDR,
		uint32_t MAX_PCI_TRANSACTION_DATA_SIZE, bool DEBUG>
MPC107<PHYSICAL_ADDR, MAX_TRANSACTION_DATA_SIZE,
	PCI_ADDR, MAX_PCI_TRANSACTION_DATA_SIZE, DEBUG>::
MPC107(const sc_module_name &name, Object *parent) :
	sc_module(name),
	Object(name, parent),
	Service<MemoryInterface<PHYSICAL_ADDR> >(name, parent),
	Client<MemoryInterface<PHYSICAL_ADDR> >(name, parent),
	Client<PCIInterface<PCI_ADDR> >(name, parent),
	Client<LoggerInterface>(name, parent),
	dma(this, "DMA", this),
	epic("epic", this),
	bus_inport("bus_inport"),
	bus_outport("bus_outport"),
//	br_outport("br_outport"),
	slave_pci_port("slave_pci_port"),
	ram_port("ram_port"),
	rom_port("rom_port"),
	erom_port("erom_port"),
	pci_port("pci_port"),
	epic_outport("epic_outport"),
	int_outport("int_outport"),
	soft_reset_outport("soft_reset_outport"),
	sdram_outport("sdram_outport"),
	sdram_to_epic_sig("sdram_to_epic_sig"),
	memory_export("memory_export", this),
	ram_import("ram_import", this),
	rom_import("rom_import", this),
	erom_import("erom_import", this),
	pci_import("pci_import", this),
	logger_import("logger_import", this),
	pci_logger_import("pci_logger_import", this),
	addr_map_logger_import("addr_map_logger_import", this),
	epic_logger_import("epic_logger_import", this),
	epic_reg_logger_import("epic_reg_logger_import", this),
	a_address_map(false),
	host_mode(true),
	memory_32bit_data_bus_size(true),
	rom0_8bit_data_bus_size(false),
	rom1_8bit_data_bus_size(false),
	frequency(0),
	sdram_cycle_time(0),
	param_a_address_map("a_address_map", this, a_address_map),
	param_host_mode("host_mode", this, host_mode),
	param_memory_32bit_data_bus_size("memory_32bit_data_bus_size", this, memory_32bit_data_bus_size),
	param_rom0_8bit_data_bus_size("rom0_8bit_data_bus_size", this, rom0_8bit_data_bus_size),
	param_rom1_8bit_data_bus_size("rom1_8bit_data_bus_size", this, rom1_8bit_data_bus_size),
	param_frequency("frequency", this, frequency),
	param_sdram_cycle_time("sdram_cycle_time", this, sdram_cycle_time),
	config_addr(0),
	config_regs(),
	pci_controller(0, config_regs, addr_map, "pci_controller", this),
	addr_map(config_regs, "address_mapper", this) {
	SetupDependsOn(logger_import);
	bus_inport(*this);
	slave_pci_port(*this);
	
	/* connect the irq input ports to the epic (interrupt controller) */
	for(unsigned int i = 0; 
		i < full_system::tlm::chipsets::mpc107::epic::EPIC<PHYSICAL_ADDR, DEBUG>::NUM_IRQS; 
		i++) {
		stringstream irq_inport_name;
		
		irq_inport_name << "irq_inport[" << i << "]";
		irq_inport[i] = new sc_export<TlmSendIf<IntType> >(irq_inport_name.str().c_str());
		(*irq_inport[i])(*epic.irq_inport[i]);
	}
	
	/* connect the int_outport and soft_reset_outport of the epic to the 
	 * int_outport and soft_reset_outport respectively of the mpc107 */
	epic.int_outport(int_outport);
	epic.soft_reset_outport(soft_reset_outport);
	/* connect the mpc107 to the epic controller with the epic_outport to send
	 *   read/write commands */
	epic_outport(epic.req_inport);
	/* connect the sdram_outport port to the epic sdram_inport port */
	sdram_outport(sdram_to_epic_sig);
	epic.sdram_inport(sdram_to_epic_sig);
	
	/* connect loggers */
	pci_controller.logger_import >> pci_logger_import;
	addr_map.logger_import >> addr_map_logger_import;
	epic.logger_import >> epic_logger_import;
	epic.reg_logger_import >> epic_reg_logger_import;
	
	SC_THREAD(PCIDispatch);
	SC_THREAD(DispatchPCIReq);
	SC_THREAD(DispatchDMALocalMemoryAccess);
	SC_THREAD(DispatchDMAPCIAccess);
}

template <class PHYSICAL_ADDR, 
		uint32_t MAX_TRANSACTION_DATA_SIZE,
		class PCI_ADDR,
		uint32_t MAX_PCI_TRANSACTION_DATA_SIZE, bool DEBUG>
MPC107<PHYSICAL_ADDR, MAX_TRANSACTION_DATA_SIZE,
	PCI_ADDR, MAX_PCI_TRANSACTION_DATA_SIZE, DEBUG>::
~MPC107() {
	
}
		
template <class PHYSICAL_ADDR, 
		uint32_t MAX_TRANSACTION_DATA_SIZE,
		class PCI_ADDR,
		uint32_t MAX_PCI_TRANSACTION_DATA_SIZE, bool DEBUG>
bool 
MPC107<PHYSICAL_ADDR, MAX_TRANSACTION_DATA_SIZE,
	PCI_ADDR, MAX_PCI_TRANSACTION_DATA_SIZE, DEBUG>::
Setup() {
	if(!logger_import) {
		if(DEBUG)
			cerr << "WARNING("
				<< __FUNCTION__ << ":"
				<< __FILE__ << ":"
				<< __LINE__ << "): "
				<< "No Logger exists to generate the output messages" << endl;
	}

	/* IMPORTANT: before initializing the different components the configuration
	 *   registers must be set */
	if(!config_regs.Reset(host_mode, 
		a_address_map,
		memory_32bit_data_bus_size,
		rom0_8bit_data_bus_size, 
		rom1_8bit_data_bus_size)) {
		if(logger_import) 
			(*logger_import) << DebugError
				<< Function << __FUNCTION__ << File << __FILE__ << Line << __LINE__
				<< "Error while initializing the configuration registers"
				<< Endl << EndDebugError;
		else
			cerr << "ERROR("
				<< __FUNCTION__ << ":"
				<< __FILE__ << ":"
				<< __LINE__ << "): "
				<< "Error while initializing the configuration registers"
				<< endl;
		return false;
	}

	/* initialize the address map */
	if(!addr_map.Reset()) {
		if(logger_import)
			(*logger_import) << DebugError
				<< Function << __FUNCTION__ << File << __FILE__ << Line << __LINE__
				<< "Error while initializing the address map"
				<< Endl << EndDebugError;
		else
			cerr << "ERROR("
				<< __FUNCTION__ << ":"
				<< __FILE__ << ":"
				<< __LINE__ << "): "
				<< "Error while initializing the address map"
				<< endl;
	}
	
	if(!host_mode) {
		if(logger_import)
			(*logger_import) << DebugError << "SERVICE_SETUP_ERROR("
				<< __FUNCTION__ << ":"
				<< __FILE__ << ":"
				<< __LINE__ << "): Only host mode is supported (sorry)" 
				<< Endl << EndDebugError;
		else
			cerr << "ERROR("
				<< __FUNCTION__ << ":"
				<< __FILE__ << ":"
				<< __LINE__ << "): "
				<< "Only host mode is supported (sorry)"
				<< endl;
		return false; // only host mode supported
	}

	if(!frequency) {
		if(logger_import)
			(*logger_import) << DebugError << "SERVICE_SETUP_ERROR("
				<< __FUNCTION__ << ":"
				<< __FILE__ << ":"
				<< __LINE__ << "): frequency parameter has not been set" 
				<< Endl << EndDebugError;
		else
			cerr << "ERROR("
				<< __FUNCTION__ << ":"
				<< __FILE__ << ":"
				<< __LINE__ << "): "
				<< "Frequency parameter has not been set"
				<< endl;
		return false;
	}
	cycle_time = sc_time(1.0 / (double) frequency, SC_US);
	if(!sdram_cycle_time) {
		if(logger_import)
			(*logger_import) << DebugError << "SERVICE_SETUP_ERROR("
				<< __FUNCTION__ << ":"
				<< __FILE__ << ":"
				<< __LINE__ << "): sdram_cycle_time parameter has not been set" 
				<< Endl << EndDebugError;
		else
			cerr << "ERROR("
				<< __FUNCTION__ << ":"
				<< __FILE__ << ":"
				<< __LINE__ << "): "
				<< "sdram_cycle_time parameter has not been set"
				<< endl;
		return false;
	}
	/* set the initial sdram_frequency */
	sdram_outport = sdram_cycle_time;

	if(!rom_import) {
		if(logger_import)
			(*logger_import) << DebugError << "SERVICE_SETUP_ERROR("
				<< __FUNCTION__ << ":"
				<< __FILE__ << ":"
				<< __LINE__ << "): rom_import not connected" 
				<< Endl << EndDebugError;
		else
			cerr << "ERROR("
				<< __FUNCTION__ << ":"
				<< __FILE__ << ":"
				<< __LINE__ << "): "
				<< "rom_import not connected"
				<< endl;
		return false;
	}
	if(!ram_import) {
		if(logger_import)
			(*logger_import) << DebugError << "SERVICE_SETUP_ERROR("
				<< __FUNCTION__ << ":"
				<< __FILE__ << ":"
				<< __LINE__ << "): ram_import not connected" 
				<< Endl << EndDebugError;
		else
			cerr << "ERROR("
				<< __FUNCTION__ << ":"
				<< __FILE__ << ":"
				<< __LINE__ << "): "
				<< "ram_import not connected"
				<< endl;
		return false;
	}
	if(!erom_import) {
		if(logger_import)
			(*logger_import) << DebugError << "SERVICE_SETUP_ERROR("
				<< __FUNCTION__ << ":"
				<< __FILE__ << ":"
				<< __LINE__ << "): erom_import not connected" 
				<< Endl << EndDebugError;
		else
			cerr << "ERROR("
				<< __FUNCTION__ << ":"
				<< __FILE__ << ":"
				<< __LINE__ << "): "
				<< "erom_import not connected"
				<< endl;
		return false;
	}
	if(!pci_import) {
		if(logger_import)
			(*logger_import) << DebugError << "SERVICE_SETUP_ERROR("
				<< __FUNCTION__ << ":"
				<< __FILE__ << ":"
				<< __LINE__ << "): pci_import not connected" 
				<< Endl << EndDebugError;
		else
			cerr << "ERROR("
				<< __FUNCTION__ << ":"
				<< __FILE__ << ":"
				<< __LINE__ << "): "
				<< "pci_import not connected"
				<< endl;
		return false;
	}

	return true;
}

template <class PHYSICAL_ADDR, 
		uint32_t MAX_TRANSACTION_DATA_SIZE,
		class PCI_ADDR,
		uint32_t MAX_PCI_TRANSACTION_DATA_SIZE, bool DEBUG>
void
MPC107<PHYSICAL_ADDR, MAX_TRANSACTION_DATA_SIZE,
	PCI_ADDR, MAX_PCI_TRANSACTION_DATA_SIZE, DEBUG>::
DEBUG_PCI_REQ(const PPCIReqType &req) {
	(*logger_import) << "(addr = 0x" << Hex << req->addr << Dec << ", " 
		<< "type = " << ((req->type == full_system::pci::TT_READ) ? "READ" : "WRITE") << ", " 
		<< "space = " << ((req->space == full_system::pci::SP_MEM) ? "MEMORY" : 
							(req->space == full_system::pci::SP_IO) ? "I/O" : "CONFIG") << ", " 
		<< "size = " << req->size;
	if(req->type == full_system::pci::TT_WRITE) {
		(*logger_import) << ", data = 0x";
		for(unsigned int i = 0; i < req->size - 1; i++)
			(*logger_import) << Hex << (unsigned int)req->write_data[i] << " ";
		(*logger_import) << (unsigned int)req->write_data[req->size - 1] << Dec; 
	} 
	(*logger_import) << ")";
}
		
template <class PHYSICAL_ADDR, 
		uint32_t MAX_TRANSACTION_DATA_SIZE,
		class PCI_ADDR,
		uint32_t MAX_PCI_TRANSACTION_DATA_SIZE, bool DEBUG>
bool 
MPC107<PHYSICAL_ADDR, MAX_TRANSACTION_DATA_SIZE,
	PCI_ADDR, MAX_PCI_TRANSACTION_DATA_SIZE, DEBUG>::
Send(const PPCIMsgType &pci_message) {
	const PPCIReqType& pci_req = pci_message->GetRequest();

	/* check if the chipset can response to pci memory space requests */
	if((config_regs.pci_command_reg.value & (1 << 1)) == 0) {
		if(DEBUG && logger_import) {
			(*logger_import) << DebugWarning
				<< Function << __FUNCTION__ << File << __FILE__ << Line << __LINE__
				<< "Received pci request but the chipset is configured not to respond to pci memory space requests";
			DEBUG_PCI_REQ(pci_req);
			(*logger_import) << "THE REQUEST IS ACCEPTED BUT A REPLY WILL NOT BE SENT" << Endl << EndDebugWarning;
		}
		return true;
	}
	
	
	AddressMapEntry *entry = addr_map.GetEntryProc(pci_req->addr);
	if(entry == NULL) {
		if(logger_import) {
			(*logger_import) << DebugError
				<< Function << __FUNCTION__ << File << __FILE__ << Line << __LINE__
				<< "Could not get an address map entry for address 0x"
				<< Hex << pci_req->addr << Dec << " for a request comming from the pci bus"
				<< Endl
				<< EndDebugError;
		}
		sc_stop();
		wait(); 
	}
	switch(entry->type) {
	case AddressMapEntry::LOCAL_MEMORY_SPACE:
		return SendPCItoMemory(pci_message, ram_port);
		break;
	case AddressMapEntry::RESERVED:
		if(logger_import) {
			(*logger_import) << DebugError 
				<< Function << __FUNCTION__ << File << __FILE__ << Line << __LINE__
				<< " Send() from PCI bus accessing a reserved entry address ";
			DEBUG_PCI_REQ(pci_req);
			(*logger_import) << Endl << EndDebugError;
		}
		sc_stop();
		wait();
		return false; // never executed
		break;
	case AddressMapEntry::EXTENDED_ROM_1:
		return SendPCItoMemory(pci_message, erom_port);
		break;
	case AddressMapEntry::EXTENDED_ROM_2:
		return SendPCItoMemory(pci_message, erom_port);
		break;
	case AddressMapEntry::PCI_MEMORY_SPACE:
		/* The request should be simply ignored,
		 * 	 even better it should be never received */
		if(logger_import) {
			(*logger_import) << DebugError
				<< Function << __FUNCTION__ << File << __FILE__ << Line << __LINE__
				<< " Send() from PCI bus to PCI memory space ";
			DEBUG_PCI_REQ(pci_req);
			(*logger_import) << Endl
				<< "This should never happen!!!!" << Endl 
				<< EndDebugError;
		}
		sc_stop();
		wait();
		return false; // never executed
		break;
	case AddressMapEntry::ROM_SPACE_1:
		return SendPCItoMemory(pci_message, rom_port);
		break;
	case AddressMapEntry::ROM_SPACE_2:
		return SendPCItoMemory(pci_message, rom_port);
		break;
	default:
		if(logger_import) {
			(*logger_import) << DebugError
				<< Function << __FUNCTION__ << File << __FILE__ << Line << __LINE__
				<< " Send() received a request to the pci memory space that is not a "
				<< "read neither a write ";
			DEBUG_PCI_REQ(pci_req);
			(*logger_import) << Endl << EndDebugError;
		}
		sc_stop();
		wait();
		return false; // never executed
		break;
	}
}

template <class PHYSICAL_ADDR, 
		uint32_t MAX_TRANSACTION_DATA_SIZE,
		class PCI_ADDR,
		uint32_t MAX_PCI_TRANSACTION_DATA_SIZE, bool DEBUG>
void 
MPC107<PHYSICAL_ADDR, MAX_TRANSACTION_DATA_SIZE,
	PCI_ADDR, MAX_PCI_TRANSACTION_DATA_SIZE, DEBUG>::
DEBUG_BUS_REQ(const PReqType &req) {
	(*logger_import) << "(addr = 0x" << Hex << req->addr << Dec
		<< ", type = ";
	switch(req->type) {
	case ReqType::READ:
		(*logger_import) << "READ";
		break;
	case ReqType::READX:
		(*logger_import) << "READX";
		break;
	case ReqType::WRITE:
		(*logger_import) << "WRITE";
		break;
	case ReqType::INV_BLOCK:
		(*logger_import) << "INV_BLOCK";
		break;
	case ReqType::FLUSH_BLOCK:
		(*logger_import) << "FLUSH_BLOCK";
		break;
	case ReqType::ZERO_BLOCK:
		(*logger_import) << "ZERO_BLOCK";
		break;
	case ReqType::INV_TLB:
		(*logger_import) << "INV_TLB";
		break;
	}
	(*logger_import) << ", global = " << (req->global ? "TRUE" : "FALSE")
		<< ", size = " << req->size;
	if(req->type == ReqType::WRITE) {
		(*logger_import) << ", data = 0x" << Hex;
		for(unsigned int i = 0; i < req->size - 1; i++) 
			(*logger_import) << (unsigned int)req->write_data[i] << " ";
		(*logger_import) << (unsigned int)req->write_data[req->size - 1];
	}
	(*logger_import) << ")";
}

template <class PHYSICAL_ADDR, 
		uint32_t MAX_TRANSACTION_DATA_SIZE,
		class PCI_ADDR,
		uint32_t MAX_PCI_TRANSACTION_DATA_SIZE, bool DEBUG>
bool 
MPC107<PHYSICAL_ADDR, MAX_TRANSACTION_DATA_SIZE,
	PCI_ADDR, MAX_PCI_TRANSACTION_DATA_SIZE, DEBUG>::
Send(const PMsgType &message) {
	const PReqType& req = message->GetRequest();
	PPCIReqType pci_req = new(pci_req) PCIReqType();
	PPCIMsgType pci_msg = new(pci_msg) PCIMsgType(pci_req);

	AddressMapEntry *entry = addr_map.GetEntryProc(req->addr);
	if(entry == NULL) {
		if(logger_import) {
			(*logger_import) << DebugError
				<< Function << __FUNCTION__ << File << __FILE__ << Line << __LINE__
				<< "Could not get an address map entry for address 0x"
				<< Hex << req->addr << Dec << " for a request comming from the system bus"
				<< Endl
				<< EndDebugError;
		}
		sc_stop();
		wait(); 
		return false;
	}
	switch(entry->type) {
	case AddressMapEntry::LOCAL_MEMORY_SPACE:
		return ram_port->Send(message);
		break;
	case AddressMapEntry::RESERVED:
		if(logger_import) {
			(*logger_import) << DebugError
				<< Function << __FUNCTION__ << File << __FILE__ << Line << __LINE__
				<< "Send() from system bus accessing a reserved entry address ";
			DEBUG_BUS_REQ(req);
			(*logger_import) << Endl << EndDebugInfo;
		}
		sc_stop();
		wait();
		return false;
		break;
	case AddressMapEntry::EXTENDED_ROM_1:
		return erom_port->Send(message);
		break;
	case AddressMapEntry::EXTENDED_ROM_2:
		return erom_port->Send(message);
		break;
	case AddressMapEntry::PCI_MEMORY_SPACE:
		/* create a new pci message */
		switch(req->type) {
		case ReqType::READ:
		case ReqType::READX:
			if(!pci_controller.GetReadRequest(req->addr, req->size,
							pci_req->type,
							pci_req->addr,
							pci_req->space,
							pci_req->size) ) {
				if(logger_import) {
					(*logger_import) << DebugError
						<< sc_time_stamp().to_string()
						<< "Send() from system bus received a bad read request from the system bus ";
					DEBUG_BUS_REQ(req);
					(*logger_import) << Endl << EndDebugError;
				}
				sc_stop();
				wait(); 		
			}
			if(!message->HasResponseEvent()) {
				if(logger_import) {
					(*logger_import) << DebugError
						<< sc_time_stamp().to_string()
						<< Function << __FUNCTION__ << File << __FILE__ << Line << __LINE__
						<< "Send() from system bus received a read request without response event "; 
					DEBUG_BUS_REQ(req);
					(*logger_import) << Endl << EndDebugError;
				}
				sc_stop();
				wait();
			}
			break;
		case ReqType::WRITE:
			if(!pci_controller.GetWriteRequest(req->addr, req->write_data, req->size,
								pci_req->type,
								pci_req->addr,
								pci_req->space,
								pci_req->write_data,
								pci_req->size) ) {
				if(logger_import) {
					(*logger_import) << DebugError
						<< Function << __FUNCTION__ << File << __FILE__ << Line << __LINE__
						<< "Send() from system bus received a bad write request from the system bus ";
					DEBUG_BUS_REQ(req);
					(*logger_import) << Endl << EndDebugError;
				}
				sc_stop();
				wait();
			}
			break;
		default:
			if(logger_import) {
				(*logger_import) << DebugError
					<< Function << __FUNCTION__ << File << File << Line << __LINE__
					<< "Send() from system bus received a request to the pci memory space that is not a "
					<< "read neither a write "; 
				DEBUG_BUS_REQ(req);
				(*logger_import) << Endl << EndDebugError;
			}
			sc_stop();
			wait();
			return false;
			break;
		}
		/* put the message in the in pci request list */
		req_list.push_back(message);
		bus_pci_req_list.push_back(pci_msg);
		if(bus_pci_req_list.size() == 1) {
			pci_dispatch_event.notify(SC_ZERO_TIME);
		}
		return true;
		break;
	case AddressMapEntry::PCI_IO_SPACE_1:
		switch(req->type) {
		case ReqType::READ:
		case ReqType::READX:
			if(!pci_controller.GetIOReadRequest(req->addr, req->size,
							pci_req->type,
							pci_req->addr,
							pci_req->space,
							pci_req->size) ) {
				if(logger_import) {
					(*logger_import) << DebugError
						<< sc_time_stamp().to_string()
						<< Function << __FUNCTION__ << File << __FILE__ << Line << __LINE__
						<< "Send() from system bus received a bad PCI I/O read request from the system bus ";
					DEBUG_BUS_REQ(req);
					(*logger_import) << Endl << EndDebugError;
				}
				sc_stop();
				wait(); 		
			}
			if(!message->HasResponseEvent()) {
				if(logger_import) {
					(*logger_import) << DebugError
						<< Function << __FUNCTION__ << File << __FILE__ << Line << __LINE__
						<< "Send() from system bus received a PCI I/O read request without response event "; 
					DEBUG_BUS_REQ(req);
					(*logger_import) << Endl << EndDebugError;
				}
				sc_stop();
				wait();
			}			
			break;
		case ReqType::WRITE:
			if(!pci_controller.GetIOWriteRequest(req->addr, req->write_data, req->size,
								pci_req->type,
								pci_req->addr,
								pci_req->space,
								pci_req->write_data,
								pci_req->size) ) {
				if(logger_import) {
					(*logger_import) << DebugError
						<< Function << __FUNCTION__ << File << __FILE__ << Line << __LINE__
						<< "Send() from system bus received a bad PCI I/O write request from the system bus ";
					DEBUG_BUS_REQ(req);
					(*logger_import) << Endl << EndDebugError;
				}
				sc_stop();
				wait();
			}
			break;
		default:
			if(logger_import) {
				(*logger_import) << DebugError
					<< Function << __FUNCTION__ << File << __FILE__ << Line << __LINE__
					<< "Send() from system bus received a request to the PCI I/O space that is not a "
					<< "read neither a write request ";
				DEBUG_BUS_REQ(req);
				(*logger_import) << Endl << EndDebugError;
			}
			sc_stop();
			wait();
			return false;
			break;
		}
		/* put the message in the in pci request list */
		req_list.push_back(message);
		bus_pci_req_list.push_back(pci_msg);
		if(bus_pci_req_list.size() == 1)
			pci_dispatch_event.notify(SC_ZERO_TIME);
		return true;
		break;
	case AddressMapEntry::PCI_IO_SPACE_2:
		if(DEBUG && logger_import) {
			(*logger_import) << DebugWarning
				<< Function << __FUNCTION__ << File << __FILE__ << Line << __LINE__
				<< " accessing the PCI I/O space 1 ";
			DEBUG_BUS_REQ(req);
			(*logger_import) << Endl << EndDebugWarning;
		}
		return false;
		break;
	case AddressMapEntry::PCI_CONFIG_ADDRESS_REG:
		switch(req->type) {
		case ReqType::READ:
		case ReqType::READX: 
			{
				PRspType rsp = new(rsp) RspType();
				if(logger_import) {
					(*logger_import) << DebugWarning
						<< Function << __FUNCTION__ << File << __FILE__ << Line << __LINE__
						<< "making a read to the pci config address register, "
						<< "I am returning 0, but maybe something else should be done "; 
					DEBUG_BUS_REQ(req);
					(*logger_import) << Endl << EndDebugWarning;
				}
				if(!message->HasResponseEvent()) {
					if(logger_import) {
						(*logger_import) << DebugError
							<< Function << __FUNCTION__ << File << __FILE__ << Line << __LINE__
							<< " received a read request without response event "; 
						DEBUG_BUS_REQ(req);
						(*logger_import) << Endl << EndDebugError;
					}
					sc_stop();
					wait();
				}
				memset(rsp->read_data, 0, MAX_TRANSACTION_DATA_SIZE);
				rsp->read_status = RspType::RS_MISS;
				message->rsp = rsp;
				message->GetResponseEvent()->notify(cycle_time);
			}	
			break;
		case ReqType::WRITE:
			if(!pci_controller.SetConfigAddr(req->addr,
								req->write_data,
								req->size)) {
				if(logger_import) {
					(*logger_import) << DebugError
						<< Function << __FUNCTION__ << File << __FILE__ << Line << __LINE__
						<< "bad write access to the configuration address register memory space ";
					DEBUG_BUS_REQ(req);
					(*logger_import) << Endl << EndDebugError;
				}
				sc_stop();
				wait();
			}
			/* nothing else to do */
			break;
		default:
			if(logger_import) {
				(*logger_import) << DebugError
					<< Function << __FUNCTION__ << File << __FILE__ << Line << __LINE__
					<< "Send() from system bus received a request to the pci config address register space that is not a "
					<< "read neither a write request ";
				DEBUG_BUS_REQ(req);
				(*logger_import) << Endl << EndDebugError;
			}
			sc_stop();
			wait();
			return false;
			break;
		}
		return true;
		break;
	case AddressMapEntry::PCI_CONFIG_DATA_REG:
		switch(req->type) {
		case ReqType::READ:
		case ReqType::READX:
			if(pci_controller.IsLocalAccess()) {
				PRspType rsp = new(rsp) RspType();
				if(!pci_controller.LocalReadConfigRequest(req->addr, req->size,
									rsp->read_data)) {
					if(logger_import) {
						(*logger_import) << DebugError
							<< Function << __FUNCTION__ << File << __FILE__ << Line << __LINE__
							<< "Send() from system bus received a bad read for the pci configuration data register ";
						DEBUG_BUS_REQ(req);
						(*logger_import) << Endl << EndDebugError;
					}
					sc_stop();
					wait(); 		
				}
				message->rsp = rsp;
				message->GetResponseEvent()->notify(cycle_time);
				return true;			
			} else {
				if(!pci_controller.GetReadConfigRequest(req->addr, req->size,
									pci_req->type,
									pci_req->addr,
									pci_req->space,
									pci_req->size) ) {
					if(logger_import) {
						(*logger_import) << DebugError
							<< Function << __FUNCTION__ << File << __FILE__ << Line << __LINE__
							<< "Send() from system bus received a bad read for the PCI configuration data register ";
						DEBUG_BUS_REQ(req);
						(*logger_import) << Endl << EndDebugError;
					}
					sc_stop();
					wait(); 		
				}
			}
			break;
		case ReqType::WRITE:
			if(pci_controller.IsLocalAccess()) {
				if(!pci_controller.LocalWriteConfigRequest(req->addr, req->write_data, req->size)) {
					if(logger_import) {
						(*logger_import) << DebugError
							<< Function << __FUNCTION__ << File << __FILE__ << Line << __LINE__
							<< "Send() from system bus received a bad write for the pci configuration data register ";
						DEBUG_BUS_REQ(req);
						(*logger_import) << Endl << EndDebugError;
					}
					sc_stop();
					wait(); 		
				}
				return true;			
			} else {
				if(!pci_controller.GetWriteConfigRequest(req->addr, req->write_data, req->size,
									pci_req->type,
									pci_req->addr,
									pci_req->space,
									pci_req->write_data,
									pci_req->size) ) {
					if(logger_import) {
						(*logger_import) << DebugError
							<< Function << __FUNCTION__ << File << __FILE__ << Line << __LINE__
							<< "Send() from system bus received a bad write request "; 
						DEBUG_BUS_REQ(req);
						(*logger_import) << Endl << EndDebugError;
					}
					sc_stop();
					wait();
				}
			}
			break;
		default:
			if(logger_import) {
				(*logger_import) << DebugError
					<< "Send() from system bus received a request to the pci config data register space that is not a "
					<< "read neither a write "; 
				DEBUG_BUS_REQ(req);
				(*logger_import) << Endl << EndDebugError;
			}
			sc_stop();
			wait();
			return false;
			break;
		}
		/* put the message in the in pci request list */
		req_list.push_back(message);
		bus_pci_req_list.push_back(pci_msg);
		if(bus_pci_req_list.size() == 1) {
			pci_dispatch_event.notify(SC_ZERO_TIME);
		}
		return true;
		break;
	case AddressMapEntry::PCI_INT_ACK:
		if(logger_import) {
			(*logger_import) << DebugError
				<< Function << __FUNCTION__ << File << __FILE__ << Line << __LINE__
				<< "Accessing the pci interruption acknowledgement ";
			DEBUG_BUS_REQ(req);
			(*logger_import) << Endl << EndDebugError;
		}
		sc_stop();
		wait();
		return false;
		break;
	case AddressMapEntry::ROM_SPACE_1:
		return rom_port->Send(message);
		break;
	case AddressMapEntry::ROM_SPACE_2:
		return rom_port->Send(message);
		break;
	case AddressMapEntry::EUMB_EPIC_SPACE:
		return epic_outport->Send(message);
		break;
	case AddressMapEntry::EUMB_MSG_UNIT_SPACE:
	case AddressMapEntry::EUMB_DMA_SPACE:
	case AddressMapEntry::EUMB_ATU_SPACE:
	case AddressMapEntry::EUMB_I2C_SPACE:
	case AddressMapEntry::EUMB_DATA_PATH_DIAGNOSTICS_SPACE:
		if(logger_import) {
			(*logger_import) << DebugError
				<< Function << __FUNCTION__ << File << __FILE__ << Line << __LINE__
				<< "TODO Embedded utilities block mapping not implemented ";
			DEBUG_BUS_REQ(req);
			(*logger_import) << Endl << EndDebugError;
		}
		sc_stop();
		wait();
		break;
	default:
		if(logger_import) {
			(*logger_import) << DebugError
				<< Function << __FUNCTION__ << File << __FILE__ << Line << __LINE__
				<< "Accessing an unmapped (or unhalded) memory space ";
			DEBUG_BUS_REQ(req);
			(*logger_import) << Endl << EndDebugError;
		}
		sc_stop();
		wait();
		break;
	}
}

template <class PHYSICAL_ADDR, 
		uint32_t MAX_TRANSACTION_DATA_SIZE,
		class PCI_ADDR,
		uint32_t MAX_PCI_TRANSACTION_DATA_SIZE, bool DEBUG>
void 
MPC107<PHYSICAL_ADDR, MAX_TRANSACTION_DATA_SIZE,
	PCI_ADDR, MAX_PCI_TRANSACTION_DATA_SIZE, DEBUG>::
Reset() {
	/* forward the reset call */
	if(ram_import) ram_import->Reset();
	if(rom_import) rom_import->Reset();
	if(erom_import) erom_import->Reset();
	if(pci_import) pci_import->Reset();
}

template <class PHYSICAL_ADDR, 
		uint32_t MAX_TRANSACTION_DATA_SIZE,
		class PCI_ADDR,
		uint32_t MAX_PCI_TRANSACTION_DATA_SIZE, bool DEBUG>
bool 
MPC107<PHYSICAL_ADDR, MAX_TRANSACTION_DATA_SIZE,
	PCI_ADDR, MAX_PCI_TRANSACTION_DATA_SIZE, DEBUG>::
ReadMemory(PHYSICAL_ADDR addr, void *buffer, uint32_t size) {
	AddressMapEntry *entry = addr_map.GetEntryProc(addr);
	if(entry == NULL) {
		if(logger_import) {
			(*logger_import) << DebugError
				<< Function << __FUNCTION__ << File << __FILE__ << Line << __LINE__
				<< "Could not get an address map entry for address 0x"
				<< Hex << addr << Dec << " for a request comming from the system bus"
				<< Endl
				<< EndDebugError;
		}
		return false;
	}
	switch(entry->type) {
	case AddressMapEntry::LOCAL_MEMORY_SPACE:
		if(DEBUG && logger_import)
			(*logger_import) << DebugInfo
				<< Function << __FUNCTION__ << File << __FILE__ << Line << __LINE__
				<< "ReadMemory() to extended ram (address = " 
				<< Hex << addr << Dec << ")" << Endl
				<< EndDebugInfo;
		return ram_import->ReadMemory(addr, buffer, size);
		break;
	case AddressMapEntry::RESERVED:
		if(DEBUG && logger_import)
			(*logger_import) << DebugWarning
				<< Function << __FUNCTION__ << File << __FILE__ << Line << __LINE__
				<< " accessing a reserved entry address (address = " 
				<< Hex << addr << Dec << ")" << Endl
				<< EndDebugWarning;
		return false;
		break;
	case AddressMapEntry::EXTENDED_ROM_1:
		if(DEBUG && logger_import)
			(*logger_import) << DebugInfo
				<< Function << __FUNCTION__ << File << __FILE__ << Line << __LINE__
				<< "ReadMemory() to extended rom 1 (address = " 
				<< Hex << addr << Dec << ")" << Endl
				<< EndDebugInfo;
		return erom_import->ReadMemory(addr, buffer, size);
		break;
	case AddressMapEntry::EXTENDED_ROM_2:
		if(DEBUG && logger_import)
			(*logger_import) << DebugInfo
				<< Function << __FUNCTION__ << File << __FILE__ << Line << __LINE__
				<< "ReadMemory() to extended rom 2 (address = " 
				<< Hex << addr << Dec << ")" << Endl
				<< EndDebugInfo;
		return erom_import->ReadMemory(addr, buffer, size);
		break;
	case AddressMapEntry::PCI_MEMORY_SPACE:
		/* TODO: to be removed */
//		if(fb_import && 
//			addr >= 0xd0000000L && addr < 0xd0096000L) 
//			fb_import->ReadMemory(addr, buffer, size);
		if(DEBUG && logger_import)
			(*logger_import) << DebugWarning
				<< Function << __FUNCTION__ << File << __FILE__ << Line << __LINE__
				<< " accessing the pci memory space (0x" << Hex << addr << Dec << ")" << Endl
				<< EndDebugWarning;
		return false;
		break;
	case AddressMapEntry::PCI_IO_SPACE_1:
		if(DEBUG && logger_import)
			(*logger_import) << DebugWarning
				<< Function << __FUNCTION__ << File << __FILE__ << Line << __LINE__
				<< " accessing the pci io space 1 (address = " 
				<< Hex << addr << Dec << ")" << Endl
				<< EndDebugWarning;
		return false;
		break;
	case AddressMapEntry::PCI_IO_SPACE_2:
		if(DEBUG && logger_import)
			(*logger_import) << DebugWarning
				<< Function << __FUNCTION__ << File << __FILE__ << Line << __LINE__
				<< " accessing the pci io space 2 (address = " 
				<< Hex << addr << Dec << ")" << Endl
				<< EndDebugWarning;
		return false;
		break;
	case AddressMapEntry::PCI_CONFIG_ADDRESS_REG:
		if(DEBUG && logger_import)
			(*logger_import) << DebugWarning
				<< Function << __FUNCTION__ << File << __FILE__ << Line << __LINE__
				<< " accessing the pci config address register (address = " 
				<< Hex << addr << Dec << ")" << Endl
				<< EndDebugWarning;
		return false;
		break;
	case AddressMapEntry::PCI_CONFIG_DATA_REG:
		if(DEBUG && logger_import)
			(*logger_import) << DebugWarning
				<< Function << __FUNCTION__ << File << __FILE__ << Line << __LINE__
				<< " accessing the pci config data register (address = " 
				<< Hex << addr << Dec << ")" << Endl
				<< EndDebugWarning;
		return false;
		break;
	case AddressMapEntry::PCI_INT_ACK:
		if(DEBUG && logger_import)
			(*logger_import) << DebugWarning
				<< Function << __FUNCTION__ << File << __FILE__ << Line << __LINE__
				<< " accessing the pci interruption acknowledgement (address = " 
				<< Hex << addr << Dec << ")" << Endl
				<< EndDebugWarning;
		return false;
		break;
	case AddressMapEntry::ROM_SPACE_1:
		if(DEBUG && logger_import)
			(*logger_import) << DebugInfo
				<< Function << __FUNCTION__ << File << __FILE__ << Line << __LINE__
				<< "ReadMemory() to rom 1 (address = " << Hex << addr << Dec << ")" << Endl
				<< EndDebugInfo;
		return rom_import->ReadMemory(addr, buffer, size);
		break;
	case AddressMapEntry::ROM_SPACE_2:
		if(DEBUG && logger_import)
			(*logger_import) << DebugInfo
				<< Function << __FUNCTION__ << File << __FILE__ << Line << __LINE__
				<< "ReadMemory() to rom 2 (address = " << Hex << addr << Dec << ")"<< Endl
				<< EndDebugInfo;
		return rom_import->ReadMemory(addr, buffer, size);
		break;
	case AddressMapEntry::EUMB_MSG_UNIT_SPACE:
	case AddressMapEntry::EUMB_DMA_SPACE:
	case AddressMapEntry::EUMB_ATU_SPACE:
	case AddressMapEntry::EUMB_I2C_SPACE:
	case AddressMapEntry::EUMB_EPIC_SPACE:
	case AddressMapEntry::EUMB_DATA_PATH_DIAGNOSTICS_SPACE:
		if(DEBUG && logger_import)
			(*logger_import) << DebugWarning
				<< Function << __FUNCTION__ << File << __FILE__ << Line << __LINE__
				<< " Embedded utilities block mapping not implemented" << Endl
				<< EndDebugWarning;
		return false;
		break;
	default:
		if(DEBUG && logger_import)
			(*logger_import) << DebugWarning
				<< Function << __FUNCTION__ << File << __FILE__ << Line << __LINE__
				<< " accessing an unmapped (or unhalded) memory space (address = "
				<< Hex << addr << Dec << ")" << Endl
				<< EndDebugWarning;
		return false;
		break;
	}
}

template <class PHYSICAL_ADDR, 
		uint32_t MAX_TRANSACTION_DATA_SIZE,
		class PCI_ADDR,
		uint32_t MAX_PCI_TRANSACTION_DATA_SIZE, bool DEBUG>
bool
MPC107<PHYSICAL_ADDR, MAX_TRANSACTION_DATA_SIZE,
	PCI_ADDR, MAX_PCI_TRANSACTION_DATA_SIZE, DEBUG>::
WriteMemory(PHYSICAL_ADDR addr, const void *buffer, uint32_t size) {
	AddressMapEntry *entry = addr_map.GetEntryProc(addr);
	if(entry == NULL) {
		if(logger_import) {
			(*logger_import) << DebugError
				<< Function << __FUNCTION__ << File << __FILE__ << Line << __LINE__
				<< "Could not get an address map entry for address 0x"
				<< Hex << addr << Dec << " for a request comming from the system bus"
				<< Endl
				<< EndDebugError;
		}
		return false;
	}
	switch(entry->type) {
	case AddressMapEntry::LOCAL_MEMORY_SPACE:
		if(DEBUG && logger_import)
			(*logger_import) << DebugInfo
				<< "Write to ram (addr = 0x" << Hex << addr << Dec
				<< ", size = " << size << ")" << Endl
				<< EndDebugInfo;
		return ram_import->WriteMemory(addr, buffer, size);
		break;
	case AddressMapEntry::RESERVED:
		if(DEBUG && logger_import)
			(*logger_import) << DebugWarning
				<< __FUNCTION__ << ":" << __FILE__ << ":" << __LINE__
				<< " accessing a reserved entry address" << Endl
				<< EndDebugWarning;
		return false;
		break;
	case AddressMapEntry::EXTENDED_ROM_1:
		if(DEBUG && logger_import)
			(*logger_import) << DebugInfo
				<< "WriteMemory() to extended rom 1" << Endl
				<< EndDebugInfo;
		return erom_import->WriteMemory(addr, buffer, size);
		break;
	case AddressMapEntry::EXTENDED_ROM_2:
		if(DEBUG && logger_import)
			(*logger_import) << DebugInfo
				<< "WriteMemory() to extended rom 2" << Endl
				<< EndDebugInfo;
		return erom_import->WriteMemory(addr, buffer, size);
		break;
	case AddressMapEntry::PCI_MEMORY_SPACE:
		/* convert the request into a pci request */
		if(DEBUG && logger_import)
			(*logger_import) << DebugWarning
				<< __FUNCTION__ << ":" << __FILE__ << ":" << __LINE__
				<< " accessing the pci memory space" << Endl
				<< EndDebugWarning;
		return false;
		break;
	case AddressMapEntry::PCI_IO_SPACE_1:
		/* convert the request into a pci request */ 
		if(DEBUG && logger_import)
			(*logger_import) << DebugWarning
				<< __FUNCTION__ << ":" << __FILE__ << ":" << __LINE__
				<< " accessing the pci io space 1" << Endl
				<< EndDebugWarning;
		return false;
		break;
	case AddressMapEntry::PCI_IO_SPACE_2:
		/* convert the request into a pci request */ 
		if(DEBUG && logger_import)
			(*logger_import) << DebugWarning
				<< __FUNCTION__ << ":" << __FILE__ << ":" << __LINE__
				<< " accessing the pci io space 1" << Endl
				<< EndDebugWarning;
		return false;
		break;
	case AddressMapEntry::PCI_CONFIG_ADDRESS_REG:
		/* convert the request into a pci request */ 
		if(DEBUG && logger_import)
			(*logger_import) << DebugWarning
				<< __FUNCTION__ << ":" << __FILE__ << ":" << __LINE__
				<< " accessing the pci config address register" << Endl
				<< EndDebugWarning;
		return false;
		break;
	case AddressMapEntry::PCI_CONFIG_DATA_REG:
		/* convert the request into a pci request */ 
		if(DEBUG && logger_import)
			(*logger_import) << DebugWarning
				<< __FUNCTION__ << ":" << __FILE__ << ":" << __LINE__
				<< " accessing the pci config data register" << Endl
				<< EndDebugWarning;
		return false;
		break;
	case AddressMapEntry::PCI_INT_ACK:
		if(DEBUG && logger_import)
			(*logger_import) << DebugWarning
				<< __FUNCTION__ << ":" << __FILE__ << ":" << __LINE__
				<< " accessing the pci config data register" << Endl
				<< EndDebugWarning;
		return false;
		break;
	case AddressMapEntry::ROM_SPACE_1:
		if(DEBUG && logger_import)
			(*logger_import) << DebugInfo
				<< "WriteMemory() to rom 1" << Endl
				<< EndDebugInfo;
		return rom_import->WriteMemory(addr, buffer, size);
		break;
	case AddressMapEntry::ROM_SPACE_2:
		if(DEBUG && logger_import)
			(*logger_import) << DebugInfo
				<< "WriteMemory() to rom 2" << Endl
				<< EndDebugInfo;
		return rom_import->WriteMemory(addr, buffer, size);
		break;
	case AddressMapEntry::EUMB_MSG_UNIT_SPACE:
	case AddressMapEntry::EUMB_DMA_SPACE:
	case AddressMapEntry::EUMB_ATU_SPACE:
	case AddressMapEntry::EUMB_I2C_SPACE:
	case AddressMapEntry::EUMB_EPIC_SPACE:
	case AddressMapEntry::EUMB_DATA_PATH_DIAGNOSTICS_SPACE:
		if(logger_import)
			(*logger_import) << DebugError
				<< "TODO(" << __FUNCTION__			<< ":" << __FILE__ << ":" << __LINE__ 
				<< "): Embedded utilities block mapping not implemented" << Endl
				<< EndDebugError;
		return false;
		break;
	default:
		(*logger_import) << DebugError
			<< "TODO(" << __FUNCTION__			<< ":" << __FILE__ << ":" << __LINE__ 
			<< "): accessing an unmapped (or unhalded) memory space (address = "
			<< Hex << addr << Dec << ")" << Endl
			<< EndDebugError;
		return false;
		break;
	}
}

template <class PHYSICAL_ADDR, 
		uint32_t MAX_TRANSACTION_DATA_SIZE,
		class PCI_ADDR,
		uint32_t MAX_PCI_TRANSACTION_DATA_SIZE, bool DEBUG>
void
MPC107<PHYSICAL_ADDR, MAX_TRANSACTION_DATA_SIZE,
	PCI_ADDR, MAX_PCI_TRANSACTION_DATA_SIZE, DEBUG>::
LocalMemoryDispatch() {
}

template <class PHYSICAL_ADDR, 
		uint32_t MAX_TRANSACTION_DATA_SIZE,
		class PCI_ADDR,
		uint32_t MAX_PCI_TRANSACTION_DATA_SIZE, bool DEBUG>
void
MPC107<PHYSICAL_ADDR, MAX_TRANSACTION_DATA_SIZE,
	PCI_ADDR, MAX_PCI_TRANSACTION_DATA_SIZE, DEBUG>::
PCIDispatch() {
	PMsgType msg;
	PPCIMsgType pci_msg;
	PPCIReqType pci_req;
	PPCIRspType pci_rsp;
	
	/* request reply event */
	sc_event rsp_event;
	
	/* we wait for new events to take pci request from the
	 *  pci request queue if the request does not expect a response
	 *  we suppress the request from the queu. If the request expects
	 *  a response we wait for the response (notification of the event)
	 *  to remove it from the pci request queue. */ 
	while(1) {
		wait(pci_dispatch_event);
		pci_msg = bus_pci_req_list.front();
		msg = req_list.front();
		pci_req = pci_msg->GetRequest();
		if((config_regs.pci_command_reg.value & (1 << 2)) == 0) {
			if(logger_import) {
				(*logger_import) << DebugWarning
						<< __FUNCTION__ << ":" << __FILE__ << ":" << __LINE__
						<< "A PCI request to send through the PCI bus is ready, but the chipset is configured not to send them (PCI command register bit Bus Master ";
				DEBUG_PCI_REQ(pci_req);
				(*logger_import) << Endl
					<< "   TODO: if write held the request, if read rise an exception (if enabled)" << Endl
					<< EndDebugWarning;					
			}
			continue; 
		} 
		/* transform the bus request into a pci request */
		/* send the pci request */
		if(msg->HasResponseEvent()) {
			/* put the the rsp_event in the msg event queue 
			 * send it
			 * wait for the response
			 * send the response to the source of the request */
			pci_msg->PushResponseEvent(rsp_event);
			if(DEBUG && logger_import)
				(*logger_import) << DebugInfo
					<< "Sending read message through the pci bus (addr = 0x" 
					<< Hex << pci_req->addr << Dec << ")" << Endl
					<< EndDebugInfo;
			while(!pci_port->Send(pci_msg)) {
				if(DEBUG && logger_import)
					(*logger_import) << DebugInfo
						<< "could not send message through the pci bus (addr = 0x" 
						<< Hex << pci_req->addr << Dec << ")" << Endl
						<< EndDebugInfo;
				wait(cycle_time);
				if(DEBUG && logger_import)
					(*logger_import) << DebugInfo
						<< "retrying to send message through the pci bus (addr = 0x" 
						<< Hex << pci_req->addr << Dec << ")" << Endl
						<< EndDebugInfo;
			}
			wait(rsp_event);
			PRspType rsp = new(rsp) RspType();
			pci_req = pci_msg->req;
			if(!pci_msg->req) {
				if(DEBUG && logger_import)
					(*logger_import) << DebugError
						<< __FUNCTION__ << ":" << __FILE__ << ":" << __LINE__
						<< " received a response for a pci message without request field" 
						<< Endl << EndDebugError;
				sc_stop();
				wait();
			}
			if(!pci_msg->rsp) {
				if(DEBUG && logger_import)
					(*logger_import) << DebugError
						<< __FUNCTION__ << ":" << __FILE__ << ":" << __LINE__
						<< " received a response for a pci message without response field" 
						<< Endl << EndDebugError;
				sc_stop();
				wait();
			}
			pci_rsp = pci_msg->rsp;
			rsp->read_status = RspType::RS_MISS;
			memcpy(rsp->read_data, pci_rsp->read_data, pci_req->size);
			msg->rsp = rsp;
			msg->GetResponseEvent()->notify(SC_ZERO_TIME); 
		} else {
			/* just send the message */
			if(DEBUG && logger_import) {
				(*logger_import) << DebugInfo
					<< "Sending write message through the pci bus (addr = 0x" 
					<< Hex << pci_req->addr << Dec << ", size = " << pci_req->size << ", data = ";
				for(unsigned int i = 0; i < pci_req->size; i++) {
					(*logger_import) << Hex << (unsigned int)pci_req->write_data[i] << Dec << " ";
				}
				(*logger_import) << ")" << Endl << EndDebugInfo;
			}
			while(!pci_port->Send(pci_msg)) {
				if(DEBUG && logger_import) {
					(*logger_import) << DebugInfo
						<< "could not send write message through the pci bus (addr = 0x" 
						<< Hex << pci_req->addr << Dec << ", size = " << pci_req->size << ", data = ";
					for(unsigned int i = 0; i < pci_req->size; i++) {
						(*logger_import) << Hex << (unsigned int)pci_req->write_data[i] << Dec << " ";
					}
					(*logger_import) << ")" << Endl << EndDebugInfo;
				}
				wait(cycle_time);
				if(DEBUG && logger_import) {
					(*logger_import) << DebugInfo
						<< "retrying send write message through the pci bus (addr = 0x" 
						<< Hex << pci_req->addr << Dec << ", size = " << pci_req->size << ", data = ";
					for(unsigned int i = 0; i < pci_req->size; i++) {
						(*logger_import) << Hex << (unsigned int)pci_req->write_data[i] << Dec << " ";
					}
					(*logger_import) << ")" << Endl << EndDebugInfo;
				}
			}
		}
		bus_pci_req_list.pop_front();
		req_list.pop_front();
		if(!bus_pci_req_list.empty()) pci_dispatch_event.notify_delayed();
	}
}

/** 
 * This method simply queues messages coming from PCI to be send to the memory system. 
 *   It notifies the dispatcher if necessary.
 * 
 * @param pci_msg   the pci message to send
 * @param out_port  the memory port that should be used
 * 
 * @return true on succes, false otherwise
 */
template <class PHYSICAL_ADDR, 
		uint32_t MAX_TRANSACTION_DATA_SIZE,
		class PCI_ADDR,
		uint32_t MAX_PCI_TRANSACTION_DATA_SIZE, bool DEBUG>
bool
MPC107<PHYSICAL_ADDR, MAX_TRANSACTION_DATA_SIZE,
	PCI_ADDR, MAX_PCI_TRANSACTION_DATA_SIZE, DEBUG>::
SendPCItoMemory(const PPCIMsgType &pci_msg, sc_port<TlmSendIf<ReqType, RspType> > &out_port) {
	bool notify = pci_req_list.empty();
	PCItoMemoryReqType *buf;
	
	if(free_pci_req_list.empty()) 
		buf = new PCItoMemoryReqType();
	else {
		buf = free_pci_req_list.front();
		free_pci_req_list.pop_front();
	}
	buf->pci_msg = pci_msg;
	buf->out_port = &out_port;
	pci_req_list.push_back(buf);
	if(notify) {
		dispatch_pci_req_ev.notify();
	}
	
	return true;
}

template <class PHYSICAL_ADDR,
	uint32_t MAX_TRANSACTION_DATA_SIZE,
	class PCI_ADDR,
	uint32_t MAX_PCI_TRANSACTION_DATA_SIZE, bool DEBUG>
void
MPC107<PHYSICAL_ADDR, MAX_TRANSACTION_DATA_SIZE,
	PCI_ADDR, MAX_PCI_TRANSACTION_DATA_SIZE, DEBUG>::
DispatchPCIReq() {
	PCItoMemoryReqType *item;
	while(1) {
		wait(dispatch_pci_req_ev);
		/* get the first element in the list of pci request for the
		 *   memory system */
		item = pci_req_list.front();
		const PPCIReqType& pci_req = item->pci_msg->GetRequest();
		
		/* prepare the bus request message */
//		Pointer<BusRequest> br_req = new(br_req) BusRequest();
//		br_req->req = true;
//		sc_event br_ev;
//		Pointer<TlmMessage<BusRequest, BusGrant> > br_msg =
//			new(br_msg) TlmMessage<BusRequest, BusGrant>(br_req, br_ev);
//		/* requesting the bus */
//		for(bool finished = false; !finished;) {
//			while(!br_outport->Send(br_msg))
//				wait(cycle_time);
//			wait(br_ev);
//			const Pointer<BusGrant> &br_rsp =
//				br_msg->GetResponse();
//			finished = br_rsp->grant;
//		}
		
		/* prepare a request (and message) for the system bus */
		PReqType sys_req = new(sys_req) ReqType();
		PMsgType sys_msg = new(sys_msg) MsgType(sys_req);
		sc_event sys_msg_ev;
		sys_req->global = true;
		sys_req->addr = pci_req->addr;
		sys_req->size = pci_req->size;
		switch(pci_req->type) {
		case full_system::pci::TT_READ:
			sys_req->type = ReqType::READ;
			sys_msg->PushResponseEvent(sys_msg_ev);
			break;
		case full_system::pci::TT_WRITE:
			memcpy(sys_req->write_data, pci_req->write_data, pci_req->size);
			sys_req->type = ReqType::WRITE;
			break;
		default:
			if(logger_import)
				(*logger_import) << DebugError
					<< "Unknown pci request type" << Endl
					<< EndDebugError;
			sc_stop();
			wait();
			break;		
		}
		
		/* send the message to the system bus */
		while(!bus_outport->Send(sys_msg)) {
			if(logger_import)
				(*logger_import) << DebugError
					<< "Message was not accepted by the system bus" << Endl
					<< EndDebugError;
			sc_stop();
			wait();
		}
		
		/* wait for the message response if the request was
		 *   a read message,
		 *   if the request was a hit, then send the response
		 *   to the pci bus, otherwise it will have to be sent to
		 *   the memory system */
		if(sys_req->type == ReqType::READ) {
			wait(sys_msg_ev);
			const PRspType &sys_rsp = sys_msg->GetResponse();
			if(!(sys_rsp->read_status == RspType::RS_SHARED ||
				sys_rsp->read_status == RspType::RS_MODIFIED)) {
				// the response was not received from the system bus,
				//   it needs to be sent to the memory system
				for(bool finished = false; !finished;) {
					finished = AdvancedResponseListener<
						Request<PHYSICAL_ADDR, MAX_TRANSACTION_DATA_SIZE>,
						Response<MAX_TRANSACTION_DATA_SIZE>,
						PCIRequest<PCI_ADDR, MAX_PCI_TRANSACTION_DATA_SIZE>,
						PCIResponse<MAX_PCI_TRANSACTION_DATA_SIZE> 
						>::Send(sys_msg, *(item->out_port), item->pci_msg, slave_pci_port);
					if(!finished)
						wait(cycle_time);
				}
			} else {
				PPCIRspType pci_rsp = new(pci_rsp) PCIRspType();
				
				if(DEBUG && logger_import)
					(*logger_import) << DebugInfo
						<< sc_time_stamp().to_string()
						<< " received response to a pci request from the memory system, "
						<< " forwarding response to the PCI bus" << Endl
						<< EndDebugInfo;
				
				memcpy(pci_rsp->read_data, sys_rsp->read_data,  pci_req->size);
				item->pci_msg->SetResponse(pci_rsp);
				item->pci_msg->GetResponseEvent()->notify(SC_ZERO_TIME);
			}
		}
		pci_req_list.pop_front();
	    free_pci_req_list.push_back(item);
        if(!pci_req_list.empty()) dispatch_pci_req_ev.notify_delayed();
	}
}

template <class PHYSICAL_ADDR, 
		uint32_t MAX_TRANSACTION_DATA_SIZE,
		class PCI_ADDR,
		uint32_t MAX_PCI_TRANSACTION_DATA_SIZE, bool DEBUG>
void
MPC107<PHYSICAL_ADDR, MAX_TRANSACTION_DATA_SIZE,
	PCI_ADDR, MAX_PCI_TRANSACTION_DATA_SIZE, DEBUG>::
ResponseReceived(const PMsgType &msg, 
	sc_port<TlmSendIf<ReqType, RspType> > &port,
	const PPCIMsgType &orig_pci_msg,
	sc_export<TlmSendIf<PCIReqType, PCIRspType> > &who_pci_port) {
	PPCIRspType pci_rsp = new(pci_rsp) PCIRspType();
	const PPCIReqType &pci_req = orig_pci_msg->GetRequest();
	const PRspType &rsp = msg->GetResponse();
	
	if(DEBUG && logger_import)
		(*logger_import) << DebugInfo
			<< sc_time_stamp().to_string()
			<< " received response to a pci request from the memory system, "
			<< " forwarding response to the PCI bus" << Endl
			<< EndDebugInfo;
	
	memcpy(pci_rsp->read_data, rsp->read_data,  pci_req->size);
	orig_pci_msg->SetResponse(pci_rsp);
	orig_pci_msg->GetResponseEvent()->notify(SC_ZERO_TIME);
}

template <class PHYSICAL_ADDR, 
		uint32_t MAX_TRANSACTION_DATA_SIZE,
		class PCI_ADDR,
		uint32_t MAX_PCI_TRANSACTION_DATA_SIZE, bool DEBUG>
void
MPC107<PHYSICAL_ADDR, MAX_TRANSACTION_DATA_SIZE,
	PCI_ADDR, MAX_PCI_TRANSACTION_DATA_SIZE, DEBUG>::
DMARead(PHYSICAL_ADDR addr,
		unsigned int size,
		unsigned int channel) {
	/* create the memory request and put it in the dma request list
	 *   to local memory */
	PReqType req;
	PMsgType msg;
	PDMAMsgType dma_msg;
	
	req = new(req) ReqType();
	req->type = ReqType::READ;
	req->global = false;
	req->addr = addr;
	req->size = size;
	
	msg = new (msg) MsgType(req);
	
	dma_msg = new(dma_msg) DMAMsgType();
	dma_msg->msg = msg;
	dma_msg->channel = channel;
	
	dma_req_list.push_back(dma_msg);
	if(dma_req_list.size() == 1) {
		dispatchDMALocalMemoryAccessEvent.notify_delayed();
	}
}

template <class PHYSICAL_ADDR, 
		uint32_t MAX_TRANSACTION_DATA_SIZE,
		class PCI_ADDR,
		uint32_t MAX_PCI_TRANSACTION_DATA_SIZE, bool DEBUG>
void
MPC107<PHYSICAL_ADDR, MAX_TRANSACTION_DATA_SIZE,
	PCI_ADDR, MAX_PCI_TRANSACTION_DATA_SIZE, DEBUG>::
DMAWrite(PHYSICAL_ADDR addr,
		Pointer<uint8_t> &data,
		unsigned int size,
		unsigned int channel) {
	/* create the memory request and put it in the dma request list
	 *   to local memory */
	PReqType req;
	PMsgType msg;
	PDMAMsgType dma_msg;
	
	req = new(req) ReqType();
	req->type = ReqType::WRITE;
	req->global = false;
	req->addr = addr;
	req->size = size;
	for(unsigned int i = 0; i < size; i++) {
		req->write_data[i] = data[i];
	}
	
	msg = new (msg) MsgType(req);
	
	dma_msg = new(dma_msg) DMAMsgType();
	dma_msg->msg = msg;
	dma_msg->channel = channel;
	
	dma_req_list.push_back(dma_msg);
	if(dma_req_list.size() == 1) {
		dispatchDMALocalMemoryAccessEvent.notify_delayed();
	}
}

template <class PHYSICAL_ADDR, 
		uint32_t MAX_TRANSACTION_DATA_SIZE,
		class PCI_ADDR,
		uint32_t MAX_PCI_TRANSACTION_DATA_SIZE, bool DEBUG>
void
MPC107<PHYSICAL_ADDR, MAX_TRANSACTION_DATA_SIZE,
	PCI_ADDR, MAX_PCI_TRANSACTION_DATA_SIZE, DEBUG>::
DispatchDMALocalMemoryAccess() {
	sc_event rsp_event;
	bool read = false;
	
	while(1) {
		wait(dispatchDMALocalMemoryAccessEvent);
		
		/* get the message waiting in the front of the list */
		PDMAMsgType dma_msg = dma_req_list.front();
		PMsgType msg = dma_msg->msg;
		
		/* check if it is a read, if so put an event in the message
		 *   to be able to wait for the response */
		read = msg->req->type == ReqType::READ;
		if(read) {
			msg->PushResponseEvent(rsp_event);			
		}
		
		/* send the message */
		while(!Send(msg)) {
			wait(cycle_time);
		}
		
		/* if the message was a read wait for the response, and signal
		 *   it to the DMA */
		if(read) {
			wait(rsp_event);

			Pointer<uint8_t> data = new(data) uint8_t[msg->req->size];
			for(unsigned int i = 0; i < msg->req->size; i++) {
				data[i] = msg->rsp->read_data[i];
			}
			dma.ReadReceived(dma_msg->channel,
				data,
				msg->req->size);
		}
		
		
		/* if still messages in the list, rise the event to be 
		 *   executed again */
		if(dma_req_list.size() != 0)
			dispatchDMALocalMemoryAccessEvent.notify_delayed();
	}
}

template <class PHYSICAL_ADDR, 
		uint32_t MAX_TRANSACTION_DATA_SIZE,
		class PCI_ADDR,
		uint32_t MAX_PCI_TRANSACTION_DATA_SIZE, bool DEBUG>
void
MPC107<PHYSICAL_ADDR, MAX_TRANSACTION_DATA_SIZE,
	PCI_ADDR, MAX_PCI_TRANSACTION_DATA_SIZE, DEBUG>::
DMAPCIRead(PHYSICAL_ADDR addr,
		 unsigned int size,
		 unsigned int channel) {
	/* create the pci request and put it in th dma request list
	 *   to the pci memory */
	PPCIReqType req;
	PPCIMsgType msg;
	PDMAPCIMsgType dma_msg;
	
	req = new(req) PCIReqType();
	req->type = full_system::pci::TT_READ;
	req->addr = addr;
	req->size = size;
	
	msg = new(msg) PCIMsgType(req);
	
	dma_msg = new(dma_msg) DMAPCIMsgType();
	dma_msg->msg = msg;
	dma_msg->channel = channel;
	
	dma_pci_req_list.push_back(dma_msg);
	if(dma_pci_req_list.size() == 1) {
		dispatchDMAPCIAccessEvent.notify_delayed();
	}
}

template <class PHYSICAL_ADDR, 
		uint32_t MAX_TRANSACTION_DATA_SIZE,
		class PCI_ADDR,
		uint32_t MAX_PCI_TRANSACTION_DATA_SIZE, bool DEBUG>
void
MPC107<PHYSICAL_ADDR, MAX_TRANSACTION_DATA_SIZE,
	PCI_ADDR, MAX_PCI_TRANSACTION_DATA_SIZE, DEBUG>::
DMAPCIWrite(PHYSICAL_ADDR addr,
		Pointer<uint8_t> &data,
		unsigned int size,
		unsigned int channel) {
	/* create the pci request and put it in th dma request list
	 *   to the pci memory */
	PPCIReqType req;
	PPCIMsgType msg;
	PDMAPCIMsgType dma_msg;
	
	req = new(req) PCIReqType();
	req->type = full_system::pci::TT_WRITE;
	req->addr = addr;
	req->size = size;
	for(unsigned int i = 0; i < size; i++) {
		req->write_data[i] = data[i];
	}
	
	msg = new(msg) PCIMsgType(req);
	
	dma_msg = new(dma_msg) DMAPCIMsgType();
	dma_msg->msg = msg;
	dma_msg->channel = channel;
	
	dma_pci_req_list.push_back(dma_msg);
	if(dma_pci_req_list.size() == 1) {
		dispatchDMAPCIAccessEvent.notify_delayed();
	}
}

template <class PHYSICAL_ADDR, 
		uint32_t MAX_TRANSACTION_DATA_SIZE,
		class PCI_ADDR,
		uint32_t MAX_PCI_TRANSACTION_DATA_SIZE, bool DEBUG>
void
MPC107<PHYSICAL_ADDR, MAX_TRANSACTION_DATA_SIZE,
	PCI_ADDR, MAX_PCI_TRANSACTION_DATA_SIZE, DEBUG>::
DispatchDMAPCIAccess() {
	sc_event rsp_event;
	bool read = false;
	
	while(1) {
		wait(dispatchDMALocalMemoryAccessEvent);
		
		/* get the message waiting in the front of the list */
		PDMAPCIMsgType dma_msg = dma_pci_req_list.front();
		PPCIMsgType msg = dma_msg->msg;
		
		/* check if it is a read, if so put an event in the message
		 *   to be able to wait for the response */
		read = msg->req->type == full_system::pci::TT_READ;
		if(read) {
			msg->PushResponseEvent(rsp_event);
		}
		
		/* send the message */
//		while(!Send(msg)) {
//			wait(cycle_time);
//		}
		
		/* if the message was a read wait for the response, and signal
		 *   it to the DMA */
//		if(read) {
//			wait(rsp_event);
//
//			Pointer<uint8_t> data = new(uint8_t) data[msg->req->size];
//			for(unsigned int i = 0; i < msg->req->size; i++) {
//				data[i] = msg->rsp->read_data[i];
//			}
//			dma->ReadReceived(dma_msg->channel,
//				data,
//				msg->req->size);
//		}
		
		
		/* if still messages in the list, rise the event to be 
		 *   executed again */
		if(dma_req_list.size() != 0)
			dispatchDMALocalMemoryAccessEvent.notify_delayed();
	}
}

#undef LOCATION

} // end of namespace mpc107
} // end of namespace chipset
} // end of namespace tlm
} // end of namespace full_system
