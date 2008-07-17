/*
 *  Copyright (c) 2008,
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
 
#include <systemc.h>
#include <tlm.h>
#include <inttypes.h>

namespace unisim {
namespace component {
namespace tlm2 {
namespace bus {
namespace simple_router {

class mapping_type {
public:
	uint64_t range_start;
	uint64_t range_end;
	unsigned int output_port;
};

class Config {
public:
	static const unsigned int BUS_WIDTH = 32;
	typedef tlm::tlm_base_protocol_types TYPES;
	static const bool DEBUG = false;
};

template<typename CONFIG = Config>
class Router :
	public unisim::kernel::service::Object,
	public sc_module {
private:
	typedef unisim::kernel::service::Object Object;
	static const unsigned int BUS_WIDTH = CONFIG::BUS_WIDTH;
	typedef typename CONFIG::TYPES TYPES;
	static const bool DEBUG = CONFIG::DEBUG;

public:
	SC_HAS_PROCESS(Router);
	Router(const sc_module_name &name, Object *parent = 0);
	~Router();

	virtual bool Setup();

private:

};

} // end of namespace simple_router
} // end of namespace bus
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

