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
 * Authors: Yves Lhuillier (yves.lhuillier@cea.fr)
 *          Gilles Mouchard (gilles.mouchard@cea.fr)
 */

#ifndef __UNISIM_SERVICE_OS_LINUX_OS_LINUX_TCC__
#define __UNISIM_SERVICE_OS_LINUX_OS_LINUX_TCC__

#include <unisim/service/os/linux_os/linux.hh>
#include <unisim/service/interfaces/linux_os.hh>
#include <unisim/service/interfaces/loader.hh>
#include <unisim/service/interfaces/memory.hh>
#include <unisim/service/interfaces/memory_injection.hh>
#include <unisim/service/interfaces/registers.hh>
#include <unisim/service/interfaces/register.hh>
#include <unisim/kernel/kernel.hh>
#include <unisim/kernel/logger/logger.hh>
#include <unisim/util/os/linux_os/linux.hh>
#include <unisim/util/loader/elf_loader/elf32_loader.hh>
#include <unisim/util/loader/elf_loader/elf64_loader.hh>
#include <unisim/util/endian/endian.hh>
#include <unisim/util/likely/likely.hh>

#include <string>
#include <sstream>

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
Linux(const char* name, unisim::kernel::Object* parent)
  : unisim::kernel::Object(name, parent)
  , unisim::kernel::Service<unisim::service::interfaces::LinuxOS>(name, parent)
  , unisim::kernel::Service<unisim::service::interfaces::Blob<ADDRESS_TYPE> >(name, parent)
  , unisim::kernel::Service<unisim::service::interfaces::SymbolTableLookup<ADDRESS_TYPE> >(name, parent)
  , unisim::kernel::Service<unisim::service::interfaces::StatementLookup<ADDRESS_TYPE> >(name, parent)
  , unisim::kernel::Client<unisim::service::interfaces::Memory<ADDRESS_TYPE> >(name, parent)
  , unisim::kernel::Client<unisim::service::interfaces::MemoryInjection<ADDRESS_TYPE> >(name, parent)
  , unisim::kernel::Client<unisim::service::interfaces::Registers>(name, parent)
  , linux_os_export_("linux-os-export", this)
  , blob_export_("blob-export", this)
  , symbol_table_lookup_export("symbol-table-lookup-export", this)
  , statement_lookup_export("statement-lookup-export", this)
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
  , stdin_pipe_filename()
  , stdout_pipe_filename()
  , stderr_pipe_filename()
  , param_stdin_pipe_filename("stdin-pipe-filename", this, stdin_pipe_filename, "stdin pipe filename")
  , param_stdout_pipe_filename("stdout-pipe-filename", this, stdout_pipe_filename, "stdout pipe filename")
  , param_stderr_pipe_filename("stderr-pipe-filename", this, stderr_pipe_filename, "stderr pipe filename")
  , elf_loader(logger_.DebugInfoStream(), logger_.DebugWarningStream(), logger_.DebugErrorStream())
  , linux_lib(logger_.DebugInfoStream(), logger_.DebugWarningStream(), logger_.DebugErrorStream())
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
                  " is the same value than the argv[0] parameter.")
  , argc_(0)
  , param_argc_("argc", this, argc_,
                "Number of commands in the program execution line (usually at"
                " least one which is the name of the program executed). The"
                " different tokens can be set up with the parameters"
                " argv[<n>] where <n> can go up to argc - 1.")
  , argv_(argc_)
  , param_argv_()
  , apply_host_cmd_line_(true)
  , param_apply_host_cmd_line_("apply-host-cmd-line", this,
                               apply_host_cmd_line_,
                               "Whether to apply the host command line on"
                               " the target simulator or use the provided"
                               " argc and argv.")
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
  , utsname_release_("5.10.46")
  , param_utsname_release_("utsname-release", this, utsname_release_,
                           "The kernel realese information that the uname"
                           " system call should return. This should usually"
                           " match the linux-kernel parameter.")
  , utsname_version_("#UNISIM SMP Thu Jun 24 14:33:54 UTC 2021")
  , param_utsname_version_("utsname-version", this, utsname_version_,
                           "The kernel version information that the uname"
                           " system call should return.")
  , utsname_machine_("noarch")
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
  , hwcap_("")
  , param_hwcap_("hwcap", this, hwcap_,
                 "CPU Hardware capabilities to enable (e.g. \"swp thumb fastmult vfp\".")
{
  param_argc_.SetFormat(unisim::kernel::VariableBase::FMT_DEC);
  param_envc_.SetFormat(unisim::kernel::VariableBase::FMT_DEC);

  typedef unisim::kernel::variable::Parameter<std::string> parameter_type;
  for (unsigned int i = 0; i < argc_; i++)
    {
      std::stringstream argv_name, argv_desc, argv_val;
      argv_name << "argv[" << i << "]";
      argv_desc << "The '" << i << "' token in the command line.";
      argv_val << "Undefined argv[" << i << "] value";
      argv_[i] = argv_val.str();
      auto param = new parameter_type(argv_name.str().c_str(), this, argv_[i], argv_desc.str().c_str());
      param_argv_.push_back(param);
    }

  for (unsigned int i = 0; i < envc_; i++)
    {
      std::stringstream envp_name, envp_desc, envp_val;
      envp_name << "envp[" << i << "]";
      envp_desc << "The '" << i << "' token in the environment.";
      envp_val << "Undefined envp[" << i << "] value";
      envp_.push_back(envp_val.str());
      auto param = new parameter_type(envp_name.str().c_str(), this, envp_[i], envp_desc.str().c_str());
      param_envp_.push_back(param);
    }
}

