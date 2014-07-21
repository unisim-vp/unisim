namespace tlm {

// Defined in tlm_helpers.h
enum tlm_endianness {TLM_UNKNOWN_ENDIAN, TLM_LITTLE_ENDIAN, TLM_BIG_ENDIAN};

class tlm_dmi_mode {
public:
	enum Type { READ = 0x1, WRITE = 0x2, READ_WRITE = READ|WRITE };
	Type type;
};

class tlm_dmi {
public:
	tlm_dmi() { init(); }
	void init();

	unsigned char* dmi_ptr;
	sc_dt::uint64 dmi_start_address;
	sc_dt::uint64 dmi_end_address;
	sc_core::sc_time read_latency;
	sc_core::sc_time write_latency;
	tlm_endianness endianness;
};

template <typename DMI_MODE = tlm_dmi_mode>
class tlm_fw_direct_mem_if : public virtual sc_core::sc_interface {
public:
	virtual bool get_direct_mem_ptr( const sc_dt::uint64& address,
			DMI_MODE& dmi_mode, tlm_dmi& dmi_data) = 0;
};

class tlm_bw_direct_mem_if : public virtual sc_core::sc_interface {
public:
	virtual void invalidate_direct_mem_ptr(
			sc_dt::uint64 start_range, 
			sc_dt::uint64 end_range) = 0;
};

} // namespace tlm
