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

#include <iostream>
#include <sstream>
#include <map>
#include <vector>
#include <string>
#include <inttypes.h>

#include "unisim/util/endian/endian.hh"
#include "unisim/util/os/linux/linux.hh"

class RegInterface:
    public unisim::util::os::linux_os::LinuxRegisterInterface<uint32_t> {
 public:
  RegInterface();
  ~RegInterface();

  bool GetRegister(uint32_t id, uint32_t * const value);
  bool SetRegister(uint32_t id, uint32_t value);

 private:
  std::map<uint32_t, uint32_t> regs_;
};

RegInterface::RegInterface() {
}

RegInterface::~RegInterface() {
}

bool RegInterface::GetRegister(uint32_t id, uint32_t * const value) {
  std::cout << "TEST GetRegister id    = " << id << std::endl
      <<       "            ---> value = 0x" << std::hex;
  bool created = false;
  std::map<uint32_t, uint32_t>::const_iterator it;
  it = regs_.find(id);
  if (it == regs_.end()) {
    regs_[id] = 0;
    created = true;
    it = regs_.find(id);
  }
  std::cout << it->second << std::dec
      << (created?" (created)":"") << std::endl;
  return true;
}

bool RegInterface::SetRegister(uint32_t id, uint32_t value) {
  std::cout << "TEST SetRegister id    = " << id << std::endl
      <<       "                 value = 0x"
      << std::hex << value << std::dec;
  bool created = false;
  std::map<uint32_t, uint32_t>::const_iterator it;
  it = regs_.find(id);
  if (it == regs_.end()) {
    regs_[id] = 0;
    created = true;
  }
  if (created)
    std::cout << " (created)";
  else
    std::cout << " (0x" << std::hex << it->second << std::dec << ")";
  std::cout << std::endl;
  return true;
}

class MemInterface:
    public unisim::util::os::linux_os::LinuxMemoryInterface<uint32_t> {
 public:
  MemInterface();
  ~MemInterface();

  bool ReadMemory(uint32_t addr, uint8_t * const buffer, uint32_t size);
  bool WriteMemory(uint32_t addr, uint8_t const * const buffer, uint32_t size);

 private:
  std::map<uint32_t, uint8_t> mem_;
};

MemInterface::MemInterface() {
}

MemInterface::~MemInterface() {
}

bool MemInterface::ReadMemory(uint32_t addr, uint8_t * const buffer,
                              uint32_t size) {
  std::cout << "TEST ReadMemory  addr = 0x" << std::hex << addr << std::dec
      << "  size = " << size << std::endl;
  for (uint32_t index = 0; index < size; ++index) {
    std::map<uint32_t, uint8_t>::const_iterator it;
    it = mem_.find(addr + index);
    if (it == mem_.end()) buffer[index] = 0;
    else buffer[index] = it->second;
  }
  std::cout << "            --->" << std::hex;
  for ( uint32_t index = 0; index < size; ++index)
    std::cout << " " << buffer[index];
  std::cout << std::dec << std::endl;
  return true;
}

bool MemInterface::WriteMemory(uint32_t addr, uint8_t const * const buffer,
                               uint32_t size) {
  std::cout << "TEST WriteMemory addr = 0x" << std::hex << addr << std::dec
      << "  size = " << size << std::endl;
  for (uint32_t index = 0; index < size; ++index)
    mem_[addr + index] = buffer[index];
  return true;
}

int main(int argc, char *argv[])
{
  RegInterface regs;
  MemInterface mem;

  if (argc != 2) {
    std::cerr << "ERROR wrong number of parameters." << std::endl;
    return -1;
  }

  std::cout << "Trying to open linux elf file " << argv[1] << " ...."
      << std::endl;

  std::ostringstream log;
  unisim::util::os::linux_os::Linux<uint32_t, uint32_t> prog(true, &log);

  std::cout << "Setting up the linux library ..." << std::endl;

  if (!prog.SetSystemType("arm-eabi")) {
    std::cout << "Could not set arm-eabi system type" << std::endl;
    std::cerr << log << std::endl;
    return -1;
  }
  prog.SetEndianess(unisim::util::endian::E_LITTLE_ENDIAN);
  prog.SetStackSize(0x800000UL);
  prog.SetMemoryPageSize(4096);
  prog.SetUname("Linux",
                "localhost",
                "2.6.27.35",
                "#UNISIM SMP Fri Mar 12 05:23:09 UTC 2010",
                "armv5",
                "localhost");
  prog.SetRegisterInterface(regs);
  prog.SetMemoryInterface(mem);

  if (!prog.AddLoadFile(argv[1])) {
    std::cout << "Could not set load file \"" << argv[1] << "\"" << std::endl;
    std::cerr << log.str() << std::endl;
    return -1;
  }

  std::cout << "... linux library setup completed." << std::endl;

  std::cout << "Setting command line and environment ..." << std::endl;

  std::vector<std::string> cmd_line;
  /// std::string argv_1(argv[1]);
  cmd_line.push_back(argv[1]);
  prog.SetCommandLine(cmd_line);
  prog.SetApplyHostEnvironment(true);

  std::cout << "... command line and environment set." << std::endl;

  std::cout << "Performing load ..." << std::endl;
  if (!prog.Load()) {
    std::cerr << "Could not perform the load" << std::endl;
    std::cerr << log.str() << std::endl;
    return -1;
  }
  std::cout << "... load completed." << std::endl;

  return 0;
}
