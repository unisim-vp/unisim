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

#include "channels/out_resolved.h"

namespace sc_core {

sc_out_resolved::sc_out_resolved()
	: sc_inout_resolved()
{
}

sc_out_resolved::sc_out_resolved(const char *_name)
	: sc_inout_resolved(_name)
{
}

sc_out_resolved::~sc_out_resolved()
{
}

sc_out_resolved& sc_out_resolved::operator = (const sc_dt::sc_logic& v)
{
	sc_inout_resolved::operator = (v);
	return *this;
}

sc_out_resolved& sc_out_resolved::operator = (const sc_signal_in_if<sc_dt::sc_logic>& itf)
{
	sc_inout_resolved::operator = (itf.read());
	return *this;	
}

sc_out_resolved& sc_out_resolved::operator = (const sc_port<sc_signal_in_if<sc_dt::sc_logic>, 1>& port)
{
	sc_inout_resolved::operator = (port->read());
	return *this;
}

sc_out_resolved& sc_out_resolved::operator = (const sc_port<sc_signal_inout_if<sc_dt::sc_logic>, 1>& port)
{
	sc_inout_resolved::operator = (port->read());
	return *this;
}

sc_out_resolved& sc_out_resolved::operator = (const sc_out_resolved& out_resolved)
{
	sc_inout_resolved::operator = (out_resolved->read());
	return *this;
}

const char *sc_out_resolved::kind() const
{
	return "sc_out_resolved";
}

// Disabled
sc_out_resolved::sc_out_resolved(const sc_out_resolved&)
{
}


} // end of namespace sc_core
