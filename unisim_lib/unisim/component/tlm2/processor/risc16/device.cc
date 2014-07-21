/*
 * can.cc
 *
 *  Created on: 4 juil. 2014
 *      Author: houssem
 */

#include "systemc.h"
#include <unisim/component/tlm2/processor/risc16/device.hh>

namespace unisim{
namespace component{
namespace tlm2{
namespace processor{
namespace risc16{

using namespace sc_core;


sc_time delay;
tlm::tlm_phase phase;
sc_event interrupt_req;
tlm::tlm_sync_enum status;
bool has_interrupt;

Device::Device(sc_module_name name, Object *parent):
		Object(name, parent)
		, sc_module(name)
		, IRQ_socket("IRQ_socket")
		, device_rout_socket("device_rout_socket")
		,IRQ(0)
		,vect(0)
		,param_vect("vect",this, vect)

		{
			IRQ_socket(*this);
			device_rout_socket(*this);

			SC_HAS_PROCESS(Device);
			SC_THREAD(process);

		}

Device::~Device()
{}

void Device::process()
{
	//TODO: generate the interruption

	while(true)
	{
		wait(10, SC_NS);

		std::cout << "vector  " << std::hex << vect << std::dec << std::endl;

		IRQ = new tlm::tlm_generic_payload;
		IRQ->set_data_ptr(reinterpret_cast<unsigned char*>(&vect));
		IRQ->set_data_length(2);

		std::cout << "interruption request " << std::endl;

		has_interrupt = true;

		std::cout<<"calling b_transport interface by IRQ_socket"<<std::endl;

		IRQ_socket->b_transport(*IRQ, delay);



	}


}




/******************bw_interfaces_implemantation*************/

tlm::tlm_sync_enum Device::nb_transport_bw(tlm::tlm_generic_payload& trans, tlm::tlm_phase& phase, sc_time& delay)
{
	if (phase == tlm::BEGIN_RESP)
	{

	}
	return tlm::TLM_ACCEPTED;

}

void Device::invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range)
{

}

/******************fw_interfaces_implementation*************/

tlm::tlm_sync_enum Device::nb_transport_fw(tlm::tlm_generic_payload& trans,tlm::tlm_phase& phase, sc_time& delay)
{
	bool IF;



	IF = true;

			if(IF)
			{
				// TODO: BEG_REQ.........END RESP
			}

	return tlm::TLM_ACCEPTED;
}

bool Device::get_direct_mem_ptr(tlm::tlm_generic_payload& trans, tlm::tlm_dmi& dmi_data)
{
	return false;
}

void Device::b_transport(tlm::tlm_generic_payload& trans, sc_time& delay)
{

}

unsigned int Device::transport_dbg(tlm::tlm_generic_payload& trans)
{
	return 0;
}


}// end of risc16
}// end of processor
}// end of tlm2
}// end of component
}// end of unisim


