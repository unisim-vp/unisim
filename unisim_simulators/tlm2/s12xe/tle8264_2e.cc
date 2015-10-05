
#include "tle8264_2e.hh"

TLE8264_2E::TLE8264_2E(const sc_module_name& name, Object *parent) :
	Object(name)
	, sc_module(name)

	, interrupt_request("interrupt-request")
	, bus_clock_socket("bus-clock-socket")
	, can_slave_rx_sock("slave-rx")
	, can_slave_tx_sock("slave-tx")

	, spi_tx_socket("tx-socket")
	, spi_rx_socket("rx-socket")

	, rx_payload_queue("input_anx_payload_queue")

	, bus_cycle_time_int(250000)
	, param_bus_cycle_time_int("bus-cycle-time", this, bus_cycle_time_int)


	, current_mode(INIT)
	, current_cmd(INIT)
	, wd_refresh(false)
	, SPIWake_read(false)

	, terminated(false)

	, frame_time(256, SC_MS)
	, limp_home(false)
	, sig_limp_home("limp-home", this, limp_home)

// Interrupt ID
	, reset_interrupt(0xFE)
	, param_reset_interrupt("reset-interrupt", this, reset_interrupt)
	, int_interrupt(1) // TODO; look to BCM specification
	, param_int_interrupt("int-interrupt", this, int_interrupt)


{
	SC_HAS_PROCESS(TLE8264_2E);

	SC_THREAD(WatchDogThread);

	xint_payload = xint_payload_fabric.allocate();

	can_slave_rx_sock(*this);
	can_slave_tx_sock(*this);

	interrupt_request(*this);

	bus_clock_socket.register_b_transport(this, &TLE8264_2E::updateBusClock);

	spi_rx_socket.register_b_transport(this, &TLE8264_2E::spi_rx_b_transport);
}

TLE8264_2E::~TLE8264_2E() { }

bool TLE8264_2E::BeginSetup() {

	Reset();

	return (true);
}


bool TLE8264_2E::Setup(ServiceExportBase *srv_export) {

	return (true);
}


bool TLE8264_2E::EndSetup() {
	return (true);
}

void TLE8264_2E::OnDisconnect() {
}

void TLE8264_2E::Reset() {

	// interrupt mask               default INPUT        default OUTPUT
	registers[0] = 0b000000000; //input=0b000111111  ; output=0b000000000
	registers[1] = 0b000000000; //input=0b111111111  ; output=0b000000000
	registers[2] = 0b000000000; //input=0b101010110  ; output=0b000000000
	registers[3] = 0b000000000; //input=0b000000000  ; output=0b000000000  reserved register

	// register
	registers[4] = 0b100010101; //input=0b100010101  ; output=0b100010101
	registers[5] = 0b100000000; //input=0b100000000  ; output=0b100000000
	registers[6] = 0b111111111; //input=0b111111111  ; output=0b111111111
	registers[7] = 0b000000000; //input=0b000000000  ; output=0b000000000  reserved register

	wd_refresh = false;
}

void TLE8264_2E::Stop(int exit_status) {}

void TLE8264_2E::spi_rx_b_transport(tlm::tlm_generic_payload& payload, sc_core::sc_time& t)
{
	payload.acquire();
	unsigned int length = payload.get_data_length();
	unsigned char* data = payload.get_data_ptr();

	if (payload.get_byte_enable_length() != 2)
	{
		std::cerr << sc_object::name() << "::Warning interface is configured for 2-bytes" << std::endl;
	}

	uint16_t spi_rx_buffer = *((uint16_t *) payload.get_data_ptr());

	setCmd(spi_rx_buffer);

	payload.release();
}

tlm_sync_enum TLE8264_2E::nb_transport_bw( XINT_Payload& payload, tlm_phase& phase, sc_core::sc_time& t)
{
	if(phase == BEGIN_RESP)
	{
		payload.release();
		return (TLM_COMPLETED);
	}
	return (TLM_ACCEPTED);
}

