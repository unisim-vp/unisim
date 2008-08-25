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
 
#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_TCM_TCM_TCC__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_TCM_TCM_TCC__

#ifdef SOCLIB

#include <iostream>

#endif // SOCLIB

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace arm {
namespace tcm {

#ifdef SOCLIB

using std::cerr;
using std::endl;
using std::hex;
using std::dec;

#else // SOCLIB

using unisim::service::interfaces::DebugInfo;
using unisim::service::interfaces::DebugWarning;
using unisim::service::interfaces::DebugError;
using unisim::service::interfaces::EndDebugInfo;
using unisim::service::interfaces::EndDebugWarning;
using unisim::service::interfaces::EndDebugError;
using unisim::service::interfaces::File;
using unisim::service::interfaces::Function;
using unisim::service::interfaces::Line;
using unisim::service::interfaces::Endl;
using unisim::service::interfaces::Hex;
using unisim::service::interfaces::Dec;

#define LOCATION File << __FILE__ << Function << __FUNCTION__ << Line << __LINE__

#endif // SOCLIB

#if (defined(__GNUC__) && (__GNUC__ >= 3))
#define INLINE inline __attribute__((always_inline))
#else
#define INLINE inline
#endif

#ifdef SOCLIB

template<class CONFIG, bool DATA_TCM>
TCM<CONFIG, DATA_TCM> ::
TCM() :
	verbose_all(false),
	verbose_pr_read(false),
	verbose_pr_write(false),
	verbose_debug_read(false),
	verbose_debug_write(false) {
	/* initialize the memory to 0 */
	memset(data, 0, SIZE);
}

#else // SOCLIB

template<class CONFIG, bool DATA_TCM>
TCM<CONFIG, DATA_TCM> ::
TCM(const char *name,
		Object *parent) :
	Object(name, parent),
	Service<Memory<typename CONFIG::address_t> >(name, parent),
	Client<Logger>(name, parent),
	memory_export("memory_import", this),
	logger_import("logger_import", this),
	param_verbose_all("verbose-all", this, verbose_all),
	verbose_all(false),
	param_verbose_pr_read("verbose-pr-read", this, verbose_pr_read),
	verbose_pr_read(false),
	param_verbose_pr_write("verbose-pr-write", this, verbose_pr_write),
	verbose_pr_write(false),
	param_verbose_debug_read("verbose-debug-read", this, verbose_debug_read),
	verbose_debug_read(false),
	param_verbose_debug_write("verbose-debug-write", this, verbose_debug_write),
	verbose_debug_write(false) {
	/* initialize the memory to 0 */
	memset(data, 0, SIZE);
	Object::SetupDependsOn(logger_import);
}

#endif // SOCLIB

template<class CONFIG, bool DATA_TCM>
TCM<CONFIG, DATA_TCM> ::
~TCM() {
}

#ifndef SOCLIB

template<class CONFIG, bool DATA_TCM>
bool
TCM<CONFIG, DATA_TCM> ::
Setup() {
	if(verbose_all) {
		verbose_pr_read = true;
		verbose_pr_write = true;
		verbose_debug_read = true;
		verbose_debug_write = true;
	}
	
	return true;
}

#endif // SOCLIB

template<class CONFIG, bool DATA_TCM>
INLINE void
TCM<CONFIG, DATA_TCM> ::
PrWrite(address_t addr, const uint8_t *buffer, uint32_t size) {
	// align address and perform write
	address_t aligned_address;
	
	aligned_address = addr & (BYTE_SIZE - 1);
	if(aligned_address + size > BYTE_SIZE) {
		
#ifdef SOCLIB
		
		cerr << "Error(" 
			<< __FUNCTION__ << ":" << __FILE__ << ":" << __LINE__ << "): "
			<< endl
			<< "Out of bonds read:" << endl
			<< " - address = 0x" << hex << addr << dec << endl
			<< " - aligned_address = 0x" << hex << aligned_address << dec << endl
			<< " - size = " << size << endl
			<< " - mem_size = " << BYTE_SIZE << endl;
			
#else // SOCLIB
		
		if(logger_import)
			(*logger_import) << DebugError << LOCATION
				<< "Out of bonds read:" << Endl
				<< " - address = 0x" << Hex << addr << Dec << Endl
				<< " - aligned_address = 0x" << Hex << aligned_address << Dec << Endl
				<< " - size = " << size << Endl
				<< " - mem_size = " << BYTE_SIZE << Endl
				<< EndDebugError;
		
#endif
		
		exit(-1);
	}
	if(VerbosePrWrite()) {
		
#ifdef SOCLIB
		
		cerr << "Info(" 
			<< __FUNCTION__ << ":" << __FILE__ << ":" << __LINE__ << "): "
			<< endl
			<< "Writing:" << endl
			<< " - address = 0x" << hex << addr << dec << endl
			<< " - aligned_address = 0x" << hex << aligned_address << dec << endl
			<< " - size = " << size << endl
			<< " - data =" << hex;
		for(unsigned int i = 0; i < size; i++)
			cerr << " " << ((unsigned int)buffer[i]);
		cerr << dec << endl;

#else // SOCLIB
		
		(*logger_import) << DebugInfo << LOCATION
			<< "Writing:" << Endl
			<< " - address = 0x" << Hex << addr << Dec << Endl
			<< " - aligned_address = 0x" << Hex << aligned_address << Dec << Endl
			<< " - size = " << size << Endl
			<< " - data =" << Hex;
		for(unsigned int i = 0; i < size; i++)
			(*logger_import) << " " << ((unsigned int)buffer[i]);
		(*logger_import) << Dec << Endl << EndDebugInfo;
		
#endif // SOCLIB
	}
	memcpy(&data[aligned_address], buffer, size);
}

template<class CONFIG, bool DATA_TCM>
INLINE
void
TCM<CONFIG, DATA_TCM> ::
PrRead(address_t addr, uint8_t *buffer, uint32_t size) {
	// align address and perform read
	address_t aligned_address;
	
	aligned_address = addr & (BYTE_SIZE - 1);
	if(aligned_address + size > BYTE_SIZE) {

#ifdef SOCLIB

		cerr << "Error(" 
			<< __FUNCTION__ << ":" << __FILE__ << ":" << __LINE__ << "): "
			<< endl
			<< "Out of bonds read:" << endl
			<< " - address = 0x" << hex << addr << dec << endl
			<< " - aligned_address = 0x" << hex << aligned_address << dec << endl
			<< " - size = " << size << endl
			<< " - mem_size = " << BYTE_SIZE << endl;
			
#else // SOCLIB
			
		if(logger_import)
			(*logger_import) << DebugError << LOCATION
				<< "Out of bonds read:" << Endl
				<< " - address = 0x" << Hex << addr << Dec << Endl
				<< " - aligned_address = 0x" << Hex << aligned_address << Dec << Endl
				<< " - size = " << size << Endl
				<< " - mem_size = " << BYTE_SIZE << Endl
				<< EndDebugError;

#endif // SOCLIB
		
		exit(-1);
	}
	memcpy(buffer, &data[aligned_address], size);
	if(VerbosePrRead()) {
		
#ifdef SOCLIB

		cerr << "Info(" 
			<< __FUNCTION__ << ":" << __FILE__ << ":" << __LINE__ << "): "
			<< endl
			<< "Reading:" << endl
			<< " - address = 0x" << hex << addr << dec << endl
			<< " - aligned_address = 0x" << hex << aligned_address << dec << endl
			<< " - size = " << size << endl
			<< " - read data =" << hex;
		for(unsigned int i = 0; i < size; i++)
			cerr << " " << ((unsigned int)buffer[i]);
		cerr << dec << endl;
			
#else // SOCLIB
			
		(*logger_import) << DebugInfo << LOCATION
			<< "Reading:" << Endl
			<< " - address = 0x" << Hex << addr << Dec << Endl
			<< " - aligned_address = 0x" << Hex << aligned_address << Dec << Endl
			<< " - size = " << size << Endl
			<< " - read data =" << Hex;
		for(unsigned int i = 0; i < size; i++)
			(*logger_import) << " " << ((unsigned int)buffer[i]);
		(*logger_import) << Dec << Endl << EndDebugInfo;

#endif // SOCLIB
		
	}
}

#ifndef SOCLIB

// Memory Interface (debugg dervice)
template<class CONFIG, bool DATA_TCM>
void
TCM<CONFIG, DATA_TCM> ::
Reset() {
	
}

template<class CONFIG, bool DATA_TCM>
bool
TCM<CONFIG, DATA_TCM> ::
ReadMemory(address_t addr, void *buffer, uint32_t size) {
	// align address and perform read
	address_t aligned_address;
	
	aligned_address = addr & (BYTE_SIZE - 1);
	if(aligned_address + size > BYTE_SIZE) {
		// copy byte per byte
		for(unsigned int i = 0; i < aligned_address + size; i++)
			((uint8_t *)buffer)[i] = data[i & (BYTE_SIZE - 1)];
	} else 
		memcpy(buffer, &data[aligned_address], size);

	if(VerboseDebugRead()) {
		(*logger_import) << DebugInfo << LOCATION
			<< "Reading (debug):" << Endl
			<< " - address = 0x" << Hex << addr << Dec << Endl
			<< " - aligned_address = 0x" << Hex << aligned_address << Dec << Endl
			<< " - size = " << size << Endl
			<< " - data =" << Hex;
		for(unsigned int i = 0; i < size; i++)
			(*logger_import) << " " << ((uint8_t *)buffer)[i];
		(*logger_import) << Dec << Endl << EndDebugInfo;
	}
	return true;
}

template<class CONFIG, bool DATA_TCM>
bool
TCM<CONFIG, DATA_TCM> ::
WriteMemory(address_t addr, const void *buffer, uint32_t size) {
	// align address and perform read
	address_t aligned_address;
	
	aligned_address = addr & (BYTE_SIZE - 1);

	if(VerboseDebugWrite()) {
		(*logger_import) << DebugInfo << LOCATION
			<< "Writing (debug):" << Endl
			<< " - address = 0x" << Hex << addr << Dec << Endl
			<< " - aligned_address = 0x" << Hex << aligned_address << Dec << Endl
			<< " - size = " << size << Endl
			<< " - data =" << Hex;
		for(unsigned int i = 0; i < size; i++)
			(*logger_import) << " " << ((uint8_t *)buffer)[i];
		(*logger_import) << Dec << Endl << EndDebugInfo;
	}
	
	if(aligned_address + size > BYTE_SIZE) {
		// copy byte per byte
		for(unsigned int i = 0; i < aligned_address + size; i++)
			data[i & (BYTE_SIZE - 1)] = ((uint8_t *)buffer)[i];
	} else 
		memcpy(&data[aligned_address], data, size);
	return true;
}

#endif // SOCLIB

template<class CONFIG, bool DATA_TCM>
INLINE
bool
TCM<CONFIG, DATA_TCM> ::
VerbosePrWrite() {
	
#ifdef SOCLIB
	
#ifdef SOCLIB_DEBUG
	return true;
#endif
	return false;
	
#else // SOCLIB
	
	return CONFIG::DEBUG_ENABLE && logger_import && verbose_pr_write;

#endif // SOCLIB
	
}

template<class CONFIG, bool DATA_TCM>
INLINE
bool
TCM<CONFIG, DATA_TCM> ::
VerbosePrRead() {
	
#ifdef SOCLIB
#ifdef SOCLIB_DEBUG
	return true;
#endif
	return false;
	
#else // SOCLIB
	
	return CONFIG::DEBUG_ENABLE && logger_import && verbose_pr_read;
	
#endif // SOCLIB
	
}

template<class CONFIG, bool DATA_TCM>
INLINE
bool
TCM<CONFIG, DATA_TCM> ::
VerboseDebugWrite() {
	
#ifdef SOCLIB
	
#ifdef SOCLIB_DEBUG
	return true;
#endif
	return false;
	
#else // SOCLIB
	
	return CONFIG::DEBUG_ENABLE && logger_import && verbose_debug_write;
	
#endif // SOCLIB
	
}

template<class CONFIG, bool DATA_TCM>
INLINE
bool
TCM<CONFIG, DATA_TCM> ::
VerboseDebugRead() {
	
#ifdef SOCLIB
	
	return CONFIG::DEBUG_ENABLE;
	
#else // SOCLIB
	
	return CONFIG::DEBUG_ENABLE && logger_import && verbose_debug_read;
	
#endif // SOCLIB
	
}

#ifdef SOCLIB

template<class CONFIG>
DTCM<CONFIG> ::
DTCM() :
	TCM<CONFIG, true>() {
}

template<class CONFIG>
ITCM<CONFIG> ::
ITCM() :
	TCM<CONFIG, false>() {
}

#else // SOCLIB

template<class CONFIG>
DTCM<CONFIG> ::
DTCM(const char *name,
		Object *parent) :
	Object(name, parent),
	TCM<CONFIG, true>(name, parent) {
}

template<class CONFIG>
ITCM<CONFIG> ::
ITCM(const char *name,
		Object *parent) :
	Object(name, parent),
	TCM<CONFIG, false>(name, parent) {
}

#endif // SOCLIB
	
template<class CONFIG>
DTCM<CONFIG> ::
~DTCM() {	
}

template<class CONFIG>
ITCM<CONFIG> ::
~ITCM() {	
}

#undef INLINE

#ifndef SOCLIB

#undef LOCATION

#endif // SOCLIB

} // end of namespace tcm
} // end of namespace arm
} // end of namespace processor
} // end of namespace component
} // end of namespace cxx
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_TCM_TCM_TCC__
