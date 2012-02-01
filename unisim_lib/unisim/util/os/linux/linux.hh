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
// #include <iostream>
#include <sstream>

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
namespace linux_os {

template <class ADDRESS_TYPE>
class LinuxMemoryInterface {
 public:
  virtual bool ReadMemory(ADDRESS_TYPE addr, uint8_t * const buffer,
                          ADDRESS_TYPE size) = 0;
  virtual bool WriteMemory(ADDRESS_TYPE addr, uint8_t const * const buffer,
                           ADDRESS_TYPE size) = 0;
  virtual bool InjectReadMemory(ADDRESS_TYPE addr, uint8_t * const buffer,
                          ADDRESS_TYPE size) = 0;
  virtual bool InjectWriteMemory(ADDRESS_TYPE addr,
                                 uint8_t const * const buffer,
                                 ADDRESS_TYPE size) = 0;
};

template <class PARAMETER_TYPE>
class LinuxRegisterInterface {
 public:
  virtual bool GetRegister(uint32_t id, PARAMETER_TYPE * const value) = 0;
  virtual bool SetRegister(uint32_t id, PARAMETER_TYPE value) = 0;
};

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
class Linux {
 public:
  Linux(std::ostringstream * const logger);
  ~Linux();

  void  SetVerbose(bool verbose);

  bool SetCommandLine(std::vector<std::string> const &cmd);

  std::vector<std::string> GetCommandLine();

  bool SetEnvironment(std::vector<std::string> const &env);

  std::vector<std::string> GetEnvironment();

  void SetApplyHostEnvironment(bool use_host_environment);

  bool GetApplyHostEnvironment();

  bool AddLoadFile(char const * const file);

  bool SetSystemType(char const * const system_type);

  // Sets the endianness of the system.
  // Note that if the loaded files endianness is different from the set
  // endianness the Load() method will fail.
  void SetEndianness(unisim::util::endian::endian_type endianness);

  // Sets the stack base address that will be used for the application stack.
  // Combined with SetStackSize the memory addresses from stack base to 
  // (stack base + stack size) will be reserved for the stack.
  void SetStackBase(ADDRESS_TYPE stack_base);

  // Sets the stack size in bytes that will be reserved for the application
  // stack
  void SetStackSize(unsigned int stack_size);

  // Sets the memory page size in bytes that will be used by the linux os
  // emulator
  void SetMemoryPageSize(ADDRESS_TYPE memory_page_size);

  // Sets the values that will be used by the uname system call.
  void SetUname(char const * const utsname_sysname,
                char const * const utsname_nodename,
                char const * const utsname_release,
                char const * const utsname_version,
                char const * const utsname_machine,
                char const * const utsname_domainname);

  // TODO: Remove
  // // Sets the registers to be used
  // void SetRegisters(std::vector<unisim::util::debug::Register *> &registers);
  // END TODO

  // Set the register interface to be used
  void SetRegisterInterface(LinuxRegisterInterface<PARAMETER_TYPE> &iface);

  // Set the memory interface to be used
  void SetMemoryInterface(LinuxMemoryInterface<ADDRESS_TYPE> &iface);

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
  // information from the loaded system with the GetBlob or request setup of the
  // target emulator with SetupTarget.
  // Returns: true on success, false otherwise.
  bool Load();
  // Checks if the system was already successfully loaded or not.
  // Returns: true if the system has already successfully been loaded, false
  //          if the system has not been loaded or if the Load() method
  //          failed.
  bool IsLoad();

  // Sets up the target system using the currently loaded configuration (blob)
  // using the register and memory interface.
  // Returns: true on success, false otherwise.
  bool SetupTarget();
  bool SetupARMTarget();
  bool SetupPPCTarget();

  // Gets the memory footprint of the application as a blob.
  // Returns: a blob describing the memory footprint of the application. NULL
  //          if the system has not been successfully loaded.
  unisim::util::debug::blob::Blob<ADDRESS_TYPE> const * const GetBlob() const;

  // Executes the given system call id depending on the architecture the linux
  // emulation is working on.
  void ExecuteSystemCall(int id);

 private:
  bool is_load_; // true if a program has been successfully loaded, false
                 // otherwise

  // basic system information
  std::string system_type_;
  static const int kNumSupportedSystemTypes;
  static const std::vector<std::string> supported_system_types_; // [kNumSupportedSystemTypes];
  unisim::util::endian::endian_type endianness_;

  // files to load
  std::map<std::string,
      unisim::util::debug::blob::Blob<ADDRESS_TYPE> const *>
          load_files_;


