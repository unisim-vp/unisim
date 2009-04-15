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
 
#ifndef __UNISIM_COMPONENT_TLM2_INTERCONNECT_GENERIC_ROUTER_HH__
#define __UNISIM_COMPONENT_TLM2_INTERCONNECT_GENERIC_ROUTER_HH__

#include <systemc.h>
#include <tlm.h>
#include <tlm_utils/simple_initiator_socket.h>
#include <tlm_utils/passthrough_target_socket.h>
#include <inttypes.h>
#include <queue>
#include <vector>
#include <map>
#include "unisim/kernel/tlm2/tlm.hh"
#include "unisim/kernel/service/service.hh"
#include "unisim/kernel/logger/logger.hh"
#include "unisim/component/tlm2/interconnect/generic_router/config.hh"
#include "unisim/component/tlm2/interconnect/generic_router/router_dispatcher.hh"

namespace unisim {
namespace component {
namespace tlm2 {
namespace interconnect {
namespace generic_router {

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

class RouterPayloadExtension :
	public tlm::tlm_extension<RouterPayloadExtension> {
public:
	RouterPayloadExtension() : fifo() {}

	virtual tlm_extension_base *clone() const {
		RouterPayloadExtension *clone = new RouterPayloadExtension(fifo);
		return clone;
	}

	virtual void copy_from(tlm_extension_base const &ext) {
		fifo = static_cast<RouterPayloadExtension const &>(ext).fifo;
	}

	void Push(unsigned int id) {
		fifo.push(id);
	}

	bool Empty() const {
		return fifo.empty();
	}

	unsigned int Front() const {
		return fifo.front();
	}

	void Pop() {
		fifo.pop();
	}

private:
	std::queue<unsigned int> fifo;

	RouterPayloadExtension(std::queue<unsigned int> const &fifo_clone) : fifo(fifo_clone) {}
};

template<class CONFIG = unisim::component::tlm2::interconnect::generic_router::Config>
class Router :
	public unisim::kernel::service::Object,
	public sc_module {
private:
	typedef unisim::kernel::service::Object Object;
	static const unsigned int INPUT_SOCKETS = CONFIG::INPUT_SOCKETS;
	static const unsigned int OUTPUT_SOCKETS = CONFIG::OUTPUT_SOCKETS;
	static const unsigned int MAX_NUM_MAPPINGS = CONFIG::MAX_NUM_MAPPINGS; 
	static const unsigned int BUSWIDTH = CONFIG::BUSWIDTH;
	typedef typename CONFIG::TYPES TYPES;
	static const bool VERBOSE = CONFIG::VERBOSE;

	class InitSocket : 
		public tlm_utils::simple_initiator_socket_tagged<Router, BUSWIDTH, TYPES> {
	public:
		InitSocket() :
			tlm_utils::simple_initiator_socket_tagged<Router, BUSWIDTH, TYPES>("init_socket")
		{
		}
	};

	class TargSocket :
		public tlm_utils::passthrough_target_socket_tagged<Router, BUSWIDTH, TYPES> {
	public:
		TargSocket() :
			tlm_utils::passthrough_target_socket_tagged<Router, BUSWIDTH, TYPES>("targ_socket")
		{
		}
	};

public:
	SC_HAS_PROCESS(Router);
	Router(const sc_module_name &name, Object *parent = 0);
	~Router();

	/** Router initiator port sockets */
	InitSocket init_socket[OUTPUT_SOCKETS];
	// tlm_utils::simple_initiator_socket_tagged<Router, BUSWIDTH, TYPES> init_socket[OUTPUT_SOCKETS];
	// TODO: remove ==> tlm_utils::multi_passthrough_initiator_socket<Router, BUSWIDTH, TYPES> init_socket;
	/** Router target port sockets */
	TargSocket targ_socket[INPUT_SOCKETS];
	// tlm_utils::passthrough_target_socket_tagged<Router, BUSWIDTH, TYPES> targ_socket[INPUT_SOCKETS];
	// TODO: remove ==> tlm_utils::multi_passthrough_target_socket<Router, BUSWIDTH, TYPES> targ_socket;
	
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
	 * Payload fabric                                                  START *
	 *************************************************************************/

	unisim::kernel::tlm2::PayloadFabric<tlm::tlm_generic_payload> payload_fabric;

	/*************************************************************************
	 * Payload fabric                                                    END *
	 *************************************************************************/

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
	 * Ready times for each incomming queue                            START *
	 *************************************************************************/

	std::vector<sc_time> m_targ_req_ready;
	std::vector<sc_time> m_init_rsp_ready;

	/*************************************************************************
	 * Ready times for each incomming queue                              END *
	 *************************************************************************/

	/*************************************************************************
	 * Dispatch handlers                                               START *
	 *************************************************************************/

	std::vector<RouterDispatcher<Router<CONFIG>, CONFIG> *> m_req_dispatcher;
	std::vector<RouterDispatcher<Router<CONFIG>, CONFIG> *> m_rsp_dispatcher;
	void SendReq(unsigned int id, transaction_type &trans);
	void SendRsp(unsigned int id, transaction_type &trans);

	/*************************************************************************
	 * Dispatch handlers                                                 END *
	 *************************************************************************/

	/*************************************************************************
	 * Helper methods                                                  START *
	 *************************************************************************/

	/**
	 * Synchronize the dispatcher with the given time
	 *
	 * @param  time   the delay against sc_time_stamp
	 * @return        the delay against sc_time_stamp once synchronized
	 */
	sc_time Sync(const sc_time &time);

	/** 
	 * Apply mapping function over the given transaction
	 *
	 * @param  trans  the transaction to apply the mapping over
	 * @param  port   the port that maps that transaction
	 * @return        true if a map was found, false otherwise
	 */
	bool ApplyMap(const transaction_type &trans, unsigned int &port) const;

	/**
	 * Apply mapping function over a given transaction and return the mapping division
	 *
	 * @param trans   the transaction to apply the mapping over
	 * @param ports   the ports that map the transaction
	 */
	void ApplyMap(const transaction_type &trans, std::vector<unsigned int> &mappings) const;

	/**
	 * Set the incomming port into the transaction using the tlm2.0 extension
	 *   mechanism.
	 *
	 * @param  trans  the transaction to mark
	 * @param  port   the incomming port
	 */
	void SetRouterExtension(transaction_type &trans, unsigned int port);

	/**
	 * Get the incomming port from the transaction using the tlm2.0 extension
	 *   mechanism. It removes the port from the transaction.
	 *
	 * @param  trans  the transaction to handle
	 * @param  port   the incomming port found in the transaction extension
	 * @return        true if a transaction extension was found, false otherwise
	 */
	bool GetRouterExtension(transaction_type &trans, unsigned int &port);

	/*************************************************************************
	 * Helper methods                                                    END *
	 *************************************************************************/

	/*************************************************************************
	 * Transport debugging helper methods                              START *
	 *************************************************************************/

	/**
	 * Transport debugging method to handle read requests.
	 *
	 * @param  id    the incomming port identifier
	 * @param  trans the read transaction to handle
	 * @return       the total number of bytes that were read
	 */
	inline unsigned int ReadTransportDbg(unsigned int id, transaction_type &trans);
	
	/**
	 * Transport debugging method to handle read requests.
	 *
	 * @param  id    the incomming port identifier
	 * @param  trans the read transaction to handle
	 * @return       the total number of bytes that were read
	 */
	inline unsigned int WriteTransportDbg(unsigned int id, transaction_type &trans);

	/*************************************************************************
	 * Transport debugging helper methods                                END *
	 *************************************************************************/

	/*************************************************************************
	 * Parameters                                                      START *
	 *************************************************************************/

	sc_core::sc_time cycle_time;
	double cycle_time_double;
	unisim::kernel::service::Parameter<double> param_cycle_time_double;
//	unsigned int num_input_sockets;
//	unisim::kernel::service::Parameter<unsigned int> param_num_input_sockets;
//	unsigned int num_output_sockets;
//	unisim::kernel::service::Parameter<unsigned int> param_num_output_sockets;
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
	unisim::kernel::service::Parameter<bool> *param_verbose_all;
	bool verbose_setup;
	unisim::kernel::service::Parameter<bool> *param_verbose_setup;
	bool verbose_non_blocking;
	unisim::kernel::service::Parameter<bool> *param_verbose_non_blocking;
	bool verbose_blocking;
	unisim::kernel::service::Parameter<bool> *param_verbose_blocking;

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

} // end of namespace unisim
} // end of namespace component
} // end of namespace tlm2
} // end of namespace interconnect
} // end of namespace generic_router

#endif // __UNISIM_COMPONENT_TLM2_INTERCONNECT_GENERIC_ROUTER_HH__
