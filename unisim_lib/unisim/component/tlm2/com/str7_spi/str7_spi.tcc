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
 * Authors: khaled Rahmouni
 */

#ifndef __UNISIM_COMPONENT_TLM2_COM_STR7_SPI_STR7_SPI_TCC__
#define __UNISIM_COMPONENT_TLM2_COM_STR7_SPI_STR7_SPI_TCC__

#define LOCATION 	" - location = " << __FUNCTION__ << ":unisim_lib/unisim/component/tlm2/com/str7_spi/str7_spi.tcc:" << __LINE__
#define TIME(X) 	" - time = " << sc_time_stamp() + (X)
#define PHASE(X) 	" - phase = " << 	( (X) == tlm::BEGIN_REQ  ? 	"BEGIN_REQ" : \
										( (X) == tlm::END_REQ    ? 	"END_REQ" : \
										( (X) == tlm::BEGIN_RESP ? 	"BEGIN_RESP" : \
										( (X) == tlm::END_RESP   ?  "END_RESP" : \
										  							"UNINITIALIZED_PHASE"))))
#define ITRANS(X)	" - trans.level = " << (X).level
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

namespace unisim {
    namespace component {
        namespace tlm2 {
            namespace com {
                namespace str7_spi {

                    using unisim::kernel::logger::DebugInfo;
                    using unisim::kernel::logger::DebugWarning;
                    using unisim::kernel::logger::DebugError;
                    using unisim::kernel::logger::EndDebug;
                    using unisim::kernel::logger::EndDebugInfo;
                    using unisim::kernel::logger::EndDebugWarning;
                    using unisim::kernel::logger::EndDebugError;

                    template <unsigned int BUS_WIDTH, bool VERBOSE>
                    STR7_SPI<BUS_WIDTH, VERBOSE> ::
                    STR7_SPI(const sc_module_name& name, Object* parent) :
                    Object(name, parent),
                    sc_module(name),
                    irq("irq"),
                    mosi("mosi"),
                    miso("miso"),
                    in_mem("in_mem"),
                    base_address(0),
                    param_base_address("base-address", this, base_address),
                    logger(*this),
                    verbose_all(false),
                    verbose_setup(false),
                    verbose_run(false),
                    verbose_tlm(false) {

                        //reset registers values
                        Registers[BSPIn_RX] = 0x0000;
                        Registers[BSPIn_TX] = 0x0000;
                        Registers[BSPIn_CSR1] = 0x0000;
                        Registers[BSPIn_CSR2] = 0x0040;
                        Registers[BSPIn_CLK] = 0x0006;

                        tx_fifo_size = 1;
                        rx_fifo_size = 1;

                        // Init read mask
                        MaskR[BSPIn_RX] = 0xFFFF;
                        MaskR[BSPIn_TX] = 0x0000;
                        MaskR[BSPIn_CSR1] = 0xFFCF;
                        MaskR[BSPIn_CSR2] = 0xFFFC;
                        MaskR[BSPIn_CLK] = 0x00FF;

                        // Init write mask
                        MaskW[BSPIn_RX] = 0x0000;
                        MaskW[BSPIn_TX] = 0xFFFF;
                        MaskW[BSPIn_CSR1] = 0xFFCF;
                        MaskW[BSPIn_CSR2] = 0xFC01;
                        MaskW[BSPIn_CLK] = 0x00FF;


                  //      SC_HAS_PROCESS(STR7_SPI);
                    //    SC_THREAD(SPIHANDLER);


                        if (VERBOSE) {
                            param_verbose_all = new unisim::kernel::service::Parameter<bool>("verbose-all", this, verbose_all, "Enable all the verbose options");
                            param_verbose_setup = new unisim::kernel::service::Parameter<bool>("verbose-setup", this, verbose_setup, "Display module setup");
                            param_verbose_run = new unisim::kernel::service::Parameter<bool>("verbose-run", this, verbose_run);
                            param_verbose_tlm = new unisim::kernel::service::Parameter<bool>("verbose-tlm", this, verbose_tlm, "Display TLM2.0 transaction management");
                        }

                        /* bind the in_mem socket to the methods implementations provided by the module */
                        in_mem.bind(*this);

                        /* register the irq socket to the methods implementation provided by the module */
                        irq.register_nb_transport_bw(this, &THIS_MODULE::OutIRQNb);
                        irq.register_invalidate_direct_mem_ptr(this, &THIS_MODULE::OutIRQDMI);
                        /* register the spi socket to the methods implementation provided by the module */
                        mosi.register_nb_transport_bw(this, &THIS_MODULE::MOSINb);
                        mosi.register_invalidate_direct_mem_ptr(this, &THIS_MODULE::MOSIDMI);
                        miso.register_b_transport(this, &THIS_MODULE::MISOB);
                    }

