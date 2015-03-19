/*
 * s12mscan.cc
 *
 *  Created on: 5 mars 2015
 *      Author: rnouacer
 */

#include <unisim/component/tlm2/processor/hcs12x/s12mscan.hh>
#include <unisim/component/tlm2/processor/hcs12x/xint.hh>
#include <unisim/util/converter/convert.hh>

namespace unisim {
namespace component {
namespace tlm2 {
namespace processor {
namespace hcs12x {


S12MSCAN::S12MSCAN(const sc_module_name& name, Object *parent) :
	Object(name, parent)
	, sc_module(name)

	, unisim::kernel::service::Client<TrapReporting>(name, parent)
	, unisim::kernel::service::Client<CharIO>(name, parent)
	, unisim::kernel::service::Service<Memory<physical_address_t> >(name, parent)
	, unisim::kernel::service::Client<Memory<physical_address_t> >(name, parent)
	, unisim::kernel::service::Service<Registers>(name, parent)

	, trap_reporting_import("trap_reporting_import", this)
	, char_io_import("char_io_import", this)

	, memory_export("memory_export", this)
	, memory_import("memory_import", this)
	, registers_export("registers_export", this)

	, slave_socket("slave_socket")
	, bus_clock_socket("bus_clock_socket")

	, bus_cycle_time_int(250000)
	, param_bus_cycle_time_int("bus-cycle-time", this, bus_cycle_time_int)

	, oscillator_clock_value(250000)
	, param_oscillator_clock_int("oscillator-clock", this, oscillator_clock_value)

	, baseAddress(0x0140)
	, param_baseAddress("base-address", this, baseAddress)

	, transmit_interrupt_offset(0xB0)
	, param_transmit_interrupt_offset("transmit-interrupt-offset", this, transmit_interrupt_offset)
	, receive_interrupt_offset(0xB2)
	, param_receive_interrupt_offset("receive-interrupt-offset", this, receive_interrupt_offset)
	, errors_interrupt_offset(0xB4)
	, param_errors_interrupt_offset("errors-interrupt-offset", this, errors_interrupt_offset)
	, wakeup_interrupt_offset(0xB6)
	, param_wakeup_interrupt_offset("wakeup-interrupt-offset", this, wakeup_interrupt_offset)

	, tx_debug_enabled(false)
	, param_tx_debug_enabled("tx-debug-enabled", this, tx_debug_enabled)
	, rx_debug_enabled(false)
	, param_rx_debug_enabled("rx-debug-enabled", this, rx_debug_enabled)

	, idle_to_send(false)

	, txd(true)
	, txd_output_pin("TXD", this, txd, "TXD output")
	, rxd(true)
	, rxd_input_pin("RXD", this, rxd, "RXD input")

	, canctl0_register(0x01)
	, canctl1_register(0x11)
	, canbtr0_register(0x00)
	, canbtr1_register(0x00)
	, canrflg_register(0x00)
	, canrier_register(0x00)
	, cantflg_register(0x07)
	, cantier_register(0x00)
	, cantarq_register(0x00)
	, cantaak_register(0x00)
	, cantbsel_register(0x00)
	, canidac_register(0x00)
	, reserved1(0x00)
	, canmisc_register(0x00)
	, canrxerr_register(0x00)
	, cantxerr_register(0x00)
	, canidar0_register(0x00)
	, canidar1_register(0x00)
	, canidar2_register(0x00)
	, canidar3_register(0x00)
	, canidmr0_register(0x00)
	, canidmr1_register(0x00)
	, canidmr2_register(0x00)
	, canidmr3_register(0x00)
	, canidar4_register(0x00)
	, canidar5_register(0x00)
	, canidar6_register(0x00)
	, canidar7_register(0x00)
	, canidmr4_register(0x00)
	, canidmr5_register(0x00)
	, canidmr6_register(0x00)
	, canidmr7_register(0x00)

	, active_canrxfg_index(0)
	, active_cantxfg_index(0)

	, telnet_enabled(false)
	, param_telnet_enabled("telnet-enabled", this, telnet_enabled)

