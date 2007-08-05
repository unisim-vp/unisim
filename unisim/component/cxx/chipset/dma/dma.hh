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
 
#ifndef __FS_CHIPSETS_MPC107_DMA_DMA_HH__
#define __FS_CHIPSETS_MPC107_DMA_DMA_HH__

#include <inttypes.h>

#include "utils/services/service.hh"
#include "memories/memory_interface.hh"
#include "plugins/logger/logger_interface.hh"
#include "chipsets/mpc107/dma/buffer.hh"
#include "chipsets/mpc107/dma/register.hh"
#include "chipsets/mpc107/dma/dma_client_interface.hh"

namespace full_system {
namespace chipsets {
namespace mpc107 {
namespace dma {
	
using full_system::utils::services::Object;
using full_system::utils::services::Service;
using full_system::utils::services::Client;
using full_system::utils::services::Parameter;
using full_system::utils::services::ServiceExport;
using full_system::utils::services::ServiceImport;
using full_system::memories::MemoryInterface;
using namespace full_system::plugins::logger;
// using full_system::chipsets::mpc107::MPC107;
//using full_system::chipsets::mpc107::dma::Registers;

template <class PHYSICAL_ADDR, 
	bool DEBUG = false>
class DMA :
		//	public Service<MemoryInterface<PHYSICAL_ADDR> >,
	public Client<LoggerInterface> {
public:
	static const unsigned int NUM_IRQS = 4;
	static const uint32_t ADDR_MASK = (uint32_t)0x0fffff;
		
	/* logger service */
	ServiceImport<LoggerInterface> dma_logger_import;

	//	/* memory service */
	//	ServiceExport<MemoryInterface<PHYSICAL_ADDR> > memory_export;

	DMA(DMAClientInterface<PHYSICAL_ADDR> *_client, 
		const char *name, Object *parent = 0);
	virtual ~DMA();

	/* Initialization methods of the service */
	virtual bool Setup();
	virtual void Reset();
		
	/* Methods to implement for 
	 *   Service<MemoryInterface<PHYSICAL_ADDR> > */
	//	virtual void Reset();
	//	virtual bool ReadMemory(PHYSICAL_ADDR addr, 
	//													void *buffer, 
	//													uint32_t size);
	//	virtual bool WriteMemory(PHYSICAL_ADDR addr, 
	//													 const void *buffer, 
	//													 uint32_t size);

	uint32_t ReadRegister(PHYSICAL_ADDR addr, 
						uint32_t size,
						bool pci_access = false);
	void WriteRegister(PHYSICAL_ADDR addr, 
					uint32_t data, 
					uint32_t size,
					bool pci_access = false);

	void ReadReceived(unsigned int channel,
					Pointer<uint8_t> &data,
					unsigned int size);

private:
	Registers reg;
	Buffer<PHYSICAL_ADDR> buffer[2]; // one buffer for each of the dma channels
	DMAClientInterface<PHYSICAL_ADDR> *client;
	PHYSICAL_ADDR MaskAddress(PHYSICAL_ADDR addr, 
														bool pci_access);
	uint32_t CheckReservedBits(string &reg_name,
														 uint32_t data,
														 uint32_t mask);
	void LogRegFieldInfo(string &field_name,
											 uint32_t data,
											 uint32_t mask);
	uint32_t GetDMR(unsigned int channel, bool pci_access);
	uint32_t GetDSR(unsigned int channel, bool pci_access);
	uint32_t GetCDAR(unsigned int channel, bool pci_access);
	uint32_t GetSAR(unsigned int channel, bool pci_access);
	uint32_t GetDAR(unsigned int channel, bool pci_access);
	uint32_t GetBCR(unsigned int channel, bool pci_access);
	uint32_t GetNDAR(unsigned int channel, bool pci_access);
	void SetDMR(unsigned int channel, uint32_t data, bool pci_access);
	void SetDSR(unsigned int channel, uint32_t data, bool pci_access);
	void SetCDAR(unsigned int channel, uint32_t data, bool pci_access);
	void SetSAR(unsigned int channel, uint32_t data, bool pci_access);
	void SetDAR(unsigned int channel, uint32_t data, bool pci_access);
	void SetBCR(unsigned int channel, uint32_t data, bool pci_access);
	void SetNDAR(unsigned int channel, uint32_t data, bool pci_access);

	void ChannelStart(unsigned int channel);
	void ChannelStop(unsigned int channel);
	void SendRead(unsigned int channel);
	void SendWrite(unsigned int channel, bool last);
};

} // end of dma namespace
} // end of mpc107 namespace	
} // end of chipsets namespace
} // end of full_system namespace

#endif /* __FS_CHIPSETS_MPC107_DMA_DMA_HH__ */
