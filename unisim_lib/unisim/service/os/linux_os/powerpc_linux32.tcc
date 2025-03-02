/*
 *  Copyright (c) 2016,
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

#ifndef __UNISIM_SERVICE_OS_LINUX_OS_POWERPC_LINUX32_TCC__
#define __UNISIM_SERVICE_OS_LINUX_OS_POWERPC_LINUX32_TCC__

#include <unisim/util/os/linux_os/ppc.hh>
#include <inttypes.h>


namespace unisim {
namespace service {
namespace os {
namespace linux_os {

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
PowerPCLinux32<ADDRESS_TYPE,PARAMETER_TYPE>::PowerPCLinux32( const char* name, unisim::kernel::Object* parent )
  : unisim::kernel::Object( name, parent )
  , Linux<uint32_t, uint32_t>( name, parent )
{
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
void
PowerPCLinux32<ADDRESS_TYPE,PARAMETER_TYPE>::SetupTargetSystem()
{
  Linux<ADDRESS_TYPE,PARAMETER_TYPE>::linux_lib.SetTargetSystem( new unisim::util::os::linux_os::PPCTS<Linux<uint32_t, uint32_t>::LinuxImpl>( Linux<ADDRESS_TYPE,PARAMETER_TYPE>::linux_lib ) );
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
PowerPCLinux32<ADDRESS_TYPE,PARAMETER_TYPE>::~PowerPCLinux32()
{
  delete Linux<ADDRESS_TYPE,PARAMETER_TYPE>::linux_lib.GetTargetSystem();
}

} // end of linux_os namespace
} // end of os namespace
} // end of service namespace
} // end of unisim namespace

#endif // __UNISIM_SERVICE_OS_LINUX_OS_POWERPC_LINUX32_TCC__
