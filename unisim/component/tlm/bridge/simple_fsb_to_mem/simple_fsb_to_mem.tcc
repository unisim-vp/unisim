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
 
#ifndef __UNISIM_COMPONENT_TLM_BRIDGE_SIMPLEFSBTOMEM_SIMPLEFSBTOMEM_HH__
#define __UNISIM_COMPONENT_TLM_BRIDGE_SIMPLEFSBTOMEM_SIMPLEFSBTOMEM_HH__

namespace unisim {
namespace component {
namespace tlm {
namespace bridge {
namespace simple_fsb_to_mem {

#define LOCATION Function << __FUNCTION__ << File << __FILE__ << Line << __LINE__

template <class CONFIG>
SimpleFSBToMemory ::
SimpleFSBToMemory(const sc_module_name& name, Object *parent = 0) :
	Object(name, parent),
	sc_module(name),
	Service<Memory<fsb_address_t> >(name, parent),
	Client<Memory<mem_address_t> >(name, parent),
	Client<Logger>(name, parent),
	master_port("master-port"),
	slave_port("slave-port"),
	memory_export("memory-export", this),
	memory_import("memory-import", this),
	logger_import("logger-import", this),
	verbose_all(false),
	param_verbose_all("verbose_all", this, verbose_all),
	fsb_cycle_sctime(0),
	fsb_cycle_time(0),
	param_fsb_cycle_time("fsb_cycle_time", this, fsb_cycle_time),
	mem_cycle_sctime(0),
	mem_cycle_time(0),
	param_mem_cycle_time("mem_cycle_time", this, mem_cycle_time),
	buffer("buffer", CONFIG::BUFFER_SIZE) {
	slave_port(this);
	
	SC_THREAD(Dispatch);
	
	SetupDependsOn(memory_import);
	SetupDependsOn(logger_import);
}

#undef LOCATION

} // end of namespace simple_fsb_to_mem
} // end of namespace bridge
} // end of namespace tlm
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM_BRIDGE_SIMPLEFSBTOMEM_SIMPLEFSBTOMEM_HH__