  // program addresses (computed from the given files)
  ADDRESS_TYPE entry_point_;
  bool load_addr_set_;
  ADDRESS_TYPE load_addr_;
  ADDRESS_TYPE start_code_;
  ADDRESS_TYPE end_code_;
  ADDRESS_TYPE start_data_;
  ADDRESS_TYPE end_data_;
  ADDRESS_TYPE elf_stack_;
  ADDRESS_TYPE elf_brk_;
  int num_segments_;
  ADDRESS_TYPE stack_base_;
  uint64_t stack_size_;
  ADDRESS_TYPE max_environ_;
  uint64_t memory_page_size_;
  ADDRESS_TYPE mmap_base_;
  ADDRESS_TYPE mmap_brk_point_;
  ADDRESS_TYPE brk_point_;


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

  // number of aux table entries on the stack (minus AT_NULL)
  int num_aux_table_entries_;
  // size of each of the entries on the aux table
  int aux_table_entry_size_;

  // the structure to keep all the loaded information
  unisim::util::debug::blob::Blob<ADDRESS_TYPE> *blob_;

  // TODO Remove registers
  // this member variable is used during the Load() operation
  //   to retrieve the registers depending on the architecture
  //   being simulated
  // std::vector<unisim::util::debug::Register *> *registers_;
  // END TODO

  // pointer to the interface for the registers to use
  LinuxRegisterInterface<PARAMETER_TYPE> *register_interface_;

  // pointer to the interface for memory to use
  LinuxMemoryInterface<ADDRESS_TYPE> *memory_interface_;

  // syscall type shortener
  typedef Linux<ADDRESS_TYPE,PARAMETER_TYPE> thistype;
  typedef void (thistype::*syscall_t)();

  // system calls indexes
  std::map<std::string, syscall_t> syscall_name_map_;
  std::map<int, std::string> syscall_name_assoc_map_;
  std::map<int, syscall_t> syscall_impl_assoc_map_;

  // current syscall information
  int current_syscall_id_;
  std::string current_syscall_name_;

  // stubs for reading the osrelease file (see calls.tcc LSC_open)
  const char *kOsreleaseFilename;
  const char *kFakeOsreleaseFilename;

  // activate the verbose
  bool verbose_;
  // logger stream
  std::ostringstream * logger_;
  // loader logger stream
  std::ostringstream loader_logger_;

  // Maps the registers depending on the system
  // Returns true on success
  bool MapRegisters();

  // Load the files set by the user into the given blob. Returns true on sucess,
  // false otherwise.
  bool LoadFiles(unisim::util::debug::blob::Blob<ADDRESS_TYPE> *blob);

  // Gets the main executable blob, that is the blob that represents the
  // executable file, not the maybe used dynamic libraries
  unisim::util::debug::blob::Blob<ADDRESS_TYPE> const * const
      GetMainBlob() const;

  // From the given blob computes the initial addresses and values that will be
  // used to initialize internal structures and the target processor
  bool ComputeStructuralAddresses(
      unisim::util::debug::blob::Blob<ADDRESS_TYPE> const &blob);

  // Merge the contents of the given file blob into the input/output blob
  bool FillBlobWithFileBlob(
      unisim::util::debug::blob::Blob<ADDRESS_TYPE> const &file_blob,
      unisim::util::debug::blob::Blob<ADDRESS_TYPE> *blob);

  // Create the stack memory image and insert it into the given blob
  bool CreateStack(unisim::util::debug::blob::Blob<ADDRESS_TYPE> * blob) const;

  // Set the aux table contents that will be added to the stack
  void SetAuxTable(uint8_t * stack_data, ADDRESS_TYPE & sp) const;

  // Set the contents of an aux table entry
  ADDRESS_TYPE SetAuxTableEntry(uint8_t * stack_data, ADDRESS_TYPE sp,
                                ADDRESS_TYPE entry, ADDRESS_TYPE value) const;

  // Fills the given blob with system dependent information
  bool SetSystemBlob(unisim::util::debug::blob::Blob<ADDRESS_TYPE> *blob) const;

  // Fills the given blob with ARM dependent information
  bool SetArmBlob(unisim::util::debug::blob::Blob<ADDRESS_TYPE> *blob) const;

  // Fills the given blob with PPC dependent information
  bool SetPPCBlob(unisim::util::debug::blob::Blob<ADDRESS_TYPE> *blob) const;

  // Associate the syscall identifier to its name
  void SetSyscallId(const std::string &syscall_name, int syscall_id);

  // Set the system calls mapping between names and their implementation
  void SetSyscallNameMap();

  // Determine if a syscall is available
  bool HasSyscall(const std::string &syscall_name);
  bool HasSyscall(int syscall_id);

