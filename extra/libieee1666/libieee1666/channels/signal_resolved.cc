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

#include "channels/signal_resolved.h"

namespace sc_core {

///////////////////////////// sc_signal_resolved ////////////////////////////////

sc_signal_resolved::sc_signal_resolved()
	: sc_signal<sc_dt::sc_logic, SC_MANY_WRITERS>()
	, writers()
	, value_counts()
{
}

sc_signal_resolved::sc_signal_resolved(const char *_name)
	: sc_signal<sc_dt::sc_logic, SC_MANY_WRITERS>(_name)
	, writers()
	, value_counts()
{
}

sc_signal_resolved::~sc_signal_resolved()
{
}

void sc_signal_resolved::register_port(sc_port_base& port, const char *if_typename)
{
	if(std::string(typeid(sc_signal_in_if<sc_dt::sc_logic>).name()).compare(if_typename) != 0)
	{
		throw std::runtime_error("sc_signal_resolved is not bound to sc_signal_in_if<sc_dt::sc_logic> nor sc_signal_inout_if<sc_dt::sc_logic>");
	}
}

void sc_signal_resolved::write(const sc_dt::sc_logic& v)
{
	sc_dt::sc_logic_value_t writer_log_value = v.value();
	sc_object *writer = sc_kernel::get_kernel()->get_current_writer();
	
	writers_t::iterator it = writers.find(writer);
	if(it == writers.end())
	{
		writers[writer] = writer_log_value;
		++value_counts[writer_log_value];
		request_update();
	}
	else
	{
		sc_dt::sc_logic_value_t& curr_writer_log_value = (*it).second;
		if(curr_writer_log_value != writer_log_value)
		{
			--value_counts[curr_writer_log_value];
			curr_writer_log_value = writer_log_value;
			++value_counts[writer_log_value];
			request_update();
		}
	}
}

sc_signal_resolved& sc_signal_resolved::operator = (const sc_dt::sc_logic& v)
{
	write(v);
	return *this;
}

sc_signal_resolved& sc_signal_resolved::operator = (const sc_signal_resolved& s)
{
	write(s.read());
	return *this;
}

const char *sc_signal_resolved::kind() const
{
	return "sc_signal_resolved";
}

void sc_signal_resolved::update()
{
	writers_t::size_type num_writers = writers.size();
	
	const sc_dt::sc_logic& resolved_value =
		(value_counts[sc_dt::Log_X]) ? sc_dt::SC_LOGIC_X :
		(((value_counts[sc_dt::Log_0] + value_counts[sc_dt::Log_Z]) == num_writers) ? sc_dt::SC_LOGIC_0 :
		(((value_counts[sc_dt::Log_1] + value_counts[sc_dt::Log_Z]) == num_writers) ? sc_dt::SC_LOGIC_1 : sc_dt::SC_LOGIC_Z));
	
	new_value = resolved_value;
	sc_signal<sc_dt::sc_logic, SC_MANY_WRITERS>::update();
}

// Disabled
sc_signal_resolved::sc_signal_resolved(const sc_signal_resolved&)
{
}

} // end of namespace sc_core
