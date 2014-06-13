struct Initiator1: 
	sc_module, tlm::tlm_bw_nb_transport_if<> // Loosely-timed initiator
{
	tlm::tlm_nb_initiator_socket<32> init_socket;

	tlm::tlm_quantumkeeper m_qk; // The quantum keeper

	SC_CTOR(Initiator1) : init_socket("init_socket") {
		SC_THREAD(T); // The initiator process
		init_socket.bind( *this ); // Initiator socket bound to the initiator itself
		m_qk.set_global_quantum( sc_time(1, SC_US) ); // Replace the global quantum
		m_qk.reset(); // Re-calculate the local quantum
	}

	void T() {
		tlm::tlm_generic_payload t;
		tlm::tlm_phase phase;
		t.set_command(tlm::TLM_WRITE_COMMAND);
		t.set_data_length(4);

		for (int i = 0; i < RUN_LENGTH; i += 4) {
			int word = i;
			t.set_address(i);
			t.set_data_ptr( (unsigned char*)(&word) );
			phase = tlm::BEGIN_REQ;
								// Annotate nb_transport with local time
			tlm::tlm_sync_enum status = init_socket->nb_transport(t, phase, m_qk.get_local_time() );
			switch (status) {
				...
			}
			
			m_qk.inc( sc_time(100, SC_NS) ); // Keep a tally of time consumed
			if ( m_qk.need_sync() ) m_qk.sync(); // Check local time against quantum
		}
	}
	...
};
