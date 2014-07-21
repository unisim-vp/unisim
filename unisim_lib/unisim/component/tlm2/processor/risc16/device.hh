/*
 * can.hh
 *
 *  Created on: 4 juil. 2014
 *      Author: houssem
 */

#ifndef CAN_HH_
#define CAN_HH_

#include <systemc>
#include <inttypes.h>
#include <tlm.h>
#include <unisim/kernel/service/service.hh>


namespace unisim{
namespace component{
namespace tlm2{
namespace processor{
namespace risc16{

using namespace sc_core;
using namespace sc_dt;
using unisim::kernel::service::Object;
using unisim::kernel::service::Service;
using unisim::kernel::service::ServiceImport;
using unisim::kernel::service::ServiceExport;
using unisim::kernel::service::Parameter;

typedef uint32_t physical_address_t;

class Device: public sc_module, public Object
, public tlm::tlm_fw_transport_if<>
, public tlm::tlm_bw_transport_if<>
{
public:

	tlm::tlm_initiator_socket<> IRQ_socket;
	// TODO::you can use also a genric payload as an interup & connect INT_socket to cpu

	tlm::tlm_target_socket<> device_rout_socket;



	Device(sc_module_name name, Object *parent=0);
	virtual ~Device();

	tlm::tlm_generic_payload* IRQ;

	void process();


	/*****************fw_interfaces****************/

    virtual tlm::tlm_sync_enum nb_transport_fw(tlm::tlm_generic_payload& trans, tlm::tlm_phase& phase,sc_time& delay);
    virtual void b_transport(tlm::tlm_generic_payload& trans, sc_core::sc_time& delay);
	virtual bool get_direct_mem_ptr(tlm::tlm_generic_payload& trans, tlm::tlm_dmi& dmi_data);
	virtual unsigned int transport_dbg(tlm::tlm_generic_payload& trans);


	/*****************bw_interfaces****************/

	virtual tlm::tlm_sync_enum nb_transport_bw(tlm::tlm_generic_payload& trans, tlm::tlm_phase& phase, sc_time& delay);
	virtual void invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range);


	uint16_t vect;
	Parameter<uint16_t> param_vect;

};






}// end of risc16
}// end of processor
}// end of tlm2
}// end of component
}// end of unisim


#endif /* CAN_HH_ */
