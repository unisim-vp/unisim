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
 
#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_COPROCESSOR_ARM966E_S_CP15_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_COPROCESSOR_ARM966E_S_CP15_HH__

#include "unisim/component/cxx/processor/arm/coprocessor_interface.hh"
#include "unisim/component/cxx/processor/arm/tcm/tcm.hh"
#include "unisim/component/cxx/processor/arm/cache_interface.hh"

#ifndef SOCLIB

#include "unisim/kernel/service/service.hh"
#include "unisim/service/interfaces/memory.hh"

#endif // SOCLIB

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace arm {
namespace coprocessor {
namespace arm966e_s {

#ifndef SOCLIB

using unisim::kernel::service::Object;
using unisim::kernel::service::Parameter;

#endif // SOCLIB

using unisim::component::cxx::processor::arm::CacheInterface;
using unisim::component::cxx::processor::arm::CPInterface;
using unisim::component::cxx::processor::arm::tcm::DTCM;
using unisim::component::cxx::processor::arm::tcm::ITCM;

#ifdef SOCLIB

template<class CONFIG>
class CP15 :
	public CPInterface<CONFIG::DEBUG_ENABLE> {

#else // SOCLIB

template<class CONFIG>
class CP15 : 
	public Object,
	public CacheInterface<typename CONFIG::address_t>,
	public CPInterface<CONFIG::DEBUG_ENABLE> {

#endif // SOCLIB
	
private:
	typedef uint32_t reg_t;
	typedef CPInterface<CONFIG::DEBUG_ENABLE> inherited;
	typedef typename CONFIG::address_t address_t;
	
public:
	
#ifdef SOCLIB

	CP15(unsigned int _cp_id,
			CPUCPInterface *_cpu,
			DTCM<CONFIG> *_dtcm,
			ITCM<CONFIG> *_itcm,
			CacheInterface<address_t> *_memory_interface);
	
#else // SOCLIB
	
	CP15(const char *name,
			unsigned int _cp_id,
			CPUCPInterface *_cpu,
			DTCM<CONFIG> *_dtcm,
			ITCM<CONFIG> *_itcm,
			CacheInterface<address_t> *_memory_interface,
			Object *parent = 0);
	
	virtual bool Setup();
	
#endif // SOCLIB
	
	/** Gives the name of the Coprocessor component.
	 * @return The name of the componenet (constant character string pointer)
	 */
	virtual const char* Description();
	/** Gives the name of a specified register
	 * @param[in] id : unsigned 32 bits integer for register id 
	 * @return The name of the componenet (constant character string pointer)
	 */
	virtual const char* RegisterName(uint32_t id, uint32_t option = 0);
	/// Set coprocessor to reset state.
	virtual void Reset();
	/** Read the value of a register
	 * @param[in]  opcode1 : the "opcode1" field of the instruction code (unsigned 8 bits integer)
	 * @param[in]  opcode2 : the "opcode2" field of the instruction code ( unsigned 8 bits integer)
	 * @param[in]  crn     : the "crn" field of the instruction code (unsigned 8 bits integer)
	 * @param[in]  crm     : the "crm" field of the instruction code (unsigned 8 bits integer)
	 * @param[out] r       : pointer to a REGISTER variable that will receive the register value
	 */
	virtual void ReadRegister(uint8_t opcode1, 
			uint8_t opcode2, 
			uint8_t crn, 
			uint8_t crm, 
			reg_t& reg);
	/** Write a value in a register	
	 * @param[in] opcode1 : the "opcode1" field of the instruction code (unsigned 8 bits integer)
	 * @param[in] opcode2 : the "opcode2" field of the instruction code (unsigned 8 bits integer)
	 * @param[in] crn     : the "crn" field of the instruction code (unsigned 8 bits integer)
	 * @param[in] crm     : the "crm" field of the instruction code (unsigned 8 bits integer)
	 * @param[in] val     : value to be written in the register (REGISTER type)
	 */
	virtual void  WriteRegister(uint8_t opcode1,
			uint8_t opcode2, 
			uint8_t crn, 
			uint8_t crm, 
			reg_t value);
    /** Perform a coprocessor operation
     * @param[in] opcode1 : the "opcode1" field of the instruction code (unsigned 8 bits integer)
     * @param[in] opcode2 : the "opcode2" field of the instruction code (unsigned 8 bits integer)
     * @param[in] crd     : the "crd" field of the instruction code (unsigned 8 bits integer)
     * @param[in] crn     : the "crn" field of the instruction code (unsigned 8 bits integer)
     * @param[in] crm     : the "crm" field of the instruction code (unsigned 8 bits integer)
     */
    virtual void Operation(uint8_t opcode1,
    		uint8_t opcode2,
    		uint8_t crd,
    		uint8_t crn,
    		uint8_t crm);
    /** Perform a coprocessor load
     * @param[in] crd     : the "crd" field of the instruction code (unsigned 8 bits integer)
	 * @param[in] address : the address to load data from
     */
    virtual void Load(uint8_t crd,
    		reg_t address);
    /** Perform a coprocessor load
     * @param[in] crd     : the "crd" field of the instruction code (unsigned 8 bits integer)
	 * @param[in] address : the address to store the data to
     */
    virtual void Store(uint8_t crd,
    		reg_t address);

    /* Specific methods of the arm966e_s cp15 */
    /** Get the base address of the exception vector
     * @return the base address of the exception vector
     */
    address_t GetExceptionVectorAddr();
    
    /* Cache interface required methods */
	virtual void PrInvalidateBlock(uint32_t set, uint32_t way);
	virtual void PrFlushBlock(uint32_t set, uint32_t way);
	virtual void PrCleanBlock(uint32_t set, uint32_t way);
	virtual void PrReset();
	virtual void PrInvalidate();
	virtual void PrInvalidateSet(uint32_t set);
	virtual void PrInvalidateBlock(address_t addr);
	virtual void PrFlushBlock(address_t addr);
	virtual void PrCleanBlock(address_t addr);
	virtual void PrZeroBlock(address_t addr);
	virtual void PrWrite(address_t addr, const uint8_t *buffer, uint32_t size);
	virtual void PrRead(address_t addr, uint8_t *buffer, uint32_t size);
    
#ifndef SOCLIB
	
	// CP15 -> Memory Interface (for debugging purposes/non-intrusive accesses)
	virtual bool ReadMemory(uint64_t addr, void *buffer, uint32_t size);
	virtual bool WriteMemory(uint64_t addr, const void *buffer, uint32_t size);
	
#endif // SOCLIB

private:
	reg_t id_code_reg;
	reg_t tcm_size_reg;
	reg_t control_reg;
	reg_t core_reg;
	reg_t tpi_reg; // trace process identifier register
	reg_t conf_control_reg;
	reg_t bist_control_reg;
	reg_t ibist_addr_reg;
	reg_t ibist_gen_reg;
	reg_t dbist_addr_reg;
	reg_t dbist_gen_reg;
	
	DTCM<CONFIG> *dtcm;
	ITCM<CONFIG> *itcm;
	CacheInterface<address_t> *memory_interface;

	/* Private register methods */
	void InitRegs();
	void WriteControlReg(reg_t value);
	bool DTCMEnabled();
	bool ITCMEnabled();
	
	/* Private memory methods */
	bool IsTCMAddress(address_t addr);
	bool IsDTCMAddress(address_t addr);
	bool IsITCMAddress(address_t addr);
	void MemWrite(address_t addr, const uint8_t *buffer, uint32_t size);
	void MemRead(address_t addr, uint8_t *buffer, uint32_t size);
	void DTCMWrite(address_t addr, const uint8_t *buffer, uint32_t size);
	void DTCMRead(address_t addr, uint8_t *buffer, uint32_t size);
	void ITCMWrite(address_t addr, const uint8_t *buffer, uint32_t size);
	void ITCMRead(address_t addr, uint8_t *buffer, uint32_t size);
	
	// Parameters
	uint32_t silicon_revision_number;
	// Verbose parameters
	bool verbose_all;
	bool verbose_read_reg;
	bool verbose_write_reg;
	bool verbose_pr_read;
	bool verbose_pr_write;
	bool verbose_debug_read;
	bool verbose_debug_write;
	
#ifndef SOCLIB
	
	Parameter<uint32_t> param_silicon_revision_number;
	Parameter<bool> param_verbose_all;
	Parameter<bool> param_verbose_read_reg;
	Parameter<bool> param_verbose_write_reg;
	Parameter<bool> param_verbose_pr_read;
	Parameter<bool> param_verbose_pr_write;
	Parameter<bool> param_verbose_debug_read;
	Parameter<bool> param_verbose_debug_write;

	unisim::kernel::logger::Logger logger;

#endif // SOCLIB
	
	// verbose methods
	bool VerboseAll();
	bool VerboseReadReg();
	bool VerboseWriteReg();
	bool VerbosePrRead();
	bool VerbosePrWrite();
	bool VerboseDebugWrite();
	bool VerboseDebugRead();
};

} // end of namespace arm966e_s
} // end of namespace coprocessor
} // end of namespace arm
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_COPROCESSOR_ARM966E_S_CP15_HH__