                    template <unsigned int BUS_WIDTH, bool VERBOSE>
                    STR7_SPI<BUS_WIDTH, VERBOSE> ::
                    ~STR7_SPI() {
                        if (param_verbose_all) delete param_verbose_all;
                        if (param_verbose_setup) delete param_verbose_setup;
                        if (param_verbose_run) delete param_verbose_run;
                    }

                    template <unsigned int BUS_WIDTH, bool VERBOSE>
                    bool
                    STR7_SPI<BUS_WIDTH, VERBOSE> ::
                    Setup() {
                        if (VERBOSE) {
                            if (verbose_all) verbose_setup = verbose_run = true;
                        }

                        if (VerboseSetup()) {
                            logger << DebugInfo << "Setup:" << endl;
                            if (VerboseAll()) logger << " - verbose-all activated" << endl;
                            if (VerboseSetup()) logger << " - verbose-setup activated" << endl;
                            if (VerboseRun()) logger << " - verbose-run activated" << endl;
                            if (VerboseTLM()) logger << " - verbose-tlm activated" << endl;

                            logger << " - base address = 0x" << hex << base_address << dec;
                            logger << EndDebugInfo;
                        }

                        return true;
                    }

                    /* START: callback methods for the out_irq sockets */

                    template <unsigned int BUS_WIDTH, bool VERBOSE>
                    tlm::tlm_sync_enum
                    STR7_SPI<BUS_WIDTH, VERBOSE> ::
                    OutIRQNb(TLMInterruptPayload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t) {
                        switch (phase) {
                            case tlm::END_REQ :
                                        /* nothing to do, just indicate that the transaction has been completed */
                                        trans.release();
                                return tlm::TLM_COMPLETED;
                                break;
                            case tlm::BEGIN_REQ :
                            case tlm::BEGIN_RESP :
                            case tlm::END_RESP :
                                        /* unexpected phase received */
                                        logger << DebugError << "Unexpected phase received on out_irq:" << endl
                                        << LOCATION << endl
                                        << ITRANS(trans) << endl
                                        << PHASE(phase) << endl
                                        << TIME(t) << EndDebugError;
                                sc_stop();
                                wait();
                                break;
                        }
                        // this code should never be executed
                        logger << DebugError << "This code should never be executed" << endl
                                << LOCATION << endl
                                << ITRANS(trans) << endl
                                << PHASE(phase) << endl
                                << TIME(t) << EndDebugError;
                        sc_stop();
                        wait();
                        return tlm::TLM_COMPLETED;
                    }

                    template <unsigned int BUS_WIDTH, bool VERBOSE>
                    void
                    STR7_SPI<BUS_WIDTH, VERBOSE> ::
                    OutIRQDMI(sc_dt::uint64 start_range, sc_dt::uint64 end_range) {
                        /* This method shoud never be used */
                    }

                    /* END: callback methods for the out_irq sockets */

                    /* START: callback methods for the spi sockets */

