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

#include <sstream>
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
Client<Blob<T> >(name, parent),
Client<Memory<T> >(name, parent),
Service<Loader<T> >(name, parent),
Service<Blob<T> >(name, parent),
unisim::kernel::service::VariableBase::Notifiable(),
loader_import("loader_import", this),
blob_import("blob_import", this),
memory_import("memory_import", this),
loader_export("loader_export", this),
blob_export("blob_export", this),
endianness(E_LITTLE_ENDIAN),
stack_base(0),
max_environ(0),
argc(0),
argv(),
envc(0),
envp(),
blob(0),
stack_blob(0),
verbose(false),
endianness_string("little-endian"),
param_endian("endianness", this, endianness_string,
		"The endianness of the binary loaded. Available values are:"
		" little-endian and big-endian."),
param_stack_base("stack-base", this, stack_base,
		"The stack base address used for the load and execution of the "
		"linux application"),
param_max_environ("max-environ", this, max_environ,
		"The maximum size of the program environment during its execution."),
param_argc("argc", this, argc,
		"Number of commands in the program execution line (usually at least one"
		" which is the name of the program executed). The different tokens"
		" can be set up with the parameters argv[<n>] where <n> can go up to"
		" argc - 1."),
param_argv(),
param_envc("envc", this, envc,
		"Number of environment variables defined for the program execution."
		" The different variables can be set up with the parameters envp[<n>]"
		" where <n> can go up to envc - 1."),
param_envp(),
param_verbose("verbose", this, verbose, "Display verbose information"),
logger(*this)
{
	param_argc.SetFormat(unisim::kernel::service::VariableBase::FMT_DEC);
	if ( argc )
	{
		for ( unsigned int i = 0; i < argc; i++ )
		{
			std::stringstream argv_name, argv_desc, argv_val;
			argv_name << "argv[" << i << "]";
			argv_desc << "The '" << i << "' token in the command line.";
			argv_val << "Undefined argv[" << i << "] value";
			argv.push_back(new string(argv_val.str().c_str()));
			param_argv.push_back(
					new Parameter<string>(argv_name.str().c_str(), this, 
						*(argv[i]), argv_desc.str().c_str()));
		}
	}

	param_envc.SetFormat(unisim::kernel::service::VariableBase::FMT_DEC);
	if ( envc )
	{
		for ( unsigned int i = 0; i < envc; i++ )
		{
			std::stringstream envp_name, envp_desc, envp_val;
			envp_name << "envp[" << i << "]";
			envp_desc << "The '" << i << "' token in the environment.";
			envp_val << "Undefined envp[" << i << "] value";
			envp.push_back(new string(envp_val.str().c_str()));
			param_envp.push_back(
					new Parameter<string>(envp_name.str().c_str(), this,
						*(envp[i]), envp_desc.str().c_str()));
		}
	}

	loader_export.SetupDependsOn(memory_import);
	loader_export.SetupDependsOn(loader_import);
	loader_export.SetupDependsOn(blob_import);
}

template<class T>
LinuxLoader<T>::~LinuxLoader() {
	for ( ;
			!argv.empty();
			argv.erase(argv.begin()) )
	{
		delete *argv.begin();
	}
	for ( ;
			!param_argv.empty();
			param_argv.erase(param_argv.begin()) )
	{
		delete *param_argv.begin();
	}

	for ( ;
			!envp.empty();
			envp.erase(envp.begin()) )
	{
		delete *envp.begin();
	}
	for ( ;
			!param_envp.empty();
			param_envp.erase(param_envp.begin()) )
	{
		delete *param_envp.begin();
	}
	
	if(blob)
	{
		blob->Release();
	}
	if(stack_blob)
	{
		stack_blob->Release();
	}
}

template<class T>
void
LinuxLoader<T>::OnDisconnect() {
}

