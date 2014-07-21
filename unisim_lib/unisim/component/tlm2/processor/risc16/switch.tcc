/*
 * switch.tcc
 *
 *  Created on: 11 juil. 2014
 *      Author: houssem
 */
#include <unisim/component/tlm2/processor/risc16/switch.hh>
#include "systemc.h"
#include "tlm.h"
#include <bitset>


namespace unisim{
namespace component{
namespace tlm2{
namespace processor{
namespace risc16{

using namespace sc_core;
using namespace std;
using unisim::component::tlm2::processor::risc16::Router;

template <class PHYSICAL_ADDR, unsigned int N_TARGETS>
Router<PHYSICAL_ADDR, N_TARGETS>::Router(sc_module_name name, Object* parent) :
 Object(name, parent)
, sc_module(name)
, Client<BFault_Injector<PHYSICAL_ADDR> > (name, parent)
, target_socket("target_socket")
, router_import("router_import", this)
, param_low_address("low-address", this, low_address, N_TARGETS, "Module low address")
, param_high_address("high-address", this, high_address, N_TARGETS, "Module high address")

{
	target_socket(*this);

	for (unsigned int i = 0; i < N_TARGETS; i++) {
		initiator_socket[i](*this);
	}

	trace_fault.open("trace-fault.txt", ios::out);
}

template <class PHYSICAL_ADDR, unsigned int N_TARGETS>
Router<PHYSICAL_ADDR, N_TARGETS>::~Router()
{
	trace_fault.close();
}

template <class PHYSICAL_ADDR, unsigned int N_TARGETS>
void Router<PHYSICAL_ADDR, N_TARGETS>::b_transport( tlm::tlm_generic_payload& trans, sc_time& delay )
{

	sc_dt::uint64 address = trans.get_address();

/////
	std::cout << " address is " <<  std::hex << address << std::endl;
/////

	uint8_t* data =(uint8_t*)trans.get_data_ptr();

/////
	std::cout << " data is " << (bitset<8>*) data << std::endl;
////

	unsigned int lenght = trans.get_data_length();

	trace_fault << " initial  address :  0x" << std::hex  <<  address << " initial data  " << std::hex <<  (bitset<8>*)data << endl;

	router_import->fault_injection(data, address, lenght);

//	((unisim::service::fault::Bus_Fault_Injector<PHYSICAL_ADDR>*) (unisim::service::interfaces::Fault_Injector<PHYSICAL_ADDR>*) router_import)->fault_injection(data, address, lenght);

	trace_fault << " address :  0x" << std::hex  <<  address << "  data  " << std::hex <<  (bitset<8>*)data << endl;


	int target_nr = decode_address( address);

	if (target_nr == -1) {
		trans.set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
	} else {
		trans.set_address( address );


		( initiator_socket[target_nr] )->b_transport( trans, delay );

	}
}


template <class PHYSICAL_ADDR, unsigned int N_TARGETS>
 int Router<PHYSICAL_ADDR, N_TARGETS>::decode_address( sc_dt::uint64 address)
{
	for (unsigned int target_nr=0; target_nr < N_TARGETS; target_nr++) {
		if ((address >= low_address[target_nr]) && (address <= high_address[target_nr])) {
			return target_nr;
		}
	}

	return -1;
}

template <class PHYSICAL_ADDR, unsigned int N_TARGETS>
tlm::tlm_sync_enum Router<PHYSICAL_ADDR, N_TARGETS>::nb_transport_fw(tlm::tlm_generic_payload& trans, tlm::tlm_phase& phase, sc_time& delay)
{
	return tlm::TLM_ACCEPTED;
}

template <class PHYSICAL_ADDR, unsigned int N_TARGETS>
tlm::tlm_sync_enum Router<PHYSICAL_ADDR, N_TARGETS>::nb_transport_bw(tlm::tlm_generic_payload& trans, tlm::tlm_phase& phase, sc_time& delay)
{
	return tlm::TLM_ACCEPTED;
}

template <class PHYSICAL_ADDR, unsigned int N_TARGETS>
unsigned int Router<PHYSICAL_ADDR, N_TARGETS>::transport_dbg(tlm::tlm_generic_payload& trans)
{
	return 0;
}

template <class PHYSICAL_ADDR, unsigned int N_TARGETS>
bool Router<PHYSICAL_ADDR, N_TARGETS>::get_direct_mem_ptr(tlm::tlm_generic_payload& trans,tlm::tlm_dmi&  dmi_data)
{
	return false;
}

template <class PHYSICAL_ADDR, unsigned int N_TARGETS>
void Router<PHYSICAL_ADDR, N_TARGETS>::invalidate_direct_mem_ptr(sc_dt::uint64 start_range,sc_dt::uint64 end_range)
{

}


}
}
}
}
}





