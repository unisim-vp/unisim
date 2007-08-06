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
 
#ifndef __FULLSYSTEM_CHIPSETS_MPC107_MPC107_HH__
#define __FULLSYSTEM_CHIPSETS_MPC107_MPC107_HH__

//#define DEBUG_MEMORY

/*
 * ---------> systemc connections
 * =========> service connections
 *
 *
 *                  /-----------\
 * [port            |           |   [non intrusive connections 
 *  connections]    |           |    provided by the module]
 *                  |           | 
 *   from           |           |    from  
 *  system-bus      |           |    cpu
 * (bus_port)       |     M     | (memory_export)
 * ---------------->|           |<=================
 *                  |     P     |
 *  to ram          |           |    to ram
 *  (ram_port)      |     C     |  (ram_export)
 * <----------------|           |=================>
 *                  |     1     |
 *  to rom          |           |    to rom
 *  (rom_port)      |     0     |  (rom_export)
 * <----------------|           |=================>
 *                  |     7     |
 *                  |           | 
 *  to pci bus      |           |
 *  (pci_port)      |     M     |
 * <----------------|     O     |
 *                  |     D     |
 *                  |     U     |
 *                  |     L     |
 *                  |     E     |
 *                  |           |
 *                  |           |
 *                  |           |
 *                  |           |
 *                  \-----------/
 */
 
#include <systemc.h>
#include "tlm/tlm.hh"
#include "tlm/shared_memory/snooping_bus/message.hh"
#include "tlm/pci/message.hh"
#include "utils/services/service.hh"
#include "utils/garbage_collector/garbage_collector.hh"
#include "memories/memory_interface.hh"
#include "pci/pci_interface.hh"
#include "generic/bus/bus_interface.hh"
#include "chipsets/mpc107/address_maps.hh"
#include "chipsets/mpc107/address_map_entry.hh"
#include "chipsets/mpc107/config_regs.hh"
#include "chipsets/mpc107/pci_controller.hh"
#include "chipsets/mpc107/dma/dma.hh"
#include "chipsets/mpc107/dma/dma_client_interface.hh"
#include "tlm/chipsets/mpc107/epic/epic.hh"
#include "tlm/interrupt/interrupt.hh"
#include "plugins/logger/logger_interface.hh"
#include <list>

