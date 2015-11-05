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


#include <unisim/kernel/service/service.hh>
#include <unisim/kernel/logger/logger.hh>
#include <unisim/util/endian/endian.hh>
#include <inttypes.h>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace arm {

struct CP15
  : public unisim::kernel::service::Object
{
  /** Constructor
   *
   * @param name   the UNISIM object name of this CP15
   * @param parent the UNISIM parent object of this CP15
   */
  CP15( const char *name, unisim::kernel::service::Object *parent = 0 );
  
  virtual ~CP15() {}
  /** Object setup method.
   * This method is required for all UNISIM objects and will be called
   *   during the setup phase.
   * 
   * @return true on success, false otherwise
   */
  virtual bool BeginSetup();

  /** Read the value of a register
   *
   * @param core    the CORE this CP15 coprocessor is attached to
   * @param opcode1 the "opcode1" field of the instruction code 
   * @param opcode2 the "opcode2" field of the instruction code
   * @param crn     the "crn" field of the instruction code
   * @param crm     the "crm" field of the instruction code
   * @return        the read value
   */
  template <class CORE>
  uint32_t ReadRegister( CORE& core, uint8_t opcode1, uint8_t opcode2, uint8_t crn, uint8_t crm );

  /** Write a value in a register
   * 
   * @param core    the CORE this CP15 coprocessor is attached to
   * @param opcode1 the "opcode1" field of the instruction code
   * @param opcode2 the "opcode2" field of the instruction code
   * @param crn     the "crn" field of the instruction code
   * @param crm     the "crm" field of the instruction code
   * @param val     value to be written to the register
   */
  template <class CORE>
  void WriteRegister( CORE& core, uint8_t opcode1, uint8_t opcode2, uint8_t crn, uint8_t crm, uint32_t value );
  
  /** Get the endianness of the processor
   *
   * @return the current endianness defined in the control register
   */
  unisim::util::endian::endian_type GetEndianness() const
  {
    return (control_register_c1 & CONTROL_REGISTER_C1_B) ? unisim::util::endian::E_BIG_ENDIAN : unisim::util::endian::E_LITTLE_ENDIAN;
  };

  /** Get location of the exception vector
   *
   * @return true if enabled, false otherwise
   */
  bool GetVINITHI() const
  {
    return bool( control_register_c1 & CONTROL_REGISTER_C1_V );
  }

  /** MMU enable/disabled.
   *
   * @return true if enabled, false otherwise
   */
  bool IsMMUEnabled() const
  {
    return bool( control_register_c1 & CONTROL_REGISTER_C1_M );
  }

  /** ICache enabled/disabled.
   *
   * @return true if enabled, false otherwise
   */
  bool IsICacheEnabled() const
  {
    return bool( control_register_c1 & CONTROL_REGISTER_C1_I );
  }

  /** DCache enabled/disabled.
   *
   * @return true if enabled, false otherwise
   */
  uint32_t IsDCacheEnabled() const
  {
    return bool( control_register_c1 & CONTROL_REGISTER_C1_C );
  }

  /** Get the Translation Table Base register value.
   *
   * @return the current TTB register
   */
  uint32_t GetTTB() const
  {
    return translation_table_base_register_c2;
  }

  /** Get the current FCSE PID.
   *
   * @return the current FCSE PID
   */
  uint32_t GetFCSE_PID() const
  {
    return fsce_pid_register_c13;
  }

  /** FCSE PID mask */
  static const uint32_t FCSE_PID_MASK = 0xfe000000UL;

private:
  /** String describing the endianness of the processor. */
  std::string bigendinit_string;
  /** UNISIM Parameter to set the default endianness.
   */
  unisim::kernel::service::Parameter<std::string> param_bigendinit;

  /** String describing the location of exception vectors
   * This corresponds to the signal VINITHI. */
  std::string vinithi_string;
  /** UNISIM Parameter to set the location of exception vectors. */
  unisim::kernel::service::Parameter<std::string> param_vinithi;

  /** Verbose. */
  uint32_t verbose;
  /** UNISIM Parameter to set the verbose mode. */
  unisim::kernel::service::Parameter<uint32_t> param_verbose;

  /** UNISIM logger */
  unisim::kernel::logger::Logger logger;

  /** CP15 ID code register */
  uint32_t id_code_register_c0;
  static const uint32_t ID_CODE_REGISTER_C0 =
    ((uint32_t)0x041  << 24) |
    ((uint32_t)0x0    << 20) |
    ((uint32_t)0x06   << 16) |
    ((uint32_t)0x0926 << 4 ) |
    (uint32_t)0x05;
  /** CP15 control register */
  uint32_t control_register_c1;
  static const uint32_t CONTROL_REGISTER_C1_SBZ = 0xfffa0c00UL;
  static const uint32_t CONTROL_REGISTER_C1_SBO = 0x00050078UL;
  static const uint32_t CONTROL_REGISTER_C1_L4 = 0x08000UL;
  static const uint32_t CONTROL_REGISTER_C1_RR = 0x04000UL;
  static const uint32_t CONTROL_REGISTER_C1_V  = 0x02000UL;
  static const uint32_t CONTROL_REGISTER_C1_I  = 0x01000UL;
  static const uint32_t CONTROL_REGISTER_C1_R  = 0x00200UL;
  static const uint32_t CONTROL_REGISTER_C1_S  = 0x00100UL;
  static const uint32_t CONTROL_REGISTER_C1_B  = 0x00080UL;
  static const uint32_t CONTROL_REGISTER_C1_C  = 0x00004UL;
  static const uint32_t CONTROL_REGISTER_C1_A  = 0x00002UL;
  static const uint32_t CONTROL_REGISTER_C1_M  = 0x00001UL;
  /** CP15 Translation table base register */
  uint32_t translation_table_base_register_c2;
  static const uint32_t TRANSLATION_TABLE_BASE_REGISTER_SBZ =
    0x00003fffUL;
  /** CP15 Domain access control register */
  uint32_t domain_access_control_register_c3;
  /** CP15 Process ID register */
  uint32_t fsce_pid_register_c13;
  uint32_t context_id_register_c13;

  /** UNISIM register access */
  unisim::kernel::service::Register<uint32_t> reg_id_code_register_c0;
  unisim::kernel::service::Register<uint32_t> reg_control_register_c1;
  unisim::kernel::service::Register<uint32_t> reg_translation_table_base_register_c2;
  unisim::kernel::service::Register<uint32_t> reg_domain_access_control_register_c3;
  unisim::kernel::service::Register<uint32_t> reg_fsce_pid_register_c13;
  unisim::kernel::service::Register<uint32_t> reg_context_id_register_c13;
};

} // end of namespace arm
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_ARM926EJS_CP15_HH__
