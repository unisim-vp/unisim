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
 
#ifndef __UNISIM_COMPONENT_TLM2_CHIPSET_ARM926EJS_PXP_VIC_VIC_HH__
#define __UNISIM_COMPONENT_TLM2_CHIPSET_ARM926EJS_PXP_VIC_VIC_HH__

#include <systemc.h>
#include <tlm.h>
#include <tlm_utils/passthrough_target_socket.h>
#include "unisim/kernel/service/service.hh"
#include "unisim/kernel/logger/logger.hh"
#include "unisim/component/tlm2/chipset/arm926ejs_pxp/vic/vic_int_source_identifier.hh"
#include <inttypes.h>

namespace unisim {
namespace component {
namespace tlm2 {
namespace chipset {
namespace arm926ejs_pxp {
namespace vic {

class VIC
	: public unisim::kernel::service::Object
	, public sc_module
	, public VICIntSourceIdentifierInterface
{
public:
	typedef tlm::tlm_base_protocol_types::tlm_payload_type transaction_type;
	typedef tlm::tlm_base_protocol_types::tlm_phase_type   phase_type;
	typedef tlm::tlm_sync_enum                             sync_enum_type;

	sc_core::sc_in<bool> vicintsource[32];
	sc_core::sc_in<bool> nvicfiqin;
	sc_core::sc_in<bool> nvicirqin;
	sc_core::sc_in<uint32_t> vicvectaddrin;

	sc_core::sc_out<bool> nvicfiq;
	sc_core::sc_out<bool> nvicirq;
	sc_core::sc_out<uint32_t> vicvectaddrout;

	/** Target socket for the bus connection */
	tlm_utils::passthrough_target_socket<VIC, 32>
		bus_target_socket;

	SC_HAS_PROCESS(VIC);
	VIC(const sc_module_name &name, Object *parent = 0);
	~VIC();

	virtual bool Setup();

private:
	/** Update the status of the VIC depending on all the possible entries
	 */
	void UpdateStatus();

	/** total number of incomming interrupts */
	static const int NUM_SOURCE_INT = 32;
	/** total number of vectored interrupts */
	static const int NUM_VECT_INT = 16;

	/** Registers storage */
	uint8_t regs[0x01000UL];

	/** Value of the source identifier */
	uint32_t int_source;
	/** Source interrupt handling */
	virtual void VICIntSourceReceived(int id, bool value);
	/** Source identifiers methods */
	VICIntSourceIdentifier *source_identifier_method[NUM_SOURCE_INT];

	/** value of the nVICFIQ */
	bool nvicfiq_value;
	/** value of the nVICIRQ */
	bool nvicirq_value;

	/**************************************************************************/
	/* Virtual methods for the target socket for the bus connection     START */
	/**************************************************************************/

	sync_enum_type bus_target_nb_transport_fw(transaction_type &trans,
			phase_type &phase,
			sc_core::sc_time &time);
	void bus_target_b_transport(transaction_type &trans, 
			sc_core::sc_time &delay);
	bool bus_target_get_direct_mem_ptr(transaction_type &trans, 
			tlm::tlm_dmi &dmi_data);
	unsigned int bus_target_transport_dbg(transaction_type &trans);

	/**************************************************************************/
	/* Virtual methods for the target socket for the bus connection       END */
	/**************************************************************************/

	/**************************************************************************/
	/* Methods to get and set the registers                             START */
	/**************************************************************************/

	static const uint32_t VICIRQSTATUSAddr      = 0x0000UL;
	static const uint32_t VICFIQSTATUSAddr      = 0x0004UL;
	static const uint32_t VICRAWINTRAddr        = 0x0008UL;
	static const uint32_t VICINTSELECTAddr      = 0x000cUL;
	static const uint32_t VICINTENABLEAddr      = 0x0010UL;
	static const uint32_t VICINTENCLEARAddr     = 0x0014UL;
	static const uint32_t VICSOFTINTAddr        = 0x0018UL;
	static const uint32_t VICSOFTINTCLEARAddr   = 0x001cUL;
	static const uint32_t VICPROTECTIONAddr     = 0x0020UL;
	static const uint32_t VICVECTADDRAddr       = 0x0030UL;
	static const uint32_t VICDEFVECTADDRAddr    = 0x0034UL;
	static const uint32_t VICVECTADDRBaseAddr   = 0x0100UL;
	static const uint32_t VICVECTCNTLBaseAddr   = 0x0200UL;
	static const uint32_t VICPERIPHIDBaseAddr   = 0x0fe0UL;
	static const uint32_t VICPCELLIDBaseAddr    = 0x0ff0UL;

	static const uint32_t VICITCRAddr           = 0x0300UL;
	static const uint32_t VICITIP1Addr          = 0x0304UL;
	static const uint32_t VICITIP2Addr          = 0x0308UL;
	static const uint32_t VICITOP1Addr          = 0x030cUL;
	static const uint32_t VICITOP2Addr          = 0x0310UL;

	/** Returns the enable field of the VICVECTCNTL register.
	 *
	 * @param value the register value
	 * @return true if enable is set, false otherwise
	 */
	bool VECTCNTLEnable(uint32_t value);
	/** Returns the source field of the VICVECTCNTL register
	 *
	 * @param value the register value
	 * @return the value of the source field
	 */
	uint32_t VECTCNTLSource(uint32_t value);

	/** Returns the register pointed by the given address
	 *
	 * @param addr the address to consider
	 * @return the value of the register pointed by the address
	 */
	uint32_t GetRegister(uint32_t addr) const;
	/** Sets the register pointed by the given address
	 *
	 * @param addr the address to consider
	 * @param value the value to set the register
	 */
	void SetRegister(uint32_t addr, uint32_t value);

	/** Returns the IRQ status register
	 *
	 * @return the value of the IRQ status register
	 */
	uint32_t GetVICIRQSTATUS() const;
	/** Sets the IRQ status register
	 *
	 * @param value the value to set
	 */
	void SetVICIRQSTATUS(uint32_t value);

	/** Returns the FIQ status register
	 *
	 * @return the value of the FIQ status register
	 */
	uint32_t GetVICFIQSTATUS() const;
	/** Sets the FIQ status register
	 *
	 * @param value the value to set
	 */
	void SetVICFIQSTATUS(uint32_t value);

	/** Returns the raw interrupt register
	 *
	 * @return the value of the raw interrupt register
	 */
	uint32_t GetVICRAWINTR() const;
	/** Sets the raw interrupt register
	 *
	 * @param value the value to set
	 */
	void SetVICRAWINTR(uint32_t value);

	/** Returns the interrupt select register
	 *
	 * @return the value of the interrupt select register
	 */
	uint32_t GetVICINTSELECT() const;
	/** Sets the interrupt select register
	 *
	 * @param value the value to set
	 */
	void SetVICINTSELECT(uint32_t value);

	/** Returns the interrupt enable register
	 *
	 * @return the value of the interrupt enable register
	 */
	uint32_t GetVICINTENABLE() const;
	/** Sets the interrupt enable register
	 *
	 * @param value the value to set
	 */
	void SetVICINTENABLE(uint32_t value);

	/** Returns the software interrupt register
	 *
	 * @return the value of the software interrupt register
	 */
	uint32_t GetVICSOFTINT() const;
	/** Sets the software interrupt register
	 *
	 * @param value the value to set
	 */
	void SetVICSOFTINT(uint32_t value);

	/** Returns the vector address register
	 *
	 * @return the value of the vector address register
	 */
	uint32_t GetVICVECTADDR() const;
	/** Set the vector address register
	 *
	 * @param value the value to set
	 */
	void SetVICVECTADDR(uint32_t value);

	/** Returns the default vector address register
	 *
	 * @return the value of the default vector address register
	 */
	uint32_t GetVICDEFVECTADDR() const;
	/** Sets the default vector address register
	 *
	 * @param value the value to set
	 */
	void SetVICDEFVECTADDR(uint32_t value);

	/** Returns one of the vector address registers
	 *
	 * @param index the index to look for
	 * @return the value of the indexed vector address register
	 */
	uint32_t GetVICVECTADDR(uint32_t index) const;
	/** Sets the value of one of the vector address registers
	 *
	 * @param index the index to look for
	 * @param value the value to set
	 */
	void SetVICVECTADDR(uint32_t index, uint32_t value);

	/** Returns one of the vector control registers
	 *
	 * @param index the index to look for
	 * @return the value of the indexed vector control register
	 */
	uint32_t GetVICVECTCNTL(uint32_t index) const;
	/** Sets the value of one of the vector controll registers
	 *
	 * @param index the index to look for
	 * @param value the value to set
	 */
	void SetVICVECTCNTL(uint32_t index, uint32_t value);

	/** Returns the test output register (nVICIRQ/nVICFIQ)
	 *
	 * @return the value of the test output register (nVICIRQ/nVICFIQ)
	 */
	uint32_t GetVICITOP1() const;
	/** Set the test output register (nVICIRQ/nVICFIQ)
	 *
	 * @param value the value to set
	 */
	void SetVICITOP1(uint32_t value);

	/**************************************************************************/
	/* Methods to get and set the registers                               END */
	/**************************************************************************/

	/** Get the source field from the given vector control register value
	 *
	 * @param value the value of the vector control register
	 * @return the source field of the value
	 */
	uint32_t VECTCNTLSource(uint32_t value) const;
	/** Get the enable field from the given vector control register value
	 *
	 * @param value the value of the vector control register
	 * @return the enable field of the value
	 */
	bool VECTCNTLEnable(uint32_t value) const;

	/** Base address of the system controller */
	uint32_t base_addr;
	/** UNISIM Parameter for the base address of the system controller */
	unisim::kernel::service::Parameter<uint32_t> param_base_addr;

	/** Interface to the UNISIM logger */
	unisim::kernel::logger::Logger logger;
};

} // end of namespace vic
} // end of namespace arm926ejs_pxp
} // end of namespace chipset
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_CHIPSET_ARM926EJS_PXP_VIC_VIC_HH__