template<class T>
bool
LinuxLoader<T>::BeginSetup()
{
	if(blob)
	{
		blob->Release();
		blob = 0;
	}

	if(stack_blob)
	{
		stack_blob->Release();
		stack_blob = 0;
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

	return true;
}

template<class T>
bool
LinuxLoader<T>::SetupLoad()
{
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

	return true;
}

template<class T>
bool
LinuxLoader<T>::SetupBlob()
{
	if(blob) return true;
	if(!loader_import) return false;
	const unisim::util::debug::blob::Blob<T> *loader_blob = blob_import->GetBlob();
	if(!loader_blob) return false;

	// Compute the stack size
	T stack_size = 0;
	stack_size += size;                     // account for argc
	stack_size += (argc + 1) * size;        // account for argv and null pointer
	stack_size += (envc + 1) * size;        // account for envp and null pointer
	stack_size += 2 * size;                 // 16 to account for the AUX_VECTOR
	for(unsigned int i = 0; i < argc; i++)
	{
		stack_size += argv[i]->length() + 1; // account for null terminated string argv[i]
	}
	for(unsigned int i = 0; i < envc; i++)
	{
		stack_size += envp[i]->length() + 1; // account for null terminated string envp[i]
	}
	
	/* checking stack overflow */
	if(stack_size >= max_environ) {
		logger << DebugError << __FUNCTION__ << ":"
				<< __FILE__ << ":"
				<< __LINE__ << "): Environment overflow. Need to increase max_environ."
				<< EndDebugError;
		return false;
	}

	stack_size += ((stack_size % size) != 0) ? (size - (stack_size % size)) : 0;
	T stack_address = stack_base - stack_size;
	
	// Fill stack data
	uint8_t *stack_data = (uint8_t *) calloc(stack_size, 1);
	
	T stack_ptr = 0;

	/* Set the stack pointer. Subtract 16 to account for the AUX_VECTOR. */
	/* write argc to stack */
	T target_argc = Host2Target(endianness, (T)argc);
	memcpy(stack_data + stack_ptr, &target_argc, size);
	if ( verbose ) Log(stack_address + stack_ptr, (uint8_t *)&target_argc, size);

	stack_ptr += size;

	/* skip stack_ptr past argv pointer array */
	T arg_address = stack_ptr;
	stack_ptr += (argc + 1) * size; // Note: there's a null pointer after argv

	/* skip env pointer array */
	T env_address = stack_ptr;
	stack_ptr += (envc + 1) * size; // Note: there's a null pointer after envp

	T aux_v_address = stack_ptr;
	stack_ptr += size * 2; /* for aux_v */

	/* write argv to stack */
	for(unsigned int i = 0; i < argc; i++) {
		T target_argv = Host2Target(endianness, stack_address + stack_ptr);
		memcpy(stack_data + (arg_address + (i * size)), &target_argv, size);
		memcpy(stack_data + stack_ptr, argv[i]->c_str(), argv[i]->length() + 1);
		if ( verbose )
		{
			Log(stack_address + arg_address + i * size, (uint8_t *)&target_argv, size);
			Log(stack_address + stack_ptr, (uint8_t *)argv[i]->c_str(), argv[i]->length());
		}
		stack_ptr += argv[i]->length() + 1;
	}

	/* write env to stack */
	for(unsigned int i = 0; i < envc; i++) {
		T target_envp = Host2Target(endianness, stack_address + stack_ptr);
		memcpy(stack_data + (env_address + (i * size)), &target_envp, size);
		memcpy(stack_data + stack_ptr, envp[i]->c_str(), envp[i]->length() + 1);
		if ( verbose )
		{
			Log(stack_address + env_address + i * size, (uint8_t *)&target_envp, size);
			Log(stack_address + stack_ptr, (uint8_t *)envp[i]->c_str(), envp[i]->length());
		}
		stack_ptr += envp[i]->length() + 1;
	}
	
	/* The following two words on the stack are needed on Linux for IA64 !!!! */
	/* This is filling in the AUX_VECTOR */
	T target_at_pagesz = Host2Target(endianness, (T)6); /* AT_PAGESZ */
	memcpy(stack_data + aux_v_address, &target_at_pagesz, size);
	if ( verbose )
		Log(stack_address + aux_v_address, (uint8_t *)&target_at_pagesz, size);
	T target_page_size = Host2Target(endianness, (T)0x4000ULL); /* PAGE_SIZE */
	memcpy(stack_data + (aux_v_address + size), &target_page_size, size);
	if ( verbose )
		Log(stack_address + aux_v_address + size, (uint8_t *)&target_page_size, size);
	
	stack_blob = new unisim::util::debug::blob::Blob<T>();
	stack_blob->Catch();
	
	stack_blob->SetEndian(endianness);
	
	stack_blob->SetStackBase(stack_address);
	
	unisim::util::debug::blob::Section<T> *stack_section = new unisim::util::debug::blob::Section<T>(
		unisim::util::debug::blob::Section<T>::TY_UNKNOWN,
		unisim::util::debug::blob::Section<T>::SA_A,
		"",
		0,
		0,
		stack_address,
		stack_size,
		stack_data
	);

	stack_blob->AddSection(stack_section);
	
	blob = new unisim::util::debug::blob::Blob<T>();
	blob->Catch();
	blob->AddBlob(loader_blob);
	blob->AddBlob(stack_blob);

	return true;
}

template<class T>
bool
LinuxLoader<T>::Setup(ServiceExportBase *srv_export)
{
	if(srv_export == &loader_export) return SetupLoad();
	if(srv_export == &blob_export) return SetupBlob();
	
	logger << DebugError << "Internal error" << EndDebugError;
	return false;
}

template<class T>
bool
LinuxLoader<T>::EndSetup() {
	return LoadStack();
}

template<class T>
bool
LinuxLoader<T>::
Load()
{
	if(!loader_import->Load()) return false;
	return LoadStack();
}

template <class T>
bool
LinuxLoader<T>::
LoadStack()
{
	if(!memory_import) return false;
	const unisim::util::debug::blob::Section<T> *stack_section = stack_blob->GetSection(0);
	if(!stack_blob) return false;

	T stack_start;
	T stack_end;

	stack_blob->GetAddrRange(stack_start, stack_end);

	if(verbose)
	{
		logger << DebugInfo << "Initializing stack at @0x" << std::hex << stack_start << " - @0x" << stack_end << std::dec << EndDebugInfo;
	}

	if(!memory_import->WriteMemory(stack_section->GetAddr(), stack_section->GetData(), stack_section->GetSize())) return false;
	
	return true;
}

template<class T>
const typename unisim::util::debug::blob::Blob<T> *
LinuxLoader<T>::
GetBlob() const
{
	return blob;
}

template<class T>
void
LinuxLoader<T>::
Log(T addr, const uint8_t *value, uint32_t size)
{
	logger << DebugInfo
			<< "0x" << std::hex << (unsigned long int)addr << std::dec
			<< " [" << (unsigned int)size << "] =" << std::hex;
	for (uint32_t i = 0; i < size; i++)
		logger << " " << (unsigned short int)value[i];
	logger << std::dec << EndDebugInfo;
}

template<class T>
void
LinuxLoader<T>::
VariableNotify(const char *name)
{
	if ( argc != argv.size() )
	{
		if ( argc < argv.size() )
		{
			// we have to delete entries on argv vector
			for ( ;
					param_argv.size() != argc;
					param_argv.pop_back() )
			{
				delete *param_argv.rbegin();
			}
			for ( ;
					argv.size() != argc;
					argv.pop_back() )
			{
				delete *argv.rbegin();
			}
		}
		else
		{
			// we have to add entries on the argv vector
			while ( argv.size() != argc )
			{
				unsigned int i = argv.size();
				std::stringstream argv_name, argv_desc, argv_val;
				argv_name << "argv[" << i << "]";
				argv_desc << "The '" << i << "' token in the command line.";
				argv_val << "Undefined argv[" << i << "] value";
				argv.push_back(new string(argv_val.str().c_str()));
				param_argv.push_back(
						new Parameter<string>(argv_name.str().c_str(), this, 
							*(argv[i]), argv_desc.str().c_str()));
			}
		}
	}

	if ( envc != envp.size() )
	{
		if ( envc < envp.size() )
		{
			// we have to delete entries on envp vector
			for ( ;
					param_envp.size() != argc;
					param_envp.pop_back() )
			{
				delete *param_envp.rbegin();
			}
			for ( ;
					envp.size() != argc;
					envp.pop_back() )
			{
				delete *envp.rbegin();
			}
		}
		else
		{
			// we have to add entries on the envp vector
			while ( envp.size() != envc )
			{
				unsigned int i = envp.size();
				std::stringstream envp_name, envp_desc, envp_val;
				envp_name << "envp[" << i << "]";
				envp_desc << "The '" << i << "' token in the environment.";
				envp_val << "Undefined envp[" << i << "] value";
				envp.push_back(new string(envp_val.str().c_str()));
				param_envp.push_back(
						new Parameter<string>(envp_name.str().c_str(), this,
							*(envp[i]), envp_desc.str().c_str()));
			}
		}
	}
}

} // end of linux_loader
} // end of loader namespace
} // end of service namespace
} // end of unisim namespace

#endif // __UNISIM_SERVICE_LOADER_LINUX_LOADER_LINUX_LOADER_TCC__