bool TLE8264_2E::get_direct_mem_ptr(CAN_Payload& payload, tlm_dmi&  dmi_data) {
	return (false);
}

unsigned int TLE8264_2E::transport_dbg(CAN_Payload& payload)
{
	// Leave this empty as it is designed for memory mapped buses
	return (0);
}

tlm_sync_enum TLE8264_2E::nb_transport_fw(CAN_Payload& payload, tlm_phase& phase, sc_core::sc_time& t)
{
	switch(phase)
	{
		case BEGIN_REQ:
			// accepts analog signals modeled by payload
			phase = END_REQ; // update the phase
			payload.acquire();
			rx_payload_queue.notify(payload, t); // queue the payload and the associative time

			return (TLM_UPDATED);
		case END_REQ:
			cout << sc_time_stamp() << ":" << sc_object::name() << ": received an unexpected phase END_REQ" << endl;
			Object::Stop(-1);
			break;
		case BEGIN_RESP:
			cout << sc_time_stamp() << ":" << sc_object::name() << ": received an unexpected phase BEGIN_RESP" << endl;
			Object::Stop(-1);
			break;
		case END_RESP:
			cout << sc_time_stamp() << ":" << sc_object::name() << ": received an unexpected phase END_RESP" << endl;
			Object::Stop(-1);
			break;
		default:
			cout << sc_time_stamp() << ":" << sc_object::name() << ": received an unexpected phase" << endl;
			Object::Stop(-1);
			break;
	}

	return (TLM_ACCEPTED);
}

tlm_sync_enum TLE8264_2E::nb_transport_bw(CAN_Payload& can_rx_payload, tlm_phase& phase, sc_core::sc_time& t)
{
	switch(phase)
	{
		case BEGIN_REQ:
			cout << sc_time_stamp() << ":" << sc_object::name() << ": received an unexpected phase BEGIN_REQ" << endl;

			Object::Stop(-1);
			break;
		case END_REQ:
			cout << sc_time_stamp() << ":" << sc_object::name() << ": received an unexpected phase END_REQ" << endl;
			Object::Stop(-1);
			break;
		case BEGIN_RESP:
			//can_rx_payload.release();
			can_bw_event.notify();
			return (TLM_COMPLETED);
		case END_RESP:
			cout << sc_time_stamp() << ":" << sc_object::name() << ": received an unexpected phase END_RESP" << endl;
			Object::Stop(-1);
			break;
		default:
			cout << sc_time_stamp() << ":" << sc_object::name() << ": received an unexpected phase" << endl;
			Object::Stop(-1);
			break;
	}

	return (TLM_ACCEPTED);
}

void TLE8264_2E::b_transport(CAN_Payload& payload, sc_core::sc_time& t) {
	payload.acquire();
	rx_payload_queue.notify(payload, t);
}

void TLE8264_2E::invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range)
{
	// Leave this empty as it is designed for memory mapped buses
}

void TLE8264_2E::updateBusClock(tlm::tlm_generic_payload& trans, sc_time& delay) {

	sc_dt::uint64*   external_bus_clock = (sc_dt::uint64*) trans.get_data_ptr();
    trans.set_response_status( tlm::TLM_OK_RESPONSE );

	bus_cycle_time_int = *external_bus_clock;

	ComputeInternalTime();
}

void TLE8264_2E::ComputeInternalTime() {

	bus_cycle_time = sc_time((double)bus_cycle_time_int, SC_PS);

}



void TLE8264_2E::setLimpHome(bool val) {
	registers[0b100] = (registers[0b100] & 0xFFBF) | ((val)? LIMPHOME: 0x0000);
}
bool TLE8264_2E::isLimpHome() { return ((registers[0b100] & LIMPHOME) != 0); }

bool TLE8264_2E::isWatchDogEnable() {
	return ((registers[6] & WDONOFF) != 0);
}
bool TLE8264_2E::isINT() { return ((registers[0] & INT) != 0); }
bool TLE8264_2E::isWKCyclicEnabled() { return ((registers[0] & WKCYCLIC) != 0); }

