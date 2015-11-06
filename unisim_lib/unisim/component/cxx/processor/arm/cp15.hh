/*
 *  Copyright (c) 2010-2015,
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
 * Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 */

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_ARM926EJS_CP15_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_ARM926EJS_CP15_HH__


#include <unisim/component/cxx/processor/arm/register_field.hh>
#include <unisim/kernel/service/service.hh>
#include <unisim/kernel/logger/logger.hh>
#include <unisim/util/endian/endian.hh>
#include <inttypes.h>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace arm {

  /** SCTLR Manip class
   *
   * Base Traits class to ease access to the SCTLR System Control
   * Register.  This class factorizes all common SCTLR register
   * fields, though some of them may not be present in a particular
   * ARM processor implementation
   */

namespace SCTLR
{
  RegisterField<30,1> const TE;      // Thumb Exception enable
  RegisterField<29,1> const AFE;     // Access flag enable.
  RegisterField<28,1> const TRE;     // TEX remap enable
  RegisterField<27,1> const NMFI;    // Non-maskable FIQ (NMFI) support
  RegisterField<25,1> const EE;      // Exception Endianness.
  RegisterField<24,1> const VE;      // Interrupt Vectors Enable
  RegisterField<22,1> const U;       // Alignment Model (up to ARMv6)
  RegisterField<21,1> const FI;      // Fast interrupts configuration enable
  RegisterField<20,1> const UWXN;    // Unprivileged write permission implies PL1 XN (Virtualization Extensions)
  RegisterField<19,1> const WXN;     // Write permission implies XN (Virtualization Extensions)
  RegisterField<17,1> const HA;      // Hardware Access flag enable.
  RegisterField<14,1> const RR;      // Round Robin select
  RegisterField<13,1> const V;       // Vectors bit
  RegisterField<12,1> const I;       // Instruction cache enable
  RegisterField<11,1> const Z;       // Branch prediction enable.
  RegisterField<10,1> const SW;      // SWP and SWPB enable. This bit enables the use of SWP and SWPB instructions.
  RegisterField< 7,1> const B;       // Endianness model (up to ARMv6)
  RegisterField< 5,1> const CP15BEN; // CP15 barrier enable.
  RegisterField< 2,1> const C;       // Cache enable. This is a global enable bit for data and unified caches.
  RegisterField< 1,1> const A;       // Alignment check enable
  RegisterField< 0,1> const M;       // MMU enable.
};

// struct CP15
// {
//   /** Object setup method.
//    * This method is required for all UNISIM objects and will be called
//    *   during the setup phase.
//    * 
//    * @return true on success, false otherwise
//    */
//   virtual bool BeginSetup();

//   /** Write a value in a register
//    * 
//    * @param core    the CORE this CP15 coprocessor is attached to
//    * @param opcode1 the "opcode1" field of the instruction code
//    * @param opcode2 the "opcode2" field of the instruction code
//    * @param crn     the "crn" field of the instruction code
//    * @param crm     the "crm" field of the instruction code
//    * @param val     value to be written to the register
//    */
//   template <class CORE>
//   void WriteRegister( CORE& core, uint8_t opcode1, uint8_t opcode2, uint8_t crn, uint8_t crm, uint32_t value );
  
//   /** Get the endianness of the processor
//    *
//    * @return the current endianness defined in the control register
//    */
//   // unisim::util::endian::endian_type GetEndianness() const
//   // {
//   //   return (control_register_c1 & CONTROL_REGISTER_C1_B) ? unisim::util::endian::E_BIG_ENDIAN : unisim::util::endian::E_LITTLE_ENDIAN;
//   // };

//   /** Get location of the exception vector
//    *
//    * @return true if enabled, false otherwise
//    */
//   bool GetVINITHI() const
//   {
//     return bool( control_register_c1 & CONTROL_REGISTER_C1_V );
//   }

//   /** MMU enable/disabled.
//    *
//    * @return true if enabled, false otherwise
//    */
//   bool IsMMUEnabled() const
//   {
//     return bool( control_register_c1 & CONTROL_REGISTER_C1_M );
//   }

//   /** ICache enabled/disabled.
//    *
//    * @return true if enabled, false otherwise
//    */
//   bool IsICacheEnabled() const
//   {
//     return bool( control_register_c1 & CONTROL_REGISTER_C1_I );
//   }

//   /** DCache enabled/disabled.
//    *
//    * @return true if enabled, false otherwise
//    */
//   uint32_t IsDCacheEnabled() const
//   {
//     return bool( control_register_c1 & CONTROL_REGISTER_C1_C );
//   }

//   /** Get the Translation Table Base register value.
//    *
//    * @return the current TTB register
//    */
//   uint32_t GetTTB() const
//   {
//     return translation_table_base_register_c2;
//   }

//   /** Get the current FCSE PID.
//    *
//    * @return the current FCSE PID
//    */
//   uint32_t GetFCSE_PID() const
//   {
//     return fsce_pid_register_c13;
//   }

//   /** FCSE PID mask */
//   static const uint32_t FCSE_PID_MASK = 0xfe000000UL;

// private:
//   /** String describing the endianness of the processor. */
//   std::string bigendinit_string;
//   /** UNISIM Parameter to set the default endianness.
//    */
//   unisim::kernel::service::Parameter<std::string> param_bigendinit;

//   /** String describing the location of exception vectors
//    * This corresponds to the signal VINITHI. */
//   std::string vinithi_string;
//   /** UNISIM Parameter to set the location of exception vectors. */
//   unisim::kernel::service::Parameter<std::string> param_vinithi;

//   /** Verbose. */
//   uint32_t verbose;
//   /** UNISIM Parameter to set the verbose mode. */
//   unisim::kernel::service::Parameter<uint32_t> param_verbose;

//   /** UNISIM logger */
//   unisim::kernel::logger::Logger logger;

//   /** CP15 ID code register */
//   uint32_t id_code_register_c0;
//   /** CP15 Domain access control register */
//   uint32_t domain_access_control_register_c3;
//   /** CP15 Process ID register */
//   uint32_t fsce_pid_register_c13;
//   uint32_t context_id_register_c13;

//   /** UNISIM register access */
//   unisim::kernel::service::Register<uint32_t> reg_id_code_register_c0;
//   unisim::kernel::service::Register<uint32_t> reg_control_register_c1;
//   unisim::kernel::service::Register<uint32_t> reg_translation_table_base_register_c2;
//   unisim::kernel::service::Register<uint32_t> reg_domain_access_control_register_c3;
//   unisim::kernel::service::Register<uint32_t> reg_fsce_pid_register_c13;
//   unisim::kernel::service::Register<uint32_t> reg_context_id_register_c13;
// };

} // end of namespace arm
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_ARM926EJS_CP15_HH__
