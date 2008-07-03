#include <tlm_utils/peq_with_get.h>

namespace unisim {
namespace component {
namespace tlm2 {
namespace bus {

template<typename PAYLOAD_TYPE = tlm::tlm_generic_payload>
class BusTlmGenericPayload {
public:
	PAYLOAD_TYPE *payload;
	bool from_initiator;
	unsigned int id;
	bool send_end_req;
	bool send_end_resp;
};

template<typename TYPES = tlm::tlm_base_protocol_types>
class BusTlmBaseProtocolTypes {
public:
	typedef BusTlmGenericPayload<typename TYPES::tlm_payload_type> tlm_payload_type;
	typedef typename TYPES::tlm_phase_type tlm_phase_type;
};

template<typename TYPES = tlm::tlm_base_protocol_types>
class BusTlmGenericProtocol {
public:
	typename TYPES::tlm_payload_type *payload;
	bool from_initiator;
	unsigned int id;
	typename TYPES::tlm_phase_type phase;
	bool send_end_req;
	bool send_end_resp;
};

template<typename TYPES = tlm::tlm_base_protocol_types>
class BusPeq : public tlm_utils::peq_with_get<BusTlmGenericProtocol<TYPES> > {
public:
	BusPeq(const char *name) : tlm_utils::peq_with_get<BusTlmGenericProtocol<TYPES> >(name) {}
};

// template<typename OWNER, typename TYPES = BusTlmBaseProtocolTypes<> >
// class BusPqe : public peq_with_cb_and_phase<OWNER, TYPES> {
// public:
// 	BusPqe(OWNER *owner, cb callback) : peq_with_cb_and_phase<OWNER, TYPES>(owner, callback) {}
// 	BusPqe(const char *name, OWNER *owner, cb callback) : peq_with_cb_and_phase<OWNER, TYPES>(name, owner, callback) {}
// 	~BusPqe() : ~peq_with_cb_and_phase<OWNER, TYPES>() {}
// }

} // end of namespace simple_bus
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim
