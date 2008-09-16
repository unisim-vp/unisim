#ifndef __UNISIM_COMPONENT_TLM2_INTERCONNECT_GENERIC_ROUTER_TCC__
#define __UNISIM_COMPONENT_TLM2_INTERCONNECT_GENERIC_ROUTER_TCC__

#define LOCATION 	" - location = " << __FUNCTION__ << ":unisim_lib/unisim/component/tlm2/interconnect/generic_router/router.tcc:" << __LINE__
#define TIME(X) 	" - time = " << sc_time_stamp() + (X)
#define PHASE(X) 	" - phase = " << 	( (X) == tlm::BEGIN_REQ  ? 	"BEGIN_REQ" : \
										( (X) == tlm::END_REQ    ? 	"END_REQ" : \
										( (X) == tlm::BEGIN_RESP ? 	"BEGIN_RESP" : \
																	"END_RESP")))
#define TRANS(L,X) \
{ \
	(L) << " - trans = " << &(X) << endl \
		<< "   - " << ((X).is_read()?"read":"write") << endl \
		<< "   - address = 0x" << hex << (X).get_address() << dec << endl \
		<< "   - data_length = " << (X).get_data_length() << endl; \
	if((X).is_write()) { \
		(L) << "   - data ="; \
		for(unsigned int _trans_i = 0; _trans_i < (X).get_data_length(); _trans_i++) { \
			(L) << " " << hex << (unsigned int)((X).get_data_ptr()[_trans_i]) << dec; \
		} \
	} \
}