                    template <unsigned int BUS_WIDTH, bool VERBOSE>
                    tlm::tlm_sync_enum
                    STR7_SPI<BUS_WIDTH, VERBOSE> ::
                    MOSINb(tlm::tlm_generic_payload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t) {
                        switch (phase) {
                            case tlm::END_REQ :
                                        /* nothing to do, just indicate that the transaction has been completed */
                                        trans.release();
                                return tlm::TLM_COMPLETED;
                                break;
                            case tlm::BEGIN_REQ :
                            case tlm::BEGIN_RESP :
                            case tlm::END_RESP :
                                        /* unexpected phase received */
                                        logger << DebugError << "Unexpected phase received on out_spi:" << endl
                                        << LOCATION << endl
                                        << PHASE(phase) << endl
                                        << TIME(t) << EndDebugError;
                                sc_stop();
                                wait();
                                break;
                        }
                        // this code should never be executed
                        logger << DebugError << "This code should never be executed" << endl
                                << LOCATION << endl
                                << PHASE(phase) << endl
                                << TIME(t) << EndDebugError;
                        sc_stop();
                        wait();
                        return tlm::TLM_COMPLETED;
                    }

                    template <unsigned int BUS_WIDTH, bool VERBOSE>
                    void
                    STR7_SPI<BUS_WIDTH, VERBOSE> ::
                    MOSIDMI(sc_dt::uint64 start_range, sc_dt::uint64 end_range) {
                        /* This method should never be used */
                    }

                    template <unsigned int BUS_WIDTH, bool VERBOSE>
                    void
                    STR7_SPI<BUS_WIDTH, VERBOSE> ::
                    MISOB(tlm::tlm_generic_payload& trans, sc_core::sc_time& t) {
                    }


                    /* END: callback methods for the spi sockets */

                    /* START: methods implementing the "in_mem" socket */

                    template <unsigned int BUS_WIDTH, bool VERBOSE>
                    void
                    STR7_SPI<BUS_WIDTH, VERBOSE> ::
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
                                *(uint32_t *) data = ReadRegister(addr);
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
                                WriteRegister(addr, *(uint32_t *) data);
                                return;
                            }
                        }
                    }

                    template <unsigned int BUS_WIDTH, bool VERBOSE>
                    tlm::tlm_sync_enum
                    STR7_SPI<BUS_WIDTH, VERBOSE> ::
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

                    template <unsigned int BUS_WIDTH, bool VERBOSE>
                    bool
                    STR7_SPI<BUS_WIDTH, VERBOSE> ::
                    get_direct_mem_ptr(tlm::tlm_generic_payload& trans, tlm::tlm_dmi& dmi_data) {
                        /* This method should never be used */
                        return false;
                    }

                    template <unsigned int BUS_WIDTH, bool VERBOSE>
                    unsigned int
                    STR7_SPI<BUS_WIDTH, VERBOSE> ::
                    transport_dbg(tlm::tlm_generic_payload& trans) {
                        /* This method should never be used */
                        return 0;
                    }

                    /* END: methods implementing the "in_mem" socket */

                    /** Read register method and update spi state as necessary.
                     * @param addr		the register address
                     * @return 			the contents of the register
                     */
                    template <unsigned int BUS_WIDTH, bool VERBOSE>
                    uint32_t
                    STR7_SPI<BUS_WIDTH, VERBOSE> ::
                    ReadRegister(uint32_t const addr) {
                        uint32_t value = -1;
                        uint32_t index = (addr >> 2);

                        if (MaskR[index] == 0x0000)
                            cout << __func__ << "(" << __LINE__ << ")" << ": WARNING read write only register at " << (void*) index << endl;
                        else {
                            value = (uint32_t) (
                                    Registers[index] /* Read register value */
                                    & MaskR[index]); /* mask with read only bits */
                        }

                        switch (index) {
                            case BSPIn_RX:
                                if (!rx_fifo.empty()) {
                                    rx_fifo.pop_front();
                                    // Load RX with the next value

                                    if (!rx_fifo.empty())
                                        Registers[BSPIn_RX] = rx_fifo.front();
                                    // Clear RFNE: Receiver FIFO Not Empty (Clreared when a valid data has been read)

                                    Registers[BSPIn_CSR2] &= ~0x0008;
                                }
                                // Clear RFF: Receiver FIFO Full

                                Registers[BSPIn_CSR2] &= ~0x0010;
                                // Clear ROFL: Receiver Overflow

                                Registers[BSPIn_CSR2] &= ~0x0020;
                                break;
                            case BSPIn_CSR1:
                                break;
                            case BSPIn_CSR2:
                                // Clear ROFL: Receiver Overflow

                                Registers[BSPIn_CSR2] &= ~0x0020;
                                break;
                            case BSPIn_CLK:
                                break;
                            default:
                                break;
                        }

                        return (value);
                    }

