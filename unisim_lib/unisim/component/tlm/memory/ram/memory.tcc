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
 * Authors: Gilles Mouchard (gilles.mouchard@cea.fr)
 */
 
#ifndef __UNISIM_COMPONENT_TLM_MEMORY_RAM_MEMORY_TCC__
#define __UNISIM_COMPONENT_TLM_MEMORY_RAM_MEMORY_TCC__

#define LOCATION Function << __FUNCTION__ << File << __FILE__ << Line << __LINE__

namespace unisim {
namespace component {
namespace tlm {
namespace memory {
namespace ram {

//using unisim::service::interfaces::operator<<;
using unisim::service::interfaces::Hex;
using unisim::service::interfaces::Dec;
using unisim::service::interfaces::Endl;
using unisim::service::interfaces::DebugInfo;
using unisim::service::interfaces::DebugWarning;
using unisim::service::interfaces::DebugError;
using unisim::service::interfaces::EndDebugInfo;
using unisim::service::interfaces::EndDebugWarning;
using unisim::service::interfaces::EndDebugError;
using unisim::service::interfaces::Function;
using unisim::service::interfaces::File;
using unisim::service::interfaces::Line;

/* Constructor */
template <class PHYSICAL_ADDR, uint32_t DATA_SIZE, uint32_t PAGE_SIZE, bool DEBUG>
Memory<PHYSICAL_ADDR, DATA_SIZE, PAGE_SIZE, DEBUG>::
Memory(const sc_module_name& name, Object *parent) :
	Object(name, parent),
	Client<Logger>(name, parent),
	sc_module(name),
	unisim::component::cxx::memory::ram::Memory<PHYSICAL_ADDR, PAGE_SIZE>(name, parent),
	slave_port("slave-port"),
	frequency(0),
	param_frequency("frequency", this, frequency),
	logger_import("logger-import", this) {
	slave_port(*this);
	Object::SetupDependsOn(logger_import);
}

/* Destructor */
template <class PHYSICAL_ADDR, uint32_t DATA_SIZE, uint32_t PAGE_SIZE, bool DEBUG>
Memory<PHYSICAL_ADDR, DATA_SIZE, PAGE_SIZE, DEBUG>::
~Memory() {
}

/* ClientIndependentSetup */
template <class PHYSICAL_ADDR, uint32_t DATA_SIZE, uint32_t PAGE_SIZE, bool DEBUG>
bool Memory<PHYSICAL_ADDR, DATA_SIZE, PAGE_SIZE, DEBUG>::
Setup() {
	if(!logger_import) {
		if(DEBUG) 
			cerr << "ERROR(";
		else
			cerr << "WARNING(";
		cerr << __FUNCTION__ << ":"
			 << __FILE__ << ":"
			 << __LINE__ << "): "
			 << "No Logger exists to generate the output messages" << endl;
		if(DEBUG) return false;
	} 
	if(DEBUG && logger_import) 
		(*logger_import) << DebugInfo << LOCATION
			<< " frequency of " << frequency << " Mhz" 
			<< Endl << EndDebugInfo;
	if(!frequency) {
		if(logger_import)
			(*logger_import) << DebugError << LOCATION
				<< "frequency must be different than 0" << Endl
				<< EndDebugError;
		else
			cerr << "ERROR("
				<< Object::GetName() << "::"
				<< __FUNCTION__ << ":"
				<< __FILE__ << ":"
				<< __LINE__ << "): "
				<< "frequency must be different than 0" << endl;  
		return false;
	}
	cycle_time = sc_time(1.0 / (double) frequency, SC_US);
	return unisim::component::cxx::memory::ram::Memory<PHYSICAL_ADDR, PAGE_SIZE>::Setup();
}

/* Tlm Send method to handle incomming memory requests */
template <class PHYSICAL_ADDR, uint32_t DATA_SIZE, uint32_t PAGE_SIZE, bool DEBUG>
bool 
Memory<PHYSICAL_ADDR, DATA_SIZE, PAGE_SIZE, DEBUG>::
Send(const Pointer<TlmMessage<MemoryRequest<PHYSICAL_ADDR, DATA_SIZE>, 
	MemoryResponse<DATA_SIZE> > >& message) {
	const Pointer<MemoryRequest<PHYSICAL_ADDR, DATA_SIZE> >& req = message->GetRequest();
	Pointer<MemoryResponse<DATA_SIZE> > rsp = new(rsp) MemoryResponse<DATA_SIZE>();
	message->SetResponse(rsp);
	
	switch(req->type)
	{
		case MemoryRequest<PHYSICAL_ADDR, DATA_SIZE>::READ: {
				if(DEBUG && logger_import)
					(*logger_import) << DebugInfo << LOCATION
						<< sc_time_stamp().to_string() 
						<< " Send() received a READ request" << Endl
						<< EndDebugInfo; 	
				ReadMemory(req->addr, rsp->read_data, req->size);
				if(logger_import && (req->addr == (PHYSICAL_ADDR)0xD40AC01D)) {
					(*logger_import) << DebugInfo << LOCATION
						<< "Read memory on:" << Endl
						<< " - addr = 0x" << Hex << req->addr << Dec << Endl
						<< " - size = " << req->size << Endl
						<< " - data =" << Hex;
					for(unsigned int i = 0; i < req->size; i++) 
						(*logger_import) << " " << (unsigned int)rsp->read_data[i];
					(*logger_import) << Dec << Endl << EndDebugInfo;
				}

				sc_event *rsp_ev = message->GetResponseEvent();
				if(rsp_ev) rsp_ev->notify(cycle_time);
			}
			break;
			
		case MemoryRequest<PHYSICAL_ADDR, DATA_SIZE>::WRITE:
			if(DEBUG && logger_import)
				(*logger_import) << DebugInfo << LOCATION
					<< sc_time_stamp().to_string() 
					<< " Send() received a WRITE request" << Endl
					<< EndDebugInfo; 	
		if(logger_import && (req->addr == (PHYSICAL_ADDR)0xD40AC01D)) {
			(*logger_import) << DebugInfo << LOCATION
				<< "Write memory on:" << Endl
				<< " - addr = 0x" << Hex << req->addr << Dec << Endl
				<< " - size = " << req->size << Endl
				<< " - data =" << Hex;
			for(unsigned int i = 0; i < req->size; i++) 
				(*logger_import) << " " << (unsigned int)req->write_data[i];
			(*logger_import) << Dec << Endl << EndDebugInfo;
		}
			WriteMemory(req->addr, req->write_data, req->size);
			break;
	}
	return true;
}

} // end of namespace unisim
} // end of namespace component
} // end of namespace tlm
} // end of namespace memory 
} // end of namespace ram 

#undef LOCATION

#endif // __UNISIM_COMPONENT_TLM_MEMORY_RAM_MEMORY_TCC__
