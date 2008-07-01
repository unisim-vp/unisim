#ifndef __UNISIM_COMPONENT_TLM2_BUS_BUS_CONTROLLER_HH__
#define __UNISIM_COMPONENT_TLM2_BUS_BUS_CONTROLLER_HH__

#include <systemc.h>
#include <tlm.h>
#include <tlm_utils/multi_passthrough_target_socket.h>
#include <tlm_utils/simple_initiator_socket.h>
#include "unisim/kernel/service/service.hh"
#include "unisim/component/tlm2/simple_bus/bus_types.hh"

namespace unisim {
namespace component {
namespace tlm2 {
namespace bus {

template<unsigned int BUSWIDTH = 32,
	typename TYPES = tlm::tlm_base_protocol_types>
class BusController : 
	public unisim::kernel::service::Object,
	public sc_module { //,
//	public tlm::tlm_bw_transport_if<TYPES>,
//	public tlm::tlm_fw_transport_if<TYPES> {
public:
	SC_HAS_PROCESS(BusController);
	BusController(const sc_module_name& name, unisim::kernel::service::Object *parent);
	~BusController();

	virtual bool Setup();

	tlm_utils::multi_passthrough_target_socket<BusController, BUSWIDTH, TYPES> targ_socket;
	tlm_utils::simple_initiator_socket<BusController, BUSWIDTH, TYPES> init_socket;

private:
	typedef typename TYPES::tlm_payload_type transaction_type;
	typedef typename TYPES::tlm_phase_type   phase_type;
	typedef tlm::tlm_sync_enum               sync_enum_type;

	/*************************************************************************
	 * Multi passtrough target socket callbacks                        START *
	 *************************************************************************/

	sync_enum_type	T_nb_transport_fw_cb(int id, transaction_type &trans, phase_type &phase, sc_core::sc_time &time);
	void			T_b_transport_cb(int id, transaction_type &trans, sc_core::sc_time &time);
	unsigned int	T_transport_dbg_cb(int id, transaction_type &trans);
	bool			T_get_direct_mem_ptr_cb(int id, transaction_type &trans, tlm::tlm_dmi &dmi);

	/*************************************************************************
	 * Multi passthrough target socket callbacks                         END *
	 *************************************************************************/

	/*************************************************************************
	 * Simple initiator socket callbacks                               START *
	 *************************************************************************/
	
	sync_enum_type	I_nb_transport_bw_cb(transaction_type &trans, phase_type &phase, sc_core::sc_time &time);
	void			I_invalidate_direct_mem_ptr_cb(sc_dt::uint64 start_range, sc_dt::uint64 end_range);

	/*************************************************************************
	 * Simple initiator socket callbacks                                 END *
	 *************************************************************************/

	BusPeq<TYPES> peq;
	BusPeq<TYPES> free_peq;
	void Dispatcher();
	void BusSynchronize();

	/*************************************************************************
	 * Parameters                                                      START *
	 *************************************************************************/

	sc_time bus_cycle_time;
	double bus_cycle_time_double;
	unisim::kernel::service::Parameter<double> param_bus_cycle_time_double;

	/*************************************************************************
	 * Parameters                                                        END *
	 *************************************************************************/

};

} // end of namespace bus
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_BUS_BUS_CONTROLLER_HH__
