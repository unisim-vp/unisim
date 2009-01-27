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
 
#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_TCM_TCM_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_TCM_TCM_HH__

#include <inttypes.h>

#ifndef SOCLIB

#include "unisim/kernel/service/service.hh"
#include "unisim/service/interfaces/logger.hh"
#include "unisim/service/interfaces/memory.hh"

#endif // SOCLIB

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace arm {
namespace tcm {

#ifndef SOCLIB

using unisim::kernel::service::Object;
using unisim::kernel::service::Service;
using unisim::kernel::service::Client;
using unisim::kernel::service::Parameter;
using unisim::kernel::service::ServiceImport;
using unisim::kernel::service::ServiceExport;
using unisim::service::interfaces::Memory;
using unisim::service::interfaces::Logger;


#endif // SOCLIB

#ifdef SOCLIB

template<class CONFIG, bool DATA_TCM>
class TCM {

#else // SOCLIB
	
template<class CONFIG, bool DATA_TCM>
class TCM :
	public Service<Memory<typename CONFIG::address_t> >,
	public Client<Logger> {
		
#endif // SOCLIB
		
private:
	typedef typename CONFIG::address_t address_t;
	
public:
	
#ifndef SOCLIB
	
	ServiceExport<Memory<uint64_t> > memory_export;
	ServiceImport<Logger> logger_import;
	
#endif // SOCLIB
	
#ifdef SOCLIB
	
	TCM();
	
#else // SOCLIB
	
	TCM(const char *name,
			Object *parent = 0);
	
#endif // SOCLIB
	
	virtual ~TCM();
	
#ifndef SOCLIB
	
	virtual bool Setup();
	
#endif // SOCLIB
	
	// access methods
	void PrWrite(address_t addr, const uint8_t *buffer, uint32_t size);
	void PrRead(address_t addr, uint8_t *buffer, uint32_t size);
	
#ifndef SOCLIB
	
	// Memory Interface (debugg dervice)
	virtual void Reset();
	virtual bool ReadMemory(address_t addr, void *buffer, uint32_t size);
	virtual bool WriteMemory(address_t addr, const void *buffer, uint32_t size);
	
#endif // SOCLIB

private:
	static const uint32_t SIZE =
		(DATA_TCM?(CONFIG::DTCM_SIZE):(CONFIG::ITCM_SIZE));
	static const uint32_t BYTE_SIZE =
		((uint32_t)1 << (SIZE - 1)) * 1024;
	uint8_t data[BYTE_SIZE];

	// verbose parameters and variables
	bool verbose_all;
	bool verbose_pr_read;
	bool verbose_pr_write;
	bool verbose_debug_read;
	bool verbose_debug_write;
	
#ifndef SOCLIB
	
	Parameter<bool> param_verbose_all;
	Parameter<bool> param_verbose_pr_read;
	Parameter<bool> param_verbose_pr_write;
	Parameter<bool> param_verbose_debug_read;
	Parameter<bool> param_verbose_debug_write;
	
#endif // SOCLIB
	
	// verbose methods
	bool VerbosePrRead();
	bool VerbosePrWrite();
	bool VerboseDebugRead();
	bool VerboseDebugWrite();
};

#ifdef SOCLIB

template<class CONFIG>
class DTCM : 
	public TCM<CONFIG, true> {
public:
	DTCM();
	~DTCM();
};

template<class CONFIG>
class ITCM : 
	public TCM<CONFIG, false> {
public:
	ITCM();
	~ITCM();
};

#else // SOCLIB

template<class CONFIG>
class DTCM : 
	public TCM<CONFIG, true> {
public:
	DTCM(const char *name,
			Object *parent = 0);
	~DTCM();
};

template<class CONFIG>
class ITCM : 
	public TCM<CONFIG, false> {
public:
	ITCM(const char *name,
			Object *parent = 0);
	~ITCM();
};

#endif // SOCLIB

} // end of namespace tcm
} // end of namespace arm
} // end of namespace processor
} // end of namespace component
} // end of namespace cxx
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_TCM_TCM_HH__