	, logger(*this)

{

	param_oscillator_clock_int.SetFormat(unisim::kernel::service::VariableBase::FMT_DEC);

	interrupt_request(*this);

	slave_socket.register_b_transport(this, &S12MSCAN::read_write);
	bus_clock_socket.register_b_transport(this, &S12MSCAN::updateBusClock);

	txd_output_pin.SetMutable(true);
	rxd_input_pin.SetMutable(true);

	SC_HAS_PROCESS(S12MSCAN);

	SC_THREAD(RunRx);

	SC_THREAD(RunTx);

	SC_THREAD(TelnetProcessInput);

	xint_payload = xint_payload_fabric.allocate();

	for (int i=0; i<5; i++) {
		for (int j=0; j<16; j++) {
			canrxfg_register[i][j] = 0x00;
		}
	}

	for (int i=0; i<3; i++) {
		for (int j=0; j<16; j++) {
			cantxfg_register[i][j] = 0x00;
		}
	}

	for (int i=0; i<16; i++) {
		tx_shift_register[i] = 0x00;
		rx_shift_register[i] = 0x00;
	}

}


S12MSCAN::~S12MSCAN() {

	xint_payload->release();

	// Release registers_registry
	map<string, unisim::util::debug::Register *>::iterator reg_iter;

	for(reg_iter = registers_registry.begin(); reg_iter != registers_registry.end(); reg_iter++)
	{
		if(reg_iter->second)
			delete reg_iter->second;
	}

	registers_registry.clear();

	unsigned int i;
	unsigned int n = extended_registers_registry.size();
	for (i=0; i<n; i++) {
		delete extended_registers_registry[i];
	}

}

void S12MSCAN::RunRx() {

	uint8_t idleCounter = 0;
	uint8_t breakCounter = 0;


	while (true) {

		while (!isReceiverEnabled()) {

			wait(rx_run_event);

			idleCounter = 0;
			breakCounter = 0;

		}


	}

}

void S12MSCAN::RunTx() {

	while (true) {

		while (!isTransmitterEnabled()) {

			wait(tx_run_event);

		}

	}

}


void S12MSCAN::read_write( tlm::tlm_generic_payload& trans, sc_time& delay )
{
	tlm::tlm_command cmd = trans.get_command();
	sc_dt::uint64 address = trans.get_address();
	uint8_t* data_ptr = (uint8_t *)trans.get_data_ptr();
	unsigned int data_length = trans.get_data_length();

	if ((address >= baseAddress) && (address < (baseAddress + MEMORY_MAP_SIZE))) {

		if (cmd == tlm::TLM_READ_COMMAND) {
			memset(data_ptr, 0, data_length);

			read(address - baseAddress, data_ptr, data_length);
		} else if (cmd == tlm::TLM_WRITE_COMMAND) {

			write(address - baseAddress, data_ptr, data_length);
		}

		trans.set_response_status( tlm::TLM_OK_RESPONSE );

	} else {
		trans.set_response_status( tlm::TLM_INCOMPLETE_RESPONSE );
	}

}

//=====================================================================
//=             registers setters and getters                         =
//=====================================================================

bool S12MSCAN::read(unsigned int offset, const void *buffer, unsigned int data_length) {

	switch (offset) {
		case CANCTL0: {
			*((uint8_t *) buffer) = canctl0_register;
		} break;
		case CANCTL1: {
			*((uint8_t *) buffer) = canctl1_register;
		} break;
		case CANBTR0: {
			*((uint8_t *) buffer) = canbtr0_register;
		} break;
		case CANBTR1: {
			*((uint8_t *) buffer) = canbtr1_register;
		} break;
		case CANRFLG: {
			*((uint8_t *) buffer) = canrflg_register;
		} break;
		case CANRIER: {
			*((uint8_t *) buffer) = canrier_register;
		} break;
		case CANTFLG: {
			*((uint8_t *) buffer) = cantflg_register;
		} break;
		case CANTIER: {
			*((uint8_t *) buffer) = cantier_register;
		} break;
		case CANTARQ: {
			*((uint8_t *) buffer) = cantarq_register;
		} break;
		case CANTAAK: {
			*((uint8_t *) buffer) = cantaak_register;
		} break;
		case CANTBSEL: {
			*((uint8_t *) buffer) = 0;
			// Read: Find the lowest ordered bit set to 1, all other bits will be read as 0
			uint8_t mask = 1;
			for (int i=0; i<3; i++) {
				if ((cantbsel_register & mask) != 0) {
					*((uint8_t *) buffer) = cantbsel_register & mask;
					break;
				} else {
					mask = mask << 1;
				}
			}

		} break;
		case CANIDAC: {
			*((uint8_t *) buffer) = canidac_register;
		} break;
		case RESERVED1: {
			*((uint8_t *) buffer) = 0x00;
		} break;
		case CANMISC: {
			*((uint8_t *) buffer) = canmisc_register;
		} break;
		case CANRXERR: {
			*((uint8_t *) buffer) = canrxerr_register;
		} break;
		case CANTXERR: {
			*((uint8_t *) buffer) = cantxerr_register;
		} break;
		case CANIDAR0: {
			*((uint8_t *) buffer) = canidar0_register;
		} break;
		case CANIDAR1: {
			*((uint8_t *) buffer) = canidar1_register;
		} break;
		case CANIDAR2: {
			*((uint8_t *) buffer) = canidar2_register;
		} break;
		case CANIDAR3: {
			*((uint8_t *) buffer) = canidar3_register;
		} break;
		case CANIDMR0: {
			*((uint8_t *) buffer) = canidmr0_register;
		} break;
		case CANIDMR1: {
			*((uint8_t *) buffer) = canidmr1_register;
		} break;
		case CANIDMR2: {
			*((uint8_t *) buffer) = canidmr2_register;
		} break;
		case CANIDMR3: {
			*((uint8_t *) buffer) = canidmr3_register;
		} break;
		case CANIDAR4: {
			*((uint8_t *) buffer) = canidar4_register;
		} break;
		case CANIDAR5: {
			*((uint8_t *) buffer) = canidar5_register;
		} break;
		case CANIDAR6: {
			*((uint8_t *) buffer) = canidar6_register;
		} break;
		case CANIDAR7: {
			*((uint8_t *) buffer) = canidar7_register;
		} break;
		case CANIDMR4: {
			*((uint8_t *) buffer) = canidmr4_register;
		} break;
		case CANIDMR5: {
			*((uint8_t *) buffer) = canidmr5_register;
		} break;
		case CANIDMR6: {
			*((uint8_t *) buffer) = canidmr6_register;
		} break;
		case CANIDMR7: {
			*((uint8_t *) buffer) = canidmr7_register;
		} break;

		default: {
			if ((offset >= CANRXFG_START) && (offset <= CANRXFG_END)) {
				*((uint8_t *) buffer) = canrxfg_register[active_canrxfg_index][offset - CANRXFG_START];
			}
			else if ((offset >= CANTXFG_START) && (offset <= CANTXFG_END)) {
				*((uint8_t *) buffer) = cantxfg_register[active_cantxfg_index][offset - CANTXFG_START];
			} else {
				return false;
			}
		} break;
	}


	return (true);
}

bool S12MSCAN::write(unsigned int offset, const void *buffer, unsigned int data_length) {

//	cout << "CAN write base 0x" << std::hex << baseAddress << "  off " << std::dec << offset << std::endl;

	switch (offset) {
		case CANCTL0: {
			uint8_t value = *((uint8_t *) buffer);

			if (isInitMode()) {
				value = (value & 0x07);
				canctl0_register = value;

				if (!isInitModeRequest()) { exitInitMode(); }

			} else {
				value = ((value & 0x80) == 0x80)? (value & 0x7F) : (value | (canctl0_register & 0x80));
				value = (isWakeupInterrupt())? (value & 0xFD): value;
				value = (value & 0xAF) | (canctl0_register & 0x50);

				canctl0_register = value;

				if (isInitModeRequest()) { enterInitMode(); }

				// TODO: refine more the following two actions
				if (isTimerActivated()) { timer_start_event.notify(); }
				if (isCANBusIdle() && isSleepModeRequest()) { enterSleepMode(); }

			}

		} break;
		case CANCTL1: {
			if (!isInitMode()) break;

			uint8_t value = (*((uint8_t *) buffer) & 0xFC) | (canctl1_register & 0x03);
			canctl1_register = value;

			ComputeInternalTime();
		} break;
		case CANBTR0: {
			if (!isInitMode()) break;

			canbtr0_register = *((uint8_t *) buffer);

			ComputeInternalTime();
		} break;
		case CANBTR1: {
			if (!isInitMode()) break;

			canbtr1_register = *((uint8_t *) buffer);

			if (isThreeSamplePerBit() && (getTimeSegment1() < 2))
			{
				std::cerr << "Warning::" << sc_object::name() << ":: PHASE_SEG1 must be at least 2 time quanta (Tq) when Sampling is set to Three samples per bit" << std::endl;
			}

			ComputeInternalTime();
		} break;
		case CANRFLG: {
			if (isInitMode()) break;

			uint8_t value =  *((uint8_t *) buffer);

			canrflg_register = (canrflg_register & 0x3C) | (canrflg_register & ~(value | 0x3C));

		} break;
		case CANRIER: {
			if (isInitMode()) break;

			canrier_register = *((uint8_t *) buffer);

		} break;
		case CANTFLG: {
			if (isInitMode() || isListen()) break;

			uint8_t value =  *((uint8_t *) buffer) & 0x07;
			/*
			 * Clearing a TXEx flag also clears the corresponding ABTAKx (see Section 10.3.2.10, “MSCAN Transmitter
			 * Message Abort Acknowledge Register (CANTAAK)”).
			 * When listen-mode is active (see Section 10.3.2.2, “MSCAN Control Register 1 (CANCTL1)”) the TXEx flags
			 * cannot be cleared and no transmission is started.
			 */

			cantflg_register = cantflg_register & ~value;
			cantaak_register = cantaak_register & ~value;

		} break;
		case CANTIER: {
			if (isInitMode()) break;

			cantier_register = *((uint8_t *) buffer) & 0x07;
		} break;
		case CANTARQ: {
			if (isInitMode()) break;

			// The CPU cannot reset CANTARQ::ABTRQx, it is reset automatically whenever the associated TXE flag is set.
			cantarq_register =  (cantarq_register & 0x07) | (*((uint8_t *) buffer) & 0x07);
		} break;
		case CANTAAK: {
			// NOP
		} break;
		case CANTBSEL: {
			if (isInitMode()) break;

			cantbsel_register = *((uint8_t *) buffer) & 0x07;
		} break;
		case CANIDAC: {
			if (!isInitMode()) break;

			canidac_register = (canidac_register & 0x07) | (*((uint8_t *) buffer) & 0x30);
		} break;
		case RESERVED1: {
			// NOP
		} break;
		case CANMISC: {
			uint8_t value = *((uint8_t *) buffer) & 0x01;

			canmisc_register = canmisc_register & ~value;
		} break;
		case CANRXERR: {
			canrxerr_register = *((uint8_t *) buffer);
		} break;
		case CANTXERR: {
			cantxerr_register = *((uint8_t *) buffer);
		} break;
		case CANIDAR0: {
			if (!isInitMode()) break;

			canidar0_register = *((uint8_t *) buffer);
		} break;
		case CANIDAR1: {
			if (!isInitMode()) break;

			canidar1_register = *((uint8_t *) buffer);
		} break;
		case CANIDAR2: {
			if (!isInitMode()) break;

			canidar2_register = *((uint8_t *) buffer);
		} break;
		case CANIDAR3: {
			if (!isInitMode()) break;

			canidar3_register = *((uint8_t *) buffer);
		} break;
		case CANIDMR0: {
			if (!isInitMode()) break;

			canidmr0_register = *((uint8_t *) buffer);
		} break;
		case CANIDMR1: {
			if (!isInitMode()) break;

			canidmr1_register = *((uint8_t *) buffer);
		} break;
		case CANIDMR2: {
			if (!isInitMode()) break;

			canidmr2_register = *((uint8_t *) buffer);
		} break;
		case CANIDMR3: {
			if (!isInitMode()) break;

			canidmr3_register = *((uint8_t *) buffer);
		} break;
		case CANIDAR4: {
			if (!isInitMode()) break;

			canidar4_register = *((uint8_t *) buffer);
		} break;
		case CANIDAR5: {
			if (!isInitMode()) break;

			canidar5_register = *((uint8_t *) buffer);
		} break;
		case CANIDAR6: {
			if (!isInitMode()) break;

			canidar6_register = *((uint8_t *) buffer);
		} break;
		case CANIDAR7: {
			if (!isInitMode()) break;

			canidar7_register = *((uint8_t *) buffer);
		} break;
		case CANIDMR4: {
			if (!isInitMode()) break;

			canidmr4_register = *((uint8_t *) buffer);
		} break;
		case CANIDMR5: {
			if (!isInitMode()) break;

			canidmr5_register = *((uint8_t *) buffer);
		} break;
		case CANIDMR6: {
			if (!isInitMode()) break;

			canidmr6_register = *((uint8_t *) buffer);
		} break;
		case CANIDMR7: {
			if (!isInitMode()) break;

			canidmr7_register = *((uint8_t *) buffer);
		} break;

		default: {
			if ((offset >= CANRXFG_START) && (offset <= CANRXFG_END)) {
				canrxfg_register[active_canrxfg_index][offset - CANRXFG_START] = *((uint8_t *) buffer);
			}
			else if ((offset >= CANTXFG_START) && (offset <= CANTXFG_END)) {
				cantxfg_register[active_cantxfg_index][offset - CANTXFG_START] = *((uint8_t *) buffer);
			} else {
				return false;
			}
		} break;
	}

	return (true);
}

void S12MSCAN::assertInterrupt(uint8_t interrupt_offset) {

	tlm_phase phase = BEGIN_REQ;

	xint_payload->acquire();

	xint_payload->setInterruptOffset(interrupt_offset);

	sc_time local_time = quantumkeeper.get_local_time();

	tlm_sync_enum ret = interrupt_request->nb_transport_fw(*xint_payload, phase, local_time);

	xint_payload->release();

	switch(ret)
	{
		case TLM_ACCEPTED:
			// neither payload, nor phase and local_time have been modified by the callee
			quantumkeeper.sync(); // synchronize to leave control to the callee
			break;
		case TLM_UPDATED:
			// the callee may have modified 'payload', 'phase' and 'local_time'
			quantumkeeper.set(local_time); // increase the time
			if(quantumkeeper.need_sync()) quantumkeeper.sync(); // synchronize if needed

			break;
		case TLM_COMPLETED:
			// the callee may have modified 'payload', and 'local_time' ('phase' can be ignored)
			quantumkeeper.set(local_time); // increase the time
			if(quantumkeeper.need_sync()) quantumkeeper.sync(); // synchronize if needed
			break;
	}

}

// Master methods

void S12MSCAN::invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range)
{
	// Leave this empty as it is designed for memory mapped buses
}

// Master methods

tlm_sync_enum S12MSCAN::nb_transport_bw( XINT_Payload& payload, tlm_phase& phase, sc_core::sc_time& t)
{
	if(phase == BEGIN_RESP)
	{
		payload.release();
		return (TLM_COMPLETED);
	}
	return (TLM_ACCEPTED);
}


void S12MSCAN::ComputeInternalTime() {

	oscillator_clock = sc_time((double) oscillator_clock_value, SC_PS);
	bus_cycle_time = sc_time((double)bus_cycle_time_int, SC_PS);
	sc_time can_clk;
	if (isBusClk()) {
		can_clk = bus_cycle_time;
	} else {
		can_clk = oscillator_clock;
	}

	bit_time = can_clk * getPrescaler() * (1 + getTimeSegment1() + getTimeSegment2());

	telnet_process_input_period = bit_time * 8 * 8;

}


void S12MSCAN::updateBusClock(tlm::tlm_generic_payload& trans, sc_time& delay) {

	sc_dt::uint64*   external_bus_clock = (sc_dt::uint64*) trans.get_data_ptr();
    trans.set_response_status( tlm::TLM_OK_RESPONSE );

	bus_cycle_time_int = *external_bus_clock;

	ComputeInternalTime();
}

//=====================================================================
//=                  Client/Service setup methods                     =
//=====================================================================


bool S12MSCAN::BeginSetup() {

	char buf[80];

	sprintf(buf, "%s.CANCTL0",sc_object::name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &canctl0_register);

	unisim::kernel::service::Register<uint8_t> *canctl0_var = new unisim::kernel::service::Register<uint8_t>("CANCTL0", this, canctl0_register, "CAN Control Register 0");
	extended_registers_registry.push_back(canctl0_var);
	canctl0_var->setCallBack(this, CANCTL0, &CallBackObject::write, NULL);

	sprintf(buf, "%s.CANCTL1",sc_object::name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &canctl1_register);

	unisim::kernel::service::Register<uint8_t> *canctl1_var = new unisim::kernel::service::Register<uint8_t>("CANCTL1", this, canctl1_register, "CAN Control Register 1");
	extended_registers_registry.push_back(canctl1_var);
	canctl1_var->setCallBack(this, CANCTL1, &CallBackObject::write, NULL);

	sprintf(buf, "%s.CANBTR0",sc_object::name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &canbtr0_register);

	unisim::kernel::service::Register<uint8_t> *canbtr0_var = new unisim::kernel::service::Register<uint8_t>("CANBTR0", this, canbtr0_register, "CAN  0");
	extended_registers_registry.push_back(canbtr0_var);
	canbtr0_var->setCallBack(this, CANBTR0, &CallBackObject::write, NULL);

	sprintf(buf, "%s.CANBTR1",sc_object::name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &canbtr1_register);

	unisim::kernel::service::Register<uint8_t> *canbtr1_var = new unisim::kernel::service::Register<uint8_t>("CANBTR1", this, canbtr1_register, "CAN  1");
	extended_registers_registry.push_back(canbtr1_var);
	canbtr1_var->setCallBack(this, CANBTR1, &CallBackObject::write, NULL);

	sprintf(buf, "%s.CANRFLG",sc_object::name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &canrflg_register);

	unisim::kernel::service::Register<uint8_t> *canrflg_var = new unisim::kernel::service::Register<uint8_t>("CANRFLG", this, canrflg_register, "CAN  ");
	extended_registers_registry.push_back(canrflg_var);
	canrflg_var->setCallBack(this, CANRFLG, &CallBackObject::write, NULL);

	sprintf(buf, "%s.CANRIER",sc_object::name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &canrier_register);

	unisim::kernel::service::Register<uint8_t> *canrier_var = new unisim::kernel::service::Register<uint8_t>("CANRIER", this, canrier_register, "CAN  ");
	extended_registers_registry.push_back(canrier_var);
	canrier_var->setCallBack(this, CANRIER, &CallBackObject::write, NULL);

	sprintf(buf, "%s.CANTFLG",sc_object::name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &cantflg_register);

	unisim::kernel::service::Register<uint8_t> *cantflg_var = new unisim::kernel::service::Register<uint8_t>("CANTFLG", this, cantflg_register, "CAN  ");
	extended_registers_registry.push_back(cantflg_var);
	cantflg_var->setCallBack(this, CANTFLG, &CallBackObject::write, NULL);

	sprintf(buf, "%s.CANTIER",sc_object::name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &cantier_register);

	unisim::kernel::service::Register<uint8_t> *cantier_var = new unisim::kernel::service::Register<uint8_t>("CANTIER", this, cantier_register, "CAN  ");
	extended_registers_registry.push_back(cantier_var);
	cantier_var->setCallBack(this, CANTIER, &CallBackObject::write, NULL);

	sprintf(buf, "%s.CANTARQ",sc_object::name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &cantarq_register);

	unisim::kernel::service::Register<uint8_t> *cantarq_var = new unisim::kernel::service::Register<uint8_t>("CANTARQ", this, cantarq_register, "CAN  ");
	extended_registers_registry.push_back(cantarq_var);
	cantarq_var->setCallBack(this, CANTARQ, &CallBackObject::write, NULL);

	sprintf(buf, "%s.CANTAAK",sc_object::name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &cantaak_register);

	unisim::kernel::service::Register<uint8_t> *cantaak_var = new unisim::kernel::service::Register<uint8_t>("CANTAAK", this, cantaak_register, "CAN  ");
	extended_registers_registry.push_back(cantaak_var);
	cantaak_var->setCallBack(this, CANTAAK, &CallBackObject::write, NULL);

	sprintf(buf, "%s.CANTBSEL",sc_object::name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &cantbsel_register);

	unisim::kernel::service::Register<uint8_t> *cantbsel_var = new unisim::kernel::service::Register<uint8_t>("CANTBSEL", this, cantbsel_register, "CAN  ");
	extended_registers_registry.push_back(cantbsel_var);
	cantbsel_var->setCallBack(this, CANTBSEL, &CallBackObject::write, NULL);

	sprintf(buf, "%s.CANIDAC",sc_object::name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &canidac_register);

	unisim::kernel::service::Register<uint8_t> *canidac_var = new unisim::kernel::service::Register<uint8_t>("CANIDAC", this, canidac_register, "CAN  ");
	extended_registers_registry.push_back(canidac_var);
	canidac_var->setCallBack(this, CANIDAC, &CallBackObject::write, NULL);

	sprintf(buf, "%s.CANMISC",sc_object::name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &canmisc_register);

	unisim::kernel::service::Register<uint8_t> *canmisc_var = new unisim::kernel::service::Register<uint8_t>("CANMISC", this, canmisc_register, "CAN  ");
	extended_registers_registry.push_back(canmisc_var);
	canmisc_var->setCallBack(this, CANMISC, &CallBackObject::write, NULL);

	sprintf(buf, "%s.CANRXERR",sc_object::name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &canrxerr_register);

	unisim::kernel::service::Register<uint8_t> *canrxerr_var = new unisim::kernel::service::Register<uint8_t>("CANRXERR", this, canrxerr_register, "CAN  ");
	extended_registers_registry.push_back(canrxerr_var);
	canrxerr_var->setCallBack(this, CANRXERR, &CallBackObject::write, NULL);

	sprintf(buf, "%s.CANTXERR",sc_object::name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &cantxerr_register);

	unisim::kernel::service::Register<uint8_t> *cantxerr_var = new unisim::kernel::service::Register<uint8_t>("CANTXERR", this, cantxerr_register, "CAN  ");
	extended_registers_registry.push_back(cantxerr_var);
	cantxerr_var->setCallBack(this, CANTXERR, &CallBackObject::write, NULL);

	sprintf(buf, "%s.CANIDAR0",sc_object::name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &canidar0_register);

	unisim::kernel::service::Register<uint8_t> *canidar0_var = new unisim::kernel::service::Register<uint8_t>("CANIDAR0", this, canidar0_register, "CAN  ");
	extended_registers_registry.push_back(canidar0_var);
	canidar0_var->setCallBack(this, CANIDAR0, &CallBackObject::write, NULL);

	sprintf(buf, "%s.CANIDAR1",sc_object::name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &canidar1_register);

	unisim::kernel::service::Register<uint8_t> *canidar1_var = new unisim::kernel::service::Register<uint8_t>("CANIDAR1", this, canidar1_register, "CAN  ");
	extended_registers_registry.push_back(canidar1_var);
	canidar1_var->setCallBack(this, CANIDAR1, &CallBackObject::write, NULL);

	sprintf(buf, "%s.CANIDAR2",sc_object::name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &canidar2_register);

	unisim::kernel::service::Register<uint8_t> *canidar2_var = new unisim::kernel::service::Register<uint8_t>("CANIDAR2", this, canidar2_register, "CAN  ");
	extended_registers_registry.push_back(canidar2_var);
	canidar2_var->setCallBack(this, CANIDAR2, &CallBackObject::write, NULL);

	sprintf(buf, "%s.CANIDAR3",sc_object::name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &canidar3_register);

	unisim::kernel::service::Register<uint8_t> *canidar3_var = new unisim::kernel::service::Register<uint8_t>("CANIDAR3", this, canidar3_register, "CAN  ");
	extended_registers_registry.push_back(canidar3_var);
	canidar3_var->setCallBack(this, CANIDAR3, &CallBackObject::write, NULL);

	sprintf(buf, "%s.CANIDMR0",sc_object::name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &canidmr0_register);

	unisim::kernel::service::Register<uint8_t> *canidmr0_var = new unisim::kernel::service::Register<uint8_t>("CANIDMR0", this, canidmr0_register, "CAN  ");
	extended_registers_registry.push_back(canidmr0_var);
	canidmr0_var->setCallBack(this, CANIDMR0, &CallBackObject::write, NULL);

	sprintf(buf, "%s.CANIDMR1",sc_object::name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &canidmr1_register);

	unisim::kernel::service::Register<uint8_t> *canidmr1_var = new unisim::kernel::service::Register<uint8_t>("CANIDMR1", this, canidmr1_register, "CAN  ");
	extended_registers_registry.push_back(canidmr1_var);
	canidmr1_var->setCallBack(this, CANIDMR1, &CallBackObject::write, NULL);

	sprintf(buf, "%s.CANIDMR2",sc_object::name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &canidmr2_register);

	unisim::kernel::service::Register<uint8_t> *canidmr2_var = new unisim::kernel::service::Register<uint8_t>("CANIDMR2", this, canidmr2_register, "CAN  ");
	extended_registers_registry.push_back(canidmr2_var);
	canidmr2_var->setCallBack(this, CANIDMR2, &CallBackObject::write, NULL);

	sprintf(buf, "%s.CANIDMR3",sc_object::name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &canidmr3_register);

	unisim::kernel::service::Register<uint8_t> *canidmr3_var = new unisim::kernel::service::Register<uint8_t>("CANIDMR3", this, canidmr3_register, "CAN  ");
	extended_registers_registry.push_back(canidmr3_var);
	canidmr3_var->setCallBack(this, CANIDMR3, &CallBackObject::write, NULL);

	sprintf(buf, "%s.CANIDAR4",sc_object::name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &canidar4_register);

	unisim::kernel::service::Register<uint8_t> *canidar4_var = new unisim::kernel::service::Register<uint8_t>("CANIDAR4", this, canidar4_register, "CAN  ");
	extended_registers_registry.push_back(canidar4_var);
	canidar4_var->setCallBack(this, CANIDAR4, &CallBackObject::write, NULL);

	sprintf(buf, "%s.CANIDAR5",sc_object::name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &canidar5_register);

	unisim::kernel::service::Register<uint8_t> *canidar5_var = new unisim::kernel::service::Register<uint8_t>("CANIDAR5", this, canidar5_register, "CAN  ");
	extended_registers_registry.push_back(canidar5_var);
	canidar5_var->setCallBack(this, CANIDAR5, &CallBackObject::write, NULL);

	sprintf(buf, "%s.CANIDAR6",sc_object::name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &canidar6_register);

	unisim::kernel::service::Register<uint8_t> *canidar6_var = new unisim::kernel::service::Register<uint8_t>("CANIDAR6", this, canidar6_register, "CAN  ");
	extended_registers_registry.push_back(canidar6_var);
	canidar6_var->setCallBack(this, CANIDAR6, &CallBackObject::write, NULL);

	sprintf(buf, "%s.CANIDAR7",sc_object::name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &canidar7_register);

	unisim::kernel::service::Register<uint8_t> *canidar7_var = new unisim::kernel::service::Register<uint8_t>("CANIDAR7", this, canidar7_register, "CAN  ");
	extended_registers_registry.push_back(canidar7_var);
	canidar7_var->setCallBack(this, CANIDAR7, &CallBackObject::write, NULL);

	sprintf(buf, "%s.CANIDMR4",sc_object::name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &canidmr4_register);

	unisim::kernel::service::Register<uint8_t> *canidmr4_var = new unisim::kernel::service::Register<uint8_t>("CANIDMR4", this, canidmr4_register, "CAN  ");
	extended_registers_registry.push_back(canidmr4_var);
	canidmr4_var->setCallBack(this, CANIDMR4, &CallBackObject::write, NULL);

	sprintf(buf, "%s.CANIDMR5",sc_object::name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &canidmr5_register);

	unisim::kernel::service::Register<uint8_t> *canidmr5_var = new unisim::kernel::service::Register<uint8_t>("CANIDMR5", this, canidmr5_register, "CAN  ");
	extended_registers_registry.push_back(canidmr5_var);
	canidmr5_var->setCallBack(this, CANIDMR5, &CallBackObject::write, NULL);

	sprintf(buf, "%s.CANIDMR6",sc_object::name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &canidmr6_register);

	unisim::kernel::service::Register<uint8_t> *canidmr6_var = new unisim::kernel::service::Register<uint8_t>("CANIDMR6", this, canidmr6_register, "CAN  ");
	extended_registers_registry.push_back(canidmr6_var);
	canidmr6_var->setCallBack(this, CANIDMR6, &CallBackObject::write, NULL);

	sprintf(buf, "%s.CANIDMR7",sc_object::name());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &canidmr7_register);

