/*
 *  Copyright (c) 2016,
 *  Commissariat a l'Energie Atomique (CEA)
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without 
 *  modification, are permitted provided that the following conditions are met:
 *
 *   - Redistributions of source code must retain the above copyright notice, 
 *     this list of conditions and the following disclaimer.
 *
 *   - Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 *   - Neither the name of CEA nor the names of its contributors may be used to
 *     endorse or promote products derived from this software without specific 
 *     prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED.
 *  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY
 *  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF 
 *  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: Yves Lhuillier (yves.lhuillier@cea.fr)
 */
 
#ifndef __UNISIM_COMPONENT_TLM2_PROCESSOR_ARM_CORTEX_A53_CPU_HH__
#define __UNISIM_COMPONENT_TLM2_PROCESSOR_ARM_CORTEX_A53_CPU_HH__

#include <systemc.h>
#include <tlm.h>
#include <unisim/component/cxx/processor/arm/vmsav8/cpu.hh>
#include <unisim/kernel/tlm2/tlm.hh>
#include <inttypes.h>

namespace unisim {
namespace component {
namespace tlm2 {
namespace processor {
namespace arm {
namespace cortex_a53 {

struct CPU
  : public sc_module
  , public tlm::tlm_bw_transport_if<>
  , public unisim::component::cxx::processor::arm::vmsav8::CPU
{
  typedef tlm::tlm_base_protocol_types::tlm_payload_type  transaction_type;
  typedef tlm::tlm_base_protocol_types::tlm_phase_type    phase_type;
  typedef tlm::tlm_sync_enum     sync_enum_type;
	
  typedef unisim::component::cxx::processor::arm::vmsav7::CPU PCPU;
  // typedef PCPU::CP15CPU CP15CPU;
  // typedef PCPU::CP15Reg CP15Reg;

  /**************************************************************************
   * Port to the bus and its virtual methods to handle                START *
   *   incomming calls.                                                     *
   **************************************************************************/

  // Master port to the bus port
  tlm::tlm_initiator_socket<32> master_socket;
	
  void Run();

};

} // end of namespace cortex_a53
} // end of namespace arm
} // end of namespace processor
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_PROCESSOR_ARM_CORTEX_A53_CPU_HH__
