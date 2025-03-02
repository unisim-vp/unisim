/*
 *  Copyright (c) 2012,
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
 */

#include <unisim/service/os/linux_os/arm_linux32.hh>
#include <unisim/util/os/linux_os/arm.hh>
#include <inttypes.h>

#include <unisim/service/os/linux_os/linux.tcc>

namespace unisim {
namespace service {
namespace os {
namespace linux_os {

ArmLinux32::ArmLinux32( const char* name, unisim::kernel::Object* parent )
  : unisim::kernel::Object( name, parent )
  , Linux<uint32_t, uint32_t>( name, parent )
  , system_name()
  , param_system_name("system", this, system_name, "Emulated system architecture. Available values are \"arm\" and \"arm-eabi\"")
{
}

void
ArmLinux32::SetupTargetSystem()
{
  linux_lib.SetTargetSystem( new unisim::util::os::linux_os::ARMTS<Linux<uint32_t, uint32_t>::LinuxImpl>( system_name, linux_lib ) );
}

ArmLinux32::~ArmLinux32()
{
  delete linux_lib.GetTargetSystem();
}

} // end of linux_os namespace
} // end of os namespace
} // end of service namespace
} // end of unisim namespace
