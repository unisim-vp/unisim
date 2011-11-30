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

#include "unisim/util/os/linux/linux.hh"

int main(int argc, char *argv[])
{
  if (argc != 2) {
    std::cerr << "ERROR wrong number of parameters." << std::endl;
    return -1;
  }

  std::cout << "Trying to open linux elf file " << argv[1] << " ...."
      << std::endl;

  std::ostringstream log;
  unisim::util::os::linux_os::Linux<uint32_t, uint32_t> prog(true, &log);

  if (!prog.SetSystemType("arm-eabi")) {
    std::cout << "Could not set arm-eabi system type" << std::endl;
    std::cerr << log << std::endl;
    return -1;
  }

  if (!prog.AddLoadFile(argv[1])) {
    std::cout << "Could not set load file \"" << argv[1] << "\"" << std::endl;
    std::cerr << log.str() << std::endl;
    return -1;
  }

  return 0;
}
