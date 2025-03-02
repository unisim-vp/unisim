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

#ifndef __E5500FPV_LINUX_SYSTEM_HH__
#define __E5500FPV_LINUX_SYSTEM_HH__

#include <unisim/util/os/linux_os/linux.hh>
#include <unisim/service/interfaces/linux_os.hh>
#include <unisim/service/interfaces/memory_injection.hh>
#include <unisim/service/interfaces/memory.hh>
#include <unisim/service/interfaces/registers.hh>
#include <unisim/service/interfaces/blob.hh>
#include <unisim/kernel/kernel.hh>
#include <inttypes.h>

struct LinuxOS
  : public unisim::service::interfaces::LinuxOS
  , public unisim::kernel::Service<unisim::service::interfaces::Blob<uint64_t> >
{
  typedef uint64_t addr_t;

  LinuxOS( std::ostream& log,
           unisim::service::interfaces::Registers* regs_if,
           unisim::service::interfaces::Memory<addr_t>* mem_if,
           unisim::service::interfaces::MemoryInjection<addr_t>* mem_inject_if );
  ~LinuxOS();

  void Setup( std::vector<std::string> const& simargs, std::vector<std::string> const& envs );

  void ExecuteSystemCall( int id );

  // unisim::service::interfaces::Blob<addr_t>
  virtual unisim::util::blob::Blob<addr_t> const* GetBlob() const { return linux_lib.GetBlob(); }
  unisim::kernel::ServiceExport<unisim::service::interfaces::Blob<addr_t> > blob_export;

  unisim::util::os::linux_os::Linux<addr_t, addr_t> linux_lib;
  bool exited;
  int app_ret_status;
};

#endif // __E5500FPV_LINUX_SYSTEM_HH__
