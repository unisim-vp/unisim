//#define SC_INCLUDE_DYNAMIC_PROCESSES

#ifndef __initCPU_tlm2__
#define __initCPU_tlm2__

#include <systemc>
#include <iostream>
#include <tlm.h>
#include <tlm_utils/simple_initiator_socket.h>
#include <tlm_utils/simple_target_socket.h>
#include <unisim/component/cxx/processor/risc16/initCPU.hh>


using namespace sc_core;
using namespace sc_dt;
using namespace std;

using unisim::kernel::service::Object;

namespace unisim{
namespace component{
namespace tlm2{
namespace processor{
namespace risc16{




struct Initiator:  public sc_module, public CPU, public tlm::tlm_bw_transport_if<>, public tlm::tlm_fw_transport_if<>
{
	tlm::tlm_initiator_socket<> initsocket;
	tlm::tlm_target_socket<> targsocket;

	int i;
	int data;


	Initiator(sc_module_name name, Object *_parent=0);
	virtual ~Initiator() { }

	void process();

	virtual tlm::tlm_sync_enum nb_transport_bw(tlm::tlm_generic_payload& trans, tlm::tlm_phase& phase, sc_time& delay)
	{
		return tlm::TLM_ACCEPTED;
	}



	virtual void invalidate_direct_mem_ptr(sc_dt::uint64 start_range,
			sc_dt::uint64 end_range)
	{

	}




	uint16_t fetch(uint16_t addr);

	void mem_write16(uint16_t addr,uint16_t data);
	void tlm_write(uint16_t addr,unsigned char* data, uint16_t size );

	virtual uint16_t mem_read16(uint16_t addr);
	uint16_t tlm_read(uint16_t addr);

	/*****************fw_interfaces****************/

	    virtual tlm::tlm_sync_enum nb_transport_fw(tlm::tlm_generic_payload& trans, tlm::tlm_phase& phase,sc_time& delay);
	    virtual void b_transport(tlm::tlm_generic_payload& trans, sc_core::sc_time& delay);
		virtual bool get_direct_mem_ptr(tlm::tlm_generic_payload& trans, tlm::tlm_dmi& dmi_data);
		virtual unsigned int transport_dbg(tlm::tlm_generic_payload& trans);




	//		        if (trans->is_response_error())
	//		        	SC_REPORT_ERROR("TLM-2", "Response error from b_transport");


//	uint16_t entry_point;
//	Parameter<uint16_t> param_entry_point;




};




}
}
}
}
}

#endif
