/*
 *  Copyright (c) 2012,
 *  Commissariat a l'Energie Atomique (CEA)
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without modification,
 *  are permitted provided that the following conditions are met:
 *
 *   - Redistributions of source code must retain the above copyright notice, this
 *     list of conditions and the following disclaimer.
 *
 *   - Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 *   - Neither the name of CEA nor the names of its contributors may be used to
 *     endorse or promote products derived from this software without specific prior
 *     written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 *  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *  DISCLAIMED.
 *  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 *  OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 *  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: Reda   Nouacer  (reda.nouacer@cea.fr)
 */

#include <unisim/component/tlm2/processor/hcs12x/s12xgate.hh>

namespace unisim {
namespace component {
namespace tlm2 {
namespace processor {
namespace s12xgate {

S12XGATE::S12XGATE(const sc_module_name& name, Object *parent) :
	Object(name, parent)
	, sc_module(name)
	, XGATE(name, parent)

	, target_socket("slave_socket")
	, xint_interrupt_request("interrupt_request")

	, bus_cycle_time_int(250000)
	, param_bus_cycle_time_int("bus-cycle-time", this, bus_cycle_time_int)

	, nice_time(1.0, SC_US)
	, next_nice_time()
	, param_nice_time("nice-time", this, nice_time)
	, cpu_cycle_time()
	, bus_cycle_time()
	, cpu_time()
	, bus_time()
	, last_cpu_time()
	, core_clock_time()
	, core_clock_int(250000)
	, param_core_clock("core-clock", this, core_clock_int)

	, enable_fine_timing(false)
	, param_enable_fine_timing("enable-fine-timing", this, enable_fine_timing)

	, xgate_enable_event()
	, xgate_idle_event()
	, xgate_newthread_event()

	, verbose_tlm_bus_synchronize(false)
	, param_verbose_tlm_bus_synchronize("verbose-tlm-bus-synchronize", this, verbose_tlm_bus_synchronize)
	, verbose_tlm_run_thread(false)
	, param_verbose_tlm_run_thread("verbose-tlm-run-thread", this, verbose_tlm_run_thread)
	, verbose_tlm_commands(false)
	, param_verbose_tlm_commands("verbose-tlm-commands", this, verbose_tlm_commands)

