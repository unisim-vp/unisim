/*
 *  Copyright (c) 2009,
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

#ifndef __UNISIM_COMPONENT_TLM2_COM_STR7_PRCCU_STR7_PRCCU_TCC__
#define __UNISIM_COMPONENT_TLM2_COM_STR7_PRCCU_STR7_PRCCU_TCC__

#define LOCATION 	" - location = " << __FUNCTION__ << ":unisim_lib/unisim/component/tlm2/com/str7_prccu/str7_prccu.tcc:" << __LINE__
#define TIME(X) 	" - time = " << sc_time_stamp() + (X)
#define PHASE(X) 	" - phase = " << 	( (X) == tlm::BEGIN_REQ  ? 	"BEGIN_REQ" : \
	( (X) == tlm::END_REQ    ? 	"END_REQ" : \
	( (X) == tlm::BEGIN_RESP ? 	"BEGIN_RESP" : \
	( (X) == tlm::END_RESP   ?  "END_RESP" : \
	"UNINITIALIZED_PHASE"))))

#define TRANS(L,X) \
	{ \
		(L) << " - trans = " << &(X) << endl \
			<< "   - " << ((X).is_read()?"read":"write") << endl \
			<< "   - address = 0x" << hex << (X).get_address() << dec << endl \
			<< "   - data_length = " << (X).get_data_length(); \
		if((X).is_write()) { \
			(L) << endl; \
			(L) << "   - data ="; \
			for(unsigned int _trans_i = 0; _trans_i < (X).get_data_length(); _trans_i++) { \
				(L) << " " << hex << (unsigned int)((X).get_data_ptr()[_trans_i]) << dec; \
			} \
		} \
	}

#define ETRANS(L,X) \
	{ \
		(L) << " - trans = " << &(X) << endl \
			<< "   - " << ((X).is_read()?"read":"write") << endl \
			<< "   - address = 0x" << hex << (X).get_address() << dec << endl \
			<< "   - data_length = " << (X).get_data_length() << endl \
			<< "   - response_status = "; \
		switch((X).get_response_status()) { \
			case tlm::TLM_OK_RESPONSE: \
				(L) << "TLM_OK_RESPONSE"; \
				break; \
			case tlm::TLM_INCOMPLETE_RESPONSE: \
				(L) << "TLM_INCOMPLETE_RESPONSE"; \
				break; \
			case tlm::TLM_GENERIC_ERROR_RESPONSE: \
				(L) << "TLM_GENERIC_ERROR_RESPONSE"; \
				break; \
			case tlm::TLM_ADDRESS_ERROR_RESPONSE: \
				(L) << "TLM_ADDRESS_ERROR_RESPONSE"; \
				break; \
			case tlm::TLM_COMMAND_ERROR_RESPONSE: \
				(L) << "TLM_COMMAND_ERROR_RESPONSE"; \
				break; \
			case tlm::TLM_BURST_ERROR_RESPONSE: \
				(L) << "TLM_BURST_ERROR_RESPONSE"; \
				break; \
			case tlm::TLM_BYTE_ENABLE_ERROR_RESPONSE: \
				(L) << "TLM_BYTE_ENABLE_ERROR_RESPONSE"; \
				break; \
		} \
		if((X).get_response_status() == tlm::TLM_OK_RESPONSE) { \
			(L) << endl; \
			(L) << "   - data ="; \
			for(unsigned int _trans_i = 0; _trans_i < (X).get_data_length(); _trans_i++) { \
				(L) << " " << hex << (unsigned int)((X).get_data_ptr()[_trans_i]) << dec; \
			} \
		} \
	}

#if defined(__GNUC__) && ((__GNUC__ >= 2 && __GNUC_MINOR__ >= 96) || __GNUC__ >= 3)
#define likely(x)       __builtin_expect((x),1)
#define unlikely(x)     __builtin_expect((x),0)
#else
#define likely(x) (x)
#define unlikely(x) (x)
#endif

#include <iostream>

namespace unisim {
namespace component {
namespace tlm2 {
namespace com {
namespace str7_prccu {
					
	using unisim::kernel::logger::DebugInfo;
	using unisim::kernel::logger::EndDebugInfo;
	using unisim::kernel::logger::EndDebug;
	
	template<unsigned int BUS_WIDTH>
	STR7_PRCCU<BUS_WIDTH> ::
	STR7_PRCCU(const sc_module_name& name, Object* parent) :
	sc_module(name),
	unisim::kernel::service::Object(name, parent),
	in_mem("in_mem"),
	rccu_ccr(0x0000),
	rccu_cfr(0x804A),
	rccu_pll1cr(0x0007),
	prccu_per(0xFFFF),
	prccu_smr(0x0001),
	pcu_mdivr(0x0000),
	pcu_pdivr(0x0000),
	pcu_rstr(0x0000),
        pcu_pll2cr(0x0033),
	pcu_bootcr(0x01C0),
	pcu_pwrcr(0x1000),

        reg_rccu_ccr("PRCCU_CCR", this, rccu_ccr, "Clock Control Register"),
	reg_rccu_cfr("PRCCU_CFR", this, rccu_cfr, "Clock Flag Register"),
	reg_rccu_pll1cr("PRCCU_PLL1CR", this, rccu_pll1cr, "PLL1 configuration register"),
	reg_prccu_per("PRCCU_PER", this, prccu_per, "Peripheral enable register"),
        reg_prccu_smr("PRCCU_SMR", this, prccu_smr, "Systeme Mode register"),
	reg_pcu_mdivr("PCU_MDIVR", this, pcu_mdivr, "MCLK Divider Control"),
	reg_pcu_pdivr("PCU_PDIVR", this, pcu_pdivr, "Peripheral clock Divider Control register"),
	reg_pcu_rstr("PCU_RSTR", this, pcu_rstr, "Peripheral Reset Control register"),
	reg_pcu_pll2cr("PCU_PLL2CR", this, pcu_pll2cr, "PLL2 configuration register"),
	reg_pcu_bootcr("PCU_BOOTCR", this, pcu_bootcr, "Boot configuration register"),
	reg_pcu_pwrcr("PCU_PWRCR", this, pcu_pwrcr, "Power control register"),

	base_address(0),
	param_base_address("base-address", this, base_address, "PRCCU base address"),
	verbose_all(false),
	param_verbose_all("verbose-all", this, verbose_all, "Activate all the verbose options"),
	verbose_setup(false),
	param_verbose_setup("verbose-setup", this, verbose_setup, "Perform verbose setup"),
	verbose_tlm(false),
	param_verbose_tlm("verbose-tlm", this, verbose_tlm, "Perform verbose TLM communications"),
	logger(*this)
	{
		SC_HAS_PROCESS(STR7_PRCCU);

		/* bind the in_mem socket to the methods implementations provided by the module */
		in_mem.bind(*this);
	}
	
	template<unsigned int BUS_WIDTH>
	STR7_PRCCU<BUS_WIDTH>::
	~STR7_PRCCU()
	{
	}
	
	template<unsigned int BUS_WIDTH>
	bool
	STR7_PRCCU<BUS_WIDTH>::
	Setup()
	{
		if (verbose_all) verbose_setup = verbose_tlm = true;
		if (VerboseSetup())
		{
			logger << DebugInfo << "Performing setup: " << std::endl;
			logger << " - base address = 0x" << std::hex << base_address << std::dec << std::endl;
			logger << " - PRCCU_CCR = 0x" << std::hex << rccu_ccr << std::dec << std::endl;
			logger << " - PRCCU_CFR = 0x" << std::hex << rccu_cfr << std::dec << std::endl;
			logger << " - PRCCU_PLL1CR = 0x" << std::hex << rccu_pll1cr << std::dec << std::endl;
			logger << " - PRCCU_PER = 0x" << std::hex << prccu_per << std::dec << std::endl;
                        logger << " - PRCCU_SMR = 0x" << std::hex << prccu_smr << std::dec << std::endl;
			logger << " - PCU_MDIVR = 0x" << std::hex << pcu_mdivr << std::dec << std::endl;
			logger << " - PCU_PDIVR = 0x" << std::hex << pcu_pdivr << std::dec << std::endl;
			logger << " - PCU_RSTR = 0x" << std::hex << pcu_rstr << std::dec << std::endl;
                        logger << " - PCU_PLL2CR = 0x" << std::hex << pcu_pll2cr << std::dec << std::endl;
			logger << " - PCU_BOOTCR = 0x" << std::hex << pcu_bootcr << std::dec << std::endl;
			logger << " - PCU_PWRCR = 0x" << std::hex << pcu_pwrcr << std::dec << std::endl;
			logger << " - verbose all = " << (verbose_all?"true":"false") << std::endl;
			logger << " - verbose setup = " << (VerboseSetup()?"true":"false") << std::endl;
			logger << " - verbose tlm = " << (VerboseTLM()?"true":"false") << std::endl;
			logger << EndDebugInfo;
		}
		return true;
	}
	
	
	/* START: PRCCU registers methods */
	
	template<unsigned int BUS_WIDTH>
	uint32_t
	STR7_PRCCU<BUS_WIDTH>::
	ReadRegister(uint32_t address, bool update)
	{
		switch (address)
		{
			case 0x0: // PRCCU_CCR
				return rccu_ccr;
				break;
			case 0x08: // PRCCU_CFR
				return rccu_cfr;
				break;
			case 0x18: // PRCCU_PLL1CR
				return rccu_pll1cr;
				break;
			case 0x1c: // PRCCU_PER
				return prccu_per;
				break;
                        case 0x20: // PRCCU_SMR
                                return prccu_smr;
                                break;
                        case 0x40: // PCU_MDIVR
                                return pcu_mdivr;
                                break;
            		case 0x44: // PCU_PDIVR
				return pcu_pdivr;
				break;
			case 0x48: // PCU_RSTR
				return pcu_rstr;
				break;
                        case 0x4c: // PCU_PLL2CR
                                return pcu_pll2cr;
                                break;
                        case 0x50: // PCU_BOOTCR
                                return pcu_bootcr;
                                break;
                        case 0x54: // PCU_PWRCR
                                return pcu_pwrcr;
                                break;

			default:
				return 0;
				break;
		}
		return 0;
	}
	
	template<unsigned int BUS_WIDTH>
	void
	STR7_PRCCU<BUS_WIDTH>::
	WriteRegister(uint32_t address, uint32_t value, bool update)
	{
		switch (address)
		{
			case 0x0: // PRCCU_CCR
				rccu_ccr=value;
				break;
			case 0x08: // PRCCU_CFR
				rccu_cfr=value|0x00000002; //to simulate that the bit PLL LOCK is true...
				break;
			case 0x18: // PRCCU_PLL1CR
				rccu_pll1cr=value;
				break;
			case 0x1c: // PRCCU_PER
				prccu_per=value;
				break;
                        case 0x20: // PRCCU_SMR
                                prccu_smr=value;
                                break;
                        case 0x40: // PCU_MDIVR
                                pcu_mdivr=(uint16_t)value;
                                break;
            		case 0x44: // PCU_PDIVR
				pcu_pdivr=(uint16_t)value;
				break;
			case 0x48: // PCU_RSTR
				pcu_rstr=(uint16_t)value;
				break;
                        case 0x4c: // PCU_PLL2CR
                                pcu_pll2cr=(uint16_t)value;
                                break;
                        case 0x50: // PCU_BOOTCR
                                pcu_bootcr=(uint16_t)value;
                                break;
                        case 0x54: // PCU_PWRCR
                                pcu_pwrcr=(uint16_t) (value|0x00001000); //to simulate the flag VROK...
                                break;
			
			default:
				break;
		}
	}

	/* END: PRCCU registers methods */
	
	/* START: methods implementing the "in_mem" socket */