namespace unisim {
namespace component {
namespace tlm2 {
namespace interconnect {
namespace generic_router {

using namespace unisim::kernel::logger;

template<class CONFIG>
Router<CONFIG>::
Router(const sc_module_name &name, Object *parent) :
sc_module(name),
Object(name, parent),
targ_socket("targ_socket"),
init_socket("init_socket"), 
cycle_time(SC_ZERO_TIME),
cycle_time_double(0.0),
param_cycle_time_double("cycle_time", this, cycle_time_double),
port_buffer_size(0),
param_port_buffer_size("port_buffer_size", this, port_buffer_size, "Defines the size of the buffer for incomming requests in each of the input ports (0 = infinite)"),
logger(*this),
verbose_all(false),
param_verbose_all("verbose_all", this, verbose_all, "Activate all the verbose options"),
verbose_setup(false),
param_verbose_setup("verbose_setup", this, verbose_setup, "Display Object setup information"),
verbose_non_blocking(false),
param_verbose_non_blocking("verbose_non_blocking", this, verbose_non_blocking, "Display non_blocking transactions handling"),
verbose_blocking(false),
param_verbose_blocking("verbose_blocking", this, verbose_blocking, "Display blocking transactions handling") {
	/* instantiate the mapping parameters */
	for(unsigned int i = 0; i < MAX_NUM_MAPPINGS; i++) {
		std::stringstream buf;
		buf << "mapping_" << i;
		param_mapping[i] = new unisim::kernel::service::Parameter<Mapping>(buf.str().c_str(), this, mapping[i], "Define the mapping of the router");
	}
	/* register target multi socket callbacks */
 	targ_socket.register_nb_transport_fw(    this, &Router<CONFIG>::T_nb_transport_fw_cb);
 	targ_socket.register_b_transport(        this, &Router<CONFIG>::T_b_transport_cb);
 	targ_socket.register_transport_dbg(      this, &Router<CONFIG>::T_transport_dbg_cb);
 	targ_socket.register_get_direct_mem_ptr( this, &Router<CONFIG>::T_get_direct_mem_ptr_cb);

	/* register initiator socket callbacks */
	init_socket.register_nb_transport_bw(           this, &Router<CONFIG>::I_nb_transport_bw_cb);
	init_socket.register_invalidate_direct_mem_ptr( this, &Router<CONFIG>::I_invalidate_direct_mem_ptr_cb);
}

template<class CONFIG>
Router<CONFIG>::
~Router() {
}

template<class CONFIG>
bool
Router<CONFIG>::
Setup() {
	const unsigned int num_mappings = CONFIG::MAX_NUM_MAPPINGS;
	
	if(cycle_time_double == 0.0) {
		logger << DebugError << "PARAMETER ERROR: the cycle_time parameter  must be bigger than 0" << endl
			<< LOCATION << EndDebug;
		return false;
	}
	bool has_mapping = false;
	for(unsigned int i = 0; i < num_mappings; i++) {
		if(mapping[i].used) has_mapping = true;	
	}
	if(!has_mapping) {
		logger << DebugError << "PARAMETER ERROR: no mapping was defined" << endl
			<< LOCATION << EndDebug;
		return false;
	}
	for(unsigned int i = 0; i < num_mappings; i++) {
		if(mapping[i].used && mapping[i].output_port >= init_socket.size()) {
			logger << DebugWarning << "PARAMETER ERROR: mapping has a bigger output_port than available ports" << endl
				<< LOCATION << endl;
			logger << "  mapping_" << i << ": " << endl;
			logger << "    - start_range = 0x" << hex << mapping[i].range_start << dec << endl;
			logger << "    - end_range   = 0x" << hex << mapping[i].range_end << dec << endl;
			logger << "    - output_port = " << mapping[i].output_port;
			logger << EndDebug;
		}
	}
	cycle_time = sc_time(cycle_time_double, SC_PS);

	/* display the configuration of the router */
	if(VerboseSetup()) {
		logger << DebugInfo << "Setting cycle_time to " << cycle_time << endl;
		logger << "Mappings (MAX_NUM_MAPPINGS = " << num_mappings << "): ";
		for(unsigned int i = 0; i < num_mappings; i++) {
			if(mapping[i].used) {
				logger << endl << " - " << i << ": start_range = 0x" << hex << mapping[i].range_start << dec << endl;
				logger << "      end_range   = 0x" << hex << mapping[i].range_end << dec << endl;
				logger << "      output_port = " << mapping[i].output_port;
			}
		}
		logger << EndDebug;
	}

	return true;
}

/*************************************************************************
 * Multi passtrough initiator socket callbacks                     START *
 *************************************************************************/

template<class CONFIG>
tlm::tlm_sync_enum 
Router<CONFIG>::
I_nb_transport_bw_cb(int id, transaction_type &trans, phase_type &phase, sc_core::sc_time &time) {
	return tlm::TLM_ACCEPTED;
}

template<class CONFIG>
void
Router<CONFIG>::
I_invalidate_direct_mem_ptr_cb(int id, sc_dt::uint64 start_range, sc_dt::uint64 end_range) {
}

/*************************************************************************
 * Multi passtrough initiator socket callbacks                       END *
 *************************************************************************/

/*************************************************************************
 * Multi passtrough target socket callbacks                        START *
 *************************************************************************/

template<class CONFIG>
tlm::tlm_sync_enum
Router<CONFIG>::
T_nb_transport_fw_cb(int id, transaction_type &trans, phase_type &phase, sc_core::sc_time &time) {
	if(VerboseNonBlocking()) {
		logger << DebugInfo << "Received nb_transport_fw on port " << id << ", checking if it can be queued" << endl
			<< TIME(time) << endl;
		TRANS(logger, trans);
		logger << EndDebug;
	}
	/* the request will be queued into the port queue and sent once the routing engine is available */
	return tlm::TLM_ACCEPTED;
}

template<class CONFIG>
void
Router<CONFIG>::
T_b_transport_cb(int id, transaction_type &trans, sc_core::sc_time &time) {
	/* the first thing that must be done is the translation from the mapping table */
	if(VerboseBlocking()) {
		logger << DebugInfo << "Received b_transport on port " << id << ", forwarding it" << endl
			<< TIME(time) << endl;
		TRANS(logger, trans);
		logger << EndDebug;
	}
	/* check the address of the transaction to perform the port routing */
	bool found = false;
	uint64_t address = trans.get_address();
	unsigned int port;
	for(unsigned int i = 0; !found && i < MAX_NUM_MAPPINGS; i++) {
		if(mapping[i].used) {
			if(address >= mapping[i].range_start && trans.get_address() < mapping[i].range_end) {
				found = true;
				port = mapping[i].output_port;
			}
		}
	}
	if(!found) {
		logger << DebugError << "Received transaction on port " << id << " has an unmapped address"  << endl
			<< LOCATION << endl
			<< TIME(time) << endl;
		TRANS(logger, trans);
		logger << EndDebug;
		return;
	}
	/* check that output port is within bounds??? */
	if(mapping[port].output_port >= init_socket.size()) {
		logger << DebugError << "When handling received transaction" << endl;
		TRANS(logger, trans);
		logger << endl << "mapping output port is bigger or equal than number of output ports ("
			<< mapping[port].output_port << ">=" << init_socket.size() << ")"
			<< EndDebug;
		return;
	}
	/* forward the transaction to the selected output port */
	if(VerboseBlocking()) {
		logger << DebugInfo << "Forwarding transaction received on port " << id << " to port " << port << endl
			<< TIME(time) << endl;
		TRANS(logger, trans);
		logger << EndDebug;
	}
	init_socket[port]->b_transport(trans, time);
}

template<class CONFIG>
unsigned int
Router<CONFIG>::
T_transport_dbg_cb(int id, transaction_type &trans) {
	return 0;
}

template<class CONFIG>
bool
Router<CONFIG>::
T_get_direct_mem_ptr_cb(int id, transaction_type &trans, tlm::tlm_dmi &dmi) {
	return false;
}

/*************************************************************************
 * Multi passthrough target socket callbacks                         END *
 *************************************************************************/

/*************************************************************************
 * Verbose methods                                                 START *
 *************************************************************************/

template<class CONFIG>
inline bool 
Router<CONFIG>::
VerboseSetup() {
	if(DEBUG && (verbose_all || verbose_setup)) 
		return true;
	return false;
}

template<class CONFIG>
inline bool
Router<CONFIG>::
VerboseNonBlocking() {
	if(DEBUG && (verbose_all || verbose_non_blocking))
		return true;
	return false;
}

template<class CONFIG>
inline bool
Router<CONFIG>::
VerboseBlocking() {
	if(DEBUG && (verbose_all || verbose_blocking))
		return true;
	return false;
}

/*************************************************************************
 * Verbose methods                                                   END *
 *************************************************************************/

} // end of namespace generic_router
} // end of namespace interconnect
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_INTERCONNECT_GENERIC_ROUTER_TCC__
