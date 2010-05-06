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
 
#ifndef __UNISIM_SERVICE_LOADER_LINUX_LOADER_LINUX_LOADER_HH__
#define __UNISIM_SERVICE_LOADER_LINUX_LOADER_LINUX_LOADER_HH__

#include "unisim/service/interfaces/memory.hh"
#include "unisim/util/endian/endian.hh"
#include "unisim/service/interfaces/loader.hh"
#include "unisim/kernel/service/service.hh"
#include "unisim/kernel/logger/logger.hh"

#include <string>

namespace unisim {
namespace service {
namespace loader {
namespace linux_loader {

using std::string;
using unisim::service::interfaces::Memory;
using unisim::service::interfaces::Loader;
using namespace unisim::util::endian;
using unisim::kernel::service::Service;
using unisim::kernel::service::Client;
using unisim::kernel::service::Object;
using unisim::kernel::service::ServiceImport;
using unisim::kernel::service::ServiceExport;
using unisim::kernel::service::Parameter;
using unisim::kernel::service::ParameterArray;
using unisim::kernel::logger::Logger;

template<class T>
class LinuxLoader :
public Client<Loader<T> >,
public Client<Memory<T> >,
public Service<Loader<T> >
{
public:
	/* Import of the different services */
	ServiceImport<Loader<T> > loader_import;
	ServiceImport<Memory<T> > memory_import;
	/* Exported services */
	ServiceExport<Loader<T> > loader_export;

	/* Constructor/Destructor */
	LinuxLoader(const char *name, Object *parent = 0);
	virtual ~LinuxLoader();

	/* Service methods */
	virtual void OnDisconnect();
	virtual bool Setup();

	/* Service interface methods */
	virtual void Reset();
	virtual T GetEntryPoint() const;
	virtual T GetTopAddr() const;
	virtual T GetStackBase() const;

protected:
	endian_type endianness;
	T stack_base;
	T max_environ;
	unsigned int argc;
	string *argv;
	unsigned int envc;
	string *envp;

	T stack_address;
	T arg_address;

	bool verbose;

private:
	static const int size = sizeof(T);

	string endianness_string;
	Parameter<string> param_endian;
	Parameter<T> param_stack_base;
	Parameter<T> param_max_environ;
	Parameter<unsigned int> param_argc;
	ParameterArray<string> *param_argv;
	Parameter<unsigned int> param_envc;
	ParameterArray<string> *param_envp;

	Parameter<bool> param_verbose;
	Logger logger;

	void Log(T addr, const uint8_t *value, uint32_t size);
};

} // end of linux_loader
} // end of loader namespace
} // end of service namespace
} // end of unisim namespace

#endif // __UNISIM_SERVICE_LOADER_LINUX_LOADER_LINUX_LOADER_HH__