  // Extract the system call number from the given identifier depending on the
  // architecture being emulated
  int GetSyscallNumber(int id);
  int ARMGetSyscallNumber(int id);
  int ARMEABIGetSyscallNumber(int id);
  int PPCGetSyscallNumber(int id);

  // helper methods to read/write from/into the system memory for performing
  // system calls or loading the initial memory image
  bool ReadMem(ADDRESS_TYPE addr, uint8_t * const buffer, uint32_t size);
  bool WriteMem(ADDRESS_TYPE addr, uint8_t const * const buffer, uint32_t size);

  // Cleans the contents of the loader logger_ and clean error flags
  void ResetLoaderLogger() {
    std::string clean_string("");
    loader_logger_.str(clean_string);
    loader_logger_.clear();
  }

  // Prints to the output console the contents of loader logger_
  void PrintLoaderLogger() {
    if (logger_ != NULL) {
      if (loader_logger_.str().size() != 0) {
        *logger_ << "Message from the elf loader:" << std::endl;
        *logger_ << "BEGIN ======================================" << std::endl;
        *logger_ << loader_logger_.str();
        *logger_ << "END ========================================" << std::endl;
        ResetLoaderLogger();
      }
    }
  }

  // The list of linux system calls
  void LSC_unknown();
  void LSC_exit();
  void LSC_read();
  void LSC_write();
  void LSC_open();
  void LSC_close();
  void LSC_lseek();
  void LSC_getpid();
  void LSC_getuid();
  void LSC_access();
  void LSC_times();
  void LSC_brk();
  void LSC_getgid();
  void LSC_geteuid();
  void LSC_getegid();
  void LSC_munmap();
  void LSC_stat();
  void LSC_fstat();
  void LSC_uname();
  void LSC_llseek();
  void LSC_writev();
  void LSC_mmap();
  void LSC_mmap2();
  void LSC_stat64();
  void LSC_fstat64();
  void LSC_getuid32();
  void LSC_getgid32();
  void LSC_geteuid32();
  void LSC_getegid32();
  void LSC_flistxattr();
  void LSC_exit_group();
  void LSC_fcntl();
  void LSC_fcntl64();
  void LSC_dup();
  void LSC_ioctl();
  void LSC_ugetrlimit();
  void LSC_getrlimit();
  void LSC_setrlimit();
  void LSC_rt_sigaction();
  void LSC_getrusage();
  void LSC_unlink();
  void LSC_rename();
  void LSC_time();
  void LSC_socketcall();
  void LSC_rt_sigprocmask();
  void LSC_kill();
  void LSC_ftruncate();
  void LSC_arm_breakpoint();
  void LSC_arm_cacheflush();
  void LSC_arm_usr26();
  void LSC_arm_usr32();
  void LSC_arm_set_tls();

  // system call 'stat' helper methods
  int Stat(int fd, struct powerpc_stat *target_stat);
  int Stat64(int fd, struct powerpc_stat64 *target_stat);
  int Stat64(int fd, struct arm_stat64 *target_stat);
  // system call 'times' helper methods
  int Times(struct powerpc_tms *target_tms);
  int Times(struct arm_tms *target_tms);
  // handling the mmap base address
  ADDRESS_TYPE GetMmapBase() const;
  void SetMmapBase(ADDRESS_TYPE base);
  // handling the mmapbrkpoint address
  ADDRESS_TYPE GetMmapBrkPoint() const;
  void SetMmapBrkPoint(ADDRESS_TYPE brk_point);
  // handling the brkpoint address
  ADDRESS_TYPE GetBrkPoint() const;
  void SetBrkPoint(ADDRESS_TYPE brk_point);
  // reading system calls parameters
  PARAMETER_TYPE GetSystemCallParam(int id);
  PARAMETER_TYPE ARMGetSystemCallParam(int id);
  PARAMETER_TYPE ARMEABIGetSystemCallParam(int id);
  PARAMETER_TYPE PPCGetSystemCallParam(int id);
  // writing system call status
  void SetSystemCallStatus(int ret, bool error);
  void ARMSetSystemCallStatus(int ret, bool error);
  void ARMEABISetSystemCallStatus(int ret, bool error);
  void PPCSetSystemCallStatus(int ret, bool error);
  // byte swap helper methods
  bool Swap();
  void PerformSwap(void *buf, int count);
  // compute the length of a buffer string
  int StringLength(ADDRESS_TYPE addr);
};

} // end of linux namespace
} // end of os namespace
} // end of util namespace
} // end of unisim namespace

#endif // __UNISIM_UTIL_OS_LINUX_LINUX_HH__