	, last_instruction_counter(0)

{

	param_nice_time.SetFormat(unisim::kernel::service::VariableBase::FMT_DEC);
	param_bus_cycle_time_int.SetFormat(unisim::kernel::service::VariableBase::FMT_DEC);

	xint_interrupt_request(*this);

	interrupt_request(*this);

	/**
	 * TODO: I have to:
	 *  - I have merge asyncIntThread() and getIntVect() because in the case of XGATE the S12X_INT hasn't need IPL
	 *  - Because XGATE thread aren't interruptible, I have to migrate "asyncintThread" to non-blocking request to do not block the S12X_INT.
	 */

//	interrupt_request.register_b_transport(this, &S12XGATE::asyncIntThread);
	target_socket.register_b_transport(this, &S12XGATE::read_write);
	bus_clock_socket.register_b_transport(this, &S12XGATE::updateBusClock);

	SC_HAS_PROCESS(S12XGATE);

	SC_THREAD(Run);

}

S12XGATE::~S12XGATE()
{

}

bool S12XGATE::BeginSetup() {

	if(!inherited::BeginSetup()) {
		if(debug_enabled && verbose_step)
			*inherited::logger << DebugError
				<< "Error while trying to set up the XGATE cpu"
				<< std::endl << EndDebugError;
		return (false);
	}

	/* check verbose settings */
	if( debug_enabled && inherited::verbose_all) {
		verbose_tlm_bus_synchronize = true;
		verbose_tlm_run_thread = true;
	} else {
		if( verbose_tlm_bus_synchronize)
			*inherited::logger << DebugInfo
				<< "verbose-tlm-bus-synchronize = true"
				<< std::endl << EndDebugInfo;
		if( verbose_tlm_run_thread)
			*inherited::logger << DebugInfo
				<< "verbose-tlm-run-thread = true"
				<< std::endl << EndDebugInfo;
	}

	computeInternalTime();

	if(debug_enabled &&  inherited::verbose_setup) {
		*inherited::logger << DebugInfo
			<< "Setting CPU cycle time to " << cpu_cycle_time.to_string() << std::endl
			<< "Setting Bus cycle time to " << bus_cycle_time.to_string() << std::endl
			<< "Setting nice time to " << nice_time.to_string() << std::endl
			<< EndDebugInfo;
	}

	this->Reset();

	return (true);


}

bool S12XGATE::Setup(ServiceExportBase *srv_export) {
	return (inherited::Setup(srv_export));
}

bool S12XGATE::EndSetup() {
	return (inherited::EndSetup());
}

void S12XGATE::Reset() {
	inherited::Reset();
}

void
S12XGATE ::
Stop(int ret) {
//	// Call BusSynchronize to account for the remaining time spent in the cpu core
//	BusSynchronize();
	Object::Stop(-1);
}


void
S12XGATE ::
Sync() {
	sc_time time_spent = cpu_time - last_cpu_time;
	last_cpu_time = cpu_time;
	wait(time_spent);
}

void S12XGATE::
enbale_xgate() {
	xgate_enabled = true;
	xgate_enable_event.notify();
}

void S12XGATE::
disable_xgate() {
	xgate_enabled = false;
	state = IDLE;

	// cancel pending request
	setXGSWT(0x0000);
}

void S12XGATE::
triggerChannelThread() {

	xgate_newthread_event.notify();

}


void S12XGATE::
Run() {

/**
 *
 * 4.7.4 Thread Execution
		When the RISC core is triggered by an interrupt request (see Figure 1-1) it first executes a vector fetch
		sequence which performs three bus accesses:
		1. A V-cycle to fetch the initial content of the program counter.
		2. A V-cycle to fetch the initial content of the data segment pointer (R1).
		3. A P-cycle to load the initial opcode.
		Afterwards a sequence of instructions (thread) is executed which is terminated by an "RTS" instruction. If
		further interrupt requests are pending after a thread has been terminated, a new vector fetch will be
		performed. Otherwise the RISC core will idle until a new interrupt request is received. A thread can not
		be interrupted by an interrupt request.
 *
 */

	uint8_t opCycles = 0;

	while(1) {

		cout << "XGATE::Run (1)" << endl;

		if (state == STOP) {
			wait(xgate_idle_event);
			continue;
		}

		cout << "XGATE::Run (2)" << endl;
		if (!xgate_enabled) {
			wait(xgate_enable_event);
			continue;
		}

		cout << "XGATE::Run (3)" << endl;
		if (state == IDLE) {
			wait(xgate_newthread_event);
		}

		bool found = false;
		address_t channelID = 0;

		if (hasAsynchronousInterrupt()) {
			found = true;
			channelID = getIntVector();
			ackAsynchronousInterrupt();

			cout << "XGATE::Run (4)" << endl;
		} else {
			cout << "XGATE::Run (5)" << endl;

			// is there an S12X SW Trigger pending request ?
			for (uint8_t i=0,j=1; i<8; i++,j=j*2) {
				// is a pending SW Trigger request on that channel ?
				if ((getXGSWT() & j) != 0) {

					cout << "XGATE::Run (6)" << endl;

					channelID = sofwtare_channel_id[i];

					found = true;

					break;
				}
			}
		}

		/**
		 * Get Initial Thread Context
		 *  - Initial program counter = XGVBR + ChannelID*4
		 *  - Initial variable pointer = XGVBR + ChannelID*4 + 2
		 *
		 *  Note1: register R1 is preloaded with the initial variable pointer of the channel’s service request vector
		 *  Note2: software_channel_id array is set by means of parameters during simulator setup phase.
		 */

		if (!found) {
			state = IDLE;
			continue;
		} else {
			state = RUNNING;

			cout << "XGATE::Run  getVec of channelID " << std::hex << (unsigned int) channelID << " : vector= " << std::hex << (unsigned int) (getXGVBR() + channelID * 4) << endl;

			address_t newPC = memRead16(getXGVBR() + channelID * 4);
			setXGPC(newPC);

			uint16_t variablePtr = memRead16(getXGVBR() + channelID * 4 + 2);
			setXGRx(variablePtr, 1);

			setXGCHID(channelID);
		}

		currentThreadTerminated = false;
		while (!currentThreadTerminated) {

			cout << "XGATE::Run (7)" << endl;

			if(debug_enabled && verbose_tlm_run_thread)
				*inherited::logger << DebugInfo
					<< "Executing step"
					<< std::endl << EndDebugInfo;

			opCycles = inherited::step();

			if(debug_enabled && verbose_tlm_run_thread)
				*inherited::logger << DebugInfo
					<< "Finished executing step"
					<< std::endl << EndDebugInfo;

			sc_time & time_per_instruction = opCyclesArray[opCycles];

			if (enable_fine_timing) {
				if (debug_enabled) {
					std::cerr << "XGATE: time_per_instruction=" << time_per_instruction << std::endl;
				}

				wait(time_per_instruction);
			} else {
				cpu_time += time_per_instruction;
				if(cpu_time >= next_nice_time) {
					next_nice_time = cpu_time + nice_time;
					Synchronize();
				}
			}
		}

	}
}

void S12XGATE::riseErrorCondition() {
	inherited::riseErrorCondition();
}

tlm_sync_enum S12XGATE::nb_transport_bw( XINT_Payload& payload, tlm_phase& phase, sc_core::sc_time& t) {

	return (TLM_ACCEPTED);
}

tlm_sync_enum S12XGATE::nb_transport_fw(tlm::tlm_generic_payload& payload, tlm_phase& phase, sc_core::sc_time& t) {

	triggerChannelThread();

	reqAsynchronousInterrupt();

	payload.set_response_status( tlm::TLM_OK_RESPONSE );

	return (TLM_ACCEPTED);
}

address_t S12XGATE ::getIntVector()
	/*
	 * The CPU issues a signal that tells the interrupt module to drive
	 * the vector address of the highest priority pending exception onto the system address bus
	 * (the CPU12 does not provide this address)
	 *
	 * Priority is as follow: reset => sw-interrupt => hw-interrupt => spurious interrupt
	 *
	 * If (RAM_ACC_VIOL | SYS || SWI || TRAP) return IVBR;
	 * Else return INT_Vector
	 */
{

	address_t address; // get INT_VECTOR from XINT

	INT_TRANS_T *buffer = new INT_TRANS_T();

	tlm::tlm_generic_payload* trans = payloadFabric.allocate();

	trans->set_command( tlm::TLM_READ_COMMAND );
	trans->set_address( 0 );
	trans->set_data_ptr( (unsigned char *) buffer );

	trans->set_data_length( sizeof(INT_TRANS_T) );
	trans->set_streaming_width( sizeof(INT_TRANS_T) );

	trans->set_byte_enable_ptr( 0 );
	trans->set_dmi_allowed( false );
	trans->set_response_status( tlm::TLM_INCOMPLETE_RESPONSE );

	tlm_phase *phase = new tlm_phase(BEGIN_REQ);
	xint_interrupt_request->nb_transport_bw(*trans, *phase, tlm2_btrans_time);

	if (trans->is_response_error() )
		SC_REPORT_ERROR("S12XGATE : ", "Unable to compute channel ID");

	delete phase;
	trans->release();

	address = buffer->getVectorAddress();

	delete buffer;

	return (address);

}

void S12XGATE::assertInterrupt(uint8_t offset, bool isXGATE_flag) {

	if (!inherited::isInterruptEnabled()) return;

	tlm_phase phase = BEGIN_REQ;
	XINT_Payload *payload = xint_payload_fabric.allocate();

	payload->setInterruptOffset(offset);
	payload->setXGATE_shared_channel(isXGATE_flag);

	sc_time local_time = quantumkeeper.get_local_time();

	tlm_sync_enum ret = interrupt_request->nb_transport_fw(*payload, phase, local_time);

	payload->release();

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

double
S12XGATE ::
GetSimulatedTime() {
//	return cpu_time.to_seconds();
	return (cpu_time.to_default_time_units()/1e6);
}


void S12XGATE::updateBusClock(tlm::tlm_generic_payload& trans, sc_time& delay) {

	sc_dt::uint64*   external_bus_clock = (sc_dt::uint64*) trans.get_data_ptr();
    trans.set_response_status( tlm::TLM_OK_RESPONSE );

	bus_cycle_time_int = *external_bus_clock;

	computeInternalTime();
}

void S12XGATE::computeInternalTime() {

	// TODO: check if the following notice is truth for XGATE as well as for CPU12

	/*
	 * From CRG specification (page 100) :
	 * The core clock is twice the bus clock as shown in Figure 2-18.
	 * But note that a CPU cycle corresponds to one bus clock.
	 */
	core_clock_time = sc_time((double)core_clock_int, SC_PS);

	bus_cycle_time = core_clock_time * 2;

	cpu_cycle_time = bus_cycle_time;

	tlm2_btrans_time = sc_time((double)0, SC_PS);

	for (int i=0; i<32; i++) opCyclesArray[i] = cpu_cycle_time * i;

}

void S12XGATE::Synchronize()
{
	sc_time time_spent = cpu_time - last_cpu_time;
	last_cpu_time = cpu_time;
	if (debug_enabled) {
		std::cerr << "HCS12X: time_spent=" << time_spent << std::endl;
	}
	wait(time_spent);
	cpu_time = sc_time_stamp();
//	next_nice_sctime = sc_time_stamp() + nice_sctime;
}

void S12XGATE::read_write( tlm::tlm_generic_payload& trans, sc_time& delay )
{
	tlm::tlm_command cmd = trans.get_command();
	sc_dt::uint64 address = trans.get_address();
	uint8_t* data_ptr = (uint8_t *)trans.get_data_ptr();
	unsigned int data_length = trans.get_data_length();

	assert(address >= baseAddress);

	if (cmd == tlm::TLM_READ_COMMAND) {
		unsigned int data_length = trans.get_data_length();
		memset(data_ptr, 0, data_length);
		inherited::read(address - baseAddress, data_ptr, data_length);
	} else if (cmd == tlm::TLM_WRITE_COMMAND) {
		inherited::write(address - baseAddress, data_ptr, data_length);
	}

	trans.set_response_status( tlm::TLM_OK_RESPONSE );
}


void S12XGATE::busWrite(address_t addr, void *buffer, uint32_t size)
{

	tlm::tlm_generic_payload* trans = payloadFabric.allocate();

	trans->set_command( tlm::TLM_WRITE_COMMAND );
	trans->set_address( addr );
	trans->set_data_ptr( (unsigned char *) buffer );

	trans->set_data_length( size );
	trans->set_streaming_width( size );

	trans->set_byte_enable_ptr( 0 );
	trans->set_dmi_allowed( false );
	trans->set_response_status( tlm::TLM_INCOMPLETE_RESPONSE );

	initiator_socket->b_transport( *trans, tlm2_btrans_time );

	if (trans->is_response_error() )
		SC_REPORT_ERROR("HCS12X : ", "Response error from b_transport");

	trans->release();
}

void S12XGATE::busRead(address_t addr, void *buffer, uint32_t size)
{

	tlm::tlm_generic_payload* trans = payloadFabric.allocate();

	trans->set_command( tlm::TLM_READ_COMMAND );
	trans->set_address( addr );
	trans->set_data_ptr( (unsigned char *) buffer );

	trans->set_data_length( size );
	trans->set_streaming_width( size );

	trans->set_byte_enable_ptr( 0 );
	trans->set_dmi_allowed( false );
	trans->set_response_status( tlm::TLM_INCOMPLETE_RESPONSE );

	initiator_socket->b_transport( *trans, tlm2_btrans_time );

	if (trans->is_response_error() )
		SC_REPORT_ERROR("HCS12X : ", "Response error from b_transport");

	trans->release();
}

} // end of namespace s12xgate
} // end of namespace processor
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim



