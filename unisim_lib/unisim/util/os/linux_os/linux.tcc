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
 *          Gilles Mouchard (gilles.mouchard@cea.fr)
 */

#ifndef __UNISIM_UTIL_OS_LINUX_LINUX_TCC__
#define __UNISIM_UTIL_OS_LINUX_LINUX_TCC__

#if !defined(linux) && !defined(__APPLE_CC__) && !defined(WIN32) && !defined(WIN64)
#error "Unsupported host machine for Linux system call translation !"
#endif

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "unisim/util/likely/likely.hh"
#include "unisim/util/debug/blob/blob.hh"
#include "unisim/util/endian/endian.hh"
#include "unisim/util/loader/elf_loader/elf32_loader.hh"

#include "unisim/util/os/linux_os/arm.hh"
#include "unisim/util/os/linux_os/ppc.hh"
#include "unisim/util/os/linux_os/environment.hh"
#include "unisim/util/os/linux_os/aux_table.hh"

namespace unisim {
namespace util {
namespace os {
namespace linux_os {

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
const int Linux<ADDRESS_TYPE, PARAMETER_TYPE>::kNumSupportedSystemTypes = 3;

const char * tmp_supported_system_types[] = {"arm", "arm-eabi", "ppc"};
template <class ADDRESS_TYPE, class PARAMETER_TYPE>
const std::vector<std::string> Linux<ADDRESS_TYPE, PARAMETER_TYPE>::
supported_system_types_(tmp_supported_system_types, 
                        tmp_supported_system_types + 
                        sizeof(tmp_supported_system_types) / sizeof(char *));

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
Linux<ADDRESS_TYPE, PARAMETER_TYPE>::
Linux(unisim::kernel::logger::Logger& logger,
  unisim::service::interfaces::Registers *regs_if, unisim::service::interfaces::Memory<ADDRESS_TYPE> *mem_if,
  unisim::service::interfaces::MemoryInjection<ADDRESS_TYPE> *mem_inject_if)
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
    , blob_(NULL)
    , regs_if_(regs_if)
    , mem_if_(mem_if)
    , mem_inject_if_(mem_inject_if)
    , syscall_name_map_()
    , syscall_name_assoc_map_()
    , syscall_impl_assoc_map_()
    , current_syscall_id_(0)
    , current_syscall_name_("(NONE)")
    , verbose_(false)
    , parse_dwarf_(false)
	, debug_dwarf_(false)
    , dwarf_to_html_output_directory_()
    , dwarf_to_xml_output_filename_()
    , logger_(logger)
    , terminated_(false)
    , return_status_(0) {
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
Linux<ADDRESS_TYPE, PARAMETER_TYPE>::~Linux() {
  typename std::map<std::string, unisim::util::debug::blob::Blob<ADDRESS_TYPE> const *>::const_iterator it;
  for(it = load_files_.begin(); it != load_files_.end(); it++)
  {
     const unisim::util::debug::blob::Blob<ADDRESS_TYPE> *blob = (*it).second;
     blob->Release();
  }
  load_files_.clear();
  if(blob_)
  {
     blob_->Release();
     blob_ = 0;
  }
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetVerbose(bool verbose) {
  verbose_ = verbose;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetParseDWARF(bool parse_dwarf) {
  parse_dwarf_ = parse_dwarf;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetDebugDWARF(bool debug_dwarf) {
  debug_dwarf_ = debug_dwarf;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetDWARFToHTMLOutputDirectory(const char *dwarf_to_html_output_directory)
{
	dwarf_to_html_output_directory_ = dwarf_to_html_output_directory;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetDWARFToXMLOutputFilename(const char *dwarf_to_xml_output_filename)
{
	dwarf_to_xml_output_filename_ = dwarf_to_xml_output_filename;
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
    logger_ << DebugWarning << "calling AddLoadFile "
        << "without handling file." << EndDebugWarning;
    return false;
  }

  // check that the file exists and that the elf loader can create a blob from it
  unisim::util::loader::elf_loader::Elf32Loader<ADDRESS_TYPE> *loader =
      new unisim::util::loader::elf_loader::Elf32Loader<ADDRESS_TYPE>(
          logger_, regs_if_, mem_if_);

  if (loader == NULL) {
    logger_ << DebugError
        << "Could not create an elf loader."
        << EndDebugError;
    return false;
  }

  loader->SetOption(unisim::util::loader::elf_loader::OPT_VERBOSE, verbose_);
  loader->SetOption(unisim::util::loader::elf_loader::OPT_FILENAME, filename);
  loader->SetOption(unisim::util::loader::elf_loader::OPT_PARSE_DWARF, parse_dwarf_);
  loader->SetOption(unisim::util::loader::elf_loader::OPT_DEBUG_DWARF, debug_dwarf_);
  loader->SetOption(unisim::util::loader::elf_loader::OPT_DWARF_TO_HTML_OUTPUT_DIRECTORY, dwarf_to_html_output_directory_.c_str());
  loader->SetOption(unisim::util::loader::elf_loader::OPT_DWARF_TO_XML_OUTPUT_FILENAME, dwarf_to_xml_output_filename_.c_str());

  if (!loader->Load()) {
    logger_ << DebugError
        << "Could not load the given file "
        << "\"" << filename << "\"" << EndDebugError;
    delete loader;
    return false;
  }

  unisim::util::debug::blob::Blob<ADDRESS_TYPE> const * const blob =
      loader->GetBlob();
  if (blob == NULL) {
    logger_ << DebugError << "Could not create blob from"
        << " requested file (" << filename << ")." << EndDebugError;
    delete loader;
    return false;
  }

  blob->Catch();
  std::string filename_str(filename);
  if(load_files_.find(filename_str) != load_files_.end()) load_files_[filename_str]->Release();
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
  endianness_ = endianness;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetStackBase(
    ADDRESS_TYPE stack_base) {
  stack_base_ = stack_base;
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

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
unisim::util::debug::Register * Linux<ADDRESS_TYPE, PARAMETER_TYPE>::GetRegisterFromId(uint32_t id) {
  if (!regs_if_) return NULL;
  char const * reg_name = 0;
  if ((system_type_.compare("arm") == 0) || (system_type_.compare("arm-eabi") == 0)) {
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
  } else if (system_type_.compare("ppc") == 0) {
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
  return regs_if_->GetRegister(reg_name);
}


template <class ADDRESS_TYPE, class PARAMETER_TYPE>
bool Linux<ADDRESS_TYPE, PARAMETER_TYPE>::GetRegister(uint32_t id,
    PARAMETER_TYPE * const value) {
  unisim::util::debug::Register *reg = 0;
  reg = GetRegisterFromId(id);
  if (reg == NULL) return false;
  if (reg->GetSize() != sizeof(PARAMETER_TYPE)) return false;
  reg->GetValue(value);
  return true;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
bool Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetRegister(uint32_t id,
    PARAMETER_TYPE value) {
  unisim::util::debug::Register *reg = 0;
  reg = GetRegisterFromId(id);
  if (reg == NULL) return false;
  if (reg->GetSize() != sizeof(PARAMETER_TYPE)) return false;
  reg->SetValue(&value);
  return true;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
bool Linux<ADDRESS_TYPE, PARAMETER_TYPE>::Load() {
  // set the environment to be used
  if (apply_host_environnement_) GetHostEnvironment(envp_, &target_envp_);
  else target_envp_ = envp_;

  // create the root blob that will be used to store the image that will be
  // loaded
  unisim::util::debug::blob::Blob<ADDRESS_TYPE>* blob =
      new unisim::util::debug::blob::Blob<ADDRESS_TYPE>();
  blob->Catch();
  blob->SetFileFormat(unisim::util::debug::blob::FFMT_ELF32);
  
  // load and add files to the blob
  if (verbose_)
    logger_ << DebugInfo
        << "Loading elf files." << EndDebugInfo;
  if (!LoadFiles(blob)) {
    logger_ << DebugError
        << "Could not load elf files." << EndDebugError;
    blob->Release();
    return false;
  }

  // create the stack footprint and add it to the blob
  uint64_t stack_size = 0;
  if (verbose_)
    logger_ << DebugInfo
        << "Creating the Linux software stack." << EndDebugInfo;
  if (!CreateStack(blob, stack_size)) {
    // TODO
    // Remove non finished state (i.e., unfinished blob, reset values, ...)
    logger_ << DebugError
        << "Could not create the Linux software stack." << EndDebugError;
    blob->Release();
    return false;
  }

  // finish the state of the memory image depending on the system we are running
  // on
  if (verbose_)
    logger_ << DebugInfo
        << "Setting the system blob." << EndDebugInfo;
  if (!SetSystemBlob(blob)) {
    // TODO
    // Remove non finished state (i.e., unfinished blob, reset values, ...)
    logger_ << DebugError
        << "Could not set the system blob." << EndDebugError;
    blob->Release();
    return false;
  }

  // Set the linux calls maps
  syscall_impl_assoc_map_.clear();
  SetSyscallNameMap();
  if ((system_type_.compare("arm") == 0) ||
      (system_type_.compare("arm-eabi") == 0)) {
    if (!SetupLinuxOSARM()) {
      logger_ << DebugError
          << "Error while trying to setup the linux os arm"
          << EndDebugError;
      blob->Release();
      return false;
    }
  }
  if (system_type_.compare("ppc") == 0) {
    if (!SetupLinuxOSPPC()) {
      logger_ << DebugError
          << "Error while trying to setup the linux os ppc"
          << EndDebugError;
      blob->Release();
      return false;
    }
  }

  // Set mmap_brk_point and brk_point
  mmap_brk_point_ = mmap_base_;

  ADDRESS_TYPE top_addr = stack_base_ + stack_size - 1;
  if(verbose_) {
    logger_ << DebugInfo << "=== top_addr = 0x" << std::hex << top_addr << std::dec
      << EndDebugInfo;
  }

  brk_point_ = top_addr +
      (memory_page_size_ - (top_addr % memory_page_size_));

  if(verbose_) {
    logger_ << DebugInfo
        << "=== stack_size_ = 0x" << std::hex << stack_size << " (" << std::dec << stack_size << ")" << std::endl
        << "=== brk_point_ = 0x" << std::hex << brk_point_ << std::endl
        << "=== mmap_brk_point_ = 0x" << mmap_brk_point_ << std::endl
        << "=== mmap_base_ = 0x" << mmap_base_ << std::endl
        << "=== memory_page_size_ = 0x" << memory_page_size_ << " ("
        << std::dec << memory_page_size_ << ")" << EndDebugInfo;
  }

  if(blob_) blob_->Release();
  blob_ = blob;
  is_load_ = true;

  return true;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
bool Linux<ADDRESS_TYPE, PARAMETER_TYPE>::IsLoad() {
  return blob_ != NULL;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
bool Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetupTarget() {
  if (mem_if_ == NULL
      || mem_inject_if_ == NULL
      || regs_if_ == NULL) {
    logger_ << DebugError
        << "The linux system interfaces (memory/register) were not"
        << " assigned" << EndDebugError;
    return false;
  }

  if (blob_ == NULL) {
    logger_ << DebugError
        << "The linux system was not loaded." << EndDebugError;
    return false;
  }

  if ((system_type_.compare("arm") == 0) || 
      (system_type_.compare("arm-eabi") == 0))
    return SetupARMTarget();
  else if (system_type_.compare("ppc") == 0)
    return SetupPPCTarget();

  logger_ << DebugError
      << "Unknown system to setup (" << system_type_ << ")." << EndDebugError;
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
      uint8_t const * data = (uint8_t const *)(*it)->GetData();
      ADDRESS_TYPE start, end;
      (*it)->GetAddrRange(start, end);

      if(verbose_) {
        logger_ << DebugInfo << "--> writing memory segment start = 0x"
            << std::hex << start << " end = 0x" << end << std::dec
            << EndDebugInfo;
      }

      success = mem_if_->WriteMemory(start, data, end - start + 1);
    }
  }

  if (!success) {
    logger_ << DebugError
        << "Error while writing the segments into the target memory."
        << EndDebugError;
    return false;
  }

  // Reset all the target registers
  unsigned int reg_id = 0;
  PARAMETER_TYPE null_param = 0;
  for (reg_id = 0; success && (reg_id < kARMNumRegs); reg_id++) {
    success = SetRegister(reg_id, null_param);
  }
  if (!success) {
    logger_ << DebugError
        << "Error while setting register '" << (reg_id - 1) << "'" << EndDebugError;
    return false;
  }
  // Set PC to the program entry point
  success = SetRegister(kARM_pc, entry_point_);
  if (!success) {
    logger_ << DebugError
        << "Error while setting pc register (" << kARM_pc << ")" << EndDebugError;
    return false;
  }
  // Set SP to the base of the created stack
  unisim::util::debug::blob::Section<ADDRESS_TYPE> const * sp_section =
      blob_->FindSection(".unisim.linux_os.stack.stack_pointer");
  if (sp_section == NULL) {
    logger_ << DebugError
        << "Could not find the stack pointer section." << EndDebugError;
    return false;
  }
  success = SetRegister(kARM_sp, sp_section->GetAddr());
  if (!success) {
    logger_ << DebugError
        << "Error while setting sp register (" << kARM_sp << ")" << EndDebugError;
    return false;
  }
  ADDRESS_TYPE par1_addr = sp_section->GetAddr() + 4;
  ADDRESS_TYPE par2_addr = sp_section->GetAddr() + 8;
  PARAMETER_TYPE par1 = 0;
  PARAMETER_TYPE par2 = 0;
  success = mem_if_->ReadMemory(par1_addr, (uint8_t *)&par1, sizeof(par1)); 
  success = mem_if_->ReadMemory(par2_addr, (uint8_t *)&par2, sizeof(par2));
  SetRegister(kARM_r1, Target2Host(endianness_, par1));
  SetRegister(kARM_r2, Target2Host(endianness_, par2));


  return true;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
bool Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetupPPCTarget() {
  typedef unisim::util::debug::blob::Segment<ADDRESS_TYPE> Segment;
  typedef std::vector<const Segment*> SegmentVector;
  typedef typename SegmentVector::const_iterator SegmentVectorIterator;

  bool success = true;
  SegmentVector segments = blob_->GetSegments();
  for (SegmentVectorIterator it = segments.begin();
       success && (it != segments.end()); it++) {
    if ((*it)->GetType() == Segment::TY_LOADABLE) {
      uint8_t const * data = (uint8_t const *)(*it)->GetData();
      ADDRESS_TYPE start, end;
      (*it)->GetAddrRange(start, end);

      if(verbose_) {
         logger_ << DebugInfo << "--> writing memory segment start = 0x"
            << std::hex << start << " end = 0x" << end << std::dec
            << EndDebugInfo;
      }

      success = mem_if_->WriteMemory(start, data, end - start + 1);
    }
  }

  if (!success) {
    logger_ << DebugError
        << "Error while writing the segments into the target memory."
        << EndDebugError;
    return false;
  }

  // Reset all the target registers
  unsigned int reg_id = 0;
  PARAMETER_TYPE null_param = 0;
  for (reg_id = 0; success && (reg_id < kPPCNumRegs); reg_id++) {
    success = SetRegister(reg_id, null_param);
  }
  if (!success) {
    logger_ << DebugError
        << "Error while setting register '" << (reg_id - 1) << "'" << EndDebugError;
    return false;
  }
  // Set PC to the program entry point
  success = SetRegister(kPPC_cia, entry_point_);
  if (!success) {
    logger_ << DebugError
        << "Error while setting cia register (" << kPPC_cia << ")" << EndDebugError;
    return false;
  }
  // Set SP to the base of the created stack
  unisim::util::debug::blob::Section<ADDRESS_TYPE> const * sp_section =
      blob_->FindSection(".unisim.linux_os.stack.stack_pointer");
  if (sp_section == NULL) {
    logger_ << DebugError
        << "Could not find the stack pointer section." << EndDebugError;
    return false;
  }
  success = SetRegister(kPPC_sp, sp_section->GetAddr());
  if (!success) {
    logger_ << DebugError
        << "Error while setting sp register (" << kPPC_sp << ")" << DebugError;
    return false;
  }
  ADDRESS_TYPE par1_addr = sp_section->GetAddr() + 4;
  ADDRESS_TYPE par2_addr = sp_section->GetAddr() + 8;
  PARAMETER_TYPE par1 = 0;
  PARAMETER_TYPE par2 = 0;
  success = mem_if_->ReadMemory(par1_addr, (uint8_t *)&par1, sizeof(par1)); 
  success = mem_if_->ReadMemory(par2_addr, (uint8_t *)&par2, sizeof(par2));
  SetRegister(kPPC_r3, Target2Host(endianness_, par1));
  SetRegister(kPPC_r4, Target2Host(endianness_, par2));

  return true;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
unisim::util::debug::blob::Blob<ADDRESS_TYPE> const * const Linux<ADDRESS_TYPE,
    PARAMETER_TYPE>::GetBlob() const { return blob_; }

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::ExecuteSystemCall(int id, bool& terminated, int& return_status) {
  if (!is_load_) {
    logger_
        << DebugError
        << "unisim::util::os::linux_os::Linux.ExecuteSystemCall: "
        << "Trying to execute system call with id " << id << " while the linux "
        << "emulation has not been correctly loaded."
        << EndDebugError;
    return;
  }

  int translated_id = GetSyscallNumber(id);

  if (HasSyscall(translated_id)) {
    current_syscall_id_ = translated_id;
    current_syscall_name_ = syscall_name_assoc_map_[translated_id];
    if (unlikely(verbose_))
      logger_
          << DebugInfo
          << "Executing syscall(name = " << current_syscall_name_ << ";"
          << " id = " << translated_id << ";"
          << " unstranslated id = " << id << ")"
          << EndDebugInfo;
    syscall_t y = syscall_impl_assoc_map_[translated_id];
    (this->*y)();
  } else {
    logger_
        << DebugInfo
        << "Could not find system call id " << id << " (translated id = "
        << translated_id << "), aborting system call "
        << "execution."
        << EndDebugInfo;
  }
  if(terminated_)
  {
    terminated = true;
    return_status = return_status_;
  }
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
bool Linux<ADDRESS_TYPE, PARAMETER_TYPE>::LoadFiles(
    unisim::util::debug::blob::Blob<ADDRESS_TYPE> *blob) {
  // Get the main executable blob
  // NOTE: current implementation of the linux os library only supports one
  // blob, the main executable blob
  unisim::util::debug::blob::Blob<ADDRESS_TYPE> const * const main_blob =
      GetMainBlob();

  if (main_blob == NULL) return false;

  if(main_blob->GetCapability() & unisim::util::debug::blob::CAP_FILENAME) {
    blob->SetFilename(main_blob->GetFilename());
  }
  if(main_blob->GetCapability() & unisim::util::debug::blob::CAP_ENTRY_POINT) {
    blob->SetEntryPoint(main_blob->GetEntryPoint());
  }
  if(main_blob->GetCapability() & unisim::util::debug::blob::CAP_ARCHITECTURE) {
    blob->SetArchitecture(main_blob->GetArchitecture());
  }
  if(main_blob->GetCapability() & unisim::util::debug::blob::CAP_STACK_BASE) {
    blob->SetStackBase(main_blob->GetStackBase());
  }
  if(main_blob->GetCapability() & unisim::util::debug::blob::CAP_ENDIAN) {
    blob->SetEndian(main_blob->GetEndian());
  }
  if(main_blob->GetCapability() & unisim::util::debug::blob::CAP_FILE_ENDIAN) {
    blob->SetFileEndian(main_blob->GetFileEndian());
  }
  if(main_blob->GetCapability() & unisim::util::debug::blob::CAP_ADDRESS_SIZE) {
    blob->SetAddressSize(main_blob->GetAddressSize());
  }
  if(main_blob->GetCapability() & unisim::util::debug::blob::CAP_ELF_PHOFF) {
    blob->SetELF_PHOFF(main_blob->GetELF_PHOFF());
  }
  if(main_blob->GetCapability() & unisim::util::debug::blob::CAP_ELF_PHENT) {
    blob->SetELF_PHENT(main_blob->GetELF_PHENT());
  }

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

  // copy all the sections
  for (SectionVectorIterator it = file_sections.begin();
       it != file_sections.end(); it++) {
    blob->AddSection((*it));
  }
  return true;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
bool Linux<ADDRESS_TYPE, PARAMETER_TYPE>::CreateStack(
    unisim::util::debug::blob::Blob<ADDRESS_TYPE>* blob, uint64_t& stack_size) const {
  typedef std::vector<ADDRESS_TYPE> AddressVector;
  typedef typename AddressVector::iterator AddressVectorIterator;
  typedef typename AddressVector::reverse_iterator AddressVectorReverseIterator;
  typedef unisim::util::debug::blob::Section<ADDRESS_TYPE> Section;
  typedef unisim::util::debug::blob::Segment<ADDRESS_TYPE> Segment;

  // make sure a blob is being handled
  if (blob == NULL) {
    logger_ << DebugError
        << "no blob handled to method." << EndDebugError;
    return false;
  }
  // make sure argv has been defined, at least for the application to execute
  if ((argc_ == 0) || (argv_.size() == 0)) {
    logger_ << DebugError
        << "argc and/or size(argv) is/are 0." << EndDebugError;
    return false;
  }
  
  ADDRESS_TYPE aux_table_size = 0;
  SetAuxTable(0, aux_table_size);
  aux_table_size = -aux_table_size;
  
  int pass;
  stack_size = 0;
  Section* env_section = 0;
  Section* argv_section = 0;
  Section* aux_table_section = 0;
  Section* envp_section = 0;
  Section* argvp_section = 0;
  Section* argc_section = 0;
  Section* stack_pointer_section = 0;
  uint8_t *stack_data = 0;
  // First pass for computing the size of the initialized stack
  // Second pass for actually initializing the stack
  for(pass = 1; pass <= 2; pass++)
  {
    // Create the stack
    ADDRESS_TYPE sp = (pass == 2) ? stack_size : 0;
    
    stack_data = (pass == 2) ? (uint8_t *) calloc(stack_size, 1) : 0;
    
    // Fill the stack
    ADDRESS_TYPE cur_length;
    // - copy envp
    // variable to keep the stack environment entries addresses
    std::vector<ADDRESS_TYPE> sp_envp;
    ADDRESS_TYPE env_top = sp;
    for (std::vector<std::string>::const_iterator it = target_envp_.begin();
        it != target_envp_.end(); it++) {
      cur_length = (*it).length() + 1;
      sp = sp - cur_length;
      sp_envp.push_back(sp);
      if(pass == 2) memcpy(stack_data + sp, (*it).c_str(), cur_length);
    }
    ADDRESS_TYPE env_bottom = sp;
    if(pass == 2) {
      uint8_t *env_section_data = (uint8_t *)malloc(env_top - env_bottom);
      memcpy(env_section_data, stack_data + env_bottom, env_top - env_bottom);
      env_section =
          new Section(Section::TY_PROGBITS, Section::SA_AW,
                      ".unisim.linux_os.stack.env", 1, 0, stack_base_ + env_bottom,
                      env_top - env_bottom,env_section_data);
    }
    // - copy argv
    // variable to keep the stack argument entries addresses
    std::vector<ADDRESS_TYPE> sp_argv;
    ADDRESS_TYPE argv_top = sp;
    for (std::vector<std::string>::const_iterator it = argv_.begin();
        it != argv_.end(); it++) {
      cur_length = (*it).length() + 1;
      sp = sp - cur_length;
      sp_argv.push_back(sp);
      if(pass == 2) memcpy(stack_data + sp, (*it).c_str(), cur_length);
    }
    ADDRESS_TYPE argv_bottom = sp;
    if(pass == 2) {
      uint8_t *argv_section_data = (uint8_t *)malloc(argv_top - argv_bottom);
      memcpy(argv_section_data, stack_data + argv_bottom, argv_top - argv_bottom);
      argv_section =
          new Section(Section::TY_PROGBITS, Section::SA_AW,
                      ".unisim.linux_os.stack.argv", 1, 0,
                      stack_base_ + argv_bottom, argv_top - argv_bottom,
                      argv_section_data);
    }
    // force 16 byte alignment
    sp = sp & ~(ADDRESS_TYPE)0x0f;
    // compute the required stack size for the aux table entries and the pointers
    // to the different environment and argument values
    ADDRESS_TYPE sp_content_size = 0;
    // aux table including AT_NULL entry
    sp_content_size += aux_table_size;
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
    SetAuxTable((pass == 2) ? stack_data : 0, sp);
    ADDRESS_TYPE aux_table_bottom = sp;
    if(pass == 2) {
      uint8_t *aux_table_section_data = (uint8_t *)malloc(aux_table_top - aux_table_bottom);
      memcpy(aux_table_section_data, stack_data + aux_table_bottom, aux_table_top - aux_table_bottom);
      aux_table_section =
          new Section(Section::TY_PROGBITS, Section::SA_AW,
                      ".unisim.linux_os.stack.aux_table", 1, 0,
                      stack_base_ + aux_table_bottom,
                      aux_table_top - aux_table_bottom,
                      aux_table_section_data);
    }
    // Now we have to put the pointers to the different argv and envp
    ADDRESS_TYPE envp_top = sp;
    ADDRESS_TYPE envp_value = 0;
    sp -= sizeof(envp_value);
    if(pass == 2) memcpy(stack_data + sp, &envp_value, sizeof(envp_value));
    for (AddressVectorIterator it = sp_envp.begin();
        it != sp_envp.end();
        ++it) {
      sp -= sizeof(envp_value);
      envp_value = Host2Target(endianness_, (*it) + stack_base_);
      if(pass == 2) memcpy(stack_data + sp, &envp_value, sizeof(envp_value));
    }
    ADDRESS_TYPE envp_bottom = sp;
    if(pass == 2) {
      uint8_t *envp_section_data = (uint8_t *)malloc(envp_top - envp_bottom);
      memcpy(envp_section_data, stack_data + envp_bottom, envp_top - envp_bottom);
      envp_section =
          new Section(Section::TY_PROGBITS, Section::SA_AW,
                      ".unisim.linux_os.stack.environment_pointers",
                      1, 0, stack_base_ + envp_bottom,
                      envp_top - envp_bottom,
                      envp_section_data);
    }
    
    ADDRESS_TYPE argvp_top = sp;
    ADDRESS_TYPE argvp_value = 0;
    sp -= sizeof(argvp_value);
    if(pass == 2) memcpy(stack_data + sp, &argvp_value, sizeof(argvp_value));
    for (AddressVectorReverseIterator it = sp_argv.rbegin();
        it != sp_argv.rend();
        ++it) {
      sp -= sizeof(argvp_value);
      argvp_value = Host2Target(endianness_, (*it) + stack_base_);
      if(pass == 2) memcpy(stack_data + sp, &argvp_value, sizeof(argvp_value));
    }
    ADDRESS_TYPE argvp_bottom = sp;
    if(pass == 2) {
      uint8_t *argvp_section_data = (uint8_t *)malloc(argvp_top - argvp_bottom);
      memcpy(argvp_section_data, stack_data + argvp_bottom, argvp_top - argvp_bottom);
      argvp_section =
          new Section(Section::TY_PROGBITS, Section::SA_AW,
                      ".unisim.linux_os.stack.argument_pointers",
                      1, 0, stack_base_ + argvp_bottom,
                      argvp_top - argvp_bottom,
                      argvp_section_data);
    }
    // and finally we put argc into the stack
    ADDRESS_TYPE argc_top = sp;
    ADDRESS_TYPE argc_value = Host2Target(endianness_, argc_);
    sp -= sizeof(argc_value);
    if(pass == 2) memcpy(stack_data + sp, &argc_value, sizeof(argc_value));
    ADDRESS_TYPE argc_bottom = sp;
    if(pass == 2) {
      uint8_t *argc_section_data = (uint8_t *)malloc(argc_top - argc_bottom);
      memcpy(argc_section_data, stack_data + argc_bottom, argc_top - argc_bottom);
      argc_section =
          new Section(Section::TY_PROGBITS, Section::SA_AW,
                      ".unisim.linux_os.stack.argument_counter",
                      1, 0, stack_base_ + argc_bottom,
                      argc_top - argc_bottom,
                      argc_section_data);
    }
    // create an empty section to keep the stack pointer
    if(pass == 2) {
      stack_pointer_section =
          new Section(Section::TY_NULL, Section::SA_NULL,
                      ".unisim.linux_os.stack.stack_pointer",
                      0, 0, stack_base_ + sp,
                      0, NULL);
    }
    
    if(pass == 1) stack_size = -sp;
  }
  
  // create a segment for the stack
  Segment* stack_segment =
      new Segment(Segment::TY_LOADABLE, Segment::SA_RW,
                  4, stack_base_, stack_size, stack_size, stack_data);

  // add the stack segment and the different sections to the blob
  blob->AddSegment(stack_segment);
  blob->AddSection(env_section);
  blob->AddSection(argv_section);
  blob->AddSection(aux_table_section);
  blob->AddSection(envp_section);
  blob->AddSection(argvp_section);
  blob->AddSection(argc_section);
  blob->AddSection(stack_pointer_section);

  return true;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetAuxTable(
    uint8_t* stack_data, ADDRESS_TYPE &sp) const {
  ADDRESS_TYPE aux_table_symbol;
  ADDRESS_TYPE aux_table_value;
  unisim::util::debug::blob::Blob<ADDRESS_TYPE> const * const main_blob =
      GetMainBlob();

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
  
  aux_table_value = load_addr_ + main_blob->GetELF_PHOFF(); // start address of ELF program headers
  sp = SetAuxTableEntry(stack_data, sp, aux_table_symbol, aux_table_value);

  aux_table_symbol = AT_PHENT;
  /* TODO
   * WARNING
   * The elf library should provide information on the size of the program
   * header.
   */
  aux_table_value = main_blob->GetELF_PHENT(); // size of the program header
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
#ifdef WIN32
  aux_table_value = (ADDRESS_TYPE) 1000;
#else
  aux_table_value = (ADDRESS_TYPE)getuid();
#endif
  sp = SetAuxTableEntry(stack_data, sp, aux_table_symbol, aux_table_value);

  aux_table_symbol = AT_EUID;
#ifdef WIN32
  aux_table_value = (ADDRESS_TYPE) 1000;
#else
  aux_table_value = (ADDRESS_TYPE)geteuid();
#endif
  sp = SetAuxTableEntry(stack_data, sp, aux_table_symbol, aux_table_value);

  aux_table_symbol = AT_GID;
#ifdef WIN32
  aux_table_value = (ADDRESS_TYPE) 1000;
#else
  aux_table_value = (ADDRESS_TYPE)getgid();
#endif
  sp = SetAuxTableEntry(stack_data, sp, aux_table_symbol, aux_table_value);

  aux_table_symbol = AT_EGID;
#ifdef WIN32
  aux_table_value = (ADDRESS_TYPE) 1000;
#else
  aux_table_value = (ADDRESS_TYPE)getegid();
#endif
  sp = SetAuxTableEntry(stack_data, sp, aux_table_symbol, aux_table_value);

  if ((system_type_.compare("arm") == 0) ||
        (system_type_.compare("arm-eabi") == 0)) {
    aux_table_symbol = AT_HWCAP;
    aux_table_value = ARM_ELF_HWCAP;
    sp = SetAuxTableEntry(stack_data, sp, aux_table_symbol, aux_table_value);
  }

  aux_table_symbol = AT_CLKTCK;
#ifdef WIN32
  aux_table_value = (ADDRESS_TYPE) 250;
#else
  aux_table_value = (ADDRESS_TYPE)sysconf(_SC_CLK_TCK);
#endif
  sp = SetAuxTableEntry(stack_data, sp, aux_table_symbol, aux_table_value);
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
ADDRESS_TYPE Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetAuxTableEntry(
    uint8_t * stack_data, ADDRESS_TYPE sp,
    ADDRESS_TYPE entry, ADDRESS_TYPE value) const {
  ADDRESS_TYPE target_entry = Host2Target(endianness_, entry);
  ADDRESS_TYPE target_value = Host2Target(endianness_, value);
  sp = sp - sizeof(target_value);
  uint8_t *addr;
  if(stack_data) {
    addr = stack_data + sp;
    memcpy(addr, &target_value, sizeof(target_value));
  }
  sp = sp - sizeof(target_entry);
  if(stack_data) {
    addr = stack_data + sp;
    memcpy(addr, &target_entry, sizeof(target_entry));
  }
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

  logger_ << DebugError
      << "Unknown system type (" << system_type_ << ")" << EndDebugError;
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
    uint32_t *segment_tls_buf = (uint32_t *) malloc(sizeof(uint32_t) * tls_buf_length);
    uint32_t *section_tls_buf = (uint32_t *) malloc(sizeof(uint32_t) * tls_buf_length);

    for (unsigned int i = 0; i < tls_buf_length; ++i) {
      segment_tls_buf[i] = unisim::util::endian::Host2Target(endianness_, tls_buf[i]);
      section_tls_buf[i] = unisim::util::endian::Host2Target(endianness_, tls_buf[i]);
    }
    Segment *tls_if_segment =
        new Segment(Segment::TY_LOADABLE, Segment::SA_X, 4, tls_base_addr,
                    sizeof(tls_buf), sizeof(tls_buf), segment_tls_buf);
    Section *tls_if_section =
        new Section(Section::TY_UNKNOWN, Section::SA_A,
                    ".unisim.linux_os.interface.tls", 4, 0, tls_base_addr,
                    sizeof(tls_buf), section_tls_buf);

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
    uint32_t *segment_cmpxchg_buf = (uint32_t *) malloc(sizeof(uint32_t) * cmpxchg_buf_length);
    uint32_t *section_cmpxchg_buf = (uint32_t *) malloc(sizeof(uint32_t) * cmpxchg_buf_length);

    for (unsigned int i = 0; i < cmpxchg_buf_length; ++i) {
        segment_cmpxchg_buf[i] = unisim::util::endian::Host2Target(endianness_, cmpxchg_buf[i]);
        section_cmpxchg_buf[i] = unisim::util::endian::Host2Target(endianness_, cmpxchg_buf[i]);
    }
    Section* cmpxchg_if_section =
        new Section(Section::TY_UNKNOWN, Section::SA_A,
                    ".unisim.linux_os.interface.cmpxchg",
                    4, 0, cmpxchg_base_addr,
                    sizeof(cmpxchg_buf), segment_cmpxchg_buf);
    Segment *cmpxchg_if_segment =
        new Segment(Segment::TY_LOADABLE, Segment::SA_X,
                    4, cmpxchg_base_addr,
                    sizeof(cmpxchg_buf), sizeof(cmpxchg_buf), section_cmpxchg_buf);

    blob->AddSegment(tls_if_segment);
    blob->AddSegment(cmpxchg_if_segment);
    blob->AddSection(tls_if_section);
    blob->AddSection(cmpxchg_if_section);
  }

  return true;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
bool Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetPPCBlob(
    unisim::util::debug::blob::Blob<ADDRESS_TYPE> *blob) const {
  // Nothing to do but that may change in the future if necessary
  return true;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
bool Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetupLinuxOSARM() {
  SetSyscallId(std::string("exit"), 1);
  SetSyscallId(std::string("read"), 3);
  SetSyscallId(std::string("write"), 4);
  SetSyscallId(std::string("open"), 5);
  SetSyscallId(std::string("close"), 6);
  SetSyscallId(std::string("unlink"), 10);
  SetSyscallId(std::string("time"), 13);
  SetSyscallId(std::string("lseek"), 19);
  SetSyscallId(std::string("getpid"), 20);
  SetSyscallId(std::string("getuid"), 24);
  SetSyscallId(std::string("access"), 33);
  SetSyscallId(std::string("kill"), 37);
  SetSyscallId(std::string("rename"), 38);
  SetSyscallId(std::string("dup"), 41);
  SetSyscallId(std::string("times"), 43);
  SetSyscallId(std::string("brk"), 45);
  SetSyscallId(std::string("getgid"), 47);
  SetSyscallId(std::string("geteuid"), 49);
  SetSyscallId(std::string("getegid"), 50);
  SetSyscallId(std::string("ioctl"), 54);
  SetSyscallId(std::string("setrlimit"), 75);
  SetSyscallId(std::string("getrusage"), 77);
  SetSyscallId(std::string("gettimeofday"), 78);
  SetSyscallId(std::string("munmap"), 91);
  SetSyscallId(std::string("ftruncate"), 93);
  SetSyscallId(std::string("socketcall"), 102);
  SetSyscallId(std::string("stat"), 106);
  SetSyscallId(std::string("fstat"), 108);
  SetSyscallId(std::string("uname"), 122);
  SetSyscallId(std::string("llseek"), 140);
  SetSyscallId(std::string("writev"), 146);
  SetSyscallId(std::string("rt_sigaction"), 174);
  SetSyscallId(std::string("rt_sigprocmask"), 175);
  SetSyscallId(std::string("ugetrlimit"), 191);
  SetSyscallId(std::string("mmap2"), 192);
  SetSyscallId(std::string("stat64"), 195);
  SetSyscallId(std::string("fstat64"), 197);
  SetSyscallId(std::string("getuid32"), 199);
  SetSyscallId(std::string("getgid32"), 200);
  SetSyscallId(std::string("geteuid32"), 201);
  SetSyscallId(std::string("getegid32"), 202);
  SetSyscallId(std::string("fcntl64"), 221);
  SetSyscallId(std::string("flistxattr"), 234);
  SetSyscallId(std::string("exit_group"), 248);
  // the following are private to the arm
  SetSyscallId(std::string("breakpoint"), 983041);
  SetSyscallId(std::string("cacheflush"), 983042);
  SetSyscallId(std::string("usr26"), 983043);
  SetSyscallId(std::string("usr32"), 983044);
  SetSyscallId(std::string("set_tls"), 983045);

  return true;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
bool Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetupLinuxOSPPC() {
  SetSyscallId(std::string("exit"), 1);
  SetSyscallId(std::string("read"), 3);
  SetSyscallId(std::string("write"), 4);
  SetSyscallId(std::string("open"), 5);
  SetSyscallId(std::string("close"), 6);
  SetSyscallId(std::string("unlink"), 10);
  SetSyscallId(std::string("time"), 13);
  SetSyscallId(std::string("lseek"), 19);
  SetSyscallId(std::string("getpid"), 20);
  SetSyscallId(std::string("getuid"), 24);
  SetSyscallId(std::string("access"), 33);
  SetSyscallId(std::string("kill"), 37);
  SetSyscallId(std::string("rename"), 38);
  SetSyscallId(std::string("times"), 43);
  SetSyscallId(std::string("brk"), 45);
  SetSyscallId(std::string("getgid"), 47);
  SetSyscallId(std::string("geteuid"), 49);
  SetSyscallId(std::string("getegid"), 50);
  SetSyscallId(std::string("ioctl"), 54);
  SetSyscallId(std::string("umask"), 60);
  SetSyscallId(std::string("setrlimit"), 75);
  SetSyscallId(std::string("getrlimit"), 76);
  SetSyscallId(std::string("getrusage"), 77);
  SetSyscallId(std::string("gettimeofday"), 78);
  SetSyscallId(std::string("mmap"), 90);
  SetSyscallId(std::string("munmap"), 91);
  SetSyscallId(std::string("ftruncate"), 93);
  SetSyscallId(std::string("statfs"), 99);
  SetSyscallId(std::string("socketcall"), 102);
  SetSyscallId(std::string("stat"), 106);
  SetSyscallId(std::string("fstat"), 108);
  SetSyscallId(std::string("uname"), 122);
  SetSyscallId(std::string("llseek"), 140);
  SetSyscallId(std::string("writev"), 146);
  SetSyscallId(std::string("rt_sigaction"), 173);
  SetSyscallId(std::string("rt_sigprocmask"), 174);
  SetSyscallId(std::string("ugetrlimit"), 190);
  SetSyscallId(std::string("mmap2"), 192);
  SetSyscallId(std::string("stat64"), 195);
  SetSyscallId(std::string("fstat64"), 197);
  SetSyscallId(std::string("fcntl64"), 204);
  SetSyscallId(std::string("getpid"), 207); // in a mono-thread environment pid=tid
  SetSyscallId(std::string("tkill"), 208);
  SetSyscallId(std::string("flistxattr"), 217);
  SetSyscallId(std::string("exit_group"), 234);
  SetSyscallId(std::string("tgkill"), 250);

  return true;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetSyscallId(
    const std::string &syscall_name, int syscall_id) {
  if(unlikely(verbose_))
    logger_
        << DebugInfo
        << "Associating syscall_name \"" << syscall_name << "\""
        << "to syscall_id number " << syscall_id
        << EndDebugInfo;
  if (HasSyscall(syscall_name)) {
    if (HasSyscall(syscall_id)) {
      std::stringstream s;
      s << __FUNCTION__ << ":" << __FILE__ << ":" << __LINE__ << std::endl;
      s << "syscall_id already associated to syscall \""
          << syscall_name_assoc_map_[syscall_id] << "\"" << std::endl;
      s << "  you wanted to associate it to " << syscall_name << std::endl;
      throw std::runtime_error(s.str().c_str());
    }
    syscall_name_assoc_map_[syscall_id] = syscall_name;
    syscall_impl_assoc_map_[syscall_id] = syscall_name_map_[syscall_name];
  } else {
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
  syscall_name_map_[std::string("tkill")] = &thistype::LSC_tkill;
  syscall_name_map_[std::string("tgkill")] = &thistype::LSC_tgkill;
  syscall_name_map_[std::string("ftruncate")] = &thistype::LSC_ftruncate;
  syscall_name_map_[std::string("umask")] = &thistype::LSC_umask;
  syscall_name_map_[std::string("gettimeofday")] = &thistype::LSC_gettimeofday;
  syscall_name_map_[std::string("statfs")] = &thistype::LSC_statfs;
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
  else if ( system_type_.compare("ppc") == 0 )
    return PPCGetSyscallNumber(id);
  logger_ << DebugError
      << "Could not translate syscall number "
      << id << " (" << std::hex << id << std::dec << ") for system \""
      << system_type_ << "\". Returning untranslated id."
      << EndDebugError;
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
  PARAMETER_TYPE translated_id = 0;
  if (regs_if_ == NULL) {
    logger_ << DebugError
        << "no register interface available." << EndDebugError;
    return 0;
  }

  if (GetRegister(kARMEABISyscallNumberReg,
                               &translated_id))
    return (int)translated_id;
  return 0;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
int Linux<ADDRESS_TYPE, PARAMETER_TYPE>::PPCGetSyscallNumber(int id) {
  return id;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
bool Linux<ADDRESS_TYPE, PARAMETER_TYPE>::ReadMem(
    ADDRESS_TYPE addr, uint8_t * buffer, uint32_t size) {
  if (mem_inject_if_ == NULL) return false;
  if (mem_inject_if_->InjectReadMemory(addr, buffer, size)) {
    if (unlikely(verbose_)) {
      logger_
          << DebugInfo
          << "OS read memory:" << std::endl
          << "\taddr = 0x" << std::hex << addr << std::dec << std::endl
          << "\tsize = " << size << std::endl
          << "\tdata =" << std::hex;
      for (unsigned int i = 0; i < size; i++)
        logger_ << " " << (unsigned int)buffer[i];
      logger_ << std::dec
              << EndDebugInfo;
    }
    return true;
  } else {
    logger_
        << DebugWarning
        << "failed OS read memory:" << std::endl
        << "\taddr = 0x" << std::hex << addr << std::dec << std::endl
        << "\tsize = " << size
        << EndDebugWarning;
    return false;
  }
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
bool Linux<ADDRESS_TYPE, PARAMETER_TYPE>::WriteMem(
    ADDRESS_TYPE addr, uint8_t const * const buffer, uint32_t size) {
  if (mem_inject_if_ == NULL) return false;
  if (unlikely(verbose_)) {
    logger_ << DebugInfo
        << "OS write memory:" << std::endl
        << "\taddr = 0x" << std::hex << addr << std::dec << std::endl
        << "\tsize = " << size << std::endl
        << "\tdata =" << std::hex;
    for (unsigned int i = 0; i < size; i++)
      logger_ << " " << (unsigned int)buffer[i];
    logger_ << std::dec
            << EndDebugInfo;
  }
  return mem_inject_if_->InjectWriteMemory(addr, buffer, size);
}

} // end of namespace linux
} // end of namespace os
} // end of namespace util
} // end of namespace unisim

#endif // __UNISIM_UTIL_OS_LINUX_LINUX_TCC__
