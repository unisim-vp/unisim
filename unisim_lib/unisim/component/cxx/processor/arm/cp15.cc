/*
 *  Copyright (c) 2010,
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

// #define CP15__DEBUG

#include <unisim/component/cxx/processor/arm/cp15.hh>
#include <assert.h>

#define likely(x)       __builtin_expect((x),1)
#define unlikely(x)     __builtin_expect((x),0)

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace arm {

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugError;

/** Constructor
 *
 * @param _cpu the cpu the coprocessor is attached to
 */
CP15::CP15( const char* name, unisim::kernel::service::Object* parent )
  : Object(name, parent)
  , bigendinit_string("little-endian")
  , param_bigendinit("bigendinit", this, bigendinit_string, "Determines the value of the BIGENDINIT signal. Available values are: little-endian and big-endian.")
  , vinithi_string("low")
  , param_vinithi("vinithi", this, vinithi_string, "Determines the value of the VINITHI signal. Available values are: low and high.")
  , verbose(0)
  , param_verbose("verbose", this, verbose, "Enable verbose mode (0 = non verbose, anything else = verbose).")
  , logger(*this)
  , id_code_register_c0(ID_CODE_REGISTER_C0)
  , control_register_c1(CONTROL_REGISTER_C1_SBO)
  , translation_table_base_register_c2(0)
  , domain_access_control_register_c3(0)
  , fsce_pid_register_c13(0)
  , context_id_register_c13(0)
  , reg_id_code_register_c0("c0_0", this, id_code_register_c0, "ID Code register.")
  , reg_control_register_c1("c1", this, control_register_c1, "Control register.")
  , reg_translation_table_base_register_c2("c2", this, translation_table_base_register_c2, "Translation table base register.")
  , reg_domain_access_control_register_c3("c3", this, domain_access_control_register_c3, "Domain access control register.")
  , reg_fsce_pid_register_c13("c13_0", this, fsce_pid_register_c13, "Fast Context Switch Extension (FCSE) Process Identifier (PID) register.")
  , reg_context_id_register_c13("c13_1", this, context_id_register_c13, "Context ID register.")
{
}

/** Object setup method.
 * This method is required for all UNISIM objects and will be called during
 *   the setup phase.
 * 
 * @return true on success, false otherwise
 */
bool 
CP15::BeginSetup()
{
  logger << DebugInfo << "CP15 Setup" << EndDebugInfo;
  /* check bigendinit parameter */
  if ( (bigendinit_string.compare("little-endian") != 0) &&
      (bigendinit_string.compare("big-endian") != 0) )
  {
    logger << DebugError
      << "Error while setting the default endianness (BIGENDINIT)."
      << " '" << bigendinit_string << "' is not a correct"
      << " value."
      << " Available values are: little-endian and big-endian."
      << EndDebugError;
    return false;
  }
  /* check vinithi parameter */
  if ( (vinithi_string.compare("low") != 0) &&
      (vinithi_string.compare("high") != 0) )
  {
    logger << DebugError
      << "Error while setting the location of the exception vectors"
      << " (VINITHI). '" << vinithi_string << "' is not a correct value."
      << " Available values are: low and high."
      << EndDebugError;
    return false;
  }

  /* fix the endianness depending on the endianness parameter */
  if ( verbose )
    logger << DebugInfo
      << "Setting endianness to the value of BIGENDINIT ("
      << bigendinit_string
      << ")"
      << EndDebugInfo;
  if ( bigendinit_string.compare("little-endian") == 0 )
    control_register_c1 &= ~CONTROL_REGISTER_C1_B;
  else
    control_register_c1 |= CONTROL_REGISTER_C1_B;

  /* fix the location of the exception vectors depending on vinithi */
  if ( verbose )
    logger << DebugInfo
      << "Setting the location of the exception to the value of VINITHI ("
      << vinithi_string
      << ")"
      << EndDebugInfo;
  if ( vinithi_string.compare("low") == 0 )
    control_register_c1 &= ~CONTROL_REGISTER_C1_V;
  else
    control_register_c1 |= CONTROL_REGISTER_C1_V;

  return true;
}

} // end of namespace arm
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim
