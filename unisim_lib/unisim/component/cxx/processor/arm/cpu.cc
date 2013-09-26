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
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
 *  SERVICES;LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 *  OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF 
 *  SUCH DAMAGE.
 *
 * Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr), Yves Lhuillier (yves.lhuillier@cea.fr)
 */

#include <iostream>
#include <sstream>
#include <inttypes.h>
#include "unisim/component/cxx/processor/arm/cpu.hh"
#include "unisim/util/debug/symbol.hh"
#include "unisim/util/debug/simple_register.hh"
#include "unisim/util/arithmetic/arithmetic.hh"

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace arm {

using unisim::util::endian::E_BIG_ENDIAN;
using unisim::util::endian::E_LITTLE_ENDIAN;
using std::string;
using std::stringstream;
using std::ostringstream;
using unisim::util::debug::Register;
using unisim::util::debug::SimpleRegister;
using std::map;

using unisim::util::endian::endian_type;
using unisim::util::endian::E_BIG_ENDIAN;
using unisim::util::endian::E_LITTLE_ENDIAN;
using unisim::util::endian::BigEndian2Host;
using unisim::util::endian::Host2BigEndian;
using unisim::util::endian::Target2Host;
using unisim::util::endian::Host2Target;
using unisim::util::arithmetic::RotateRight;
using std::cout;
using std::cerr;
using std::endl;
using std::hex;
using std::dec;
using std::ostringstream;
using unisim::util::debug::SimpleRegister;
using unisim::util::debug::Symbol;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::EndDebugWarning;


// class ProgramCounterRegister (unisim::util::debug::Register) used for PC/R15 view
class ProgramCounterRegister : public Register
{
public:
  ProgramCounterRegister(const char* _name, CPU& _cpu) : name(_name), cpu(_cpu) {}
  virtual ~ProgramCounterRegister() {}
  virtual const char *GetName() const { return name.c_str(); }
  virtual void GetValue(void *buffer) const { *((uint32_t*)buffer) = cpu.GetNPC(); }
  virtual void SetValue(const void *buffer) { uint32_t address = *((uint32_t*)buffer); cpu.BranchExchange( address ); }
  virtual int GetSize() const { return 4; }
private:
  std::string name;
  CPU&        cpu;
};

// Constructor
CPU::CPU(char const* name, Object* parent, endian_type endianness)
  : Object(name, parent)
  , logger(*this)
  , default_endianness(endianness)
  , munged_address_mask8(0)
  , munged_address_mask16(0)
  , registers_registry()
  , cpsr()
  , fake_fps(0)
  , exception(0)
{
  // Initialize general purpose registers
  for(unsigned int i = 0; i < num_log_gprs; i++)
    gpr[i] = 0;
  this->current_pc = 0;
  this->next_pc = 0;
	
  for(unsigned int i = 0; i < num_phys_gprs; i++)
    phys_gpr[i] = 0;

  // Initialize fake floating point registers
  for(unsigned int i = 0; i < 8; i++) {
    fake_fpr[i] = 0;
  }
  fake_fps = 0;

  // Initialize address mungling
  SetEndianness(default_endianness);

  // initialize the registers debugging interface
  for(int i = 0; i < 15; i++) {
    stringstream str;
    str << "r" << i;
    registers_registry[str.str().c_str()] =
      new SimpleRegister<uint32_t>(str.str().c_str(), &gpr[i]);
  }
  registers_registry["r15"] =  new ProgramCounterRegister("r15", *this);
  registers_registry["pc"] =   new ProgramCounterRegister("pc", *this);
  registers_registry["sp"] =   new SimpleRegister<uint32_t>("sp", &gpr[13]);
  registers_registry["lr"] =   new SimpleRegister<uint32_t>("lr", &gpr[14]);
  registers_registry["cpsr"] = new SimpleRegister<uint32_t>("cpsr", &(cpsr.m_value));
}

/** Destructor.
 */
CPU ::
~CPU()
{
	for (map<string, Register *>::iterator it = registers_registry.begin();
			it != registers_registry.end();
			it++)
		delete it->second;

	registers_registry.clear();
}

/** Set the endianness of the processor to the given endianness.
 * This method takes into charge to set the correct address mungling 
 *   depending on the endianness.
 *
 * @param endianness the endianness to use
 */
void 
CPU::
SetEndianness(endian_type endianness)
{
	default_endianness = endianness;

	// set the variables to compute the final address on memory
	//   accesses
	if(endianness == E_BIG_ENDIAN) {
		munged_address_mask8 = (uint32_t)0x03;;
		munged_address_mask16 = (uint32_t)0x02;
	} else {
		munged_address_mask8 = 0;
		munged_address_mask16 = 0;
	}
}

/** Arrange the GPR mapping depending on initial and target running mode.
 *
 * @param src_mode the running mode the processor is currently in
 * @param tar_mode the target running mode the registers should be mapped to
 */
void 
CPU::
SetGPRMapping(uint32_t src_mode, uint32_t tar_mode)
{
	/* gpr organization per running mode:
	 * - user:           0-14 (R0-R14),                  15 (PC)
	 * - system:         0-14 (R0-R14),                  15 (PC)
	 * - supervisor:     0-12 (R0-R12), 16-17 (R13-R14), 15 (PC)
	 * - abort:          0-12 (R0-R12), 18-19 (R13-R14), 15 (PC)
	 * - undefined:      0-12 (R0-R12), 20-21 (R13-R14), 15 (PC)
	 * - interrupt:      0-12 (R0-R12), 22-23 (R13-R14), 15 (PC)
	 * - fast interrupt: 0-7 (R0-R7),   24-30 (R8-R14),  15 (PC)
	 */

	/* Backup current running mode */
	switch (src_mode)
	{
		case USER_MODE:
		case SYSTEM_MODE:
			for (unsigned int i = 0; i < 15; i++) 
			{
				phys_gpr[i] = gpr[i];
			}
			phys_gpr[15] = this->next_pc;
			break;
		case SUPERVISOR_MODE:
			for (unsigned int i = 0; i < 13; i++) 
			{
				phys_gpr[i] = gpr[i];
			}
			phys_gpr[16] = gpr[13];
			phys_gpr[17] = gpr[14];
			phys_gpr[15] = this->next_pc;
			break;
		case ABORT_MODE:
			for (unsigned int i = 0; i < 13; i++) 
			{
				phys_gpr[i] = gpr[i];
			}
			phys_gpr[18] = gpr[13];
			phys_gpr[19] = gpr[14];
			phys_gpr[15] = this->next_pc;
			break;
		case UNDEFINED_MODE:
			for (unsigned int i = 0; i < 13; i++) 
			{
				phys_gpr[i] = gpr[i];
			}
			phys_gpr[20] = gpr[13];
			phys_gpr[21] = gpr[14];
			phys_gpr[15] = this->next_pc;
			break;
		case IRQ_MODE:
			for (unsigned int i = 0; i < 13; i++) 
			{
				phys_gpr[i] = gpr[i];
			}
			phys_gpr[22] = gpr[13];
			phys_gpr[23] = gpr[14];
			phys_gpr[15] = this->next_pc;
			break;
		case FIQ_MODE:
			for (unsigned int i = 0; i < 8; i++) 
			{
				phys_gpr[i] = gpr[i];
			}
			for (unsigned int i = 0; i < 7; i++) 
			{
				phys_gpr[24 + i] = gpr[8 + i];
			}
			phys_gpr[15] = this->next_pc;
			break;
		default:
			assert(0);
	}

	/* Update registers to the target mode */
	switch (tar_mode) 
	{
		case USER_MODE:
		case SYSTEM_MODE:
			for (unsigned int i = 0; i < 15; i++) 
			{
				gpr[i] = phys_gpr[i];
			}
			this->next_pc = phys_gpr[15];
			break;
		case SUPERVISOR_MODE:
			for (unsigned int i = 0; i < 13; i++) 
			{
				gpr[i] = phys_gpr[i];
			}
			gpr[13] = phys_gpr[16];
			gpr[14] = phys_gpr[17];
			this->next_pc = phys_gpr[15];
			break;
		case ABORT_MODE:
			for (unsigned int i = 0; i < 13; i++) 
			{
				gpr[i] = phys_gpr[i];
			}
			gpr[13] = phys_gpr[18];
			gpr[14] = phys_gpr[19];
			this->next_pc = phys_gpr[15];
			break;
		case UNDEFINED_MODE:
			for (unsigned int i = 0; i < 13; i++) 
			{
				gpr[i] = phys_gpr[i];
			}
			gpr[13] = phys_gpr[20];
			gpr[14] = phys_gpr[21];
			this->next_pc = phys_gpr[15];
			break;
		case IRQ_MODE:
			for (unsigned int i = 0; i < 13; i++) 
			{
				gpr[i] = phys_gpr[i];
			}
			gpr[13] = phys_gpr[22];
			gpr[14] = phys_gpr[23];
			this->next_pc = phys_gpr[15];
			break;
		case FIQ_MODE:
			for (unsigned int i = 0; i < 8; i++) 
			{
				gpr[i] = phys_gpr[i];
			}
			for (unsigned int i = 0; i < 7; i++) 
			{
				gpr[8 + i] = phys_gpr[24 + i];
			}
			this->next_pc = phys_gpr[15];
			break;
		default:
			assert(0);
	}
}

/** Get the value contained by a user GPR.
 * Returns the value contained by a user GPR. It is the same than GetGPR but
 *   restricting the index from 0 to 15 (only the first 16 registers).
 *
 * @param id the register index
 * @return the value contained by the register
 */
uint32_t 
CPU::
GetGPR_usr(uint32_t id)
{
  uint32_t mode = cpsr.M().Get();
  switch ( mode )
    {
    case SUPERVISOR_MODE:
    case ABORT_MODE:
    case UNDEFINED_MODE:
    case IRQ_MODE:
      if ( (id < 13) || (id == 15) )
        return GetGPR(id);
      else
        return phys_gpr[id];
      break;
    case FIQ_MODE:
      if ( (id < 8) || (id == 15) )
        return GetGPR(id);
      else
        return phys_gpr[id];
      break;
    case USER_MODE:
    case SYSTEM_MODE:
    default:
      return GetGPR(id);
      break;
    }
}

/** Set the value contained by a user GPR.
 * Sets the value contained by a user GPR. It is the same than SetGPR byt
 *   restricting the index from 0 to 15 (only the first 16 registers).
 *
 * @param id the register index
 * @param val the value to set
 */
void
CPU::
SetGPR_usr(uint32_t id, uint32_t val)
{
  uint32_t mode = cpsr.M().Get();
  switch ( mode )
    {
    case USER_MODE:
    case SYSTEM_MODE:
      SetGPR( id, val );
      break;
    case SUPERVISOR_MODE:
    case ABORT_MODE:
    case UNDEFINED_MODE:
    case IRQ_MODE:
      if ( (id < 13) || (id == 15) )
        SetGPR( id, val );
      else
        phys_gpr[id] = val;
      break;
    case FIQ_MODE:
      if ( (id < 8) || (id == 15 ) )
        SetGPR( id, val );
      else
        phys_gpr[id] = val;
      break;
    }
}

/** Get SPSR index from current running mode
 *
 * @return the SPSR index from current running mode
 */
uint32_t 
CPU::
GetSPSRIndex()
{
  uint32_t rm = 0;
  uint32_t run_mode = cpsr.M().Get();
  switch (run_mode)
    {
    case USER_MODE: case SYSTEM_MODE: {
      /* In user or system mode, access to SPSR are unpredictable,
       * thus the code whould never try to access SPSR in such
       * modes. */
      
      logger << DebugWarning
             << "trying to access SPSR while running in "
             << ((run_mode == USER_MODE) ? "user" : "system")
             << " mode with the following instruction: "
             << std::endl
             << "Location: " << __FUNCTION__
             << ":" << __FILE__
             << ":" << __LINE__
             << EndDebugWarning;
      Stop(-1);
    } break;
    case SUPERVISOR_MODE:
      rm = 0;
      break;
    case ABORT_MODE:
      rm = 1;
      break;
    case UNDEFINED_MODE:
      rm = 2;
      break;
    case IRQ_MODE:
      rm = 3;
      break;
    case FIQ_MODE:
      rm = 4;
      break;
    default:
      assert(0);
      break;
    }
	
  return rm;
}

/** Copy the value of current SPSR register into CPSR.
 */
void 
CPU::
MoveSPSRtoCPSR()
{
  /* SPSR needs to be moved to CPSR
   * This means that we need to change the register mapping if the running mode has changed
   */
  uint32_t src_mode = CPSR().M().Get();
  uint32_t dst_mode = SPSR().M().Get();
  CPSR().bits() = SPSR().bits();
  if (src_mode != dst_mode)
    SetGPRMapping(src_mode, dst_mode);
}

/** Mark an exception in the virtual exception vector.
 * This marks an new exception in the virtual exception vector for 
 *   later treatment.
 *   NOTE: exception types are available at cxx/processor/arm/exception.hh
 *
 * @param except the exception to mark
 */
void 
CPU::
MarkVirtualExceptionVector(uint32_t except)
{
	exception |= except;
}

/** Get the virtual exception vector.
 * This returns the value of the virtual exception vector.
 *
 * @return the value of the exception vector
 */
uint32_t 
CPU::
GetVirtualExceptionVector()
{
	return exception;
}

/** Reset the value of the virtual exception vector.
 *
 * @param mask the value to set at reset
 */
void 
CPU::
ResetVirtualExceptionVector(uint32_t mask)
{
	exception = mask;
}

} // end of namespace arm
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim


