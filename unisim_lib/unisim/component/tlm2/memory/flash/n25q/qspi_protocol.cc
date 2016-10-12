/*
 *  Copyright (c) 2016,
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
 
#include <unisim/component/tlm2/memory/flash/n25q/qspi_protocol.hh>

namespace unisim {
namespace component {
namespace tlm2 {
namespace memory {
namespace flash {
namespace n25q {

tlm_qspi_extension::tlm_qspi_extension()
	: qspi_cmd(TLM_QSPI_UNKNOWN_COMMAND)
	, cycle_period(sc_core::SC_ZERO_TIME)
	, xip_confirmation_bit(false)
{
}

tlm_qspi_extension::tlm_qspi_extension(tlm_qspi_command _qspi_cmd, const sc_core::sc_time& _cycle_period, bool _xip_confirmation_bit)
	: qspi_cmd(_qspi_cmd)
	, cycle_period(_cycle_period)
	, xip_confirmation_bit(_xip_confirmation_bit)
{
}

tlm::tlm_extension_base *tlm_qspi_extension::clone() const
{
	tlm_qspi_extension *new_qspi_extension = new tlm_qspi_extension(qspi_cmd, cycle_period, xip_confirmation_bit);
	
	return new_qspi_extension;
}

void tlm_qspi_extension::copy_from(const tlm::tlm_extension_base& ext)
{
	const tlm_qspi_extension& from = static_cast<const tlm_qspi_extension&>(ext);
	
	qspi_cmd = from.qspi_cmd;
	cycle_period = from.cycle_period;
	xip_confirmation_bit = from.xip_confirmation_bit;
}

} // end of namespace n25q
} // end of namespace flash
} // end of namespace memory
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim
