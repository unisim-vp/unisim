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
 *          Gilles Mouchard (gilles.mouchard@cea.fr)
 */
 
#ifndef __UNISIM_SERVICE_OS_LINUX_OS_LINUX_TCC__
#define __UNISIM_SERVICE_OS_LINUX_OS_LINUX_TCC__

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

#include "unisim/service/os/linux_os/linux.hh"
#include "unisim/util/os/linux_os/linux.hh"
#include "unisim/util/os/linux_os/arm.hh"
#include "unisim/util/os/linux_os/ppc.hh"

#define LOCATION 	" - location = " << __FUNCTION__ << ":unisim/service/os/linux_os/linux_os.tcc:" << __LINE__

namespace unisim {
namespace service {
namespace os {
namespace linux_os {

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugError;
using unisim::kernel::logger::EndDebug;

/** Constructor. */
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
Linux<ADDRESS_TYPE, PARAMETER_TYPE>::
Linux(const char *name, unisim::kernel::service::Object *parent)
    : unisim::kernel::service::Object(name, parent)
    , unisim::kernel::service::Service<unisim::service::interfaces::LinuxOS>(
        name, parent)
    , unisim::kernel::service::Service<unisim::service::interfaces::Blob<ADDRESS_TYPE> >(
        name, parent)
    , unisim::kernel::service::Client<
      unisim::service::interfaces::Memory<ADDRESS_TYPE> >(name, parent)
    , unisim::kernel::service::Client<
      unisim::service::interfaces::MemoryInjection<ADDRESS_TYPE> >(name, parent)
    , unisim::kernel::service::Client<unisim::service::interfaces::Registers>(
        name, parent)
    , linux_os_export_("linux-os-export", this)
    , blob_export_("blob-export", this)
    , memory_import_("memory-import", this)
    , memory_injection_import_("memory-injection-import", this)
    , registers_import_("registers-import", this)
    , logger_(*this)
    , verbose_(false)
    , param_verbose_("verbose", this, verbose_)
    , parse_dwarf_(false)
	, debug_dwarf_(false)
    , dwarf_to_html_output_directory_()
    , dwarf_to_xml_output_filename_()
    , param_parse_dwarf_("parse-dwarf", this, parse_dwarf_)
	, param_debug_dwarf_("debug-dwarf", this, debug_dwarf_)
    , param_dwarf_to_html_output_directory_("dwarf-to-html-output-directory", this, dwarf_to_html_output_directory_)
    , param_dwarf_to_xml_output_filename_("dwarf-to-xml-output-filename", this, dwarf_to_xml_output_filename_)
    , linuxlib_(0)
    , system_("")
    , param_system_("system", this, system_, "Emulated system architecture "
                   "available values are \"arm\", \"arm-eabi\" and \"powerpc\"")
    , endianness_(unisim::util::endian::E_LITTLE_ENDIAN)
    , param_endianness_("endianness", this, endianness_,
                   "The endianness of the binary loaded. Available values are:"
                   " little-endian and big-endian.")
    , memory_page_size_(4096)
    , param_memory_page_size_("memory-page-size", this, memory_page_size_)
    , stack_base_(0x4000000UL)
    , param_stack_base_("stack-base", this, stack_base_,
                        "The stack base address used for the load and execution"
                        " of the linux application")
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
    , apply_host_environment_(false)
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
                               " system call should return.") {

  param_argc_.SetFormat(unisim::kernel::service::VariableBase::FMT_DEC);
  
  linux_os_export_.SetupDependsOn(memory_import_);
  linux_os_export_.SetupDependsOn(registers_import_);
}

/** Destructor. */
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
Linux<ADDRESS_TYPE, PARAMETER_TYPE>::~Linux() {
  if(linuxlib_)
  {
    delete linuxlib_;
    linuxlib_ = 0;
  }

  typename std::vector<std::string *>::iterator argv_it;
  for(argv_it = argv_.begin(); argv_it != argv_.end(); argv_it++) {
    delete *argv_it;
  }
  typename std::vector<unisim::kernel::service::Parameter<std::string> *>::iterator param_argv_it;
  for(param_argv_it = param_argv_.begin(); param_argv_it != param_argv_.end(); param_argv_it++) {
    delete *param_argv_it;
  }
  typename std::vector<std::string *>::iterator envp_it;
  for(envp_it = envp_.begin(); envp_it != envp_.end(); envp_it++) {
    delete *envp_it;
  }
  typename std::vector<unisim::kernel::service::Parameter<std::string> *>::iterator param_envp_it;
  for(param_envp_it = param_envp_.begin(); param_envp_it != param_envp_.end(); param_envp_it++) {
    delete *param_envp_it;
  }
}

/** Method to execute when the Linux is disconnected from its client. */
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::OnDisconnect() {
  // NOTHING ?
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
bool Linux<ADDRESS_TYPE, PARAMETER_TYPE>::BeginSetup() {
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

  // check the endianness parameter
  if(endianness_ == unisim::util::endian::E_UNKNOWN_ENDIAN)
  {
    logger_ << DebugError
        << "Unknown endian value. Correct values are:"
        << " little-endian and big-endian"
        << EndDebugError;
    return false;
  }

  // check that the given system is supported
  // NOTE: This should not be done because the linux library is already doing it
  if ((system_.compare("arm") != 0) &&
      (system_.compare("arm-eabi") != 0) &&
      (system_.compare("ppc") != 0)) {
    logger_ << DebugError
        << "Unsupported system (" << system_ << "), this service only supports"
        << " arm and ppc systems" << std::endl
        << LOCATION
        << EndDebugError;
    return false;
  }

  linuxlib_ = new unisim::util::os::linux_os::Linux<ADDRESS_TYPE, PARAMETER_TYPE>(logger_, registers_import_, memory_import_, memory_injection_import_);
  
  // set up the different linuxlib parameters
  linuxlib_->SetVerbose(verbose_);
  linuxlib_->SetParseDWARF(parse_dwarf_);
  linuxlib_->SetDebugDWARF(debug_dwarf_);
  linuxlib_->SetDWARFToHTMLOutputDirectory(dwarf_to_html_output_directory_.c_str());
  linuxlib_->SetDWARFToXMLOutputFilename(dwarf_to_xml_output_filename_.c_str());

  // set the linuxlib command line
  if (argc_ != 0) {
    std::vector<std::string> cmdline;
    for (unsigned int i = 0; i < argc_; i++)
      cmdline.push_back(*argv_[i]);
    bool success = linuxlib_->SetCommandLine(cmdline);
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
    bool success = linuxlib_->SetEnvironment(envparms);
    if (!success) {
      logger_ << DebugError
          << "Could not set the application environment."
          << EndDebugError;
      return false;
    }
  }

  // set the linuxlib option to set the target environment with the host
  // environment
  linuxlib_->SetApplyHostEnvironment(apply_host_environment_);

  // set the binary that will be simulated in the target simulator
  {
    bool success = linuxlib_->AddLoadFile(binary_.c_str());
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
    bool success = linuxlib_->SetSystemType(system_.c_str());
    if (!success) {
      logger_ << DebugError
          << "System type not supported (\"" << system_ << "\")."
          << EndDebugError;
      return false;
    }
  }

  // set the endianness of the target simulator
  linuxlib_->SetEndianness(endianness_);
  // .. the stack base address
  linuxlib_->SetStackBase(stack_base_);
  // .. and memory page size
  linuxlib_->SetMemoryPageSize(memory_page_size_);
  // .. and the uname information
  linuxlib_->SetUname(utsname_sysname_.c_str(), utsname_nodename_.c_str(),
                     utsname_release_.c_str(), utsname_version_.c_str(),
                     utsname_machine_.c_str(), utsname_domainname_.c_str());

  // now it is time to try to run the initialization of the linuxlib
  {
    bool success = linuxlib_->Load();
    if (!success) {
      logger_ << DebugError
          << "Could not initialize the linux support with the given parameters"
          << ", please check them."
          << EndDebugError;
      return false;
    }
  }

  return true;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
bool Linux<ADDRESS_TYPE, PARAMETER_TYPE>::Setup(
    unisim::kernel::service::ServiceExportBase *srv_export) {
  if ( srv_export == &linux_os_export_ ) {
    if (!linuxlib_->SetupTarget()) {
      logger_ << DebugError << "Could not setup the linux system"
          << EndDebugError;
      return false;
    }
    return true;
  }

  if ( srv_export == &blob_export_ ) {
    // already setup in BeginSetup
    return true;
  }

  logger_ << DebugError << "Internal error" << EndDebugError;

  return false;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
bool Linux<ADDRESS_TYPE, PARAMETER_TYPE>::EndSetup() {
  return true;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::ExecuteSystemCall(int id) {
  bool terminated = false;
  int return_status = 0;
  linuxlib_->ExecuteSystemCall(id, terminated, return_status);
  if(terminated) Object::Stop(return_status);
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
const unisim::util::debug::blob::Blob<ADDRESS_TYPE> *Linux<ADDRESS_TYPE, PARAMETER_TYPE>::GetBlob() const {
  return linuxlib_->GetBlob();
}

} // end of linux_os namespace
} // end of os namespace
} // end of service namespace
} // end of unisim namespace

#undef LOCATION

#endif // __UNISIM_SERVICE_OS_LINUX_OS_LINUX_TCC__