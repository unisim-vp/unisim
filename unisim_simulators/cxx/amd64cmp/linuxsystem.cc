/*
 *  Copyright (c) 2018,
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
 * Authors: Yves Lhuillier (yves.lhuillier@cea.fr)
 */

#include <linuxsystem.hh>
#include <arch.hh>
#include <unisim/util/os/linux_os/linux.tcc>
#include <unisim/util/os/linux_os/calls.tcc>
#include <iomanip>
#include <sys/utsname.h>

Amd64LinuxOS::Amd64LinuxOS(Arch& cpu)
  : ThisLinux( std::cout, std::cout, std::cout )
{
  // Set up the different linuxlib parameters
  SetInterfaces(&cpu, &cpu, &cpu);
  SetTargetSystem(new unisim::util::os::linux_os::AMD64TS<ThisLinux>(*this));
  SetEndianness( unisim::util::endian::E_LITTLE_ENDIAN );
  struct utsname unm;
  uname(&unm);
  SetUname(unm.sysname, unm.nodename, unm.release, unm.version, unm.machine, unm.domainname);
  is_load_ = true;
}

Amd64LinuxOS::~Amd64LinuxOS()
{
  delete GetTargetSystem();
}

void
Amd64LinuxOS::ExecuteSystemCall(int id)
{
  /* Takes special care of FD side effects. Always clear the
   * descriptor from filelists so that guest IO always fails (and
   * never translates to host IO). */
  target_to_host_fildes.clear();
  target_fildes_free_list = std::queue<int32_t>();
  bool exited;
  int app_ret_status;
  ThisLinux::ExecuteSystemCall( id, exited, app_ret_status );
  if (exited)
    throw 0;
}