	unisim::kernel::service::Register<uint8_t> *canidmr7_var = new unisim::kernel::service::Register<uint8_t>("CANIDMR7", this, canidmr7_register, "CAN  ");
	extended_registers_registry.push_back(canidmr7_var);
	canidmr7_var->setCallBack(this, CANIDMR7, &CallBackObject::write, NULL);


//	CANRXFG_START=0x20, CANRXFG_END=0x2F, CANTXFG_START=0x30, CANTXFG_END=0x3F

	Reset();

	ComputeInternalTime();

	return (true);
}


bool S12MSCAN::Setup(ServiceExportBase *srv_export) {

	return (true);
}


bool S12MSCAN::EndSetup() {
	return (true);
}


Register* S12MSCAN::GetRegister(const char *name)
{
	if(registers_registry.find(string(name)) != registers_registry.end())
		return (registers_registry[string(name)]);
	else
		return (NULL);

}


void S12MSCAN::OnDisconnect() {
}


void S12MSCAN::Reset() {

	canctl0_register = 0x01;
	canctl1_register = 0x11;
	canbtr0_register = 0x00;
	canbtr1_register = 0x00;
	canrflg_register = 0x00;
	canrier_register = 0x00;
	cantflg_register = 0x07;
	cantier_register = 0x00;
	cantarq_register = 0x00;
	cantaak_register = 0x00;
	cantbsel_register = 0x00;
	canidac_register = 0x00;
	reserved1 = 0x00;
	canmisc_register = 0x00;
	canrxerr_register = 0x00;
	cantxerr_register = 0x00;
	canidar0_register = 0x00;
	canidar1_register = 0x00;
	canidar2_register = 0x00;
	canidar3_register = 0x00;
	canidmr0_register = 0x00;
	canidmr1_register = 0x00;
	canidmr2_register = 0x00;
	canidmr3_register = 0x00;
	canidar4_register = 0x00;
	canidar5_register = 0x00;
	canidar6_register = 0x00;
	canidar7_register = 0x00;
	canidmr4_register = 0x00;
	canidmr5_register = 0x00;
	canidmr6_register = 0x00;
	canidmr7_register = 0x00;

	for (int i=0; i<5; i++) {
		for (int j=0; j<16; j++) {
			canrxfg_register[i][j] = 0x00;
		}
	}

	for (int i=0; i<3; i++) {
		for (int j=0; j<16; j++) {
			cantxfg_register[i][j] = 0x00;
		}
	}

	for (int i=0; i<16; i++) {
		tx_shift_register[i] = 0x00;
		rx_shift_register[i] = 0x00;
	}

	if(char_io_import)
	{
		char_io_import->Reset();
	}

}


//=====================================================================
//=             memory interface methods                              =
//=====================================================================


bool S12MSCAN::ReadMemory(physical_address_t addr, void *buffer, uint32_t size) {

	if ((addr >= baseAddress) && (addr < (baseAddress + MEMORY_MAP_SIZE))) {

		physical_address_t offset = addr-baseAddress;

		switch (offset) {
			case CANCTL0: {
				*((uint8_t *) buffer) = canctl0_register;
			} break;
			case CANCTL1: {
				*((uint8_t *) buffer) = canctl1_register;
			} break;
			case CANBTR0: {
				*((uint8_t *) buffer) = canbtr0_register;
			} break;
			case CANBTR1: {
				*((uint8_t *) buffer) = canbtr1_register;
			} break;
			case CANRFLG: {
				*((uint8_t *) buffer) = canrflg_register;
			} break;
			case CANRIER: {
				*((uint8_t *) buffer) = canrier_register;
			} break;
			case CANTFLG: {
				*((uint8_t *) buffer) = cantflg_register;
			} break;
			case CANTIER: {
				*((uint8_t *) buffer) = cantier_register;
			} break;
			case CANTARQ: {
				*((uint8_t *) buffer) = cantarq_register;
			} break;
			case CANTAAK: {
				*((uint8_t *) buffer) = cantaak_register;
			} break;
			case CANTBSEL: {
				*((uint8_t *) buffer) = cantbsel_register;
			} break;
			case CANIDAC: {
				*((uint8_t *) buffer) = canidac_register;
			} break;
			case RESERVED1: {
				*((uint8_t *) buffer) = 0x00;
			} break;
			case CANMISC: {
				*((uint8_t *) buffer) = canmisc_register;
			} break;
			case CANRXERR: {
				*((uint8_t *) buffer) = canrxerr_register;
			} break;
			case CANTXERR: {
				*((uint8_t *) buffer) = cantxerr_register;
			} break;
			case CANIDAR0: {
				*((uint8_t *) buffer) = canidar0_register;
			} break;
			case CANIDAR1: {
				*((uint8_t *) buffer) = canidar1_register;
			} break;
			case CANIDAR2: {
				*((uint8_t *) buffer) = canidar2_register;
			} break;
			case CANIDAR3: {
				*((uint8_t *) buffer) = canidar3_register;
			} break;
			case CANIDMR0: {
				*((uint8_t *) buffer) = canidmr0_register;
			} break;
			case CANIDMR1: {
				*((uint8_t *) buffer) = canidmr1_register;
			} break;
			case CANIDMR2: {
				*((uint8_t *) buffer) = canidmr2_register;
			} break;
			case CANIDMR3: {
				*((uint8_t *) buffer) = canidmr3_register;
			} break;
			case CANIDAR4: {
				*((uint8_t *) buffer) = canidar4_register;
			} break;
			case CANIDAR5: {
				*((uint8_t *) buffer) = canidar5_register;
			} break;
			case CANIDAR6: {
				*((uint8_t *) buffer) = canidar6_register;
			} break;
			case CANIDAR7: {
				*((uint8_t *) buffer) = canidar7_register;
			} break;
			case CANIDMR4: {
				*((uint8_t *) buffer) = canidmr4_register;
			} break;
			case CANIDMR5: {
				*((uint8_t *) buffer) = canidmr5_register;
			} break;
			case CANIDMR6: {
				*((uint8_t *) buffer) = canidmr6_register;
			} break;
			case CANIDMR7: {
				*((uint8_t *) buffer) = canidmr7_register;
			} break;

			default: {
				if ((offset >= CANRXFG_START) && (offset <= CANRXFG_END)) {
					*((uint8_t *) buffer) = canrxfg_register[active_canrxfg_index][offset - CANRXFG_START];
				}
				else if ((offset >= CANTXFG_START) && (offset <= CANTXFG_END)) {
					*((uint8_t *) buffer) = cantxfg_register[active_cantxfg_index][offset - CANTXFG_START];
				} else {
					return false;
				}
			} break;
		}

		return (true);

	}

	return (false);
}


bool S12MSCAN::WriteMemory(physical_address_t addr, const void *buffer, uint32_t size) {

	if ((addr >= baseAddress) && (addr < (baseAddress + MEMORY_MAP_SIZE))) {

		if (size == 0) {
			return (true);
		}

		physical_address_t offset = addr-baseAddress;

		switch (offset) {
			case CANCTL0: {
				canctl0_register = *((uint8_t *) buffer);
			} break;
			case CANCTL1: {
				canctl1_register = *((uint8_t *) buffer);
			} break;
			case CANBTR0: {
				canbtr0_register = *((uint8_t *) buffer);
			} break;
			case CANBTR1: {
				canbtr1_register = *((uint8_t *) buffer);
			} break;
			case CANRFLG: {
				canrflg_register = *((uint8_t *) buffer);
			} break;
			case CANRIER: {
				canrier_register = *((uint8_t *) buffer);
			} break;
			case CANTFLG: {
				cantflg_register = *((uint8_t *) buffer);
			} break;
			case CANTIER: {
				cantier_register = *((uint8_t *) buffer);
			} break;
			case CANTARQ: {
				cantarq_register = *((uint8_t *) buffer);
			} break;
			case CANTAAK: {
				cantaak_register = *((uint8_t *) buffer);
			} break;
			case CANTBSEL: {
				cantbsel_register = *((uint8_t *) buffer);
			} break;
			case CANIDAC: {
				canidac_register = *((uint8_t *) buffer);
			} break;
			case RESERVED1: {
				// NOP
			} break;
			case CANMISC: {
				canmisc_register = *((uint8_t *) buffer);
			} break;
			case CANRXERR: {
				canrxerr_register = *((uint8_t *) buffer);
			} break;
			case CANTXERR: {
				cantxerr_register = *((uint8_t *) buffer);
			} break;
			case CANIDAR0: {
				canidar0_register = *((uint8_t *) buffer);
			} break;
			case CANIDAR1: {
				canidar1_register = *((uint8_t *) buffer);
			} break;
			case CANIDAR2: {
				canidar2_register = *((uint8_t *) buffer);
			} break;
			case CANIDAR3: {
				canidar3_register = *((uint8_t *) buffer);
			} break;
			case CANIDMR0: {
				canidmr0_register = *((uint8_t *) buffer);
			} break;
			case CANIDMR1: {
				canidmr1_register = *((uint8_t *) buffer);
			} break;
			case CANIDMR2: {
				canidmr2_register = *((uint8_t *) buffer);
			} break;
			case CANIDMR3: {
				canidmr3_register = *((uint8_t *) buffer);
			} break;
			case CANIDAR4: {
				canidar4_register = *((uint8_t *) buffer);
			} break;
			case CANIDAR5: {
				canidar5_register = *((uint8_t *) buffer);
			} break;
			case CANIDAR6: {
				canidar6_register = *((uint8_t *) buffer);
			} break;
			case CANIDAR7: {
				canidar7_register = *((uint8_t *) buffer);
			} break;
			case CANIDMR4: {
				canidmr4_register = *((uint8_t *) buffer);
			} break;
			case CANIDMR5: {
				canidmr5_register = *((uint8_t *) buffer);
			} break;
			case CANIDMR6: {
				canidmr6_register = *((uint8_t *) buffer);
			} break;
			case CANIDMR7: {
				canidmr7_register = *((uint8_t *) buffer);
			} break;

			default: {
				if ((offset >= CANRXFG_START) && (offset <= CANRXFG_END)) {
					canrxfg_register[active_canrxfg_index][offset - CANRXFG_START] = *((uint8_t *) buffer);
				}
				else if ((offset >= CANTXFG_START) && (offset <= CANTXFG_END)) {
					cantxfg_register[active_cantxfg_index][offset - CANTXFG_START] = *((uint8_t *) buffer);
				} else {
					return false;
				}
			} break;
		}

		return (true);
	}

	return (false);

}


} // end of namespace hcs12x
} // end of namespace processor
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim





