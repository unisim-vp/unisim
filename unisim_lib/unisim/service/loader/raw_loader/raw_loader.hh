/*
 *  Copyright (c) 2010,
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
 
#ifndef __UNISIM_SERVICE_LOADER_RAW_LOADER_RAW_LOADER_HH__
#define __UNISIM_SERVICE_LOADER_RAW_LOADER_RAW_LOADER_HH__

#include <inttypes.h>
#include <string>
#include "unisim/service/interfaces/memory.hh"
#include "unisim/kernel/service/service.hh"
#include "unisim/kernel/logger/logger.hh"

namespace unisim {
namespace service {
namespace loader {
namespace raw_loader {

class RawLoader
	: public unisim::kernel::service::Client<
	  	unisim::service::interfaces::Memory<uint64_t> >
{
public:
	unisim::kernel::service::ServiceImport<
		unisim::service::interfaces::Memory<uint64_t> > memory_import;

	RawLoader(const char *name, unisim::kernel::service::Object *parent = 0);
	virtual ~RawLoader();

	virtual bool Setup();

private:
	std::string filename;
	uint64_t base_addr;
	uint64_t size;
	uint32_t verbose;
	
	unisim::kernel::service::Parameter<std::string> param_filename;
	unisim::kernel::service::Parameter<uint64_t> param_base_addr;
	unisim::kernel::service::Parameter<uint64_t> param_size;
	unisim::kernel::service::Parameter<uint32_t> param_verbose;

	unisim::kernel::logger::Logger logger;
};

} // end of namespace raw_loader
} // end of namespace loader
} // end of namespace service
} // end of namespace unisim

#endif // __UNISIM_SERVICE_LOADER_RAW_LOADER_RAW_LOADER_HH__