/** Destructor. */
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
Linux<ADDRESS_TYPE, PARAMETER_TYPE>::~Linux()
{
  for (auto param_argv_ptr : param_argv_)
    delete param_argv_ptr;

  for (auto param_envp_ptr : param_envp_)
    delete param_envp_ptr;
}

/** Setup os emulator implementaion, apply external parameters */
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
bool Linux<ADDRESS_TYPE, PARAMETER_TYPE>::BeginSetup()
{
  // check the endianness parameter
  if(endianness_ == unisim::util::endian::E_UNKNOWN_ENDIAN)
  {
    logger_ << DebugError
        << "Unknown endian value. Correct values are:"
        << " little-endian and big-endian"
        << EndDebugError;
    return false;
  }

  // set up the different linuxlib parameters
  linux_lib.SetVerbose(verbose_);

  // set the linuxlib command line
  std::vector<std::string> const& cmd_args = apply_host_cmd_line_ ? GetSimulator()->GetCmdArgs() : argv_;

  if (cmd_args.empty())
    {
      logger_ << DebugError << "No command line was given for the target simulator." << EndDebugError;
      return false;
    }

  linux_lib.SetCommandLine(cmd_args);

  // set the binary that will be simulated in the target simulator
  {
    elf_loader.SetOption(unisim::util::loader::elf_loader::OPT_VERBOSE, verbose_);
    elf_loader.SetFileName(std::string(cmd_args[0]));
    elf_loader.SetRegistersInterface(/* prc_num */ 0, registers_import_);
    elf_loader.SetMemoryInterface(/* prc_num */ 0, memory_import_);
    elf_loader.SetOption(unisim::util::loader::elf_loader::OPT_PARSE_DWARF, parse_dwarf_);
    elf_loader.SetOption(unisim::util::loader::elf_loader::OPT_DEBUG_DWARF, debug_dwarf_);
    elf_loader.SetOption(unisim::util::loader::elf_loader::OPT_DWARF_TO_HTML_OUTPUT_DIRECTORY, dwarf_to_html_output_directory_.c_str());
    elf_loader.SetOption(unisim::util::loader::elf_loader::OPT_DWARF_TO_XML_OUTPUT_FILENAME, dwarf_to_xml_output_filename_.c_str());
    if (not elf_loader.Load())
      {
        logger_ << DebugError << "Could not set the binary file to simulate on the target simulator." << EndDebugError;
        return false;
      }

    linux_lib.SetFileBlob(elf_loader.GetBlob());
  }

  // set the linuxlib environment
  if (envc_ != 0) {
    bool success = linux_lib.SetEnvironment(envp_);
    if (!success) {
      logger_ << DebugError
          << "Could not set the application environment."
          << EndDebugError;
      return false;
    }
  }

  // set the linuxlib option to set the target environment with the host
  // environment
  linux_lib.SetApplyHostEnvironment(apply_host_environment_);

  // setup target specific implementation

  this->SetupTargetSystem();

  // set the endianness of the target simulator
  linux_lib.SetEndianness(endianness_);
  // .. the stack base address
  linux_lib.SetStackBase(stack_base_);
  // .. and memory page size
  linux_lib.SetMemoryPageSize(memory_page_size_);
  // .. and the uname information
  linux_lib.SetUname(utsname_sysname_.c_str(), utsname_nodename_.c_str(),
                     utsname_release_.c_str(), utsname_version_.c_str(),
                     utsname_machine_.c_str(), utsname_domainname_.c_str());
  linux_lib.SetHWCap(hwcap_.c_str());
  // .. and the stdin/stdout/stderr pipe filenames (if any)
  linux_lib.SetStdinPipeFilename(stdin_pipe_filename.c_str());
  linux_lib.SetStdoutPipeFilename(stdout_pipe_filename.c_str());
  linux_lib.SetStderrPipeFilename(stderr_pipe_filename.c_str());

  // now it is time to try to run the initialization of the linuxlib
  linux_lib.SetInterfaces(registers_import_, memory_import_, memory_injection_import_);
  if (not linux_lib.Load())
    {
      logger_ << DebugError << "Could not initialize the linux support with the given parameters, please check them." << EndDebugError;
      return false;
    }

  return true;
}


