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
 
#ifndef __UNISIM_COMPONENT_TLM2_BUS_SIMPLE_ROUTER_HH__
#define __UNISIM_COMPONENT_TLM2_BUS_SIMPLE_ROUTER_HH__

#include <systemc.h>
#include <tlm.h>
#include <tlm_utils/multi_passthrough_initiator_socket.h>
#include <tlm_utils/multi_passthrough_target_socket.h>
#include <inttypes.h>
#include "unisim/kernel/service/service.hh"
#include "unisim/kernel/logger/logger.hh"

namespace unisim {
namespace component {
namespace tlm2 {
namespace bus {
namespace simple_router {

class Mapping {
public:
	bool used;
	uint64_t range_start;
	uint64_t range_end;
	unsigned int output_port;

	Mapping() {
		used = false;
		range_start = 0;
		range_end = 0;
		output_port = 0;
	}
};

class Config {
public:
	static const unsigned int MAX_NUM_MAPPINGS = 10;
	static const unsigned int BUSWIDTH = 32;
	typedef tlm::tlm_base_protocol_types TYPES;
	static const bool DEBUG = false;
};

class DebugConfig {
public:
	static const unsigned int MAX_NUM_MAPPINGS = 10;
	static const unsigned int BUSWIDTH = 32;
	typedef tlm::tlm_base_protocol_types TYPES;
	static const bool DEBUG = true;
};

template<class CONFIG = unisim::component::tlm2::bus::simple_router::Config>
class Router :
	public unisim::kernel::service::Object,
	public sc_module {
private:
	typedef unisim::kernel::service::Object Object;
	static const unsigned int MAX_NUM_MAPPINGS = CONFIG::MAX_NUM_MAPPINGS; 
	static const unsigned int BUSWIDTH = CONFIG::BUSWIDTH;
	typedef typename CONFIG::TYPES TYPES;
	static const bool DEBUG = CONFIG::DEBUG;

public:
	SC_HAS_PROCESS(Router);
	Router(const sc_module_name &name, Object *parent = 0);
	~Router();

	/** Router initiator port sockets */
	tlm_utils::multi_passthrough_initiator_socket<Router, BUSWIDTH, TYPES> init_socket;
	/** Router target port sockets */
	tlm_utils::multi_passthrough_target_socket<Router, BUSWIDTH, TYPES> targ_socket;
	
	/** Object setup method
	 * This method is required by the inherited Object class.
	 *
	 * @return true if setup is performed with success, false otherwise
	 */
	virtual bool Setup();

private:
	typedef typename TYPES::tlm_payload_type transaction_type;
	typedef typename TYPES::tlm_phase_type   phase_type;
	typedef tlm::tlm_sync_enum               sync_enum_type;

	/*************************************************************************
	 * Multi passtrough initiator socket callbacks                     START *
	 *************************************************************************/

	sync_enum_type  I_nb_transport_bw_cb(int id, transaction_type &trans, phase_type &phase, sc_core::sc_time &time);
	void            I_invalidate_direct_mem_ptr_cb(int id, sc_dt::uint64 start_range, sc_dt::uint64 end_range);

	/*************************************************************************
	 * Multi passtrough initiator socket callbacks                       END *
	 *************************************************************************/

	/*************************************************************************
	 * Multi passtrough target socket callbacks                        START *
	 *************************************************************************/

	sync_enum_type	T_nb_transport_fw_cb(int id, transaction_type &trans, phase_type &phase, sc_core::sc_time &time);
	void			T_b_transport_cb(int id, transaction_type &trans, sc_core::sc_time &time);
	unsigned int	T_transport_dbg_cb(int id, transaction_type &trans);
	bool			T_get_direct_mem_ptr_cb(int id, transaction_type &trans, tlm::tlm_dmi &dmi);

	/*************************************************************************
	 * Multi passthrough target socket callbacks                         END *
	 *************************************************************************/
	
	/*************************************************************************
	 * Parameters                                                      START *
	 *************************************************************************/

	sc_core::sc_time cycle_time;
	double cycle_time_double;
	unisim::kernel::service::Parameter<double> param_cycle_time_double;
	Mapping mapping[MAX_NUM_MAPPINGS];
	unisim::kernel::service::Parameter<Mapping> *param_mapping[MAX_NUM_MAPPINGS];
	unsigned int port_buffer_size;
	unisim::kernel::service::Parameter<unsigned int> param_port_buffer_size;

	/*************************************************************************
	 * Parameters                                                        END *
	 *************************************************************************/
	
	/*************************************************************************
	 * Logger and verbose parameters                                   START *
	 *************************************************************************/

	unisim::kernel::logger::Logger logger;
	bool verbose_all;
	unisim::kernel::service::Parameter<bool> param_verbose_all;
	bool verbose_setup;
	unisim::kernel::service::Parameter<bool> param_verbose_setup;
	bool verbose_non_blocking;
	unisim::kernel::service::Parameter<bool> param_verbose_non_blocking;
	bool verbose_blocking;
	unisim::kernel::service::Parameter<bool> param_verbose_blocking;

	/*************************************************************************
	 * Logger and verbose parameters                                     END *
	 *************************************************************************/

	/*************************************************************************
	 * Verbose methods                                                 START *
	 *************************************************************************/

	inline bool VerboseSetup();
	inline bool VerboseNonBlocking();
	inline bool VerboseBlocking();

	/*************************************************************************
	 * Verbose methods                                                   END *
	 *************************************************************************/
};

} // end of namespace simple_router
} // end of namespace bus
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_BUS_SIMPLE_ROUTER_HH__
