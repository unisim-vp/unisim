namespace tlm {

class tlm_debug_payload {
public:
	sc_dt::uint64 address;
	bool do_read;
	unsigned int num_bytes;
	unsigned char* data;
};

class tlm_transport_dbg_if : public virtual sc_core::sc_interface {
public:
	virtual unsigned int transport_dbg(tlm_debug_payload& r) = 0;
};

} // namespace tlm
