/*
 *  Copyright (c) 2007,
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
 * Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 */
 
#ifndef __UNISIM_COMPONENT_TLM_FSB_SIMPLEBUS_BUS_TCC__
#define __UNISIM_COMPONENT_TLM_FSB_SIMPLEBUS_BUS_TCC__

#include <sstream>

namespace unisim {
namespace component {
namespace tlm {
namespace fsb {
namespace simple_bus {

#define LOCATION File << __FILE__ << Function << __FUNCTION__ << Line << __LINE__

using std::stringstream;
using unisim::kernel::service::Object;
//using unisim::service::interfaces::operator<<;
using unisim::service::interfaces::DebugInfo;
using unisim::service::interfaces::DebugWarning;
using unisim::service::interfaces::DebugError;
using unisim::service::interfaces::EndDebugInfo;
using unisim::service::interfaces::EndDebugWarning;
using unisim::service::interfaces::EndDebugError;
using unisim::service::interfaces::Hex;
using unisim::service::interfaces::Dec;
using unisim::service::interfaces::Endl;
using unisim::service::interfaces::File;
using unisim::service::interfaces::Function;
using unisim::service::interfaces::Line;
	
/****************************************************/
/* Bus request port controller implementation START */
/****************************************************/

template<class ADDRESS_TYPE, unsigned int DATA_SIZE, unsigned int NUM_PROCS, bool DEBUG>
BusMasterPortController<ADDRESS_TYPE, DATA_SIZE, NUM_PROCS, DEBUG>::
BusMasterPortController(const char *_name,
	Bus<ADDRESS_TYPE, DATA_SIZE, NUM_PROCS, DEBUG> *_bus,
	unsigned int _id) :
	name(_name),
	bus(_bus),
	id(_id) {}

/** TlmSendIf interface implementation.
 * Puts the incomming request in the fifo. The request will be handled
 *   when the bus will be ready.
 * 
 * @param message the received message.
 */
template<class ADDRESS_TYPE, unsigned int DATA_SIZE, unsigned int NUM_PROCS, bool DEBUG>
bool 
BusMasterPortController<ADDRESS_TYPE, DATA_SIZE, NUM_PROCS, DEBUG>::
Send(const Pointer<TlmMessage<ReqType, RspType> > &message) {
 	return bus->Send(message, id);
}

/****************************************************/
/* Bus request port controller implementation  STOP */
/****************************************************/

/****************************************************/
/* Bus implementation                         START */
/****************************************************/

template <class ADDRESS_TYPE, unsigned int DATA_SIZE,
		unsigned int NUM_PROCS, bool DEBUG>
Bus<ADDRESS_TYPE, DATA_SIZE, NUM_PROCS, DEBUG> :: 
Bus(const sc_module_name& module_name, Object *parent) :
	Object(module_name, parent),
	Service<Memory<ADDRESS_TYPE> >(module_name, parent),
	Client<Memory<ADDRESS_TYPE> >(module_name, parent),
	Client<Logger>(module_name, parent),
	sc_module(module_name),
	ResponseListener<ReqType, RspType>(),		
	memory_export("memory-export", this),
	memory_import("memory-import", this),
	logger_import("logger-import", this),
	cycle_time(),
	cycle_time_int(0),
	cycle_time_parameter("cycle-time", this, cycle_time_int),
	working_mutex("working_mutex"),
	working(false),
	bus_synchro_event(),
	next_serviced(0),
	chipset_rsp_fifo("chipset_rsp_fifo") {
	Object::SetupDependsOn(logger_import);
	/* create input ports and name them */
	for(unsigned int i = 0; i < NUM_PROCS; i++) {
		stringstream s;
		s << "inport[" << i << "]";
		inport[i] = new sc_export<TransactionSendIf>(s.str().c_str());
	}

	for(unsigned int i = 0; i < NUM_PROCS; i++) {
		stringstream s;
		s << "inport_controller[" << i << "]";
		inport_controller[i] = 
			new BusMasterPortController<ADDRESS_TYPE, DATA_SIZE, NUM_PROCS, DEBUG>
				(s.str().c_str(), this, i);
		/* associate the input port with its implementation */
		(*inport[i])(*inport_controller[i]);
	}
	
	for(unsigned int i = 0; i < NUM_PROCS; i++) {
		stringstream s;
		s << "req_fifo[" << i << "]";
		req_fifo[i] = new sc_fifo<PTransactionMsgType>(s.str().c_str());
	}

	/* create the chipset ports and name them */
	{	stringstream s;
		s << "chipset_outport";
		chipset_outport = new sc_port<TransactionSendIf>(s.str().c_str());
	}

	SC_THREAD(BusClock);
}

template <class ADDRESS_TYPE, unsigned int DATA_SIZE,
		unsigned int NUM_PROCS, bool DEBUG>
Bus<ADDRESS_TYPE, DATA_SIZE, NUM_PROCS, DEBUG> :: 
~Bus() {}

template <class ADDRESS_TYPE, unsigned int DATA_SIZE,
		unsigned int NUM_PROCS, bool DEBUG>
bool
Bus<ADDRESS_TYPE, DATA_SIZE, NUM_PROCS, DEBUG> :: 
Setup() {
	if(!memory_import) {
		if(logger_import) {
			(*logger_import) << DebugError << LOCATION
				<< "No memory connected" << Endl
				<< EndDebugError;
		} else 
			cerr << Object::GetName() << ": No memory connected" << endl;
		return false;
	}

	cerr << "cycle time of " << cycle_time_int << " microseconds" << endl;
	if(DEBUG && logger_import) {
		(*logger_import) << DebugInfo << LOCATION
			<< "cycle time of " << cycle_time_int << " microseconds" << Endl
			<< EndDebugInfo;
	}
	if(!cycle_time_int) {
		if(logger_import) {
			(*logger_import) << DebugError << LOCATION
				<< "cycle_time should be bigger than 0" << Endl
				<< EndDebugInfo;
		} else
			cerr << "cycle_time should be bigger than 0" << endl;
		return false;
	}
	cycle_time = sc_time((double)cycle_time_int, SC_PS);
	return true;
}

template <class ADDRESS_TYPE, unsigned int DATA_SIZE,
		unsigned int NUM_PROCS, bool DEBUG>
void Bus<ADDRESS_TYPE, DATA_SIZE, NUM_PROCS, DEBUG> :: 
Reset() {
}
	
template <class ADDRESS_TYPE, unsigned int DATA_SIZE,
		unsigned int NUM_PROCS, bool DEBUG>
bool 
Bus<ADDRESS_TYPE, DATA_SIZE, NUM_PROCS, DEBUG> :: 
ReadMemory(ADDRESS_TYPE addr, void *buffer, uint32_t size) {
	return memory_import ? 
		memory_import->ReadMemory(addr, buffer, size) : false;
}
	
template <class ADDRESS_TYPE, unsigned int DATA_SIZE,
		unsigned int NUM_PROCS, bool DEBUG>
bool 
Bus<ADDRESS_TYPE, DATA_SIZE, NUM_PROCS, DEBUG> :: 
WriteMemory(ADDRESS_TYPE addr, const void *buffer, uint32_t size) {
	return memory_import ? 
		memory_import->WriteMemory(addr, buffer, size) : false;
}

template <class ADDRESS_TYPE, unsigned int DATA_SIZE,
		unsigned int NUM_PROCS, bool DEBUG>
bool 
Bus<ADDRESS_TYPE, DATA_SIZE, NUM_PROCS, DEBUG> :: 
Send(const PTransactionMsgType &msg, unsigned int id) {
	if(DEBUG && logger_import)
		(*logger_import) << DebugInfo << LOCATION
			<< "Request received on port " << id << Endl
			<< EndDebugInfo;

	/* check if the incomming request can be inserted in its
	 *   request fifo, if no just return returning false, which
	 *   means that the request could not be processed */
	if(!req_fifo[id]->nb_write(msg)) {
		if(logger_import) {
			(*logger_import) << DebugWarning << LOCATION 
				<< "Could not accept incomming request on port " << id
				<< " because fifo full"
				<< Endl << EndDebugWarning;
		}
		return false;
	}
	
	/* if necessary, that is if there was no work to be done,
	 *   notify the BusClock thread that a new request is available */
	bool synchronize = true;
	working_mutex.lock();
	if(working) {
		working_mutex.unlock();
		synchronize = false;
	}
	working = true;
	working_mutex.unlock();

	if(synchronize) BusSynchronize();
	return true;
}
	
template <class ADDRESS_TYPE, unsigned int DATA_SIZE,
		unsigned int NUM_PROCS, bool DEBUG>
void
Bus<ADDRESS_TYPE, DATA_SIZE, NUM_PROCS, DEBUG> :: 
ResponseReceived(const PTransactionMsgType &msg,
	sc_port<TransactionSendIf> &port) {
	/* check if the response comes from the chipset, if so
	 *   put the response in the chipset fifo queue */
	if(&port == chipset_outport) {
		if(DEBUG && logger_import) {
			(*logger_import) << DebugInfo << LOCATION
				<< "Response received on chipset_outport." << Endl
				<< "Request:" << Endl << (*(msg->req)) << Endl
				<< "Response:" << Endl << (*(msg->rsp)) << Endl
				<< EndDebugInfo;
		}
		chipset_rsp_fifo.write(msg);
		/* if necessary, that is if there was no work to be done,
		 *   notify the BusClock thread that a new response from the 
		 *   chipset is available (they are handled as request) */
		bool synchronize = true;
		working_mutex.lock();
		if(working) {
			working_mutex.unlock();
			synchronize = false;
		}
		working = true;
		working_mutex.unlock();

		if(synchronize) BusSynchronize();
		return;
	}
}
	
template <class ADDRESS_TYPE, unsigned int DATA_SIZE,
		unsigned int NUM_PROCS, bool DEBUG>
void 
Bus<ADDRESS_TYPE, DATA_SIZE, NUM_PROCS, DEBUG> :: 
BusSynchronize() {
	/* compute the current bus cycles */
	sc_time cur_time = sc_time_stamp();
	sc_dt::uint64 cur_time_int = cur_time.value();
	sc_dt::uint64 cycle_time_int = cycle_time.value();
	sc_dt::uint64 cur_cycle = cur_time_int / cycle_time_int;
	sc_dt::uint64 cur_cycle_init_int = cur_cycle * cycle_time_int;
	if(cur_cycle_init_int > cur_time_int) {
		if(logger_import)
			(*logger_import) << DebugError << LOCATION
				<< "current cycle time is bigger than simulation time ("
				<< "cur_cycle_init_int = " << cur_cycle_init_int << ", "
				<< "cur_time_int = " << cur_time_int << ")" << Endl
				<< EndDebugError;
		sc_stop();
		wait();
	}
	// check if a message can be send right now
	if(cur_cycle_init_int == cur_time_int) {
		if(DEBUG && logger_import)
			(*logger_import) << DebugInfo << LOCATION
				<< "bus synchronize in 0 time" << Endl
				<< EndDebugInfo;
		// yes a message can be sent right now
		bus_synchro_event.notify(SC_ZERO_TIME);
	} else {
		if(DEBUG && logger_import)
			(*logger_import) << DebugInfo << LOCATION
				<< "bus synchronize in " 
				<< ((cycle_time * (cur_cycle + 1)) - cur_time).to_string() 
				<< Endl
				<< EndDebugInfo;
		bus_synchro_event.notify((cycle_time * (cur_cycle + 1)) - cur_time);
	}
}
	
template <class ADDRESS_TYPE, unsigned int DATA_SIZE,
		unsigned int NUM_PROCS, bool DEBUG>
void 
Bus<ADDRESS_TYPE, DATA_SIZE, NUM_PROCS, DEBUG> :: 
BusClock() {
	while(1) {
		wait(bus_synchro_event);
		
		if(DEBUG && logger_import) {
			(*logger_import) << DebugInfo << LOCATION
				<< "Executing bus cycle" << Endl
				<< EndDebugInfo;
		}
		
		/* start locking in the queues for which is the next fifo that can
		 *   be serviced */
		bool cont = true;
		unsigned int counter = 0;
		do {
			if(next_serviced < NUM_PROCS) {
				if(req_fifo[next_serviced]->num_available() != 0)
					cont = false;
				else
					next_serviced++;
			} else {
				if(chipset_rsp_fifo.num_available() != 0) 
					cont = false;
				else
					next_serviced = 0;
			}
			counter++;
		} while(cont && counter < NUM_PROCS + 2);
		/* if we did not found at least one fifo with one message pending
		 *   show a warning message, because it should have never occurred,
		 * and go back to the beginning of the BusClock loop after indicating
		 *   that there is no work in the bus */
		if(counter == NUM_PROCS + 2) {
			if(logger_import) {
				(*logger_import) << DebugWarning << LOCATION
					<< "Dispatch thread BusClock was requested when there was "
					<< "no message to dispatch, this should never occur" << Endl
					<< EndDebugWarning;
			}
			working_mutex.lock();
			working = false;
			working_mutex.unlock();
			continue;
		}
		/* dispatch the message */
		if(next_serviced == CHIPSET_ID) {
			DispatchChipsetMessage();
		} else {
			DispatchCPUMessage();
		}
		/* the message has been handled:
		 * - increase time
		 * - set the next_serviced to the next fifo
		 * - check if there are still messages to handle, if so synchronize the bus
		 *     again so it will be handled */
		wait(cycle_time);
		wait(SC_ZERO_TIME);
		if(DEBUG && logger_import) {
			(*logger_import) << DebugInfo << LOCATION
				<< "Dispatch finished" << Endl
				<< EndDebugInfo;
		}
		next_serviced += 1;
		next_serviced = next_serviced % (NUM_PROCS + 1);
		bool msg_found = false;
		for(unsigned int i = 0; !msg_found && i < NUM_PROCS; i++) {
			if(req_fifo[i]->num_available() != 0) {
				if(DEBUG && logger_import)
					(*logger_import) << DebugInfo << LOCATION
						<< req_fifo[i]->num_available()
						<< " messages found in req_fifo[" << i << "]"
						<< " at the end of bus cycle" << Endl
						<< EndDebugInfo;
				msg_found = true;
			}
		}
		if(!msg_found) {
			if(chipset_rsp_fifo.num_available() != 0) {
				if(DEBUG && logger_import)
					(*logger_import) << DebugInfo << LOCATION
						<< chipset_rsp_fifo.num_available()
						<< " messages found in chipset_rsp_fifo"
						<< " at the end of bus cycle" << Endl
						<< EndDebugInfo;
				msg_found = true;
			}
		}
		if(msg_found) {
			if(DEBUG && logger_import) {
				(*logger_import) << DebugInfo << LOCATION
					<< "Finished bus cycle but a message found in one of " 
					<< "the fifos (working=" << working << ")" << Endl
					<< EndDebugInfo;
			}
			BusSynchronize();
		} else {
			/* unset the working boolean, there is nothing else to do */
			working_mutex.lock();
			working = false;
			working_mutex.unlock();			
		}
	}
}

template <class ADDRESS_TYPE, unsigned int DATA_SIZE,
		unsigned int NUM_PROCS, bool DEBUG>
void 
Bus<ADDRESS_TYPE, DATA_SIZE, NUM_PROCS, DEBUG> :: 
DispatchChipsetMessage() {
	PTransactionMsgType msg;
	
	/* Responses have higher priority than request comming from the chipset.
	 * Check if a response is available if so, notify the message event. */
	if(chipset_rsp_fifo.num_available() != 0) {
		msg = chipset_rsp_fifo.read();
		if(DEBUG && logger_import) 
			(*logger_import) << DebugInfo << LOCATION
				<< "Dispatching chipset_rsp_fifo message." << Endl
				<< "Request:" << Endl << (*(msg->req)) << Endl 
				<< "Response:" << Endl << (*(msg->rsp)) << Endl
				<< EndDebugInfo;
		msg->GetResponseEvent()->notify(SC_ZERO_TIME);
		return;
	} 
}

template <class ADDRESS_TYPE, unsigned int DATA_SIZE,
		unsigned int NUM_PROCS, bool DEBUG>
void 
Bus<ADDRESS_TYPE, DATA_SIZE, NUM_PROCS, DEBUG> :: 
DispatchCPUMessage() {
	/* get the message */
	PTransactionMsgType msg = req_fifo[next_serviced]->read();
	
	if(DEBUG && logger_import) 
		(*logger_import) << DebugInfo << LOCATION
			<< "Dispatching req_fifo[" << next_serviced << "] message." << Endl
			<< "Request:" << Endl << (*(msg->req)) << Endl 
			<< EndDebugInfo;
	while(!inheritedRspListener::Send(msg, *chipset_outport))
		wait(cycle_time);
	wait(cycle_time);
	
}

/****************************************************/
/* Bus implementation                          STOP */
/****************************************************/

#undef LOCATION

} // end of simple_bus namespace
} // end of namespace fsb
} // end of namespace tlm
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM_FSB_SIMPLEBUS_BUS_TCC__