                    /** Write register method and update spi state as necessary.
                     * @param addr		the register address
                     * @param value		the data to write into the register
                     */
                    template <unsigned int BUS_WIDTH, bool VERBOSE>
                    void
                    STR7_SPI<BUS_WIDTH, VERBOSE> ::
                    WriteRegister(uint32_t addr, uint32_t value) {
                        uint32_t index = (addr >> 2);
                        if (MaskW[index] == 0x0000)
                            cout << __func__ << "(" << __LINE__ << ")" << ": WARNING write read only register at " << (void*) index << endl;
                        else {
                            Registers[index] = (uint16_t) (
                                    (Registers[index] & ~MaskW[index]) /* protect read only bits */
                                    | (value & MaskW[index])); /* affect only writtable bits */
                        }
                        sc_time delay = sc_time(0, SC_NS);
                        tlm::tlm_generic_payload trans;


                        switch (index) {
                            case BSPIn_RX:
                                break;
                            case BSPIn_TX:
                                if ((Registers[BSPIn_CSR2] & 0x0040) == 0x0040) // transmit FIFO empty enable
                                {
                                   //irq_payload->level = false;
                                    //    irq->b_transport(*irq_payload, delay);
                                }
                                if ((Registers[BSPIn_CSR1] & 0x0001) && (tx_fifo.size() < tx_fifo_size)) {
                                    tx_fifo.push_back(Registers[BSPIn_TX]);

                                    // Set TFNE: Transmit FIFO Not Empty
                                    Registers[BSPIn_CSR2] |= 0x0200;

                                    // Clear TFE: Transmit FIFO Empty
                                    Registers[BSPIn_CSR2] &= ~0x0040;

                                    //  Clear TUFL: Transmit Underflow
                                    Registers[BSPIn_CSR2] &= ~0x0080;
                                }

                                if (tx_fifo.size() == tx_fifo_size) {
                                    // Set TFF: Transmit FIFO Full
                                    Registers[BSPIn_CSR2] |= 0x0100;
                                }

                                trans.set_command(tlm::TLM_WRITE_COMMAND);
                                trans.set_address(0x00);
                                trans.set_data_length(2);
                                trans.set_data_ptr((unsigned char *) &(Registers[BSPIn_TX]));

                                mosi->b_transport(trans, delay);
                                break;
                            case BSPIn_CSR1:

                                // Receive FIFO size (resize fifo if needed)

                                rx_fifo_size = ((Registers[BSPIn_CSR1] & 0xF000) >> 12) + 1;
                                if (rx_fifo_size > 10) rx_fifo_size = 1;
                                if (rx_fifo.size() > rx_fifo_size) rx_fifo.resize(rx_fifo_size);
                                break;
                            case BSPIn_CSR2:

                                // Transmit FIFO size (resize fifo if needed)

                                tx_fifo_size = ((Registers[BSPIn_CSR2] & 0x3C00) >> 10) + 1;
                                if (tx_fifo_size > 10) tx_fifo_size = 1;
                                if (tx_fifo.size() > tx_fifo_size) tx_fifo.resize(tx_fifo_size);

                                // Manage DFIFO : Disable for the FIFO

                                if (Registers[BSPIn_CSR2] & 0x0001) {
                                    // Empty FIFO

                                    while (!tx_fifo.empty()) tx_fifo.pop_front();
                                    while (!rx_fifo.empty()) rx_fifo.pop_front();
                                    // RFE bits set to 0 => Receive FIFO = 1 word.

                                    Registers[BSPIn_CSR1] &= 0x0FFF;
                                    Registers[BSPIn_CSR1] |= 0x1000;
                                    rx_fifo_size = 1;
                                    // Reset the DFIFO bit

                                    Registers[BSPIn_CSR2] &= 0xFFFE;
                                }
                                break;
                            case BSPIn_CLK:
                                break;
                            default:
                                break;
                        }
                    }

