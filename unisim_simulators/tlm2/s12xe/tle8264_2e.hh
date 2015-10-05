#ifndef _TLE8264_2E_HH_
#define _TLE8264_2E_HH_

#include "can_stub.hh"
#include <systemc>
#include <inttypes.h>

#include <iostream>
#include <queue>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libxml/xmlmemory.h>
#include <libxml/xpath.h>
#include <libxml/parser.h>

#include <tlm.h>
#include <tlm_utils/tlm_quantumkeeper.h>
#include <tlm_utils/peq_with_get.h>
#include "tlm_utils/simple_initiator_socket.h"
#include "tlm_utils/simple_target_socket.h"

#include <unisim/kernel/service/service.hh>

#include <unisim/component/cxx/processor/hcs12x/config.hh>
#include <unisim/component/tlm2/processor/hcs12x/tlm_types.hh>

using namespace std;

using namespace sc_core;
using namespace sc_dt;
using namespace tlm;
using namespace tlm_utils;

using unisim::kernel::service::Object;
using unisim::kernel::service::Parameter;
using unisim::kernel::service::Signal;
using unisim::kernel::service::ServiceExportBase;

using unisim::component::cxx::processor::hcs12x::CONFIG;

using unisim::component::tlm2::processor::hcs12x::XINT_Payload;
using unisim::component::tlm2::processor::hcs12x::XINT_REQ_ProtocolTypes;

using unisim::component::tlm2::processor::hcs12x::CAN_Payload;
//using unisim::component::tlm2::processor::hcs12x::CAN_DATATYPE;
//using unisim::component::tlm2::processor::hcs12x::CAN_DATATYPE_ARRAY;
using unisim::component::tlm2::processor::hcs12x::UNISIM_CAN_ProtocolTypes;

using unisim::kernel::tlm2::PayloadFabric;

