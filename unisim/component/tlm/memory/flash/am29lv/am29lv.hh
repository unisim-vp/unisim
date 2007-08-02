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
 
#ifndef __UNISIM_COMPONENT_TLM_MEMORY_FLASH_AM29LV_AM29LV_HH__
#define __UNISIM_COMPONENT_TLM_MEMORY_FLASH_AM29LV_AM29LV_HH__

#include <systemc.h>
#include "unisim/component/tlm/message/memory.hh"
#include "unisim/kernel/tlm/tlm.hh"
#include "unisim/component/cxx/memory/flash/am29lv/am29lv.hh"

namespace unisim {
namespace component {
namespace tlm {
namespace memory {
namespace flash {
namespace am29lv {

using unisim::kernel::tlm::TlmMessage;
using unisim::kernel::tlm::TlmSendIf;
using unisim::kernel::service::Object;
using unisim::kernel::service::Parameter;
using unisim::util::garbage_collector::Pointer;
using unisim::component::cxx::memory::flash::am29lv::CMD_READ;
using unisim::component::cxx::memory::flash::am29lv::CMD_WRITE;

using unisim::component::tlm::message::MemoryRequest;
using unisim::component::tlm::message::MemoryResponse;

using unisim::service::interfaces::Logger;
using unisim::service::interfaces::operator<<;
using unisim::service::interfaces::Hex;
using unisim::service::interfaces::Dec;
using unisim::service::interfaces::Endl;
using unisim::service::interfaces::DebugInfo;
using unisim::service::interfaces::DebugWarning;
using unisim::service::interfaces::DebugError;
using unisim::service::interfaces::EndDebugInfo;
using unisim::service::interfaces::EndDebugWarning;
using unisim::service::interfaces::EndDebugError;

template <class CONFIG, uint32_t BYTESIZE, uint32_t IO_WIDTH, uint32_t MAX_TRANSACTION_DATA_SIZE>
class AM29LV :
	public sc_module,
	public unisim::component::cxx::memory::flash::am29lv::AM29LV<CONFIG, BYTESIZE, IO_WIDTH>,
	public TlmSendIf<MemoryRequest<typename CONFIG::ADDRESS, MAX_TRANSACTION_DATA_SIZE>, MemoryResponse<MAX_TRANSACTION_DATA_SIZE> >
{
public:
	typedef unisim::component::cxx::memory::flash::am29lv::AM29LV<CONFIG, BYTESIZE, IO_WIDTH> inherited;

	sc_export<TlmSendIf<MemoryRequest<typename CONFIG::ADDRESS, MAX_TRANSACTION_DATA_SIZE>, 
						MemoryResponse<MAX_TRANSACTION_DATA_SIZE> > > slave_port;

	AM29LV(const sc_module_name& name, Object *parent = 0);
	virtual ~AM29LV();
	virtual bool Setup();
	virtual bool Send(const Pointer<TlmMessage<MemoryRequest<typename CONFIG::ADDRESS, MAX_TRANSACTION_DATA_SIZE>, MemoryResponse<MAX_TRANSACTION_DATA_SIZE> > >& message);
	void Process();
private:
	sc_fifo<Pointer<TlmMessage<MemoryRequest<typename CONFIG::ADDRESS, MAX_TRANSACTION_DATA_SIZE>, MemoryResponse<MAX_TRANSACTION_DATA_SIZE> > > > input_queue;
	unsigned int frequency;
	sc_time cycle_time;
	Parameter<unsigned int> param_frequency;
};

} // end of namespace am29lv
} // end of namespace flash
} // end of namespace memory
} // end of namespace tlm
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM_MEMORY_FLASH_AM29LV_AM29LV_HH__