template <unsigned int BUS_WIDTH>
void
STR7_PRCCU<BUS_WIDTH> ::
b_transport(tlm::tlm_generic_payload& trans, sc_core::sc_time& t)
{
	if (VerboseTLM())
	{
		logger << DebugInfo << "Received memory b_transport transaction:" << endl;
		TRANS(logger, trans);
		logger << endl << TIME(t) << EndDebug;
	}
	unsigned char *data = trans.get_data_ptr();
	unsigned int size = trans.get_data_length();
	uint64_t addr = trans.get_address();

	if (trans.is_read())
	{
		trans.set_response_status(tlm::TLM_OK_RESPONSE);
		if ((size !=4) && (size !=2))
		{
			memset(data, 0, size);
		}
		else
		{
			addr = addr - base_address;
			*(uint32_t *)data = ReadRegister(addr);
		}

		if (VerboseTLM())
		{
			logger << DebugInfo << "Replying received memory b_transport transaction:" << endl;
			ETRANS(logger, trans);
			logger << endl << TIME(t) << EndDebug;
		}
		return;
	}
	else
	{
		if (trans.is_write())
		{
			trans.set_response_status(tlm::TLM_OK_RESPONSE);
			if ((size !=4) && (size !=2))
				return;
			addr = addr - base_address;
			WriteRegister(addr, *(uint32_t *)data);
			return;
		}
	}
}
	template<unsigned int BUS_WIDTH>
	tlm::tlm_sync_enum 
	STR7_PRCCU<BUS_WIDTH>::
	nb_transport_fw(tlm::tlm_generic_payload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t)
	{
		if (VerboseTLM())
		{
			logger << DebugInfo << "Received memory nb_transport_fw transaction:" << endl;
			TRANS(logger, trans);
			logger << endl
			<< PHASE(phase) << endl
			<< TIME(t) << EndDebug;
		}
		/* TODO */
		return tlm::TLM_COMPLETED;
	}
	
	template<unsigned int BUS_WIDTH>
	bool 
	STR7_PRCCU<BUS_WIDTH>::
	get_direct_mem_ptr(tlm::tlm_generic_payload& trans, tlm::tlm_dmi& dmi_data)
	{
		// not supported
		return false;
	}
	
	template<unsigned int BUS_WIDTH>
	unsigned int 
	STR7_PRCCU<BUS_WIDTH>::
	transport_dbg(tlm::tlm_generic_payload& trans)
	{
		unsigned char *data = trans.get_data_ptr();
		unsigned int size = trans.get_data_length();
		uint64_t addr = trans.get_address();
		
		if (trans.is_read())
		{
			trans.set_response_status(tlm::TLM_OK_RESPONSE);
			if (size == 0) return size;
			unsigned int read_size = 0;
			do
			{
				uint64_t offset = ((addr + read_size) % 2);
				uint64_t base = (addr + read_size) - offset;
				uint16_t reg = 0;
				reg = ReadRegister(base - base_address, false);
				reg = reg >> ((offset) * 8);
				reg = reg & (uint16_t)0x00ff;
				unsigned char t = reg;
				data[read_size] = t;
				read_size++;
			} while (read_size != size);
			
			for (unsigned int i = 0; i < size; i++)
				cerr << hex << (unsigned int)(data[i]) << dec << " ";
			cerr << endl;
			return size;
		}
		else if (trans.is_write())
		{
			trans.set_response_status(tlm::TLM_OK_RESPONSE);
			if (size == 0) return size;
			unsigned int write_size = 0;
			do
			{
				uint64_t offset = (addr + write_size) % 2;
				uint64_t base = addr - offset;
				uint16_t reg = 0;
				reg = ReadRegister(base - base_address, false);
				reg = reg & ~((uint16_t)0xff << (offset * 8));
				reg = reg + ((uint16_t)(data[write_size]) << (offset * 8));
				WriteRegister(base - base_address, reg, false);
				write_size++;
			} while (write_size != size);
			
			return size;
		}
		
		return 0;
	}
	
	/* END: methods implementing the "in_mem" socket */
					
	/* START: verbose options */
	
	template<unsigned int BUS_WIDTH>
	bool 
	STR7_PRCCU<BUS_WIDTH>::
	VerboseSetup()
	{
		return verbose_setup;
	}
	
	template<unsigned int BUS_WIDTH>
	bool 
	STR7_PRCCU<BUS_WIDTH>::
	VerboseTLM()
	{
		return verbose_tlm;
	}
	
	/* END: verbose options */
				
} // end of namespace str7_prccu
} // end of namespace com
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_COM_STR7_PRCCU_STR7_PRCCU_TCC__
