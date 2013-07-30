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
 * Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 */

#include <iostream>
#include <sstream>
#include <inttypes.h>
#include <assert.h>
#include "unisim/component/cxx/processor/arm/cpu.hh"
#include "unisim/component/cxx/processor/arm/masks.hh"
#include "unisim/util/debug/symbol.hh"
#include "unisim/util/debug/simple_register.hh"
#include "unisim/util/arithmetic/arithmetic.hh"

//#include <sstream>
//#include <iostream>
//#include <stdlib.h>
//#include "unisim/component/cxx/processor/arm/cpu.hh"
//#include "unisim/component/cxx/processor/arm/masks.hh"
//#include "unisim/component/cxx/processor/arm/config.hh"
//#include "unisim/component/cxx/processor/arm/isa_arm32.tcc"
//#include "unisim/component/cxx/processor/arm/isa_thumb.tcc"
//#include "unisim/component/cxx/processor/arm/instruction.tcc"
//#include "unisim/component/cxx/processor/arm/exception.tcc"
//#include "unisim/util/debug/simple_register.hh"
//#ifndef __STDC_CONSTANT_MACROS
//#define __STDC_CONSTANT_MACROS
//#endif // __STDC_CONSTANT_MACROS
//#include <stdint.h>

// #if (defined(__GNUC__) && (__GNUC__ >= 3))
// #define INLINE __attribute__((always_inline))
// #else
// #define INLINE
// #endif

