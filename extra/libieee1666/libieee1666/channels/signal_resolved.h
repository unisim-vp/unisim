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

#ifndef __LIBIEEE1666_CHANNELS_SIGNAL_RESOLVED_H__
#define __LIBIEEE1666_CHANNELS_SIGNAL_RESOLVED_H__

#include "core/kernel.h"
#include "core/port.h"
#include "channels/fwd.h"
#include "channels/signal.h"
#include "data_types/bit/logic.h"
#include <map>

namespace sc_core {

///////////////////////////// sc_signal_resolved ////////////////////////////////

class sc_signal_resolved : public sc_signal<sc_dt::sc_logic, SC_MANY_WRITERS>
{
public:
	sc_signal_resolved();
	explicit sc_signal_resolved(const char *);
	virtual ~sc_signal_resolved();
	virtual void register_port(sc_port_base&, const char *);
	virtual void write(const sc_dt::sc_logic&);
	sc_signal_resolved& operator = (const sc_dt::sc_logic&);
	sc_signal_resolved& operator = (const sc_signal_resolved&);
	virtual const char* kind() const;
protected:
	virtual void update();
private:
	// Disabled
	sc_signal_resolved(const sc_signal_resolved&);
	
	void resolve(sc_dt::sc_logic_value_t);
	
	typedef std::map<sc_object *, sc_dt::sc_logic_value_t> writers_t;
	writers_t writers;
	writers_t::size_type value_counts[4];
};

} // end of namespace sc_core

#endif
