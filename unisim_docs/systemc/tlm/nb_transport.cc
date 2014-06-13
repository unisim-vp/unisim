namespace tlm {

enum tlm_phase { BEGIN_REQ, END_REQ, 
	BEGIN_RESP, END_RESP };
enum tlm_sync_enum { TLM_REJECTED = 0, TLM_ACCEPTED = 1, 
	TLM_UPDATED = 2, TLM_COMPLETED = 3 };

template <typename TRANS = tlm_generic_payload, typename PHASE = tlm_phase>
class tlm_nonblocking_transport_if : public virtual sc_core::sc_interface {
public:
	virtual tlm_sync_enum nb_transport(TRANS& trans, PHASE& phase, sc_core::sc_time& t) = 0;
};

} // namespace tlm
