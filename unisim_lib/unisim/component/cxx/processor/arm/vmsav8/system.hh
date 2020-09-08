/*
 *  Copyright (c) 2019-2020,
 *  Commissariat a l'Energie Atomique (CEA)
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without 
 *  modification, are permitted provided that the following conditions are met:
 *
 *   - Redistributions of source code must retain the above copyright notice, 
 *     this list of conditions and the following disclaimer.
 *
 *   - Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 *   - Neither the name of CEA nor the names of its contributors may be used to
 *     endorse or promote products derived from this software without specific 
 *     prior written permission.
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
 * Authors: Yves Lhuillier Perez (yves.lhuillier@cea.fr)
 */

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_VMSAV8_SYSTEM_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_VMSAV8_SYSTEM_HH__


#include <unisim/component/cxx/processor/arm/register_field.hh>
#include <inttypes.h>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace arm {
namespace vmsav8 {

  /** TCR_EL1 Manips
   *
   * Base Register Fields to ease access to the TCR System Control
   * Register.  This class provides all TCR register fields. Each of
   * them are not mandatory in all TCR instances and may be omitted
   * by particular ARM implementation.
   */

  namespace tcr
  {
    RegisterField<38,1> const TBI1;   // Top Byte ignored
    RegisterField<37,1> const TBI0;   // Top Byte ignored
    RegisterField<36,1> const AS;     // ASID Size
    RegisterField<32,3> const IPS;    // Intermediate Physical Address Size
    
    RegisterField<30,2> const TG1;    // TTBR1 Granule size
    RegisterField<28,2> const SH1;    // Shareability attribute for memory associated with translation table walks using TTBR1
    RegisterField<26,2> const ORGN1;  // Outer cacheability attribute for memory associated with translation table walks using TTBR1
    RegisterField<24,2> const IRGN1;  // Inner cacheability attribute for memory associated with translation table walks using TTBR1
    RegisterField<23,1> const EPD1;   // Translation table walk disable for translations using TTBR1
    RegisterField<22,1> const A1;     // Selects whether TTBR0_EL1 or TTBR1_EL1 defines the ASID
    RegisterField<16,6> const T1SZ;   // The size offset of the memory region addressed by TTBR1
    
    RegisterField<14,2> const TG0;    // TTBR0 Granule size
    RegisterField<12,2> const SH0;    // Shareability attribute for memory associated with translation table walks using TTBR0
    RegisterField<10,2> const ORGN0;  // Outer cacheability attribute for memory associated with translation table walks using TTBR0
    RegisterField< 8,2> const IRGN0;  // Inner cacheability attribute for memory associated with translation table walks using TTBR0
    RegisterField< 7,1> const EPD0;   // Translation table walk disable for translations using TTBR0
    RegisterField< 0,6> const T0SZ;   // The size offset of the memory region addressed by TTBR0
  }

  namespace sctlr
  {
    RegisterField<14,1> const DZE;    // Access to DC ZVA instruction at EL0.
  }
  
} // end of namespace vmsav8
} // end of namespace arm
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_VMSAV8_SYSTEM_HH__
