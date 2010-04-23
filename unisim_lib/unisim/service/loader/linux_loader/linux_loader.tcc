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
 
#ifndef __UNISIM_SERVICE_LOADER_LINUX_LOADER_LINUX_LOADER_TCC__
#define __UNISIM_SERVICE_LOADER_LINUX_LOADER_LINUX_LOADER_TCC__

#include <iostream>
#include <stdlib.h>

namespace unisim {
namespace service {
namespace loader {
namespace linux_loader {

using namespace std;
using namespace unisim::kernel::logger;

template<class T>
LinuxLoader<T>::LinuxLoader(const char *name, Object *parent) :
Object(name, parent),
Client<Loader<T> >(name, parent),
Client<Memory<T> >(name, parent),
Service<Loader<T> >(name, parent),
loader_import("loader_import", this),
memory_import("memory_import", this),
loader_export("loader_export", this),
endianness(E_LITTLE_ENDIAN),
stack_base(0),
max_environ(0),
argc(0),
envc(0),
endianness_string("little-endian"),
param_endian("endianness", this, endianness_string, "The endianness of the binary loaded. Available values are: little-endian and big-endian."),
param_stack_base("stack-base", this, stack_base),
param_max_environ("max-environ", this, max_environ),
param_argc("argc", this, argc),
param_argv("argv", this, argv, MAX_ARGC),
param_envc("envc", this, envc),
param_envp("envp", this, envp, MAX_ENVC),
param_verbose("verbose", this, verbose, "Display verbose information"),
logger(*this)
{
	Object::SetupDependsOn(memory_import);
	Object::SetupDependsOn(loader_import);
}

template<class T>
LinuxLoader<T>::~LinuxLoader() {
}

template<class T>
void
LinuxLoader<T>::OnDisconnect() {
}

template<class T>
bool
LinuxLoader<T>::Setup() {
	/* check that mem and elf_loader are available */
	if(!memory_import) {
		logger << DebugError << "ERROR(" << __FUNCTION__ << ":"
				<< __FILE__ << ":"
				<< __LINE__ << "): "
				<< "Trying to execute LinuxLoader without a memory service connected."
				<< EndDebugError;
		return false;
	}
	if(!loader_import) {
		logger << DebugError << "ERROR(" << __FUNCTION__ << ":"
				<< __FILE__ << ":"
				<< __LINE__ << "): "
				<< "Trying to execute LinuxLoader without an elf_loader service connected."
				<< EndDebugError;
		return false;
	}

	/* set the endianness depending on the endian parameter */
	if ( (endianness_string.compare("little-endian") != 0) &&
			(endianness_string.compare("big-endian") != 0) )
	{
		logger << DebugError
				<< "Error while setting the endianness."
				<< " '" << endianness_string << "' is not a correct"
				<< " value."
				<< " Available values are: little-endian and big-endian."
				<< EndDebugError;
		return false;
	}
	else
	{
		if ( endianness_string.compare("little-endian") == 0 )
			endianness = E_LITTLE_ENDIAN;
		else
			endianness = E_BIG_ENDIAN;
	}

	/* perform the initialization of the linuxloader */
	T stack_ptr;

	/* Set the stack pointer. Subtract 16 to account for the AUX_VECTOR. */
	stack_ptr = stack_base - max_environ - (2 * sizeof(T));
	/* write argc to stack */
	stack_address = stack_ptr;
	T target_argc = Host2Target(endianness, (T)argc);
	memory_import->WriteMemory(stack_ptr, &target_argc, size);
	if ( verbose ) Log(stack_ptr, (uint8_t *)&target_argc, size);

	stack_ptr += size;

	/* skip stack_ptr past argv pointer array */
	arg_address = stack_ptr;
	stack_ptr += (argc + 1) * size;

	/* skip env pointer array */
	T env_address = stack_ptr;
	for(unsigned int i = 0; i < envc; i++)
		stack_ptr += size;
	stack_ptr += size;

	T aux_v_address = stack_ptr;
	stack_ptr += size * 2; /* for aux_v */

	/* write argv to stack */
	for(unsigned int i = 0; i < argc; i++) {
		T target_argv = Host2Target(endianness, stack_ptr);
		memory_import->WriteMemory(arg_address + i * size, &target_argv, size);
		memory_import->WriteMemory(stack_ptr, argv[i].c_str(), argv[i].length());
		if ( verbose )
		{
			Log(arg_address + i * size, (uint8_t *)&target_argv, size);
			Log(stack_ptr, (uint8_t *)argv[i].c_str(), argv[i].length());
		}
		stack_ptr += argv[i].length() + 1;
	}

	/* write env to stack */
	for(unsigned int i = 0; i < envc; i++) {
		T target_envp = Host2Target(endianness, stack_ptr);
		memory_import->WriteMemory(env_address + i * size, &target_envp, size);
		memory_import->WriteMemory(stack_ptr, envp[i].c_str(), envp[i].length());
		if ( verbose )
		{
			Log(env_address + i * size, (uint8_t *)&target_envp, size);
			Log(stack_ptr, (uint8_t *)envp[i].c_str(), envp[i].length());
		}
		stack_ptr += envp[i].length();
	}

	/* The following two words on the stack are needed on Linux for IA64 !!!! */
	/* This is filling in the AUX_VECTOR */
	T target_at_pagesz = Host2Target(endianness, (T)6); /* AT_PAGESZ */
	memory_import->WriteMemory(aux_v_address, &target_at_pagesz, size);
	if ( verbose )
		Log(aux_v_address, (uint8_t *)&target_at_pagesz, size);
	T target_page_size = Host2Target(endianness, (T)0x4000ULL); /* PAGE_SIZE */
	memory_import->WriteMemory(aux_v_address + size, &target_page_size, size);
	if ( verbose )
		Log(aux_v_address + size, (uint8_t *)&target_page_size, size);

	/* checking stack overflow */
	if(stack_ptr + size >= stack_base) {
		cerr << "ERROR(" << __FUNCTION__ << ":"
				<< __FILE__ << ":"
				<< __LINE__ << "): Environment overflow. Need to increase max_environ."
				<< "  (stack_ptr = 0x" << hex << stack_ptr << dec << ", "
				<< "stack_base = 0x" << hex << stack_base << dec << ", "
				<< "size = " << size << ", stack_ptr + size = "
				<< hex << (stack_ptr + size) << dec << ")" << endl;
		exit(-1);
	}
	return true;
}

template<class T>
void
LinuxLoader<T>::
Reset()
{
}

template<class T>
T
LinuxLoader<T>::
GetEntryPoint()
const
{
	return loader_import->GetEntryPoint();
}

template<class T>
T
LinuxLoader<T>::
GetTopAddr()
const
{
	return loader_import->GetTopAddr();
}

template<class T>
T
LinuxLoader<T>::
GetStackBase()
const
{
	return stack_address;
}

template<class T>
void
LinuxLoader<T>::
Log(T addr, const uint8_t *value, uint32_t size)
{
	logger << DebugInfo
			<< "0x" << hex << (unsigned long int)addr << dec
			<< " [" << (unsigned int)size << "] =" << hex;
	for (int i = 0; i < size; i++)
		logger << " " << (unsigned short int)value[i];
	logger << dec << EndDebugInfo;
}

} // end of linux_loader
} // end of loader namespace
} // end of service namespace
} // end of unisim namespace

#endif // __UNISIM_SERVICE_LOADER_LINUX_LOADER_LINUX_LOADER_TCC__
