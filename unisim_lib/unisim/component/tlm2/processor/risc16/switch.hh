#ifndef __switch__
#define __switch__

//#define SC_INCLUDE_DYNAMIC_PROCESSES
#include <systemc>
#include <iostream>
#include <tlm.h>
#include <tlm_utils/simple_initiator_socket.h>
#include <tlm_utils/simple_target_socket.h>

#include <unisim/kernel/service/service.hh>
#include "unisim/service/interfaces/Fault_Injector.hh"
#include <unisim/service/Fault/BFI.hh>

using namespace sc_core;
using namespace sc_dt;
using namespace std;

using unisim::kernel::service::ParameterArray;
using unisim::kernel::service::Object;
using unisim::kernel::service::ServiceImport;
using unisim::kernel::service::Client;
using unisim::service::interfaces::Fault_Injector;
using unisim::service::interfaces::BFault_Injector;
using unisim::service::fault::Bus_Fault_Injector;
using unisim::kernel::service::Parameter;

namespace unisim{
namespace component{
namespace tlm2{
namespace processor{
namespace risc16{


template <class PHYSICAL_ADDR, unsigned int N_TARGETS>
struct Router:
		public sc_module
		, public tlm::tlm_bw_transport_if<>
		, public tlm::tlm_fw_transport_if<>
		, public Client<BFault_Injector<PHYSICAL_ADDR> >
{
	tlm::tlm_target_socket<> target_socket;

	tlm::tlm_initiator_socket<> initiator_socket[N_TARGETS];

//	ServiceImport<unisim::service::interfaces::Router > router_import;
	ServiceImport<BFault_Injector<PHYSICAL_ADDR> > router_import;

	Router(sc_module_name name, Object* parent=0);


	virtual ~Router();



	virtual void b_transport( tlm::tlm_generic_payload& trans, sc_time& delay );
	int decode_address( sc_dt::uint64 address);



	virtual tlm::tlm_sync_enum nb_transport_fw(tlm::tlm_generic_payload& trans, tlm::tlm_phase& phase, sc_time& delay);

	virtual tlm::tlm_sync_enum nb_transport_bw(tlm::tlm_generic_payload& trans, tlm::tlm_phase& phase, sc_time& delay);

	virtual unsigned int transport_dbg(tlm::tlm_generic_payload& trans);

	virtual bool get_direct_mem_ptr(tlm::tlm_generic_payload& trans,tlm::tlm_dmi&  dmi_data);

	virtual void invalidate_direct_mem_ptr(sc_dt::uint64 start_range,sc_dt::uint64 end_range);


private:
	uint16_t low_address[N_TARGETS];
	ParameterArray<uint16_t> param_low_address;
	uint16_t high_address[N_TARGETS];
	ParameterArray<uint16_t> param_high_address;
	std::ofstream trace_fault;




};


}
}
}
}
}

#endif

