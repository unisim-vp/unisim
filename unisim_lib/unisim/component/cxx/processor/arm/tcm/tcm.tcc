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

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace arm {
namespace tcm {

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

#if (defined(__GNUC__) && (__GNUC__ >= 3))
#define INLINE inline __attribute__((always_inline))
#else
#define INLINE inline
#endif

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
	bzero(data, SIZE);
	SetupDependsOn(logger_import);
}

template<class CONFIG, bool DATA_TCM>
TCM<CONFIG, DATA_TCM> ::
~TCM() {
}

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

template<class CONFIG, bool DATA_TCM>
INLINE void
TCM<CONFIG, DATA_TCM> ::
PrWrite(address_t addr, const uint8_t *buffer, uint32_t size) {
	// align address and perform write
	address_t aligned_address;
	
	aligned_address = addr & (BYTE_SIZE - 1);
	if(aligned_address + size >= BYTE_SIZE) {
		if(logger_import)
			(*logger_import) << DebugError << LOCATION
				<< "Out of bonds read:" << Endl
				<< " - address = 0x" << Hex << addr << Dec << Endl
				<< " - aligned_address = 0x" << Hex << aligned_address << Dec << Endl
				<< " - size = " << size << Endl
				<< " - mem_size = " << BYTE_SIZE << Endl
				<< EndDebugError;
		exit(-1);
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
	if(aligned_address + size >= BYTE_SIZE) {
		if(logger_import)
			(*logger_import) << DebugError << LOCATION
				<< "Out of bonds read:" << Endl
				<< " - address = 0x" << Hex << addr << Dec << Endl
				<< " - aligned_address = 0x" << Hex << aligned_address << Dec << Endl
				<< " - size = " << size << Endl
				<< " - mem_size = " << BYTE_SIZE << Endl
				<< EndDebugError;
		exit(-1);
	}
	memcpy(buffer, &data[aligned_address], size);
}

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
	if(aligned_address + size >= BYTE_SIZE) {
		// copy byte per byte
		for(unsigned int i = 0; i < aligned_address + size; i++)
			((uint8_t *)buffer)[i] = data[i & (BYTE_SIZE - 1)];
	} else 
		memcpy(buffer, &data[aligned_address], size);
	return true;
}

template<class CONFIG, bool DATA_TCM>
bool
TCM<CONFIG, DATA_TCM> ::
WriteMemory(address_t addr, const void *buffer, uint32_t size) {
	// align address and perform read
	address_t aligned_address;
	
	aligned_address = addr & (BYTE_SIZE - 1);
	if(aligned_address + size >= BYTE_SIZE) {
		// copy byte per byte
		for(unsigned int i = 0; i < aligned_address + size; i++)
			data[i & (BYTE_SIZE - 1)] = ((uint8_t *)buffer)[i];
	} else 
		memcpy(&data[aligned_address], data, size);
	return true;
}

template<class CONFIG, bool DATA_TCM>
INLINE
bool
TCM<CONFIG, DATA_TCM> ::
VerbosePrWrite() {
	return CONFIG::DEBUG_ENABLE && logger_import && verbose_pr_write;
}

template<class CONFIG, bool DATA_TCM>
INLINE
bool
TCM<CONFIG, DATA_TCM> ::
VerbosePrRead() {
	return CONFIG::DEBUG_ENABLE && logger_import && verbose_pr_read;
}

template<class CONFIG, bool DATA_TCM>
INLINE
bool
TCM<CONFIG, DATA_TCM> ::
VerboseDebugWrite() {
	return CONFIG::DEBUG_ENABLE && logger_import && verbose_debug_write;
}

template<class CONFIG, bool DATA_TCM>
INLINE
bool
TCM<CONFIG, DATA_TCM> ::
VerboseDebugRead() {
	return CONFIG::DEBUG_ENABLE && logger_import && verbose_debug_read;
}

template<class CONFIG>
DTCM<CONFIG> ::
DTCM(const char *name,
		Object *parent) :
	Object(name, parent),
	TCM<CONFIG, true>(name, parent) {
}

template<class CONFIG>
DTCM<CONFIG> ::
~DTCM() {	
}

template<class CONFIG>
ITCM<CONFIG> ::
ITCM(const char *name,
		Object *parent) :
	Object(name, parent),
	TCM<CONFIG, false>(name, parent) {
}

template<class CONFIG>
ITCM<CONFIG> ::
~ITCM() {	
}

#undef INLINE

#undef LOCATION

} // end of namespace tcm
} // end of namespace arm
} // end of namespace processor
} // end of namespace component
} // end of namespace cxx
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_TCM_TCM_TCC__
