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
pending_transactions(),
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
	cerr << GetName() << "(" << sc_time_stamp() + time << "): received nb_transport_fw on port id = " << id << endl;
	if(phase == tlm::BEGIN_REQ) {
		trans.acquire();
		BusTlmGenericProtocol<TYPES> *item = 0;
		item = free_peq.get_next_transaction();
		if(item == 0) {
			item = new BusTlmGenericProtocol<TYPES>();
		}
		assert(trans.has_mm());
		trans.acquire();
		item->payload = &trans;
		item->from_initiator = true;
		item->id = id;
		item->send_end_req = true;
		item->send_end_resp = true;
		AddPendingTransaction(trans, item);
		peq.notify(*item, time);
		return tlm::TLM_ACCEPTED;
	} else if(phase == tlm::END_RESP) {
		end_resp_event.notify(time);
		return tlm::TLM_COMPLETED;
	} else {
		cerr << "FATAL_ERROR(" << __FUNCTION__ << ":" << __FILE__ << ":" << __LINE__ << "):" << endl;
		cerr << GetName() << "(" << sc_time_stamp() + time << "): unexpected phase value ";
		if(phase == tlm::END_REQ) cerr << "END_REQ";
		else cerr << "BEGIN_RESP";
		cerr << endl;
		sc_stop();
		wait();
	}

	// this should never be executed
	cerr << "FATAL_ERROR(" << __FUNCTION__ << ":" << __FILE__ << ":" << __LINE__ << "):" << endl;
	cerr << GetName() << "(" << sc_time_stamp() + time << "): unreacheable code reached" << endl;
	sc_stop();
	wait();
	return tlm::TLM_ACCEPTED; // unnecessary, but to avoid compilation errors/warnings
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
	cerr << GetName() << "(" << sc_time_stamp() + time << "): received nb_transport_bw from output port" << endl;

	if(phase == tlm::END_REQ) {
		// send the END_REQ to the initiator module and unlock the dispatcher
		BusTlmGenericProtocol<TYPES> *item = pending_transactions[&trans];
		item->send_end_req = true;
		item->send_end_resp = true;
		end_req_event.notify(time);
		return tlm::TLM_ACCEPTED;
	} else if(phase == tlm::BEGIN_RESP) {
		assert(pending_transactions.find(&trans) != pending_transactions.end());
		BusTlmGenericProtocol<TYPES> *item = pending_transactions[&trans];
		item->send_end_req = false;
		item->send_end_resp = true;
		item->from_initiator = false;
		peq.notify(*item, time);
		return tlm::TLM_ACCEPTED;
	} else {
		cerr << "FATAL_ERROR(" << __FUNCTION__ << ":" << __FILE__ << ":" << __LINE__ << "):" << endl;
		cerr << GetName() << "(" << sc_time_stamp() + time << "): unexpected phase value ";
		if(phase == tlm::BEGIN_REQ) cerr << "BEGIN_REQ";
		else cerr << "END_RESP";
		cerr << endl;
		sc_stop();
		wait();
	}

	// this should never be executed
	cerr << "FATAL_ERROR(" << __FUNCTION__ << ":" << __FILE__ << ":" << __LINE__ << "):" << endl;
	cerr << GetName() << "(" << sc_time_stamp() + time << "): unreacheable code reached" << endl;
	sc_stop();
	wait();
	return tlm::TLM_ACCEPTED; // unnecessary, but to avoid compilation errors/warnings
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

