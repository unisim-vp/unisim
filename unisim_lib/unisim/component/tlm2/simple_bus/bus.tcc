#ifndef __UNISIM_COMPONENT_TLM2_BUS_BUS_CONTROLLER_TCC__
#define __UNISIM_COMPONENT_TLM2_BUS_BUS_CONTROLLER_TCC__

namespace unisim {
namespace component {
namespace tlm2 {
namespace bus {

template<unsigned int BUSWIDTH, typename TYPES>
BusController<BUSWIDTH, TYPES>::
BusController(const sc_module_name& name, unisim::kernel::service::Object *parent) :
unisim::kernel::service::Object(name, parent),
targ_socket("target_socket"),
init_socket("init_socket"),
peq("peq"),
free_peq("peq"),
bus_cycle_time(SC_ZERO_TIME),
bus_cycle_time_double(0.0),
param_bus_cycle_time_double("bus_cycle_time", this, bus_cycle_time_double) {
	/* register target multi socket callbacks */
 	targ_socket.register_nb_transport_fw(    this, &BusController<BUSWIDTH, TYPES>::T_nb_transport_fw_cb);
 	targ_socket.register_b_transport(        this, &BusController<BUSWIDTH, TYPES>::T_b_transport_cb);
 	targ_socket.register_transport_dbg(      this, &BusController<BUSWIDTH, TYPES>::T_transport_dbg_cb);
 	targ_socket.register_get_direct_mem_ptr( this, &BusController<BUSWIDTH, TYPES>::T_get_direct_mem_ptr_cb);

	/* register initiator socket callbacks */
	init_socket.register_nb_transport_bw(           this, &BusController<BUSWIDTH, TYPES>::I_nb_transport_bw_cb);
	init_socket.register_invalidate_direct_mem_ptr( this, &BusController<BUSWIDTH, TYPES>::I_invalidate_direct_mem_ptr_cb);

	SC_THREAD(Dispatcher);
}

template<unsigned int BUSWIDTH, typename TYPES>
BusController<BUSWIDTH, TYPES>::
~BusController() {
}

template<unsigned int BUSWIDTH, typename TYPES>
bool
BusController<BUSWIDTH, TYPES>::
Setup() {
	if(bus_cycle_time_double == 0.0) {
		cerr << "PARAMETER ERROR(" << __FUNCTION__ << ":" << __FILE__ << ":" << __LINE__ << "): the bus cycle time time must be bigger than 0" << endl;
		cerr << GetName() << endl;
		return false;
	}
	bus_cycle_time = sc_time(bus_cycle_time_double, SC_PS);
	cerr << GetName() << ": setting bus_cycle_time to " << bus_cycle_time << endl;

	return true;
}

/*************************************************************************
 * Multi passtrough target socket callbacks                        START *
 *************************************************************************/

template<unsigned int BUSWIDTH, typename TYPES>
tlm::tlm_sync_enum
BusController<BUSWIDTH, TYPES>::
T_nb_transport_fw_cb(int id, 
		typename TYPES::tlm_payload_type &trans, 
		typename TYPES::tlm_phase_type &phase, 
		sc_core::sc_time &time) {
	cerr << GetName() << "(" << sc_time_stamp() + time << "): received nb_transport_fw on port id = " << id << ", putting it in the waiting queue" << endl;
	BusTlmGenericProtocol<TYPES> *item = 0;
	switch(phase) {
	case tlm::BEGIN_REQ:
		// put the incomming request in the waiting queue
		// return tlm::ACCEPTED
		item = free_peq.get_next_transaction();
		if(item == 0) {
			item = new BusTlmGenericProtocol<TYPES>();
			assert(trans.has_mm());
			trans.acquire();
			item->payload = &trans;
			item->from_initiator = true;
			item->id = id;
			item->phase = phase;
		}
		peq.notify(*item, time);
		return tlm::TLM_ACCEPTED;
		break;
	case tlm::END_RESP:
		// forward the end response message to the target module
		return init_socket->nb_transport_fw(trans, phase, time);
		break;
	// cases that can not be handled
	case tlm::END_REQ:
		cerr << "FATAL_ERROR(" << __FUNCTION__ << ":" << __FILE__ << ":" << __LINE__ << "):" << endl;
		cerr << GetName() << "(" << sc_time_stamp() + time << "): unhandled phase END_REQ received (" << phase << ")" << endl;
		sc_stop();
		wait();
		break;
	case tlm::BEGIN_RESP:
	default:
		cerr << "FATAL_ERROR(" << __FUNCTION__ << ":" << __FILE__ << ":" << __LINE__ << "):" << endl;
		cerr << GetName() << "(" << sc_time_stamp() + time << "): unhandled phase BEGIN_RESP received (" << phase << ")" << endl;
		sc_stop();
		wait();
		break;
	}
}

template<unsigned int BUSWIDTH, typename TYPES>
void
BusController<BUSWIDTH, TYPES>::
T_b_transport_cb(int id,
		typename TYPES::tlm_payload_type &trans,
		sc_core::sc_time &time) {
	cerr << GetName() << ": received b_transport on port id = " << id << " forwarding it" << endl;
	init_socket->b_transport(trans, time);
}

template<unsigned int BUSWIDTH, typename TYPES>
unsigned int
BusController<BUSWIDTH, TYPES>::
T_transport_dbg_cb(int id,
		typename TYPES::tlm_payload_type &trans) {
	return 0;  // Dummy implementation
}

template<unsigned int BUSWIDTH, typename TYPES>
bool
BusController<BUSWIDTH, TYPES>::
T_get_direct_mem_ptr_cb(int id,
		typename TYPES::tlm_payload_type &trans,
		tlm::tlm_dmi &dmi) {
	return false;  // Dummy implementation
}

/*************************************************************************
 * Multi passthrough target socket callbacks                         END *
 *************************************************************************/

/*************************************************************************
 * Simple initiator socket callbacks                               START *
 *************************************************************************/
	
template<unsigned int BUSWIDTH, typename TYPES>
tlm::tlm_sync_enum
BusController<BUSWIDTH, TYPES>::
I_nb_transport_bw_cb(typename TYPES::tlm_payload_type &trans, 
		typename TYPES::tlm_phase_type &phase, 
		sc_core::sc_time &time) {
}

template<unsigned int BUSWIDTH, typename TYPES>
void
BusController<BUSWIDTH, TYPES>::
I_invalidate_direct_mem_ptr_cb(sc_dt::uint64 start_range, 
		sc_dt::uint64 end_range) {
	// Dummy implementation
}

/*************************************************************************
 * Simple initiator socket callbacks                                 END *
 *************************************************************************/

template<unsigned int BUSWIDTH, typename TYPES>
void
BusController<BUSWIDTH, TYPES>::
Dispatcher() {
	sc_core::sc_event &ev = peq.get_event();
	BusTlmGenericProtocol<TYPES> *item;
	sc_time cur_time;

	while(1) {
		wait(ev);
		item = peq.get_next_transaction();
		if(item != 0) BusSynchronize();
		while(item != 0) {
	        item->payload = 0;
			free_peq.notify(*item);
			wait(bus_cycle_time);
			item = peq.get_next_transaction();
		}
	}
}

template<unsigned int BUSWIDTH, typename TYPES>
void
BusController<BUSWIDTH, TYPES>::
BusSynchronize() {
	sc_time cur_time = sc_time_stamp();
	sc_dt::uint64 cur_time_int = cur_time.value();
	sc_dt::uint64 bus_cycle_time_int = bus_cycle_time.value();
	sc_dt::uint64 diff_int = bus_cycle_time_int - (cur_time_int % bus_cycle_time_int);
	sc_time diff = sc_time(diff_int, false);
	wait(diff);
}

} // end of namespace bus
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_BUS_BUS_CONTROLLER_TCC___

