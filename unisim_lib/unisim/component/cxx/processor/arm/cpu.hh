/*
 *  Copyright (c) 2010-2013,
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
 * Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr), Yves Lhuillier (yves.lhuillier@cea.fr)
 */

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_CPU_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_CPU_HH__

#include "unisim/kernel/service/service.hh"
#include "unisim/kernel/logger/logger.hh"
#include "unisim/util/debug/register.hh"
#include "unisim/util/endian/endian.hh"
#include <string>
#include <map>
#include <assert.h>
#include <inttypes.h>

#ifdef GCC_INLINE
#undef GCC_INLINE
#endif

#if defined(__GNUC__) && (__GNUC__ >= 3)
#define GCC_INLINE __attribute__((always_inline))
#else
#define GCC_INLINE
#endif

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace arm {

/** Base class for the ARM family.
 * This class is the base for all the cpu's of the ARM processor family, for
 *   that purpose it defiens the basic registers and different methods to handle
 *   them.
 * It describes some pseudo virtual methods that specialized classes should 
 *   implement.
 * IMPORTANT: addresses are 32 bits (uint32_t), registers are 32 bits signed or
 *   unsigned (int32_t and uint32_t).
 * IMPORTANT: The arm instruction set requires a set of methods to access the
 *   memory subsystem, that this class doesn't define as pure virtual methods
 *   (as should have been done). Instead, the derived class should implement
 *   them directly, those functions are a kind of interface of the arm isa.
 *   This choice has been done to enhance runtime execution.
 *   The following is the list of methods that should be implemented:
 *
 * * reads 16bits instructions from the memory system
 * This method allows the user to read instructions from the memory system,
 *   that is, it tries to read from the pertinent caches and if failed from
 *   the external memory system.
 * This method is not really needed by the isa, but required to fetch the thumb
 *   instructions.
 *
 * @param address the address to read data from
 * @param val the buffer to fill with the read data
 *
 * void ReadInsn(uint32_t address, uint16_t &val);
 *
 * * reads 32bits instructions from the memory system
 * This method allows the user to read instructions from the memory system,
 *   that is, it tries to read from the pertinent caches and if failed from
 *   the external memory system.
 * This method is not really needed by the isa, but required to fetch the arm32
 *   instructions.
 *
 * @param address the address to read data from
 * @param val the buffer to fill with the read data
 *
 * void ReadInsn(uint32_t address, uint32_t &val);
 *
 * * Memory prefetch instruction.
 * This method is used to make memory prefetches into the caches (if available),
 *   that is it sends a memory read that doesn't keep the request result.
 *
 * @param address the address of the prefetch
 *
 * void ReadPrefetch(uint32_t address);
 *
 * * 32bits memory read that stores result into one of the general purpose 
 *  registers
 * This method reads 32bits from memory and return a pending memory
 *   operation
 *
 * @param address the base address of the 32bits read
 *
 * void MemRead32(uint32_t address);
 *
 * * 16bits aligned memory read that stores result into one of the general 
 *  purpose registers
 * This method reads 16bits from memory and return a pending memory
 *   operation. Note that this
 *   read methods supposes that the address is 16bits aligned.
 *
 * @param address the base address of the 16bits read
 *
 * void MemRead16(uint32_t address);
 *
 * * signed 16bits aligned memory read that stores result into one of the 
 *  general purpose registers
 * This method reads 16bits from memory and return a pending memory
 *   operation. Note that this
 *   read methods supposes that the address is 16bits aligned. The 16bits value
 *   is considered signed and sign extended to the register size 
 *
 * @param address the base address of the 16bits read
 *
 * void MemReadS16(uint32_t address);
 *
 * * 8bits memory read that stores result into one of the general purpose 
 *  registers
 * This method reads 8bits from memory and return a pending memory
 *   operation
 *
 * @param address the base address of the 8bits read
 *
 * void MemReadS8(uint32_t address);
 *
 * * signed 8bits memory read that stores result into one of the general purpose
 *  registers
 * This method reads 8bits from memory and return a pending memory
 *   operation. The 8bits value
 *   is considered signed and sign extended to the register size
 *
 * @param address the base address of the 8bits read
 *
 * void MemRead8(uint32_t address);
 *
 * * 32bits memory write.
 * This method write the giving 32bits value into the memory system.
 *
 * @param address the base address of the 32bits write
 * @param value the value to write into memory
 *
 * void MemWrite32(uint32_t address, uint32_t value);
 *
 * * 16bits memory write.
 * This method write the giving 16bits value into the memory system.
 *
 * @param address the base address of the 16bits write
 * @param value the value to write into memory
 *
 * void MemWrite16(uint32_t address, uint16_t value);
 *
 * * 8bits memory write.
 * This method write the giving 8bits value into the memory system.
 *
 * @param address the base address of the 8bits write
 * @param value the value to write into memory
 *
 * void MemWrite8(uint32_t address, uint8_t value);
 *
 * * Unpredictable Instruction Behaviour.
 * This method is just called when an unpredictable behaviour is detected to
 *   notifiy the processor.
 *
 * void UnpredictableInsnBehaviour();
 */

class CPU
  : public virtual unisim::kernel::service::Object
{
public:
  //=====================================================================
  //=                    Constructor/Destructor                         =
  //=====================================================================
	
  /** Constructor.
   * Resets the simulator state, requires the endianness of the processor
   *   to set address mungling. By default the processor is set to big
   *   endian, but it can be changed later.
   *
   * @param endianness the endianness to use
   */
  CPU( char const* name, Object* parent = 0,
       unisim::util::endian::endian_type endianness =
       unisim::util::endian::E_BIG_ENDIAN
       );
  /** Destructor.
   */
  ~CPU();
  
  //=====================================================================
  //=                       Logger                                      =
  //=====================================================================
  
  /** Unisim logging services. */
  unisim::kernel::logger::Logger logger;
  
  /**************************************************************/
  /* Endian variables and methods                         START */
  /**************************************************************/

  /** The endian of the processor.
   */
  unisim::util::endian::endian_type default_endianness;

  /** Set the endianness of the processor to the given endianness.
   * This method takes into charge to set the correct address mungling 
   *   depending on the endianness.
   *
   * @param endianness the endianness to use
   */
  void SetEndianness(unisim::util::endian::endian_type endianness);
  /** Get the endian configuration of the processor.
   *
   * @return the endian being used
   */
  unisim::util::endian::endian_type GetEndianness() const
  {
	  return default_endianness;
  }

  /**************************************************************/
  /* Endian variables and methods                           END */
  /**************************************************************/

  /**************************************************************/
  /* Registers access methods    START                          */
  /**************************************************************/
		
  /* GPR access functions */
  /** Arrange the GPR mapping depending on initial and target running mode.
   *
   * @param src_mode the running mode the processor is currently in
   * @param tar_mode the target running mode the registers should be mapped to
   */
  void SetGPRMapping(uint32_t src_mode, uint32_t tar_mode);
  /** Get the value contained by a GPR.
   *
   * @param id the register index
   * @return the value contained by the register
   */
  uint32_t GetGPR(uint32_t id) const
  {
    return gpr[id];
  }

  /** Get the value contained by a GPR, excluding PC.
   *
   * @param id the register index
   * @return the value contained by the register
   */
  uint32_t GetGPR_npc(uint32_t id) const
  {
    assert(id != 15); /* Should be unpredictable*/
    return gpr[id];
  }

  /** Assign a GPR with a value coming from the Execute stage.  In
   * architectures up to ARMv6, this is not interworking (simple
   * branch when destination register is PC).
   *
   * @param id the register index
   * @param val the value to set
   */
  void SetGPR(uint32_t id, uint32_t val)
  {
    if (id != 15) gpr[id] = val;
    else this->Branch( val );
  }
	
  /** Assign a GPR with a value coming from the Memory stage.  In
   * architectures from to ARMv5T, this is interworking (exchanging
   * branch when destination register is PC).
   *
   * @param id the register index
   * @param val the value to set
   */
  void SetGPR_mem(uint32_t id, uint32_t val)
  {
    if (id != 15) gpr[id] = val;
    else this->BranchExchange( val );
  }
	
  /** Set the value contained by a GPR, excluding PC.
   *
   * @param id the register index
   * @param val the value to set
   */
  void SetGPR_npc(uint32_t id, uint32_t val)
  {
    assert(id != 15);
    gpr[id] = val;
  }
  
  /** Sets the PC (and potentially exchanges mode ARM<->Thumb)
   *
   * @param val the value to set PC
   */
  void BranchExchange(uint32_t val)
  { this->next_pc = val; this->SetCPSR_T( val & 1 ); }
	
  /** Sets the PC (and preserve mode)
   *
   * @param val the value to set PC
   */
  void Branch(uint32_t val)
  { this->next_pc = (this->next_pc & 1) | (val & -2); }
	
  /** Gets the updated PC value (next PC as currently computed)
   *
   */
  uint32_t GetNPC()
  { return this->next_pc; }
	
  /** Get the value contained by a user GPR.
   * Returns the value contained by a user GPR. It is the same than GetGPR but
   *   restricting the index from 0 to 15 (only the first 16 registers).
   *
   * @param id the register index
   * @return the value contained by the register
   */
  uint32_t GetGPR_usr(uint32_t id) const;
  /** Set the value contained by a user GPR.
   * Sets the value contained by a user GPR. It is the same than SetGPR byt
   *   restricting the index from 0 to 15 (only the first 16 registers).
   *
   * @param id the register index
   * @param val the value to set
   */
  void SetGPR_usr(uint32_t id, uint32_t val);
  
  /* CPSR access functions */
  /** Get the value of the CPSR register.
   *
   * @return the value of the CPSR register.
   */
  uint32_t GetCPSR() const;
  /** Get the NZCV bits of the CPSR register
   * 
   * @return the NZCV bits of the CPSR register
   */
  uint32_t GetCPSR_NZCV() const { return (cpsr >> 28) & 0xf; }
	
  /** Set the value of the CPSR register.
   *
   * @param val the value to set
   */
  void SetCPSR(uint32_t val);
  /** Set the value of the NZCV bits of the CPSR register.
   *
   * @param n the N bit
   * @param z the Z bit
   * @param c the C bit
   * @param v the V bit
   */
  void SetCPSR_NZCV(bool n,
                    bool z,
                    bool c,
                    bool v);
  /** Set the N bit of the CPSR register.
   *
   * @param val the value of the N bit
   */
  void SetCPSR_N(const bool val = true);
  /** Unset the N bit of the CPSR register (set to 0).
   * This method is analagous to SetCPSR_N(false).
   */
  void UnsetCPSR_N();
  /** Get the value of the CPSR register N bit.
   *
   * @return the value of the CPSR register N bit
   */
  bool GetCPSR_N() const;
  /** Set the Z bit of the CPSR register.
   *
   * @param val the value of the Z bit
   */
  void SetCPSR_Z(const bool val = true);
  /** Unset the Z bit of the CPSR register (set to 0).
   * This method is analogous to SetCPSR_Z(false).
   */
  void UnsetCPSR_Z();
  /** Get the value of the CPSR register Z bit.
   *
   * @return the value of the CPSR register Z bit
   */
  bool GetCPSR_Z() const;
  /** Set the C bit of the CPSR register.
   *
   * @param val the value of the C bit
   */
  void SetCPSR_C(const bool val = true);
  /** Unset the C bit of the CPSR register (set to 0).
   * This method is analogous to SetCPSR_C(false).
   */
  void UnsetCPSR_C();
  /** Get the value of the CPSR register C bit.
   *
   * @return the value of the CPSR register C bit
   */
  bool GetCPSR_C() const;
  /** Set teh V bit of the CPSR register.
   *
   * @param val the value of the V bit
   */
  void SetCPSR_V(const bool val = true);
  /** Unset the V bit of the CPSR register (set to 0).
   * This method is analogous to SetCPSR_V(false).
   */
  void UnsetCPSR_V();
  /** Get the value of the CPSR register V bit.
   *
   * @return the value of the CPSR register V bit
   */
  bool GetCPSR_V() const;
  /** Set the Q bit of the CPSR register.
   *
   * @param val teh value of the Q bit
   */
  void SetCPSR_Q(const bool val = true);
  /** Unset the Q bit of the CPSR register (set to 0).
   * This method is analogous to SetCPSR_Q(false).
   */
  void UnsetCPSR_Q();
  /** Get the value of the CPSR register Q bit.
   * 
   * @return the value of the CPSR register Q bit
   */
  bool GetCPSR_Q();
  /** Set the I bit of the CPSR register.
   *
   * @param val the value of the I bit
   */
  void SetCPSR_I(const bool val = true);
  /** Unset the I bit of the CPSR register (set to 0).
   * This method is analogous to SetCPSR_I(false).
   */
  void UnsetCPSR_I();
  /** Get the value of the CPSR register I bit.
   *
   * @return the value of the CPSR register I bit
   */
  bool GetCPSR_I();
  /** Set the F bit of the CPSR register.
   *
   * @param val the value of the F bit
   */
  void SetCPSR_F(const bool val = true);
  /** Unset the F bit of the CPSR register (set to 0).
   * This method is analogous to SetCPSR_F(false).
   */
  void UnsetCPSR_F();
  /** Get the value of the CPSR register F bit.
   *
   * @return the value of the CPSR register F bit
   */
  bool GetCPSR_F();
  /** Set the T bit of the CPSR register.
   *
   * @param val the value of the T bit
   */
  void SetCPSR_T(const bool val = true);
  /** Unset the T bit of the CPSR register (set to 0).
   * This method is analogous to SetCPSR_T(false).
   */
  void UnsetCPSR_T();
  /** Get the value of the CPSR register T bit.
   *
   * @return the value of the CPSR register T bit.
   */
  bool GetCPSR_T();
  /** Set the mode value of the CPSR register.
   *
   * @param mode the mode to set in the CPSR register
   */
  void SetCPSR_Mode(uint32_t mode);
  /** Get the mode value of the CPSR register.
   *
   * @return the mode value of the CPSR register
   */
  uint32_t GetCPSR_Mode() const;
		
  /* SPSR access functions */
  /** Get the value of the CPSR register.
   *
   * @return the value of the CPSR register
   */
  uint32_t GetSPSR();
  /** Set the value of the SPSR register.
   *
   * @param val the value to set
   */
  void SetSPSR(uint32_t val);
  /** Set the value of the NZCV bits of the SPSR register.
   *
   * @param n the N bit
   * @param z the Z bit
   * @param c the C bit
   * @param v the V bit
   */
  void SetSPSR_NZCV(bool n,
                    bool z,
                    bool c,
                    bool v);
  /** Set the N bit of the SPSR register.
   *
   * @param val the value of the N bit
   */
  void SetSPSR_N(const bool val = true);
  /** Unset the N bit of the SPSR register (set to 0).
   * This method is analogous to SetSPSR_N(false).
   */
  void UnsetSPSR_N();
  /** Get the value of the SPSR register N bit.
   *
   * @return the value of the SPSR register N bit.
   */
  bool GetSPSR_N();
  /** Set the Z bit of the SPSR register.
   *
   * @param val the value of the Z bit
   */
  void SetSPSR_Z(const bool val = true);
  /** Unset the Z bit of the SPSR register (set to 0).
   * This method is analogous to SetSPSR_Z(false).
   */
  void UnsetSPSR_Z();
  /** Get the value of the SPSR Z bit.
   *
   * @return the value of the SPSR register Z bit
   */
  bool GetSPSR_Z();
  /** Set the C bit of the SPSR register.
   *
   * @param val the value of the C bit
   */
  void SetSPSR_C(const bool val = true);
  /** Unset the C bit of the SPSR register (set to 0).
   * This method is analogous to SetSPSR_C(false).
   */
  void UnsetSPSR_C();
  /** Get the value of the SPSR C bit.
   *
   * @return the value of the SPSR register C bit
   */
  bool GetSPSR_C();
  /** Set the V bit of the SPSR register.
   *
   * @param val the value of the V bit
   */
  void SetSPSR_V(const bool val = true);
  /** Unset the V bit of the SPSR register (set to 0).
   * This method is analogous to SetSPSR_V(false).
   */
  void UnsetSPSR_V();
  /** Get the value of the SPSR V bit.
   *
   * @return the value of the SPSR register V bit
   */
  bool GetSPSR_V();
  /** Set the Q bit of the SPSR register.
   *
   * @param val the value of the Q bit
   */
  void SetSPSR_Q(const bool val = true);
  /** Unset the Q bit of the SPSR register (set to 0).
   * This method is analogous to SetSPSR_Q(false).
   */
  void UnsetSPSR_Q();
  /** Get the value of the SPSR Q bit.
   *
   * @return the value of the SPSR register Q bit
   */
  bool GetSPSR_Q();
  /** Set the I bit of the SPSR register.
   *
   * @param val the value of the I bit
   */
  void SetSPSR_I(const bool val = true);
  /** Unset the I bit of the SPSR register (set to 0).
   * This method is analogous to SetSPSR_I(false).
   */
  void UnsetSPSR_I();
  /** Get the value of the SPSR I bit.
   *
   * @return the value of the SPSR register I bit
   */
  bool GetSPSR_I();
  /** Set the F bit of the SPSR register.
   *
   * @param val the value of the F bit
   */
  void SetSPSR_F(const bool val = true);
  /** Unset the F bit of the SPSR register (set to 0).
   * This method is analogous to SetSPSR_F(false).
   */
  void UnsetSPSR_F();
  /** Get the value of the SPSR F bit.
   * 
   * @return the value of the SPSR register F bit
   */
  bool GetSPSR_F();
  /** Set the T bit of the SPSR register (set to 0).
   *
   * @param val the value of the T bit
   */
  void SetSPSR_T(const bool val = true);
  /** Unset the T bit of the SPSR register (set to 0).
   * This method is analogous to SetSPSR_T(false).
   */
  void UnsetSPSR_T();
  /** Get the value of the SPSR T bit.
   *
   * @return the value of the SPSR register T bit
   */
  bool GetSPSR_T();
  /** Set the mode value of the SPSR register.
   *
   * @param mode the mode to set in the SPSR register
   */
  void SetSPSR_Mode(uint32_t mode);
  /** Get the mode value of the SPSR register.
   *
   * @return the mode value of the SPSR register
   */
  uint32_t GetSPSR_Mode();

  /** Get SPSR index from current running mode
   *
   * @return the SPSR index from current running mode
   */
  uint32_t GetSPSRIndex();

  /** Copy the value of current SPSR register into CPSR.
   */
  void MoveSPSRtoCPSR();

  /**************************************************************/
  /* Registers access methods    END                            */
  /**************************************************************/

  /* TODO: this method needs to be implemented. Should it be removed? */
  /** Check that an address is aligned
   */
  void CheckAlignmentExcep(uint32_t addr);
  /* END TODO */
	
  /** Mark an exception in the virtual exception vector.
   * This marks an new exception in the virtual exception vector for 
   *   later treatment.
   *   NOTE: exception types are available at cxx/processor/arm/exception.hh
   *
   * @param except the exception to mark
   */
  void MarkVirtualExceptionVector(uint32_t except);

  /** Get the virtual exception vector.
   * This returns the value of the virtual exception vector.
   *
   * @return the value of the exception vector
   */
  uint32_t GetVirtualExceptionVector();

  /** Reset the value of the virtual exception vector.
   *
   * @param mask the value to set at reset
   */
  void ResetVirtualExceptionVector(uint32_t mask = 0);
  
  /*
   * ARM architecture constants
   */
    
  /* specific register indices */
  static unsigned int const PC_reg = 15;
  static unsigned int const LR_reg = 14;
  static unsigned int const SP_reg = 13;

  /* masks for the different running modes */
  static uint32_t const RUNNING_MODE_MASK = 0x1F;
  static uint32_t const USER_MODE = 0x10;
  static uint32_t const FIQ_MODE = 0x11;
  static uint32_t const IRQ_MODE = 0x12;
  static uint32_t const SUPERVISOR_MODE = 0x13;
  static uint32_t const ABORT_MODE = 0x17;
  static uint32_t const UNDEFINED_MODE = 0x1B;
  static uint32_t const SYSTEM_MODE = 0x1F;
  /* masks for the different CPSR status bits */
  static uint32_t const CPSR_N_MASK = 0x80000000;
  static uint32_t const CPSR_Z_MASK = 0x40000000;
  static uint32_t const CPSR_C_MASK = 0x20000000;
  static uint32_t const CPSR_V_MASK = 0x10000000;
  static uint32_t const CPSR_Q_MASK = 0x08000000;
  static uint32_t const CPSR_I_MASK = 0x00000080;
  static uint32_t const CPSR_F_MASK = 0x00000040;
  static uint32_t const CPSR_T_MASK = 0x00000020;
  static uint32_t const CPSR_RUNNING_MODE_MASK = 0x0000001F;
  /* masks for the different SPSR status bits */
  static uint32_t const SPSR_N_MASK = CPSR_N_MASK;
  static uint32_t const SPSR_Z_MASK = CPSR_Z_MASK;
  static uint32_t const SPSR_C_MASK = CPSR_C_MASK;
  static uint32_t const SPSR_V_MASK = CPSR_V_MASK;
  static uint32_t const SPSR_Q_MASK = CPSR_Q_MASK;
  static uint32_t const SPSR_I_MASK = CPSR_I_MASK;
  static uint32_t const SPSR_F_MASK = CPSR_F_MASK;
  static uint32_t const SPSR_T_MASK = CPSR_T_MASK;
  static uint32_t const SPSR_RUNNING_MODE_MASK = CPSR_RUNNING_MODE_MASK;
  /* values of the different condition codes */
  static uint32_t const COND_EQ = 0x00;
  static uint32_t const COND_NE = 0x01;
  static uint32_t const COND_CS_HS = 0x02;
  static uint32_t const COND_CC_LO = 0x03;
  static uint32_t const COND_MI = 0x04;
  static uint32_t const COND_PL = 0x05;
  static uint32_t const COND_VS = 0x06;
  static uint32_t const COND_VC = 0x07;
  static uint32_t const COND_HI = 0x08;
  static uint32_t const COND_LS = 0x09;
  static uint32_t const COND_GE = 0x0a;
  static uint32_t const COND_LT = 0x0b;
  static uint32_t const COND_GT = 0x0c;
  static uint32_t const COND_LE = 0x0d;
  static uint32_t const COND_AL = 0x0e;

protected:
  /*
   * Memory access variables
   */
  /**
   * This variable is used to compute the effective address of a 1 byte memory
   * access using the arm endianess mapping (only if os support is not 
   * connected to the simulator. 
   * This variable is set during the setup face or later when the endianess of
   * the processor is changed.
   */
  uint32_t munged_address_mask8;
  /**
   * This variable is used to compute the effective address of a 2 byte memory
   * access using the arm endianess mapping (only if os support is not 
   * connected to the simulator.
   * This variable is set during the setup face or later when the endianess of
   * the processor is changed.
   */
  uint32_t munged_address_mask16;
		
  /** The registers interface for debugging purpose */
  std::map<std::string, unisim::util::debug::Register *> registers_registry;
		
  /** The total number of physical registers.
   * The arm has only 31 registers, but we are using an 
   *   additional one to store the NextPC, which does not really
   *   exist */
  const static uint32_t num_phys_gprs = 32;
  /** The total number of logical registers.
   * At any given running mode only 16 registers are accessible.
   * The following list indicates the indexes used per running mode:
   * - user:           0-14 (R0-R14),                  15 (PC)
   * - system:         0-14 (R0-R14),                  15 (PC)
   * - supervisor:     0-12 (R0-R12), 16-17 (R13-R14), 15 (PC)
   * - abort:          0-12 (R0-R12), 18-19 (R13-R14), 15 (PC)
   * - undefined:      0-12 (R0-R12), 20-21 (R13-R14), 15 (PC)
   * - interrupt:      0-12 (R0-R12), 22-23 (R13-R14), 15 (PC)
   * - fast interrupt: 0-7 (R0-R7),   24-30 (R8-R14),  15 (PC)
   */
  const static uint32_t num_log_gprs = 16;
  /** Storage for the physical registers */
  uint32_t phys_gpr[num_phys_gprs]; 
  /** Storage for the logical registers */
  uint32_t gpr[num_log_gprs];
  uint32_t current_pc, next_pc;
  /** The CPSR register.
   * All the running modes share the same CPSR register
   * CPSR organization:
   * - bit 31: N
   * - bit 30: Z
   * - bit 29: C
   * - bit 28: V
   * - bit 27: Q
   * - bits 26-8: DNM(RAZ) not used in current arm versions
   * - bit 7: I
   * - bit 6: F
   * - bit 5: T
   * - bits 4-0: M bits, running mode, see bellow
   * Bits 4 to 0 indicate the processor current running mode.
   *   Accepted running modes are:
   * - 0b10000: User
   * - 0b10001: FIQ (Fast Interrupt)
   * - 0b10010: IRQ (Interrupt) 
   * - 0b10011: Supervisor
   * - 0b10111: Abort
   * - 0b11011: Undefined
   * - 0b11111: System
   */
  uint32_t cpsr;
  /** Number of SPSR registers.
   * Privileged modes have private SPSR registers, the following is
   *   the organization per running mode:
   * - user:           --
   * - system:         --
   * - supervisor:     0
   * - abort:          1
   * - undefined:      2
   * - interrupt:      3
   * - fast interrupt: 4
   */
  const static uint32_t num_phys_spsrs = 5;
  /** The SPSR registers storage.
   */
  uint32_t spsr[5];
		
  /* TODO: check if the fake floating point registers could be removed. */
  uint64_t fake_fpr[8];
  uint32_t fake_fps;
  /* END TODO */

  /** Exception vector.
   * This is a virtual exception vector (it doesn't exists as such in the arm
   *   architecture) to rapidly set and check exceptions.
   *   NOTE: exceptions are defined at cxx/arm/exception.hh
   */
  uint32_t exception;
};
	
} // end of namespace arm
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_CPU_HH__

