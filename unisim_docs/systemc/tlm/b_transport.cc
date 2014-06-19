namespace tlm {
	
template <typename TRANS = tlm_generic_payload>
class tlm_blocking_transport_if : public virtual sc_core::sc_interface {
public:
	virtual void b_transport(TRANS& trans) = 0;
};

} // namespace tlm