class TLE8264_2E :
		public Object
	, public sc_module

	, virtual public tlm_bw_transport_if<XINT_REQ_ProtocolTypes>
	, virtual public tlm_fw_transport_if<UNISIM_CAN_ProtocolTypes >
	, virtual public tlm_bw_transport_if<UNISIM_CAN_ProtocolTypes >
{
public:
	static const unsigned int INIT=0b000;
	static const unsigned int RESTART=0b001;
	static const unsigned int SWFLASH=0b010;
	static const unsigned int NORMAL=0b011;
	static const unsigned int SLEEP=0b100;
	static const unsigned int STOP=0b101;
	static const unsigned int FAILSAFE=0b110;
	static const unsigned int READONLY=0b111;

	//		static const uint16_t mask		= 0b00000001 00000000;

	// reg 0b000
	static const uint16_t WKCAN		= 0b0000000000000001;
	static const uint16_t WKLIN1	= 0b0000000000000010;
	static const uint16_t WKLIN2	= 0b0000000000000100;
	static const uint16_t WKLIN3	= 0b0000000000001000;
	static const uint16_t WK0PIN	= 0b0000000000010000;
	static const uint16_t WK1PIN	= 0b0000000000100000;
	static const uint16_t WKCYCLIC	= 0b0000000001000000;
	static const uint16_t INT		= 0b0000000100000000;

	// reg 0b001
	static const uint16_t RESET		= 0b0000000010000000;
	static const uint16_t FAILSPI	= 0b0000000001000000;

	// reg 0b100
	static const uint16_t LIMPHOME	= 0b0000000001000000;
	static const uint16_t RESETDELAY= 0b0000000000000100;

	// reg 0b110
	static const uint16_t CHKSUM	= 0b0000000100000000;
	static const uint16_t WDONOFF	= 0b0000000010000000;
	static const uint16_t TIOUTWIN	= 0b0000000001000000;
	static const uint16_t SETTO1	= 0b0000000000100000;
	static const uint16_t TIMEBITS	= 0b0000000000011111;

	static const uint16_t REG_OOO_DEFAULT = 0b000000000; //input=0b000111111  ; output=0b000000000
	static const uint16_t REG_001_DEFAULT = 0b000000000; //input=0b111111111  ; output=0b000000000
	static const uint16_t REG_010_DEFAULT = 0b000000000; //input=0b101010110  ; output=0b000000000
	static const uint16_t REG_011_DEFAULT = 0b000000000; //input=0b000000000  ; output=0b000000000  reserved register
	static const uint16_t REG_100_DEFAULT = 0b100010101; //input=0b100010101  ; output=0b100010101
	static const uint16_t REG_101_DEFAULT = 0b100000000; //input=0b100000000  ; output=0b100000000
	static const uint16_t REG_110_DEFAULT = 0b111111111; //input=0b111111111  ; output=0b111111111
	static const uint16_t REG_111_DEFAULT = 0b000000000; //input=0b000000000  ; output=0b000000000  reserved register

	tlm_initiator_socket<CONFIG::EXTERNAL2UNISIM_BUS_WIDTH, XINT_REQ_ProtocolTypes> interrupt_request;

	tlm_utils::simple_target_socket<TLE8264_2E> bus_clock_socket;

	tlm_target_socket<CAN_BUS_WIDTH, UNISIM_CAN_ProtocolTypes > can_slave_rx_sock;
	tlm_initiator_socket<CAN_BUS_WIDTH, UNISIM_CAN_ProtocolTypes > can_slave_tx_sock;

//	tlm_target_socket<CAN_BUS_WIDTH, UNISIM_CAN_ProtocolTypes > init_rx_sock;
//	tlm_initiator_socket<CAN_BUS_WIDTH, UNISIM_CAN_ProtocolTypes > init_tx_sock;

	tlm_utils::simple_initiator_socket<TLE8264_2E> spi_tx_socket;
	tlm_utils::simple_target_socket<TLE8264_2E> spi_rx_socket;

	TLE8264_2E(const sc_module_name& name, Object *parent = 0);
	~TLE8264_2E();

	virtual bool BeginSetup();
	virtual bool Setup(ServiceExportBase *srv_export);
	virtual bool EndSetup();

	virtual void OnDisconnect();
	virtual void Reset();

	virtual void Stop(int exit_status);

	bool isTerminated() { return terminated; }
	void ComputeInternalTime();

	void spi_rx_b_transport(tlm::tlm_generic_payload& payload, sc_core::sc_time& t);

	// Slave methods
	virtual tlm_sync_enum nb_transport_bw( XINT_Payload& payload, tlm_phase& phase, sc_core::sc_time& t);
    void updateBusClock(tlm::tlm_generic_payload& trans, sc_time& delay);

	virtual bool get_direct_mem_ptr( CAN_Payload& payload, tlm_dmi&  dmi_data);
	virtual unsigned int transport_dbg( CAN_Payload& payload);

	virtual tlm_sync_enum nb_transport_fw( CAN_Payload& payload, tlm_phase& phase, sc_core::sc_time& t);
	virtual void b_transport( CAN_Payload& payload, sc_core::sc_time& t);
	// Master methods
	virtual tlm_sync_enum nb_transport_bw( CAN_Payload& payload, tlm_phase& phase, sc_core::sc_time& t);
	virtual void invalidate_direct_mem_ptr( sc_dt::uint64 start_range, sc_dt::uint64 end_range);


	void setLimpHome(bool val);
	bool isLimpHome();

	bool isWatchDogEnable();
	bool isINT();
	bool isWKCyclicEnabled();

	void WatchDogThread();
	void refresh_watchdog();

	void enter_init();

	void enter_restart();
	void enter_swflash();
	void enter_normal();
	void enter_sleep();
	void enter_stop();
	void enter_sailsafe();

	void assertInterrupt(uint8_t int_offset);

	void assert_int_interrup();
	void assert_reset_interrupt();

	void triggerStateMachine(uint16_t spi_cmd);
	uint16_t setCmd(uint16_t cmd);

	uint16_t write(uint16_t sel, uint16_t val);
	uint16_t read(uint16_t sel, uint16_t& val);

private:
	peq_with_get<CAN_Payload > rx_payload_queue;

	PayloadFabric<CAN_Payload > payload_fabric;

	tlm_quantumkeeper quantumkeeper;
	PayloadFabric<XINT_Payload> xint_payload_fabric;

	XINT_Payload *xint_payload;

	PayloadFabric<tlm::tlm_generic_payload> spi_payload_fabric;

	double	bus_cycle_time_int;
	Parameter<double>	param_bus_cycle_time_int;
	sc_time		bus_cycle_time;


	uint16_t current_mode;
	uint16_t current_cmd;

	uint16_t registers[8]; // 9-bits by register
	bool wd_refresh;
	bool SPIWake_read;

	bool terminated;

	sc_time frame_time;
	sc_event watchdog_enable_event;
	sc_event can_bw_event;

	bool limp_home;
	Signal<bool> sig_limp_home;

	// Interrupt ID
	uint8_t reset_interrupt;
	Parameter<uint8_t> param_reset_interrupt;
	uint8_t int_interrupt;
	Parameter<uint8_t> param_int_interrupt;

};

#endif // _TLE8264_2E_HH_