template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::Setup(unisim::service::interfaces::LinuxOS*)
{
  if (memory_import_)
    memory_import_.RequireSetup();
  if (registers_import_)
    registers_import_.RequireSetup();

  if (!linux_lib.SetupTarget())
    {
      logger_ << DebugError << "Could not setup the linux system" << EndDebugError;
      throw unisim::kernel::ServiceAgent::SetupError();
    }
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
Linux<ADDRESS_TYPE, PARAMETER_TYPE>::ExecuteSystemCall(int id)
{
  bool terminated = false;
  int return_status = 0;
  //  linux_lib.LogSystemCall(id);
  linux_lib.ExecuteSystemCall(id, terminated, return_status);
  if (terminated) Object::Stop(return_status);
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
const unisim::util::blob::Blob<ADDRESS_TYPE>*
Linux<ADDRESS_TYPE, PARAMETER_TYPE>::GetBlob() const
{
  return linux_lib.GetBlob();
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
void
Linux<ADDRESS_TYPE, PARAMETER_TYPE>::ScanSymbols(unisim::service::interfaces::SymbolTableScanner<ADDRESS_TYPE>& scanner) const
{
  if (elf_loader.GetBlob())
    { elf_loader.ScanSymbols(scanner); }
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
void
Linux<ADDRESS_TYPE, PARAMETER_TYPE>::ScanSymbols(unisim::service::interfaces::SymbolTableScanner<ADDRESS_TYPE>& scanner, typename unisim::util::debug::SymbolBase::Type type) const
{
  if (elf_loader.GetBlob())
    { elf_loader.ScanSymbols(scanner, type); }
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
const typename unisim::util::debug::Symbol<ADDRESS_TYPE>*
Linux<ADDRESS_TYPE, PARAMETER_TYPE>::FindSymbolByAddr(ADDRESS_TYPE addr) const
{
  return elf_loader.GetBlob() ? elf_loader.FindSymbolByAddr(addr) : 0;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
const typename unisim::util::debug::Symbol<ADDRESS_TYPE>*
Linux<ADDRESS_TYPE, PARAMETER_TYPE>::FindSymbolByName(const char *name) const
{
  return elf_loader.GetBlob() ? elf_loader.FindSymbolByName(name) : 0;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
const typename unisim::util::debug::Symbol<ADDRESS_TYPE>*
Linux<ADDRESS_TYPE, PARAMETER_TYPE>::FindSymbolByName(const char *name, typename unisim::util::debug::SymbolBase::Type type) const
{
  return elf_loader.GetBlob() ? elf_loader.FindSymbolByName(name, type) : 0;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
const typename unisim::util::debug::Symbol<ADDRESS_TYPE>*
Linux<ADDRESS_TYPE, PARAMETER_TYPE>::FindSymbolByAddr(ADDRESS_TYPE addr, typename unisim::util::debug::SymbolBase::Type type) const
{
  return elf_loader.GetBlob() ? elf_loader.FindSymbolByAddr(addr, type) : 0;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
void
Linux<ADDRESS_TYPE, PARAMETER_TYPE>::ScanStatements(unisim::service::interfaces::StatementScanner<ADDRESS_TYPE>& scanner, const char *filename) const
{
  if (elf_loader.GetBlob()) { elf_loader.ScanStatements(scanner); }
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
const unisim::util::debug::Statement<ADDRESS_TYPE>*
Linux<ADDRESS_TYPE, PARAMETER_TYPE>::FindStatement(ADDRESS_TYPE addr, const char *filename, typename unisim::service::interfaces::StatementLookup<ADDRESS_TYPE>::Scope scope) const
{
  return elf_loader.GetBlob() ? elf_loader.FindStatement(addr, scope) : 0;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
const unisim::util::debug::Statement<ADDRESS_TYPE>*
Linux<ADDRESS_TYPE, PARAMETER_TYPE>::FindStatements(unisim::service::interfaces::StatementScanner<ADDRESS_TYPE>& scanner, ADDRESS_TYPE addr, const char *filename, typename unisim::service::interfaces::StatementLookup<ADDRESS_TYPE>::Scope scope) const
{
  return elf_loader.GetBlob() ? elf_loader.FindStatements(scanner, addr, scope) : 0;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
const unisim::util::debug::Statement<ADDRESS_TYPE>*
Linux<ADDRESS_TYPE, PARAMETER_TYPE>::FindStatement(const unisim::util::debug::SourceCodeLocation& source_code_location, const char *filename) const
{
  return elf_loader.GetBlob() ? elf_loader.FindStatement(source_code_location) : 0;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
const unisim::util::debug::Statement<ADDRESS_TYPE>*
Linux<ADDRESS_TYPE, PARAMETER_TYPE>::FindStatements(unisim::service::interfaces::StatementScanner<ADDRESS_TYPE>& scanner, const unisim::util::debug::SourceCodeLocation& source_code_location, const char *filename) const
{
  return elf_loader.GetBlob() ? elf_loader.FindStatements(scanner, source_code_location) : 0;
}

} // end of linux_os namespace
} // end of os namespace
} // end of service namespace
} // end of unisim namespace

#undef LOCATION

#endif // __UNISIM_SERVICE_OS_LINUX_OS_LINUX_TCC__