                    template <unsigned int BUS_WIDTH, bool VERBOSE>
                    void
                    STR7_SPI<BUS_WIDTH, VERBOSE>::manage_interrupt() // Set interruption signals
                    {
                        sc_time delay = sc_time(0, SC_NS);
                        switch ((Registers[BSPIn_CSR2] >> 14)&0x0003) // read TIE
                        {
                            case 0:
                                break;
                            case 1:
                                if ((Registers[BSPIn_CSR2] & 0x0040) == 0x0040) // transmit FIFO empty enable
                                {
                                    //irq_payload->level = true;
                                    //    irq->b_transport(*irq_payload, delay);
                                }
                                break;
                            case 2:
                                if ((Registers[BSPIn_CSR2] & 0x0080) == 0x0080) // transmit FIFO underflow enable
                                {
                                    //irq_payload->level = true;
                                    //   irq->b_transport(*irq_payload, delay);
                                }
                                break;
                            case 3:
                                if ((Registers[BSPIn_CSR2] & 0x0100) == 0x0100) // transmit FIFO full enable
                                {
                                    //irq_payload->level = true;
                                    //     irq->b_transport(*irq_payload, delay);
                                }
                                break;
                            default:
                                break;
                        }
                        switch (((Registers[BSPIn_CSR1] >> 2)&0x0003)) // read RIE
                        {
                            case 0:
                                break;
                            case 1:
                                if ((Registers[BSPIn_CSR2] & 0x0008) == 0x0008) // receive FIFO not empty enable
                                {
                                    //irq_payload->level = true;
                                    //     irq->b_transport(*irq_payload, delay);
                                }
                                break;
                            case 2:
                                break;
                            case 3:
                                if ((Registers[BSPIn_CSR2] & 0x0010) == 0x0010) // receive FIFO full enable
                                {
                                    //irq_payload->level = true;
                                    //      irq->b_transport(*irq_payload, delay);
                                }
                                break;
                            default:
                                break;
                        }
                        if (((Registers[BSPIn_CSR1] & 0x0010) == 0x0010) && ((Registers[BSPIn_CSR2] & 0x0020) == 0x0020)) {
                            // irq_payload->level = true;
                            //  irq->b_transport(*irq_payload, delay);
                        }
                    }

                    template <unsigned int BUS_WIDTH, bool VERBOSE>
                    void
                    STR7_SPI<BUS_WIDTH, VERBOSE>::SPIHANDLER() {

                        while (1) {

                            cout << "+ toto " << sc_time_stamp() << endl;
                         //   wait(PERIODE_2000HZ, SC_NS);
                            cout << "  - toto " << sc_time_stamp() << endl;


                        }

                    }

                    /* START: verbose methods */

                    template <unsigned int BUS_WIDTH, bool VERBOSE>
                    bool
                    STR7_SPI<BUS_WIDTH, VERBOSE> ::
                    VerboseAll() {
                        return VERBOSE && verbose_all;
                    }

                    template <unsigned int BUS_WIDTH, bool VERBOSE>
                    bool
                    STR7_SPI<BUS_WIDTH, VERBOSE> ::
                    VerboseSetup() {
                        return VERBOSE && verbose_setup;
                    }

                    template <unsigned int BUS_WIDTH, bool VERBOSE>
                    bool
                    STR7_SPI<BUS_WIDTH, VERBOSE> ::
                    VerboseRun() {
                        return VERBOSE && verbose_run;
                    }

                    template <unsigned int BUS_WIDTH, bool VERBOSE>
                    bool
                    STR7_SPI<BUS_WIDTH, VERBOSE> ::
                    VerboseTLM() {
                        return VERBOSE && verbose_tlm;
                    }

                    /* END: verbose methods */







                } // end of namespace str7_spi
            } // end of namespace com
        } // end of namespace tlm2
    } // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_COM_STR7_SPI_STR7_SPI_TCC__


