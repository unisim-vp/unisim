/*
 *  Copyright (c) 2007,
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
 * Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 */

#ifndef __UNISIM_SERVICE_INTERFACES_CPU_LINUX_OS_HH__
#define __UNISIM_SERVICE_INTERFACES_CPU_LINUX_OS_HH__

#include <sys/stat.h>
#include <sys/times.h>
#include "unisim/service/interfaces/cpu_os.hh"
  
namespace unisim {
namespace service {
namespace interfaces {

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
class CPULinuxOS :
  	public unisim::service::interfaces::CPUOS<ADDRESS_TYPE, PARAMETER_TYPE> {
public:
    virtual void* ConvertStat(struct stat *) = 0;
    virtual unsigned int StatSize(void) = 0;
    virtual void* ConvertStat64(struct stat64 *) = 0;
    virtual unsigned int Stat64Size(void) = 0;
    virtual void* ConvertTms(struct tms *) = 0;
    virtual unsigned int TmsSize(void) = 0;
  };

} // end of interfaces namespace
} // end of service namespace
} // end of unisim namespace

#endif // __UNISIM_SERVICE_INTERFACES_CPU_LINUX_OS_HH__
