/*
 *  Copyright (c) 2011 Commissariat a l'Energie Atomique (CEA) All rights
 *  reserved.
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
 *  ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 */

#ifndef __UNISIM_UTIL_OS_LINUX_LINUX_HH__
#define __UNISIM_UTIL_OS_LINUX_LINUX_HH__

#include <map>
#include <string>
#include <vector>
#include <iostream>

#include "unisim/util/endian/endian.hh"

namespace unisim {
namespace util {
namespace debug {
class Register;
namespace blob {
template<class MEMORY_ADDR>
class Blob;
} // end of namespace blob
} // end of namespace debug
} // end of namespace util
} // end of namespace unisim

namespace unisim {
namespace util {
namespace os {
namespace linux {

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
class Linux {
 public:
  Linux(bool verbose, std::ostream &logger);
  ~Linux();

  bool AddLoadFile(char const * const file);

  bool SetSystemType(char const * const system_type);

  // Sets the endianess of the system.
  // Note that if the loaded files endianess is different from the set
  // endianess the Load() method will fail.
  void SetEndianess(unisim::util::endian::endian_type endianess);

  // Sets the stack size that will be reserved for the application stack
  bool SetStackSize(unsigned int stack_size);

  // Sets the memory page size that will be used by the linux os emulator
  bool SetMemoryPageSize(ADDRESS_TYPE memory_page_size);

  // Sets the values that will be used by the uname system call.
  void SetUname(char const * const utsname_sysname,
                char const * const utsname_nodename,
                char const * const utsname_release,
                char const * const utsname_version,
                char const * const utsname_machine,
                char const * const utsname_domainname);

  void SetRegisters(std::vector<unisim::util::debug::Register *> &registers);

  // Loads all the defined files using the user settings.
  // Basic usage:
  //   linux_os = new Linux<X,Y>(false);
  //   // set options (for simplicity we assume successful calls)
  //   linux_os->SetSystemType("arm-eabi");
  //   linux_os->AddLoadFile("my_fabulous_elf_file");
  //   ...
  //   // actually load the system
  //   if (linux_os->Load()) //success
  //     ...;
  //   else // failure
  //     ...;
  // Once Load() has been called and successfully returned, the user can get
  // information from the loaded system with the GetBlob, GetRegister, and
  // other get methods.
  // Returns: true on success, false otherwise.
  bool Load();
  // Checks if the system was already successfully loaded or not.
  // Returns: true if the system has already successfully been loaded, false
  //          if the system has not been loaded or if the Load() method
  //          failed.
  bool IsLoad();

  // Gets the memory footprint of the application as a blob.
  // Returns: a blob describing the memory footprint of the application. NULL
  //          if the system has not been successfully loaded.
  unisim::util::debug::blob::Blob<ADDRESS_TYPE> const * const GetBlob() const;

 private:
  bool is_load_; // true if a program has been successfully loaded, false 
                 // otherwise

  // basic system information
  std::string system_type_;
  static const int kNumSupportedSystemTypes = 3;
  static const std::string supported_system_types_[kNumSupportedSystemTypes];
  unisim::util::endian::endian_type endianess_;

  // files to load
  std::map<std::string, 
      unisim::util::debug::blob::Blob<ADDRESS_TYPE> const * const>
          load_files_;


  // program addresses (computed from the given files)
  ADDRESS_TYPE entry_point;
  bool load_addr_set = false;
  ADDRESS_TYPE load_addr;
  ADDRESS_TYPE start_code;
  ADDRESS_TYPE end_code;
  ADDRESS_TYPE start_data;
  ADDRESS_TYPE end_data;
  ADDRESS_TYPE elf_stack;
  ADDRESS_TYPE elf_brk;
  ADDRESS_TYPE num_segments;

  // argc, argv and envp variables
  unsigned int argc_;
  std::vector<std::string> argv_;
  unsigned int envc_;
  std::vector<std::string> envp_;
  bool apply_host_environnement_;
  std::vector<std::string> target_envp_;

  // utsname values
  std::string utsname_sysname_;
  std::string utsname_nodename_;
  std::string utsname_release_;
  std::string utsname_version_;
  std::string utsname_machine_;
  std::string utsname_domainname_;

  // the structure to keep all the loaded information
  unisim::util::debug::blob::Blob<ADDRESS_TYPE> *blob_;

  PARAMETER_TYPE entry_point_;
  PARAMETER_TYPE stack_base_;
  PARAMETER_TYPE stack_size_;
  PARAMETER_TYPE max_environ_;
  PARAMETER_TYPE memory_page_size_;
  PARAMETER_TYPE mmap_base_;
  PARAMETER_TYPE mmap_brk_point_;
  PARAMETER_TYPE brk_point_;

  // this member variable is used during the Load() operation
  //   to retrieve the registers depending on the architecture
  //   being simulated
  std::vector<unisim::util::debug::Register *> *registers_;

  // registers for the arm system
  const int kARMNumRegs = 16;
  const int kARMNumSysRegs = 0;
  unisim::util::debug::Register *arm_regs_[kARMNumRegs];

  // registers for the ppc system
  const int kPPCNumRegs = 31;
  const int kPPCNumSysRegs = 2;
  unisim::util::debug::Register *ppc_cr_;
  unisim::util::debug::Register *ppc_cia_;
  unisim::util::debug::Register *ppc_regs_[kPPCNumRegs];

  // activate the verbose
  bool verbose_;
  // logger for the elf loader
  std::ostream &logger_;

  // Gets the main executable blob, that is the blob that represents the
  // executable file, not the maybe used dynamic libraries
  unisim::util::debug::blob::Blob<ADDRESS_TYPE> const * const GetMainBlob() const;

  // Maps the registers depending on the system
  // Returns true on success
  bool MapRegisters();

  // Cleans the contents of the loader_logger_ and clean error flags
  void ResetLoaderLogger() {
    std::string clean_string();
    loader_logger_.str(clean_string);
    loader_logger_.clear();
  }

  // Prints to the output console the contents of loader_logger_
  void PrintLoaderLogger(std::ostream &output) {
    if (loader_logger_.str().size() != 0) {
      output << "Message from the elf loader:" << std::endl;
      output << "BEGIN ======================================" << std::endl;
      output << loader_logger_.str();
      output << "END ========================================" << std::endl;
      ResetLoaderLogger();
    }
  }
};

} // end of linux namespace
} // end of os namespace
} // end of util namespace
} // end of unisim namespace

#endif // __UNISIM_UTIL_OS_LINUX_LINUX_HH__
