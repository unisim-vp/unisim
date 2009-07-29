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

#ifndef __UNISIM_COMPONENT_TLM2_COM_STR7_UART_STR7_UART_TCC__
#define __UNISIM_COMPONENT_TLM2_COM_STR7_UART_STR7_UART_TCC__

#define LOCATION 	" - location = " << __FUNCTION__ << ":unisim_lib/unisim/component/tlm2/com/str7_uart/str7_uart.tcc:" << __LINE__
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
                namespace str7_uart {

                    using unisim::kernel::logger::DebugInfo;
                    using unisim::kernel::logger::EndDebugInfo;
                    using unisim::kernel::logger::EndDebug;

                    template<unsigned int BUS_WIDTH>
                    STR7_UART<BUS_WIDTH> ::
                    STR7_UART(const sc_module_name& name, Object* parent) :
                    sc_module(name),
                    unisim::kernel::service::Object(name, parent),
                    in_mem("in_mem"),
                    uartn_br(0x0001),
                    uartn_txbufr(0x0000),
                    uartn_rxbufr(0x0000),
                    uartn_cr(0x0000),
                    uartn_ier(0x0000),
                    uartn_sr(0x0006),
                    uartn_gtr(0x0000),
                    uartn_tor(0x0000),
                    uartn_txrstr(0x0000),
                    uartn_rxrstr(0x0000),

                    reg_uartn_br("UARTN_BR", this, uartn_br, "Uart Baudrate register"),
                    reg_uartn_txbufr("UARTN_TXBUFR", this, uartn_txbufr, "Uart Tx register"),
                    reg_uartn_rxbufr("UARTN_RXBUFR", this, uartn_rxbufr, "Uart Rx register"),
                    reg_uartn_cr("UARTN_CR", this, uartn_cr, "Uart control register"),
                    reg_uartn_ier("UARTN_IER", this, uartn_ier, "Uart INTEnable register"),
                    reg_uartn_sr("UARTN_SR", this, uartn_sr, "Uart Status register"),
                    reg_uartn_gtr("UARTN_GTR", this, uartn_gtr, "Uart Guardtime register"),
                    reg_uartn_tor("UARTN_TOR", this, uartn_tor, "Uart Timeout register"),
                    reg_uartn_txrstr("UARTN_TXRSTR", this, uartn_txrstr, "Uart Tx reset register"),
                    reg_uartn_rxrstr("UARTN_RXRSTR", this, uartn_rxrstr, "Uart Rx reset register"),

                    base_address(0),
                    param_base_address("base-address", this, base_address, "UART base address"),
                    verbose_all(false),
                    param_verbose_all("verbose-all", this, verbose_all, "Activate all the verbose options"),
                    verbose_setup(false),
                    param_verbose_setup("verbose-setup", this, verbose_setup, "Perform verbose setup"),
                    verbose_tlm(false),
                    param_verbose_tlm("verbose-tlm", this, verbose_tlm, "Perform verbose TLM communications"),
                    logger(*this) {
                        SC_HAS_PROCESS(STR7_UART);

                        /* bind the in_mem socket to the methods implementations provided by the module */
                        in_mem.bind(*this);
                    }

                    template<unsigned int BUS_WIDTH>
                    STR7_UART<BUS_WIDTH>::
                    ~STR7_UART() {
                    }

                    template<unsigned int BUS_WIDTH>
                    bool
                    STR7_UART<BUS_WIDTH>::
                    Setup() {
                        if (verbose_all) verbose_setup = verbose_tlm = true;
                        if (VerboseSetup()) {
                            logger << DebugInfo << "Performing setup: " << std::endl;
                            logger << " - base address = 0x" << std::hex << base_address << std::dec << std::endl;
                            logger << " - UARTN_BR = 0x" << std::hex << uartn_br << std::dec << std::endl;
                            logger << " - UARTN_TXBUFR = 0x" << std::hex << uartn_txbufr << std::dec << std::endl;
                            logger << " - UARTN_RXBUFR = 0x" << std::hex << uartn_rxbufr << std::dec << std::endl;
                            logger << " - UARTN_CR = 0x" << std::hex << uartn_cr << std::dec << std::endl;
                            logger << " - UARTN_IER = 0x" << std::hex << uartn_ier << std::dec << std::endl;
                            logger << " - UARTN_SR = 0x" << std::hex << uartn_sr << std::dec << std::endl;
                            logger << " - UARTN_GTR = 0x" << std::hex << uartn_gtr << std::dec << std::endl;
                            logger << " - UARTN_TOR = 0x" << std::hex << uartn_tor << std::dec << std::endl;
                            logger << " - UARTN_TXRSTR = 0x" << std::hex << uartn_txrstr << std::dec << std::endl;
                            logger << " - UARTN_RXRSTR = 0x" << std::hex << uartn_rxrstr << std::dec << std::endl;
                            logger << " - verbose all = " << (verbose_all ? "true" : "false") << std::endl;
                            logger << " - verbose setup = " << (VerboseSetup() ? "true" : "false") << std::endl;
                            logger << " - verbose tlm = " << (VerboseTLM() ? "true" : "false") << std::endl;
                            logger << EndDebugInfo;
                        }
                        return true;
                    }

                    /* START: UART registers methods */

                    template<unsigned int BUS_WIDTH>
                    uint16_t
                    STR7_UART<BUS_WIDTH>::
                    ReadRegister(uint64_t address, bool update) {
                        switch (address) {

                            case 0x0: // UARTN_BR
                                return uartn_br;
                                break;
                            case 0x04: // UARTN_TXBUFR
                                return uartn_txbufr;
                                break;
                            case 0x08: // UARTN_RXBUFR
                                return uartn_rxbufr;
                                break;
                            case 0x0c: // UARTN_CR
                                return uartn_cr;
                            case 0x10: // UARTN_IER
                                return uartn_ier;
                                break;
                            case 0x14: // UARTN_SR
                                return uartn_sr;
                                break;
                            case 0x18: // UARTN_GTR
                                return uartn_gtr;
                                break;
                            case 0x1C: // UARTN_TOR
                                return uartn_tor;
                                break;
                            case 0x20: // UARTN_TXRSTR
                                return uartn_txrstr;
                                break;
                            case 0x24: // UARTN_RXRSTR
                                return uartn_rxrstr;
                                break;
                            default:
                                return 0;
                                break;
                        }
                        return 0;
                    }

                    template<unsigned int BUS_WIDTH>
                    void
                    STR7_UART<BUS_WIDTH>::
                    WriteRegister(uint64_t address, uint16_t value, bool update) {
                        switch (address) {
                            case 0x0: // UARTN_BR
                                uartn_br = value;
                                break;
                            case 0x04: // UARTN_TXBUFR
                                uartn_txbufr = value;
                                break;
                            case 0x08: // UARTN_RXBUFR
                                uartn_rxbufr = value;
                                break;
                            case 0x0c: // UARTN_CR
                                uartn_cr = value;
                            case 0x10: // UARTN_IER
                                uartn_ier = value;
                                break;
                            case 0x14: // UARTN_SR
                                uartn_sr = value;
                                break;
                            case 0x18: // UARTN_GTR
                                uartn_gtr = value;
                                break;
                            case 0x1C: // UARTN_TOR
                                uartn_tor = value;
                                break;
                            case 0x20: // UARTN_TXRSTR
                                uartn_txrstr = value;
                                break;
                            case 0x24: // UARTN_RXRSTR
                                uartn_rxrstr = value;
                                break;
                            default:
                                break;
                        }
                    }

                    /* END: UART registers methods */

                    /* START: methods implementing the "in_mem" socket */

                    template<unsigned int BUS_WIDTH>
                    void
                    STR7_UART<BUS_WIDTH>::
                    b_transport(tlm::tlm_generic_payload& trans, sc_core::sc_time& t) {
                        if (VerboseTLM()) {
                            logger << DebugInfo << "Received memory b_transport transaction:" << endl;
                            TRANS(logger, trans);
                            logger << endl << TIME(t) << EndDebug;
                        }
                        unsigned char *data = trans.get_data_ptr();
                        unsigned int size = trans.get_data_length();
                        uint64_t addr = trans.get_address();

                        if (trans.is_read()) {
                            trans.set_response_status(tlm::TLM_OK_RESPONSE);
                            if (size != 2) {
                                memset(data, 0, size);
                            } else {
                                addr = addr - base_address;
                                *(uint16_t *) data = ReadRegister(addr);
                            }

                            if (VerboseTLM()) {
                                logger << DebugInfo << "Replying received memory b_transport transaction:" << endl;
                                ETRANS(logger, trans);
                                logger << endl << TIME(t) << EndDebug;
                            }
                            return;
                        } else {
                            if (trans.is_write()) {
                                trans.set_response_status(tlm::TLM_OK_RESPONSE);
                                if (size != 2)
                                    return;
                                addr = addr - base_address;
                                WriteRegister(addr, *(uint16_t *) data);
                                return;
                            }
                        }
                    }

                    template<unsigned int BUS_WIDTH>
                    tlm::tlm_sync_enum
                    STR7_UART<BUS_WIDTH>::
                    nb_transport_fw(tlm::tlm_generic_payload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t) {
                        if (VerboseTLM()) {
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
                    STR7_UART<BUS_WIDTH>::
                    get_direct_mem_ptr(tlm::tlm_generic_payload& trans, tlm::tlm_dmi& dmi_data) {
                        // not supported
                        return false;
                    }

                    template<unsigned int BUS_WIDTH>
                    unsigned int
                    STR7_UART<BUS_WIDTH>::
                    transport_dbg(tlm::tlm_generic_payload& trans) {
                        unsigned char *data = trans.get_data_ptr();
                        unsigned int size = trans.get_data_length();
                        uint64_t addr = trans.get_address();

                        if (trans.is_read()) {
                            trans.set_response_status(tlm::TLM_OK_RESPONSE);
                            if (size == 0) return size;
                            unsigned int read_size = 0;
                            do {
                                uint64_t offset = ((addr + read_size) % 2);
                                uint64_t base = (addr + read_size) - offset;
                                uint16_t reg = 0;
                                reg = ReadRegister(base - base_address, false);
                                reg = reg >> ((offset) * 8);
                                reg = reg & (uint16_t) 0x00ff;
                                unsigned char t = reg;
                                data[read_size] = t;
                                read_size++;
                            } while (read_size != size);

                            for (unsigned int i = 0; i < size; i++)
                                cerr << hex << (unsigned int) (data[i]) << dec << " ";
                            cerr << endl;
                            return size;
                        } else if (trans.is_write()) {
                            trans.set_response_status(tlm::TLM_OK_RESPONSE);
                            if (size == 0) return size;
                            unsigned int write_size = 0;
                            do {
                                uint64_t offset = (addr + write_size) % 2;
                                uint64_t base = addr - offset;
                                uint16_t reg = 0;
                                reg = ReadRegister(base - base_address, false);
                                reg = reg & ~((uint16_t) 0xff << (offset * 8));
                                reg = reg + ((uint16_t) (data[write_size]) << (offset * 8));
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
                    STR7_UART<BUS_WIDTH>::
                    VerboseSetup() {
                        return verbose_setup;
                    }

                    template<unsigned int BUS_WIDTH>
                    bool
                    STR7_UART<BUS_WIDTH>::
                    VerboseTLM() {
                        return verbose_tlm;
                    }

                    /* END: verbose options */

                } // end of namespace str7_uart
            } // end of namespace com
        } // end of namespace tlm2
    } // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_COM_STR7_UART_STR7_UART_TCC__
