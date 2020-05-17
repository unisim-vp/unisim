/*
 *  Copyright (c) 2014,
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

#include "channels/inout_resolved.h"
#include "channels/signal_resolved.h"

namespace sc_core {

sc_inout_resolved::sc_inout_resolved()
	: sc_inout<sc_dt::sc_logic>()
{
}

sc_inout_resolved::sc_inout_resolved(const char *_name)
	: sc_inout<sc_dt::sc_logic>(_name)
{
}

sc_inout_resolved::~sc_inout_resolved()
{
}

void sc_inout_resolved::end_of_elaboration()
{
	sc_interface *itf = get_interface();
	
	if(!dynamic_cast<sc_signal_resolved *>(itf))
	{
		throw std::runtime_error("sc_inout_resolved is not bound to a sc_signal_resolved");
	}
}

sc_inout_resolved& sc_inout_resolved::operator = (const sc_dt::sc_logic& v)
{
	sc_inout<sc_dt::sc_logic>::operator = (v);
	return *this;
}

sc_inout_resolved& sc_inout_resolved::operator = (const sc_signal_in_if<sc_dt::sc_logic>& itf)
{
	sc_inout<sc_dt::sc_logic>::operator = (itf.read());
	return *this;	
}

sc_inout_resolved& sc_inout_resolved::operator = (const sc_port<sc_signal_in_if<sc_dt::sc_logic>, 1>& port)
{
	sc_inout<sc_dt::sc_logic>::operator = (port->read());
	return *this;	
}

sc_inout_resolved& sc_inout_resolved::operator = (const sc_port<sc_signal_inout_if<sc_dt::sc_logic>, 1>& port)
{
	sc_inout<sc_dt::sc_logic>::operator = (port->read());
	return *this;	
}

sc_inout_resolved& sc_inout_resolved::operator = (const sc_inout_resolved& inout_resolved)
{
	sc_inout<sc_dt::sc_logic>::operator = (inout_resolved->read());
	return *this;	
}

const char *sc_inout_resolved::kind() const
{
	return "sc_inout_resolved";
}

// Disabled
sc_inout_resolved::sc_inout_resolved(const sc_inout_resolved&)
{
}


} // end of namespace sc_core
