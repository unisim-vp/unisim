#include <unisim/component/tlm2/processor/risc16/initCPU_tlm2.hh>
#include <unisim/util/endian/endian.hh>

#include <bitset>

using namespace unisim::util::endian;

namespace unisim{
namespace component{
namespace tlm2{
namespace processor{
namespace risc16{



Initiator::Initiator(sc_module_name name, Object *_parent):
		Object(name, _parent, "")
	, sc_module(name)
	, CPU(name, _parent)
	, initsocket("initsocket")
	, targsocket("targsocket")
	, i(0)
	, data(0)
//    , entry_point(0)
//	, param_entry_point ("param_entry_point", this, entry_point)
{

	initsocket(*this);
	targsocket(*this);

	SC_HAS_PROCESS(Initiator);
	SC_THREAD(process);
}

void Initiator::process() {
//	const char* fname = "tt.bin";
//
//	generateBinary(fname);
//	mem_loader(fname, load_address );
//
////*****
//	entry_point = load_address + 4; // 4 ins de isr
////*****
//
//	cia = entry_point;

	while (true) {
		Step();
		wait(1, SC_NS);
	}

}

uint16_t Initiator::fetch(uint16_t addr) {
//	std::cout << "*** fetch ***" << std::endl;
	uint16_t valraw = mem_read16(addr);
	return valraw;
}

uint16_t Initiator::mem_read16(uint16_t addr)
{
	uint16_t valraw = tlm_read(addr);
	std::cout << "rawval " << std::hex << valraw << std::dec << std::endl;
	uint16_t val = BigEndian2Host(valraw);
	return val;
}


uint16_t Initiator::tlm_read(uint16_t addr)
{
	tlm::tlm_generic_payload* trans = new tlm::tlm_generic_payload;

	sc_time delay;
	uint16_t data;

	trans->set_command(tlm::TLM_READ_COMMAND);
	trans->set_address( addr );
	trans->set_data_ptr( reinterpret_cast<unsigned char*>(&data) );
	trans->set_data_length( 2 );
	//trans->set_streaming_width( 4 );
	//trans->set_byte_enable_ptr( 0 );
	//trans->set_dmi_allowed( false );
	trans->set_response_status( tlm::TLM_INCOMPLETE_RESPONSE );

	initsocket->b_transport(*trans, delay);

	delete trans; trans = NULL;

//	std::cout << "tlm_read:: " << (bitset<16>) data << std::endl;
	return data;
}

void Initiator::mem_write16(uint16_t addr,uint16_t data ) {

	uint16_t val = Host2BigEndian(data);

	std::cout << "write add " << std::hex << addr << "   data " << std::hex << data << "  val  " << std::hex << val << std::endl;

	tlm_write(addr, (unsigned char*) &val, 2 );

}

void Initiator::tlm_write(uint16_t addr,unsigned char* data, uint16_t size )
{

	tlm::tlm_generic_payload* trans = new tlm::tlm_generic_payload;

	sc_time delay;

	trans->set_command(tlm::TLM_WRITE_COMMAND);
	trans->set_address( addr );
	trans->set_data_ptr( data );
	trans->set_data_length( size );
	//trans->set_streaming_width( 4 );
	//trans->set_byte_enable_ptr( 0 );
	//trans->set_dmi_allowed( false );
	trans->set_response_status( tlm::TLM_INCOMPLETE_RESPONSE );

	initsocket->b_transport(*trans, delay);

	delete trans; trans = NULL;

}

void Initiator::b_transport(tlm::tlm_generic_payload& IRQ_receiver,sc_time& delay)
{

	received_vect= *((uint16_t*)IRQ_receiver.get_data_ptr());

	std::cout<<"interruption vector received"<< std::hex << received_vect << std::endl;

	has_interrupt = true;
}

/**************interfaces_implementation************//////

 tlm::tlm_sync_enum Initiator::nb_transport_fw(tlm::tlm_generic_payload& trans, tlm::tlm_phase& phase,sc_time& delay)
{
	return tlm::TLM_ACCEPTED;
}

bool Initiator::get_direct_mem_ptr(tlm::tlm_generic_payload& trans, tlm::tlm_dmi& dmi_data)

{
	return false;
}

unsigned int Initiator::transport_dbg(tlm::tlm_generic_payload& trans)

{
	return 0;
}



}
}
}
}
}

