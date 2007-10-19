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

#include "unisim/kernel/service/service.hh"

#include "unisim/component/cxx/processor/arm/coprocessor_interface.hh"

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace arm {
namespace coprocessor {
namespace arm966e_s {

using unisim::kernel::service::Object;
using unisim::kernel::service::Parameter;

using unisim::component::cxx::processor::arm::CPInterface;

template<class CONFIG>
class CP15 : 
	public CPInterface<CONFIG::DEBUG_ENABLE> {
private:
	typedef uint32_t reg_t;
	typedef CPInterface<CONFIG::DEBUG_ENABLE> inherited;
		
public:
	CP15(const char *name,
			unsigned int _cp_id,
			CPUCPInterface *_cpu,
			Object *parent = 0);	
	virtual bool Setup();
	
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
	
	void InitRegs();
	void WriteControlReg(reg_t value);
	
	// Parameters
	uint32_t silicon_revision_number;
	Parameter<uint32_t> param_silicon_revision_number;
	// Verbose parameters
	bool verbose_all;
	Parameter<bool> param_verbose_all;
	bool verbose_read_reg;
	Parameter<bool> param_verbose_read_reg;
	bool verbose_write_reg;
	Parameter<bool> param_verbose_write_reg;
	
	// verbose methods
	bool VerboseAll();
	bool VerboseReadReg();
	bool VerboseWriteReg();
	
};

} // end of namespace arm966e_s
} // end of namespace coprocessor
} // end of namespace arm
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_COPROCESSOR_ARM966E_S_CP15_HH__
