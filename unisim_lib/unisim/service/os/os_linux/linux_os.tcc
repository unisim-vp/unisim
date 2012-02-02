/*
 *  Copyright (c) 2011,
 *  Commissariat a l'Energie Atomique (CEA)
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *
 *   - Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 *
 *   - Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 *   - Neither the name of CEA nor the names of its contributors may be used to
 *   endorse or promote products derived from this software without specific
 *   prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED.
 *  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY
 *  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 *  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 */
 
#ifndef __UNISIM_SERVICE_OS_OS_LINUX_LINUX_OS_TCC__
#define __UNISIM_SERVICE_OS_OS_LINUX_LINUX_OS_TCC__

#include <string>
#include <sstream>

#include "unisim/kernel/service/service.hh"
#include "unisim/kernel/logger/logger.hh"
#include "unisim/service/interfaces/linux_os.hh"
#include "unisim/service/interfaces/loader.hh"
#include "unisim/service/interfaces/memory.hh"
#include "unisim/service/interfaces/memory_injection.hh"
#include "unisim/service/interfaces/registers.hh"
// #include "unisim/service/os/linux_os/linux_os_exception.hh"
#include "unisim/util/endian/endian.hh"
#include "unisim/util/debug/register.hh"
#include "unisim/util/likely/likely.hh"

#include "unisim/service/os/os_linux/linux_os.hh"
#include "unisim/util/os/linux/linux.hh"
#include "unisim/util/os/linux/arm.hh"
#include "unisim/util/os/linux/ppc.hh"

#define LOCATION 	" - location = " << __FUNCTION__ << ":unisim/service/os/os_linux/linux_os.tcc:" << __LINE__

namespace unisim {
namespace service {
namespace os {
namespace os_linux {

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugError;
using unisim::kernel::logger::EndDebug;

/** Constructor. */
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LinuxOS(const char *name, unisim::kernel::service::Object *parent)
    : unisim::kernel::service::Object(name, parent)
    , unisim::kernel::service::Service<unisim::service::interfaces::LinuxOS>(
        name, parent)
    //, unisim::kernel::service::Service<unisim::service::interfaces::Loader>(
    //    name, parent)
    , unisim::kernel::service::Client<
      unisim::service::interfaces::Memory<ADDRESS_TYPE> >(name, parent)
    , unisim::kernel::service::Client<
      unisim::service::interfaces::MemoryInjection<ADDRESS_TYPE> >(name, parent)
    , unisim::kernel::service::Client<unisim::service::interfaces::Registers>(
        name, parent)
    , unisim::util::os::linux_os::LinuxMemoryInterface<ADDRESS_TYPE>()
    , unisim::util::os::linux_os::LinuxRegisterInterface<PARAMETER_TYPE>()
    // , loader_export_("loader-export", this)
    , linux_os_export_("linux-os-export", this)
    , memory_import_("memory-import", this)
    , memory_injection_import_("memory-injection-import", this)
    , registers_import_("registers-import", this)
    , linuxlib_stream_()
    , linuxlib_(&linuxlib_stream_)
    , system_("")
    , param_system_("system", this, system_, "Emulated system architecture "
                   "available values are \"arm\", \"arm-eabi\" and \"powerpc\"")
    , endianness_(unisim::util::endian::E_LITTLE_ENDIAN)
    , endianness_string_("little-endian")
    , param_endian_("endianness", this, endianness_string_,
                   "The endianness of the binary loaded. Available values are:"
                   " little-endian and big-endian.")
    , memory_page_size_(4096)
    , param_memory_page_size_("memory-page-size", this, memory_page_size_)
    , stack_base_(0x4000000UL)
    , param_stack_base_("stack-base", this, stack_base_,
                        "The stack base address used for the load and execution"
                        " of the linux application")
    , stack_size_(0x800000UL)
    , param_stack_size_("stack-size", this, stack_size_,
                        "The stack size used for the load and execution of the"
                        " linux application. The top of the stack will be"
                        " stack-base + stack-size.")
    , max_environ_(0)
    , param_max_environ_("max-environ", this, max_environ_,
                         "The maximum size of the program environment during"
                         " its execution.")
    , binary_()
    , param_binary_("binary", this, binary_,
                    "The binary to execute on the target simulator. Usually it"
                    " is the same value than the argv[1] parameter.")
    , argc_(0)
    , param_argc_("argc", this, argc_,
                  "Number of commands in the program execution line (usually at"
                  " least one which is the name of the program executed). The"
                  " different tokens can be set up with the parameters"
                  " argv[<n>] where <n> can go up to argc - 1.")
    , argv_()
    , param_argv_()
    , apply_host_environment_(true)
    , param_apply_host_environment_("apply-host-environment", this,
                                    apply_host_environment_,
                                    "Wether to apply the host environment on"
                                    " the target simulator or use the provided"
                                    " envc and envp.")
    , envc_(0)
    , param_envc_("envc", this, envc_,
                  "Number of environment variables defined for the program"
                  " execution. The different variables can be set up with the"
                  " parameters envp[<n>] where <n> can go up to envc - 1.")
    , envp_()
    , param_envp_()
    , utsname_sysname_("Linux")
    , param_utsname_sysname_("utsname-sysname", this, utsname_sysname_,
                            "The value that the uname system call should"
                            " return. As this service is providing linux"
                            " emulation supoort its value should be 'Linux', so"
                            " you should not modify it.")
    , utsname_nodename_("localhost")
    , param_utsname_nodename_("utsname-nodename", this, utsname_nodename_,
                             "The network node hostname that the uname system"
                             " call should return. Default value is localhost,"
                             " but you could write whatever name you want.")
    , utsname_release_("2.6.27.35")
    , param_utsname_release_("utsname-release", this, utsname_release_,
                            "The kernel realese information that the uname"
                            " system call should return. This should usually"
                            " match the linux-kernel parameter.")
    , utsname_version_("#UNISIM SMP Fri Mar 12 05:23:09 UTC 2010")
    , param_utsname_version_("utsname-version", this, utsname_version_,
                            "The kernel version information that the uname"
                            " system call should return.")
    , utsname_machine_("armv5")
    , param_utsname_machine_("utsname-machine", this, utsname_machine_,
                            "The machine information that the uname system"
                            " call should  return. This should be one of the"
                            " supported architectures (the  system parameter,"
                            " that is, arm or powerpc) or a specific model "
                            " derived from it (i.e., arm926ejs).")
    , utsname_domainname_("localhost")
    , param_utsname_domainname_("utsname-domainname", this, utsname_domainname_,
                               "The domain name information that the uname"
                               " system call should return.")
    , logger_(*this)
    , verbose_(false)
    , param_verbose_("verbose", this, verbose_) {
  param_max_environ_.SetFormat(unisim::kernel::service::VariableBase::FMT_DEC);

  param_argc_.SetFormat(unisim::kernel::service::VariableBase::FMT_DEC);
  for (unsigned int i = 0; i < ((argc_ == 0)?1:argc_); i++) {
    std::stringstream argv_name, argv_desc, argv_val;
    argv_name << "argv[" << i << "]";
    argv_desc << "The '" << i << "' token in the command line.";
    argv_val << "Undefined argv[" << i << "] value";
    argv_.push_back(new std::string(argv_val.str().c_str()));
    param_argv_.push_back(
        new unisim::kernel::service::Parameter<std::string>(
            argv_name.str().c_str(), this, *(argv_[i]),
            argv_desc.str().c_str()));
  }
  //param_argc.AddListener(this);
  //param_argc.NotifyListeners();

  param_envc_.SetFormat(unisim::kernel::service::VariableBase::FMT_DEC);
  for (unsigned int i = 0; i < ((envc_ == 0)?1:envc_); i++) {
    std::stringstream envp_name, envp_desc, envp_val;
    envp_name << "envp[" << i << "]";
    envp_desc << "The '" << i << "' token in the environment.";
    envp_val << "Undefined envp[" << i << "] value";
    envp_.push_back(new std::string(envp_val.str().c_str()));
    param_envp_.push_back(
        new unisim::kernel::service::Parameter<std::string>(
            envp_name.str().c_str(), this, *(envp_[i]),
            envp_desc.str().c_str()));
  }
  //param_envc.AddListener(this);
  //param_envc.NotifyListeners();


}

/** Destructor. */
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::~LinuxOS() {
}

/** Method to execute when the LinuxOS is disconnected from its client. */
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::OnDisconnect() {
  // NOTHING ?
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
bool LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::BeginSetup() {
  // check the endianness parameter
  if ((endianness_string_.compare("little-endian") != 0) &&
      (endianness_string_.compare("big-endian") != 0)) {
    logger_ << DebugError
        << "Unknown endian value. Correct values are:"
        << " little-endian and big-endian"
        << EndDebugError;
    return false;
  } else {
    if (endianness_string_.compare("little-endian") == 0)
      endianness_ = unisim::util::endian::E_LITTLE_ENDIAN;
    else
      endianness_ = unisim::util::endian::E_BIG_ENDIAN;
  }

  // check that the given system is supported
  // NOTE: This should not be done because the linux library is already doing it
  if ((system_.compare("arm") != 0) &&
      (system_.compare("arm-eabi") != 0) &&
      (system_.compare("powerpc") != 0)) {
    logger_ << DebugError
        << "Unsupported system (" << system_ << "), this service only supports"
        << " arm and ppc systems" << std::endl
        << LOCATION
        << EndDebugError;
    return false;
  }

  // set up the different linuxlib parameters
  linuxlib_.SetVerbose(true);

  // set the linuxlib command line
  if (argc_ != 0) {
    std::vector<std::string> cmdline;
    for (unsigned int i = 0; i < argc_; i++)
      cmdline.push_back(*argv_[i]);
    bool success = linuxlib_.SetCommandLine(cmdline);
    if (!success) {
      logger_ << DebugError
          << "Could not set the command line."
          << EndDebugError;
      return false;
    }
  } else {
    logger_ << DebugError
        << "No command line was given for the target simulator."
        << EndDebugError;
    return false;
  }

  // set the linuxlib environment
  if (envc_ != 0) {
    std::vector<std::string> envparms;
    for (unsigned int i = 0; i < envc_; i++)
      envparms.push_back(*envp_[i]);
    bool success = linuxlib_.SetEnvironment(envparms);
    if (!success) {
      logger_ << DebugError
          << "Could not set the application environment."
          << EndDebugError;
      return false;
    }
  }

  // set the linuxlib option to set the target environment with the host
  // environment
  linuxlib_.SetApplyHostEnvironment(apply_host_environment_);

  // set the binary that will be simulated in the target simulator
  {
    bool success = linuxlib_.AddLoadFile(binary_.c_str());
    if (!success) {
      logger_ << DebugError
          << "Could not set the binary file to simulate on the target"
          << " simulator." << EndDebugError;
      return false;
    }
  }

  // set the system type of the target simulator (should be the same than the
  // binary)
  {
    bool success = linuxlib_.SetSystemType(system_.c_str());
    if (!success) {
      logger_ << DebugError
          << "System type not supported (\"" << system_ << "\")."
          << EndDebugError;
      return false;
    }
  }

  // set the endianness of the target simulator
  linuxlib_.SetEndianness(endianness_);
  // .. the stack base address
  linuxlib_.SetStackBase(stack_base_);
  // .. the stack size
  linuxlib_.SetStackSize(stack_size_);
  // .. and memory page size
  linuxlib_.SetMemoryPageSize(memory_page_size_);
  // .. and the uname information
  linuxlib_.SetUname(utsname_sysname_.c_str(), utsname_nodename_.c_str(),
                     utsname_release_.c_str(), utsname_version_.c_str(),
                     utsname_machine_.c_str(), utsname_domainname_.c_str());

  // now it is time to try to run the initialization of the linuxlib
  {
    bool success = linuxlib_.Load();
    if (!success) {
      logger_ << DebugError
          << "Could not initialize the linux support with the given parameters"
          << ", please check them."
          << EndDebugError;
      return false;
    }
  }

  linuxlib_.SetRegisterInterface(*this);
  linuxlib_.SetMemoryInterface(*this);
  linuxlib_.SetControlInterface(*this);

  return true;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
bool LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::Setup(
    unisim::kernel::service::ServiceExportBase *srv_export) {
  //if ( srv_export == &loader_export ) {
    //return true;
  //}

  if ( srv_export == &linux_os_export_ ) {
    if (!linuxlib_.SetupTarget()) {
      logger_ << DebugError << "Could not setup the linux system"
          << EndDebugError;
      return false;
    }
    return true;
  }

  logger_ << DebugError << "Internal error" << EndDebugError;

  return false;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
bool LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::EndSetup() {
  return true;
  //if ( (system_.compare("arm") == 0) || (system_.compare("arm-eabi") == 0) )
  //{
    //if ( !LoadARM() )
    //{
      //logger << DebugError
        //<< "Failed the ARM setup."
        //<< EndDebugError;
      //return false;
    //}
    //return true;
  //}

  //if ( system_.compare("powerpc") == 0 )
  //{
    //if ( !LoadPPC() )
    //{
      //logger << DebugError
        //<< "Failed the PowerPC setup."
        //<< EndDebugError;
      //return false;
    //}
    //return true;
  //}
  
  //logger << DebugError
    //<< "Unknown system to load (supported arm and powerpc)."
    //<< EndDebugError;
  //return false;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::ExecuteSystemCall(int id) {
  linuxlib_stream_.str("");
  linuxlib_.ExecuteSystemCall(id);
  if (unlikely(verbose_)) {
    if (!linuxlib_stream_.str().empty()) {
      logger_ << DebugInfo
          << linuxlib_stream_.str()
          << EndDebugInfo;
    }
  }
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
bool LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::ReadMemory(ADDRESS_TYPE addr, 
    uint8_t * const buffer, ADDRESS_TYPE size) {
  if (!memory_import_) return false;
  return memory_import_->ReadMemory(addr, buffer, size);
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
bool LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::WriteMemory(ADDRESS_TYPE addr,
    uint8_t const * const buffer, ADDRESS_TYPE size) {
  if (!memory_import_) return false;
  return memory_import_->WriteMemory(addr, buffer, size);
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
bool LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::InjectReadMemory(ADDRESS_TYPE addr, 
    uint8_t * const buffer, ADDRESS_TYPE size) {
  if (!memory_injection_import_) return false;
  return memory_injection_import_->InjectReadMemory(addr, buffer, size);
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
bool LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::InjectWriteMemory(ADDRESS_TYPE addr,
    uint8_t const * const buffer, ADDRESS_TYPE size) {
  if (!memory_injection_import_) return false;
  return memory_injection_import_->InjectWriteMemory(addr, buffer, size);
}

unisim::util::debug::Register * GetRegisterFromId(
    unisim::kernel::service::ServiceImport<unisim::service::interfaces::Registers> &import,
    std::string &system,
    uint32_t id) {
  if (!import) return NULL;
  char const * reg_name = 0;
  if ((system.compare("arm") == 0) || (system.compare("arm-eabi") == 0)) {
    switch(id) {
      case unisim::util::os::linux_os::kARM_r0: reg_name = "r0"; break;
      case unisim::util::os::linux_os::kARM_r1: reg_name = "r1"; break;
      case unisim::util::os::linux_os::kARM_r2: reg_name = "r2"; break;
      case unisim::util::os::linux_os::kARM_r3: reg_name = "r3"; break;
      case unisim::util::os::linux_os::kARM_r4: reg_name = "r4"; break;
      case unisim::util::os::linux_os::kARM_r5: reg_name = "r5"; break;
      case unisim::util::os::linux_os::kARM_r6: reg_name = "r6"; break;
      case unisim::util::os::linux_os::kARM_r7: reg_name = "r7"; break;
      case unisim::util::os::linux_os::kARM_r8: reg_name = "r8"; break;
      case unisim::util::os::linux_os::kARM_r9: reg_name = "r9"; break;
      case unisim::util::os::linux_os::kARM_r10: reg_name = "r10"; break;
      case unisim::util::os::linux_os::kARM_r11: reg_name = "r11"; break;
      case unisim::util::os::linux_os::kARM_r12: reg_name = "r12"; break;
      case unisim::util::os::linux_os::kARM_sp: reg_name = "sp"; break;
      case unisim::util::os::linux_os::kARM_lr: reg_name = "lr"; break;
      case unisim::util::os::linux_os::kARM_pc: reg_name = "pc"; break;
      default: return NULL;
    }
  } else if (system.compare("powerpc") == 0) {
    switch(id) {
      case unisim::util::os::linux_os::kPPC_r0: reg_name = "r0"; break;
      case unisim::util::os::linux_os::kPPC_r1: reg_name = "r1"; break;
      case unisim::util::os::linux_os::kPPC_r2: reg_name = "r2"; break;
      case unisim::util::os::linux_os::kPPC_r3: reg_name = "r3"; break;
      case unisim::util::os::linux_os::kPPC_r4: reg_name = "r4"; break;
      case unisim::util::os::linux_os::kPPC_r5: reg_name = "r5"; break;
      case unisim::util::os::linux_os::kPPC_r6: reg_name = "r6"; break;
      case unisim::util::os::linux_os::kPPC_r7: reg_name = "r7"; break;
      case unisim::util::os::linux_os::kPPC_r8: reg_name = "r8"; break;
      case unisim::util::os::linux_os::kPPC_r9: reg_name = "r9"; break;
      case unisim::util::os::linux_os::kPPC_r10: reg_name = "r10"; break;
      case unisim::util::os::linux_os::kPPC_r11: reg_name = "r11"; break;
      case unisim::util::os::linux_os::kPPC_r12: reg_name = "r12"; break;
      case unisim::util::os::linux_os::kPPC_r13: reg_name = "r13"; break;
      case unisim::util::os::linux_os::kPPC_r14: reg_name = "r14"; break;
      case unisim::util::os::linux_os::kPPC_r15: reg_name = "r15"; break;
      case unisim::util::os::linux_os::kPPC_r16: reg_name = "r16"; break;
      case unisim::util::os::linux_os::kPPC_r17: reg_name = "r17"; break;
      case unisim::util::os::linux_os::kPPC_r18: reg_name = "r18"; break;
      case unisim::util::os::linux_os::kPPC_r19: reg_name = "r19"; break;
      case unisim::util::os::linux_os::kPPC_r20: reg_name = "r20"; break;
      case unisim::util::os::linux_os::kPPC_r21: reg_name = "r21"; break;
      case unisim::util::os::linux_os::kPPC_r22: reg_name = "r22"; break;
      case unisim::util::os::linux_os::kPPC_r23: reg_name = "r23"; break;
      case unisim::util::os::linux_os::kPPC_r24: reg_name = "r24"; break;
      case unisim::util::os::linux_os::kPPC_r25: reg_name = "r25"; break;
      case unisim::util::os::linux_os::kPPC_r26: reg_name = "r26"; break;
      case unisim::util::os::linux_os::kPPC_r27: reg_name = "r27"; break;
      case unisim::util::os::linux_os::kPPC_r28: reg_name = "r28"; break;
      case unisim::util::os::linux_os::kPPC_r29: reg_name = "r29"; break;
      case unisim::util::os::linux_os::kPPC_r30: reg_name = "r30"; break;
      case unisim::util::os::linux_os::kPPC_r31: reg_name = "r31"; break;
      case unisim::util::os::linux_os::kPPC_cr: reg_name = "cr"; break;
      case unisim::util::os::linux_os::kPPC_cia: reg_name = "cia"; break;
      default: return NULL;
    }
  }
  return import->GetRegister(reg_name);
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
bool LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::GetRegister(uint32_t id,
    PARAMETER_TYPE * const value) {
  unisim::util::debug::Register *reg = 0;
  reg = GetRegisterFromId(registers_import_, system_, id);
  if (reg == NULL) return false;
  reg->GetValue(value);
  return true;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
bool LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::SetRegister(uint32_t id,
    PARAMETER_TYPE value) {
  unisim::util::debug::Register *reg = 0;
  reg = GetRegisterFromId(registers_import_, system_, id);
  if (reg == NULL) return false;
  reg->SetValue(&value);
  return true;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
bool LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::ExitSysCall(int ret) {
  Object::Stop(ret);
  return true;
}

#if 0
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
bool
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LoadARM()
{
	if ( !blob ) 
	{
		logger << DebugError
			<< "blob not found while loading ARM setup."
			<< EndDebugError;
		return false;
	}

	bool status = true;
	const typename unisim::util::debug::blob::Section<ADDRESS_TYPE> 
		*tls_if_section = blob->FindSection("tls_if");
	if ( tls_if_section )
	{
		if ( !memory_import )
		{
			logger << DebugError
				<< "memory_import not found when loading ARM setup (tls)."
				<< EndDebugError;
			status = false;
		}
		else if ( !memory_import->WriteMemory(tls_if_section->GetAddr(), 
						tls_if_section->GetData(), 
						tls_if_section->GetSize()) )
		{
			logger << DebugError
				<< "Could not write into memory tls data while loading"
				<< " ARM setup."
				<< EndDebugError;
			status = false;
		}
	}
	const typename unisim::util::debug::blob::Section<ADDRESS_TYPE> 
		*cmpxchg_if_section = blob->FindSection("cmpxchg_if");
	if ( cmpxchg_if_section )
	{
		if ( !memory_import )
		{
			logger << DebugError
				<< "memory_import not found when loading ARM setup (cmpxchg)."
				<< EndDebugError;
			status = false;
		}
		else if ( !memory_import->WriteMemory(cmpxchg_if_section->GetAddr(), 
					cmpxchg_if_section->GetData(), 
					cmpxchg_if_section->GetSize()) ) 
		{
			logger << DebugError
				<< "Could not write into memory cmpxchg data while loading"
				<< " ARM setup."
				<< EndDebugError;
			status = false;
		}
	}
	
	// Set initial CPU registers
	PARAMETER_TYPE pc = blob->GetEntryPoint();
	if ( unlikely(verbose) )
		logger << DebugInfo
			<< "Setting register \"" << arm_regs[15]->GetName() << "\""
			<< " to value 0x" << hex << pc << dec << endl
			<< LOCATION
			<< EndDebugInfo;
	arm_regs[15]->SetValue(&pc);
	PARAMETER_TYPE st = blob->GetStackBase();
	if ( unlikely(verbose) )
		logger << DebugInfo
			<< "Setting register \"" << arm_regs[13]->GetName() << "\""
			<< " to value 0x" << hex << st << dec << endl
			<< LOCATION
			<< EndDebugInfo;
	arm_regs[13]->SetValue(&st);

	PARAMETER_TYPE envp4;
	PARAMETER_TYPE envp8;
	memory_import->ReadMemory(st + 4, &envp4, sizeof(envp4));
	memory_import->ReadMemory(st + 8, &envp8, sizeof(envp4));	
	arm_regs[1]->SetValue(&envp4);
	arm_regs[2]->SetValue(&envp8);
	return status;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
bool
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LoadPPC()
{
	if ( !blob ) 
	{
		logger << DebugError
			<< "blob not found while loading PPC setup."
			<< EndDebugError;
		return false;
	}

    for (unsigned int i = 0; i < 32; i++) 
	{
		if (!ppc_regs[i]) return false;
		PARAMETER_TYPE zero = 0;
		ppc_regs[i]->SetValue(&zero);
    }

    // Set initial CPU registers
    PARAMETER_TYPE pc = blob->GetEntryPoint();
    if (!ppc_cia) return false;
    ppc_cia->SetValue(&pc);
    PARAMETER_TYPE st = blob->GetStackBase();
	if(!ppc_regs[1]) return false;
    ppc_regs[1]->SetValue(&st);
	
	return true;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
bool
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
Load() 
{
	if(!loader_import) return false;
	if(!loader_import->Load()) return false;
	// Call the system dependent Load
	if (system == "arm") return LoadARM();
	if (system == "arm-eabi") return LoadARM();
	if (system == "powerpc") return LoadPPC();
	
	return false;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
const typename unisim::util::debug::blob::Blob<ADDRESS_TYPE> *
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
GetBlob()
{
	return blob;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
bool
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
ReadMem(ADDRESS_TYPE addr, void *buffer, uint32_t size) 
{
	bool result;
	result = memory_injection_import->InjectReadMemory(addr, buffer, size);
	if(unlikely(verbose)) 
	{
		logger << DebugInfo
			<< "OS read memory: " << endl
			<< " - addr = 0x" << hex << addr << dec << endl
			<< " - size = " << size << endl
			<< " - data =" << hex;
		for (unsigned int i = 0; i < size; i++)
			logger << " " << (unsigned int)(((uint8_t *)buffer)[i]);
		logger << dec << endl
			<< LOCATION
			<< EndDebugInfo;
	}
	return result;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
bool
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
WriteMem(ADDRESS_TYPE addr, const void *buffer, uint32_t size) 
{
	bool result;

	if(unlikely(verbose)) 
	{
		logger << DebugInfo 
			<< "OS write memory: " << endl
			<< " - addr = 0x" << hex << addr << dec << endl
			<< " - size = " << size << endl
			<< " - data =" << hex;
		for (unsigned int i = 0; i < size; i++)
			logger << " " << (unsigned int)(((uint8_t *)buffer)[i]);
		logger << dec << endl
			<< LOCATION
			<< EndDebugInfo;
	}

	result = memory_injection_import->InjectWriteMemory(addr, buffer, size);
	return result;
}

/**
 * Checks that an implementation exists for a syscall name.
 *
 * @param syscall_name the name of the syscall to check.
 * @return True if the syscall exists.
 */
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
bool
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
HasSyscall(const string &syscall_name) 
{
	return syscall_name_map.find(syscall_name) != syscall_name_map.end();
}

/**
 * Checks that an id has been associated to a system call implementation.
 *
 * @param syscall_id id to check. 
 * @return True if syscall_id has been associated to an implementation.
 */
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
bool
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
HasSyscall(int syscall_id) 
{
	return syscall_impl_assoc_map.find(syscall_id) != syscall_impl_assoc_map.end();
}

/**
 * Associates an id (integer) to a the implementation of the syscall specified by syscall_name.
 *
 * @param syscall_name
 * @param syscall_id
 */
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
SetSyscallId(const string &syscall_name, int syscall_id) 
{
	if(unlikely(verbose))
		logger << DebugInfo
			<< "Associating syscall_name \"" << syscall_name << "\""
			<< "to syscall_id number " << syscall_id << endl
			<< LOCATION
			<< EndDebugInfo;
    if (HasSyscall(syscall_name)) 
	{
    	if (HasSyscall(syscall_id)) 
		{
    		stringstream s;
    		s << __FUNCTION__ << ":" << __FILE__ << ":" << __LINE__ << endl;
    		s << "syscall_id already associated to syscall \"" << syscall_name_assoc_map[syscall_id] << "\"" << endl;
    		s << "  you wanted to associate it to " << syscall_name << endl;
    		throw std::runtime_error(s.str().c_str());
    	}
    	syscall_name_assoc_map[syscall_id] = syscall_name;
    	syscall_impl_assoc_map[syscall_id] = syscall_name_map[syscall_name];
    } 
	else 
	{
    	stringstream s;
    	s << __FUNCTION__ << ":" << __FILE__ << ":" << __LINE__ << endl;
    	s << "Unimplemented system call (" << syscall_name << ")";
    	throw std::runtime_error(s.str().c_str());      
    }
}

/**
 * Gets from the cpu calling this method the syscall id to execute and emulates it if it exists.
 */
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
ExecuteSystemCall(int id) 
{
	int translated_id = GetSyscallNumber(id);

	if (HasSyscall(translated_id)) 
	{
		current_syscall_id = translated_id;
		current_syscall_name = syscall_name_assoc_map[translated_id];
		if(unlikely(verbose))
			logger << DebugInfo
				<< "Executing syscall(name = " << current_syscall_name << ";"
				<< " id = " << translated_id << ";"
				<< " unstranslated id = " << id << ")" << endl
				<< LOCATION
				<< EndDebugInfo;
		syscall_t y = syscall_impl_assoc_map[translated_id];
		(this->*y)();
    } 
	else
    	throw UnimplementedSystemCall(__FUNCTION__,
				    __FILE__,
				    __LINE__,
				    translated_id);
}

/**
 * Checks if the endianness of the host and the target system differs.
 */
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
bool 
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
Swap() 
{
#if BYTE_ORDER == BIG_ENDIAN
	if (GetEndianness() == E_BIG_ENDIAN) return false;
	else return true;
#else
    if (GetEndianness() == E_BIG_ENDIAN) return true;
    else return false;
#endif
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void 
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
PerformSwap(void *buf,
		int count) 
{
	int size = count;
    
    if (count > 0) 
	{
    	char *dst_base, *dst;
    	char *src;

    	dst = (char *)malloc(sizeof(char) * count);
    	dst_base = dst;
    	src = (char *)buf + count - 1;
      
    	do 
		{
    		*dst = *src;
        }
 		while (src--, dst++, --count);
    	memcpy(buf, dst_base, size);
    	free(dst_base);
    }
}
  
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
int 
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
StringLength(ADDRESS_TYPE addr) 
{
    int len = 0;
    char buffer;
    
    while (1) 
	{
    	ReadMem(addr, &buffer, 1);
    	if (buffer == 0) return len;
    	len++;
    	addr += 1;
    }
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::LSC_unknown() 
{
	if(unlikely(verbose))
		logger << DebugInfo
			<< "Unimplemented system call #" << current_syscall_id << endl
			<< LOCATION
			<< EndDebugInfo;
	SetSystemCallStatus(-1, true);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_exit() 
{
	string name = "exit";
	int ret;
    
	ret = GetSystemCallParam(0);
	if(unlikely(verbose))
		logger << DebugInfo
			<< "LSC_exit with ret = 0X" << hex << ret << dec
			<< LOCATION
			<< EndDebugInfo;
	Object::Stop(ret);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_read() 
{
	string name = "read";
	int fd;
	size_t count;
	ADDRESS_TYPE buf_addr;
	void *buf;
	size_t ret;
   
	fd = GetSystemCallParam(0);
	buf_addr = (ADDRESS_TYPE) GetSystemCallParam(1);
	count = (size_t) GetSystemCallParam(2);
	
	buf = malloc(count);
   
	if (buf) 
	{
		ret = read(fd, buf, count);
		if (ret > 0) WriteMem(buf_addr, buf, ret);
		free(buf);
	}
	else 
		ret = (size_t)-1;
		
	if(unlikely(verbose))
		logger << DebugInfo
			<< "read(fd=" << fd << ", buf=0x" << hex << buf_addr << dec 
			<< ", count=" << count << ") return " << ret << endl 
			<< LOCATION 
			<< EndDebugInfo;
		
	SetSystemCallStatus(ret, ret < 0);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_write() 
{
	int fd; 
	size_t count;
	void *buf;
	ADDRESS_TYPE buf_addr;
	size_t ret;
   
	fd = GetSystemCallParam(0);
	buf_addr = GetSystemCallParam(1);
	count = (size_t)GetSystemCallParam(2);
	buf = malloc(count);
   
	ret = (size_t)-1;
   
	if (buf) 
	{
		ReadMem(buf_addr, buf, count);
		if ((fd == 1 || fd == 2)) 
		{
			char *tbuf = new char[count + 1];
			memcpy(tbuf, buf, count);
			tbuf[count] = '\0';
			string str(tbuf);
			cout << (str);
			cout << flush;
			ret = count;
			delete[] tbuf;
		}
		else 
			ret = write(fd, buf, count);
		free(buf);
	} 
	else 
		ret = (size_t)-1;
		
	if(unlikely(verbose)) 
		logger << DebugInfo
			<< "write(fd=" << fd << ", buf=0x" << hex << buf_addr << dec 
			<< ", count=" << count << ") return " << ret << endl 
			<< LOCATION
			<< EndDebugInfo;
	SetSystemCallStatus(ret, ret < 0);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void 
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_open() 
{
	ADDRESS_TYPE addr;
	int pathnamelen;
	char *pathname;
	int flags;
	int64_t ret;
	mode_t mode;
    
	addr = GetSystemCallParam(0);
	pathnamelen = StringLength(addr);
	pathname = (char *) malloc(pathnamelen + 1);
	ReadMem(addr, pathname, pathnamelen + 1);
	flags = GetSystemCallParam(1);
	mode = GetSystemCallParam(2);
#if defined(linux)
	ret = open(pathname, flags, mode);
#else
	int host_flags = 0;
	int host_mode = 0;
	// non-Linux open flags encoding may differ from a true Linux host
	if ((flags & LINUX_O_ACCMODE) == LINUX_O_RDONLY) host_flags = (host_flags & ~O_ACCMODE) | O_RDONLY;
	if ((flags & LINUX_O_ACCMODE) == LINUX_O_WRONLY) host_flags = (host_flags & ~O_ACCMODE) | O_WRONLY;
	if ((flags & LINUX_O_ACCMODE) == LINUX_O_RDWR) host_flags = (host_flags & ~O_ACCMODE) | O_RDWR;
	if (flags & LINUX_O_CREAT) host_flags |= O_CREAT;
	if (flags & LINUX_O_EXCL) host_flags |= O_EXCL;
	if (flags & LINUX_O_TRUNC) host_flags |= O_TRUNC;
	if (flags & LINUX_O_APPEND) host_flags |= O_APPEND;
#if defined(WIN32) || defined(WIN64)
	host_mode = mode & S_IRWXU; // Windows doesn't have bits for group and others
#else
	host_mode = mode; // other UNIX systems should have the same bit encoding for protection
#endif
	if (strcmp(pathname, osrelease_filename) == 0)
	{
		{
			std::ofstream fake_file(fake_osrelease_filename);
			fake_file << utsname_release << std::endl;
		}
		ret = open(fake_osrelease_filename, host_flags, host_mode);
	}
	else
	{
		ret = open(pathname, host_flags, host_mode);
	}
#endif
	
	if(unlikely(verbose))
		logger << DebugInfo
			<< "open(pathname=\"" << pathname << "\", flags=0x" << hex << flags 
			<< ", mode=0x" << mode << dec << ") return " << ret << endl 
			<< LOCATION
			<< EndDebugInfo;
		
	free(pathname);
	SetSystemCallStatus(ret, (ret < 0));
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_close() 
{
	int fd;
	int ret;
   
	fd = GetSystemCallParam(0);
	ret = close(fd);
	if(unlikely(verbose))
		logger << DebugInfo
			<< "close(fd=" << fd << ") return " << ret << endl
			<< LOCATION
			<< EndDebugInfo;
	SetSystemCallStatus(ret, ret < 0);
}
	

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_lseek() 
{
	int fildes;
	off_t offset;
	int whence;
	off_t ret;
   
	fildes = GetSystemCallParam(0);
	offset = GetSystemCallParam(1);
	whence = GetSystemCallParam(2);
	ret = lseek(fildes, offset, whence);
	if(unlikely(verbose))
		logger << DebugInfo
			<< "lseek(fildes=" << fildes << ", offset=" << offset 
			<< ", whence=" << whence << ") return " << ret << endl
			<< LOCATION
			<< EndDebugInfo;
  
	if (ret == (off_t)-1) 
		SetSystemCallStatus(errno, true);
	else
		SetSystemCallStatus(ret, false);	
}


template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void 
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_getpid() 
{
	pid_t ret;
    
	ret = (pid_t) 1000;
	if(unlikely(verbose))
		logger << DebugInfo
			<< "getpid() return " << ret << endl
			<< LOCATION
			<< EndDebugInfo;
	SetSystemCallStatus(ret,false);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE> 
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_getuid() 
{
#ifdef WIN32
	uint32_t ret = 0;
#else
	uid_t ret;
    
	ret = getuid();
#endif
	if(unlikely(verbose))
		logger << DebugInfo
			<< "getuid() return " << ret << endl
			<< LOCATION
			<< EndDebugInfo;
	SetSystemCallStatus(ret,false);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void 
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_access() 
{
	ADDRESS_TYPE addr;
	int pathnamelen;
	char *pathname;
	mode_t mode;
	int ret;
    
	addr = GetSystemCallParam(0);
	pathnamelen = StringLength(addr);
	pathname = (char *) malloc(pathnamelen + 1);
	ReadMem(addr, pathname, pathnamelen + 1);
	mode = GetSystemCallParam(1);
#if defined(WIN32) || defined(WIN64)
	int win_mode = 0;
	win_mode = mode & S_IRWXU; // Windows doesn't have bits for group and others
	ret = access(pathname, win_mode);
#else
	ret = access(pathname, mode);
#endif
	if(unlikely(verbose))
		logger << DebugInfo
			<< "access(pathname=\"" << pathname 
			<< "\", mode=0x" << hex << mode << dec << ") return " << ret << endl
			<< LOCATION
			<< EndDebugInfo;
	free(pathname);
	SetSystemCallStatus(ret, ret < 0);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE> 
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_times() 
{
	int ret;
	ADDRESS_TYPE buf_addr;
	buf_addr = GetSystemCallParam(0);

	if ( (system == "arm") || (system == "arm-eabi") )
	{
		struct arm_tms_t target_tms;
		ret = Times(&target_tms);

		if (ret >= 0)
		{
			WriteMem(buf_addr, &target_tms, sizeof(target_tms));
		}
	}
	else if (system == "powerpc")
	{
		struct powerpc_tms_t target_tms;
		ret = Times(&target_tms);

		if (ret >= 0)
		{
			WriteMem(buf_addr, &target_tms, sizeof(target_tms));
		}
	}
	else ret = -1;

	if(unlikely(verbose))
		logger << DebugInfo
			<< "times(buf=0x" << hex << buf_addr << dec << ") return " << ret 
			<< endl 
			<< LOCATION
			<< EndDebugInfo;
	SetSystemCallStatus(ret, ret != -1);
}
	
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_brk() 
{
	ADDRESS_TYPE new_brk_point;
    
	new_brk_point = GetSystemCallParam(0);
   
	if (new_brk_point > GetBrkPoint())
		SetBrkPoint(new_brk_point);      
		
	if(unlikely(verbose))
		logger << DebugInfo
			<< "brk(new_brk_point=0x" << hex << new_brk_point 
			<< ") return 0x" << GetBrkPoint() << dec << endl
			<< LOCATION 
			<< EndDebugInfo;
	SetSystemCallStatus(GetBrkPoint(),false);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE> 
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_getgid() 
{
#ifdef WIN32
	uint32_t ret = 0;
#else
	gid_t ret;
    
	ret = getgid();
#endif
	if(unlikely(verbose))
		logger << DebugInfo
			<< "getgid() return " << ret << endl
			<< LOCATION
			<< EndDebugInfo;
	SetSystemCallStatus(ret,false);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_geteuid() 
{
#ifdef WIN32
	uint32_t ret = 0;
#else
	uid_t ret;
    
	ret = geteuid();
#endif
	if(unlikely(verbose))
		logger << DebugInfo
			<< "geteuid() return " << ret << endl 
			<< LOCATION
			<< EndDebugInfo;
	SetSystemCallStatus(ret,false);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE> 
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_getegid() 
{
#ifdef WIN32
	uint32_t ret = 0;
#else
	gid_t ret;
    
	ret = getegid();
#endif
	if(unlikely(verbose))
		logger << DebugInfo
			<< "getegid() return " << ret << endl
			<< LOCATION
			<< EndDebugInfo;
	SetSystemCallStatus(ret,false);
}


template<class ADDRESS_TYPE, class PARAMETER_TYPE> 
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_munmap() 
{
	size_t u = (size_t)(GetSystemCallParam(1));
   
	if (GetMmapBrkPoint() - u > GetMmapBrkPoint()) 
	{
		SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL),true);
		if(unlikely(verbose))
			logger << DebugInfo
				<< "size = " << u 
				<< ", ret = 0x" << hex << ((PARAMETER_TYPE)(-EINVAL)) << dec
				<< endl
				<< LOCATION << EndDebugInfo;
		return;
	}
    
	if (GetMmapBrkPoint() - u < GetMmapBase())
		u = (size_t)(GetMmapBrkPoint() - GetMmapBase());
   
	if (GetMmapBrkPoint() - u >= GetMmapBrkPoint()) 
	{
		SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL),true);
		if(unlikely(verbose))
			logger << DebugInfo
				<< "size = " << u 
				<< ", ret = 0x" << hex << ((PARAMETER_TYPE)(-EINVAL)) << dec 
				<< endl
				<< LOCATION << EndDebugInfo;
		return;
	}

	SetSystemCallStatus((PARAMETER_TYPE)0,false);
	if(unlikely(verbose))
		logger << DebugInfo
			<< "size = " << u 
			<< ", ret = 0x" << hex << 0 << dec 
			<< endl 
			<< LOCATION << EndDebugInfo;
	SetMmapBrkPoint(GetMmapBrkPoint() - u);
}
	
	
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_stat() 
{
	logger << DebugWarning
			<< "TODO" << endl
			<< LOCATION
			<< EndDebugWarning;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
int LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
Stat(int fd, struct powerpc_stat_t *target_stat)
{
	int ret;
	struct stat host_stat;
	ret = fstat(fd, &host_stat);
	if (ret < 0) return ret;

#if defined(__x86_64) || defined(__amd64) || defined(__LP64__) || defined(_LP64) || defined(__amd64__)
	// 64-bit host
	target_stat->st_dev = Host2Target(endianness_, (uint64_t) host_stat.st_dev);
	target_stat->st_ino = Host2Target(endianness_, (uint64_t) host_stat.st_ino);
	target_stat->st_mode = Host2Target(endianness_, (uint32_t) host_stat.st_mode);
	target_stat->st_nlink = Host2Target(endianness_, (uint64_t) host_stat.st_nlink);
	target_stat->st_uid = Host2Target(endianness_, (uint32_t) host_stat.st_uid);
	target_stat->st_gid = Host2Target(endianness_, (uint32_t) host_stat.st_gid);
	target_stat->st_rdev = Host2Target(endianness_, (int64_t) host_stat.st_rdev);
	target_stat->st_size = Host2Target(endianness_, (int64_t) host_stat.st_size);
#if defined(WIN64) // Windows x64
	target_stat->st_blksize = Host2Target((int32_t) 512);
	target_stat->st_blocks = Host2Target((int64_t)((host_stat.st_size + 511) / 512));
	target_stat->st_atim.tv_sec = Host2Target(endianness_, (int64_t) host_stat.st_atim);
	target_stat->st_atim.tv_nsec = 0;
	target_stat->st_mtim.tv_sec = Host2Target(endianness_, (int64_t) host_stat.st_mtim);
	target_stat->st_mtim.tv_nsec = 0;
	target_stat->st_ctim.tv_sec = Host2Target(endianness_, (int64_t) host_stat.st_ctim);
	target_stat->st_ctim.tv_nsec = 0;
#elif defined(linux) // Linux x64
	target_stat->st_blksize = Host2Target(endianness_, (int64_t) host_stat.st_blksize);
	target_stat->st_blocks = Host2Target(endianness_, (int64_t) host_stat.st_blocks);
	target_stat->st_atim.tv_sec = Host2Target(endianness_, (int64_t) host_stat.st_atim.tv_sec);
	target_stat->st_atim.tv_nsec = Host2Target(endianness_, (int64_t) host_stat.st_atim.tv_nsec);
	target_stat->st_mtim.tv_sec = Host2Target(endianness_, (int64_t) host_stat.st_mtim.tv_sec);
	target_stat->st_mtim.tv_nsec = Host2Target(endianness_, (int64_t) host_stat.st_mtim.tv_nsec);
	target_stat->st_ctim.tv_sec = Host2Target(endianness_, (int64_t) host_stat.st_ctim.tv_sec);
	target_stat->st_ctim.tv_nsec = Host2Target(endianness_, (int64_t) host_stat.st_ctim.tv_nsec);
#elif defined(__APPLE_CC__) // darwin PPC64/x86_64
	target_stat->st_blksize = Host2Target(endianness_, (int64_t) host_stat.st_blksize);
	target_stat->st_blocks = Host2Target(endianness_, (int64_t) host_stat.st_blocks);
	target_stat->st_atim.tv_sec = Host2Target(endianness_, (int64_t) host_stat.st_atimespec.tv_sec);
	target_stat->st_atim.tv_nsec = Host2Target(endianness_, (int64_t) host_stat.st_atimespec.tv_nsec);
	target_stat->st_mtim.tv_sec = Host2Target(endianness_, (int64_t) host_stat.st_mtimespec.tv_sec);
	target_stat->st_mtim.tv_nsec = Host2Target(endianness_, (int64_t) host_stat.st_mtimespec.tv_nsec);
	target_stat->st_ctim.tv_sec = Host2Target(endianness_, (int64_t) host_stat.st_ctimespec.tv_sec);
	target_stat->st_ctim.tv_nsec = Host2Target(endianness_, (int64_t) host_stat.st_ctimespec.tv_nsec);
#endif

#else
	// 32-bit host
	target_stat->st_dev = Host2Target(endianness_, (uint64_t) host_stat.st_dev);
	target_stat->st_ino = Host2Target(endianness_, (uint32_t) host_stat.st_ino);
	target_stat->st_mode = Host2Target(endianness_, (uint32_t) host_stat.st_mode);
	target_stat->st_nlink = Host2Target(endianness_, (uint32_t) host_stat.st_nlink);
	target_stat->st_uid = Host2Target(endianness_, (uint32_t) host_stat.st_uid);
	target_stat->st_gid = Host2Target(endianness_, (uint32_t) host_stat.st_gid);
	target_stat->st_rdev = Host2Target(endianness_, (int64_t) host_stat.st_rdev);
	target_stat->st_size = Host2Target(endianness_, (int64_t) host_stat.st_size);
#if defined(WIN32) // Windows 32
	target_stat->st_blksize = Host2Target(endianness_, (int32_t) 512);
	target_stat->st_blocks = Host2Target(endianness_, (int64_t)((host_stat.st_size + 511) / 512));
	target_stat->st_atim.tv_sec = Host2Target(endianness_, (int32_t) host_stat.st_atime);
	target_stat->st_atim.tv_nsec = 0;
	target_stat->st_mtim.tv_sec = Host2Target(endianness_, (int32_t) host_stat.st_mtime);
	target_stat->st_mtim.tv_nsec = 0;
	target_stat->st_ctim.tv_sec = Host2Target(endianness_, (int32_t) host_stat.st_ctime);
	target_stat->st_ctim.tv_nsec = 0;
#elif defined(linux) // Linux 32
	target_stat->st_blksize = Host2Target(endianness_, (int32_t) host_stat.st_blksize);
	target_stat->st_blocks = Host2Target(endianness_, (int64_t) host_stat.st_blocks);
	target_stat->st_atim.tv_sec = Host2Target(endianness_, (int32_t) host_stat.st_atim.tv_sec);
	target_stat->st_atim.tv_nsec = Host2Target(endianness_, (int32_t) host_stat.st_atim.tv_nsec);
	target_stat->st_mtim.tv_sec = Host2Target(endianness_, (int32_t) host_stat.st_mtim.tv_sec);
	target_stat->st_mtim.tv_nsec = Host2Target(endianness_, (int32_t) host_stat.st_mtim.tv_nsec);
	target_stat->st_ctim.tv_sec = Host2Target(endianness_, (int32_t) host_stat.st_ctim.tv_sec);
	target_stat->st_ctim.tv_nsec = Host2Target(endianness_, (int32_t) host_stat.st_ctim.tv_nsec);
#elif defined(__APPLE_CC__) // Darwin PPC32/x86
	target_stat->st_blksize = Host2Target(endianness_, (int32_t) host_stat.st_blksize);
	target_stat->st_blocks = Host2Target(endianness_, (int64_t) host_stat.st_blocks);
	target_stat->st_atim.tv_sec = Host2Target(endianness_, (int32_t) host_stat.st_atimespec.tv_sec);
	target_stat->st_atim.tv_nsec = Host2Target(endianness_, (int32_t) host_stat.st_atimespec.tv_nsec);
	target_stat->st_mtim.tv_sec = Host2Target(endianness_, (int32_t) host_stat.st_mtimespec.tv_sec);
	target_stat->st_mtim.tv_nsec = Host2Target(endianness_, (int32_t) host_stat.st_mtimespec.tv_nsec);
	target_stat->st_ctim.tv_sec = Host2Target(endianness_, (int32_t) host_stat.st_ctimespec.tv_sec);
	target_stat->st_ctim.tv_nsec = Host2Target(endianness_, (int32_t) host_stat.st_ctimespec.tv_nsec);
#endif

#endif
	target_stat->__pad1 = 0;
	target_stat->__pad2 = 0;
	target_stat->__unused4 = 0;
	target_stat->__unused5 = 0;
	return ret;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
int LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
Stat64(int fd, struct powerpc_stat64_t *target_stat)
{
	int ret;
#if defined(WIN32) || defined(WIN64)
	struct _stati64 host_stat;
	ret = _fstati64(fd, &host_stat);
#elif defined(linux)
	struct stat64 host_stat;
	ret = fstat64(fd, &host_stat);
#elif defined(__APPLE_CC__)
	struct stat host_stat;
	ret = fstat(fd, &host_stat);
#endif

	if (ret < 0) return ret;

#if defined(__x86_64) || defined(__amd64) || defined(__x86_64__) || defined(__amd64__) || defined(__LP64__) || defined(_LP64)
	// 64-bit host
	target_stat->st_dev = Host2Target(endianness_, (uint64_t) host_stat.st_dev);
	target_stat->st_ino = Host2Target(endianness_, (uint64_t) host_stat.st_ino);
	target_stat->st_mode = Host2Target(endianness_, (uint32_t) host_stat.st_mode);
	target_stat->st_nlink = Host2Target(endianness_, (uint64_t) host_stat.st_nlink);
	target_stat->st_uid = Host2Target(endianness_, (uint32_t) host_stat.st_uid);
	target_stat->st_gid = Host2Target(endianness_, (uint32_t) host_stat.st_gid);
	target_stat->st_rdev = Host2Target(endianness_, (int64_t) host_stat.st_rdev);
	target_stat->st_size = Host2Target(endianness_, (int64_t) host_stat.st_size);
#if defined(WIN64) // Windows x64
	target_stat->st_blksize = Host2Target(endianness_, (int32_t) 512);
	target_stat->st_blocks = Host2Target(endianness_, (int64_t)((host_stat.st_size + 511) / 512));
	target_stat->st_atim.tv_sec = Host2Target(endianness_, (int64_t) host_stat.st_atim);
	target_stat->st_atim.tv_nsec = 0;
	target_stat->st_mtim.tv_sec = Host2Target(endianness_, (int64_t) host_stat.st_mtim);
	target_stat->st_mtim.tv_nsec = 0;
	target_stat->st_ctim.tv_sec = Host2Target(endianness_, (int64_t) host_stat.st_ctim);
	target_stat->st_ctim.tv_nsec = 0;
#elif defined(linux) // Linux x64
	target_stat->st_blksize = Host2Target(endianness_, (int32_t) host_stat.st_blksize);
	target_stat->st_blocks = Host2Target(endianness_, (int64_t) host_stat.st_blocks);
	target_stat->st_atim.tv_sec = Host2Target(endianness_, (int64_t) host_stat.st_atim.tv_sec);
	target_stat->st_atim.tv_nsec = Host2Target(endianness_, (int64_t) host_stat.st_atim.tv_nsec);
	target_stat->st_mtim.tv_sec = Host2Target(endianness_, (int64_t) host_stat.st_mtim.tv_sec);
	target_stat->st_mtim.tv_nsec = Host2Target(endianness_, (int64_t) host_stat.st_mtim.tv_nsec);
	target_stat->st_ctim.tv_sec = Host2Target(endianness_, (int64_t) host_stat.st_ctim.tv_sec);
	target_stat->st_ctim.tv_nsec = Host2Target(endianness_, (int64_t) host_stat.st_ctim.tv_nsec);
#elif defined(__APPLE_CC__) // Darwin PPC64/x86_64
	target_stat->st_blksize = Host2Target(endianness_, (int32_t) host_stat.st_blksize);
	target_stat->st_blocks = Host2Target(endianness_, (int64_t) host_stat.st_blocks);
	target_stat->st_atim.tv_sec = Host2Target(endianness_, (int64_t) host_stat.st_atimespec.tv_sec);
	target_stat->st_atim.tv_nsec = Host2Target(endianness_, (int64_t) host_stat.st_atimespec.tv_nsec);
	target_stat->st_mtim.tv_sec = Host2Target(endianness_, (int64_t) host_stat.st_mtimespec.tv_sec);
	target_stat->st_mtim.tv_nsec = Host2Target(endianness_, (int64_t) host_stat.st_mtimespec.tv_nsec);
	target_stat->st_ctim.tv_sec = Host2Target(endianness_, (int64_t) host_stat.st_ctimespec.tv_sec);
	target_stat->st_ctim.tv_nsec = Host2Target(endianness_, (int64_t) host_stat.st_ctimespec.tv_nsec);
#endif

#else
	// 32-bit host
	target_stat->st_dev = Host2Target(endianness_, (uint64_t) host_stat.st_dev);
	target_stat->st_ino = Host2Target(endianness_, (uint32_t) host_stat.st_ino);
	target_stat->st_mode = Host2Target(endianness_, (uint32_t) host_stat.st_mode);
	target_stat->st_nlink = Host2Target(endianness_, (uint32_t) host_stat.st_nlink);
	target_stat->st_uid = Host2Target(endianness_, (uint32_t) host_stat.st_uid);
	target_stat->st_gid = Host2Target(endianness_, (uint32_t) host_stat.st_gid);
	target_stat->st_rdev = Host2Target(endianness_, (int64_t) host_stat.st_rdev);
	target_stat->st_size = Host2Target(endianness_, (int64_t) host_stat.st_size);
#if defined(WIN32) // Windows 32
	target_stat->st_blksize = Host2Target(endianness_, (int32_t) 512);
	target_stat->st_blocks = Host2Target(endianness_, (int64_t)((host_stat.st_size + 511) / 512));
	target_stat->st_atim.tv_sec = Host2Target(endianness_, (int32_t) host_stat.st_atime);
	target_stat->st_atim.tv_nsec = 0;
	target_stat->st_mtim.tv_sec = Host2Target(endianness_, (int32_t) host_stat.st_mtime);
	target_stat->st_mtim.tv_nsec = 0;
	target_stat->st_ctim.tv_sec = Host2Target(endianness_, (int32_t) host_stat.st_ctime);
	target_stat->st_ctim.tv_nsec = 0;
#elif defined(linux) // Linux 32
	target_stat->st_blksize = Host2Target(endianness_, (int32_t) host_stat.st_blksize);
	target_stat->st_blocks = Host2Target(endianness_, (int64_t) host_stat.st_blocks);
	target_stat->st_atim.tv_sec = Host2Target(endianness_, (int32_t) host_stat.st_atim.tv_sec);
	target_stat->st_atim.tv_nsec = Host2Target(endianness_, (int32_t) host_stat.st_atim.tv_nsec);
	target_stat->st_mtim.tv_sec = Host2Target(endianness_, (int32_t) host_stat.st_mtim.tv_sec);
	target_stat->st_mtim.tv_nsec = Host2Target(endianness_, (int32_t) host_stat.st_mtim.tv_nsec);
	target_stat->st_ctim.tv_sec = Host2Target(endianness_, (int32_t) host_stat.st_ctim.tv_sec);
	target_stat->st_ctim.tv_nsec = Host2Target(endianness_, (int32_t) host_stat.st_ctim.tv_nsec);
#elif defined(__APPLE_CC__) // Darwin PPC32/x86
	target_stat->st_blksize = Host2Target(endianness_, (int32_t) host_stat.st_blksize);
	target_stat->st_blocks = Host2Target(endianness_, (int64_t) host_stat.st_blocks);
	target_stat->st_atim.tv_sec = Host2Target(endianness_, (int32_t) host_stat.st_atimespec.tv_sec);
	target_stat->st_atim.tv_nsec = Host2Target(endianness_, (int32_t) host_stat.st_atimespec.tv_nsec);
	target_stat->st_mtim.tv_sec = Host2Target(endianness_, (int32_t) host_stat.st_mtimespec.tv_sec);
	target_stat->st_mtim.tv_nsec = Host2Target(endianness_, (int32_t) host_stat.st_mtimespec.tv_nsec);
	target_stat->st_ctim.tv_sec = Host2Target(endianness_, (int32_t) host_stat.st_ctimespec.tv_sec);
	target_stat->st_ctim.tv_nsec = Host2Target(endianness_, (int32_t) host_stat.st_ctimespec.tv_nsec);
#endif

#endif
	target_stat->__pad2 = 0;
    target_stat->__unused4 = 0;
    target_stat->__unused5 = 0;
	return ret;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
int LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
Stat64(int fd, arm_stat64_t *target_stat)
{
	int ret;
#if defined(WIN32) || defined(WIN64)
	struct _stati64 host_stat;
	ret = _fstati64(fd, &host_stat);
#elif defined(linux)
	struct stat64 host_stat;
	ret = fstat64(fd, &host_stat);
#elif defined(__APPLE_CC__)
	struct stat host_stat;
	ret = fstat(fd, &host_stat);
#endif
	if(ret < 0) return ret;

#if defined(__x86_64) || defined(__amd64) || defined(__x86_64__) || defined(__amd64__) || defined(__LP64__) || defined(_LP64)
	// 64-bit host
	target_stat->st_dev = Host2Target(endianness_, (uint64_t) host_stat.st_dev);
	target_stat->__st_ino = Host2Target(endianness_, (uint32_t) host_stat.st_ino);
	target_stat->st_ino = Host2Target(endianness_, (uint64_t) host_stat.st_ino);
	target_stat->st_mode = Host2Target(endianness_, (uint32_t) host_stat.st_mode);
	target_stat->st_nlink = Host2Target(endianness_, (uint64_t) host_stat.st_nlink);
	target_stat->st_uid = Host2Target(endianness_, (uint32_t) host_stat.st_uid);
	target_stat->st_gid = Host2Target(endianness_, (uint32_t) host_stat.st_gid);
	target_stat->st_rdev = Host2Target(endianness_, (int64_t) host_stat.st_rdev);
	target_stat->st_size = Host2Target(endianness_, (int64_t) host_stat.st_size);
#if defined(WIN64) // Windows x64
	target_stat->st_blksize = Host2Target(endianness_, (int32_t) 512);
	target_stat->st_blocks = Host2Target(endianness_, (int64_t)((host_stat.st_size + 511) / 512));
	target_stat->st_atim.tv_sec = Host2Target(endianness_, (int64_t) host_stat.st_atim);
	target_stat->st_atim.tv_nsec = 0;
	target_stat->st_mtim.tv_sec = Host2Target(endianness_, (int64_t) host_stat.st_mtim);
	target_stat->st_mtim.tv_nsec = 0;
	target_stat->st_ctim.tv_sec = Host2Target(endianness_, (int64_t) host_stat.st_ctim);
	target_stat->st_ctim.tv_nsec = 0;
#elif defined(linux) // Linux x64
	target_stat->st_blksize = Host2Target(endianness_, (int32_t) host_stat.st_blksize);
	target_stat->st_blocks = Host2Target(endianness_, (int64_t) host_stat.st_blocks);
	target_stat->st_atim.tv_sec = Host2Target(endianness_, (int64_t) host_stat.st_atim.tv_sec);
	target_stat->st_atim.tv_nsec = Host2Target(endianness_, (int64_t) host_stat.st_atim.tv_nsec);
	target_stat->st_mtim.tv_sec = Host2Target(endianness_, (int64_t) host_stat.st_mtim.tv_sec);
	target_stat->st_mtim.tv_nsec = Host2Target(endianness_, (int64_t) host_stat.st_mtim.tv_nsec);
	target_stat->st_ctim.tv_sec = Host2Target(endianness_, (int64_t) host_stat.st_ctim.tv_sec);
	target_stat->st_ctim.tv_nsec = Host2Target(endianness_, (int64_t) host_stat.st_ctim.tv_nsec);
#elif defined(__APPLE_CC__) // darwin PPC64/x86_64
	target_stat->st_blksize = Host2Target(endianness_, (int32_t) host_stat.st_blksize);
	target_stat->st_blocks = Host2Target(endianness_, (int64_t) host_stat.st_blocks);
	target_stat->st_atim.tv_sec = Host2Target(endianness_, (int64_t) host_stat.st_atimespec.tv_sec);
	target_stat->st_atim.tv_nsec = Host2Target(endianness_, (int64_t) host_stat.st_atimespec.tv_nsec);
	target_stat->st_mtim.tv_sec = Host2Target(endianness_, (int64_t) host_stat.st_mtimespec.tv_sec);
	target_stat->st_mtim.tv_nsec = Host2Target(endianness_, (int64_t) host_stat.st_mtimespec.tv_nsec);
	target_stat->st_ctim.tv_sec = Host2Target(endianness_, (int64_t) host_stat.st_ctimespec.tv_sec);
	target_stat->st_ctim.tv_nsec = Host2Target(endianness_, (int64_t) host_stat.st_ctimespec.tv_nsec);
#endif

#else
	// 32-bit host
	target_stat->st_dev = Host2Target(endianness_, (uint64_t) host_stat.st_dev);
	target_stat->__st_ino = Host2Target(endianness_, (uint32_t) host_stat.st_ino);
	target_stat->st_ino = Host2Target(endianness_, (uint32_t) host_stat.st_ino);
	target_stat->st_mode = Host2Target(endianness_, (uint32_t) host_stat.st_mode);
	target_stat->st_nlink = Host2Target(endianness_, (uint32_t) host_stat.st_nlink);
	target_stat->st_uid = Host2Target(endianness_, (uint32_t) host_stat.st_uid);
	target_stat->st_gid = Host2Target(endianness_, (uint32_t) host_stat.st_gid);
	target_stat->st_rdev = Host2Target(endianness_, (int64_t) host_stat.st_rdev);
	target_stat->st_size = Host2Target(endianness_, (int64_t) host_stat.st_size);
#if defined(WIN32) // Windows 32
	target_stat->st_blksize = Host2Target(endianness_, (int32_t) 512);
	target_stat->st_blocks = Host2Target(endianness_, (int64_t)((host_stat.st_size + 511) / 512));
	target_stat->st_atim.tv_sec = Host2Target(endianness_, (int32_t) host_stat.st_atime);
	target_stat->st_atim.tv_nsec = 0;
	target_stat->st_mtim.tv_sec = Host2Target(endianness_, (int32_t) host_stat.st_mtime);
	target_stat->st_mtim.tv_nsec = 0;
	target_stat->st_ctim.tv_sec = Host2Target(endianness_, (int32_t) host_stat.st_ctime);
	target_stat->st_ctim.tv_nsec = 0;
#elif defined(linux) // Linux 32
	target_stat->st_blksize = Host2Target(endianness_, (int32_t) host_stat.st_blksize);
	target_stat->st_blocks = Host2Target(endianness_, (int64_t) host_stat.st_blocks);
	target_stat->st_atim.tv_sec = Host2Target(endianness_, (int32_t) host_stat.st_atim.tv_sec);
	target_stat->st_atim.tv_nsec = Host2Target(endianness_, (int32_t) host_stat.st_atim.tv_nsec);
	target_stat->st_mtim.tv_sec = Host2Target(endianness_, (int32_t) host_stat.st_mtim.tv_sec);
	target_stat->st_mtim.tv_nsec = Host2Target(endianness_, (int32_t) host_stat.st_mtim.tv_nsec);
	target_stat->st_ctim.tv_sec = Host2Target(endianness_, (int32_t) host_stat.st_ctim.tv_sec);
	target_stat->st_ctim.tv_nsec = Host2Target(endianness_, (int32_t) host_stat.st_ctim.tv_nsec);
#elif defined(__APPLE_CC__) // Darwin PPC32/x86
	target_stat->st_blksize = Host2Target(endianness_, (int32_t) host_stat.st_blksize);
	target_stat->st_blocks = Host2Target(endianness_, (int64_t) host_stat.st_blocks);
	target_stat->st_atim.tv_sec = Host2Target(endianness_, (int32_t) host_stat.st_atimespec.tv_sec);
	target_stat->st_atim.tv_nsec = Host2Target(endianness_, (int32_t) host_stat.st_atimespec.tv_nsec);
	target_stat->st_mtim.tv_sec = Host2Target(endianness_, (int32_t) host_stat.st_mtimespec.tv_sec);
	target_stat->st_mtim.tv_nsec = Host2Target(endianness_, (int32_t) host_stat.st_mtimespec.tv_nsec);
	target_stat->st_ctim.tv_sec = Host2Target(endianness_, (int32_t) host_stat.st_ctimespec.tv_sec);
	target_stat->st_ctim.tv_nsec = Host2Target(endianness_, (int32_t) host_stat.st_ctimespec.tv_nsec);
#endif

#endif
	target_stat->__pad1 = 0;
	target_stat->__pad2 = 0;
	return ret;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
int LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
Times(struct powerpc_tms_t *target_tms)
{
	int ret;
#ifdef WIN32
	FILETIME ftCreationTime;
	FILETIME ftExitTime;
	FILETIME ftKernelTime;
	FILETIME ftUserTime;

	if (GetProcessTimes(GetCurrentProcess(), &ftCreationTime, &ftExitTime, &ftKernelTime, &ftUserTime)) return -1;

	target_tms->tms_utime = Host2Target(endianness_, (uint32_t) ftUserTime.dwLowDateTime);
	target_tms->tms_stime = Host2Target(endianness_, (uint32_t) ftKernelTime.dwLowDateTime);
	target_tms->tms_cutime = 0;   // User CPU time of dead children
	target_tms->tms_cstime = 0;   // System CPU time of dead children
#else
	struct tms host_tms;

	ret = (int) times(&host_tms);
	target_tms->tms_utime = Host2Target(endianness_, (int32_t) host_tms.tms_utime);
	target_tms->tms_stime = Host2Target(endianness_, (int32_t) host_tms.tms_stime);
	target_tms->tms_cutime = Host2Target(endianness_, (int32_t) host_tms.tms_cutime);
	target_tms->tms_cstime = Host2Target(endianness_, (int32_t) host_tms.tms_cstime);
#endif
	return ret;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
int LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
Times(struct arm_tms_t *target_tms)
{
	int ret;
#ifdef WIN32
	FILETIME ftCreationTime;
	FILETIME ftExitTime;
	FILETIME ftKernelTime;
	FILETIME ftUserTime;

	if (GetProcessTimes(GetCurrentProcess(), &ftCreationTime, &ftExitTime, &ftKernelTime, &ftUserTime)) return -1;

	target_tms->tms_utime = Host2Target(endianness_, (uint32_t) ftUserTime.dwLowDateTime);
	target_tms->tms_stime = Host2Target(endianness_, (uint32_t) ftKernelTime.dwLowDateTime);
	target_tms->tms_cutime = 0;   // User CPU time of dead children
	target_tms->tms_cstime = 0;   // System CPU time of dead children
#else
	struct tms host_tms;

	ret = (int) times(&host_tms);
	target_tms->tms_utime = Host2Target(endianness_, (int32_t) host_tms.tms_utime);
	target_tms->tms_stime = Host2Target(endianness_, (int32_t) host_tms.tms_stime);
	target_tms->tms_cutime = Host2Target(endianness_, (int32_t) host_tms.tms_cutime);
	target_tms->tms_cstime = Host2Target(endianness_, (int32_t) host_tms.tms_cstime);
#endif
	return ret;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_fstat() 
{
	int ret;
	int fd;
	ADDRESS_TYPE buf_address;

	fd = GetSystemCallParam(0);
	buf_address = GetSystemCallParam(1);
	if ( (system == "arm") || (system == "arm-eabi") )
	{
		ret = -1;
	}
	else if (system == "powerpc")
	{
		struct powerpc_stat_t target_stat;
		ret = Stat(fd, &target_stat);
		WriteMem(buf_address, &target_stat, sizeof(target_stat));
	}
	else ret = -1;

	if(unlikely(verbose))
		logger << DebugInfo
			<< "stat(fd=" << fd 
			<< ", buf_addr=0x" << hex << buf_address << dec 
			<< ") return " << ret << endl
			<< LOCATION 
			<< EndDebugInfo;
	SetSystemCallStatus((PARAMETER_TYPE)ret,ret < 0);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_uname() 
{
	int ret;
 	ADDRESS_TYPE buf_addr = GetSystemCallParam(0);
 	struct utsname_t value;
 	memset(&value, 0, sizeof(value));
 	memcpy(&(value.sysname),
 			utsname_sysname.c_str(), utsname_sysname.length() + 1);
 	memcpy(&(value.nodename),
 			utsname_nodename.c_str(), utsname_nodename.length() + 1);
 	memcpy(&(value.release),
 			utsname_release.c_str(), utsname_release.length() + 1);
 	memcpy(&(value.version),
 			utsname_version.c_str(), utsname_version.length() + 1);
 	memcpy(&(value.machine),
 			utsname_machine.c_str(), utsname_machine.length() + 1);
 	WriteMem(buf_addr, &value, sizeof(value));
 	ret = 0;
	SetSystemCallStatus((PARAMETER_TYPE) ret, ret < 0);
}
	
	
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_llseek() 
{
	int fd;
	uint32_t offset_high;
	uint32_t offset_low;
	PARAMETER_TYPE result_addr;
	uint64_t lseek_ret64;
	off_t lseek_ret;
	int whence;
		
	fd = GetSystemCallParam(0);
	offset_high = GetSystemCallParam(1);
	offset_low = GetSystemCallParam(2);
	result_addr = GetSystemCallParam(3);
	whence = GetSystemCallParam(4);
	if(unlikely(verbose))
		logger << DebugInfo
			<< "llseek(fd=" << fd 
			<< ", offset=" << (((int64_t) offset_high << 32) | (int64_t) offset_low)
			<< ", result_addr=0x" << hex << result_addr << dec 
			<< ", whence=" << whence << ")" << endl 
			<< LOCATION << EndDebugInfo;
	if (offset_high == 0) 
	{
		lseek_ret = lseek(fd, offset_low, whence);
		if (lseek_ret >= 0) 
		{
			lseek_ret64 = lseek_ret;
			if (Swap())
				PerformSwap(&lseek_ret64, sizeof(lseek_ret64));
			WriteMem(result_addr, &lseek_ret64, sizeof(lseek_ret64));
			SetSystemCallStatus((PARAMETER_TYPE)lseek_ret, false);
		} 
		else 
			SetSystemCallStatus((PARAMETER_TYPE)errno, true);
	} 
	else 
		SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL), true);
}


template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_writev() 
{
	if(unlikely(verbose)) 
		logger << DebugInfo
			<< "ret = 0x" 
			<< hex << ((PARAMETER_TYPE)(-EINVAL)) << dec << endl
			<< LOCATION << EndDebugInfo;

	SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL),true);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_mmap() 
{
	SetSystemCallStatus(-1,true); return;
	if (GetSystemCallParam(3) == 0x32) 
	{ /* MAP_PRIVATE | MAP_ANONYMOUS */
		SetSystemCallStatus(GetSystemCallParam(0),false);
		if(unlikely(verbose)) 
			logger << DebugInfo
				<< "map_type = 0x" << hex << GetSystemCallParam(3) << dec 
				<< ", size = " << GetSystemCallParam(1)
				<< ", ret = 0x" << hex << ((PARAMETER_TYPE)GetSystemCallParam(0)) << dec 
				<< endl << LOCATION << EndDebugInfo;
		return;
	}
		
	if ((GetSystemCallParam(3)&0xFF) != 0x22) 
	{ /* MAP_PRIVATE | MAP_ANONYMOUS */
		SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL),true);
		if(unlikely(verbose)) 
			logger << DebugInfo
				<< "map_type = 0x" << hex << GetSystemCallParam(3) << dec 
				<< ", size = " << GetSystemCallParam(1)
				<< ", ret = 0x" << hex << ((PARAMETER_TYPE)(-EINVAL)) << dec 
				<< endl << LOCATION << EndDebugInfo;
		return;
	}
	SetSystemCallStatus(GetMmapBrkPoint(),false);
	if(unlikely(verbose)) 
		logger << DebugInfo
			<< "map_type = 0x" << hex << GetSystemCallParam(3) << dec 
			<< ", size = " << GetSystemCallParam(1)
			<< ", ret = 0x" << hex << GetMmapBrkPoint() << dec 
			<< endl << LOCATION << EndDebugInfo;
	SetMmapBrkPoint(GetMmapBrkPoint() + GetSystemCallParam(1));
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_mmap2() 
{
	if (GetSystemCallParam(3) != 0x22) 
	{ /* MAP_PRIVATE | MAP_ANONYMOUS */
		SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL),true);
		if(unlikely(verbose))    
			logger << DebugInfo 
				<< "map_type = 0x" << hex << GetSystemCallParam(3) << dec 
				<< ", size = " << GetSystemCallParam(1)
				<< ", ret = 0x" << hex << ((PARAMETER_TYPE)(-EINVAL)) << dec 
				<< endl << LOCATION << EndDebugInfo;
		return;
	}
    
	if (GetMmapBrkPoint() + GetSystemCallParam(1) > GetSystemCallParam(1)) 
	{
		SetSystemCallStatus(GetMmapBrkPoint(),false);
		if(unlikely(verbose)) 
			logger << DebugInfo
				<< "map_type = 0x" << hex << GetSystemCallParam(3) << dec 
				<< ", size = " << GetSystemCallParam(1)
				<< ", ret = 0x" << hex << GetMmapBrkPoint() << dec 
				<< endl << LOCATION << EndDebugInfo;
		SetMmapBrkPoint(GetMmapBrkPoint() + GetSystemCallParam(1));
	} 
	else 
	{
		if(unlikely(verbose)) 
			logger << DebugInfo
				<< "map_type = 0x" << hex << GetSystemCallParam(3) << dec 
				<< ", size = " << GetSystemCallParam(1)
				<< ", ret = 0x" << hex << ((PARAMETER_TYPE)(-EINVAL)) << dec 
				<< endl << LOCATION << EndDebugInfo;
		SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL),true);
	}
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_stat64() 
{ 
	int ret;
	ADDRESS_TYPE buf_address;
	int fd;

	fd = GetSystemCallParam(0);
	buf_address = GetSystemCallParam(1);
	if ( (system == "arm") || (system == "arm-eabi") )
	{
		struct arm_stat64_t target_stat;
		ret = Stat64(fd, &target_stat);
		WriteMem(buf_address, &target_stat, sizeof(target_stat));
	}
	else if (system == "powerpc")
	{
		struct powerpc_stat64_t target_stat;
		ret = Stat64(fd, &target_stat);
		WriteMem(buf_address, &target_stat, sizeof(target_stat));
	}
	else ret = -1;
	if(unlikely(verbose))     
		logger << DebugInfo
			<< "fd = " << fd << ", buf_address = 0x" << hex << buf_address << dec 
			<< ", ret = 0x" << hex << ret << dec 
			<< endl << LOCATION << EndDebugInfo;
	SetSystemCallStatus((PARAMETER_TYPE)ret,ret < 0);
}
	
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_fstat64() 
{ 
	int ret;
	ADDRESS_TYPE buf_address;
	int fd;

	fd = GetSystemCallParam(0);
	buf_address = GetSystemCallParam(1);
	if ( (system == "arm") || (system == "arm-eabi") )
	{
		struct arm_stat64_t target_stat;
		ret = Stat64(fd, &target_stat);
		WriteMem(buf_address, &target_stat, sizeof(target_stat));
	}
	else if (system == "powerpc")
	{
		struct powerpc_stat64_t target_stat;
		ret = Stat64(fd, &target_stat);
		WriteMem(buf_address, &target_stat, sizeof(target_stat));
	}
	else ret = -1;
	if(unlikely(verbose))     
		logger << DebugInfo
			<< "fd = " << fd << ", buf_address = 0x" << hex << buf_address << dec 
			<< ", ret = 0x" << hex << ret << dec 
			<< endl << LOCATION << EndDebugInfo;
	SetSystemCallStatus((PARAMETER_TYPE)ret,ret < 0);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_getuid32() 
{
#ifdef WIN32
	uint32_t ret = 0;
#else
	uid_t ret;

	ret = getuid();
#endif
	if(unlikely(verbose)) 
		logger << DebugInfo
			<< "ret = 0x" << hex << ((PARAMETER_TYPE)ret) << dec 
			<< endl << LOCATION << EndDebugInfo;
	SetSystemCallStatus((PARAMETER_TYPE)ret,false);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_getgid32() 
{
#ifdef WIN32
	uint32_t ret = 0;
#else
	gid_t ret;
    
	ret = getgid();
#endif
	if(unlikely(verbose)) 
		logger << DebugInfo
			<< "ret = 0x" << hex << ((PARAMETER_TYPE)ret) << dec 
			<< endl << LOCATION << EndDebugInfo;
	SetSystemCallStatus((PARAMETER_TYPE)ret,false);
}
	
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_geteuid32() 
{
#ifdef WIN32
	uint32_t ret = 0;
#else
	uid_t ret;
    
	ret = geteuid();
#endif
	if(unlikely(verbose)) 
		logger << DebugInfo
			<< "ret = 0x" << hex << ((PARAMETER_TYPE)ret) << dec 
			<< endl << LOCATION << EndDebugInfo;
	SetSystemCallStatus((PARAMETER_TYPE)ret,false);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_getegid32() 
{
#ifdef WIN32
	uint32_t ret = 0;
#else
	gid_t ret;
    
	ret = getegid();
#endif
	if(unlikely(verbose)) 
		logger << DebugInfo
			<< "ret = 0x" << hex << ((PARAMETER_TYPE)ret) << dec 
			<< endl << LOCATION << EndDebugInfo;
	SetSystemCallStatus((PARAMETER_TYPE)ret,false);
}
	
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_flistxattr() 
{
	logger << DebugWarning
		<< "TODO" 
		<< endl << LOCATION << EndDebugWarning;
}
	
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_exit_group() 
{ 
	if(unlikely(verbose))  
		logger << DebugInfo
			<< "ret = 0x" << hex << ((PARAMETER_TYPE)(-EINVAL)) << dec 
			<< endl << LOCATION << EndDebugInfo;
	SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL),true);
}
	
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_fcntl() 
{ 
	int64_t ret;
    
#if defined(WIN32) || defined(WIN64)
	ret = -1;
#else
	ret = fcntl(GetSystemCallParam(0),
			GetSystemCallParam(1),
			GetSystemCallParam(2));
#endif
	if(unlikely(verbose)) 
		logger << DebugInfo
			<< "ret = " <<  ((PARAMETER_TYPE)ret)  
			<< endl << LOCATION << EndDebugInfo;
	SetSystemCallStatus((PARAMETER_TYPE)ret,ret < 0);
}
	
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_fcntl64() 
{
	if(unlikely(verbose))
		logger << DebugInfo
			<< "ret = 0x" << hex << ((PARAMETER_TYPE)(-EINVAL)) << dec 
			<< endl << LOCATION << EndDebugInfo;
	SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL),true);
}
	
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_dup() 
{
	int ret;
	int oldfd = GetSystemCallParam(0);
    
	ret = dup(oldfd);
	if(unlikely(verbose)) 
		logger << DebugInfo
			<< "oldfd = " << oldfd << ", new fd = " << ((PARAMETER_TYPE)ret) 
			<< endl << LOCATION << EndDebugInfo;
	SetSystemCallStatus((PARAMETER_TYPE)ret, ret < 0);
}
	
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_ioctl() 
{
	if(unlikely(verbose))  
		logger << DebugInfo
			<< "ret = 0x" << hex << ((PARAMETER_TYPE)(-EINVAL)) << dec 
			<< endl << LOCATION << EndDebugInfo;
	SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL),true);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_ugetrlimit() 
{
	if(unlikely(verbose))
		logger << DebugInfo
			<< "ret = 0x" << hex << ((PARAMETER_TYPE)(-EINVAL)) << dec 
			<< endl << LOCATION << EndDebugInfo;
	SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL),true);
}
	
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_getrlimit() 
{
	if(unlikely(verbose))
		logger << DebugInfo
			<< "ret = 0x" << hex << ((PARAMETER_TYPE)(-EINVAL)) << dec 
			<< endl << LOCATION << EndDebugInfo;
	SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL),true);
}
	
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_setrlimit() 
{
	if(unlikely(verbose))
		logger << DebugInfo
			<< "ret = 0x" << hex << ((PARAMETER_TYPE)(-EINVAL)) << dec 
			<< endl << LOCATION << EndDebugInfo;
	SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL),true);
}
	
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_rt_sigaction() 
{
	if(unlikely(verbose))
		logger << DebugInfo
			<< "ret = 0x" << hex << ((PARAMETER_TYPE)(-EINVAL)) << dec 
			<< endl << LOCATION << EndDebugInfo;
	SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL),true);
}
	
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_getrusage() 
{
	if(unlikely(verbose))
		logger << DebugInfo
			<< "ret = 0x" << hex << ((PARAMETER_TYPE)(-EINVAL)) << dec 
			<< endl << LOCATION << EndDebugInfo;
	SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL),true);
}
	
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_unlink() 
{
	if(unlikely(verbose))
		logger << DebugInfo
			<< "ret = 0x" << hex << ((PARAMETER_TYPE)(-EINVAL)) << dec 
			<< endl << LOCATION << EndDebugInfo;
	SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL),true);
}
	
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_rename() 
{
	if(unlikely(verbose))
		logger << DebugInfo
			<< "ret = 0x" << hex << ((PARAMETER_TYPE)(-EINVAL)) << dec 
			<< endl << LOCATION << EndDebugInfo;
	SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL),true);
}
	
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_time() 
{
	if(unlikely(verbose))
		logger << DebugInfo
			<< "ret = 0x" << hex << ((PARAMETER_TYPE)(-EINVAL)) << dec 
			<< endl << LOCATION << EndDebugInfo;
	SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL), true);
}
	
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_socketcall() 
{
	if(unlikely(verbose))
		logger << DebugInfo
			<< "ret = 0x" << hex << ((PARAMETER_TYPE)(-EINVAL)) << dec 
			<< endl << LOCATION << EndDebugInfo;
	SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL), true);
}
	
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_rt_sigprocmask() 
{
	if(unlikely(verbose))
		logger << DebugInfo
			<< "ret = 0x" << hex << ((PARAMETER_TYPE)(-EINVAL)) << dec 
			<< endl << LOCATION << EndDebugInfo;
	SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL), true);
}
	
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_kill() 
{
	if(unlikely(verbose))
		logger << DebugInfo
			<< "ret = 0x" << hex << ((PARAMETER_TYPE)(-EINVAL)) << dec 
			<< endl << LOCATION << EndDebugInfo;
	SetSystemCallStatus((PARAMETER_TYPE)0, false);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_ftruncate()
{
	int ret;
	
	ret = ftruncate(GetSystemCallParam(0), GetSystemCallParam(1));

	if(unlikely(verbose)) 
		logger << DebugInfo
			<< "ret = 0x" << hex << ((PARAMETER_TYPE)ret) << dec 
			<< endl << LOCATION << EndDebugInfo;

	SetSystemCallStatus((PARAMETER_TYPE)ret,ret < 0);
}
	
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_arm_breakpoint()
{
	if(unlikely(verbose))
		logger << DebugInfo
			<< "ret = 0x" << hex << ((PARAMETER_TYPE)(-EINVAL)) << dec
			<< endl << LOCATION << EndDebugInfo;
	SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL), true);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_arm_cacheflush()
{
	if(unlikely(verbose))
		logger << DebugInfo
			<< "ret = 0x" << hex << ((PARAMETER_TYPE)(-EINVAL)) << dec
			<< endl << LOCATION << EndDebugInfo;
	SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL), true);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_arm_usr26()
{
	if(unlikely(verbose))
		logger << DebugInfo
			<< "ret = 0x" << hex << ((PARAMETER_TYPE)(-EINVAL)) << dec
			<< endl << LOCATION << EndDebugInfo;
	SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL), true);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_arm_usr32()
{
	if(unlikely(verbose))
		logger << DebugInfo
			<< "ret = 0x" << hex << ((PARAMETER_TYPE)(-EINVAL)) << dec
			<< endl << LOCATION << EndDebugInfo;
	SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL), true);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_arm_set_tls()
{
	uint32_t r0 = GetSystemCallParam(0);
	memory_injection_import->InjectWriteMemory(0xffff0ff0UL,
			(void *)&(r0), 4);
	if(unlikely(verbose))
		logger << DebugInfo
			<< "ret = 0x" << hex << ((PARAMETER_TYPE)0) << dec
			<< endl << LOCATION << EndDebugInfo;
	SetSystemCallStatus((PARAMETER_TYPE)0, true);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void 
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
SetSyscallNameMap()
{
	syscall_name_map[string("unknown")] = &thistype::LSC_unknown;
	syscall_name_map[string("exit")] = &thistype::LSC_exit;
	syscall_name_map[string("read")] = &thistype::LSC_read;
	syscall_name_map[string("write")] = &thistype::LSC_write;
	syscall_name_map[string("open")] = &thistype::LSC_open;
	syscall_name_map[string("close")] = &thistype::LSC_close;
	syscall_name_map[string("lseek")] = &thistype::LSC_lseek;
	syscall_name_map[string("getpid")] = &thistype::LSC_getpid;
	syscall_name_map[string("getuid")] = &thistype::LSC_getuid;
	syscall_name_map[string("access")] = &thistype::LSC_access;
	syscall_name_map[string("times")] = &thistype::LSC_times;
	syscall_name_map[string("brk")] = &thistype::LSC_brk;
	syscall_name_map[string("getgid")] = &thistype::LSC_getgid;
	syscall_name_map[string("geteuid")] = &thistype::LSC_geteuid;
	syscall_name_map[string("getegid")] = &thistype::LSC_getegid;
	syscall_name_map[string("munmap")] = &thistype::LSC_munmap;
	syscall_name_map[string("stat")] = &thistype::LSC_stat;
	syscall_name_map[string("fstat")] = &thistype::LSC_fstat;
	syscall_name_map[string("uname")] = &thistype::LSC_uname;
	syscall_name_map[string("llseek")] = &thistype::LSC_llseek;
	syscall_name_map[string("writev")] = &thistype::LSC_writev;
	syscall_name_map[string("mmap")] = &thistype::LSC_mmap;
	syscall_name_map[string("mmap2")] = &thistype::LSC_mmap2;
	syscall_name_map[string("stat64")] = &thistype::LSC_stat64;
	syscall_name_map[string("fstat64")] = &thistype::LSC_fstat64;
	syscall_name_map[string("getuid32")] = &thistype::LSC_getuid32;
	syscall_name_map[string("getgid32")] = &thistype::LSC_getgid32;
	syscall_name_map[string("geteuid32")] = &thistype::LSC_geteuid32;
	syscall_name_map[string("getegid32")] = &thistype::LSC_getegid32;
	syscall_name_map[string("fcntl64")] = &thistype::LSC_fcntl64;
	syscall_name_map[string("flistxattr")] = &thistype::LSC_flistxattr;
	syscall_name_map[string("exit_group")] = &thistype::LSC_exit_group;
	syscall_name_map[string("fcntl")] = &thistype::LSC_fcntl;
	syscall_name_map[string("dup")] = &thistype::LSC_dup;
	syscall_name_map[string("ioctl")] = &thistype::LSC_ioctl;
	syscall_name_map[string("ugetrlimit")] = &thistype::LSC_ugetrlimit;
	syscall_name_map[string("getrlimit")] = &thistype::LSC_getrlimit;
	syscall_name_map[string("setrlimit")] = &thistype::LSC_setrlimit;
	syscall_name_map[string("rt_sigaction")] = &thistype::LSC_rt_sigaction;
	syscall_name_map[string("getrusage")] = &thistype::LSC_getrusage;
	syscall_name_map[string("unlink")] = &thistype::LSC_unlink;
	syscall_name_map[string("rename")] = &thistype::LSC_rename;
	syscall_name_map[string("time")] = &thistype::LSC_time;
	syscall_name_map[string("socketcall")] = &thistype::LSC_socketcall;
	syscall_name_map[string("rt_sigprocmask")] = &thistype::LSC_rt_sigprocmask;
	syscall_name_map[string("kill")] = &thistype::LSC_kill;
	syscall_name_map[string("ftruncate")] = &thistype::LSC_ftruncate;
	// the following are arm private system calls
	if (utsname_machine.compare("armv5") == 0)
	{
		syscall_name_map[string("breakpoint")] = &thistype::LSC_arm_breakpoint;
		syscall_name_map[string("cacheflush")] = &thistype::LSC_arm_cacheflush;
		syscall_name_map[string("usr26")] = &thistype::LSC_arm_usr26;
		syscall_name_map[string("usr32")] = &thistype::LSC_arm_usr32;
		syscall_name_map[string("set_tls")] = &thistype::LSC_arm_set_tls;
	}
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
int LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::GetSyscallNumber(int id) {
  if ( system_.compare("arm") == 0 )
    std::cerr << "getsyscallnumber arm " << id << std::endl;
  else if ( system_.compare("arm-eabi") == 0 )
    std::cerr << "getsyscallnumber arm-eabi " << id << std::endl;
  else if ( system_.compare("powerpc") == 0 )
    std::cerr << "getsyscallnumber powerpc " << id << std::endl;
  if ( system_.compare("arm") == 0 )
    return ARMGetSyscallNumber(id);
  else if ( system_.compare("arm-eabi") == 0 )
    return ARMEABIGetSyscallNumber(id);
  else if ( system_.compare("powerpc") == 0 )
    return PPCGetSyscallNumber(id);
  std::cerr << "ERROR(unisim::service::os::os_linux): Did not know how to"
      << " translate syscall number " << id << " (0x" << std::hex << id
      << std::dec << ") for system \"" << system_ << "\"" << std::endl;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
int
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
ARMGetSyscallNumber(int id) 
{
	int translated_id = id - 0x0900000;
	return translated_id;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
int
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
ARMEABIGetSyscallNumber(int id) 
{
	// the arm eabi ignores the given id and uses register 7
	//   as the id and translated id
	uint32_t translated_id = 0;
	arm_regs[7]->GetValue(&translated_id);
	return (int)translated_id;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
int
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
PPCGetSyscallNumber(int id) 
{
	return id;
}

// template<class ADDRESS_TYPE, class PARAMETER_TYPE>
// void *
// LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
// PPCConvertStat(struct stat *s) {
//     // this implementation supposes that the host is a x86 with __USE_LARGEFILE64
//     //   and __USE_MISC
//     struct powerpc_stat_t *res;
//       
//     res = (powerpc_stat_t *)malloc(sizeof(powerpc_stat_t));
//     if(GetEndianness() == E_BIG_ENDIAN) {
// #if defined(__x86_64)
// 		res->st_dev = Host2Target(endianness_, (uint64_t)s->st_dev);
//         res->st_ino = Host2Target(endianness_, (uint64_t)s->st_ino);
//         res->st_mode = Host2Target(endianness_, (uint32_t)s->st_mode);
//         res->st_nlink = Host2Target(endianness_, (uint64_t)s->st_nlink);
//         res->st_uid = Host2Target(endianness_, (uint32_t)s->st_uid);
//         res->st_gid = Host2Target(endianness_, (uint32_t)s->st_gid);
//         res->st_rdev = Host2Target(endianness_, (int64_t)s->st_rdev);
//         res->__pad2 = 0;
//         res->st_size = Host2Target(endianness_, (int64_t)s->st_size);
//         res->st_blksize = Host2Target(endianness_, (int64_t)s->st_blksize);
//         res->st_blocks = Host2Target(endianness_, (int64_t)s->st_blocks);
//         res->st_atim.tv_sec = Host2Target(endianness_, (int64_t)s->st_atim.tv_sec);
//         res->st_atim.tv_nsec = Host2Target(endianness_, (int64_t)s->st_atim.tv_nsec);
//         res->st_mtim.tv_sec = Host2Target(endianness_, (int64_t)s->st_mtim.tv_sec);
//         res->st_mtim.tv_nsec = Host2Target(endianness_, (int64_t)s->st_mtim.tv_nsec);
//         res->st_ctim.tv_sec = Host2Target(endianness_, (int64_t)s->st_ctim.tv_sec);
//         res->st_ctim.tv_nsec = Host2Target(endianness_, (int64_t)s->st_ctim.tv_nsec);
// #else
//         res->st_dev = Host2Target(endianness_, (uint64_t)s->st_dev);
//         res->st_ino = Host2Target(endianness_, (uint32_t)s->st_ino);
//         res->st_mode = Host2Target(endianness_, (uint32_t)s->st_mode);
//         res->st_nlink = Host2Target(endianness_, (uint32_t)s->st_nlink);
//         res->st_uid = Host2Target(endianness_, (uint32_t)s->st_uid);
//         res->st_gid = Host2Target(endianness_, (uint32_t)s->st_gid);
//         res->st_rdev = Host2Target(endianness_, (int64_t)s->st_rdev);
//         res->__pad2 = 0;
//         res->st_size = Host2Target(endianness_, (int64_t)s->st_size);
// #ifdef WIN32
//         res->st_blksize = 512;
//         res->st_blocks = s->st_size / 512;
// #else
//         res->st_blksize = Host2Target(endianness_, (int32_t)s->st_blksize);
//         res->st_blocks = Host2Target(endianness_, (int64_t)s->st_blocks);
// #endif
//         res->st_atim.tv_sec = Host2Target(endianness_, (int32_t)s->st_atim);
//         res->st_atim.tv_nsec = 0;
//         res->st_mtim.tv_sec = Host2Target(endianness_, (int32_t)s->st_mtim.tv_sec);
//         res->st_mtim.tv_nsec = 0;
//         res->st_ctim.tv_sec = Host2Target(endianness_, (int32_t)s->st_ctim.tv_sec);
//         res->st_ctim.tv_nsec = 0;
// #endif
// 	} else {
// #if defined(__x86_64)
//         res->st_dev = Host2LittleEndian((uint64_t)s->st_dev);
//         res->st_ino = Host2LittleEndian((uint64_t)s->st_ino);
//         res->st_mode = Host2LittleEndian((uint32_t)s->st_mode);
//         res->st_nlink = Host2LittleEndian((uint64_t)s->st_nlink);
//         res->st_uid = Host2LittleEndian((uint32_t)s->st_uid);
//         res->st_gid = Host2LittleEndian((uint32_t)s->st_gid);
//         res->st_rdev = Host2LittleEndian((int64_t)s->st_rdev);
//         res->__pad2 = 0;
//         res->st_size = Host2LittleEndian((int64_t)s->st_size);
//         res->st_blksize = Host2LittleEndian((int64_t)s->st_blksize);
//         res->st_blocks = Host2LittleEndian((int64_t)s->st_blocks);
//         res->st_atim.tv_sec = Host2LittleEndian((int64_t)s->st_atim.tv_sec);
//         res->st_atim.tv_nsec = Host2LittleEndian((int64_t)s->st_atim.tv_nsec);
//         res->st_mtim.tv_sec = Host2LittleEndian((int64_t)s->st_mtim.tv_sec);
//         res->st_mtim.tv_nsec = Host2LittleEndian((int64_t)s->st_mtim.tv_nsec);
//         res->st_ctim.tv_sec = Host2LittleEndian((int64_t)s->st_ctim.tv_sec);
//         res->st_ctim.tv_nsec = Host2LittleEndian((int64_t)s->st_ctim.tv_nsec);
// #else
//         res->st_dev = Host2LittleEndian((uint64_t)s->st_dev);
//         res->st_ino = Host2LittleEndian((uint32_t)s->st_ino);
//         res->st_mode = Host2LittleEndian((uint32_t)s->st_mode);
//         res->st_nlink = Host2LittleEndian((uint32_t)s->st_nlink);
//         res->st_uid = Host2LittleEndian((uint32_t)s->st_uid);
//         res->st_gid = Host2LittleEndian((uint32_t)s->st_gid);
//         res->st_rdev = Host2LittleEndian((int64_t)s->st_rdev);
//         res->__pad2 = 0;
//         res->st_size = Host2LittleEndian((int64_t)s->st_size);
//         res->st_blksize = Host2LittleEndian((int32_t)s->st_blksize);
//         res->st_blocks = Host2LittleEndian((int64_t)s->st_blocks);
//         res->st_atim.tv_sec = Host2LittleEndian((int32_t)s->st_atim.tv_sec);
//         res->st_atim.tv_nsec = Host2LittleEndian((int32_t)s->st_atim.tv_nsec);
//         res->st_mtim.tv_sec = Host2LittleEndian((int32_t)s->st_mtim.tv_sec);
//         res->st_mtim.tv_nsec = Host2LittleEndian((int32_t)s->st_mtim.tv_nsec);
//         res->st_ctim.tv_sec = Host2LittleEndian((int32_t)s->st_ctim.tv_sec);
//         res->st_ctim.tv_nsec = Host2LittleEndian((int32_t)s->st_ctim.tv_nsec);
// #endif
//     }
//     res->__unused4 = 0;
//     res->__unused5 = 0;
//     return (void *)res;
// }
/*
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void *
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
ARMConvertStat64(struct stat64 *s) {
	// this implementation supposes that the host is a x86 with __USE_LARGEFILE64
	//   and __USE_MISC
  
	struct arm_stat64_t *res;
  
	res = (arm_stat64_t *)malloc(sizeof(arm_stat64_t));
	if(GetEndianness() == E_BIG_ENDIAN) {
#if defined(__x86_64)
		res->st_dev = Host2Target(endianness_, (uint64_t)s->st_dev);
		res->__pad1 = 0;
	    res->__st_ino = Host2Target(endianness_, (uint64_t)s->st_ino);
	    res->st_mode = Host2Target(endianness_, (uint32_t)s->st_mode);
	    res->st_nlink = Host2Target(endianness_, (uint64_t)s->st_nlink);
	    res->st_uid = Host2Target(endianness_, (uint32_t)s->st_uid);
	    res->st_gid = Host2Target(endianness_, (uint32_t)s->st_gid);
	    res->st_rdev = Host2Target(endianness_, (uint64_t)s->st_rdev);
	    res->__pad2 = 0;
	    res->st_size = Host2Target(endianness_, (uint64_t)s->st_size);
	    res->st_blksize = Host2Target(endianness_, (uint32_t)s->st_blksize);
	    res->st_blocks = Host2Target(endianness_, (uint64_t)s->st_blocks);
	    res->st_atim.tv_sec = Host2Target(endianness_, (uint64_t)s->st_atim.tv_sec);
	    res->st_atim.tv_nsec = Host2Target(endianness_, (uint64_t)s->st_atim.tv_nsec);
	    res->st_mtim.tv_sec = Host2Target(endianness_, (uint64_t)s->st_mtim.tv_sec);
	    res->st_mtim.tv_nsec = Host2Target(endianness_, (uint64_t)s->st_mtim.tv_nsec);
	    res->st_ctim.tv_sec = Host2Target(endianness_, (uint64_t)s->st_ctim.tv_sec);
	    res->st_ctim.tv_nsec = Host2Target(endianness_, (uint64_t)s->st_ctim.tv_nsec);
	    res->st_ino = Host2Target(endianness_, (uint64_t)s->st_ino);
#else
	    res->st_dev = Host2Target(endianness_, (uint64_t)s->st_dev);
	    res->__pad1 = 0;
	    res->__st_ino = Host2Target(endianness_, (uint32_t)s->__st_ino);
	    res->st_mode = Host2Target(endianness_, (uint32_t)s->st_mode);
	    res->st_nlink = Host2Target(endianness_, (uint32_t)s->st_nlink);
	    res->st_uid = Host2Target(endianness_, (uint32_t)s->st_uid);
	    res->st_gid = Host2Target(endianness_, (uint32_t)s->st_gid);
	    res->st_rdev = Host2Target(endianness_, (uint64_t)s->st_rdev);
	    res->__pad2 = 0;
	    res->st_size = Host2Target(endianness_, (uint64_t)s->st_size);
	    res->st_blksize = Host2Target(endianness_, (uint32_t)s->st_blksize);
	    res->st_blocks = Host2Target(endianness_, (uint64_t)s->st_blocks);
	    res->st_atim.tv_sec = Host2Target(endianness_, (uint32_t)s->st_atim.tv_sec);
	    res->st_atim.tv_nsec = Host2Target(endianness_, (uint32_t)s->st_atim.tv_nsec);
	    res->st_mtim.tv_sec = Host2Target(endianness_, (uint32_t)s->st_mtim.tv_sec);
	    res->st_mtim.tv_nsec = Host2Target(endianness_, (uint32_t)s->st_mtim.tv_nsec);
	    res->st_ctim.tv_sec = Host2Target(endianness_, (uint32_t)s->st_ctim.tv_sec);
	    res->st_ctim.tv_nsec = Host2Target(endianness_, (uint32_t)s->st_ctim.tv_nsec);
	    res->st_ino = Host2Target(endianness_, (uint64_t)s->st_ino);
#endif
	} else {
#if defined(__x86_64)
	    res->st_dev = Host2LittleEndian((uint64_t)s->st_dev);
	    res->__pad1 = 0;
	    res->__st_ino = Host2LittleEndian((uint64_t)s->st_ino);
	    res->st_mode = Host2LittleEndian((uint32_t)s->st_mode);
	    res->st_nlink = Host2LittleEndian((uint64_t)s->st_nlink);
	    res->st_uid = Host2LittleEndian((uint32_t)s->st_uid);
	    res->st_gid = Host2LittleEndian((uint32_t)s->st_gid);
	    res->st_rdev = Host2LittleEndian((uint64_t)s->st_rdev);
	    res->__pad2 = 0;
	    res->st_size = Host2LittleEndian((uint64_t)s->st_size);
	    res->st_blksize = Host2LittleEndian((uint32_t)s->st_blksize);
	    res->st_blocks = Host2LittleEndian((uint64_t)s->st_blocks);
	    res->st_atim.tv_sec = Host2LittleEndian((uint64_t)s->st_atim.tv_sec);
	    res->st_atim.tv_nsec = Host2LittleEndian((uint64_t)s->st_atim.tv_nsec);
	    res->st_mtim.tv_sec = Host2LittleEndian((uint64_t)s->st_mtim.tv_sec);
	    res->st_mtim.tv_nsec = Host2LittleEndian((uint64_t)s->st_mtim.tv_nsec);
	    res->st_ctim.tv_sec = Host2LittleEndian((uint64_t)s->st_ctim.tv_sec);
	    res->st_ctim.tv_nsec = Host2LittleEndian((uint64_t)s->st_ctim.tv_nsec);
	    res->st_ino = Host2LittleEndian((uint64_t)s->st_ino);
#else
	    res->st_dev = Host2LittleEndian((uint64_t)s->st_dev);
	    res->__pad1 = 0;
	    res->__st_ino = Host2LittleEndian((uint32_t)s->__st_ino);
	    res->st_mode = Host2LittleEndian((uint32_t)s->st_mode);
	    res->st_nlink = Host2LittleEndian((uint32_t)s->st_nlink);
	    res->st_uid = Host2LittleEndian((uint32_t)s->st_uid);
	    res->st_gid = Host2LittleEndian((uint32_t)s->st_gid);
	    res->st_rdev = Host2LittleEndian((uint64_t)s->st_rdev);
	    res->__pad2 = 0;
	    res->st_size = Host2LittleEndian((uint64_t)s->st_size);
	    res->st_blksize = Host2LittleEndian((uint32_t)s->st_blksize);
	    res->st_blocks = Host2LittleEndian((uint64_t)s->st_blocks);
	    res->st_atim.tv_sec = Host2LittleEndian((uint32_t)s->st_atim.tv_sec);
	    res->st_atim.tv_nsec = Host2LittleEndian((uint32_t)s->st_atim.tv_nsec);
	    res->st_mtim.tv_sec = Host2LittleEndian((uint32_t)s->st_mtim.tv_sec);
	    res->st_mtim.tv_nsec = Host2LittleEndian((uint32_t)s->st_mtim.tv_nsec);
	    res->st_ctim.tv_sec = Host2LittleEndian((uint32_t)s->st_ctim.tv_sec);
	    res->st_ctim.tv_nsec = Host2LittleEndian((uint32_t)s->st_ctim.tv_nsec);
	    res->st_ino = Host2LittleEndian((uint64_t)s->st_ino);
#endif
	}
	return (void *)res;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void *
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
PPCConvertStat64(struct stat64 *s) {
    // this implementation supposes that the host is a x86 with __USE_LARGEFILE64
    //   and __USE_MISC
    
	struct powerpc_stat64_t *res;
    
    res = (powerpc_stat64_t *)malloc(sizeof(powerpc_stat64_t));
    if(GetEndianness() == E_BIG_ENDIAN) {
#if defined(__x86_64)
    	res->st_dev = Host2Target(endianness_, (uint64_t)s->st_dev);
      	res->st_ino = Host2Target(endianness_, (uint64_t)s->st_ino);
      	res->st_mode = Host2Target(endianness_, (uint32_t)s->st_mode);
      	res->st_nlink = Host2Target(endianness_, (uint64_t)s->st_nlink);
      	res->st_uid = Host2Target(endianness_, (uint32_t)s->st_uid);
      	res->st_gid = Host2Target(endianness_, (uint32_t)s->st_gid);
      	res->st_rdev = Host2Target(endianness_, (int64_t)s->st_rdev);
      	res->__pad2 = 0;
      	res->st_size = Host2Target(endianness_, (int64_t)s->st_size);
      	res->st_blksize = Host2Target(endianness_, (int32_t)s->st_blksize);
      	res->st_blocks = Host2Target(endianness_, (int64_t)s->st_blocks);
      	res->st_atim.tv_sec = Host2Target(endianness_, (int64_t)s->st_atim.tv_sec);
      	res->st_atim.tv_nsec = Host2Target(endianness_, (int64_t)s->st_atim.tv_nsec);
      	res->st_mtim.tv_sec = Host2Target(endianness_, (int64_t)s->st_mtim.tv_sec);
		res->st_mtim.tv_nsec = Host2Target(endianness_, (int64_t)s->st_mtim.tv_nsec);
		res->st_ctim.tv_sec = Host2Target(endianness_, (int64_t)s->st_ctim.tv_sec);
		res->st_ctim.tv_nsec = Host2Target(endianness_, (int64_t)s->st_ctim.tv_nsec);
#else
		res->st_dev = Host2Target(endianness_, (uint64_t)s->st_dev);
      	res->st_ino = Host2Target(endianness_, (uint32_t)s->__st_ino);
      	res->st_mode = Host2Target(endianness_, (uint32_t)s->st_mode);
      	res->st_nlink = Host2Target(endianness_, (uint32_t)s->st_nlink);
      	res->st_uid = Host2Target(endianness_, (uint32_t)s->st_uid);
      	res->st_gid = Host2Target(endianness_, (uint32_t)s->st_gid);
      	res->st_rdev = Host2Target(endianness_, (int64_t)s->st_rdev);
      	res->__pad2 = 0;
      	res->st_size = Host2Target(endianness_, (int64_t)s->st_size);
      	res->st_blksize = Host2Target(endianness_, (int32_t)s->st_blksize);
      	res->st_blocks = Host2Target(endianness_, (int64_t)s->st_blocks);
      	res->st_atim.tv_sec = Host2Target(endianness_, (int32_t)s->st_atim.tv_sec);
      	res->st_atim.tv_nsec = Host2Target(endianness_, (int32_t)s->st_atim.tv_nsec);
      	res->st_mtim.tv_sec = Host2Target(endianness_, (int32_t)s->st_mtim.tv_sec);
      	res->st_mtim.tv_nsec = Host2Target(endianness_, (int32_t)s->st_mtim.tv_nsec);
      	res->st_ctim.tv_sec = Host2Target(endianness_, (int32_t)s->st_ctim.tv_sec);
      	res->st_ctim.tv_nsec = Host2Target(endianness_, (int32_t)s->st_ctim.tv_nsec);
#endif
    } else {
#if defined(__x86_64)
    	res->st_dev = Host2LittleEndian((uint64_t)s->st_dev);
      	res->st_ino = Host2LittleEndian((uint64_t)s->st_ino);
      	res->st_mode = Host2LittleEndian((uint32_t)s->st_mode);
      	res->st_nlink = Host2LittleEndian((uint64_t)s->st_nlink);
      	res->st_uid = Host2LittleEndian((uint32_t)s->st_uid);
      	res->st_gid = Host2LittleEndian((uint32_t)s->st_gid);
      	res->st_rdev = Host2LittleEndian((int64_t)s->st_rdev);
      	res->__pad2 = 0;
      	res->st_size = Host2LittleEndian((int64_t)s->st_size);
      	res->st_blksize = Host2LittleEndian((int32_t)s->st_blksize);
      	res->st_blocks = Host2LittleEndian((int64_t)s->st_blocks);
      	res->st_atim.tv_sec = Host2LittleEndian((int64_t)s->st_atim.tv_sec);
		res->st_atim.tv_nsec = Host2LittleEndian((int64_t)s->st_atim.tv_nsec);
		res->st_mtim.tv_sec = Host2LittleEndian((int64_t)s->st_mtim.tv_sec);
		res->st_mtim.tv_nsec = Host2LittleEndian((int64_t)s->st_mtim.tv_nsec);
		res->st_ctim.tv_sec = Host2LittleEndian((int64_t)s->st_ctim.tv_sec);
		res->st_ctim.tv_nsec = Host2LittleEndian((int64_t)s->st_ctim.tv_nsec);
#else
		res->st_dev = Host2LittleEndian((uint64_t)s->st_dev);
		res->st_ino = Host2LittleEndian((uint32_t)s->__st_ino);
		res->st_mode = Host2LittleEndian((uint32_t)s->st_mode);
		res->st_nlink = Host2LittleEndian((uint32_t)s->st_nlink);
		res->st_uid = Host2LittleEndian((uint32_t)s->st_uid);
		res->st_gid = Host2LittleEndian((uint32_t)s->st_gid);
		res->st_rdev = Host2LittleEndian((int64_t)s->st_rdev);
		res->__pad2 = 0;
		res->st_size = Host2LittleEndian((int64_t)s->st_size);
		res->st_blksize = Host2LittleEndian((int32_t)s->st_blksize);
		res->st_blocks = Host2LittleEndian((int64_t)s->st_blocks);
		res->st_atim.tv_sec = Host2LittleEndian((int32_t)s->st_atim.tv_sec);
		res->st_atim.tv_nsec = Host2LittleEndian((int32_t)s->st_atim.tv_nsec);
		res->st_mtim.tv_sec = Host2LittleEndian((int32_t)s->st_mtim.tv_sec);
		res->st_mtim.tv_nsec = Host2LittleEndian((int32_t)s->st_mtim.tv_nsec);
		res->st_ctim.tv_sec = Host2LittleEndian((int32_t)s->st_ctim.tv_sec);
		res->st_ctim.tv_nsec = Host2LittleEndian((int32_t)s->st_ctim.tv_nsec);
#endif
    }
    res->__unused4 = 0;
    res->__unused5 = 0;
    return (void *)res;
}
  
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void *
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
ARMConvertTms(struct tms *t) {
	struct arm_tms_t *res;
  
	res = (arm_tms_t *)malloc(sizeof(arm_tms_t));
	if(GetEndianness() == E_BIG_ENDIAN) {
		res->tms_utime = Host2Target(endianness_, (uint32_t)t->tms_utime);
		res->tms_stime = Host2Target(endianness_, (uint32_t)t->tms_stime);
		res->tms_cutime = Host2Target(endianness_, (uint32_t)t->tms_cutime);
		res->tms_cstime = Host2Target(endianness_, (uint32_t)t->tms_cstime);
	} else {
		res->tms_utime = Host2LittleEndian((uint32_t)t->tms_utime);
		res->tms_stime = Host2LittleEndian((uint32_t)t->tms_stime);
		res->tms_cutime = Host2LittleEndian((uint32_t)t->tms_cutime);
		res->tms_cstime = Host2LittleEndian((uint32_t)t->tms_cstime);
	}
  
	return (void *)res;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void *
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
PPCConvertTms(struct tms *t) {
    struct powerpc_tms_t *res;
    
    res = (powerpc_tms_t *)malloc(sizeof(powerpc_tms_t));
    if(GetEndianness() == E_BIG_ENDIAN) {
		res->tms_utime = Host2Target(endianness_, (int32_t)t->tms_utime);
		res->tms_stime = Host2Target(endianness_, (int32_t)t->tms_stime);
		res->tms_cutime = Host2Target(endianness_, (int32_t)t->tms_cutime);
		res->tms_cstime = Host2Target(endianness_, (int32_t)t->tms_cstime);
    } else {
		res->tms_utime = Host2LittleEndian((int32_t)t->tms_utime);
		res->tms_stime = Host2LittleEndian((int32_t)t->tms_stime);
		res->tms_cutime = Host2LittleEndian((int32_t)t->tms_cutime);
		res->tms_cstime = Host2LittleEndian((int32_t)t->tms_cstime);
    }
    
    return (void *)res;
}*/
  

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
ADDRESS_TYPE 
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
GetMmapBase() const 
{
	return mmap_base;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void 
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
SetMmapBase(ADDRESS_TYPE base) 
{
	mmap_base = base;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
ADDRESS_TYPE 
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
GetMmapBrkPoint() const 
{
	return mmap_brk_point;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void 
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
SetMmapBrkPoint(ADDRESS_TYPE brk_point) 
{
	mmap_brk_point = brk_point;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
ADDRESS_TYPE 
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
GetBrkPoint() const 
{
	return brk_point;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void 
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
SetBrkPoint(ADDRESS_TYPE brk_point) 
{
	this->brk_point = brk_point;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
PARAMETER_TYPE
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
GetSystemCallParam(int id) 
{
	if ( system == "arm" )
		return ARMGetSystemCallParam(id);
	else if ( system == "arm-eabi" )
		return ARMEABIGetSystemCallParam(id);
	else
		return PPCGetSystemCallParam(id);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
PARAMETER_TYPE
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
ARMGetSystemCallParam(int id) 
{
	PARAMETER_TYPE val;
	arm_regs[id]->GetValue(&val);
	return val;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
PARAMETER_TYPE
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
ARMEABIGetSystemCallParam(int id) 
{
	PARAMETER_TYPE val;
	arm_regs[id]->GetValue(&val);
	return val;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
PARAMETER_TYPE
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
PPCGetSystemCallParam(int id) 
{
    PARAMETER_TYPE val;
    ppc_regs[id+3]->GetValue(&val);
    return val;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void 
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
SetSystemCallStatus(int ret, bool error) 
{
	if ( system == "arm" )
		ARMSetSystemCallStatus(ret, error);
	else if ( system == "arm-eabi" )
		ARMEABISetSystemCallStatus(ret, error);
	else
		PPCSetSystemCallStatus(ret, error);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void 
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
ARMSetSystemCallStatus(int ret, bool error) 
{
	PARAMETER_TYPE val = (PARAMETER_TYPE)ret;
	arm_regs[0]->SetValue(&val);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void 
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
ARMEABISetSystemCallStatus(int ret, bool error) 
{
	PARAMETER_TYPE val = (PARAMETER_TYPE)ret;
	arm_regs[0]->SetValue(&val);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void 
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
PPCSetSystemCallStatus(int ret, bool error) 
{
    PARAMETER_TYPE val;
    if (error) 
	{
		ppc_cr->GetValue(&val);
		val |= (1 << 28); // CR0[SO] <- 1
      	ppc_cr->SetValue(&val);
    }
 	else
	{
		ppc_cr->GetValue(&val);
		val &= ~(1 << 28); // CR0[SO] <- 0
		ppc_cr->SetValue(&val);
    }
    val = (PARAMETER_TYPE)ret;
    ppc_regs[3]->SetValue(&val);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
endian_type
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
GetEndianness()
{
	return endianness_;
}
//
//void 
//ARMLinuxOS::Reset() {
//	return cpu_arm_linux_os_import->Reset();
//}
//
//bool 
//ARMLinuxOS::ReadMemory(ADDRESS_TYPE addr, void *buffer, uint32_t size) {
//	return cpu_arm_linux_os_import->ReadMemory(addr, buffer, size);
//}
//
//bool 
//ARMLinuxOS::WriteMemory(ADDRESS_TYPE addr, const void *buffer, uint32_t size) {
//	return cpu_arm_linux_os_import->WriteMemory(addr, buffer, size);
//}
//

#endif

} // end of os_linux namespace
} // end of os namespace
} // end of service namespace
} // end of unisim namespace

#undef LOCATION

#endif // __UNISIM_SERVICE_OS_OS_LINUX_LINUX_OS_TCC__
