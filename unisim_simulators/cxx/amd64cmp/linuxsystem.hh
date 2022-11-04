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

#ifndef __AMD64EMU_LINUXSYSTEM_HH__
#define __AMD64EMU_LINUXSYSTEM_HH__

#include <unisim/util/os/linux_os/linux.hh>
#include <unisim/util/os/linux_os/amd64.hh>
#include <unisim/kernel/kernel.hh>
#include <unisim/service/interfaces/linux_os.hh>
#include <unisim/service/interfaces/blob.hh>
#include <unisim/service/interfaces/memory_injection.hh>
#include <unisim/service/interfaces/memory.hh>
#include <unisim/service/interfaces/registers.hh>
#include <inttypes.h>

struct Arch;

struct Amd64LinuxOS : public unisim::util::os::linux_os::Linux<uint64_t, uint64_t>
{
  typedef unisim::util::os::linux_os::Linux<uint64_t, uint64_t> ThisLinux;
  
  Amd64LinuxOS( Arch& cpu );
  ~Amd64LinuxOS();

  void ExecuteSystemCall( int id );
};

#endif // __AMD64EMU_LINUXSYSTEM_HH__
