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

#ifndef __UNISIM_COMPONENT_TLM2_COM_STR7_EMI_STR7_EMI_TCC__
#define __UNISIM_COMPONENT_TLM2_COM_STR7_EMI_STR7_EMI_TCC__

#define LOCATION 	" - location = " << __FUNCTION__ << ":unisim_lib/unisim/component/tlm2/com/str7_emi/str7_emi.tcc:" << __LINE__
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
namespace str7_emi {
					
	using unisim::kernel::logger::DebugInfo;
	using unisim::kernel::logger::EndDebugInfo;
	using unisim::kernel::logger::EndDebug;
	
	template<unsigned int BUS_WIDTH>
	STR7_EMI<BUS_WIDTH> ::
	STR7_EMI(const sc_module_name& name, Object* parent) :
	sc_module(name),
	unisim::kernel::service::Object(name, parent),
	in_mem("in_mem"),
	bcon0(0x803d),
	bcon1(0x003d),
	bcon2(0x003e),
	bcon3(0x003c),
	reg_bcon0("BCON0", this, bcon0, "Bank configuration register 0"),
	reg_bcon1("BCON1", this, bcon1, "Bank configuration register 1"),
	reg_bcon2("BCON2", this, bcon2, "Bank configuration register 2"),
	reg_bcon3("BCON3", this, bcon3,  "Bank configuration register 3"),
	base_address(0),
	param_base_address("base-address", this, base_address, "EMI base address"),
	verbose_all(false),
	param_verbose_all("verbose-all", this, verbose_all, "Activate all the verbose options"),
	verbose_setup(false),
	param_verbose_setup("verbose-setup", this, verbose_setup, "Perform verbose setup"),
	verbose_tlm(false),
	param_verbose_tlm("verbose-tlm", this, verbose_tlm, "Perform verbose TLM communications"),
	logger(*this)
	{
		SC_HAS_PROCESS(STR7_EMI);

		/* bind the in_mem socket to the methods implementations provided by the module */
		in_mem.bind(*this);
	}
	
	template<unsigned int BUS_WIDTH>
	STR7_EMI<BUS_WIDTH>::
	~STR7_EMI()
	{
	}
	
	template<unsigned int BUS_WIDTH>
	bool
	STR7_EMI<BUS_WIDTH>::
	Setup()
	{
		if (verbose_all) verbose_setup = verbose_tlm = true;
		if (VerboseSetup())
		{
			logger << DebugInfo << "Performing setup: " << std::endl;
			logger << " - base address = 0x" << std::hex << base_address << std::dec << std::endl;
			logger << " - BCON0 = 0x" << std::hex << bcon0 << std::dec << std::endl;
			logger << " - BCON1 = 0x" << std::hex << bcon1 << std::dec << std::endl;
			logger << " - BCON2 = 0x" << std::hex << bcon2 << std::dec << std::endl;
			logger << " - BCON3 = 0x" << std::hex << bcon3 << std::dec << std::endl;
			logger << " - verbose all = " << (verbose_all?"true":"false") << std::endl;
			logger << " - verbose setup = " << (VerboseSetup()?"true":"false") << std::endl;
			logger << " - verbose tlm = " << (VerboseTLM()?"true":"false") << std::endl;
			logger << EndDebugInfo;
		}
		return true;
	}
	
	
	/* START: EMI registers methods */
	
	template<unsigned int BUS_WIDTH>
	uint16_t
	STR7_EMI<BUS_WIDTH>::
	ReadRegister(uint64_t address, bool update)
	{
		switch (address)
		{
			case 0x0: // BCON0
				return bcon0;
				break;
			case 0x04: // BCON1
				return bcon1;
				break;
			case 0x08: // BCON2
				return bcon2;
				break;
			case 0x0c: // BCON3
				return bcon3;
				break;
			default:
				return 0;
				break;
		}
		return 0;
	}
	
	template<unsigned int BUS_WIDTH>
	void
	STR7_EMI<BUS_WIDTH>::
	WriteRegister(uint64_t address, uint16_t value, bool update)
	{
		switch (address)
		{
			case 0x0: // BCON0
				bcon0 = value;
				break;
			case 0x04: // BCON1
				bcon1 = value;
				break;
			case 0x08: // BCON2
				bcon2 = value;
				break;
			case 0x0c: // BCON3
				bcon3 = value;
				break;
			default:
				break;
		}
	}

	/* END: EMI registers methods */
	
	/* START: methods implementing the "in_mem" socket */

	template<unsigned int BUS_WIDTH>
	void
	STR7_EMI<BUS_WIDTH>::
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
			if (size != 2)
			{
				memset(data, 0, size);
			}
			else
			{
				addr = addr - base_address;
				*(uint16_t *)data = ReadRegister(addr);
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
				if (size != 2)
					return;
				addr = addr - base_address;
				WriteRegister(addr, *(uint16_t *)data);
				return;
			}
		}
	}
	
	template<unsigned int BUS_WIDTH>
	tlm::tlm_sync_enum 
	STR7_EMI<BUS_WIDTH>::
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
	STR7_EMI<BUS_WIDTH>::
	get_direct_mem_ptr(tlm::tlm_generic_payload& trans, tlm::tlm_dmi& dmi_data)
	{
		// not supported
		return false;
	}
	
	template<unsigned int BUS_WIDTH>
	unsigned int 
	STR7_EMI<BUS_WIDTH>::
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
	STR7_EMI<BUS_WIDTH>::
	VerboseSetup()
	{
		return verbose_setup;
	}
	
	template<unsigned int BUS_WIDTH>
	bool 
	STR7_EMI<BUS_WIDTH>::
	VerboseTLM()
	{
		return verbose_tlm;
	}
	
	/* END: verbose options */
				
} // end of namespace str7_emi
} // end of namespace com
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_COM_STR7_EMI_STR7_EMI_TCC__
