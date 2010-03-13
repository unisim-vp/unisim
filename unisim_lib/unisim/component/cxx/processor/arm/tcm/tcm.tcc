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

#include <iostream>
#include <stdlib.h>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace arm {
namespace tcm {

using std::endl;
using std::hex;
using std::dec;

#ifdef SOCLIB

using std::cerr;

#else // SOCLIB

#define LOCATION __FUNCTION__ << ":" << __FILE__ << ":" << __LINE__

using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugError;
using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebug;

#endif // SOCLIB

#if (defined(__GNUC__) && (__GNUC__ >= 3))
#define INLINE inline __attribute__((always_inline))
#else
#define INLINE inline
#endif

template <class CONFIG, bool DATA_TCM> const uint32_t TCM<CONFIG, DATA_TCM>::BYTE_SIZE;

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
	verbose_all(false),
	verbose_pr_read(false),
	verbose_pr_write(false),
	verbose_debug_read(false),
	verbose_debug_write(false),
	param_verbose_all("verbose-all", this, verbose_all),
	param_verbose_pr_read("verbose-pr-read", this, verbose_pr_read),
	param_verbose_pr_write("verbose-pr-write", this, verbose_pr_write),
	param_verbose_debug_read("verbose-debug-read", this, verbose_debug_read),
	param_verbose_debug_write("verbose-debug-write", this, verbose_debug_write),
	logger(*this) {
	/* initialize the memory to 0 */
	memset(data, 0, SIZE);
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
		
		logger << DebugError << LOCATION
			<< "Out of bonds read:" << endl
			<< " - address = 0x" << hex << addr << dec << endl
			<< " - aligned_address = 0x" << hex << aligned_address << dec << endl
			<< " - size = " << size << endl
			<< " - mem_size = " << BYTE_SIZE 
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
		
		logger << DebugInfo << LOCATION
			<< "Writing:" << endl
			<< " - address = 0x" << hex << addr << dec << endl
			<< " - aligned_address = 0x" << hex << aligned_address << dec << endl
			<< " - size = " << size << endl
			<< " - data =" << hex;
		for(unsigned int i = 0; i < size; i++)
			logger << " " << ((unsigned int)buffer[i]);
		logger << dec << EndDebugInfo;
		
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
			
		logger << DebugError << LOCATION
			<< "Out of bonds read:" << endl
			<< " - address = 0x" << hex << addr << dec << endl
			<< " - aligned_address = 0x" << hex << aligned_address << dec << endl
			<< " - size = " << size << endl
			<< " - mem_size = " << BYTE_SIZE 
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
			
		logger << DebugInfo << LOCATION
			<< "Reading:" << endl
			<< " - address = 0x" << hex << addr << dec << endl
			<< " - aligned_address = 0x" << hex << aligned_address << dec << endl
			<< " - size = " << size << endl
			<< " - read data =" << hex;
		for(unsigned int i = 0; i < size; i++)
			logger << " " << ((unsigned int)buffer[i]);
		logger << dec << EndDebugInfo;

#endif // SOCLIB
		
	}
}

#ifndef SOCLIB

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
		logger << DebugInfo << LOCATION
			<< "Reading (debug):" << endl
			<< " - address = 0x" << hex << addr << dec << endl
			<< " - aligned_address = 0x" << hex << aligned_address << dec << endl
			<< " - size = " << size << endl
			<< " - data =" << hex;
		for(unsigned int i = 0; i < size; i++)
			logger << " " << ((uint8_t *)buffer)[i];
		logger << dec << EndDebugInfo;
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
		logger << DebugInfo << LOCATION
			<< "Writing (debug):" << endl
			<< " - address = 0x" << hex << addr << dec << endl
			<< " - aligned_address = 0x" << hex << aligned_address << dec << endl
			<< " - size = " << size << endl
			<< " - data =" << hex;
		for(unsigned int i = 0; i < size; i++)
			logger << " " << ((uint8_t *)buffer)[i];
		logger << dec << EndDebugInfo;
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
	
	return CONFIG::DEBUG_ENABLE && verbose_pr_write;

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
	
	return CONFIG::DEBUG_ENABLE && verbose_pr_read;
	
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
	
	return CONFIG::DEBUG_ENABLE && verbose_debug_write;
	
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
	
	return CONFIG::DEBUG_ENABLE && verbose_debug_read;
	
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
	TCM<CONFIG, true>(name, parent) {
}

template<class CONFIG>
ITCM<CONFIG> ::
ITCM(const char *name,
		Object *parent) :
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
