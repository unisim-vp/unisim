// TODO Fix register handling, should be done with methods.

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

#ifndef __UNISIM_UTIL_OS_LINUX_LINUX_TCC__
#define __UNISIM_UTIL_OS_LINUX_LINUX_TCC__

#if !defined(linux) && !defined(__APPLE_CC__) && !defined(WIN32) && !defined(WIN64)
#error "Unsupported host machine for Linux system call translation !"
#endif

#include <string>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <string.h>
#include <stdlib.h>

#ifdef WIN32
#include <process.h>
#else
#include <sys/times.h>
#endif

#include "unisim/util/likely/likely.hh"
#include "unisim/util/debug/blob/blob.hh"
#include "unisim/util/endian/endian.hh"
#include "unisim/util/loader/elf_loader/elf32_loader.hh"

#include "unisim/util/os/linux/arm.hh"
#include "unisim/util/os/linux/ppc.hh"
#include "unisim/util/os/linux/environment.hh"
#include "unisim/util/os/linux/aux_table.hh"

namespace unisim {
namespace util {
namespace os {
namespace linux_os {

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
const int Linux<ADDRESS_TYPE, PARAMETER_TYPE>::kNumSupportedSystemTypes = 3;

const char * tmp_supported_system_types[] = {"arm", "arm-eabi", "powerpc"};
template <class ADDRESS_TYPE, class PARAMETER_TYPE>
const std::vector<std::string> Linux<ADDRESS_TYPE, PARAMETER_TYPE>::
supported_system_types_(tmp_supported_system_types, 
                        tmp_supported_system_types + 
                        sizeof(tmp_supported_system_types) / sizeof(char *));

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
Linux<ADDRESS_TYPE, PARAMETER_TYPE>::
Linux(std::ostringstream * const logger)
    : is_load_(false)
    , system_type_("arm-eabi")
    , endianness_(unisim::util::endian::E_LITTLE_ENDIAN)
    , load_files_()
    , entry_point_(0)
    , load_addr_set_(false)
    , load_addr_(0)
    , start_code_(0)
    , end_code_(0)
    , start_data_(0)
    , end_data_(0)
    , elf_stack_(0)
    , elf_brk_(0)
    , num_segments_(0)
    , stack_base_(0)
    , stack_size_(0)
    , max_environ_(0)
    , memory_page_size_(0)
    , mmap_base_(0xd4000000)
    , mmap_brk_point_(0)
    , brk_point_(0)
    , argc_(0)
    , argv_()
    , envc_(0)
    , envp_()
    , apply_host_environnement_(false)
    , target_envp_()
    , utsname_sysname_()
    , utsname_nodename_()
    , utsname_release_()
    , utsname_version_()
    , utsname_machine_()
    , utsname_domainname_()
    , num_aux_table_entries_(13) // TODO Set the number of aux table entries
                                 //      depending on the architecture
    , aux_table_entry_size_(2 * sizeof(ADDRESS_TYPE))
    , blob_(NULL)
    , register_interface_(NULL)
    , memory_interface_(NULL)
    , syscall_name_map_()
    , syscall_name_assoc_map_()
    , syscall_impl_assoc_map_()
    , current_syscall_id_(0)
    , current_syscall_name_("(NONE)")
    , kOsreleaseFilename("/proc/sys/kernel/osrelease")
    , kFakeOsreleaseFilename("osrelease")
    // , registers_(NULL) // TODO Remove
    , verbose_(false)
    , logger_(logger)
    , loader_logger_(std::ostringstream::out) {
  //for (int i = 0; i < kArmNumRegs; ++i)
    //arm_regs_[i] = NULL;
  //for (int i = 0; i < kPpcNumRegs; ++i)
    //ppc_regs_[i] = NULL;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
Linux<ADDRESS_TYPE, PARAMETER_TYPE>::~Linux() {
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetVerbose(bool verbose) {
  verbose_ = verbose;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
bool Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetCommandLine(
    std::vector<std::string> const &cmd) {
  argc_ = cmd.size();
  argv_ = cmd;

  return true;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
std::vector<std::string> Linux<ADDRESS_TYPE, PARAMETER_TYPE>::GetCommandLine() {
  return argv_;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
bool Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetEnvironment(
    std::vector<std::string> const &env) {
  envc_ = env.size();
  envp_ = env;

  return true;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
std::vector<std::string> Linux<ADDRESS_TYPE, PARAMETER_TYPE>::GetEnvironment() {
  return envp_;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetApplyHostEnvironment(
    bool use_host_environment) {
  apply_host_environnement_ = use_host_environment;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
bool Linux<ADDRESS_TYPE, PARAMETER_TYPE>::GetApplyHostEnvironment() {
  return apply_host_environnement_;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
bool Linux<ADDRESS_TYPE, PARAMETER_TYPE>::AddLoadFile(
    char const * const filename) {
  // NOTE: for the moment we only support one file to load, if this
  // method is called more than once it will return false.
  if (load_files_.size() != 0) return false;

  // check that filename points to something
  if (filename == NULL) {
    std::cerr << "WARNING(unisim::util::os::linux): calling AddLoadFile "
        << "without handling file." << std::endl;
    return false;
  }

  ResetLoaderLogger();
  // check that the file exists and that the elf loader can create a blob from it
  unisim::util::loader::elf_loader::Elf32Loader<ADDRESS_TYPE> *loader =
      new unisim::util::loader::elf_loader::Elf32Loader<ADDRESS_TYPE>(
          loader_logger_);

  if (loader == NULL) {
    std::cerr
        << "ERROR(unisim::util::os::linux): Could not create an elf loader."
        << std::endl;
    PrintLoaderLogger();
    return false;
  }

  loader->SetOption(unisim::util::loader::elf_loader::OPT_VERBOSE, verbose_);
  loader->SetOption(unisim::util::loader::elf_loader::OPT_FILENAME, filename);

  if (!loader->Load()) {
    std::cerr
        << "ERROR(unisim::util::os::linux): Could not load the given file "
        << "\"" << filename << "\"" << std::endl;
    PrintLoaderLogger();
    return false;
  }

  unisim::util::debug::blob::Blob<ADDRESS_TYPE> const * const blob =
      loader->GetBlob();
  if (blob == NULL) {
    std::cerr << "ERROR(unisim::util::os::linux): Could not create blob from"
        << " requested file (" << filename << ")." << std::endl;
    PrintLoaderLogger();
    delete loader;
    return false;
  }

  blob->Catch();
  std::string filename_str(filename);
  load_files_[filename_str] = blob;
  delete loader;
  return true;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
bool Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetSystemType(
    char const * const system_type) {
  bool supported = false;
  for (int i = 0; (!supported) && (i < kNumSupportedSystemTypes); ++i) {
    supported = (supported_system_types_[i].compare(system_type) == 0);
  }
  if (!supported) return false;
  system_type_ = system_type;
  return true;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetEndianness(
    unisim::util::endian::endian_type endianness) {
  endianness = endianness_;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetStackBase(
    ADDRESS_TYPE stack_base) {
  stack_base_ = stack_base;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetStackSize(
    unsigned int stack_size) {
  stack_size_ = stack_size;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetMemoryPageSize(
    ADDRESS_TYPE memory_page_size) {
  memory_page_size_ = memory_page_size;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetUname(
    char const * const utsname_sysname,
    char const * const utsname_nodename,
    char const * const utsname_release,
    char const * const utsname_version,
    char const * const utsname_machine,
    char const * const utsname_domainname) {
  utsname_sysname_    = utsname_sysname;
  utsname_nodename_   = utsname_nodename;
  utsname_release_    = utsname_release;
  utsname_version_    = utsname_version;
  utsname_machine_    = utsname_machine;
  utsname_domainname_ = utsname_domainname;
}

// TODO Remove
//template <class ADDRESS_TYPE, class PARAMETER_TYPE>
//void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetRegisters(
    //std::vector<unisim::util::debug::Register *> &registers) {
  //registers_ = &registers;
//}
// END TODO

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetRegisterInterface(
    LinuxRegisterInterface<PARAMETER_TYPE> &iface) {
  register_interface_ = &iface;
}
template <class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetMemoryInterface(
    LinuxMemoryInterface<ADDRESS_TYPE> &iface) {
  memory_interface_ = &iface;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
bool Linux<ADDRESS_TYPE, PARAMETER_TYPE>::Load() {
  std::vector<std::string> target_envp;

  //// get and check all the cpu registers
  //if (!MapRegisters()) {
    //return false;
  //}

  // set the environment to be used
  if (apply_host_environnement_) GetHostEnvironment(envp_, &target_envp);
  else target_envp = envp_;

  // create the root blob that will be used to store the image that will be
  // loaded
  unisim::util::debug::blob::Blob<ADDRESS_TYPE>* blob =
      new unisim::util::debug::blob::Blob<ADDRESS_TYPE>();
  blob->Catch();

  // load and add files to the blob
  if (verbose_)
    std::cout << "(unisim::util::os::linux_os::Linux.Load): "
        << "Loading elf files." << std::endl;
  if (!LoadFiles(blob)) {
    PrintLoaderLogger();
    std::cerr << "ERROR(unisim::util::os::linux_os::Linux.Load): "
        << "Could not load elf files." << std::endl;
    blob->Release();
    return false;
  }

  // create the stack footprint and add it to the blob
  if (verbose_)
    std::cout << "(unisim::util::os::linux_os::Linux.Load): "
        << "Creating the Linux software stack." << std::endl;
  if (!CreateStack(blob)) {
    // TODO
    // Remove non finished state (i.e., unfinished blob, reset values, ...)
    std::cerr << "ERROR(unisim::util::os::linux_os::Linux.Load): "
        << "Could not create the Linux software stack." << std::endl;
    blob->Release();
    return false;
  }

  // finish the state of the memory image depending on the system we are running
  // on
  if (verbose_)
    std::cout << "(unisim::util::os::linux_os::Linux.Load): "
        << "Setting the system blob." << std::endl;
  if (!SetSystemBlob(blob)) {
    // TODO
    // Remove non finished state (i.e., unfinished blob, reset values, ...)
    std::cerr << "ERROR(unisim::util::os::linux_os::Linux.Load): "
        << "Could not set the system blob." << std::endl;
    blob->Release();
    return false;
  }

  blob_ = blob;

  return true;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
bool Linux<ADDRESS_TYPE, PARAMETER_TYPE>::IsLoad() {
  return blob_ != NULL;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
bool Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetupTarget() {
  if (blob_ == NULL) {
    std::cerr << "ERROR(unisim::util::os::linux_os::Linux.SetupTarget): "
        << "The linux system was not loaded." << std::endl;
    return false;
  }

  if ((system_type_.compare("arm") == 0) || 
      (system_type_.compare("arm-eabi") == 0))
    return SetupARMTarget();
  else if (system_type_.compare("powerpc") == 0)
    return SetupPPCTarget();

  std::cerr << "ERROR(unisim::util::os::linux_os::Linux.SetupTarget): "
      << "Unknown system to setup (" << system_type_ << ")." << std::endl;
  return false;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
bool Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetupARMTarget() {
  typedef unisim::util::debug::blob::Segment<ADDRESS_TYPE> Segment;
  typedef std::vector<const Segment*> SegmentVector;
  typedef typename SegmentVector::const_iterator SegmentVectorIterator;

  bool success = true;
  SegmentVector segments = blob_->GetSegments();
  for (SegmentVectorIterator it = segments.begin();
       success && (it != segments.end()); it++) {
    if ((*it)->GetType() == Segment::TY_LOADABLE) {
      std::cerr << "--> writing memory segment" << std::endl;
      uint8_t const * data = (uint8_t const *)(*it)->GetData();
      ADDRESS_TYPE start, end;
      (*it)->GetAddrRange(start, end);

      success = memory_interface_->WriteMemory(start, data, end - start + 1);
    }
  }

  if (!success) {
    std::cerr << "ERROR(unisim::util::os::linux_os::Linux.SetupARMTarget): "
        << "Error while writing the segments into the target memory."
        << std::endl;
    return false;
  }

  // Reset all the target registers
  unsigned int reg_id = 0;
  for (reg_id = 0; success && (reg_id < kARMNumRegs); reg_id++) {
    success = register_interface_->SetRegister(reg_id, 0);
  }
  if (!success) {
    std::cerr << "ERROR(unisim::util::os::linux_os::Linux.SetupARMTarget): "
        << "Error while setting register '" << (reg_id - 1) << "'" << std::endl;
    return false;
  }
  // Set PC to the program entry point
  success = register_interface_->SetRegister(kARM_pc, entry_point_);
  if (!success) {
    std::cerr << "ERROR(unisim::util::os::linux_os::Linux.SetupARMTarget): "
        << "Error while setting pc register (" << kARM_pc << ")" << std::endl;
    return false;
  }
  // Set SP to the base of the created stack
  unisim::util::debug::blob::Section<ADDRESS_TYPE> const * sp_section =
      blob_->FindSection(".unisim.linux_os.stack.stack_pointer");
  if (sp_section == NULL) {
    std::cerr << "ERROR(unisim::util::os::linux_os::Linux.SetupARMTarget): "
        << "Could not find the stack pointer section." << std::endl;
    return false;
  }
  success = register_interface_->SetRegister(kARM_sp, sp_section->GetAddr());
  if (!success) {
    std::cerr << "ERROR(unisim::util::os::linux_os::Linux.SetupARMTarget): "
        << "Error while setting sp register (" << kARM_sp << ")" << std::endl;
    return false;
  }
  ADDRESS_TYPE par1_addr = sp_section->GetAddr() + 4;
  ADDRESS_TYPE par2_addr = sp_section->GetAddr() + 8;
  PARAMETER_TYPE par1 = 0;
  PARAMETER_TYPE par2 = 0;
  // TODO check endianness conversions
  success = memory_interface_->ReadMemory(par1_addr, (uint8_t *)&par1, sizeof(par1)); 
  success = memory_interface_->ReadMemory(par2_addr, (uint8_t *)&par2, sizeof(par2));
  register_interface_->SetRegister(kARM_r1, par1);
  register_interface_->SetRegister(kARM_r2, par2);


  return true;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
bool Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetupPPCTarget() {
  return false;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
unisim::util::debug::blob::Blob<ADDRESS_TYPE> const * const Linux<ADDRESS_TYPE,
    PARAMETER_TYPE>::GetBlob() const { return blob_; }

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::ExecuteSystemCall(int id) {
  if (is_load_) {
    std::cerr << "ERROR(unisim::util::os::linux_os::Linux.ExecuteSystemCall): "
        << "Trying to execute system call with id " << id << " while the linux "
        << "emulation has not been correctly loaded." << std::endl;
    return;
  }

  int translated_id = GetSyscallNumber(id);

  if (HasSyscall(translated_id)) {
    current_syscall_id_ = translated_id;
    current_syscall_name_ = syscall_name_assoc_map_[translated_id];
    if (unlikely(verbose_))
      (*logger_)
          << "Executing syscall(name = " << current_syscall_name_ << ";"
          << " id = " << translated_id << ";"
          << " unstranslated id = " << id << ")" << std::endl;
    syscall_t y = syscall_impl_assoc_map_[translated_id];
    (this->*y)();
  } else {
    (*logger_)
        << "Could not find system call id " << id << " (translated id = "
        << translated_id << "), aborting system call "
        << "execution." << std::endl;
  }
}

//template <class ADDRESS_TYPE, class PARAMETER_TYPE>
//bool Linux<ADDRESS_TYPE, PARAMETER_TYPE>::MapRegisters() {
  //typedef std::vector<unisim::util::debug::Register *>::iterator RegIterator;

  //int num_registers_expected = 0;
  //if ((system_type_.compare("arm") == 0) ||
      //(system_type_.compare("arm-eabi") == 0))
    //num_registers_expected = kARMNumRegs + kARMNumSysRegs;
  //else
    //num_registers_expected = kPPCNumRegs + kPPCNumSysRegs;
  //if (registers_.size() < num_registers_expected) {
    //logger_ << "Could not initiliaze register mapping. "
        //"Number of registers handled is inferior to the number of registers "
        //"expected." << std::endl
        //<< "Number of registers expected = "
        //<< num_registers_expected << std::endl
        //<< "Number of registers given = " << registers_.size() << std::endl;
    //return false;
  //}

  //if ((system_type_.compare("arm") == 0) ||
      //(system_type_.compare("arm-eabi") == 0)) {
    //for (RegIterator it = registers_.begin(); it != registers_.end(); it++) {
      //for (int index = 0; index < kARMNumRegs; index++) {
        //std::stringstream reg_name;
        //reg_name << "r" << index;
        //if (strcmp((*it)->GetName(), reg_name.str().c_str()) == 0)
          //arm_regs_[index] = *it;
      //}
      //// try getting alternative naming for pc, sp and lr
      //if (strcmp((*it)->GetName(), "sp") == 0) arm_regs_[13] = *it;
      //if (strcmp((*it)->GetName(), "lr") == 0) arm_regs_[14] = *it;
      //if (strcmp((*it)->GetName(), "pc") == 0) arm_regs_[15] = *it;
    //}
    //bool regs_ok = true;
    //for (int index = 0; regs_ok && (index < kARMNumRegs); index++)
      //regs_ok = arm_regs_ != NULL;
    //if (!regs_ok) {
      //logger_ << "Could not initialize register mapping. "
          //"Missing the following registers:" << std::endl;
      //for (int index = 0; index < kARMNumRegs; index++) {
        //if (arm_regs_[index] == NULL) {
          //logger_ << " - r" << index;
          //if (index == 13) logger_ << " (or sp)";
          //if (index == 14) logger_ << " (or lr)";
          //if (index == 15) logger_ << " (or pc)";
          //logger_ << std::endl;
        //}
        //arm_regs_[index] = NULL;
      //}
      //return false;
    //}
  //}

  //if (system_type_.compare("powerpc")) {
    //for (RegIterator it = registers_.begin(); it != registers_.end(); it++) {
      //for (int index = 0; indx < kPPCNumRegs; index++) {
        //std::stringstream reg_name;
        //reg_name << "r" << index;
        //if (strcmp((*it)->GetName(), reg_name.str().c_str()) == 0)
          //ppc_regs_[index] = *it;
      //}
      //if (strcmp((*it)->GetName(), "cr") == 0) ppc_cr_ = (*it);
      //if (strcmp((*it)->GetName(), "cia") == 0) ppc_cia_ = (*it);
    //}
    //bool regs_ok = (ppc_cr_ != NULL) && (ppc_cia_ != NULL);
    //for (int index = 0; regs_ok && (index < kPPCNumRegs); index++)
      //regs_ok = ppc_regs_ != NULL;
    //if (!regs_ok) {
      //logger_ << "Could not initialize register mapping. "
          //"Missing the following registers:" << std::endl;
      //for (int index = 0; index < kPPCNumRegs; index++) {
        //if (ppc_regs_[index] == NULL)
          //logger_ << " - r" << index << std::endl;
        //ppc_regs_[index] = NULL;
      //}
      //if (ppc_cr_ == NULL) logger_ << " - cr" << std::endl;
      //if (ppc_cia_ == NULL) logger_ << " - cia" << std::endl;
      //ppc_cr_ = ppc_cia_ = NULL;
      //return false;
    //}
  //}

  //return true;
//}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
bool Linux<ADDRESS_TYPE, PARAMETER_TYPE>::LoadFiles(
    unisim::util::debug::blob::Blob<ADDRESS_TYPE> *blob) {
  // Get the main executable blob
  // NOTE: current implementation of the linux os library only supports one
  // blob, the main executable blob
  unisim::util::debug::blob::Blob<ADDRESS_TYPE> const * const main_blob =
      GetMainBlob();

  if (main_blob == NULL) return false;

  // compute the different structure addresses from the given blob
  if (!ComputeStructuralAddresses(*main_blob))
    return false;

  if (!FillBlobWithFileBlob(*main_blob, blob))
    return false;

  return true;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
unisim::util::debug::blob::Blob<ADDRESS_TYPE> const * const Linux<ADDRESS_TYPE,
    PARAMETER_TYPE>:: GetMainBlob() const {
  /* NOTE only one file is supported, so we just get the first one if any */
  typename std::map<std::string,
      unisim::util::debug::blob::Blob<ADDRESS_TYPE> const *>::const_iterator
      it = load_files_.begin();
  if (it == load_files_.end()) return NULL;
  else return it->second;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
bool Linux<ADDRESS_TYPE, PARAMETER_TYPE>::ComputeStructuralAddresses(
    unisim::util::debug::blob::Blob<ADDRESS_TYPE> const &blob) {
  int loaded_segments = 0;
  typedef unisim::util::debug::blob::Segment<ADDRESS_TYPE> Segment;
  typedef std::vector<const Segment *> SegmentVector;
  typedef typename SegmentVector::const_iterator SegmentVectorIterator;
  const SegmentVector &segments = blob.GetSegments();

  // Reset application structure addresses
  entry_point_ = blob.GetEntryPoint();
  load_addr_ = ~((ADDRESS_TYPE)0);
  start_code_ = ~((ADDRESS_TYPE)0);
  end_code_ = 0;
  start_data_ = 0;
  end_data_ = 0;
  elf_stack_ = ~((ADDRESS_TYPE)0);
  elf_brk_ = 0;
  num_segments_ = 0;

  for (SegmentVectorIterator it = segments.begin(); it != segments.end(); it++) {
    typename unisim::util::debug::blob::Segment<ADDRESS_TYPE>::Type type =
        (*it)->GetType();
    // ignore the segment if it is not a loadable
    if (type != unisim::util::debug::blob::Segment<ADDRESS_TYPE>::TY_LOADABLE)
      continue;

    loaded_segments++;

    ADDRESS_TYPE segment_addr = (*it)->GetAddr();
    ADDRESS_TYPE segment_end_addr = segment_addr + (*it)->GetSize();
    if (load_addr_ > segment_addr) {
      // TODO
      // WARNING
      // We are not considering the offset in the elf file. It would be better
      // to set load_addr = segment_addr - segment_offset
      load_addr_ = segment_addr;
    }
    if (segment_addr < start_code_)
      start_code_ = segment_addr;
    if (start_data_ < segment_addr)
      start_data_ = segment_addr;
    if ((*it)->GetAttr() & Segment::SA_X) {
      if (end_code_ < segment_end_addr)
        end_code_ = segment_end_addr;
    }
    if (end_data_ < segment_end_addr)
      end_data_ = segment_end_addr;
    if (segment_end_addr > elf_brk_)
      elf_brk_ = segment_end_addr;
  }

  if (loaded_segments == 0) return false;
  num_segments_ = segments.size();
  return true;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
bool Linux<ADDRESS_TYPE, PARAMETER_TYPE>::FillBlobWithFileBlob(
    unisim::util::debug::blob::Blob<ADDRESS_TYPE> const &file_blob,
    unisim::util::debug::blob::Blob<ADDRESS_TYPE> *blob) {
  typedef unisim::util::debug::blob::Segment<ADDRESS_TYPE> Segment;
  typedef std::vector<const Segment*> SegmentVector;
  typedef typename SegmentVector::const_iterator SegmentVectorIterator;
  typedef unisim::util::debug::blob::Section<ADDRESS_TYPE> Section;
  typedef std::vector<const Section*> SectionVector;
  typedef typename SectionVector::const_iterator SectionVectorIterator;
  const SegmentVector &file_segments = file_blob.GetSegments();
  const SectionVector &file_sections = file_blob.GetSections();

  // copy only segments that have data to be loaded
  for (SegmentVectorIterator it = file_segments.begin();
       it != file_segments.end(); it++) {
    typename unisim::util::debug::blob::Segment<ADDRESS_TYPE>::Type type =
        (*it)->GetType();
    // ignore the segment if it is not a loadable
    if (type != unisim::util::debug::blob::Segment<ADDRESS_TYPE>::TY_LOADABLE)
      continue;

    blob->AddSegment((*it));
  }

  // copy sections that correspond to the copied segments
  const SegmentVector &blob_segments = blob->GetSegments();
  for (SectionVectorIterator it = file_sections.begin();
       it != file_sections.end(); it++) {
    ADDRESS_TYPE section_bottom, section_top;
    (*it)->GetAddrRange(section_bottom, section_top);
    bool found_segment = false;
    for (SegmentVectorIterator bit = blob_segments.begin();
         (!found_segment) && (bit != blob_segments.end());
         ++bit) {
      ADDRESS_TYPE segment_bottom, segment_top;
      (*bit)->GetAddrRange(segment_bottom, segment_top);
      if ((section_bottom >= segment_bottom) && (section_top <= segment_top)) {
        blob->AddSection((*it));
        found_segment = true;
      }
    }
  }

  return true;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
bool Linux<ADDRESS_TYPE, PARAMETER_TYPE>::CreateStack(
    unisim::util::debug::blob::Blob<ADDRESS_TYPE>* blob) const {
  typedef std::vector<ADDRESS_TYPE> AddressVector;
  typedef typename AddressVector::iterator AddressVectorIterator;
  typedef unisim::util::debug::blob::Section<ADDRESS_TYPE> Section;
  typedef unisim::util::debug::blob::Segment<ADDRESS_TYPE> Segment;

  // make sure a blob is being handled
  if (blob == NULL) {
    std::cerr << "ERROR(unisim::util::os::linux_os::Linux.CreateStack): "
        << "no blob handled to method." << std::endl;
    return false;
  }
  // make sure argv has been defined, at least for the application to execute
  if ((argc_ == 0) || (argv_.size() == 0)) {
    std::cerr << "ERROR(unisim::util::os::linux_os::Linux.CreateStack): "
        << "argc and/or size(argv) is/are 0." << std::endl;
    return false;
  }
  // Create the stack
  // TODO: maybe we should check for a bigger stack
  if (stack_size_ == 0) {
    std::cerr << "ERROR(unisim::util::os::linux_os::Linux.CreateStack): "
        << "defined stack size is 0." << std::endl;
        return false;
  }

  uint8_t *stack_data = (uint8_t *)calloc(stack_size_, 1);
  ADDRESS_TYPE sp = stack_size_;
  // Fill the stack
  // - copy the filename
  ADDRESS_TYPE argv0_top = sp;
  sp -= sizeof(ADDRESS_TYPE);
  ADDRESS_TYPE cur_length = argv_[0].length() + 1;
  sp = sp - cur_length;
  // TODO Remove this line? What was this being used for?
  // ADDRESS_TYPE sp_argc = sp;
  memcpy(stack_data + sp, argv_[0].c_str(), cur_length);
  ADDRESS_TYPE argv0_bottom = sp;
  Section* argv0_section = new Section(Section::TY_PROGBITS, Section::SA_AW,
                                       ".unisim.linux_os.stack.argv0", 1, 0,
                                       stack_base_ + argv0_bottom,
                                       argv0_top - argv0_bottom,
                                       stack_data + argv0_bottom);
  argv0_section->Catch();
  // - copy envp
  // variable to keep the stack environment entries addresses
  std::vector<ADDRESS_TYPE> sp_envp;
  ADDRESS_TYPE env_top = sp;
  for (std::vector<std::string>::const_iterator it = target_envp_.begin();
       it != target_envp_.end(); it++) {
    cur_length = (*it).length() + 1;
    sp = sp - cur_length;
    sp_envp.push_back(sp);
    memcpy(stack_data + sp, (*it).c_str(), cur_length);
  }
  ADDRESS_TYPE env_bottom = sp;
  Section* env_section =
      new Section(Section::TY_PROGBITS, Section::SA_AW,
                  ".unisim.linux_os.stack.env", 1, 0, stack_base_ + env_bottom,
                  env_top - env_bottom,stack_data + env_bottom);
  env_section->Catch();

  // - copy argv
  // variable to keep the stack argument entries addresses
  std::vector<ADDRESS_TYPE> sp_argv;
  ADDRESS_TYPE argv_top = sp;
  for (std::vector<std::string>::const_iterator it = argv_.begin();
       it != argv_.end(); it++) {
    cur_length = (*it).length() + 1;
    sp = sp - cur_length;
    sp_argv.push_back(sp);
    memcpy(stack_data + sp, (*it).c_str(), cur_length);
  }
  ADDRESS_TYPE argv_bottom = sp;
  Section* argv_section =
      new Section(Section::TY_PROGBITS, Section::SA_AW,
                  ".unisim.linux_os.stack.argv", 1, 0,
                  stack_base_ + argv_bottom, argv_top - argv_bottom,
                  stack_data + argv_bottom);
  argv_section->Catch();

  // force 16 byte alignment
  sp = sp & ~(ADDRESS_TYPE)0x0f;
  // compute the required stack size for the aux table entries and the pointers
  // to the different environment and argument values
  ADDRESS_TYPE sp_content_size = 0;
  // number of aux table entries
  sp_content_size += num_aux_table_entries_ * 2 * sizeof(ADDRESS_TYPE);
  //   and their values
  sp_content_size += num_aux_table_entries_ * sizeof(ADDRESS_TYPE);
  // number of environment entries
  sp_content_size += target_envp_.size() * sizeof(ADDRESS_TYPE);
  //   and its termination
  sp_content_size += sizeof(ADDRESS_TYPE);
  // number of argument entries (argc)
  sp_content_size += argv_.size() * sizeof(ADDRESS_TYPE);
  //   and its termination
  sp_content_size += sizeof(ADDRESS_TYPE);
  // entry for argc itself
  sp_content_size += sizeof(ADDRESS_TYPE);
  if ( sp_content_size & 0x0f ) // force aligned start
    sp = sp - (16 - (sp_content_size & 0x0f));

  ADDRESS_TYPE aux_table_top = sp;
  SetAuxTable(stack_data, sp);
  ADDRESS_TYPE aux_table_bottom = sp;
  Section* aux_table_section =
      new Section(Section::TY_PROGBITS, Section::SA_AW,
                  ".unisim.linux_os.stack.aux_table", 1, 0,
                  stack_base_ + aux_table_bottom,
                  aux_table_top - aux_table_bottom,
                  stack_data + aux_table_bottom);
  aux_table_section->Catch();

  // Now we have to put the pointers to the different argv and envp
  ADDRESS_TYPE envp_top = sp;
  ADDRESS_TYPE envp_value = 0;
  sp -= sizeof(envp_value);
  memcpy(stack_data + sp, &envp_value, sizeof(envp_value));
  for (AddressVectorIterator it = sp_envp.begin();
       it != sp_envp.end();
       ++it) {
    sp -= sizeof(envp_value);
    envp_value = (*it) + stack_base_;
    memcpy(stack_data + sp, &envp_value, sizeof(envp_value));
  }
  ADDRESS_TYPE envp_bottom = sp;
  Section* envp_section =
      new Section(Section::TY_PROGBITS, Section::SA_AW,
                  ".unisim.linux_os.stack.environment_pointers",
                  1, 0, stack_base_ + envp_bottom,
                  envp_top - envp_bottom,
                  stack_data + envp_bottom);
  envp_section->Catch();

  ADDRESS_TYPE argvp_top = sp;
  ADDRESS_TYPE argvp_value = 0;
  sp -= sizeof(argvp_value);
  memcpy(stack_data + sp, &argvp_value, sizeof(argvp_value));
  for (AddressVectorIterator it = sp_argv.begin();
       it != sp_argv.end();
       ++it) {
    sp -= sizeof(argvp_value);
    argvp_value = (*it) + stack_base_;
    memcpy(stack_data + sp, &argvp_value, sizeof(argvp_value));
  }
  ADDRESS_TYPE argvp_bottom = sp;
  Section* argvp_section =
      new Section(Section::TY_PROGBITS, Section::SA_AW,
                  ".unisim.linux_os.stack.argument_pointers",
                  1, 0, stack_base_ + argvp_bottom,
                  argvp_top - argvp_bottom,
                  stack_data + argvp_bottom);
  argvp_section->Catch();

  // and finally we put argc into the stack
  ADDRESS_TYPE argc_top = sp;
  ADDRESS_TYPE argc_value = argc_;
  sp -= sizeof(argc_value);
  memcpy(stack_data + sp, &argc_value, sizeof(argc_value));
  ADDRESS_TYPE argc_bottom = sp;
  Section* argc_section =
      new Section(Section::TY_PROGBITS, Section::SA_AW,
                  ".unisim.linux_os.stack.argument_counter",
                  1, 0, stack_base_ + argc_bottom,
                  argc_top - argc_bottom,
                  stack_data + argc_bottom);
  argc_section->Catch();

  // create an empty section to keep the stack pointer
  Section* stack_pointer_section =
      new Section(Section::TY_NULL, Section::SA_NULL,
                  ".unisim.linux_os.stack.stack_pointer",
                  0, 0, stack_base_ + sp,
                  0, NULL);
  stack_pointer_section->Catch();

  // create a segment for the stack
  Segment* stack_segment =
      new Segment(Segment::TY_LOADABLE, Segment::SA_RW,
                  4, stack_base_, stack_size_, stack_size_, stack_data);
  stack_segment->Catch();

  // add the stack segment and the different sections to the blob
  blob->AddSegment(stack_segment);
  blob->AddSection(argv0_section);
  blob->AddSection(env_section);
  blob->AddSection(argv_section);
  blob->AddSection(aux_table_section);
  blob->AddSection(envp_section);
  blob->AddSection(argvp_section);
  blob->AddSection(argc_section);
  blob->AddSection(stack_pointer_section);

  stack_pointer_section->Release();
  argc_section->Release();
  argvp_section->Release();
  envp_section->Release();
  aux_table_section->Release();
  argv_section->Release();
  env_section->Release();
  argv0_section->Release();
  stack_segment->Release();

  return true;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetAuxTable(
    uint8_t* stack_data, ADDRESS_TYPE &sp) const {
  ADDRESS_TYPE aux_table_symbol;
  ADDRESS_TYPE aux_table_value;

  aux_table_symbol = AT_NULL;
  aux_table_value = 0;
  sp = SetAuxTableEntry(stack_data, sp, aux_table_symbol, aux_table_value);

  aux_table_symbol = AT_PHDR;
  /* TODO
   * WARNING
   * load_addr does not consider the segments offsets in the elf file
   * (see previous warning).
   * The elf library should provide information on the size of the elf header.
   */
  aux_table_value = load_addr_ + sizeof(Elf32_Ehdr); // size of the elf32 header
  sp = SetAuxTableEntry(stack_data, sp, aux_table_symbol, aux_table_value);

  aux_table_symbol = AT_PHENT;
  /* TODO
   * WARNING
   * The elf library should provide information on the size of the program
   * header.
   */
  aux_table_value = sizeof(Elf32_Phdr); // 32 = size of the program header
  sp = SetAuxTableEntry(stack_data, sp, aux_table_symbol, aux_table_value);

  aux_table_symbol = AT_PHNUM;
  aux_table_value = num_segments_;
  sp = SetAuxTableEntry(stack_data, sp, aux_table_symbol, aux_table_value);

  aux_table_symbol = AT_PAGESZ;
  aux_table_value = memory_page_size_;
  sp = SetAuxTableEntry(stack_data, sp, aux_table_symbol, aux_table_value);

  /* TODO
   * Add support for interpreter.
   */
  aux_table_symbol = AT_BASE;
  aux_table_value = 0; // 0 to signal no interpreter
  sp = SetAuxTableEntry(stack_data, sp, aux_table_symbol, aux_table_value);

  aux_table_symbol = AT_FLAGS;
  aux_table_value = 0;
  sp = SetAuxTableEntry(stack_data, sp, aux_table_symbol, aux_table_value);

  aux_table_symbol = AT_ENTRY;
  aux_table_value = entry_point_;
  sp = SetAuxTableEntry(stack_data, sp, aux_table_symbol, aux_table_value);

  aux_table_symbol = AT_UID;
  aux_table_value = (ADDRESS_TYPE)getuid();
  sp = SetAuxTableEntry(stack_data, sp, aux_table_symbol, aux_table_value);

  aux_table_symbol = AT_EUID;
  aux_table_value = (ADDRESS_TYPE)geteuid();
  sp = SetAuxTableEntry(stack_data, sp, aux_table_symbol, aux_table_value);

  aux_table_symbol = AT_GID;
  aux_table_value = (ADDRESS_TYPE)getgid();
  sp = SetAuxTableEntry(stack_data, sp, aux_table_symbol, aux_table_value);

  aux_table_symbol = AT_EGID;
  aux_table_value = (ADDRESS_TYPE)getegid();
  sp = SetAuxTableEntry(stack_data, sp, aux_table_symbol, aux_table_value);

  /* TODO
   * Adapt for other architectures as PowerPC,...
   */
  aux_table_symbol = AT_HWCAP;
  aux_table_value = ARM_ELF_HWCAP;
  sp = SetAuxTableEntry(stack_data, sp, aux_table_symbol, aux_table_value);

  aux_table_symbol = AT_CLKTCK;
  aux_table_value = (ADDRESS_TYPE)sysconf(_SC_CLK_TCK);
  sp = SetAuxTableEntry(stack_data, sp, aux_table_symbol, aux_table_value);

  /* TODO
   * Enforce required alignment necessary in some architectures (i.e., PowerPC).
   */

  // ??? ADDRESS_TYPE saved_aux = sp;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
ADDRESS_TYPE Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetAuxTableEntry(
    uint8_t * stack_data, ADDRESS_TYPE sp,
    ADDRESS_TYPE entry, ADDRESS_TYPE value) const {
  sp = sp - sizeof(sp);
  uint8_t *addr = stack_data + sp;
  memcpy(addr, &value, sizeof(sp));
  sp = sp - sizeof(sp);
  addr = stack_data + sp;
  memcpy(addr, &entry, sizeof(sp));
  return sp;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
bool Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetSystemBlob(
    unisim::util::debug::blob::Blob<ADDRESS_TYPE> *blob) const {
  if ((system_type_.compare("arm") == 0) ||
      (system_type_.compare("arm-eabi") == 0))
    return SetArmBlob(blob);
  else if (system_type_.compare("ppc") == 0)
    return SetPPCBlob(blob);

  std::cerr << "ERROR(unisim::util::os::linux_os::Linux.SetSystemBlob): "
      << "Unknown system type (" << system_type_ << ")" << std::endl;
  return false;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
bool Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetArmBlob(
    unisim::util::debug::blob::Blob<ADDRESS_TYPE> *blob) const {
  typedef unisim::util::debug::blob::Section<ADDRESS_TYPE> Section;
  typedef unisim::util::debug::blob::Segment<ADDRESS_TYPE> Segment;
  if (utsname_machine_.compare("armv5") == 0) {
    if (blob == NULL) return false;
    // TODO: Check that the program/stack is not in conflict with the
    //   tls and cmpxchg interfaces
    // Set the tls interface, this requires a write into the memory
    //   system
    // The following instructions need to be added to memory:
    // 0xffff0fe0:  e59f0008   ldr r0, [pc, #(16 - 8)] @ TLS stored
    //                                                 @ at 0xffff0ff0
    // 0xffff0fe4:  e1a0f00e   mov pc, lr
    // 0xffff0fe8:  0
    // 0xffff0fec:  0
    // 0xffff0ff0:  0
    // 0xffff0ff4:  0
    // 0xffff0ff8:  0
    ADDRESS_TYPE tls_base_addr = 0xffff0fe0UL;
    static const uint32_t tls_buf_length = 7;
    static const uint32_t tls_buf[tls_buf_length] =
    {0xe59f0008UL, 0xe1a0f00eUL, 0, 0, 0, 0, 0};
    uint32_t *blob_tls_buf = 0;
    blob_tls_buf = (uint32_t *)malloc(sizeof(uint32_t) * tls_buf_length);

    for (unsigned int i = 0; i < tls_buf_length; ++i) {
      if (endianness_ == unisim::util::endian::E_BIG_ENDIAN)
        blob_tls_buf[i] = unisim::util::endian::Host2BigEndian(tls_buf[i]);
      else
        blob_tls_buf[i] = unisim::util::endian::Host2LittleEndian(tls_buf[i]);
    }
    Section *tls_if_section =
        new Section(Section::TY_UNKNOWN, Section::SA_A,
                    ".unisim.linux_os.interface.tls", 4, 0, tls_base_addr,
                    sizeof(tls_buf), blob_tls_buf);
    Segment *tls_if_segment =
        new Segment(Segment::TY_LOADABLE, Segment::SA_X, 4, tls_base_addr,
                    sizeof(tls_buf), sizeof(tls_buf), blob_tls_buf);
    tls_if_section->Catch();
    tls_if_segment->Catch();

    // Set the cmpxchg (atomic compare and exchange) interface, the
    //   following instructions need to be added to memory:
    // 0xffff0fc0:  e5923000   ldr r3, [r2]
    // 0xffff0fc4:  e0533000   subs r3, r3, r0
    // 0xffff0fc8:  05821000   streq r1, [r2]
    // 0xffff0fcc:  e2730000   rsbs r0, r3, #0	; 0x0
    // 0xffff0fd0:  e1a0f00e   mov pc, lr
    ADDRESS_TYPE cmpxchg_base_addr = 0xffff0fc0UL;
    static const uint32_t cmpxchg_buf_length = 5;
    static const uint32_t cmpxchg_buf[cmpxchg_buf_length] = {
      0xe5923000UL,
      0xe0533000UL,
      0x05821000UL,
      0xe2730000UL,
      0xe1a0f00eUL
    };
    uint32_t *blob_cmpxchg_buf = 0;
    blob_cmpxchg_buf = (uint32_t *)malloc(sizeof(uint32_t) *
                                          cmpxchg_buf_length);

    for (unsigned int i = 0; i < cmpxchg_buf_length; ++i) {
      if ( endianness_ == unisim::util::endian::E_BIG_ENDIAN )
        blob_cmpxchg_buf[i] =
            unisim::util::endian::Host2BigEndian(cmpxchg_buf[i]);
      else
        blob_cmpxchg_buf[i] =
            unisim::util::endian::Host2LittleEndian(cmpxchg_buf[i]);
    }
    Section* cmpxchg_if_section =
        new Section(Section::TY_UNKNOWN, Section::SA_A,
                    ".unisim.linux_os.interface.cmpxchg",
                    4, 0, cmpxchg_base_addr,
                    sizeof(cmpxchg_buf), blob_cmpxchg_buf);
    Segment *cmpxchg_if_segment =
        new Segment(Segment::TY_LOADABLE, Segment::SA_X,
                    4, cmpxchg_base_addr,
                    sizeof(cmpxchg_buf), sizeof(cmpxchg_buf), blob_cmpxchg_buf);
    cmpxchg_if_section->Catch();
    cmpxchg_if_segment->Catch();

    blob->AddSegment(tls_if_segment);
    blob->AddSegment(cmpxchg_if_segment);
    blob->AddSection(tls_if_section);
    blob->AddSection(cmpxchg_if_section);

    tls_if_section->Release();
    tls_if_segment->Release();
    cmpxchg_if_section->Release();
    cmpxchg_if_segment->Release();
  }

  return true;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
bool Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetPPCBlob(
    unisim::util::debug::blob::Blob<ADDRESS_TYPE> *blob) const {
  // TODO
  return false;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetSyscallId(
    const std::string &syscall_name, int syscall_id) {
  if(unlikely(verbose_))
    *logger_ << "Associating syscall_name \"" << syscall_name << "\""
        << "to syscall_id number " << syscall_id << std::endl
        << std::endl;
  if (HasSyscall(syscall_name))
  {
    if (HasSyscall(syscall_id))
    {
      std::stringstream s;
      s << __FUNCTION__ << ":" << __FILE__ << ":" << __LINE__ << std::endl;
      s << "syscall_id already associated to syscall \""
          << syscall_name_assoc_map_[syscall_id] << "\"" << std::endl;
      s << "  you wanted to associate it to " << syscall_name << std::endl;
      throw std::runtime_error(s.str().c_str());
    }
    syscall_name_assoc_map_[syscall_id] = syscall_name;
    syscall_impl_assoc_map_[syscall_id] = syscall_name_map_[syscall_name];
  }
  else
  {
    std::stringstream s;
    s << __FUNCTION__ << ":" << __FILE__ << ":" << __LINE__ << std::endl;
    s << "Unimplemented system call (" << syscall_name << ")";
    throw std::runtime_error(s.str().c_str());
  }
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetSyscallNameMap() {
  syscall_name_map_[std::string("unknown")] = &thistype::LSC_unknown;
  syscall_name_map_[std::string("exit")] = &thistype::LSC_exit;
  syscall_name_map_[std::string("read")] = &thistype::LSC_read;
  syscall_name_map_[std::string("write")] = &thistype::LSC_write;
  syscall_name_map_[std::string("open")] = &thistype::LSC_open;
  syscall_name_map_[std::string("close")] = &thistype::LSC_close;
  syscall_name_map_[std::string("lseek")] = &thistype::LSC_lseek;
  syscall_name_map_[std::string("getpid")] = &thistype::LSC_getpid;
  syscall_name_map_[std::string("getuid")] = &thistype::LSC_getuid;
  syscall_name_map_[std::string("access")] = &thistype::LSC_access;
  syscall_name_map_[std::string("times")] = &thistype::LSC_times;
  syscall_name_map_[std::string("brk")] = &thistype::LSC_brk;
  syscall_name_map_[std::string("getgid")] = &thistype::LSC_getgid;
  syscall_name_map_[std::string("geteuid")] = &thistype::LSC_geteuid;
  syscall_name_map_[std::string("getegid")] = &thistype::LSC_getegid;
  syscall_name_map_[std::string("munmap")] = &thistype::LSC_munmap;
  syscall_name_map_[std::string("stat")] = &thistype::LSC_stat;
  syscall_name_map_[std::string("fstat")] = &thistype::LSC_fstat;
  syscall_name_map_[std::string("uname")] = &thistype::LSC_uname;
  syscall_name_map_[std::string("llseek")] = &thistype::LSC_llseek;
  syscall_name_map_[std::string("writev")] = &thistype::LSC_writev;
  syscall_name_map_[std::string("mmap")] = &thistype::LSC_mmap;
  syscall_name_map_[std::string("mmap2")] = &thistype::LSC_mmap2;
  syscall_name_map_[std::string("stat64")] = &thistype::LSC_stat64;
  syscall_name_map_[std::string("fstat64")] = &thistype::LSC_fstat64;
  syscall_name_map_[std::string("getuid32")] = &thistype::LSC_getuid32;
  syscall_name_map_[std::string("getgid32")] = &thistype::LSC_getgid32;
  syscall_name_map_[std::string("geteuid32")] = &thistype::LSC_geteuid32;
  syscall_name_map_[std::string("getegid32")] = &thistype::LSC_getegid32;
  syscall_name_map_[std::string("fcntl64")] = &thistype::LSC_fcntl64;
  syscall_name_map_[std::string("flistxattr")] = &thistype::LSC_flistxattr;
  syscall_name_map_[std::string("exit_group")] = &thistype::LSC_exit_group;
  syscall_name_map_[std::string("fcntl")] = &thistype::LSC_fcntl;
  syscall_name_map_[std::string("dup")] = &thistype::LSC_dup;
  syscall_name_map_[std::string("ioctl")] = &thistype::LSC_ioctl;
  syscall_name_map_[std::string("ugetrlimit")] = &thistype::LSC_ugetrlimit;
  syscall_name_map_[std::string("getrlimit")] = &thistype::LSC_getrlimit;
  syscall_name_map_[std::string("setrlimit")] = &thistype::LSC_setrlimit;
  syscall_name_map_[std::string("rt_sigaction")] = &thistype::LSC_rt_sigaction;
  syscall_name_map_[std::string("getrusage")] = &thistype::LSC_getrusage;
  syscall_name_map_[std::string("unlink")] = &thistype::LSC_unlink;
  syscall_name_map_[std::string("rename")] = &thistype::LSC_rename;
  syscall_name_map_[std::string("time")] = &thistype::LSC_time;
  syscall_name_map_[std::string("socketcall")] = &thistype::LSC_socketcall;
  syscall_name_map_[std::string("rt_sigprocmask")] =
      &thistype::LSC_rt_sigprocmask;
  syscall_name_map_[std::string("kill")] = &thistype::LSC_kill;
  syscall_name_map_[std::string("ftruncate")] = &thistype::LSC_ftruncate;
  // the following are arm private system calls
  if (utsname_machine_.compare("armv5") == 0) {
    syscall_name_map_[std::string("breakpoint")] =
        &thistype::LSC_arm_breakpoint;
    syscall_name_map_[std::string("cacheflush")] =
        &thistype::LSC_arm_cacheflush;
    syscall_name_map_[std::string("usr26")] = &thistype::LSC_arm_usr26;
    syscall_name_map_[std::string("usr32")] = &thistype::LSC_arm_usr32;
    syscall_name_map_[std::string("set_tls")] = &thistype::LSC_arm_set_tls;
  }
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
bool Linux<ADDRESS_TYPE, PARAMETER_TYPE>::HasSyscall(
    const std::string &syscall_name) {
  return syscall_name_map_.find(syscall_name) != syscall_name_map_.end();
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
bool Linux<ADDRESS_TYPE, PARAMETER_TYPE>::HasSyscall(int syscall_id) {
  return syscall_impl_assoc_map_.find(syscall_id) != 
      syscall_impl_assoc_map_.end();
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
int Linux<ADDRESS_TYPE, PARAMETER_TYPE>::GetSyscallNumber(int id) {
  if ( system_type_.compare("arm") == 0 )
    return ARMGetSyscallNumber(id);
  else if ( system_type_.compare("arm-eabi") == 0 )
    return ARMEABIGetSyscallNumber(id);
  else if ( system_type_.compare("powerpc") == 0 )
    return PPCGetSyscallNumber(id);
  std::cerr
      << "ERROR(unisim::util::os::linux): Could not translate syscall number "
      << id << " (" << std::hex << id << std::dec << ") for system \""
      << system_type_ << "\". Returning untranslated id."
      << std::endl;
  return id;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
int Linux<ADDRESS_TYPE, PARAMETER_TYPE>::ARMGetSyscallNumber(int id) {
  int translated_id = id - 0x0900000;
  return translated_id;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
int Linux<ADDRESS_TYPE, PARAMETER_TYPE>::ARMEABIGetSyscallNumber(int id) {
  // the arm eabi ignores the given id and uses register 7
  //   as the id and translated id
  uint32_t translated_id = 0;
  if (register_interface_ == NULL) 
    // TODO Add warning
    return 0;

  if (!register_interface_->GetRegister(kARMEABISyscallNumberReg,
                                        &translated_id))
    return (int)translated_id;
  return 0;
  //arm_regs[7]->GetValue(&translated_id);
  //return (int)translated_id;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
int Linux<ADDRESS_TYPE, PARAMETER_TYPE>::PPCGetSyscallNumber(int id) {
  return id;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
bool Linux<ADDRESS_TYPE, PARAMETER_TYPE>::ReadMem(
    ADDRESS_TYPE addr, uint8_t * buffer, uint32_t size) {
  if (memory_interface_ != NULL) return false;
  return memory_interface_->ReadMemory(addr, buffer, size);
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
bool Linux<ADDRESS_TYPE, PARAMETER_TYPE>::WriteMem(
    ADDRESS_TYPE addr, uint8_t const * const buffer, uint32_t size) {
  if (memory_interface_ == NULL) return false;
  return memory_interface_->WriteMemory(addr, buffer, size);
}

} // end of namespace linux
} // end of namespace os
} // end of namespace util
} // end of namespace unisim

#endif // __UNISIM_UTIL_OS_LINUX_LINUX_TCC__