namespace full_system {
namespace tlm {
namespace chipsets {
namespace mpc107 {
	
using namespace std;
using full_system::tlm::TlmSendIf;
using full_system::tlm::TlmMessage;
using full_system::tlm::ResponseListener;
using full_system::tlm::shared_memory::snooping_bus::Request;
using full_system::tlm::shared_memory::snooping_bus::Response;
//using full_system::tlm::shared_memory::snooping_bus::BusRequest;
//using full_system::tlm::shared_memory::snooping_bus::BusGrant;
using full_system::tlm::pci::PCIRequest;
using full_system::tlm::pci::PCIResponse;
using full_system::utils::services::Object;
using full_system::utils::services::Service;
using full_system::utils::services::Client;
using full_system::utils::services::Parameter;
using full_system::utils::garbage_collector::Pointer;
using full_system::utils::services::ServiceExport;
using full_system::utils::services::ServiceImport;
using full_system::memories::MemoryInterface;
using full_system::pci::PCIInterface;
using full_system::chipsets::mpc107::AddressMap;
using full_system::chipsets::mpc107::AddressMapEntry;
using full_system::chipsets::mpc107::PCIController;
using full_system::chipsets::mpc107::ConfigurationRegister;
using full_system::chipsets::mpc107::ConfigurationRegisters;
using full_system::chipsets::mpc107::dma::DMA;
using full_system::chipsets::mpc107::dma::DMAClientInterface;
using full_system::tlm::chipsets::mpc107::epic::EPIC;
using full_system::tlm::interrupt::InterruptRequest;
using namespace full_system::plugins::logger;
		
template <class PHYSICAL_ADDR, 
		uint32_t MAX_TRANSACTION_DATA_SIZE,
		class PCI_ADDR,
		uint32_t MAX_PCI_TRANSACTION_DATA_SIZE,
		bool DEBUG = false>
class MPC107 :
	public sc_module,
	public TlmSendIf<Request<PHYSICAL_ADDR, MAX_TRANSACTION_DATA_SIZE>, 
					 Response<MAX_TRANSACTION_DATA_SIZE> >,
	public TlmSendIf<PCIRequest<PCI_ADDR, MAX_PCI_TRANSACTION_DATA_SIZE>,
					PCIResponse<MAX_PCI_TRANSACTION_DATA_SIZE> >,
	public AdvancedResponseListener<Request<PHYSICAL_ADDR, MAX_TRANSACTION_DATA_SIZE>,
							Response<MAX_TRANSACTION_DATA_SIZE>,
							PCIRequest<PCI_ADDR, MAX_PCI_TRANSACTION_DATA_SIZE>,
							PCIResponse<MAX_PCI_TRANSACTION_DATA_SIZE> >,
	public DMAClientInterface<PHYSICAL_ADDR>,
//	public DMA<PHYSICAL_ADDR, DEBUG>,
	public Service<MemoryInterface<PHYSICAL_ADDR> >,
	public Client<MemoryInterface<PHYSICAL_ADDR> >,
	public Client<PCIInterface<PCI_ADDR> >,
	public Client<LoggerInterface> {
//	public Client<DeviceInterface<PHYSICAL_ADDR> > {
private:
	typedef Request<PHYSICAL_ADDR, MAX_TRANSACTION_DATA_SIZE> ReqType;
	typedef Pointer<ReqType> PReqType;
	typedef Response<MAX_TRANSACTION_DATA_SIZE> RspType;
	typedef Pointer<RspType> PRspType;
	typedef TlmMessage<ReqType, RspType> MsgType;
	typedef Pointer<MsgType> PMsgType;
	typedef PCIRequest<PCI_ADDR, MAX_PCI_TRANSACTION_DATA_SIZE> PCIReqType;
	typedef Pointer<PCIReqType> PPCIReqType;
	typedef PCIResponse<MAX_PCI_TRANSACTION_DATA_SIZE> PCIRspType;
	typedef Pointer<PCIRspType> PPCIRspType;
	typedef TlmMessage<PCIReqType, PCIRspType> PCIMsgType;
	typedef Pointer<PCIMsgType> PPCIMsgType;

	typedef InterruptRequest IntType;
	typedef Pointer<InterruptRequest> PIntType;
public:
	SC_HAS_PROCESS(MPC107);

	/* Module ports declaration */
	/** Input port for incomming requests from the system bus */
	sc_export<TlmSendIf<ReqType, RspType> > bus_inport;
//	/** 
//	 * Output request port to request system bus usage 
//	 * (needed to inform processors of pci requests to the memory) */
//	sc_port<TlmSendIf<BusRequest, BusGrant> > br_outport;
	/** 
	 * Output port to send transactions to the system bus 
	 * (needed to inform of pci requests to the memory) */
	sc_port<TlmSendIf<ReqType, RspType> > bus_outport;
	/** Input port to listen for pci requests */ 
	sc_export<TlmSendIf<PCIReqType, PCIRspType> > slave_pci_port;
	/** Output port to send memory requests to the ram */
	sc_port<TlmSendIf<ReqType, RspType> > ram_port;
	/** Output port to send memory requests to the rom */
	sc_port<TlmSendIf<ReqType, RspType> > rom_port;
	/** Output port to send memory requests to the extended ram */
	sc_port<TlmSendIf<ReqType, RspType> > erom_port;
	/** Output port to send pci requests to devices (probably through a bus) */
	sc_port<TlmSendIf<PCIReqType, PCIRspType> > pci_port;
	/** Input port for the incomming interruptions
	 * (to be directly attached to the EPIC) */
	sc_export<TlmSendIf<IntType> > *irq_inport[full_system::tlm::chipsets::mpc107::epic::EPIC<PHYSICAL_ADDR, DEBUG>::NUM_IRQS];
	/** output port for the outgoing interruptions 
	 * (to be directly attached to the EPIC) */
	sc_port<TlmSendIf<IntType> > int_outport;
	/** output port for the outgoing soft reset interruption
	 * (to be directly attached to the EPIC) */
	sc_port<TlmSendIf<IntType> > soft_reset_outport;
		
	/* Service/client exports/imports declaration */
	ServiceExport<MemoryInterface<PHYSICAL_ADDR> > memory_export;
	ServiceImport<MemoryInterface<PHYSICAL_ADDR> > ram_import;
	ServiceImport<MemoryInterface<PHYSICAL_ADDR> > rom_import;
	ServiceImport<MemoryInterface<PHYSICAL_ADDR> > erom_import;
	ServiceImport<PCIInterface<PCI_ADDR> > pci_import;
	/* logger service */
	ServiceImport<LoggerInterface> logger_import;
	ServiceImport<LoggerInterface> pci_logger_import;
	ServiceImport<LoggerInterface> addr_map_logger_import;
	ServiceImport<LoggerInterface> epic_logger_import;
	ServiceImport<LoggerInterface> epic_reg_logger_import;
		
	MPC107(const sc_module_name &name, Object *parent = 0);
	virtual ~MPC107();
	
	/* Initialization methods of the service */
	virtual bool Setup();
		
	/* Methods to implement for Service<MemoryInterface<PHYSICAL_ADDR> > */
	virtual void Reset();
	virtual bool ReadMemory(PHYSICAL_ADDR addr, void *buffer, uint32_t size);
	virtual bool WriteMemory(PHYSICAL_ADDR addr, const void *buffer, uint32_t size);

	/* Interface of the MPC107 to the local bus */
	virtual bool Send(const PMsgType &message);
	/* Interface of the MPC107 to the PCI bus (MPC107 as slave device) */
	virtual bool Send(const PPCIMsgType &message);
	
	/**
	 * Method required by ResponseListener.
	 * Called each time a request using the ResponseListener Send method receives a response. In this
	 *   case is for requests comming through the PCI bus that must be send to the memory system. 
	 * 
	 * @param msg the message to send
	 * @param port the port which should be used to send the message
	 * @param orig_pci_msg the original pci message
	 * @param who_pci_port the port which received the original pci message
	 */
	void ResponseReceived(const PMsgType &msg, 
						sc_port<TlmSendIf<ReqType, RspType> > &port,
						const PPCIMsgType &orig_pci_msg,
						sc_export<TlmSendIf<PCIReqType, PCIRspType> > &who_pci_port);
	
	/* Methods required by the DMA controller to handle PCI and local memory space accesses */
	virtual void DMARead(PHYSICAL_ADDR addr,
		unsigned int size,
		unsigned int channel);
	virtual void DMAWrite(PHYSICAL_ADDR addr,
		Pointer<uint8_t> &data,
		unsigned int size,
		unsigned int channel);
	virtual void DMAPCIRead(PHYSICAL_ADDR addr,
		unsigned int size,
		unsigned int channel);
	virtual void DMAPCIWrite(PHYSICAL_ADDR addr,
		Pointer<uint8_t> &data,
		unsigned int size,
		unsigned int channel);

private:
	/** DMA channels */
	DMA<PHYSICAL_ADDR, DEBUG> dma;
	class DMAMsgType {
	public:
		PMsgType msg;
		unsigned int channel;
	};
	class DMAPCIMsgType {
	public:
		PPCIMsgType msg;
		unsigned int channel;
	};
	typedef Pointer<DMAMsgType> PDMAMsgType;
	typedef Pointer<DMAPCIMsgType> PDMAPCIMsgType;
	/* list of DMA local memory requests */
	list<PDMAMsgType> dma_req_list;
	/* list of DMA PCI requests */
	list<PDMAPCIMsgType> dma_pci_req_list;
	/* Threads to handle DMA requests */
	void DispatchDMALocalMemoryAccess();
	void DispatchDMAPCIAccess();
	/* Events to handle DMA threads */
	sc_event dispatchDMALocalMemoryAccessEvent;
	sc_event dispatchDMAPCIAccessEvent;
	
	/**
	 * EPIC Interrupt controller */
	EPIC<PHYSICAL_ADDR, MAX_TRANSACTION_DATA_SIZE, DEBUG> epic;
	/** 
	 * Output port to send transactions to the EPIC 
	 * (this is a private, not visible port ) */
	sc_port<TlmSendIf<ReqType, RspType> > epic_outport;
	/**
	 * Output port to notify the sdram clock (the sdram cycle time) 
	 *   to internal components, i.e., EPIC */
	sc_out<uint64_t> sdram_outport;
	/**
	 * Signal to connect the sdram_outport to the EPIC sdram_inport
	 */
	sc_signal<uint64_t> sdram_to_epic_sig;
	
	/* the configuration registers */
	ConfigurationRegisters config_regs;

	/* the pci controller class that handles request to the pci bus */
	PCIController<PHYSICAL_ADDR, MAX_TRANSACTION_DATA_SIZE,
				PCI_ADDR, MAX_PCI_TRANSACTION_DATA_SIZE, DEBUG> pci_controller;
	/* list of pci requests generated by the system bus */
	list<PPCIMsgType> bus_pci_req_list;
	/* list of the requests queued for the pci that need a response */
	list<PMsgType> req_list;
	/* event to control the dispatch of pci requests */
	sc_event pci_dispatch_event;
	/* process to dispatch pending pci requests */
	void PCIDispatch();
	/* process to dispatch pending local memory requests */
	void LocalMemoryDispatch();

	/* CONFIG_ADDR register */
	uint32_t config_addr;

	static const PHYSICAL_ADDR local_memory_orig = 0x0; // local memory base address
	AddressMap<DEBUG> addr_map;
	/* Reset values */
	/** Indicates if the mpc107 is running in host mode (true) or agent mode (false) */
	bool host_mode;
	/** Indicates if the address map to use: true = A address map, false = B address map */
	bool a_address_map;
	/** 
	 * Determines the bit bus size of rom chip 2 and 3, and DRAM: true = 32bits, false = 64bits.
	 * Default: true (32 bits) */
	bool memory_32bit_data_bus_size;
	/** Determines if the rom chip 0 has a data bus size of 8bits or the same than the DRAM */
	bool rom0_8bit_data_bus_size;
	/** Determines if the rom chip 1 has a data bus size of 8bits or the same than the DRAM */
	bool rom1_8bit_data_bus_size;
	
	unsigned int frequency;
	uint64_t sdram_cycle_time;
	sc_time cycle_time;
	
	/* Parameters for reset values */
	Parameter<bool> param_host_mode;
	Parameter<bool> param_a_address_map;
	Parameter<bool> param_memory_32bit_data_bus_size;
	Parameter<bool> param_rom0_8bit_data_bus_size;
	Parameter<bool> param_rom1_8bit_data_bus_size;
	Parameter<unsigned int> param_frequency;
	Parameter<uint64_t> param_sdram_cycle_time;
	
	/** 
 	 * This method simply queues messages coming from PCI to be send to the memory system. 
	 *   It notifies the dispatcher if necessary.
 	 * 
	 * @param pci_msg   the pci message to send
	 * @param out_port  the memory port that should be used
	 * 
	 * @return true on succes, false otherwise
	 */
	bool SendPCItoMemory(const PPCIMsgType &pci_msg, sc_port<TlmSendIf<ReqType, RspType> > &out_port);

	class PCItoMemoryReqType {
	public:
		PPCIMsgType pci_msg;
		sc_port<TlmSendIf<ReqType, RspType> > *out_port;
	};
	/** list of pci requests to send to the memory system */
	list<PCItoMemoryReqType *> pci_req_list;
	/** list of free pci requests to send to the memory system */
	list<PCItoMemoryReqType *> free_pci_req_list;
	/**
	 * Thread to handle incomming pci requests that need to be send to the
	 * memory system. The PowerPC architecture requires that this requests is
	 * snooped by all the processors, so it will be sent to all the processors
	 * and memory system (if necessary).
	 */
	void DispatchPCIReq();
	/** even to handle the DispatchPCIReq thread */
	sc_event dispatch_pci_req_ev;

	/**
	 * Function to show debug information from a bus request.
	 * 
	 * @param req  the pci message to visualize
	 */
	void DEBUG_BUS_REQ(const PReqType &req);
	/**
	 * Function to show debug information from a pci request.
	 * 
	 * @param req  the pci message to visualize
	 */
	void DEBUG_PCI_REQ(const PPCIReqType &req);
};

} // end of namespace mpc107
} // end of namespace chipset
} // end of namespace tlm
} // end of namespace full_system

//#undef DEBUG_MEMORY

#endif // __FULLSYSTEM_CHIPSETS_MPC107_MPC107_HH__
