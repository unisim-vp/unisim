/*
 *  Copyright (c) 2015-2016,
 *  Commissariat a l'Energie Atomique et aux Energies Alternatives (CEA)
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

#include <unisim/kernel/tlm2/simulator.hh>
#include <unisim/kernel/tlm2/tlm_can.hh>

namespace unisim {
namespace kernel {
namespace tlm2 {

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;

Simulator::Simulator(sc_core::sc_module_name const& name, int argc, char **argv, void (*LoadBuiltInConfig)(unisim::kernel::Simulator *simulator))
	: unisim::kernel::Simulator(argc, argv, LoadBuiltInConfig)
	, unisim::kernel::Object(name)
	, logger(*this)
	, stat_cur_sim_time("cur-sim-time", this, *const_cast<sc_core::sc_time *>(&sc_core::sc_time_stamp()), "SystemC current simulation time (as returned by sc_core::sc_time_stamp()) ")
	, global_quantum(sc_core::SC_ZERO_TIME)
	, param_global_quantum("global-quantum", this, global_quantum, "SystemC TLM-2.0 global quantum")
	, can_global_quantum(sc_core::SC_ZERO_TIME)
	, param_can_global_quantum("can-global-quantum", this, can_global_quantum, "SystemC TLM CAN global quantum")
{
	stat_cur_sim_time.SetMutable(false);
	stat_cur_sim_time.SetSerializable(false);
	param_global_quantum.SetMutable(false);
	param_can_global_quantum.SetMutable(false);
	
	tlm::tlm_global_quantum::instance().set(global_quantum);
	tlm_can_global_quantum::instance().set(can_global_quantum);
}

Simulator::~Simulator()
{
}

} // end of namespace tlm2
} // end of namespace kernel
} // end of namespace unisim
