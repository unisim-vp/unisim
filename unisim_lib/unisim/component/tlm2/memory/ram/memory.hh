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
 
#ifndef __UNISIM_COMPONENT_TLM2_MEMORY_RAM_MEMORY_HH__
#define __UNISIM_COMPONENT_TLM2_MEMORY_RAM_MEMORY_HH__

#include <systemc.h>
#include "unisim/kernel/tlm2/tlm.hh"
#include <tlm_utils/tlm_quantumkeeper.h>
#include <tlm_utils/peq_with_get.h>
#include "unisim/component/cxx/memory/ram/memory.hh"
#include "unisim/kernel/logger/logger.hh"
#include <inttypes.h>

namespace unisim {
namespace component {
namespace tlm2 {
namespace memory {
namespace ram {

using unisim::kernel::tlm2::PayloadFabric;
using unisim::kernel::service::Object;
using unisim::kernel::service::Client;
using unisim::kernel::service::Parameter;
using unisim::kernel::logger::Logger;

const unsigned int DEFAULT_BUSWIDTH = 32; // 32-bit bus
const uint32_t DEFAULT_PAGE_SIZE = 1024 * 1024; // 1 MB page size (implementation detail)
const bool DEFAULT_DEBUG = false; // no debug

/**
 * Memory module for the simple bus.
 * It implements a tlm2 module using the tlm_generic_payload and (as it inherits from the memory service) the 
 *   service methods (Read and WriteMemory).
 */
template <unsigned int BUSWIDTH = DEFAULT_BUSWIDTH, uint32_t PAGE_SIZE = DEFAULT_PAGE_SIZE, bool DEBUG = DEFAULT_DEBUG>
class Memory :
	public sc_module,
	public unisim::component::cxx::memory::ram::Memory<uint64_t, PAGE_SIZE>,
	public tlm::tlm_fw_transport_if<>
{
public:
	typedef unisim::component::cxx::memory::ram::Memory<uint64_t, PAGE_SIZE> inherited;
	/**
	 * TLM2 Target socket
	 */
	tlm::tlm_target_socket<> slave_sock;

	/**
	 * Constructor.
	 * 
	 * @param name the name of the module
	 * @param parent the parent service
	 */
	Memory(const sc_module_name& name, Object *parent = 0);
	/**
	 * Destructor
	 */
	virtual ~Memory();

	/* Service methods */
	/** Setup
	 * Initializes the service interface. */
	virtual bool Setup();
	
	/**
	 * TLM2 Slave methods
	 */
	virtual bool get_direct_mem_ptr(tlm::tlm_generic_payload& payload, tlm::tlm_dmi& dmi_data);
	virtual unsigned int transport_dbg(tlm::tlm_generic_payload& payload);
	virtual tlm::tlm_sync_enum nb_transport_fw(tlm::tlm_generic_payload& payload, tlm::tlm_phase& phase, sc_core::sc_time& t);
	virtual void b_transport(tlm::tlm_generic_payload& payload, sc_core::sc_time& t);
	
private:
	/**
	 * Check the verbosity
	 */
	inline bool IsVerbose() { return (DEBUG && verbose); }

	/** Logger */
	Logger logger;
	/** Verbosity */
	bool verbose;
	/** The cycle time in ps */
	unsigned int cycle_time;
	/** The frequency in sc_time format */
	sc_time cycle_sctime;
	/** The parameter to set frequency */
	Parameter<unsigned int> param_cycle_time;
	/** The parameter to set the verbosity */
	Parameter<bool> param_verbose;
};

} // end of namespace ram
} // end of namespace memory
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_MEMORY_RAM_MEMORY_HH__ 
