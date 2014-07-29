#ifndef __UTROUTER_HH__
#define __UTROUTER_HH__

#include <systemc.h>
#include <tlm.h>

class UTRouter :
	public sc_core::sc_module,
	public virtual tlm::tlm_bw_b_transport_if,
	public virtual tlm::tlm_fw_b_transport_if<> { 

public:
	tlm::tlm_b_target_socket<> in_socket;
	tlm::tlm_b_initiator_socket<> out_socket1;
	tlm::tlm_b_initiator_socket<> out_socket2;

	SC_HAS_PROCESS(UTRouter);
	UTRouter(sc_core::sc_module_name name) :
		sc_core::sc_module(name),
		in_socket("in_socket"),
		out_socket1("out_socket1"),
		out_socket2("out_socket2") {
			// bind the interfaceto the target socket
			in_socket(*this);
			out_socket1(*this);
			out_socket2(*this);
		}
	
	void b_transport(tlm::tlm_generic_payload &payload) {
		sc_dt::uint64 addr = payload.get_address();

		if(addr % 2) 
			out_socket2->b_transport(payload);
		else
			out_socket1->b_transport(payload);
	}

	unsigned int transport_dbg(tlm::tlm_debug_payload &payload) {
		sc_dt::uint64 addr = payload.address;

		if(addr % 2)
			return out_socket2->transport_dbg(payload);
		else
			return out_socket1->transport_dbg(payload);
	}

	bool get_direct_mem_ptr(const sc_dt::uint64 &address,
			tlm::tlm_dmi_mode &dmi_mode,
			tlm::tlm_dmi &dmi_data) {
		if(address % 2)
			return out_socket2->get_direct_mem_ptr(address, dmi_mode, dmi_data);
		else
			return out_socket1->get_direct_mem_ptr(address, dmi_mode, dmi_data);
	}

	void invalidate_direct_mem_ptr(sc_dt::uint64 start_range,
			sc_dt::uint64 end_range)
	{
		in_socket->invalidate_direct_mem_ptr(start_range, end_range);
	}

};

#endif // __UTROUTER_HH__