void TLE8264_2E::WatchDogThread() {

	while (true) {
		while (!isWatchDogEnable()) {
			wait(watchdog_enable_event);
		}

		while (isWatchDogEnable()) {
			uint16_t state_before = current_mode;

			wait(frame_time);

			if ((state_before == INIT) && (state_before == current_mode)) {
				setLimpHome(true);
				current_mode = RESTART;
				enter_restart();
			}
			else if (isWKCyclicEnabled() && (state_before == STOP) && (state_before == current_mode)) {
				if (isINT() && !SPIWake_read) {
					current_mode = RESTART;
					enter_restart();
				} else {
					assert_int_interrup();
				}
			} else if ((state_before == SWFLASH) && (state_before == current_mode)) {
				/* Note: A mode change to Restart mode can be caused by SPI command,
				 *    a timeout or window watchdog failure or an undervoltage reset
				 */

				// when leaving the SWFlash a reset is generated
				assert_reset_interrupt();
				current_mode = RESTART;
				enter_restart();
			} else if ((state_before == RESTART) && (state_before == current_mode)) {
				current_mode = NORMAL;
				enter_normal();
			}
		}
	}
}

void TLE8264_2E::refresh_watchdog()
{
	// TODO: compute frame_time for watchdog
}

void TLE8264_2E::enter_init() {

}
void TLE8264_2E::enter_restart() {
	/*
	 * TODO:
	 * The first SPI output data will provide information about the reason for entering Restart Mode.
	 * The reason for entering Restart Mode is stored and kept until the microcontroller reads
	 * the corresponding “LH0..2” or “RM0..1” SPI bits.
	 */
	if ((registers[0b100] & RESETDELAY) == 0) {
		current_mode = NORMAL;
		enter_normal();
	}
}
void TLE8264_2E::enter_swflash() {

}
void TLE8264_2E::enter_normal() {
	if (!isWatchDogEnable()) { std::cerr << "In Normal Mode the watchdog needs to be triggered !!!" << std::endl; }

}
void TLE8264_2E::enter_sleep() {
	// Watchdog is disabled is Sleep Mode and the last configuration isn't saved
	registers[0b110] = REG_110_DEFAULT;

	/*
	 *  TODO: emulate sleep mode and wakeup waiting
	 *  As work-around the SBC goes directly to RESTART mode
	 *
	 * Note:
	 * The first SPI output data when going to SBC Normal Mode will always indicate the wake up source,
	 * as well as the SBC Sleep Mode to indicate where the device comes from and why it left the state.
	 *
	 * The CAN wakeup event has to be set using random canmsg generator or cosimulation stub
	 */

	/*
	 * In case of a wakeup from Sleep Mode the wake source is seen at the interrupt bit
	 * (config. select 0b000) an interrupt is not generated
	 */
	registers[0b000] = registers[0b000] | INT;
	current_mode = RESTART;
	registers[0b000] = registers[0b000] | WKCAN;
	enter_restart();
}
void TLE8264_2E::enter_stop() {

}
void TLE8264_2E::enter_sailsafe() {

}