/*************************************************************************
 * Dispatcher methods and variables                                START *
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
		while(item != 0) {
			BusSynchronize();
			cerr << GetName() << "(" << sc_time_stamp() << "): dispatching transaction (item " << item << ", trans " << item->payload << ", from_initiator = " << item->from_initiator << ")" << endl;
			if(item->from_initiator) {
				DispatchFW(item);
			} else {
				DispatchBW(item);
			}
			item = peq.get_next_transaction();
		}
	}
}

template<unsigned int BUSWIDTH, typename TYPES>
void
BusController<BUSWIDTH, TYPES>::
DispatchFW(BusTlmGenericProtocol<TYPES> *item) {
	sc_time time(SC_ZERO_TIME);
	sc_time end_req_time(SC_ZERO_TIME);
	typename TYPES::tlm_phase_type phase;

	phase = tlm::BEGIN_REQ;
	cerr << GetName() << "(" << sc_time_stamp() << "): sending request to target (" << item->payload << ")" << endl;
	switch(init_socket->nb_transport_fw(*(item->payload), phase, time)) {
	case tlm::TLM_ACCEPTED:
	case tlm::TLM_UPDATED:
		// transaction not yet finished
		if(phase == tlm::BEGIN_REQ) {
			// request phase not yet finished
			wait(end_req_event);
			// check if the END_REQ message needs to be sent
			if(item->send_end_req) {
				phase = tlm::END_REQ;
				end_req_time = SC_ZERO_TIME;
				targ_socket[item->id]->nb_transport_bw(*(item->payload), phase, end_req_time);
				item->send_end_req = false;
			}
		} else if (phase == tlm::END_REQ) {
			// request phase finished, but respose phase not yet started
			phase = tlm::END_REQ;
			end_req_time = time;
			targ_socket[item->id]->nb_transport_bw(*(item->payload), phase, end_req_time);
			item->send_end_req = false;
			wait(time);
		} else if(phase == tlm::BEGIN_RESP) {
			// send the END_REQ message 
			phase = tlm::END_REQ;
			end_req_time = SC_ZERO_TIME;
			targ_socket[item->id]->nb_transport_bw(*(item->payload), phase, end_req_time);
			// reuse the item to send the response to the initiator module
			item->phase = tlm::BEGIN_RESP;
			item->from_initiator = false;
			item->send_end_req = false;
			item->send_end_resp = true;
			peq.notify(*item, time);
		} else {
			cerr << "FATAL_ERROR(" << __FUNCTION__ << ":" << __FILE__ << ":" << __LINE__ << "):" << endl;
			cerr << GetName() << "(" << sc_time_stamp() + time << "): unhandled phase value tlm::END_RESP" << endl;
			sc_stop();
			wait();
		}
		break;
	case tlm::TLM_COMPLETED:
		cerr << GetName() << "(" << sc_time_stamp() + time << "): received TLM_COMPLETE to BEGIN_REQ (for transaction " << item->payload << ")" << endl;
		cerr << "\tgeneratig an END_REQ for the source" << endl;
		phase = tlm::END_REQ;
		end_req_time = SC_ZERO_TIME;
		switch(targ_socket[item->id]->nb_transport_bw(*(item->payload), phase, time)) {
		case tlm::TLM_ACCEPTED:
			cerr << GetName() << "(" << sc_time_stamp() + time << "): END_REQ accepted (TLM_ACCEPTED) by the source" << endl;
			// the response can be sent, reuse the item for that
			item->from_initiator = false;
			item->send_end_req = false;
			item->send_end_resp = false;
			peq.notify(*item, time);
			break;
		case tlm::TLM_UPDATED:
			cerr << "FATAL_ERROR(" << __FUNCTION__ << ":" << __FILE__ << ":" << __LINE__ << "):" << endl;
			cerr << GetName() << "(" << sc_time_stamp() + time << "): unhandled sync tlm::TLM_UPDATED" << endl;
			sc_stop();
			wait();
			break;
		case tlm::TLM_COMPLETED:
			cerr << GetName() << "(" << sc_time_stamp() + time << "): END_REQ accepted (TLM_COMPLETED) by the source" << endl;
			// the item can be removed
			item->payload->release();
			item->payload = 0;
			RemovePendingTransaction(*(item->payload));
			free_peq.notify(*item);
			break;
		}
		break;
	default:
		cerr << "FATAL_ERROR(" << __FUNCTION__ << ":" << __FILE__ << ":" << __LINE__ << "):" << endl;
		cerr << GetName() << "(" << sc_time_stamp() + time << "): unhandled sync value received" << endl;
		sc_stop();
		wait();
		break;
	}
}

template<unsigned int BUSWIDTH, typename TYPES>
void
BusController<BUSWIDTH, TYPES>::
DispatchBW(BusTlmGenericProtocol<TYPES> *item) {
	typename TYPES::tlm_phase_type phase;
	sc_time time(SC_ZERO_TIME);
	sc_time end_resp_time;

	phase = tlm::BEGIN_RESP;
	cerr << GetName() << "(" << sc_time_stamp() + time << "): sending BEGIN_RESP to source (for transaction " << item->payload << ")" << endl;
	switch(targ_socket[item->id]->nb_transport_bw(*(item->payload), phase, time)) {
	case tlm::TLM_ACCEPTED:
	case tlm::TLM_UPDATED:
		// the bus must wait for the response (END_RESP) from the initiator to the target
		wait(end_resp_event);
		if(item->send_end_resp) {
			phase = tlm::END_RESP;
			end_resp_time = SC_ZERO_TIME;
			init_socket->nb_transport_fw(*(item->payload), phase, end_resp_time);
			// the transaction can be released and removed from the lookup table
			RemovePendingTransaction(*(item->payload));
			item->payload->release();
			item->payload = 0;
			free_peq.notify(*item);
		}
		break;
	case tlm::TLM_COMPLETED:
		// a response (END_RESP) must be sent to the target
		phase = tlm::END_RESP;
		end_resp_time = time;
		init_socket->nb_transport_fw(*(item->payload), phase, end_resp_time);
		// the transaction can be released and removed from the lookup table
		RemovePendingTransaction(*(item->payload));
		item->payload->release();
		item->payload = 0;
		free_peq.notify(*item);
		break;
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

template<unsigned int BUSWIDTH, typename TYPES>
void 
BusController<BUSWIDTH, TYPES>::
PrintPendingTransactions() {
	typename std::map<transaction_type *, BusTlmGenericProtocol<TYPES> *>::iterator it;

	for(it = pending_transactions.begin(); it != pending_transactions.end(); it++) {
		cerr << "\t" << it->first << "\t" << it->second << endl;
	}

}

template<unsigned int BUSWIDTH, typename TYPES>
void
BusController<BUSWIDTH, TYPES>::
AddPendingTransaction(typename TYPES::tlm_payload_type &trans, BusTlmGenericProtocol<TYPES> *item) {
	assert(pending_transactions.find(&trans) == pending_transactions.end());
	cerr << GetName() << ": adding pending transaction " << &trans << endl;
	pending_transactions[&trans] = item;
	PrintPendingTransactions();
}

template<unsigned int BUSWIDTH, typename TYPES>
void
BusController<BUSWIDTH, TYPES>::
RemovePendingTransaction(typename TYPES::tlm_payload_type &trans) {
	typename std::map<transaction_type *, BusTlmGenericProtocol<TYPES> *>::iterator it;
	cerr << GetName() << ": removing pending transaction " << &trans << endl;
	PrintPendingTransactions();
	it = pending_transactions.find(&trans);
	assert(it != pending_transactions.end());
	pending_transactions.erase(it);
}

/*************************************************************************
 * Dispatcher methods and variables                                  END *
 *************************************************************************/

} // end of namespace bus
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_BUS_BUS_CONTROLLER_TCC___