// #define ARM_OPTIMIZATION

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
CPU ::
CPU(endian_type endianness)
	: default_endianness(endianness)
	, munged_address_mask8(0)
	, munged_address_mask16(0)
	, registers_registry()
	, cpsr(0)
	, fake_fps(0)
	, exception(0)
{
	// Initialize general purpose registers
	for(unsigned int i = 0; i < num_log_gprs; i++)
		gpr[i] = 0;
	pc = 0;
	
	for(unsigned int i = 0; i < num_phys_gprs; i++)
		phys_gpr[i] = 0;

	// Initialize status (CPSR/SPSR) registers
	cpsr = 0;
	for(unsigned int i = 0; i < num_phys_spsrs; i++) {
		spsr[i] = 0;
	}

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
	registers_registry["cpsr"] = new SimpleRegister<uint32_t>("cpsr", &cpsr);
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

/** Get the endian configuration of the processor.
 *
 * @return the endian being used
 */
endian_type
CPU ::
GetEndianness() 
{
	return default_endianness;
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
			phys_gpr[15] = pc;
			break;
		case SUPERVISOR_MODE:
			for (unsigned int i = 0; i < 13; i++) 
			{
				phys_gpr[i] = gpr[i];
			}
			phys_gpr[16] = gpr[13];
			phys_gpr[17] = gpr[14];
			phys_gpr[15] = pc;
			break;
		case ABORT_MODE:
			for (unsigned int i = 0; i < 13; i++) 
			{
				phys_gpr[i] = gpr[i];
			}
			phys_gpr[18] = gpr[13];
			phys_gpr[19] = gpr[14];
			phys_gpr[15] = pc;
			break;
		case UNDEFINED_MODE:
			for (unsigned int i = 0; i < 13; i++) 
			{
				phys_gpr[i] = gpr[i];
			}
			phys_gpr[20] = gpr[13];
			phys_gpr[21] = gpr[14];
			phys_gpr[15] = pc;
			break;
		case IRQ_MODE:
			for (unsigned int i = 0; i < 13; i++) 
			{
				phys_gpr[i] = gpr[i];
			}
			phys_gpr[22] = gpr[13];
			phys_gpr[23] = gpr[14];
			phys_gpr[15] = pc;
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
			phys_gpr[15] = pc;
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
			pc = phys_gpr[15];
			break;
		case SUPERVISOR_MODE:
			for (unsigned int i = 0; i < 13; i++) 
			{
				gpr[i] = phys_gpr[i];
			}
			gpr[13] = phys_gpr[16];
			gpr[14] = phys_gpr[17];
			pc = phys_gpr[15];
			break;
		case ABORT_MODE:
			for (unsigned int i = 0; i < 13; i++) 
			{
				gpr[i] = phys_gpr[i];
			}
			gpr[13] = phys_gpr[18];
			gpr[14] = phys_gpr[19];
			pc = phys_gpr[15];
			break;
		case UNDEFINED_MODE:
			for (unsigned int i = 0; i < 13; i++) 
			{
				gpr[i] = phys_gpr[i];
			}
			gpr[13] = phys_gpr[20];
			gpr[14] = phys_gpr[21];
			pc = phys_gpr[15];
			break;
		case IRQ_MODE:
			for (unsigned int i = 0; i < 13; i++) 
			{
				gpr[i] = phys_gpr[i];
			}
			gpr[13] = phys_gpr[22];
			gpr[14] = phys_gpr[23];
			pc = phys_gpr[15];
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
			pc = phys_gpr[15];
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
GetGPR_usr(uint32_t id) const
{
	uint32_t mode = GetCPSR_Mode();
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
	uint32_t mode = GetCPSR_Mode();
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

/** Get the value of the CPSR register.
 *
 * @return the value of the CPSR register.
 */
uint32_t 
CPU::
GetCPSR() const
{
	return cpsr;
}

/** Set the value of the CPSR register.
 *
 * @param val the value to set
 */
void 
CPU::
SetCPSR(uint32_t val)
{
	cpsr = val;
}

/** Set the value of the NZCV bits of the CPSR register.
 *
 * @param n the N bit
 * @param z the Z bit
 * @param c the C bit
 * @param v the V bit
 */
void 
CPU::
SetCPSR_NZCV(bool n,
		bool z,
		bool c,
		bool v)
{
	if(n) cpsr = cpsr | CPSR_N_MASK;
	else cpsr = cpsr & ~(CPSR_N_MASK);
	if(z) cpsr = cpsr | CPSR_Z_MASK;
	else cpsr = cpsr & ~(CPSR_Z_MASK);
	if(c) cpsr = cpsr | CPSR_C_MASK;
	else cpsr = cpsr & ~(CPSR_C_MASK);
	if(v) cpsr = cpsr | CPSR_V_MASK;
	else cpsr = cpsr & ~(CPSR_V_MASK);
}

/** Set the N bit of the CPSR register.
 *
 * @param val the value of the N bit
 */
void 
CPU::
SetCPSR_N(const bool val)
{
	if(val) cpsr = cpsr | CPSR_N_MASK;
	else cpsr = cpsr & ~(CPSR_N_MASK);
}

/** Unset the N bit of the CPSR register (set to 0).
 * This method is analagous to SetCPSR_N(false).
 */
void 
CPU::
UnsetCPSR_N()
{
	cpsr = cpsr & ~(CPSR_N_MASK);
}

/** Get the value of the CPSR register N bit.
 *
 * @return the value of the CPSR register N bit
 */
bool 
CPU::
GetCPSR_N() const
{
	return (cpsr & CPSR_N_MASK) == CPSR_N_MASK;
}

/** Set the Z bit of the CPSR register.
 *
 * @param val the value of the Z bit
 */
void 
CPU::
SetCPSR_Z(const bool val)
{
	if(val) cpsr = cpsr | CPSR_Z_MASK;
	else cpsr = cpsr & ~(CPSR_Z_MASK);
}

/** Unset the Z bit of the CPSR register (set to 0).
 * This method is analogous to SetCPSR_Z(false).
 */
void 
CPU:: 
UnsetCPSR_Z()
{
	cpsr = cpsr & ~(CPSR_Z_MASK);
}

/** Get the value of the CPSR register Z bit.
 *
 * @return the value of the CPSR register Z bit
 */
bool 
CPU::
GetCPSR_Z() const
{
	return (cpsr & CPSR_Z_MASK) == CPSR_Z_MASK;
}

/** Set the C bit of the CPSR register.
 *
 * @param val the value of the C bit
 */
void 
CPU::
SetCPSR_C(const bool val)
{
	if(val) cpsr = cpsr | CPSR_C_MASK;
	else cpsr = cpsr & ~(CPSR_C_MASK);
}

/** Unset the C bit of the CPSR register (set to 0).
 * This method is analogous to SetCPSR_C(false).
 */
void 
CPU::
UnsetCPSR_C()
{
	cpsr = cpsr & ~(CPSR_C_MASK);
}

/** Get the value of the CPSR register C bit.
 *
 * @return the value of the CPSR register C bit
 */
bool 
CPU::
GetCPSR_C() const
{
	return (cpsr & CPSR_C_MASK) == CPSR_C_MASK;
}

/** Set teh V bit of the CPSR register.
 *
 * @param val the value of the V bit
 */
void 
CPU::
SetCPSR_V(const bool val)
{
	if(val) cpsr = cpsr | CPSR_V_MASK;
	else cpsr = cpsr & ~(CPSR_V_MASK);
}

/** Unset the V bit of the CPSR register (set to 0).
 * This method is analogous to SetCPSR_V(false).
 */
void 
CPU::
UnsetCPSR_V()
{
	cpsr = cpsr & ~(CPSR_V_MASK);
}

/** Get the value of the CPSR register V bit.
 *
 * @return the value of the CPSR register V bit
 */
bool 
CPU::
GetCPSR_V() const
{
	return (cpsr & CPSR_V_MASK) == CPSR_V_MASK;
}

/** Set the Q bit of the CPSR register.
 *
 * @param val teh value of the Q bit
 */
void 
CPU::
SetCPSR_Q(const bool val)
{
	if(val) cpsr = cpsr | CPSR_Q_MASK;
	else cpsr = cpsr & ~(CPSR_Q_MASK);
}

/** Unset the Q bit of the CPSR register (set to 0).
 * This method is analogous to SetCPSR_Q(false).
 */
void 
CPU::
UnsetCPSR_Q()
{
	cpsr = cpsr & ~(CPSR_Q_MASK);
}

/** Get the value of the CPSR register Q bit.
 * 
 * @return the value of the CPSR register Q bit
 */
bool 
CPU::
GetCPSR_Q()
{
	return (cpsr & CPSR_Q_MASK) == CPSR_Q_MASK;
}

/** Set the I bit of the CPSR register.
 *
 * @param val the value of the I bit
 */
void 
CPU::
SetCPSR_I(const bool val)
{
	if(val) cpsr = cpsr | CPSR_I_MASK;
	else cpsr = cpsr & ~(CPSR_I_MASK);
}

/** Unset the I bit of the CPSR register (set to 0).
 * This method is analogous to SetCPSR_I(false).
 */
void 
CPU::
UnsetCPSR_I()
{
	cpsr = cpsr & ~(CPSR_I_MASK);
}

/** Get the value of the CPSR register I bit.
 *
 * @return the value of the CPSR register I bit
 */
bool 
CPU::
GetCPSR_I()
{
	return (cpsr & CPSR_I_MASK) == CPSR_I_MASK;
}

/** Set the F bit of the CPSR register.
 *
 * @param val the value of the F bit
 */
void 
CPU::
SetCPSR_F(const bool val)
{
	if(val) cpsr = cpsr | CPSR_F_MASK;
	else cpsr = cpsr & ~(CPSR_F_MASK);
}

/** Unset the F bit of the CPSR register (set to 0).
 * This method is analogous to SetCPSR_F(false).
 */
void 
CPU::
UnsetCPSR_F()
{
	cpsr = cpsr & ~(CPSR_F_MASK);
}

/** Get the value of the CPSR register F bit.
 *
 * @return the value of the CPSR register F bit
 */
bool 
CPU::
GetCPSR_F()
{
	return (cpsr & CPSR_F_MASK) == CPSR_F_MASK;
}

/** Set the T bit of the CPSR register.
 *
 * @param val the value of the T bit
 */
void 
CPU::
SetCPSR_T(const bool val)
{
	if(val) cpsr = cpsr | CPSR_T_MASK;
	else cpsr = cpsr & ~(CPSR_T_MASK);
}

/** Unset the T bit of the CPSR register (set to 0).
 * This method is analogous to SetCPSR_T(false).
 */
void 
CPU::
UnsetCPSR_T()
{
	cpsr = cpsr & ~(CPSR_T_MASK);
}

/** Get the value of the CPSR register T bit.
 *
 * @return the value of the CPSR register T bit.
 */
bool 
CPU::
GetCPSR_T()
{
	return (cpsr & CPSR_T_MASK) == CPSR_T_MASK;
}

/** Set the mode value of the CPSR register.
 *
 * @param mode the mode to set in the CPSR register
 */
void 
CPU::
SetCPSR_Mode(uint32_t mode)
{
	cpsr = cpsr & ~(CPSR_RUNNING_MODE_MASK);
	cpsr = cpsr | mode;
}

/** Get the mode value of the CPSR register.
 *
 * @return the mode value of the CPSR register
 */
uint32_t 
CPU::
GetCPSR_Mode() const
{
	uint32_t mode = cpsr & CPSR_RUNNING_MODE_MASK;
	return mode;
}

/** Get the value of the CPSR register.
 *
 * @return the value of the CPSR register
 */
uint32_t 
CPU::
GetSPSR()
{
	unsigned int rm;
	
	rm = GetSPSRIndex();
	
	return spsr[rm];
}

/** Set the value of the SPSR register.
 *
 * @param val the value to set
 */
void 
CPU::
SetSPSR(uint32_t val)
{
	unsigned int rm;
	
	rm = GetSPSRIndex();
	
	spsr[rm] = val;
}

/** Set the value of the NZCV bits of the SPSR register.
 *
 * @param n the N bit
 * @param z the Z bit
 * @param c the C bit
 * @param v the V bit
 */
void 
CPU::
SetSPSR_NZCV(bool n,
		bool z,
		bool c,
		bool v)
{
	unsigned int rm;
	
	rm = GetSPSRIndex();
	
	if (n) spsr[rm] = spsr[rm] | SPSR_N_MASK;
	else spsr[rm] = spsr[rm] & ~(SPSR_N_MASK);
	if (z) spsr[rm] = spsr[rm] | SPSR_Z_MASK;
	else spsr[rm] = spsr[rm] & ~(SPSR_Z_MASK);
	if (c) spsr[rm] = spsr[rm] | SPSR_C_MASK;
	else spsr[rm] = spsr[rm] & ~(SPSR_C_MASK);
	if (v) spsr[rm] = spsr[rm] | SPSR_V_MASK;
	else spsr[rm] = spsr[rm] & ~(SPSR_V_MASK);
}

/** Set the N bit of the SPSR register.
 *
 * @param val the value of the N bit
 */
void 
CPU::
SetSPSR_N(const bool val)
{
	unsigned int rm;
	
	rm = GetSPSRIndex();
	
	if (val) spsr[rm] = spsr[rm] | SPSR_N_MASK;
	else spsr[rm] = spsr[rm] & ~(SPSR_N_MASK);
}

/** Unset the N bit of the SPSR register (set to 0).
 * This method is analogous to SetSPSR_N(false).
 */
void 
CPU::
UnsetSPSR_N()
{
	unsigned int rm;

	rm = GetSPSRIndex();
	
	spsr[rm] = spsr[rm] & ~(SPSR_N_MASK);
}

/** Get the value of the SPSR register N bit.
 *
 * @return the value of the SPSR register N bit.
 */
bool 
CPU::
GetSPSR_N()
{
	unsigned int rm;
	
	rm = GetSPSRIndex();
	
	return (spsr[rm] & SPSR_N_MASK) == SPSR_N_MASK;
}

/** Set the Z bit of the SPSR register.
 *
 * @param val the value of the Z bit
 */
void 
CPU::
SetSPSR_Z(const bool val)
{
	unsigned int rm;
	
	rm = GetSPSRIndex();
	
	if (val) spsr[rm] = spsr[rm] | SPSR_Z_MASK;
	else spsr[rm] = spsr[rm] & ~(SPSR_Z_MASK);
}

/** Unset the Z bit of the SPSR register (set to 0).
 * This method is analogous to SetSPSR_Z(false).
 */
void 
CPU::
UnsetSPSR_Z()
{
	unsigned int rm;
	
	rm = GetSPSRIndex();
	
	spsr[rm] = spsr[rm] & ~(SPSR_Z_MASK);
}

/** Get the value of the SPSR Z bit.
 *
 * @return the value of the SPSR register Z bit
 */
bool 
CPU::
GetSPSR_Z()
{
	unsigned int rm;
	
	rm = GetSPSRIndex();
	
	return (spsr[rm] & SPSR_Z_MASK) == SPSR_Z_MASK;
}

/** Set the C bit of the SPSR register.
 *
 * @param val the value of the C bit
 */
void 
CPU::
SetSPSR_C(const bool val)
{
	unsigned int rm;

	rm = GetSPSRIndex();
	
	if (val) spsr[rm] = spsr[rm] | SPSR_C_MASK;
	else spsr[rm] = spsr[rm] & ~(SPSR_C_MASK);
}

/** Unset the C bit of the SPSR register (set to 0).
 * This method is analogous to SetSPSR_C(false).
 */
void 
CPU::
UnsetSPSR_C()
{
	unsigned int rm;
	
	rm = GetSPSRIndex();
	
	spsr[rm] = spsr[rm] & ~(SPSR_C_MASK);
}

/** Get the value of the SPSR C bit.
 *
 * @return the value of the SPSR register C bit
 */
bool 
CPU::
GetSPSR_C()
{
	unsigned int rm;
	
	rm = GetSPSRIndex();
	
	return (spsr[rm] & SPSR_C_MASK) == SPSR_C_MASK;
}

/** Set the V bit of the SPSR register.
 *
 * @param val the value of the V bit
 */
void 
CPU::
SetSPSR_V(const bool val)
{
	unsigned int rm;
	
	rm = GetSPSRIndex();
	
	if (val) spsr[rm] = spsr[rm] | SPSR_V_MASK;
	else spsr[rm] = spsr[rm] & ~(SPSR_V_MASK);
}

/** Unset the V bit of the SPSR register (set to 0).
 * This method is analogous to SetSPSR_V(false).
 */
void 
CPU::
UnsetSPSR_V()
{
	unsigned int rm;
	
	rm = GetSPSRIndex();
	
	spsr[rm] = spsr[rm] & ~(SPSR_V_MASK);
}

/** Get the value of the SPSR V bit.
 *
 * @return the value of the SPSR register V bit
 */
bool 
CPU::
GetSPSR_V()
{
	unsigned int rm;

	rm = GetSPSRIndex();
	
	return (spsr[rm] & SPSR_V_MASK) == SPSR_V_MASK;
}

/** Set the Q bit of the SPSR register.
 *
 * @param val the value of the Q bit
 */
void 
CPU::
SetSPSR_Q(const bool val)
{
	unsigned int rm;
	
	rm = GetSPSRIndex();
	
	if (val) spsr[rm] = spsr[rm] | SPSR_Q_MASK;
	else spsr[rm] = spsr[rm] & ~(SPSR_Q_MASK);
}

/** Unset the Q bit of the SPSR register (set to 0).
 * This method is analogous to SetSPSR_Q(false).
 */
void 
CPU::
UnsetSPSR_Q()
{
	unsigned int rm;
	
	rm = GetSPSRIndex();
	
	spsr[rm] = spsr[rm] & ~(SPSR_Q_MASK);
}

/** Get the value of the SPSR Q bit.
 *
 * @return the value of the SPSR register Q bit
 */
bool 
CPU::
GetSPSR_Q()
{
	unsigned int rm;
	
	rm = GetSPSRIndex();
	
	return (spsr[rm] & SPSR_Q_MASK) == SPSR_Q_MASK;
}

/** Set the I bit of the SPSR register.
 *
 * @param val the value of the I bit
 */
void 
CPU::
SetSPSR_I(const bool val)
{
	unsigned int rm;
	
	rm = GetSPSRIndex();
	
	if (val) spsr[rm] = spsr[rm] | SPSR_I_MASK;
	else spsr[rm] = spsr[rm] & ~(SPSR_I_MASK);
}

/** Unset the I bit of the SPSR register (set to 0).
 * This method is analogous to SetSPSR_I(false).
 */
void 
CPU::
UnsetSPSR_I()
{
	unsigned int rm;
	
	rm = GetSPSRIndex();
	
	spsr[rm] = spsr[rm] & ~(SPSR_I_MASK);
}

/** Get the value of the SPSR I bit.
 *
 * @return the value of the SPSR register I bit
 */
bool 
CPU::
GetSPSR_I()
{
	unsigned int rm;
	
	rm = GetSPSRIndex();
	
	return (spsr[rm] & SPSR_I_MASK) == SPSR_I_MASK;
}

/** Set the F bit of the SPSR register.
 *
 * @param val the value of the F bit
 */
void 
CPU::
SetSPSR_F(const bool val)
{
	unsigned int rm;
	
	rm = GetSPSRIndex();
	
	if (val) spsr[rm] = spsr[rm] | SPSR_F_MASK;
	else spsr[rm] = spsr[rm] & ~(SPSR_F_MASK);
}

/** Unset the F bit of the SPSR register (set to 0).
 * This method is analogous to SetSPSR_F(false).
 */
void 
CPU::
UnsetSPSR_F()
{
	unsigned int rm;
	
	rm = GetSPSRIndex();

	spsr[rm] = spsr[rm] & ~(SPSR_F_MASK);
}

/** Get the value of the SPSR F bit.
 * 
 * @return the value of the SPSR register F bit
 */
bool 
CPU::
GetSPSR_F()
{
	unsigned int rm;

	rm = GetSPSRIndex();
	
	return (spsr[rm] & SPSR_F_MASK) == SPSR_F_MASK;
}

/** Set the T bit of the SPSR register (set to 0).
 *
 * @param val the value of the T bit
 */
void 
CPU::
SetSPSR_T(const bool val)
{
	unsigned int rm;
	
	rm = GetSPSRIndex();
	
	if (val) spsr[rm] = spsr[rm] | SPSR_T_MASK;
	else spsr[rm] = spsr[rm] & ~(SPSR_T_MASK);
}

/** Unset the T bit of the SPSR register (set to 0).
 * This method is analogous to SetSPSR_T(false).
 */
void 
CPU::
UnsetSPSR_T()
{
	unsigned int rm;

	rm = GetSPSRIndex();

	spsr[rm] = spsr[rm] & ~(SPSR_T_MASK);
}

/** Get the value of the SPSR T bit.
 *
 * @return the value of the SPSR register T bit
 */
bool 
CPU::
GetSPSR_T()
{
	unsigned int rm;

	rm = GetSPSRIndex();

	return (spsr[rm] & SPSR_T_MASK) == SPSR_T_MASK;
}

/** Set the mode value of the SPSR register.
 *
 * @param mode the mode to set in the SPSR register
 */
void 
CPU::
SetSPSR_Mode(uint32_t mode)
{
	unsigned int rm;
	
	rm = GetSPSRIndex();
	
	spsr[rm] = spsr[rm] & ~(SPSR_RUNNING_MODE_MASK);
	spsr[rm] = spsr[rm] | mode;
}

/** Get the mode value of the SPSR register.
 *
 * @return the mode value of the SPSR register
 */
uint32_t 
CPU::
GetSPSR_Mode()
{
	unsigned int rm;

	rm = GetSPSRIndex();

	return spsr[rm] & (SPSR_RUNNING_MODE_MASK);
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

	switch (cpsr & CPSR_RUNNING_MODE_MASK) 
	{
		case USER_MODE:
			assert(USER_MODE != USER_MODE);
			break;
		case SYSTEM_MODE:
			assert(SYSTEM_MODE != SYSTEM_MODE);
			break;
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
	uint32_t src_mode = GetCPSR_Mode();
	uint32_t dst_mode = GetSPSR_Mode();
	uint32_t cur_spsr = GetSPSR();
	SetCPSR(cur_spsr);
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


