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
#include <vector>
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
Client<Loader>(name, parent),
Client<Blob<T> >(name, parent),
Client<Memory<T> >(name, parent),
Service<Loader>(name, parent),
Service<Blob<T> >(name, parent),
unisim::kernel::service::VariableBaseListener(),
loader_import("loader_import", this),
blob_import("blob_import", this),
memory_import("memory_import", this),
loader_export("loader_export", this),
blob_export("blob_export", this),
endianness(E_LITTLE_ENDIAN),
stack_base(0x4000000UL),
stack_size(0x800000UL),
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
param_stack_size("stack-size", this, stack_size,
		"The stack size used for the load and execution of the linux "
		"application. The top of the stack will be stack-base + stack-size."),
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
	param_max_environ.SetFormat(unisim::kernel::service::VariableBase::FMT_DEC);
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
	param_argc.AddListener(this);
	param_argc.NotifyListeners();

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
	param_envc.AddListener(this);
	param_envc.NotifyListeners();

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

template <class T>
void
LinuxLoader<T> ::
DumpSection(unisim::util::debug::blob::Section<T> const &s, int indent)
{
	std::string s_indent_pre(indent + 1, '-');
	std::stringstream ss_indent;
	ss_indent << s_indent_pre << " ";
	std::string s_indent(ss_indent.str().c_str());

	logger << s_indent << "S ";
	logger << "Name: " << s.GetName() << std::endl;
	logger << s_indent << "  Type: ";
	switch ( s.GetType() )
	{
		case unisim::util::debug::blob::Section<T>::TY_UNKNOWN:
			logger << "Unknown ";
			break;
		case unisim::util::debug::blob::Section<T>::TY_NULL:
			logger << "Null    ";
			break;
		case unisim::util::debug::blob::Section<T>::TY_PROGBITS:
			logger << "Progbits";
			break;
		case unisim::util::debug::blob::Section<T>::TY_NOBITS:
			logger << "Nobits  ";
			break;
		case unisim::util::debug::blob::Section<T>::TY_STAB:
			logger << "Stab    ";
			break;
		case unisim::util::debug::blob::Section<T>::TY_STRTAB:
			logger << "Strtab  ";
			break;
	}
	logger << "  Attribute: ";
	switch ( s.GetAttr() )
	{
		case unisim::util::debug::blob::Section<T>::SA_NULL:
			logger << "(None)";
			break;
		case unisim::util::debug::blob::Section<T>::SA_A:
			logger << "A     ";
			break;
		case unisim::util::debug::blob::Section<T>::SA_W:
			logger << "W     ";
			break;
		case unisim::util::debug::blob::Section<T>::SA_AW:
			logger << "AW    ";
			break;
		case unisim::util::debug::blob::Section<T>::SA_X:
			logger << "X     ";
			break;
		case unisim::util::debug::blob::Section<T>::SA_AX:
			logger << "AX    ";
			break;
		case unisim::util::debug::blob::Section<T>::SA_WX:
			logger << "WX    ";
			break;
		case unisim::util::debug::blob::Section<T>::SA_AWX:
			logger << "AWX   ";
			break;
	}
	logger << "  Link: " << s.GetLink();
	logger << "  Alignment: " << s.GetAlignment();
	logger << "  Address: 0x" << std::hex << s.GetAddr() << std::dec;
	logger << "  Size: " << s.GetSize();
	T min_range, max_range;
	s.GetAddrRange(min_range, max_range);
	logger << "  Range: 0x" << std::hex << min_range
		<< "-0x" << max_range << std::dec << std::endl;
}

template <class T>
void
LinuxLoader<T> ::
DumpBlob(unisim::util::debug::blob::Blob<T> const &b, int indent)
{
	std::string s_indent_pre(indent + 1, '+');
	std::stringstream ss_indent;
	ss_indent << s_indent_pre << " ";
	std::string s_indent(ss_indent.str().c_str());

	typename unisim::util::debug::blob::Blob<T>::Capability cap =
		b.GetCapability();
	const char *filename = b.GetFilename();
	T entry = b.GetEntryPoint();
	T stack = b.GetStackBase();
	unisim::util::endian::endian_type endian =
		b.GetEndian();

	if ( indent == 0 )
		logger << DebugInfo
			<< "Blob structure" << std::endl;
	logger << s_indent << "+" << std::endl;
	logger << s_indent << "Capabilities: ";
	std::stringstream ss_cap;
	bool first = true;
	if ( cap & unisim::util::debug::blob::Blob<T>::CAP_FILENAME ) { first = false; ss_cap << "Filename";}
	if ( cap & unisim::util::debug::blob::Blob<T>::CAP_ENTRY_POINT ) 
	{
		if ( !first ) ss_cap << "|";
		first = false; ss_cap << "EntryPoint";
	}
	if ( cap & unisim::util::debug::blob::Blob<T>::CAP_ARCHITECTURE )
	{
		if ( !first ) ss_cap << "|";
		first = false; ss_cap << "Architecture";
	}
	if ( cap & unisim::util::debug::blob::Blob<T>::CAP_STACK_BASE )
	{
		if ( !first ) ss_cap << "|";
		first = false; ss_cap << "StackBase";
	}
	if ( cap & unisim::util::debug::blob::Blob<T>::CAP_ENDIAN )
	{
		if ( !first ) ss_cap << "|";
		first = false; ss_cap << "Endian";
	}
	if ( cap & unisim::util::debug::blob::Blob<T>::CAP_ADDRESS_SIZE )
	{
		if ( !first ) ss_cap << "|";
		first = false; ss_cap << "AddressSize";
	}
	if ( first )
		ss_cap << "(Default)";
	logger << ss_cap.str() << std::endl;
	
	if ( cap & unisim::util::debug::blob::Blob<T>::CAP_FILENAME )
		logger << s_indent << "Filename: "
			<< b.GetFilename() << std::endl;
	if ( cap & unisim::util::debug::blob::Blob<T>::CAP_ENTRY_POINT ) 
		logger << s_indent << "Entry point: 0x"
			<< std::hex << b.GetEntryPoint() << std::dec << std::endl;
	if ( cap & unisim::util::debug::blob::Blob<T>::CAP_ARCHITECTURE )
		logger << s_indent << "Architecture: "
			<< b.GetArchitecture() << std::endl;
	if ( cap & unisim::util::debug::blob::Blob<T>::CAP_STACK_BASE )
		logger << s_indent << "Stack base: "
			<< b.GetStackBase() << std::endl;
	if ( cap & unisim::util::debug::blob::Blob<T>::CAP_ENDIAN )
		logger << s_indent << "Endian: "
			<< (b.GetEndian() == unisim::util::endian::E_LITTLE_ENDIAN ?
					"Little endian" : "Big endian") << std::endl;

	const std::vector<const unisim::util::debug::blob::Section<T> *> &sections =
		b.GetSections();
	for ( typename std::vector<const unisim::util::debug::blob::Section<T> *>::const_iterator it = sections.begin();
			it != sections.end();
			it++ )
		DumpSection(*(*it), indent);

	const std::vector<const unisim::util::debug::blob::Blob<T> *> &blobs =
		b.GetBlobs();
	for ( typename std::vector<const unisim::util::debug::blob::Blob<T> *>::const_iterator it = blobs.begin();
			it != blobs.end();
			it++ )
		DumpBlob(*(*it), indent + 1);

	logger << s_indent << "+" << std::endl;
	
	if ( indent == 0 )
		logger << EndDebugInfo;
}
template<class T>
bool
LinuxLoader<T>::SetupBlob()
{
	if(blob) return true;
	if(!loader_import) return false;
	const unisim::util::debug::blob::Blob<T> *loader_blob = blob_import->GetBlob();
	if(!loader_blob) return false;

	DumpBlob(*loader_blob, 0);
	T start_code = ~((T)0);
	T end_code = 0;
	T start_data = 0;
	T end_data = 0;
	T elf_stack = ~((T)0);
	T elf_bss = 0;
	T elf_brk = 0;

	const std::vector<const unisim::util::debug::blob::Section<T> *> &sections =
		loader_blob->GetSections();
	for ( typename std::vector<const unisim::util::debug::blob::Section<T> *>::const_iterator it = sections.begin();
			it != sections.end();
			it++ )
	{
		typename unisim::util::debug::blob::Section<T>::Attribute attr = (*it)->GetAttr();
		if ( (attr != unisim::util::debug::blob::Section<T>::SA_A) &&
				(attr != unisim::util::debug::blob::Section<T>::SA_AW) &&
				(attr != unisim::util::debug::blob::Section<T>::SA_AX) &&
				(attr != unisim::util::debug::blob::Section<T>::SA_AWX) )
			continue;

		T sec_addr = (*it)->GetAddr();
		T sec_end_addr = sec_addr + (*it)->GetSize();
		if ( sec_addr < start_code )
			start_code = sec_addr;
		if ( start_data < sec_addr )
			start_data = sec_addr;


		if ( (*it)->GetType() == unisim::util::debug::blob::Section<T>::TY_NOBITS )
		{
			if ( sec_addr > elf_bss )
				elf_bss = sec_addr;
			if ( attr & unisim::util::debug::blob::Section<T>::SA_X )
				if ( end_code < sec_addr )
					end_code = sec_addr;
			if ( end_data < sec_addr)
				end_data = sec_addr;
		}
		else
		{
			if ( sec_end_addr > elf_bss )
				elf_bss = sec_end_addr;
			if ( attr & unisim::util::debug::blob::Section<T>::SA_X )
				if ( end_code < sec_end_addr )
					end_code = sec_end_addr;
			if ( end_data < sec_end_addr)
				end_data = sec_end_addr;
		}

		if ( sec_end_addr > elf_brk )
			elf_brk = sec_end_addr;

	}
	logger << DebugInfo
		<< "Loader locations:" << std::endl
		<< " start_code = 0x" << std::hex << start_code << std::endl
		<< " end_code   = 0x" << end_code << std::endl
		<< " start_data = 0x" << start_data << std::endl
		<< " end_data   = 0x" << end_data << std::endl
		<< " elf_stack  = 0x" << elf_stack << std::endl
		<< " elf_bss    = 0x" << elf_bss << std::endl
		<< " elf_brk    = 0x" << elf_brk << std::dec
		<< EndDebugInfo;

	// Fill the stack
	T sp = stack_base + stack_size;

	// Compute the stack size
	T stack_size = 0;
	stack_size += arch_size;                     // account for argc
	stack_size += (argc + 1) * arch_size;        // account for argv and null pointer
	stack_size += (envc + 1) * arch_size;        // account for envp and null pointer
	stack_size += 2 * arch_size;                 // 16 to account for the AUX_VECTOR
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

	stack_size += ((stack_size % arch_size) != 0) ? (arch_size - (stack_size % arch_size)) : 0;
	T stack_address = stack_base - stack_size;
	
	// Fill stack data
	uint8_t *stack_data = (uint8_t *) calloc(stack_size, 1);
	
	T stack_ptr = 0;

	/* Set the stack pointer. Subtract 16 to account for the AUX_VECTOR. */
	/* write argc to stack */
	T target_argc = Host2Target(endianness, (T)argc);
	memcpy(stack_data + stack_ptr, &target_argc, arch_size);
	if ( verbose ) Log(stack_address + stack_ptr, (uint8_t *)&target_argc, arch_size);

	stack_ptr += arch_size;

	/* skip stack_ptr past argv pointer array */
	T arg_address = stack_ptr;
	stack_ptr += (argc + 1) * arch_size; // Note: there's a null pointer after argv

	/* skip env pointer array */
	T env_address = stack_ptr;
	stack_ptr += (envc + 1) * arch_size; // Note: there's a null pointer after envp

	T aux_v_address = stack_ptr;
	stack_ptr += arch_size * 2; /* for aux_v */

	/* write argv to stack */
	for(unsigned int i = 0; i < argc; i++) {
		T target_argv = Host2Target(endianness, stack_address + stack_ptr);
		memcpy(stack_data + (arg_address + (i * arch_size)), &target_argv, arch_size);
		memcpy(stack_data + stack_ptr, argv[i]->c_str(), argv[i]->length() + 1);
		if ( verbose )
		{
			Log(stack_address + arg_address + i * arch_size, (uint8_t *)&target_argv, arch_size);
			Log(stack_address + stack_ptr, (uint8_t *)argv[i]->c_str(), argv[i]->length());
		}
		stack_ptr += argv[i]->length() + 1;
	}

	/* write env to stack */
	for(unsigned int i = 0; i < envc; i++) {
		T target_envp = Host2Target(endianness, stack_address + stack_ptr);
		memcpy(stack_data + (env_address + (i * arch_size)), &target_envp, arch_size);
		memcpy(stack_data + stack_ptr, envp[i]->c_str(), envp[i]->length() + 1);
		if ( verbose )
		{
			Log(stack_address + env_address + i * arch_size, (uint8_t *)&target_envp, arch_size);
			Log(stack_address + stack_ptr, (uint8_t *)envp[i]->c_str(), envp[i]->length());
		}
		stack_ptr += envp[i]->length() + 1;
	}
	
	/* The following two words on the stack are needed on Linux for IA64 !!!! */
	/* This is filling in the AUX_VECTOR */
	T target_at_pagesz = Host2Target(endianness, (T)6); /* AT_PAGESZ */
	memcpy(stack_data + aux_v_address, &target_at_pagesz, arch_size);
	if ( verbose )
		Log(stack_address + aux_v_address, (uint8_t *)&target_at_pagesz, arch_size);
	T target_page_size = Host2Target(endianness, (T)0x4000ULL); /* PAGE_SIZE */
	memcpy(stack_data + (aux_v_address + arch_size), &target_page_size, arch_size);
	if ( verbose )
		Log(stack_address + aux_v_address + arch_size, (uint8_t *)&target_page_size, arch_size);
	
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
VariableBaseNotify(const unisim::kernel::service::VariableBase *var)
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