void TLE8264_2E::assertInterrupt(uint8_t interrupt_offset) {


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

void TLE8264_2E::assert_int_interrup() {
	registers[0b000] = registers[0b000] | INT;
	assertInterrupt(int_interrupt);
}
void TLE8264_2E::assert_reset_interrupt() {
	registers[0b001] = registers[0b001] | RESET;
	assertInterrupt(reset_interrupt);
}

void TLE8264_2E::triggerStateMachine(uint16_t spi_cmd) {

	uint16_t new_mode = spi_cmd & 0x0007;
	uint16_t sel  = (spi_cmd & 0x0038) >> 3;
	uint16_t val = spi_cmd >> 6;

	switch (current_cmd & 0x0007)
	{
	case INIT: {
		switch (new_mode)
		{
		case INIT: {
			// No mode change
			current_cmd = write(sel, val);
		} break;
		case NORMAL: {
			current_mode = new_mode;
			current_cmd = write(sel, val);
			enter_normal();
		} break;
		case SWFLASH: {
			current_mode = new_mode;
			current_cmd = write(sel, val);
			enter_swflash();
		} break;
		case READONLY: /*no change but triggers wd and clear int ...*/ break;
		default: std::cout << "change mode not supported 0x" << std::hex << spi_cmd << std::dec << std::endl; break;
		}

	} break;
	case RESTART: {
		switch (new_mode)
		{
		case INIT: {
			current_cmd = write(sel, val);
		} break;
		//					case NORMAL: current_cmd = normal(spi_cmd); break;
		case READONLY: /*no change but triggers wd and clear int ...*/ break;
		default: std::cout << "change mode not supported 0x" << std::hex << spi_cmd << std::dec << std::endl; break;
		}

	} break;
	case SWFLASH: {
		switch (new_mode)
		{
		case INIT: {
			current_cmd = write(sel, val);
		} break;
		case SWFLASH: {
			// No mode change
			current_cmd = write(sel, val);
		} break;
		case RESTART: {
			current_mode = new_mode;
			current_cmd = write(sel, val);
			// when leaving the SWFlash a reset is generated
			assert_reset_interrupt();
			enter_restart();
		} break;
		case READONLY: /*no change but triggers wd and clear int ...*/ break;
		default: std::cout << "change mode not supported 0x" << std::hex << spi_cmd << std::dec << std::endl; break;
		}

	} break;
	case NORMAL: {
		switch (new_mode)
		{
		case INIT: {
			current_cmd = write(sel, val);
		} break;
		case RESTART: {
			current_cmd = write(sel, val);
			std::cout << "change mode not supported 0x" << std::hex << spi_cmd << std::dec << " Register updated"<< std::endl;
		} break;
		case SWFLASH: {
			current_mode = new_mode;
			current_cmd = write(sel, val);
			assert_reset_interrupt(); // A reset is triggered when entering SWFlash from Normal
			enter_swflash();
		} break;
		case NORMAL: {
			current_cmd = write(sel, val);
		} break;
		case SLEEP: {
			current_cmd = write(sel, val);
			if ((registers[0b000] & 0x7F) == 0) {
				/*
				 * In Sleep Mode, not all wake-up sources should be inhibited,
				 * this is required to not program the device in a mode where it can not wake up.
				 * If all wake sources are inhibited when sending the SBC to Sleep Mode,
				 * the SBC does not go to Sleep Mode, the microcontroller is informed via the INT output, and the SPI bit “Fail SPI” is set.
				 */
				registers[0b001] = registers[0b001] | FAILSPI;
				assert_int_interrup();

			} else {
				current_mode = new_mode;
				enter_sleep();
			}

		} break;
		case STOP: {
			uint16_t watchdog_settings = registers[0b110];
			current_cmd = write(sel, val);
			if ((registers[0b000] & 0x7F) == 0) {
				/*
				 * If all wakeup sources are disabled, (CAN, LIN, WK, cyclic wake) the watchdog can not be disabled,
				 * the SBC stays in Normal Mode and the watchdog continues with the old settings.
				 */
				registers[0b110] = watchdog_settings;
			} else {
				current_mode = new_mode;
				enter_stop();
			}
		} break;
		case READONLY: /*no change but triggers wd and clear int ...*/ break;
		default: std::cout << "change mode not supported 0x" << std::hex << spi_cmd << std::dec << std::endl; break;
		}

	} break;
	case SLEEP: {
		switch (new_mode)
		{
		case INIT: {
			current_cmd = write(sel, val);
		} break;
		case SLEEP: {
			current_cmd = write(sel, val);
		} break;
		//					case RESTART: current_cmd = restart(spi_cmd); break;
		case READONLY: /*no change but triggers wd and clear int ...*/ break;
		default: std::cout << "change mode not supported 0x" << std::hex << spi_cmd << std::dec << std::endl; break;
		}

	} break;
	case STOP: {
		switch (new_mode)
		{
		case INIT: {
			current_cmd = write(sel, val);
		} break;
		case NORMAL: {
			current_mode = new_mode;
			current_cmd = write(sel, val);
			enter_normal();
		} break;
		case SLEEP: {
			current_mode = new_mode;
			current_cmd = write(sel, val);
			enter_sleep();
		} break;
		case STOP: {
			current_cmd = write(sel, val);
		} break;
		case READONLY: /*no change but triggers wd and clear int ...*/ break;
		default: std::cout << "change mode not supported 0x" << std::hex << spi_cmd << std::dec << std::endl; break;
		}

	} break;
	case FAILSAFE: {
		switch (new_mode)
		{
		case INIT: {
			current_cmd = write(sel, val);
		} break;
		//					case RESTART: current_cmd = restart(spi_cmd); break;
		case FAILSAFE: {
			current_cmd = write(sel, val);
		} break;
		case READONLY: /*no change but triggers wd and clear int ...*/ break;
		default: std::cout << "change mode not supported 0x" << std::hex << spi_cmd << std::dec << std::endl; break;
		}

	} break;
	default: break;
	}

	// The default value of 'WD refresh bit' is 0. The first trigger must be 1 and the following triggers must be 0.
	if (wd_refresh ^ ((spi_cmd & 0x8000) != 0x8000))
	{
		wd_refresh = true;
		refresh_watchdog();
	}

}

uint16_t TLE8264_2E::setCmd(uint16_t cmd) {

	uint16_t last_state = current_cmd;

	triggerStateMachine(cmd);

	tlm::tlm_generic_payload *payload = spi_payload_fabric.allocate();

	payload->set_data_length(2);
	payload->set_data_ptr((unsigned char*) &last_state);

	sc_time local_time = SC_ZERO_TIME;
	spi_tx_socket->b_transport(*payload, local_time);

	payload->release();


	return last_state;

}

uint16_t TLE8264_2E::write(uint16_t sel, uint16_t val) {

	switch (sel)
	{
	case 0b000: registers[sel] = ~(registers[sel] ^ val) & 0x01FF; break;
	case 0b001: registers[sel] = ~(registers[sel] ^ val) & 0x01FF; break;
	case 0b010: registers[sel] = ~(registers[sel] ^ val) & 0x01FF; break;
	case 0b011: /* NOP; reserved register */ std::cerr << "TLE8264_2E:: write to reserved register 0b000" << std::endl; break;
	case 0b100: registers[sel] = val; break;
	case 0b101: registers[sel] = val; break;
	case 0b110: registers[sel] = val; break;
	case 0b111: /* NOP; reserved register */ std::cerr << "TLE8264_2E:: write to reserved register 0b111" << std::endl;break;
	}

	return ((registers[sel] << 6) | (sel << 3) | current_mode);
}

uint16_t TLE8264_2E::read(uint16_t sel, uint16_t& val) {

	switch (sel)
	{
	/* 0b000-0b001-0b010-0b011: Interrupt mask. A value of 0 will set the SBC into the opposite state. */
	case 0b000: {
		val = registers[sel];
		if ((current_mode == STOP) && isWKCyclicEnabled() && isINT()) {
			SPIWake_read = true;
		}
	} break;
	case 0b001: val = registers[sel]; break;
	case 0b010: val = registers[sel]; break;
	case 0b011: /* NOP; reserved register */ val = 0; std::cerr << "TLE8264_2E:: read of reserved register 0b000" << std::endl; break;
	case 0b100: val = registers[sel]; break;
	case 0b101: val = registers[sel]; break;
	case 0b110: val = registers[sel]; break;
	case 0b111: /* NOP; reserved register */ val = 0; std::cerr << "TLE8264_2E:: read of reserved register 0b111" << std::endl; break;
	}

	return ((registers[sel] << 6) | (sel << 3) | current_mode);
}

