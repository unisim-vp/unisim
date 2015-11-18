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
 
#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_EXCEPTION_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_EXCEPTION_HH__

#include <unisim/component/cxx/processor/arm/register_field.hh>
#include <inttypes.h>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace arm {
namespace exception {
  
  /*** RegisterField for the Synchronous Virtual exception vector ***/
  /* Synchronous */
  static RegisterField< 0, 1> const RESET;  // Reset
  static RegisterField< 1, 1> const PABRT;  // Prefetch Abort (including prefetch TLB miss)
  static RegisterField< 2, 1> const UNDEF;  // Undefined instruction
  static RegisterField< 3, 1> const SWI;    // Software Interrupt
  static RegisterField< 4, 1> const DABRT;  // Data Abort (including data TLB miss)
  static RegisterField< 5, 1> const EABRT;  // Synchronous External Abort (External memory system exception)
  
  /* Asynchronous */
  static RegisterField<24, 8> const ASYNC;  // Mask for all asynchronous abort
  
  static RegisterField<24, 1> const FIQ;    // FIQ external asynchronous abort
  static RegisterField<25, 1> const IRQ;    // IRQ external asynchronous abort
  
  /** SynchronousAbort the class used to abort normal execution of an
   *  instruction (using a throw).
   */
  struct UndefInstrException { UndefInstrException() {} };
  struct HypTrapException { HypTrapException() {} };
  struct SVCException { SVCException() {} };
  struct SMCException { SMCException() {} };
  struct HVCException { HVCException() {} };
  struct PrefetchAbortException { PrefetchAbortException() {} };
  struct DataAbortException { DataAbortException() {} };
  struct VirtualAbortException { VirtualAbortException() {} };
  
} // end of namespace exception
} // end of namespace arm
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_EXCEPTION_HH__
